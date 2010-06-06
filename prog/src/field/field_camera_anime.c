///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�J���� �A�j���[�V����
 * @file   field_camera_anime.c
 * @author obata
 * @date   2010.04.10
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field_camera_anime.h"

#include "fieldmap.h"
#include "field_task.h"  
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "field_task_target_pos.h"


//=============================================================================
// ���J�������샏�[�N
//=============================================================================
struct _FCAM_ANIME_WORK {

  FIELDMAP_WORK*         fieldmap;
  FIELD_CAMERA*          camera;
  const FCAM_ANIME_DATA* pAnimeData;

  // �J�������A�p�f�[�^
  BOOL              recoveryValidFlag;     // ���A�f�[�^���L�����ǂ���
  FIELD_CAMERA_MODE initialCameraMode;     // �J�������[�h
  BOOL              initialCameraAreaFlag; // �J�����G���A�̓���t���O
  const VecFx32*    initialWatchTarget;
  BOOL              recoveryWatchTargetFlag;

};


//=============================================================================
// ��private functions
//=============================================================================
// �����E�j���E������
static FCAM_ANIME_WORK* CreateWork( HEAPID heapID ); // �J�������샏�[�N�𐶐�����
static void DeleteWork( FCAM_ANIME_WORK* work ); // �J�������샏�[�N��j������
static void InitWork( FCAM_ANIME_WORK* work, FIELDMAP_WORK* fieldmap ); // �J�������샏�[�N������������
// �J�����̏����E���A
static void SetupCamera( FCAM_ANIME_WORK* work ); // �J�����̐ݒ��ύX����
static void SetupCameraMode( FCAM_ANIME_WORK* work ); // �J�������[�h�̐ݒ��ύX����
static void RecoverCamera( const FCAM_ANIME_WORK* work ); // �J�����̐ݒ�𕜋A����
static void AdjustCameraAngle( FIELD_CAMERA* camera ); // �J�����A���O�����Čv�Z����
static void SetCurrentCameraTargetPos( FIELD_CAMERA* camera ); // �^�[�Q�b�g���W�����݂̎����l�ɐݒ肷��
static void GetCurrentCameraParam( const FIELD_CAMERA* camera, FCAM_PARAM* dest ); // �J�����̌��݂̃p�����[�^���擾����
// �J�����A�j���[�V����
static void SetCameraParam( const FCAM_ANIME_WORK* work ); // �J�����𑦎����f����
static void StartCameraAnime( const FCAM_ANIME_WORK* work ); // �J�����̃A�j���[�V�������J�n����
// �A�N�Z�b�T
static FIELDMAP_WORK* GetFieldmap( const FCAM_ANIME_WORK* work ); // �t�B�[���h�}�b�v���擾����
static FIELD_CAMERA* GetCamera( const FCAM_ANIME_WORK* work ); // �J�������擾����
static const FCAM_ANIME_DATA* GetAnimeData( const FCAM_ANIME_WORK* work ); // �A�j���[�V�����f�[�^���擾����



//=============================================================================
// ��public functions
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ���݂̃J�����p�����[�^���擾����
 *
 * @param camera
 * @param dest   �擾�����p�����[�^�̊i�[��
 */
//-----------------------------------------------------------------------------
void FCAM_PARAM_GetCurrentParam( const FIELD_CAMERA* camera, FCAM_PARAM* dest )
{
  GetCurrentCameraParam( camera, dest );
}


//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N�𐶐�����
 *
 * @param fieldamp
 *
 * @return �����������[�N
 */
//-----------------------------------------------------------------------------
FCAM_ANIME_WORK* FCAM_ANIME_CreateWork( FIELDMAP_WORK* fieldmap )
{
  FCAM_ANIME_WORK* work;

  work = CreateWork( FIELDMAP_GetHeapID(fieldmap) ); // ����
  InitWork( work, fieldmap ); // ������
  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_DeleteWork( FCAM_ANIME_WORK* work )
{
  DeleteWork( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����f�[�^��ݒ肷��
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_SetAnimeData( FCAM_ANIME_WORK* work, const FCAM_ANIME_DATA* data )
{
  work->pAnimeData = data;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_SetupCamera( FCAM_ANIME_WORK* work )
{ 
  SetupCameraMode( work );
  AdjustCameraAngle( work->camera );
  SetCurrentCameraTargetPos( work->camera ); // �^�[�Q�b�g���W��������
  SetupCamera( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̏�Ԃ��J�n�p�����[�^�̐ݒ�ɂ���
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_SetCameraStartParam( const FCAM_ANIME_WORK* work )
{
  FIELD_CAMERA* camera;
  const FCAM_ANIME_DATA* anime;
  const FCAM_PARAM* param;

  camera = GetCamera( work );
  anime = GetAnimeData( work );
  param = &(anime->startParam);

  FIELD_CAMERA_SetAnglePitch( camera, param->pitch );
  FIELD_CAMERA_SetAngleYaw( camera, param->yaw );
  FIELD_CAMERA_SetAngleLen( camera, param->length );
  FIELD_CAMERA_SetTargetPos( camera, &param->targetPos );
  FIELD_CAMERA_SetTargetOffset( camera, &param->targetOffset );
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_StartAnime( const FCAM_ANIME_WORK* work )
{
  const FCAM_ANIME_DATA* anime;

  anime = GetAnimeData( work );

  // �������f
  if( anime->frame == 0 ) {
    SetCameraParam( work );
  }
  // �^�X�N��o�^
  else {
    StartCameraAnime( work );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����̏I�����`�F�b�N����
 *
 * @param work
 *
 * @param �A�j���[�V�������I�����Ă���ꍇ TRUE
 *        �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
BOOL FCAM_ANIME_IsAnimeFinished( const FCAM_ANIME_WORK* work )
{
  FIELD_TASK_MAN* taskMan;

  taskMan = FIELDMAP_GetTaskManager( work->fieldmap );

  if( FIELD_TASK_MAN_IsAllTaskEnd( taskMan ) ) { return TRUE; }
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����𕜋A����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void FCAM_ANIME_RecoverCamera( const FCAM_ANIME_WORK* work )
{
  if( work->recoveryValidFlag ) {
    RecoverCamera( work );
  }
} 




//=============================================================================
// ��private functions
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N�𐶐�����
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------
static FCAM_ANIME_WORK* CreateWork( HEAPID heapID )
{
  FCAM_ANIME_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(FCAM_ANIME_WORK) );

  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DeleteWork( FCAM_ANIME_WORK* work )
{
  GFL_HEAP_FreeMemory( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void InitWork( FCAM_ANIME_WORK* work, FIELDMAP_WORK* fieldmap )
{
  // �[���N���A
  GFL_STD_MemClear( work, sizeof(FCAM_ANIME_WORK) );

  // ������
  work->fieldmap = fieldmap;
  work->camera = FIELDMAP_GetFieldCamera( fieldmap );
  work->pAnimeData = NULL;
  work->recoveryValidFlag = FALSE;
  work->initialCameraAreaFlag = FALSE;
  work->initialWatchTarget = NULL;
  work->recoveryWatchTargetFlag = FALSE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J��������̏���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupCamera( FCAM_ANIME_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = work->camera;
  const FCAM_ANIME_DATA* anime = GetAnimeData( work );

  // ���[���V�X�e���̃J����������~
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    FLDNOGRID_MAPPER* NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
    FLDNOGRID_MAPPER_SetRailCameraActive( NGMapper, FALSE );
  }

  // �^�[�Q�b�g�̃o�C���h��OFF
  if( anime->targetBindOffFlag ) {
    if( FIELD_CAMERA_GetWatchTarget( work->camera ) != NULL ) {
      work->recoveryWatchTargetFlag = TRUE;
      work->initialWatchTarget = FIELD_CAMERA_GetWatchTarget( camera );
      FIELD_CAMERA_FreeTarget( camera );
    }
  }

  // �J�����G���A�𖳌���
  if( anime->cameraAreaOffFlag ) {
    work->initialCameraAreaFlag = FIELD_CAMERA_GetCameraAreaActive( camera );
    FIELD_CAMERA_SetCameraAreaActive( camera, FALSE );
  }

  // �J�����V�[�����~
  {
    FLD_SCENEAREA* sceneArea;
    sceneArea = FIELDMAP_GetFldSceneArea( fieldmap );
    FLD_SCENEAREA_SetActiveFlag( sceneArea, FALSE );
  }

  // �t���O�Z�b�g
  work->recoveryValidFlag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������[�h���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupCameraMode( FCAM_ANIME_WORK* work )
{
  FIELD_CAMERA* camera = work->camera;

  // �J�������[�h��ύX
  work->initialCameraMode = FIELD_CAMERA_GetMode( camera ); 
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̕��A������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void RecoverCamera( const FCAM_ANIME_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = work->camera;
  const FCAM_ANIME_DATA* anime = GetAnimeData( work );

  GF_ASSERT( work->recoveryValidFlag );

  // �J�����V�[���̍Ďn��
  {
    FLD_SCENEAREA* sceneArea;
    sceneArea = FIELDMAP_GetFldSceneArea( fieldmap );
    FLD_SCENEAREA_SetActiveFlag( sceneArea, TRUE );
  }

  // �J�����G���A�𕜋A
  if( anime->cameraAreaOffFlag ) {
    FIELD_CAMERA_SetCameraAreaActive( camera, work->initialCameraAreaFlag );
  }

  // �^�[�Q�b�g�����ɖ߂�
  if( work->recoveryWatchTargetFlag ) {
    FIELD_CAMERA_BindTarget( camera, work->initialWatchTarget );
    //FIELD_CAMERA_BindDefaultTarget( camera );
  }

  // �J�������[�h�𕜋A
  FIELD_CAMERA_ChangeMode( camera, work->initialCameraMode );

  // ���[���V�X�e���̃J��������𕜋A
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    FLDNOGRID_MAPPER* NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
    FLDNOGRID_MAPPER_SetRailCameraActive( NGMapper, TRUE );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����A���O�����Čv�Z����
 *
 * @param camera
 */
//-----------------------------------------------------------------------------
static void AdjustCameraAngle( FIELD_CAMERA* camera )
{
  u16 pitch, yaw;
  fx32 length;
  VecFx32 targetPos, cameraPos;
  VecFx32 toCameraVec;

  // �^�[�Q�b�g �� �J���� �x�N�g�����Z�o
  FIELD_CAMERA_GetCameraAreaAfterCameraPos( camera, &cameraPos );
  FIELD_CAMERA_GetCameraAreaAfterTargetPos( camera, &targetPos );
  VEC_Subtract( &cameraPos, &targetPos, &toCameraVec );

  // �A���O�����v�Z
  FIELD_CAMERA_CalcVecAngle( &toCameraVec, &pitch, &yaw, &length );

  // ���߂��A���O�����Z�b�g
  FIELD_CAMERA_SetAngleLen( camera, length );
  FIELD_CAMERA_SetAnglePitch( camera, pitch );
  FIELD_CAMERA_SetAngleYaw( camera, yaw );
}

//-----------------------------------------------------------------------------
/**
 * @brief �^�[�Q�b�g���W�����݂̎����l�ɐݒ肷��
 *
 * @param camera
 */
//-----------------------------------------------------------------------------
static void SetCurrentCameraTargetPos( FIELD_CAMERA* camera )
{
  VecFx32 pos;

  // �J�����G���A���l�������^�[�Q�b�g���W���擾
  FIELD_CAMERA_GetCameraAreaAfterTargetPos( camera, &pos );

  // �^�[�Q�b�g���W�ɔ��f
  FIELD_CAMERA_SetTargetPos( camera, &pos );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̌��݂̃p�����[�^���擾����
 *
 * @param camera
 * @param dest   �擾�����p�����[�^�̊i�[��
 */
//-----------------------------------------------------------------------------
static void GetCurrentCameraParam( const FIELD_CAMERA* camera, FCAM_PARAM* dest )
{
  VecFx32 targetPos, cameraPos;
  VecFx32 toCameraVec;

  // �A���O�����v�Z ( �J�����G���A���l�� )
  FIELD_CAMERA_GetCameraAreaAfterCameraPos( camera, &cameraPos );
  FIELD_CAMERA_GetCameraAreaAfterTargetPos( camera, &targetPos );
  VEC_Subtract( &cameraPos, &targetPos, &toCameraVec ); 
  FIELD_CAMERA_CalcVecAngle( 
      &toCameraVec, &(dest->pitch), &(dest->yaw), &(dest->length) );

  // �^�[�Q�b�g���W�E�^�[�Q�b�g�I�t�Z�b�g���擾 ( �J�����G���A���l�� )
  FIELD_CAMERA_GetCameraAreaAfterTargetPos( camera, &(dest->targetPos) );
  FIELD_CAMERA_GetTargetOffset( camera, &(dest->targetOffset) );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����𑦎����f����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetCameraParam( const FCAM_ANIME_WORK* work )
{
  FIELD_CAMERA*     camera   = work->camera;
  const FCAM_ANIME_DATA* anime    = GetAnimeData( work );
  const FCAM_PARAM* endParam = &(anime->endParam);

  GF_ASSERT( anime->frame == 0 ); // �������f�f�[�^�łȂ��Ă̓_��

  // �J�����ɔ��f������
  FIELD_CAMERA_SetAnglePitch( camera, endParam->pitch );
  FIELD_CAMERA_SetAngleYaw( camera, endParam->yaw );
  FIELD_CAMERA_SetAngleLen( camera, endParam->length );
  FIELD_CAMERA_SetTargetPos( camera, &(endParam->targetPos) );
  FIELD_CAMERA_SetTargetOffset( camera, &(endParam->targetOffset) );
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̃A�j���[�V�������J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void StartCameraAnime( const FCAM_ANIME_WORK* work )
{
  FIELDMAP_WORK*    fieldmap   = work->fieldmap;
  FIELD_CAMERA*     camera     = work->camera;
  const FCAM_ANIME_DATA* anime      = GetAnimeData( work );
  const FCAM_PARAM* startParam = &(anime->startParam);
  const FCAM_PARAM* endParam   = &(anime->endParam);

  GF_ASSERT( 0 < anime->frame ); // �A�j���p�f�[�^�łȂ��Ă̓_��

  // �^�X�N��o�^
  {
    FIELD_TASK* pitch;
    FIELD_TASK* yaw;
    FIELD_TASK* length;
    FIELD_TASK* TPos;
    FIELD_TASK* TOfs;

    // �^�X�N�𐶐�
    {
      u32 frame  = anime->frame;
      pitch  = FIELD_TASK_CameraRot_Pitch( fieldmap, frame, endParam->pitch );
      yaw    = FIELD_TASK_CameraRot_Yaw( fieldmap, frame, endParam->yaw );
      length = FIELD_TASK_CameraLinearZoom( fieldmap, frame, endParam->length ); 
      TPos   = FIELD_TASK_CameraTargetMove( fieldmap, frame, &(startParam->targetPos), &(endParam->targetPos) );
      TOfs   = FIELD_TASK_CameraTargetOffset( fieldmap, frame, &(endParam->targetOffset) );
    }

    // �^�X�N��o�^
    {
      FIELD_TASK_MAN* taskMan = FIELDMAP_GetTaskManager( fieldmap );
      FIELD_TASK_MAN_AddTask( taskMan, pitch, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, yaw, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, length, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, TPos, NULL );
      FIELD_TASK_MAN_AddTask( taskMan, TOfs, NULL );
    }
  }
}

//----------------------------------------------------------------------------- 
/**
 * @brief �t�B�[���h�}�b�v���擾����
 *
 * @param work
 *
 * @return FIELDMAP_WORK*
 */
//----------------------------------------------------------------------------- 
static FIELDMAP_WORK* GetFieldmap( const FCAM_ANIME_WORK* work )
{
  return work->fieldmap;
}

//----------------------------------------------------------------------------- 
/**
 * @brief �J�������擾����
 *
 * @param work
 *
 * @return FIELD_CAMERA* 
 */
//----------------------------------------------------------------------------- 
static FIELD_CAMERA* GetCamera( const FCAM_ANIME_WORK* work )
{
  return work->camera;
}

//----------------------------------------------------------------------------- 
/**
 * @brief �A�j���[�V�����f�[�^���擾����
 *
 * @param work
 *
 * @return �A�j���[�V�����f�[�^
 */
//----------------------------------------------------------------------------- 
static const FCAM_ANIME_DATA* GetAnimeData( const FCAM_ANIME_WORK* work )
{
  GF_ASSERT( work->pAnimeData );

  return work->pAnimeData;
}
