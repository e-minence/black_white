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

#include "map_attr.h"
//======================================================================
//======================================================================

extern u16 EVENTDATA_CheckPosEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

extern u16 EVENTDATA_CheckTalkBGEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir );

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;              ///<�q�[�vID
  GAMESYS_WORK * gsys;        ///<�Q�[���V�X�e���ւ̃|�C���^
  GAMEDATA * gamedata;        ///<�Q�[���f�[�^�ւ̃|�C���^
  EVENTDATA_SYSTEM *evdata;   ///<�Q�Ƃ��Ă���C�x���g�f�[�^
  BOOL isGridMap;             ///<�O���b�h�}�b�v���ǂ����H�̃t���O
  u16 map_id;                 ///<���݂̃}�b�vID
  //���ω����Ȃ��p�����[�^�@

  //������ω�����\��������p�����[�^
  FIELD_PLAYER * field_player;
	PLAYER_MOVE_STATE player_state;
  PLAYER_MOVE_VALUE player_value;
  u16 player_dir;                   ///<���@�̌���
  int key_trg;                ///<�L�[���i�g���K�[�j
  int key_cont;               ///<�L�[���i�j
  const VecFx32 * now_pos;        ///<���݂̃}�b�v�ʒu
  u32 mapattr;                  ///<�����̃A�g���r���[�g���

  BOOL talkRequest; ///<�b�������L�[���삪���������ǂ���
  BOOL menuRequest; ///<���j���[�I�[�v�����삪���������ǂ���
  BOOL moveRequest; ///<����ړ��I���^�C�~���O���ǂ���
  BOOL stepRequest; ///<�U�����or����ړ��I���^�C�~���O���ǂ���
  BOOL pushRequest; ///<�������ݑ��삪���������ǂ���

  BOOL debugRequest;  ///<�f�o�b�O���삪���������ǂ���
}EV_REQUEST;

//======================================================================
//======================================================================
//event
static GMEVENT * fldmap_Event_CheckConnect(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * fldmap_Event_CheckPushConnect(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * fldmap_Event_Check_SubScreen(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos);
static BOOL checkDirection(u16 player_dir, u16 exit_dir);

static GMEVENT * DEBUG_checkKeyEvent(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);
//======================================================================
//
//
//    �C�x���g�N���`�F�b�N�{��
//
//
//======================================================================
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
	
  EV_REQUEST req;
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;
  
  setupRequest( &req, gsys, fieldWork );

  
  //�f�o�b�O�p�`�F�b�N
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //debug
	
//����������X�N���v�g�N���`�F�b�N�������ɓ���
    /* ���͂Ȃ� */

//�������g���[�i�[�����`�F�b�N�������ɓ���
    /* ���͂Ȃ� */

//����������ړ��`�F�b�N����������
  if( req.moveRequest )
  { //���W�ڑ��`�F�b�N
    event = fldmap_Event_CheckConnect(&req, gsys, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
  }

  //���W�C�x���g�`�F�b�N
  if( req.moveRequest )
  {
    VecFx32 pos;
    u16 id;
    EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
    FIELD_PLAYER_GetPos( req.field_player, &pos );
    
    id = EVENTDATA_CheckPosEvent( req.evdata, evwork, &pos );
    
    if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
      SCRIPT_FLDPARAM fparam;
      fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
      event = SCRIPT_SetScript( gsys, id, NULL, req.heapID, &fparam );
      return event;
    }
  }

	
  //���[���̏ꍇ�̃}�b�v�J�ڃ`�F�b�N
  if (ZONEDATA_DEBUG_IsRailMap(req.map_id) == TRUE)
  {
    event = checkRailExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
//�������X�e�b�v�`�F�b�N�i����ړ�or�U������j����������
  //�퓬�ڍs�`�F�b�N
  if( req.isGridMap ){
    FIELD_ENCOUNT * encount = FIELDMAP_GetEncount(fieldWork);
    #ifdef PM_DEBUG
    if( !(req.debugRequest) ){
      if( req.stepRequest ){
        if( FIELD_ENCOUNT_CheckEncount(encount) == TRUE ){
          return EVENT_Battle( gsys, fieldWork );
        }
      }
    }
    #else
    if( req.stepRequest ){
			if(!GFL_NET_IsInit())
			{
				if( FIELD_ENCOUNT_CheckEncount(encount) == TRUE ){
					return EVENT_Battle( gsys, fieldWork );
				}
			}
    }
    #endif
  }
  

  //�ŔC�x���g�`�F�b�N
  if( req.isGridMap ){
    if( req.stepRequest ){
      u16 id;
      VecFx32 pos;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( req.field_player );
      u16 dir = FLDMMDL_GetDirDisp( fmmdl );
    
      FIELD_PLAYER_GetPos( req.field_player, &pos );
      FLDMMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
      {
        //OBJ�Ŕ`�F�b�N
      }
      
      id = EVENTDATA_CheckTalkBoardEvent( req.evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){
        SCRIPT_FLDPARAM fparam;
        fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
        event = SCRIPT_SetScript( gsys, id, NULL, req.heapID, &fparam );
      }
    }
  }
//���������@��ԃC�x���g�`�F�b�N����������
    /* ���͂Ȃ� */

//��������b�`�F�b�N
	//�t�B�[���h�b���|���`�F�b�N
	if(	req.isGridMap )
	{
		if( req.talkRequest )
		{
      { //OBJ�b���|��
			  int gx,gy,gz;
			  FLDMMDL *fmmdl_talk;
			  FIELD_PLAYER_GetFrontGridPos( req.field_player, &gx, &gy, &gz );
	  		fmmdl_talk = FLDMMDLSYS_SearchGridPos(
	  				FIELDMAP_GetFldMMdlSys(fieldWork), gx, gz, FALSE );

		  	if( fmmdl_talk != NULL )
	  		{
	  			u32 scr_id = FLDMMDL_GetEventID( fmmdl_talk );
  				FLDMMDL *fmmdl_player = FIELD_PLAYER_GetFldMMdl( req.field_player );
          FIELD_PLAYER_GRID_ForceStop( req.field_player );
  				return EVENT_FieldTalk( gsys, fieldWork,
  					scr_id, fmmdl_player, fmmdl_talk, req.heapID );
  			}
	  	}
      
      { //BG�b���|��
        u16 id;
        VecFx32 pos;
        EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
        FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( req.field_player );
        u16 dir = FLDMMDL_GetDirDisp( fmmdl );
        
        FIELD_PLAYER_GetPos( req.field_player, &pos );
        FLDMMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
        id = EVENTDATA_CheckTalkBGEvent( req.evdata, evwork, &pos, dir );
        
        if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
          SCRIPT_FLDPARAM fparam;
          fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
          event = SCRIPT_SetScript( gsys, id, NULL, req.heapID, &fparam );
          return event;
        }
      }
    }
	}
//�������������ݑ���`�F�b�N�i�}�b�g�ł̃}�b�v�J�ڂȂǁj
	//�L�[���͐ڑ��`�F�b�N
  if (req.pushRequest) {
    event = fldmap_Event_CheckPushConnect(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
	

//���������@�ʒu�Ɋ֌W�Ȃ��L�[���̓C�x���g�`�F�b�N
	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
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
    if( req.talkRequest ){
      if( FIELD_COMM_MAIN_CanTalk( commSys ) == TRUE ){
        return FIELD_COMM_EVENT_StartTalk( gsys , fieldWork , commSys );
      }
    }

    //�b���������鑤(���ňꏏ�ɘb�����Ԃ��̃`�F�b�N�����Ă��܂�
    if( FIELD_COMM_MAIN_CheckReserveTalk( commSys ) == TRUE ){
      return FIELD_COMM_EVENT_StartTalkPartner( gsys , fieldWork , commSys );
    }
  }
	
	
	//�f�o�b�O�F�p���X�Ŗ؂ɐG�ꂽ�烏�[�v
  {
    GMEVENT *ev;
    FIELD_COMM_MAIN * commSys = FIELDMAP_GetCommSys(fieldWork);
    ev =  DEBUG_PalaceTreeMapWarp(fieldWork, gsys, req.field_player, commSys);
    if(ev != NULL){
      return ev;
    }
	}
	return NULL;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ����p�p�����[�^�̐���
 *
 *
 * ����`�F�b�N���Ƃɐ������Ă��邪���͂قƂ�ǂ̒l�͏��������Ɉ�x�����ł������̂����B
 */
//--------------------------------------------------------------
static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork)
{
  req->heapID = FIELDMAP_GetHeapID(fieldWork);
  req->gsys = gsys;
  req->gamedata = GAMESYSTEM_GetGameData(gsys);
	req->evdata = GAMEDATA_GetEventData(req->gamedata);
  req->map_id = FIELDMAP_GetZoneID(fieldWork);
  { //�O���b�h�}�b�v���ǂ����̔���t���O�Z�b�g
    const DEPEND_FUNCTIONS *func_tbl = FIELDMAP_GetDependFunctions(fieldWork);
    req->isGridMap = ( func_tbl->type == FLDMAP_CTRLTYPE_GRID );
  }

  req->key_trg = GFL_UI_KEY_GetTrg();
  req->key_cont = GFL_UI_KEY_GetCont();

  req->field_player = FIELDMAP_GetFieldPlayer( fieldWork );
  //���@����X�e�[�^�X�X�V
  FIELD_PLAYER_UpdateMoveStatus( req->field_player );
  //���@�̓����Ԃ��擾
  req->player_value = FIELD_PLAYER_GetMoveValue( req->field_player );
  req->player_state = FIELD_PLAYER_GetMoveState( req->field_player );
  {
    FLDMMDL *fmmdl = FIELD_PLAYER_GetFldMMdl( req->field_player );
    req->player_dir = FLDMMDL_GetDirDisp( fmmdl );
  }

  req->now_pos = FIELDMAP_GetNowPos( fieldWork );

  {
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    req->mapattr = MAPATTR_GetAttribute(g3Dmapper, req->now_pos);
  }
  req->talkRequest = ((req->key_trg & PAD_BUTTON_A) != 0);

  req->menuRequest = ((req->key_trg & PAD_BUTTON_X) != 0);

  //�{����value�����邪�A���͎b��
  req->moveRequest = ((req->player_state == PLAYER_MOVE_STATE_END));

  req->stepRequest = ((req->player_state == PLAYER_MOVE_STATE_END));

  if ( ( (req->player_dir == DIR_UP) && (req->key_cont & PAD_KEY_UP) )
      || ( (req->player_dir == DIR_DOWN) && (req->key_cont & PAD_KEY_DOWN) )
      || ( (req->player_dir == DIR_LEFT) && (req->key_cont & PAD_KEY_LEFT) )
      || ( (req->player_dir == DIR_RIGHT) && (req->key_cont & PAD_KEY_RIGHT) )
     )
  {
    req->pushRequest = TRUE;
  }
  else 
  {
    req->pushRequest = FALSE;
  }

  req->debugRequest = ( (req->key_cont & PAD_BUTTON_R) != 0);
    
}

//======================================================================
//	�t�B�[���h�}�b�v�@�T�u�@�C�x���g
//======================================================================
#ifdef  PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief �f�o�b�O�p�C�x���g�N���`�F�b�N�i��ɃL�[���͂ɂ��j
 */
//--------------------------------------------------------------
static GMEVENT * DEBUG_checkKeyEvent(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork)
{
	enum{
		resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT
	};

	//�\�t�g���Z�b�g�`�F�b�N
	if( (req->key_cont & resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (req->key_cont & chgCont) == chgCont ){
		return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
  if( req->menuRequest && req->debugRequest )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				req->heapID, ZONEDATA_GetMapRscID(req->map_id));
	}
  return NULL;
}
#endif

//--------------------------------------------------------------
/**
 * �C�x���g �}�b�v�ڑ��`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos �`�F�b�N������W
 * @retval GMEVENT NULL=�ڑ��Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckConnect(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
	const CONNECT_DATA * cnct;
	int idx = EVENTDATA_SearchConnectIDByPos(req->evdata, now_pos);
	
	if( idx == EXIT_ID_NONE ){
		return NULL;
	}
	cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  if (CONNECTDATA_GetExitType(cnct) != EXIT_TYPE_NONE)
  {
    EXIT_DIR dir = CONNECTDATA_GetExitDir(cnct);
    if (checkDirection(req->player_dir, dir) == FALSE)
    {
      return NULL;
    }
  }

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(
				&ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0,
				now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
	}

	
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(req->gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	}else{
		LOCATION loc_req;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos)
{
  enum { GRIDSIZE = 16 * FX32_ONE };
	*pos = *req->now_pos;
	
	switch( req->player_dir) {
	case DIR_UP:		pos->z -= GRIDSIZE; break;
	case DIR_DOWN:	pos->z += GRIDSIZE; break;
	case DIR_LEFT:	pos->x -= GRIDSIZE; break;
	case DIR_RIGHT:	pos->x += GRIDSIZE; break;
	default:
                  GF_ASSERT(0);
	}
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL checkDirection(u16 player_dir, u16 exit_dir)
{
  if (
      (player_dir == DIR_UP && exit_dir == EXIT_DIR_DOWN) ||
      (player_dir == DIR_DOWN && exit_dir == EXIT_DIR_UP) ||
      (player_dir == DIR_LEFT && exit_dir == EXIT_DIR_RIGHT) ||
      (player_dir == DIR_RIGHT && exit_dir == EXIT_DIR_LEFT))
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g �L�[���̓C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * fldmap_Event_CheckPushConnect(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 now_pos;
	
  setFrontPos(req, &now_pos);
  {
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &now_pos) );
    if (!(flag & MAPATTR_FLAGBIT_ENTER))
    {
      return NULL;
    }
  }
	
	return fldmap_Event_CheckConnect(req, gsys, fieldWork, &now_pos);
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
  FIELD_SUBSCREEN_WORK * subscreen;
  subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);

  switch(FIELD_SUBSCREEN_GetAction(subscreen)){
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
		FIELD_SUBSCREEN_GrantPermission(subscreen);
	}
	else
	{
		FIELD_SUBSCREEN_ResetAction(subscreen);
	}
  return event;
}

//--------------------------------------------------------------
/**
 * @brief �m�[�O���b�h�}�b�v�A���[���V�X�e����ł̏o��������
 */
//--------------------------------------------------------------
static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  int idx;
  VecFx32 pos;
  int * firstID = FIELDMAP_GetFirstConnectID(fieldWork);

  FIELD_RAIL_MAN_GetPos( FIELDMAP_GetFieldRailMan(fieldWork) , &pos );
  idx = EVENTDATA_SearchConnectIDBySphere(req->evdata, &pos);
  if (*firstID == idx) return NULL;
  if (idx == EXIT_ID_NONE)
  {
    *firstID = idx;
    return NULL;
  }

  {
		LOCATION loc_req;
    const CONNECT_DATA * cnct;
    cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
	
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
  }
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

