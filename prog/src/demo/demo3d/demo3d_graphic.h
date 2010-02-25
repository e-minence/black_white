//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		demo3d_graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	genya_hosaka
 *	@date		2009.11.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gflib.h>

#include "demo/demo3d.h" // for DEMO3D_ID

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
typedef struct _DEMO3D_GRAPHIC_WORK  DEMO3D_GRAPHIC_WORK;

/*
 *  @brief  3D�V�[���p�����[�^�w��\����
 */
typedef struct _DEMO3D_3DSCENE_PARAM{
  u8  edge_marking_f:1;
  u8  fog_f:1;
  u8  dmy:6;

  GXRgb  edgeColorTbl[8]; 
}DEMO3D_3DSCENE_PARAM;

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
extern DEMO3D_GRAPHIC_WORK * DEMO3D_GRAPHIC_Init( int display_select, DEMO3D_ID demo_id, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void DEMO3D_GRAPHIC_Exit( DEMO3D_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void DEMO3D_GRAPHIC_2D_Draw( DEMO3D_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void DEMO3D_GRAPHIC_3D_StartDraw( DEMO3D_GRAPHIC_WORK *p_wk );
extern void DEMO3D_GRAPHIC_3D_EndDraw( DEMO3D_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * DEMO3D_GRAPHIC_GetClunit( const DEMO3D_GRAPHIC_WORK *cp_wk );

//----------------------------------------------------------------------------
/**
 *	@brief	�f���V�[����3D�p�����[�^�ݒ�
 *
 *	@param	const GRAPHIC_WORK *cp_wk		���[�N
 */
//-----------------------------------------------------------------------------
extern void DEMO3D_GRAPHIC_Scene3DParamSet( DEMO3D_GRAPHIC_WORK *p_wk, const FIELD_LIGHT_STATUS* f_light, DEMO3D_3DSCENE_PARAM* prm );

