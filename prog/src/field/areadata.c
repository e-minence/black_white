//============================================================================================
/**
 * @file	areadata.c
 * @date	2008.12.18
 * @author	tamada GAME FREAK inc.
 * @brief	�G���A�f�[�^�A�N�Z�X
 */
//============================================================================================
#include <gflib.h>

#include "field/areadata.h"

#include "system/file_util.h"
#include "arc/arc_def.h"
#include "arc/fieldmap/area_id.h"

#include "arc/fieldmap/area_map_ita.naix"
#include "arc/fieldmap/area_map_itp.naix"

//------------------------------------------------------------------
//------------------------------------------------------------------
struct AREADATA{
	u16 model_set_index;		//�z�u���f���Z�b�g�w��
	u16 tex_set_index;			//�e�N�X�`���Z�b�g�w��
	u8  anm_ita_id;	  		//�n�`ITA�A�j���Z�b�g�w��
  u8  anm_itp_id;	  		//�n�`ITP�A�j���Z�b�g�w��
	u8  inner_outer;			//����ǉ��F�z�u���f����ގw��i�����E���O�j
	u8 light_type;				//���C�g�w��
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u16 check_range(u16 area_id)
{
	GF_ASSERT(area_id < AREA_ID_MAX);
	if (area_id < AREA_ID_MAX) {
		return area_id;
	} else {
		return 0;
	}
}
#define	CHECK_RANGE(value)	{value = check_range(value);}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetAreaIDMax(void)
{
	return AREA_ID_MAX;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static AREADATA * loadZoneData(HEAPID heapID)
{
	AREADATA * buffer;
	buffer = GFL_FILE_LoadDataAlloc(ARCID_AREADATA, heapID);
	return buffer;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void getAreaData(AREADATA * adbuf, u16 area_id)
{
	CHECK_RANGE(area_id);
	GFL_FILE_LoadDataOfs(adbuf, ARCID_AREADATA,
			sizeof(AREADATA) * area_id, sizeof(AREADATA));
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetModelSetID(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
	return adbuf.model_set_index;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetTextureSetID(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
	return adbuf.tex_set_index;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u32 AREADATA_GetGroundITAID(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
  if (adbuf.anm_ita_id == 0xff) return AREADATA_NO_ANIME_DATA;
	return adbuf.anm_ita_id;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u32 AREADATA_GetGroundITPID(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
  if (adbuf.anm_itp_id == 0xff) return AREADATA_NO_ANIME_DATA;
	return adbuf.anm_itp_id;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u8 AREADATA_GetInnerOuterSwitch(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
	return adbuf.inner_outer;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u8 AREADATA_GetLightType(u16 area_id)
{
	AREADATA adbuf;
	getAreaData(&adbuf, area_id);
	return adbuf.light_type;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL AREADATA_HasSeason(u16 area_id)
{
	if (AREA_ID_OUT01 <= area_id && area_id < AREA_ID_OUT29) return TRUE;
	return FALSE;
}

