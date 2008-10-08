/*=============================================================================
/*!

	@file	npParticleComponent.h

	@brief	�p�[�e�B�N���R���|�[�l���g(���\�[�X)��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.8 $
	$Date: 2006/03/22 06:49:05 $

*//*=========================================================================*/
#ifndef	NPPARTICLECOMPONENT_H
#define	NPPARTICLECOMPONENT_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npParticleAttribute.h"

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

	@union	�p�[�e�B�N���R���|�[�l���g�Ɋ܂܂��v�f

*/
typedef union __npPARTICLECOMPONENTSTYLE
{
	npPARTICLEEMITCYCLE		Cycle;
	npPARTICLEQUAD			Quad;
	//npPARTICLEMODEL		Model;
	//npPARTICLEACTOR		Actor;
}
npPARTICLECOMPONENTSTYLE;

/*!

	@struct	�p�[�e�B�N���R���|�[�l���g

*/
typedef NP_HSTRUCT __npPARTICLECOMPONENT
{
	npPARTICLECOMPONENTSTYLE	Style;

	/* �A���[�i�̈�̎��̃|�C���^ */
	struct __npPARTICLECOMPONENT*	m_pNext;

	npU32	m_pReserve[ 3 ];
}
NP_FSTRUCT npPARTICLECOMPONENT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�R���|�[�l���g�ŕK�v�ȃ������T�C�Y���擾����

	@param	nNum	�g�p����R���|�[�l���g��

	@return	�K�v�ȃ������T�C�Y

*/
#define	npParticleCheckComponentHeapSize( nNum )	(	\
	( npSIZE )( sizeof( npPARTICLECOMPONENT ) * ( nNum ) ) )

/*!

	�R���|�[�l���g�̃A���[�i������������

	@param	pBuf	�R���|�[�l���g�̃A���[�i�̈�
	@param	nSize	��L�o�b�t�@�̃T�C�Y

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleInitComponentFactory( npVOID* pBuf, npSIZE nSize );

/*!

	�R���|�[�l���g�̃A���[�i�̈�̏I������

*/
NP_API npVOID NP_APIENTRY npParticleExitComponentFactory( npVOID );

/*!

	�R���|�[�l���g�𐶐�����

	@param	ppComponent	���������R���|�[�l���g���󂯎��

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateComponent( npPARTICLECOMPONENT** ppComponent );

/*!

	�R���|�[�l���g���J������

	@param	pComponent	�Ώۂ̃R���|�[�l���g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseComponent( npPARTICLECOMPONENT* pComponent );

/*!

	�R���|�[�l���g�̃A���[�i���_���v����

*/
#ifdef NP_DEBUG
NP_API npVOID NP_APIENTRY npParticleDumpComponentFactory( npVOID );
#else
#	define	npParticleDumpComponentFactory( npVOID )
#endif

/*!
	
	�R���|�[�l���g�̃������g�p�ʂ�Ԃ�

*/
NP_API npU32 NP_APIENTRY npParticleGetUsingComponentSize( npVOID );

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLECOMPONENT_H */
