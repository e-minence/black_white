//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_nogrid_mapper.h
 *	@brief  �m�[�O���b�h����@�}�b�p�[
 *	@author	tomoya takahashi
 *	@date		2009.08.25
 *
 *	���W���[�����FFLDNOGRID_MAPPER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_rail.h"
#include "field/field_rail_loader.h"
#include "fld_scenearea.h"
#include "fld_scenearea_loader.h"
#include "rail_attr.h"


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
///	�m�[�O���b�h����}�b�p�[���[�N
//=====================================
typedef struct _FLDNOGRID_MAPPER FLDNOGRID_MAPPER;



//-------------------------------------
/// �m�[�O���b�h�}�b�v�@���	
//=====================================
typedef struct 
{
  u16 railDataID;
  u16 areaDataID;
  u16 attrDataID;

  u16 pad;
} FLDNOGRID_RESISTDATA;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


// �}�b�p�[�����E�j��
extern FLDNOGRID_MAPPER* FLDNOGRID_MAPPER_Create( u32 heapID, FIELD_CAMERA* p_camera );
extern void FLDNOGRID_MAPPER_Delete( FLDNOGRID_MAPPER* p_mapper );


// ���̐ݒ�A�j��
extern void FLDNOGRID_MAPPER_ResistData( FLDNOGRID_MAPPER* p_mapper, const FLDNOGRID_RESISTDATA* cp_data, u32 heapID );
extern void FLDNOGRID_MAPPER_Release( FLDNOGRID_MAPPER* p_mapper );


// ���C���Ǘ�
extern void FLDNOGRID_MAPPER_Main( FLDNOGRID_MAPPER* p_mapper );


// ���[�����[�N�̎擾�E�ԋp
extern FIELD_RAIL_WORK* FLDNOGRID_MAPPER_CreateRailWork( FLDNOGRID_MAPPER* p_mapper );
extern void FLDNOGRID_MAPPER_DeleteRailWork( FLDNOGRID_MAPPER* p_mapper, FIELD_RAIL_WORK* p_railWork );


// �A�g���r���[�g���̎擾
extern MAPATTR FLDNOGRID_MAPPER_GetAttr( const FLDNOGRID_MAPPER* cp_mapper, const RAIL_LOCATION* cp_location );


// �e���W���[���擾
extern const FIELD_RAIL_MAN* FLDNOGRID_MAPPER_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper );
extern const FLD_SCENEAREA* FLDNOGRID_MAPPER_GetSceneAreaMan( const FLDNOGRID_MAPPER* cp_mapper );
extern const FIELD_RAIL_LOADER* FLDNOGRID_MAPPER_GetRailLoader( const FLDNOGRID_MAPPER* cp_mapper );
extern const FLD_SCENEAREA_LOADER* FLDNOGRID_MAPPER_GetSceneAreaLoader( const FLDNOGRID_MAPPER* cp_mapper );



#ifdef _cplusplus
}	// extern "C"{
#endif



