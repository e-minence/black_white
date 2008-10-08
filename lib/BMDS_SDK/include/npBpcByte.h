/*!	\defgroup GROUP_BPCBYTE バイト操作モジュール
	\brief バイト操作に関連するユーティリティです。
	\{
*/
/*=============================================================================
/*!
	\file npByte.h
	\brief	( RV )
	
	\author	F.Fujimoto
	$Author: otabe $

	$Date: 2006/03/02 01:03:10 $
	$Revision: 1.2 $
*//*=========================================================================*/
#ifndef	__BPCBYTE_H__
#define	__BPCBYTE_H__
/*
#ifdef __cplusplus
extern "C" {
#endif
*/
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	include
//-----------------------------------------------------------------------------
#include "core/npType.h"

#define NP_IMPORTER_BIGENDIAN 0
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	define
//-----------------------------------------------------------------------------
/*!	@name	バイト操作	*/
//!@{
#if NP_IMPORTER_BIGENDIAN
#define npSwapI64( d )			( ( ( d ) >> 32 ) | ( ( ( d ) & 0xffffffff ) << 32 ) )	//!< 64bit整数の上位下位32bitをスワップする
#define	npSwapI32( l )			( ( ( l ) >> 16 ) | ( ( ( l ) & 0xffff )	 << 16 ) )	//!< 32bit整数の上位下位16bitをスワップする
#define	npSwapI16( w )			( ( ( w ) >>  8 ) | ( ( ( w ) & 0xff )		 <<  8 ) )	//!< 16bit整数の上位下位8bitをスワップする
#define npHiI8( w )			( ( ( w ) >> 8 ) & 0xFF )								//!< 16bit整数の上位8bitの値を取得
#define npLoI8( w )			( ( w ) & 0xFF )										//!< 16bit整数の下位8bitの値を取得
#define npHiI16( l )			( ( ( l ) >> 16 ) & 0xFFFF )							//!< 32bit整数の上位16bitの値を取得
#define npLoI16( l )			( ( l ) & 0xFFFF )										//!< 32bit整数の下位16bitの値を取得
#define npHiI32( d )			( ( ( d ) >> 32 ) & 0xFFFFFFFF )						//!< 64bit整数の上位32bitの値を取得
#define npLoI32( d )			( ( d ) & 0xFFFFFFFF )									//!< 64bit整数の下位32bitの値を取得
#define npMakeI16( hb, lb )	( ( ( ( ( hb ) | 0xFF00 ) << 8 ) & 0xFF00 ) | ( lb ) )	//!<上位下位二つの8bit整数で16bit整数を作る
#define npMakeI32( hw, lw )	( ( ( ( ( hw ) | 0xFFFF0000 ) << 16 ) & 0xFFFF0000 ) | ( lw ) )						//!<上位下位二つの16bit整数で32bit整数を作る
#define npMakeI64( hl, ll )	( ( ( ( ( hl ) | 0xFFFFFFFF00000000 ) <<  32 ) & 0xFFFFFFFF00000000 ) | ( ll ) )	//!<上位下位二つの32bit整数で64bit整数を作る
#else
#define npSwapI64( d )			( d )
#define	npSwapI32( l )			( l )
#define	npSwapI16( w )			( w )
#define npHiI8( w )			( w )
#define npLoI8( w )			( w )
#define npHiI16( l )			( l )
#define npLoI16( l )			( l )
#define npHiI32( d )			( d )
#define npLoI32( d )			( d )
#define npMakeI16( hb, lb )	( hb, lb )
#define npMakeI32( hw, lw )	( hw, lw )
#define npMakeI64( hl, ll )	( hl, ll )
#endif
//!@}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	typedef
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------

/*!	@name	バイト操作	*/
/*! @note ▼以下は、バッファポインタと併用することが多い関数郡です。<BR>
	マクロにすると、誤ってインクリメント演算子を重複させてしまう
	危険性が高いのでインライン関数にしました。*/
//!@{

//! 16bit符号つき整数のバイトオーダーを逆転する
npS16 npTurnS16( npS16 w )
{
	return npSwapI16( w );
}

//! 16bit符号なし整数のバイトオーダーを逆転する
npU16 npTurnU16( npU16 w )
{
	return npSwapI16( w );
}
//! 32bit符号つき整数のバイトオーダーを逆転する
npS32 npTurnS32( npS32 l )
{
	return npMakeI32( npSwapI16( npLoI16( l ) ), npSwapI16( npHiI16( l ) ) );
}
 //! 32bit整数のバイトオーダーを逆転する
npU32 npTurnU32( npU32 l )
{
	return npMakeI32( npSwapI16( npLoI16( l ) ), npSwapI16( npHiI16( l ) ) );
}
/*
//! 64bit符号つき整数のバイトオーダーを逆転する
npS64 npTurnS64( npS64 d )
{
	return npMakeI64( npTurnS32( npLoI32( d ) ), npTurnS32( npHiI32( d ) ) );
}
//! 64bit符号なし整数のバイトオーダーを逆転する
npU64 npTurnU64( npU64 d )
{
	return npMakeI64( npTurnU32( npLoI32( d ) ), npTurnU32( npHiI32( d ) ) );
}
*/
//! Floatのバイトオーダーを逆転する
npFLOAT npTurnF( npFLOAT l )
{
	npS32 s = npTurnS32( *( npS32 * )&l );
	// FOR DS
	float f = *((float *)&s);
	s = FX_F32_TO_FX32(f); 
	return s;
}
/*
//! Doubleのバイトオーダーを逆転する
double npTurnD( double d )
{
	npS64 s = npTurnS64( *( npS64 * )&d );
	return *( double * )&s;
}
*/
//!@}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*
#ifdef __cplusplus
}
#endif
*/
#endif	// __BPCBYTE_H__
/*\}*/ // end of GROUP_BPCBYTE
