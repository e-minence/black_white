//==============================================================================
/**
 * @file    union_main.h
 * @brief   ���j�I�����[���F���C��
 * @author  matsuda
 * @date    2009.07.03(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_comm.h"
#include "field/fieldmap_proc.h"
#include "net_app/union/union_types.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�����V�`���G�[�V����Index
typedef enum{
  UNION_MYSITU_PARAM_IDX_CALLING_PC,      ///<�ڑ����Ăт����Ă���PC�ւ̃f�[�^
  UNION_MYSITU_PARAM_IDX_ANSWER_PC,       ///<�ڑ����悤�Ƃ��Ă���(�ڑ���)PC�ւ̃f�[�^
  UNION_MYSITU_PARAM_IDX_CONNECT_PC,      ///<�ڑ�����PC�ւ̃f�[�^
  UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY,   ///<���s���̃Q�[���J�e�S���[
}UNION_MYSITU_PARAM_IDX;


//==============================================================================
//  �\���̒�`
//==============================================================================
///���j�I�����[���Ăяo���p��ParentWork
typedef struct{
  MYSTATUS *mystatus;
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *game_data;
  GAMESYS_WORK *gsys;
}UNION_PARENT_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  union_comm.c
//--------------------------------------------------------------
extern void * UnionComm_Init(int *seq, void *pwk);
extern BOOL UnionComm_InitWait(int *seq, void *pwk, void *pWork);
extern BOOL UnionComm_Exit(int *seq, void *pwk, void *pWork);
extern BOOL UnionComm_ExitWait(int *seq, void *pwk, void *pWork);
extern void UnionComm_Update(int *seq, void *pwk, void *pWork);

extern void UnionBeacon_ClearAllReceiveData(UNION_SYSTEM_PTR unisys);

extern void UnionMySituation_SetParam(UNION_SYSTEM_PTR unisys, UNION_MYSITU_PARAM_IDX index, void *work);
extern void UnionMySituation_Clear(UNION_SYSTEM_PTR unisys);

extern void UnionMyComm_Init(UNION_SYSTEM_PTR unisys, UNION_MY_COMM *mycomm);
extern void UnionMyComm_InitMenuParam(UNION_MY_COMM *mycomm);
extern BOOL UnionMyComm_PartyAddParam(UNION_MY_COMM *mycomm, const u8 *mac_address, u8 trainer_view, u8 sex);
extern BOOL UnionMyComm_PartyAdd(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc);
extern void UnionMyComm_PartyDel(UNION_MY_COMM *mycomm, const UNION_BEACON_PC *pc);

extern void UnionComm_Req_ShutdownRestarts(UNION_SYSTEM_PTR unisys);
extern void UnionComm_Req_Shutdown(UNION_SYSTEM_PTR unisys);
extern void UnionComm_Req_Restarts(UNION_SYSTEM_PTR unisys);
extern BOOL UnionComm_Check_ShutdownRestarts(UNION_SYSTEM_PTR unisys);

//--------------------------------------------------------------
//  union_main.c
//--------------------------------------------------------------
extern void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELD_MAIN_WORK *fieldmap);
extern BOOL Union_FieldCheck(UNION_SYSTEM_PTR unisys);
extern void UnionMain_Callback_FieldCreate(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork);
extern void UnionMain_Callback_FieldDelete(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork);
extern BOOL UnionMain_GetFinishReq(UNION_SYSTEM_PTR unisys);
extern BOOL UnionMain_GetFinishExe(UNION_SYSTEM_PTR unisys);

