//============================================================================================
/**
 * @file	poketch_sound.h
 * @bfief	ポケッチ（SE指定）ヘッダ
 * @author	taya GAME FREAK inc.
 */
//============================================================================================
#ifndef __POKETCH_SOUND_H__
#define __POKETCH_SOUND_H__


#include "system\snd_tool.h"

enum {
// 基本操作
	POKETCH_SE_BUTTON_PUSH		= SEQ_SE_DP_DENSI01,		// ソフト切り替え
	POKETCH_SE_BEEP				= SEQ_SE_DP_BEEP,			// ロック中

	//POKETCH_SE_BUTTON_ROCK	= SEQ_SE_DP_POKETCH_005,	// ロック中ソフト切り替え
	//POKETCH_SE_BUTTON_ROCK	= SEQ_SE_DP_BEEP,			// ロック中ソフト切り替え
	POKETCH_SE_BUTTON_ROCK		= SEQ_SE_DP_DENSI04,		// ロック中ソフト切り替え

// メモ帳
	POKETCH_SE_MEMO_BUTTON		= SEQ_SE_DP_POKETCH_003,	// ボタン押した

// 電卓
	POKETCH_SE_CALC_BUTTON		= SEQ_SE_DP_POKETCH_003,	// ボタン押した

// カウンター
	//POKETCH_SE_COUNTER_BUTTON	= SEQ_SE_DP_POKETCH_003,	// ボタン押した
	POKETCH_SE_COUNTER_BUTTON	= SEQ_SE_DP_POKETCH_010,	// ボタン押した

// 万歩計
	//POKETCH_SE_PEDOMETER_BUTTON = SEQ_SE_DP_POKETCH_003,	// ボタン押した
	POKETCH_SE_PEDOMETER_BUTTON = SEQ_SE_DP_POKETCH_010,	// ボタン押した

// コイントス
	POKETCH_SE_COINTOSS_START	= SEQ_SE_DP_DENSI09,		// コインはじいた
	POKETCH_SE_COINTOSS_BOUND	= SEQ_SE_DP_DENSI10,		// コインバウンドした

// わざ相性チェッカー
	POKETCH_SE_AISYOU_BUTTON	= SEQ_SE_DP_POKETCH_003,	// ボタン押した

// アラーム時計
	POKETCH_SE_ALARM_BUTTON		= SEQ_SE_DP_POKETCH_003,	// ボタン押した
	POKETCH_SE_ALARM_BUTTON_DISABLE = SEQ_SE_DP_BEEP,		// 無効なボタン押した

// キッチンタイマー
	POKETCH_SE_KITCHEN_BUTTON	= SEQ_SE_DP_POKETCH_003,	// ボタン押した
	POKETCH_SE_KITCHEN_BUTTON_DISABLE = SEQ_SE_DP_BEEP,		// 無効なボタン押した
	POKETCH_SE_KITCHEN_POKO		= SEQ_SE_DP_DENSI11,		// カビゴンがポコポコたたく音

// カラーチェンジャー
	POKETCH_SE_CHANGER_CHANGE	= SEQ_SE_DP_POKETCH_003,	// 色変えた

// つうしんサーチャー
	POKETCH_SE_TUSIN_SWITCH_MODE = SEQ_SE_DP_POKETCH_003,	// 画面切り替わり（通常）
	POKETCH_SE_TUSIN_SWITCH_MODE_ERR = SEQ_SE_DP_BEEP,		// 画面切り替わり（エラー）

// ストップウォッチ
	//POKETCH_SE_STOPWATCH_PUSH = SEQ_SE_DP_POKETCH_007,
	POKETCH_SE_STOPWATCH_PUSH = SEQ_SE_DP_POKETCH_010,
	//POKETCH_SE_STOPWATCH_RESET = SEQ_SE_DP_POKETCH_007,
	POKETCH_SE_STOPWATCH_RESET	= SEQ_SE_DP_POKETCH_010,
	POKETCH_SE_STOPWATCH_BOMB	= SEQ_SE_DP_POKETCH_006,

// そだてやカメラ
	POKETCH_SE_SODATE_UPDATE	= SEQ_SE_DP_DENSI12,		// 画面更新

// ルーレット
	POKETCH_SE_ROULETTE_BUTTON = SEQ_SE_DP_POKETCH_003,		// ボタン押した
	//POKETCH_SE_ROULETTE_BUTTON	= SEQ_SE_DP_POKETCH_010,// ボタン押した
	POKETCH_SE_ROULETTE_BUTTON_DISABLE = SEQ_SE_DP_BEEP,	// 無効なボタン押した
	//POKETCH_SE_ROULETTE_STOP = SEQ_SE_DP_POKETCH_007,		// ルーレット止まった
	POKETCH_SE_ROULETTE_STOP	= SEQ_SE_DP_POKETCH_011,	// ルーレット止まった

// トレーナーランチャー
	POKETCH_SE_TRANER_BUTTON	= SEQ_SE_DP_POKETCH_003,	// ボタン押した
	POKETCH_SE_TRANER_BUTTON_DISABLE = SEQ_SE_DP_BEEP,		// 無効なボタンを押した

// ラブテスター
	//POKETCH_SE_LOVE_BUTTON = SEQ_SE_DP_POKETCH_003,		// ボタン押した
	POKETCH_SE_LOVE_BUTTON		= SEQ_SE_DP_POKETCH_010,	// ボタン押した
	POKETCH_SE_LOVE_BUTTON_DISABLE = SEQ_SE_DP_BEEP,		// ボタン無効状態で押した
	POKETCH_SE_LOVE_CHANGE_POKE = SEQ_SE_DP_POKETCH_003,	// ポケモン切り替え
	//POKETCH_SE_LOVE_MOVE_POKE_1 = SEQ_SE_DP_POKETCH_003,	// 移動中１段階目
	POKETCH_SE_LOVE_MOVE_POKE_1 = SEQ_SE_DP_POKETCH_012,	// 移動中１段階目
	//POKETCH_SE_LOVE_MOVE_POKE_2 = SEQ_SE_DP_POKETCH_003,	// 移動中２段階目
	POKETCH_SE_LOVE_MOVE_POKE_2 = SEQ_SE_DP_POKETCH_012,	// 移動中２段階目
	//POKETCH_SE_LOVE_MOVE_POKE_3 = SEQ_SE_DP_POKETCH_003,	// 移動中３段階目
	POKETCH_SE_LOVE_MOVE_POKE_3 = SEQ_SE_DP_POKETCH_012,	// 移動中３段階目
	//POKETCH_SE_LOVE_LV_0 = SEQ_SE_DP_POKETCH_007,			// レベル判定最低
	POKETCH_SE_LOVE_LV_0		= SEQ_SE_DP_POKETCH_013,	// レベル判定最低
	//POKETCH_SE_LOVE_LV_MAX = SEQ_SE_DP_POKETCH_006,		// レベル判定最高
	POKETCH_SE_LOVE_LV_MAX		= SEQ_SE_DP_POKETCH_014,	// レベル判定最高


// バックライトスイッチ
	POKETCH_SE_BACKLIGHT_SWITCH	= SEQ_SE_DP_POKETCH_003,		// ボタン押した

// ダウジングマシン
	//POKETCH_SE_DOWSING_TOUCH	= SEQ_SE_DP_POKETCH_003,		// 画面タッチした
	POKETCH_SE_DOWSING_TOUCH	= SEQ_SE_DP_POKETCH_009,		// 画面タッチした

// なつきどチェッカー
	POKETCH_SE_NATSUKI_JUMP = SEQ_SE_DP_POKETCH_010,				// ダブルタップで全ポケジャンプ

// つうしんサーチャー
	POKETCH_SE_TUSHIN_WARNING = SEQ_SE_DP_BEEP,			// 通信準備が整う前に開始しようとした

// かんいリスト
	POKETCH_SE_LIST_UPDATE = SEQ_SE_DP_POKETCH_010,		// 画面更新
};


#endif
