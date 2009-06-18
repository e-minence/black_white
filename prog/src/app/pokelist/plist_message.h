//======================================================================
/**
 * @file	plist_message.c
 * @brief	ポケモンリスト メッセージ
 * @author	ariizumi
 * @data	09/06/17
 *
 * モジュール名：PLIST_MSG
 */
//======================================================================
#pragma once

#include "plist_local_def.h"

//メッセージウィンドウの種類
typedef enum
{
  PMT_BAR,      //バーの1行メッセージ
  PMT_MENU,     //メニューの小さめ2行
  PMT_MESSAGE,  //フルサイズ
  
  PMT_NONE,     //ウィンドウ閉じてる
  PMT_MAX,
}PLIST_MSGWIN_TYPE;


extern PLIST_MSG_WORK* PLIST_MSG_CreateSystem( PLIST_WORK *work );
extern void PLIST_MSG_DeleteSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );
extern void PLIST_MSG_UpdateSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );

extern void PLIST_MSG_OpenWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const PLIST_MSGWIN_TYPE winType);
extern void PLIST_MSG_CloseWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork );

extern void PLIST_MSG_DrawMessageNoWait( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const u32 msgId );


