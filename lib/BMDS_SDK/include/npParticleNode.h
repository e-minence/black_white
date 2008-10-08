/*=============================================================================
/*!

	@file	npParticleNode.h

	@brief	�p�[�e�B�N���̊K�w��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.6 $
	$Date: 2006/03/13 06:13:22 $

*//*=========================================================================*/
#ifndef	NPPARTICLENODE_H
#define	NPPARTICLENODE_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npObject.h"

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

	@struct	�p�[�e�B�N���̊K�w�m�[�h

*/
typedef NP_HSTRUCT __npPARTICLENODE
{
	npOBJECT	Object;

	struct	__npPARTICLENODE*	m_pParent;
	struct	__npPARTICLENODE*	m_pSibling;
	struct	__npPARTICLENODE*	m_pChild;

	npU32	m_pReserved[ 1 ];
}
NP_FSTRUCT npPARTICLENODE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	���g�̐e�m�[�h���擾����

	@param	pNode	�Ώۂ̃m�[�h

	@return	���g�̐e�m�[�h
	@retval	NP_NULL		���g�����[�g�m�[�h

*/
#define	npParticleParentNode( pNode )	( ( pNode )->m_pParent )

/*!

	���g�̌Z��m�[�h���擾����

	@param	pNode	�Ώۂ̃m�[�h

	@return	���g�̌Z��m�[�h
	@retval	NM_NULL		���g���I�[�m�[�h

*/
#define	npParticleSiblingNode( pNode )	( ( pNode )->m_pSibling )

/*!

	���g�̎q�m�[�h���擾����

	@param	pNode	�Ώۂ̃m�[�h

	@return	���g�̎q�m�[�h
	@retval	NM_NULL		���g�����[�t�m�[�h

*/
#define	npParticleChildNode( pNode )	( ( pNode )->m_pChild )

/*!

	���g�����[�g�m�[�h�����肷��

	@param	pNode	�Ώۂ̃m�[�h

	@retval	NP_TRUE		���[�g�m�[�h
	@retval	NP_FALSE	���[�g�ł͂Ȃ�

*/
#define	npParticleIsRootNode( pNode )	(	\
	( ( pNode )->m_pParent == NP_NULL ) ? NP_TRUE : NP_FALSE )

/*!

	���g�̃m�[�h�����[�t�m�[�h�����肷��

	@param	pNode	�Ώۂ̃m�[�h

	@retval	NP_TRUE		���[�t�m�[�h
	@retval	NP_FALSE	���[�t�m�[�h�ł͂Ȃ�

*/
#define	npParticleIsLeafNode( pNode )	(	\
	( ( pNode )->m_pChild == NP_NULL ) ? NP_TRUE : NP_FALSE )

/*!

	�e�m�[�h�Ɏ��g�̃m�[�h��ǉ�����

	@param	pParent	�e�m�[�h
	@param	pNode	�ǉ�����m�[�h

	@note	pNode �̓��[�g( �e���Ȃ� )�ł���K�v������

*/
NP_API npVOID NP_APIENTRY npParticlePushNode( 
	npPARTICLENODE* pParent, 
	npPARTICLENODE* pNode 
);

/*!

	�e�m�[�h���玩�M�̃m�[�h���O��

	@param	pNode	���g�̃m�[�h

*/
NP_API npVOID NP_APIENTRY npParticlePopNode( npPARTICLENODE* pNode );

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPPARTICLENODE_H */
