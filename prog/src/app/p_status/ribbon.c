//============================================================================================
/**
 * @file  ribbon.c
 * @brief ���{���f�[�^�֘A
 * @author  Hiroyuki Nakamura
 * @date  2006.04.18
 */
//============================================================================================
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "p_status_gra.naix"
#include "msg/msg_ribbon.h"

#include "ribbon.h"


//============================================================================================
//  �萔��`
//============================================================================================
typedef struct {
  u32 pp_id;    // PokeParaGet�̎擾ID
  u16 graphic;  // �O���t�B�b�NID
  u16 pal_num;  // �p���b�g�ԍ�
  u16 name;   // ���{����
  u16 info;   // ���{������
  u8  category; //���{�����
}RIBBON_TABLE;


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================
// ���{���f�[�^
static const RIBBON_TABLE RibbonTable[] =
{
	{ID_PARA_champ_ribbon,	NARC_p_status_gra_ribbon_league_00_NCGR,	0,	mes_ribbon_name_league_001,	mes_ribbon_text_league_001,	RIBBON_CATEGORY_LEAGUE	},	//	�z�E�G��
	{ID_PARA_sinou_champ_ribbon,	NARC_p_status_gra_ribbon_league_01_NCGR,	0,	mes_ribbon_name_league_002,	mes_ribbon_text_league_002,	RIBBON_CATEGORY_LEAGUE	},	//	�V���I�E
	{ID_PARA_stylemedal_normal,	NARC_p_status_gra_ribbon_contest_00_NCGR,	0,	mes_ribbon_name_contest_001,	mes_ribbon_text_contest_001,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{��
	{ID_PARA_stylemedal_super,	NARC_p_status_gra_ribbon_contest_01_NCGR,	0,	mes_ribbon_name_contest_002,	mes_ribbon_text_contest_002,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���X�[�p�[
	{ID_PARA_stylemedal_hyper,	NARC_p_status_gra_ribbon_contest_02_NCGR,	0,	mes_ribbon_name_contest_003,	mes_ribbon_text_contest_003,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���n�C�p�[
	{ID_PARA_stylemedal_master,	NARC_p_status_gra_ribbon_contest_03_NCGR,	0,	mes_ribbon_name_contest_004,	mes_ribbon_text_contest_004,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���}�X�^�[
	{ID_PARA_beautifulmedal_normal,	NARC_p_status_gra_ribbon_contest_00_NCGR,	1,	mes_ribbon_name_contest_005,	mes_ribbon_text_contest_005,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{��
	{ID_PARA_beautifulmedal_super,	NARC_p_status_gra_ribbon_contest_01_NCGR,	1,	mes_ribbon_name_contest_006,	mes_ribbon_text_contest_006,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���X�[�p�[
	{ID_PARA_beautifulmedal_hyper,	NARC_p_status_gra_ribbon_contest_02_NCGR,	1,	mes_ribbon_name_contest_007,	mes_ribbon_text_contest_007,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���n�C�p�[
	{ID_PARA_beautifulmedal_master,	NARC_p_status_gra_ribbon_contest_03_NCGR,	1,	mes_ribbon_name_contest_008,	mes_ribbon_text_contest_008,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���}�X�^�[
	{ID_PARA_cutemedal_normal,	NARC_p_status_gra_ribbon_contest_00_NCGR,	2,	mes_ribbon_name_contest_009,	mes_ribbon_text_contest_009,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{��
	{ID_PARA_cutemedal_super,	NARC_p_status_gra_ribbon_contest_01_NCGR,	2,	mes_ribbon_name_contest_010,	mes_ribbon_text_contest_010,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���X�[�p�[
	{ID_PARA_cutemedal_hyper,	NARC_p_status_gra_ribbon_contest_02_NCGR,	2,	mes_ribbon_name_contest_011,	mes_ribbon_text_contest_011,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���n�C�p�[
	{ID_PARA_cutemedal_master,	NARC_p_status_gra_ribbon_contest_03_NCGR,	2,	mes_ribbon_name_contest_012,	mes_ribbon_text_contest_012,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���}�X�^�[
	{ID_PARA_clevermedal_normal,	NARC_p_status_gra_ribbon_contest_00_NCGR,	3,	mes_ribbon_name_contest_013,	mes_ribbon_text_contest_013,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{��
	{ID_PARA_clevermedal_super,	NARC_p_status_gra_ribbon_contest_01_NCGR,	3,	mes_ribbon_name_contest_014,	mes_ribbon_text_contest_014,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���X�[�p�[
	{ID_PARA_clevermedal_hyper,	NARC_p_status_gra_ribbon_contest_02_NCGR,	3,	mes_ribbon_name_contest_015,	mes_ribbon_text_contest_015,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���n�C�p�[
	{ID_PARA_clevermedal_master,	NARC_p_status_gra_ribbon_contest_03_NCGR,	3,	mes_ribbon_name_contest_016,	mes_ribbon_text_contest_016,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���}�X�^�[
	{ID_PARA_strongmedal_normal,	NARC_p_status_gra_ribbon_contest_00_NCGR,	4,	mes_ribbon_name_contest_017,	mes_ribbon_text_contest_017,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{��
	{ID_PARA_strongmedal_super,	NARC_p_status_gra_ribbon_contest_01_NCGR,	4,	mes_ribbon_name_contest_018,	mes_ribbon_text_contest_018,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���X�[�p�[
	{ID_PARA_strongmedal_hyper,	NARC_p_status_gra_ribbon_contest_02_NCGR,	4,	mes_ribbon_name_contest_019,	mes_ribbon_text_contest_019,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���n�C�p�[
	{ID_PARA_strongmedal_master,	NARC_p_status_gra_ribbon_contest_03_NCGR,	4,	mes_ribbon_name_contest_020,	mes_ribbon_text_contest_020,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���}�X�^�[
	{ID_PARA_trial_stylemedal_normal,	NARC_p_status_gra_ribbon_contest_04_NCGR,	0,	mes_ribbon_name_contest_021,	mes_ribbon_text_contest_021,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{��
	{ID_PARA_trial_stylemedal_super,	NARC_p_status_gra_ribbon_contest_05_NCGR,	0,	mes_ribbon_name_contest_022,	mes_ribbon_text_contest_022,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���O���[�g
	{ID_PARA_trial_stylemedal_hyper,	NARC_p_status_gra_ribbon_contest_06_NCGR,	0,	mes_ribbon_name_contest_023,	mes_ribbon_text_contest_023,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���E���g��
	{ID_PARA_trial_stylemedal_master,	NARC_p_status_gra_ribbon_contest_07_NCGR,	0,	mes_ribbon_name_contest_024,	mes_ribbon_text_contest_024,	RIBBON_CATEGORY_CONTEST	},	//	�N�[�����{���}�X�^�[
	{ID_PARA_trial_beautifulmedal_normal,	NARC_p_status_gra_ribbon_contest_04_NCGR,	1,	mes_ribbon_name_contest_025,	mes_ribbon_text_contest_025,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{��
	{ID_PARA_trial_beautifulmedal_super,	NARC_p_status_gra_ribbon_contest_05_NCGR,	1,	mes_ribbon_name_contest_026,	mes_ribbon_text_contest_026,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���O���[�g
	{ID_PARA_trial_beautifulmedal_hyper,	NARC_p_status_gra_ribbon_contest_06_NCGR,	1,	mes_ribbon_name_contest_027,	mes_ribbon_text_contest_027,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���E���g��
	{ID_PARA_trial_beautifulmedal_master,	NARC_p_status_gra_ribbon_contest_07_NCGR,	1,	mes_ribbon_name_contest_028,	mes_ribbon_text_contest_028,	RIBBON_CATEGORY_CONTEST	},	//	�r���[�e�B���{���}�X�^�[
	{ID_PARA_trial_cutemedal_normal,	NARC_p_status_gra_ribbon_contest_04_NCGR,	2,	mes_ribbon_name_contest_029,	mes_ribbon_text_contest_029,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{��
	{ID_PARA_trial_cutemedal_super,	NARC_p_status_gra_ribbon_contest_05_NCGR,	2,	mes_ribbon_name_contest_030,	mes_ribbon_text_contest_030,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���O���[�g
	{ID_PARA_trial_cutemedal_hyper,	NARC_p_status_gra_ribbon_contest_06_NCGR,	2,	mes_ribbon_name_contest_031,	mes_ribbon_text_contest_031,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���E���g��
	{ID_PARA_trial_cutemedal_master,	NARC_p_status_gra_ribbon_contest_07_NCGR,	2,	mes_ribbon_name_contest_032,	mes_ribbon_text_contest_032,	RIBBON_CATEGORY_CONTEST	},	//	�L���[�g���{���}�X�^�[
	{ID_PARA_trial_clevermedal_normal,	NARC_p_status_gra_ribbon_contest_04_NCGR,	3,	mes_ribbon_name_contest_033,	mes_ribbon_text_contest_033,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{��
	{ID_PARA_trial_clevermedal_super,	NARC_p_status_gra_ribbon_contest_05_NCGR,	3,	mes_ribbon_name_contest_034,	mes_ribbon_text_contest_034,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���O���[�g
	{ID_PARA_trial_clevermedal_hyper,	NARC_p_status_gra_ribbon_contest_06_NCGR,	3,	mes_ribbon_name_contest_035,	mes_ribbon_text_contest_035,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���E���g��
	{ID_PARA_trial_clevermedal_master,	NARC_p_status_gra_ribbon_contest_07_NCGR,	3,	mes_ribbon_name_contest_036,	mes_ribbon_text_contest_036,	RIBBON_CATEGORY_CONTEST	},	//	�W�[�j�A�X���{���}�X�^�[
	{ID_PARA_trial_strongmedal_normal,	NARC_p_status_gra_ribbon_contest_04_NCGR,	4,	mes_ribbon_name_contest_037,	mes_ribbon_text_contest_037,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{��
	{ID_PARA_trial_strongmedal_super,	NARC_p_status_gra_ribbon_contest_05_NCGR,	4,	mes_ribbon_name_contest_038,	mes_ribbon_text_contest_038,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���O���[�g
	{ID_PARA_trial_strongmedal_hyper,	NARC_p_status_gra_ribbon_contest_06_NCGR,	4,	mes_ribbon_name_contest_039,	mes_ribbon_text_contest_039,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���E���g��
	{ID_PARA_trial_strongmedal_master,	NARC_p_status_gra_ribbon_contest_07_NCGR,	4,	mes_ribbon_name_contest_040,	mes_ribbon_text_contest_040,	RIBBON_CATEGORY_CONTEST	},	//	�p���t�����{���}�X�^�[
	{ID_PARA_winning_ribbon,	NARC_p_status_gra_ribbon_tower_00_NCGR,	0,	mes_ribbon_name_tower_001,	mes_ribbon_text_tower_001,	RIBBON_CATEGORY_TOWER	},	//	�E�C�j���O���{��
	{ID_PARA_victory_ribbon,	NARC_p_status_gra_ribbon_tower_01_NCGR,	0,	mes_ribbon_name_tower_002,	mes_ribbon_text_tower_002,	RIBBON_CATEGORY_TOWER	},	//	�r�N�g���[���{��
	{ID_PARA_sinou_battle_tower_ttwin_first,	NARC_p_status_gra_ribbon_tower_02_NCGR,	0,	mes_ribbon_name_tower_003,	mes_ribbon_text_tower_003,	RIBBON_CATEGORY_TOWER	},	//	�A�r���e�B���{��
	{ID_PARA_sinou_battle_tower_ttwin_second,	NARC_p_status_gra_ribbon_tower_03_NCGR,	0,	mes_ribbon_name_tower_004,	mes_ribbon_text_tower_004,	RIBBON_CATEGORY_TOWER	},	//	�O���[�g�A�r���e�B���{��
	{ID_PARA_sinou_battle_tower_2vs2_win50,	NARC_p_status_gra_ribbon_tower_04_NCGR,	0,	mes_ribbon_name_tower_005,	mes_ribbon_text_tower_005,	RIBBON_CATEGORY_TOWER	},	//	�_�u���A�r���e�B���{��
	{ID_PARA_sinou_battle_tower_aimulti_win50,	NARC_p_status_gra_ribbon_tower_05_NCGR,	0,	mes_ribbon_name_tower_006,	mes_ribbon_text_tower_006,	RIBBON_CATEGORY_TOWER	},	//	�}���`�A�r���e�B���{��
	{ID_PARA_sinou_battle_tower_siomulti_win50,	NARC_p_status_gra_ribbon_tower_06_NCGR,	0,	mes_ribbon_name_tower_007,	mes_ribbon_text_tower_007,	RIBBON_CATEGORY_TOWER	},	//	�y�A�A�r���e�B���{��
	{ID_PARA_sinou_battle_tower_wifi_rank5,	NARC_p_status_gra_ribbon_tower_07_NCGR,	0,	mes_ribbon_name_tower_008,	mes_ribbon_text_tower_008,	RIBBON_CATEGORY_TOWER	},	//	���[���h�A�r���e�B���{��
	{ID_PARA_bromide_ribbon,	NARC_p_status_gra_ribbon_memory_00_NCGR,	1,	mes_ribbon_name_memory_001,	mes_ribbon_text_memory_001,	RIBBON_CATEGORY_MEMORY	},	//	�u���}�C�h���{��
	{ID_PARA_ganba_ribbon,	NARC_p_status_gra_ribbon_memory_01_NCGR,	2,	mes_ribbon_name_memory_002,	mes_ribbon_text_memory_002,	RIBBON_CATEGORY_MEMORY	},	//	����΃��{��
	{ID_PARA_sinou_syakki_ribbon,	NARC_p_status_gra_ribbon_memory_02_NCGR,	2,	mes_ribbon_name_memory_003,	mes_ribbon_text_memory_003,	RIBBON_CATEGORY_MEMORY	},	//	����������{��
	{ID_PARA_sinou_dokki_ribbon,	NARC_p_status_gra_ribbon_memory_03_NCGR,	0,	mes_ribbon_name_memory_004,	mes_ribbon_text_memory_004,	RIBBON_CATEGORY_MEMORY	},	//	�ǂ������{��
	{ID_PARA_sinou_syonbo_ribbon,	NARC_p_status_gra_ribbon_memory_04_NCGR,	1,	mes_ribbon_name_memory_005,	mes_ribbon_text_memory_005,	RIBBON_CATEGORY_MEMORY	},	//	�����ڃ��{��
	{ID_PARA_sinou_ukka_ribbon,	NARC_p_status_gra_ribbon_memory_05_NCGR,	2,	mes_ribbon_name_memory_006,	mes_ribbon_text_memory_006,	RIBBON_CATEGORY_MEMORY	},	//	���������{��
	{ID_PARA_sinou_sukki_ribbon,	NARC_p_status_gra_ribbon_memory_06_NCGR,	3,	mes_ribbon_name_memory_007,	mes_ribbon_text_memory_007,	RIBBON_CATEGORY_MEMORY	},	//	���������{��
	{ID_PARA_sinou_gussu_ribbon,	NARC_p_status_gra_ribbon_memory_07_NCGR,	0,	mes_ribbon_name_memory_008,	mes_ribbon_text_memory_008,	RIBBON_CATEGORY_MEMORY	},	//	���������{��
	{ID_PARA_sinou_nikko_ribbon,	NARC_p_status_gra_ribbon_memory_08_NCGR,	2,	mes_ribbon_name_memory_009,	mes_ribbon_text_memory_009,	RIBBON_CATEGORY_MEMORY	},	//	�ɂ������{��
	{ID_PARA_sinou_gorgeous_ribbon,	NARC_p_status_gra_ribbon_memory_09_NCGR,	1,	mes_ribbon_name_memory_010,	mes_ribbon_text_memory_010,	RIBBON_CATEGORY_MEMORY	},	//	�S�[�W���X���{��
	{ID_PARA_sinou_royal_ribbon,	NARC_p_status_gra_ribbon_memory_10_NCGR,	3,	mes_ribbon_name_memory_011,	mes_ribbon_text_memory_011,	RIBBON_CATEGORY_MEMORY	},	//	���C�������{��
	{ID_PARA_sinou_gorgeousroyal_ribbon,	NARC_p_status_gra_ribbon_memory_11_NCGR,	0,	mes_ribbon_name_memory_012,	mes_ribbon_text_memory_012,	RIBBON_CATEGORY_MEMORY	},	//	�S�[�W���X���C�������{��
	{ID_PARA_sinou_ashiato_ribbon,	NARC_p_status_gra_ribbon_memory_12_NCGR,	0,	mes_ribbon_name_memory_013,	mes_ribbon_text_memory_013,	RIBBON_CATEGORY_MEMORY	},	//	�������ƃ��{��
	{ID_PARA_sinou_record_ribbon,	NARC_p_status_gra_ribbon_memory_13_NCGR,	1,	mes_ribbon_name_memory_014,	mes_ribbon_text_memory_014,	RIBBON_CATEGORY_MEMORY	},	//	���R�[�h���{��
	{ID_PARA_sinou_legend_ribbon,	NARC_p_status_gra_ribbon_memory_14_NCGR,	0,	mes_ribbon_name_memory_015,	mes_ribbon_text_memory_015,	RIBBON_CATEGORY_MEMORY	},	//	���W�F���h���{��
	{ID_PARA_country_ribbon,	NARC_p_status_gra_ribbon_present_00_NCGR,	3,	mes_ribbon_name_present_001,	mes_ribbon_text_present_001,	RIBBON_CATEGORY_PRESENT	},	//	�J���g���[���{��
	{ID_PARA_national_ribbon,	NARC_p_status_gra_ribbon_present_00_NCGR,	4,	mes_ribbon_name_present_002,	mes_ribbon_text_present_002,	RIBBON_CATEGORY_PRESENT	},	//	�i�V���i�����{��
	{ID_PARA_earth_ribbon,	NARC_p_status_gra_ribbon_present_01_NCGR,	0,	mes_ribbon_name_present_003,	mes_ribbon_text_present_003,	RIBBON_CATEGORY_PRESENT	},	//	�A�[�X���{��
	{ID_PARA_world_ribbon,	NARC_p_status_gra_ribbon_present_01_NCGR,	1,	mes_ribbon_name_present_004,	mes_ribbon_text_present_004,	RIBBON_CATEGORY_PRESENT	},	//	���[���h���{��
	{ID_PARA_sinou_classic_ribbon,	NARC_p_status_gra_ribbon_present_02_NCGR,	1,	mes_ribbon_name_present_005,	mes_ribbon_text_present_005,	RIBBON_CATEGORY_PRESENT	},	//	�N���V�b�N���{��
	{ID_PARA_sinou_premiere_ribbon,	NARC_p_status_gra_ribbon_present_03_NCGR,	0,	mes_ribbon_name_present_006,	mes_ribbon_text_present_006,	RIBBON_CATEGORY_PRESENT	},	//	�v���~�A���{��
	{ID_PARA_sinou_history_ribbon,	NARC_p_status_gra_ribbon_present_04_NCGR,	3,	mes_ribbon_name_present_007,	mes_ribbon_text_present_007,	RIBBON_CATEGORY_PRESENT	},	//	�C�x���g���{��
	{ID_PARA_sinou_green_ribbon,	NARC_p_status_gra_ribbon_present_05_NCGR,	2,	mes_ribbon_name_present_008,	mes_ribbon_text_present_008,	RIBBON_CATEGORY_PRESENT	},	//	�o�[�X�f�[���{��
	{ID_PARA_sinou_blue_ribbon,	NARC_p_status_gra_ribbon_present_06_NCGR,	2,	mes_ribbon_name_present_009,	mes_ribbon_text_present_009,	RIBBON_CATEGORY_PRESENT	},	//	�X�y�V�������{��
	{ID_PARA_sinou_festival_ribbon,	NARC_p_status_gra_ribbon_present_07_NCGR,	1,	mes_ribbon_name_present_010,	mes_ribbon_text_present_010,	RIBBON_CATEGORY_PRESENT	},	//	�������A�����{��
	{ID_PARA_sinou_carnival_ribbon,	NARC_p_status_gra_ribbon_present_08_NCGR,	0,	mes_ribbon_name_present_011,	mes_ribbon_text_present_011,	RIBBON_CATEGORY_PRESENT	},	//	�E�B�b�V�����{��
	{ID_PARA_marine_ribbon,	NARC_p_status_gra_ribbon_present_09_NCGR,	3,	mes_ribbon_name_present_012,	mes_ribbon_text_present_012,	RIBBON_CATEGORY_PRESENT	},	//	�o�g���`�����v���{��
	{ID_PARA_land_ribbon,	NARC_p_status_gra_ribbon_present_10_NCGR,	3,	mes_ribbon_name_present_013,	mes_ribbon_text_present_013,	RIBBON_CATEGORY_PRESENT	},	//	�G���A�`�����v���{��
	{ID_PARA_sky_ribbon,	NARC_p_status_gra_ribbon_present_11_NCGR,	1,	mes_ribbon_name_present_014,	mes_ribbon_text_present_014,	RIBBON_CATEGORY_PRESENT	},	//	�i�V���i���`�����v���{��
	{ID_PARA_sinou_red_ribbon,	NARC_p_status_gra_ribbon_present_12_NCGR,	1,	mes_ribbon_name_present_015,	mes_ribbon_text_present_015,	RIBBON_CATEGORY_PRESENT	},	//	���[���h�`�����v���{��
};



//--------------------------------------------------------------------------------------------
/**
 * ���{���f�[�^�擾
 *
 * @param dat_id    �f�[�^�ԍ�
 * @param prm_id    �擾�p�����[�^ID
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
u32 RIBBON_DataGet( u8 dat_id, u8 prm_id )
{
  switch( prm_id ){
  case RIBBON_PARA_POKEPARA:  // PokeParaGet�̎擾ID
    return RibbonTable[dat_id].pp_id;

  case RIBBON_PARA_GRAPHIC: // �O���t�B�b�NID
    return RibbonTable[dat_id].graphic;

  case RIBBON_PARA_PALNUM:  // �p���b�g�ԍ�
    return RibbonTable[dat_id].pal_num;

  case RIBBON_PARA_NAME:    // ���{����
    return RibbonTable[dat_id].name;

  case RIBBON_PARA_INFO:    // ���{������
    return RibbonTable[dat_id].info;

  case RIBBON_PARA_CATEGORY:    // ���{������
    return RibbonTable[dat_id].category;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���{�������擾
 *
 * @param sv      �Z�[�u�f�[�^
 * @param dat_id    �f�[�^�ԍ�
 *
 * @return  ���{���������b�Z�[�W�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 RIBBON_InfoGet( u8 dat_id )
{
  /*
  if( RibbonTable[dat_id].info & MSG_SP_RIBBON ){
    return ( mes_ribbon_haihu_text_000 + sv[ RibbonTable[dat_id].info & 0xff ] );
  }
  */
  return RibbonTable[dat_id].info;
}

//--------------------------------------------------------------------------------------------
/**
 * ���{��ID����������i�[����Ă���ʒu���擾
 *
 * @param dat_id    �f�[�^�ԍ�
 *
 * @retval  "0xff = �z�z���{���ȊO"
 * @retval  "0xff != �i�[�ʒu"
 */
//--------------------------------------------------------------------------------------------
u8 RIBBON_SaveIndexGet( u8 dat_id )
{
  if( RibbonTable[dat_id].info & MSG_SP_RIBBON ){
    return ( RibbonTable[dat_id].info & 0xff );
  }
  return 0xff;
}

//--------------------------------------------------------------------------------------------
/**
 * ID_PARA_���{������msgID���擾(08.06.03)  //  PLATINUM MERGE
 *
 * @param para_id   ID_PARA_���{��
 *
 * @retval  "msgID�A0xffff = ������Ȃ�����"
 */
//--------------------------------------------------------------------------------------------
u16 RIBBON_NameGet( u32 pp_id )
{
  int i;

  for( i=0; i < NELEMS(RibbonTable) ;i++ ){

    if( RibbonTable[i].pp_id == pp_id ){
      return ( RibbonTable[i].name );
    }

  }
  return 0xffff;
}


