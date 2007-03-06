//=============================================================================
/**
 * @file	procsys.h
 * @brief	プロセス管理ヘッダ
 * @date	2005.07.25
 * 
 * 2006.11.13	DPプロジェクトから移動、改変開始
 */
//=============================================================================

#ifndef	__PROCSYS_H__
#define	__PROCSYS_H__

#include <nitro.h>
#include <nnsys.h>

#include "heapsys.h"
#include "gf_overlay.h"

//===========================================================================
//
//		定義
//
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の結果
 */
//------------------------------------------------------------------
typedef enum {
	GFL_PROC_RES_CONTINUE = 0,		///<動作継続中
	GFL_PROC_RES_FINISH				///<動作終了
}GFL_PROC_RESULT;

//------------------------------------------------------------------
/**
 * @brief	プロセス構造体への不完全型
 *
 * 構造体の実際の定義はprocsys.cに封じられている
 */
//------------------------------------------------------------------
typedef struct _GFL_PROC GFL_PROC;

//------------------------------------------------------------------
/**
 * @brief	プロセス制御構造体への不完全型
 *
 * 構造体の実際の定義はprocsys.cに封じられている
 */
//------------------------------------------------------------------
typedef struct _GFL_PROCSYS GFL_PROCSYS;

//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の型定義
 *
 * フィールド、戦闘、メニューなどの動作関数は下記の形式で作成する
 */
//------------------------------------------------------------------
typedef GFL_PROC_RESULT (*GFL_PROC_FUNC)(GFL_PROC *, int *, void *, void *);

//------------------------------------------------------------------
/**
 * @brief	プロセス定義データ
 *
 * プロセスの初期化・メイン・終了関数を登録したデータ
 */
//------------------------------------------------------------------
typedef struct {
	GFL_PROC_FUNC init_func;		///<プロセス初期化関数
	GFL_PROC_FUNC main_func;		///<プロセスメイン関数
	GFL_PROC_FUNC end_func;			///<プロセス終了関数
}GFL_PROC_DATA;

//------------------------------------------------------------------
/**
 * @brief	無効オーバーレイ指定
 */
//------------------------------------------------------------------
#define	NO_OVERLAY_ID	(0xffffffff)


//===========================================================================
//
//		関数外部参照
//
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	PROCシステムの初期化処理
 * @param	heap_id		PROCシステムで使用するヒープID
 */
//------------------------------------------------------------------
extern void GFL_PROC_SysInit(HEAPID heap_id);

//------------------------------------------------------------------
/**
 * @brief	PROCシステムのメイン処理
 */
//------------------------------------------------------------------
extern void GFL_PROC_SysMain(void);

//------------------------------------------------------------------
/**
 * @brief	PROCシステムの終了処理
 */
//------------------------------------------------------------------
extern void GFL_PROC_SysExit(void);

//------------------------------------------------------------------
/**
 * @brief	PROCの呼び出し（下位コール）
 * @param	ov_id		新しいプロセスが存在するオーバーレイID
 * @param	procdata	新しいプロセスの定義データアドレス
 * @param	pwork		パラメータワークへのポインタ
 *
 * この関数を呼び出した次のフレームから指定したプロセスが呼び出される。
 * 呼び出されたプロセスが終了すると、現在のプロセスへと自動復帰する。
 */
//------------------------------------------------------------------
extern void GFL_PROC_SysCallProc(FSOverlayID ov_id, const GFL_PROC_DATA * procdata, void * pwork);

//------------------------------------------------------------------
/**
 * @brief	PROCの切り替え
 * @param	ov_id		新しいプロセスが存在するオーバーレイID
 * @param	procdata	新しいプロセスの定義データアドレス
 * @param	pwork		パラメータワークへのポインタ
 *
 * 現在のプロセスが終了した後に、この関数で指定したプロセスへと制御が切り替わる。
 */
//------------------------------------------------------------------
extern void GFL_PROC_SysSetNextProc(FSOverlayID ov_id, const GFL_PROC_DATA * procdata, void * pwork);

//------------------------------------------------------------------
/**
 * @brief	プロセス内ワークの確保
 * @param	proc	プロセスへのポインタ
 * @param	size	確保するワークサイズ
 * @param	heap_id	使用するヒープ
 * @return	void *	確保したプロセス内ワークへのポインタ
 */
//------------------------------------------------------------------
extern void * GFL_PROC_AllocWork(GFL_PROC * proc, unsigned int size, HEAPID heap_id);

//------------------------------------------------------------------
/**
 * @brief	プロセス内ワークの解放
 * @param	proc	プロセスへのポインタ
 */
//------------------------------------------------------------------
extern void GFL_PROC_FreeWork(GFL_PROC * proc);



#endif /* __PROCSYS_H__ */

