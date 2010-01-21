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
typedef struct _COMM_BTL_DEMO_GRAPHIC_WORK  COMM_BTL_DEMO_GRAPHIC_WORK;

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
extern COMM_BTL_DEMO_GRAPHIC_WORK * COMM_BTL_DEMO_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void COMM_BTL_DEMO_GRAPHIC_Exit( COMM_BTL_DEMO_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void COMM_BTL_DEMO_GRAPHIC_2D_Draw( COMM_BTL_DEMO_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void COMM_BTL_DEMO_GRAPHIC_3D_StartDraw( COMM_BTL_DEMO_GRAPHIC_WORK *p_wk );
extern void COMM_BTL_DEMO_GRAPHIC_3D_EndDraw( COMM_BTL_DEMO_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * COMM_BTL_DEMO_GRAPHIC_GetClunit( const COMM_BTL_DEMO_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * COMM_BTL_DEMO_GRAPHIC_GetCamera( const COMM_BTL_DEMO_GRAPHIC_WORK *cp_wk );
