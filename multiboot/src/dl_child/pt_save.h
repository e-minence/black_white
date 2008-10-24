
#define CRC_LOADCHECK (1)
//---------------------------------------------------------------------------
///	�Z�[�u�Ɏg�p���Ă���Z�N�^��
//---------------------------------------------------------------------------
#define	SAVE_PAGE_MAX		(32)

#define	SAVE_SECTOR_SIZE	(0x1000)
//---------------------------------------------------------------------------
///	�Z�[�u���ڂ̒�`
//---------------------------------------------------------------------------
typedef enum {
	//�m�[�}���f�[�^�O���[�v
	GMDATA_ID_SYSTEM_DATA,
	GMDATA_ID_PLAYER_DATA,
	GMDATA_ID_TEMOTI_POKE,
	GMDATA_ID_TEMOTI_ITEM,
	GMDATA_ID_EVENT_WORK,
	GMDATA_ID_POKETCH_DATA,
	GMDATA_ID_SITUATION,
	GMDATA_ID_ZUKANWORK,
	GMDATA_ID_SODATEYA,
	GMDATA_ID_FRIEND,
	GMDATA_ID_MISC,
	GMDATA_ID_FIELDOBJSV,
	GMDATA_ID_UNDERGROUNDDATA,
    GMDATA_ID_REGULATION,
	GMDATA_ID_IMAGECLIPDATA,
	GMDATA_ID_MAILDATA,
	GMDATA_ID_PORUTODATA,
	GMDATA_ID_RANDOMGROUP,
	GMDATA_ID_FNOTE,
	GMDATA_ID_TRCARD,
	GMDATA_ID_RECORD,
	GMDATA_ID_CUSTOM_BALL,
	GMDATA_ID_PERAPVOICE,
	GMDATA_ID_FRONTIER,
	GMDATA_ID_SP_RIBBON,
	GMDATA_ID_ENCOUNT,
	GMDATA_ID_WORLDTRADEDATA,
	GMDATA_ID_TVWORK,
	GMDATA_ID_GUINNESS,
	GMDATA_ID_WIFILIST,
	GMDATA_ID_WIFIHISTORY,
	GMDATA_ID_FUSHIGIDATA,
	GMDATA_ID_POKEPARKDATA,
	GMDATA_ID_CONTEST,
	GMDATA_ID_PMS,
	GMDATA_ID_EMAIL,
	GMDATA_ID_WFHIROBA,

	GMDATA_NORMAL_FOTTER,	//WB�Œǉ��B�Z�[�u��CRC�Ƃ��J�E���^�Ƃ������Ă�
	//�{�b�N�X�f�[�^�O���[�v
	GMDATA_ID_BOXDATA,

	GMDATA_BOX_FOTTER,		//WB�Œǉ��B�{�b�N�X��CRC�Ƃ��J�E���^�Ƃ������Ă�

	GMDATA_ID_MAX,
}PT_GMDATA_ID;
//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER_PT	(0x20060623)
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

#define	FIRST_MIRROR_START	(0)
#define	SECOND_MIRROR_START	(64)

#define MIRROR1ST	(0)
#define	MIRROR2ND	(1)
#define	MIRRORERROR	(2)

#define HEAPID_SAVE_TEMP	(HEAPID_BASE_APP)

#define	SEC_DATA_SIZE		SECTOR_SIZE
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
	u32 g_count;		///<�O���[�o���J�E���^�iMY�f�[�^�ABOX�f�[�^���L�j
	u32 b_count;		///<�u���b�N���J�E���^�iMY�f�[�^��BOX�f�[�^�ƂœƗ��j
	u32 size;			///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
	u32 magic_number;	///<�}�W�b�N�i���o�[
	u8 blk_id;			///<�Ώۂ̃u���b�N�w��ID
	u16 crc;			///<�f�[�^�S�̂�CRC�l
}SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u���[�N�\����
 *
 * ���ۂ̃Z�[�u����镔���̍\��
 */
//---------------------------------------------------------------------------
typedef struct {
	u8 data[SECTOR_SIZE * SECTOR_MAX];	///<���ۂ̃f�[�^�ێ��̈�
}SAVEWORK;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define	WAZA_TEMOTI_MAX		(4)		///<1�̂̃|�P���������Ă�Z�̍ő�l
#define PERSON_NAME_SIZE	7	// �l���̖��O�̒����i�������܂ށj
#define MONS_NAME_SIZE		10	// �|�P�������̒���(�o�b�t�@�T�C�Y EOM_�܂܂�)
#define	EOM_SIZE			1	// �I���R�[�h�̒���
#define BOX_MAX_TRAY			(18)
#define BOX_MAX_POS				(5*6)
#define BOX_TRAYNAME_BUFSIZE	(20)	// ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B
typedef struct pokemon_para_data11 POKEMON_PARA_DATA11;
typedef struct pokemon_para_data12 POKEMON_PARA_DATA12;
typedef struct pokemon_para_data13 POKEMON_PARA_DATA13;
typedef struct pokemon_para_data14 POKEMON_PARA_DATA14;
typedef struct pokemon_para_data15 POKEMON_PARA_DATA15;
typedef struct pokemon_para_data16 POKEMON_PARA_DATA16;
typedef struct pokemon_para_data21 POKEMON_PARA_DATA21;
typedef struct pokemon_para_data22 POKEMON_PARA_DATA22;
typedef struct pokemon_para_data23 POKEMON_PARA_DATA23;
typedef struct pokemon_para_data24 POKEMON_PARA_DATA24;
typedef struct pokemon_para_data25 POKEMON_PARA_DATA25;
typedef struct pokemon_para_data26 POKEMON_PARA_DATA26;
typedef struct pokemon_para_data31 POKEMON_PARA_DATA31;
typedef struct pokemon_para_data32 POKEMON_PARA_DATA32;
typedef struct pokemon_para_data33 POKEMON_PARA_DATA33;
typedef struct pokemon_para_data34 POKEMON_PARA_DATA34;
typedef struct pokemon_para_data35 POKEMON_PARA_DATA35;
typedef struct pokemon_para_data36 POKEMON_PARA_DATA36;
typedef struct pokemon_para_data41 POKEMON_PARA_DATA41;
typedef struct pokemon_para_data42 POKEMON_PARA_DATA42;
typedef struct pokemon_para_data43 POKEMON_PARA_DATA43;
typedef struct pokemon_para_data44 POKEMON_PARA_DATA44;
typedef struct pokemon_para_data45 POKEMON_PARA_DATA45;
typedef struct pokemon_para_data46 POKEMON_PARA_DATA46;

typedef struct pt_box_data PT_BOX_DATA;
typedef struct pt_pokemon_param PT_POKEMON_PARAM;
typedef struct pokemon_paso_param1 POKEMON_PASO_PARAM1;
typedef struct pokemon_paso_param2 POKEMON_PASO_PARAM2;
typedef struct pokemon_paso_param3 POKEMON_PASO_PARAM3;
typedef struct pokemon_paso_param4 POKEMON_PASO_PARAM4;

struct pokemon_paso_param1
{
	u16	monsno;								//02h	�����X�^�[�i���o�[
	u16 item;								//04h	�����A�C�e���i���o�[
	u32	id_no;								//08h	IDNo
	u32	exp;								//0ch	�o���l
	u8	friend;								//0dh	�Ȃ��x
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
} ;
	
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
#define BOX_TRAYNAME_BUFSIZE	(20)	// ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B
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
	u32	marine_ribbon	:1;				//�|�P�����p�����[�^�擾�n�̒�`
enum{
	ID_POKEPARADATA11=0,	//0
	ID_POKEPARADATA12,		//1
	ID_POKEPARADATA13,		//2
	ID_POKEPARADATA14,		//3
	ID_POKEPARADATA15,		//4
	ID_POKEPARADATA16,		//5
	ID_POKEPARADATA21,		//6
	ID_POKEPARADATA22,		//7
	ID_POKEPARADATA23,		//8
	ID_POKEPARADATA24,		//9
	ID_POKEPARADATA25,		//10
	ID_POKEPARADATA26,		//11
	ID_POKEPARADATA31,		//12
	ID_POKEPARADATA32,		//13
	ID_POKEPARADATA33,		//14
	ID_POKEPARADATA34,		//15
	ID_POKEPARADATA35,		//16
	ID_POKEPARADATA36,		//17
	ID_POKEPARADATA41,		//18
	ID_POKEPARADATA42,		//19
	ID_POKEPARADATA43,		//20
	ID_POKEPARADATA44,		//21
	ID_POKEPARADATA45,		//22
	ID_POKEPARADATA46,		//23

	ID_POKEPARADATA51,		//24
	ID_POKEPARADATA52,		//25
	ID_POKEPARADATA53,		//26
	ID_POKEPARADATA54,		//27
	ID_POKEPARADATA55,		//28
	ID_POKEPARADATA56,		//29

	ID_POKEPARADATA61,		//30
	ID_POKEPARADATA62,		//31
};

enum{
	ID_POKEPARA1=0,
	ID_POKEPARA2,
	ID_POKEPARA3,
	ID_POKEPARA4,
};
	//		�}�������{��
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
	u16	new_get_place;						//1eh	�߂܂����ꏊ�i�Ȃ��̏ꏊ�Ή��p�j
	u16	new_birth_place;					//20h	���܂ꂽ�ꏊ�i�Ȃ��̏ꏊ�Ή��p�j
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
	STRCODE	oyaname[7+EOM_SIZE];				//10h	�e�̖��O(PERSON_NAME_SIZE=7)+(EOM_SIZE_=1)=8*2(STRCODE=u16)

	u8	get_year;								//11h	�߂܂����N
	u8	get_month;								//12h	�߂܂�����
	u8	get_day;								//13h	�߂܂�����
	u8	birth_year;								//14h	���܂ꂽ�N

	u8	birth_month;							//15h	���܂ꂽ��
	u8	birth_day;								//16h	���܂ꂽ��
	u16	get_place;								//18h	�߂܂����ꏊ

	u16	birth_place;							//1ah	���܂ꂽ�ꏊ
	u8	pokerus;								//1bh	�|�P���X
	u8	get_ball;								//1ch	�߂܂����{�[��

	u8	get_level		:7;						//1dh	�߂܂������x��
	u8	oyasex			:1;						//1dh	�e�̐���
	u8	get_ground_id;							//1eh	�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
	u16	dummy_p4_1;								//20h	���܂�
};

///<�{�b�N�X�|�P�������̍\���̐錾
struct pt_pokemon_param 
{
	u32	personal_rnd;							//04h	������
	u16	pp_fast_mode		:1;					//06h	�Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
	u16	ppp_fast_mode		:1;					//06h	�Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
	u16	fusei_tamago_flag	:1;					//06h	�_���^�}�S�t���O
	u16						:13;				//06h
	u16	checksum;								//08h	�`�F�b�N�T��

	u8	paradata[sizeof(POKEMON_PASO_PARAM1)+
				 sizeof(POKEMON_PASO_PARAM2)+
				 sizeof(POKEMON_PASO_PARAM3)+
				 sizeof(POKEMON_PASO_PARAM4)];	//88h
};


struct pt_box_data
{
	u32					currentTrayNumber;
	PT_POKEMON_PARAM	ppp[BOX_MAX_TRAY][BOX_MAX_POS];
	STRCODE				trayName[BOX_MAX_TRAY][BOX_TRAYNAME_BUFSIZE];
	u8					wallPaper[BOX_MAX_TRAY];
	u8					daisukiBitFlag;
};


//�|�P�����p�����[�^�擾�n�̒�`
enum{
	ID_POKEPARADATA11=0,	//0
	ID_POKEPARADATA12,		//1
	ID_POKEPARADATA13,		//2
	ID_POKEPARADATA14,		//3
	ID_POKEPARADATA15,		//4
	ID_POKEPARADATA16,		//5
	ID_POKEPARADATA21,		//6
	ID_POKEPARADATA22,		//7
	ID_POKEPARADATA23,		//8
	ID_POKEPARADATA24,		//9
	ID_POKEPARADATA25,		//10
	ID_POKEPARADATA26,		//11
	ID_POKEPARADATA31,		//12
	ID_POKEPARADATA32,		//13
	ID_POKEPARADATA33,		//14
	ID_POKEPARADATA34,		//15
	ID_POKEPARADATA35,		//16
	ID_POKEPARADATA36,		//17
	ID_POKEPARADATA41,		//18
	ID_POKEPARADATA42,		//19
	ID_POKEPARADATA43,		//20
	ID_POKEPARADATA44,		//21
	ID_POKEPARADATA45,		//22
	ID_POKEPARADATA46,		//23

	ID_POKEPARADATA51,		//24
	ID_POKEPARADATA52,		//25
	ID_POKEPARADATA53,		//26
	ID_POKEPARADATA54,		//27
	ID_POKEPARADATA55,		//28
	ID_POKEPARADATA56,		//29

	ID_POKEPARADATA61,		//30
	ID_POKEPARADATA62,		//31
};

enum{
	ID_POKEPARA1=0,
	ID_POKEPARA2,
	ID_POKEPARA3,
	ID_POKEPARA4,
};

struct pokemon_para_data11
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data12
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data13
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data14
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data15
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data16
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data21
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data22
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data23
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data24
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data25
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data26
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data31
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data32
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data33
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data34
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data35
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data36
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data41
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data42
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data43
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data44
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data45
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data46
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};


