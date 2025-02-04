//============================================================================================
/**
 * @file	fieldmap_func.c
 * @brief	フィールドマップ上で動作する制御タスクシステム
 * @author	tamada GAME FREAK inc.
 * @date	2006.03.15
 * @date	2009.07.22	DSから移植	tomoya 
 *
 */
//============================================================================================

#include "gflib.h"

#include "fieldmap.h"

#include "fieldmap_func_def.h"
#include "fieldmap_func.h"

//============================================================================================
//============================================================================================
typedef struct {
	u32 dmy[8];			///<汎用ワーク
}FREE_WORK;

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ用制御タスクシステムのシステムワーク定義
 */
//------------------------------------------------------------------
struct FLDMAPFUNC_SYS{
	FIELDMAP_WORK * fsys;		///<フィールド制御システムへのポインタ
	int heapID;					///<使用するヒープのID
	int max;					///<FLDMAPFUNCの最大数
	FLDMAPFUNC_WORK * array;	///<FLDMAPFUNC用ワークの配列へのポインタ
	GFL_TCBSYS * mainsys;			///<メイン用TCB
	GFL_TCBSYS * drawsys;			///<描画用TCB
};

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ用制御タスクの制御ワーク定義
 */
//------------------------------------------------------------------
struct FLDMAPFUNC_WORK{
	FLDMAPFUNC_SYS * sys;			///<制御システムへのポインタ
	GFL_TCB* tcb;					///<保持しているTCBへのポインタ
	GFL_TCB* drawtcb;				///<保持している描画TCBへのポインタ
	const FLDMAPFUNC_DATA * data;	///<定義データへのポインタ
	void * free_work;				///<それぞれで使用するワークへのポインタ
  FSOverlayID ov_id;      ///< オーバーレイID
};

#define	DEFAULT_FREEWORK_SIZE	(sizeof(FREE_WORK))

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：システム生成、初期化
 * @param	fsys			フィールド制御ワークへのポインタ
 * @param	heapID			使用するヒープの指定
 * @param	max
 * @return	FLDMAPFUNC_SYS	フィールドマップ制御タスクシステムのシステムワークへのポインタ
 */
//------------------------------------------------------------------
FLDMAPFUNC_SYS * FLDMAPFUNC_Sys_Create(FIELDMAP_WORK * fsys, HEAPID heapID, int max)
{
	FLDMAPFUNC_SYS * sys;
	u32 tcb_work_size;
	sys = GFL_HEAP_AllocClearMemory(heapID, sizeof(FLDMAPFUNC_SYS));
	sys->fsys = fsys;
	sys->heapID = heapID;
	sys->max = max;
	sys->array = GFL_HEAP_AllocClearMemory(heapID, sizeof(FLDMAPFUNC_WORK) * max);

	///描画用TCB生成
	///なぜfieldmap_funcが描画用TCBを持つのかの理由
	///描画用のTCBをfieldmap_work.c/hで持っていても良いですが、
	///描画用TCB全体の破棄をするときに問題があります。
	///個々のタスク内でメモリを確保していると、
	///全体破棄時にメモリリークがおきてしまいます。
	///ですので、破棄処理が別に用意されている
	///fieldmap_funcの中に描画処理を追加して
	///メモリリークの起こらないようにします。
	tcb_work_size = GFL_TCB_CalcSystemWorkSize( max );
	sys->drawsys = GFL_HEAP_AllocClearMemory( heapID, tcb_work_size );
	sys->drawsys = GFL_TCB_Init( max, sys->drawsys );

	sys->mainsys = GFL_HEAP_AllocClearMemory( heapID, tcb_work_size );
	sys->mainsys = GFL_TCB_Init( max, sys->mainsys );
	
	return sys;
}

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：システム終了、解放
 * @return	sys		フィールドマップ制御タスクシステムのシステムワークへのポインタ
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Sys_Delete(FLDMAPFUNC_SYS * sys)
{
	int i;
	for (i = 0; i < sys->max; i++) {
		FLDMAPFUNC_Delete(&sys->array[i]);
	}
	GFL_HEAP_FreeMemory(sys->array);
	GFL_HEAP_FreeMemory( sys->mainsys );		///<メインTCB破棄
	GFL_HEAP_FreeMemory( sys->drawsys );		///<描画TCB破棄
	GFL_HEAP_FreeMemory(sys);
}

//----------------------------------------------------------------------------
/**
 *	@brief	フィールドマップ制御タスク：メイン
 */
//-----------------------------------------------------------------------------
void FLDMAPFUNC_Sys_Main(FLDMAPFUNC_SYS * sys)
{
	// メインタスク実行
	GFL_TCB_Main( sys->mainsys );
}

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：システム3D描画
 * @return	sys		フィールドマップ制御タスクシステムのシステムワークへのポインタ
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Sys_Draw3D(FLDMAPFUNC_SYS * sys)
{
	// 描画タスク実行
	GFL_TCB_Main( sys->drawsys );
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスクの呼び出し処理
 * @param	tcb		使用するTCBへのポインタ
 * @param	work	使用するワークへのポインタ（この場合、必ずFLDMAPFUNC_WORKへのポインタ）
 */
//------------------------------------------------------------------
static void FLDMAPFUNC_Tcb(GFL_TCB* tcb, void * work)
{
	FLDMAPFUNC_WORK * fwk = work;
	if( fwk->data->update_func ){
		fwk->data->update_func(fwk, fwk->sys->fsys, fwk->free_work);
	}
}

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ描画タスクの呼び出し処理
 * @param	tcb		使用するTCBへのポインタ
 * @param	work	使用するワークへのポインタ（この場合、必ずFLDMAPFUNC_WORKへのポインタ）
 */
//------------------------------------------------------------------
static void FLDMAPFUNC_DrawTcb(GFL_TCB* tcb, void * work)
{
	FLDMAPFUNC_WORK * fwk = work;
	if( fwk->data->draw3d_func ){
		fwk->data->draw3d_func(fwk, fwk->sys->fsys, fwk->free_work);
	}
}

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：生成
 * @param	sys		フィールドマップ制御タスクシステムのシステムワークへのポインタ
 * @param	data	生成しようとしている制御タスクの定義データ
 * @return	FLDMAPFUNC_WORK	生成した制御タスクのワークへのポインタ
 */
//------------------------------------------------------------------
FLDMAPFUNC_WORK * FLDMAPFUNC_Create(FSOverlayID ov_id, FLDMAPFUNC_SYS * sys, const FLDMAPFUNC_DATA * data)
{
	int i;
	FLDMAPFUNC_WORK * fwk;
	for (fwk = sys->array, i = 0; i < sys->max; fwk ++, i++) {
		if (fwk->tcb == NULL) {

      // Overlay読み込み
      if(ov_id != NO_OVERLAY_ID){
        GFL_OVERLAY_Load( ov_id );
      }
      fwk->ov_id = ov_id;
			fwk->tcb = GFL_TCB_AddTask(sys->mainsys, FLDMAPFUNC_Tcb, fwk, data->pri);
			fwk->drawtcb = GFL_TCB_AddTask(sys->drawsys, FLDMAPFUNC_DrawTcb, fwk, data->pri);
			fwk->sys = sys;
			fwk->data = data;
			GF_ASSERT(fwk->tcb != NULL);
			GF_ASSERT(fwk->drawtcb != NULL);
			if (data->work_size != 0) {
				fwk->free_work = GFL_HEAP_AllocClearMemory(sys->heapID, data->work_size);
			}
			if( data->create_func ){
				data->create_func(fwk, sys->fsys, fwk->free_work);
			}
			return fwk;
		}
	}
	GF_ASSERT_MSG(0, "FLDMAPFUNC:新規追加に失敗しました\n");
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：消去
 * @param	fwk		FLDMAPFUNC_WORKへのポインタ
 */
//------------------------------------------------------------------
void FLDMAPFUNC_Delete(FLDMAPFUNC_WORK * fwk)
{
	if (fwk->tcb == NULL) {
		return;
	}
	//個別消去処理呼び出し
	if( fwk->data->delete_func ){
		fwk->data->delete_func(fwk, fwk->sys->fsys, fwk->free_work);	}
	if (fwk->data->work_size != 0) {
		GFL_HEAP_FreeMemory(fwk->free_work);
	}
	GFL_TCB_DeleteTask(fwk->tcb);
	GFL_TCB_DeleteTask(fwk->drawtcb);

  // Overlay破棄
  if(fwk->ov_id != NO_OVERLAY_ID){
		GFL_OVERLAY_Unload( fwk->ov_id );
  }
	GFL_STD_MemClear(fwk, sizeof(FLDMAPFUNC_WORK));
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	フィールドマップ制御タスク：フリーワークポインタ取得
 * @param	fwk		FLDMAPFUNC_WORKへのポインタ
 * @retval	フリーワークのポインタ
 *		何かフラグを立てるなど今必要という
 *		時にこの関数で、ワークを取得すること。
 *		自動でワークが破棄されるので、ずっと保持しないこと。
 */
//------------------------------------------------------------------
void * FLDMAPFUNC_GetFreeWork(FLDMAPFUNC_WORK * fwk)
{
	return fwk->free_work;
}

//------------------------------------------------------------------
//------------------------------------------------------------------


//------------------------------------------------------------------
//------------------------------------------------------------------
