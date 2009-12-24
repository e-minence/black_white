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
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "arc/arc_def.h"
#include "gamesystem/pm_season.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//==========================================================================================
// ���萔�E�}�N��
//========================================================================================== 
// �����ȃ]�[��ID
#define INVALID_ZONE_ID (0xffff)

// �s�b�`(�L�[)�ݒ�Ώۃg���b�N
#define PITCH_TRACK_MASK (~((1<<(9-1)) | (1<<(10-1))))  // 9, 10�g���b�N

// ���o�[�u�ݒ�
#define REVERB_SAMPLE_RATE (16000)	// �T���v�����O���[�g
#define REVERB_VOLUME      (63)		  // �{�����[��
#define REVERB_STOP_FRAME  (0)		  // ��~�܂ł̃t���[����


//========================================================================================== 
// ��BGM�p�����[�^
//========================================================================================== 
typedef struct
{
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
static void SetBGMStatus( const BGM_PARAM* param, u8 season )
{
	if( !param ) return;
  if( PMSEASON_TOTAL <= season ) return;

  // �e���|
	PMSND_SetStatusBGM( param->tempo[season], PMSND_NOEFFECT, 0 ); 
  // �s�b�`
	NNS_SndPlayerSetTrackPitch( 
      PMSND_GetBGMhandlePointer(), PITCH_TRACK_MASK, param->pitch[season] ); 
  // ���o�[�u
  if( param->reverb[season] == 0 )
  {
    PMSND_DisableCaptureReverb();
  }
  else
  {
    PMSND_EnableCaptureReverb( 
        param->reverb[season], REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );
  }

	// DEBUG:
	OBATA_Printf( "ISS-D: Set BGM status\n" );
	OBATA_Printf( "- pitch  = %d\n", param->pitch[season] );
	OBATA_Printf( "- tempo  = %d\n", param->tempo[season] );
	OBATA_Printf( "- reverb = %d\n", param->reverb[season] );
}


//========================================================================================== 
// ���_���W����ISS�f�[�^
//========================================================================================== 
typedef struct
{
	u8         dataNum;	// �ێ����
	BGM_PARAM* param;	  // �p�����[�^�z��

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
	paramset = (BGM_PARAMSET*)GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAMSET) );

	// �f�[�^�����擾
	paramset->dataNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_DUNGEON );

	// �o�b�t�@�m��
	paramset->param = GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAM) * paramset->dataNum );

	// �e�f�[�^���擾
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{
    GFL_ARC_LoadDataOfs( &paramset->param[dat_id], 
        ARCID_ISS_DUNGEON, dat_id, 0, sizeof(BGM_PARAM) );
	} 

	// DEBUG:
	OBATA_Printf( "ISS-D: Load BGM parameters\n" );
	OBATA_Printf( "- dataNum = %d\n", paramset->dataNum );
	for( dat_id=0; dat_id<paramset->dataNum; dat_id++ )
	{ 
		OBATA_Printf( "- data[%d].zoneID = %d\n", dat_id, paramset->param[dat_id].zoneID ); 
		OBATA_Printf( "- data[%d].pitch  = %d, %d, %d, %d\n", dat_id, 
        paramset->param[dat_id].pitch[PMSEASON_SPRING], 
        paramset->param[dat_id].pitch[PMSEASON_SUMMER], 
        paramset->param[dat_id].pitch[PMSEASON_AUTUMN], 
        paramset->param[dat_id].pitch[PMSEASON_WINTER] );
		OBATA_Printf( "- data[%d].tempo  = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].tempo[PMSEASON_SPRING], 
        paramset->param[dat_id].tempo[PMSEASON_SUMMER], 
        paramset->param[dat_id].tempo[PMSEASON_AUTUMN], 
        paramset->param[dat_id].tempo[PMSEASON_WINTER] );
		OBATA_Printf( "- data[%d].reverb = %d, %d, %d, %d\n", dat_id,
        paramset->param[dat_id].reverb[PMSEASON_SPRING], 
        paramset->param[dat_id].reverb[PMSEASON_SUMMER], 
        paramset->param[dat_id].reverb[PMSEASON_AUTUMN], 
        paramset->param[dat_id].reverb[PMSEASON_WINTER] );
	}

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

	// DEBUG: BGM�p�����[�^�����ł���
  OBATA_Printf( "ISS-D: BGM param is not found\n" );
	return NULL;
}


//========================================================================================== 
// ���_���W����ISS�V�X�e���Ǘ����[�N
//========================================================================================== 
struct _ISS_DUNGEON_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώ�
	PLAYER_WORK* player; // �v���C���[
  GAMEDATA* gdata;  // �Q�[���f�[�^

	// �N�����
	BOOL isActive; 

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


//========================================================================================== 
// �����J�֐�
//========================================================================================== 

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
	sys->isActive      = FALSE;
	sys->currentZoneID = INVALID_ZONE_ID;
	sys->nextZoneID    = INVALID_ZONE_ID;
	sys->paramset      = LoadParamset( heap_id );
	sys->pActiveParam  = NULL;
  // �f�t�H���g�p�����[�^�ݒ�
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

	// DEBUG:
	OBATA_Printf( "ISS-D: Create\n" );

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

	// DEBUG:
	OBATA_Printf( "ISS-D: Delete\n" );
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
	// �N�����Ă��Ȃ����, �������Ȃ�
	if( sys->isActive != TRUE ) return; 

	// �]�[���؂�ւ����ʒm���ꂽ�ꍇ
	if( sys->currentZoneID != sys->nextZoneID )
	{
		// �X�V
		sys->currentZoneID = sys->nextZoneID;

		// �V�]�[��ID��BGM�p�����[�^������
		sys->pActiveParam = GetBGMParam( sys->paramset, sys->nextZoneID );

    // BGM�p�����[�^���ݒ肳��Ă��Ȃ��ꍇ ==> �V�X�e����~
    if( sys->pActiveParam == NULL )
    {
      ISS_DUNGEON_SYS_Off( sys );
      return;
    }

		// BGM�̐ݒ�𔽉f������
    {
      u8 season = GAMEDATA_GetSeasonID( sys->gdata );
      SetBGMStatus( sys->pActiveParam, season ); 
    }
	} 
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys        �ʒm�Ώۂ̃_���W����ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* sys, u16 next_zone_id )
{ 
	sys->nextZoneID = next_zone_id; 

  // DEBUG:
  OBATA_Printf( "ISS-D: ZoneChange\n" );
  OBATA_Printf( "- next zone id = %d\n", next_zone_id );
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
  // �N���ς�
  if( sys->isActive ){ return; }

  // �N��
	sys->isActive = TRUE;

#if 0
  // �p�����[�^�ݒ�
  {
    u8 season = GAMEDATA_GetSeasonID( sys->gdata );
    SetBGMStatus( sys->pActiveParam, season ); 
  }
#endif

  // DEBUG:
  OBATA_Printf( "ISS-D: On\n" );
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
  // ��~�ς�
  if( !sys->isActive ){ return; }

	// ��~
	sys->isActive = FALSE;

  // �f�t�H���g�E�p�����[�^�ɖ߂�
  SetBGMStatus( &sys->defaultParam, 0 );

  // DEBUG:
  OBATA_Printf( "ISS-D: Off\n" );
}

//----------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//----------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsOn( const ISS_DUNGEON_SYS* sys )
{
	return sys->isActive; 
}
