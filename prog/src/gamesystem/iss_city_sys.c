/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_sys.c
 * @brief  �XISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "../../include/field/field_const.h"		// FIELD_CONST_GRID_FX32_SIZE
#include "arc/arc_def.h"
#include "field/field_const.h"
#include "iss_city_unit.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "sound/pm_sndsys.h"


//===========================================================================================
// ���萔�E�}�N��
//===========================================================================================
// �f�o�b�O���̏o�͐�
#define PRINT_DEST (1)

// �������j�b�g�ԍ�
// ( �XISS��BGM�����Ă��邪, ISS���j�b�g���z�u����Ă��Ȃ���. )
#define UNIT_NOT_FOUND (0xff)

#define MIN_VOLUME               (0)   // �ŏ��{�����[��
#define MAX_VOLUME               (127) // �ő�{�����[��
#define MASTER_VOLUME_FADE_SPEED (2)   // �}�X�^�[�{�����[���̃t�F�[�h���x

// ����g���b�N
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // 9,10�g���b�N


//===========================================================================================
// ���XISS�V�X�e���\����
//===========================================================================================
struct _ISS_CITY_SYS
{
	HEAPID       heapID; 
	PLAYER_WORK* player; // �Ď��Ώۃv���C���[

	// �V�X�e��
	BOOL boot;		               // �N�������ǂ���
	u8   activeUnitIdx;	         // ���쒆�̃��j�b�g�ԍ�
  int  trackVolume;            // ����g���b�N�̃{�����[��
  int  masterVolume;           // �}�X�^�[�{�����[�� ( �ݒ�l )
  int  practicalMasterVolume;  // �}�X�^�[�{�����[�� ( ���s�l )

	// ���j�b�g
	ISS_C_UNIT** unit;		 // ���j�b�g�z��
	u8           unitNum;	 // ���j�b�g��
};


//===========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//===========================================================================================
// ����/�j��
static void InitializeSystem( ISS_CITY_SYS* system );
static void LoadUnitData    ( ISS_CITY_SYS* system );
static void UnloadUnitData  ( ISS_CITY_SYS* system );
// �V�X�e������
static void BootSystem( ISS_CITY_SYS* system );
static void StopSystem( ISS_CITY_SYS* system );
static void ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID );
static void SystemMain( ISS_CITY_SYS* system );
// ���j�b�g����
static void ChangeUnit( ISS_CITY_SYS* system, u16 zoneID ); 
// �{�����[������ ( �}�X�^�[�{�����[�� )
static BOOL UpdateMasterVolume( ISS_CITY_SYS* system );
static void SetMasterVolume   ( ISS_CITY_SYS* system, int volume );
// �{�����[������ ( �g���b�N�{�����[�� )
static BOOL UpdateTrackVolume   ( ISS_CITY_SYS* system );
static void ChangeBGMTrackVolume( const ISS_CITY_SYS* system );


//===========================================================================================
// �����J�֐��̎���
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e�����쐬����
 *
 * @param  player  �Ď��Ώۂ̃v���C���[
 * @param  heapID  �g�p����q�[�vID
 * 
 * @return �XISS�V�X�e��
 */
//-------------------------------------------------------------------------------------------
ISS_CITY_SYS* ISS_CITY_SYS_Create( PLAYER_WORK* player, HEAPID heapID )
{
	ISS_CITY_SYS* system;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: create\n" );

	// ����
	system = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof( ISS_CITY_SYS ) );

	// ������
  InitializeSystem( system );
	system->heapID = heapID;
	system->player = player;

	// ���j�b�g���̓ǂݍ���
	LoadUnitData( system );
	
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
  // ���j�b�g�f�[�^�̔j��
  UnloadUnitData( system );

	// �{�̂̔j��
	GFL_HEAP_FreeMemory( system );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: delete\n" );
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


//===========================================================================================
// ������J�֐��̎���
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e��������������
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void InitializeSystem( ISS_CITY_SYS* system )
{
	system->heapID                = 0;
	system->player                = NULL;
	system->boot                  = FALSE;
	system->activeUnitIdx         = UNIT_NOT_FOUND;
  system->masterVolume          = MAX_VOLUME;
  system->practicalMasterVolume = MAX_VOLUME;
	system->unitNum               = 0;
	system->unit                  = NULL;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��ǂݍ���
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void LoadUnitData( ISS_CITY_SYS* system )
{
  int datnum;
  int datid;

  GF_ASSERT( system->unit == NULL );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: load unit data\n" );

  // ���j�b�g�� �擾
  datnum          = GFL_ARC_GetDataFileCnt( ARCID_ISS_CITY );
  system->unitNum = datnum;

  // �e���j�b�g�f�[�^ ���[�N����
  system->unit = GFL_HEAP_AllocMemory( system->heapID, datnum * sizeof(ISS_C_UNIT*) );

  // �e���j�b�g�f�[�^��ǂݍ���
  for( datid=0; datid < datnum; datid++ )
  {
    system->unit[ datid ] = ISS_C_UNIT_Create( system->heapID, datid );
  } 
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��j������
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void UnloadUnitData( ISS_CITY_SYS* system )
{
  int unitIdx;
  int unitNum;

  GF_ASSERT( system->unit );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: unload unit data\n" );

  unitNum = system->unitNum;

	// �e���j�b�g��j��
  for( unitIdx=0; unitIdx < unitNum; unitIdx++ )
  {
    ISS_C_UNIT_Delete( system->unit[ unitIdx ] );
  }
	GFL_HEAP_FreeMemory( system->unit );
  system->unit = NULL;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void BootSystem( ISS_CITY_SYS* system )
{
  // �N����
  if( system->boot ){ return; }

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: boot\n" );

  // �N��
  system->boot        = TRUE;
  system->trackVolume = MIN_VOLUME; 

  // ���j�b�g����
  {
    u16 zoneID;

    zoneID = PLAYERWORK_getZoneID( system->player );
    ChangeUnit( system, zoneID ); 
  } 

  // ���ʂ𒲐�
  UpdateTrackVolume( system );
  ChangeBGMTrackVolume( system );
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void StopSystem( ISS_CITY_SYS* system )
{
  // ��~��
  if( system->boot == FALSE ){ return; }

  // ��~
	system->boot          = FALSE;
  system->activeUnitIdx = UNIT_NOT_FOUND;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: stop\n" );
} 

//-------------------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm����
 *
 * @param system
 * @param nextZoneID �؂�ւ���̃]�[��ID
 */
//-------------------------------------------------------------------------------------------
static void ZoneChange( ISS_CITY_SYS* system, u16 nextZoneID )
{
  // ��~��
  if( system->boot == FALSE ){ return; }

  // ���j�b�g��؂�ւ���
  ChangeUnit( system, nextZoneID );

  // �}�X�^�[�{�����[���ݒ�
  if( system->activeUnitIdx == UNIT_NOT_FOUND )
  {
    SetMasterVolume( system, MIN_VOLUME );
  }
  else
  {
    SetMasterVolume( system, MAX_VOLUME );
  }

  // ���ʂ𒲐�
  if( UpdateTrackVolume( system ) )
  {
    ChangeBGMTrackVolume( system ); 
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e������
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void SystemMain( ISS_CITY_SYS* system )
{
  BOOL changeMasterVolume;
  BOOL changeTrackVolume;

  // ��~��
  if( system->boot == FALSE ){ return; }

  // �}�X�^�[�{�����[���X�V
  changeMasterVolume = UpdateMasterVolume( system );

  // �{�����[������
  changeTrackVolume = UpdateTrackVolume( system );
  
  // �{�����[�����ω�
  if( changeMasterVolume || changeTrackVolume )
  {
    ChangeBGMTrackVolume( system );
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���ɔz�u���ꂽ���j�b�g�ɐ؂�ւ���
 *
 * @param system
 * @param zoneID �؂�ւ���̃]�[��ID
 */
//-------------------------------------------------------------------------------------------
static void ChangeUnit( ISS_CITY_SYS* system, u16 zoneID )
{
	int unitIdx, unitNum;

  // �N�����Ă��Ȃ�
  GF_ASSERT( system->boot );

  unitNum = system->unitNum;

  // ����
	for( unitIdx=0; unitIdx < unitNum; unitIdx++ )
	{
		// ����
		if( ISS_C_UNIT_GetZoneID( system->unit[ unitIdx ] ) == zoneID )
		{ 
			system->activeUnitIdx = unitIdx;

      // DEBUG:
      OS_TFPrintf( PRINT_DEST, "ISS-C: change unit index ==> %d\n", unitIdx );
      return;
		}
	}

  // �z�u����Ă��Ȃ�
  system->activeUnitIdx = UNIT_NOT_FOUND;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �}�X�^�[�{�����[�����X�V����
 *
 * @param system
 *
 * @return �}�X�^�[�{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------------------
static BOOL UpdateMasterVolume( ISS_CITY_SYS* system )
{
  int nowVolume;
  int destVolume;
  int nextVolume;

  // �N�����Ă��Ȃ�
  GF_ASSERT( system->boot );

  // ���ݒl/�ڕW�l
  nowVolume  = system->practicalMasterVolume;
  destVolume = system->masterVolume;

  // �t�F�[�h�����ς�
  if( nowVolume == destVolume ){ return FALSE; }

  // �t�F�[�h
  if( destVolume < nowVolume )
  {
    nextVolume = nowVolume - MASTER_VOLUME_FADE_SPEED;
  }
  else
  {
    nextVolume = nowVolume + MASTER_VOLUME_FADE_SPEED;
  }

  // �ŏ�/�ő�␳
  if( nextVolume < MIN_VOLUME ){ nextVolume = MIN_VOLUME; }
  if( MAX_VOLUME < nextVolume ){ nextVolume = MAX_VOLUME; }

  // �}�X�^�[�X�V
  system->practicalMasterVolume = nextVolume;

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: update master volume => %d\n", nextVolume );

  return TRUE;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �}�X�^�[�{�����[����ݒ肷��
 *
 * @param system
 * @param volume �ݒ肷��{�����[��
 */
//-------------------------------------------------------------------------------------------
static void SetMasterVolume( ISS_CITY_SYS* system, int volume )
{
  GF_ASSERT( system->boot );
  GF_ASSERT( MIN_VOLUME <= volume );
  GF_ASSERT( volume <= MAX_VOLUME );

  system->masterVolume = volume;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �g���b�N�{�����[���𒲐�����
 *
 * @param system
 *
 * @return �{�����[�����ω������ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-------------------------------------------------------------------------------------------
static BOOL UpdateTrackVolume( ISS_CITY_SYS* system )
{
  int newVolume = 0;
  const VecFx32* playerPos;

  // �N�����Ă��Ȃ�
  GF_ASSERT( system->boot );  

  // ���j�b�g���z�u����Ă��Ȃ�
  if( system->activeUnitIdx == UNIT_NOT_FOUND ){ return FALSE; }

  // ���j�b�g�Ǝ��@�̈ʒu����{�����[�����Z�o
  playerPos = PLAYERWORK_getPosition( system->player );
  newVolume = ISS_C_UNIT_GetVolume( system->unit[ system->activeUnitIdx ], playerPos );

  // �{�����[�����ω������ꍇ
  if( system->trackVolume != newVolume )
  {
    system->trackVolume = newVolume; 
    return TRUE;
  }
  return FALSE;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief BGM�̃{�����[����ύX����
 *
 * @param system
 */
//-------------------------------------------------------------------------------------------
static void ChangeBGMTrackVolume( const ISS_CITY_SYS* system )
{
  int trackVolume;
  int masterVolume;
  int volume;

  // ���{�����[���l���Z�o
  trackVolume  = system->trackVolume;
  masterVolume = system->practicalMasterVolume;
  volume       = trackVolume * masterVolume / MAX_VOLUME;

  // �{�����[���̒l��`�F�b�N
  GF_ASSERT( MIN_VOLUME   <= trackVolume );
  GF_ASSERT( MIN_VOLUME   <= masterVolume );
  GF_ASSERT( MIN_VOLUME   <= volume );
  GF_ASSERT( trackVolume  <= MAX_VOLUME );
  GF_ASSERT( masterVolume <= MAX_VOLUME );
  GF_ASSERT( volume       <= MAX_VOLUME );

  // �g���b�N�{�����[���ύX
  PMSND_ChangeBGMVolume( TRACKBIT, volume );

  // DEBUG:
  OS_TFPrintf( PRINT_DEST, "ISS-C: change BGM track volume ==> %d\n", volume );
}
