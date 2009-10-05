/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WXC - libraries -
  File:     wxc_api.c

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-12-16#$
  $Rev: 9661 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <nitro/wxc.h>


/*****************************************************************************/
/* declaration */

/* WXC���C�u�����̑S���[�N�ϐ� */
typedef struct WXCWork
{
    u32     wm_dma;
    WXCCallback system_callback;
    WXCScheduler scheduler;
    WXCUserInfo user_info[WM_NUM_MAX_CHILD + 1];
    BOOL    stopping;
    u8      beacon_count;              /* �r�[�R�����M�� */
    u8      padding[3];
    WMParentParam parent_param ATTRIBUTE_ALIGN(32);
    WXCDriverWork driver ATTRIBUTE_ALIGN(32);
    WXCProtocolContext protocol[1] ATTRIBUTE_ALIGN(32);
}
WXCWork;

SDK_STATIC_ASSERT(sizeof(WXCWork) <= WXC_WORK_SIZE);


/*****************************************************************************/
/* variable */

static WXCStateCode state = WXC_STATE_END;
static WXCWork *work = NULL;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXCi_ChangeState

  Description:  ������Ԉڍs�Ɠ����ɃR�[���o�b�N����.

  Arguments:    stat          �ڍs������
                arg           �R�[���o�b�N���� (��Ԃɂ���ĕς��)
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXCi_ChangeState(WXCStateCode stat, void *arg)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        state = stat;
        if (work->system_callback)
        {
            (*work->system_callback) (state, arg);
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_SeekNextBlock

  Description:  ���ɋN�����ׂ��u���b�N������.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_SeekNextBlock(void)
{
    /* ���̃u���b�N��I��. */
    WXCProtocolRegistry *target =
        WXC_FindNextBlock(work->protocol, work->protocol->current_block, 0, FALSE);
    if (!target)
    {
        /* �o�^���ꂽ�f�[�^�������ꍇ��, �d���Ȃ��̂ŋ�̃u���b�N��I�� */
        target = WXC_FindNextBlock(work->protocol, NULL, 0, TRUE);
    }
    // �O�̂��߁A�v���g�R����S���C���X�g�[�����Ă��Ȃ��ꍇ���l���B
    if (target != NULL)
    {
        WXC_SetCurrentBlock(work->protocol, target);
        work->parent_param.ggid = target->ggid;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_DriverEventCallback

  Description:  ���C�����X�h���C�o����̃C�x���g�R�[���o�b�N

  Arguments:    event         �ʒm����C�x���g
                arg           �C�x���g���Ƃɒ�`���ꂽ����

  Returns:      �C�x���g���Ƃɒ�`���ꂽ u32 �̃C�x���g���ʒl.
 *---------------------------------------------------------------------------*/
static u32 WXCi_DriverEventCallback(WXCDriverEvent event, void *arg)
{
    u32     result = 0;

    switch (event)
    {
    case WXC_DRIVER_EVENT_BEACON_SEND:
        /*
         * �r�[�R���X�V�^�C�~���O (������ WMParentParam �|�C���^)
         * �N�����Ȃ�r�[�R����Ԃ��X�V.
         */
        if (!work->stopping && (WXC_GetStateCode() == WXC_STATE_ACTIVE))
        {
            WXC_BeaconSendHook(work->protocol, &work->parent_param);
            /* ���΂炭���ڑ��Ȃ�ċN������ */
            if (work->driver.peer_bitmap == 0)
            {
                if (++work->beacon_count > WXC_PARENT_BEACON_SEND_COUNT_OUT)
                {
                    work->beacon_count = 0;
                    /* �X�P�W���[�����X�V��, �܂��e�@���[�h�Ȃ�p�� */
                    if (WXCi_UpdateScheduler(&work->scheduler))
                    {
                    }
                    /* �q�@���[�h�ł���Ώ�ԕύX */
                    else
                    {
                        WXCi_StartChild(&work->driver);
                    }
                }
            }
        }
        break;

    case WXC_DRIVER_EVENT_BEACON_RECV:
        /*
         * �r�[�R�����o�^�C�~���O (������ WMBssDesc �|�C���^)
         * �ڑ��ΏۂƂ݂Ȃ��� TRUE ��, �����łȂ���� FALSE ��Ԃ�.
         */
        {
            WXCBeaconFoundCallback cb;
            /* �r�[�R���̔��f���܂��v���g�R���ɍs�킹�� */
            cb.bssdesc = (const WMBssDesc *)arg;
            cb.matched = WXC_BeaconRecvHook(work->protocol, cb.bssdesc);
            /* ���̌��ʂ��ӂ܂����[�U�A�v���ł����f�ł���悤�ɂ��� */
            (*work->system_callback) (WXC_STATE_BEACON_RECV, &cb);
            result = (u32)cb.matched;
            /* ���ʂƂ��Đڑ����ׂ��Ȃ炱���� TRUE ��Ԃ� */
            if (result)
            {
                work->parent_param.ggid = WXC_GetCurrentBlock(work->protocol)->ggid;
            }
        }
        break;

    case WXC_DRIVER_EVENT_STATE_END:
        /*
         * ���C�����X�I�� (������WXC�Ɋ��蓖�Ă�ꂽ���[�N������)
         */
        work->stopping = FALSE;
        WXCi_ChangeState(WXC_STATE_END, work);
        break;

    case WXC_DRIVER_EVENT_STATE_STOP:
        /*
         * STOP �X�e�[�g�ɑJ�ڊ���. (�����͏�� NULL)
         */
        work->stopping = FALSE;
        WXCi_ChangeState(WXC_STATE_READY, NULL);
        break;

    case WXC_DRIVER_EVENT_STATE_IDLE:
        /*
         * IDLE �X�e�[�g�ɑJ�ڊ���. (�����͏�� NULL)
         */
        /* �I���������Ȃ炱���Ŗ����I�� */
        if (WXC_GetStateCode() != WXC_STATE_ACTIVE)
        {
            WXCi_End(&work->driver);
        }
        /* ��~�������Ȃ炱���Ŗ�����~ */
        else if (work->stopping)
        {
            WXCi_Stop(&work->driver);
        }
        /* �����łȂ���ΐe�@�E�q�@�̐؂�ւ� */
        else if (WXCi_UpdateScheduler(&work->scheduler))
        {
            /* �����p�f�[�^�������o�^����Ă���ꍇ�͂����� GGID ���؂�ւ�� */
            WXCi_SeekNextBlock();
            /* GGID �ɉ������e�@�ݒ菉�������v���g�R���ɍs�킹�� */
            WXC_BeaconSendHook(work->protocol, &work->parent_param);
            /* �e�@�J�n */
            WXCi_StartParent(&work->driver);
        }
        else
        {
            WXCi_StartChild(&work->driver);
        }
        break;

    case WXC_DRIVER_EVENT_STATE_PARENT:
    case WXC_DRIVER_EVENT_STATE_CHILD:
        /*
         * MP �X�e�[�g�ɑJ�ڊ���. (�����͏�� NULL)
         */
        /* �I���������Ȃ炱���Ŗ����I�� */
        if (WXC_GetStateCode() != WXC_STATE_ACTIVE)
        {
            WXCi_End(&work->driver);
        }
        /* ��~�������Ȃ炱���Ŗ�����~ */
        else if (work->stopping)
        {
            WXCi_Stop(&work->driver);
        }
        work->beacon_count = 0;
        /* ���g�̃��[�U����ݒ� */
        {
            WXCUserInfo *p_user = &work->user_info[work->driver.own_aid];
            p_user->aid = work->driver.own_aid;
            OS_GetMacAddress(p_user->macAddress);
        }
        break;

    case WXC_DRIVER_EVENT_CONNECTING:
        /*
         * �ڑ��O�ʒm. (������ WMBssDesc �|�C���^)
         */
        {
            WMBssDesc * bss = (WMBssDesc *)arg;
            WXC_CallPreConnectHook(work->protocol, bss, &bss->ssid[8]);
        }
        break;

    case WXC_DRIVER_EVENT_CONNECTED:
        /*
         * �ڑ��ʒm. (������ WMStartParentCallback �� WMStartConnectCallback)
         */
        {
            const WXCProtocolRegistry *block = WXC_GetCurrentBlock(work->protocol);
            WMStartParentCallback *cb = (WMStartParentCallback *)arg;
            const BOOL is_parent = WXC_IsParentMode();
            const u16 parent_size =
                (u16)(is_parent ? work->parent_param.
                      parentMaxSize : WXCi_GetParentBssDesc(&work->driver)->gameInfo.parentMaxSize);
            const u16 child_size =
                (u16)(is_parent ? work->parent_param.
                      childMaxSize : WXCi_GetParentBssDesc(&work->driver)->gameInfo.childMaxSize);
            const u16 aid = (u16)(is_parent ? cb->aid : 0);
            WXCUserInfo *p_user = &work->user_info[aid];
            const BOOL is_valid_block = (!work->stopping && (WXC_GetStateCode() == WXC_STATE_ACTIVE) && block);

            /* �܂�, �ڑ�����̃��[�U�����R�s�[ */
            p_user->aid = aid;
            if (is_parent)
            {
                WM_CopyBssid16(cb->macAddress, p_user->macAddress);
            }
            else
            {
                const WMBssDesc *p_bss = WXCi_GetParentBssDesc(&work->driver);
                WM_CopyBssid16(p_bss->bssid, p_user->macAddress);
            }

            /* ����, ���݂�MP�ݒ�� impl �������� */
            WXC_ResetSequence(work->protocol, parent_size, child_size);

            /*
             * �I�����łȂ�, ����
             * RegisterData �����ݗL���Ȃ炱���Ŏ����o�^
             */
            if (is_valid_block)
            {
                /* �����o�^����f�[�^������ꍇ�̂݁A�f�[�^��o�^ */
                if ((block->send.buffer != NULL) && (block->recv.buffer != NULL))
                {
                    WXC_AddBlockSequence(work->protocol,
                                         block->send.buffer, block->send.length,
                                         block->recv.buffer, block->recv.length);
                }
            }

            /* �v���g�R���֐ڑ���ʒm */
            WXC_ConnectHook(work->protocol, (u16)(1 << aid));

            /* �K�v�Ȃ烆�[�U�A�v���փR�[���o�b�N */
            if (is_valid_block)
            {
                (*work->system_callback) (WXC_STATE_CONNECTED, p_user);
            }

            /*
             * ���̎��_�ŒN�� AddData() ���Ă��Ȃ����,
             * impl �� !IsExecuting() �ɂȂ�悤�ɐi�s����.
             */
        }
        break;

    case WXC_DRIVER_EVENT_DISCONNECTED:
        /*
         * �ؒf�ʒm.
         * �EWM_Dissconect ������
         * �EWM_StartParent �C���W�P�[�g
         * �EPortRecvCallback
         * ��3�ӏ��ŌĂяo����Ă���, �d���̉\��������.
         */
        WXC_DisconnectHook(work->protocol, (u16)(u32)arg);
        /*
         * �����R�[���o�b�N���� WXC_End() ���ꂽ��
         * �S�Ẵf�[�^�� Unregister ����Ă���ꍇ�ɂ�
         * �ؒf�̃^�C�~���O�Ń��Z�b�g����.
         */
        if (!WXC_GetUserBitmap())
        {
            if ((WXC_GetStateCode() == WXC_STATE_ACTIVE) ||
                (WXC_GetStateCode() == WXC_STATE_ENDING) || !WXC_GetCurrentBlock(work->protocol))
            {
                work->beacon_count = 0;
                WXCi_Reset(&work->driver);
            }
        }
        break;

    case WXC_DRIVER_EVENT_DATA_SEND:
        /* MP �p�P�b�g���M�t�b�N. (������ WXCPacketInfo �|�C���^) */
        WXC_PacketSendHook(work->protocol, (WXCPacketInfo *) arg);
        break;

    case WXC_DRIVER_EVENT_DATA_RECV:
        /* MP �p�P�b�g��M�t�b�N. (������ const WXCPacketInfo �|�C���^) */
        (void)WXC_PacketRecvHook(work->protocol, (const WXCPacketInfo *)arg);
        /* ���łɃv���g�R���I�ɐؒf���Ă���ꍇ�� Reset */
        if (!work->protocol->current_block->impl->IsExecuting(work->protocol))
        {
            WXCi_Reset(&work->driver);
        }
        break;

    default:
        OS_TPanic("unchecked event implementations!");
        break;
    }

    return result;
}

/*---------------------------------------------------------------------------*
    �O���֐�
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WXC_Init

  Description:  ���ꂿ�����ʐM�������֐�.
                ������Ԃ�����������.

  Arguments:    work_mem      ���C�u���������Ŏg�p���郏�[�N������.
                              WXC_WORK_SIZE �o�C�g�ȏ��, ����
                              32 BYTE �A���C�������g����Ă���K�v������.
                callback      ���C�u�����̊e��V�X�e����Ԃ�ʒm����
                              �R�[���o�b�N�ւ̃|�C���^.
                dma           ���C�u���������Ŏg�p���� DMA �`�����l��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_Init(void *work_mem, WXCCallback callback, u32 dma)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    // ����Ăяo�����Ɋe��v���g�R�����C���X�g�[���B
    static BOOL initialized = FALSE;
    if (!initialized)
    {
        WXC_InstallProtocolImpl(WXCi_GetProtocolImplCommon());
        WXC_InstallProtocolImpl(WXCi_GetProtocolImplWPB());
        WXC_InstallProtocolImpl(WXCi_GetProtocolImplWXC());
        initialized = TRUE;
    }
    {
        if (WXC_GetStateCode() == WXC_STATE_END)
        {
            if (((u32)work_mem & 31) != 0)
            {
                OS_TPanic("WXC work buffer must be 32-byte aligned!");
            }
            work = (WXCWork *) work_mem;
            MI_CpuClear32(work, sizeof(WXCWork));
            work->wm_dma = dma;
            work->system_callback = callback;
            WXCi_InitScheduler(&work->scheduler);
            WXC_InitProtocol(work->protocol);
            /* �e�@�ݒ�������� */
            work->parent_param.maxEntry = 1;    /* ����� 1��1�ڑ��̂ݑΉ� */
            work->parent_param.parentMaxSize = WXC_PACKET_SIZE_MAX;
            work->parent_param.childMaxSize = WXC_PACKET_SIZE_MAX;
            work->parent_param.CS_Flag = 1;     /* �A���]�� */
            WXC_InitDriver(&work->driver, &work->parent_param, WXCi_DriverEventCallback,
                           work->wm_dma);
            WXCi_ChangeState(WXC_STATE_READY, NULL);
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetParentParameter

  Description:  ���C�����X�̒ʐM�ݒ���w�肷��.
                �ݒ���e�͐e�@���[�h�̏�Ԏ��ł̂ݎg�p����,
                �q�@���[�h�Őڑ������ꍇ�͐e�@���̐ݒ�ɏ]��.
                ���̊֐��̓I�v�V�����ł���, �K�v�ȏꍇ�Ɍ���
                WXC_Start �֐����O�ɌĂяo��.

  Arguments:    sendSize      �e�@���M�T�C�Y.
                              WXC_PACKET_SIZE_MIN �ȏォ��
                              WXC_PACKET_SIZE_MAX �ȉ��ł���K�v������.
                recvSize      �e�@��M�T�C�Y.
                              WXC_PACKET_SIZE_MIN �ȏォ��
                              WXC_PACKET_SIZE_MAX �ȉ��ł���K�v������.
                maxEntry      �ő�ڑ��l��.
                              ���݂̎����ł͂��̎w��� 1 �̂݃T�|�[�g�����.

  Returns:      �ݒ肪�L���ł���Γ����ɔ��f���� TRUE,
                �����łȂ���Όx�������f�o�b�O�o�͂��� FALSE.
 *---------------------------------------------------------------------------*/
BOOL WXC_SetParentParameter(u16 sendSize, u16 recvSize, u16 maxEntry)
{
    BOOL    ret = FALSE;

    /* �������ł͈�Α��̒ʐM�ɖ��Ή� */
    if (maxEntry > 1)
    {
        OS_TWarning("unsupported maxEntry. (must be 1)\n");
    }
    /* �p�P�b�g�T�C�Y���� */
    else if ((sendSize < WXC_PACKET_SIZE_MIN) || (sendSize > WXC_PACKET_SIZE_MAX) ||
             (recvSize < WXC_PACKET_SIZE_MIN) || (recvSize > WXC_PACKET_SIZE_MAX))
    {
        OS_TWarning("packet size is out of range.\n");
    }
    /* MP �ʐM���v���Ԃ𔻒� */
    else
    {
        /* 1���MP�ʐM�ɂ����鎞�Ԃɑ΂���]�� */
        int     usec = 330 + 4 * (sendSize + 38) + maxEntry * (112 + 4 * (recvSize + 32));
        const int max_time = 5600;
        if (usec >= max_time)
        {
            OS_TWarning("specified MP setting takes %d[usec]. (should be lower than %d[usec])\n",
                        usec, max_time);
        }
        else
        {
            work->parent_param.parentMaxSize = sendSize;
            work->parent_param.childMaxSize = recvSize;
            ret = TRUE;
        }
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_Start

  Description:  ���ꂿ�����ʐM�J�n�֐�.
                �����̃��C�����X�쓮���J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_Start(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_END);

        if (WXC_GetStateCode() == WXC_STATE_READY)
        {
            WXCi_Reset(&work->driver);
            WXCi_ChangeState(WXC_STATE_ACTIVE, NULL);
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_Stop

  Description:  ���ꂿ�����ʐM��~�֐�.
                �����̃��C�����X�쓮���~����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_Stop(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_END);

        switch (WXC_GetStateCode())
        {

        case WXC_STATE_ACTIVE:
            if (!work->stopping)
            {
                work->stopping = TRUE;
                /* ���ݒʐM���Ȃ��~�v�������ݒ肵������҂� */
                if (WXC_GetUserBitmap() == 0)
                {
                    WXCi_Stop(&work->driver);
                }
            }
            break;

        case WXC_STATE_READY:
        case WXC_STATE_ENDING:
        case WXC_STATE_END:
            /* �I�����������I���ς݂Ȃ牽�����Ȃ� */
            break;

        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);

}

/*---------------------------------------------------------------------------*
  Name:         WXC_End

  Description:  ���ꂿ�����ʐM�I���֐�.
                �����̃��C�����X�쓮���I������.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_End(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    {
        switch (WXC_GetStateCode())
        {

        case WXC_STATE_READY:
        case WXC_STATE_ACTIVE:
            WXCi_ChangeState(WXC_STATE_ENDING, NULL);
            /* ���ݒʐM���Ȃ�I���v�������ݒ肵������҂� */
            if (WXC_GetUserBitmap() == 0)
            {
                WXCi_End(&work->driver);
            }
            break;

        case WXC_STATE_ENDING:
        case WXC_STATE_END:
            /* �I�����������I���ς݂Ȃ牽�����Ȃ� */
            break;

        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetStateCode

  Description:  ���݂̃��C�u������Ԃ��擾.

  Arguments:    None.

  Returns:      ���݂̃��C�u������Ԃ����� WXCStateCode �^.
 *---------------------------------------------------------------------------*/
WXCStateCode WXC_GetStateCode(void)
{
    return state;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_IsParentMode

  Description:  ���݂̃��C�����X��Ԃ��e�@���[�h������.
                WXC_STATE_ACTIVE �̏�ԂɌ���L��.

  Arguments:    None.

  Returns:      ���C�����X��Ԃ��e�@���[�h�Ȃ� TRUE, �q�@���[�h�Ȃ� FALSE.
 *---------------------------------------------------------------------------*/
BOOL WXC_IsParentMode(void)
{
    return WXCi_IsParentMode(&work->driver);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetParentParam

  Description:  ���݂̃��C�����X��Ԃ� WMParentParam �\���̂��Q��.

  Arguments:    None.

  Returns:      WMParentParam �\����.
 *---------------------------------------------------------------------------*/
const WMParentParam *WXC_GetParentParam(void)
{
    return &work->parent_param;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetParentBssDesc

  Description:  ���ݐڑ��Ώۂ� WMBssDesc �\���̂��Q��.

  Arguments:    None.

  Returns:      WMBssDesc �\����.
 *---------------------------------------------------------------------------*/
const WMBssDesc *WXC_GetParentBssDesc(void)
{
    return WXCi_GetParentBssDesc(&work->driver);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetUserBitmap

  Description:  ���ݐڑ����̃��[�U�󋵂��r�b�g�}�b�v�Ŏ擾.
                

  Arguments:    None.

  Returns:      WMBssDesc �\����.
 *---------------------------------------------------------------------------*/
u16 WXC_GetUserBitmap(void)
{
    u16     bitmap = work->driver.peer_bitmap;
    if (bitmap != 0)
    {
        bitmap = (u16)(bitmap | (1 << work->driver.own_aid));
    }
    return bitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetCurrentGgid

  Description:  ���݂̐ڑ��őI������Ă���GGID���擾.
                �ʐM���m������Ă��Ȃ��󋵂ł� 0 ��Ԃ�.
                ���݂̒ʐM��Ԃ� WXC_GetUserBitmap() �֐��̕Ԃ�l�Ŕ���\.

  Arguments:    None.

  Returns:      ���݂̐ڑ��őI������Ă���GGID.
 *---------------------------------------------------------------------------*/
u32 WXC_GetCurrentGgid(void)
{
    u32     ggid = 0;
    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        WXCProtocolRegistry *current = WXC_GetCurrentBlock(work->protocol);
        if (current)
        {
            ggid = current->ggid;
            /* ANY �ł���Ό��o��������� GGID ��͕킷�� */
            if (ggid == WXC_GGID_COMMON_ANY)
            {
                ggid = work->protocol->target_ggid;
            }
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
    return ggid;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetOwnAid

  Description:  ���݂̐ڑ��ɂ����鎩�g��AID���擾.
                �ʐM���m������Ă��Ȃ��󋵂ł͕s��Ȓl��Ԃ�.
                ���݂̒ʐM��Ԃ� WXC_GetUserBitmap() �֐��̕Ԃ�l�Ŕ���\.

  Arguments:    None.

  Returns:      ���݂̐ڑ��ɂ����鎩�g��AID.
 *---------------------------------------------------------------------------*/
u16 WXC_GetOwnAid(void)
{
    return work->driver.own_aid;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetUserInfo

  Description:  ���ݐڑ����̃��[�U�����擾.
                �Ԃ����|�C���^���w�����e��ύX���Ă͂Ȃ�Ȃ�.

  Arguments:    aid           �����擾���郆�[�U��AID.

  Returns:      �w�肳�ꂽAID���L���Ȃ�΃��[�U���, �����łȂ���� NULL
 *---------------------------------------------------------------------------*/
const WXCUserInfo *WXC_GetUserInfo(u16 aid)
{
    const WXCUserInfo *ret = NULL;

    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        if ((aid <= WM_NUM_MAX_CHILD) && ((WXC_GetUserBitmap() & (1 << aid)) != 0))
        {
            ret = &work->user_info[aid];
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetChildMode

  Description:  ���C�����X���q�@���ł����N�������Ȃ��悤�ݒ�.
                ���̊֐��� WXC_Start �֐����O�ɌĂяo���K�v������.

  Arguments:    enable        �q�@���ł����N�������Ȃ��ꍇ�� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_SetChildMode(BOOL enable)
{
    SDK_ASSERT(WXC_GetStateCode() != WXC_STATE_ACTIVE);
    WXCi_SetChildMode(&work->scheduler, enable);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_AddData

  Description:  ���������u���b�N�f�[�^�����ɒǉ��Ńf�[�^��ݒ�

  Arguments:    send_buf      ���M�o�b�t�@.
                send_size     ���M�o�b�t�@�T�C�Y.
                recv_buf      ��M�o�b�t�@.
                recv_max      ��M�o�b�t�@�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_AddData(const void *send_buf, u32 send_size, void *recv_buf, u32 recv_max)
{
    WXC_AddBlockSequence(work->protocol, send_buf, send_size, recv_buf, recv_max);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_RegisterDataEx

  Description:  �����p�̃f�[�^��o�^����

  Arguments:    ggid          �o�^�f�[�^��GGID
                callback      ���[�U�[�ւ̃R�[���o�b�N�֐��i�f�[�^�����������ɃR�[���o�b�N�����j
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y
                type          ���ꂿ�����v���g�R���̎��(PIType)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_RegisterDataEx(u32 ggid, WXCCallback callback, u8 *send_ptr, u32 send_size, u8 *recv_ptr,
                        u32 recv_size, const char *type)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    /*
     * ���� GGID �͕����o�^�ł��Ȃ��悤�ɂ��Ă���.
     * (�q�@������ڑ����ɂǂ���g�p���Ă悢�����f�ł��Ȃ�����)
     */
    WXCProtocolRegistry *same_ggid = WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);
    if (same_ggid != NULL)
    {
        OS_TWarning("already registered same GGID!");
    }
    /* �����łȂ����, �󂢂Ă���u���b�N���g�p */
    else
    {
        WXCProtocolRegistry *p_data = WXC_FindNextBlock(work->protocol, NULL, 0, TRUE);
        /* �󂫂�������Ύ��s */
        if (!p_data)
        {
            OS_TPanic("no more memory to register data!");
        }
        else
        {
            /* �I�����ꂽ�v���g�R���ɂ���āA�o�^����ʐM�v���g�R����ς��� */
            WXCProtocolImpl *impl = WXC_FindProtocolImpl(type);
            if (!impl)
            {
                OS_TPanic("unknown protocol \"%s\"!", type);
            }
            WXC_InitProtocolRegistry(p_data, ggid, callback, impl);
            WXC_SetInitialExchangeBuffers(p_data, send_ptr, send_size, recv_ptr, recv_size);
        }
    }

    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetInitialData

  Description:  ����f�[�^�����Ŗ���g�p����f�[�^�u���b�N���w�肷��

  Arguments:    ggid          �o�^�f�[�^��GGID
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_SetInitialData(u32 ggid, u8 *send_ptr, u32 send_size, u8 *recv_ptr, u32 recv_size)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    /* �w��̃u���b�N������ */
    WXCProtocolRegistry *target = WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);
    if (target)
    {
        WXC_SetInitialExchangeBuffers(target, send_ptr, send_size, recv_ptr, recv_size);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_UnregisterData

  Description:  �����p�̃f�[�^��o�^����j������

  Arguments:    ggid          �j������u���b�N�Ɋ֘A�t����ꂽ GGID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_UnregisterData(u32 ggid)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    /* �w��̃u���b�N������ */
    WXCProtocolRegistry *target = WXC_FindNextBlock(work->protocol, NULL, ggid, TRUE);
    /* ���ʂ��ꂿ�����ʐM�d�l�̏ꍇ���܂߂Ă���1�񌟍� */
    if (!target)
    {
        target = WXC_FindNextBlock(work->protocol, NULL, (u32)(ggid | WXC_GGID_COMMON_BIT), TRUE);
    }
    if (target)
    {
        /* ���ݒʐM�Ɏg�p���ł���Ύ��s */
        if ((WXC_GetUserBitmap() != 0) && (target == WXC_GetCurrentBlock(work->protocol)))
        {
            OS_TWarning("specified data is now using.");
        }
        else
        {
            /* �f�[�^��������� */
            target->ggid = 0;
            target->callback = NULL;
            target->send.buffer = NULL;
            target->send.length = 0;
            target->recv.buffer = NULL;
            target->recv.length = 0;
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_SetSsid

  Description:  �q�@�Ƃ��Đڑ�����ۂ� SSID ��ݒ肷��.
                �����֐�.

  Arguments:    buffer        �ݒ肷SSID�f�[�^.
                length        SSID�f�[�^��.
                              WM_SIZE_CHILD_SSID �ȉ��ł���K�v������.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_SetSsid(const void *buffer, u32 length)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    WXC_SetDriverSsid(&work->driver, buffer, length);
    (void)OS_RestoreInterrupts(bak_cpsr);
}

