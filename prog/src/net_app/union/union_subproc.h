//==============================================================================
/**
 * @file    union_subproc.h
 * @brief   
 * @author  matsuda
 * @date    2009.07.16(木)
 */
//==============================================================================
#pragma once


//==============================================================================
//  外部関数宣言
//==============================================================================
extern GMEVENT * UnionSubProc_Create(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork, UNION_SYSTEM_PTR unisys);
extern void UnionSubProc_EventSet(UNION_SYSTEM_PTR unisys, UNION_SUBPROC_ID sub_proc_id, void *parent_wk);
extern BOOL UnionSubProc_IsExits(UNION_SYSTEM_PTR unisys);

