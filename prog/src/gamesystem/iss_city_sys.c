///////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_sys.c
 * @brief  �XISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
///////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_city_unit.h"
#include "iss_city_sys.h"
#include "arc/arc_def.h"
#include "sound/pm_sndsys.h"
#include "gamesystem/playerwork.h" 
#include "field/field_const.h"
#include "../field/field_sound.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//=================================================================================
// ���萔�E�}�N��
//=================================================================================
//#define DEBUG_PRINT_ON // �f�o�b�O�o�̓X�C�b�`
#define PRINT_DEST (1) // �f�o�b�O���̏o�͐�

#define UNIT_IDX_NULL (0xff) // ���j�b�g�ԍ��̖����l

#define MIN_TRACK_VOLUME    (0) // �g���b�N�{�����[���̍ŏ��l
#define MAX_TRACK_VOLUME  (127) // �g���b�N�{�����[���̍ő�l
#define MIN_SYSTEM_VOLUME   (0) // �V�X�e���{�����[���̍ŏ��l
#define MAX_SYSTEM_VOLUME (127) // �V�X�e���{�����[���̍ő�l
#define SYSTEM_VOLUME_FADEIN_SPEED  (2) // �V�X�e���{�����[���̕ύX���x ( �t�F�[�h�C�� )
#define SYSTEM_VOLUME_FADEOUT_SPEED (2) // �V�X�e���{�����[���̕ύX���x ( �t�F�[�h�A�E�g )

// ����g���b�N
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // 9,10�g���b�N

// �V�X�e�����
typedef enum {
  SYSTEM_STATE_WAIT,    // �ҋ@
  SYSTEM_STATE_FADEIN,  // �t�F�[�h�C��
  SYSTEM_STATE_FADEOUT, // �t�F�[�h�A�E�g
} SYSTEM_STATE;


//=================================================================================
// ���XISS�V�X�e���\����
//=================================================================================
struct _ISS_CITY_SYS
{
	PLAYER_WORK* player; // �Ď��Ώۃv���C���[

	// �V�X�e��
  BOOL         bootFlag;     // �N�����Ă��邩�ǂ���
  SYSTEM_STATE state;        // ���
  int          trackVolume;  // ����g���b�N�̃{�����[��
  int          systemVolume; // ��ʃ{�����[��

	// ���j�b�g
	ISS_C_UNIT** unit;		 // ���j�b�g�z��
	u8           unitNum;	 // ���j�b�g��
	u8           unitIdx;	 // ���쒆�̃��j�b�g�ԍ�
};


//=================================================================================
// ���֐��C���f�b�N�X
//=================================================================================
//---------------------------------------------------------------------------------
// �������E�j��
//---------------------------------------------------------------------------------
// layer 2
static void InitializeSystem( ISS_CITY_SYS* system, HEAPID heapID, PLAYER_WORK* player ); // �V�X�e��������������
static void SetupSystem( ISS_CITY_SYS* system, HEAPID heapID ); // �V�X�e�����Z�b�g�A�b�v����
static void CleanUpSystem( ISS_CITY_SYS* system ); // �V�X�e�����N���[���A�b�v����
// layer 1
static void LoadUnitData( ISS_CITY_SYS* system, HEAPID heapID ); // ���j�b�g�f�[�^��ǂݍ���
static void UnloadUnitData( ISS_CITY_SYS* system ); // ���j�b�g�f�[�^��j������
//---------------------------------------------------------------------------------
// ���{�����[������
//---------------------------------------------------------------------------------
// ��ʃ{�����[��
static int GetSystemVolume( const ISS_CITY_SYS* system ); // ��ʃ{�����[�����擾����
static void SetSystemVolume( ISS_CITY_SYS* system, int volume ); // ��ʃ{�����[����ݒ肷��
static BOOL SystemVolumeUp( ISS_CITY_SYS* system ); // ��ʃ{�����[�����グ��
static BOOL SystemVolumeDown( ISS_CITY_SYS* system ); // ��ʃ{�����[����������
// �g���b�N�{�����[��
static int GetTrackVolume( const ISS_CITY_SYS* system ); // �g���b�N�{�����[�����擾����
static void SetTrackVolume( ISS_CITY_SYS* system, int volume ); // �g���b�N�{�����[����ݒ肷��
static BOOL UpdateTrackVolume( ISS_CITY_SYS* system ); // �g���b�N�{�����[�����X�V����
static int CalcTrackVolume( ISS_CITY_SYS* system ); // �g���b�N�{�����[�����v�Z����
static int CalcEffectiveTrackVolume( const ISS_CITY_SYS* system ); // BGM�̑���g���b�N�ɔ��f������{�����[�����Z�o����
static void ChangeBGMTrackVolume( const ISS_CITY_SYS* system ); // BGM�̃g���b�N�{�����[����ύX����
//---------------------------------------------------------------------------------
// ���V�X�e������
//---------------------------------------------------------------------------------
static void SystemMain( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐�
static void SystemMain_WAIT( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_WAIT )
static void SystemMain_FADEIN( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_FADEIN )
static void SystemMain_FADEOUT( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_FADEOUT )
//---------------------------------------------------------------------------------
// ���V�X�e������
//---------------------------------------------------------------------------------
static SYSTEM_STATE GetSystemState( const ISS_CITY_SYS* system ); // �V�X�e����Ԃ��擾����
static void SetSystemState( ISS_CITY_SYS* system, SYSTEM_STATE state ); // �V�X�e����Ԃ�ύX����
static void BootSystem( ISS_CITY_SYS* system ); // �V�X�e�����N������
static void StopSystem( ISS_CITY_SYS* system ); // �V�X�e�����~����
static void ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID ); // �]�[���̐؂�ւ��������s��
static u16 GetCurrentZoneID( const ISS_CITY_SYS* system ); // ���݂̃]�[��ID���擾����
static BOOL CheckSystemBoot( const ISS_CITY_SYS* system ); // �V�X�e�����N�����Ă��邩�ǂ������`�F�b�N����
//---------------------------------------------------------------------------------
// �����j�b�g
//---------------------------------------------------------------------------------
static ISS_C_UNIT* GetCurrentUnit( const ISS_CITY_SYS* system ); // ���݂̃��j�b�g���擾����
static u8 GetCurrentUnitIndex( const ISS_CITY_SYS* system ); // ���݂̃��j�b�g�C���f�b�N�X���擾����
static BOOL CheckUnitExistNow( const ISS_CITY_SYS* system ); // ����, ���j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
static BOOL CheckUnitExistAt( const ISS_CITY_SYS* system, u16 zoneID ); // �w�肵���ꏊ�Ƀ��j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
static BOOL ChangeUnit( ISS_CITY_SYS* system, u16 zoneID ); // ���j�b�g��ύX����
static u8 SearchUnit( const ISS_CITY_SYS* system, u16 zoneID ); // ���j�b�g����������
static BOOL CheckUnitChange( const ISS_CITY_SYS* system, u16 zoneID ); // ���j�b�g���ω����邩�ǂ������`�F�b�N����


//=================================================================================
// �����J�֐��̎���
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e�����쐬����
 *
 * @param  player  �Ď��Ώۂ̃v���C���[
 * @param  heapID  �g�p����q�[�vID
 * 
 * @return �XISS�V�X�e��
 */
//---------------------------------------------------------------------------------
ISS_CITY_SYS* ISS_CITY_SYS_Create( PLAYER_WORK* player, HEAPID heapID )
{
	ISS_CITY_SYS* system;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: create\n" );
#endif

	// �V�X�e���𐶐�
	system = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof(ISS_CITY_SYS) );

	// �V�X�e����������
  InitializeSystem( system, heapID, player );
  SetupSystem( system, heapID ); 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: create\n" );
#endif
	
	return system;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �XISS�V�X�e����j������
 *
 * @param system
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Delete( ISS_CITY_SYS* system )
{
#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: delete\n" );
#endif

  // �V�X�e�����N���[���A�b�v
  CleanUpSystem( system );

	// �{�̂̔j��
	GFL_HEAP_FreeMemory( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: delete\n" );
#endif
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param system
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Main( ISS_CITY_SYS* system )
{
  // �N�����Ă��Ȃ�
  if( CheckSystemBoot(system) == FALSE ) { return; }

  SystemMain( system );
}
	
//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_On( ISS_CITY_SYS* system )
{ 
  BootSystem( system );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param system
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Off( ISS_CITY_SYS* system )
{
  StopSystem( system );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param system
 * @param nextZoneID �V�����]�[��ID
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID )
{ 
  ZoneChange( system, nextZoneID );
}


//=================================================================================
// ������J�֐��̎���
//=================================================================================

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e��������������
 *
 * @param system
 * @param heapID �V�X�e�����g�p����q�[�vID
 * @param player �Ď��Ώۂ̃v���C���[ ( ���@ )
 */
//---------------------------------------------------------------------------------
static void InitializeSystem( ISS_CITY_SYS* system, HEAPID heapID, PLAYER_WORK* player )
{
  GF_ASSERT( system );
  GF_ASSERT( player );

	system->player        = player;
  system->bootFlag      = FALSE;
  system->state         = SYSTEM_STATE_WAIT;
  system->trackVolume   = MIN_TRACK_VOLUME;
  system->systemVolume  = MIN_SYSTEM_VOLUME;
	system->unit          = NULL;
	system->unitNum       = 0;
  system->unitIdx       = UNIT_IDX_NULL;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����Z�b�g�A�b�v���� 
 *
 * @param system
 * @param heapID
 */
//---------------------------------------------------------------------------------
static void SetupSystem( ISS_CITY_SYS* system, HEAPID heapID )
{
  GF_ASSERT( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: setup system\n" );
#endif 

	LoadUnitData( system, heapID ); // ���j�b�g����ǂݍ���

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: setup system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N���[���A�b�v����
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void CleanUpSystem( ISS_CITY_SYS* system )
{
  GF_ASSERT( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: clean up system\n" );
#endif

  UnloadUnitData( system ); // ���j�b�g�f�[�^��j������

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: clean up system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��ǂݍ���
 *
 * @param system
 * @param heapID
 */
//---------------------------------------------------------------------------------
static void LoadUnitData( ISS_CITY_SYS* system, HEAPID heapID )
{
  int dataNum;
  int datID;

  GF_ASSERT( system->unit == NULL );

  // ���j�b�g�����擾
  dataNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_CITY );
  system->unitNum = dataNum;

  // �e���j�b�g�f�[�^�̃��[�N���m��
  system->unit = GFL_HEAP_AllocMemory( heapID, dataNum * sizeof(ISS_C_UNIT*) );

  // �e���j�b�g�f�[�^��ǂݍ���
  for( datID=0; datID < dataNum; datID++ )
  {
    system->unit[ datID ] = ISS_C_UNIT_Create( heapID, datID );
  } 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: load unit data\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��j������
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void UnloadUnitData( ISS_CITY_SYS* system )
{
  int unitIdx;
  int unitNum;

  GF_ASSERT( system );
  GF_ASSERT( system->unit );

  unitNum = system->unitNum;

	// �e���j�b�g��j��
  for( unitIdx=0; unitIdx < unitNum; unitIdx++ )
  {
    ISS_C_UNIT_Delete( system->unit[ unitIdx ] );
  }
	GFL_HEAP_FreeMemory( system->unit );
  system->unit = NULL;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: unload unit data\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ��擾����
 *
 * @param system
 *
 * @return ���݂̃V�X�e���̏��
 */
//---------------------------------------------------------------------------------
static SYSTEM_STATE GetSystemState( const ISS_CITY_SYS* system )
{
  GF_ASSERT( system );
  return system->state;
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ�ύX����
 *
 * @param system
 * @param state  �ݒ肷����
 */
//---------------------------------------------------------------------------------
static void SetSystemState( ISS_CITY_SYS* system, SYSTEM_STATE state )
{
  GF_ASSERT( system );

  // ��Ԃ��X�V
  system->state = state;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: set system state ==> " );
  switch( state ) {
  case SYSTEM_STATE_WAIT:    OS_TFPrintf( PRINT_DEST, "WAIT" );    break;
  case SYSTEM_STATE_FADEIN:  OS_TFPrintf( PRINT_DEST, "FADEIN" );  break;
  case SYSTEM_STATE_FADEOUT: OS_TFPrintf( PRINT_DEST, "FADEOUT" ); break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_DEST, "\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void BootSystem( ISS_CITY_SYS* system )
{
  // �N���ς�
  if( CheckSystemBoot(system) ) { return; }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: boot system\n" );
#endif

  system->bootFlag = TRUE; // �N��
  SetTrackVolume( system, MIN_TRACK_VOLUME ); // �g���b�N�{�����[�����ŏ��ɂ���
  ChangeBGMTrackVolume( system ); // BGM�ɔ��f������

  // ���j�b�g��ύX
  ChangeUnit( system, GetCurrentZoneID(system) ); 

  // ���j�b�g������
  if( CheckUnitExistNow(system) == TRUE ) {
    SetSystemState( system, SYSTEM_STATE_FADEIN );
  }
  // ���j�b�g���Ȃ�
  else {
    SetSystemState( system, SYSTEM_STATE_WAIT );
  } 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: boot system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void StopSystem( ISS_CITY_SYS* system )
{
  // ��~��
  if( CheckSystemBoot(system) == FALSE ) { return; }

  // ��~������
  system->bootFlag = FALSE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: stop system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ��������s��
 *
 * @param system
 * @param nextZoneID �؂�ւ���̃]�[��ID
 */
//---------------------------------------------------------------------------------
static void ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID )
{
  // ��~��
  if( CheckSystemBoot(system) == FALSE ) { return; }

  // ����, ���j�b�g�����݂���
  if( CheckUnitExistNow( system ) == TRUE ) {
    // �ړ���ɂ����j�b�g�����݂���
    if( CheckUnitExistAt( system, nextZoneID ) == TRUE ) {
      // �Ȃ̕ύX�ɑ΂��郊�A�N�V������, �V�X�e����ON/OFF�ōs��. 
      StopSystem( system );
      return;
    }
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: zone change\n" );
#endif

  // ���j�b�g��ύX
  ChangeUnit( system, nextZoneID );

  // ���j�b�g�����݂���
  if( CheckUnitExistNow( system ) == TRUE ) {
    SetSystemVolume( system, MIN_SYSTEM_VOLUME );
    SetSystemState( system, SYSTEM_STATE_FADEIN );
  }
  // ���j�b�g�����݂��Ȃ�
  else {
    SetSystemState( system, SYSTEM_STATE_FADEOUT );
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "--- ISS-C: zone change\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID���擾����
 *
 * @param system
 *
 * @return ���݂̃]�[��ID
 */
//---------------------------------------------------------------------------------
static u16 GetCurrentZoneID( const ISS_CITY_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( system->player );

  return PLAYERWORK_getZoneID( system->player );
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N�����Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 *
 * @return �V�X�e�����N�����Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckSystemBoot( const ISS_CITY_SYS* system )
{
  return system->bootFlag;
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain( ISS_CITY_SYS* system )
{
  GF_ASSERT( system );

  switch( GetSystemState(system) ) {
  case SYSTEM_STATE_WAIT:    SystemMain_WAIT( system );    break;
  case SYSTEM_STATE_FADEIN:  SystemMain_FADEIN( system );  break;
  case SYSTEM_STATE_FADEOUT: SystemMain_FADEOUT( system ); break;
  default: GF_ASSERT(0);
  } 
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_WAIT )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_WAIT( ISS_CITY_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_WAIT ); 

  // �g���b�N�{�����[�����X�V
  if( UpdateTrackVolume( system ) ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_FADEIN )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_FADEIN( ISS_CITY_SYS* system )
{
  BOOL sysVolChanged;
  BOOL trkVolChanged;

  GF_ASSERT( system );
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_FADEIN );

  // ��ʃ{�����[���E�g���b�N�{�����[�����X�V
  sysVolChanged = SystemVolumeUp( system );
  trkVolChanged = UpdateTrackVolume( system );
  
  // �{�����[�����ω�
  if( sysVolChanged || trkVolChanged ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }

  // �t�F�[�h�C������
  if( MAX_SYSTEM_VOLUME <= GetSystemVolume(system) ) {
    SetSystemState( system, SYSTEM_STATE_WAIT );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_FADEOUT )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_FADEOUT( ISS_CITY_SYS* system )
{
  BOOL sysVolChanged;
  BOOL trkVolChanged;

  GF_ASSERT( system );
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_FADEOUT );

  // ��ʃ{�����[���E�g���b�N�{�����[�����X�V
  sysVolChanged = SystemVolumeDown( system );
  trkVolChanged = UpdateTrackVolume( system );
  
  // �{�����[�����ω�
  if( sysVolChanged || trkVolChanged ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }

  // �t�F�[�h�A�E�g����
  if( GetSystemVolume(system) <= MIN_SYSTEM_VOLUME ) {
    SetSystemState( system, SYSTEM_STATE_WAIT );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ���݂̃��j�b�g���擾����
 *
 * @param system
 *
 * @return ���݂̃��j�b�g
 *         ���j�b�g�����݂��Ȃ��ꍇ NULL
 */
//---------------------------------------------------------------------------------
static ISS_C_UNIT* GetCurrentUnit( const ISS_CITY_SYS* system )
{
  return system->unit[ GetCurrentUnitIndex(system) ];
}

//---------------------------------------------------------------------------------
/**
 * @brief ���݂̃��j�b�g�C���f�b�N�X���擾����
 *
 * @param system 
 *
 * @return ���݂̃��j�b�g�̃C���f�b�N�X
 *         ���j�b�g�����݂��Ȃ��ꍇ, UNIT_IDX_NULL
 */
//---------------------------------------------------------------------------------
static u8 GetCurrentUnitIndex( const ISS_CITY_SYS* system )
{
  return system->unitIdx;
}

//---------------------------------------------------------------------------------
/**
 * @brief ����, ���j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 * 
 * @return ���j�b�g�����݂��Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckUnitExistNow( const ISS_CITY_SYS* system )
{
  if( GetCurrentUnitIndex(system) == UNIT_IDX_NULL ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �w�肵���ꏊ�Ƀ��j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 * @param zoneID �`�F�b�N����ꏊ���w��
 * 
 * @return ���j�b�g�����݂��Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckUnitExistAt( const ISS_CITY_SYS* system, u16 zoneID )
{
  if( SearchUnit( system, zoneID ) == UNIT_IDX_NULL ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���ɔz�u���ꂽ���j�b�g�ɐ؂�ւ���
 *
 * @param system
 * @param zoneID �؂�ւ���̃]�[��ID
 *
 * @return ���j�b�g���ω������ꍇ, TRUE
 *         �����łȂ���΁@FALSE
 */
//---------------------------------------------------------------------------------
static BOOL ChangeUnit( ISS_CITY_SYS* system, u16 zoneID )
{
  int oldUnitIdx;
  int newUnitIdx;

  GF_ASSERT( system );

  oldUnitIdx = GetCurrentUnitIndex(system);
  newUnitIdx = SearchUnit( system, zoneID );

  // �ω��Ȃ�
  if( oldUnitIdx == newUnitIdx ) { return FALSE; }

  // ���j�b�g�C���f�b�N�X��ύX
  system->unitIdx = newUnitIdx;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: change unit ==> %d\n", newUnitIdx );
#endif

  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g����������
 *
 * @param system
 * @param zoneID �����Ώۂ̃]�[��
 *
 * @return �w�肵���]�[���ɑΉ����郆�j�b�g�̃C���f�b�N�X
 *         ���j�b�g�����݂��Ȃ��ꍇ UNIT_IDX_NULL
 */
//---------------------------------------------------------------------------------
static u8 SearchUnit( const ISS_CITY_SYS* system, u16 zoneID )
{
	int unitIdx;
  int unitNum;

  GF_ASSERT( system );
  GF_ASSERT( system->unit );
  GF_ASSERT( 0 < system->unitNum );

  unitNum = system->unitNum;

  // �w�肳�ꂽ�]�[���ɊY�����郆�j�b�g����������
	for( unitIdx=0; unitIdx < unitNum; unitIdx++ )
	{
		// ����
		if( ISS_C_UNIT_GetZoneID( system->unit[ unitIdx ] ) == zoneID )
		{ 
      return unitIdx;
		}
	}

  // �z�u����Ă��Ȃ�
  return UNIT_IDX_NULL;
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g���ω����邩�ǂ������`�F�b�N����
 *
 * @param system
 * @param zoneID �`�F�b�N����]�[�����w��
 *
 * @return �w�肵���]�[���Ɉړ��������� ���j�b�g���ω�����ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckUnitChange( const ISS_CITY_SYS* system, u16 zoneID )
{
  int oldUnitIdx;
  int newUnitIdx;

  oldUnitIdx = GetCurrentUnitIndex(system);
  newUnitIdx = SearchUnit( system, zoneID );

  // �ω��Ȃ�
  if(oldUnitIdx == newUnitIdx ) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ��ʃ{�����[�����擾����
 *
 * @param system
 *
 * @return ��ʃ{�����[���l
 */
//---------------------------------------------------------------------------------
static int GetSystemVolume( const ISS_CITY_SYS* system )
{ 
  GF_ASSERT( system );

  return system->systemVolume;
}

//---------------------------------------------------------------------------------
/**
 * @brief ��ʃ{�����[�����グ��
 *
 * @param system
 *
 * @return ��ʃ{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL SystemVolumeUp( ISS_CITY_SYS* system )
{
  int nextVolume;

  GF_ASSERT( system );

  // �{�����[�����v�Z
  nextVolume = GetSystemVolume(system) + SYSTEM_VOLUME_FADEIN_SPEED;

  // �ő�l�␳
  if( MAX_SYSTEM_VOLUME <= nextVolume ) { nextVolume = MAX_SYSTEM_VOLUME; }

  // �{�����[���ω��Ȃ�
  if( nextVolume == GetSystemVolume(system) ) { return FALSE; }

  // ��ʃ{�����[�����X�V
  SetSystemVolume( system, nextVolume );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: system volume up ==> %d\n", nextVolume );
#endif

  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief ��ʃ{�����[����������
 *
 * @param system
 *
 * @return ��ʃ{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL SystemVolumeDown( ISS_CITY_SYS* system )
{
  int nextVolume;

  GF_ASSERT( system );

  // �{�����[�����v�Z
  nextVolume = GetSystemVolume(system) - SYSTEM_VOLUME_FADEOUT_SPEED;

  // �ŏ��l�␳
  if( nextVolume <= MIN_SYSTEM_VOLUME ) { nextVolume = MIN_SYSTEM_VOLUME; }

  // �{�����[���ω��Ȃ�
  if( nextVolume == GetSystemVolume(system) ) { return FALSE; }

  // ��ʃ{�����[�����X�V
  SetSystemVolume( system, nextVolume );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: system volume down ==> %d\n", nextVolume );
#endif

  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief ��ʃ{�����[����ݒ肷��
 *
 * @param system
 * @param volume �ݒ肷��{�����[��
 */
//---------------------------------------------------------------------------------
static void SetSystemVolume( ISS_CITY_SYS* system, int volume )
{
  GF_ASSERT( system );
  GF_ASSERT( MIN_SYSTEM_VOLUME <= volume );
  GF_ASSERT( volume <= MAX_SYSTEM_VOLUME );

  system->systemVolume = volume;
}

//---------------------------------------------------------------------------------
/**
 * @brief �g���b�N�{�����[�����擾����
 *
 * @param system
 * 
 * @return �g���b�N�{�����[���l
 */
//---------------------------------------------------------------------------------
static int GetTrackVolume( const ISS_CITY_SYS* system )
{
  GF_ASSERT( system );

  return system->trackVolume;
}

//---------------------------------------------------------------------------------
/**
 * @brief �g���b�N�{�����[�����X�V����
 *
 * @param system
 *
 * @return �{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL UpdateTrackVolume( ISS_CITY_SYS* system )
{
  int newVolume = 0;
  const VecFx32* playerPos;
  const ISS_C_UNIT* unit;

  GF_ASSERT( system );
  GF_ASSERT( CheckSystemBoot(system) );

  // ���j�b�g���z�u����Ă��Ȃ�
  if( CheckUnitExistNow(system) == FALSE ) { return FALSE; }

  // ���j�b�g�Ǝ��@�̈ʒu����{�����[�����Z�o
  newVolume = CalcTrackVolume( system );

  // �{�����[���ω��Ȃ�
  if( newVolume == GetTrackVolume(system) ) { return FALSE; }

  // �{�����[����ύX
  SetTrackVolume( system, newVolume ); 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: update track volume ==> %d\n", newVolume );
#endif
  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief �g���b�N�{�����[�����v�Z����
 *
 * @param system
 *
 * @return ���j�b�g�Ǝ��@�̈ʒu����Z�o�����g���b�N�{�����[��
 */
//---------------------------------------------------------------------------------
static int CalcTrackVolume( ISS_CITY_SYS* system )
{
  int volume;
  const VecFx32* playerPos;
  const ISS_C_UNIT* unit;

  GF_ASSERT( system );
  GF_ASSERT( system->player );
  GF_ASSERT( CheckUnitExistNow(system) == TRUE );

  // ���j�b�g�Ǝ��@�̈ʒu����{�����[�����Z�o
  unit      = GetCurrentUnit( system );
  playerPos = PLAYERWORK_getPosition( system->player );
  volume    = ISS_C_UNIT_GetVolume( unit, playerPos );

  return volume;
}

//---------------------------------------------------------------------------------
/**
 * @brief �g���b�N�{�����[����ݒ肷��
 *
 * @param system
 * @param volume �ݒ肷��{�����[��
 */
//---------------------------------------------------------------------------------
static void SetTrackVolume( ISS_CITY_SYS* system, int volume )
{
  GF_ASSERT( system );
  GF_ASSERT( MIN_TRACK_VOLUME <= volume );
  GF_ASSERT( volume <= MAX_TRACK_VOLUME );

  system->trackVolume = volume;
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̃{�����[����ύX����
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void ChangeBGMTrackVolume( const ISS_CITY_SYS* system )
{
  int volume;

  // �{�����[�������l���Z�o
  volume = CalcEffectiveTrackVolume( system );

  // BGM�̃g���b�N�{�����[���ɔ��f
  PMSND_ChangeBGMVolume( TRACKBIT, volume );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: change BGM track volume ==> %d\n", volume );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief BGM�̑���g���b�N�ɔ��f������{�����[�����Z�o����
 *
 * @param system
 *
 * @return BGM�ɔ��f������{�����[���l
 */
//---------------------------------------------------------------------------------
static int CalcEffectiveTrackVolume( const ISS_CITY_SYS* system )
{
  int trackVolume;
  int systemVolume;
  int volume;

  GF_ASSERT( system );

  // �{�����[�������l���Z�o
  trackVolume  = GetTrackVolume(system);
  systemVolume = GetSystemVolume(system);
  volume       = trackVolume * systemVolume / MAX_SYSTEM_VOLUME;

  // �{�����[���̒l��`�F�b�N
  GF_ASSERT( MIN_TRACK_VOLUME <= trackVolume );
  GF_ASSERT( MIN_SYSTEM_VOLUME <= systemVolume );
  GF_ASSERT( 0 <= volume );
  GF_ASSERT( trackVolume <= MAX_TRACK_VOLUME );
  GF_ASSERT( systemVolume <= MAX_SYSTEM_VOLUME );
  GF_ASSERT( volume <= 127 );

  return volume;
}
