////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_dungeon_sys.h
 * @brief  �_���W����ISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_dungeon_sys.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/pm_season.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h"
#include "arc/arc_def.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../field/field_sound.h"


//=========================================================================================
// ���萔�E�}�N��
//=========================================================================================
//#define DEBUG_PRINT_ON // �f�o�b�O�o�̓X�C�b�`
#define PRINT_TARGET (1)

// �����ȃ]�[��ID
#define INVALID_ZONE_ID (0xffff)

// �s�b�`(�L�[)�ݒ�Ώۃg���b�N
#define PITCH_TRACK_MASK (~((1<<(9-1)) | (1<<(10-1))))  // 9, 10�g���b�N

// ���o�[�u�ݒ�
#define REVERB_SAMPLE_RATE (16000)	// �T���v�����O���[�g
#define REVERB_VOLUME      (63)		  // �{�����[��
#define REVERB_STOP_FRAME  (0)		  // ��~�܂ł̃t���[����


//=========================================================================================
// ��BGM�p�����[�^
//=========================================================================================
typedef struct {

	u16 zoneID;	                // �]�[��ID
  u16 padding;
	s16 pitch[PMSEASON_TOTAL];	// �s�b�`(�L�[)
	u16 tempo[PMSEASON_TOTAL];	// �e���|
	u16 reverb[PMSEASON_TOTAL];	// ���o�[�u

} BGM_PARAM; 

//------------------------------------------------------------------------------------------
/**
 * @brief �w��p�����[�^�𔽉f������
 *
 * @param param  �ݒ肷��p�����[�^
 * @param season �G�߂��w��
 */
//------------------------------------------------------------------------------------------
static void SetBGMParam( const BGM_PARAM* param, u8 season )
{
  GF_ASSERT( param );
  GF_ASSERT( season < PMSEASON_TOTAL );

  // �e���|
	PMSND_SetStatusBGM( param->tempo[season], PMSND_NOEFFECT, 0 ); 

  // �s�b�`
	NNS_SndPlayerSetTrackPitch( 
      PMSND_GetBGMhandlePointer(), PITCH_TRACK_MASK, param->pitch[season] ); 

  // ���o�[�u
#if 0 // 2010.04.20 ���o�[�u�͎g�p���Ȃ�!!
  if( param->reverb[season] == 0 )
  {
    PMSND_DisableCaptureReverb();
  }
  else
  {
    PMSND_EnableCaptureReverb( 
        param->reverb[season], REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );
  }
#endif

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: set BGM param\n" );
	OS_TFPrintf( PRINT_TARGET, "- pitch  = %d\n", param->pitch[season] );
	OS_TFPrintf( PRINT_TARGET, "- tempo  = %d\n", param->tempo[season] );
	OS_TFPrintf( PRINT_TARGET, "- reverb = %d\n", param->reverb[season] );
#endif 
}


//=========================================================================================
// ���_���W����ISS�f�[�^�Z�b�g
//=========================================================================================
typedef struct {

	u8         dataNum;	 // �f�[�^��
	BGM_PARAM*   param;	 // �p�����[�^�z��

} BGM_PARAMSET;


//-----------------------------------------------------------------------------------------
/**
 * @brief �f�[�^��ǂݍ���
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �ǂݍ��񂾃f�[�^
 */
//-----------------------------------------------------------------------------------------
static BGM_PARAMSET* LoadParamset( HEAPID heap_id )
{
	int dat_id;
	BGM_PARAMSET* paramset;

	// �{�̂��쐬
	paramset          = (BGM_PARAMSET*)GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAMSET) ); 
	paramset->dataNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_DUNGEON ); 
	paramset->param   = GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAM) * paramset->dataNum );

	// �S�f�[�^���擾
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{
    GFL_ARC_LoadDataOfs( &paramset->param[dat_id], 
                         ARCID_ISS_DUNGEON, dat_id, 0, sizeof(BGM_PARAM) );
	} 

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: load BGM parameters\n" );
	OS_TFPrintf( PRINT_TARGET, "- dataNum = %d\n", paramset->dataNum );
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{ 
		OS_TFPrintf( PRINT_TARGET, "- data[%d].zoneID = %d\n", dat_id, paramset->param[dat_id].zoneID ); 
		OS_TFPrintf( PRINT_TARGET, "- data[%d].pitch  = %d, %d, %d, %d\n", dat_id, 
        paramset->param[dat_id].pitch[PMSEASON_SPRING], 
        paramset->param[dat_id].pitch[PMSEASON_SUMMER], 
        paramset->param[dat_id].pitch[PMSEASON_AUTUMN], 
        paramset->param[dat_id].pitch[PMSEASON_WINTER] );
		OS_TFPrintf( PRINT_TARGET, "- data[%d].tempo  = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].tempo[PMSEASON_SPRING], 
        paramset->param[dat_id].tempo[PMSEASON_SUMMER], 
        paramset->param[dat_id].tempo[PMSEASON_AUTUMN], 
        paramset->param[dat_id].tempo[PMSEASON_WINTER] );
		OS_TFPrintf( PRINT_TARGET, "- data[%d].reverb = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].reverb[PMSEASON_SPRING], 
        paramset->param[dat_id].reverb[PMSEASON_SUMMER], 
        paramset->param[dat_id].reverb[PMSEASON_AUTUMN], 
        paramset->param[dat_id].reverb[PMSEASON_WINTER] );
	}
#endif

	// �ǂݍ��񂾃f�[�^��Ԃ�
	return paramset;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �f�[�^��j������
 *
 * @param paramset �j������f�[�^
 */
//-----------------------------------------------------------------------------------------
static void UnloadIssData( BGM_PARAMSET* paramset )
{ 
	// �p�����[�^�z���j��
	GFL_HEAP_FreeMemory( paramset->param );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( paramset );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �w��]�[��ID��BGM�p�����[�^���擾����
 *
 * @param paramset �������f�[�^
 * @param zone_id  �����Ώۃ]�[��ID
 *
 * @return �w��]�[��ID�̃p�����[�^(���݂��Ȃ��ꍇ, NULL)
 */
//-----------------------------------------------------------------------------------------
static const BGM_PARAM* GetBGMParam( const BGM_PARAMSET* paramset, u16 zone_id )
{
	int i;

	// �w��]�[��ID������
	for( i=0; i<paramset->dataNum; i++ )
	{
		// ����
		if( paramset->param[i].zoneID == zone_id )
		{
			return &paramset->param[i];
		}
	}

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: BGM param is not found\n" );
#endif

	return NULL;
}


//=========================================================================================
// ���_���W����ISS�V�X�e���Ǘ����[�N
//=========================================================================================
struct _ISS_DUNGEON_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώ�
	PLAYER_WORK* player;  // �v���C���[
  GAMEDATA*     gdata;  // �Q�[���f�[�^

	// �N�����
	BOOL boot_flag; 

	// �]�[��ID
	u16 currentZoneID;
	u16 nextZoneID;

	// �f�[�^
	BGM_PARAMSET* paramset;

	// ���݂̐ݒ�f�[�^
	const BGM_PARAM* pActiveParam;

  // �f�t�H���g�E�p�����[�^
  BGM_PARAM defaultParam;
};


//=========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//=========================================================================================
static void SetupDefaultParam( ISS_DUNGEON_SYS* sys );
static void BootSystem( ISS_DUNGEON_SYS* sys );
static void StopSystem( ISS_DUNGEON_SYS* sys );
static void UpdateBGMParam( ISS_DUNGEON_SYS* sys );


//=========================================================================================
// �����J�֐�
//========================================================================================= 

//-----------------------------------------------------------------------------------------
/**
 * @brief  �_���W����ISS�V�X�e�����쐬����
 *
 * @param  gdata   �Q�[���f�[�^
 * @param  player  �Ď��Ώۂ̃v���C���[
 * @param  heap_id �g�p����q�[�vID
 * 
 * @return �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
ISS_DUNGEON_SYS* ISS_DUNGEON_SYS_Create( GAMEDATA* gdata, 
                                         PLAYER_WORK* player, HEAPID heap_id )
{
	ISS_DUNGEON_SYS* sys;

	// �������m��
	sys = (ISS_DUNGEON_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_DUNGEON_SYS) );

	// ������
	sys->heapID        = heap_id;
  sys->gdata         = gdata;
	sys->player        = player;
	sys->boot_flag     = FALSE;
	sys->currentZoneID = INVALID_ZONE_ID;
	sys->nextZoneID    = INVALID_ZONE_ID;
	sys->paramset      = LoadParamset( heap_id );
	sys->pActiveParam  = NULL;

  // �f�t�H���g�p�����[�^�ݒ�
  SetupDefaultParam( sys );

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: create\n" );
#endif

	// �쐬�����_���W����ISS�V�X�e����Ԃ�
	return sys;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief  �_���W����ISS�V�X�e����j������
 *
 * @param sys �j������_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Delete( ISS_DUNGEON_SYS* sys )
{
	// �V�X�e����~
	ISS_DUNGEON_SYS_Off( sys );
	
	// �f�[�^��j��
	UnloadIssData( sys->paramset );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( sys );

#ifdef DEBUG_PRINT_ON
	OS_TFPrintf( PRINT_TARGET, "ISS-D: delete\n" );
#endif
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param sys ����Ώۂ̃_���W����ISS�V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Update( ISS_DUNGEON_SYS* sys )
{
  // �N�����ĂȂ�
  if( !sys->boot_flag ){ return; }

  // �X�V
  UpdateBGMParam( sys );
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys          �ʒm�Ώۂ̃_���W����ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* sys, u16 next_zone_id )
{ 
  // �]�[���X�V
	sys->nextZoneID = next_zone_id; 

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: zone change\n" );
  OS_TFPrintf( PRINT_TARGET, "- next zone id = %d\n", next_zone_id );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_On( ISS_DUNGEON_SYS* sys )
{
  BootSystem( sys );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param sys ��~������V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Off( ISS_DUNGEON_SYS* sys )
{
  StopSystem( sys );
}

//-----------------------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//-----------------------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsOn( const ISS_DUNGEON_SYS* sys )
{
	return sys->boot_flag; 
}

//-----------------------------------------------------------------------------------------
/**
 * @breif �ݒ�f�[�^�̗L���𒲂ׂ�
 *
 * @param system
 * @param zone_id
 * 
 * @return �w�肵���]�[���̐ݒ�f�[�^�������Ă���ꍇ TRUE
 *         �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsActiveAt( const ISS_DUNGEON_SYS* system, u16 zone_id )
{
  // �f�[�^��ǂݍ���ł��Ȃ�
  if( system->paramset == NULL ) {
    GF_ASSERT(0);
    return FALSE;
  }

  // �w�肳�ꂽ�]�[���̃f�[�^�͎����Ă��Ȃ�
  if( GetBGMParam( system->paramset, zone_id ) == NULL ) {
    return FALSE;
  }

  return TRUE;
}


//=========================================================================================
// ������J�֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �f�t�H���g�p�����[�^��ݒ肷��
 *
 * @param sys �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
static void SetupDefaultParam( ISS_DUNGEON_SYS* sys )
{
  int i;
  sys->defaultParam.zoneID = INVALID_ZONE_ID;
  for( i=0; i<PMSEASON_TOTAL; i++ )
  {
    sys->defaultParam.pitch[i]  = 0;
    sys->defaultParam.tempo[i]  = 256;
    sys->defaultParam.reverb[i] = 0;
  }
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
static void BootSystem( ISS_DUNGEON_SYS* sys )
{
  // �N���ς�
  if( sys->boot_flag ){ return; }

  // �N��
	sys->boot_flag = TRUE;

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: boot\n" );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
static void StopSystem( ISS_DUNGEON_SYS* sys )
{
  // ��~�ς�
  if( !sys->boot_flag ){ return; }

	// ��~
	sys->boot_flag = FALSE;

  // �f�t�H���g�E�p�����[�^�ɖ߂�
  //SetBGMParam( &sys->defaultParam, 0 );

#ifdef DEBUG_PRINT_ON
  OS_TFPrintf( PRINT_TARGET, "ISS-D: stop\n" );
#endif
}

//-----------------------------------------------------------------------------------------
/**
 * @brief BGM�p�����[�^���X�V����
 *
 * @param sys �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
static void UpdateBGMParam( ISS_DUNGEON_SYS* sys )
{
	// �N�����Ă��Ȃ�
  GF_ASSERT( sys->boot_flag );

	// �]�[���؂�ւ����ʒm���ꂽ�ꍇ
	if( sys->currentZoneID != sys->nextZoneID )
	{
		// �X�V
		sys->currentZoneID = sys->nextZoneID;

		// �V�]�[��ID��BGM�p�����[�^������
		sys->pActiveParam = GetBGMParam( sys->paramset, sys->nextZoneID );

    // BGM�p�����[�^���ݒ肳��Ă��Ȃ�
    if( sys->pActiveParam == NULL ){ return; }

		// BGM�̐ݒ�𔽉f������
    {
      u8 season = GAMEDATA_GetSeasonID( sys->gdata );
      SetBGMParam( sys->pActiveParam, season ); 
    }
	} 
} 
