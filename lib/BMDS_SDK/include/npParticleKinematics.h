/*=============================================================================
/*!

	@file	npParticleKinematics.h

	@brief	�p�[�e�B�N��������`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.14 $
	$Date: 2006/03/28 09:38:25 $

*//*=========================================================================*/
#ifndef	NPPARTICLEKINEMATICS_H
#define	NPPARTICLEKINEMATICS_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npObject.h"
#include "npParticleKeyFrame.h"

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
/*!

	@enum	�����̎��

*/
enum npPARTICLEKINEMATICSTYPE
{
	NP_PARTICLE_FIXED = 0,	/*!< �Œ�l�p�^�[�� */
	NP_PARTICLE_VELOCITY,	/*!< �����x�p�^�[�� */
	NP_PARTICLE_FCURVE,		/*!< FCURVE�p�^�[�� */

	NP_PARTICLE_KINEMATICS_MAX
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@struct	�Œ�l�p�^�[��

*/
typedef NP_HSTRUCT __npPARTICLEFIXED
{
	npOBJECT	Object;

	npFVECTOR	m_vecInitial;	/* �����l */
	npFVECTOR	m_vecRange;		/* �����l�̃����_���� */
}
NP_FSTRUCT npPARTICLEFIXED;

/*!

	@struct	�����x�p�^�[��

*/
typedef NP_HSTRUCT __npPARTICLEVELOCITY
{
	npOBJECT	Object;

	npFVECTOR	m_vecInitial;		/* �����l */
	npFVECTOR	m_vecRange;			/* �����l�̃����_���� */
	npFVECTOR	m_vecVelocity;		/* ���x */
	npFVECTOR	m_vecVelrange;		/* ���x�̃����_���� */
	npFVECTOR	m_vecAccel;			/* �����x */
	npFVECTOR	m_vecAccrange;		/* �����x�̃����_���� */
	npFVECTOR	m_vecRateofChange;	/* �ω��� */
}
NP_FSTRUCT npPARTICLEVELOCITY;

/*!

	@struct	�t�@���N�V�����J�[�u�p�^�[��

*/
typedef NP_HSTRUCT __npPARTICLEFCURVE
{
	npOBJECT	Object;

	npFVECTOR	m_vecInitial;	/* �����l */
	npFVECTOR	m_vecRange;		/* �����l�̃����_���� */
	npFVECTOR	m_vecDirection;	/* �ω����� */
	npFVECTOR	m_vecDirRange;	/* �ω������̃����_���� */

#if 0
	npSTATE		m_nRepeat;		/* �Đ����@ */
	npSTATE		m_nInterp;		/* ��Ԃ̎�� */
	npBOOL		m_nMode;		/* ���䃂�[�h */
	npSTATE		m_nUnit;		/* �L�[�t���[���̒P�� ( npPARTICLEFRAMEUNIT ) */
	
	npU32		m_pReserved[ 1 ];
#else
	/* 
		Repeat(�Đ����@) | Interp(��ԕ��@) | Mode(���䃂�[�h) | Unit(�L�[�t���[���̒P��)
		�e�S�o�C�g����
	*/
	npSTATE		m_nConfig;
	npU32		m_pReserved[ 2 ];		
#endif
	npU32			m_nKeys;	/* �L�[�t���[���� */
//	npPARTICLEFKEY	m_pKeyFrames[ NP_PARTICLE_KEYFRAME_MAX ];
	npPARTICLEFKEY*	m_pKeyFrames;
}
NP_FSTRUCT npPARTICLEFCURVE;

/*!

	@union	�����X�^�C��

*/
typedef union __npPARTICLEKINEMATICSSTYLE
{
	npPARTICLEFIXED		Fixed;		/*!< �Œ�l�p�^�[�� */
	npPARTICLEVELOCITY	Velocity;   /*!< �����x�p�^�[�� */
	npPARTICLEFCURVE	FCurve;     /*!< FCURVE�p�^�[�� */
}
npPARTICLEKINEMATICSSTYLE;

/*!

	@struct	������`

*/
typedef NP_HSTRUCT __npPARTICLEKINEMATICS
{
	npPARTICLEKINEMATICSSTYLE	Style;
}
NP_FSTRUCT npPARTICLEKINEMATICS;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*

*/
#define	npParticleSetFCurveRepeatState( pFCurve, nState )	( ( pFCurve )->m_nConfig =	\
	( ( ( pFCurve )->m_nConfig & 0x00FFFFFF ) | ( ( ( npU8 )( nState ) ) << 24 ) ), NP_SUCCESS )

/*

*/
#define	npParticleGetFCurveRepeatState( pFCurve )	(		\
	( ( pFCurve )->m_nConfig & 0xFF000000 ) >> 24 )

/*

*/
#define	npParticleSetFCurveInterpState( pFCurve, nState )	( ( pFCurve )->m_nConfig =	\
	( ( ( pFCurve )->m_nConfig & 0xFF00FFFF ) | ( ( ( npU8 )( nState ) ) << 16 ) ), NP_SUCCESS )

/*

*/
#define	npParticleGetFCurveInterpState( pFCurve )	(		\
	( ( pFCurve )->m_nConfig & 0x00FF0000 ) >> 16 )

/*

*/
#define	npParticleEnableFCurveControlMode( pFCurve )	(	\
	( pFCurve )->m_nConfig |= 0x0000FF00, NP_SUCCESS )

/*

*/
#define	npParticleDisableFCurveControlMode( pFCurve )	(	\
	( pFCurve )->m_nConfig &= 0xFFFF00FF, NP_SUCCESS )

/*

*/
#define	npParticleIsEnabledFCurveControlMode( pFCurve )	(	\
	( ( pFCurve )->m_nConfig & 0x0000FF00 ) ? NP_TRUE : NP_FALSE )

/*

*/
#define	npParticleSetFCurveFKeyUnit( pFCurve, nState )	( ( pFCurve )->m_nConfig =	\
	( ( ( pFCurve )->m_nConfig & 0xFFFFFF00 ) | ( ( npU8 )( nState ) ) ), NP_SUCCESS )

/*

*/
#define	npParticleGetFCurveFKeyUnit( pFCurve )	( ( pFCurve )->m_nConfig & 0x000000FF )

/*!
	F�J�[�u�̃L�[�t���[���𐶐�����
*/
npERROR npParticleCreateFCurveFkey( npPARTICLEFCURVE *pFCrv, npU32 nKey );

/*!
	F�J�[�u�̃L�[�t���[�����J������
*/
npVOID npParticleReleaseFCurveFkey( npPARTICLEFCURVE *pFCrv );



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEKINEMATICS_H */
