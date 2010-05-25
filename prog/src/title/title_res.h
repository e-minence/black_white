//======================================================================
/**
 * @file  title_res.h
 * @brief �^�C�g���̃o�[�W�����Ⴂ�z���p
 * @author  ariizumi
 * @data  100515
 *
 * ���W���[�����FTITLE_RES
 */
//======================================================================
#pragma once

#include "pm_version.h"

#define TITLE_RES_ARCID (ARCID_TITLE)

#if PM_VERSION == VERSION_BLACK

#include "title_black.naix"

// ���S(BG)
#define TITLE_RES_LOGO_NCLR (NARC_title_black_blk_logo_NCLR)
#define TITLE_RES_LOGO_NCGR (NARC_title_black_blk_logo_lz_NCGR)
#define TITLE_RES_LOGO_NSCR (NARC_title_black_blk_logo_lz_NSCR)
#define TITLE_RES_BACK_NSCR (NARC_title_black_blk_base_lz_NSCR)

// �uPUSH START BUTTON�v�i�Z���A�N�^�[�j
#define TITLE_RES_PUSH_NCLR (NARC_title_black_blk_obj_NCLR)
#define TITLE_RES_PUSH_NCGR (NARC_title_black_blk_obj_lz_NCGR)
#define TITLE_RES_PUSH_NCER (NARC_title_black_blk_obj_NCER)
#define TITLE_RES_PUSH_NANR (NARC_title_black_blk_obj_NANR)

// �uDeveloped by GameFreak�v(BG)
#define TITLE_RES_DEV_NCLR (NARC_title_black_blk_develop_NCLR)
#define TITLE_RES_DEV_NCGR (NARC_title_black_blk_develop_lz_NCGR)
#define TITLE_RES_DEV_NSCR (NARC_title_black_blk_develop_lz_NSCR)

#else

#include "title_white.naix"

// ���S
#define TITLE_RES_LOGO_NCLR (NARC_title_white_wht_logo_NCLR)
#define TITLE_RES_LOGO_NCGR (NARC_title_white_wht_logo_lz_NCGR)
#define TITLE_RES_LOGO_NSCR (NARC_title_white_wht_logo_lz_NSCR)
#define TITLE_RES_BACK_NSCR (NARC_title_white_wht_base_lz_NSCR)

// �uPUSH START BUTTON�v
#define TITLE_RES_PUSH_NCLR (NARC_title_white_wht_obj_NCLR)
#define TITLE_RES_PUSH_NCGR (NARC_title_white_wht_obj_lz_NCGR)
#define TITLE_RES_PUSH_NCER (NARC_title_white_wht_obj_NCER)
#define TITLE_RES_PUSH_NANR (NARC_title_white_wht_obj_NANR)

// �uDeveloped by GameFreak�v(BG)
#define TITLE_RES_DEV_NCLR (NARC_title_white_wht_develop_NCLR)
#define TITLE_RES_DEV_NCGR (NARC_title_white_wht_develop_lz_NCGR)
#define TITLE_RES_DEV_NSCR (NARC_title_white_wht_develop_lz_NSCR)


#endif