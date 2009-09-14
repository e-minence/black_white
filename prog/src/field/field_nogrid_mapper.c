//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_nogrid_mapper.c
 *	@brief  ノーグリッド動作　マッパー
 *	@author	tomoya takahashi
 *	@date		2009.08.25
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc/fieldmap/field_rail_data.naix"

#include "field_nogrid_mapper.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	アトリビュート情報開始インデックス
//=====================================
#define FIELD_NOGRID_MAPPER_ARC_ATTR_INDEX_START  ( NARC_field_rail_data_c3_atdat )


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ノーグリッド動作マッパーワーク
//=====================================
struct _FLDNOGRID_MAPPER 
{
  // 各モジュール
  FIELD_RAIL_MAN*       p_railMan;
  FIELD_RAIL_LOADER*    p_railLoader;
  RAIL_ATTR_DATA*       p_attr;
  FLD_SCENEAREA*        p_areaMan;
  FLD_SCENEAREA_LOADER* p_areaLoader;

#ifdef PM_DEBUG
  BOOL debug_rail_camera_stop;
#endif
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッド動作マッパー　ワーク生成
 *
 *	@param	heapID   ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
FLDNOGRID_MAPPER* FLDNOGRID_MAPPER_Create( u32 heapID, FIELD_CAMERA* p_camera )
{
  FLDNOGRID_MAPPER* p_mapper;

  p_mapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDNOGRID_MAPPER) );

  // 各モジュールワーク生成
  p_mapper->p_railMan       = FIELD_RAIL_MAN_Create( heapID, FIELD_RAIL_WORK_MAX, p_camera ); 
  p_mapper->p_railLoader    = FIELD_RAIL_LOADER_Create( heapID );
  p_mapper->p_areaMan       = FLD_SCENEAREA_Create( heapID, p_camera );
  p_mapper->p_areaLoader    = FLD_SCENEAREA_LOADER_Create( heapID );
  p_mapper->p_attr          = RAIL_ATTR_Create( heapID );

  return p_mapper;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッド動作マッパー破棄
 *
 *	@param	p_mapper  マッパーワーク
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Delete( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );

  RAIL_ATTR_Delete( p_mapper->p_attr );
  FLD_SCENEAREA_Delete( p_mapper->p_areaMan );
  FLD_SCENEAREA_LOADER_Delete( p_mapper->p_areaLoader );
  FIELD_RAIL_MAN_Delete( p_mapper->p_railMan );
  FIELD_RAIL_LOADER_Delete( p_mapper->p_railLoader );

  GFL_HEAP_FreeMemory( p_mapper );
}



//----------------------------------------------------------------------------
/**
 *	@brief  データの反映
 *
 *	@param	p_mapper  マッパー
 *	@param	cp_data   データ
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_ResistData( FLDNOGRID_MAPPER* p_mapper, const FLDNOGRID_RESISTDATA* cp_data, u32 heapID )
{
  GF_ASSERT( p_mapper );
  
  // 破棄処理
  FLDNOGRID_MAPPER_Release( p_mapper );

  // 情報ロード＆構築
  if( cp_data->railDataID != FLDNOGRID_RESISTDATA_NONE )
  {
    FIELD_RAIL_LOADER_Load( p_mapper->p_railLoader, cp_data->railDataID, heapID );
    FIELD_RAIL_MAN_Load( p_mapper->p_railMan, FIELD_RAIL_LOADER_GetData( p_mapper->p_railLoader ) );

    RAIL_ATTR_Load( p_mapper->p_attr, cp_data->railDataID + FIELD_NOGRID_MAPPER_ARC_ATTR_INDEX_START, heapID );
  }
  if( cp_data->areaDataID != FLDNOGRID_RESISTDATA_NONE )
  {
    FLD_SCENEAREA_LOADER_Load( p_mapper->p_areaLoader, cp_data->areaDataID, heapID );
    FLD_SCENEAREA_Load( p_mapper->p_areaMan, 
        FLD_SCENEAREA_LOADER_GetData( p_mapper->p_areaLoader ),
        FLD_SCENEAREA_LOADER_GetDataNum( p_mapper->p_areaLoader ),
        FLD_SCENEAREA_LOADER_GetFunc( p_mapper->p_areaLoader ) );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッド動作　情報破棄
 *
 *	@param	p_mapper  マッパー
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Release( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );

  FLD_SCENEAREA_Release( p_mapper->p_areaMan );
  FIELD_RAIL_MAN_Clear( p_mapper->p_railMan );
  FIELD_RAIL_LOADER_Clear( p_mapper->p_railLoader );
  FLD_SCENEAREA_LOADER_Clear( p_mapper->p_areaLoader );
  RAIL_ATTR_Release( p_mapper->p_attr );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッド動作  設定データの有無
 *
 *	@param	cp_mapper   マッパー
 *
 *	@retval TRUE  ある
 *	@retval FALSE ない
 */
//-----------------------------------------------------------------------------
BOOL FLDNOGRID_MAPPER_IsResistData( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );

  if( FIELD_RAIL_MAN_IsLoad( cp_mapper->p_railMan ) )
  {
    return TRUE;
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  ノーグリッド動作　メイン処理
 *
 *	@param	p_mapper  マッパーワーク
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_Main( FLDNOGRID_MAPPER* p_mapper )
{
  VecFx32 rail_pos;

  // レールシステムメイン
  FIELD_RAIL_MAN_Update( p_mapper->p_railMan );
#ifdef PM_DEBUG
  if( p_mapper->debug_rail_camera_stop == FALSE )
#endif
  {
    FIELD_RAIL_MAN_UpdateCamera( p_mapper->p_railMan );
  }
  FIELD_RAIL_MAN_GetBindWorkPos( p_mapper->p_railMan, &rail_pos );
  FLD_SCENEAREA_Update( p_mapper->p_areaMan, &rail_pos );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レールワークの生成
 *
 *	@param	p_mapper  マッパー
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_WORK* FLDNOGRID_MAPPER_CreateRailWork( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );
  return FIELD_RAIL_MAN_CreateWork( p_mapper->p_railMan );
}

//----------------------------------------------------------------------------
/**
 *	@brief  レールワークの破棄
 *
 *	@param	p_mapper    マッパー
 *	@param	p_railWork  破棄するレールワーク
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DeleteRailWork( FLDNOGRID_MAPPER* p_mapper, FIELD_RAIL_WORK* p_railWork )
{
  GF_ASSERT( p_mapper );
  FIELD_RAIL_MAN_DeleteWork( p_mapper->p_railMan, p_railWork );
}



//----------------------------------------------------------------------------
/**
 *	@brief  カメラとバインドするレールワークを取得
 *
 *	@param	p_mapper      マッパー
 *	@param	p_railWork    レールワーク
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_BindCameraWork( FLDNOGRID_MAPPER* p_mapper, const FIELD_RAIL_WORK* cp_railWork )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( cp_railWork );
  FIELD_RAIL_MAN_BindCamera( p_mapper->p_railMan, cp_railWork );
}

//----------------------------------------------------------------------------
/**
 *	@brief  カメラとのバインドを破棄
 *
 *	@param	p_mapper  マッパー
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_UnBindCameraWork( FLDNOGRID_MAPPER* p_mapper )
{
  GF_ASSERT( p_mapper );
  FIELD_RAIL_MAN_UnBindCamera( p_mapper->p_railMan );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レールアトリビュート情報の取得
 *
 *	@param	cp_mapper     マッパー
 *	@param	cp_location   レールロケーション
 *
 *	@return マップアトリビュート
 */
//-----------------------------------------------------------------------------
MAPATTR FLDNOGRID_MAPPER_GetAttr( const FLDNOGRID_MAPPER* cp_mapper, const RAIL_LOCATION* cp_location )
{
  GF_ASSERT( cp_mapper );
  return RAIL_ATTR_GetAttribute( cp_mapper->p_attr, cp_location );
}


//----------------------------------------------------------------------------
/**
 *	@brief  レールマネージャモジュールの取得
 *
 *	@param	cp_mapper   マッパー
 *
 *	@return モジュール
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_MAN* FLDNOGRID_MAPPER_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_railMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  エリアマネージャモジュールの取得
 *
 *	@param	cp_mapper   マッパー
 *
 *	@return モジュール
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA* FLDNOGRID_MAPPER_GetSceneAreaMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_areaMan;
}

//----------------------------------------------------------------------------
/**
 *	@brief  レールローダーモジュールの取得
 *
 *	@param	cp_mapper   マッパー
 *
 *	@return モジュール
 */
//-----------------------------------------------------------------------------
const FIELD_RAIL_LOADER* FLDNOGRID_MAPPER_GetRailLoader( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_railLoader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  エリアローダーのモジュール取得
 *
 *	@param	cp_mapper   マッパー
 *
 *	@return モジュール
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_LOADER* FLDNOGRID_MAPPER_GetSceneAreaLoader( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_areaLoader;
}

//----------------------------------------------------------------------------
/**
 *	@brief  アトリビュートデータの取得
 *
 *	@param	cp_mapper   マッパー
 *
 *	@return アトリビュートデータ
 */
//-----------------------------------------------------------------------------
const RAIL_ATTR_DATA* FLDNOGRID_MAPPER_GetRailAttrData( const FLDNOGRID_MAPPER* cp_mapper )
{
  GF_ASSERT( cp_mapper );
  return cp_mapper->p_attr;
}


// デバック用
#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  レールbinaryの読み込み
 *
 *	@param	p_mapper    マッパー
 *	@param	p_dat       データ
 *	@param	size        データサイズ
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadRailBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( p_dat );

  FIELD_RAIL_LOADER_Clear( p_mapper->p_railLoader );
  FIELD_RAIL_LOADER_DEBUG_LoadBinary( p_mapper->p_railLoader, p_dat, size );

  FIELD_RAIL_MAN_Clear( p_mapper->p_railMan );
	FIELD_RAIL_MAN_Load( p_mapper->p_railMan, FIELD_RAIL_LOADER_GetData( p_mapper->p_railLoader ) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  エリアbinaryの読み込み
 *
 *	@param	p_mapper  マッパー
 *	@param	p_dat     データ
 *	@param	size      データサイズ
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadAreaBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size )
{
  GF_ASSERT( p_mapper );
  GF_ASSERT( p_dat );

  FLD_SCENEAREA_LOADER_Clear( p_mapper->p_areaLoader );
  FLD_SCENEAREA_LOADER_LoadBinary( p_mapper->p_areaLoader, p_dat, size );


	FLD_SCENEAREA_Release( p_mapper->p_areaMan );
	FLD_SCENEAREA_Load( p_mapper->p_areaMan, 
			FLD_SCENEAREA_LOADER_GetData( p_mapper->p_areaLoader ),
			FLD_SCENEAREA_LOADER_GetDataNum( p_mapper->p_areaLoader ),
			FLD_SCENEAREA_LOADER_GetFunc( p_mapper->p_areaLoader ) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  アトリビュートbinaryの読み込み
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_LoadAttrBynary( FLDNOGRID_MAPPER* p_mapper, void* p_dat, u32 size, u32 heapID )
{
  RAIL_ATTR_Release( p_mapper->p_attr );
  
  RAIL_ATTR_DEBUG_LoadBynary( p_mapper->p_attr, p_dat, size, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  アクティブ設定
 *
 *	@param	p_mapper    マッパー
 *	@param	flag        フラグ
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_SetActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag )
{
  GF_ASSERT( p_mapper );
	FIELD_RAIL_MAN_SetActiveFlag(p_mapper->p_railMan, flag);
	FLD_SCENEAREA_SetActiveFlag(p_mapper->p_areaMan, flag);
}


//----------------------------------------------------------------------------
/**
 *	@brief  アクティブ状態かチェック
 *
 *	@param	cp_mapper   マッパー
 *
 *	@retval TRUE    アクティブ
 *	@retval FALSE   非アクティブ
 */
//-----------------------------------------------------------------------------
BOOL FLDNOGRID_MAPPER_DEBUG_IsActive( const FLDNOGRID_MAPPER* cp_mapper )
{
  BOOL result;
  BOOL result2;
  
  GF_ASSERT( cp_mapper );

  result = FIELD_RAIL_MAN_GetActiveFlag( cp_mapper->p_railMan );

  if( FLD_SCENEAREA_IsLoad( cp_mapper->p_areaMan ) )
  {
    result2 = FLD_SCENEAREA_GetActiveFlag( cp_mapper->p_areaMan );

    GF_ASSERT( result2 == result );
  }

  return result;
}


//----------------------------------------------------------------------------
/**
 *	@brief  レールカメラを実行するか設定
 *
 *	@param	p_mapper  マッパー
 *	@param	flag      フラグ
 */
//-----------------------------------------------------------------------------
void FLDNOGRID_MAPPER_DEBUG_SetRailCameraActive( FLDNOGRID_MAPPER* p_mapper, BOOL flag )
{
  if( flag )
  {
    p_mapper->debug_rail_camera_stop = FALSE;
  }
  else
  {
    p_mapper->debug_rail_camera_stop = TRUE;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  自由にアクセスできるレールマネージャを取得
 *
 *	@param	cp_mapper 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
FIELD_RAIL_MAN* FLDNOGRID_MAPPER_DEBUG_GetRailMan( const FLDNOGRID_MAPPER* cp_mapper )
{
  return cp_mapper->p_railMan;
}


#endif



