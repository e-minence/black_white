//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_light.h
 *	@brief		�t�B�[���h���C�g�V�X�e��
 *	@author		tomoya takahashi
 *	@date		2009.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_LIGHT_H__
#define __FIELD_LIGHT_H__

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>
#include "field_fog.h"

#ifdef PM_DEBUG
#define DEBUG_FIELD_LIGHT		// �t�B�[���h���C�g�̃f�o�b�N�@�\��L���ɂ���
#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------



//-------------------------------------
/// ���C�g�t�F�[�h
//=====================================
#define LIGHT_FADE_COUNT_MAX  ( 60 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�t�B�[���h���C�g�V�X�e��
//=====================================
typedef struct _FIELD_LIGHT FIELD_LIGHT;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�V�X�e���쐬�E�j��
//=====================================
extern FIELD_LIGHT* FIELD_LIGHT_Create( u32 light_no, int rtc_second, FIELD_FOG_WORK* p_fog, GFL_G3D_LIGHTSET* p_liblight, HEAPID heapID );
extern void FIELD_LIGHT_Delete( FIELD_LIGHT* p_sys );

//-------------------------------------
///	�V�X�e�����C��
//=====================================
extern void FIELD_LIGHT_Main( FIELD_LIGHT* p_sys, int rtc_second );
extern void FIELD_LIGHT_Reflect( FIELD_LIGHT* p_sys, BOOL force );


//-------------------------------------
///	���C�g���̕ύX
//=====================================
extern void FIELD_LIGHT_Change( FIELD_LIGHT* p_sys, u32 light_no );
extern void FIELD_LIGHT_ChangeEx( FIELD_LIGHT* p_sys, u32 arcid, u32 dataid, s32 sync );

// FIELD_LIGHT_Create or FIELD_LIGHT_Change�Őݒ肵��light_no�̃��C�g���ɖ߂�
extern void FIELD_LIGHT_ReLoadDefault( FIELD_LIGHT* p_sys );


//-------------------------------------
///	���C�g�t�F�[�h�@�\
//=====================================
extern void FIELD_LIGHT_COLORFADE_Start( FIELD_LIGHT* p_sys, GXRgb color, u32 insync, u32 outsync );
extern void FIELD_LIGHT_COLORFADE_StartOneWay( FIELD_LIGHT* p_sys, GXRgb color, u32 sync );
extern BOOL FIELD_LIGHT_COLORFADE_IsFade( const FIELD_LIGHT* cp_sys );

//-------------------------------------
///	���C�g��񔽉f��ON�EOFF
//=====================================
extern void FIELD_LIGHT_SetReflect( FIELD_LIGHT* p_sys, BOOL flag );
extern BOOL FIELD_LIGHT_GetReflect( const FIELD_LIGHT* cp_sys );


//-------------------------------------
///	�F�̎擾
// (�ǂݍ���ł��郉�C�g�f�[�^�i�G�N�Z���f�[�^�j�𔽉f���邽�߁A�t�F�[�h�r���̃J���[�Ȃǂł͂Ȃ�)
//=====================================
extern GXRgb FIELD_LIGHT_GetLightColor( const FIELD_LIGHT* cp_sys, u32 index );
extern GXRgb FIELD_LIGHT_GetDiffuseColor( const FIELD_LIGHT* cp_sys );
extern GXRgb FIELD_LIGHT_GetAmbientColor( const FIELD_LIGHT* cp_sys );
extern GXRgb FIELD_LIGHT_GetSpecularColor( const FIELD_LIGHT* cp_sys );
extern GXRgb FIELD_LIGHT_GetEmissionColor( const FIELD_LIGHT* cp_sys );


#ifdef DEBUG_FIELD_LIGHT

extern void FIELD_LIGHT_DEBUG_Init( FIELD_LIGHT* p_sys, HEAPID heapID );
extern void FIELD_LIGHT_DEBUG_Exit( FIELD_LIGHT* p_sys );

extern void FIELD_LIGHT_DEBUG_Control( FIELD_LIGHT* p_sys );
extern void FIELD_LIGHT_DEBUG_PrintData( FIELD_LIGHT* p_sys, GFL_BMPWIN* p_win );

#else

#define FIELD_LIGHT_DEBUG_Init( a,b )		((void)0)
#define FIELD_LIGHT_DEBUG_Exit( a )			((void)0)
#define FIELD_LIGHT_DEBUG_Control( a )		((void)0)
#define FIELD_LIGHT_DEBUG_PrintData( a,b )	((void)0)

#endif



//-----------------------------------------------------------------------------
/**
 *					���C�g�f�[�^�A�N�Z�X
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// ���C�g�P�f�[�^
//=====================================
typedef struct {
  u32     endtime;
  u8      light_flag[4];
  GXRgb   light_color[4];
  VecFx16   light_vec[4];

  GXRgb   diffuse;
  GXRgb   ambient;
  GXRgb   specular;
  GXRgb   emission;
  GXRgb   fog_color;
  GXRgb   bg_color;
} LIGHT_DATA;



#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __FIELD_LIGHT_H__


