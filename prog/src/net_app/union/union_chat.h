//==============================================================================
/**
 * @file    union_chat.h
 * @brief   �`���b�g�̃w�b�_
 * @author  matsuda
 * @date    2009.08.26(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void UnionChat_AddChat(UNION_SYSTEM_PTR unisys, UNION_BEACON_PC *bpc, const PMS_DATA *mine_pmsdata);
extern UNION_CHAT_DATA * UnionChat_GetSpaceBuffer(UNION_CHAT_LOG *log);
extern UNION_CHAT_DATA * UnionChat_GetReadBuffer(UNION_CHAT_LOG *log, u32 log_no);
