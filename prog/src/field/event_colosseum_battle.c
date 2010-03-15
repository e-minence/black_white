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

#include "system\main.h"
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
#include "gamesystem\btl_setup.h"
#include "field/event_battle_call.h"


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
  FIELDMAP_WORK * fieldmap;
  BATTLE_SETUP_PARAM para;
  COMM_BTL_DEMO_PARAM *demo_prm;
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
#if 0 //�|�P�������X�g��A���ڗ���悤�ɂȂ����̂ō폜
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, cbw->fieldmap) );
    (*seq)++;
    break;
  case 1:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, cbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent( event, EVENT_FieldClose(gsys, cbw->fieldmap));
    (*seq)++;
    break;
#else
    (*seq) = 3;
    break;
#endif
  case 3:
    GMEVENT_CallEvent(event, 
      EVENT_FSND_PushPlayNextBGM(gsys, cbw->para.musicDefault, FSND_FADE_SHORT, FSND_FADE_NONE)); 
    (*seq)++;
    break;
  case 4:
    GMEVENT_CallEvent( event, EVENT_CommBattle(gsys, &cbw->para, cbw->demo_prm) );
    (*seq)++;
    break;
  case 5:
    BATTLE_PARAM_Release( &cbw->para ); //�o�g��SetupParam���
    
    OS_TPrintf("_FIELD_OPEN\n");
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case 6:
    OS_TPrintf("_FIELD_FADEIN\n");
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, cbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 7:
    GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_SHORT, FSND_FADE_NORMAL));
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

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ColosseumBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, UNION_PLAY_CATEGORY play_category, COLOSSEUM_BATTLE_SETUP *setup)
{
  BATTLE_SETUP_PARAM * para;
  EVENT_COLOSSEUM_BATTLE_WORK * cbw;
  BtlRule rule;
  int multi;
  GMEVENT *event;

  event = GMEVENT_Create(
    gsys, NULL, EVENT_ColosseumBattleMain, sizeof(EVENT_COLOSSEUM_BATTLE_WORK));
  cbw = GMEVENT_GetEventWork(event);
  cbw->gsys = gsys;
  cbw->fieldmap = fieldmap;
  cbw->demo_prm = &setup->demo;
  para = &cbw->para;

  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT:   //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER:   //�R���V�A��
    BTL_SETUP_Single_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT:   //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER:   //�R���V�A��
    BTL_SETUP_Double_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT:   //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER:   //�R���V�A��
    BTL_SETUP_Triple_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:       //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT:   //�R���V�A��
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER:   //�R���V�A��
    //��check �܂����[�e�[�V�����̒�`�������̂łƂ肠�����g���v���w��
    BTL_SETUP_Triple_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT:
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER:
    BTL_SETUP_Multi_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, setup->standing_pos, HEAPID_PROC );
    break;
  default:
    GF_ASSERT_MSG(0, "play_category = %d\n", play_category);
    return NULL;
  }
  BATTLE_PARAM_SetPokeParty( para, setup->partyPlayer, BTL_CLIENT_PLAYER );
  BATTLE_PARAM_SetRegulation( para, setup->regulation, HEAPID_PROC );
  
#if 0 //�퓬�����ŃV�F�A�������̂ŁA�����Ŏw��͂���Ȃ�
  para->partyEnemy1 = setup->partyEnemy1;   ///< 1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
  para->partyPartner = setup->partyPartner; ///< 2vs2���̖���AI�i�s�v�Ȃ�null�j
  para->partyEnemy2 = setup->partyEnemy2;    ///< 2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j
#endif
  BTL_SETUP_AllocRecBuffer(para, GFL_HEAPID_APP); //�^��o�b�t�@�m��
  return event;
}



