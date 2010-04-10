//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_util.h
 *	@brief  ���E�ΐ�p���ʃ��W���[��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.29
 *
 *	WBM = WifiBattleMatch
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "system/bmp_menulist.h"  //BMPMENULIST_NULL

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
 *            �E�����`��̓X�g���[��
 *            �E�e�L�X�g�͈͉͂�ʉ�������2�s�ق�
 *            �EBMPWIN�̎���Ƀt���[������
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�`�����
//=====================================
typedef enum
{
  WBM_TEXT_TYPE_QUE,     //�v�����g�L���[���g��
  WBM_TEXT_TYPE_STREAM,  //�X�g���[�����g��
  WBM_TEXT_TYPE_WAIT,   //�ҋ@�A�C�R��������  �I����EndWait

  WBM_TEXT_TYPE_MAX,    //c�����ɂĎg�p
} WBM_TEXT_TYPE;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _WBM_TEXT_WORK WBM_TEXT_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_TEXT_WORK * WBM_TEXT_Init( u16 frm, u16 font_plt, u16 frm_plt, u16 frm_chr, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern void WBM_TEXT_Exit( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Main( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Print( WBM_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, WBM_TEXT_TYPE type );
extern BOOL WBM_TEXT_IsEnd( const WBM_TEXT_WORK* cp_wk );
extern void WBM_TEXT_EndWait( WBM_TEXT_WORK* p_wk );


extern void WBM_TEXT_PrintDebug( WBM_TEXT_WORK* p_wk, const u16 *cp_str, u16 len, GFL_FONT *p_font );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���X�g
 *				    �E�ȒP�Ƀ��X�g���o�����߂ɍő�l���߂���
 *            �E�\���������X�g�ő吔
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define WBM_LIST_SELECT_NULL    (BMPMENULIST_NULL)
#define WBM_LIST_WINDOW_MAX     (4)

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[WBM_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;

  BOOL  is_cancel;
  u32   cancel_idx;
} WBM_LIST_SETUP;

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct _WBM_LIST_WORK WBM_LIST_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_LIST_WORK * WBM_LIST_Init( const WBM_LIST_SETUP *cp_setup, HEAPID heapID );
extern WBM_LIST_WORK * WBM_LIST_InitEx( const WBM_LIST_SETUP *cp_setup, u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
extern void WBM_LIST_Exit( WBM_LIST_WORK *p_wk );
extern u32 WBM_LIST_Main( WBM_LIST_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �V�[�P���X�Ǘ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _WBM_SEQ_WORK WBM_SEQ_WORK;

//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
typedef void (*WBM_SEQ_FUNCTION)( WBM_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_SEQ_WORK *WBM_SEQ_Init( void *p_wk_adrs, WBM_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void WBM_SEQ_Exit( WBM_SEQ_WORK *p_wk );
extern void WBM_SEQ_Main( WBM_SEQ_WORK *p_wk );
extern BOOL WBM_SEQ_IsEnd( const WBM_SEQ_WORK *cp_wk );
extern void WBM_SEQ_SetNext( WBM_SEQ_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
extern void WBM_SEQ_End( WBM_SEQ_WORK *p_wk );
extern void WBM_SEQ_SetReservSeq( WBM_SEQ_WORK *p_wk, int seq );
extern void WBM_SEQ_NextReservSeq( WBM_SEQ_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �T�u�v���Z�X
 *				    �E�v���Z�X���s��������V�X�e��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���[�N
//=====================================
typedef struct _WBM_SUBPROC_WORK WBM_SUBPROC_WORK;

//-------------------------------------
///	�T�u�v���Z�X�������E����֐��R�[���o�b�N
//=====================================
typedef void *(*WBM_SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*WBM_SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );

//-------------------------------------
///	�T�u�v���Z�X�ݒ�\����
//=====================================
typedef struct 
{
	FSOverlayID							    ov_id;
	const GFL_PROC_DATA			    *cp_procdata;
	WBM_SUBPROC_ALLOC_FUNCTION	alloc_func;
	WBM_SUBPROC_FREE_FUNCTION		free_func;
} WBM_SUBPROC_DATA;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_SUBPROC_WORK * WBM_SUBPROC_Init( const WBM_SUBPROC_DATA *cp_procdata_tbl, u32 tbl_len, void *p_wk_adrs, HEAPID heapID );
extern void WBM_SUBPROC_Exit( WBM_SUBPROC_WORK *p_wk );
extern BOOL WBM_SUBPROC_Main( WBM_SUBPROC_WORK *p_wk );
extern GFL_PROC_MAIN_STATUS WBM_SUBPROC_GetStatus( const WBM_SUBPROC_WORK *cp_wk );
extern void WBM_SUBPROC_CallProc( WBM_SUBPROC_WORK *p_wk, u32 procID );


