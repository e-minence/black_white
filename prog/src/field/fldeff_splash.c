//======================================================================
/**
 * @file	fldeff_splash.c
 * @brief	�t�B�[���h ����
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_splash.h"

//======================================================================
//	define
//======================================================================
#define SPLASH_DRAW_Z_OFFSET (FX32_ONE*(8))

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_SPLASH�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_SPLASH FLDEFF_SPLASH;

//--------------------------------------------------------------
///	FLDEFF_SPLASH�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_SPLASH
{
	FLDEFF_CTRL *fectrl;
  FLD_G3DOBJ_RESIDX res_idx_splash;
  FLD_G3DOBJ_RESIDX res_idx_shoal;
};

//--------------------------------------------------------------
/// TASKHEADER_SPLASH
//--------------------------------------------------------------
typedef struct
{
  BOOL joint;
  FLDEFF_SPLASH *eff_splash;
  const MMDL *mmdl;
}TASKHEADER_SPLASH;

//--------------------------------------------------------------
/// TASKWORK_SPLASH
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  TASKHEADER_SPLASH head;
  MMDL_CHECKSAME_DATA samedata;
  FLD_G3DOBJ_OBJIDX obj_idx;
}TASKWORK_SPLASH;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void splash_InitResource( FLDEFF_SPLASH *splash );
static void splash_DeleteResource( FLDEFF_SPLASH *splash );

static const FLDEFF_TASK_HEADER DATA_splashTaskHeader;

//======================================================================
//	���򖗁@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ���� ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_SPLASH_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_SPLASH *splash;
	
	splash = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_SPLASH) );
	splash->fectrl = fectrl;
  
	splash_InitResource( splash );
	return( splash );
}

//--------------------------------------------------------------
/**
 * ���� �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_SPLASH_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_SPLASH *splash = work;
  splash_DeleteResource( splash );
  GFL_HEAP_FreeMemory( splash );
}

//======================================================================
//	���򖗁@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ���򖗁@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void splash_InitResource( FLDEFF_SPLASH *splash )
{
  ARCHANDLE *handle;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  FLD_G3DOBJ_RES_HEADER head;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( splash->fectrl );
  handle = FLDEFF_CTRL_GetArcHandleEffect( splash->fectrl );
  
  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
        &head, handle, NARC_fldeff_shibuki_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_shibuki_nsbtp );
  
  splash->res_idx_splash = 
      FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );

  FLD_G3DOBJ_RES_HEADER_Init( &head );
  FLD_G3DOBJ_RES_HEADER_SetMdl(
        &head, handle, NARC_fldeff_shibuki02_nsbmd );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
  FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx(
      &head, NARC_fldeff_shibuki02_nsbtp );
  
  splash->res_idx_shoal = 
      FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
}

//--------------------------------------------------------------
/**
 * ���򖗁@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void splash_DeleteResource( FLDEFF_SPLASH *splash )
{
  FLD_G3DOBJ_CTRL *obj_ctrl;
  
  obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( splash->fectrl );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, splash->res_idx_splash );
  FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, splash->res_idx_shoal );
}

//======================================================================
//	���򖗁@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p���򖗁@�ǉ�
 * @param FLDEFF_CTRL*
 * @param fmmdl MMDL
 * @param joint �ڑ��t���O TRUE=�ڑ��B
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_SPLASH_SetMMdl(
    FLDEFF_CTRL *fectrl, const MMDL *mmdl, BOOL joint )
{
  TASKHEADER_SPLASH head;
  
  head.joint = joint;
  head.eff_splash = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_SPLASH );
  head.mmdl = mmdl;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_splashTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ���򖗃^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void splashTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SPLASH *work = wk;
  const TASKHEADER_SPLASH *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  
  if( work->head.joint == TRUE ){
    MMDL_InitCheckSameData( work->head.mmdl, &work->samedata );
  }

  {
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    pos.z += SPLASH_DRAW_Z_OFFSET;
    FLDEFF_TASK_SetPos( task, &pos );
 
    {
      FLDEFF_SPLASH *splash = work->head.eff_splash;
      FLD_G3DOBJ_CTRL *obj_ctrl =
        FLDEFF_CTRL_GetFldG3dOBJCtrl( splash->fectrl );
      u16 idx = splash->res_idx_splash;

      if( work->head.joint == TRUE ){
        idx = splash->res_idx_shoal;
      }
      
      work->obj_idx = FLD_G3DOBJ_CTRL_AddObject( obj_ctrl, idx, 0, &pos );
    }
  }
  
//�����f����Ɛe��joint�t���O���Z�b�g����Ă��Ȃ���ԁB
//  FLDEFF_TASK_CallUpdate( task );
}

//--------------------------------------------------------------
/**
 * ���򖗃^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void splashTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SPLASH *work = wk;
  FLDEFF_SPLASH *splash = work->head.eff_splash;
  FLD_G3DOBJ_CTRL *obj_ctrl =
    FLDEFF_CTRL_GetFldG3dOBJCtrl( splash->fectrl );
  FLD_G3DOBJ_CTRL_DeleteObject( obj_ctrl, work->obj_idx );
}

//--------------------------------------------------------------
/**
 * ���򖗃^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void splashTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_SPLASH *work = wk;
  
  if( work->head.joint == TRUE ){
    if( MMDL_CheckSameData(work->head.mmdl,&work->samedata) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
  }
  
  if( work->head.joint == TRUE ){
    if( MMDL_CheckMoveBitShoalEffect(work->head.mmdl) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
  }

  {
    FLDEFF_SPLASH *splash = work->head.eff_splash;
    FLD_G3DOBJ_CTRL *obj_ctrl =
      FLDEFF_CTRL_GetFldG3dOBJCtrl( splash->fectrl );
    
    if( FLD_G3DOBJ_CTRL_LoopAnimeObject(
          obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
      if( work->head.joint == FALSE ){
        FLDEFF_TASK_CallDelete( task );
        return;
      }
    }
  
    if( work->head.joint == TRUE ){
      VecFx32 pos;
      GFL_G3D_OBJSTATUS *st = FLD_G3DOBJ_CTRL_GetObjStatus(
          obj_ctrl, work->obj_idx );
      MMDL_GetVectorPos( work->head.mmdl, &pos );
      pos.z += SPLASH_DRAW_Z_OFFSET;
      FLDEFF_TASK_SetPos( task, &pos );
      st->trans = pos;
    }
  }
}

//--------------------------------------------------------------
/**
 * ���򖗃^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void splashTask_Draw( FLDEFF_TASK *task, void *wk )
{
#if 0 //FLD_G3DOBJ�ɔC����
  VecFx32 pos;
  TASKWORK_SPLASH *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
#endif
}

//--------------------------------------------------------------
//  ���򖗃^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_splashTaskHeader =
{
  sizeof(TASKWORK_SPLASH),
  splashTask_Init,
  splashTask_Delete,
  splashTask_Update,
  splashTask_Draw,
};
