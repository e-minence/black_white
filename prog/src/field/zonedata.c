//============================================================================================
/**
 * @file	zonedata.c
 * @brief	�]�[���ʃf�[�^
 * @author	tamada GAME FREAK inc.
 * @date	08.11.12
 */
//============================================================================================

#include <gflib.h>

#include "arc/arc_def.h"
#include "arc/fieldmap/zonedata.naix"
#include "arc/fieldmap/zone_id.h"
#include "arc/fieldmap/map_matrix.naix"

#include "field/field_const.h"

#include "field/zonedata.h"
#include "zonetableformat.h"
#include "gamesystem/pm_season.h"

#include "arc/fieldmap/field_rail_setup.naix"  //NARC_field_rail_data_�`
#include "../../resource/fldmapdata/zonetable/header/maptype.h"


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�͈̓`�F�b�N
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
#define	CHECK_RANGE(value)	{value = check_range(value);}

//------------------------------------------------------------------
/**
 * @brief	�]�[���̍ő吔�̎擾
 * @return	u16	�ő吔
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
	buffer = GFL_ARC_LoadDataAlloc(ARCID_ZONEDATA, NARC_zonedata_zonetable_bin, heapID);
	return buffer;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static ZONEDATA * getZoneData(ZONEDATA * zdbuf, u16 zone_id)
{
	CHECK_RANGE(zone_id);	//�͈͊O�`�F�b�N
	GFL_ARC_LoadDataOfs(zdbuf,
			ARCID_ZONEDATA, NARC_zonedata_zonetable_bin,
			sizeof(ZONEDATA) * zone_id, sizeof(ZONEDATA));
	return zdbuf;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�G���A�w��̎擾
 * @param	zone_id	�]�[���w��ID
 * @return	u16	�G���A�w��l
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
 * @brief	�}�b�v�A�[�J�C�u�w��̎擾
 * @param	zone_id	�]�[���w��ID
 * @return	u8		map archive�w��
 *
 */
//------------------------------------------------------------------
u8 ZONEDATA_GetMapArc(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.matrix_arc;
}
//------------------------------------------------------------------
/**
 * @brief	�}�g���b�N�X�w��̎擾
 * @param	zone_id	�]�[���w��ID
 * @return	u16	�}�g���b�N�X�w��l
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
 * @brief	�}�b�v�\���w��̎擾
 * @param	zone_id	�]�[���w��ID
 * @return	u16	�}�b�v�\���w��l
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
 * @param	zone_id	�]�[���w��ID
 * @return	u16	�X�N���v�g�A�[�J�C�u�ł̎w��ID
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
 * @param	zone_id	�]�[���w��ID
 * @return	u16	�X�N���v�g�A�[�J�C�u�ł̎w��ID
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
 * @param	zone_id	�]�[���w��ID
 * @return	u16	���b�Z�[�W�A�[�J�C�u�ł̎w��ID
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
 * @brief	�}�b�v�J�n�ʒu�̎擾�i�f�o�b�O�p�j
 * @param	zone_id	�]�[���w��ID
 * @param	pos		�J�n�ʒu���󂯎�邽�߂�fx32�^�|�C���^
 */
//------------------------------------------------------------------
void ZONEDATA_DEBUG_GetStartPos(u16 zone_id, VecFx32 * pos)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	pos->x = zdbuf.sx * FX32_ONE * FIELD_CONST_GRID_SIZE;
	pos->y = zdbuf.sy * FX32_ONE * FIELD_CONST_GRID_SIZE;
	pos->z = zdbuf.sz * FX32_ONE * FIELD_CONST_GRID_SIZE;
	TAMADA_Printf("%s x,y,z=%d,%d,%d\n",__FILE__,pos->x, pos->y, pos->z);
}

//------------------------------------------------------------------
/**
 * @brief	�J�����w��ID�̎擾
 * @param	zone_id	�]�[���w��ID
 * @return	u8		�J�����w��ID�̒l
 */
//------------------------------------------------------------------
u8 ZONEDATA_GetCameraID(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.camera_id;
}
//------------------------------------------------------------------
/**
 * @brief	BGM�w��ID�̎擾
 * @param	zone_id		�]�[���w��ID
 * @param	season_id	�G�ߎw��ID
 * @return	u8		BGM�w��ID�̒l
 */
//------------------------------------------------------------------
u16 ZONEDATA_GetBGMID(u16 zone_id, u8 season_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	switch (season_id) {
	case PMSEASON_SPRING:
		return zdbuf.bgm_spring_id;
	case PMSEASON_SUMMER:
		return zdbuf.bgm_summer_id;
	case PMSEASON_AUTUMN:
		return zdbuf.bgm_autumn_id;
	case PMSEASON_WINTER:
		return zdbuf.bgm_winter_id;
	}
	GF_ASSERT(season_id < PMSEASON_TOTAL);
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief �V��w��ID�̎擾
 * @param	zone_id	�]�[���w��ID
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
/**
 * @brief	����ȃT���v��OBJ���g�p���邩�ǂ����̐ݒ�擾
 * @param	zone_id	�]�[���w��ID
 * @param	BOOL	TRUE�̂Ƃ��A�T���v��OBJ���g�p����
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
  switch (zone_id)
  {
  case ZONE_ID_C03:
    return NARC_field_rail_setup_C03_dat;
  case ZONE_ID_H01:
    return NARC_field_rail_setup_H01_dat;
  case ZONE_ID_C03P02:
    return NARC_field_rail_setup_C03P02_dat;
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
  return (zone_id == ZONE_ID_UNION);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL ZONEDATA_IsColosseum(u16 zone_id)
{
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
 * @brief  �~���[�W�J���ҍ������ǂ����̃`�F�b�N
 * @param  zoneid �]�[���w��ID
 * @return BOOL �~���[�W�J���ҍ�����������TRUE
 */
//------------------------------------------------------------------
BOOL ZONEDATA_IsMusicalWaitingRoom(u16 zone_id)
{
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


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�]�[�����f�[�^�̎擾
 * @param	buffer		���O���擾���邽�߂̃o�b�t�@(ZONEDATA_NAME_LENGTH�̒������K�v�j
 * @param	zoneid		�擾����]�[���̖��O
 */
//------------------------------------------------------------------
void ZONEDATA_DEBUG_GetZoneName(char * buffer, u16 zone_id)
{
	CHECK_RANGE(zone_id);	//�͈͊O�`�F�b�N
	GFL_ARC_LoadDataOfs(buffer,
			ARCID_ZONEDATA, NARC_zonedata_zonename_bin,
			ZONEDATA_NAME_LENGTH * zone_id, ZONEDATA_NAME_LENGTH);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const char * ZONEDATA_GetAllZoneName(HEAPID heapID)
{
	char * namedata;
	namedata = GFL_ARC_LoadDataAlloc(
			ARCID_ZONEDATA, NARC_zonedata_zonename_bin, heapID);
	return namedata;
}




