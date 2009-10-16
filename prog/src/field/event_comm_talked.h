//==============================================================================
/**
 * @file    event_comm_talked.h
 * @brief   �ʐM�v���C���[����b��������ꂽ�C�x���g
 * @author  matsuda
 * @date    2009.10.11(��)
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
extern GMEVENT * EVENT_CommWasTalkedTo(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, MMDL *fmmdl_player, u32 talk_net_id, HEAPID heap_id);
