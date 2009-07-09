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


//==============================================================================
//  �\���̒�`
//==============================================================================
///���j�I�����[���Ăяo���p��ParentWork
typedef struct{
  MYSTATUS *mystatus;
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *game_data;
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

//--------------------------------------------------------------
//  union_main.c
//--------------------------------------------------------------
extern void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELD_MAIN_WORK *fieldmap);

