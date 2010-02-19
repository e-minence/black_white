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
#define FLD_G3DOBJ_ANM_MAX (8)

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
/// FLD_G3DOBJ_RES_HEADER
//  �g�p���@
//  1:FLD_G3DOBJ_RES_HEADER���`
//  2:FLD_G3DOBJ_RES_HEADER_Init()�ŏ�����
//  3:FLD_G3DOBJ_RES_HEADER_SetMdl()�œǂݍ��ރ��f�����w�肷��
//  4:�e�N�X�`���p�̃��\�[�X������ꍇ��FLD_G3DOBJ_RES_HEADER_SetTex()���Ă�
//  5:�A�j�����w�肷��ۂ�FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle()��
//  �@�n���h�����Z�b�g���AFLD_G3DOBJ_RES_HEADER_SetAnmArcIdx()���ĂԁB
//--------------------------------------------------------------
typedef struct
{
  ARCHANDLE *arcHandleMdl; //���f�����\�[�X�擾�p�A�[�J�C�u�n���h��
  ARCHANDLE *arcHandleTex; //arcHandleMdl���e�N�X�`���Q�Ƃ̍ۂ�NULL
  ARCHANDLE *arcHandleAnm; //�A�j�����\�[�X�擾�p�A�[�J�C�u�n���h��
  u16 arcIdxMdl; //���f���p�A�[�J�C�u�C���f�b�N�X
  u16 arcIdxTex; //�e�N�X�`���p�A�[�J�C�u�C���f�b�N�X
  u16 arcIdxAnmTbl[FLD_G3DOBJ_ANM_MAX]; //�A�j���p�A�[�J�C�u�C���f�b�N�X
  u8 anmCount; //�A�j������
  u8 padding[3]; //byte rest
}FLD_G3DOBJ_RES_HEADER;
  
//======================================================================
//  extern
//======================================================================
extern FLD_G3DOBJ_CTRL * FLD_G3DOBJ_CTRL_Create(
    HEAPID heapID, u16 res_max, u16 obj_max );
extern void FLD_G3DOBJ_CTRL_Delete( FLD_G3DOBJ_CTRL *ctrl );
extern void FLD_G3DOBJ_CTRL_Trans( FLD_G3DOBJ_CTRL *ctrl );
extern void FLD_G3DOBJ_CTRL_Draw( FLD_G3DOBJ_CTRL *ctrl );
extern FLD_G3DOBJ_RESIDX FLD_G3DOBJ_CTRL_CreateResource(
    FLD_G3DOBJ_CTRL *ctrl,
    const FLD_G3DOBJ_RES_HEADER *head, BOOL transFlag );
extern void FLD_G3DOBJ_CTRL_DeleteResource(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_RESIDX idx);
extern u16 FLD_G3DOBJ_CTRL_AddObject( FLD_G3DOBJ_CTRL *ctrl,
    const u16 resIdx, u16 mdlIdx, const VecFx32 *pos );
extern void FLD_G3DOBJ_CTRL_DeleteObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx );
extern GFL_G3D_OBJ * FLD_G3DOBJ_CTRL_GetObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx );
extern GFL_G3D_OBJSTATUS * FLD_G3DOBJ_CTRL_GetObjStatus(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx );
extern void FLD_G3DOBJ_CTRL_SetObjPos(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, const VecFx32 *pos );
extern void FLD_G3DOBJ_CTRL_SetObjCullingFlag(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag );
extern void FLD_G3DOBJ_CTRL_SetObjVanishFlag(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, BOOL flag );
extern BOOL FLD_G3DOBJ_CTRL_AnimeObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, fx32 frame );
extern BOOL FLD_G3DOBJ_CTRL_LoopAnimeObject(
    FLD_G3DOBJ_CTRL *ctrl, FLD_G3DOBJ_OBJIDX idx, fx32 frame );

extern void FLD_G3DOBJ_RES_HEADER_Init( FLD_G3DOBJ_RES_HEADER *head );
extern void FLD_G3DOBJ_RES_HEADER_SetMdl(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle, u16 idx );
extern void FLD_G3DOBJ_RES_HEADER_SetTex(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle, u16 idx );
extern void FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle(
    FLD_G3DOBJ_RES_HEADER *head, ARCHANDLE *handle );
extern void FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
    FLD_G3DOBJ_RES_HEADER *head, u16 idx );
