///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_city_unit.c
 * @brief  �XISS���j�b�g
 * @author obata_toshihiro
 * @date   2009.11.02
 */
///////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "iss_city_unit.h"
#include "field/field_const.h" // for FX32_TO_GRID
#include "arc_def.h"  // for ARCID_ISS_CITY
#include "../../../resource/fldmapdata/zonetable/zone_id.h" 


//===========================================================================================
// ���萔
//===========================================================================================

// 1�̊XISS���j�b�g�������ʋ�Ԃ̐�
#define VOLUME_SPACE_NUM (4)



//===========================================================================================
// ���XISS���j�b�g�\����
//===========================================================================================
struct _ISS_C_UNIT
{
	// �z�u�ꏊ�̃]�[��ID
	u16 zoneID;
  u16 padding;

	// ���W[grid]
	int x;
	int y;
	int z;

	// ���ʋ��
  u8 volume[VOLUME_SPACE_NUM];
  u8 x_range[VOLUME_SPACE_NUM];
  u8 y_range[VOLUME_SPACE_NUM];
  u8 z_range[VOLUME_SPACE_NUM];
};

//-------------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����W����ԓ��ɂ��邩�ǂ����𔻒肷��
 *
 * @param unit  ����Ώۂ̃��j�b�g
 * @param index ����Ώۂ̉��ʋ�Ԃ��w��
 * @param cx	  ���ʋ�Ԓ��S����x���W
 * @param cy	  ���ʋ�Ԓ��S����y���W
 * @param cz	  ���ʋ�Ԓ��S����z���W
 * @param x		  ��l����x���W
 * @param y		  ��l����y���W
 * @param z		  ��l����z���W
 *
 * @return (x, y, z) �� �w�肵����Ԃ̓����ɂ���ꍇ TRUE
 */
//-------------------------------------------------------------------------------------------
static BOOL IsCover( const ISS_C_UNIT* unit, int index, int cx, int cy, int cz, int x, int y, int z )
{
	int min_x, min_y, min_z;
	int max_x, max_y, max_z;

	// ��Ԃ̍ŏ��l�E�ő�l�����߂�
	min_x = cx - unit->x_range[index];
	min_y = cy - unit->y_range[index];
	min_z = cz - unit->z_range[index]; 
	max_x = cx + unit->x_range[index];
	max_y = cy + unit->y_range[index];
	max_z = cz + unit->z_range[index];

	// ����
	if( ( x < min_x ) | ( max_x < x ) |
		  ( y < min_y ) | ( max_y < y ) |
		  ( z < min_z ) | ( max_z < z ) )
  {
    return FALSE;
  } 
	return TRUE;
}


//=========================================================================================
// ���쐬�E�j��
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @breif ���j�b�g���쐬����
 *
 * @param heap_id �g�p����q�[�vID
 * @param dat_id  �ǂݍ��ރ��j�b�g�f�[�^�̃A�[�J�C�u���C���f�b�N�X
 */
//-----------------------------------------------------------------------------------------
ISS_C_UNIT* ISS_C_UNIT_Create( HEAPID heap_id, u16 dat_id )
{
  ISS_C_UNIT* unit = NULL;

  // �ǂݍ���
  unit = GFL_ARC_LoadDataAllocOfs( ARCID_ISS_CITY, dat_id, heap_id, 0, sizeof(ISS_C_UNIT) );

  // DEBUG:
  OBATA_Printf( "ISS-C-UNIT: Create\n" );
  ISS_C_UNIT_DebugPrint( unit );
  return unit; 
}

//-----------------------------------------------------------------------------------------
/**
 * @breif ���j�b�g��j������
 *
 * @param unit �j�����郆�j�b�g
 */
//-----------------------------------------------------------------------------------------
void ISS_C_UNIT_Delete( ISS_C_UNIT* unit )
{
  GFL_HEAP_FreeMemory( unit );
}


//=========================================================================================
// ���擾
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief �]�[��ID���擾����
 *
 * @param unit �擾�Ώۂ̃��j�b�g
 *
 * @return �w�肵�����j�b�g�̃]�[��ID
 */
//-----------------------------------------------------------------------------------------
u16 ISS_C_UNIT_GetZoneID( const ISS_C_UNIT* unit )
{
  return unit->zoneID;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ���ʂ��擾����
 *
 * @param unit ����Ώۂ̃��j�b�g
 * @param pos  ��l���̍��W[�P�ʁF���[���h���W]
 *
 * @return �w�肵����l�����W�ɂ����鉹��[0, 127]
 */
//-------------------------------------------------------------------------------------------
int ISS_C_UNIT_GetVolume( const ISS_C_UNIT* unit, const VecFx32* pos )
{
	int i;
	int volume = 0;
	int x, y, z;

	// �O���b�h���W���v�Z
  x = FX32_TO_GRID(pos->x);
  y = FX32_TO_GRID(pos->y);
  z = FX32_TO_GRID(pos->z);

	// �傫�ȋ�Ԃ��画�肵, �����Ȃ���Ԃ���������, �����I��
	for( i=0; i<VOLUME_SPACE_NUM; i++ )
	{
		if( IsCover( unit, i, unit->x, unit->y, unit->z, x, y, z ) != TRUE )
		{
			break;
		}

		// ��l����������, �ł���������Ԃɐݒ肳�ꂽ���ʂ��擾����
		volume = unit->volume[i];
	} 
	return volume;
}


//=========================================================================================
// ���f�o�b�O
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief ���j�b�g�����o�͂���
 *
 * @param unit �o�͑Ώۃ��j�b�g
 */
//-----------------------------------------------------------------------------------------
void ISS_C_UNIT_DebugPrint( const ISS_C_UNIT* unit )
{
  int i;
  OBATA_Printf( "- zoneID = %d\n", unit->zoneID );
  OBATA_Printf( "- pos = (%d, %d, %d)\n", unit->x, unit->y, unit->z );
  for( i=0; i<VOLUME_SPACE_NUM; i++ )
  {
    OBATA_Printf( "- volume, range = %d, (%d, %d, %d)\n",
        unit->volume[i], unit->x_range[i], unit->y_range[i], unit->z_range[i] );
  }
}
