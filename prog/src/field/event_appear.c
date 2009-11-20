////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �ޏꁕ�o��C�x���g
 * @file   event_appear.c
 * @author obata
 * @date   2009.08.28
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "event_appear.h"
#include "fieldmap.h"
#include "field_player.h"
#include "fieldmap_tcb.h"
#include "sound/pm_sndsys.h"
#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_player_rot.h"
#include "field_task_player_fall.h"


//==========================================================================================
// ���萔
//========================================================================================== 
#define ZOOM_IN_DIST   (50 << FX32_SHIFT)   // �J�����̃Y�[���C������
#define ZOOM_IN_FRAME  (60)   // �Y�[���C���Ɋ|����t���[����
#define ZOOM_OUT_FRAME (60)   // �Y�[���A�E�g�Ɋ|����t���[����


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct
{
  // ����t�B�[���h�}�b�v
  FIELDMAP_WORK* fieldmap;   

} EVENT_WORK;


//==========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================== 
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Rotate( GMEVENT* event, int* seq, void* wk );   // ��]
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* wk );     // ����


//========================================================================================== 
// �����J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���� )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Fall( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Fall, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work           = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;

  // �쐬�����C�x���g��Ԃ�
  return event;
}


//========================================================================================== 
// ������J�֐��̒�`
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ��] )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Rotate( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    { // �J���������ݒ�
      FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );
      FIELD_CAMERA_SetAngleLen( camera, FIELD_CAMERA_GetAngleLen( camera ) - ZOOM_IN_DIST );
    }
    // �t�F�[�h�C���J�n
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 1 );
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̒ǉ�
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK_MAN* man;
      rot  = FIELD_TASK_PlayerRotate_SpeedDown( work->fieldmap, 120, 10 );
      zoom = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, rot, NULL );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
    }
    ++( *seq );
    break;
  case 2:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ���� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  case 0:
    { // �^�X�N�̒ǉ�
      FIELD_TASK* fall;
      FIELD_TASK_MAN* man;
      fall = FIELD_TASK_PlayerFall( work->fieldmap, 40, 250 );
      man  = FIELDMAP_GetTaskManager( work->fieldmap ); 
      FIELD_TASK_MAN_AddTask( man, fall, NULL );
    }
    ++( *seq );
    break;
  case 1:
    // �t�F�[�h�C���J�n
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0 );
    ++( *seq );
    break;
  case 2:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap ); 
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 3: 
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
