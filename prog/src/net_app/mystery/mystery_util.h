//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_util.h
 *	@brief  �ӂ����Ȃ�������̂Ŏg�p���郂�W���[����
 *	@author	Toru=Nagihashi
 *	@date		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//�V�X�e��
#include "system/main.h"    //HEAPID
#include "print/gf_font.h"  //GFL_FONT
#include "print/printsys.h"
#include "system/bmp_menulist.h"  //BMPMENULIST_NULL
#include "system/bmp_oam.h"


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
 *					  �E�����`���PRINTQUE
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���W�ʒu
//=====================================
typedef enum 
{
  MYSTERY_MSGWIN_POS_ABSOLUTE,     //��Έʒu
  MYSTERY_MSGWIN_POS_WH_CENTER,  //���΍��W  ���A�����Ƃ��ɒ���
  MYSTERY_MSGWIN_POS_H_CENTER,    //���΍��W  �����̂ݒ���
} MYSTERY_MSGWIN_POS;

#define MYSTERY_MSGWIN_DEFAULT_COLOR  (PRINTSYS_MACRO_LSB( 1, 2, 0 ))
#define MYSTERY_MSGWIN_WHITE_COLOR    (PRINTSYS_MACRO_LSB( 0xf, 2, 0 ))

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _MYSTERY_MSGWIN_WORK MYSTERY_MSGWIN_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_MSGWIN_WORK * MYSTERY_MSGWIN_Init( u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID );
extern void MYSTERY_MSGWIN_Clear( MYSTERY_MSGWIN_WORK* p_wk );
extern void MYSTERY_MSGWIN_Exit( MYSTERY_MSGWIN_WORK* p_wk );
extern void MYSTERY_MSGWIN_Print( MYSTERY_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void MYSTERY_MSGWIN_PrintBuf( MYSTERY_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void MYSTERY_MSGWIN_SetColor( MYSTERY_MSGWIN_WORK* p_wk, PRINTSYS_LSB color );
extern void MYSTERY_MSGWIN_SetPos( MYSTERY_MSGWIN_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGWIN_POS type );
extern BOOL MYSTERY_MSGWIN_PrintMain( MYSTERY_MSGWIN_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�`�����
//=====================================
typedef enum
{
  MYSTERY_TEXT_TYPE_QUE,     //�v�����g�L���[���g��
  MYSTERY_TEXT_TYPE_STREAM,  //�X�g���[�����g��

  MYSTERY_TEXT_TYPE_MAX,    //c�����ɂĎg�p
} MYSTERY_TEXT_TYPE;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _MYSTERY_TEXT_WORK MYSTERY_TEXT_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_TEXT_WORK * MYSTERY_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern MYSTERY_TEXT_WORK * MYSTERY_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern void MYSTERY_TEXT_Exit( MYSTERY_TEXT_WORK* p_wk );
extern void MYSTERY_TEXT_Main( MYSTERY_TEXT_WORK* p_wk );

extern void MYSTERY_TEXT_Print( MYSTERY_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, MYSTERY_TEXT_TYPE type );
extern void MYSTERY_TEXT_PrintBuf( MYSTERY_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, MYSTERY_TEXT_TYPE type );
extern BOOL MYSTERY_TEXT_IsEndPrint( const MYSTERY_TEXT_WORK *cp_wk );

extern void MYSTERY_TEXT_WriteWindowFrame( MYSTERY_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt );

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
#define MYSTERY_LIST_SELECT_NULL  (BMPMENULIST_NULL)
#define MYSTERY_LIST_WINDOW_MAX   (3)
//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[MYSTERY_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;
} MYSTERY_LIST_SETUP;

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct _MYSTERY_LIST_WORK MYSTERY_LIST_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_LIST_WORK * MYSTERY_LIST_Init( const MYSTERY_LIST_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_LIST_Exit( MYSTERY_LIST_WORK *p_wk );
extern u32 MYSTERY_LIST_Main( MYSTERY_LIST_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  BMPWIN�̏W�܂�  �����ȕ������͂����ʂȂǂŎg�p
 *				    �EBMPWIN����C�ɍ쐬���A��C�ɏ�������
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�ݒ胏�[�N
//=====================================
typedef struct 
{
  u8  x;
  u8  y;
  u8  w;
  u8  h;
  u32 strID;
  STRBUF *p_strbuf;
  MYSTERY_MSGWIN_POS  pos_type;
  s16 pos_x;
  s16 pos_y;
  PRINTSYS_LSB  color;
} MYSTERY_MSGWINSET_SETUP_TBL;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _MYSTERY_MSGWINSET_WORK MYSTERY_MSGWINSET_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
MYSTERY_MSGWINSET_WORK * MYSTERY_MSGWINSET_Init( const MYSTERY_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID );
extern void MYSTERY_MSGWINSET_Exit( MYSTERY_MSGWINSET_WORK* p_wk );
extern void MYSTERY_MSGWINSET_Clear( MYSTERY_MSGWINSET_WORK* p_wk );
extern BOOL MYSTERY_MSGWINSET_PrintMain( MYSTERY_MSGWINSET_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���j���[
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define MYSTERY_MENU_SELECT_NULL  (BMPMENULIST_NULL)
#define MYSTERY_MENU_WINDOW_MAX   (4)

//-------------------------------------
///	���j���[�ݒ�
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg; //NULL�̏ꍇ�͉��Lp_strbuf���g�p����A�����Ȃ���strID���g�����͂��쐬
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  GFL_CLWK    *p_cursor;
  union
  { 
    u32     strID[MYSTERY_MENU_WINDOW_MAX];
    STRBUF  *p_strbuf[MYSTERY_MENU_WINDOW_MAX];  //�����J�����Ă�OK
  };
  u32 list_max;

  u16 font_frm;
  u16 font_plt;
  u16 bg_frm; //�g  
  u16 bg_ofs;  //BG�p���b�g
} MYSTERY_MENU_SETUP;

//-------------------------------------
///	���j���[
//=====================================
typedef struct _MYSTERY_MENU_WORK MYSTERY_MENU_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_MENU_WORK * MYSTERY_MENU_Init( const MYSTERY_MENU_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_MENU_Exit( MYSTERY_MENU_WORK *p_wk );
extern u32 MYSTERY_MENU_Main( MYSTERY_MENU_WORK *p_wk );

extern void MYSTERY_MENU_BlinkMain( MYSTERY_MENU_WORK *p_wk, u32 list_num );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �V�[�P���X�Ǘ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _MYSTERY_SEQ_WORK MYSTERY_SEQ_WORK;

//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
typedef void (*MYSTERY_SEQ_FUNCTION)( MYSTERY_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_SEQ_WORK *MYSTERY_SEQ_Init( void *p_wk_adrs, MYSTERY_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void MYSTERY_SEQ_Exit( MYSTERY_SEQ_WORK *p_wk );
extern void MYSTERY_SEQ_Main( MYSTERY_SEQ_WORK *p_wk );
extern BOOL MYSTERY_SEQ_IsEnd( const MYSTERY_SEQ_WORK *cp_wk );
extern void MYSTERY_SEQ_SetNext( MYSTERY_SEQ_WORK *p_wk, MYSTERY_SEQ_FUNCTION seq_function );
extern void MYSTERY_SEQ_End( MYSTERY_SEQ_WORK *p_wk );
extern void MYSTERY_SEQ_SetReservSeq( MYSTERY_SEQ_WORK *p_wk, int seq );
extern void MYSTERY_SEQ_NextReservSeq( MYSTERY_SEQ_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  MSGOAM  OAM�ŕ������������W���[��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���W�ʒu
//=====================================
typedef enum 
{
  MYSTERY_MSGOAM_POS_ABSOLUTE,    //��Έʒu
  MYSTERY_MSGOAM_POS_WH_CENTER,  //���΍��W  ���A�����Ƃ��ɒ���
  MYSTERY_MSGOAM_POS_H_CENTER,  //���΍��W  �����̂ݒ���
} MYSTERY_MSGOAM_POS;

//-------------------------------------
///	���b�Z�[�WOAM
//=====================================
typedef struct _MYSTERY_MSGOAM_WORK MYSTERY_MSGOAM_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_MSGOAM_WORK * MYSTERY_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID );
extern void MYSTERY_MSGOAM_Exit( MYSTERY_MSGOAM_WORK* p_wk );
extern void MYSTERY_MSGOAM_Clear( MYSTERY_MSGOAM_WORK* p_wk );
extern void MYSTERY_MSGOAM_Print( MYSTERY_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void MYSTERY_MSGOAM_PrintBuf( MYSTERY_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void MYSTERY_MSGOAM_SetStrColor( MYSTERY_MSGOAM_WORK* p_wk, PRINTSYS_LSB color );
extern void MYSTERY_MSGOAM_SetStrPos( MYSTERY_MSGOAM_WORK* p_wk, s16 x, s16 y, MYSTERY_MSGOAM_POS type );
extern BOOL MYSTERY_MSGOAM_PrintMain( MYSTERY_MSGOAM_WORK* p_wk );
extern BMPOAM_ACT_PTR MYSTERY_MSGOAM_GetBmpOamAct( MYSTERY_MSGOAM_WORK* p_wk );

