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
	BR_BROWSE_MENUID_TOP,						//ブラウザモード　初期メニュー

	BR_BROWSE_MENUID_BTLVIDEO,			//ブラウザモード　バトルビデオメニュー,

	BR_BROWSE_MENUID_OTHER_RECORD,	//ブラウザモード　だれかの記録メニュー
	BR_BROWSE_MENUID_DELETE_RECORD,	//ブラウザモード　記録を消すメニュー
	BR_BROWSE_MENUID_DELETE_OTHER,	//ブラウザモード　誰かの記録を消すメニュー
	
	BR_BTLVIDEO_MENUID_TOP,					//グローバルバトルビデオモード　初期メニュー

	BR_BTLVIDEO_MENUID_LOOK,				//グローバルバトルビデオモード　見るメニュー
	BR_BTLVIDEO_MENUID_RANK,				//グローバルバトルビデオモード　ランキングで探すメニュー

	BR_MUSICAL_MENUID_TOP,					//グローバルミュージカルショットモード　初期メニュー

	BR_MENUID_YESNO,								//汎用	はい、いいえ
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
	
	GFL_CLUNIT			*p_unit;			//[in]ユニット
	BR_RES_WORK			*p_res;				//[in]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in]プロセス管理
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
