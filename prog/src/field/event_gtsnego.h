//============================================================================================
/**
 * @file	  event_gtsnego.h
 * @brief	  �C�x���g�FGTS�l�S�V�G�[�V����
 * @author	ohno
 * @date	  2009.11.15
 */
//============================================================================================
#pragma once

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "net_app/gtsnego.h"

FS_EXTERN_OVERLAY(event_gtsnego);

//-----------------------------------------------------------------------------
/**
 *    �I�[�o�[���C�Ǘ��{�C�x���g�N��
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� FIELDMAP_WORK*��n���B
//=====================================
extern GMEVENT* EVENT_CallGTSNego( GAMESYS_WORK* gsys, void* work );


#if PM_DEBUG

extern GMEVENT* EVENT_GTSNegoChangeDebug(GAMESYS_WORK * gsys, void * work);
#endif
