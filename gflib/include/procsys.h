//=============================================================================
/**
 * @file	プロセス管理ヘッダ
 * @brief
 * @date	2005.07.25
 */
//=============================================================================

#ifndef	__PROCSYS_H__
#define	__PROCSYS_H__

//===========================================================================
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の結果
 */
//------------------------------------------------------------------
typedef enum {
	PROC_RES_CONTINUE = 0,		///<動作継続中
	PROC_RES_FINISH				///<動作終了
}PROC_RESULT;

//------------------------------------------------------------------
/**
 * @brief	プロセス構造体への不完全型
 *
 * 構造体の実際の定義はprocsys.cに封じられている
 */
//------------------------------------------------------------------
typedef struct _PROC PROC;

//------------------------------------------------------------------
/**
 * @brief	プロセス動作関数の型定義
 *
 * フィールド、戦闘、メニューなどの動作関数は下記の形式で作成する
 */
//------------------------------------------------------------------
typedef PROC_RESULT (*PROC_FUNC)(PROC *, int *);

//------------------------------------------------------------------
/**
 * @brief	プロセス定義データ
 *
 * プロセスの初期化・メイン・終了関数を登録したデータ
 * オーバーレイID指定は今のところダミー
 */
//------------------------------------------------------------------
#define	NO_OVERLAY_ID	(0xffffffff)

typedef struct {
	PROC_FUNC init_func;
	PROC_FUNC main_func;
	PROC_FUNC end_func;
	const FSOverlayID overlay_id;
}PROC_DATA;

//===========================================================================
//===========================================================================
extern PROC * GFL_PROC_Create(const PROC_DATA * data, void * parent_work, const u32 heap_id);
extern PROC * GFL_PROC_CreateChild(	PROC * proc,
								const PROC_DATA * data,
								void * parent_work,
								const u32 heap_id);

extern void GFL_PROC_Delete(PROC * proc);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL GFL_PROC_Main(PROC * proc);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void * GFL_PROC_AllocWork(PROC * proc, unsigned int size, u32 heap_id);
extern void * GFL_PROC_GetWork(PROC * proc);
extern void GFL_PROC_FreeWork(PROC * proc);
extern void * GFL_PROC_GetParentWork(PROC * proc);
//extern void PROC_SetPause(PROC * proc, BOOL pause_flag);



#endif /* __PROCSYS_H__ */
