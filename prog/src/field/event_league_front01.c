////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief �|�P�������[�O�t�����g�ł̃C�x���g
 * @file   event_league_front01.c
 * @author obata
 * @date   2010.04.08
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include <gflib.h>
#include "fieldmap.h"
#include "field_gimmick_league_front01.h"

#include "gamesystem/game_event.h"
#include "gamesystem/gamesystem.h"
#include "savedata/gimmickwork.h"  // for GIMMICKWORK
#include "system/ica_anime.h"  // for ICA_ANIME

#include "fld_exp_obj.h"  // for FLD_EXP_OBJ
#include "arc/league_front.naix"  // for NARC_xxxx
#include "../../../resource/fldmapdata/zonetable/zone_id.h"  // for ZONE_ID_C09P02
#include "event_mapchange.h"  // for EVENT_ChangeMapPosNoFade
#include "event_fieldmap_control.h"  // for EVENT_FieldBrightOut
#include "field_gimmick_league_front02.h"  // for EVENT_LEAGUE_FRONT_02_GIMMICK_GetLiftDownEvent
#include "eventwork.h"  // for EVENTWORK_
#include "../../../resource/fldmapdata/flagwork/flag_define.h"  // for SYS_FLAG_BIGFOUR_xxxx
#include "sound/pm_sndsys.h" // for PMSND_xxxx
#include "event_league_front01.h"
#include "event_league_front02.h"
#include "field_task.h"  
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "field_task_target_pos.h"

//==========================================================================================
// ���萔
//==========================================================================================
// �l�V�����C�g�����t�g�ƈꏏ�ɍ~�����邩�ǂ���
#define LIGHT_DOWN
// ���t�g�A�b�v���̃p�����[�^
#define LIFT_UP_FRAME  (20)
#define LIFT_UP_PITCH  (0x2eff)
#define LIFT_UP_LENGTH (0x0115 << FX32_SHIFT)


//==========================================================================================
// �� �C�x���g���[�N
//==========================================================================================
typedef struct {

  GAMESYS_WORK*   gameSystem;
  FIELDMAP_WORK*  fieldmap;
  FIELD_CAMERA*   camera;
  FIELD_TASK_MAN* taskMan;
  ICA_ANIME*      liftAnime;  // ���t�g�̈ړ��A�j���f�[�^

} EVENT_WORK;


//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g����
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT LiftDownEvent( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work       = (EVENT_WORK*)wk;
  GAMESYS_WORK*   gameSystem = work->gameSystem;
  FIELDMAP_WORK*  fieldmap   = work->fieldmap;
  FIELD_CAMERA*   camera     = work->camera;
  FIELD_TASK_MAN* taskMan    = work->taskMan;

  switch( *seq ) {
  // �J�����ړ��J�n
  case 0:
  {
    FIELD_TASK* pitch;
    FIELD_TASK* length; 
    // �^�X�N�𐶐�
    pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, LIFT_UP_FRAME, LIFT_UP_PITCH );
    //length = FIELD_TASK_CameraLinearZoom( fieldmap, LIFT_UP_FRAME, LIFT_UP_LENGTH ); 
    // �^�X�N��o�^
    FIELD_TASK_MAN_AddTask( taskMan, pitch, NULL );
    //FIELD_TASK_MAN_AddTask( taskMan, length, pitch );
  }
    (*seq)++;
    break;

  // �J�����ړ��I���҂�
  case 1:
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;

  // ���t�g�ړ��J�n
  case 2:
    {
      HEAPID heapID;
      heapID = FIELDMAP_GetHeapID( fieldmap );
      work->liftAnime = ICA_ANIME_CreateAlloc( heapID, ARCID_LEAGUE_FRONT_GIMMICK, NARC_league_front_pl_ele_00_ica_bin );
    }
    PMSND_PlaySE( SEQ_SE_FLD_148 ); // �G���x�[�^�ғ����J�n
    ++(*seq);
    break;

  // ���t�g�ړ��I���҂�
  case 3:
    // ���@, ���t�g�̍��W���X�V
    {
      VecFx32 trans, pos;
      FIELD_PLAYER* player;
      GFL_G3D_OBJSTATUS* objstatus;
      FLD_EXP_OBJ_CNT_PTR exobj_cnt;
      // ���t�g
      ICA_ANIME_GetTranslate( work->liftAnime, &trans );
      exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF01_EXUNIT_GIMMICK, LF01_EXOBJ_LIFT );
      objstatus->trans.y = trans.y;
#ifdef LIGHT_DOWN
      // ���C�g
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF01_EXUNIT_GIMMICK, LF01_EXOBJ_LIGHT_FIGHT );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF01_EXUNIT_GIMMICK, LF01_EXOBJ_LIGHT_EVIL );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF01_EXUNIT_GIMMICK, LF01_EXOBJ_LIGHT_GHOST );
      objstatus->trans.y = trans.y;
      objstatus = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, LF01_EXUNIT_GIMMICK, LF01_EXOBJ_LIGHT_ESPER );
      objstatus->trans.y = trans.y;
#endif
      // ���@
      player = FIELDMAP_GetFieldPlayer( fieldmap );
      FIELD_PLAYER_GetPos( player, &pos );
      pos.y = trans.y;
      FIELD_PLAYER_SetPos( player, &pos );
    }

    // �A�j���[�V�����X�V
    if( ICA_ANIME_IncAnimeFrame( work->liftAnime, FX32_ONE ) ) { 
      ICA_ANIME_Delete( work->liftAnime );
      ++(*seq); 
    }
    break;

  // �t�F�[�h�A�E�g
  case 4:
    {
      GMEVENT* new_event;
      new_event = EVENT_FieldFadeOut_Black( gameSystem, fieldmap, FIELD_FADE_WAIT );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;

  // �}�b�v�`�F���W
  case 5:
    {
      GMEVENT* new_event;
      VecFx32 pos;
      VEC_Set( &pos, 0, 0, 0 );
      new_event = EVENT_ChangeMapPosNoFade( gameSystem, fieldmap, ZONE_ID_C09P02, &pos, DIR_DOWN );
      GMEVENT_CallEvent( event, new_event );
    }
    ++(*seq);
    break;

  // ���̃C�x���g�֐؂�ւ�
  case 6:
    {
      GMEVENT* next_event;
      next_event = EVENT_LFRONT02_LiftDown( gameSystem, fieldmap );
      GMEVENT_ChangeEvent( event, next_event );
    }
    ++(*seq);
    break;
  // �I��
  case 7:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g�𐶐�����
 * 
 * @param gameSystem
 * @param fieldmap
 *
 * @return ���t�g�~���C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_LFRONT01_LiftDown( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap )
{
  GMEVENT* event;
  EVENT_WORK* evwork;

  // �C�x���g�𐶐�
  event = GMEVENT_Create( gameSystem, NULL, LiftDownEvent, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  evwork = GMEVENT_GetEventWork( event );
  evwork->gameSystem = gameSystem;
  evwork->fieldmap   = fieldmap;
  evwork->liftAnime  = NULL;
  evwork->camera     = FIELDMAP_GetFieldCamera( fieldmap );
  evwork->taskMan    = FIELDMAP_GetTaskManager( fieldmap );
  return event;
} 
