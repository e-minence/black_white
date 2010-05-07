//======================================================================
/**
 *
 * @file	event_fieldtalk.c
 * @brief	�t�B�[���h�b���|���C�x���g
 * @author	kagaya
 * @date	2009.01.22
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

#include "message.naix"

#include "script.h"
#include "event_fieldtalk.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//======================================================================
//	�C�x���g�F�t�B�[���h�b���|���C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�b���|���C�x���g�N��
 * @param	GAMESYS_WORK
 * @param	fieldWork		FIELDMAP_WORK
 * @param	scr_id			�X�N���v�gID
 * @param	fmmdl_player	���@���샂�f��
 * @param	fmmdl_talk		�b���|���Ώۓ��샂�f��
 * @param	heapID		HEAPID
 * @retval	GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldTalk(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u32 scr_id,
		MMDL *fmmdl_player, MMDL *fmmdl_talk, HEAPID heapID )
{
	GMEVENT *event;
	event = SCRIPT_SetEventScript( gsys, scr_id, fmmdl_talk, heapID );
	return( event );
}
