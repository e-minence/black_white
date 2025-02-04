//============================================================================================
/**
 * @file	pms_input.h
 * @bfief	簡易会話入力画面
 * @author	taya
 * @date	06.01.20
 */
//============================================================================================
#ifndef __PMS_INPUT_H__
#define __PMS_INPUT_H__

#include "system/pms_data.h"


//------------------------------------------------------
/**
 *  入力画面用パラメータ型
 */
//------------------------------------------------------
typedef  struct _PMSI_PARAM		PMSI_PARAM;

//------------------------------------------------------
/**
 *  入力モード
 */
//------------------------------------------------------
typedef enum {
	PMSI_MODE_SINGLE,	///< 単語１個モード
	PMSI_MODE_DOUBLE,	///< 単語２個モード
	PMSI_MODE_SENTENCE,	///< 文章モード
}PMSI_MODE;


//------------------------------------------------------
/**
 *  説明文タイプ
 */
//------------------------------------------------------
enum PMSI_GUIDANCE {
	PMSI_GUIDANCE_DEFAULT,		///< 汎用的な文言
};
// PMSI_GUIDANCE は、ユーザに選んでもらう文は何かの説明メッセージを決めるだけのもの。
// ユーザの入力できる文を制限するものではない。
// 例
// ことばをえらんでね！
// たいせんまえのことばをかんがえよう
// かったときのきめゼリフをかんがえよう
// まけたときのいいわけをかんがえよう
// はやっているものは？
// resource/message/src/pms/pms_input.gmm
// info00
// 〜
// info04
// 2010年2月25日現在、使われていないので
// PMSI_PARAM_Create の guidance_type には PMSI_GUIDANCE_DEFAULT をいれておけばよい。

// 定型文のタイプを決めるのは
// prog/include/system/pms_data.h
// PMS_TYPE
// である。


//------------------------------------------------------
/**
 *  その他の関連定数
 */
//------------------------------------------------------
enum {
	PMS_INPUT_WORD_MAX = 2,		///< 入力可能な単語最大数
};


//------------------------------------------------------
/**
 *  簡易会話入力画面用パラメータ操作関数
 */
//------------------------------------------------------

//------------------------------------------------------------------
/**
 * 簡易会話入力画面パラメータ作成 
	*
	* @param   input_mode			入力モード（enum PMSI_MODE）
	* @param   guidance_type	説明文タイプ（enum PMSI_GUIDANCE）(PMSI_GUIDANCE_DEFAULTに限る)
	* @param   savedata			  セーブデータポインタ
  * @param   pms            文章を固定したい場合はPMS_DATAを指定。NULLにすれば固定化しない。
  * @param   picture_flag   TRUE:デコメ入力可能
	* @param   heapID				  作成用ヒープID
	*
	* @retval  PMSI_PARAM*		作成されたパラメータオブジェクトへのポインタ
	*/
//------------------------------------------------------------------
extern PMSI_PARAM*  PMSI_PARAM_Create( u32 input_mode, u32 guidance_type,
    PMS_DATA* pms, BOOL picture_flag, SAVE_CONTROL_WORK* savedata, u32 heapID );


//------------------------------------------------------------------
/**
 * 簡易会話入力画面パラメータ破棄
 *
 * @param   p		パラメータオブジェクトへのポインタ
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_Delete( PMSI_PARAM* p );



//------------------------------------------------------------------
/**
 * 簡易会話入力画面の初期状態に使用するパラメータセット（単語１つモード用）
 *
 * @param   p			パラメータオブジェクト
 * @param   word		セットする簡易会話単語コード
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_SetInitializeDataSingle( PMSI_PARAM* p, PMS_WORD word );

//------------------------------------------------------------------
/**
 * 簡易会話入力画面の初期状態に使用するパラメータセット（単語２つモード用）
 *
 * @param   p			パラメータオブジェクト
 * @param   word0		セットする簡易会話単語コード１
 * @param   word1		セットする簡易会話単語コード２
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_SetInitializeDataDouble( PMSI_PARAM* p, PMS_WORD word0, PMS_WORD word1 );

//------------------------------------------------------------------
/**
 * 簡易会話入力画面の初期状態に使用するパラメータセット（文章モード用）
 *
 * @param   p		パラメータオブジェクト
 * @param   pms		セットする簡易会話データポインタ
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_SetInitializeDataSentence( PMSI_PARAM* p, const PMS_DATA* pms );


//------------------------------------------------------------------
/**
 * 初期状態から変更がなくても、入力キャンセルとみなさないようにする
 *
 * @param   p		パラメータオブジェクト
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_SetNotEditEgnore( PMSI_PARAM* p );

//------------------------------------------------------------------
/**
 * 一度結果を受け取った後、もう１度、入力画面用パラメータとして使えるようにする
 *
 * @param   p		パラメータオブジェクト
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_SetReuseState( PMSI_PARAM* p );

//------------------------------------------------------------------
/**
 * 簡易会話入力画面の終了後、入力をキャンセルされたかどうかチェック
 *
 * @param   p			パラメータオブジェクト
 *
 * @retval  BOOL		TRUEでキャンセルされた
 */
//------------------------------------------------------------------
extern BOOL PMSI_PARAM_CheckCanceled( const PMSI_PARAM* p );

//------------------------------------------------------------------
/**
 * 簡易会話入力画面の終了後、初期状態から変更がなされたかチェック
 *
 * @param   p			パラメータオブジェクト
 *
 * @retval  BOOL		TRUEで変更された
 */
//------------------------------------------------------------------
extern BOOL PMSI_PARAM_CheckModified( const PMSI_PARAM* p );



//------------------------------------------------------------------
/**
 * 入力画面で入力されたデータを取得（単語１つモード）
 *
 * @param   p			パラメータオブジェクト
 *
 * @retval  PMS_WORD		入力データ（単語コード）
 */
//------------------------------------------------------------------
extern PMS_WORD  PMSI_PARAM_GetInputDataSingle( const PMSI_PARAM* p );

//------------------------------------------------------------------
/**
 * 入力画面で入力されたデータを取得（単語２つモード）
 *
 * @param   p		パラメータオブジェクト
 * @param   dst		入力されたデータを受け取るバッファ
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_GetInputDataDouble( const PMSI_PARAM* p,  PMS_WORD* dst );

//------------------------------------------------------------------
/**
 * 入力画面で入力されたデータを取得（文章モード）
 *
 * @param   p		パラメータオブジェクト
 * @param   pms		入力されたデータを受け取る構造体アドレス
 *
 */
//------------------------------------------------------------------
extern void PMSI_PARAM_GetInputDataSentence( const PMSI_PARAM* p, PMS_DATA* pms );









//------------------------------------------------------
/**
 *  簡易会話入力画面PROC関数
 */
//------------------------------------------------------
extern GFL_PROC_RESULT PMSInput_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSInput_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT PMSInput_Quit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern const GFL_PROC_DATA ProcData_PMSInput;
FS_EXTERN_OVERLAY(pmsinput);


#endif
