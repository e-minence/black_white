////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_dungeon_sys.h
 * @brief  �_���W����ISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_dungeon_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "arc/arc_def.h"
#include "arc/iss.naix"
#include "arc/fieldmap/zone_id.h"	// TEMP:


//=====================================================================================================
/**
 * @brief �萔�E�}�N��
 */
//===================================================================================================== 

// �����ȃ]�[��ID
#define INVALID_ZONE_ID (0xffff)

// �s�b�`(�L�[)�ݒ�Ώۃg���b�N
#define PITCH_TRACK_MASK (~((1<<(9-1)) | (1<<(10-1))))

// ���o�[�u�ݒ�
#define REVERB_SAMPLE_RATE (16000)	// �T���v�����O���[�g
#define REVERB_VOLUME      (63)		// �{�����[��
#define REVERB_STOP_FRAME  (0)		// ��~�܂ł̃t���[����


//=====================================================================================================
/**
 * @brief BGM�p�����[�^
 */ 
//=====================================================================================================
typedef struct
{
	u16 zoneID;	// �]�[��ID
	s16 pitch;	// �s�b�`(�L�[)
	u16 tempo;	// �e���|
	u16 reverb;	// ���o�[�u
}
BGM_PARAM;

//------------------------------------------------------------------
/**
 * @brief BGM�p�����[�^�Ɋւ���, ����J�֐��̃v���g�^�C�v�錾
 */
//------------------------------------------------------------------

// �w��p�����[�^�𔽉f������
static void SetBGMStatus( const BGM_PARAM* p_param );


//=====================================================================================================
/**
 * @brief BGM�p�����[�^�Ɋւ���, ����J�֐��̎���
 */ 
//=====================================================================================================

//------------------------------------------------------------------
/**
 * @brief �w��p�����[�^�𔽉f������
 *
 * @param �ݒ肷��p�����[�^
 */
//------------------------------------------------------------------
static void SetBGMStatus( const BGM_PARAM* p_param )
{
	if( !p_param ) return;

	// ���f
	PMSND_SetStatusBGM( p_param->tempo, PMSND_NOEFFECT, 0 );
	NNS_SndPlayerSetTrackPitch( PMSND_GetBGMhandlePointer(), PITCH_TRACK_MASK, p_param->pitch );
	PMSND_ChangeCaptureReverb( p_param->reverb, REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );

	// DEBUG:
  /*
	OBATA_Printf( "Dungeon ISS SetBGMStatus\n" ); 
	OBATA_Printf( "pitch  = %d\n", p_param->pitch );
	OBATA_Printf( "tempo  = %d\n", p_param->tempo );
	OBATA_Printf( "reverb = %d\n", p_param->reverb );
  */
}


//=====================================================================================================
/**
 * @brief �_���W����ISS�f�[�^
 */
//=====================================================================================================
typedef struct
{
	u8         dataNum;	// �ێ����
	BGM_PARAM* param;	// �p�����[�^�z��
}
ISS_DATA;

//------------------------------------------------------------------
/**
 * @brief �_���W����ISS�f�[�^�Ɋւ���, ����J�֐��̃v���g�^�C�v�錾
 */
//------------------------------------------------------------------

// ���g���G���f�B�A����u16�l�擾�֐�
static u16 GetU16( u8* data, int pos );

// �f�[�^��ǂݍ���
static ISS_DATA* LoadIssData( HEAPID heap_id );

// �f�[�^��j������
static void UnloadIssData( ISS_DATA* p_data );

// �w��]�[��ID��BGM�p�����[�^���擾����
static const BGM_PARAM* GetBGMParam( const ISS_DATA* p_data, u16 zone_id );


//=====================================================================================================
/** 
 * @brief �_���W����ISS�f�[�^�Ɋւ���, ����J�֐��̎���
 */
//=====================================================================================================

//---------------------------------------------------------------------------- 
/**
 * @brief data + pos �̈ʒu����n�܂�2�o�C�g��, 
 *        ���g���G���f�B�A���ŕ���u16�̃f�[�^�Ƃ��ĉ��߂��l���擾����
 *
 * @param data �f�[�^�J�n�ʒu
 * @param pos  �J�n�ʒu�I�t�Z�b�g
 *
 * @return u16
 */
//---------------------------------------------------------------------------- 
static u16 GetU16( u8* data, int pos )
{
	u16 lower = (u16)( data[ pos ] );
	u16 upper = (u16)( data[ pos + 1 ] );
	u16 value = ( upper << 8 ) | lower;

	return value;
}

//----------------------------------------------------------------------------
/**
 * @brief �f�[�^��ǂݍ���
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �ǂݍ��񂾃f�[�^
 */
//----------------------------------------------------------------------------
static ISS_DATA* LoadIssData( HEAPID heap_id )
{
	int i;
	ISS_DATA* iss_data;
	u32 size;
	u8* p_data;
	u8 data_num;

	// �{�̂��쐬
	iss_data = (ISS_DATA*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_DATA ) );

	// �f�[�^�ǂݍ���
	p_data = (u8*)GFL_ARC_UTIL_LoadEx( ARCID_ISS_UNIT, NARC_iss_dungeon_bin, FALSE, heap_id, &size );

	// �f�[�^�����擾
	data_num = p_data[0];

	// �o�b�t�@�m��
	iss_data->dataNum = data_num; 
	iss_data->param   = (BGM_PARAM*)GFL_HEAP_AllocMemory( heap_id, sizeof( BGM_PARAM ) * data_num );

	// �e�f�[�^���擾
	for( i=0; i<data_num; i++ )
	{
		int pos     = 1 + ( sizeof( u16 ) + sizeof( u16 ) ) * i;
		u16 zone_id = GetU16( p_data, pos );
		u16 offset  = GetU16( p_data, pos + 2 );

		// DEBUG:
    /*
		OBATA_Printf( "pos[%d]     = %d\n", i, pos );
		OBATA_Printf( "zone_id[%d] = %d\n", i, zone_id );
		OBATA_Printf( "offset[%d]  = %d\n", i, offset );
    */

		iss_data->param[i].zoneID = GetU16( p_data, offset );
		iss_data->param[i].pitch  = (s16)GetU16( p_data, offset + 2 );
		iss_data->param[i].tempo  = GetU16( p_data, offset + 4 );
		iss_data->param[i].reverb = GetU16( p_data, offset + 6 );
	} 


	// DEBUG:
  /*
	OBATA_Printf( "------------------------LoadIssData\n" );
	OBATA_Printf( "data_num = %d\n", data_num );
	for( i=0; i<data_num; i++ )
	{ 
		OBATA_Printf( "data[%d].zone_id = %d\n", i, iss_data->param[i].zoneID ); 
		OBATA_Printf( "data[%d].pitch   = %d\n", i, iss_data->param[i].pitch ); 
		OBATA_Printf( "data[%d].tempo   = %d\n", i, iss_data->param[i].tempo ); 
		OBATA_Printf( "data[%d].reverb  = %d\n", i, iss_data->param[i].reverb ); 
	}
  */

	// �ǂݍ��񂾃f�[�^��Ԃ�
	return iss_data;
}

//----------------------------------------------------------------------------
/**
 * @brief �f�[�^��j������
 *
 * @param p_data �j������f�[�^
 */
//----------------------------------------------------------------------------
static void UnloadIssData( ISS_DATA* p_data )
{ 
	// �p�����[�^�z���j��
	GFL_HEAP_FreeMemory( p_data->param );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_data );
}

//----------------------------------------------------------------------------
/**
 * @brief �w��]�[��ID��BGM�p�����[�^���擾����
 *
 * @param p_data  �������f�[�^
 * @param zone_id �����Ώۃ]�[��ID
 *
 * @return �w��]�[��ID�̃p�����[�^(���݂��Ȃ��ꍇ, NULL)
 */
//----------------------------------------------------------------------------
static const BGM_PARAM* GetBGMParam( const ISS_DATA* p_data, u16 zone_id )
{
	int i;

	// �w��]�[��ID������
	for( i=0; i<p_data->dataNum; i++ )
	{
		// ����
		if( p_data->param[i].zoneID == zone_id )
		{
			return &p_data->param[i];
		}
	}

	// �����ł���
	return NULL;
}


//=====================================================================================================
/**
 * @breif �_���W����ISS�V�X�e���\����
 */
//=====================================================================================================
struct _ISS_DUNGEON_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

	// �N�����
	BOOL isActive; 

	// �]�[��ID
	u16 currentZoneID;
	u16 nextZoneID;

	// �f�[�^
	ISS_DATA* pData;

	// ���݂̐ݒ�f�[�^
	const BGM_PARAM* pActiveParam;
};


//=====================================================================================================
/**
 * ���J�֐��̎���
 */
//===================================================================================================== 

//----------------------------------------------------------------------------
/**
 * @brief  �_���W����ISS�V�X�e�����쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return �_���W����ISS�V�X�e��
 */
//----------------------------------------------------------------------------
ISS_DUNGEON_SYS* ISS_DUNGEON_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_DUNGEON_SYS* p_sys;

	// �������m��
	p_sys = (ISS_DUNGEON_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_DUNGEON_SYS ) );

	// ������
	p_sys->heapID        = heap_id;
	p_sys->pPlayer       = p_player;
	p_sys->isActive      = FALSE;
	p_sys->currentZoneID = INVALID_ZONE_ID;
	p_sys->nextZoneID    = INVALID_ZONE_ID;
	p_sys->pData         = LoadIssData( heap_id );
	p_sys->pActiveParam  = NULL;

	// �쐬�����_���W����ISS�V�X�e����Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  �_���W����ISS�V�X�e����j������
 *
 * @param p_sys �j������_���W����ISS�V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Delete( ISS_DUNGEON_SYS* p_sys )
{
	// �V�X�e����~
	ISS_DUNGEON_SYS_Off( p_sys );
	
	// �f�[�^��j��
	UnloadIssData( p_sys->pData );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_sys );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_sys ����Ώۂ̃_���W����ISS�V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Update( ISS_DUNGEON_SYS* p_sys )
{
	// �N�����Ă��Ȃ����, �������Ȃ�
	if( p_sys->isActive != TRUE ) return; 

	// �]�[���؂�ւ����ʒm���ꂽ�ꍇ
	if( p_sys->currentZoneID != p_sys->nextZoneID )
	{
		// �V�]�[��ID��BGM�p�����[�^������
		p_sys->pActiveParam = GetBGMParam( p_sys->pData, p_sys->nextZoneID );

		// BGM�̐ݒ�𔽉f������
		SetBGMStatus( p_sys->pActiveParam );

		// �X�V
		p_sys->currentZoneID = p_sys->nextZoneID;
	}

	// DEBUG:
	OBATA_Printf( "Dungeon ISS is ON\n" );
} 

//---------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_sys        �ʒm�Ώۂ̃_���W����ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//---------------------------------------------------------------------------
void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* p_sys, u16 next_zone_id )
{ 
	p_sys->nextZoneID = next_zone_id; 
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param p_sys �N������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_On( ISS_DUNGEON_SYS* p_sys )
{
	p_sys->isActive = TRUE;
	PMSND_EnableCaptureReverb( 0, REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );
	SetBGMStatus( p_sys->pActiveParam );
}

//----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param p_sys ��~������V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Off( ISS_DUNGEON_SYS* p_sys )
{
	// ��~��, �p�����[�^���f�t�H���g�ɖ߂�
	p_sys->isActive = FALSE;
	PMSND_SetStatusBGM( 256, 0, 0 ); 
	PMSND_DisableCaptureReverb();
}

//----------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param p_sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//----------------------------------------------------------------------------
BOOL ISS_DUNGEON_SYS_IsOn( const ISS_DUNGEON_SYS* p_sys )
{
	return p_sys->isActive;
}
