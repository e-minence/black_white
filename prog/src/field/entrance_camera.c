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

#include "field_camera_anime.h"
#include "field_rail.h"
#include "fieldmap.h"

#include "field/field_const.h"     // for FIELD_CONST_xxxx
#include "field/eventdata_type.h"  // for EXIT_TYPE_xxxx
#include "arc/arc_def.h"           // for ARCID_xxxx



//=============================================================================
// ���萔
//=============================================================================
//#define DEBUG_PRINT_ENABLE    // �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_TARGET (3)// �f�o�b�O�o�͐�

//#define ZERO_FRAME 
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
#define L_DOOR_YAW    (0x1089)
#define L_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// �E�h�A�ւ̏o����
#define R_DOOR_FRAME  (0)
#define R_DOOR_PITCH  (0x1fa1)
#define R_DOOR_YAW    (0xf047)
#define R_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
//-------------------------------------
// �ʏ�o������̃J�������� ( �ʏ�� )
//-------------------------------------
#else 
// ��h�A�ւ̏o����
#define U_DOOR_FRAME  (10)
#define U_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// ���h�A�ւ̏o����
#define L_DOOR_FRAME  (10)
#define L_DOOR_PITCH  (0x1fa1)
#define L_DOOR_YAW    (0x1089)
#define L_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
// �E�h�A�ւ̏o����
#define R_DOOR_FRAME  (10)
#define R_DOOR_PITCH  (0x1fa1)
#define R_DOOR_YAW    (0xf047)
#define R_DOOR_LENGTH (0x00b4 << FX32_SHIFT) 
#endif



//=============================================================================
// ���G���g�����X�E�J��������f�[�^�ǂݍ��ݗp���[�N
//=============================================================================
typedef struct {

  u32 exitType;          // �o������^�C�v
  u32 pitch;             // �s�b�`
  u32 yaw;               // ���[
  u32 length;            // ����
  u32 manualTargetFlag;  // �^�[�Q�b�g���W���L�����ǂ���
  u32 targetPosX;        // �^�[�Q�b�g���W x
  u32 targetPosY;        // �^�[�Q�b�g���W y
  u32 targetPosZ;        // �^�[�Q�b�g���W z
  u32 targetOffsetX;     // �^�[�Q�b�g�I�t�Z�b�g x
  u32 targetOffsetY;     // �^�[�Q�b�g�I�t�Z�b�g y
  u32 targetOffsetZ;     // �^�[�Q�b�g�I�t�Z�b�g z
  u32 frame;             // ����t���[����
  u32 validFlag_IN;      // �i�����ɗL���ȃf�[�^���ǂ���
  u32 validFlag_OUT;     // �ޏo���ɗL���ȃf�[�^���ǂ���
  u32 targetBindOffFlag; // �^�[�Q�b�g�̃o�C���h��OFF�ɂ��邩�ǂ���
  u32 cameraAreaOffFlag; // �J�����G���A��OFF�ɂ��邩�ǂ���

} ECAM_LOAD_DATA; 


//=============================================================================
// ���G���g�����X�E�J�������샏�[�N 
//=============================================================================
struct _ECAM_WORK {

  FIELDMAP_WORK* fieldmap;
  FIELD_CAMERA*  camera;
  FCAM_ANIME_DATA animeData;
  FCAM_ANIME_WORK* animeWork;
  ECAM_PARAM param;     // ���o�p�����[�^
  BOOL setupFlag;       // ���o�̃Z�b�g�A�b�v�������ς݂��ǂ���
  BOOL validFlag_IN;    // �i�����ɗL���ȃf�[�^���ǂ���
  BOOL validFlag_OUT;   // �ޏo���ɗL���ȃf�[�^���ǂ���
};



//=============================================================================
// ��private functions
//=============================================================================
// �����E�j���E������
static ECAM_WORK* CreateWork( HEAPID heapID ); // ���[�N�𐶐�����
static void DeleteWork( ECAM_WORK* work ); // ���[�N��j������
static void InitWork( ECAM_WORK* work, FIELDMAP_WORK* fieldmap ); // ���[�N������������
static void CreateAnimeWork( ECAM_WORK* work, HEAPID heapID ); // �J�������샏�[�N�𐶐�����
static void DeleteAnimeWork( ECAM_WORK* work ); // �J�������샏�[�N��j������
// �Z�b�g�A�b�v
static void ECamSetup_IN( ECAM_WORK* work ); // �ʏ�o�����֓��鉉�o���Z�b�g�A�b�v����
static void ECamSetup_OUT( ECAM_WORK* work ); // �ʏ�o��������o�鉉�o���Z�b�g�A�b�v����
static void ECamSetup_SP_IN( ECAM_WORK* work ); // ����o�����֓��鉉�o���Z�b�g�A�b�v����
static void ECamSetup_SP_OUT( ECAM_WORK* work ); // ����o��������o�鉉�o���Z�b�g�A�b�v����
// �A�j���[�V�����f�[�^
static void LoadSpData( ECAM_LOAD_DATA* dest, EXIT_TYPE exitType ); // ����o������̃J��������f�[�^��ǂݍ���
// �J�����̏����E���A
static void SetupCameraTargetPos( FIELD_CAMERA* camera, const VecFx32* targetPos ); // �^�[�Q�b�g���W��ύX��, �J�����ɔ��f������
// ���@
static void GetOneStepAfterPos( const ECAM_WORK* work, VecFx32* dest ); // �����̍��W���擾����
static void GetOneStepAfterPos_GRID( const ECAM_WORK* work, VecFx32* dest ); // �����̍��W���擾���� ( �O���b�h�}�b�v�p )
static void GetOneStepAfterPos_RAIL( const ECAM_WORK* work, VecFx32* dest ); // �����̍��W���擾���� ( ���[���}�b�v�p )
static u16 GetPlayerDir( const ECAM_WORK* work ); // ���@�̌������擾����
// �A�N�Z�b�T
static FIELDMAP_WORK* GetFieldmap( const ECAM_WORK* work ); // �t�B�[���h�}�b�v���擾����
static FIELD_CAMERA* GetCamera( const ECAM_WORK* work ); // �J�������擾����
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
  HEAPID heapID;
  ECAM_WORK* work;

  heapID = FIELDMAP_GetHeapID( fieldmap );

  // ���[�N�𐶐�
  work = CreateWork( heapID );

  // ���[�N��������
  InitWork( work, fieldmap );
  CreateAnimeWork( work, heapID );

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
  DeleteAnimeWork( work );
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
  FCAM_ANIME_SetAnimeData( work->animeWork, &work->animeData );

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

  // �Z�b�g�A�b�v����
  work->setupFlag = TRUE;
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
  // �L���ȃA�j���[�V�������Ȃ�
  if( CheckCameraAnime(work) == FALSE ) { return; }

  FCAM_ANIME_StartAnime( work->animeWork );
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
  FCAM_ANIME_RecoverCamera( work->animeWork );
}

//-----------------------------------------------------------------------------
/**
 * @brief ���o�̗L�����`�F�b�N����
 *
 * @param work
 *
 * @return �L���ȉ��o������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
BOOL ENTRANCE_CAMERA_IsAnimeExist( const ECAM_WORK* work )
{
  // �Z�b�g�A�b�v��łȂ��ƃ_��
  GF_ASSERT( work->setupFlag );

  return CheckCameraAnime(work);
}

//-----------------------------------------------------------------------------
/**
 * @brief ���o���O�t���[�����ǂ������`�F�b�N����
 *
 * @param work
 *
 * @return �O�t���[�����o�f�[�^�Ȃ� TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
BOOL ENTRANCE_CAMERA_IsZeroFrameAnime( const ECAM_WORK* work )
{
  const FCAM_ANIME_DATA* anime = &work->animeData;

  if( anime->frame == 0 ) { return TRUE; } 
  return FALSE;
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
  work->setupFlag = FALSE;

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: InitWork\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N�𐶐�����
 *
 * @parma work
 * @param heapID
 */
//-----------------------------------------------------------------------------
static void CreateAnimeWork( ECAM_WORK* work, HEAPID heapID )
{
  GF_ASSERT( work->animeWork == NULL );
  GF_ASSERT( work->fieldmap );

  work->animeWork = FCAM_ANIME_CreateWork( work->fieldmap );

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: CreateWork\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�������샏�[�N��j������
 *
 * @param work
 */
//-----------------------------------------------------------------------------
static void DeleteAnimeWork( ECAM_WORK* work )
{
  if( work->animeWork ) {
    FCAM_ANIME_DeleteWork( work->animeWork );
    work->animeWork = NULL;
  }

#ifdef DEBUG_PRINT_ENABLE
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: DeleteAnimeWork\n" );
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
  FCAM_ANIME_DATA*   anime      = &work->animeData;
  FCAM_PARAM* startParam = &anime->startParam;
  FCAM_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 endPos;

  // �A�j���̗L��������
  switch( playerDir ) {
  case DIR_LEFT:
  case DIR_RIGHT:
    work->validFlag_IN  = TRUE;
    work->validFlag_OUT = FALSE; 
    break;
  case DIR_UP:
  case DIR_DOWN:
    work->validFlag_IN  = FALSE;
    work->validFlag_OUT = FALSE; 
    break;
  default: 
    GF_ASSERT(0); 
    break; 
  }

  if( work->validFlag_IN == FALSE ) { 
    return; // �ȉ��̏����͕s�v
  }

  // �^�[�Q�b�g�I�t�Z�b�g�E�J�����G���A�͑��삵�Ȃ�
  anime->cameraAreaOffFlag = FALSE;
  anime->targetBindOffFlag = FALSE;

  // ���@�̍ŏI���W������
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &endPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &endPos );
  }

  // �J�������Z�b�g�A�b�v
  FCAM_ANIME_SetupCamera( work->animeWork );

  // �J�n�p�����[�^ ( = ���ݒl )
  FCAM_PARAM_GetCurrentParam( work->camera, startParam );

  // �ŏI�p�����[�^
  {
    VecFx32 ofs = {0, 0, 0};

    FCAM_PARAM_GetCurrentParam( work->camera, endParam );
    //endParam->targetPos = endPos;
    //endParam->targetOffset = ofs;

    switch( playerDir ) {
    case DIR_UP:    endParam->length = U_DOOR_LENGTH; break;
    case DIR_LEFT:  endParam->yaw = L_DOOR_YAW;    break;
    case DIR_RIGHT: endParam->yaw = R_DOOR_YAW;    break;
    case DIR_DOWN:  break;
    default:        GF_ASSERT(0); break;
    }
  }

  // �A�j���t���[����
  switch( playerDir ) {
  case DIR_UP:    anime->frame = 0; break;
  case DIR_DOWN:  anime->frame = 0; break;
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
  FCAM_ANIME_DATA*   anime      = &work->animeData;
  FCAM_PARAM* startParam = &anime->startParam;
  FCAM_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;

  // �A�j���̗L��������
  switch( playerDir ) {
  case DIR_DOWN:
  case DIR_LEFT:
  case DIR_RIGHT:
    work->validFlag_IN  = FALSE;
    work->validFlag_OUT = TRUE; 
    break;
  case DIR_UP:
    work->validFlag_IN  = FALSE;
    work->validFlag_OUT = FALSE; 
    break;
  default: 
    GF_ASSERT(0); 
    break; 
  }

  if( work->validFlag_OUT == FALSE ) { 
    return; // �ȉ��̏����͕s�v
  }

  // �^�[�Q�b�g�I�t�Z�b�g�E�J�����G���A�͑��삵�Ȃ�
  anime->cameraAreaOffFlag = FALSE;
  anime->targetBindOffFlag = FALSE;

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

  // �J�������Z�b�g�A�b�v
  FCAM_ANIME_SetupCamera( work->animeWork );

  // �J�n�p�����[�^
  {
    FCAM_PARAM_GetCurrentParam( work->camera, startParam );

    switch( playerDir ) {
    case DIR_UP:
      break;
    case DIR_DOWN:  
      startParam->length = U_DOOR_LENGTH; 
      break;
    case DIR_LEFT:  
      startParam->yaw = R_DOOR_YAW;    
      break;
    case DIR_RIGHT: 
      startParam->yaw = L_DOOR_YAW;    
      break;
    default: GF_ASSERT(0); break;
    }
    //startParam->targetPos = stepPos;
    //VEC_Set( &startParam->targetOffset, 0, 0, 0 );
  }

  // �ŏI�p�����[�^ ( = ���ݒl )
  FCAM_PARAM_GetCurrentParam( work->camera, endParam );

  // �J�n�J�����ݒ�
  if( work->validFlag_OUT ) {
    FCAM_ANIME_SetCameraStartParam( work->animeWork );
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
  FCAM_ANIME_DATA*   anime      = &work->animeData;
  FCAM_PARAM* startParam = &anime->startParam;
  FCAM_PARAM* endParam   = &anime->endParam;
  FIELD_CAMERA* camera     = GetCamera( work );
  FIELD_PLAYER* player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16           playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;
  ECAM_LOAD_DATA loadData;

  // ���o�f�[�^�����[�h
  LoadSpData( &loadData, GetExitType(work) );
  anime->cameraAreaOffFlag = loadData.cameraAreaOffFlag;
  anime->targetBindOffFlag = loadData.targetBindOffFlag;

  // ���o�̗L��������
  work->validFlag_IN  = loadData.validFlag_IN;
  work->validFlag_OUT = loadData.validFlag_OUT;
  if( work->validFlag_IN == FALSE ) { 
    return; // �ȉ��̏����͕s�v
  }

  // ����ړ���̍��W
  if( CheckOneStep(work) ) {
    GetOneStepAfterPos( work, &stepPos );
  }
  else {
    FIELD_PLAYER_GetPos( player, &stepPos );
  }

  // �J�������Z�b�g�A�b�v
  FCAM_ANIME_SetupCamera( work->animeWork );

  // �J�n�p�����[�^ ( = ���ݒl )
  FCAM_PARAM_GetCurrentParam( work->camera, startParam );

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
  FCAM_ANIME_DATA* anime      = &work->animeData;
  FCAM_PARAM*      startParam = &anime->startParam;
  FCAM_PARAM*      endParam   = &anime->endParam;
  FIELD_CAMERA*    camera     = GetCamera( work );
  FIELD_PLAYER*    player     = FIELDMAP_GetFieldPlayer( work->fieldmap );
  u16              playerDir  = FIELD_PLAYER_GetDir( player );

  VecFx32 stepPos;
  ECAM_LOAD_DATA loadData;

  // ���o�f�[�^�����[�h
  LoadSpData( &loadData, GetExitType(work) );
  anime->cameraAreaOffFlag = loadData.cameraAreaOffFlag;
  anime->targetBindOffFlag = loadData.targetBindOffFlag;

  // ���o�̗L��������
  work->validFlag_IN  = loadData.validFlag_IN;
  work->validFlag_OUT = loadData.validFlag_OUT;
  if( work->validFlag_OUT == FALSE ) { 
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
  if( anime->targetBindOffFlag && CheckOneStep(work) ) {
    SetupCameraTargetPos( camera, &stepPos );
  }

  // �J�������Z�b�g�A�b�v
  FCAM_ANIME_SetupCamera( work->animeWork );

  // �ŏI�p�����[�^ ( = ���ݒl )
  FCAM_PARAM_GetCurrentParam( work->camera, endParam );

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
  if( work->validFlag_OUT ) {
    FCAM_ANIME_SetCameraStartParam( work->animeWork );
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
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - cameraAreaOffFlag   = %d\n", dest->cameraAreaOffFlag );
  OS_TFPrintf( DEBUG_PRINT_TARGET, "ENTRANCE-CAMERA: - targetBindOffFlag   = %d\n", dest->targetBindOffFlag );
#endif
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
 * @brief �����̍��W���擾����
 *
 * @param work
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetOneStepAfterPos( const ECAM_WORK* work, VecFx32* dest )
{
  if( FIELDMAP_GetBaseSystemType( work->fieldmap ) == FLDMAP_BASESYS_RAIL ) {
    GetOneStepAfterPos_RAIL( work, dest );
  }
  else {
    GetOneStepAfterPos_GRID( work, dest );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �����̍��W���擾���� ( �O���b�h�}�b�v�p )
 *
 * @param work
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetOneStepAfterPos_GRID( const ECAM_WORK* work, VecFx32* dest )
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
 * @brief �����̍��W���擾���� ( ���[���}�b�v�p )
 *
 * @param work
 * @param dest �擾�������W�̊i�[��
 */
//----------------------------------------------------------------------------- 
static void GetOneStepAfterPos_RAIL( const ECAM_WORK* work, VecFx32* dest )
{
  FIELD_PLAYER* player;
  u16 playerDir;
  RAIL_LOCATION location;
  FLDNOGRID_MAPPER* NGMapper;
  const FIELD_RAIL_MAN* railMan;
  VecFx32 step_pos;

  // ���@�f�[�^���擾
  player = FIELDMAP_GetFieldPlayer( work->fieldmap );
  playerDir = FIELD_PLAYER_GetDir( player );

  // �����̍��W���擾
  NGMapper = FIELDMAP_GetFldNoGridMapper( work->fieldmap );
  railMan = FLDNOGRID_MAPPER_GetRailMan( NGMapper );
  FIELD_PLAYER_GetDirNoGridLocation( player, playerDir, &location );
  FIELD_RAIL_MAN_GetLocationPosition( railMan, &location, &step_pos );

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

  return EXIT_TYPE_IsSpExit( param->exitType );
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

  param = GetECamParam( work );

  if( param->situation == ECAM_SITUATION_IN ) {
    return work->validFlag_IN;
  }
  else {
    return work->validFlag_OUT;
  }
} 
