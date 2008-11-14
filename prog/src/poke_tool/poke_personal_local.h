//=============================================================================================
/**
 * @file	poke_personal.h
 * @brief	�|�P�����p�[�\�i���f�[�^�A�N�Z�X�֐��Q�ipoke_tool�O���[�v���j
 * @author	taya
 *
 * @date	2008.11.06	�쐬
 */
//=============================================================================================
#ifndef __POKE_PERSONAL_LOCAL_H__
#define __POKE_PERSONAL_LOCAL_H__

#include    "poke_tool/poke_personal.h"
#include    "poke_tool/monsno_def.h"


//--------------------------------------------------------------
/**
 * �|�P�����p�[�\�i���f�[�^�̍\���̒�`
 */
//--------------------------------------------------------------
struct	pokemon_personal_data
{
	u8		basic_hp;			//��{�g�o
	u8		basic_pow;			//��{�U����
	u8		basic_def;			//��{�h���
	u8		basic_agi;			//��{�f����

	u8		basic_spepow;		//��{����U����
	u8		basic_spedef;		//��{����h���
	u8		type1;				//�����P
	u8		type2;				//�����Q

	u8		get_rate;			//�ߊl��
	u8		give_exp;			//���^�o���l

	u16		pains_hp	:2;		//���^�w�͒l�g�o
	u16		pains_pow	:2;		//���^�w�͒l�U����
	u16		pains_def	:2;		//���^�w�͒l�h���
	u16		pains_agi	:2;		//���^�w�͒l�f����
	u16		pains_spepow:2;		//���^�w�͒l����U����
	u16		pains_spedef:2;		//���^�w�͒l����h���
	u16					:4;		//���^�w�͒l�\��

	u16		item1;				//�A�C�e���P
	u16		item2;				//�A�C�e���Q

	u8		sex;				//���ʃx�N�g��
	u8		egg_birth;			//�^�}�S�̛z������
	u8		friend;				//�Ȃ��x�����l
	u8		grow;				//�����Ȑ�����

	u8		egg_group1;			//���Â���O���[�v1
	u8		egg_group2;			//���Â���O���[�v2
	u8		speabi1;			//����\�͂P
	u8		speabi2;			//����\�͂Q

	u8		escape;				//�����闦
	u8		color	:7;			//�F�i�}�ӂŎg�p�j
	u8		reverse	:1;			//���]�t���O
	u32		machine1;			//�Z�}�V���t���O�P
	u32		machine2;			//�Z�}�V���t���O�Q
	u32		machine3;			//�Z�}�V���t���O�Q
	u32		machine4;			//�Z�}�V���t���O�Q
};

//--------------------------------------------------------------
/**
 * �|�P�����p�[�\�i���f�[�^�̊֘A�萔
 */
//--------------------------------------------------------------
enum {
	//	���ʃx�N�g���i�p�[�\�i����̃I�X���X�䗦�j(1�`253�ŃI�X���X�̔䗦���ς��j
	POKEPER_SEX_MALE = 0,			///<�I�X�̂�
	POKEPER_SEX_FEMALE = 254,		///<���X�̂�
	POKEPER_SEX_UNKNOWN = 255,		///<���ʂȂ�
};

//=============================================================================================
/**
 * �V�X�e���������i�v���O�����N����ɂP�x�����Ăяo���j
 *
 * @param   heapID		�V�X�e���������p�q�[�vID�i�A�[�J�C�u�n���h���p�ɐ��\�o�C�g�K�v�j
 */
//=============================================================================================
extern void POKE_PERSONAL_InitSystem( HEAPID heapID );


extern void POKE_PERSONAL_LoadData( u16 mons_no, u16 form_no, POKEMON_PERSONAL_DATA* ppd );
extern void POKE_PERSONAL_LoadWazaOboeTable( u16 mons_no, u16 form_no, u16* dst );

extern u16 POKE_PERSONAL_GetPersonalID( u16 mons_no, u16 form_no );



#endif
