//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_battle.h
 *	@brief  �f���`�o�g���`�I���f���ւ̂Ȃ�(�^��Ȃ�)
 *	@author	Toru=Nagihashi
 *	@date		2010.02.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "demo/comm_btl_demo.h"
#include "gamesystem/game_data.h"
#include "battle/battle.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�o�g���v���Z�X�̖߂�l
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS,          //����ɏI��
  WIFIBATTLEMATCH_BATTLELINK_RESULT_DISCONNECT,       //�ؒf���ꂽ���m
  WIFIBATTLEMATCH_BATTLELINK_RESULT_EVIL,             //�s���Ɏ��
} WIFIBATTLEMATCH_BATTLELINK_RESULT;



//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �o�g���v���Z�X�̈���
//=====================================
typedef struct
{
  // [IN]
  GAMEDATA            *p_gamedata;
  BATTLE_SETUP_PARAM  *p_btl_setup_param;
  COMM_BTL_DEMO_PARAM *p_demo_param;

  // [out]
  WIFIBATTLEMATCH_BATTLELINK_RESULT result;

} WIFIBATTLEMATCH_BATTLELINK_PARAM;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�o�g���{�f���Ȃ�
//=====================================
extern const GFL_PROC_DATA	WifiBattleMatch_BattleLink_ProcData;
