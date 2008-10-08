/*=============================================================================
/*!

	@file	npParticleComposite.h

	@brief	�p�[�e�B�N���ʂ̊Ǘ��Ώے�`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.10 $
	$Date: 2006/03/20 09:14:56 $

*//*=========================================================================*/
#ifndef	NPPARTICLECOMPOSITE_H
#define	NPPARTICLECOMPOSITE_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npParticleObject.h"

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
/* �R���|�W�b�g�̕`��X�^�b�N�T�C�Y */
#define	NP_PARTICLE_STACK_MAX	( 128 )
//#define	NP_PARTICLE_STACK_MAX	( 2048 )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@struct	�p�[�e�B�N���ʂ̊Ǘ��I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLECOMPOSITE
{
	npPARTICLEEMITTER		Root;

	npFVECTOR				m_vecScale;

	/* �`��X�^�b�N */
	npU32					m_nStack;
	npU32					m_nLightID;
	npU32					m_pReserved[ 2 ];	
	npPARTICLEPRIMITIVE*	m_pStack[ NP_PARTICLE_STACK_MAX ];	
}
NP_FSTRUCT npPARTICLECOMPOSITE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�R���|�W�b�g�̗̈���A���[�i����擾����

	@param	pAttribute	�p�[�e�B�N���̑�����`�t�@�C��
	@param	ppComposite	���������R���|�W�b�g���󂯎��

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateComposite(
	NP_CONST	npPARTICLEEMITCYCLE*	pAttribute,
				npPARTICLECOMPOSITE**	ppComposite
);

/*!

	�R���|�W�b�g�̗̈���A���[�i�֕ԋp����

	@param	pComposite	�Ώۂ̃R���|�W�b�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseComposite( 
	npPARTICLECOMPOSITE*	pComposite
);

/*!

	�R���|�W�b�g�̍Đ������Z�b�g����

	@param	pComposite	�Ώۂ̃R���|�W�b�g

*/
NP_API npVOID NP_APIENTRY npParticleResetComposite(
	npPARTICLECOMPOSITE*	pComposite
);

/*!

	�R���|�W�b�g�ɃX�P�[���l���Z�b�g����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pScale		npFVECTOR �^�̃X�P�[���l

	@return	�G���[�R�[�h

*/
#define	npParticleSetScaling( pComposite, pScale )	(	\
	npCopyFVEC4( &pComposite->m_vecScale, pScale ), NP_SUCCESS )

/*!

	�R���|�W�b�g�̃X�P�[���l���擾����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pScale		npFVECTOR �^�̃X�P�[���l

	@return	�G���[�R�[�h

*/
#define	npParticleGetScaling( pComposite, pScale )	(	\
	npCopyFVEC4( pScale, &pComposite->m_vecScale ), NP_SUCCESS )

/*!

	�p�[�e�B�N���̕`�惊�X�g�ɕ`��Ώۂ̃v���~�e�B�u��ǉ�����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pPrimitive	�`��Ώۂ̃v���~�e�B�u

	@return	�G���[�R�[�h

*/
#define	npParticleStorePrimitive( pComposite, pPrimitive )	(				\
	( pComposite )->m_nStack >= NP_PARTICLE_STACK_MAX ? NP_ERR_OVERFLOW :	\
	( ( pComposite )->m_pStack[ ( pComposite )->m_nStack ] = ( pPrimitive ), ( pComposite )->m_nStack++, NP_SUCCESS ) ) 


/*!

	�G�t�F�N�g�̃��C�gID��ݒ肷��

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	nLightID	���C�g��ID

	@return	�G���[�R�[�h

*/
#define npParticleSetLightID( pComposite , nLightID ) ( pComposite->m_nLightID = nLightID ,NP_SUCCESS )

/*!

	�G�t�F�N�g�̃��C�gID���擾����

	@param	pComposite	�Ώۂ̃R���|�W�b�g

*/
#define npParticleGetLightID( pComposite ) ( pComposite->m_nLightID )
/*!

	�R���|�W�b�g���X�V����

	@param	pComposit	�Ώۂ̃R���|�W�b�g
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateComposite(
	npPARTICLECOMPOSITE*	pComposite,
	npU32					nStep
);

/*!

	�R���|�W�b�g�������_�����O����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pList		�g�p����e�N�X�`���̔z��
	@param	nNum		�g�p����e�N�X�`���̖���

*/
NP_API npVOID NP_APIENTRY npParticleRenderComposite(
	npCONTEXT*				pContext,
	npPARTICLECOMPOSITE*	pComposite,
	npTEXTURE**				pList,
	npSIZE					nNum
);

/*!

	�G�~�b�^�������_�����O����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pComposite	�Ώۂ̃R���|�W�b�g

*/
#ifdef	NP_DEBUG
NP_API npVOID NP_APIENTRY npParticleRenderEmitter(
	npCONTEXT*			pContext,
	npPARTICLEOBJECT*	pObject,
	npFLOAT				size
);
#else
#	define	npParticleRenderEmitter( pContext, pComposite, size )
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLECOMPOSITE_H */
