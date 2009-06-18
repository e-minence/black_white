//======================================================================
/**
 *
 * @file	event_fieldtalk.h
 * @brief	フィールド話し掛けイベント
 * @author	kagaya
 * @data	2009.01.22
 *
 */
//======================================================================
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "fldmmdl.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================

//======================================================================
//	extern
//======================================================================
extern GMEVENT * EVENT_FieldTalk(
		GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, u32 scr_id,
		MMDL *fmmdl_player, MMDL *fmmdl_talk, HEAPID heapID );
