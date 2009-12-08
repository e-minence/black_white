

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
	MMLID_NOENTRY	=	0,
	MMLID_SAMPLEOBJ	=	1,

	DUMMY_DEFINE_END
};

enum {
	RSC_GRID_DEFAULT = 0,
	RSC_NOGRID_BRIDGE = 1,
	RSC_NOGRID_C03 = 2,
  RSC_GRID_PALACE = 3,
  RSC_GRID_RANDOM_MAP = 4,
  RSC_NOGRID_LEAGUE = 5,
  RSC_GRID_COMM = 6,
  RSC_NOGRID_DEFAULT = 7,
  RSC_NOGRID_C03P02 = 8,
  RSC_GRID_MUSICAL = 9,
  RSC_GRID_GYM_ELEC = 10,
  RSC_GRID_WIDE = 11,     // 3x2
  RSC_NOGRID_D09 = 12,     // 3x3
  RSC_GRID_BRIDGE_H03 = 13,     // 1x6
  RSC_HYBRID = 14,     // 
  RSC_GRID_FOURKINGS = 15,     // �l�V�������p
  RSC_GRID_NOSCROLL = 16,     // �O���b�h�@�X�N���[���Ȃ�����

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
#include "header/maptype.h"
#include "../../encount/encount_data_w.naix"  //�G���J�E���g�f�[�^ID��`�t�@�C��
#include "../eventdata/binary/eventdata.naix"      //eventdata ID ��`�t�@�C��
#include "../../../prog/include/battle/battle_bg_def.h"  //�퓬�w�iID��`
#include "../camera_scroll/camera_scroll.naix"//�J�����G���A�f�[�^

enum {
	enc_dummy = 0xffff,

	camera_area_dummy = 0xffff,
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

