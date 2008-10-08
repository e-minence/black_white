/*=============================================================================
/*!

	@file	npError.h

	@brief	エラー定義ファイル
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.3 $
	$Date: 2006/03/22 05:10:50 $

*//*=========================================================================*/
#ifndef	NPERROR_H
#define	NPERROR_H



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	include
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	define
//-----------------------------------------------------------------------------
/*!

	エラー検出

	@param	code		エラーコード

	@retval	NP_TRUE		エラー無し
	@retval	NP_FALSE	エラー

*/
#define	NP_SUCCEDED( code )	( ( code ) == NP_SUCCESS ? NP_TRUE : NP_FALSE )

/*!

	エラー検出

	@param	code		エラーコード

	@retval	NP_TRUE		エラー
	@retval	NP_FALSE	エラー無し

*/
#define	NP_FAILED( code )	( ( code ) != NP_SUCCESS ? NP_TRUE : NP_FALSE )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------
/*!

	@enum	Error codes

*/
enum npERRORCODE
{
	NP_SUCCESS = 0,	/*!< Success */
	
	NP_ERR_USAGE,	/*!< Usage */
	NP_ERR_NONE,	/*!< None */
	NP_ERR_STATUS,
	NP_ERR_MODE,

	NP_ERR_WARNING,
	NP_ERR_INTERNAL,
	NP_ERR_ERROR,

	NP_ERR_BAD_ARGUMENT,

	NP_ERR_ALLOC_PROBLEM,

	NP_ERR_NOTSUPPORTED,
	NP_ERR_NOTIMPLIMENTED,
	NP_ERR_NOTFOUND,

	NP_ERR_OVERFLOW,
	NP_ERR_MEMORYLEAK,

	NP_ERR_MAX
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */



#endif	/* NPERROR_H */
