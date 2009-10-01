//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Toru=Nagihashi
 *	@date		2009.09.30
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
typedef struct _UI_TEMPLATE_GRAPHIC_WORK  UI_TEMPLATE_GRAPHIC_WORK;

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
extern UI_TEMPLATE_GRAPHIC_WORK * UI_TEMPLATE_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void UI_TEMPLATE_GRAPHIC_Exit( UI_TEMPLATE_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void UI_TEMPLATE_GRAPHIC_2D_Draw( UI_TEMPLATE_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void UI_TEMPLATE_GRAPHIC_3D_StartDraw( UI_TEMPLATE_GRAPHIC_WORK *p_wk );
extern void UI_TEMPLATE_GRAPHIC_3D_EndDraw( UI_TEMPLATE_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * UI_TEMPLATE_GRAPHIC_GetClunit( const UI_TEMPLATE_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * UI_TEMPLATE_GRAPHIC_GetCamera( const UI_TEMPLATE_GRAPHIC_WORK *cp_wk );
