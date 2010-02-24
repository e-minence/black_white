//==============================================================================
/**
 * @file    monolith_common.h
 * @brief   ���m���X��ʋ��ʃw�b�_
 * @author  matsuda
 * @date    2009.11.21(�y)
 */
//==============================================================================
#pragma once

#include "field/monolith_main.h"

#include "print\printsys.h"
#include "print\gf_font.h"
#include "print\wordset.h"
#include "system/palanm.h"
#include "system/actor_tool.h"
#include "system/bmp_oam.h"
#include "mission_types.h"
#include "app/app_printsys_common.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���m���X���C�����j���[Index
typedef enum{
  MONOLITH_MENU_TITLE,      ///<�^�C�g�����
  MONOLITH_MENU_MISSION,    ///<�~�b�V�������󂯂�
  MONOLITH_MENU_STATUS,     ///<��Ԃ�����
  MONOLITH_MENU_POWER,      ///<�s�v�c�ȃp���[�����炤
  
  MONOLITH_MENU_END,        ///<���m���X��ʏI��
}MONOLITH_MENU;

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

///�����F�̃f�t�H���g�ݒ�
#define MONOLITH_FONT_DEFCOLOR_LETTER   (0xf)
#define MONOLITH_FONT_DEFCOLOR_SHADOW   (2)
#define MONOLITH_FONT_DEFCOLOR_BACK     (0)

///���ʑf��Index�ւ̃A�N�Z�X
typedef enum{
  COMMON_RESOURCE_INDEX_UP,     ///<���ʗp���ʑf��INDEX�ւ̃A�N�Z�X
  COMMON_RESOURCE_INDEX_DOWN,   ///<����ʗp���ʑf��INDEX�ւ̃A�N�Z�X
  
  COMMON_RESOURCE_INDEX_MAX,
}COMMON_RESOURCE_INDEX;

///���ʑf�ރZ���̃A�j���V�[�P���X�ԍ�
typedef enum{
  COMMON_ANMSEQ_PANEL_LARGE,
  COMMON_ANMSEQ_PANEL_SMALL,
  COMMON_ANMSEQ_UP,
  COMMON_ANMSEQ_DOWN,
  COMMON_ANMSEQ_BLACK_TOWN,
  COMMON_ANMSEQ_WHITE_TOWN,
  COMMON_ANMSEQ_RETURN,
  COMMON_ANMSEQ_SCROLL,
  COMMON_ANMSEQ_PANEL_DECIDE,
  COMMON_ANMSEQ_ARROW_UP,
  COMMON_ANMSEQ_ARROW_DOWN,
}COMMON_ANMSEQ;

///���ʑf�ރp���b�g�̊��蓖��(OBJ)
typedef enum{
  COMMON_PAL_PANEL = 0,       ///<�p���b�g(���I��)
  COMMON_PAL_PANEL_FOCUS,     ///<�p���b�g(�I������Ă���)
  
  COMMON_PAL_TOWN = 0,        ///<�X�A�C�R��
  
  COMMON_PAL_CURSOR = 0,      ///<�㉺�J�[�\��(���I��)
  COMMON_PAL_CURSOR_FOCUS = 2,///<�㉺�J�[�\��(�I������Ă���)
  
  COMMON_PAL_RETURN = 0,      ///<�߂�A�C�R��
}COMMON_PAL;

///���ʑf�ރp���b�g�̊��蓖��(BG)
typedef enum{
  COMMON_PALBG_PANEL = 0,           ///<�p���b�g(���I��)
  COMMON_PALBG_PANEL_FOCUS = 3,     ///<�p���b�g(�I������Ă���)
}COMMON_PALBG;

///�p�l���J���[���[�h
typedef enum{
  PANEL_COLORMODE_NONE,       ///<�J���[�A�j������
  PANEL_COLORMODE_FOCUS,      ///<�t�H�[�J�X���̃J���[�A�j��
  PANEL_COLORMODE_FLASH,      ///<���蒆�̃t���b�V���A�j��
}PANEL_COLORMODE;

///�~�b�V���������{���̎��ɊX�ԍ��̈�ɃZ�b�g�����
#define SELECT_TOWN_ENFORCEMENT   (INTRUDE_TOWN_MAX)


///���m���XVINTR TCB�̃v���C�I���e�B
enum{
  MONOLITH_VINTR_TCB_PRI_POWER = 3,     ///<�p���[���
  MONOLITH_VINTR_TCB_PRI_MAIN = 10,     ///<���m���X��ʑS�̂Ŏg�p���Ă���VINTR TCB
};

///�p�l���R���g���[����
#define PANEL_CONTROL_MAX     (4)

///G�p���[�K����
typedef enum{
  MONO_USE_POWER_OK,        ///<�K�����Ă���
  MONO_USE_POWER_SOMEMORE,  ///<���Ə����ŏK��
  MONO_USE_POWER_NONE,      ///<�K�����Ă��Ȃ�
  
  MONO_USE_POWER_MAX,
}MONO_USE_POWER;


//==============================================================================
//  �\���̒�`
//==============================================================================
///���m���X�S��ʋ��ʐݒ�f�[�^
typedef struct{
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
	ARCHANDLE *hdl;
  POWER_CONV_DATA *powerdata;
  APP_PRINTSYS_COMMON_WORK app_printsys;
  
	struct{
    u32 pltt_index;          ///<�A�N�^�[���ʑf�ރp���b�gIndex
    u32 char_index;          ///<�A�N�^�[���ʑf�ރL����Index
    u32 cell_index;          ///<�A�N�^�[���ʑf�ރZ��Index
    u32 pltt_bmpfont_index;  ///<BMP�t�H���g�p���b�gIndex
  }common_res[COMMON_RESOURCE_INDEX_MAX];
}MONOLITH_SETUP;

///���m���X�S��ʋ��p���[�N(�㉺PROC�ԁA��ʂ��܂��������[�N�̂����Ɏg�p)
typedef struct{
  u8 mission_select_town;    ///<�~�b�V������ʁF�I�����Ă���X�ԍ� or SELECT_TOWN_ENFORCEMENT
  u8 power_select_no;        ///<�p���[��ʁF�I�����Ă���p���[�ԍ�
  u8 power_mono_use;         ///<�p���[��ʁFpower_select_no��MONO_USE_POWER���
  u8 power_eqp_update;       ///<�p���[��ʁFTRUE:�������Ă���p���[�ɕύX��������
  u8 padding;
}MONOLITH_COMMON_WORK;

///�p�l���J���[�A�j������\����
typedef struct{
  u16 evy;            ///<EVY�l(����8�r�b�g����)
  u8 mode;            ///<PANEL_COLORMODE_???
  //�t�F�[�h�Ŏg�p
  u8 add_dir;         ///<EVY���Z����
  //�t���b�V���Ŏg�p
  u8 wait;            ///<�E�F�C�g
  u8 count;           ///<�t���b�V���񐔂��J�E���g
  u8 padding[2];
}PANEL_COLOR_CONTROL;

///���L�c�[���Ŏg�p���郏�[�N
typedef struct{
  PANEL_COLOR_CONTROL panel_color[PANEL_CONTROL_MAX];
}MONOLITH_TOOL_WORK;

///���m���XAPP PROC�pParentWork
typedef struct{
  MONOLITH_PARENT_WORK *parent;     ///<�ePROC��ParentWork
  MONOLITH_SETUP *setup;            ///<���m���X�S��ʋ��ʐݒ�f�[�^�ւ̃|�C���^
  MONOLITH_COMMON_WORK *common;     ///<���m���X�S��ʋ��p���[�N�ւ̃|�C���^
  MONOLITH_TOOL_WORK tool;          ///<���L�c�[���Ŏg�p���郏�[�N
  u8 next_menu_index;     ///<APP Proc�I�����Ɏ��̉�ʂ̃��j���[Index���Z�b�g����
  u8 up_proc_finish;      ///<TRUE:����PROC�I�����N�G�X�g
  u8 force_finish;        ///<TRUE:���m���X���L�҂����Ȃ��Ȃ����ׁA�����I��
  u8 padding;
}MONOLITH_APP_PARENT;



//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const GFL_PROC_DATA MonolithAppProc_Up_PalaceMap;
extern const GFL_PROC_DATA MonolithAppProc_Down_Title;
extern const GFL_PROC_DATA MonolithAppProc_Down_MissionSelect;
extern const GFL_PROC_DATA MonolithAppProc_Up_MissionExplain;
extern const GFL_PROC_DATA MonolithAppProc_Down_Status;
extern const GFL_PROC_DATA MonolithAppProc_Down_PowerSelect;
extern const GFL_PROC_DATA MonolithAppProc_Up_PowerExplain;
