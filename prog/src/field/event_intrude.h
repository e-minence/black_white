//==============================================================================
/**
 * @file    event_intrude.h
 * @brief   �N���F�C�x���g
 * @author  matsuda
 * @date    2009.12.15(��)
 */
//==============================================================================
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/field_msgbg.h"
#include "fieldmap.h"
#include "fldmmdl.h"

#include "field_comm\intrude_types.h"


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * EVENT_Disguise(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, HEAPID heap_id);

