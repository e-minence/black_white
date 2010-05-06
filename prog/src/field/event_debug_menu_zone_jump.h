#ifdef PM_DEBUG 
///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �f�o�b�O���j���[�w�]�[���W�����v�x
 * @file event_debug_menu_zone_jump.h
 * @author obata
 * @date   2010.05.06
 *
 * ��event_debug_menu.c ���ړ�
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h"

FS_EXTERN_OVERLAY( debug_menu_zone_jump ); 

//--------------------------------------------------------------
/**
 * �C�x���g�F�]�[���W�����v
 * @param gsys
 * @param wk
 * @return GMEVENT*
 */
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_DebugMenu_ZoneJump( GAMESYS_WORK *gsys, void* wk );

#endif // PM_DEBUG


