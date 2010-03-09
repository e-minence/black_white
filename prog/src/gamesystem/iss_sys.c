////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_sys.h
 * @brief  ISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////
#include "iss_sys.h"
#include "iss_city_sys.h"
#include "iss_road_sys.h"
#include "iss_dungeon_sys.h"
#include "iss_zone_sys.h"
#include "iss_switch_sys.h"
#include "iss_3ds_sys.h"
#include "../field/field_sound.h"
#include "sound/bgm_info.h"
#include "../../../resource/sound/bgm_info/iss_type.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"
#include "sound/pm_sndsys.h" 


//==================================================================================
// ���萔
//==================================================================================
#define ISS_ON                       // ISS����X�C�b�`
#define INVALID_BGM_NO (0xffffffff)  // �Ď�BGM�ԍ��̖����l


//==================================================================================
// ��ISS�V�X�e�����[�N
//==================================================================================
struct _ISS_SYS
{
	HEAPID    heapID;     // �g�p����q�[�vID 
	GAMEDATA* gameData;   // �Q�[���f�[�^

	u32 soundIdx;  // �Ď�����BGM�ԍ�
	u32 frame;     // �t���[���J�E���^

	// �eISS�V�X�e��
	ISS_CITY_SYS*    issC;	// �X
	ISS_ROAD_SYS*    issR;	// ���H
	ISS_DUNGEON_SYS* issD;	// �_���W���� 
  ISS_ZONE_SYS*    issZ;  // �]�[��
  ISS_SWITCH_SYS*  issS;  // �X�C�b�`
  ISS_3DS_SYS*     issB;  // ��
};


//==================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==================================================================================
static void BGMChangeCheck( ISS_SYS* sys );


//==================================================================================
// ���O�����J�֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief ISS�V�X�e�����쐬����
 *
 * @param gameData
 * @param heapID   �g�p����q�[�vID
 * 
 * @return ISS�V�X�e��
 */
//----------------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* gameData, HEAPID heapID )
{
	ISS_SYS* sys;
	PLAYER_WORK* player;

  GF_ASSERT( gameData );

	player = GAMEDATA_GetMyPlayerWork( gameData );

	// �V�X�e�����[�N���m��
	sys = (ISS_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SYS) );

	// �����ݒ�
	sys->heapID   = heapID;
	sys->gameData = gameData;
	sys->issC     = ISS_CITY_SYS_Create( player, heapID );
	sys->issR     = ISS_ROAD_SYS_Create( player, heapID );
	sys->issD     = ISS_DUNGEON_SYS_Create( gameData, player, heapID );
  sys->issZ     = ISS_ZONE_SYS_Create( heapID );
  sys->issS     = ISS_SWITCH_SYS_Create( heapID );
  sys->issB     = ISS_3DS_SYS_Create( heapID );
	sys->soundIdx = INVALID_BGM_NO;
	sys->frame    = 0;

	return sys;
}

//----------------------------------------------------------------------------------
/**
 * @brief ISS�V�X�e����j������
 *
 * @param sys
 */
//----------------------------------------------------------------------------------
void ISS_SYS_Delete( ISS_SYS* sys )
{
  GF_ASSERT( sys );

	// �eISS�V�X�e����j��
	ISS_CITY_SYS_Delete( sys->issC );
	ISS_ROAD_SYS_Delete( sys->issR );
	ISS_DUNGEON_SYS_Delete( sys->issD );
  ISS_ZONE_SYS_Delete( sys->issZ );
  ISS_SWITCH_SYS_Delete( sys->issS );
  ISS_3DS_SYS_Delete( sys->issB );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( sys );

#ifdef ISS_ON
	// 9, 10�g���b�N�̉��ʂ����ɖ߂�
  PMSND_ChangeBGMVolume( (1<<8)|(1<<9), 127 );
#endif
}

//----------------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param sys
 */
//----------------------------------------------------------------------------------
void ISS_SYS_Update( ISS_SYS* sys )
{ 
#ifdef ISS_ON
	// BGM�ύX�`�F�b�N
	BGMChangeCheck( sys );

  // �eISS�̍X�V
	if( sys->frame++ % 2 == 0 ) {
    ISS_ROAD_SYS_Update( sys->issR );  // ���HISS (30�t���[���œ���)
    ISS_CITY_SYS_Main( sys->issC );    // �XISS (30�t���[���œ���)
    ISS_3DS_SYS_Main( sys->issB );     // ��ISS (30�t���[���œ���)
  }
	ISS_DUNGEON_SYS_Update( sys->issD ); // �_���W����ISS 
  ISS_ZONE_SYS_Update( sys->issZ );    // �]�[��ISS 
  ISS_SWITCH_SYS_Update( sys->issS );  // �X�C�b�`ISS
#endif
}
	

//----------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys
 * @param nextZoneID �V�����]�[��ID
 */
//----------------------------------------------------------------------------------
void ISS_SYS_ZoneChange( ISS_SYS* sys, u16 nextZoneID )
{
#ifdef ISS_ON
	PLAYER_WORK* player;
	PLAYER_MOVE_FORM form;

	// ���@�̏�Ԃ��擾
	player = GAMEDATA_GetMyPlayerWork( sys->gameData );
	form   = PLAYERWORK_GetMoveForm( player ); 
	// ���]�Ԃɏ���Ă��� ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_CYCLE ) { return; }
  // �Ȃ݂̂蒆 ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_SWIM ) { return; }

  // �eISS�V�X�e���Ƀ]�[���`�F���W��ʒm
	ISS_CITY_SYS_ZoneChange( sys->issC, nextZoneID );    // �XISS 
	ISS_DUNGEON_SYS_ZoneChange( sys->issD, nextZoneID ); // �_���W����ISS 
  ISS_SWITCH_SYS_ZoneChange( sys->issS, nextZoneID );  // �X�C�b�`ISS
  ISS_ZONE_SYS_ZoneChange( sys->issZ, nextZoneID );    // �]�[��ISS
  ISS_3DS_SYS_ZoneChange( sys->issB );                 // ��ISS
#endif
}

//----------------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�V�X�e�����擾����
 *
 * @param sys �擾�ΏۃV�X�e��
 *
 * @return �X�C�b�`ISS�V�X�e��
 */
//----------------------------------------------------------------------------------
ISS_SWITCH_SYS* ISS_SYS_GetIssSwitchSystem( const ISS_SYS* sys )
{
  return sys->issS;
}

//------------------------------------------------------------------------------------
/**
 * @brief 3D ISS�V�X�e�����擾����
 *
 * @param sys �擾�ΏۃV�X�e��
 *
 * @return 3D ISS�V�X�e��
 */
//------------------------------------------------------------------------------------
ISS_3DS_SYS* ISS_SYS_GetIss3DSSystem( const ISS_SYS* sys )
{
  return sys->issB;
}


//==================================================================================
// ������J�֐�
//==================================================================================

//----------------------------------------------------------------------------------
/**
 * @brief BGM�̕ω����Ď�����
 *
 * @param �������V�X�e��
 */
//----------------------------------------------------------------------------------
static void BGMChangeCheck( ISS_SYS* sys )
{
#ifdef ISS_ON
	u32 soundIdx, issType;
	BGM_INFO_SYS* bgmInfoSys = GAMEDATA_GetBGMInfoSys( sys->gameData );
	PLAYER_WORK*  player     = GAMEDATA_GetMyPlayerWork( sys->gameData );
  u16           zoneID     = PLAYERWORK_getZoneID( player );

	// �Đ�����BGM��ISS�^�C�v���擾
	soundIdx = PMSND_GetBGMsoundNo();
	issType  = BGM_INFO_GetIssType( bgmInfoSys, soundIdx ); 

  // BGM�ɕω����Ȃ���Ή������Ȃ�
  if( sys->soundIdx == soundIdx ){ return; }

  // �eISS�V�X�e���̋N����Ԃ�ύX
  switch( issType )
  {
  case ISS_TYPE_LOAD:  // ���H
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_ROAD_SYS_On( sys->issR );
      ISS_3DS_SYS_Off( sys->issB );
    break;
  case ISS_TYPE_CITY:  // �X
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_CITY_SYS_On( sys->issC );
      ISS_3DS_SYS_Off( sys->issB );
    break;
  case ISS_TYPE_DUNGEON:  // �_���W����
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_DUNGEON_SYS_On( sys->issD );
      ISS_3DS_SYS_Off( sys->issB );
    break;
  case ISS_TYPE_ZONE:  // �]�[��
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_ZONE_SYS_On( sys->issZ, zoneID );
      ISS_3DS_SYS_Off( sys->issB );
      break;
  case ISS_TYPE_SWITCH:  // �X�C�b�`
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_On( sys->issS );
      ISS_3DS_SYS_Off( sys->issB );
    break;
  case ISS_TYPE_BRIDGE:  // ��
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_3DS_SYS_On( sys->issB );
      break;
  default:  // ���̑�
      ISS_ROAD_SYS_Off( sys->issR );
      ISS_CITY_SYS_Off( sys->issC );
      ISS_DUNGEON_SYS_Off( sys->issD );
      ISS_ZONE_SYS_Off( sys->issZ );
      ISS_SWITCH_SYS_Off( sys->issS );
      ISS_3DS_SYS_Off( sys->issB );
    break;
  } 

  // BGM�ԍ����L��
  sys->soundIdx = soundIdx;
#endif
} 
