//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_g3dmap_exwork.h
 *	@brief	field_g3d_mapper��GFL_G3D_MAP�g�����[�N�Ǘ�����
 *	@author	tomoya takahashi
 *	@date		2009.05.01
 *
 *	���W���[�����FFLD_G3D_MAP_EXWORK
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "field_ground_anime.h"
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
///	GFL_G3D_MAP�g�����[�N
//=====================================
typedef struct _FLD_G3D_MAP_EXWORK FLD_G3D_MAP_EXWORK;

	
//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �g�����[�N����
extern BOOL FLD_G3D_MAP_EXWORK_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk );
extern FIELD_GRANM_WORK* FLD_G3D_MAP_EXWORK_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk );

// �z�u���f������
extern FIELD_BMODEL_MAN* FLD_G3D_MAP_EXWORK_GetBModelMan( const FLD_G3D_MAP_EXWORK* cp_wk );

// �}�b�v�C���f�b�N�X�擾
const u32 FLD_G3D_MAP_EXWORK_GetMapIndex( const FLD_G3D_MAP_EXWORK* cp_wk );

#ifdef _cplusplus
}	// extern "C"{
#endif



