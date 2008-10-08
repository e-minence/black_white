/*=============================================================================
/*!

	@file	npCamera.h

	@brief	�J������`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: $
	$Date: $

*//*=========================================================================*/
#ifndef	NPCAMERA_H
#define	NPCAMERA_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npObject.h"

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

	�r���[�}�g���N�X�𐶐�����

	@param	dst		�o�͐�̃}�g���N�X
	@param	eye		�J�����ʒu
	@param	center	�����_
	@param	up		����x�N�g��

*/
NP_API npFMATRIX* NP_APIENTRY npLookAtFMATRIX( 
	npFMATRIX*	dst,
	npFVECTOR*	eye,
	npFVECTOR*	center,
	npFVECTOR*	up
);

/*!

	���ˉe�ɂ�镽�s�Ȏ��̐ς̍s��
	
	@param	dst		�o�͐�̃}�g���N�X
	@param	left	���ʒu
	@param	right	�E�ʒu
	@param	bottom	���ʒu
	@param	top		��ʒu
	@param	znear	Z near
	@param	zfar	Z far

*/
NP_API npFMATRIX* NP_APIENTRY npOrthoFMATRIX( 
	npFMATRIX*	dst,
	npFLOAT		left,
	npFLOAT		right,
	npFLOAT		bottom,
	npFLOAT		top,
	npFLOAT		znear,
	npFLOAT		zfar
);

/*!

	�����@�ɂ�鐍��̍s����쐬����

	@param	dst		�o�͐�̃}�g���N�X
	@param	left	���ʒu
	@param	right	�E�ʒu
	@param	bottom	���ʒu
	@param	top		��ʒu
	@param	znear	Z near
	@param	zfar	Z far

*/
NP_API npFMATRIX* NP_APIENTRY npFrustumFMATRIX( 
	npFMATRIX*	dst,
	npFLOAT		left,
	npFLOAT		right,
	npFLOAT		bottom,
	npFLOAT		top,
	npFLOAT		znear,
	npFLOAT		zfar
);

/*!

	�p�[�X�y�N�e�B�u�}�g���N�X�𐶐�����

	@param	dst		�o�͐�̃}�g���N�X
	@param	angle	Field of View ( Radiun )
	@param	aspect	�A�X�y�N�g��
	@param	znear	Z Near
	@param	zfar	Z Far

*/
NP_API npFMATRIX* NP_APIENTRY npPerspectiveFMATRIX( 
	npFMATRIX*	dst,
	npFLOAT		fovy,
	npFLOAT		aspect,
	npFLOAT		znear,
	npFLOAT		zfar
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPCAMERA_H */
