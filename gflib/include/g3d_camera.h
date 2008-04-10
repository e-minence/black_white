//=============================================================================================
/**
 * @file	g3d_camera.h
 * @brief	カメラ設定
 * @date	
 */
//=============================================================================================
#ifndef _G3D_CAMERA_H_
#define _G3D_CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================================
//	型宣言
//=============================================================================================
typedef struct _GFL_G3D_CAMERA	GFL_G3D_CAMERA;

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
 * カメラ作成
 *
 * @param	type		射影タイプ
 * @param	param1		パラメータ１（射影タイプによって異なる。g3d_system.h 参照）
 * @param	param2		パラメータ２（射影タイプによって異なる。g3d_system.h 参照）
 * @param	param3		パラメータ３（射影タイプによって異なる。g3d_system.h 参照）
 * @param	param4		パラメータ４（射影タイプによって異なる。g3d_system.h 参照）
 * @param	near		視点からnearクリップ面までの距離	
 * @param	far			視点からfarクリップ面までの距離	
 * @param	scaleW		ビューボリュームの精度調整パラメータ（使用しないときは0）
 * @param	camPos		カメラ位置ベクトルポインタ
 * @param	camUp		カメラの上方向のベクトルへのポインタ
 * @param	target		カメラ焦点へのポインタ
 *
 * @return	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_Create
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW,
					const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target,
						HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * 一般的な透視射影カメラ作成(inline)
 *
 * @param	fovy		縦(Y)方向の視界角度
 * @param	camPos		カメラ位置ベクトルポインタ
 * @param	camUp		カメラの上方向のベクトルへのポインタ
 * @param	target		カメラ焦点へのポインタ
 *
 * @return	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
#define defaultCameraFovy	( 40 )
#define defaultCameraAspect	( FX32_ONE * 4 / 3 )
#define defaultCameraNear	( 1 << FX32_SHIFT )
#define defaultCameraFar	( 1024 << FX32_SHIFT )

inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateDefault
		( const VecFx32* camPos, const VecFx32* target, HEAPID heapID )
{
	VecFx32 defaultCameraUp = { 0, FX32_ONE, 0 };
 
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
									defaultCameraAspect, 0,
									defaultCameraNear, defaultCameraFar, 0,
									camPos, &defaultCameraUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * 視界角度による透視射影カメラ作成(inline)
 *
 * @param	fovy		縦(Y)方向の視界角度
 * @param	aspect		縦横比：幅／高さ×FX32_ONE
 * @param	near		視点からnearクリップ面までの距離	
 * @param	far			視点からfarクリップ面までの距離	
 * @param	scaleW		ビューボリュームの精度調整パラメータ（使用しないときは0）
 * @param	camPos		カメラ位置ベクトルポインタ
 * @param	camUp		カメラの上方向のベクトルへのポインタ
 * @param	target		カメラ焦点へのポインタ
 *
 * @return	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreatePerspective
		( const u16 fovy, const fx32 aspect, const fx32 near, const fx32 far, const fx32 scaleW,
			const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJPERS, 
									FX_SinIdx( fovy/2 *PERSPWAY_COEFFICIENT ),
									FX_CosIdx( fovy/2 *PERSPWAY_COEFFICIENT ),
									aspect, 0, near, far, scaleW,
									camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * クリップ座標指定による透視射影カメラ作成(inline)
 *
 * @param	top			nearクリップ面上辺のY座標
 * @param	bottom		nearクリップ面下辺のY座標
 * @param	left		nearクリップ面左辺のY座標
 * @param	right		nearクリップ面右辺のY座標
 * @param	near		視点からnearクリップ面までの距離	
 * @param	far			視点からfarクリップ面までの距離	
 * @param	scaleW		ビューボリュームの精度調整パラメータ（使用しないときは0）
 * @param	camPos		カメラ位置ベクトルポインタ
 * @param	camUp		カメラの上方向のベクトルへのポインタ
 * @param	target		カメラ焦点へのポインタ
 *
 * @return	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateFrustum
		( const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			const fx32 near, const fx32 far, const fx32 scaleW,
				const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJFRST, top, bottom, left, right,
									near, far, scaleW, camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * クリップ座標指定による正射影カメラ作成(inline)
 *
 * @param	top			nearクリップ面上辺のY座標
 * @param	bottom		nearクリップ面下辺のY座標
 * @param	left		nearクリップ面左辺のY座標
 * @param	right		nearクリップ面右辺のY座標
 * @param	near		視点からnearクリップ面までの距離	
 * @param	far			視点からfarクリップ面までの距離	
 * @param	scaleW		ビューボリュームの精度調整パラメータ（使用しないときは0）
 * @param	camPos		カメラ位置ベクトルポインタ
 * @param	camUp		カメラの上方向のベクトルへのポインタ
 * @param	target		カメラ焦点へのポインタ
 *
 * @return	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
inline GFL_G3D_CAMERA*
	GFL_G3D_CAMERA_CreateOrtho
		( const fx32 top, const fx32 bottom, const fx32 left, const fx32 right, 
			const fx32 near, const fx32 far, const fx32 scaleW,
				const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target, HEAPID heapID )
{
	return GFL_G3D_CAMERA_Create(	GFL_G3D_PRJORTH, top, bottom, left, right,
									near, far, scaleW, camPos, camUp, target, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * カメラ破棄
 *
 * @param	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_CAMERA_Delete
		( GFL_G3D_CAMERA* g3Dcamera );

//--------------------------------------------------------------------------------------------
/**
 * カメラ反映
 *
 * @param	g3Dcamera	カメラハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_CAMERA_Switching
		( GFL_G3D_CAMERA* g3Dcamera );

//--------------------------------------------------------------------------------------------
/**
 * カメラ座標の取得と変更
 *
 * @param	g3Dcamera	カメラハンドル
 * @param	pos			座標ベクトルの格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetPos( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos );
extern void GFL_G3D_CAMERA_SetPos( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camPos );
//--------------------------------------------------------------------------------------------
/**
 * カメラ上方向の取得と変更
 *
 * @param	g3Dcamera	カメラハンドル
 * @param	pos			上方向ベクトルの格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetCamUp( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp );
extern void GFL_G3D_CAMERA_SetCamUp( GFL_G3D_CAMERA* g3Dcamera, VecFx32* camUp );
//--------------------------------------------------------------------------------------------
/**
 * カメラ注視点の取得と変更
 *
 * @param	g3Dcamera	カメラハンドル
 * @param	target		注視点ベクトルの格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetTarget( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target );
extern void GFL_G3D_CAMERA_SetTarget( GFL_G3D_CAMERA* g3Dcamera, VecFx32* target );
//--------------------------------------------------------------------------------------------
/**
 * ニアファークリップの取得と変更
 *
 * @param	g3Dcamera	カメラハンドル
 * @param	near or far	ニアファー値の格納もしくは参照ワークポインタ	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_CAMERA_GetNear( GFL_G3D_CAMERA* g3Dcamera, fx32* near );
extern void GFL_G3D_CAMERA_SetNear( GFL_G3D_CAMERA* g3Dcamera, fx32* near );
extern void GFL_G3D_CAMERA_GetFar( GFL_G3D_CAMERA* g3Dcamera, fx32* far );
extern void GFL_G3D_CAMERA_SetFar( GFL_G3D_CAMERA* g3Dcamera, fx32* far );

//--------------------------------------------------------------------------------------------
/**
 * カメラ方向と対象位置との内積を取得する
 *
 * @param	g3Dcamera	カメラハンドル
 * @param	objPos		オブジェクト位置ベクトル
 *
 * 主に簡易カリングなどに使用する
 * 内積の値が　	0	:対象は水平（カメラの向きに対して垂直のベクトル）に位置する
 *				正	:対象は前方に位置する
 *				負	:対象は後方に位置する
 */
//--------------------------------------------------------------------------------------------
//正規版
extern fx32
	GFL_G3D_CAMERA_GetDotProduct
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos );

//簡易版　※２Ｄ（ＸＺ座標）専用。スカラー精度は落ちるがやや高速
extern int
	GFL_G3D_CAMERA_GetDotProductXZfast
		( GFL_G3D_CAMERA* g3Dcamera, VecFx32* objPos );

#ifdef __cplusplus
}/* extern "C" */
#endif


#endif

