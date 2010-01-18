//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shinka_demo_graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Koji Kawada
 *	@date		2010.01.13
 *	@note   ui_template_graphic.h���R�s�y���Ė��O��ύX���������ł��B
 *
 *	���W���[�����FSHINKADEMO_GRAPHIC
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include <gflib.h>
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�O���t�B�b�N���[�N
//=====================================
typedef struct _SHINKADEMO_GRAPHIC_WORK  SHINKADEMO_GRAPHIC_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///������
//	�f�B�X�v���C�Z���N�g�ɂ�GX_DISP_SELECT_MAIN_SUB or GX_DISP_SELECT_SUB_MAIN��
//	�n���Ă��������B�r���ŏ������������ꍇ�AGFL_DISP_SetDispSelect���g�p���ĉ������B
//  �T�uBG�̏������͍s��Ȃ��B
//=====================================
extern SHINKADEMO_GRAPHIC_WORK * SHINKADEMO_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//  �T�uBG�̔j���͍s��Ȃ��B
//=====================================
extern void SHINKADEMO_GRAPHIC_Exit( SHINKADEMO_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void SHINKADEMO_GRAPHIC_2D_Draw( SHINKADEMO_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void SHINKADEMO_GRAPHIC_3D_StartDraw( SHINKADEMO_GRAPHIC_WORK *p_wk );
extern void SHINKADEMO_GRAPHIC_3D_EndDraw( SHINKADEMO_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * SHINKADEMO_GRAPHIC_GetClunit( const SHINKADEMO_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * SHINKADEMO_GRAPHIC_GetCamera( const SHINKADEMO_GRAPHIC_WORK *cp_wk );


//-------------------------------------
/// �T�uBG�̏�����
//=====================================
extern void SHINKADEMO_GRAPHIC_InitBGSub( SHINKADEMO_GRAPHIC_WORK* p_wk );
//-------------------------------------
/// �T�uBG�̔j��
//=====================================
extern void SHINKADEMO_GRAPHIC_ExitBGSub( SHINKADEMO_GRAPHIC_WORK* p_wk );
