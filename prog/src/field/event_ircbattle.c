//============================================================================================
/**
 * @file  event_ircbattle.c
 * @brief �C�x���g�F�ԊO���o�g��
 * @author  k.ohno
 * @date  2009.03.24
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "gamesystem/btl_setup.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"
#include "./event_ircbattle.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "net_app/irc_match/ircbattlematch.h"
#include "net_app/irc_match/ircbattlemenu.h"
#include "net_app/friend_trade/ircbattlefriend.h"
#include "net_app/pokemontrade.h"
#include "net_app/irc_compatible.h"
#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h"      //GFL_HEAPID_APP�Q��

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/status_rcv.h"
#include "savedata/battle_box_save.h"
#include "field/event_battle_call.h"

//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------


FS_EXTERN_OVERLAY(irc_compatible);

#define HEAPID_CORE GFL_HEAPID_APP

FS_EXTERN_OVERLAY(battle);
FS_EXTERN_OVERLAY(fieldmap);
FS_EXTERN_OVERLAY(ircbattlematch);
FS_EXTERN_OVERLAY(pokemon_trade);
FS_EXTERN_OVERLAY(shinka_demo);
//FS_EXTERN_OVERLAY(app_mail);

#define _LOCALMATCHNO (110)
#define _IRC_BATTEL_LEVEL (50)

enum _EVENT_IRCBATTLE {
  _IRCBATTLE_START,
  _IRCBATTLE_START_FIELD_CLOSE,
  _CALL_IRCBATTLE_MENU,
  _WAIT_IRCBATTLE_MENU,
  _FIELD_FADEOUT,
  _CALL_IRCBATTLE_MATCH,
  _WAIT_IRCBATTLE_MATCH,
  _BATTLE_MATCH_START,
  _TIMING_SYNC_CALL_BATTLE,
  _PLAY_EVENT_BGM,
  _TIMINGBATTLE,
  _CALL_BATTLE,
  _WAIT_BATTLE,
  _CALL_TRADE,
  _WAIT_TRADE,
  _CALL_EVOLUTION,
  _WAIT_EVOLUTION,
 // _CALL_MAIL,
 // _WAIT_MAIL,
  _CALL_NET_END,
  _WAIT_NET_END,
  _FIELD_OPEN,
  _FIELD_FADEIN,
  _FIELD_POP_BGM,
  _FIELD_END,

  _FIELD_FADEOUT_IRCBATTLE,
  _FIELD_END_IRCBATTLE,
  _CALL_IRCCOMMPATIBLE,
  _WAIT_IRCCOMMPATIBLE,
};

//----------------------------------------------------------------------------
/**
 *	@brief  ���x���␳��������
 */
//-----------------------------------------------------------------------------
static void _ModifyLevelPokeParty(  POKEPARTY *party )
{ 
  int i;
  POKEMON_PARAM *pp;
  const u32 modfity_level = _IRC_BATTEL_LEVEL;

  for( i = 0; i < PokeParty_GetPokeCount( party ); i++ )
  {
    pp  = PokeParty_GetMemberPointer( party, i );
    POKETOOL_MakeLevelRevise(pp, modfity_level);
  }
}



static void _PartySet(BOOL bBattleBox,EVENT_IRCBATTLE_WORK *dbw)
{
  BATTLE_BOX_SAVE * bxsv;
  int i;

  bxsv = BATTLE_BOX_SAVE_GetBattleBoxSave(dbw->ctrl);
  if(!bBattleBox){
    POKEPARTY* party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(dbw->gsys));
    POKEMON_PARAM *pp;
    for( i = 0; i < PokeParty_GetPokeCount( party ); i++ ){
      pp  = PokeParty_GetMemberPointer( party, i );
      if(!PP_Get( pp, ID_PARA_tamago_flag, NULL )){
        PokeParty_Add(dbw->pParty, pp);
      }
    }
  }
  else{
    POKEPARTY* party = BATTLE_BOX_SAVE_MakePokeParty( bxsv, HEAPID_PROC );
    PokeParty_Copy(party, dbw->pParty);
    GFL_HEAP_FreeMemory(party);
  }
  _ModifyLevelPokeParty(dbw->pParty);
}

//�p�[�e�B�[���R���ɍ��
static void _partySub3(POKEPARTY* party)
{
  int i,num;

  num = PokeParty_GetPokeCount( party );
  for(i=3; i < num;i++){
    PokeParty_Delete(party, 3);
  }
}


static void _partySub3All(EVENT_IRCBATTLE_WORK *dbw)
{
  int i;
  _partySub3(dbw->pParty);
  for(i = 0;i < 4; i++){
    _partySub3(dbw->pNetParty[i]);
  }
}


static void _battleParaFree(EVENT_IRCBATTLE_WORK *dbw);

//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
static GMEVENT_RESULT EVENT_IrcBattleMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_IRCBATTLE_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;
  FIELDMAP_WORK * pFieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

#if PM_DEBUG
  if(dbw->debugseq != *seq){
    NET_PRINT("e irc %d\n");
    dbw->debugseq = *seq;
  }
#endif

  switch (*seq) {
  case _IRCBATTLE_START:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, pFieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case _IRCBATTLE_START_FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, pFieldmap));
    //
    (*seq) = _CALL_IRCBATTLE_MENU;
    break;
  case _CALL_IRCBATTLE_MENU:
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(fieldmap), &IrcBattleMenuProcData, dbw);
    (*seq)++;
    break;
  case _WAIT_IRCBATTLE_MENU:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }

    if(dbw->selectType == EVENTIRCBTL_ENTRYMODE_COMPATIBLE )
    {
      *seq = _FIELD_FADEOUT_IRCBATTLE;
    }
    else if(dbw->selectType == EVENTIRCBTL_ENTRYMODE_EXIT){
      *seq = _FIELD_OPEN;
    }
    else{
      (*seq) ++;
    }
    break;
  case _FIELD_FADEOUT:
    (*seq)++;
    break;
  case _CALL_IRCBATTLE_MATCH:
    _PartySet( dbw->bBattelBox, dbw );
    dbw->irc_match.gamedata = dbw->gamedata;
    dbw->irc_match.pParty = dbw->pParty;
    dbw->irc_match.pNetParty[0] = dbw->pNetParty[0];
    dbw->irc_match.pNetParty[1] = dbw->pNetParty[1];
    dbw->irc_match.pNetParty[2] = dbw->pNetParty[2];
    dbw->irc_match.pNetParty[3] = dbw->pNetParty[3];
    dbw->irc_match.selectType = dbw->selectType;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(ircbattlematch), &IrcBattleMatchProcData, &dbw->irc_match);
    (*seq)++;
    break;
  case _WAIT_IRCBATTLE_MATCH:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      // �}�b�`���O���e�ɂ��Q�[������
      dbw->selectType = dbw->irc_match.selectType;
      switch(dbw->selectType){
      case EVENTIRCBTL_ENTRYMODE_FRIEND:
        *seq = _WAIT_NET_END;
        break;
      case EVENTIRCBTL_ENTRYMODE_EXIT:
        NetErr_DEBUG_ErrorSet();
        *seq = _WAIT_NET_END;
        break;
      case EVENTIRCBTL_ENTRYMODE_RETRY:
        *seq = _CALL_IRCBATTLE_MENU;
        break;
      case EVENTIRCBTL_ENTRYMODE_TRADE:
        dbw->aPokeTr.ret = POKEMONTRADE_MOVE_START;
        *seq = _CALL_TRADE;
        break;
      case EVENTIRCBTL_ENTRYMODE_MULTH:
/*        
        OS_TPrintf("�}���`�ԍ� %d %d %d %d\n",dbw->irc_match.MultiNo[0],dbw->irc_match.MultiNo[1],
                   dbw->irc_match.MultiNo[2],dbw->irc_match.MultiNo[3]);
        dbw->irc_match.pNetParty[0] = dbw->pNetParty[dbw->irc_match.MultiNo[0]];
        dbw->irc_match.pNetParty[1] = dbw->pNetParty[dbw->irc_match.MultiNo[1]];
        dbw->irc_match.pNetParty[2] = dbw->pNetParty[dbw->irc_match.MultiNo[2]];
        dbw->irc_match.pNetParty[3] = dbw->pNetParty[dbw->irc_match.MultiNo[3]];
   */
        //break; throw
      default:
        *seq = _BATTLE_MATCH_START;
        break;
      }
    }
 
    break;
  case _BATTLE_MATCH_START:
    {
      
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO, WB_NET_IRCBATTLE);
      (*seq) ++;
    }
    break;
  case _TIMING_SYNC_CALL_BATTLE:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_LOCALMATCHNO, WB_NET_IRCBATTLE)){
      (*seq) ++;
    }
    break;
  case _PLAY_EVENT_BGM:

//    _PartySet( dbw->bBattelBox, dbw );

    dbw->para =BATTLE_PARAM_Create(HEAPID_PROC);

    dbw->demo_prm.record = GAMEDATA_GetRecordPtr(dbw->gamedata);
    dbw->demo_prm.fight_count=0;
    switch(dbw->selectType){
    case EVENTIRCBTL_ENTRYMODE_SINGLE:
      dbw->demo_prm.type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      dbw->demo_prm.battle_mode = BATTLE_MODE_COLOSSEUM_SINGLE_FREE;
      BTL_SETUP_Single_Comm( dbw->para , dbw->gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
      break;
    case EVENTIRCBTL_ENTRYMODE_DOUBLE:
      dbw->demo_prm.type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      dbw->demo_prm.battle_mode = BATTLE_MODE_COLOSSEUM_DOUBLE_FREE;
      BTL_SETUP_Double_Comm( dbw->para , dbw->gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
      break;
    case EVENTIRCBTL_ENTRYMODE_TRI:
      dbw->demo_prm.type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      dbw->demo_prm.battle_mode = BATTLE_MODE_COLOSSEUM_TRIPLE_FREE;
      BTL_SETUP_Triple_Comm( dbw->para , dbw->gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
      break;
    case EVENTIRCBTL_ENTRYMODE_ROTATE:
      dbw->demo_prm.type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      dbw->demo_prm.battle_mode = BATTLE_MODE_COLOSSEUM_ROTATION_FREE;
      BTL_SETUP_Rotation_Comm( dbw->para , dbw->gamedata , GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_DS, HEAPID_PROC );
      break;
    case EVENTIRCBTL_ENTRYMODE_MULTH:
      {
        int commpos[]= {0,2,1,3};
      dbw->demo_prm.type = COMM_BTL_DEMO_TYPE_MULTI_START;
      dbw->demo_prm.battle_mode = BATTLE_MODE_COLOSSEUM_MULTI_FREE;
      BTL_SETUP_Multi_Comm( dbw->para , dbw->gamedata , GFL_NET_HANDLE_GetCurrentHandle(),
                            BTL_COMM_DS,  commpos[dbw->irc_match.MultiNo[GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() )]]  , HEAPID_PROC );
      dbw->para->multiMode = 1;
      dbw->para->rule = BTL_RULE_DOUBLE;
        _partySub3All(dbw);
      }
      break;
    default:
      GF_ASSERT(0);
      break;
    }
    STATUS_RCV_PokeParty_RecoverAll(dbw->pParty);  //�S��
    BATTLE_PARAM_SetPokeParty( dbw->para, dbw->pParty, BTL_CLIENT_PLAYER );
    BTL_SETUP_AllocRecBuffer( dbw->para, GFL_HEAPID_APP); //�^��o�b�t�@�m��
//    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayNextBGM( gsys, dbw->para->musicDefault, FSND_FADE_FAST, FSND_FADE_NONE ) );
    dbw->soundNo = PMSND_GetBGMsoundNo();
    PMSND_StopBGM();
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGBATTLE, WB_NET_IRCBATTLE);
    (*seq) ++;
    break;
  case _TIMINGBATTLE:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGBATTLE, WB_NET_IRCBATTLE)==FALSE){
      break;
    }

    if(GFL_NET_IsParentMachine()){
      PMSND_PlayBGM( SEQ_BGM_VS_TRAINER_M );
    }
    else{
      PMSND_PlayBGM( SEQ_BGM_VS_TRAINER_S );
    }
    PMSND_FadeInBGM(PMSND_FADE_NORMAL);
    (*seq) ++;
    break;
    
  case _CALL_BATTLE:
    {
      GMEVENT* next_event;
      EV_BATTLE_CALL_PARAM battlecall_param;
      int i;

      NET_PRINT("�}���`�ԍ� %d %d %d %d\n",dbw->irc_match.MultiNo[0],dbw->irc_match.MultiNo[1], dbw->irc_match.MultiNo[2],dbw->irc_match.MultiNo[3]);
      for( i=0;i<4;i++){
        dbw->demo_prm.trainer_data[i].party = dbw->pNetParty[dbw->irc_match.MultiNo[i]];
        dbw->demo_prm.trainer_data[i].mystatus = GAMEDATA_GetMyStatusPlayer( GAMESYSTEM_GetGameData( gsys ),dbw->irc_match.MultiNo[i] );
      }
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

      battlecall_param.btl_setup_prm = dbw->para;
      battlecall_param.demo_prm = &dbw->demo_prm;

      next_event = GMEVENT_CreateOverlayEventCall( gsys, 
                    FS_OVERLAY_ID(event_battlecall), 
                    EVENT_CallCommBattle, &battlecall_param );

      GMEVENT_CallEvent( event, next_event );
    }
    (*seq)++;
    break;
  case _WAIT_BATTLE:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    BATTLE_PARAM_Delete(dbw->para);
    dbw->para = NULL;
    NET_PRINT("�o�g������\n");
//    GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_FAST, FSND_FADE_NONE));
    PMSND_StopBGM();
    PMSND_PlayBGM( dbw->soundNo );
    PMSND_FadeInBGM(PMSND_FADE_NORMAL);

    (*seq) = _CALL_NET_END;
    break;
  case _CALL_TRADE:  //  �|�P��������
    dbw->aPokeTr.gamedata = dbw->gamedata;
    dbw->aPokeTr.pParty = dbw->pParty;  //�{���̃|�C���^�n��
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(pokemon_trade), &PokemonTradeIrcProcData, &dbw->aPokeTr);
    (*seq)++;
    break;
  case _WAIT_TRADE:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
     // NET_PRINT("�|�P�������������\n");
      if(dbw->aPokeTr.ret == POKEMONTRADE_MOVE_EVOLUTION){
        (*seq) = _CALL_EVOLUTION;
      }
 //     else if(dbw->aPokeTr.ret == POKEMONTRADE_MOVE_MAIL){
   //     (*seq) = _CALL_MAIL;
  //    }
      else{
        (*seq) = _CALL_NET_END;
      }
    }
    break;
  case _CALL_EVOLUTION:
    //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
    //dbw->aPokeTr.shinka_param = SHINKADEMO_AllocParam( HEAPID_PROC, dbw->gamedata,
    //                                           dbw->aPokeTr.pParty,
    //                                           dbw->aPokeTr.after_mons_no,
    //                                           0, dbw->aPokeTr.cond, TRUE );
    {
      SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof( SHINKA_DEMO_PARAM ) );
      sdp->gamedata          = dbw->gamedata;
      sdp->ppt               = dbw->aPokeTr.pParty;
      sdp->after_mons_no     = dbw->aPokeTr.after_mons_no;
      sdp->shinka_pos        = 0;
      sdp->shinka_cond       = dbw->aPokeTr.cond;
      sdp->b_field           = TRUE;
      sdp->b_enable_cancel   = FALSE;
      dbw->aPokeTr.shinka_param  = sdp;
    }
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, dbw->aPokeTr.shinka_param );
    (*seq)++;
    break;
  case _WAIT_EVOLUTION:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      //SHINKADEMO_FreeParam( dbw->aPokeTr.shinka_param );
      {
        SHINKA_DEMO_PARAM* sdp = dbw->aPokeTr.shinka_param;
        GFL_HEAP_FreeMemory( sdp );
      }
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      dbw->aPokeTr.ret = POKEMONTRADE_MOVE_EVOLUTION;
      (*seq)=_CALL_TRADE;
    }
    break;
//  case _CALL_MAIL:
//    dbw->aPokeTr.aMailBox.gamedata = dbw->gamedata;
//    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(app_mail), &MailBoxProcData, &dbw->aPokeTr.aMailBox );
//    (*seq)++;
//    break;
//  case _WAIT_MAIL:
//    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
///      (*seq)=_CALL_TRADE;
 //   }
//    break;
  case _CALL_NET_END:
    if(GFL_NET_IsInit())
    {
      if(GFL_NET_IsParentMachine())
      {
        if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_CMD_EXIT_REQ,0,NULL))
        {
          (*seq) ++;
          break;
        }
        else
        {
          break;
        }
      }
    }
    (*seq) ++;
    break;
  case _WAIT_NET_END:
    if(NET_ERR_CHECK_NONE != NetErr_App_CheckError()){
      NetErr_ErrWorkInit();
    }
    if(GFL_NET_IsExit()){
      (*seq) ++;
    }
    break;
  case _FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case _FIELD_FADEIN:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, pFieldmap, FIELD_FADE_WAIT);
        GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case _FIELD_POP_BGM:
    (*seq) ++;
    break;
  case _FIELD_END:
    _battleParaFree(dbw);
    return GMEVENT_RES_FINISH;

  //�����`�F�b�N�̓v���Z�X�ړ�
  case _FIELD_FADEOUT_IRCBATTLE:
/*    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, pFieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }*/
    (*seq)++;
    break;
  case _FIELD_END_IRCBATTLE:
    (*seq)++;
    break;
  case _CALL_IRCCOMMPATIBLE:  //�����`�F�b�N��ʂ�
    dbw->compatible_param.p_gamesys   = dbw->gsys;
#ifdef DEBUG_IRC_COMPATIBLE_ONLYPLAY
    dbw->compatible_param.is_only_play  = FALSE;
#endif //DEBUG_IRC_COMPATIBLE_ONLYPLAY
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(irc_compatible), &IrcCompatible_ProcData, &dbw->compatible_param );
    (*seq)++;
    break;
  case _WAIT_IRCCOMMPATIBLE:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL)
    {
      GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
      NET_PRINT("�����`�F�b�N��ʂ����\n");
      (*seq)=_CALL_IRCBATTLE_MENU;
    }
    break;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_IrcBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * prevevent,BOOL bCreate)
{
  GMEVENT * event = prevevent;
  EVENT_IRCBATTLE_WORK * dbw;
//  BATTLE_BOX_SAVE * bxsv;

  if(bCreate){
    event = GMEVENT_Create(gsys, NULL, EVENT_IrcBattleMain, sizeof(EVENT_IRCBATTLE_WORK));
  }
  else{
    GMEVENT_Change( event,EVENT_IrcBattleMain, sizeof(EVENT_IRCBATTLE_WORK) );
  }
  dbw = GMEVENT_GetEventWork(event);
  dbw->ctrl =  GAMEDATA_GetSaveControlWork( GAMESYSTEM_GetGameData(gsys) ); 
  dbw->gamedata = GAMESYSTEM_GetGameData(gsys);
  dbw->gsys = gsys;

//  bxsv = BATTLE_BOX_SAVE_GetBattleBoxSave(dbw->ctrl);

  {
    int i;
    for(i=0;i<4;i++){
      dbw->pNetParty[i] = PokeParty_AllocPartyWork( HEAPID_PROC );
    }
    dbw->pParty = PokeParty_AllocPartyWork( HEAPID_PROC );
  }

  return event;
}


static void _battleParaFree(EVENT_IRCBATTLE_WORK *dbw)
{
  
  int i;
  for(i=0;i<4;i++){
    GFL_HEAP_FreeMemory(dbw->pNetParty[i]);
  }
  GFL_HEAP_FreeMemory(dbw->pParty);
}



//--------------------------------------------------------------
/**
 * @brief   �ԊO�����[�h�̐ݒ�
 * @param   pWork      ���[�N�G���A
 * @param   type       �^�C�v
 * @retval  none
 */
//--------------------------------------------------------------
void EVENT_IrcBattleSetType(EVENT_IRCBATTLE_WORK* pWork, int type)
{
  pWork->selectType = type;
}

int EVENT_IrcBattleGetType(EVENT_IRCBATTLE_WORK* pWork)
{
  return pWork->selectType;
}


//--------------------------------------------------------------
/**
 * @brief   EVENT_IRCBATTLE_WORK����GAMESYS_WORK�𓾂�
 * @param   pWork      EVENT_IRCBATTLE_WORK
 * @retval  GAMESYS_WORK
 */
//--------------------------------------------------------------
GAMESYS_WORK* IrcBattle_GetGAMESYS_WORK(EVENT_IRCBATTLE_WORK* pWork)
{
  return pWork->gsys;
}

//--------------------------------------------------------------
/**
 * @brief   EVENT_IRCBATTLE_WORK����SAVE_CONTROL_WORK������
 * @param   pWork      EVENT_IRCBATTLE_WORK
 * @retval  SAVE_CONTROL_WORK
 */
//--------------------------------------------------------------
SAVE_CONTROL_WORK* IrcBattle_GetSAVE_CONTROL_WORK(EVENT_IRCBATTLE_WORK* pWork)
{
  return pWork->ctrl;
}

