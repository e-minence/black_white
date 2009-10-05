/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - libraries
  File:     wm_sync.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         WM_SetIndCallback

  Description:  WM7����̏󋵒ʒm�ɑ΂��ČĂт������֐���ݒ肷��B

  Arguments:    callback    -   �����t�@�[���E�F�A����̏󋵒ʒm���ɌĂяo�����
                                �R�[���o�b�N�֐����w��B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetIndCallback(WMCallbackFunc callback)
{
    WMErrCode result;
    OSIntrMode e;

    // �����݋֎~
    e = OS_DisableInterrupts();

    // �������ς݊m�F
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        // �����݋֎~�߂�
        (void)OS_RestoreInterrupts(e);
        return result;
    }

    // Indication�p�R�[���o�b�N��ݒ�
    WMi_GetSystemWork()->indCallback = callback;
    // �����݋֎~�߂�
    (void)OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetPortCallback

  Description:  WM7����̒ʐM�t���[����M�ʒm�ɑ΂��ČĂт������֐���ݒ肷��B

  Arguments:    port        -   �|�[�g�ԍ��B
                callback    -   ��M�ʒm���ɌĂяo�����R�[���o�b�N�֐��B
                arg         -   �R�[���o�b�N�֐��� WMPortRecvCallback.arg �Ƃ���
                                �n���������B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPortCallback(u16 port, WMCallbackFunc callback, void *arg)
{
    WMErrCode result;
    OSIntrMode e;
    WMPortRecvCallback cb_Port;

    // �p�����[�^�`�F�b�N
#ifdef SDK_DEBUG
    if (port >= WM_NUM_OF_PORT)
    {
        WM_WARNING("Parameter \"port\" must be less than 16.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
#endif

    if (callback != NULL)
    {
        MI_CpuClear8(&cb_Port, sizeof(WMPortRecvCallback));
        cb_Port.apiid = WM_APIID_PORT_RECV;
        cb_Port.errcode = WM_ERRCODE_SUCCESS;
        cb_Port.state = WM_STATECODE_PORT_INIT;
        cb_Port.port = port;
        cb_Port.recvBuf = NULL;
        cb_Port.data = NULL;
        cb_Port.length = 0;
        cb_Port.seqNo = 0xffff;
        cb_Port.arg = arg;
        cb_Port.aid = 0;
        OS_GetMacAddress(cb_Port.macAddress);
    }

    // �����݋֎~
    e = OS_DisableInterrupts();
    // �������ς݊m�F
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        // �����݋֎~�߂�
        (void)OS_RestoreInterrupts(e);
        return result;
    }

    // �f�[�^��M�p�R�[���o�b�N���Z�b�g
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        p->portCallbackTable[port] = callback;
        p->portCallbackArgument[port] = arg;
    }

    if (callback != NULL)
    {
        cb_Port.connectedAidBitmap = WM_GetConnectedAIDs();
        cb_Port.myAid = WM_GetAID();
        (*callback) ((void *)&cb_Port);
    }

    // �����݋֎~�߂�
    (void)OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_ReadStatus

  Description:  �������C�u������Ԃ������\���̂��擾����B

  Arguments:    statusBuf   -   ��Ԃ��擾����ϐ��ւ̃|�C���^�B

  Returns:      WMErrCode   -   �������ʂ�Ԃ��B
 *---------------------------------------------------------------------------*/
WMErrCode WM_ReadStatus(WMStatus *statusBuf)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �������ς݊m�F
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // �p�����[�^�`�F�b�N
    if (statusBuf == NULL)
    {
        WM_WARNING("Parameter \"statusBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // WMStatus�\���̂�CPU�R�s�[
    DC_InvalidateRange(p->status, sizeof(WMStatus));
    MI_CpuCopyFast(p->status, statusBuf, sizeof(WMStatus));

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPSendBufferSize

  Description:  MP �ʐM�̑��M�o�b�t�@�̃T�C�Y���v�Z����B
                StartParent, StartConnect ���I����Ă���K�v������B

  Arguments:    None.

  Returns:      int     -   WM_StartMP �ɓn���K�v�����鑗�M�o�b�t�@�̃T�C�Y�B
 *---------------------------------------------------------------------------*/
int WM_GetMPSendBufferSize(void)
{
    WMErrCode result;
    int     maxSendSize;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }

    // MP��Ԃ��m�F
    DC_InvalidateRange(&(p->status->mp_flag), 4);       // ARM7�X�e�[�^�X�̈�̃L���b�V���𖳌���
    if (p->status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return 0;
    }

    // �v�Z�ɕK�v�ȏ���WMStatus�\���̂���Q��
    DC_InvalidateRange(&(p->status->mp_maxSendSize), 4);
    maxSendSize = p->status->mp_maxSendSize;

    return ((maxSendSize + 31) & ~0x1f);
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPReceiveBufferSize

  Description:  MP �ʐM�̎�M�o�b�t�@�̃T�C�Y���v�Z����B
                StartParent, StartConnect ���I����Ă���K�v������B

  Arguments:    None.

  Returns:      int     -   WM_StartMP �ɓn���K�v�������M�o�b�t�@�̃T�C�Y�B
 *---------------------------------------------------------------------------*/
int WM_GetMPReceiveBufferSize(void)
{
    WMErrCode result;
    BOOL    isParent;
    int     maxReceiveSize;
    int     maxEntry;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �X�e�[�g�`�F�b�N
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }

    // MP��Ԃ��m�F
    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return 0;
    }

    // �v�Z�ɕK�v�ȏ���WMStatus�\���̂���Q��
    DC_InvalidateRange(&(p->status->aid), 2);
    isParent = (p->status->aid == 0) ? TRUE : FALSE;
    DC_InvalidateRange(&(p->status->mp_maxRecvSize), 2);
    maxReceiveSize = p->status->mp_maxRecvSize;
    if (isParent == TRUE)
    {
        DC_InvalidateRange(&(p->status->pparam.maxEntry), 2);
        maxEntry = p->status->pparam.maxEntry;
        return (int)((sizeof(WMmpRecvHeader) - sizeof(WMmpRecvData) +
                      ((sizeof(WMmpRecvData) + maxReceiveSize - 2 + 2 /*MACBUG*/) * maxEntry)
                      + 31) & ~0x1f) * 2;
    }
    else
    {
        return (int)((sizeof(WMMpRecvBuf) + maxReceiveSize - 4 + 31) & ~0x1f) * 2;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WM_ReadMPData

  Description:  ��M�����f�[�^�S�̂���w��q�@�̃f�[�^�����𒊏o����B

  Arguments:    header      -   ��M�f�[�^�S�̂������|�C���^�B
                aid         -   �f�[�^�𒊏o����q�@��AID�B

  Returns:      WMMpRecvData* - �Y���q�@����̎�M�f�[�^�ւ̃|�C���^��Ԃ��B
                                ���o�Ɏ��s�����ꍇ�ANULL��Ԃ��B
 *---------------------------------------------------------------------------*/
WMMpRecvData *WM_ReadMPData(const WMMpRecvHeader *header, u16 aid)
{
    int     i;
    WMErrCode result;
    WMMpRecvData *recvdata_p[WM_NUM_MAX_CHILD]; // �e�q�@�f�[�^�̊J�n�ʒu�|�C���^�z��(15�䕪)
    WMArm9Buf *p = WMi_GetSystemWork();

    // �������ς݂��m�F
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return NULL;
    }

    // �p�����[�^�`�F�b�N
    if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
    {
        WM_WARNING("Parameter \"aid\" must be between 1 and %d.\n", WM_NUM_MAX_CHILD);
        return NULL;
    }

    // ����aid�̎q�@�͑��݂��邩�H
    DC_InvalidateRange(&(p->status->child_bitmap), 2);
    if (0 == (p->status->child_bitmap & (0x0001 << aid)))
    {
        return NULL;
    }

    // ��M�f�[�^�����݂��邩�H || count �t�B�[���h���ُ�l�łȂ����H
    if (header->count == 0 || header->count > WM_NUM_MAX_CHILD)
    {
        return NULL;
    }

    // �e�q�@�f�[�^�̊J�n�ʒu���v�Z
    recvdata_p[0] = (WMMpRecvData *)(header->data);

    i = 0;
    do
    {
        // �w�肳�ꂽAID�̎q�@�f�[�^�Ȃ炻�̃|�C���^��Ԃ�
        if (recvdata_p[i]->aid == aid)
        {
            return recvdata_p[i];
        }
        ++i;
        recvdata_p[i] = (WMMpRecvData *)((u32)(recvdata_p[i - 1]) + header->length);
    }
    while (i < header->count);

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetAllowedChannel

  Description:  �ʐM���Ɏg�p�������ꂽ�`�����l�����擾����B

  Arguments:    None.

  Returns:      u16 -   �g�p���`�����l���̃r�b�g�t�B�[���h��Ԃ��B�ŉ��ʃr�b�g��
                        1�`�����l���A�ŏ�ʃr�b�g��16�`�����l���������B�r�b�g��1��
                        �`�����l�����g�p���A�r�b�g��0�̃`�����l���͎g�p�֎~�B
                        �ʏ�� 1�`13 �`�����l���̓��������̃r�b�g��1�ɂȂ����l��
                        �Ԃ����B0x0000���Ԃ��ꂽ�ꍇ�͎g�p�������ꂽ�`�����l����
                        ���݂��Ȃ����߁A�����@�\���̂��̂��g�p�֎~�ł���B
                        �܂��A�����������ȂǊ֐��Ɏ��s�����ꍇ��0x8000���Ԃ����B
 *---------------------------------------------------------------------------*/
u16 WM_GetAllowedChannel(void)
{
#ifdef WM_PRECALC_ALLOWEDCHANNEL
    WMErrCode result;

    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return 0;
        }
    }
    #endif
    
    // �������ς݂��m�F
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_GET_ALLOWED_CHANNEL_BEFORE_INIT;
    }
    return *((u16 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
                     ((sizeof(NVRAMConfig) + 3) & ~0x00000003) + 6));
#else
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    
    #ifdef SDK_TWL // TWL �{�̐ݒ�Ŗ������g�p���Ȃ��悤�ɐݒ肵���ꍇ�G���[��Ԃ�
    if( OS_IsRunOnTwl() )
    {
        if( WMi_CheckEnableFlag() == FALSE )
        {
            return 0;
        }
    }
    #endif
    
    // �����n�[�h�N���ς݂��m�F
    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_GET_ALLOWED_CHANNEL_BEFORE_INIT;
    }

    DC_InvalidateRange(&(p->status->allowedChannel), 2);
    return p->status->allowedChannel;
#endif
}

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
/*---------------------------------------------------------------------------*
  Name:         WM_IsExistAllowedChannel

  Description:  �ʐM���Ɏg�p�������ꂽ�`�����l�������݂��邩�ǂ����m�F����B
                WM���C�u�����𖢏������ł����Ă�����ɔ���\�B

  Arguments:    None.

  Returns:      BOOL    -   �g�p�����ꂽ�`�����l�������݂���ꍇ��TRUE���A
                            ���݂��Ȃ��ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
BOOL WM_IsExistAllowedChannel(void)
{
    u16     allowedChannel;

    allowedChannel = *((u16 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
                     ((sizeof(NVRAMConfig) + 3) & ~0x00000003) + 6));
    if (allowedChannel)
    {
        return TRUE;
    }
    return FALSE;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         WM_GetLinkLevel

  Description:  �ʐM���̃����N���x���擾����B�����֐��B

  Arguments:    None.

  Returns:      WMLinkLevel -   4�i�K�ɕ]�����������N���x��Ԃ��B
 *---------------------------------------------------------------------------*/
WMLinkLevel WM_GetLinkLevel(void)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // �������ς݂��m�F
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_LINK_LEVEL_0;        // �Œ�d�g���x
    }

    DC_InvalidateRange(&(p->status->state), 2);
    switch (p->status->state)
    {
    case WM_STATE_MP_PARENT:
        // �e�@�̏ꍇ
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (p->status->child_bitmap == 0)
        {
            // �q�@�����Ȃ��ꍇ
            return WM_LINK_LEVEL_0;    // �Œ�d�g���x
        }
    case WM_STATE_MP_CHILD:
    case WM_STATE_DCF_CHILD:
        // �q�@�̏ꍇ
        DC_InvalidateRange(&(p->status->linkLevel), 2);
        return (WMLinkLevel)(p->status->linkLevel);
    }

    // ���ڑ��̏ꍇ
    return WM_LINK_LEVEL_0;            // �Œ�d�g���x
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionBeaconPeriod

  Description:  �e�@�Ƃ��Ċ�������ۂɐݒ肷�ׂ��r�[�R���Ԋu�l���擾����B

  Arguments:    None.

  Returns:      u16 -   �ݒ肷�ׂ��r�[�R���Ԋu�l(ms)�B
 *---------------------------------------------------------------------------*/
u16 WM_GetDispersionBeaconPeriod(void)
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
    ret *= 7;
    return (u16)(WM_DEFAULT_BEACON_PERIOD + (ret % 20));
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionScanPeriod

  Description:  �q�@�Ƃ��Đe�@��T������ۂɐݒ肷�ׂ��T�����x���Ԃ��擾����B

  Arguments:    None.

  Returns:      u16 -   �ݒ肷�ׂ��T�����x����(ms)�B
 *---------------------------------------------------------------------------*/
u16 WM_GetDispersionScanPeriod(void)
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
    return (u16)(WM_DEFAULT_SCAN_PERIOD + (ret % 10));
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetOtherElements

  Description:  �r�[�R�����̊g���G�������g���擾����B
                �����֐��B

  Arguments:    bssDesc - �e�@���\���́B
                          WM_StartScan(Ex)�ɂĎ擾�����\���̂��w�肷��B

  Returns:      WMOtherElements - �g���G�������g�\���́B
 *---------------------------------------------------------------------------*/
WMOtherElements WM_GetOtherElements(WMBssDesc *bssDesc)
{
    WMOtherElements elems;
    u8     *p_elem;
    int     i;
    u16      curr_elem_len;
    u16      elems_len;                 // elements�S�̂̒���
    u16      elems_len_counter;         // elements�̒����`�F�b�N�p�J�E���^

    // gameInfo�������Ă���ꍇ�͏I��
    if (bssDesc->gameInfoLength != 0)
    {
        elems.count = 0;
        return elems;
    }

    // otherElement�̐����擾���A0�Ȃ�I��
    elems.count = (u8)(bssDesc->otherElementCount);
    if (elems.count == 0)
        return elems;

    // elems�̐���WM_SCAN_OTHER_ELEMENT_MAX�܂łɐ�������
    if (elems.count > WM_SCAN_OTHER_ELEMENT_MAX)
        elems.count = WM_SCAN_OTHER_ELEMENT_MAX;

    // �G�������g�̐擪���A�܂�gameInfo�ɐݒ�
    p_elem = (u8 *)&(bssDesc->gameInfo);

    // elements�S�̂̒������擾���A�`�F�b�N�p�J�E���^��������
    elems_len = (u16)((bssDesc->length * sizeof(u16)) - 64);
    elems_len_counter = 0;

    // elems�̐��������[�v����
    for (i = 0; i < elems.count; ++i)
    {
        elems.element[i].id = p_elem[0];
        elems.element[i].length = p_elem[1];
        elems.element[i].body = (u8 *)&(p_elem[2]);

        // ����̃G�������g�����v�Z���A�`�F�b�N�J�E���^�ɉ�����
        curr_elem_len = (u16)(elems.element[i].length + 2);
        elems_len_counter += curr_elem_len;

        // OS_TPrintf("eles_len        =%d\n", elems_len);
        // OS_TPrintf("eles_len_counter=%d\n", elems_len_counter);

        // elements�S�̂̒����𒴂����ꍇ�̓G���[�Ƃ��A
        // element�͖����������̂Ƃ��Ēʒm����
        if (elems_len_counter > elems_len)
        {
            WM_WARNING("Elements length error.\n");
            elems.count = 0;
            return elems;
        }

        // ���̃G�������g�̐擪�A�h���X���v�Z
        p_elem = (u8 *)(p_elem + curr_elem_len);
    }

    return elems;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetNextTgid

  Description:  �����������ꂽ��ӂ� TGID �l���擾����B
                �����֐��B

  Arguments:    None.

  Returns:      ����Ăяo������ RTC �����Ƃɐ������ꂽ TGID���A
                ����ȍ~�͑O��̕Ԃ�l�� 1 �������Z�����l��Ԃ��B
 *---------------------------------------------------------------------------*/
u16 WM_GetNextTgid(void)
{
    enum
    { TGID_DEFAULT = (1 << 16) };
    static u32 tgid_bak = (u32)TGID_DEFAULT;
    /* ���@�ŗL�̎��Ԃɂ������Ӑ���ۂ悤, ����� RTC �̎����l���g�p */
    if (tgid_bak == (u32)TGID_DEFAULT)
    {
        RTCTime rt[1];
        RTC_Init();
        if (RTC_GetTime(rt) == RTC_RESULT_SUCCESS)
        {
            tgid_bak = (u16)(rt->second + (rt->minute << 8));
        }
        else
        {
            OS_TWarning("failed to get RTC-data to create unique TGID!\n");
        }
    }
    /* ��ӂȒl����ɂ�����C���N�������g���� */
    tgid_bak = (u16)(tgid_bak + 1);
    return (u16)tgid_bak;
}


/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
