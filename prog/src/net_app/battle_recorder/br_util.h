//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_util.h
 *	@brief  �ePROC�Ŏg�����ʃ��W���[���n
 *	@author	Toru=Nagihashi
 *	@date		2009.11.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "print/gf_font.h"
#include "print/printsys.h"
#include "system/bmp_menuwork.h"

#include "br_res.h"

#include "savedata/gds_profile.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					BMPWIN���b�Z�[�W�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _BR_MSGWIN_WORK BR_MSGWIN_WORK;
//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_MSGWIN_WORK * BR_MSGWIN_Init( u16 frm, u8 x, u8 y, u8 w, u8 h, u8 plt, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_MSGWIN_Exit( BR_MSGWIN_WORK* p_wk );
extern void BR_MSGWIN_Print( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
extern void BR_MSGWIN_PrintBuf( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
extern void BR_MSGWIN_PrintColor( BR_MSGWIN_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, PRINTSYS_LSB lsb );
void BR_MSGWIN_PrintBufColor( BR_MSGWIN_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font, PRINTSYS_LSB lsb );
extern BOOL BR_MSGWIN_PrintMain( BR_MSGWIN_WORK* p_wk );

typedef enum
{
  BR_MSGWIN_POS_ABSOLUTE,
  BR_MSGWIN_POS_WH_CENTER,
}BR_MSGWIN_POS;
extern void BR_MSGWIN_SetPos( BR_MSGWIN_WORK* p_wk, s16 x, s16 y, BR_MSGWIN_POS type );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �������{�[�����o
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�����̎��
//=====================================
typedef enum
{ 
  BR_BALLEFF_MOVE_NOP,           //�����Ȃ�(�\������Ȃ�)
  BR_BALLEFF_MOVE_EMIT,          //���˂ɓ���          STOP
  BR_BALLEFF_MOVE_LINE,          //���̓���            STOP
  BR_BALLEFF_MOVE_OPENING,       //�J�n�̓���          STOP
  BR_BALLEFF_MOVE_BIG_CIRCLE,    //�傫���~��`��      LOOP
  BR_BALLEFF_MOVE_CIRCLE,        //�~��`��            LOOP
  BR_BALLEFF_MOVE_CIRCLE_CONT,   //�ꏊ��ς��ĉ~��`��LOOP
  BR_BALLEFF_MOVE_EMIT_FOLLOW,   //���˂ɓ����A���čs�� STOP
  BR_BALLEFF_MOVE_OPENING_TOUCH, //�J�n���̃^�b�`     �@STOP
}BR_BALLEFF_MOVE;
//-------------------------------------
///	�J�[�\�����[�N
//=====================================
typedef struct _BR_BALLEFF_WORK BR_BALLEFF_WORK;

//-------------------------------------
///	�O�����J
//=====================================
extern BR_BALLEFF_WORK *BR_BALLEFF_Init( GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, CLSYS_DRAW_TYPE draw, HEAPID heapID );
extern void BR_BALLEFF_Exit( BR_BALLEFF_WORK *p_wk );
extern void BR_BALLEFF_Main( BR_BALLEFF_WORK *p_wk );

extern void BR_BALLEFF_StartMove( BR_BALLEFF_WORK *p_wk, BR_BALLEFF_MOVE move, const GFL_POINT *cp_pos );
extern BOOL BR_BALLEFF_IsMoveEnd( const BR_BALLEFF_WORK *cp_wk );

extern void BR_BALLEFF_SetAnmSeq( BR_BALLEFF_WORK *p_wk, int seq );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���X�g�\���\����
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	SELECT�̕Ԃ�l
//=====================================
#define BR_LIST_SELECT_NONE (0xFFFFFFFF)

//-------------------------------------
///	���X�g�^�C�v
//=====================================
typedef enum
{
  BR_LIST_TYPE_TOUCH,  //�^�b�`���X�g
  BR_LIST_TYPE_CURSOR, //�J�[�\�����X�g
} BR_LIST_TYPE;

//-------------------------------------
///	���X�g�ݒ���
//=====================================
typedef struct 
{
  const BMP_MENULIST_DATA *cp_list;
  u32                     list_max;
  u8  x;
  u8  y;
  u8  w;
  u8  h;
  u8  plt;
  u8  frm;
  u8  str_line; //���s�̕�����(���L�����g����)
  BR_LIST_TYPE    type;
  BR_RES_WORK     *p_res;
  GFL_CLUNIT      *p_unit;
  BR_BALLEFF_WORK *p_balleff_main;  //[in]���X�g�J�[�\���p
  BR_BALLEFF_WORK *p_balleff_sub;   //[in]�^�b�`���o�p
  BR_LIST_POS     *p_pos; //[in/out]���X�g�ʒu�ibr_inner.h�ɒ�`���Ă���j
} BR_LIST_PARAM;

//-------------------------------------
///	���X�g
//=====================================
typedef struct _BR_LIST_WORK BR_LIST_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_LIST_WORK * BR_LIST_Init( const BR_LIST_PARAM *cp_param, HEAPID heapID );
extern void BR_LIST_Exit( BR_LIST_WORK* p_wk );
extern void BR_LIST_Main( BR_LIST_WORK* p_wk );

extern BOOL BR_LIST_IsMoveEnable( const BR_LIST_WORK* cp_wk );
extern u32 BR_LIST_GetSelect( const BR_LIST_WORK* cp_wk );

//-------------------------------------
///	���X�g�g��
//=====================================
//  �ʏ�͏�L�֐��݂̂œ��삷�邪�A�O���������ȕ������������݂����Ƃ��́A
//  BMP�ɕ��������������ŁA���L�֐��ɓn�����ƂŁA�X�N���[���ɑΉ��ł���
//  ���̍ہABMP_MENULIST_DATA��str��NULL�ɂ��邱�ƁB
extern void BR_LIST_SetBmp( BR_LIST_WORK* p_wk, u16 idx, GFL_BMP_DATA *p_src );
extern GFL_BMP_DATA *BR_LIST_GetBmp( const BR_LIST_WORK* cp_wk, u16 idx );

typedef enum
{
  BR_LIST_PARAM_IDX_CURSOR_POS,
  BR_LIST_PARAM_IDX_LIST_POS,
  BR_LIST_PARAM_IDX_MOVE_TIMING,
  BR_LIST_PARAM_IDX_LIST_MAX,

} BR_LIST_PARAM_IDX;
extern u32 BR_LIST_GetParam( const BR_LIST_WORK* cp_wk, BR_LIST_PARAM_IDX param );
extern void BR_LIST_Write( BR_LIST_WORK *p_wk );
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  ���ʃe�L�X�g
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�e�L�X�g���[�N
//    �����I�ɂ�MSGWIN�Ɠ�������
//    �t���[���̏������݂��s�Ȃ�
//=====================================
typedef BR_MSGWIN_WORK BR_TEXT_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_TEXT_WORK * BR_TEXT_Init( BR_RES_WORK *p_res, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_TEXT_Exit( BR_TEXT_WORK *p_wk, BR_RES_WORK *p_res );
extern void BR_TEXT_Print( BR_TEXT_WORK* p_wk, const BR_RES_WORK *cp_res, u32 strID );
extern void BR_TEXT_PrintBuff( BR_TEXT_WORK* p_wk, const BR_RES_WORK *cp_res, const STRBUF *cp_strbuf );
extern BOOL BR_TEXT_PrintMain( BR_TEXT_WORK* p_wk );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �v���t�B�[����ʍ쐬
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�v���t�B�[�����[�N
//=====================================
typedef struct _BR_PROFILE_WORK BR_PROFILE_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern BR_PROFILE_WORK * BR_PROFILE_CreateMainDisplay( const GDS_PROFILE_PTR cp_profile, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit, PRINT_QUE *p_que, HEAPID heapID );
extern void BR_PROFILE_DeleteMainDisplay( BR_PROFILE_WORK *p_wk );
extern BOOL BR_PROFILE_PrintMain( BR_PROFILE_WORK *p_wk );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					  �V�[�P���X�Ǘ�
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _BR_SEQ_WORK BR_SEQ_WORK;

//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
typedef void (*BR_SEQ_FUNCTION)( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�O�����J
//=====================================
extern BR_SEQ_WORK *BR_SEQ_Init( void *p_wk_adrs, BR_SEQ_FUNCTION seq_function, HEAPID heapID );
extern void BR_SEQ_Exit( BR_SEQ_WORK *p_wk );
extern void BR_SEQ_Main( BR_SEQ_WORK *p_wk );
extern BOOL BR_SEQ_IsEnd( const BR_SEQ_WORK *cp_wk );
extern void BR_SEQ_SetNext( BR_SEQ_WORK *p_wk, BR_SEQ_FUNCTION seq_function );
extern void BR_SEQ_End( BR_SEQ_WORK *p_wk );
extern void BR_SEQ_SetReservSeq( BR_SEQ_WORK *p_wk, int seq );
extern void BR_SEQ_NextReservSeq( BR_SEQ_WORK *p_wk );
extern BOOL BR_SEQ_IsComp( const BR_SEQ_WORK *cp_wk, BR_SEQ_FUNCTION seq_function );

