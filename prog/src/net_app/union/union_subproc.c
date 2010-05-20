//==============================================================================
/**
 * @file    union_subproc.c
 * @brief   ���j�I���F�T�uPROC�Ăяo������
 * @author  matsuda
 * @date    2009.07.16(��)
 */
//==============================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
#include "field/fieldmap.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_types.h"
#include "net_app/union/union_event_check.h"
#include "union_local.h"
#include "app/trainer_card.h"
#include "field/event_fieldmap_control.h"
#include "net_app/union/union_subproc.h"
#include "net_app/union/union_msg.h"
#include "field/event_mapchange.h"
#include "fieldmap/zone_id.h"
#include "field/event_colosseum_battle.h"
#include "colosseum.h"
#include "colosseum_tool.h"
#include "app/mailbox.h"
#include "app\pms_input.h"
#include "app\pms_select.h"
#include "net_app/pokemontrade.h"
#include "net_app/union_app.h"
#include "union_comm_command.h"
#include "net_app/guru2.h"
#include "net_app/oekaki.h"
#include "sound/pm_sndsys.h"
#include "field/field_sound.h"
#include "pm_define.h"
#include "colosseum_comm_command.h"
#include "app/pokelist/plist_comm.h"
#include "system/main.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///���j�I���F�T�uPROC�Ăяo���C�x���g�p���[�N�\����
typedef struct{
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
  UNION_SYSTEM_PTR unisys;
}UNION_SUBPROC_EVENT_WORK;


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static GMEVENT_RESULT UnionSubProc_GameChangeEvent(GMEVENT * event, int * seq, void * work);

static BOOL SubEvent_TrainerCard(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_Trade(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_Minigame(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_ColosseumWarp(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_ColosseumWarpMulti(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_UnionWarp(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_PokelistBattle(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_Battle(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);
static BOOL SubEvent_Chat(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);


//--------------------------------------------------------------
//  �I�[�o�[���C��`
//--------------------------------------------------------------
FS_EXTERN_OVERLAY(union_app);
FS_EXTERN_OVERLAY(pokelist);
FS_EXTERN_OVERLAY(poke_status);
FS_EXTERN_OVERLAY(pokemon_trade);
FS_EXTERN_OVERLAY(app_mail);
FS_EXTERN_OVERLAY(guru2);
FS_EXTERN_OVERLAY(oekaki);


//==============================================================================
//  �f�[�^
//==============================================================================
///�T�uPROC���s����play_category�l�̃e�[�u��
static const struct{
  BOOL (*sub_event)(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq);  ///<���s�֐�
  u8 play_category;       ///<�T�uPROC���s����play_category�l
  u8 after_play_category; ///<�T�uPROC�I����ɐݒ肷��play_category�l
  u8 padding[2];
}SubProc_PlayCategoryTbl[] = {
  {//UNION_SUBPROC_ID_NULL
    NULL,
    UNION_PLAY_CATEGORY_UNION, 
    UNION_PLAY_CATEGORY_UNION,
  },
  {//UNION_SUBPROC_ID_TRAINERCARD
    SubEvent_TrainerCard,
    UNION_PLAY_CATEGORY_TRAINERCARD, 
    UNION_PLAY_CATEGORY_TALK,
  },
  {//UNION_SUBPROC_ID_TRADE
    SubEvent_Trade,
    UNION_PLAY_CATEGORY_TRADE, 
    UNION_PLAY_CATEGORY_TALK,
  },
  {//UNION_SUBPROC_ID_PICTURE
    SubEvent_Minigame,
    UNION_PLAY_CATEGORY_PICTURE, 
    UNION_PLAY_CATEGORY_UNION,
  },
  {//UNION_SUBPROC_ID_GURUGURU
    SubEvent_Minigame,
    UNION_PLAY_CATEGORY_GURUGURU, 
    UNION_PLAY_CATEGORY_UNION,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FREE_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FREE
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FLAT_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FLAT
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FREE_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FREE
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FLAT_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FLAT
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FREE_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FREE
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FLAT_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FLAT
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FREE_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FREE
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FLAT_SHOOTER
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FLAT
    SubEvent_ColosseumWarp,
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT, 
    UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI_FREE_SHOOTER
    SubEvent_ColosseumWarpMulti,
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI_FREE
    SubEvent_ColosseumWarpMulti,
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE, 
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI_FLAT_SHOOTER
    SubEvent_ColosseumWarpMulti,
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER, 
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI_FLAT
    SubEvent_ColosseumWarpMulti,
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT, 
    UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT,
  },
  {//UNION_SUBPROC_ID_UNION_WARP
    SubEvent_UnionWarp,
    UNION_PLAY_CATEGORY_MAX,  //MAX=�ύX���Ȃ�
    UNION_PLAY_CATEGORY_MAX,
  },
  {//UNION_SUBPROC_ID_POKELIST_BATTLE
    SubEvent_PokelistBattle,
    UNION_PLAY_CATEGORY_MAX,  //MAX=�ύX���Ȃ�
    UNION_PLAY_CATEGORY_MAX,
  },
  {//UNION_SUBPROC_ID_BATTLE
    SubEvent_Battle,
    UNION_PLAY_CATEGORY_MAX,  //MAX=�ύX���Ȃ�
    UNION_PLAY_CATEGORY_MAX,
  },
  {//UNION_SUBPROC_ID_COLOSSEUM_TRAINERCARD
    SubEvent_TrainerCard,
    UNION_PLAY_CATEGORY_MAX,  //MAX=�ύX���Ȃ�
    UNION_PLAY_CATEGORY_MAX,
  },
  {//UNION_SUBPROC_ID_CHAT
    SubEvent_Chat,
    UNION_PLAY_CATEGORY_MAX,  //MAX=�ύX���Ȃ�
    UNION_PLAY_CATEGORY_MAX,
  },
};
SDK_COMPILER_ASSERT(UNION_SUBPROC_ID_MAX == NELEMS(SubProc_PlayCategoryTbl));


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * �T�uPROC�Ăяo���C�x���g����
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   unisys		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
GMEVENT * UnionSubProc_Create(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork, UNION_SYSTEM_PTR unisys)
{
	GMEVENT *event;
	UNION_SUBPROC_EVENT_WORK *subev;
	
	event = GMEVENT_Create(gsys, NULL, 
	  UnionSubProc_GameChangeEvent, sizeof(UNION_SUBPROC_EVENT_WORK));

	subev = GMEVENT_GetEventWork(event);
	subev->gsys = gsys;
	subev->fieldWork = fieldWork;
	subev->unisys = unisys;

  unisys->subproc.active = TRUE;
	return event;
}

//--------------------------------------------------------------
/**
 * �T�uPROC�Ăяo���C�x���g
 *
 * @param   event		
 * @param   seq		
 * @param   work		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT UnionSubProc_GameChangeEvent(GMEVENT * event, int * seq, void * work)
{
	UNION_SUBPROC_EVENT_WORK *subev = work;
	GAMESYS_WORK *gsys = subev->gsys;
	UNION_SYSTEM_PTR unisys = subev->unisys;
	UNION_SUB_PROC *subproc = &unisys->subproc;
  GMEVENT *child_event = NULL;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  BOOL next;
  
	switch(*seq) {
	case 0:
	  OS_TPrintf("GMEVENT �T�uPROC�Ăяo�� id = %d\n", subproc->id);
    UnionMsg_AllDel(unisys);

    if(SubProc_PlayCategoryTbl[subproc->id].play_category != UNION_PLAY_CATEGORY_MAX){
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, 
        (void*)SubProc_PlayCategoryTbl[subproc->id].play_category);
    }
    OS_TPrintf("play_category = %d\n", situ->play_category);
    
    (*seq)++;
    //break through
    
	case 1:
	  next = SubProc_PlayCategoryTbl[subproc->id].sub_event(
	      gsys, unisys, subev->fieldWork, subproc->parent_work, event, &child_event, &subproc->seq);
	  if(child_event != NULL){
      GMEVENT_CallEvent(event, child_event);
    }
    else if(next == TRUE){
		  (*seq)++;
		}
		break;
	
	case 2:
    if(SubProc_PlayCategoryTbl[subproc->id].after_play_category != UNION_PLAY_CATEGORY_MAX){
      UnionMySituation_SetParam(unisys, UNION_MYSITU_PARAM_IDX_PLAY_CATEGORY, 
        (void*)SubProc_PlayCategoryTbl[subproc->id].after_play_category);
    }
    OS_TPrintf("after play_category = %d\n", situ->play_category);

    subproc->id = UNION_SUBPROC_ID_NULL;
    subproc->parent_work = NULL;
    subproc->seq = 0;
    subproc->active = FALSE;
	  OS_TPrintf("GMEVENT �T�uPROC�I��\n");
		return GMEVENT_RES_FINISH;
	}
	return GMEVENT_RES_CONTINUE;
}


//==================================================================
/**
 * �T�uPROC�Ăяo���ݒ�
 *
 * @param   unisys		
 * @param   sub_proc_id		
 * @param   parent_wk		
 */
//==================================================================
void UnionSubProc_EventSet(UNION_SYSTEM_PTR unisys, UNION_SUBPROC_ID sub_proc_id, void *parent_wk)
{
  GF_ASSERT(unisys->subproc.id == UNION_SUBPROC_ID_NULL && unisys->subproc.active == FALSE);
  
  unisys->subproc.id = sub_proc_id;
  unisys->subproc.parent_work = parent_wk;
  unisys->subproc.seq = 0;
}

//==================================================================
/**
 * �T�uPROC�����s�������ׂ�
 *
 * @param   unisys		
 *
 * @retval  TRUE:���s���B�@FALSE:�������s���Ă��Ȃ�
 */
//==================================================================
BOOL UnionSubProc_IsExits(UNION_SYSTEM_PTR unisys)
{
  if(unisys->subproc.id == UNION_SUBPROC_ID_NULL && unisys->subproc.active == FALSE){
    return FALSE;
  }
  return TRUE;
}


//==============================================================================
//  �T�u�C�x���g���s�֐�
//==============================================================================
//--------------------------------------------------------------
/**
 * �C�x���g�F�g���[�i�[�J�[�h
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_TrainerCard(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  switch(*seq){
  case 0:
    *child_event = EVENT_FieldSubProc(
  	    gsys, fieldWork, TRCARD_OVERLAY_ID, &TrCardSysCommProcData, pwk);
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�|�P��������
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_Trade(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
#if 1
  POKEMONTRADE_PARAM* pPTP = pwk;
  enum{
    _SEQ_FADEOUT,
    _SEQ_FIELD_OUT,
    _SEQ_TRADE,
    _SEQ_CHECK,
    _SEQ_EVOLUTION,
    _SEQ_EVOLUTIONEND,
    _SEQ_FIELD_IN,
    _SEQ_FADEIN,
    _SEQ_END,
  };
	switch(*seq) {
  case _SEQ_FADEOUT:
    // �t�F�[�h�A�E�g
    *child_event = EVENT_FieldFadeOut_Black(gsys, fieldWork, FIELD_FADE_WAIT);
		(*seq) ++;
    break;
  case _SEQ_FIELD_OUT:
    // �t�F�[�h�A�E�g
    *child_event = EVENT_FieldClose(gsys, fieldWork);
		(*seq) ++;
    break;
	case _SEQ_TRADE:
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(pokemon_trade), &PokemonTradeProcData, pPTP );
		(*seq) ++;
    break;
  case _SEQ_CHECK:
    if( GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL )
    {
      if(pPTP->ret == POKEMONTRADE_MOVE_EVOLUTION){
        (*seq) = _SEQ_EVOLUTION;
      }
      else
      {
        (*seq) = _SEQ_FIELD_IN;
      }
    }
    break;
  case _SEQ_EVOLUTION:
    {
      SHINKA_DEMO_PARAM* sdp = pPTP->shinka_param;
      sdp->gamedata          = pPTP->gamedata;
      sdp->ppt               = pPTP->pParty;
      sdp->after_mons_no     = pPTP->after_mons_no;
      sdp->shinka_pos        = 0;
      sdp->shinka_cond       = pPTP->cond;
      sdp->b_field           = TRUE;
      sdp->b_enable_cancel   = FALSE;
      pPTP->shinka_param = sdp;
    }
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, pPTP->shinka_param );
		(*seq) ++;
    break;
  case _SEQ_EVOLUTIONEND:
    if( GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL )
    {
      if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
      {
        //�G���[����
        (*seq) = _SEQ_FIELD_IN;
      }
      else
      {
        pPTP->ret = POKEMONTRADE_MOVE_EVOLUTION;
        (*seq)=_SEQ_TRADE;
      }
    }
    break;
    
  case _SEQ_FIELD_IN:
    *child_event = EVENT_FieldOpen(gsys);
    (*seq) = _SEQ_FADEIN;
    break;
  case _SEQ_FADEIN:
    // �t�F�[�h�C��
    *child_event = EVENT_FieldFadeIn_Black(gsys, fieldWork, FIELD_FADE_WAIT);
    (*seq) = _SEQ_END;
    break;
  default:
    return TRUE;
  }
  
#endif
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���G����
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_Minigame(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
	UNION_SUB_PROC *subproc = &unisys->subproc;
  enum{
    _SEQ_FADEOUT,
    _SEQ_FIELD_CLOSE,

    _SEQ_INIT,
    _SEQ_TIMING,
    _SEQ_TIMING_WAIT,
    _SEQ_BASIC_STATUS_REQ,
    _SEQ_BASIC_STATUS_WAIT,
    _SEQ_MYSTATUS_SEND,
    _SEQ_MYSTATUS_WAIT,
    _SEQ_START_BEFORE_TIMING,
    _SEQ_START_BEFORE_TIMING_WAIT,
    _SEQ_MINIGAME_PROC,
    _SEQ_MINIGAME_PROC_WAIT,
    
    _SEQ_BGM_FADE,
    _SEQ_APP_FREE,

    _SEQ_FIELD_OPEN,
    _SEQ_FADEIN,
    _SEQ_FINISH,
  };

  if(unisys->alloc.uniapp != NULL){
    UnionAppSystem_Update(unisys->alloc.uniapp, unisys);
  }
  
  switch(*seq){
  case _SEQ_FADEOUT:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut(gsys, fieldWork, 
                                      FIELD_FADE_BLACK, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(parent_event, fade_event);
    }
		(*seq) ++;
		break;
	case _SEQ_FIELD_CLOSE:
		GMEVENT_CallEvent(parent_event, EVENT_FieldClose(gsys, fieldWork));
		(*seq) ++;
		break;

  case _SEQ_INIT:
    {
      int member_max;
      
      //�t�B�[���h��CLOSE���Ă���I�[�o�[���C�ǂݍ���
      GFL_OVERLAY_Load( FS_OVERLAY_ID( union_app ) );
      if(situ->play_category == UNION_PLAY_CATEGORY_PICTURE){
        member_max = UNION_PICTURE_MEMBER_MAX;
      }
      else{ //UNION_PLAY_CATEGORY_GURUGURU
        member_max = UNION_GURUGURU_MEMBER_MAX;
      }
      unisys->alloc.uniapp = UnionAppSystem_AllocAppWork(
        unisys, HEAPID_WORLD, member_max,
        GAMEDATA_GetMyStatus(unisys->uniparent->game_data));
      if(situ->mycomm.intrude == TRUE){  //�������Ȃ�ΐe�͂��ł�uniapp���m�ۂ��Ă���̂œ����̕K�v����
        *seq = _SEQ_BASIC_STATUS_REQ;
      }
      else{
        (*seq)++;
      }
    }
    break;
  case _SEQ_TIMING:
    //�{���Ȃ�t�F�[�h�A�E�g��̐^���Âȉ�ʂœ��������������Ȃ���
    //union_app�̃I�[�o�[���C�z�u��fieldmap�ƕ���ׁ̈A�����ŒʐMTblAdd��̓������s��
    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_SETUP_AFTER, WB_NET_UNION);
    (*seq)++;
    break;
  case _SEQ_TIMING_WAIT:
		if(GFL_NET_HANDLE_IsTimeSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_SETUP_AFTER, WB_NET_UNION) == TRUE){
      (*seq)++;
    }
    break;
  
  case _SEQ_BASIC_STATUS_REQ:
    if(UnionSend_MinigameBasicStatusReq() == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_BASIC_STATUS_WAIT:
    if(UnionAppSystem_CheckBasicStatus(unisys->alloc.uniapp) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_MYSTATUS_SEND:
    if(UnionSend_MinigameMystatusReq(Union_App_GetMemberNetBit(unisys->alloc.uniapp),
        GAMEDATA_GetMyStatus(unisys->uniparent->game_data)) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_MYSTATUS_WAIT:
    if(UnionAppSystem_CheckMystatus(unisys->alloc.uniapp) == TRUE){
      (*seq)++;
    }
    break;

  case _SEQ_START_BEFORE_TIMING:
    if(situ->mycomm.intrude == TRUE){  //�������Ȃ�Γ����̕ς��ɗ����錾�𑗐M
      if(UnionSend_MinigameIntrudeReady(Union_App_GetMemberNetBit(unisys->alloc.uniapp), GAMEDATA_GetMyStatus(unisys->uniparent->game_data)) == TRUE){
        UnionAppSystem_IntrudeReadySetup(unisys->alloc.uniapp);
        *seq = _SEQ_MINIGAME_PROC;
      }
    }
    else{ //�����łȂ��ꍇ�͓������
      GFL_NET_HANDLE_TimeSyncStart(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_START_BEFORE, WB_NET_UNION);
      *seq = _SEQ_START_BEFORE_TIMING_WAIT;
    }
    break;
	case _SEQ_START_BEFORE_TIMING_WAIT:
		if(GFL_NET_HANDLE_IsTimeSync(
		    GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_MINIGAME_START_BEFORE, WB_NET_UNION) == TRUE){
      (*seq) = _SEQ_MINIGAME_PROC;
    }
    break;
      
	case _SEQ_MINIGAME_PROC:
    OS_TPrintf("�~�j�Q�[��PROC�Ăяo��\n");
    GFL_NET_SetNoChildErrorCheck(FALSE);  //�q�@�������Ă��G���[�ɂ��Ȃ�
    if(situ->play_category == UNION_PLAY_CATEGORY_PICTURE){
  	  PICTURE_PARENT_WORK *picpwk = subproc->parent_work;
  	  picpwk->uniapp = unisys->alloc.uniapp;
  	  GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(oekaki), &OekakiProcData, picpwk);
  	}
  	else{ //UNION_PLAY_CATEGORY_GURUGURU
  	  GURUGURU_PARENT_WORK *gurupwk = subproc->parent_work;
  	  gurupwk->uniapp = unisys->alloc.uniapp;
  	  GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(guru2), &Guru2ProcData, gurupwk);
    }
		(*seq) ++;
		break;
	case _SEQ_MINIGAME_PROC_WAIT:
		if(GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }

    if(situ->play_category == UNION_PLAY_CATEGORY_GURUGURU){
  	  GURUGURU_PARENT_WORK *gurupwk = subproc->parent_work;
  	  
  	  if(gurupwk->bgm_change == TRUE){
        u8 season_id = GAMEDATA_GetSeasonID(unisys->uniparent->game_data);
        u16 snd_index = FSND_GetFieldBGM(unisys->uniparent->game_data, ZONE_ID_UNION, season_id);
        PMSND_PlayNextBGM( snd_index, FSND_FADE_FAST, FSND_FADE_NORMAL );
        *seq = _SEQ_BGM_FADE;
      }
      else{
        *seq = _SEQ_APP_FREE;
      }
    }
    else{
      *seq = _SEQ_APP_FREE;
    }
    break;
  
  case _SEQ_BGM_FADE:
    if(PMSND_CheckFadeOnBGM() == FALSE){
      *seq = _SEQ_APP_FREE;
    }
    break;

  case _SEQ_APP_FREE:
    UnionAppSystem_FreeAppWork(unisys->alloc.uniapp);
    unisys->alloc.uniapp = NULL;
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_app ) );
    *seq = _SEQ_FIELD_OPEN;
    break;
	case _SEQ_FIELD_OPEN:
		GMEVENT_CallEvent(parent_event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case _SEQ_FADEIN:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, fieldWork, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(parent_event, fade_event);
    }
		(*seq) ++;
		break;
	case _SEQ_FINISH:
  default:
    return TRUE;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�R���V�A���փ��[�v
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_ColosseumWarp(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  switch(*seq){
  case 0:
    {
      VecFx32 pos;
      
      pos.x = (88 + 16*GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())) << FX32_SHIFT;
      pos.y = 0;
      pos.z = 152 << FX32_SHIFT;
      *child_event = EVENT_ChangeMapPos(gsys, fieldWork, ZONE_ID_CLOSSEUM, &pos, 0, FALSE);
    }
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�}���`�p�̃R���V�A���փ��[�v
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_ColosseumWarpMulti(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  switch(*seq){
  case 0:
    {
      VecFx32 pos;
      
      pos.x = (88 + 16*GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())) << FX32_SHIFT;
      pos.y = 0;
      pos.z = 152 << FX32_SHIFT;
      *child_event = EVENT_ChangeMapPos(gsys, fieldWork, ZONE_ID_CLOSSEUM02, &pos, 0, FALSE);
    }
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F���j�I�����[���փ��[�v
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_UnionWarp(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  switch(*seq){
  case 0:
    {
      VecFx32 pos;
      
      pos.x = 168 << FX32_SHIFT;
      pos.y = 0;
      pos.z = 232 << FX32_SHIFT;
      *child_event = EVENT_ChangeMapPos(gsys, fieldWork, ZONE_ID_UNION, &pos, 0, FALSE);
    }
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�|�P�������X�g�Ăяo�� > �o�g���Ăяo��
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_PokelistBattle(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  UNION_SUBPROC_PARENT_POKELIST *parent_list = pwk;
  PLIST_DATA *plist = &parent_list->plist;
  PSTATUS_DATA *pstatus = &parent_list->pstatus;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  COLOSSEUM_BATTLE_SETUP *battle_setup = parent_list->battle_setup;
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  int my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  
  enum{
    SEQ_FADEOUT,
    SEQ_FIELD_CLOSE,
    SEQ_POKELIST_COMM_INIT,
    SEQ_POKELIST,
    SEQ_POKELIST_WAIT,
    SEQ_STATUS,
    SEQ_STATUS_WAIT,
    SEQ_POKELIST_COMM_EXIT,
    SEQ_BATTLE_INIT,
    SEQ_BATTLE_PARTY_TIMING_WAIT,
    SEQ_BATTLE_PARTY_SEND,
    SEQ_BATTLE_PARTY_RECV_WAIT,
    SEQ_BATTLE_CALL,
    SEQ_BATTLE_EXIT_WAIT,
    SEQ_FIELD_OPEN,
    SEQ_FADEIN,
    SEQ_FINISH,
    SEQ_ERR,
  };
  
  if((*seq) > SEQ_POKELIST_COMM_INIT && (*seq) < SEQ_POKELIST_COMM_EXIT){
    PLIST_COMM_UpdateComm( plist );
  }
  
	switch(*seq) {
	case SEQ_FADEOUT:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut(gsys, fieldWork, 
                                      FIELD_FADE_BLACK, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(parent_event, fade_event);
    }
		(*seq) ++;
		break;
	case SEQ_FIELD_CLOSE:
		GMEVENT_CallEvent(parent_event, EVENT_FieldClose(gsys, fieldWork));
		(*seq) ++;
		break;
	case SEQ_POKELIST_COMM_INIT:
    GFL_OVERLAY_Load(FS_OVERLAY_ID(pokelist_comm));
    PLIST_COMM_InitComm( plist );
    (*seq)++;
    break;
	case SEQ_POKELIST:
    OS_TPrintf("�|�P�������X�g�Ăяo��\n");
		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(pokelist), &PokeList_ProcData, plist);
		(*seq) ++;
		break;
	case SEQ_POKELIST_WAIT:
		if(GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      if(unisys->colosseum_sys != NULL){
        //List���Ő��䂷��̂ŃR�����g�A�E�g Ari100311
        //plist->comm_selected_num 
        //  = ColosseumTool_ReceiveCheck_PokeListSeletedNum(unisys->colosseum_sys);
      }
      break;
    }
    if(plist->ret_mode == PL_RET_STATUS){
      *seq = SEQ_STATUS;
    }
    else{
      *seq = SEQ_POKELIST_COMM_EXIT;
    }
    break;
  case SEQ_STATUS:
    OS_TPrintf("�|�P�����X�e�[�^�X�Ăяo��\n");
    pstatus->ppd = (void*)plist->pp;
    pstatus->cfg = plist->cfg;
    pstatus->game_data = GAMESYSTEM_GetGameData( gsys );

    pstatus->ppt = PST_PP_TYPE_POKEPARTY;
    pstatus->max = PokeParty_GetPokeCount( plist->pp );
    pstatus->mode = PST_MODE_NORMAL;
    pstatus->pos = plist->ret_sel;
    pstatus->page = PPT_INFO;
    
    pstatus->zukan_mode = ZUKANSAVE_GetZenkokuZukanFlag( GAMEDATA_GetZukanSave(pstatus->game_data) );

		GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(poke_status), &PokeStatus_ProcData, pstatus);
		(*seq) ++;
		break;
	case SEQ_STATUS_WAIT:
		if(GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    plist->ret_sel = pstatus->pos;
    (*seq) = SEQ_POKELIST;
    break;
  case SEQ_POKELIST_COMM_EXIT:
    PLIST_COMM_ExitComm( plist );
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(pokelist_comm));
    (*seq) = SEQ_BATTLE_INIT;
    break;
    
  //----------------------------------------------------------------------------
  case SEQ_BATTLE_INIT:
    if(NetErr_App_CheckError()){
      *seq = SEQ_ERR;
      break;
    }
    GF_ASSERT_MSG(plist->ret_mode == PL_RET_NORMAL, "plist->ret_mode �s�� %d\n", plist->ret_mode);
    GF_ASSERT_MSG(plist->ret_sel == PL_SEL_POS_ENTER, "ret_sel=%d\n", plist->ret_sel);
    {//�����̎�M�o�b�t�@�Ƀ��X�g�őI�񂾏��Ƀ|�P�����f�[�^���Z�b�g
      int entry_no, temoti_no;
      POKEPARTY *temoti_party;
      POKEPARTY *dest_party = clsys->recvbuf.pokeparty[my_net_id];
      
      temoti_party = PokeParty_AllocPartyWork(HEAPID_UNION);
      PokeParty_Copy(dest_party, temoti_party);
      PokeParty_InitWork(dest_party);
      
      for(entry_no = 0; entry_no < TEMOTI_POKEMAX; entry_no++){
        if(plist->in_num[entry_no] == 0){
          break;
        }
        PokeParty_Add(dest_party, 
          PokeParty_GetMemberPointer(temoti_party, plist->in_num[entry_no] - 1));
        OS_TPrintf("�|�P�����莝���o�^ entry_no=%d, in_num=%d\n", entry_no, plist->in_num[entry_no]);
      }
      
      GFL_HEAP_FreeMemory(temoti_party);
    }
    
    Colosseum_Clear_ReceivePokeParty(clsys, TRUE);

    GFL_NET_HANDLE_TimeSyncStart(
      GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_POKEPARTY_BEFORE, WB_NET_UNION);
    OS_TPrintf("�o�g���p��POKEPARTY����M�O�̓������J�n\n");
    (*seq)++;
    break;
  case SEQ_BATTLE_PARTY_TIMING_WAIT:
    if(NetErr_App_CheckError()){
      *seq = SEQ_ERR;
      break;
    }
    if(GFL_NET_HANDLE_IsTimeSync(
        GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_POKEPARTY_BEFORE, WB_NET_UNION) == TRUE){
      OS_TPrintf("�o�g���p��POKEPARTY����M�O�̓�����萬��\n");
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_PARTY_SEND:
    if(NetErr_App_CheckError()){
      *seq = SEQ_ERR;
      break;
    }
    if(ColosseumSend_Pokeparty(clsys->recvbuf.pokeparty[my_net_id]) == TRUE){
      OS_TPrintf("POKEPARTY���M\n");
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_PARTY_RECV_WAIT:
    if(NetErr_App_CheckError()){
      *seq = SEQ_ERR;
      break;
    }
    if(Colosseum_AllReceiveCheck_Pokeparty(clsys) == TRUE){
      OS_TPrintf("�S������POKEPARTY��M\n");
      (*seq)++;
    }
    break;
  case SEQ_BATTLE_CALL:
    battle_setup->partyPlayer = clsys->recvbuf.pokeparty[my_net_id];
    battle_setup->standing_pos = clsys->recvbuf.stand_position[my_net_id];
    battle_setup->regulation = unisys->alloc.regulation;
    if(GFL_NET_NO_PARENTMACHINE == my_net_id 
        || (clsys->recvbuf.stand_position[GFL_NET_NO_PARENTMACHINE] & 2) == (clsys->recvbuf.stand_position[my_net_id] & 2)){  //�e�@ or �e�@�̃p�[�g�i�[(�}���`)
      battle_setup->music_no = SEQ_BGM_VS_TRAINER_M;
    }
    else{
      battle_setup->music_no = SEQ_BGM_VS_TRAINER_S;
    }
    Colosseum_SetupBattleDemoParent(clsys, gsys, &battle_setup->demo, HEAPID_UNION);
    
    *child_event = EVENT_ColosseumBattle(gsys, fieldWork, situ->play_category, battle_setup);
    (*seq)++;
    break;
  case SEQ_BATTLE_EXIT_WAIT:
    if(NetErr_App_CheckError() == NET_ERR_CHECK_NONE){
      //�F�B�蒠�̏��s�����X�V
      WIFI_LIST *pWifilist = GAMEDATA_GetWiFiList(GAMESYSTEM_GetGameData( gsys ));
      MYSTATUS *target_myst;
      int member_max = GFL_NET_GetConnectNum();
      int net_id, my_pos;
      int friend_index, win_lose;
      
      my_pos = clsys->recvbuf.stand_position[my_net_id] & 1;
      for(net_id = 0; net_id < member_max; net_id++){
        if(net_id == my_net_id){
          continue;
        }
        if(WifiList_CheckFriendMystatus( 
            pWifilist, &clsys->basic_status[net_id].myst, &friend_index ) == TRUE){
          if((clsys->recvbuf.stand_position[net_id] & 1) == my_pos){  //�p�[�g�i�[
            WifiList_SetLastPlayDate(pWifilist, friend_index); //���t�X�V�̂�
          }
          else{ //�G
            switch(battle_setup->demo.result){
            case COMM_BTL_DEMO_RESULT_WIN:
              WifiList_SetResult(pWifilist, friend_index, 1, 0, 0);
              break;
            case COMM_BTL_DEMO_RESULT_LOSE:
              WifiList_SetResult(pWifilist, friend_index, 0, 1, 0);
              break;
            case COMM_BTL_DEMO_RESULT_DRAW: //���t�X�V�̂�
              WifiList_SetLastPlayDate(pWifilist, friend_index);
              break;
            }
          }
        }
      }
    }
    
    Colosseum_DeleteBattleDemoParent(&battle_setup->demo);
    (*seq) = SEQ_FINISH;  //SEQ_FIELD_OPEN; EVENT_ColosseumBattle��Field���A���Ă���
    break;

  //----------------------------------------------------------------------------
	case SEQ_FIELD_OPEN:
		GMEVENT_CallEvent(parent_event, EVENT_FieldOpen(gsys));
		(*seq) ++;
		break;
	case SEQ_FADEIN:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, fieldWork, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(parent_event, fade_event);
    }
		(*seq) ++;
		break;
	case SEQ_FINISH:
  default:
    return TRUE;

	case SEQ_ERR:
	  (*seq) = SEQ_FIELD_OPEN;
	  break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�퓬��ʌĂяo��
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_Battle(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  
  switch(*seq){
  case 0:
    *child_event = EVENT_ColosseumBattle(gsys, fieldWork, situ->play_category, pwk);
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�ȈՉ�b���͉��
 *
 * @param   gsys		
 * @param   unisys		
 * @param   fieldWork		
 * @param   pwk		
 * @param   seq		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static BOOL SubEvent_Chat(GAMESYS_WORK *gsys, UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, void *pwk, GMEVENT * parent_event, GMEVENT **child_event, u8 *seq)
{
  switch(*seq){
  case 0:
    *child_event = EVENT_FieldSubProc(
  	    gsys, fieldWork, FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, pwk);
    break;
  default:
    return TRUE;
  }
  
  (*seq)++;
  return FALSE;
}

