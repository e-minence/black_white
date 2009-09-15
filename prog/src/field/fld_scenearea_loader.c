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

#include "fld_scenearea_loader_func.h"
#include "fld_scenearea_loader.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

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
};

//-----------------------------------------------------------------------------
//	各種コールバック関数
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_FxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );


static FLD_SCENEAREA_CHECK_AREA_FUNC* sp_FLD_SCENEAREA_CHECK_AREA_FUNC[FLD_SCENEAREA_AREACHECK_MAX] = 
{
	C3_SCENEAREA_CheckArea,
};

static FLD_SCENEAREA_UPDATA_FUNC* sp_FLD_SCENEAREA_UPDATA_FUNC[FLD_SCENEAREA_UPDATE_MAX] = 
{
	C3_SCENEAREA_Update,
	C3_SCENEAREA_FxCamera,
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
FLD_SCENEAREA_LOADER* FLD_SCENEAREA_LOADER_Create( u32 heapID )
{
	FLD_SCENEAREA_LOADER* p_sys;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SCENEAREA_LOADER) );
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
	FLD_SCENEAREA_LOADER_Clear( p_sys );
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
void FLD_SCENEAREA_LOADER_Load( FLD_SCENEAREA_LOADER* p_sys, u32 datano, u32 heapID )
{
	u32 size;
	
	GF_ASSERT( p_sys );

	p_sys->p_data = GFL_ARC_UTIL_LoadEx( ARCID_SCENEAREA_DATA, datano, FALSE, heapID, &size );
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

	FIELD_CAMERA_GetTargetPos( p_camera, &target);
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  // 方向ベクトルから、カメラangleを求める
  camera_pos.y = FX_Mul( FX_SinIdx( cp_param->pitch ), cp_param->length );
  xz_dist      = FX_Mul( FX_CosIdx( cp_param->pitch ), cp_param->length );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target_y;
  camera_pos.z += target.z;
  
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

