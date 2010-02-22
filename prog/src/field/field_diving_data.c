//============================================================================================
/**
 * @file  field_diving_data.c
 * @brief �_�C�r���O�ڑ���`�F�b�N�p�f�[�^�Ƃ��̃A�N�Z�X�֐�
 * @date  2010.02.21
 * @author  tamada GAME FREAK inc.
 */
//============================================================================================

#include <gflib.h>

#include "field_diving_data.h"

#include "fieldmap.h"
#include "field_player.h"
#include "fldmmdl.h"

#include "arc/fieldmap/zone_id.h"
//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct {
  u16 check_zone_id;
  u16 grid_x, grid_z;
  u16 connect_zone_id;
}DIVING_SPOT_TABLE;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "../../../resource/fldmapdata/diving_spot/diving_spot.cdat"

#define DIVING_SPOT_MAX NELEMS(DivingSpotTable)

//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
/**
 * @brief �_�C�r���O������̃]�[��ID���擾����
 * @param zone_id   ���݂̃]�[��
 * @param x     �T��X�ʒu�i�O���b�h�P�ʁj
 * @param z     �T��Z�ʒu�i�O���b�h�P�ʁj
 * @return  u16 zone_id ZONE_ID_MAX�̎��A������Ȃ�����
 */
//-----------------------------------------------------------------------------
static u16 DIVINGSPOT_GetZoneID( u16 zone_id, u16 x, u16 z )
{
  int i;
  const DIVING_SPOT_TABLE * tbl = DivingSpotTable;
  for ( i = 0; i < DIVING_SPOT_MAX; i++, tbl++ )
  {
    if (tbl->check_zone_id == zone_id
        && tbl->grid_x <= x && x < tbl->grid_x + 3
        && tbl->grid_z <= z && z < tbl->grid_z + 3 )
    {
      return tbl->connect_zone_id;
    }
  }
  return ZONE_ID_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @brief �_�C�r���O�ł��邩�ǂ����̔���
 * @param zone_id   ���݂̃]�[��
 * @param x     �T��X�ʒu�i�O���b�h�P�ʁj
 * @param z     �T��Z�ʒu�i�O���b�h�P�ʁj
 * @return  BOOL  TRUE�̂Ƃ��A�_�C�r���O�\
 */
//-----------------------------------------------------------------------------
static BOOL DIVINGSPOT_SearchData( u16 zone_id, u16 x, u16 z )
{
  return DIVINGSPOT_GetZoneID( zone_id, x, z) != ZONE_ID_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @brief �_�C�r���O���菈��
 * @param fieldmap  FIELDMAP_WORK�ւ̃|�C���^
 * @param zone_id   �J�ڐ�}�b�vID���󂯎�邽�߂̃|�C���^
 * @return  BOOL  TRUE�̂Ƃ��A�_�C�r���O�\
 */
//-----------------------------------------------------------------------------
BOOL DIVINGSPOT_Check( FIELDMAP_WORK * fieldmap, u16 * zone_id )
{
  u16 now_zone_id = FIELDMAP_GetZoneID( fieldmap );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  u16 x = MMDL_GetGridPosX( mmdl );
  u16 z = MMDL_GetGridPosZ( mmdl );
  *zone_id = DIVINGSPOT_GetZoneID( now_zone_id, x, z );
  if ( *zone_id != ZONE_ID_MAX ) return TRUE;
  //������̕s��Ώ�
  *zone_id = now_zone_id;
  return FALSE;
}

