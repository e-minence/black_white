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
#include "iss_dungeon_sys.h"
#include "../field/field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h" 


//=========================================================================================
/**
 * @brief �萔�E�}�N��
 */
//=========================================================================================

// �Đ���BGM�ԍ��̖����l
#define INVALID_BGM_NO (0xffff)


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
	ISS_CITY_SYS*    issC;	// �X
	ISS_ROAD_SYS*    issR;	// ���H
	ISS_DUNGEON_SYS* issD;	// �_���W���� 

	// �Đ�����BGM�ԍ�
	u16 bgmNo; 

	// �t���[���J�E���^
	u32 frame;
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

// BGM�̕ω����Ď�����
void BGMChangeCheck( ISS_SYS* p_sys );


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
 * @param heap_id   �g�p����q�[�vID
 * 
 * @return ISS�V�X�e��
 */
//----------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* p_gdata, HEAPID heap_id )
{
	ISS_SYS* p_sys;
	PLAYER_WORK* p_player;

	// �Ď��Ώۂ̎�l�����擾
	p_player = GAMEDATA_GetMyPlayerWork( p_gdata );

	// �������m��
	p_sys = (ISS_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_SYS ) );

	// �����ݒ�
	p_sys->heapID         = heap_id;
	p_sys->pGameData      = p_gdata;
	p_sys->cycle          = FALSE;
	p_sys->surfing        = FALSE;
	p_sys->issC    = ISS_CITY_SYS_Create( p_player, heap_id );
	p_sys->issR    = ISS_ROAD_SYS_Create( p_player, heap_id );
	p_sys->issD = ISS_DUNGEON_SYS_Create( p_player, heap_id );
	p_sys->bgmNo          = INVALID_BGM_NO;
	p_sys->frame          = 0;


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
	ISS_CITY_SYS_Delete( p_sys->issC );
	ISS_ROAD_SYS_Delete( p_sys->issR );
	ISS_DUNGEON_SYS_Delete( p_sys->issD );

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
	p_sys->frame++;

	// ���]�ԃ`�F�b�N
	CycleCheck( p_sys );

	// �Ȃ݂̂�`�F�b�N
	SurfingCheck( p_sys );

	// BGM�ύX�`�F�b�N
	BGMChangeCheck( p_sys );

	// �XISS
	ISS_CITY_SYS_Update( p_sys->issC );

	// ���HISS(��l����30fps�œ��삷�邽��, ���HISS������ɍ��킹��)
	if( p_sys->frame % 2 == 0 ) ISS_ROAD_SYS_Update( p_sys->issR ); 
	
	// �_���W����ISS
	ISS_DUNGEON_SYS_Update( p_sys->issD );
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

	// ���]��or�Ȃ݂̂蒆�Ȃ�, ISS�ɕύX�͂Ȃ�
	if( p_sys->cycle | p_sys->surfing ) return;

	// �XISS
	ISS_CITY_SYS_ZoneChange( p_sys->issC, next_zone_id );

	// �_���W����ISS
	ISS_DUNGEON_SYS_ZoneChange( p_sys->issD, next_zone_id );

	// DEBUG:
	//OBATA_Printf( "ISS_SYS_ZoneChange()\n" );
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
		ISS_CITY_SYS_Off( p_sys->issC );
		ISS_ROAD_SYS_Off( p_sys->issR );
		ISS_DUNGEON_SYS_Off( p_sys->issD );
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
		ISS_CITY_SYS_Off( p_sys->issC );
		ISS_ROAD_SYS_Off( p_sys->issR );
		ISS_DUNGEON_SYS_Off( p_sys->issD );
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
 * @brief BGM�̕ω����Ď�����
 *
 * @param �������V�X�e��
 */
//----------------------------------------------------------------------------
void BGMChangeCheck( ISS_SYS* p_sys )
{
	PLAYER_WORK*  p_player       = GAMEDATA_GetMyPlayerWork( p_sys->pGameData );
	BGM_INFO_SYS* p_bgm_info_sys = GAMEDATA_GetBGMInfoSys( p_sys->pGameData );
	int bgm_no, iss_type;

	// ���ɍĐ��\���BGM��ISS�^�C�v���擾
	// (����BGM���w�肳��Ă��Ȃ��ꍇ, ���ݍĐ�����BGM��ISS�^�C�v���擾����)
	bgm_no   = PMSND_GetBGMsoundNo();
	iss_type = BGM_INFO_GetIssType( p_bgm_info_sys, bgm_no ); 

  // BGM�ɕω����Ȃ���Ή������Ȃ�
  if( p_sys->bgmNo == bgm_no ) return;

  // �eISS�V�X�e���̋N����Ԃ�ύX
  switch( iss_type )
  {
  case ISS_TYPE_LOAD:  // ���H
      ISS_ROAD_SYS_On( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  case ISS_TYPE_CITY:  // �X
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_On( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  case ISS_TYPE_DUNGEON:  // �_���W����
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_On( p_sys->issD );
    break;
  default:  // ���̑�
      ISS_ROAD_SYS_Off( p_sys->issR );
      ISS_CITY_SYS_Off( p_sys->issC );
      ISS_DUNGEON_SYS_Off( p_sys->issD );
    break;
  } 

  // BGM�ԍ����L��
  p_sys->bgmNo = bgm_no;
}
