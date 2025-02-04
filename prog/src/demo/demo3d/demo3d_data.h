//============================================================================
/**
 *
 *	@file		demo3d_data.h
 *	@brief  コンバートデータのアクセサ
 *	@author	hosaka genya
 *	@data		2009.11.27
 *
 */
//============================================================================
#pragma once

#include <gflib.h>

#include "arc/fieldmap/zone_id.h"

#include "demo/demo3d.h" // for DEMO3D_ID
#include "demo3d_local.h"
#include "demo3d_cmd_def.h" // for DEMO3D_CMD_DATA

#include "system/ica_anime.h" // for ICA_ANIME


#define DEMO3D_BGM_NONE         (0xFFFF)

typedef enum{
  DEMO3D_FRAME_RATE_60FPS,
  DEMO3D_FRAME_RATE_30FPS,
}DEMO3D_FRAME_RATE;

typedef enum{
  DEMO3D_FADE_BLACK,
  DEMO3D_FADE_WHITE,
}DEMO3D_FADE_TYPE;

///シーンパラメータ
typedef struct _DEMO3D_SCENE_DATA{
  u16   camera_bin_id;
  u16   bgm_no;
  u16   zone_id;
 
  u16   frame_rate:1; //<フレームレート 0:60fps, 1:30fps
  u16   double_view_f:1;
  u16   alpha_blend_f:1;
  u16   anti_alias_f:1;
  u16   fog_f:1;
  u16   edge_marking_f:1;
  u16   sbuf_sort_mode:1;
  u16   sbuf_buf_mode:1;
  u16   alpha_test_f:1;
  u16   alpha_test_val:5;
  
  fx32  fovy_sin, fovy_cos, aspect;
  fx32  near, far;
  fx32  scale_w;

  s16   dview_main_ofs, dview_sub_ofs;
  fx32  poly_1dot_depth;

  GXRgb clear_col;
  u16   clear_alpha;
  u16   clear_poly_id:15;
  u16   clear_fog_f:1;
  u16   clear_depth;

  GXRgb fog_col;
  u8    fog_alpha;
  u8    fog_shift;
  u16   fog_offset:15;
  u16   fog_mode:1;
  u8    fog_tbl[8];
  
  GXRgb edge_col[8];

  u8    fadein_type;
  u8    fadein_sync;
  u8    fadeout_type;
  u8    fadeout_sync;
}DEMO3D_SCENE_DATA;

/*
 *  @brief  シーンデータ取得
 */
extern const DEMO3D_SCENE_DATA* Demo3D_DATA_GetSceneData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからユニット数を取得
 *
 *	@param	DEMO3D_ID id  デモID
 *
 *	@retval　ユニット数
 */
//-----------------------------------------------------------------------------
extern u8 Demo3D_DATA_GetUnitMax( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  GFL_G3D_UTIL_SETUPの動的初期化
 *
 *	@retval GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern GFL_G3D_UTIL_SETUP* Demo3D_DATA_InitG3DUtilSetup( HEAPID heapID );

//-----------------------------------------------------------------------------
/**
 *	@brief  GFL_G3D_UTIL_SETUPの解放
 *
 *	@param GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_FreeG3DUtilSetup( GFL_G3D_UTIL_SETUP* sp );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからセットアップデータを取得
 *
 *	@param	DEMO3D_ID id  デモID
 *	@param	setup_idx     セットアップデータのインデックス
 *
 *	@retval GFL_G3D_UTIL_SETUP*
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_GetG3DUtilSetup( GFL_G3D_UTIL_SETUP* sp, DEMO3D_SCENE_ENV* env, u8 idx );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからICAカメラを生成
 *
 *	@param	DEMO3D_ID id  デモID
 *	@param	heapID  ヒープID
 *	@param	buf_interval バッファリングする感覚
 *
 *	@retval ICA_ANIME*
 */
//-----------------------------------------------------------------------------
extern ICA_ANIME* Demo3D_DATA_CreateICACamera( DEMO3D_ID id, HEAPID heapID, int buf_interval );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからコマンドデータを取得
 *
 *	@param	DEMO3D_ID id デモID
 *
 *	@retval DEMO3D_CMD_DATA* コマンドデータ配列へのポインタ
 */
//-----------------------------------------------------------------------------
extern const DEMO3D_CMD_DATA* Demo3D_DATA_GetCmdData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータから終了コマンドデータを取得
 *
 *	@param	DEMO3D_ID id デモID
 *
 *	@retval DEMO3D_CMD_DATA* コマンドデータ配列へのポインタ
 */
//-----------------------------------------------------------------------------
extern const DEMO3D_CMD_DATA* Demo3D_DATA_GetEndCmdData( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからアニメーションスピードを取得
 *
 *	@param	DEMO3D_ID id  デモID
 *
 *	@retval fx32 アニメーションスピード
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetAnimeSpeed( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータから2画面連結表示フラグを取得
 *
 *	@param	DEMO3D_ID id  デモID
 *
 *	@retval BOOL 2画面フラグ
 */
//-----------------------------------------------------------------------------
extern const BOOL Demo3D_DATA_GetDoubleFlag( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからパース値SINを取得
 *
 *	@param	DEMO3D_ID id  デモID
 *
 *	@retval fx32 fovy_sin パース値SIN
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetCameraFovySin( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからパース値COSを取得
 *
 *	@param	DEMO3D_ID id  デモID
 *
 *	@retval fx32 fovy_sin パース値COS
 */
//-----------------------------------------------------------------------------
extern const fx32 Demo3D_DATA_GetCameraFovyCos( DEMO3D_ID id );

//-----------------------------------------------------------------------------
/**
 *	@brief  コンバートデータからフェードイン・アウトパラメータを取得
 *
 *	@param	DEMO3D_ID id  デモID
 */
//-----------------------------------------------------------------------------
extern void Demo3D_DATA_GetFadeParam( DEMO3D_ID id, u8 inout_f, u8* outType, u8* outWait );

