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
#include "script.h"     //SCRIPT_SetEventScript


#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount

#include "fieldmap_ctrl_grid.h"
#include "field_player_grid.h"

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
  BOOL convRequest; ///<�֗��{�^�����삪���������ǂ���

  BOOL debugRequest;  ///<�f�o�b�O���삪���������ǂ���
}EV_REQUEST;

//======================================================================
//======================================================================
//event
static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * checkPushExit(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkSubScreenEvent(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos);
static BOOL checkDirection(u16 player_dir, u16 exit_dir);
static int getConnectID(const EV_REQUEST * req, const VecFx32 * now_pos);
static void rememberExitInfo(EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const VecFx32 * pos);
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx);
static GMEVENT * DEBUG_checkKeyEvent(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static GMEVENT * checkEvent_ConvenienceButton( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static GMEVENT * checkEvent_PlayerNaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static GMEVENT * checkEvent_PlayerNaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

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
      fparam.fieldMap = fieldWork;
      fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID, &fparam );
      return event;
    }

   //���W�ڑ��`�F�b�N
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
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
      MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );
      u16 dir = MMDL_GetDirDisp( fmmdl );
    
      FIELD_PLAYER_GetPos( req.field_player, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
      {
        //OBJ�Ŕ`�F�b�N
      }
      
      id = EVENTDATA_CheckTalkBoardEvent( req.evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){
        SCRIPT_FLDPARAM fparam;
        fparam.fieldMap = fieldWork;
        fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID, &fparam );
      }
    }
  }
  
//���������@��ԃC�x���g�`�F�b�N����������
    /* ���͂Ȃ� */

//��������b�`�F�b�N

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

	//�t�B�[���h�b���|���`�F�b�N
	if(	req.isGridMap )
	{
		if( req.talkRequest )
		{
      { //OBJ�b���|��
			  s16 gx,gy,gz;
			  MMDL *fmmdl_talk;
			  FIELD_PLAYER_GetFrontGridPos( req.field_player, &gx, &gy, &gz );
	  		fmmdl_talk = MMDLSYS_SearchGridPos(
	  				FIELDMAP_GetMMdlSys(fieldWork), gx, gz, FALSE );

		  	if( fmmdl_talk != NULL )
	  		{
	  			u32 scr_id = MMDL_GetEventID( fmmdl_talk );
  				MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          FIELD_PLAYER_GRID_ForceStop( req.field_player );
  				return EVENT_FieldTalk( gsys, fieldWork,
  					scr_id, fmmdl_player, fmmdl_talk, req.heapID );
  			}
	  	}
      
      { //BG�b���|��
        u16 id;
        VecFx32 pos;
        EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
        MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );
        u16 dir = MMDL_GetDirDisp( fmmdl );
        
        FIELD_PLAYER_GetPos( req.field_player, &pos );
        MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
        id = EVENTDATA_CheckTalkBGEvent( req.evdata, evwork, &pos, dir );
        
        if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
          SCRIPT_FLDPARAM fparam;
          fparam.fieldMap = fieldWork;
          fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldWork);
          event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID, &fparam );
          return event;
        }
      }
    }
  }


  { //�g���e�X�g

    if( req.player_state == PLAYER_MOVE_STATE_END ||
        req.player_state == PLAYER_MOVE_STATE_OFF )
    {
#if 0
      if( req.key_trg & PAD_BUTTON_SELECT ){
        event = checkEvent_PlayerNaminoriStart( &req, gsys, fieldWork );
        if( event != NULL ){
          return event;
       }
      }
#endif         
      #ifdef PM_DEBUG
      if( !(req.debugRequest) ){
        event = checkEvent_PlayerNaminoriEnd( &req, gsys, fieldWork );
        if( event != NULL ){
         return event;
       }
      }
      #else
      event = checkEvent_PlayerNaminoriEnd( &req, gsys, fieldWork );
      if( event != NULL ){
        return event;
      }
      #endif
    }
  }
  
//�������������ݑ���`�F�b�N�i�}�b�g�ł̃}�b�v�J�ڂȂǁj
	//�L�[���͐ڑ��`�F�b�N
  if (req.pushRequest) {
    event = checkPushExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
  
//���������@�ʒu�Ɋ֌W�Ȃ��L�[���̓C�x���g�`�F�b�N
  //�֗��{�^���`�F�b�N
  if( req.convRequest ){
    event = checkEvent_ConvenienceButton( &req, gsys, fieldWork );
    
    if( event != NULL ){
      return event;
    }
  }
  
	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
		}
	}

	//�T�u�X�N���[������̃C�x���g�N���`�F�b�N
	event = checkSubScreenEvent(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
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
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req->field_player );
    req->player_dir = MMDL_GetDirDisp( fmmdl );
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

  if( req->key_trg & PAD_BUTTON_Y ){
    req->convRequest = TRUE;
  }else{
    req->convRequest = FALSE;
  }

  req->debugRequest = ( (req->key_cont & PAD_BUTTON_R) != 0);
    
}

//======================================================================
//
//
//    �C�x���g�N���`�F�b�N�F�f�o�b�O�֘A
//
//
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

//======================================================================
//
//
//    �C�x���g�N���`�F�b�N�F�}�b�v�J�ڊ֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g �}�b�v�ڑ��`�F�b�N
 *
 * @param req   �C�x���g�`�F�b�N�p���[�N
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos �`�F�b�N������W
 * @return GMEVENT  ���������C�x���g�f�[�^�ւ̃|�C���^�B NULL�̏ꍇ�A�ڑ��Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
  int idx = getConnectID(req, now_pos);
  if (idx == EXIT_ID_NONE) {
    return NULL;
  }
  {
    const CONNECT_DATA * cnct;
    cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if (CONNECTDATA_GetExitType(cnct) != EXIT_TYPE_NONE)
    {
      return NULL;
    }
  }

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  rememberExitInfo(req, fieldWork, idx, now_pos);
  return getChangeMapEvent(req, fieldWork, idx);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx)
{
	const CONNECT_DATA * cnct;
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);

  return EVENT_ChangeMapByConnect(req->gsys, fieldWork, cnct);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void rememberExitInfo(EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const VecFx32 * pos)
{
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  LOCATION ent_loc;
  LOCATION_Set( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0,
      pos->x, pos->y, pos->z);
  GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static int getConnectID(const EV_REQUEST * req, const VecFx32 * now_pos)
{
	const CONNECT_DATA * cnct;
	int idx = EVENTDATA_SearchConnectIDByPos(req->evdata, now_pos);
	
	if( idx == EXIT_ID_NONE ){
		return EXIT_ID_NONE;
	}
	cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  if (CONNECTDATA_GetExitType(cnct) != EXIT_TYPE_NONE)
  {
    EXIT_DIR dir = CONNECTDATA_GetExitDir(cnct);
    if (checkDirection(req->player_dir, dir) == FALSE)
    {
      return EXIT_ID_NONE;
    }
  }
  return idx;
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
 * �C�x���g �L�[���̓}�b�v�J�ڋN���`�F�b�N
 *
 * @param req   �C�x���g�`�F�b�N�p���[�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * checkPushExit(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 front_pos;
  int idx;
	
  //�ڂ̑O���ʍs�s�łȂ��ꍇ�A�`�F�b�N���Ȃ�
  setFrontPos(req, &front_pos);
#if 0
  {
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &front_pos) );
    //MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &now_pos) );
    if (!(flag & MAPATTR_FLAGBIT_ENTER))
    {
      return NULL;
    }
  }
#endif

  //�����`�F�b�N�i�}�b�g�j
  idx = getConnectID(req, req->now_pos);
  if (idx != EXIT_ID_NONE)
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if (CONNECTDATA_GetExitType(cnct) == EXIT_TYPE_MAT)
    {
      rememberExitInfo(req, fieldWork, idx, req->now_pos);
      return getChangeMapEvent(req, fieldWork, idx);
    }
  }
	
  //�ڂ̑O�`�F�b�N�i�K�i�A�ǁA�h�A�j
  idx = getConnectID(req, &front_pos);
  if (idx == EXIT_ID_NONE) {
    return NULL;
  }

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  rememberExitInfo(req, fieldWork, idx, &front_pos);
  return getChangeMapEvent(req, fieldWork, idx);
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

  return getChangeMapEvent(req, fieldWork, idx);
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �T�u�X�N���[������̃C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * checkSubScreenEvent(
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

//======================================================================
//  �֗��{�^���C�x���g
//======================================================================
typedef struct
{
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
}EVWORK_CONVBTN;

static GMEVENT_RESULT event_ConvenienceButton(
    GMEVENT *event, int *seq, void *wk )
{
  //�� ���]�ԃe�X�g
  EVWORK_CONVBTN *work = wk;
  FIELD_PLAYER *fld_player;
  FIELDMAP_CTRL_GRID *gridMap;
  FIELD_PLAYER_GRID *gjiki;
  PLAYER_MOVE_FORM form;
  
  gridMap = FIELDMAP_GetMapCtrlWork( work->fieldWork );
  gjiki = FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( gridMap );
  
  fld_player = FIELDMAP_GetFieldPlayer( work->fieldWork );
  form = FIELD_PLAYER_GetMoveForm( fld_player );
  
  switch( form ){
  case PLAYER_MOVE_FORM_NORMAL:
    FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_GRID_REQBIT_CYCLE );
    break;
  case PLAYER_MOVE_FORM_CYCLE:
    FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_GRID_REQBIT_NORMAL );
    break;
  default:
    GF_ASSERT( 0 ); //���s,���]�Ԃł͂Ȃ��̂ɃC�x���g�����s����Ă���
  }
  
  return GMEVENT_RES_FINISH;
} 


static GMEVENT * eventSet_ConvenienceButton( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT *event;
  EVWORK_CONVBTN *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_ConvenienceButton, sizeof(EVWORK_CONVBTN) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  return( event );
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * checkEvent_ConvenienceButton( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  { //�C�x���g�`�F�b�N
  }

  { //�C�x���g���s
    GMEVENT *event;
    event = eventSet_ConvenienceButton( req, gsys, fieldWork );
    return( event );
  }
}

//======================================================================
//  �g���C�x���g
//======================================================================
#include "fldeff_namipoke.h"

typedef struct
{
  int wait;
  u16 dir;
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
}EVWORK_NAMINORI;

static GMEVENT_RESULT event_NaminoriStart(
    GMEVENT *event, int *seq, void *wk )
{
  EVWORK_NAMINORI *work = wk;

  FIELD_PLAYER *fld_player =
    FIELDMAP_GetFieldPlayer( work->fieldWork );
  FIELDMAP_CTRL_GRID *gridMap =
    FIELDMAP_GetMapCtrlWork( work->fieldWork );
  FIELD_PLAYER_GRID *gjiki =
    FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( gridMap );
  PLAYER_MOVE_FORM form =
    FIELD_PLAYER_GetMoveForm( fld_player );
  MMDL *mmdl =
    FIELD_PLAYER_GetMMdl( fld_player );
  
  FLDEFF_TASK *task;
  
  switch( *seq )
  {
  case 0: //�g���|�P�����o��
    {
      u16 dir;
      s16 gx,gz;
      fx32 height;
      VecFx32 pos;
      dir = MMDL_GetDirDisp( mmdl );
      gx = MMDL_GetGridPosX( mmdl );
      gz = MMDL_GetGridPosZ( mmdl );
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
      MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32*2 ); //2�}�X��
  
      height = 0;
      MMDL_GetMapPosHeight( mmdl, &pos, &height );
      pos.y = height;
     
      {
        FLDEFF_CTRL *fectrl;
        fectrl = FIELDMAP_GetFldEffCtrl( work->fieldWork );
        
        task = FLDEFF_NAMIPOKE_SetMMdl(
            fectrl, dir, &pos, mmdl, FALSE );
        
        FIELD_PLAYER_GRID_SetEffectTaskWork( gjiki, task );
      }
    }
    
    (*seq)++;
    break;
  case 1:
    work->wait++;
    if( work->wait > 15 ){
      (*seq)++;
    }
    break;
  case 2:
    {
      u16 dir = MMDL_GetDirDisp( mmdl );
      u16 ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
      MMDL_SetAcmd( mmdl, ac );
    }
    (*seq)++;
    break;
  case 3:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      MMDL_EndAcmd( mmdl );
      task = FIELD_PLAYER_GRID_GetEffectTaskWork( gjiki );
      FLDEFF_NAMIPOKE_SetJointFlag( task, TRUE );
      FIELD_PLAYER_SetNaminori( gjiki );
      (*seq)++;
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

static GMEVENT * eventSet_NaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT *event;
  EVWORK_NAMINORI *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_NaminoriStart, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  return( event );
}

void FIELD_EVENT_ChangeNaminoriStart( GMEVENT *event,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  EVWORK_NAMINORI *work;
  GMEVENT_Change( event, event_NaminoriStart, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * checkEvent_PlayerNaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  { //�C�x���g�`�F�b�N
    FIELD_PLAYER *fld_player;
    PLAYER_MOVE_FORM form;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    form = FIELD_PLAYER_GetMoveForm( fld_player );
    if( form == PLAYER_MOVE_FORM_SWIM ){
      return NULL;
    }
  }
  
  { //�C�x���g���s
    GMEVENT *event;
    event = eventSet_NaminoriStart( req, gsys, fieldWork );
    return( event );
  }
}

//======================================================================
//  �g���㗤�C�x���g
//======================================================================
static GMEVENT_RESULT event_NaminoriEnd(
    GMEVENT *event, int *seq, void *wk )
{
  EVWORK_NAMINORI *work = wk;

  FIELD_PLAYER *fld_player =
    FIELDMAP_GetFieldPlayer( work->fieldWork );
  FIELDMAP_CTRL_GRID *gridMap =
    FIELDMAP_GetMapCtrlWork( work->fieldWork );
  FIELD_PLAYER_GRID *gjiki =
    FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( gridMap );
  PLAYER_MOVE_FORM form =
    FIELD_PLAYER_GetMoveForm( fld_player );
  MMDL *mmdl =
    FIELD_PLAYER_GetMMdl( fld_player );
  
  FLDEFF_TASK *task;
  
  switch( *seq )
  {
  case 0: //���͂��ꂽ�����Ɍ���
    MMDL_SetDirDisp( mmdl, work->dir );
    (*seq)++;
    break;
  case 1: //�g���|�P�����؂藣��
    task = FIELD_PLAYER_GRID_GetEffectTaskWork( gjiki );
    FLDEFF_NAMIPOKE_SetJointFlag( task, FALSE );
    FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_GRID_REQBIT_NORMAL );
    FIELD_PLAYER_GRID_UpdateRequest( gjiki );
    {
      u16 ac = MMDL_ChangeDirAcmdCode( work->dir, AC_JUMP_U_2G_16F );
      MMDL_SetAcmd( mmdl, ac );
    }
    (*seq)++;
    break;
  case 2:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      FIELD_PLAYER_SetNaminoriEnd( gjiki );
      (*seq)++;
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

static GMEVENT * eventSet_NaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 dir )
{
  GMEVENT *event;
  EVWORK_NAMINORI *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_NaminoriEnd, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  work->dir = dir;
  return( event );
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * checkEvent_PlayerNaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
    
  if( form == PLAYER_MOVE_FORM_SWIM ) //�g���
  {
    VecFx32 pos;
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    FIELDMAP_CTRL_GRID *gridMap =
      FIELDMAP_GetMapCtrlWork( fieldWork );
    FIELD_PLAYER_GRID *gjiki =
      FIELDMAP_CTRL_GRID_GetFieldPlayerGrid( gridMap );
    u16 dir = FIELD_PLAYER_GRID_GetKeyDir( gjiki, req->key_cont );
    
    if( dir != DIR_NOT )
    {
      FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
      attr = MAPATTR_GetAttribute( mapper, &pos );
      attr_flag = MAPATTR_GetAttrFlag( attr );
    
      if( (attr_flag&MAPATTR_FLAGBIT_HITCH) == 0 && //�i���\��
          (attr_flag&MAPATTR_FLAGBIT_WATER) == 0 ){ //���ȊO
        GMEVENT *event;
        event = eventSet_NaminoriEnd( req, gsys, fieldWork, dir );
        return( event );
      }
    }
  }

  return NULL;
}
