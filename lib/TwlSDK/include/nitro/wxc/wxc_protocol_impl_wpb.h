/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     wxc_protocol_impl_wpb.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_WXC_PROTOCOL_IMPL_WPB_H_
#define	NITRO_WXC_PROTOCOL_IMPL_WPB_H_

#include <nitro.h>


/*****************************************************************************/
/* constant */


#ifdef  __cplusplus
extern "C" {
#endif


/* ここから wpb の丸コピー */

// ワークエリア
#define PASS_PACKET_SIZE                 512
#define PASS_BUFFER_SIZE                 (PASS_PACKET_SIZE - 4)
#define WPBC_PARENT_DATA_SIZE_MAX        PASS_PACKET_SIZE
#define WPBC_CHILD_DATA_SIZE_MAX         PASS_PACKET_SIZE

/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/

#define REQUEST_DONE                          0xffff
#define REQUEST_NONE                          0xfffe
#define REQUEST_BYE                           0xfffd

#define HARDWARE_BUFFER_DUMMY_COUNT           3

#define MAX_PATTERN                           4
#define MAX_SEQ                               4

/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/
typedef struct
{
    int     size;
    int     total_count;
    const u8 *user_send_data;
} PassData;

typedef struct
{
    u16     req_count;
    u16     res_count;
    u8      buf[PASS_BUFFER_SIZE];
} PassBuffer;

typedef struct
{
    u8     *user_recv_buffer;
    BOOL    reset_done;
    BOOL    send_done;
    BOOL    recv_done;
    int     hardware_buffer_count;
    PassBuffer send_buf;
    PassBuffer recv_buf;
    int     pre_send_count;
    u32     recv_bitmap;               /* mod32でループ */
    int     recv_bitmap_index;
} PassCtrl;


/* ここまで */

// wpb 用の ImplWork 構造体
typedef struct
{
    // PassCtrl と同一部分
/*	u8 *user_recv_buffer;
	BOOL reset_done;
	BOOL send_done;
	BOOL recv_done;
	int hardware_buffer_count;
	PassBuffer send_buf;
	PassBuffer recv_buf;
	int pre_send_count;
	u32 recv_bitmap; /* mod32でループ */
//      int recv_bitmap_index;

    PassCtrl pass_ctrl;

    // wpb がグローバルで持ってた my_pass_data をそのまま持つように。
    PassData my_pass_data;

    // WXCProtocolContext のメンバの一部がプロトコルでわかれることになったので、
    // wpb でも使いそうなものを wxc だけでなくこちらにも持ってきた
    /* 受信管理情報 */
    u32     recv_bitmap_buf[WXC_RECV_BITSET_SIZE];

    u16     send_unit;                 /* 送信パケット単位サイズ */
    u16     recv_unit;                 /* 受信パケット単位サイズ */

    /* 送信管理情報 */
    u16     recent_index[WXC_RECENT_SENT_LIST_MAX];

    BOOL    executing;
} WXCImplWorkWpb;


// wpb プロトコルの送受信関数群配列
extern WXCProtocolImpl wxc_protocol_impl_wpb;


#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_PROTOCOL_IMPL_WPB_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
