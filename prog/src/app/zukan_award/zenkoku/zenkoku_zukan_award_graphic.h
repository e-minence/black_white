//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		zenkoku_zukan_award_graphic.h
 *	@brief	�O���t�B�b�N�ݒ�
 *	@author	Koji Kawada
 *	@date		2010.03.04
 *	@note   ui_template_graphic.h���R�s�y���Ė��O��ύX���������ł��B
 *
 *	���W���[�����FZENKOKU_ZUKAN_AWARD_GRAPHIC
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
typedef struct _ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK  ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK;

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
extern ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK * ZENKOKU_ZUKAN_AWARD_GRAPHIC_Init( int display_select, HEAPID heapID );
//-------------------------------------
///�j��
//=====================================
extern void ZENKOKU_ZUKAN_AWARD_GRAPHIC_Exit( ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *p_wk );
//-------------------------------------
///�QD�`��
//=====================================
extern void ZENKOKU_ZUKAN_AWARD_GRAPHIC_2D_Draw( ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *p_wk );

//-------------------------------------
///�RD�`��	�RD���g��Ȃ��ꍇ�A�ǂ�ł��������܂���
//=====================================
extern void ZENKOKU_ZUKAN_AWARD_GRAPHIC_3D_StartDraw( ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *p_wk );
extern void ZENKOKU_ZUKAN_AWARD_GRAPHIC_3D_EndDraw( ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *p_wk );

//-------------------------------------
///���擾	�����Őݒ肵��CLUNIT��Ԃ��܂�	OBJ�g��Ȃ��ꍇNULL
//=====================================
extern GFL_CLUNIT * ZENKOKU_ZUKAN_AWARD_GRAPHIC_GetClunit( const ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *cp_wk );

//-------------------------------------
///���擾	�����Őݒ肵���J������Ԃ��܂�	3D�g��Ȃ��ꍇNULL
//=====================================
extern GFL_G3D_CAMERA * ZENKOKU_ZUKAN_AWARD_GRAPHIC_GetCamera( const ZENKOKU_ZUKAN_AWARD_GRAPHIC_WORK *cp_wk );
