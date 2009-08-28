#include <math.h>     // sqrt
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "event_warp.h"
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
  FIELDMAP_WORK* pFieldmap;
  int frame;
}
WARP_WORK;


//==========================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//========================================================================================== 
// �C�x���g�����֐�
GMEVENT_RESULT WarpInEvent_Rotate( GMEVENT* event, int* seq, void* work );
GMEVENT_RESULT WarpInEvent_Jump( GMEVENT* event, int* seq, void* work ); 
GMEVENT_RESULT WarpOutEvent_Rotate( GMEVENT* event, int* seq, void* work );
GMEVENT_RESULT WarpOutEvent_Jump( GMEVENT* event, int* seq, void* work );


//========================================================================================== 
/**
 * @brief ���J�֐��̒�`
 */
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief ���[�v�o��C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_WarpIn( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  WARP_WORK* work;

  // �C�x���g���쐬
  //event = GMEVENT_Create( gsys, parent, WarpInEvent_Rotate, sizeof( WARP_WORK ) );
  event = GMEVENT_Create( gsys, parent, WarpInEvent_Jump, sizeof( WARP_WORK ) );

  // �C�x���g���[�N��������
  work            = (WARP_WORK*)GMEVENT_GetEventWork( event );
  work->pFieldmap = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���[�v�ޏ�C�x���g���쐬����
 *
 * @param parent   �e�C�x���g
 * @param gsys     �Q�[���V�X�e��
 * @param fieldmap �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_WarpOut( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  WARP_WORK* work;

  // �C�x���g���쐬
  //event = GMEVENT_Create( gsys, parent, WarpOutEvent_Rotate, sizeof( WARP_WORK ) );
  event = GMEVENT_Create( gsys, parent, WarpOutEvent_Jump, sizeof( WARP_WORK ) );

  // �C�x���g���[�N��������
  work            = (WARP_WORK*)GMEVENT_GetEventWork( event );
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
 * @brief �ޏ�C�x���g�����֐�( ��] )
 */
//------------------------------------------------------------------------------------------
GMEVENT_RESULT WarpOutEvent_Rotate( GMEVENT* event, int* seq, void* work )
{
  WARP_WORK*    ww     = (WARP_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( ww->pFieldmap );

  switch( *seq )
  {
  // ���@��]�^�X�N�̒ǉ�
  case 0:
    FIELDMAP_TCB_ROT_PLAYER_AddTask_SpeedUp( ww->pFieldmap, 120, 10 );    // ���@��]
    FIELDMAP_TCB_CAMERA_AddTask_Zoom( ww->pFieldmap, ZOOM_IN_FRAME, -ZOOM_IN_DIST ); // �Y�[���C��
    ++( *seq );
    break;
  // �t�F�[�h�A�E�g�J�n
  case 1:
    if( 100 < ww->frame++ )
    {
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 1 );
      ++( *seq );
    }
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 120 < ww->frame++ )
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
 * @brief �o��C�x���g�����֐�( ��] )
 */
//------------------------------------------------------------------------------------------
GMEVENT_RESULT WarpInEvent_Rotate( GMEVENT* event, int* seq, void* work )
{
  WARP_WORK*        ww = (WARP_WORK*)work;
  GFL_TCBSYS*   tcbsys = FIELDMAP_GetFieldmapTCBSys( ww->pFieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( ww->pFieldmap );

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
    FIELDMAP_TCB_ROT_PLAYER_AddTask_SlowDown( ww->pFieldmap, 120, 10 );   // ���@��]
    FIELDMAP_TCB_CAMERA_AddTask_Zoom( ww->pFieldmap, ZOOM_OUT_FRAME, ZOOM_IN_DIST );  // �Y�[���A�E�g
    ++( *seq );
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 120 < ww->frame++ )
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
 * @brief �ޏ�C�x���g�����֐�( �W�����v )
 */
//------------------------------------------------------------------------------------------
GMEVENT_RESULT WarpOutEvent_Jump( GMEVENT* event, int* seq, void* work )
{
  WARP_WORK*    ww     = (WARP_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( ww->pFieldmap );

  switch( *seq )
  {
  // �^�X�N�̒ǉ�
  case 0:
    FIELDMAP_TCB_WARP_PLAYER_AddTask_DisappearUp( ww->pFieldmap, 60, 300 );   // ���@�ړ�
    FIELDMAP_TCB_ROT_PLAYER_AddTask_SpeedUp( ww->pFieldmap, 30, 10 );         // ���@��]
    ++( *seq );
    break;
  // �t�F�[�h�A�E�g�J�n
  case 1:
    if( 30 < ww->frame++ )
    {
      GFL_FADE_SetMasterBrightReq(
          GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB, 0, 16, 0 );
      ++( *seq );
    }
    break;
  // �^�X�N�̏I���҂�
  case 2:
    if( 60 < ww->frame++ )
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
 * @brief �o��C�x���g�����֐�( �W�����v )
 */
//------------------------------------------------------------------------------------------
GMEVENT_RESULT WarpInEvent_Jump( GMEVENT* event, int* seq, void* work )
{
  WARP_WORK*    ww     = (WARP_WORK*)work;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( ww->pFieldmap );

  switch( *seq )
  {
  // �^�X�N�̒ǉ�
  case 0:
    FIELDMAP_TCB_WARP_PLAYER_AddTask_AppearDown( ww->pFieldmap, 40, 300 );  // ���@�ړ�
    FIELDMAP_TCB_ROT_PLAYER_AddTask_SlowDown( ww->pFieldmap, 60, 10 );      // ���@��]
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
    if( 60 < ww->frame++ )
    {
      ++( *seq );
    }
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}





