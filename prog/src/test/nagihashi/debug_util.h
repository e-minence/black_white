//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_util.h
 *	@brief  �ł΂����Ŏg�p���郂�W���[����
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
 *            �E�����]�����[�h�ƔC�ӓ]�����[�h������
 *            �E�����]�����[�h�͍쐬�⏑���݌㑦�����f
 *            �E�C�ӓ]�����[�h��DEBUG_MSGWIN_Trans������܂ŉ�ʔ��f�����Ȃ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���W�ʒu
//=====================================
typedef enum 
{
  DEBUG_MSGWIN_POS_ABSOLUTE,     //��Έʒu
  DEBUG_MSGWIN_POS_WH_CENTER,  //���΍��W  ���A�����Ƃ��ɒ���
  DEBUG_MSGWIN_POS_H_CENTER,    //���΍��W  �����̂ݒ���
} DEBUG_MSGWIN_POS;

#define DEBUG_MSGWIN_DEFAULT_COLOR  (PRINTSYS_MACRO_LSB( 1, 2, 0 ))
#define DEBUG_MSGWIN_WHITE_COLOR    (PRINTSYS_MACRO_LSB( 0xf, 2, 0 ))

//-------------------------------------
///	�������݃��[�h
//=====================================
typedef enum
{
  DEBUG_MSGWIN_TRANS_MODE_AUTO,  //�����]��
  DEBUG_MSGWIN_TRANS_MODE_MANUAL, //�C�Ӄ^�C�~���O�œ]���֐�DEBUG_MSGWIN_Trans���ĂԂ��ƂŔ��f
} DEBUG_MSGWIN_TRANS_MODE;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _DEBUG_MSGWIN_WORK DEBUG_MSGWIN_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_MSGWIN_WORK * DEBUG_MSGWIN_Init( DEBUG_MSGWIN_TRANS_MODE mode, u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID );
extern void DEBUG_MSGWIN_Clear( DEBUG_MSGWIN_WORK* p_wk );
extern void DEBUG_MSGWIN_Exit( DEBUG_MSGWIN_WORK* p_wk );
extern void DEBUG_MSGWIN_Print( DEBUG_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void DEBUG_MSGWIN_PrintBuf( DEBUG_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void DEBUG_MSGWIN_SetColor( DEBUG_MSGWIN_WORK* p_wk, PRINTSYS_LSB color );
extern void DEBUG_MSGWIN_SetPos( DEBUG_MSGWIN_WORK* p_wk, s16 x, s16 y, DEBUG_MSGWIN_POS type );
extern BOOL DEBUG_MSGWIN_PrintMain( DEBUG_MSGWIN_WORK* p_wk );
extern void DEBUG_MSGWIN_Trans( DEBUG_MSGWIN_WORK* p_wk );

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
  DEBUG_TEXT_TYPE_QUE,     //�v�����g�L���[���g��
  DEBUG_TEXT_TYPE_STREAM,  //�X�g���[�����g��
  DEBUG_TEXT_TYPE_WAIT,   //�ҋ@�A�C�R���t��

  DEBUG_TEXT_TYPE_MAX,    //c�����ɂĎg�p
} DEBUG_TEXT_TYPE;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _DEBUG_TEXT_WORK DEBUG_TEXT_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_TEXT_WORK * DEBUG_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern DEBUG_TEXT_WORK * DEBUG_TEXT_InitOneLine( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
extern void DEBUG_TEXT_Exit( DEBUG_TEXT_WORK* p_wk );
extern void DEBUG_TEXT_Main( DEBUG_TEXT_WORK* p_wk );

extern void DEBUG_TEXT_Print( DEBUG_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, DEBUG_TEXT_TYPE type );
extern void DEBUG_TEXT_PrintBuf( DEBUG_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, DEBUG_TEXT_TYPE type );
extern BOOL DEBUG_TEXT_IsEndPrint( const DEBUG_TEXT_WORK *cp_wk );

extern void DEBUG_TEXT_WriteWindowFrame( DEBUG_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt );
extern void DEBUG_TEXT_EndWait( DEBUG_TEXT_WORK *p_wk );
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
#define DEBUG_LIST_SELECT_NULL  (BMPMENULIST_NULL)
#define DEBUG_LIST_WINDOW_MAX   (3)

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[DEBUG_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;

} DEBUG_LIST_SETUP;

//-------------------------------------
///	�I�����X�g
//=====================================
typedef struct _DEBUG_LIST_WORK DEBUG_LIST_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_LIST_WORK * DEBUG_LIST_Init( const DEBUG_LIST_SETUP *cp_setup, HEAPID heapID );
extern void DEBUG_LIST_Exit( DEBUG_LIST_WORK *p_wk );
extern u32 DEBUG_LIST_Main( DEBUG_LIST_WORK *p_wk );

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
  DEBUG_MSGWIN_POS  pos_type;
  s16 pos_x;
  s16 pos_y;
  PRINTSYS_LSB  color;
} DEBUG_MSGWINSET_SETUP_TBL;

//-------------------------------------
///	��������
//=====================================
typedef struct 
{
  BOOL  is_print;
  u32 strID;
  STRBUF *p_strbuf;
  DEBUG_MSGWIN_POS  pos_type;
  s16 pos_x;
  s16 pos_y;
  PRINTSYS_LSB  color;
} DEBUG_MSGWINSET_PRINT_TBL;

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _DEBUG_MSGWINSET_WORK DEBUG_MSGWINSET_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
DEBUG_MSGWINSET_WORK * DEBUG_MSGWINSET_Init( DEBUG_MSGWIN_TRANS_MODE mode, const DEBUG_MSGWINSET_SETUP_TBL *cp_tbl, u32 tbl_len, u16 frm, u8 plt, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, GFL_FONT *p_font, HEAPID heapID );
extern void DEBUG_MSGWINSET_Exit( DEBUG_MSGWINSET_WORK* p_wk );
extern void DEBUG_MSGWINSET_Clear( DEBUG_MSGWINSET_WORK* p_wk );
extern BOOL DEBUG_MSGWINSET_PrintMain( DEBUG_MSGWINSET_WORK* p_wk );
extern void DEBUG_MSGWINSET_Print( DEBUG_MSGWINSET_WORK* p_wk, const DEBUG_MSGWINSET_PRINT_TBL *cp_tbl );
extern void DEBUG_MSGWINSET_ClearOne( DEBUG_MSGWINSET_WORK* p_wk, u16 idx );

extern void DEBUG_MSGWINSET_Trans( DEBUG_MSGWINSET_WORK* p_wk );

//-------------------------------------
/// ��������ɕ`�悵�Ă����A��C�ɏ������ނ��߂̃o�b�t�@
//=====================================
typedef struct _DEBUG_MSGWINBUFF_WORK DEBUG_MSGWINBUFF_WORK;

extern DEBUG_MSGWINBUFF_WORK *DEBUG_MSGWINSET_CreateBuff( DEBUG_MSGWINSET_WORK *p_wk, HEAPID heapID );
extern void DEBUG_MSGWINSET_DeleteBuff( DEBUG_MSGWINBUFF_WORK *p_wk );

extern void DEBUG_MSGWINSET_PrintBuff( DEBUG_MSGWINBUFF_WORK *p_wk, const DEBUG_MSGWINSET_PRINT_TBL *cp_tbl, PRINT_QUE *p_que );
extern void DEBUG_MSGWINSET_SetBuff( DEBUG_MSGWINBUFF_WORK *p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �V�[�P���X�Ǘ�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _DEBUG_SEQ_WORK DEBUG_SEQ_WORK;

//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
typedef void (*DEBUG_SEQ_FUNCTION)( DEBUG_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_SEQ_WORK *DEBUG_SEQ_Init( void *p_wk_adrs, DEBUG_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void DEBUG_SEQ_Exit( DEBUG_SEQ_WORK *p_wk );
extern void DEBUG_SEQ_Main( DEBUG_SEQ_WORK *p_wk );
extern BOOL DEBUG_SEQ_IsEnd( const DEBUG_SEQ_WORK *cp_wk );
extern void DEBUG_SEQ_SetNext( DEBUG_SEQ_WORK *p_wk, DEBUG_SEQ_FUNCTION seq_function );
extern void DEBUG_SEQ_End( DEBUG_SEQ_WORK *p_wk );
extern void DEBUG_SEQ_SetReservSeq( DEBUG_SEQ_WORK *p_wk, int seq );
extern void DEBUG_SEQ_NextReservSeq( DEBUG_SEQ_WORK *p_wk );

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
  DEBUG_MSGOAM_POS_ABSOLUTE,    //��Έʒu
  DEBUG_MSGOAM_POS_WH_CENTER,  //���΍��W  ���A�����Ƃ��ɒ���
  DEBUG_MSGOAM_POS_H_CENTER,  //���΍��W  �����̂ݒ���
} DEBUG_MSGOAM_POS;

//-------------------------------------
///	���b�Z�[�WOAM
//=====================================
typedef struct _DEBUG_MSGOAM_WORK DEBUG_MSGOAM_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_MSGOAM_WORK * DEBUG_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID );
extern void DEBUG_MSGOAM_Exit( DEBUG_MSGOAM_WORK* p_wk );
extern void DEBUG_MSGOAM_Clear( DEBUG_MSGOAM_WORK* p_wk );
extern void DEBUG_MSGOAM_Print( DEBUG_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void DEBUG_MSGOAM_PrintBuf( DEBUG_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void DEBUG_MSGOAM_SetStrColor( DEBUG_MSGOAM_WORK* p_wk, PRINTSYS_LSB color );
extern void DEBUG_MSGOAM_SetStrPos( DEBUG_MSGOAM_WORK* p_wk, s16 x, s16 y, DEBUG_MSGOAM_POS type );
extern BOOL DEBUG_MSGOAM_PrintMain( DEBUG_MSGOAM_WORK* p_wk );
extern BMPOAM_ACT_PTR DEBUG_MSGOAM_GetBmpOamAct( DEBUG_MSGOAM_WORK* p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���l�㉺
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�WOAM
//=====================================
typedef struct _DEBUG_NUMIN_WORK DEBUG_NUMIN_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern DEBUG_NUMIN_WORK * DEBUG_NUMIN_Init( u8 frm, u8 plt, GFL_FONT *p_font, HEAPID heapID );
extern void DEBUG_NUMIN_Exit( DEBUG_NUMIN_WORK *p_wk );
extern void DEBUG_NUMIN_Start( DEBUG_NUMIN_WORK *p_wk, u8 keta, s32 min, s32 max, s32 now, u8 x, u8 y );
extern void DEBUG_NUMIN_Main( DEBUG_NUMIN_WORK *p_wk );
extern u16  DEBUG_NUMIN_GetNum( const DEBUG_NUMIN_WORK *cp_wk );
extern BOOL DEBUG_NUMIN_IsEnd( const DEBUG_NUMIN_WORK *cp_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ETC
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
extern void DEBUG_UTIL_MainPltAnm( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
extern void DEBUG_UTIL_MainPltAnmLine( NNS_GFD_DST_TYPE type, u16 *p_buff, u16 cnt, u8 plt_num, GXRgb start[], GXRgb end[] );
