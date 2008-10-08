/*=============================================================================
/*!

	@file 	npTransform.h

	@brief	トランスフォーム定義ファイル
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.9 $
	$Date: 2006/03/28 03:56:45 $

*//*=========================================================================*/
#ifndef	NPTRANSFORM_H
#define	NPTRANSFORM_H

/*! @name	C リンケージ外対応 */
//!@{

#include "core/npTypedef.h"

//!@}

#ifdef __cplusplus
extern "C" {
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	include
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	define
//-----------------------------------------------------------------------------
/*! マトリクスパレットの最大数 */
#define	NP_INDEXMATRIX_MAX	( 2 + NP_WORLDMATRIX_MAX )

/*! ワールドインデックスマトリクスの最大数 */
#define	NP_WORLDMATRIX_MAX	( 1 )

/*!

	インデックスを対応するトランスフォームステートにマップする。

	@param	nIndex	[ 0 - NP_WORLDMATRIX_MAX ] のマトリクスインデックス

*/
#define	NP_WORLDMATRIX( nIndex )	( 2 + nIndex )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------
/*!

	@enum	トランスフォームステートの値を記述する定数

*/
enum npTRANSFORMTYPE
{
	NP_PROJECTION = 0,	//	= D3DTS_PROJECTION,
	NP_VIEW,			//	= D3DTS_VIEW,
	NP_WORLD			//	= D3DTS_WORLD
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
struct __npCONTEXT;

/*!

	@struct	ビューポート

*/
typedef struct __npVIEWPORT
{
	npU32	X;
	npU32	Y;
	npU32	Width;
	npU32	Height;
}
npVIEWPORT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	トランスフォームを設定する

	@param	pContext		レンダリングコンテキスト
	@param	nType			トランスフォームの種類( npTRANSFORMTYPE )	
	@param	pTransform		トランスフォームを変更するマトリクス

	@return	エラーコード

*/
NP_API npERROR NP_APIENTRY npSetTransformFMATRIX( 
	struct	__npCONTEXT*	pContext,
	npSTATE					nType,
	npFMATRIX*				pTransform 
);

/*!

	トランスフォームを取得する

	@param	pContext		レンダリングコンテキスト
	@param	nType			トランスフォームの種類( npTRANSFORMTYPE )	
	@param	pTransform		トランスフォームを取得するマトリクス

	@return	エラーコード

*/
NP_API npERROR NP_APIENTRY npGetTransformFMATRIX( 
	struct	__npCONTEXT*	pContext, 
	npSTATE					nType, 
	npFMATRIX*				pTransform 
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPTRANSFORM_H */
