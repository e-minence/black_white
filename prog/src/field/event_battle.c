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

#include "event_battle_return.h"


//======================================================================
//  define
//======================================================================
#define HEAPID_CORE GFL_HEAPID_APP

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

  u16 timeWait;
  BOOL bgm_pushed_flag;
  BOOL is_sub_event;
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
GMEVENT * EVENT_WildPokeBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp )
{
  GMEVENT * event;
  BATTLE_EVENT_WORK * bew;

  event = GMEVENT_Create(
      gsys, NULL, fieldBattleEvent, sizeof(BATTLE_EVENT_WORK) );

  bew = GMEVENT_GetEventWork(event);
  BEW_Initialize( bew, gsys, bp );

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

  BEW_Initialize( bew, gsys, bp );
  bew->bgm_pushed_flag = TRUE; //�����C�x���gBGM���ɑޔ��ς�
  bew->is_sub_event = TRUE;   //�X�N���v�g����Ă΂�遁�g�b�v���x���̃C�x���g�łȂ�

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

  switch (*seq) {
  case 0:
    // �퓬�p�a�f�l�Z�b�g
    if( bew->bgm_pushed_flag == FALSE ){
      FIELD_SOUND_PushBGM( GAMEDATA_GetFieldSound( gamedata ) );
      bew->bgm_pushed_flag = TRUE;
    }
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
      FIELD_SOUND_PopBGM( GAMEDATA_GetFieldSound( gamedata ) );
      bew->bgm_pushed_flag = FALSE;
    }

    //�퓬���ʔ��f����
    BEW_reflectBattleResult( bew, gamedata );

    //������������
    if (BEW_IsLoseResult( bew) == TRUE )
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
    GMEVENT_CallEvent(event, EVENT_FieldFadeIn(gsys, fieldmap, 0));
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
  case BTL_RESULT_CAPTURE:     ///< �߂܂����i�쐶�̂݁j
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
static void BEW_Initialize(BATTLE_EVENT_WORK * bew, GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM* bp)
{
  GFL_STD_MemClear32( bew, sizeof(BATTLE_EVENT_WORK) );
  bew->gsys = gsys;
  bew->gamedata = GAMESYSTEM_GetGameData( gsys );
  bew->battle_param = bp;
  bew->bgm_pushed_flag = FALSE;
  bew->is_sub_event = FALSE;
  bew->timeWait = 0;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void BEW_Destructor(BATTLE_EVENT_WORK * bew)
{
  BATTLE_PARAM_Delete( bew->battle_param );
}

