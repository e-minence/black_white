/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ��ISS�V�X�e�� ( 3D�T�E���h�V�X�e�� )
 * @file   iss_3d_sys.c
 * @author obata
 * @date   2009.09.08
 */
/////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_3ds_sys.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"


//===============================================================================
// ���萔
//===============================================================================
// �f�o�b�O���̏o�͐�
#define PRINT_TARGET (1)

// �Q�ƒ��f�[�^�C���f�b�N�X�̖����l
#define CURRENT_DATA_NONE (0xff)

// ����Ώۃg���b�N�̏������{�����[��
#define INITIAL_VOLUME (0)

// �ő�{�����[��
#define MAX_VOLUME (127)

// �}�X�^�[�{�����[���̕ύX���x
#define MASTER_VOLUME_FADE_SPEED (2)

// ��Βl�擾�}�N��
#define ABS(n) (n>0 ? n : -n)


//===============================================================================
// ��1�̉����������\���� ( ���j�b�g )
//===============================================================================
typedef struct
{ 
  BOOL    active;          // �L�����ǂ���
  VecFx32 pos;             // �����̈ʒu
  fx32    effectiveRange;  // �����͂�����
  int     maxVolume;       // �ő剹��(0�`127)
  u16     trackBit;        // ����g���b�N�}�X�N

} ISS_3DS_UNIT;


//===============================================================================
// ���V�X�e���f�[�^
//===============================================================================
typedef struct
{
  u32 soundIdx;     // BGM No.
  u16 holdTrackBit; // �V�X�e���N�����ɗ}����g���b�N�̃r�b�g�}�X�N
  u16 padding;

} ISS_BRIDGE_DATA;


//===============================================================================
// ���V�X�e�����[�N
//===============================================================================
struct _ISS_3DS_SYS
{
  HEAPID heapID;
  BOOL   boot;    // �N�����Ă��邩�ǂ���

  u8 masterVolume;           // �}�X�^�[�{�����[�� ( �ݒ�l )
  u8 practicalMasterVolume;  // �}�X�^�[�{�����[�� ( ���s�l )
  u8 masterVolumeFadeSpeed;  // �}�X�^�[�{�����[���̕ύX���x

  VecFx32 observerPos;        // �ϑ��҂̍��W
  VecFx32 observerTargetPos;  // �ϑ��҂̒����_���W

  ISS_3DS_UNIT unit[ ISS3DS_UNIT_NUM ];  // ���j�b�g

  ISS_BRIDGE_DATA* systemData;         // �V�X�e���f�[�^
  u8               systemDataNum;      // �V�X�e���f�[�^�̐�
  u8               currentSysDataIdx;  // �Q�ƒ��f�[�^�̃C���f�b�N�X
};


//===============================================================================
// ������J�֐�
//=============================================================================== 
// �V�X�e���f�[�^
static void InitSystemData          ( ISS_3DS_SYS* system );
static void LoadSystemData          ( ISS_3DS_SYS* system );
static void UnloadSystemData        ( ISS_3DS_SYS* system );
static void ChangeCurrentSystemData ( ISS_3DS_SYS* system, u8 nextDataIdx );
static u8   SearchSystemData        ( const ISS_3DS_SYS* system, u32 soundIdx );
// ���j�b�g
static void InitAllUnit ( ISS_3DS_SYS* system ); 
static void RegisterUnit( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, 
                          fx32 effectiveRange, int maxVolume );
static void SetUnitPos  ( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, const VecFx32* pos );
// �ϑ���
static void SetObserverPos( ISS_3DS_SYS* system, const VecFx32* pos, const VecFx32* targetPos );
// �V�X�e������
static void BootSystem( ISS_3DS_SYS* system );
static void StopSystem( ISS_3DS_SYS* system );
static void SystemMain( ISS_3DS_SYS* system );
static void ZoneChange( ISS_3DS_SYS* system );
// �{�����[�� / �p��
static void InitTrackVolume  ( const ISS_3DS_SYS* system );
static void UpdateTrackVolume( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx );
static void UpdateTrackPan   ( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx );
static int  CalcTrackVolume  ( const ISS_3DS_UNIT* unit, const VecFx32* observerPos );
static int  CalcTrackPan     ( const ISS_3DS_UNIT* unit, const VecFx32* observerPos, 
                               const VecFx32* observerTargetPos );
// �}�X�^�[�{�����[��
static void FadePracticalMasterVolume( ISS_3DS_SYS* system );
static u8   AdjustByMasterVolume     ( const ISS_3DS_SYS* system, u8 volume );
// �f�o�b�O
static void DebugPrint_unit( const ISS_3DS_UNIT* unit );
static void DebugPrint_systemData( const ISS_3DS_SYS* system );


//===============================================================================
// ���V�X�e���쐬/�j��/����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����쐬����
 *
 * @param heapID �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//-------------------------------------------------------------------------------
ISS_3DS_SYS* ISS_3DS_SYS_Create( HEAPID heapID )
{
  ISS_3DS_SYS* system;

  // ����
  system = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_3DS_SYS) );

  // ������
  system->heapID = heapID;
  system->boot   = FALSE;
  system->masterVolume = MAX_VOLUME;
  system->practicalMasterVolume = MAX_VOLUME;
  system->masterVolumeFadeSpeed = MASTER_VOLUME_FADE_SPEED;
  VEC_Set( &(system->observerPos), 0, 0, 0 );
  VEC_Set( &(system->observerTargetPos), 0, 0, 0 );
  InitAllUnit( system );
  InitSystemData( system );

  // �V�X�e���f�[�^�ǂݍ���
  LoadSystemData( system );

  return system;
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e����j������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_Delete( ISS_3DS_SYS* system )
{
  UnloadSystemData( system );
  GFL_HEAP_FreeMemory( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̓��쏈��
 *
 * @param system �������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_Main( ISS_3DS_SYS* system )
{
  SystemMain( system );
}


//===============================================================================
// ���V�X�e������
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_On( ISS_3DS_SYS* system )
{
  BootSystem( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_Off( ISS_3DS_SYS* system )
{
  StopSystem( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_ZoneChange( ISS_3DS_SYS* system )
{
  ZoneChange( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �}�X�^�[�{�����[����ݒ肷��
 *
 * @param system
 * @param volume �ݒ肷��{�����[���l [0, 127]
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_SetMasterVolume( ISS_3DS_SYS* system, u8 volume )
{
  // �{�����[����␳
  if( MAX_VOLUME < volume ){ volume = MAX_VOLUME; }

  // �ݒ�
  system->masterVolume( volume );
}


//===============================================================================
// �����j�b�g����
//=============================================================================== 

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g��o�^����
 *
 * @param system
 * @param unitIdx        �o�^���郆�j�b�g���w��
 * @param effectiveRange �����������鋗��
 * @param maxVolume      �ő�{�����[��
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_RegisterUnit( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, 
                               fx32 effectiveRange, int maxVolume )
{
  RegisterUnit( system, unitIdx, effectiveRange, maxVolume );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g���o�^�ς݂��ǂ������擾����
 *
 * @param system
 * @param unitIdx ���肷�郆�j�b�g���w��
 *
 * @return �w�肵�����j�b�g���o�^�ς݂Ȃ� TRUE
 *         �����łȂ��Ȃ�                 FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_3DS_SYS_IsUnitRegistered( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx )
{
  return system->unit[ unitIdx ].active;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�̍��W��ݒ肷��
 *
 * @param system
 * @param unitIdx ���j�b�g���w��
 * @param pos     �ݒ肷����W
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_SetUnitPos( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, const VecFx32* pos )
{
  SetUnitPos( system, unitIdx, pos );
}


//===============================================================================
// ���ϑ��� ����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �ϑ��҂̈ʒu��ݒ肷��
 *
 * @param system
 * @param pos       �ϑ��҂̍��W
 * @param targetPos �ϑ��҂̒����_���W
 */
//-------------------------------------------------------------------------------
void ISS_3DS_SYS_SetObserverPos( ISS_3DS_SYS* system, const VecFx32* pos, const VecFx32* targetPos )
{
  SetObserverPos( system, pos, targetPos );
}


//===============================================================================
// ������J�֐�
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���f�[�^������������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void InitSystemData( ISS_3DS_SYS* system )
{
  system->systemData        = NULL;
  system->systemDataNum     = 0;
  system->currentSysDataIdx = CURRENT_DATA_NONE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: init system data\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���f�[�^��ǂݍ���
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void LoadSystemData( ISS_3DS_SYS* system )
{
  HEAPID heapID;
  ARCHANDLE* handle;
  u8 dataNum;
  u32 datID;

  GF_ASSERT( system->systemData == NULL );

  heapID  = system->heapID;
  handle  = GFL_ARC_OpenDataHandle( ARCID_ISS_BRIDGE, heapID );
  dataNum = GFL_ARC_GetDataFileCntByHandle( handle );

  // ���[�N���m��
  system->systemDataNum = dataNum;
  system->systemData    = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_BRIDGE_DATA) * dataNum );

  // ���[�h
  for( datID=0; datID < dataNum; datID++ )
  {
    GFL_ARC_LoadDataByHandle( handle, datID, &(system->systemData[ datID ]) );
  } 
  GFL_ARC_CloseDataHandle( handle );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: load system data\n" );
  DebugPrint_systemData( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���f�[�^��j������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void UnloadSystemData( ISS_3DS_SYS* system )
{
  GF_ASSERT( system->systemData );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: unload system data\n" );

  // �j�� / ������
  GFL_HEAP_FreeMemory( system->systemData );
  InitSystemData( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �Q�Ƃ���V�X�e���f�[�^��ύX����
 *
 * @param system
 * @param nextDataIdx �V�����Q�ƃf�[�^�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
static void ChangeCurrentSystemData( ISS_3DS_SYS* system, u8 nextDataIdx )
{
  // �ω��Ȃ�
  if( system->currentSysDataIdx == nextDataIdx ){ return; }

  // �ύX
  system->currentSysDataIdx = nextDataIdx;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: update current system data index ==> %d\n", nextDataIdx ); 
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���f�[�^����������
 *
 * @param system
 * @param soundIdx ��������f�[�^�̃L�[�ƂȂ�BGM No.
 *
 * @return �w�肵��BGMNo.�ɑΉ�����V�X�e���f�[�^�̃C���f�b�N�X
 *         �Ή�����f�[�^�������ꍇ CURRENT_DATA_NONE
 */
//-------------------------------------------------------------------------------
static u8 SearchSystemData( const ISS_3DS_SYS* system, u32 soundIdx )
{
  u8 dataIdx;
  u8 dataNum;
  const ISS_BRIDGE_DATA* dataArray;
  const ISS_BRIDGE_DATA* data;

  // �f�[�^���ǂݍ��܂�Ă��Ȃ�
  if( system->systemData == NULL )
  {
    OS_Printf( "ISS-B: don't have system data\n" );
    GF_ASSERT(0);
    return CURRENT_DATA_NONE;
  }

  dataNum   = system->systemDataNum;
  dataArray = system->systemData;

  // ����
  for( dataIdx=0; dataIdx < dataNum; dataIdx++ )
  {
    // ����
    if( dataArray[ dataIdx ].soundIdx == soundIdx ){ return dataIdx; } 
  } 

  // �����ł���
  return CURRENT_DATA_NONE;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ׂẴ��j�b�g������������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void InitAllUnit( ISS_3DS_SYS* system )
{
  ISS3DS_UNIT_INDEX unitIdx;

  // ������
  for( unitIdx=ISS3DS_UNIT_TRACK01; unitIdx < ISS3DS_UNIT_NUM; unitIdx++ )
  {
    system->unit[ unitIdx ].active    = FALSE;
    system->unit[ unitIdx ].maxVolume = MAX_VOLUME;
    system->unit[ unitIdx ].trackBit  = ( 1 << unitIdx );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: init all unit\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g��o�^����
 *
 * @param system
 * @param unitIdx        �o�^���郆�j�b�g���w��
 * @param effectiveRange �����������鋗��
 * @param maxVolume      �ő�{�����[��
 */
//-------------------------------------------------------------------------------
void RegisterUnit( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, 
                   fx32 effectiveRange, int maxVolume )
{
  ISS_3DS_UNIT* unit;

  unit = &( system->unit[ unitIdx ] );

  // ���łɓo�^�ς�
  if( unit->active == TRUE )
  {
    OS_Printf( "ISS-B: unit %d is already registered\n", unitIdx );
    GF_ASSERT(0);
    return;
  }

  // �o�^
  unit->active         = TRUE;
  unit->effectiveRange = effectiveRange;
  unit->maxVolume      = maxVolume;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: register unit no.%d\n", unitIdx );
  DebugPrint_unit( unit );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�̍��W��ݒ肷��
 *
 * @param system
 * @param unitIdx ���j�b�g���w��
 * @param pos     �ݒ肷����W
 */
//-------------------------------------------------------------------------------
static void SetUnitPos( ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx, const VecFx32* pos )
{
  ISS_3DS_UNIT* unit;

  unit = &( system->unit[ unitIdx ] );

  // ���j�b�g���o�^����Ă��Ȃ�
  if( unit->active == FALSE )
  {
    OS_Printf( "ISS-B: unit no.%d is not registered\n", unitIdx );
    GF_ASSERT(0);
    return;
  }

  // ���W���X�V
  VEC_Set( &(unit->pos), pos->x, pos->y, pos->z );
}

//-------------------------------------------------------------------------------
/**
 * @brief �ϑ��҂̈ʒu��ݒ肷��
 *
 * @param system
 * @param pos       �ϑ��҂̍��W
 * @param targetPos �ϑ��҂̒����_���W
 */
//-------------------------------------------------------------------------------
static void SetObserverPos( ISS_3DS_SYS* system, const VecFx32* pos, const VecFx32* targetPos )
{
  VEC_Set( &(system->observerPos), pos->x, pos->y, pos->z );
  VEC_Set( &(system->observerTargetPos), targetPos->x, targetPos->y, targetPos->z );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @parma system
 */
//-------------------------------------------------------------------------------
static void BootSystem( ISS_3DS_SYS* system )
{
  // �N���ς�
  if( system->boot ){ return; }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: boot system\n" );

  // �N��
  system->boot = TRUE; 

  // �Q�Ƃ���V�X�e���f�[�^���X�V
  {
    u32 soundIdx;
    u8 nextDataIdx;

    soundIdx    = PMSND_GetBGMsoundNo();
    nextDataIdx = SearchSystemData( system, soundIdx );
    ChangeCurrentSystemData( system, nextDataIdx );
  }

  // �{�����[����������
  InitTrackVolume( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @parma system
 */
//-------------------------------------------------------------------------------
static void StopSystem( ISS_3DS_SYS* system )
{ 
  // ��~�ς�
  if( system->boot == FALSE ){ return; }

  // ��~
  system->boot = FALSE;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "ISS-B: stop system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void SystemMain( ISS_3DS_SYS* system )
{
  ISS3DS_UNIT_INDEX unitIdx;

  // �N�����Ă��Ȃ�
  if( system->boot == FALSE ){ return; }

  // �}�X�^�[�{�����[���X�V
  FadePracticalMasterVolume( system );

  // �{�����[���ƃp�����X�V
  for( unitIdx=ISS3DS_UNIT_TRACK01; unitIdx < ISS3DS_UNIT_NUM; unitIdx++ )
  {
    UpdateTrackVolume( system, unitIdx );
    UpdateTrackPan( system, unitIdx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���`�F���W���̓���
 */
//-------------------------------------------------------------------------------
static void ZoneChange( ISS_3DS_SYS* system )
{
  // �S���j�b�g������
  InitAllUnit( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief BGM�{�����[��������������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void InitTrackVolume( const ISS_3DS_SYS* system )
{
  const ISS_BRIDGE_DATA* systemData;

  // �Q�Ƃ���f�[�^������
  if( system->currentSysDataIdx == CURRENT_DATA_NONE )
  {
    OS_Printf( "ISS-B: not found system data\n" );;
    GF_ASSERT(0);
    return;
  } 

  // �{�����[��������
  systemData = &( system->systemData[ system->currentSysDataIdx ] );
  PMSND_ChangeBGMVolume( systemData->holdTrackBit, INITIAL_VOLUME );
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵�����j�b�g�̃g���b�N�{�����[�����X�V����
 *
 * @param system
 * @param unitIdx ���j�b�g���w��
 */
//-------------------------------------------------------------------------------
static void UpdateTrackVolume( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx )
{ 
  const ISS_3DS_UNIT* unit;
  int volume;

  unit = &( system->unit[ unitIdx ] );

  // ���j�b�g���L���łȂ�
  if( unit->active == FALSE ){ return; }

  // �{�����[���X�V
  volume = CalcTrackVolume( unit, &(system->observerPos) ); 
  volume = AdjustByMasterVolume( system, volume );
  PMSND_ChangeBGMVolume( unit->trackBit, volume );

  // DEBUG:
#if 0
  OS_TFPrintf( PRINT_TARGET, 
               "ISS-B: update track volume (unit[%d] ==> %d)\n", unitIdx, volume );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵�����j�b�g�̃g���b�N�p�����X�V����
 *
 * @param system
 * @param unitIdx ���j�b�g���w��
 */
//-------------------------------------------------------------------------------
static void UpdateTrackPan( const ISS_3DS_SYS* system, ISS3DS_UNIT_INDEX unitIdx )
{
  const ISS_3DS_UNIT* unit;
  int pan;

  unit = &( system->unit[ unitIdx ] );

  // ���j�b�g���L���łȂ�
  if( unit->active == FALSE ){ return; }

  // �p����ݒ�
  pan = CalcTrackPan( unit, &(system->observerPos), &(system->observerTargetPos) ); 
  PMSND_SetStatusBGM_EX( unit->trackBit, 256, 0, pan );

  // DEBUG:
#if 0
  OS_TFPrintf( PRINT_TARGET, 
               "ISS-B: update track pan (unit[%d] ==> %d)\n", unitIdx, pan );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �{�����[�����v�Z����
 *
 * @param unit        �v�Z�Ώۂ̃��j�b�g
 * @param observerPos �ϑ��҂̈ʒu
 *
 * @return �w�肵�����j�b�g�����삷��g���b�N�̃{�����[��
 */
//-------------------------------------------------------------------------------
static int CalcTrackVolume( const ISS_3DS_UNIT* unit, const VecFx32* observerPos )
{
  int volume;

  GF_ASSERT( unit );
  GF_ASSERT( unit->active );

  // �{�����[�����v�Z
  {
    VecFx32 vecToUnit;
    fx32 dist, range;

    // �������v�Z
    VEC_Subtract( &(unit->pos), observerPos, &vecToUnit );
    dist  = VEC_Mag( &vecToUnit );
    range = unit->effectiveRange;

    // �{�����[������
    if( range < dist )  
    { // �����͂��Ȃ�����
      volume = 0;
    }
    else                
    { // �����͂�����
      float rate = FX_FX32_TO_F32( FX_Div( range - dist, range ) ); 
      volume     = unit->maxVolume * rate;
    }
  } 

  return volume;
}

//-------------------------------------------------------------------------------
/**
 * @brief �p�����v�Z����
 *
 * @param unit              �v�Z�Ώۂ̃��j�b�g
 * @param observerPos       �ϑ��҂̍��W
 * @param observerTargetPos �ϑ��҂̒����_���W
 *
 * @return �w�肵�����j�b�g�����삷��g���b�N�̃p��
 */
//-------------------------------------------------------------------------------
static int CalcTrackPan( const ISS_3DS_UNIT* unit, const VecFx32* observerPos, const VecFx32* observerTargetPos )
{
  int pan;

  GF_ASSERT( unit );
  GF_ASSERT( unit->active );

  // �p�����v�Z
  {
    VecFx32 v0, v1, v2, v3, vup, vp;
    VecFx32 cx, cy, cz;
    VecFx32 camPos, targetPos, unitPos;
    fx32 dot;

    // �J�����E�����_�E�������ꂼ��̈ʒu
    camPos    = *observerPos;
    targetPos = *observerTargetPos;
    unitPos   = unit->pos;

    // �J�����̌����x�N�g��v0
    VEC_Subtract( &targetPos, &camPos, &v0 );

    // �J�����������x�N�g��v1
    VEC_Subtract( &unitPos, &camPos, &v1 );

    // �J��������x�N�g��vup
    VEC_Set( &vup, 0, FX32_ONE, 0 );

    // �J�������W��cx, cy, cz
    VEC_Normalize( &v0, &cz );
    VEC_CrossProduct( &cz, &vup, &cx );   VEC_Normalize( &cx, &cx );
    VEC_CrossProduct( &cx, &cz, &cy );    VEC_Normalize( &cy, &cy );

    // v1���J�������W�n��xz���ʂ֎ˉe�����x�N�g��v2
    VEC_Normalize( &cy, &cy );
    dot = VEC_DotProduct( &v1, &cy );
    VEC_Set( &vp, FX_Mul(cy.x, dot), FX_Mul(cy.y, dot), FX_Mul(cy.z, dot) );
    VEC_Subtract( &v1, &vp, &v2 );  // v1��cy����������
    VEC_Normalize( &v2, &v2 );

    // �p�� = cx�Ev2
    dot = VEC_DotProduct( &cx, &v2 );
    pan = 128 * FX_FX32_TO_F32(dot);

    // �p���̒l��␳
    if( pan < -128 ) pan = -128;
    if( 127 < pan  ) pan =  127;
  }
  
  return pan;
} 

//-------------------------------------------------------------------------------
/**
 * @brief �}�X�^�[�{�����[���̎��s�l���X�V����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void FadePracticalMasterVolume( ISS_3DS_SYS* system )
{
  int diff;
  int add;

  // ���s�l == �ݒ�l
  if( system->practicalMasterVolume == system->masterVolume ){ return; }

  // �ݒ�l�܂ł̍���
  diff = system->masterVolume - system->practicalMasterVolume;

  // ���Z�l������
  if( ABS(diff) < system->masterVolumeFadeSpeed )
  {
    add = diff;
  }
  else if( diff < 0 )
  {
    add = -( system->masterVolumeFadeSpeed );
  }
  else
  {
    add = system->masterVolumeFadeSpeed;
  }

  // �}�X�^�[�{�����[�� ( ���s�l ) �X�V
  system->practicalMasterVolume += add;

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, 
               "ISS-B: update master volume ==> %d\n", system->practicalMasterVolume );
}

//-------------------------------------------------------------------------------
/**
 * @brief �}�X�^�[�{�����[���ɂ��␳��������
 *
 * @param system
 * @param volume �␳��������O�̉���
 *
 * @return �␳����������̉���
 */
//-------------------------------------------------------------------------------
static u8 AdjustByMasterVolume( const ISS_3DS_SYS* system, u8 volume )
{
  u32 v;

  v = system->practicalMasterVolume * volume;
  v = v / MAX_VOLUME;

  GF_ASSERT( MAX_VOLUME < v );

  return v;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�̃p�����[�^���o�͂���
 *
 * @param unit
 */
//-------------------------------------------------------------------------------
static void DebugPrint_unit( const ISS_3DS_UNIT* unit )
{
  // �L�����ǂ���
  OS_TFPrintf( PRINT_TARGET, "ISS-B-UNIT: active = %d\n", unit->active );

  // ���W
  OS_TFPrintf( PRINT_TARGET, 
              "ISS-B-UNIT: pos = (%d, %d, %d)\n", 
              FX_Whole(unit->pos.x), FX_Whole(unit->pos.y), FX_Whole(unit->pos.z) );

  // �����͂�����
  OS_TFPrintf( PRINT_TARGET, 
               "ISS-B-UNIT: effectiveRange = %d\n", FX_Whole(unit->effectiveRange) );

  // �ő剹��
  OS_TFPrintf( PRINT_TARGET, "ISS-B-UNIT: maxVolume = %d\n", unit->maxVolume );

  // ����g���b�N�}�X�N
  OS_TFPrintf( PRINT_TARGET, "ISS-B-UNIT: trackBit = %x\n", unit->trackBit );
} 

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���f�[�^��\������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void DebugPrint_systemData( const ISS_3DS_SYS* system )
{
  u8 dataNum;
  u8 dataIdx;
  u8 bitIdx;
  const ISS_BRIDGE_DATA* data;

  dataNum = system->systemDataNum;

  for( dataIdx=0; dataIdx < dataNum; dataIdx++ )
  {
    data = &( system->systemData[ dataIdx ] );

    // BGM No.
    OS_TFPrintf( PRINT_TARGET, "ISS-B-SYSTEM-DATA[%d]: soundIdx = %d\n", dataIdx, data->soundIdx );

    // �r�b�g�}�X�N
    OS_TFPrintf( PRINT_TARGET, "ISS-B-SYSTEM-DATA[%d]: holdTrackBit = ", dataIdx );
    for( bitIdx=0; bitIdx<16; bitIdx++ )
    {
      u16 mask = ( 1 << (15 - bitIdx) );
      if( mask & data->holdTrackBit ){ OS_TFPrintf( PRINT_TARGET, "��" ); }
      else{ OS_TFPrintf( PRINT_TARGET, "��" ); }
    }
    OS_TFPrintf( PRINT_TARGET, "\n" );
  }
}
