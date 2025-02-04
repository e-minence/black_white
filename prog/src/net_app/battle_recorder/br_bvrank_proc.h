//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvrank_proc.h
 *	@brief	バトルビデオランクプロセス
 *	@author	Toru=Nagihashi
 *	@date		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "br_proc_sys.h"
#include "br_res.h"
#include "br_fade.h"
#include "br_net.h"
#include "br_data.h"
#include "br_inner.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	ランキングモード
//=====================================
typedef enum
{
  BR_BVRANK_MODE_NEW,       //最新３０件
  BR_BVRANK_MODE_FAVORITE,  //通信対戦ランキング
  BR_BVRANK_MODE_SUBWAY,    //サブウェイ
  BR_BVRANK_MODE_SEARCH,    //検索結果

} BR_BVRANK_MODE;



//=============================================================================
/**
 *					構造体
*/
//=============================================================================
//-------------------------------------
///	バトルビデオランクプロセス引数
//=====================================
typedef struct 
{
  BR_BVRANK_MODE  mode;         //[in ]モード
  BOOL            is_return;    //[in ]レコード画面からの戻り
  BR_FADE_WORK    *p_fade;      //[in ]フェード
	GFL_CLUNIT			*p_unit;			//[in ]ユニット
	BR_RES_WORK			*p_res;				//[in ]リソース管理
	BR_PROC_SYS			*p_procsys;		//[in ]プロセス管理
  BR_NET_WORK     *p_net;       //[in ]ネット管理
  BR_OUTLINE_DATA *p_outline;   //[in ]常駐データ
  BR_NET_VIDEO_SEARCH_DATA  search_data;  //[in]検索項目（modeがBR_BVRANK_MODE_SEARCHのとき入れる）
  BR_LIST_POS     *p_list_pos;  //[in ]常駐データ
} BR_BVRANK_PROC_PARAM;


//=============================================================================
/**
 *					外部公開関数
*/
//=============================================================================
//-------------------------------------
///	バトルビデオランクプロセスデータ
//=====================================
extern const GFL_PROC_DATA	BR_BVRANK_ProcData;
