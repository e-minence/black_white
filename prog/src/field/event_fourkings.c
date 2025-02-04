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

#include "sound/pm_sndsys.h"

#include "savedata/mystatus.h"

#include "arc/arc_def.h"
#include "arc/fourkings_scene.naix"

#include "field/field_const.h"
#include "field/eventwork.h"
#include "eventwork_def.h"

#include "event_fourkings.h"

#include "fieldmap.h"
#include "fldmmdl.h"
#include "field_camera.h"
#include "fieldmap_func.h"

#include "field_fk_sound_anime.h"

#include "field_sound.h"


#ifdef PM_DEBUG

//#define DEBUG_FRAME_CONTROL // フレームのコマ送りなどを行う

#endif

#ifdef DEBUG_FRAME_CONTROL

static BOOL s_DEBUG_FRAME_CONTROL_FLAG = 0;

#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
enum
{
  SEQ_INIT00,
  SEQ_INIT01,
  SEQ_INIT02,
  SEQ_UPDATE,
  SEQ_EXIT,
};


// アニメ
enum
{
  HERO_ANIME_ICA,
  HERO_ANIME_ITP,

  HERO_ANIME_MAX,
};

// ストリーミング再生
#define EV_CAMERA_ANIME_STREAMING_INTERVAL  ( 10 )
#define EV_SE_ANIME_STREAMING_INTERVAL  ( 10 )

// 各リソースＩＤ取得マクロ
#define EV_CAMERA_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_camera_bin + ((x)) )
#define EV_HERO_M_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_m_nsbmd + ((x) * 6) )
#define EV_HERO_F_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_f_nsbmd + ((x) * 6) )
#define EV_HERO_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_mf_nsbca + ((x) * 6) )
#define EV_HERO_M_ITPANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_m_nsbtp + ((x) * 6) )
#define EV_HERO_F_ITPANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_f_nsbtp + ((x) * 6) )
#define EV_SE_ANIME_GETDATA_ID( x )   ( NARC_fourkings_scene_c09_01_se_se + ((x)) )

// アニメーションスピード
#define EV_CAMERA_ANIME_SPEED (FX32_ONE)

// 自機デモ後出現位置
#define EV_HERO_WALK_END_GRID_X (15)
#define EV_HERO_WALK_END_GRID_Y (0)
#define EV_HERO_WALK_END_GRID_Z (16)
static const VecFx32 EV_DEMO_HERO_WALK_END_GRID[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, 13 }, // ゴースト
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, EV_HERO_WALK_END_GRID_Z },  // あく
  { EV_HERO_WALK_END_GRID_X, EV_HERO_WALK_END_GRID_Y, 18 },  // かくとう
  { EV_HERO_WALK_END_GRID_X-1, EV_HERO_WALK_END_GRID_Y, EV_HERO_WALK_END_GRID_Z }, // エスパーだけ１つずれる
};

// 出てくる方向
static const u16 EV_DEMO_HERO_WALK_END_DIR[] = 
{
  DIR_UP,
  DIR_RIGHT,
  DIR_UP,
  DIR_UP,
};

// 演出の中心
//static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(10) };
static const VecFx32 EV_DEMO_CenterVec = { GRID_TO_FX32(16), 0, GRID_TO_FX32(16) };



// 配置アニメーションの有無
static const BOOL EV_DEMO_HAICHI_ANIME[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  FALSE,
  FALSE,
  TRUE,
  TRUE,
};
static const VecFx32 EV_DEMO_HAICHI_ANIME_POS[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  {0,0,0},
  {0,0,0},
  {GRID_TO_FX32(24), 0, GRID_TO_FX32(16)},
  {GRID_TO_FX32(16), 0, GRID_TO_FX32(16)},
};


// SEアニメーションの有無
static const BOOL EV_DEMO_SE_ANIME[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  TRUE,
  TRUE,
  TRUE,
  TRUE,
};
static const FIELD_FK_SOUND_ANIME_DATA EV_DEMO_SE_DATA[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  // 
  {
    {
      SEQ_SE_FLD_113, // お化け画面に出現
      SEQ_SE_FLD_114, // 取り付くお化け
      SEQ_SE_FLD_115, // お化け解散
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },

  // 
  {
    {
      SEQ_SE_FLD_104, // ろうそく点灯
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },
  
  // 
  {
    {
      SEQ_SE_FLD_105, // 台車上りLOOP
      SEQ_SE_FLD_106, // スチーム
      SEQ_SE_FLD_117, // 台車に乗った
      SEQ_SE_FLD_118, // 到着音
      SEQ_SE_FLD_119, // ゲートが開く
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },

  // 
  {
    {
      SEQ_SE_FLD_110, // 光
      SEQ_SE_FLD_108, // 流れ星
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
      FIELD_FK_SOUND_ANIME_SE_NONE,
    },
  },
};


// 頂上カメラID
static const u32 sc_EV_DEMO_CAMERA_ID_TBL[ FIELD_EVENT_FOURKINGS_MAX ] = 
{
  28,
  29,
  30,
  31
};

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

  u32 camera_type;
} EV_CIRCLEWALK_CAMERA;

//-------------------------------------
///	自機ダミー動作
//=====================================
typedef struct {
  // リソース
  GFL_G3D_RES* p_mdlres;
  GFL_G3D_RES* p_anmres[HERO_ANIME_MAX];
  BOOL trans;

  // 管理オブジェ
  GFL_G3D_RND* p_rend;
  GFL_G3D_ANM* p_anm[HERO_ANIME_MAX];
  GFL_G3D_OBJ* p_obj;
  GFL_G3D_OBJSTATUS status;
  BOOL draw;

  // 人物
  const MYSTATUS* cp_mystatus;
  FIELD_PLAYER* p_player;
  MMDL* p_player_core;
  VecFx32 end_gridpos;
  u16 end_dir;
  
  int frame;
} EV_CIRCLEWALK_HERO;

//-------------------------------------
///	SE動作
//=====================================
typedef struct {
  FIELD_FK_SOUND_ANIME* p_anime;
} EV_CIRCLEWALK_SE;



//-------------------------------------
///	配置アニメーションワーク
//=====================================
typedef struct {
  
  // 
  FIELD_BMODEL* p_model;
  G3DMAPOBJST * p_objst;

  FIELD_BMODEL_MAN* p_modelman;

  
} EV_BMODEL_ANIME;


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

  // モデルアニメーション
  EV_BMODEL_ANIME bmodel;

  // SE
  EV_CIRCLEWALK_SE  se;

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

static fx32 G3DANM_GetFrameMax( const GFL_G3D_ANM* cp_anm );

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


//-------------------------------------
/// 配置オブジェのアニメーション
//=====================================
static void EV_BMODEL_Init( EV_BMODEL_ANIME* p_wk, FIELD_BMODEL_MAN * bmodel_man, u32 fourkings_no );
static void EV_BMODEL_Exit( EV_BMODEL_ANIME* p_wk );
static void EV_BMODEL_Start( EV_BMODEL_ANIME* p_wk );
static BOOL EV_BMODEL_IsEnd( const EV_BMODEL_ANIME* cp_wk );


//-------------------------------------
/// SEアニメーション
//=====================================
static void EV_SE_ANIME_Init( EV_CIRCLEWALK_SE* p_wk, u32 fourkings_no, HEAPID heapID );
static void EV_SE_ANIME_Exit( EV_CIRCLEWALK_SE* p_wk );
static BOOL EV_SE_ANIME_Update( EV_CIRCLEWALK_SE* p_wk );


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
  GF_ASSERT( FIELD_EVENT_FOURKINGS_MAX > fourkins_no );
  return EVENT_SetUp_CircleWalk(p_gsys, p_fieldmap, fourkins_no);
}



//----------------------------------------------------------------------------
/**
 *	@brief  四天王かく部屋の頂上カメラIDを取得
 *
 *	@param	fourkins_no   四天王ナンバー
 *
 *	@return カメラID
 */
//-----------------------------------------------------------------------------
u32 EVENT_FourKings_GetCameraID( u32 fourkins_no )
{
  GF_ASSERT( fourkins_no < FIELD_EVENT_FOURKINGS_MAX );
  return sc_EV_DEMO_CAMERA_ID_TBL[ fourkins_no ];
}



//-----------------------------------------------------------------------------
/**
 *        private関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションのフレーム最大数を取得する
 *
 *	@param	cp_anm  アニメーションオブジェ
 *
 *	@return フレーム最大数
 */
//-----------------------------------------------------------------------------
static fx32 G3DANM_GetFrameMax( const GFL_G3D_ANM* cp_anm )
{
  const NNSG3dAnmObj* cp_anm_core = GFL_G3D_ANIME_GetAnmObj( cp_anm ); 
  return NNS_G3dAnmObjGetNumFrame( cp_anm_core );
}


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
    // メモリ残量の表示
    OS_Printf( "init start heap free size 0x%x\n", GFI_HEAP_GetHeapFreeSize(heapID) );
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
      FIELD_CAMERA_StopTraceRequest( p_camera );
    }
    (*p_seq)++;
    break;

  case SEQ_INIT02:
    {
      FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( p_work->p_fieldmap ); 

      if( FIELD_CAMERA_CheckTrace(p_camera) )
      {
        break;
      }

      EV_CAMERA_Init( &p_work->camera, p_camera, heapID, p_work->fourkings_no );
      
    }

    // SE
    {
      EV_SE_ANIME_Init( &p_work->se, p_work->fourkings_no, heapID );
    }
    
    // 配置モデル
    {
      FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_work->p_fieldmap );
      FIELD_BMODEL_MAN* p_bmodelman = FLDMAPPER_GetBuildModelManager( p_mapper );

      EV_BMODEL_Init( &p_work->bmodel, p_bmodelman, p_work->fourkings_no );
    }
    EV_HERO_Init1( &p_work->hero );

    // 描画開始
    {
      EV_CIRCLEWALK_DRAWWK* p_drawwk;
      p_work->p_drawtask = FLDMAPFUNC_Create( NO_OVERLAY_ID, FIELDMAP_GetFldmapFuncSys(p_work->p_fieldmap), &sc_DRAWTASK_DATA );
      p_drawwk = FLDMAPFUNC_GetFreeWork( p_work->p_drawtask );
      p_drawwk->p_hero = &p_work->hero;
    }

    // 描画スイッチ
    EV_HERO_Switch( &p_work->hero, TRUE );

    // 配置アニメーション開始
    EV_BMODEL_Start( &p_work->bmodel ); 
    
    // メモリ残量の表示
    OS_Printf( "init end heap free size 0x%x\n", GFI_HEAP_GetHeapFreeSize(heapID) );
    
    (*p_seq)++;
    break;

  case SEQ_UPDATE:
    {
      BOOL result1;
      BOOL result2;
      BOOL result3;
      BOOL result4;

#ifdef DEBUG_FRAME_CONTROL
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
      {
        s_DEBUG_FRAME_CONTROL_FLAG ^= TRUE;
      }
#endif

      result1 = EV_CAMERA_Update( &p_work->camera );
      result2 = EV_HERO_Update( &p_work->hero );
      result4 = EV_SE_ANIME_Update( &p_work->se );
      GF_ASSERT( (result1 == result2) );

      result3 = EV_BMODEL_IsEnd( &p_work->bmodel );

      if( result1 && result3 && result4 )
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
    EV_SE_ANIME_Exit( &p_work->se );
    EV_CAMERA_Exit( &p_work->camera );
    EV_BMODEL_Exit( &p_work->bmodel );
    return GMEVENT_RES_FINISH;

  default:
    GF_ASSERT(0);
    break;
  }

  return GMEVENT_RES_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  配置モデルのアニメーション初期化
 *
 *	@param	p_wk              ワーク
 *	@param	bmodel_man        配置管理システム
 *	@param	fourkings_no      四天王ナンバー
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Init( EV_BMODEL_ANIME* p_wk, FIELD_BMODEL_MAN * bmodel_man, u32 fourkings_no )
{
  if( EV_DEMO_HAICHI_ANIME[ fourkings_no ] )
  {
    p_wk->p_model = FIELD_BMODEL_Create_Search( bmodel_man, BM_SEARCH_ID_NULL, &EV_DEMO_HAICHI_ANIME_POS[fourkings_no] );
    p_wk->p_objst = FIELD_BMODEL_MAN_SearchObjStatusPos( bmodel_man, BM_SEARCH_ID_NULL, &EV_DEMO_HAICHI_ANIME_POS[fourkings_no] );
    p_wk->p_modelman = bmodel_man;
  }
  else
  {
    p_wk->p_model = NULL;
    p_wk->p_objst = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  破棄
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Exit( EV_BMODEL_ANIME* p_wk )
{
  if( p_wk->p_model )
  {
    FIELD_BMODEL_Delete( p_wk->p_model );

    // 大本も動かしておく
    G3DMAPOBJST_setAnime( p_wk->p_modelman, p_wk->p_objst, 0, BMANM_REQ_REVERSE_START );
    G3DMAPOBJST_setAnime( p_wk->p_modelman, p_wk->p_objst, 0, BMANM_REQ_STOP );
    
    p_wk->p_model = NULL;
    p_wk->p_objst = NULL;
    p_wk->p_modelman = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションの開始
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void EV_BMODEL_Start( EV_BMODEL_ANIME* p_wk )
{
  if( p_wk->p_model )
  {
    FIELD_BMODEL_SetAnime( p_wk->p_model, 0, BMANM_REQ_START );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  アニメーションの完了街
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE  完了
 *	@retval FALSE 途中
 */
//-----------------------------------------------------------------------------
static BOOL EV_BMODEL_IsEnd( const EV_BMODEL_ANIME* cp_wk )
{
  if( cp_wk->p_model )
  {
    return FIELD_BMODEL_WaitAnime( cp_wk->p_model, 0 );
  }
  return TRUE;
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

  // 復帰カメラタイプ
  p_wk->camera_type = sc_EV_DEMO_CAMERA_ID_TBL[ fourkings_no ];
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

  // 頂上カメラを指定
  FIELD_CAMERA_SetCameraType( p_wk->p_camera, p_wk->camera_type );

  // トレース情報の初期化
  FIELD_CAMERA_RestartTrace( p_wk->p_camera );

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
  fx32 max_frame;

  max_frame = ICA_ANIME_GetMaxFrame( p_wk->p_anime )<<FX32_SHIFT;

#ifndef DEBUG_FRAME_CONTROL
  // すでに完了していないか？
  if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = max_frame - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += EV_CAMERA_ANIME_SPEED;
  }
#else

  if( s_DEBUG_FRAME_CONTROL_FLAG == FALSE )
  {
    // すでに完了していないか？
    if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
    {
      result = TRUE;
      p_wk->frame = max_frame - FX32_ONE;
    }
    else
    {
      result = FALSE;
      p_wk->frame += EV_CAMERA_ANIME_SPEED;
    }
  }
  else
  {
    result = FALSE;
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
    {
      if( (p_wk->frame - EV_CAMERA_ANIME_SPEED) >= 0 )
      {
        p_wk->frame -= EV_CAMERA_ANIME_SPEED;
      }
    }
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
    {
      if( (p_wk->frame + EV_CAMERA_ANIME_SPEED) < max_frame )
      {
        p_wk->frame += EV_CAMERA_ANIME_SPEED;
      }
    }
  }
#endif


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

  // フレームの初期化
  p_wk->frame = 0;

  // 最初は非表示
  p_wk->draw = FALSE;

  //  終了後出現位置
  p_wk->end_gridpos = EV_DEMO_HERO_WALK_END_GRID[ fourkings_no ];
  p_wk->end_dir = EV_DEMO_HERO_WALK_END_DIR[ fourkings_no ];
  
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
    p_wk->p_anmres[HERO_ANIME_ITP] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_M_ITPANIME_GETDATA_ID(fourkings_no) );
  }
  else
  {
    p_wk->p_mdlres = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_F_GETDATA_ID(fourkings_no) );
    p_wk->p_anmres[HERO_ANIME_ITP] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_F_ITPANIME_GETDATA_ID(fourkings_no) );
  }
  p_wk->p_anmres[HERO_ANIME_ICA] = GFL_G3D_CreateResourceHandle( p_handle, EV_HERO_ANIME_GETDATA_ID(fourkings_no) );


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
  p_wk->p_anm[HERO_ANIME_ICA] = GFL_G3D_ANIME_Create( p_wk->p_rend, p_wk->p_anmres[HERO_ANIME_ICA], 0 );
  p_wk->p_anm[HERO_ANIME_ITP] = GFL_G3D_ANIME_Create( p_wk->p_rend, p_wk->p_anmres[HERO_ANIME_ITP], 0 );

  // 表示オブジェ生成
  p_wk->p_obj = GFL_G3D_OBJECT_Create( p_wk->p_rend, p_wk->p_anm, HERO_ANIME_MAX );

  {
    int frame = 0;
    int i;
    for( i=0; i<HERO_ANIME_MAX; i++ )
    {
      GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, i, &frame );
      GFL_G3D_OBJECT_EnableAnime( p_wk->p_obj, i );
    }
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
  GFL_G3D_ANIME_Delete( p_wk->p_anm[HERO_ANIME_ICA] );
  GFL_G3D_ANIME_Delete( p_wk->p_anm[HERO_ANIME_ITP] );
  GFL_G3D_RENDER_Delete( p_wk->p_rend );

  //リソース
  GFL_G3D_FreeVramTexture( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_mdlres );
  GFL_G3D_DeleteResource( p_wk->p_anmres[HERO_ANIME_ICA] );
  GFL_G3D_DeleteResource( p_wk->p_anmres[HERO_ANIME_ITP] );
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
  fx32 max_frame;

  max_frame = G3DANM_GetFrameMax( p_wk->p_anm[HERO_ANIME_ICA] );

#ifndef DEBUG_FRAME_CONTROL
  // すでに完了していないか？
  if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
  {
    result = TRUE;
    p_wk->frame = max_frame - FX32_ONE;
  }
  else
  {
    result = FALSE;
    p_wk->frame += EV_CAMERA_ANIME_SPEED;
  }
#else

  if( s_DEBUG_FRAME_CONTROL_FLAG == FALSE )
  {
    // すでに完了していないか？
    if( max_frame <= (p_wk->frame + EV_CAMERA_ANIME_SPEED) )
    {
      result = TRUE;
      p_wk->frame = max_frame - FX32_ONE;
    }
    else
    {
      result = FALSE;
      p_wk->frame += EV_CAMERA_ANIME_SPEED;
    }
  }
  else
  {
    result = FALSE;
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
    {
      if( (p_wk->frame - EV_CAMERA_ANIME_SPEED) >= 0 )
      {
        p_wk->frame -= EV_CAMERA_ANIME_SPEED;
      }
    }
    if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
    {
      if( (p_wk->frame + EV_CAMERA_ANIME_SPEED) < max_frame )
      {
        p_wk->frame += EV_CAMERA_ANIME_SPEED;
      }
    }
  }
#endif

  //　アニメーションをすすめる
  GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, HERO_ANIME_ICA, &p_wk->frame );
  GFL_G3D_OBJECT_SetAnimeFrame( p_wk->p_obj, HERO_ANIME_ITP, &p_wk->frame );

  return result;
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
      p_wk->end_gridpos.x, p_wk->end_gridpos.y, p_wk->end_gridpos.z, 
      p_wk->end_dir );

    MMDL_UpdateCurrentHeight( p_wk->p_player_core );

    MMDL_GetVectorPos( p_wk->p_player_core, &vecpos );
    FIELD_PLAYER_SetPos( p_wk->p_player, &vecpos );

    // 写りこみOFF
    MMDL_SetMoveBitReflect( p_wk->p_player_core, FALSE );
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



//----------------------------------------------------------------------------
/**
 *	@brief  SE再生アニメーションの初期化
 *
 *	@param	p_wk            ワーク
 *	@param	fourkings_no    四天王ID
 *	@param	heapID          ヒープID
 */
//-----------------------------------------------------------------------------
static void EV_SE_ANIME_Init( EV_CIRCLEWALK_SE* p_wk, u32 fourkings_no, HEAPID heapID )
{
  if( EV_DEMO_SE_ANIME[fourkings_no] )
  {
    // アニメーション読み込み
    p_wk->p_anime = FIELD_FK_SOUND_ANIME_Create( &EV_DEMO_SE_DATA[ fourkings_no ],ARCID_FOURKINGS_SCENE, EV_SE_ANIME_GETDATA_ID(fourkings_no), EV_SE_ANIME_STREAMING_INTERVAL, heapID );
  }
  else
  {
    GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_SE) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE再生アニメーションの破棄
 *
 *	@param	p_wk        ワーク
 */ 
//-----------------------------------------------------------------------------
static void EV_SE_ANIME_Exit( EV_CIRCLEWALK_SE* p_wk )
{
  if( p_wk->p_anime )
  {
    FIELD_FK_SOUND_ANIME_Delete( p_wk->p_anime );
  }
  GFL_STD_MemClear( p_wk, sizeof(EV_CIRCLEWALK_SE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  SE再生アニメーションのアップデート
 *
 *	@param	p_wk  ワーク
 *
 *	@retval TRUE  完了
 *	@retval FALSE 再生中
 */
//-----------------------------------------------------------------------------
static BOOL EV_SE_ANIME_Update( EV_CIRCLEWALK_SE* p_wk )
{
  BOOL result;

  result = TRUE;
  if( p_wk->p_anime )
  {
    result = FIELD_FK_SOUND_ANIME_Update( p_wk->p_anime );  
  }
  return result;
}








//-----------------------------------------------------------------------------
/**
 *      雷音用のシステム
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define GHOST_SPARK_SOUND_ANM_INDEX (1)

// 位置
static const VecFx32 sc_GHOST_SPARK_SARCH_POS = 
{
  FX32_CONST(256), 0, FX32_CONST(240)
};

// 音のタイミング
//static fx32 sc_GHOST_SPARK_SOUND_TIMING[] = {
static const fx32 sc_GHOST_SPARK_SOUND_TIMING[] = {
  FX32_CONST(6), 
};

#define GHOST_SPARK_SOUND_TIMING_TBL_MAX  (NELEMS(sc_GHOST_SPARK_SOUND_TIMING))


// サウンドID
#define GHOST_SND_SE_SPARK      ( SEQ_SE_FLD_33 )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	SPARK SOUND
//=====================================
typedef struct 
{
  FIELD_BMODEL_MAN* p_man;
  const G3DMAPOBJST* cp_spark_obj;

  fx32 next_frame;
  int tbl_index;
} GHOST_SPARK_SOUND;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static void GHOST_SPARK_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void GHOST_SPARK_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void GHOST_SPARK_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


const FLDMAPFUNC_DATA c_FLDMAPFUNC_GHOST_SPARK_SOUND = 
{
  128,
  sizeof(GHOST_SPARK_SOUND),
  GHOST_SPARK_SOUND_Create,
  GHOST_SPARK_SOUND_Delete,
  GHOST_SPARK_SOUND_Update,
  NULL,
};




//----------------------------------------------------------------------------
/**
 *	@brief  ゴースト　雷音　管理
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  GHOST_SPARK_SOUND* p_wk = p_work;
  FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
  FIELD_BMODEL_MAN* p_model_man = FLDMAPPER_GetBuildModelManager( p_mapper );
  fx32 start_frame;
  int  i;

  // 動作配置オブジェ
  p_wk->cp_spark_obj  = FIELD_BMODEL_MAN_SearchObjStatusPos( p_model_man, BM_SEARCH_ID_NULL, &sc_GHOST_SPARK_SARCH_POS );
  p_wk->p_man         = p_model_man;

  GF_ASSERT(p_wk->cp_spark_obj);
  GF_ASSERT(p_wk->p_man);
  
  start_frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_spark_obj, GHOST_SPARK_SOUND_ANM_INDEX );
  
  // 開始時の音だしフレームを設定
  p_wk->next_frame  = sc_GHOST_SPARK_SOUND_TIMING[0];
  p_wk->tbl_index   = 0;
  for( i=0; i<GHOST_SPARK_SOUND_TIMING_TBL_MAX; i++ )
  {
    if( start_frame < sc_GHOST_SPARK_SOUND_TIMING[i] )
    {
      p_wk->next_frame  = sc_GHOST_SPARK_SOUND_TIMING[i];
      p_wk->tbl_index   = i;
      break;
    }
  }

  // ループ再生開始
  {
    GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
    GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
    FIELD_SOUND* p_fieldsound = GAMEDATA_GetFieldSound( p_gdata );

    FSND_PlayEnvSE( p_fieldsound, SEQ_SE_FLD_111 );
  }
  //PMSND_PlaySE( SEQ_SE_FLD_112 );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ゴースト　雷音　破棄
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  // ループ停止
  {
    GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
    GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
    FIELD_SOUND* p_fieldsound = GAMEDATA_GetFieldSound( p_gdata );

    FSND_StopEnvSE( p_fieldsound, SEQ_SE_FLD_111 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ゴースト　雷音　更新
 */
//-----------------------------------------------------------------------------
static void GHOST_SPARK_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  GHOST_SPARK_SOUND* p_wk = p_work;
  fx32 now_frame;

  now_frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_spark_obj, GHOST_SPARK_SOUND_ANM_INDEX );

  // 決まったフレームで、音を鳴らす。
  if( p_wk->next_frame == now_frame )
  {
    GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
    GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
    FIELD_SOUND* p_fieldsound = GAMEDATA_GetFieldSound( p_gdata );

    p_wk->tbl_index = (p_wk->tbl_index + 1) % GHOST_SPARK_SOUND_TIMING_TBL_MAX;

    p_wk->next_frame = sc_GHOST_SPARK_SOUND_TIMING[ p_wk->tbl_index ];
    FSND_PlayEnvSE( p_fieldsound, GHOST_SND_SE_SPARK );
  }

  /*
#ifdef PM_DEBUG
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT  )
  {
    sc_GHOST_SPARK_SOUND_TIMING[0] -= FX32_ONE;
    TOMOYA_Printf( "frame %d\n", FX_Whole(sc_GHOST_SPARK_SOUND_TIMING[0]) );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT  )
  {
    sc_GHOST_SPARK_SOUND_TIMING[0] += FX32_ONE;
    TOMOYA_Printf( "frame %d\n", FX_Whole(sc_GHOST_SPARK_SOUND_TIMING[0]) );
  }
#endif
//*/
}




//-----------------------------------------------------------------------------
/**
 *    えすぱー部屋
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	ESPERT SOUND
//=====================================
typedef struct 
{
  FIELD_BMODEL_MAN* p_man;
  const G3DMAPOBJST* cp_obj;
  u16 se_2_on;
  s16 se_2_time;

  FIELD_SOUND* p_fieldsound;
} ESPERT_SOUND;

// エスパー部屋星の位置
static const VecFx32 sc_ESPERT_STAR_SARCH_POS = 
{
  FX32_CONST(118), 0, FX32_CONST(236)
};

#define ESPERT_SE_START_TIMING_MAX  (2)
//static sc_ESPERT_SE_START_TIMING[ ESPERT_SE_START_TIMING_MAX ] = 
static const sc_ESPERT_SE_START_TIMING[ ESPERT_SE_START_TIMING_MAX ] = 
{
  FX32_CONST(90), FX32_CONST(213),
};
#define ESPERT_BMODEL_ANIME_IDX  ( 0 )

#define ESPERT_SE_2_TIMING  ( 31 )
//static ESPERT_SE_2_TIMING = 31;


static void ESPERT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void ESPERT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void ESPERT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


const FLDMAPFUNC_DATA c_FLDMAPFUNC_ESPERT_SOUND = 
{
  128,
  sizeof(ESPERT_SOUND),
  ESPERT_SOUND_Create,
  ESPERT_SOUND_Delete,
  ESPERT_SOUND_Update,
  NULL,
};



//----------------------------------------------------------------------------
/**
 *	@brief  えすぱーSEシステム  生成
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
  FLDMAPPER* p_mapper = FIELDMAP_GetFieldG3Dmapper( p_fieldmap );
  FIELD_BMODEL_MAN* p_model_man = FLDMAPPER_GetBuildModelManager( p_mapper );
  fx32 start_frame;
  int  i;

  // 動作配置オブジェ
  p_wk->cp_obj  = FIELD_BMODEL_MAN_SearchObjStatusPos( p_model_man, BM_SEARCH_ID_NULL, &sc_ESPERT_STAR_SARCH_POS );
  p_wk->p_man         = p_model_man;

  {
    GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
    GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
    p_wk->p_fieldsound = GAMEDATA_GetFieldSound( p_gdata );
  }
  GF_ASSERT(p_wk->cp_obj);
  GF_ASSERT(p_wk->p_man);
  
}


//----------------------------------------------------------------------------
/**
 *	@brief  えすぱーSEシステム  破棄
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  えすぱーSEシステム  更新
 */
//-----------------------------------------------------------------------------
static void ESPERT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  ESPERT_SOUND* p_wk = p_work;
  fx32 frame;
  int i;

  frame = G3DMAPOBJST_getAnimeFrame( p_wk->p_man, p_wk->cp_obj, ESPERT_BMODEL_ANIME_IDX );

  for( i=0; i<ESPERT_SE_START_TIMING_MAX; i++ )
  {
    if(frame == sc_ESPERT_SE_START_TIMING[i])
    {
      FSND_PlayEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_108 );
      p_wk->se_2_on = TRUE;
      p_wk->se_2_time = ESPERT_SE_2_TIMING;
      break;
    }
  }

  // 水滴の音
  if(  p_wk->se_2_on  ){
    p_wk->se_2_time --;
    if( p_wk->se_2_time <= 0 ){
      FSND_PlayEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_130 );
      p_wk->se_2_on = FALSE;
      p_wk->se_2_time = 0;
    }
  }

#if 0
  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    sc_ESPERT_SE_START_TIMING[0] += FX32_HALF;
    TOMOYA_Printf( "start timing 0 0x%x\n", sc_ESPERT_SE_START_TIMING[0] );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_B )
  {
    sc_ESPERT_SE_START_TIMING[0] -= FX32_HALF;
    TOMOYA_Printf( "start timing 0 0x%x\n", sc_ESPERT_SE_START_TIMING[0] );
  }

  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_X )
  {
    sc_ESPERT_SE_START_TIMING[1] += FX32_HALF;
    TOMOYA_Printf( "start timing 1 0x%x\n", sc_ESPERT_SE_START_TIMING[1] );
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_Y )
  {
    sc_ESPERT_SE_START_TIMING[1] -= FX32_HALF;
    TOMOYA_Printf( "start timing 1 0x%x\n", sc_ESPERT_SE_START_TIMING[1] );
  }

  if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_START )
  {
    ESPERT_SE_2_TIMING --;
    TOMOYA_Printf( "se timing %d\n", ESPERT_SE_2_TIMING );
  }else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_SELECT )
  {
    ESPERT_SE_2_TIMING ++;
    TOMOYA_Printf( "se timing %d\n", ESPERT_SE_2_TIMING );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
 *    悪部屋
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	BAD SOUND
//=====================================
typedef struct 
{
  u16*  p_localwk_1;
  u16   se_stop;
  EVENTWORK* p_evwork;
  FIELD_SOUND* p_fieldsound;
} BAD_SOUND;


static void BAD_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void BAD_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void BAD_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


const FLDMAPFUNC_DATA c_FLDMAPFUNC_BAD_SOUND = 
{
  128,
  sizeof(BAD_SOUND),
  BAD_SOUND_Create,
  BAD_SOUND_Delete,
  BAD_SOUND_Update,
  NULL,
};



//----------------------------------------------------------------------------
/**
 *	@brief  悪SEシステム  生成
 */
//-----------------------------------------------------------------------------
static void BAD_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys );
  BAD_SOUND* p_wk = p_work;

  p_wk->p_evwork      = GAMEDATA_GetEventWork( p_gdata );
  p_wk->p_fieldsound  = GAMEDATA_GetFieldSound( p_gdata );
  p_wk->p_localwk_1   = EVENTWORK_GetEventWorkAdrs( p_wk->p_evwork, LOCALWORK1 );
  
  // ループ再生開始
  if( *p_wk->p_localwk_1 == FALSE ){
    FSND_PlayEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_116 );
    p_wk->se_stop = FALSE;
  }else{
    p_wk->se_stop = TRUE;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  悪SEシステム  破棄
 */
//-----------------------------------------------------------------------------
static void BAD_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  BAD_SOUND* p_wk = p_work;

  if( p_wk->se_stop == FALSE ){
    FSND_StopEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_116 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  悪SEシステム  アップデート
 */
//-----------------------------------------------------------------------------
static void BAD_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  BAD_SOUND* p_wk = p_work;

  if( *p_wk->p_localwk_1 ){
    if( p_wk->se_stop == FALSE ){
      FSND_StopEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_116 );
      p_wk->se_stop = TRUE;
    }
  }
}


//-----------------------------------------------------------------------------
/**
 *    格闘部屋
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	FIGHT SOUND
//=====================================
typedef struct 
{
  u16*  p_localwk_1;
  u16   se_stop;
  EVENTWORK* p_evwork;
  FIELD_SOUND* p_fieldsound;
} FIGHT_SOUND;


static void FIGHT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIGHT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );
static void FIGHT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work );


const FLDMAPFUNC_DATA c_FLDMAPFUNC_FIGHT_SOUND = 
{
  128,
  sizeof(FIGHT_SOUND),
  FIGHT_SOUND_Create,
  FIGHT_SOUND_Delete,
  FIGHT_SOUND_Update,
  NULL,
};



//----------------------------------------------------------------------------
/**
 *	@brief  格闘SEシステム  生成
 */
//-----------------------------------------------------------------------------
static void FIGHT_SOUND_Create(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIGHT_SOUND* p_wk = p_work;
  GAMESYS_WORK* p_gsys = FIELDMAP_GetGameSysWork( p_fieldmap );
  GAMEDATA* p_gdata = GAMESYSTEM_GetGameData( p_gsys ); 
  
  p_wk->p_evwork      = GAMEDATA_GetEventWork( p_gdata );
  p_wk->p_fieldsound  = GAMEDATA_GetFieldSound( p_gdata );
  p_wk->p_localwk_1   = EVENTWORK_GetEventWorkAdrs( p_wk->p_evwork, LOCALWORK1 );

  if( *p_wk->p_localwk_1 == FALSE ){
    FSND_PlayEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_174 );
    //TOMOYA_Printf( "EnvSe Start\n" );
    p_wk->se_stop = FALSE;
  }else{
    p_wk->se_stop = TRUE;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  格闘SEシステム  破棄
 */
//-----------------------------------------------------------------------------
static void FIGHT_SOUND_Delete(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIGHT_SOUND* p_wk = p_work;

  if( p_wk->se_stop == FALSE ){
    FSND_StopEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_174 );
    //TOMOYA_Printf( "EnvSe Stop del\n" );
  }
}




//----------------------------------------------------------------------------
/**
 *	@brief  格闘SEシステム  更新
 */
//-----------------------------------------------------------------------------
static void FIGHT_SOUND_Update(FLDMAPFUNC_WORK* p_funcwk, FIELDMAP_WORK* p_fieldmap, void* p_work )
{
  FIGHT_SOUND* p_wk = p_work;

  if( *p_wk->p_localwk_1 ){
    if( p_wk->se_stop == FALSE ){
      FSND_StopEnvSE( p_wk->p_fieldsound, SEQ_SE_FLD_174 );
      //TOMOYA_Printf( "EnvSe Stop\n" );
      p_wk->se_stop = TRUE;
    }
  }
}

