//=============================================================================
/**
 *
 *	@file		event_battle_call.c
 *	@brief  �C�x���g�F�o�g���Ăяo��
 *	@author	hosaka genya
 *	@data		2010.01.22
 *
 */
//=============================================================================
#include <gflib.h>
#include "net/network_define.h"

#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"

#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "net_app/union/union_types.h"
#include "net_app/union/colosseum_types.h"
#include "field/event_fldmmdl_control.h"
#include "field/event_colosseum_battle.h"

#include "gamesystem/btl_setup.h"

#include "field/event_battle_call.h"


static GFL_PROC_RESULT CommBattleCallProc_Init(  GFL_PROC *proc, int *seq, void* pwk, void* mywk );
static GFL_PROC_RESULT CommBattleCallProc_Main(  GFL_PROC *proc, int *seq, void* pwk, void* mywk );
static GFL_PROC_RESULT CommBattleCallProc_End(  GFL_PROC *proc, int *seq, void* pwk, void* mywk );

const GFL_PROC_DATA CommBattleCommProcData = 
{ 
  CommBattleCallProc_Init,
  CommBattleCallProc_Main,
  CommBattleCallProc_End,
};

//==============================================================================
//  PROC�\����
//==============================================================================
typedef struct{
  GFL_PROCSYS* procsys_up; 
}COMM_BTL_DEMO_PROC_WORK;

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_Init(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  COMM_BTL_DEMO_PROC_WORK*   work;
  
  // ���[�N �A���P�[�g
  work = GFL_PROC_AllocWork( proc , sizeof(COMM_BTL_DEMO_PROC_WORK) , GFL_HEAPID_APP );

  work->procsys_up = GFL_PROC_LOCAL_boot( GFL_HEAPID_APP );
  
  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_End(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  COMM_BTL_DEMO_PROC_WORK*   work;

  GFL_PROC_LOCAL_Exit( work->procsys_up );
  
  GFL_PROC_FreeWork( proc );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �ʐM�o�g���Ăяo��PROC
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc_Main(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  enum
  {
    SEQ_CALL_START_DEMO = 0,  ///< �o�g���O�f���Ăяo��
    SEQ_WAIT_START_DEMO,
    SEQ_BATTLE_TIMING_INIT,
    SEQ_BATTLE_TIMING_WAIT,
    SEQ_BATTLE_INIT,
    SEQ_BATTLE_WAIT,
    SEQ_BATTLE_END,
    SEQ_CALL_END_DEMO,        ///< �o�g����f���Ăяo��
    SEQ_CALL_BTL_REC_SEL,     ///< �ʐM�ΐ��̘^��I�����
    SEQ_BGM_POP,
    SEQ_END
  };

  COMM_BTL_DEMO_PROC_WORK * work = mywk;
  EVENT_BATTLE_CALL_WORK * bcw = pwk;
  GFL_PROC_MAIN_STATUS up_status;

  GF_ASSERT(work);
  GF_ASSERT(bcw);

  up_status = GFL_PROC_LOCAL_Main( work->procsys_up );

  switch (*seq) {
  case SEQ_CALL_START_DEMO:
    // �ʐM�o�g���O�f���Ăяo��
    {
      // �}���`�o�g������
      if( bcw->btl_setup_prm->multiMode == 0 )
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      }
      else
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_MULTI_START;
      }
      GFL_PROC_LOCAL_CallProc( work->procsys_up, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, &bcw->demo_prm);
    }
    (*seq) = SEQ_WAIT_START_DEMO;
    break;
  
  case SEQ_WAIT_START_DEMO:
    if ( up_status != GFL_PROC_MAIN_VALID ){
      (*seq) = SEQ_BATTLE_TIMING_INIT;
    }
    break;
  
  case SEQ_BATTLE_TIMING_INIT:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), EVENT_BATTLE_ADD_CMD_TBL_TIMING);
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add�����̂œ������\n");
      (*seq) = SEQ_BATTLE_TIMING_WAIT;
    }
    break;
  case SEQ_BATTLE_TIMING_WAIT:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), EVENT_BATTLE_ADD_CMD_TBL_TIMING)){
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add��̓�����芮��\n");
      (*seq) = SEQ_BATTLE_INIT;
    }
    break;
  case SEQ_BATTLE_INIT:
//  GMEVENT_CallEvent(event, EVENT_FSND_PushPlayNextBGM(gsys, bcw->btl_setup_prm->musicDefault, FSND_FADE_SHORT, FSND_FADE_NONE)); 
    GFL_PROC_LOCAL_CallProc(work->procsys_up, NO_OVERLAY_ID, &BtlProcData, bcw->btl_setup_prm);
    (*seq) = SEQ_BATTLE_WAIT;
    break;
  case SEQ_BATTLE_WAIT:
    if ( up_status != GFL_PROC_MAIN_VALID ){
      (*seq) = SEQ_BATTLE_END;
    }
  case SEQ_BATTLE_END:
    OS_TPrintf("�o�g������\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    (*seq) = SEQ_CALL_END_DEMO;
    break;
  case SEQ_CALL_END_DEMO:
    // �ʐM�o�g����f���Ăяo��
    {
      // �}���`�o�g������
      if( bcw->btl_setup_prm->multiMode == 0 )
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      }
      else
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_MULTI_START;
      }

      // ���s�ݒ�
      switch( bcw->btl_setup_prm->result )
      {
        case BTL_RESULT_WIN :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_WIN;
          break;
        case BTL_RESULT_LOSE :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_LOSE;
          break;
        case BTL_RESULT_DRAW :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_DRAW;
          break;
        default : GF_ASSERT(0);
      }

      GFL_PROC_LOCAL_CallProc(work->procsys_up, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, &bcw->demo_prm);
      (*seq) = SEQ_CALL_BTL_REC_SEL;
    }
    break;
  case SEQ_CALL_BTL_REC_SEL:
    {
      // �ʐM����Ǝ�����ROM�̃T�[�o�[�o�[�W�������r����
      BOOL b_rec = TRUE;  // TRUE�̂Ƃ��A�u�ʐM���� <= �����v�ƂȂ�^��ł���B
      u8 trainer_num = ( bcw->btl_setup_prm->multiMode == 0 ) ? (2) : (4);
      u8 my_version = bcw->demo_prm->trainer_data[COMM_BTL_DEMO_TRDATA_A].server_version;
      int i; 
      for( i=COMM_BTL_DEMO_TRDATA_B; i<trainer_num; i++ )
      {
        u8 other_version = bcw->demo_prm->trainer_data[i].server_version;
        if( other_version > my_version )
        {
          b_rec = FALSE;
          break;
        }
      }
      // �ʐM�ΐ��̘^��I����ʂֈڍs(�^�悵�Ȃ��l���ڍs���܂�)
      {
        bcw->btl_rec_sel_param.gamedata  = NULL;//GAMESYSTEM_GetGameData( work->procsys_up );
        bcw->btl_rec_sel_param.b_rec     = b_rec;
        GFL_PROC_LOCAL_CallProc( work->procsys_up, FS_OVERLAY_ID( btl_rec_sel ), &BTL_REC_SEL_ProcData, &bcw->btl_rec_sel_param );
      }
      (*seq) = SEQ_BGM_POP;
    }
    break;
  case SEQ_BGM_POP:
    if ( up_status != GFL_PROC_MAIN_VALID){
      break;
    }
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

//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------

FS_EXTERN_OVERLAY(battle);

//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
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
GMEVENT * EVENT_CommBattle(GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM *btl_setup_prm, COMM_BTL_DEMO_PARAM *demo_prm)
{
  EVENT_BATTLE_CALL_WORK *bcw;
  GMEVENT * event;

  event = GMEVENT_Create( gsys, NULL, EVENT_CommBattleMain, sizeof(EVENT_BATTLE_CALL_WORK) );
  bcw = GMEVENT_GetEventWork(event);
  bcw->gsys = gsys;
  bcw->btl_setup_prm = btl_setup_prm;
  bcw->demo_prm = demo_prm;
  
  return event;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g�F�ʐM�o�g���Ăяo��
 *
 *	@param	* event
 *	@param	*  seq
 *	@param	* work
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT_RESULT EVENT_CommBattleMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_BATTLE_CALL_WORK * bcw = work;
  GAMESYS_WORK * gsys = bcw->gsys;

  switch (*seq) 
  {
  case 0:
    GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &CommBattleCommProcData, bcw);
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

