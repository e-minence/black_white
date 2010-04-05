//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_menu_proc.h
 *	@brief	メニュープロセス
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
#include "br_inner.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	メニューインデックス
//		ここでのメニューとは上画面が切り替らず、
//		ボタンを押していくことで進んでいく箇所です。
//		（消すは別画面のように思えますが上記の意味で範疇です）
//=====================================
typedef enum
{
  //以下ブラウザ
	BR_BROWSE_MENUID_TOP,						//ブラウザモード　初期メニュー
	BR_BROWSE_MENUID_BTLVIDEO,			//ブラウザモード　バトルビデオメニュー,
	BR_BROWSE_MENUID_OTHER_RECORD,	//ブラウザモード　だれかの記録メニュー
	BR_BROWSE_MENUID_DELETE_RECORD,	//ブラウザモード　記録を消すメニュー
	BR_BROWSE_MENUID_DELETE_OTHER,	//ブラウザモード　誰かの記録を消すメニュー
	
  //以下バトルビデオ
	BR_BTLVIDEO_MENUID_TOP,					//グローバルバトルビデオモード　初期メニュー
	BR_BTLVIDEO_MENUID_LOOK,				//グローバルバトルビデオモード　見るメニュー
	BR_BTLVIDEO_MENUID_RANK,				//グローバルバトルビデオモード　ランキングで探すメニュー
	BR_MENUID_BVSEND_YESNO,					//バトルビデオを送る	はい、いいえ
	BR_BTLVIDEO_MENUID_EXIT_YESNO,	//バトルビデオをやめる	はい、いいえ

  //以下ミュージカル
	BR_MUSICAL_MENUID_TOP,					//グローバルミュージカルショットモード　初期メニュー
	BR_MENUID_MUSICALEXIT_YESNO,		//ミュージカルショットをやめる	はい、いいえ

	BR_MENUID_MAX
} BR_MENUID;

//=============================================================================
/**
 *					構造体
*/
//=============================================================================


//-------------------------------------
///	メニュープロセス引数
//=====================================
typedef struct 
{
	BR_MENUID				menuID;				//[in/out]プロセス開始時、終了時のメニュー
	u32							next_proc;		//[out]次へ移動するプロセス(実際の移動はmenu_procのpush)
	u32							next_mode;		//[out]次へ移動するプロセスに渡す情報
  BR_FADE_TYPE    fade_type;    //[in ]開始フェードタイプ
	
  BR_FADE_WORK    *p_fade;      //[in ]フェード
	GFL_CLUNIT			*p_unit;			//[in ]ユニット
	BR_RES_WORK			*p_res;				//[in ]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in ]プロセス管理
  const BR_SAVE_INFO  *cp_saveinfo; //[in]外部データ状況
  BR_RESULT       *p_result;    //<[out] 終了モードへのポインタ
} BR_MENU_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	メニュープロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_MENU_ProcData;
