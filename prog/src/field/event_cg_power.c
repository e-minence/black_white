//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_cg_power.c
 *	@brief  CGEAR�@�d���@�C�x���g
 *	@author	tomoya takahashi
 *	@date		2010.04.10
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "fieldmap.h"
#include "field_subscreen.h"

#include "event_cg_power.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	CGEAR�@�d���@ONOFF�@�Ǘ��@����
//=====================================
enum {
  SEQ_INIT,
  SEQ_MAIN,
  SEQ_EXIT,
} ;

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	CGEAR�@�d���@�C�x���g
//=====================================
typedef struct {
  FIELDMAP_WORK* p_fieldmap;
  FIELD_SUBSCREEN_WORK* p_subscreen;
} CGEAR_POWER_EVENT_WK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static GMEVENT_RESULT CGearPowerEvent( GMEVENT* p_event, int* p_seq, void* p_work );

//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR �d���C�x���g
 *
 *	@param	gsys  �Q�[���V�X�e��
 *  
 *	@return �C�x���g
 */
//-----------------------------------------------------------------------------
GMEVENT * EVENT_CGearPower( GAMESYS_WORK *gsys )
{
  GMEVENT* p_event = GMEVENT_Create( gsys, NULL, CGearPowerEvent, sizeof(CGEAR_POWER_EVENT_WK));
  CGEAR_POWER_EVENT_WK* p_wk =GMEVENT_GetEventWork(p_event);

  p_wk->p_fieldmap  = GAMESYSTEM_GetFieldMapWork( gsys );
  p_wk->p_subscreen = FIELDMAP_GetFieldSubscreenWork( p_wk->p_fieldmap );
  
  return p_event;
}





//-----------------------------------------------------------------------------
/**
 *      private
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  C�M�A�@�d���@�C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT CGearPowerEvent( GMEVENT* p_event, int* p_seq, void* p_work )
{
  CGEAR_POWER_EVENT_WK* p_wk = p_work;

  switch( *p_seq ){
  case SEQ_INIT:
    FIELD_SUBSCREEN_Change(p_wk->p_subscreen, FIELD_SUBSCREEN_CGEAR_ONOFF);
    (*p_seq) ++;
    break;

  case SEQ_MAIN:
    {
      FIELD_SUBSCREEN_ACTION action;

      action = FIELD_SUBSCREEN_GetAction( p_wk->p_subscreen );
      if( action == FIELD_SUBSCREEN_ACTION_CGEAR_POWER_EXIT ){
        FIELD_SUBSCREEN_ResetAction( p_wk->p_subscreen );
        (*p_seq) ++;
      }
    }
    break;

  case SEQ_EXIT:
	  FIELD_SUBSCREEN_Change( p_wk->p_subscreen, FIELD_SUBSCREEN_NORMAL );
		return GMEVENT_RES_FINISH;

  default:
    break;
  }

  return GMEVENT_RES_CONTINUE;
}

