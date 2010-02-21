//======================================================================
/**
 * @file  bsubway_battle_data.h
 * @brief  �o�g���T�u�E�F�C�@�o�g���֘A�f�[�^��`
 * @author	HisashiSogabe/Miyuki Iwasawa 
 * @date	06.04.20
 * @note �v���`�i���ڐA kagaya
 */
//======================================================================
#ifndef __H_BSUBWAY_BATTLE_DATA_H__
#define __H_BSUBWAY_BATTLE_DATA_H__

#include "savedata/bsubway_savedata_def.h"

//======================================================================
//  define
//======================================================================
//kari #include "../../../multiboot/src/dl_child/pt_save.h"
#define MONS_NAME_SIZE		10	// �|�P�������̒���(�o�b�t�@�T�C�Y EOM_�܂܂�)
#define	EOM_SIZE			1	// �I���R�[�h�̒���
#define PERSON_NAME_SIZE	7	// �l���̖��O�̒����i�������܂ށj

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�����p�g���[�i�[�f�[�^�\����
/// �g���[�i�[���A��b�f�[�^�́Agmm�ŊǗ��i�g���[�i�[ID����擾�j
//--------------------------------------------------------------
typedef struct
{
	u16		tr_type;				//�g���[�i�[�^�C�v
	u16		use_poke_cnt;			//�g�p�\�|�P������	
	u16		use_poke_table[1];		//�g�p�\�|�P����INDEX�e�[�u���i�ϒ��j
}BSUBWAY_TRAINER_ROM_DATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�����p�|�P�����f�[�^�\����
//--------------------------------------------------------------
typedef struct
{
	u16		mons_no;				//�����X�^�[�i���o�[
	u16		waza[WAZA_TEMOTI_MAX];	//�����Z
	u8		exp_bit;				//�w�͒l�U�蕪���r�b�g
	u8		chr;					//���i
	u16		item_no;				//��������
	u16		form_no;				//�t�H�����i���o�[
}BSUBWAY_POKEMON_ROM_DATA;

//--------------------------------------------------------------
/// Wifi�f�[�^
/// �T�u�E�F�C�p�|�P�����f�[�^�^
/// �Z�[�u�f�[�^�Ƃ���肷��̂�savedata/b_tower.h��typedef��`��؂�
/// �s���S�|�C���^�ł����ł���悤�ɂ��Ă���
//(Dpw_Bt_PokemonData)
//--------------------------------------------------------------
struct _BSUBWAY_POKEMON
{
	union{
		struct{
			u16	mons_no:11;	///<�����X�^�[�i���o�[
			u16	form_no:5;	///<�����X�^�[�i���o�[
		};
		u16	mons_param;
	};
	u16	item_no;	///<��������

	u16	waza[WAZA_TEMOTI_MAX];		///<�����Z

	u32	id_no;					///<IDNo
	u32	personal_rnd;			///<������

	union{
		struct{
		u32	hp_rnd		:5;		///<HP�p���[����
		u32	pow_rnd		:5;		///<POW�p���[����
		u32	def_rnd		:5;		///<DEF�p���[����
		u32	agi_rnd		:5;		///<AGI�p���[����
		u32	spepow_rnd	:5;		///<SPEPOW�p���[����
		u32	spedef_rnd	:5;		///<SPEDEF�p���[����
		u32	ngname_f	:1;		///<NG�l�[���t���O
		u32				:1;		//1ch �]��
		};
		u32 power_rnd;
	};

	union{
		struct{
		u8	hp_exp;				///<HP�w�͒l
		u8	pow_exp;			///<POW�w�͒l
		u8	def_exp;			///<DEF�w�͒l
		u8	agi_exp;			///<AGI�w�͒l
		u8	spepow_exp;			///<SPEPOW�w�͒l
		u8	spedef_exp;			///<SPEDEF�w�͒l
		};
		u8 exp[6];
	};
	union{
		struct{
		u8	pp_count0:2;	///<�Z1�|�C���g�A�b�v
		u8	pp_count1:2;	///<�Z2�|�C���g�A�b�v
		u8	pp_count2:2;	///<�Z3�|�C���g�A�b�v
		u8	pp_count3:2;	///<�Z4�|�C���g�A�b�v
		};
		u8 pp_count;
	};

	u8	country_code;			///<���R�[�h

	u8	tokusei;				///<����
	u8	natuki;				///<�Ȃ��x

	///�j�b�N�l�[�� ((MONS_NAME_SIZE:10)+(EOM_SIZE:1))*(STRCODE:u16)=22
	STRCODE nickname[MONS_NAME_SIZE+EOM_SIZE];
};

//--------------------------------------------------------------
/// �T�u�E�F�C�g���[�i�[�f�[�^
//--------------------------------------------------------------
typedef struct _BSUBWAY_TRAINER
{
	u32		player_id;	///<�g���[�i�[��ID
	u16		tr_type;	///<�g���[�i�[�^�C�v
	u16		dummy;		///<�_�~�[
	STRCODE	name[PERSON_NAME_SIZE+EOM_SIZE];
	
	u16		appear_word[4];						//�o�ꃁ�b�Z�[�W	
	u16		win_word[4];						//�������b�Z�[�W	
	u16		lose_word[4];						//�s�ރ��b�Z�[�W
}BSUBWAY_TRAINER;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�ΐ푊��f�[�^�\����
//--------------------------------------------------------------
struct _BSUBWAY_PARTNER_DATA
{
	BSUBWAY_TRAINER	bt_trd;			//�g���[�i�[�f�[�^
	struct _BSUBWAY_POKEMON	btpwd[4];		//�����|�P�����f�[�^
};

//--------------------------------------------------------------
/// Wifi�o�g���T�u�E�F�C���[�_�[�f�[�^�\����
/// �Z�[�u�f�[�^�Ƃ���肷��̂�savedata/b_tower.h��typedef��`��؂�
/// �s���S�|�C���^�ł����ł���悤�ɂ��Ă���
//(Dpw_Bt_Leader)
//--------------------------------------------------------------
struct _BSUBWAY_LEADER_DATA
{
	STRCODE	name[PERSON_NAME_SIZE+EOM_SIZE];	//�g���[�i�[��((PERSON_NAME_SIZE:7)+(EOM_SIZE:1))*(STRCODE:u16)=16

	u8		casette_version;				//�J�Z�b�g�o�[�W����
	u8		lang_version;					//����o�[�W����
	u8		country_code;					//���R�[�h
	u8		address;						//�Z��ł���Ƃ���

	u8		id_no[4];						//!< �v���C���[ID(u32�^�����\���̃T�C�Y��34�ɂ��邽��u8[4]�ɂ��Ă���)
//	u32		id_no;							//IDNo	

	u16		leader_word[4];					//���[�_�[���b�Z�[�W

	union{
		struct{
			u8	ngname_f:1;	//NG�l�[���t���O(on�Ȃ�NG�l�[��)
			u8	gender	:1;	//����
			u8			:6;	//���܂�6bit
		};
		u8	flags;
	};
	u8	padding;							///<�p�f�B���O
};

//--------------------------------------------------------------
/// �T�u�E�F�CAI�}���`�y�A�̕ۑ����K�v�ȃ|�P�����p�����[�^
/// �Z�[�u�f�[�^�Ƃ���肷��̂�savedata/b_tower.h��typedef��`��؂�
/// �s���S�|�C���^�ł����ł���悤�ɂ��Ă���
//--------------------------------------------------------------
struct _BSUBWAY_PAREPOKE_PARAM
{
  ///<�|�P������id�ۑ�
  u32  poke_id;
  ///<�T�u�E�F�C�f�[�^���|�P�����f�[�^�Q��index0-999
  u16  poke_no[BSUBWAY_STOCK_PAREPOKE_MAX];
  ///<�|�P�����̌������ۑ�
  u32  poke_rnd[BSUBWAY_STOCK_PAREPOKE_MAX];
};

//======================================================================
//  extern
//======================================================================

#endif //__H_BSUBWAY_BATTLE_DATA_H__
