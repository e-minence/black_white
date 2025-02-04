//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea_loader.c
 *	@brief  フィールド　特殊シーン領域外部データ読み込み
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	モジュール名：FLD_SCENEAREA_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc_def.h"


#include "field/field_const.h"
#include "fldmmdl.h"

#include "fieldmap.h"

#include "fld_scenearea_loader_func.h"
#include "fld_scenearea_loader.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	
//=====================================
static const u32 sc_ARCID[ FLD_SCENEAREA_LOADTYPE_MAX ] = {
  ARCID_SCENEAREA_DATA,
  ARCID_GRID_CAMERA_SCENE,
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	読み込みシステム
//=====================================
struct _FLD_SCENEAREA_LOADER {
	void* p_data;
	u32 num;
  ARCHANDLE* arcHandle[ FLD_SCENEAREA_LOADTYPE_MAX ];
};

//-----------------------------------------------------------------------------
//	各種コールバック関数
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_FxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );


static BOOL SCENEAREA_CheckGridRect( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleChange_IN( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleChange_OUT( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_GridAngleGetOfsAndMax( const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param, const VecFx32* cp_pos, fx32* p_dist, fx32* p_sub_num );


static void SCENEAREA_UpdateGridAngleOffsChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleOffsChange_IN( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleOffsChange_OUT( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleOffsChange_SetOffs( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleOffsChange_SetFovy( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );


static FLD_SCENEAREA_CHECK_AREA_FUNC* sp_FLD_SCENEAREA_CHECK_AREA_FUNC[FLD_SCENEAREA_AREACHECK_MAX] = 
{
	C3_SCENEAREA_CheckArea,

  SCENEAREA_CheckGridRect,
};

static FLD_SCENEAREA_UPDATA_FUNC* sp_FLD_SCENEAREA_UPDATA_FUNC[FLD_SCENEAREA_UPDATE_MAX] = 
{
	C3_SCENEAREA_Update,
	C3_SCENEAREA_FxCamera,

  SCENEAREA_UpdateGridAngleChange,
  SCENEAREA_UpdateGridAngleChange_IN,
  SCENEAREA_UpdateGridAngleChange_OUT,

  SCENEAREA_UpdateGridAngleOffsChange,
  SCENEAREA_UpdateGridAngleOffsChange_IN,
  SCENEAREA_UpdateGridAngleOffsChange_OUT,
};

static const FLD_SCENEAREA_FUNC sc_FLD_SCENEAREA_FUNC = 
{
	sp_FLD_SCENEAREA_CHECK_AREA_FUNC,
	sp_FLD_SCENEAREA_UPDATA_FUNC,
	FLD_SCENEAREA_AREACHECK_MAX,
	FLD_SCENEAREA_UPDATE_MAX,
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/**
 *	@brief	シーンエリアデータローダー　生成
 *
 *	@param	heapID	ヒープID
 *
 *	@return	ローダー
 */
//-----------------------------------------------------------------------------
FLD_SCENEAREA_LOADER* FLD_SCENEAREA_LOADER_Create( HEAPID heapID )
{
	FLD_SCENEAREA_LOADER* p_sys;
  int i;

  p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SCENEAREA_LOADER) );

  for( i=0; i<FLD_SCENEAREA_LOADTYPE_MAX; i++ ){
    p_sys->arcHandle[i] = GFL_ARC_OpenDataHandle( sc_ARCID[i], heapID );
  }
  
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーンエリアローダー	破棄
 *
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Delete( FLD_SCENEAREA_LOADER* p_sys )
{
  int i;
  
	FLD_SCENEAREA_LOADER_Clear( p_sys );
  for( i=0; i<FLD_SCENEAREA_LOADTYPE_MAX; i++ ){
    GFL_ARC_CloseDataHandle( p_sys->arcHandle[i] );
  }
	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	シーンエリアローダー　読み込み
 *
 *	@param	p_sys			システム
 *	@param	datano		データナンバー
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Load( FLD_SCENEAREA_LOADER* p_sys, FLD_SCENEAREA_LOADTYPE loadtype, u32 datano, HEAPID heapID )
{
	u32 size;
	
	GF_ASSERT( p_sys );
  GF_ASSERT( loadtype < FLD_SCENEAREA_LOADTYPE_MAX );

	p_sys->p_data = GFL_ARCHDL_UTIL_LoadEx( p_sys->arcHandle[loadtype], datano, FALSE, heapID, &size );
	p_sys->num		= size / sizeof(FLD_SCENEAREA_DATA);

	TOMOYA_Printf( "SCENEAREA_size = %d\n", size );
	TOMOYA_Printf( "SCENEAREA num = %d\n",  p_sys->num );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ARCIDも指定するバージョン
 *
 *	@param	p_sys			システム
 *	@param	arcID     アーカイブID
 *	@param	datano		データナンバー
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_LoadOriginal( FLD_SCENEAREA_LOADER* p_sys, u32 arcID, u32 datano, HEAPID heapID )
{
	u32 size;
	
	GF_ASSERT( p_sys );

	p_sys->p_data = GFL_ARC_UTIL_LoadEx( arcID, datano, FALSE, heapID, &size );
	p_sys->num		= size / sizeof(FLD_SCENEAREA_DATA);

	TOMOYA_Printf( "SCENEAREA_size = %d\n", size );
	TOMOYA_Printf( "SCENEAREA num = %d\n",  p_sys->num );
}

//----------------------------------------------------------------------------
/**
 *	@brief	クリア処理
 *
 *	@param	p_sys		システム
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Clear( FLD_SCENEAREA_LOADER* p_sys )
{
	GF_ASSERT( p_sys );

	if( p_sys->p_data )
	{
		GFL_HEAP_FreeMemory( p_sys->p_data );
		p_sys->p_data = NULL;
		p_sys->num		= 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーンエリア情報の取得
 *
	*	@param	cp_sys	システム
 *
 *	@return	情報
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_DATA* FLD_SCENEAREA_LOADER_GetData( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーンエリア情報数の取得
 *
 *	@param	cp_sys	システム
 *
	*	@return	情報数
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_LOADER_GetDataNum( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	関数ポインタテーブルの取得
 *
 *	@param	cp_sys	システム
 *
 *	@return	テーブル
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_FUNC* FLD_SCENEAREA_LOADER_GetFunc( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return &sc_FLD_SCENEAREA_FUNC;
}


// デバック機能
#ifdef PM_DEBUG
void FLD_SCENEAREA_LOADER_LoadBinary( FLD_SCENEAREA_LOADER* p_sys, void* p_dat, u32 size )
{
	GF_ASSERT( p_sys );

	p_sys->p_data = p_dat;
	p_sys->num		= size / sizeof(FLD_SCENEAREA_DATA);

	// あまりがない？
	GF_ASSERT( (size % sizeof(FLD_SCENEAREA_DATA)) == 0 );

	TOMOYA_Printf( "SCENEAREA num = %d\n",  p_sys->num );
}

//----------------------------------------------------------------------------
/**
 *	@brief	scenearea情報の取得
 *
	*	@param	cp_sys	わーく
 *
 *	@return	情報
 */
//-----------------------------------------------------------------------------
void* FLD_SCENEAREA_LOADER_DEBUG_GetData( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );

	return cp_sys->p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	scenearea情報サイズのシュトク
 *
 *	@param	cp_sys	ワーク
 *
 *	@return	サイズ
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );

	return cp_sys->num * sizeof(FLD_SCENEAREA_DATA);
}

#endif





//-----------------------------------------------------------------------------
/**
 *				各コールバック関数郡
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  シーンエリア情報  エリア判定
 *
 *	@param	cp_sys      システム
 *	@param	cp_data     データ
 *	@param	cp_pos      自機位置
 *
 *	@retval TRUE    エリア内
 *	@retval FALSE   エリア外
 */
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 npos;
  fx32 dist;
  u32 rotate;
  VecFx32 target;
  const FLD_SCENEAREA_CIRCLE_PARAM* cp_param = (FLD_SCENEAREA_CIRCLE_PARAM*)cp_data->area;

  VEC_Set( &target, cp_param->center_x, cp_param->center_y, cp_param->center_z );
  

  VEC_Subtract( cp_pos, &target, &npos );
  npos.y  = 0;
  dist    = VEC_Mag( &npos );
  VEC_Normalize( &npos, &npos );
  rotate  = FX_Atan2Idx( npos.x, npos.z );
  /*
  VEC_CrossProduct( &npos, &normal_vec, &normal );
  if( normal.y < 0 ){
    rotate = 0x10000 - rotate;
  }*/

  // エリア内判定
  if( (cp_param->dist_min <= dist) && (cp_param->dist_max > dist) ){
    // rot_end - rot_startが180度以上なら、０を通過する１８０の角度
    if( cp_param->rot_end < cp_param->rot_start )
    {
//      TOMOYA_Printf( "rot_end %d rot_start %d rotate %d\n", cp_param->rot_end, cp_param->rot_start, rotate );
      if( ((cp_param->rot_end >= rotate) && (rotate >= 0)) ||
          ((0x10000 > rotate) && (rotate >= cp_param->rot_start)) )
      {
        return TRUE;
      }
    }
    else
    {
      if( (cp_param->rot_start <= rotate) && (cp_param->rot_end > rotate) )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  更新処理
 *
 *	@param	cp_sys      システム
 *	@param	cp_data     データ
 *	@param	cp_pos      位置情報
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 pos, target, n0, camera_pos;
  FIELD_CAMERA* p_camera;
  fx32 xz_dist;
  fx32 target_y;
  const FLD_SCENEAREA_CIRCLE_PARAM* cp_param = (const FLD_SCENEAREA_CIRCLE_PARAM*)cp_data->area;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  target.x = cp_param->center_x;
  target.y = cp_param->center_y;
  target.z = cp_param->center_z;
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  target.y = target_y;

  // 方向ベクトルから、カメラangleを求める
  camera_pos.y = FX_Mul( FX_SinIdx( cp_param->pitch ), cp_param->length );
  xz_dist      = FX_Mul( FX_CosIdx( cp_param->pitch ), cp_param->length );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target.y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetTargetPos( p_camera, &target );
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  固定カメラの　設定処理
 *  
 *	@param	cp_sys    システムワーク
 *	@param	cp_data   データ
 *	@param	cp_pos    位置情報
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_FxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  const FLD_SCENEAREA_CIRCLE_FIXCAMERA_PARAM* cp_param = (const FLD_SCENEAREA_CIRCLE_FIXCAMERA_PARAM*)cp_data->area;
  VecFx32 target, camera_pos;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  // ターゲット位置設定
  VEC_Set( &target, cp_param->target_x, cp_param->target_y, cp_param->target_z );
  VEC_Set( &camera_pos, cp_param->camera_x, cp_param->camera_y, cp_param->camera_z );
  FIELD_CAMERA_SetTargetPos( p_camera, &target );
  FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}



//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　矩形チェック
 *
 *	@param	cp_sys      システム
 *	@param	cp_data     データ
 *	@param	cp_pos      位置
 *
 *	@retval TRUE  ヒット* 
 *	@retval FALSE 外野  
 */
//-----------------------------------------------------------------------------
static BOOL SCENEAREA_CheckGridRect( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;
  u16 grid_x, grid_z, grid_sizx, grid_sizz;
  VecFx32 local_pos;
  const FIELDMAP_WORK* cp_fieldmap = FLD_SCENEAREA_GetFieldMapWork( cp_sys );

  // ベースシステムがグリッドマップかチェック
  // グリッドの上でしか動作しない。
  if( FIELDMAP_GetBaseSystemType( cp_fieldmap ) == FLDMAP_BASESYS_RAIL ){
    return FALSE;
  }

  local_pos = *cp_pos;
  local_pos.x -= MMDL_VEC_X_GRID_OFFS_FX32;
  local_pos.z -= MMDL_VEC_Z_GRID_OFFS_FX32;

  grid_x = FX32_TO_GRID( local_pos.x );
  grid_z = FX32_TO_GRID( local_pos.z );

  // X と Zどちらかが1である必要がある
  GF_ASSERT( (cp_param->grid_sizx == 1) || (cp_param->grid_sizz == 1) );
  
  if( cp_param->grid_sizx == 1 )
  {
    grid_sizx = cp_param->grid_depth;
    grid_sizz = cp_param->grid_sizz;
  }
  else
  {
    grid_sizx = cp_param->grid_sizx;
    grid_sizz = cp_param->grid_depth;
  }

  if( (grid_x >= cp_param->grid_x) && (grid_x < cp_param->grid_x + grid_sizx) )
  {
    if( (grid_z >= cp_param->grid_z) && (grid_z < cp_param->grid_z + grid_sizz) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングルスムーズ変更
 *
 *	@param	cp_sys      システム
 *	@param	cp_data     データ
 *	@param	cp_pos      位置
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;
  FIELD_CAMERA* p_camera;
  VecFx32 target, camera_pos;
  fx32 dist;
  fx32 start_num;
  fx32 sub_num;
  s32 pitch;
  s32 yaw;
  fx32 length;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );


  //TOMOYA_Printf( "param\n" );
  //TOMOYA_Printf( "start angle pitch=0x%x yaw=0x%x len=0x%x\n", cp_param->start_pitch, cp_param->start_yaw, cp_param->start_length );
  //TOMOYA_Printf( "end angle pitch=0x%x yaw=0x%x len=0x%x\n", cp_param->end_pitch, cp_param->end_yaw, cp_param->end_length );

  // 計算用のパラメータを求める
  SCENEAREA_GridAngleGetOfsAndMax( cp_param, cp_pos, &dist, &sub_num );


  sub_num = FX_Div( sub_num, dist );


  // 移動割合からアングルと距離を求める
  pitch = (s32)cp_param->end_pitch - (s32)cp_param->start_pitch;
  pitch = (pitch * sub_num) / FX32_ONE;
  pitch += cp_param->start_pitch;

  yaw = (s32)cp_param->end_yaw - (s32)cp_param->start_yaw;
  yaw = (yaw * sub_num) / FX32_ONE;
  yaw += cp_param->start_yaw;

  length = cp_param->end_length - cp_param->start_length;
  length = FX_Mul(length, sub_num);
  length += cp_param->start_length;

  //TOMOYA_Printf( "set angle\n" );
  //TOMOYA_Printf( "pitch 0x%x yaw 0x%x len 0x%x\n", (u16)pitch, (u16)yaw, length );

  // アングルを設定
  FIELD_CAMERA_SetAnglePitch( p_camera, pitch );
  FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
  FIELD_CAMERA_SetAngleLen( p_camera, length );
}

//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングルスムーズ変更    IN
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleChange_IN( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );
  if( cp_param->trace_off ){
    FIELD_CAMERA_StopTraceNow( p_camera );
  }

  SCENEAREA_UpdateGridAngleChange( cp_sys, cp_data, cp_pos );

  //TOMOYA_Printf( "GridAngle Start\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングルスムーズ変更    OUT
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleChange_OUT( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

  SCENEAREA_UpdateGridAngleChange( cp_sys, cp_data, cp_pos );

  if( cp_param->trace_off ){
    FIELD_CAMERA_RestartTrace( p_camera );
  }

  //TOMOYA_Printf( "GridAngle End\n" );
}

static void SCENEAREA_GridAngleGetOfsAndMax( const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param, const VecFx32* cp_pos, fx32* p_dist, fx32* p_sub_num )
{
  fx32 start_num;
  VecFx32 local_pos;

  local_pos = *cp_pos;
  local_pos.x -= MMDL_VEC_X_GRID_OFFS_FX32;
  local_pos.z -= MMDL_VEC_Z_GRID_OFFS_FX32;


  // 入り口からの奥への距離から、カメラを動かす
  *p_dist = GRID_TO_FX32( cp_param->grid_depth );
  // 横？　縦？
  if( cp_param->grid_sizx == 1 )
  {
    // 右に奥行き
    start_num = GRID_TO_FX32( cp_param->grid_x );
    *p_sub_num   = local_pos.x - start_num;
  }
  else
  {
    // 手前に奥行き
    start_num = GRID_TO_FX32( cp_param->grid_z );
    *p_sub_num   = local_pos.z - start_num;
  }

}



//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングル　ターゲットオフセット　SMOOTH　変更　更新
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleOffsChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  SCENEAREA_UpdateGridAngleChange( cp_sys, cp_data, cp_pos );

  // ターゲットオフセット設定
  SCENEAREA_UpdateGridAngleOffsChange_SetOffs( cp_sys, cp_data, cp_pos );

  // FOVY設定
  SCENEAREA_UpdateGridAngleOffsChange_SetFovy( cp_sys, cp_data, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングル　ターゲットオフセット　SMOOTH　変更　IN
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleOffsChange_IN( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  SCENEAREA_UpdateGridAngleChange_IN( cp_sys, cp_data, cp_pos );
  
  // ターゲットオフセット設定
  SCENEAREA_UpdateGridAngleOffsChange_SetOffs( cp_sys, cp_data, cp_pos );

  // FOVY設定
  SCENEAREA_UpdateGridAngleOffsChange_SetFovy( cp_sys, cp_data, cp_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  グリッド　アングル　ターゲットオフセット　SMOOTH　変更　OUT
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleOffsChange_OUT( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{

  SCENEAREA_UpdateGridAngleChange_OUT( cp_sys, cp_data, cp_pos );

  // ターゲットオフセット設定
  SCENEAREA_UpdateGridAngleOffsChange_SetOffs( cp_sys, cp_data, cp_pos );

  // FOVY設定
  SCENEAREA_UpdateGridAngleOffsChange_SetFovy( cp_sys, cp_data, cp_pos );
}


//----------------------------------------------------------------------------
/**
 *	@brief  GridAngleOffs
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleOffsChange_SetOffs( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLEOFFS_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLEOFFS_PARAM*)cp_data->area;
  FIELD_CAMERA* p_camera;
  VecFx32 target_offs;
  fx32 x, y, z;
  fx32 dis_x, dis_y, dis_z;
  fx32 dist, sub_num;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

  // 計算用のパラメータを求める
  SCENEAREA_GridAngleGetOfsAndMax( (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_param, cp_pos, &dist, &sub_num );


  // 移動値を求める
  dis_x = cp_param->end_offs_x - cp_param->start_offs_x;
  dis_y = cp_param->end_offs_y - cp_param->start_offs_y;
  dis_z = cp_param->end_offs_z - cp_param->start_offs_z;
  
  target_offs.x = cp_param->start_offs_x + FX_Div( FX_Mul( dis_x, sub_num ), dist );
  target_offs.y = cp_param->start_offs_y + FX_Div( FX_Mul( dis_y, sub_num ), dist );
  target_offs.z = cp_param->start_offs_z + FX_Div( FX_Mul( dis_z, sub_num ), dist );

  FIELD_CAMERA_SetTargetOffset( p_camera, &target_offs );
}

//----------------------------------------------------------------------------
/**
 *	@brief  Fovy設定
 *
 *	@param	cp_sys
 *	@param	cp_data
 *	@param	cp_pos 
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleOffsChange_SetFovy( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLEOFFS_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLEOFFS_PARAM*)cp_data->area;
  FIELD_CAMERA* p_camera;
  s32 fovy_dif;
  u16 set_fovy;
  fx32 dist, sub_num;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

  // 計算用のパラメータを求める
  SCENEAREA_GridAngleGetOfsAndMax( (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_param, cp_pos, &dist, &sub_num );

  sub_num = FX_Div( sub_num, dist );

  // fovy
  fovy_dif = cp_param->end_fovy - cp_param->start_fovy;
  set_fovy = cp_param->start_fovy + ((fovy_dif * sub_num) / FX32_ONE);
  FIELD_CAMERA_SetFovy( p_camera, set_fovy );
}



