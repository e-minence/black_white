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
#include "arc/iss.naix"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"
#include "../../../resource/iss/dungeon/entry_table.cdat"


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
  u32 season; // �G��
	u16 zoneID;	// �]�[��ID
	s16 pitch;	// �s�b�`(�L�[)
	u16 tempo;	// �e���|
	u16 reverb;	// ���o�[�u

} BGM_PARAM; 

//------------------------------------------------------------------------------------------
/**
 * @brief �w��p�����[�^�𔽉f������
 *
 * @param �ݒ肷��p�����[�^
 */
//------------------------------------------------------------------------------------------
static void SetBGMStatus( const BGM_PARAM* param )
{
	if( !param ) return;

  // �e���|
	PMSND_SetStatusBGM( param->tempo, PMSND_NOEFFECT, 0 );

  // �s�b�`
	NNS_SndPlayerSetTrackPitch( PMSND_GetBGMhandlePointer(), PITCH_TRACK_MASK, param->pitch );

  // ���o�[�u
  if( param->reverb == 0 )
  {
    PMSND_DisableCaptureReverb();
  }
  else
  {
    PMSND_EnableCaptureReverb( 
        param->reverb, REVERB_SAMPLE_RATE, REVERB_VOLUME, REVERB_STOP_FRAME );
  }

	// DEBUG:
	OBATA_Printf( "ISS-D: Set BGM status\n" );
	OBATA_Printf( "- pitch  = %d\n", param->pitch );
	OBATA_Printf( "- tempo  = %d\n", param->tempo );
	OBATA_Printf( "- reverb = %d\n", param->reverb );
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
	int i;
	BGM_PARAMSET* paramset;

	// �{�̂��쐬
	paramset = (BGM_PARAMSET*)GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAMSET) );

	// �f�[�^�����擾
	paramset->dataNum = NELEMS(entry_table);

	// �o�b�t�@�m��
	paramset->param = GFL_HEAP_AllocMemory( heap_id, sizeof(BGM_PARAM) * paramset->dataNum );

	// �e�f�[�^���擾
	for( i=0; i<paramset->dataNum; i++ )
	{
    GFL_ARC_LoadDataOfs( &paramset->param[i], 
        ARCID_ISS_DUNGEON, entry_table[i].datID, 0, sizeof(BGM_PARAM) );
	} 

	// DEBUG:
	OBATA_Printf( "ISS-D: Load BGM parameters\n" );
	OBATA_Printf( "- dataNum = %d\n", paramset->dataNum );
	for( i=0; i<paramset->dataNum; i++ )
	{ 
		OBATA_Printf( "- data[%d].zoneID = %d\n", i, paramset->param[i].zoneID ); 
		OBATA_Printf( "- data[%d].pitch  = %d\n", i, paramset->param[i].pitch ); 
		OBATA_Printf( "- data[%d].tempo  = %d\n", i, paramset->param[i].tempo ); 
		OBATA_Printf( "- data[%d].reverb = %d\n", i, paramset->param[i].reverb ); 
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

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

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
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return �_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
ISS_DUNGEON_SYS* ISS_DUNGEON_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_DUNGEON_SYS* p_sys;

	// �������m��
	p_sys = (ISS_DUNGEON_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof(ISS_DUNGEON_SYS) );

	// ������
	p_sys->heapID        = heap_id;
	p_sys->pPlayer       = p_player;
	p_sys->isActive      = FALSE;
	p_sys->currentZoneID = INVALID_ZONE_ID;
	p_sys->nextZoneID    = INVALID_ZONE_ID;
	p_sys->paramset      = LoadParamset( heap_id );
	p_sys->pActiveParam  = NULL;
  p_sys->defaultParam.zoneID = INVALID_ZONE_ID;
  p_sys->defaultParam.pitch  = 0;
  p_sys->defaultParam.tempo  = 256;
  p_sys->defaultParam.reverb = 0;

	// DEBUG:
	OBATA_Printf( "ISS-D: Create\n" );

	// �쐬�����_���W����ISS�V�X�e����Ԃ�
	return p_sys;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief  �_���W����ISS�V�X�e����j������
 *
 * @param p_sys �j������_���W����ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Delete( ISS_DUNGEON_SYS* p_sys )
{
	// �V�X�e����~
	ISS_DUNGEON_SYS_Off( p_sys );
	
	// �f�[�^��j��
	UnloadIssData( p_sys->paramset );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_sys );

	// DEBUG:
	OBATA_Printf( "ISS-D: Delete\n" );
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
		// �X�V
		p_sys->currentZoneID = p_sys->nextZoneID;

		// �V�]�[��ID��BGM�p�����[�^������
		p_sys->pActiveParam = GetBGMParam( p_sys->paramset, p_sys->nextZoneID );

    // BGM�p�����[�^���ݒ肳��Ă��Ȃ��ꍇ ==> �V�X�e����~
    if( p_sys->pActiveParam == NULL )
    {
      ISS_DUNGEON_SYS_Off( p_sys );
      return;
    }

		// BGM�̐ݒ�𔽉f������
		SetBGMStatus( p_sys->pActiveParam ); 
	} 
} 

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_sys        �ʒm�Ώۂ̃_���W����ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* p_sys, u16 next_zone_id )
{ 
	p_sys->nextZoneID = next_zone_id; 

  // DEBUG:
  OBATA_Printf( "ISS-D: ZoneChange\n" );
  OBATA_Printf( "- next zone id = %d\n", next_zone_id );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param p_sys �N������V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_On( ISS_DUNGEON_SYS* p_sys )
{
  // �N��
	p_sys->isActive = TRUE;

  // �p�����[�^�ݒ�
	SetBGMStatus( p_sys->pActiveParam );

  // DEBUG:
  OBATA_Printf( "ISS-D: On\n" );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param p_sys ��~������V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_DUNGEON_SYS_Off( ISS_DUNGEON_SYS* p_sys )
{
	// ��~
	p_sys->isActive = FALSE;

  // �f�t�H���g�E�p�����[�^�ɖ߂�
  SetBGMStatus( &p_sys->defaultParam );

  // DEBUG:
  OBATA_Printf( "ISS-D: Off\n" );
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
