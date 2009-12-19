//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_res.h
 *	@brief	�o�g�����R�[�_�[���\�[�X�Ǘ�
 *	@author	Toru=Nagihashi
 *	@date		2009.11.11
 *
 *	�e�v���Z�X�𔲂��Ă����\�[�X���c�邱�Ƃ��l����
 *	���\�[�X�Ǘ����[�N���g���ǂݍ���
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "print/gf_font.h"
#include "print/wordset.h"

#include "br_core.h"	//BR_PROCID
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	BG
//=====================================
typedef enum
{
	BR_RES_BG_START_M,
	BR_RES_BG_START_S,
  BR_RES_BG_TEXT_M,

	BR_RES_BG_RECORD_S,
	BR_RES_BG_RECORD_M_BTL_SINGLE,
	BR_RES_BG_RECORD_M_BTL_DOUBLE,
	BR_RES_BG_RECORD_M_PROFILE,

  BR_RES_BG_SUBWAY_S,
  BR_RES_BG_SUBWAY_M_SINGLE,
  BR_RES_BG_SUBWAY_M_DOUBLE,
  BR_RES_BG_SUBWAY_M_MULTI,
  BR_RES_BG_SUBWAY_M_WIFI,
  BR_RES_BG_SUBWAY_M_NONE,

  BR_RES_BG_RNDMATCH_S,
  BR_RES_BG_RNDMATCH_M_RND,
  BR_RES_BG_RNDMATCH_M_RATE,
  BR_RES_BG_RNDMATCH_M_FREE,
  BR_RES_BG_RNDMATCH_M_NONE,

  BR_RES_BG_POKESEARCH_S_HEAD,
  BR_RES_BG_POKESEARCH_S_LIST,

  BR_RES_BG_PHOTO_S,
  BR_RES_BG_PHOTO_SEND_S,

  BR_RES_BG_BVRANK,

  BR_RES_BG_BVSEARCH_M,
  BR_RES_BG_BVSEARCH_MENU_S,
  BR_RES_BG_BVSEARCH_PLACE_S,

  BR_RES_BG_CODEIN_NUMBER_S,

} BR_RES_BGID;



////-------------------------------------
///	OBJ
//=====================================
typedef enum
{
	BR_RES_OBJ_BROWSE_BTN_M,		//�u���E�U�p�{�^���A���C��
	BR_RES_OBJ_BROWSE_BTN_S,		//�u���E�U�p�{�^���A���C��
	BR_RES_OBJ_LINE,						//�^�O�A���C��
	BR_RES_OBJ_SHORT_BTN_S,			//�Z���{�^��	�{	�X�N���[���o�[
  BR_RES_OBJ_ALLOW_S,         //���
  BR_RES_OBJ_ALLOW_M,         //���

  BR_RES_OBJ_NUM_S,           //�i���o�[
  BR_RES_OBJ_NUM_CURSOR_S,    //�i���o�[�J�[�\��

	BR_RES_OBJ_MAX
} BR_RES_OBJID;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	OBJ�p���\�[�X
//=====================================
typedef struct
{
	u32 nce;
	u32 ncg;
	u32 ncl;
}BR_RES_OBJ_DATA;

//-------------------------------------
///	���\�[�X���[�N
//=====================================
typedef struct _BR_RES_WORK BR_RES_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���\�[�X�Ǘ��쐬
//=====================================
extern BR_RES_WORK *BR_RES_Init( HEAPID heapID );
extern void BR_RES_Exit( BR_RES_WORK *p_wk );

//-------------------------------------
///	�O���t�B�b�N�f��
//=====================================
//BG�̓��j���[�ɂ��������̂�ǂݍ���
extern void BR_RES_LoadBG( BR_RES_WORK *p_wk, BR_RES_BGID bgID, HEAPID heapID );
extern void BR_RES_UnLoadBG( BR_RES_WORK *p_wk, BR_RES_BGID bgID );

//OBJ�͌ʂɓǂݍ���
extern void BR_RES_LoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID, HEAPID heapID );
extern BOOL BR_RES_GetOBJRes( const BR_RES_WORK *cp_wk, BR_RES_OBJID objID, BR_RES_OBJ_DATA *p_data );
extern void BR_RES_UnLoadOBJ( BR_RES_WORK *p_wk, BR_RES_OBJID objID );

//���ʑf��
extern void BR_RES_LoadCommon( BR_RES_WORK *p_wk, CLSYS_DRAW_TYPE type, HEAPID heapID );
extern void BR_RES_UnLoadCommon( BR_RES_WORK *p_wk, CLSYS_DRAW_TYPE type );

//-------------------------------------
///	���̑��f��
//=====================================
extern GFL_FONT * BR_RES_GetFont( const BR_RES_WORK *cp_wk );
extern GFL_MSGDATA * BR_RES_GetMsgData( const BR_RES_WORK *cp_wk );
extern WORDSET * BR_RES_GetWordSet( const BR_RES_WORK *cp_wk );
