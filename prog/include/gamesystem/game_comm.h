//==============================================================================
/**
 * @file    game_comm.h
 * @brief   ゲーム通信管理のヘッダ
 * @author  matsuda
 * @date    2009.05.12(火)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_data.h"


//==============================================================================
//  定数定義
//==============================================================================
///実行している通信モード
typedef enum{
  GAME_COMM_STATUS_NULL,              ///<何もしていない
  
  GAME_COMM_STATUS_WIRELESS,          ///<ワイヤレス通信
  GAME_COMM_STATUS_WIFI,              ///<Wi-Fi通信
  GAME_COMM_STATUS_IRC,               ///<赤外線通信
}GAME_COMM_STATUS;


///実行しているゲーム種類
typedef enum{
  GAME_COMM_NO_NULL,                  ///<何も起動していない状態
  
  GAME_COMM_NO_FIELD_BEACON_SEARCH,   ///<フィールド上でビーコンサーチ
  GAME_COMM_NO_INVASION,              ///<侵入
  GAME_COMM_NO_UNION,                 ///<ユニオンルーム
}GAME_COMM_NO;


///インフォメーションメッセージ内のwordset最大数
#define INFO_WORDSET_MAX      (4)

//==============================================================================
//  型定義
//==============================================================================
///ゲーム通信管理ワークの不定形ポインタ
typedef struct _GAME_COMM_SYS * GAME_COMM_SYS_PTR;


//==============================================================================
//  構造体定義
//==============================================================================
///インフォメーションメッセージ構造体(キューを取得する時、この構造体に変換して取得する)
typedef struct{
  STRBUF *name[INFO_WORDSET_MAX];     ///<未使用の場合はNULLが入っています
  u8 wordset_no[INFO_WORDSET_MAX];
  u16 message_id;
  u8 padding[2];
}GAME_COMM_INFO_MESSAGE;


//==============================================================================
//  外部関数宣言
//==============================================================================
//--------------------------------------------------------------
//  システム使用    ※通常ユーザーは使用しない
//--------------------------------------------------------------
extern GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id, GAMEDATA *gamedata);
extern void GameCommSys_Free(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_Main(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_Callback_FieldCreate(GAME_COMM_SYS_PTR gcsp, void *fieldWork);
extern void GameCommSys_Callback_FieldDelete(GAME_COMM_SYS_PTR gcsp, void *fieldWork);

//--------------------------------------------------------------
//  ユーザー使用
//--------------------------------------------------------------
extern void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work);
extern void GameCommSys_ExitReq(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work);
extern GAME_COMM_STATUS GameCommSys_GetCommStatus(GAME_COMM_SYS_PTR gcsp);
extern GAME_COMM_NO GameCommSys_BootCheck(GAME_COMM_SYS_PTR gcsp);
extern GAME_COMM_NO GameCommSys_GetCommGameNo(GAME_COMM_SYS_PTR gcsp);
extern BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp);
extern void *GameCommSys_GetAppWork(GAME_COMM_SYS_PTR gcsp);
extern GAMEDATA * GameCommSys_GetGameData(GAME_COMM_SYS_PTR gcsp);
extern void GameCommStatus_SetPlayerStatus(GAME_COMM_SYS_PTR gcsp, int comm_net_id, ZONEID zone_id, u8 invasion_netid);
extern BOOL GameCommInfo_GetMessage(GAME_COMM_SYS_PTR gcsp, GAME_COMM_INFO_MESSAGE *dest_msg);


