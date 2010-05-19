////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �|�P�������[�O�t�����g�ł̃C�x���g
 * @file   event_league_front02.c
 * @author obata
 * @date   2010.04.09
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "fieldmap.h"
#include "gmk_tmp_wk.h"
#include "field_gimmick_league_front01.h"
#include "gamesystem/game_event.h"
#include "gamesystem/gamesystem.h"
#include "savedata/gimmickwork.h"  // for GIMMICKWORK
#include "system/ica_anime.h"  // for ICA_ANIME
#include "field_gimmick_def.h"  // for FLD_GIMMICK_C09P01
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "fld_exp_obj.h"  // for FLD_EXP_OBJ
#include "arc/league_front.naix"  // for NARC_xxxx
#include "../../../resource/fldmapdata/zonetable/zone_id.h"  // for ZONE_ID_C09P02
#include "event_mapchange.h"  // for EVENT_ChangeMapPosNoFade
#include "event_fieldmap_control.h"  // for EVENT_FieldBrightOut
#include "field_gimmick_league_front02.h"  // for EVENT_LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent
#include "field/eventwork.h"  // for EVENTWORK_
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  // for SYS_FLAG_BIGFOUR_xxxx
#include "sound/pm_sndsys.h" // for PMSND_xxxx
#include "event_camera_shake.h"
#include "event_league_front02.h"


//==========================================================================================
// ���e�I�u�W�F�̔z�u
//========================================================================================== 
#define HALF_GRID ((FIELD_CONST_GRID_SIZE / 2) << FX32_SHIFT)
#define PLAYER_OFS_X (0)
#define PLAYER_OFS_Y (0)
#define PLAYER_OFS_Z ((FIELD_CONST_GRID_SIZE * 2) << FX32_SHIFT)


//==========================================================================================
// �� �C�x���g
//==========================================================================================
typedef struct {

  HEAPID         heap_id;
  GAMESYS_WORK*  gsys;
  FIELDMAP_WORK* fieldmap;
  FIELD_CAMERA*  camera;
  ICA_ANIME*     liftAnime;  // ���t�g�̈ړ��A�j���f�[�^

} LIFTDOWN_EVENTWORK;

//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�̈ړ��A�j���[�V�������X�V����
 *
 * @return ���I�������� TRUE, �����łȂ���΁@FALSE
 */
//------------------------------------------------------------------------------------------
static BOOL UpdateLiftMove( LIFTDOWN_EVENTWORK* work )
{
  BOOL anime_end;

  // �A�j���[�V������i�߂�
  anime_end = ICA_ANIME_IncAnimeFrame( work->liftAnime, FX32_ONE );

  return anime_end;
}

//------------------------------------------------------------------------------------------
/**
 * @brief  ���t�g�̍��W���X�V����
 */
//------------------------------------------------------------------------------------------
static void UpdateLiftPos( LIFTDOWN_EVENTWORK* work )
{ 
  VecFx32 trans;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GFL_G3D_OBJSTATUS* objstatus;

  ICA_ANIME_GetTranslate( work->liftAnime, &trans );
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF02_EXUNIT_GIMMICK, LF02_EXOBJ_LIFT );
  objstatus->trans.y = trans.y; 
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̍��W�����t�g�̓����ɍ��킹��
 */
//------------------------------------------------------------------------------------------
static void SetPlayerOnLift( LIFTDOWN_EVENTWORK* work )
{
  FIELD_PLAYER* player;
  VecFx32 pos, trans;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GFL_G3D_OBJSTATUS* objstatus;
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );
  objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF02_EXUNIT_GIMMICK, LF02_EXOBJ_LIFT );
  player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  VEC_Set( &pos, 
           objstatus->trans.x + PLAYER_OFS_X, 
           objstatus->trans.y + PLAYER_OFS_Y, 
           objstatus->trans.z + PLAYER_OFS_Z );
  // x, z���W���O���b�h�̒��S�ɂȂ�悤�ɒ���
  pos.x = GRID_TO_FX32( FX32_TO_GRID(pos.x) ) + HALF_GRID;
  pos.z = GRID_TO_FX32( FX32_TO_GRID(pos.z) ) + HALF_GRID;
  FIELD_PLAYER_SetPos( player, &pos );
}


//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g����
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT LiftDownEvent( GMEVENT* event, int* seq, void* wk )
{
  int i;
  LIFTDOWN_EVENTWORK* work = (LIFTDOWN_EVENTWORK*)wk;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( work->fieldmap );

  switch( *seq ) {
  case 0:
    // �ړ��A�j���f�[�^�ǂݍ���
    work->liftAnime = ICA_ANIME_CreateAlloc( work->heap_id, 
        ARCID_LEAGUE_FRONT_GIMMICK, NARC_league_front_pl_ele_01_ica_bin );
    // ���t�g�Ǝ��@�̍��W��������
    UpdateLiftPos( work );
    SetPlayerOnLift( work );
    // �J�����̃g���[�X������~���N�G�X�g���s
    FIELD_CAMERA_StopTraceRequest( work->camera );
    (*seq)++;
    break;

  case 1:
    // �J�����̃g���[�X�����I���҂�
    if( FIELD_CAMERA_CheckTrace( work->camera ) == FALSE ) { (*seq)++; }
    break;

  case 2:
    // ���t�g�̃A�j���[�V�����J�n
    LEAGUE_FRONT_02_GIMMICK_StartLiftAnime( work->fieldmap );
    // ���t�g�ғ��G�t�F�N�g��\������
    LEAGUE_FRONT_02_GIMMICK_ShowLiftEffect( work->fieldmap );
    // �t�F�[�h�C��
    {
      GMEVENT* new_event;
      new_event = EVENT_FieldFadeIn_Black( work->gsys, work->fieldmap, FIELD_FADE_WAIT );
      GMEVENT_CallEvent( event, new_event );
    }
    (*seq)++;
    break;

  // �A�j���I���҂�
  case 3:
    {
      BOOL anime_end;

      // ���t�g�𓮂���
      anime_end = UpdateLiftMove( work );

      if( !anime_end ) {
        UpdateLiftPos( work );   // ���t�g�̈ʒu���X�V
        SetPlayerOnLift( work ); // ���@�����t�g�ɍ��킹��
      } 

      // �A�j���I��
      if( anime_end ) { 
        ICA_ANIME_Delete( work->liftAnime );
        PMSND_StopSE(); // �G���x�[�^�ғ������~
        PMSND_PlaySE( SEQ_SE_FLD_94 ); // �G���x�[�^������
        // �J�����U���C�x���g���R�[��
        {
          CAM_SHAKE_PARAM shakeParam;
          shakeParam.Width           = 2;
          shakeParam.Height          = 0;
          shakeParam.Sync            = 3;
          shakeParam.Time            = 2;
          shakeParam.TimeMax         = 2;
          shakeParam.NowSync         = 0;
          shakeParam.SubW            = 0;
          shakeParam.SubH            = 1;
          shakeParam.SubStartTime    = 0;
          shakeParam.SubMargineCount = 0;
          shakeParam.SubMargine      = 2;
          GMEVENT_CallEvent( event, EVENT_ShakeCamera( work->gsys, &shakeParam ) );
        }
        (*seq)++;
      } 
    }
    break;
    
  // ���t�g�̃A�j���I���҂�
  case 4:
    // ���t�g�{�̂ƃG�t�F�N�g�̃A�j�����I��
    if( LEAGUE_FRONT_02_GIMMICK_CheckLiftAnimeEnd( work->fieldmap ) &&
        LEAGUE_FRONT_02_GIMMICK_CheckLiftEffectEnd( work->fieldmap ) ) {
      LEAGUE_FRONT_02_GIMMICK_StopLiftAnime( work->fieldmap ); // ���t�g�̃A�j�����~�߂�
      LEAGUE_FRONT_02_GIMMICK_HideLiftEffect( work->fieldmap ); // ���t�g�ғ��G�t�F�N�g������
      (*seq)++;
    }
    break;

  // �I������
  case 5:
    // ���@�𒅒n������
    {
      VecFx32 pos;
      fx32 height;
      FIELD_PLAYER* player;
      MMDL* mmdl;
      player = FIELDMAP_GetFieldPlayer( work->fieldmap );
      mmdl   = FIELD_PLAYER_GetMMdl( player );
      FIELD_PLAYER_GetPos( player, &pos );
      MMDL_GetMapPosHeight( mmdl, &pos, &height );
      pos.y = height;
      FIELD_PLAYER_SetPos( player, &pos );
    }
    (*seq)++;
    break;

  case 6:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g���쐬����
 * 
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return ���t�g�~���C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_LFRONT02_LiftDown( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  LIFTDOWN_EVENTWORK* evwork;

  // ����
  event = GMEVENT_Create( gsys, NULL, LiftDownEvent, sizeof(LIFTDOWN_EVENTWORK) );

  // ������
  evwork = GMEVENT_GetEventWork( event );
  evwork->heap_id   = FIELDMAP_GetHeapID( fieldmap );
  evwork->gsys      = gsys;
  evwork->fieldmap  = fieldmap;
  evwork->camera    = FIELDMAP_GetFieldCamera( fieldmap );
  evwork->liftAnime = NULL;
  return event;
}
