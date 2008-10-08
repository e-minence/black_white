/*=============================================================================
/*!

	@file	npShader.h

	@brief	�V�F�[�_��`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: $
	$Date: $

*//*=========================================================================*/
#ifndef	NPSHADER_H
#define	NPSHADER_H

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
#define	NP_HLSL
//#define	NP_CGFX

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------
/*!

	@enum	�V�F�[�_�̃^�C�v

*/
enum npSHADERTYPE
{
	NP_SHADER_FX	= 0,
	NP_SHADER_VERTEX,
	NP_SHADER_PIXEL,

	NP_SHADER_MAX
};

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
struct __npCONTEXT;
struct __npTEXTURE;

/*!

	@struct	�V�F�[�_�I�u�W�F�N�g

*/
typedef struct __declspec( align( 16 ) ) __npSHADER
{
	npOBJECT			Object;					/* ���ۃI�u�W�F�N�g */
	npCHAR				m_szName[ NP_MAXPATH ];	/* �V�F�[�_��	*/

	npU32				m_pReserved[ 1 ];
	struct __npSHADER*	m_pNext;
}
npSHADER;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�R���p�C���ς݂̃V�F�[�_���\�[�X����V�F�[�_�I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pBuf		�t�@�C���o�b�t�@
	@param	nSize		�t�@�C���T�C�Y
	@param	ppShader	���������V�F�[�_�I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npCreateShaderFromBin(
	struct	__npCONTEXT*	pContext,
	npVOID*					pBuf,
	npSIZE					nSize,
	npSHADER**				ppShader
);

/*!

	(.fx) �t�@�C������V�F�[�_�I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	szFileName	�t�@�C����
	@param	ppShader	���������V�F�[�_�I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npCreateShaderFromFile(
	struct __npCONTEXT*		pContext,
	npCHAR*					szFileName,
	npSHADER**				ppShader
);

/*!
	@todo	������
			D3DXAssembleShaderFromFile(...)
			IDirect3DDevice9::CreateVertexShader(...)
			SetVertexShaderConstantF(...)
			SetVertexShader(...)
*/
NP_API npERROR NP_APIENTRY npCreateVertexShaderFromFile(
	struct __npCONTEXT*		pContext,
	npCHAR*					szFileName,
	npSHADER**				ppShader
);

/*!
	@todo	������
*/
NP_API npERROR NP_APIENTRY npCreateVertexShaderFromBin(
	struct __npCONTEXT*		pContext,
	npVOID*					pBuf,
	npSIZE					nSize,
	npSHADER**				ppShader
);

/*!
	@todo	������
*/
NP_API npERROR NP_APIENTRY npCreatePixelShaderFromFile(
	struct __npCONTEXT*		pContext,
	npCHAR*					szFileName,
	npSHADER**				ppShader
);

/*!
	@todo	������
*/
NP_API npERROR NP_APIENTRY npCreatePixelShaderFromBin(
	struct __npCONTEXT*		pContext,
	npVOID*					pBuf,
	npSIZE					nSize,
	npSHADER**				ppShader
);

/*!

	�V�X�e���ˑ��̃V�F�[�_�C���X�^���X����I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pInstance	�V�X�e���ˑ��̃V�F�[�_�C���X�^���X
	@param	ppShader	���������V�F�[�_�I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npCreateShaderFromRef(
	struct __npCONTEXT*		pContext,
	npVOID*					pInstance,
	npSHADER**				ppShader
);

/*!

	�V�F�[�_�I�u�W�F�N�g�̃N���[���𐶐�����

	@param	pShader		��������V�F�[�_�I�u�W�F�N�g

	@return	�V�F�[�_�I�u�W�F�N�g

	@note	���t�@�����X�J�E���^�𑝉����܂�
			VRAM �ߖ�̂��ߓ����`�󂪂���ꍇ��
			������̊֐��ŕ���������Ă��������B

*/
#define	npCloneShader( pShader )	(	\
	npIncrementObjectReferences( &( pShader )->Object ), ( pShader ) )

/*!

	�V�F�[�_�I�u�W�F�N�g���������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�Ώۂ̃V�F�[�_�I�u�W�F�N�g

	@note	���t�@�����X�J�E���^�� 0 �Ȃ� VRAM ����J��.<br>
			0 �ɂȂ�܂ŊJ������Ȃ�

*/
NP_API npVOID NP_APIENTRY npReleaseShader(
	struct __npCONTEXT*	pContext,
	npSHADER*			pShader
);

/*!

	���t�@�����X�J�E���^�𖳎����ăe�N�X�`�����������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npDestroyShader(
	struct __npCONTEXT*	pContext,
	npSHADER*			pShader
);

/*!

	�V�F�[�_�����Z�b�g����

	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	szName		�o�^����V�F�[�_��

	@return	�G���[�R�[�h

*/
#define	npSetShaderName( pShader, szName )	(	\
	( strlen( ( szName ) ) < NP_MAXPATH ) ? 	\
	( strcpy( ( pShader )->m_szName, ( szName ) ), NP_SUCCESS ) : NP_ERR_OVERFLOW )
 
/*!

	�V�F�[�_�����擾����

	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	pName		�擾����V�F�[�_���p�̃|�C���^

	@return	�G���[�R�[�h
	
*/
#define	npGetShaderName( pShader, pName )	(	\
	strcpy( ( pName ), ( pShader )->m_szName ), NP_SUCCESS )

/*!

	�e�N�j�b�N�̃n���h�����擾����

	@param	pShader	�V�F�[�_�I�u�W�F�N�g
	@param	szName	�e�N�j�b�N��

	@return	�e�N�j�b�N�̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npTechniqueHANDLE( 
				npSHADER*	pShader, 
	NP_CONST	npCHAR*		szName 
);

/*!

	�擪�̃e�N�j�b�N�̃n���h�����擾����

	@param	pShader	�V�F�[�_�I�u�W�F�N�g
	
	@return	�e�N�j�b�N�̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npFirstTechniqueHANDLE( 
	npSHADER*	pShader 
);

/*!

	�w�肵���e�N�j�b�N�̎��̃n���h�����擾����

	@param	pShader	�V�F�[�_�I�u�W�F�N�g

	@return	�e�N�j�b�N�̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npNextTechniqueHANDLE( 
				npSHADER*	pShader, 
	NP_CONST	npHANDLE	hPrev 
);

/*!

	�e�N�j�b�N���f�o�C�X�ɃZ�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hTechnique	�e�N�j�b�N�̃n���h��

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npRegistTechnique(
	struct	__npCONTEXT*	pContext,
	npSHADER*				pShader,
	npHANDLE				hTechnique
);

/*!

	�e�N�j�b�N�𖳌��ɂ���

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g

	@return	�G���[�R�[�h

	@note	���e�͋�

*/
#define	npUnregistTechnique( pContext, pShader )	( NP_SUCCESS )

/*!

	�V�F�[�_�̃e�N�j�b�N���J�n����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hTechnique	�e�N�j�b�N�̃n���h��
	@param	pPass		�p�X�̐�(�c�����Ă�ꍇ��NP_NULL�ŉ�)

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npBeginTechnique(
	struct __npCONTEXT*	pContext,
	npSHADER*			pShader,
	npU32*				pPass
);

/*!

	�V�F�[�_�̃e�N�j�b�N���I������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npVOID NP_APIENTRY npEndTechnique( 
	struct __npCONTEXT*	pContext,
	npSHADER*			pShader
);

/*!

	�V�F�[�_�p�X���J�n����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	nPass		�p�X�̃C���f�b�N�X

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npBeginPass(
	struct __npCONTEXT*	pContext, 
	npSHADER*			pShader,
	npU32				nPass
);

/*!

	�V�F�[�_�p�X���I������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npEndPass( 
	struct __npCONTEXT*	pContext, 
	npSHADER*			pShader 
);

/*!

	�V�F�[�_���̃O���[�o���ϐ��̃n���h�����擾����

	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	szName		�ϐ���

	@return	�擾����ϐ��ւ̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npConstantHANDLE(
				npSHADER*	pShader,
	NP_CONST	npCHAR*		szName
);

/*!

	�V�F�[�_���̃Z�}���e�B�N�X�̃n���h�����擾����

	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	szName		�Z�}���e�B�N�X��

	@return	�擾����Z�}���e�B�N�X�̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npSemanticsHANDLE(
				npSHADER*	pShader,
	NP_CONST	npCHAR*		szName
);

/*!

	�V�F�[�_���̃A�m�e�[�V�����̃n���h�����擾����

	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	szName		�A�m�e�[�V������

	@return	�擾����A�m�e�[�V�����̃n���h��

*/
NP_API npHANDLE NP_APIENTRY npAnnotationHANDLE(
				npSHADER*	pShader,
	NP_CONST	npCHAR*		szName
);

/*!

	�V�F�[�_�p�̃��W�X�^�փe�N�X�`�����Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pTexture	�Ώۂ̃e�N�X�`���I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetConstantTEXTURE(
	struct __npCONTEXT*	pContext,
	npSHADER*			pShader,
	npHANDLE			hParam,
	struct __npTEXTURE*	pTexture
);

/*!

	�V�F�[�_�p�̃��W�X�^����e�N�X�`�����擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	ppTexture	�Ώۂ̃e�N�X�`���I�u�W�F�N�g

	@note	�����_�����O�R���e�L�X�g������ϐ��̃n���h����
			��v����e�N�X�`�����������鏈��������܂��B

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npGetConstantTEXTURE(
	struct __npCONTEXT*		pContext,
	npSHADER*				pShader,
	npHANDLE				hParam,
	struct __npTEXTURE**	ppTexture
);

/*!

	�V�F�[�_�p�̃��W�X�^�փ}�g���N�X���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetConstantFMATRIX( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFMATRIX*				pValue
);

/*!

	�V�F�[�_�p�̃��W�X�^����}�g���N�X���擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npGetConstantFMATRIX( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFMATRIX*				pValue
);

/*!

	�V�F�[�_�p�̃��W�X�^�� FVEC3 ���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetConstantFVEC3( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFVECTOR*				pValue 
);

/*!

	�V�F�[�_�p�̃��W�X�^���� FVEC4 ���擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npGetConstantFVEC3( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFVECTOR*				pValue 
);

/*!

	�V�F�[�_�p�̃��W�X�^�� FVEC4 ���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetConstantFVEC4( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFVECTOR*				pValue 
);

/*!

	�V�F�[�_�p�̃��W�X�^���� FVEC4 ���擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npGetConstantFVEC4( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFVECTOR*				pValue 
);

/*!

	�V�F�[�_�p�̃��W�X�^�� FRGBA ���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSetConstantFRGBA( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFRGBA*				pValue 
);

/*!

	�V�F�[�_�p�̃��W�X�^���� FRGBA ���擾����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pShader		�V�F�[�_�I�u�W�F�N�g
	@param	hParam		���W�X�^�ϐ��ւ̃n���h��
	@param	pValue		�Ώۂ̒l

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npGetConstantFRGBA( 
	struct __npCONTEXT*		pContext, 
	npSHADER*				pShader, 
	npHANDLE				hParam, 
	npFRGBA*				pValue 
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPSHADER_H */
