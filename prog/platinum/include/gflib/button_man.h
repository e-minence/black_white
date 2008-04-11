//============================================================================================
/**
 * @file	button_man.h
 * @bfief	ボタン状態管理マネージャ
 * @date	2005.11.08
 * @author	taya GAME FREAK inc.
 */
//============================================================================================
#ifndef __BUTTON_MAN_H__
#define __BUTTON_MAN_H__

#include "gflib\touchpanel.h"

//--------------------------------------------------------------
/**
 *  ボタンイベント種類の定義
 *
 *  マネージャに登録したボタンについて、これらのイベントが発生すると
 *  登録コールバック関数が呼び出される
 */
//--------------------------------------------------------------
enum {
	BMN_EVENT_TOUCH,		///< 触れた瞬間
	BMN_EVENT_RELEASE,		///< 離された瞬間
	BMN_EVENT_HOLD,			///< 触りつづけている
	BMN_EVENT_SLIDEOUT,		///< 画面タッチしたままボタン領域から外れた瞬間
};


//--------------------------------------------------------------
/**
 * コールバック関数型定義。
 * 引数内容は以下の順である。
 *
 * u32		ボタンID
 * u32		イベント種類
 * void*	マネージャ作成時に引数として設定した任意ポインタ
 *
 */
//--------------------------------------------------------------
typedef		void (*pBmnCallBackFunc)(u32, u32, void*);


//--------------------------------------------------------------
/**
 * ボタン管理マネージャのワーク構造体宣言
 */
//--------------------------------------------------------------
typedef		struct _BUTTON_MAN		BUTTON_MAN;




//======================================================================================================
/**
 * ボタン管理マネージャ作成
 *
 * @param   hit_tbl			[in]  当たり判定テーブルへのポインタ
 * @param   btn_cnt			[in]  hit_tbl の要素数（=これが登録するボタンの数ということ）
 * @param   callback		[in]  ボタンイベント発生時に呼び出されるコールバック関数
 * @param   callback_arg	[in]  callback 呼び出し時に引数として渡される任意のポインタ
 * @param   heapID			[in]  マネージャ領域作成用ヒープID
 *
 * @retval  BUTTON_MAN*		マネージャポインタ
 *
 *
 * btn_cnt の数だけ、内部ではボタン管理用ワークが生成される。
 * コールバックで引き渡されるボタンIDというのは、0 〜 (btn_cnt-1）の数値で、
 * hit_tbl の何件目に該当するかを示す。
 *
 */
//======================================================================================================
extern BUTTON_MAN* BMN_Create( const TP_HIT_TBL *hit_tbl, u32 btn_cnt, pBmnCallBackFunc callback, void *callback_arg, u32 heapID );


//======================================================================================================
/**
 * ボタン管理マネージャメイン
 *
 * 各ボタン状態のチェック＆更新、イベントが発生すればコールバック呼び出しを行う。
 *
 * @param   bmn		マネージャポインタ
 */
//======================================================================================================
extern void BMN_Main( BUTTON_MAN *bmn );


//======================================================================================================
/**
 * ボタン管理マネージャ消去
 *
 * @param   bmn		マネージャポインタ
 */
//======================================================================================================
extern void BMN_Delete( BUTTON_MAN *bmn );


//======================================================================================================
/**
 * ボタンがホールド状態にある時、そのホールド時間（フレーム数）を返す
 *
 * @param   bmn		マネージャワーク
 * @param   idx		ボタンインデックス
 *
 * @retval  u32		ホールド中ならホールド時間、ホールド中以外なら０
 */
//======================================================================================================
extern u32 BMN_GetHoldFrameCount( BUTTON_MAN* bmn, u32 idx );


#endif
