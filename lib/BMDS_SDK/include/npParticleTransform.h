/*=============================================================================
/*!

	@file	npParticleTransform.h

	@brief	�g�����X�t�H�[������`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.10 $
	$Date: 2006/03/13 06:13:21 $

*//*=========================================================================*/
#ifndef	NPPARTICLETRANSFORM_H
#define	NPPARTICLETRANSFORM_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npParticleKinematics.h"

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

	@struct	�Œ�l�p�^�[���g�����X�t�H�[��

*/
typedef NP_HSTRUCT __npPARTICLEFIXEDTRANSFORM
{
	npOBJECT	Object;
}
NP_FSTRUCT npPARTICLEFIXEDTRANSFORM;

/*!

	@struct	�����x�p�^�[���g�����X�t�H�[��

*/
typedef NP_HSTRUCT __npPARTICLEVELOCITYTRANSFORM
{
	npOBJECT	Object;
	npFVECTOR	m_vecVel;
	npFVECTOR	m_vecAcc;
}
NP_FSTRUCT npPARTICLEVELOCITYTRANSFORM;

/*!

	@struct	�t�@���N�V�����J�[�u�g�����X�t�H�[��

*/
typedef NP_HSTRUCT __npPARTICLEFCURVETRANSFORM
{
	npOBJECT	Object;
	npFVECTOR	m_vecDir;
}
NP_FSTRUCT npPARTICLEFCURVETRANSFORM;

/*!

	@union	�g�����X�t�H�[���X�^�C��

*/
typedef union __npPARTICLETRANSFORMSTYLE
{
	npPARTICLEFIXEDTRANSFORM		Fixed;
	npPARTICLEVELOCITYTRANSFORM		Velocity;
	npPARTICLEFCURVETRANSFORM		FCurve;
}
npPARTICLETRANSFORMSTYLE;

/*!

	@struct	�g�����X�t�H�[��

*/
typedef NP_HSTRUCT __npPARTICLETRANSFORM
{
	npPARTICLETRANSFORMSTYLE			Style;
	
	npFVECTOR							m_vecFValue;
	NP_CONST npPARTICLEKINEMATICS*		m_pKinematics;
	npS32								m_nFrameLength;	/* ���t���[����		*/
	npS32								m_nLocalFrame;	/* ���݂̃t���[���� */
	npU32	m_pReserved[ 1 ];
}
NP_FSTRUCT npPARTICLETRANSFORM;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�g�����X�t�H�[���I�u�W�F�N�g�Ƀt���[���̒������Z�b�g����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	nLength		�g�����X�t�H�[���̃t���[���̒���

	@return	�G���[�R�[�h

	@note	�����֐��Ȃ̂� ( * 200 ) �͂��Ȃ�

*/
#define	npParticleSetTransformFrameLength( pTransform, nLength )	(	\
	( pTransform )->m_nFrameLength = ( nLength ), NP_SUCCESS )

/*!

	�g�����X�t�H�[���I�u�W�F�N�g�̃t���[���̒������擾����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g

	@return	�t���[����

	@note	�����֐��Ȃ̂� ( / 200 ) �͂��Ȃ�

*/
#define	npParticleGetTransformFrameLength( pTransform )	(	\
	( pTransform )->m_nFrameLength )	

/*!

	FLOAT �g�����X�t�H�[���̏�����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	pKinematics	�����I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupTransformFLOAT(
				npPARTICLETRANSFORM*	pTransform,
	NP_CONST	npPARTICLEKINEMATICS*	pKinematics
);

/*!

	FVEC2 �g�����X�t�H�[���̏�����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	pKinematics	�����I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupTransformFVEC2(
				npPARTICLETRANSFORM*	pTransform,
	NP_CONST	npPARTICLEKINEMATICS*	pKinematics
);

/*!

	FVEC3 �g�����X�t�H�[���̏�����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	pKinematics	�����I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupTransformFVEC3(
				npPARTICLETRANSFORM*	pTransform,
	NP_CONST	npPARTICLEKINEMATICS*	pKinematics
);

/*!

	FVEC4 �g�����X�t�H�[���̏�����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	pKinematics	�����I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupTransformFVEC4(
				npPARTICLETRANSFORM*	pTransform,
	NP_CONST	npPARTICLEKINEMATICS*	pKinematics
);

/*!

	FLOAT �g�����X�t�H�[���̍X�V����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateTransformFLOAT(
	npPARTICLETRANSFORM*	pTransform,
	npU32					nStep
);

/*!

	FVEC2 �g�����X�t�H�[���̍X�V����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateTransformFVEC2(
	npPARTICLETRANSFORM*	pTransform,
	npU32					nStep
);

/*!

	FVEC3 �g�����X�t�H�[���̍X�V����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateTransformFVEC3(
	npPARTICLETRANSFORM*	pTransform,
	npU32					nStep
);

/*!

	FVEC4 �g�����X�t�H�[���̍X�V����

	@param	pTransform	�g�����X�t�H�[���I�u�W�F�N�g
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateTransformFVEC4(
	npPARTICLETRANSFORM*	pTransform,
	npU32					nStep
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLETRANSFORM_H */
