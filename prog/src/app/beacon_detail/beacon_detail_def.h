/*
 *  @file beacon_detail_def.h
 *  @brief  ����Ⴂ�r�[�R���ڍ׉�ʃ��e������`
 *  @author Miyuki Iwasawa
 *  @date   10.02.02
 */

#pragma once

#define  BEACON_DETAIL_HEAP_SIZE  (0x30000)  ///< �q�[�v�T�C�Y

enum{
 SEQ_FADEIN,
 SEQ_MAIN,
 SEQ_FADEOUT,
 SEQ_EXIT,
};


//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME0_M,
	BG_FRAME_MSG_M	= GFL_BG_FRAME1_M,
	BG_FRAME_MAP01_M	= GFL_BG_FRAME2_M,
	BG_FRAME_MAP02_M	= GFL_BG_FRAME3_M,
	BG_FRAME_DAT01_S	= GFL_BG_FRAME0_S,
	BG_FRAME_WIN01_S	= GFL_BG_FRAME1_S,
	BG_FRAME_DAT02_S	= GFL_BG_FRAME2_S,
	BG_FRAME_WIN02_S	= GFL_BG_FRAME3_S,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_POKE_M				= 1,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_LOCALIZE_M		= 15, //���[�J���C�Y�p

	//�T�uBG
	PLTID_BG_BACK_S				=	0,
	PLTID_BG_LOCALIZE_S		= 15, //���[�J���C�Y�p

	//���C��OBJ
	PLTID_OBJ_TOUCHBAR_M	= 0, // 3�{�g�p
	PLTID_OBJ_LOCALIZE_M	= 14,		//���[�J���C�Y�p
  PLTID_OBJ_WMI_M = 15,	  //�ʐM�A�C�R��

  //�T�uOBJ
  PLTID_OBJ_PMS_DRAW = 0, // 5�{�g�p
	PLTID_OBJ_LOCALIZE_S	= 15,		//���[�J���C�Y�p
};


