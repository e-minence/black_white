//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_fade.h
 *	@brief  バトルレコーダー用フェードシステム
 *	@author	Toru=Nagihashi
 *	@date		2009.12.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	フェードの種類
//=====================================
typedef enum
{
  BR_FADE_TYPE_MASTERBRIGHT_BLACK,      //輝度  黒
  BR_FADE_TYPE_MASTERBRIGHT_WHITE,      //輝度  白
  BR_FADE_TYPE_ALPHA_BG012OBJ,          //アルファ　BG0,1,2,OBJ
  BR_FADE_TYPE_PALLETE,                 //パレットフェード
  BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA,  //上画面がマスターブライト、下画面がアルファ（BR_FADE_DISPLAYはBOTH）

} BR_FADE_TYPE;

//-------------------------------------
///	フェードをする箇所
//=====================================
typedef enum
{ 
  BR_FADE_DISPLAY_MAIN  = 1<<0,
  BR_FADE_DISPLAY_SUB   = 1<<1,
  BR_FADE_DISPLAY_BOTH  = BR_FADE_DISPLAY_MAIN | BR_FADE_DISPLAY_SUB,
  BR_FADE_DISPLAY_TOUCH_HERE  = 0xF,  //パレットフェードのみ有効

}BR_FADE_DISPLAY;

//-------------------------------------
///	フェードの方向
//=====================================
typedef enum
{ 
  BR_FADE_DIR_IN,   //16->0
  BR_FADE_DIR_OUT,  //0->16
}BR_FADE_DIR;

typedef BOOL (*BR_FADE_CHENGEFADE_CALLBACK)( void *p_wk_adrs );

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	フェードシステム
//=====================================
typedef struct _BR_FADE_WORK BR_FADE_WORK;

//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
//-------------------------------------
///	システム系
//=====================================
extern BR_FADE_WORK * BR_FADE_Init( HEAPID heapID );
extern void BR_FADE_Exit( BR_FADE_WORK *p_wk );
extern void BR_FADE_Main( BR_FADE_WORK *p_wk );

//-------------------------------------
///	フェード操作
//=====================================
extern void BR_FADE_StartFade( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR  dir );
extern void BR_FADE_StartFadeEx( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR dir, u32 sync );
extern BOOL BR_FADE_IsEnd( const BR_FADE_WORK *cp_wk );

typedef struct
{ 
  BR_FADE_TYPE fadein_type;
  BR_FADE_TYPE fadeout_type;
  BR_FADE_DISPLAY disp;
  BR_FADE_CHENGEFADE_CALLBACK callback;
  void *p_wk_adrs;
}BR_FADE_CHANGEFADE_PARAM;
extern void BR_FADE_StartChangeFade( BR_FADE_WORK *p_wk, const BR_FADE_CHANGEFADE_PARAM *cp_param );

//-------------------------------------
///	その他特殊
//=====================================
extern void BR_FADE_PALETTE_Copy( BR_FADE_WORK *p_wk );
extern void BR_FADE_PALETTE_SetColor( BR_FADE_WORK *p_wk, GXRgb rgb );
extern void BR_FADE_PALETTE_TransColor( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display );
extern void BR_FADE_ALPHA_SetAlpha( BR_FADE_WORK *p_wk, BR_FADE_DISPLAY display, u8 ev );


typedef enum  //FADEREQのSUB_OBJまでと同じならび
{ 
  BR_FADE_PALETTE_LOADTYPE_BG_M,
  BR_FADE_PALETTE_LOADTYPE_BG_S,
  BR_FADE_PALETTE_LOADTYPE_OBJ_M,
  BR_FADE_PALETTE_LOADTYPE_OBJ_S,
}BR_FADE_PALETTE_LOADTYPE;
extern void BR_FADE_PALETTE_LoadPalette( BR_FADE_WORK *p_wk, ARCHANDLE *p_handle, ARCDATID datID, BR_FADE_PALETTE_LOADTYPE type, u32 offset, u32 trans_size, HEAPID heapID );


