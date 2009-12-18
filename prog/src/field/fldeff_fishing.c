//======================================================================
/**
 * @file	fldeff_fishing.c
 * @brief	�t�B�[���h �ނ�C�x���g�p�G�t�F�N�g
 * @author	iwasawa
 * @date	09.12.18
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_fishing.h"

//======================================================================
//	define
//======================================================================
typedef enum{
  ANIME_ITP,
  ANIME_NUM
} ANIME_INDEX;

static u16 dat_id[ANIME_NUM] =
{
  NARC_fldeff_fishing_lure_nsbtp,
};


//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_FISHING_LURE�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_FISHING_LURE FLDEFF_FISHING_LURE;

//--------------------------------------------------------------
///	FLDEFF_FISHING_LURE�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_FISHING_LURE
{
	FLDEFF_CTRL *fectrl;
  
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RES *g3d_res_anm[ANIME_NUM];
};

//--------------------------------------------------------------
/// TASKWORK_FISHING_LURE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_FISHING_LURE *eff_lure;
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[ANIME_NUM];
  GFL_G3D_RND *obj_rnd;
}TASKWORK_FISHING_LURE;

//--------------------------------------------------------------
/// TASKHEADER_FISHING_LURE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_FISHING_LURE *eff_lure;
  VecFx32 pos;
}TASKHEADER_FISHING_LURE;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void fishing_lure_InitResource( FLDEFF_FISHING_LURE *wk );
static void fishing_lure_DeleteResource( FLDEFF_FISHING_LURE *wk );

static const FLDEFF_TASK_HEADER DATA_fishing_lure_TaskHeader;

//======================================================================
//	���A�[�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ���A�[ ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_FISHING_LURE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_FISHING_LURE *wk;
	
	wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_FISHING_LURE) );
	wk->fectrl = fectrl;
  
	fishing_lure_InitResource( wk );
	return( wk );
}

//--------------------------------------------------------------
/**
 * ���A�[ �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_FISHING_LURE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_FISHING_LURE *wk = work;
  fishing_lure_DeleteResource( wk );
  GFL_HEAP_FreeMemory( wk );
}

//======================================================================
//	���A�[�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ���A�[�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishing_lure_InitResource( FLDEFF_FISHING_LURE *wk )
{
  int i;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( wk->fectrl );
  
  wk->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_fishing_lure_nsbmd );
  GFL_G3D_TransVramTexture( wk->g3d_res_mdl );

  for( i=0; i<ANIME_NUM; i++ )
  {
    wk->g3d_res_anm[i] =
      GFL_G3D_CreateResourceHandle( handle, dat_id[i] );
  }
}

//--------------------------------------------------------------
/**
 * ���A�[�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishing_lure_DeleteResource( FLDEFF_FISHING_LURE *wk )
{
  int i;
  for( i=0; i<ANIME_NUM; i++ )
  {
    GFL_G3D_DeleteResource( wk->g3d_res_anm[i] );
  }
	GFL_G3D_FreeVramTexture( wk->g3d_res_mdl );
 	GFL_G3D_DeleteResource( wk->g3d_res_mdl );
}

//======================================================================
//	���A�[�@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���A�[�@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK* FLDEFF_FISHING_LURE_Set( FLDEFF_CTRL *fectrl, VecFx32* tpos )
{
  VecFx32 pos;
  FLDEFF_FISHING_LURE *wk;
  TASKHEADER_FISHING_LURE head;
  
  if( FLDEFF_CTRL_CheckRegistEffect( fectrl, FLDEFF_PROCID_FISHING_LURE ) == FALSE )
  { // �G�t�F�N�g��o�^
    FLDEFF_PROCID id = FLDEFF_PROCID_FISHING_LURE;
    FLDEFF_CTRL_RegistEffect( fectrl, &id, 1 );
  }

  wk = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_FISHING_LURE );
  head.eff_lure = wk;
  head.pos = *tpos; 
  
  return FLDEFF_CTRL_AddTask(
      fectrl, &DATA_fishing_lure_TaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ���A�[�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishingLureTask_Init( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_FISHING_LURE *work = wk;
  const TASKHEADER_FISHING_LURE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->eff_lure = head->eff_lure;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->eff_lure->g3d_res_mdl, 0, work->eff_lure->g3d_res_mdl );
  
  for( i=0; i<ANIME_NUM; i++ )
  {
    work->obj_anm[i] =
      GFL_G3D_ANIME_Create(
          work->obj_rnd, work->eff_lure->g3d_res_anm[i], 0 );
  }
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, work->obj_anm, ANIME_NUM );
  for( i=0; i<ANIME_NUM; i++ )
  { 
    GFL_G3D_OBJECT_EnableAnime( work->obj, i );
  }
}

//--------------------------------------------------------------
/**
 * ���A�[�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishingLureTask_Delete( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_FISHING_LURE *work = wk;

  for( i=0; i<ANIME_NUM; i++ )
  {
    GFL_G3D_ANIME_Delete( work->obj_anm[i] );
  }
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
}

//--------------------------------------------------------------
/**
 * ���A�[�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishingLureTask_Update( FLDEFF_TASK *task, void *wk )
{
  int i;
  BOOL end = TRUE;
  TASKWORK_FISHING_LURE *work = wk;
  
  for( i=0; i<ANIME_NUM; i++ )
  {
    GFL_G3D_OBJECT_LoopAnimeFrame( work->obj, i, FX32_ONE );
#if 0  
    if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,i,FX32_ONE) == TRUE ){
      end = FALSE;
    }
#endif
  }
#if 0
  if( end ){
    FLDEFF_TASK_CallDelete( task );
  }
#endif
}

//--------------------------------------------------------------
/**
 * ���A�[�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void fishingLureTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_FISHING_LURE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};

  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
}

//--------------------------------------------------------------
//  ���A�[�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_fishing_lure_TaskHeader =
{
  sizeof(TASKWORK_FISHING_LURE),
  fishingLureTask_Init,
  fishingLureTask_Delete,
  fishingLureTask_Update,
  fishingLureTask_Draw,
};
