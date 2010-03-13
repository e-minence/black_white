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

#include "sound/pm_sndsys.h"

#include "battle/battle.h"
#include "system/main.h" //HEAPID_PROC�Q��

//#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "waza_tool/wazano_def.h"

#include "event_encount_effect.h"
#include "effect_encount.h"
#include "field_sound.h"
#include "event_gameover.h" //EVENT_NormalLose
#include "move_pokemon.h"
#include "event_battle_return.h"

#include "script_def.h"   //SCR_BATTLE_�`

#include "enceffno_def.h"
#include "enceffno.h"

#include "gamesystem/game_beacon.h"
#include "tr_tool/trtype_def.h"
#include "tr_tool/tr_tool.h"
#include "battle/battle.h"

#include "event_mission_help_after.h"

#include "debug/debug_flg.h"

#include "trial_house.h"

#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��

//======================================================================
//  define
//======================================================================
enum {
  BATTLE_BGM_FADEOUT_WAIT = 60,
  BATTLE_BGM_FADEIN_WAIT = 60,
};

typedef void (*TR_BEACON_SET_FUNC)( u16 tr_no );

typedef enum{
 TR_BEACON_NORMAL,
 TR_BEACON_LEADER,
 TR_BEACON_BIGFOUR,
 TR_BEACON_CHAMPION,
 TR_BEACON_TYPE_MAX,
}TRAINER_BEACON_TYPE;

typedef enum{
 BTL_BEACON_ST_START,
 BTL_BEACON_ST_WIN,
 BTL_BEACON_ST_CAPTURE,
 BTL_BEACON_ST_MAX,
}BTL_BEACON_ST;

static const TR_BEACON_SET_FUNC DATA_TrBeaconSetFuncTbl[TR_BEACON_TYPE_MAX][2] = {
 { GAMEBEACON_Set_BattleTrainerStart, GAMEBEACON_Set_BattleTrainerVictory },
 { GAMEBEACON_Set_BattleLeaderStart, GAMEBEACON_Set_BattleLeaderVictory },
 { GAMEBEACON_Set_BattleBigFourStart, GAMEBEACON_Set_BattleBigFourVictory },
 { GAMEBEACON_Set_BattleChampionStart, GAMEBEACON_Set_BattleChampionVictory },
};

static u8 DATA_TrBeaconLeaderTbl[] = {
 TRTYPE_LEADER1A,
 TRTYPE_LEADER1B,
 TRTYPE_LEADER1C,
 TRTYPE_LEADER2,
 TRTYPE_LEADER3,
 TRTYPE_LEADER4,
 TRTYPE_LEADER5,
 TRTYPE_LEADER6,
 TRTYPE_LEADER7,
 TRTYPE_LEADER8A,
 TRTYPE_LEADER8B,
};
#define TR_BEACON_LEADER_NUM NELEMS( DATA_TrBeaconLeaderTbl)

static u8 DATA_TrBeaconBigFourTbl[] = {
 TRTYPE_BIGFOUR1,
 TRTYPE_BIGFOUR2,
 TRTYPE_BIGFOUR3,
 TRTYPE_BIGFOUR4,
};
#define TR_BEACON_BIGFOUR_NUM NELEMS( DATA_TrBeaconLeaderTbl)

static u8 DATA_TrBeaconChampTbl[] = {
 TRTYPE_SAGE1,
};
#define TR_BEACON_CHAMP_NUM NELEMS( DATA_TrBeaconChampTbl )



//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/**
 * @struct  BATTLE_EVENT_WORK
 * @brief �퓬�Ăяo���C�x���g�̐��䃏�[�N��`
 *
 */
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;    ///<GAMESYS_WORK�ւ̃|�C���^
  GAMEDATA * gamedata;    ///<GAMEDATA�ւ̃|�C���^�i�ȈՃA�N�Z�X�̂��߁j

  ///�퓬�Ăяo���p�p�����[�^�ւ̃|�C���^
  BATTLE_SETUP_PARAM* battle_param;

  ///�퓬�㏈���i������ǉ���ʁA�i����ʂȂǁj�Ăяo���p�̃p�����[�^
  BTLRET_PARAM        btlret_param;

  ///BGM�����łɑޔ��ς݂��H�t���O
  BOOL bgm_pushed_flag;

  /** @brief  �T�u�C�x���g���ǂ����H�t���O
   * �T�u�C�x���g�̏ꍇ�́A�s�k�����ȂǏI���������Ăяo��������
   * �Ăяo���Ă��炤�K�v�����邽�߁A�t�b�N����
   */
  BOOL is_sub_event;

  /** @brief  �s�k�������Ȃ��o�g�����H�̃t���O */
  BOOL is_no_lose;

  BOOL Examination;   //�g���C�A���n�E�X�R���������s�����HTRUE�ōs��
  
  BOOL not_free_bsp; //�퓬��ABATTLE_SETUP_PARAM���J�����Ȃ�

  //�G���J�E���g�G�t�F�N�g�i���o�[
  int EncEffNo;

}BATTLE_EVENT_WORK;

//======================================================================
//  proto
//======================================================================

static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work );

static void BeaconReq_BtlTrainer( u16 tr_id, BTL_BEACON_ST state );
static void BeaconReq_BtlWild( BATTLE_SETUP_PARAM* bp, BTL_BEACON_ST state );
static void BeaconReq_BattleEnd( BATTLE_EVENT_WORK* bew );

static BOOL BEW_IsLoseResult(BATTLE_EVENT_WORK * bew);
static void BEW_reflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata);
static void BEW_Initialize(BATTLE_EVENT_WORK * bew, GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM* bp);
static void BEW_Destructor(BATTLE_EVENT_WORK * bew);


//======================================================================
//
//
//  �t�B�[���h�@�o�g���C�x���g
//
//
//======================================================================
#ifdef PM_DEBUG
static void debug_FieldDebugFlagSet( BATTLE_SETUP_PARAM* bp )
{
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_ShortcutBtlIn )){
    BTL_SETUP_SetDebugFlag( bp, BTL_DEBUGFLAG_SKIP_BTLIN );
  }
}
#endif  //PM_DEBUG

//--------------------------------------------------------------
//  �쐶��p���b�p�[�C�x���g�̃p�����[�^
//--------------------------------------------------------------
typedef struct {
  GAMESYS_WORK * gsys;
  BATTLE_SETUP_PARAM * bp;
  BOOL sub_event_f;
  ENCOUNT_TYPE EncType;
}WILD_BATTLE_EVENT_WORK;

//--------------------------------------------------------------
//  �쐶��p���b�p�[�C�x���g
//--------------------------------------------------------------
static GMEVENT_RESULT wildBattleEvent( GMEVENT * event, int *seq, void *wk )
{
  WILD_BATTLE_EVENT_WORK * wbew = wk;
  GAMESYS_WORK * gsys =  wbew->gsys;
  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  switch (*seq)
  {
  case 0:
    { //�o�g���C�x���g�̌Ăяo��
      GMEVENT * battle_main_event;
      BATTLE_EVENT_WORK * bew;
      battle_main_event = GMEVENT_Create( gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );
      bew = GMEVENT_GetEventWork( battle_main_event );
      BEW_Initialize( bew, gsys, wbew->bp );
      bew->is_sub_event = TRUE; //���̃C�x���g�����݂���ȏ�A��Ƀo�g���̓T�u�C�x���g
      //bew->is_sub_event = wbew->sub_event_f;

      //�o�g���Z�b�g�A�b�v�p��������퓬����|�P�������擾
      {
        POKEPARTY* enemy = BATTLE_PARAM_GetPokePartyPointer( wbew->bp, BTL_CLIENT_ENEMY1 );
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(enemy, 0);
        int monsno = PP_Get( pp, ID_PARA_monsno, NULL );
        FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork(gsys);
        //�G���J�E���g�G�t�F�N�g�Ƃa�f�l���擾���ăZ�b�g
        ENCEFFNO_GetWildEncEffNoBgmNo( monsno, wbew->EncType, fieldmap, &bew->EncEffNo, &bew->battle_param->musicDefault );
      }

      GMEVENT_CallEvent( event, battle_main_event );
    }
    (*seq) ++;
    break;

  case 1:
    //���̃C�x���g���̂��T�u�C�x���g�̏ꍇ�A���������ɖ߂�
    if ( wbew->sub_event_f == TRUE )
    {
      return GMEVENT_RES_FINISH;
    }

    //���̃C�x���g���g�b�v�̃C�x���g�̏ꍇ�A�퓬�㏈�����s��
    if (FIELD_BATTLE_IsLoseResult(GAMEDATA_GetLastBattleResult(gamedata), BTL_COMPETITOR_WILD) == TRUE)
    {
      //�������ꍇ�͔s�k������
      GMEVENT_ChangeEvent( event, EVENT_NormalLose(gsys) );
    }
    else
    {
      //�����Ă��Ȃ��ꍇ�̓t�F�[�h�C��
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, GAMESYSTEM_GetFieldMapWork(gsys), FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;

  case 2:
    //�N�����肪�������Ă��ꂽ���b�Z�[�W�C�x���g
    {
      COMM_PLAYER_SUPPORT *cps = GAMEDATA_GetCommPlayerSupportPtr(gamedata);
      if(COMM_PLAYER_SUPPORT_GetBattleEndSupportType(cps) == TRUE){
        GMEVENT_CallEvent(event, EVENT_Intrude_BattleHelpAfterEvent( gsys, HEAPID_PROC ));
      }
    }
    (*seq)++;
    break;
  
  case 3:
    return GMEVENT_RES_FINISH;
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�쐶�|�P�����o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param bp
 * @param sub_event
 * @param enc_type
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_WildPokeBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp, BOOL sub_event_f, ENCOUNT_TYPE enc_type )
{
#if 0
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create( gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = sub_event_f;
  bew->EncEffNo = enc_eff_no;     //�G���J�E���g�G�t�F�N�g�Z�b�g
#endif
  GMEVENT * event = GMEVENT_Create( gsys, NULL, wildBattleEvent, sizeof(WILD_BATTLE_EVENT_WORK) );
  WILD_BATTLE_EVENT_WORK * wbew = GMEVENT_GetEventWork( event );
  wbew->gsys = gsys;
  wbew->bp = bp;
  wbew->sub_event_f = sub_event_f;
  wbew->EncType = enc_type;

#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif
  //����Ⴂ�r�[�R�����M���N�G�X�g
  BeaconReq_BtlWild( bp, BTL_BEACON_ST_START );

  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));
  return event;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param tr_id
 * @param flags
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int partner_id, int tr_id0, int tr_id1, u32 flags )
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
    FIELD_ENCOUNT_SetTrainerBattleParam( enc, bp, partner_id, tr_id0, tr_id1, HEAPID_PROC );

    BeaconReq_BtlTrainer( tr_id0, BTL_BEACON_ST_START );

#if 0
    //2009�N12���������p�����F�W�����[�_�[��̂݁ABGM��ύX����
    switch (tr_id0)
    {
    case TRID_LEADER1A_01:
    case TRID_LEADER1B_01:
    case TRID_LEADER1C_01:
    case TRID_LEADER2_01:
    case TRID_LEADER3_01:
    case TRID_LEADER4_01:
    case TRID_LEADER5_01:
    case TRID_LEADER6_01:
    case TRID_LEADER7_01:
    case TRID_LEADER8A_01:
    case TRID_LEADER8B_01:
      bp->musicDefault = SEQ_BGM_VS_GYMLEADER;
    }
#endif
  }

#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif

  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = TRUE;   //�X�N���v�g����Ă΂�遁�g�b�v���x���̃C�x���g�łȂ�
  if ( (flags & SCR_BATTLE_MODE_NOLOSE) != 0 )
  {
    bew->is_no_lose = TRUE;
  }
  //�G���J�E���g�G�t�F�N�g�Ƃa�f�l���Z�b�g
  ENCEFFNO_GetTrEncEffNoBgmNo(tr_id0, fieldmap, &bew->EncEffNo, &bew->battle_param->musicDefault);
  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));

  return event;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param tr_id
 * @param flags
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_BSubwayTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM *bp )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK *bew;
  
#if 0
  {
    POKEPARTY * myparty = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys));
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( myparty, 0 );
    PP_Put( pp, ID_PARA_hp, 1 );
  }
#endif
  
  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );
  
#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif
  
  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
  
#if 0
  bew->is_sub_event = TRUE; //�T�u�C�x���g�Ăяo��
#else
  bew->is_sub_event = FALSE; //�퓬��̃t�F�[�h�C���ړ���
#endif
  
  bew->is_no_lose = TRUE; //�s�폈������
  bew->not_free_bsp = TRUE; //�J���͂��Ȃ�
  
  //�G���J�E���g�G�t�F�N�g�Ƃa�f�l�Z�b�g(�T�u�E�F�C�ŗL)
  bew->EncEffNo = ENCEFFID_SUBWAY;
  bew->battle_param->musicDefault = SEQ_BGM_VS_SUBWAY_TRAINER;
  
  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));
  return event;
}

//--------------------------------------------------------------
/**
 * �g���C�A���n�E�X�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param tr_id
 * @param flags
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TrialHouseTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM *bp )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK *bew;
  
  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );
  
#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
#if 0
  bew->is_sub_event = TRUE; //�T�u�C�x���g�Ăяo��
#else
  bew->is_sub_event = FALSE; //�퓬��̃t�F�[�h�C���ړ���
#endif
  bew->Examination = TRUE;    //�̓_����
  bew->is_no_lose = TRUE; //�s�폈������
  //�G���J�E���g�G�t�F�N�g�Ƃa�f�l�Z�b�g(�T�u�E�F�C�Ɠ���) 
  bew->EncEffNo = ENCEFFID_SUBWAY;
  bew->battle_param->musicDefault = SEQ_BGM_VS_SUBWAY_TRAINER;

  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));
  return event;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�ߊl�f���o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_CaptureDemoBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, HEAPID heapID )
{
  GMEVENT * event;
  GAMEDATA* gdata;
  BATTLE_EVENT_WORK * bew;
  BATTLE_SETUP_PARAM* bp;
  HEAPID tmpHeapID = GFL_HEAP_LOWID( heapID );

  gdata = GAMESYSTEM_GetGameData( gsys );
  //�o�g���p�����[�^�Z�b�g
  bp = BATTLE_PARAM_Create( heapID );

  {
    BTL_FIELD_SITUATION sit;
    POKEPARTY* partyMine = PokeParty_AllocPartyWork( tmpHeapID );
    POKEPARTY* partyEnemy = PokeParty_AllocPartyWork( tmpHeapID );
    POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( tmpHeapID, POKETOOL_GetWorkSize() );

    PP_Setup( pp, MONSNO_TIRAAMHI, 5, 0 );
    PP_Put( pp, ID_PARA_waza1, WAZANO_HATAKU );
    PP_Put( pp, ID_PARA_waza2, WAZANO_NAKIGOE );
    PP_Put( pp, ID_PARA_waza3, WAZANO_NULL );
    PP_Put( pp, ID_PARA_waza4, WAZANO_NULL );
    PokeParty_Add( partyMine, pp );
    
    PP_Setup( pp, MONSNO_MINEZUMI, 2, 0 );
    PP_Put( pp, ID_PARA_waza1, WAZANO_TAIATARI );
    PP_Put( pp, ID_PARA_waza2, WAZANO_NIRAMITUKERU );
    PP_Put( pp, ID_PARA_waza3, WAZANO_NULL );
    PP_Put( pp, ID_PARA_waza4, WAZANO_NULL );
    PokeParty_Add( partyEnemy, pp );

    BTL_FIELD_SITUATION_SetFromFieldStatus( &sit, gdata, fieldmap );
    sit.bgAttr = BATTLE_BG_ATTR_GRASS;
    sit.bgType = BATTLE_BG_TYPE_GRASS;
    BTL_SETUP_CaptureDemo( bp, gdata, partyMine, partyEnemy, &sit, heapID );

    GFL_HEAP_FreeMemory( pp );
    GFL_HEAP_FreeMemory( partyEnemy );
    GFL_HEAP_FreeMemory( partyMine );
  }

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = TRUE;
  //�G���J�E���g�G�t�F�N�g�Ƃa�f�l���Z�b�g
  ENCEFFNO_GetWildEncEffNoBgmNo( MONSNO_MINEZUMI, ENC_TYPE_DEMO, fieldmap, &bew->EncEffNo, &bew->battle_param->musicDefault );

  return event;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�o�g���C�x���g
 * @param event GMEVENT
 * @param seq �C�x���g�V�[�P���X
 * @param wk �C�x���g���[�N
 * @retval GMEVENT_RESULT
 *
 * @todo
 * ���͒P��̃C�x���g�ł��ׂẴt�B�[���h�퓬�Ăяo����
 * ���s���Ă��邪�A�o���G�[�V�������������Ƃ����
 * ��������ׂ����ǂ�������������
 */
//--------------------------------------------------------------
static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work )
{
  BATTLE_EVENT_WORK * bew = work;
  GAMESYS_WORK * gsys = bew->gsys;
  GAMEDATA * gamedata = bew->gamedata;
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_SOUND * fsnd = GAMEDATA_GetFieldSound( gamedata );
  
  switch (*seq) {
  case 0:
    // �퓬�p�a�f�l�Z�b�g
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, bew->battle_param->musicDefault));
    bew->bgm_pushed_flag = TRUE;
    (*seq)++;
    break;
  case 1:
    //�G���J�E���g�G�t�F�N�g
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(fieldmap), event, bew->EncEffNo);
/**
    GMEVENT_CallEvent( event,
        EVENT_FieldEncountEffect(gsys,fieldmap) );
*/
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
    (*seq)++;
    break;
  case 3:
    //�N���V�X�e���ɐ퓬���ł��邱�Ƃ�`����
    {
      FIELD_STATUS *fldstatus = GAMEDATA_GetFieldStatus(gamedata);
      FIELD_STATUS_SetProcAction( fldstatus, PROC_ACTION_BATTLE );
      COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
    }
    //�o�g���v���Z�X�Ăяo���F�v���Z�X���I�������炱�̃C�x���g�ɕ��A����
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle), &BtlProcData, bew->battle_param );
    (*seq)++;
    break;
  case 4:
    bew->btlret_param.btlResult = bew->battle_param;
    bew->btlret_param.gameData  = gamedata;
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle_return), &BtlRet_ProcData, &bew->btlret_param );
    (*seq)++;
    break;
  case 5:
    //�N���V�X�e���Ƀt�B�[���h���ł��邱�Ƃ�`����
    {
      FIELD_STATUS *fldstatus = GAMEDATA_GetFieldStatus(gamedata);
      FIELD_STATUS_SetProcAction( fldstatus, PROC_ACTION_FIELD );
      COMM_PLAYER_SUPPORT_SetBattleEnd(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
    }
    (*seq) ++;
    break;
  case 6: 
    if (bew->bgm_pushed_flag == TRUE) {
      GMEVENT_CallEvent( event, EVENT_FSND_PopPlayBGM_fromBattle(gsys) );
      bew->bgm_pushed_flag = FALSE;
    }
    (*seq) ++;
    break;
  case 7: 
    //�퓬���ʔ��f����
    BEW_reflectBattleResult( bew, gamedata );

    //�̓_����
    if (bew->Examination)
    {
      TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);
      NOZOMU_Printf("����̓_\n");
      TRIAL_HOUSE_AddBtlPoint( *ptr, bew->battle_param );
    }

    //������������
    if (bew->is_no_lose == FALSE && BEW_IsLoseResult( bew) == TRUE )
    {
      //�����̏ꍇ�A�C�x���g�͂����ŏI���B
      //���A�C�x���g�ւƑJ�ڂ���
      if (bew->is_sub_event == TRUE) {
        PMSND_PauseBGM( TRUE );
        BEW_Destructor( bew );
        return GMEVENT_RES_FINISH;
      } else {
        PMSND_PauseBGM( TRUE );
        BEW_Destructor( bew );
        GMEVENT_ChangeEvent( event, EVENT_NormalLose(gsys) );
        return GMEVENT_RES_CONTINUE;  //ChangeEvent�ł�FINISH�����Ă͂����Ȃ�
      }
    }
    else
    {
      //PMSND_FadeInBGM(BATTLE_BGM_FADEIN_WAIT);
      GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    }
    (*seq) ++;
    break;
  case 8:
    if ( bew->is_sub_event == FALSE )
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 9:
    BEW_Destructor( bew );
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


//======================================================================
//
//  �r�[�R���z�M�֘A�@�T�u�֐�
//
//
//======================================================================

//--------------------------------------------------------------
/*
 *  @brief  �g���[�i�[�r�[�R���^�C�v�擾
 */
//--------------------------------------------------------------
static u8 btl_trainer_GetBeaconType( u16 tr_id )
{
  int i;
  u16 tr_type = TT_TrainerDataParaGet( tr_id, ID_TD_tr_type );
  
  for(i = 0;i < TR_BEACON_CHAMP_NUM;i++){
    if( tr_type == DATA_TrBeaconChampTbl[i] ){
      return TR_BEACON_CHAMPION;
    }
  }
  for(i = 0;i < TR_BEACON_BIGFOUR_NUM;i++){
    if( tr_type == DATA_TrBeaconBigFourTbl[i] ){
      return TR_BEACON_BIGFOUR;
    }
  }
  for(i = 0;i < TR_BEACON_LEADER_NUM;i++){
    if( tr_type == DATA_TrBeaconLeaderTbl[i] ){
      return TR_BEACON_LEADER;
    }
  }
  return TR_BEACON_NORMAL;
}

//--------------------------------------------------------------
/*
 *  @brief  �g���[�i�[�퓬�r�[�R�����N�G�X�g
 */
//--------------------------------------------------------------
static void BeaconReq_BtlTrainer( u16 tr_id, BTL_BEACON_ST state )
{
  TRAINER_BEACON_TYPE type = btl_trainer_GetBeaconType( tr_id );

  DATA_TrBeaconSetFuncTbl[type][state]( tr_id );
}

//--------------------------------------------------------------
/*
 *  @brief  �쐶��r�[�R�����N�G�X�g
 */
//--------------------------------------------------------------
static void BeaconReq_BtlWild( BATTLE_SETUP_PARAM* bp, BTL_BEACON_ST state )
{
  POKEPARTY* party = BATTLE_PARAM_GetPokePartyPointer( bp, BTL_CLIENT_ENEMY1 );
  u16 monsno = PP_Get( PokeParty_GetMemberPointer(party,0), ID_PARA_monsno, NULL );

  switch( state ){
  case BTL_BEACON_ST_START:
    GAMEBEACON_Set_BattlePokeStart( monsno );
    break;
  case BTL_BEACON_ST_WIN:
    GAMEBEACON_Set_BattlePokeVictory( monsno );
    break;
  case BTL_BEACON_ST_CAPTURE:
    monsno = PP_Get( PokeParty_GetMemberPointer(party,bp->capturedPokeIdx), ID_PARA_monsno, NULL );
    GAMEBEACON_Set_PokemonGet( monsno );
    break;
  }
}

//--------------------------------------------------------------
/*
 *  @brief  �o�g���I�����r�[�R�����N�G�X�g
 */
//--------------------------------------------------------------
static void BeaconReq_BattleEnd( BATTLE_EVENT_WORK* bew )
{
  BATTLE_SETUP_PARAM* bp = bew->battle_param;

  switch( bp->competitor ){
  case BTL_COMPETITOR_WILD:
    if( bp->result == BTL_RESULT_CAPTURE ){
      BeaconReq_BtlWild( bp, BTL_BEACON_ST_CAPTURE );
    }else if( bp->result == BTL_RESULT_WIN ){
      BeaconReq_BtlWild( bp, BTL_BEACON_ST_WIN );
    }
    break;
  case BTL_COMPETITOR_TRAINER:
    if( bp->result == BTL_RESULT_WIN ){
      BeaconReq_BtlTrainer( bp->tr_data[BTL_CLIENT_ENEMY1]->tr_id, BTL_BEACON_ST_WIN );
    }
    break;
  }
}

//======================================================================
//
//
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   �o�g����ʂł̃A�g���r���[�g�w���Ԃ�
 * @param value   �}�b�v�A�g���r���[�g
 * @retval  BtlBgAttr
 */
//--------------------------------------------------------------
BtlBgAttr FIELD_BATTLE_GetBattleAttrID( MAPATTR_VALUE value )
{
  if ( MAPATTR_VALUE_CheckLawn(value) ) return BATTLE_BG_ATTR_LAWN; //�Ő�
  if ( MAPATTR_VALUE_CheckMonoGround(value) ) return BATTLE_BG_ATTR_NORMAL_GROUND; //�ʏ�n��
  if ( MAPATTR_VALUE_CheckSeasonGround1(value) ) return BATTLE_BG_ATTR_GROUND1; //�n�ʂP
  if ( MAPATTR_VALUE_CheckSeasonGround2(value) ) return BATTLE_BG_ATTR_GROUND2; //�n�ʂQ
  if ( MAPATTR_VALUE_CheckEncountGrass(value) ) return BATTLE_BG_ATTR_GRASS;
  if ( MAPATTR_VALUE_CheckWaterType(value) ) return BATTLE_BG_ATTR_WATER; //�݂�
  if ( MAPATTR_VALUE_CheckSnowType(value) ) return BATTLE_BG_ATTR_SNOW;
  if ( MAPATTR_VALUE_CheckSandType(value) ) return BATTLE_BG_ATTR_SAND;
  if ( MAPATTR_VALUE_CheckMarsh(value) ) return BATTLE_BG_ATTR_MARSH;
  if ( MAPATTR_VALUE_CheckEncountGround(value) ) return BATTLE_BG_ATTR_CAVE; //�ǂ�����
  if ( MAPATTR_VALUE_CheckPool(value) ) return BATTLE_BG_ATTR_POOL;
  if ( MAPATTR_VALUE_CheckShoal(value) ) return BATTLE_BG_ATTR_SHOAL;
  if ( MAPATTR_VALUE_CheckIce(value) ) return BATTLE_BG_ATTR_ICE;
  if ( MAPATTR_VALUE_CheckEncountIndoor(value) ) return BATTLE_BG_ATTR_INDOOR;
  OS_Printf("Warning! GetBattleAttrID = 0x%02x\n", value ); //�ӂ[�͒�`�ς݂̂ł�������I
  return BATTLE_BG_ATTR_LAWN; //�W��
}

//--------------------------------------------------------------
/**
 * @brief �u�s�k�����v�Ƃ���ׂ����ǂ����̔���
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @param competitor  �ΐ푊��̎��
 * @return  BOOL  TRUE�̂Ƃ��A�s�k����������ׂ�
 */
//--------------------------------------------------------------
BOOL FIELD_BATTLE_IsLoseResult(BtlResult result, BtlCompetitor competitor)
{
  enum {
    RES_LOSE = 0,
    RES_WIN,
    RES_ERR,
  };

  static const u8 result_table[ BTL_RESULT_MAX ][ BTL_COMPETITOR_MAX ] = {
    //�쐶        �g���[�i�[  �T�u�E�F�C  �ʐM
    { RES_LOSE,   RES_LOSE,   RES_LOSE,   RES_LOSE }, //BTL_RESULT_LOSE
    { RES_WIN,    RES_WIN,    RES_WIN,    RES_WIN  }, //BTL_RESULT_WIN
    { RES_LOSE,   RES_LOSE,   RES_LOSE,   RES_LOSE }, //BTL_RESULT_DRAW
    { RES_WIN,    RES_ERR,    RES_ERR,    RES_LOSE }, //BTL_RESULT_RUN
    { RES_WIN,    RES_ERR,    RES_WIN,    RES_WIN  }, //BTL_RESULT_RUN_ENEMY
    { RES_WIN,    RES_ERR,    RES_ERR,    RES_ERR  }, //BTL_RESULT_CAPTURE
  };

  SDK_COMPILER_ASSERT( BTL_COMPETITOR_WILD == 0 );
  SDK_COMPILER_ASSERT( BTL_COMPETITOR_TRAINER == 1 );
  SDK_COMPILER_ASSERT( BTL_COMPETITOR_SUBWAY == 2 );
  SDK_COMPILER_ASSERT( BTL_COMPETITOR_COMM == 3 );
  SDK_COMPILER_ASSERT( BTL_RESULT_CAPTURE == 5 );

  u8 lose_flag;
  GF_ASSERT_MSG( result <= BTL_RESULT_CAPTURE, "�z��O��BtlResult(%d)\n", result );
  GF_ASSERT_MSG( competitor < BTL_COMPETITOR_MAX, "�z��O��BtlCompetitor(%d)\n", competitor);

  if( competitor == BTL_COMPETITOR_DEMO_CAPTURE ){
    return FALSE;
  }
  lose_flag = result_table[result][competitor];
   
  if( lose_flag == RES_ERR )
  {
    OS_Printf("�o�g�����炠�肦�Ȃ�����(Result=%d, Competitor=%d)\n", result, competitor );
  }
  return (lose_flag == RES_LOSE);
}

//--------------------------------------------------------------
/**
 * @brief �쐶��@�Đ�R�[�h�`�F�b�N
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @return  SCR_WILD_BTL_RES_XXXX
 */
//--------------------------------------------------------------
u8 FIELD_BATTLE_GetWildBattleRevengeCode(BtlResult result)
{
  enum {
    RES_ERR = 0xFF,
  };

  static const u8 result_table[6] = {
    RES_ERR,                 //BTL_RESULT_LOSE
    SCR_WILD_BTL_RET_WIN,    //BTL_RESULT_WIN
    RES_ERR,                 //BTL_RESULT_DRAW
    SCR_WILD_BTL_RET_ESCAPE, //BTL_RESULT_RUN
    SCR_WILD_BTL_RET_ESCAPE, //BTL_RESULT_RUN_ENEMY
    SCR_WILD_BTL_RET_CAPTURE,//BTL_RESULT_CAPTURE
  };

  SDK_COMPILER_ASSERT( BTL_COMPETITOR_WILD == 0 );

  GF_ASSERT_MSG( result <= BTL_RESULT_CAPTURE, "�z��O��BtlResult(%d)\n", result );
  if( result_table[result] == RES_ERR )
  {
    OS_Printf("���������͂��̃`�F�b�N�͕K�v�Ȃ��͂�\n");
    return SCR_WILD_BTL_RET_ESCAPE;
  }
  return result_table[result];
}

//--------------------------------------------------------------
/**
 * @brief �����������ʃ`�F�b�N
 * @retval  TRUE  ������
 * @retval  FALSE ���̑�
 */
//--------------------------------------------------------------
static BOOL BEW_IsLoseResult(BATTLE_EVENT_WORK * bew)
{
  return FIELD_BATTLE_IsLoseResult( bew->battle_param->result, bew->battle_param->competitor );
}

//--------------------------------------------------------------
/**
 * @brief �퓬���ʔ��f����
 * @param bew   BATTLE_EVENT_WORK�ւ̃|�C���^
 * @param gamedata
 *
 * @todo  �퓬���ʂ��炸�����莝���|�P������ԂȂǂ̔��f��������������
 */
//--------------------------------------------------------------
static void BEW_reflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata)
{
  GAMEDATA_SetLastBattleResult( gamedata, bew->battle_param->result );

  //�ړ��|�P�����퓬�㏈��
  MP_SetAfterBattle( gamedata, bew->battle_param);

  //�r�[�R�����N�G�X�g
  BeaconReq_BattleEnd( bew );
  
  //�O��ł͒����ւ̔��f�A�T�t�@���{�[���J�E���g�̔��f�A
  //�����������Ă�����PokeParam�̔��f�Ȃǂ��s���Ă���
}

//--------------------------------------------------------------
/**
 * @brief BATTLE_EVENT_WORK�̏���������
 * @param bew   BATTLE_EVENT_WORK�ւ̃|�C���^
 * @param gsys
 * @param bp
 */
//--------------------------------------------------------------
static void BEW_Initialize(BATTLE_EVENT_WORK * bew, GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM* bp)
{
  GFL_STD_MemClear32( bew, sizeof(BATTLE_EVENT_WORK) );
  bew->gsys = gsys;
  bew->gamedata = GAMESYSTEM_GetGameData( gsys );
  bew->battle_param = bp;
  bew->bgm_pushed_flag = FALSE;
  bew->is_sub_event = FALSE;
  bew->is_no_lose = FALSE;
  bew->Examination = FALSE;
}

//--------------------------------------------------------------
/**
 * @brief BATTLE_EVENT_WORK�̏I������
 * @param bew   BATTLE_EVENT_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BEW_Destructor(BATTLE_EVENT_WORK * bew)
{
  if( bew->not_free_bsp == FALSE ){
    BATTLE_PARAM_Delete( bew->battle_param );
  }
}

