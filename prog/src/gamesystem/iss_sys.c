///////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_sys.h
 * @brief  ISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
///////////////////////////////////////////////////////////////////////////////
#include "iss_sys.h"
#include "iss_city_sys.h"
#include "iss_road_sys.h"
#include "iss_dungeon_sys.h"
#include "iss_zone_sys.h"
#include "iss_switch_sys.h"
#include "iss_3ds_sys.h"

#include "sound/bgm_info.h"
#include "sound/pm_sndsys.h" 
#include "gamesystem/playerwork.h"
#include "gamesystem/game_data.h"

#include "../field/field_sound.h"
#include "../../../resource/sound/bgm_info/iss_type.h"


//=============================================================================
// ���萔
//=============================================================================
#define ISS_ENABLE   // ISS ����X�C�b�`
#define ISS_R_ENABLE // ISS-R ����X�C�b�`
#define ISS_C_ENABLE // ISS-C ����X�C�b�`
#define ISS_D_ENABLE // ISS-D ����X�C�b�`
#define ISS_Z_ENABLE // ISS-Z ����X�C�b�`
#define ISS_S_ENABLE // ISS-S ����X�C�b�`
#define ISS_B_ENABLE // ISS-B ����X�C�b�`
//#define PRINT_ENABLE
#define PRINT_TARGET (1) // �f�o�b�O���̏o�͐�
#define INVALID_BGM_NO (0xffffffff)  // �Ď�BGM�ԍ��̖����l


//=============================================================================
// ���֐��C���f�b�N�X
//=============================================================================
// ISS�V�X�e���̐����E�j��
static void InitISS( ISS_SYS* system ); // �V�X�e��������������
static void SetupISS( ISS_SYS* system, GAMEDATA* gameData, HEAPID heapID ); // �V�X�e�����Z�b�g�A�b�v����
static void CleanUpISS( ISS_SYS* system ); // �V�X�e�����N���[���A�b�v����
// �eISS�̐���
static void CreateISS_R( ISS_SYS* system ); // ��ISS�𐶐�����
static void CreateISS_C( ISS_SYS* system ); // �XISS�𐶐�����
static void CreateISS_D( ISS_SYS* system ); // �_���W����ISS�𐶐�����
static void CreateISS_Z( ISS_SYS* system ); // �]�[��ISS�𐶐�����
static void CreateISS_S( ISS_SYS* system ); // �X�C�b�`ISS�𐶐�����
static void CreateISS_B( ISS_SYS* system ); // ��ISS�𐶐�����
// �eISS�̔j��
static void DeleteISS_R( ISS_SYS* system ); // ��ISS��j������
static void DeleteISS_C( ISS_SYS* system ); // �XISS��j������
static void DeleteISS_D( ISS_SYS* system ); // �_���W����ISS��j������
static void DeleteISS_Z( ISS_SYS* system ); // �]�[��ISS��j������
static void DeleteISS_S( ISS_SYS* system ); // �X�C�b�`ISS��j������
static void DeleteISS_B( ISS_SYS* system ); // ��ISS��j������
// �eISS�̓���
static void MainISS_R( ISS_SYS* system ); // ��ISS�̃��C������
static void MainISS_C( ISS_SYS* system ); // �XISS�̃��C������
static void MainISS_D( ISS_SYS* system ); // �_���W����ISS�̃��C������
static void MainISS_Z( ISS_SYS* system ); // �]�[��ISS�̃��C������
static void MainISS_S( ISS_SYS* system ); // �X�C�b�`ISS�̃��C������
static void MainISS_B( ISS_SYS* system ); // ��ISS�̃��C������
// �eISS�̋N��
static void BootISS_R( ISS_SYS* system ); // ��ISS���N������
static void BootISS_C( ISS_SYS* system ); // �XISS���N������
static void BootISS_D( ISS_SYS* system ); // �_���W����ISS���N������
static void BootISS_Z( ISS_SYS* system ); // �]�[��ISS���N������
static void BootISS_S( ISS_SYS* system ); // �X�C�b�`ISS���N������
static void BootISS_B( ISS_SYS* system ); // ��ISS���N������
// �eISS�̒�~
static void StopISS_R( ISS_SYS* system ); // ��ISS���~����
static void StopISS_C( ISS_SYS* system ); // �XISS���~����
static void StopISS_D( ISS_SYS* system ); // �_���W����ISS���~����
static void StopISS_Z( ISS_SYS* system ); // �]�[��ISS���~����
static void StopISS_S( ISS_SYS* system ); // �X�C�b�`ISS���~����
static void StopISS_B( ISS_SYS* system ); // ��ISS���~����
// �]�[���̕ύX�ʒm
static void ZoneChangeISS_R( ISS_SYS* system, u16 nextZoneID ); // ��ISS�Ƀ]�[���̕ύX��ʒm����
static void ZoneChangeISS_C( ISS_SYS* system, u16 nextZoneID ); // �XISS�Ƀ]�[���̕ύX��ʒm����
static void ZoneChangeISS_D( ISS_SYS* system, u16 nextZoneID ); // �_���W����ISS�Ƀ]�[���̕ύX��ʒm����
static void ZoneChangeISS_Z( ISS_SYS* system, u16 nextZoneID ); // �]�[��ISS�Ƀ]�[���̕ύX��ʒm����
static void ZoneChangeISS_S( ISS_SYS* system, u16 nextZoneID ); // �X�C�b�`ISS�Ƀ]�[���̕ύX��ʒm����
static void ZoneChangeISS_B( ISS_SYS* system, u16 nextZoneID ); // ��ISS�Ƀ]�[���̕ύX��ʒm����
// �eISS��ON�EOFF�؂�ւ�
static PLAYER_WORK* GetPlayerWork( const ISS_SYS* system ); // ���@���擾����
static u16 GetCurrentZoneID( const ISS_SYS* system ); // ���݂̃]�[��ID���擾����
static u32 GetPlayingBGM( void ); // �Đ�����BGM���擾����
static u8 GetISSTypeOfPlayingBGM( const ISS_SYS* system ); // �Đ�����BGM��ISS�^�C�v���擾����
static BOOL CheckBGMChange( ISS_SYS* system ); // BGM�̕ω����`�F�b�N����
static void BGMChange( ISS_SYS* system ); // BGM�ύX����


//=============================================================================
// ��ISS�V�X�e�����[�N
//=============================================================================
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



//=============================================================================
// ���O�����J�֐�
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief ISS�V�X�e�����쐬����
 *
 * @param gameData
 * @param heapID   �g�p����q�[�vID
 * 
 * @return ISS�V�X�e��
 */
//-----------------------------------------------------------------------------
ISS_SYS* ISS_SYS_Create( GAMEDATA* gameData, HEAPID heapID )
{
	ISS_SYS* system;

	// �V�X�e���𐶐�
	system = (ISS_SYS*)GFL_HEAP_AllocMemory( heapID, sizeof(ISS_SYS) );

  // �V�X�e�����������E�Z�b�g�A�b�v
  InitISS( system );
  SetupISS( system, gameData, heapID );

	return system;
}

//-----------------------------------------------------------------------------
/**
 * @brief ISS�V�X�e����j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
void ISS_SYS_Delete( ISS_SYS* system )
{ 
  // �V�X�e�����N���[���A�b�v
  CleanUpISS( system );

	// �{�̂�j��
	GFL_HEAP_FreeMemory( system );
}

//-----------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
void ISS_SYS_Update( ISS_SYS* system )
{ 
#ifdef ISS_ENABLE
	// BGM���ω�
  if( CheckBGMChange( system ) == TRUE ) {
    BGMChange( system ); // BGM�ύX����
    system->soundIdx = GetPlayingBGM(); // �V����BGM�ԍ����L��
  }

  // �eISS�̍X�V
  MainISS_R( system ); // ��ISS
  MainISS_C( system ); // �XISS
  MainISS_D( system ); // �_���W����ISS
  MainISS_Z( system ); // �]�[��ISS
  MainISS_S( system ); // �X�C�b�`ISS
  MainISS_B( system ); // ��ISS

  // �t���[���J�E���^���X�V
  system->frame++;
#endif
}
	

//-----------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param system
 * @param nextZoneID �V�����]�[��ID
 */
//-----------------------------------------------------------------------------
void ISS_SYS_ZoneChange( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_ENABLE
	PLAYER_MOVE_FORM form;

  // ���@�̃t�H�[�����擾
	form = PLAYERWORK_GetMoveForm( GetPlayerWork(system) ); 

	// ���]�Ԃɏ���Ă��� ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_CYCLE ) { return; }

  // �Ȃ݂̂蒆 ==> ISS�ύX����
	if( form == PLAYER_MOVE_FORM_SWIM ) { return; }

  // �eISS�V�X�e���Ƀ]�[���̕ύX��ʒm
  ZoneChangeISS_R( system, nextZoneID ); // ��ISS
  ZoneChangeISS_C( system, nextZoneID ); // �XISS
  ZoneChangeISS_D( system, nextZoneID ); // �_���W����ISS
  ZoneChangeISS_Z( system, nextZoneID ); // �]�[��ISS
  ZoneChangeISS_S( system, nextZoneID ); // �X�C�b�`ISS
  ZoneChangeISS_B( system, nextZoneID ); // ��ISS
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�V�X�e�����擾����
 *
 * @param system �擾�ΏۃV�X�e��
 *
 * @return �X�C�b�`ISS�V�X�e��
 */
//-----------------------------------------------------------------------------
ISS_SWITCH_SYS* ISS_SYS_GetIssSwitchSystem( const ISS_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  return system->issS;
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�V�X�e�����擾����
 *
 * @param system �擾�ΏۃV�X�e��
 *
 * @return ��ISS�V�X�e��
 */
//-----------------------------------------------------------------------------
ISS_3DS_SYS* ISS_SYS_GetIss3DSSystem( const ISS_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  return system->issB;
}

//------------------------------------------------------------------------------------
/**
 * @brief �_���W���� ISS�V�X�e�����擾����
 *
 * @param sys �擾�ΏۃV�X�e��
 *
 * @return �_���W���� ISS�V�X�e��
 */
//------------------------------------------------------------------------------------
ISS_DUNGEON_SYS* ISS_SYS_GetIssDungeonSystem( const ISS_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

  return system->issD;
}


//=============================================================================
// ������J�֐�
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �V�X�e��������������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void InitISS( ISS_SYS* system )
{
  system->heapID   = 0;
  system->gameData = NULL;
  system->soundIdx = 0;
  system->frame    = 0;
  system->issC     = NULL;
  system->issR     = NULL;
  system->issD     = NULL;
  system->issZ     = NULL;
  system->issS     = NULL;
  system->issB     = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: init ISS\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����Z�b�g�A�b�v����
 *
 * @param system
 * @param gameData
 * @param heapID
 */
//-----------------------------------------------------------------------------
static void SetupISS( ISS_SYS* system, GAMEDATA* gameData, HEAPID heapID )
{ 
  GF_ASSERT( system );
  GF_ASSERT( gameData );

	system->heapID   = heapID;
	system->gameData = gameData;
	system->soundIdx = INVALID_BGM_NO;
	system->frame    = 0;

  CreateISS_R( system );
  CreateISS_C( system );
  CreateISS_D( system );
  CreateISS_Z( system );
  CreateISS_S( system );
  CreateISS_B( system );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: setup ISS\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N���[���A�b�v����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CleanUpISS( ISS_SYS* system )
{
  GF_ASSERT( system );

  DeleteISS_R( system );
  DeleteISS_C( system );
  DeleteISS_D( system );
  DeleteISS_Z( system );
  DeleteISS_S( system );
  DeleteISS_B( system );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: clean up ISS\n" );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_R( ISS_SYS* system )
{
  PLAYER_WORK* player;

#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->gameData );
  GF_ASSERT( system->issR == NULL );

	player = GetPlayerWork( system );
	system->issR = ISS_ROAD_SYS_Create( player, system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-R\n" );
#endif
#endif 
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_C( ISS_SYS* system )
{
#ifdef ISS_C_ENABLE
	PLAYER_WORK* player;
	player = GetPlayerWork( system );

  GF_ASSERT( system );
  GF_ASSERT( system->gameData );
  GF_ASSERT( system->issC == NULL ); 

	system->issC = ISS_CITY_SYS_Create( player, system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-C\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_D( ISS_SYS* system )
{
#ifdef ISS_D_ENABLE
	PLAYER_WORK* player;
	player = GetPlayerWork( system );

  GF_ASSERT( system );
  GF_ASSERT( system->gameData );
  GF_ASSERT( system->issD == NULL );

	system->issD = 
    ISS_DUNGEON_SYS_Create( system->gameData, player, system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-D\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_Z( ISS_SYS* system )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ == NULL );

  system->issZ = ISS_ZONE_SYS_Create( system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-Z\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_S( ISS_SYS* system )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS == NULL );

  system->issS = ISS_SWITCH_SYS_Create( system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-S\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�𐶐�����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void CreateISS_B( ISS_SYS* system )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB == NULL );

  system->issB = ISS_3DS_SYS_Create( system->heapID );

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: create ISS-B\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
// �eISS�̔j��
static void DeleteISS_R( ISS_SYS* system )
{
#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issR );

	ISS_ROAD_SYS_Delete( system->issR );
  system->issR = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-R\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void DeleteISS_C( ISS_SYS* system )
{
#ifdef ISS_C_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issC );

	ISS_CITY_SYS_Delete( system->issC );
  system->issC = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-C\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void DeleteISS_D( ISS_SYS* system )
{
#ifdef ISS_D_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

	ISS_DUNGEON_SYS_Delete( system->issD );
  system->issD = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-D\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void DeleteISS_Z( ISS_SYS* system )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ );

  ISS_ZONE_SYS_Delete( system->issZ );
  system->issZ = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-Z\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void DeleteISS_S( ISS_SYS* system )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  ISS_SWITCH_SYS_Delete( system->issS );
  system->issS = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-S\n" );
#endif
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS��j������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void DeleteISS_B( ISS_SYS* system )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  ISS_3DS_SYS_Delete( system->issB );
  system->issB = NULL;

#ifdef PRINT_ENABLE
  OS_TFPrintf( PRINT_TARGET, "ISS: delete ISS-B\n" );
#endif
#endif
} 

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_R( ISS_SYS* system )
{
#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issR );

  // �b��30�t���[���œ��삳����
	if( system->frame % 2 == 0 ) {
    ISS_ROAD_SYS_Update( system->issR );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_C( ISS_SYS* system )
{
#ifdef ISS_C_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issC );

  // �b��30�t���[���œ��삳����
	if( system->frame % 2 == 0 ) {
    ISS_CITY_SYS_Main( system->issC );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_D( ISS_SYS* system )
{
#ifdef ISS_D_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

	ISS_DUNGEON_SYS_Update( system->issD );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_Z( ISS_SYS* system )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ );

  ISS_ZONE_SYS_Update( system->issZ );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_S( ISS_SYS* system )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  ISS_SWITCH_SYS_Update( system->issS );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�̃��C������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void MainISS_B( ISS_SYS* system )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  // �b��30�t���[���œ��삳����
	if( system->frame % 2 == 0 ) {
    ISS_3DS_SYS_Main( system->issB );
  }
#endif
} 

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_R( ISS_SYS* system )
{
#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issR );

  ISS_ROAD_SYS_On( system->issR );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_C( ISS_SYS* system )
{
#ifdef ISS_C_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issC );

  ISS_CITY_SYS_On( system->issC );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_D( ISS_SYS* system )
{
#ifdef ISS_D_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

  ISS_DUNGEON_SYS_On( system->issD );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_Z( ISS_SYS* system )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ );

  ISS_ZONE_SYS_On( system->issZ, GetCurrentZoneID( system ) );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_S( ISS_SYS* system )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  ISS_SWITCH_SYS_On( system->issS );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS���N������
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BootISS_B( ISS_SYS* system )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  ISS_3DS_SYS_On( system->issB );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_R( ISS_SYS* system )
{
#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issR );

  ISS_ROAD_SYS_Off( system->issR );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_C( ISS_SYS* system )
{
#ifdef ISS_C_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issC );

  ISS_CITY_SYS_Off( system->issC );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_D( ISS_SYS* system )
{
#ifdef ISS_D_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

  ISS_DUNGEON_SYS_Off( system->issD );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_Z( ISS_SYS* system )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ );

  ISS_ZONE_SYS_Off( system->issZ );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_S( ISS_SYS* system )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  ISS_SWITCH_SYS_Off( system->issS );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS���~����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void StopISS_B( ISS_SYS* system )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  ISS_3DS_SYS_Off( system->issB );
#endif
}


//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_R( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_R_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issR );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �XISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_C( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_C_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issC );

	ISS_CITY_SYS_ZoneChange( system->issC, nextZoneID );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �_���W����ISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_D( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_D_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issD );

	ISS_DUNGEON_SYS_ZoneChange( system->issD, nextZoneID );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �]�[��ISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_Z( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_Z_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issZ );

  ISS_ZONE_SYS_ZoneChange( system->issZ, nextZoneID );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_S( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_S_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issS );

  ISS_SWITCH_SYS_ZoneChange( system->issS, nextZoneID );
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ISS�Ƀ]�[���̕ύX��ʒm����
 *
 * @param system
 * @param nextZoneID �ύX��̃]�[��ID
 */
//-----------------------------------------------------------------------------
static void ZoneChangeISS_B( ISS_SYS* system, u16 nextZoneID )
{
#ifdef ISS_B_ENABLE
  GF_ASSERT( system );
  GF_ASSERT( system->issB );

  ISS_3DS_SYS_ZoneChange( system->issB );
#endif
} 

//-----------------------------------------------------------------------------
/**
 * @brief ���@���擾����
 *
 * @parma system
 *
 * @return PLAYER_WORK
 */
//-----------------------------------------------------------------------------
static PLAYER_WORK* GetPlayerWork( const ISS_SYS* system )
{
  GF_ASSERT( system );
  GF_ASSERT( system->gameData );

	return GAMEDATA_GetMyPlayerWork( system->gameData );
}

//-----------------------------------------------------------------------------
/**
 * @brief ���݂̃]�[��ID���擾����
 *
 * @parma system
 *
 * @return ���݂̎��@�̈ʒu����擾�����]�[��ID
 */
//-----------------------------------------------------------------------------
static u16 GetCurrentZoneID( const ISS_SYS* system )
{
  GF_ASSERT( system );

  return PLAYERWORK_getZoneID( GetPlayerWork(system) );
}

//-----------------------------------------------------------------------------
/**
 * @brief �Đ�����BGM���擾����
 *
 * @return ���ݍĐ�����BGM�̃V�[�P���X�ԍ�
 */
//-----------------------------------------------------------------------------
static u32 GetPlayingBGM( void )
{
  return PMSND_GetBGMsoundNo();
}

//-----------------------------------------------------------------------------
/**
 * @brief �Đ�����BGM��ISS�^�C�v���擾����
 *
 * @param system
 *
 * @return ���ݍĐ�����BGM��ISS�^�C�v ( ISS_TYPE_xxxx )
 */
//-----------------------------------------------------------------------------
static u8 GetISSTypeOfPlayingBGM( const ISS_SYS* system )
{
	u8 type;
	BGM_INFO_SYS* BGMInfo;

  GF_ASSERT( system );
  GF_ASSERT( system->gameData );
  
	// �Đ�����BGM��ISS�^�C�v���擾
	BGMInfo = GAMEDATA_GetBGMInfoSys( system->gameData ); 
	type    = BGM_INFO_GetIssType( BGMInfo, GetPlayingBGM() ); 

  return type;
}

//-----------------------------------------------------------------------------
/**
 * @brief BGM�̕ω����`�F�b�N����
 *
 * @param system
 *
 * @return BGM���ω������ꍇ TRUE, �����łȂ���� FALSE
 */
//-----------------------------------------------------------------------------
static BOOL CheckBGMChange( ISS_SYS* system )
{
  GF_ASSERT( system );

  // BGM�ω��Ȃ�
  if( system->soundIdx == GetPlayingBGM() ) { return FALSE; }

  // BGM�ω�����
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief BGM�ύX����
 *
 * @param system
 */
//-----------------------------------------------------------------------------
static void BGMChange( ISS_SYS* system )
{
	u8 ISSType;

	// �Đ�����BGM��ISS�^�C�v���擾
	ISSType = GetISSTypeOfPlayingBGM( system );

  // �eISS�V�X�e�����~
  switch( ISSType ) {
  case ISS_TYPE_LOAD:
      StopISS_C( system ); StopISS_D( system ); StopISS_Z( system );
      StopISS_S( system ); StopISS_B( system ); break;
  case ISS_TYPE_CITY:
      StopISS_R( system ); StopISS_D( system ); StopISS_Z( system );
      StopISS_S( system ); StopISS_B( system ); break;
  case ISS_TYPE_DUNGEON:
      StopISS_R( system ); StopISS_C( system ); StopISS_Z( system );
      StopISS_S( system ); StopISS_B( system ); break;
  case ISS_TYPE_ZONE:
      StopISS_R( system ); StopISS_C( system ); StopISS_D( system );
      StopISS_S( system ); StopISS_B( system ); break;
  case ISS_TYPE_SWITCH:
      StopISS_R( system ); StopISS_C( system ); StopISS_D( system );
      StopISS_Z( system ); StopISS_B( system ); break;
  case ISS_TYPE_BRIDGE:
      StopISS_R( system ); StopISS_C( system ); StopISS_D( system );
      StopISS_Z( system ); StopISS_S( system ); break;
  default:
      StopISS_R( system ); StopISS_C( system ); StopISS_D( system );
      StopISS_Z( system ); StopISS_S( system ); StopISS_B( system );
      break;
  }

  // �Đ�����BGM�ɑΉ�����ISS���N��
  switch( ISSType ) {
  case ISS_TYPE_LOAD:    BootISS_R( system ); break;
  case ISS_TYPE_CITY:    BootISS_C( system ); break;
  case ISS_TYPE_DUNGEON: BootISS_D( system ); break;
  case ISS_TYPE_ZONE:    BootISS_Z( system ); break;
  case ISS_TYPE_SWITCH:  BootISS_S( system ); break;
  case ISS_TYPE_BRIDGE:  BootISS_B( system ); break;
  } 
}
