/*=============================================================================
/*!

	@file 	npObject.h

	@brief	���ۓI�ȃI�u�W�F�N�g��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.9 $
	$Date: 2006/03/22 06:49:06 $

*//*=========================================================================*/
#ifndef	NPOBJECT_H
#define	NPOBJECT_H

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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------
/*!

	@enum	�I�u�W�F�N�g�^�C�v

*/
enum npOBJECTTYPE
{
	NP_UNKNOWN = 0,	/*!< �s���ȃI�u�W�F�N�g 	*/

	NP_LIGHT,
	NP_MATERIAL,	/*!< �}�e���A��				*/
	NP_TEXTURE,		/*!< �e�N�X�`���I�u�W�F�N�g */
	NP_SHADER,		/*!< �V�F�[�_�I�u�W�F�N�g	*/

	NP_LIGHT_POINT			= NP_MAKEINT32( NP_LIGHT, 1 ),
	NP_LIGHT_SPOT			= NP_MAKEINT32( NP_LIGHT, 2 ),
	NP_LIGHT_DIRECTIONAL	= NP_MAKEINT32( NP_LIGHT, 3 )
};

/*!

	@enum	�I�u�W�F�N�g�X�e�[�g

*/
enum npOBJECTSTATE
{
	NP_STATE_NONE					= 0x00000000,
	NP_STATE_ALREADY_INITIALIZED	= 1,
	NP_STATE_UNRELEASED_INSTANCE	= NP_STATE_ALREADY_INITIALIZED	<< 1,
	NP_STATE_FILL					= 0xFFFFFFFF
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	@struct	���ۓI�ȃI�u�W�F�N�g

*/
typedef struct __npOBJECT
{
	npSTATE		m_nType;	/*!< ���g�̎��				*/
	npSTATE		m_nState;	/*!< ��ԃt���O				*/
	npS32		m_nRefs;	/*!< ���t�@�����X�J�E���^	*/
	npU32		m_nUser;	/*!< ���[�U�[�t���O			*/
}
npOBJECT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	@brief	�I�u�W�F�N�g�̎�ނ��Z�b�g����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nType	�Z�b�g����I�u�W�F�N�g�̎��

	@return	�G���[�R�[�h

*/
#define npSetObjectType( pObject, nType )		\
	( ( pObject )->m_nType = ( nType ), NP_SUCCESS )

/*!
	
	@brief	�I�u�W�F�N�g�̎�ނ��擾����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g

	@return	�I�u�W�F�N�g�̎��

*/
#define	npGetObjectType( pObject )	( ( pObject )->m_nType )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ��㏑������

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nState	�㏑������I�u�W�F�N�g�̏��

	@return	�G���[�R�[�h

*/
#define	npSetObjectState( pObject, nState )	(	\
	( pObject )->m_nState = ( nState ), NP_SUCCESS )

/*!
	
	@brief	�I�u�W�F�N�g�̏�Ԃ��擾����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	
	@return	�擾����I�u�W�F�N�g�̏��

*/
#define	npGetObjectState( pObject )	( ( pObject )->m_nState )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ�L���ɂ���

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	�L���ɂ���I�u�W�F�N�g�̏��

	@return	�G���[�R�[�h

*/
#define npEnableObjectState( pObject, nMask )	\
	( ( pObject )->m_nState |= ( nMask ), NP_SUCCESS )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ𖳌��ɂ���

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	�����ɂ���I�u�W�F�N�g�̏��

	@return	�G���[�R�[�h

*/
#define npDisableObjectState( pObject, nMask )	\
	( ( pObject )->m_nState &= ~( nMask ), NP_SUCCESS )

/*!

	@brief	�I�u�W�F�N�g�̏�Ԃ��m�F����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	��Ԃ��m�F����I�u�W�F�N�g�̏��

	@retval	NP_TRUE		�L��	
	@retval	NP_FALSE	����

*/
#define npIsEnabledObjectState( pObject, nMask )\
	((((pObject)->m_nState & (npSTATE)(nMask)) != 0) ? NP_TRUE : NP_FALSE)

/*!

	@brief	���t�@�����X�J�E���^�̒l���Z�b�g����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nNum	�㏑������l

	@return	�G���[�R�[�h

*/
#define	npSetObjectReferences( pObject, nNum )	(	\
	( pObject )->m_nRefs = ( nNum ), NP_SUCCESS )

/*!

	@brief	���t�@�����X�J�E���^�̒l���擾����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	
	@return	���t�@�����X�J�E���^�̒l

*/
#define	npGetObjectReferences( pObject )	(	\
	( pObject )->m_nRefs )

/*!

	@brief	�I�u�W�F�N�g�̃��t�@�����X�J�E���^�𑝉�������

	@param	pObject	����Ώۂ̃I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
#define	npIncrementObjectReferences( pObject )	\
	( ( ( pObject )->m_nRefs )++, NP_SUCCESS )

/*!

	@brief	�I�u�W�F�N�g�̃��t�@�����X�J�E���^������������

	@param	pObject	����Ώۂ̃I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
#define	npDecrementObjectReferences( pObject )	\
	( ( ( pObject )->m_nRefs )--, NP_SUCCESS )

/*!

	@brief	�I�u�W�F�N�g�̃��t�@�����X�J�E���^���m�F����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g

	@return	���t�@�����X�J�E���^

*/
#define npCountObjectReferences( pObject )	(	\
	( pObject )->m_nRefs )

/*!

	@brief	���[�U�[�t���O�̏�Ԃ�L���ɂ���

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	�L���ɂ���I�u�W�F�N�g�̏��

	@return	�G���[�R�[�h

*/
#define npEnableUserState( pObject, nMask )		\
	( ( pObject )->m_nUser |= ( nMask ), NP_SUCCESS )

/*!

	@brief	���[�U�[�t���O�̏�Ԃ𖳌��ɂ���

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	�����ɂ���I�u�W�F�N�g�̏��

	@return	�G���[�R�[�h

*/
#define npDisableUserState( pObject, nMask )	\
	( ( pObject )->m_nUser &= ~( nMask ), NP_SUCCESS )

/*!

	@brief	���[�U�[�t���O�̏�Ԃ��m�F����

	@param	pObject	����Ώۂ̃I�u�W�F�N�g
	@param	nMask	��Ԃ��m�F����I�u�W�F�N�g�̏��

	@retval	NP_TRUE		�L��	
	@retval	NP_FALSE	����

*/
#define npIsEnabledUserState( pObject, nMask )	\
	( ( ( pObject )->m_nUser & ( nMask ) ) == ( nMask ) ? NP_TRUE : NP_FALSE )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPOBJECT_H */
