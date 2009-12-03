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
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _WBM_TEXT_WORK WBM_TEXT_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_TEXT_WORK * WBM_TEXT_Init( u16 frm, u16 font_plt, u16 frm_plt, u16 frm_chr, HEAPID heapID );
extern void WBM_TEXT_Exit( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Main( WBM_TEXT_WORK* p_wk );
extern void WBM_TEXT_Print( WBM_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern BOOL WBM_TEXT_IsEnd( const WBM_TEXT_WORK* cp_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���X�g
 *				    �E�ȒP�Ƀ��X�g���o�����߂ɍő�l���߂���
 *            �E�\���������X�g�ő吔
 *            �E�ʒu�Œ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define WBM_LIST_SELECT_NULL  (BMPMENULIST_NULL)
#define WBM_LIST_WINDOW_MAX   (3)
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
} WBM_LIST_SETUP;

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct _WBM_LIST_WORK WBM_LIST_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern WBM_LIST_WORK * WBM_LIST_Init( const WBM_LIST_SETUP *cp_setup, HEAPID heapID );
extern void WBM_LIST_Exit( WBM_LIST_WORK *p_wk );
extern u32 WBM_LIST_Main( WBM_LIST_WORK *p_wk );
