//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_fog.h
 *	@brief		�t�H�O�Ǘ��V�X�e��
 *	@author		tomoya takahashi
 *	@date		2009.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_FOG_H__
#define __FIELD_FOG_H__

#ifdef _cplusplus
extern "C"{
#endif

#include <nitro.h>

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	FOG�ݒ�̒萔
//  NITRO-SDK�̒萔�����l�[�����Ă邾���ł�
//=====================================
typedef enum{
	FIELD_FOG_BLEND_COLOR_ALPHA	= GX_FOGBLEND_COLOR_ALPHA, 		// �s�N�Z���̃J���[�l�ƃ��l�Ƀt�H�O�u�����f�B���O 
	FIELD_FOG_BLEND_ALPHA		= GX_FOGBLEND_ALPHA,			// �s�N�Z���̃��l�݂̂Ƀt�H�O�u�����f�B���O 

	FIELD_FOG_BLEND_MAX,	// �V�X�e�����Ŏg�p
} FIELD_FOG_BLEND;

typedef enum{
	FIELD_FOG_SLOPE_0x8000		= GX_FOGSLOPE_0x8000,			// �t�H�O���f�v�X�l��0x8000�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x4000		= GX_FOGSLOPE_0x4000,			// �t�H�O���f�v�X�l��0x4000�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x2000		= GX_FOGSLOPE_0x2000,			// �t�H�O���f�v�X�l��0x2000�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x1000		= GX_FOGSLOPE_0x1000,			// �t�H�O���f�v�X�l��0x1000�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0800		= GX_FOGSLOPE_0x0800,			// �t�H�O���f�v�X�l��0x0800�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0400		= GX_FOGSLOPE_0x0400,			// �t�H�O���f�v�X�l��0x0400�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0200		= GX_FOGSLOPE_0x0200,			// �t�H�O���f�v�X�l��0x0200�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0100		= GX_FOGSLOPE_0x0100,			// �t�H�O���f�v�X�l��0x0100�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0080		= GX_FOGSLOPE_0x0080,			// �t�H�O���f�v�X�l��0x0080�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0040		= GX_FOGSLOPE_0x0040,			// �t�H�O���f�v�X�l��0x0040�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 
	FIELD_FOG_SLOPE_0x0020		= GX_FOGSLOPE_0x0020,			// �t�H�O���f�v�X�l��0x0020�̕��Œi�K�I�ɔZ���Ȃ��Ă����܂� 

	FIELD_FOG_SLOPE_MAX,	// �V�X�e�����Ŏg�p
} FIELD_FOG_SLOPE;

#define FIELD_FOG_TBL_MAX		( 32 )	// �t�H�O�e�[�u���̒i�K��

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�X�e�����[�N
//=====================================
typedef struct _FIELD_FOG_WORK FIELD_FOG_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�X�e�������E�j��
//=====================================
extern FIELD_FOG_WORK* FIELD_FOG_Create( HEAPID heapID );
extern void FIELD_FOG_Delete( FIELD_FOG_WORK* p_wk );


//-------------------------------------
///	���C������
//=====================================
extern void FIELD_FOG_Main( FIELD_FOG_WORK* p_wk );
extern void FIELD_FOG_Reflect( FIELD_FOG_WORK* p_wk );


//-------------------------------------
///	���ݒ�E�擾
//=====================================
// �ݒ�
extern void FIELD_FOG_SetFlag( FIELD_FOG_WORK* p_wk, BOOL flag );
extern void FIELD_FOG_SetBlendMode( FIELD_FOG_WORK* p_wk, FIELD_FOG_BLEND blendmode );
extern void FIELD_FOG_SetSlope( FIELD_FOG_WORK* p_wk, FIELD_FOG_SLOPE slope );
extern void FIELD_FOG_SetOffset( FIELD_FOG_WORK* p_wk, s32 depth_offset );
extern void FIELD_FOG_SetColorRgb( FIELD_FOG_WORK* p_wk, GXRgb rgb );
extern void FIELD_FOG_SetColorA( FIELD_FOG_WORK* p_wk, u8 a );
extern void FIELD_FOG_SetTbl( FIELD_FOG_WORK* p_wk, u8 index, u8 data );
extern void FIELD_FOG_SetTblAll( FIELD_FOG_WORK* p_wk, const u8* cp_tbl );

// �擾
extern BOOL FIELD_FOG_GetFlag( const FIELD_FOG_WORK* cp_wk );
extern FIELD_FOG_BLEND FIELD_FOG_GetBlendMode( const FIELD_FOG_WORK* cp_wk );
extern FIELD_FOG_SLOPE FIELD_FOG_GetSlope( const FIELD_FOG_WORK* cp_wk );
extern s32 FIELD_FOG_GetOffset( const FIELD_FOG_WORK* cp_wk );
extern GXRgb FIELD_FOG_GetColorRgb( const FIELD_FOG_WORK* cp_wk );
extern u8 FIELD_FOG_GetColorA( const FIELD_FOG_WORK* cp_wk );
extern u8 FIELD_FOG_GetTbl( const FIELD_FOG_WORK* cp_wk, u8 index );

//-------------------------------------
///	��{�I�ȃt�H�O�e�[�u���̐���
// �T�^�I�ȂP�����̃O���t
//=====================================
extern void FIELD_FOG_TBL_SetUpDefault( FIELD_FOG_WORK* p_wk );

//-------------------------------------
///	�t�F�[�h���N�G�X�g
//=====================================
extern void FIELD_FOG_FADE_Init( FIELD_FOG_WORK* p_wk, s32 offset_end, FIELD_FOG_SLOPE slope_end, u32 count_max );
extern void FIELD_FOG_FADE_InitEx( FIELD_FOG_WORK* p_wk, s32 offset_start, s32 offset_end, FIELD_FOG_SLOPE slope_start, FIELD_FOG_SLOPE slope_end, u16 count_max  );
extern BOOL FIELD_FOG_FADE_IsFade( const FIELD_FOG_WORK* cp_wk );


#ifdef PM_DEBUG
extern void FIELD_FOG_DEBUG_Init( FIELD_FOG_WORK* p_wk, HEAPID heapID );
extern void FIELD_FOG_DEBUG_Exit( FIELD_FOG_WORK* p_wk );

extern void FIELD_FOG_DEBUG_Control( FIELD_FOG_WORK* p_wk );
extern void FIELD_FOG_DEBUG_PrintData( FIELD_FOG_WORK* p_wk, GFL_BMPWIN* p_win );
#endif


#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __FIELD_FOG_H__


