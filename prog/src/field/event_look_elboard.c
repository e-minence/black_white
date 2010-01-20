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
#include "field_gimmick_gate.h"
#include "event_look_elboard.h"


//==========================================================================================
// ���萔
//==========================================================================================
// �C�x���g�V�[�P���X
enum
{
  SEQ_INIT,               // �C�x���g������
  SEQ_WAIT_CAMERA_SET,    // �J�����̈ړ������҂�
  SEQ_KEY_WAIT,           // �L�[���͑҂�
  SEQ_WAIT_CAMERA_RESET,  // �J�������Z�b�g�����҂�
  SEQ_EXIT,               // �C�x���g�I��
};


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap; 
  u16 cameraMoveFrame;  // �J�����ړ��ɗv����t���[����

} EVENT_WORK;


//==========================================================================================
// ���C�x���g�����֐�
//==========================================================================================
static GMEVENT_RESULT SeasonDisplay( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK* work = (EVENT_WORK*)wk;

  switch( *seq )
  {
  // �C�x���g������
  case SEQ_INIT:
    // �J�����ړ��^�X�N��o�^
    GATE_GIMMICK_Camera_LookElboard( work->fieldmap, work->cameraMoveFrame );
    ++(*seq);
    break;
  // �J�����̈ړ������҂�
  case SEQ_WAIT_CAMERA_SET:
    { // �S�^�X�N�����I���Ŏ��̃V�[�P���X��
      FIELD_TASK_MAN* man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) ){ ++(*seq); }
    }
    break;
  // �L�[���͑҂�
  case SEQ_KEY_WAIT:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( (trg & PAD_KEY_LEFT)  ||
          (trg & PAD_KEY_RIGHT) ||
          (trg & PAD_KEY_DOWN)  ||
          (trg & PAD_BUTTON_B) )
      {
        // ���Z�b�g�^�X�N��o�^
        GATE_GIMMICK_Camera_Reset( work->fieldmap, work->cameraMoveFrame );
        ++(*seq);
      }
    }
    break;
  // �J�������Z�b�g�����҂�
  case SEQ_WAIT_CAMERA_RESET:
    { // �S�^�X�N�����I���Ŏ��̃V�[�P���X��
      FIELD_TASK_MAN* man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) ){ ++(*seq); }
    }
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
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 * @param frame    �J�����̈ړ��ɗv����t���[����
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_LookElboard( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u16 frame)
{
  GMEVENT* event;
  EVENT_WORK* work;

  // �C�x���g����
  event = GMEVENT_Create( gsys, NULL, SeasonDisplay, sizeof(EVENT_WORK) ); 
  // �C�x���g���[�N������
  work                  = GMEVENT_GetEventWork( event );
  work->gsys            = gsys;
  work->fieldmap        = fieldmap;
  work->cameraMoveFrame = frame;
  return event;
} 
