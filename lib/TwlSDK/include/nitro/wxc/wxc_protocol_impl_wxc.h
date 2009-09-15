/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     wxc_protocol_impl_wxc.h

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

#ifndef	NITRO_WXC_PROTOCOL_IMPL_WXC_H_
#define	NITRO_WXC_PROTOCOL_IMPL_WXC_H_

#include <nitro.h>


/*****************************************************************************/
/* constant */


#ifdef  __cplusplus
extern "C" {
#endif


// wxc �p�� ImplWork �\����
// �O�܂ł� protocol.h �� WXCProtocolContext �ɂ���������
typedef struct
{
    WXCBlockInfo req;                  /* ���g����̗v�� */
    WXCBlockInfo ack;                  /* ����ւ̉��� */

    u32     recv_total;                /* ��M�p�P�b�g���� */
    u32     recv_rest;                 /* ��M�p�P�b�g�c�� */

    /* ��M�Ǘ���� */
    u32     recv_bitmap_buf[WXC_RECV_BITSET_SIZE];

    u16     send_unit;                 /* ���M�p�P�b�g�P�ʃT�C�Y */
    u16     recv_unit;                 /* ��M�p�P�b�g�P�ʃT�C�Y */

    /* ���M�Ǘ���� */
    u16     recent_index[WXC_RECENT_SENT_LIST_MAX];

    BOOL    executing;
} WXCImplWorkWxc;

extern WXCProtocolImpl wxc_protocol_impl_wxc;


#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_PROTOCOL_IMPL_WXC_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
