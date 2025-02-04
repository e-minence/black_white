//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc.c
 *	@brief  White Forest  Black City 
 *	@author	tomoya takahashi
 *	@date		2009.11.09
 *
 *	モジュール名：FIELD_WFBC
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/arc_def.h"
#include "arc/field_wfbc_data.naix"
#include "arc/fieldmap/eventdata.naix"

#include "savedata/mystatus.h"

#include "field/field_const.h"

#include "field_wfbc.h"
#include "land_data_patch.h"

#include "eventdata_local.h"



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	ブロック配置情報
//=====================================
#define FIELD_WFBC_BLOCK_PATCH_MAX  (16)
#define FIELD_WFBC_BLOCK_NONE  (0xff)

static const u32 sc_FIELD_WFBC_BLOCK_FILE[ FIELD_WFBC_CORE_TYPE_MAX ] = 
{
  NARC_field_wfbc_data_wb_wfbc_block_bc_block,
  NARC_field_wfbc_data_wb_wfbc_block_wf_block,
};
#define FIELD_WFBC_BLOCK_BUFF_SIZE  ( 0x600 )


//-------------------------------------
///	イベント情報
//=====================================
#define FIELD_WFBC_EVENT_WF  (NARC_eventdata_wfblock_bin)
#define FIELD_WFBC_EVENT_BC  (NARC_eventdata_bcblock_bin)


//-------------------------------------
///	現在のマップ情報
//  不定値
//=====================================
#define FIELD_WFBC_NOW_MAP_ERR  ( 0xffff )




//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	land_data_patchの情報
//=====================================
typedef struct 
{
  u32 patch[FIELD_WFBC_BLOCK_PATCH_MAX];
} FIELD_WFBC_BLOCK_PATCH;

//-------------------------------------
///	
//=====================================
typedef struct 
{
  u8 pos_x:4;   // ブロックｘ位置
  u8 pos_z:4;   // ブロックｚ位置
  u8 block_no;
} WFBC_BLOCK_DATA;


//-------------------------------------
///	ブロック配置情報
//=====================================
typedef struct 
{
  // 各グリッド位置のブロック情報
  WFBC_BLOCK_DATA block_data[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X];

  // パッチ情報
  FIELD_WFBC_BLOCK_PATCH patch[]; 
} FIELD_WFBC_BLOCK_DATA;


//-------------------------------------
///	配置１情報
//=====================================
typedef struct 
{
  WFBC_BLOCK_DATA block_tag;
  u16 block_id;
} WFBC_BLOCK_NOW;

//-------------------------------------
///	実際の配置情報
//=====================================
typedef struct 
{
   WFBC_BLOCK_NOW map_now[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X];
} FIELD_WFBC_NOW_MAPDATA;




//-------------------------------------
///	イベント起動情報
//=====================================
typedef struct 
{
  // 条件
  u16 block_tag;
  u16 block_id;

  // 接続イベントインデックス
  u32 connect_idx;
} FIELD_WFBC_EVENT_IF;


//-------------------------------------
///	カメラ情報
//=====================================
typedef struct 
{
  u16 camera_no[ FIELD_WFBC_PEOPLE_MAX+1 ];
} FIELD_WFBC_CAMERA_SETUP;


//-------------------------------------
///	人物情報
//=====================================
struct _FIELD_WFBC_PEOPLE 
{
  u16   status;   // FIELD_WFBC_PEOPLE_STATUS
  FIELD_WFBC_PEOPLE_DATA people_data;
  FIELD_WFBC_CORE_PEOPLE people_local;

  u16   gx;   // グリッドｘ座標
  u16   gz;   // グリッドｚ座標
};

//-------------------------------------
///	見た目構築用の情報郡
//  
//  見た目は常に表の情報を使用します。
//
//  いる人は、
//  　表のときもあれば
//    裏のときもあります。
//=====================================
typedef struct {
  // 乱数データ
  // 配置ブロックの計算に使用
  // その他の用途では使用しないこと！！！
  GFL_STD_RandContext randData;
  u16 block_rand_max;
  u16 block_pattern_num;

  u32 draw_people_num;
  u32 random_no;
} WFBC_DRAW_PARAM;



//-------------------------------------
///	WFBCワーク
//=====================================
struct _FIELD_WFBC 
{
  u8    type;     // WF / BC FIELD_WFBC_CORE_TYPE
  u8    mapmode;  // マップモードMAPMODE
  FIELD_WFBC_PEOPLE* p_people;

  // mapmodeがMAPMODE_NORMALなら、即時に変更がセーブデータに設定される。
  FIELD_WFBC_CORE* p_core;  

  // ローダー
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader;

  // アーカイブハンドル
  ARCHANDLE* p_handle;

  // ブロック配置情報
  FIELD_WFBC_BLOCK_DATA* p_block;

  // 今のマップ情報
  // 生成されたブロックの情報
  FIELD_WFBC_NOW_MAPDATA map_data;

  // 人物位置情報
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;

  // 描画生成パラメータ
  WFBC_DRAW_PARAM draw_param;

  // 動作モデルシステム
  MMDLSYS* p_mmdlsys;
};





#ifdef PM_DEBUG

extern s8 DEUBG_WFBC_BLOCK_LEVEL;

u8 DEBUG_WFBC_RandData[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X] = {0};
u8 DEBUG_WFBC_LevelData[FIELD_WFBC_BLOCK_SIZE_Z][FIELD_WFBC_BLOCK_SIZE_X] = {0};

#endif


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_WFBC
//=====================================
static void WFBC_InitSystem( FIELD_WFBC* p_wk, HEAPID heapID );
static void WFBC_ExitSystem( FIELD_WFBC* p_wk );

static void WFBC_Clear( FIELD_WFBC* p_wk );
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk );
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );

static void WFBC_SetCore( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_buff, MAPMODE mapmode );
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff );

static void WFBC_ReadCore( FIELD_WFBC* p_wk );
static void WFBC_UpdateCore( FIELD_WFBC* p_wk );

static FIELD_WFBC_PEOPLE* WFBC_GetPeople( FIELD_WFBC* p_wk, u32 npc_id );
static const FIELD_WFBC_PEOPLE* WFBC_GetConstPeople( const FIELD_WFBC* cp_wk, u32 npc_id );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data );
static void WFBC_PEOPLE_SetCore( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core );
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core );


//-------------------------------------
///	FIELD_WFBC_NOW_MAPDATA
//=====================================
static void WFBC_NOW_MAPDATA_Init( FIELD_WFBC_NOW_MAPDATA* p_wk );
static void WFBC_NOW_MAPDATA_SetData( FIELD_WFBC_NOW_MAPDATA* p_wk, u32 block_x, u32 block_z, WFBC_BLOCK_DATA block_tag, u16 block_id );
static WFBC_BLOCK_NOW WFBC_NOW_MAPDATA_GetData( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u32 block_x, u32 block_z );
static BOOL WFBC_NOW_MAPDATA_IsDataIn( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u16 block_tag, u16 block_id );


//-------------------------------------
///	WFBC_DRAW_PARAM
//=====================================
static void WFBC_DRAW_PARAM_Init( WFBC_DRAW_PARAM* p_wk );
static void WFBC_DRAW_PARAM_SetUp( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_CORE* cp_data, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader );
static void WFBC_DRAW_PARAM_MakeMapData( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_BLOCK_DATA* cp_block, FIELD_WFBC_NOW_MAPDATA* p_mapdata );



//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用ワークの生成
 *
 *	@param	heapID  ヒープID
 *
 *	@return WFBCワーク
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FIELD_WFBC_Create( HEAPID heapID )
{
  FIELD_WFBC* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC) );

  WFBC_Clear( p_wk );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用ワークの破棄
 *
 *	@param	p_wk  ワーク
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_Delete( FIELD_WFBC* p_wk )
{
  GF_ASSERT( p_wk );

  WFBC_ExitSystem( p_wk );

  // 動作モデルの影を復帰
  if( p_wk->p_mmdlsys ){
    MMDLSYS_SetJoinShadow( p_wk->p_mmdlsys, TRUE );
  }

  GFL_HEAP_FreeMemory( p_wk );
}



//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用高さテーブルインデックスを返す
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 高さテーブルのインデックス
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );

  // 人数を数える
  return WFBC_GetPeopleNum( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCCOREデータを取得する
 *
 *	@param	cp_wk   ワーク
 *	@param	p_buff  格納バッファ
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  WFBC_GetCore( cp_wk, p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCCOREデータを最新にする。
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_ReadCore( FIELD_WFBC* p_wk )
{
  WFBC_ReadCore( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物情報の取得
 *
 *	@param	cp_wk   ワーク
 *	@param	index   インデックス
 *
 *	@retval 人物情報
 *	@retval NULL  いない 
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, u32 npc_id )
{
  if( npc_id < FIELD_WFBC_NPCID_MAX )
  {
    return WFBC_GetConstPeople( cp_wk, npc_id );
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCの街タイプを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 街タイプ
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_TYPE FIELD_WFBC_GetType( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCのマップモードの取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return マップモード
 */
//-----------------------------------------------------------------------------
MAPMODE FIELD_WFBC_GetMapMode( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->mapmode;
}

//----------------------------------------------------------------------------
/**
 *	@brief  配置ブロックの設定
 *
 *	@param	p_wk     ワーク
 *	@param  p_attr    アトリビュート
 *	@param	p_bm      buildmodel
 *	@param	g3Dmap    マップブロック管理
 *	@param  build_count 配置モデルカウント
 *	@param	block_x   ブロックｘインデックス
 *	@param	block_z   ブロックｚインデックス
 *	@param  score     ブロックスコア
 *	@param	heapID    ヒープID 
 *
 *	@retval build_count 追加後
 */
//-----------------------------------------------------------------------------
int FIELD_WFBC_SetUpBlock( FIELD_WFBC* p_wk, NormalVtxFormat* p_attr, FIELD_BMODEL_MAN* p_bm, FLD_G3D_MAP* g3Dmap, int build_count, u32 block_x, u32 block_z, HEAPID heapID )
{
  WFBC_BLOCK_NOW map_data;
  u32 local_grid_x, local_grid_z;
  FIELD_DATA_PATCH* p_patch;
  

  map_data = WFBC_NOW_MAPDATA_GetData( &p_wk->map_data, block_x, block_z );

  
  if( map_data.block_tag.block_no != FIELD_WFBC_BLOCK_NONE )
  {
    TOMOYA_Printf( "block_tag %d\n", map_data.block_tag.block_no );
    TOMOYA_Printf( "block pos x %d  z %d\n", map_data.block_tag.pos_x, map_data.block_tag.pos_z );
    TOMOYA_Printf( "block_x=%d  block_z=%d\n", block_x, block_z );


    // g3Dmapローカルのグリッド座標を求める
    local_grid_x = FIELD_WFBC_BLOCK_TO_GRID_X( block_x ) % 32;
    local_grid_z = FIELD_WFBC_BLOCK_TO_GRID_Z( block_z ) % 32;
    TOMOYA_Printf( "local_grid_x %d\n", local_grid_x );
    TOMOYA_Printf( "local_grid_z %d\n", local_grid_z );

    TOMOYA_Printf( "build_count %d\n", build_count );
    
    // パッチ情報読み込み
    p_patch = FIELD_DATA_PATCH_Create( map_data.block_id, GFL_HEAP_LOWID(heapID) );

    // アトリビュートのオーバーライト
    FIELD_DATA_PATCH_OverWriteAttrEx( p_patch, p_attr, map_data.block_tag.pos_x*8, map_data.block_tag.pos_z*8, local_grid_x, local_grid_z, 8, 8 );

    // 配置上書き
    if( (map_data.block_tag.pos_x == 0) && (map_data.block_tag.pos_z == 0) )
    {
      // 左上のブロック情報のときにのみ設定
      build_count = FIELD_LAND_DATA_PATCH_AddBuildModel( p_patch, p_bm, g3Dmap, build_count, local_grid_x, local_grid_z );
    }

    // パッチ情報破棄
    FIELD_DATA_PATCH_Delete( p_patch );
  }

  return build_count;
}


//----------------------------------------------------------------------------
/**
 *	@brief  イベントの設定変更
 *
 *	@param	cp_wk     ワーク
 *	@param	p_evdata  イベントシステム
 *	@param	heapID    ヒープID
 */
//-----------------------------------------------------------------------------
void FILED_WFBC_EventDataOverwrite( const FIELD_WFBC* cp_wk, EVENTDATA_SYSTEM* p_evdata, MAPMODE mapmode, HEAPID heapID )
{
  int i, j;
  FIELD_WFBC_EVENT_IF* p_event_if;
  u32 load_size;
  const CONNECT_DATA* cp_connect;
  static const u32 sc_EVIF[ FIELD_WFBC_CORE_TYPE_MAX ] = 
  {
    NARC_field_wfbc_data_bc_block_event_evp,
    NARC_field_wfbc_data_wf_block_event_evp,
  };
  u32 add_connect_num;
  u32* p_ok_connect_idx_tbl;
  u32 ok_connect_idx_num;

  // 裏世界ではなにもしない
  if( mapmode != MAPMODE_NORMAL )
  {
    return ;
  }

  // 拡張イベント情報を取得
  load_size = GFL_ARC_GetDataSizeByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ] );
  p_event_if = GFL_ARC_LoadDataAllocByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ], GFL_HEAP_LOWID(heapID) );
  
  // データサイズから、管理接続ポイントの数をチェック
  add_connect_num = load_size / sizeof(FIELD_WFBC_EVENT_IF);

  // OK CONNECT IDX テーブル作成
  p_ok_connect_idx_tbl = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), sizeof(u32)*add_connect_num );
  ok_connect_idx_num = 0;

  // 足しこむ
  for( i=0; i<add_connect_num; i++ )
  {
    TOMOYA_Printf( "Event Check blockTag=%d  blockID=%d\n", p_event_if[i].block_tag, p_event_if[i].block_id );
    if( WFBC_NOW_MAPDATA_IsDataIn( &cp_wk->map_data, p_event_if[i].block_tag, p_event_if[i].block_id ) )
    {
      // OK CONNECT_IDXに追加
      p_ok_connect_idx_tbl[ ok_connect_idx_num ] = p_event_if[i].connect_idx;
      ok_connect_idx_num ++;
    }
    else
    {
      // OKがいなくて、最後のconnect_idxなら本当に破棄チェック
      for( j=0; j<ok_connect_idx_num; j++ )
      {
        if( p_ok_connect_idx_tbl[j] == p_event_if[i].connect_idx )
        {
          break;
        }
      }
      if( j>=ok_connect_idx_num )
      {
        for( j=i+1; j<add_connect_num; j++ )
        {
          if( p_event_if[i].connect_idx == p_event_if[j].connect_idx )
          {
            break;
          }
        }
        // 最後のconnect_idxなら本当に破棄
        if( j>=add_connect_num ){
          TOMOYA_Printf( "Event Check EventDel %d\n", p_event_if[i].connect_idx );
          // 削除
          EVENTDATA_SYS_DelConnectEventIdx( p_evdata, p_event_if[i].connect_idx );
        }
      }
    }
  }

  GFL_HEAP_FreeMemory( p_ok_connect_idx_tbl );
  GFL_HEAP_FreeMemory( p_event_if );
}


//----------------------------------------------------------------------------
/**
 *	@brief  カメラの設定
 *
 *	@param	cp_wk     ワーク
 *	@param	p_camera  カメラ
 *	@param	heapID    ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUpCamera( const FIELD_WFBC* cp_wk, FIELD_CAMERA* p_camera, HEAPID heapID )
{
  FIELD_WFBC_CAMERA_SETUP* p_buff;
  u32 people_num;
  
  // 読み込み
  p_buff = GFL_ARCHDL_UTIL_Load( cp_wk->p_handle, NARC_field_wfbc_data_wb_wfbc_camera_cam, FALSE, GFL_HEAP_LOWID(heapID) );  

  // 人物の数を取得
  people_num = FIELD_WFBC_GetPeopleNum( cp_wk );

  GF_ASSERT( people_num <= FIELD_WFBC_PEOPLE_MAX );

  // カメラ設定
  FIELD_CAMERA_SetCameraType( p_camera, p_buff->camera_no[ people_num ] );

  GFL_HEAP_FreeMemory( p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  動作モデルシステムの設定
 *
 *	@param	cp_wk       ワーク
 *	@param	p_mmdlsys   動作モデル
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUpMmdlSys( FIELD_WFBC* p_wk, MMDLSYS* p_mmdlsys )
{
  p_wk->p_mmdlsys = p_mmdlsys;
  //@todo もしどうしても、処理落ちが軽減できなかったら、
  //影をけすことで300 ms軽くなる。
  //MMDLSYS_SetJoinShadow( p_mmdlsys, FALSE );
}



//----------------------------------------------------------------------------
/**
 *	@brief  街情報を設定
 *
 *	@param	p_wk        ワーク
 *	@param	cp_core     コア情報
 *	@param  mapmode     マップモード
 *	@param  heapID      ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUp( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_core, MAPMODE mapmode, HEAPID heapID )
{
  // 内部ワークの初期か
  WFBC_InitSystem( p_wk, heapID );
  
  WFBC_SetCore( p_wk, p_core, mapmode );
}

//----------------------------------------------------------------------------
/**
 *	@brief  街情報をクリア
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_Clear( FIELD_WFBC* p_wk )
{
  WFBC_Clear( p_wk );
  WFBC_ExitSystem( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物の追加
 *
 *	@param	p_wk    ワーク
 *
 *	@retval 追加したインデックス
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  int index;
  const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos;

  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );
  
  // 空いているワークを取得
  index = WFBC_GetClearPeople( p_wk );

  // NPC情報読み込み
  FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, cp_core->npc_id );
  
  // 人情報をセットアップ
  cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, index );
  WFBC_PEOPLE_SetUp( &p_wk->p_people[index], cp_core, cp_pos, FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );

  // セーブデータに反映
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物の破棄
 *
 *	@param	p_wk    ワーク
 *	@param	npc_id  破棄する人物のnpc_ID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );
  // インデックスのワークをクリーン
  WFBC_PEOPLE_Clear( p_people );

  // セーブデータに反映
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_idの人を連れて行かれた設定にする
 *
 *	@param	p_wk      ワーク
 *	@param	npc_id    npc_id
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetAwayPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );
  p_people->status = FIELD_WFBC_PEOPLE_STATUS_AWAY;
 
  // セーブデータに反映
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  その人と会話した、機嫌値加算処理
 *
 *	@param	p_wk
 *	@param	npc_id 
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_AddTalkPointPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );

  // 会話加算
  if( p_wk->mapmode == MAPMODE_NORMAL )
  {
    FIELD_WFBC_CORE_PEOPLE_CalcTalk( &p_people->people_local );

    // セーブデータに反映
    WFBC_UpdateCore( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワードセットに履歴のプレイヤー情報を設定
 *
 *	@param	cp_wk       ワーク
 *	@param	p_wordset   ワードセット
 *	@param	npc_id      NPCID
 *	@param  word_set_idx  ワードセットのインデックス
 *	@param  heapID      ヒープID
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetWordSetParentPlayer( FIELD_WFBC* p_wk, WORDSET* p_wordset, u32 npc_id, u32 word_set_idx, HEAPID heapID )
{
  MYSTATUS* p_mystatus;
  const FIELD_WFBC_PEOPLE* cp_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  cp_people = WFBC_GetConstPeople( p_wk, npc_id );

  p_mystatus = MyStatus_AllocWork( GFL_HEAP_LOWID(heapID) );
  MyStatus_Init( p_mystatus );
  
  MyStatus_SetMyName( p_mystatus, cp_people->people_local.parent );
  MyStatus_SetID( p_mystatus, cp_people->people_local.parent_id );
  

  // ワードセットに設定
  WORDSET_RegisterPlayerName( p_wordset, word_set_idx, p_mystatus );


  GFL_HEAP_FreeMemory( p_mystatus );
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルしたことを設定
 *
 *	@param	p_wk      ワーク
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetBattlePeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  FIELD_WFBC_PEOPLE* p_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  p_people = WFBC_GetPeople( p_wk, npc_id );

  FIELD_WFBC_CORE_PEOPLE_SetBattle( &p_people->people_local );

  // セーブデータに反映
  WFBC_UpdateCore( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルすることが出来るのかを返す
 *
 *	@param	p_wk      ワーク
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE    出来る
 *	@retval FALSE   出来ない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_IsBattlePeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  const FIELD_WFBC_PEOPLE* cp_people;
  GF_ASSERT( p_wk );

  // 1日変化でデータが変わっている可能性があるので、書き戻す。
  WFBC_ReadCore( p_wk );

  cp_people = WFBC_GetConstPeople( p_wk, npc_id );

  return FIELD_WFBC_CORE_PEOPLE_IsBattle( &cp_people->people_local );
}



//----------------------------------------------------------------------------
/**
 *	@brief  人物　見た目の取得
 *
 *	@param	cp_people   ワーク
 *
 *	@return 見た目
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_GetOBJCode( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->people_data.objid;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク  から　人物状態を取得
 *
 *	@param	cp_people   人物ワーク
 *
 *	@retval 人物状態
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_PEOPLE_STATUS FIELD_WFBC_PEOPLE_GetStatus( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物データ  から　人物基本情報を取得
 *
 *	@param	cp_people   人物ワーク
 *
 *	@return 基本情報
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE_DATA* FIELD_WFBC_PEOPLE_GetPeopleData( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return &cp_people->people_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物データ　から　ローカル　のセーブ人物情報を取得
 *
 *	@param	cp_people   人物ワーク
 *
 *	@return ローカルのセーブ人物情報
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_PEOPLE_GetPeopleCore( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return &cp_people->people_local;
}







//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  必要システムの初期化
 *
 *	@param	p_wk        ワーク
 *	@param	heapID      ヒープID
 */
//-----------------------------------------------------------------------------
static void WFBC_InitSystem( FIELD_WFBC* p_wk, HEAPID heapID )
{
  int i;
  
  if(p_wk->p_loader == NULL)
  {
    p_wk->p_loader = FIELD_WFBC_PEOPLE_DATA_Create( 0, heapID );
    p_wk->p_people = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC_PEOPLE) * FIELD_WFBC_PEOPLE_MAX );

    //　人物ワークをクリア
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      WFBC_PEOPLE_Clear( &p_wk->p_people[i] );
    }

  
    // データ読み込みようのハンドル生成
    p_wk->p_handle = GFL_ARC_OpenDataHandle( ARCID_WFBC_PEOPLE, heapID );

    // 配置情報のバッファを生成
    p_wk->p_block = GFL_HEAP_AllocClearMemory( heapID, FIELD_WFBC_BLOCK_BUFF_SIZE );

    // 人物位置情報を生成
    p_wk->p_pos = FIELD_WFBC_PEOPLE_POS_Create( p_wk->p_loader, p_wk->type, heapID );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  必要システムの破棄
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WFBC_ExitSystem( FIELD_WFBC* p_wk )
{
  if(p_wk->p_loader)
  {
    // 人物位置情報を生成
    FIELD_WFBC_PEOPLE_POS_Delete( p_wk->p_pos );
    GFL_HEAP_FreeMemory( p_wk->p_block );
    GFL_ARC_CloseDataHandle( p_wk->p_handle );
    FIELD_WFBC_PEOPLE_DATA_Delete( p_wk->p_loader );
    GFL_HEAP_FreeMemory( p_wk->p_people );

    p_wk->p_loader = NULL;
    p_wk->p_people = NULL;
    p_wk->p_handle = NULL;
    p_wk->p_block = NULL;
    p_wk->p_pos = NULL;
  } 
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBCワークをクリア
 */
//-----------------------------------------------------------------------------
static void WFBC_Clear( FIELD_WFBC* p_wk )
{
  int i;

  p_wk->type = 0;
  p_wk->mapmode = 0;
  p_wk->p_core = NULL;

  //　人物ワークをクリア
  if( p_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      WFBC_PEOPLE_Clear( &p_wk->p_people[i] );
    }
  }

  // ブロック配置情報をクリア
  if( p_wk->p_block )
  {
    GFL_STD_MemClear( p_wk->p_block, FIELD_WFBC_BLOCK_BUFF_SIZE );
  }

  // 配置情報クリア
  WFBC_NOW_MAPDATA_Init( &p_wk->map_data );

  // 描画生成ワーククリア
  WFBC_DRAW_PARAM_Init( &p_wk->draw_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief  空いている人物ワークを取得する
 *
 *	@param	cp_wk     ワーク
 *
 *	@return 空いているワークのインデックス
 */
//-----------------------------------------------------------------------------
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );

  if( cp_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        return i;
      }
    }
  }

  GF_ASSERT_MSG( 0, "WFBC People Over\n" );
  return 0; // メモリ破壊などがないように０を返す
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物の数を取得する
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 人物の数
 */
//-----------------------------------------------------------------------------
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  int num;
  int i;
  
  GF_ASSERT( cp_wk );

  num = 0;
  if( cp_wk->p_people )
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        num ++;
      }
    }
  }

  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  CORE情報を設定する
 *
 *	@param	p_wk        ワーク
 *	@param	cp_buff     バッファ
 */
//-----------------------------------------------------------------------------
static void WFBC_SetCore( FIELD_WFBC* p_wk, FIELD_WFBC_CORE* p_buff, MAPMODE mapmode )
{
  int i;
  BOOL result;
  const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos;
  u32 random_max;
  u32 pattern_num;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;

  GF_ASSERT( p_wk );
  GF_ASSERT( p_wk->p_people );
  GF_ASSERT( p_wk->p_loader );
  GF_ASSERT( p_buff );

  result = FIELD_WFBC_CORE_IsInData( p_buff );
  GF_ASSERT( result );
  
  // 現状方WFBC情報をクリーンアップ
  WFBC_Clear( p_wk );
  

  // 情報を設定
  p_wk->type    = p_buff->type;
  p_wk->mapmode = mapmode;
  random_max    = 0;
  pattern_num   = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // 人物情報の設定
    // データがあったら設定
    if( mapmode == MAPMODE_NORMAL )
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->people[i] ) )
      {
        // NPC情報読み込み
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->people[i].npc_id );
        cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, i );
        cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->people[i], cp_pos, cp_people_data );
      }
    }
    else
    {
      if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_buff->back_people[i] ) )
      {
        // NPC情報読み込み
        FIELD_WFBC_PEOPLE_DATA_Load( p_wk->p_loader, p_buff->back_people[i].npc_id );
        cp_pos = FIELD_WFBC_PEOPLE_POS_GetIndexData( p_wk->p_pos, i );
        cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader );
        WFBC_PEOPLE_SetUp( &p_wk->p_people[i], &p_buff->back_people[i], cp_pos, FIELD_WFBC_PEOPLE_DATA_GetData( p_wk->p_loader ) );
      }
    }
  }

  // 見た目にかかわるパラメータの生成
  WFBC_DRAW_PARAM_SetUp( &p_wk->draw_param, p_buff, p_wk->p_loader );

  // コア情報保存
  if( mapmode == MAPMODE_NORMAL )
  {
    p_wk->p_core = p_buff;
  }
  else
  {
    p_wk->p_core = NULL;
  }


  // ブロック配置情報を読み込む
  {
    u32 size = GFL_ARC_GetDataSizeByHandle( p_wk->p_handle, sc_FIELD_WFBC_BLOCK_FILE[p_wk->type] );
    GF_ASSERT( size < FIELD_WFBC_BLOCK_BUFF_SIZE );
    GFL_ARC_LoadDataByHandle( p_wk->p_handle, sc_FIELD_WFBC_BLOCK_FILE[p_wk->type], p_wk->p_block );
  }

  // ブロック配置情報生成
  WFBC_DRAW_PARAM_MakeMapData( &p_wk->draw_param, p_wk->p_block, &p_wk->map_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief  CORE情報を取得する
 *
 *	@param	cp_wk     ワーク
 *	@param	p_buff    データ格納先
 */
//-----------------------------------------------------------------------------
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  int i;

  GF_ASSERT( cp_wk );
  GF_ASSERT( p_buff );
  GF_ASSERT( cp_wk->p_people );

  // 情報を格納
  p_buff->data_in = TRUE;
  p_buff->type    = cp_wk->type;

  if( cp_wk->mapmode == MAPMODE_NORMAL )
  {
    FIELD_WFBC_CORE_ClearNormal( p_buff );
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      // データがあったら設定
      WFBC_PEOPLE_GetCore( &cp_wk->p_people[i], &p_buff->people[i] );
    }
  }
  else
  {
    FIELD_WFBC_CORE_ClearBack( p_buff );
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      // データがあったら設定
      WFBC_PEOPLE_GetCore( &cp_wk->p_people[i], &p_buff->back_people[i] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  現状の人物情報を巻き戻す。
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void WFBC_ReadCore( FIELD_WFBC* p_wk )
{
  int i;
  
  if( (p_wk->mapmode == MAPMODE_NORMAL) && (p_wk->p_core != NULL) )
  {
    // 情報の格納
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      WFBC_PEOPLE_SetCore( &p_wk->p_people[i], &p_wk->p_core->people[i] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  マップモードが通常時のセーブデータ更新処理
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WFBC_UpdateCore( FIELD_WFBC* p_wk )
{
  if( (p_wk->mapmode == MAPMODE_NORMAL) && (p_wk->p_core != NULL) )
  {
    // 情報の格納
    FIELD_WFBC_GetCore( p_wk, p_wk->p_core );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_idの人の取得
 *
 *	@param	p_wk    ワーク
 *	@param	npc_id 
 *
 *	@return 人
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_PEOPLE* WFBC_GetPeople( FIELD_WFBC* p_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( p_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  if(p_wk->p_people)
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        if( FIELD_WFBC_CORE_PEOPLE_GetNpcID(&p_wk->p_people[i].people_local ) == npc_id )
        {
          return &p_wk->p_people[i];
        }
      }
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  npc_idの人の取得
 *
 *	@param	cp_wk
 *	@param	npc_id 
 *
 *	@return 人
 */
//-----------------------------------------------------------------------------
static const FIELD_WFBC_PEOPLE* WFBC_GetConstPeople( const FIELD_WFBC* cp_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( cp_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  if(cp_wk->p_people)
  {
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( cp_wk->p_people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
      {
        if( FIELD_WFBC_CORE_PEOPLE_GetNpcID(&cp_wk->p_people[i].people_local) == npc_id )
        {
          return &cp_wk->p_people[i];
        }
      }
    }
  }

  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物ワークのクリア
 *
 *	@param	p_people  人物
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people )
{
  GFL_STD_MemClear( p_people, sizeof(FIELD_WFBC_PEOPLE) );
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NONE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人情報のセットアップ
 *
 *	@param	p_people    人物ワーク
 *	@param	cp_core     人物CORE情報
 *	@param  cp_pos      位置情報
 *	@param  cp_data     データ
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data )
{
  BOOL result;
  
  GF_ASSERT( p_people );
  // 正常な情報かチェック
  result = FIELD_WFBC_CORE_PEOPLE_IsInData( cp_core );
  GF_ASSERT( result == TRUE );

  p_people->status        = FIELD_WFBC_PEOPLE_STATUS_NORMAL;
  GFL_STD_MemCopy( cp_core, &p_people->people_local, sizeof(FIELD_WFBC_CORE_PEOPLE) );

  // NPCIDの基本情報を読み込む
  GFL_STD_MemCopy( cp_data, &p_people->people_data, sizeof(FIELD_WFBC_PEOPLE_DATA) );
  p_people->gx        = cp_pos->gx;
  p_people->gz        = cp_pos->gz;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物CORE情報の設定
 *
 *	@param	cp_people   人物ワーク
 *	@param	p_core      情報格納先
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_SetCore( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  GF_ASSERT( p_people );
  GF_ASSERT( cp_core );

  if( p_people->status == FIELD_WFBC_PEOPLE_STATUS_NORMAL )
  {
    GFL_STD_MemCopy( cp_core, &p_people->people_local, sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物CORE情報の取得
 *
 *	@param	cp_people   人物ワーク
 *	@param	p_core      情報格納先
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core )
{
  GF_ASSERT( cp_people );
  GF_ASSERT( p_core );

  if( cp_people->status == FIELD_WFBC_PEOPLE_STATUS_NORMAL )
  {
    // 情報をコピー
    GFL_STD_MemCopy( &cp_people->people_local, p_core, sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  生成マップ情報　初期化
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WFBC_NOW_MAPDATA_Init( FIELD_WFBC_NOW_MAPDATA* p_wk )
{
  int i, j;

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      p_wk->map_now[i][j].block_tag.block_no = FIELD_WFBC_BLOCK_NONE;
      p_wk->map_now[i][j].block_id = FIELD_WFBC_NOW_MAP_ERR;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  マップ情報の設定
 *
 *	@param	p_wk        ワーク
 *	@param	block_x     ブロックX
 *	@param	block_z     ブロックZ
 *	@param	block_tag   ブロック　タグID
 *	@param	block_id    ブロック　LAND_DATA_PATCH　ID
 */
//-----------------------------------------------------------------------------
static void WFBC_NOW_MAPDATA_SetData( FIELD_WFBC_NOW_MAPDATA* p_wk, u32 block_x, u32 block_z, WFBC_BLOCK_DATA block_tag, u16 block_id )
{
  GF_ASSERT( block_x < FIELD_WFBC_BLOCK_SIZE_X );
  GF_ASSERT( block_z < FIELD_WFBC_BLOCK_SIZE_Z );
  
  p_wk->map_now[block_z][block_x].block_tag = block_tag;
  p_wk->map_now[block_z][block_x].block_id = block_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロックの　マップ情報取得
 *
 *	@param	p_wk      ワーク
 *	@param	block_x   ブロックｘ
 *	@param	block_z   ブロックｚ
 *
 *	@return マップ情報
 */
//-----------------------------------------------------------------------------
static WFBC_BLOCK_NOW WFBC_NOW_MAPDATA_GetData( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u32 block_x, u32 block_z )
{
  GF_ASSERT( block_x < FIELD_WFBC_BLOCK_SIZE_X );
  GF_ASSERT( block_z < FIELD_WFBC_BLOCK_SIZE_Z );
  return cp_wk->map_now[block_z][block_x];
}

//----------------------------------------------------------------------------
/**
 *	@brief  block_tagのblock_idがあるかチェック
 *
 *	@param	cp_wk       ワーク
 *	@param	block_tag
 *	@param	block_id 
 *
 *	@retval TRUE  ある
 *	@retval FALSE なし
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_NOW_MAPDATA_IsDataIn( const FIELD_WFBC_NOW_MAPDATA* cp_wk, u16 block_tag, u16 block_id )
{
  int i, j;

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      if( cp_wk->map_now[i][j].block_tag.block_no == block_tag )
      {
        if( cp_wk->map_now[i][j].block_id == block_id )
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  描画用パラメータの初期化
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WFBC_DRAW_PARAM_Init( WFBC_DRAW_PARAM* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WFBC_DRAW_PARAM) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  描画用パラメータの設定
 *
 *	@param	p_wk      ワーク
 *	@param	cp_data   WFBCコア情報
 *	@param  p_loader  ローダー
 */
//-----------------------------------------------------------------------------
static void WFBC_DRAW_PARAM_SetUp( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_CORE* cp_data, FIELD_WFBC_PEOPLE_DATA_LOAD* p_loader )
{
  int i;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  u32 random_max;
  u32 pattern_num;

  random_max = 0;
  pattern_num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // 見た目にかかわる。
    // ブロック補正値系は、表世界の情報を使用する
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_data->people[i] ) )
    {
      // NPC情報読み込み
      FIELD_WFBC_PEOPLE_DATA_Load( p_loader, cp_data->people[i].npc_id );
      cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_loader );
      // ブロック補正値を求める
      random_max += cp_people_data->block_param;
      pattern_num += cp_data->people[i].npc_id;
    }
  }

  // 描画人物数
  p_wk->draw_people_num = FIELD_WFBC_CORE_GetPeopleNum( cp_data, MAPMODE_NORMAL );
  
  GFL_STD_RandInit( &p_wk->randData, (u64)((cp_data->random_no << 32) + cp_data->random_no) );
  if( p_wk->draw_people_num == 0 ){
    p_wk->block_rand_max = 1; // 常に０
  }else{
    p_wk->block_rand_max = 2 + random_max;
  }
  p_wk->block_pattern_num = pattern_num;
  p_wk->random_no = cp_data->random_no;


}

//----------------------------------------------------------------------------
/**
 *	@brief  描画パラメータを使用して、ブロック配置情報を生成
 *
 *	@param	p_wk      ワーク
 *	@param	p_mapdata マップ情報
 */
//-----------------------------------------------------------------------------
#ifdef PM_DEBUG
extern s16 DEBUG_FIELD_WFBC_MAKE_score;
extern u16 DEBUG_FIELD_WFBC_MAKE_flag;
#endif
static void WFBC_DRAW_PARAM_MakeMapData( WFBC_DRAW_PARAM* p_wk, const FIELD_WFBC_BLOCK_DATA* cp_block, FIELD_WFBC_NOW_MAPDATA* p_mapdata )
{
  int i, j;
  WFBC_BLOCK_DATA block_tag;
  const FIELD_WFBC_BLOCK_PATCH* cp_patch_data;
  u8 tbl_index;
  s32 score;
  u32 land_data_patch_id;

  // RANDOM生成
  GFL_STD_RandInit( &p_wk->randData, (u64)((p_wk->random_no << 32) + p_wk->random_no) );

  for( i=0; i<FIELD_WFBC_BLOCK_SIZE_Z; i++ )
  {
    for( j=0; j<FIELD_WFBC_BLOCK_SIZE_X; j++ )
    {
      block_tag = cp_block->block_data[ i ][ j ];

      if( block_tag.block_no != FIELD_WFBC_BLOCK_NONE )
      {
        // ブロックのスコアを計算
        // ブロックの左上のみ計算
        if( (block_tag.pos_x != 0) || (block_tag.pos_z != 0) )
        {
          // 左上ブロックの情報をそのまま保存
          WFBC_NOW_MAPDATA_SetData( p_mapdata, j, i, block_tag, 
              p_mapdata->map_now[i - block_tag.pos_z][j - block_tag.pos_x].block_id );

#ifdef PM_DEBUG
          DEBUG_WFBC_RandData[i][j] = DEBUG_WFBC_RandData[i - block_tag.pos_z][j - block_tag.pos_x];
          DEBUG_WFBC_LevelData[i][j] = DEBUG_WFBC_LevelData[i - block_tag.pos_z][j - block_tag.pos_x];
#endif // PM_DEBUG
        }
        else
        {
          u32 block_lefttop_x, block_lefttop_z;
          u32 rand_num;
          block_lefttop_x = j - block_tag.pos_x;
          block_lefttop_z = i - block_tag.pos_z;
          
          // （（WFにいる全てのOBJのNOの合計　＋　ブロック左上のＸ座標＋　ブロック左上のY座標） / (ブロック左上のＸ座標+1) /(ブロック左上のY座標+1) +２ ）／２
          tbl_index = p_wk->block_pattern_num;
          tbl_index += block_lefttop_x + block_lefttop_z;
          tbl_index /= (block_lefttop_x+1) + (block_lefttop_z+1) + 2;
          tbl_index %= 2;

          // （OBJの人数×0.8）＋ランダム（０〜１）
          score = p_wk->draw_people_num;
          score = (score * 8) / 10;
          rand_num = GFL_STD_Rand( &p_wk->randData, p_wk->block_rand_max );
          score += rand_num;

          if( score >= (FIELD_WFBC_BLOCK_PATCH_MAX/2) )
          {
            score = (FIELD_WFBC_BLOCK_PATCH_MAX/2)-1;
          }
          
          score += tbl_index*(FIELD_WFBC_BLOCK_PATCH_MAX/2);
        
          GF_ASSERT( score < FIELD_WFBC_BLOCK_PATCH_MAX );

          // 設定
          cp_patch_data = &cp_block->patch[ block_tag.block_no ];
          land_data_patch_id = cp_patch_data->patch[ score ];

#ifdef PM_DEBUG
          if( DEUBG_WFBC_BLOCK_LEVEL > 0 ){
            score = DEUBG_WFBC_BLOCK_LEVEL;
            land_data_patch_id = cp_patch_data->patch[ score ];
            rand_num = 0;
          }

          DEBUG_WFBC_LevelData[ i ][ j ] = score;
          DEBUG_WFBC_RandData[ i ][ j ] = rand_num;
#endif // PM_DEBUG

#ifdef FIELD_WFBC_MAKE_MAPDATA_DEBUG
          if( DEBUG_FIELD_WFBC_MAKE_flag )
          {
            int i;
            for( i=0; i<FIELD_WFBC_BLOCK_PATCH_MAX; i++ )
            {
              if( cp_patch_data->patch[ i ] == DEBUG_FIELD_WFBC_MAKE_score )
              {
                land_data_patch_id = DEBUG_FIELD_WFBC_MAKE_score;
              }
            }
          }
#endif

          // 情報の保存
          WFBC_NOW_MAPDATA_SetData( p_mapdata, j, i, block_tag, land_data_patch_id );
        }
      }

    }
  }
}



//デバッグ操作
#ifdef PM_DEBUG

#include "gamesystem/game_data.h"
#include "poke_tool/pokeparty.h"
#include "msg/msg_place_name.h"  // for MAPNAME_xxxx
#include "system/main.h"  // 

static HEAPID DEBUG_WFBC_USE_HEAP_ID;


// 人物書き換えようパラメータ
static u8 DEBUG_WFBCPeople_mode = 0;
static s8 DEBUG_WFBCPeople_index = 0;
static s8 DEBUG_WFBCPeople_npc_id = 0;
static s8 DEBUG_WFBCPeople_mood = 0;
//アイテム書き換えよう
static u8 DEBUG_WFBC_ITEM_index = 0;
static u8 DEBUG_WFBC_ITEM_npc_id = 0;
// 村長ポケモン書き換えよう
static u8 DEBUG_WFBC_GETPOKE_TARGET_npc_id = 0;
static u8 DEBUG_WFBC_GETPOKE_TARGET_poke_idx = 0;


static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_BCWinTargetAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_BCWinTargetAdd10( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFItemSet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFItemSet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFPokeTargetSet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFPokeTargetSet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_WFBlockLevelSet( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_WFBlockLevelSet( void* userWork , DEBUGWIN_ITEM* item );


void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId, void* p_gdata )
{
  DEBUG_WFBC_USE_HEAP_ID = heapId;
  
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             p_gdata , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_BCWinTargetAdd10,DEBWIN_Draw_BCWinTargetAdd10, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_BCWinNumAdd10 ,DEBWIN_Draw_BCWinNumAdd10 , 
                             p_gdata , 10 , heapId );
  
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFPokeTargetSet,DEBWIN_Draw_WFPokeTargetSet, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFPokeGet ,DEBWIN_Draw_WFPokeGet , 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFBCBlockCheck,DEBWIN_Draw_WFBCBlockCheck, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFBCPeopleCheck,DEBWIN_Draw_WFBCPeopleCheck, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFItemSet,DEBWIN_Draw_WFItemSet, 
                             p_gdata , 10 , heapId );

  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_WFBlockLevelSet, DEBWIN_Draw_WFBlockLevelSet, 
                             p_gdata , 10 , heapId );


}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  int  i;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    // 増やす
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in == FALSE )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
        p_wk->people[i].npc_id  = FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( p_wk );
        p_wk->people[i].mood  = 50;
        p_wk->people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
        p_wk->people[i].data_in = TRUE;
        if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
        {
          FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_wk->people[i], GAMEDATA_GetMyStatus(p_gdata) );
        }
        break;
      }
    }

    // サブにも増やす
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->back_people[i].data_in == FALSE )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
        p_wk->back_people[i].npc_id  = FIELD_WFBC_CORE_DEBUG_GetRandomNpcID( p_wk );
        p_wk->back_people[i].mood  = 0;
        p_wk->back_people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
        p_wk->back_people[i].data_in = TRUE;
        if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
        {
          FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_wk->back_people[i], GAMEDATA_GetMyStatus(p_gdata) );
        }
        break;
      }
    }
    
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    // 減らす
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
        break;
      }
    }

    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->back_people[i].data_in )
      {
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
        break;
      }
    }
    FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_NORMAL );
    FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_INTRUDE );
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  u16 level = FIELD_WFBC_CORE_GetPeopleNum( p_wk, MAPMODE_NORMAL );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  FIELD_WFBC_EVENT* p_event = GAMEDATA_GetWFBCEventData( p_gdata );
  u8 type = p_wk->type;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    FIELD_WFBC_EVENT_Clear( p_event );

    if( type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
    {
      type = FIELD_WFBC_CORE_TYPE_WHITE_FOREST;
    }
    else
    {
      type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
    }
    p_wk->type = type;
    DEBUGWIN_RefreshScreen();

    FIELD_WFBC_CORE_SetUpZoneData( p_wk );
    FIELD_WFBC_CORE_CalcOneDataStart( p_gdata, 1, HEAPID_FIELDMAP );
  }
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  u8 type = p_wk->type;

  if( type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetName( item , "Type[BLACK CITY]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "Type[WHITE FOREST]" );
  }
}

static void DEBWIN_Update_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  int i;
  int add = 10;

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
      add = 100;
    }
    
    if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_RIGHT) )
    {
      p_wk->bc_npc_win_count += add;
      DEBUGWIN_RefreshScreen();
    }
    if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_LEFT) )
    {
      if(p_wk->bc_npc_win_count>=add){
        p_wk->bc_npc_win_count -= add;
      }else{
        p_wk->bc_npc_win_count = 0;
      }
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_BCWinNumAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetNameV( item , "BC Win[%d]", p_wk->bc_npc_win_count );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "BC Win??" );
  }
}

static void DEBWIN_Update_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  POKEPARTY* p_party = GAMEDATA_GetMyPokemon( p_gdata );
  POKEMON_PARAM * pp = PokeParty_GetMemberPointer( p_party, 0 );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      static u16 oyaName[6] = {L'で',L'ば',L'ぐ',L'ぽ',L'け',0xFFFF};
      RTCDate now_date;
      GFL_RTC_GetDate( &now_date );
      // 手持ち０番目に捕獲場所WFの目的ポケモンを設定
      PP_Setup( pp, p_wk->wf_poke_catch_monsno, 10, 100 );
      PP_Put( pp, ID_PARA_birth_place, MAPNAME_WC10 );
      PP_Put( pp, ID_PARA_birth_year, now_date.year );
      PP_Put( pp, ID_PARA_birth_month, now_date.month );
      PP_Put( pp, ID_PARA_birth_day, now_date.day );

      PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      static u16 oyaName[6] = {L'で',L'ば',L'ぐ',L'ぽ',L'け',0xFFFF};
      RTCDate now_date;
      GFL_RTC_GetDate( &now_date );
      // 手持ち０番目に捕獲場所WFの目的ポケモンを設定
      PP_Setup( pp, p_wk->wf_poke_catch_monsno, 10, 100 );
      PP_Put( pp, ID_PARA_birth_place, MAPNAME_WC10 );
      PP_Put( pp, ID_PARA_birth_year, now_date.year+1 );
      PP_Put( pp, ID_PARA_birth_month, now_date.month );
      PP_Put( pp, ID_PARA_birth_day, now_date.day );

      PP_Put( pp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( pp , ID_PARA_oyasex , PTL_SEX_MALE );
    }
  }
}

static void DEBWIN_Draw_WFPokeGet( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetName( item , "WF PokeGet" );
}

static void DEBWIN_Update_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  s32 min;
  s32 max;
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    min = LAND_DATA_PATCH_BC_BUILD_01;
    max = LAND_DATA_PATCH_WF_TREE_01;
  }
  else
  {
    min = LAND_DATA_PATCH_WF_TREE_01;
    max = LAND_DATA_PATCH_MAX;
  }
  
  if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A) )
  {
    if(DEBUG_FIELD_WFBC_MAKE_flag){
      DEBUG_FIELD_WFBC_MAKE_flag = FALSE;
    }else{
      DEBUG_FIELD_WFBC_MAKE_flag = TRUE;
    }
    DEBUGWIN_RefreshScreen();
  }

  if(DEBUG_FIELD_WFBC_MAKE_flag)
  {
    if( (DEBUG_FIELD_WFBC_MAKE_score < min) || (DEBUG_FIELD_WFBC_MAKE_score > max) )
    {
      DEBUG_FIELD_WFBC_MAKE_score = min;
    }
    
    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT) )
    {
      DEBUG_FIELD_WFBC_MAKE_score --;
      if( DEBUG_FIELD_WFBC_MAKE_score < min )
      {
        DEBUG_FIELD_WFBC_MAKE_score += (max - min);
      }
      DEBUGWIN_RefreshScreen();
    }

    if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT) )
    {
      DEBUG_FIELD_WFBC_MAKE_score ++;
      if( DEBUG_FIELD_WFBC_MAKE_score >= max )
      {
        DEBUG_FIELD_WFBC_MAKE_score -= (max - min);
      }
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_WFBCBlockCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  if( DEBUG_FIELD_WFBC_MAKE_flag )
  {
    DEBUGWIN_ITEM_SetNameV( item , "BlockCheckON next=[%d]", DEBUG_FIELD_WFBC_MAKE_score );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "BlockCheckOFF" );
  }
}

static void DEBWIN_Update_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
    // 設定
    FIELD_WFBC_CORE_PEOPLE* p_array;
    
    if( DEBUG_WFBCPeople_mode == 0 ){
      // 表
      p_array = p_wk->people;
    }else{
      // 裏
      p_array = p_wk->back_people;
    }

    // もしなかったら位置から生成
    if( p_array[ DEBUG_WFBCPeople_index ].data_in == FALSE ){

      p_array[ DEBUG_WFBCPeople_index ].npc_id  = DEBUG_WFBCPeople_npc_id;
      p_array[ DEBUG_WFBCPeople_index ].mood  = DEBUG_WFBCPeople_mood;
      p_array[ DEBUG_WFBCPeople_index ].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
      p_array[ DEBUG_WFBCPeople_index ].data_in = TRUE;

      if( p_wk->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
      {
        FIELD_WFBC_CORE_PEOPLE_SetParentData( &p_array[ DEBUG_WFBCPeople_index ], GAMEDATA_GetMyStatus(p_gdata) );
      }
      
    }else{

      p_array[ DEBUG_WFBCPeople_index ].npc_id = DEBUG_WFBCPeople_npc_id;
      p_array[ DEBUG_WFBCPeople_index ].mood  = DEBUG_WFBCPeople_mood;

    }

    DEBUGWIN_RefreshScreen();
    
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){

    // 破棄
    FIELD_WFBC_CORE_PEOPLE* p_array;

    if( DEBUG_WFBCPeople_mode == 0 ){
      // 表
      p_array = p_wk->people;
    }else{
      // 裏
      p_array = p_wk->back_people;
    }

    FIELD_WFBC_CORE_PEOPLE_Clear( &p_array[DEBUG_WFBCPeople_index] );

    
    //つめる
    if( DEBUG_WFBCPeople_mode == 0 ){
      // 表
      FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_NORMAL );
    }else{
      // 裏
      FIELD_WFBC_CORE_PackPeopleArray( p_wk, MAPMODE_INTRUDE );
    }
  

    DEBUGWIN_RefreshScreen();
    
  }else{

    // 表裏設定
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      DEBUG_WFBCPeople_mode ^= 1;
      DEBUGWIN_RefreshScreen();
    }

    // インデックス設定
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){

      FIELD_WFBC_CORE_PEOPLE* p_array;
      
      if( DEBUG_WFBCPeople_mode == 0 ){
        // 表
        p_array = p_wk->people;
      }else{
        // 裏
        p_array = p_wk->back_people;
      }

      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
        DEBUG_WFBCPeople_index = (DEBUG_WFBCPeople_index + 1) % FIELD_WFBC_PEOPLE_MAX;
        // 今のそいつの情報を引き継ぐ
        DEBUG_WFBCPeople_npc_id = p_array[ DEBUG_WFBCPeople_index ].npc_id;
        DEBUG_WFBCPeople_mood = p_array[ DEBUG_WFBCPeople_index ].mood;
        
        DEBUGWIN_RefreshScreen();
      }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
        DEBUG_WFBCPeople_index --;
        if( DEBUG_WFBCPeople_index < 0 ){
          DEBUG_WFBCPeople_index += FIELD_WFBC_PEOPLE_MAX;
        }
        // 今のそいつの情報を引き継ぐ
        DEBUG_WFBCPeople_npc_id = p_array[ DEBUG_WFBCPeople_index ].npc_id;
        DEBUG_WFBCPeople_mood = p_array[ DEBUG_WFBCPeople_index ].mood;

        DEBUGWIN_RefreshScreen();
      }
    }
    // MOOD
    else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ){
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
        DEBUG_WFBCPeople_mood = (DEBUG_WFBCPeople_mood + 20) % FIELD_WFBC_MOOD_MAX;
        DEBUGWIN_RefreshScreen();
      }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
        DEBUG_WFBCPeople_mood -= 20;
        if( DEBUG_WFBCPeople_mood < 0 ){
          DEBUG_WFBCPeople_mood += FIELD_WFBC_MOOD_MAX;
        }
        DEBUGWIN_RefreshScreen();
      }

    }
    // NPC_ID
    else{
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){
        DEBUG_WFBCPeople_npc_id = (DEBUG_WFBCPeople_npc_id + 1) % FIELD_WFBC_NPCID_MAX;
        DEBUGWIN_RefreshScreen();
      }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){
        DEBUG_WFBCPeople_npc_id --;
        if( DEBUG_WFBCPeople_npc_id < 0 ){
          DEBUG_WFBCPeople_npc_id += FIELD_WFBC_NPCID_MAX;
        }
        DEBUGWIN_RefreshScreen();
      }
    }
  }
}

static void DEBWIN_Draw_WFBCPeopleCheck( void* userWork , DEBUGWIN_ITEM* item )
{
  if( DEBUG_WFBCPeople_mode == 0 ){
    DEBUGWIN_ITEM_SetNameV( item , "N idx %d npc %d mood %d", DEBUG_WFBCPeople_index, DEBUG_WFBCPeople_npc_id, DEBUG_WFBCPeople_mood );
  }else{
    DEBUGWIN_ITEM_SetNameV( item , "B idx %d npc %d mood %d", DEBUG_WFBCPeople_index, DEBUG_WFBCPeople_npc_id, DEBUG_WFBCPeople_mood );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  市長クエストターゲットアップ
 */
//-----------------------------------------------------------------------------
static void DEBWIN_Update_BCWinTargetAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  int i;
  int add = 10;

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
      add = 100;
    }
    
    if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_RIGHT) )
    {
      p_wk->bc_npc_win_target += add;
      DEBUGWIN_RefreshScreen();
    }
    if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_LEFT) )
    {
      if( p_wk->bc_npc_win_target >= add ){
        p_wk->bc_npc_win_target -= add;
      }else{
        p_wk->bc_npc_win_target = 0;
      }
      DEBUGWIN_RefreshScreen();
    }
  }
}

static void DEBWIN_Draw_BCWinTargetAdd10( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetNameV( item , "BC WinTarget[%d]", p_wk->bc_npc_win_target );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "BC WinTarget??" );
  }
}
  
//----------------------------------------------------------------------------
/**
 *	@brief  アイテム配置
 */
//-----------------------------------------------------------------------------
static void DEBWIN_Update_WFItemSet( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_CORE* p_wk = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  FIELD_WFBC_CORE_ITEM* p_item = GAMEDATA_GetWFBCItemData( p_gdata );
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;

  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ){

    p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( DEBUG_WFBC_ITEM_npc_id, GFL_HEAP_LOWID( DEBUG_WFBC_USE_HEAP_ID ) );
    cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );

    //登録
    FIELD_WFBC_CORE_ITEM_SetItemData( p_item, cp_people_data->goods_wf, DEBUG_WFBC_ITEM_index );

    FIELD_WFBC_PEOPLE_DATA_Delete( p_people_loader );

    DEBUGWIN_RefreshScreen();
    
  }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){

      if( DEBUG_WFBC_ITEM_index>0 ){
        DEBUG_WFBC_ITEM_index --;
      }
      
      DEBUGWIN_RefreshScreen();
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){

      if( DEBUG_WFBC_ITEM_index < (FIELD_WFBC_PEOPLE_MAX-1) ){
        DEBUG_WFBC_ITEM_index ++;
      }
      DEBUGWIN_RefreshScreen();
    }
  }else{

    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){

      if( DEBUG_WFBC_ITEM_npc_id>0 ){
        DEBUG_WFBC_ITEM_npc_id --;
      }
      DEBUGWIN_RefreshScreen();
      
    }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){

      if( DEBUG_WFBC_ITEM_npc_id < (FIELD_WFBC_NPCID_MAX-1) ){
        DEBUG_WFBC_ITEM_npc_id ++;
      }
      DEBUGWIN_RefreshScreen();
    }
  }
  
}
static void DEBWIN_Draw_WFItemSet( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "ITEM Npc %d pos %d", DEBUG_WFBC_ITEM_npc_id, DEBUG_WFBC_ITEM_index );
}

//----------------------------------------------------------------------------
/**
 *	@brief  村長ポケモン選択
 */
//-----------------------------------------------------------------------------
static void DEBWIN_Update_WFPokeTargetSet( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );
  FIELD_WFBC_PEOPLE_DATA_LOAD* p_people_loader;
  const FIELD_WFBC_PEOPLE_DATA* cp_people_data;
  int i;

  if( p_core->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){

      p_people_loader = FIELD_WFBC_PEOPLE_DATA_Create( DEBUG_WFBC_GETPOKE_TARGET_npc_id, GFL_HEAP_LOWID( DEBUG_WFBC_USE_HEAP_ID ) );
      cp_people_data = FIELD_WFBC_PEOPLE_DATA_GetData( p_people_loader );

      //登録
      FIELD_WFBC_EVENT_SetWFPokeCatchEventMonsNo( p_wk, cp_people_data->enc_monsno[DEBUG_WFBC_GETPOKE_TARGET_poke_idx] );
      FIELD_WFBC_EVENT_SetWFPokeCatchEventItem( p_wk, cp_people_data->goods_wf );

      FIELD_WFBC_PEOPLE_DATA_Delete( p_people_loader );

      DEBUGWIN_RefreshScreen();

    }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){


      if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT) )
      {
        if( DEBUG_WFBC_GETPOKE_TARGET_poke_idx < FIELD_WFBC_PEOPLE_ENC_POKE_MAX-1 ){
          DEBUG_WFBC_GETPOKE_TARGET_poke_idx ++;
        }
        DEBUGWIN_RefreshScreen();
      }
      if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT) )
      {
        if( DEBUG_WFBC_GETPOKE_TARGET_poke_idx > 0 ){
          DEBUG_WFBC_GETPOKE_TARGET_poke_idx --;
        }
        DEBUGWIN_RefreshScreen();
      }
      
    }else{
    
      
      if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT) )
      {
        if( DEBUG_WFBC_GETPOKE_TARGET_npc_id < FIELD_WFBC_NPCID_MAX-1 ){
          DEBUG_WFBC_GETPOKE_TARGET_npc_id ++;
        }
        DEBUGWIN_RefreshScreen();
      }
      if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT) )
      {
        if( DEBUG_WFBC_GETPOKE_TARGET_npc_id > 0 ){
          DEBUG_WFBC_GETPOKE_TARGET_npc_id --;
        }
        DEBUGWIN_RefreshScreen();
      }
    }
  }
}

static void DEBWIN_Draw_WFPokeTargetSet( void* userWork , DEBUGWIN_ITEM* item )
{
  GAMEDATA* p_gdata = userWork;
  FIELD_WFBC_EVENT* p_wk = GAMEDATA_GetWFBCEventData( p_gdata );
  FIELD_WFBC_CORE* p_core = GAMEDATA_GetMyWFBCCoreData( p_gdata );

  if( p_core->type == FIELD_WFBC_CORE_TYPE_WHITE_FOREST )
  {
    DEBUGWIN_ITEM_SetNameV( item , "WFPoke Npc[%d] Poke[%d]", DEBUG_WFBC_GETPOKE_TARGET_npc_id, DEBUG_WFBC_GETPOKE_TARGET_poke_idx );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "WFPoke??" );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロックレベル設定
 */
//-----------------------------------------------------------------------------
static void DEBWIN_Update_WFBlockLevelSet( void* userWork , DEBUGWIN_ITEM* item )
{
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    DEUBG_WFBC_BLOCK_LEVEL --;
    if( DEUBG_WFBC_BLOCK_LEVEL < -1 ){
      DEUBG_WFBC_BLOCK_LEVEL = -1;
    }
    DEBUGWIN_RefreshScreen();
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    DEUBG_WFBC_BLOCK_LEVEL ++;
    if( DEUBG_WFBC_BLOCK_LEVEL >= FIELD_WFBC_BLOCK_PATCH_MAX  ){
      DEUBG_WFBC_BLOCK_LEVEL = FIELD_WFBC_BLOCK_PATCH_MAX - 1;
    }
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_WFBlockLevelSet( void* userWork , DEBUGWIN_ITEM* item )
{
  if( DEUBG_WFBC_BLOCK_LEVEL >= 0 )
  {
    DEBUGWIN_ITEM_SetNameV( item , "WFBLOCK Level[%d]", DEUBG_WFBC_BLOCK_LEVEL );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "WFBLOCK Level Off" );
  }
}



#endif //PM_DEBUG


