/*=============================================================================
/*!

	@file	npParticleKeyFrame.h

	@brief	�L�[�t���[���p��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.19 $
	$Date: 2006/03/28 09:38:25 $

*//*=========================================================================*/
#ifndef	NPPARTICLEKEYFRAME_H
#define	NPPARTICLEKEYFRAME_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "core/npTypedef.h"

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
#define	NP_PARTICLE_KEYFRAME_MAX	( 16 )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------
/*!

	@enum	�L�[�t���[���̒P��

*/
enum npPARTICLEFRAMEUNIT
{
	NP_PARTICLE_FRAME_LENGTH = 0,
	NP_PARTICLE_FRAME_RATIO
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@union	FCURVE �L�[�̃X�^�C��

*/
typedef union __npPARTICLEFKEYSTYLE
{
	npFVECTOR	m_vecFValue;
	npIVECTOR	m_vecUValue;
}
npPARTICLEFKEYSTYLE;

/*!

	@union	�t���[���̒P��
	
*/
typedef union __npPARTICLEFRAMESTYLE
{
	npU32		m_nFrame;
	npFLOAT		m_fRatio;
}
npPARTICLEFRAMESTYLE;

/*!

	@struct	FCURVE �̃L�[

*/
typedef struct __npPARTICLEFKEY
{
	npPARTICLEFKEYSTYLE		Style;	// Value
	npPARTICLEFRAMESTYLE	Unit;	// Frame �ɕύX
	struct __npPARTICLEFKEY		*m_pNext;	// ����FKEY
	npU32	m_pReserved[ 2 ];
}
NP_FSTRUCT npPARTICLEFKEY;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	F�J�[�u�L�[�ŕK�v�ȃ������T�C�Y���擾����

	@param	nNum	�g�p����F�J�[�u�L�[�̐�

	@return	�K�v�ȃ������T�C�Y

*/
#define	npParticleCheckFKeyHeapSize( nNum )	(			\
	( npSIZE )( sizeof( npPARTICLEFKEY ) * ( nNum ) ) )

/*!
	�w��̒�����F�J�[�u�L�[�z����m��
*/
npERROR NP_APIENTRY npParticleAllocFKeys( npPARTICLEFKEY **ppFkeys, npU32 nKey );

/*!
	F�J�[�u�L�[�z����J������
*/
npVOID NP_APIENTRY npParticleFreeFKeys( npPARTICLEFKEY *pFkeys );

/*!
	F�J�[�u�L�[�̃A���[�i������������

	@param	pBuf	FCurve�p�̐擪�o�b�t�@
	@param	nSize	�m�ۂ����o�b�t�@�̃T�C�Y

*/
npERROR NP_APIENTRY npParticleInitFKeyFactory( npVOID* pBuf, npSIZE nSize );
/*!
	F�J�[�u�L�[�̃A���[�i�̈�̏I������
*/
npVOID NP_APIENTRY npParticleExitFkeyFactory( npVOID );

/*!

*/
#define	npParticleGetFrameFKEY( pKey, nUnit, nLength )	(				\
	( nUnit ) == NP_PARTICLE_FRAME_LENGTH ? ( pKey )->Unit.m_nFrame : 	\
	( npU32 )(FX_Mul((pKey)->Unit.m_fRatio, ((nLength)<<FX32_SHIFT))>>FX32_SHIFT) )
	
/*!
	
	�R���|�[�l���g�̃������g�p�ʂ�Ԃ�

*/
NP_API npU32 NP_APIENTRY npParticleGetUsingFKeySize( npVOID );



/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEKEYFRAME_H */
