
//============================================================================================
/**
 * @file	batt_bg_tbl.h
 * @bfief	�퓬�w�i�e�[�u��
 * @author	BattBGConverter
 * �퓬�w�i�e�[�u���R���o�[�^���琶������܂���
*/
//============================================================================================

#pragma once

#define  ZONE_SPEC_ATTR_MAX  ( 17 )
#define  BATT_BG_TBL_NO_FILE     ( 0xffffffff )
#define  BATT_BG_TBL_FILE_MAX  ( 4 )

#define  BATT_BG_TBL_SEASON_MAX  ( 4 )

typedef enum
{
	BATT_BG_TBL_FILE_NSBMD = 0,
	BATT_BG_TBL_FILE_NSBCA,
	BATT_BG_TBL_FILE_NSBTA,
	BATT_BG_TBL_FILE_NSBMA,
}BATT_BG_TBL_FILE;

typedef struct
{
	u8 time_zone;
	u8 season;
	u8 bg_file[ ZONE_SPEC_ATTR_MAX ];
	u8 stage_file[ ZONE_SPEC_ATTR_MAX ];
}BATT_BG_TBL_ZONE_SPEC_TABLE;

typedef struct
{
	u16  edge_color;
	u16  padding;
	ARCDATID file[BATT_BG_TBL_FILE_MAX ][ BATT_BG_TBL_SEASON_MAX ];
}BATT_BG_TBL_STAGE_TABLE;

typedef struct
{
	ARCDATID file[BATT_BG_TBL_FILE_MAX ][ BATT_BG_TBL_SEASON_MAX ];
}BATT_BG_TBL_BG_TABLE;

typedef enum{
	BATT_BG_OBONID_00 = 0,		//�n�ʁi�l�G�Ȃ��j
	BATT_BG_OBONID_01,				//�n�ʁi�l�G����j
	BATT_BG_OBONID_02,				//���i���O�j
	BATT_BG_OBONID_03,				//���i�����j
	BATT_BG_OBONID_04,				//���A
	BATT_BG_OBONID_05,				//�Ő��i�l�G�Ȃ��j
	BATT_BG_OBONID_06,				//�Ő��i�l�G����j
	BATT_BG_OBONID_07,				//�R��
	BATT_BG_OBONID_08,				//����i���O�j
	BATT_BG_OBONID_09,				//����i�����j
	BATT_BG_OBONID_10,				//�����i���O�j
	BATT_BG_OBONID_11,				//�����i�����j
	BATT_BG_OBONID_12,				//�󂢎��n
	BATT_BG_OBONID_13,				//�X��i�����j
	BATT_BG_OBONID_14,				//�p���X��p
	BATT_BG_OBONID_15,				//�l�V���i�S�[�X�g�j��p
	BATT_BG_OBONID_16,				//�l�V���i�i���j��p
	BATT_BG_OBONID_17,				//�l�V���i���j��p
	BATT_BG_OBONID_18,				//�l�V���i�G�X�p�[�j��p
	BATT_BG_OBONID_19,				//N��p
	BATT_BG_OBONID_20,				//�Q�[�`�X��p
	BATT_BG_OBONID_21,				//�`�����s�I����p
	BATT_BG_OBONID_22,				//�G���J�E���g���i�l�G����j
	BATT_BG_OBONID_23,				//�G���J�E���g���i�l�G�Ȃ��j
	BATT_BG_OBONID_24,				//�A�X�t�@���g
}BATT_BG_OBONID;

