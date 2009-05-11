//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_3dbg.h
 *	@brief		�t�B�[���h�@�R�c�a�f��
 *	@author		tomoya takahshi
 *	@data		2009.03.30
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_3DBG_H__
#define __FIELD_3DBG_H__

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h�R�c�a�f�V�X�e��
//=====================================
typedef struct _FIELD_3DBG FIELD_3DBG;


//-------------------------------------
///	�t�B�[���h�R�c�a�f�V�X�e��
//	�������֐�
//=====================================
typedef struct {
	u16		size_x;			// �s�N�Z���P�ʂ̂a�f�O�ɕ`�悷�邳���̖ʃT�C�Y	
	u16		size_y;			// �s�N�Z���P�ʂ̂a�f�O�ɕ`�悷�邳���̖ʃT�C�Y
	fx32	camera_dist;	// �ʂ�z�u����J�����i�j�A�ʁj����̋���
	u8		polygon_id;		// �|���S��ID

	u8		pad[3];
} FIELD_3DBG_SETUP;

//-------------------------------------
///	�`��f�[�^
//=====================================
typedef struct {
	u16		nsbtex_id;
	u8		texsiz_s;		// GXTexSizeS
	u8		texsiz_t;		// GXTexSizeT
	u8		repeat;			// GXTexRepeat
	u8		flip;			// GXTexFlip
	u8		texfmt;			// GXTexFmt
	u8		texpltt;		// GXTexPlttColor0
	u8		alpha;			// �A���t�@�ݒ�

	u8		pad[3];
} FIELD_3DBG_WRITE_DATA;



//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�V�X�e���Ǘ�
//=====================================
extern FIELD_3DBG* FIELD_3DBG_Create( const FIELD_3DBG_SETUP* cp_setup, u32 heapID );
extern void FIELD_3DBG_Delete( FIELD_3DBG* p_sys );
extern void FIELD_3DBG_Write( FIELD_3DBG* p_sys );
extern void FIELD_3DBG_VBlank( FIELD_3DBG* p_sys );


//-------------------------------------
///	BG���\�[�X�̐ݒ�
//=====================================
extern void FIELD_3DBG_ClearWriteData( FIELD_3DBG* p_sys );
extern void FIELD_3DBG_SetWriteData( FIELD_3DBG* p_sys, ARCHANDLE* p_handle, const FIELD_3DBG_WRITE_DATA* cp_data, u32 heapID );


//-------------------------------------
///	���̐ݒ�E�擾
//=====================================
// ON/OFF
extern void FIELD_3DBG_SetVisible( FIELD_3DBG* p_sys, BOOL flag );
extern BOOL FIELD_3DBG_GetVisible( const FIELD_3DBG* cp_sys );

// �s�N�Z���P�ʂ̂a�f�O�ɕ`�悷�邳���̖ʃT�C�Y
extern void FIELD_3DBG_SetScreenSizeX( FIELD_3DBG* p_sys, u16 size_x );
extern u16 FIELD_3DBG_GetScreenSizeX( const FIELD_3DBG* cp_sys );
extern void FIELD_3DBG_SetScreenSizeY( FIELD_3DBG* p_sys, u16 size_y );
extern u16 FIELD_3DBG_GetScreenSizeY( const FIELD_3DBG* cp_sys );

// �ʂ�z�u����J�����i�j�A�ʁj����̋���
extern void FIELD_3DBG_SetCameraDist( FIELD_3DBG* p_sys, fx32 dist );
extern fx32 FIELD_3DBG_GetCameraDist( const FIELD_3DBG* cp_sys );

// �|���S��ID
extern void FIELD_3DBG_SetPolygonID( FIELD_3DBG* p_sys, u8 polygon_id );
extern u8 FIELD_3DBG_GetPolygonID( const FIELD_3DBG* cp_sys );

// �e�N�X�`�����s�[�g�ݒ�
extern void FIELD_3DBG_SetRepeat( FIELD_3DBG* p_sys, GXTexRepeat repeat );
extern GXTexRepeat FIELD_3DBG_GetRepeat( const FIELD_3DBG* cp_sys );

// �e�N�X�`���t���b�v�ݒ�
extern void FIELD_3DBG_SetFlip( FIELD_3DBG* p_sys, GXTexFlip flip );
extern GXTexFlip FIELD_3DBG_GetFlip( const FIELD_3DBG* cp_sys );

// �X�N���[���ݒ�
extern void FIELD_3DBG_SetScrollX( FIELD_3DBG* p_sys, s32 x );
extern s32 FIELD_3DBG_GetScrollX( const FIELD_3DBG* cp_sys );
extern void FIELD_3DBG_SetScrollY( FIELD_3DBG* p_sys, s32 y );
extern s32 FIELD_3DBG_GetScrollY( const FIELD_3DBG* cp_sys );

// ��]
extern void FIELD_3DBG_SetRotate( FIELD_3DBG* p_sys, u16 rotate );
extern u16 FIELD_3DBG_GetRotate( const FIELD_3DBG* cp_sys );

// �g��
extern void FIELD_3DBG_SetScaleX( FIELD_3DBG* p_sys, fx32 x );
extern fx32 FIELD_3DBG_GetScaleX( const FIELD_3DBG* cp_sys );
extern void FIELD_3DBG_SetScaleY( FIELD_3DBG* p_sys, fx32 y );
extern fx32 FIELD_3DBG_GetScaleY( const FIELD_3DBG* cp_sys );


// �A���t�@�ݒ�
// *�g�p�ɂ͒��ӂ��K�v�ł��B
// �������|���S�����t�B�[���h�ɕ`�悳��Ă���ꏊ�ł�
// �������ɂ��Ȃ��ł��������B
// �d�Ȃ�������������Ȃ�܂��B
extern void FIELD_3DBG_SetAlpha( FIELD_3DBG* p_sys, u8 alpha );
extern u8 FIELD_3DBG_GetAlpha( const FIELD_3DBG* cp_sys );

#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __FIELD_3DBG_H__


