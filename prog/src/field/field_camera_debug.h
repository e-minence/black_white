//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_camera_debug.h
 *	@brief  �t�B�[���h�J�����@�f�o�b�N�@�\
 *	@author	tomoya takahashi
 *	@date		2010.04.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "field_camera.h"
#include "test/camera_adjust_view.h"

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

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

#ifdef  PM_DEBUG
//------------------------------------------------------------------
//  �f�o�b�O�p�F����ʑ���Ƃ̃o�C���h
//------------------------------------------------------------------
extern void FIELD_CAMERA_DEBUG_BindSubScreen(FIELD_CAMERA * camera, void * param, FIELD_CAMERA_DEBUG_BIND_TYPE type, HEAPID heapID );
extern void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera);
extern void FIELD_CAMERA_DEBUG_GetBindSubScreenTarget( const FIELD_CAMERA * camera, VecFx32* p_target );



//------------------------------------------------------------------
//  �J�����@�f�o�b�N�\���֐�
//------------------------------------------------------------------
extern void FIELD_CAMERA_DEBUG_Draw( const FIELD_CAMERA* camera);




//------------------------------------------------------------------
//  �f�o�b�O�J�����ڍב���
/*
 *  NONE�F�^�[�Q�b�g�A�J�������s�ړ�
 *	B�F�J�������]�i�^�[�Q�b�g���W���ς��j
 *	Y�F�J�������]�i�J�������W���ς��j
 *	A�F�p�[�X����
 *	X�F�^�[�Q�b�g�J�����o�C���h��ON�EOFF
 *	
 *	START�F�o�b�t�@�����O���[�h�ύX
 *
 *	�߂�l�@TRUE  �ύX����    FALSE�@�ύX�Ȃ�
 */
//------------------------------------------------------------------
extern void FIELD_CAMERA_DEBUG_InitControl( FIELD_CAMERA* camera, HEAPID heapID );
extern void FIELD_CAMERA_DEBUG_ExitControl( FIELD_CAMERA* camera );
extern BOOL FIELD_CAMERA_DEBUG_Control( FIELD_CAMERA* camera, int trg, int cont, int repeat );
extern void FIELD_CAMERA_DEBUG_DrawInfo( FIELD_CAMERA* camera, GFL_BMPWIN* p_win, fx32 map_size_x, fx32 map_size_z );
extern void FIELD_CAMERA_DEBUG_DrawControlHelp( FIELD_CAMERA* camera, GFL_BMPWIN* p_win );

#endif

#ifdef _cplusplus
}	// extern "C"{
#endif



