//==============================================================================
/**
 * @file    union_msg.h
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELD_MAIN_WORK *fieldWork);
extern void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys);
extern void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id);
extern BOOL UnionMsg_TalkStrem_Check(UNION_SYSTEM_PTR unisys);
