//======================================================================
/**
 * @file  field_event_check.c
 * @brief �t�B�[���h�ł̃C�x���g�N���`�F�b�N�����Q
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
#include "field_comm/intrude_field.h"
#include "event_debug_menu.h"     //DEBUG_EVENT_DebugMenu
#include "event_battle.h"         //EVENT_Battle
#include "event_gsync.h"         //EVENT_GSync
#include "event_cg_wireless.h"         //EVENT_CG_Wireless
#include "event_beacon_detail.h"         //EVENT_BeaconDetail
#include "event_gpower.h"         //EVENT_CheckGPower
#include "event_fieldtalk.h"      //EVENT_FieldTalk
#include "event_fieldmap_menu.h"  //EVENT_FieldMapMenu
#include "rail_editor.h"
#include "script.h"     //SCRIPT_SetEventScript
#include "net_app/union/union_event_check.h"
#include "event_comm_talk.h"      //EVENT_CommTalk
#include "event_comm_talked.h"      //EVENT_CommWasTalkedTo
#include "event_comm_common.h"
#include "event_comm_result.h"      //EVENT_CommWasTalkedTo
#include "event_rail_slipdown.h"    //EVENT_RailSlipDown
#include "event_intrude_subscreen.h"
#include "event_subscreen.h"      //EVENT_ChangeSubScreen
#include "event_shortcut_menu.h"	//EVENT_ShortCutMenu
#include "event_research_radar.h"  //EVENT_ResearchRadar
#include "event_seatemple.h"  //EVENT_SeaTemple�`

#include "system/main.h"    //HEAPID_FIELDMAP
#include "isdbglib.h"

#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount
#include "effect_encount.h"

#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid_work.h"

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
#include "gym_insect.h"     //for GYM_INSECT_CreatePushWallEvt
#include "field_gimmick_d06.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"
#include "../../../resource/fldmapdata/flagwork/work_define.h"  // for WK_OTHER_DISCOVER_EGG

#include "waza_tool/wazano_def.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_mission.h"

#include "savedata/encount_sv.h"
#include "../../../resource/fldmapdata/script/eggevent_scr_def.h"   // for SCRID_EGG_BIRTH
#include "../../../resource/fldmapdata/script/pasokon_def.h" // for SCRID_PC
#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLE_EV_SPRAY_EFFECT_END
#include "../../../resource/fldmapdata/script/vending_machine_def.h" // for SCRID_VENDING_MACHINE01

#include "debug/debug_flg.h" //DEBUG_FLG_�`

#include "field/field_const.h"  // for FX32_TO_GRID

#include "event_fourkings.h"  // 

#include "pleasure_boat.h"    //for PL_BOAT_

#include "event_autoway.h" // for EVENT_PlayerMoveOnAutoWay
#include "event_current.h" // for EVENT_PlayerMoveOnCurrent

#include "field_status_local.h" //FIELD_STATUS_GetReservedScript

#include "event_intrude.h"
#include "event_symbol.h"
#include "field_comm/intrude_work.h"

#include "scrcmd_intrude.h"

#include "savedata/situation.h"
#include "gamesystem/g_power.h" //GPOWER_Calc_Hatch

#include "savedata/intrude_save_field.h"  //ISC_SAVE_CheckItem
#include "event_intrude_secret_item.h"    //
#include "field_diving_data.h"  //DIVINGSPOT_Check
#include "poke_tool/natsuki.h"  //NATSUKI_CalcTsurearuki

#include "net_app/cg_help.h"  //CGHELP�Ăяo��
#include "event_fieldmap_control.h"  //CGHELP�Ăяo��

#ifdef PM_DEBUG
extern BOOL DebugBGInitEnd;    //BG�������Ď��t���O             �錾���@fieldmap.c
extern BOOL MapFadeReqFlg;    //�}�b�v�t�F�[�h���N�G�X�g�t���O  �錾���@script.c
#endif


//======================================================================
//======================================================================

//======================================================================
//======================================================================
#define FRONT_TALKOBJ_HEIGHT_DIFF (FX32_CONST(16)) //�b�������\��OBJ��Y�l����

//--------------------------------------------------------------
/**
 * @brief ����`�F�b�N�p�p�����[�^
 */
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;              ///<�q�[�vID
  GAMESYS_WORK * gsys;        ///<�Q�[���V�X�e���ւ̃|�C���^
  GAMEDATA * gamedata;        ///<�Q�[���f�[�^�ւ̃|�C���^
  EVENTDATA_SYSTEM *evdata;   ///<�Q�Ƃ��Ă���C�x���g�f�[�^
  u16 map_id;                 ///<���݂̃}�b�vID
  //���ω����Ȃ��p�����[�^�@

  //������ω�����\��������p�����[�^
  FIELDMAP_WORK * fieldWork;
  FIELD_PLAYER * field_player;
	PLAYER_MOVE_STATE player_state;
  PLAYER_MOVE_VALUE player_value;
  u16 player_dir;                   ///<���@�̌���
  u16 reserved_scr_id;        ///<�\��X�N���v�g��ID
  int key_trg;                ///<�L�[���i�g���K�[�j
  int key_cont;               ///<�L�[���i�j
  u16 key_direction;          ///<�L�[���͂̕���
  const VecFx32 * now_pos;        ///<���݂̃v���C���[�ʒu
  u32 mapattr;                  ///<�����̃A�g���r���[�g���

  u32 talkRequest:1; ///<�b�������L�[���삪���������ǂ���
  u32 menuRequest:1; ///<���j���[�I�[�v�����삪���������ǂ���
  u32 moveRequest:1; ///<����ړ��I���^�C�~���O���ǂ���
  u32 stepRequest:1; ///<�U�����or����ړ��I���^�C�~���O���ǂ���
  u32 dirPosRequest:1; ///<������POS�C�x���g�̃`�F�b�N������^�C�~���O���H
  u32 pushRequest:1; ///<�������ݑ��삪���������ǂ���
  u32 convRequest:1; ///<�֗��{�^�����삪���������ǂ���
  u32 subscreenRequest:1; ///<�T�u�X�N���[���C�x���g���N�����Ă悢�^�C�~���O���H

  u32 debugRequest:1;  ///<�f�o�b�O���삪���������ǂ���
  u32 exMoveRequest:1;  ///<�g���J�n����moveReq�����o���Ȃ�����ړ��̌��o
  u32 req_dmy:22;

}EV_REQUEST;

//======================================================================
//======================================================================
//--------------------------------------------------------------
//
//--------------------------------------------------------------
static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

//--------------------------------------------------------------
//  �ʃC�x���g�`�F�b�N
//--------------------------------------------------------------
static GMEVENT * checkMoveEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork);
static GMEVENT* CheckSodateya(
    const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckSpray( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckEffectEncount( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckGPowerEffectEnd( GAMESYS_WORK* gsys );
static void updatePartyEgg( GAMEDATA * gamedata, POKEPARTY* party );
static BOOL checkPartyEgg( POKEPARTY* party );
static void updateFriendyStepCount( GAMEDATA * gamedata, FIELDMAP_WORK * fieldmap );

static GMEVENT* CheckSeaTemple( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u16 zone_id );

static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * checkAttribute(EV_REQUEST * req, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkPushExit(EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkPushGimmick(const EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkPushIntrude(const EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkIntrudeSubScreenEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, ZONEID zone_id);
static GMEVENT * checkSubScreenEvent(
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkNormalEncountEvent( const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );


static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos);
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir);
static BOOL checkConnectIsFreeExit( const CONNECT_DATA * cnct );
static int getConnectID(const EV_REQUEST * req, const VecFx32 * now_pos);
static void rememberExitInfo(EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const VecFx32 * pos);
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const VecFx32 * pos);
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

static GMEVENT* checkSpecialEvent( EV_REQUEST * req );

static GMEVENT* checkPosEvent( EV_REQUEST* req );
static GMEVENT* checkPosEvent_core( EV_REQUEST * req, u16 dir );
static GMEVENT* checkPosEvent_OnlyDirection( EV_REQUEST * req );
static GMEVENT* checkPosEvent_prefetchDirection( EV_REQUEST * req );
static GMEVENT* checkPosEvent_sandstream( EV_REQUEST* req );

//--------------------------------------------------------------
//���[���p
//--------------------------------------------------------------
static GMEVENT * eventCheckNoGrid( GAMESYS_WORK *gsys, void *work );

static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkRailPushExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkRailSlipDown(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static void rememberExitRailInfo(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const RAIL_LOCATION* loc);
static GMEVENT * getRailChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const RAIL_LOCATION* loc);
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
 * @brief�@�C�x���g�N���`�F�b�N�F�ʏ�p
 * @param	gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=�C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * FIELD_EVENT_CheckNormal(
    GAMESYS_WORK *gsys, void *work, BOOL *eff_delete_flag, BOOL *menu_open_flag )
{
	
  EV_REQUEST req;
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;

  *eff_delete_flag = FALSE;  //�܂��̓��N�G�X�g�Ȃ�
  *menu_open_flag = FALSE;

  //���N�G�X�g�X�V
  setupRequest( &req, gsys, fieldWork );

//�������f�o�b�O�p�`�F�b�N���ŗD��Ƃ���
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //PM_DEBUG
	
//����������X�N���v�g�N���`�F�b�N�������ɓ���
  if (DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEvents) == FALSE) {
    event = checkSpecialEvent( &req );
    if ( event )
    {
      *eff_delete_flag = TRUE;  //�G�t�F�N�g�G���J�E���g�������N�G�X�g
      return event;
    }
  }

#ifdef PM_DEBUG
  GF_ASSERT_MSG(DebugBGInitEnd,"ERROR:BG NOT INITIALIZE");    //BG�������Ď��t���O
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:CALLED MAP FADE REQ");   //�}�b�v�t�F�[�h���N�G�X�g�t���O
#endif  

  //�V���D���ԊĎ�
  {
    PL_BOAT_WORK_PTR *wk_ptr = GAMEDATA_GetPlBoatWorkPtr(req.gamedata);
    event = PL_BOAT_CheckEvt(gsys, *wk_ptr);
    if (event != NULL) return event;
  }


//�������g���[�i�[�����`�F�b�N�������ɓ���
  if( !(req.debugRequest) && DEBUG_FLG_GetFlg(DEBUG_FLG_DisableTrainerEye) == FALSE ){
    u32 count = FIELD_EVENT_CountBattleMember( gsys );
    event = EVENT_CheckTrainerEye( fieldWork, count );
    if( event != NULL ){
      *eff_delete_flag = TRUE;  //�G�t�F�N�g�G���J�E���g�������N�G�X�g
      return event;
    }
  }

//����������ړ��`�F�b�N����������
  //���W�C�x���g�`�F�b�N
  if( req.moveRequest )
  {
    // POS�C�x���g�`�F�b�N
    event = checkPosEvent( &req );
    if( event != NULL ){
      *eff_delete_flag = TRUE;  //�G�t�F�N�g�G���J�E���g�������N�G�X�g
      return event;
    } 
    //���W�ڑ��`�F�b�N
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
    // �A�g���r���[�g�`�F�b�N
    event = checkAttribute( &req, fieldWork );
    if( event != NULL ) {
      return event;
    }
  }
  //�ėp����ړ��C�x���g�`�F�b�N�Q
  event = checkMoveEvent(&req, fieldWork);
  if ( event != NULL) {
    return event;
  }

//�������X�e�b�v�`�F�b�N�i����ړ�or�U������j����������
  if (req.stepRequest )
  {
    //POS�C�x���g�F�����w�肠����`�F�b�N
    event = checkPosEvent_OnlyDirection( &req );
    if (event)
    {
      return event;
    }
  }
  //POS�C�x���g�F�����w�肠��`�F�b�N���̂Q
  //if ( req->player_state != PLAYER_MOVE_STATE_END && req->player_value == PLAYER_MOVE_VALUE_TURN )
  {
    event = checkPosEvent_prefetchDirection( &req );
    if (event)
    {
      return event;
    }
  }
  
  //�퓬�ڍs�`�F�b�N
  { //�m�[�}���G���J�E���g�C�x���g�N���`�F�b�N
    GMEVENT* enc_event = checkNormalEncountEvent( &req, gsys, fieldWork );
    if(enc_event != NULL){
      return enc_event;
    }
  }


  //�ŔC�x���g�`�F�b�N
  /*
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
      return event;
    }
  }
  */
  
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
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
    MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
    u32 talk_netid;
    
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION && intcomm != NULL){
      //�V���{���}�b�v�ɂ��ď��L�҂̍X�V�C�x���g���������Ă��Ȃ����`�F�b�N
      if(IntrudeSymbol_CheckSymbolDataChange(intcomm, req.map_id) == TRUE){
        return EVENT_SymbolMapWarpEasy( gsys, DIR_NOT, GAMEDATA_GetSymbolMapID( req.gamedata ) );
      }
      //�b���������Ă��Ȃ����`�F�b�N
      if(IntrudeField_CheckTalkedTo(intcomm, &talk_netid) == TRUE){
        FIELD_PLAYER_ForceStop( req.field_player );
        return EVENT_CommCommon_Talked(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
      }
      //�~�b�V�������ʂ���M���Ă��Ȃ����`�F�b�N
      if(MISSION_CheckRecvResult(&intcomm->mission) == TRUE){
        FIELD_PLAYER_ForceStop( req.field_player );
        return EVENT_CommMissionResult(gsys, fieldWork, intcomm, fmmdl_player, req.heapID);
      }
      //�B���A�C�e���̖ړI�n���B�`�F�b�N
      if (req.stepRequest ){
        event = IntrudeField_CheckSecretItemEvent(gsys, intcomm, req.field_player);
        if(event != NULL){
          return event;
        }
      }
      //�b��������
      if( req.talkRequest ){
        if(IntrudeField_CheckTalk(intcomm, req.field_player, &talk_netid) == TRUE){
          FIELD_PLAYER_ForceStop( req.field_player );
          return EVENT_CommCommon_Talk(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
        }
      }
    }
  }

	//�t�B�[���h�b���|���`�F�b�N
  if( req.talkRequest )
  {
    // ���@����O�̍��W�����߂�
    VecFx32 pos;
    u16 dir = FIELD_PLAYER_GetDir( req.field_player );
    FIELD_PLAYER_GetPos( req.field_player, &pos );
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
    { //OBJ�b���|��
      MMDL *fmmdl_talk = getFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          FIELD_PLAYER_ForceStop( req.field_player );
          *eff_delete_flag = TRUE;  //�G�t�F�N�g�G���J�E���g�������N�G�X�g
          return EVENT_FieldTalk( gsys, fieldWork,
            scr_id, fmmdl_player, fmmdl_talk, req.heapID );
        }
      }
    }
    
    { //BG�b���|��
      u16 id;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      
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

    { //�B����A�C�e���`�F�b�N
      INTRUDE_SAVE_WORK * intsave;
      s16 gx,gy,gz;
      int result;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( req.gamedata ) );
      gx = MMDL_GetGridPosX( mmdl );
      gy = MMDL_GetGridPosY( mmdl );
      gz = MMDL_GetGridPosZ( mmdl );
      result = ISC_SAVE_CheckItem( intsave, FIELDMAP_GetZoneID( fieldWork ), gx, gy, gz );
      if ( result != ISC_SAVE_SEARCH_NONE )
      {
        return EVENT_IntrudeSecretItem( gsys, req.heapID, result );
      }
    }

    { //�_�C�r���O�`�F�b�N
      u16 dummy;
      if ( DIVINGSPOT_Check( fieldWork, &dummy ) == TRUE )
      {
        return SCRIPT_SetEventScript( gsys, SCRID_HIDEN_DIVING, NULL, req.heapID );
      }
    }

    { //BG �_�~�[�|�W�V���� �b������  �C�x���g�f�[�^�ߖ�̂��߂̓��ꏈ���@

      if( ZONEDATA_IsSeaTempleDungeon( req.map_id ) ){ // ���݊C��_�a�̂�
        EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
        u16 id = EVENTDATA_CheckDummyPosEvent( req.evdata, evwork, &pos );
        if( id != EVENTDATA_ID_NONE ){ //���W�C�x���g�N��
          event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
          return event;
        }
      }
    }

  }


  { //�g���e�X�g

    if( req.player_state == PLAYER_MOVE_STATE_END ||
        req.player_state == PLAYER_MOVE_STATE_OFF )
    {
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
    event = checkPushGimmick(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
    event = checkPushIntrude(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
  
//���������@�ʒu�Ɋ֌W�Ȃ��L�[���̓C�x���g�`�F�b�N
  //�֗��{�^���`�F�b�N
  if( req.convRequest ){
		if(WIPE_SYS_EndCheck()){
      FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
      if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // �_�E�W���O�}�V�����g���Ă���Ƃ��́A�֗��{�^�����g�p�ł��Ȃ�
      {
        // �������Ȃ�
      }
      else
      if( EVENT_ShortCutMenu_IsOpen(gsys) )
			{	
				event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
				if( event != NULL ){
					return event;
				}
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
      {
        FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
        if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // �_�E�W���O�}�V�����g���Ă���Ƃ��́A�_�E�W���O�}�V�����I�������邾��
        {
          return NULL;
        }
      }
      *menu_open_flag = TRUE;
  	  return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
		}
	}
	
	//�N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N���`�F�b�N
	if(WIPE_SYS_EndCheck()){
    event = checkIntrudeSubScreenEvent(gsys, fieldWork, req.map_id);
    if(event != NULL){
      return event;
    }
  }
	
	//�V�T�u�X�N���[������̃C�x���g�N���`�F�b�N
	if(WIPE_SYS_EndCheck()){
    event = FIELD_SUBSCREEN_EventCheck( FIELDMAP_GetFieldSubscreenWork(fieldWork), req.subscreenRequest );
    if(event != NULL){
      return event;
    }
  }
	/*
   * ���T�u�X�N���[������̃C�x���g�N���`�F�b�N
   *
   * @todo
   * �V�T�u�X�N���[������̃C�x���g�N���`�F�b�N���S�Ċ�������������\��ł�
   */
	event = checkSubScreenEvent(gsys, fieldWork);
	if( event != NULL ){
		return event;
	}

  //G�p���[���ʏI���`�F�b�N
  event = CheckGPowerEffectEnd( gsys );
  if( event != NULL ){
    return event;
  }
  
	
	//�p���X���W�C�x���g
  event =  Intrude_CheckPosEvent(fieldWork, gsys, req.field_player);
  if(event != NULL){
    return event;
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
	FIELD_PLACE_NAME *place_name_sys;
	FIELDMAP_WORK * fieldmap_work = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_ENCOUNT* encount = FIELDMAP_GetEncount(fieldmap_work); 
  BOOL eff_delete_flag,menu_open_flag;

  //�G�t�F�N�g�G���J�E���g�́@OBJ�Ƃ̐ڐG�ɂ��G�t�F�N�g�j���`�F�b�N
  EFFECT_ENC_CheckObjHit( encount );

	// �C�x���g�N���`�F�b�N
	event = FIELD_EVENT_CheckNormal( gsys, work, &eff_delete_flag, &menu_open_flag );

	// �C�x���g�������̏���
	if( event != NULL )
	{
    if (eff_delete_flag)
    {
      EFFECT_ENC_EffectDelete( encount );
    }
		// �n���\���E�B���h�E������
		place_name_sys  = FIELDMAP_GetPlaceNameSys( fieldmap_work );
		FIELD_PLACE_NAME_Hide( place_name_sys );
    
    // ���@������\���̏ꍇ�͌��ɖ߂�
    FIELD_PLAYER_CheckSpecialDrawForm(
        FIELDMAP_GetFieldPlayer(fieldmap_work), menu_open_flag );
	}

	return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�F���j�I��or�R���V�A��
 *
 * @param   gsys		
 * @param   work		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
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
  #if 1   //��check�@�R���V�A���}�b�v���`�F�b�N����ׁA���j�I������ɂ��Ă���
          //         �R���V�A�����݂ŃG���[�������m�肵����C�����s��
    if(ZONEDATA_IsUnionRoom(FIELDMAP_GetZoneID(fieldWork)) == TRUE){
      return EVENT_ChangeMapFromUnion( gsys, fieldWork );
    }
        //�f�o�b�O�p�`�F�b�N
      #ifdef  PM_DEBUG
        event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
        if (event != NULL) {
          return event;
        }
      #endif //debug
    return NULL;
  #else
    return EVENT_ChangeMapFromUnion( gsys, fieldWork );
  #endif
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
  
//����������X�N���v�g�N���`�F�b�N�������ɓ���

//����������ړ��`�F�b�N����������
  //���W�C�x���g�`�F�b�N
  if( req.moveRequest )
  {
    GMEVENT * event = checkPosEvent_core( &req, DIR_NOT );
    if ( event != NULL )
    {
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

	//�t�B�[���h�b���|���`�F�b�N
  if( req.talkRequest )
  {
    { //OBJ�b���|��
      MMDL *fmmdl_talk = getFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          FIELD_PLAYER_ForceStop( req.field_player );
          //*eff_delete_flag = TRUE;  //�G�t�F�N�g�G���J�E���g�������N�G�X�g
          return EVENT_FieldTalk( gsys, fieldWork,
            scr_id, fmmdl_player, fmmdl_talk, req.heapID );
        }
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

  //�ʐM�G���[��ʌĂяo���`�F�b�N(�����j���[�N���`�F�b�N�̐^��ɔz�u���鎖�I)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
		if(WIPE_SYS_EndCheck()){
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }

  //++++ �������牺�͎��@�����S�ɓ���t���[�ȏ�ԂłȂ���΃`�F�b�N���Ȃ� ++++
	if(UnionMain_CheckPlayerFreeMode(gsys) == FALSE){
    return NULL;
  }

	//���j���[�N���`�F�b�N
	if( req.menuRequest ){
		if(WIPE_SYS_EndCheck()){
  			return EVENT_UnionMapMenu( gsys, fieldWork, req.heapID );
		}
	}

	return NULL;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�N���`�F�b�N�F�m�[�O���b�h�}�b�v
 *
 *	@param	gsys
 *	@param	work 
 *
 *	@return
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNoGrid( GAMESYS_WORK *gsys, void *work )
{
	GMEVENT *event;
	FIELDMAP_WORK *fieldmap_work;
	FIELD_PLACE_NAME *place_name_sys;

	// �C�x���g�N���`�F�b�N
	event = eventCheckNoGrid( gsys, work );

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

//--------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h�}�b�v�A�@�C�x���g�`�F�b�N
 */
//--------------------------------------------------------------
static GMEVENT * eventCheckNoGrid( GAMESYS_WORK *gsys, void *work )
{
  EV_REQUEST req;
	GMEVENT *event;
	FIELDMAP_WORK *fieldWork = work;
  RAIL_LOCATION front_location;
  const MMDL *player_fmmdl;
  
  setupRequest( &req, gsys, fieldWork );

  player_fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );

  // �ڂ̑O�̃��P�[�V�������擾
  MMDL_GetRailFrontLocation( player_fmmdl, &front_location );

  
  //�f�o�b�O�p�`�F�b�N
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //debug

//����������X�N���v�g�N���`�F�b�N�������ɓ���
  if (DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEvents) == FALSE) {
    event = checkSpecialEvent( &req );
    if ( event )
    {
      return event;
    }
  }


#ifdef PM_DEBUG
  GF_ASSERT_MSG(DebugBGInitEnd,"ERROR:BG NOT INITIALIZE");    //BG�������Ď��t���O
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:CALLED MAP FADE REQ");   //�}�b�v�t�F�[�h���N�G�X�g�t���O
#endif  

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

    //�ėp����ړ��C�x���g�`�F�b�N�Q
    event = checkMoveEvent(&req, fieldWork);
    if ( event != NULL) {
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
  
#if 0 //WB�ŗv��Ȃ��Ȃ���
  //�ŔC�x���g�`�F�b�N
  if( req.stepRequest ){
    //*
    u16 id;
    RAIL_LOCATION pos;
    EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
    u16 dir = MMDL_GetDirDisp( player_fmmdl );
  
    // 1���O�̃��[�����P�[�V�����擾
    pos = front_location;
    {
      //OBJ�Ŕ`�F�b�N
    }
    
    id = EVENTDATA_CheckTalkBoardEventRailLocation( req.evdata, evwork, &pos, dir );
    
    if( id != EVENTDATA_ID_NONE ){
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
    }
    //*/
  }
#endif

//���������@��ԃC�x���g�`�F�b�N����������
    /* ���͂Ȃ� */

//��������b�`�F�b�N
	//�t�B�[���h�b���|���`�F�b�N
  if( req.talkRequest )
  {
    { //OBJ�b���|��
      MMDL *fmmdl_talk = getRailFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          
          FIELD_PLAYER_ForceStop( req.field_player );
          return EVENT_FieldTalk( gsys, fieldWork,
            scr_id, fmmdl_player, fmmdl_talk, req.heapID );
        }
      }
    }
    
    { //BG�b���|��
      u16 id;
      RAIL_LOCATION pos;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      u16 dir = MMDL_GetDirDisp( player_fmmdl );
      
      pos = front_location;
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
      FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
      if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // �_�E�W���O�}�V�����g���Ă���Ƃ��́A�֗��{�^�����g�p�ł��Ȃ�
      {
        // �������Ȃ�
      }
      else
			if( EVENT_ShortCutMenu_IsOpen(gsys) )
			{	
				event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
				if( event != NULL ){
					return event;
				}
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
      {
        FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
        if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // �_�E�W���O�}�V�����g���Ă���Ƃ��́A�_�E�W���O�}�V�����I�������邾��
        {
          return NULL;
        }
      }
  			return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
		}
	}

	//�N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N���`�F�b�N
	if(WIPE_SYS_EndCheck()){
    event = checkIntrudeSubScreenEvent(gsys, fieldWork, req.map_id);
    if(event != NULL){
      return event;
    }
  }
	//�V�T�u�X�N���[������̃C�x���g�N���`�F�b�N
  {
    u8 ev_ok = FALSE;
    if( req.subscreenRequest && WIPE_SYS_EndCheck() ){
      ev_ok = TRUE;
    }
    event = FIELD_SUBSCREEN_EventCheck( FIELDMAP_GetFieldSubscreenWork(fieldWork), ev_ok );
    if(event != NULL){
      return event;
    }
  }
	/*
   * ���T�u�X�N���[������̃C�x���g�N���`�F�b�N
   *
   * @todo
   * �V�T�u�X�N���[������̃C�x���g�N���`�F�b�N���S�Ċ�������������\��ł�
   */
	event = checkSubScreenEvent(gsys, fieldWork);
	if( event != NULL )
  {
		return event;
	}
  
  /*	
	//�p���X���W�C�x���g
  event =  Intrude_CheckPosEvent(fieldWork, gsys, req.field_player);
  if(event != NULL){
    return event;
  }
  //*/
	return NULL;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 *	@brief  �C�x���g�N���`�F�b�N�F�n�C�u���b�h�}�b�v
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckHybrid( GAMESYS_WORK *gsys, void *work )
{
	FIELDMAP_WORK *fieldWork = work;
  u32 base_system;
  GMEVENT * event;
  
  // ���݂�Base�V�X�e�����擾 
  base_system = FIELDMAP_GetBaseSystemType( fieldWork );
  
  if( base_system == FLDMAP_BASESYS_GRID )
  {
    event = FIELD_EVENT_CheckNormal_Wrap( gsys, work );
  }
  else
  {
    event = FIELD_EVENT_CheckNoGrid( gsys, work );
  }

  return event;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ����p�p�����[�^�̐���
 *
 *
 * @note
 * ����`�F�b�N���Ƃɐ������Ă��邪���͂قƂ�ǂ̒l�͏��������Ɉ�x�����ł������̂����B
 */
//--------------------------------------------------------------
static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork)
{
  GFL_STD_MemClear( req, sizeof(EV_REQUEST) );
  req->heapID = FIELDMAP_GetHeapID(fieldWork);
  req->gsys = gsys;
  req->gamedata = GAMESYSTEM_GetGameData(gsys);
	req->evdata = GAMEDATA_GetEventData(req->gamedata);
  req->map_id = FIELDMAP_GetZoneID(fieldWork);

  req->key_trg = GFL_UI_KEY_GetTrg();
  req->key_cont = GFL_UI_KEY_GetCont();
  req->key_direction = DIR_NOT;
  if ( req->key_cont & PAD_KEY_UP    ) {
    req->key_direction = DIR_UP;
  } else if ( req->key_cont & PAD_KEY_DOWN  ) {
    req->key_direction = DIR_DOWN;
  } else if ( req->key_cont & PAD_KEY_LEFT  ) {
    req->key_direction = DIR_LEFT;
  } else if ( req->key_cont & PAD_KEY_RIGHT ) {
    req->key_direction = DIR_RIGHT;
  }


  req->fieldWork = fieldWork;
  req->field_player = FIELDMAP_GetFieldPlayer( fieldWork );
  //���@����X�e�[�^�X�X�V
  FIELD_PLAYER_UpdateMoveStatus( req->field_player );
  //���@�̓����Ԃ��擾
  req->player_value = FIELD_PLAYER_GetMoveValue( req->field_player );
  req->player_state = FIELD_PLAYER_GetMoveState( req->field_player );
  {
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req->field_player );
    req->player_dir = MMDL_GetDirDisp( fmmdl );
    req->now_pos = MMDL_GetVectorPosAddress( fmmdl );
  }
  req->reserved_scr_id = FIELD_STATUS_GetReserveScript( GAMEDATA_GetFieldStatus(req->gamedata) );
  
  // �A�g���r���[�g�擾
  req->mapattr = FIELD_PLAYER_GetMapAttr( req->field_player );

  if (req->player_state == PLAYER_MOVE_STATE_END || req->player_state == PLAYER_MOVE_STATE_OFF )
  {
    req->talkRequest = ((req->key_trg & PAD_BUTTON_A) != 0);

    req->menuRequest = ((req->key_trg & PAD_BUTTON_X) != 0);

    //�쐬�����Ǝ~�܂����肷��̂ŁA�ꎞ�I�ɓ��͕s�ɂ��܂�
    req->convRequest = ((req->key_trg & PAD_BUTTON_Y) != 0);

    {
      //�T�u�X�N���[���C�x���g�N�����`�F�b�N
      u8 input = 0;

      input += req->talkRequest;
      input += req->menuRequest;
      input += req->convRequest;
      if( input == 0 ){
        req->subscreenRequest = TRUE;
      }
    }
  }
  
  req->moveRequest = 0;
  req->exMoveRequest = 0;

  if( req->player_value == PLAYER_MOVE_VALUE_WALK ){
    req->moveRequest = ((req->player_state == PLAYER_MOVE_STATE_END));
  }
  if(FIELD_PLAYER_CheckNaminoriEventEnd( req->field_player )){
    req->exMoveRequest = TRUE; 
  }
  req->stepRequest = ((req->player_state == PLAYER_MOVE_STATE_END));

  req->dirPosRequest = FALSE;
  if ( req->player_state != PLAYER_MOVE_STATE_END && req->player_value == PLAYER_MOVE_VALUE_TURN )
  {
    req->dirPosRequest = TRUE;
  } 
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
    
  if (req->key_cont & PAD_BUTTON_L) {
    TAMADA_Printf("KEY:%04x PSTATE:%d PVALUE:%d DIR:%d\n",
        req->key_cont & 0xff, req->player_state, req->player_value, req->player_dir );
  }
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ����X�N���v�g�N���`�F�b�N
 *
 * @param req EV_REQUEST
 *
 * @return �N������POS�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT * checkSpecialEvent( EV_REQUEST * req )
{
  GMEVENT * event;
  if (req->reserved_scr_id != SCRID_NULL)
  {
    FIELD_STATUS_SetReserveScript( GAMEDATA_GetFieldStatus(req->gamedata), SCRID_NULL );
    event = SCRIPT_SetEventScript( req->gsys, req->reserved_scr_id, NULL, req->heapID );
    return event;
  }
  event = SCRIPT_SearchSceneScript( req->gsys, req->heapID );
  if (event){
    return event;
  }
  return NULL;
}

//======================================================================
//
//
//  �C�x���g�N���`�F�b�N�FPOS�C�x���g�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief POS�C�x���g�`�F�b�N
 *
 * @param req EV_REQUEST
 *
 * @return �N������POS�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent( EV_REQUEST* req )
{
  GMEVENT* event;

  // ����POS�C�x���g�`�F�b�N
  event = checkPosEvent_sandstream( req );
  if( event != NULL ) { 
    return event;
  }

  // �ʏ�POS�C�x���g�`�F�b�N
  return checkPosEvent_core( req, DIR_NOT );
}
//--------------------------------------------------------------
/**
 * @brief POS�C�x���g�`�F�b�N
 * @param req EV_REQUEST
 * @param dir �`�F�b�N��������̎w��
 * @return �N������POS�C�x���g
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent_core( EV_REQUEST * req, u16 dir )
{
  u16 id;
  VecFx32 pos;
  MAPATTR_VALUE attrval;
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( req->gsys );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( req->gamedata ); 
  FIELD_PLAYER_GetPos( req->field_player, &pos );

  // �A�g���r���[�g���`�F�b�N
  { 
    FLDMAPPER* mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
    MAPATTR attr = MAPATTR_GetAttribute( mapper, &pos );
    attrval = MAPATTR_GetAttrValue( attr );
  }
  // �����A�g���r���[�g�ɂ���ꍇ��POS�C�x���g�𖳎�����
  if( MAPATTR_VALUE_CheckSandStream(attrval) == TRUE ){ return NULL; }

  id = EVENTDATA_CheckPosEvent( req->evdata, evwork, &pos, dir ); 
  if( id != EVENTDATA_ID_NONE ) 
  { //���W�C�x���g�N��
    GMEVENT* event = SCRIPT_SetEventScript( req->gsys, id, NULL, req->heapID );
    return event;
  } 
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief POS�C�x���g�`�F�b�N�F�����w�肠��
 * @param req EV_REQUEST
 * @return �N������POS�C�x���g
 *
 * @todo
 * �X��Ԃŋ����ړ��E�����Œ�̂܂�POS�ɓ��ݍ��ޏ󋵂ŁA�L�[��
 * �������ςȂ��ɂ��Ă���Ɣ������Ȃ��^�C�~���O����������Ă���
 * �i����͂��̂悤�Ȓn�`���Ȃ������ƂőΏ������j
 */
//--------------------------------------------------------------
static GMEVENT * checkPosEvent_OnlyDirection( EV_REQUEST * req )
{
  GMEVENT * event;
  u16 next_dir;

  //���݂̌����ɓK������C�x���g�̗L�����`�F�b�N
  event = checkPosEvent_core( req, req->player_dir );
  if (event)
  {
    return event;
  }

  next_dir = FIELD_PLAYER_GetKeyDir( req->field_player, req->key_cont );
  if ( next_dir == req->player_dir )
  {
    return NULL;
  }
  //���݂̌����ӊO�ɁA�L�[�̐�ǂ݂Ŏ��@�������������`�F�b�N
  event = checkPosEvent_core( req, next_dir );
  if ( event )
  {
    OS_Printf( " dir pos event!!\n" );
    return event;
  }
  return NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkPosEvent_prefetchDirection( EV_REQUEST * req )
{
  GMEVENT * event;
  u16 key_dir = DIR_NOT;
  int st = req->player_state;
  int val = req->player_value;
  //if ( req->player_state == PLAYER_MOVE_STATE_END ) return NULL;
  //if ( req->player_value == PLAYER_MOVE_VALUE_WALK ) return NULL;

  if ( req->debugRequest ) return NULL;

  if ( req->key_cont & PAD_KEY_UP    ) {
    key_dir = DIR_UP;
  } else if ( req->key_cont & PAD_KEY_DOWN  ) {
    key_dir = DIR_DOWN;
  } else if ( req->key_cont & PAD_KEY_LEFT  ) {
    key_dir = DIR_LEFT;
  } else if ( req->key_cont & PAD_KEY_RIGHT ) {
    key_dir = DIR_RIGHT;
  }

#if 0
  switch ( val )
  {
  case PLAYER_MOVE_VALUE_STOP:
    GF_ASSERT( st == PLAYER_MOVE_STATE_OFF );
    break;
  case PLAYER_MOVE_VALUE_WALK:
  case PLAYER_MOVE_VALUE_TURN:
    GF_ASSERT( st != PLAYER_MOVE_STATE_OFF );
    break;
  }
#endif
  if ( key_dir == req->player_dir ) return NULL;
  if ( key_dir == DIR_NOT ) return NULL;

  switch ( st )
  {
  //case PLAYER_MOVE_STATE_OFF:
  case PLAYER_MOVE_STATE_START:
  case PLAYER_MOVE_STATE_ON:
    return NULL;
  //case PLAYER_MOVE_STATE_END:
  }

#if 0
  if ( FIELD_PLAYER_GRID_GetMoveValue( fpg, key_dir ) == PLAYER_MOVE_VALUE_STOP )
  {
    return NULL;
  }
  switch ( val )
  {
  case PLAYER_MOVE_VALUE_STOP:
    if
  case PLAYER_MOVE_VALUE_WALK:
  case PLAYER_MOVE_VALUE_TURN:
  }
#endif
  event = checkPosEvent_core( req, key_dir );
#if 0
  if ( event )
  {
    OS_Printf( " prefetch dir pos event!!\n" );
  } else if ( req->key_cont & PAD_BUTTON_L ) {
    OS_Printf( " prefetch dir pos not!!\n" );
  }
#endif

  return event;
}

//--------------------------------------------------------------
/**
 * @brief �����A�g���r���[�g�`�F�b�N
 *
 * @param req EV_REQUEST
 *
 * @return �����������A�g���r���[�g�������ꍇ,
 *         �ȉ��̂ǂ��炩�̏����ɍ��v����ꍇ��POS�C�x���g��������, 
 *         ���s���ׂ��C�x���g��Ԃ�.
 *
 *         ����1.POS�C�x���g�̐^��ɂ���
 *         ����2.����2�O���b�h�ȓ���POS�C�x���g�����݂��� and
 *               (���]�Ԃɏ���Ă��� or �����Ă���)
 *
 *         ���s���ׂ��C�x���g�������ꍇ, NULL ��Ԃ�.
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent_sandstream( EV_REQUEST* req )
{
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( req->gsys );
  FIELD_PLAYER*  player   = FIELDMAP_GetFieldPlayer( fieldmap );
  u16            zone_id  = FIELDMAP_GetZoneID( fieldmap );

  const POS_EVENT_DATA* pos_event;
  VecFx32 pos;
  FIELD_PLAYER_GetPos( player, &pos );

  // �A�g���r���[�g���`�F�b�N
  {
		FLDMAPPER*    mapper  = FIELDMAP_GetFieldG3Dmapper( fieldmap );
    MAPATTR       attr    = MAPATTR_GetAttribute( mapper, &pos );
    MAPATTR_VALUE attrval = MAPATTR_GetAttrValue( attr );
    if( MAPATTR_VALUE_CheckSandStream(attrval) != TRUE ) { return NULL; } // �����A�g���r���[�g����Ȃ�
  }

  // ����POS�C�x���g���擾
  {
    EVENTWORK* evwork = GAMEDATA_GetEventWork( req->gamedata );
    pos_event = EVENTDATA_GetPosEvent_XZ( req->evdata, evwork, &pos, DIR_NOT );
    if( !pos_event ) { return NULL; } // POS�C�x���g������
    if( pos_event->pos_type != EVENTDATA_POSTYPE_GRID ) { return NULL; } // �O���b�h�ȊO��POS�C�x���g����
  }

  // �����̒��S�ɂ��邩�ǂ����`�F�b�N
  {
    const POS_EVENT_DATA_GPOS* pos_event_gpos;
    int centerGX, centerGZ;
    pos_event_gpos = (const POS_EVENT_DATA_GPOS*)pos_event->pos_buf;
    centerGX = pos_event_gpos->gx + pos_event_gpos->sx/2;
    centerGZ = pos_event_gpos->gz + pos_event_gpos->sz/2;
    // ���S�ɂ�����POS�C�x���g�N��
    if( (centerGX == FX32_TO_GRID(pos.x)) &&
        (centerGZ == FX32_TO_GRID(pos.z)) ) { 
      GMEVENT* event = SCRIPT_SetEventScript( req->gsys, pos_event->id, NULL, req->heapID );
      return event;
    }
  }

  // ��l���̏�ԃ`�F�b�N
  {
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( player );
    MMDL*            mmdl = FIELD_PLAYER_GetMMdl( player );
    u16              acmd = MMDL_GetAcmdCode( mmdl );
    // �����ȊO�Ȃ�POS�C�x���g�N��
    if( (form != PLAYER_MOVE_FORM_NORMAL) ||
        (acmd == AC_DASH_U_6F) ||
        (acmd == AC_DASH_D_6F) ||
        (acmd == AC_DASH_L_6F) ||
        (acmd == AC_DASH_R_6F) ) { 
      GMEVENT* event = SCRIPT_SetEventScript( req->gsys, pos_event->id, NULL, req->heapID );
      return event;
    }
  }

  return NULL;
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

  //�g���J�n�����̓������ړ������o
  if( req->moveRequest || req->exMoveRequest ){
  
    //�G���J�E���g�G�t�F�N�g�N���Ď�
    event = CheckEffectEncount( fieldWork, gsys, gdata );
    if( event != NULL) return event;
  }

  //�ȉ��͒ʏ����ړ��̂݌��o
  if( !req->moveRequest ){
    return NULL;
  }
  updateFriendyStepCount( req->gamedata, fieldWork );

  //�C��_�a�`�F�b�N
  event = CheckSeaTemple( gsys, fieldWork, req->map_id );
  if( event != NULL) return event;

  //��ĉ��`�F�b�N
  event = CheckSodateya( req, fieldWork, gsys, gdata );
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
 *
 * ���Ȃ��x(�^�}�S�̏ꍇ�͛z���܂ł̎c�����)
 */
//--------------------------------------------------------------
static void updatePartyEgg( GAMEDATA * gamedata, POKEPARTY* party )
{
  enum {
    ONE_STEP_COUNT = 0x00100, ///<���������̑���
    MAX_STEP_COUNT = 0x10000, ///<�J�E���g�ő�l
  };
  SAVE_CONTROL_WORK* saveData;
  SITUATION* situation;
  u32 count;

  // �󋵃f�[�^�擾
  saveData  = GAMEDATA_GetSaveControlWork( gamedata );
  situation = SaveData_GetSituation( saveData );

  // �^�}�S�z���J�E���^���X�V
  Situation_GetEggStepCount( situation, &count );
  count += GPOWER_Calc_Hatch( ONE_STEP_COUNT );

  // �莝���̃^�}�S�ɔ��f
  if( MAX_STEP_COUNT < count )
  {
    int i;
    int partyCount;

    partyCount = PokeParty_GetPokeCount( party );

    // �莝���^�}�S�̎c����������炷
    for( i=0; i<partyCount; i++ )
    {
      POKEMON_PARAM* param;
      u32 tamagoFlag, friend;

      param      = PokeParty_GetMemberPointer( party, i );
      tamagoFlag = PP_Get( param, ID_PARA_tamago_flag, NULL );
      friend     = PP_Get( param, ID_PARA_friend, NULL ); 

      if( tamagoFlag == TRUE ) 
      { 
        if( 0 < friend ){ PP_Put( param, ID_PARA_friend, friend -1 ); }
      }
    }

    // �z���J�E���^������
    count = 0;
  }

  // �Z�[�u�f�[�^�ɔ��f
  Situation_SetEggStepCount( situation, count );

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
  int pokeCount = PokeParty_GetPokeCount( party );

  for( i=0; i<pokeCount; i++ )
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

//--------------------------------------------------------------
/*
 *  @brief  ��ĉ��C�x���g�`�F�b�N
 */
//--------------------------------------------------------------
static GMEVENT* CheckSodateya( 
    const EV_REQUEST * req, FIELDMAP_WORK* fieldmap, GAMESYS_WORK* gameSystem, GAMEDATA* gameData )
{
  HEAPID     heapID   = FIELDMAP_GetHeapID( fieldmap );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gameData );
  SODATEYA*  sodateya = FIELDMAP_GetSodateya( fieldmap );

  // ��ĉ�: �o���l���Z, �q��蔻��Ȃ�1�����̏���
  if ( SODATEYA_BreedPokemon( sodateya ) == TRUE )
  {
    //R03�̂�������C�x���g�p�F�^�}�S�����܂ꂽ�烏�[�N��ύX����
    EVENTWORK *ev = GAMEDATA_GetEventWork( req->gamedata );
    u16 * work = EVENTWORK_GetEventWorkAdrs( ev, WK_OTHER_DISCOVER_EGG );
    *work = 1;
  }

  // �莝���^�}�S: �z���J�E���^�X�V
  updatePartyEgg( gameData, party );

  // �莝���^�}�S: �z���`�F�b�N
  if( checkPartyEgg( party ) )
  {
    return SCRIPT_SetEventScript( gameSystem, SCRID_EGG_BIRTH, NULL, heapID );
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief �Ȃ��x�㏸����
 * @param gamedata
 * @param fieldmap
 */
//--------------------------------------------------------------
static void updateFriendyStepCount( GAMEDATA * gamedata, FIELDMAP_WORK * fieldmap )
{
  enum {
    FRIENDLY_STEP_MAX = 128,
  };
  SITUATION * sit = SaveData_GetSituation( GAMEDATA_GetSaveControlWork( gamedata ) );
  u16 step_count;
  BOOL result = FALSE;
  step_count = Situation_GetFriendlyStepCount( sit );
  step_count ++;
  if ( step_count >= FRIENDLY_STEP_MAX )
  {
    step_count = 0;
    result = TRUE;
  }
  Situation_SetFriendlyStepCount( sit, step_count );
  if ( result )
  {
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
    HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );
    int count = PokeParty_GetPokeCount( party );
    int i;

    for ( i = 0; i < count; i++ )
    {
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
      NATSUKI_CalcTsurearuki( pp, zone_id, heapID );
    }
  }
}



//======================================================================
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �C��_�a�@�����C�x���g�`�F�b�N
 *
 *	@param	gsys  �Q�[���V�X�e��
 *
 *	@return �Q�[���C�x���g
 */
//-----------------------------------------------------------------------------
static GMEVENT* CheckSeaTemple( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u16 zone_id )
{
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* saveData = GAMEDATA_GetSaveControlWork(gdata);
  SITUATION* situation = SaveData_GetSituation( saveData );
  u16 count;
  GMEVENT * event = NULL;

  if( ZONEDATA_IsSeaTempleDungeon( zone_id ) ){
    // �����̃X�N���v�g�N��
    event = EVENT_SeaTemple_GetStepEvent( gsys, fieldmap );
  
    // �����J�E���g
    count = Situation_GetSeaTempleStepCount( situation );
    count ++;
    Situation_SetSeaTempleStepCount( situation, count );
  }

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * 1�����Ƃ̃X�v���[���ʌ��Z
 */
//--------------------------------------------------------------
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
//======================================================================
//--------------------------------------------------------------
/**
 * 1�����Ƃ̃G���J�E���g�G�t�F�N�g�N���`�F�b�N
 */
//--------------------------------------------------------------
static GMEVENT* CheckEffectEncount( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata )
{
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork(gdata);
  FIELD_ENCOUNT * encount = FIELDMAP_GetEncount(fieldWork);
  GMEVENT* event = NULL;
  
  //�G�t�F�N�g�G���J�E���g�ڐG�`�F�b�N
  event = EFFECT_ENC_CheckEventApproch( encount );
  if( event != NULL){
    return event;
  }
  //�G�t�F�N�g�G���J�E���g�N���`�F�b�N
  EFFECT_ENC_CheckEffectEncountStart( encount );
	return NULL;
}

//--------------------------------------------------------------
/**
 * ���A���^�C���N���b�N�ɂ��G�p���[���ʏI���`�F�b�N
 */
//--------------------------------------------------------------
static GMEVENT* CheckGPowerEffectEnd( GAMESYS_WORK* gsys )
{
  if( GPOWER_Get_FinishWaitID() == GPOWER_ID_NULL ){
    return NULL;
  }
  return EVENT_GPowerEffectEnd( gsys );
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

		fourkingsCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_A,
	};

	//�\�t�g���Z�b�g�`�F�b�N
	if( (req->key_cont & resetCont) == resetCont ){
#if 0
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
#endif
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (req->key_cont & chgCont) == chgCont ){
#if 0
    ISDPrintSetBlockingMode(1);
    GFL_HEAP_DEBUG_PrintSystemInfo();
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_FIELDMAP );
    ISDPrintSetBlockingMode(0);
#endif
		//return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
  if( req->menuRequest && req->debugRequest )
  {
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				req->heapID, ZONEDATA_GetMapRscID(req->map_id));
	}

#if defined(DEBUG_ONLY_FOR_tomoya_takahashi) | defined(DEBUG_ONLY_FOR_nakatsui) | defined(DEBUG_ONLY_FOR_takahashi) | defined(DEBUG_ONLY_FOR_hozumi_yukiko) | defined(DEBUG_ONLY_FOR_iwao_kazumasa) | defined(DEBUG_ONLY_FOR_murakami_naoto) | defined(DEBUG_ONLY_FOR_nakamura_akira) | defined(DEBUG_ONLY_FOR_suginaka_katsunori)
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
    if ( CONNECTDATA_IsClosedExit(cnct) == TRUE || checkConnectIsFreeExit( cnct ) == FALSE )
    {
      return NULL;
    }
  }

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  rememberExitInfo(req, fieldWork, idx, now_pos);
  return getChangeMapEvent(req, fieldWork, idx, now_pos);
}

//--------------------------------------------------------------
/**
 * �C�x���g �A�g���r���[�g�`�F�b�N
 *
 * @param req   �C�x���g�`�F�b�N�p���[�N
 * @param fieldWork FIELDMAP_WORK
 * @return GMEVENT  ���������C�x���g�f�[�^�ւ̃|�C���^�B NULL�̏ꍇ�A�C�x���g�Ȃ�
 */
//--------------------------------------------------------------
static GMEVENT * checkAttribute(EV_REQUEST * req, FIELDMAP_WORK *fieldWork )
{
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attrval;
  FLDMAPPER* mapper;

  // �����̃A�g���r���[�g���擾
  FIELD_PLAYER_GetPos( req->field_player, &pos );
  mapper  = FIELDMAP_GetFieldG3Dmapper( fieldWork );
  attr    = MAPATTR_GetAttribute( mapper, &pos );
  attrval = MAPATTR_GetAttrValue( attr );

  if( CheckAttributeIsAutoWay(attrval) )
  { // �ړ���
    return EVENT_PlayerMoveOnAutoWay( NULL, req->gsys, fieldWork );
  }
  else if( CheckAttributeIsCurrent(attrval) )
  { // ����
    return EVENT_PlayerMoveOnCurrent( NULL, req->gsys, fieldWork );
  }
  return NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const VecFx32 * pos)
{
  LOC_EXIT_OFS exit_ofs;
	const CONNECT_DATA * cnct;
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  if (pos != NULL) {
    exit_ofs = CONNECTDATA_GetExitOfs(cnct, pos);
  } else {
    exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  }

  return EVENT_ChangeMapByConnect(req->gsys, fieldWork, cnct, exit_ofs);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void rememberExitInfo(EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const VecFx32 * pos)
{
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  LOCATION ent_loc;
  LOCATION_Set( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0, LOCATION_DEFAULT_EXIT_OFS,
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
  if ( checkConnectIsFreeExit( cnct ) == FALSE )
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
/**
 * @brief ���ނ����o�������ǂ����̃`�F�b�N
 * @param cnct
 * @retval  TRUE  ���ނ���
 * @retval  FALSE ����ȊO�i�����ⓖ���蔻�������j
 */
//--------------------------------------------------------------
static BOOL checkConnectIsFreeExit( const CONNECT_DATA * cnct )
{
  EXIT_TYPE exit_type = CONNECTDATA_GetExitType( cnct );
  return ( exit_type == EXIT_TYPE_NONE
      || exit_type == EXIT_TYPE_WARP
      || exit_type == EXIT_TYPE_INTRUDE );
}

//--------------------------------------------------------------
/**
 * @brief �o�����̎������w��`�F�b�N
 * @param cnct
 * @param player_dir
 * @retval  TRUE  �K�����Ă���
 * @retval  FALSE �K�����Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir)
{
  EXIT_DIR exit_dir;

  exit_dir = CONNECTDATA_GetExitDir( cnct );
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
  {
		FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &front_pos) );
    if (!(flag & MAPATTR_FLAGBIT_HITCH))
    {
      return NULL;
    }
  }


  //�����`�F�b�N�i�}�b�g�j
  idx = getConnectID(req, req->now_pos);
  if (idx != EXIT_ID_NONE)
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if ( CONNECTDATA_IsClosedExit(cnct) == FALSE && CONNECTDATA_GetExitType(cnct) == EXIT_TYPE_MAT )
    {
      rememberExitInfo(req, fieldWork, idx, req->now_pos);
      return getChangeMapEvent(req, fieldWork, idx, req->now_pos);
    }
  }
	
  //�ڂ̑O�`�F�b�N�i�K�i�A�ǁA�h�A�j
  idx = getConnectID(req, &front_pos);
  if (idx != EXIT_ID_NONE) 
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if ( CONNECTDATA_IsClosedExit(cnct) == FALSE )
    {
      //�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
      rememberExitInfo(req, fieldWork, idx, &front_pos);
      return getChangeMapEvent(req, fieldWork, idx, &front_pos);
    }
  }


  // �����C�x���g������
  return NULL;
}

//--------------------------------------------------------------
/**
 * �C�x���g �L�[���̓M�~�b�N�N���`�F�b�N
 *
 * @param req   �C�x���g�`�F�b�N�p���[�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * checkPushGimmick(const EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  int idx;
	VecFx32 front_pos;
  
  setFrontPos(req, &front_pos);
  
  //�M�~�b�N�����蓖�Ă��Ă��邩�𒲂ׂāA
  //�W���\�[�X�̃I�[�o���C���Ȃ���Ă��邱�Ƃ��m�F����
  if ( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_GYM_INSECT) )
  {
    //���W��
    int x,z;
    x =front_pos.x / FIELD_CONST_GRID_FX32_SIZE;
    z =front_pos.z / FIELD_CONST_GRID_FX32_SIZE;
    return GYM_INSECT_CreatePushWallEvt(gsys, x, z);
  }
  
  //D06�d�C���A
  if( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0101) ||
      FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0201) ||
      FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0301) )
  {
    return( D06_GIMMICK_CheckPushEvent(fieldWork,req->player_dir) );
  }
  
  { //�i���֎~�A�g���r���[�g�����邩
    FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR attr = MAPATTR_GetAttribute( g3Dmapper, &front_pos );
    MAPATTR_FLAG attr_flag =  MAPATTR_GetAttrFlag( attr );
    
    if( !(attr_flag & MAPATTR_FLAGBIT_HITCH) )
    {
      return NULL; //�O���i���֎~�A�g���r���[�g����
    }
    
    { //�g����Ԃł̑ꉺ��`�F�b�N
      PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( req->field_player );
      
      if( form == PLAYER_MOVE_FORM_SWIM )
      {
        if( checkPokeWazaGroup(req->gamedata,WAZANO_TAKINOBORI) != 6 )
        {
          MAPATTR_VALUE attr_value = MAPATTR_GetAttrValue( attr );
        
          if( MAPATTR_VALUE_CheckWaterFall(attr_value) == TRUE )
          {
            fx32 t_height;
            VecFx32 t_pos = front_pos;
            MMDL *mmdl = FIELD_PLAYER_GetMMdl( req->field_player );
            MMDL_TOOL_AddDirVector( req->player_dir, &t_pos, GRID_FX32 );
          
            if( MMDL_GetMapPosHeight(mmdl,&t_pos,&t_height) == TRUE )
            {
              if( front_pos.y > t_height ) //����Ȃ�ꉺ��C�x���g
              {
                return SCRIPT_SetEventScript(
                    req->gsys, SCRID_HIDEN_TAKIKUDARI, NULL, req->heapID );
              }
            }
          }
        }
      }
    }
    
    //�ǂ̏ꍇ�A�d�C�W�����𒲂ׂ�
    //�M�~�b�N�����蓖�Ă��Ă��邩�𒲂ׂāA
    //�W���\�[�X�̃I�[�o���C���Ȃ���Ă��邱�Ƃ��m�F����
    {
      idx = getConnectID(req, &front_pos);
      
      if( idx == EXIT_ID_NONE )
      {
        if( FLDGMK_GimmickCodeCheck(fieldWork,FLD_GIMMICK_GYM_ELEC) )
        { //�d�C�W��
          return GYM_ELEC_CreateMoveEvt(gsys);
        }
        else if( FLDGMK_GimmickCodeCheck(fieldWork,FLD_GIMMICK_GYM_FLY) )
        { //��s�W��
          return GYM_FLY_CreateShotEvt(gsys);
        }
      }
    }
  }
  
  return NULL;
}

//--------------------------------------------------------------
/**
 * �C�x���g �L�[���́F�N���M�~�b�N�N���`�F�b�N
 *
 * @param req   �C�x���g�`�F�b�N�p���[�N
 * @param gsys GAMESYS_WORK
 * @param	fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT * checkPushIntrude(const EV_REQUEST * req,
		GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
	VecFx32 front_pos;
  int idx;

  if(ZONEDATA_IsPalace(req->map_id) == FALSE){
    return NULL;
  }
  
  setFrontPos(req, &front_pos);
  return Intrude_CheckPushEvent(gsys, fieldWork, req->field_player, req->now_pos, &front_pos, req->player_dir);
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
  const CONNECT_DATA* cnct;

  // @TODO �RD���W�̃C�x���g�ƃ��[�����W�̃C�x���g�𕹗p���Ă��邽�ߕ��G
  {
    VecFx32 pos_3d;
    FIELD_PLAYER_GetNoGridPos( req->field_player, &pos_3d );
    idx = EVENTDATA_SearchConnectIDBySphere(req->evdata, &pos_3d);
  }
  if( idx == EXIT_ID_NONE )
  {
    FIELD_PLAYER_GetNoGridLocation( req->field_player, &pos );
    idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &pos);
  }

  if (*firstID == idx) return NULL;
  if (idx == EXIT_ID_NONE){
    *firstID = idx;
    return NULL;
  }

  // ����������ŋN������̂��`�F�b�N
  cnct = EVENTDATA_GetConnectByID( req->evdata, idx );
  if( CONNECTDATA_IsClosedExit(cnct) == TRUE || checkConnectIsFreeExit( cnct ) == FALSE  )
  {
    return NULL;
  }
  
  rememberExitRailInfo( req, fieldWork, idx, &pos );
  return getRailChangeMapEvent(req, fieldWork, idx, &pos);
}

//--------------------------------------------------------------
/**
 *	@brief  �m�[�O���b�h�}�b�v�@���[���V�X�e����ł̉������ݏo���������
 */
//--------------------------------------------------------------
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

    if( CONNECTDATA_IsClosedExit(cnct) == FALSE && checkConnectExitDir( cnct, req->player_dir ) )
    {
      if( CONNECTDATA_GetExitType( cnct ) == EXIT_TYPE_MAT)
      {
        // GO
        rememberExitRailInfo( req, fieldWork, idx, &pos );
        return getRailChangeMapEvent(req, fieldWork, idx, &pos);
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

  if( CONNECTDATA_IsClosedExit(cnct) == FALSE && checkConnectExitDir( cnct, req->player_dir ) )
  {
    rememberExitRailInfo( req, fieldWork, idx, &pos );  // front_pos�͈ړ��s�Ȃ̂ŁApos�ɂ���
    return getRailChangeMapEvent(req, fieldWork, idx, &pos);
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
static GMEVENT * checkIntrudeSubScreenEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, ZONEID zone_id)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  FIELD_SUBSCREEN_WORK * subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
  FIELD_SUBSCREEN_MODE subscreen_mode = Intrude_SUBSCREEN_Watch(game_comm, subscreen, zone_id);
  GMEVENT* event = NULL;
  
  if(subscreen_mode != FIELD_SUBSCREEN_MODE_MAX){
    event = EVENT_ChangeSubScreen(gsys, fieldWork, subscreen_mode);
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
  case FIELD_SUBSCREEN_ACTION_IRC:
#if 0 //�m�F�����ŃC�x���g���N�����悤�ɏC�� �ŏI�I�ɏ����܂�
    {
			GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);
			GAME_COMM_NO no = GameCommSys_BootCheck(gcsp);
			if(GAME_COMM_NO_FIELD_BEACON_SEARCH == no || GAME_COMM_NO_DEBUG_SCANONLY == no){
				GameCommSys_ExitReq(gcsp);
			}
			if((GAME_COMM_NO_FIELD_BEACON_SEARCH == no) || (GAME_COMM_NO_NULL == no)||
         (GAME_COMM_NO_DEBUG_SCANONLY == no)){
				event = EVENT_IrcBattle(gsys, fieldWork, NULL, TRUE);
			}
		}
#endif
    event = EVENT_IrcBattle(gsys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_GSYNC:
    event = EVENT_GSync(gsys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_WIRELESS:
    event = EVENT_CG_Wireless(gsys, fieldWork, NULL, TRUE);
    break;
  case FIELD_SUBSCREEN_ACTION_PALACE_WARP:
    event = EVENT_IntrudeTownWarp(gsys, fieldWork, PALACE_TOWN_DATA_PALACE);
    break;
  
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW:
    event = EVENT_ChangeSubScreen(gsys, fieldWork, FIELD_SUBSCREEN_BEACON_VIEW);
    break;
  case FIELD_SUBSCREEN_ACTION_SCANRADAR:
    event = EVENT_ResearchRadar( gsys, fieldWork );
    break;
  case FIELD_SUBSCREEN_ACTION_CGEAR_HELP:
    {
      CG_HELP_INIT_WORK *initWork = GFL_HEAP_AllocClearMemory( HEAPID_PROC,sizeof(CG_HELP_INIT_WORK) );
      initWork->myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));
      event = EVENT_FieldSubProc_Callback(gsys, fieldWork, FS_OVERLAY_ID(cg_help),&CGearHelp_ProcData,initWork,NULL,initWork);
    }
    break;
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR:
    event = EVENT_ChangeSubScreen(gsys, fieldWork, FIELD_SUBSCREEN_NORMAL);
    break;
  case FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_INTRUDE:
    event = EVENT_ChangeSubScreen(gsys, fieldWork, FIELD_SUBSCREEN_INTRUDE);
    break;
#if 0
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
  
  // WFBC
  if( ZONEDATA_IsWfbc( req->map_id ) )
  {
    FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    FIELD_WFBC * p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
    return (GMEVENT*)FIELD_ENCOUNT_CheckWfbcEncount(encount, p_wfbc );
  }
  
  enc_type = ENC_TYPE_NORMAL; //(ZONEDATA_IsBingo( req->map_id ) == TRUE) ? ENC_TYPE_BINGO : ENC_TYPE_NORMAL;
  return (GMEVENT*)FIELD_ENCOUNT_CheckEncount(encount, enc_type);
}

//--------------------------------------------------------------
/**
 *	@brief  ���[�����藎���`�F�b�N
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�̏o���������ۑ�
 */
//--------------------------------------------------------------
static void rememberExitRailInfo(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx, const RAIL_LOCATION* loc)
{
	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
  LOCATION ent_loc;
  LOCATION_SetRail( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0, LOCATION_DEFAULT_EXIT_OFS,
      loc->rail_index, loc->line_grid, loc->width_grid);
  GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
}

//--------------------------------------------------------------
/**
 *	@brief  ���[���̃}�b�v�ύX�C�x���g���擾����
 */
//--------------------------------------------------------------
static GMEVENT * getRailChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const RAIL_LOCATION* loc)
{
  LOC_EXIT_OFS exit_ofs;
	const CONNECT_DATA * cnct;
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  exit_ofs = CONNECTDATA_GetRailExitOfs( cnct, loc );
  return EVENT_ChangeMapByConnect(req->gsys, fieldWork, cnct, exit_ofs);
}

//--------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�ڂ̑O�Ɉړ����Ă邩�`�F�b�N
 *
 *	@retval TRUE    ���Ă���
 *	@retval FALSE   ���Ă��Ȃ�
 */
//--------------------------------------------------------------
static BOOL checkRailFrontMove( const FIELD_PLAYER* cp_player )
{
  RAIL_LOCATION now;
  RAIL_LOCATION old;
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );

  
  MMDL_GetRailLocation( cp_mmdl, &now );
  MMDL_GetOldRailLocation( cp_mmdl, &old );

  if( GFL_STD_MemComp( &now, &old, sizeof(RAIL_LOCATION) ) == 0 )
  {
    return FALSE;
  }
  return TRUE;
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
        
        FIELD_PLAYER_SetEffectTaskWork( fld_player, task );
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
      task = FIELD_PLAYER_GetEffectTaskWork( fld_player );
      FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_ON );
      FIELD_PLAYER_SetNaminori( fld_player );
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
    task = FIELD_PLAYER_GetEffectTaskWork( fld_player );
    FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_OFF );
    FIELD_PLAYER_SetRequest( fld_player, FIELD_PLAYER_REQBIT_NORMAL );
    FIELD_PLAYER_UpdateRequest( fld_player );
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
      FIELD_PLAYER_SetNaminoriEnd( fld_player );
      FIELD_PLAYER_SetNaminoriEventEnd( fld_player, TRUE );
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
    u16 dir = FIELD_PLAYER_GetKeyDir( req->field_player, req->key_cont );
    
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


//--------------------------------------------------------------
/**
 *	@brief  ���[���}�b�v�ł́A�ڂ̑O�̃��f�����擾
 *
 *	@param	req
 *	@param	*fieldMap 
 *
 *	@return
 */
//--------------------------------------------------------------
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
    { MAPATTR_VALUE_CheckPC,            DIR_UP,  SCRID_PC },
    { MAPATTR_VALUE_CheckMap,           DIR_NOT, EVENTDATA_ID_NONE },
    { MAPATTR_VALUE_CheckTV,            DIR_NOT, EVENTDATA_ID_NONE },
    { MAPATTR_VALUE_CheckBookShelf1,    DIR_NOT, SCRID_BG_MSG_BOOK1_01 },
    { MAPATTR_VALUE_CheckBookShelf2,    DIR_NOT, SCRID_BG_MSG_BOOK2_01 },
    { MAPATTR_VALUE_CheckBookShelf3,    DIR_NOT, SCRID_BG_MSG_BOOKRACK1_01 },
    { MAPATTR_VALUE_CheckBookShelf4,    DIR_NOT, SCRID_BG_MSG_BOOKRACK2_01 },
    { MAPATTR_VALUE_CheckVase,          DIR_NOT, SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckDustBox,       DIR_NOT, SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckShopShelf1,    DIR_NOT, SCRID_BG_MSG_SHOPRACK1_01 },
    { MAPATTR_VALUE_CheckShopShelf2,    DIR_NOT, SCRID_BG_MSG_SHOPRACK2_01 },
    { MAPATTR_VALUE_CheckShopShelf3,    DIR_NOT, SCRID_BG_MSG_SHOPRACK3_01 },
    { MAPATTR_VALUE_CheckWaterFall,     DIR_NOT, SCRID_HIDEN_TAKINOBORI },
    { MAPATTR_VALUE_CheckVendorMachine, DIR_UP,  SCRID_VENDING_MACHINE01 },
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
    if ( check_attr_data[i].dir_code == req->player_dir || check_attr_data[i].dir_code == DIR_NOT )
    {
      if ( check_attr_data[i].attr_check_func(attr_val) )
      {
        return check_attr_data[i].script_id;
      }
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


//--------------------------------------------------------------
/**
 * 2vs2�ΐ킪�\���ǂ����`�F�b�N
 * @param
 * @retval
 */
//--------------------------------------------------------------
u32 FIELD_EVENT_CountBattleMember( GAMESYS_WORK *gsys )
{
  int i,max,count;
  POKEMON_PARAM *pp;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gdata );
  
  max = PokeParty_GetPokeCount( party );
  
  for( i = 0, count = 0; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    //���܂��`�F�b�N
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //HP�`�F�b�N
    if( PP_Get(pp,ID_PARA_hp,NULL) == 0 ){
      continue;
    }

    count++;
  }
  
  return count;
}

