//======================================================================
/**
 * @file	musical_system.h
 * @brief	�~���[�W�J���p �ėp��`
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUSICAL_SYSTEM_H__
#define MUSICAL_SYSTEM_H__
#include "musical_define.h"
#include "savedata/save_control.h"

typedef struct
{
  SAVE_CONTROL_WORK *saveCtrl;
}MUSICAL_INIT_WORK;

extern GFL_PROC_DATA Musical_ProcData;


//�~���[�W�J���̎Q�����i�����邩���ׂ�
const BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
const BOOL	MUSICAL_SYSTEM_CheckEntryMusicalPokeNo( const u16 mons_no );
const u16	MUSICAL_SYSTEM_ChangeMusicalPokeNumber( POKEMON_PARAM *pokePara );
//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );



#endif MUSICAL_SYSTEM_H__
