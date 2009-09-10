//======================================================================
/**
 * @file  event_battle.c
 * @brief �C�x���g�F�t�B�[���h�o�g��
 * @author  tamada GAMEFREAK inc.
 * @date  2008.01.19
 */
//======================================================================
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"
#include "./event_battle.h"

#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��
#include "sound/pm_sndsys.h"

#include "battle/battle.h"
#include "poke_tool/monsno_def.h"
#include "system/main.h" //GFL_HEAPID_APP�Q��

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "event_encount_effect.h"

#include "field_sound.h"

extern const GFL_PROC_DATA DebugSogabeMainProcData;

//======================================================================
//  define
//======================================================================
#define HEAPID_CORE GFL_HEAPID_APP

//======================================================================
//  OVERLAY
//======================================================================
FS_EXTERN_OVERLAY(battle);

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// BATTLE_EVENT_WORK
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  FIELD_MAIN_WORK * fieldmap;
  BATTLE_SETUP_PARAM para;
  u16 timeWait;
  u16 bgmpush_off;
}BATTLE_EVENT_WORK;

//======================================================================
//  proto
//======================================================================
//BATTLE_SETUP_PARAM�̉������
static const void BATTLE_SETUP_PARAM_Destructor(BATTLE_SETUP_PARAM * para);
//POKEPARTY�Ƀ|�P������������
static const void addPartyPokemon(POKEPARTY * party, u16 monsno, u8 level, u16 id);

static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work );

//debug
static GMEVENT_RESULT DebugBattleEvent(
    GMEVENT * event, int *  seq, void * work );

const u32 data_EncountPoke200905[];
const u32 data_EncountPoke200905Max;


//======================================================================
//  �t�B�[���h�@�o�g���C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_Battle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT * event;
  BATTLE_SETUP_PARAM * para;
  BATTLE_EVENT_WORK * dbw;
  
  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );
  
  dbw = GMEVENT_GetEventWork(event);
  MI_CpuClear8( dbw, sizeof(BATTLE_EVENT_WORK) );
  
  dbw->gsys = gsys;
  dbw->fieldmap = fieldmap;
  para = &dbw->para;

  {
    FIELD_ENCOUNT *enc = FIELDMAP_GetEncount( fieldmap );
    FIELD_ENCOUNT_GetBattleSetupParam( enc, para );
  }

  return event;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int tr_id )
{
  GMEVENT * event;
  BATTLE_SETUP_PARAM * para;
  BATTLE_EVENT_WORK * dbw;

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  dbw = GMEVENT_GetEventWork(event);
  MI_CpuClear8( dbw, sizeof(BATTLE_EVENT_WORK) );
  dbw->gsys = gsys;
  dbw->fieldmap = fieldmap;
  dbw->bgmpush_off = TRUE; //�����C�x���gBGM���ɑޔ��ς�
  para = &dbw->para;
  
  {
    FIELD_ENCOUNT *enc = FIELDMAP_GetEncount( fieldmap );
    FIELD_ENCOUNT_SetTrainerBattleSetupParam(
        enc, para, tr_id, HEAPID_PROC );
  }
  
  return event;
}


//--------------------------------------------------------------
/**
 * �t�B�[���h�o�g���C�x���g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work )
{
  BATTLE_EVENT_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;
  
  switch (*seq) {
  case 0:
    // �퓬�p�a�f�l�Z�b�g
    if( dbw->bgmpush_off == FALSE ){ //���ɑޔ��ς�
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, dbw->para.musicDefault );
    }else{
      PMSND_PlayBGM( dbw->para.musicDefault );
    }
    
    //�G���J�E���g�G�t�F�N�g
    GMEVENT_CallEvent( event,
        EVENT_FieldEncountEffect(gsys,dbw->fieldmap) );
    (*seq)++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    (*seq)++;
    break;
  case 2:
    GAMESYSTEM_CallProc(
        gsys, FS_OVERLAY_ID(battle), &BtlProcData, &dbw->para);
    (*seq)++;
    break;
  case 3:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    dbw->timeWait = 60; // �퓬�a�f�l�t�F�[�h�A�E�g
    PMSND_FadeOutBGM( 60 );
    (*seq) ++;
    break;
  case 4:
    if(dbw->timeWait){
      dbw->timeWait--;
    } else {
      (*seq) ++;
    }
    break;
  case 5:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
    }
    PMSND_FadeInBGM(60);
    //
    (*seq) ++;
    break;
  case 6:
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, dbw->fieldmap, 0));
    (*seq) ++;
    break;
  case 7:
    BATTLE_SETUP_PARAM_Destructor(&dbw->para);
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//======================================================================
//  �t�B�[���h�@�f�o�b�O�@�o�g���C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �f�o�b�O�C�x���g �t�B�[���h�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * DEBUG_EVENT_Battle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap )
{
  GMEVENT * event;
  BATTLE_SETUP_PARAM * para;
  BATTLE_EVENT_WORK * dbw;

  event = GMEVENT_Create(
      gsys, NULL, DebugBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  dbw = GMEVENT_GetEventWork(event);
  MI_CpuClear8( dbw, sizeof(BATTLE_EVENT_WORK) );

  dbw->gsys = gsys;
  dbw->fieldmap = fieldmap;
  para = &dbw->para;

  {
    para->engine = BTL_ENGINE_ALONE;
    para->rule = BTL_RULE_SINGLE;
    para->competitor = BTL_COMPETITOR_WILD;

    para->netHandle = NULL;
    para->commMode = BTL_COMM_NONE;
    para->netID = 0;

    //�v���C���[�̃p�[�e�B
    para->partyPlayer = PokeParty_AllocPartyWork( HEAPID_CORE );
    PokeParty_Copy(
        GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys)),
        para->partyPlayer);

    //1vs1���̓GAI, 2vs2���̂P�ԖړGAI�p
    para->partyEnemy1 = PokeParty_AllocPartyWork( HEAPID_CORE );
    addPartyPokemon( para->partyEnemy1, MONSNO_ARUSEUSU+1, 15, 3594 );
    //PokeParty_Add( para->partyEnemy1, PP_Create(MONSNO_ARUSEUSU+1,15,3594,HEAPID_CORE) );

    //2vs2���̖���AI�i�s�v�Ȃ�null�j
    para->partyPartner = NULL;
    //2vs2���̂Q�ԖړGAI�p�i�s�v�Ȃ�null�j
    para->partyEnemy2 = NULL;
    
    //�v���C���[�X�e�[�^�X add
    para->statusPlayer = SaveData_GetMyStatus( SaveControl_GetPointer() );
    
    //�f�t�H���g����BGM�i���o�[
    para->musicDefault = SEQ_WB_BA_TEST_250KB;
    //�s���`����BGM�i���o�[
    para->musicPinch = SEQ_WB_BA_PINCH_TEST_150KB;

    dbw->timeWait = 0;
  }

  return event;
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�o�g���C�x���g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT DebugBattleEvent(
    GMEVENT * event, int *  seq, void * work )
{
  BATTLE_EVENT_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;
#if 0
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent(event,
        EVENT_FieldSubProc(dbw->gsys, dbw->fieldmap,
          FS_OVERLAY_ID(battle), &BtlProcData, &(dbw->para))
        );
    (*seq) ++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
#endif
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    // �퓬�p�a�f�l�Z�b�g
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, dbw->para.musicDefault );
    }
    (*seq)++;
    break;
  case 1:
    GAMESYSTEM_CallProc(
        gsys, FS_OVERLAY_ID(battle), &BtlProcData, &dbw->para);
    (*seq)++;
    break;
  case 2:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL) break;
    // �퓬�a�f�l�t�F�[�h�A�E�g
    dbw->timeWait = 60;
    PMSND_FadeOutBGM(60);
    (*seq) ++;
    break;
  case 3:
    if(dbw->timeWait){
      dbw->timeWait--;
    } else {
      (*seq) ++;
    }
    break;
  case 4:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
    }
    PMSND_FadeInBGM(60);
    //
    (*seq) ++;
    break;
  case 5:
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, dbw->fieldmap, 0));
    (*seq) ++;
    break;
  case 6:
    BATTLE_SETUP_PARAM_Destructor(&dbw->para);
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const void addPartyPokemon(POKEPARTY * party, u16 monsno, u8 level, u16 id)
{
  POKEMON_PARAM * pp = PP_Create(monsno, level, id, HEAPID_CORE);
  PokeParty_Add( party, pp);
  GFL_HEAP_FreeMemory( pp );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static const void BATTLE_SETUP_PARAM_Destructor(BATTLE_SETUP_PARAM * para)
{
  if (para->partyPlayer)
  {
    GFL_HEAP_FreeMemory(para->partyPlayer);
  }
  if (para->partyPartner)
  {
    GFL_HEAP_FreeMemory(para->partyPartner);
  }
  if (para->partyEnemy1)
  {
    GFL_HEAP_FreeMemory(para->partyEnemy1);
  }
  if (para->partyEnemy2)
  {
    GFL_HEAP_FreeMemory(para->partyEnemy2);
  }
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/// �T�����p�G���J�E���g�|�P�����ꗗ
//--------------------------------------------------------------
const u32 data_EncountPoke200905[] =
{
  494,//�s���{�[
  495,//���O�����[
  496,//�S���_���}
  497,//���j���K�l
  498,//���j�O���X
//  499,//���j�o�[��
  500,//�n�S�����V
  501,//�n�L�V�[�h
//  502,//�I�m�b�N�X
  503,//�J�[�����g
//  504,//�o���r�[�i
  505,//�o�g���[�h
  506,//�v�����X
  507,//�R�o�g
  508,//���C�u��
  509,//�o�N�p�N
  510,//�����R�l
  511,//�I�[�o�g
  512,//�p���`�[�k
  513,//���R�E����
};

const u32 data_EncountPoke200905Max = NELEMS(data_EncountPoke200905);
