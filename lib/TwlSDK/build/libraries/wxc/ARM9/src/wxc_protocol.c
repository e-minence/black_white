/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - libraries -
  File:     wxc_protocol.c

  Copyright 2005-2009 Nintendo.  All rights reserved.

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

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>


/*****************************************************************************/
/* variable */

/* ���݂܂łɒǉ�����Ă��邷�ꂿ�����ʐM�v���g�R�� */
static WXCProtocolImpl *impl_list;


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXC_InitProtocol

  Description:  WXC���C�u�����v���g�R����������

  Arguments:    protocol      WXCProtocolContext �\����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_InitProtocol(WXCProtocolContext * protocol)
{
    protocol->current_block = NULL;
    MI_CpuClear32(protocol, sizeof(protocol));
}

/*---------------------------------------------------------------------------*
  Name:         WXC_InstallProtocolImpl

  Description:  �I���\�ȐV�����v���g�R����ǉ�

  Arguments:    impl          WXCProtocolImpl�\���̂ւ̃|�C���^.
                              ���̍\���̂̓��C�u�����I�����܂œ����Ŏg�p�����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_InstallProtocolImpl(WXCProtocolImpl * impl)
{
    WXCProtocolImpl *p = impl_list;
    if (!p)
    {
        impl_list = impl;
        impl->next = NULL;
    }
    else
    {
        for (;; p = p->next)
        {
            if (p == impl)
            {
                break;
            }
            else if (!p->next)
            {
                p->next = impl;
                impl->next = NULL;
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXC_FindProtocolImpl

  Description:  �w��̖��O�����v���g�R��������

  Arguments:    name          �v���g�R����.

  Returns:      �w��̖��O�����v���g�R�������݂���΂��̃|�C���^.
 *---------------------------------------------------------------------------*/
WXCProtocolImpl *WXC_FindProtocolImpl(const char *name)
{
    WXCProtocolImpl *p = impl_list;
    for (; p; p = p->next)
    {
        if (STD_CompareString(name, p->name) == 0)
        {
            break;
        }
    }
    return p;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_ResetSequence

  Description:  WXC���C�u�����v���g�R�����ď�����

  Arguments:    protocol      WXCProtocolContext �\����.
                send_max      ���M�p�P�b�g�ő�T�C�Y.
                recv_max      ��M�p�P�b�g�ő�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_ResetSequence(WXCProtocolContext * protocol, u16 send_max, u16 recv_max)
{
    WXC_GetCurrentBlockImpl(protocol)->Init(protocol, send_max, recv_max);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_AddBlockSequence

  Description:  �u���b�N�f�[�^������ݒ�

  Arguments:    protocol      WXCProtocolContext �\����.
                send_buf      ���M�o�b�t�@.
                send_size     ���M�o�b�t�@�T�C�Y.
                recv_buf      ��M�o�b�t�@.
                recv_max      ��M�o�b�t�@�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_AddBlockSequence(WXCProtocolContext * protocol,
                          const void *send_buf, u32 send_size, void *recv_buf, u32 recv_max)
{
    int     result;
    result =
        WXC_GetCurrentBlockImpl(protocol)->AddData(protocol, send_buf, send_size, recv_buf,
                                                   recv_max);
    SDK_TASSERTMSG(result, "sequence is now busy.");
}

/*---------------------------------------------------------------------------*
  Name:         WXC_FindNextBlock

  Description:  GGID ���w�肵�ău���b�N�f�[�^������.

  Arguments:    protocol      WXCProtocolContext �\����.
                from          �����J�n�ʒu.
                              NULL �Ȃ�擪���猟����, �����łȂ����
                              �w�肳�ꂽ�u���b�N�̎����猟������.
                ggid          ��������u���b�N�f�[�^�Ɋ֘A�t����ꂽ GGID.
                              0 �Ȃ�󂫃u���b�N������.
                match         ��������.
                              TRUE �Ȃ� GGID �̍��v������̂������ΏۂƂ�,
                              FALSE �Ȃ� ���v���Ȃ����̂�ΏۂƂ���.

  Returns:      ���v����u���b�N�f�[�^�����݂���΂��̃|�C���^��,
                ������� NULL ��Ԃ�.
 *---------------------------------------------------------------------------*/
WXCProtocolRegistry *WXC_FindNextBlock(WXCProtocolContext * protocol,
                                       const WXCProtocolRegistry * from, u32 ggid, BOOL match)
{
    WXCProtocolRegistry *target;

    /* NULL �Ȃ�擪���猟�� */
    if (!from)
    {
        from = &protocol->data_array[WXC_REGISTER_DATA_MAX - 1];
    }

    target = (WXCProtocolRegistry *) from;
    for (;;)
    {
        BOOL    eq;
        /* ���̗v�f���z��̏I�[�ɒB�����烋�[�v */
        if (++target >= &protocol->data_array[WXC_REGISTER_DATA_MAX])
        {
            target = &protocol->data_array[0];
        }
        /* ���������̈�v����u���b�N�����݂���ΏI�� */
        eq = (target->ggid == ggid);
        if ((match && eq) || (!match && !eq))
        {
            break;
        }
        /* �S�v�f�������ς݂Ȃ�I�� */
        if (target == from)
        {
            target = NULL;
            break;
        }
    }
    return target;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_BeaconSendHook

  Description:  �r�[�R���X�V�^�C�~���O�ŌĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_param       ����̃r�[�R���Ɏg�p���� WMParentParam �\����.
                              ���̊֐��̓����ŕK�v�ɉ����ĕύX����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_BeaconSendHook(WXCProtocolContext * protocol, WMParentParam *p_param)
{
    WXC_GetCurrentBlockImpl(protocol)->BeaconSend(protocol, p_param);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_BeaconRecvHook

  Description:  �X�L�������ꂽ�e�r�[�R���ɑ΂��ČĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_desc        �X�L�������ꂽ WMBssDesc �\����

  Returns:      �ڑ��ΏۂƂ݂Ȃ��� TRUE ��, �����łȂ���� FALSE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL WXC_BeaconRecvHook(WXCProtocolContext * protocol, const WMBssDesc *p_desc)
{
    BOOL    ret = FALSE;

    /* GGID 0 �͋󂫃u���b�N�Ƃ��Ĉ�����̂ŏ��O */
    u32 ggid = p_desc->gameInfo.ggid;
    if (ggid != 0)
    {
        /* �o�^�ς݂� GGID �����v������̂����� */
        WXCProtocolRegistry *found = NULL;
        int i;
        for (i = 0; i < WXC_REGISTER_DATA_MAX; ++i)
        {
            WXCProtocolRegistry *p = &protocol->data_array[i];
            /* �����Ɉ�v */
            if (p->ggid == ggid)
            {
                found = p;
                break;
            }
            /* �o���Ƃ����ʂ��ꂿ���� */
            else if (WXC_IsCommonGgid(ggid) && WXC_IsCommonGgid(p->ggid))
            {
                /* �����ꂩ�����p���ł���Έ�v */
                const BOOL is_target_any = (ggid == WXC_GGID_COMMON_PARENT);
                const BOOL is_current_any = (p->ggid == WXC_GGID_COMMON_ANY);
                if (is_target_any)
                {
                    if (!is_current_any)
                    {
                        ggid = p->ggid;
                        found = p;
                        break;
                    }
                }
                else
                {
                    if (is_current_any)
                    {
                        found = p;
                        break;
                    }
                }
            }
        }
        /* ��v����΃v���g�R�����ł����菈�����s�� */
        if (found)
        {
            ret = found->impl->BeaconRecv(protocol, p_desc);
            /* GGID ���v���g�R������v����ΑI����؂�ւ��� */
            if (ret)
            {
                WXC_SetCurrentBlock(protocol, found);
                protocol->target_ggid = ggid;
            }
        }
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_PacketSendHook

  Description:  MP �p�P�b�g���M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ���M�p�P�b�g����ݒ肷�� WXCPacketInfo �|�C���^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_PacketSendHook(WXCProtocolContext * protocol, WXCPacketInfo * packet)
{
    WXC_GetCurrentBlockImpl(protocol)->PacketSend(protocol, packet);
}

/*---------------------------------------------------------------------------*
  Name:         WXC_PacketRecvHook

  Description:  MP �p�P�b�g��M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ��M�p�P�b�g�����Q�Ƃ��� WXCPacketInfo �|�C���^.

  Returns:      1��̃f�[�^���������������� TRUE.
 *---------------------------------------------------------------------------*/
BOOL WXC_PacketRecvHook(WXCProtocolContext * protocol, const WXCPacketInfo * packet)
{
    int     ret = FALSE;

    ret = WXC_GetCurrentBlockImpl(protocol)->PacketRecv(protocol, packet);
    /*
     * 1��̃f�[�^����������.
     * �����ł�, ���̂܂ܒʐM�𑱂���΂₪�� !IsExecuting() �ɂȂ�͂�.
     */
    if (ret)
    {
        /* �]�����������[�U�R�[���o�b�N�֒ʒm */
        WXCCallback callback = protocol->current_block->callback;
        if (callback)
        {
            (*callback) (WXC_STATE_EXCHANGE_DONE, &protocol->recv);
        }
        /*
         * �R�[���o�b�N���� AddData() ����Ă����, �ʐM�p������͂�.
         * �����łȂ���ΑO�q�̒ʂ� !IsExecuting() �ɂȂ�悤�ɏ������i��.
         */
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_ConnectHook

  Description:  �ʐM�Ώۂ̐ڑ����m���ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                bitmap        �ڑ����ꂽ�ʐM��� AID �r�b�g�}�b�v.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_ConnectHook(WXCProtocolContext * protocol, u16 bitmap)
{
#pragma unused(protocol)
#pragma unused(bitmap)
}

/*---------------------------------------------------------------------------*
  Name:         WXC_DisconnectHook

  Description:  �ʐM�Ώۂ̐ؒf���m���ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                bitmap        �ؒf���ꂽ�ʐM��� AID �r�b�g�}�b�v.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_DisconnectHook(WXCProtocolContext * protocol, u16 bitmap)
{
#pragma unused(protocol)
#pragma unused(bitmap)
}

/*---------------------------------------------------------------------------*
  Name:         WXC_CallPreConnectHook

  Description:  �ʐM�Ώۂւ̐ڑ��O�ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_desc        �ڑ��Ώۂ� WMBssDesc �\����
                ssid          �ݒ肷�邽�߂�SSID���i�[����o�b�t�@.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_CallPreConnectHook(WXCProtocolContext * protocol, WMBssDesc *p_desc, u8 *ssid)
{
    WXCProtocolImpl * const impl = WXC_GetCurrentBlockImpl(protocol);
    if (impl->PreConnectHook)
    {
        impl->PreConnectHook(protocol, p_desc, ssid);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXC_InitProtocolRegistry

  Description:  �w��̓o�^�\���̂� GGID �ƃR�[���o�b�N���֘A�t����

  Arguments:    p_data        �o�^�Ɏg�p���� WXCProtocolRegistry �\����
                ggid          �ݒ肷�� GGID
                callback      ���[�U�[�ւ̃R�[���o�b�N�֐�
                              (NULL �̏ꍇ�͐ݒ����)
                impl          �̗p����ʐM�v���g�R��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_InitProtocolRegistry(WXCProtocolRegistry * p_data, u32 ggid, WXCCallback callback,
                              WXCProtocolImpl * impl)
{
    p_data->ggid = ggid;
    p_data->callback = callback;
    p_data->impl = impl;
    p_data->send.buffer = NULL;
    p_data->send.length = 0;
    p_data->recv.buffer = NULL;
    p_data->recv.length = 0;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetInitialExchangeBuffers

  Description:  ����̃f�[�^�����Ŗ��񎩓��I�Ɏg�p����f�[�^��ݒ�

  Arguments:    p_data        �o�^�Ɏg�p���� WXCProtocolRegistry �\����
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXC_SetInitialExchangeBuffers(WXCProtocolRegistry * p_data, u8 *send_ptr, u32 send_size,
                                   u8 *recv_ptr, u32 recv_size)
{
    p_data->send.buffer = send_ptr;
    p_data->send.length = (u32)send_size;

    p_data->recv.buffer = recv_ptr;
    p_data->recv.length = (u32)recv_size;
}
