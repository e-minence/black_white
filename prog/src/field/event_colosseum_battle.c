//==============================================================================
/**
 * @file    event_colosseum_battle.c
 * @brief   �C�x���g�F�R���V�A���o�g��
 * @author  matsuda
 * @date    2009.07.22(��)
 */
//==============================================================================
#include <gflib.h>
#include "net/network_define.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "net_app/union/union_types.h"
#include "net_app/union/colosseum_types.h"
#include "field/event_fldmmdl_control.h"
#include "field/event_colosseum_battle.h"


//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// �o�g���p��`
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------

FS_EXTERN_OVERLAY(battle);


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct{
  GAMESYS_WORK * gsys;
  FIELD_MAIN_WORK * fieldmap;
  BATTLE_SETUP_PARAM para;
}EVENT_COLOSSEUM_BATTLE_WORK;



//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
static GMEVENT_RESULT EVENT_ColosseumBattleMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_COLOSSEUM_BATTLE_WORK * cbw = work;
  GAMESYS_WORK * gsys = cbw->gsys;

  switch (*seq) {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, cbw->fieldmap) );
    (*seq)++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, cbw->fieldmap, FIELD_FADE_BLACK));
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, cbw->fieldmap));
    (*seq)++;
    break;
  case 3:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_ADD_CMD_TBL_AFTER);
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add�����̂œ������\n");
      (*seq) ++;
    }
    break;
  case 4:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_ADD_CMD_TBL_AFTER)){
      OS_TPrintf("�퓬�p�ʐM�R�}���h�e�[�u����Add��̓�����芮��\n");
      (*seq) ++;
    }
    break;
  case 5:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, cbw->para.musicDefault );
    }

    GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &BtlProcData, &cbw->para);
//    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
    (*seq)++;
    break;
  case 6:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    OS_TPrintf("�o�g������\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    (*seq)++;
    break;

  case 7:
    OS_TPrintf("_FIELD_OPEN\n");
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case 8:
    OS_TPrintf("_FIELD_FADEIN\n");
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, cbw->fieldmap, 0));
    (*seq) ++;
    break;
  case 9:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
    }
    PMSND_FadeInBGM(60);
    return GMEVENT_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ColosseumBattle(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap, UNION_PLAY_CATEGORY play_category, const COLOSSEUM_BATTLE_SETUP *setup)
{
  BATTLE_SETUP_PARAM * para;
  EVENT_COLOSSEUM_BATTLE_WORK * cbw;
  BtlRule rule;
  int multi;
  GMEVENT *event;

  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:         //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:   //�R���V�A��
    rule = BTL_RULE_SINGLE;
    multi = 0;
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:         //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:   //�R���V�A��
    rule = BTL_RULE_DOUBLE;
    multi = 0;
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:      //�R���V�A��
    rule = BTL_RULE_DOUBLE;
    multi = 1;
    break;
  default:
    GF_ASSERT(0);
    return NULL;
  }

  event = GMEVENT_Create(
    gsys, NULL, EVENT_ColosseumBattleMain, sizeof(EVENT_COLOSSEUM_BATTLE_WORK));
  cbw = GMEVENT_GetEventWork(event);
  cbw->gsys = gsys;
  cbw->fieldmap = fieldmap;
  para = &cbw->para;
  {
    para->engine = BTL_ENGINE_ALONE;
    para->competitor = BTL_COMPETITOR_COMM;
    para->rule = rule;

    para->commMode = BTL_COMM_DS;
    para->multiMode = multi;

    para->partyPlayer = setup->partyPlayer;
    para->partyEnemy1 = setup->partyEnemy1;   ///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
    para->partyPartner = setup->partyPartner; ///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
    para->partyEnemy2 = setup->partyEnemy2;    ///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j
    para->statusPlayer =  GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));

    para->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
    para->netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );

    para->musicDefault = SEQ_BA_TEST_250KB;   ///< �f�t�H���g����BGM�i���o�[
    para->musicPinch = SEQ_BA_PINCH_TEST_150KB;     ///< �s���`����BGM�i���o�[
  }
  return event;
}



