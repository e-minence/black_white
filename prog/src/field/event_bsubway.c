//======================================================================
/**
 * @file  event_bsubway.c
 * @brief  �o�g���T�u�E�F�C�@�C�x���g
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem\btl_setup.h"
#include "event_fieldmap_control.h"
#include "event_fldmmdl_control.h"
#include "poke_tool/poke_regulation.h"
#include "app/pokelist.h"
#include "app/p_status.h"
#include "app/leader_board.h"
#include "battle/battle.h"

#include "event_battle.h"
#include "field/event_battle_call.h"
#include "event_fieldmap_control.h"
#include "event_ircbattle.h"

#include "fieldmap.h"

#include "field/field_msgbg.h"
#include "message.naix"

#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "scrcmd_bsubway.h"
#include "bsubway_tr.h"

#include "event_bsubway.h"

FS_EXTERN_OVERLAY(pokelist);

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// EVENT_BSW_COMM_BATTLE_WORK
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  BATTLE_SETUP_PARAM *para;
  COMM_BTL_DEMO_PARAM *demo_prm;
}EVENT_BSW_COMM_BATTLE_WORK;

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �o�g���T�u�E�F�C�@�C�x���g�@�|�P�������X�g
//======================================================================
//--------------------------------------------------------------
/// �|�P�������X�g�I���C�x���g
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldmap;
  
  PSTATUS_DATA status_data;
  PLIST_DATA list_data;
  
  u16 *return_mode;
  u16 *result_select;
  u8 *result_num;
  
  POKEPARTY *pp_btl_box;
}EVENT_WORK_POKE_LIST;

//--------------------------------------------------------------
/**
 * �|�P�������X�g�I���C�x���g
 * @param event GMEVENT
 * @param seq event sequence
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ev_SelectPokeList( GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK_POKE_LIST *work = wk;
  GAMESYS_WORK *gsys = work->gsys;
  
  switch( *seq )
  {
  case 0:
		GMEVENT_CallEvent( event,
        EVENT_FieldFadeOut_Black(gsys,work->fieldmap,FIELD_FADE_WAIT) );
    (*seq)++;
    break;
  case 1:
		GMEVENT_CallEvent( event, EVENT_FieldClose(gsys,work->fieldmap) );
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent( event, EVENT_PokeSelect(
          gsys,work->fieldmap,&work->list_data,&work->status_data) );
    (*seq)++;
    break;
  case 3:
		GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
    (*seq)++;
    break;
  case 4:
    GMEVENT_CallEvent( event, EVENT_FieldFadeIn_Black(
          gsys,work->fieldmap, FIELD_FADE_WAIT) );
    MI_CpuCopy8( work->list_data.in_num, work->result_num, 6 );
    *work->result_select = work->list_data.ret_sel;
    *work->return_mode = work->list_data.ret_mode;
    GFL_HEAP_FreeMemory( work->list_data.reg );
    (*seq)++;
    break;
  case 5:
    return( GMEVENT_RES_FINISH );
  }
  
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �|�P�������X�g�I���C�x���g�@�Ăяo��
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_SetSelectPokeList(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, BOOL btl_box )
{
  GMEVENT *event;
  GAMEDATA *gdata;
  POKEPARTY *pp;
  FIELDMAP_WORK *fieldmap;
  EVENT_WORK_POKE_LIST *work;
  
  gdata = GAMESYSTEM_GetGameData( gsys );
  
  fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  event = GMEVENT_Create( gsys, NULL,
      ev_SelectPokeList, sizeof(EVENT_WORK_POKE_LIST) );
  
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldmap = fieldmap;
  work->result_num = bsw_scr->pokelist_select_num;
  work->result_select = &bsw_scr->pokelist_result_select;
  work->return_mode = &bsw_scr->pokelist_return_mode;
  
  if( btl_box == FALSE ){
    pp = GAMEDATA_GetMyPokemon( gdata );
  }else{
    pp = bsw_scr->btl_box_party;
  }
  
  GF_ASSERT( pp != NULL );

  {
    int reg = REG_SUBWAY_SINGLE;
    int type = PL_TYPE_SINGLE;
    PLIST_DATA *list = &work->list_data;
    
    MI_CpuClear8( list, sizeof(PLIST_DATA) );
    list->pp = pp;
    
    switch( bsw_scr->play_mode ){
    case BSWAY_MODE_DOUBLE:
    case BSWAY_MODE_S_DOUBLE:
      reg = REG_SUBWAY_DOUBLE;
      type = PL_TYPE_DOUBLE;
      break;
    case BSWAY_MODE_MULTI:
    case BSWAY_MODE_S_MULTI:
    case BSWAY_MODE_COMM_MULTI:
    case BSWAY_MODE_S_COMM_MULTI:
      reg = REG_SUBWAY_MALTI;
      type = PL_TYPE_MULTI;
      break;
    }
    
    KAGAYA_Printf( "���M�����[�V�����@%d\n", reg );
    
    list->reg = (void*)PokeRegulation_LoadDataAlloc( reg, HEAPID_PROC );
    list->mode = PL_MODE_BATTLE_SUBWAY;
    list->type = type;
    
    //debug
    #ifdef DEBUG_ONLY_FOR_kagaya
    {
      REGULATION *pReg = list->reg;
      
      KAGAYA_Printf( "BSWAY REG NUM_LO = %d, HI =%d, LV = %d\n",
        pReg->NUM_LO, pReg->NUM_HI, pReg->LEVEL );
    }
    #endif
  }
  
  {
    PSTATUS_DATA *st = &work->status_data;
    ZUKAN_SAVEDATA *zukanSave = GAMEDATA_GetZukanSave( gdata );
    MI_CpuClear8( st, sizeof(PSTATUS_DATA) );
    st->ppd = pp;
    st->ppt = PST_PP_TYPE_POKEPARTY;
    st->max = PokeParty_GetPokeCount( pp );
    st->mode = PST_MODE_NORMAL;
    st->page = PPT_INFO;

    st->zukan_mode = ZUKANSAVE_GetZenkokuZukanFlag( zukanSave );
  }
   
  return( event );
}

//======================================================================
//  �o�g���T�u�E�F�C�@�퓬
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�g���[�i�[�o�g��
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_TrainerBattle(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT * event;
  BATTLE_SETUP_PARAM *bp;
  
  //�ΐ�^��f�[�^���쐬����
  BattleRec_Init( HEAPID_PROC );
  
  //�o�g���p�����쐬
  bsw_scr->btl_setup_param =
    BSUBWAY_SCRWORK_CreateBattleParam( bsw_scr, gsys );
  
  if( bsw_scr->play_mode == BSWAY_MODE_COMM_MULTI ||
      bsw_scr->play_mode == BSWAY_MODE_S_COMM_MULTI ){
    return BSUBWAY_EVENT_CommBattle( bsw_scr, gsys, fieldmap );
  }else{
    event = EVENT_BSubwayTrainerBattle(
        gsys, fieldmap, bsw_scr->btl_setup_param );
  }
  
  return( event );
}

//======================================================================
//  �o�g���T�u�E�F�C�@�C�x���g�@�ΐ�O���b�Z�[�W
//======================================================================
//--------------------------------------------------------------
/// �ΐ�O���b�Z�[�W
//--------------------------------------------------------------
typedef struct
{
  GAMESYS_WORK *gsys;
  BSUBWAY_SCRWORK *bsw_scr;
  
  FLDSYSWIN_STREAM *sysWin;
  STRBUF *strBuf;
}EVENT_WORK_TRAINER_BEFORE_MSG;

//--------------------------------------------------------------
/**
 * �ΐ�O���b�Z�[�W
 * @param event GMEVENT
 * @param seq event sequence
 * @param wk event work
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ev_TrainerMsgBefore(
    GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK_TRAINER_BEFORE_MSG *work = wk;
  GAMESYS_WORK *gsys = work->gsys;
  
  switch( *seq )
  {
  case 0:
    FLDSYSWIN_STREAM_PrintStrBufStart( work->sysWin, 0, 0, work->strBuf );
    (*seq)++;
    break;
  case 1:
    if( FLDSYSWIN_STREAM_Print(work->sysWin) == TRUE ){
      (*seq)++;
    }
    break;
  case 2:
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      FLDSYSWIN_STREAM_Delete( work->sysWin );
      GFL_STR_DeleteBuffer( work->strBuf );
      return( GMEVENT_RES_FINISH );
    }
    break;
  }
  return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �ΐ�O���b�Z�[�W �Ăяo��
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_TrainerBeforeMsg(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, int tr_idx )
{
  GMEVENT *event;
  GAMEDATA *gdata;
  FLDMSGBG *msgbg;
  FIELDMAP_WORK *fieldmap;
  EVENT_WORK_TRAINER_BEFORE_MSG *work;
  
  gdata = GAMESYSTEM_GetGameData( gsys );
  fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  msgbg = FIELDMAP_GetFldMsgBG( fieldmap );
  
  event = GMEVENT_Create( gsys, NULL,
    ev_TrainerMsgBefore, sizeof(EVENT_WORK_POKE_LIST) );
  
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->strBuf = GFL_STR_CreateBuffer( 92, HEAPID_PROC );
  
  if( bsw_scr->tr_data[tr_idx].bt_trd.appear_word[0] == 0xffff ){ //ROM MSG
    GFL_MSGDATA *msgdata;
    
    msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_tower_trainer_dat, HEAPID_PROC );
    
    GFL_MSG_GetString(
        msgdata, 
        bsw_scr->tr_data[tr_idx].bt_trd.appear_word[1],
        work->strBuf );
    
    GFL_MSG_Delete( msgdata );
  }else{ //�ȈՉ�b kari
    GFL_MSGDATA *msgdata;
    
    msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_tower_trainer_dat, HEAPID_PROC );
    
    GFL_MSG_GetString(
        msgdata,
        0,
        work->strBuf );
    
    GFL_MSG_Delete( msgdata );
  }
  
  work->sysWin = FLDSYSWIN_STREAM_Add( msgbg, NULL, 19 );
  return( event );
}

//======================================================================
//  ��ナ�[�_�[���̕\��
//======================================================================
//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef struct {
  FIELDMAP_WORK* fieldmap;
  GAMESYS_WORK* gsys;
  LEADERBOARD_PARAM param;
}EVENT_WORK_CALL_LEADERBOARD;

static GMEVENT_RESULT ev_CallLeaderBoard(
    GMEVENT *event, int *seq, void *wk )
{
  EVENT_WORK_CALL_LEADERBOARD* work = wk;
  
  switch( *seq ){
  case 0:
    {
      GMEVENT* ev;
      BSUBWAY_WIFI_DATA* bsw_wifi;

      work->param.gamedata = GAMESYSTEM_GetGameData( work->gsys );

      

#if 0
      bsw_wifi = GAMEDATA_GetBSubwayWifiData( work->param.gamedata );
      work->param.rank_no   = BSUBWAY_WIFIDATA_GetLeaderRank();
      work->param.train_no  = BSUBWAY_WIFIDATA_GetLeaderRoomNo();
#else
      // @TODO ���ł�
      work->param.rank_no   = 1;
      work->param.train_no  = 1;
#endif
      ev = EVENT_FieldSubProc( work->gsys, work->fieldmap,
		          FS_OVERLAY_ID(leader_board), &LeaderBoardProcData, &work->param);
  		GMEVENT_CallEvent( event, ev );

      (*seq) ++;
    }
    break;
  case 1:
    // �I��
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��ナ�[�_�[�{�[�h�Ăяo��
 */
//-----------------------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_CallLeaderBoard( GAMESYS_WORK *gsys )
{
  GMEVENT * event;
  EVENT_WORK_CALL_LEADERBOARD* work;
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );


  event = GMEVENT_Create( gsys, NULL,
    ev_CallLeaderBoard, sizeof(EVENT_WORK_CALL_LEADERBOARD) );
  
  work = GMEVENT_GetEventWork( event );
  work->gsys      = gsys;
  work->fieldmap  = fieldmap;

  return event;
}

//======================================================================
//  �ʐM�o�g��
//======================================================================
#include "net/network_define.h"

static GFL_PROC_RESULT CommBattleCallProc_Init(
    GFL_PROC *proc, int *seq, void* pwk, void* mywk );
static GFL_PROC_RESULT CommBattleCallProc_Main(
    GFL_PROC *proc, int *seq, void* pwk, void* mywk );
static GFL_PROC_RESULT CommBattleCallProc_End(
    GFL_PROC *proc, int *seq, void* pwk, void* mywk );

const GFL_PROC_DATA bsw_CommBattleCommProcData = 
{ 
  CommBattleCallProc_Init,
  CommBattleCallProc_Main,
  CommBattleCallProc_End,
};

typedef struct{
  GFL_PROCSYS* procsys_up; 
}COMM_BTL_DEMO_PROC_WORK;

typedef struct{
  // [IN]
  GAMESYS_WORK * gsys;  
  BATTLE_SETUP_PARAM  *btl_setup_prm;
  COMM_BTL_DEMO_PARAM *demo_prm;
  // [PRIVATE]
  COMM_BATTLE_CALL_PROC_PARAM cbc;
}BSW_EVENT_BATTLE_CALL_WORK;

#define BATTLE_CALL_HEAP_SIZE (0x4000) // PROC���p�q�[�v�̃T�C�Y
#define BATTLE_ADD_CMD_TBL_TIMING 200 ///< ����NO

// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];

FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------
/**
 *	@brief  �ʐM�o�g���Ăяo��PROC ����������
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *	@retval
 */
//-------------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_Init(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  COMM_BTL_DEMO_PROC_WORK*   work;

  // �q�[�v����
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_CALL, BATTLE_CALL_HEAP_SIZE );

  // ���[�N �A���P�[�g
  work = GFL_PROC_AllocWork( proc , sizeof(COMM_BTL_DEMO_PROC_WORK) , HEAPID_BATTLE_CALL );

  work->procsys_up = GFL_PROC_LOCAL_boot( HEAPID_BATTLE_CALL );
  
  return GFL_PROC_RES_FINISH;
}

//---------------------------------------------------
/**
 *	@brief  �ʐM�o�g���Ăяo��PROC �I������
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//--------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_End(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  COMM_BTL_DEMO_PROC_WORK*   work = mywk;

  GFL_PROC_LOCAL_Exit( work->procsys_up );
  GFL_PROC_FreeWork( proc );
  
  // �q�[�v�J��
  GFL_HEAP_DeleteHeap( HEAPID_BATTLE_CALL );
  return GFL_PROC_RES_FINISH;
}

//-------------------------------------------------
/**
 *	@brief  �ʐM�o�g���Ăяo��PROC �又��
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_Main(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  enum
  {
    SEQ_BATTLE_TIMING_INIT,
    SEQ_BATTLE_TIMING_WAIT,
    SEQ_BATTLE_INIT,
    SEQ_BATTLE_WAIT,
    SEQ_BATTLE_END,
    SEQ_END
  };
  
  COMM_BTL_DEMO_PROC_WORK *work = mywk;
  COMM_BATTLE_CALL_PROC_PARAM * bcw = pwk;
  GFL_PROC_MAIN_STATUS up_status;
  
  GF_ASSERT(work);
  GF_ASSERT(bcw);
  
  up_status = GFL_PROC_LOCAL_Main( work->procsys_up );
  
  switch (*seq) {
  case SEQ_BATTLE_TIMING_INIT:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(
          GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL );
      GFL_NET_TimingSyncStart(
          GFL_NET_HANDLE_GetCurrentHandle(), BATTLE_ADD_CMD_TBL_TIMING);
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add�����̂œ������\n");
      (*seq) = SEQ_BATTLE_TIMING_WAIT;
    }
    break;
  case SEQ_BATTLE_TIMING_WAIT:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), BATTLE_ADD_CMD_TBL_TIMING)){
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add��̓�����芮��\n");
      (*seq) = SEQ_BATTLE_INIT;
    }
    break;
  case SEQ_BATTLE_INIT:
    GFL_PROC_LOCAL_CallProc( work->procsys_up, NO_OVERLAY_ID, &BtlProcData, bcw->btl_setup_prm);
    (*seq) = SEQ_BATTLE_WAIT;
    break;
  case SEQ_BATTLE_WAIT:
    if ( up_status != GFL_PROC_MAIN_VALID ){
      (*seq) = SEQ_BATTLE_END;
    }
    break;
  case SEQ_BATTLE_END:
    OS_TPrintf("�o�g������\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
#if 0    
    BattleRec_LoadToolModule();                       // �^��
    BattleRec_StoreSetupParam( bcw->btl_setup_prm );  // �^��
    BattleRec_UnloadToolModule();                     // �^��
#endif
    (*seq) = SEQ_END;
    break;
  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------
/**
 *	@brief  �C�x���g�F�ʐM�o�g���Ăяo��
 *	@param	* event
 *	@param	*  seq
 *	@param	* work
 *	@retval
 */
//----------------------------------------------------
static GMEVENT_RESULT bsw_EventCommBattleMain(
    GMEVENT * event, int *  seq, void * work)
{
  BSW_EVENT_BATTLE_CALL_WORK *bcw = work;
  GAMESYS_WORK * gsys = bcw->gsys;
  
  switch (*seq) 
  {
  case 0:
    {
      COMM_BATTLE_CALL_PROC_PARAM *prm = &bcw->cbc;
      prm->gdata = GAMESYSTEM_GetGameData( gsys );
      prm->btl_setup_prm = bcw->btl_setup_prm;
      prm->demo_prm = bcw->demo_prm;
      GAMESYSTEM_CallProc( gsys, NO_OVERLAY_ID,
          &bsw_CommBattleCommProcData, prm);
    }
    (*seq)++;
    break;

  case 1:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}


//==================================================================
/**
 * �C�x���g�쐬�F�ʐM�o�g���Ăяo��
 *
 * @param   gsys		
 * @param   para		
 * @param   demo_prm		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
static GMEVENT * bsw_CommBattle(GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM *btl_setup_prm, COMM_BTL_DEMO_PARAM *demo_prm)
{
  BSW_EVENT_BATTLE_CALL_WORK *bcw;
  GMEVENT * event;

  event = GMEVENT_Create(
      gsys, NULL, bsw_EventCommBattleMain,
      sizeof(BSW_EVENT_BATTLE_CALL_WORK) );
  bcw = GMEVENT_GetEventWork(event);
  bcw->gsys = gsys;
  bcw->btl_setup_prm = btl_setup_prm;
  bcw->demo_prm = demo_prm;
  return event;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�ʐM�o�g���@���C��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT bsw_CommBattleMain( GMEVENT *event, int *seq, void *wk )
{
  EVENT_BSW_COMM_BATTLE_WORK *work = wk;
  GAMESYS_WORK *gsys = work->gsys;
  
  switch (*seq){
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, work->fieldmap) );
    (*seq)++;
    break;
  case 1:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(
          gsys, work->fieldmap, FIELD_FADE_WAIT );
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent( event, EVENT_FieldClose(gsys,work->fieldmap) );
    (*seq)++;
    break;
  case 3:
    GMEVENT_CallEvent( event, 
      EVENT_FSND_PushPlayNextBGM(gsys,work->para->musicDefault,
        FSND_FADE_FAST, FSND_FADE_NONE) ); 
    (*seq)++;
    break;
  case 4: //battle main
    GMEVENT_CallEvent( event,
        bsw_CommBattle(gsys,work->para,work->demo_prm) );
    (*seq)++;
    break;
  case 5:
    #if 0
    BATTLE_PARAM_Delete( work->para ); //�o�g��SetupParam������܂���B
    #endif
    
    OS_TPrintf("_FIELD_OPEN\n");
    GMEVENT_CallEvent( event, EVENT_FieldOpen(gsys) );
    (*seq) ++;
    break;
  case 6:
    OS_TPrintf("_FIELD_FADEIN\n");
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(
          gsys, work->fieldmap, FIELD_FADE_WAIT );
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 7:
    GMEVENT_CallEvent( event,
        EVENT_FSND_PopBGM(gsys, FSND_FADE_FAST,FSND_FADE_NORMAL) );
    (*seq) ++;
    break;
  case 8:
    return GMEVENT_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//==================================================================
/**
 * �C�x���g�쐬�F�ʐM�o�g���Ăяo��
 *
 * @param   gsys		
 * @param   para		
 * @param   demo_prm		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
static GMEVENT * ev_CommBattle(
    GAMESYS_WORK * gsys,
    FIELDMAP_WORK *fieldmap,
    BATTLE_SETUP_PARAM *btl_setup_prm,
    COMM_BTL_DEMO_PARAM *demo_prm)
{
  EVENT_BSW_COMM_BATTLE_WORK *bcw;
  GMEVENT * event;

  event = GMEVENT_Create(
      gsys, NULL, bsw_CommBattleMain, sizeof(EVENT_BSW_COMM_BATTLE_WORK) );
  bcw = GMEVENT_GetEventWork(event);
  bcw->gsys = gsys;
  bcw->para = btl_setup_prm;
  bcw->demo_prm = demo_prm;
  bcw->fieldmap = fieldmap;
  return event;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�@�ʐM�o�g��
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_CommBattle(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT * event;
  BATTLE_SETUP_PARAM *bp;
  COMM_BTL_DEMO_PARAM *demo;
  
  demo = BSUBWAY_SCRWORK_CreateBattleDemoParam( bsw_scr, gsys );
  event = ev_CommBattle( gsys, fieldmap, bsw_scr->btl_setup_param, demo );
  return( event );
}

//======================================================================
//  �o�g���T�u�E�F�C wifi
//======================================================================
#if 0
///Wifi�C�x���g���[�N
typedef struct _BTWR_WIFI_EVENT
{
	int	ret_val;
	int	seq;
	WBTOWER_PARAM*	app_wk;
	u16**			scr_ret_wk;
	u16				scr_ret_wkno;
	u16	mode;
	u16	dpw_code;
}BTWR_WIFI_EVENT;

typedef enum
{
	BTWR_WIFI_CONNECT_CALL,
	BTWR_WIFI_CONNECT_WAIT,
	BTWR_WIFI_EV_EXIT,
};

//--------------------------------------------------------------
/// EVENT_WORK_WIFI_CONNECT
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  u16 mode;
  u16 ret_wk_no;
  u16 dpw_code;

  GAMESYS_WORK *gsys;
  BSUBWAY_SCRWORK *bsw_scr;
}EVENT_WORK_WIFI_CONNECT;

/**
 *	@brief	�o�g���^���[�@WiFi�R�l�N�g�Ăяo��
 */
static int BtlTower_WifiConnectCall(
    BTWR_WIFI_EVENT* wk, FIELDSYS_WORK* fsys )
{
	SAVEDATA* sv;
  
	if( mydwc_checkMyGSID() ){
		wk->app_wk = Field_WifiBattleTowerConnect_SetProc(
        fsys,wk->mode,wk->dpw_code);
		return BTWR_WIFI_CONNECT_WAIT;
	}else{
		wk->ret_val = WIFI_BTOWER_RESULT_CANCEL;
		return BTWR_WIFI_EV_EXIT;
	}
}

/**
 *	@brief	�o�g���^���[�@WiFi�R�l�N�g�I���҂�
 */
static int BtlTower_WifiConnectWait(BTWR_WIFI_EVENT* wk,FIELDSYS_WORK* fsys)
{
	u8	i;
	WBTOWER_PARAM	*param;

	// �T�u�v���Z�X�I���҂�
	if( FieldEvent_Cmd_WaitSubProcEnd(fsys) ) {
		return BTWR_WIFI_CONNECT_WAIT;
	}
//	param = (WBTOWER_PARAM*)wk->app_wk;

	//�X�e�[�^�X���擾
	wk->ret_val = wk->app_wk->result;//param->result;
	OS_Printf("wifi_ret_val = %d,%d\n",wk->app_wk->result,wk->ret_val);
	//���������
	sys_FreeMemoryEz(wk->app_wk);
	return BTWR_WIFI_EV_EXIT;
}

//--------------------------------------------------------------
/**
 * @brief	�Q�[���C�x���g�R���g���[���@WiFi�ڑ�
 * @param	ev	GMEVENT_CONTROL *
 * @retval	BOOL	TRUE=�C�x���g�I��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT BtlTowerEv_WifiConnect(
    GMEVENT *event, int *seq, void *work )
{
	u16	*buf16;
	BTWR_WIFI_EVENT *wk = work;
  
	switch( (*seq) ){
	case BTWR_WIFI_CONNECT_CALL:
		(*seq) = BtlTower_WifiConnectCall(wk,fsys);
		break;
	case BTWR_WIFI_CONNECT_WAIT:
		(*seq) = BtlTower_WifiConnectWait(wk,fsys);
		break;
	case BTWR_WIFI_EV_EXIT:
		//�߂�l�w��
		buf16 = GetEventWorkAdrs(fsys,wk->scr_ret_wkno);//*(wk->scr_ret_wk);
		OS_Printf("wifi_ret_val = %d\n",wk->ret_val);
		*buf16 = wk->ret_val;
	  return GMEVENT_RES_FINISH;
	}
  
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief	WiFi�ڑ��@�T�u�C�x���g�Ăяo��
 * @param	event	GMEVENT_CONTROL*
 * @param	mode	�ڑ����[�h
 * @param	ret_wk_no	�Ԃ�l��Ԃ����[�NID
 * @param	dpw_code	DPW_INIT_PROC���Ԃ����Ԃ�l
 * @retval  nothing
 */
//--------------------------------------------------------------
void EventCmd_BTowerWifiCall(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap,
    GMEVENT *m_event, u16 mode, u16 ret_wk_no, u16 dpw_code )
{
  GMEVENT *event;
	BTWR_WIFI_EVENT *wk;
  
  event = GMEVENT_Create( gsys, NULL,
      BtlTowerEv_WifiConnect, sizeof(BTWR_WIFI_EVENT) );

  wk = GMEVENT_GetEventWork( event );
	wk->mode = mode;
	wk->dpw_code = dpw_code;
	wk->scr_ret_wkno = ret_wk_no;
}
#endif

#if 0
//==============================================================================
/**
 * $brief   WIFI�o�g���^���[�ڑ���ʂ��Ăяo��
 *
 * @param   fsys		
 *
 * @retval  none		
 */
//==============================================================================
static void * Field_WifiBattleTowerConnect_SetProc( FIELDSYS_WORK *fsys, int mode, int connect )
{
	WBTOWER_PARAM *param;
	FS_EXTERN_OVERLAY(dpw_tower);
  
  WIFILOGIN_PARAM *p_param;
  
  work->p_sub_wk = GFL_HEAP_AllocClearMemory(
      HEAPID_PROC, sizeof(WIFILOGIN_PARAM) );
  
  p_param = work->p_sub_wk;
  p_param->gamedata = GAMESYSTEM_GetGameData( gsys );
  
  GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(wifi_login),
      &WiFiLogin_ProcData, work->p_sub_wk );
  
  (*seq)++;
  break;
  
  if( GAMESYSTEM_IsProcExist(gsys) == GFL_PROC_MAIN_NULL ){
    WIFILOGIN_PARAM *p_param = work->p_sub_wk;
    
    if( p_param->result == WIFILOGIN_RESULT_LOGIN ){ 
      (*seq)++;
    }else if( p_param->result == WIFILOGIN_RESULT_CANCEL )
      { 
        (*seq)  = _WAIT_NET_END;
      }
      GFL_HEAP_FreeMemory( work->p_sub_wk );
    }
    
    p_param = work->

	// �v���Z�X��`�f�[�^
	const PROC_DATA WBTowerProcData = {
		WBTowerProc_Init,
		WBTowerProc_Main,
		WBTowerProc_End,
		FS_OVERLAY_ID(dpw_tower),
	};
	

	param = sys_AllocMemoryLo(HEAPID_WORLD, sizeof(WBTOWER_PARAM));
  
	param->btowerscore   = SaveData_GetTowerScoreData(fsys->savedata);
	param->wifitowerdata = SaveData_GetTowerWifiData(fsys->savedata);
	param->systemdata    = SaveData_GetSystemData(fsys->savedata);
	param->config        = SaveData_GetConfig(fsys->savedata);
	param->MyUserData    = WifiList_GetMyUserInfo( SaveData_GetWifiListData(fsys->savedata) );
	param->savedata      = fsys->savedata;
	param->profileId     = mydwc_getMyGSID(SaveData_GetWifiListData(fsys->savedata));
	param->mode          = mode;	// btower.h�Q��
	param->connect       = connect;

#if 1
	//�v���`�iBTS �ʐM 090
	//�����Fresult�̏��������s���Ă��Ȃ�
	//
	//DP�̎��́Aresult�ɓK���Ȓl�������Ă����̂ŁA
	//�X�N���v�g��WIFI_BTOWER_RESULT_SUCCESS�̕���Ɉ���������Ȃ������B
	//
	//WBTOWER_PARAM��0�N���A����ƁA�������邩�킩��Ȃ��̂ŁAresult�̂݃L�����Z������
	param->result		= WIFI_BTOWER_RESULT_CANCEL;
#endif

	GameSystem_StartSubProc(fsys,  &WBTowerProcData, param );
	return (void*)param;
} 

//--------------------------------------------------------------
/**
 * @brief	�o�g���^���[�@WiFi�R�l�N�g�Ăяo��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static int call_WifiConnect( EVENT_WORK_WIFI_CONNECT *wk )
{
	if( mydwc_checkMyGSID() == TRUE ){
    /*
		wk->app_wk = Field_WifiBattleTowerConnect_SetProc(
        fsys,wk->mode,wk->dpw_code);
		return BTWR_WIFI_CONNECT_WAIT;
    */
	}
  
  wk->ret_val = WIFI_BTOWER_RESULT_CANCEL;
	return BTWR_WIFI_EV_EXIT;
}

//--------------------------------------------------------------
/**
 * @brief	�Q�[���C�x���g�R���g���[���@WiFi�ڑ�
 * @param	ev	GMEVENT_CONTROL *
 * @retval	BOOL	TRUE=�C�x���g�I��
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ev_WifiConnect( GMEVENT *ev, int *seq, void *wk )
{
	u16	*buf16;
  EVENT_WORK_WIFI_CONNECT *work = wk;
  
	switch( (*seq) ){
  case 0:
    if( mydwc_checkMyGSID() == FALSE ){ //error
      work->ret_val = WIFI_BTOWER_RESULT_CANCEL;
      (*seq) = exit;
      break;
    }
    //FALL THROUGH	
  case 1:
    if( GAME_COMM_NO_NULL == GameCommSys_BootCheck(
          GAMESYSTEM_GetGameCommSysPtr(gsys)) ){
      work->fieldmap = NULL; //�N���[�Y������g�p�֎~�I
      (*seq)++;
    }
    break;
  case 2:
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(wifi_login),
        &WiFiLogin_ProcData, &dbw->login );
    (*seq)++;
    break;
  case 3:
    if( GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL ){
      if( work->login.result == WIFILOGIN_RESULT_LOGIN ){ 
        (*seq)++;
      }else{ 
        (*seq) = _WAIT_NET_END;
      }
    }
    break;
  case 4:
    //�^���[�f�[�^������
    (*seq)++;
    break;
  case 5:
    //�^���[�f�[�^�����I���҂�
    if( GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL ){
      (*seq)++;
      dbw->aPokeTr.ret = POKEMONTRADE_MOVE_START;
    }
    break;
	case 6:
		//�߂�l�w��
		buf16 = GetEventWorkAdrs(fsys,wk->scr_ret_wkno);//*(wk->scr_ret_wk);
		OS_Printf("wifi_ret_val = %d\n",wk->ret_val);
		*buf16 = wk->ret_val;
		sys_FreeMemoryEz(wk);
		return TRUE;
	}
	return FALSE;
}


//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
GMEVENT * BSUBWAY_EVENT_SetWifiConnect(
    BSUWBAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys,
    u16 mode, u16 ret_wk_no, u16 dpw_code )
{
  GMEVENT *event;
  EVENT_WORK_WIFI_CONNECT *work;
  
  event = GMEVENT_Create( gsys, NULL,
      NULL, sizeof(EVENT_WORK_WIFI_CONNECT) );
  
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->bsw_scr = bsw_scr;
  work->mode = mode;
  work->ret_wk_no = ret_wk_no;
  work->dpw_code = dpw_code;
}
#endif



