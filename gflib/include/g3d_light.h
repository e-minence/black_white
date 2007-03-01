//=============================================================================================
/**
 * @file	g3d_light.h
 * @brief	ライト設定
 * @date	
 */
//=============================================================================================
#ifndef _G3D_LIGHT_H_
#define _G3D_LIGHT_H_

//=============================================================================================
//	型宣言
//=============================================================================================
typedef struct _GFL_G3D_LIGHTSET	GFL_G3D_LIGHTSET;

typedef struct {
	const u8				num;
	const GFL_G3D_LIGHT		data;
}GFL_G3D_LIGHT_DATA;

typedef struct {
	const GFL_G3D_LIGHT_DATA*	lightData;
	const u8					lightCount;
}GFL_G3D_LIGHTSET_SETUP;

//=============================================================================================
/**
 *
 *
 * 管理システム
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ライトセット作成
 *
 * @param	light		ライト設定データポインタ
 * @param	lightCount	ライト数
 *
 * @return	g3Dlightset	ライトセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_LIGHTSET*
	GFL_G3D_LightCreate
		( const GFL_G3D_LIGHTSET_SETUP* setUp, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ライトセット破棄
 *
 * @param	g3Dlightset	ライトセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_LightDelete
		( GFL_G3D_LIGHTSET* g3Dlightset );

//--------------------------------------------------------------------------------------------
/**
 * ライトセット反映
 *
 * @param	g3Dlightset	ライトセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_LightSwitching
		( GFL_G3D_LIGHTSET* g3Dlightset );

//--------------------------------------------------------------------------------------------
/**
 * ライト方向の取得と変更
 *
 * @param	g3Dlightset	ライトセットハンドル
 * @param	idx			ライトＩＮＤＥＸ
 * @param	vec			ライトベクトルの格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_LightVecGet( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec );
extern void GFL_G3D_LightVecSet( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec );

//--------------------------------------------------------------------------------------------
/**
 * ライト色の取得と変更
 *
 * @param	g3Dlightset	ライトセットハンドル
 * @param	idx			ライトＩＮＤＥＸ
 * @param	color		ライト色の格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_LightColorGet( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color );
extern void GFL_G3D_LightColorSet( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color );

#endif


