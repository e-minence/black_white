//=============================================================================
/**
 *
 *	@file		beacon_detail_local.h
 *	@brief  すれ違い詳細画面ローカルヘッダ
 *	@author	Miyuki Iwasawa
 *	@data		2010.02.01
 *
 */
//=============================================================================

#pragma once

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_accessor.h"
#include "gamesystem/game_beacon_types.h"
#include "gamesystem/beacon_status.h"
#include "gamesystem/msgspeed.h"
#include "tr_tool/trtype_def.h"
#include "system/tr2dgra.h"
#include "savedata/intrude_save.h"
#include "net_app/union/union_beacon_tool.h"
#include "app/townmap_data_sys.h"
#include "app/townmap_util.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

#include "beacon_detail_def.h"

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//キャラクターリソース
typedef struct _RES2D_CHAR{
  NNSG2dCharacterData* p_char;
  void* buf;
}RES2D_CHAR;

//パレットリソース
typedef struct _RES2D_PLTT{
  NNSG2dPaletteData* p_pltt;
  u16* dat;
  void* buf;
}RES2D_PLTT;

//オブジェリソース定義
typedef struct _OBJ_RES_SRC{
  u8  type, pltt_ofs, pltt_siz;
  u16 pltt_id;
  u16 cgr_id;
  u16 cell_id;
}OBJ_RES_SRC;

//オブジェリソース管理構造体
typedef struct _OBJ_RES_TBL{
  u32 pltt;
  u32 cgr;
  u32 cell;
}OBJ_RES_TBL;


// Bmpウィンドウ
typedef struct{
  GFL_BMPWIN *win;
  GFL_BMP_DATA* bmp;
  PRINT_UTIL putil;
}BMP_WIN;

typedef struct _BEACON_WIN{
  BMP_WIN prof[BEACON_PROF_MAX];
  BMP_WIN home[BEACON_HOME_MAX];
  BMP_WIN record;   ///
  GFL_BMPWIN* pms;  ///簡易会話
  GFL_CLWK* cRank;
  GFL_CLWK* cTrainer;
  u8  union_char;
  u8  rank;
  u8  frame;
}BEACON_WIN;

//--------------------------------------------------------------
///	メインワーク
//==============================================================
typedef struct 
{
  HEAPID heapID;
  HEAPID tmpHeapID;
  int                   seq;
	int										sub_seq;
  int                   eff_task_ct;
  BOOL                  data_error_f;

  BEACON_DETAIL_PARAM*  param;
  GFL_TCBLSYS*  pTcbSys;
  TOWNMAP_DATA* tmap; ///タウンマップデータアーカイブ

  BEACON_STATUS* b_status;
  GAMEBEACON_INFO_TBL*  infoLog;    //ログテーブル
  GAMEBEACON_INFO*      tmpInfo;
  u16                   tmpTime;
//  MY_DATA               my_data;
  u8                    flip_sw;
  u8                    list_max;
  u8                    list_top;
  u8                    list[GAMEBEACON_INFO_TBL_MAX];

  u8                    msg_spd;

	//描画設定
	BEACON_DETAIL_GRAPHIC_WORK	*graphic;
  ARCHANDLE* handle;
  ARCHANDLE* h_trgra;
  OBJ_RES_TBL objResNormal;
  OBJ_RES_TBL objResTrainer[BEACON_WIN_MAX];
  OBJ_RES_TBL objResUnion;
  
  RES2D_CHAR  resCharUnion[UNION_CHAR_MAX];
  RES2D_PLTT  resPlttUnion;
  RES2D_PLTT  resPlttPanel;

	//タッチバー
	TOUCHBAR_WORK							*touchbar;

	//フォント
	GFL_FONT									*font;

	//プリントキュー
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;
	GFL_MSGDATA								*msg_area;
  WORDSET                   *wset;
  STRBUF *str_tmp;
  STRBUF *str_expand;
  STRBUF *str_popup;  //ポップアップ用テンポラリ

#ifdef	BEACON_DETAIL_PRINT_TOOL
	//プリントユーティリティ
	PRINT_UTIL								print_util;
#endif	//BEACON_DETAIL_PRINT_TOOL

  BMP_WIN         win_popup;
  PMS_DRAW_WORK*  pms_draw;
  BEACON_WIN      beacon_win[BEACON_WIN_MAX];
 
  s16   icon_x,icon_y;
  u8    icon_enable_f;
  GFL_CLWK* pAct[ACT_MAX];

} BEACON_DETAIL_WORK;



