/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - libraries -
  File:     wxc_protocol_impl_wxc.c

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
#include <nitro/wxc/wxc_protocol_impl_wxc.h>


/*****************************************************************************/
/* declaration */

static void WXCi_BeaconSendHook(WXCProtocolContext * protocol, WMParentParam *p_param);
static BOOL WXCi_BeaconRecvHook(WXCProtocolContext * protocol, const WMBssDesc *p_desc);
static void WXCi_PacketSendHook(WXCProtocolContext * protocol, WXCPacketInfo * packet);
static BOOL WXCi_PacketRecvHook(WXCProtocolContext * protocol, const WXCPacketInfo * packet);
static void WXCi_InitSequence(WXCProtocolContext * protocol, u16 send_max, u16 recv_max);
static BOOL WXCi_AddData(WXCProtocolContext * protocol, const void *send_buf, u32 send_size,
                         void *recv_buf, u32 recv_max);
static BOOL WXCi_IsExecuting(WXCProtocolContext * protocol);


/*****************************************************************************/
/* variable */

/* WXC �v���g�R���p�̃��[�N�o�b�t�@ */
static WXCImplWorkWxc impl_wxc_work;

/* WXC �v���g�R���C���^�t�F�[�X */
static WXCProtocolImpl impl_wxc = {
    "WXC",
    WXCi_BeaconSendHook,
    WXCi_BeaconRecvHook,
    NULL,
    WXCi_PacketSendHook,
    WXCi_PacketRecvHook,
    WXCi_InitSequence,
    WXCi_AddData,
    WXCi_IsExecuting,
    &impl_wxc_work,
};


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXCi_GetProtocolImplWXC

  Description:  WXC���ꂿ�����ʐM�v���g�R���̃C���^�t�F�[�X���擾�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
WXCProtocolImpl* WXCi_GetProtocolImplWXC(void)
{
    return &impl_wxc;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_BeaconSendHook

  Description:  �r�[�R���X�V�^�C�~���O�ŌĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_param       ����̃r�[�R���Ɏg�p���� WMParentParam �\����.
                              ���̊֐��̓����ŕK�v�ɉ����ĕύX����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_BeaconSendHook(WXCProtocolContext * protocol, WMParentParam *p_param)
{
#pragma unused(protocol)
#pragma unused(p_param)

    /*
     * ���� WXC �ł͓��� UserGameInfo ���g�p���Ă��Ȃ�.
     * ����, GGID �̃l�b�g�}�X�N���⋤�� GGID ���[�h�ȂǓ������ꂽ�ꍇ
     * �K�v�ɉ����Ċg������\��.
     */
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_BeaconRecvHook

  Description:  �X�L�������ꂽ�e�r�[�R���ɑ΂��ČĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_desc        �X�L�������ꂽ WMBssDesc �\����

  Returns:      �ڑ��ΏۂƂ݂Ȃ��� TRUE ��, �����łȂ���� FALSE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL WXCi_BeaconRecvHook(WXCProtocolContext * protocol, const WMBssDesc *p_desc)
{
#pragma unused(protocol)
#pragma unused(p_desc)

    /* ���� WXC �ł� GGID �݂̂ŋ�ʂ���u���[�J�����[�h�v�̂ݓ��� */

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_PacketSendHook

  Description:  MP �p�P�b�g���M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ���M�p�P�b�g����ݒ肷�� WXCPacketInfo �|�C���^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_PacketSendHook(WXCProtocolContext * protocol, WXCPacketInfo * packet)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    WXCBlockHeader *p_header = (WXCBlockHeader *) packet->buffer;

    WXC_PACKET_LOG("--SEND:ACK=(%3d,%d,%04X),REQ=(%3d,%d,%04X)\n",
                   wxc_work->ack.phase, wxc_work->ack.command, wxc_work->ack.index,
                   wxc_work->req.phase, wxc_work->req.command, wxc_work->req.index);

    /* ���M�o�b�t�@�Ƀp�P�b�g�w�b�_������ݒ� */
    p_header->req = wxc_work->req;
    p_header->ack = wxc_work->ack;

    /* �t�F�[�Y�������ꍇ�̂݉����f�[�^���M */
    if (wxc_work->ack.phase == wxc_work->req.phase)
    {
        u8     *p_body = packet->buffer + sizeof(WXCBlockHeader);

        switch (wxc_work->ack.command)
        {
        case WXC_BLOCK_COMMAND_INIT:
            /* �������(�T�C�Y + �`�F�b�N�T��)�𑗐M */
            WXC_PACKET_LOG("       INIT(%6d)\n", protocol->send.length);
            *(u16 *)(p_body + 0) = (u16)protocol->send.length;
            *(u16 *)(p_body + 2) = protocol->send.checksum;
            break;
        case WXC_BLOCK_COMMAND_SEND:
            /* �Ō�ɗv�����ꂽ�C���f�b�N�X�𑗐M */
            {
                int     offset = (wxc_work->ack.index * wxc_work->send_unit);
                u32     len = (u32)(protocol->send.length - offset);
                if (len > wxc_work->send_unit)
                {
                    len = wxc_work->send_unit;
                }
                MI_CpuCopy8((const u8 *)protocol->send.buffer + offset, p_body, len);
            }
            break;
        }
    }

    /* �p�P�b�g�T�C�Y���w�� */
    packet->length = (u16)MATH_ROUNDUP(sizeof(WXCBlockHeader) + wxc_work->send_unit, 2);
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_MergeBlockData

  Description:  ��M�����u���b�N�f�[�^�f�Ђ�ۑ�.

  Arguments:    protocol      WXCProtocolContext �\����.
                index         ��M�f�[�^�̃C���f�b�N�X.
                src           ��M�f�[�^�o�b�t�@.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WXCi_MergeBlockData(WXCProtocolContext * protocol, int index, const void *src)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    if (index < wxc_work->recv_total)
    {
        u32    *bmp = wxc_work->recv_bitmap_buf + (index >> 5);
        u32     bit = (u32)(1 << (index & 31));
        if ((*bmp & bit) == 0)
        {
            int     offset = (index * wxc_work->recv_unit);
            u32     len = (u32)(protocol->recv.length - offset);
            if (len > wxc_work->recv_unit)
            {
                len = wxc_work->recv_unit;
            }
            /* ������ recv.buffer �� NULL ��������ۑ����Ȃ� */
            if (protocol->recv.buffer != NULL)
            {
                MI_CpuCopy8(src, (u8 *)protocol->recv.buffer + offset, len);
            }
            *bmp |= bit;
            /* ��M�����Ȃ� DONE */
            if (--wxc_work->recv_rest == 0)
            {
                wxc_work->req.command = WXC_BLOCK_COMMAND_DONE;
            }
            /* �����łȂ���Ζ���M�̃C���f�b�N�X������ */
            else
            {
                int     i;
                /* �O��v�������C���f�b�N�X����_�Ƃ��� */
                int     count = wxc_work->recent_index[0];
                int     last_count = count;
                if (last_count >= wxc_work->recv_total)
                {
                    last_count = (int)wxc_work->recv_total - 1;
                }
                for (;;)
                {
                    /* �I�[�ɒB������擪�փ��[�v */
                    if (++count >= wxc_work->recv_total)
                    {
                        count = 0;
                    }
                    /* ��������������ꍇ�͗�������đI�� */
                    if (count == last_count)
                    {
                        count = wxc_work->recent_index[WXC_RECENT_SENT_LIST_MAX - 1];
                        break;
                    }
                    /* ����M������ */
                    if ((*(wxc_work->recv_bitmap_buf + (count >> 5)) & (1 << (count & 31))) == 0)
                    {
                        /* �ŋߗv�������C���f�b�N�X�łȂ������� */
                        for (i = 0; i < WXC_RECENT_SENT_LIST_MAX; ++i)
                        {
                            if (count == wxc_work->recent_index[i])
                            {
                                break;
                            }
                        }
                        if (i >= WXC_RECENT_SENT_LIST_MAX)
                        {
                            break;
                        }
                    }
                }
                /* �C���f�b�N�X�v���������X�V */
                for (i = WXC_RECENT_SENT_LIST_MAX; --i > 0;)
                {
                    wxc_work->recent_index[i] = wxc_work->recent_index[i - 1];
                }
                wxc_work->recent_index[0] = (u16)count;
                wxc_work->req.index = wxc_work->recent_index[0];
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_PacketRecvHook

  Description:  MP �p�P�b�g��M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ��M�p�P�b�g�����Q�Ƃ��� WXCPacketInfo �|�C���^.

  Returns:      1��̃f�[�^���������������� TRUE.
 *---------------------------------------------------------------------------*/
BOOL WXCi_PacketRecvHook(WXCProtocolContext * protocol, const WXCPacketInfo * packet)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    int     ret = FALSE;

    WXCBlockHeader *p_header = (WXCBlockHeader *) packet->buffer;

    if (packet->length >= wxc_work->recv_unit)
    {
        WXC_PACKET_LOG("--RECV:REQ=(%3d,%d,%04X),ACK=(%3d,%d,%04X)\n",
                       p_header->req.phase, p_header->req.command, p_header->req.index,
                       p_header->ack.phase, p_header->ack.command, p_header->ack.index);

        /* �t�F�[�Y�������ꍇ�̂ݑ���̗v���f�[�^��ۑ� */
        if (p_header->req.phase == wxc_work->req.phase)
        {
            wxc_work->ack = p_header->req;
        }

        /* �t�F�[�Y�������ꍇ�̂ݑ���̉����f�[�^���Q�� */
        if (p_header->ack.phase == wxc_work->req.phase)
        {
            u8     *p_body = packet->buffer + sizeof(WXCBlockHeader);

            /* �R�}���h���Ƃ̎�M���� */
            switch (p_header->ack.command)
            {
            case WXC_BLOCK_COMMAND_QUIT:
                /* �v���g�R���̐ؒf */
                wxc_work->executing = FALSE;
                break;
            case WXC_BLOCK_COMMAND_INIT:
                /* �������(�T�C�Y + �`�F�b�N�T��)����M */
                protocol->recv.length = *(u16 *)(p_body + 0);
                protocol->recv.checksum = *(u16 *)(p_body + 2);
                wxc_work->recv_total =
                    (u16)((protocol->recv.length + wxc_work->recv_unit - 1) / wxc_work->recv_unit);
                wxc_work->recv_rest = wxc_work->recv_total;
                wxc_work->req.index = 0;
                /* �T�C�Y�� 0 �̏ꍇ�� WXC_BLOCK_COMMAND_DONE �Ɉڍs */
                if (wxc_work->recv_total > 0)
                {
                    wxc_work->req.command = WXC_BLOCK_COMMAND_SEND;
                }
                else
                {
                    wxc_work->req.command = WXC_BLOCK_COMMAND_DONE;
                }
                WXC_PACKET_LOG("       INIT(%6d)\n", protocol->recv.length);
                break;
            case WXC_BLOCK_COMMAND_SEND:
                /* ���ۂɉ������ꂽ��M�f�[�^���i�[ */
                WXCi_MergeBlockData(protocol, p_header->ack.index, p_body);
                break;
            }
        }

        /* �o���̃f�[�^�������� */
        if ((p_header->ack.phase == wxc_work->req.phase) &&
            (p_header->ack.command == WXC_BLOCK_COMMAND_DONE) &&
            (wxc_work->ack.command == WXC_BLOCK_COMMAND_DONE))
        {
            /* �f�[�^�����t�F�[�Y���C���N�������g���čď����� */
            ++wxc_work->req.phase;
            /* ���̂܂ܒʐM�𑱂���΂₪�� !executing �ɂȂ�悤�ɂ��Ă��� */
            wxc_work->req.command = WXC_BLOCK_COMMAND_QUIT;
            ret = TRUE;
        }
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_InitSequence

  Description:  WXC���C�u�����v���g�R�����ď���������֐�

  Arguments:    protocol      WXCProtocolContext �\����.
                send_max      ���M�p�P�b�g�ő�T�C�Y.
                recv_max      ��M�p�P�b�g�ő�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WXCi_InitSequence(WXCProtocolContext * protocol, u16 send_max, u16 recv_max)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    /* ����M�o�b�t�@�̏����� */
    protocol->send.buffer = NULL;
    protocol->send.length = 0;
    protocol->send.checksum = 0;
    protocol->recv.buffer = NULL;
    protocol->recv.length = 0;
    protocol->recv.buffer_max = 0;

    /* �u���b�N�]���̕����T�C�Y���v�Z */
    wxc_work->send_unit = (u16)(send_max - sizeof(WXCBlockHeader));
    wxc_work->recv_unit = (u16)(recv_max - sizeof(WXCBlockHeader));

    /* ���M�Ǘ����̏����� */
    {
        int     i;
        for (i = 0; i < WXC_RECENT_SENT_LIST_MAX; ++i)
        {
            wxc_work->recent_index[i] = 0;
        }
    }
    /* ��M�Ǘ����̏����� */
    wxc_work->req.phase = 0;
    wxc_work->recv_total = 0;

    /*
     * �Ȃ�� AddData() ����Ȃ��ŒʐM���J�n������
     * !IsExecuting() �֌������悤�ɏ��������Ă�������.
     */
    wxc_work->req.command = WXC_BLOCK_COMMAND_QUIT;
    /*
     * ����������ւ� ack �ɂ���Ȃ��Ƃ������
     * �������ɏI�����Ă��܂��̂�, ������(IDLE) �Ƃ��Ă�������.
     */
    wxc_work->ack.phase = 0;
    wxc_work->ack.command = WXC_BLOCK_COMMAND_IDLE;

    MI_CpuClear32(wxc_work->recv_bitmap_buf, sizeof(wxc_work->recv_bitmap_buf));

    wxc_work->executing = TRUE;

}

/*---------------------------------------------------------------------------*
  Name:         WXCi_AddData

  Description:  �u���b�N�f�[�^������ݒ肷��֐��B

  Arguments:    protocol      WXCProtocolContext �\����.
                send_buf      ���M�o�b�t�@.
                send_size     ���M�o�b�t�@�T�C�Y.
                recv_buf      ��M�o�b�t�@.
                recv_max      ��M�o�b�t�@�T�C�Y.

  Returns:      �o�^�ł���󋵂Ȃ�A�ݒ肵���� TRUE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL WXCi_AddData(WXCProtocolContext * protocol, const void *send_buf, u32 send_size,
                  void *recv_buf, u32 recv_max)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    if (wxc_work->req.command == WXC_BLOCK_COMMAND_QUIT)
    {
        wxc_work->req.command = WXC_BLOCK_COMMAND_INIT;

        protocol->send.buffer = (void *)send_buf;
        protocol->send.length = (u16)send_size;
        protocol->send.checksum = MATH_CalcChecksum8(send_buf, send_size);

        protocol->recv.buffer = recv_buf;
        protocol->recv.buffer_max = (u16)recv_max;
        MI_CpuClear32(wxc_work->recv_bitmap_buf, sizeof(wxc_work->recv_bitmap_buf));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_IsExecuting

  Description:  ���݃f�[�^���������𔻒�.

  Arguments:    None.

  Returns:      ���݃f�[�^�������Ȃ�� TRUE.
 *---------------------------------------------------------------------------*/
BOOL WXCi_IsExecuting(WXCProtocolContext * protocol)
{
    WXCImplWorkWxc *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    return wxc_work->executing;
}

