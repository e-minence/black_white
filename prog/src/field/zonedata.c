//============================================================================================
/**
 * @file	zonedata.c
 * @brief	ゾーン別データ
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

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	範囲チェック
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
	CHECK_RANGE(zone_id);	//範囲外チェック
	GFL_ARC_LoadDataOfs(zdbuf,
			ARCID_ZONEDATA, NARC_zonedata_zonetable_bin,
			sizeof(ZONEDATA) * zone_id, sizeof(ZONEDATA));
	return zdbuf;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
u16 ZONEDATA_GetAreaID(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.area_id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u16 ZONEDATA_GetMatrixID(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.matrix_id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u16 ZONEDATA_GetMapRscID(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.maprsc_id;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void ZONEDATA_DEBUG_GetStartPos(u16 zone_id, VecFx32 * pos)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	pos->x = zdbuf.sx * FX32_ONE;
	pos->y = zdbuf.sy * FX32_ONE;
	pos->z = zdbuf.sz * FX32_ONE;
	OS_Printf("x,y,z=%d,%d,%d\n",pos->x, pos->y, pos->z);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u8 ZONEDATA_GetCameraID(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.camera_id;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL ZONEDATA_DEBUG_GetSampleObject(u16 zone_id)
{
	ZONEDATA zdbuf;
	getZoneData(&zdbuf, zone_id);
	return zdbuf.movemodel_id != 0;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void ZONEDATA_GetZoneName(HEAPID heapID, char * buffer, u16 zone_id)
{
	CHECK_RANGE(zone_id);	//範囲外チェック
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




