//==============================================================================
/**
 * @file    monolith_common.h
 * @brief   ���m���X��ʋ��ʃw�b�_
 * @author  matsuda
 * @date    2009.11.21(�y)
 */
//==============================================================================
#pragma once

#include "print\printsys.h"
#include "print\gf_font.h"
#include "print\wordset.h"
#include "system/palanm.h"
#include "system/actor_tool.h"
#include "system/bmp_oam.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���ʑf�ޓW�J�A�h���X
enum{
  MONO_BG_COMMON_CHARBASE = GX_BG_CHARBASE_0x04000,   ///<���ʑf��BG�L�����N�^�x�[�X
  MONO_BG_COMMON_CHAR_SIZE = 0x8000,                  ///<���ʑf�ރL�����N�^�T�C�Y
  
  MONO_BG_COMMON_SCRBASE = GX_BG_SCRBASE_0x0000,      ///<���ʑf��BG�X�N���[���x�[�X
  MONO_BG_COMMON_SCR_SIZE = 0x800,                    ///<���ʑf��BG�X�N���[���T�C�Y
};

///���ʃt�H���g�p���b�g�ԍ�
#define MONOLITH_BG_UP_FONT_PALNO   (13)
///����ʃt�H���g�p���b�g�ԍ�
#define MONOLITH_BG_DOWN_FONT_PALNO   (MONOLITH_BG_UP_FONT_PALNO)

///���ʑf��Index�ւ̃A�N�Z�X
typedef enum{
  COMMON_RESOURCE_INDEX_UP,     ///<���ʗp���ʑf��INDEX�ւ̃A�N�Z�X
  COMMON_RESOURCE_INDEX_DOWN,   ///<����ʗp���ʑf��INDEX�ւ̃A�N�Z�X
  
  COMMON_RESOURCE_INDEX_MAX,
}COMMON_RESOURCE_INDEX;

//==============================================================================
//  �\���̒�`
//==============================================================================
///���m���X�S��ʋ��ʐݒ�f�[�^
typedef struct{
  GAMESYS_WORK *gsys;
  PALETTE_FADE_PTR pfd;
  PLTTSLOT_SYS_PTR plttslot;
  PRINT_QUE *printQue;
  GFL_FONT *font_handle;
  WORDSET *wordset;
	GFL_CLUNIT *clunit;
	GFL_TCBLSYS *tcbl_sys;
	GFL_MSGDATA *mm_mission_mono;   ///<mision_monolith.gmm
	GFL_MSGDATA *mm_power;          ///<power.gmm
	GFL_MSGDATA *mm_power_explain;  ///<power_explain.gmm
	GFL_MSGDATA *mm_monolith;       ///<monolith.gmm
	BMPOAM_SYS_PTR bmpoam_sys;
	
	struct{
    u32 pltt_index;          ///<�A�N�^�[���ʑf�ރp���b�gIndex
    u32 char_index;          ///<�A�N�^�[���ʑf�ރL����Index
    u32 cell_index;          ///<�A�N�^�[���ʑf�ރZ��Index
    u32 pltt_bmpfont_index;  ///<BMP�t�H���g�p���b�gIndex
  }common_res[COMMON_RESOURCE_INDEX_MAX];
}MONOLITH_SETUP;

///���m���X�S��ʋ��p���[�N(�㉺PROC�ԁA��ʂ��܂��������[�N�̂����Ɏg�p)
typedef struct{
  u8 mission_select_town;           ///<�~�b�V������ʁF�I�����Ă���X�ԍ�
  u8 power_select_no;               ///<�p���[��ʁF�I�����Ă���p���[�ԍ�
  u8 padding[2];
}MONOLITH_COMMON_WORK;

///���m���XAPP PROC�pParentWork
typedef struct{
  MONOLITH_SETUP *setup;            ///<���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
  MONOLITH_COMMON_WORK *common;     ///<���m���X�S��ʋ��p���[�N�ւ̃|�C���^
  u8 next_menu_index;     ///<APP Proc�I�����Ɏ��̉�ʂ̃��j���[Index���Z�b�g����
  u8 padding[3];
}MONOLITH_APP_PARENT;



//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const GFL_PROC_DATA MonolithAppProc_Down_MissionSelect;
