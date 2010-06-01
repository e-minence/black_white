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
#include "field_task_wait.h"
#include "field_task_anahori_effect.h"
#include "field_buildmodel.h"
#include "field/field_const.h"  // for FIELD_CONST_GRID_SIZE
#include "field_camera_anime.h"
#include "event_field_fade.h" // for EVENT_FieldFadeOut_xxxx


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
typedef struct {

  GAMESYS_WORK*     gameSystem;
  FIELDMAP_WORK*    fieldmap;
  FIELD_CAMERA_MODE camera_mode; // �C�x���g�J�n���̃J�������[�h
  int               frame;       // �t���[�����J�E���^
  BOOL              teleport_f;  // �e���|�[�g�ƃ��j�I�����[���ޏꂪ�����C�x���g�Ȃ̂ŁASE����̂��߃t���O������(100601 iwasawa)

  // �����C�x���g
  VecFx32       sandStreamPos; // �������S���̈ʒu
  FIELD_BMODEL* bmSandStream;  // �����̔z�u���f��

  // �J�����A�j���[�V����
  FCAM_ANIME_DATA  FCamAnimeData;
  FCAM_ANIME_WORK* FCamAnimeWork;

  // �t�F�[�h�A�E�g
  int  wait_count;         // �t�F�[�h�A�E�g�J�n�҂��J�E���^
  BOOL season_change_flag; // �G�߂��ω��������ǂ���

} EVENT_WORK;


//==========================================================================================
// ���v���g�^�C�v�錾
//========================================================================================== 
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_FallInSand( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Teleport( GMEVENT* event, int* seq, void* wk );
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Warp( GMEVENT* event, int* seq, void* wk );

// ���Ȃʂ��̃q��
static void RegisterPlayerEffectTask_Ananukenohimo( EVENT_WORK* work ); // ���@�̉��o�^�X�N��o�^����
static void StartCameraAnime_Ananukenohimo( EVENT_WORK* work ); // �J�������o���J�n����
static void FinishCameraAnime_Ananukenohimo( EVENT_WORK* work ); // �J�������o���I������
static GMEVENT* GetFadeOutEvent_Ananukenohimo( EVENT_WORK* work ); // �t�F�[�h�A�E�g�̃C�x���g���擾����
// ���Ȃ��ق�
static void RegisterPlayerEffectTask_Anawohoru( EVENT_WORK* work ); // ���@�̉��o�^�X�N��o�^����
static void StartCameraAnime_Anawohoru( EVENT_WORK* work ); // �J�������o���J�n����
static void FinishCameraAnime_Anawohoru( EVENT_WORK* work ); // �J�������o���I������
static GMEVENT* GetFadeOutEvent_Anawohoru( EVENT_WORK* work ); // �t�F�[�h�A�E�g�̃C�x���g���擾����


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
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem   = gsys;
  work->fieldmap     = fieldmap;
  work->frame        = 0;
  work->bmSandStream = NULL;
  VEC_Set( &work->sandStreamPos, stream_pos->x, stream_pos->y, stream_pos->z );

  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���Ȃʂ��̃q�� )
 *
 * @param parent             �e�C�x���g
 * @param gsys               �Q�[���V�X�e��
 * @param fieldmap           �t�B�[���h�}�b�v
 * @param season_change_flag �G�߂��ω��������ǂ���
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Ananukenohimo( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, BOOL season_change_flag )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Ananukenohimo, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem         = gsys;
  work->fieldmap           = fieldmap;
  work->frame              = 0;
  work->wait_count         = 0;
  work->season_change_flag = season_change_flag;

  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���Ȃ��ق� )
 *
 * @param parent             �e�C�x���g
 * @param gsys               �Q�[���V�X�e��
 * @param fieldmap           �t�B�[���h�}�b�v
 * @param season_change_flag �G�߂��ω��������ǂ���
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Anawohoru( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, BOOL season_change_flag )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Anawohoru, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem         = gsys;
  work->fieldmap           = fieldmap;
  work->frame              = 0;
  work->wait_count         = 0;
  work->season_change_flag = season_change_flag;

  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( �e���|�[�g/���j�I�����[������̑ޏꌓ�p )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 * @param teleport_f  �e���|�[�g�ł̌Ăяo���Ȃ�TRUE ������SE����Ɏg�p
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Teleport( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, BOOL teleport_f )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Teleport, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem = gsys;
  work->fieldmap   = fieldmap;
  work->frame      = 0;
  work->teleport_f = teleport_f;

  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g���쐬����( ���[�v )
 *
 * @param parent     �e�C�x���g
 * @param gsys       �Q�[���V�X�e��
 * @param fieldmap   �t�B�[���h�}�b�v
 *
 * @return �쐬�����C�x���g
 */
//------------------------------------------------------------------------------------------
GMEVENT* EVENT_DISAPPEAR_Warp( GMEVENT* parent, 
                               GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap )
{
  GMEVENT*   event;
  EVENT_WORK* work;

  // �C�x���g���쐬
  event = GMEVENT_Create( gsys, parent, EVENT_FUNC_DISAPPEAR_Warp, sizeof(EVENT_WORK) );

  // �C�x���g���[�N��������
  work = (EVENT_WORK*)GMEVENT_GetEventWork( event );
  work->gameSystem = gsys;
  work->fieldmap   = fieldmap;
  work->frame      = 0;

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
enum {
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

  switch( *seq ) {
  case SEQ_INIT:
    // �J�������[�h�̐ݒ�
    work->camera_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    *seq = SEQ_SAND_ANIME_START;
    break;

  // �����A�j���[�V�����J�n
  case SEQ_SAND_ANIME_START:
    // SE
    {
      SEPLAYER_ID player_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_FLD_15 );
      PMSND_PlaySE_byPlayerID( SEQ_SE_FLD_15, player_id );
    }
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
      if( objst[0] ) {
        work->bmSandStream = FIELD_BMODEL_Create( taskMan, objst[0] );        // ����
        FIELD_BMODEL_SetAnime( work->bmSandStream, SAND_ANM_IDX, BMANM_REQ_START );  // �Đ�
      }
      else {
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
      if( 0 < animeFrame ) {
        FIELD_TASK* move;
        FIELD_TASK_MAN* taskMan;
        move = FIELD_TASK_TransPlayer( work->fieldmap, animeFrame, &work->sandStreamPos );
        taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
        FIELD_TASK_MAN_AddTask( taskMan, move, NULL );
        *seq = SEQ_PLAYER_DRAW_ANIME_WAIT;
      }
      else {
        *seq = SEQ_PLAYER_SALLOW_ANIME_START;
      }
    }
    work->frame = 0;
    break;

  // �^�X�N�I���҂�&����
  case SEQ_PLAYER_DRAW_ANIME_WAIT:
    // �����A�j�����I��������Đ�����
    if( work->bmSandStream ) {
      if( FIELD_BMODEL_GetAnimeStatus( work->bmSandStream, SAND_ANM_IDX ) ) {
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
    if( work->frame++ % 10 == 0 ) {
      FLDEFF_KEMURI_SetMMdl( mmdl, fectrl );
    }
    // �^�X�N�I���`�F�b�N
    {
      FIELD_TASK_MAN* taskMan;
      taskMan = FIELDMAP_GetTaskManager( work->fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) {
        *seq = SEQ_PLAYER_SALLOW_ANIME_START;
      }
    }
    break;

  // ���@�����ݍ��܂��A�j���[�V�����J�n
  case SEQ_PLAYER_SALLOW_ANIME_START:
    // SE�Đ�
    PMSND_PlaySE( SEQ_SE_FLD_147 );
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
      if( FIELD_TASK_MAN_IsAllTaskEnd(taskMan) ) { *seq = SEQ_EXIT; }
    }
    break;

  // �I��
  case SEQ_EXIT:
    // SE ��~
    {
      SEPLAYER_ID player_id = PMSND_GetSE_DefaultPlayerID( SEQ_SE_FLD_15 );
      PMSND_StopSE_byPlayerID( player_id );
    }
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->camera_mode );
    // �o�^�����z�u���f��������
    if( work->bmSandStream ) {
      FIELD_BMODEL_Delete( work->bmSandStream );
    }
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}


//==========================================================================================
/**
 * @brief �ޏ�C�x���g�����֐� ( ���Ȃʂ��̃q�� )
 */
//==========================================================================================
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Ananukenohimo( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work    = (EVENT_WORK*)wk;
  FIELD_TASK_MAN* taskMan = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    // ���o�J�n
    RegisterPlayerEffectTask_Ananukenohimo( work ); // ���@�̉��o�^�X�N��o�^
    StartCameraAnime_Ananukenohimo( work ); // �J�������o�J�n
    (*seq)++;
    break;

  case 1:
    // �t�F�[�h�A�E�g�J�n�҂�
    if( 35 < work->wait_count++ ) {
      GMEVENT_CallEvent( event, GetFadeOutEvent_Ananukenohimo( work ) );
      (*seq)++;
      break;
    }

  case 2:
    // �S���o�^�X�N�̏I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) {
      PMSND_PlaySE( SEQ_SE_KAIDAN ); //�u�U�b�U�b�U�b�v
      (*seq)++;
    }
    break;
    
  case 3:
    FinishCameraAnime_Ananukenohimo( work ); // �J�������o�I��
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̉��o�^�X�N��o�^���� ( ���Ȃʂ��̃q�� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void RegisterPlayerEffectTask_Ananukenohimo( EVENT_WORK* work )
{
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( fieldmap );

  FIELD_TASK* rot_up;
  FIELD_TASK* rot;

  rot_up = FIELD_TASK_PlayerRotate_SpeedUp( fieldmap, 20, 3 );
  rot    = FIELD_TASK_PlayerRotate( fieldmap, 20, 5 );
  FIELD_TASK_MAN_AddTask( taskMan, rot_up, NULL );
  FIELD_TASK_MAN_AddTask( taskMan, rot, rot_up );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�������o���J�n���� ( ���Ȃʂ��̃q�� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void StartCameraAnime_Ananukenohimo( EVENT_WORK* work )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  work->FCamAnimeWork = FCAM_ANIME_CreateWork( work->fieldmap );
  work->FCamAnimeData.frame = 60;
  work->FCamAnimeData.targetBindOffFlag = TRUE;
  work->FCamAnimeData.cameraAreaOffFlag = TRUE;
  FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
  FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
  FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.startParam );
  work->FCamAnimeData.endParam = work->FCamAnimeData.startParam;
  work->FCamAnimeData.endParam.length -= (100 << FX32_SHIFT);
  FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
  FCAM_ANIME_StartAnime( work->FCamAnimeWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�������o���I������ ( ���Ȃʂ��̃q�� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void FinishCameraAnime_Ananukenohimo( EVENT_WORK* work )
{
  FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
  FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g�̃C�x���g���Ăяo�� ( ���Ȃʂ��̃q�� )
 *
 * @param work
 *
 * @return �t�F�[�h�A�E�g�C�x���g
 */
//------------------------------------------------------------------------------------------
static GMEVENT* GetFadeOutEvent_Ananukenohimo( EVENT_WORK* work )
{
  GAMESYS_WORK*  gameSystem = work->gameSystem;
  FIELDMAP_WORK* fieldmap   = work->fieldmap;
  GMEVENT* event;

  // �G�߂��ω�
  if( work->season_change_flag ) { 
    event = EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ); // �G�߃t�F�[�h
  }
  else { 
    event = EVENT_FieldFadeOut_White( gameSystem, fieldmap, FIELD_FADE_WAIT ); // �z���C�g�A�E�g
  }

  return event;
}

//==========================================================================================
/**
 * @brief �ޏ�C�x���g�����֐�( ���Ȃ��ق� )
 */
//==========================================================================================
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Anawohoru( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work    = (EVENT_WORK*)wk;
  FIELD_TASK_MAN* taskMan = FIELDMAP_GetTaskManager( work->fieldmap );

  switch( *seq ) {
  case 0:
    RegisterPlayerEffectTask_Anawohoru( work ); // ���@�̉��o�^�X�N��o�^
    StartCameraAnime_Anawohoru( work ); // �J�������o�J�n
    PMSND_PlaySE( SEQ_SE_FLD_80 ); // SE �Đ�
    (*seq)++;
    break;

  case 1:
    // �t�F�[�h�A�E�g�J�n�҂�
    if( 35 < work->wait_count++ ) {
      GMEVENT_CallEvent( event, GetFadeOutEvent_Anawohoru( work ) );
      (*seq)++;
      break;
    }

  case 2:
    // �S�^�X�N�I��
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) {
      PMSND_PlaySE( SEQ_SE_KAIDAN ); //�u�U�b�U�b�U�b�v
      (*seq)++;
    }
    break;

  case 3:
    FinishCameraAnime_Anawohoru( work ); // �J�������o�I��
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̉��o�^�X�N��o�^���� ( ���Ȃ��ق� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void RegisterPlayerEffectTask_Anawohoru( EVENT_WORK* work )
{
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( fieldmap );
  FIELD_PLAYER*   player   = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL*           mmdl     = FIELD_PLAYER_GetMMdl( player );

  FIELD_TASK* effect;
  FIELD_TASK* rot_up;
  FIELD_TASK* rot;

  effect = FIELD_TASK_AnahoriEffect( fieldmap, mmdl, 40, ANAHORI_EFF_INTVL );
  rot_up = FIELD_TASK_PlayerRotate_SpeedUp( fieldmap, 20, 4 );
  rot    = FIELD_TASK_PlayerRotate( fieldmap, 20, 5 );

  FIELD_TASK_MAN_AddTask( taskMan, effect, NULL );
  FIELD_TASK_MAN_AddTask( taskMan, rot_up, NULL );
  FIELD_TASK_MAN_AddTask( taskMan, rot, rot_up );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�������o���J�n���� ( ���Ȃ��ق� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void StartCameraAnime_Anawohoru( EVENT_WORK* work )
{
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( work->fieldmap );

  work->FCamAnimeWork = FCAM_ANIME_CreateWork( work->fieldmap );
  work->FCamAnimeData.frame = 60;
  work->FCamAnimeData.targetBindOffFlag = TRUE;
  work->FCamAnimeData.cameraAreaOffFlag = TRUE;
  FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
  FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
  FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.startParam );
  work->FCamAnimeData.endParam = work->FCamAnimeData.startParam;
  work->FCamAnimeData.endParam.length -= (100 << FX32_SHIFT);
  FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
  FCAM_ANIME_StartAnime( work->FCamAnimeWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �J�������o���I������ ( ���Ȃ��ق� )
 *
 * @param work
 */
//------------------------------------------------------------------------------------------
static void FinishCameraAnime_Anawohoru( EVENT_WORK* work )
{
  FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
  FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �t�F�[�h�A�E�g�̃C�x���g���Ăяo�� ( ���Ȃ��ق� )
 *
 * @param work
 *
 * @return �t�F�[�h�A�E�g�C�x���g
 */
//------------------------------------------------------------------------------------------
static GMEVENT* GetFadeOutEvent_Anawohoru( EVENT_WORK* work )
{
  GAMESYS_WORK*  gameSystem = work->gameSystem;
  FIELDMAP_WORK* fieldmap   = work->fieldmap;
  GMEVENT* event;

  // �G�߂��ω�
  if( work->season_change_flag ) { 
    event = EVENT_FieldFadeOut_Season( gameSystem, fieldmap, FIELD_FADE_WAIT ); // �G�߃t�F�[�h
  }
  else { 
    event = EVENT_FieldFadeOut_White( gameSystem, fieldmap, FIELD_FADE_WAIT ); // �z���C�g�A�E�g
  }

  return event;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( �e���|�[�g )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Teleport( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*     work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK*  fieldmap = work->fieldmap;
  FIELD_PLAYER*   player   = FIELDMAP_GetFieldPlayer( work->fieldmap );
  FIELD_CAMERA*   camera   = FIELDMAP_GetFieldCamera( work->fieldmap );
  FIELD_TASK_MAN* taskMan  = FIELDMAP_GetTaskManager( fieldmap );

  switch( *seq )
  {
  case 0:
    // �^�X�N�o�^
    {
      FIELD_TASK* rot_up;
      FIELD_TASK* rot;
      FIELD_TASK* wait;
      FIELD_TASK* fade_out;
      rot_up   = FIELD_TASK_PlayerRotate_SpeedUp( fieldmap, 20, 3 );
      rot      = FIELD_TASK_PlayerRotate( fieldmap, 20, 5 );
      wait     = FIELD_TASK_Wait( fieldmap, ZOOM_IN_FRAME );
      fade_out = FIELD_TASK_Fade( fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
      FIELD_TASK_MAN_AddTask( taskMan, rot_up, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, rot, rot_up );
      FIELD_TASK_MAN_AddTask( taskMan, wait, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, fade_out, wait );
    }
    // SE
    if( work->teleport_f ){
      PMSND_PlaySE( SEQ_SE_FLD_86 );  //�e���|�[�g�͐�pSE
    }else{
      PMSND_PlaySE( SEQ_SE_FLD_05 );  //���̑��͒ʏ탏�[�v��
    }
    // �J�������o�J�n
    work->FCamAnimeWork = FCAM_ANIME_CreateWork( fieldmap );
    work->FCamAnimeData.frame = 60;
    work->FCamAnimeData.targetBindOffFlag = TRUE;
    work->FCamAnimeData.cameraAreaOffFlag = TRUE;
    FCAM_ANIME_SetAnimeData( work->FCamAnimeWork, &work->FCamAnimeData );
    FCAM_ANIME_SetupCamera( work->FCamAnimeWork );
    FCAM_PARAM_GetCurrentParam( camera, &work->FCamAnimeData.startParam );
    work->FCamAnimeData.endParam = work->FCamAnimeData.startParam;
    work->FCamAnimeData.endParam.length -= (100 << FX32_SHIFT);
    FCAM_ANIME_SetCameraStartParam( work->FCamAnimeWork );
    FCAM_ANIME_StartAnime( work->FCamAnimeWork );
    (*seq)++;
    break;

  case 1:
    // �^�X�N�̏I���҂�
    if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { (*seq)++; }
    break;

  case 2:
    FCAM_ANIME_RecoverCamera( work->FCamAnimeWork );
    FCAM_ANIME_DeleteWork( work->FCamAnimeWork );
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �ޏ�C�x���g�����֐�( ���[�v )
 */
//------------------------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_FUNC_DISAPPEAR_Warp( GMEVENT* event, int* seq, void* wk )
{
  EVENT_WORK*    work     = (EVENT_WORK*)wk;
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_PLAYER*  player   = FIELDMAP_GetFieldPlayer( fieldmap );
  FIELD_CAMERA*  camera   = FIELDMAP_GetFieldCamera( fieldmap );

  switch( *seq ) {
  case 0:
    // �J�������[�h�̐ݒ�
    work->camera_mode = FIELD_CAMERA_GetMode( camera );
    FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    // �^�X�N�o�^
    { 
      FIELD_TASK* rotTask;
      FIELD_TASK* fadeOutTask;
      FIELD_TASK* moveTask;
      FIELD_TASK_MAN* taskMan;
      VecFx32 moveVec;
      VEC_Set( &moveVec, 0, 150<<FX32_SHIFT, 0 );
      rotTask = FIELD_TASK_PlayerRotate( fieldmap, 24, 3 );
      fadeOutTask = FIELD_TASK_Fade( fieldmap, GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1 );
      moveTask = FIELD_TASK_TransDrawOffset( fieldmap, 24, &moveVec );
      taskMan = FIELDMAP_GetTaskManager( fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, rotTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, moveTask, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, fadeOutTask, moveTask );
    }
    // SE
    PMSND_PlaySE( SEQ_SE_FLD_05 );
    (*seq)++;
    break;
  case 1:
    // �^�X�N�̏I���҂�
    { 
      FIELD_TASK_MAN* man;
      man = FIELDMAP_GetTaskManager( fieldmap );
      if( FIELD_TASK_MAN_IsAllTaskEnd(man) && !GFL_FADE_CheckFade() ) { 
        (*seq)++; 
      }
    }
    break;
  case 2:
    // �J�������[�h�̕��A
    FIELD_CAMERA_ChangeMode( camera, work->camera_mode );
    (*seq)++;
    break;
  case 3:
    return GMEVENT_RES_FINISH;
  } 
  return GMEVENT_RES_CONTINUE;
}
