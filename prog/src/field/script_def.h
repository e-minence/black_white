//============================================================================================
/**
 * @file	script_def.h
 * @brief	スクリプト定義ファイル
 * @author	Satoshi Nohara
 * @date	05.12.14
 *
 * スクリプト環境とプログラムとで共通して参照する必要がある定義を集約するヘッダファイル。
 * スクリプト側はenumなどC特有の文法解釈ができないため、defineを使用すること。
 */
//============================================================================================
#pragma once

//============================================================================================
//
//  スクリプトヘッダ関連
//
//  スクリプト側ではマクロに埋め込まれているため、直接これらの値を
//  意識的に使用することはない。
//============================================================================================
//-----------------------------------------------------------------------------
//特殊スクリプト定義
//-----------------------------------------------------------------------------
#define SP_SCRID_NONE				(0)
#define SP_SCRID_SCENE_CHANGE		(1)     ///<特殊スクリプトテーブルでのシーン発動イベントデータの指定ID
#define SP_SCRID_ZONE_CHANGE		(2)     ///<特殊スクリプトテーブルでのゾーン切り替えスクリプトの指定ID
#define SP_SCRID_FIELD_RECOVER	(3)			///<特殊スクリプトテーブルでのフィールド復帰スクリプト指定ID
#define SP_SCRID_FIELD_INIT 		(4)			///<特殊スクリプトテーブルでのフィールド初期化スクリプト指定ID

///イベントデータテーブル末尾を判定するための値
#define EV_DATA_END_CODE  (0xfd13)  //適当な値


//============================================================================================
//
//    OBJ関連
//
//============================================================================================
//-----------------------------------------------------------------------------
//エイリアス識別用スクリプトID
#define SP_SCRID_ALIES				(0xffff)

//トレーナータイプ定義
#define SCR_EYE_TR_TYPE_SINGLE		(0)
#define SCR_EYE_TR_TYPE_DOUBLE		(1)
#define SCR_EYE_TR_TYPE_TAG			(2)

//トレーナーの番号定義
#define SCR_EYE_TR_0				(0)
#define SCR_EYE_TR_1				(1)

//連れ歩きOBJを取得するOBJID定義
#define SCR_OBJID_MV_PAIR			(0xf2)

//透明ダミーOBJを取得するOBJID定義
#define SCR_OBJID_DUMMY				(0xf1)

//============================================================================================
//
//    各コマンド依存の定義
//
//============================================================================================
//-----------------------------------------------------------------------------
//fld_menu.c
//-----------------------------------------------------------------------------
#define SCR_REPORT_OK				(0)			//セーブした
#define SCR_REPORT_CANCEL			(1)			//セーブしていない

///YES_NO_WINの戻り値
#define SCR_YES   (0) //YES_NO_WINの戻り値 「はい」を返した
#define SCR_NO    (1) //YES_NO_WINの戻り値 「いいえ」を返した

///TRAINER_LOSE_CHECK,NORMAL_LOSE_CHECK
#define SCR_BATTLE_LOSE (0) //負け
#define SCR_BATTLE_WIN  (1) //勝ち

//ショップのID定義
#define SCR_SHOPID_NULL ( 0xFF )  //変動ショップ呼び出しID

//-----------------------------------------------------------------------------
//曜日取得関数の戻り値
//  _GET_WEEK
//-----------------------------------------------------------------------------
#define RTC_WEEK_SUNDAY		0
#define RTC_WEEK_MONDAY		1
#define RTC_WEEK_TUESDAY	2
#define RTC_WEEK_WEDNESDAY	3
#define RTC_WEEK_THURSDAY	4
#define RTC_WEEK_FRIDAY		5
#define RTC_WEEK_SATURDAY	6

//-----------------------------------------------------------------------------
//バッジフラグ指定IDの定義
//  _GET_BADGE_FLAG/_SET_BADGE_FLAG
//-----------------------------------------------------------------------------
#define BADGE_ID_01   0
#define BADGE_ID_02   1
#define BADGE_ID_03   2
#define BADGE_ID_04   3
#define BADGE_ID_05   4
#define BADGE_ID_06   5
#define BADGE_ID_07   6
#define BADGE_ID_08   7

//-----------------------------------------------------------------------------
// 手持ちポケモン数カウントモード
//-----------------------------------------------------------------------------
#define POKECOUNT_MODE_TOTAL         (0)  // 手持ちの数
#define POKECOUNT_MODE_NOT_EGG       (1)  // タマゴを除く手持ちの数
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // 戦える(タマゴと瀕死を除いた)ポケモン数
#define POKECOUNT_MODE_ONLY_EGG      (3)  // タマゴの数(駄目タマゴを除く)
#define POKECOUNT_MODE_ONLY_DAME_EGG (4)  // 駄目タマゴの数




/*
//フラグ操作関数のモード指定用
#define	FLAG_MODE_RESET				(0)
#define	FLAG_MODE_SET				(1)
#define	FLAG_MODE_GET				(2)

//図鑑モード
#define SCR_SHINOU_ZUKAN_MODE	0
#define SCR_ZENKOKU_ZUKAN_MODE	1

//パソコンアニメ管理ナンバー
#define SCR_PASO_ANM_NO		90


//技教え：3人いて、それぞれの色の技しか教えてくれない
#define WAZA_OSHIE_COL_BLUE			(0)
#define WAZA_OSHIE_COL_RED			(1)
#define WAZA_OSHIE_COL_YELLOW		(2)

//技覚え共通ウェイト
#define WAZA_COMMON_WAIT			(30)
#define WAZA_COMMON_WAIT2			(32)
#define WAZA_COMMON_WAIT3			(16)
*/

