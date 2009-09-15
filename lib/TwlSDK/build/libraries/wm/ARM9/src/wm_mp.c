/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - libraries
  File:     wm_mp.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WMi_StartMP

  Description:  MP�ʐM���J�n����B�e�@�E�q�@���ʊ֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                recvBuf     -   ��M�f�[�^�i�[�o�b�t�@�B
                                ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                recvBufSize -   ��M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                WM_GetReceiveBufferSize() �̕Ԃ�l�����傫���K�v������B
                sendBuf     -   ���M�f�[�^�i�[�o�b�t�@�B
                                ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                sendBufSize -   ���M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                WM_GetSendBufferSize() �̕Ԃ�l�����傫���K�v������B
                tmpParam    -   MP �̊J�n�O�ɐݒ肷�� MP �ʐM�̈ꎞ�p�����[�^�̍\���́B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartMP(WMCallbackFunc callback,
                      u16 *recvBuf,
                      u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, WMMPTmpParam* tmpParam)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    WM_CHECK_RESULT(result);

    // �q�@�ł̓p���[�Z�[�u���[�h�łȂ��ꍇ�� MP�J�n�֎~
    DC_InvalidateRange(&(status->aid), 2);
    DC_InvalidateRange(&(status->pwrMgtMode), 2);
    if (status->aid != 0 && status->pwrMgtMode != 1)
    {
        WM_WARNING("Could not start MP. Power save mode is illegal.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // MP��Ԋm�F
    DC_InvalidateRange(&(status->mp_flag), 4);  // ARM7�X�e�[�^�X�̈�̃L���b�V���𖳌���
    if (status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // �p�����[�^�`�F�b�N
    if ((recvBufSize & 0x3f) != 0)     // recvBufSize/2 �� 32Byte �̔{��
    {
        WM_WARNING("Parameter \"recvBufSize\" is not a multiple of 64.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((sendBufSize & 0x1f) != 0)     // sendBufSize �� 32Byte �̔{��
    {
        WM_WARNING("Parameter \"sendBufSize\" is not a multiple of 32.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)recvBuf & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"recvBuf\" is not 32-byte aligned.\n");
    }
    if ((u32)sendBuf & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"sendBuf\" is not 32-byte aligned.\n");
    }

    DC_InvalidateRange(&(status->mp_ignoreSizePrecheckMode),
                       sizeof(status->mp_ignoreSizePrecheckMode));
    if (status->mp_ignoreSizePrecheckMode == FALSE)
    {
        // ����M�T�C�Y�̎��O�`�F�b�N���s��
        if (recvBufSize < WM_GetMPReceiveBufferSize())
        {
            WM_WARNING("Parameter \"recvBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if (sendBufSize < WM_GetMPSendBufferSize())
        {
            WM_WARNING("Parameter \"sendBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
#ifndef SDK_FINALROM
        // �f�[�^�]�����v���Ԃ��m�F
        DC_InvalidateRange(&(status->state), 2);
        if (status->state == WM_STATE_PARENT)
        {
            DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
            (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                                status->pparam.childMaxSize,
                                                (u8)(status->pparam.maxEntry));
        }
#endif
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_START_MP, callback);

    // ARM7��FIFO�Œʒm
    {
        WMStartMPReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_START_MP;
        Req.recvBuf = (u32 *)recvBuf;
        Req.recvBufSize = (u32)(recvBufSize / 2);       // �o�b�t�@�P���̃T�C�Y
        Req.sendBuf = (u32 *)sendBuf;
        Req.sendBufSize = (u32)sendBufSize;

        MI_CpuClear32(&Req.param, sizeof(Req.param)); // unused
        MI_CpuCopy32(tmpParam, &Req.tmpParam, sizeof(Req.tmpParam));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartMPEx

  Description:  MP�ʐM���J�n����B�ڍׂȓ��샂�[�h���w��\�B�e�@�E�q�@���ʊ֐��B

  Arguments:    callback        -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                recvBuf         -   ��M�f�[�^�i�[�o�b�t�@�B
                                    ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                recvBufSize     -   ��M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                    WM_GetReceiveBufferSize() �̕Ԃ�l�����傫���K�v������B
                sendBuf         -   ���M�f�[�^�i�[�o�b�t�@�B
                                    ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                sendBufSize     -   ���M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                    WM_GetSendBufferSize() �̕Ԃ�l�����傫���K�v������B
                mpFreq          -   1�t���[���ɉ��� MP �ʐM�����邩�B
                                    0 �ŘA�����M���[�h�B�e�@�݈̂Ӗ������B
                defaultRetryCount - 0�`7�ԃ|�[�g�̒ʐM�̍ۂɁA���M�Ɏ��s�����ꍇ��
                                    ���g���C���s���W���̉񐔁B0 �Ń��g���C���s��Ȃ��B
                minPollBmpMode  -   MP �ʐM���� pollBitmap �𑗐M���� packet �̈���ɉ�����
                                    �Œ���̏W���ɗ}���铮�샂�[�h�B
                singlePacketMode -  1 ��� MP �ɂ��Ĉ�� packet ��������Ȃ��Ȃ�
                                    ����ȓ��샂�[�h�B
                fixFreqMode     -   ���g���C�ɂ�� MP �ʐM�̑������֎~�������ȓ��샂�[�h�B
                                    1�t���[������ MP �ʐM�̉񐔂����傤�� mpFreq ��ɌŒ�B

                ignoreFatalError -  FatalError��������AutoDisconnect���s�Ȃ�Ȃ��Ȃ�B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartMPEx(WMCallbackFunc callback,
             u16 *recvBuf,
             u16 recvBufSize,
             u16 *sendBuf,
             u16 sendBufSize,
             u16 mpFreq,
             u16 defaultRetryCount,
             BOOL minPollBmpMode, BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError)
{
    WMMPTmpParam tmpParam;

    MI_CpuClear32(&tmpParam, sizeof(tmpParam));

    tmpParam.mask = WM_MP_TMP_PARAM_MIN_FREQUENCY | WM_MP_TMP_PARAM_FREQUENCY | WM_MP_TMP_PARAM_DEFAULT_RETRY_COUNT
        | WM_MP_TMP_PARAM_MIN_POLL_BMP_MODE | WM_MP_TMP_PARAM_SINGLE_PACKET_MODE |
        WM_MP_TMP_PARAM_IGNORE_FATAL_ERROR_MODE;
    tmpParam.minFrequency = mpFreq;
    tmpParam.frequency = mpFreq;
    tmpParam.defaultRetryCount = defaultRetryCount;
    tmpParam.minPollBmpMode = (u8)minPollBmpMode;
    tmpParam.singlePacketMode = (u8)singlePacketMode;
    tmpParam.ignoreFatalErrorMode = (u8)ignoreFatalError;

    if (fixFreqMode != FALSE && mpFreq != 0)
    {
        tmpParam.mask |= WM_MP_TMP_PARAM_MAX_FREQUENCY;
        tmpParam.maxFrequency = mpFreq;
    }

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &tmpParam);
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartMP

  Description:  MP�ʐM���J�n����B�e�@�E�q�@���ʊ֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                recvBuf     -   ��M�f�[�^�i�[�o�b�t�@�B
                                ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                recvBufSize -   ��M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                WM_GetReceiveBufferSize() �̕Ԃ�l�����傫���K�v������B
                sendBuf     -   ���M�f�[�^�i�[�o�b�t�@�B
                                ARM7�����ڃf�[�^�������o���̂ŁA�L���b�V���ɒ��ӁB
                sendBufSize -   ���M�f�[�^�i�[�o�b�t�@�̃T�C�Y�B
                                WM_GetSendBufferSize() �̕Ԃ�l�����傫���K�v������B
                mpFreq      -   1�t���[���ɉ��� MP �ʐM�����邩�B
                                0 �ŘA�����M���[�h�B�e�@�݈̂Ӗ������B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMP(WMCallbackFunc callback,
                     u16 *recvBuf, u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, u16 mpFreq)
{
    WMMPTmpParam tmpParam;

    MI_CpuClear32(&tmpParam, sizeof(tmpParam));

    tmpParam.mask = WM_MP_TMP_PARAM_FREQUENCY | WM_MP_TMP_PARAM_MIN_FREQUENCY;
    tmpParam.minFrequency = mpFreq;
    tmpParam.frequency = mpFreq;

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &tmpParam);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParameter

  Description:  MP �ʐM�p�̊e��p�����[�^��ݒ肷��

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                param       -   MP �ʐM�p�̃p�����[�^���i�[�����\���̂ւ̃|�C���^

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParameter(WMCallbackFunc callback, const WMMPParam * param)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_SET_MP_PARAMETER, callback);

    // ARM7��FIFO�Œʒm
    {
        WMSetMPParameterReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_SET_MP_PARAMETER;
        MI_CpuCopy32(param, &Req.param, sizeof(Req.param));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPChildSize

  Description:  1���MP�ʐM�ł̎q�@�̑��M�o�C�g����ݒ肷��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                childSize   -   �q�@�̑��M�o�C�g���B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPChildSize(WMCallbackFunc callback, u16 childSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_CHILD_SIZE;
    param.childSize = childSize;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParentSize

  Description:  1���MP�ʐM�ł̐e�@�̑��M�o�C�g����ݒ肷��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                childSize   -   �e�@�̑��M�o�C�g���B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParentSize(WMCallbackFunc callback, u16 parentSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_SIZE;
    param.parentSize = parentSize;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPFrequency

  Description:  1�t���[���ɉ��� MP �ʐM���邩��؂�ւ���B�e�@�p�֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                mpFreq      -   1�t���[���ɉ��� MP �ʐM�����邩�B
                                0 �ŘA�����M���[�h�B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPFrequency(WMCallbackFunc callback, u16 mpFreq)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_FREQUENCY | WM_MP_PARAM_MIN_FREQUENCY;
    param.minFrequency = mpFreq;
    param.frequency = mpFreq;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPInterval

  Description:  1�t���[�����ɘA������ MP �ʐM���s���ꍇ�̊Ԋu��ݒ肷��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                parentInterval - 1�t���[�����ɘA�����čs���ꍇ�̐e�@�� MP �ʐM�Ԋu (ms)
                childInterval  - 1�t���[�����ɘA�����čs���ꍇ�̎q�@�� MP �ʐM����������Ԋu (ms)

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPInterval(WMCallbackFunc callback, u16 parentInterval, u16 childInterval)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_INTERVAL | WM_MP_PARAM_CHILD_INTERVAL;
    param.parentInterval = parentInterval;
    param.childInterval = childInterval;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPTiming

  Description:  �t���[�������ʐM���[�h���� MP �ʐM�̏������J�n����^�C�~���O��ݒ肷��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                parentVCount -  �t���[�������ʐM���̐e�@�����̊J�n V Count
                childVCount  -  �t���[�������ʐM���̎q�@�����̊J�n V Count

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPTiming(WMCallbackFunc callback, u16 parentVCount, u16 childVCount)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_VCOUNT | WM_MP_PARAM_CHILD_VCOUNT;
    param.parentVCount = parentVCount;
    param.childVCount = childVCount;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPortEx

  Description:  MP�ʐM�ɂđ��M����f�[�^��\�񂷂�B�e�@�E�q�@���ʊ֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B
                arg         -   callback �ɓn��������
                sendData    -   ���M�\�񂷂�f�[�^�ւ̃|�C���^�B
                                ���̃f�[�^�̎��̂͋����I�ɃL���b�V���X�g�A�����_�ɒ��ӁB
                sendDataSize -  ���M�\�񂷂�f�[�^�̃T�C�Y�B
                destBitmap  -   ���M��q�@������ aid �� bitmap ���w��B
                port        -   ���M����|�[�g�ԍ��B
                prio        -   �D��x (0:�ō��`3:�Œ�)

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode
WM_SetMPDataToPortEx(WMCallbackFunc callback, void *arg, const u16 *sendData, u16 sendDataSize,
                     u16 destBitmap, u16 port, u16 prio)
{
    WMErrCode result;
    BOOL    isParent;
    u16     mpReadyBitmap = 0x0001;
    u16     childBitmap = 0x0001;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // �K�v�ȏ���WMStatus�\���̂���擾
    DC_InvalidateRange(&(status->aid), 2);
    isParent = (status->aid == 0) ? TRUE : FALSE;
    if (isParent == TRUE)
    {
        DC_InvalidateRange(&(status->child_bitmap), 2);
        childBitmap = status->child_bitmap;
        DC_InvalidateRange(&(status->mp_readyBitmap), 2);
        mpReadyBitmap = status->mp_readyBitmap;
    }

    // �p�����[�^�`�F�b�N
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (childBitmap == 0)
    {
        WM_WARNING("There is no child connected.\n");
        return WM_ERRCODE_NO_CHILD;
    }
    if ((u32)sendData & 0x01f)
    {
        // �A���C���`�F�b�N�͌x���݂̂ŃG���[�ɂ͂��Ȃ�
        WM_WARNING("Parameter \"sendData\" is not 32-byte aligned.\n");
    }
    DC_InvalidateRange(&(status->mp_sendBuf), 2);
    if ((void *)sendData == (void *)status->mp_sendBuf)
    {
        WM_WARNING
            ("Parameter \"sendData\" must not be equal to the WM_StartMP argument \"sendBuf\".\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // ���M�T�C�Y�`�F�b�N
    if (sendDataSize > WM_SIZE_MP_DATA_MAX)
    {
        WM_WARNING("Parameter \"sendDataSize\" is over limit.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    if (sendDataSize == 0)
    {
        WM_WARNING("Parameter \"sendDataSize\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

#ifndef SDK_FINALROM
    // �f�[�^�]�����v���Ԃ��m�F
    DC_InvalidateRange(&(status->mp_current_minPollBmpMode), 2);
    if (isParent && status->mp_current_minPollBmpMode)
    {
        DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
        (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                            status->pparam.childMaxSize,
                                            (u8)MATH_CountPopulation((u32)destBitmap));
    }
#endif

    // �w��o�b�t�@�̃L���b�V���������o��
    DC_StoreRange((void *)sendData, sendDataSize);

    // �R�[���o�b�N�֐���o�^����K�v�Ȃ�

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_SET_MP_DATA, 7,
                             (u32)sendData,
                             (u32)sendDataSize,
                             (u32)destBitmap, (u32)port, (u32)prio, (u32)callback, (u32)arg);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndMP

  Description:  MP�ʐM���~����B�e�@�E�q�@���ʊ֐��B

  Arguments:    callback    -   �񓯊������������������ɌĂяo�����R�[���o�b�N�֐��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B�񓯊�����������ɊJ�n���ꂽ�ꍇ��
                                WM_ERRCODE_OPERATING���Ԃ���A���̌�R�[���o�b�N��
                                �񓯊������̌��ʂ����߂ēn�����B
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndMP(WMCallbackFunc callback)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // MP��Ԋm�F
    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == FALSE)
    {
        WM_WARNING("It is not MP mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // �R�[���o�b�N�֐���o�^
    WMi_SetCallbackTable(WM_APIID_END_MP, callback);

    // ARM7��FIFO�Œʒm
    result = WMi_SendCommand(WM_APIID_END_MP, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
