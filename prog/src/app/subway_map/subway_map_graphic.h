//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		subway_map_graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Koji Kawada
 *	@date		2010.03.15
 *	@note   ui_template_graphic.h���R�s�y���Ė��O��ύX���������ł��B
 *
 *	���W���[�����FSUBWAY_MAP_GRAPHIC
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
typedef struct _SUBWAY_MAP_GRAPHIC_WORK  SUBWAY_MAP_GRAPHIC_WORK;

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
extern SUBWAY_MAP_GRAPHIC_WORK * SUBWAY_MAP_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void SUBWAY_MAP_GRAPHIC_Exit( SUBWAY_MAP_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void SUBWAY_MAP_GRAPHIC_2D_Draw( SUBWAY_MAP_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void SUBWAY_MAP_GRAPHIC_3D_StartDraw( SUBWAY_MAP_GRAPHIC_WORK *p_wk );
extern void SUBWAY_MAP_GRAPHIC_3D_EndDraw( SUBWAY_MAP_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * SUBWAY_MAP_GRAPHIC_GetClunit( const SUBWAY_MAP_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * SUBWAY_MAP_GRAPHIC_GetCamera( const SUBWAY_MAP_GRAPHIC_WORK *cp_wk );
