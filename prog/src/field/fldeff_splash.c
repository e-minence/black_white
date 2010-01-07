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
  
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RES *g3d_res_anm;
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
  
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm;
  GFL_G3D_RND *obj_rnd;
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
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( splash->fectrl );
  
  splash->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_shibuki_nsbmd );
  
  GFL_G3D_TransVramTexture( splash->g3d_res_mdl );

  splash->g3d_res_anm	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_shibuki_nsbtp );
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
 	GFL_G3D_DeleteResource( splash->g3d_res_anm );
 	GFL_G3D_DeleteResource( splash->g3d_res_mdl );
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
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->head.eff_splash->g3d_res_mdl, 0,
        work->head.eff_splash->g3d_res_mdl );
  
  work->obj_anm =
    GFL_G3D_ANIME_Create(
        work->obj_rnd, work->head.eff_splash->g3d_res_anm, 0 );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, &work->obj_anm, 1 );
  GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );

  {
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    pos.z += SPLASH_DRAW_Z_OFFSET;
    FLDEFF_TASK_SetPos( task, &pos );
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
  GFL_G3D_ANIME_Delete( work->obj_anm );
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
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
  
  if( GFL_G3D_OBJECT_LoopAnimeFrame(work->obj,0,FX32_ONE) == FALSE ){
    if( work->head.joint == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
  }
  
  if( work->head.joint == TRUE ){
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    pos.z += SPLASH_DRAW_Z_OFFSET;
    FLDEFF_TASK_SetPos( task, &pos );
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
  VecFx32 pos;
  TASKWORK_SPLASH *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
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
