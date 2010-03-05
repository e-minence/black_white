//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		battle_championship_core.h
 *	@brief  �o�g�����R�A
 *	@author	Toru=Nagihashi
 *	@date		2010.03.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//���C�u����
#include <gflib.h>
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
/// ���R�A���[�h
//=====================================
typedef enum
{
  BATTLE_CHAMPIONSHIP_CORE_MODE_MAIN_MEMU, //���C�����j���[�ւ���
  BATTLE_CHAMPIONSHIP_CORE_MODE_WIFI_MENU, //WIFI���j���[�ւ���
  BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_MENU, //LIVE���j���[�ւ���
  BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_START, //���C�u���t���[�J�n�ւ����i����Ȃ��H�j
  BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_BTLEND, //���C�u���t���[�o�g���I����ւ���
  BATTLE_CHAMPIONSHIP_CORE_MODE_LIVE_FLOW_RECEND, //���C�u���t���[�^��I����ւ���
} BATTLE_CHAMPIONSHIP_CORE_MODE;

//-------------------------------------
///	���R�A�߂�l
//=====================================
typedef enum
{
  BATTLE_CHAMPIONSHIP_CORE_RET_TITLE,   //�^�C�g���֍s��
  BATTLE_CHAMPIONSHIP_CORE_RET_WIFICUP, //WIFI���֍s��
  BATTLE_CHAMPIONSHIP_CORE_RET_LIVEBTL, //LIVE�p�o�g���֍s��
  BATTLE_CHAMPIONSHIP_CORE_RET_LIVEREC, //LIVE�p�^��֍s��

} BATTLE_CHAMPIONSHIP_CORE_RET;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����
//=====================================
typedef struct 
{
  GAMEDATA                      *p_gamedata;  //[in ]�Q�[���f�[�^
  BATTLE_CHAMPIONSHIP_CORE_MODE mode;         //[in ]���[�h�ǂ�����J�n���邩
  BATTLE_CHAMPIONSHIP_CORE_RET  ret;          //[out]�߂�l�ǂ��ւ�����
} BATTLE_CHAMPIONSHIP_CORE_PARAM;


//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern const GFL_PROC_DATA BATTLE_CHAMPIONSHIP_CORE_ProcData;
