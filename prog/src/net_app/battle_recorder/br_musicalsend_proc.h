//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_musicalsend_proc.h
 *	@brief	ミュージカル送信プロセス
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_graphic.h"
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
///	ミュージカル送信プロセス引数
//=====================================
typedef struct 
{
  BR_FADE_WORK    *p_fade;      //[in]フェード
  BR_GRAPHIC_WORK	*p_graphic;	  //[in]グラフィック管理
	BR_RES_WORK			*p_res;				//[in]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in]プロセス管理
} BR_MUSICALSEND_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	ミュージカル送信プロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_MUSICALSEND_ProcData;
