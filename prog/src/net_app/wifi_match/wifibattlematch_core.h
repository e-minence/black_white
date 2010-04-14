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
#include "net/dreamworld_netdata.h"
#include "wifibattlematch_net.h"

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
  WIFIBATTLEMATCH_CORE_RESULT_FINISH,           //�A�v���I��
  WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE,      //�o�g����
  WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC,         //�^��
  WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN,   //�V���b�g�_�E���G���[�̂��߁A���O�C�������蒼��
} WIFIBATTLEMATCH_CORE_RESULT;

//-------------------------------------
///	�N�����[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_MODE_START,          //�J�n
  WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE,      //�o�g���I��
  WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR,  //�o�g���I���������G���[
  WIFIBATTLEMATCH_CORE_MODE_ENDREC,         //�^��I��
} WIFIBATTLEMATCH_CORE_MODE;

//-------------------------------------
///	�I�������[�h
//=====================================
typedef enum 
{
  WIFIBATTLEMATCH_CORE_RETMODE_RATE,  //���[�e�B���O���[�h
  WIFIBATTLEMATCH_CORE_RETMODE_FREE,  //�t���[���[�h
  WIFIBATTLEMATCH_CORE_RETMODE_WIFI,  //WIFI���
  WIFIBATTLEMATCH_CORE_RETMODE_NONE,  //���߂��ɔ�����
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
  RNDMATCH_DATA               *p_rndmatch;  //[in ]�Z�[�u�f�[�^

  //�ȉ��A[in ]�풓�f�[�^�u����
  WIFIBATTLEMATCH_ENEMYDATA   *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA   *p_enemy_data;
  DWCSvlResult                *p_svl_result;
  u32                         *p_server_time;
  WIFIBATTLEMATCH_RECORD_DATA *p_record_data;
  WIFIBATTLEMATCH_RECV_DATA   *p_recv_data;
  const BATTLEMATCH_BATTLE_SCORE  *cp_btl_score;
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_gpf_data;
  WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA       *p_sake_data;
  WIFIBATTLEMATCH_NET_DATA    *p_net_data;
} WIFIBATTLEMATCH_CORE_PARAM;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
FS_EXTERN_OVERLAY( wifibattlematch_core );
extern const GFL_PROC_DATA	WifiBattleMatchCore_ProcData;
