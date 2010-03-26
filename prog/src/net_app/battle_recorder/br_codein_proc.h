//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_codein_proc.h
 *	@brief	数値入力プロセス
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

//=============================================================================
/**
 *					構造体
*/
//=============================================================================
//-------------------------------------
///	数値入力プロセス引数
//=====================================
typedef struct 
{
	
  BR_FADE_WORK    *p_fade;      //[in ]フェード
	GFL_CLUNIT			*p_unit;			//[in ]ユニット
	BR_RES_WORK			*p_res;				//[in ]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in ]プロセス管理
  u64             video_number; //[out]入力されたナンバー
} BR_CODEIN_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	数値入力プロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_CODEIN_ProcData;
