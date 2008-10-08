/*=============================================================================
/*!

	@file	npParticleFrameCycle.h

	@brief	�p�[�e�B�N���̔���������`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.15 $
	$Date: 2006/03/28 09:38:25 $

*//*=========================================================================*/
#ifndef	NPPARTICLEFRAMECYCLE_H
#define	NPPARTICLEFRAMECYCLE_H

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

	�����T�C�N���̎��

*/
enum npPARTICLEFRAMECYCLETYPE
{
	NP_PARTICLE_CNSTCYCLE = 0,
	NP_PARTICLE_SEQCYCLE,

	NP_PARTICLE_FRAMECYCLE_MAX
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@struct	�R���X�^���g�����o�^�[��

*/
typedef NP_HSTRUCT __npPARTICLECNSTCYCLE
{
	npOBJECT	Object;

	npBOOL		m_bEach;		/* �x���̔����L�� */
	npBOOL		m_bDelay;		/* �x���̊J�n�� */
	npS32		m_nInterval;	/* �x������ */
	npS32		m_nRange;		/* �x���̃����_���� */
}
NP_FSTRUCT npPARTICLECNSTCYCLE;

/*!

	@struct	�V�[�P���X�����p�^�[��

*/
typedef NP_HSTRUCT __npPARTICLESEQCYCLE
{
	npOBJECT		Object;
	npU32			m_pReserved[ 3 ];
	npU32			m_nKeys;
	npPARTICLEFKEY	m_pKeyFrames[ NP_PARTICLE_KEYFRAME_MAX ];
}
NP_FSTRUCT npPARTICLESEQCYCLE;

/*!

	@union	�����T�C�N���X�^�C��

*/
typedef union __npPARTICLEFRAMECYCLESTYLE
{
	npPARTICLECNSTCYCLE	Constant;
	npPARTICLESEQCYCLE	Sequence;
}
npPARTICLEFRAMECYCLESTYLE;

/*!

	@struct	�����T�C�N��

*/
typedef NP_HSTRUCT __npPARTICLEFRAMECYCLE
{
	npPARTICLEFRAMECYCLESTYLE	Style;

	npS32	m_nCycle;	/* ������ */
	npS32	m_nRate;	/* ������̐����� */
	npS32	m_nLength;	/* ������̃t���[������ */

	npU32	m_pReserved[ 1 ];
}
NP_FSTRUCT npPARTICLEFRAMECYCLE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	���������Z�b�g����

	@param	pCycle	�����T�C�N��
	@param	nNum	������

	@return	�G���[�R�[�h

*/
#define	npParticleSetFrameCycleNum( pCycle, nNum )	\
	( ( pCycle )->m_nCycle = ( nNum ), NP_SUCCESS )
 
/*!

	���������擾����

	@param	pCycle	�����T�C�N��
	
	@return	������

*/
#define	npParticleGetFrameCycleNum( pCycle )	\
	( ( pCycle )->m_nCycle )

/*!

	������̔��������Z�b�g����

	@param	pCycle	�����T�C�N��
	@param	nRate	������

	@return	�G���[�R�[�h

*/
#define	npParticleSetFrameCycleRate( pCycle, nNum )	\
	( ( pCycle )->m_nRate = ( nNum ), NP_SUCCESS )

/*!

	������̔��������擾����

	@param	pCycle	�����T�C�N��
	
	@return	������

*/
#define	npParticleGetFrameCycleRate( pCycle )	\
	( ( pCycle )->m_nRate )

/*!

	������̒������Z�b�g����

	@param	pCycle	�����T�C�N��
	@param	nLength	������̒���

	@return	�G���[�R�[�h

*/
#define	npParticleSetFrameCycleLength( pCycle, nFrame )	\
	( ( pCycle )->m_nLength = ( nFrame ) , NP_SUCCESS )
//	( ( pCycle )->m_nLength = ( nFrame ) * 200, NP_SUCCESS )

/*!

	������̒������擾����

	@param	pCycle	�����T�C�N��

	@return	������̒���

*/
#define	npParticleGetFrameCycleLength( pCycle )	\
	( ( pCycle )->m_nLength )
//	( ( pCycle )->m_nLength / 200 )

/*!

	�x���̔�����L���ɂ���

	@param	pCycle	�����T�C�N��

	@return	�G���[�R�[�h

*/
#define	npParticleEnableConstCycleEachDelay( pCycle )	\
	( ( pCycle )->m_bEach = NP_TRUE, NP_SUCCESS )

/*!

	�x���̔����𖳌��ɂ���

	@param	pCycle	�����T�C�N��

	@return	�G���[�R�[�h

*/
#define	npParticleDisableConstCycleEachDelay( pCycle )	\
	( ( pCycle )->m_bEach = NP_FALSE, NP_SUCCESS )

/*!

	�x���̔����̏�Ԃ��擾����

	@param	pCycle	�����T�C�N��

	@retval	NP_TRUE		�L��
	@retval	NP_FALSE	����

*/
#define	npParticleIsEnabledConstCycleEachDelay( pCycle )	\
	( ( pCycle )->m_bEach )

/*!

	�J�n���̒x����L���ɂ���

	@param	pCycle	�����T�C�N��

	@return	�G���[�R�[�h

*/
#define	npParticleEnableConstCycleStartDelay( pCycle )	\
	( ( pCycle )->m_bDelay = NP_TRUE, NP_SUCCESS )

/*!

	�J�n���̒x���𖳌��ɂ���

	@param	pCycle	�����T�C�N��

	@return	�G���[�R�[�h

*/
#define	npParticleDisableConstCycleStartDelay( pCycle )	\
	( ( pCycle )->m_bDelay = NP_FALSE, NP_SUCCESS )

/*!

	�J�n���̒x���̏�Ԃ��擾����

	@param	pCycle	�����T�C�N��

	@retval	NP_TRUE		�L��
	@retval	NP_FALSE	����

*/
#define	npParticleIsEnabledConstCycleStartDelay( pCycle )	\
	( ( pCycle )->m_bDelay )

/*!

	�x�����Ԃ��Z�b�g����

	@param	pCycle	�����T�C�N��
	@param	nFrame	�x������

	@return	�G���[�R�[�h

*/
#define npParticleSetConstCycleIntervalLength( pCycle, nLength )	\
	( ( pCycle )->m_nInterval = ( nLength ) , NP_SUCCESS )
//	( ( pCycle )->m_nInterval = ( nLength ) * 200, NP_SUCCESS )

/*!

	�x�����Ԃ��擾����

	@param	pCycle	�����T�C�N��

	@return	�x������

*/
#define	npParticleGetConstCycleIntervalLength( pCycle )	\
	( ( pCycle )->m_nInterval )
//	( ( pCycle )->m_nInterval / 200 )

/*!

	�x�����Ԃ̃����_�������Z�b�g����

	@param	pCycle	�����T�C�N��
	@param	nRange	�����_����

	@return	�G���[�R�[�h

*/
#define	npParticleSetConstCycleIntervalRange( pCycle, nRange )	\
	( ( pCycle )->m_nRange = ( nRange ) , NP_SUCCESS )
//	( ( pCycle )->m_nRange = ( nRange ) * 200, NP_SUCCESS )

/*!

	�x�����Ԃ̃����_�������擾����

	@param	pCycle	�����T�C�N��

	@return	�x�����Ԃ̃����_����

*/
#define	npParticleGetConstCycleIntervalRange( pCycle )	\
	( ( pCycle )->m_nRange )
//	( ( pCycle )->m_nRange / 200 )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEFRAMECYCLE_H */
