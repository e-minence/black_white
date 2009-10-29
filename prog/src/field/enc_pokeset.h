/*
 *  @file   enc_pokeset.c
 *  @brief  �G���J�E���g����쐶�|�P�����f�[�^����
 *  @author Miyuki Iwasawa
 *  @date   09.10.08
 */
#pragma once

#include "field/weather_no.h"

#define FLD_ENCPOKE_NUM_MAX (2) ///<��x�ɃG���J�E���g����|�P������max

///�����_���|�P�������I�^�C�v
typedef enum{
  ENCPROB_CALCTYPE_NORMAL,  ///<�m�[�}��
  ENCPROB_CALCTYPE_WATER,   ///<����
  ENCPROB_CALCTYPE_FISHING, ///<�ނ�
  ENCPROB_CALCTYPE_EQUAL,  ///<1/�w�萔
  ENCPROB_CALCTYPE_MAX,
}ENCPROB_CALCTYPE;

//======================================================================
//  struct
//======================================================================
typedef struct _ENC_POKE_PARAM
{
  u16 monsNo;
  u16 item;
  u8  level;
  u8  form;
  u8  rare;
  u16 waza[4];
}ENC_POKE_PARAM;

typedef struct _ENCPOKE_FLD_PARAM
{
  //���P�[�V������
  ENCOUNT_LOCATION  location;
  ENCOUNT_TYPE      enc_type;

  ENCPROB_CALCTYPE  prob_calctype;  ///<�m���v�Z�^�C�v
  u8                tbl_num;  ///<�G���J�E�g�f�[�^�e�[�u���̗v�f��
  u8                enc_poke_num; //�G���J�E���g������|�P������

  //�莝���擪�|�P�����̃p�����[�^
  u16 mons_no;  //�莝���擪�����X�^�[No
  u16 mons_item;  //�A�C�e���`�F�b�N
	u8  mons_egg_f; //�^�}�S�t���O�`�F�b�N
	u8  mons_spa;  //�莝���擪�����`�F�b�N
//  u8  mons_lv;  //�����X�^�[Lv�`�F�b�N
  u8  mons_sex; //���ʃ`�F�b�N
  u8  mons_chr; //���i�`�F�b�N
	u8  spray_lv; //�ނ��悯�X�v���[�y�ђ჌�x���G���J�E���g����`�F�b�N�ɗp���郌�x��

  u32 myID; //�v���C���[�̃g���[�i�[�h�c
  MYSTATUS* my;

  //�e��t���O
  u32 fishing_f:1;    ///<�ނ�G���J�E���g�t���O
	u32 spray_f:1;     ///<�X�v���[�`�F�b�N���邩�̃t���O	TRUE:�`�F�b�N����
	u32 enc_force_f:1;  ///<�퓬��𖳌�	TRUE:����	FALSE:�L��		(����ł́A���܂�������E���܂��~�c�p)
  u32 companion_f:1;  ///<�A������`�F�b�N
  u32 enc_double_f:1;     ///<�_�u���G���J�E�g

  //������100
  u32 spa_rate_up:1;            ///<�G���J�E���g��2�{(���肶������)
  u32 spa_rate_down:1;          ///<�G���J�E���g��1/2(�������イ��)
  u32 spa_item_rate_up:1;       ///<�A�C�e������ɓ���₷��(�ӂ�����)
  u32 spa_chr_fix:1;            ///<�莝���Ɠ������i(�V���N����)

  //������2/3
  u32 spa_sex_fix:1;      ///<�莝���ƈقȂ鐫��(���������{�f�B��)

  //������1/2
  u32 spa_hagane_up:1;    ///<�n�K�l�^�C�v�Ƃ̃G���J�E���g���A�b�v
  u32 spa_denki_up:1;     ///<�d�C�^�C�v�Ƃ̃G���J�E���g���A�b�v
  u32 spa_high_lv_hit:1;  ///<���x���̍����|�P�����ƃG���J�E���g���₷��(��邫��)
  u32 spa_low_lv_rm:1;    ///<���x���̒Ⴂ�|�P�����ƃG���J�E���g���Ȃ�(�Њd��)

  u32 dmy:18;
}ENCPOKE_FLD_PARAM;

extern void ENCPOKE_SetEFPStruct(ENCPOKE_FLD_PARAM* outEfp, const GAMEDATA* gdata,
    const ENCOUNT_LOCATION location, const ENCOUNT_TYPE enc_type,const WEATHER_NO weather );

extern u32 ENCPOKE_EncProbManipulation(const ENCPOKE_FLD_PARAM* efp, const GAMEDATA* gdata, const u32 inProb);
extern u32 ENCPOKE_GetEncountPoke( const ENCPOKE_FLD_PARAM *efp, const ENC_COMMON_DATA *enc_tbl, ENC_POKE_PARAM* outPokeTbl );
extern int ENCPOKE_GetNormalEncountPokeData( const ENCOUNT_DATA *inData, ENCPOKE_FLD_PARAM* efp, ENC_POKE_PARAM* outPokeTbl);

extern POKEMON_PARAM* ENCPOKE_PPCreate(const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke, int heapID);
extern void ENCPOKE_PPSetup(POKEMON_PARAM* pp,const ENCPOKE_FLD_PARAM* efp, const ENC_POKE_PARAM* poke );

