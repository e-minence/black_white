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
#include "net_app/battle_recorder.h"	//BR_MODE
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
	BR_BTN_SYS_INPUT_NONE,			//�����Ă��Ȃ��ior��������̓��쒆�j
	BR_BTN_SYS_INPUT_CHANGESEQ,	//�ʃV�[�P���X�ɔ��
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
extern BR_BTN_SYS_WORK *BR_BTN_SYS_Init( BR_MODE mode, GFL_CLUNIT *p_unit, HEAPID heapID );
extern void BR_BTN_SYS_Exit( BR_BTN_SYS_WORK *p_wk );
extern void BR_BTN_SYS_Main( BR_BTN_SYS_WORK *p_wk );
extern BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq );
extern BR_BTN_SYS_STATE BR_BTN_SYS_GetState( const BR_BTN_SYS_WORK *cp_wk );
