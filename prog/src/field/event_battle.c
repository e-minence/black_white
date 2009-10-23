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
#include "gamesystem/btl_setup.h"

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

#include "event_gameover.h" //EVENT_NormalLose
#include "field/field_comm/intrude_work.h"
#include "field/field_comm/bingo_system.h"
#include "event_battle_return.h"


//======================================================================
//  define
//======================================================================
#define HEAPID_CORE GFL_HEAPID_APP

//======================================================================
//  OVERLAY
//======================================================================
FS_EXTERN_OVERLAY(battle);
FS_EXTERN_OVERLAY(battle_return);

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// BATTLE_EVENT_WORK
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  BATTLE_SETUP_PARAM* battle_param;
  u16 timeWait;
  u16 bgmpush_off;
}BATTLE_EVENT_WORK;

//======================================================================
//  proto
//======================================================================
//BATTLE_SETUP_PARAM�̃f�o�b�O��������
static void BATTLE_SETUP_PARAM_DebugConstructer(BATTLE_SETUP_PARAM * para, GAMESYS_WORK * gsys);
//POKEPARTY�Ƀ|�P������������
static const void addPartyPokemon(POKEPARTY * party, u16 monsno, u8 level, u16 id);

static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work );
static GMEVENT_RESULT bingoBattleEvent(
    GMEVENT * event, int *  seq, void * work );

//debug
static GMEVENT_RESULT DebugBattleEvent( GMEVENT * event, int *  seq, void * work );

const u32 data_EncountPoke200905[];
const u32 data_EncountPoke200905Max;


static BOOL BEW_IsLoseResult(BATTLE_EVENT_WORK * bew);
static void BEW_reflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata);
static void BEW_Initialize(BATTLE_EVENT_WORK * bew, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, BATTLE_SETUP_PARAM* bp);
static void BEW_Destructor(BATTLE_EVENT_WORK * bew);
//======================================================================
//  �t�B�[���h�@�o�g���C�x���g
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�쐶�|�P�����o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_WildPokeBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, fieldmap, bp );

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
  BATTLE_EVENT_WORK * bew;
  BATTLE_SETUP_PARAM* bp;

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);

  {
    FIELD_ENCOUNT* enc = FIELDMAP_GetEncount(fieldmap);

    bp = BATTLE_PARAM_Create(HEAPID_PROC);
    FIELD_ENCOUNT_SetTrainerBattleParam( enc, bp, tr_id, HEAPID_PROC );
  }

  BEW_Initialize( bew, gsys, fieldmap, bp );
  bew->bgmpush_off = TRUE; //�����C�x���gBGM���ɑޔ��ς�

  return event;
}

//--------------------------------------------------------------
/**
 * �r���S�o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_BingoBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create(
      gsys, NULL, bingoBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, fieldmap, bp );

  //�N���V�X�e���Ɍ��݂̏�Ԃ�ʒm
  IntrudeWork_SetActionStatus(GAMESYSTEM_GetGameCommSysPtr(gsys), INTRUDE_ACTION_BINGO_BATTLE);
  Bingo_Clear_BingoIntrusionBeforeBuffer(Bingo_GetBingoSystemWork(GameCommSys_GetAppWork(GAMESYSTEM_GetGameCommSysPtr(gsys))));

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
  BATTLE_EVENT_WORK * bew = work;
  GAMESYS_WORK * gsys = bew->gsys;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );

  switch (*seq) {
  case 0:
    // �퓬�p�a�f�l�Z�b�g
    if( bew->bgmpush_off == FALSE ){ //���ɑޔ��ς�
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushBGM( fsnd );
    }
    PMSND_PlayBGM( bew->battle_param->musicDefault );
    IWASAWA_Printf("mineParty %08x\n",bew->battle_param->partyPlayer);
    //�G���J�E���g�G�t�F�N�g
    GMEVENT_CallEvent( event,
        EVENT_FieldEncountEffect(gsys,bew->fieldmap) );
    (*seq)++;
    break;
  case 1:
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, bew->fieldmap));
    (*seq)++;
    break;
  case 2:
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    //�o�g���v���Z�X�Ăяo���F�v���Z�X���I�������炱�̃C�x���g�ɕ��A����
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle), &BtlProcData, bew->battle_param );
    (*seq)++;
    break;
  case 3:
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle_return), &BtlRet_ProcData, bew->battle_param );
    (*seq)++;
    break;
  case 4:
    IWASAWA_Printf( "mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer );
    bew->timeWait = 60; // �퓬�a�f�l�t�F�[�h�A�E�g
    PMSND_FadeOutBGM( 60 );
    (*seq) ++;
    break;
  case 5:
    if(bew->timeWait){
      bew->timeWait--;
    } else {
      (*seq) ++;
    }
    break;
  case 6:
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    //�퓬���ʔ��f����
    BEW_reflectBattleResult( bew, gamedata );
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
    }

    //������������
    if (BEW_IsLoseResult( bew) == TRUE )
    {
      //�����̏ꍇ�A�C�x���g�͂����ŏI���B
      //���A�C�x���g�ւƑJ�ڂ���
      BEW_Destructor( bew );
      PMSND_PauseBGM( TRUE );
      GMEVENT_ChangeEvent( event, EVENT_NormalLose(gsys) );
    }
    else
    {
      PMSND_FadeInBGM(60);
      GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    }
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    (*seq) ++;
    break;
  case 7:
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, bew->fieldmap, 0));
    (*seq) ++;
    break;
  case 8:
    IWASAWA_Printf("mineParty%d %08x\n",*seq,bew->battle_param->partyPlayer);
    BEW_Destructor( bew );
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �r���S�o�g���C�x���g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 */
//--------------------------------------------------------------
#include "field/field_comm/bingo_system.h"
static GMEVENT_RESULT bingoBattleEvent(
    GMEVENT * event, int *  seq, void * work )
{
  GMEVENT_RESULT result;
  BATTLE_EVENT_WORK * bew = work;
  GAMESYS_WORK * gsys = bew->gsys;

  result = fieldBattleEvent(event, seq, work);
  if(result == GMEVENT_RES_FINISH){
    //�N���V�X�e���Ɍ��݂̏�Ԃ�ʒm
    IntrudeWork_SetActionStatus(GAMESYSTEM_GetGameCommSysPtr(gsys), INTRUDE_ACTION_FIELD);
  }
  return result;
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
  BATTLE_SETUP_PARAM * bp;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create(
      gsys, NULL, DebugBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  bp = BATTLE_PARAM_Create(HEAPID_PROC);
  BATTLE_SETUP_PARAM_DebugConstructer( bp, gsys );

  BEW_Initialize( bew, gsys, fieldmap , bp );

  bew->timeWait = 0;

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
static GMEVENT_RESULT DebugBattleEvent( GMEVENT * event, int *  seq, void * work )
{
  BATTLE_EVENT_WORK * bew = work;
  GAMESYS_WORK * gsys = bew->gsys;
#if 0
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent(event,
        EVENT_FieldSubProc(bew->gsys, bew->fieldmap,
          FS_OVERLAY_ID(battle), &BtlProcData, &(bew->para))
        );
    (*seq) ++;
    break;
  case 1:
    return GMEVENT_RES_FINISH;
  }
#endif
  switch (*seq) {
  case 0:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, bew->fieldmap));
    // �퓬�p�a�f�l�Z�b�g
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, bew->battle_param->musicDefault );
    }
    (*seq)++;
    break;
  case 1:
    //�o�g���v���Z�X�Ăяo���F�v���Z�X���I�������炱�̃C�x���g�ɕ��A����
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle), &BtlProcData, bew->battle_param );
    (*seq)++;
    break;
  case 2:
    // �퓬�a�f�l�t�F�[�h�A�E�g
    bew->timeWait = 60;
    PMSND_FadeOutBGM(60);
    (*seq) ++;
    break;
  case 3:
    if(bew->timeWait){
      bew->timeWait--;
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
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, bew->fieldmap, 0));
    (*seq) ++;
    break;
  case 6:
    BEW_Destructor( bew );
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
static void BATTLE_SETUP_PARAM_DebugConstructer(BATTLE_SETUP_PARAM * para, GAMESYS_WORK * gsys)
{
  BATTLE_PARAM_Init(para);

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
  para->musicDefault = SEQ_BGM_VS_NORAPOKE;
  //�s���`����BGM�i���o�[
  para->musicPinch = SEQ_BGM_BATTLEPINCH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
/**
 * @brief �����������ʃ`�F�b�N
 * @retval  TRUE  ������
 * @retval  FALSE ���̑�
 */
//--------------------------------------------------------------
static BOOL BEW_IsLoseResult(BATTLE_EVENT_WORK * bew)
{
  switch ((BtlResult)bew->battle_param->result)
  {
  case BTL_RESULT_LOSE:        ///< ������
    return TRUE;
  default:
    GF_ASSERT_MSG(0, "�z��O��BtlResult %d\n", bew->battle_param->result );
    /* FALL THROUGH */
  case BTL_RESULT_WIN:         ///< ������
  case BTL_RESULT_DRAW:        ///< �Ђ��킯
  case BTL_RESULT_RUN:         ///< ������
  case BTL_RESULT_RUN_ENEMY:   ///< ���肪�������i�쐶�̂݁j
  case BTL_RESULT_CATCH:       ///< �߂܂����i�쐶�̂݁j
    return FALSE;
  }
}

//--------------------------------------------------------------
/**
 * @brief �퓬���ʔ��f����
 *
 * @todo  �퓬���ʂ��炸�����莝���|�P������ԂȂǂ̔��f��������������
 */
//--------------------------------------------------------------
static void BEW_reflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata)
{
  GAMEDATA_SetLastBattleResult( gamedata, bew->battle_param->result );
  //�O��ł͒����ւ̔��f�A�T�t�@���{�[���J�E���g�̔��f�A
  //�����������Ă�����PokeParam�̔��f�Ȃǂ��s���Ă���
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void BEW_Initialize(BATTLE_EVENT_WORK * bew, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, BATTLE_SETUP_PARAM* bp)
{
  MI_CpuClear8( bew, sizeof(BATTLE_EVENT_WORK) );
  bew->gsys = gsys;
  bew->fieldmap = fieldmap;
  bew->battle_param = bp;
  bew->bgmpush_off = FALSE;
  bew->timeWait = 0;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void BEW_Destructor(BATTLE_EVENT_WORK * bew)
{
  BATTLE_PARAM_Delete( bew->battle_param );
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
