//======================================================================
/**
 * @file	fldeff_iwakudaki.c
 * @brief	�t�B�[���h ��ӂ�
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_iwakudaki.h"

//======================================================================
//	define
//======================================================================
typedef enum{
  ANIME_ICA,
  ANIME_ITP,
  ANIME_ITA,
  ANIME_IMA,
  ANIME_NUM
} ANIME_INDEX;

static u32 dat_id[ANIME_NUM] =
{
  NARC_fldeff_iwakudaki_nsbca,
  NARC_fldeff_iwakudaki_nsbtp,
  NARC_fldeff_iwakudaki_nsbta,
  NARC_fldeff_iwakudaki_nsbma,
};


//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_IWAKUDAKI�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_IWAKUDAKI FLDEFF_IWAKUDAKI;

//--------------------------------------------------------------
///	FLDEFF_IWAKUDAKI�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_IWAKUDAKI
{
	FLDEFF_CTRL *fectrl;
  
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RES *g3d_res_anm[ANIME_NUM];

  GFL_TCB* vblank_task;
  BOOL trans_finished;
};

//--------------------------------------------------------------
/// TASKWORK_IWAKUDAKI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_IWAKUDAKI *eff_iwakudaki;
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[ANIME_NUM];
  GFL_G3D_RND *obj_rnd;
}TASKWORK_IWAKUDAKI;

//--------------------------------------------------------------
/// TASKHEADER_IWAKUDAKI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_IWAKUDAKI *eff_iwakudaki;
  VecFx32 pos;
}TASKHEADER_IWAKUDAKI;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void iwakudaki_InitResource( FLDEFF_IWAKUDAKI *kemu );
static void iwakudaki_DeleteResource( FLDEFF_IWAKUDAKI *kemu );
static void VBlankFunc( GFL_TCB* tcb, void* wk ); 
static const FLDEFF_TASK_HEADER DATA_iwakudakiTaskHeader;

//======================================================================
//	��ӂ��@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * ��ӂ� ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_IWAKUDAKI_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_IWAKUDAKI *kemu;
	
	kemu = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_IWAKUDAKI) );
	kemu->fectrl = fectrl;
  
	iwakudaki_InitResource( kemu );
	return( kemu );
}

//--------------------------------------------------------------
/**
 * ��ӂ� �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_IWAKUDAKI_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_IWAKUDAKI *kemu = work;
  iwakudaki_DeleteResource( kemu );
  GFL_HEAP_FreeMemory( kemu );
}

//======================================================================
//	��ӂ��@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * ��ӂ��@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudaki_InitResource( FLDEFF_IWAKUDAKI *kemu )
{
  int i;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( kemu->fectrl );
  
  kemu->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_iwakudaki_nsbmd );

  GFL_G3D_AllocVramTexture( kemu->g3d_res_mdl );
  kemu->vblank_task = GFUser_VIntr_CreateTCB( VBlankFunc, kemu, 0 );

  for( i=0; i<ANIME_NUM; i++ )
  {
    kemu->g3d_res_anm[i] =
      GFL_G3D_CreateResourceHandle( handle, dat_id[i] );
  }
}

//--------------------------------------------------------------
/**
 * ��ӂ��@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudaki_DeleteResource( FLDEFF_IWAKUDAKI *kemu )
{
  int i;
  for( i=0; i<ANIME_NUM; i++ )
  {
    GFL_G3D_DeleteResource( kemu->g3d_res_anm[i] );
  }

  GFL_G3D_FreeVramTexture( kemu->g3d_res_mdl );
 	GFL_G3D_DeleteResource( kemu->g3d_res_mdl );

  GFL_TCB_DeleteTask( kemu->vblank_task );
}

//--------------------------------------------------------------
/**
 * @brief VBlank ���Ԓ��̏���
 *
 * @param tcb
 * @param wk
 */
//--------------------------------------------------------------
static void VBlankFunc( GFL_TCB* tcb, void* wk )
{
  FLDEFF_IWAKUDAKI* kemu = wk;

  if( kemu->trans_finished == FALSE ) {
    GFL_G3D_TransOnlyTexture( kemu->g3d_res_mdl );
    kemu->trans_finished = TRUE;
  }
}

//======================================================================
//	��ӂ��@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�y���@�ǉ�
 * @param fmmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_IWAKUDAKI_SetMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  VecFx32 pos;
  FLDEFF_IWAKUDAKI *kemu;
  TASKHEADER_IWAKUDAKI head;
  
  MMDL_GetVectorPos( fmmdl, &pos );
  pos.y += FX32_ONE*1;
  pos.z += FX32_ONE*12;
  
  if( FLDEFF_CTRL_CheckRegistEffect( fectrl, FLDEFF_PROCID_IWAKUDAKI ) == FALSE )
  { // �G�t�F�N�g��o�^
    FLDEFF_PROCID id = FLDEFF_PROCID_IWAKUDAKI;
    FLDEFF_CTRL_RegistEffect( fectrl, &id, 1 );
  }

  kemu = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_IWAKUDAKI );
  head.eff_iwakudaki = kemu;
  head.pos = pos; 
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_iwakudakiTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * ��ӂ��^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudakiTask_Init( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_IWAKUDAKI *work = wk;
  const TASKHEADER_IWAKUDAKI *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->eff_iwakudaki = head->eff_iwakudaki;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->eff_iwakudaki->g3d_res_mdl, 0, work->eff_iwakudaki->g3d_res_mdl );
  
  for( i=0; i<ANIME_NUM; i++ )
  {
    work->obj_anm[i] =
      GFL_G3D_ANIME_Create(
          work->obj_rnd, work->eff_iwakudaki->g3d_res_anm[i], 0 );
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
 * ��ӂ��^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudakiTask_Delete( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_IWAKUDAKI *work = wk;

  for( i=0; i<ANIME_NUM; i++ )
  {
    GFL_G3D_ANIME_Delete( work->obj_anm[i] );
  }
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
}

//--------------------------------------------------------------
/**
 * ��ӂ��^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudakiTask_Update( FLDEFF_TASK *task, void *wk )
{
  int i;
  BOOL end = TRUE;
  TASKWORK_IWAKUDAKI *work = wk;
  
  for( i=0; i<ANIME_NUM; i++ )
  {
    if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,i,FX32_ONE) == TRUE ){
      end = FALSE;
    } 
  }
  if( end ){
    FLDEFF_TASK_CallDelete( task );
  }
}

//--------------------------------------------------------------
/**
 * ��ӂ��^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void iwakudakiTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_IWAKUDAKI *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};

  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
}

//--------------------------------------------------------------
//  ��ӂ��^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_iwakudakiTaskHeader =
{
  sizeof(TASKWORK_IWAKUDAKI),
  iwakudakiTask_Init,
  iwakudakiTask_Delete,
  iwakudakiTask_Update,
  iwakudakiTask_Draw,
};
