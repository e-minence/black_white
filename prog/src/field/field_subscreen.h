//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_subscreen.h
 *	@brief		InforBerの初期化・破棄　
 *	@date		2009.03.26	fieldmapから移植
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "field/fieldmap_proc.h"  //FIELDMAP_WORK参照
#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE参照

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//BG面とパレット番号(仮設定
#define FIELD_SUBSCREEN_BGPLANE	(GFL_BG_FRAME0_S)
#define FIELD_SUBSCREEN_PALLET	(0xE)

///下画面のモード指定（デバッグでしかつかわないはず）
typedef enum {	
	FIELD_SUBSCREEN_NORMAL = 0,
  FIELD_SUBSCREEN_TOPMENU,
  FIELD_SUBSCREEN_UNION,
	FIELD_SUBSCREEN_INTRUDE,
	FIELD_SUBSCREEN_BEACON_VIEW,
  FIELD_SUBSCREEN_NOGEAR,
#if PM_DEBUG
	FIELD_SUBSCREEN_DEBUG_LIGHT,
	FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,
	FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,
#endif
	FIELD_SUBSCREEN_MODE_MAX,

    //サブメニューの初期化に渡す前回モードで起動時に呼ばれた場合
  FIELD_SUBSCREEN_FIRST_CALL,
}FIELD_SUBSCREEN_MODE;


///下画面の受け渡し値
typedef enum {
  FIELD_SUBSCREEN_ACTION_NONE,
  FIELD_SUBSCREEN_ACTION_IRC,  ///< IRCアプリ起動
  FIELD_SUBSCREEN_ACTION_TOPMENU,   ///< TOPMENU起動
  FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE,  ///< FieldMenu項目決定
  FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT,    ///< FieldMenu終了
  FIELD_SUBSCREEN_ACTION_WIRELESS,  ///<ワイヤレス
  FIELD_SUBSCREEN_ACTION_UNION_CHAT,        ///<ユニオン：チャット画面ショートカット
  FIELD_SUBSCREEN_ACTION_INTRUDE_TOWN_WARP, ///<侵入：街ワープ
  FIELD_SUBSCREEN_ACTION_INTRUDE_MISSION_PUT, ///<侵入：ミッションの表示
  FIELD_SUBSCREEN_ACTION_GSYNC,  ///< GAMESYNC
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW, ///< サブスクリーンをビーコン参照画面に変更
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR, ///< サブスクリーンをCGEAR画面に変更
  
  FIELD_SUBSCREEN_ACTION_MAX,

}FIELD_SUBSCREEN_ACTION;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

typedef struct _FIELD_SUBSCREEN_WORK FIELD_SUBSCREEN_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID,
    FIELDMAP_WORK * fieldmap, FIELD_SUBSCREEN_MODE mode );
extern u8 FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Draw( FIELD_SUBSCREEN_WORK* pWork );
extern const BOOL FIELD_SUBSCREEN_CanChange( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeForce( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeFromWithin( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode, BOOL bFade);

extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork);

extern FIELD_SUBSCREEN_ACTION FIELD_SUBSCREEN_GetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_ResetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_SetAction( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno);
extern void FIELD_SUBSCREEN_GrantPermission( FIELD_SUBSCREEN_WORK* pWork);

//フィールドメニューと決定項目番号のやり取り
extern const FIELD_MENU_ITEM_TYPE FIELD_SUBSCREEN_GetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_SetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork , const FIELD_MENU_ITEM_TYPE itemType );


#ifdef	PM_DEBUG
extern void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork);
#endif	//PM_DEBUG

#ifdef _cplusplus
}	// extern "C"{
#endif



