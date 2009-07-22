//==============================================================================
/**
 * @file    union_main.h
 * @brief   ユニオンルーム：メイン
 * @author  matsuda
 * @date    2009.07.03(金)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_comm.h"
#include "field/fieldmap_proc.h"
#include "net_app/union/union_types.h"


//==============================================================================
//  定数定義
//==============================================================================
///自分シチュエーションIndex
typedef enum{
  UNION_MYSITU_PARAM_IDX_CALLING_PC,      ///<接続を呼びかけているPCへのデータ
  UNION_MYSITU_PARAM_IDX_ANSWER_PC,       ///<接続しようとしている(接続先)PCへのデータ
  UNION_MYSITU_PARAM_IDX_CONNECT_PC,      ///<接続中のPCへのデータ
  UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY,   ///<実行中のゲームカテゴリー
}UNION_MYSITU_PARAM_IDX;


//==============================================================================
//  構造体定義
//==============================================================================
///ユニオンルーム呼び出し用のParentWork
typedef struct{
  MYSTATUS *mystatus;
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *game_data;
  GAMESYS_WORK *gsys;
}UNION_PARENT_WORK;


//==============================================================================
//  外部関数宣言
//==============================================================================
//--------------------------------------------------------------
//  union_comm.c
//--------------------------------------------------------------
extern void * UnionComm_Init(int *seq, void *pwk);
extern BOOL UnionComm_InitWait(int *seq, void *pwk, void *pWork);
extern BOOL UnionComm_Exit(int *seq, void *pwk, void *pWork);
extern BOOL UnionComm_ExitWait(int *seq, void *pwk, void *pWork);
extern void UnionComm_Update(int *seq, void *pwk, void *pWork);

extern void UnionMySituation_SetParam(UNION_SYSTEM_PTR unisys, UNION_MYSITU_PARAM_IDX index, void *work);
extern void UnionMySituation_Clear(UNION_SYSTEM_PTR unisys);

extern void UnionMyComm_Init(UNION_SYSTEM_PTR unisys, UNION_MY_COMM *mycomm);
extern BOOL UnionMyComm_PartyAdd(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc);
extern void UnionMyComm_PartyDel(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc);

extern void UnionComm_Req_ShutdownRestarts(UNION_SYSTEM_PTR unisys);
extern BOOL UnionComm_Check_ShutdownRestarts(UNION_SYSTEM_PTR unisys);

//--------------------------------------------------------------
//  union_main.c
//--------------------------------------------------------------
extern void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELD_MAIN_WORK *fieldmap);
extern BOOL Union_FieldCheck(UNION_SYSTEM_PTR unisys);
extern void Union_FieldDelete(UNION_SYSTEM_PTR unisys);
extern void Union_FieldComeback(UNION_SYSTEM_PTR unisys);

