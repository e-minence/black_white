//=============================================================================================
/**
 * @file	personal.c
 * @brief	�|�P����WB  �|�P�����p�[�\�i���f�[�^�A�N�Z�b�T�iDOS�p���쐬�j
 * @author	taya
 *
 * @date	2008.10.20	�쐬
 */
//=============================================================================================

#include "ds_types.h"

#include "poketype.h"
#include "personal.h"


typedef struct {
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

}pokemon_personal_data;


u32 PERSONAL_GetValue( u16 monsno, PersonalValueID  pvid )
{
	static pokemon_personal_data  readBuf;

	char filename[ 64 ];
	FILE* fp;

	sprintf( filename, ".\\personal_bin\\per_%03d.bin", monsno );
	fp = fopen( filename, "rb" );

	if( fp )
	{
		fread( &readBuf, sizeof(readBuf), 1, fp );
		fclose( fp );

		switch( pvid ){
		case PERSONAL_VALUE_TYPE1:				return readBuf.type1;
		case PERSONAL_VALUE_TYPE2:				return readBuf.type2;
		case PERSONAL_VALUE_TOKUSEI1:			return readBuf.speabi1;
		case PERSONAL_VALUE_TOKUSEI2:			return readBuf.speabi2;
		case PERSONAL_VALUE_BASE_HP:			return readBuf.basic_hp;
		case PERSONAL_VALUE_BASE_ATTACK:		return readBuf.basic_pow;
		case PERSONAL_VALUE_BASE_DEFENCE:		return readBuf.basic_def;
		case PERSONAL_VALUE_BASE_AGILITY:		return readBuf.basic_agi;
		case PERSONAL_VALUE_BASE_SP_ATTACK:		return readBuf.basic_spepow;
		case PERSONAL_VALUE_BASE_SP_DEFENCE:	return readBuf.basic_spedef;
		default:
			GF_ASSERT(0);
			break;
		}
	}
	else
	{
		BTL_Printf("PERSONAL IREAGAL MONSNO : %d (%s)\n", monsno, filename);
		GF_ASSERT(0);
	}

	return 1;
}


PokeTypePair  PERSONAL_GetPokeTypePair( u16 monsno )
{
	PokeType  type1, type2;

	type1 = PERSONAL_GetValue( monsno, PERSONAL_VALUE_TYPE1 );
	type2 = PERSONAL_GetValue( monsno, PERSONAL_VALUE_TYPE2 );

	return PokeTypePair_Make( type1, type2 );

}



