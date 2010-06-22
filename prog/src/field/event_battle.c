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
#include "gamesystem/pm_weather.h"

#include "field/fieldmap.h"

#include "./event_fieldmap_control.h"
#include "./event_battle.h"

#include "sound/pm_sndsys.h"

#include "battle/battle.h"
#include "system/main.h" //HEAPID_PROC�Q��

//#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/natsuki.h"
#include "waza_tool/wazano_def.h"

//#include "event_encount_effect.h"
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
#include "savedata/record.h"

#include "event_mission_help_after.h"

#include "debug/debug_flg.h"

#include "trial_house.h"

#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��

#include "amprotect.h"

FS_EXTERN_OVERLAY(amprotect);

#ifdef AMPROTECT_FUNC
static void magicon1_need_func(void);
static void magicon2_need_func(void);
static void magicon3_failure_func(void);
extern void magicon_dummy_task(GFL_TCB *tcb, void *pWork);
extern int  debug_tcb_num;

#endif

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
 TR_BEACON_SPACIAL,
 TR_BEACON_TYPE_MAX,
}TRAINER_BEACON_TYPE;

typedef enum{
 BTL_BEACON_ST_START,
 BTL_BEACON_ST_WIN,
 BTL_BEACON_ST_ESCAPE,
 BTL_BEACON_ST_MAX,
}BTL_BEACON_ST;

static const TR_BEACON_SET_FUNC DATA_TrBeaconSetFuncTbl[TR_BEACON_TYPE_MAX][2] = {
 { GAMEBEACON_Set_BattleTrainerStart, GAMEBEACON_Set_BattleTrainerVictory },
 { GAMEBEACON_Set_BattleLeaderStart, GAMEBEACON_Set_BattleLeaderVictory },
 { GAMEBEACON_Set_BattleSpTrainerStart, GAMEBEACON_Set_BattleSpTrainerVictory },
};

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
static void ReflectRecord( BATTLE_EVENT_WORK* bew );

static BOOL BEW_IsLoseResult(BATTLE_EVENT_WORK * bew);
static void BEW_ReflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata);
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
        ENCEFFNO_GetWildEncEffNoBgmNo( monsno, wbew->EncType, ( wbew->bp->rule == BTL_RULE_DOUBLE),
            fieldmap, &bew->EncEffNo, &bew->battle_param->musicDefault );
      }

      GMEVENT_CallEvent( event, battle_main_event );
    }
    (*seq) ++;
    break;

  case 1:
    //���̃C�x���g���̂��T�u�C�x���g�̏ꍇ�A���������ɖ߂�
    if ( wbew->sub_event_f == TRUE )
    {
      COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
      return GMEVENT_RES_FINISH;
    }

    //���̃C�x���g���g�b�v�̃C�x���g�̏ꍇ�A�퓬�㏈�����s��
    if (FIELD_BATTLE_IsLoseResult(GAMEDATA_GetLastBattleResult(gamedata), BTL_COMPETITOR_WILD) == TRUE)
    {
      //�������ꍇ�͔s�k������
      COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
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
    COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
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
  {
    RECORD* record = GAMEDATA_GetRecordPtr( GAMESYSTEM_GetGameData(gsys));
    RECORD_Inc( record, RECID_BTL_ENCOUNT );
  }

  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));
  return event;
}

//--------------------------------------------------------------
/**
 * �t�B�[���h�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param BtlRule
 * @param tr_id
 * @param flags
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int rule, int partner_id, int tr_id0, int tr_id1, u32 flags )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;
  BATTLE_SETUP_PARAM* bp;
  
  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);

  {
    FIELD_ENCOUNT* enc = FIELDMAP_GetEncount(fieldmap);
    GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
    RECORD* record = GAMEDATA_GetRecordPtr( gdata );

    bp = BATTLE_PARAM_Create(HEAPID_PROC);
    FIELD_ENCOUNT_SetTrainerBattleParam( enc, bp, rule, partner_id, tr_id0, tr_id1, HEAPID_PROC );

    BeaconReq_BtlTrainer( tr_id0, BTL_BEACON_ST_START );
    RECORD_Inc( record, RECID_BTL_TRAINER);
    RECORD_Inc( record, RECID_DAYCNT_TRAINER_BATTLE);
  }
#ifdef PM_DEBUG
  debug_FieldDebugFlagSet( bp );
#endif

  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = TRUE;   //�X�N���v�g����Ă΂�遁�g�b�v���x���̃C�x���g�łȂ�
  if ( (flags & SCR_BATTLE_MODE_NOLOSE) != 0 )
  {
    BATTLE_PARAM_SetBtlStatusFlag( bp, BTL_STATUS_FLAG_NO_LOSE );
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
#if 0
  bew->battle_param->musicDefault = SEQ_BGM_VS_SUBWAY_TRAINER;
  bew->battle_param->musicWin = SEQ_BGM_WIN2;
#endif
   
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

    PP_Setup( pp, MONSNO_TIRAAMHI, 7, 0 );
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
    sit.bgAttr = BATTLE_BG_ATTR_E_GRASS;
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
  ENCEFFNO_GetWildEncEffNoBgmNo( MONSNO_MINEZUMI, ENC_TYPE_DEMO, FALSE, fieldmap, &bew->EncEffNo, &bew->battle_param->musicDefault );
  //�ߊl�ł��͖쐶��G���J�E���g�G�t�F�N�g�Œ�
  bew->EncEffNo = ENCEFFID_WILD_NORMAL;

  return event;
}


//--------------------------------------------------------------
/**
 * �t�B�[���h�g���[�i�[�o�g���C�x���g
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @param BtlRule
 * @param tr_id
 * @param flags
 * @param trade_type    ������@�^�C�v
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_TradeAfterTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int rule, int partner_id, int tr_id0, int tr_id1, u32 flags, TRPOKE_AFTER_SAVE_TYPE trade_type )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;
  POKEPARTY* enemy_party;
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  TRPOKE_AFTER_SAVE* trpoke_sv = GAMEDATA_GetTrPokeAfterSaveData( gdata );
  POKEMON_PARAM* pp;
  u32 monsno;


  event = EVENT_TrainerBattle( gsys, fieldmap, rule, partner_id, tr_id0, tr_id1, flags );


  if( TRPOKE_AFTER_SV_IsData( trpoke_sv, trade_type ) ){
    // trade_type�̃|�P�����ɏ�������
    bew = GMEVENT_GetEventWork(event);
    enemy_party = bew->battle_param->party[ BTL_CLIENT_ENEMY1 ];
    
    // �p�[�e�B�̂P�Ԗ�
    pp = PokeParty_GetMemberPointer( enemy_party, 0 );

    // MonsNo Waza���p��
    monsno = PP_Get( pp, ID_PARA_monsno, NULL );

    PP_SetupEx( pp, TRPOKE_AFTER_SV_GetTradeMonsNo( trpoke_sv, trade_type, GET_VERSION() ), 
        TRPOKE_AFTER_SV_GetLevel( trpoke_sv, trade_type ),
        TRPOKE_AFTER_SV_GetID( trpoke_sv, trade_type ),
        PTL_SETUP_POW_AUTO,
        TRPOKE_AFTER_SV_GetRnd( trpoke_sv, trade_type ) );

    // ��R�����ɑ΂��鏈��
    if( TRPOKE_AFTER_SV_IsSpeabino3( trpoke_sv, trade_type ) ){
      PP_Put( pp, ID_PARA_speabino, TRPOKE_AFTER_SV_GetSpeabino3( trpoke_sv, trade_type ) );
    }
    PP_Put( pp, ID_PARA_nickname_raw, (u32)TRPOKE_AFTER_SV_GetNickName( trpoke_sv, trade_type ) );
    // �i��������B
    PP_ChangeMonsNo( pp, monsno );

    // �f�t�H���g�Z��ݒ�
    PP_SetWazaDefault( pp );

  }else{

    // �����������Ă��Ȃ��B
    GF_ASSERT_MSG( 0, "�����|�P�������Ȃ�\n" );
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
  BATTLE_EVENT_WORK * bew      = work;
  GAMESYS_WORK *      gsys     = bew->gsys;
  GAMEDATA *          gamedata = bew->gamedata;
  FIELDMAP_WORK *     fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u32 magicon_number  = 6451*3191;
  
  
  switch( *seq ) {
  // �퓬�p�a�f�l�Z�b�g
  case 0:
    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayBattleBGM(gsys, bew->battle_param->musicDefault));
    (*seq)++;
    break;

  //�G���J�E���g�G�t�F�N�g
  case 1:
    ENCEFF_SetEncEff(FIELDMAP_GetEncEffCntPtr(fieldmap), event, bew->EncEffNo);
    (*seq)++;
    break;

  // �t�B�[���h�}�b�v�I��
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
#ifdef AMPROTECT_FUNC
    GFL_OVERLAY_Load( FS_OVERLAY_ID(amprotect) );  
#endif
    //�N���V�X�e���Ƀt�B�[���h���ł��邱�Ƃ�`����
    {
      FIELD_STATUS *fldstatus = GAMEDATA_GetFieldStatus(gamedata);
      FIELD_STATUS_SetProcAction( fldstatus, PROC_ACTION_FIELD );
      COMM_PLAYER_SUPPORT_SetBattleEnd(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
    }
    (*seq) ++;
    break;

  case 6: 
#ifdef AMPROTECT_FUNC
    magicon_number += 977*(!AM_IsNotMagiconA1(magicon1_need_func));
#endif
    //�퓬���ʔ��f����
    BEW_ReflectBattleResult( bew, gamedata ); 
    //�̓_����
    if (bew->Examination)
    {
      TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);
      NOZOMU_Printf("����̓_\n");
      TRIAL_HOUSE_AddBtlPoint( *ptr, bew->battle_param );
    }
    (*seq) ++;
#ifdef AMPROTECT_FUNC
    magicon_number += 1489*(!AM_IsNotMagiconA2(magicon2_need_func));
#endif
    break;

  case 7:
#ifdef AMPROTECT_FUNC
    magicon_number += 157*AM_IsMagiconA3(magicon3_failure_func);
#endif
    // �������ꍇ, BGM���~����
    if( (bew->is_no_lose == FALSE) && (BEW_IsLoseResult(bew) == TRUE) ) { 
      GMEVENT_CallEvent( event, EVENT_FSND_ResetBGM( gsys, FSND_FADE_SHORT ) );
    }
    // �����łȂ����, �t�B�[���hBGM�𕜋A
    else {
      GMEVENT_CallEvent( event, EVENT_FSND_PopPlayBGM_fromBattle( gsys ) );
    }
#ifdef AMPROTECT_FUNC
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(amprotect) );  
#endif
    (*seq) ++;
    break;

  case 8: 
    //������������
    if( (bew->is_no_lose == FALSE) && (BEW_IsLoseResult(bew) == TRUE) ) {
      //�����̏ꍇ�A�C�x���g�͂����ŏI���B
      //���A�C�x���g�ւƑJ�ڂ���
      if (bew->is_sub_event == TRUE) {
        BEW_Destructor( bew );
        return GMEVENT_RES_FINISH;
      } else {
        BEW_Destructor( bew );
        GMEVENT_ChangeEvent( event, EVENT_NormalLose(gsys) );
        return GMEVENT_RES_CONTINUE;  //ChangeEvent�ł�FINISH�����Ă͂����Ȃ�
      }
    }
    else {
      GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    }
    (*seq) ++;
    break;

  // BGM�̕��A�҂�
  case 9:
    GMEVENT_CallEvent( event, EVENT_FSND_WaitBGMPop( gsys ) );
    (*seq) ++;
    break;

  case 10:
    if ( bew->is_sub_event == FALSE ) {
      GMEVENT_CallEvent( event, 
          EVENT_FieldFadeIn_Black( gsys, fieldmap, FIELD_FADE_WAIT ) );
    }
    (*seq) ++;
    break;

  case 11:
    BEW_Destructor( bew );
    return GMEVENT_RES_FINISH;
  }


#ifdef AMPROTECT_FUNC

  if(magicon_number % 6451){
    GFL_TCB *dust_tcb = GFUser_VIntr_CreateTCB( magicon_dummy_task, NULL, 127 );
    debug_tcb_num++;
  }
#endif
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
  u8 grp = TT_TrainerTypeGrpGet(TT_TrainerDataParaGet( tr_id, ID_TD_tr_type ) );

  if( grp == TRTYPE_GRP_NONE ||
      grp == TRTYPE_GRP_PLASMA ){
    return TR_BEACON_NORMAL;
  }
  if( grp == TRTYPE_GRP_LEADER ){
    return TR_BEACON_LEADER;
  }
  return TR_BEACON_SPACIAL;
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
  case BTL_BEACON_ST_ESCAPE:
    GAMEBEACON_Set_Escape();
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
  
  POKEPARTY * party = GAMEDATA_GetMyPokemon( bew->gamedata );
  POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, PokeParty_GetMemberTopIdxNotEgg( party ));

  if( (bp->result != BTL_RESULT_LOSE) && (bp->result != BTL_RESULT_DRAW))
  {
    STRBUF* nickname = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, GFL_HEAP_LOWID( HEAPID_PROC ) );
    PP_Get( pp, ID_PARA_nickname, nickname );

    //�擪�|�P�����̎cHP�`�F�b�N
    {
      u16 hp = PP_Get(pp,ID_PARA_hp,NULL);
      u16 hp_max = PP_Get(pp,ID_PARA_hpmax,NULL);

      if( hp == 0 ){
        GAMEBEACON_Set_Dying(nickname);
      }else if( (hp*2) <= hp_max ){
        GAMEBEACON_Set_HPLittle(nickname);
      }
    }
    { //�cPP�`�F�b�N
      int i;

      for(i = 0;i < 4;i++){
        if( ( PP_Get( pp, ID_PARA_waza1+i, NULL) != WAZANO_NULL ) &&
            ( PP_Get( pp, ID_PARA_pp1+i,NULL) == 0) ){
          GAMEBEACON_Set_PPLittle(nickname);
          break;
        }
      }
    }
    { //��Ԉُ�`�F�b�N
      if( PP_Get( pp, ID_PARA_condition, NULL) != 0 ) {
        GAMEBEACON_Set_StateIsAbnormal(nickname);
      }
    }
    GFL_STR_DeleteBuffer( nickname );
  }

  switch( bp->competitor ){
  case BTL_COMPETITOR_WILD:
    if( bp->result == BTL_RESULT_RUN ){
      BeaconReq_BtlWild( bp, BTL_BEACON_ST_ESCAPE );
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

//--------------------------------------------------------------
/*
 *  @brief  �o�g���I�������R�[�h�C���N�������g
 */
//--------------------------------------------------------------
static void ReflectRecord( BATTLE_EVENT_WORK* bew )
{
  BATTLE_SETUP_PARAM* bp = bew->battle_param;
  RECORD* record = GAMEDATA_GetRecordPtr( GAMESYSTEM_GetGameData(bew->gsys));
  
  if( bp->competitor == BTL_COMPETITOR_WILD ){
    switch( bp->result ){
    case BTL_RESULT_RUN_ENEMY:
      RECORD_Inc( record, RECID_NIGERARETA );
      break;
    }
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
 *  �퓬���ʂ���GAMEDATA�ւ̏��s���f�Ȃǂ̏�������������
 *  �O��ł͒����ւ̔��f�A�T�t�@���{�[���J�E���g�̔��f�Ȃǂ��s���Ă���
 */
//--------------------------------------------------------------
static void BEW_ReflectBattleResult(BATTLE_EVENT_WORK * bew, GAMEDATA * gamedata)
{
  GAMEDATA_SetLastBattleResult( gamedata, bew->battle_param->result );

  //�ړ��|�P�����퓬�㏈��
  MP_SetAfterBattle( gamedata, bew->battle_param);

  //�r�[�R�����N�G�X�g
  BeaconReq_BattleEnd( bew );

  //���R�[�h�f�[�^�C���N�������g
  ReflectRecord( bew );

  //���X�̐퓬���ʂ𔽉f���āA�V����Đݒ肷��
  PM_WEATHER_UpdateZoneChangeWeatherNo( bew->gsys, PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork( gamedata )) );
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



#ifdef AMPROTECT_FUNC
static void magicon1_need_func(void)
{
  int primal = 71*83;
  primal += GFL_STD_MtRand(GFL_STD_RAND_MAX)+GFL_STD_MtRand(GFL_STD_RAND_MAX);
  return;
}
static void magicon2_need_func(void)
{
  int primal = 2111*101;
  primal += GFL_STD_MtRand(GFL_STD_RAND_MAX)+GFL_STD_MtRand(GFL_STD_RAND_MAX);
  return;
}
static void magicon3_failure_func(void)
{
  int primal = 1759*2677;
  primal += GFL_STD_MtRand(GFL_STD_RAND_MAX)+GFL_STD_MtRand(GFL_STD_RAND_MAX);
  return;
}
#endif
