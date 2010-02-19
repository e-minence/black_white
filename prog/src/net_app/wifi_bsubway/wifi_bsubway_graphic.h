//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		graphic.h
 *	@brief	�O���t�B�b�N��{�ݒ�
 *	@author	Toru=Nagihashi
 *	@date		2009.09.30
 *	@date		2010.02.18  WiFi�o�g���T�u�E�F�C�p�ɕύX  tomoya takahashi
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
typedef struct _WIFI_BSUBWAY_GRAPHIC_WORK  WIFI_BSUBWAY_GRAPHIC_WORK;

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
extern WIFI_BSUBWAY_GRAPHIC_WORK * WIFI_BSUBWAY_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void WIFI_BSUBWAY_GRAPHIC_Exit( WIFI_BSUBWAY_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void WIFI_BSUBWAY_GRAPHIC_2D_Draw( WIFI_BSUBWAY_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void WIFI_BSUBWAY_GRAPHIC_3D_StartDraw( WIFI_BSUBWAY_GRAPHIC_WORK *p_wk );
extern void WIFI_BSUBWAY_GRAPHIC_3D_EndDraw( WIFI_BSUBWAY_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * WIFI_BSUBWAY_GRAPHIC_GetClunit( const WIFI_BSUBWAY_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * WIFI_BSUBWAY_GRAPHIC_GetCamera( const WIFI_BSUBWAY_GRAPHIC_WORK *cp_wk );
