////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �d���f��������C�x���g
 * @file   event_look_elboard.c
 * @author obata
 * @date   2009.11.25
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "field_task_wait.h"
#include "field_gimmick_gate.h"
#include "event_look_elboard.h"


//==========================================================================================
// ���萔
//==========================================================================================
// �C�x���g�V�[�P���X
enum
{
  SEQ_INIT,               // �C�x���g������
  SEQ_CREATE_LOOK_TASK,   // �J�����ړ��^�X�N�̍쐬
  SEQ_WAIT_CAMERA_SET,    // �J�����̈ړ������҂�
  SEQ_KEY_WAIT,           // �L�[���͑҂�
  SEQ_CREATE_RESET_TASK,  // �J�������Z�b�g �^�X�N�̍쐬
  SEQ_WAIT_CAMERA_RESET,  // �J�������Z�b�g�����҂�
  SEQ_EXIT,               // �C�x���g�I��
};


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct
{
  GAMESYS_WORK*  gameSystem;
  FIELDMAP_WORK* fieldmap; 

  u16 cameraAnimeFrame;  // �J�����ړ��ɗv����t���[����

  // �J�����̕��A�p�����[�^
  u16     pitch;
  u16     yaw;
  fx32    length;
  VecFx32 targetOffset;

} EVENT_WORK;


//==========================================================================================
// ���C�x���g�����֐�
//==========================================================================================
static GMEVENT_RESULT SeasonDisplay( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*            work = (EVENT_WORK*)wk;
  FIELDMAP_WORK*     fieldmap = work->fieldmap;
  FIELD_CAMERA*        camera = FIELDMAP_GetFieldCamera( fieldmap );
  FIELD_TASK_MAN* taskManager = FIELDMAP_GetTaskManager( fieldmap );

  switch( *seq )
  {
  // �C�x���g������
  case SEQ_INIT:
    // �J�����̕��A�p�����[�^���擾
    work->pitch = FIELD_CAMERA_GetAnglePitch( camera );
    work->yaw = FIELD_CAMERA_GetAngleYaw( camera );
    work->length = FIELD_CAMERA_GetAngleLen( camera );
    FIELD_CAMERA_GetTargetOffset( camera, &work->targetOffset );
    ++(*seq);
    break;
  // �J�����ړ��^�X�N�̍쐬
  case SEQ_CREATE_LOOK_TASK:
    {
      u32 pitch, yaw;
      fx32 length;
      VecFx32 targetOffset; 
      // �J�����ŏI�p�����[�^����
      switch( GATE_GIMMICK_GetElboardDir(fieldmap) )
      {
      default:
        OBATA_Printf( "GIMMICK-GATE: �f���̌������J�����A�j�����Ή�\n" );
        GF_ASSERT(0);
      case DIR_DOWN:
        pitch  = 0x0ee5;
        yaw    = 0;
        length = 0x0086 << FX32_SHIFT;
        VEC_Set( &targetOffset, 0, 0x001b<<FX32_SHIFT, 0xfff94000 );
        break;
      case DIR_RIGHT: 
        pitch  = 0x0e93;
        yaw    = 0x4000;
        length = 0x0086 << FX32_SHIFT;
        VEC_Set( &targetOffset, 0xffff9000, 0x0028<<FX32_SHIFT, 0 );
        break;
      } 
      // �^�X�N�o�^
      {
        FIELD_TASK* wait;
        FIELD_TASK* prevZoom;
        FIELD_TASK* prevPitch;
        FIELD_TASK* zoomTask;
        FIELD_TASK* pitchTask;
        FIELD_TASK* yawTask;
        FIELD_TASK* targetOffsetTask;
        // ����
        wait      = FIELD_TASK_Wait( fieldmap, 40 );
        yawTask   = FIELD_TASK_CameraRot_Yaw( fieldmap, 60, yaw );
        prevPitch = FIELD_TASK_CameraRot_Pitch( fieldmap, 40, 0x1154 );
        targetOffsetTask = FIELD_TASK_CameraTargetOffset( fieldmap, 40, &targetOffset );
        prevZoom  = FIELD_TASK_CameraLinearZoom( fieldmap, 30, 0x0084<<FX32_SHIFT );
        zoomTask  = FIELD_TASK_CameraLinearZoom( fieldmap, 20, length );
        pitchTask = FIELD_TASK_CameraRot_Pitch( fieldmap, 20, pitch );
        // �o�^
        FIELD_TASK_MAN_AddTask( taskManager, wait, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, yawTask, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, prevZoom, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, prevPitch, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, targetOffsetTask, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, zoomTask, wait );
        FIELD_TASK_MAN_AddTask( taskManager, pitchTask, wait );
      }
    }
    ++(*seq);
    break;
  // �J�����̈ړ������҂�
  case SEQ_WAIT_CAMERA_SET:
    // �S�^�X�N�����I���Ŏ��̃V�[�P���X��
    if( FIELD_TASK_MAN_IsAllTaskEnd(taskManager) ){ ++(*seq); } 
    break;
  // �L�[���͑҂�
  case SEQ_KEY_WAIT:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( (trg & PAD_KEY_LEFT)  || (trg & PAD_KEY_RIGHT) ||
          (trg & PAD_KEY_DOWN)  || (trg & PAD_BUTTON_B) ){ ++(*seq); }
    }
    break;
  // �J�������Z�b�g �^�X�N�̍쐬
  case SEQ_CREATE_RESET_TASK:
    {
      // �^�X�N�o�^
      {
        FIELD_TASK* zoomTask;
        FIELD_TASK* pitchTask;
        FIELD_TASK* yawTask;
        FIELD_TASK* targetOffsetTask;
        zoomTask  = FIELD_TASK_CameraLinearZoom( fieldmap, work->cameraAnimeFrame, work->length );
        pitchTask = FIELD_TASK_CameraRot_Pitch( fieldmap, work->cameraAnimeFrame, work->pitch );
        yawTask   = FIELD_TASK_CameraRot_Yaw( fieldmap, work->cameraAnimeFrame, work->yaw );
        targetOffsetTask = FIELD_TASK_CameraTargetOffset( fieldmap, work->cameraAnimeFrame, &work->targetOffset );
        FIELD_TASK_MAN_AddTask( taskManager, zoomTask, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, pitchTask, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, yawTask, NULL );
        FIELD_TASK_MAN_AddTask( taskManager, targetOffsetTask, NULL );
      }
    }
    ++(*seq);
    break;
  // �J�������Z�b�g�����҂�
  case SEQ_WAIT_CAMERA_RESET:
    // �S�^�X�N�����I���Ŏ��̃V�[�P���X��
    if( FIELD_TASK_MAN_IsAllTaskEnd(taskManager) ){ ++(*seq); } 
    break;
  // �C�x���g�I��
  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//==========================================================================================
// ���O�����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �d���f��������C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap 
 * @param cameraAnimeFrame �J�����̈ړ��ɗv����t���[����
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_LookElboard( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, u16 cameraAnimeFrame )
{
  GMEVENT* event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gameSystem, NULL, SeasonDisplay, sizeof(EVENT_WORK) ); 
  // �C�x���g���[�N������
  work                   = GMEVENT_GetEventWork( event );
  work->gameSystem       = gameSystem;
  work->fieldmap         = fieldmap;
  work->cameraAnimeFrame = cameraAnimeFrame;
  return event;
} 
