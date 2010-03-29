//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvdelete_proc.h
 *	@brief	バトルビデオ消去プロセス
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	起動モード
//=====================================
typedef enum
{
  BR_BVDELETE_MODE_MY,
  BR_BVDELETE_MODE_OTHER1,
  BR_BVDELETE_MODE_OTHER2,
  BR_BVDELETE_MODE_OTHER3,
} BR_BVDELETE_MODE;

//=============================================================================
/**
 *					構造体
*/
//=============================================================================
//-------------------------------------
///	バトルビデオ送信プロセス引数
//=====================================
typedef struct 
{
	BR_BVDELETE_MODE  mode;         //[in]起動モード
  BR_FADE_WORK      *p_fade;      //[in]フェード
	GFL_CLUNIT			  *p_unit;			//[in]ユニット
	BR_RES_WORK			  *p_res;				//[in]リソース管理
	BR_PROC_SYS			  *p_procsys;		//[in]プロセス管理
  GAMEDATA          *p_gamedata;  //[in]ゲームデータ

} BR_BVDELETE_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	バトルビデオ送信プロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_BVDELETE_ProcData;
