//======================================================================
/**
 * @file	debugwin_sys.h
 * @brief	汎用的デバッグシステム
 * @author	ariizumi
 * @data	09/05/01
 */
//======================================================================

#include "print/gf_font.h"


#ifndef DEBUGWIN_SYS_H__
#define DEBUGWIN_SYS_H__

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#if PM_DEBUG
#define USE_DEBUGWIN_SYSTEM (1)
#else
#define USE_DEBUGWIN_SYSTEM (0)
#endif

#define DEBUGWIN_GROUPID_TOPMENU (255)

#define DEBUGWIN_CONT_KEY (PAD_BUTTON_L)
#define DEBUGWIN_TRG_KEY (PAD_BUTTON_SELECT)

//各種退避領域サイズ(WBではエラー画面から拝借するのでサイズをあわせてる
#define DEBUGWIN_CHAR_TEMP_AREA (0x4000)
#define DEBUGWIN_SCRN_TEMP_AREA (0x800)
#define DEBUGWIN_PLTT_TEMP_AREA (0x20)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
/// 項目選択中のコールバック

typedef struct _DEBUGWIN_ITEM DEBUGWIN_ITEM;
typedef struct _DEBUGWIN_GROUP DEBUGWIN_GROUP;

//アイテムの選択中に呼ばれるコールバック関数です
typedef void (*DEBUGWIN_UPDATE_FUNC)( void* userWork , DEBUGWIN_ITEM* item );
//アイテムの描画更新時に呼ばれるコールバック関数です
typedef void (*DEBUGWIN_DRAW_FUNC)( void* userWork , DEBUGWIN_ITEM* itemWo );

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
//--------------------------------------------------------------
//	gfl_useで行う初期化・開放・実行処理
//--------------------------------------------------------------

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムの初期化
 *
 * @param   charArea			キャラ退避メモリ
 * @param   scrnArea			スクリーン退避メモリ
 * @param   plttArea			パレット退避メモリ
 */
//------------------------------------------------------------------
void DEBUGWIN_InitSystem( u8* charArea , u16* scrnArea , u16* plttArea );

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムの更新
 */
//------------------------------------------------------------------
void DEBUGWIN_UpdateSystem(void);

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムの開放
 */
//------------------------------------------------------------------
void DEBUGWIN_ExitSystem(void);

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムの開閉確認
 *
 * @return デバッグウィンドウが開いているか？
 */
//------------------------------------------------------------------
BOOL DEBUGWIN_IsActive(void);


//--------------------------------------------------------------
//	Proc毎に行う行う初期化・開放
//--------------------------------------------------------------

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムのProc毎の初期化
 *
 * @param frmnum      デバッグウィンドウを開くBG面
 * @param fonthandle  デバッグウィンドウ描画に使うフォント
 */
//------------------------------------------------------------------
void DEBUGWIN_InitProc( const u32 frmnum , GFL_FONT *fontHandle );

//------------------------------------------------------------------
/**
 * デバッグウィンドウシステムのProc毎の開放処理
 *   グループ・アイテムは自動的に解放されません
 */
//------------------------------------------------------------------
void DEBUGWIN_ExitProc( void );

//--------------------------------------------------------------
//	グループの処理
//--------------------------------------------------------------
//------------------------------------------------------------------
/**
 * メニュートップへグループの追加
 *
 * @param id      作成するグループの管理番号(0〜255)
 * @param nameStr グループの表示名
 * @param heapId  ヒープID
 */
//------------------------------------------------------------------
void DEBUGWIN_AddGroupToTop( const u8 id , const char *nameStr , const HEAPID heapId );

//------------------------------------------------------------------
/**
 * 指定グループへグループの追加
 *
 * @param id            作成するグループの管理番号(0〜255)
 * @param nameStr       グループの表示名
 * @param parentGroupId グループを追加するグループの管理番号(0〜255)
 * @param heapId        ヒープID
 */
//------------------------------------------------------------------
void DEBUGWIN_AddGroupToGroup( const u8 id , const char *nameStr , const u8 parentGroupId, const HEAPID heapId );

//------------------------------------------------------------------
/**
 * グループの削除
 *   このグループに追加されているグループ・アイテムも削除されます。
 *
 * @param id            削除するグループの管理番号(0〜255)
 */
//------------------------------------------------------------------
void DEBUGWIN_RemoveGroup( const u8 id );

//--------------------------------------------------------------
//	アイテムの処理
//--------------------------------------------------------------

//------------------------------------------------------------------
/**
 * アイテムをグループに追加する(名前固定
 *
 * @param nameStr        アイテムの表示名
 * @param updateFunc     アイテムの選択時に呼ばれるコールバック関数
 * @param work           コールバック関数用のワーク
 * @param parentGroupId  アイテムを追加するグループの管理番号(0〜255)
 * @param heapId         ヒープID
 */
//------------------------------------------------------------------
void DEBUGWIN_AddItemToGroup( const char *nameStr , 
                              DEBUGWIN_UPDATE_FUNC updateFunc , 
                              void *work , 
                              const u8 parentGroupId , 
                              const HEAPID heapId );

//------------------------------------------------------------------
/**
 * アイテムをグループに追加する(名前変更可
 *
 * @param updateFunc     アイテムの選択時に呼ばれるコールバック関数
 * @param drawFunc       アイテムの描画時に呼ばれるコールバック関数
 * @param work           コールバック関数用のワーク
 * @param parentGroupId  アイテムを追加するグループの管理番号(0〜255)
 * @param heapId         ヒープID
 */
//------------------------------------------------------------------
void DEBUGWIN_AddItemToGroupEx( DEBUGWIN_UPDATE_FUNC updateFunc , 
                                DEBUGWIN_DRAW_FUNC drawFunc , 
                                void *work , 
                                const u8 parentGroupId , 
                                const HEAPID heapId );

//------------------------------------------------------------------
/**
 * アイテムの名前を変更する
 *   DEBUGWIN_DRAW_FUNC内で呼んでください
 *
 * @param item      アイテムのワーク(DEBUGWIN_DRAW_FUNCの第2引数)
 * @param nameStr   ヒープID
 */
//------------------------------------------------------------------
void DEBUGWIN_ITEM_SetName( DEBUGWIN_ITEM* item , const char *nameStr );
void DEBUGWIN_ITEM_SetNameV( DEBUGWIN_ITEM* item , char *nameStr , ...);
void DEBUGWIN_ITEM_SetNameU16( DEBUGWIN_ITEM* item , const STRCODE *strcode );

//--------------------------------------------------------------
//	色の変更
//--------------------------------------------------------------
//------------------------------------------------------------------
/**
 * デバッグウィンドウの非選択時の色
 *
 * @param r R値(0〜31)
 * @param g G値(0〜31)
 * @param b B値(0〜31)
 */
//------------------------------------------------------------------
void DEBUGWIN_ChangeLetterColor( const u8 r , const u8 g , const u8 b );

//------------------------------------------------------------------
/**
 * デバッグウィンドウの選択時の色
 *
 * @param r R値(0〜31)
 * @param g G値(0〜31)
 * @param b B値(0〜31)
 */
//------------------------------------------------------------------
void DEBUGWIN_ChangeSelectColor( const u8 r , const u8 g , const u8 b );

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------

//------------------------------------------------------------------
/**
 * デバッグウィンドウの再描画
 */
//------------------------------------------------------------------
void DEBUGWIN_RefreshScreen( void );

//------------------------------------------------------------------
/**
 * フレームを1フレーム更新させる
 */
//------------------------------------------------------------------
void DEBUGWIN_UpdateFrame( void );

#endif //DEBUGWIN_SYS_H__
