//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_ground_anime.h
 *	@brief	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
 *	@author	tomoya takahshi
 *	@data		2009.04.30
 *
 *	���W���[�����FFIELD_GRANM
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

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
///	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
//=====================================
typedef struct _FIELD_GRANM FIELD_GRANM;

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�������[�N
//=====================================
typedef struct _FIELD_GRANM_WORK FIELD_GRANM_WORK;

//-------------------------------------
///	�������f�[�^
//=====================================
typedef struct {
	u16 ita_use:1;
	u16 itp_use:1;
	u16 block_num:14;
	u16 ita_arcID;
	u16 ita_dataID;
	u16 itp_arcID;
	u16 itp_anmID;
	u16 itp_texID;
} FIELD_GRANM_SETUP;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�����Ǘ��V�X�e��
//	����֐�
//=====================================
extern FIELD_GRANM* FIELD_GRANM_Create( const FIELD_GRANM_SETUP* cp_setup, const GFL_G3D_RES* cp_tex, u32 heapID );
extern void FIELD_GRANM_Delete( FIELD_GRANM* p_sys );
extern void FIELD_GRANM_Main( FIELD_GRANM* p_sys );
extern FIELD_GRANM_WORK* FIELD_GRANM_GetWork( const FIELD_GRANM* cp_sys, u32 idx );
extern void FIELD_GRANM_SetAnimeSpeed( FIELD_GRANM* p_sys, fx32 speed );
extern fx32 FIELD_GRANM_GetAnimeSpeed( const FIELD_GRANM* cp_sys );
extern void FIELD_GRANM_SetAutoAnime( FIELD_GRANM* p_sys, BOOL flag );
extern BOOL FIELD_GRANM_GetAutoAnime( const FIELD_GRANM* cp_sys );


//-------------------------------------
///	�t�B�[���h�n�ʃA�j���[�V�������[�N
//	����֐�
//=====================================
extern void FIELD_GRANM_WORK_Bind( FIELD_GRANM_WORK* p_wk, const GFL_G3D_RES* cp_mdlres, const GFL_G3D_RES* cp_texres, NNSG3dRenderObj* p_rendobj );
extern void FIELD_GRANM_WORK_Release( FIELD_GRANM_WORK* p_wk );



#ifdef _cplusplus
}	// extern "C"{
#endif



