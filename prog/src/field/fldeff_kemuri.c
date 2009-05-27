//======================================================================
/**
 * @file	fldeff_kemuri.c
 * @brief	�t�B�[���h �y��
 * @author	kagaya
 * @data	05.07.13
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
  
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RES *g3d_res_anm;
  GFL_G3D_RND *g3d_rnd;
  GFL_G3D_ANM *g3d_anm;
};

//--------------------------------------------------------------
/// TASKWORK_KEMURI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_KEMURI *eff_kemuri;
  GFL_G3D_OBJ *obj;
}TASKWORK_KEMURI;

//--------------------------------------------------------------
/// TASKHEADER_KEMURI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_KEMURI *eff_kemuri;
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
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( kemu->fectrl );
  
  kemu->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_hero_kemu_nsbmd );
  GFL_G3D_TransVramTexture( kemu->g3d_res_mdl );

  kemu->g3d_res_anm	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_hero_kemu_nsbtp );
  
  kemu->g3d_rnd =
    GFL_G3D_RENDER_Create( kemu->g3d_res_mdl, 0, kemu->g3d_res_mdl );
  
  kemu->g3d_anm =
    GFL_G3D_ANIME_Create( kemu->g3d_rnd, kemu->g3d_res_anm, 0 );
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
  GFL_G3D_ANIME_Delete( kemu->g3d_anm );
	GFL_G3D_RENDER_Delete( kemu->g3d_rnd );
 	GFL_G3D_DeleteResource( kemu->g3d_res_anm );
 	GFL_G3D_DeleteResource( kemu->g3d_res_mdl );
}

//======================================================================
//	�y���@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�y���@�ǉ�
 * @param fmmdl FLDMMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_KEMURI_SetFldMMdl( FLDMMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  VecFx32 pos;
  FLDEFF_KEMURI *kemu;
  TASKHEADER_KEMURI head;
  
  FLDMMDL_GetVectorPos( fmmdl, &pos );
  pos.y += FX32_ONE*1;
  pos.z += FX32_ONE*12;
  
  kemu = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_KEMURI );
  head.eff_kemuri = kemu;
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
  work->eff_kemuri = head->eff_kemuri;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->eff_kemuri->g3d_rnd, &work->eff_kemuri->g3d_anm, 1 );
	GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );
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
  GFL_G3D_OBJECT_Delete( work->obj );
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
#if 1 
  if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,0,FX32_ONE) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
  }
#else
  if( GFL_G3D_OBJECT_LoopAnimeFrame(work->obj,0,FX32_ONE) == FALSE ){
//    FLDEFF_TASK_CallDelete( task );
  }
#endif
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
  VecFx32 pos;
  TASKWORK_KEMURI *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};

  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
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
