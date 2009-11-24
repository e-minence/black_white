////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �ޏ�C�x���g
 * @file   event_disappear.c
 * @author obata
 * @date   2009.08.28
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#include "include/field/fieldmap_proc.h"
#include "include/gamesystem/game_event.h"
#include "event_disappear.h"
#include "fieldmap.h"
#include "field_player.h"
#include "fldeff_kemuri.h"
#include "fldeff_iwakudaki.h"
#include "fldeff_gyoe.h"
#include "sound/pm_sndsys.h"
#include "field_task.h"
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_player_rot.h"
#include "field_task_player_trans.h"
#include "field_task_player_drawoffset.h"
#include "field_task_fade.h"


//==========================================================================================
// ���萔
//========================================================================================== 
#define ZOOM_IN_DIST   (70 << FX32_SHIFT)   // �J�����̃Y�[���C������
#define ZOOM_IN_FRAME  (30)   // �Y�[���C���Ɋ|����t���[����
#define ZOOM_OUT_FRAME (60)   // �Y�[���A�E�g�Ɋ|����t���[����
#define ANAHORI_EFF_INTVL (6)  //�u���Ȃ��ق�v�̃G�t�F�N�g�\���Ԋu


//==========================================================================================
// ���C�x���g���[�N
//==========================================================================================
typedef struct
{
  int                    frame;   // �t���[�����J�E���^
  FIELDMAP_WORK*      fieldmap;   // ����t�B�[���h�}�b�v
  FIELD_CAMERA_MODE cameraMode;   // �C�x���g�J�n���̃J�������[�h
  VecFx32        sandStreamPos;   // �������S���̈ʒu( �����C�x���g�̂ݎg�p )

} EVENT_WORK;


//==========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================== 
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_FallInSand( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Teleport( GMEVENT* event, int* seq, void* wk );


//========================================================================================== 
// �����J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���� )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 * @param stream_pos �������S���̍��W
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_FallInSand( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, const VecFx32* stream_pos )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_FallInSand, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap  = fieldmap;
  work->frame     = 0;
  VEC_Set( &work->sandStreamPos, stream_pos->x, stream_pos->y, stream_pos->z );

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���Ȃʂ��̃q�� )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Ananukenohimo( GMEVENT* parent, 
                                        GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Ananukenohimo, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap  = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���Ȃ��ق� )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Anawohoru( GMEVENT* parent, 
                                    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Anawohoru, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( �e���|�[�g )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Teleport( GMEVENT* parent, 
                                   GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Teleport, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work            = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap  = fieldmap;
  work->frame     = 0;

  // �쐬�����C�x���g��Ԃ�
  return event;
}


//========================================================================================== 
// ������J�֐�
//========================================================================================== 

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( ���� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_FallInSand( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );
  FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // �^�X�N�o�^
      FIELD_TASK* move;
      FIELD_TASK_MAN* man;
      move = FIELD_TASK_TransPlayer( work->fieldmap, 120, &work->sandStreamPos );
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, move, NULL );
    }
    // �т�����}�[�N�\��
    FLDEFF_GYOE_SetMMdlNonDepend( fectrl, mmdl, FLDEFF_GYOETYPE_GYOE, TRUE );
    PMSND_PlaySE( SEQ_SE_FLD_15 );
    work->frame = 0;
    ++( *seq );
    break;
  // �^�X�N�I���҂�&����
  case 1:
    { // ������ς���
      int key = GFL_UI_KEY_GetCont();
      if( key & PAD_KEY_UP )    MMDL_SetAcmd( mmdl, AC_STAY_WALK_U_4F );
      if( key & PAD_KEY_DOWN )  MMDL_SetAcmd( mmdl, AC_STAY_WALK_D_4F );
      if( key & PAD_KEY_LEFT )  MMDL_SetAcmd( mmdl, AC_STAY_WALK_L_4F );
      if( key & PAD_KEY_RIGHT ) MMDL_SetAcmd( mmdl, AC_STAY_WALK_R_4F );
    }
    // ����
    if( work->frame++ % 10 == 0 )
    {
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
    }
    // �^�X�N�I���`�F�b�N
    {
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        // �e������
        MMDL_OnStatusBit( mmdl, MMDL_STABIT_SHADOW_VANISH );     
        // �^�X�N�o�^
        { 
          VecFx32 vec;
          FIELD_TASK* move;
          FIELD_TASK* rot;
          FIELD_TASK* zoom;
          FIELD_TASK* fade_out;
          FIELD_TASK_MAN* man;
          VEC_Set( &vec, 0, -50<<FX32_SHIFT, 0 );
          move     = FIELD_TASK_TransDrawOffset( work->fieldmap, 80, &vec );
          rot      = FIELD_TASK_PlayerRotate( work->fieldmap, 80, 10 );
          zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, 30, -50<<FX32_SHIFT );
          fade_out = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
          man = FIELDMAP_GetTaskManager( work->fieldmap );
          FIELD_TASK_MAN_AddTask( man, move, NULL );
          FIELD_TASK_MAN_AddTask( man, rot, NULL );
          FIELD_TASK_MAN_AddTask( man, zoom, NULL );
          FIELD_TASK_MAN_AddTask( man, fade_out, zoom );
        }
        work->frame = 0;
        ++( *seq );
      }
    }
    break;
  // �^�X�N�̏I���҂�
  case 2:
    { 
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  // �J�������[�h�̕��A
  case 3:
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( ���Ȃʂ��̃q�� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // �^�X�N�o�^
      FIELD_TASK* rot_up;
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* fade_out;
      FIELD_TASK_MAN* man;
      rot_up   = FIELD_TASK_PlayerRotate_SpeedUp( work->fieldmap, 60, 10 );
      rot      = FIELD_TASK_PlayerRotate( work->fieldmap, 60, 20 );
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, -ZOOM_IN_DIST );
      fade_out = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1 );
      man      = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( man, rot, rot_up );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_out, zoom );
    }
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( ���Ȃ��ق� )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // �^�X�N�o�^
      FIELD_TASK* rot_up;
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* fade_out;
      FIELD_TASK_MAN* man;
      rot_up   = FIELD_TASK_PlayerRotate_SpeedUp( work->fieldmap, 30, 10 );
      rot      = FIELD_TASK_PlayerRotate( work->fieldmap, 60, 20 );
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, -ZOOM_IN_DIST );
      fade_out = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1 );
      man      = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( man, rot, rot_up );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_out, rot_up );
    }
    work->frame = 0;
    ++( *seq );
    break;
  case 1:
    if( ++work->frame % ANAHORI_EFF_INTVL == 0 )
    { // ��ӂ��G�t�F�N�g�\��
      MMDL*          mmdl = FIELD_PLAYER_GetMMdl( player );
      FLDEFF_CTRL* fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
      FLDEFF_IWAKUDAKI_SetMMdl( mmdl, fectrl );
    }
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( �e���|�[�g )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Teleport( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case 0:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    { // �^�X�N�o�^
      FIELD_TASK* rot_up;
      FIELD_TASK* rot;
      FIELD_TASK* zoom;
      FIELD_TASK* fade_out;
      FIELD_TASK_MAN* man;
      rot_up   = FIELD_TASK_PlayerRotate_SpeedUp( work->fieldmap, 30, 10 );
      rot      = FIELD_TASK_PlayerRotate( work->fieldmap, 60, 20 );
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, -ZOOM_IN_DIST );
      fade_out = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
      man      = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( man, rot, rot_up );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_out, rot_up );
    }
    // SE
    PMSND_PlaySE( SEQ_SE_FLD_05 );
    ++( *seq );
    break;
  case 1:
    { // �^�X�N�̏I���҂�
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )
      {
        ++( *seq );
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    ++( *seq );
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
