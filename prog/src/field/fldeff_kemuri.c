//======================================================================
/**
 * @file	fldeff_kemu.c
 * @brief	�t�B�[���h �y��
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_kemuri.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_KEMURI�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_KEMURI FLDEFF_KEMURI;

//--------------------------------------------------------------
///	FLDEFF_KEMURI�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_KEMURI
{
	FLDEFF_CTRL *fectrl;
  FLD_G3DOBJ_RESIDX res_idx;
};

//--------------------------------------------------------------
/// TASKWORK_KEMURI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_KEMURI *eff_kemu;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  FLD_G3DOBJ_OBJIDX obj_idx;
}TASKWORK_KEMURI;

//--------------------------------------------------------------
/// TASKHEADER_KEMURI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_KEMURI *eff_kemu;
  VecFx32 pos;
}TASKHEADER_KEMURI;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void kemuri_InitResource( FLDEFF_KEMURI *kemu );
static void kemuri_DeleteResource( FLDEFF_KEMURI *kemu );

static const FLDEFF_TASK_HEADER DATA_kemuriTaskHeader;

//======================================================================
//	�y���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �y�� ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_KEMURI_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_KEMURI *kemu;
	
	kemu = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_KEMURI) );
	kemu->fectrl = fectrl;
  
	kemuri_InitResource( kemu );
	return( kemu );
}

//--------------------------------------------------------------
/**
 * �y�� �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_KEMURI_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_KEMURI *kemu = work;
  kemuri_DeleteResource( kemu );
  GFL_HEAP_FreeMemory( kemu );
}

//======================================================================
//	�y���@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �y���@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuri_InitResource( FLDEFF_KEMURI *kemu )
{
  ARCHANDLE *handle;
  FLD_G3DOBJ_RES_HEADER head;
  FLD_G3DOBJ_CTRL *obj_ctrl;

  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( kemu->fectrl );
  handle = FLDEFF_CTRL_GetArcHandleEffect( kemu->fectrl );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl( &head, handle, NARC_fldeff_hero_kemu_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx( &head, NARC_fldeff_hero_kemu_nsbtp );
  
  kemu->res_idx = FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
}

//--------------------------------------------------------------
/**
 * �y���@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuri_DeleteResource( FLDEFF_KEMURI *kemu )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( kemu->fectrl );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, kemu->res_idx );
}

//======================================================================
//	�y���@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�y���@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_KEMURI_SetMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  VecFx32 pos;
  FLDEFF_KEMURI *kemu;
  TASKHEADER_KEMURI head;
  
  MMDL_GetVectorPos( fmmdl, &pos );
  pos.y += FX32_ONE*1;
  pos.z += FX32_ONE*6;
  
  kemu = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_KEMURI );
  head.eff_kemu = kemu;
  head.pos = pos;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_kemuriTaskHeader, NULL, 0, &head, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���[����̓��샂�f���p�y���@�@�ǉ�
 *
 *	@param	fmmdl   MMDL
 *	@param	fectrl  FLDEFF_CTRL
 *	@retval nothing
 *
 *	��{��GRID�ꏏ�ł����A�G�t�F�N�g�̕\���ʒu���J������YAW�ɂ��킹�܂��B
 */
//-----------------------------------------------------------------------------
void FLDEFF_KEMURI_SetRailMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  VecFx32 pos;
  FLDEFF_KEMURI *kemu;
  TASKHEADER_KEMURI head;
  u16 camera_yaw;
  const MMDLSYS * mmdlSys = MMDL_GetMMdlSys( fmmdl );

  camera_yaw = MMDLSYS_GetTargetCameraAngleYaw( mmdlSys );
  
  MMDL_GetVectorPos( fmmdl, &pos );
  pos.y += FX32_ONE;
  pos.z += FX_Mul( FX_CosIdx( camera_yaw ), FX32_ONE*6 );
  pos.x += FX_Mul( FX_SinIdx( camera_yaw ), FX32_ONE*6 );
  
  kemu = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_KEMURI );
  head.eff_kemu = kemu;
  head.pos = pos;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_kemuriTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * �y���^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuriTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_KEMURI *work = wk;
  const TASKHEADER_KEMURI *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->eff_kemu = head->eff_kemu;
  work->obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( work->eff_kemu->fectrl );
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj_idx = FLD_G3DOBJ_CTRL_AddObject(
      work->obj_ctrl, work->eff_kemu->res_idx, 0, &head->pos );
}

//--------------------------------------------------------------
/**
 * �y���^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuriTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_KEMURI *work = wk;
  FLD_G3DOBJ_CTRL_DeleteObject( work->obj_ctrl, work->obj_idx );
}

//--------------------------------------------------------------
/**
 * �y���^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuriTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_KEMURI *work = wk;
  
  if( FLD_G3DOBJ_CTRL_AnimeObject(
        work->obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
  }
}

//--------------------------------------------------------------
/**
 * �y���^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void kemuriTask_Draw( FLDEFF_TASK *task, void *wk )
{
#if 0 //field_g3dobj�𗘗p����
  VecFx32 pos;
  TASKWORK_KEMURI *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
#endif
}

//--------------------------------------------------------------
//  �y���^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_kemuriTaskHeader =
{
  sizeof(TASKWORK_KEMURI),
  kemuriTask_Init,
  kemuriTask_Delete,
  kemuriTask_Update,
  kemuriTask_Draw,
};
