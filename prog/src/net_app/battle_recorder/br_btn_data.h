//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn_data.h
 *	@brief	�{�^�����
 *	@author	Toru=Nagihashi
 *	@date		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "system/main.h"  //HEAPID
#include "br_inner.h"			//BR_MENUID
#include "net_app/battle_recorder.h"	//BR_MODE
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�������Ă��Ȃ��l
//=====================================
#define BR_BTN_DATA_NONE	(0xFFFF)

#define BR_BTN_DATA_WIDTH		(0)
#define BR_BTN_DATA_HEIGHT	(0)

//-------------------------------------
///	�{�^���̎��
//=====================================
typedef enum
{
	BR_BTN_TYPE_SELECT,				//�I��p�{�^��
	BR_BTN_TYPE_RETURN,				//1�O�̃��j���[�֖߂�p�{�^��
	BR_BTN_TYPE_EXIT,					//�o�g�����R�[�_�[�I���p�{�^��
	BR_BTN_TYPE_MYRECORD,			//�����̋L�^�{�^��
	BR_BTN_TYPE_OTHERRECORD,	//�N���̋L�^�{�^��
	BR_BTN_TYPE_DELETE_MY,		//�����̋L�^�������{�^��
	BR_BTN_TYPE_DELETE_OTHER,	//�N���̋L�^�������{�^��
	BR_BTN_TYPE_CHANGESEQ,		//�V�[�P���X�ύX�{�^��
} BR_BTN_TYPE;

//-------------------------------------
///	�{�^���f�[�^���J���C���f�b�N�X
//=====================================
typedef enum
{
	BR_BTN_DATA_PARAM_X,			//�ʒuX
	BR_BTN_DATA_PARAM_Y,			//�ʒuY
	BR_BTN_DATA_PARAM_MENUID,	//�����̑����Ă��郁�j���[ID
	BR_BTN_DATA_PARAM_MSGID,	//�����̃A�C�R���ɕ\�����郁�b�Z�[�WID
	BR_BTN_DATA_PARAM_ANMSEQ,	//�����̃A�C�R���̃A�j���V�[�P���X
	BR_BTN_DATA_PARAM_TYPE,		//�{�^���̎��
	BR_BTN_DATA_PARAM_DATA,		//��ޕʂ̃f�[�^

	BR_BTN_DATA_PARAM_MAX
} BR_BTN_DATA_PARAM;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�{�^�����
//=====================================
typedef struct _BR_BTN_DATA  BR_BTN_DATA;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�f�[�^�擾
//=====================================
extern const BR_BTN_DATA * BR_BTN_DATA_SYS_GetData( BR_MENUID menuID, u16 btnID );
extern u32 BR_BTN_DATA_SYS_GetDataNum( BR_MENUID menuID );
extern u32 BR_BTN_DATA_SYS_GetDataMax( void );
extern BR_MENUID BR_BTN_DATA_SYS_GetStartMenuID( BR_MODE mode );
//-------------------------------------
///	�������擾
//=====================================
extern u16 BR_BTN_DATA_GetParam( const BR_BTN_DATA *cp_data, BR_BTN_DATA_PARAM paramID );
