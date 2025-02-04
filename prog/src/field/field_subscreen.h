//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   field_subscreen.h
 *  @brief    InforBerの初期化・破棄　
 *  @date   2009.03.26  fieldmapから移植
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"  //FIELDMAP_WORK参照
#include "field/field_menu_item.h"     //FIELD_MENU_ITEM_TYPE参照

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

typedef void (STARTUP_ENDCALLBACK) (void* pWork);


//BG面とパレット番号
#define FIELD_SUBSCREEN_BGPLANE (GFL_BG_FRAME0_S)
#define FIELD_SUBSCREEN_PALLET  (0xE)

///下画面を非アクティブにした場合の基準となる輝度
#define FIELD_NONE_ACTIVE_EVY   (8)

///下画面のモード指定（デバッグでしかつかわないはず）
typedef enum {  
  FIELD_SUBSCREEN_NORMAL = 0,         // Cギア
  FIELD_SUBSCREEN_TOPMENU,            // フィールドメニュー
  FIELD_SUBSCREEN_UNION,              // ユニオン下画面
  FIELD_SUBSCREEN_INTRUDE,            // 侵入
  FIELD_SUBSCREEN_BEACON_VIEW,        // すれ違い結果画面
  FIELD_SUBSCREEN_NOGEAR,             // Cギアの未取得時
  FIELD_SUBSCREEN_DOWSING,            // ダウジング
  FIELD_SUBSCREEN_REPORT,             // レポート画面
  FIELD_SUBSCREEN_CGEARFIRST,         // CGEAR起動画面
  FIELD_SUBSCREEN_CGEAR_ONOFF,         // CGEAR電源ON　OFF画面
#if PM_DEBUG
  FIELD_SUBSCREEN_DEBUG_LIGHT,        // デバッグライト制御パネル
  FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,  // デバッグカメラ
  FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,  // デバッグサウンド
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
  FIELD_SUBSCREEN_ACTION_GSYNC,  ///< GAMESYNC
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_BEACON_VIEW, ///< サブスクリーンをビーコン参照画面に変更
  FIELD_SUBSCREEN_ACTION_CHANGE_SCREEN_CGEAR, ///< サブスクリーンをCGEAR画面に変更
  FIELD_SUBSCREEN_ACTION_BEACON_DETAIL,  ///<ビーコン詳細画面呼び出し
  FIELD_SUBSCREEN_ACTION_DOWSING,         ///< ダウジング
  FIELD_SUBSCREEN_ACTION_SCANRADAR,     ///< 調査レーダー
  FIELD_SUBSCREEN_ACTION_CGEAR_HELP,    ///< CGEARHELP
  FIELD_SUBSCREEN_ACTION_CGEAR_POWER,    ///< CGEAR　ONOFF
  FIELD_SUBSCREEN_ACTION_CGEAR_POWER_EXIT,///< CGEAR　ONOFF処理完了
  
  FIELD_SUBSCREEN_ACTION_MAX,

}FIELD_SUBSCREEN_ACTION;

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

typedef struct _FIELD_SUBSCREEN_WORK FIELD_SUBSCREEN_WORK;

//-----------------------------------------------------------------------------
/**
 *          プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID,
    FIELDMAP_WORK * fieldmap, FIELD_SUBSCREEN_MODE mode );
extern u8 FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Draw( FIELD_SUBSCREEN_WORK* pWork );
extern GMEVENT* FIELD_SUBSCREEN_EventCheck( FIELD_SUBSCREEN_WORK* pWork, BOOL bEvReq );
extern const BOOL FIELD_SUBSCREEN_CanChange( FIELD_SUBSCREEN_WORK* pWork );
extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_CGearCheck(FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeForce( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern void FIELD_SUBSCREEN_ChangeFromWithin( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode, BOOL bFade);
extern void FIELD_SUBSCREEN_CgearFirst( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode,STARTUP_ENDCALLBACK* pCall,void* pWork2);

extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork);

extern FIELD_SUBSCREEN_ACTION FIELD_SUBSCREEN_GetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_ResetAction( FIELD_SUBSCREEN_WORK* pWork);
extern void FIELD_SUBSCREEN_MainDispBrightnessOff( FIELD_SUBSCREEN_WORK* pWork, HEAPID heapId );
extern void FIELD_SUBSCREEN_SetAction( FIELD_SUBSCREEN_WORK* pWork , FIELD_SUBSCREEN_ACTION actionno);
extern void FIELD_SUBSCREEN_GrantPermission( FIELD_SUBSCREEN_WORK* pWork);

//フィールドメニューと決定項目番号のやり取り
extern const FIELD_MENU_ITEM_TYPE FIELD_SUBSCREEN_GetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_SetTopMenuItemNo( FIELD_SUBSCREEN_WORK* pWork , const FIELD_MENU_ITEM_TYPE itemType );

extern BOOL FIELD_SUBSCREEN_CheckReportInit( FIELD_SUBSCREEN_WORK * pWork );
extern void FIELD_SUBSCREEN_SetReportSize( FIELD_SUBSCREEN_WORK * pWork );
extern void FIELD_SUBSCREEN_SetReportStart( FIELD_SUBSCREEN_WORK * pWork );
extern BOOL FIELD_SUBSCREEN_SetReportEnd( FIELD_SUBSCREEN_WORK * pWork );
extern void FIELD_SUBSCREEN_SetReportBreak( FIELD_SUBSCREEN_WORK * pWork );
extern BOOL FIELD_SUBSCREEN_CheckReportType( FIELD_SUBSCREEN_WORK * pWork );

extern BOOL FIELD_SUBSCREEN_EnablePalaceUse( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_SetMainLCDNavigationScreen( FIELD_SUBSCREEN_WORK* pWork, HEAPID heapID );

// C_GEAR_WORK* Cgearワークの取得
extern void* FIELD_SUBSCREEN_GetCGearWork( const FIELD_SUBSCREEN_WORK* cpWork );


#ifdef  PM_DEBUG
extern void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork);
#endif  //PM_DEBUG

#ifdef _cplusplus
} // extern "C"{
#endif



