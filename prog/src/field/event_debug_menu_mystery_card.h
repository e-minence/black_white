#ifdef PM_DEBUG 
///////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �f�o�b�O���j���[�w�ӂ����f�[�^�쐬�x
 * @file event_debug_menu_mystery_card.h
 * @author obata
 * @date   2010.05.06
 *
 * ��event_debug_menu.c ���ړ�
 */
///////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "gamesystem/gamesystem.h"

FS_EXTERN_OVERLAY( debug_mystery_card ); 

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�Z
 * @param gsys
 * @param wk
 * @return GMEVENT*
 */
//--------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_DebugMenu_MakeMysteryCard( GAMESYS_WORK *gsys, void* wk );

#endif // PM_DEBUG 


