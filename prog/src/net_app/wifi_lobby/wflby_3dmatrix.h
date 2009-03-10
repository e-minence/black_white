//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wflby_3dmatrix.h
 *	@brief		広場部屋	３D座標管理関数
 *	@author		tomoya takahashi
 *	@data		2007.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WFLBY_3DMATRIX_H__
#define __WFLBY_3DMATRIX_H__

//#include "system/d3dobj.h"
//#include "system/arc_util.h"

#include "net_app/wifi2dmap/wf2dmap_common.h"


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	グリッド３Dサイズ
//=====================================
#define WFLBY_3DMATRIX_GRID_SIZ		( FX32_ONE )
#define WFLBY_3DMATRIX_GRID_GRIDSIZ	( WF2DMAP_GRID_SIZ*FX32_ONE )
#define WFLBY_3DMATRIX_GRID_GRIDSIZ_HALF	( (WF2DMAP_GRID_SIZ/2)*FX32_ONE )

//-------------------------------------
///	床の位置
//=====================================
#define WFLBY_3DMATRIX_FLOOR_MAT	( FX32_CONST(0) )

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern void WFLBY_3DMATRIX_GetPosVec( const WF2DMAP_POS* cp_pos, VecFx32* p_vec );
extern void WFLBY_3DMATRIX_GetVecPos( const VecFx32* cp_vec, WF2DMAP_POS* p_pos );


// 汎用関数
// テクスチャ読み込み	テクスチャ転送後破棄バージョン
extern void WFLBY_3DMAPOBJ_TEX_LoatCutTex( GFL_G3D_RES** pp_in, ARCHANDLE* p_handle, u32 data_idx, u32 gheapID );
#if WB_FIX
extern BOOL WFLBY_3DMAPOBJ_MDL_BOXCheck( const GFL_G3D_OBJ* cp_mdl, const GFL_G3D_OBJSTATUS* cp_obj );
#endif




#endif		// __WFLBY_3DMATRIX_H__

