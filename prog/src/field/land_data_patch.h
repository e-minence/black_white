//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		land_data_patch.h
 *	@brief  �}�b�v�̊�{�I�ȃA�g���r���[�g�A�z�u�f�[�^�����������A�ǉ��ݒ�
 *	@author	tomoya takahashi
 *	@date		2009.11.30
 *
 *	���W���[�����FFIELD_LAND_DATA_PATCH
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "arc/fieldmap/land_data_patch_def.h"

#include "map/dp3format.h"

#include "field_g3d_map.h"
#include "field_buildmodel.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	  LAND_DATA�p�b�`���
//=====================================
typedef struct _FIELD_DATA_PATCH FIELD_DATA_PATCH;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �����E�j��
extern FIELD_DATA_PATCH* FIELD_DATA_PATCH_Create( u32 data_id, HEAPID heapID );
extern void FIELD_DATA_PATCH_Delete( FIELD_DATA_PATCH* p_sys );

// �A�g���r���[�g�㏑��
extern void FIELD_DATA_PATCH_OverWriteAttr( const FIELD_DATA_PATCH* cp_sys, NormalVtxFormat* p_buff, u32 grid_x, u32 grid_z );
extern void FIELD_DATA_PATCH_OverWriteAttrEx( const FIELD_DATA_PATCH* cp_sys, NormalVtxFormat* p_buff, u32 read_grid_x, u32 read_grid_z, u32 write_grid_x, u32 write_grid_z, u32 size_grid_x, u32 size_grid_z );

// �z�u���f���ݒ�
extern int FIELD_LAND_DATA_PATCH_AddBuildModel( const FIELD_DATA_PATCH* cp_sys, FIELD_BMODEL_MAN * p_bmodel, FLD_G3D_MAP * g3Dmap, int count_start, u32 grid_x, u32 grid_z );

#ifdef _cplusplus
}	// extern "C"{
#endif



