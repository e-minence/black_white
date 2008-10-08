/*=============================================================================
/*!

	@file	npParticleAttribute.h

	@brief	�p�[�e�B�N���̑�����`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.31 $
	$Date: 2006/04/06 11:22:34 $

*//*=========================================================================*/
#ifndef	NPPARTICLEATTRIBUTE_H
#define	NPPARTICLEATTRIBUTE_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "core/npTexture.h"
#include "npParticleNode.h"
#include "npParticleFrameCycle.h"
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
/*!

	@enum	�p�[�e�B�N���̎��

*/
enum npPARTICLETYPE
{
	NP_PARTICLE_EMITTER = 0,
	NP_PARTICLE_QUAD,
	//NP_PARTICLE_SPRITE,
	//NP_PARTICLE_PATH,
	//NP_PARTICLE_MODEL,
	//NP_PARTICLE_ACTOR,
	
	NP_PARTICLETYPE_MAX
};

/*!

	@enum	�p�[�e�B�N���̐���

*/
enum npPARTICLESTATE
{
	NP_PARTICLE_BILLBOARD		= 1,									//!< �r���{�[�h
	NP_PARTICLE_AXISBILLBOARD	= NP_PARTICLE_BILLBOARD			<< 1,	//!< ���Œ�r���{�[�h
	NP_PARTICLE_SUBDIVISION		= NP_PARTICLE_AXISBILLBOARD		<< 1,	//!< �����|���S��
	NP_PARTICLE_ASPECT_LOCK		= NP_PARTICLE_SUBDIVISION		<< 1,	//!< �A�X�y�N�g���b�N
	NP_PARTICLE_ACCELERATION	= NP_PARTICLE_ASPECT_LOCK		<< 1,	//!< �O�������x

	NP_PARTICLE_TEXTURE			= NP_PARTICLE_ACCELERATION 		<< 1,	//!< �e�N�X�`��
	NP_PARTICLE_TRANSPARENCY	= NP_PARTICLE_TEXTURE			<< 1,	//!< �A���t�@
	NP_PARTICLE_FILL_SMOOTH		= NP_PARTICLE_TRANSPARENCY		<< 1,	//!< �O�[���V�F�[�f�B���O
	NP_PARTICLE_LIFESPAN		= NP_PARTICLE_FILL_SMOOTH		<< 1,	//!< ���C�t�ŏI��

	NP_PARTICLE_PARENT_TRANS	= NP_PARTICLE_LIFESPAN			<< 1,	//!< �e�� Translation
	NP_PARTICLE_PARENT_ROT		= NP_PARTICLE_PARENT_TRANS		<< 1,	//!< �e�� Rotation
	NP_PARTICLE_PARENT_ROT_Z	= NP_PARTICLE_PARENT_ROT		<< 1,	//!< �e�� Z Rotation
	NP_PARTICLE_PARENT_ROT_ONCE	= NP_PARTICLE_PARENT_ROT_Z		<< 1,	//!< �������̂ݐe�� Rotation 
	
	NP_PARTICLE_PARENT_LIFE		= NP_PARTICLE_PARENT_ROT_ONCE	<< 1,	//!< �e�̃��C�t���p������

	//!<
	//!<
	//!< 

	//!< 
	//!< 
	//!<
	//!<
	NP_PARTICLE_FINISHED		= NP_PARTICLE_PARENT_LIFE		<< 1,	//!< �I���t���O
	NP_PARTICLE_EMITOUT			= NP_PARTICLE_FINISHED			<< 1,	//!< �P�T�C�N�������o���ďI��
	NP_PARTICLE_REPEAT			= NP_PARTICLE_EMITOUT			<< 1,	//!< 
	NP_PARTICLE_INV_PRIORITY	= NP_PARTICLE_REPEAT			<< 1,	//!< �p�[�e�B�N���̕`��D��x
	NP_PARTICLE_ZSORT			= NP_PARTICLE_INV_PRIORITY		<< 1,	//!< �p�[�e�B�N����Z�l�ɂ��`��D��x

	NP_PARTICLE_NORMAL			= NP_PARTICLE_ZSORT				<< 1,	//!< �y�����ɖ@��������
	NP_PARTICLE_NORMAL_EX		= NP_PARTICLE_NORMAL			<< 1	//!< ���������̂y�����ɖ@��������

};

/*
@enum
*/
enum npPARTICLEUSERSTATE
{
	NP_PARTICLE_VISIBILITY = 1
};


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
struct __npPARTICLECOMPONENT;

/*!

	@struct	�p�[�e�B�N�������̒��ۍ\����

*/
typedef NP_HSTRUCT __npPARTICLEATTRIBUTE
{
	npPARTICLENODE			Node;

	npPARTICLEFRAMECYCLE	Cycle;			/* ���o�^�C�~���O */

	npPARTICLEKINEMATICS	Translate;		/* ���s�ړ����	*/
	npPARTICLEKINEMATICS	Rotate;			/* ��]���	*/
	npPARTICLEKINEMATICS	Scale;			/* �g�k���	*/

	npFVECTOR				m_vecAccel;		/* �����x��� */

	npS32					m_nLifespan;	/* ���� */
	npS32					m_nLiferange;	/* �����̃����_���͈� */
	
	npU32					m_pReserved[ 2 ];
}
NP_FSTRUCT npPARTICLEATTRIBUTE;

/*!

	@struct	�p�[�e�B�N�����o�^�C�~���O

*/
typedef NP_HSTRUCT __npPARTICLEEMITCYCLE
{
	npPARTICLEATTRIBUTE		Attribute;
}
NP_FSTRUCT npPARTICLEEMITCYCLE;

/*!

	@struct	�p�[�e�B�N���A�C�e��

*/
typedef NP_HSTRUCT __npPARTICLEITEM
{
	npPARTICLEATTRIBUTE		Attribute;

	npPARTICLEKINEMATICS	m_vecAlpha;		/* �A���t�@			*/
	
	npU32					m_nTexID;		/* �e�N�X�`��ID		*/	
	npU32					m_nPriority;	/* �`��D�揇��		*/
	npSTATE					m_nSrcBlend;	/* �u�����h�W��		*/
	npSTATE					m_nDstBlend;	/* �u�����h�W��		*/
}
NP_FSTRUCT npPARTICLEITEM;

/*!

	@struct	�l�p�`�A�C�e��

*/
typedef NP_HSTRUCT __npPARTICLEQUAD
{
	npPARTICLEITEM			Item;

	npPARTICLEKINEMATICS	m_vecVertex[ 4 ];
	npPARTICLEKINEMATICS	m_vecColor[ 4 ];
	npPARTICLEKINEMATICS	m_vecUVCoord[ 2 ];
}
NP_FSTRUCT npPARTICLEQUAD;

#if 0 
/*!

	@struct	���f���A�C�e��

*/
typedef struct __npPARTICLESHAPE
{
	npPARTICLEITEM	Item;
	npU32			m_pReserved[ 4 ];
}
__attribute__( ( aligned( 16 ) ) ) npPARTICLESHAPE;

/*!

*/
typedef struct __npPARTICLEPATH
{
	npPARTICLEITEM	Item;
	npU32			m_pReserved[ 4 ];
}
__attribute__( ( aligned( 16 ) ) ) npPARTICLEPATH;

/*!

	@struct	�A�N�^�[�A�C�e��

*/
typedef struct __npPARTICLEACTOR
{
	npPARTICLEMODEL	Model;
	npU32			m_pReserved[ 4 ];
}
__attribute__( ( aligned( 16 ) ) ) npPARTICLEACTOR;

#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
#if 0
/*!

	���o�^�C�~���O�I�u�W�F�N�g���������

	@param	pCycle

*/
#define	npParticleReleaseStructure( pCycle )	\
	npParticleReleaseEmitCycle( ( npPARTICLEEMITCYCLE* )pCycle )
#endif

/*!

	���o�^�C�~���O�I�u�W�F�N�g�𐶐�����

	@param	ppCycle		�󂯎��ϐ�

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateEmitCycle( 
	npPARTICLEEMITCYCLE**	ppCycle
);

/*!

	���o�^�C�~���O�I�u�W�F�N�g���J������

	@param	pCycle	�Ώۂ̃I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseEmitCycle( 
	struct	__npPARTICLECOMPONENT* 	pCycle
);

/*!

	�l�p�`�I�u�W�F�N�g�𐶐�����

	@param	ppQuad		�󂯎��ϐ�

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateQuad( 
	npPARTICLEQUAD**	ppQuad 
);

/*!

	�l�p�`�I�u�W�F�N�g���J������

	@param	pQuad	�Ώۂ̃I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseQuad( 
	struct __npPARTICLECOMPONENT*	pQuad 
);

/*!

	�A�g���r���[�g�Ɏ������Z�b�g����

	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g
	@param	nLength		�Z�b�g��������̒���

	@return	�G���[�R�[�h

*/
#define	npParticleSetLifespan( pAttribute, nLength )	(	\
	( pAttribute )->m_nLifespan = ( nLength ), NP_SUCCESS )
//	( pAttribute )->m_nLifespan = ( nLength ) * 200, NP_SUCCESS )

/*!

	�A�g���r���[�g�̎����̒������擾����

	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g

	@return	�����̒���

*/
#define	npParticleGetLifespan( pAttribute )	( ( pAttribute )->m_nLifespan )
//#define	npParticleGetLifespan( pAttribute )	( ( pAttribute )->m_nLifespan / 200 )
	
/*!

	�A�g���r���[�g�Ɏ����̃����_�������Z�b�g����

	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g
	@param	nRange		�Z�b�g��������̃����_����

	@return	�G���[�R�[�h

*/
#define	npParticleSetLiferange( pAttribute, nRange )	(	\
	( pAttribute )->m_nLiferange = ( nRange ), NP_SUCCESS )
//	( pAttribute )->m_nLiferange = ( nRange ) * 200, NP_SUCCESS )

/*!

	�A�g���r���[�g�̎����̃����_�������擾����

	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g

	@return	�����̃����_����

*/
#define	npParticleGetLiferange( pAttribute )	( ( pAttribute )->m_nLiferange )
//#define	npParticleGetLiferange( pAttribute )	( ( pAttribute )->m_nLiferange / 200 )


/*!

	�A�g���r���[�g�̊O�������x���Z�b�g����

	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g
	@param	pVec		�Z�b�g���� npFVEC3 �^�̒l

	@return	�G���[�R�[�h

*/
#define	npParticelSetAccelFVEC3( pAttribute, pVec )	(	npSetFVEC4(	\
	&( pAttribute )->m_vecAccel, NP_X( pVec ), NP_Y( pVec ), NP_Z( pVec ), FX32_ONE ), NP_SUCCESS )

/*!

	�A�g���r���[�g�̊O�������x���擾����
	
	@param	pAttribute	�Ώۂ̃A�g���r���[�g�I�u�W�F�N�g
	@param	pVec		�擾�����l���������� npFVEC3 �^�̃|�C���^

*/
#define	npParticleGetAccelFVEC3( pAttribute, pVec )	(	\
	npCopyFVEC3( ( pVec ), ( npFVEC3* )&( pAttribute )->m_vecAccel ), NP_SUCCESS )

/*!

	�A�C�e���ɕ`�揇���̒l���Z�b�g����

	@param	pItem		�Ώۂ̃A�C�e��
	@param	nPriority	�`�揇��

	@return	�G���[�R�[�h

*/
#define	npParticleSetRenderPrioriy( pItem, nPriority )	(	\
	( pItem )->m_nPriority = ( nPriority ), NP_SUCCESS )

/*!

	�A�C�e���̕`�揇���̒l���擾����

	@param	pItem		�Ώۂ̃A�C�e��
	
	@return	�`�揇��

*/
#define	npParticleGetRenderPrioriy( pItem )	( ( pItem )->m_nPriority )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEATTRIBUTE_H */
