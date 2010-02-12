//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		zukan_detail_graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Koji Kawada
 *	@date		2010.02.02
 *	@note   ui_template_graphic.h���R�s�y���Ė��O��ύX���������ł��B
 *
 *	���W���[�����FZUKAN_DETAIL_GRAPHIC
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
typedef struct _ZUKAN_DETAIL_GRAPHIC_WORK  ZUKAN_DETAIL_GRAPHIC_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///������
//	�f�B�X�v���C�Z���N�g�ɂ�GX_DISP_SELECT_MAIN_SUB or GX_DISP_SELECT_SUB_MAIN��
//	�n���Ă��������B�r���ŏ������������ꍇ�AGFL_DISP_SetDispSelect���g�p���ĉ������B
//=====================================
extern ZUKAN_DETAIL_GRAPHIC_WORK * ZUKAN_DETAIL_GRAPHIC_Init( int display_select, HEAPID heapID, BOOL use_3d );
//-------------------------------------
///�j��
//=====================================
extern void ZUKAN_DETAIL_GRAPHIC_Exit( ZUKAN_DETAIL_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void ZUKAN_DETAIL_GRAPHIC_2D_Draw( ZUKAN_DETAIL_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void ZUKAN_DETAIL_GRAPHIC_3D_StartDraw( ZUKAN_DETAIL_GRAPHIC_WORK *p_wk );
extern void ZUKAN_DETAIL_GRAPHIC_3D_EndDraw( ZUKAN_DETAIL_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * ZUKAN_DETAIL_GRAPHIC_GetClunit( const ZUKAN_DETAIL_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * ZUKAN_DETAIL_GRAPHIC_GetCamera( const ZUKAN_DETAIL_GRAPHIC_WORK *cp_wk );

//-------------------------------------
/// 3D��L��/�����ɂ���
//=====================================
extern void ZUKAN_DETAIL_GRAPHIC_Init3D( ZUKAN_DETAIL_GRAPHIC_WORK* p_wk, HEAPID heapID );
extern void ZUKAN_DETAIL_GRAPHIC_Exit3D( ZUKAN_DETAIL_GRAPHIC_WORK* p_wk );

