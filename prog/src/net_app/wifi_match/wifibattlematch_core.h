//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.h
 *	@brief	WIFI�̃o�g���}�b�`�R�A���
 *	@author	Toru=Nagihashi
 *	@date		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net_app/wifibattlematch.h"
#include "battle/battle.h"
#include "wifibattlematch_data.h"
#include "savedata/rndmatch_savedata.h"

#include <dwc.h>
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�I���R�[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_RESULT_FINISH,       //�A�v���I��
  WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE,  //�o�g����
  WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN,  //�V���b�g�_�E���G���[�̂��߁A���O�C�������蒼��
} WIFIBATTLEMATCH_CORE_RESULT;

//-------------------------------------
///	�N�����[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_MODE_START,      //�J�n
  WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE,  //�o�g���I��
} WIFIBATTLEMATCH_CORE_MODE;

//-------------------------------------
///	�I�������[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_RETMODE_RATE,  //���[�e�B���O���[�h
  WIFIBATTLEMATCH_CORE_RETMODE_FREE,  //�t���[���[�h
} WIFIBATTLEMATCH_CORE_RETMODE;

//=============================================================================
/**
 *					�\����
*/
//=============================================================================
//-------------------------------------
///	�R�A�v���Z�X�̈���
//=====================================
typedef struct
{
  WIFIBATTLEMATCH_CORE_RETMODE retmode;     //[in/out]�O��I�����̃��[�h
  WIFIBATTLEMATCH_CORE_RESULT result;       //[out]�I���R�[�h
  WIFIBATTLEMATCH_PARAM       *p_param;     //[in ]�O������
  WIFIBATTLEMATCH_CORE_MODE   mode;         //[in ]�N�����[�h
  BtlResult                   btl_result;   //[in ]�o�g������
  RNDMATCH_DATA               *p_rndmatch;  //[in ]�Z�[�u�f�[�^

  //�ȉ��A[in ]�풓�f�[�^�u����
  WIFIBATTLEMATCH_ENEMYDATA   *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA   *p_enemy_data;
  DWCSvlResult                *p_svl_result;

} WIFIBATTLEMATCH_CORE_PARAM;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
FS_EXTERN_OVERLAY( wifibattlematch_core );
extern const GFL_PROC_DATA	WifiBattleMatchCore_ProcData;
