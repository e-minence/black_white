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
#include "system/main.h" //HEAPID_PROC�Q��

//#include "poke_tool/monsno_def.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "event_encount_effect.h"
#include "effect_encount.h"
#include "field_sound.h"
#include "event_gameover.h" //EVENT_NormalLose
#include "move_pokemon.h"
#include "event_battle_return.h"

#include "script_def.h"   //SCR_BATTLE_�`

//======================================================================
//  define
//======================================================================
enum {
  BATTLE_BGM_FADEOUT_WAIT = 60,
  BATTLE_BGM_FADEIN_WAIT = 60,
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

  ///�^�C�~���O�p���[�N�i���BGM�t�F�[�h����j
  u16 timeWait;

  ///BGM�����łɑޔ��ς݂��H�t���O
  BOOL bgm_pushed_flag;

  /** @brief  �T�u�C�x���g���ǂ����H�t���O
   * �T�u�C�x���g�̏ꍇ�́A�s�k�����ȂǏI���������Ăяo��������
   * �Ăяo���Ă��炤�K�v�����邽�߁A�t�b�N����
   */
  BOOL is_sub_event;

  /** @brief  �s�k�������Ȃ��o�g�����H�̃t���O */
  BOOL is_no_lose;

}BATTLE_EVENT_WORK;

//======================================================================
//  proto
//======================================================================

static GMEVENT_RESULT fieldBattleEvent(
    GMEVENT * event, int *  seq, void * work );


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
//--------------------------------------------------------------
/**
 * �t�B�[���h�쐶�|�P�����o�g���C�x���g�쐬
 * @param gsys  GAMESYS_WORK
 * @param fieldmap FIELDMAP_WORK
 * @retval GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_WildPokeBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp, BOOL sub_event_f )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = sub_event_f;

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
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int tr_id0, int tr_id1, u32 flags )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;
  BATTLE_SETUP_PARAM* bp;

#if 0
  {
    POKEPARTY * myparty = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys));
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( myparty, 0 );
    PP_Put( pp, ID_PARA_hp, 1 );
  }
#endif
  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);

  {
    FIELD_ENCOUNT* enc = FIELDMAP_GetEncount(fieldmap);

    bp = BATTLE_PARAM_Create(HEAPID_PROC);
    FIELD_ENCOUNT_SetTrainerBattleParam( enc, bp, tr_id0, tr_id1, HEAPID_PROC );
  }

  BEW_Initialize( bew, gsys, bp );
  bew->is_sub_event = TRUE;   //�X�N���v�g����Ă΂�遁�g�b�v���x���̃C�x���g�łȂ�
  if ( (flags & SCR_BATTLE_MODE_NOLOSE) != 0 )
  {
    bew->is_no_lose = TRUE;
  }
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
  
  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );
  
  //�G�t�F�N�g�G���J�E�g�@�G�t�F�N�g���A�L�����Z��
  EFFECT_ENC_EffectRecoverCancel( FIELDMAP_GetEncount(fieldmap));
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
    switch ( FIELD_SOUND_GetBGMPushCount( fsnd ) )
    {
    case FSND_PUSHCOUNT_NONE:
      //�ʏ�t�B�[���hBGM�K�w��Push
      FIELD_SOUND_PushBGM( fsnd );
      break;
    case FSND_PUSHCOUNT_BASEBGM:
      //�C�x���gBGM�K�w�Ȃ̂ŉ������Ȃ��i�����ɂ���܂ł�Push����Ă���j
      break;
    case FSND_PUSHCOUNT_EVENTBGM:
    case FSND_PUSHCOUNT_OVER:
      GF_ASSERT_MSG(0, "EventBattle:BGM�K�w���d�˂����Ă��܂��I�I\n");
    }
    bew->bgm_pushed_flag = TRUE;
    PMSND_PlayBGM( bew->battle_param->musicDefault );
    //�G���J�E���g�G�t�F�N�g
    GMEVENT_CallEvent( event,
        EVENT_FieldEncountEffect(gsys,fieldmap) );
    (*seq)++;
    break;
  case 1:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, fieldmap));
    (*seq)++;
    break;
  case 2:
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
  case 3:
    bew->btlret_param.btlResult = bew->battle_param;
    bew->btlret_param.gameData  = gamedata;
    GMEVENT_CallProc( event, FS_OVERLAY_ID(battle_return), &BtlRet_ProcData, &bew->btlret_param );
    (*seq)++;
    break;
  case 4:
    //�N���V�X�e���Ƀt�B�[���h���ł��邱�Ƃ�`����
    {
      FIELD_STATUS *fldstatus = GAMEDATA_GetFieldStatus(gamedata);
      FIELD_STATUS_SetProcAction( fldstatus, PROC_ACTION_FIELD );
      COMM_PLAYER_SUPPORT_Init(GAMEDATA_GetCommPlayerSupportPtr(gamedata));
    }
    bew->timeWait = BATTLE_BGM_FADEOUT_WAIT; // �퓬�a�f�l�t�F�[�h�A�E�g
    PMSND_FadeOutBGM( BATTLE_BGM_FADEOUT_WAIT );
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
    if (bew->bgm_pushed_flag == TRUE) {
      FIELD_SOUND_PopBGM( fsnd );
      bew->bgm_pushed_flag = FALSE;
    }

    //�퓬���ʔ��f����
    BEW_reflectBattleResult( bew, gamedata );

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
      PMSND_FadeInBGM(BATTLE_BGM_FADEIN_WAIT);
      GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    }
    (*seq) ++;
    break;
  case 7:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 8:
    BEW_Destructor( bew );
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
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
  GF_ASSERT_MSG( competitor <= BTL_COMPETITOR_COMM, "�z��O��BtlCompetitor(%d)\n", competitor);
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
  bew->timeWait = 0;
}

//--------------------------------------------------------------
/**
 * @brief BATTLE_EVENT_WORK�̏I������
 * @param bew   BATTLE_EVENT_WORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void BEW_Destructor(BATTLE_EVENT_WORK * bew)
{
  BATTLE_PARAM_Delete( bew->battle_param );
}

