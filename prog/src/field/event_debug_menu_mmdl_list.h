#ifdef PM_DEBUG 
///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �f�o�b�O���j���[�wMMDL LIST�x
 * @file event_debug_menu_mmdl_list.h
 * @author obata
 * @date   2010.05.06
 *
 * ��event_debug_menu.c ���ړ�
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h"

FS_EXTERN_OVERLAY( debug_menu_mmdl_list ); 

//--------------------------------------------------------------
/**
 * �C�x���g�FMMDL LIST
 * @param gsys
 * @param wk
 * @return GMEVENT*
 */
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_DebugMenu_MmdlList( GAMESYS_WORK *gsys, void* wk );

#endif // PM_DEBUG 
