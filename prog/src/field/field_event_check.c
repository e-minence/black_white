//======================================================================
/**
 * @file  field_event_check.c
 * @brief
 * @author  tamada GAME FREAK inc.
 * @date  2009.06.09
 *
 * 2009.06.09 fieldmap.c����C�x���g�N���`�F�b�N������Ɨ�������
 */
//======================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
//#include "field/areadata.h"

#include "field_event_check.h"

#include "fieldmap.h"
#include "field/fieldmap_proc.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"
#include "eventdata_local.h"

#include "system/wipe.h"      //WIPE_SYS_EndCheck
#include "event_mapchange.h"
#include "event_ircbattle.h"      //EVENT_IrcBattle
#include "field_comm/field_comm_main.h"   //DEBUG_PalaceJamp
#include "field_comm/field_comm_event.h"   //FIELD_COMM_EVENT_�`
#include "event_debug_menu.h"     //DEBUG_EVENT_DebugMenu
#include "event_battle.h"         //EVENT_Battle
#include "event_fieldtalk.h"      //EVENT_FieldTalk
#include "event_fieldmap_menu.h"  //EVENT_FieldMapMenu
#include "script.h"     //SCRIPT_SetScript

#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount

//======================================================================
//======================================================================
//event
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@�C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g �}�b�v�ڑ��`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos �`�F�b�N������W
 * @retval GMEVENT NULL=�ڑ��Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
	const CONNECT_DATA * cnct;
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
	int idx = EVENTDATA_SearchConnectIDByPos(evdata, now_pos);
	
	if( idx == EXIT_ID_NONE ){
		return NULL;
	}

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}

	cnct = EVENTDATA_GetConnectByID(evdata, idx);
	
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
}

//--------------------------------------------------------------
/**
 * �C�x���g �L�[���̓C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckPushConnect(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 now_pos = *FIELDMAP_GetNowPos(fieldWork);
	
	switch( GFL_UI_KEY_GetCont() ) {
	case PAD_KEY_UP:		now_pos.z -= FX32_ONE * 16; break;
	case PAD_KEY_DOWN:	now_pos.z += FX32_ONE * 16; break;
	case PAD_KEY_LEFT:	now_pos.x -= FX32_ONE * 16; break;
	case PAD_KEY_RIGHT:	now_pos.x += FX32_ONE * 16; break;
	default: return NULL;
	}
	
	{	
		u32 attr = 0;
		FLDMAPPER_GRIDINFO gridInfo;
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
		
		if( FLDMAPPER_GetGridInfo(g3Dmapper,&now_pos,&gridInfo) == TRUE ){
			attr = gridInfo.gridData[0].attr;
		}

		if( attr == 0 ){
			return NULL;
		}
	}
	
	return fldmap_Event_CheckConnect(gsys, fieldWork, &now_pos);
}

//--------------------------------------------------------------
/**
 * �T�u�X�N���[������̃C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_Check_SubScreen(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT* event=NULL;
  
  switch(FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork))){
  case FIELD_SUBSCREEN_ACTION_DEBUGIRC:
		{
			GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
			GAME_COMM_NO no = GameCommSys_BootCheck(gcsp);
			if(GAME_COMM_NO_FIELD_BEACON_SEARCH == no){
				GameCommSys_ExitReq(gcsp);
			}
			if((GAME_COMM_NO_FIELD_BEACON_SEARCH == no) || (GAME_COMM_NO_NULL == no)){
				event = EVENT_IrcBattle(gsys, fieldWork, NULL, TRUE);
			}
		}
    break;
#if PM_DEBUG
  case FIELD_SUBSCREEN_ACTION_DEBUG_PALACEJUMP:
		event = DEBUG_PalaceJamp(fieldWork , gsys, FIELDMAP_GetFieldPlayer(fieldWork));
		break;
#endif
	}
	if(event)
	{
		FIELD_SUBSCREEN_GrantPermission(FIELDMAP_GetFieldSubscreenWork(fieldWork));
	}
	else
	{
		FIELD_SUBSCREEN_ResetAction(FIELDMAP_GetFieldSubscreenWork(fieldWork));
	}
  return event;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkRailExit(GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  int idx;
  VecFx32 pos;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
  EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData(gamedata);
  int * firstID = FIELDMAP_GetFirstConnectID(fieldWork);

  if (ZONEDATA_DEBUG_IsRailMap(FIELDMAP_GetZoneID(fieldWork)) == FALSE) return NULL;
  FIELD_RAIL_MAN_GetPos( FIELDMAP_GetFieldRailMan(fieldWork) , &pos );
  idx = EVENTDATA_SearchConnectIDBySphere(evdata, &pos);
  if (*firstID == idx) return NULL;
  if (idx == EXIT_ID_NONE)
  {
    *firstID = idx;
    return NULL;
  }

  {
		LOCATION loc_req;
    const CONNECT_DATA * cnct;
    cnct = EVENTDATA_GetConnectByID(evdata, idx);
	
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
  }
#if 0
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}
#endif

#if 0
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
#endif

}

extern u16 EVENTDATA_CheckPosEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

extern u16 EVENTDATA_CheckTalkBGEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir );

//--------------------------------------------------------------
/**
 * �C�x���g�@�C�x���g�N���`�F�b�N
 * @param	gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=�C�x���g����
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNormal( GAMESYS_WORK *gsys, void *work )
{
	enum{
		resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT
	};
	
  int trg,cont;
	GMEVENT *event;
	PLAYER_MOVE_STATE state;
  PLAYER_MOVE_VALUE value;
	FIELDMAP_WORK *fieldWork = work;
  BOOL isGridMap = FALSE;
  
  //�O���b�h�}�b�v���ǂ����̔���t���O�Z�b�g
  {
    const DEPEND_FUNCTIONS *func_tbl = FIELDMAP_GetDependFunctions(fieldWork);
    if( func_tbl->type == FLDMAP_CTRLTYPE_GRID )
    {
      isGridMap = TRUE;
    }
  }

  //�L�[�����擾
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();

  //���@����X�e�[�^�X�X�V
  FIELD_PLAYER_UpdateMoveStatus( FIELDMAP_GetFieldPlayer(fieldWork) );
  
  //���@�̓����Ԃ��擾
  value = FIELD_PLAYER_GetMoveValue( FIELDMAP_GetFieldPlayer(fieldWork) );
  state = FIELD_PLAYER_GetMoveState( FIELDMAP_GetFieldPlayer(fieldWork) );
  
  //�f�o�b�O�p�`�F�b�N
#ifdef  PM_DEBUG
	//�\�t�g���Z�b�g�`�F�b�N
	if( (cont&resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (cont&chgCont) == chgCont ){
		return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
  if( (trg & PAD_BUTTON_X) && (cont & PAD_BUTTON_R) )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				FIELDMAP_GetHeapID(fieldWork), ZONEDATA_GetMapRscID(FIELDMAP_GetZoneID(fieldWork)));
	}
#endif //debug
	
  //�퓬�ڍs�`�F�b�N
  if( isGridMap ){
    #ifdef PM_DEBUG
    if( (cont & PAD_BUTTON_R) == 0 ){
      if( state == PLAYER_MOVE_STATE_END ){
        if( FIELD_ENCOUNT_CheckEncount(FIELDMAP_GetEncount(fieldWork)) == TRUE ){
          return EVENT_Battle( gsys, fieldWork );
        }
      }
    }
    #else
    if( state == PLAYER_MOVE_STATE_END ){
			if(!GFL_NET_IsInit())
			{
				if( FIELD_ENCOUNT_CheckEncount(FIELDMAP_GetEncount(fieldWork)) == TRUE ){
					return EVENT_Battle( gsys, fieldWork );
				}
			}
    }
    #endif
  }
  

  if( state == PLAYER_MOVE_STATE_END )
  { //���W�ڑ��`�F�b�N
    event = fldmap_Event_CheckConnect(gsys, fieldWork, FIELDMAP_GetNowPos(fieldWork));
    if( event != NULL ){
      return event;
    }
  }

  //�ŔC�x���g�`�F�b�N
  if( isGridMap ){
    if( state == PLAYER_MOVE_STATE_END ){
      u16 id;
      VecFx32 pos;
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
      EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
      FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( FIELDMAP_GetFieldPlayer(fieldWork) );
      u16 dir = FLDMMDL_GetDirDisp( fmmdl );
    
      FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer(fieldWork), &pos );
      FLDMMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
      {
        //OBJ�Ŕ`�F�b�N
      }
      
      id = EVENTDATA_CheckTalkBoardEvent( evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){
        SCRIPT_FLDPARAM fparam;
        fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
        event = SCRIPT_SetScript(
           gsys, id, NULL, FIELDMAP_GetHeapID(fieldWork), &fparam );
      }
    }
  }
  

  //���W�C�x���g�`�F�b�N
  {
    VecFx32 pos;
    u16 id,param;
    GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
    EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
    EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
    FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer(fieldWork), &pos );
    
    id = EVENTDATA_CheckPosEvent( evdata, evwork, &pos );
    
    if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
      SCRIPT_FLDPARAM fparam;
      fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
      event = SCRIPT_SetScript(
         gsys, id, NULL, FIELDMAP_GetHeapID(fieldWork), &fparam );
      return event;
    }
  }

	
  //���[���̏ꍇ�̃}�b�v�J�ڃ`�F�b�N
  event = checkRailExit(gsys, fieldWork);
  if( event != NULL ){
    return event;
  }

	//�L�[���͐ڑ��`�F�b�N
	event = fldmap_Event_CheckPushConnect(gsys, fieldWork);
	if( event != NULL ){
		return event;
	}
	
	//���j���[�N���`�F�b�N
	if( trg == PAD_BUTTON_X ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, FIELDMAP_GetHeapID(fieldWork) );
		}
	}

	//�T�u�X�N���[������̃C�x���g�N���`�F�b�N
	event = fldmap_Event_Check_SubScreen(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
	}
  
	///�ʐM�p��b����(��
  {
    FIELD_COMM_MAIN * commSys = FIELDMAP_GetCommSys(fieldWork);
    //�b�������鑤
    if( trg == PAD_BUTTON_A ){
      if( FIELD_COMM_MAIN_CanTalk( commSys ) == TRUE ){
        return FIELD_COMM_EVENT_StartTalk(
            gsys , fieldWork , commSys );
      }
    }

    //�b���������鑤(���ňꏏ�ɘb�����Ԃ��̃`�F�b�N�����Ă��܂�
    if( FIELD_COMM_MAIN_CheckReserveTalk( commSys ) == TRUE ){
      return FIELD_COMM_EVENT_StartTalkPartner(
          gsys , fieldWork , commSys );
    }
  }
	
	//�t�B�[���h�b���|���`�F�b�N
	if(	isGridMap )
	{
		if( trg == PAD_BUTTON_A )
		{
      { //OBJ�b���|��
			  int gx,gy,gz;
			  FLDMMDL *fmmdl_talk;
			  FIELD_PLAYER_GetFrontGridPos(
            FIELDMAP_GetFieldPlayer(fieldWork), &gx, &gy, &gz );
	  		fmmdl_talk = FLDMMDLSYS_SearchGridPos(
	  				FIELDMAP_GetFldMMdlSys(fieldWork), gx, gz, FALSE );

		  	if( fmmdl_talk != NULL )
	  		{
	  			u32 scr_id = FLDMMDL_GetEventID( fmmdl_talk );
  				FLDMMDL *fmmdl_player = FIELD_PLAYER_GetFldMMdl(
  						FIELDMAP_GetFieldPlayer(fieldWork) );
          FIELD_PLAYER_GRID_ForceStop( FIELDMAP_GetFieldPlayer(fieldWork) );
  				return EVENT_FieldTalk( gsys, fieldWork,
  					scr_id, fmmdl_player, fmmdl_talk, FIELDMAP_GetHeapID(fieldWork) );
  			}
	  	}
      
      { //BG�b���|��
        u16 id;
        VecFx32 pos;
        GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
        EVENTDATA_SYSTEM *evdata = GAMEDATA_GetEventData( gdata );
        EVENTWORK *evwork = GAMEDATA_GetEventWork( gdata );
        FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( FIELDMAP_GetFieldPlayer(fieldWork) );
        u16 dir = FLDMMDL_GetDirDisp( fmmdl );
        
        FIELD_PLAYER_GetPos( FIELDMAP_GetFieldPlayer(fieldWork), &pos );
        FLDMMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
        id = EVENTDATA_CheckTalkBGEvent( evdata, evwork, &pos, dir );
        
        if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
          SCRIPT_FLDPARAM fparam;
          fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
          event = SCRIPT_SetScript(
             gsys, id, NULL, FIELDMAP_GetHeapID(fieldWork), &fparam );
          return event;
        }
      }
    }
	}
	
	//�f�o�b�O�F�p���X�Ŗ؂ɐG�ꂽ�烏�[�v
  {
    GMEVENT *ev;
    FIELD_COMM_MAIN * commSys = FIELDMAP_GetCommSys(fieldWork);
    ev =  DEBUG_PalaceTreeMapWarp(fieldWork, gsys, FIELDMAP_GetFieldPlayer(fieldWork), commSys);
    if(ev != NULL){
      return ev;
    }
	}
	return NULL;
}

//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�@����{�^�����͎��ɔ�������C�x���g�`�F�b�N
 * @param fieldWork FIELDMAP_WORK
 * @param event �����C�x���g�v���Z�X�i�[��
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval BOOL TRUE=�C�x���g�N��
 */
//--------------------------------------------------------------
static BOOL event_CheckEventDecideButton( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�@���@����I�����ɔ�������C�x���g�`�F�b�N�B
 * �ړ��I���A�U������A�ǏՓ˓���ȂǂŔ����B
 * @param fieldWork FIELDMAP_WORK
 * @param event �����C�x���g�v���Z�X�i�[��
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval BOOL TRUE=�C�x���g�N��
 */
//--------------------------------------------------------------
static BOOL event_CheckEventMoveEnd( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�@����ړ��I�����ɔ�������C�x���g�`�F�b�N
 * @param fieldWork FIELDMAP_WORK
 * @param event �����C�x���g�v���Z�X�i�[��
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval BOOL TRUE=�C�x���g�N��
 */
//--------------------------------------------------------------
static BOOL event_CheckEventOneStepMoveEnd( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�@�\���L�[���͒��ɔ�������C�x���g�`�F�b�N
  * @param fieldWork FIELDMAP_WORK
 * @param event �����C�x���g�v���Z�X�i�[��
 * @param key_trg �L�[�g���K
 * @param key_cont �L�[�R���e�B�j���[
 * @retval BOOL TRUE=�C�x���g�N��
 */
//--------------------------------------------------------------
static BOOL event_CheckEventPushKey( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

