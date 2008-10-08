/*=============================================================================
/*!

	@file	npParticleSubject.h

	@brief	�p�[�e�B�N���T�u�W�F�N�g(�`��Ώ�)
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.4 $
	$Date: 2006/02/19 06:35:50 $

*//*=========================================================================*/
#ifndef	NPPARTICLESUBJECT_H
#define	NPPARTICLESUBJECT_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npParticleObject.h"
#include "npParticleComposite.h"

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
/*!
	
	@union	�p�[�e�B�N���T�u�W�F�N�g�Ɋ܂܂��v�f

*/
typedef	union __npPARTICLESUBJECTSTYLE
{
	npPARTICLECOMPOSITE		Composite;
	npPARTICLEEMITTER		Emitter;
	npPARTICLEQUADSURFACE	QuadSurface;
}
npPARTICLESUBJECTSTYLE;

/*!

	@struct	�p�[�e�B�N���T�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLESUBJECT
{
	npPARTICLESUBJECTSTYLE		Style;

	/* �A���[�i�̈�̎��̃|�C���^ */
	struct __npPARTICLESUBJECT*	m_pNext;

	npU32	m_pReserve[ 3 ];
}
NP_FSTRUCT npPARTICLESUBJECT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�T�u�W�F�N�g�ŕK�v�ȃ������T�C�Y���擾����

	@param	nNum	�g�p����T�u�W�F�N�g�̐�

	@return	�K�v�ȃ������T�C�Y

*/
#define	npParticleCheckSubjectHeapSize( nNum )	(			\
	( npSIZE )( sizeof( npPARTICLESUBJECT ) * ( nNum ) + npParticleCheckFrameEmitHeapSize( nNum ) ) )

/*!

	�T�u�W�F�N�g�̃A���[�i������������

	@param	pBuf	�T�u�W�F�N�g�̃A���[�i�̈�
	@param	nSize	��L�̃������T�C�Y

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleInitSubjectFactory( npVOID* pBuf, npSIZE nSize );

/*!

	�T�u�W�F�N�g�̃A���[�i�̈�̏I������

*/
NP_API npVOID NP_APIENTRY npParticleExitSubjectFactory( npVOID );

/*!

	�T�u�W�F�N�g�𐶐�����

	@param	ppSubject	���������T�u�W�F�N�g���󂯎��

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateSubject( npPARTICLESUBJECT** ppSubject );

/*!

	�T�u�W�F�N�g���J������

	@param	pSubject	�Ώۂ̃T�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseSubject( npPARTICLESUBJECT* pSubject );

/*!
	
	�T�u�W�F�N�g�̃������g�p�ʂ�Ԃ�

*/
NP_API npU32 NP_APIENTRY npParticleGetUsingSubjectSize( npVOID );

#ifdef __cplusplus
}
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#endif	/* NPPARTICLESUBJECT_H */
