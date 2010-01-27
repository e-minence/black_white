//============================================================================================
/**
 * @file  zonedata.c
 * @brief �]�[���ʃf�[�^
 * @author  tamada GAME FREAK inc.
 * @date  08.11.12
 */
//============================================================================================

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/fieldmap/zonedata.naix"
#include "arc/fieldmap/zone_id.h"
#include "arc/fieldmap/map_matrix.naix"
#include "arc/fieldmap/grid_camera_scene.naix"

#include "field/field_const.h"

#include "field/zonedata.h"
#include "zonetableformat.h"
#include "gamesystem/pm_season.h"

#include "arc/fieldmap/field_rail_setup.naix"  //NARC_field_rail_data_�`
#include "../../resource/fldmapdata/zonetable/header/maptype.h"

#include "fieldmap/field_fog_table.naix"
#include "fieldmap/field_zone_light.naix"
#include "fieldmap/fieldskill_mapeff.naix"

#include "field/field_zonefog.h"
#include "field/fieldskill_mapeff.h"


#include "sound/wb_sound_data.sadl" //SEQ_BGM_C_08_�`

//-------------------------------------
// �������[�N
//-------------------------------------
typedef struct
{
  // �A�[�J�C�u�n���h��
  ARCHANDLE* handle;

  // �]�[���t�H�O�@���C�g�@���X�g
  ZONE_FOG_DATA* zonefoglist;
  ZONE_LIGHT_DATA* zonelightlist;
  u16 zonefoglist_max;
  u8 zonelightlist_max;

  // �t�B�[���h�Z ���ʃ��X�g
  u8 fieldskill_mapeff_list_max;
  FIELDSKILL_MAPEFF_DATA* fieldskill_mapeff_list;

  // �]�[��ID�ύXID
  u8 zone_change_flag[ ZONEDATA_CHANGE_MAX ];

} ZONE_DATA_HANDLE;

static ZONE_DATA_HANDLE* data_handle = NULL;


//------------------------------------------------------------------
/**
 * @brief �]�[��ID�ύX�e�[�u��
 */
//------------------------------------------------------------------
#define ZONEDATA_CHANGE_ID_BEFORE (0)
#define ZONEDATA_CHANGE_ID_AFTER (1)
#define ZONEDATA_CHANGE_ID_TBL_NUM (2)
static u16 ZONEDATA_CHANGE_ID_TBL[ ZONEDATA_CHANGE_MAX ][ ZONEDATA_CHANGE_ID_TBL_NUM ] =
{
  { ZONE_ID_BC10, ZONE_ID_WC10 },   // ZONEDATA_CHANGE_BC_WF_ID BC��WF�ɕύX����B
  { ZONE_ID_PLC10, ZONE_ID_PLCW10 },   // ZONEDATA_CHANGE_BC_WF_ID BC��WF�ɕύX����B
};



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�n���h���E�I�[�v��
 */
//------------------------------------------------------------------
void ZONEDATA_Open( HEAPID heap_id )
{
  if( data_handle == NULL )
  {
    data_handle = GFL_HEAP_AllocClearMemory( heap_id, sizeof(ZONE_DATA_HANDLE) );

    // �A�[�J�C�u�n���h��
    data_handle->handle = GFL_ARC_OpenDataHandle( ARCID_ZONEDATA, heap_id );

    // �t�H�O�@���C�g
    {
      u32 size;

      data_handle->zonefoglist = GFL_ARC_UTIL_LoadEx( ARCID_ZONEFOG_TABLE, NARC_field_fog_table_zonefog_table_bin, FALSE, heap_id, &size );
      data_handle->zonefoglist_max = size / sizeof(ZONE_FOG_DATA);

      data_handle->zonelightlist = GFL_ARC_UTIL_LoadEx( ARCID_ZONELIGHT_TABLE, NARC_field_zone_light_light_list_bin, FALSE, heap_id, &size );
      data_handle->zonelightlist_max = size / sizeof(ZONE_LIGHT_DATA);

    }

    // �t�B�[���h�Z�@����
    {
      u32 size;

      data_handle->fieldskill_mapeff_list = GFL_ARC_UTIL_LoadEx( ARCID_FLDSKILL_MAPEFF, NARC_fieldskill_mapeff_fieldskill_mapeffect_bin, FALSE, heap_id, &size );
      data_handle->fieldskill_mapeff_list_max = size / sizeof(FIELDSKILL_MAPEFF_DATA);
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�n���h���E�N���[�Y
 */
//------------------------------------------------------------------
void ZONEDATA_Close()
{
  if( data_handle != NULL )
  {
    // �t�B�[���h�Z�@���ʃ��X�g
    {
      GFL_HEAP_FreeMemory( data_handle->fieldskill_mapeff_list );
    }

    // �t�H�O�@���C�g
    {
      GFL_HEAP_FreeMemory( data_handle->zonelightlist );
      GFL_HEAP_FreeMemory( data_handle->zonefoglist );
    }

    // �A�[�J�C�u�n���h��
    GFL_ARC_CloseDataHandle( data_handle->handle );

    GFL_HEAP_FreeMemory( data_handle );
    data_handle = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  ZONEID�̕ύX�ݒ�
 *
 *  @param  id      �]�[��ID�ύXID
 *  @param  flag    TRUE:�ύXON�@�@FALSE�F�ύXOFF
 */
//-----------------------------------------------------------------------------
void ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_ID id, BOOL flag )
{
  GF_ASSERT( data_handle );
  if( data_handle != NULL )
  {
    GF_ASSERT( id < ZONEDATA_CHANGE_MAX );
    data_handle->zone_change_flag[ id ] = flag;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ZONEID�̕ύX�ݒ���m�F
 *
 *  @param  id    �]�[��ID�ύXID
 *
 *  @retval TRUE      �ύXON
 *  @retval FALSE     �ύXOFF
 */
//-----------------------------------------------------------------------------
BOOL ZONEDATA_GetChangeZoneID( ZONEDATA_CHANGE_ID id )
{
  GF_ASSERT( data_handle );
  GF_ASSERT( id < ZONEDATA_CHANGE_MAX );
  return data_handle->zone_change_flag[ id ];
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �͈̓`�F�b�N
 */
//------------------------------------------------------------------
static inline u16 check_range(u16 zone_id)
{
  GF_ASSERT(zone_id < ZONE_ID_MAX);
  if ( zone_id >= ZONE_ID_MAX ) {
    return 0;
  } else {
    return zone_id;
  }
}
#define CHECK_RANGE(value)  {value = check_range(value);}


//------------------------------------------------------------------
/**
 * @brief ZONEID�̕ύX����
 */
//------------------------------------------------------------------
static u16 ControlZoneID( u16 zone_id )
{
  int i;

  for( i=0; i<ZONEDATA_CHANGE_MAX; i++ )
  {
    if( data_handle->zone_change_flag[i] )
    {
      if( ZONEDATA_CHANGE_ID_TBL[ i ][ ZONEDATA_CHANGE_ID_BEFORE ] == zone_id )
      {
        return ZONEDATA_CHANGE_ID_TBL[ i ][ ZONEDATA_CHANGE_ID_AFTER ];
      }
    }
  }
  return zone_id;
}

//------------------------------------------------------------------
/**
 * @brief �]�[���̍ő吔�̎擾
 * @return  u16 �ő吔
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetZoneIDMax(void)
{
  return ZONE_ID_MAX;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static ZONEDATA * loadZoneData(HEAPID heapID)
{
  ZONEDATA * buffer;
  if( data_handle != NULL )
  {
    buffer = GFL_ARC_LoadDataAllocByHandle(data_handle->handle, NARC_zonedata_zonetable_bin, heapID);
  }
  else
  {
    buffer = GFL_ARC_LoadDataAlloc(ARCID_ZONEDATA, NARC_zonedata_zonetable_bin, heapID);
  }
  return buffer;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static ZONEDATA * getZoneData(ZONEDATA * zdbuf, u16 zone_id)
{
  if( data_handle == NULL )
  {
    OS_Printf( "�A�[�J�C�u�n���h�����I�[�v�������Ă��܂���B\n" );
    return zdbuf;
  }

  // ZONEID�̂���ւ�
  zone_id = ControlZoneID(zone_id);

  CHECK_RANGE(zone_id); //�͈͊O�`�F�b�N

  if( data_handle != NULL )
  {
    GFL_ARC_LoadDataOfsByHandle(data_handle->handle,
        NARC_zonedata_zonetable_bin,
        sizeof(ZONEDATA) * zone_id, sizeof(ZONEDATA), zdbuf);
  }
  else
  {
    GFL_ARC_LoadDataOfs(zdbuf,
        ARCID_ZONEDATA, NARC_zonedata_zonetable_bin,
        sizeof(ZONEDATA) * zone_id, sizeof(ZONEDATA));
  }
  return zdbuf;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �G���A�w��̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �G���A�w��l
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetAreaID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.area_id;
}
//------------------------------------------------------------------
/**
 * @brief �}�g���b�N�X�w��̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �}�g���b�N�X�w��l
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetMatrixID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.matrix_id;
}
//------------------------------------------------------------------
/**
 * @brief �}�b�v�\���w��̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �}�b�v�\���w��l
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetMapRscID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.maprsc_id;
}

//------------------------------------------------------------------
/**
 * @brief �X�N���v�g�w��ID�̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �X�N���v�g�A�[�J�C�u�ł̎w��ID
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetScriptArcID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.script_id;
}

//------------------------------------------------------------------
/**
 * @brief ����X�N���v�g�w��ID�̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �X�N���v�g�A�[�J�C�u�ł̎w��ID
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetSpScriptArcID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.sp_script_id;
}

//------------------------------------------------------------------
/**
 * @brief �t�B�[���h���b�Z�[�W�w��ID�̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 ���b�Z�[�W�A�[�J�C�u�ł̎w��ID
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetMessageArcID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.msg_id;
}

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�n�ʒu�̎擾
 * @param zone_id �]�[���w��ID
 * @param pos   �J�n�ʒu���󂯎�邽�߂�fx32�^�|�C���^
 */
//------------------------------------------------------------------
void ZONEDATA_GetStartPos(u16 zone_id, VecFx32 * pos)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  pos->x = zdbuf.sx * FX32_ONE * FIELD_CONST_GRID_SIZE;
  pos->y = zdbuf.sy * FX32_ONE * FIELD_CONST_GRID_SIZE;
  pos->z = zdbuf.sz * FX32_ONE * FIELD_CONST_GRID_SIZE;
  TAMADA_Printf("%s x,y,z=%d,%d,%d\n",__FILE__,pos->x, pos->y, pos->z);
}

//----------------------------------------------------------------------------
/**
 * @brief �}�b�v�J�n���[���ʒu�̎擾
 *
 *  @param  zone_id
 *  @param  pos
 */
//-----------------------------------------------------------------------------
void ZONEDATA_GetStartRailPos(u16 zone_id, VecFx32 * pos)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  pos->x = zdbuf.sx;
  pos->y = zdbuf.sy;
  pos->z = zdbuf.sz;
}

//------------------------------------------------------------------
/**
 * @brief �J�����w��ID�̎擾
 * @param zone_id �]�[���w��ID
 * @return  u8    �J�����w��ID�̒l
 */
//------------------------------------------------------------------
u8 ZONEDATA_GetCameraID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.camera_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�����G���A�w��ID�̎擾
 *  @param  zone_id �]�[���w��ID
 *  @return �J�����G���A�w��ID
 */
//-----------------------------------------------------------------------------
u16 ZONEDATA_GetCameraAreaID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.camera_area;
}

//------------------------------------------------------------------
/**
 * @brief BGM�w��ID�̎擾
 * @param zone_id   �]�[���w��ID
 * @param season_id �G�ߎw��ID
 * @return  u8    BGM�w��ID�̒l
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetBGMID(u16 zone_id, u8 season_id)
{
  u16 bgm_id = 0;
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  switch (season_id) {
  case PMSEASON_SPRING:
    bgm_id = zdbuf.bgm_spring_id;
    break;
  case PMSEASON_SUMMER:
    bgm_id = zdbuf.bgm_summer_id;
    break;
  case PMSEASON_AUTUMN:
    bgm_id = zdbuf.bgm_autumn_id;
    break;
  case PMSEASON_WINTER:
    bgm_id = zdbuf.bgm_winter_id;
    break;
  default:
    GF_ASSERT(season_id < PMSEASON_TOTAL);
  }
  if (bgm_id == SEQ_BGM_C_08_B && GetVersion() == VERSION_WHITE )
  {
    bgm_id = SEQ_BGM_C_08_W;
  }
  return bgm_id;
}

//------------------------------------------------------------------
/**
 * @brief �V��w��ID�̎擾
 * @param zone_id �]�[���w��ID
 * @return  u16 �V��w��ID�iinclude/field/weather_no.h���Q�Ɓj
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetWeatherID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.weather_id;
}

//------------------------------------------------------------------
/// ���]�Ԃɏ���}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_BicycleEnable( u16 zone_id )
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.bicycle_flag;
}

//------------------------------------------------------------------
/// ���]�Ԃɏ�����ꍇ, BGM��ύX���邩�ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_BicycleBGMEnable( u16 zone_id )
{
  // @todo �]�[���e�[�u�����Q�Ƃ���
  return TRUE;
}

//------------------------------------------------------------------
/// �_�b�V���ł���}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_DashEnable( u16 zone_id )
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.dash_flag;
}
//------------------------------------------------------------------
/// �u������Ƃԁv�����g����}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_FlyEnable( u16 zone_id )
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.fly_flag;
}
//------------------------------------------------------------------
/// �u���Ȃʂ��v�ɂ��E�o���g����}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_EscapeEnable( u16 zone_id )
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.escape_flag;
}

//------------------------------------------------------------------
/**
 * @brief ����ȃT���v��OBJ���g�p���邩�ǂ����̐ݒ�擾
 * @param zone_id �]�[���w��ID
 * @param BOOL  TRUE�̂Ƃ��A�T���v��OBJ���g�p����
 */
//------------------------------------------------------------------
BOOL ZONEDATA_DEBUG_IsSampleObjUse(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return zdbuf.movemodel_id != 0;
}

//------------------------------------------------------------------
/**
 * @brief ���[���g�p�}�b�v���ǂ����̃`�F�b�N
 *
 *
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsRailMap(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  return (ZONEDATA_GetRailDataID(zone_id) != ZONEDATA_NO_RAILDATA_ID);
}

//------------------------------------------------------------------
/**
 *  @brief  ���[���f�[�^�w��ID���擾
 *  @param  zone_id �]�[���w��ID
 *  @retval ZONEDATA_NO_RAILDATA_ID( == -1)  ���[���f�[�^�����݂��Ȃ�
 *  @retval other ���[���f�[�^�A�[�J�C�u�w��ID
 */
//------------------------------------------------------------------
int ZONEDATA_GetRailDataID(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  switch (zone_id)
  {
  case ZONE_ID_C03:
    return NARC_field_rail_setup_C03_bin;
  case ZONE_ID_H01:
    return NARC_field_rail_setup_H01_bin;
  case ZONE_ID_C03P02:
    return NARC_field_rail_setup_C03P02_bin;
  case ZONE_ID_D09:
    return NARC_field_rail_setup_D09_bin;
  case ZONE_ID_R07R0101:
    return NARC_field_rail_setup_R07R0101_bin;
  case ZONE_ID_R07R0102:
    return NARC_field_rail_setup_R07R0102_bin;
  case ZONE_ID_R07R0103:
    return NARC_field_rail_setup_R07R0103_bin;
  case ZONE_ID_R07R0104:
    return NARC_field_rail_setup_R07R0104_bin;
  case ZONE_ID_C07GYM0101:
    return NARC_field_rail_setup_C07GYM0101_bin;
  case ZONE_ID_D08R0501:
    return NARC_field_rail_setup_D08R0501_bin;
  case ZONE_ID_D08R0701:
    return NARC_field_rail_setup_D08R0701_bin;
  case ZONE_ID_C04R0101:
    return NARC_field_rail_setup_C04R0101_bin;
  case ZONE_ID_C08GYM0101:
    return NARC_field_rail_setup_C08GYM0101_bin;
  }
  return ZONEDATA_NO_RAILDATA_ID;
}
//------------------------------------------------------------------
/**
 * @brief  ���j�I�����[���}�b�v���ǂ����̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @return BOOL ���j�I�����[����������TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsUnionRoom(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  return (zone_id == ZONE_ID_UNION);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL ZONEDATA_IsColosseum(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  if ( (zone_id == ZONE_ID_CLOSSEUM) || (zone_id == ZONE_ID_CLOSSEUM02) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//------------------------------------------------------------------
/**
 * @brief  �V���D���}�b�v���ǂ����̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @return BOOL �V���D����������TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsPlBoat(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  return (zone_id == ZONE_ID_C03R0801);
}

//==================================================================
/**
 * �p���X�}�b�v���ǂ����̃`�F�b�N
 * @param   zone_id   �]�[���w��ID
 * @retval  BOOL    �r���S�}�b�v��������TRUE
 */
//==================================================================
BOOL ZONEDATA_IsPalace(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  return (zone_id == ZONE_ID_PALACE01);
}
//==================================================================
/**
 * ���t�B�[���h���ǂ����̃`�F�b�N
 * @param   zone_id   �]�[���w��ID
 * @retval  BOOL    �r���S�}�b�v��������TRUE
 */
//==================================================================
BOOL ZONEDATA_IsPalaceField(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  if((zone_id >= ZONE_ID_PLC04 && zone_id <= ZONE_ID_PLT04) //���t�B�[���h
      || (zone_id == ZONE_ID_PALACE02)){      //�r���S�}�b�v
    return TRUE;
  }
  return FALSE;
}
//==================================================================
/**
 * �r���S�}�b�v���ǂ����̃`�F�b�N
 * @param   zone_id   �]�[���w��ID
 * @retval  BOOL    �r���S�}�b�v��������TRUE
 */
//==================================================================
BOOL ZONEDATA_IsBingo(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);
  return (zone_id == ZONE_ID_PALACE02);
}

//==================================================================
/**
 * WFBC���ǂ���
 * @param   zone_id   �]�[���w��ID
 * @retval  BOOL    �r���S�}�b�v��������TRUE
 */
//==================================================================
BOOL ZONEDATA_IsWfbc(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);
  return ((zone_id == ZONE_ID_BCWFTEST) || (zone_id == ZONE_ID_WC10) || (zone_id == ZONE_ID_BC10) || (zone_id == ZONE_ID_PLC10) || (zone_id == ZONE_ID_PLCW10));
}

//------------------------------------------------------------------
/**
 * @brief  �~���[�W�J���ҍ������ǂ����̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @return BOOL �~���[�W�J���ҍ�����������TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsMusicalWaitingRoom(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);
  return (zone_id == ZONE_ID_C04R0202);
}
//------------------------------------------------------------------
/**
 * @brief  �n�����b�Z�[�W��ID���擾
 * @param  zoneid �]�[���w��ID
 * @return �n�����b�Z�[�WID
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetPlaceNameID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData( &zdbuf, zone_id );
  return zdbuf.placename_id;
}

//------------------------------------------------------------------
/**
 * @brief  �n���\������p�t���O�擾
 * @param  zoneid �]�[���w��ID
 * @return  BOOL
 */
//------------------------------------------------------------------
BOOL ZONEDATA_GetPlaceNameFlag(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData( &zdbuf, zone_id );
  return zdbuf.placename_flag;
}

//------------------------------------------------------------------
/**
 * @brief �w��]�[���̃}�b�v�^�C�v�擾
 */
//------------------------------------------------------------------
static u16 getMapType(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData( &zdbuf, zone_id );
  return zdbuf.maptype;
}

//------------------------------------------------------------------
/**
 * @brief  �w��]�[�����_���W�������ǂ����𔻒�
 * @param  zone_id ���肷��]�[����ID
 * @return �_���W�����Ȃ�TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsDungeon(u16 zone_id)
{
  u16 maptype = getMapType( zone_id );
  if( maptype == MAPTYPE_DUNGEON )
  {
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief �w��]�[�����t�B�[���h�}�b�v�̃}�g���b�N�X�w�肩�ǂ����𔻒�
 * @param  zone_id ���肷��]�[����ID
 * @return  BOOL TRUE�̂Ƃ��A�t�B�[���h�}�b�v�̃}�g���b�N�X�w��
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsFieldMatrixID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return (zdbuf.matrix_id == NARC_map_matrix_wb_mat_bin);
}

//------------------------------------------------------------------
/**
 * @brief �w��]�[���̃G���J�E���g�f�[�^ID���擾
 * @param  zone_id ���肷��]�[����ID
 * @return  BOOL TRUE�̂Ƃ��A�t�B�[���h�}�b�v�̃}�g���b�N�X�w��
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetEncountDataID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return (zdbuf.enc_data_id);
}

//------------------------------------------------------------------
/**
 * @brief �w��]�[���̃C�x���g�f�[�^ID�̎擾
 * @param  zone_id ���肷��]�[����ID
 * @return  �C�x���g�f�[�^�̃A�[�J�C�uID
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetEventDataArcID(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return (zdbuf.event_data_id);
}

//------------------------------------------------------------------
/**
 * @brief �w��]�[���̐퓬�w�iID�̎擾
 * @param  zone_id ���肷��]�[����ID
 * @return  �C�x���g�f�[�^�̃A�[�J�C�uID
 */
//------------------------------------------------------------------
u8 ZONEDATA_GetBattleBGType(u16 zone_id)
{
  ZONEDATA zdbuf;
  getZoneData(&zdbuf, zone_id);
  return (zdbuf.battle_bg_type);
}


//----------------------------------------------------------------------------
/**
 *  @brief  ���[�񂲂Ƃ�FOG�f�[�^�C���f�b�N�X�擾 field_zonefog.h�Q��
 *
 *  @param  zone_id �]�[��ID
 *
 *  @retval �f�[�^�C���f�b�N�X
 *  @retval FIELD_ZONEFOGLIGHT_DATA_NONE  �Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZONEDATA_GetFog(u16 zone_id)
{
  int i;
  u32 ret = FIELD_ZONEFOGLIGHT_DATA_NONE;

  GF_ASSERT( data_handle );

  zone_id = ControlZoneID(zone_id);

  for( i=0; i<data_handle->zonefoglist_max; i++ )
  {
    if( data_handle->zonefoglist[i].zone_id == zone_id )
    {
      ret = data_handle->zonefoglist[i].data_id;
      break;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���[�񂲂Ƃ�LIGHT�f�[�^�C���f�b�N�X�擾 field_zonefog.h�Q��
 *
 *  @param  zone_id �]�[��ID
 *
 *  @retval �f�[�^�C���f�b�N�X
 *  @retval FIELD_ZONEFOGLIGHT_DATA_NONE  �Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZONEDATA_GetLight(u16 zone_id)
{
  int i;
  u32 ret = FIELD_ZONEFOGLIGHT_DATA_NONE;

  GF_ASSERT( data_handle );

  zone_id = ControlZoneID(zone_id);

  for( i=0; i<data_handle->zonelightlist_max; i++ )
  {
    if( data_handle->zonelightlist[i].zone_id == zone_id )
    {
      ret = data_handle->zonelightlist[i].data_id;
      break;
    }
  }

  return ret;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�Z�@�}�b�v�G�t�F�N�g�}�X�N�擾
 *
 *  @param  zone_id �]�[��ID
 *
 *  @return FIELDSKILL_MAPEFF_MSK
 */
//-----------------------------------------------------------------------------
u32 ZONEDATA_GetFieldSkillMapEffMsk(u16 zone_id)
{
  int i;
  u32 ret = 0;

  GF_ASSERT( data_handle );

  zone_id = ControlZoneID(zone_id);

  for( i=0; i<data_handle->fieldskill_mapeff_list_max; i++ )
  {
    if( data_handle->fieldskill_mapeff_list[i].zone_id == zone_id )
    {
      ret = data_handle->fieldskill_mapeff_list[i].msk;
      break;
    }
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[���G���AID�̎擾
 *
 *  @param  zone_id �]�[��ID
 *
 *  @return �V�[���G���A�f�[�^ID
 */
//-----------------------------------------------------------------------------
u32 ZONEDATA_GetSceneAreaID(u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  if( zone_id == ZONE_ID_C04 )
  {
    return NARC_grid_camera_scene_camera_scene_C04_dat;
  }
  return ZONEDATA_NO_SCENEAREA_ID;
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �]�[�����f�[�^�̎擾
 * @param buffer    ���O���擾���邽�߂̃o�b�t�@(ZONEDATA_NAME_LENGTH�̒������K�v�j
 * @param zoneid    �擾����]�[���̖��O
 */
//------------------------------------------------------------------
void ZONEDATA_DEBUG_GetZoneName(char * buffer, u16 zone_id)
{
  zone_id = ControlZoneID(zone_id);

  if( data_handle != NULL )
  {
    GFL_ARC_LoadDataOfsByHandle(data_handle->handle,
        NARC_zonedata_zonename_bin,
        ZONEDATA_NAME_LENGTH * zone_id, ZONEDATA_NAME_LENGTH, buffer);
  }
  else
  {
    GFL_ARC_LoadDataOfs(buffer,
        ARCID_ZONEDATA, NARC_zonedata_zonename_bin,
        ZONEDATA_NAME_LENGTH * zone_id, ZONEDATA_NAME_LENGTH);
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const char * ZONEDATA_GetAllZoneName(HEAPID heapID)
{
  char * namedata;
  if( data_handle != NULL )
  {
    namedata = GFL_ARC_LoadDataAllocByHandle(
        data_handle->handle, NARC_zonedata_zonename_bin, heapID);
  }
  else
  {
    namedata = GFL_ARC_LoadDataAlloc(
        ARCID_ZONEDATA, NARC_zonedata_zonename_bin, heapID);
  }
  return namedata;
}




