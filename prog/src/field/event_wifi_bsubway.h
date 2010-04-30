//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_wifi_bsubway.h
 *	@brief  Wifi�o�g���T�u�E�F�C  �C�x���g
 *	@author	tomoya takahashi
 *	@date		2010.02.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "net_app/wifi_bsubway.h"



FS_EXTERN_OVERLAY(event_wifibsubway);


//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� EV_WIFIBATTLEMATCH_PARAM*��n���B
//=====================================
//-------------------------------------
///	�p�����[�^
//=====================================
typedef struct {
  
  WIFI_BSUBWAY_MODE mode;
  u16* ret_wk;
  
} EV_WIFIBSUBWAY_PARAM;


//-----------------------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C  WiFi�@Data�A�b�v���[�h�@�_�E�����[�h
 *
 *�@mode 
      WIFI_BSUBWAY_MODE_SCORE_UPLOAD,         // �O��̋L�^���A�b�v���[�h
      WIFI_BSUBWAY_MODE_GAMEDATA_DOWNLOAD,    // �Q�[�������_�E�����[�h
      WIFI_BSUBWAY_MODE_SUCCESSDATA_DOWNLOAD, // �������_�E�����[�h
 *
 */
//-----------------------------------------------------------------------------
extern GMEVENT * WIFI_BSUBWAY_EVENT_CallStart(
    GAMESYS_WORK *gsys, void* p_work );



#ifdef _cplusplus
}	// extern "C"{
#endif



