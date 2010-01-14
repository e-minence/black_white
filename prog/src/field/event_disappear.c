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
#include "field_buildmodel.h"
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE


//==========================================================================================
// ���萔
//========================================================================================== 
#define ZOOM_IN_DIST   (180 << FX32_SHIFT)   // �J�����Y�[�����̋���
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

  // �����C�x���g
  VecFx32      sandStreamPos;  // �������S���̈ʒu
  FIELD_BMODEL* bmSandStream;  // �����̔z�u���f��

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
  work               = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->fieldmap     = fieldmap;
  work->frame        = 0;
  work->bmSandStream = NULL;
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
// �����̃A�j���ԍ�
#define SAND_ANM_IDX (0)  

// �V�[�P���X�ԍ�
enum
{
  SEQ_INIT,                       // �����ݒ�
  SEQ_SAND_ANIME_START,           // �����A�j���J�n
  SEQ_PLAYER_DRAW_ANIME_START,    // ���@���������܂��ړ��J�n
  SEQ_PLAYER_DRAW_ANIME_WAIT,     // ���@���������܂��ړ��I���҂�
  SEQ_PLAYER_SALLOW_ANIME_START,  // ���@�����ݍ��܂�鉉�o�J�n
  SEQ_PLAYER_SALLOW_ANIME_WAIT,   // ���@�����ݍ��܂�鉉�o�I��
  SEQ_EXIT,                       // �I������
};

static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_FallInSand( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work = (EVENT_WORK*)wk;
  FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );
  FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( work->fieldmap );
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  switch( *seq )
  {
  case SEQ_INIT:
    // �J�������[�h�̐ݒ�
    work->cameraMode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    *seq = SEQ_SAND_ANIME_START;
    break;
  // �����A�j���[�V�����J�n
  case SEQ_SAND_ANIME_START:
    // SE
    PMSND_PlaySE( SEQ_SE_FLD_15 );
    // �z�u���f���̃A�j���Đ�
    {
      FLDMAPPER* mapper = FIELDMAP_GetFieldG3Dmapper( work->fieldmap );
      FIELD_BMODEL_MAN* taskMan = FLDMAPPER_GetBuildModelManager( mapper );
      G3DMAPOBJST** objst;
      u32 objnum;
      FLDHIT_RECT rect;
      // �z�u���f��(����)������
      rect.left   = work->sandStreamPos.x - (FIELD_CONST_GRID_SIZE*2 << FX32_SHIFT);
      rect.right  = work->sandStreamPos.x + (FIELD_CONST_GRID_SIZE*2 << FX32_SHIFT);
      rect.top    = work->sandStreamPos.z - (FIELD_CONST_GRID_SIZE*2 << FX32_SHIFT);
      rect.bottom = work->sandStreamPos.z + (FIELD_CONST_GRID_SIZE*2 << FX32_SHIFT);
      objst = FIELD_BMODEL_MAN_CreateObjStatusList( taskMan, &rect, BM_SEARCH_ID_SANDSTREAM, &objnum ); 
      // �z�u���f���𕡐���, �A�j���[�V�����Đ�
      if( objst[0] )
      {
        work->bmSandStream = FIELD_BMODEL_Create( taskMan, objst[0] );        // ����
        FIELD_BMODEL_SetAnime( work->bmSandStream, SAND_ANM_IDX, BMANM_REQ_START );  // �Đ�
      }
      else
      {
        work->bmSandStream = NULL;
      }
      GFL_HEAP_FreeMemory( objst );
    }
    *seq = SEQ_PLAYER_DRAW_ANIME_START;
    break;
  // ���@���������܂��ړ��J�n
  case SEQ_PLAYER_DRAW_ANIME_START:
    // ���@�ړ��^�X�N�o�^
    { 
      int animeFrame;
      // �A�j���[�V�����t���[����������
      { 
        VecFx32 playerPos;
        int playerGX, playerGZ;
        int sandStreamGX, sandStreamGZ;
        int distX, distZ;
        FIELD_PLAYER_GetPos( player, &playerPos );
        playerGX = FX32_TO_GRID( playerPos.x );
        playerGZ = FX32_TO_GRID( playerPos.z );
        sandStreamGX = FX32_TO_GRID( work->sandStreamPos.x );
        sandStreamGZ = FX32_TO_GRID( work->sandStreamPos.z );
        distX = playerGX - sandStreamGX;
        distZ = playerGZ - sandStreamGZ;
        if( distX < 0 ){ distX = -distX; }
        if( distZ < 0 ){ distZ = -distZ; }
        if( (distX == 0) && (distZ == 0) ){ animeFrame = 0; }
        else if( (distX < 2) && (distZ < 2) ){ animeFrame = 60; }
        else{ animeFrame = 120; }
      }
      // �^�X�N�o�^
      if( 0 < animeFrame )
      {
        FIELD_TASK* move;
        FIELD_TASK_MAN* taskMan;
        move = FIELD_TASK_TransPlayer( work->fieldmap, animeFrame, &work->sandStreamPos );
        taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
        FIELD_TASK_MAN_AddTask( taskMan, move, NULL );
        *seq = SEQ_PLAYER_DRAW_ANIME_WAIT;
      }
      else
      {
        *seq = SEQ_PLAYER_SALLOW_ANIME_START;
      }
    }
    work->frame = 0;
    break;
  // �^�X�N�I���҂�&����
  case SEQ_PLAYER_DRAW_ANIME_WAIT:
    // �����A�j�����I��������Đ�����
    if( work->bmSandStream )
    {
      if( FIELD_BMODEL_GetAnimeStatus( work->bmSandStream, SAND_ANM_IDX ) )  // if(��~��)
      {
        FIELD_BMODEL_SetAnime( work->bmSandStream, SAND_ANM_IDX, BMANM_REQ_START );
      }
    }
    // ���@�̌�����ς���
    { 
      int key = GFL_UI_KEY_GetCont();
      if( key & PAD_KEY_UP )    { MMDL_SetAcmd( mmdl, AC_STAY_WALK_U_4F ); }
      if( key & PAD_KEY_DOWN )  { MMDL_SetAcmd( mmdl, AC_STAY_WALK_D_4F ); }
      if( key & PAD_KEY_LEFT )  { MMDL_SetAcmd( mmdl, AC_STAY_WALK_L_4F ); }
      if( key & PAD_KEY_RIGHT ) { MMDL_SetAcmd( mmdl, AC_STAY_WALK_R_4F ); }
    }
    // ����
    if( work->frame++ % 10 == 0 )
    {
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
    }
    // �^�X�N�I���`�F�b�N
    {
      FIELD_TASK_MAN* taskMan;
      taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) )
      {
        *seq = SEQ_PLAYER_SALLOW_ANIME_START;
      }
    }
    break;
  // ���@�����ݍ��܂��A�j���[�V�����J�n
  case SEQ_PLAYER_SALLOW_ANIME_START:
    // �e������
    MMDL_OnMoveBit( mmdl, MMDL_MOVEBIT_SHADOW_VANISH );
    // �^�X�N�o�^
    { 
      VecFx32 moveVec;
      FIELD_TASK* moveTask;
      FIELD_TASK* rotTask;
      FIELD_TASK* zoomTask;
      FIELD_TASK* fadeTask;
      FIELD_TASK_MAN* taskMan;
      VEC_Set( &moveVec, 0, -30<<FX32_SHIFT, 0 );
      moveTask = FIELD_TASK_TransDrawOffset( work->fieldmap, 34, &moveVec );
      rotTask  = FIELD_TASK_PlayerRotate( work->fieldmap, 34, 5 );
      zoomTask = FIELD_TASK_CameraLinearZoom( work->fieldmap, 30, ZOOM_IN_DIST );
      fadeTask = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
      taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, moveTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, rotTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, zoomTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, fadeTask, zoomTask );
    }
    work->frame = 0;
    *seq = SEQ_PLAYER_SALLOW_ANIME_WAIT;
    break;
  // �^�X�N�̏I���҂�
  case SEQ_PLAYER_SALLOW_ANIME_WAIT:
    { 
      FIELD_TASK_MAN* taskMan;
      taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) )
      {
        *seq = SEQ_EXIT;
      }
    }
    break;
  // �I��
  case SEQ_EXIT:
    // SE ��~
    PMSND_StopSE();
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->cameraMode );
    // �o�^�����z�u���f��������
    if( work->bmSandStream )
    {
      FIELD_BMODEL_Delete( work->bmSandStream );
    }
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
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, ZOOM_IN_DIST );
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
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )  // if(�S�^�X�N�I��)
      {
        PMSND_PlaySE( SEQ_SE_KAIDAN ); //�u�U�b�U�b�U�b�v
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
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, ZOOM_IN_DIST );
      fade_out = FIELD_TASK_Fade( work->fieldmap, GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1 );
      man      = FIELDMAP_GetTaskManager( work->fieldmap );
      FIELD_TASK_MAN_AddTask( man, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( man, rot, rot_up );
      FIELD_TASK_MAN_AddTask( man, zoom, NULL );
      FIELD_TASK_MAN_AddTask( man, fade_out, rot_up );
      // SE�Đ�
      PMSND_PlaySE( SEQ_SE_FLD_80 );
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
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) )  // if(�S�^�X�N�I��)
      {
        PMSND_PlaySE( SEQ_SE_KAIDAN ); //�u�U�b�U�b�U�b�v
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
      zoom     = FIELD_TASK_CameraLinearZoom( work->fieldmap, ZOOM_IN_FRAME, ZOOM_IN_DIST );
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
