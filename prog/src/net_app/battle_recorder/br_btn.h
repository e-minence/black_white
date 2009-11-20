//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn.h
 *	@brief	�o�g�����R�[�_�[	�{�^���Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "system/main.h"  //HEAPID
#include "br_menu_proc.h"	//MENUID
#include "br_res.h"
#include "br_inner.h"
#include "system/bmp_oam.h"
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�{�^���Ǘ��V�X�e���iMENU_PROC�ł����g��Ȃ��͂��j
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^�����
//=====================================
typedef enum
{
	BR_BTN_SYS_STATE_WAIT,	//�^�b�`�҂�
	BR_BTN_SYS_STATE_HANGER_MOVE,	//�ړ���
	BR_BTN_SYS_STATE_BTN_CHANGE,	//�{�^���؂�ւ���
	BR_BTN_SYS_STATE_APPEAR_MOVE,
	BR_BTN_SYS_STATE_INPUT,
} BR_BTN_SYS_STATE;

//-------------------------------------
///	�{�^������
//=====================================
typedef enum
{
	BR_BTN_SYS_INPUT_NONE,						//�����Ă��Ȃ��ior��������̓��쒆�j
	BR_BTN_SYS_INPUT_CHANGESEQ,				//�ʃV�[�P���X�ɔ��
	BR_BTN_SYS_INPUT_EXIT,			//�I��
} BR_BTN_SYS_INPUT;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�o�g�����R�[�_�[�@�{�^���Ǘ�
//=====================================
typedef struct _BR_BTN_SYS_WORK BR_BTN_SYS_WORK;

//=============================================================================
/**
 *					PUBILIC�֐�
*/
//=============================================================================
extern BR_BTN_SYS_WORK *BR_BTN_SYS_Init( BR_MENUID menuID, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, const BR_BTLREC_SET *cp_rec, HEAPID heapID );
extern void BR_BTN_SYS_Exit( BR_BTN_SYS_WORK *p_wk );
extern void BR_BTN_SYS_Main( BR_BTN_SYS_WORK *p_wk );
extern BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq, u32 *p_param );
extern BR_BTN_SYS_STATE BR_BTN_SYS_GetState( const BR_BTN_SYS_WORK *cp_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�{�^���P�̍쐬�֐�(�e�v���b�N�Ŏg�p����)
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�o�g�����R�[�_�[�@�{�^���Ǘ�
//=====================================
typedef struct _BR_BTN_WORK BR_BTN_WORK;

extern BR_BTN_WORK * BR_BTN_Init( const GFL_CLWK_DATA *cp_cldata, u16 msgID, CLSYS_DRAW_TYPE display, GFL_CLUNIT *p_unit, BMPOAM_SYS_PTR p_bmpoam, GFL_FONT *p_font, GFL_MSGDATA *p_msg, const BR_RES_OBJ_DATA *cp_res, HEAPID heapID );
extern void BR_BTN_Exit( BR_BTN_WORK *p_wk );
extern BOOL BR_BTN_GetTrg( const BR_BTN_WORK *cp_wk, u32 x, u32 y );
extern void BR_BTN_SetPos( BR_BTN_WORK *p_wk, u32 x, u32 y );
 

