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
#include "net/dreamworld_netdata.h"
#include "app/box2.h"
#include "app/box_mode.h"


typedef enum{
  GSYNC_CALLTYPE_INFO,    // ��Ԃ��m�F>�_�E�����[�h
  GSYNC_CALLTYPE_BOXSET,  // BOX�|�P�����Z�b�g��
  GSYNC_CALLTYPE_POKELIST,    // �I���\�ȃ|�P�����_�E�����[�h
  GSYNC_CALLTYPE_BOXNULL,    // BOX�Ƀ|�P����������
} GSYNC_CALLTYPE_ENUM;



typedef struct {
  GAMESYS_WORK * gsys;
  GAMEDATA * gameData;
  SAVE_CONTROL_WORK *ctrl;
  BATTLE_SETUP_PARAM para;
  int selectType;
  BOOL push;
  u16 boxNo;
  u16 boxIndex;
  int PlayBGM;
  DREAM_WORLD_SERVER_POKEMONLIST_DATA selectPokeList;
  WIFILOGIN_PARAM aLoginWork;
  BOX2_GFL_PROC_PARAM boxParam;
} EVENT_GSYNC_WORK;

extern void EVENT_GSYNC_SetEnd(EVENT_GSYNC_WORK* pWk);


extern GMEVENT* EVENT_GSync(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate);

