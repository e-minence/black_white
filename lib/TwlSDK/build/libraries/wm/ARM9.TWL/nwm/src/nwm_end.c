/*---------------------------------------------------------------------------*
  Project:  TwlSDK - NWM - libraries
  File:     nwm_end.c

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
#include "nwm_common_private.h"
#include "nwm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         NWM_End

  Description:  NWM���C�u�����̏I���������s���B
                ARM9���̏I���݂̂��s�������֐��B

  Arguments:    None

  Returns:      NWMRetCode   -  �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/

NWMRetCode NWM_End(void)
{
    NWMRetCode result;
    OSIntrMode e;
    extern NWMArm9Buf *nwm9buf;
    extern u8 nwmInitialized;

    SDK_NULL_ASSERT(nwm9buf);
    
    // INITIALIZED�X�e�[�g�ȊO�ł͎��s�s��
    result = NWMi_CheckState(1, NWM_STATE_INITIALIZED);
    NWM_CHECK_RESULT(result);

    e = OS_DisableInterrupts();
    
    nwm9buf->status->state = NWM_STATE_NONE;  // ��O�I�ɂ����ł�������State���Z�b�g(�{����ARM7�̎d��)

    // FIFO�o�b�t�@�������݋��t���O���N���A
    NWMi_ClearFifoRecvFlag();
    
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WMW, NULL);
    nwm9buf = 0;
    
    // ����I��
    nwmInitialized = 0;
    (void)OS_RestoreInterrupts(e);
    return NWM_RETCODE_SUCCESS;
}

