//============================================================================
/**
 *
 *	@file		event_battle_call.h
 *	@brief  �C�x���g�F�o�g���Ăяo��
 *	@author hosaka genya
 *	@data		2010.01.22
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "battle/battle.h"
#include "demo/comm_btl_demo.h"
#include "net_app/btl_rec_sel.h"


FS_EXTERN_OVERLAY(event_battlecall);

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  EVENT_BATTLE_ADD_CMD_TBL_TIMING = 200, ///< ����NO�@�퓬�J�n
  EVENT_BATTLE_LIST_DEMO_TIMING,				 ///< ����NO�@�}���`���X�g�Ăяo��
};

// PROC
extern const GFL_PROC_DATA CommBattleCommProcData;

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
  
//--------------------------------------------------------------
///	�ʐM�o�g���Ăяo��PROC
//==============================================================
typedef struct {
  // [IN]
  GAMEDATA* gdata;
  BATTLE_SETUP_PARAM  *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
  int battle_mode;      ///<BATTLE_MODE_xxx
  int fight_count;      ///<�A����
  BOOL error_auto_disp;         ///<TRUE:�G���[�����������ꍇ�A��ʂ𔲂��鎞�ɃG���[��ʂ�\��
                                ///<FALSE:�G���[���������Ă���ʂ𔲂��邾��
  // [PRIVATE]
  BTL_REC_SEL_PARAM  btl_rec_sel_param;
} COMM_BATTLE_CALL_PROC_PARAM;


//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {
  BATTLE_SETUP_PARAM *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
  BOOL error_auto_disp;         ///<TRUE:�G���[�����������ꍇ�A��ʂ𔲂��鎞�ɃG���[��ʂ�\��
                                ///<FALSE:�G���[���������Ă���ʂ𔲂��邾��
} EV_BATTLE_CALL_PARAM;


//=============================================================================
/**
 *								EXTERN�錾
 */
//
//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� EV_BATTLE_CALL_PARAM*��n���B
//=====================================

//==================================================================
/**
 * �C�x���g�쐬�F�ʐM�o�g���Ăяo��
 *
 * @param   gsys		
 * @param   work
 */
//==================================================================
extern GMEVENT * EVENT_CallCommBattle(GAMESYS_WORK * gsys, void *work );

