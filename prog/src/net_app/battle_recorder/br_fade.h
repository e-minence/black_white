//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_fade.h
 *	@brief  �o�g�����R�[�_�[�p�t�F�[�h�V�X�e��
 *	@author	Toru=Nagihashi
 *	@date		2009.12.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�t�F�[�h�̎��
//=====================================
typedef enum
{
  BR_FADE_TYPE_MASTERBRIGHT_BLACK,  //�P�x  ��
  BR_FADE_TYPE_ALPHA_BG012OBJ,      //�A���t�@�@BG0,1,2,OBJ

} BR_FADE_TYPE;

//-------------------------------------
///	�t�F�[�h������ӏ�
//=====================================
typedef enum
{ 
  BR_FADE_DISPLAY_MAIN  = 1<<0,
  BR_FADE_DISPLAY_SUB   = 1<<1,
  BR_FADE_DISPLAY_BOTH  = BR_FADE_DISPLAY_MAIN | BR_FADE_DISPLAY_SUB,

}BR_FADE_DISPLAY;

//-------------------------------------
///	�t�F�[�h�̕���
//=====================================
typedef enum
{ 
  BR_FADE_DIR_IN,   //16->0
  BR_FADE_DIR_OUT,  //0->16
}BR_FADE_DIR;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�t�F�[�h�V�X�e��
//=====================================
typedef struct _BR_FADE_WORK BR_FADE_WORK;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern BR_FADE_WORK * BR_FADE_Init( HEAPID heapID );
extern void BR_FADE_Exit( BR_FADE_WORK *p_wk );
extern void BR_FADE_Main( BR_FADE_WORK *p_wk );

extern void BR_FADE_StartFade( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR  dir );
extern void BR_FADE_StartFadeEx( BR_FADE_WORK *p_wk, BR_FADE_TYPE type, BR_FADE_DISPLAY disp, BR_FADE_DIR dir, u32 sync );
extern BOOL BR_FADE_IsEnd( const BR_FADE_WORK *cp_wk );
