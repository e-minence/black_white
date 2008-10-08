/*=============================================================================
/*!

	@file	npContext.h

	@brief	�����_�����O�R���e�L�X�g�̒�`�t�@�C��
	
	@author	ka2<br>
			Copyright (c) 2005 Polygon Magic, Inc.

	$Revision: 1.9 $
	$Date: 2006/03/28 03:57:03 $

*//*=========================================================================*/
#ifndef	NPCONTEXT_H
#define	NPCONTEXT_H

/*! @name	C �����P�[�W�O�Ή� */
//!@{

#include "core/npSystem.h"

#include "core/npCamera.h"
#include "core/npLight.h"

#include "npAnimation.h"

#include "core/npRenderState.h"
#include "core/npVertexFormat.h"

#include "core/npMaterial.h"
#include "core/npTexture.h"

#include "core/npTransform.h"
#include "core/npPrimitive.h"

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
/*! �ő僌���_�����O�p�X�� */
#define	NP_MAXPASSES	( 8 )

/*! @name	�N���A����T�[�t�F�C�X�������t���O */
//!@{

//!@}

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	enum
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	struct
//-----------------------------------------------------------------------------
/*!

	�f�o�C�X�f�X�N���v�V����

*/
typedef struct __npDEVICEDESC
{
	int a;
}
npDEVICEDESC;

/*!

	@struct	�����_�����O�R���e�L�X�g

*/
typedef struct __npCONTEXT
{
	npOBJECT		Object;			/* ���ۃI�u�W�F�N�g				*/

	npSTATE			m_hResult;		/* �V�X�e���ˑ��̃G���[�R�[�h	*/
	npU32			m_nStencil;		/* �X�e���V���l		*/
	npSTATE			m_nVertexFormat;

//	npFLOAT			m_fDepth;		/* �[�x�l			*/
//	npFRGBA			m_pFColor;		/* �N���A�J���[		*/

	npU32			m_nColor;	///< Clear Color
	npU32			m_nDepth;	///< Clear Depth

	/* �g�����X�t�H�[���}�g���N�X�p���b�g */
	npFMATRIX		m_matPallet[ NP_INDEXMATRIX_MAX ];

	npTEXTURE*		m_pTextureHeap;	/* �e�N�X�`���A���[�i	*/
	npTEXTURE*		m_pTextureList;	/* �e�N�X�`�����X�g		*/
//	npSHADER*		m_pShaderHeap;	/* �V�F�[�_�A���[�i		*/
//	npSHADER*		m_pShaderList;	/* �V�F�[�_���X�g		*/

	npU32			m_nWidth;
	npU32			m_nHeight;
	npU32			m_pReserved1[ 2 ];
	
	npU32			m_nPolyMax;
	npU32			m_nPolyMin;
	npU32			m_nPolygonID;
}
npCONTEXT;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global variable
//-----------------------------------------------------------------------------

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
//-----------------------------------------------------------------------------
//	global function
//-----------------------------------------------------------------------------

/*!

	�����_�����O�R���e�L�X�g�𐶐�����

	@param	pSystem		�V�X�e���I�u�W�F�N�g
	@param	pInstance	�O���C���X�^���X
	@param	ppContext	�󂯎�郌���_�����O�R���e�L�X�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npCreateContext( 
	npSYSTEM*	pSystem,
	npVOID*		pInstance,
	npCONTEXT** ppContext 
);

/*!

	�����_�����O�R���e�L�X�g���J������

	@param	pContext	�Ώۂ̃����_�����O�R���e�L�X�g

*/
NP_API npVOID NP_APIENTRY npReleaseContext( npCONTEXT* pContext );

/*!

	�����_�����O�R���e�L�X�g�̏�Ԃ����Z�b�g����

	@param	pContext	�Ώۂ̃����_�����O�R���e�L�X�g

	@return	�G���[�R�[�h

	@note	npCreateContext(...) ���͕`��O�Ɉ�x�A�K���R�[�����Ȃ���΂Ȃ�Ȃ�

*/
NP_API npERROR NP_APIENTRY npResetEnvironment( npCONTEXT* pContext );

/*!

	�����_�����O�R���e�L�X�g�̃X�y�b�N���m�F����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nState		����������

	@return	�X�y�b�N�̏��

*/
NP_API npSTATE NP_APIENTRY npConfirmContext( npCONTEXT* pContext, npSTATE nState );

/*!

	�V�X�e���ˑ��̃G���[�R�[�h���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�V�X�e���ˑ��̃G���[�R�[�h

*/
#define	npErrorCode( pContext )	( ( pContext )->m_hResult )

/*!

	�E�B���h�E�n���h�����Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	hWnd		�E�B���h�E�n���h��

	@param	�G���[�R�[�h

	@note	Windows ���̂�

*/
#ifdef NP_WGL32
#	define	npSetWindowHandle( pContext, hWnd )	(	\
	( pContext )->m_hWnd = ( hWnd ), NP_SUCCESS )
#else
#	define	npSetWindowHandle( pContext, hWnd )	( NP_SUCCESS )
#endif

/*!

	�E�B���h�E�n���h�����擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�E�B���h�E�n���h��

	@note	Windows ���̂�

*/
#define	npGetWindowHandle( pContext ) ( ( pContext )->m_hWnd )

/*!
	
	�N���A�J���[�l���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nColor		�N���A�J���[�l(16bit�l X1R5G5B5)

	@return	�G���[�R�[�h

*/
#define npSetClearColor(pContext, nColor)	((pContext)->m_nColor = (nColor), NP_SUCCESS)

/*!

	�N���A�J���[�l���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�J���[�l

*/
#define npGetClearColor(pContext)	((pContext)->nColor)

/*!

	�N���A����[�x�l���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	fDepth		�N���A�[�x�l

	@return	�G���[�R�[�h

*/
#define npSetClearDepth(pContext, nDepth)	((pContext)->m_nDepth = (nDepth), NP_SUCCESS)

/*!

	�N���A����[�x�l���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�[�x�l

*/
#define	npGetClearDepth(pContext)	((pContext)->m_nDepth)

/*!

	�X�e���V���o�b�t�@�̃G���g���ɕۑ����鐮���l���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nStencil	�Ώۂ̐����l

	@return	�G���[�R�[�h

*/
#define	npSetClearStencil( pContext, nStencil )	(	\
	( pContext )->m_nStencil = ( nStencil ), NP_SUCCESS )

/*!
	
	�X�e���V���o�b�t�@�̃G���g���ɕۑ����鐮���l���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�Ώۂ̐����l

*/
#define	npGetClearStencil( pContext )	( ( pContext )->m_nStencil )

/*!
	
	�|���S��ID�̕��̍ő���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�Ώۂ̐����l

*/
#define	npGetPolygonIDMax( pContext )	( ( pContext )->m_nPolyMax )

/*!
	
	�|���S��ID�̕��̍ő��ݒ肷��

	@param	pContext	�����_�����O�R���e�L�X�g


*/
#define	npSetPolygonIDMax( pContext, polymax )	{( pContext )->m_nPolyMax = polymax;}

/*!
	
	�|���S��ID�̕��̍ŏ����擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�Ώۂ̐����l

*/
#define	npGetPolygonIDMin( pContext )	( ( pContext )->m_nPolyMin )

/*!
	
	�|���S��ID�̕��̍ŏ���ݒ肷��

	@param	pContext	�����_�����O�R���e�L�X�g


*/
#define	npSetPolygonIDMin( pContext, polymin )	{( pContext )->m_nPolyMin = polymin;}

/*!
	
	�|���S��ID���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�Ώۂ̐����l

*/
#define	npGetPolygonID( pContext )	( ( pContext )->m_nPolygonID )

/*!
	
	�|���S��ID��ݒ肷��

	@param	pContext	�����_�����O�R���e�L�X�g


*/
#define	npSetPolygonID( pContext, polyID )	{( pContext )->m_nPolygonID = polyID;}

/*!

	�`��p�o�b�t�@���N���A����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nMask		�N���A�}�X�N

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npClear( npCONTEXT* pContext, npSTATE nMask );

/*!

	�o�b�t�@�̃X���b�v���s��

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npSwapBuffers( npCONTEXT* pContext );

/*!

	�`����J�n����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�G���[�R�[�h

*/
#define	npBeginRender( pContext )	( NP_SUCCESS )

/*!

	�`����I������

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	�G���[�R�[�h

*/
NP_API npERROR NP_APIENTRY npEndRender( npCONTEXT* pContext );

/*!

	�f�o�b�O�t�H���g�̐F���Z�b�g����

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	nColor		�t�H���g�J���[

	@return	�G���[�R�[�h

*/
#define	npDebugSetFontColor( pContext, nColor )

/*!

	�f�o�b�O�t�H���g�̐F���擾����

	@param	pContext	�����_�����O�R���e�L�X�g

	@return	���݂̃f�o�b�O�t�H���g�J���[

*/
#define	npDebugGetFontColor( pContext )

/*!

	�f�o�b�O�t�H���g���o�͂���

	@param	pContext	�����_�����O�R���e�L�X�g
	@param	X			�o�͂��镶����� X ���W
	@param	Y			�o�͂��镶����� Y ���W
	@param	szString	�o�͂��镶����
	@param	nLength		�o�͂��镶����̒���

*/
#ifdef NP_DEBUG
NP_API npVOID NP_APIENTRY npDebugTextOut( 
	npCONTEXT*	pContext,
	npU32		X,
	npU32		Y,
	npCHAR*		szString,
	npSIZE		nLength
);
#else
#	define	npDebugTextOut( pContext, X, Y, szString, nLength )
#endif

/*!

	�R���e�L�X�g���������_���v����

*/
#ifdef NP_DEBUG
NP_API npVOID NP_APIENTRY npDebugDumpContext( npVOID );
#else
#	define npDebugDumpContext( npVOID )
#endif

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifdef __cplusplus
}
#endif

#endif	/* NPCONTEXT_H */
