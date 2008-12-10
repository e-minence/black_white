//==============================================================================
/**
 * @file	trade.h
 * @brief	交換画面ヘッダ
 * @author	matsuda
 * @date	2008.12.10(水)
 */
//==============================================================================
#ifndef __TRADE_H__
#define __TRADE_H__

#include "savedata/save_control.h"


//==============================================================================
//	定数定義
//==============================================================================
///交換画面起動時に最初に実行するシーケンス
enum{
	TRADE_STARTUP_SEQ_INIT,			///<通常の開始処理
	TRADE_STARTUP_SEQ_SELECT,		///<リスト、終了の選択から
	TRADE_STARTUP_SEQ_CHANGE,		///<交換画面実行するところから
};

///交換画面から戻ってくるときの戻り値
enum{
	TRADE_MENU_NULL,		///<初回呼び出し
	TRADE_MENU_CANCEL,		///<キャンセル
	TRADE_MENU_LIST,		///<ポケモンリスト
};


//==============================================================================
//	構造体定義
//==============================================================================
///交換画面呼び出し時に渡してもらうParentWork
typedef struct{
	int first_seq;		///<交換画面起動時に実行するシーケンス
	POKEPARTY *party;	///<手持ちポケモンへのポインタ
	int sel_mons;		///<交換に出すポケモン(何番目か)
	SAVE_CONTROL_WORK *sv;	///<セーブデータへのポインタ
	
	//交換画面から呼び出し側へ返す返事
	int select_menu;
}TRADE_DISP_PARENT;


//==============================================================================
//	外部データ
//==============================================================================
extern const GFL_PROC_DATA TradeProcData;


#endif	//__TRADE_H__
