//======================================================================
/**
 *
 * @file  event_fieldmap_menu.c
 * @brief �t�B�[���h�}�b�v���j���[�̐���C�x���g
 * @author  kagaya
 * @date  2008.11.13
 *
 * ���W���[�����FFIELD_MENU
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"


#include "message.naix"
#include "msg/msg_fldmapmenu.h"

#include "system/wipe.h"
#include "gamesystem/game_comm.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"
#include "field/field_msgbg.h"
#include "field/event_fieldmap_menu.h"
#include "field/fieldmap.h"
#include "field/map_attr.h"
#include "field/itemuse_event.h"
#include "field/zonedata.h"
#include "field/event_field_proclink.h"
#include "field/field_skill.h"

//======================================================================
//  define
//======================================================================

//--------------------------------------------------------------
/// FMENURET
//--------------------------------------------------------------
typedef enum
{
  FMENURET_CONTINUE,
  FMENURET_FINISH,
  FMENURET_NEXTEVENT,
}FMENURET;

typedef enum
{
  FMENUSTATE_INIT,
  FMENUSTATE_MAIN,
  FMENUSTATE_DECIDE_ITEM, //���ڂ����肳�ꂽ
  FMENUSTATE_EXIT_MENU,
  FMENUSTATE_USE_SKILL,

	FMENUSTATE_CALL_PROC,
	FMENUSTATE_RETURN_PROC,

	FMENUSTATE_USE_ITEM,
	FMENUSTATE_RETURN_ITEM,
  
}FMENU_STATE;

//======================================================================
//  typedef struct
//======================================================================
/// FMENU_EVENT_WORK
typedef struct _TAG_FMENU_EVENT_WORK FMENU_EVENT_WORK;

//--------------------------------------------------------------
/// FMENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_FMENU_EVENT_WORK
{
  HEAPID heapID;
  GMEVENT *gmEvent;
  GAMESYS_WORK *gmSys;
  FIELDMAP_WORK *fieldWork;
	FIELD_MENU_ITEM_TYPE	item_type;
  FMENU_STATE state;
  
  FIELD_SUBSCREEN_MODE return_subscreen_mode;

	EVENT_PROCLINK_PARAM	link;	//�v���Z�X�����N���
  
};

//======================================================================
//  proto
//======================================================================
static GMEVENT_RESULT FldMapMenuEvent( GMEVENT *event, int *seq, void *wk );
static EVENT_PROCLINK_CALL_TYPE FldMapGetProclinkToMenuItem( FIELD_MENU_ITEM_TYPE type );

//�R�[���o�b�N
static void FldMapMenu_Open_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );
static void FldMapMenu_Close_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );
//======================================================================
//  �C�x���g�F�t�B�[���h�}�b�v���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldMapMenu(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, HEAPID heapID )
{
  FMENU_EVENT_WORK *mwk;
  GMEVENT * event;
  FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
  
  event = GMEVENT_Create( gsys, NULL, FldMapMenuEvent, sizeof(FMENU_EVENT_WORK));
  
  mwk = GMEVENT_GetEventWork(event);

  GFL_STD_MemClear( mwk, sizeof(FMENU_EVENT_WORK) );
  
  mwk->gmSys = gsys;
  mwk->gmEvent = event;
  mwk->fieldWork = fieldWork;
  mwk->heapID = heapID;
  mwk->state = FMENUSTATE_INIT;

	mwk->link.gsys			= gsys;
	mwk->link.field_wk	= fieldWork;
	mwk->link.event			= event;
	mwk->link.open_func		= FldMapMenu_Open_Callback;
	mwk->link.close_func	= FldMapMenu_Close_Callback;
	mwk->link.wk_adrs		= mwk;
	mwk->link.data			= EVENT_PROCLINK_DATA_NONE;

  if(FIELD_SUBSCREEN_GetMode(subscreen) == FIELD_SUBSCREEN_INTRUDE){
    mwk->return_subscreen_mode = FIELD_SUBSCREEN_INTRUDE;
  }
  else{
    mwk->return_subscreen_mode = FIELD_SUBSCREEN_NORMAL;
  }

  return event;
}

//--------------------------------------------------------------
/**
 * ���j�I�����[���p�t�B�[���h�}�b�v���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param heapID  HEAPID
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_UnionMapMenu(
  GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, HEAPID heapID )
{
  FMENU_EVENT_WORK *mwk;
  GMEVENT * event;
  
  event = EVENT_FieldMapMenu(gsys, fieldWork, heapID);
  
  mwk = GMEVENT_GetEventWork(event);
  if(ZONEDATA_IsColosseum(FIELDMAP_GetZoneID(fieldWork)) == TRUE){
    mwk->return_subscreen_mode = FIELD_SUBSCREEN_NOGEAR;
  }
  else{
    mwk->return_subscreen_mode = FIELD_SUBSCREEN_UNION;
  }

  return event;
}


//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�t�B�[���h�}�b�v���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FldMapMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_EVENT_WORK *mwk = wk;
  switch (mwk->state) 
  {
		//-------------------------------------
		///		���j���[����
		//=====================================
  case FMENUSTATE_INIT:
    if( FIELD_SUBSCREEN_CanChange( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) ) == TRUE )
    {
      MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      MMDLSYS_PauseMoveProc( fldMdlSys );
      GAMEDATA_SetSubScreenType( gameData , FMIT_POKEMON );
      FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
      mwk->state = FMENUSTATE_MAIN;
    }
    break;
  case FMENUSTATE_MAIN:
    {
      const FIELD_SUBSCREEN_ACTION action = FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork));
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE )
      {
        //�������ڂ����肳�ꂽ
        mwk->state = FMENUSTATE_DECIDE_ITEM;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }
      else
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT )
      {
        //�L�����Z��
        mwk->state = FMENUSTATE_EXIT_MENU;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }
/*      else
      if( action == FIELD_SUBSCREEN_ACTION_DEBUG_PALACEJUMP )
      {
        //�L�����Z��+�p���X�W�����v
        mwk->state = FMENUSTATE_EXIT_MENU;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }*/
    }
    break;
  case FMENUSTATE_DECIDE_ITEM:
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      const FIELD_MENU_ITEM_TYPE type = FIELD_SUBSCREEN_GetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      GAMEDATA_SetSubScreenType( gameData , type );

			mwk->item_type	= FIELD_SUBSCREEN_GetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );

			mwk->link.call	= FldMapGetProclinkToMenuItem( type );
			mwk->state			= FMENUSTATE_CALL_PROC;
     }
    break;
    
  case FMENUSTATE_EXIT_MENU:
    {
      MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
      MMDLSYS_ClearPauseMoveProc( fldMdlSys );
			//Exit�̏ꍇ��Callback�̒��Ŗ߂����̂ōs��Ȃ�
			if( mwk->link.result != EVENT_PROCLINK_RESULT_EXIT )
			{	
				FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), mwk->return_subscreen_mode);
			}
			return( GMEVENT_RES_FINISH );
    }
    break;

		//-------------------------------------
		///		�t�B�[���h�Z�g�p
		//=====================================
  case FMENUSTATE_USE_SKILL:
    {
      FLDSKILL_CHECK_WORK skillCheckWork;
      FLDSKILL_USE_HEADER skillUseWork;
      GMEVENT *event;

			//�t�B�[���h���A
			{
				MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
				MMDLSYS_ClearPauseMoveProc( fldMdlSys );
			}

			//ITEM�̏ꍇ��Callback�̒��Ŗ߂����̂ōs��Ȃ�
			//FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), mwk->return_subscreen_mode);

			//�Z�g�p
			{	
				FLDSKILL_InitCheckWork( GAMESYSTEM_GetFieldMapWork(mwk->gmSys), &skillCheckWork ); //�ď�����
				FLDSKILL_InitUseHeader( &skillUseWork , mwk->link.select_poke, mwk->link.select_param,
            mwk->link.zoneID, mwk->link.grid_x, mwk->link.grid_y, mwk->link.grid_z );
				event = FLDSKILL_UseSkill( mwk->link.select_param, &skillUseWork , &skillCheckWork );
				GMEVENT_ChangeEvent(mwk->gmEvent , event);
			}
    }
		break;

		//-------------------------------------
		///		�o�b�O�A���j���[�ȂǃA�v���֔��
		//=====================================
	case FMENUSTATE_CALL_PROC:
		GMEVENT_CallEvent( event, EVENT_ProcLink( &mwk->link, mwk->heapID ) );
		mwk->state = FMENUSTATE_RETURN_PROC;
		break;

	case FMENUSTATE_RETURN_PROC:
		if( mwk->link.result == EVENT_PROCLINK_RESULT_RETURN )
		{
			mwk->state	= FMENUSTATE_MAIN;
		}
		else if( mwk->link.result == EVENT_PROCLINK_RESULT_EXIT )
		{	
			mwk->state	= FMENUSTATE_EXIT_MENU;
		}
		else if( mwk->link.result == EVENT_PROCLINK_RESULT_ITEM )
		{	
			mwk->state	= FMENUSTATE_USE_ITEM;
		}
		else if( mwk->link.result == EVENT_PROCLINK_RESULT_SKILL )
		{	
			mwk->state	= FMENUSTATE_USE_SKILL;
		}
		break;

		//-------------------------------------
		///		�A�C�e���g�p
		//=====================================
	case FMENUSTATE_USE_ITEM:
		{	
			GMEVENT *p_item_event;
			//�t�B�[���h���A
			{
				MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
				MMDLSYS_ClearPauseMoveProc( fldMdlSys );
			}

			//ITEM�̏ꍇ��Callback�̒��Ŗ߂����̂ōs��Ȃ�
			//FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), mwk->return_subscreen_mode);

			//�A�C�e���R�[��
			//GFL_OVERLAY_Load( FS_OVERLAY_ID(itemuse) );
			p_item_event	= EVENT_FieldItemUse( mwk->link.select_param, mwk->gmSys, mwk->fieldWork );
			GMEVENT_CallEvent(event, p_item_event );
			mwk->state = FMENUSTATE_RETURN_ITEM;
		}
		break;

	case FMENUSTATE_RETURN_ITEM:
		//GFL_OVERLAY_Unload( FS_OVERLAY_ID(itemuse) );
		return (GMEVENT_RES_FINISH);
 	}

  return( GMEVENT_RES_CONTINUE );

}
//----------------------------------------------------------------------------
/**
 *	@brief	FIELD_MENU_ITEM_TYPE����EVENT_PROCLINK_CALL_TYPE�ւ̕ϊ�
 *
 *	@param	FIELD_MENU_ITEM_TYPE type		���j���[�A�C�e��
 *
 *	@return	�v���b�N�����N
 */
//-----------------------------------------------------------------------------
static EVENT_PROCLINK_CALL_TYPE FldMapGetProclinkToMenuItem( FIELD_MENU_ITEM_TYPE type )
{	
	static const int proclink_to_menu_item_tbl[	FMIT_MAX ]	=
	{	
		-1,//  FMIT_EXIT,
		EVENT_PROCLINK_CALL_POKELIST,//  FMIT_POKEMON,
		EVENT_PROCLINK_CALL_ZUKAN,//  FMIT_ZUKAN,
		EVENT_PROCLINK_CALL_BAG,//  FMIT_ITEMMENU,
		EVENT_PROCLINK_CALL_TRAINERCARD,//  FMIT_TRAINERCARD,
		EVENT_PROCLINK_CALL_REPORT,//  FMIT_REPORT,
		EVENT_PROCLINK_CALL_CONFIG,//  FMIT_CONFING,
		-1,//  FMIT_NONE,
	};

  GF_ASSERT( type < FMIT_MAX );
	GF_ASSERT( proclink_to_menu_item_tbl[ type ] != -1 );
	return proclink_to_menu_item_tbl[ type ];
}

//=============================================================================
/**
 *		CALLBACK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�����N�ɓn���R�[���o�b�N
 *
 *	@param	const EVENT_PROCLINK_PARAM *param	����
 *	@param	*wk_adrs													�����̃��[�N
 */
//-----------------------------------------------------------------------------
static void FldMapMenu_Open_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs )
{	
	FMENU_EVENT_WORK *mwk	= wk_adrs;

	FIELD_SUBSCREEN_WORK *subscreen = FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork);

	//�t�B�[���h�ɖ߂�ꍇ
	if( param->result == EVENT_PROCLINK_RESULT_EXIT ||
			param->result == EVENT_PROCLINK_RESULT_ITEM ||
			param->result == EVENT_PROCLINK_RESULT_SKILL )
	{	
		if( FIELD_SUBSCREEN_GetMode(subscreen) != mwk->return_subscreen_mode )
		{	
			FIELD_SUBSCREEN_ChangeForce(subscreen, mwk->return_subscreen_mode );
		}
	}
	else
	{	
		//���j���[�ɖ߂�ꍇ
		//fieldmap.c�I�����ɃT�u�X�N���[�����[�h��gamedata�ɕۑ����A
		//���������ɂ킽���Ă���̂ŁA�����ōēx���j���[������K�v�͂Ȃ��B
		//���Ă��܂��ƁA
		// fieldmap.c�̃T�u�X�N���[�����[�h�ǂݒ�����init->exit->init�����Ă��܂��A
		// PRINTQUE�o�b�t�@�����s����O�ɏI���Ă��܂�
		//FIELD_SUBSCREEN_ChangeForce(subscreen, FIELD_SUBSCREEN_TOPMENU);
	
		FIELD_SUBSCREEN_SetTopMenuItemNo( subscreen, mwk->item_type );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�����N�ɓn���R�[���o�b�N
 *
 *	@param	const EVENT_PROCLINK_PARAM *param	����
 *	@param	*wk_adrs													�����̃��[�N
 */
//-----------------------------------------------------------------------------
static void FldMapMenu_Close_Callback( const EVENT_PROCLINK_PARAM *param, void *wk_adrs )
{	
	FMENU_EVENT_WORK *mwk	= wk_adrs;
	//FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) , mwk->item_type );
}
