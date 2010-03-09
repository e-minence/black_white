//============================================================================================
/**
 * @file		box2_snd_def.h
 * @brief		ボックス画面サウンドナンバー定義
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 */
//============================================================================================
#pragma	once

//--------------------------------------------------------------------------------------------
//		
//--------------------------------------------------------------------------------------------
#define  SE_BOX2_LOG_IN				( SEQ_SE_PC_LOGIN )		// ボックス画面開始
#define  SE_BOX2_LOG_OFF			( SEQ_SE_PC_LOGOFF )	// ボックス画面終了

//--------------------------------------------------------------------------------------------
//	ポケモンいどう
//--------------------------------------------------------------------------------------------
#define  SE_BOX2_POKE_CATCH		( SEQ_SE_SYS_39 )		// ポケモンつかむ
#define  SE_BOX2_POKE_PUT			( SEQ_SE_SYS_40 )		// ポケモンおく

//--------------------------------------------------------------------------------------------
//	てもちポケモン
//--------------------------------------------------------------------------------------------
#define  SE_BOX2_OPEN_PARTY_TRAY		( SEQ_SE_SYS_42 )		// 手持ちウィンドウを開く
//#define  SE_BOX2_CLOSE_PARTY_TRAY		( SEQ_SE_SELECT1 )		// 手持ちウィンドウを閉じる

//--------------------------------------------------------------------------------------------
//	操作
//--------------------------------------------------------------------------------------------
#define  SE_BOX2_MOVE_CURSOR			( SEQ_SE_SELECT1 )		// カーソル移動
#define  SE_BOX2_DECIDE						( SEQ_SE_DECIDE1 )		// 決定音
#define  SE_BOX2_CANCEL						( SEQ_SE_CANCEL1 )		// キャンセル音
#define  SE_BOX2_CLOSE						( SEQ_SE_CLOSE1 )			// Ｘボタン
#define  SE_BOX2_CHANGE_TRAY			( SEQ_SE_SELECT1 )		// トレイ切り替え

#define  SE_BOX2_WARNING					( SEQ_SE_BEEP )		// 警告音(最後の1匹を逃がそうとした時)

//#define  SE_BOX2_DISABLE_BUTTON		( SEQ_SE_SELECT1 )		// 押せないボタンを押したとき

//#define  SE_BOX2_ITEM_WITHDRAW		( SEQ_SE_SELECT1 )		// アイテムあずかる
//#define  SE_BOX2_ITEM_SET					( SEQ_SE_SELECT1 )		// アイテムもたせる
//#define  SE_BOX2_ITEM_SWAP				( SEQ_SE_SELECT1 )		// アイテムとりかえる
