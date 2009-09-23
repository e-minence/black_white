//=============================================================================
/**
 * @file	  ircpoketrade_3d.c
 * @bfief	  ポケモン交換３Ｄ部分
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/18
 */
//=============================================================================


#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/monsno_def.h"

#include "trade.naix"

#include "ircpokemontrade.h"
#include "ircpokemontrade_local.h"
#include "msg/msg_poke_trade.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "font/font.naix"    // NARC_font_default_nclr

#include "system/bmp_winframe.h"
#include "savedata/box_savedata.h"
#include "pokeicon/pokeicon.h"

#include "tradedemo.naix"



#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear	    ( 1 << FX32_SHIFT )
#define cameraFar       ( 400 << FX32_SHIFT )


//============================================================================================
/**
 * @brief BGデータ
 */
//============================================================================================
// 優先順位
enum
{
	PRIORITY_MAIN_BG1,
	PRIORITY_MAIN_BG0,
};

// VRAMバンクの割り当て
static const GFL_DISP_VRAM sc_VRAM_param = 
{ 
	GX_VRAM_BG_128_B,				      // メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,		  	// メイン2DエンジンのBG拡張パレット
	GX_VRAM_SUB_BG_128_C,		  	  // サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,  	// サブ2DエンジンのBG拡張パレット
	GX_VRAM_OBJ_80_EF,				    // メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,		  // メイン2DエンジンのOBJ拡張パレット
	GX_VRAM_SUB_OBJ_16_I,			    // サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
	GX_VRAM_TEX_0_A,				      // テクスチャイメージスロット
	GX_VRAM_TEXPLTT_0_G,			    // テクスチャパレットスロット
	GX_OBJVRAMMODE_CHAR_1D_32K,
	GX_OBJVRAMMODE_CHAR_1D_32K,
};


// BGモード設定
static const GFL_BG_SYS_HEADER sc_BG_mode = 
{
	GX_DISPMODE_GRAPHICS,	// 表示モード
	GX_BGMODE_0,		   	  // BGモード(メインスクリーン)
	GX_BGMODE_0,		      // BGモード(サブスクリーン) 
	GX_BG0_AS_3D,		      // BG0の2D or 3Dモード選択
};

// BGコントロール設定
static const GFL_BG_BGCNT_HEADER sc_BGCNT1_M = 
{
	0, 0,				              // 初期表示位置
	0x800,					          // スクリーンバッファサイズ
	0,						            // スクリーンバッファオフセット
	GFL_BG_SCRSIZ_256x256,	  // スクリーンサイズ
	GX_BG_COLORMODE_16,		    // カラーモード
	GX_BG_SCRBASE_0x0000,	    // スクリーンベースブロック
	GX_BG_CHARBASE_0x04000,	  // キャラクタベースブロック
	GFL_BG_CHRSIZ_256x256,	  // キャラクタエリアサイズ
	GX_BG_EXTPLTT_01,		      // BG拡張パレットスロット選択
	PRIORITY_MAIN_BG1,		    // 表示プライオリティー
	0,						            // エリアオーバーフラグ
	0,						            // DUMMY
	FALSE,					          // モザイク設定
};

static const GFL_BG_BGCNT_HEADER sc_BGCNT1_S = 
{
	0, 0,				            // 初期表示位置
	0x800,					        // スクリーンバッファサイズ
	0,						          // スクリーンバッファオフセット
	GFL_BG_SCRSIZ_256x256,	// スクリーンサイズ
	GX_BG_COLORMODE_16,		  // カラーモード
	GX_BG_SCRBASE_0x0000,	  // スクリーンベースブロック
	GX_BG_CHARBASE_0x04000,	// キャラクタベースブロック
	GFL_BG_CHRSIZ_256x256,	// キャラクタエリアサイズ
	GX_BG_EXTPLTT_01,		    // BG拡張パレットスロット選択
	0,						          // 表示プライオリティー
	0,						          // エリアオーバーフラグ
	0,						          // DUMMY
	FALSE,					        // モザイク設定
};


//ライト初期設定データ
static const GFL_G3D_LIGHT_DATA light_data[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light_setup = { light_data, NELEMS(light_data) };
static GFL_G3D_LIGHTSET* g_lightSet;


//-------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 */
//-------------------------------------------------------------------------------------------
static void _demoInit( HEAPID heap_id )
{
	//VRAM設定
//	GFL_DISP_SetBank(&sc_VRAM_param );

  // BGL初期化
//	GFL_BG_Init( heap_id );	// システム初期化
//	GFL_BG_SetBGMode( &sc_BG_mode );	  // BGモード設定
//	GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &sc_BGCNT1_M, GFL_BG_MODE_TEXT );	// MAIN_BG1の設定 
//	GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );						// MAIN_BG0を表示ON
//	GFL_BG_SetBGControl3D( PRIORITY_MAIN_BG0 );		              // 3D面の表示優先順位を設定

  // 3Dシステムを初期化
 // GFL_G3D_Init( 
 //   GFL_G3D_VMANLNK, GFL_G3D_TEX128K, 
  //    GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0x1000, heap_id, NULL );

  // ライト作成
  g_lightSet = GFL_G3D_LIGHT_Create( &light_setup, heap_id );
  GFL_G3D_LIGHT_Switching( g_lightSet );

  // カメラ初期設定
  {
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 }; 
    proj.param1 = FX_SinIdx( cameraPerspway ); 
    proj.param2 = FX_CosIdx( cameraPerspway ); 
    GFL_G3D_SetSystemProjection( &proj );	
  }
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 終了処理
 */
//-------------------------------------------------------------------------------------------
static void _demoExit()
{
  // ライトセット破棄
  GFL_G3D_LIGHT_Delete( g_lightSet );

  // 3Dシステム終了
  GFL_G3D_Exit();

  // BGL破棄
  GFL_BG_Exit();
}


//============================================================================================
/**
 * @brief 3Dデータ
 */
//============================================================================================

static const GFL_G3D_UTIL_RES res_table_ball[] = 
{
  { ARCID_POKETRADE,  //ARCID_POKETRADEDEMO, 
    NARC_trade_box_001_nsbmd, //NARC_tradedemo_trade_nsbmd,
    GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_ANM anm_table_ball[] = 
{
  { 1, 0 },
  { 2, 0 },
};
static const GFL_G3D_UTIL_OBJ obj_table_ball[] = 
{
  {
    0,                         // モデルリソースID
    0,                         // モデルデータID(リソース内部INDEX)
    0,                         // テクスチャリソースID
    NULL,
    0,
 //   anm_table_ball,           // アニメテーブル(複数指定のため)
 //   NELEMS(anm_table_ball),   // アニメリソース数
  },
}; 

// セットアップデータ
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_ball, NELEMS(res_table_ball), obj_table_ball, NELEMS(obj_table_ball) },
};


//============================================================================================
/**
 * @brief プロトタイプ宣言
 */
//============================================================================================
static void Initialize( IRC_POKEMON_TRADE* work );
static void Finalize( IRC_POKEMON_TRADE* work );
static void Draw( IRC_POKEMON_TRADE* work );


//============================================================================================
/**
 * @brief 初期化関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_Init( IRC_POKEMON_TRADE* pWork )
{
  VecFx32 campos;
  VecFx32 tarpos;

  // 初期化処理
  _demoInit( pWork->heapID );
  Initialize( pWork );

  campos.x = 0*FX32_ONE;
  campos.y = FX32_ONE*43;
  campos.z = 241*FX32_ONE;
  tarpos.x = 0;
  tarpos.y = FX32_ONE*43;
  tarpos.z = 0;
  
  GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
  GFL_G3D_CAMERA_SetFar( pWork->camera, &campos.z );
  tarpos.x = 0;
  tarpos.y = FX32_ONE;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetCamUp( pWork->camera, &tarpos );

}


//============================================================================================
/**
 * @brief メイン関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_Main( IRC_POKEMON_TRADE* pWork )
{
	IRC_POKEMON_TRADE* work = pWork;
  BOOL end = FALSE;
  VecFx32 campos;
  VecFx32 tarpos;

#if 0  // リングの操作
  
  GFL_G3D_CAMERA_GetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_GetTarget( pWork->camera, &tarpos );
  if(GFL_UI_KEY_GetCont()& PAD_KEY_UP){
    campos.z-=FX32_ONE;
    GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
    OS_TPrintf("pos %d %d %d\n",campos.x , FX32_ONE, campos.z/FX32_ONE);
  }
  else if(GFL_UI_KEY_GetCont()& PAD_KEY_DOWN){
    campos.z+=FX32_ONE;
    GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
    OS_TPrintf("pos %d %d %d\n",campos.x,campos.y,campos.z);
  }

  {
    fx32 far;
    GFL_G3D_CAMERA_GetFar( work->camera, &far );
  
    if(GFL_UI_KEY_GetCont()& PAD_KEY_UP){
      far-=FX32_ONE;
      GFL_G3D_CAMERA_SetFar( work->camera, &far );
      OS_TPrintf("far %d \n",far/FX32_ONE);
    }
    else if(GFL_UI_KEY_GetCont()& PAD_KEY_DOWN){
      far+=FX32_ONE;
      GFL_G3D_CAMERA_SetFar( work->camera, &far );
      OS_TPrintf("far %d \n",far/FX32_ONE);
    }
  }

  
//  {
//    fx32 far = FX32_ONE * 0;
//    GFL_G3D_CAMERA_SetFar( work->camera, &far );
//  }


  if(GFL_UI_KEY_GetCont()& PAD_KEY_LEFT){
    tarpos.y-=FX32_ONE;
    campos.y-=FX32_ONE;
    GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
    GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
    OS_TPrintf("tar %d %d %d\n",tarpos.x,tarpos.y,tarpos.z);
  }
  else if(GFL_UI_KEY_GetCont()& PAD_KEY_RIGHT){
    tarpos.y+=FX32_ONE;
    campos.y+=FX32_ONE;
    GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
    GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
    OS_TPrintf("tar %d %d %d\n",tarpos.x,tarpos.y,tarpos.z);
  }

#endif



  Draw( work );
}


//============================================================================================
/**
 * @brief 終了関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_End( IRC_POKEMON_TRADE* pWork )
{
	IRC_POKEMON_TRADE* work = pWork;

  // 終了処理
//  ICA_ANIME_Delete( work->icaAnime );
  Finalize( work );

  _demoExit();

} 


//============================================================================================
/**
 * @brief 非公開関数の定義
 */
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @breif 初期化
 */
//-------------------------------------------------------------------------------------------- 
static void Initialize( IRC_POKEMON_TRADE* work )
{
  // 3D管理ユーティリティーのセットアップ
  work->g3dUtil = GFL_G3D_UTIL_Create( 10, 10, work->heapID );

  // ユニット追加
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      work->unitIndex[i] = GFL_G3D_UTIL_AddUnit( work->g3dUtil, &setup[i] );
    }
  }

  // アニメーション有効化
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // icaデータをロード
//  work->icaAnime = ICA_ANIME_CreateStreamingAlloc(
//      work->heapID, ARCID_POKETRADEDEMO, NARC_debug_obata_ica_test_data2_bin, 10 );

  // カメラ作成
  {
    VecFx32    pos = { 0, 0, 0 };
    VecFx32 target = { 0, 0, 0 };
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, work->heapID );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @breif 終了
 */
//-------------------------------------------------------------------------------------------- 
static void Finalize( IRC_POKEMON_TRADE* work )
{ 
  // カメラ破棄
  GFL_G3D_CAMERA_Delete( work->camera );

  // ユニット破棄
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_UTIL_DelUnit( work->g3dUtil, work->unitIndex[i] );
    }
  }

  // 3D管理ユーティリティーの破棄
  GFL_G3D_UTIL_Delete( work->g3dUtil );
}


//--------------------------------------------------------------------------------------------
/**
 * @breif 描画
 */
//-------------------------------------------------------------------------------------------- 
static void Draw( IRC_POKEMON_TRADE* work )
{
  static fx32 frame = 0;
  static fx32 anime_speed = FX32_ONE;
  GFL_G3D_OBJSTATUS status;
  int a;

  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  a = -work->FriendBoxScrollNum * 65535 / 2976;
  
  MTX_RotY33(	&status.rotate,FX_SinIdx((u16)a),FX_CosIdx((u16)a));
  
  // カメラ更新
//  ICA_ANIME_SetCameraStatus( work->icaAnime, work->camera );
  GFL_G3D_CAMERA_Switching( work->camera );

  // TEMP: カメラ設定
//  {
//    fx32 far = FX32_ONE * 0;
//    GFL_G3D_CAMERA_SetFar( work->camera, &far );
//  }

  // アニメーション更新
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
  }

  // 描画
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
  GFL_G3D_DRAW_End();

  frame += anime_speed;
//  ICA_ANIME_IncAnimeFrame( work->icaAnime, anime_speed );
}
