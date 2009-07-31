///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_sys.h
 * @brief  ISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include "iss_sys.h"
#include "iss_city_sys.h"
#include "iss_road_sys.h"
#include "field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h" // TEST:


//=========================================================================================
/**
 * @brief �萔�E�}�N��
 */
//=========================================================================================


//=========================================================================================
/**
 * @brief ISS�V�X�e�����[�N
 */
//=========================================================================================
struct _ISS_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��ΏۃQ�[���f�[�^
	GAMEDATA* pGameData;

	// ���]�ԏ�Ԃ��ǂ���
	BOOL cycle;

	// �Ȃ݂̂��Ԃ��ǂ���
	BOOL surfing;

	// �eISS�V�X�e��
	ISS_CITY_SYS* pIssCitySys;	// �X
	ISS_ROAD_SYS* pIssRoadSys;	// ���H

	// �Đ�����BGM�ԍ�
	int bgmNo; 
};


//=========================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//=========================================================================================

// ��l���̎��]�ԏ�Ԃ��Ď�����
void CycleCheck( ISS_SYS* p_sys );

// ��l���̂Ȃ݂̂��Ԃ��Ď�����
void SurfingCheck( ISS_SYS* p_sys );

// �퓬�G���J�E���g�̔������Ď�����
void BattleCheck( ISS_SYS* p_sys );


//=========================================================================================
/**
 * @brief ���J�֐��̎���
 */
//=========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief  ISS�V�X�e�����쐬����
 *
 * @param p_gdata	�Ď��ΏۃQ�[���f�[�^
 * @param zone_id   �]�[��ID
 * @param heap_id   �g�p����q�[�vID
 * 
 * @return ISS�V�X�e��
 */
//----------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* p_gdata, u16 zone_id, HEAPID heap_id )
{
	ISS_SYS* p_sys;
	PLAYER_WORK* p_player;

	// �Ď��Ώۂ̎�l�����擾
	p_player = GAMEDATA_GetMyPlayerWork( p_gdata );

	// �������m��
	p_sys = (ISS_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_SYS ) );

	// �����ݒ�
	p_sys->heapID      = heap_id;
	p_sys->pGameData   = p_gdata;
	p_sys->cycle       = FALSE;
	p_sys->surfing     = FALSE;
	p_sys->pIssCitySys = ISS_CITY_SYS_Create( p_player, zone_id, heap_id );
	p_sys->pIssRoadSys = ISS_ROAD_SYS_Create( p_player, zone_id, heap_id );
	p_sys->bgmNo       = 0;
	ISS_SYS_ZoneChange( p_sys, zone_id );
	
	// �쐬����ISS�V�X�e����Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  ISS�V�X�e����j������
 * * @param p_sys �j������ISS�V�X�e�� 
 */
//----------------------------------------------------------------------------
void ISS_SYS_Delete( ISS_SYS* p_sys )
{
	// �eISS�V�X�e����j��
	ISS_CITY_SYS_Delete( p_sys->pIssCitySys );
	ISS_ROAD_SYS_Delete( p_sys->pIssRoadSys );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_sys );

	// 9, 10�g���b�N�̉��ʂ����ɖ߂�
	FIELD_SOUND_ChangeBGMActionVolume( 127 );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_sys ����Ώۂ̃V�X�e��
 */
//----------------------------------------------------------------------------
void ISS_SYS_Update( ISS_SYS* p_sys )
{
	// ���]�ԃ`�F�b�N
	CycleCheck( p_sys );

	// �Ȃ݂̂�`�F�b�N
	SurfingCheck( p_sys );

	// �퓬�`�F�b�N
	BattleCheck( p_sys );

	// �XISS
	ISS_CITY_SYS_Update( p_sys->pIssCitySys );

	// ���HISS
	ISS_ROAD_SYS_Update( p_sys->pIssRoadSys ); 
}
	

//---------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_sys       �ʒm�Ώۂ�ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//---------------------------------------------------------------------------
void ISS_SYS_ZoneChange( ISS_SYS* p_sys, u16 next_zone_id )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;
	BGM_INFO_SYS*    p_bgm_info_sys;
	int              iss_type;

	// ���]��or�Ȃ݂̂蒆�Ȃ�, ISS�ɕύX�͂Ȃ�
	if( p_sys->cycle | p_sys->surfing ) return;

	// �e�I�u�W�F�N�g���擾
	p_player       = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form           = PLAYERWORK_GetMoveForm( p_player );
	p_bgm_info_sys = GAMEDATA_GetBGMInfoSys( p_sys->pGameData );

	// �؂�ւ���]�[����ISS�^�C�v���擾
	p_sys->bgmNo = FIELD_SOUND_GetFieldBGMNo( p_sys->pGameData, form, next_zone_id );
	iss_type     = BGM_INFO_GetIssType( p_bgm_info_sys, p_sys->bgmNo );

	// �XISS
	ISS_CITY_SYS_ZoneChange( p_sys->pIssCitySys, next_zone_id );

	// ���HISS
	if( iss_type == ISS_TYPE_LOAD )
	{
		ISS_ROAD_SYS_SetActive( p_sys->pIssRoadSys, TRUE );
	}
	else
	{
		ISS_ROAD_SYS_SetActive( p_sys->pIssRoadSys, FALSE );
	}

	// DEBUG:
	OBATA_Printf( "ISS_SYS_ZoneChange()\n" );
	OBATA_Printf( "seq = %d, iss_type = %d\n", p_sys->bgmNo, iss_type );
}


//=========================================================================================
/**
 * @param ����J�֐��̎���
 */
//=========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief ��l���̎��]�ԏ�Ԃ��Ď�����
 */
//----------------------------------------------------------------------------
void CycleCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;

	// ���@�̏�Ԃ��擾
	p_player = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form     = PLAYERWORK_GetMoveForm( p_player );

	// ���]�Ԃɏ��̂����o������, ISS���~
	if( ( p_sys->cycle == FALSE ) && ( form == PLAYER_MOVE_FORM_CYCLE ) )
	{
		p_sys->cycle = TRUE;
		ISS_CITY_SYS_SetActive( p_sys->pIssCitySys, FALSE );
		ISS_ROAD_SYS_SetActive( p_sys->pIssRoadSys, FALSE );
	}

	// ���]�Ԃ���~���̂����o������, �]�[���؂�ւ����Ɠ�������
	if( ( p_sys->cycle == TRUE ) && ( form != PLAYER_MOVE_FORM_CYCLE ) )
	{
		p_sys->cycle = FALSE;
		ISS_SYS_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief ��l���̂Ȃ݂̂��Ԃ��Ď�����
 */
//----------------------------------------------------------------------------
void SurfingCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;

	// ���@�̏�Ԃ��擾
	p_player = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form     = PLAYERWORK_GetMoveForm( p_player );

	// �Ȃ݂̂�J�n�����o������, ISS���~
	if( ( p_sys->surfing == FALSE ) && ( form == PLAYER_MOVE_FORM_SWIM ) )
	{
		p_sys->surfing = TRUE;
		ISS_CITY_SYS_SetActive( p_sys->pIssCitySys, FALSE );
		ISS_ROAD_SYS_SetActive( p_sys->pIssRoadSys, FALSE );
	}

	// ���]�Ԃ���~���̂����o������, �]�[���؂�ւ����Ɠ�������
	if( ( p_sys->surfing == TRUE ) && ( form != PLAYER_MOVE_FORM_SWIM ) )
	{
		p_sys->surfing = FALSE;
		ISS_SYS_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief �퓬�G���J�E���g�̔������Ď�����
 */
//----------------------------------------------------------------------------
void BattleCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;
	BGM_INFO_SYS*    p_bgm_info_sys;
	int              iss_type;

	if( !ISS_ROAD_SYS_IsActive( p_sys->pIssRoadSys ) ) return;
	
	// �e�I�u�W�F�N�g���擾
	p_player       = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	form           = PLAYERWORK_GetMoveForm( p_player );
	p_bgm_info_sys = GAMEDATA_GetBGMInfoSys( p_sys->pGameData );

	// �؂�ւ���]�[����ISS�^�C�v���擾
	p_sys->bgmNo = PMSND_GetNextBGMsoundNo();
	iss_type     = BGM_INFO_GetIssType( p_bgm_info_sys, p_sys->bgmNo ); 

	// ���HISS��BGM�łȂ�������, ���HISS���~����
	if( iss_type != ISS_TYPE_LOAD )
	{
		ISS_ROAD_SYS_SetActive( p_sys->pIssRoadSys, FALSE );
		FIELD_SOUND_ChangeBGMActionVolume( 127 );
	}

}
