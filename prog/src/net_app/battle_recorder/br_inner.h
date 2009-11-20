//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_inner.h
 *	@brief	�o�g�����R�[�_�[	�v���C�x�[�g�w�b�_
 *	@author	Toru=Nagihashi
 *	@date		2009.11.11
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
///	BG�ʐݒ�
//=====================================
enum 
{
  BG_FRAME_M_TEXT = GFL_BG_FRAME0_M,
	BG_FRAME_M_FONT	= GFL_BG_FRAME1_M,
	BG_FRAME_M_WIN	= GFL_BG_FRAME2_M,
	BG_FRAME_M_BACK	= GFL_BG_FRAME3_M,
	BG_FRAME_S_FONT	= GFL_BG_FRAME0_S,
	BG_FRAME_S_WIN	= GFL_BG_FRAME1_S,
	BG_FRAME_S_BACK	= GFL_BG_FRAME2_S,
};

//-------------------------------------
///	PLT�ݒ�
//=====================================
enum 
{
	//BG
	PLT_BG_M_COMMON	= 0,
	PLT_BG_S_COMMON	= 0,
  PLT_BG_M_FONT	= 14,
  PLT_BG_S_FONT	= 14,

	//PLT
	PLT_OBJ_M_COMMON	= 0,
	PLT_OBJ_S_COMMON	= 0,
};

//-------------------------------------
///	�����F�iBMPWIN�t�H���g�j
//=====================================
#define BR_PRINT_COL_BOOT	  	( PRINTSYS_LSB_Make(  1, 14,  0 ) )	// �N�����̕����F
#define BR_PRINT_COL_NORMAL 	( PRINTSYS_LSB_Make( 15, 13,  0 ) )	// �悭�g�������F
#define	BR_PRINT_COL_BLACK		( PRINTSYS_LSB_Make( 14, 13,  0 ) )	// �t�H���g�J���[
#define	BR_PRINT_COL_INFO		  ( PRINTSYS_LSB_Make( 15, 13, 12 ) )	// �t�H���g�J���[
#define	BR_PRINT_COL_PHOTO		( BR_PRINT_COL_NORMAL )
#define	BR_PRINT_COL_BOX_SHOT	( BR_PRINT_COL_NORMAL )
#define	BR_PRINT_COL_BOX_NAME	( PRINTSYS_LSB_Make( 15,  1,  0 ) )
#define	BR_PRINT_COL_FREC		  ( BR_PRINT_COL_NORMAL )
#define	BR_PRINT_COL_PROF		  ( BR_PRINT_COL_NORMAL )


//-------------------------------------
///�퓬�^��f�[�^
//=====================================
typedef struct 
{
	BOOL						is_valid;
	void						*p_btlrec;
} BR_BTLREC_DATA;
typedef struct 
{
	BR_BTLREC_DATA	my;
	BR_BTLREC_DATA	other[3];
} BR_BTLREC_SET;

