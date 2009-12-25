/////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_sys.c
 * @brief  �XISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.16
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include "iss_city_sys.h"
#include "../field/field_sound.h"
#include "gamesystem/playerwork.h"
#include "../../include/field/field_const.h"		// FIELD_CONST_GRID_FX32_SIZE
#include "arc/arc_def.h"
#include "field/field_const.h"
#include "iss_city_unit.h"
#include "../../../resource/fldmapdata/zonetable/zone_id.h"


//===========================================================================================
// ���萔�E�}�N��
//===========================================================================================

// �������j�b�g�ԍ�(�XISS��BGM�����Ă��邪, ISS���j�b�g���z�u����Ă��Ȃ���)
#define INVALID_UNIT_NO (0xff)

// �ő剹��
#define MAX_VOLUME (127)
#define INVALID_VOLUME (0xffff)

// ����g���b�N
#define TRACKBIT ((1<<(9-1))|(1<<(10-1))) // 9,10�g���b�N


//===========================================================================================
// ���XISS�V�X�e���\����
//===========================================================================================
struct _ISS_CITY_SYS
{
	// �g�p����q�[�vID
	HEAPID heapID;

	// �Ď��Ώۃv���C���[
	PLAYER_WORK* pPlayer;

	// �N�����
	BOOL isActive;		  // ���쒆���ǂ���
	u8   activeUnitNo;	// ���쒆�̃��j�b�g�ԍ�
  int  volume;        // ����

	// ���j�b�g���
	u8        unitNum;		// ���j�b�g��
	ISS_C_UNIT** unit;		// ���j�b�g�z��
};


//===========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//===========================================================================================
static void BootSystem( ISS_CITY_SYS* sys );
static void StopSystem( ISS_CITY_SYS* sys );
static void LoadUnitData( ISS_CITY_SYS* sys );
static void UpdateVolume( ISS_CITY_SYS* sys );
static void ChangeUnit( ISS_CITY_SYS* sys, u16 zone_id );


//===========================================================================================
// �����J�֐��̎���
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e�����쐬����
 *
 * @param  p_player �Ď��Ώۂ̃v���C���[
 * @param  heap_id  �g�p����q�[�vID
 * 
 * @return �XISS�V�X�e��
 */
//-------------------------------------------------------------------------------------------
ISS_CITY_SYS* ISS_CITY_SYS_Create( PLAYER_WORK* p_player, HEAPID heap_id )
{
	ISS_CITY_SYS* sys;

	// �������m��
	sys = (ISS_CITY_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( ISS_CITY_SYS ) );

	// ������
	sys->heapID       = heap_id;
	sys->pPlayer      = p_player;
	sys->isActive     = FALSE;
	sys->activeUnitNo = INVALID_UNIT_NO;
  sys->volume       = INVALID_VOLUME;
	sys->unitNum      = 0;
	sys->unit         = NULL;

	// ���j�b�g���̓ǂݍ���
	LoadUnitData( sys );

  // DEBUG:
  OBATA_Printf( "ISS-C: create\n" );
	
	// �쐬�����XISS�V�X�e����Ԃ�
	return sys;
}

//------------------------------------------------------------------------------------------
/**
 * @brief  �XISS�V�X�e����j������
 *
 * @param sys �j������XISS�V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Delete( ISS_CITY_SYS* sys )
{
  int i;

	// �e���j�b�g��j��
  for( i=0; i<sys->unitNum; i++ )
  {
    ISS_C_UNIT_Delete( sys->unit[i] );
  }
	GFL_HEAP_FreeMemory( sys->unit );

	// �{�̂̔j��
	GFL_HEAP_FreeMemory( sys );

  // DEBUG:
  OBATA_Printf( "ISS-C: delete\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �v���C���[���Ď���, ���ʂ𒲐�����
 *
 * @param sys ����Ώۂ̊XISS�V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Update( ISS_CITY_SYS* sys )
{
  // ��~��
	if( sys->isActive != TRUE ){ return; }

  // ���ʂ𒲐�����
  UpdateVolume( sys );
}
	
//------------------------------------------------------------------------------------------
/**
 * @brief �]�[���؂�ւ���ʒm����
 *
 * @param sys        �ʒm�Ώۂ̊XISS�V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_ZoneChange( ISS_CITY_SYS* sys, u16 next_zone_id )
{ 
  // ��~��
  if( sys->isActive != TRUE ){ return; }

  // ���j�b�g��؂�ւ���
  ChangeUnit( sys, next_zone_id );

  // ���ʂ𒲐�
  UpdateVolume( sys );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �N������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_On( ISS_CITY_SYS* sys )
{ 
  BootSystem( sys );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~������
 *
 * @param sys ��~������V�X�e��
 */
//------------------------------------------------------------------------------------------
void ISS_CITY_SYS_Off( ISS_CITY_SYS* sys )
{
  StopSystem( sys );
}

//------------------------------------------------------------------------------------------
/**
 * @breif �����Ԃ��擾����
 *
 * @param sys ��Ԃ𒲂ׂ�ISS�V�X�e��
 * 
 * @return ���쒆���ǂ���
 */
//------------------------------------------------------------------------------------------
BOOL ISS_CITY_SYS_IsOn( const ISS_CITY_SYS* sys )
{
	return sys->isActive;
}


//===========================================================================================
// ������J�֐��̎���
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����N������
 *
 * @param sys �XISS�V�X�e��
 */
//-------------------------------------------------------------------------------------------
static void BootSystem( ISS_CITY_SYS* sys )
{
  // �N����
  if( sys->isActive ){ return; }

  // �V�X�e���N��
  sys->isActive = TRUE;
  sys->volume   = INVALID_VOLUME;
  // DEBUG:
  OBATA_Printf( "ISS-C: boot\n" );

  // ���j�b�g����
  {
    u16 zone_id;
    zone_id = PLAYERWORK_getZoneID( sys->pPlayer );
    ChangeUnit( sys, zone_id ); 
  } 
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �V�X�e�����~����
 *
 * @param sys �XISS�V�X�e��
 */
//-------------------------------------------------------------------------------------------
static void StopSystem( ISS_CITY_SYS* sys )
{
  // ��~��
  if( !sys->isActive ){ return; }

  // ��~
	sys->isActive     = FALSE;
  sys->activeUnitNo = INVALID_UNIT_NO;
  // ���삵�Ă����g���b�N�����ɖ߂�
  //PMSND_ChangeBGMVolume( TRACKBIT, 127 ); 

  // DEBUG:
  OBATA_Printf( "ISS-C: stop\n" );
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�f�[�^��ǂݍ���
 *
 * @param sys �ǂݍ��񂾃f�[�^��ێ�����V�X�e��
 */
//-------------------------------------------------------------------------------------------
static void LoadUnitData( ISS_CITY_SYS* sys )
{
  int datid;

  // �o�^����Ă��郆�j�b�g�̐����擾
  sys->unitNum = GFL_ARC_GetDataFileCnt( ARCID_ISS_CITY );

  // �e���j�b�g���쐬
  sys->unit = GFL_HEAP_AllocMemory( sys->heapID, sys->unitNum * sizeof(ISS_C_UNIT*) );

  // �e���j�b�g�f�[�^��ǂݍ���
  for( datid=0; datid<sys->unitNum; datid++ )
  {
    sys->unit[datid] = ISS_C_UNIT_Create( sys->heapID, datid );
  } 
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ���ʂ𒲐�����
 *
 * @param sys �������s��
 */
//-------------------------------------------------------------------------------------------
static void UpdateVolume( ISS_CITY_SYS* sys )
{
  int new_volume = 0;
  const VecFx32* pos;

  // �N�����Ă��Ȃ�
  GF_ASSERT( sys->isActive );  

  // ���j�b�g���z�u����Ă��Ȃ�
  if( sys->activeUnitNo == INVALID_UNIT_NO ){ return; }

  // ���j�b�g�Ǝ��@�̈ʒu����{�����[�����Z�o
  pos        = PLAYERWORK_getPosition( sys->pPlayer );
  new_volume = ISS_C_UNIT_GetVolume( sys->unit[sys->activeUnitNo], pos );

  // ���ʂ��ω������ꍇ
  if( sys->volume != new_volume )
  {
    // �X�V
    sys->volume = new_volume;
    PMSND_ChangeBGMVolume( TRACKBIT, new_volume );
    // DEBUG:
    OBATA_Printf( "ISS-C: update volume ==> %d\n", new_volume );
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �w�肵���]�[���ɔz�u���ꂽ���j�b�g�ɐ؂�ւ���
 *
 * @param sys     ���j�b�g��؂�ւ���V�X�e��
 * @param zone_id �؂�ւ���̃]�[��ID
 */
//-------------------------------------------------------------------------------------------
static void ChangeUnit( ISS_CITY_SYS* sys, u16 zone_id )
{
	int unit_idx;

  // �N�����Ă��Ȃ�
  GF_ASSERT( sys->isActive );

  // ����
	for( unit_idx=0; unit_idx<sys->unitNum; unit_idx++ )
	{
		// ���� ==> ���j�b�g�ԍ����X�V
		if( ISS_C_UNIT_GetZoneID( sys->unit[unit_idx] ) == zone_id )
		{ 
			sys->activeUnitNo = unit_idx;
      // DEBUG:
      OBATA_Printf( "ISS-C: change unit index ==> %d\n", sys->activeUnitNo );
      return;
		}
	}

  // �z�u����Ă��Ȃ�
  sys->activeUnitNo = INVALID_UNIT_NO;
}
