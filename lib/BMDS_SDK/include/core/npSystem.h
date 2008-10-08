/*=============================================================================
/*!

	@file	npSystem.h

	@brief	���C�u�����V�X�e����`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.8 $
	$Date: 2006/03/22 05:10:39 $

*//*=========================================================================*/
#ifndef	NPSYSTEM_H
#define	NPSYSTEM_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npObject.h"

//!@}

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
struct __npCONTEXT;

/*!

	@struct	�V�X�e���p����������

*/
typedef struct __npSYSTEMDESC
{
	/*! �쐬���郌���_�����O�R���e�L�X�g�̍ő吔 */
	npSIZE	MaxContext;
}
npSYSTEMDESC;

/*!

	@struct	�V�X�e���I�u�W�F�N�g

	@note	���C�u�������g�p���Ă���ԁA�ێ����Ȃ���΂Ȃ�Ȃ�.<br>
			�V���O���g���I�u�W�F�N�g

*/
typedef NP_HSTRUCT __npSYSTEM
{
	npOBJECT				Object;

	void*					m_pInstance;
	/* �����_�����O�R���e�L�X�g�̃o�b�t�@ */
	struct	__npCONTEXT*	m_pBuf;
	npSIZE					m_nNum;

	npU32					m_pReserved[ 1 ];
}
NP_FSTRUCT npSYSTEM;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------
extern npSYSTEM	npINSTANCE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�V�X�e���ɕK�v�ȃ������T�C�Y���擾����

	@param	pDesc	npSYSTEMDESC �\����
	
	@return	�V�X�e���ɕK�v�ȃ������T�C�Y

*/
#define	npCheckSystemHeapSize( pDesc )	\
	( npSIZE )( sizeof( npCONTEXT ) * ( pDesc )->MaxContext )

/*!

	�V�X�e���̏���������

	@param	pInstance	�O���C���X�^���X
	@param	pDesc		�V�X�e���̏���������
	@param	pBuf		�V�X�e�����g�p����o�b�t�@��n��
	@param	nSize		�o�b�t�@�̃T�C�Y�i Byte �P�� �j
	@param	ppSystem	���������V�X�e���I�u�W�F�N�g���󂯎��ϐ�

	@return	�G���[�R�[�h

	@note	���C�u�������g�p����O�ɕK���Ăяo��

*/
NP_API npERROR NP_APIENTRY npInitSystem(
				npVOID*			pInstance,
	NP_CONST	npSYSTEMDESC*	pDesc,
				npVOID*			pBuf,
				npSIZE			nSize,
				npSYSTEM** 		ppSystem 
);

/*!

	�V�X�e���̏I������

	@param	pSystem	����Ώۂ̃V�X�e���I�u�W�F�N�g

	@note	���C�u�������I�����鎞�͕K���Ăяo��

*/
NP_API npVOID NP_APIENTRY npExitSystem( npSYSTEM* pSystem );

/*!

	�V�X�e���̃C���X�^���X���Q�Ƃ���

	@return	�V�X�e���I�u�W�F�N�g
			
*/
#define npRefSystem()	( &npINSTANCE )

/*!

	�V�X�e�����������_���v����

*/
#ifdef NP_DEBUG
NP_API npVOID NP_APIENTRY npDumpSystemMemory( npVOID );
#else
#	define npDumpSystemMemory( npVOID )
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


#endif	/* NPSYSTEM_H */
