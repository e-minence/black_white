//======================================================================
/**
 * @file	fldeff_btrain.c
 * @brief	�t�B�[���h �o�g���g���C��
 * @author	kagaya
 * @date	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_effect.h"
#include "fldeff_btrain.h"

//======================================================================
//  define
//======================================================================
#define BTRAIN_SHAKE_FRAME (FX32_ONE*12)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_BTRAIN�^
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_BTRAIN FLDEFF_BTRAIN;

//--------------------------------------------------------------
///	FLDEFF_BTRAIN�\����
//--------------------------------------------------------------
struct _TAG_FLDEFF_BTRAIN
{
	FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl[FLDEFF_BTRAIN_TYPE_MAX];
  GFL_G3D_RES *g3d_res_anm[FLDEFF_BTRAIN_ANIME_TYPE_MAX];
  GFL_G3D_RND *g3d_rnd[FLDEFF_BTRAIN_TYPE_MAX];
};

//--------------------------------------------------------------
/// TASKHEADER_BTRAIN
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_BTRAIN *eff_btrain;
  FLDEFF_BTRAIN_TYPE type;
}TASKHEADER_BTRAIN;

//--------------------------------------------------------------
/// TASKWORK_BTRAIN
//--------------------------------------------------------------
typedef struct
{
  u8 seq_no;
  u8 vanish;
  u8 anm_type;
  u8 anm_end;
  
  TASKHEADER_BTRAIN head;
  
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *anm[FLDEFF_BTRAIN_ANIME_TYPE_MAX];
  
#ifdef DEBUG_ONLY_FOR_kagaya
  int debugAnimeFrame;
#endif
}TASKWORK_BTRAIN;

//======================================================================
//	�v���g�^�C�v
//======================================================================
static void btrain_InitResource(
    FLDEFF_BTRAIN *btrain, FLDEFF_BTRAIN_TYPE type );
static void btrain_DeleteResource( FLDEFF_BTRAIN *btrain );

static const FLDEFF_TASK_HEADER data_BTrainTaskHeader;

static const u16 data_ArcIdxBTrainMdl[FLDEFF_BTRAIN_TYPE_MAX];
static const u16 data_ArcIdxBTrainAnm[FLDEFF_BTRAIN_ANIME_TYPE_MAX];

//======================================================================
//	�o�g���g���C���@�V�X�e��
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���g���C�� ������
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	�G�t�F�N�g�g�p���[�N
 */
//--------------------------------------------------------------
void * FLDEFF_BTRAIN_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_BTRAIN *btrain;
	
	btrain = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_BTRAIN) );
	btrain->fectrl = fectrl;
  
//btrain_InitResource( btrain );
	return( btrain );
}

//--------------------------------------------------------------
/**
 * �o�g���g���C�� �폜
 * @param fectrl FLDEFF_CTRL
 * @param	work	�G�t�F�N�g�g�p���[�N
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_BTRAIN_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_BTRAIN *btrain = work;
  btrain_DeleteResource( btrain );
  GFL_HEAP_FreeMemory( btrain );
}

//======================================================================
//	�o�g���g���C���@���\�[�X
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���g���C���@���\�[�X������
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrain_InitResource(
    FLDEFF_BTRAIN *btrain, FLDEFF_BTRAIN_TYPE type )
{
  int i;
  const u16 *tbl;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( btrain->fectrl );
  
  tbl = data_ArcIdxBTrainMdl;

#if 0  
  for( i = 0; i < FLDEFF_BTRAIN_TYPE_MAX; i++, tbl++ ){
    btrain->g3d_res_mdl[i] = GFL_G3D_CreateResourceHandle( handle, *tbl );
    GFL_G3D_TransVramTexture( btrain->g3d_res_mdl[i] );
    
    btrain->g3d_rnd[i] = GFL_G3D_RENDER_Create(
        btrain->g3d_res_mdl[i], 0, btrain->g3d_res_mdl[i] );
  }
#else
  {
    i = type;
    if( btrain->g3d_res_mdl[i] == NULL ){
      btrain->g3d_res_mdl[i] = GFL_G3D_CreateResourceHandle(
          handle, tbl[type] );
      GFL_G3D_TransVramTexture( btrain->g3d_res_mdl[i] );
      btrain->g3d_rnd[i] = GFL_G3D_RENDER_Create(
          btrain->g3d_res_mdl[i], 0, btrain->g3d_res_mdl[i] );
    }
  }
#endif
  
  tbl = data_ArcIdxBTrainAnm;
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++, tbl++ ){
    if( btrain->g3d_res_anm[i] == NULL ){
      btrain->g3d_res_anm[i] = GFL_G3D_CreateResourceHandle( handle, *tbl );
    }
  }
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@���\�[�X�폜
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrain_DeleteResource( FLDEFF_BTRAIN *btrain )
{
  int i;
  
  for( i = 0; i < FLDEFF_BTRAIN_TYPE_MAX; i++ ){
    if( btrain->g3d_res_mdl[i] != NULL ){
	    GFL_G3D_RENDER_Delete( btrain->g3d_rnd[i] );
      GFL_G3D_FreeVramTexture( btrain->g3d_res_mdl[i] );
      GFL_G3D_DeleteResource( btrain->g3d_res_mdl[i] );
    }
  }
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++ ){
    if( btrain->g3d_res_anm[i] != NULL ){
      GFL_G3D_DeleteResource( btrain->g3d_res_anm[i] );
    }
  }
}

//======================================================================
//	�o�g���g���C���@�^�X�N
//======================================================================
//--------------------------------------------------------------
/**
 * �o�g���g���C���@�Z�b�g
 * @param fectrl FLDEFF_CTRL
 * @param type FLDEFF_BTRAIN_TYPE
 * @param pos �z�u������W
 * @retval nothing
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_BTRAIN_SetTrain(
    FLDEFF_CTRL *fectrl, FLDEFF_BTRAIN_TYPE type, const VecFx32 *pos )
{
  int i;
  FLDEFF_TASK *task;
  TASKHEADER_BTRAIN head;
  
  head.eff_btrain = FLDEFF_CTRL_GetEffectWork(
      fectrl, FLDEFF_PROCID_BTRAIN );
  head.type = type;
  
  btrain_InitResource( head.eff_btrain, head.type ); //���\�[�X�Z�b�g
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &data_BTrainTaskHeader, pos, 0, &head, 0 );
  return( task );
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@�A�j���ݒ�
 * @param task FLDEFF_TASK
 * @param type FLDEFF_BTRAIN_ANIME_TYPE
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_BTRAIN_SetAnime(
    FLDEFF_TASK *task, FLDEFF_BTRAIN_ANIME_TYPE type )
{
  int i;
  TASKWORK_BTRAIN *work = FLDEFF_TASK_GetWork( task );
  
  GF_ASSERT( type < FLDEFF_BTRAIN_ANIME_TYPE_MAX );
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++ ){
	  GFL_G3D_OBJECT_ResetAnimeFrame( work->obj, i ); //���������Z�b�g
    
    if( i == type ){
      GFL_G3D_OBJECT_EnableAnime( work->obj, i );
    }else{
      GFL_G3D_OBJECT_DisableAnime( work->obj, i );
    }
  }
  
  work->anm_type = type;
  work->anm_end = FALSE;
  
#ifdef DEBUG_ONLY_FOR_kagaya
  work->debugAnimeFrame = 0;
#endif
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@�A�j���I���擾
 * @param task FLDEFF_TASK
 * @param type FLDEFF_BTRAIN_ANIME_TYPE
 * @retval nothing
 */
//--------------------------------------------------------------
BOOL FLDEFF_BTRAIN_CheckAnimeEnd( FLDEFF_TASK *task )
{
  TASKWORK_BTRAIN *work = FLDEFF_TASK_GetWork( task );
  return( work->anm_end );
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@��\���t���O�Z�b�g
 * @param task FLDEFF_TASK
 * @param flag TRUE=��\���AFALSE=�\��
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_BTRAIN_SetVanishFlag( FLDEFF_TASK *task, BOOL flag )
{
  TASKWORK_BTRAIN *work = FLDEFF_TASK_GetWork( task );
  work->vanish = flag;
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@������
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrainTask_Init( FLDEFF_TASK *task, void *wk )
{
  int i;
  FLDEFF_BTRAIN_TYPE type;
  FLDEFF_BTRAIN *eff_btrain;
  TASKWORK_BTRAIN *work = wk;
  const TASKHEADER_BTRAIN *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  
  eff_btrain = work->head.eff_btrain;
  type = work->head.type;
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++ ){
    work->anm[i] = GFL_G3D_ANIME_Create(
      eff_btrain->g3d_rnd[type], eff_btrain->g3d_res_anm[i], 0 );
  }
  
  work->anm_type = FLDEFF_BTRAIN_ANIME_TYPE_MAX;
  
  work->obj = GFL_G3D_OBJECT_Create(
      eff_btrain->g3d_rnd[type], work->anm, FLDEFF_BTRAIN_ANIME_TYPE_MAX );
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++ ){
    GFL_G3D_OBJECT_DisableAnime( work->obj, i );
  }
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@�폜
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrainTask_Delete( FLDEFF_TASK *task, void *wk )
{
  int i;
  TASKWORK_BTRAIN *work = wk;
  
  GFL_G3D_OBJECT_Delete( work->obj );
  
  for( i = 0; i < FLDEFF_BTRAIN_ANIME_TYPE_MAX; i++ ){
    GFL_G3D_ANIME_Delete( work->anm[i] );
  }
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@�X�V
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrainTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BTRAIN *work = wk;

#ifdef DEBUG_ONLY_FOR_kagaya
  {
    int cont = GFL_UI_KEY_GetCont();
    int trg = GFL_UI_KEY_GetTrg();
    
    if( (cont & PAD_BUTTON_R) && (trg & PAD_BUTTON_A) ){
      work->anm_type++;
      
      if( work->anm_type >= FLDEFF_BTRAIN_ANIME_TYPE_MAX ){
        work->anm_type = 0;
      }
      
      OS_Printf( "�g���C���A�j�� %d\n", work->anm_type );
      FLDEFF_BTRAIN_SetAnime( task, work->anm_type );
    }
  }
#endif
  
  if( work->anm_type != FLDEFF_BTRAIN_ANIME_TYPE_MAX ){
     work->anm_end = FALSE;
    
	  if( GFL_G3D_OBJECT_IncAnimeFrame(
          work->obj,work->anm_type,FX32_ONE) == FALSE ){
      work->anm_end = TRUE;
    }

#ifdef DEBUG_ONLY_FOR_kagaya    
    if( work->anm_end == FALSE ){
      work->debugAnimeFrame++;
      KAGAYA_Printf( "�o�g���g���C���@�A�j�� %d, �t���[�� %d\n",
          work->anm_type, work->debugAnimeFrame );
    }
#endif
  }
}

//--------------------------------------------------------------
/**
 * �o�g���g���C���@�`��
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void btrainTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_BTRAIN *work = wk;

  if( work->vanish == FALSE ){
    VecFx32 pos;
    GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
    MTX_Identity33( &status.rotate );
    FLDEFF_TASK_GetPos( task, &status.trans );
    GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
  }
}

//--------------------------------------------------------------
//  �o�g���g���C���^�X�N �w�b�_�[
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_BTrainTaskHeader =
{
  sizeof(TASKWORK_BTRAIN),
  btrainTask_Init,
  btrainTask_Delete,
  btrainTask_Update,
  btrainTask_Draw,
};

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  �o�g���g���C���@���f���A�[�J�C�u�C���f�b�N�X
//--------------------------------------------------------------
static const u16 data_ArcIdxBTrainMdl[FLDEFF_BTRAIN_TYPE_MAX] =
{
  NARC_fldeff_b_train01_nsbmd,
  NARC_fldeff_b_train02_nsbmd,
  NARC_fldeff_b_train03_nsbmd,
  NARC_fldeff_b_train04_nsbmd,
  NARC_fldeff_b_train05_nsbmd,
  NARC_fldeff_b_train06_nsbmd,
  NARC_fldeff_b_train07_nsbmd,
  NARC_fldeff_b_train08_nsbmd,
};

//--------------------------------------------------------------
//  �o�g���g���C���@���f���A�[�J�C�u�C���f�b�N�X
//--------------------------------------------------------------
static const u16 data_ArcIdxBTrainAnm[FLDEFF_BTRAIN_ANIME_TYPE_MAX] =
{
  NARC_fldeff_b_train_a1_nsbca,
  NARC_fldeff_b_train_a2_nsbca,
  NARC_fldeff_b_train_a3_nsbca,
  NARC_fldeff_b_train_a4_nsbca,
};
