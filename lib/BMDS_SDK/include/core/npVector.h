/*=============================================================================
/*!

	@file	npVector.h

	@brief	�x�N�g����`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.4 $
	$Date: 2006/03/01 07:23:38 $

*//*=========================================================================*/
#ifndef	NPVECTOR_H
#define	NPVECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	include
//-----------------------------------------------------------------------------
#include "core/npTypedef.h"
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	define
//-----------------------------------------------------------------------------
/*! @name	�x�N�g���̗v�f���擾���� */
//!@{

#define	NP_X( v1 )	( ( *( v1 ) )[ 0 ] )
#define NP_Y( v1 )	( ( *( v1 ) )[ 1 ] )
#define	NP_Z( v1 )	( ( *( v1 ) )[ 2 ] )
#define	NP_W( v1 )	( ( *( v1 ) )[ 3 ] )

//!@}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------

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
/*!	@name	npFVEC2 */
//!@{

/*!

	2D �x�N�g�����R�s�[����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	src	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^
	
*/
#define	npCopyFVEC2( dst, src )	\
	( npCopyMemory( dst, src, sizeof( npFLOAT ) * 2 ), ( dst ) )

/*!

	2D �x�N�g�����Z�b�g����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^ 
	@param	x	�����̊�ɂȂ�l
	@param	y	�����̊�ɂȂ�l

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
#define npSetFVEC2( dst, x1, y1 )	(	\
	NP_X( dst ) = ( x1 ),				\
	NP_Y( dst ) = ( y1 ), ( dst )	)
		
/*!

	2 �� 2D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
#define	npAddFVEC2( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) + NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) + NP_Y( v2 ), ( dst ) )

/*!

	2 �� 2D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
#define	npSubFVEC2( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) - NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) - NP_Y( v2 ), ( dst ) )

/*!

	2D �x�N�g�����X�P�[�����O����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^
	@param	s	�X�P�[�����O�l

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
#define	npScaleFVEC2( dst, v1, s )	(	\
	NP_X( dst ) = FX_MUL(NP_X( v1 ), ( s )),	\
	NP_Y( dst ) = FX_MUL(NP_Y( v1 ), ( s )), ( dst ) )

/*!

	2D �x�N�g���̐��K�������x�N�g����Ԃ�

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
NP_API npFVECTOR* NP_APIENTRY npNormalizeFVEC2( npFVECTOR* dst, npFVECTOR* v1 );

/*!

	2D �x�N�g���̒�����Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^

	@return	�x�N�g���̒���

*/
#define	npLengthFVEC2( v1 )		npSqrt( npLengthSqrFVEC2( v1 ) )

/*!

	2D �x�N�g���̒����� 2 ���Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^

	@return	�x�N�g���� 2 �悳�ꂽ����

*/
#define	npLengthSqrFVEC2( v1 )	( FX_MUL(NP_X( v1 ), NP_X( v1 )) + FX_MUL(NP_Y( v1 ), NP_Y( v1 )) )

/*!

	2 �� 2D �x�N�g���̓��ς��v�Z����
	
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^
	@param	v2	�����̊�ɂȂ� npFVEC2 �̃|�C���^

	@return	����

*/
#define	npDotFVEC2( v1, v2 )	( FX_MUL(NP_X( v1 ), NP_X( v2 )) + FX_MUL(NP_Y( v1 ), NP_Y( v2 )) )

/*!

	2 �� 2D �x�N�g���̊O�ς��v�Z����

	@param	dst	���Z���ʂł��� npFVEC2 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC2 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC2 �̃|�C���^

*/
#define	npCrossFVEC2( v1, v2 )	( FX_MUL(NP_X( v1 ), NP_Y( v2 )) - FX_MUL(NP_Y( v1 ), NP_X( v2 )) )

//!@}

/*!	@name	npFVEC3 */
//!@{

/*!

	3D �x�N�g�����R�s�[����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	src	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^
	
*/
#define	npCopyFVEC3( dst, src )	(	\
	npCopyMemory( ( dst ), ( src ), sizeof( npFLOAT ) * 3 ), ( dst ) )

/*!

	3D �x�N�g�����Z�b�g����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^ 
	@param	x	�����̊�ɂȂ�l
	@param	y	�����̊�ɂȂ�l
	@param	z	�����̊�ɂȂ�l

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
#define npSetFVEC3( dst, x1, y1, z1 )	(	\
	NP_X( dst ) = ( x1 ),					\
	NP_Y( dst ) = ( y1 ),					\
	NP_Z( dst ) = ( z1 ), ( dst ) )

/*!

	2 �� 3D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
#define	npAddFVEC3( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) + NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) + NP_Y( v2 ),	\
	NP_Z( dst ) = NP_Z( v1 ) + NP_Z( v2 ), ( dst ) )

/*!

	2 �� 3D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
#define	npSubFVEC3( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) - NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) - NP_Y( v2 ),	\
	NP_Z( dst ) = NP_Z( v1 ) - NP_Z( v2 ), ( dst ) )

/*!

	3D �x�N�g�����X�P�[�����O����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^
	@param	s	�X�P�[�����O�l

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
#define	npScaleFVEC3( dst, v1, s )	(	\
	NP_X( dst ) = FX_Mul(NP_X( v1 ), ( s )),	\
	NP_Y( dst ) = FX_Mul(NP_Y( v1 ), ( s )),	\
	NP_Z( dst ) = FX_Mul(NP_Z( v1 ), ( s )), ( dst ) )

/*!

	3D �x�N�g���̐��K�������x�N�g����Ԃ�

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
NP_API npFVECTOR* NP_APIENTRY npNormalizeFVEC3( npFVECTOR* dst, npFVECTOR* v1 );

/*!

	3D �x�N�g���̒�����Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^

	@return	�x�N�g���̒���

*/
#define	npLengthFVEC3( v1 )		npSqrt( npLengthSqrFVEC3( v1 ) )

/*!

	3D �x�N�g���̒����� 2 ���Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^

	@return	�x�N�g���� 2 �悳�ꂽ����

*/
#define	npLengthSqrFVEC3( v1 )	( FX_MUL(NP_X( v1 ), NP_X( v1 )) + FX_MUL(NP_Y( v1 ), NP_Y( v1 )) + FX_MUL(NP_Z( v1 ), NP_Z( v1 )) )

/*!

	2 �� 3D �x�N�g���̓��ς��v�Z����
	
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^
	@param	v2	�����̊�ɂȂ� npFVEC3 �̃|�C���^

	@return	����

*/
#define	npDotFVEC3( v1, v2 )	( FX_MUL(NP_X( v1 ), NP_X( v2 )) + FX_MUL(NP_Y( v1 ), NP_Y( v2 )) + FX_MUL(NP_Z( v1 ), NP_Z( v2 )) )

/*!

	2 �� 3D �x�N�g���̊O�ς��v�Z����

	@param	dst	���Z���ʂł��� npFVEC3 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC3 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC3 �̃|�C���^

*/
#define	npCrossFVEC3( dst, v1, v2 )	(									\
	NP_X( dst ) = FX_MUL(NP_Y( v1 ), NP_Z( v2 )) - FX_MUL(NP_Z( v1 ), NP_Y( v2 )),	\
	NP_Y( dst ) = FX_MUL(NP_Z( v1 ), NP_X( v2 )) - FX_MUL(NP_X( v1 ), NP_Z( v2 )),	\
	NP_Z( dst ) = FX_MUL(NP_X( v1 ), NP_Y( v2 )) - FX_MUL(NP_Y( v1 ), NP_X( v2 )) , ( dst ) )

//!@}

/*!	@name	npFVEC4 */
//!@{

/*!

	4D �x�N�g�����R�s�[����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	src	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^
	
*/
#define	npCopyFVEC4( dst, src )	\
	( npCopyMemory( dst, src, sizeof( npFVECTOR ) ), ( dst ) )

/*!

	4D �x�N�g�����Z�b�g����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^ 
	@param	x	�����̊�ɂȂ�l
	@param	y	�����̊�ɂȂ�l
	@param	z	�����̊�ɂȂ�l
	@param	w	�����̊�ɂȂ�l

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
#define	npSetFVEC4( dst, x1, y1, z1, w1 )	(	\
	NP_X( dst ) = ( x1 ),						\
	NP_Y( dst ) = ( y1 ),						\
	NP_Z( dst ) = ( z1 ),						\
	NP_W( dst ) = ( w1 ), ( dst ) )

/*!

	2 �� 4D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
#define	npAddFVEC4( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) + NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) + NP_Y( v2 ),	\
	NP_Z( dst ) = NP_Z( v1 ) + NP_Z( v2 ),	\
	NP_W( dst ) = NP_W( v1 ) + NP_W( v2 ), ( dst ) )

/*!

	2 �� 4D �x�N�g�������Z����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
#define	npSubFVEC4( dst, v1, v2 )	(		\
	NP_X( dst ) = NP_X( v1 ) - NP_X( v2 ),	\
	NP_Y( dst ) = NP_Y( v1 ) - NP_Y( v2 ),	\
	NP_Z( dst ) = NP_Z( v1 ) - NP_Z( v2 ),	\
	NP_W( dst ) = NP_W( v1 ) - NP_W( v2 ), ( dst ) )

/*!

	4D �x�N�g�����X�P�[�����O����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^
	@param	s	�X�P�[�����O�l

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
#define	npScaleFVEC4( dst, v1, s )	(	\
	NP_X( dst ) = FX_MUL(NP_X( v1 ), ( s )),	\
	NP_Y( dst ) = FX_MUL(NP_Y( v1 ), ( s )),	\
	NP_Z( dst ) = FX_MUL(NP_Z( v1 ), ( s )),	\
	NP_W( dst ) = FX_MUL(NP_W( v1 ), ( s )), ( dst ) )

/*!

	4D �x�N�g���̐��K�������x�N�g����Ԃ�

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
NP_API npFVECTOR* NP_APIENTRY npNormalizeFVEC4( npFVECTOR* dst, npFVECTOR* v1 );

/*!

	4D �x�N�g���̒�����Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^

	@return	�x�N�g���̒���

*/
#define	npLengthFVEC4( v1 )		npSqrt( npLengthSqrFVEC4( v1 ) )

/*!

	4D �x�N�g���̒����� 2 ���Ԃ�

	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^

	@return	�x�N�g���� 2 �悳�ꂽ����

*/
#define	npLengthSqrFVEC4( v1 )	( FX_MUL(NP_X( v1 ), NP_X( v1 )) + FX_MUL(NP_Y( v1 ), NP_Y( v1 )) + FX_MUL(NP_Z( v1 ), NP_Z( v1 )) + FX_MUL(NP_W( v1 ), NP_W( v1 )) )

/*!

	2 �� 4D �x�N�g���̓��ς��v�Z����
	
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^
	@param	v2	�����̊�ɂȂ� npFVEC4 �̃|�C���^

	@return	����

*/
#define	npDotFVEC4( v1, v2 )	( FX_MUL(NP_X( v1 ), NP_X( v2 )) + FX_MUL(NP_Y( v1 ), NP_Y( v2 )) + FX_MUL(NP_Z( v1 ), NP_Z( v2 )) + FX_MUL(NP_W( v1 ), NP_W( v2 )) )

/*!

	2 �� 4D �x�N�g���̊O�ς��v�Z����

	@param	dst	���Z���ʂł��� npFVEC4 �̃|�C���^
	@param	v1	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 
	@param	v2	�����̊�ɂȂ� npFVEC4 �̃|�C���^ 

	@return	���Z���ʂł��� npFVEC4 �̃|�C���^

*/
#define	npCrossFVEC4( dst, v1, v2 )	(	\
	( npFVEC4* )D3DXVec4Cross( ( D3DXVECTOR4* )( dst ), ( D3DXVECTOR4* )( v1 ), ( D3DXVECTOR4* )( v2 ) ) )

//!@}

#define npPrintFVECTOR(_v)\
	OS_Printf("--- BEGIN VECTOR "#_v" ---\n");\
	OS_Printf("%8.8x, %8.8x, %8.8x, %8.8x\n", NP_X(_v), NP_Y(_v), NP_Z(_v), NP_W(_v));\
	OS_Printf("--- END ---\n")

#define npPrintFVECTORF(_v)\
	OS_Printf("--- BEGIN VECTOR "#_v" ---\n");\
	OS_Printf("%f, %f, %f, %f\n", FX_FX32_TO_F32(NP_X(_v)), FX_FX32_TO_F32(NP_Y(_v)), FX_FX32_TO_F32(NP_Z(_v)), FX_FX32_TO_F32(NP_W(_v)));\
	OS_Printf("--- END ---\n")

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPVECTOR_H */
