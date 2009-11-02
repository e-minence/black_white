//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		msgsearch.h
 *	@brief	MSG���當������������郂�W���[��
 *	@author	Toru=Nagihashi
 *	@date		2009.10.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	MSG�f�[�^���當������������郏�[�N
//=====================================
typedef struct _MSGSEARCH_WORK MSGSEARCH_WORK;

//-------------------------------------
///	�������ʎ󂯎��\����
//=====================================
typedef struct 
{
	s32 msg_idx;			//-1���ƌ�����Ȃ�����
	s32 str_idx;			//-1���ƌ�����Ȃ�����
} MSGSEARCH_RESULT;


//=============================================================================
/**
 *					�O�����J�֐�
*/
//=============================================================================
//-------------------------------------
///	�������A�j��
//=====================================
extern MSGSEARCH_WORK *MSGSEARCH_Init( GFL_MSGDATA *p_msg_tbl[], u32 msg_max, HEAPID heapID );
extern void MSGSEARCH_Exit( MSGSEARCH_WORK *p_wk );
//-------------------------------------
///	����
//=====================================
extern u32 MSGSEARCH_Search( const MSGSEARCH_WORK *cp_wk, u16 msg_idx, u16 start_str_idx, const STRBUF *cp_buf, MSGSEARCH_RESULT *p_result_tbl, u32 tbl_max );
extern u32 MSGSEARCH_SearchAll( const MSGSEARCH_WORK *cp_wk, const STRBUF *cp_buf, MSGSEARCH_RESULT *p_result_tbl, u32 tbl_max );
extern u32 MSGSEARCH_SearchNum( const MSGSEARCH_WORK *cp_wk, const STRBUF *cp_buf );
//-------------------------------------
///	������֌W
//=====================================
extern STRBUF *MSGSEARCH_CreateString( const MSGSEARCH_WORK	*cp_wk, const MSGSEARCH_RESULT *cp_result_tbl );
extern void MSGSEARCH_GetString( const MSGSEARCH_WORK *cp_wk, const MSGSEARCH_RESULT *cp_result_tbl, STRBUF *p_str );

