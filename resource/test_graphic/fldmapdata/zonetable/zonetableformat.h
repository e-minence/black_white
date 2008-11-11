
//NitroSDK/include/nitro/types.h���R�s�y
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long u32;

#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	(!TRUE)
#endif

//===========================================================================
//===========================================================================
//------------------------------------------------------------------
/**
 * @brief	�]�[�����Ɏ��f�[�^�̍\��
 */
//------------------------------------------------------------------
typedef struct{
	u8 area_id;							///<AREA_ID�̎w��
	u8 movemodel_id;					///<���샂�f���̎w��
	u16 matrix_id;						///<�}�b�v�}�g���b�N�X�̎w��
	u16 script_id;						///<�X�N���v�g�t�@�C���̃A�[�J�C�uID
	u16 sp_script_id;					///<����X�N���v�g�t�@�C���̃A�[�J�C�uID
	u16 msg_id;							///<���b�Z�[�W�t�@�C���̃A�[�J�C�uID
	u16 bgm_day_id;						///<BGM�w��i���j��ID
	u16 bgm_night_id;					///<BGM�w��i��j��ID
	u16 enc_data_id;					///<�G���J�E���g�w��
	u16 event_data_id;					///<�C�x���g�f�[�^�t�@�C���̃A�[�J�C�uID
	u16 placename_id:8;					///<�n�����b�Z�[�W�̂h�c
	u16 placename_win:8;				///<�n�����b�Z�[�W�p�̃E�B���h�E
	u8 weather_id;						///<�V��w���ID
	u8 camera_id;						///<�J�����w���ID
	u16 maptype:7;							///<�}�b�v�^�C�v�̎w��(MAPTYPE��`�Q�Ɓj
	u16 battle_bg_type:5;				///<�퓬�w�i�̎w��
	u16 bicycle_flag:1;					///<���]�Ԃɏ��邩�ǂ���
	u16 dash_flag:1;						///<�_�b�V���ł��邩�ǂ���
	u16 escape_flag:1;					///<���Ȃʂ��ł��邩�ǂ���
	u16 fly_flag:1;
}ZONE_DATA;
//------------------------------------------------------------------
//------------------------------------------------------------------

enum {
	//sp_scr_dummy = NARC_scr_seq_sp_dummy_bin,
	sp_scr_dummy = 0xf000,
	//scr_dummy = NARC_scr_seq_dummy_bin,
	scr_dummy = 0xe000,
	//event_dummy = NARC_zone_event_zone_dummy_total_bin,
	event_dummy = 0xd000,
	//sp_scr_dummy = NARC_scr_seq_sp_c01_bin,
	//scr_dummy = NARC_scr_seq_c01_bin,
	//msg_dummy = NARC_msg_c01_dat,
	msg_dummy = 0xcccc,
	enc_dummy = 0xffff
};

enum {
	AREA_ID_FIELD	=	0,
	MATRIX_ID_SINOU	=	1,
	BG_ID_FOREST	=	3,
	MMLID_NOENTRY	=	5,
	MAPTYPE_NOWHERE	=	0xff,
	MAPNAME_NOTHING	=	0x33,
	WEATHER_SYS_SUNNY	=	0x11,
	PLACE_WIN_SEE	=	0x80,
	PLACE_WIN_CITY	=	0x81
};

