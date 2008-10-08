/*=============================================================================
/*!

	@file	npMatrix.h

	@brief	�}�g���N�X��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.6 $
	$Date: 2006/03/10 10:35:09 $

*//*=========================================================================*/
#ifndef	NPMATRIX_H
#define	NPMATRIX_H

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
/*!

	�}�g���N�X�̗v�f���擾����

	@param	m		�}�g���N�X�ւ̃|�C���^
	@param	row		�s�ԍ�
	@param	col		��ԍ�

	@return	�}�g���N�X�̗v�f

*/
#define	NP_MATRIX( m, row, col )	( ( *( m ) )[ row ][ col ] )

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
/*!

	�}�g���N�X���R�s�[����

	@param	dst	���Z���ʂł��� npFMATRIX �̃|�C���^
	@param	src	�����̊�ɂȂ� npFMATRIX �̃|�C���^

	@return	���Z���ʂł��� npFMATRIX �̃|�C���^

*/
#define	npCopyFMATRIX( dst, src )	(	\
	npCopyMemory( ( dst ), ( src ), sizeof( npFMATRIX ) ), ( dst ) )

/*!

	�P�ʍs����쐬����B

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^�B
	
	@return	���Z���ʂł��� npFMATRIX �ւ̃|�C���^�B

*/
NP_API npFMATRIX* NP_APIENTRY npUnitFMATRIX( npFMATRIX* dst );
	
/*!

	�s��̋t�s����v�Z����

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	src	�����̊�ɂȂ� npFMATRIX �ւ̃|�C���^
	
	@return	�s��̋t�s��ł��� npFMATRIX �ւ̃|�C���^
	
	@note	�t�s��̌v�Z�����s�����ꍇ�́ANP_NULL ��Ԃ�

*/
NP_API npFMATRIX* NP_APIENTRY npInverseFMATRIX( npFMATRIX* dst, npFMATRIX* src );

/*!

	�s��̓]�u�s���Ԃ�

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	src	�����̊�ɂȂ� npFMATRIX �ւ̃|�C���^

	@return	�s��̓]�u�s��ł��� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npTransposeFMATRIX( npFMATRIX* dst, npFMATRIX* src );

/*!

	2 �̍s��̐ς��v�Z����

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	m1	�����̊�ɂȂ� npFMATRIX �ւ̃|�C���^
	@param	m2	�����̊�ɂȂ� npFMATRIX �ւ̃|�C���^

	@return	2 �̍s��̐ςł��� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npMulFMATRIX( npFMATRIX* dst, npFMATRIX* m2, npFMATRIX* m1 );

/*!

	�I�t�Z�b�g���w�肵�čs����쐬����

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	x	x ���W�̃I�t�Z�b�g
	@param	y	y ���W�̃I�t�Z�b�g
	@param	z	z ���W�̃I�t�Z�b�g
	
	@return	���s�ړ����ꂽ�g�����X�t�H�[���s����܂� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npTranslateFMATRIX( npFMATRIX* dst, npFLOAT x, npFLOAT y, npFLOAT z );

/*!

	�C�ӂ̎�����]���ɂ��ĉ�]����s����쐬����

	@param	dst		���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	x		�C�ӎ���\���x�N�g����X�v�f
	@param	y		�C�ӎ���\���x�N�g����Y�v�f
	@param	z		�C�ӎ���\���x�N�g����Z�v�f
	@param	angle	��]�p�x

	@return	���Z���ʂł��� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npRotateFMATRIX( npFMATRIX* dst, npFVECTOR* v, npFLOAT angle );

/*!

	x ������]���ɂ��ĉ�]����s����쐬����

	@param	dst		���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	angle	��]�̊p�x (���W�A���P��)�B�p�x�́A��]���𒆐S�ɂ���<br>
					���_���������������v���ő��肵�����̂ł���

	@return	x ������]���Ƃ��ĉ�]���� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npRotateXFMATRIX( npFMATRIX* dst, npFLOAT angle );

/*!

	y ������]���ɂ��ĉ�]����s����쐬����

	@param	dst		���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	angle	��]�̊p�x (���W�A���P��)�B�p�x�́A��]���𒆐S�ɂ���<br>
					���_���������������v���ő��肵�����̂ł���

	@return	y ������]���Ƃ��ĉ�]���� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npRotateYFMATRIX( npFMATRIX* dst, npFLOAT angle );

/*!

	z ������]���ɂ��ĉ�]����s����쐬����

	@param	dst		���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	angle	��]�̊p�x (���W�A���P��)�B�p�x�́A��]���𒆐S�ɂ���<br>
					���_���������������v���ő��肵�����̂ł���

	@return	z ������]���Ƃ��ĉ�]���� npFMATRIX �ւ̃|�C���^

*/
NP_API npFMATRIX* NP_APIENTRY npRotateZFMATRIX( npFMATRIX* dst, npFLOAT angle );

/*!

	x ���Ay ���Az ���ɉ����ăX�P�[�����O����s����쐬����

	@param	dst	���Z���ʂł��� npFMATRIX �ւ̃|�C���^
	@param	x	x ���ɉ����ēK�p�����X�P�[�����O�W��
	@param	y	y ���ɉ����ēK�p�����X�P�[�����O�W��
	@param	z	z ���ɉ����ēK�p�����X�P�[�����O�W��

	@return	�X�P�[�����O �g�����X�t�H�[�� npFMATRIX

*/
NP_API npFMATRIX* NP_APIENTRY npScaleFMATRIX( npFMATRIX* dst, npFLOAT x, npFLOAT y, npFLOAT z );

/*!

	�x�N�g���̃}�g���N�X�ϊ����s��
	
	@param	dst	���Z���ʂł��� npFVEC3 �ւ̃|�C���^
	@param	m	�}�g���N�X
	@param	v	�x�N�g��
	
	@return	�}�g���N�X�ϊ����s���� npFVEC3 �ւ̃|�C���^

*/
NP_API npFVECTOR* NP_APIENTRY npTransformFMATRIX( npFVECTOR* dst, npFMATRIX* m, npFVECTOR* v );

#define npPrintFMATRIX(_m) \
	OS_Printf("--- BEGIN MATRIX "#_m" ---\n");\
	OS_Printf("%8.8x, %8.8x %8.8x, %8.8x\n", NP_MATRIX(_m, 0, 0), NP_MATRIX(_m, 0, 1), NP_MATRIX(_m, 0, 2), NP_MATRIX(_m, 0, 3));\
	OS_Printf("%8.8x, %8.8x %8.8x, %8.8x\n", NP_MATRIX(_m, 1, 0), NP_MATRIX(_m, 1, 1), NP_MATRIX(_m, 1, 2), NP_MATRIX(_m, 1, 3));\
	OS_Printf("%8.8x, %8.8x %8.8x, %8.8x\n", NP_MATRIX(_m, 2, 0), NP_MATRIX(_m, 2, 1), NP_MATRIX(_m, 2, 2), NP_MATRIX(_m, 2, 3));\
	OS_Printf("%8.8x, %8.8x %8.8x, %8.8x\n", NP_MATRIX(_m, 3, 0), NP_MATRIX(_m, 3, 1), NP_MATRIX(_m, 3, 2), NP_MATRIX(_m, 3, 3));\
	OS_Printf("--- End "#_m" ---\n")

#define npPrintFMATRIXF(_m) \
	OS_Printf("--- BEGIN MATRIX "#_m" ---\n");\
	OS_Printf("%f, %f %f, %f\n", FX_FX32_TO_F32(NP_MATRIX(_m, 0, 0)), FX_FX32_TO_F32(NP_MATRIX(_m, 0, 1)), FX_FX32_TO_F32(NP_MATRIX(_m, 0, 2)), FX_FX32_TO_F32(NP_MATRIX(_m, 0, 3)));\
	OS_Printf("%f, %f %f, %f\n", FX_FX32_TO_F32(NP_MATRIX(_m, 1, 0)), FX_FX32_TO_F32(NP_MATRIX(_m, 1, 1)), FX_FX32_TO_F32(NP_MATRIX(_m, 1, 2)), FX_FX32_TO_F32(NP_MATRIX(_m, 1, 3)));\
	OS_Printf("%f, %f %f, %f\n", FX_FX32_TO_F32(NP_MATRIX(_m, 2, 0)), FX_FX32_TO_F32(NP_MATRIX(_m, 2, 1)), FX_FX32_TO_F32(NP_MATRIX(_m, 2, 2)), FX_FX32_TO_F32(NP_MATRIX(_m, 2, 3)));\
	OS_Printf("%f, %f %f, %f\n", FX_FX32_TO_F32(NP_MATRIX(_m, 3, 0)), FX_FX32_TO_F32(NP_MATRIX(_m, 3, 1)), FX_FX32_TO_F32(NP_MATRIX(_m, 3, 2)), FX_FX32_TO_F32(NP_MATRIX(_m, 3, 3)));\
	OS_Printf("--- End "#_m" ---\n")

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPMATRIX_H */
