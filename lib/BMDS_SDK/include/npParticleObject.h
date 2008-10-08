/*=============================================================================
/*!

	@file	npParticleObject.h

	@brief	�p�[�e�B�N���I�u�W�F�N�g��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.23 $
	$Date: 2006/03/28 09:49:00 $

*//*=========================================================================*/
#ifndef	NPPARTICLEOBJECT_H
#define	NPPARTICLEOBJECT_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "core/npContext.h"
#include "npParticleNode.h"
#include "npParticleFrameEmit.h"
#include "npParticleAttribute.h"
#include "npParticleTransform.h"

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
struct __npPARTICLESUBJECT;
struct __npPARTICLECOMPOSITE;

/* �p�[�e�B�N���p�J�X�^���o�[�e�b�N�X�o�b�t�@ */
typedef	struct __npPARTICLECUSTUMVERTEX
{
	npFLOAT	Pos[ 3 ];
	npFLOAT	Normal[ 3 ];
	npCOLOR	Col;
	npFLOAT	Tex[ 2 ];
}
npPARTICLECUSTUMVERTEX;

/*!

	@struct	�p�[�e�B�N���I�u�W�F�N�g�̒��ۍ\����

*/
typedef NP_HSTRUCT __npPARTICLEOBJECT
{
	npPARTICLENODE					Node;

	npFMATRIX						m_matOrigin;	/* �����n�_�}�g���N�X */
	npFMATRIX						m_matGlobal;	/* �\���ʒu�}�g���N�X */

	npPARTICLETRANSFORM				m_vecTrans;		/* ���s�ړ� */
	npPARTICLETRANSFORM				m_vecRot;		/* ��] 	*/
	npPARTICLETRANSFORM				m_vecScale;		/* �g�k		*/

	npFVECTOR						m_vecVel;		/* ���x		*/
	npFVECTOR						m_vecAcc;		/* �����x	*/

	NP_CONST npPARTICLEATTRIBUTE*	m_pAttr;		/* �A�g���r���[�g	*/
	npS32							m_nLifespan;	/* ���g�̎���		*/
	npS32							m_nLocalFrame;	/* ���[�J���t���[��	*/

	npFVECTOR						m_vecParentScale;	/* �e�̃X�P�[����� */

	npU32							m_pReserved[ 1 ];
}
NP_FSTRUCT npPARTICLEOBJECT;

/*!

	@struct	�G�~�b�^�I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLEEMITTER
{
	npPARTICLEOBJECT		Object;

	npPARTICLEFRAMEEMIT*	m_pFirst;
	npU32					m_pReserved[ 3 ];
}
NP_FSTRUCT npPARTICLEEMITTER;

/*!

	@struct	�v���~�e�B�u�I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npPARTICLEPRIMITIVE
{
	npPARTICLEOBJECT		Object;
	
	npFLOAT					m_fAlphaRate;
	npSTATE					m_nPrimType;
	npU32					m_nStride;
	npFLOAT					m_fZValue;
	
	npPARTICLETRANSFORM		m_vecAlpha;
}
NP_FSTRUCT npPARTICLEPRIMITIVE;

/*!

	@struct	�l�p�`�T�[�t�F�C�X

*/
typedef NP_HSTRUCT __npPARTICLEQUADSURFACE
{
	npPARTICLEPRIMITIVE		Primitive;

	npPARTICLETRANSFORM		m_vecVertex[ 4 ];
	npPARTICLETRANSFORM		m_vecColor[ 4 ];
	npPARTICLETRANSFORM		m_vecUVCoord[ 2 ];

	/* ���_�o�b�t�@ */
	npPARTICLECUSTUMVERTEX	m_pCustumVertex[ 4 ];
}
NP_FSTRUCT npPARTICLEQUADSURFACE;

typedef npVOID ( NP_APIENTRY *npPARTICLERENDERFUNC )(
	npCONTEXT*						pContext,
	npPARTICLEPRIMITIVE*			pPrimitive,
	npTEXTURE**						pList,
	npSIZE							nNum,
	npFVECTOR*						pScale,
	npU32							nLightID
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ�L���ɂ���

	@param	pObject		����Ώۂ̃I�u�W�F�N�g
	@param	nMask		�L���ɂ���I�u�W�F�N�g�̏��

*/
#define	npParticleEnableObjectState( pObject, nMask )		\
	npEnableObjectState( ( npOBJECT* )pObject, nMask )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ𖳌��ɂ���

	@param	pObject		����Ώۂ̃I�u�W�F�N�g
	@param	nMask		�����ɂ���I�u�W�F�N�g�̏��

*/
#define	npParticleDisableObjectState( pObject, nMask )		\
	npDisableObjectState( ( npOBJECT* )pObject, nMask )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ��m�F����

	@param	pObject		����Ώۂ̃I�u�W�F�N�g
	@param	nMask		��Ԃ��m�F����I�u�W�F�N�g�̏��

	@retval	NP_TRUE		�L��	
	@retval	NP_FALSE	����

*/
#define	npParticleIsEnabledObjectState( pObject, nMask )	\
	npIsEnabledObjectState( ( npOBJECT* )pObject, nMask )

/*!
	
	�R���|�W�b�g�̃}�g���N�X���Z�b�g����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pTransform	�Z�b�g����}�g���N�X

	@return	�G���[�R�[�h

*/
#define	npParticleSetGlobalFMATRIX( pObject, pTransform )	(	(void)npCopyFMATRIX(	\
	&( pObject )->m_matGlobal, ( pTransform ) ), NP_SUCCESS )

/*!
	
	�R���|�W�b�g�̃}�g���N�X���擾����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pTransform	�擾����}�g���N�X�̃|�C���^

	@return	�G���[�R�[�h

*/
#define	npParticleGetGlobalFMATRIX( pObject, pTransform )	(	npCopyFMATRIX(	\
	( pTransform ),	&( pObject )->m_matGlobal ), NP_SUCCESS )

/*!

	���[�J���t���[�����擾����
	
	@param	pObject	�Ώۂ̃I�u�W�F�N�g
	
	@return	���[�J���t���[��

*/
#define	npParticleGetLocalFrame( pObject )	( ( pObject )->m_nLocalFrame )

/*!
	
	�v���~�e�B�u�̂y�l���Z�b�g����

	@param	pPrimitive	�Ώۂ̃v���~�e�B�u
	@param	val			�y�l

	@return	�G���[�R�[�h

*/
#define npParticleSetZValue( pPrimitive ,val )	( pPrimitive->m_fZValue	= val , NP_SUCCESS )

/*!
	
	�v���~�e�B�u�̂y�l���擾����

	@param	pPrimitive	�Ώۂ̃v���~�e�B�u
	@param	val			�y�l

*/
#define npParticleGetZValue( pPrimitive )	( pPrimitive->m_fZValue )
/*!

	�G�~�b�^�I�u�W�F�N�g�𐶐�����

	@param	pAttirbute	�G�~�b�^�I�u�W�F�N�g�̃A�g���r���[�g
	@param	ppEmitter	���������G�~�b�^���󂯎��ϐ�

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateEmitter(
	NP_CONST	npPARTICLEATTRIBUTE*			pAttribute,
				npPARTICLEEMITTER*				pParent,
				struct __npPARTICLESUBJECT**	ppEmitter 
);

/*!

	�G�~�b�^�I�u�W�F�N�g���������

	@param	pEmitter	�Ώۂ̃G�~�b�^�I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleReleaseEmitter( 
	struct __npPARTICLESUBJECT*		pEmitter
);

/*!

	�G�~�b�^�I�u�W�F�N�g������������

	@param	pEmitter	�Ώۂ̃G�~�b�^�I�u�W�F�N�g
	@param	pCycle		�G�~�b�^�̃A�g���r���[�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupEmitter(
				npPARTICLEEMITTER*		pEmitter,
				npPARTICLEEMITTER*		pParent,
	NP_CONST	npPARTICLEEMITCYCLE*	pCycle
);

/*!

	�G�~�b�^�I�u�W�F�N�g���X�V����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pEmitter	�Ώۂ̃G�~�b�^
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateEmitter( 
	struct __npPARTICLECOMPOSITE*	pComposite,
	struct __npPARTICLESUBJECT*		pEmitter,
	npU32							nStep 
);

/*

	���o�ҋ@���X�g���N���A����

	@param	pEmitter	���o�I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleEmitOutObject(
	npPARTICLEEMITTER*	pEmitter
);

/*

	�q�I�u�W�F�N�g���N���A����

	@param	pEmitter	�e�I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npParticleClearChildObject(
	npPARTICLEEMITTER*	pEmitter
);

/*!

	�l�p�`�T�[�t�F�C�X�𐶐�����

	@param	pAttribute	�l�p�`�T�[�t�F�C�X�̃A�g���r���[�g
	@param	ppSurface	���������l�p�`�T�[�t�F�C�X���󂯎��ϐ�

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleCreateQuadSurface(
	NP_CONST	npPARTICLEATTRIBUTE*			pAttribute,
				npPARTICLEEMITTER*				pParent,
				struct __npPARTICLESUBJECT**	ppSurface 
);

/*!

	�l�p�`�T�[�t�F�C�X���������

	@param	pSurface	�Ώۂ̎l�p�`�T�[�t�F�C�X

*/
NP_API npVOID NP_APIENTRY npParticleReleaseQuadSurface(
	struct __npPARTICLESUBJECT*		pSurface
);

/*!

	�l�p�`�I�u�W�F�N�g������������

	@param	pSurface	�Ώۂ̎l�p�`�I�u�W�F�N�g
	@param	pQuad		�l�p�`�̃A�g���r���[�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npParticleSetupQuadSurface(
				npPARTICLEQUADSURFACE*	pSurface,
				npPARTICLEEMITTER*		pParent,
	NP_CONST	npPARTICLEQUAD*			pQuad
);

/*!

	�l�p�`�T�[�t�F�C�X���X�V����

	@param	pComposite	�Ώۂ̃R���|�W�b�g
	@param	pSurface	�Ώۂ̎l�p�`�T�[�t�F�C�X
	@param	nStep		�t���[���X�e�b�v

*/
NP_API npVOID NP_APIENTRY npParticleUpdateQuadSurface(
	struct __npPARTICLECOMPOSITE*	pComposite,
	struct __npPARTICLESUBJECT*		pSurface,
	npU32							nStep
);

/*!

	�T�[�t�F�C�X��`�悷��

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pPrimitive	�Ώۂ̃T�[�t�F�C�X
	@param	pList		�g�p����e�N�X�`���̔z��
	@param	nNum		�g�p����e�N�X�`���̖���

*/
NP_API npVOID NP_APIENTRY npParticleRenderQuadSurface(
	npCONTEXT*				pContext,
	npPARTICLEPRIMITIVE*	pPrimitive,
	npTEXTURE**				pList,
	npSIZE					nNum,
	npFVECTOR*				pScale,
	npU32					nLIghtID
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLEOBJECT_H */
