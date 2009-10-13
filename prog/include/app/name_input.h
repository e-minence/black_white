//======================================================================
/**
 * @file	name_input.h
 * @brief	���O����
 * @author	ariizumi
 * @data	09/01/08
 */
//======================================================================

#ifndef NAME_INPUT_H__
#define NAME_INPUT_H__

#include "savedata\config.h"
#include "savedata\box_savedata.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
// ���O���̓��[�h(�Ƃ肠����Pt����ڐA
enum {
	NAMEIN_MYNAME = 0,	// �����̖��O
	NAMEIN_POKEMON,		// �|�P�����̖��O
	NAMEIN_BOX,			// �{�b�N�X�̖��O
	NAMEIN_RIVALNAME,	// ���C�o���l�[��
	NAMEIN_FRIENDCODE,	// �Ƃ������R�[�h
	NAMEIN_RANDOMGROUP, // �����̎�O���[�v�̖��O
	NAMEIN_STONE,		// �Δ�(�z�z�j
	NAMEIN_FRIENDNAME,  // WIFI�Ƃ������蒠�ɏ������ނƂ������̖��O
	//���₵���菇�Ԃ�ς����ꍇ��.c�̕��̕����񒷃e�[�u�����ҏW���Ă��������I
	NAMEIN_MAX,
};

// �|�P�������O���͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_POKEMON_LENGTH	(  5 )
#else
#define NAMEIN_POKEMON_LENGTH	( 10 )
#endif

// �g���[�i�[�����͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_PERSON_LENGTH	(  5 )
#else
#define NAMEIN_PERSON_LENGTH	( 7 )
#endif


// �O���[�v�����͂̒���NAMEIN_PERSON_LENGTH
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_GROUP_LENGTH		(  5 )
#else
#define NAMEIN_GROUP_LENGTH		( 7 )
#endif


//======================================================================
//	typedef struct
//======================================================================
//-------------------------------------
///	
//=====================================
typedef struct {
	int mode;		// ���̓��[�h(enum�Q�Ɓj
	int info;		// ���̓��[�h�Ƃ��Ēj��l���E����l���A�|�P�����̊J��NO�Ȃ�
	int form;		// ���̓��[�h���|�P�����̎��Ɍ`��l���󂯎��
	int wordmax;	// ���͕����ő吔
	int sex;		// �|�P�����̐���(PARA_MALE=0...PARA_FEMALE=1...PARA_UNK=2(���ʂȂ�))


	int cancel;		// ���O���͂��I���������ɔ��f�����t���O�B
					// ���͕������O�����������B�������͍ŏ��Ɠ����������ꍇ�͂��̃t���O�����B

	STRBUF *strbuf; // ����STRBUF�ɂ͂Q�̈Ӗ������݂���B
					// �P�͖��O���͉�ʂ���f�[�^���󂯎�郏�[�N�ł��邱�ƁB���O���͂��I������炱������R�s�[����
					// �����P�́A���O���͂ɍs���Ƃ��ɂ����ɕ�������i�[���Ă�����,
					// ���O���͉�ʂ͂��̕������\�����Ȃ���J�n����B�����Ƃ��Ă��@�\����

	u16 str[20];	// ���̔z��ɂ����ʂ͕Ԃ��Ă��܂��B�i20�ɈӖ��͂���܂���j


	// --------�|�P�����ߊl�Ń{�b�N�X�]���ɂȂ������ɕK�v-------------
	int get_msg_id; 	// �|�P�����ߊl�̎���BOX�]���������������̃��b�Z�[�WID������
	BOX_MANAGER  *boxdata;	// �|�P�����ߊl�̎���BOX�]���������������ɁABOX�̖��O���擾���邽�߂ɕK�v
	CONFIG    *config;  // �R���t�B�O�\����
} NAMEIN_PARAM;

//======================================================================
//	proto
//======================================================================
extern const GFL_PROC_DATA NameInputProcData;

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
//GS����KEYTOUCH_STATUS������Ă܂�
NAMEIN_PARAM *NameIn_ParamAllocMake(int heapId, int mode, int info, int wordmax, CONFIG *config );
void NameIn_ParamDelete(NAMEIN_PARAM *param);


#endif	//NAME_INPUT_H__
