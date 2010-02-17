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
#include "savedata/save_control.h"
#include "gamesystem/game_comm.h"

typedef struct
{
  BOOL          isComm;
  BOOL          isDebug; //Musical�̏I�����ɏ����InitWork��PP���J������
  POKEMON_PARAM *pokePara;
  SAVE_CONTROL_WORK *saveCtrl;
  GAME_COMM_SYS_PTR gameComm;
}MUSICAL_INIT_WORK;

typedef struct
{
  void *eventWork;  //MUSICAL_EVENT_WORK
  void *commWork;   //MUS_COMM_WORK
  
  COMM_ENTRY_MENU_PTR entryWork;
  u16                 *scriptRet;
  u16                 commSyncNo;
}MUSICAL_SCRIPT_WORK;

extern GFL_PROC_DATA Musical_ProcData;


//�~���[�W�J���̎Q�����i�����邩���ׂ�
const BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
const BOOL	MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( const u16 mons_no );
const u16	MUSICAL_SYSTEM_ChangeMusicalPokeNumber( const u16 mons_no );
const u16 MUSICAL_SYSTEM_GetMusicalPokemonRandom( void );
//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPokeParam( u16 monsno , u8 sex , u8 form , u8 rare , HEAPID heapId );


