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
#define DEBUG_PRINT_ON // �f�o�b�O�o�̓X�C�b�`
#define PRINT_DEST (1) // �f�o�b�O���̏o�͐�

#define UNIT_IDX_NULL (0xff) // ���j�b�g�ԍ��̖����l

#define MIN_TRACK_VOLUME    (0) // �g���b�N�{�����[���̍ŏ��l
#define MAX_TRACK_VOLUME  (127) // �g���b�N�{�����[���̍ő�l
#define MIN_SYSTEM_VOLUME   (0) // �V�X�e���{�����[���̍ŏ��l
#define MAX_SYSTEM_VOLUME (127) // �V�X�e���{�����[���̍ő�l
#define SYSTEM_VOLUME_FADEIN_SPEED  (1) // �V�X�e���{�����[���̕ύX���x ( �t�F�[�h�C�� )
#define SYSTEM_VOLUME_FADEOUT_SPEED (2) // �V�X�e���{�����[���̕ύX���x ( �t�F�[�h�A�E�g )

// ����g���b�N
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // 9,10�g���b�N

// �V�X�e�����
typedef enum {
  SYSTEM_STATE_STOP, // ��~��
  SYSTEM_STATE_BOOT, // �N����
  SYSTEM_STATE_RUN,  // ���쒆
  SYSTEM_STATE_DOWN, // �I����
} SYSTEM_STATE;


//=================================================================================
// ���XISS�V�X�e���\����
//=================================================================================
struct _ISS_CITY_SYS
{
	HEAPID       heapID; 
	PLAYER_WORK* player; // �Ď��Ώۃv���C���[

	// �V�X�e��
  SYSTEM_STATE state;         // ���
  int          trackVolume;   // ����g���b�N�̃{�����[��
  int          systemVolume;  // ��ʃ{�����[��

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
static void SetupSystem( ISS_CITY_SYS* system ); // �V�X�e�����Z�b�g�A�b�v����
static void CleanUpSystem( ISS_CITY_SYS* system ); // �V�X�e�����N���[���A�b�v����
// layer 1
static void LoadUnitData( ISS_CITY_SYS* system ); // ���j�b�g�f�[�^��ǂݍ���
static void UnloadUnitData( ISS_CITY_SYS* system ); // ���j�b�g�f�[�^��j������
//---------------------------------------------------------------------------------
// ���{�����[������
//---------------------------------------------------------------------------------
// ��ʃ{�����[��
static int GetSystemVolume( const ISS_CITY_SYS* system ); // ��ʃ{�����[�����擾����
static void SetSystemVolume( ISS_CITY_SYS* system, int volume ); // ��ʃ{�����[����ݒ肷��
static BOOL UpdateSystemVolume( ISS_CITY_SYS* system ); // ��ʃ{�����[�����X�V����
// �g���b�N�{�����[��
static int GetTrackVolume( const ISS_CITY_SYS* system ); // �g���b�N�{�����[�����擾����
static BOOL UpdateTrackVolume( ISS_CITY_SYS* system ); // �g���b�N�{�����[�����X�V����
static void SetTrackVolume( ISS_CITY_SYS* system, int volume ); // �g���b�N�{�����[����ݒ肷��
static void ChangeBGMTrackVolume( const ISS_CITY_SYS* system ); // BGM�̃g���b�N�{�����[����ύX����
static int CalcBGMTrackVolume( const ISS_CITY_SYS* system ); // BGM�̑���g���b�N�ɔ��f������{�����[�����Z�o����
//---------------------------------------------------------------------------------
// ���V�X�e������
//---------------------------------------------------------------------------------
static void SystemMain( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐�
static void SystemMain_forSTOP( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_STOP )
static void SystemMain_forBOOT( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_BOOT )
static void SystemMain_forRUN( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_RUN )
static void SystemMain_forDOWN( ISS_CITY_SYS* system ); // �V�X�e���̃��C������֐� ( SYSTEM_STATE_DOWN )
static void InitSystemState_forSTOP( ISS_CITY_SYS* system ); // �V�X�e����Ԃ����������� ( SYSTEM_STATE_STOP )
static void InitSystemState_forBOOT( ISS_CITY_SYS* system ); // �V�X�e����Ԃ����������� ( SYSTEM_STATE_BOOT )
static void InitSystemState_forRUN( ISS_CITY_SYS* system ); // �V�X�e����Ԃ����������� ( SYSTEM_STATE_RUN )
static void InitSystemState_forDOWN( ISS_CITY_SYS* system ); // �V�X�e����Ԃ����������� ( SYSTEM_STATE_DOWN )
//---------------------------------------------------------------------------------
// ���V�X�e������
//---------------------------------------------------------------------------------
static SYSTEM_STATE GetSystemState( const ISS_CITY_SYS* system ); // �V�X�e����Ԃ��擾����
static void SetSystemState( ISS_CITY_SYS* system, SYSTEM_STATE state ); // �V�X�e����Ԃ�ύX����
static void BootSystem( ISS_CITY_SYS* system ); // �V�X�e�����N������
static void DownSystem( ISS_CITY_SYS* system ); // �V�X�e�����I������
static void StopSystem( ISS_CITY_SYS* system ); // �V�X�e�����~����
static void ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID ); // �]�[���̐؂�ւ��������s��
static u16 GetCurrentZoneID( const ISS_CITY_SYS* system ); // ���݂̃]�[��ID���擾����
//---------------------------------------------------------------------------------
// �����j�b�g
//---------------------------------------------------------------------------------
static BOOL CheckUnitExist( const ISS_CITY_SYS* system ); // ���j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
static void ChangeUnit( ISS_CITY_SYS* system, u16 zoneID ); // ���j�b�g��ύX����


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
  OS_TFPrintf( PRINT_DEST, "ISS-C: create\n" );
#endif

	// �V�X�e���𐶐�
	system = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof(ISS_CITY_SYS) );

	// �V�X�e����������
  InitializeSystem( system, heapID, player );
  SetupSystem( system ); 
	
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
  // �V�X�e�����N���[���A�b�v
  CleanUpSystem( system );

	// �{�̂̔j��
	GFL_HEAP_FreeMemory( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: delete\n" );
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

	system->heapID        = heapID;
	system->player        = player;
  system->state         = SYSTEM_STATE_STOP;
  system->trackVolume   = MIN_TRACK_VOLUME;
  system->systemVolume  = MAX_SYSTEM_VOLUME;
	system->unit          = NULL;
	system->unitNum       = 0;
  system->unitIdx       = UNIT_IDX_NULL;

  SetSystemState( system, SYSTEM_STATE_STOP );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����Z�b�g�A�b�v���� 
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SetupSystem( ISS_CITY_SYS* system )
{
  GF_ASSERT( system );

	LoadUnitData( system ); // ���j�b�g����ǂݍ���

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: setup system\n" );
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

  UnloadUnitData( system ); // ���j�b�g�f�[�^��j������

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: clean up system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��ǂݍ���
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void LoadUnitData( ISS_CITY_SYS* system )
{
  int dataNum;
  int datID;

  GF_ASSERT( system->unit == NULL );

  // ���j�b�g�����擾
  dataNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_CITY );
  system->unitNum = dataNum;

  // �e���j�b�g�f�[�^�̃��[�N���m��
  system->unit = GFL_HEAP_AllocMemory( system->heapID, dataNum * sizeof(ISS_C_UNIT*) );

  // �e���j�b�g�f�[�^��ǂݍ���
  for( datID=0; datID < dataNum; datID++ )
  {
    system->unit[ datID ] = ISS_C_UNIT_Create( system->heapID, datID );
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

  // ��Ԃɉ�����������
  switch( state ) {
  case SYSTEM_STATE_STOP: InitSystemState_forSTOP( system ); break;
  case SYSTEM_STATE_BOOT: InitSystemState_forBOOT( system ); break;
  case SYSTEM_STATE_RUN:  InitSystemState_forRUN( system );  break;
  case SYSTEM_STATE_DOWN: InitSystemState_forDOWN( system ); break;
  default: GF_ASSERT(0);
  }

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: set system state ==> " );
  switch( state ) {
  case SYSTEM_STATE_STOP: OS_TFPrintf( PRINT_DEST, "STOP" ); break;
  case SYSTEM_STATE_BOOT: OS_TFPrintf( PRINT_DEST, "BOOT" ); break;
  case SYSTEM_STATE_RUN:  OS_TFPrintf( PRINT_DEST, "RUN" );  break;
  case SYSTEM_STATE_DOWN: OS_TFPrintf( PRINT_DEST, "DOWN" ); break;
  default: GF_ASSERT(0);
  }
  OS_TFPrintf( PRINT_DEST, "\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ����������� ( SYSTEM_STATE_STOP )
 * 
 * @param system 
 */
//---------------------------------------------------------------------------------
static void InitSystemState_forSTOP( ISS_CITY_SYS* system )
{
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_STOP );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system state for STOP\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ����������� ( SYSTEM_STATE_BOOT )
 * 
 * @param system 
 */
//---------------------------------------------------------------------------------
static void InitSystemState_forBOOT( ISS_CITY_SYS* system )
{
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_BOOT );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system state for BOOT\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ����������� ( SYSTEM_STATE_RUN )
 * 
 * @param system 
 */
//---------------------------------------------------------------------------------
static void InitSystemState_forRUN( ISS_CITY_SYS* system )
{
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_RUN );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system state for RUN\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e����Ԃ����������� ( SYSTEM_STATE_DOWN )
 * 
 * @param system 
 */
//---------------------------------------------------------------------------------
static void InitSystemState_forDOWN( ISS_CITY_SYS* system )
{
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_DOWN );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: init system state for DOWN\n" );
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
  // ���łɋN����
  if( (GetSystemState(system) == SYSTEM_STATE_BOOT) ||
      (GetSystemState(system) == SYSTEM_STATE_RUN) ) { return; }

  // ���j�b�g��ύX
  ChangeUnit( system, GetCurrentZoneID(system) ); 

  // ���j�b�g������
  if( CheckUnitExist(system) == TRUE ) {
    // �N��
    SetSystemState( system, SYSTEM_STATE_BOOT );
  }
  // ���j�b�g���Ȃ�
  else {
    // �I��
    SetSystemState( system, SYSTEM_STATE_DOWN );
  }

  // ���ʂ𒲐�
  UpdateTrackVolume( system );
  ChangeBGMTrackVolume( system );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: boot system\n" );
#endif
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����I������
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void DownSystem( ISS_CITY_SYS* system )
{
  // ���łɒ�~��
  if( (GetSystemState(system) == SYSTEM_STATE_DOWN) ||
      (GetSystemState(system) == SYSTEM_STATE_STOP) ) { return; }

  // �I��������
  SetSystemState( system, SYSTEM_STATE_DOWN );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: down system\n" );
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
  // ��~������
  SetSystemState( system, SYSTEM_STATE_STOP );

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
  // ���j�b�g��ύX����
  ChangeUnit( system, nextZoneID );

  // ���j�b�g�����݂��Ȃ�
  if( CheckUnitExist(system) == FALSE ) {
    DownSystem( system ); // �V�X�e�����I������
  }
  // ���j�b�g�����݂���
  else {
    BootSystem( system ); // �V�X�e�����N������
  }

  UpdateTrackVolume( system ); // �g���b�N�{�����[����������
  ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
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
 * @brief �V�X�e���̃��C������
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain( ISS_CITY_SYS* system )
{
  switch( GetSystemState(system) ) {
  case SYSTEM_STATE_STOP: SystemMain_forSTOP( system ); break;
  case SYSTEM_STATE_BOOT: SystemMain_forBOOT( system ); break;
  case SYSTEM_STATE_RUN:  SystemMain_forRUN( system );  break;
  case SYSTEM_STATE_DOWN: SystemMain_forDOWN( system ); break;
  default: GF_ASSERT(0);
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_STOP )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_forSTOP( ISS_CITY_SYS* system )
{
  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_STOP );
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_BOOT )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_forBOOT( ISS_CITY_SYS* system )
{
  BOOL sysVolChanged;
  BOOL trkVolChanged;

  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_BOOT );

  sysVolChanged = UpdateSystemVolume( system ); // ��ʃ{�����[�����X�V
  trkVolChanged = UpdateTrackVolume( system );  // �g���b�N�{�����[�����X�V
  
  // �{�����[�����ω�
  if( sysVolChanged || trkVolChanged ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }

  // �N������
  if( MAX_SYSTEM_VOLUME <= GetSystemVolume(system) ) {
    SetSystemState( system, SYSTEM_STATE_RUN );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_RUN )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_forRUN( ISS_CITY_SYS* system )
{
  BOOL sysVolChanged;
  BOOL trkVolChanged;

  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_RUN ); 

  sysVolChanged = UpdateSystemVolume( system ); // ��ʃ{�����[�����X�V
  trkVolChanged = UpdateTrackVolume( system );  // �g���b�N�{�����[�����X�V
  
  // �{�����[�����ω�
  if( sysVolChanged || trkVolChanged ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief �V�X�e���̃��C������֐� ( SYSTEM_STATE_DOWN )
 *
 * @param system
 */
//---------------------------------------------------------------------------------
static void SystemMain_forDOWN( ISS_CITY_SYS* system )
{
  BOOL sysVolChanged;
  BOOL trkVolChanged;

  GF_ASSERT( GetSystemState(system) == SYSTEM_STATE_DOWN );

  sysVolChanged = UpdateSystemVolume( system ); // ��ʃ{�����[�����X�V
  trkVolChanged = UpdateTrackVolume( system );  // �g���b�N�{�����[�����X�V
  
  // �{�����[�����ω�
  if( sysVolChanged || trkVolChanged ) {
    ChangeBGMTrackVolume( system ); // BGM�ɔ��f������
  }

  // �I������
  if( GetSystemVolume(system) <= MIN_SYSTEM_VOLUME ) {
    SetSystemState( system, SYSTEM_STATE_STOP );
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�����݂��Ă��邩�ǂ������`�F�b�N����
 *
 * @param system
 * 
 * @return ���j�b�g�����݂��Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL CheckUnitExist( const ISS_CITY_SYS* system )
{
  if( system->unitIdx == UNIT_IDX_NULL )
  {
    return FALSE;
  }
  return TRUE;
}

//---------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���ɔz�u���ꂽ���j�b�g�ɐ؂�ւ���
 *
 * @param system
 * @param zoneID �؂�ւ���̃]�[��ID
 */
//---------------------------------------------------------------------------------
static void ChangeUnit( ISS_CITY_SYS* system, u16 zoneID )
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
			system->unitIdx = unitIdx;

#ifdef DEBUG_PRINT_ON
      OS_TFPrintf( PRINT_DEST, "ISS-C: change unit index ==> %d\n", unitIdx );
#endif
      return;
		}
	}

  // �z�u����Ă��Ȃ�
  system->unitIdx = UNIT_IDX_NULL;
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
 * @brief ��ʃ{�����[�����X�V����
 *
 * @param system
 *
 * @return ��ʃ{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//---------------------------------------------------------------------------------
static BOOL UpdateSystemVolume( ISS_CITY_SYS* system )
{
  int nextVolume;

  GF_ASSERT( system );
  GF_ASSERT( GetSystemState(system) != SYSTEM_STATE_STOP );

  // �{�����[�����v�Z
  if( GetSystemState(system) == SYSTEM_STATE_DOWN ) {
    nextVolume = GetSystemVolume(system) - SYSTEM_VOLUME_FADEOUT_SPEED;
  }
  else {
    nextVolume = GetSystemVolume(system) + SYSTEM_VOLUME_FADEIN_SPEED;
  }

  // �ő�l�E�ŏ��l�␳
  if( MAX_SYSTEM_VOLUME <= nextVolume ) { nextVolume = MAX_SYSTEM_VOLUME; }
  if( nextVolume <= MIN_SYSTEM_VOLUME ) { nextVolume = MIN_SYSTEM_VOLUME; }

  // �{�����[���ω��Ȃ�
  if( nextVolume == GetSystemVolume(system) ) { return FALSE; }

  // ��ʃ{�����[�����X�V
  SetSystemVolume( system, nextVolume );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_DEST, "ISS-C: update system volume => %d\n", nextVolume );
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

  // �V�X�e����~��
  if( GetSystemState(system) == SYSTEM_STATE_STOP ) { return FALSE; }

  // ���j�b�g���z�u����Ă��Ȃ�
  if( CheckUnitExist(system) == FALSE ) { return FALSE; }

  // ���j�b�g�Ǝ��@�̈ʒu����{�����[�����Z�o
  unit      = system->unit[ system->unitIdx ];
  playerPos = PLAYERWORK_getPosition( system->player );
  newVolume = ISS_C_UNIT_GetVolume( unit, playerPos );

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
  volume = CalcBGMTrackVolume( system );

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
static int CalcBGMTrackVolume( const ISS_CITY_SYS* system )
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
