//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_musicallook_proc.h
 *	@brief	ミュージカルを見るプロセス
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
#include "br_net.h"
#include "br_sidebar.h"
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
///	ミュージカルを見るプロセス引数
//=====================================
typedef struct 
{
  BR_FADE_WORK    *p_fade;      //[in ]フェード
  BR_SIDEBAR_WORK *p_sidebar;   //[in ]サイドバー
	BR_GRAPHIC_WORK	*p_graphic;	  //[in ]グラフィック管理
	BR_RES_WORK			*p_res;				//[in ]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in ]プロセス管理
  BR_NET_WORK     *p_net;       //[in ]通信管理
} BR_MUSICALLOOK_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	ミュージカルを見るプロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_MUSICALLOOK_ProcData;
