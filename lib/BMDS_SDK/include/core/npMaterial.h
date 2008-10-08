/*=============================================================================
/*!

	@file	npMaterial.h

	@brief	�}�e���A����`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: $
	$Date: $

*//*=========================================================================*/
#ifndef	NPMATERIAL_H
#define	NPMATERIAL_H

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
struct __npCONTEXT;

/*!

	@param	�}�e���A���I�u�W�F�N�g

*/
typedef struct __npMATERIAL
{
	npOBJECT	Object;

	npCOLOR		m_nDiffuse;
	npCOLOR		m_nAmbient;
	npCOLOR		m_nSpecular;
	npCOLOR		m_nEmissive;
	npFLOAT		m_fPower;
}
npMATERIAL;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�}�e���A���I�u�W�F�N�g�̃p�����[�^���f�o�C�X�Ɋ��蓖�Ă�

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pMater		�}�e���A���I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npRegistMaterial( 
	struct	__npCONTEXT*	pContext, 
	npMATERIAL*				pMater
);

/*!

	�}�e���A���Ɋg�U�����Z�b�g����

	@param	pMater	�}�e���A���I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define	npSetMaterialDiffuse( pMater, nColor )	(										\
	( pMatter )->m_paramMaterial.Diffuse.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Diffuse.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Diffuse.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Diffuse.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	�}�e���A���̊g�U�����擾����

	@param	pMater	�}�e���A���I�u�W�F�N�g

	@return	�g�U���˒l

*/
#define	npGetMaterialDiffuse( pMater )	( NP_RGBA(				\
	( npU8 )( ( pMater )->m_paramMaterial.Diffuse.r * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Diffuse.g * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Diffuse.b * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Diffuse.a * 255.0f ) ) )

/*!

	�}�e���A���Ɋ������Z�b�g����

	@param	pMater	�}�e���A���I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define	npSetMaterialAmbient( pMater, nColor )	(										\
	( pMatter )->m_paramMaterial.Ambient.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Ambient.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Ambient.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Ambient.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	�}�e���A���̊������擾����

	@param	pMater	�}�e���A���I�u�W�F�N�g

	@return	�g�U���˒l

*/
#define	npGetMaterialAmbient( pMater )	( NP_RGBA(				\
	( npU8 )( ( pMater )->m_paramMaterial.Ambient.r * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Ambient.g * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Ambient.b * 255.0f ),	\
	( npU8 )( ( pMater )->m_paramMaterial.Ambient.a * 255.0f ) ) )

/*!

	�}�e���A���ɃX�y�L�����l���Z�b�g����

	@param	pMater	�}�e���A���I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define	npSetMaterialSpecular( pMater, nColor )	(										\
	( pMatter )->m_paramMaterial.Specular.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Specular.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Specular.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Specular.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	�}�e���A���̃X�y�L�����l���擾����

	@param	pMater	�}�e���A���I�u�W�F�N�g

	@return	�g�U���˒l

*/
#define	npGetMaterialSpecular( pMater )	( NP_RGBA(				\
	( npU8 )( ( pMater )->m_paramMaterial.Specular.r * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Specular.g * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Specular.b * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Specular.a * 255.0f ) ) )

/*!

	�}�e���A���ɕ��ˌ����Z�b�g����

	@param	pMater	�}�e���A���I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define	npSetMaterialEmissive( pMater, nColor )	(										\
	( pMatter )->m_paramMaterial.Emissive.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Emissive.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Emissive.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,	\
	( pMatter )->m_paramMaterial.Emissive.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	�}�e���A���̕��ˌ����擾����

	@param	pMater	�}�e���A���I�u�W�F�N�g

	@return	�g�U���˒l

*/
#define	npGetMaterialEmissive( pMater )	( NP_RGBA(				\
	( npU8 )( ( pMater )->m_paramMaterial.Emissive.r * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Emissive.g * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Emissive.b * 255.0f ),\
	( npU8 )( ( pMater )->m_paramMaterial.Emissive.a * 255.0f ) ) )

/*!

	�}�e���A���ɋP�x���Z�b�g����

	@param	pMater	�}�e���A���I�u�W�F�N�g
	@param	fPower	�P�x�̋���

	@return	�G���[�R�[�h

*/
#define	npSetMaterialShininess( pMater, fPower )	\
	( ( pMatter )->m_paramMaterial.Power = ( fPower ), NP_SUCCESS )

/*!

	�}�e���A���̋P�x���擾����

	@param	pMater	�}�e���A���I�u�W�F�N�g

	@return	�P�x

*/
#define	npGetMaterialShininess( pMater )	( ( pMatter )->m_paramMaterial.Power )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPMATERIAL_H */
