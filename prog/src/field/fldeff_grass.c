//======================================================================
/**
 * @file	fldeff_grass.c
 * @brief	フィールド 草エフェクト
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_grass.h"

//======================================================================
//  define
//======================================================================
#define GRASS_SHAKE_FRAME (FX32_ONE*12)

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_GRASS型
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_GRASS FLDEFF_GRASS;

//--------------------------------------------------------------
///	FLDEFF_GRASS構造体
//--------------------------------------------------------------
struct _TAG_FLDEFF_GRASS
{
	FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RES *g3d_res_anm;
};

//--------------------------------------------------------------
/// TASKHEADER_GRASS
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_GRASS *eff_grass;
  FLDMMDL *fmmdl;
  int init_gx;
  int init_gz;
  u16 obj_id;
  u16 zone_id;
  VecFx32 pos;
}TASKHEADER_GRASS;

//--------------------------------------------------------------
/// TASKWORK_GRASS
//--------------------------------------------------------------
typedef struct
{
  int seq_no;
  TASKHEADER_GRASS head;
  GFL_G3D_OBJ *obj;
  GFL_G3D_ANM *obj_anm;
  GFL_G3D_RND *obj_rnd;
}TASKWORK_GRASS;

//======================================================================
//	プロトタイプ
//======================================================================
static void grass_InitResource( FLDEFF_GRASS *grass );
static void grass_DeleteResource( FLDEFF_GRASS *grass );

static const FLDEFF_TASK_HEADER DATA_grassTaskHeader;

//======================================================================
//	草エフェクト　システム
//======================================================================
//--------------------------------------------------------------
/**
 * 草エフェクト 初期化
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	エフェクト使用ワーク
 */
//--------------------------------------------------------------
void * FLDEFF_GRASS_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_GRASS *grass;
	
	grass = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_GRASS) );
	grass->fectrl = fectrl;
  
	grass_InitResource( grass );
	return( grass );
}

//--------------------------------------------------------------
/**
 * 草エフェクト 削除
 * @param fectrl FLDEFF_CTRL
 * @param	work	エフェクト使用ワーク
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_GRASS *grass = work;
  grass_DeleteResource( grass );
  GFL_HEAP_FreeMemory( grass );
}

//======================================================================
//	草エフェクト　リソース
//======================================================================
//--------------------------------------------------------------
/**
 * 草エフェクト　リソース初期化
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_InitResource( FLDEFF_GRASS *grass )
{
  BOOL ret;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( grass->fectrl );
  
  grass->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_kusaeff_nsbmd );
  ret = GFL_G3D_TransVramTexture( grass->g3d_res_mdl );
  
  GF_ASSERT( ret );
  
  grass->g3d_res_anm	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_kusaeff_nsbtp );
}

//--------------------------------------------------------------
/**
 * 草エフェクト　リソース削除
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void grass_DeleteResource( FLDEFF_GRASS *grass )
{
 	GFL_G3D_DeleteResource( grass->g3d_res_anm );
 	GFL_G3D_DeleteResource( grass->g3d_res_mdl );
}

//======================================================================
//	草エフェクト　タスク
//======================================================================
//--------------------------------------------------------------
/**
 * 動作モデル用草エフェクト　追加
 * @param fmmdl FLDMMDL
 * @param FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDEFF_GRASS_SetFldMMdl( FLDEFF_CTRL *fectrl, FLDMMDL *fmmdl, BOOL anm )
{
  fx32 h;
  VecFx32 pos;
  FLDEFF_GRASS *grass;
  TASKHEADER_GRASS head;
  
  grass = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GRASS );
  head.eff_grass = grass;
  head.fmmdl = fmmdl;
  head.obj_id = FLDMMDL_GetOBJID( fmmdl );
  head.zone_id = FLDMMDL_GetZoneID( fmmdl );
  head.init_gx = FLDMMDL_GetGridPosX( fmmdl );
  head.init_gz = FLDMMDL_GetGridPosZ( fmmdl );
//  KAGAYA_Printf( "草エフェクト GX=%d,GZ=%d\n", head.init_gx, head.init_gz );
  FLDMMDL_TOOL_GetCenterGridPos( head.init_gx, head.init_gz, &pos );
  pos.y = FLDMMDL_GetVectorPosY( fmmdl );
  
  FLDMMDL_GetMapPosHeight( fmmdl, &pos, &h );
  pos.y = h;
  
  pos.y += NUM_FX32(4);
  pos.z += NUM_FX32(12);
  head.pos = pos;
  
  FLDEFF_CTRL_AddTask(
      fectrl, &DATA_grassTaskHeader, NULL, anm, &head, 0 );
}

//--------------------------------------------------------------
/**
 * 草エフェクトタスク　初期化
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  const TASKHEADER_GRASS *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  FLDEFF_TASK_SetPos( task, &head->pos );
  
  work->obj_rnd =
    GFL_G3D_RENDER_Create(
        work->head.eff_grass->g3d_res_mdl, 0,
        work->head.eff_grass->g3d_res_mdl );
  
  work->obj_anm =
    GFL_G3D_ANIME_Create(
        work->obj_rnd, work->head.eff_grass->g3d_res_anm, 0 );
  
  work->obj = GFL_G3D_OBJECT_Create(
      work->obj_rnd, &work->obj_anm, 1 );
  GFL_G3D_OBJECT_EnableAnime( work->obj, 0 );
  
  if( FLDEFF_TASK_GetAddParam(task) == FALSE ){ //アニメ無し
    work->seq_no = 1;
  }
  
  FLDEFF_TASK_CallUpdate( task ); //即動作
}

//--------------------------------------------------------------
/**
 * 草エフェクトタスク　削除
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Delete( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  GFL_G3D_ANIME_Delete( work->obj_anm );
  GFL_G3D_OBJECT_Delete( work->obj );
	GFL_G3D_RENDER_Delete( work->obj_rnd );
}

//--------------------------------------------------------------
/**
 * 草エフェクトタスク　更新
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Update( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GRASS *work = wk;
  
  switch( work->seq_no ){
  case 0: //アニメ終了待ち
    if( GFL_G3D_OBJECT_IncAnimeFrame(work->obj,0,FX32_ONE) == FALSE ){
      work->seq_no++;
    }
    break;
  case 1: //アニメ終了 or 揺れ無し
    {
      int frame = GRASS_SHAKE_FRAME;
	    GFL_G3D_OBJECT_SetAnimeFrame( work->obj, 0, &frame );
    }
    work->seq_no++;
  case 2:
    if( FLDMMDL_CheckSameID(work->head.fmmdl,
          work->head.obj_id,work->head.zone_id) == FALSE ){
      FLDEFF_TASK_CallDelete( task );
      return;
    }
     
    {
      int gx = FLDMMDL_GetGridPosX( work->head.fmmdl );
      int gz = FLDMMDL_GetGridPosZ( work->head.fmmdl );
      if( work->head.init_gx != gx || work->head.init_gz != gz ){
         FLDEFF_TASK_CallDelete( task );
         return;
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * 草エフェクトタスク　描画
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void grassTask_Draw( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_GRASS *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->obj, &status );
}

//--------------------------------------------------------------
//  草エフェクトタスク　ヘッダー
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_grassTaskHeader =
{
  sizeof(TASKWORK_GRASS),
  grassTask_Init,
  grassTask_Delete,
  grassTask_Update,
  grassTask_Draw,
};
