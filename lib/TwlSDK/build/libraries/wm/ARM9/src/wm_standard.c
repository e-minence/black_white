/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - libraries
  File:     wm_standard.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-19#$
  $Rev: 9342 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static BOOL WmCheckParentParameter(const WMParentParam *param);


/*---------------------------------------------------------------------------*
  Name:         WM_Enable

  Description:  �����n�[�h�E�F�A���g�p�\�ȏ�Ԃɂ���B
                ������Ԃ�READY��Ԃ���STOP��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_Enable(WMCallbackFunc callback)
{
    return WMi_EnableEx(callback, 0);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EnableForListening

  Description:  �����n�[�h�E�F�A���g�p�\�ȏ�Ԃɂ���B
                ������Ԃ�READY��Ԃ���STOP��ԂɑJ�ڂ����B
                �d�g�𑗏o����I�y���[�V�����͂ł��Ȃ��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                blink       -   LED ��_�ł����邩

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_EnableForListening(WMCallbackFunc callback, BOOL blink)
{
    u32 miscFlags = WM_MISC_FLAG_LISTEN_ONLY;

    if (!blink)
    {
        miscFlags |= WM_MISC_FLAG_NO_BLINK;
    }

    return WMi_EnableEx(callback, miscFlags);
}

/*---------------------------------------------------------------------------*
  Name:         WMi_EnableEx

  Description:  �����n�[�h�E�F�A���g�p�\�ȏ�Ԃɂ���B
                ������Ԃ�READY��Ԃ���STOP��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                miscFlags   -   ���������̃t���O

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WMi_EnableEx(WMCallbackFunc callback, u32 miscFlags)
{
    WMErrCode result;

    // READY�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_READY);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_ENABLE, callback);
    
    // �ʐM���̃X���[�v���֎~���邽�߂̃R�[���o�b�N��o�^
    WMi_RegisterSleepCallback();

    // ARM7��FIFO�Œʒm
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        result = WMi_SendCommand(WM_APIID_ENABLE, 4,
                                 (u32)(p->WM7), (u32)(p->status), (u32)(p->fifo7to9), miscFlags);
        WM_CHECK_RESULT(result);
    }

    // ����I��
    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Disable

  Description:  �����n�[�h�E�F�A���g�p�֎~��Ԃɂ���B
                ������Ԃ�STOP��Ԃ���READY��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disable(WMCallbackFunc callback)
{
    WMErrCode result;

    // STOP�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_STOP);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_DISABLE, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_DISABLE, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOn

  Description:  �����n�[�h�E�F�A���N������B
                ������Ԃ�STOP��Ԃ���IDLE��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOn(WMCallbackFunc callback)
{
    WMErrCode result;

    // STOP�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_STOP);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_POWER_ON, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_POWER_ON, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOff

  Description:  �����n�[�h�E�F�A���V���b�g�_�E������B
                ������Ԃ�IDLE��Ԃ���STOP��ԂɑJ�ڂ����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOff(WMCallbackFunc callback)
{
    WMErrCode result;

    // IDLE�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_POWER_OFF, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_POWER_OFF, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Initialize

  Description:  WM�������������s���B

  Arguments:    wmSysBuf    -   �Ăяo�����ɂ���Ċm�ۂ��ꂽ�o�b�t�@�ւ̃|�C���^�B
                                �o�b�t�@�̃T�C�Y��WM_SYSTEM_BUF_SIZE�����K�v�B
                callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                dmaNo       -   WM���g�p����DMA�ԍ��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_Initialize(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo)
{
    return WMi_InitializeEx(wmSysBuf, callback, dmaNo, 0);
}

/*---------------------------------------------------------------------------*
  Name:         WM_InitializeForListening

  Description:  WM�������������s���B
                �d�g�𑗏o����I�y���[�V�����͂ł��Ȃ��B

  Arguments:    wmSysBuf    -   �Ăяo�����ɂ���Ċm�ۂ��ꂽ�o�b�t�@�ւ̃|�C���^�B
                                �o�b�t�@�̃T�C�Y��WM_SYSTEM_BUF_SIZE�����K�v�B
                callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                dmaNo       -   WM���g�p����DMA�ԍ��B
                blink       -   LED ��_�ł����邩�ǂ���

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_InitializeForListening(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo, BOOL blink)
{
    u32 miscFlags = WM_MISC_FLAG_LISTEN_ONLY;

    if (!blink)
    {
        miscFlags |= WM_MISC_FLAG_NO_BLINK;
    }

    return WMi_InitializeEx(wmSysBuf, callback, dmaNo, miscFlags);
}

/*---------------------------------------------------------------------------*
  Name:         WMi_InitializeEx

  Description:  WM�������������s���B

  Arguments:    wmSysBuf    -   �Ăяo�����ɂ���Ċm�ۂ��ꂽ�o�b�t�@�ւ̃|�C���^�B
                                �o�b�t�@�̃T�C�Y��WM_SYSTEM_BUF_SIZE�����K�v�B
                callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                dmaNo       -   WM���g�p����DMA�ԍ��B
                miscFlags   -   ���������̃t���O

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WMi_InitializeEx(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo, u32 miscFlags)
{
    WMErrCode result;

    // �����������s
    result = WM_Init(wmSysBuf, dmaNo);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_INITIALIZE, callback);

    // �ʐM���̃X���[�v���֎~���邽�߂̃R�[���o�b�N��o�^
    WMi_RegisterSleepCallback();

    // ARM7��FIFO�Œʒm
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        result = WMi_SendCommand(WM_APIID_INITIALIZE, 4,
                                 (u32)(p->WM7), (u32)(p->status), (u32)(p->fifo7to9), miscFlags);
        WM_CHECK_RESULT(result);
    }

    // ����I��
    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Reset

  Description:  �������C�u���������Z�b�g���A����������̏�Ԃɖ߂��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_Reset(WMCallbackFunc callback)
{
    WMErrCode result;

    // �����n�[�h�N���ς݂��m�F
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_RESET, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_RESET, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_End

  Description:  �������C�u�������I������B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_End(WMCallbackFunc callback)
{
    WMErrCode result;

    // IDLE�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_END, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_END, 0);
    WM_CHECK_RESULT(result);

    // ARM9��WM���C�u�����̏I�������̓R�[���o�b�N���ōs��

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetParentParameter

  Description:  �e�@����ݒ肷��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                pparaBuf    -   �e�@���������\���̂ւ̃|�C���^�B
                                pparaBuf�y��pparaBuf->userGameInfo�̎��̂͋����I��
                                �L���b�V���X�g�A�����_�ɒ��ӁB

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetParentParameter(WMCallbackFunc callback, const WMParentParam *pparaBuf)
{
    WMErrCode result;

    // IDLE�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);
    
    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return WM_ERRCODE_WM_DISABLE;
        }
    }
    #endif
    
    // �p�����[�^�`�F�b�N
    if (pparaBuf == NULL)
    {
        WM_WARNING("Parameter \"pparaBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)pparaBuf & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"pparaBuf\" is not 32-byte aligned.\n");
    }
    if (pparaBuf->userGameInfoLength > 0)
    {
        if (pparaBuf->userGameInfo == NULL)
        {
            WM_WARNING("Parameter \"pparaBuf->userGameInfo\" must not be NULL.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if ((u32)(pparaBuf->userGameInfo) & 0x01f)
        {
            // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
            WM_WARNING("Parameter \"pparaBuf->userGameInfo\" is not 32-byte aligned.\n");
        }
    }

    // �]���f�[�^�ő咷���`�F�b�N
    if ((pparaBuf->parentMaxSize +
         (pparaBuf->KS_Flag ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) >
         WM_SIZE_MP_DATA_MAX)
        || (pparaBuf->childMaxSize +
            (pparaBuf->KS_Flag ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) >
            WM_SIZE_MP_DATA_MAX))
    {
        WM_WARNING("Transfer data size is over %d byte.\n", WM_SIZE_MP_DATA_MAX);
        return WM_ERRCODE_INVALID_PARAM;
    }
    (void)WmCheckParentParameter(pparaBuf);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_SET_P_PARAM, callback);

    // �w��o�b�t�@�̃L���b�V���������o��
    DC_StoreRange((void *)pparaBuf, WM_PARENT_PARAM_SIZE);
    if (pparaBuf->userGameInfoLength > 0)
    {
        DC_StoreRange(pparaBuf->userGameInfo, pparaBuf->userGameInfoLength);
    }

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_SET_P_PARAM, 1, (u32)pparaBuf);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WmCheckParentParameter

  Description:  �e�@�ݒ�p�����[�^���K�C�h���C���Ɏ�����鋖�e�͈͓����ǂ���
                ���`�F�b�N����f�o�b�O�p�֐��B

  Arguments:    param   -   �`�F�b�N����e�@�ݒ�p�����[�^�ւ̃|�C���^�B

  Returns:      BOOL    -   ���Ȃ��ꍇ��TRUE���A���e�ł��Ȃ��ݒ�l�̏ꍇ��
                            FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL WmCheckParentParameter(const WMParentParam *param)
{
    // ���[�U�[�Q�[������ 112 �o�C�g�܂�
    if (param->userGameInfoLength > WM_SIZE_USER_GAMEINFO)
    {
        OS_TWarning("User gameInfo length must be less than %d .\n", WM_SIZE_USER_GAMEINFO);
        return FALSE;
    }
    // �r�[�R�����M�Ԋu�� 10 �` 1000
    if ((param->beaconPeriod < 10) || (param->beaconPeriod > 1000))
    {
        OS_TWarning("Beacon send period must be between 10 and 1000 .\n");
        return FALSE;
    }
    // �ڑ��`�����l���� 1 �` 14
    if ((param->channel < 1) || (param->channel > 14))
    {
        OS_TWarning("Channel must be between 1 and 14 .\n");
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_StartParentEx

  Description:  �e�@�Ƃ��ĒʐM���J�n����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                powerSave   -   �ȓd�̓��[�h���g�p����ꍇ��TRUE�A���Ȃ��ꍇ��FALSE�B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartParentEx(WMCallbackFunc callback, BOOL powerSave)
{
    WMErrCode result;

    // IDLE�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return WM_ERRCODE_WM_DISABLE;
        }
    }
    #endif
    
    {
        WMArm9Buf *w9b = WMi_GetSystemWork();
#ifdef WM_DEBUG
        if (w9b->connectedAidBitmap != 0)
        {
            WM_DPRINTF("Warning: connectedAidBitmap should be 0, but %04x",
                       w9b->connectedAidBitmap);
        }
#endif
        w9b->myAid = 0;
        w9b->connectedAidBitmap = 0;
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_START_PARENT, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_START_PARENT, 1, (u32)powerSave);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartParent

  Description:  �e�@�Ƃ��ĒʐM���J�n����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartParent(WMCallbackFunc callback)
{
    return WMi_StartParentEx(callback, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndParent

  Description:  �e�@�Ƃ��Ă̒ʐM���~����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndParent(WMCallbackFunc callback)
{
    WMErrCode result;

    // PARENT�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_PARENT);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_END_PARENT, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_END_PARENT, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartScan

  Description:  �q�@�Ƃ��Đe�@�̃X�L�������J�n����B
                ��x�̌Ăяo���ň��̐e�@�����擾����B
                WM_EndScan���Ăяo�����ɘA�����ČĂяo���Ă��ǂ��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                param       -   �X�L�������������\���̂ւ̃|�C���^�B
                                �X�L�������ʂ̏���param->scanBuf��ARM7�����ڏ����o��
                                �̂ŁA�L���b�V�����C���ɍ��킹�Ă����K�v������B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScan(WMCallbackFunc callback, const WMScanParam *param)
{
    WMErrCode result;

    // IDLE CALSS1 SCAN �X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckStateEx(3, WM_STATE_IDLE, WM_STATE_CLASS1, WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return WM_ERRCODE_WM_DISABLE;
        }
    }
    #endif
    
    // �p�����[�^�`�F�b�N
    if (param == NULL)
    {
        WM_WARNING("Parameter \"param\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBuf == NULL)
    {
        WM_WARNING("Parameter \"param->scanBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((param->channel < 1) || (param->channel > 14))
    {
        WM_WARNING("Parameter \"param->channel\" must be between 1 and 14.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)(param->scanBuf) & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"param->scanBuf\" is not 32-byte aligned.\n");
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_START_SCAN, callback);

    // ARM7��FIFO�Œʒm
    {
        WMStartScanReq Req;

        Req.apiid = WM_APIID_START_SCAN;
        Req.channel = param->channel;
        Req.scanBuf = param->scanBuf;
        Req.maxChannelTime = param->maxChannelTime;
        Req.bssid[0] = param->bssid[0];
        Req.bssid[1] = param->bssid[1];
        Req.bssid[2] = param->bssid[2];
        Req.bssid[3] = param->bssid[3];
        Req.bssid[4] = param->bssid[4];
        Req.bssid[5] = param->bssid[5];
        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartScanEx

  Description:  �q�@�Ƃ��Đe�@�̃X�L�������J�n����B
                ��x�̌Ăяo���ŕ�����̐e�@�����擾����B
                WM_EndScan���Ăяo�����ɘA�����ČĂяo���Ă��ǂ��B

  Arguments:    callback - �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                param    - �X�L�������������\���̂ւ̃|�C���^�B
                           �X�L�������ʂ̏���param->scanBuf��ARM7�����ڏ����o��
                           �̂ŁA�L���b�V�����C���ɍ��킹�Ă����K�v������B

  Returns:      int      - WM_ERRCODE_*�^�̏������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScanEx(WMCallbackFunc callback, const WMScanExParam *param)
{
    WMErrCode result;

    // IDLE CALSS1 SCAN �X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckStateEx(3, WM_STATE_IDLE, WM_STATE_CLASS1, WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return WM_ERRCODE_WM_DISABLE;
        }
    }
    #endif
    
    // �p�����[�^�`�F�b�N
    if (param == NULL)
    {
        WM_WARNING("Parameter \"param\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBuf == NULL)
    {
        WM_WARNING("Parameter \"param->scanBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBufSize > WM_SIZE_SCAN_EX_BUF)
    {
        WM_WARNING
            ("Parameter \"param->scanBufSize\" must be less than or equal to WM_SIZE_SCAN_EX_BUF.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)(param->scanBuf) & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"param->scanBuf\" is not 32-byte aligned.\n");
    }
    if (param->ssidLength > WM_SIZE_SSID)
    {
        WM_WARNING("Parameter \"param->ssidLength\" must be less than or equal to WM_SIZE_SSID.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanType != WM_SCANTYPE_ACTIVE && param->scanType != WM_SCANTYPE_PASSIVE
        && param->scanType != WM_SCANTYPE_ACTIVE_CUSTOM
        && param->scanType != WM_SCANTYPE_PASSIVE_CUSTOM)
    {
        WM_WARNING
            ("Parameter \"param->scanType\" must be WM_SCANTYPE_PASSIVE or WM_SCANTYPE_ACTIVE.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((param->scanType == WM_SCANTYPE_ACTIVE_CUSTOM
         || param->scanType == WM_SCANTYPE_PASSIVE_CUSTOM) && param->ssidMatchLength > WM_SIZE_SSID)
    {
        WM_WARNING
            ("Parameter \"param->ssidMatchLength\" must be less than or equal to WM_SIZE_SSID.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_START_SCAN_EX, callback);

    // ARM7��FIFO�Œʒm
    {
        WMStartScanExReq Req;

        Req.apiid = WM_APIID_START_SCAN_EX;
        Req.channelList = param->channelList;
        Req.scanBuf = param->scanBuf;
        Req.scanBufSize = param->scanBufSize;
        Req.maxChannelTime = param->maxChannelTime;
        MI_CpuCopy8(param->bssid, Req.bssid, WM_SIZE_MACADDR);
        Req.scanType = param->scanType;
        Req.ssidMatchLength = param->ssidMatchLength;
        Req.ssidLength = param->ssidLength;
        MI_CpuCopy8(param->ssid, Req.ssid, WM_SIZE_SSID);

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndScan

  Description:  �q�@�Ƃ��ẴX�L�����������~����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndScan(WMCallbackFunc callback)
{
    WMErrCode result;

    // SCAN�X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_END_SCAN, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_END_SCAN, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartConnectEx

  Description:  �q�@�Ƃ��Đe�@�ւ̐ڑ����J�n����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                pInfo       -   �ڑ�����e�@�̏��B
                                WM_StartScan�ɂĎ擾�����\���̂��w�肷��B
                                ���̍\���̂̎��̂͋����I�ɃL���b�V���X�g�A
                                �����_�ɒ��ӁB
                ssid        -   �e�@�ɒʒm����q�@���(24Byte(WM_SIZE_CHILD_SSID)�Œ�T�C�Y)
                powerSave   -   �ȓd�̓��[�h���g�p����ꍇ��TRUE�A���Ȃ��ꍇ��FALSE�B
                authMode    -   Authentication�̃��[�h�I���B
                                  WM_AUTHMODE_OPEN_SYSTEM : OPEN SYSTEM���[�h
                                  WM_AUTHMODE_SHARED_KEY  : SHARED KEY ���[�h

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartConnectEx(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid,
                  BOOL powerSave, const u16 authMode)
{
    WMErrCode result;

    // IDLE �X�e�[�g�ȊO�ł͎��s�s��
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return WM_ERRCODE_WM_DISABLE;
        }
    }
    #endif
    
    // �p�����[�^�`�F�b�N
    if (pInfo == NULL)
    {
        WM_WARNING("Parameter \"pInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)pInfo & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"pInfo\" is not 32-byte aligned.\n");
    }
    if ((authMode != WM_AUTHMODE_OPEN_SYSTEM) && (authMode != WM_AUTHMODE_SHARED_KEY))
    {
        WM_WARNING
            ("Parameter \"authMode\" must be WM_AUTHMODE_OPEN_SYSTEM or WM_AUTHMODE_SHARED_KEY.\n");
    }

    // �w��o�b�t�@�̃L���b�V���������o��
    DC_StoreRange((void *)pInfo, (u32)(pInfo->length * 2));

    {
        WMArm9Buf *w9b = WMi_GetSystemWork();
#ifdef WM_DEBUG
        if (w9b->connectedAidBitmap != 0)
        {
            WM_DPRINTF("Warning: connectedAidBitmap should be 0, but %04x",
                       w9b->connectedAidBitmap);
        }
#endif
        w9b->myAid = 0;
        w9b->connectedAidBitmap = 0;
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_START_CONNECT, callback);

    // ARM7��FIFO�Œʒm
    {
        WMStartConnectReq Req;

        Req.apiid = WM_APIID_START_CONNECT;
        Req.pInfo = (WMBssDesc *)pInfo;
        if (ssid != NULL)
        {
            MI_CpuCopy8(ssid, Req.ssid, WM_SIZE_CHILD_SSID);
        }
        else
        {
            MI_CpuClear8(Req.ssid, WM_SIZE_CHILD_SSID);
        }
        Req.powerSave = powerSave;
        Req.authMode = authMode;

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Disconnect

  Description:  �m������Ă���ڑ���ؒf����B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                aid         -   �ؒf����ʐM�����AID�B
                                �e�@�̏ꍇ�AID��1�`15�̎q�@���ʂɐؒf�B
                                �q�@�̏ꍇ�AID��0�̐e�@�Ƃ̒ʐM���I���B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disconnect(WMCallbackFunc callback, u16 aid)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(5,
                              WM_STATE_PARENT, WM_STATE_MP_PARENT,
                              WM_STATE_CHILD, WM_STATE_MP_CHILD, WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    if (                               // p->status->state�̃L���b�V���͔j���ς�
           (p->status->state == WM_STATE_PARENT) || (p->status->state == WM_STATE_MP_PARENT))
    {
        // �e�@�̏ꍇ
        if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
        {
            WM_WARNING("Parameter \"aid\" must be between 1 and %d.\n", WM_NUM_MAX_CHILD);
            return WM_ERRCODE_INVALID_PARAM;
        }
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (!(p->status->child_bitmap & (0x0001 << aid)))
        {
            WM_WARNING("There is no child that have aid %d.\n", aid);
            return WM_ERRCODE_NO_CHILD;
        }
    }
    else
    {
        // �q�@�̏ꍇ
        if (aid != 0)
        {
            WM_WARNING("Now child mode , so aid must be 0.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_DISCONNECT, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_DISCONNECT, 1, (u32)(0x0001 << aid));
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_DisconnectChildren

  Description:  �ڑ��m������Ă���q�@�����ꂼ��ؒf����B�e�@��p�֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                aidBitmap   -   �ؒf����q�@��AID�r�b�g�t�B�[���h�B
                                �ŉ��ʃr�b�g�͖�������Abit 1�`15 ��AID 1�`15 �̎q�@
                                �����ꂼ�ꎦ���B
                                �ڑ�����Ă��Ȃ��q�@�������r�b�g�͖��������̂ŁA
                                �ڑ��󋵂Ɋւ�炸�S�q�@��ؒf����ꍇ��0xFFFF���w��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_DisconnectChildren(WMCallbackFunc callback, u16 aidBitmap)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    WM_CHECK_RESULT(result);

    // �p�����[�^�`�F�b�N
    DC_InvalidateRange(&(p->status->child_bitmap), 2);
    if (!(p->status->child_bitmap & aidBitmap & 0xfffe))
    {
        WM_WARNING("There is no child that is included in \"aidBitmap\" %04x_.\n", aidBitmap);
        return WM_ERRCODE_NO_CHILD;
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_DISCONNECT, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_DISCONNECT, 1, (u32)aidBitmap);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
