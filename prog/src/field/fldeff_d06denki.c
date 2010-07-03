//======================================================================
/**
 * @file	fldeff_d06denki.c
 * @brief	�t�B�[���h D06�d�C���A�p�G�t�F�N�g
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_d06denki.h"

//======================================================================
//	define
//======================================================================
enum
{
  RESNO_MDL_BIRI2,
  RESNO_MDL_STONE_BIG,
  RESNO_MDL_STONE_MINI,
  RESNO_MDL_STONE_SHADOW,
  RESNO_MDL_MAX,
};

enum
{
  RESNO_ANM_BIRI2,
  RESNO_ANM_STONE_BIG_0,
  RESNO_ANM_STONE_BIG_1,
  RESNO_ANM_STONE_MINI_0,
  RESNO_ANM_STONE_MINI_1,
  RESNO_ANM_MAX,
};

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_D06DENKI�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_D06DENKI FLDEFF_D06DENKI;

//--------------------------------------------------------------
///	FLDEFF_D06DENKI�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_D06DENKI
{
  FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl[RESNO_MDL_MAX];
  GFL_G3D_RES *g3d_res_anm[RESNO_ANM_MAX];
  
  GFL_G3D_ANM *obj_anm_biri2;
  GFL_G3D_RND *obj_rnd_biri2;
  GFL_G3D_OBJ *obj_biri2;
};

//--------------------------------------------------------------
/// TASKHEADER_BIRIBIRI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_D06DENKI *eff_d06;
  VecFx32 pos;
}TASKHEADER_BIRIBIRI;

//--------------------------------------------------------------
/// TASKWORK_BIRIBIRI
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_D06DENKI *eff_d06;
#if 0
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm;
  GFL_G3D_RND *obj_rnd;
#endif
  fx32 anm_frame;
  BOOL anm_end_flag;
}TASKWORK_BIRIBIRI;

//--------------------------------------------------------------
/// TASKHEADER_BIGSTONE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_D06DENKI *eff_d06;
  VecFx32 pos;
  const MMDL *mmdl;
}TASKHEADER_BIGSTONE;

//--------------------------------------------------------------
/// TASKWORK_BIGSTONE
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_BIGSTONE head;
  
  u8 off_flag_shakeV; //�c�h��OFF
  u8 set_flag_shakeH; //���h��Z�b�g
  u8 end_flag_shakeH; //���h��I��
  u8 dmy;

  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[2];
  GFL_G3D_RND *obj_rnd;
  
  VecFx32 shadow_pos;
  GFL_G3D_OBJ *obj_shadow;
  GFL_G3D_RND *obj_rnd_shadow;
}TASKWORK_BIGSTONE;

//--------------------------------------------------------------
/// TASKHEADER_MINISTONE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_D06DENKI *eff_d06;
  VecFx32 pos;
}TASKHEADER_MINISTONE;

//--------------------------------------------------------------
/// TASKWORK_MINISTONE
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_MINISTONE head;
  
  u8 set_flag_shake; //���h��Z�b�g
  u8 dmy[3];
  
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm[2];
  GFL_G3D_RND *obj_rnd;
}TASKWORK_MINISTONE;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void d06_InitResource( FLDEFF_D06DENKI *d06 );
static void d06_DeleteResource( FLDEFF_D06DENKI *d06 );

static const FLDEFF_TASK_HEADER DATA_d06TaskHeader;

static const FLDEFF_TASK_HEADER DATA_biribiriTaskHeader;
static const FLDEFF_TASK_HEADER DATA_bigstoneTaskHeader;
static const FLDEFF_TASK_HEADER DATA_ministoneTaskHeader;

//======================================================================
//	D06�G�t�F�N�g�@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * D06�G�t�F�N�g ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_D06DENKI_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_D06DENKI *d06;
	
	d06 = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_D06DENKI) );
	d06->fectrl = fectrl;
  
	d06_InitResource( d06 );
	return( d06 );
}

//--------------------------------------------------------------
/**
 * D06�G�t�F�N�g �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_D06DENKI_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_D06DENKI *d06 = work;
  d06_DeleteResource( d06 );
  GFL_HEAP_FreeMemory( d06 );
}

//======================================================================
//	D06�G�t�F�N�g�@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * D06�G�t�F�N�g�@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void d06_InitResource( FLDEFF_D06DENKI *d06 )
{
  u32 mdl_idx[RESNO_MDL_MAX] = {
    NARC_fldeff_d6_yuka_biri01_nsbmd,
    NARC_fldeff_d6_ishi_big01_nsbmd,
    NARC_fldeff_d6_ishi_mini01_nsbmd,
    NARC_fldeff_d6_ishikage01_nsbmd
  };
  u32 anm_idx[RESNO_ANM_MAX] = {
    NARC_fldeff_d6_yuka_biri01_nsbtp,
    NARC_fldeff_d6_ishi_big01_nsbca,
    NARC_fldeff_d6_ishi_big02_nsbca,
    NARC_fldeff_d6_ishi_mini01_nsbca,
    NARC_fldeff_d6_ishi_mini02_nsbca,
  };
  u32 i;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( d06->fectrl );
  
  for( i = 0; i < RESNO_MDL_MAX; i++ ){
    d06->g3d_res_mdl[i] = GFL_G3D_CreateResourceHandle(
        handle, mdl_idx[i] );
    GFL_G3D_TransVramTexture( d06->g3d_res_mdl[i] );
  }
  
  for( i = 0; i < RESNO_ANM_MAX; i++ ){
    d06->g3d_res_anm[i] = GFL_G3D_CreateResourceHandle(
        handle, anm_idx[i] );
  }
  
  d06->obj_rnd_biri2 = GFL_G3D_RENDER_Create(
      d06->g3d_res_mdl[RESNO_MDL_BIRI2], 0,
      d06->g3d_res_mdl[RESNO_MDL_BIRI2] );
  d06->obj_anm_biri2 = GFL_G3D_ANIME_Create(
      d06->obj_rnd_biri2,
      d06->g3d_res_anm[RESNO_ANM_BIRI2], 0 );
  d06->obj_biri2 = GFL_G3D_OBJECT_Create(
      d06->obj_rnd_biri2, &d06->obj_anm_biri2, 1 );
  GFL_G3D_OBJECT_EnableAnime( d06->obj_biri2, 0 );
}

//--------------------------------------------------------------
/**
 * D06�G�t�F�N�g�@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void d06_DeleteResource( FLDEFF_D06DENKI *d06 )
{
  u32 i;
  
  GFL_G3D_ANIME_Delete( d06->obj_anm_biri2 );
  GFL_G3D_OBJECT_Delete( d06->obj_biri2 );
	GFL_G3D_RENDER_Delete( d06->obj_rnd_biri2 );
 
  for( i = 0; i < RESNO_MDL_MAX; i++ ){
    GFL_G3D_FreeVramTexture( d06->g3d_res_mdl[i] );
 	  GFL_G3D_DeleteResource( d06->g3d_res_mdl[i] );
  }
  
  for( i = 0; i < RESNO_ANM_MAX; i++ ){
 	  GFL_G3D_DeleteResource( d06->g3d_res_anm[i] );
  }
}

//======================================================================
//	D06�G�t�F�N�g�@�^�X�N�@�r���r���G�t�F�N�g
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�r���r���G�t�F�N�g�@�ǉ�
 * @param fmmdl MMDL
 * @param fectrl FLDEFF_CTRL
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_D06DENKI_BIRIBIRI_SetMMdl( MMDL *fmmdl, FLDEFF_CTRL *fectrl )
{
  VecFx32 pos;
  FLDEFF_D06DENKI *d06;
  TASKHEADER_BIRIBIRI head;
  
  MMDL_GetVectorPos( fmmdl, &pos );
#if 0
  pos.y += FX32_ONE*1;
  pos.z += FX32_ONE*12;
#endif

  d06 = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_D06DENKI );
  head.eff_d06 = d06;
  head.pos = pos;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_biribiriTaskHeader, NULL, 0, &head, 0 );
}

//--------------------------------------------------------------
/**
 * �r���r���G�t�F�N�g�^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void biribiriTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIRIBIRI *work = wk;
  const TASKHEADER_BIRIBIRI *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->eff_d06 = head->eff_d06;
  FLDEFF_TASK_SetPos( task, &head->pos );
#if 0  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->eff_d06->g3d_res_mdl[RESNO_MDL_BIRI2], 0,
        work->eff_d06->g3d_res_mdl[RESNO_MDL_BIRI2] );
  
  work->obj_anm =
    GFL_G3D_ANIME_Create( work->obj_rnd,
        work->eff_d06->g3d_res_anm[RESNO_ANM_BIRI2], 0 );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, &work->obj_anm, 1 );
  GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );
#endif
  
  PMSND_PlaySE( SEQ_SE_FLD_49 );
}

//--------------------------------------------------------------
/**
 * �r���r���G�t�F�N�g�^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void biribiriTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIRIBIRI *work = wk;
#if 0
  GFL_G3D_ANIME_Delete( work->obj_anm );
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
#endif
}

//--------------------------------------------------------------
/**
 * �r���r���G�t�F�N�g�^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void biribiriTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIRIBIRI *work = wk;

#if 0  
  if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,0,FX32_ONE) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
  }
#else
  if( work->anm_end_flag == TRUE ){
    FLDEFF_TASK_CallDelete( task );
  }
#endif
}

//--------------------------------------------------------------
/**
 * �r���r���G�t�F�N�g�^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void biribiriTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIRIBIRI *work = wk;
  
  if( work->anm_end_flag == FALSE ){
    VecFx32 pos;
    GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
    GFL_G3D_OBJ *pObj = work->eff_d06->obj_biri2;
	  GFL_G3D_OBJECT_SetAnimeFrame( pObj, 0, (int*)&work->anm_frame );
    
	  if( GFL_G3D_OBJECT_IncAnimeFrame(pObj,0,FX32_ONE) == FALSE ){
      work->anm_end_flag = TRUE;
    }
    
    if( GFL_G3D_OBJECT_GetAnimeFrame(
          pObj,0,(int*)&work->anm_frame) == FALSE ){
      work->anm_end_flag = TRUE;
    }
    
    MTX_Identity33( &status.rotate );
    FLDEFF_TASK_GetPos( task, &status.trans );
    GFL_G3D_DRAW_DrawObjectCullingON( pObj, &status );
  }
}

//--------------------------------------------------------------
//  �r���r���G�t�F�N�g�^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_biribiriTaskHeader =
{
  sizeof(TASKWORK_BIRIBIRI),
  biribiriTask_Init,
  biribiriTask_Delete,
  biribiriTask_Update,
  biribiriTask_Draw,
};

//======================================================================
//	D06�G�t�F�N�g�@�^�X�N�@�傫�ȕ��V��
//======================================================================
//--------------------------------------------------------------
/**
 * ���샂�f���p�傫�ȕ��V�Βǉ�
 * @param fmmdl MMDL
 * @param fectrl FLDEFF_CTRL
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_D06DENKI_BIGSTONE_Add(
    FLDEFF_CTRL *fectrl, const VecFx32 *pos, const MMDL *mmdl )
{
  FLDEFF_D06DENKI *d06;
  TASKHEADER_BIGSTONE head;
  FLDEFF_TASK *task;
  
  d06 = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_D06DENKI );
  head.eff_d06 = d06;
  head.pos = *pos;
  head.mmdl = mmdl;
  
#if 0
  head.pos.y += FX32_ONE*1;
  head.pos.z += FX32_ONE*12;
#endif
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &DATA_bigstoneTaskHeader, NULL, 0, &head, 0 );
  return( task );
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΁@�c���OFF
 * @param task FLDEFF_TASK*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_D06DENKI_BIGSTONE_OFFShakeV( FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_BIGSTONE *work = FLDEFF_TASK_GetWork( task );
    work->off_flag_shakeV = TRUE;
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΁@���h��ON
 * @param task FLDEFF_TASK*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_D06DENKI_BIGSTONE_ONShakeH( FLDEFF_TASK *task )
{
  if( task != NULL ){
    int frame = 0;
    TASKWORK_BIGSTONE *work = FLDEFF_TASK_GetWork( task );
    work->set_flag_shakeH = TRUE;
    work->end_flag_shakeH = FALSE;
	  GFL_G3D_OBJECT_SetAnimeFrame( work->obj, 1, &frame );
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΁@���h��I���`�F�b�N
 * @param task FLDEFF_TASK*
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FLDEFF_D06DENKI_BIGSTONE_CheckShakeH( FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_BIGSTONE *work = FLDEFF_TASK_GetWork( task );
    return( (BOOL)work->end_flag_shakeH );
  }
  
  GF_ASSERT( 0 );
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΃^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigstoneTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIGSTONE *work = wk;
  const TASKHEADER_BIGSTONE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  FLDEFF_TASK_SetPos( task, &work->head.pos );
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_BIG], 0,
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_BIG] );
  
  work->obj_anm[0] =
    GFL_G3D_ANIME_Create( work->obj_rnd,
        work->head.eff_d06->g3d_res_anm[RESNO_ANM_STONE_BIG_0], 0 );
  work->obj_anm[1] =
    GFL_G3D_ANIME_Create( work->obj_rnd,
        work->head.eff_d06->g3d_res_anm[RESNO_ANM_STONE_BIG_1], 0 );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, work->obj_anm, 2 );
  
  GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );
  GFL_G3D_OBJECT_EnableAnime( work->obj, 1 );
  
  work->obj_rnd_shadow =
    GFL_G3D_RENDER_Create(
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_SHADOW], 0,
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_SHADOW] );

  work->obj_shadow = GFL_G3D_OBJECT_Create(
      work->obj_rnd_shadow, NULL, 0 );
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΃^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigstoneTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIGSTONE *work = wk;
  GFL_G3D_ANIME_Delete( work->obj_anm[0] );
  GFL_G3D_ANIME_Delete( work->obj_anm[1] );
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
  
  GFL_G3D_OBJECT_Delete( work->obj_shadow );
	GFL_G3D_RENDER_Delete( work->obj_rnd_shadow );
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΃^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigstoneTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BIGSTONE *work = wk;
  
  if( MMDL_CheckStatusBitUse(work->head.mmdl) == FALSE ){
    return;
  }
  
  { //���W�̔��f
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    work->shadow_pos = pos;
    FLDEFF_TASK_SetPos( task, &pos );
    
    work->shadow_pos.y += NUM_FX32(-4);
    work->shadow_pos.z += NUM_FX32(2)+0x800;
  }
  
  if( work->off_flag_shakeV == FALSE ){
    GFL_G3D_OBJECT_LoopAnimeFrame( work->obj, 0, FX32_ONE );
  }
  
  if( work->set_flag_shakeH == TRUE && work->end_flag_shakeH == FALSE ){
    if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,1,FX32_ONE) == FALSE ){
      work->end_flag_shakeH = TRUE;
    }
  }
}

//--------------------------------------------------------------
/**
 * �傫�ȕ��V�΃^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void bigstoneTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_BIGSTONE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
  
  status.trans = work->shadow_pos;
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj_shadow, &status );
}

//--------------------------------------------------------------
//  �傫�ȕ��V�΃^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_bigstoneTaskHeader =
{
  sizeof(TASKWORK_BIGSTONE),
  bigstoneTask_Init,
  bigstoneTask_Delete,
  bigstoneTask_Update,
  bigstoneTask_Draw,
};

//======================================================================
//	D06�G�t�F�N�g�@�^�X�N�@�����ȕ��V��
//======================================================================
//--------------------------------------------------------------
/**
 * �����ȕ��V�Βǉ�
 * @param fectrl FLDEFF_CTRL
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_D06DENKI_MINISTONE_Add(
    FLDEFF_CTRL *fectrl, const VecFx32 *pos )
{
  FLDEFF_D06DENKI *d06;
  TASKHEADER_MINISTONE head;
  FLDEFF_TASK *task;
  
  d06 = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_D06DENKI );
  head.eff_d06 = d06;
  head.pos = *pos;
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &DATA_ministoneTaskHeader, NULL, 0, &head, 0 );
  return( task );
}

//--------------------------------------------------------------
/**
 * �����ȕ��V�΁@�h��Z�b�g
 * @param task FLDEFF_TASK*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_D06DENKI_MINISTONE_SetShake( FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_MINISTONE *work = FLDEFF_TASK_GetWork( task );
    if( work->set_flag_shake == FALSE ){
      int frame = 0;
      work->set_flag_shake = TRUE;
	    GFL_G3D_OBJECT_SetAnimeFrame( work->obj, 1, &frame );
    }
  }
}

//--------------------------------------------------------------
/**
 * �����ȕ��V�΃^�X�N�@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void ministoneTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_MINISTONE *work = wk;
  const TASKHEADER_MINISTONE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  FLDEFF_TASK_SetPos( task, &work->head.pos );
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_MINI], 0,
        work->head.eff_d06->g3d_res_mdl[RESNO_MDL_STONE_MINI] );
  
  work->obj_anm[0] =
    GFL_G3D_ANIME_Create( work->obj_rnd,
        work->head.eff_d06->g3d_res_anm[RESNO_ANM_STONE_MINI_0], 0 );
  work->obj_anm[1] =
    GFL_G3D_ANIME_Create( work->obj_rnd,
        work->head.eff_d06->g3d_res_anm[RESNO_ANM_STONE_MINI_1], 0 );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, work->obj_anm, 2 );
  
  GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );
  GFL_G3D_OBJECT_EnableAnime( work->obj, 1 );
}

//--------------------------------------------------------------
/**
 * �����ȕ��V�΃^�X�N�@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void ministoneTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_MINISTONE *work = wk;
  GFL_G3D_ANIME_Delete( work->obj_anm[0] );
  GFL_G3D_ANIME_Delete( work->obj_anm[1] );
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
}

//--------------------------------------------------------------
/**
 * �����ȕ��V�΃^�X�N�@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void ministoneTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_MINISTONE *work = wk;
  
  GFL_G3D_OBJECT_LoopAnimeFrame( work->obj, 0, FX32_ONE );
  
  if( work->set_flag_shake == TRUE ){
    if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,1,FX32_ONE) == FALSE ){
      work->set_flag_shake = FALSE;
    }
  }
}

//--------------------------------------------------------------
/**
 * �����ȕ��V�΃^�X�N�@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void ministoneTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_MINISTONE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};

  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
}

//--------------------------------------------------------------
//  �����ȕ��V�΃^�X�N�@�w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_ministoneTaskHeader =
{
  sizeof(TASKWORK_MINISTONE),
  ministoneTask_Init,
  ministoneTask_Delete,
  ministoneTask_Update,
  ministoneTask_Draw,
};
