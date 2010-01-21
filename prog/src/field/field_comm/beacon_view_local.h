/**
 *  @file   beacon_view_local.h
 *  @brief  すれ違い通信画面ローカルヘッダ
 *  @author Miyuki Iwasawa
 *  @date   10.01.19
 */

#pragma once

/////////////////////////////////////
//リテラル
#define HEAPID_BVIEW_TMP      GFL_HEAP_LOWID( HEAPID_FIELDMAP )
#define HEAPID_BEACON_VIEW    (HEAPID_FIELDMAP)

///フォントパレット展開位置
#define FONT_PAL    (15)
#define ACT_PAL_FONT  (3)
#define ACT_PAL_UNION (9)

///表示するログ件数
#define VIEW_LOG_MAX    (4)

#define BS_LOG_MAX  (30)  //ログ管理数
#define PANEL_MAX   (5)   //同時描画されるパネル数
#define PANEL_LINE_MAX    (4)   //画面内に描画されるパネル数

//OBJリソース参照コード
enum{
 OBJ_RES_PLTT,
 OBJ_RES_CGR,
 OBJ_RES_CELLANIM,
 OBJ_RES_MAX,
};

#define UNION_CHAR_MAX      (16)  ///<ユニオンキャラクターmax
#define BEACON_VIEW_OBJ_MAX (5*8) ///<画面内に表示するOBJの登録max数

///OBJ BGプライオリティ
#define OBJ_BG_PRI (3)

///OBJソフトウェアプライオリティ
enum{
 OBJ_SPRI_MSG = 0,
 OBJ_SPRI_ICON = OBJ_SPRI_MSG + PANEL_MAX,
 OBJ_SPRI_UNION = OBJ_SPRI_ICON + PANEL_MAX,
 OBJ_SPRI_PANEL = OBJ_SPRI_UNION + PANEL_MAX,
};

///ノーマルOBJ アニメID
enum{
 ACTANM_PANEL,
};

#define ACT_PANEL_OX  (13)
#define ACT_PANEL_OY  (8*5)
#define ACT_UNION_OX  (4*8)
#define ACT_UNION_OY  (3*8)
#define ACT_ICON_OX   (ACT_UNION_OX+28)
#define ACT_ICON_OY   (ACT_UNION_OY)


///アイコンパターン数
enum{
  ICON_HELLO,
  ICON_BTL_START,
  ICON_BTL_WIN,
  ICON_POKE_GET,
  ICON_LV_UP,
  ICON_SHINKA,
  ICON_GPOWER,
  ICON_ITEM_GET,
  ICON_SPATIAL,
  ICON_UNION,
  ICON_CM,
  ICON_THANKS,
  ICON_MAX,
};

//==============================================================================
//  構造体定義
//==============================================================================
//パネル管理構造体
typedef struct _PANEL_WORK{
  u8  id;
  u8  data_idx;
  s16 px;
  s16 py;
  GFL_CLWK* cPanel;
  GFL_CLWK* cUnion;
  GFL_CLWK* cIcon;
  GFL_CLWK* cMsg;

  struct _PANEL_WORK* next;
  struct _PANEL_WORK* prev;
}PANEL_WORK;

//オブジェリソース管理
typedef struct _OBJ_RES{
  u32   num;
  u32*  tbl;
}OBJ_RES;

typedef struct _OBJ_RES_SRC{
  u16 num;
  u16 srcID;
}OBJ_RES_SRC;

//オブジェリソース管理構造体
typedef struct _OBJ_RES_TBL{
  OBJ_RES res[OBJ_RES_MAX];
}OBJ_RES_TBL;

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

///すれ違い通信状態参照画面管理ワーク
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  FIELD_SUBSCREEN_WORK *subscreen;

  HEAPID      heapID;
  HEAPID      tmpHeapID;
  ARCHANDLE*  arc_handle;

  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  WORDSET *wordset;
  GFL_MSGDATA *mm_status;
  STRBUF *strbuf_temp;
  STRBUF *strbuf_expand;
 
  GFL_CLUNIT* cellUnit;
  OBJ_RES_TBL objResNormal;
  OBJ_RES_TBL objResUnion;
  OBJ_RES_TBL objResIcon;

  GFL_BMPWIN *win[VIEW_LOG_MAX];
  PRINT_UTIL print_util[VIEW_LOG_MAX];
 
  PANEL_WORK  panel[PANEL_MAX];
  u32 log_count;


  //リソース
  RES2D_CHAR  resCharIcon[ICON_MAX];
  RES2D_CHAR  resCharUnion[UNION_CHAR_MAX];
  RES2D_PLTT  resPlttUnion;
}BEACON_VIEW;


