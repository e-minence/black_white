/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISS�X�C�b�`�V�X�e��
 * @file iss_switch_sys.c
 * @author obata
 * @date 2009.11.17
 */
/////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_switch_sys.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"  // for ARCID_ISS_SWITCH


//===============================================================================
// ���萔
//===============================================================================
#define PRINT_DEST       (1)       // �f�o�b�O���̏o�͐�
#define TRACK_NUM        (16)      // �g���b�N��
#define TRACKBIT_ALL     (0xffff)  // �S�g���b�N�w��
#define MAX_VOLUME       (127)     // �{�����[���ő�l
#define SWITCH_DATA_NONE (0xff)    // �Q�ƃf�[�^�̖����C���f�b�N�X

// �X�C�b�`�̏��
typedef enum{
  SWITCH_STATE_ON,       // ON
  SWITCH_STATE_OFF,      // OFF
  SWITCH_STATE_FADE_IN,  // �t�F�[�h �C��
  SWITCH_STATE_FADE_OUT, // �t�F�[�h �A�E�g
} SWITCH_STATE; 


//===============================================================================
// ���X�C�b�`�f�[�^
//===============================================================================
typedef struct
{ 
  u32  soundIdx;               // �V�[�P���X�ԍ�
  u16  trackBit[ SWITCH_NUM ]; // �e�X�C�b�`�̑���Ώۃg���b�N�r�b�g
  u16  fadeFrame;              // �t�F�[�h����
  u16  validZoneNum;           // �X�C�b�`���L���ȏꏊ�̐�
  u16* validZone;              // �X�C�b�`���L���ȏꏊ

} SWITCH_DATA;


//===============================================================================
// ��ISS-S�V�X�e��
//===============================================================================
struct _ISS_SWITCH_SYS
{ 
  HEAPID heapID;
  BOOL   boot;    // �N�����Ă��邩�ǂ���

  // �X�C�b�`���
  SWITCH_STATE switchState[ SWITCH_NUM ]; // �e�X�C�b�`�̏��
  u16          fadeCount  [ SWITCH_NUM ]; // �t�F�[�h�J�E���^

  // �X�C�b�`�f�[�^
  SWITCH_DATA* switchData;     // �X�C�b�`�f�[�^�z��
  u8           switchDataNum;  // �X�C�b�`�f�[�^�̐�
  u8           switchDataIdx;  // �Q�ƃf�[�^�̃C���f�b�N�X
};


//===============================================================================
// ���V�X�e������
//===============================================================================
// �V�X�e������
static void SystemMain( ISS_SWITCH_SYS* sys );
static void SwitchMain( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
static void SwitchMain_FADE_IN( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
static void SwitchMain_FADE_OUT( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
// �V�X�e������
static void BootSystem( ISS_SWITCH_SYS* sys );
static void StopSystem( ISS_SWITCH_SYS* sys ); 
static void NotifyZoneChange( ISS_SWITCH_SYS* sys, u16 nextZoneID );
// �X�C�b�`����
static void SwitchOn ( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
static void SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );

//===============================================================================
// ���f�[�^����
//===============================================================================
// �X�C�b�`�f�[�^
static u8 SearchSwitchDataIndex( const ISS_SWITCH_SYS* sys, u32 soundIdx ); 
// �Q�ƒ��X�C�b�`�f�[�^
static const SWITCH_DATA* GetCurrentSwitchData   ( const ISS_SWITCH_SYS* sys );
static BOOL               ChangeCurrentSwitchData( ISS_SWITCH_SYS* sys );
static BOOL               CheckZoneIsValid       ( const ISS_SWITCH_SYS* sys, u16 zoneID );
// �X�C�b�`���
static SWITCH_STATE GetSwitchState( const ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx );
static void         SetSwitchState( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx, SWITCH_STATE switchState );
static void         InitSwitchState ( ISS_SWITCH_SYS* sys );
static void         ResetSwitchState( ISS_SWITCH_SYS* sys ); 
// BGM���ʑ���
static void SetBGMVolume( const ISS_SWITCH_SYS* sys );

//===============================================================================
// ���쐬/�j��
//===============================================================================
// �X�C�b�`�f�[�^
static void InitSwitchData     ( ISS_SWITCH_SYS* sys );
static void LoadAllSwitchData  ( ISS_SWITCH_SYS* sys );
static void UnloadAllSwitchData( ISS_SWITCH_SYS* sys );
static void LoadSwitchData  ( SWITCH_DATA* switchData, ARCDATID datID, HEAPID heapID );
static void UnloadSwitchData( SWITCH_DATA* switchData );

//===============================================================================
// ���f�o�b�O�o��
//===============================================================================
static void DebugPrint_SwitchData( const ISS_SWITCH_SYS* sys );


//===============================================================================
// ���쐬�E�j��
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief ISS�X�C�b�`�V�X�e�����쐬����
 * 
 * @param heapID �g�p����q�[�vID
 *
 * @return �쐬�����V�X�e��
 */
//-------------------------------------------------------------------------------
ISS_SWITCH_SYS* ISS_SWITCH_SYS_Create( HEAPID heapID )
{
  ISS_SWITCH_SYS* sys;

  // ����
  sys = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SWITCH_SYS) );

  // ������
  sys->heapID = heapID;
  sys->boot   = FALSE;
  InitSwitchState( sys );
  InitSwitchData( sys );

  // �����ݒ�
  LoadAllSwitchData( sys );
  DebugPrint_SwitchData( sys );

  return sys;
}

//-------------------------------------------------------------------------------
/**
 * @brief ISS�X�C�b�`�V�X�e����j������
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Delete( ISS_SWITCH_SYS* sys )
{ 
  UnloadAllSwitchData( sys );
  GFL_HEAP_FreeMemory( sys );
}


//===============================================================================
// ������
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���E���C������
 *
 * @param sys �������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Update( ISS_SWITCH_SYS* sys )
{
  SystemMain( sys );
}


//===============================================================================
// ���V�X�e������
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_On( ISS_SWITCH_SYS* sys )
{
  BootSystem( sys );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys ��~����V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Off( ISS_SWITCH_SYS* sys )
{
  StopSystem( sys );
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���̕ύX��ʒm����
 * 
 * @param sys
 * @param zoneID �ύX��̃]�[��ID
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_ZoneChange( ISS_SWITCH_SYS* sys, u16 zoneID )
{
  NotifyZoneChange( sys, zoneID );
}


//===============================================================================
// ���X�C�b�`����
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� ON �ɂ���
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SwitchOn( sys, idx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� OFF �ɂ���
 *
 * @param sys �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  SwitchOff( sys, idx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� ON/OFF ���擾����
 *
 * @param sys �擾�ΏۃV�X�e��
 * @param idx ���肷��X�C�b�`�ԍ�
 *
 * @return �X�C�b�`idx��������Ă���ꍇ TRUE, �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_SYS_IsSwitchOn( const ISS_SWITCH_SYS* sys, SWITCH_INDEX idx )
{
  if( SWITCH_MAX < idx )
  {
    OS_TFPrintf( PRINT_DEST, "ISS-S: switch index range over\n" );
    return FALSE;
  }
  return (sys->switchState[idx] == SWITCH_STATE_ON);
}


//===============================================================================
// ������J�֐�
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵���X�C�b�`�f�[�^��ǂݍ���
 *
 * @param switchData �ǂݍ��񂾃f�[�^�̊i�[��
 * @param datID      �ǂݍ��ރf�[�^���w��
 * @param heapID     �g�p����q�[�vID
 */
//-------------------------------------------------------------------------------
static void LoadSwitchData( SWITCH_DATA* switchData, ARCDATID datID, HEAPID heapID )
{
  int offset = 0;
  int size = 0;

  // �V�[�P���X�ԍ�
  size = sizeof(u32);
  GFL_ARC_LoadDataOfs( &(switchData->soundIdx), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �e�X�C�b�`�̑���Ώۃg���b�N�r�b�g
  size = sizeof(u16) * SWITCH_NUM;
  GFL_ARC_LoadDataOfs( &(switchData->trackBit), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �t�F�[�h����
  size = sizeof(u16);
  GFL_ARC_LoadDataOfs( &(switchData->fadeFrame), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �X�C�b�`���L���ȏꏊ�̐�
  size = sizeof(u16);
  GFL_ARC_LoadDataOfs( &(switchData->validZoneNum), ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;

  // �X�C�b�`���L���ȏꏊ
  size = sizeof(u16) * switchData->validZoneNum;
  switchData->validZone = GFL_HEAP_AllocMemory( heapID, size );
  GFL_ARC_LoadDataOfs( switchData->validZone, ARCID_ISS_SWITCH, datID, offset, size );
  offset += size;
}

//-------------------------------------------------------------------------------
/**
 * @brief �ǂݍ��񂾃X�C�b�`�f�[�^��j������
 *
 * @param switchData �j������f�[�^
 */
//-------------------------------------------------------------------------------
static void UnloadSwitchData( SWITCH_DATA* switchData )
{
  if( switchData == NULL ){ return; }

  if( switchData->validZone )
  {
    GFL_HEAP_FreeMemory( switchData->validZone );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ݎQ�ƒ��̃X�C�b�`�f�[�^���擾����
 *
 * @param sys
 *
 * @return �Q�ƒ��̃X�C�b�`�f�[�^
 *         �Q�ƒ��̃f�[�^���Ȃ��ꍇ NULL
 */
//-------------------------------------------------------------------------------
static const SWITCH_DATA* GetCurrentSwitchData( const ISS_SWITCH_SYS* sys )
{
  // �Q�ƒ��̃f�[�^���Ȃ�
  if( sys->switchDataIdx == SWITCH_DATA_NONE ){ return NULL; }

  // �Q�ƒ��̃f�[�^��Ԃ�
  return &( sys->switchData[ sys->switchDataIdx ] );
}

//-------------------------------------------------------------------------------
/**
 * @brief �Q�Ƃ���X�C�b�`�f�[�^���X�V����
 *
 * @param sys
 *
 * @return �Q�ƃf�[�^���ω������� TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL ChangeCurrentSwitchData( ISS_SWITCH_SYS* sys )
{
  u32 soundIdx;
  u8 nextDataIdx;

  // �Đ�����BGM�̃X�C�b�`�f�[�^�ɕύX
  soundIdx    = PMSND_GetBGMsoundNo(); 
  nextDataIdx = SearchSwitchDataIndex( sys, soundIdx );

  if( nextDataIdx == SWITCH_DATA_NONE ){ return FALSE; } // �f�[�^�����݂��Ȃ�
  if( nextDataIdx == sys->switchDataIdx ){ return FALSE; } // �ω��Ȃ�

  // �X�V
  sys->switchDataIdx = nextDataIdx;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, 
               "ISS-S: change switch data index ==> %d\n", nextDataIdx );

  return TRUE;
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[�����X�C�b�`��Ԉێ��̑Ώۂ��ǂ����𒲂ׂ�
 *
 * @param sys
 * @param zoneID ���ׂ�]�[��
 *
 * @return �X�C�b�`��Ԉێ��̑ΏۂȂ� TRUE
 *         �����łȂ��Ȃ� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckZoneIsValid ( const ISS_SWITCH_SYS* sys, u16 zoneID )
{
  int i;
  const SWITCH_DATA* switchData;

  // �Q�ƒ��̃X�C�b�`�f�[�^�擾
  switchData = GetCurrentSwitchData( sys );

  if( switchData == NULL ){ return NULL; } // �Q�ƒ��̃f�[�^�����݂��Ȃ�

  // �f�[�^������
  for( i=0; i < switchData->validZoneNum; i++ )
  {
    // ����
    if( switchData->validZone[i] == zoneID ){ return TRUE; }
  }
  return FALSE;
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵��BGM�ɑΉ�����X�C�b�`�f�[�^�̃C���f�b�N�X����������
 *
 * @param sys
 * @param soundIdx �f�[�^�̃C���f�b�N�X���擾����BGM
 *
 * @return �w�肵��BGM�̃X�C�b�`�f�[�^�̃C���f�b�N�X
 *         �X�C�b�`�f�[�^�����݂��Ȃ��ꍇ SWITCH_DATA_NONE
 */
//-------------------------------------------------------------------------------
static u8 SearchSwitchDataIndex( const ISS_SWITCH_SYS* sys, u32 soundIdx )
{
  SWITCH_DATA* switchData;
  u8 dataNum;
  u8 dataIdx;

  switchData = sys->switchData;
  dataNum    = sys->switchDataNum;

  // ����
  for( dataIdx=0; dataIdx < dataNum; dataIdx++ )
  {
    // ����
    if( switchData[ dataIdx ].soundIdx == soundIdx )
    {
      return dataIdx;
    }
  }

  // �f�[�^���Ȃ�
  return SWITCH_DATA_NONE;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�f�[�^������������
 * 
 * @param sys
 */
//-------------------------------------------------------------------------------
static void InitSwitchData( ISS_SWITCH_SYS* sys )
{
  sys->switchData    = NULL;
  sys->switchDataNum = 0;
  sys->switchDataIdx = SWITCH_DATA_NONE;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: init switch data\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief ���ׂẴX�C�b�`�f�[�^��ǂݍ���
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void LoadAllSwitchData( ISS_SWITCH_SYS* sys )
{
  int dataIdx;
  u8 dataNum;
  ARCHANDLE* handle;

  // �A�[�J�C�u�n���h���I�[�v��
  handle = GFL_ARC_OpenDataHandle( ARCID_ISS_SWITCH, sys->heapID );

  // �o�b�t�@���m��
  dataNum = GFL_ARC_GetDataFileCntByHandle( handle );
  sys->switchDataNum = dataNum;
  sys->switchData    = GFL_HEAP_AllocMemory( sys->heapID, sizeof(SWITCH_DATA) * dataNum );

  // �A�[�J�C�u�n���h���N���[�Y
  GFL_ARC_CloseDataHandle( handle );

  // �S�Ẵf�[�^��ǂݍ���
  for( dataIdx=0; dataIdx<dataNum; dataIdx++ )
  {
    LoadSwitchData( &(sys->switchData[ dataIdx ]), dataIdx, sys->heapID );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: load all switch data\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �ǂݍ��񂾑S�ẴX�C�b�`�f�[�^��j������
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void UnloadAllSwitchData( ISS_SWITCH_SYS* sys )
{
  int dataIdx;
  int dataNum;
  SWITCH_DATA* data;

  dataNum = sys->switchDataNum;
  data    = sys->switchData;

  // �z��v�f��j��
  for( dataIdx=0; dataIdx < dataNum; dataIdx++ )
  {
    UnloadSwitchData( &(data[ dataIdx ]) );
  }
  // �z��{�̂�j��
  GFL_HEAP_FreeMemory( data );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: unload all switch data\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ��擾����
 *
 * @param sys
 * @param switchIdx ��Ԃ��擾����X�C�b�`���w��
 *
 * @return �w�肵���X�C�b�`�̏��
 */
//-------------------------------------------------------------------------------
static SWITCH_STATE GetSwitchState( const ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  // �C���f�b�N�X �G���[
  if( SWITCH_NUM < switchIdx )
  {
    OS_Printf( "ISS-S: switch index error\n" );
    GF_ASSERT(0);
    return SWITCH_STATE_OFF;
  }

  return sys->switchState[ switchIdx ];
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ�ݒ肷��
 *
 * @param sys
 * @param switchIdx   ��Ԃ��擾����X�C�b�`���w��
 * @param switchState �ݒ肷����
 */
//-------------------------------------------------------------------------------
static void SetSwitchState( ISS_SWITCH_SYS* sys, 
                            SWITCH_INDEX switchIdx, SWITCH_STATE switchState )
{
  // �C���f�b�N�X �G���[
  if( SWITCH_NUM < switchIdx )
  {
    OS_Printf( "ISS-S: switch index error\n" );
    GF_ASSERT(0);
    return;
  }

  sys->switchState[ switchIdx ] = switchState;
  sys->fadeCount  [ switchIdx ] = 0;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`��Ԃ�����������
 *
 * @param sys �������ΏۃV�X�e��
 */
//-------------------------------------------------------------------------------
static void InitSwitchState( ISS_SWITCH_SYS* sys )
{
  SWITCH_INDEX switchIdx;

  // �S�X�C�b�`��������
  for( switchIdx=SWITCH_00; switchIdx < SWITCH_NUM; switchIdx++ )
  {
    SetSwitchState( sys, switchIdx, SWITCH_STATE_OFF );
  }

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: init switch state\n" );
} 

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`0�݂̂�������Ă����ԂɃ��Z�b�g����
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void ResetSwitchState( ISS_SWITCH_SYS* sys )
{
  InitSwitchState( sys );
  SetSwitchState( sys, SWITCH_00, SWITCH_STATE_ON );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: reset switch state\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���N��
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void BootSystem( ISS_SWITCH_SYS* sys )
{
  if( sys->boot ){ return; } // ���łɋN�����Ă���

  // �N��
  sys->boot = TRUE;

  // �Q�Ƃ���X�C�b�`�f�[�^���ς������, ���Z�b�g����
  if( ChangeCurrentSwitchData( sys ) ){ ResetSwitchState( sys ); }

  // BGM���ʂ�ݒ�
  SetBGMVolume( sys );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: boot system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys ��~������V�X�e��
 */
//-------------------------------------------------------------------------------
static void StopSystem( ISS_SWITCH_SYS* sys )
{
  if( sys->boot == FALSE ){ return; } // ���łɒ�~���Ă���

  // ��~
  sys->boot = FALSE;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: stop system\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���̕ω���ʒm����
 *
 * @param sys
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-------------------------------------------------------------------------------
static void NotifyZoneChange( ISS_SWITCH_SYS* sys, u16 nextZoneID )
{
  // �X�C�b�`��Ԉێ� �L���]�[��
  if( CheckZoneIsValid( sys, nextZoneID ) )
  {
    // DEBUG:
    OS_TFPrintf( PRINT_DEST, "ISS-S: detect zone change ==> valid zone\n" );
    return;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: detect zone change ==> invalid zone\n" );

  // ���Z�b�g ( BGM ���ʑ���Ȃ� )
  ResetSwitchState( sys ); 
}

//-------------------------------------------------------------------------------
/**
 * @beirf �X�C�b�`������
 *
 * @param sys 
 * @param switchIdx �����X�C�b�`�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
static void SwitchOn( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  // �N�����Ă��Ȃ�
  if( !sys->boot ){ return; }
  // �X�C�b�`���������Ԃ���Ȃ�
  if( GetSwitchState( sys, switchIdx ) != SWITCH_STATE_OFF ){ return; }

  // �X�C�b�`ON ( �t�F�[�h�C���J�n )
  SetSwitchState( sys, switchIdx, SWITCH_STATE_FADE_IN );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: switch %d on\n", switchIdx );
}

//-------------------------------------------------------------------------------
/**
 * @beirf �X�C�b�`�𗣂�
 *
 * @param sys
 * @param switchIdx �����X�C�b�`�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
static void SwitchOff( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  // �N�����Ă��Ȃ�
  if( !sys->boot ){ return; }
  // �X�C�b�`���������Ԃ���Ȃ�
  if( GetSwitchState( sys, switchIdx ) != SWITCH_STATE_ON ){ return; }

  // �X�C�b�`OFF ( �t�F�[�h�A�E�g�J�n )
  SetSwitchState( sys, switchIdx, SWITCH_STATE_FADE_OUT );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-S: switch %d off\n", switchIdx );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void SystemMain( ISS_SWITCH_SYS* sys )
{
  SWITCH_INDEX switchIdx;
  
  if( sys->boot == FALSE ){ return; } // �N�����Ă��Ȃ�
  if( GetCurrentSwitchData( sys ) == NULL ){ return; } // �Q�ƃf�[�^���Ȃ�

  // �X�C�b�`����
  for( switchIdx=SWITCH_00; switchIdx < SWITCH_NUM; switchIdx++ )
  {
    SwitchMain( sys, switchIdx );
  }

  // �X�C�b�`�̏�Ԃ�BGM�ɔ��f
  SetBGMVolume( sys );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`����
 *
 * @param sys 
 * @param switchIdx �X�V����X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
static void SwitchMain( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  switch( sys->switchState[ switchIdx ] )
  {
  case SWITCH_STATE_ON:
  case SWITCH_STATE_OFF:
    // �ω����Ȃ�
    break;
  case SWITCH_STATE_FADE_IN:   SwitchMain_FADE_IN( sys, switchIdx );   break;
  case SWITCH_STATE_FADE_OUT:  SwitchMain_FADE_OUT( sys, switchIdx );  break;
  default:
    // �G���[
    OS_TFPrintf( PRINT_DEST, "ISS-S: switch state error\n" ); 
    GF_ASSERT(0);
    break;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���� ( FADE_IN )
 *
 * @param sys
 * @param switchIdx ���삷��X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
static void SwitchMain_FADE_IN( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  const SWITCH_DATA* switchData;

  // �Q�ƃf�[�^�擾
  switchData = GetCurrentSwitchData( sys );

  // �Q�ƃf�[�^�����݂��Ȃ�
  if( switchData == NULL )
  {
    OS_Printf( "ISS-S: switch data not found\n" );
    GF_ASSERT(0);
    return;
  }

  // �t�F�[�h�J�E���g�X�V
  sys->fadeCount[ switchIdx ]++;

  // �t�F�[�h�C���I��
  if( switchData->fadeFrame <= sys->fadeCount[ switchIdx ] )
  {
    // ON
    SetSwitchState( sys, switchIdx, SWITCH_STATE_ON );

    // DEBUG:
    OS_TFPrintf( PRINT_DEST, "ISS-S: switch %d fade in finish\n", switchIdx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`���� ( FADE_OUT )
 *
 * @param sys
 * @param switchIdx ���삷��X�C�b�`���w��
 */
//-------------------------------------------------------------------------------
static void SwitchMain_FADE_OUT( ISS_SWITCH_SYS* sys, SWITCH_INDEX switchIdx )
{
  const SWITCH_DATA* switchData;

  // �Q�ƃf�[�^
  switchData = GetCurrentSwitchData( sys ); 

  // �Q�ƃf�[�^�����݂��Ȃ�
  if( switchData == NULL )
  {
    OS_Printf( "ISS-S: switch data not found\n" );
    GF_ASSERT(0);
    return;
  }

  // �t�F�[�h�J�E���g�X�V
  sys->fadeCount[ switchIdx ]++;

  // �t�F�[�h�A�E�g�I��
  if( switchData->fadeFrame <= sys->fadeCount[ switchIdx ] )
  {
    // OFF
    SetSwitchState( sys, switchIdx, SWITCH_STATE_OFF );

    // DEBUG:
    OS_TFPrintf( PRINT_DEST, "ISS-S: switch %d fade out finish\n", switchIdx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̃X�C�b�`�̏�Ԃ�BGM�̉��ʂɔ��f����
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void SetBGMVolume( const ISS_SWITCH_SYS* sys )
{
  int switchIdx;
  const SWITCH_DATA* switchData;

  // �Q�ƃf�[�^
  switchData = GetCurrentSwitchData( sys );

  // �e�X�C�b�`�̏�Ԃ𔽉f����
  for( switchIdx=0; switchIdx < SWITCH_NUM; switchIdx++ )
  {
    switch( sys->switchState[ switchIdx ] )
    {
    case SWITCH_STATE_ON:
        PMSND_ChangeBGMVolume( switchData->trackBit[ switchIdx ], MAX_VOLUME );
        break;
    case SWITCH_STATE_OFF:
        PMSND_ChangeBGMVolume( switchData->trackBit[ switchIdx ], 0 );
        break;
    case SWITCH_STATE_FADE_IN:
      { // �t�F�[�h�C����
        float now = (float)sys->fadeCount[ switchIdx ];
        float end = (float)switchData->fadeFrame;
        int   vol = MAX_VOLUME * (now / end);
        PMSND_ChangeBGMVolume( switchData->trackBit[ switchIdx ], vol );
      }
      break;
    case SWITCH_STATE_FADE_OUT:
      { // �t�F�[�h�A�E�g��
        float now = (float)sys->fadeCount[ switchIdx ];
        float end = (float)switchData->fadeFrame;
        int   vol = MAX_VOLUME * ( 1.0f - (now / end) );
        PMSND_ChangeBGMVolume( switchData->trackBit[ switchIdx ], vol );
      }
      break;
    }
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�f�[�^���o�͂���
 *
 * @param sys
 */
//-------------------------------------------------------------------------------
static void DebugPrint_SwitchData( const ISS_SWITCH_SYS* sys )
{
  int dataIdx;
  int bitIdx;
  int switchIdx;
  int validZoneIdx;

  for( dataIdx=0; dataIdx < sys->switchDataNum; dataIdx++ )
  {
    SWITCH_DATA* switchData = &sys->switchData[ dataIdx ];

    // �V�[�P���X�ԍ�
    OS_TFPrintf( PRINT_DEST, 
                 "- switchData[%d].soundIdx = %d\n", dataIdx, switchData->soundIdx );

    // �e�X�C�b�`�̃r�b�g�}�X�N
    for( switchIdx=0; switchIdx < SWITCH_NUM; switchIdx++ )
    {
      OS_TFPrintf( PRINT_DEST, 
                   "- switchData[%d].trackBit[%d] = ", dataIdx, switchIdx );

      for( bitIdx=0; bitIdx < TRACK_NUM; bitIdx++ )
      {
        if( switchData->trackBit[switchIdx] & (1<<(TRACK_NUM-1-bitIdx)) )
        {
          OS_TFPrintf( PRINT_DEST, "��" ); 
        }
        else
        { 
          OS_TFPrintf( PRINT_DEST, "��" ); 
        }
      }
      OS_TFPrintf( PRINT_DEST, "\n" );
    }

    // �t�F�[�h �t���[����
    OS_TFPrintf( PRINT_DEST, 
                "- switchData[%d].fadeFrame = %d\n", dataIdx, switchData->fadeFrame );

    // �X�C�b�`���L���ȏꏊ
    OS_TFPrintf( PRINT_DEST, 
                 "- switchData[%d].validZoneNum = %d\n", dataIdx, switchData->validZoneNum ); 
    for( validZoneIdx=0; validZoneIdx<switchData->validZoneNum; validZoneIdx++ )
    {
      OS_TFPrintf( PRINT_DEST, 
                   "- switchData[%d].validZone[%d] = %d\n", 
                   dataIdx, validZoneIdx, switchData->validZone[ validZoneIdx ] ); 
    }
  }
}
