//============================================================================================
/**
 * @file	mapdata_attr.h
 *
 * @brief	�}�b�v�f�[�^�A�g���r���[�g�E�����擾�֐�
 *
 *
 *
 */
//============================================================================================
#pragma once
#include "../field_g3d_map.h"   //for FLD_G3D_MAP_ATTRINFO

extern void MAPDATA_ATR_GetAttrFunc( FLD_G3D_MAP_ATTRINFO* attrInfo, const u8 idx, const u32 attrAdrs,
    const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height );
