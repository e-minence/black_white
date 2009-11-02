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
typedef struct _WIFIBATTLEMATCH_GRAPHIC_WORK  WIFIBATTLEMATCH_GRAPHIC_WORK;

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
extern WIFIBATTLEMATCH_GRAPHIC_WORK * WIFIBATTLEMATCH_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void WIFIBATTLEMATCH_GRAPHIC_Exit( WIFIBATTLEMATCH_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void WIFIBATTLEMATCH_GRAPHIC_2D_Draw( WIFIBATTLEMATCH_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void WIFIBATTLEMATCH_GRAPHIC_3D_StartDraw( WIFIBATTLEMATCH_GRAPHIC_WORK *p_wk );
extern void WIFIBATTLEMATCH_GRAPHIC_3D_EndDraw( WIFIBATTLEMATCH_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * WIFIBATTLEMATCH_GRAPHIC_GetClunit( const WIFIBATTLEMATCH_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * WIFIBATTLEMATCH_GRAPHIC_GetCamera( const WIFIBATTLEMATCH_GRAPHIC_WORK *cp_wk );
