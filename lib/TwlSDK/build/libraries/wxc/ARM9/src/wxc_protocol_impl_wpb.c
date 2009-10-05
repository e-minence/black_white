/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - libraries -
  File:     wxc_protocol_impl_wpb.c

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
#include <nitro/wxc/wxc_protocol_impl_wpb.h>

#include <nitro/wxc.h>                 /* use WXC_IsParentMode() */


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

static void pass_copy_to_structure(u8 *buf, PassBuffer * pb);


/*****************************************************************************/
/* variable */

static WXCImplWorkWpb impl_work_wpb;

WXCProtocolImpl impl_wpb = {
    "WPB",
    WXCi_BeaconSendHook,
    WXCi_BeaconRecvHook,
    NULL,
    WXCi_PacketSendHook,
    WXCi_PacketRecvHook,
    WXCi_InitSequence,
    WXCi_AddData,
    WXCi_IsExecuting,
    &impl_work_wpb,
};


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         WXCi_GetProtocolImplWPB

  Description:  WPB���ꂿ�����ʐM�v���g�R���̃C���^�t�F�[�X���擾�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
WXCProtocolImpl* WXCi_GetProtocolImplWPB(void)
{
    return &impl_wpb;
}

/*---------------------------------------------------------------------------*
  Name:         PrintPassBuffer

  Description:  ��M���e���f�o�b�O�o��.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PrintPassBuffer(void *buf, const char *message)
{
#ifdef SDK_FINALROM
#pragma unused(message)
#endif

    static PassBuffer pass;
    pass_copy_to_structure((u8 *)buf, &pass);

    OS_TPrintf("%s(req=%02X,res=%02X)\n", message, pass.req_count, pass.res_count);
    if (pass.req_count == pass.req_count)
    {
        pass.req_count = pass.req_count;
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_data_init_recv_bitmap

  Description:  ��M�����r�b�g�}�b�v�̏�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void pass_data_init_recv_bitmap(PassCtrl * pass_ctrl)
{
    pass_ctrl->recv_bitmap = 0;
    pass_ctrl->recv_bitmap_index = 0;
}

/*---------------------------------------------------------------------------*
  Name:         pass_InitBuf

  Description:  PassBuffer�\���̂̏�����

  Arguments:    pb - PassBuffer�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void pass_InitBuf(PassBuffer * pb)
{
    pb->res_count = REQUEST_NONE;
    pb->req_count = 0;
    MI_CpuClear8(pb->buf, PASS_BUFFER_SIZE);
}

/*---------------------------------------------------------------------------*
  Name:         pass_ResetData

  Description:  �ɌĂяo�����֐��B

  Arguments:    None.

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void pass_ResetData(WXCImplWorkWpb * wxc_work)
{
    PassCtrl *pc;
    pc = &wxc_work->pass_ctrl;

    pass_InitBuf(&(pc->send_buf));
    pass_InitBuf(&(pc->recv_buf));
    pass_data_init_recv_bitmap(pc);
    pc->pre_send_count = REQUEST_NONE;
    pc->reset_done = TRUE;
    pc->send_done = FALSE;
    pc->recv_done = FALSE;
    pc->hardware_buffer_count = 0;
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

    /* gameInfo�ɋ��ʂ��ꂿ�������𖄂ߍ��ޏ���
     *p_param->userGameInfo �� p_param->userGameInfoLength ����������
     */

    static u16 gameInfo[2] ATTRIBUTE_ALIGN(32);
    u16     gameInfoLength = sizeof gameInfo;

    /* gameInfo �̐擪 4 �o�C�g�͗\�� (0) */
    MI_CpuClear16(gameInfo, sizeof gameInfo);

    if (gameInfoLength > WM_SIZE_USER_GAMEINFO)
    {
        SDK_ASSERT(FALSE);
    }

    if (gameInfoLength)
    {
        p_param->userGameInfo = gameInfo;
    }
    p_param->userGameInfoLength = gameInfoLength;
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

    BOOL    ret = FALSE;

    /* �����ŋ��ʂ��ꂿ���������f   
     * userGameInfo �� 4 �o�C�g�ȏ゠��A�ŏ��� 4 �o�C�g�� u32 �Ƃ���
     * �ŏ�ʃr�b�g�� 0 �Ȃ狤�ʂ���Ⴂ�ɑΉ�
     */

    if (p_desc->gameInfoLength >=
        (char *)&p_desc->gameInfo.ggid - (char *)&p_desc->gameInfo
        + sizeof p_desc->gameInfo.ggid &&
        p_desc->gameInfo.userGameInfoLength >= 4 &&
        (*(u32 *)p_desc->gameInfo.userGameInfo & 1 << 31) == 0)
    {
        ret = TRUE;
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         pass_data_get_recv_bitmap

  Description:  ��M�����r�b�g�}�b�v���`�F�b�N����

  Arguments:    seq_no - �V�[�P���X�ԍ�

  Returns:      None.
 *---------------------------------------------------------------------------*/
static BOOL pass_data_get_recv_bitmap(WXCProtocolContext * protocol, int seq_no)
{
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pass_ctrl = &wxc_work->pass_ctrl;

    if (seq_no < pass_ctrl->recv_bitmap_index)
    {
        return TRUE;
    }
    if (seq_no >= pass_ctrl->recv_bitmap_index + 32)
    {
        return FALSE;
    }
    if (pass_ctrl->recv_bitmap & 1 << seq_no % 32)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_data_get_next_count

  Description:  ���ɑ���ɗv������V�[�P���X�ԍ��𓾂�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static u16 pass_data_get_next_count(WXCProtocolContext * protocol)
{
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pc = &wxc_work->pass_ctrl;

    int     count;

    if (pc->recv_bitmap_index >= wxc_work->my_pass_data.total_count)
    {
        return REQUEST_DONE;           /* ���ׂĎ�M�ς� */
    }
    count = pc->pre_send_count;
    for (;;)
    {
        count++;
        if (count >= wxc_work->my_pass_data.total_count || count >= pc->recv_bitmap_index + 32)
        {
            count = pc->recv_bitmap_index;
        }
        if (!pass_data_get_recv_bitmap(protocol, count))
        {
            pc->pre_send_count = count;
            return (u16)count;
        }
        if (count == pc->pre_send_count)
        {
            /* �����ɗ��邱�Ƃ͂Ȃ��͂� */
            OS_TPrintf("Error ! %d %d %d %08X\n", pc->pre_send_count, pc->recv_bitmap_index,
                       wxc_work->my_pass_data.total_count, pc->recv_bitmap);
            return REQUEST_DONE;       /* ���ׂĎ�M�ς� */
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_copy_to_buffer

  Description:  PassBuffer�\���̂���WM���M�o�b�t�@�ɃR�s�[����

  Arguments:    pb  - PassBuffer�\����
                buf - WM���M�o�b�t�@

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void pass_copy_to_buffer(PassBuffer * pb, u8 *buf)
{
    int     i;

    *buf++ = (u8)(((pb->req_count) >> 8) & 0xff);       /* HI */
    *buf++ = (u8)(pb->req_count & 0xff);        /* LO */

    *buf++ = (u8)(((pb->res_count) >> 8) & 0xff);       /* HI */
    *buf++ = (u8)(pb->res_count & 0xff);        /* LO */

    for (i = 0; i < PASS_BUFFER_SIZE; i++)
    {
        *buf++ = pb->buf[i];
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_DataToBuf

  Description:  ���[�U�[���M�o�b�t�@����PassBuffer�\���̂ɃR�s�[����

  Arguments:    seq_no - �V�[�P���X�ԍ�
                pb     - PassBuffer�\����
                buf    - ���[�U�[���M�o�b�t�@

  Returns:      None.    {TRUE, TRUE, FALSE, TRUE},

 *---------------------------------------------------------------------------*/
static void pass_DataToBuf(WXCProtocolContext * protocol, int seq_no, PassBuffer * pb,
                           PassData * pd)
{
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pass_ctrl = &wxc_work->pass_ctrl;

    const u8 *src;
    u8     *dest;

    pb->res_count = (u16)seq_no;

    if (seq_no != REQUEST_DONE && seq_no != REQUEST_NONE && seq_no != REQUEST_BYE)
    {
        src = pd->user_send_data + (seq_no * wxc_work->send_unit);
        dest = pb->buf;
        if (seq_no == wxc_work->my_pass_data.total_count - 1)
        {
            int     mod = wxc_work->my_pass_data.size % wxc_work->send_unit;
            if (mod)
            {
                MI_CpuCopy8(src, dest, (u32)mod);
            }
            else
            {
                MI_CpuCopy8(src, dest, wxc_work->send_unit);
            }
        }
        else
        {
            MI_CpuCopy8(src, dest, wxc_work->send_unit);
        }
    }
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
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pass_ctrl = &wxc_work->pass_ctrl;

    /* WPB �͐e�@�Ƃ��Ďq�@�ƒʐM����ꍇ�ƁA�q�@�Ƃ��Đe�@�ƒʐM����ꍇ�ŏ������قȂ� */
    if (WXC_IsParentMode())
    {
        int     send_size = 0;
        int     send_buf_count;
        u8     *send_buf = packet->buffer;

        /* ���ǂ̃��N�G�X�g�ԍ��̃Z�b�g */
        if (pass_ctrl->recv_done == TRUE)
        {
            if (pass_ctrl->reset_done)
            {
                pass_ctrl->send_buf.req_count = REQUEST_DONE;
            }
            else
            {
                pass_ctrl->send_buf.req_count = REQUEST_BYE;
            }
        }
        else
        {
            pass_ctrl->send_buf.req_count = pass_data_get_next_count(protocol);
            if (pass_ctrl->send_buf.req_count == REQUEST_DONE)
            {
                pass_ctrl->recv_done = TRUE;
            }
        }

        /* ����̃��N�G�X�g�ɑ΂���f�[�^�̃Z�b�g */
        if (pass_ctrl->send_done)
        {
            send_buf_count = REQUEST_NONE;
        }
        else
        {
            send_buf_count = pass_ctrl->recv_buf.req_count;
        }
        pass_DataToBuf(protocol, send_buf_count, &(pass_ctrl->send_buf), &wxc_work->my_pass_data);

        /* send_buf���瑗�M�o�b�t�@�ɃR�s�[ */
        pass_copy_to_buffer(&(pass_ctrl->send_buf), send_buf);

#ifdef DEBUG
        OS_TPrintf("parent send->%x req->%x\n", send_buf_count, pass_ctrl->send_buf.req_count);
#endif
        send_size = PASS_PACKET_SIZE;

        /* �p�P�b�g�T�C�Y���w�� */
        packet->length = (u16)MATH_ROUNDUP(4 + wxc_work->send_unit, 2);

#ifdef DEBUG
        PrintPassBuffer(packet->buffer, "parent send");
#endif
    }
    else
    {
        int     peer_request;

        /* ���ǂ̃��N�G�X�g�ԍ����Z�b�g */
        if (pass_ctrl->recv_done == TRUE)
        {
            if (pass_ctrl->reset_done)
            {
                pass_ctrl->send_buf.req_count = REQUEST_DONE;
            }
            else
            {
                pass_ctrl->send_buf.req_count = REQUEST_BYE;
            }
        }
        else
        {
            pass_ctrl->send_buf.req_count = pass_data_get_next_count(protocol);
            if (pass_ctrl->send_buf.req_count == REQUEST_DONE)
            {
                pass_ctrl->recv_done = TRUE;
            }
        }

        /* ����ǂ̃��N�G�X�g�ԍ��ɑ΂��ăf�[�^���Z�b�g */
        peer_request = (int)(pass_ctrl->recv_buf.req_count);
        pass_DataToBuf(protocol, peer_request, &(pass_ctrl->send_buf), &wxc_work->my_pass_data);

        /* send_buf���瑗�M�o�b�t�@�ɃR�s�[ */
        pass_copy_to_buffer(&(pass_ctrl->send_buf), packet->buffer);

#ifdef DEBUG
        OS_TPrintf("child send->%x req->%x\n", peer_request, pass_ctrl.send_buf.req_count);
#endif
        /* �p�P�b�g�T�C�Y���w�� */
        packet->length = (u16)MATH_ROUNDUP(4 + wxc_work->send_unit, 2);
#ifdef DEBUG
        PrintPassBuffer(packet->buffer, "child send");
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_copy_to_structure

  Description:  WM��M�o�b�t�@����PassBuffer�\���̂ɃR�s�[����

  Arguments:    buf - WM��M�o�b�t�@
                pb  - PassBuffer�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void pass_copy_to_structure(u8 *buf, PassBuffer * pb)
{
    int     i;

    pb->req_count = (u16)(((u32)(*buf++)) << 8);        /* HI */
    pb->req_count += (u16)(*buf++);    /* LO */

    pb->res_count = (u16)(((u32)(*buf++)) << 8);        /* HI */
    pb->res_count += (u16)(*buf++);    /* LO */

    for (i = 0; i < PASS_BUFFER_SIZE; i++)
    {
        pb->buf[i] = *buf++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         pass_data_set_recv_bitmap

  Description:  ��M�����r�b�g�}�b�v���Z�b�g����

  Arguments:    aid    - AID(AID���ƂɎ�M�o�b�t�@���Ǘ����Ă��邽��)
                seq_no - �V�[�P���X�ԍ�

  Returns:      BOOL   - FALSE/���łɃ`�F�b�N�ς�
 *---------------------------------------------------------------------------*/
static BOOL pass_data_set_recv_bitmap(WXCProtocolContext * protocol, int seq_no)
{
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pass_ctrl = &wxc_work->pass_ctrl;

    if (seq_no < pass_ctrl->recv_bitmap_index)
    {
        return FALSE;
    }
    if (seq_no >= pass_ctrl->recv_bitmap_index + 32)
    {
        return FALSE;
    }
    if (pass_ctrl->recv_bitmap & 1 << seq_no % 32)
    {
        return FALSE;
    }
    pass_ctrl->recv_bitmap |= 1 << seq_no % 32;
    while (pass_ctrl->recv_bitmap & 1 << pass_ctrl->recv_bitmap_index % 32)
    {
        pass_ctrl->recv_bitmap &= ~(1 << pass_ctrl->recv_bitmap_index++ % 32);
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         pass_BufToData

  Description:  PassBuffer�\���̂��烆�[�U�[��M�o�b�t�@�ɃR�s�[����

  Arguments:    pb  - PassBuffer�\����
                buf - ���[�U�[��M�o�b�t�@

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void pass_BufToData(WXCProtocolContext * protocol, PassBuffer * pb, PassCtrl * pctrl)
{
    WXCImplWorkWpb *wxc_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    int     res_count;
    u8     *src, *dest;

    res_count = (int)pb->res_count;
    src = pb->buf;

    if (pctrl->user_recv_buffer == NULL)
    {
        return;
    }

    dest = pctrl->user_recv_buffer + (res_count * PASS_BUFFER_SIZE);
    if (res_count == wxc_work->my_pass_data.total_count - 1)
    {
        int     mod = wxc_work->my_pass_data.size % wxc_work->recv_unit;
        if (mod)
        {
            MI_CpuCopy8(src, dest, (u32)mod);
        }
        else
        {
            MI_CpuCopy8(src, dest, wxc_work->recv_unit);
        }
    }
    else
    {
        MI_CpuCopy8(src, dest, wxc_work->recv_unit);
    }
}

static void disconnect_callback(PassCtrl * pass_ctrl)
{
    if (!(pass_ctrl->reset_done == FALSE && pass_ctrl->recv_done))
    {
        pass_ctrl->reset_done = FALSE;
        pass_ctrl->recv_done = TRUE;
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
    WXCImplWorkWpb *wpb_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;
    PassCtrl *pass_ctrl = &wpb_work->pass_ctrl;

    /* WPB �͐e�@�Ƃ��Ďq�@�ƒʐM����ꍇ�ƁA�q�@�Ƃ��Đe�@�ƒʐM����ꍇ�ŏ������قȂ� */
    if (WXC_IsParentMode())
    {
        if (packet->buffer == NULL || packet->length == 0)
        {
            return FALSE;              /* �ʐM���p������ */
        }

        PrintPassBuffer(packet->buffer, "parent recv");

        /* �܂���M�o�b�t�@����pass_recv_buf�ɃR�s�[ */
        pass_copy_to_structure(((u8 *)packet->buffer), &(pass_ctrl->recv_buf));
        if (pass_ctrl->recv_buf.req_count == REQUEST_BYE)
        {
#ifdef DEBUG
            OS_TPrintf("parent : get REQUEST_BYE\n");
#endif
            wpb_work->executing = FALSE;        /* �ʐM��ؒf���� */
            return TRUE;
        }
        if (pass_ctrl->reset_done == TRUE)
        {
            if (pass_ctrl->recv_done == FALSE)
            {
#ifdef DEBUG
                OS_TPrintf("parent recv->%x\n", pass_ctrl->recv_buf.res_count);
#endif
                if (pass_ctrl->recv_buf.res_count < wpb_work->my_pass_data.total_count)
                {
                    /* ��M�����Ƀ`�F�b�N������ */
                    if (TRUE == pass_data_set_recv_bitmap(protocol, pass_ctrl->recv_buf.res_count))
                    {
                        /* ��M�f�[�^���Z�[�u */
                        pass_BufToData(protocol, &(pass_ctrl->recv_buf), pass_ctrl);
                    }
                }
            }
            else
            {
                if (pass_ctrl->recv_buf.req_count == REQUEST_DONE)
                {
                    pass_ctrl->send_done = TRUE;
                }
                if (pass_ctrl->send_done == TRUE)
                {

                    /* ����������M�f�[�^�I���̏ꍇ */
                    if (pass_ctrl->hardware_buffer_count < (HARDWARE_BUFFER_DUMMY_COUNT * 2))
                    {
                        /* �S�񂭂�܂ŕۗ� */
                        pass_ctrl->hardware_buffer_count++;
                        return FALSE;  /* �ʐM���p������ */
                    }

                    pass_ctrl->reset_done = FALSE;

                    /* ���� */
                    return TRUE;
                }
            }
        }
        return FALSE;                  /* �ʐM���p������ */
    }
    else
    {
        PrintPassBuffer(packet->buffer, "child recv");

        /* �e�@�Ƃ��Ďq�@�ƒʐM����v���g�R�� */
        if (packet->buffer == NULL || packet->length == 0)
        {
            return FALSE;              /* �ʐM���p������ */
        }

#ifdef DEBUG
        OS_TPrintf("child recv->%x\n", pass_ctrl->recv_buf.res_count);
#endif

        /* �܂���M�o�b�t�@����pass_recv_buf�ɃR�s�[ */
        pass_copy_to_structure(((u8 *)packet->buffer), &(pass_ctrl->recv_buf));
        if (pass_ctrl->recv_buf.req_count == REQUEST_BYE)
        {
#ifdef DEBUG
            OS_TPrintf("child: get REQUEST_BYE\n");
#endif
            wpb_work->executing = FALSE;        /* �ʐM��ؒf���� */
            return TRUE;
        }
        if (pass_ctrl->reset_done == TRUE)
        {
            if (packet->length < PASS_PACKET_SIZE)
            {
                return FALSE;          /* �ʐM���p������ */
            }
            if (pass_ctrl->recv_done == FALSE)
            {
                if (pass_ctrl->recv_buf.res_count < wpb_work->my_pass_data.total_count)
                {
                    /* ��M�����Ƀ`�F�b�N������ */
                    if (TRUE == pass_data_set_recv_bitmap(protocol, pass_ctrl->recv_buf.res_count))
                    {
                        pass_BufToData(protocol, &(pass_ctrl->recv_buf), pass_ctrl);
                    }
                }
            }
            else
            {
                if (pass_ctrl->recv_buf.req_count == REQUEST_DONE)
                {
                    pass_ctrl->send_done = TRUE;
                }
                if (pass_ctrl->send_done == TRUE)
                {

                    /* ���ǂ����M�f�[�^�I���̏ꍇ */
                    if (pass_ctrl->hardware_buffer_count < HARDWARE_BUFFER_DUMMY_COUNT)
                    {
                        /* �Q�񂭂�܂ŕۗ�
                           pass_ctrl->hardware_buffer_count++;
                           return FALSE;        /* �ʐM���p������ */
                    }

                    pass_ctrl->reset_done = FALSE;

                    /* ���� */
                    return TRUE;
                }
            }
        }
        return FALSE;                  /* �ʐM���p������ */
    }
    return FALSE;
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
    WXCImplWorkWpb *wpb_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    /* 4 �� WPB �p�P�b�g�w�b�_�̒���.
     * ���̃v���g�R���ɓK�p (WPB �� 512byte)
     * �����ł́AParentParam �̒l�����������Ȃ��悤�ɒ���!!
     */
    wpb_work->send_unit = (u16)(send_max - 4);  /* 512 - 4 = 508 �̂͂� */
    wpb_work->recv_unit = (u16)(recv_max - 4);  /* 512 - 4 = 508 �̂͂� */

    /* ��M�Ǘ����̏����� */
    MI_CpuClear32(wpb_work->recv_bitmap_buf, sizeof(wpb_work->recv_bitmap_buf));

    /* WPB �֌W�̏����� */
    wpb_work->my_pass_data.total_count = 0;
    wpb_work->my_pass_data.size = 0;
    wpb_work->my_pass_data.user_send_data = NULL;

    wpb_work->executing = TRUE;
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
    WXCImplWorkWpb *wpb_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    /* �f�[�^�o�b�t�@�̏����� */
    pass_ResetData(WXC_GetCurrentBlockImpl(protocol)->impl_work);

    /* protocol ���ʃ����o�� ����M�o�b�t�@���֘A�t���Ă��� */
    protocol->send.buffer = (void *)send_buf;
    protocol->send.length = (u16)send_size;
    protocol->send.checksum = MATH_CalcChecksum8(send_buf, send_size);
    protocol->recv.buffer = recv_buf;
    protocol->recv.buffer_max = (u16)recv_max;
    MI_CpuClear32(wpb_work->recv_bitmap_buf, sizeof(wpb_work->recv_bitmap_buf));

    /* WPB ���̃o�b�t�@�ݒ� */
    wpb_work->my_pass_data.total_count =
        (int)(recv_max / PASS_BUFFER_SIZE) + ((recv_max % PASS_BUFFER_SIZE) ? 1 : 0);
    wpb_work->my_pass_data.size = (int)send_size;
    wpb_work->my_pass_data.user_send_data = send_buf;

    wpb_work->pass_ctrl.user_recv_buffer = recv_buf;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WXCi_IsExecuting

  Description:  ���݃f�[�^���������𔻒�.

  Arguments:    None.

  Returns:      ���݃f�[�^�������Ȃ�� TRUE.
 *---------------------------------------------------------------------------*/
BOOL WXCi_IsExecuting(WXCProtocolContext * protocol)
{
    WXCImplWorkWpb *wpb_work = WXC_GetCurrentBlockImpl(protocol)->impl_work;

    return wpb_work->executing;
}
