//=============================================================================
/**
 * @file	  pokemontrade_3d.c
 * @bfief	  ポケモン交換３Ｄ部分
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/09/18
 */
//=============================================================================


#define DEF_POKEMONTRADE_OFS_POS_ARRANGE  // これが定義されているとき、MCSS_SetOfsPositionをポケモンごとに個別調整できる


#include <gflib.h>

#include "system/main.h"
#include "arc_def.h"
#include "poke_tool/monsno_def.h"

#include "trade.naix"

#include "net/dwc_rap.h"
#include "net_app/pokemontrade.h"
#include "pokemontrade_local.h"
#include "msg/msg_poke_trade.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "font/font.naix"    // NARC_font_default_nclr

#include "system/bmp_winframe.h"
#include "savedata/box_savedata.h"
#include "pokeicon/pokeicon.h"

#include "box_gra.naix"
#include "tradedemo.naix"
#include "tradeirdemo.naix"
#include "system/ica_anime.h"
#include "system/ica_camera.h"

#define UNIT_NULL		(0xffff)
#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear	    ( 1 << FX32_SHIFT )
#define cameraFar       ( 400 << FX32_SHIFT )

static void _polygondraw( POKEMON_TRADE_WORK* pWork);
static void _paletteLoad(POKEMON_TRADE_WORK *pWork);
static void _paletteFade(POKEMON_TRADE_WORK *pWork);


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




//ライト初期設定データ
static const GFL_G3D_LIGHT_DATA light_data[] = {
  { 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light_setup = { light_data, NELEMS(light_data) };
static GFL_G3D_LIGHTSET* g_lightSet;

typedef void (MODEL3D_CAMERA_FUNC)(POKEMON_TRADE_WORK* pWork);
typedef void (MODEL3D_MOVE_FUNC)(POKEMON_TRADE_WORK* pWork,GFL_G3D_OBJSTATUS* pStatus);


typedef struct
{
  MODEL3D_CAMERA_FUNC* setCamera;
  MODEL3D_MOVE_FUNC* setMove;
} MODEL3D_SET_FUNCS;


//-------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 */
//-------------------------------------------------------------------------------------------
static void _demoInit( HEAPID heap_id )
{


  // ライト作成
  g_lightSet = GFL_G3D_LIGHT_Create( &light_setup, heap_id );
  GFL_G3D_LIGHT_Switching( g_lightSet );

  // カメラ初期設定
  {
//    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 };
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJORTH, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 };
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
}



//============================================================================================
/**
 * @brief 3Dデータ

 */
//============================================================================================

static const GFL_G3D_UTIL_RES res_table_reel[] =
{
  { ARCID_POKETRADE,   NARC_trade_box_001_nsbmd,   GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_trade1[] =
{
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_matome_test_nsbmd,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_matome_test_nsbca,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_matome_test_nsbta,    GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_trade_ir[] =
{
  { ARCID_POKETRADEDEMO_IR,    NARC_tradeirdemo_ir_sean_nsbmd,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO_IR,    NARC_tradeirdemo_ir_sean_nsbca,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO_IR,    NARC_tradeirdemo_ir_sean_nsbta,    GFL_G3D_UTIL_RESARC },
};


static const GFL_G3D_UTIL_RES res_table_tradeup[] =
{
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_up_nsbmd,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_up_nsbca,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_up_nsbta,    GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_trademiddle[] =
{
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_middle_nsbmd,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_middle_nsbca,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_middle_nsbta,    GFL_G3D_UTIL_RESARC },
};

static const GFL_G3D_UTIL_RES res_table_tradedown[] =
{
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_down_nsbmd,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_down_nsbca,    GFL_G3D_UTIL_RESARC },
  { ARCID_POKETRADEDEMO,    NARC_tradedemo_short_down_nsbta,    GFL_G3D_UTIL_RESARC },
};



static const GFL_G3D_UTIL_ANM anm_table_trade1[] =
{
  { 1, 0 },
  { 2, 0 },
};


static const GFL_G3D_UTIL_OBJ obj_table_reel[] =
{
  {
    0,                         // モデルリソースID
    0,                         // モデルデータID(リソース内部INDEX)
    0,                         // テクスチャリソースID
    0,           // アニメテーブル(複数指定のため)
    NULL,   // アニメリソース数
  },
};


static const GFL_G3D_UTIL_OBJ obj_table_trade1[] =
{
  {
    0,                         // モデルリソースID
    0,                         // モデルデータID(リソース内部INDEX)
    0,                         // テクスチャリソースID
    anm_table_trade1,           // アニメテーブル(複数指定のため)
    NELEMS(anm_table_trade1),   // アニメリソース数
  }
};


// セットアップデータ
static const GFL_G3D_UTIL_SETUP setup[] =
{
//  { res_table_reel, NELEMS(res_table_reel), obj_table_reel, NELEMS(obj_table_reel) },
  { NULL, 0, NULL, 0},
  { res_table_trade1, NELEMS(res_table_trade1), obj_table_trade1, NELEMS(obj_table_trade1) },
  { res_table_trade_ir, NELEMS(res_table_trade_ir), obj_table_trade1, NELEMS(obj_table_trade1) },
  { res_table_tradeup, NELEMS(res_table_tradeup), obj_table_trade1, NELEMS(obj_table_trade1) },
  { res_table_trademiddle, NELEMS(res_table_trademiddle), obj_table_trade1, NELEMS(obj_table_trade1) },
  { res_table_tradedown, NELEMS(res_table_tradedown), obj_table_trade1, NELEMS(obj_table_trade1) },

};


static void _cameraSetReel(POKEMON_TRADE_WORK* pWork)
{
  VecFx32 campos;
  VecFx32 tarpos;

  campos.x = 0*FX32_ONE;
  campos.y = FX32_ONE*43;
  campos.z = 241*FX32_ONE;
  tarpos.x = 0;
  tarpos.y = FX32_ONE*43;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
  campos.z-=FX32_ONE;
  GFL_G3D_CAMERA_SetFar( pWork->camera, &campos.z );
  tarpos.x = 0;
  tarpos.y = FX32_ONE;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetCamUp( pWork->camera, &tarpos );
}

static void _cameraSetTrade01(POKEMON_TRADE_WORK* pWork)
{
  VecFx32 campos;
  VecFx32 tarpos;
  fx32 far= 8000*FX32_ONE;
  fx32 near= FX32_ONE;

  campos.x = 0;
  campos.y = 3*FX32_ONE;
  campos.z = 15*FX32_ONE;
  tarpos.x = 0;
  tarpos.y = 0;
  tarpos.z = 0;

  GFL_G3D_CAMERA_SetPos( pWork->camera, &campos );
  GFL_G3D_CAMERA_SetTarget( pWork->camera, &tarpos );
  GFL_G3D_CAMERA_SetNear( pWork->camera, &near );
  GFL_G3D_CAMERA_SetFar( pWork->camera, &far );
  tarpos.x = 0;
  tarpos.y = 16*FX32_ONE;
  tarpos.z = FX32_ONE;

  GFL_G3D_CAMERA_SetCamUp( pWork->camera, &tarpos );
}


static void _moveSetReel(POKEMON_TRADE_WORK* pWork,GFL_G3D_OBJSTATUS* pStatus)
{
  _polygondraw(pWork);
}

static void _moveSetTrade01(POKEMON_TRADE_WORK* pWork,GFL_G3D_OBJSTATUS* pStatus)
{
  int a;

  VEC_Set( &pStatus->trans, 0, 0, 0 );
  VEC_Set( &pStatus->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &pStatus->rotate );

  if(pWork->pPokemonTradeDemo){
    if(!pWork->pPokemonTradeDemo->bCamera){
      ICA_CAMERA_SetCameraPos( pWork->camera ,pWork->pPokemonTradeDemo->icaCamera );
    }
    if(!pWork->pPokemonTradeDemo->bTarget){
      ICA_CAMERA_SetTargetPos( pWork->camera ,pWork->pPokemonTradeDemo->icaTarget );
    }
  }

#if 0  //カメラ位置確認用
  {
    VecFx32 pos;
    fx32 far;;
    GFL_G3D_CAMERA_GetPos( pWork->camera , &pos );
    OS_TPrintf("pos %d %d %d\n",pos.x/FX32_ONE,pos.y/FX32_ONE,pos.z/FX32_ONE);

    GFL_G3D_CAMERA_GetFar(pWork->camera ,&far);
    OS_TPrintf("far %d \n",far/FX32_ONE);


    GFL_G3D_CAMERA_GetTarget( pWork->camera , &pos );
    pos.x += FX32_ONE*4;
    pos.y += FX32_ONE*4;
    pos.z += FX32_ONE*4;
    GFL_G3D_CAMERA_SetPos( pWork->camera , &pos );
  }
#endif

  GFL_G3D_CAMERA_Switching(pWork->camera );


  {
    // 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
    G3X_SetShading( GX_SHADING_HIGHLIGHT );
    G3X_AntiAlias( TRUE );
    G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
    G3X_AlphaBlend( FALSE );		// アルファブレンド　オン
    G3X_EdgeMarking( FALSE );
    G3X_SetFog( TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

    // クリアカラーの設定
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
    // ビューポートの設定
    G3_ViewPort(0, 0, 255, 191);

    // ライト設定
    {
      static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] =
      {
        {
          { 0, -FX16_ONE, 0 },
          GX_RGB( 16,16,16),
        },
        {
          { 0, FX16_ONE, 0 },
          GX_RGB( 16,16,16),
        },
        {
          { 0, -FX16_ONE, 0 },
          GX_RGB( 16,16,16),
        },
        {
          { 0, -FX16_ONE, 0 },
          GX_RGB( 16,16,16),
        },
      };
      int i;

      for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
        GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
      }
    }
  }

  if(pWork->pPokemonTradeDemo){
    if(pWork->pPokemonTradeDemo->pBallInPer){
      VecFx32 pos;
      ICA_ANIME_GetTranslate( pWork->pPokemonTradeDemo->icaBallin, &pos );
      GFL_PTC_SetEmitterPosition(pWork->pPokemonTradeDemo->pBallInPer, &pos);
    }
    if(pWork->pPokemonTradeDemo->pBallOutPer){
      VecFx32 pos;
      ICA_ANIME_GetTranslate( pWork->pPokemonTradeDemo->icaBallout, &pos );
      GFL_PTC_SetEmitterPosition(pWork->pPokemonTradeDemo->pBallOutPer, &pos);
    }
  }


  // カメラ位置を求める



}


static const MODEL3D_SET_FUNCS modelset[] =
{
  { _cameraSetReel,  _moveSetReel },
  { _cameraSetTrade01,_moveSetTrade01 },
  { _cameraSetTrade01,_moveSetTrade01 },
  { _cameraSetTrade01,_moveSetTrade01 },
  { _cameraSetTrade01,_moveSetTrade01 },
  { _cameraSetTrade01,_moveSetTrade01 },
};


//============================================================================================
/**
 * @brief ICA初期化
 */
//============================================================================================

void POKEMONTRADE_DEMO_ICA_Init(POKEMONTRADE_DEMO_WORK* pWork,int type)
{
  switch(type){
  case _DEMO_TYPE_ALL:

    pWork->icaCamera = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_icacamera_bin, 10 );
    pWork->icaTarget = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_icatarget_bin, 10 );
    pWork->icaBallin = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_ball_001_bin, 10 );
    pWork->icaBallout = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_ball_002_bin, 10 );
    break;

  case _DEMO_TYPE_UP:
    pWork->icaCamera = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_up_cameraichi_bin, 10 );
    pWork->icaTarget = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_up_camerachusiten_bin, 10 );
//      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_icatarget_bin, 10 );
    pWork->icaBallin = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_ball_001_bin, 10 );
    break;
  case _DEMO_TYPE_MIDDLE:
    pWork->icaCamera = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_mcamera_ichi_bin, 10 );
    pWork->icaTarget = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_mcamera_chushiten_bin, 10 );
    break;
  case _DEMO_TYPE_DOWN:
    pWork->icaCamera = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_down_cameraichi_bin, 10 );
    pWork->icaTarget = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_short_down_camerachushiten_bin, 10 );
    pWork->icaBallout = ICA_ANIME_CreateStreamingAlloc(
      pWork->heapID, ARCID_POKETRADEDEMO, NARC_tradedemo_ball_002_bin, 10 );
//後半部分から再生
     ICA_ANIME_SetAnimeFrame( pWork->icaBallout, FX32_ONE*_POKECHANGE_WHITEIN_START );
    
    break;
  }


}


//============================================================================================
/**
 * @brief ICA消去
 */
//============================================================================================

void POKEMONTRADE_DEMO_ICA_Delete(POKEMONTRADE_DEMO_WORK* pWork)
{
  if(pWork->icaCamera){
    ICA_ANIME_Delete(pWork->icaCamera);
    pWork->icaCamera=NULL;
  }
  if(pWork->icaTarget){
    ICA_ANIME_Delete(pWork->icaTarget);
    pWork->icaTarget=NULL;
  }
  if(pWork->icaBallin){
    ICA_ANIME_Delete(pWork->icaBallin);
    pWork->icaBallin=NULL;
  }
  if(pWork->icaBallout){
    ICA_ANIME_Delete(pWork->icaBallout);
    pWork->icaBallout=NULL;
  }
}


//============================================================================================
/**
 * @brief ICA初期化
 */
//============================================================================================

void POKEMONTRADE_DEMO_IRICA_Init(POKEMONTRADE_DEMO_WORK* pWork)
{
  pWork->icaCamera = ICA_ANIME_CreateStreamingAlloc(
    pWork->heapID, ARCID_POKETRADEDEMO_IR, NARC_tradeirdemo_icacamera_bin, 10 );
  pWork->icaTarget = ICA_ANIME_CreateStreamingAlloc(
    pWork->heapID, ARCID_POKETRADEDEMO_IR, NARC_tradeirdemo_icatarget_bin, 10 );
}

//============================================================================================
/**
 * @brief ICA消去
 */
//============================================================================================

void POKEMONTRADE_DEMO_IRICA_Delete(POKEMONTRADE_DEMO_WORK* pWork)
{
  POKEMONTRADE_DEMO_ICA_Delete( pWork);

}




//============================================================================================
/**
 * @brief パーティクル初期化 読み込み
 */
//============================================================================================
void POKEMONTRADE_DEMO_PTC_Init( POKEMONTRADE_DEMO_WORK* pWork )
{
  VecFx32 campos;
  VecFx32 tarpos;
  int i;

  //パーティクルシステムワーク初期化
  GFL_PTC_Init(pWork->heapID);
  pWork->effectRes = NULL;

  {
    int i;
    BOOL bCamera;
    for(i=0;i<PTC_KIND_NUM_MAX;i++){
      pWork->ptcheap[i] = GFL_HEAP_AllocClearMemory(pWork->heapID, PARTICLE_LIB_HEAP_SIZE);
      switch(i){
      case PTC_KIND_DEMO1:
      case PTC_KIND_DEMO2:
      case PTC_KIND_DEMO3:
      case PTC_KIND_DEMO6:
      case PTC_KIND_DEMO7:
      case PTC_KIND_ORG:
        bCamera=FALSE;
        break;
      default:
        bCamera=TRUE;
        break;
      }
      pWork->ptc[i] = GFL_PTC_Create(pWork->ptcheap[i], PARTICLE_LIB_HEAP_SIZE, bCamera, pWork->heapID);
    }

  }
}
void POKEMONTRADE_DEMO_PTC_Load1( POKEMONTRADE_DEMO_WORK* pWork )
{
  //リソース読み込み＆登録
  {
    int i;
    pWork->effectRes = GFL_PTC_LoadArcResource(
      ARCID_POKETRADEDEMO, NARC_tradedemo_demo_tex001_spa, GetHeapLowID(pWork->heapID));
    for(i=0;i<4;i++)
    {
      
      OS_WaitInterrupt( TRUE, OS_IE_V_BLANK );
      GFL_PTC_SetResourceEx(pWork->ptc[i], pWork->effectRes, TRUE, GFUser_VIntr_GetTCBSYS());
      //GFL_PTC_SetResource(pWork->ptc[i], pWork->effectRes, TRUE, NULL);
    }
  }
}
void POKEMONTRADE_DEMO_PTC_Load2( POKEMONTRADE_DEMO_WORK* pWork )
{
  //リソース登録
  {
    int i;
    for(i=4;i<PTC_KIND_NUM_MAX;i++)
    {
      OS_WaitInterrupt( TRUE, OS_IE_V_BLANK );
      GFL_PTC_SetResourceEx(pWork->ptc[i], pWork->effectRes, TRUE, GFUser_VIntr_GetTCBSYS());
      //GFL_PTC_SetResource(pWork->ptc[i], pWork->effectRes, TRUE, NULL);
    }
  }
}
/*
void POKEMONTRADE_DEMO_PTC_LoadOne_Init( POKEMONTRADE_DEMO_WORK* pWork )
{
  //リソース読み込み＆登録
  pWork->effLoadCnt = 0;
  pWork->effectRes = GFL_PTC_LoadArcResource(
    ARCID_POKETRADEDEMO, NARC_tradedemo_demo_tex001_spa, GetHeapLowID(pWork->heapID));
}
const BOOL POKEMONTRADE_DEMO_PTC_LoadOne_Loop( POKEMONTRADE_DEMO_WORK* pWork )
{
  //リソース登録
  OS_WaitInterrupt( TRUE, OS_IE_V_BLANK );
//  GFL_PTC_SetResourceEx(pWork->ptc[pWork->effLoadCnt], pWork->effectRes, FALSE, GFUser_VIntr_GetTCBSYS());
  GFL_PTC_SetResource(pWork->ptc[pWork->effLoadCnt], pWork->effectRes, TRUE, NULL);
  
  pWork->effLoadCnt++;
  if( pWork->effLoadCnt < PTC_KIND_NUM_MAX )
  {
    return FALSE;
  }
  return TRUE;

}
*/
//============================================================================================
/**
 * @brief パーティクル終了関数
 */
//============================================================================================
void POKEMONTRADE_DEMO_PTC_End( POKEMONTRADE_DEMO_WORK* pWork ,int num)
{
  int i;

  GFL_PTC_Exit();
  if( pWork->effectRes != NULL )
  {
    GFL_HEAP_FreeMemory( pWork->effectRes );
    pWork->effectRes = NULL;
  }
  for(i=0;i<PTC_KIND_NUM_MAX;i++){
    if(pWork->ptcheap[i]){
      GFL_HEAP_FreeMemory(pWork->ptcheap[i]);
      pWork->ptcheap[i]=NULL;
    }
  }
}


//============================================================================================
/**
 * @brief IRパーティクル初期化 読み込み
 */
//============================================================================================
void POKEMONTRADE_DEMO_IRPTC_Init( POKEMONTRADE_DEMO_WORK* pWork )
{
  VecFx32 campos;
  VecFx32 tarpos;
  int i;

  //パーティクルシステムワーク初期化
  GFL_PTC_Init(pWork->heapID);

  {
    int i;
    BOOL bCamera;
    for(i = 0 ; i < IRPTC_KIND_NUM_MAX ; i++){
      pWork->ptcheap[i] = GFL_HEAP_AllocClearMemory(pWork->heapID, PARTICLE_LIB_HEAP_SIZE);
      bCamera = FALSE;
      pWork->ptc[i] = GFL_PTC_Create(pWork->ptcheap[i], PARTICLE_LIB_HEAP_SIZE, bCamera, pWork->heapID);
    }

  }
  //リソース読み込み＆登録
  {
    void *resource;
    int i;
    resource = GFL_PTC_LoadArcResource(
      ARCID_POKETRADEDEMO_IR, NARC_tradeirdemo_irdemo_spa, pWork->heapID);
    for(i=0;i<IRPTC_KIND_NUM_MAX;i++){
      GFL_PTC_SetResource(pWork->ptc[i], resource, TRUE, NULL);
    }
  }

}

static void _panelLoad(POKEMON_TRADE_WORK* pWork,int boxnum);



//============================================================================================
/**
 * @brief プロトタイプ宣言
 */
//============================================================================================
static void Initialize( POKEMON_TRADE_WORK* pWork,int no );
static void Finalize( POKEMON_TRADE_WORK* pWork );
static void Draw( POKEMON_TRADE_WORK* pWork );


//============================================================================================
/**
 * @brief 初期化関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_Init( POKEMON_TRADE_WORK* pWork )
{
  VecFx32 campos;
  VecFx32 tarpos;
  int i;

  pWork->unitIndex = UNIT_NULL;
  pWork->modelno = -1;

  // 初期化処理
//  _demoInit( GetHeapLowID(pWork->heapID) );
  _demoInit( HEAPID_POKEMONTRADE_STATIC );
  // 3D管理ユーティリティーのセットアップ
//  pWork->g3dUtil = GFL_G3D_UTIL_Create( 20, 20, GetHeapLowID(pWork->heapID) );
  pWork->g3dUtil = GFL_G3D_UTIL_Create( 20, 20, HEAPID_POKEMONTRADE_STATIC );

  // カメラ作成
  if(pWork->camera==NULL)
  {
    VecFx32    pos = { 0, 0, 0 };
    VecFx32 target = { 0, 0, 0 };
    pWork->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, pWork->heapID );
  }
  if(pWork->modelno!=-1){
    modelset[pWork->modelno].setCamera(pWork);
  }

}


//============================================================================================
/**
 * @brief メイン関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_Main( POKEMON_TRADE_WORK* pWork )
{
  Draw( pWork );
}


//============================================================================================
/**
 * @brief 終了関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_End( POKEMON_TRADE_WORK* pWork )
{

  // カメラ破棄
  if(pWork->camera)
  {
    GFL_G3D_CAMERA_Delete( pWork->camera );
  }
  // 3D管理ユーティリティーの破棄
  GFL_G3D_UTIL_Delete( pWork->g3dUtil );

  if(pWork->pPokemonTradeDemo){
    POKEMONTRADE_DEMO_PTC_End(pWork->pPokemonTradeDemo, IRPTC_KIND_NUM_MAX);
    POKEMONTRADE_DEMO_IRICA_Delete(pWork->pPokemonTradeDemo);
    GFL_HEAP_FreeMemory(pWork->pPokemonTradeDemo);
    pWork->pPokemonTradeDemo = NULL;
  }

  _demoExit();

}


//============================================================================================
/**
 * @brief カラーテクスチャーを作る関数
 */
//============================================================================================
void IRC_POKETRADE3D_SetColorTex( POKEMON_TRADE_WORK* pWork)
{
   _panelLoad(pWork,25);
}

//============================================================================================
/**
 * @brief モデルをセットしなおす関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_SetModel( POKEMON_TRADE_WORK* pWork, int modelno)
{
  //  if(modelno != TRADE01_OBJECT){
  //    return;
  //  }


  Initialize( pWork, modelno);

  if( pWork->modelno != -1){
    modelset[pWork->modelno].setCamera(pWork);
  }


}

//============================================================================================
/**
 * @brief モデルを消す関数
 */
//============================================================================================
void IRC_POKETRADEDEMO_RemoveModel( POKEMON_TRADE_WORK* pWork)
{
  if(pWork->modelno == -1){
    return;
  }
  Finalize(pWork);
  pWork->modelno = -1;

}



//============================================================================================
/**
 * @brief 非公開関数の定義
 */
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief 初期化
 */
//--------------------------------------------------------------------------------------------
static void Initialize( POKEMON_TRADE_WORK* pWork, int modelno )
{

  // ユニット追加
  GF_ASSERT(modelno < elementof(setup));
  {
    if(setup[modelno].objCount!=0){
      pWork->unitIndex = GFL_G3D_UTIL_AddUnit( pWork->g3dUtil, &setup[modelno] );
    }
    else{
      pWork->unitIndex = UNIT_NULL;
    }
    //    pWork->objCount = setup[modelno].objCount;
  }


  // アニメーション有効化
  if(pWork->unitIndex != UNIT_NULL){
    int i;
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( pWork->g3dUtil, pWork->unitIndex );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        OS_TPrintf("アニメーション有効化%d\n",j);
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }
  pWork->modelno = modelno;


}

//--------------------------------------------------------------------------------------------
/**
 * @brief 終了
 */
//--------------------------------------------------------------------------------------------
static void Finalize( POKEMON_TRADE_WORK* pWork )
{

  // ユニット破棄
  if(pWork->unitIndex != UNIT_NULL){
    GFL_G3D_UTIL_DelUnit( pWork->g3dUtil, pWork->unitIndex );
  }
  pWork->unitIndex = UNIT_NULL;
  //  pWork->objCount = 0;

}


//--------------------------------------------------------------------------------------------
/**
 * @brief 描画
 */
//--------------------------------------------------------------------------------------------


static void Draw( POKEMON_TRADE_WORK* pWork )
{
  static fx32 anime_speed = FX32_ONE;  // 1/60での動作の為
  GFL_G3D_OBJSTATUS status;


  
  if(pWork->modelno==-1){
    return;
  }


  if(modelset[pWork->modelno].setMove){
    modelset[pWork->modelno].setMove(pWork, &status);
  }
  // TEMP: カメラ設定
  //  {
  //    fx32 far = FX32_ONE * 0;
  //    GFL_G3D_CAMERA_SetFar( pWork->camera, &far );
  //  }



  if(UNIT_NULL!=pWork->unitIndex){
    int i;
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( pWork->g3dUtil, pWork->unitIndex );
    int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
    for( i=0; i<anime_count; i++ )
    {
      GFL_G3D_OBJECT_LoopAnimeFrame( obj, i, anime_speed );
    }
  }
  // 描画
  GFL_G3D_DRAW_Start();
  GFL_G3D_CAMERA_Switching( pWork->camera );
  GFL_G3D_DRAW_SetLookAt();

  if(pWork->pPokemonTradeDemo){
    GFL_PTC_DrawAll();	//パーティクル描画
    GFL_PTC_CalcAll();	//パーティクル計算
  }
  {
    if(UNIT_NULL!=pWork->unitIndex){
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( pWork->g3dUtil, pWork->unitIndex );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
  //  GFL_G3D_DRAW_End();

  if(pWork->pPokemonTradeDemo){
    if(pWork->pPokemonTradeDemo->icaCamera){
      if(!pWork->pPokemonTradeDemo->bCamera){
        pWork->pPokemonTradeDemo->bCamera = ICA_ANIME_IncAnimeFrame( pWork->pPokemonTradeDemo->icaCamera, anime_speed );
      }
      if(!pWork->pPokemonTradeDemo->bTarget){
        pWork->pPokemonTradeDemo->bTarget = ICA_ANIME_IncAnimeFrame( pWork->pPokemonTradeDemo->icaTarget, anime_speed );
      }
    }
    if(pWork->pPokemonTradeDemo->icaBallout){
      ICA_ANIME_IncAnimeFrame( pWork->pPokemonTradeDemo->icaBallout, anime_speed );
    }
    if(pWork->pPokemonTradeDemo->icaBallin){
      ICA_ANIME_IncAnimeFrame( pWork->pPokemonTradeDemo->icaBallin, anime_speed );
    }
  }
}



#ifdef DEF_POKEMONTRADE_OFS_POS_ARRANGE
// ポケモンのオフセット配置
typedef struct
{
  int  mons_no;         // 1スタート  // MONSNO_ANNOON
  int  form_no;         // 0スタート  // FORMNO_ANNOON_UNR
  int  sex;             // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN  // prog/include/poke_tool/poke_tool.h
  int  dir;             // MCSS_DIR_FRONT, MCSS_DIR_BACK
 
  f32  ofs_x;           // ポケモンのオフセット配置
  f32  ofs_y;
  f32  ofs_z;
}
POKE_ARRANGE_INFO;
#define MALE_FEMALE_UNKNOWN (3)  // オスメス性別なしどれでも構わない  // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWNと被らない値
#define FRONT_BACK_UNKNOWN  (2)  // 正面背面どれでも構わない  // MCSS_DIR_FRONT, MCSS_DIR_BACKと被らない値
#define POKE_ARRANGE_INFO_TBL_NUM (4)

#define POKE_ARRANGE_ADJUST (0.33f)

static const POKE_ARRANGE_INFO poke_arrange_info_tbl[POKE_ARRANGE_INFO_TBL_NUM] =
{
  { MONSNO_OOTATI, 0, MALE_FEMALE_UNKNOWN, MCSS_DIR_BACK, (-6.0f) *POKE_ARRANGE_ADJUST -0.1f, 0.0f, 0.0f },  // (-6)が本来の値
  { MONSNO_HUWANTE, 0, MALE_FEMALE_UNKNOWN, MCSS_DIR_FRONT, (2.0f) *POKE_ARRANGE_ADJUST +6.0f, 0.0f, 0.0f },  // (2)が本来の値
  { MONSNO_MIROKAROSU, 0, MALE_FEMALE_UNKNOWN, MCSS_DIR_FRONT, (12.0f) *POKE_ARRANGE_ADJUST +0.4f, 0.1f, 0.0f },  // (12)が本来の値
  { MONSNO_YUNGERAA, 0, MALE_FEMALE_UNKNOWN, MCSS_DIR_FRONT, (10.0f) *POKE_ARRANGE_ADJUST, -0.1f, 0.0f },  // (10)が本来の値
};

static void _McssSizeCheck(MCSS_WORK *pWork,VecFx32* pScale, int bFront, const POKEMON_PARAM *pp)
{
  int  mons_no  = (int)PP_Get( pp, ID_PARA_monsno, NULL );
  int  form_no  = (int)PP_Get( pp, ID_PARA_form_no, NULL );
  int  sex      = PP_GetSex( pp );
  int  dir      = (bFront)?(MCSS_DIR_FRONT):(MCSS_DIR_BACK);

  u8       i;
  VecFx32  ofs_position;
  BOOL     b_tbl  = FALSE;

  for( i=0; i<POKE_ARRANGE_INFO_TBL_NUM; i++ )
  {
    if(    poke_arrange_info_tbl[i].mons_no == mons_no
        && poke_arrange_info_tbl[i].form_no == form_no )
    {
      if(
             (    poke_arrange_info_tbl[i].sex == MALE_FEMALE_UNKNOWN
               || poke_arrange_info_tbl[i].sex == sex )
          && (    poke_arrange_info_tbl[i].dir == FRONT_BACK_UNKNOWN
               || poke_arrange_info_tbl[i].dir == dir )
      )
      {
        ofs_position.x  = FX_F32_TO_FX32(poke_arrange_info_tbl[i].ofs_x);
        if(pScale->x >= 0) ofs_position.x = - ofs_position.x;
        ofs_position.y  = FX_F32_TO_FX32(poke_arrange_info_tbl[i].ofs_y);
        ofs_position.z  = FX_F32_TO_FX32(poke_arrange_info_tbl[i].ofs_z);
        b_tbl  = TRUE;
        break;
      }
    }
  }

  if( !b_tbl )
  {
  //  u16 size_y = MCSS_GetSizeY( pWork );  // マルチセルのおおよそのYサイズを取得
    u16 size_x = MCSS_GetSizeX( pWork );  // マルチセルのおおよそのXサイズを取得
  //  s16 offset_y = MCSS_GetOffsetY( pWork );  // マルチセルのY方向のズレを取得  // 浮いているとき-, 沈んでいるとき+
    s16 offset_x = MCSS_GetOffsetX( pWork );  // マルチセルのX方向のズレを取得  // 右にずれているとき+, 左にずれているとき-
  //  f32 ofs_position_y;
    f32 ofs_position_x;
  
  //  ofs_position_y = ( POKE_SIZE_MAX - size_y ) / 2.0f + offset_y;
    if(pScale->x < 0){
      ofs_position_x = (f32)( offset_x) *POKE_ARRANGE_ADJUST;
    }
    else{
      ofs_position_x = (f32)(-offset_x) *POKE_ARRANGE_ADJUST;
    }
  
    ofs_position.x = FX_F32_TO_FX32(ofs_position_x);
    ofs_position.y = 0;
    ofs_position.z = 0;
  }
  
  MCSS_SetOfsPosition( pWork, &ofs_position );  // オフセットポジション
}
#else
static void _McssSizeCheck(MCSS_WORK *pWork,VecFx32* pScale)
{
//  u16 size_y = MCSS_GetSizeY( pWork );  // マルチセルのおおよそのYサイズを取得
  u16 size_x = MCSS_GetSizeX( pWork );  // マルチセルのおおよそのXサイズを取得
//  s16 offset_y = MCSS_GetOffsetY( pWork );  // マルチセルのY方向のズレを取得  // 浮いているとき-, 沈んでいるとき+
  s16 offset_x = MCSS_GetOffsetX( pWork );  // マルチセルのX方向のズレを取得  // 右にずれているとき+, 左にずれているとき-
//  f32 ofs_position_y;
  f32 ofs_position_x;
  VecFx32  ofs_position;
  
//  ofs_position_y = ( POKE_SIZE_MAX - size_y ) / 2.0f + offset_y;
  if(pScale->x < 0){
    ofs_position_x = (f32)( offset_x);
  }
  else{
    ofs_position_x = (f32)(-offset_x);
  }
  
  ofs_position.x = FX_F32_TO_FX32(ofs_position_x);
  ofs_position.y = 0;
  ofs_position.z = 0;
  MCSS_SetOfsPosition( pWork, &ofs_position );  // オフセットポジション
}
#endif

//--------------------------------------------------------------
//	ポケモンMCSS作成
//   bRev  反転フラグ
//   bGTS  正面フラグ
//--------------------------------------------------------------
void IRCPOKETRADE_PokeCreateMcssNormal( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  int xpos[] = { PSTATUS_MCSS_POS_X1 , PSTATUS_MCSS_POS_X2 , PSTATUS_MCSS_POS_X1 , PSTATUS_MCSS_POS_X2};
  int z;
  BOOL rev;

  GF_ASSERT( pWork->pokeMcss[no] == NULL );


  if(bRev){
    u16 mons = PP_Get( pp, ID_PARA_monsno, NULL );
    u16 frm = PP_Get( pp, ID_PARA_form_no, NULL );
    POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle(mons, frm, pWork->heapID);
    rev = POKE_PERSONAL_GetParam(ppd,POKEPER_ID_reverse);
    POKE_PERSONAL_CloseHandle(ppd);
    if(!rev){
      scale.x = -scale.x;
    }
  }
#ifdef BUGFIX_BTS7722_20100713
  if(POKEMONTRADE_TYPE_WIFICLUB == pWork->type){
    GFL_NET_DWC_pausevchat(TRUE);
  }
#endif
  if(bFront){
    pWork->pokeMcss[no] = MCSS_TOOL_AddPokeMcss( pWork->mcssSys,pp,MCSS_DIR_FRONT, xpos[no] , PSTATUS_MCSS_POS_Y , PSTATUS_MCSS_POS_MYZ  );
  }
  else{
    pWork->pokeMcss[no] = MCSS_TOOL_AddPokeMcss( pWork->mcssSys,pp,MCSS_DIR_BACK, xpos[no] , PSTATUS_MCSS_POS_Y , PSTATUS_MCSS_POS_YOUZ  );
  }
#ifdef BUGFIX_BTS7722_20100713
  if(POKEMONTRADE_TYPE_WIFICLUB == pWork->type){
   GFL_NET_DWC_pausevchat(FALSE);
  }
#endif
  MCSS_TOOL_SetAnmRestartCallback(pWork->pokeMcss[no]);
  MCSS_SetScale( pWork->pokeMcss[no] , &scale );
  MCSS_SetShadowVanishFlag(pWork->pokeMcss[no] ,TRUE);
#ifdef DEF_POKEMONTRADE_OFS_POS_ARRANGE
  _McssSizeCheck(pWork->pokeMcss[no], &scale, bFront, pp);
#else
  _McssSizeCheck(pWork->pokeMcss[no], &scale);
#endif
}



//--------------------------------------------------------------
//	ポケモンMCSS作成
//   bRev  反転フラグ
//   bGTS  正面フラグ
//--------------------------------------------------------------
void IRCPOKETRADE_PokeCreateMcssGTS( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev,BOOL bGTS )
{
  int z;
  BOOL rev;

  if(bGTS && !bFront){
    bFront=TRUE;
    bRev = FALSE;
  }
  IRCPOKETRADE_PokeCreateMcssNormal(pWork, no, bFront, pp, bRev);
}

//上のラッパー関数
void IRCPOKETRADE_PokeCreateMcss( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev )
{
  //IRCPOKETRADE_PokeCreateMcssGTS( pWork ,no, bFront, pp, bRev, FALSE );
  IRCPOKETRADE_PokeCreateMcssNormal(pWork, no, bFront, pp, bRev);
}



//--------------------------------------------------------------
//	ポケモンMCSS削除
//--------------------------------------------------------------
void IRCPOKETRADE_PokeDeleteMcss( POKEMON_TRADE_WORK *pWork,int no  )
{
  if( pWork->pokeMcss[no] == NULL ){
    return;
  }

  MCSS_SetVanishFlag( pWork->pokeMcss[no] );
  MCSS_Del(pWork->mcssSys,pWork->pokeMcss[no]);
  pWork->pokeMcss[no] = NULL;
}

#if 1



static s16     gCubeGeometry[3 * 8] = {
  FX16_ONE, FX16_ONE, FX16_ONE,
  FX16_ONE, FX16_ONE, -FX16_ONE,
  FX16_ONE, -FX16_ONE, FX16_ONE,
  FX16_ONE, -FX16_ONE, -FX16_ONE,
  -FX16_ONE, FX16_ONE, FX16_ONE,
  -FX16_ONE, FX16_ONE, -FX16_ONE,
  -FX16_ONE, -FX16_ONE, FX16_ONE,
  -FX16_ONE, -FX16_ONE, -FX16_ONE
  };

static VecFx10 gCubeNormal[6] = {
  GX_VECFX10(0, 0, FX32_ONE - 1),
  GX_VECFX10(0, FX32_ONE - 1, 0),
  GX_VECFX10(FX32_ONE - 1, 0, 0),
  GX_VECFX10(0, 0, -FX32_ONE + 1),
  GX_VECFX10(0, -FX32_ONE + 1, 0),
  GX_VECFX10(-FX32_ONE + 1, 0, 0)
  };

static GXSt    gCubeTexCoord[] = {
  GX_ST(0, 0),
  GX_ST(0, 32 * FX32_ONE),
  GX_ST(32 * FX32_ONE, 0),
  GX_ST(32 * FX32_ONE, 32 * FX32_ONE)
  };

static void vtx(int idx)
{
  G3_Vtx(gCubeGeometry[idx * 3], gCubeGeometry[idx * 3 + 1], gCubeGeometry[idx * 3 + 2]);
}

static void normal(int idx)
{
  G3_Direct1(G3OP_NORMAL, gCubeNormal[idx]);
  // use G3_Normal(x, y, z) if not packed yet
}

static void tex_coord(int idx)
{
  G3_Direct1(G3OP_TEXCOORD, gCubeTexCoord[idx]);
  // use G3_TexCoord if not packed yet
}


static const  u32     myTexAddr = 0x03000;       // a texture image at 0x1000 of the texture image slots
static const  u32     myTexPlttAddr = 0x02000;   // a texture palette at 0x1000 of the texture palette slots

static const  u32     myTexSize = 512;   // a texture palette at 0x1000 of the texture palette slots


// 横方向の定数
#define ONE_COLUMN_SIZE (3)
#define ONE_COLUMN_YOHAKU_SIZE (2)

// 縦方向の定数
#define ONE_RAW_SIZE (3)
#define ONE_RAW_YOHAKU_SIZE (3)

// POKEPARTYの表示
#define POKEPARTY_DRAW_RAW      (3)
#define POKEPARTY_DRAW_COLUMN   (2)
#define POKEPARTY_DRAW_START_IDX_RAW    (1)
#define POKEPARTY_DRAW_START_IDX_COLUMN (2)


//----------------------------------------------------------------------------
/**
 *	@brief  １ポケモン分の描画
 *
 *	@param	tempBuff  バッファ
 *	@param	pos_x     X位置  （０〜BOX_MAX_COLUMN）
 *	@param	pos_y     Y位置 （０〜BOX_MAX_RAW）
 *	@param	colno     色
 */
//-----------------------------------------------------------------------------
static void _printColorSquear(u16* tempBuff, int pos_x, int pos_y, int colno )
{
  int i,j;
  int buff_pos;
  int buff_index;
  int buff_ofs;
  int start_x, start_y;

  start_x = ONE_COLUMN_YOHAKU_SIZE + (pos_x * (ONE_COLUMN_SIZE+ONE_COLUMN_YOHAKU_SIZE));
  start_y = ONE_RAW_YOHAKU_SIZE + (pos_y * (ONE_RAW_SIZE+ONE_RAW_YOHAKU_SIZE));
    
  for(i = start_y; i < start_y+ONE_RAW_SIZE; i++){  //y座標
    for(j = start_x; j < start_x+ONE_COLUMN_SIZE; j++){  //x座標
      buff_pos = (i*32) + j;

      buff_index  = buff_pos / 4;
      buff_ofs    = buff_pos % 4;

      tempBuff[buff_index] &= ~(0xF<<(buff_ofs*4));
      tempBuff[buff_index] |= colno<<(buff_ofs*4);
    }
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンテクスチャ作成
 *	        32 * 32 のテクスチャを生成
 *
 *	@param	tempBuff    バッファ
 *	@param  box         ボックス情報
 *	@param	tray_num    トレイナンバー
 *	@param	on_colno    (16色)
 */
//-----------------------------------------------------------------------------
static void _printColorBox(u16* tempBuff, const u8* box, int tray_num, u16 on_colno )
{
  int i,j;
  int index;
  TOMOYA_Printf( "Boxプリント %d\n", tray_num );
  
  for(i = 0; i < BOX_MAX_RAW; i++){  //y座標
    for(j = 0; j < BOX_MAX_COLUMN; j++){  //x座標
       index = (i *  BOX_MAX_COLUMN) + j;
      _printColorSquear( tempBuff, j, i, box[ (tray_num * BOX_MAX_POS) + index ] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンパーティテクスチャ作成
 *	        32 * 32 のテクスチャを生成
 *
 *	@param	tempBuff    バッファ
 *	@param  party       ポケモンパーティ情報
 *	@param	on_colno    (16色)
 */
//-----------------------------------------------------------------------------
static void _printColorPokeParty(u16* tempBuff, const u8* party_col, u16 on_colno )
{
  int i;
  int poke_num = TEMOTI_POKEMAX;
  int x, y;

  TOMOYA_Printf( "Partyプリント %d\n", poke_num );
  
  for(i = 0; i < poke_num; i++){
    x = POKEPARTY_DRAW_START_IDX_COLUMN + (i % POKEPARTY_DRAW_COLUMN);
    y = POKEPARTY_DRAW_START_IDX_RAW + (i / POKEPARTY_DRAW_COLUMN);

    _printColorSquear( tempBuff, x, y, party_col[i] );
  }
}


//全部のテクスチャーを作る
static void _panelLoad(POKEMON_TRADE_WORK *pWork,int num)
{
  u16* tempBuff;
  //---------------------------------------------------------------------------
  // Download the texture images:
  //
  // Transfer the texture data on the main memory to the texture image slots.
  //---------------------------------------------------------------------------
  GX_BeginLoadTex();                 // map the texture image slots onto LCDC address space

  tempBuff = GFL_HEAP_AllocClearMemory(pWork->heapID, myTexSize);
  {
    int i,index;
    for(i=0;i < num;i++){
      GFL_STD_MemFill(tempBuff, (0x0|(0x0<<4)), myTexSize);

      if( i==0 ){
        _printColorPokeParty( tempBuff, &pWork->FriendPokemonCol[1][BOX_POKESET_MAX], 2 );
      }else{
        _printColorBox( tempBuff, &pWork->FriendPokemonCol[1][0], i-1, 2 );
      }


#if 0
      if(i==BOX_MAX_TRAY){
        for( index=0;index<6;index++){
          _printColorSquear(tempBuff, num%2, num/2, pWork->FriendPokemonCol[1][i*BOX_MAX_POS+index]);
        }
      }
      else{
        for( index=0;index<30;index++){
          _printColorSquear(tempBuff, num%6, num/6, pWork->FriendPokemonCol[1][num*BOX_MAX_POS+index]);
          OS_TPrintf("color %d\n",pWork->FriendPokemonCol[1][i*BOX_MAX_POS+index]);
        }
      }
#endif
        
      DC_FlushRange(tempBuff,myTexSize);

      GX_LoadTex((void *)tempBuff,        // a pointer to the texture data on the main memory(4 bytes aligned)
                 myTexAddr+myTexSize*i,          // an offset address in the texture image slots
                 myTexSize                // the size of the texture(s)(in bytes)
                 );
    }
  }
  GFL_HEAP_FreeMemory(tempBuff);
  GX_EndLoadTex();                   // restore the texture image slots

  _paletteLoad(pWork);
}



#define DEMO_INTENSITY_DF   23
#define DEMO_INTENSITY_AM   26
#define DEMO_INTENSITY_SP   31

static const GXRgb DEMO_DIFFUSE_COL =
GX_RGB(DEMO_INTENSITY_DF, DEMO_INTENSITY_DF, DEMO_INTENSITY_DF);
static const GXRgb DEMO_AMBIENT_COL =
GX_RGB(DEMO_INTENSITY_AM, DEMO_INTENSITY_AM, DEMO_INTENSITY_AM);
static const GXRgb DEMO_SPECULAR_COL =
GX_RGB(DEMO_INTENSITY_SP, DEMO_INTENSITY_SP, DEMO_INTENSITY_SP);
static const GXRgb DEMO_EMISSION_COL = GX_RGB(0, 0, 0);

static const u32 LIGHTING_L_DOT_S_SHIFT = 8;


static void DEMO_Set3DDefaultMaterial(BOOL bUsediffuseAsVtxCol, BOOL bUseShininessTbl)
{
    G3_MaterialColorDiffAmb(DEMO_DIFFUSE_COL,   // diffuse
                            DEMO_AMBIENT_COL,   // ambient
                            bUsediffuseAsVtxCol // use diffuse as vtx color if TRUE
        );

    G3_MaterialColorSpecEmi(DEMO_SPECULAR_COL,  // specular
                            DEMO_EMISSION_COL,  // emission
                            bUseShininessTbl    // use shininess table if TRUE
        );
}



static void _createBoard(fx32 r, int index, u16 rotate, u16 global_rotate)
{
  static u16 rotate_x = 0x299a;
  static u16 rotate_y = 0x8000; //裏から見るために反転
  fx32 height;

    // 角度操作
#if 0
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      rotate_x += 182;
      TOMOYA_Printf( "rotate_x 0x%x\n", rotate_x );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      rotate_x -= 182;
      TOMOYA_Printf( "rotate_x 0x%x\n", rotate_x );
    }
#endif

  
 G3_PushMtx();
 {
    //以下の変換は、ポジションにのみ反映
    //方向ベクトルには反映されない
    G3_MtxMode(GX_MTXMODE_POSITION);
    // 全体にかかる回転
    G3_RotY( FX_SinIdx(global_rotate), FX_CosIdx(global_rotate) );
 
    G3_PushMtx();
    {
      fx16    s = FX_SinIdx(rotate);
      fx16    c = FX_CosIdx(rotate);



      // Y軸のローカル回転角は移動に反映
      G3_RotY( FX_SinIdx(rotate), FX_CosIdx(rotate) );

      // 移動
      G3_Translate(0, 0, r);

      // X軸回転は描画にのみ反映
      G3_RotY( FX_SinIdx(rotate_y), FX_CosIdx(rotate_y) );
      G3_RotX( FX_SinIdx(rotate_x), FX_CosIdx(rotate_x) );

      // まず大本を
      // 拡大
      G3_Scale(FX32_CONST(0.15),FX32_CONST(0.15),FX32_CONST(0.15));

    }

    {
      G3_MtxMode(GX_MTXMODE_TEXTURE);
      G3_Identity();
      // Use an identity matrix for the texture matrix for simplicity
      G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    }

    // Set the material color( diffuse, ambient , specular ) as basic white
    DEMO_Set3DDefaultMaterial(TRUE, TRUE);
    //DEMO_Set3DDefaultShininessTable();

    G3_TexImageParam(GX_TEXFMT_PLTT16,      // use 16 colors palette texture
                     GX_TEXGEN_TEXCOORD,    // use texcoord
                     GX_TEXSIZE_S32,        // 32 pixels
                     GX_TEXSIZE_T32,        // 32 pixels
                     GX_TEXREPEAT_NONE,     // no repeat
                     GX_TEXFLIP_NONE,       // no flip
                     GX_TEXPLTTCOLOR0_USE,  // use color 0 of the palette
                     myTexAddr+index*myTexSize     // the offset of the texture image
                     );

    G3_TexPlttBase(myTexPlttAddr,  // the offset of the texture palette
                   GX_TEXFMT_PLTT16 // 16 colors palette texture
                   );

    G3_PolygonAttr(GX_LIGHTMASK_0, // Light none 
                   GX_POLYGONMODE_MODULATE, // modulation mode
                   GX_CULL_FRONT,   // 裏だけ描画
                   index,              // polygon ID(0 - 63)
                   31,             // alpha(0 - 31)
                   0               // OR of GXPolygonAttrMisc's value
                   );

    G3_Begin(GX_BEGIN_QUADS);
    {
      tex_coord(1);
      normal(0);
      vtx(2);
      tex_coord(0);
      normal(0);
      vtx(0);
      tex_coord(2);
      normal(0);
      vtx(4);
      tex_coord(3);
      normal(0);
      vtx(6);

    }
    G3_End();
    G3_PopMtx(1);
  }
  G3_PopMtx(1);
}



static void _polygondraw(POKEMON_TRADE_WORK *pWork)
{
  G3X_Reset();

  //---------------------------------------------------------------------------
  // Set up a camera matrix
  //---------------------------------------------------------------------------
  {
    static VecFx32 Eye = { 0, 0x100, 0x3000 };   // Eye position
    static VecFx32 at = { 0, 0xaa0, 0 };  // Viewpoint
    static const VecFx32 vUp = { 0, FX32_ONE, 0 };   // Up

    G3_LookAt(&Eye, &vUp, &at, NULL);

    // カメラ操作
#if 0
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
      Eye.z += FX32_HALF;
      TOMOYA_Printf( "Eye.z 0x%x\n", Eye.z );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
      Eye.z -= FX32_HALF;
      TOMOYA_Printf( "Eye.z 0x%x\n", Eye.z );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      Eye.y += FX32_HALF;
      TOMOYA_Printf( "Eye.y 0x%x\n", Eye.y );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      Eye.y -= FX32_HALF;
      TOMOYA_Printf( "Eye.y 0x%x\n", Eye.y );
    }

    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L ){
      Eye.y -= 0x100;
      at.y -= 0x100;
      TOMOYA_Printf( "Eye.y 0x%x\n", Eye.y );
      TOMOYA_Printf( "at.y 0x%x\n", at.y );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R ){
      Eye.y += 0x100;
      at.y += 0x100;
      TOMOYA_Printf( "Eye.y 0x%x\n", Eye.y );
      TOMOYA_Printf( "at.y 0x%x\n", at.y );
    }


    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
      at.y += 0x10;
      OHNO_Printf( "at.y 0x%x\n", at.y );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
      at.y -= 0x10;
      OHNO_Printf( "at.y 0x%x\n", at.y );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      Eye.y += 0x10;
      OHNO_Printf( "Eye.y 0x%x\n", Eye.y );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      Eye.y -= 0x10;
      OHNO_Printf( "Eye.y 0x%x\n", Eye.y );
    }
#endif

  }

  //---------------------------------------------------------------------------
  // Set up light colors and direction.
  // Notice that light vector is transformed by the current vector matrix
  // immediately after LightVector command is issued.
  //
  // GX_LIGHTID_0: white, downward
  //---------------------------------------------------------------------------
  G3_LightVector(GX_LIGHTID_0, FX16_SQRT1_3, -FX16_SQRT1_3, -FX16_SQRT1_3);
  G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));





  {
    // 各スクリーンのスクロール幅 
    enum{
      PARTY_TRAY_SCROLL_NUM = _TEMOTITRAY_MAX,
      BOX_TRAY_SCROLL_NUM = _BOXTRAY_MAX,
    };
    
    int i;
    int tray_num = POKEMONTRADE_GetFriendBoxNum( pWork );
    //計算の都合上、手持ちトレイをBOXと同じサイズと見る
    int scroll_max = (tray_num * BOX_TRAY_SCROLL_NUM)+BOX_TRAY_SCROLL_NUM;  // BoxScrollの最大数
    int num = pWork->FriendBoxScrollNum;
    int roop = tray_num + 1;
    u32 rotate_add;
    u32 global_rotate;
    static fx32 circle_r = 0x13cd;

    //手持ちトレイ補正
    //手持ちトレイ幅96をBOX幅160に補正
    if( num < (PARTY_TRAY_SCROLL_NUM) )
    {
      if( num < 48 )
      {
        num = (num)*112/48;
      }
      else
      {
        num += (112-48);
      }
    }
    else
    {
      num += (BOX_TRAY_SCROLL_NUM-PARTY_TRAY_SCROLL_NUM);
    }

    //更なる手持ちトレイ補正
    //手持ちトレイの幅が変わるので、最後の1つのBOX分を補正する80:80→112:48
    if( num > tray_num*BOX_TRAY_SCROLL_NUM )
    {
      const u32 ofs = num - tray_num*BOX_TRAY_SCROLL_NUM;
      if( ofs < 80 )
      {
        num = tray_num*BOX_TRAY_SCROLL_NUM + (ofs*112/80);
      }
      else
      {
        num = tray_num*BOX_TRAY_SCROLL_NUM + ((ofs-80)*48/80) + 112;
      }
    }

    // 半分ループを微調整 相手の画面でカーソルの中心の場所を移す
    num += (256-BOX_TRAY_SCROLL_NUM)/2;
    num %= scroll_max;

    
    global_rotate = (u64)(0x10000 * (u64)num) / (u64)(scroll_max);
    //TOMOYA_Printf( "num %d scroll_max %d\n", num, scroll_max );

    //TOMOYA_Printf( "global rotate %d num %d scroll_max %d\n", global_rotate / 182, num, scroll_max );

    for(i=0;i<roop;i++){
      rotate_add = (0x10000 * i) / roop;
      _createBoard( circle_r,  i, rotate_add, 0x10000 - global_rotate );
    }

    // 円半径操作
#if 0
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
      circle_r += 0x100;
      TOMOYA_Printf( "circle_r 0x%x\n", circle_r );
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
      circle_r -= 0x100;
      TOMOYA_Printf( "circle_r 0x%x\n", circle_r );
    }
#endif
  }

  
}

#endif


static void _paletteLoad(POKEMON_TRADE_WORK *pWork)
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, pWork->heapID );
  NNSG2dPaletteData* pPal;
  void* pData = GFL_ARCHDL_UTIL_LoadPalette(p_handle, NARC_box_gra_box_tray_NCLR,
                                            &pPal, pWork->heapID );
  u16* paldata = pPal->pRawData;

  DC_FlushRange(&paldata[16*1],32);
  STD_MoveMemory(&paldata[16*1+1],&paldata[16*1],32-2);
  
  paldata[16*1]=0x14a5;  //絵の直値  ベースパレット
  DC_FlushRange(&paldata[16*1],32);

  GX_BeginLoadTexPltt();             // map the texture palette slots onto LCDC address space
  GX_LoadTexPltt(&paldata[16*1], // a pointer to the texture data on the main memory(4 bytes aligned)
                 myTexPlttAddr,  // an offset address in the texture palette slots
                 32);            // the size of the texture palette(s)(in bytes)
  GX_EndLoadTexPltt();               // restore the texture palette slots
  // パレットを保存してフェードアウトの時に使う
  GFL_STD_MemCopy(&paldata[16*1], pWork->palette3d, 32);

  GFL_HEAP_FreeMemory(pData);
  GFL_ARC_CloseDataHandle( p_handle );

}


void POKEMONTRADE3D_3DReelPaletteFade(POKEMON_TRADE_WORK *pWork)
{
  static const int _CHANGEPAL_NUM = 16;
  int pal;

  for(pal = 0; pal < _CHANGEPAL_NUM; pal++){
    int add,rgb,base;
    int shift;
    int mod;
    pWork->palTrans[pal] = 0;
    for(rgb = 0; rgb < 3; rgb++){
      shift = rgb * 5;
      base = (pWork->palette3d[pal] >> shift) & 0x1f;
      if(base!=0){
        base = base - 1;
      }
      pWork->palTrans[pal] |= ( base & 0x1f ) << (shift);
    }
  }

  GX_BeginLoadTexPltt();             // map the texture palette slots onto LCDC address space
  {
    GX_LoadTexPltt(pWork->palTrans,
                   myTexPlttAddr,  // an offset address in the texture palette slots
                   32);            // the size of the texture palette(s)(in bytes)
  }
  GX_EndLoadTexPltt();               // restore the texture palette slots

}


