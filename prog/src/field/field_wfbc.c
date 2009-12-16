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
#define FIELD_WFBC_EVENT_WF  (NARC_eventdata_wf_block_event_bin)
#define FIELD_WFBC_EVENT_BC  (NARC_eventdata_bc_block_event_bin)


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
///	イベント起動情報
//=====================================
typedef struct 
{
  // 条件
  u32 if_para;
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

  // 人物位置情報
  FIELD_WFBC_CORE_PEOPLE_POS* p_pos;

  // 乱数データ
  // 配置ブロックの計算に使用
  // その他の用途では使用しないこと！！！
  GFL_STD_RandContext randData;
  u16 block_rand_max;
  u16 block_pattern_num;
};


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

static void WFBC_UpdateCore( FIELD_WFBC* p_wk );

static FIELD_WFBC_PEOPLE* WFBC_GetPeople( FIELD_WFBC* p_wk, u32 npc_id );
static const FIELD_WFBC_PEOPLE* WFBC_GetConstPeople( const FIELD_WFBC* cp_wk, u32 npc_id );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core, const FIELD_WFBC_CORE_PEOPLE_POS* cp_pos, const FIELD_WFBC_PEOPLE_DATA* cp_data );
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core );


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
  return WFBC_GetConstPeople( cp_wk, npc_id );
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
int FIELD_WFBC_SetUpBlock( FIELD_WFBC* p_wk, NormalVtxFormat* p_attr, FIELD_BMODEL_MAN* p_bm, GFL_G3D_MAP* g3Dmap, int build_count, u32 block_x, u32 block_z, HEAPID heapID )
{
  WFBC_BLOCK_DATA block_tag;
  const FIELD_WFBC_BLOCK_PATCH* cp_patch_data;
  FIELD_DATA_PATCH* p_patch;
  u32 local_grid_x, local_grid_z;
  u8 tbl_index;
  s32 score;

  block_tag = p_wk->p_block->block_data[ block_z ][ block_x ];
  
  if( block_tag.block_no != FIELD_WFBC_BLOCK_NONE )
  {
    TOMOYA_Printf( "block_tag %d\n", block_tag.block_no );
    TOMOYA_Printf( "block pos x %d  z %d\n", block_tag.pos_x, block_tag.pos_z );
    TOMOYA_Printf( "block_x=%d  block_z=%d\n", block_x, block_z );

    // ブロックのスコアを計算
    // ブロックの左上で計算
    {
      u32 block_lefttop_x, block_lefttop_z;
      block_lefttop_x = block_x - block_tag.pos_x;
      block_lefttop_z = block_z - block_tag.pos_z;
      
      // （（WFにいる全てのOBJのNOの合計　＋　ブロック左上のＸ座標＋　ブロック左上のY座標） / (ブロック左上のＸ座標+1) /(ブロック左上のY座標+1) +２ ）／２
      tbl_index = p_wk->block_pattern_num;
      tbl_index += block_lefttop_x + block_lefttop_z;
      tbl_index /= (block_lefttop_x+1) + (block_lefttop_z+1) + 2;
      tbl_index %= 2;

      // （OBJの人数×0.8）＋ランダム（０〜１）
      score = FIELD_WFBC_GetPeopleNum( p_wk );
      score = (score * 8) / 10;
      score += GFL_STD_Rand( &p_wk->randData, p_wk->block_rand_max );

      if( score >= (FIELD_WFBC_BLOCK_PATCH_MAX/2) )
      {
        score = (FIELD_WFBC_BLOCK_PATCH_MAX/2)-1;
      }
      
      score += tbl_index*(FIELD_WFBC_BLOCK_PATCH_MAX/2);
    }
    
    
    GF_ASSERT( score < FIELD_WFBC_BLOCK_PATCH_MAX );

    // g3Dmapローカルのグリッド座標を求める
    local_grid_x = FIELD_WFBC_BLOCK_TO_GRID_X( block_x ) % 32;
    local_grid_z = FIELD_WFBC_BLOCK_TO_GRID_Z( block_z ) % 32;
    TOMOYA_Printf( "local_grid_x %d\n", local_grid_x );
    TOMOYA_Printf( "local_grid_z %d\n", local_grid_z );

    TOMOYA_Printf( "build_count %d\n", build_count );
    
    // 設定
    cp_patch_data = &p_wk->p_block->patch[ block_tag.block_no ];

    // パッチ情報読み込み
    p_patch = FIELD_DATA_PATCH_Create( cp_patch_data->patch[ score ], GFL_HEAP_LOWID(heapID) );

    // アトリビュートのオーバーライト
    FIELD_DATA_PATCH_OverWriteAttrEx( p_patch, p_attr, block_tag.pos_x*8, block_tag.pos_z*8, local_grid_x, local_grid_z, 8, 8 );

    // 配置上書き
    if( (block_tag.pos_x == 0) && (block_tag.pos_z == 0) )
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
void FILED_WFBC_EventDataOverwrite( const FIELD_WFBC* cp_wk, EVENTDATA_SYSTEM* p_evdata, HEAPID heapID )
{
  u16 bg_num, npc_num, connect_num, pos_num;
  int i;
  FIELD_WFBC_EVENT_IF* p_event_if;
  int add_connect_num;
  u32 load_size;
  EVENTDATA_TABLE* p_eventdata;
  const CONNECT_DATA* cp_connect;
  static const u32 sc_EVIF[ FIELD_WFBC_CORE_TYPE_MAX ] = 
  {
    NARC_field_wfbc_data_bc_block_event_evp,
    NARC_field_wfbc_data_wf_block_event_evp,
  };
  static const u32 sc_EVDATA[ FIELD_WFBC_CORE_TYPE_MAX ] = 
  {
    NARC_eventdata_bc_block_event_bin,
    NARC_eventdata_wf_block_event_bin,
  };
  u32 score;

  // 今の人の数
  score = FIELD_WFBC_GetPeopleNum( cp_wk );
  

  // 今のイベント数
  bg_num      = EVENTDATA_GetBgEventNum(p_evdata);
  npc_num     = EVENTDATA_GetNpcEventNum(p_evdata);
  connect_num = EVENTDATA_GetConnectEventNum(p_evdata);
  pos_num     = EVENTDATA_GetPosEventNum(p_evdata);

  // 拡張イベント情報を取得
  load_size = GFL_ARC_GetDataSizeByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ] );
  p_event_if = GFL_ARC_LoadDataAllocByHandle( cp_wk->p_handle, sc_EVIF[ cp_wk->type ], GFL_HEAP_LOWID(heapID) );
  p_eventdata = GFL_ARC_UTIL_Load( ARCID_FLD_EVENTDATA, sc_EVDATA[ cp_wk->type ], FALSE, GFL_HEAP_LOWID(heapID) );
  
  // データサイズから、追加接続ポイントの数をチェック
  add_connect_num = load_size / sizeof(FIELD_WFBC_EVENT_IF);
  connect_num += add_connect_num;
  
  // 接続ポイントのエリアを増やしつつ再読み込み
  EVENTDATA_SYS_ReloadEventDataEx( p_evdata, bg_num, npc_num, connect_num, pos_num );

  // 接続情報の追加処理
  cp_connect = EVENTDATA_SYS_HEADER_GetConnectEvent( p_eventdata );
  // 足しこむ
  for( i=0; i<add_connect_num; i++ )
  {
    if(score > p_event_if[i].if_para)
    {
#ifdef PM_DEBUG
      {
        const CONNECT_DATA_GPOS* cp_pos = (const CONNECT_DATA_GPOS*)cp_connect[i].pos_buf;
        TOMOYA_Printf( "link zone_id %d\n", cp_connect[i].link_zone_id );
        TOMOYA_Printf( "link door_id %d\n", cp_connect[i].link_exit_id );
        TOMOYA_Printf( "exit_dir %d\n", cp_connect[i].exit_dir );
        TOMOYA_Printf( "exit_type %d\n", cp_connect[i].exit_type );
        TOMOYA_Printf( "pos_x %d\n", cp_pos->x );
        TOMOYA_Printf( "pos_z %d\n", cp_pos->z );
        TOMOYA_Printf( "siz_x %d\n", cp_pos->sizex );
        TOMOYA_Printf( "siz_z %d\n", cp_pos->sizez );
      }
#endif // PM_DEBUG
      
      // 追加
      EVENTDATA_SYS_AddConnectEvent( p_evdata, &cp_connect[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_event_if );
  GFL_HEAP_FreeMemory( p_eventdata );
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

  p_people = WFBC_GetPeople( p_wk, npc_id );
  p_people->status = FIELD_WFBC_PEOPLE_STATUS_AWAY;
 
  // セーブデータに反映
  WFBC_UpdateCore( p_wk );
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
        // ブロック補正値を求める
        random_max += cp_people_data->block_param;
        pattern_num += p_buff->people[i].npc_id;
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

        // ブロック補正値を求める
        random_max += cp_people_data->block_param;
        pattern_num += p_buff->people[i].npc_id;
      }
    }
  }

  // random初期化
  {
    GFL_STD_RandInit( &p_wk->randData, (u64)((p_buff->random_no << 32) + p_buff->random_no) );
    p_wk->block_rand_max = 2 + random_max;
    p_wk->block_pattern_num = pattern_num;
  }

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





//デバッグ操作
#ifdef PM_DEBUG
static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );

void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId, FIELD_WFBC_CORE* p_core )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             p_core , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             p_core , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  int  i;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    // 増やす
    for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
    {
      if( p_wk->people[i].data_in == FALSE )
      {
        p_wk->people[i].data_in = TRUE;
        p_wk->people[i].npc_id  = 0;
        p_wk->people[i].mood  = 50;
        p_wk->people[i].one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;
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
    DEBUGWIN_RefreshScreen();
  }
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  u16 level = FIELD_WFBC_CORE_GetPeopleNum( p_wk, MAPMODE_NORMAL );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
  u8 type = p_wk->type;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
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
  }
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
  FIELD_WFBC_CORE* p_wk = userWork;
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
#endif
