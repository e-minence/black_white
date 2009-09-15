/*---------------------------------------------------------------------------*
  Project:  TwlSDK - CARD - include
  File:     pullOut.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $

 *---------------------------------------------------------------------------*/
#ifndef NITRO_CARD_PULLOUT_H_
#define NITRO_CARD_PULLOUT_H_


#include <nitro/types.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*/
/* declarations */

//---- callback type for card pulled out
typedef BOOL (*CARDPulledOutCallback) (void);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         CARD_InitPulledOutCallback

  Description:  set system callback for being pulled out card

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARD_InitPulledOutCallback(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_IsPulledOut

  Description:  return whether card is pulled out

  Arguments:    None

  Returns:      TRUE if detect pulled out
 *---------------------------------------------------------------------------*/
BOOL    CARD_IsPulledOut(void);

#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         CARD_SetPulledOutCallback

  Description:  set user callback for being pulled out card

  Arguments:    callback : callback

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARD_SetPulledOutCallback(CARDPulledOutCallback callback);

/*---------------------------------------------------------------------------*
  Name:         CARD_TerminateForPulledOut

  Description:  terminate for pulling out card
                send message to do termination to ARM7

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARD_TerminateForPulledOut(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_PulledOutCallbackProc

  Description:  callback for card pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARD_PulledOutCallbackProc(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckPulledOut

  Description:  get whether system has detected pulled out card
                by comparing IPL cardID with current cardID
                (notice that once card pulled out, IDs are different absolutely)

  Arguments:    None

  Returns:      TRUE if current cardID is equal to IPL cardID
 *---------------------------------------------------------------------------*/
void    CARD_CheckPulledOut(void);

#endif

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         CARD_CheckPullOut_Polling

  Description:  polling that card is pulled out

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARD_CheckPullOut_Polling(void);

#endif


/*---------------------------------------------------------------------------*
 * internal functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_ResetSlotStatus

  Description:  �X���b�g���đ}�����ꂽ���Ƃɂ��J�[�h������Ԃ��������B
                ���̊֐��͖{�̊J���p�̓����֐��ł���A
                �ʏ�̃A�v���P�[�V�������P�ɂ�����Ăяo���������ł�
                �X���b�g�̃n�[�h�E�F�A��Ԃ͉�������������Ȃ��_�ɒ��ӁB

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    CARDi_ResetSlotStatus(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetSlotResetCount

  Description:  �X���b�g���đ}�����ꂽ�񐔂��擾�B
                �P��CARDi_ResetSlotStatus�֐����Ă΂ꂽ�񐔂�Ԃ��B

  Arguments:    None

  Returns:      �X���b�g���đ}�����ꂽ�񐔁B�����l��0�B
 *---------------------------------------------------------------------------*/
u32     CARDi_GetSlotResetCount(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsPulledOutEx

  Description:  �X���b�g����J�[�h�����������ꂽ������B

  Arguments:    count        �O��m�F�����X���b�g�đ}���񐔁B
                             CARDi_GetSlotResetCount�֐��Ŏ擾����B

  Returns:      �w�肳�ꂽ�X���b�g�đ}���񐔂����݂������܂܂ŁA
                �����݃J�[�h������ԂłȂ����TRUE�B
 *---------------------------------------------------------------------------*/
BOOL    CARDi_IsPulledOutEx(u32 count);




#ifdef __cplusplus
} // extern "C"
#endif


#endif // NITRO_CARD_PULLOUT_H_
