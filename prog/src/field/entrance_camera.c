///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ����o������̃J�������o�f�[�^
 * @file   entrance_camera.c
 * @author obata
 * @date   2010.02.16
 */
///////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "entrance_camera.h"

#include "fieldmap.h"
#include "field_task.h"  
#include "field_task_manager.h"
#include "field_task_camera_zoom.h"
#include "field_task_camera_rot.h"
#include "field_task_target_offset.h"
#include "field_task_target_pos.h"

#include "field/field_const.h"     // for FIELD_CONST_xxxx
#include "field/eventdata_type.h"  // for EXIT_TYPE_xxxx
#include "arc/arc_def.h"           // for ARCID_xxxx
#include "../../resource/fldmapdata/entrance_camera/entrance_camera.naix"  // for NARC_xxxx



//=============================================================================
// ���萔
//=============================================================================
#define DEBUG_PRINT_ENABLE    // �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_TARGET (3)// �f�o�b�O�o�͐�

#define ZERO_FRAME 
//--------------------------------------------
// �ʏ�o������̃J�������� ( �[���t���[���� )
//--------------------------------------------
#ifdef ZERO_FRAME 
// ��h�A�ւ̏o����
#define U_DOOR_FRAME  (10)
#define U_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// ���h�A�ւ̏o����
#define L_DOOR_FRAME  (0)
#define L_DOOR_PITCH  (0x1fa1)
#define L_DOOR_YAW    (0x4000)
#define L_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// �E�h�A�ւ̏o����
#define R_DOOR_FRAME  (0)
#define R_DOOR_PITCH  (0x1fa1)
#define R_DOOR_YAW    (0xc051)
#define R_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
//-------------------------------------
// �ʏ�o������̃J�������� ( �ʏ�� )
//-------------------------------------
#else 
// ��h�A�ւ̏o����
#define U_DOOR_FRAME  (10)
#define U_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// ���h�A�ւ̏o����
#define L_DOOR_FRAME  (20)
#define L_DOOR_PITCH  (0x1fa1)
#define L_DOOR_YAW    (0x0e0f)
#define L_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// �E�h�A�ւ̏o����
#define R_DOOR_FRAME  (20)
#define R_DOOR_PITCH  (0x1fa1)
#define R_DOOR_YAW    (0xf2cc)
#define R_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
#endif



//=============================================================================
// ���G���g�����X�E�J��������f�[�^�ǂݍ��ݗp���[�N
//=============================================================================
typedef struct {

  u32 exitType;        // �o������^�C�v
  u32 pitch;           // �s�b�`
  u32 yaw;             // ���[
  u32 length;          // ����
  u32 manualTargetFlag;// �^�[�Q�b�g���W���L�����ǂ���
  u32 targetPosX;      // �^�[�Q�b�g���W x
  u32 targetPosY;      // �^�[�Q�b�g���W y
  u32 targetPosZ;      // �^�[�Q�b�g���W z
  u32 targetOffsetX;   // �^�[�Q�b�g�I�t�Z�b�g x
  u32 targetOffsetY;   // �^�[�Q�b�g�I�t�Z�b�g y
  u32 targetOffsetZ;   // �^�[�Q�b�g�I�t�Z�b�g z
  u32 frame;           // ����t���[����
  u32 validFlag_IN;    // �i�����ɗL���ȃf�[�^���ǂ���
  u32 validFlag_OUT;   // �ޏo���ɗL���ȃf�[�^���ǂ���

} ECAM_LOAD_DATA; 


//=============================================================================
// ���J�����p�����[�^
//=============================================================================
typedef struct {

  u16     pitch;        // �s�b�`
  u16     yaw;          // ���[
  fx32    length;       // ����
  VecFx32 targetPos;    // �^�[�Q�b�g���W
  VecFx32 targetOffset; // �^�[�Q�b�g�I�t�Z�b�g

} CAMERA_PARAM;


//=============================================================================
// ���G���g�����X�E�J��������f�[�^
//=============================================================================
typedef struct {

  u32 frame; // ����t���[����

  CAMERA_PARAM startParam; // �J�n�p�����[�^
  CAMERA_PARAM endParam;   // �ŏI�p�����[�^

  BOOL validFlag_IN;  // �i�����ɗL���ȃf�[�^���ǂ���
  BOOL validFlag_OUT; // �ޏo���ɗL���ȃf�[�^���ǂ���

} ANIME_DATA; 


//=============================================================================
// ���G���g�����X�E�J�������샏�[�N 
//=============================================================================
struct _ECAM_WORK {

  FIELDMAP_WORK* fieldmap;
  FIELD_CAMERA*  camera;

  ECAM_PARAM param;     // ���o�p�����[�^
  ANIME_DATA animeData; // �A�j���[�V�����f�[�^

  // �J�������A�p�f�[�^
  BOOL              recoveryValidFlag; // ���A�f�[�^���L�����ǂ���
  FIELD_CAMERA_MODE initialCameraMode; // �J�������[�h
  BOOL              cameraAreaFlag;    // �J�����G���A�̓���t���O

};



//=============================================================================
// ��private functions
//=============================================================================
// �����E�j���E������
static ECAM_WORK* CreateWork( HEAPID heapID ); // ���[�N�𐶐�����
static void DeleteWork( ECAM_WORK* work ); // ���[�N��j������
static void InitWork( ECAM_WORK* work, FIELDMAP_WORK* fieldmap ); // ���[�N������������
// �Z�b�g�A�b�v
static void ECamSetup_IN( ECAM_WORK* work ); // �ʏ�o�����֓��鉉�o���Z�b�g�A�b�v����
static void ECamSetup_OUT( ECAM_WORK* work ); // �ʏ�o��������o�鉉�o���Z�b�g�A�b�v����
static void ECamSetup_SP_IN( ECAM_WORK* work ); // ����o�����֓��鉉�o���Z�b�g�A�b�v����
static void ECamSetup_SP_OUT( ECAM_WORK* work ); // ����o��������o�鉉�o���Z�b�g�A�b�v����
// �A�j���[�V�����f�[�^
static void LoadSpData( ECAM_LOAD_DATA* dest, EXIT_TYPE exitType ); // ����o������̃J��������f�[�^��ǂݍ���
// �J�����̏����E���A
static void SetupCamera( ECAM_WORK* work ); // �J�����̐ݒ��ύX����
static void RecoverCamera( const ECAM_WORK* work ); // �J�����̐ݒ�𕜋A����
static void AdjustCameraAngle( FIELD_CAMERA* camera ); // �J�����A���O�����Čv�Z����
static void SetCurrentCameraTargetPos( FIELD_CAMERA* camera ); // �^�[�Q�b�g���W�����݂̎����l�ɐݒ肷��
static void SetupCameraTargetPos( FIELD_CAMERA* camera, const VecFx32* targetPos ); // �^�[�Q�b�g���W��ύX��, �J�����ɔ��f������
static void GetCurrentCameraParam( const FIELD_CAMERA* camera, CAMERA_PARAM* dest ); // �J�����̌��݂̃p�����[�^���擾����
// �J�����A�j���[�V����
static void SetCameraParam( const ECAM_WORK* work ); // �J�����𑦎����f����
static void StartCameraAnime( const ECAM_WORK* work ); // �J�����̃A�j���[�V�������J�n����
// ���@
static void GetOneStepAfterPos( const ECAM_WORK* work, VecFx32* dest ); // �����̍��W���擾����
static u16 GetPlayerDir( const ECAM_WORK* work ); // ���@�̌������擾����
// �A�N�Z�b�T
static FIELDMAP_WORK* GetFieldmap( const ECAM_WORK* work ); // �t�B�[���h�}�b�v���擾����
static FIELD_CAMERA* GetCamera( const ECAM_WORK* work ); // �J�������擾����
static const ANIME_DATA* GetAnimeData( const ECAM_WORK* work ); // �A�j���[�V�����f�[�^���擾����
static const ECAM_PARAM* GetECamParam( const ECAM_WORK* work ); // ���o�p�����[�^���擾����
static void SetECamParam( ECAM_WORK* work, const ECAM_PARAM* param ); // ���o�p�����[�^��ݒ肷��
static EXIT_TYPE GetExitType( const ECAM_WORK* work ); // �o������^�C�v���擾����
static BOOL CheckSpExit( const ECAM_WORK* work ); // ����ȏo��������ǂ������`�F�b�N����
static BOOL CheckOneStep( const ECAM_WORK* work ); // ���@�̈���ړ��̗L�����`�F�b�N����
static BOOL CheckCameraAnime( const ECAM_WORK* work ); // �V�`���G�[�V�����ɍ������A�j���[�V�����̗L�����`�F�b�N����



//=============================================================================
// ��public functions
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ���[�N�𐶐�����
 *
 * @param fieldmap
 *
 * @return �����������[�N
 */
//-----------------------------------------------------------------------------
ECAM_WORK* ENTRANCE_CAMERA_CreateWork( FIELDMAP_WORK* fieldmap )
{
  ECAM_WORK* work;

  work = CreateWork( FIELDMAP_GetHeapID(fieldmap) ); // ����
  InitWork( work, fieldmap ); // ������

  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void ENTRANCE_CAMERA_DeleteWork( ECAM_WORK* work )
{
  DeleteWork( work );
}

//-----------------------------------------------------------------------------
/**
 * @brief ���o���Z�b�g�A�b�v����
 *
 * @param work
 * @param param ���o�p�����[�^
 */
//----------------------------------------------------------------------------- 
void ENTRANCE_CAMERA_Setup( ECAM_WORK* work, const ECAM_PARAM* param )
{
  // ���o�p�����[�^��ݒ�
  SetECamParam( work, param );

  // ����o�����
  if( CheckSpExit(work) ) {
    switch( param->situation ) {
    case ECAM_SITUATION_IN:  ECamSetup_SP_IN( work );  break;
    case ECAM_SITUATION_OUT: ECamSetup_SP_OUT( work ); break;
    default:                 GF_ASSERT(0);             break;
    }
  }
  // �ʏ�o�����
  else {
    switch( param->situation ) {
    case ECAM_SITUATION_IN:  ECamSetup_IN( work );  break;
    case ECAM_SITUATION_OUT: ECamSetup_OUT( work ); break;
    default:                 GF_ASSERT(0);          break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ���o���J�n����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void ENTRANCE_CAMERA_Start( ECAM_WORK* work )
{
  const ECAM_PARAM* param = GetECamParam( work );
  const ANIME_DATA* anime = GetAnimeData( work );

  // �L���ȃA�j���[�V�������Ȃ�
  if( CheckCameraAnime(work) == FALSE ) { return; }

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
 * @brief ���o�ɂ���đ��삵���J�����𕜋A����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
void ENTRANCE_CAMERA_Recover( ECAM_WORK* work )
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
 * @brief ���[�N�𐶐�����
 *
 * @param heapID
 */
//-----------------------------------------------------------------------------
static ECAM_WORK* CreateWork( HEAPID heapID )
{
  ECAM_WORK* work;

  work = GFL_HEAP_AllocMemory( heapID, sizeof(ECAM_WORK) );

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: CreateWork\n" );
#endif

  return work;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DeleteWork( ECAM_WORK* work )
{
  GFL_HEAP_FreeMemory( work );

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: DeleteWork\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ���[�N������������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void InitWork( ECAM_WORK* work, FIELDMAP_WORK* fieldmap )
{
  // �[���N���A
  GFL_STD_MemClear( work, sizeof(ECAM_WORK) );

  // ������
  work->fieldmap = fieldmap;
  work->camera   = FIELDMAP_GetFieldCamera( fieldmap );
  work->recoveryValidFlag = FALSE;
  work->cameraAreaFlag    = FALSE;

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: InitWork\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �ʏ�o�����֓��鉉�o���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ECamSetup_IN( ECAM_WORK* work )
{
  ANIME_DATA*   anime      = &work->animeData;
  CAMERA_PARAM* startParam = &anime->startParam;
  CAMERA_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 endPos;

  // �A�j���̗L��������
  switch( playerDir ) {
  case DIR_UP:
  case DIR_LEFT:
  case DIR_RIGHT:
    anime->validFlag_IN  = TRUE;
    anime->validFlag_OUT = FALSE; 
    break;
  case DIR_DOWN:
    anime->validFlag_IN  = FALSE;
    anime->validFlag_OUT = FALSE; 
    return; // �ȉ��̏����͕s�v
  default: 
    GF_ASSERT(0); 
    break; 
  }

  // ���@�̍ŏI���W������
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &endPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &endPos );
  }

  SetupCamera( work ); // �J�����̐ݒ��ύX
  AdjustCameraAngle( work->camera ); // �J�����A���O�����Čv�Z
  SetCurrentCameraTargetPos( work->camera ); // �^�[�Q�b�g���W��������

  // �J�n�p�����[�^ ( = ���ݒl )
  GetCurrentCameraParam( work->camera, startParam );

  // �ŏI�p�����[�^
  {
    VecFx32 ofs = {0, 0, 0};

    GetCurrentCameraParam( work->camera, endParam );
    endParam->targetPos = endPos;
    endParam->targetOffset = ofs;

    switch( playerDir ) {
    case DIR_UP:    
      endParam->length = U_DOOR_LENGTH; 
      break;
    case DIR_LEFT:  
      endParam->pitch = L_DOOR_PITCH;
      endParam->yaw = L_DOOR_YAW;    
      endParam->length = L_DOOR_LENGTH; 
      break;
    case DIR_RIGHT: 
      endParam->pitch = R_DOOR_PITCH;
      endParam->yaw = R_DOOR_YAW;    
      endParam->length = R_DOOR_LENGTH; 
      break;
    case DIR_DOWN: 
      break;
    default: 
      GF_ASSERT(0); 
      break;
    }
  }

  // �A�j���t���[����
  switch( playerDir ) {
  case DIR_UP:    anime->frame = U_DOOR_FRAME; break;
  case DIR_DOWN:  anime->frame = 0;            break;
  case DIR_LEFT:  anime->frame = L_DOOR_FRAME; break;
  case DIR_RIGHT: anime->frame = R_DOOR_FRAME; break;
  default: GF_ASSERT(0); break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �ʏ�o��������o�鉉�o���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ECamSetup_OUT( ECAM_WORK* work )
{
  ANIME_DATA*   anime      = &work->animeData;
  CAMERA_PARAM* startParam = &anime->startParam;
  CAMERA_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;

  // �A�j���̗L��������
  switch( playerDir ) {
  case DIR_DOWN:
  case DIR_LEFT:
  case DIR_RIGHT:
    anime->validFlag_IN  = FALSE;
    anime->validFlag_OUT = TRUE; 
    break;
  case DIR_UP:
    anime->validFlag_IN  = FALSE;
    anime->validFlag_OUT = FALSE; 
    return; // �ȉ��̏����͕s�v
  default: 
    GF_ASSERT(0); 
    break; 
  }

  // �J�n���̃^�[�Q�b�g���W
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &stepPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &stepPos );
  }

  // �A�j���t���[����
  switch( playerDir ) {
  case DIR_UP:    anime->frame = 0;            break;
  case DIR_DOWN:  anime->frame = U_DOOR_FRAME; break;
  case DIR_LEFT:  anime->frame = R_DOOR_FRAME; break;
  case DIR_RIGHT: anime->frame = L_DOOR_FRAME; break;
  default: GF_ASSERT(0); break;
  }

  // ���@������ړ���������ԂɃJ�������X�V
  if( CheckOneStep(work) ) {
    SetupCameraTargetPos( camera, &stepPos );
  }

  SetupCamera( work ); // �J�����̐ݒ��ύX
  AdjustCameraAngle( camera ); // �J�����A���O�����Čv�Z

  // �J�n�p�����[�^
  {
    GetCurrentCameraParam( camera, startParam );

    switch( playerDir ) {
    case DIR_UP:
      break;
    case DIR_DOWN:  
      startParam->length = U_DOOR_LENGTH; 
      break;
    case DIR_LEFT:  
      endParam->pitch = R_DOOR_PITCH;
      startParam->yaw = R_DOOR_YAW;    
      startParam->length = R_DOOR_LENGTH; 
      break;
    case DIR_RIGHT: 
      endParam->pitch = L_DOOR_PITCH;
      startParam->yaw = L_DOOR_YAW;    
      startParam->length = L_DOOR_LENGTH; 
      break;
    default: GF_ASSERT(0); break;
    }
    startParam->targetPos = stepPos;
    VEC_Set( &startParam->targetOffset, 0, 0, 0 );
  }

  // �ŏI�p�����[�^ ( = ���ݒl )
  GetCurrentCameraParam( camera, endParam );

  // �J�n�J�����ݒ�
  if( anime->validFlag_OUT ) {
    FIELD_CAMERA_SetAnglePitch( camera, startParam->pitch );
    FIELD_CAMERA_SetAngleYaw( camera, startParam->yaw );
    FIELD_CAMERA_SetAngleLen( camera, startParam->length );
    FIELD_CAMERA_SetTargetPos( camera, &startParam->targetPos );
    FIELD_CAMERA_SetTargetOffset( camera, &startParam->targetOffset );
  } 

}

//-----------------------------------------------------------------------------
/**
 * @brief ����o�����֓��鉉�o���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ECamSetup_SP_IN( ECAM_WORK* work )
{
  ANIME_DATA*   anime      = &work->animeData;
  CAMERA_PARAM* startParam = &anime->startParam;
  CAMERA_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;
  ECAM_LOAD_DATA loadData;

  // ���o�f�[�^�����[�h
  LoadSpData( &loadData, GetExitType(work) );

  // ���o�̗L��������
  anime->validFlag_IN  = loadData.validFlag_IN;
  anime->validFlag_OUT = loadData.validFlag_OUT;
  if( anime->validFlag_IN == FALSE ) { 
    return; // �ȉ��̏����͕s�v
  }

  // ����ړ���̍��W
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &stepPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &stepPos );
  }

  SetupCamera( work ); // �J�����̐ݒ��ύX
  AdjustCameraAngle( camera ); // �J�����A���O�����Čv�Z
  SetCurrentCameraTargetPos( camera ); // �^�[�Q�b�g���W��������

  // �J�n�p�����[�^ ( = ���ݒl )
  GetCurrentCameraParam( camera, startParam );

  // �ŏI�p�����[�^
  anime->frame = loadData.frame;
  endParam->pitch  = loadData.pitch;
  endParam->yaw    = loadData.yaw;
  endParam->length = loadData.length << FX32_SHIFT;
  VEC_Set( &endParam->targetOffset,
      loadData.targetOffsetX << FX32_SHIFT,
      loadData.targetOffsetY << FX32_SHIFT,
      loadData.targetOffsetZ << FX32_SHIFT );
  if( loadData.manualTargetFlag ) {
    VEC_Set( &endParam->targetPos,
        loadData.targetPosX << FX32_SHIFT,
        loadData.targetPosY << FX32_SHIFT,
        loadData.targetPosZ << FX32_SHIFT );
  }
  else {
    endParam->targetPos = stepPos;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ����o��������o�鉉�o���Z�b�g�A�b�v����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void ECamSetup_SP_OUT( ECAM_WORK* work )
{
  ANIME_DATA*   anime      = &work->animeData;
  CAMERA_PARAM* startParam = &anime->startParam;
  CAMERA_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;
  ECAM_LOAD_DATA loadData;

  // ���o�f�[�^�����[�h
  LoadSpData( &loadData, GetExitType(work) );

  // ���o�̗L��������
  anime->validFlag_IN  = loadData.validFlag_IN;
  anime->validFlag_OUT = loadData.validFlag_OUT;
  if( anime->validFlag_OUT == FALSE ) { 
    return; // �ȉ��̏����͕s�v
  }

  // �J�n���̃^�[�Q�b�g���W
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &stepPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &stepPos );
  }

  // ���@������ړ���������ԂɃJ�������X�V
  if( CheckOneStep(work) ) {
    SetupCameraTargetPos( camera, &stepPos );
  }

  SetupCamera( work );
  AdjustCameraAngle( camera );

  // �ŏI�p�����[�^ ( = ���ݒl )
  GetCurrentCameraParam( camera, endParam );

  // �J�n�p�����[�^
  anime->frame = loadData.frame;
  startParam->pitch  = loadData.pitch;
  startParam->yaw    = loadData.yaw;
  startParam->length = loadData.length << FX32_SHIFT;
  VEC_Set( &startParam->targetOffset,
      loadData.targetOffsetX << FX32_SHIFT,
      loadData.targetOffsetY << FX32_SHIFT,
      loadData.targetOffsetZ << FX32_SHIFT );
  if( loadData.manualTargetFlag ) {
    VEC_Set( &startParam->targetPos,
        loadData.targetPosX << FX32_SHIFT,
        loadData.targetPosY << FX32_SHIFT,
        loadData.targetPosZ << FX32_SHIFT );
  }
  else {
    startParam->targetPos = stepPos;
  }

  // �J�n�J�����ݒ�
  if( anime->validFlag_OUT ) {
    FIELD_CAMERA_SetAnglePitch( camera, startParam->pitch );
    FIELD_CAMERA_SetAngleYaw( camera, startParam->yaw );
    FIELD_CAMERA_SetAngleLen( camera, startParam->length );
    FIELD_CAMERA_SetTargetPos( camera, &startParam->targetPos );
    FIELD_CAMERA_SetTargetOffset( camera, &startParam->targetOffset );
  } 
}

//-----------------------------------------------------------------------------
/**
 * @brief ����o������̃J��������f�[�^��ǂݍ���
 *
 * @param dest     �ǂݍ��񂾃f�[�^�̊i�[��
 * @param exitType ����o�����^�C�v ( EXIT_TYPE_SPxx )
 */
//-----------------------------------------------------------------------------
static void LoadSpData( ECAM_LOAD_DATA* dest, EXIT_TYPE exitType )
{
  u32 datID;

  // EXIT_TYPE_SPxx�ȊO�̓_��
  GF_ASSERT( EXIT_TYPE_SP1 <= exitType );
  GF_ASSERT( exitType < EXIT_TYPE_MAX );

  // �ǂݍ��ރf�[�^������
  datID = exitType - EXIT_TYPE_SP1;

  // �ǂݍ���
  GFL_ARC_LoadData( dest, ARCID_ENTRANCE_CAMERA, datID );

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: LoadSpData\n" );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - exitType         = %d\n", dest->exitType );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - pitch            = 0x%x\n", dest->pitch );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - yaw              = 0x%x\n", dest->yaw );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - length           = 0x%x\n", dest->length );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - manualTargetFlag = %d\n", dest->manualTargetFlag );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetPosX       = 0x%x\n", dest->targetPosX );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetPosY       = 0x%x\n", dest->targetPosY );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetPosZ       = 0x%x\n", dest->targetPosZ );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetOffsetX    = 0x%x\n", dest->targetOffsetX );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetOffsetY    = 0x%x\n", dest->targetOffsetY );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetOffsetZ    = 0x%x\n", dest->targetOffsetZ );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - frame            = %d\n", dest->frame );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - validFlag_IN     = %d\n", dest->validFlag_IN );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - validFlag_OUT    = %d\n", dest->validFlag_OUT );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̌��݂̃p�����[�^���擾����
 *
 * @param camera
 * @param dest   �擾�����p�����[�^�̊i�[��
 */
//-----------------------------------------------------------------------------
static void GetCurrentCameraParam( const FIELD_CAMERA* camera, CAMERA_PARAM* dest )
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
 * @brief �J��������̏���������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetupCamera( ECAM_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = work->camera;

  // ���[���V�X�e���̃J����������~
  if( FIELDMAP_GetBaseSystemType( fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    FLDNOGRID_MAPPER* NGMapper = FIELDMAP_GetFldNoGridMapper( fieldmap );
    FLDNOGRID_MAPPER_SetRailCameraActive( NGMapper, FALSE );
  }

  // �^�[�Q�b�g�̃o�C���h��OFF
  FIELD_CAMERA_FreeTarget( camera );

  // �J�����G���A�𖳌���
  work->cameraAreaFlag = FIELD_CAMERA_GetCameraAreaActive( camera );
  FIELD_CAMERA_SetCameraAreaActive( camera, FALSE );

  // �J�������[�h��ύX
  work->initialCameraMode = FIELD_CAMERA_GetMode( camera ); 
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  // �t���O�Z�b�g
  work->recoveryValidFlag = TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����̕��A������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void RecoverCamera( const ECAM_WORK* work )
{
  FIELDMAP_WORK* fieldmap = work->fieldmap;
  FIELD_CAMERA*  camera   = work->camera;

  GF_ASSERT( work->recoveryValidFlag );

  // �J�����G���A�𕜋A
  FIELD_CAMERA_SetCameraAreaActive( camera, work->cameraAreaFlag );

  // �^�[�Q�b�g�����ɖ߂�
  FIELD_CAMERA_BindDefaultTarget( camera );

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
 * @brief �^�[�Q�b�g���W��ύX��, �J�����ɔ��f������
 *
 * @param camera
 * @param targetPos �^�[�Q�b�g���W
 */
//-----------------------------------------------------------------------------
static void SetupCameraTargetPos( FIELD_CAMERA* camera, const VecFx32* targetPos )
{
  FIELD_CAMERA_FreeTarget( camera ); // �^�[�Q�b�g���O��
  FIELD_CAMERA_SetTargetPos( camera, targetPos ); // �ꎞ�I�Ƀ^�[�Q�b�g���W��ݒ�
  FIELD_CAMERA_Main( camera, 0 ); // �^�[�Q�b�g�̕ύX���J�����ɔ��f������
  FIELD_CAMERA_BindDefaultTarget( camera ); // �^�[�Q�b�g�����ɖ߂�
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����𑦎����f����
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void SetCameraParam( const ECAM_WORK* work )
{
  FIELD_CAMERA*       camera   = work->camera;
  const ANIME_DATA*   anime    = &(work->animeData); 
  const CAMERA_PARAM* endParam = &(anime->endParam);

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
static void StartCameraAnime( const ECAM_WORK* work )
{
  FIELDMAP_WORK*      fieldmap   = work->fieldmap;
  FIELD_CAMERA*       camera     = work->camera;
  const ANIME_DATA*   anime      = &(work->animeData); 
  const CAMERA_PARAM* startParam = &(anime->startParam);
  const CAMERA_PARAM* endParam   = &(anime->endParam);

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
 * @brief �����̍��W���擾����
 *
 * @param work
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetOneStepAfterPos( const ECAM_WORK* work, VecFx32* dest )
{
  FIELD_PLAYER* player;
  u16 playerDir;
  VecFx32 now_pos, way_vec, step_pos;

  // ���@�f�[�^���擾
  player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  playerDir = FIELD_PLAYER_GetDir( player );

  // ����O�i�������W���Z�o
  FIELD_PLAYER_GetPos( player, &now_pos );
  FIELD_PLAYER_GetDirWay( player, playerDir, &way_vec );
  VEC_MultAdd( FIELD_CONST_GRID_FX32_SIZE, &way_vec, &now_pos, &step_pos );

  // y ���W��␳
  {
    MMDL* mmdl;
    fx32 height;

    mmdl = FIELD_PLAYER_GetMMdl( player );
    MMDL_GetMapPosHeight( mmdl, &step_pos, &height );
    step_pos.y = height;
  }

  // ���W��Ԃ�
  *dest = step_pos;
}

//----------------------------------------------------------------------------- 
/**
 * @brief ���@�̌������擾����
 *
 * @param work
 */
//----------------------------------------------------------------------------- 
static u16 GetPlayerDir( const ECAM_WORK* work )
{
  FIELD_PLAYER* player;

  player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  return FIELD_PLAYER_GetDir( player );
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
static FIELDMAP_WORK* GetFieldmap( const ECAM_WORK* work )
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
static FIELD_CAMERA* GetCamera( const ECAM_WORK* work )
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
static const ANIME_DATA* GetAnimeData( const ECAM_WORK* work )
{
  return &(work->animeData);
}

//----------------------------------------------------------------------------- 
/**
 * @brief ���o�p�����[�^���擾����
 *
 * @param work
 *
 * @return ���o�p�����[�^
 */
//----------------------------------------------------------------------------- 
static const ECAM_PARAM* GetECamParam( const ECAM_WORK* work )
{
  return &(work->param);
}

//----------------------------------------------------------------------------- 
/**
 * @brief ���o�p�����[�^��ݒ肷��
 *
 * @param work
 * @param param �ݒ肷��p�����[�^
 */
//----------------------------------------------------------------------------- 
static void SetECamParam( ECAM_WORK* work, const ECAM_PARAM* param )
{
  work->param = *param;
}

//----------------------------------------------------------------------------- 
/**
 * @brief �o������^�C�v���擾����
 *
 * @param work
 *
 * @return �o������^�C�v
 */
//----------------------------------------------------------------------------- 
static EXIT_TYPE GetExitType( const ECAM_WORK* work )
{
  const ECAM_PARAM* param;

  param = GetECamParam( work );

  return param->exitType;
}

//----------------------------------------------------------------------------- 
/**
 * @brief ����ȏo��������ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return ����ȏo������̏ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------- 
static BOOL CheckSpExit( const ECAM_WORK* work )
{
  const ECAM_PARAM* param;

  param = GetECamParam( work );

  if( EXIT_TYPE_SP1 <= param->exitType ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//----------------------------------------------------------------------------- 
/**
 * @brief ���@�̈���ړ��̗L�����`�F�b�N����
 *
 * @param work
 *
 * @return ���@�̈���ړ����o������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------- 
static BOOL CheckOneStep( const ECAM_WORK* work )
{
  const ECAM_PARAM* param;

  param = GetECamParam( work );

  return (param->oneStep == ECAM_ONESTEP_ON );
}

//----------------------------------------------------------------------------- 
/**
 * @brief �V�`���G�[�V�����ɍ������A�j���[�V�����̗L�����`�F�b�N����
 *
 * @param work
 *
 * @return �L���ȃA�j���[�V�����f�[�^�����݂���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//----------------------------------------------------------------------------- 
static BOOL CheckCameraAnime( const ECAM_WORK* work )
{
  const ECAM_PARAM* param;
  const ANIME_DATA* anime;

  param = GetECamParam( work );
  anime = GetAnimeData( work );

  if( param->situation == ECAM_SITUATION_IN ) {
    return anime->validFlag_IN;
  }
  else {
    return anime->validFlag_OUT;
  }
}












