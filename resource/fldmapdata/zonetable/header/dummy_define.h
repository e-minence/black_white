

//------------------------------------------------------------------
//------------------------------------------------------------------
//NitroSDK/include/nitro/types.h���R�s�y
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long u32;
typedef signed long s32;

#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	(!TRUE)
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------

enum {
	BG_ID_FOREST	=	3,

	MMLID_NOENTRY	=	0,
	MMLID_SAMPLEOBJ	=	1,

	MAPTYPE_NOWHERE	=	0xff,
	//MAPNAME_NOTHING	=	0x33,
	WEATHER_SYS_SUNNY	=	0x11,
	PLACE_WIN_SEE	=	0x80,
	PLACE_WIN_CITY	=	0x81,

	MATRIX_ARC_MAPMATRIX = 0,
	MATRIX_ARC_NGMATRIX = 1,

	MATRIX_ID_SINOU	=	1,
	MATRIX_ID_TEST0 =	0,
	MATRIX_ID_TEST1 =	1,
	MATRIX_ID_TEST2 =	2,
	MATRIX_ID_TEST3 =	3,
	MATRIX_ID_TEST4 =	4,
	MATRIX_ID_TEST5 =	5,
	MATRIX_ID_TEST6 =	6,
	MATRIX_ID_TEST7 =	7,
	MATRIX_ID_TEST8 =	8,
	MATRIX_ID_TEST9 =	9,
	MATRIX_ID_TEST10 =	10,

	DUMMY_DEFINE_END
};

enum {
	RSC_GRID_DEFAULT = 0,
	RSC_NOGRID_BRIDGE = 1,
	RSC_NOGRID_C03 = 2,
  RSC_GRID_PALACE = 3,
  RSC_GRID_RANDOM_MAP = 4,
  RSC_NOGRID_LEAGUE = 5,
  RSC_GRID_UNION = 6,

	//NO_GRID_SAMPLE = 2,
	//GRID_GS_SAMPLE = 3,
	RSC_TYPE_END
};
#include "../area_data/area_id.h"
#include "..\..\sound\wb_sound_data.sadl"
//#include "../map_matrix/map_matrix.naix"
#include "tmp/map_matrix.h"
#include "../script/script_seq.naix"      //script ID ��`�t�@�C��
#include "../../message/script_message.naix"     //message archive ID��`�t�@�C��
#include "..\..\message\dst\msg_place_name.h"   //�n���\���p������ID��`�t�@�C��
#include "..\..\place_name\area_win_gra.naix"   //�n���\���E�B���h�E���\�[�XID��`�t�@�C��

enum {
	//sp_scr_dummy = NARC_scr_seq_sp_dummy_bin,
	//sp_scr_dummy = NARC_scr_seq_sp_c01_bin,
	//scr_dummy = NARC_scr_seq_c01_bin,
	sp_scr_dummy = 0xf000,
	scr_dummy = NARC_script_seq_dummy_bin,

	//event_dummy = NARC_zone_event_zone_dummy_total_bin,
	event_dummy = 0xd000,

	//msg_dummy = NARC_msg_c01_dat,
	msg_dummy = 0,      //�Ƃ肠�����A���b�Z�[�W���Ȃ��ꍇ�͂O�𓖂ĂĂ���

	enc_dummy = 0xffff
};

//pokemon_wb/prog/include/field/weather_no.h����b�肱�ҁ[�B
//���ۂɂ͂����ƎQ�Ƃł���悤�ɂ��邱�ƁI�I
#define		WEATHER_NO_SUNNY			(0)				// ����
#define		WEATHER_NO_SNOW				(1)				// ��		
#define		WEATHER_NO_RAIN				(2)				// �J		
#define		WEATHER_NO_STORM			(3)				// ����		
#define		WEATHER_NO_SPARK			(4)				// ���J
#define		WEATHER_NO_SNOWSTORM		(5)				// ����
#define		WEATHER_NO_ARARE			(6)				// ����

#define		WEATHER_NO_MIRAGE			(7)				// 凋C�O

