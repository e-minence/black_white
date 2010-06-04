//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_album.h
 *	@brief  �A���o���A�J�[�h
 *	@author	Toru=Nagihashi
 *	@date		2009.12.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "gamesystem/game_data.h"
#include "print/wordset.h"
#include "print/gf_font.h"
#include "print/printsys.h"

//�Z�[�u�f�[�^
#include "savedata/mystery_data.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �A���o��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�A���o���N�����[�h
//=====================================
typedef enum
{
  MYSTERY_ALBUM_MODE_VIEW,
  MYSTERY_ALBUM_MODE_DELETE,
} MYSTERY_ALBUM_MODE;

//-------------------------------------
///	�A���o���ݒ�\����
//=====================================
typedef struct 
{
  MYSTERY_ALBUM_MODE  mode;
  u16                 back_frm;
  u16                 font_frm;
  GFL_CLUNIT          *p_clunit;
  MYSTERY_DATA        *p_sv;
  GFL_FONT            *p_font;
  PRINT_QUE           *p_que;
  WORDSET             *p_word;
  GFL_MSGDATA         *p_msg;
  GAMEDATA            *p_gamedata;
} MYSTERY_ALBUM_SETUP;

//-------------------------------------
///	�A���o�����[�N
//=====================================
typedef struct _MYSTERY_ALBUM_WORK MYSTERY_ALBUM_WORK;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern MYSTERY_ALBUM_WORK * MYSTERY_ALBUM_Init( const MYSTERY_ALBUM_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_ALBUM_Exit( MYSTERY_ALBUM_WORK *p_wk );
extern void MYSTERY_ALBUM_Main( MYSTERY_ALBUM_WORK *p_wk );
extern void MYSTERY_ALBUM_PrintMain( MYSTERY_ALBUM_WORK *p_wk );
extern BOOL MYSTERY_ALBUM_IsEnd( const MYSTERY_ALBUM_WORK *cp_wk );


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  �J�[�h
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�ݒ�
//=====================================
typedef struct 
{
  u32 back_frm;
  u32 font_frm;
  u32 back_plt_num;
  u32 font_plt_num;
  u32 icon_obj_plt_num;
  u32 silhouette_obj_plt_num;
  GFL_CLUNIT *p_clunit;
  MYSTERY_DATA *p_sv;
  GFL_MSGDATA *p_msg;
  GFL_FONT *p_font;
  PRINT_QUE *p_que;
  WORDSET *p_word;
} MYSTERY_CARD_SETUP;

//-------------------------------------
///	�J�[�h���[�N
//=====================================
typedef struct _MYSTERY_CARD_WORK MYSTERY_CARD_WORK;

//-------------------------------------
///	���\�[�X�i�J�[�h�������\���ؑւ���ۋ��L���郊�\�[�X�j
//=====================================
typedef struct _MYSTERY_CARD_RES MYSTERY_CARD_RES;

//-------------------------------------
///	�p�u���b�N
//====================================
//���\�[�X
extern MYSTERY_CARD_RES * MYSTERY_CARD_RES_Init( const MYSTERY_CARD_SETUP *cp_setup, HEAPID heapID );
extern void MYSTERY_CARD_RES_Exit( MYSTERY_CARD_RES *p_res );
extern void MYSTERY_CARD_RES_PrintMain( MYSTERY_CARD_RES *p_wk );
extern void MYSTERY_CARD_RES_Clear( MYSTERY_CARD_RES *p_wk );
//�J�[�h
extern MYSTERY_CARD_WORK * MYSTERY_CARD_Init( const GIFT_PACK_DATA *cp_data, MYSTERY_CARD_RES *p_res, GAMEDATA *p_gamedata, HEAPID heapID );
extern void MYSTERY_CARD_Exit( MYSTERY_CARD_WORK *p_wk );
extern void MYSTERY_CARD_Main( MYSTERY_CARD_WORK *p_wk );
extern void MYSTERY_CARD_StartEffect( MYSTERY_CARD_WORK *p_wk );
extern void MYSTERY_CARD_EndEffect( MYSTERY_CARD_WORK *p_wk );
extern BOOL MYSTERY_CARD_IsEndEffect( const MYSTERY_CARD_WORK *cp_wk );
extern u32 MYSTERY_CARD_GetWorkSize( void );

//trans�֐����s�Ȃ��܂ŉ�ʔ��f�͍s���܂���
typedef enum
{
  MYSTERY_CARD_TRANS_TYPE_NORMAL, //�ʏ�
  MYSTERY_CARD_TRANS_TYPE_NOBG,   //BG�]���Ȃ�
}MYSTERY_CARD_TRANS_TYPE;
extern void MYSTERY_CARD_Trans( MYSTERY_CARD_WORK *p_wk, MYSTERY_CARD_TRANS_TYPE type );
