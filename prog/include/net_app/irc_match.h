
//============================================================================================
/**
 * @file	  irc_match.h
 * @brief	  �ԊO���}�b�`���O�p
 * @author	k.ohno
 * @date	  2010.02.14
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
//#include "gamesystem/game_event.h"
#include "net_app/pokemontrade.h"
//#include "net_app/irc_compatible.h"
//#include "include/demo/comm_btl_demo.h"



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
	EVENTIRCBTL_ENTRYMODE_MUSICAL,
	EVENTIRCBTL_ENTRYMODE_SUBWAY,
  EVENTIRCBTL_ENTRYMODE_EXIT,
  EVENTIRCBTL_ENTRYMODE_RETRY,
};


typedef struct {
  GAMEDATA* gamedata;         ///< �Q�[���f�[�^:�K�{
  POKEPARTY* pParty;          ///< ����PARTY : �o�g���p
  POKEPARTY* pNetParty[4];    ///< NETParty : �o�g���p
  int selectType;             ///< EVENTIRCBTL_IBMODE_ENTRY�̎� �K�{
} IRC_MATCH_WORK;


