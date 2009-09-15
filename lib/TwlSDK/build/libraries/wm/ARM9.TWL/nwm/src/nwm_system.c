/*---------------------------------------------------------------------------*
  Project:  TwlSDK - NWM - libraries
  File:     nwm_system.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include "nwm_arm9_private.h"
#include "nwm_common_private.h"

NWMArm9Buf *nwm9buf = NULL;
static PMSleepCallbackInfo sleepCbInfo;  //PM �ɓo�^����X���[�v�R�[���o�b�N���

#ifdef NWM_SUPPORT_HWRESET
PMExitCallbackInfo hwResetCbInfo;
#endif

static void NwmSleepCallback(void *);

/*---------------------------------------------------------------------------*
  Name:         NWMi_GetSystemWork

  Description:  New WM���C�u���������Ŏg�p����o�b�t�@�̐擪�ւ̃|�C���^���擾����B

  Arguments:    None.

  Returns:      NWMArm9Buf*  -   �������[�N�o�b�t�@�ւ̃|�C���^��Ԃ��B
 *---------------------------------------------------------------------------*/
NWMArm9Buf *NWMi_GetSystemWork(void)
{
//    SDK_NULL_ASSERT(nwm9buf);
    return nwm9buf;
}


/*---------------------------------------------------------------------------*
  Name:         NWMi_ClearFifoRecvFlag

  Description:  New WM7 ����̃R�[���o�b�N�ɗp����ꂽFIFO�f�[�^���Q�Ƃ��I�������Ƃ�
                New WM7 �ɒʒm����B
                New WM7 �ɂ����ăR�[���o�b�N��FIFO��p����ہA���̃t���O���A�����b�N
                �����̂�҂��Ă��玟�̃R�[���o�b�N��ҏW����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

void NWMi_ClearFifoRecvFlag(void)
{
    NWMArm7Buf *w7b;

    if (nwm9buf == NULL) {
        return;
    }
    
    w7b = nwm9buf->NWM7;

    // ARM7���X�V�����l��ǂ݂ɍs�����߁AARM9���̃L���b�V���𖳌�������B
    DC_InvalidateRange(&w7b->callbackSyncFlag, 1);
    
    if (w7b->callbackSyncFlag & NWM_EXCEPTION_CB_MASK)
    {
        // CB�r���t���O��������
        w7b->callbackSyncFlag &= ~NWM_EXCEPTION_CB_MASK;
        // store cache immediately
        DC_StoreRange(&w7b->callbackSyncFlag, 1);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NWMi_ReceiveFifo9

  Description:  FIFO����� New WM7 ����̃R�[���o�b�N����M����B

  Arguments:    tag          - ���g�p
                fifo_buf_adr - �R�[���o�b�N�̃p�����[�^�Q�ւ̃|�C���^
                err          - ���g�p

  Returns:      None.
 *---------------------------------------------------------------------------*/

void NWMi_ReceiveFifo9(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err)
{
    #pragma unused( tag )
    NWMCallback *pCallback = (NWMCallback *)fifo_buf_adr;
    NWMCallbackFunc callback;
    NWMArm9Buf *w9b = nwm9buf;

    if (w9b == NULL) {
        return;
    }
    
    if (err) {
        NWM_WARNING("NWM9 FIFO receive error. :%d\n", err);
        return;
    }

    if (!fifo_buf_adr) {
        NWM_WARNING("NWM9 FIFO receive error.(NULL address) :%d\n", err);
        return;
    }

    DC_InvalidateRange(w9b->fifo7to9, NWM_APIFIFO_BUF_SIZE);
    DC_InvalidateRange(w9b->status, NWM_STATUS_BUF_SIZE);
    if ((u32)pCallback != (u32)(w9b->fifo7to9))
    {
        DC_InvalidateRange(pCallback, NWM_APIFIFO_BUF_SIZE);
    }

    NWM_DPRINTF("APIID%04x\n", pCallback->apiid);

    // NWM_LoadDevice �����s�����ꍇ�AWM_UnloadDevice �����������ꍇ�ɂ̓X���[�v�R�[���o�b�N���폜
    if( (pCallback->apiid == NWM_APIID_LOAD_DEVICE   && pCallback->retcode != NWM_RETCODE_SUCCESS) ||
        (pCallback->apiid == NWM_APIID_UNLOAD_DEVICE && pCallback->retcode == NWM_RETCODE_SUCCESS) ||
         pCallback->apiid == NWM_APIID_INSTALL_FIRMWARE )
    {
        NWMi_DeleteSleepCallback();
    }

    // apiid�ɉ������R�[���o�b�N���� (�R�[���o�b�N�����ݒ�(NULL)�Ȃ牽�����Ȃ�)
    {
        NWMSendFrameCallback *pSfcb = (NWMSendFrameCallback *)pCallback;
        NWMRetCode result = NWM_RETCODE_FAILED;
        
        if (pCallback->apiid == NWM_APIID_SEND_FRAME)
        {
            if (pCallback->retcode != NWM_RETCODE_INDICATION && NULL != pSfcb->callback)
            {
                NWM_DPRINTF("Execute CallbackFunc APIID 0x%04x\n", pCallback->apiid);
                (pSfcb->callback)((void *)pCallback);
            }

        }
    }

    // in the case of several special APIs
    if (pCallback->apiid == NWM_APIID_SEND_FRAME) {
        NWMSendFrameCallback *pSfcb = (NWMSendFrameCallback *)pCallback;

    } else {

#ifdef NWM_SUPPORT_HWRESET
        if (pCallback->apiid == NWM_APIID_UNLOAD_DEVICE
            || pCallback->apiid == NWM_APIID_INSTALL_FIRMWARE
            || (pCallback->apiid == NWM_APIID_LOAD_DEVICE
                && pCallback->retcode != NWM_RETCODE_SUCCESS)) {
            // delete HW reset callback
            PM_DeletePostExitCallback(&hwResetCbInfo);
        }
#endif
        
        // other APIs
        callback = w9b->callbackTable[pCallback->apiid];

        // in case START_SCAN callback, scan buffer cache must be invalidated.
        if (pCallback->apiid == NWM_APIID_START_SCAN) {
            NWMStartScanCallback *psscb = (NWMStartScanCallback *)pCallback;

            DC_InvalidateRange(psscb->bssDesc[0], psscb->allBssDescSize);
        }

        if (NULL != callback)
        {
            NWM_DPRINTF("Execute CallbackFunc APIID 0x%04x\n", pCallback->apiid);
            (callback) ((void *)pCallback);
        }

    }

    MI_CpuClear8(pCallback, NWM_APIFIFO_BUF_SIZE);
    DC_StoreRange(pCallback, NWM_APIFIFO_BUF_SIZE);
    if (w9b) { // nwm might be terminated after callback
        NWMi_ClearFifoRecvFlag();
    }

}

/*---------------------------------------------------------------------------*
  Name:         NWM_GetState

  Description:  NWM���C�u�����̓�����Ԃ��m�F����B
                NWMi_CheckState �Ǝ����ړI�Ŏg�p����邪�p�r���傫���قȂ�̂�
                �ʁX�ɗp�ӂ��Ă���B

  Arguments:    None.

  Returns:      u16  -   NWM �̓�����Ԃ���������
 *---------------------------------------------------------------------------*/
u16 NWM_GetState(void)
{
    NWMStatus*  nwmStatus;
    NWMArm9Buf* sys = NWMi_GetSystemWork();
    u16 state = NWM_STATE_NONE;

    if (sys) {
        nwmStatus = sys->status;
        DC_InvalidateRange(nwmStatus, 2);
        state = nwmStatus->state;
    }

    return state;
}

/*---------------------------------------------------------------------------*
  Name:         NWMi_CheckState

  Description:  NWM���C�u�����̓�����Ԃ��m�F����B
                ��������Ԃ�����WMState�^�̃p�����[�^��񋓂��Ďw�肷��B

  Arguments:    paramNum    -   ���z�����̐��B
                ...         -   ���z�����B

  Returns:      int -   NWM_RETCODE_*�^�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
NWMRetCode NWMi_CheckState(s32 paramNum, ...)
{
    NWMRetCode result;
    u16     now;
    u32     temp;
    va_list vlist;
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    SDK_NULL_ASSERT(sys);

    // �������ς݂��m�F
    result = NWMi_CheckInitialized();
    NWM_CHECK_RESULT(result);

    // ���X�e�[�g�擾
    DC_InvalidateRange(&(sys->status->state), 2);
    now = sys->status->state;

    // ��v�m�F
    result = NWM_RETCODE_ILLEGAL_STATE;
    va_start(vlist, paramNum);
    for (; paramNum; paramNum--)
    {
        temp = va_arg(vlist, u32);
        if (temp == now)
        {
            result = NWM_RETCODE_SUCCESS;
        }
    }
    va_end(vlist);

    if (result == NWM_RETCODE_ILLEGAL_STATE)
    {
        NWM_WARNING("New WM state is \"0x%04x\" now. So can't execute request.\n", now);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         NWM_GetAllowedChannel

  Description:  �V�����Ŏg�p�\�� IEEE 802.11b/g �̃`�����l�����X�g���擾����B

  Arguments:    None.

  Returns:      u16  -   WM �Ɠ���t�H�[�}�b�g�̃`�����l���r�b�g�}�b�v��Ԃ��B
 *---------------------------------------------------------------------------*/
u16 NWM_GetAllowedChannel(void)
{
    NWMArm9Buf *sys = NWMi_GetSystemWork();

    return sys->status->allowedChannel;
}

/*---------------------------------------------------------------------------*
  Name:         NWM_CalcLinkLevel

  Description:  nwm_common_private.h �ɂĒ�`����Ă���臒l���烊���N���x�����Z�o

  Arguments:    s16          -   Atheros �h���C�o����ʒm�����RSSI�l

  Returns:      u16          -   WM �Ɠ��l�̃����N���x��
 *---------------------------------------------------------------------------*/
u16 NWM_CalcLinkLevel(s16 rssi)
{

/* [TODO] ���샂�[�h�ɂ���Đ؂�ւ���K�v������? �K�v�Ȃ�����������߂�K�v���� */

    if(1) /* Infra Structure Mode */
    {
        if(rssi < NWM_RSSI_INFRA_LINK_LEVEL_1)
        {
            return WM_LINK_LEVEL_0;
        }
        if(rssi < NWM_RSSI_INFRA_LINK_LEVEL_2)
        {
            return WM_LINK_LEVEL_1;
        }
        if(rssi < NWM_RSSI_INFRA_LINK_LEVEL_3)
        {
            return WM_LINK_LEVEL_2;
        }

        return WM_LINK_LEVEL_3;
    }
    else if(0) /*Ad Hoc Mode*/
    {
        if(rssi < NWM_RSSI_ADHOC_LINK_LEVEL_1)
        {
            return WM_LINK_LEVEL_0;
        }
        if(rssi < NWM_RSSI_ADHOC_LINK_LEVEL_2)
        {
            return WM_LINK_LEVEL_1;
        }
        if(rssi < NWM_RSSI_ADHOC_LINK_LEVEL_3)
        {
            return WM_LINK_LEVEL_2;
        }
        return WM_LINK_LEVEL_3;
    }

}

/*---------------------------------------------------------------------------*
  Name:         NWM_GetDispersionScanPeriod

  Description:  STA �Ƃ��� AP �������� DS �e�@��T������ۂɐݒ肷�ׂ��T�����x���Ԃ��擾����B

  Arguments:    u16 scanType - �X�L�����^�C�v�ANWM_SCANTYPE_PASSIVE or NWM_SCANTYPE_ACTIVE
  
  Returns:      u16 -   �ݒ肷�ׂ��T�����x����(ms)�B
 *---------------------------------------------------------------------------*/
u16 NWM_GetDispersionScanPeriod(u16 scanType)
{
    u8      mac[6];
    u16     ret;
    s32     i;

    OS_GetMacAddress(mac);
    for (i = 0, ret = 0; i < 6; i++)
    {
        ret += mac[i];
    }
    ret += OS_GetVBlankCount();
    ret *= 13;
    
    if( scanType == NWM_SCANTYPE_ACTIVE )
    {
        ret = (u16)(NWM_DEFAULT_ACTIVE_SCAN_PERIOD + (ret % 10));
    }
    else /* ���m�̃X�L�����^�C�v�̓p�b�V�u�X�L�����Ɠ����Ɉ��� */
    {
        ret = (u16)(NWM_DEFAULT_PASSIVE_SCAN_PERIOD + (ret % 10));
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         NWMi_RegisterSleepCallback

  Description:  �X���[�v���[�h�ֈڍs���Ɏ��s�����R�[���o�b�N�֐���o�^����
  
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NWMi_RegisterSleepCallback(void)
{
    PM_SetSleepCallbackInfo(&sleepCbInfo, NwmSleepCallback, NULL);
    PMi_InsertPreSleepCallbackEx(&sleepCbInfo, PM_CALLBACK_PRIORITY_NWM );
}

/*---------------------------------------------------------------------------*
  Name:         NWMi_DeleteSleepCallback

  Description:  �X���[�v���[�h�ֈڍs���Ɏ��s�����R�[���o�b�N�֐����폜����
  
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NWMi_DeleteSleepCallback(void)
{
    PM_DeletePreSleepCallback( &sleepCbInfo );
}

/*---------------------------------------------------------------------------*
  Name:         NwmSleepCallback

  Description:  �����ʐM���ɃX���[�v���[�h�ɓ���Ȃ��悤�ɂ���B
  
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void NwmSleepCallback(void *)
{
    /* ---------------------------------------------- *
     * �v���O���~���O�K�C�h���C���̒ʂ�A             *
     * �����ʐM���� OS_GoSleepMode() �����s���邱�Ƃ� *
     * �֎~����Ă��܂��B                             *
     * ---------------------------------------------- */
    OS_Panic("Could not sleep during wireless communications.");
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
