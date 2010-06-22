/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief ISS�X�C�b�`�V�X�e��
 * @file iss_switch_sys.c
 * @author obata
 * @date 2009.11.17
 */
/////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_switch_set.h"
#include "iss_switch_sys.h"

#include "sound/pm_sndsys.h" // for PMSND_xxxx
#include "arc/arc_def.h"     // for ARCID_ISS_SWITCH


//===============================================================================
// ���萔
//===============================================================================
//#define DEBUG_PRINT_ON           // �f�o�b�O�o�̓X�C�b�`
#define PRINT_DEST           (1) // �f�o�b�O���̏o�͐�
#define TRACK_NUM           (16) // �g���b�N��
#define TRACKBIT_ALL    (0xffff) // �S�g���b�N�w��


//===============================================================================
// ��ISS-S�V�X�e��
//===============================================================================
struct _ISS_SWITCH_SYS
{ 
  BOOL             bootFlag;     // �N�����Ă��邩�ǂ���
  u8               switchSetNum; // �X�C�b�`�Z�b�g�̐�
  ISS_SWITCH_SET** switchSet;    // �X�C�b�`�Z�b�g
  ISS_SWITCH_SET*  curSwitchSet; // ���쒆�̃X�C�b�`�Z�b�g
};


//===============================================================================
// ���֐��C���f�b�N�X
//===============================================================================
static void InitializeSystem( ISS_SWITCH_SYS* system ); // �V�X�e��������������
static ISS_SWITCH_SYS* CreateSystem( HEAPID heapID ); // �V�X�e���𐶐�����
static void DeleteSystem( ISS_SWITCH_SYS* system ); // �V�X�e����j������
static void CreateSwitchSet( ISS_SWITCH_SYS* system, HEAPID heapID ); // �X�C�b�`�Z�b�g�𐶐�����
static void DeleteSwitchSet( ISS_SWITCH_SYS* system ); // �X�C�b�`�Z�b�g��j������
static u8 GetSwitchSetNum( const ISS_SWITCH_SYS* system ); // �X�C�b�`�̐����擾����
static ISS_SWITCH_SET* GetSwitchSet( const ISS_SWITCH_SYS* system, u8 setIdx ); // �X�C�b�`�Z�b�g���擾����
static ISS_SWITCH_SET* GetCurrentSwitchSet( const ISS_SWITCH_SYS* system ); // ���쒆�̃X�C�b�`�Z�b�g���擾����
static BOOL CheckCurrentSwitchSetExist( const ISS_SWITCH_SYS* system ); // ���쒆�̃X�C�b�`�Z�b�g�����邩�ǂ����𔻒肷��
static BOOL CheckCurrentSwitchSetValid( const ISS_SWITCH_SYS* system, u16 zoneID ); // ���݂̃X�C�b�`�Z�b�g���w�肵���]�[���ɑΉ����Ă��邩�ǂ������`�F�b�N����
static BOOL CheckSwitchSetExist( const ISS_SWITCH_SYS* system, u32 soundIdx ); // �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g�����邩�ǂ����𔻒肷��
static ISS_SWITCH_SET* SearchSwitchSet( const ISS_SWITCH_SYS* system, u32 soundIdx ); // �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g����������
static BOOL ChangeSwitchSet( ISS_SWITCH_SYS* system, u32 soundIdx ); // �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g�ɕύX����
static void SetBGMVolume( const ISS_SWITCH_SYS* system ); // ���݂̃X�C�b�`�Z�b�g��BGM�{�����[���ɔ��f������
static u32 GetCurrentBGM( void ); // ���݂�BGM���擾����
static BOOL CheckSystemBoot( const ISS_SWITCH_SYS* system ); // �V�X�e�����N�����Ă��邩�ǂ������`�F�b�N����
static void SystemMain( ISS_SWITCH_SYS* system ); // �V�X�e�����C������
static void BootSystem( ISS_SWITCH_SYS* system ); // �V�X�e�����N������
static void StopSystem( ISS_SWITCH_SYS* system ); // �V�X�e�����I������
static void ZoneChange( ISS_SWITCH_SYS* system, u16 nextZoneID ); // �]�[���ύX����
static void ResetCurrentSwitchSet( ISS_SWITCH_SYS* system ); // ���݂̃X�C�b�`�Z�b�g�����Z�b�g����


//===============================================================================
// ��public method
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
  ISS_SWITCH_SYS* system;

  system = CreateSystem( heapID ); // �V�X�e���𐶐�
  InitializeSystem( system );      // �V�X�e����������
  CreateSwitchSet( system, heapID ); // �X�C�b�`�Z�b�g�𐶐�

  return system;
}

//-------------------------------------------------------------------------------
/**
 * @brief ISS�X�C�b�`�V�X�e����j������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Delete( ISS_SWITCH_SYS* system )
{ 
  DeleteSwitchSet( system );
  DeleteSystem( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���E���C������
 *
 * @param system �������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Update( ISS_SWITCH_SYS* system )
{
  // ��~��
  if( CheckSystemBoot(system) == FALSE ) { return; }
  
  SystemMain( system );
  SetBGMVolume( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system �N������V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_On( ISS_SWITCH_SYS* system )
{
  BootSystem( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param system ��~����V�X�e��
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_Off( ISS_SWITCH_SYS* system )
{
  StopSystem( system );
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���̕ύX��ʒm����
 * 
 * @param system
 * @param zoneID �ύX��̃]�[��ID
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_ZoneChange( ISS_SWITCH_SYS* system, u16 zoneID )
{
  ZoneChange( system, zoneID );
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� ON �ɂ���
 *
 * @param system �X�C�b�`�������V�X�e��
 * @param idx    �����{�^���C���f�b�N�X
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOn( ISS_SWITCH_SYS* system, SWITCH_INDEX idx )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  if( set ) { 
    ISS_SWITCH_SET_SwitchOn( set, idx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� OFF �ɂ���
 *
 * @param system �X�C�b�`�������V�X�e��
 * @param idx �����X�C�b�`�̃C���f�b�N�X
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_SwitchOff( ISS_SWITCH_SYS* system, SWITCH_INDEX idx )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  if( set ) { 
    ISS_SWITCH_SET_SwitchOff( set, idx );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�� ON/OFF ���擾����
 *
 * @param system �擾�ΏۃV�X�e��
 * @param idx    ���肷��X�C�b�`�ԍ�
 *
 * @return �X�C�b�`idx��������Ă���ꍇ TRUE, �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
BOOL ISS_SWITCH_SYS_IsSwitchOn( const ISS_SWITCH_SYS* system, SWITCH_INDEX idx )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  if( set ) { 
    return ISS_SWITCH_SET_CheckSwitchOn( set, idx );
  }
  else {
    return FALSE;
  }
} 


//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�̏�Ԃ����Z�b�g����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
void ISS_SWITCH_SYS_ResetSwitch( ISS_SWITCH_SYS* system )
{ 
  ResetCurrentSwitchSet( system );
}


//===============================================================================
// ��private method
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e��������������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void InitializeSystem( ISS_SWITCH_SYS* system )
{
  system->bootFlag      = FALSE;
  system->switchSetNum  = 0;
  system->switchSet     = NULL;
  system->curSwitchSet  = NULL;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: init system\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e���𐶐�����
 *
 * @param heapID
 *
 * @return ���������V�X�e��
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH_SYS* CreateSystem( HEAPID heapID )
{
  ISS_SWITCH_SYS* system;

  system = GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SWITCH_SYS) );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: create system\n" );
#endif

  return system;
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e����j������*
 * @param system
 */
//-------------------------------------------------------------------------------
static void DeleteSystem( ISS_SWITCH_SYS* system )
{
  GF_ASSERT( system );

  GFL_HEAP_FreeMemory( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: delete system\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�𐶐�����
 *
 * @param system
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void CreateSwitchSet( ISS_SWITCH_SYS* system, HEAPID heapID )
{
  int datID;
  int num;
  ISS_SWITCH_SET* set;

  GF_ASSERT( system->switchSet == NULL );

  // �f�[�^�̐����擾
  num = GFL_ARC_GetDataFileCnt( ARCID_ISS_SWITCH );
  system->switchSetNum = num;

  // ���[�N���m��
  system->switchSet = 
    GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SWITCH_SET*)*num );

  // �X�C�b�`�Z�b�g�𐶐�
  for( datID=0; datID < num; datID++ )
  {
    set = ISS_SWITCH_SET_Create( heapID );
    ISS_SWITCH_SET_Load( set, datID, heapID );
    system->switchSet[ datID ] = set;
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: create switch-set\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g��j������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void DeleteSwitchSet( ISS_SWITCH_SYS* system )
{
  int i;
  int num;

  GF_ASSERT( system );
  GF_ASSERT( system->switchSet );

  num = system->switchSetNum;

  // �S�X�C�b�`��j��
  for( i=0; i<num; i++ )
  {
    ISS_SWITCH_SET_Delete( system->switchSet[i] );
  } 
  // ���[�N��j��
  GFL_HEAP_FreeMemory( system->switchSet );

  // �N���A
  system->switchSetNum  = 0;
  system->switchSet     = NULL;
  system->curSwitchSet  = NULL;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: delete switch-set\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����C������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void SystemMain( ISS_SWITCH_SYS* system )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  if( set ) {
    ISS_SWITCH_SET_Main( set );
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void BootSystem( ISS_SWITCH_SYS* system )
{
  // �N���ς�
  if( CheckSystemBoot(system) == TRUE ) { return; }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "��ISS-S: boot system\n" );
#endif

  // �N������
  system->bootFlag = TRUE;

  // �X�C�b�`���ω�������, �X�C�b�`�����Z�b�g����
  if( ChangeSwitchSet( system, GetCurrentBGM() ) == TRUE )
  {
    ResetCurrentSwitchSet( system );
    SetBGMVolume( system );
  } 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "��ISS-S: boot system\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����I������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void StopSystem( ISS_SWITCH_SYS* system )
{
  // ��~�ς�
  if( CheckSystemBoot(system) == FALSE ) { return; }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "��ISS-S: down system\n" );
#endif

  // ��~����
  system->bootFlag = FALSE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "��ISS-S: down system\n" );
#endif
}

//-------------------------------------------------------------------------------
/**
 * @brief �]�[���ύX����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-------------------------------------------------------------------------------
static void ZoneChange( ISS_SWITCH_SYS* system, u16 nextZoneID )
{
  // ��~��
  if( CheckSystemBoot(system) == FALSE ) { return; }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: ��zone change\n" );
#endif

  // �X�C�b�`�ێ��]�[������͂��ꂽ
  if( CheckCurrentSwitchSetValid( system, nextZoneID ) == FALSE ) { 
    ResetCurrentSwitchSet( system );
    StopSystem( system );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-S: ��zone change\n" );
#endif
} 

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g�̐����擾����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static u8 GetSwitchSetNum( const ISS_SWITCH_SYS* system )
{
  return system->switchSetNum;
}

//-------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`�Z�b�g���擾����
 *
 * @param system 
 * @param setIdx �X�C�b�`�Z�b�g�̃C���f�b�N�X
 *
 * @return �w�肵���C���f�b�N�X�̃X�C�b�`�Z�b�g
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH_SET* GetSwitchSet( const ISS_SWITCH_SYS* system, u8 setIdx )
{ 
  GF_ASSERT( setIdx < system->switchSetNum );

  return system->switchSet[ setIdx ];
}

//-------------------------------------------------------------------------------
/**
 * @brief ���쒆�̃X�C�b�`�Z�b�g���擾����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH_SET* GetCurrentSwitchSet( const ISS_SWITCH_SYS* system )
{
  return system->curSwitchSet;
} 

//-------------------------------------------------------------------------------
/**
 * @brief ���쒆�̃X�C�b�`�Z�b�g�����邩�ǂ����𔻒肷��
 *
 * @param system
 *
 * @return ���쒆�̃X�C�b�`������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckCurrentSwitchSetExist( const ISS_SWITCH_SYS* system )
{
  if( GetCurrentSwitchSet(system) ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̃X�C�b�`�Z�b�g��, 
 *        �w�肵���]�[���ɑΉ����Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 * @param zoneID ���肷��]�[��ID
 *
 * @return �w�肵���]�[����, ���݂̃X�C�b�`�Z�b�g�ɑΉ����Ă���ꍇ, TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckCurrentSwitchSetValid( const ISS_SWITCH_SYS* system, u16 zoneID )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  // �X�C�b�`���Ȃ�
  if( set == FALSE ) { return FALSE; }

  return ISS_SWITCH_SET_IsValidAt( set, zoneID );
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g�����邩�ǂ����𔻒肷��
 *
 * @param system
 * @param soundIdx �����L�[�ƂȂ�BGM
 *
 * @return �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckSwitchSetExist( const ISS_SWITCH_SYS* system, u32 soundIdx )
{
  if( SearchSwitchSet( system, soundIdx ) == NULL ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g����������
 *
 * @param system
 * @param soundIdx �����L�[�ƂȂ�BGM
 *
 * @return �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g������ꍇ, 
 *         ���̃X�C�b�`�Z�b�g��Ԃ�.
 *         �����łȂ����, NULL
 */
//-------------------------------------------------------------------------------
static ISS_SWITCH_SET* SearchSwitchSet( const ISS_SWITCH_SYS* system, u32 soundIdx )
{
  int idx;
  int setNum;
  ISS_SWITCH_SET* set;

  setNum = GetSwitchSetNum( system );

  // �S�ẴX�C�b�`�Z�b�g���`�F�b�N
  for( idx=0; idx < setNum; idx++ )
  {
    set = GetSwitchSet( system, idx );

    // �w�肳�ꂽBGM�ɑΉ�����X�C�b�`�𔭌�
    if( ISS_SWITCH_SET_GetSoundIdx(set) == soundIdx ) { return set; }
  } 

  // �Y������X�C�b�`�Z�b�g�͎����Ă��Ȃ�
  return NULL;
}

//-------------------------------------------------------------------------------
/**
 * @brief �w�肵��BGM�ɑΉ�����X�C�b�`�Z�b�g�ɕύX����
 *
 * @param system
 * @param soundIdx BGM���w��
 *
 * @return �X�C�b�`���ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL ChangeSwitchSet( ISS_SWITCH_SYS* system, u32 soundIdx )
{
  ISS_SWITCH_SET* oldSet;
  ISS_SWITCH_SET* newSet;

  oldSet = GetCurrentSwitchSet( system );
  newSet = SearchSwitchSet( system, soundIdx );

  // �X�C�b�`�Z�b�g�ɕω��Ȃ�
  if( oldSet == newSet ) { return FALSE; }

  // �X�C�b�`�Z�b�g��ύX
  system->curSwitchSet = newSet;
  return TRUE;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̃X�C�b�`�Z�b�g��BGM�{�����[���ɔ��f������
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void SetBGMVolume( const ISS_SWITCH_SYS* system )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );
  
  if( set ) {
    ISS_SWITCH_SET_SetBGMVolume( set );
  } 
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂�BGM���擾����
 *
 * @return �Đ�����BGM�ԍ�
 */
//-------------------------------------------------------------------------------
static u32 GetCurrentBGM( void )
{
  return PMSND_GetBGMsoundNo();
}

//-------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N�����Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 *
 * @return �V�X�e�����N�����Ȃ� TRUE
 *         �����łȂ��Ȃ� FALSE
 */
//-------------------------------------------------------------------------------
static BOOL CheckSystemBoot( const ISS_SWITCH_SYS* system )
{
  return system->bootFlag;
}

//-------------------------------------------------------------------------------
/**
 * @brief ���݂̃X�C�b�`�Z�b�g�����Z�b�g����
 *
 * @param system
 */
//-------------------------------------------------------------------------------
static void ResetCurrentSwitchSet( ISS_SWITCH_SYS* system )
{
  ISS_SWITCH_SET* set;

  set = GetCurrentSwitchSet( system );

  if( set ) {
    ISS_SWITCH_SET_ResetSwitch( set );
  }
}
