
//============================================================================================
/**
 * @file	waza_tool_def.h
 * @bfief	�Z�p�����[�^�c�[���Q�iWB�����Łj
 * @author	HisashiSogabe
 * @date	08.11.13
 */
//============================================================================================

//==============================================================================
//	�\���̒�`
//==============================================================================
///�R���e�X�g�Z�f�[�^
typedef struct{
	u8 ap_no;		///<�A�s�[��No
	u8 contype;		///<�R���e�X�g�Z�^�C�v(CONTYPE_STYLE��)
	
	u16 dummy;
}CONWAZA_DATA;

struct waza_table_data
{
	u16				battleeffect;
	u8				kind;
	u8				damage;

	u8				wazatype;
	u8				hitprobability;
	u8				pp;
	u8				addeffect;

	u16				attackrange;
	s8				attackpri;
	u8				flag;

	CONWAZA_DATA	condata;
};

