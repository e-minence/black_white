//======================================================================
/**
 * @brief  �f�o�b�O���j���[�u�n�C�����N�v
 * @file   event_debug_menu_make_egg.h
 * @author tamada GAMEFREAK inc.
 * @date   2010.05.29
 */
//======================================================================
#pragma once

#ifdef  PM_DEBUG
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h" 

FS_EXTERN_OVERLAY( debug_menu_intrude );

//--------------------------------------------------------------
/**
 * @brief �n�C�����N�C�x���g����
 *
 * @parma gameSystem
 * @param wk FIELDMAP_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_FLDMENU_Intrude( GAMESYS_WORK* gameSystem, void* wk );

#endif  //PM_DEBUG
