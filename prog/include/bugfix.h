//=============================================================================
/**
 * @file	bugfix.h
 * @date	2010.07.12
 * @brief	バグ対処用定義
 * 定義の有効・無効で決まります  反映させない不具合はコメントにしてください
 */
//=============================================================================

#pragma once


//--バグ掲示板 BTSXXXX
#define BUGFIX_BTS7711_100712   //WiFi大会の選手証でトレーナー名が切れて表示されるバグの対処

/*
	[ BTS7714 ] ボックス
	メニュー「もちもの」でバッグに戻すときの「はい・いいえ」時に
	トレイ切り替えの矢印が表示されたままになっている
*/
#define	BUGFIX_BTS7714_20100712

// 【フォルムNo.が「0」以外のガーメイルが、通信交換に出せない】不具合修正 
#define BUGFIX_BTS7718_100712

// 【ユンゲラーの通信進化デモ中に額の星マークが崩れている】不具合修正
#define BUGFIX_BTS7721_100712

//--社内バグ掲示板 GFBTSXXXX

// WIFI交換の時にタイマーアイコンの更新処理と削除処理が重なりフリーズした不具合修正
#define BUGFIX_GFBTS1957_100712

#define BUGFIX_GFBTS1958_100712 //WiFi大会でマッチング中に、はまるバグの対処


//--ガイドライン掲示板 GLBTSXXXX

