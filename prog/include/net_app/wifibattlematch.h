//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch.h
 *	@brief	WIFI�̃o�g���}�b�`���
 *	@author	Toru=Nagihashi
 *	@date		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "battle/battle.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_MODE_RANDOM,  //�����_���ΐ�
	WIFIBATTLEMATCH_MODE_WIFI,    //WIFI���
	WIFIBATTLEMATCH_MODE_LIVE,    //���C�u���

} WIFIBATTLEMATCH_MODE;

//-------------------------------------
///	�g�p�|�P����
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_POKE_TEMOTI,      //�莝���|�P
	WIFIBATTLEMATCH_POKE_BTLBOX,      //�o�g���{�b�N�X�|�P

} WIFIBATTLEMATCH_POKE;

//-------------------------------------
///	�퓬�^�C�v
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_BTLRULE_SINGLE,
  WIFIBATTLEMATCH_BTLRULE_DOUBLE,
  WIFIBATTLEMATCH_BTLRULE_TRIPLE,
  WIFIBATTLEMATCH_BTLRULE_ROTATE,
  WIFIBATTLEMATCH_BTLRULE_SHOOTER,
} WIFIBATTLEMATCH_BTLRULE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	PROC�p�����[�^
//    ���L�͏��PROC�Ŏ����������Ȃ��󋵂�����̂ŁA
//    ���̏ꍇ��is_auto_release��TRUE�ɂ��Ĉ�����n�����ƂŁA
//    �����ŉ������
//=====================================
typedef struct 
{
  GAMEDATA              *p_game_data; //[in ]�Q�[���f�[�^ NULL�̏ꍇ�͓����ō쐬����
  WIFIBATTLEMATCH_BTLRULE   btl_rule;     //[in ]�o�g�����[��
	WIFIBATTLEMATCH_MODE	mode;         //[in ]�N�����[�h
  WIFIBATTLEMATCH_POKE  poke;         //[in ]�I��p�p�[�e�B�i�莝��or�o�g���{�b�N�X�̂͂��j
  BOOL                  is_auto_release;//[in]�A�h���X������������t���O
} WIFIBATTLEMATCH_PARAM;


//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
FS_EXTERN_OVERLAY( wifibattlematch_sys );
extern const GFL_PROC_DATA	WifiBattleMaptch_ProcData;
