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

#include "field/zonedata.h"
#include "zonetableformat.h"
#include "gamesystem/pm_season.h"

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
	pos->x = zdbuf.sx * FX32_ONE * 16;
	pos->y = zdbuf.sy * FX32_ONE * 16;
	pos->z = zdbuf.sz * FX32_ONE * 16;
	OS_Printf("x,y,z=%d,%d,%d\n",pos->x, pos->y, pos->z);
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
BOOL ZONEDATA_DEBUG_IsRailMap(u16 zone_id)
{
  return ( (zone_id == ZONE_ID_C03) || (zone_id == ZONE_ID_H01) );
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�]�[�����f�[�^�̎擾
 * @param	heapID		��Ɨp�q�[�v�̎w��
 * @param	buffer		���O���擾���邽�߂̃o�b�t�@(ZONEDATA_NAME_LENGTH�̒������K�v�j
 * @param	zoneid		�擾����]�[���̖��O
 */
//------------------------------------------------------------------
void ZONEDATA_GetZoneName(HEAPID heapID, char * buffer, u16 zone_id)
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




