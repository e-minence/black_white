//======================================================================
/**
 * @file	fldeff_gyoe.c
 * @brief	フィールド びっくりマークエフェクト
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "fieldmap.h"
#include "field_effect.h"
#include "fldmmdl.h"

#include "fldeff_gyoe.h"

//======================================================================
//  define
//======================================================================
#define GYOE_FLDOBJ_Y_OFFSET (NUM_FX32(32)) ///<フィールドOBJからのYオフセット
#define GYOE_FLDOBJ_Z_OFFSET (0x1000) ///<フィールドOBJからのZオフセット
#define GYOE_FLDOBJ_Y_MOVE_START (0x6000) ///<ギョエー初速
#define GYOE_END_FRAME (30) ///<ギョエー終了フレーム	

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
///	FLDEFF_GYOE型
//--------------------------------------------------------------
typedef struct _TAG_FLDEFF_GYOE FLDEFF_GYOE;

//--------------------------------------------------------------
///	FLDEFF_GYOE構造体
//--------------------------------------------------------------
struct _TAG_FLDEFF_GYOE
{
	FLDEFF_CTRL *fectrl;
  GFL_G3D_RES *g3d_res_mdl;
  GFL_G3D_RND *g3d_rnd;
  GFL_G3D_OBJ *g3d_obj;
};

//--------------------------------------------------------------
/// TASKHEADER_GYOE
//--------------------------------------------------------------
typedef struct
{
  FLDEFF_GYOETYPE type;
  BOOL se_play;
  
  FLDEFF_GYOE *eff_gyoe;
  const MMDL *mmdl;
  MMDL_CHECKSAME_DATA samedata;
}TASKHEADER_GYOE;

//--------------------------------------------------------------
/// TASKWORK_GYOE
//--------------------------------------------------------------
typedef struct
{
  TASKHEADER_GYOE head;
  
  int seq_no;
  int frame;
  BOOL end_flag;
  fx32 offs_y;
  fx32 move_y;
}TASKWORK_GYOE;

//======================================================================
//	プロトタイプ
//======================================================================
static void gyoe_InitResource( FLDEFF_GYOE *gyoe );
static void gyoe_DeleteResource( FLDEFF_GYOE *gyoe );

static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader;

//======================================================================
//	びっくりマークエフェクト　システム
//======================================================================
//--------------------------------------------------------------
/**
 * びっくりマークエフェクト 初期化
 * @param	fectrl		FLDEFF_CTRL *
 * @param heapID HEAPID
 * @retval	void*	エフェクト使用ワーク
 */
//--------------------------------------------------------------
void * FLDEFF_GYOE_Init( FLDEFF_CTRL *fectrl, HEAPID heapID )
{
	FLDEFF_GYOE *gyoe;
	
	gyoe = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDEFF_GYOE) );
	gyoe->fectrl = fectrl;
	gyoe_InitResource( gyoe );
	return( gyoe );
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクト 削除
 * @param fectrl FLDEFF_CTRL
 * @param	work	エフェクト使用ワーク
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDEFF_GYOE_Delete( FLDEFF_CTRL *fectrl, void *work )
{
	FLDEFF_GYOE *gyoe = work;
  gyoe_DeleteResource( gyoe );
  GFL_HEAP_FreeMemory( gyoe );
}

//======================================================================
//	びっくりマークエフェクト　リソース
//======================================================================
//--------------------------------------------------------------
/**
 * びっくりマークエフェクト　リソース初期化
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoe_InitResource( FLDEFF_GYOE *gyoe )
{
  BOOL ret;
  ARCHANDLE *handle;
  
  handle = FLDEFF_CTRL_GetArcHandleEffect( gyoe->fectrl );
  
  gyoe->g3d_res_mdl	=
    GFL_G3D_CreateResourceHandle( handle, NARC_fldeff_sisen_ef_nsbmd );
  ret = GFL_G3D_TransVramTexture( gyoe->g3d_res_mdl );
  GF_ASSERT( ret );
  
  gyoe->g3d_rnd = GFL_G3D_RENDER_Create(
      gyoe->g3d_res_mdl, 0, gyoe->g3d_res_mdl );
  
  gyoe->g3d_obj = GFL_G3D_OBJECT_Create( gyoe->g3d_rnd, NULL, 0 );
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクト　リソース削除
 * @param fectrl FLDEFF_CTRL*
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoe_DeleteResource( FLDEFF_GYOE *gyoe )
{
  GFL_G3D_OBJECT_Delete( gyoe->g3d_obj );
	GFL_G3D_RENDER_Delete( gyoe->g3d_rnd );
 	GFL_G3D_DeleteResource( gyoe->g3d_res_mdl );
}

//======================================================================
//	びっくりマークエフェクト　タスク
//======================================================================
//--------------------------------------------------------------
/**
 * 動作モデル用びっくりマークエフェクト　追加
 * @param mmdl MMDL
 * @param FLDEFF_CTRL*
 * @retval FLDEFF_TASK*
 */
//--------------------------------------------------------------
FLDEFF_TASK * FLDEFF_GYOE_SetMMdl( FLDEFF_CTRL *fectrl,
    const MMDL *mmdl, FLDEFF_GYOETYPE type, BOOL se_play )
{
  FLDEFF_GYOE *gyoe;
  TASKHEADER_GYOE head;
  
  head.type = type;
  head.se_play = se_play;
  
  head.eff_gyoe = FLDEFF_CTRL_GetEffectWork( fectrl, FLDEFF_PROCID_GYOE );
  head.mmdl = mmdl;
  MMDL_InitCheckSameData( mmdl, &head.samedata );
  
  return( FLDEFF_CTRL_AddTask(fectrl,&DATA_gyoeTaskHeader,NULL,0,&head,0) );
}

//--------------------------------------------------------------
/**
 * 動作モデル用びっくりマークエフェクト　終了チェック
 * @param task FLDEFF_TASK
 * @retval
 */
//--------------------------------------------------------------
BOOL FLDEFF_GYOE_CheckEnd( FLDEFF_TASK *task )
{
  TASKWORK_GYOE *work = FLDEFF_TASK_GetWork( task );
  return( work->end_flag );
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクトタスク　初期化
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Init( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GYOE *work = wk;
  const TASKHEADER_GYOE *head;
  
  head = FLDEFF_TASK_GetAddPointer( task );
  work->head = *head;
  work->move_y = GYOE_FLDOBJ_Y_MOVE_START;
  FLDEFF_TASK_CallUpdate( task ); //即動作
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクトタスク　削除
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Delete( FLDEFF_TASK *task, void *wk )
{
//  TASKWORK_GYOE *work = wk;
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクトタスク　更新
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Update( FLDEFF_TASK *task, void *wk )
{
  VecFx32 pos;
  TASKWORK_GYOE *work = wk;
  
  if( MMDL_CheckSameData(work->head.mmdl,&work->head.samedata) == FALSE ){
    GF_ASSERT( 0 ); //マーク表示中に消去
    work->end_flag = TRUE;
    return;
  }
    
  switch( work->seq_no ){
  case 0:
    work->offs_y += work->move_y;
    if( work->offs_y ){
      work->move_y += -0x2000;
    }else{
      work->move_y = 0;
      work->seq_no++;
    }
    break;
  case 1:
    work->frame++;
    
    if( work->frame >= GYOE_END_FRAME ){
      work->seq_no++;
      work->end_flag = TRUE;
    }
  }
  
  MMDL_GetVectorPos( work->head.mmdl, &pos );
  pos.y += GYOE_FLDOBJ_Y_OFFSET + work->offs_y;
  pos.z += GYOE_FLDOBJ_Z_OFFSET;
  FLDEFF_TASK_SetPos( task, &pos );
}

//--------------------------------------------------------------
/**
 * びっくりマークエフェクトタスク　描画
 * @param task FLDEFF_TASK
 * @param wk task work
 * @retval nothing
 */
//--------------------------------------------------------------
static void gyoeTask_Draw( FLDEFF_TASK *task, void *wk )
{
  TASKWORK_GYOE *work = wk;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  MTX_Identity33( &status.rotate );
  FLDEFF_TASK_GetPos( task, &status.trans );
  GFL_G3D_DRAW_DrawObjectCullingON( work->head.eff_gyoe->g3d_obj, &status );
}

//--------------------------------------------------------------
//  びっくりマークエフェクトタスク　ヘッダー
//--------------------------------------------------------------
static const FLDEFF_TASK_HEADER DATA_gyoeTaskHeader =
{
  sizeof(TASKWORK_GYOE),
  gyoeTask_Init,
  gyoeTask_Delete,
  gyoeTask_Update,
  gyoeTask_Draw,
};
