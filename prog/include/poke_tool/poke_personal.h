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

	POKE_PER_ID_basic_hp=0,			//��{�g�o
	POKE_PER_ID_basic_pow,			//��{�U����
	POKE_PER_ID_basic_def,			//��{�h���
	POKE_PER_ID_basic_agi,			//��{�f����
	POKE_PER_ID_basic_spepow,		//��{����U����
	POKE_PER_ID_basic_spedef,		//��{����h���
	POKE_PER_ID_type1,				//�����P
	POKE_PER_ID_type2,				//�����Q
	POKE_PER_ID_get_rate,			//�ߊl��
	POKE_PER_ID_give_exp,			//���^�o���l
	POKE_PER_ID_pains_hp,			//���^�w�͒l�g�o
	POKE_PER_ID_pains_pow,			//���^�w�͒l�U����
	POKE_PER_ID_pains_def,			//���^�w�͒l�h���
	POKE_PER_ID_pains_agi,			//���^�w�͒l�f����
	POKE_PER_ID_pains_spepow,		//���^�w�͒l����U����
	POKE_PER_ID_pains_spedef,		//���^�w�͒l����h���
	POKE_PER_ID_item1,				//�A�C�e���P
	POKE_PER_ID_item2,				//�A�C�e���Q
	POKE_PER_ID_sex,					//���ʃx�N�g��
	POKE_PER_ID_egg_birth,			//�^�}�S�̛z������
	POKE_PER_ID_friend,				//�Ȃ��x�����l
	POKE_PER_ID_grow,				//�����Ȑ�����
	POKE_PER_ID_egg_group1,			//���Â���O���[�v1
	POKE_PER_ID_egg_group2,			//���Â���O���[�v2
	POKE_PER_ID_speabi1,				//����\�͂P
	POKE_PER_ID_speabi2,				//����\�͂Q
	POKE_PER_ID_escape,				//�����闦
	POKE_PER_ID_color,				//�F�i�}�ӂŎg�p�j
	POKE_PER_ID_reverse,				//���]�t���O
	POKE_PER_ID_machine1,			//�Z�}�V���t���O�P
	POKE_PER_ID_machine2,			//�Z�}�V���t���O�Q
	POKE_PER_ID_machine3,			//�Z�}�V���t���O�R
	POKE_PER_ID_machine4				//�Z�}�V���t���O�S

}PokePersonalParamID;



//�|�P�����p�[�\�i������֐��n
extern POKEMON_PERSONAL_DATA*  POKE_PERSONAL_OpenHandle( u16 mons_no, u16 form_no, HEAPID heapID );
extern void POKE_PERSONAL_CloseHandle( POKEMON_PERSONAL_DATA* handle );
extern u32 POKE_PERSONAL_GetParam( POKEMON_PERSONAL_DATA *ppd, PokePersonalParamID paramID );


extern	u8		PokePersonal_SexGet( POKEMON_PERSONAL_DATA* personalData, u16 monsno, u32 rnd );



#endif
