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
#include "field/field_player.h"
#include "test/performance.h"
#include "system/main.h"
#include "field/field_comm_actor.h"
#include "gamesystem/game_comm.h"
#include "fieldmap/zone_id.h"
#include "field/event_mapchange.h"
#include "intrude_types.h"
#include "intrude_work.h"
#include "intrude_field.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "system/net_err.h"
#include "field/zonedata.h"
#include "field/field_g3d_mapper.h"
#include "intrude_main.h"
#include "intrude_mission.h"
#include "field/intrude_secret_item.h"
#include "field/event_mission_secretitem_ex.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_invasion.h"
#include "fieldmap/map_matrix.naix"  //MATRIX_ID

#include "intrude_minimono.h"   //DEBUG_INTRUDE_Pokemon_Add
#include "field/event_intrude.h"
#include "savedata/symbol_save.h"
#include "field/event_symbol.h"



//==============================================================================
//  �\���̒�`
//==============================================================================
///�f�o�b�O�p
typedef struct{
  VecFx32 pos;
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELDMAP_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  ZONEID zoneID;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_WARP;  //DEBUG_SIOEND_MAPCHANGE;

///�f�o�b�O�p
typedef struct{
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELDMAP_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_CHILD;

///NG���b�Z�[�W�C�x���g�p���[�N
typedef struct{
  FIELDMAP_WORK *fieldWork;
  MMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 dir;
}EVENT_PALACE_NGWIN;

///�o���A�[�C�x���g�p���[�N
typedef struct{
  FIELDMAP_WORK *fieldWork;
  MMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  WORDSET *wordset;
  u16 dir;
  u8 no_change;
  u8 padding;
  INTRUDE_EVENT_DISGUISE_WORK ev_diswork;
}EVENT_PALACE_BARRIER;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, INTRUDE_COMM_SYS_PTR intcomm );
static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work);
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm);
static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work);
static void _PalaceMapCommBootCheck(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor);
static void _PalaceFieldPlayerWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm);
static GMEVENT * EVENT_PalaceNGWin( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir );
static GMEVENT_RESULT _EventPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT * EVENT_PalaceBarrierMove( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir );
static GMEVENT_RESULT _EventPalaceBarrierMove( GMEVENT *event, int *seq, void *wk );



//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
// �t�B�[���h�ʐM�V�X�e���X�V
//  @param  FIELDMAP_WORK �t�B�[���h���[�N
//  @param  GAMESYS_WORK  �Q�[���V�X�e�����[�N(PLAYER_WORK�擾�p
//  @param  PC_ACTCONT    �v���C���[�A�N�^�[(�v���C���[���l�擾�p
//  �����̃L�����̐��l���擾���ĒʐM�p�ɕۑ�
//  ���L�����̏����擾���A�ʐM����ݒ�
//--------------------------------------------------------------
void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor )
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSys);
  int i, my_net_id;
  BOOL update_ret;

#if 0
  //��check�@�f�o�b�O
  if(ZONEDATA_IsBingo(FIELDMAP_GetZoneID(fieldWork)) && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT)){
    DEBUG_INTRUDE_Pokemon_Add(fieldWork);
  }
#endif

  //�p���X�ʐM�����N��ON/OFF����
  _PalaceMapCommBootCheck(fieldWork, gameSys, game_comm, pcActor);

  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL){
    return;
  }

  if(intcomm->palace_in == FALSE){
    PLAYER_WORK *my_player = GAMEDATA_GetMyPlayerWork(gamedata);
    if(ZONEDATA_IsPalace( PLAYERWORK_getZoneID( my_player ) ) == TRUE){
      intcomm->palace_in = TRUE;
    }
  }
  IntrudeField_ConnectMap(fieldWork, gameSys, intcomm);

  if(intcomm->comm_status != INTRUDE_COMM_STATUS_UPDATE){
    return;
  }

  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);

  if(Intrude_SetSendStatus(intcomm) == TRUE){
    intcomm->send_status = TRUE;
  }

  CommPlayer_Update(intcomm->cps);
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(i == my_net_id){
      continue;
    }
    if(intcomm->recv_profile & (1 << i)){
      if(CommPlayer_CheckOcc(intcomm->cps, i) == FALSE){
        MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gamedata, i);
        u16 obj_code = Intrude_GetObjCode(&intcomm->intrude_status[i], 
          GAMEDATA_GetMyStatusPlayer(gamedata, i));
        CommPlayer_Add(intcomm->cps, i, obj_code, &intcomm->intrude_status[i].player_pack);
      }
      else{
        CommPlayer_SetParam(intcomm->cps, i, &intcomm->intrude_status[i].player_pack);
        //�o�g�� or �r���S�o�g�����Ȃ�u�I�v��\��
        if(intcomm->intrude_status[i].action_status == INTRUDE_ACTION_BATTLE
            || intcomm->intrude_status[i].action_status == INTRUDE_ACTION_BINGO_BATTLE){
          CommPlayer_SetGyoeTask(intcomm->cps, i);
        }
      }
    }
  }
  
  _PalaceFieldPlayerWarp(fieldWork, gameSys, pcActor, intcomm);
}

//==================================================================
/**
 * �����̐��ʍ��W�ɑ΂��Ă̒ʐM�v���C���[�b�������`�F�b�N
 *
 * @param   intcomm		
 * @param   fld_player		
 * @param   hit_netid		  �q�b�g�����ꍇ�A�Ώۃv���C���[��NetID����������
 *
 * @retval  BOOL		TRUE:�ʐM�v���C���[�q�b�g�B�@FALSE:���肪���Ȃ�
 */
//==================================================================
BOOL IntrudeField_CheckTalk(INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *fld_player, u32 *hit_netid)
{
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(intcomm == NULL || intcomm->cps == NULL){
    return FALSE;
  }
  
  FIELD_PLAYER_GetFrontGridPos(fld_player, &check_gx, &check_gy, &check_gz);
  if(CommPlayer_SearchGridPos(intcomm->cps, check_gx, check_gz, &out_index) == TRUE){
    OS_TPrintf("Talk�^�[�Q�b�g����! net_id = %d, gx=%d, gz=%d\n", 
      out_index, check_gx, check_gz);
    *hit_netid = out_index;
    return TRUE;
  }
  
  return FALSE;
}

//==================================================================
/**
 * �ʐM�v���C���[����b���������Ă��Ȃ����`�F�b�N
 *
 * @param   intcomm		
 * @param   fld_player		
 * @param   hit_netid		  �b���������Ă���ꍇ�A�Ώۃv���C���[��NetID����������
 *
 * @retval  BOOL		TRUE:�ʐM�v���C���[����b���������Ă���@FALSE:�b���������Ă��Ȃ�
 */
//==================================================================
BOOL IntrudeField_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid)
{
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(intcomm == NULL || intcomm->cps == NULL){
    return FALSE;
  }
  
  //��������b�������Ă��Ȃ��̂ɒl��NULL�ȊO�Ȃ̂͘b��������ꂽ����
  if(intcomm->talk.talk_netid != INTRUDE_NETID_NULL){
    *hit_netid = intcomm->talk.talk_netid;
    return TRUE;
  }
  
  return FALSE;
}

//==============================================================================
//
//  ���̓���ʒu�܂ŗ��āA�N�Ƃ��ʐM���Ă��Ȃ��Ȃ璍�Ӄ��b�Z�[�W���o���Ď��@��߂�
//
//==============================================================================
//--------------------------------------------------------------
/**
 * ���̓���ʒu�܂ŗ��āA�N�Ƃ��ʐM���Ă��Ȃ��Ȃ璍�Ӄ��b�Z�[�W���o���Ď��@��߂�
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   fld_player		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_PalaceNGWin( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir )
{
  EVENT_PALACE_NGWIN *ngwin;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, _EventPalaceNGWinEvent, sizeof(EVENT_PALACE_NGWIN));
  
  ngwin = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( ngwin, sizeof(EVENT_PALACE_NGWIN) );
  
  ngwin->fieldWork = fieldWork;
  ngwin->player_mmdl = FIELD_PLAYER_GetMMdl(fld_player);
  ngwin->dir = dir;
  
  return event;
}

//--------------------------------------------------------------
/**
 * ���̓���ʒu�܂ŗ��āA�N�Ƃ��ʐM���Ă��Ȃ��Ȃ璍�Ӄ��b�Z�[�W���o���Ď��@��߂�
 *
 * @param   event		
 * @param   seq		
 * @param   wk		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _EventPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_PALACE_NGWIN *ngwin = wk;
  
  switch(*seq){
  case 0:
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(ngwin->fieldWork);
      ngwin->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      ngwin->msgWin = FLDMSGWIN_AddTalkWin( msgBG, ngwin->msgData );
      FLDMSGWIN_Print( ngwin->msgWin, 0, 0, plc_connect_06 );
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(ngwin->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( ngwin->msgWin );
    GFL_MSG_Delete( ngwin->msgData );
    (*seq)++;
    break;
  case 4:
    if(MMDL_CheckPossibleAcmd(ngwin->player_mmdl) == TRUE){
      u16 code = (ngwin->dir == DIR_RIGHT) ? AC_WALK_L_16F : AC_WALK_R_16F;
      MMDL_SetAcmd(ngwin->player_mmdl, code);
      (*seq)++;
    }
    break;
  case 5:
    if(MMDL_CheckEndAcmd(ngwin->player_mmdl) == TRUE){
      MMDL_EndAcmd(ngwin->player_mmdl);
      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�F�o���A�[�ړ�
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   fld_player		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_PalaceBarrierMove( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir )
{
  EVENT_PALACE_BARRIER *barrier;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, _EventPalaceBarrierMove, sizeof(EVENT_PALACE_BARRIER));
  
  barrier = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( barrier, sizeof(EVENT_PALACE_BARRIER) );
  
  barrier->fieldWork = fieldWork;
  barrier->player_mmdl = FIELD_PLAYER_GetMMdl(fld_player);
  barrier->dir = dir;
  
  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�o���A�[�ړ�
 *
 * @param   event		
 * @param   seq		
 * @param   wk		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _EventPalaceBarrierMove( GMEVENT *event, int *seq, void *wk )
{
  EVENT_PALACE_BARRIER *barrier = wk;
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  enum{
    _SEQ_INIT,
    _SEQ_INIT_WAIT,
    _SEQ_MSG_INIT,
    _SEQ_MSG_WAIT,
    _SEQ_MSG_LASTKEY_WAIT,
    _SEQ_FINISH,
  };
  
  switch(*seq){
  case _SEQ_INIT:
    if(MMDL_CheckPossibleAcmd(barrier->player_mmdl) == FALSE){
      break;  
    }

    if(intcomm == NULL){  //�ϐg�O�ɃG���[�`�F�b�N�F�ʐM�ؒf��ԂȂ�΂����ŏI��
      *seq = _SEQ_FINISH;
      break;
    }

    // �~�b�V�����p�̌�����
    if(MISSION_GetMissionPlay(&intcomm->mission) == TRUE)
    {  
      barrier->no_change = TRUE;

      // �ϐg�Ȃ����o
      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(
        &barrier->ev_diswork, gsys, barrier->fieldWork, intcomm, DISGUISE_NO_NULL, 0, 0, barrier->dir, FALSE); //<-FALSE�ŕϐg�Ȃ�
    }
    // �����̃G���A�֖߂�
    else if( Intrude_CheckNextPalaceAreaMine( game_comm, gamedata, barrier->dir ) == TRUE ){ // �ړ��悪�����̃G���A���H
      
      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(
        &barrier->ev_diswork, gsys, barrier->fieldWork, intcomm, DISGUISE_NO_NULL, 0, 0, barrier->dir, TRUE);
    }
    // �ʏ�i��
    else{

      u16 disguise_code;
      u8 disguise_type, disguise_sex;
      Intrude_GetNormalDisguiseObjCode(
        GAMEDATA_GetMyStatus(gamedata), &disguise_code, &disguise_type, &disguise_sex);

      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(&barrier->ev_diswork, gsys, barrier->fieldWork, intcomm,
        disguise_code, disguise_type, disguise_sex, barrier->dir, TRUE);
    }
    (*seq)++;
    break;

  case _SEQ_INIT_WAIT:
    // BARRIER�����ڕύX����
    if(IntrudeEvent_Sub_BarrierDisguiseEffectMain(&barrier->ev_diswork, intcomm) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_MSG_INIT:
    if(intcomm == NULL){  //���b�Z�[�W�O�ɃG���[�`�F�b�N�F�ʐM�ؒf��ԂȂ�΂����ŏI��
      *seq = _SEQ_FINISH;
      break;
    }
    
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(barrier->fieldWork);
      barrier->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      barrier->msgWin = FLDMSGWIN_AddTalkWin( msgBG, barrier->msgData );
      barrier->wordset = WORDSET_Create(HEAPID_PROC);
      
      if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
        if(barrier->no_change == TRUE){
          FLDMSGWIN_Print( barrier->msgWin, 0, 0, plc_connect_03 );
        }
        else{
          FLDMSGWIN_Print( barrier->msgWin, 0, 0, plc_connect_02 );
        }
      }
      else{
        STRBUF *expand_buf = GFL_STR_CreateBuffer(256, HEAPID_FIELDMAP);
        STRBUF *src_buf;
        
        WORDSET_RegisterPlayerName( barrier->wordset, 0, 
          GAMEDATA_GetMyStatusPlayer(gamedata, intcomm->intrude_status_mine.palace_area) );
        
        if(barrier->no_change == TRUE){
          src_buf = GFL_MSG_CreateString( barrier->msgData, plc_connect_04 );
        }
        else{
          src_buf = GFL_MSG_CreateString( barrier->msgData, plc_connect_05 );
        }
        WORDSET_ExpandStr(barrier->wordset, expand_buf, src_buf);
        FLDMSGWIN_PrintStrBuf( barrier->msgWin, 0, 0, expand_buf );
        GFL_STR_DeleteBuffer(src_buf);
        GFL_STR_DeleteBuffer(expand_buf);
      }
      (*seq)++;
    }
    break;
  case _SEQ_MSG_WAIT:
    if( FLDMSGWIN_CheckPrintTrans(barrier->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case _SEQ_MSG_LASTKEY_WAIT:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case _SEQ_FINISH:
    if(barrier->msgWin != NULL){
      FLDMSGWIN_Delete( barrier->msgWin );
      GFL_MSG_Delete( barrier->msgData );
      WORDSET_Delete(barrier->wordset);
    }
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//==================================================================
/**
 * �p���X�F���W�C�x���g
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * Intrude_CheckPosEvent(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  INTRUDE_COMM_SYS_PTR intcomm;
  int i;
  
  intcomm = Intrude_Check_CommConnect(game_comm);

  if(intcomm != NULL && intcomm->comm_status == INTRUDE_COMM_STATUS_UPDATE){
    if(intcomm->exit_recv == TRUE){
      return DEBUG_EVENT_ChildCommEnd(gameSys, fieldWork, intcomm);
    }
  }

  FIELD_PLAYER_GetPos( pcActor, &pos );
  
  if(ZONEDATA_IsBingo(zone_id) == TRUE){  //�r���S�}�b�v����E�o�Ď�
    if(pos.z >= FX32_CONST(680)){
    	pos.x = PALACE_MAP_LEN/2;
    	pos.y = 0;
    	pos.z = 408 << FX32_SHIFT;
      return EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_PALACE01, &pos, 0, FALSE);
    }
    return NULL;
  }
  
  
  
  //------------- �������牺�̓p���X���݂̂̏��� -----------------
  
  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return NULL;
  }
  
  //T01�փ��[�v
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(pos.x >= FX32_CONST(504) + PALACE_MAP_LEN*i && pos.x <= FX32_CONST(504) + PALACE_MAP_LEN*i
        && pos.z >= FX32_CONST(712) && pos.z <= FX32_CONST(712)){
      pos.x = 12536 << FX32_SHIFT;
      pos.y = 0;
      pos.z = 12120 << FX32_SHIFT;
      
      if(intcomm != NULL 
//          && GFL_NET_GetConnectNum() > 1 
          && GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gameSys)) == GAME_COMM_NO_INVASION
//          && GFL_NET_IsParentMachine() == TRUE
          && intcomm->intrude_status_mine.palace_area == GFL_NET_SystemGetCurrentID()){
        //�e���ʐM��ԂŎ����̊X�ɓ���ꍇ�͐ؒf
        return DEBUG_EVENT_ChangeMapPosCommEnd(gameSys, fieldWork, ZONE_ID_T01, &pos, intcomm);
      }
      return EVENT_ChangeMapFromPalace(gameSys);
      //return EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_T01, &pos, 0, FALSE);
    }
  }
  //�r���S�}�b�v�փ��[�v
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(pos.x >= FX32_CONST(488) + PALACE_MAP_LEN*i
        && pos.x <= FX32_CONST(536) + PALACE_MAP_LEN*i 
        && pos.z >= FX32_CONST(328) && pos.z <= FX32_CONST(344)){
      pos.x = 248 << FX32_SHIFT;
      pos.y = 0;
      pos.z = 360 << FX32_SHIFT;
      return EVENT_SymbolMapWarp(gameSys, fieldWork, NULL, &pos, 0, 5);
    }
  }
  
#if 0
  {//���@�̍��W���Ď����A�N�Ƃ��ʐM���Ă��Ȃ��̂Ƀp���X�̋��̈��ʒu�܂ŗ�����
   //���Ӄ��b�Z�[�W���o���Ĉ����Ԃ�����
    BOOL left_right;
    
    if(GFL_NET_GetConnectNum() <= 1 && ((pos.x <= 136 && pos.x >= 136-16) || (pos.x >= 872 && pos.x <= 872+16)) && (pos.z >= 472 && pos.z <= 536)){
      left_right = pos.x <= (PALACE_MAP_LEN/2 >> FX32_SHIFT) ? 0 : 1;
      return EVENT_PalaceNGWin( gameSys, fieldWork, pcActor, left_right );
    }
  }
#endif
  return NULL;
}

//==================================================================
/**
 * �N���F�L�[���͋N���C�x���g
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   pcActor		
 * @param   now_pos		    ���݂̍��W
 * @param   front_pos		  �i�s�����̍��W
 * @param   player_dir		���@�̈ړ�����
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * Intrude_CheckPushEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *pcActor, const VecFx32 *now_pos, const VecFx32 *front_pos, u16 player_dir)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  s32 now_gx;
  u16 event_dir = DIR_NOT;
  
  now_gx = FX32_TO_GRID(now_pos->x) % PALACE_MAP_LEN_GRID;
  
  //�p���X�}�b�v�̋��ō����֍s�����Ƃ��Ă���
  if(player_dir == DIR_LEFT){
    if(now_gx == 0){
      event_dir = DIR_LEFT;
    }
  }
  else if(player_dir == DIR_RIGHT){
    if(now_gx == PALACE_MAP_LEN_GRID - 1){
      event_dir = DIR_RIGHT;
    }
  }
  
  if(event_dir != DIR_NOT){
    if(intcomm == NULL || GFL_NET_GetConnectNum() <= 1 || intcomm->member_num < 2){
      return EVENT_PalaceNGWin(gsys, fieldWork, pcActor, event_dir);
    }
    else{
      return EVENT_PalaceBarrierMove(gsys, fieldWork, pcActor, event_dir);
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
static GMEVENT * DEBUG_EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
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
	FIELDMAP_WORK * fieldmap = dsw->fieldmap;
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
  	  //event, EVENT_ChangeMapPos(gsys, dsw->fieldmap, dsw->zoneID, &dsw->pos, 0, FALSE));
      event, EVENT_ChangeMapFromPalace(gsys) );
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
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm)
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
	FIELDMAP_WORK * fieldmap = dsc->fieldmap;
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
 * �����̃p���X���̒��ɂ��邩�A�����ɂ��邩�𔻒�
 *
 * @param   player_pos		
 *
 * @retval  BOOL		    TRUE:���@FALSE:�p���X��
 */
//--------------------------------------------------------------
static BOOL _PlayerPosCheck_PalaceBridge(const VecFx32 *player_pos)
{
  if(player_pos->x >= 0x348000 || player_pos->x <= 0xb8000){
    return TRUE;  //���̏�
  }
  return FALSE;   //�����̃p���X���̒�
}

//--------------------------------------------------------------
/**
 * �p���X���̒��S�t�߂ɂ��邩�A�O���ɂ��邩�𔻒�
 *
 * @param   player_pos		
 *
 * @retval  BOOL		    TRUE:���S�̊O���@FALSE:���S
 *
 * �A���}�b�v�̐����^�C�~���O�Ɏg�p
 * �אڂ����}�b�v�������Ȃ��^�C�~���O�ŘA������
 */
//--------------------------------------------------------------
static BOOL _PlayerPosCheck_PalaceCenter(const VecFx32 *player_pos)
{
  if(player_pos->x > 0x308000 || player_pos->x < 0xe8000){
    return TRUE;  //�p���X���̒��S����O��Ă���
  }
  return FALSE;   //
}

//--------------------------------------------------------------
/**
 * �p���X�ʐM�����N��ON/OFF����
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   game_comm		
 * @param   pcActor		
 */
//--------------------------------------------------------------
static void _PalaceMapCommBootCheck(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor)
{
  ZONEID zone_id;
  GAME_COMM_NO comm_no;

  VecFx32 player_pos;

  if(GameCommSys_CheckSystemWaiting(game_comm) == TRUE){
    return;
  }
  
  zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(GAMESYSTEM_GetGameData(gameSys)));
  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &player_pos );
  comm_no = GameCommSys_BootCheck(game_comm);
  switch(comm_no){
  case GAME_COMM_NO_NULL:
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == TRUE){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(HEAPID_PROC), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->my_invasion = TRUE;
      invalid_parent->gsys = gameSys;
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
      GameCommSys_Boot(game_comm, GAME_COMM_NO_INVASION, invalid_parent);

      OS_TPrintf("�p���X�ʐM�N���F�e\n");
    }
    break;
  case GAME_COMM_NO_INVASION:
    //�����̃p���X���ɂ��Đڑ����Ă��鑊������Ȃ��ꍇ�͒ʐM�I��
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == FALSE
        && GFL_NET_SystemGetConnectNum() <= 1){
      INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
      if(intcomm->comm_status != INTRUDE_COMM_STATUS_BOOT_CHILD){ //�q�Ƃ��Đe��T���ɍs���Ă���ꍇ�͒ʐM�I���ɂ͂܂������Ȃ�
        if(GAMESYSTEM_GetAlwaysNetFlag(gameSys) == TRUE){
          GameCommSys_ChangeReq(game_comm, GAME_COMM_NO_FIELD_BEACON_SEARCH, gameSys);
          OS_TPrintf("�p���X�ʐM�I���F�r�[�R���T�[�`��\n");
        }
        else{
          GameCommSys_ExitReq(game_comm);
          OS_TPrintf("�p���X�ʐM�I���F�ʐMEND\n");
        }
      }
    }
    break;
  case GAME_COMM_NO_FIELD_BEACON_SEARCH:
    //�����̃p���X���ɂ��ċ��̏�ɂ���Ȃ�r�[�R���T�[�`��OFF
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == TRUE){
      GameCommSys_ExitReq(game_comm);
      OS_TPrintf("�r�[�R���T�[�`�I��\n");
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
GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
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
  	pos.x = PALACE_MAP_LEN/2;
  	pos.y = 0;
  	pos.z = 408 << FX32_SHIFT;
  	jump_zone = ZONE_ID_PALACE01;
  }
  return EVENT_ChangeMapPos(gameSys, fieldWork, jump_zone, &pos, 0, FALSE);
}

//--------------------------------------------------------------
/**
 * �p���X�}�b�v�̒[�ɗ�����}�b�v���[�v�������s��
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void _PalaceFieldPlayerWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  u16 player_dir = PLAYERWORK_getDirection_Type(plWork);
  VecFx32 pos, new_pos;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  int i, now_area, new_area;
  BOOL warp = FALSE;
  fx32 left_end, right_end;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSys);
  
#if 1
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL){
    return;
  }
#endif

  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return;
  }
  
  if(intcomm->member_num < 2){
    return;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &pos );
  new_pos = pos;
  
  now_area = intcomm->intrude_status_mine.palace_area;
  new_area = now_area;
  
  left_end = 0;
  right_end = PALACE_MAP_LEN * (intcomm->connect_map_count + 1);  //intcomm->member_num;
  
  //�}�b�v���[�v�`�F�b�N
  if(pos.x < left_end){// && player_dir == DIR_LEFT){
    new_pos.x = right_end - (left_end - pos.x);
    warp = TRUE;
    OS_TPrintf("left warp now_pos.x = %d, new_pos.x = %d, left_end=%d, right_end=%d\n", pos.x>>FX32_SHIFT, new_pos.x>>FX32_SHIFT, left_end>>FX32_SHIFT, right_end>>FX32_SHIFT);
  }
  else if(pos.x > right_end){// && player_dir == DIR_RIGHT){
    new_pos.x = left_end + (pos.x - right_end);
    warp = TRUE;
    OS_TPrintf("right warp now_pos.x = %d, new_pos.x = %d, left_end=%d, right_end=%d\n", pos.x>>FX32_SHIFT, new_pos.x>>FX32_SHIFT, left_end>>FX32_SHIFT, right_end>>FX32_SHIFT);
  }
  
  //���݂̍��W����p���X�G���A�𔻒�
  new_area = pos.x / PALACE_MAP_LEN;

  if(warp == TRUE){
    FIELD_PLAYER_SetPos( pcActor, &new_pos );
    Intrude_PlayerPosRecalc(intcomm, new_pos.x);
  }
  
  new_area = new_area + GFL_NET_SystemGetCurrentID();
  if(new_area >= intcomm->member_num){
    new_area -= intcomm->member_num;
  }
  if(now_area != new_area){
    OS_TPrintf("new_palace_area = %d new_pos_x = %x\n", new_area, new_pos.x);
    intcomm->intrude_status_mine.palace_area = new_area;
    intcomm->send_status = TRUE;
  #if 0 //�p���X���ɗ��������Ńt���OON�ɂ���悤�ɂ��� 2010.03.10(��)
    if(GAMEDATA_GetIntrudeMyID(gamedata) == new_area){
      GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
    }
    else{
      GAMEDATA_SetIntrudeReverseArea(gamedata, TRUE);
    }
  #endif

    // WFBCCOMM�ɐV�ʐM���ʒm
    FIELD_WFBC_COMM_DATA_SetIntrudeNetID( &intcomm->wfbc_comm_data, new_area );
  }
}

//==================================================================
/**
 * �[���̃}�b�v���[�v�ꏊ��A��
 *
 * @param   fieldWork		
 * @param   gameSys		
 * 
 * wb -- col wb wb wb -
 */
//==================================================================
void IntrudeField_ConnectMapInit(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm)
{
#if 0
  MAP_MATRIX *mmatrix = MAP_MATRIX_Create( HEAPID_WORLD );
  
  MAP_MATRIX_Init(mmatrix, NARC_map_matrix_palace01_mat_bin, ZONE_ID_PALACE01);
  
  OS_TPrintf("--- Map�A�� default ----\n");
  FLDMAPPER_Connect( FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );

  MAP_MATRIX_Delete( mmatrix );
#else
  //�p���X�}�b�v�ɓ��������_��1�͎����ŘA�����Ă��邪�A
  //���̏�����ʂ�ꍇ��IntrudeField_ConnectMap�ɘA������C����ׁA
  //0����������
  intcomm->connect_map_count = 0;
#endif
}

//==================================================================
/**
 * �v���t�B�[����M�l�����}�b�v��A��
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   intcomm		
 * 
 * wb -- col wb wb wb -
 */
//==================================================================
void IntrudeField_ConnectMap(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm)
{
  int use_num;
  
  if(intcomm == NULL || fieldWork == NULL || ZONEDATA_IsPalace(FIELDMAP_GetZoneID(fieldWork)) == FALSE){
    return;
  }
  
#if 0 //���[�v�}�b�v�Ȃ̂�1���A������Ă��Ȃ��̂̓`�����̌������ۂ��̂�
      //�Œ�ł���͘A�����Ă��� 2010.01.23(�y)
  use_num = intcomm->member_num - 1;  // -1 = �����̕��͈���
#else
  use_num = intcomm->member_num - 1;
  if(use_num <= 0){
    use_num = 1;
  }
#endif
  if(intcomm->connect_map_count < use_num){
    MAP_MATRIX *mmatrix;
    
    //�אڂ���}�b�v�������Ă���ꍇ�͘A�������͂��Ȃ�
    {
      VecFx32 player_pos;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      FIELD_PLAYER_GetPos( fld_player, &player_pos );
      player_pos.x %= PALACE_MAP_LEN;
      if(_PlayerPosCheck_PalaceCenter(&player_pos) == TRUE){
        return;
      }
    }

    mmatrix = MAP_MATRIX_Create( GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
    MAP_MATRIX_Init(mmatrix,
        NARC_map_matrix_palace02_mat_bin, ZONE_ID_PALACE01, GFL_HEAP_LOWID( HEAPID_FIELDMAP ) );

    do{
      OS_TPrintf("--- Map�A�� %d ----\n", intcomm->connect_map_count + 1);
      FLDMAPPER_Connect( fieldWork, FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );
      intcomm->connect_map_count++;
    }while(intcomm->connect_map_count < use_num);

    MAP_MATRIX_Delete( mmatrix );
  }
}

//==================================================================
/**
 * �}�b�v�P�i�A��
 *
 * @param   fieldWork		
 */
//==================================================================
void IntrudeField_ConnectMapOne(FIELDMAP_WORK *fieldWork)
{
  MAP_MATRIX *mmatrix = MAP_MATRIX_Create( HEAPID_FIELDMAP );
  MAP_MATRIX_Init(mmatrix,
      NARC_map_matrix_palace02_mat_bin, ZONE_ID_PALACE01, GFL_HEAP_LOWID( HEAPID_FIELDMAP ));

  OS_TPrintf("--- Map�A�� One ----\n");
  FLDMAPPER_Connect( fieldWork, FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );

  MAP_MATRIX_Delete( mmatrix );
}

//==================================================================
/**
 * ���@��ϑ�������(OBJ�R�[�h�̕ύX)
 *
 * @param   intcomm		         NULL��n�����ꍇ�͒ʏ�̎p�ɖ߂��܂�
 * @param   gsys		
 * @param   disguise_code      �ϑ����OBJCODE 
 *                  (DISGUISE_NO_NULL�̏ꍇ�͒ʏ�̎p�ADISGUISE_NO_NORMAL�̏ꍇ�̓p���X���̕W���p)
 * @param   disguise_type      �ϑ���̃^�C�v(TALK_TYPE_xxx)
 * @param   disguise_sex       �ϑ���̐���
 */
//==================================================================
void IntrudeField_PlayerDisguise(INTRUDE_COMM_SYS_PTR intcomm, GAMESYS_WORK *gsys, u32 disguise_code, u8 disguise_type, u8 disguise_sex)
{
  FIELDMAP_WORK *fieldWork;
  FIELD_PLAYER *fld_player;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  int obj_code;
  int sex;
  MYSTATUS *myst;
  
  if(GAMESYSTEM_CheckFieldMapWork( gsys ) == FALSE){
    return;
  }
  
  myst = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));
  sex = MyStatus_GetMySex(myst);
  
  if(intcomm != NULL){
    if(disguise_code == DISGUISE_NO_NORMAL 
        && GAMEDATA_GetIntrudeMyID(gamedata) == intcomm->intrude_status_mine.palace_area){
      disguise_code = DISGUISE_NO_NULL; //�����̃p���X�ɂ���ꍇ��NORMAL��NULL�ɂ���
    }
    
    if(disguise_code == DISGUISE_NO_NULL){
      disguise_type = sex == PM_MALE ? TALK_TYPE_MAN : TALK_TYPE_WOMAN;
      disguise_sex = sex;
    }

    intcomm->intrude_status_mine.disguise_no = disguise_code;
    intcomm->intrude_status_mine.disguise_type = disguise_type;
    intcomm->intrude_status_mine.disguise_sex = disguise_sex;
    intcomm->send_status = TRUE;

    obj_code = Intrude_GetObjCode(&intcomm->intrude_status_mine, myst);
  }
  else{
    disguise_code = DISGUISE_NO_NULL;
    obj_code = sex == PM_MALE ? HERO : HEROINE;
  }

  fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  FIELD_PLAYER_ChangeOBJCode( fld_player, obj_code );
  if(disguise_code == DISGUISE_NO_NULL){
    FIELD_PLAYER_ClearOBJCodeFix( fld_player );
  }
}

//--------------------------------------------------------------
/**
 * �B���A�C�e���~�b�V�������������Ă��鎞�ɖړI�n�ɓ��B�������`�F�b�N
 *
 * @param   gsys		
 * @param   intcomm		
 * @param   pcActor		
 *
 * @retval  MISSION_DATA *		���B�����ꍇ�̓~�b�V�����f�[�^�ւ̃|�C���^
 * @retval  MISSION_DATA *		���B���Ă��Ȃ��ꍇ��NULL
 */
//--------------------------------------------------------------
GMEVENT * IntrudeField_CheckSecretItemEvent(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *pcActor)
{
  if(MISSION_GetMissionEntry(&intcomm->mission) == TRUE
      && MISSION_GetMissionComplete(&intcomm->mission) == FALSE){
    const MISSION_DATA *mdata = MISSION_GetRecvData(&intcomm->mission);
    if(mdata != NULL && mdata->cdata.type == MISSION_TYPE_ITEM 
        && intcomm->intrude_status_mine.palace_area == mdata->target_info.net_id){
      const MISSION_TYPEDATA_ITEM *d_item = (void*)mdata->cdata.data;
      PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gsys );
      ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
    	const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( pcActor );
    	MMDL_GRIDPOS grid_pos;
    	
      MMDL_GetGridPos( fmmdl, &grid_pos );
      if(IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].zone_id == zone_id
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_x == grid_pos.gx
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_y == grid_pos.gy
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_z == grid_pos.gz){
        return EVENT_Intrude_SecretItemArrivalEvent(gsys, intcomm, mdata);
      }
    }
  }
  
  return NULL;
}
