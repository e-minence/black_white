///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_unit.h
 * @brief  ISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.07.16
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include "iss_unit.h"
#include "iss_unit_city.h"
#include "iss_unit_load.h"
#include "field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"


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
struct _ISS_UNIT
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��ΏۃQ�[���f�[�^
	GAMEDATA* pGameData;

	// ���]�ԏ�Ԃ��ǂ���
	BOOL cycle;

	// �Ȃ݂̂��Ԃ��ǂ���
	BOOL surfing;

	// �eISS���j�b�g
	ISS_UNIT_CITY* pCityUnit;	// �X
	ISS_UNIT_LOAD* pLoadUnit;	// ���H
};


//=========================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//=========================================================================================

// ��l���̎��]�ԏ�Ԃ��Ď�����
void CycleCheck( ISS_UNIT* p_sys );

// ��l���̂Ȃ݂̂��Ԃ��Ď�����
void SurfingCheck( ISS_UNIT* p_sys );


//=========================================================================================
/**
 * @brief ���J�֐��̎���
 */
//=========================================================================================

//----------------------------------------------------------------------------
/**
 * @brief  ISS���j�b�g���쐬����
 *
 * @param p_gdata	�Ď��ΏۃQ�[���f�[�^
 * @param zone_id   �]�[��ID
 * @param heap_id   �g�p����q�[�vID
 * 
 * @return ISS���j�b�g
 */
//----------------------------------------------------------------------------
ISS_UNIT* ISS_UNIT_Create( GAMEDATA* p_gdata, u16 zone_id, HEAPID heap_id )
{
	ISS_UNIT* p_sys;
	PLAYER_WORK* p_player;

	// �Ď��Ώۂ̎�l�����擾
	p_player = GAMEDATA_GetMyPlayerWork( p_gdata );

	// �������m��
	p_sys = (ISS_UNIT*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_UNIT ) );

	// �����ݒ�
	p_sys->heapID      = heap_id;
	p_sys->pGameData   = p_gdata;
	p_sys->cycle       = FALSE;
	p_sys->surfing     = FALSE;
	p_sys->pCityUnit   = ISS_UNIT_CITY_Create( p_player, zone_id, heap_id );
	p_sys->pLoadUnit   = ISS_UNIT_LOAD_Create( p_player, zone_id, heap_id );
	ISS_UNIT_ZoneChange( p_sys, zone_id );
	
	// �쐬����ISS���j�b�g��Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief  ISS���j�b�g��j������
 * * @param p_unit �j������ISS���j�b�g 
 */
//----------------------------------------------------------------------------
void ISS_UNIT_Delete( ISS_UNIT* p_unit )
{
	// �eISS���j�b�g��j��
	ISS_UNIT_CITY_Delete( p_unit->pCityUnit );
	ISS_UNIT_LOAD_Delete( p_unit->pLoadUnit );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_unit );
}

//----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param p_unit ����Ώۂ̃��j�b�g
 */
//----------------------------------------------------------------------------
void ISS_UNIT_Update( ISS_UNIT* p_unit )
{
	// ���]�ԃ`�F�b�N
	CycleCheck( p_unit );

	// �Ȃ݂̂�`�F�b�N
	SurfingCheck( p_unit );

	// �XISS
	ISS_UNIT_CITY_Update( p_unit->pCityUnit );

	// ���HISS
	ISS_UNIT_LOAD_Update( p_unit->pLoadUnit );
}
	

//---------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param p_unit       �ʒm�Ώۂ�ISS���j�b�g
 * @param next_zone_id �V�����]�[��ID
 */
//---------------------------------------------------------------------------
void ISS_UNIT_ZoneChange( ISS_UNIT* p_unit, u16 next_zone_id )
{
	PLAYER_WORK*     p_player;
	PLAYER_MOVE_FORM form;
	BGM_INFO_SYS*    p_bgm_info_sys;
	int              bgm_index;
	int              iss_type;

	// ���]��or�Ȃ݂̂蒆�Ȃ�, ISS�ɕύX�͂Ȃ�
	if( p_unit->cycle | p_unit->surfing ) return;

	// �e�I�u�W�F�N�g���擾
	p_player       = GAMEDATA_GetMyPlayerWork( p_unit->pGameData );
	form           = PLAYERWORK_GetMoveForm( p_player );
	p_bgm_info_sys = GAMEDATA_GetBGMInfoSys( p_unit->pGameData );

	// �؂�ւ���]�[����ISS�^�C�v���擾
	bgm_index = FIELD_SOUND_GetFieldBGMNo( p_unit->pGameData, form, next_zone_id );
	iss_type  = BGM_INFO_GetIssType( p_bgm_info_sys, bgm_index );

	// �XISS
	ISS_UNIT_CITY_ZoneChange( p_unit->pCityUnit, next_zone_id );

	// ���HISS
	if( iss_type == ISS_TYPE_LOAD )
	{
		ISS_UNIT_LOAD_SetActive( p_unit->pLoadUnit, TRUE );
	}
	else
	{
		ISS_UNIT_LOAD_SetActive( p_unit->pLoadUnit, FALSE );
	}

	// DEBUG:
	OBATA_Printf( "ISS_UNIT_ZoneChange()\n" );
	OBATA_Printf( "seq = %d, iss_type = %d\n", bgm_index, iss_type );
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
void CycleCheck( ISS_UNIT* p_sys )
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
		ISS_UNIT_CITY_SetActive( p_sys->pCityUnit, FALSE );
		ISS_UNIT_LOAD_SetActive( p_sys->pLoadUnit, FALSE );
	}

	// ���]�Ԃ���~���̂����o������, �]�[���؂�ւ����Ɠ�������
	if( ( p_sys->cycle == TRUE ) && ( form != PLAYER_MOVE_FORM_CYCLE ) )
	{
		p_sys->cycle = FALSE;
		ISS_UNIT_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
}

//----------------------------------------------------------------------------
/**
 * @brief ��l���̂Ȃ݂̂��Ԃ��Ď�����
 */
//----------------------------------------------------------------------------
void SurfingCheck( ISS_UNIT* p_sys )
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
		ISS_UNIT_CITY_SetActive( p_sys->pCityUnit, FALSE );
		ISS_UNIT_LOAD_SetActive( p_sys->pLoadUnit, FALSE );
	}

	// ���]�Ԃ���~���̂����o������, �]�[���؂�ւ����Ɠ�������
	if( ( p_sys->surfing == TRUE ) && ( form != PLAYER_MOVE_FORM_SWIM ) )
	{
		p_sys->surfing = FALSE;
		ISS_UNIT_ZoneChange( p_sys, PLAYERWORK_getZoneID( p_player ) );
	}
}
