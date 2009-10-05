/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - libraries
  File:     card_pullOut.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $

 *---------------------------------------------------------------------------*/


#include <nitro/card/rom.h>
#include <nitro/card/pullOut.h>

#include "card_rom.h"

//---- user callback for card pulled out
static CARDPulledOutCallback CARD_UserCallback;

//---- flag to be pulled out
static u32 CARDiSlotResetCount;
static BOOL CARDi_IsPulledOutFlag = FALSE;

static void CARDi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err);
static void CARDi_SendtoPxi(u32 data, u32 wait);

/*---------------------------------------------------------------------------*
  Name:         CARD_InitPulledOutCallback

  Description:  initialize callback setting

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_InitPulledOutCallback(void)
{
    PXI_Init();

    CARDiSlotResetCount = 0;
    CARDi_IsPulledOutFlag = FALSE;

    //---- set PXI callback
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_CARD, CARDi_PulledOutCallback);

    //---- init user callback
    CARD_UserCallback = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_PulledOutCallback

  Description:  callback to receive data from PXI

  Arguments:    tag  : tag from PXI (unused)
                data : data from PXI
                err  : error bit (unused)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CARDi_PulledOutCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag, err )

    u32     command = data & CARD_PXI_COMMAND_MASK;

    //---- receive message 'pulled out'
    if (command == CARD_PXI_COMMAND_PULLED_OUT)
    {
        if (CARDi_IsPulledOutFlag == FALSE)
        {
            BOOL    isTerminateImm = TRUE;

            CARDi_IsPulledOutFlag = TRUE;
            CARDi_NotifyEvent(CARD_EVENT_PULLEDOUT, NULL);

            //---- call user callback
            if (CARD_UserCallback)
            {
                isTerminateImm = CARD_UserCallback();
            }

            //---- terminate
            if (isTerminateImm)
            {
                CARD_TerminateForPulledOut();
            }
        }
    }
    else if (command == CARD_PXI_COMMAND_RESET_SLOT)
    {
        CARDiSlotResetCount += 1;
        CARDi_IsPulledOutFlag = FALSE;
        CARDi_NotifyEvent(CARD_EVENT_SLOTRESET, NULL);
    }
    else
    {
#ifndef SDK_FINALROM
        OS_TPanic("illegal card pxi command.");
#else
        OS_TPanic("");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_SetPulledOutCallback

  Description:  set user callback for being pulled out card

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_SetPulledOutCallback(CARDPulledOutCallback callback)
{
    CARD_UserCallback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         CARD_IsPulledOut

  Description:  return if have detected card pulled out

  Arguments:    None

  Returns:      TRUE if detected
 *---------------------------------------------------------------------------*/
BOOL CARD_IsPulledOut(void)
{
    return CARDi_IsPulledOutFlag;
}

//================================================================================
//         TERMINATION
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CARD_TerminateForPulledOut

  Description:  terminate for pulling out card
                send message to do termination to ARM7

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void CARD_TerminateForPulledOut(void)
{
    //---- if folding, power off
    if (PAD_DetectFold())
    {
        (void)PM_ForceToPowerOff();
        //---- �d���f�͕K����������̂ł����ɓ��B���邱�Ƃ͂Ȃ�
    }

    // �W�������Ă��Ȃ��Ƃ���ARM7���~�߂Ȃ��Ƃ����Ȃ��̂�Terminate�R�}���h�𑗂�B
#ifdef SDK_TWL
    if ( OS_IsRunOnTwl() )
    {
        // �I������
        // ARM7�ɏ������˗����Ă���ꍇ�̓R�[���o�b�N�֐����Ŋ�����҂���
        PMi_ExecutePostExitCallbackList();
    }
#endif // SDK_TWL
    //---- send 'TERMINATE' command to ARM7, and terminate itself immediately
    CARDi_SendtoPxi(CARD_PXI_COMMAND_TERMINATE, 1);

    //---- stop all dma
    MI_StopAllDma();
#ifdef SDK_TWL
	if ( OS_IsRunOnTwl() )
	{
		MI_StopAllNDma();
	}
#endif

    OS_Terminate();
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CheckPulledOutCore

  Description:  �J�[�h�������o�֐��̃��C������.
                �J�[�h�o�X�̓��b�N����Ă���K�v������.

  Arguments:    id            �J�[�h����ǂݏo���ꂽ ROM-ID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CheckPulledOutCore(u32 id)
{
    //---- card ID IPL had read
    vu32    iplCardID = *(vu32 *)(HW_BOOT_CHECK_INFO_BUF);
    //---- if card removal has been detected, we simulate PXI-notification from ARM7
    if (id != (u32)iplCardID)
    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        CARDi_PulledOutCallback(PXI_FIFO_TAG_CARD, CARD_PXI_COMMAND_PULLED_OUT, FALSE);
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckPulledOut

  Description:  get whether system has detected pulled out card
                by comparing IPL cardID with current cardID
                (notice that once card pulled out, IDs are different absolutely)

  Arguments:    None

  Returns:      TRUE if current cardID is equal to IPL cardID
 *---------------------------------------------------------------------------*/
void CARD_CheckPulledOut(void)
{
    CARDi_CheckPulledOutCore(CARDi_ReadRomID());
}

//================================================================================
//         SEND PXI COMMAND
//================================================================================
/*---------------------------------------------------------------------------*
  Name:         CARDi_SendtoPxi

  Description:  send data via PXI

  Arguments:    data : data to send

  Returns:      None
 *---------------------------------------------------------------------------*/
static void CARDi_SendtoPxi(u32 data, u32 wait)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_CARD, data, FALSE) != PXI_FIFO_SUCCESS)
    {
        SVC_WaitByLoop((s32)wait);
    }
}


/*---------------------------------------------------------------------------*
 * internal functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetSlotResetCount

  Description:  �X���b�g���đ}�����ꂽ�񐔂��擾�B
                �P��CARDi_ResetSlotStatus�֐����Ă΂ꂽ�񐔂�Ԃ��B

  Arguments:    None

  Returns:      �X���b�g���đ}�����ꂽ�񐔁B�����l��0�B
 *---------------------------------------------------------------------------*/
u32     CARDi_GetSlotResetCount(void)
{
    return CARDiSlotResetCount;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsPulledOutEx

  Description:  �X���b�g����J�[�h�����������ꂽ������B

  Arguments:    count        �O��m�F�����X���b�g�đ}���񐔁B
                             CARDi_GetSlotResetCount�֐��Ŏ擾����B

  Returns:      �w�肳�ꂽ�X���b�g�đ}���񐔂����݂������܂܂ŁA
                �����݃J�[�h������ԂłȂ����TRUE�B
 *---------------------------------------------------------------------------*/
BOOL    CARDi_IsPulledOutEx(u32 count)
{
    BOOL    result = FALSE;
    OSIntrMode  bak = OS_DisableInterrupts();
    {
        result = ((count == CARDi_GetSlotResetCount()) && !CARD_IsPulledOut());
    }
    (void)OS_RestoreInterrupts(bak);
    return result;
}
