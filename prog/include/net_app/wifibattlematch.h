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
///	�N�����[�h
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_MODE_MAINMENU,  //�^�C�g������i�ށA���C�����j���[
	WIFIBATTLEMATCH_MODE_RANDOM,    //�|�P�Z����WIFI�J�E���^�[���炷���ށA�����_���ΐ�
} WIFIBATTLEMATCH_MODE;

//-------------------------------------
///	���[�h  �����Ŏg�p����l�ł�
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_TYPE_WIFICUP,
	WIFIBATTLEMATCH_TYPE_LIVECUP,
	WIFIBATTLEMATCH_TYPE_RNDRATE,
	WIFIBATTLEMATCH_TYPE_RNDFREE,

  WIFIBATTLEMATCH_TYPE_MAX
} WIFIBATTLEMATCH_TYPE;

//-------------------------------------
///	�g�p�|�P����  �����_���ΐ�̂Ƃ��̂ݐݒ�
//=====================================
typedef enum
{
	WIFIBATTLEMATCH_POKE_TEMOTI,      //�莝���|�P
	WIFIBATTLEMATCH_POKE_BTLBOX,      //�o�g���{�b�N�X�|�P

} WIFIBATTLEMATCH_POKE;

//-------------------------------------
///	�퓬�^�C�v    �����_���ΐ�̂Ƃ��̂ݐݒ�
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
  GAMEDATA                  *p_game_data; //[in ]�Q�[���f�[�^ NULL�̏ꍇ�͓����ō쐬����
  WIFIBATTLEMATCH_BTLRULE   btl_rule;     //[in ]�o�g�����[��
	WIFIBATTLEMATCH_MODE	    mode;         //[in ]�N�����[�h
  WIFIBATTLEMATCH_POKE      poke;         //[in ]�I��p�p�[�e�B�i�莝��or�o�g���{�b�N�X�̂͂��j
  BOOL                      is_auto_release;//[in]�A�h���X������������t���O
} WIFIBATTLEMATCH_PARAM;


//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
FS_EXTERN_OVERLAY( wifibattlematch_sys );
extern const GFL_PROC_DATA	WifiBattleMatch_ProcData;
