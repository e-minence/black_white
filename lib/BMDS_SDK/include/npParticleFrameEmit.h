/*=============================================================================
/*!

	@file	npParticleFrameEmit.h

	@brief	���o�ҋ@�I�u�W�F�N�g��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.8 $
	$Date: 2006/03/13 06:13:23 $

*//*=========================================================================*/
#ifndef	NPPARTICLEFRAMEEMIT_H
#define	NPPARTICLEFRAMEEMIT_H

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

	@struct	�R���X�^���g�����ҋ@�I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLECNSTEMIT
{
	npOBJECT	Object;

	npS32		m_nCycle;		/* �c�胋�[�v��	*/
	npS32		m_nRate;		/* ���݂܂ł̐����� */
	npS32		m_nInterval;	/* �x�����̎c�莞��	*/

	npU32		m_nReserved[ 1 ];
}
NP_FSTRUCT npPARTICLECNSTEMIT;

/*!

	@struct	�V�[�P���X�����ҋ@�I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLESEQEMIT
{
	npOBJECT	Object;

	npS32		m_nCycle;		/* �c��̃��[�v��	*/
	npS32		m_nStep;

	npU32		m_pReserved[ 2 ];
}
NP_FSTRUCT npPARTICLESEQEMIT;

/*!

	@struct	�����ҋ@�I�u�W�F�N�g�X�^�C��

*/
typedef union __npPARTICLEFRAMEEMITSTYLE
{
	npPARTICLECNSTEMIT	Constant;
	npPARTICLESEQEMIT	Sequence;
}
npPARTICLEFRAMEEMITSTYLE;

/*!

	@struct	�����ҋ@�I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLEFRAMEEMIT
{
	npPARTICLEFRAMEEMITSTYLE		Style;

	NP_CONST npPARTICLEATTRIBUTE*	m_pAttr;
	npS32							m_nFrameLength;	/* ���t���[����		*/
	npS32							m_nLocalFrame;	/* ���݂̃t���[���� */
	struct __npPARTICLEFRAMEEMIT*	m_pNext;
}
NP_FSTRUCT npPARTICLEFRAMEEMIT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	���o�ҋ@�I�u�W�F�N�g�ŕK�v�ȃ������T�C�Y���擾����

	@param	nNum	�g�p������o�ҋ@�I�u�W�F�N�g�̐�

	@return	�K�v�ȃ������T�C�Y

*/
#define	npParticleCheckFrameEmitHeapSize( nNum )	(	\
	( npSIZE )( sizeof( npPARTICLEFRAMEEMIT ) * ( nNum ) ) )

/*!

	���o�ҋ@�I�u�W�F�N�g�̃A���[�i������������

	@param	pBuf	���o�ҋ@�I�u�W�F�N�g�̃A���[�i�̈�
	@param	nSize	��L�̃������T�C�Y

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleInitFrameEmitList( 
	npVOID*	pBuf,
	npSIZE 	nSize
);

/*!

	���o�ҋ@�I�u�W�F�N�g�̃A���[�i�̈�̏I������

*/
NP_API npVOID NP_APIENTRY npParticleExitFrameEmitList( npVOID );

/*!

	���o�ҋ@�I�u�W�F�N�g�𐶐�����

	@param	pCycle		���o�T�C�N��
	@param	ppFrame		�����������o�ҋ@�I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateFrameEmit(
	NP_CONST	npPARTICLEATTRIBUTE*	pAttribute,
				npPARTICLEFRAMEEMIT**	ppFrame
);

/*!

	���o�ҋ@�I�u�W�F�N�g���������

	@param	pFrame	�Ώۂ̃I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseFrameEmit(
	npPARTICLEFRAMEEMIT*	pFrame
);

/*!

	���o����I�u�W�F�N�g�̐����擾����

	@param	pFrame	�Ώۂ̃I�u�W�F�N�g
	@param	nStep	�X�e�b�v��

	@return	���o����I�u�W�F�N�g�̐�

*/
NP_API npU32 NP_APIENTRY npParticleCountFrameEmitObject(
	npPARTICLEFRAMEEMIT*	pFrame,
	npU32					nStep
);

/*!

	�Ώۂ̃I�u�W�F�N�g�����X�g�ɒǉ�����

	@param	pFirst	���X�g�̐擪�I�u�W�F�N�g
	@param	pFrame	�Ώۂ̃I�u�W�F�N�g

	@return	���X�g�̐擪�I�u�W�F�N�g

	@note	�ǉ������ꏊ�̓V�X�e���Ɉˑ�

*/
#define	npParticleInsertFrameEmit( pFirst, pFrame )	(	\
	( pFirst ) == NP_NULL ? ( pFrame ) : ( ( pFrame )->m_pNext = ( pFirst ), ( pFrame ) ) )

/*!

	���X�g����Ώۂ̃I�u�W�F�N�g���O��

	@param	pFirst	���X�g�̐擪�I�u�W�F�N�g
	@param	pFrame	�Ώۂ̃I�u�W�F�N�g

	@return	���X�g�̐擪�I�u�W�F�N�g

*/
NP_API npPARTICLEFRAMEEMIT*	npParticleRemoveFrameEmit(
	npPARTICLEFRAMEEMIT*	pFirst,
	npPARTICLEFRAMEEMIT*	pFrame
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEFRAMEEMIT_H */
