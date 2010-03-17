//============================================================================================
/**
 * @file  b_plist_bmp.c
 * @brief �퓬�p�|�P�������X�g���BMP�֘A
 * @author  Hiroyuki Nakamura
 * @date  05.02.01
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "item/item.h"
#include "gamesystem/msgspeed.h"
#include "waza_tool/wazadata.h"
#include "waza_tool/wazano_def.h"
#include "system/bmp_winframe.h"
#include "poke_tool/gauge_tool.h"
#include "app/app_menu_common.h"

#include "msg/msg_b_plist.h"

#include "b_plist.h"
#include "b_plist_bmp_def.h"
#include "b_plist_main.h"
#include "b_plist_bmp.h"


//============================================================================================
//  �萔��`
//============================================================================================
/*
#define PCOL_N_BLACK  ( PRINTSYS_LSB_Make( 1, 2, 0 ) )    // �t�H���g�J���[�F��
#define PCOL_N_WHITE  ( PRINTSYS_LSB_Make( 15, 14, 0 ) )    // �t�H���g�J���[�F��
#define PCOL_N_BLUE   ( PRINTSYS_LSB_Make( 7, 8, 0 ) )    // �t�H���g�J���[�F��
#define PCOL_N_RED    ( PRINTSYS_LSB_Make( 3, 4, 0 ) )    // �t�H���g�J���[�F��

#define PCOL_BTN    ( PRINTSYS_LSB_Make( 7, 8, 9 ) )    // �t�H���g�J���[�F�{�^��
#define PCOL_B_WHITE  ( PRINTSYS_LSB_Make( 15, 14, 0 ) )    // �t�H���g�J���[�F�{�^���p��
#define PCOL_B_BLUE   ( PRINTSYS_LSB_Make( 10, 11, 0 ) )    // �t�H���g�J���[�F�{�^���p��
#define PCOL_B_RED    ( PRINTSYS_LSB_Make( 12, 13, 0 ) )    // �t�H���g�J���[�F�{�^���p��
*/

#define FCOL_P09WN    ( PRINTSYS_LSB_Make(15,14,0) )    // �t�H���g�J���[�F�p���b�g�X����
#define FCOL_P09BLN   ( PRINTSYS_LSB_Make(10,11,0) )    // �t�H���g�J���[�F�p���b�g�X��
#define FCOL_P09RN    ( PRINTSYS_LSB_Make(12,13,0) )    // �t�H���g�J���[�F�p���b�g�X�Ԕ�
/*
#define FCOL_P13BKN   ( PRINTSYS_LSB_Make(1,2,0) )      // �t�H���g�J���[�F�p���b�g�P�R����
#define FCOL_P13WN    ( PRINTSYS_LSB_Make(15,2,0) )     // �t�H���g�J���[�F�p���b�g�P�R����
#define FCOL_P13BLN   ( PRINTSYS_LSB_Make(6,5,0) )      // �t�H���g�J���[�F�p���b�g�P�R��
#define FCOL_P13RN    ( PRINTSYS_LSB_Make(4,3,0) )      // �t�H���g�J���[�F�p���b�g�P�R�Ԕ�
*/
#define FCOL_P13BKN   ( PRINTSYS_LSB_Make(8,9,0) )      // �t�H���g�J���[�F�p���b�g�P�R����
#define FCOL_P13WN    ( PRINTSYS_LSB_Make(15,14,0) )    // �t�H���g�J���[�F�p���b�g�P�R����
#define FCOL_P13BLN   ( PRINTSYS_LSB_Make(10,11,0) )    // �t�H���g�J���[�F�p���b�g�P�R��
#define FCOL_P13RN    ( PRINTSYS_LSB_Make(12,13,0) )    // �t�H���g�J���[�F�p���b�g�P�R�Ԕ�

#define FCOL_P13TALK  ( PRINTSYS_LSB_Make(1,2,0) )

/*
#define HP_GAGE_COL_G1  ( 1 ) // HP�Q�[�W�J���[�΂P
#define HP_GAGE_COL_G2  ( 2 ) // HP�Q�[�W�J���[�΂Q
#define HP_GAGE_COL_Y1  ( 3 ) // HP�Q�[�W�J���[���P
#define HP_GAGE_COL_Y2  ( 4 ) // HP�Q�[�W�J���[���Q
#define HP_GAGE_COL_R1  ( 5 ) // HP�Q�[�W�J���[�ԂP
#define HP_GAGE_COL_R2  ( 6 ) // HP�Q�[�W�J���[�ԂQ
*/

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void BPL_Page1BmpWrite( BPLIST_WORK * wk );
static void BPL_ChgPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StMainPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StWazaSelPageBmpWrite( BPLIST_WORK * wk );
static void BPL_StWazaInfoPageBmpWrite( BPLIST_WORK * wk );
static void BPL_PPRcvPageBmpWrite( BPLIST_WORK * wk );
static void BPL_WazaDelSelPageBmpWrite( BPLIST_WORK * wk );
static void BPL_Page6BmpWrite( BPLIST_WORK * wk );
//static void BPL_Page8BmpWrite( BPLIST_WORK * wk );

static void BPL_PokeSelStrPut( BPLIST_WORK * wk, u32 midx );


static void AddDummyBmpWin( BPLIST_WORK * wk );
static void DelDummyBmpWin( BPLIST_WORK * wk );


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================
// ���ʃf�t�H���g�E�B���h�E�f�[�^
static const u8 CommBmpData[][6] =
{
  { // �R�����g�\��
    GFL_BG_FRAME0_S, WIN_P1_COMMENT_PX, WIN_P1_COMMENT_PY,
    WIN_P1_COMMENT_SX, WIN_P1_COMMENT_SY, WIN_P1_COMMENT_PAL
  },
  { // ���b�Z�[�W�\��
    GFL_BG_FRAME0_S, WIN_TALK_PX, WIN_TALK_PY,
    WIN_TALK_SX, WIN_TALK_SY, WIN_TALK_PAL
  }
};

// �y�[�W�P��BMP�E�B���h�E�f�[�^
static const u8 Page1_BmpData[][6] =
{
  { // �|�P�����P
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE1_PX, BPL_COMM_WIN_P1_POKE1_PY,
    BPL_COMM_WIN_P1_POKE1_SX, BPL_COMM_WIN_P1_POKE1_SY, BPL_COMM_WIN_P1_POKE1_PAL
  },
  { // �|�P�����Q
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE2_PX, BPL_COMM_WIN_P1_POKE2_PY,
    BPL_COMM_WIN_P1_POKE2_SX, BPL_COMM_WIN_P1_POKE2_SY, BPL_COMM_WIN_P1_POKE2_PAL
  },
  { // �|�P�����R
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE3_PX, BPL_COMM_WIN_P1_POKE3_PY,
    BPL_COMM_WIN_P1_POKE3_SX, BPL_COMM_WIN_P1_POKE3_SY, BPL_COMM_WIN_P1_POKE3_PAL
  },
  { // �|�P�����S
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE4_PX, BPL_COMM_WIN_P1_POKE4_PY,
    BPL_COMM_WIN_P1_POKE4_SX, BPL_COMM_WIN_P1_POKE4_SY, BPL_COMM_WIN_P1_POKE4_PAL
  },
  { // �|�P�����T
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE5_PX, BPL_COMM_WIN_P1_POKE5_PY,
    BPL_COMM_WIN_P1_POKE5_SX, BPL_COMM_WIN_P1_POKE5_SY, BPL_COMM_WIN_P1_POKE5_PAL
  },
  { // �|�P�����U
    GFL_BG_FRAME1_S, BPL_COMM_WIN_P1_POKE6_PX, BPL_COMM_WIN_P1_POKE6_PY,
    BPL_COMM_WIN_P1_POKE6_SX, BPL_COMM_WIN_P1_POKE6_SY, BPL_COMM_WIN_P1_POKE6_PAL
  },
};

// ����ւ��y�[�W��BMP�E�B���h�E�f�[�^
static const u8 ChgPage_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_CHG_NAME_PX, WIN_CHG_NAME_PY,
    WIN_CHG_NAME_SX, WIN_CHG_NAME_SY, WIN_CHG_NAME_PAL
  },
  { // �u���ꂩ����v
    GFL_BG_FRAME1_S, WIN_CHG_IREKAE_PX, WIN_CHG_IREKAE_PY,
    WIN_CHG_IREKAE_SX, WIN_CHG_IREKAE_SY, WIN_CHG_IREKAE_PAL
  },
  { // �u�悳���݂�v
    GFL_BG_FRAME1_S, WIN_CHG_STATUS_PX, WIN_CHG_STATUS_PY,
    WIN_CHG_STATUS_SX, WIN_CHG_STATUS_SY, WIN_CHG_STATUS_PAL
  },
  { // �u�킴���݂�v
    GFL_BG_FRAME1_S, WIN_CHG_WAZACHECK_PX, WIN_CHG_WAZACHECK_PY,
    WIN_CHG_WAZACHECK_SX, WIN_CHG_WAZACHECK_SY, WIN_CHG_WAZACHECK_PAL
  }
};

// �X�e�[�^�X�Z�I���y�[�W��BMP�E�B���h�E�f�[�^
static const u8 StWazaSelPage_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
    WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
  },
  { // �Z�P
    GFL_BG_FRAME1_S, WIN_STW_SKILL1_PX, WIN_STW_SKILL1_PY,
    WIN_STW_SKILL1_SX, WIN_STW_SKILL1_SY, WIN_STW_SKILL1_PAL
  },
  { // �Z�Q
    GFL_BG_FRAME1_S, WIN_STW_SKILL2_PX, WIN_STW_SKILL2_PY,
    WIN_STW_SKILL2_SX, WIN_STW_SKILL2_SY, WIN_STW_SKILL2_PAL
  },
  { // �Z�R
    GFL_BG_FRAME1_S, WIN_STW_SKILL3_PX, WIN_STW_SKILL3_PY,
    WIN_STW_SKILL3_SX, WIN_STW_SKILL3_SY, WIN_STW_SKILL3_PAL
  },
  { // �Z�S
    GFL_BG_FRAME1_S, WIN_STW_SKILL4_PX, WIN_STW_SKILL4_PY,
    WIN_STW_SKILL4_SX, WIN_STW_SKILL4_SY, WIN_STW_SKILL4_PAL
  },

  { // �u�悳���݂�v
    GFL_BG_FRAME1_S, WIN_STW_STATUS_PX, WIN_STW_STATUS_PY,
    WIN_STW_STATUS_SX, WIN_STW_STATUS_SY, WIN_STW_STATUS_PAL
  },

/*
  { // ���O�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
    WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
  },
  { // �Z�P�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STW_SKILL1_PX, WIN_STW_SKILL1_PY,
    WIN_STW_SKILL1_SX, WIN_STW_SKILL1_SY, WIN_STW_SKILL1_PAL
  },
  { // �Z�Q�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STW_SKILL2_PX, WIN_STW_SKILL2_PY,
    WIN_STW_SKILL2_SX, WIN_STW_SKILL2_SY, WIN_STW_SKILL2_PAL
  },
  { // �Z�R�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STW_SKILL3_PX, WIN_STW_SKILL3_PY,
    WIN_STW_SKILL3_SX, WIN_STW_SKILL3_SY, WIN_STW_SKILL3_PAL
  },
  { // �Z�S�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STW_SKILL4_PX, WIN_STW_SKILL4_PY,
    WIN_STW_SKILL4_SX, WIN_STW_SKILL4_SY, WIN_STW_SKILL4_PAL
  },
*/
};

// �X�e�[�^�X���C���y�[�W��BMP�E�B���h�E�f�[�^
static const u8 StMainPage_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
    WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
  },
  { // ������
    GFL_BG_FRAME1_S, WIN_STM_SPANAME_PX, WIN_STM_SPANAME_PY,
    WIN_STM_SPANAME_SX, WIN_STM_SPANAME_SY, WIN_STM_SPANAME_PAL
  },
  { // ��������
    GFL_BG_FRAME1_S, WIN_STM_SPAINFO_PX, WIN_STM_SPAINFO_PY,
    WIN_STM_SPAINFO_SX, WIN_STM_SPAINFO_SY, WIN_STM_SPAINFO_PAL
  },
  { // ���
    GFL_BG_FRAME1_S, WIN_STM_ITEMNAME_PX, WIN_STM_ITEMNAME_PY,
    WIN_STM_ITEMNAME_SX, WIN_STM_ITEMNAME_SY, WIN_STM_ITEMNAME_PAL
  },
  { // HP�l
    GFL_BG_FRAME1_S, WIN_STM_HPNUM_PX, WIN_STM_HPNUM_PY,
    WIN_STM_HPNUM_SX, WIN_STM_HPNUM_SY, WIN_STM_HPNUM_PAL
  },
  { // �U���l
    GFL_BG_FRAME1_S, WIN_STM_POWNUM_PX, WIN_STM_POWNUM_PY,
    WIN_STM_POWNUM_SX, WIN_STM_POWNUM_SY, WIN_STM_POWNUM_PAL
  },
  { // �h��l
    GFL_BG_FRAME1_S, WIN_STM_DEFNUM_PX, WIN_STM_DEFNUM_PY,
    WIN_STM_DEFNUM_SX, WIN_STM_DEFNUM_SY, WIN_STM_DEFNUM_PAL
  },
  { // �f�����l
    GFL_BG_FRAME1_S, WIN_STM_AGINUM_PX, WIN_STM_AGINUM_PY,
    WIN_STM_AGINUM_SX, WIN_STM_AGINUM_SY, WIN_STM_AGINUM_PAL
  },
  { // ���U�l
    GFL_BG_FRAME1_S, WIN_STM_SPPNUM_PX, WIN_STM_SPPNUM_PY,
    WIN_STM_SPPNUM_SX, WIN_STM_SPPNUM_SY, WIN_STM_SPPNUM_PAL
  },
  { // ���h�l
    GFL_BG_FRAME1_S, WIN_STM_SPDNUM_PX, WIN_STM_SPDNUM_PY,
    WIN_STM_SPDNUM_SX, WIN_STM_SPDNUM_SY, WIN_STM_SPDNUM_PAL
  },
  { // HP�Q�[�W
    GFL_BG_FRAME1_S, WIN_STM_HPGAGE_PX, WIN_STM_HPGAGE_PY,
    WIN_STM_HPGAGE_SX, WIN_STM_HPGAGE_SY, WIN_STM_HPGAGE_PAL
  },
  { // ���x���l
    GFL_BG_FRAME1_S, WIN_STM_LVNUM_PX, WIN_STM_LVNUM_PY,
    WIN_STM_LVNUM_SX, WIN_STM_LVNUM_SY, WIN_STM_LVNUM_PAL
  },
  { // ���̃��x���l
    GFL_BG_FRAME1_S, WIN_STM_NEXTNUM_PX, WIN_STM_NEXTNUM_PY,
    WIN_STM_NEXTNUM_SX, WIN_STM_NEXTNUM_SY, WIN_STM_NEXTNUM_PAL
  },

  { // �uHP�v
    GFL_BG_FRAME1_S, WIN_STM_HP_PX, WIN_STM_HP_PY,
    WIN_STM_HP_SX, WIN_STM_HP_SY, WIN_STM_HP_PAL
  },
  { // �u���������v
    GFL_BG_FRAME1_S, WIN_STM_POW_PX, WIN_STM_POW_PY,
    WIN_STM_POW_SX, WIN_STM_POW_SY, WIN_STM_POW_PAL
  },
  { // �u�ڂ�����v
    GFL_BG_FRAME1_S, WIN_STM_DEF_PX, WIN_STM_DEF_PY,
    WIN_STM_DEF_SX, WIN_STM_DEF_SY, WIN_STM_DEF_PAL
  },
  { // �u���΂₳�v
    GFL_BG_FRAME1_S, WIN_STM_AGI_PX, WIN_STM_AGI_PY,
    WIN_STM_AGI_SX, WIN_STM_AGI_SY, WIN_STM_AGI_PAL
  },
  { // �u�Ƃ������v
    GFL_BG_FRAME1_S, WIN_STM_SPP_PX, WIN_STM_SPP_PY,
    WIN_STM_SPP_SX, WIN_STM_SPP_SY, WIN_STM_SPP_PAL
  },
  { // �u�Ƃ��ڂ��v
    GFL_BG_FRAME1_S, WIN_STM_SPD_PX, WIN_STM_SPD_PY,
    WIN_STM_SPD_SX, WIN_STM_SPD_SY, WIN_STM_SPD_PAL
  },
  { // �uLv.�v
    GFL_BG_FRAME1_S, WIN_STM_LV_PX, WIN_STM_LV_PY,
    WIN_STM_LV_SX, WIN_STM_LV_SY, WIN_STM_LV_PAL
  },
  { // �u���̃��x���܂Łv
    GFL_BG_FRAME1_S, WIN_STM_NEXT_PX, WIN_STM_NEXT_PY,
    WIN_STM_NEXT_SX, WIN_STM_NEXT_SY, WIN_STM_NEXT_PAL
  },
  { // �u�킴���݂�v
    GFL_BG_FRAME1_S, WIN_STM_WAZACHECK_PX, WIN_STM_WAZACHECK_PY,
    WIN_STM_WAZACHECK_SX, WIN_STM_WAZACHECK_SY, WIN_STM_WAZACHECK_PAL
  },
/*
  { // ���O�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_ST_NAME_PX, WIN_ST_NAME_PY,
    WIN_ST_NAME_SX, WIN_ST_NAME_SY, WIN_ST_NAME_PAL
  },
  { // �������i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_SPANAME_PX, WIN_STM_SPANAME_PY,
    WIN_STM_SPANAME_SX, WIN_STM_SPANAME_SY, WIN_STM_SPANAME_PAL
  },
  { // ���������i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_SPAINFO_PX, WIN_STM_SPAINFO_PY,
    WIN_STM_SPAINFO_SX, WIN_STM_SPAINFO_SY, WIN_STM_SPAINFO_PAL
  },
  { // ����i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_ITEMNAME_PX, WIN_STM_ITEMNAME_PY,
    WIN_STM_ITEMNAME_SX, WIN_STM_ITEMNAME_SY, WIN_STM_ITEMNAME_PAL
  },
  { // HP�l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_HPNUM_PX, WIN_STM_HPNUM_PY,
    WIN_STM_HPNUM_SX, WIN_STM_HPNUM_SY, WIN_STM_HPNUM_PAL
  },
  { // �U���l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_POWNUM_PX, WIN_STM_POWNUM_PY,
    WIN_STM_POWNUM_SX, WIN_STM_POWNUM_SY, WIN_STM_POWNUM_PAL
  },
  { // �h��l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_DEFNUM_PX, WIN_STM_DEFNUM_PY,
    WIN_STM_DEFNUM_SX, WIN_STM_DEFNUM_SY, WIN_STM_DEFNUM_PAL
  },
  { // �f�����l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_AGINUM_PX, WIN_STM_AGINUM_PY,
    WIN_STM_AGINUM_SX, WIN_STM_AGINUM_SY, WIN_STM_AGINUM_PAL
  },
  { // ���U�l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_SPPNUM_PX, WIN_STM_SPPNUM_PY,
    WIN_STM_SPPNUM_SX, WIN_STM_SPPNUM_SY, WIN_STM_SPPNUM_PAL
  },
  { // ���h�l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_SPDNUM_PX, WIN_STM_SPDNUM_PY,
    WIN_STM_SPDNUM_SX, WIN_STM_SPDNUM_SY, WIN_STM_SPDNUM_PAL
  },
  { // HP�Q�[�W�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_HPGAGE_PX, WIN_STM_HPGAGE_PY,
    WIN_STM_HPGAGE_SX, WIN_STM_HPGAGE_SY, WIN_STM_HPGAGE_PAL
  },
  { // ���x���l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_LVNUM_PX, WIN_STM_LVNUM_PY,
    WIN_STM_LVNUM_SX, WIN_STM_LVNUM_SY, WIN_STM_LVNUM_PAL
  },
  { // ���̃��x���l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_STM_NEXTNUM_PX, WIN_STM_NEXTNUM_PY,
    WIN_STM_NEXTNUM_SX, WIN_STM_NEXTNUM_SY, WIN_STM_NEXTNUM_PAL
  }
*/
};

// �X�e�[�^�X�Z�ڍ׃y�[�W��BMP�E�B���h�E�f�[�^
static const u8 StWazaInfoPage_BmpData[][6] =
{
  { // �Z��
    GFL_BG_FRAME1_S, WIN_P4_SKILL_PX, WIN_P4_SKILL_PY,
    WIN_P4_SKILL_SX, WIN_P4_SKILL_SY, WIN_P4_SKILL_PAL
  },
  { // PP/PP
    GFL_BG_FRAME1_S, WIN_P4_PPNUM_PX, WIN_P4_PPNUM_PY,
    WIN_P4_PPNUM_SX, WIN_P4_PPNUM_SY, WIN_P4_PPNUM_PAL
  },
  { // �����l
    GFL_BG_FRAME1_S, WIN_P4_HITNUM_PX, WIN_P4_HITNUM_PY,
    WIN_P4_HITNUM_SX, WIN_P4_HITNUM_SY, WIN_P4_HITNUM_PAL
  },
  { // �З͒l
    GFL_BG_FRAME1_S, WIN_P4_POWNUM_PX, WIN_P4_POWNUM_PY,
    WIN_P4_POWNUM_SX, WIN_P4_POWNUM_SY, WIN_P4_POWNUM_PAL
  },
  { // �Z����
    GFL_BG_FRAME1_S, WIN_P4_INFO_PX, WIN_P4_INFO_PY,
    WIN_P4_INFO_SX, WIN_P4_INFO_SY, WIN_P4_INFO_PAL
  },
  { // ���ޖ�
    GFL_BG_FRAME1_S, WIN_P4_BRNAME_PX, WIN_P4_BRNAME_PY,
    WIN_P4_BRNAME_SX, WIN_P4_BRNAME_SY, WIN_P4_BRNAME_PAL
  },

  { // ���O
    GFL_BG_FRAME1_S, WIN_P4_NAME_PX, WIN_P4_NAME_PY,
    WIN_P4_NAME_SX, WIN_P4_NAME_SY, WIN_P4_NAME_PAL
  },
  { // PP
    GFL_BG_FRAME1_S, WIN_P4_PP_PX, WIN_P4_PP_PY,
    WIN_P4_PP_SX, WIN_P4_PP_SY, WIN_P4_PP_PAL
  },
  { // �u�߂����イ�v
    GFL_BG_FRAME1_S, WIN_P4_HIT_PX, WIN_P4_HIT_PY,
    WIN_P4_HIT_SX, WIN_P4_HIT_SY, WIN_P4_HIT_PAL
  },
  { // �u����傭�v
    GFL_BG_FRAME1_S, WIN_P4_POW_PX, WIN_P4_POW_PY,
    WIN_P4_POW_SX, WIN_P4_POW_SY, WIN_P4_POW_PAL
  },
  { // �u�Ԃ�邢�v
    GFL_BG_FRAME1_S, WIN_P4_BUNRUI_PX, WIN_P4_BUNRUI_PY,
    WIN_P4_BUNRUI_SX, WIN_P4_BUNRUI_SY, WIN_P4_BUNRUI_PAL
  },
/*
  { // �Z���i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_SKILL_PX, WIN_P4_SKILL_PY,
    WIN_P4_SKILL_SX, WIN_P4_SKILL_SY, WIN_P4_SKILL_PAL
  },
  { // PP/PP�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_PPNUM_PX, WIN_P4_PPNUM_PY,
    WIN_P4_PPNUM_SX, WIN_P4_PPNUM_SY, WIN_P4_PPNUM_PAL
  },
  { // �����l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_HITNUM_PX, WIN_P4_HITNUM_PY,
    WIN_P4_HITNUM_SX, WIN_P4_HITNUM_SY, WIN_P4_HITNUM_PAL
  },
  { // �З͒l�i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_POWNUM_PX, WIN_P4_POWNUM_PY,
    WIN_P4_POWNUM_SX, WIN_P4_POWNUM_SY, WIN_P4_POWNUM_PAL
  },
  { // �Z�����i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_INFO_PX, WIN_P4_INFO_PY,
    WIN_P4_INFO_SX, WIN_P4_INFO_SY, WIN_P4_INFO_PAL
  },
  { // ���ޖ��i�X���b�v�j
    GFL_BG_FRAME1_S, WIN_P4_BRNAME_PX, WIN_P4_BRNAME_PY,
    WIN_P4_BRNAME_SX, WIN_P4_BRNAME_SY, WIN_P4_BRNAME_PAL
  },
*/
};

// �y�[�W�T��BMP�E�B���h�E�f�[�^
static const u8 Page5_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_P5_NAME_PX, WIN_P5_NAME_PY,
    WIN_P5_NAME_SX, WIN_P5_NAME_SY, WIN_P5_NAME_PAL
  },
  { // �Z�P
    GFL_BG_FRAME1_S, WIN_P5_SKILL1_PX, WIN_P5_SKILL1_PY,
    WIN_P5_SKILL1_SX, WIN_P5_SKILL1_SY, WIN_P5_SKILL1_PAL
  },
  { // �Z�Q
    GFL_BG_FRAME1_S, WIN_P5_SKILL2_PX, WIN_P5_SKILL2_PY,
    WIN_P5_SKILL2_SX, WIN_P5_SKILL2_SY, WIN_P5_SKILL2_PAL
  },
  { // �Z�R
    GFL_BG_FRAME1_S, WIN_P5_SKILL3_PX, WIN_P5_SKILL3_PY,
    WIN_P5_SKILL3_SX, WIN_P5_SKILL3_SY, WIN_P5_SKILL3_PAL
  },
  { // �Z�S
    GFL_BG_FRAME1_S, WIN_P5_SKILL4_PX, WIN_P5_SKILL4_PY,
    WIN_P5_SKILL4_SX, WIN_P5_SKILL4_SY, WIN_P5_SKILL4_PAL
  },
  { // �Z�T
    GFL_BG_FRAME1_S, WIN_P5_SKILL5_PX, WIN_P5_SKILL5_PY,
    WIN_P5_SKILL5_SX, WIN_P5_SKILL5_SY, WIN_P5_SKILL5_PAL
  },
};

// �y�[�W�U��BMP�E�B���h�E�f�[�^
static const u8 Page6_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_P6_NAME_PX, WIN_P6_NAME_PY,
    WIN_P6_NAME_SX, WIN_P6_NAME_SY, WIN_P6_NAME_PAL
  },
  { // �Z��
    GFL_BG_FRAME1_S, WIN_P6_SKILL_PX, WIN_P6_SKILL_PY,
    WIN_P6_SKILL_SX, WIN_P6_SKILL_SY, WIN_P6_SKILL_PAL
  },
  { // PP
    GFL_BG_FRAME1_S, WIN_P6_PP_PX, WIN_P6_PP_PY,
    WIN_P6_PP_SX, WIN_P6_PP_SY, WIN_P6_PP_PAL
  },
  { // PP/PP
    GFL_BG_FRAME1_S, WIN_P6_PPNUM_PX, WIN_P6_PPNUM_PY,
    WIN_P6_PPNUM_SX, WIN_P6_PPNUM_SY, WIN_P6_PPNUM_PAL
  },
  { // �u�߂����イ�v
    GFL_BG_FRAME1_S, WIN_P6_HIT_PX, WIN_P6_HIT_PY,
    WIN_P6_HIT_SX, WIN_P6_HIT_SY, WIN_P6_HIT_PAL
  },
  { // �u����傭�v
    GFL_BG_FRAME1_S, WIN_P6_POW_PX, WIN_P6_POW_PY,
    WIN_P6_POW_SX, WIN_P6_POW_SY, WIN_P6_POW_PAL
  },
  { // �����l
    GFL_BG_FRAME1_S, WIN_P6_HITNUM_PX, WIN_P6_HITNUM_PY,
    WIN_P6_HITNUM_SX, WIN_P6_HITNUM_SY, WIN_P6_HITNUM_PAL
  },
  { // �З͒l
    GFL_BG_FRAME1_S, WIN_P6_POWNUM_PX, WIN_P6_POWNUM_PY,
    WIN_P6_POWNUM_SX, WIN_P6_POWNUM_SY, WIN_P6_POWNUM_PAL
  },
  { // �Z����
    GFL_BG_FRAME1_S, WIN_P6_INFO_PX, WIN_P6_INFO_PY,
    WIN_P6_INFO_SX, WIN_P6_INFO_SY, WIN_P6_INFO_PAL
  },
  { // �u�Ԃ�邢�v
    GFL_BG_FRAME1_S, WIN_P6_BUNRUI_PX, WIN_P6_BUNRUI_PY,
    WIN_P6_BUNRUI_SX, WIN_P6_BUNRUI_SY, WIN_P6_BUNRUI_PAL
  },
  { // ���ޖ�
    GFL_BG_FRAME1_S, WIN_P6_BRNAME_PX, WIN_P6_BRNAME_PY,
    WIN_P6_BRNAME_SX, WIN_P6_BRNAME_SY, WIN_P6_BRNAME_PAL
  },
  { // �u�킷���v
    GFL_BG_FRAME1_S, WIN_P6_WASURERU_PX, WIN_P6_WASURERU_PY,
    WIN_P6_WASURERU_SX, WIN_P6_WASURERU_SY, WIN_P6_WASURERU_PAL
  }
};

// �Z�񕜃y�[�W��BMP�E�B���h�E�f�[�^
static const u8 PPRcvPage_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_P7_NAME_PX, WIN_P7_NAME_PY,
    WIN_P7_NAME_SX, WIN_P7_NAME_SY, WIN_P7_NAME_PAL
  },
  { // �Z�P
    GFL_BG_FRAME1_S, WIN_P7_SKILL1_PX, WIN_P7_SKILL1_PY,
    WIN_P7_SKILL1_SX, WIN_P7_SKILL1_SY, WIN_P7_SKILL1_PAL
  },
  { // �Z�Q
    GFL_BG_FRAME1_S, WIN_P7_SKILL2_PX, WIN_P7_SKILL2_PY,
    WIN_P7_SKILL2_SX, WIN_P7_SKILL2_SY, WIN_P7_SKILL2_PAL
  },
  { // �Z�R
    GFL_BG_FRAME1_S, WIN_P7_SKILL3_PX, WIN_P7_SKILL3_PY,
    WIN_P7_SKILL3_SX, WIN_P7_SKILL3_SY, WIN_P7_SKILL3_PAL
  },
  { // �Z�S
    GFL_BG_FRAME1_S, WIN_P7_SKILL4_PX, WIN_P7_SKILL4_PY,
    WIN_P7_SKILL4_SX, WIN_P7_SKILL4_SY, WIN_P7_SKILL4_PAL
  },
};

// �y�[�W�W��BMP�E�B���h�E�f�[�^
/*
static const u8 Page8_BmpData[][6] =
{
  { // ���O
    GFL_BG_FRAME1_S, WIN_P8_NAME_PX, WIN_P8_NAME_PY,
    WIN_P8_NAME_SX, WIN_P8_NAME_SY, WIN_P8_NAME_PAL
  },
  { // �Z��
    GFL_BG_FRAME1_S, WIN_P8_SKILL_PX, WIN_P8_SKILL_PY,
    WIN_P8_SKILL_SX, WIN_P8_SKILL_SY, WIN_P8_SKILL_PAL
  },
  { // PP
    GFL_BG_FRAME1_S, WIN_P8_PP_PX, WIN_P8_PP_PY,
    WIN_P8_PP_SX, WIN_P8_PP_SY, WIN_P8_PP_PAL
  },
  { // PP/PP
    GFL_BG_FRAME1_S, WIN_P8_PPNUM_PX, WIN_P8_PPNUM_PY,
    WIN_P8_PPNUM_SX, WIN_P8_PPNUM_SY, WIN_P8_PPNUM_PAL
  },
  { // �u�A�s�[���|�C���g�v
    GFL_BG_FRAME1_S, WIN_P8_APP_PX, WIN_P8_APP_PY,
    WIN_P8_APP_SX, WIN_P8_APP_SY, WIN_P8_APP_PAL
  },
  { // �Z����
    GFL_BG_FRAME1_S, WIN_P8_INFO_PX, WIN_P8_INFO_PY,
    WIN_P8_INFO_SX, WIN_P8_INFO_SY, WIN_P8_INFO_PAL
  },
  { // �u�킷���v
    GFL_BG_FRAME1_S, WIN_P8_WASURERU_PX, WIN_P8_WASURERU_PY,
    WIN_P8_WASURERU_SX, WIN_P8_WASURERU_SY, WIN_P8_WASURERU_PAL
  },
};
*/

// �|�P�������̃��b�Z�[�WID
static const u32 NameMsgID_Tbl[] = {
  mes_b_plist_01_001,
  mes_b_plist_01_101,
  mes_b_plist_01_201,
  mes_b_plist_01_301,
  mes_b_plist_01_401,
  mes_b_plist_01_501
};

// �Z���̃��b�Z�[�WID
static const u32 WazaMsgID_Tbl[] = {
  mes_b_plist_05_100,
  mes_b_plist_05_200,
  mes_b_plist_05_300,
  mes_b_plist_05_400,
  mes_b_plist_05_500
};



//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E������
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpInit( BPLIST_WORK * wk )
{
  const u8 * dat;
  u32 i;

  dat = CommBmpData[0];
  for( i=0; i<WIN_MAX; i++ ){
//    GF_BGL_BmpWinAddEx( wk->bgl, &wk->win[i], &CommBmpData[i] );
    wk->win[i].win = GFL_BMPWIN_Create(
                      dat[0],
                      dat[1], dat[2],
                      dat[3], dat[4],
                      dat[5],
                      GFL_BMP_CHRAREA_GET_B );
    dat += 6;
  }
  BattlePokeList_BmpAdd( wk, wk->page );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W���Ƃ�BMP�E�B���h�E�ǉ�
 *
 * @param wk    ���[�N
 * @param page  �y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpAdd( BPLIST_WORK * wk, u32 page )
{
  const u8 * dat;
  u32 i;

  DelDummyBmpWin( wk );
  AddDummyBmpWin( wk );

  switch( page ){
  case BPLIST_PAGE_SELECT:    // �|�P�����I���y�[�W
	case BPLIST_PAGE_DEAD:			// �m������ւ��I���y�[�W
    dat = Page1_BmpData[0];
    wk->bmp_add_max = WIN_P1_MAX;
    break;

  case BPLIST_PAGE_POKE_CHG:    // �|�P��������ւ��y�[�W
    dat = ChgPage_BmpData[0];
    wk->bmp_add_max = WIN_CHG_MAX;
    break;

  case BPLIST_PAGE_MAIN:      // �X�e�[�^�X���C���y�[�W
    dat = StMainPage_BmpData[0];
    wk->bmp_add_max = WIN_P3_MAX;
    break;

  case BPLIST_PAGE_WAZA_SEL:    // �X�e�[�^�X�Z�I���y�[�W
    dat = StWazaSelPage_BmpData[0];
    wk->bmp_add_max = WIN_STW_MAX;
    break;

  case BPLIST_PAGE_SKILL:     // �X�e�[�^�X�Z�y�[�W
    dat = StWazaInfoPage_BmpData[0];
    wk->bmp_add_max = WIN_P4_MAX;
    break;

  case BPLIST_PAGE_PP_RCV:    // PP�񕜋Z�I���y�[�W
    dat = PPRcvPage_BmpData[0];
    wk->bmp_add_max = WIN_P7_MAX;
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
//  case BPLIST_PAGE_WAZASET_CS:  // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�I���j
    dat = Page5_BmpData[0];
    wk->bmp_add_max = WIN_P5_MAX;
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
    dat = Page6_BmpData[0];
    wk->bmp_add_max = WIN_P6_MAX;
    break;
/*
  case BPLIST_PAGE_WAZASET_CI:  // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�ڍׁj
    dat = Page8_BmpData[0];
    wk->bmp_add_max = WIN_P8_MAX;
    break;
*/
  }

//  wk->add_win = GFL_BMPWIN_Init( wk->dat->heap, wk->bmp_add_max );

  for( i=0; i<wk->bmp_add_max; i++ ){
//    GF_BGL_BmpWinAddEx( wk->bgl, &wk->add_win[i], &dat[i] );
    wk->add_win[i].win = GFL_BMPWIN_Create(
                          dat[0],
                          dat[1], dat[2],
                          dat[3], dat[4],
                          dat[5],
                          GFL_BMP_CHRAREA_GET_B );
    dat += 6;
  }
}

static void AddDummyBmpWin( BPLIST_WORK * wk )
{
  if( wk->bmp_swap == 1 ){
    wk->dmy_win = GFL_BMPWIN_Create( GFL_BG_FRAME1_S, 0, 0, 32, 16, 0, GFL_BMP_CHRAREA_GET_B );
  }
  wk->bmp_swap ^= 1;
}

static void DelDummyBmpWin( BPLIST_WORK * wk )
{
  if( wk->dmy_win != NULL ){
    GFL_BMPWIN_Delete( wk->dmy_win );
    wk->dmy_win = NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �ǉ�BMP�E�B���h�E�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpFree( BPLIST_WORK * wk )
{
  u32 i;

  for( i=0; i<wk->bmp_add_max; i++ ){
    GFL_BMPWIN_Delete( wk->add_win[i].win );
  }

//  GF_BGL_BmpWinFree( wk->add_win, wk->bmp_add_max );
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�S�폜
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_BmpFreeAll( BPLIST_WORK * wk )
{
  u32 i;

//  GF_BGL_BmpWinFree( wk->add_win, wk->bmp_add_max );
  BattlePokeList_BmpFree( wk );
  for( i=0; i<WIN_MAX; i++ ){
    GFL_BMPWIN_Delete( wk->win[i].win );
  }
  DelDummyBmpWin( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * BMP��������
 *
 * @param wk    ���[�N
 * @param page  �y�[�W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
// �y�[�W�P�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 PokeSelPutWin[] = {
  WIN_P1_POKE1,   // �|�P�����P
  WIN_P1_POKE2,   // �|�P�����Q
  WIN_P1_POKE3,   // �|�P�����R
  WIN_P1_POKE4,   // �|�P�����S
  WIN_P1_POKE5,   // �|�P�����T
  WIN_P1_POKE6,   // �|�P�����U
  BAPPTOOL_SET_STR_SCRN_END
};

// ����ւ��y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 PokeChgPutWin[] = {
  WIN_CHG_NAME,       // ���O
  WIN_CHG_IREKAE,     // �u���ꂩ����v
  WIN_CHG_STATUS,     // �u�悳���݂�v
  WIN_CHG_WAZACHECK,  // �u�킴���݂�v
  BAPPTOOL_SET_STR_SCRN_END
};

// �Z�I���y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 WazaSelPutWin[] = {
  WIN_STW_NAME,     // ���O
  WIN_STW_SKILL1,   // �Z�P
  WIN_STW_SKILL2,   // �Z�Q
  WIN_STW_SKILL3,   // �Z�R
  WIN_STW_SKILL4,   // �Z�S
  WIN_STW_STATUS,   // �u�悳���݂�v
  BAPPTOOL_SET_STR_SCRN_END
};
/*
static const u8 WazaSelPutWinS[] = {
  WIN_STW_NAME_S,     // ���O�i�X���b�v�j
  WIN_STW_SKILL1_S,   // �Z�P�i�X���b�v�j
  WIN_STW_SKILL2_S,   // �Z�Q�i�X���b�v�j
  WIN_STW_SKILL3_S,   // �Z�R�i�X���b�v�j
  WIN_STW_SKILL4_S,   // �Z�S�i�X���b�v�j
  WIN_STW_STATUS,   // �u�悳���݂�v
  BAPPTOOL_SET_STR_SCRN_END
};
*/

// �X�e�[�^�X���C���y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 StMainPutWin[] = {
  WIN_P3_NAME,      // ���O
  WIN_P3_SPANAME,   // ������
  WIN_P3_SPAINFO,   // ��������
  WIN_P3_ITEMNAME,  // ���
  WIN_P3_HPNUM,     // HP�l
  WIN_P3_POWNUM,    // �U���l
  WIN_P3_DEFNUM,    // �h��l
  WIN_P3_AGINUM,    // �f�����l
  WIN_P3_SPPNUM,    // ���U�l
  WIN_P3_SPDNUM,    // ���h�l
  WIN_P3_HPGAGE,    // HP�Q�[�W
  WIN_P3_LVNUM,     // ���x���l
  WIN_P3_NEXTNUM,   // ���̃��x���l

  WIN_P3_HP,        // �uHP�v
  WIN_P3_POW,       // �u���������v
  WIN_P3_DEF,       // �u�ڂ�����v
  WIN_P3_AGI,       // �u���΂₳�v
  WIN_P3_SPP,       // �u�Ƃ������v
  WIN_P3_SPD,       // �u�Ƃ��ڂ��v
  WIN_P3_LV,        // �uLv.�v
  WIN_P3_NEXT,      // �u���̃��x���܂Łv
  WIN_P3_WAZACHECK, // �u�킴���݂�v
  BAPPTOOL_SET_STR_SCRN_END
};
/*
static const u8 StMainPutWinS[] = {
  WIN_P3_NAME_S,      // ���O�i�X���b�v�j
  WIN_P3_SPANAME_S,   // �������i�X���b�v�j
  WIN_P3_SPAINFO_S,   // ���������i�X���b�v�j
  WIN_P3_ITEMNAME_S,  // ����i�X���b�v�j
  WIN_P3_HPNUM_S,     // HP�l�i�X���b�v�j
  WIN_P3_POWNUM_S,    // �U���l�i�X���b�v�j
  WIN_P3_DEFNUM_S,    // �h��l�i�X���b�v�j
  WIN_P3_AGINUM_S,    // �f�����l�i�X���b�v�j
  WIN_P3_SPPNUM_S,    // ���U�l�i�X���b�v�j
  WIN_P3_SPDNUM_S,    // ���h�l�i�X���b�v�j
  WIN_P3_HPGAGE_S,    // HP�Q�[�W�i�X���b�v�j
  WIN_P3_LVNUM_S,     // ���x���l�i�X���b�v�j
  WIN_P3_NEXTNUM_S,   // ���̃��x���l�i�X���b�v�j

  WIN_P3_HP,        // �uHP�v
  WIN_P3_POW,       // �u���������v
  WIN_P3_DEF,       // �u�ڂ�����v
  WIN_P3_AGI,       // �u���΂₳�v
  WIN_P3_SPP,       // �u�Ƃ������v
  WIN_P3_SPD,       // �u�Ƃ��ڂ��v
  WIN_P3_LV,        // �uLv.�v
  WIN_P3_NEXT,      // �u���̃��x���܂Łv
  WIN_P3_WAZACHECK, // �u�킴���݂�v
  BAPPTOOL_SET_STR_SCRN_END
};
*/

// �X�e�[�^�X�Z�ڍ׃y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 WazaInfoPutWin[] = {
  WIN_P4_SKILL,     // �Z��
  WIN_P4_PPNUM,     // PP/PP
  WIN_P4_HITNUM,    // �����l
  WIN_P4_POWNUM,    // �З͒l
  WIN_P4_INFO,      // �Z����
  WIN_P4_BRNAME,    // ���ޖ�

  WIN_P4_NAME,      // ���O
  WIN_P4_PP,        // PP
  WIN_P4_HIT,       // �u�߂����イ�v
  WIN_P4_POW,       // �u����傭�v
  WIN_P4_BUNRUI,    // �u�Ԃ�邢�v
  BAPPTOOL_SET_STR_SCRN_END
};
/*
static const u8 WazaInfoPutWinS[] = {
  WIN_P4_SKILL_S,   // �Z��
  WIN_P4_PPNUM_S,   // PP/PP
  WIN_P4_HITNUM_S,  // �����l
  WIN_P4_POWNUM_S,  // �З͒l
  WIN_P4_INFO_S,    // �Z����
  WIN_P4_BRNAME_S,  // ���ޖ�

  WIN_P4_NAME,      // ���O
  WIN_P4_PP,        // PP
  WIN_P4_HIT,       // �u�߂����イ�v
  WIN_P4_POW,       // �u����傭�v
  WIN_P4_BUNRUI,    // �u�Ԃ�邢�v
  BAPPTOOL_SET_STR_SCRN_END
};
*/

// �Z�Y��I���y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 WazaDelPutWin[] = {
  WIN_P5_NAME,      // ���O
  WIN_P5_SKILL1,    // �Z�P
  WIN_P5_SKILL2,    // �Z�Q
  WIN_P5_SKILL3,    // �Z�R
  WIN_P5_SKILL4,    // �Z�S
  WIN_P5_SKILL5,    // �Z�T
  BAPPTOOL_SET_STR_SCRN_END
};

// �y�[�W�U�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 WazaDelInfoPutWin[] = {
  WIN_P6_NAME,      // ���O
  WIN_P6_SKILL,     // �Z��
  WIN_P6_PP,        // PP
  WIN_P6_PPNUM,     // PP/PP
  WIN_P6_HIT,       // �u�߂����イ�v
  WIN_P6_POW,       // �u����傭�v
  WIN_P6_HITNUM,    // �����l
  WIN_P6_POWNUM,    // �З͒l
  WIN_P6_INFO,      // �Z����
  WIN_P6_BUNRUI,    // �u�Ԃ�邢�v
  WIN_P6_BRNAME,    // ���ޖ�
  WIN_P6_WASURERU,  // �u�킷���v
  BAPPTOOL_SET_STR_SCRN_END
};

// �Z�񕜑I���y�[�W�̒ǉ��E�B���h�E�C���f�b�N�X
static const u8 WazaRcvPutWin[] = {
  WIN_P7_NAME,      // ���O
  WIN_P7_SKILL1,    // �Z�P
  WIN_P7_SKILL2,    // �Z�Q
  WIN_P7_SKILL3,    // �Z�R
  WIN_P7_SKILL4,    // �Z�S
  BAPPTOOL_SET_STR_SCRN_END
};

void BattlePokeList_BmpWrite( BPLIST_WORK * wk, u32 page )
{
  switch( page ){
  case BPLIST_PAGE_SELECT:    // �|�P�����I���y�[�W
    BPL_Page1BmpWrite( wk );
		break;

	case BPLIST_PAGE_DEAD:			// �m������ւ��I���y�[�W
    BPL_Page1BmpWrite( wk );
    BPL_PokeSelStrPut( wk, mes_b_plist_01_602 );
    break;

  case BPLIST_PAGE_POKE_CHG:    // �|�P��������ւ��y�[�W
    BPL_ChgPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_MAIN:      // �X�e�[�^�X���C���y�[�W
    BPL_StMainPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_WAZA_SEL:    // �X�e�[�^�X�Z�I���y�[�W
    BPL_StWazaSelPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_SKILL:     // �X�e�[�^�X�Z�ڍ׃y�[�W
    BPL_StWazaInfoPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_PP_RCV:    // PP�񕜋Z�I���y�[�W
    BPL_PPRcvPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_WAZASET_BS:  // �X�e�[�^�X�Z�Y��P�y�[�W�i�퓬�Z�I���j
//  case BPLIST_PAGE_WAZASET_CS:  // �X�e�[�^�X�Z�Y��S�y�[�W�i�R���e�X�g�Z�I���j
    BPL_WazaDelSelPageBmpWrite( wk );
    break;

  case BPLIST_PAGE_WAZASET_BI:  // �X�e�[�^�X�Z�Y��Q�y�[�W�i�퓬�Z�ڍׁj
    BPL_Page6BmpWrite( wk );
    break;

/*
  case BPLIST_PAGE_WAZASET_CI:  // �X�e�[�^�X�Z�Y��R�y�[�W�i�R���e�X�g�Z�ڍׁj
    BPL_Page8BmpWrite( wk );
    break;
*/
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �|�P�������\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_NamePut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
  GFL_BMPWIN * win;
  BPL_POKEDATA * pd;
  STRBUF * exp;
  STRBUF * str;
  u16 sex_px;
  u16 pal;

  win = wk->add_win[idx].win;
  pal = GFL_BMPWIN_GetPalette( win );
  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, wk->dat->heap );
  str = GFL_MSG_CreateString( wk->mman, NameMsgID_Tbl[pos] );

  WORDSET_RegisterPokeNickName( wk->wset, 0, pd->pp );
  WORDSET_ExpandStr( wk->wset, exp, str );

  if( pal == BPL_COMM_PAL_HPGAGE ){
//    PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), px, py, exp, wk->dat->font, FCOL_P09WN );
    PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, py, exp, wk->dat->font, FCOL_P09WN );
  }else{
//    PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), px, py, exp, wk->dat->font, FCOL_P13WN );
    PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, py, exp, wk->dat->font, FCOL_P13WN );
  }

  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

  // ����
  if( pd->sex_put == 0 && pd->egg == 0 ){
    if( pd->sex == PTL_SEX_MALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_502 );
      sex_px = GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->dat->font,0);
      if( pal == BPL_COMM_PAL_HPGAGE ){
//        PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, FCOL_P09BLN );
        PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, sex_px, py, str, wk->dat->font, FCOL_P09BLN );
      }else{
//        PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, FCOL_P13BLN );
        PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, sex_px, py, str, wk->dat->font, FCOL_P13BLN );
      }
      GFL_STR_DeleteBuffer( str );
    }else if( pd->sex == PTL_SEX_FEMALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_503 );
      sex_px = GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(str,wk->dat->font,0);
      if( pal == BPL_COMM_PAL_HPGAGE ){
//        PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, FCOL_P09RN );
        PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, sex_px, py, str, wk->dat->font, FCOL_P09RN );
      }else{
//        PRINTSYS_PrintQueColor( wk->que, GFL_BMPWIN_GetBmp(win), sex_px, py, str, wk->dat->font, FCOL_P13RN );
        PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, sex_px, py, str, wk->dat->font, FCOL_P13RN );
      }
      GFL_STR_DeleteBuffer( str );
    }
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * ���x���\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_LvPut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
  BPL_POKEDATA * pd;
  STRBUF * str;

  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_701 );

  WORDSET_RegisterNumber(
    wk->wset, 0, pd->lv, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * HP�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_HPPut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  u8  sx;

  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  //�u�^�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_702 );
  sx  = PRINTSYS_GetStrWidth( str, wk->nfnt, 0 );
  px  = px - ( sx / 2 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py, str, wk->nfnt, FCOL_P09WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, py, str, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
  // HP
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->hp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win),
//    px-PRINTSYS_GetStrWidth(wk->msg_buf,wk->nfnt,0), py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que,
    px-PRINTSYS_GetStrWidth(wk->msg_buf,wk->nfnt,0), py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
  // MHP
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_01_703 );
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->mhp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win),
//    px+sx, py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que,
    px+sx, py, wk->msg_buf, wk->nfnt, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * HP�Q�[�W�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_HPGagePut( BPLIST_WORK * wk, u32 idx, u16 pos, u8 px, u8 py )
{
  BPL_POKEDATA * pd;
  u8  col=1;
  u8  dot;

  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  col = 1;
  dot = GAUGETOOL_GetNumDotto( pd->hp, pd->mhp, BPL_COMM_HP_DOTTO_MAX );

  switch( GAUGETOOL_GetGaugeDottoColor( pd->hp, pd->mhp ) ){
  case GAUGETOOL_HP_DOTTO_NULL:
//    GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[idx].win );
    return;
  case GAUGETOOL_HP_DOTTO_GREEN:    // ��
    col = BPL_COMM_HP_GAGE_COL_G1;
    break;
  case GAUGETOOL_HP_DOTTO_YELLOW:   // ��
    col = BPL_COMM_HP_GAGE_COL_Y1;
    break;
  case GAUGETOOL_HP_DOTTO_RED:      // ��
    col = BPL_COMM_HP_GAGE_COL_R1;
    break;
  }

/*
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+1, dot, 1, col+1 );
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+2, dot, 2, col );
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+4, dot, 1, col+1 );
*/
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+3, dot, 1, col );
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py+4, dot, 1, col+1 );

//  GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[idx].win );
}

//--------------------------------------------------------------------------------------------
/**
 * �����\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_TokuseiPut( BPLIST_WORK * wk, u32 idx, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * exp;
  STRBUF * str;

  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_ABILITY_NAME, wk->dat->heap );
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_000 );

  WORDSET_RegisterTokuseiName( wk->wset, 0, pd->spa );
  WORDSET_ExpandStr( wk->wset, exp, str );

//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), 0, 0, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, 0, 0, exp, wk->dat->font, FCOL_P13WN );

  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ItemPut( BPLIST_WORK * wk, u32 idx, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * exp;
  STRBUF * str;

  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  if( pd->item == 0 ){
    exp = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_600 );
  }else{
    exp = GFL_STR_CreateBuffer( BUFLEN_ITEM_NAME, wk->dat->heap );
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_001 );
    WORDSET_RegisterItemName( wk->wset, 0, pd->item );
    WORDSET_ExpandStr( wk->wset, exp, str );
    GFL_STR_DeleteBuffer( str );
  }
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), 0, 0, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, 0, 0, exp, wk->dat->font, FCOL_P13WN );

  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���\��
 *
 * @param wk    ���[�N
 * @param waza  �Z�ԍ�
 * @param widx  �E�B���h�E�C���f�b�N�X
 * @param midx  ���b�Z�[�W�C���f�b�N�X
 * @param fidx  �t�H���g�C���f�b�N�X
 * @param py    Y���W
 * @param col   �J���[
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaNamePut(
        BPLIST_WORK * wk, u32 waza, u32 widx, u32 midx, u16 py, u32 col )
{
  GFL_BMPWIN * win;
  STRBUF * exp;
  STRBUF * str;
  u32 px;

  win = wk->add_win[widx].win;

  exp = GFL_STR_CreateBuffer( BUFLEN_WAZA_NAME, wk->dat->heap );
  str = GFL_MSG_CreateString( wk->mman, midx );

  WORDSET_RegisterWazaName( wk->wset, 0, waza );
  WORDSET_ExpandStr( wk->wset, exp, str );
/*
  if( fidx == FONT_TOUCH ){
    px = (GFL_BMPWIN_GetSizeX(win)*8-PRINTSYS_GetStrWidth(exp,wk->dat->font,0))/2;
  }else{
    px = 0;
  }
*/
  px = 0;

//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px, py, exp, wk->dat->font, col );
  PRINT_UTIL_PrintColor( &wk->add_win[widx], wk->que, px, py, exp, wk->dat->font, col );

  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[widx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[widx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �uPP�v�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PPPut( BPLIST_WORK * wk, u16 idx, u8 px, u8 py )
{
  STRBUF * str;

  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_500 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[idx].win), px, py, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, py, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * PP�l�\��
 *
 * @param wk    ���[�N
 * @param waza  �Z�f�[�^
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param px    �\��X���W
 * @param py    �\��Y���W
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_PPNumPut( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u16 idx, u8 px, u8 py )
{
  NUMFONT_WriteNumber(
    wk->nfnt, waza->pp, 2, NUMFONT_MODE_SPACE, &wk->add_win[idx], px, py );
  NUMFONT_WriteMark(
    wk->nfnt, NUMFONT_MARK_SLASH, &wk->add_win[idx], px+NUMFONT_NUM_WIDTH*2, py );
  NUMFONT_WriteNumber(
    wk->nfnt, waza->mpp, 2, NUMFONT_MODE_LEFT,
    &wk->add_win[idx], px+NUMFONT_NUM_WIDTH*2+8, py );

  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * �|�P�����I���y�[�W�R�����g�\��
 *
 * @param wk    ���[�N
 * @param midx  ���b�Z�[�W�C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PokeSelStrPut( BPLIST_WORK * wk, u32 midx )
{
  STRBUF * str;

/*
  BmpWinFrame_Write(
    &wk->win[WIN_COMMENT], WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
*/

/*
  BmpWinFrame_Write(
    wk->win[WIN_COMMENT].win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
*/

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_COMMENT].win), 15 );

  str = GFL_MSG_CreateString( wk->mman, midx );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->win[WIN_COMMENT].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->win[WIN_COMMENT], wk->que, 0, 0, str, wk->dat->font, FCOL_P13TALK );
  GFL_STR_DeleteBuffer( str );

  wk->page_chg_comm = 1;
//  BAPPTOOL_PrintScreenTrans( &wk->win[WIN_COMMENT] );
//  BAPPTOOL_PrintQueOn( &wk->win[WIN_COMMENT] );
}

void BPLISTBMP_PokeSelInfoMesPut( BPLIST_WORK * wk )
{
	BPL_PokeSelStrPut( wk, mes_b_plist_01_600 );
}

void BPLISTBMP_DeadSelInfoMesPut( BPLIST_WORK * wk )
{
	BPL_PokeSelStrPut( wk, mes_b_plist_01_602 );
}


//--------------------------------------------------------------------------------------------
/**
 * �u���ꂩ���v�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#define IREKAE_PY ( 8-1 )

static void BPL_StrIrekaePut( BPLIST_WORK * wk )
{
  STRBUF * str;
  u32 siz;

  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_501 );
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_IREKAE].win),
//    (WIN_CHG_IREKAE_SX*8-siz)/2, IREKAE_PY, str, wk->dat->font, PCOL_BTN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_CHG_IREKAE], wk->que,
    (WIN_CHG_IREKAE_SX*8-siz)/2, IREKAE_PY, str, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_CHG_IREKAE] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_IREKAE] );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�}���h�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#define COMMAND_STR_PY  ( 6-1 )

static void BPL_StrCommandPut( BPLIST_WORK * wk, u32 wid, u32 mid )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  u32 siz;

  win = wk->add_win[wid].win;
  str = GFL_MSG_CreateString( wk->mman, mid );
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    (GFL_BMPWIN_GetSizeX(win)*8-siz)/2, COMMAND_STR_PY, str, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[wid], wk->que,
    (GFL_BMPWIN_GetSizeX(win)*8-siz)/2, COMMAND_STR_PY, str, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[wid] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[wid] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̃��x���\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_LvPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u16 px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �uLv.�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_000 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LV].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_LV], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // ���x���l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_001 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->lv, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LVNUM].win), 0, 0, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_LVNUM], wk->que, 0, 0, exp, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );
  // �u���̃��x���܂Łv
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_100 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXT].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_NEXT], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // ���̃��x���l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_101 );
  exp = GFL_STR_CreateBuffer( (6+1)*2, wk->dat->heap ); // (6��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  if( pd->lv < 100 ){
    WORDSET_RegisterNumber(
      wk->wset, 0, pd->next_lv_exp - pd->now_exp,
      6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  }else{
    WORDSET_RegisterNumber(
      wk->wset, 0, 0, 6, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  }
  WORDSET_ExpandStr( wk->wset, exp, str );
  px = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_NEXTNUM].win ) * 8
      - PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXTNUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_NEXTNUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_LV] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_LVNUM] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_NEXT] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_NEXTNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_LV] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_LVNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_NEXT] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_NEXTNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̍U���\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_PowPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u8  siz;
  u8  px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �u���������v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_400 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POW].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[WIN_P3_POW], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // �U���l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_401 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_POWNUM].win ) * 8 - siz;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POWNUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_POWNUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_POW] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_POWNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_POW] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_POWNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̖h��\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_DefPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u8  siz;
  u8  px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �u�ڂ�����v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_500 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEF].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_DEF], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // �h��l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_501 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->def, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_DEFNUM].win ) * 8 - siz;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEFNUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_DEFNUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_DEF] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_DEFNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_DEF] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_DEFNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̑f�����\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_AgiPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u8  siz;
  u8  px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �u���΂₳�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_800 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGI].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_AGI], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // �f�����l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_801 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->agi, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_AGINUM].win ) * 8 - siz;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGINUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_AGINUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_AGI] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_AGINUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_AGI] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_AGINUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̓��U�\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_SppPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u8  siz;
  u8  px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �u�Ƃ������v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_600 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPP].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_SPP], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // ���U�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_601 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->spp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_SPPNUM].win ) * 8 - siz;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPPNUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_SPPNUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_SPP] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_SPPNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPP] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPPNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̓��h�\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_SpdPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u8  siz;
  u8  px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �u�Ƃ��ڂ��v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_700 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPD].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_SPD], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // ���h�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_701 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->spd, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  px  = GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_SPDNUM].win ) * 8 - siz;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPDNUM].win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_SPDNUM], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_SPD] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_SPDNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPD] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPDNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R��HP�\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_HPPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  STRBUF * str;
  STRBUF * exp;
  u32 sra_siz, tmp_siz;
  u16 px;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  // �uHP�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_300 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HP].win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_HP], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // �u/�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_303 );
  sra_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
  px = (GFL_BMPWIN_GetSizeX( wk->add_win[WIN_P3_HPNUM].win )*8-sra_siz)/2;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM].win), px, 0, str, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_HPNUM], wk->que, px, 0, str, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  // HP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_301 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->hp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  tmp_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM].win),
//    px-tmp_siz, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_HPNUM], wk->que, px-tmp_siz, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );
  // �ő�HP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_03_302 );
  exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, pd->mhp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM].win),
//    px+sra_siz, 0, exp, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_HPNUM], wk->que, px+sra_siz, 0, exp, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_HP] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_HPNUM] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_HP] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_HPNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�R�̓����\��
 *
 * @param wk    ���[�N
 * @param pos   �|�P�����ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_P3_TokuseiInfoPut( BPLIST_WORK * wk, u32 pos )
{
  BPL_POKEDATA * pd;
  GFL_MSGDATA * man;
  STRBUF * str;

  pd   = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];

  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_tokuseiinfo_dat, wk->dat->heap );
  str = GFL_MSG_CreateString( man, pd->spa );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPAINFO].win), 0, 0, str, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_P3_SPAINFO], wk->que, 0, 0, str, wk->dat->font, FCOL_P13BKN );
  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( man );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P3_SPAINFO] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P3_SPAINFO] );
}

//--------------------------------------------------------------------------------------------
/**
 * �u�߂����イ�v�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaHitStrPut( BPLIST_WORK * wk, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;

  win = wk->add_win[idx].win;
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_200 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�̖����l�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param hit   �����l
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaHitNumPut( BPLIST_WORK * wk, u32 idx, u32 hit )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  STRBUF * exp;
  u16 siz;
  u16 px;

  win = wk->add_win[idx].win;

  if( hit == 0 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_102 );
    siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
    px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
//    PRINTSYS_PrintQueColor(
//      wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, FCOL_P13BKN );
    PRINT_UTIL_PrintColor(
      &wk->add_win[idx], wk->que, px, 0, str, wk->dat->font, FCOL_P13BKN );
    GFL_STR_DeleteBuffer( str );
  }else{
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_201 );
    exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
    WORDSET_RegisterNumber(
      wk->wset, 0, hit, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_ExpandStr( wk->wset, exp, str );
    siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
    px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
//    PRINTSYS_PrintQueColor(
//      wk->que, GFL_BMPWIN_GetBmp(win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
    PRINT_UTIL_PrintColor(
      &wk->add_win[idx], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
    GFL_STR_DeleteBuffer( str );
    GFL_STR_DeleteBuffer( exp );
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �u����傭�v�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPowStrPut( BPLIST_WORK * wk, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;

  win = wk->add_win[idx].win;
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_100 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�̈З͒l�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param pow   �З͒l
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPowNumPut( BPLIST_WORK * wk, u32 idx, u32 pow )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  STRBUF * exp;
  u16 siz;
  u16 px;

  win = wk->add_win[idx].win;

  if( pow <= 1 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_102 );
    siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
    px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
//    PRINTSYS_PrintQueColor(
//      wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, FCOL_P13BKN );
    PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, 0, str, wk->dat->font, FCOL_P13BKN );
    GFL_STR_DeleteBuffer( str );
  }else{
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_101 );
    exp = GFL_STR_CreateBuffer( (3+1)*2, wk->dat->heap ); // (3��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
    WORDSET_RegisterNumber(
      wk->wset, 0, pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_ExpandStr( wk->wset, exp, str );
    siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
    px  = GFL_BMPWIN_GetSizeX(win) * 8 - siz;
//    PRINTSYS_PrintQueColor(
//      wk->que, GFL_BMPWIN_GetBmp(win), px, 0, exp, wk->dat->font, FCOL_P13BKN );
    PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, 0, exp, wk->dat->font, FCOL_P13BKN );
    GFL_STR_DeleteBuffer( str );
    GFL_STR_DeleteBuffer( exp );
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�����\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param waza  �ZID
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaInfoPut( BPLIST_WORK * wk, u32 idx, u32 waza )
{
  GFL_MSGDATA * man;
  GFL_BMPWIN * win;
  STRBUF * str;

  win = wk->add_win[idx].win;
  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wazainfo_dat, wk->dat->heap );
  str = GFL_MSG_CreateString( man, waza );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( man );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}


//--------------------------------------------------------------------------------------------
/**
 * �u�Ԃ�邢�v�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaBunruiStrPut( BPLIST_WORK * wk, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  u16 siz;
  u16 px;

  win = wk->add_win[idx].win;
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_300 );
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
  px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * ���ގ�ޕ\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param kind  ���ގ��
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaKindPut( BPLIST_WORK * wk, u32 idx, u32 kind )
{
  GFL_BMPWIN * win;
  STRBUF * str;

  win = wk->add_win[idx].win;

  switch( kind ){
  case 0:   // ����
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_301 );
    break;
  case 1:   // �ω�
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_303 );
    break;
  case 2:   // ����
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_302 );
  }
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), 0, 0, str, wk->dat->font, FCOL_P13BKN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �ZPP�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 * @param npp   ���݂�PP
 * @param mpp   �ő�PP
 *
 * @return  none
 *
 *  �y�[�W�S�E�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaPPPut( BPLIST_WORK * wk, u32 idx, u32 npp, u32 mpp )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  STRBUF * exp;
  u32 sra_siz, tmp_siz;
  u32 px;

  win = wk->add_win[idx].win;

  // �u/�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_004 );
  sra_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
  px = ( GFL_BMPWIN_GetSizeX(win) * 8 - sra_siz ) / 2;
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px, 0, str, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor( &wk->add_win[idx], wk->que, px, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  // PP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
  exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap ); // (2��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, npp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  tmp_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px-tmp_siz, 0, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, px-tmp_siz, 0, exp, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );
  // �ő�PP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_003 );
  exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap ); // (2��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)
  WORDSET_RegisterNumber(
    wk->wset, 0, mpp, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px+sra_siz, 0, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, px+sra_siz, 0, exp, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

#define P5_PPNUM5_PX  ( 56 )    // PP�l�\��X���W
#define P5_PPNUM5_PY  ( 32 )    // PP�l�\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�T��PP�l�\��
 *
 * @param wk    ���[�N
 * @param waza  �ZID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
/*
static void BPL_P5_PPNumPut( BPLIST_WORK * wk, u32 waza )
{
  GFL_BMPWIN * win;
  u32 pp;

  win = wk->add_win[WIN_P5_SKILL5].win;
  pp  = WAZADATA_GetParam( waza, WAZAPARAM_BASE_PP );

  NUMFONT_WriteNumber( wk->nfnt, pp, 2, NUMFONT_MODE_SPACE, win, P5_PPNUM5_PX, P5_PPNUM5_PY );
  NUMFONT_WriteNumber(
    wk->nfnt, pp, 2, NUMFONT_MODE_LEFT,
    win, P5_PPNUM5_PX+NUMFONT_NUM_WIDTH*2+8, P5_PPNUM5_PY );

  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL5] );
}
*/

#define P6_WASURERU_PY  ( 6-1 ) // �u�킷���v�\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �u�킷���v�\��
 *
 * @param wk    ���[�N
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 *
 *  �y�[�W�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
static void BPL_WasureruStrPut( BPLIST_WORK * wk, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  u32 siz;

  win = wk->add_win[idx].win;

  if( wk->dat->sel_wp == 4 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_001 );
  }else{
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_000 );
  }
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    (WIN_P6_WASURERU_SX*8-siz)/2, P6_WASURERU_PY, str, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que,
    (WIN_P6_WASURERU_SX*8-siz)/2, P6_WASURERU_PY, str, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * ��`�Z�G���[���b�Z�[�W�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 *
 *  �y�[�W�U�Ŏg�p
 */
//--------------------------------------------------------------------------------------------
/*
void BPL_HidenMsgPut( BPLIST_WORK * wk )
{
//  BAPP_BMPWIN_QUE * dat;
  PRINT_UTIL * dat;
  STRBUF * str;

  if( wk->page == BPLIST_PAGE_WAZASET_BI ){
    dat = &wk->add_win[WIN_P6_INFO];
  }else{
    dat = &wk->add_win[WIN_P8_INFO];
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(dat->win), 0 );
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_05_002 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(dat->win), 0, 0, str, wk->dat->font, PCOL_N_BLACK );
  PRINT_UTIL_PrintColor( dat, wk->que, 0, 0, str, wk->dat->font, FCOL_P13WN );
  GFL_STR_DeleteBuffer( str );
  BAPPTOOL_PrintScreenTrans( dat );
//  BAPPTOOL_PrintQueOn( dat );
}
*/


#define BTN_PP_PX   ( 40 )    // �uPP�v�\��X���W
#define BTN_PP_PY   ( 24 )    // �uPP�v�\��Y���W
#define BTN_PPSRA_PX  ( 80 )    // �u/�v�\��X���W
#define BTN_PPSRA_PY  ( 24 )    // �u/�v�\��Y���W

//--------------------------------------------------------------------------------------------
/**
 * �{�^�����PP��\��
 *
 * @param wk    ���[�N
 * @param waza  �Z�f�[�^
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaButtonPPPut( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  STRBUF * exp;
  u32 siz;

  win = wk->add_win[idx].win;
  exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap ); // (2��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)

  // �uPP�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_001 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), BTN_PP_PX, BTN_PP_PY, str, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, BTN_PP_PX, BTN_PP_PY, str, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  // �u/�v
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_004 );
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    BTN_PPSRA_PX, BTN_PPSRA_PY, str, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, BTN_PPSRA_PX, BTN_PPSRA_PY, str, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  // �ő�PP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_003 );
  WORDSET_RegisterNumber(
    wk->wset, 0, waza->mpp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    BTN_PPSRA_PX+siz, BTN_PPSRA_PY, exp, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, BTN_PPSRA_PX+siz, BTN_PPSRA_PY, exp, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  // PP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
  WORDSET_RegisterNumber(
    wk->wset, 0, waza->pp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, FCOL_P09WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�^�����PP�l��\��
 *
 * @param wk    ���[�N
 * @param waza  �Z�f�[�^
 * @param idx   �E�B���h�E�C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaButtonPPRcv( BPLIST_WORK * wk, BPL_POKEWAZA * waza, u32 idx )
{
  GFL_BMPWIN * win;
  STRBUF * str;
  STRBUF * exp;
  u32 siz;

  win = wk->add_win[idx].win;
  exp = GFL_STR_CreateBuffer( (2+1)*2, wk->dat->heap ); // (2��+EOM_)x2(���[�J���C�Y�p�Ɉꉞ)

  // �N���A
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_001 );
  siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
  GFL_STR_DeleteBuffer( str );
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win), BTN_PP_PX+siz, BTN_PPSRA_PY, BTN_PPSRA_PX-(BTN_PP_PX+siz), 16, 0 );

  // PP�l
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_002 );
  WORDSET_RegisterNumber(
    wk->wset, 0, waza->pp, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
  WORDSET_ExpandStr( wk->wset, exp, str );
  siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win),
//    BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, PCOL_B_WHITE );
  PRINT_UTIL_PrintColor(
    &wk->add_win[idx], wk->que, BTN_PPSRA_PX-siz, BTN_PPSRA_PY, exp, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( str );

  GFL_STR_DeleteBuffer( exp );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[idx] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[idx] );
}

// �p�����[�^�\�����W
/*
#define P1_NAME_PX    ( 32 )
#define P1_NAME_PY    ( 8-1 )
#define P1_HP_PX    ( 92 )//( 56 )
#define P1_HP_PY    ( 32 )
#define P1_HP_SX    ( 24 )
#define P1_HP_SY    ( 8 )
#define P1_HPGAGE_PX  ( 48+16 )
#define P1_HPGAGE_PY  ( 24 )
#define P1_HPGAGE_SX  ( 64 )
#define P1_HPGAGE_SY  ( 8 )
#define P1_LV_PX    ( 0 )
#define P1_LV_PY    ( 32 )
*/

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�P��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page1BmpWrite( BPLIST_WORK * wk )
{
	u16	pos;
  s16 i;

  wk->putWin = PokeSelPutWin;

  for( i=0; i<TEMOTI_POKEMAX; i++ ){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+i].win), 0 );

		pos = BPLISTMAIN_GetListRow( wk, i );

    if( wk->poke[pos].mons == 0 ){
      wk->add_win[WIN_P1_POKE1+i].transReq = TRUE;
//      GFL_BMPWIN_TransVramCharacter( wk->add_win[WIN_P1_POKE1+i].win );
      continue;
    }

    BPL_NamePut( wk, WIN_P1_POKE1+i, i, BPL_COMM_P1_NAME_PX, BPL_COMM_P1_NAME_PY );

    if( wk->poke[pos].egg == 0 ){
      BattlePokeList_P1_HPPut( wk, i );
    }

    if( APP_COMMON_GetStatusIconAnime( wk->poke[pos].pp ) != APP_COMMON_ST_ICON_NONE ){
      continue;
    }

    BattlePokeList_P1_LvPut( wk, i );
  }

  if( wk->dat->mode == BPL_MODE_ITEMUSE ){
    BPL_PokeSelStrPut( wk, mes_b_plist_01_601 );
  }else{
    BPL_PokeSelStrPut( wk, mes_b_plist_01_600 );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�P��HP�\��
 *
 * @param wk    ���[�N
 * @param pos   ���шʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_P1_HPPut( BPLIST_WORK * wk, u8 pos )
{
  GFL_BMP_Fill(
    GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+pos].win), BPL_COMM_P1_HP_PX, BPL_COMM_P1_HP_PY, BPL_COMM_P1_HP_SX, BPL_COMM_P1_HP_SY, 0 );
  GFL_BMP_Fill(
    GFL_BMPWIN_GetBmp(wk->add_win[WIN_P1_POKE1+pos].win),
    BPL_COMM_P1_HPGAGE_PX, BPL_COMM_P1_HPGAGE_PY, BPL_COMM_P1_HPGAGE_SX, BPL_COMM_P1_HPGAGE_SY, 0 );
  BPL_HPPut( wk, WIN_P1_POKE1+pos, pos, BPL_COMM_P1_HP_PX, BPL_COMM_P1_HP_PY );
  BPL_HPGagePut( wk, WIN_P1_POKE1+pos, pos, BPL_COMM_P1_HPGAGE_PX, BPL_COMM_P1_HPGAGE_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�P��Lv�\��
 *
 * @param wk    ���[�N
 * @param pos   ���шʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_P1_LvPut( BPLIST_WORK * wk, u8 pos )
{
  if( wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ].egg == 0 ){
    BPL_LvPut( wk, WIN_P1_POKE1+pos, pos, BPL_COMM_P1_LV_PX, BPL_COMM_P1_LV_PY );
  }
}

#define P_CHG_NAME_PX ( 0 )
#define P_CHG_NAME_PY ( 8 )
static void BPL_IrekaeNamePut( BPLIST_WORK * wk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//-------------------------------------------------------------------------------------------
static void BPL_ChgPageBmpWrite( BPLIST_WORK * wk )
{
  wk->putWin = PokeChgPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_IREKAE].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_STATUS].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_CHG_WAZACHECK].win), 0 );

//  BPL_NamePut(
//    wk, WIN_CHG_NAME, FONT_TOUCH, wk->dat->sel_poke, P_CHG_NAME_PX, P_CHG_NAME_PY );

  BPL_IrekaeNamePut( wk, wk->dat->sel_poke );


  BPL_StrCommandPut( wk, WIN_CHG_IREKAE, mes_b_plist_02_501 );

  if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].egg == 0 ){
    BPL_StrCommandPut( wk, WIN_CHG_STATUS, mes_b_plist_02_504 );
    BPL_StrCommandPut( wk, WIN_CHG_WAZACHECK, mes_b_plist_02_505 );
  }else{
//    BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_STATUS] );
//    BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_WAZACHECK] );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ WIN_CHG_STATUS ].win );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ WIN_CHG_WAZACHECK ].win );
  }
}

#define NAME_CENTER_SPC_SX    ( 8 )

//--------------------------------------------------------------------------------------------
/**
 * ����ւ��y�[�W�̖��O�\��
 *
 * @param wk    ���[�N
 * @param pos   ���шʒu
 *
 * @return  none
 */
//-------------------------------------------------------------------------------------------
static void BPL_IrekaeNamePut( BPLIST_WORK * wk, u32 pos )
{
  GFL_BMPWIN * win;
  BPL_POKEDATA * pd;
  STRBUF * exp;
  STRBUF * str;
  u8  name_siz;
  u8  sex_siz;
  u8  spc_siz;
  u8  px;

  win = wk->add_win[WIN_CHG_NAME].win;
  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,pos) ];
  exp = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, wk->dat->heap );
  str = GFL_MSG_CreateString( wk->mman, NameMsgID_Tbl[pos] );

  WORDSET_RegisterPokeNickName( wk->wset, 0, pd->pp );
  WORDSET_ExpandStr( wk->wset, exp, str );
  GFL_STR_DeleteBuffer( str );

  str = NULL;
  if( pd->sex_put == 0 && pd->egg == 0 ){
    if( pd->sex == PTL_SEX_MALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_502 );
    }else if( pd->sex == PTL_SEX_FEMALE ){
      str = GFL_MSG_CreateString( wk->mman, mes_b_plist_02_503 );
    }
  }

  name_siz = PRINTSYS_GetStrWidth( exp, wk->dat->font, 0 );
  if( str == NULL ){
    sex_siz = 0;
    spc_siz = 0;
  }else{
    sex_siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
    spc_siz = NAME_CENTER_SPC_SX;
  }
  px = (GFL_BMPWIN_GetSizeX(win)*8-name_siz-sex_siz-spc_siz)/2;

//  PRINTSYS_PrintQueColor(
//    wk->que, GFL_BMPWIN_GetBmp(win), px, P_CHG_NAME_PY-1, exp, wk->dat->font, FCOL_P13WN );
  PRINT_UTIL_PrintColor(
    &wk->add_win[WIN_CHG_NAME], wk->que, px, P_CHG_NAME_PY-1, exp, wk->dat->font, FCOL_P09WN );
  GFL_STR_DeleteBuffer( exp );

  if( str != NULL ){
    if( pd->sex == PTL_SEX_MALE ){
//      PRINTSYS_PrintQueColor(
//        wk->que, GFL_BMPWIN_GetBmp(win),
//        px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, PCOL_B_BLUE );
      PRINT_UTIL_PrintColor(
        &wk->add_win[WIN_CHG_NAME], wk->que,
        px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, FCOL_P09BLN );
    }else{
//      PRINTSYS_PrintQueColor(
//        wk->que, GFL_BMPWIN_GetBmp(win),
//        px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, PCOL_B_RED );
      PRINT_UTIL_PrintColor(
        &wk->add_win[WIN_CHG_NAME], wk->que,
        px+name_siz+spc_siz, P_CHG_NAME_PY, str, wk->dat->font, FCOL_P09RN );
    }
    GFL_STR_DeleteBuffer( str );
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_CHG_NAME] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_CHG_NAME] );
}




#define STW_NAME_PX   ( 0 )
#define STW_NAME_PY   ( 0 )
#define STW_WAZANAME_PY ( 8-1 )
#define P2_PPNUM_PX   ( 0 )
#define P2_PPNUM_PY   ( 0 )
#define P2_PP_PX    ( 0 )
#define P2_PP_PY    ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�I���y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaSelPageBmpWrite( BPLIST_WORK * wk )
{
  BPL_POKEWAZA * waza;
  u16 i;

  wk->putWin = WazaSelPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL1].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL2].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL3].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_SKILL4].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_STW_STATUS].win), 0 );

  BPL_NamePut( wk, WIN_STW_NAME, wk->dat->sel_poke, STW_NAME_PX, STW_NAME_PY );

  for( i=0; i<4; i++ ){
    waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[i];
    if( waza->id == 0 ){
      wk->add_win[WIN_STW_SKILL1+i].transReq = TRUE;
//      GFL_BMPWIN_TransVramCharacter( wk->add_win[WIN_STW_SKILL1+i].win );
      continue;
    }

    BPL_WazaNamePut(
      wk, waza->id, WIN_STW_SKILL1+i,
      WazaMsgID_Tbl[i], STW_WAZANAME_PY, FCOL_P09WN );

    BPL_WazaButtonPPPut( wk, waza, WIN_STW_SKILL1+i );
  }

  BPL_StrCommandPut( wk, WIN_STW_STATUS, mes_b_plist_02_504 );

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_STW_SKILL1] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_STW_SKILL2] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_STW_SKILL3] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_STW_SKILL4] );

//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL1] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL2] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL3] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_STW_SKILL4] );
}

#define P3_NAME_PX    ( 0 )
#define P3_NAME_PY    ( 0 )
#define P3_HPGAGE_PX  ( 0 )
#define P3_HPGAGE_PY  ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X���C���y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StMainPageBmpWrite( BPLIST_WORK * wk )
{
  wk->putWin = StMainPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LV].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXT].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POW].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEF].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGI].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPD].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_WAZACHECK].win), 0 );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPGAGE].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_LVNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_NEXTNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_POWNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_DEFNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_AGINUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPPNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPDNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_HPNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPANAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_SPAINFO].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P3_ITEMNAME].win), 0 );

  BPL_NamePut( wk, WIN_P3_NAME, wk->dat->sel_poke, P3_NAME_PX, P3_NAME_PY );
  BPL_P3_HPPut( wk, wk->dat->sel_poke );
  BPL_HPGagePut( wk, WIN_P3_HPGAGE, wk->dat->sel_poke, P3_HPGAGE_PX, P3_HPGAGE_PY );
  GFL_BMPWIN_TransVramCharacter( wk->add_win[WIN_P3_HPGAGE].win );
  BPL_P3_LvPut( wk, wk->dat->sel_poke );
  BPL_P3_PowPut( wk, wk->dat->sel_poke );
  BPL_P3_DefPut( wk, wk->dat->sel_poke );
  BPL_P3_AgiPut( wk, wk->dat->sel_poke );
  BPL_P3_SppPut( wk, wk->dat->sel_poke );
  BPL_P3_SpdPut( wk, wk->dat->sel_poke );
  BPL_TokuseiPut( wk, WIN_P3_SPANAME, wk->dat->sel_poke );
  BPL_ItemPut( wk, WIN_P3_ITEMNAME, wk->dat->sel_poke );
  BPL_P3_TokuseiInfoPut( wk, wk->dat->sel_poke );
  BPL_StrCommandPut( wk, WIN_P3_WAZACHECK, mes_b_plist_02_505 );
}

#define P4_NAME_PX    ( 0 )
#define P4_NAME_PY    ( 0 )
#define P4_WAZANAME_PY  ( 0 )
#define P4_PP_PX    ( 0 )
#define P4_PP_PY    ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�Z�ڍ׃y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_StWazaInfoPageBmpWrite( BPLIST_WORK * wk )
{
  BPL_POKEWAZA * waza;

  wk->putWin = WazaInfoPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_PP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_HIT].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_POW].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_BUNRUI].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_PPNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_SKILL].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_HITNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_POWNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_BRNAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P4_INFO].win), 0 );

  waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[wk->dat->sel_wp];

  BPL_NamePut( wk, WIN_P4_NAME, wk->dat->sel_poke, P4_NAME_PX, P4_NAME_PY );
  BPL_PPPut( wk, WIN_P4_PP, P4_PP_PX, P4_PP_PY );
  BPL_WazaNamePut(
    wk, waza->id, WIN_P4_SKILL,
    WazaMsgID_Tbl[wk->dat->sel_wp], P4_WAZANAME_PY, FCOL_P13WN );
  BPL_WazaHitStrPut( wk, WIN_P4_HIT );
  BPL_WazaHitNumPut( wk, WIN_P4_HITNUM, waza->hit );
  BPL_WazaPowStrPut( wk, WIN_P4_POW );
  BPL_WazaPowNumPut( wk, WIN_P4_POWNUM, waza->pow );
  BPL_WazaInfoPut( wk, WIN_P4_INFO, waza->id );
  BPL_WazaBunruiStrPut( wk, WIN_P4_BUNRUI );
  BPL_WazaKindPut( wk, WIN_P4_BRNAME, waza->kind );
  BPL_WazaPPPut( wk, WIN_P4_PPNUM, waza->pp, waza->mpp );
}

#define P5_NAME_PX    ( 0 )
#define P5_NAME_PY    ( 0 )
#define P5_WAZANAME_PY  ( 8-1 )

//--------------------------------------------------------------------------------------------
/**
 * �Z�Y��I���y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_WazaDelSelPageBmpWrite( BPLIST_WORK * wk )
{
  BPL_POKEWAZA * waza;
  u32 i;

  wk->putWin = WazaDelPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL1].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL2].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL3].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL4].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P5_SKILL5].win), 0 );

  BPL_NamePut( wk, WIN_P5_NAME, wk->dat->sel_poke, P5_NAME_PX, P5_NAME_PY );

  for( i=0; i<4; i++ ){
    waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[i];
    if( waza->id == 0 ){ continue; }

    BPL_WazaNamePut(
      wk, waza->id, WIN_P5_SKILL1+i,
      WazaMsgID_Tbl[i], P5_WAZANAME_PY, FCOL_P09WN );

    BPL_WazaButtonPPPut( wk, waza, WIN_P5_SKILL1+i );
  }

  BPL_WazaNamePut(
    wk, wk->dat->chg_waza, WIN_P5_SKILL5,
    WazaMsgID_Tbl[4], P5_WAZANAME_PY, FCOL_P09WN );
  {
    BPL_POKEWAZA  tmp;

    tmp.pp  = WAZADATA_GetParam( wk->dat->chg_waza, WAZAPARAM_BASE_PP );
    tmp.mpp = tmp.pp;
    BPL_WazaButtonPPPut( wk, &tmp, WIN_P5_SKILL5 );
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P5_SKILL1] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P5_SKILL2] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P5_SKILL3] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P5_SKILL4] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P5_SKILL5] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL1] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL2] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL3] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL4] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P5_SKILL5] );
}

#define P6_NAME_PX    ( 0 )
#define P6_NAME_PY    ( 0 )
#define P6_WAZANAME_PY  ( 0 )
#define P6_PP_PX    ( 0 )
#define P6_PP_PY    ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�U��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page6BmpWrite( BPLIST_WORK * wk )
{
  wk->putWin = WazaDelInfoPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_PP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_PPNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_SKILL].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_HIT].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_HITNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_POW].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_POWNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_BUNRUI].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_BRNAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_INFO].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P6_WASURERU].win), 0 );

  BPL_NamePut( wk, WIN_P6_NAME, wk->dat->sel_poke, P6_NAME_PX, P6_NAME_PY );
  BPL_PPPut( wk, WIN_P6_PP, P6_PP_PX, P6_PP_PY );
  BPL_WazaHitStrPut( wk, WIN_P6_HIT );
  BPL_WazaPowStrPut( wk, WIN_P6_POW );
  BPL_WazaBunruiStrPut( wk, WIN_P6_BUNRUI );

  if( wk->dat->sel_wp < 4 ){
    BPL_POKEWAZA * waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[wk->dat->sel_wp];

    BPL_WazaNamePut(
      wk, waza->id, WIN_P6_SKILL,
      WazaMsgID_Tbl[wk->dat->sel_wp], P6_WAZANAME_PY, FCOL_P13WN );
    BPL_WazaHitNumPut( wk, WIN_P6_HITNUM, waza->hit );
    BPL_WazaPowNumPut( wk, WIN_P6_POWNUM, waza->pow );
    BPL_WazaInfoPut( wk, WIN_P6_INFO, waza->id );
    BPL_WazaKindPut( wk, WIN_P6_BRNAME, waza->kind );
    BPL_WazaPPPut( wk, WIN_P6_PPNUM, waza->pp, waza->mpp );
  }else{
    u32 pp = WAZADATA_GetParam( wk->dat->chg_waza, WAZAPARAM_BASE_PP );

    BPL_WazaNamePut(
      wk, wk->dat->chg_waza, WIN_P6_SKILL,
      WazaMsgID_Tbl[4], P6_WAZANAME_PY, FCOL_P13WN );
    BPL_WazaInfoPut( wk, WIN_P6_INFO, wk->dat->chg_waza );
    BPL_WazaHitNumPut(
      wk, WIN_P6_HITNUM, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_HITPER) );
    BPL_WazaPowNumPut(
      wk, WIN_P6_POWNUM, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_POWER) );
    BPL_WazaKindPut(
      wk, WIN_P6_BRNAME, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_DAMAGE_TYPE) );
    BPL_WazaPPPut( wk, WIN_P6_PPNUM, pp, pp );
  }

  BPL_WasureruStrPut( wk, WIN_P6_WASURERU );
}

#define P7_NAME_PX    ( 0 )
#define P7_NAME_PY    ( 0 )
#define P7_WAZANAME_PY  ( 8-1 )

//--------------------------------------------------------------------------------------------
/**
 * �Z�񕜑I���y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_PPRcvPageBmpWrite( BPLIST_WORK * wk )
{
  BPL_POKEWAZA * waza;
  u32 i;

  wk->putWin = WazaRcvPutWin;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL1].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL2].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL3].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P7_SKILL4].win), 0 );

  BPL_NamePut( wk, WIN_P7_NAME, wk->dat->sel_poke, P7_NAME_PX, P7_NAME_PY );

  for( i=0; i<4; i++ ){
    waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[i];
    if( waza->id == 0 ){ continue; }

    BPL_WazaNamePut(
      wk, waza->id, WIN_P7_SKILL1+i,
      WazaMsgID_Tbl[i], P7_WAZANAME_PY, FCOL_P09WN );
    BPL_WazaButtonPPPut( wk, waza, WIN_P7_SKILL1+i );
  }

  if( ITEM_GetParam( wk->dat->item, ITEM_PRM_ALL_PP_RCV, wk->dat->heap ) == 0 ){
    BPL_PokeSelStrPut( wk, mes_b_plist_m19 );
  }

//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P7_SKILL1] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P7_SKILL2] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P7_SKILL3] );
//  BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P7_SKILL4] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL1] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL2] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL3] );
//  BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P7_SKILL4] );
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�񕜑I���y�[�W��BMP�\��
 *
 * @param wk    ���[�N
 * @param widx  �E�B���h�E�C���f�b�N�X
 * @param pos   �Z�ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_PPRcv( BPLIST_WORK * wk, u16 widx, u16 pos )
{
/*
  BPL_POKEWAZA * waza;

  GFL_BMP_Clear( &wk->add_win[ widx ], 0 );
  waza = &wk->poke[wk->dat->sel_poke].waza[pos];
  BPL_WazaNamePut(
    wk, waza->id, widx,
    WazaMsgID_Tbl[pos], FONT_TOUCH, P7_WAZANAME_PY, PCOL_BTN );
  BPL_WazaButtonPPPut( wk, waza, widx );
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->add_win[ widx ] );
*/
  BPL_POKEWAZA * waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[pos];

  BPL_WazaButtonPPRcv( wk, waza, widx );
}


//--------------------------------------------------------------------------------------------
#if 0
#define P8_NAME_PX    ( 0 )
#define P8_NAME_PY    ( 0 )
#define P8_WAZANAME_PY  ( 0 )
#define P8_PP_PX    ( 0 )
#define P8_PP_PY    ( 0 )

//--------------------------------------------------------------------------------------------
/**
 * �y�[�W�W��BMP�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BPL_Page8BmpWrite( BPLIST_WORK * wk )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_NAME].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_PP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_PPNUM].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_SKILL].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_APP].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_INFO].win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_WASURERU].win), 0 );

  BPL_NamePut( wk, WIN_P8_NAME, wk->dat->sel_poke, P8_NAME_PX, P8_NAME_PY );
  BPL_PPPut( wk, WIN_P8_PP, P8_PP_PX, P8_PP_PY );

  {
    STRBUF * str;

    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_04_400 );
//    PRINTSYS_PrintQueColor(
//      wk->que, GFL_BMPWIN_GetBmp(wk->add_win[WIN_P8_APP].win), 0, 0, str, wk->dat->font, PCOL_N_WHITE );
    PRINT_UTIL_PrintColor(
      &wk->add_win[WIN_P8_APP], wk->que, 0, 0, str, wk->dat->font, FCOL_P13WM );
    GFL_STR_DeleteBuffer( str );
//    BAPPTOOL_PrintScreenTrans( &wk->add_win[WIN_P8_APP] );
//    BAPPTOOL_PrintQueOn( &wk->add_win[WIN_P8_APP] );
  }

  if( wk->dat->sel_wp < 4 ){
    BPL_POKEWAZA * waza = &wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[wk->dat->sel_wp];

    BPL_WazaNamePut(
      wk, waza->id, WIN_P8_SKILL,
      WazaMsgID_Tbl[wk->dat->sel_wp], P8_WAZANAME_PY, FCOL_P13WM );
//    BPL_ContestWazaInfoPut( wk, WIN_P8_INFO, waza->id );

/*
    BPL_WazaHitNumPut( wk, WIN_P6_HITNUM, waza->hit );
    BPL_WazaPowNumPut( wk, WIN_P6_POWNUM, waza->pow );
    BPL_WazaKindPut( wk, WIN_P6_BRNAME, waza->kind );
*/
    BPL_WazaPPPut( wk, WIN_P8_PPNUM, waza->pp, waza->mpp );

  }else{
    u32 pp = WAZADATA_GetParam( wk->dat->chg_waza, WAZAPARAM_BASE_PP );

    BPL_WazaNamePut(
      wk, wk->dat->chg_waza, WIN_P8_SKILL,
      WazaMsgID_Tbl[4], P8_WAZANAME_PY, FCOL_P13WM );
//    BPL_ContestWazaInfoPut( wk, WIN_P8_INFO, wk->dat->chg_waza );

/*
    BPL_WazaHitNumPut(
      wk, WIN_P6_HITNUM, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_HITPER) );
    BPL_WazaPowNumPut(
      wk, WIN_P6_POWNUM, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_POWER) );
    BPL_WazaKindPut(
      wk, WIN_P6_BRNAME, WAZADATA_GetParam(wk->dat->chg_waza,WAZAPARAM_DAMAGE_TYPE) );
*/
    BPL_WazaPPPut( wk, WIN_P8_PPNUM, pp, pp );
  }

//  BPL_WasureruStrPut( wk, WIN_P8_WASURERU );
}
#endif


//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\��
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_TalkMsgSet( BPLIST_WORK * wk )
{
//  BmpWinFrame_Write( &wk->win[WIN_TALK], WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
  BmpWinFrame_Write( wk->win[WIN_TALK].win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_TALK]), FBMP_COL_WHITE );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[WIN_TALK].win), 15 );
  BattlePokeList_TalkMsgStart( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�\���J�n
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_TalkMsgStart( BPLIST_WORK * wk )
{
	GFL_FONTSYS_SetColor( 1, 2, 0 );

  wk->stream = PRINTSYS_PrintStream(
                  wk->win[WIN_TALK].win,
                  0, 0, wk->msg_buf, wk->dat->font,
                  MSGSPEED_GetWait(),
                  wk->tcbl,
                  10,   // tcbl pri
                  wk->dat->heap,
                  15 ); // clear color

  GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[WIN_TALK].win );

/*
  MsgPrintSkipFlagSet( MSG_SKIP_ON );
  wk->midx = GF_STR_PrintSimple(
        &wk->win[WIN_TALK], FONT_TALK, wk->msg_buf,
        0, 0, BattleWorkConfigMsgSpeedGet(wk->dat->bw), NULL );
*/
}





#define BPL_RCV_SLEEP   ( 0x01 )  // ����
#define BPL_RCV_POISON    ( 0x02 )  // ��
#define BPL_RCV_BURN    ( 0x04 )  // �Ώ�
#define BPL_RCV_ICE     ( 0x08 )  // �X
#define BPL_RCV_PARALYZE  ( 0x10 )  // ���
#define BPL_RCV_PANIC   ( 0x20 )  // ����
#define BPL_RCV_MEROMERO  ( 0x40 )  // ��������
#define BPL_RCV_ALL     ( 0x7f )  // �S��


//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���g�p���b�Z�[�W�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ItemUseMsgSet( BPLIST_WORK * wk )
{
/*
  POKEMON_PARAM * pp;
  BPLIST_DATA * dat;
  void * item;
  STRBUF * str;
  u16 hp;
  u8  prm;
  u8  i;

  dat  = wk->dat;
  item = ITEM_GetItemArcData( dat->item, ITEM_GET_DATA, dat->heap );
  pp   = BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
  hp   = PokeParaGet( pp, ID_PARA_hp, NULL );
  prm  = 0;
  if( ITEM_GetBufParam( item, ITEM_PRM_SLEEP_RCV ) != 0 ){
    prm |= BPL_RCV_SLEEP;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_POISON_RCV ) != 0 ){
    prm |= BPL_RCV_POISON;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_BURN_RCV ) != 0 ){
    prm |= BPL_RCV_BURN;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_ICE_RCV ) != 0 ){
    prm |= BPL_RCV_ICE;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_PARALYZE_RCV ) != 0 ){
    prm |= BPL_RCV_PARALYZE;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_PANIC_RCV ) != 0 ){
    prm |= BPL_RCV_PANIC;
  }
  if( ITEM_GetBufParam( item, ITEM_PRM_MEROMERO_RCV ) != 0 ){
    prm |= BPL_RCV_MEROMERO;
  }

  // �m����
  if( wk->poke[dat->sel_poke].hp == 0 && hp != 0 ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m13 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // HP��
  }else if( wk->poke[dat->sel_poke].hp != hp ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m07 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_RegisterNumber(
      wk->wset, 1, hp-wk->poke[dat->sel_poke].hp, 3,
      STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // PP��
  }else if( ITEM_GetBufParam( item, ITEM_PRM_PP_RCV ) != 0 ||
    ITEM_GetBufParam( item, ITEM_PRM_ALL_PP_RCV ) != 0 ){

    GFL_MSG_GetString( wk->mman, mes_b_plist_m12, wk->msg_buf );
  // ����
  }else if( prm == BPL_RCV_SLEEP ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m17 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // ��
  }else if( prm == BPL_RCV_POISON ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m08 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // �Ώ�
  }else if( prm == BPL_RCV_BURN ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m10 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // �X
  }else if( prm == BPL_RCV_ICE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m11 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // ���
  }else if( prm == BPL_RCV_PARALYZE ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m09 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // ����
  }else if( prm == BPL_RCV_PANIC ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m15 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // ��������
  }else if( prm == BPL_RCV_MEROMERO ){
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m16 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  // �X�e�[�^�X�S��
//  }else if( prm == BPL_RCV_ALL ){
  }else{
    str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m14 );
    WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
    WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
    GFL_STR_DeleteBuffer( str );
  }

  GFL_HEAP_FreeMemory( item );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�ɂ��A�C�e���g�p�G���[���b�Z�[�W�Z�b�g
 *
 * @param wk    ���[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ItemUseSkillErrMsgSet( BPLIST_WORK * wk )
{
/*
  POKEMON_PARAM * pp;
  BPLIST_DATA * dat;
  STRBUF * str;

  dat = wk->dat;
  pp  = BattleWorkPokemonParamGet( dat->bw, dat->client_no, dat->list_row[dat->sel_poke] );
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m20 );

  WORDSET_RegisterPokeNickName( wk->wset, 0, pp );
  WORDSET_RegisterWazaName( wk->wset, 1, WAZANO_SASIOSAE );
  WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
  GFL_STR_DeleteBuffer( str );
*/
  BPL_POKEDATA * pd;
  BPLIST_DATA * dat;
  STRBUF * str;

  dat = wk->dat;
  pd  = &wk->poke[ BPLISTMAIN_GetListRow(wk,dat->sel_poke) ];
  str = GFL_MSG_CreateString( wk->mman, mes_b_plist_m20 );
  WORDSET_RegisterPokeNickName( wk->wset, 0, pd->pp );
  WORDSET_RegisterWazaName( wk->wset, 1, WAZANO_SASIOSAE );
  WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
  GFL_STR_DeleteBuffer( str );
}



void BPLISTBMP_PrintMain( BPLIST_WORK * wk )
{
  BAPPTOOL_PrintUtilTrans( wk->win, wk->que, WIN_MAX );
  BAPPTOOL_PrintUtilTrans( wk->add_win, wk->que, wk->bmp_add_max );
}

void BPLISTBMP_SetStrScrn( BPLIST_WORK * wk )
{
  BAPPTOOL_SetStrScrn( wk->add_win, wk->putWin );
	BPLISTBMP_SetCommentScrn( wk );
}

void BPLISTBMP_SetCommentScrn( BPLIST_WORK * wk )
{
  if( wk->page_chg_comm == 1 ){
		BmpWinFrame_Write(
			wk->win[WIN_COMMENT].win, WINDOW_TRANS_OFF, TALK_WIN_CGX_POS, BPL_PAL_TALK_WIN );
		BAPPTOOL_PrintScreenTrans( &wk->win[WIN_COMMENT] );
    wk->page_chg_comm = 0;
  }
}
