//======================================================================
/**
 * @file	fldeff_hide.c
 * @brief	動作モデル隠れ蓑
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
#include "fldeff_hide.h"

//======================================================================
//	define
//======================================================================
#define HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ

//======================================================================
//	struct
//======================================================================
//--------------------------------------------------------------
///	FE_HIDE型
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_HIDE FLDEFF_HIDE;

//--------------------------------------------------------------
///	FE_HIDE構造体
//--------------------------------------------------------------
struct _TAG_FLDEFF_HIDE
{
  FLD_G3DOBJ_RESIDX res_idx[HIDE_MAX];
  FLDEFF_CTRL *fectrl;
#ifdef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
  FLD_G3DOBJ_OBJIDX obj_idx[HIDE_MAX];
#endif
};

//--------------------------------------------------------------
/// HIDE_TASKHEADER
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_HIDE *eff_hide;
  FLD_G3DOBJ_CTRL *obj_ctrl;
  MMDL *mmdl;
  HIDE_TYPE hide_type;
}TASKHEADER_HIDE;

//--------------------------------------------------------------
/// TASKWORK_HIDE
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_HIDE head;
  MMDL_CHECKSAME_DATA samedata;
  FLD_G3DOBJ_OBJIDX obj_idx;
  
  u16 anm_flag;
  u16 anm_end_flag;
  
#ifdef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
  fx32 anm_frame;
#endif
}TASKWORK_HIDE;

//--------------------------------------------------------------
/// HIDE_ARCIDX
//--------------------------------------------------------------
typedef struct
{
  u16 mdl_idx;
  u16 anm_idx;
}HIDE_ARCIDX;

//======================================================================
//	proto
//======================================================================
static void hide_InitResource( FLDEFF_HIDE *hide );
static void hide_DeleteResource( FLDEFF_HIDE *hide );

static const FLDEFF_TASK_HEADER data_HideTaskHeader;

static const HIDE_ARCIDX data_ArcIdxTbl[HIDE_MAX];
static const u32 data_PullOffSeTbl[HIDE_MAX];

//======================================================================
//	隠れ蓑　システム
//======================================================================
//--------------------------------------------------------------
/**
 * 隠れ蓑初期化
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	エフェクト使用ワーク
 */
//--------------------------------------------------------------
void * FLDEFF_HIDE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_HIDE *hide;
	
	hide = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_HIDE) );
	hide->fectrl = fectrl;
  
	hide_InitResource( hide );
	return( hide );
}

//--------------------------------------------------------------
/**
 * 隠れ蓑削除
 * @param fectrl FLDEFF_CTRL
 * @param	work	エフェクト使用ワーク
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_HIDE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_HIDE *hide = work;
  hide_DeleteResource( hide );
  GFL_HEAP_FreeMemory( hide );
}

//======================================================================
//	隠れ蓑　リソース
//======================================================================
//--------------------------------------------------------------
/**
 * 隠れ蓑　リソース初期化
 * @param hide FLDEFF_HIDE
 * @retval nothing
 */
//--------------------------------------------------------------
static void hide_InitResource( FLDEFF_HIDE *hide )
{
  int i = 0;
  const HIDE_ARCIDX *tbl = data_ArcIdxTbl;
  ARCHANDLE *handle = FLDEFF_CTRL_GetArcHandleEffect( hide->fectrl );
  FLD_G3DOBJ_CTRL *obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( hide->fectrl );
  FLD_G3DOBJ_RES_HEADER head;
  
  for( ; i < HIDE_MAX; i++, tbl++ ){
    FLD_G3DOBJ_RES_HEADER_Init( &head );
    FLD_G3DOBJ_RES_HEADER_SetMdl( &head, handle, tbl->mdl_idx );
    FLD_G3DOBJ_RES_HEADER_SetAnmArcHandle( &head, handle );
    FLD_G3DOBJ_RES_HEADER_SetAnmArcIdx( &head, tbl->anm_idx );
    hide->res_idx[i] =
      FLD_G3DOBJ_CTRL_CreateResource( obj_ctrl, &head, FALSE );
    
#ifdef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
    hide->obj_idx[i] = FLD_G3DOBJ_CTRL_AddObject(
        obj_ctrl, hide->res_idx[i], 0, NULL );
    FLD_G3DOBJ_CTRL_SetOuterDrawFlag(
        obj_ctrl, hide->obj_idx[i], TRUE );
#endif
  }
}

//--------------------------------------------------------------
/**
 * 隠れ蓑　リソース削除
 * @param hide FLDEFF_HIDE
 * @retval nothing
 */
//--------------------------------------------------------------
static void hide_DeleteResource( FLDEFF_HIDE *hide )
{
  int i = 0;
  FLD_G3DOBJ_CTRL *obj_ctrl =
    FLDEFF_CTRL_GetFldG3dOBJCtrl( hide->fectrl );
  
  for( ; i < HIDE_MAX; i++ ){
#ifdef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
    FLD_G3DOBJ_CTRL_DeleteObject( obj_ctrl, hide->obj_idx[i] );
#endif
    FLD_G3DOBJ_CTRL_DeleteResource( obj_ctrl, hide->res_idx[i] );
  }
}

//======================================================================
//	隠れ蓑　タスク
//======================================================================
//--------------------------------------------------------------
/**
 * 動作モデル用隠れ蓑　追加
 * @param FLDEFF_CTRL*
 * @param mmdl MMDL
 * @param type HIDE_TYPE
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_HIDE_SetMMdl(
    FLDEFF_CTRL *fectrl, MMDL *mmdl, HIDE_TYPE type )
{
  FLDEFF_TASK *task;
  TASKHEADER_HIDE head;
  
  head.eff_hide = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_HIDE );
  head.obj_ctrl = FLDEFF_CTRL_GetFldG3dOBJCtrl( fectrl );
  head.mmdl = mmdl;
  head.hide_type = type;
  
  task = FLDEFF_CTRL_AddTask(
      fectrl, &data_HideTaskHeader, NULL, 0, &head, 0 );
  return( task );
}

//--------------------------------------------------------------
/**
 * 動作モデル用隠れ蓑　アニメセット
 * @param task FLDEFF_TASK*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_HIDE_SetAnime( FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_HIDE *work = FLDEFF_TASK_GetWork( task );
    work->anm_flag = TRUE;
    PMSND_PlaySE( data_PullOffSeTbl[work->head.hide_type] );
  }else{
    GF_ASSERT( 0 );
  }
}

//--------------------------------------------------------------
/**
 * 動作モデル用隠れ蓑　アニメ終了チェック
 * @param task FLDEFF_TASK*
 * @retval BOOL TRUE=終了
 */
//--------------------------------------------------------------
BOOL FLDEFF_HIDE_CheckAnime( FLDEFF_TASK *task )
{
  if( task != NULL ){
    TASKWORK_HIDE *work = FLDEFF_TASK_GetWork( task );
    return( work->anm_end_flag );
  }
    
  GF_ASSERT( 0 );
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * 隠れ蓑タスク　初期化
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void hideTask_Init( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_HIDE *work = wk;
  const TASKHEADER_HIDE *head;
  head = FLDEFF_TASK_GetAddPointer( task );
  
  work->head = *head;
  MMDL_InitCheckSameData( work->head.mmdl, &work->samedata );
  
#ifdef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
  work->obj_idx = work->head.eff_hide->obj_idx[work->head.hide_type];
#else
  work->obj_idx = FLD_G3DOBJ_CTRL_AddObject( work->head.obj_ctrl,
      work->head.eff_hide->res_idx[work->head.hide_type], 0, NULL );
#endif
}

//--------------------------------------------------------------
/**
 * 隠れ蓑タスク　削除
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void hideTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_HIDE *work = wk;
  
  if( MMDL_CheckSameData(work->head.mmdl,&work->samedata) == TRUE ){
    MMDL_SetMoveHideEffectTask( work->head.mmdl, NULL );
  }
  
#ifndef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
  FLD_G3DOBJ_CTRL_DeleteObject( work->head.obj_ctrl, work->obj_idx );
#endif
}

//--------------------------------------------------------------
/**
 * 隠れ蓑タスク　更新
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void hideTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_HIDE *work = wk;
  
  if( MMDL_CheckSameData(work->head.mmdl,&work->samedata) == FALSE ){
    FLDEFF_TASK_CallDelete( task );
  }else{
    fx32 height;
    VecFx32 pos;
    MMDL_GetVectorPos( work->head.mmdl, &pos );
    
    if( MMDL_GetMapPosHeight(work->head.mmdl,&pos,&height) == TRUE ){
      pos.y = height;
    }
    
#ifndef HIDE_OBJ_ONLY_ONE //隠れ蓑OBJ一つのみ
    if( work->anm_flag == TRUE ){
      if( work->anm_end_flag == FALSE ){
        if( FLD_G3DOBJ_CTRL_AnimeObject(
              work->head.obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
          work->anm_end_flag = TRUE;
        }
      }
    }
    
    FLDEFF_TASK_SetPos( task, &pos );
    FLD_G3DOBJ_CTRL_SetObjPos( work->head.obj_ctrl, work->obj_idx, &pos );
#else
    FLDEFF_TASK_SetPos( task, &pos );
#endif
  }
}

//--------------------------------------------------------------
/**
 * 隠れ蓑タスク　描画
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void hideTask_Draw( FLDEFF_TASK *task, void *wk )
{
#ifdef HIDE_OBJ_ONLY_ONE
  TASKWORK_HIDE *work = wk;
  
  if( work->anm_flag == TRUE && work->anm_end_flag == FALSE ){
    FLD_G3DOBJ_CTRL_SetAnimeFrame(
        work->head.obj_ctrl, work->obj_idx, 0, work->anm_frame );
     
    if( FLD_G3DOBJ_CTRL_AnimeObject(
        work->head.obj_ctrl,work->obj_idx,FX32_ONE) == FALSE ){
      work->anm_end_flag = TRUE;
    }
     
    work->anm_frame =  FLD_G3DOBJ_CTRL_GetAnimeFrame(
        work->head.obj_ctrl, work->obj_idx, 0 );
  }else{
    FLD_G3DOBJ_CTRL_SetAnimeFrame(
        work->head.obj_ctrl, work->obj_idx, 0, work->anm_frame );
  }
  
  {
    VecFx32 pos;
    FLDEFF_TASK_GetPos( task, &pos );
    FLD_G3DOBJ_CTRL_SetObjPos( work->head.obj_ctrl, work->obj_idx, &pos );
    FLD_G3DOBJ_CTRL_DrawObject( work->head.obj_ctrl, work->obj_idx );
  }
#endif
}

//--------------------------------------------------------------
//  隠れ蓑タスク　ヘッダー
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER data_HideTaskHeader =
{
  sizeof(TASKWORK_HIDE),
  hideTask_Init,
  hideTask_Delete,
  hideTask_Update,
  hideTask_Draw,
};

//======================================================================
//	data
//======================================================================
//--------------------------------------------------------------
/// 隠れ蓑アーカイブインデックス
//--------------------------------------------------------------
static const HIDE_ARCIDX data_ArcIdxTbl[HIDE_MAX] =
{
  { NARC_fldeff_mv_hide_kusa_nsbmd, NARC_fldeff_mv_hide_kusa_nsbtp },
  { NARC_fldeff_mv_hide_tsuchi_nsbmd, NARC_fldeff_mv_hide_tsuchi_nsbtp },
};

//--------------------------------------------------------------
/// 隠れ蓑が脱げる時のSE
//--------------------------------------------------------------
static const u32 data_PullOffSeTbl[HIDE_MAX] =
{
  SEQ_SE_TB_START,
  SEQ_SE_FLD_149,
};
