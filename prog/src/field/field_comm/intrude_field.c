//==============================================================================
/**
 * @file    intrude_field.c
 * @brief   �N���Ńt�B�[���h���ɍs�����C���F�t�B�[���h�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "infowin/infowin.h"
#include "gamesystem/gamesystem.h"
#include "field_comm_menu.h"
#include "field_comm_data.h"
#include "field/field_comm/palace_sys.h"
#include "test/performance.h"
#include "system/main.h"
#include "field/field_comm_actor.h"
#include "gamesystem/game_comm.h"
#include "fieldmap/zone_id.h"
#include "field/event_mapchange.h"
#include "intrude_types.h"
#include "intrude_field.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "system/net_err.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_invasion.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�f�o�b�O�p
typedef struct{
  VecFx32 pos;
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELD_MAIN_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  ZONEID zoneID;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_WARP;  //DEBUG_SIOEND_MAPCHANGE;

///�f�o�b�O�p
typedef struct{
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELD_MAIN_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_CHILD;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, INTRUDE_COMM_SYS_PTR intcomm );
static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work);
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm);
static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work);
static void DEBUG_PalaceMapInCheck(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor);


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���X�V
//  @param  FIELD_MAIN_WORK �t�B�[���h���[�N
//  @param  GAMESYS_WORK  �Q�[���V�X�e�����[�N(PLAYER_WORK�擾�p
//  @param  PC_ACTCONT    �v���C���[�A�N�^�[(�v���C���[���l�擾�p
//  �����̃L�����̐��l���擾���ĒʐM�p�ɕۑ�
//  ���L�����̏����擾���A�ʐM����ݒ�
//--------------------------------------------------------------
void IntrudeField_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor )
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  int i;
  BOOL update_ret;
  
  //�p���X�}�b�v�ɗ������`�F�b�N
  DEBUG_PalaceMapInCheck(fieldWork, gameSys, game_comm, pcActor);

#if 0
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION){
    if(intcomm->palace != NULL){ //��������������ۂ����Ǐ��F�f�o�b�O�p�̃A�N�^�[�Ȃ̂�
      PALACE_DEBUG_EnableNumberAct(intcomm->palace, FALSE);
    }
    return;
  }
#endif

  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL
      || intcomm->comm_status != INTRUDE_COMM_STATUS_UPDATE){
    return;
  }

  if(CommPlayer_Mine_DataUpdate(intcomm->cps, &intcomm->intrude_status_mine.player_pack) == TRUE){
    intcomm->send_status = TRUE;
  }
  if(intcomm->send_status == TRUE){
    IntrudeSend_PlayerStatus(intcomm, GAMESYSTEM_GetGameData(gameSys), 
      &intcomm->intrude_status_mine, PALACE_SYS_GetArea(intcomm->palace), 
      PALACE_SYS_GetMissionNo(intcomm->palace));
  }

  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(intcomm->recv_status & (1 << i)){
      if(CommPlayer_CheckOcc(intcomm->cps, i) == FALSE){
        MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(GAMESYSTEM_GetGameData(gameSys), i);
        CommPlayer_Add(intcomm->cps, i, MyStatus_GetMySex(myst), 
          &intcomm->intrude_status[i].player_pack);
      }
      else{
        CommPlayer_SetParam(intcomm->cps, i, &intcomm->intrude_status[i].player_pack);
      }
    }
  }
  
  PALACE_SYS_Update(intcomm->palace, GAMESYSTEM_GetMyPlayerWork( gameSys ), 
    pcActor, intcomm, fieldWork, game_comm);
}

//==================================================================
/**
 *   �؂ɏd�Ȃ�����T1�փ��[�v
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * DEBUG_IntrudeTreeMapWarp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL){
    return NULL;
  }
  
  if(intcomm->comm_status == INTRUDE_COMM_STATUS_UPDATE){
    if(intcomm->exit_recv == TRUE){
      return DEBUG_EVENT_ChildCommEnd(gameSys, fieldWork, intcomm);
    }
  }
  
  if(zone_id != ZONE_ID_PALACETEST){
    return NULL;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &pos );
  if(pos.x >= FX32_CONST(744) && pos.x <= FX32_CONST(776) 
      && pos.z >= FX32_CONST(72) && pos.z <= FX32_CONST(88-1)){
    pos.x = 12536 << FX32_SHIFT;
    pos.y = 0;
    pos.z = 12120 << FX32_SHIFT;
    
    if(intcomm != NULL 
        && GFL_NET_GetConnectNum() > 1 
        && GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gameSys)) == GAME_COMM_NO_INVASION
        && GFL_NET_IsParentMachine() == TRUE
        && intcomm->invalid_netid == GFL_NET_SystemGetCurrentID()){
      //�e���ʐM��ԂŎ����̊X�ɓ���ꍇ�͐ؒf
      return DEBUG_EVENT_ChangeMapPosCommEnd(gameSys, fieldWork, ZONE_ID_T01, &pos, intcomm);
    }
    return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_T01, &pos, 0);
  }
  
  {//���@�̍��W���Ď����A�N�Ƃ��ʐM���Ă��Ȃ��̂Ƀp���X�̋��̈��ʒu�܂ŗ�����
   //���Ӄ��b�Z�[�W���o���Ĉ����Ԃ�����
    VecFx32 pos;
    BOOL left_right;
    
    FIELD_PLAYER_GetPos( pcActor, &pos );
    pos.x >>= FX32_SHIFT;
    if(GFL_NET_GetConnectNum() <= 1 && (pos.x <= 536 || pos.x >= 1000)){
      left_right = pos.x <= 536 ? 0 : 1;
      return EVENT_DebugPalaceNGWin( gameSys, fieldWork, pcActor, left_right );
    }
  }
  
  return NULL;
}


//==================================================================
/**
 * ���f�o�b�O�p�@�ʐM���I�������Ă���}�b�v�ړ�
 *
 * @param   gsys		
 * @param   fieldmap		
 * @param   zone_id		
 * @param   pos		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, INTRUDE_COMM_SYS_PTR intcomm )
{
  DEBUG_SIOEND_WARP *dsw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_MapChangeCommEnd, sizeof(DEBUG_SIOEND_WARP));
	dsw = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsw, sizeof(DEBUG_SIOEND_WARP));
	dsw->fieldmap = fieldmap;
	dsw->zoneID = zone_id;
	dsw->pos = *pos;
	dsw->intcomm = intcomm;
	
	OS_TPrintf("�e�@�F�ʐM�ؒf�C�x���g�N��\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work)
{
	DEBUG_SIOEND_WARP * dsw = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELD_MAIN_WORK * fieldmap = dsw->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //�ʐM�I�����N�G�X�g
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsw->fieldmap);
      dsw->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsw->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsw->msgData );
      FLDMSGWIN_Print( dsw->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsw->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsw->wait++;
    if(dsw->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsw->msgWin );
    GFL_MSG_Delete( dsw->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	case 4:
  	GMEVENT_CallEvent(
  	  event, DEBUG_EVENT_ChangeMapPos(gsys, dsw->fieldmap, dsw->zoneID, &dsw->pos, 0));
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���f�o�b�O�p�@�q�@�̒ʐM�I��
 *
 * @param   gsys		
 * @param   fieldmap		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm)
{
  DEBUG_SIOEND_CHILD *dsc;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_ChildCommEnd, sizeof(DEBUG_SIOEND_CHILD));
	dsc = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsc, sizeof(DEBUG_SIOEND_CHILD));
	dsc->fieldmap = fieldmap;
	dsc->intcomm = intcomm;
	
	OS_TPrintf("�q�@�F�ʐM�ؒf�C�x���g�N��\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work)
{
  DEBUG_SIOEND_CHILD *dsc = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELD_MAIN_WORK * fieldmap = dsc->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //�ʐM�I�����N�G�X�g
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsc->fieldmap);
      dsc->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsc->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsc->msgData );
      FLDMSGWIN_Print( dsc->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsc->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsc->wait++;
    if(dsc->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsc->msgWin );
    GFL_MSG_Delete( dsc->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�F�p���X�}�b�v�ɓ����Ă������`�F�b�N���A�����Ă���ΒʐM�N���Ȃǂ�����
 *   ��check�@�����I�ɃX�N���v�g�Ń}�b�v���Ɏ����N���C�x���g���o����悤�ɂȂ����炱��͍폜����
 *
 * @param   fieldWork		
 * @param   gameSys		
 */
//--------------------------------------------------------------
extern u8 debug_palace_comm_seq;
static void DEBUG_PalaceMapInCheck(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
//  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  ZONEID zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(GAMESYSTEM_GetGameData(gameSys)));
  GAME_COMM_NO comm_no;
  
  switch(debug_palace_comm_seq){
  case 0:
    comm_no = GameCommSys_BootCheck(game_comm);
    //�q�Ƃ��Đڑ������ꍇ
    if(comm_no == GAME_COMM_NO_INVASION){
      debug_palace_comm_seq = 2;
      break;
    }
    
    {
      VecFx32 pos;
      
      FIELD_PLAYER_GetPos( pcActor, &pos );
      pos.x >>= FX32_SHIFT;
      if( !((pos.x >= 512 && pos.x <= 648) || (pos.x >= 888 && pos.x <= 1016)) ){
        //���ł͂Ȃ��ꏊ�ɂ���Ȃ�ʐM�͊J�n���Ȃ�
        return;
      }
    }
    
    //�r�[�R���T�[�`��ԂŃp���X�ɓ����Ă����ꍇ
    if(zone_id == ZONE_ID_PALACETEST && comm_no == GAME_COMM_NO_FIELD_BEACON_SEARCH){
      OS_TPrintf("�r�[�R���T�[�`���I��\n");
      GameCommSys_ExitReq(game_comm);
      debug_palace_comm_seq++;
      break;
    }
    
    //�e�Ƃ��ċN�����Ă���ꍇ�̃`�F�b�N
    if(zone_id != ZONE_ID_PALACETEST || GFL_NET_IsExit() == FALSE || comm_no != GAME_COMM_NO_NULL){
      return;
    }
    debug_palace_comm_seq++;
    break;
  case 1:
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(HEAPID_PROC), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->my_invasion = TRUE;
      invalid_parent->gsys = gameSys;
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
      GameCommSys_Boot(game_comm, GAME_COMM_NO_INVASION, invalid_parent);

      OS_TPrintf("�p���X�ʐM�����N��\n");
      debug_palace_comm_seq++;
    }
    break;
  case 2:
    if(GameCommSys_CheckSystemWaiting(game_comm) == FALSE){
      OS_TPrintf("�p���X�ʐM�����N������\n");
      debug_palace_comm_seq++;
    }
    break;
  case 3:
    if(zone_id == ZONE_ID_PALACETEST){
//      PALACE_DEBUG_CreateNumberAct(intcomm->palace, GFL_HEAP_LOWID(GFL_HEAPID_APP));
      debug_palace_comm_seq++;
    }
    break;
  case 4:
    if(zone_id != ZONE_ID_PALACETEST){
//      PALACE_DEBUG_DeleteNumberAct(intcomm->palace);
      debug_palace_comm_seq = 3;
    }
    break;
  }
}

//==================================================================
/**
 *   �f�o�b�O�p �p���X�ɔ�ԃC�x���g���쐬����
 *  @param   fieldWork		
 *  @param   gameSys		
 *  @param   pcActor		
 *  @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * DEBUG_PalaceJamp(FIELD_MAIN_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;
  ZONEID jump_zone;
  
  if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
  	pos.x = 184 << FX32_SHIFT;
  	pos.y = 0;
  	pos.z = 184 << FX32_SHIFT;
  	jump_zone = ZONE_ID_UNION;
  }
  else{
  	pos.x = 760 << FX32_SHIFT;
  	pos.y = 0;
  	pos.z = 234 << FX32_SHIFT;
  	jump_zone = ZONE_ID_PALACETEST;
  }
  return DEBUG_EVENT_ChangeMapPos(gameSys, fieldWork, jump_zone, &pos, 0);
}
