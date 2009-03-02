//============================================================================================
/**
 * @file	event_ircbattle.h
 * @brief	�C�x���g�F�o�g���Ăяo��
 * @author	tamada GAMEFREAK inc.
 * @date	2009.01.19
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap.h"

typedef struct _EVENT_IRCBATTLE_WORK EVENT_IRCBATTLE_WORK;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
//extern GMEVENT* EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap);
extern void EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,GMEVENT * event);
extern void EVENT_IrcBattle_SetEnd(EVENT_IRCBATTLE_WORK* pWk);


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
