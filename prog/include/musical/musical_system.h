//======================================================================
/**
 * @file	musical_system.h
 * @brief	�~���[�W�J���p �ėp��`
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#pragma once

#include "musical_define.h"
#include "net_app/comm_entry_menu.h"
#include "net_app/irc_match.h"
#include "savedata/save_control.h"
#include "gamesystem/game_comm.h"

typedef struct
{
  void *eventWork;  //MUSICAL_EVENT_WORK
  void *commWork;   //MUS_COMM_WORK
  
  //�X�N���v�g���Ŏg�p
  //�ʐM�󂯕t���p
  COMM_ENTRY_MENU_PTR entryWork;
   IRC_MATCH_WORK     irEntryWork;
  u16                 *scriptRet;

  u16                 commSyncNo;
  
  
}MUSICAL_SCRIPT_WORK;

//�~���[�W�J���̎Q�����i�����邩���ׂ�
const BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
const BOOL	MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( const u16 mons_no );
const u16	MUSICAL_SYSTEM_ChangeMusicalPokeNumber( const u16 mons_no );
const u16 MUSICAL_SYSTEM_GetMusicalPokemonRandom( void );
//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPokeParam( u16 monsno , u8 sex , u8 form , u8 rare , HEAPID heapId );


