//======================================================================
/**
 * @file	musical_debug.h
 * @brief	�~���[�W�J���f�o�b�O�p
 * @author	ariizumi
 * @data	100322
 *
 * ���W���[�����FMUSICAL_DEBUG
 */
//======================================================================

#pragma once

#include "musical/musical_system.h"
#include "musical/musical_define.h"

#if PM_DEBUG

typedef struct
{
  HEAPID heapId;
  MUSICAL_SCRIPT_WORK *scriptWork;
  BOOL enableArr;
  BOOL forceNpc;
  u16 arr[4];
  u16 npc[3];
}MUSICAL_DEBUG_MENU_WORK;

extern void MUSICAL_DEBUG_InitDebugMenu( MUSICAL_SCRIPT_WORK *work , const HEAPID heapId );
extern void MUSICAL_DEBUG_TermDebugMenu( void );
extern MUSICAL_DEBUG_MENU_WORK* MUSICAL_DEBUG_GetWork( void );

#endif