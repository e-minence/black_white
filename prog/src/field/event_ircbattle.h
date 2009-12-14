//============================================================================================
/**
 * @file	event_ircbattle.h
 * @brief	�C�x���g�F�o�g���Ăяo��
 * @author	k.ohno
 * @date	2009.01.19
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap.h"

typedef struct _EVENT_IRCBATTLE_WORK EVENT_IRCBATTLE_WORK;


enum EVENTIRCBTL_IBMODE_ENTRY {
  EVENTIRCBTL_ENTRYMODE_NONE=0,
  EVENTIRCBTL_ENTRYMODE_SINGLE,
  EVENTIRCBTL_ENTRYMODE_DOUBLE,
  EVENTIRCBTL_ENTRYMODE_TRI,
  EVENTIRCBTL_ENTRYMODE_ROTATE,
  EVENTIRCBTL_ENTRYMODE_MULTH,
  EVENTIRCBTL_ENTRYMODE_TRADE,
  EVENTIRCBTL_ENTRYMODE_FRIEND,
	EVENTIRCBTL_ENTRYMODE_COMPATIBLE,
  EVENTIRCBTL_ENTRYMODE_EXIT,
  EVENTIRCBTL_ENTRYMODE_RETRY,
};



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
//extern GMEVENT* EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);
//extern void EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * event);
//extern void EVENT_IrcBattle_SetEnd(EVENT_IRCBATTLE_WORK* pWk);


//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

extern void* IrcBattleBeaconGetFunc(void* pWork);
extern int IrcBattleBeaconGetSizeFunc(void* pWork);
extern BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
extern void EVENT_IrcBattleSetType(EVENT_IRCBATTLE_WORK* pWork, int type);
extern int EVENT_IrcBattleGetType(EVENT_IRCBATTLE_WORK* pWork);
extern GAMESYS_WORK* IrcBattle_GetGAMESYS_WORK(EVENT_IRCBATTLE_WORK* pWork);
extern SAVE_CONTROL_WORK* IrcBattle_GetSAVE_CONTROL_WORK(EVENT_IRCBATTLE_WORK* pWork);

extern GMEVENT* EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate);
