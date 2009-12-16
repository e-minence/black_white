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
//  �\���̒�`
//==============================================================================
///�ϑ����o�Ǘ����[�N
typedef struct{
	GAMESYS_WORK *gsys;
	FIELDMAP_WORK *fieldWork;
	s16 wait;
	s16 wait_max;
	u16 disguise_code;
  u8 seq;
	u8 anm_no;
	u8 loop;
	u8 padding[3];
}INTRUDE_EVENT_DISGUISE_WORK;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern GMEVENT * EVENT_Disguise(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
  INTRUDE_COMM_SYS_PTR intcomm, HEAPID heap_id);

//--------------------------------------------------------------
//  �ϑ����o�V�[�P���X
//--------------------------------------------------------------
extern void IntrudeEvent_Sub_DisguiseEffectSetup(INTRUDE_EVENT_DISGUISE_WORK *iedw, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 disguise_code);
extern BOOL IntrudeEvent_Sub_DisguiseEffectMain(INTRUDE_EVENT_DISGUISE_WORK *iedw, INTRUDE_COMM_SYS_PTR intcomm);
