//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_fourkings.c
 *	@brief  四天王部屋　演出
 *	@author	tomoya takahashi
 *	@date		2009.11.21
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/ica_anime.h"
#include "system/ica_camera.h"

#include "savedata/mystatus.h"

#include "arc/arc_def.h"
#include "arc/fourkings_scene.naix"

#include "field/field_const.h"

#include "event_fourkings.h"

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_camera.h"
#include "fieldmap_func.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
enum
{
  SEQ_INIT00,
  SEQ_INIT01,
  SEQ_UPDATE,
  SEQ_EXIT,
};


// ストリーミング再生
#define EV_CAMERA_ANIME_STREAMING_INTERVAL  ( 10 )

// 各リソースＩＤ取得マクロ
#define EV_CAMERA_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_camera_bin + ((x)) )
#define EV_HERO_M_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_m_nsbmd + ((x) * 4) )
#define EV_HERO_F_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_f_nsbmd + ((x) * 4) )
#define EV_HERO_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_mf_nsbca + ((x) * 4) )

// アニメーションスピード
#define EV_CAMERA_ANIME_SPEED (FX32_ONE)

// 自機デモ後出現位置
#define EV_HERO_WALK_END_GRID_X (16)
#define EV_HERO_WALK_END_GRID_Y (0)
#define EV_HERO_WALK_END_GRID_Z (17)
#define EV_HERO_WALK_END_DIR (DIR_LEFT)

// 演出の中心
//static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(10) };
static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(16) };

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	カメラ動作
//=====================================
typedef struct {
  ICA_ANIME* p_anime;
  GFL_G3D_CAMERA* p_dummy_camera;
  FIELD_CAMERA* p_camera;
  u16 camera_mode_save;
  u16 camera_area_save;
  VecFx32 camera_target_offset_save;
  VecFx32 camera_up_save;

  fx32 frame;
} EV_CIRCLEWALK_CAMERA;

//-------------------------------------
///	自機ダミー動作
//=====================================
typedef struct {
  // リソース
  GFL_G3D_RES* p_mdlres;
  GFL_G3D_RES* p_anmres;
  BOOL trans;

  // 管理オブジェ
  GFL_G3D_RND* p_rend;
  GFL_G3D_ANM* p_anm[1];
  GFL_G3D_OBJ* p_obj;
  GFL_G3D_OBJSTATUS status;
  BOOL draw;

  // 人物
  const MYSTATUS* cp_mystatus;
  FIELD_PLAYER* p_player;
  MMDL* p_player_core;
  
} EV_CIRCLEWALK_HERO;

//-------------------------------------
///	DRAW_TASKワーク
//=====================================
typedef struct 
{
  EV_CIRCLEWALK_HERO* p_hero;
} EV_CIRCLEWALK_DRAWWK;


//-------------------------------------
///	イベントワーク
//=====================================
typedef struct 
{
  FIELDMAP_WORK* p_fieldmap;
  GAMESYS_WORK* p_gsys;

  // 四天王ナンバー
  u32 fourkings_no;
  
  // カメラ動作
  EV_CIRCLEWALK_CAMERA camera;

  // 自機のダミー動作
  EV_CIRCLEWALK_HERO hero;

  // 描画タスク
  FLDMAPFUNC_WORK* p_drawtask;

  // VBlank TCB
  GFL_TCB * p_vintr;
} EV_CIRCLEWALK_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


static void VBLANK_Update( GFL_TCB * p_tcb, void* p_work );

//-------------------------------------
///	描画タスク
//=====================================
static void DRAW_Update( FLDMAPFUNC_WORK* p_taskwork, FIELDMAP_WORK* p_fieldmap , void* p_work );
static const FLDMAPFUNC_DATA sc_DRAWTASK_DATA = 
{
  0,
  sizeof(EV_CIRCLEWALK_DRAWWK),
  NULL,
  NULL,
  NULL,
  DRAW_Update,
};


//-------------------------------------
///	カメラ動作
//=====================================
static void EV_CAMERA_Init( EV_CIRCLEWALK_CAMERA* p_wk, FIELD_CAMERA* p_camera, HEAPID heapID, u32 fourkings_no );
static void EV_CAMERA_Exit( EV_CIRCLEWALK_CAMERA* p_wk );
static BOOL EV_CAMERA_Update( EV_CIRCLEWALK_CAMERA* p_wk );
static void EV_CAMERA_SetUpParam( EV_CIRCLEWALK_CAMERA* p_wk );


//-------------------------------------
/// 人物動作動作
//=====================================
static void EV_HERO_Init0( EV_CIRCLEWALK_HERO* p_wk, FIELD_PLAYER * p_fld_player, const MYSTATUS* cp_mystatus, HEAPID heapID, u32 fourkings_no );
static void EV_HERO_Init1( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Exit( EV_CIRCLEWALK_HERO* p_wk );
static BOOL EV_HERO_Update( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Switch( EV_CIRCLEWALK_HERO* p_wk, BOOL sw );
static void EV_HERO_VBlank( EV_CIRCLEWALK_HERO* p_wk );
static void EV_HERO_Draw( EV_CIRCLEWALK_HERO* p_wk );


//-------------------------------------
///	イベント生成関数・イベント関数
//=====================================
static GMEVENT* EVENT_SetUp_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkings_no );
static GMEVENT_RESULT EVENT_CircleWalk( GMEVENT* p_event, int* p_seq, void* p_wk );


//----------------------------------------------------------------------------
/**
 *	@brief  歩いてあがる演出
 *
 *	@param	p_parent          親のイベント
 *	@param	p_gsys            ゲームデータ
 *	@param	p_fieldmap        フィールドデータ
 *	@param	fourkins_no       四天王ナンバー
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_FourKings_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkins_no )
{
  return EVENT_SetUp_CircleWalk(p_gsys, p_fieldmap, fourkins_no);
}




//-----------------------------------------------------------------------------
/**
 *        private関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  VBLANKタスク
 */
//-----------------------------------------------------------------------------
static void VBLANK_Update( GFL_TCB * p_tcb, void* p_work )
{
  EV_CIRCLEWALK_WORK * p_wk = p_work;
  
  // テクスチャ転送
  EV_HERO_VBlank( &p_wk->hero );
}



//----------------------------------------------------------------------------
/**
 *	@brief  四天王部屋　円形移動デモイベント　生成
 *
 *	@param	p_gsys        ゲームシステム
 *	@param	fieldmap      フィールドマップ
 *
 *	@return イベント
 */
//-----------------------------------------------------------------------------
static GMEVENT* EVENT_SetUp_CircleWalk( GAMESYS_WORK* p_gsys, FIELDMAP_WORK* p_fieldmap, u32 fourkings_no )
{
  GMEVENT*              p_event;
  EV_CIRCLEWALK_WORK*   p_work;
  HEAPID heapID = FIELDMAP_GetHeapID( p_fieldmap );

  // イベント生成
  p_event = GMEVENT_Create( p_gsys, NULL, EVENT_CircleWalk, sizeof( EV_CIRCLEWALK_WORK ) );

  // イベントワークを初期化
  p_work              = GMEVENT_GetEventWork( p_event );
  p_work->p_fieldmap  = p_fieldmap;
  p_work->p_gsys      = p_gsys;
  p_work->fourkings_no= fourkings_no;


  // VBlankタスク登録
  p_work->p_vintr = GFUser_VIntr_CreateTCB( VBLANK_Update, p_work, 0 );


  // 生成したイベントを返す
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief  円形移動デモ　イベント
 *
 *	@param	p_event     イベントワーク
 *	@param	p_seq       シーケンス
 *	@param	p_wk        ワーク
 *
 *  @retval GMEVENT_RES_CONTINUE = 0, ///<イベント継続中
 *  @retval GMEVENT_RES_FINISH,       ///<イベント終了
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_CircleWalk( GMEVENT* p_event, int* p_seq, void* p_wk )
{
  EV_CIRCLEWALK_WORK*   p_work = p_wk;
  HEAPID heapID = FIELDMAP_GetHeapID( p_work->p_fieldmap );

  switch( *p_seq )
  {
  case SEQ_INIT00:
    {
      GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_work->p_gsys );
      MYSTATUS* p_mystatus = GAMEDATA_GetMyStatus( p_gdata );
      FIELD_PLAYER* p_player = FIELDMAP_GetFieldPlayer( p_work->p_fieldmap );

      EV_HERO_Init0( &p_work->hero, p_player, p_mystatus, heapID, p_work->fourkings_no );
    }
    (*p_seq)++;
    break;

  case SEQ_INIT01:
    {
      FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->p_fieldmap ); 
      EV_CAMERA_Init( &p_work->camera, p_camera, heapID, p_work->fourkings_no );
    }
    EV_HERO_Init1( &p_work->hero );

    // 描画開始
    {
      EV_CIRCLEWALK_DRAWWK* p_drawwk;
      p_work->p_drawtask = FLDMAPFUNC_Create( FIELDMAP_GetFldmapFuncSys(p_work->p_fieldmap), &sc_DRAWTASK_DATA );
      p_drawwk = FLDMAPFUNC_GetFreeWork( p_work->p_drawtask );
      p_drawwk->p_hero = &p_work->hero;
    }

    // 描画スイッチ
    EV_HERO_Switch( &p_work->hero, TRUE );
    
    (*p_seq)++;
    break;

  case SEQ_UPDATE:
    {
      BOOL result1;
      BOOL result2;

      result1 = EV_CAMERA_Update( &p_work->camera );
      result2 = EV_HERO_Update( &p_work->hero );
      GF_ASSERT( result1 == result2 );

      if( result1 )
      {
        (*p_seq) ++;
      }
    }
    break;

  case SEQ_EXIT:
    // 描画スイッチ
    EV_HERO_Switch( &p_work->hero, FALSE );

    // 描画終了
    FLDMAPFUNC_Delete( p_work->p_drawtask );

    // TCB破棄
    GFL_TCB_DeleteTask( p_work->p_vintr );
    
    // 各デモ破棄
    EV_HERO_Exit( &p_work->hero );
    EV_CAMERA_Exit( &p_work->camera );
    return GMEVENT_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  カメラワーク  初期化
 *
 *	@param	p_wk        ワーク
 *	@param	p_camera    カメラ
 *	@param	heapID      ヒープＩＤ
 *	@param  fourkins_no 四天王ナンバー
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_Init( EV_CIRCLEWALK_CAMERA* p_wk, FIELD_CAMERA* p_camera, HEAPID heapID, u32 fourkings_no )
{
  p_wk->frame = 0;
  
  // アニメーション読み込み
  p_wk->p_anime = ICA_ANIME_CreateStreamingAlloc( heapID, ARCID_FOURKINGS_SCENE, EV_CAMERA_ANIME_GETDATA_ID(fourkings_no), EV_CAMERA_ANIME_STREAMING_INTERVAL );

  // カメラ作成
  {
    VecFx32 target, pos;
    const GFL_G3D_CAMERA* cp_camera = FIELD_CAMERA_GetCameraPtr( p_camera );
    GFL_G3D_CAMERA_GetTarget( cp_camera, &target );
    GFL_G3D_CAMERA_GetPos( cp_camera, &pos );
    p_wk->p_dummy_camera = GFL_G3D_CAMERA_CreateDefault( &pos, &target, heapID );
  }

  // データ反映先フィールドカメラ
  p_wk->p_camera = p_camera;
  // 直地指定モード
  p_wk->camera_mode_save = FIELD_CAMERA_GetMode( p_wk->p_camera );
  FIELD_CAMERA_ChangeMode( p_wk->p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  // カメラエリア設定ＯＦＦ
  p_wk->camera_area_save = FIELD_CAMERA_GetCameraAreaActive( p_wk->p_camera );
  FIELD_CAMERA_SetCameraAreaActive( p_wk->p_camera, FALSE );

  // ターゲットのバインドをOFF
  FIELD_CAMERA_FreeTarget( p_wk->p_camera );

  // カメラ座標のバインドがないかチェック
  GF_ASSERT( FIELD_CAMERA_IsBindCamera(p_wk->p_camera) == FALSE );

  // ターゲットオフセットをセーブ
  {
    VecFx32 clear = {0,0,0};
    FIELD_CAMERA_GetTargetOffset( p_wk->p_camera, &p_wk->camera_target_offset_save );
    FIELD_CAMERA_SetTargetOffset( p_wk->p_camera, &clear );
  }
  FIELD_CAMERA_GetCameraUp( p_wk->p_camera, &p_wk->camera_up_save );

  // 初期パラメータ設定
  EV_CAMERA_SetUpParam( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラワーク  破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_Exit( EV_CIRCLEWALK_CAMERA* p_wk )
{
  // ターゲットのバインドをON
  FIELD_CAMERA_BindDefaultTarget( p_wk->p_camera );
  FIELD_CAMERA_SetTargetPos( p_wk->p_camera, FIELD_CAMERA_GetWatchTarget(p_wk->p_camera) );
  
  // カメラを元に戻す
  FIELD_CAMERA_SetTargetOffset( p_wk->p_camera, &p_wk->camera_target_offset_save );
  FIELD_CAMERA_ChangeMode( p_wk->p_camera, p_wk->camera_mode_save );
  FIELD_CAMERA_SetCameraAreaActive( p_wk->p_camera, p_wk->camera_area_save );
  FIELD_CAMERA_SetCameraUp( p_wk->p_camera, &p_wk->camera_up_save );

  // 最後にZONEの初期値に戻す
  FIELD_CAMERA_SetDefaultParameter( p_wk->p_camera );

  // ダミーカメラ破棄
  GFL_G3D_CAMERA_Delete( p_wk->p_dummy_camera );

  // アニメーション破棄
  ICA_ANIME_Delete( p_wk->p_anime );

  // ワーククリア
  GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_CAMERA) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラ  アップデート
 *
 *	@param	p_wk  ワーク
 *
 *	@retval TRUE    カメラアニメーション完了
 *	@retval FALSE   カメラアニメーション途中
 */
//-----------------------------------------------------------------------------
static BOOL EV_CAMERA_Update( EV_CIRCLEWALK_CAMERA* p_wk )
{
  BOOL result;

  // すでに完了していないか？
  if( (ICA_ANIME_GetMaxFrame( p_wk->p_anime )<<FX32_SHIFT) <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = (ICA_ANIME_GetMaxFrame( p_wk->p_anime )<<FX32_SHIFT) - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += EV_CAMERA_ANIME_SPEED;
  }

  // アニメーションを進め、カメラ座標を設定
  ICA_ANIME_SetAnimeFrame( p_wk->p_anime, p_wk->frame );

  EV_CAMERA_SetUpParam( p_wk );
  
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  パラメータ設定
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_CAMERA_SetUpParam( EV_CIRCLEWALK_CAMERA* p_wk )
{
  VecFx32 target, pos, up;
  
  // カメラパラメータを取得
  ICA_CAMERA_SetCameraStatus( p_wk->p_dummy_camera, p_wk->p_anime );
  GFL_G3D_CAMERA_GetPos( p_wk->p_dummy_camera, &pos ); 
  GFL_G3D_CAMERA_GetTarget( p_wk->p_dummy_camera, &target ); 
  GFL_G3D_CAMERA_GetCamUp( p_wk->p_dummy_camera, &up );

  // 中心分平行移動
  VEC_Add( &pos, &EV_DEMO_CenterVec, &pos );
  VEC_Add( &target, &EV_DEMO_CenterVec, &target );

  TOMOYA_Printf( "pos x[0x%x] y[0x%x] z[0x%x]\n", pos.x, pos.y, pos.z );

  // 座標をフィールドカメラに設定
  FIELD_CAMERA_SetCameraPos( p_wk->p_camera, &pos ); 
  FIELD_CAMERA_SetTargetPos( p_wk->p_camera, &target ); 
  FIELD_CAMERA_SetCameraUp( p_wk->p_camera, &up ); 
}


//----------------------------------------------------------------------------
/**
 *	@brief  自機ダミー  初期化
 *
 *	@param	p_wk          ワーク
 *	@param	cp_mystatus   自機情報
 *	@param	heapID        ヒープＩＤ
 *	@param	fourkings_no  四天王ナンバー
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Init0( EV_CIRCLEWALK_HERO* p_wk, FIELD_PLAYER * p_fld_player, const MYSTATUS* cp_mystatus, HEAPID heapID, u32 fourkings_no )
{
  u32 sex;
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_FOURKINGS_SCENE, GFL_HEAP_LOWID(heapID) );
  BOOL result;

  // 最初は非表示
  p_wk->draw = FALSE;
  
  //　保存
  p_wk->cp_mystatus   = cp_mystatus;
  p_wk->p_player      = p_fld_player;
  p_wk->p_player_core = FIELD_PLAYER_GetMMdl( p_wk->p_player );

  // 座標
  p_wk->status.trans = EV_DEMO_CenterVec;
  VEC_Set( &p_wk->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &p_wk->status.rotate );

  // 性別取得
  sex = MyStatus_GetMySex( cp_mystatus );
  GF_ASSERT( sex != PM_NEUTRAL );
  
  // ３Ｄモデルの読み込み
  if( sex == PM_MALE )
  {
    p_wk->p_mdlres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_M_GETDATA_ID(fourkings_no) );
  }
  else
  {
    p_wk->p_mdlres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_F_GETDATA_ID(fourkings_no) );
  }
  p_wk->p_anmres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_ANIME_GETDATA_ID(fourkings_no) );

  // VRAMKEYの取得
  result = GFL_G3D_AllocVramTexture( p_wk->p_mdlres );
  GF_ASSERT( result );
  p_wk->trans = TRUE;

  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ヒーロー初期化　2フレーム目
 *
 *	@param	p_wk          ワーク
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Init1( EV_CIRCLEWALK_HERO* p_wk )
{
  // レンダーオブジェ生成
  p_wk->p_rend = GFL_G3D_RENDER_Create( p_wk->p_mdlres, 0, p_wk->p_mdlres );

  // アニメーションオブジェ生成
  p_wk->p_anm[0] = GFL_G3D_ANIME_Create( p_wk->p_rend, p_wk->p_anmres, 0 );

  // 表示オブジェ生成
  p_wk->p_obj = GFL_G3D_OBJECT_Create( p_wk->p_rend, p_wk->p_anm, 1 );

  {
    int frame = 0;
    GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, 0, &frame );
    GFL_G3D_OBJECT_EnableAnime( p_wk->p_obj, 0 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  自機ダミー　破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Exit( EV_CIRCLEWALK_HERO* p_wk )
{
  // 全部破棄
  GFL_G3D_OBJECT_Delete( p_wk->p_obj );
  GFL_G3D_ANIME_Delete( p_wk->p_anm[0] );
  GFL_G3D_RENDER_Delete( p_wk->p_rend );

  //リソース
  GFL_G3D_FreeVramTexture( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_anmres );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自機ダミー　デモ更新
 *
 *	@param	p_wk  ワーク
 *
 *	@retval TRUE    デモ完了
 *	@retval FALSE   デモ途中
 */
//-----------------------------------------------------------------------------
static BOOL EV_HERO_Update( EV_CIRCLEWALK_HERO* p_wk )
{
  BOOL result;

  //　アニメーションをすすめる
  result = GFL_G3D_OBJECT_IncAnimeFrame( p_wk->p_obj, 0, EV_CAMERA_ANIME_SPEED );

  if( result == FALSE )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ダミーと自機の表示をスイッチ
 *
 *	@param	p_wk  ワーク
 *	@param	sw    スイッチ    TRUE:ダミーを表示
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Switch( EV_CIRCLEWALK_HERO* p_wk, BOOL sw )
{
  if(sw)
  {
    p_wk->draw = TRUE;
    MMDL_SetStatusBitVanish( p_wk->p_player_core, TRUE );
  }
  else
  {
    VecFx32 vecpos;
    
    p_wk->draw = FALSE;
    MMDL_SetStatusBitVanish( p_wk->p_player_core, FALSE );

    MMDL_InitGridPosition(
      p_wk->p_player_core, 
      EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, EV_HERO_WALK_END_GRID_Z, 
      EV_HERO_WALK_END_DIR );
    MMDL_UpdateCurrentHeight( p_wk->p_player_core );

    MMDL_GetVectorPos( p_wk->p_player_core, &vecpos );
    FIELD_PLAYER_SetPos( p_wk->p_player, &vecpos );

  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  自機ダミー　VBlank
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void EV_HERO_VBlank( EV_CIRCLEWALK_HERO* p_wk )
{
  if( p_wk->trans )
  {
    GFL_G3D_TransOnlyTexture( p_wk->p_mdlres );
    p_wk->trans = FALSE;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  主人公の表示
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void EV_HERO_Draw( EV_CIRCLEWALK_HERO* p_wk )
{
  GFL_G3D_DRAW_DrawObject(  p_wk->p_obj, &p_wk->status );
}




//----------------------------------------------------------------------------
/**
 *	@brief  描画アップデート
 *
 *	@param	p_taskwork    タスクワーク
 *	@param	p_fieldmap    フィールドマップ
 *	@param	p_work        ワーク
 */
//-----------------------------------------------------------------------------
static void DRAW_Update( FLDMAPFUNC_WORK* p_taskwork, FIELDMAP_WORK* p_fieldmap , void* p_work )
{
  EV_CIRCLEWALK_DRAWWK* p_wk = p_work; 

  EV_HERO_Draw( p_wk->p_hero );
}


