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
  u8 edgemark_type;      ///<�G�b�W�}�[�L���O�w��
  u8 bbd_color;        ///< �r���{�[�h���f���̐F�w��
};

//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  AREA_ID_OUT_MAX = AREA_ID_IN01,
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u16 check_area_range(u16 area_id)
{
	GF_ASSERT(area_id < AREA_ID_MAX);
	if (area_id < AREA_ID_MAX) {
		return area_id;
	} else {
		return 0;
	}
}
#define	CHECK_AREA_RANGE(value)	{value = check_area_range(value);}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetAreaIDMax(void)
{
	return AREA_ID_MAX;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void dumpAreaData(const AREADATA * areadata)
{
  OS_Printf("sizeof AREADATA=%d\n", sizeof(AREADATA) );
	OS_Printf("model_set_index=%04x ",areadata->model_set_index);		//�z�u���f���Z�b�g�w��
	OS_Printf("tex_set_index=%04x ",areadata->tex_set_index);			//�e�N�X�`���Z�b�g�w��
	OS_Printf("anm_ita_id=%02x ",areadata->anm_ita_id);	  		//�n�`ITA�A�j���Z�b�g�w��
  OS_Printf("anm_itp_id=%02x \n",areadata->anm_itp_id);	  		//�n�`ITP�A�j���Z�b�g�w��
	OS_Printf("inner_outer=%02x ",areadata->inner_outer);			//����ǉ��F�z�u���f����ގw��i�����E���O�j
	OS_Printf("light_type=%02x ",areadata->light_type);				//���C�g�w��
  OS_Printf("edgemark_type=%02x\n",areadata->edgemark_type);      ///<�G�b�W�}�[�L���O�w��
  OS_Printf("bbd_color=%02x\n",areadata->bbd_color);      ///<�r���{�[�h���f���F�w��
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   �G���A�f�[�^�̎擾
 * @param   heapID
 * @return  AREADATA
 */
//------------------------------------------------------------------
AREADATA * AREADATA_Create(HEAPID heapID, u16 area_id, u32 season_id)
{
  AREADATA * areadata;
  if (AREADATA_HasSeason(area_id))
  {
    area_id += season_id;
  }
  CHECK_AREA_RANGE(area_id);
  areadata = GFL_FILE_LoadDataAllocOfs(
      ARCID_AREADATA,
      heapID,
      sizeof(AREADATA) * area_id,
      sizeof(AREADATA) );
  dumpAreaData(areadata);
  return areadata;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void AREADATA_Delete(AREADATA * areadata)
{
  GFL_HEAP_FreeMemory( areadata );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetModelSetID(const AREADATA * areadata)
{
  return areadata->model_set_index;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 AREADATA_GetTextureSetID(const AREADATA * areadata)
{
  return areadata->tex_set_index;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u32 AREADATA_GetGroundITAID(const AREADATA * areadata)
{
  if (areadata->anm_ita_id == 0xff )
  {
    return AREADATA_NO_ANIME_DATA;
  }
  else
  {
    return areadata->anm_ita_id;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u32 AREADATA_GetGroundITPID(const AREADATA * areadata)
{
  if (areadata->anm_itp_id == 0xff)
  {
    return AREADATA_NO_ANIME_DATA;
  }
  else
  {
    return areadata->anm_itp_id;
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u8 AREADATA_GetInnerOuterSwitch(const AREADATA * areadata)
{
  return areadata->inner_outer;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u8 AREADATA_GetLightType(const AREADATA * areadata)
{
  return areadata->light_type;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u8 AREADATA_GetEdgeMarkingType(const AREADATA * areadata)
{
  return areadata->edgemark_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h���f���J���[
 */
//-----------------------------------------------------------------------------
u8 AREADATA_GetBBDColor(const AREADATA * areadata)
{
  return areadata->bbd_color;
}

//------------------------------------------------------------------
//�l�G�̂���/�Ȃ�
//------------------------------------------------------------------
BOOL AREADATA_HasSeason(u16 area_id)
{
	if (AREA_ID_OUT01 <= area_id && area_id < AREA_ID_OUT_MAX) return TRUE;
	return FALSE;
}

//------------------------------------------------------------------
//�l�G�̂���ꏊ�ŁA�ω��������ȏꏊ�������łȂ����H
//------------------------------------------------------------------
BOOL AREADATA_HasSeasonDiff(u16 area_id)
{
  if( !AREADATA_HasSeason(area_id)){
    return FALSE;
  }
	if (AREA_ID_OUT09 >= area_id && area_id <= AREA_ID_OUT11) return TRUE;
  if (AREA_ID_OUT51 == area_id ) return TRUE;
	return FALSE;
}

