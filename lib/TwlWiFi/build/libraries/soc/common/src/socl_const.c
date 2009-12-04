/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - SOC - libraries
  File:     socl_const.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

/*---------------------------------------------------------------------------*
  Name:         SOCLSocketParamTCP/SOCLSocketParamUDP

  Description:  �\�P�b�g�ɂ�����f�t�H���g�̃��������p�p�����[�^�D
                �ア�V���{���Ƃ��Ē�`���Ă���̂ŃA�v���P�[�V��������
                ���������p�Ɋւ���ׂ��Ȑݒ�̂��߂ɃI�[�o�[���C�h�\�D
                TCP �ł� UDP �ł�����
 *---------------------------------------------------------------------------*/

// TCP
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamTCP =
{
    SOCL_FLAGMODE_TCP,                      // TCP
    SOCL_FLAGBLOCK_BLOCK,                   // BLOCK
    {                                       // cps
        0,                                  //SOCL_TCP_SOCKET_CPS_RCVBUF_SIZE,		// rcvbuf_size

        //****Set in SOCL_Startup()
        0,                                  //SOCL_TCP_SOCKET_CPS_RCVBUF_CONSUME_MIN,	// rcvbuf_consume_min

        //****Set in SOCL_Startup()
        SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE,    // sndbuf_size
        SOCL_TCP_SOCKET_CPS_LINBUF_SIZE,    // linbuf_size
        SOCL_TCP_SOCKET_CPS_OUTBUF_SIZE,    // outbuf_size
        SOCL_TCP_SOCKET_SEND_WRTBUF_SIZE,   // wrtbuf_size
        0,  // udpbuf_size
    },

    {       // recv_thread
        SOCL_TCP_SOCKET_RECV_THREAD_STACK_SIZE, // stack_size
        SOCL_TCP_SOCKET_RECV_THREAD_PRIORITY,   // priority
        SOCL_TCP_SOCKET_RECV_THREAD_QUEUE_MAX,  // queue_max
    },

    {   // send_thread
        SOCL_TCP_SOCKET_SEND_THREAD_STACK_SIZE, // stack_size
        SOCL_TCP_SOCKET_SEND_THREAD_PRIORITY,   // priority
        SOCL_TCP_SOCKET_SEND_THREAD_QUEUE_MAX,  // queue_max
    },
};

//----------------------------------------------------------------------------
// UDP
//
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamUDP =
{
    SOCL_FLAGMODE_UDP,                      // UDP
    SOCL_FLAGBLOCK_BLOCK,                   // BLOCK
    {                                       // cps
        SOCL_UDP_SOCKET_CPS_RCVBUF_SIZE,    // rcvbuf_size
        1,  // rcvbuf_consume_min
        0,  // sndbuf_size
        SOCL_UDP_SOCKET_CPS_LINBUF_SIZE,    // linbuf_size
        0,  // outbuf_size
        0,  // wrtbuf_size
        SOCL_UDP_SOCKET_RECV_UDPBUF_SIZE,   // udpbuf_size
    },

    {   // recv_thread
        SOCL_UDP_SOCKET_RECV_THREAD_STACK_SIZE, // stack_size
        SOCL_UDP_SOCKET_RECV_THREAD_PRIORITY,   // priority
        SOCL_UDP_SOCKET_RECV_THREAD_QUEUE_MAX,  // queue_max
    },

    {                       // send_thread
        0,                  // stack_size
        0,                  // priority
        0,                  // queue_max
    },
};

//----------------------------------------------------------------------------
// UDP for Send
//
SDK_WEAK_SYMBOL SOCLSocketParam SOCLSocketParamUDPSend =
{
    SOCL_FLAGMODE_UDPSEND,  // UDPSEND
    SOCL_FLAGBLOCK_NOBLOCK, // BLOCK
    {                       // cps
        0,                  // rcvbuf_size
        0,                  // rcvbuf_consume_min
        SOCL_UDP_SOCKET_CPS_SNDBUF_SIZE,    // sndbuf_size
        0,  // linbuf_size
        SOCL_UDP_SOCKET_CPS_OUTBUF_SIZE,    // outbuf_size
        SOCL_UDP_SOCKET_SEND_WRTBUF_SIZE,   // wrtbuf_size
        0,  // udpbuf_size
    },

    {       // recv_thread
        0,  // stack_size
        0,  // priority
        0,  // queue_max
    },

    {       // send_thread
        SOCL_UDP_SOCKET_SEND_THREAD_STACK_SIZE, // stack_size
        SOCL_UDP_SOCKET_SEND_THREAD_PRIORITY,   // priority
        SOCL_UDP_SOCKET_SEND_THREAD_QUEUE_MAX,  // queue_max
    },
};
