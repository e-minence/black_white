//=============================================================================================
/**
 * @file	tcb.h
 * @brief	汎用タスク制御モジュールヘッダ
 * @author	tamada, taya
 * @date	2006.11.06
 *
 * @since	2003.02.20
 *
 * AGBライブラリ→ポケモンダイヤモンド＆パールプロジェクトからそのまま移動
 */
//=============================================================================================

#ifndef	__TCB_H__
#define	__TCB_H__




/*====================================================================================*/
/*  型宣言                                                                            */
/*====================================================================================*/

//------------------------------------------------------------------
/**
 * @brief	TCBSYS 型宣言
 *
 * TCBシステムワーク構造体の宣言。
 * メイン処理用・VBlank用など、任意の箇所で実行される
 * 複数のTCBシステムを作成することが出来る。
 *
 * 内容は隠蔽されており、直接のアクセスはできない。
  */
//------------------------------------------------------------------
typedef struct _TCBSYS		TCBSYS;

//------------------------------------------------------------------
/**
 * @brief	TCBポインタ型
 *
 * TCBへはこのポインタ型経由でアクセスする。
 * TCB構造体を直接アクセスできないようになっている。
 */
//------------------------------------------------------------------
typedef	struct _TCB * TCB_PTR;

//------------------------------------------------------------------
/**
 * @brief	TCB関数型
 *
 * TCBに登録する実行関数の型定義。
 * 引数としてTCB_PTRと、ワークへのポインタをとるようになっている
 */
//------------------------------------------------------------------
typedef void (*TCB_FUNC)( TCB_PTR, void * );

/*====================================================================================*/
/*  関数宣言                                                                          */
/*====================================================================================*/


//------------------------------------------------------------------
/**
 * TCB システムを構築するのに必要なメモリ量を計算
 *
 * @param   task_max		稼働できる最大タスク数
 *
 * @retval  u32		メモリサイズ（バイト単位）
 */
//------------------------------------------------------------------
extern u32 TCBSYS_CalcSystemWorkSize( u32 task_max );

//------------------------------------------------------------------
/**
 * TCBシステムを作成
 *
 * @param   task_max		稼働できる最大タスク数
 * @param   work_area		システム作成に必要充分なサイズのワークエリアアドレス
 *
 * @retval  TCBSYS*		作成されたTCBシステムポインタ
 *
 * work_area に必要なサイズは、TCBSYS_CalcSystemWorkSize で計算する。
 */
//------------------------------------------------------------------
extern TCBSYS*  TCBSYS_Create( u32 task_max, void* work_area );

//------------------------------------------------------------------
/**
 *	TCBシステム使用前の初期化
 */
//------------------------------------------------------------------
extern void TCBSYS_Init( TCBSYS* tcbsys );

//------------------------------------------------------------------
/**
 *	@brief	TCBシステムメイン
 *
 *	TCBシステムに登録されている各タスクを優先順に呼び出す
 *
 *  @param	tcbsys		TCBシステムワークポインタ
 *
 */
//------------------------------------------------------------------
extern void TCBSYS_Main( TCBSYS* tcbsys );

//------------------------------------------------------------------------------
/**
	@brief	TCBを追加する
	@param	tcbsys	TCBシステムポインタ
	@param	func	TCB_FUNC:関連付ける実行関数ポインタ
	@param	work	void*:関連付けるワークエリアへのvoid型ポインタ
	@param	pri		u32:タスクプライオリティ

	@return	TCB_PTR	追加したTCBを示すポインタ
*/
//------------------------------------------------------------------------------
extern TCB_PTR TCBSYS_AddTask( TCBSYS* tcbsys, TCB_FUNC func, void* work, u32 pri );


//------------------------------------------------------------------------------
/**
	@brief	TCBを消去する
	@param	tcb		TCBポインタ
*/
//------------------------------------------------------------------------------
extern void TCBSYS_DeleteTask( TCB_PTR tcb );


//------------------------------------------------------------------
/**
 * @brief	TCB関数の切り替え
 *
 * TCBの動作関数を切り替える
 *
 * @param	tcb		対象となるTCBへのポインタ
 * @param	func	新しく切り替える動作関数
*/
//------------------------------------------------------------------
extern void TCB_ChangeFunc(TCB_PTR tcb, TCB_FUNC func);

//------------------------------------------------------------------
/**
 * @brief	TCBワークの取得
 *
 * TCBの保持しているワークアドレスを取得する
 *
 * @param	tcb	TCBへのポインタ
 * @return	ワークへのポインタ
 */ 
//------------------------------------------------------------------
extern void * TCB_GetWork(TCB_PTR tcb);

//------------------------------------------------------------------
/**
 * @brief	TCBプライオリティの取得
 *
 * TCBの動作優先順位を取得する
 *
 * @param	tcb	TCBへのポインタ
 * @return	プライオリティの値
 */
//------------------------------------------------------------------
extern u32 TCB_GetPriority(TCB_PTR tcb);


#endif /*__TCB_H__*/
