/*=============================================================================
/*!

	@file	npType.h

	@brief	型定義ファイル
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.2 $
	$Date: 2006/02/23 08:09:33 $

*//*=========================================================================*/
#ifndef	NPTYPE_H
#define	NPTYPE_H


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	include
//-----------------------------------------------------------------------------
#include <types.h>
#include "core/npConst.h"

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	define
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	typedef
//-----------------------------------------------------------------------------
/*!	@name	型定義 */
//!@{

typedef void	npVOID;

typedef char	npCHAR;

typedef s8		npS8;
typedef	u8		npU8;
typedef s16		npS16;
typedef	u16		npU16;
typedef s32		npS32;
typedef u32		npU32;
typedef s64		npS64;
typedef u64		npU64;

typedef	fx32	npFLOAT;
typedef	npU32	npFIXED;
typedef npU64	npFRAME;

typedef int		npBOOL;
typedef npU32	npSTATE;
typedef npU8	npBYTE;
typedef npU32	npSIZE;

typedef	npS32	npERROR;
typedef void*	npHANDLE;


typedef npFLOAT			npFRGBA[ 4 ];
typedef	npU32			npCOLOR;

//typedef npU32			npUVEC2[ 2 ];
//typedef npU32			npUVEC3[ 3 ];
//typedef npU32			npUVEC4[ 4 ];
typedef	npS32			npIVECTOR[ 4 ];

/*

	C リンケージ外では igVec〜 などはクラスとして
	扱われるので下記のものを使用する

*/
//typedef npFLOAT			npFVEC2[ 2 ];
//typedef npFLOAT			npFVEC3[ 3 ];
//typedef npFLOAT			npFVEC4[ 4 ];

typedef npFLOAT			npFVECTOR[ 4 ];
typedef npFLOAT			npFQUATE[ 4 ];
typedef npFLOAT			npFMATRIX[ 4 ][ 4 ];

//!@}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@struct	汎用タグ

*/
typedef NP_HSTRUCT __npTAG
{
	npSTATE	m_nType;
	npSTATE m_nState;
	npVOID*	m_pAddr;
	npSIZE	m_nSize;	
}
NP_FSTRUCT npTAG;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#endif	/* NPTYPE_H */
