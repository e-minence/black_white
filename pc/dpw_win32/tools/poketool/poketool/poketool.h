/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     poketool.h

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*! @file
	@brief	�|�P�����f�[�^����舵�����߂̃��C�u����
	
	@author	kitayama(kitayama_shigetoshi@nintendo.co.jp)
	
	@version 0.03 (2006/09/13)
		@li	B_TOWER_POKEMON::form_no ��ǉ����܂����B
		@li PokeTool_GetPokeParam() �Ńt�H�[���i���o�[���擾�ł���悤�ɂ��܂����B
			�������A�����X�^�[�i���o�[493�̃A���Z�E�X�����͎擾�ł��܂���B
			�i����ł̓A�T�[�g�ɂȂ�܂��̂ŁA�A���Z�E�X�ɑ΂��ăt�H�[���i���o�[���擾
			���Ȃ��ł��������B�j
		@li	PokeTool_IsRare(), PokeTool_BtIsRare(), PokeTool_HasMail(), PokeTool_GetMailLang()
			��ǉ����܂����B
		@li PokeTool_GetDefaultName(), PokeTool_SetNickNameDefault() ��ǉ����܂����B
			�������A�����͌���UNIX���ł͐���ɓ��삵�܂���B
	@version 0.01 (2006/07/20)
		@li �V�K�����[�X���܂����B
*/

#ifndef POKETOOL_H_
#define POKETOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dpw_typedef.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

/*======== �|�P�����̃\�[�X����R�s�[���Ă����A�|�P�����̃p�����[�^��\���^ ========*/

typedef u16		STRCODE;
typedef	u16		PMS_WORD;

typedef struct _CUSTOM_BALL_SEAL	CB_SEAL;	// �V�[���̃f�[�^
typedef struct _CUSTOM_BALL_CORE	CB_CORE;	// �J�X�^���{�[���̃f�[�^

typedef struct pokemon_paso_param1 POKEMON_PASO_PARAM1;
typedef struct pokemon_paso_param2 POKEMON_PASO_PARAM2;
typedef struct pokemon_paso_param3 POKEMON_PASO_PARAM3;
typedef struct pokemon_paso_param4 POKEMON_PASO_PARAM4;

// �{�b�N�X�|�P�������̍\����
typedef struct pokemon_paso_param POKEMON_PASO_PARAM;

//�{�b�N�X�|�P�������ȊO�̍\����
typedef struct pokemon_calc_param POKEMON_CALC_PARAM;

//! �莝���|�P�����̍\����
typedef struct pokemon_param POKEMON_PARAM;

//! �o�g���^���[�Ŏg�p�����|�P�����̃f�[�^
typedef struct _B_TOWER_POKEMON	B_TOWER_POKEMON;

typedef struct _MAIL_DATA MAIL_DATA;

#define	EOM_SIZE			1		//!< �I���R�[�h�̒���
#define	EOM_			0xffff		//!< �I���R�[�h

#define	WAZA_TEMOTI_MAX		(4)		//!< 1�̂̃|�P���������Ă�Z�̍ő�l
#define MONS_NAME_SIZE		10		//!< �|�P�������̒���(�o�b�t�@�T�C�Y EOM_�܂܂�)
#define PERSON_NAME_SIZE	7		//!< �l���̖��O�̒����i�������܂ށj

#define MAILDAT_ICONMAX		(3)		//!< �|�P�����A�C�R���v�f��
#define PMS_WORD_MAX		(2)		//!< �����Ɋ܂܂��P��̍ő吔
#define MAILDAT_MSGMAX		(3)		//!< �ȈՕ��v�f��

#define CB_DEF_SEAL_MAX		(8)		//!< �\��閇��

#define POKEMON_NONE_NAME	L"NONE"	//!< �|�P�����f�t�H���g����L""�������ꍇ�̖��O

//! �|�P�����p�����[�^�f�[�^�擾�̂��߂̃C���f�b�N�X
typedef enum {
	ID_PARA_personal_rnd=0,						//!< ������
	ID_PARA_pp_fast_mode,						//
	ID_PARA_ppp_fast_mode,						//
	ID_PARA_checksum,							//!< �`�F�b�N�T��

	ID_PARA_monsno,								//!< �����X�^�[�i���o�[
	ID_PARA_item,								//!< �����A�C�e���i���o�[
	ID_PARA_id_no,								//!< IDNo
	ID_PARA_exp,								//!< �o���l
	ID_PARA_friend,								//!< �Ȃ��x
	ID_PARA_speabino,							//!< ����\��
	ID_PARA_mark,								//!< �|�P�����ɂ���}�[�N�i�{�b�N�X�j
	ID_PARA_country_code,						//!< ���R�[�h
	ID_PARA_hp_exp,								//!< HP�w�͒l
	ID_PARA_pow_exp,							//!< �U���͓w�͒l
	ID_PARA_def_exp,							//!< �h��͓w�͒l
	ID_PARA_agi_exp,							//!< �f�����w�͒l
	ID_PARA_spepow_exp,							//!< ���U�w�͒l
	ID_PARA_spedef_exp,							//!< ���h�w�͒l
	ID_PARA_style,								//!< �������悳
	ID_PARA_beautiful,							//!< ��������
	ID_PARA_cute,								//!< ���킢��
	ID_PARA_clever,								//!< ��������
	ID_PARA_strong,								//!< �����܂���
	ID_PARA_fur,								//!< �щ�
	ID_PARA_sinou_champ_ribbon,					//!< �V���I�E�`�����v���{��
	ID_PARA_sinou_battle_tower_ttwin_first,		//!< �V���I�E�o�g���^���[�^���[�^�C�N�[������1���
	ID_PARA_sinou_battle_tower_ttwin_second,	//!< �V���I�E�o�g���^���[�^���[�^�C�N�[������2���
	ID_PARA_sinou_battle_tower_2vs2_win50,		//!< �V���I�E�o�g���^���[�^���[�_�u��50�A��
	ID_PARA_sinou_battle_tower_aimulti_win50,	//!< �V���I�E�o�g���^���[�^���[AI�}���`50�A��
	ID_PARA_sinou_battle_tower_siomulti_win50,	//!< �V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
	ID_PARA_sinou_battle_tower_wifi_rank5,		//!< �V���I�E�o�g���^���[Wifi�����N�T����
	ID_PARA_sinou_syakki_ribbon,				//!< �V���I�E����������{��
	ID_PARA_sinou_dokki_ribbon,					//!< �V���I�E�ǂ������{��
	ID_PARA_sinou_syonbo_ribbon,				//!< �V���I�E�����ڃ��{��
	ID_PARA_sinou_ukka_ribbon,					//!< �V���I�E���������{��
	ID_PARA_sinou_sukki_ribbon,					//!< �V���I�E���������{��
	ID_PARA_sinou_gussu_ribbon,					//!< �V���I�E���������{��
	ID_PARA_sinou_nikko_ribbon,					//!< �V���I�E�ɂ������{��
	ID_PARA_sinou_gorgeous_ribbon,				//!< �V���I�E�S�[�W���X���{��
	ID_PARA_sinou_royal_ribbon,					//!< �V���I�E���C�������{��
	ID_PARA_sinou_gorgeousroyal_ribbon,			//!< �V���I�E�S�[�W���X���C�������{��
	ID_PARA_sinou_ashiato_ribbon,				//!< �V���I�E�������ƃ��{��
	ID_PARA_sinou_record_ribbon,				//!< �V���I�E���R�[�h���{��
	ID_PARA_sinou_history_ribbon,				//!< �V���I�E�q�X�g���[���{��
	ID_PARA_sinou_legend_ribbon,				//!< �V���I�E���W�F���h���{��
	ID_PARA_sinou_red_ribbon,					//!< �V���I�E���b�h���{��
	ID_PARA_sinou_green_ribbon,					//!< �V���I�E�O���[�����{��
	ID_PARA_sinou_blue_ribbon,					//!< �V���I�E�u���[���{��
	ID_PARA_sinou_festival_ribbon,				//!< �V���I�E�t�F�X�e�B�o�����{��
	ID_PARA_sinou_carnival_ribbon,				//!< �V���I�E�J�[�j�o�����{��
	ID_PARA_sinou_classic_ribbon,				//!< �V���I�E�N���V�b�N���{��
	ID_PARA_sinou_premiere_ribbon,				//!< �V���I�E�v���~�A���{��
	ID_PARA_sinou_amari_ribbon,					//!< ���܂�

	ID_PARA_waza1,								//!< �����Z1
	ID_PARA_waza2,								//!< �����Z2
	ID_PARA_waza3,								//!< �����Z3
	ID_PARA_waza4,								//!< �����Z4
	ID_PARA_pp1,								//!< �����ZPP1
	ID_PARA_pp2,								//!< �����ZPP2
	ID_PARA_pp3,								//!< �����ZPP3
	ID_PARA_pp4,								//!< �����ZPP4
	ID_PARA_pp_count1,							//!< �����ZPP_COUNT1
	ID_PARA_pp_count2,							//!< �����ZPP_COUNT2
	ID_PARA_pp_count3,							//!< �����ZPP_COUNT3
	ID_PARA_pp_count4,							//!< �����ZPP_COUNT4
	ID_PARA_pp_max1,							//!< �����ZPPMAX1
	ID_PARA_pp_max2,							//!< �����ZPPMAX2
	ID_PARA_pp_max3,							//!< �����ZPPMAX3
	ID_PARA_pp_max4,							//!< �����ZPPMAX4
	ID_PARA_hp_rnd,								//!< HP����
	ID_PARA_pow_rnd,							//!< �U���͗���
	ID_PARA_def_rnd,							//!< �h��͗���
	ID_PARA_agi_rnd,							//!< �f��������
	ID_PARA_spepow_rnd,							//!< ���U����
	ID_PARA_spedef_rnd,							//!< ���h����
	ID_PARA_tamago_flag,						//!< �^�}�S�t���O
	ID_PARA_nickname_flag,						//!< �j�b�N�l�[�����������ǂ����t���O
	ID_PARA_stylemedal_normal,					//!< �������悳�M��(�m�[�}��)AGB�R���e�X�g
	ID_PARA_stylemedal_super,					//!< �������悳�M��(�X�[�p�[)AGB�R���e�X�g
	ID_PARA_stylemedal_hyper,					//!< �������悳�M��(�n�C�p�[)AGB�R���e�X�g
	ID_PARA_stylemedal_master,					//!< �������悳�M��(�}�X�^�[)AGB�R���e�X�g
	ID_PARA_beautifulmedal_normal,				//!< ���������M��(�m�[�}��)AGB�R���e�X�g
	ID_PARA_beautifulmedal_super,				//!< ���������M��(�X�[�p�[)AGB�R���e�X�g
	ID_PARA_beautifulmedal_hyper,				//!< ���������M��(�n�C�p�[)AGB�R���e�X�g
	ID_PARA_beautifulmedal_master,				//!< ���������M��(�}�X�^�[)AGB�R���e�X�g
	ID_PARA_cutemedal_normal,					//!< ���킢���M��(�m�[�}��)AGB�R���e�X�g
	ID_PARA_cutemedal_super,					//!< ���킢���M��(�X�[�p�[)AGB�R���e�X�g
	ID_PARA_cutemedal_hyper,					//!< ���킢���M��(�n�C�p�[)AGB�R���e�X�g
	ID_PARA_cutemedal_master,					//!< ���킢���M��(�}�X�^�[)AGB�R���e�X�g
	ID_PARA_clevermedal_normal,					//!< ���������M��(�m�[�}��)AGB�R���e�X�g
	ID_PARA_clevermedal_super,					//!< ���������M��(�X�[�p�[)AGB�R���e�X�g
	ID_PARA_clevermedal_hyper,					//!< ���������M��(�n�C�p�[)AGB�R���e�X�g
	ID_PARA_clevermedal_master,					//!< ���������M��(�}�X�^�[)AGB�R���e�X�g
	ID_PARA_strongmedal_normal,					//!< �����܂����M��(�m�[�}��)AGB�R���e�X�g
	ID_PARA_strongmedal_super,					//!< �����܂����M��(�X�[�p�[)AGB�R���e�X�g
	ID_PARA_strongmedal_hyper,					//!< �����܂����M��(�n�C�p�[)AGB�R���e�X�g
	ID_PARA_strongmedal_master,					//!< �����܂����M��(�}�X�^�[)AGB�R���e�X�g
	ID_PARA_champ_ribbon,						//!< �`�����v���{��
	ID_PARA_winning_ribbon,						//!< �E�B�j���O���{��
	ID_PARA_victory_ribbon,						//!< �r�N�g���[���{��
	ID_PARA_bromide_ribbon,						//!< �u���}�C�h���{��
	ID_PARA_ganba_ribbon,						//!< ����΃��{��
	ID_PARA_marine_ribbon,						//!< �}�������{��
	ID_PARA_land_ribbon,						//!< �����h���{��
	ID_PARA_sky_ribbon,							//!< �X�J�C���{��
	ID_PARA_country_ribbon,						//!< �J���g���[���{��
	ID_PARA_national_ribbon,					//!< �i�V���i�����{��
	ID_PARA_earth_ribbon,						//!< �A�[�X���{��
	ID_PARA_world_ribbon,						//!< ���[���h���{��
	ID_PARA_event_get_flag,						//!< �C�x���g�Ŕz�z���ꂽ���Ƃ������t���O
	ID_PARA_sex,								//!< ����
	ID_PARA_form_no,							//!< �`��i���o�[�i�A���m�[���A�f�I�L�V�X�A�~�m���X�ȂǗp�j
	ID_PARA_dummy_p2_1,							//!< ���܂�
	ID_PARA_dummy_p2_2,							//!< ���܂�
	ID_PARA_dummy_p2_3,							//!< ���܂�

	ID_PARA_nickname,							//!< �j�b�N�l�[��
	ID_PARA_nickname_buf,						//!< �j�b�N�l�[���iSTRBUF�g�p�j
	ID_PARA_nickname_buf_flag,					//!< �j�b�N�l�[���iSTRBUF�g�p,nickname_flag���I���ɂ���j
	ID_PARA_pref_code,							//!< �s���{���R�[�h
	ID_PARA_get_cassette,						//!< �߂܂����J�Z�b�g�i�J���[�o�[�W�����j
	ID_PARA_trial_stylemedal_normal,			//!< �������悳�M��(�m�[�}��)�g���C�A��
	ID_PARA_trial_stylemedal_super,				//!< �������悳�M��(�X�[�p�[)�g���C�A��
	ID_PARA_trial_stylemedal_hyper,				//!< �������悳�M��(�n�C�p�[)�g���C�A��
	ID_PARA_trial_stylemedal_master,			//!< �������悳�M��(�}�X�^�[)�g���C�A��
	ID_PARA_trial_beautifulmedal_normal,		//!< ���������M��(�m�[�}��)�g���C�A��
	ID_PARA_trial_beautifulmedal_super,			//!< ���������M��(�X�[�p�[)�g���C�A��
	ID_PARA_trial_beautifulmedal_hyper,			//!< ���������M��(�n�C�p�[)�g���C�A��
	ID_PARA_trial_beautifulmedal_master,		//!< ���������M��(�}�X�^�[)�g���C�A��
	ID_PARA_trial_cutemedal_normal,				//!< ���킢���M��(�m�[�}��)�g���C�A��
	ID_PARA_trial_cutemedal_super,				//!< ���킢���M��(�X�[�p�[)�g���C�A��
	ID_PARA_trial_cutemedal_hyper,				//!< ���킢���M��(�n�C�p�[)�g���C�A��
	ID_PARA_trial_cutemedal_master,				//!< ���킢���M��(�}�X�^�[)�g���C�A��
	ID_PARA_trial_clevermedal_normal,			//!< ���������M��(�m�[�}��)�g���C�A��
	ID_PARA_trial_clevermedal_super,			//!< ���������M��(�X�[�p�[)�g���C�A��
	ID_PARA_trial_clevermedal_hyper,			//!< ���������M��(�n�C�p�[)�g���C�A��
	ID_PARA_trial_clevermedal_master,			//!< ���������M��(�}�X�^�[)�g���C�A��
	ID_PARA_trial_strongmedal_normal,			//!< �����܂����M��(�m�[�}��)�g���C�A��
	ID_PARA_trial_strongmedal_super,			//!< �����܂����M��(�X�[�p�[)�g���C�A��
	ID_PARA_trial_strongmedal_hyper,			//!< �����܂����M��(�n�C�p�[)�g���C�A��
	ID_PARA_trial_strongmedal_master,			//!< �����܂����M��(�}�X�^�[)�g���C�A��
	ID_PARA_amari_ribbon,						//!< �]�胊�{��

	ID_PARA_oyaname,							//!< �e�̖��O
	ID_PARA_oyaname_buf,						//!< �e�̖��O�iSTRBUF�g�p�j
	ID_PARA_get_year,							//!< �߂܂����N
	ID_PARA_get_month,							//!< �߂܂�����
	ID_PARA_get_day,							//!< �߂܂�����
	ID_PARA_birth_year,							//!< ���܂ꂽ�N
	ID_PARA_birth_month,						//!< ���܂ꂽ��
	ID_PARA_birth_day,							//!< ���܂ꂽ��
	ID_PARA_get_place,							//!< �߂܂����ꏊ
	ID_PARA_birth_place,						//!< ���܂ꂽ�ꏊ
	ID_PARA_pokerus,							//!< �|�P���X
	ID_PARA_get_ball,							//!< �߂܂����{�[��
	ID_PARA_get_level,							//!< �߂܂������x��
	ID_PARA_oyasex,								//!< �e�̐���
	ID_PARA_get_ground_id,						//!< �߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
	ID_PARA_dummy_p4_1,							//!< ���܂�

	ID_PARA_condition,							//!< �R���f�B�V����
	ID_PARA_level,								//!< ���x��
	ID_PARA_cb_id,								//!< �J�X�^���{�[��ID
	ID_PARA_hp,									//!< HP
	ID_PARA_hpmax,								//!< HPMAX
	ID_PARA_pow,								//!< �U����
	ID_PARA_def,								//!< �h���
	ID_PARA_agi,								//!< �f����
	ID_PARA_spepow,								//!< ���U
	ID_PARA_spedef,								//!< ���h
	ID_PARA_mail_data,							//!< ���[���f�[�^
	ID_PARA_cb_core,							//!< �J�X�^���{�[���f�[�^
	
	ID_PARA_poke_exist,							//!< �|�P�������݃t���O
	ID_PARA_tamago_exist,						//!< �^�}�S���݃t���O

	ID_PARA_monsno_egg,							//!< �^�}�S���ǂ����������Ƀ`�F�b�N

	ID_PARA_power_rnd,							//!< �p���[������u32�ň���
	ID_PARA_nidoran_nickname,					//!< �����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N

	ID_PARA_type1,								//!< �|�P�����̃^�C�v�P���擾�i�A�E�X�A�}���`�^�C�v�`�F�b�N������j
	ID_PARA_type2,								//!< �|�P�����̃^�C�v�Q���擾�i�A�E�X�A�}���`�^�C�v�`�F�b�N������j

	ID_PARA_default_name,						//!< �|�P�����̃f�t�H���g��

	ID_PARA_end									// �p�����[�^��ǉ�����Ƃ��́A���������ɒǉ�
} PokeToolParamID;

enum COUNTRY_CODE{          // ����R�[�h�ڍ�
	COUNTRY_CODE_JP = 1,    // 1, ���{��
	COUNTRY_CODE_US,        // 2, �p��
	COUNTRY_CODE_FR,        // 3, �t�����X��
	COUNTRY_CODE_IT,        // 4, �C�^���A��
	COUNTRY_CODE_DE,        // 5, �h�C�c��
	COUNTRY_CODE_RESERVE,   // 6, ���g�p
	COUNTRY_CODE_SP,        // 7, �X�y�C����
    COUNTRY_CODE_KR,        // 8, �؍���
    COUNTRY_CODE_MAX = 8	//!< ���R�[�h�̌�
};

enum GENDER
{
    GENDER_MALE = 0,
    GENDER_FEMALE = 1,
    GENDER_NONE = 2
};

struct pokemon_paso_param1
{
	u16	monsno;								//02h	�����X�^�[�i���o�[
	u16 item;								//04h	�����A�C�e���i���o�[
	u32	id_no;								//08h	IDNo
	u32	exp;								//0ch	�o���l
	u8	_friend;							//0dh	�Ȃ��x
	u8	speabino;							//0eh	����\��
	u8	mark;								//0fh	�|�P�����ɂ���}�[�N�i�{�b�N�X�j
	u8	country_code;						//10h	���R�[�h
	u8	hp_exp;								//11h	HP�w�͒l
	u8	pow_exp;							//12h	�U���͓w�͒l
	u8	def_exp;							//13h	�h��͓w�͒l
	u8	agi_exp;							//14h	�f�����w�͒l
	u8	spepow_exp;							//15h	���U�w�͒l
	u8	spedef_exp;							//16h	���h�w�͒l
	u8	style;								//17h	�������悳
	u8	beautiful;							//18h	��������
	u8	cute;								//19h	���킢��
	u8	clever;								//1ah	��������
	u8	strong;								//1bh	�����܂���
	u8	fur;								//1ch	�щ�
	u32	sinou_ribbon;						//20h	�V���I�E�n���{���i�r�b�g�t�B�[���h�ŃA�N�Z�X����ƃv���O�����������������̂ŁA�r�b�g�V�t�g�ɕύX�j
//�ȉ�����
#if 0
	u32	sinou_champ_ribbon					:1;	//�V���I�E�`�����v���{��
	u32	sinou_battle_tower_ttwin_first		:1;	//�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
	u32	sinou_battle_tower_ttwin_second		:1;	//�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
	u32	sinou_battle_tower_2vs2_win50		:1;	//�V���I�E�o�g���^���[�^���[�_�u��50�A��
	u32	sinou_battle_tower_aimulti_win50	:1;	//�V���I�E�o�g���^���[�^���[AI�}���`50�A��
	u32	sinou_battle_tower_siomulti_win50	:1;	//�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
	u32	sinou_battle_tower_wifi_rank5		:1;	//�V���I�E�o�g���^���[Wifi�����N�T����
	u32	sinou_syakki_ribbon					:1;	//�V���I�E����������{��
	u32	sinou_dokki_ribbon					:1;	//�V���I�E�ǂ������{��
	u32	sinou_syonbo_ribbon					:1;	//�V���I�E�����ڃ��{��

	u32	sinou_ukka_ribbon					:1;	//�V���I�E���������{��
	u32	sinou_sukki_ribbon					:1;	//�V���I�E���������{��
	u32	sinou_gussu_ribbon					:1;	//�V���I�E���������{��
	u32	sinou_nikko_ribbon					:1;	//�V���I�E�ɂ������{��
	u32	sinou_gorgeous_ribbon				:1;	//�V���I�E�S�[�W���X���{��
	u32	sinou_royal_ribbon					:1;	//�V���I�E���C�������{��
	u32	sinou_gorgeousroyal_ribbon			:1;	//�V���I�E�S�[�W���X���C�������{��
	u32	sinou_ashiato_ribbon				:1;	//�V���I�E�������ƃ��{��
	u32	sinou_record_ribbon					:1;	//�V���I�E���R�[�h���{��
	u32	sinou_history_ribbon				:1;	//�V���I�E�q�X�g���[���{��

	u32	sinou_legend_ribbon					:1;	//�V���I�E���W�F���h���{��
	u32	sinou_red_ribbon					:1;	//�V���I�E���b�h���{��
	u32	sinou_green_ribbon					:1;	//�V���I�E�O���[�����{��
	u32	sinou_blue_ribbon					:1;	//�V���I�E�u���[���{��
	u32	sinou_festival_ribbon				:1;	//�V���I�E�t�F�X�e�B�o�����{��
	u32	sinou_carnival_ribbon				:1;	//�V���I�E�J�[�j�o�����{��
	u32	sinou_classic_ribbon				:1;	//�V���I�E�N���V�b�N���{��
	u32	sinou_premiere_ribbon				:1;	//�V���I�E�v���~�A���{��

	u32	sinou_amari_ribbon					:4;	//20h	���܂�
#endif
};
	
struct pokemon_paso_param2
{
	u16	waza[WAZA_TEMOTI_MAX];				//08h	�����Z
	u8	pp[WAZA_TEMOTI_MAX];				//0ch	�����ZPP
	u8	pp_count[WAZA_TEMOTI_MAX];			//10h	�����ZPP_COUNT
	u32	hp_rnd			:5;					//		HP����
	u32	pow_rnd			:5;					//		�U���͗���
	u32	def_rnd			:5;					//		�h��͗���
	u32	agi_rnd			:5;					//		�f��������
	u32	spepow_rnd		:5;					//		���U����
	u32	spedef_rnd		:5;					//		���h����
	u32	tamago_flag		:1;					//		�^�}�S�t���O�i0:�^�}�S����Ȃ��@1:�^�}�S����j
	u32	nickname_flag	:1;					//14h	�j�b�N�l�[�����������ǂ����t���O�i0:���Ă��Ȃ��@1:�����j

	u32	old_ribbon;							//18h	�ߋ���̃��{���n�i�r�b�g�t�B�[���h�ŃA�N�Z�X����ƃv���O�����������������̂ŁA�r�b�g�V�t�g�ɕύX�j
//�ȉ�����
#if 0
	u32	stylemedal_normal		:1;			//	�������悳�M��(�m�[�}��)(AGB�R���e�X�g)
	u32	stylemedal_super		:1;			//	�������悳�M��(�X�[�p�[)(AGB�R���e�X�g)
	u32	stylemedal_hyper		:1;			//	�������悳�M��(�n�C�p�[)(AGB�R���e�X�g)
	u32	stylemedal_master		:1;			//	�������悳�M��(�}�X�^�[)(AGB�R���e�X�g)
	u32	beautifulmedal_normal	:1;			//	���������M��(�m�[�}��)(AGB�R���e�X�g)
	u32	beautifulmedal_super	:1;			//	���������M��(�X�[�p�[)(AGB�R���e�X�g)
	u32	beautifulmedal_hyper	:1;			//	���������M��(�n�C�p�[)(AGB�R���e�X�g)
	u32	beautifulmedal_master	:1;			//	���������M��(�}�X�^�[)(AGB�R���e�X�g)
	u32	cutemedal_normal		:1;			//	���킢���M��(�m�[�}��)(AGB�R���e�X�g)
	u32	cutemedal_super			:1;			//	���킢���M��(�X�[�p�[)(AGB�R���e�X�g)
	u32	cutemedal_hyper			:1;			//	���킢���M��(�n�C�p�[)(AGB�R���e�X�g)
	u32	cutemedal_master		:1;			//	���킢���M��(�}�X�^�[)(AGB�R���e�X�g)
	u32	clevermedal_normal		:1;			//	���������M��(�m�[�}��)(AGB�R���e�X�g)
	u32	clevermedal_super		:1;			//	���������M��(�X�[�p�[)(AGB�R���e�X�g)
	u32	clevermedal_hyper		:1;			//	���������M��(�n�C�p�[)(AGB�R���e�X�g)
	u32	clevermedal_master		:1;			//	���������M��(�}�X�^�[)(AGB�R���e�X�g)
	u32	strongmedal_normal		:1;			//	�����܂����M��(�m�[�}��)(AGB�R���e�X�g)
	u32	strongmedal_super		:1;			//	�����܂����M��(�X�[�p�[)(AGB�R���e�X�g)
	u32	strongmedal_hyper		:1;			//	�����܂����M��(�n�C�p�[)(AGB�R���e�X�g)
	u32	strongmedal_master		:1;			//	�����܂����M��(�}�X�^�[)(AGB�R���e�X�g)

	u32	champ_ribbon	:1;					//		�`�����v���{��
	u32	winning_ribbon	:1;					//		�E�B�j���O���{��
	u32	victory_ribbon	:1;					//		�r�N�g���[���{��
	u32	bromide_ribbon	:1;					//		�u���}�C�h���{��
	u32	ganba_ribbon	:1;					//		����΃��{��
	u32	marine_ribbon	:1;					//		�}�������{��
	u32	land_ribbon		:1;					//		�����h���{��
	u32	sky_ribbon		:1;					//		�X�J�C���{��
	u32	country_ribbon	:1;					//		�J���g���[���{��
	u32	national_ribbon	:1;					//		�i�V���i�����{��
	u32	earth_ribbon	:1;					//		�A�[�X���{��
	u32	world_ribbon	:1;					//		���[���h���{��
#endif

	u8	event_get_flag	:1;					//		�C�x���g�Ŕz�z�������Ƃ������t���O
	u8	sex				:2;					//   	�|�P�����̐���
	u8	form_no			:5;					//19h	�`��i���o�[�i�A���m�[���A�f�I�L�V�X�A�~�m���X�ȂǗp�j
	u8	dummy_p2_1;							//1ah	���܂�
	u16	dummy_p2_2;							//1ch	���܂�
	u32	dummy_p2_3;							//20h	���܂�
};
	
struct pokemon_paso_param3
{
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	�j�b�N�l�[��(MONS_NAME_SIZE=10)+(EOM_SIZE=1)=11
	u8	pref_code;								//18h	�s���{���R�[�h
	u8	get_cassette;							//		�߂܂����J�Z�b�g�o�[�W����
	u64	new_ribbon;								//20h	�V���{���n�i�r�b�g�t�B�[���h�ŃA�N�Z�X����ƃv���O�����������������̂ŁA�r�b�g�V�t�g�ɕύX�j
//�ȉ�����
#if 0
	u32	trial_stylemedal_normal			:1;		//	�������悳�M��(�m�[�}��)(�g���C�A��)
	u32	trial_stylemedal_super			:1;		//	�������悳�M��(�X�[�p�[)(�g���C�A��)
	u32	trial_stylemedal_hyper			:1;		//	�������悳�M��(�n�C�p�[)(�g���C�A��)
	u32	trial_stylemedal_master			:1;		//	�������悳�M��(�}�X�^�[)(�g���C�A��)
	u32	trial_beautifulmedal_normal		:1;		//	���������M��(�m�[�}��)(�g���C�A��)
	u32	trial_beautifulmedal_super		:1;		//	���������M��(�X�[�p�[)(�g���C�A��)
	u32	trial_beautifulmedal_hyper		:1;		//	���������M��(�n�C�p�[)(�g���C�A��)
	u32	trial_beautifulmedal_master		:1;		//	���������M��(�}�X�^�[)(�g���C�A��)
	u32	trial_cutemedal_normal			:1;		//	���킢���M��(�m�[�}��)(�g���C�A��)
	u32	trial_cutemedal_super			:1;		//	���킢���M��(�X�[�p�[)(�g���C�A��)
	u32	trial_cutemedal_hyper			:1;		//	���킢���M��(�n�C�p�[)(�g���C�A��)
	u32	trial_cutemedal_master			:1;		//	���킢���M��(�}�X�^�[)(�g���C�A��)
	u32	trial_clevermedal_normal		:1;		//	���������M��(�m�[�}��)(�g���C�A��)
	u32	trial_clevermedal_super			:1;		//	���������M��(�X�[�p�[)(�g���C�A��)
	u32	trial_clevermedal_hyper			:1;		//	���������M��(�n�C�p�[)(�g���C�A��)
	u32	trial_clevermedal_master		:1;		//	���������M��(�}�X�^�[)(�g���C�A��)
	u32	trial_strongmedal_normal		:1;		//	�����܂����M��(�m�[�}��)(�g���C�A��)
	u32	trial_strongmedal_super			:1;		//	�����܂����M��(�X�[�p�[)(�g���C�A��)
	u32	trial_strongmedal_hyper			:1;		//	�����܂����M��(�n�C�p�[)(�g���C�A��)
	u32	trial_strongmedal_master		:1;		//	�����܂����M��(�}�X�^�[)(�g���C�A��)
	u32 amari_ribbon					:12;	//20h	���܂�
	u32	amari;									//20h	���܂�
#endif
};
	
struct pokemon_paso_param4
{
	STRCODE	oyaname[7+EOM_SIZE];		//10h	�e�̖��O(PERSON_NAME_SIZE=7)+(EOM_SIZE_=1)=8*2(STRCODE=u16)

	u8	get_year;						//11h	�߂܂����N
	u8	get_month;						//12h	�߂܂�����
	u8	get_day;						//13h	�߂܂�����
	u8	birth_year;						//14h	���܂ꂽ�N

	u8	birth_month;					//15h	���܂ꂽ��
	u8	birth_day;						//16h	���܂ꂽ��
	u16	get_place;						//18h	�߂܂����ꏊ

	u16	birth_place;					//1ah	���܂ꂽ�ꏊ
	u8	pokerus;						//1bh	�|�P���X
	u8	get_ball;						//1ch	�߂܂����{�[��

	u8	get_level		:7;				//1dh	�߂܂������x��
	u8	oyasex			:1;				//1dh	�e�̐���
	u8	get_ground_id;					//1eh	�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
	u16	dummy_p4_1;						//20h	���܂�
};

// �{�b�N�X�|�P�������̍\���̐錾
struct pokemon_paso_param
{
	u32	personal_rnd;					//04h	������
	u16	pp_fast_mode	:1;				//06h	�Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
	u16	ppp_fast_mode	:1;				//06h	�Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
	u16					:14;			//06h
	u16	checksum;						//08h	�`�F�b�N�T��

	u8	paradata[sizeof(POKEMON_PASO_PARAM1)+
				 sizeof(POKEMON_PASO_PARAM2)+
				 sizeof(POKEMON_PASO_PARAM3)+
				 sizeof(POKEMON_PASO_PARAM4)];	//88h
};

// ���[���|�P�����A�C�R���f�[�^�^
typedef union _MAIL_ICON{
	struct{
		u16	cgxID:12;	// �|�P�����A�C�R���L�����N�^CgxID
		u16	palID:4;	// �|�P�����A�C�R���p���b�gID
	};
	u16	dat;
}MAIL_ICON;

typedef struct {
	u16				sentence_type;			// ���̓^�C�v
	u16				sentence_id;			// �^�C�v��ID
	PMS_WORD		word[PMS_WORD_MAX];		// �P��ID
}PMS_DATA;

typedef struct _MAIL_DATA{
	u32	writerID;	// �g���[�i�[ID 4
	u8	sex;		// ��l���̐��� 1
	u8	region;		// ���R�[�h 1
	u8	version;	// �J�Z�b�g�o�[�W���� 1
	u8	design;		// �f�U�C���i���o�[ 1
	STRCODE name[PERSON_NAME_SIZE+EOM_SIZE];	// 16
	MAIL_ICON	icon[MAILDAT_ICONMAX+1];		// �A�C�R��NO�i�[�ꏊ[](3+1<padding>) 6+2=8
	PMS_DATA	msg[MAILDAT_MSGMAX];			// ���̓f�[�^
}_MAIL_DATA;

struct _CUSTOM_BALL_SEAL {
	u8	seal_id;	// �V�[���ԍ�
	u8	x;			// x ���W
	u8	y;			// y ���W
};

struct _CUSTOM_BALL_CORE {

	CB_SEAL	cb_seal[ CB_DEF_SEAL_MAX ];

};

// �{�b�N�X�|�P�������ȊO�̍\���̐錾
struct pokemon_calc_param
{
	u32			condition;			//04h	�R���f�B�V����
	u8			level;				//05h	���x��
	u8			cb_id;				//06h	�J�X�^���{�[��ID
	u16			hp;					//08h	HP
	u16			hpmax;				//0ah	HPMAX
	u16			pow;				//0ch	�U����
	u16			def;				//0eh	�h���
	u16			agi;				//10h	�f����
	u16			spepow;				//12h	���U
	u16			spedef;				//14h	���h
	_MAIL_DATA	mail_data;			//3ch	�|�P�����ɂ������郁�[���f�[�^
	CB_CORE		cb_core;			//54h	�J�X�^���{�[��
};

//! �莝���|�P�����̍\���̐錾
struct	pokemon_param
{
	POKEMON_PASO_PARAM	ppp;		//!< �{�b�N�X�|�P�������̍\����
	POKEMON_CALC_PARAM	pcp;		//!< �{�b�N�X�|�P�������ȊO�̍\����
};

//! �^���[�p�|�P�����f�[�^�^
struct _B_TOWER_POKEMON{
	union{
        struct{
            u16    mons_no    :11;    ///<�����X�^�[�i���o�[
            u16    form_no    :5;    ///<�����X�^�[�t�H�[���i���o�[
        };
    };

    u16    item_no;    ///<��������

    u16    waza[WAZA_TEMOTI_MAX];        ///<�����Z

    u32    id_no;                    ///<IDNo
    u32    personal_rnd;            ///<������

    union{
        struct{
        u32    hp_rnd        :5;        ///<HP�p���[����
        u32    pow_rnd        :5;        ///<POW�p���[����
        u32    def_rnd        :5;        ///<DEF�p���[����
        u32    agi_rnd        :5;        ///<AGI�p���[����
        u32    spepow_rnd    :5;        ///<SPEPOW�p���[����
        u32    spedef_rnd    :5;        ///<SPEDEF�p���[����
        u32    ngname_f    :1;        ///<NG�l�[���t���O
        u32                :1;        //1ch �]��
        };
        u32 power_rnd;
    };

    union{
        struct{
        u8    hp_exp;                ///<HP�w�͒l
        u8    pow_exp;            ///<POW�w�͒l
        u8    def_exp;            ///<DEF�w�͒l
        u8    agi_exp;            ///<AGI�w�͒l
        u8    spepow_exp;            ///<SPEPOW�w�͒l
        u8    spedef_exp;            ///<SPEDEF�w�͒l
        };
        u8 exp[6];
    };
    union{
        struct{
        u8    pp_count0:2;    ///<�Z1�|�C���g�A�b�v
        u8    pp_count1:2;    ///<�Z2�|�C���g�A�b�v
        u8    pp_count2:2;    ///<�Z3�|�C���g�A�b�v
        u8    pp_count3:2;    ///<�Z4�|�C���g�A�b�v
        };
        u8 pp_count;
    };

    u8    country_code;            ///<���R�[�h

    u8    tokusei;                ///<����
    u8    natuki;                ///<�Ȃ��x

    ///�j�b�N�l�[�� ((MONS_NAM22E_SIZE:10)+(EOM_SIZE:1))*(STRCODE:u16)=22
    STRCODE    nickname[MONS_NAME_SIZE+EOM_SIZE];
};

/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					�֐��O���錾
 *-----------------------------------------------------------------------*/

void PokeTool_EncodePokemonParam(const POKEMON_PARAM* src, POKEMON_PARAM* dst);
BOOL PokeTool_DecodePokemonParam(const POKEMON_PARAM* src, POKEMON_PARAM* dst);
u32 PokeTool_GetPokeParam(POKEMON_PARAM *pp, PokeToolParamID id, void *buf);
void PokeTool_SetPokeParam(POKEMON_PARAM *pp, PokeToolParamID id, void *buf);
BOOL PokeTool_GetNickNameFlag(POKEMON_PARAM *pp);
void PokeTool_SetNickNameFlag(POKEMON_PARAM *pp, BOOL flag);
BOOL PokeTool_GetNickName(POKEMON_PARAM *pp, u16* buf);
BOOL PokeTool_SetNickName(POKEMON_PARAM *pp, const u16* buf);
BOOL PokeTool_GetParentName(POKEMON_PARAM *pp, u16* buf);
BOOL PokeTool_SetParentName(POKEMON_PARAM *pp, const u16* buf);
BOOL PokeTool_GetMailName(POKEMON_PARAM *pp, u16* buf);
BOOL PokeTool_SetMailName(POKEMON_PARAM *pp, const u16* buf);
BOOL PokeTool_BtGetNickName(B_TOWER_POKEMON* btp, u16* buf);
BOOL PokeTool_BtSetNickName(B_TOWER_POKEMON* btp, const u16* buf);
const wchar_t* PokeTool_GetDefaultName(u32 monsno, u8 countryCode);
BOOL PokeTool_SetNickNameDefault(POKEMON_PARAM *pp);
BOOL PokeTool_IsRare(POKEMON_PARAM *pp);
BOOL PokeTool_BtIsRare(B_TOWER_POKEMON *btp);
BOOL PokeTool_HasMail(POKEMON_PARAM *pp);
u8 PokeTool_GetMailLang(POKEMON_PARAM *pp);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // POKETOOL_H_