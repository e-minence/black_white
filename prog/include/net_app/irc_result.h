//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_result.h
 *	@brief	赤外線ミニゲーム	結果表示
 *	@author	Toru=Nagihashi
 *	@date		2009.05.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "gamesystem/gamesystem.h"
#include "net_app/compatible_irc_sys.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	PROCに渡すパラメータ
//=====================================
typedef struct {
	GAMESYS_WORK				*p_gamesys;//[in]ゲームシステム
	COMPATIBLE_IRC_SYS	*p_irc;		//[in]赤外線システム
	u8									score;		//[in]ゲームの点数
}IRC_RESULT_PARAM;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	リズムチェック用プロックデータ
//=====================================
extern const GFL_PROC_DATA IrcResult_ProcData;

