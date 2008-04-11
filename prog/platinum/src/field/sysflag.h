//============================================================================================
/**
 * @file	sysflag.c
 * @bfief	プログラム、スクリプトからアクセスされるシステムフラグ操作
 * @author	Satoshi Nohara
 * @date	05.11.01
 */
//============================================================================================
#ifndef	__SYSFLAG_H__
#define	__SYSFLAG_H__


#include "field/eventflag.h"


//============================================================================================
//	フラグ定義
//
//	SysFlag_...Set
//	SysFlag_...Check
//	関数を作り、そこからアクセスするようにする
//
//============================================================================================
#include "sysflag_def.h"


//--------------------------------------------------------------------------------------------
//	フラグ操作用モード定義
//--------------------------------------------------------------------------------------------
typedef enum {
	SYSFLAG_MODE_RESET = 0,	///<フラグ操作：リセット
	SYSFLAG_MODE_SET,		///<フラグ操作：セット
	SYSFLAG_MODE_CHECK,		///<フラグ操作：取得
}SYSFLAG_MODE;


//============================================================================================
//
//	バッグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * バッグ入手フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BagSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * バッグ入手フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_BagCheck( EVENTWORK* ev );


//============================================================================================
//
//	カンナギのギンガ団イベント関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * カンナギのギンガ団イベントフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_T05GingaEventSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * カンナギのギンガ団イベントフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_T05GingaEventCheck( EVENTWORK* ev );


//============================================================================================
//
//	ゲームクリア関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ゲームクリアフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_GameClearSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * ゲームクリアフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_GameClearCheck( EVENTWORK* ev );


//============================================================================================
//
//	地下初回イベントフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 地下初回イベントフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgFirstSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下初回イベントフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgFirstReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下初回イベントフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgFirstCheck( EVENTWORK* ev );


//============================================================================================
//
//	連れ歩きフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 連れ歩きフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_PairSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 連れ歩きフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_PairReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 連れ歩きフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_PairCheck( EVENTWORK* ev );


//============================================================================================
//
//	一歩動いたかフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 一歩動いたかフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_OneStepSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 一歩動いたかフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_OneStepReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 一歩動いたかフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_OneStepCheck( EVENTWORK* ev );


//============================================================================================
//
//	コンテスト マスターランククリアフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * コンテスト マスターランククリア フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	type	コンテストタイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ConMasterSet( EVENTWORK* ev, int type );

//--------------------------------------------------------------------------------------------
/**
 * コンテスト マスターランククリア フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	type	コンテストタイプ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ConMasterReset( EVENTWORK* ev, int type );

//--------------------------------------------------------------------------------------------
/**
 * コンテスト マスターランククリア フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	type	コンテストタイプ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_ConMasterCheck( EVENTWORK* ev, int type );


//============================================================================================
//
//	バトルサーチャー：フラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャー フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	lv		レベル
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BsLvSet( EVENTWORK* ev, int lv );

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャー フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	lv		レベル
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BsLvReset( EVENTWORK* ev, int lv );

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャー フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	lv		レベル
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_BsLvCheck( EVENTWORK* ev, int lv );

//--------------------------------------------------------------------------------------------
/**
 * イベントBGM切り替え フラグのチェックをして、切り替えるBGMナンバーを取得
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	zone_id ゾーンID
 *
 * @retval	"0		= フラグオフなので何もなし"
 * @retval	"0以外	= 変更するBGMナンバー"
 */
//--------------------------------------------------------------------------------------------
extern u16 SysFlag_EventBgmGet( EVENTWORK* ev, int zone_id );


//============================================================================================
//
//	通信カウンター通過フラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 通信カウンター通過フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_CommCounterSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 通信カウンター通過フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_CommCounterReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 通信カウンター通過フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_CommCounterCheck( EVENTWORK* ev );


//============================================================================================
//
//	ポケモンステータス：コンテスト技フラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモンステータス：コンテスト技フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_PSTContestSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンステータス：コンテスト技フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_PSTContestReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンステータス：コンテスト技フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_PSTContestCheck( EVENTWORK* ev );


//============================================================================================
//
//	GTCオープンフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * GTCオープンフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_GTCOpenSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * GTCオープンフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_GTCOpenReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * GTCオープンフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_GTCOpenCheck( EVENTWORK* ev );

//============================================================================================
//
//	バトルタワーオープンフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * バトルタワーオープンフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BTowerOpenSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * バトルタワーオープンフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BTowerOpenReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * バトルタワーオープンフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_BTowerOpenCheck( EVENTWORK* ev );

//============================================================================================
//
//	船が使えるフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 船が使えるフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ShipSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 船が使えるフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ShipReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 船が使えるフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_ShipCheck( EVENTWORK* ev );


//============================================================================================
//
//	レイアイハイ救出フラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * レイアイハイ救出フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ReiAiHaiSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * レイアイハイ救出フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_ReiAiHaiReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * レイアイハイ救出フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_ReiAiHaiCheck( EVENTWORK* ev );


//============================================================================================
//
//	ミズキフラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ミズキフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_MizukiSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * ミズキフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_MizukiReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * ミズキフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_MizukiCheck( EVENTWORK* ev );


//============================================================================================
//		ポケッチフック処理関連
//============================================================================================
extern void SysFlag_PoketchHookSet(EVENTWORK * ev);
extern void SysFlag_PoketchHookReset(EVENTWORK * ev);
extern BOOL SysFlag_PoketchHookCheck(EVENTWORK * ev);

//============================================================================================
//		サファリモード関連
//============================================================================================
extern void SysFlag_SafariSet(EVENTWORK * ev);
extern void SysFlag_SafariReset(EVENTWORK * ev);
extern BOOL SysFlag_SafariCheck(EVENTWORK * ev);

//============================================================================================
//		ポケパークモード関連
//============================================================================================
extern void SysFlag_PokeParkSet(EVENTWORK * ev);
extern void SysFlag_PokeParkReset(EVENTWORK * ev);
extern BOOL SysFlag_PokeParkCheck(EVENTWORK * ev);

//============================================================================================
//		サイクリングロードモード関連
//============================================================================================
extern void SysFlag_CyclingRoadSet(EVENTWORK * ev);
extern void SysFlag_CyclingRoadReset(EVENTWORK * ev);
extern BOOL SysFlag_CyclingRoadCheck(EVENTWORK * ev);

//============================================================================================
//		ひでんわざ関連
//============================================================================================
//--------------------------------------------------------------------------------------------
//		フラッシュ状態フラグ
//--------------------------------------------------------------------------------------------
///フラッシュ状態フラグセット
extern void SysFlag_FlashSet(EVENTWORK* ev);
///フラッシュ状態フラグリセット
extern void SysFlag_FlashReset(EVENTWORK* ev);
///フラッシュ状態フラグチェック
extern BOOL SysFlag_FlashCheck(EVENTWORK* ev);

//--------------------------------------------------------------------------------------------
//		きりばらい状態フラグ
//--------------------------------------------------------------------------------------------
///きりばらい状態フラグセット
extern void SysFlag_KiribaraiSet(EVENTWORK* ev);
///きりばらい状態フラグリセット
extern void SysFlag_KiribaraiReset(EVENTWORK* ev);
///きりばらい状態フラグチェック
extern BOOL SysFlag_KiribaraiCheck(EVENTWORK* ev);

//--------------------------------------------------------------------------------------------
//		かいりきフラグ
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_Kairiki( EVENTWORK* ev, SYSFLAG_MODE mode);

//--------------------------------------------------------------------------------------------
/**
 * @brief	かいりきフラグのセット
 * @param	ev		EVENTWORKのポインタ
 */
//--------------------------------------------------------------------------------------------
static inline void SysFlag_KairikiSet( EVENTWORK* ev )
{
	SysFlag_Kairiki(ev, SYSFLAG_MODE_SET);
}
//--------------------------------------------------------------------------------------------
/**
 * @brief	かいりきフラグのリセット
 * @param	ev		EVENTWORKのポインタ
 */
//--------------------------------------------------------------------------------------------
static inline void SysFlag_KairikiReset( EVENTWORK* ev )
{
	SysFlag_Kairiki(ev, SYSFLAG_MODE_RESET);
}
//--------------------------------------------------------------------------------------------
/**
 * @brief	かいりきフラグのチェック
 * @param	ev		EVENTWORKのポインタ
 * @return	BOOL	ON/OFF
 */
//--------------------------------------------------------------------------------------------
static inline BOOL SysFlag_KairikiCheck( EVENTWORK* ev )
{
	return SysFlag_Kairiki(ev, SYSFLAG_MODE_CHECK);
}

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャーのバッテリーを使用しているかフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BtlSearcherUseSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャーのバッテリーを使用しているかフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_BtlSearcherUseReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * バトルサーチャーのバッテリーを使用しているかフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_BtlSearcherUseCheck( EVENTWORK* ev );


//============================================================================================
//
//	地下フラグ関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 地下：到着フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgArriveSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：到着フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgArriveReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：到着フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgArriveCheck( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：化石フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgDigSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：化石フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgDigReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：化石フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgDigCheck( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：玉フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgTamaSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：玉フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgTamaReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：玉フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgTamaCheck( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：基地フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgBaseSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：基地フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgBaseReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：基地フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgBaseCheck( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：模様替えフラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgGoodsSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：模様替えフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgGoodsReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：模様替えフラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgGoodsCheck( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：旗フラグのセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgHataSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：旗フラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_UgHataReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 地下：旗フラグのチェック
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_UgHataCheck( EVENTWORK* ev );


//============================================================================================
//		サイクリングロードゲートを通過したかフラグ
//
//		*サイクリングロードゲートで、自転車から降りられない判定で使用している
//		*マップ遷移で落ちるようになっているので、ゲート内でのみ有効
//============================================================================================
extern BOOL SysFlag_BicycleGatePassage( EVENTWORK* ev, SYSFLAG_MODE mode );


//============================================================================================
//		冒険ノート関連
//============================================================================================
//--------------------------------------------------------------------------------------------
//		冒険ノート入手フラグ
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_FNote( EVENTWORK* ev, SYSFLAG_MODE mode);

//--------------------------------------------------------------------------------------------
/**
 * @brief	冒険ノート入手フラグのセット
 * @param	ev		EVENTWORKのポインタ
 */
//--------------------------------------------------------------------------------------------
static inline void SysFlag_FNoteSet( EVENTWORK* ev )
{
	SysFlag_FNote(ev, SYSFLAG_MODE_SET);
}
//--------------------------------------------------------------------------------------------
/**
 * @brief	冒険ノート入手フラグのリセット
 * @param	ev		EVENTWORKのポインタ
 */
//--------------------------------------------------------------------------------------------
static inline void SysFlag_FNoteReset( EVENTWORK* ev )
{
	SysFlag_FNote(ev, SYSFLAG_MODE_RESET);
}
//--------------------------------------------------------------------------------------------
/**
 * @brief	冒険ノート入手フラグのチェック
 * @param	ev		EVENTWORKのポインタ
 * @return	BOOL	ON/OFF
 */
//--------------------------------------------------------------------------------------------
static inline BOOL SysFlag_FNoteCheck( EVENTWORK* ev )
{
	return SysFlag_FNote(ev, SYSFLAG_MODE_CHECK);
}

//============================================================================================
//
//		到着フラグ関連
//
//============================================================================================
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_Arrive( EVENTWORK * ev, SYSFLAG_MODE mode, u32 arrive_id);

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static inline void SysFlag_ArriveSet( EVENTWORK * ev, u32 arrive_id)
{
	SysFlag_Arrive(ev, SYSFLAG_MODE_SET, arrive_id);
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static inline void SysFlag_ArriveReset( EVENTWORK * ev, u32 arrive_id)
{
	SysFlag_Arrive(ev, SYSFLAG_MODE_RESET, arrive_id);
}
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
static inline BOOL SysFlag_ArriveGet( EVENTWORK * ev, u32 arrive_id)
{
	return SysFlag_Arrive(ev, SYSFLAG_MODE_CHECK, arrive_id);
}

//============================================================================================
//
//		イベント関連
//
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * WiFiに繋いだ
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_WifiUseSet( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * 別荘　家具フラグセット
 *
 * @param	ev		EVENTWORKのポインタ
 * @param	mode	SYSFLAG_MODE
 * @param	no		家具番号 VILLA_FTURE_MAX未満
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern BOOL SysFlag_VillaFurniture( EVENTWORK* ev, SYSFLAG_MODE mode, int no );


//============================================================================================
//
//	別荘関連
//
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 別荘外のOBJが中に入ったかフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_T07ObjInReset( EVENTWORK* ev );

//--------------------------------------------------------------------------------------------
/**
 * POSでOBJが表示されているフラグのリセット
 *
 * @param	ev		EVENTWORKのポインタ
 *
 * @retval	none
 */
//--------------------------------------------------------------------------------------------
extern void SysFlag_T07R0201PosAppearReset( EVENTWORK* ev );

//============================================================================================
//	破れた世界関連
//============================================================================================
extern BOOL SysFlag_TornWorldGirasFlag( EVENTWORK* ev, SYSFLAG_MODE mode, int no );
extern BOOL SysFlag_TornWorldRockFinish( EVENTWORK* ev, SYSFLAG_MODE mode );
extern BOOL SysFlag_TornWorldRockLiftFlagCheck( EVENTWORK* ev );

//============================================================================================
//
//		デバッグ機能関連
//
//============================================================================================
#ifdef PM_DEBUG
//システムフラグ系をゲットする
extern u32 DNumInput_SysFlagGet(SAVEDATA* sv,u32 param);
//システムフラグ系をセットする
extern void DNumInput_SysFlagSet(SAVEDATA* sv,u32 param,u32 value);
#endif	//PM_DEBUG


#endif	//__SYSFLAG_H__


