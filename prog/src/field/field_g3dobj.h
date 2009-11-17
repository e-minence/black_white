//======================================================================
/**
 * @file	field_g3dobj.h
 * @brief �t�B�[���h
 * @author
 * @date
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "arc_def.h"

//======================================================================
//  define 
//======================================================================
#define FLD_G3DOBJ_IDX_ERROR (0xffff)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// �C���f�b�N�X�^
//--------------------------------------------------------------
typedef u16 FLD_G3DOBJ_RESIDX; ///<���\�[�X�C���f�b�N�X
typedef u16 FLD_G3DOBJ_OBJIDX; ///<�I�u�W�F�N�g�C���f�b�N�X

//--------------------------------------------------------------
/// FLD_G3DOBJ_CTRL
//--------------------------------------------------------------
typedef struct _TAG_FLD_G3DOBJ_CTRL FLD_G3DOBJ_CTRL;

//--------------------------------------------------------------
/// FLD_G3DMDL_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  ARCHANDLE *handle;
  u32 arcIdx;
}FLD_G3DMDL_ARCIDX;

//--------------------------------------------------------------
/// FLD_G3DTEX_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  ARCHANDLE *handle;
  u32 arcIdx;
}FLD_G3DTEX_ARCIDX;

//--------------------------------------------------------------
/// FLD_G3DTEX_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  ARCHANDLE *handle;
  u32 *arcIdxTbl;
  u32 count;
}FLD_G3DANM_ARCIDX;

//======================================================================
//  extern
//======================================================================
extern FLD_G3DOBJ_CTRL * FLD_G3DOBJ_CTRL_Create(
    HEAPID heapID, u16 res_max, u16 obj_max );
extern void FLD_G3DOBJ_CTRL_Delete( FLD_G3DOBJ_CTRL *ctrl );
extern void FLD_G3DOBJ_CTRL_Trans( FLD_G3DOBJ_CTRL *ctrl );
extern void FLD_G3DOBJ_CTRL_Draw( FLD_G3DOBJ_CTRL *ctrl );
extern u16 FLD_G3DOBJ_CTRL_CreateResource(
    FLD_G3DOBJ_CTRL *ctrl, 
    const FLD_G3DMDL_ARCIDX *mdl,
    const FLD_G3DTEX_ARCIDX *tex,
    const FLD_G3DANM_ARCIDX *anm,
    BOOL transFlag );
extern void FLD_G3DOBJ_CTRL_DeleteResource(FLD_G3DOBJ_CTRL *ctrl, u16 idx);
extern u16 FLD_G3DOBJ_CTRL_AddObject(
    FLD_G3DOBJ_CTRL *ctrl, const u16 resIdx, u16 mdlIdx );
extern void FLD_G3DOBJ_DeleteObject( FLD_G3DOBJ_CTRL *ctrl, u16 idx );
extern GFL_G3D_OBJ * FLD_G3DOBJ_CTRL_GetObject( FLD_G3DOBJ_CTRL *ctrl, u16 idx );
extern GFL_G3D_OBJSTATUS * FLD_G3DOBJ_CTRL_GetObjStatus(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx );
extern void FLD_G3DOBJ_CTRL_SetObjPos(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, const VecFx32 *pos );
extern void FLD_G3DOBJ_CTRL_SetObjCullingFlag(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, BOOL flag );
extern void FLD_G3DOBJ_CTRL_SetObjVanishFlag(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx, BOOL flag );
extern BOOL FLD_G3DOBJ_CTRL_LoopAnimeObject(
    FLD_G3DOBJ_CTRL *ctrl, u16 idx );
