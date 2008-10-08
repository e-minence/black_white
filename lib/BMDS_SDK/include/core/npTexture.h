/*=============================================================================
/*!

	@file	npTexture.h

	@brief	�e�N�X�`����`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.8 $
	$Date: 2006/03/28 03:56:49 $

*//*=========================================================================*/
#ifndef	NPTEXTURE_H
#define	NPTEXTURE_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "npObject.h"
#include <nitro/gx/g3imm.h>
#include <nitro/gx/g3b.h>
#include <nitro/gx/g3c.h>

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

	@enum	�s�N�Z���t�H�[�}�b�g

*/
enum npPIXELFORMAT
{
	//NP_PIXEL_INDEX12	= NVD_TEXPIXEL_TF_I4,
	//NP_PIXEL_INDEX16	= NVD_TEXPIXEL_TF_IA4,
	//NP_PIXEL_INDEX24	= NVD_TEXPIXEL_TF_I8,
	//NP_PIXEL_INDEX32	= NVD_TEXPIXEL_TF_IA8,

	//NP_PIXEL_R5G6B5	= NVD_TEXPIXEL_TF_RGB565,
	//NP_PIXEL_R5G5B5A3	= NVD_TEXPIXEL_TF_RGB5A3,
	NP_PIXEL_R8G8B8A8	= 0//GL_RGBA,	//	= D3DFMT_A8R8G8B8,

	//NP_PIXEL_TLUT4	= NVD_TEXPIXEL_TF_C4,
	//NP_PIXEL_TLUT8	= NVD_TEXPIXEL_TF_C8

	// CLUT : Color Look Up Table
};

#if 0
/*!

	@enum	�e�N�X�`���X�e�[�W�X�e�[�g

*/
enum npTEXTURESTATE
{

};

/*!

	@enum	�e�N�X�`���t�B���^

*/
enum npTEXTUREFILTER
{

};
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
struct __npCONTEXT;

/*!

	@struct	�e�N�X�`���I�u�W�F�N�g

*/
typedef	struct __npTEXTURE
{
	npOBJECT	Object;					/* ���ۃI�u�W�F�N�g */

	npCHAR		m_szName[ NP_MAXPATH ];	/* �e�N�X�`���� */
	npU32		m_nWidth;				///< Width
	npU32		m_nHeight;				///< Height
	npU32		m_nSize;				///< Size
	npU32		m_TexAddr;				///< TexAddr
	npU32		m_PltAddr;				///< PltAddr
	GXTexFmt	m_TexFmt;				///< TextureFormat
	GXTexSizeS	m_TexSizeS;				///< GX_TEXSIZE_S***
	GXTexSizeT	m_TexSizeT;				///< GX_TEXSIZE_T***

	struct __npTEXTURE*		m_pNext;
}
npTEXTURE;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------
/*!

	�e�N�X�`���I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nWidth		����
	@param	nHeight		�c��
	@param	nLevels		�~�b�v�}�b�v���x��
	@param	nUsage		
	@param	nFormat		�s�N�Z���t�H�[�}�b�g�̎��
	@param	nPool		
	@param	ppTexture	���������e�N�X�`���I�u�W�F�N�g���󂯎��ϐ�

	@return	�G���[�R�[�h

	@note	�O���C���X�^���X���L���ȂƂ����̃p�����[�^�͖���

*/
NP_API npERROR NP_APIENTRY npCreateTexture(
	struct __npCONTEXT*	pContext,
	npU32 				nWidth,
	npU32				nHeight,
	npU32				nLevels,
	npSTATE				nUsage,
	npSTATE				nFormat,
	npSTATE				nPool,
	npTEXTURE**			ppTexture
);

/*!

	���\�[�X����e�N�X�`���I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pBuf		GVR �t�@�C���o�b�t�@
	@param	nSize		GVR �t�@�C���T�C�Y
	@param	ppTexture	���������e�N�X�`���I�u�W�F�N�g

	@return	�G���[�R�[�h

	@todo	������ 0x20 �I�t�Z�b�g�ړ�����̂ŃR�s�[���<br>
			�o�b�t�@�ɂ̓f�[�^�̐擪��n��

*/
NP_API npERROR NP_APIENTRY npCreateTextureFromBin( 
	struct	__npCONTEXT*	pContext, 
	npVOID*					pBuf,
	npSIZE					nSize, 
	npTEXTURE**				ppTexture 
);

/*!

	�t�@�C������e�N�X�`���I�u�W�F�N�g�𐶐�����

	@param	pContex		�����_�����O�R���e�L�X�g
	@param	szFileName	�t�@�C����
	@param	ppTexture	���������e�N�X�`���I�u�W�F�N�g

	@return	�G���[�R�[�h

	@note	������ szFileName �� npRegistTextureName() ���R�[�������

*/
NP_API npERROR NP_APIENTRY npCreateTextureFromFile(
	struct	__npCONTEXT*	pContext,
	npCHAR*					szFileName,
	npTEXTURE**				ppTexture
);

/*!

	�V�X�e���ˑ��̃e�N�X�`���C���X�^���X����I�u�W�F�N�g�𐶐�����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pInstance	�V�X�e���ˑ��̃e�N�X�`���C���X�^���X
	@param	ppTexture	���������e�N�X�`���I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npCreateTextureFromRef(
	struct	__npCONTEXT*	pContext,
	npVOID*					pInstance,
	npTEXTURE**				ppTexture
);

/*!

	�e�N�X�`���I�u�W�F�N�g�̃N���[���𐶐�����

	@param	pTexture	��������e�N�X�`���I�u�W�F�N�g

	@return	�e�N�X�`���I�u�W�F�N�g

	@note	���t�@�����X�J�E���^�𑝉����܂�
			VRAM �ߖ�̂��ߓ����`�󂪂���ꍇ��
			������̊֐��ŕ���������Ă��������B

*/
#define	npCloneTexture( pTexture )	(	\
	npIncrementObjectReferences( &( pTexture )->Object ), ( pTexture ) )

/*!

	�e�N�X�`���I�u�W�F�N�g���J������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pTexture	�e�N�X�`���I�u�W�F�N�g

	@note	���t�@�����X�J�E���^�� 0 �Ȃ� VRAM ����J��.<br>
			0 �ɂȂ�܂ŊJ������Ȃ�

*/
NP_API npVOID NP_APIENTRY npReleaseTexture( 
	struct __npCONTEXT*	pContext,
	npTEXTURE*			pTexture
);

/*!

	���t�@�����X�J�E���^�𖳎����ăe�N�X�`�����J������

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	pTexture	�e�N�X�`���I�u�W�F�N�g

*/
NP_API npVOID NP_APIENTRY npDestroyTexture(
	struct __npCONTEXT*	pContext,
	npTEXTURE*			pTexture
);

/*!

	�e�N�X�`�������Z�b�g����

	@param	pTexture	�e�N�X�`���I�u�W�F�N�g
	@param	szName		�o�^����e�N�X�`����

	@return	�G���[�R�[�h

*/
#define	npSetTextureName( pTexture, szName )	(		\
	( strlen( ( char* )( szName ) ) < NP_MAXPATH ) ? 	\
	( strcpy( ( char* )( ( pTexture )->m_szName ), ( char* )( szName ) ), NP_SUCCESS ) : NP_ERR_OVERFLOW )
 
/*!

	�e�N�X�`�������擾����

	@param	pTexture	�e�N�X�`���I�u�W�F�N�g
	@param	pName		�擾����e�N�X�`�����p�̃|�C���^

	@return	�G���[�R�[�h
	
*/
#define	npGetTextureName( pTexture, pName )	(	\
	strcpy( ( pName ), ( pTexture )->m_szName ), NP_SUCCESS )

/*!

	�J�����g�ɂ���e�N�X�`���������_�����O�R���e�L�X�g�֓o�^����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nStage		�e�N�X�`���X�e�[�W
	@param	pTexture	�Ώۂ̃e�N�X�`���I�u�W�F�N�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npRegistTexture(
	struct	__npCONTEXT*	pContext,
	npU32					nStage,
	npTEXTURE*				pTexture
);

/*!

	�����_�����O�R���e�L�X�g�ɓo�^����Ă���e�N�X�`���𖳌��ɂ���

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nStage		�e�N�X�`���X�e�[�W

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npUnregistTexture(
	struct	__npCONTEXT*	pContext,
	npU32					nStage
);

/*!

*/
NP_API npERROR NP_APIENTRY npSetTextureParam(
	npTEXTURE	*pTexture,
	npU32		nWidth,
	npU32		nHeight,
	GXTexFmt	fmt,
	npU32		uTexAddr,
	npU32		uPltAddr
);

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPTEXTURE_H */
