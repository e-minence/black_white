//============================================================================
/**
 *
 *	@file		demo3d_engine.h
 *	@brief  3Dデモ再生エンジン
 *	@author	hosaka genya
 *	@data		2009.12.08
 *
 */
//============================================================================
#pragma once

typedef struct _DEMO3D_ENGINE_WORK DEMO3D_ENGINE_WORK;

#ifdef PM_DEBUG

#define DEBUG_USE_KEY // デバッグ用のキー操作を使うフラグ。基本的に↓の機能を使う場合は必須

//#define DEBUG_CAMERA_CONTROL // 2画面時のカメラ操作デバッグ
#define DEBUG_CAMERA_DISP_OFFSET_CONTROL // 2画面のオフセットを操作

#endif //PM_DEBUG

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3Dグラフィック 初期化
 *
 *	@param	DEMO3D_GRAPHIC_WORK* graphic  グラフィックワーク
 *	@param	demo_id デモID
 *	@param	start_frame 初期フレーム値(1SYNC=1)
 *	@param	heapID ヒープID
 *
 *	@retval DEMO3D_ENGINE_WORK* ワーク
 */
//-----------------------------------------------------------------------------
extern DEMO3D_ENGINE_WORK* Demo3D_ENGINE_Init( DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ID demo_id, u32 start_frame, HEAPID heapID );
//-----------------------------------------------------------------------------
/**
 *	@brief  3Dグラフィック 開放
 *
 *	@param	DEMO3D_ENGINE_WORK* wk  ワーク
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
extern void Demo3D_ENGINE_Exit( DEMO3D_ENGINE_WORK* wk );
//-----------------------------------------------------------------------------
/**
 *	@brief  3Dグラフィック 主処理
 *
 *	@param	DEMO3D_ENGINE_WORK* wk ワーク
 *
 *	@retval TRUE : 終了
 */
//-----------------------------------------------------------------------------
extern BOOL Demo3D_ENGINE_Main( DEMO3D_ENGINE_WORK* wk );
//-----------------------------------------------------------------------------
/**
 *	@brief  現在のフレーム値を取得
 *
 *	@param	DEMO3D_ENGINE_WORK* wk 　ワーク
 *
 *	@retval フーレム値
 */
//-----------------------------------------------------------------------------
extern fx32 DEMO3D_ENGINE_GetNowFrame( const DEMO3D_ENGINE_WORK* wk );
