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
#include "field_comm/field_comm_event.h"   //FIELD_COMM_EVENT_�`
#include "field_comm/intrude_field.h"
#include "event_debug_menu.h"     //DEBUG_EVENT_DebugMenu
#include "event_battle.h"         //EVENT_Battle
#include "event_fieldtalk.h"      //EVENT_FieldTalk
#include "event_fieldmap_menu.h"  //EVENT_FieldMapMenu
#include "rail_editor.h"
#include "script.h"     //SCRIPT_SetEventScript
#include "net_app/union/union_event_check.h"
#include "event_comm_talk.h"      //EVENT_CommTalk
#include "event_comm_talked.h"      //EVENT_CommWasTalkedTo
#include "event_rail_slipdown.h"    //EVENT_RailSlipDown
#include "event_intrude_subscreen.h"      //EVENT_ChangeIntrudeSubScreen
#include "event_shortcut_menu.h"	//EVENT_ShortCutMenu

#include "system/main.h"    //HEAPID_FIELDMAP
#include "isdbglib.h"

#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount

#include "fieldmap_ctrl.h"
#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid_work.h"
#include "field_player_grid.h"

#include "map_attr.h"

#include "event_trainer_eye.h"
#include "fieldmap/zone_id.h"
#include "net_app/union/union_main.h"
#include "event_comm_error.h"

#include "../../../resource/fldmapdata/script/bg_attr_def.h" //SCRID_BG_MSG_�`
#include "../../../resource/fldmapdata/script/hiden_def.h"

#include "field_gimmick.h"   //for FLDGMK_GimmickCodeCheck
#include "field_gimmick_def.h"  //for FLD_GIMMICK_GYM_�`
#include "gym_elec.h"     //for GYM_ELEC_CreateMoveEvt
#include "gym_fly.h"     //for GYM_FLY_CreateShotEvt

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "waza_tool/wazano_def.h"
#include "field/field_comm/intrude_main.h"

#include "savedata/encount_sv.h"
#include "../../../resource/fldmapdata/script/eggevent_scr_def.h"   // for SCRID_EGG_BIRTH
#include "../../../resource/fldmapdata/script/pokecen_scr_def.h" // for SCRID_PC
#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLE_EV_SPRAY_EFFECT_END

//======================================================================
//======================================================================

extern u16 EVENTDATA_CheckPosEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork, const VecFx32 *pos );

extern u16 EVENTDATA_CheckTalkBGEvent(
    const EVENTDATA_SYSTEM *evdata, EVENTWORK *evwork,
    const VecFx32 *pos, u16 talk_dir );

//======================================================================
//======================================================================
#define FRONT_TALKOBJ_HEIGHT_DIFF (FX32_CONST(16)) //�b�������\��OBJ��Y�l����

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;              ///<�q�[�vID
  GAMESYS_WORK * gsys;        ///<�Q�[���V�X�e���ւ̃|�C���^
  GAMEDATA * gamedata;        ///<�Q�[���f�[�^�ւ̃|�C���^
  EVENTDATA_SYSTEM *evdata;   ///<�Q�Ƃ��Ă���C�x���g�f�[�^
  u16 map_id;                 ///<���݂̃}�b�vID
  //���ω����Ȃ��p�����[�^�@

  //������ω�����\��������p�����[�^
  FIELD_PLAYER * field_player;
	PLAYER_MOVE_STATE player_state;
  PLAYER_MOVE_VALUE player_value;
  u16 player_dir;                   ///<���@�̌���
  int key_trg;                ///<�L�[���i�g���K�[�j
  int key_cont;               ///<�L�[���i�j
  const VecFx32 * now_pos;        ///<���݂̃v���C���[�ʒu
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
static GMEVENT * checkMoveEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork);
static GMEVENT* CheckSodateya( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckSpray( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static void updatePartyEgg( POKEPARTY* party );
static BOOL checkPartyEgg( POKEPARTY* party );

static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * checkPushExit(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkIntrudeSubScreenEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork);
static GMEVENT * checkSubScreenEvent(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkNormalEncountEvent( const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );


static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos);
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir);
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

static MMDL * getFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap );

static MMDL * getRailFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap );

static u16 checkTalkAttrEvent( EV_REQUEST *req, FIELDMAP_WORK *fieldMap);

static int checkPokeWazaGroup( GAMEDATA *gdata, u32 waza_no );



//-----------------------------------------------------------------------------
//���[���p
//-----------------------------------------------------------------------------
static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkRailPushExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkRailSlipDown(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static void rememberExitRailInfo(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const RAIL_LOCATION* loc);
static BOOL checkRailFrontMove( const FIELD_PLAYER* cp_player );


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
static GMEVENT * FIELD_EVENT_CheckNormal( GAMESYS_WORK *gsys, void *work )
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
  event = SCRIPT_SearchSceneScript( gsys, req.heapID );
  if (event) return event;


//�������g���[�i�[�����`�F�b�N�������ɓ���
  if( !(req.debugRequest) ){
    event = EVENT_CheckTrainerEye( fieldWork, FALSE );
    if( event != NULL ){
      return event;
    }
  }

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
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
      return event;
    }

   //���W�ڑ��`�F�b�N
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
    //�ėp����ړ��C�x���g�`�F�b�N�Q
    event = checkMoveEvent(&req, fieldWork);
    if ( event != NULL) {
      return event;
    }
  }


	
//�������X�e�b�v�`�F�b�N�i����ړ�or�U������j����������
  //�퓬�ڍs�`�F�b�N
  { //�m�[�}���G���J�E���g�C�x���g�N���`�F�b�N
    GMEVENT* enc_event = checkNormalEncountEvent( &req, gsys, fieldWork );
    if(enc_event != NULL){
      return enc_event;
    }
  }
  

  //�ŔC�x���g�`�F�b�N
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
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
    }
  }
  
//���������@��ԃC�x���g�`�F�b�N
  if( !(req.debugRequest) ){
    PLAYER_EVENTBIT evbit = PLAYER_EVENTBIT_NON;
     EVENTWORK *ev = GAMEDATA_GetEventWork( req.gamedata );
    
    if( EVENTWORK_CheckEventFlag(ev,SYS_FLAG_KAIRIKI) == TRUE ){
      evbit |= PLAYER_EVENTBIT_KAIRIKI;
    }
    
    event = FIELD_PLAYER_CheckMoveEvent(
        req.field_player, req.key_trg, req.key_cont, evbit );
    
    if( event != NULL ){
      return event;
    }
  }
  
//��������b�`�F�b�N

	///�ʐM�p��b����(��
#if 0 //��check�@�N���ʐM�ύX�ׁ̈A�b��Fix
  {
    FIELD_COMM_MAIN * commSys = FIELDMAP_GetCommSys(fieldWork);
    
    if(commSys != NULL){
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
  }
#else
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
    MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
    u32 talk_netid;
    
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION && intcomm != NULL){
      //�b���������Ă��Ȃ����`�F�b�N
      if(IntrudeField_CheckTalkedTo(intcomm, &talk_netid) == TRUE){
        FIELD_PLAYER_GRID_ForceStop( req.field_player );
        return EVENT_CommWasTalkedTo(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
      }
      
      //�b��������
      if( req.talkRequest ){
        if(IntrudeField_CheckTalk(intcomm, req.field_player, &talk_netid) == TRUE){
          FIELD_PLAYER_GRID_ForceStop( req.field_player );
          return EVENT_CommTalk(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
        }
      }
    }
  }
#endif

	//�t�B�[���h�b���|���`�F�b�N
  if( req.talkRequest )
  {
    { //OBJ�b���|��
      MMDL *fmmdl_talk = getFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
        FIELD_PLAYER_GRID_ForceStop( req.field_player );
#ifdef DEBUG_ONLY_FOR_kagaya
        if( MMDL_GetOBJCode(fmmdl_talk) == ROCK ){
          scr_id = 10000;
        }
#endif
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
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }
    }

    { //BG Attribute �b������
      u16 id = checkTalkAttrEvent( &req, fieldWork );
      if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
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
		if(WIPE_SYS_EndCheck()){
			event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
			if( event != NULL ){
				return event;
			}
		}
  }
  
  //�ʐM�G���[��ʌĂяo���`�F�b�N(�����j���[�N���`�F�b�N�̐^��ɔz�u���鎖�I)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
		if(WIPE_SYS_EndCheck()){
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }
  
	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
		}
	}
	
	//�N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N���`�F�b�N
	if(WIPE_SYS_EndCheck()){
    event = checkIntrudeSubScreenEvent(gsys, fieldWork);
    if(event != NULL){
      return event;
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
    INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(GAMESYSTEM_GetGameCommSysPtr(gsys));
    ev =  DEBUG_IntrudeTreeMapWarp(fieldWork, gsys, req.field_player, intcomm);
    if(ev != NULL){
      return ev;
    }
	}
	return NULL;
}


//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N ��, �C�x���g�������̏���
 * @param	gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=�C�x���g����
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNormal_Wrap( GAMESYS_WORK *gsys, void *work )
{
	GMEVENT *event;
	FIELDMAP_WORK *fieldmap_work;
	FIELD_PLACE_NAME *place_name_sys;

	// �C�x���g�N���`�F�b�N
	event = FIELD_EVENT_CheckNormal( gsys, work );

	// �C�x���g�������̏���
	if( event != NULL )
	{
		// �n���\���E�B���h�E������
		fieldmap_work   = (FIELDMAP_WORK*)GAMESYSTEM_GetFieldMapWork( gsys );
		place_name_sys  = FIELDMAP_GetPlaceNameSys( fieldmap_work );
		FIELD_PLACE_NAME_Hide( place_name_sys );
	}

	return event;
}

//==================================================================
/**
 * �C�x���g�N���`�F�b�N�F���j�I��or�R���V�A��
 *
 * @param   gsys		
 * @param   work		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * FIELD_EVENT_CheckUnion( GAMESYS_WORK *gsys, void *work )
{
  EV_REQUEST req;
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  
  setupRequest( &req, gsys, fieldWork );

  if(unisys == NULL){
    //���j�I���I��
    VecFx32 pos;
    pos.x = 72 << FX32_SHIFT;
    pos.y = 48;
    pos.z = 88 << FX32_SHIFT;
    return DEBUG_EVENT_ChangeMapPos(gsys, fieldWork, ZONE_ID_T02PC0101, &pos, 0);
  }
  
  if(UnionMain_GetFinishReq(unisys) == TRUE){
    if(UnionMain_GetFinishExe(unisys) == FALSE){
      //���j�I���I�������J�n
      GameCommSys_ExitReq(game_comm);
    }
    return NULL;
  }
  
  //�f�o�b�O�p�`�F�b�N
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //debug
  
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
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
      return event;
    }

#if 0
   //���W�ڑ��`�F�b�N
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
#endif
  }

//���������@��ԃC�x���g�`�F�b�N����������
    /* ���͂Ȃ� */

//��������b�`�F�b�N

	///�ʐM�p��b����(��
  {
    //�ʐM�C�x���g�����`�F�b�N
    event = UnionEvent_CommCheck( gsys, fieldWork );
    if(event != NULL){
      return event;
    }
    
    //�ʐM�b�������C�x���g�����`�F�b�N
    if(req.talkRequest){
      event = UnionEvent_TalkCheck( gsys, fieldWork );
      if(event != NULL){
        return event;
      }
    }
  }

  //�ʐM�G���[��ʌĂяo���`�F�b�N(�����j���[�N���`�F�b�N�̐^��ɔz�u���鎖�I)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
		if(WIPE_SYS_EndCheck()){
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }

	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_UnionMapMenu( gsys, fieldWork, req.heapID );
		}
	}

	return NULL;
}


//----------------------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�F�m�[�O���b�h�}�b�v
 *
 *	@param	gsys
 *	@param	work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNoGrid( GAMESYS_WORK *gsys, void *work )
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
  if( !(req.debugRequest) ){
/*
    event = EVENT_CheckTrainerEye( fieldWork, FALSE );
    if( event != NULL ){
      return event;
    }
//*/
  }

//����������ړ��`�F�b�N����������
  //���W�C�x���g�`�F�b�N
  if( req.moveRequest )
  {
    //*
    RAIL_LOCATION pos;
    const MMDL* player = FIELD_PLAYER_GetMMdl( req.field_player );
    u16 id;
    EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
    MMDL_GetRailLocation( player, &pos );
    
    id = EVENTDATA_CheckPosEventRailLocation( req.evdata, evwork, &pos );
    
    if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
      return event;
    }
    //*/

    //���W�ڑ��`�F�b�N
    event = checkRailExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }


	
//�������X�e�b�v�`�F�b�N�i����ړ�or�U������j����������
  //�퓬�ڍs�`�F�b�N
  {
    GMEVENT* enc_event = checkNormalEncountEvent( &req, gsys, fieldWork );
    if(enc_event != NULL){
      return enc_event;
    }
  }
  

  //�ŔC�x���g�`�F�b�N
  if( req.stepRequest ){
    //*
    u16 id;
    RAIL_LOCATION pos;
    EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
    const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );
    u16 dir = MMDL_GetDirDisp( fmmdl );
  
    // 1���O�̃��[�����P�[�V�����擾
    MMDL_GetRailFrontLocation( fmmdl, &pos );
  
    {
      //OBJ�Ŕ`�F�b�N
    }
    
    id = EVENTDATA_CheckTalkBoardEventRailLocation( req.evdata, evwork, &pos, dir );
    
    if( id != EVENTDATA_ID_NONE ){
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
    }
    //*/
  }
  
//���������@��ԃC�x���g�`�F�b�N����������
    /* ���͂Ȃ� */

//��������b�`�F�b�N

	///�ʐM�p��b����(��
#if 0 //��check�@�N���ʐM�ύX�ׁ̈A�b��Fix
  {
    FIELD_COMM_MAIN * commSys = FIELDMAP_GetCommSys(fieldWork);
    
    if(commSys != NULL){
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
  }
#endif

	//�t�B�[���h�b���|���`�F�b�N
  if( req.talkRequest )
  {
    { //OBJ�b���|��
      MMDL *fmmdl_talk = getRailFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        FIELD_PLAYER_NOGRID* player_nogrid = FIELDMAP_GetPlayerNoGrid( fieldWork );
        MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
        
        FIELD_PLAYER_NOGRID_ForceStop( player_nogrid );
        return EVENT_FieldTalk( gsys, fieldWork,
          scr_id, fmmdl_player, fmmdl_talk, req.heapID );
      }
    }
    
    { //BG�b���|��
      u16 id;
      RAIL_LOCATION pos;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );
      u16 dir = MMDL_GetDirDisp( fmmdl );
      
      MMDL_GetRailFrontLocation( fmmdl, &pos );
      id = EVENTDATA_CheckTalkBGEventRailLocation( req.evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }
    }
    /*
    { //BG Attribute �b������
      u16 id = checkTalkAttrEvent( &req, fieldWork );
      if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }

    }
    //*/
  }


#if 0
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
#endif
  
//�������������ݑ���`�F�b�N�i�}�b�g�ł̃}�b�v�J�ڂȂǁj
	//�L�[���͐ڑ��`�F�b�N
  if (req.pushRequest) {
    event = checkRailPushExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }

    event = checkRailSlipDown(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
  
//���������@�ʒu�Ɋ֌W�Ȃ��L�[���̓C�x���g�`�F�b�N
  //�֗��{�^���`�F�b�N
  if( req.convRequest ){
		if(WIPE_SYS_EndCheck()){
			event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
			if( event != NULL ){
				return event;
			}
		}
  }
  
  //�ʐM�G���[��ʌĂяo���`�F�b�N(�����j���[�N���`�F�b�N�̐^��ɔz�u���鎖�I)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
		if(WIPE_SYS_EndCheck()){
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }

	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
		}
	}

	//�N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N���`�F�b�N
	if(WIPE_SYS_EndCheck()){
    event = checkIntrudeSubScreenEvent(gsys, fieldWork);
    if(event != NULL){
      return event;
    }
  }

	//�T�u�X�N���[������̃C�x���g�N���`�F�b�N
	event = checkSubScreenEvent(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
	}
  
  /*	
	//�f�o�b�O�F�p���X�Ŗ؂ɐG�ꂽ�烏�[�v
  {
    GMEVENT *ev;
    INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(GAMESYSTEM_GetGameCommSysPtr(gsys));
    ev =  DEBUG_IntrudeTreeMapWarp(fieldWork, gsys, req.field_player, intcomm);
    if(ev != NULL){
      return ev;
    }
	}
  //*/
	return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�N���`�F�b�N�F�n�C�u���b�h�}�b�v
 */
//-----------------------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckHybrid( GAMESYS_WORK *gsys, void *work )
{
	FIELDMAP_WORK *fieldWork = work;
  u32 base_system;
  
  // ���݂�Base�V�X�e�����擾 
  base_system = FIELDMAP_GetBaseSystemType( fieldWork );
  
  if( base_system == FLDMAP_BASESYS_GRID )
  {
    return FIELD_EVENT_CheckNormal( gsys, work );
  }
  return FIELD_EVENT_CheckNoGrid( gsys, work );
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

  {
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( req->field_player );
    req->now_pos = MMDL_GetVectorPosAddress( mmdl );
  }
  
  // �A�g���r���[�g�擾
  req->mapattr = FIELD_PLAYER_GetMapAttr( req->field_player );

  req->talkRequest = ((req->key_trg & PAD_BUTTON_A) != 0);

  req->menuRequest = ((req->key_trg & PAD_BUTTON_X) != 0);

	//�쐬�����Ǝ~�܂����肷��̂ŁA�ꎞ�I�ɓ��͕s�ɂ��܂�
 	req->convRequest = ((req->key_trg & PAD_BUTTON_Y) != 0);
  
  req->moveRequest = 0;

  if( FIELD_PLAYER_GetMoveValue(req->field_player) == PLAYER_MOVE_VALUE_WALK ){
    req->moveRequest = ((req->player_state == PLAYER_MOVE_STATE_END));
  }

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
  if (req->debugRequest)
  {
    req->talkRequest = FALSE;
    //req->menuRequest = FALSE;
    req->stepRequest = FALSE;
    req->moveRequest = FALSE;
    req->pushRequest = FALSE;
    req->convRequest = FALSE;
  }
    
}

//======================================================================
//
//
//    �C�x���g�N���`�F�b�N�F����ړ��֘A
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkMoveEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork)
{
  HEAPID     heap_id = FIELDMAP_GetHeapID( fieldWork );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( fieldWork ); 
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  GMEVENT*     event = NULL;

  //��ĉ��`�F�b�N
  event  = CheckSodateya( fieldWork, gsys, gdata );
  if( event != NULL) return event;

  //�������X�v���[�`�F�b�N
  event = CheckSpray( fieldWork, gsys, gdata );
  if( event != NULL) return event;

  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief �莝���^�}�S�̛z���J�E���g�X�V
 * @param party �X�V�Ώۂ̃|�P�p�[�e�B�[
 */
//--------------------------------------------------------------
static void updatePartyEgg( POKEPARTY* party )
{
  int i;
  int poke_count = PokeParty_GetPokeCount( party );

  for( i=0; i<poke_count; i++ )
  {
    POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, i );
    u32      tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    u32           friend = PP_Get( param, ID_PARA_friend, NULL ); // �Ȃ��x(�^�}�S�̏ꍇ�͛z���܂ł̎c�����)

    if( tamago_flag == TRUE ) // �^�}�S�Ȃ�
    { 
      // �c��������f�N�������g
      if( 0 < friend ) PP_Put( param, ID_PARA_friend, --friend );
    }
  }
}
//--------------------------------------------------------------
/**
 * @brief �莝���^�}�S�̛z���`�F�b�N
 * @param party �`�F�b�N�Ώۂ̃|�P�p�[�e�B�[
 * @return �z�����闑������ꍇTRUE
 *
 * ���^�}�S�̊Ԃ�, �Ȃ��x��z�������J�E���^�Ƃ��ė��p����.
 */
//--------------------------------------------------------------
static BOOL checkPartyEgg( POKEPARTY* party )
{
  int i;
  int poke_count = PokeParty_GetPokeCount( party );

  for( i=0; i<poke_count; i++ )
  {
    POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, i );
    u32      tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    u32           friend = PP_Get( param, ID_PARA_friend, NULL ); 

    if( (tamago_flag == TRUE) && (friend == 0) ) 
    { // �J�E���^0�̃^�}�S�𔭌�
      return TRUE;
    }
  } 
  return FALSE;
}

/*
 *  @brief  ��ĉ��C�x���g�`�F�b�N
 */
static GMEVENT* CheckSodateya( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata )
{
  HEAPID       heap_id = FIELDMAP_GetHeapID( fieldWork );
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gdata );
  SODATEYA*    sodateya = FIELDMAP_GetSodateya( fieldWork );

  // ��ĉ�: �o���l���Z, �q��蔻��Ȃ�1�����̏���
  SODATEYA_BreedPokemon( sodateya ); 

  // �莝���^�}�S: �z���J�E���^�X�V
  updatePartyEgg( party );

  // �莝���^�}�S: �z���`�F�b�N
  if( checkPartyEgg( party ) )
  {
    return SCRIPT_SetEventScript( gsys, SCRID_EGG_BIRTH, NULL, heap_id );
  }
  return NULL;
}

//==============================================================================
/**
 * 1�����Ƃ̃X�v���[���ʌ��Z
 */
//==============================================================================
static GMEVENT* CheckSpray( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata )
{
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork(gdata);
	u8 end_f;

	//�Z�[�u�f�[�^�擾
	end_f = EncDataSave_DecSprayCnt(EncDataSave_GetSaveDataPtr(save));
  //���Z��������0�ɂȂ�������ʐ؂ꃁ�b�Z�[�W
	if( end_f ){
		OS_Printf("�X�v���[���ʐ؂�\n");
     return SCRIPT_SetEventScript( gsys, SCRID_FLD_EV_SPRAY_EFFECT_END, NULL, FIELDMAP_GetHeapID( fieldWork ));
	}
	return NULL;
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
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT,

		railCont = PAD_BUTTON_R | PAD_BUTTON_B | PAD_BUTTON_A,
	};

	//�\�t�g���Z�b�g�`�F�b�N
	if( (req->key_cont & resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (req->key_cont & chgCont) == chgCont ){
    ISDPrintSetBlockingMode(1);
    GFL_HEAP_DEBUG_PrintSystemInfo();
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_FIELDMAP );
    ISDPrintSetBlockingMode(0);
		//return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
  if( req->menuRequest && req->debugRequest )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				req->heapID, ZONEDATA_GetMapRscID(req->map_id));
	}

#if defined(DEBUG_ONLY_FOR_tomoya_takahashi) | defined(DEBUG_ONLY_FOR_nakatsui)
	// ���[���G�f�B�^�N��
	if( (req->key_cont & railCont) == railCont )
	{
		return DEBUG_EVENT_RailEditor( gsys, fieldWork );
	}
#endif
	
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
    if( checkConnectExitDir( cnct, req->player_dir ) == FALSE )
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
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir)
{
  EXIT_DIR dir;

  dir = CONNECTDATA_GetExitDir( cnct );
  return checkDirection( player_dir, dir );
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
#if 1
  {
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &front_pos) );
    //MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &now_pos) );
    if (!(flag & MAPATTR_FLAGBIT_HITCH))
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
    //�ǂ̏ꍇ�A�d�C�W�����𒲂ׂ�
    //�M�~�b�N�����蓖�Ă��Ă��邩�𒲂ׂāA�W���\�[�X�̃I�[�o���C���Ȃ���Ă��邱�Ƃ��m�F����
    if ( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_GYM_ELEC) ){  
      //�d�C�W��
      return GYM_ELEC_CreateMoveEvt(gsys);
    }else if( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_GYM_FLY) ){
      //��s�W��
      return GYM_FLY_CreateShotEvt(gsys);
    }
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
  RAIL_LOCATION pos;
  int * firstID = FIELDMAP_GetFirstConnectID(fieldWork);
  FLDNOGRID_MAPPER* nogridMapper = FIELDMAP_GetFldNoGridMapper( fieldWork );
  FIELD_PLAYER_NOGRID* p_nogrid_player = FIELDMAP_GetPlayerNoGrid( fieldWork );
  const CONNECT_DATA* cnct;

  // @TODO �RD���W�̃C�x���g�ƃ��[�����W�̃C�x���g�𕹗p���Ă��邽�ߕ��G
  {
    VecFx32 pos_3d;
    FIELD_PLAYER_NOGRID_GetPos( p_nogrid_player , &pos_3d );
    idx = EVENTDATA_SearchConnectIDBySphere(req->evdata, &pos_3d);
  }
  if( idx == EXIT_ID_NONE )
  {
    FIELD_PLAYER_NOGRID_GetLocation( p_nogrid_player , &pos );
    idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &pos);
  }

  if (*firstID == idx) return NULL;
  if (idx == EXIT_ID_NONE){
    *firstID = idx;
    return NULL;
  }

  // ����������ŋN������̂��`�F�b�N
  cnct = EVENTDATA_GetConnectByID( req->evdata, idx );
  if( CONNECTDATA_GetExitType( cnct ) != EXIT_TYPE_NONE )
  {
    return NULL;
  }
  
  rememberExitRailInfo( req, fieldWork, idx, &pos );
  return getChangeMapEvent(req, fieldWork, idx);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h�}�b�v�@���[���V�X�e����ł̉������ݏo���������
 */
//-----------------------------------------------------------------------------
static GMEVENT * checkRailPushExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  int idx;
  RAIL_LOCATION pos, front_pos;
  BOOL result;
  const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( fieldWork );
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );
  const CONNECT_DATA* cnct;

  MMDL_GetRailLocation( cp_mmdl, &pos );
  result = MMDL_GetRailFrontLocation( cp_mmdl, &front_pos );

  // �ڂ̑O����ʕs�\�o�Ȃ��ꍇ�ɂ̓`�F�b�N���Ȃ�
  if( checkRailFrontMove( cp_player ) )
  {
    return NULL;
  }
   
  // ���̏�`�F�b�N�@���@�}�b�g
  idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &pos);

  if (idx != EXIT_ID_NONE){
    cnct = EVENTDATA_GetConnectByID( req->evdata, idx );

    if( checkConnectExitDir( cnct, req->player_dir ) )
    {
      if( CONNECTDATA_GetExitType( cnct ) == EXIT_TYPE_MAT)
      {
        // GO
        rememberExitRailInfo( req, fieldWork, idx, &pos );
        return getChangeMapEvent(req, fieldWork, idx);
      }
    }
  }

  // �ڂ̑O�`�F�b�N = �K�i�A�ǁA�h�A
  idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &front_pos);
  if(idx == EXIT_ID_NONE)
  {
    return NULL;
  }

  
  cnct = EVENTDATA_GetConnectByID( req->evdata, idx );

  if( checkConnectExitDir( cnct, req->player_dir ) )
  {
    rememberExitRailInfo( req, fieldWork, idx, &pos );  // front_pos�͈ړ��s�Ȃ̂ŁApos�ɂ���
    return getChangeMapEvent(req, fieldWork, idx);
  }

  return NULL;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N���`�F�b�N
 * @param   gsys		
 * @param   fieldWork		
 * @retval  GMEVENT *		NULL=�C�x���g�Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * checkIntrudeSubScreenEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  FIELD_SUBSCREEN_WORK * subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
  FIELD_SUBSCREEN_MODE subscreen_mode = Intrude_SUBSCREEN_Watch(game_comm, subscreen);
  GMEVENT* event = NULL;
  
  if(subscreen_mode != FIELD_SUBSCREEN_MODE_MAX){
    event = EVENT_ChangeIntrudeSubScreen(gsys, fieldWork, subscreen_mode);
  }
  return event;
}

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
  case FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP:
    event = EVENT_IntrudeTownWarp(gsys, fieldWork);
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
 * �t�B�[���h�m�[�}���G���J�E���g�C�x���g�N���`�F�b�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * checkNormalEncountEvent( const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  FIELD_ENCOUNT * encount = FIELDMAP_GetEncount(fieldWork);
  ENCOUNT_TYPE enc_type;
  
#ifdef PM_DEBUG
  if( req->debugRequest ){
    return NULL;
  }
#endif
  if( !(req->stepRequest) ){
    return NULL;
  }
#ifndef PM_DEBUG
  if(GFL_NET_IsInit()){
    return NULL;
  }
#endif
  enc_type = (ZONEDATA_IsBingo( req->map_id ) == TRUE) ? ENC_TYPE_BINGO : ENC_TYPE_NORMAL;
  return (GMEVENT*)FIELD_ENCOUNT_CheckEncount(encount, enc_type);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[�����藎���`�F�b�N
 */
//-----------------------------------------------------------------------------
static GMEVENT * checkRailSlipDown(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( fieldWork );

  // �ڂ̑O����ʕs�\�o�Ȃ��ꍇ�ɂ̓`�F�b�N���Ȃ�
  if( checkRailFrontMove( cp_player ) )
  {
    return NULL;
  }

  // ���藎������
  if( RAIL_ATTR_VALUE_CheckSlipDown( MAPATTR_GetAttrValue(req->mapattr) ) )
  {
    // �v���C���[�͉����݂Ă��邩�H
    if( req->player_dir == DIR_DOWN )
    {
      // ���藎���J�n
      return EVENT_RailSlipDown( gsys, fieldWork );
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�̏o���������ۑ�
 */
//-----------------------------------------------------------------------------
static void rememberExitRailInfo(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const RAIL_LOCATION* loc)
{
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  LOCATION ent_loc;
  LOCATION_SetRail( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0,
      loc->rail_index, loc->line_grid, loc->width_grid);
  GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�ڂ̑O�Ɉړ����Ă邩�`�F�b�N
 *
 *	@retval TRUE    ���Ă���
 *	@retval FALSE   ���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL checkRailFrontMove( const FIELD_PLAYER* cp_player )
{
  RAIL_LOCATION now;
  RAIL_LOCATION old;
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );

  
  MMDL_GetRailLocation( cp_mmdl, &now );
  MMDL_GetOldRailLocation( cp_mmdl, &old );

  if( GFL_STD_MemComp( &now, &old, sizeof(RAIL_LOCATION) ) == 0 )
  {
    return TRUE;
  }
  return FALSE;
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
#if 0 //Y�{�^����֗����j���[�ɒu�����������ߕs�v�B��ŏ����܂�nagihashi
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
  GF_ASSERT( fieldWork != NULL );
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
#endif
//======================================================================
//  �g���C�x���g
//======================================================================
#include "fldeff_namipoke.h"

typedef struct
{
  int wait;
  u16 dir;
  u16 attr_kishi_flag;
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
}EVWORK_NAMINORI;

static GMEVENT_RESULT event_NaminoriStart(
    GMEVENT *event, int *seq, void *wk )
{
  EVWORK_NAMINORI *work = wk;

  FIELD_PLAYER *fld_player =
    FIELDMAP_GetFieldPlayer( work->fieldWork );
  FIELD_PLAYER_GRID *gjiki =
    FIELDMAP_GetPlayerGrid( work->fieldWork );
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
      
      {
        fx32 range = GRID_FX32; //�P�}�X��
        
        if( work->attr_kishi_flag == TRUE ){
          range <<= 1; //�Q�}�X��
        }
      
        MMDL_TOOL_AddDirVector( dir, &pos, range );
      }
      
      height = 0;
      MMDL_GetMapPosHeight( mmdl, &pos, &height );
      pos.y = height;
     
      {
        FLDEFF_CTRL *fectrl;
        fectrl = FIELDMAP_GetFldEffCtrl( work->fieldWork );
        
        task = FLDEFF_NAMIPOKE_SetMMdl(
            fectrl, dir, &pos, mmdl, NAMIPOKE_JOINT_OFF );
        
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
      u16 ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_1G_8F );
      
      if( work->attr_kishi_flag == TRUE ){
        ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
      }
      
      MMDL_SetAcmd( mmdl, ac );
    }
    (*seq)++;
    break;
  case 3:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      MMDL_EndAcmd( mmdl );
      task = FIELD_PLAYER_GRID_GetEffectTaskWork( gjiki );
      FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_ON );
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
  FIELD_PLAYER_GRID *gjiki =
    FIELDMAP_GetPlayerGrid( work->fieldWork );
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
    FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_OFF );
    FIELD_PLAYER_GRID_SetRequest( gjiki, FIELD_PLAYER_REQBIT_NORMAL );
    FIELD_PLAYER_GRID_UpdateRequest( gjiki );
    {
      u16 ac = MMDL_ChangeDirAcmdCode( work->dir, AC_JUMP_U_1G_8F );
      if( work->attr_kishi_flag == TRUE ){
        ac = MMDL_ChangeDirAcmdCode( work->dir, AC_JUMP_U_2G_16F );
      }
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
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
    u16 dir, BOOL attr_kishi_flag )
{
  GMEVENT *event;
  EVWORK_NAMINORI *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_NaminoriEnd, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  work->dir = dir;
  work->attr_kishi_flag = attr_kishi_flag;
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
    BOOL kishi;
    VecFx32 pos;
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;
    MAPATTR_VALUE attr_value;
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    FIELD_PLAYER_GRID *gjiki =
      FIELDMAP_GetPlayerGrid( fieldWork );
    u16 dir = FIELD_PLAYER_GRID_GetKeyDir( gjiki, req->key_cont );
    
    if( dir != DIR_NOT )
    {
      FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
      attr = MAPATTR_GetAttribute( mapper, &pos );
      attr_flag = MAPATTR_GetAttrFlag( attr );
      attr_value = MAPATTR_GetAttrValue( attr );
      
      kishi = FALSE;

      if( MAPATTR_VALUE_CheckShore(attr_value) == TRUE ){ //��
        MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 ); //���������
        attr = MAPATTR_GetAttribute( mapper, &pos );
        attr_flag = MAPATTR_GetAttrFlag( attr );
        kishi = TRUE;
      }
      
      if( (attr_flag&MAPATTR_FLAGBIT_HITCH) == 0 && //�i���\��
          (attr_flag&MAPATTR_FLAGBIT_WATER) == 0 ){ //���ȊO
        GMEVENT *event;
        event = eventSet_NaminoriEnd( req, gsys, fieldWork, dir, kishi );
        return( event );
      }
    }
  }
  
  return NULL;
}

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
/**
 * ���@�O���̉�bOBJ�擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
static MMDL * getFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap )
{
  MMDL *mmdl,*jiki;
  VecFx32 pos;
  s16 gx,gy,gz;
  FLDMAPPER *mapper;
  MAPATTR attr;
  MAPATTR_VALUE attr_val;

  jiki = FIELD_PLAYER_GetMMdl( req->field_player );
  
  FIELD_PLAYER_GetFrontGridPos( req->field_player, &gx, &gy, &gz );
  MMDL_TOOL_GridPosToVectorPos( gx, gy, gz, &pos );
  
  mapper = FIELDMAP_GetFieldG3Dmapper( fieldMap );
  attr = MAPATTR_GetAttribute( mapper, &pos );
  attr_val = MAPATTR_GetAttrValue( attr );
  
  if( MAPATTR_VALUE_CheckCounter(attr_val) == TRUE ){
    u16 dir = MMDL_GetDirDisp( jiki );
    MMDL_TOOL_AddDirGrid( dir, &gx, &gz, 1 );
  }
  
  mmdl = MMDLSYS_SearchGridPosEx(
      FIELDMAP_GetMMdlSys(fieldMap), gx, gz, MMDL_GetVectorPosY(jiki), FRONT_TALKOBJ_HEIGHT_DIFF, FALSE );
  
  return( mmdl );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�ł́A�ڂ̑O�̃��f�����擾
 *
 *	@param	req
 *	@param	*fieldMap 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static MMDL * getRailFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap )
{
  MMDL *mmdl;
  MMDL* player_mmdl;
  RAIL_LOCATION location;

  player_mmdl = FIELD_PLAYER_GetMMdl( req->field_player );
  if( MMDL_GetRailFrontLocation( player_mmdl, &location ) )
  {
    mmdl = MMDLSYS_SearchRailLocation(
        FIELDMAP_GetMMdlSys(fieldMap), &location, FALSE );
  }
  else
  {
    mmdl = NULL;
  }
  
  return( mmdl );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  BOOL (*attr_check_func)(const MAPATTR_VALUE );
  u16 dir_code;
  u16 script_id;
}MAPATTR_EVENTDATA;

//--------------------------------------------------------------
/**
 * @brief BG�A�g���r���[�g�b�������`�F�b�N
 */
//--------------------------------------------------------------
static u16 checkTalkAttrEvent( EV_REQUEST *req, FIELDMAP_WORK *fieldMap)
{
  static const MAPATTR_EVENTDATA check_attr_data[] = {
    { MAPATTR_VALUE_CheckPC,          0,  SCRID_PC },
    { MAPATTR_VALUE_CheckMap,         0,  EVENTDATA_ID_NONE },
    { MAPATTR_VALUE_CheckTV,          0,  EVENTDATA_ID_NONE },
    { MAPATTR_VALUE_CheckBookShelf1,  0,  SCRID_BG_MSG_BOOK1_01 },
    { MAPATTR_VALUE_CheckBookShelf2,  0,  SCRID_BG_MSG_BOOK2_01 },
    { MAPATTR_VALUE_CheckBookShelf3,  0,  SCRID_BG_MSG_BOOKRACK1_01 },
    { MAPATTR_VALUE_CheckBookShelf4,  0,  SCRID_BG_MSG_BOOKRACK2_01 },
    { MAPATTR_VALUE_CheckVase,        0,  SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckDustBox,     0,  SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckShopShelf1,  0,  SCRID_BG_MSG_SHOPRACK1_01 },
    { MAPATTR_VALUE_CheckShopShelf2,  0,  SCRID_BG_MSG_SHOPRACK2_01 },
    { MAPATTR_VALUE_CheckShopShelf3,  0,  SCRID_BG_MSG_SHOPRACK2_01 },
    { MAPATTR_VALUE_CheckWaterFall,   0,  SCRID_HIDEN_TAKINOBORI },
  };
  int i;

  VecFx32 pos;
  s16 gx,gy,gz;
  FLDMAPPER *mapper;
  MAPATTR attr;
  MAPATTR_VALUE attr_val;
  
  FIELD_PLAYER_GetFrontGridPos( req->field_player, &gx, &gy, &gz );
  MMDL_TOOL_GridPosToVectorPos( gx, gy, gz, &pos );
  
  mapper = FIELDMAP_GetFieldG3Dmapper( fieldMap );
  attr = MAPATTR_GetAttribute( mapper, &pos );
  attr_val = MAPATTR_GetAttrValue( attr );

  for (i = 0; i < NELEMS(check_attr_data); i++)
  {
    if ( check_attr_data[i].attr_check_func(attr_val) )
    {
      //�{���͂���ɕ������`�F�b�N
      return check_attr_data[i].script_id;
    }
  }
  
  //�g���A�g���r���[�g�b���|���`�F�b�N

  if( FIELD_PLAYER_GetMoveForm(req->field_player) != PLAYER_MOVE_FORM_SWIM )
  {
    MAPATTR_FLAG attr_flag = MAPATTR_GetAttrFlag( attr );
    
    if( MAPATTR_VALUE_CheckShore(attr_val) == TRUE ||
        (MAPATTR_GetHitchFlag(attr) == FALSE &&
         (attr_flag&MAPATTR_FLAGBIT_WATER) ) )
    {
      //�{���̓o�b�W�`�F�b�N������B
      if( checkPokeWazaGroup(req->gamedata,WAZANO_NAMINORI) != 6 ){
        return SCRID_HIDEN_NAMINORI;
      }
    }
  }
  
  return EVENTDATA_ID_NONE;
}

//--------------------------------------------------------------
/**
 * �莝���|�P�����Z�`�F�b�N
 * @param gdata GAMEDATA
 * @param waza_no �`�F�b�N����Z�i���o�[
 * @retval int 6=�Z����
 */
//--------------------------------------------------------------
static int checkPokeWazaGroup( GAMEDATA *gdata, u32 waza_no )
{
  int i,max;
  POKEMON_PARAM *pp;
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gdata );
  
  max = PokeParty_GetPokeCount( party );
  
  for( i = 0; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    //���܂��`�F�b�N
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //�Z���X�g����`�F�b�N
    if( PP_Get(pp,ID_PARA_waza1,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza2,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza3,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza4,NULL) == waza_no ){
      return i;
    }
  }

#ifdef DEBUG_ONLY_FOR_kagaya  //test
  return 0;
#endif
   
  return 6;
}
