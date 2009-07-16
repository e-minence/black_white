//==============================================================================
/**
 * @file    union_event_check.h
 * @brief   ���j�I���F�C�x���g�N���`�F�b�N�̃w�b�_
 * @author  matsuda
 * @date    2009.07.09(��)
 */
//==============================================================================
#pragma once

#include "net_app/union/union_main.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * UnionEvent_TalkCheck( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
extern void UnionEvent_SubProcSet(UNION_SYSTEM_PTR unisys, UNION_SUBPROC_ID sub_proc_id, void *parent_wk);
extern UNION_SUBPROC_ID UnionEvent_SubProcGet(UNION_SYSTEM_PTR unisys);

