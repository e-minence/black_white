//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		calc_3d.h
 *	@brief		3D計算ツール
 *	@author		Toru=Nagihashi
 *	@date		2008.09.19
 *	@date		2009.03.30	gflibに移植	tomoya takahshi
 *
 *	３Dのベクトル操作や行列の作成を行う関数郡です。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __CALC_3D_H__
#define __CALC_3D_H__

#include <nnsys.h>

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ベクトルにスカラー乗算
 *
 *	@param	a	かけられるベクトル
 *	@param	b	かけるスカラー値
 *	@param	ab	求まったベクトル(a*b)
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_MulScalar( const VecFx32* p_a, fx32 b, VecFx32* p_ab );
//----------------------------------------------------------------------------
/**
 *	@brief	ベクトルにスカラー除算
 *
 *	@param	a	わられるべクトル
 *	@param	b	わるスカラー値
 *	@param	ab	求まったベクトル（a/b）
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_DivScalar( const VecFx32* p_a, fx32 b, VecFx32* p_ab );
//----------------------------------------------------------------------------
/**	
 *	@brief	ベクトルの投影演算（aとbへ投影）
 *
 *	@param	const VecFx32* cp_a	投影するベクトル
 *	@param	VecFx32* cp_b		投影元ベクトル（正規化してあること）
 *	@param	p_dst				求まったベクトル
 *
 *	使用例）
 *	　あるベクトル(A)の、主人公の向いている方向(B)での長さ(C)を求める。
 *		   A　CB
 *		       |
 *		  /   ||
 *		 /    ||
 *		/     ||
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_Project( const VecFx32* cp_a, const VecFx32* cp_b, VecFx32* p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	ベクトルの反射演算（aをb軸に対して反射）
 *
 *	@param	const VecFx32* cp_a	反射するベクトル
 *	@param	VecFx32* cp_b		軸ベクトル（正規化してあること）
 *	@param	p_dst				求まったベクトル
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_Refrect( const VecFx32* cp_a, const VecFx32* cp_b, VecFx32* p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	角度から、回転マトリクスを作成
 *			回転順番を一定にするためにできるだけこの関数を使用すること
 *
 *	@param	rot_x		X軸角度
 *	@param	rot_y		Y軸角度
 *	@param	rot_z		Z軸角度
 *	@param	p_rot_mtx	回転マトリクス
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_MTX_CreateRot( u16 rot_x, u16 rot_y, u16 rot_z, MtxFx33* p_rot_mtx );
//----------------------------------------------------------------------------
/**
 *	@brief	2D座標から3D上での方向を求める（レイトレースなどに使用する）	
 *	グローバルステートの射影行列やカメラ設定を使用します
 *
 *	@param	p_near_pos		ニアクリップ面の座標
 *	@param	p_way			方向ベクトル
 *	@param	x				入力ｘ座標（dot）
 *	@param	y				入力ｙ座標（dot） 
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_Get2DPosTo3DRay( VecFx32* p_near_pos, VecFx32* p_way, u8 x, u8 y );
//----------------------------------------------------------------------------
/**
 *	@brief	2つのベクトルのなす角のコサインを求める
 *	
 *	@param	const VecFx32 cp_a	ベクトルA
 *	@param	VecFx32 cp_b		ベクトルB
 *
 *	@return	コサイン
 *
 * ＊補足
 *	FX_AcosIdxを使用すると、コサイン値からラジアン角度を求めることが出来ます。
 */
//-----------------------------------------------------------------------------
extern fx32 GFL_CALC3D_VEC_GetCos( const VecFx32 *cp_a, const VecFx32 *cp_b );
//----------------------------------------------------------------------------
/**
 *	@brief	方向ベクトルから回転行列を求める
 *			(XZ平面基準での、角度0のときZ軸が１になる回転行列を求める)
 *
 *	@param	const VecFx32 *cp_v	方向ベクトル（正規化されていること）
 *	@param	*p_dst				うけとる行列
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_MTX_GetVecToRotMtxXZ( const VecFx32 *cp_v, MtxFx33 *p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	XZ平面上で、自分が点に対してn度角度移動したときの、自分から移動地点までのベクトルを作成
 *
 *	@param	const VecFx32 *cp_v		自分からある点までのベクトル
 *	@param	angle					点を基準に何度移動するか
 *	@param	*p_dst					自分から移動地点までのベクトル
 *
 *	使用例）
 *			.点
 *		  /	 |
 *		 /	 |
 *		/____|
 * 自分.  ↑ .移動先
 *      p_dst
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_GetRotVectorDiffXZ( const VecFx32 *cp_v, u16 angle, VecFx32 *p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	ベクトル１がベクトル２になる回転行列を作成する
 *
 *	@param	cp_v1		ベクトル１
 *	@param	cp_v2		ベクトル２
 *	@param	p_dst		回転行列格納先
 *
 *	使用例）	AがBになるための行列を作成
 *    /|
 *   / |
 *  /  |
 *  A  B
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_MTX_GetRotVecToVec( const VecFx32 *cp_v1, const VecFx32 *cp_v2, MtxFx33 *p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	逆の方向を求める
 *
 *	@param	const VecFx32 *cp_src	元になるベクトル
 *	@param	*p_dst					逆方向ベクトル受け取り
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_VEC_Inv( const VecFx32 *cp_src, VecFx32 *p_dst );
//----------------------------------------------------------------------------
/**
 *	@brief	平面上でベクトルが軸ベクトルに対して左か右かを求める
 *
 *	@param	VecFx32 cp_norm		基準となる法線ベクトル	（XZ平面だった場合(0,1,0)のベクトル）
 *	@param	VecFx32 *cp_axis	軸ベクトル
 *	@param	VecFx32 cp_v		ベクトル
 *
 *	@return	0ならばベクトルは軸ベクトルに水平、正ならば左、負ならば右
 */
//-----------------------------------------------------------------------------
extern int GFL_CALC3D_VEC_CalcRotVectorLR( const VecFx32 *cp_norm, const VecFx32 *cp_axis, const VecFx32 *cp_v );
//----------------------------------------------------------------------------
/**
 *	@brief	4x4行列に座標を掛け合わせる
 *			Vecをx,y,z,1として計算し、計算後のVecとwを返します。
 *
 *	@param	*cp_src	Vector座標
 *	@param	*cp_m	4*4行列
 *	@param	*p_dst	Vecror計算結果
 *	@param	*p_w	4つ目の要素の値
 *
 *  射影変換後の座標を求めるために、作成した関数です。
 *	p_dstのx,y,zをp_wの値で割ると、射影変換後の値になります。
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w );
//----------------------------------------------------------------------------
/**
 *	@brief	スクリーン座標変換を行う行列を作成
 *			デカルト座標からスクリーン座標への変換なので
 *			Y方向が完全に反転することに注意
 *
 *	@param	width	スクリーン横幅
 *	@param	height	スクリーン縦幅
 *	@param	z_min	Z最小値
 *	@param	z_max	Z最大値
 *	@param	*p_mtx	行列受け取り
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_MTX_Screen( fx32 width, fx32 height, fx32 z_min, fx32 z_max, MtxFx33 *p_mtx );

//----------------------------------------------------------------------------
/**
 *	@brief	4*4行列を逆行列に
 *		system/build/libraries/g3d/src/glbstate.cのinner関数を移動
 *		注意　計算量がO(N^3)ほどなので重い処理です。
 *
 *	@param	pSrc
 *	@param	* pDst
 *
 *	@return	0ならば正常終了 それ以外ならば逆行列が存在しない
 */
//-----------------------------------------------------------------------------
extern int GFL_CALC3D_MTX_Inverse44(const MtxFx44 * pSrc, MtxFx44 * pDst);


//----------------------------------------------------------------------------
/**
 *	@brief	直線上の交点を求める
 *
 *	@param  p0      直線1　始点
 *	@param  way0    直線1　方向
 *	@param  p1      直線2　始点
 *	@param  way1    直線2　方向
 *	@param  p_ans   結果格納先
 */
//-----------------------------------------------------------------------------
extern void GFL_CALC3D_GetVectorCrossPos( const VecFx32* p0, const VecFx32* way0, const VecFx32* p1, const VecFx32* way1, VecFx32* p_ans );

//----------------------------------------------------------------------------
/**
 *	@brief	交点の有無を調べる
 *
 *	@param  way0    ベクトル1
 *	@param  way1    ベクトル2
 *
 *	@retval TRUE  ある
 *	@retval FALSE ない
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_CALC3D_IsVectorCross( const VecFx32* vec0, const VecFx32* vec1 );



#ifdef __cplusplus
}/* extern "C" */
#endif

#endif		// __CALC_3D_H__

