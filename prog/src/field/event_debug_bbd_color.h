#ifdef PM_DEBUG 
///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �f�o�b�O���j���[�w�r���{�[�h�F�x
 * @file event_debug_menu_bbd_color.h
 * @author obata
 * @date   2010.05.06
 *
 * ��event_debug_menu.c ���ړ�
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h"

FS_EXTERN_OVERLAY( debug_bbd_color ); 

//--------------------------------------------------------------
/**
 * �C�x���g�F�r���{�[�h�F
 * @param gsys
 * @param wk
 * @return GMEVENT*
 */
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_DebugMenu_BBD_Color( GAMESYS_WORK *gsys, void* wk );

#endif // PM_DEBUG 
