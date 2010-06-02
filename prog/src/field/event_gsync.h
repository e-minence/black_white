//============================================================================================
/**
 * @file	  event_gsync.h
 * @brief	  �C�x���g�FGAMESYNC
 * @author	k.ohno
 * @date	  2009.01.19
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net/dreamworld_netdata.h"
#include "app/box2.h"
#include "app/box_mode.h"


FS_EXTERN_OVERLAY(event_gsync);



//-----------------------------------------------------------------------------
/**
 *    �I�[�o�[���C�Ǘ��{�C�x���g�N��
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� FIELDMAP_WORK*��n���B
//=====================================
extern GMEVENT* EVENT_CallGSync( GAMESYS_WORK* gsys, void* work );



//-----------------------------------------------------------------------------
/**
 */
//-----------------------------------------------------------------------------
typedef enum{
  GSYNC_CALLTYPE_INFO,    // ��Ԃ��m�F>�_�E�����[�h
  GSYNC_CALLTYPE_BOXSET,  // BOX�|�P�����Z�b�g��
  GSYNC_CALLTYPE_BOXSET_NOACC,// BOX�|�P�����Z�b�g��NOACC
  GSYNC_CALLTYPE_POKELIST,    // �I���\�ȃ|�P�����_�E�����[�h
  GSYNC_CALLTYPE_BOXNULL,    // BOX�Ƀ|�P����������
} GSYNC_CALLTYPE_ENUM;



typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gameData;
  SAVE_CONTROL_WORK *ctrl;
  DREAM_WORLD_SERVER_STATUS_DATA aDreamStatus;  //�_�E�����[�h���Ă����X�e�[�^�X�l
  BOOL baDreamStatusON;  //�_�E�����[�h������1
  //  BATTLE_SETUP_PARAM para;
  DWCSvlResult aSVL;
  int selectType;
  BOOL push;
  u16 boxNo;
  u16 boxIndex;
  int PlayBGM;
  DREAM_WORLD_SERVER_POKEMONLIST_DATA selectPokeList;
  WIFILOGIN_PARAM aLoginWork;
  WIFILOGOUT_PARAM   logout;
  BOX2_GFL_PROC_PARAM boxParam;
} EVENT_GSYNC_WORK;

extern void EVENT_GSYNC_SetEnd(EVENT_GSYNC_WORK* pWk);





