//==============================================================================
/**
 * @file	quaternion.h
 * @brief	クォータニオン
 * @author	soga
 * @date	2008.03.26
 *
 * ここに色々な解説等を書いてもよい
 *
 */
//==============================================================================

#ifndef __QUATERNION_H
#define __QUATERNION_H

#ifdef __cplusplus
extern "C" {
#endif

/* クォータニオン構造体 */
typedef struct _GFL_QUATERNION GFL_QUATERNION;

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（メモリ確保）
 *
 * @param[in]	heapID	メモリ確保するヒープID
 */
//--------------------------------------------------------------------------------------------
extern	GFL_QUATERNION *GFL_QUAT_Init(int heapID);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン破棄
 *
 * @param[out]	qt	破棄するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Free(GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（０）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Zero(GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（１）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Identity(GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオン初期化（指定された引数）
 *
 * @param[out]	qt	初期化するクォータニオン構造体へのポインタ
 * @param[in]	w	初期化するｗ値
 * @param[in]	x	初期化するｘ値
 * @param[in]	y	初期化するｙ値
 * @param[in]	z	初期化するｚ値
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Assign(GFL_QUATERNION *qt, fx32 w, fx32 x, fx32 y, fx32 z);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオンの加算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	加算値A
 * @param[in]	qt_b	加算値B
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Add(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオンの減算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	減算値A
 * @param[in]	qt_b	減算値B
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Sub(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオンの乗算
 *
 * @param[out]	ans		計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt_a	乗算値A
 * @param[in]	qt_b	乗算値B
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_Mul(GFL_QUATERNION *ans, const GFL_QUATERNION *qt_a, const GFL_QUATERNION *qt_b);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオンと実数の乗算
 *
 * @param[out]	ans	計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	s	乗算値実数
 * @param[in]	qt	乗算値クォータニオン
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_MulReal(GFL_QUATERNION *ans, fx32 s, const GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 * クォータニオンと実数の除算
 *
 * @param[out]	ans	計算結果を返すクォータニオン構造体へのポインタ
 * @param[in]	qt	除算値クォータニオン
 * @param[in]	s	除算値実数
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_QUAT_DivReal(GFL_QUATERNION *ans, const GFL_QUATERNION *qt, fx32 s);

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンのノルム（ ||a||^2 ）
 *
 * @param[in]	qt	計算するクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
extern	fx32 GFL_QUAT_NormSqr(const GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンのノルム（ ||a|| ）
 *
 * @param[in]	qt	計算するクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
extern	fx32 GFL_QUAT_Norm(const GFL_QUATERNION *qt);

//--------------------------------------------------------------------------------------------
/**
 *	各軸の回転量からクォータニオンを生成する
 *
 * @param[out]	qt		生成したクォータニオンを格納する構造体へのポインタ
 * @param[in]	rot_x	X軸の回転量
 * @param[in]	rot_y	Y軸の回転量
 * @param[in]	rot_z	Z軸の回転量
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_QUAT_MakeQuaternionXYZ(GFL_QUATERNION *qt,u16 rot_x,u16 rot_y,u16 rot_z);

//--------------------------------------------------------------------------------------------
/**
 *	指定された2点に直行するベクトルを回転軸とするクォータニオンを生成する
 *
 * @param[out]	qt		生成したクォータニオンを格納する構造体へのポインタ
 * @param[in]	pos1_x	指定点１のX座標（0〜256）
 * @param[in]	pos1_y	指定点１のY座標（0〜192）
 * @param[in]	pos2_x	指定点２のX座標（0〜256）
 * @param[in]	pos2_y	指定点２のY座標（0〜192）
 * @param[in]	radius	回転軸の半径
 */
//--------------------------------------------------------------------------------------------
void	GFL_QUAT_MakeQuaternionPos(GFL_QUATERNION *qt,u32 pos1_x,u32 pos1_y,u32 pos2_x,u32 pos2_y,fx32 radius);

//--------------------------------------------------------------------------------------------
/**
 *	クォータニオンから回転行列を生成する
 *
 * @param[out]	mtx	生成する回転行列構造体へのポインタ
 * @param[in]	qt	元になるクォータニオン構造体のポインタ
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_QUAT_MakeRotateMatrix(MtxFx44 *mtx,GFL_QUATERNION *qt);

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
