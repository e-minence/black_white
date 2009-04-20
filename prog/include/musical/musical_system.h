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


//�~���[�W�J���̎Q�����i�����邩���ׂ�
const BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
const u16	MUSICAL_SYSTEM_ChangeMusicalPokeNumber( POKEMON_PARAM *pokePara );
//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );

#endif MUSICAL_SYSTEM_H__
