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

//-------------------------------------
/// �m�[�O���b�h�}�b�v�@���	�s��l
//=====================================
#define FLDNOGRID_RESISTDATA_NONE  ( 0xffff )

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
//  �ǂ݂��܂Ȃ��ꍇ  �FFLDNOGRID_RESISTDATA_NONE  
//=====================================
typedef struct 
{
  u16 railDataID;
  u16 areaDataID;
} FLDNOGRID_RESISTDATA;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


// �}�b�p�[�����E�j��
extern FLDNOGRID_MAPPER* FLDNOGRID_MAPPER_Create( u32 heapID, FIELD_CAMERA* p_camera, FLD_SCENEAREA* p_sceneArea, FLD_SCENEAREA_LOADER* p_sceneAreaLoader );
extern void FLDNOGRID_MAPPER_Delete( FLDNOGRID_MAPPER* p_mapper );


// �m�[�O���b�h�}�b�v�����̍X�V�𐧌�
// �S�̂̍X�V���X�g�b�v
extern void FLDNOGRID_MAPPER_SetActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag );
extern BOOL FLDNOGRID_MAPPER_IsActive( const FLDNOGRID_MAPPER* cp_mapper );
// �J�����̂݃X�g�b�v
extern void FLDNOGRID_MAPPER_SetRailCameraActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag );

// ���̐ݒ�A�j��
extern void FLDNOGRID_MAPPER_ResistData( FLDNOGRID_MAPPER* p_mapper, const FLDNOGRID_RESISTDATA* cp_data, u32 heapID );
extern void FLDNOGRID_MAPPER_ResistDataArc( FLDNOGRID_MAPPER* p_mapper, u32 dataID, u32 heapID );
extern void FLDNOGRID_MAPPER_Release( FLDNOGRID_MAPPER* p_mapper );
extern BOOL FLDNOGRID_MAPPER_IsResistData( const FLDNOGRID_MAPPER* cp_mapper );


// ���C���Ǘ�
extern void FLDNOGRID_MAPPER_Main( FLDNOGRID_MAPPER* p_mapper );

// ���[�����[�N�̎擾�E�ԋp
extern FIELD_RAIL_WORK* FLDNOGRID_MAPPER_CreateRailWork( FLDNOGRID_MAPPER* p_mapper );
extern void FLDNOGRID_MAPPER_DeleteRailWork( FLDNOGRID_MAPPER* p_mapper, FIELD_RAIL_WORK* p_railWork );

// ���C�����[�����[�N�̐ݒ�
extern void FLDNOGRID_MAPPER_BindCameraWork( FLDNOGRID_MAPPER* p_mapper, const FIELD_RAIL_WORK* cp_railWork );
extern void FLDNOGRID_MAPPER_UnBindCameraWork( FLDNOGRID_MAPPER* p_mapper );


// �A�g���r���[�g���̎擾
extern MAPATTR FLDNOGRID_MAPPER_GetAttr( const FLDNOGRID_MAPPER* cp_mapper, const RAIL_LOCATION* cp_location );


// ���[�����C������̊Ǘ�
// flag TRUE:����\�@�@FALSE�F����s�\
extern void FLDNOGRID_MAPPER_SetLineActive( FLDNOGRID_MAPPER* p_mapper, u32 line_index, BOOL flag );
extern BOOL FLDNOGRID_MAPPER_GetLineActive( const FLDNOGRID_MAPPER* cp_mapper, u32 line_index );


// �e���W���[���擾
extern const FIELD_RAIL_MAN* FLDNOGRID_MAPPER_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper );
extern const FLD_SCENEAREA* FLDNOGRID_MAPPER_GetSceneAreaMan( const FLDNOGRID_MAPPER* cp_mapper );
extern const FIELD_RAIL_LOADER* FLDNOGRID_MAPPER_GetRailLoader( const FLDNOGRID_MAPPER* cp_mapper );
extern const FLD_SCENEAREA_LOADER* FLDNOGRID_MAPPER_GetSceneAreaLoader( const FLDNOGRID_MAPPER* cp_mapper );
extern const RAIL_ATTR_DATA* FLDNOGRID_MAPPER_GetRailAttrData( const FLDNOGRID_MAPPER* cp_mapper );


// �f�o�b�N�p
#ifdef PM_DEBUG
extern void FLDNOGRID_MAPPER_DEBUG_LoadRailBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size );
extern void FLDNOGRID_MAPPER_DEBUG_LoadAreaBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size );
extern void FLDNOGRID_MAPPER_DEBUG_LoadAttrBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size, u32 heapID );


extern FIELD_RAIL_MAN* FLDNOGRID_MAPPER_DEBUG_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper );

#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



