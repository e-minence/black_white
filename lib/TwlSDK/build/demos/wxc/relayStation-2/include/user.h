/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - demos - relayStation-2
  File:     user.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-11-15#$
  $Rev: 2414 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_BUILD_DEMOS_INCLUDE_USER_H_
#define NITRO_BUILD_DEMOS_INCLUDE_USER_H_


/*****************************************************************************/
/* constant */

/*
 * テスト用の GGID.
 * このサンプルは simple-1 の中継所として動作します.
 */
#define SDK_MAKEGGID_SYSTEM(num)              (0x003FFF00 | (num))
#define GGID_ORG_1                            SDK_MAKEGGID_SYSTEM(0x51)

/* テスト用送受信データサイズ */
#define DATA_SIZE (1024 * 20)


/*****************************************************************************/
/* variable */

/* 中継所がリレーモードなら TRUE */
extern BOOL station_is_relay;

/* 現在 Register されている送信データ */
extern u8 send_data[DATA_SIZE];

/* 上記送信データの元の所有者 */
extern u8 send_data_owner[6];


/*****************************************************************************/
/* function */

/*---------------------------------------------------------------------------*
  Name:         User_Init

  Description:  WXC ライブラリに関するユーザ側初期化処理

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    User_Init(void);



#endif /* NITRO_BUILD_DEMOS_INCLUDE_USER_H_ */
