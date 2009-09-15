/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - libraries
  File:     wm_common.c

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

#include "wm_common.h"

/*---------------------------------------------------------------------------*
  Name:         WM_CheckInitialized

  Description:  WM ���g�p����Ă��邩���`�F�b�N����B

  Arguments:    None.

  Returns:      int -   WM_ERRCODE_*�^�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_CheckInitialized(void)
{
		return WMi_CheckInitialized();
}

/*---------------------------------------------------------------------------*
  Name:         NWM_CheckInitialized

  Description:  New WM ���g�p����Ă��邩���`�F�b�N����B

  Arguments:    None.

  Returns:      int -   NWM_ERRCODE_*�^�̏������ʂ�Ԃ��B�B
 *---------------------------------------------------------------------------*/
NWMRetCode NWM_CheckInitialized(void)
{
#ifdef SDK_TWL
	if(OS_IsRunOnTwl()) //TWL ��œ��삵�Ă���ꍇ�̂݁A���������C�u�����̏�Ԃ��m�F
	{
		return NWMi_CheckInitialized();
	}
#endif
    //NITRO ��ł́A�V�����`�b�v�����݂��Ȃ��̂ŁA��ӂ� NWM_RETCODE_SUCCESS ��Ԃ��B
	return NWM_RETCODE_ILLEGAL_STATE;
}