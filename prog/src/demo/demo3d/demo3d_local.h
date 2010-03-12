/*
 *  @file   demo3d_local.h
 *  @brief  demo3d���ʊ֘A�t�@�C�����L�w�b�_
 *
 *  @author Miyuki Iwasawa
 *  @date   10.02.25
 */

#pragma once

#include "demo/demo3d.h"
#include "demo/demo3d_demoid.h"

///�V�[���Ăяo���p�����[�^�i�[�\����
typedef struct _DEMO3D_SCENE_ENV{
  DEMO3D_ID demo_id;
  u32       start_frame;

  u8        scene_id;
  u8        time_zone;
  u8        player_sex;
}DEMO3D_SCENE_ENV;

// �L�[���������}�N��
#define CHECK_KEY_TRG( key ) ( (GFL_UI_KEY_GetTrg() & (key) ) == (key) )
#define CHECK_KEY_CONT( key ) ( (GFL_UI_KEY_GetCont() & (key) ) == (key) )

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_POKE_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME3_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
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
	PLTID_BG_INFOWIN_M		= 15,
	//�T�uBG
	PLTID_BG_BACK_S				=	0,

	//���C��OBJ
	PLTID_OBJ_TOUCHBAR_M	= 0, // 3�{�g�p
	PLTID_OBJ_TOWNMAP_M	= 14,		

  //�T�uOBJ
  PLTID_OBJ_COMMON_S = 0,
};


