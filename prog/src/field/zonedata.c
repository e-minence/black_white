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
#include "msg/msg_place_name.h"  // for MAPNAME_xxxx

#include "field/field_zonefog.h"
#include "field/fieldskill_mapeff.h"

#include "map_change_type.h"

#include "sound/wb_sound_data.sadl" //SEQ_BGM_C_08_�`

//-------------------------------------
// �������[�N
//-------------------------------------
typedef struct
{
  // �A�[�J�C�u�n���h��
  ARCHANDLE* handle;

  // �]�[���f�[�^
  ZONEDATA zoneData;    // �ǂݍ��񂾃]�[���f�[�^
  u16      zoneDataID;  // �ǂ̃]�[���̃f�[�^��ǂݍ��񂾂�

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

    // �]�[���f�[�^
    data_handle->zoneDataID = ZONE_ID_MAX;

    // �]�[���f�[�^�`�F���W�����Z�b�g�A�b�v
    if( GET_VERSION() == VERSION_WHITE ){
      ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_BC_WF_ID, TRUE );
      ZONEDATA_SetChangeZoneID( ZONEDATA_CHANGE_PLC10_PLCW10_ID, TRUE );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�n���h���E�N���[�Y
 */
//------------------------------------------------------------------
void ZONEDATA_Close( void )
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
static ZONEDATA * loadZoneData( u16 zoneID )
{
  // �n���h�����I�[�v������Ă��Ȃ�
  if( data_handle == NULL )
  {
    GF_ASSERT( 0 && "�A�[�J�C�u�n���h�����I�[�v�������Ă��܂���B" );
    return NULL;
  }

  // ZONEID�̂���ւ�
  zoneID = ControlZoneID(zoneID);

  // �ǂݍ��ݍς�
  if( zoneID == data_handle->zoneDataID ){ return &( data_handle->zoneData ); }  

  GFL_ARC_LoadDataOfsByHandle( data_handle->handle,
      NARC_result_zonedata_zonetable_bin,
      sizeof(ZONEDATA) * zoneID, sizeof(ZONEDATA), &( data_handle->zoneData ) );

  data_handle->zoneDataID = zoneID;

  return &( data_handle->zoneData );
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
        NARC_result_zonedata_zonetable_bin,
        sizeof(ZONEDATA) * zone_id, sizeof(ZONEDATA), zdbuf);
  }
  else
  {
    GFL_ARC_LoadDataOfs(zdbuf,
        ARCID_ZONEDATA, NARC_result_zonedata_zonetable_bin,
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->area_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->matrix_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->maprsc_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->script_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->sp_script_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->msg_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  pos->x = zoneData->sx * FX32_ONE * FIELD_CONST_GRID_SIZE;
  pos->y = zoneData->sy * FX32_ONE * FIELD_CONST_GRID_SIZE;
  pos->z = zoneData->sz * FX32_ONE * FIELD_CONST_GRID_SIZE;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  pos->x = zoneData->sx;
  pos->y = zoneData->sy;
  pos->z = zoneData->sz;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->camera_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->camera_area;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );

  switch (season_id) {
  case PMSEASON_SPRING:
    bgm_id = zoneData->bgm_spring_id;
    break;
  case PMSEASON_SUMMER:
    bgm_id = zoneData->bgm_summer_id;
    break;
  case PMSEASON_AUTUMN:
    bgm_id = zoneData->bgm_autumn_id;
    break;
  case PMSEASON_WINTER:
    bgm_id = zoneData->bgm_winter_id;
    break;
  default:
    GF_ASSERT(season_id < PMSEASON_TOTAL);
  }
  if (bgm_id == SEQ_BGM_C_08_B && GET_VERSION() == VERSION_WHITE )
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->weather_id;
}

//------------------------------------------------------------------
/// ���]�Ԃɏ���}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_BicycleEnable( u16 zone_id )
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->bicycle_flag;
}

//------------------------------------------------------------------
/// ���]�Ԃɏ�����ꍇ, BGM��ύX���邩�ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_BicycleBGMEnable( u16 zone_id )
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->sp_bgm_flag;
}

//------------------------------------------------------------------
/// �_�b�V���ł���}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_DashEnable( u16 zone_id )
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->dash_flag;
}
//------------------------------------------------------------------
/// �u������Ƃԁv�����g����}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_FlyEnable( u16 zone_id )
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->fly_flag;
}
//------------------------------------------------------------------
/// �u���Ȃʂ��v�ɂ��E�o���g����}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_EscapeEnable( u16 zone_id )
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->escape_flag;
}
//------------------------------------------------------------------
/// �p���X�ɑJ�ڂł���}�b�v���ǂ���
//------------------------------------------------------------------
BOOL ZONEDATA_EnablePalaceUse( u16 zone_id )
{
  ZONEDATA* zoneData = loadZoneData( zone_id );
  return zoneData->palace_ok_flag;
}

//------------------------------------------------------------------
// ���[���I�����[�̃}�b�v���ǂ����`�F�b�N
//------------------------------------------------------------------
BOOL ZONEDATA_IsRailOnlyMap( u16 zone_id )
{
  switch(zone_id)
  {
  case ZONE_ID_C03:
  case ZONE_ID_H01:
  case ZONE_ID_D09:
  case ZONE_ID_D08R0501:
  case ZONE_ID_D08R0701:
  case ZONE_ID_C04R0101:
  case ZONE_ID_D20R0101:
  case ZONE_ID_D20R0201:
  case ZONE_ID_D20R0301:
  case ZONE_ID_D20R0401:
  case ZONE_ID_C03P13:
    return TRUE;
  }
  return FALSE;
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
  case ZONE_ID_C03P01:
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
  case ZONE_ID_D20R0101:
  case ZONE_ID_D20R0201:
  case ZONE_ID_D20R0301:
  case ZONE_ID_D20R0401:
    return NARC_field_rail_setup_D20_bin;

  case ZONE_ID_C03P13:
    return NARC_field_rail_setup_C03P13_bin;
  case ZONE_ID_H04:
    return NARC_field_rail_setup_H04_bin;
  case ZONE_ID_C09P01:
    return NARC_field_rail_setup_C09P01_bin;
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

//------------------------------------------------------------------
/**
 * @brief �r�[�R���T�[�`�֎~�}�b�v���ǂ����̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @retval  �r�[�R���T�[�`�֎~�}�b�v�̏ꍇ�ATRUE
 *
 * APP_CONTROL���g�p���Ă���A�ʂ̒ʐM���N�����Ă���Ȃǂ̗��R��
 * �r�[�R���T�[�`�̋N�����֎~�������}�b�v�̔�����s���Ă���B
 *
 * @todo  �]�[��ID�̗񋓂łȂ��A�}�b�v�Ǘ��\�ł̎w��ȂǊO������͍�����
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsFieldBeaconNG(u16 zone_id)
{
  return ( ZONEDATA_IsUnionRoom(zone_id)
      || ZONEDATA_IsColosseum(zone_id)
      || ZONEDATA_IsPlBoat(zone_id)
      || ( zone_id == ZONE_ID_C04R0901 )          //�g���C�A���n�E�X
      );
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
      || (ZONEDATA_IsBingo(zone_id) == TRUE)){      //�r���S�}�b�v
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
  if(zone_id >= ZONE_ID_PALACE02 && zone_id <= ZONE_ID_PALACE10){
    return TRUE;
  }
  return FALSE;
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
  return ((zone_id == ZONE_ID_WC10) || (zone_id == ZONE_ID_BC10) || (zone_id == ZONE_ID_PLC10) || (zone_id == ZONE_ID_PLCW10));
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
  return (zone_id == ZONE_ID_C04R0202)||(zone_id == ZONE_ID_C04R0201);
}

//------------------------------------------------------------------
/**
 * @brief  �t�B�[���h�Z���N�����Ă����]�[���̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @return BOOL �t�B�[���h�Z���g����]�[���Ȃ�TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_CheckFieldSkillUse(u16 zone_id)
{
  if( ZONEDATA_IsUnionRoom(zone_id) ||
      ZONEDATA_IsColosseum(zone_id) ||
      ZONEDATA_IsPalaceField(zone_id)){
    return FALSE;
  }
  return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief �O���[�v�̑�\�ƂȂ�]�[��ID���擾����
 * @param  zoneid �]�[���w��ID
 * @return  �]�[��ID
 *
 * @note  �^�E���}�b�v��ł̕\���ʒu�������]�[����ID��Ԃ��B
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetGroupID( u16 zone_id)
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->zone_group;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->placename_id;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->placename_flag;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return (zoneData->matrix_id == NARC_map_matrix_wb_mat_bin);
}

//------------------------------------------------------------------
/**
 * @brief �Q�̎w��]�[�����N�����Ă����Ԃ�
 *        �\�������}�g���b�N�X�Ɣ���(�p�������Ă���)���Ă��悢���𔻒�
 * @param  zone_id_a ���肷��]�[����ID
 * @param  zone_id_b ���肷��]�[����ID
 * @return  BOOL TRUE�̂Ƃ��A�����}�g���b�N�X�w��
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsIntrudeSameMatrixID(u16 zone_id_a, u16 zone_id_b)
{
  u16 matrix_a, matrix_b;
  
  matrix_a = ZONEDATA_GetMatrixID(zone_id_a);
  matrix_b = ZONEDATA_GetMatrixID(zone_id_b);

  if(matrix_a == matrix_b){
    return TRUE;
  }
  
  if((matrix_a == NARC_map_matrix_wb_mat_bin && matrix_b == NARC_map_matrix_palacefield_mat_bin)
      || (matrix_a == NARC_map_matrix_palacefield_mat_bin && matrix_b == NARC_map_matrix_wb_mat_bin)){
    return TRUE;
  }
  
  return FALSE;
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return (zoneData->enc_data_id);
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return (zoneData->event_data_id);
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return (zoneData->battle_bg_type);
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

  switch( zone_id )
  {
  case ZONE_ID_C04:
    return NARC_grid_camera_scene_camera_scene_C04_dat;
  case ZONE_ID_T05:
    return NARC_grid_camera_scene_camera_scene_T05_dat;
  case ZONE_ID_R04:
    return NARC_grid_camera_scene_camera_scene_R04_dat;
  case ZONE_ID_H04:
    return NARC_grid_camera_scene_camera_scene_H04_dat;
  case ZONE_ID_C09P02:
    return NARC_grid_camera_scene_camera_scene_C09P02_dat;
  }
  return ZONEDATA_NO_SCENEAREA_ID;
}

//-----------------------------------------------------------------------------
/**
 * @brief �}�b�v�؂�ւ��^�C�v�̎擾
 *
 * @param zone_id �]�[��ID
 *
 * @return �w�肵���]�[���̃}�b�v�؂�ւ��^�C�v ( MC_TYPE_xxxx )
 */
//-----------------------------------------------------------------------------
u8 ZONEDATA_GetMapChangeType(u16 zone_id)
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->map_change_type;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �C�ꂩ�H
 */
//-----------------------------------------------------------------------------
BOOL ZONEDATA_IsSeaTemple( u16 zone_id )
{
  switch( zone_id ){
  case ZONE_ID_D20R0101:
  case ZONE_ID_D20R0201:
  case ZONE_ID_D20R0301:
  case ZONE_ID_D20R0401:
  case ZONE_ID_D20R0501:
  case ZONE_ID_D20R0601:
  case ZONE_ID_D20R0701:
  case ZONE_ID_D20R0801:
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------
// �C��_�a�@�_���W�����}�b�v���H
//------------------------------------------------------------------
BOOL ZONEDATA_IsSeaTempleDungeon( u16 zone_id )
{
  switch( zone_id ){
  case ZONE_ID_D20R0501:
  case ZONE_ID_D20R0601:
  case ZONE_ID_D20R0701:
  case ZONE_ID_D20R0801:
    return TRUE;
  }

  return FALSE;
}

// 2F���H
BOOL ZONEDATA_IsSeaTempleDungeon2F( u16 zone_id )
{
  if( zone_id == ZONE_ID_D20R0601 ){
    return TRUE;
  }
  return FALSE;
}
// 3F���H
BOOL ZONEDATA_IsSeaTempleDungeon3F( u16 zone_id )
{
  if( zone_id == ZONE_ID_D20R0701 ){
    return TRUE;
  }
  return FALSE;
}


//------------------------------------------------------------------
// C04�Đ�{�݂��H
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  C04�Đ�{�݁@�`�F�b�N
 *
 *	@param	zone_id   
 *
 *	@retval TRUE    �͂�
 *	@retval FALSE   ������
 */
//-----------------------------------------------------------------------------
BOOL ZONEDATA_IsC04RebattleZone( u16 zone_id )
{
  if( (zone_id == ZONE_ID_C04R0303) ||
      (zone_id == ZONE_ID_C04R0304) ||
      (zone_id == ZONE_ID_C04R0305) ||
      (zone_id == ZONE_ID_C04R0403) ||
      (zone_id == ZONE_ID_C04R0404) ){
    return TRUE;
  }
  return FALSE;
}



//------------------------------------------------------------------
/**
 * @brief  �n��ID(ZoneID�ł͂Ȃ�PlaceNameId)���p���X�̂��̂��H
 * @param  PlaceNameId �n��ID
 * @return �p���X���ǂ����H
 */
//------------------------------------------------------------------
const BOOL ZONEDATA_CheckPlaceNameID_IsPalace(const u16 planeNameId)
{
  if( planeNameId >= MAPNAME_P_C4RAIMON &&
      planeNameId < msg_place_name_max )
  {
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
// �a�����ɓV�C���͂��ZONE���H
//------------------------------------------------------------------
BOOL ZONEDATA_IsBirthDayWeatherChange( u16 zone_id )
{
  if( (zone_id == ZONE_ID_R15) || (zone_id == ZONE_ID_R14) ){
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const char * ZONEDATA_GetAllZoneName(HEAPID heapID)
{
  char * namedata;
  if( data_handle != NULL )
  {
    namedata = GFL_ARC_LoadDataAllocByHandle(
        data_handle->handle, NARC_result_zonedata_zonename_bin, heapID);
  }
  else
  {
    namedata = GFL_ARC_LoadDataAlloc(
        ARCID_ZONEDATA, NARC_result_zonedata_zonename_bin, heapID);
  }
  return namedata;
}

//------------------------------------------------------------------
/**
 * @brief �]�[�����f�[�^�̎擾
 * @param buffer    ���O���擾���邽�߂̃o�b�t�@(ZONEDATA_NAME_LENGTH�̒������K�v�j
 * @param zoneid    �擾����]�[���̖��O
 */
//------------------------------------------------------------------
u8 ZONEDATA_GetMvMdlID(u16 zone_id)
{
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->movemodel_id;
}


//------------------------------------------------------------------
// D09�`�����s�I�����[�h�֌W��ZONE���H
//------------------------------------------------------------------
BOOL ZONEDATA_IsChampionLord(u16 zone_id)
{
  // @todo ZONEID���ς�����炱�����m�F����K�v������܂��B 2010.04.14
  switch( zone_id ){
  case ZONE_ID_D09:
  case ZONE_ID_D09R0101:
  case ZONE_ID_D09R0201:
  case ZONE_ID_D09R0301:
  case ZONE_ID_D09R0401:
  case ZONE_ID_D09R0501:
  case ZONE_ID_D09R0601:
  case ZONE_ID_D09R0701:
  case ZONE_ID_D09R0801:
  case ZONE_ID_D09R0901:
  case ZONE_ID_D09R1001:
  case ZONE_ID_D09R1101:
  case ZONE_ID_D09R1201:
  case ZONE_ID_D09R1301:
  case ZONE_ID_D09R1401:
  case ZONE_ID_D09R1501:
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//  �o�g���T�u�E�F�C�֘A�̃}�b�v��
//------------------------------------------------------------------
BOOL ZONEDATA_IsBattleSubway( u16 zone_id )
{
  zone_id = ControlZoneID(zone_id);
  
  switch( zone_id ){
  case ZONE_ID_C04R0102:
  case ZONE_ID_C04R0103:
  case ZONE_ID_C04R0104:
  case ZONE_ID_C04R0105:
  case ZONE_ID_C04R0106:
  case ZONE_ID_C04R0107:
  case ZONE_ID_C04R0108:
  case ZONE_ID_C04R0110:
  case ZONE_ID_C04R0111:
    return TRUE;
  }
  return FALSE;
}

//============================================================================================
//============================================================================================

#ifdef PM_DEBUG
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
        NARC_result_zonedata_zonename_bin,
        ZONEDATA_NAME_LENGTH * zone_id, ZONEDATA_NAME_LENGTH, buffer);
  }
  else
  {
    GFL_ARC_LoadDataOfs(buffer,
        ARCID_ZONEDATA, NARC_result_zonedata_zonename_bin,
        ZONEDATA_NAME_LENGTH * zone_id, ZONEDATA_NAME_LENGTH);
  }
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
  ZONEDATA* zoneData;
  zoneData = loadZoneData( zone_id );
  return zoneData->movemodel_id != 0;
}

#endif  //PM_DEBUG
