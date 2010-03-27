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
//-------------------------------------
///	�O���t�B�b�N�ݒ胂�[�h
//=====================================
typedef enum
{
  BR_GRAPHIC_SETUP_2D,  //2D���C���ɃZ�b�g�A�b�v
  BR_GRAPHIC_SETUP_3D,  //3D���C���ɃZ�b�g�A�b�v(G3D�̏��������͍s���܂���)
} BR_GRAPHIC_SETUP_TYPE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�O���t�B�b�N���[�N
//=====================================
typedef struct _BR_GRAPHIC_WORK  BR_GRAPHIC_WORK;

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
extern BR_GRAPHIC_WORK * BR_GRAPHIC_Init( BR_GRAPHIC_SETUP_TYPE type, int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void BR_GRAPHIC_Exit( BR_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void BR_GRAPHIC_2D_Draw( BR_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void BR_GRAPHIC_3D_StartDraw( BR_GRAPHIC_WORK *p_wk );
extern void BR_GRAPHIC_3D_EndDraw( BR_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * BR_GRAPHIC_GetClunit( const BR_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * BR_GRAPHIC_GetCamera( const BR_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///�o�g�����[�R�[�_�[�ʐݒ�
//=====================================
extern void BR_GRAPHIC_SetMusicalMode( BR_GRAPHIC_WORK *p_wk, HEAPID heapID );
extern void BR_GRAPHIC_ReSetMusicalMode( BR_GRAPHIC_WORK *p_wk, HEAPID heapID );
