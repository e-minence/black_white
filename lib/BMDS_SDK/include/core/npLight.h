/*=============================================================================
/*!

	@file	npLight.h

	@brief	���C�g��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: $
	$Date: $

*//*=========================================================================*/
#ifndef	NPLIGHT_H
#define	NPLIGHT_H

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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
struct __npCONTEXT;
	
/*!

	@brief	���C�g�I�u�W�F�N�g

*/
typedef struct __npLIGHT
{
	npOBJECT	Object;

	npFVECTOR	m_vecPos;
	npFVECTOR	m_vecDir;

	npCOLOR		m_nDiffuse;
	npCOLOR		m_nSpecular;
	npCOLOR		m_nAmbient;

	npFLOAT		m_fRange;
	npFLOAT		m_fFalloff;
	npFLOAT		m_fAttenuation0;
	npFLOAT		m_fAttenuation1;
	npFLOAT		m_fAttenuation2;
	npFLOAT		m_fTheta;
	npFLOAT		m_fPhi;
}
npLIGHT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�A���r�G���g���C�g���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nColor		�J���[�l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetAmbientLight( 
	struct	__npCONTEXT*	pContext, 
	npCOLOR					nColor
);

/*!

	�A���r�G���g���C�g�J���[�l���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�J���[�l

*/
NP_API npCOLOR NP_APIENTRY npGetAmbientLight( struct __npCONTEXT* pContext );

/*!

	���C�g�I�u�W�F�N�g�̃p�����[�^���f�o�C�X�Ɋ��蓖�Ă�

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pLight		���C�g�I�u�W�F�N�g
	@param	nIndex		���C�g�̃C���f�b�N�X

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npRegistLight( 
	struct __npCONTEXT*	pContext,
	npLIGHT*			pLight,
	npU32				nIndex
);

/*!

	���C�g��L���ɂ���

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nIndex		�L���ɂ��郉�C�g�̃C���f�b�N�X

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npEnableLight( 
	struct __npCONTEXT*	pContext,
	npU32				nIndex
);

/*!

	���C�g�𖳌��ɂ���

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nIndex		�����ɂ��郉�C�g�̃C���f�b�N�X

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npDisableLight( 
	struct __npCONTEXT*	pContext,
	npU32				nIndex
);

/*!

	���C�g�̏�Ԃ��擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nIndex		���C�g�I�u�W�F�N�g�̃C���f�b�N�X

	@return	��ԃt���O

*/
NP_API npBOOL NP_APIENTRY npIsEnabledLight( 
	struct __npCONTEXT*	pContext,
	npU32				nIndex
);

/*!

	���C�g�̎�ނ��Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	nType	���C�g�̎��

	@return	�G���[�R�[�h

*/
#define	npSetLightType( pLight, nType )	(	\
	npSetObjectType( &pLight->Object, nType ), NP_SUCCESS )

/*!

	���C�g�̎�ނ��擾����

	@param	pLight	���C�g�I�u�W�F�N�g
	
	@return	���C�g�̎��

*/
#define npGetLightType( pLight )	npGetObjectType( &pLight->Object )

/*!

	���C�g�̊g�U�����Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	nColor	�J���[�l	
	
	@return	�G���[�R�[�h

*/
#define	npSetLightDiffuse( pLight, nColor )	(									\
	( pLight )->m_paramLight.Diffuse.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Diffuse.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Diffuse.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Diffuse.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	���C�g�̊g�U�����擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�g�U���l

*/
#define npGetLightDiffuse( pLight )	(	NP_RGBA(			\
	( npU8 )( ( pLight )->m_paramLight.Diffuse.r * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Diffuse.g * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Diffuse.b * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Diffuse.a * 255.0f ) ) )

/*!

	���C�g�̃X�y�L���������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define npSetLightSpecular( pLight, nColor )	(									\
	( pLight )->m_paramLight.Specular.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,	\
	( pLight )->m_paramLight.Specular.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,	\
	( pLight )->m_paramLight.Specular.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,	\
	( pLight )->m_paramLight.Specular.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	���C�g�̃X�y�L���������擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�X�y�L�������l

*/
#define	npGetLightSpecular( pLight )	(	NP_RGBA(			\
	( npU8 )( ( pLight )->m_paramLight.Specular.r * 255.0f ),	\
	( npU8 )( ( pLight )->m_paramLight.Specular.g * 255.0f ),	\
	( npU8 )( ( pLight )->m_paramLight.Specular.b * 255.0f ),	\
	( npU8 )( ( pLight )->m_paramLight.Specular.a * 255.0f ) ) )

/*!

	���C�g�̊������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	nColor	�J���[�l

	@return	�G���[�R�[�h

*/
#define npSetLightAmbient( pLight, nColor )	(									\
	( pLight )->m_paramLight.Ambient.r = ( npFLOAT )NP_R( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Ambient.g = ( npFLOAT )NP_G( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Ambient.b = ( npFLOAT )NP_B( ( nColor ) ) / 255.0f,\
	( pLight )->m_paramLight.Ambient.a = ( npFLOAT )NP_A( ( nColor ) ) / 255.0f, NP_SUCCESS )

/*!

	���C�g�̊������擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�����l

*/
#define	npGetLightAmbient( pLight )	(	NP_RGBA(			\
	( npU8 )( ( pLight )->m_paramLight.Ambient.r * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Ambient.g * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Ambient.b * 255.0f ),\
	( npU8 )( ( pLight )->m_paramLight.Ambient.a * 255.0f ) ) )

/*!

	���C�g�̈ʒu���Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	pPos	���C�g�̃|�W�V����

	@return	�G���[�R�[�h

*/
#define	npSetLightPos( pLight, pPos )	(					\
	( pLight )->m_paramLight.Position.x = NP_X( ( pPos ) ),	\
	( pLight )->m_paramLight.Position.y = NP_Y( ( pPos ) ),	\
	( pLight )->m_paramLight.Position.z = NP_Z( ( pPos ) ), NP_SUCCESS )

/*!

	���C�g�̈ʒu���擾����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	pPos	���C�g�̃|�W�V����

	@return	�G���[�R�[�h

*/
#define	npGetLightPos( pLight, pPos )	(						\
	( *( pPos ) )[ 0 ] = ( pLight )->m_paramLight.Position.x,	\
	( *( pPos ) )[ 1 ] = ( pLight )->m_paramLight.Position.y,	\
	( *( pPos ) )[ 2 ] = ( pLight )->m_paramLight.Position.z, NP_SUCCESS )

/*!

	���C�g�̕������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	pDir	���C�g�̕���

	@return	�G���[�R�[�h

*/
#define	npSetLightDir( pLight, pDir )	(						\
	( pLight )->m_paramLight.Direction.x = NP_X( ( pDir ) ),	\
	( pLight )->m_paramLight.Direction.y = NP_Y( ( pDir ) ),	\
	( pLight )->m_paramLight.Direction.z = NP_Z( ( pDir ) ), NP_SUCCESS )

/*!

	���C�g�̕������擾����

	@param	pLight	���C�g�I�u�W�F�N�g�m�[�h
	@param	pDir	���C�g�̕���

	@return	�G���[�R�[�h

*/
#define	npGetLightDir( pLight, pDir )	(						\
	( *( pDir ) )[ 0 ] = ( pLight )->m_paramLight.Direction.x,	\
	( *( pDir ) )[ 1 ] = ( pLight )->m_paramLight.Direction.y,	\
	( *( pDir ) )[ 2 ] = ( pLight )->m_paramLight.Direction.z, NP_SUCCESS )	

/*!

	���C�g�̉e���͈͂��Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fRadius	�e���͈�

	@return	�G���[�R�[�h
	
*/
#define	npSetLightRadius( pLight, fRadius )	\
	( ( pLight )->m_paramLight.Range = ( fRadius ), NP_SUCCESS )

/*!

	���C�g�̉e���͈͂��擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�e���͈͂̔��a

*/
#define	npGetLightRadius( pLight )	( ( pLight )->m_paramLight.Range )

/*!

	���C�g�I�u�W�F�N�g�̈�茸�������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fRatio	������

	@return	�G���[�R�[�h

*/
#define npSetLightConstAttenuation( pLight, fRatio )	\
	( ( pLight )->m_paramLight.Attenuation0 = ( fRatio ), NP_SUCCESS )

/*!

	���C�g�I�u�W�F�N�g�̈�茸�������擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	��茸����

*/
#define npGetLightConstAttenuation( pLight )	\
	( ( pLight )->m_paramLight.Attenuation0 )

/*!

	���C�g�I�u�W�F�N�g�̐��`���������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fRatio	������

	@return	�G���[�R�[�h

*/
#define npSetLightLinearAttenuation( pLight, fRatio )	\
	( ( pLight )->m_paramLight.Attenuation1 = ( fRatio ), NP_SUCCESS )

/*!

	���C�g�I�u�W�F�N�g�m�[�h�̐��`���������擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	���`������

*/
#define	npGetLightLinearAttenuation( pLight )	\
	( ( pLight )->m_paramLight.Attenuation1 )

/*!

	���C�g�I�u�W�F�N�g�̓񎟌��������Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fRatio	������

	@return	�G���[�R�[�h

*/
#define	npSetLightQuadAttenuation( pLight, fRatio )		\
	( ( pLight )->m_paramLight.Attenuation2 = ( fRatio ), NP_SUCCESS )

/*!

	���C�g�I�u�W�F�N�g�m�[�h�̓񎟌��������擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�񎟌�����

*/
#define	npGetLightQuadAttenuation( pLight )		\
	( ( pLight )->m_paramLight.Attenuation2 )

/*!

	�X�|�b�g���C�g�̓����R�[���̊p�x���Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fAngle	�X�|�b�g���C�g�̓����R�[���̊p�x

	@return	�G���[�R�[�h

*/
#define	npSetLightConeAngle( pLight, fAngle )	\
	( ( pLight )->m_paramLight.Theta = ( fAngle ), NP_SUCCESS )

/*!

	�X�|�b�g���C�g�̓����R�[���̊p�x���擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�X�|�b�g���C�g�̓����R�[���̊p�x

*/
#define npGetLightConeAngle( pLight )	( ( pLight )->m_paramLight.Theta )

/*!

	�X�|�b�g���C�g�̊O���R�[���̊p�x���Z�b�g����

	@param	pLight	���C�g�I�u�W�F�N�g
	@param	fAngle	�X�|�b�g���C�g�̓����R�[���̊p�x

	@return	�G���[�R�[�h

*/
#define npSetLightSpreadAngle( pLight, fAngle )		\
	( ( pLight )->m_paramLight.Phi = ( fAngle ), NP_SUCCESS )

/*!

	�X�|�b�g���C�g�̊O���R�[���̊p�x���擾����

	@param	pLight	���C�g�I�u�W�F�N�g

	@return	�X�|�b�g���C�g�̊O���R�[���̊p�x

*/
#define	npGetLightSpreadAngle( pLight )	( ( pLight )->m_paramLight.Phi )

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPLIGHT_H */
