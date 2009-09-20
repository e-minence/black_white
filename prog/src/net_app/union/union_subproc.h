//==============================================================================
/**
 * @file    union_subproc.h
 * @brief   
 * @author  matsuda
 * @date    2009.07.16(��)
 */
//==============================================================================
#pragma once

#include "app/pokelist.h"
#include "app/p_status.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�|�P�������X�g�T�uProc�Ăяo���悤��ParentWork
typedef struct{
  PLIST_DATA plist;
  PSTATUS_DATA pstatus;
}UNION_SUBPROC_PARENT_POKELIST;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * UnionSubProc_Create(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork, UNION_SYSTEM_PTR unisys);
extern void UnionSubProc_EventSet(UNION_SYSTEM_PTR unisys, UNION_SUBPROC_ID sub_proc_id, void *parent_wk);
extern BOOL UnionSubProc_IsExits(UNION_SYSTEM_PTR unisys);

