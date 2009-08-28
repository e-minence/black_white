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


//==========================================================================================
/**
 * @brief �萔
 */
//==========================================================================================

#define ZOOM_IN_DIST   (50 << FX32_SHIFT)   // �J�����̃Y�[���C������
#define ZOOM_IN_FRAME  (60)   // �Y�[���C���Ɋ|����t���[����
#define ZOOM_OUT_FRAME (60)   // �Y�[���A�E�g�Ɋ|����t���[����


//==========================================================================================
/**
 * @brief �C�x���g���[�N
 */
//==========================================================================================
typedef struct
{
  int                frame;   // �t���[�����J�E���^
  FIELDMAP_WORK* pFieldmap;   // ����t�B�[���h�}�b�v
}
EVENT_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
// �o��C�x���g�����֐�
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Rotate( GMEVENT* event, int* seq, void* work );      // ��]
static GMEVENT_RESULT EVENT_FUNC_APPEAR_RollingFall( GMEVENT* event, int* seq, void* work ); // ��]����
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* work );        // ����


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g���쐬����( ���[�v )
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_APPEAR_Warp( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  //event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_Rotate, sizeof( EVENT_WORK ) );
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_APPEAR_RollingFall, sizeof( EVENT_WORK ) );

  // �C�x���g���[�N��������
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->pFieldmap = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
} 

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
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->pFieldmap = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
}


//========================================================================================== 
/**
 * @brief ����J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ��] )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Rotate( GMEVENT* event, int* seq, void* work )
{
  EVENT_WORK*       ew = (EVENT_WORK*)work;
  GFL_TCBSYS*   tcbsys = FIELDMAP_GetFieldmapTCBSys( ew->pFieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( ew->pFieldmap );

  switch( *seq )
  {
  // �t�F�[�h�C���J�n
  case 0:
    FIELD_CAMERA_SetAngleLen( camera, FIELD_CAMERA_GetAngleLen( camera ) - ZOOM_IN_DIST );// �J���������ݒ�
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 1 );
    ++( *seq );
    break;
  // �^�X�N�̒ǉ�
  case 1:
    FIELDMAP_TCB_AddTask_RotatePlayer_SpeedDown( ew->pFieldmap, 120, 10 );   // ���@��]
    FIELDMAP_TCB_AddTask_CameraZoom( ew->pFieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );  // �Y�[���A�E�g
    ++( *seq );
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 120 < ew->frame++ )
    {
      ++( *seq );
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �o��C�x���g�����֐�( ��]���� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_APPEAR_RollingFall( GMEVENT* event, int* seq, void* work )
{
  EVENT_WORK*       ew = (EVENT_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( ew->pFieldmap );

  switch( *seq )
  {
  // �^�X�N�̒ǉ�
  case 0:
    FIELDMAP_TCB_AddTask_AppearPlayer_LinearDown( ew->pFieldmap, 40, 300 );  // ���@�ړ�
    FIELDMAP_TCB_AddTask_RotatePlayer_SpeedDown( ew->pFieldmap, 60, 10 );    // ���@��]
    ++( *seq );
    break;
  // �t�F�[�h�C���J�n
  case 1:
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0 );
    ++( *seq );
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 60 < ew->frame++ )
    {
      ++( *seq );
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
static GMEVENT_RESULT EVENT_FUNC_APPEAR_Fall( GMEVENT* event, int* seq, void* work )
{
  EVENT_WORK*       ew = (EVENT_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( ew->pFieldmap );

  switch( *seq )
  {
  // �^�X�N�̒ǉ�
  case 0:
    FIELDMAP_TCB_AddTask_AppearPlayer_Fall( ew->pFieldmap, 40, 250 );  // ���@�ړ�
    ++( *seq );
    break;
  // �t�F�[�h�C���J�n
  case 1:
    GFL_FADE_SetMasterBrightReq(
        GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 16, 0, 0 );
    ++( *seq );
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 60 < ew->frame++ )
    {
      ++( *seq );
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
} 
