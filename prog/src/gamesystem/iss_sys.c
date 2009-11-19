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
#include "iss_zone_sys.h"
#include "iss_switch_sys.h"
#include "../field/field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h" 


//=========================================================================================
// ���萔
//=========================================================================================
#define INVALID_BGM_NO (0xffff)  // �Đ���BGM�ԍ��̖����l


//=========================================================================================
// ��ISS�V�X�e�����[�N
//=========================================================================================
struct _ISS_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώ�
	GAMEDATA* gdata;  // �Q�[���f�[�^

	// �eISS�V�X�e��
	ISS_CITY_SYS*    issC;	// �X
	ISS_ROAD_SYS*    issR;	// ���H
	ISS_DUNGEON_SYS* issD;	// �_���W���� 
  ISS_ZONE_SYS*    issZ;  // �]�[��
  ISS_SWITCH_SYS*  issS;  // �X�C�b�`

	// �Đ�����BGM�ԍ�
	u16 bgmNo; 

	// �t���[���J�E���^
	u32 frame;
};


//=========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//========================================================================================= 
static void BGMChangeCheck( ISS_SYS* sys );


//=========================================================================================
// ���O�����J�֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief  ISS�V�X�e�����쐬����
 *
 * @param gdata	  �Ď��ΏۃQ�[���f�[�^
 * @param heap_id �g�p����q�[�vID
 * 
 * @return ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* gdata, HEAPID heap_id )
{
	ISS_SYS* sys;
	PLAYER_WORK* player;

	// �Ď��Ώۂ̎�l�����擾
	player = GAMEDATA_GetMyPlayerWork( gdata );

	// �������m��
	sys = (ISS_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_SYS ) );

	// �����ݒ�
	sys->heapID  = heap_id;
	sys->gdata   = gdata;
	sys->issC    = ISS_CITY_SYS_Create( player, heap_id );
	sys->issR    = ISS_ROAD_SYS_Create( player, heap_id );
	sys->issD    = ISS_DUNGEON_SYS_Create( gdata, player, heap_id );
  sys->issZ    = ISS_ZONE_SYS_Create( heap_id );
  sys->issS    = ISS_SWITCH_SYS_Create( heap_id );
	sys->bgmNo   = INVALID_BGM_NO;
	sys->frame   = 0;

	// �쐬����ISS�V�X�e����Ԃ�
	return sys;
}

//-----------------------------------------------------------------------------------------
/**
 * @brief  ISS�V�X�e����j������
 * * @param sys �j������ISS�V�X�e�� 
 */
//-----------------------------------------------------------------------------------------
void ISS_SYS_Delete( ISS_SYS* sys )
{
	// �eISS�V�X�e����j��
	ISS_CITY_SYS_Delete( sys->issC );
	ISS_ROAD_SYS_Delete( sys->issR );
	ISS_DUNGEON_SYS_Delete( sys->issD );
  ISS_ZONE_SYS_Delete( sys->issZ );
  ISS_SWITCH_SYS_Delete( sys->issS );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( sys );

	// 9, 10�g���b�N�̉��ʂ����ɖ߂�
  PMSND_ChangeBGMVolume( (1<<8)|(1<<9), 127 );
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param sys ����Ώۂ̃V�X�e��
 */
//-----------------------------------------------------------------------------------------
void ISS_SYS_Update( ISS_SYS* sys )
{ 
	// BGM�ύX�`�F�b�N
	BGMChangeCheck( sys );

  // �eISS�̍X�V
	ISS_CITY_SYS_Update( sys->issC );    // �XISS 
	if( sys->frame++ % 2 == 0 )
  {
    ISS_ROAD_SYS_Update( sys->issR );  // ���HISS(30�t���[���œ���)
  }
	ISS_DUNGEON_SYS_Update( sys->issD ); // �_���W����ISS 
  ISS_ZONE_SYS_Update( sys->issZ );    // �]�[��ISS 
  ISS_SWITCH_SYS_Update( sys->issS );  // �X�C�b�`ISS
}
	

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys       �ʒm�Ώۂ�ISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//-----------------------------------------------------------------------------------------
void ISS_SYS_ZoneChange( ISS_SYS* sys, u16 next_zone_id )
{
	PLAYER_WORK* player;
	PLAYER_MOVE_FORM form;

	// ���@�̏�Ԃ��擾
	player = GAMEDATA_GetMyPlayerWork( sys->gdata );
	form   = PLAYERWORK_GetMoveForm( player ); 
	// ���]�Ԃɏ���Ă��� ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_CYCLE ) return;
  // �Ȃ݂̂蒆 ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_SWIM ) return;

  // �eISS�V�X�e���Ƀ]�[���`�F���W��ʒm
	ISS_CITY_SYS_ZoneChange( sys->issC, next_zone_id );    // �XISS 
	ISS_DUNGEON_SYS_ZoneChange( sys->issD, next_zone_id ); // �_���W����ISS 
  ISS_ZONE_SYS_ZoneChange( sys->issZ, next_zone_id );    // �]�[��ISS
}

//-----------------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�V�X�e�����擾����
 *
 * @param sys �擾�ΏۃV�X�e��
 *
 * @return �X�C�b�`ISS�V�X�e��
 */
//-----------------------------------------------------------------------------------------
ISS_SWITCH_SYS* ISS_SYS_GetIssSwitchSystem( const ISS_SYS* sys )
{
  return sys->issS;
}


//=========================================================================================
// ������J�֐�
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief BGM�̕ω����Ď�����
 *
 * @param �������V�X�e��
 */
//-----------------------------------------------------------------------------------------
static void BGMChangeCheck( ISS_SYS* sys )
{
	int bgm_no, iss_type;
	BGM_INFO_SYS* bgm_info_sys = GAMEDATA_GetBGMInfoSys( sys->gdata );
	PLAYER_WORK*        player = GAMEDATA_GetMyPlayerWork( sys->gdata );
  u16                zone_id = PLAYERWORK_getZoneID( player );

	// �Đ�����BGM��ISS�^�C�v���擾
	bgm_no   = PMSND_GetBGMsoundNo();
	iss_type = BGM_INFO_GetIssType( bgm_info_sys, bgm_no ); 

  // BGM�ɕω����Ȃ���Ή������Ȃ�
  if( sys->bgmNo == bgm_no ) return;

  // �eISS�V�X�e���̋N����Ԃ�ύX
  switch( iss_type )
  {
  case ISS_TYPE_LOAD:  // ���H
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_ROAD_SYS_On( sys->issR );
    break;
  case ISS_TYPE_CITY:  // �X
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_CITY_SYS_On( sys->issC );
    break;
  case ISS_TYPE_DUNGEON:  // �_���W����
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_DUNGEON_SYS_On( sys->issD );
    break;
  case ISS_TYPE_ZONE:  // �]�[��
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_ZONE_SYS_On( sys->issZ, zone_id );
      break;
  case ISS_TYPE_SWITCH:  // �X�C�b�`
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_On( sys->issS );
    break;
  default:  // ���̑�
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
    break;
  } 

  // BGM�ԍ����L��
  sys->bgmNo = bgm_no;
} 
