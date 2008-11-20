//=============================================================================================
/**
 * @file	poke_personal.h
 * @brief	�|�P�����p�[�\�i���f�[�^�A�N�Z�X�֐��Q
 * @author	taya
 *
 * @date	2008.11.06	�쐬
 */
//=============================================================================================
#ifndef __POKE_PERSONAL_H__
#define __POKE_PERSONAL_H__

//--------------------------------------------------------------
/**
 *	�|�P�����p�[�\�i���f�[�^�\���̃n���h���^�̐錾
 */
//--------------------------------------------------------------
typedef struct pokemon_personal_data	POKEMON_PERSONAL_DATA;

//--------------------------------------------------------------
/**
 *	�|�P�����p�[�\�i���f�[�^�擾�̂��߂̃C���f�b�N�X
 */
//--------------------------------------------------------------
typedef enum {

	POKEPER_ID_basic_hp=0,			//��{�g�o
	POKEPER_ID_basic_pow,			//��{�U����
	POKEPER_ID_basic_def,			//��{�h���
	POKEPER_ID_basic_agi,			//��{�f����
	POKEPER_ID_basic_spepow,		//��{����U����
	POKEPER_ID_basic_spedef,		//��{����h���
	POKEPER_ID_type1,				//�����P
	POKEPER_ID_type2,				//�����Q
	POKEPER_ID_get_rate,			//�ߊl��
	POKEPER_ID_give_exp,			//���^�o���l
	POKEPER_ID_pains_hp,			//���^�w�͒l�g�o
	POKEPER_ID_pains_pow,			//���^�w�͒l�U����
	POKEPER_ID_pains_def,			//���^�w�͒l�h���
	POKEPER_ID_pains_agi,			//���^�w�͒l�f����
	POKEPER_ID_pains_spepow,		//���^�w�͒l����U����
	POKEPER_ID_pains_spedef,		//���^�w�͒l����h���
	POKEPER_ID_item1,				//�A�C�e���P
	POKEPER_ID_item2,				//�A�C�e���Q
	POKEPER_ID_sex,					//���ʃx�N�g��
	POKEPER_ID_egg_birth,			//�^�}�S�̛z������
	POKEPER_ID_friend,				//�Ȃ��x�����l
	POKEPER_ID_grow,				//�����Ȑ�����
	POKEPER_ID_egg_group1,			//���Â���O���[�v1
	POKEPER_ID_egg_group2,			//���Â���O���[�v2
	POKEPER_ID_speabi1,				//����\�͂P
	POKEPER_ID_speabi2,				//����\�͂Q
	POKEPER_ID_escape,				//�����闦
	POKEPER_ID_color,				//�F�i�}�ӂŎg�p�j
	POKEPER_ID_reverse,				//���]�t���O
	POKEPER_ID_machine1,			//�Z�}�V���t���O�P
	POKEPER_ID_machine2,			//�Z�}�V���t���O�Q
	POKEPER_ID_machine3,			//�Z�}�V���t���O�R
	POKEPER_ID_machine4				//�Z�}�V���t���O�S

}PokePersonalParamID;


//�p�[�\�i���f�[�^�̐F��`
typedef enum {
	POKEPER_COLOR_RED = 0,		//��
	POKEPER_COLOR_BLUE,			//��
	POKEPER_COLOR_YELLOW,		//��
	POKEPER_COLOR_GREEN,		//��
	POKEPER_COLOR_BLACK,		//��
	POKEPER_COLOR_BROWN,		//��
	POKEPER_COLOR_PERPLE,		//��
	POKEPER_COLOR_GRAY,			//�D
	POKEPER_COLOR_WHITE,		//��
	POKEPER_COLOR_PINK,			//��
}PokePersonal_Color;


//�|�P�����p�[�\�i������֐��n
extern POKEMON_PERSONAL_DATA*  POKE_PERSONAL_OpenHandle( u16 mons_no, u16 form_no, HEAPID heapID );
extern void POKE_PERSONAL_CloseHandle( POKEMON_PERSONAL_DATA* handle );
extern u32 POKE_PERSONAL_GetParam( POKEMON_PERSONAL_DATA *ppd, PokePersonalParamID paramID );


extern	u8		PokePersonal_SexGet( POKEMON_PERSONAL_DATA* personalData, u16 monsno, u32 rnd );



#endif
