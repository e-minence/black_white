//==============================================================================
/**
 * @file    union_subproc.c
 * @brief   ���j�I���F�T�uPROC�Ăяo������
 * @author  matsuda
 * @date    2009.07.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_types.h"
#include "net_app/union/union_event_check.h"
#include "union_local.h"
#include "app/trainer_card.h"
#include "field/event_fieldmap_control.h"
#include "net_app/union/union_subproc.h"
#include "net_app/union/union_msg.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///���j�I���F�T�uPROC�Ăяo���C�x���g�p���[�N�\����
typedef struct{
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
  UNION_SYSTEM_PTR unisys;
}UNION_SUBPROC_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT UnionSubProc_GameChangeEvent(GMEVENT * event, int * seq, void * work);


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �T�uPROC�Ăяo���C�x���g����
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   unisys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
GMEVENT * UnionSubProc_Create(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork, UNION_SYSTEM_PTR unisys)
{
	GMEVENT *event;
	UNION_SUBPROC_EVENT_WORK *subev;
	
	event = GMEVENT_Create(gsys, NULL, 
	  UnionSubProc_GameChangeEvent, sizeof(UNION_SUBPROC_EVENT_WORK));

	subev = GMEVENT_GetEventWork(event);
	subev->gsys = gsys;
	subev->fieldWork = fieldWork;
	subev->unisys = unisys;

  unisys->subproc.active = TRUE;
	return event;
}

//--------------------------------------------------------------
/**
 * �T�uPROC�Ăяo���C�x���g
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT UnionSubProc_GameChangeEvent(GMEVENT * event, int * seq, void * work)
{
	UNION_SUBPROC_EVENT_WORK *subev = work;
	GAMESYS_WORK *gsys = subev->gsys;
	UNION_SUB_PROC *subproc = &subev->unisys->subproc;
  GMEVENT *child_event = NULL;
  
	switch(*seq) {
	case 0:
	  OS_TPrintf("GMEVENT �T�uPROC�Ăяo�� id = %d\n", subproc->id);
    UnionMsg_AllDel(subev->unisys);

	  switch(subproc->id){
	  case UNION_SUBPROC_ID_TRAINERCARD:
  	  child_event = EVENT_FieldSubProc(
  	    gsys, subev->fieldWork, TRCARD_OVERLAY_ID, &TrCardSysCommProcData, subproc->parent_work);
  	  break;
  	default:
  	  GF_ASSERT(0); //�s���ȃT�uPROC ID
  	  break;
  	}
    GMEVENT_CallEvent(event, child_event);

		(*seq) ++;
		break;
	
	case 1:
    subproc->id = UNION_SUBPROC_ID_NULL;
    subproc->parent_work = NULL;
    subproc->active = FALSE;
	  OS_TPrintf("GMEVENT �T�uPROC�I��\n");
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}


//==================================================================
/**
 * �T�uPROC�Ăяo���ݒ�
 *
 * @param   unisys		
 * @param   sub_proc_id		
 * @param   parent_wk		
 */
//==================================================================
void UnionSubProc_EventSet(UNION_SYSTEM_PTR unisys, UNION_SUBPROC_ID sub_proc_id, void *parent_wk)
{
  GF_ASSERT(unisys->subproc.id == UNION_SUBPROC_ID_NULL && unisys->subproc.active == FALSE);
  
  unisys->subproc.id = sub_proc_id;
  unisys->subproc.parent_work = parent_wk;
}

//==================================================================
/**
 * �T�uPROC�����s�������ׂ�
 *
 * @param   unisys		
 *
 * @retval  TRUE:���s���B�@FALSE:�������s���Ă��Ȃ�
 */
//==================================================================
BOOL UnionSubProc_IsExits(UNION_SYSTEM_PTR unisys)
{
  if(unisys->subproc.id == UNION_SUBPROC_ID_NULL && unisys->subproc.active == FALSE){
    return FALSE;
  }
  return TRUE;
}

