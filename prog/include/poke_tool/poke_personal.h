//=============================================================================================
/**
 * @file  poke_personal.h
 * @brief �|�P�����p�[�\�i���f�[�^�A�N�Z�X�֐��Q
 * @author  taya
 *
 * @date  2008.11.06  �쐬
 */
//=============================================================================================
#ifndef __POKE_PERSONAL_H__
#define __POKE_PERSONAL_H__

//--------------------------------------------------------------
/**
 *  �|�P�����p�[�\�i���f�[�^�\���̃n���h���^�̐錾
 */
//--------------------------------------------------------------
typedef struct pokemon_personal_data  POKEMON_PERSONAL_DATA;

//--------------------------------------------------------------
/**
 *  �|�P�����p�[�\�i���f�[�^�擾�̂��߂̃C���f�b�N�X
 */
//--------------------------------------------------------------
typedef enum {

  POKEPER_ID_basic_hp=0,      //��{�g�o
  POKEPER_ID_basic_pow,       //��{�U����
  POKEPER_ID_basic_def,       //��{�h���
  POKEPER_ID_basic_agi,       //��{�f����
  POKEPER_ID_basic_spepow,    //��{����U����
  POKEPER_ID_basic_spedef,    //��{����h���
  POKEPER_ID_type1,           //�����P
  POKEPER_ID_type2,           //�����Q
  POKEPER_ID_get_rate,        //�ߊl��
  POKEPER_ID_give_exp,        //���^�o���l
  POKEPER_ID_pains_hp,        //���^�w�͒l�g�o
  POKEPER_ID_pains_pow,       //���^�w�͒l�U����
  POKEPER_ID_pains_def,       //���^�w�͒l�h���
  POKEPER_ID_pains_agi,       //���^�w�͒l�f����
  POKEPER_ID_pains_spepow,    //���^�w�͒l����U����
  POKEPER_ID_pains_spedef,    //���^�w�͒l����h���
  POKEPER_ID_item1,           //�A�C�e���P
  POKEPER_ID_item2,           //�A�C�e���Q
  POKEPER_ID_item3,           //�A�C�e���R
  POKEPER_ID_sex,             //���ʃx�N�g��
  POKEPER_ID_egg_birth,       //�^�}�S�̛z������
  POKEPER_ID_friend,          //�Ȃ��x�����l
  POKEPER_ID_grow,            //�����Ȑ�����
  POKEPER_ID_egg_group1,      //���Â���O���[�v1
  POKEPER_ID_egg_group2,      //���Â���O���[�v2
  POKEPER_ID_speabi1,         //����\�͂P
  POKEPER_ID_speabi2,         //����\�͂Q
  POKEPER_ID_speabi3,         //����\�͂R
  POKEPER_ID_escape,          //�����闦
  POKEPER_ID_form_index,      //�ʃt�H�����p�[�\�i���f�[�^�J�n�ʒu
  POKEPER_ID_form_gra_index,  //�ʃt�H�����O���t�B�b�N�f�[�^�J�n�ʒu
  POKEPER_ID_form_max,        //�ʃt�H����MAX
  POKEPER_ID_color,           //�F�i�}�ӂŎg�p�j
  POKEPER_ID_reverse,         //���]�t���O
  POKEPER_ID_pltt_only,       //�ʃt�H�������p���b�g�̂ݕω�
  POKEPER_ID_rank,            //�|�P���������N
  POKEPER_ID_height,          //����
  POKEPER_ID_weight,          //�d��

  POKEPER_ID_machine1,        //�Z�}�V���t���O�P
  POKEPER_ID_machine2,        //�Z�}�V���t���O�Q
  POKEPER_ID_machine3,        //�Z�}�V���t���O�R
  POKEPER_ID_machine4,        //�Z�}�V���t���O�S

  POKEPER_ID_waza_oshie1,     //�Z�����P

}PokePersonalParamID;


//�p�[�\�i���f�[�^�̐F��`
typedef enum {
  POKEPER_COLOR_RED = 0,    //��
  POKEPER_COLOR_BLUE,     //��
  POKEPER_COLOR_YELLOW,   //��
  POKEPER_COLOR_GREEN,    //��
  POKEPER_COLOR_BLACK,    //��
  POKEPER_COLOR_BROWN,    //��
  POKEPER_COLOR_PERPLE,   //��
  POKEPER_COLOR_GRAY,     //�D
  POKEPER_COLOR_WHITE,    //��
  POKEPER_COLOR_PINK,     //��
}PokePersonal_Color;


//--------------------------------------------------------------
/**
 * �|�P�����p�[�\�i���f�[�^�̊֘A�萔
 */
//--------------------------------------------------------------
typedef enum {
  //  ���ʃx�N�g���i�p�[�\�i����̃I�X���X�䗦�j(1�`253�ŃI�X���X�̔䗦���ς��j
  POKEPER_SEX_MALE = 0,     ///<�I�X�̂�
  POKEPER_SEX_FEMALE = 254,   ///<���X�̂�
  POKEPER_SEX_UNKNOWN = 255,    ///<���ʂȂ�
}POKEPER_SEX;

typedef enum{
  //���ʃx�N�g���̃^�C�v(�I�X���X�����_��or�Œ�)
  POKEPER_SEXTYPE_RND,
  POKEPER_SEXTYPE_FIX,
}POKEPER_SEX_TYPE;

enum{
  // ���Â���O���[�v
POKEPER_EGG_GROUP_KAIJUU,     //�P�F���b
POKEPER_EGG_GROUP_SUISEI,     //�Q�F����
POKEPER_EGG_GROUP_MUSI,       //�R�F��
POKEPER_EGG_GROUP_TORI,       // �S�F��
POKEPER_EGG_GROUP_DOUBUTU,    //�T�F����
POKEPER_EGG_GROUP_YOUSEI,     // �U�F�d��
POKEPER_EGG_GROUP_SYOKUBUTU,  // �V�F�A��
POKEPER_EGG_GROUP_HITOGATA,   // �W�F�l�^
POKEPER_EGG_GROUP_KOUKAKU,    // �X�F�b�k
POKEPER_EGG_GROUP_MUKIBUTU,   // 10�F���@��
POKEPER_EGG_GROUP_FUTEIKEI,   // 11�F�s��`
POKEPER_EGG_GROUP_SAKANA,     // 12�F��
POKEPER_EGG_GROUP_METAMON,    // 13�F���^����
POKEPER_EGG_GROUP_DORAGON,    // 14�F�h���S��
POKEPER_EGG_GROUP_MUSEISYOKU, //15�F�����B
};
//--------------------------------------------------------------
/**
 * �|�P�������U�o���f�[�^�֘A
 */
//--------------------------------------------------------------

///< ���U�o���f�[�^�R�[�h
typedef struct
{ 
  u16 waza_no;    //�o����Z�i���o�[
  u16 level;      //�o���郌�x��
}POKEPER_WAZAOBOE_CODE;

enum {
  POKEPER_WAZAOBOE_TABLE_ELEMS = 26,  ///< ���U���ڂ��e�[�u���̃��[�h�ɕK�v�ȗv�f��
  POKEPER_WAZAOBOE_ENDCODE = 0xffff,
};

// ���U�o���f�[�^�R�[�h�F�I�[�`�F�b�N
static inline BOOL POKEPER_WAZAOBOE_IsEndCode( POKEPER_WAZAOBOE_CODE code )
{
  return ( ( code.waza_no == POKEPER_WAZAOBOE_ENDCODE ) && ( code.level == POKEPER_WAZAOBOE_ENDCODE ) );
}
// ���U�o���f�[�^�R�[�h�F���x���擾
static inline u16 POKEPER_WAZAOBOE_GetLevel( POKEPER_WAZAOBOE_CODE code )
{
  return code.level;
}
// ���U�o���f�[�^�R�[�h�F���UID�擾
static inline u16 POKEPER_WAZAOBOE_GetWazaID( POKEPER_WAZAOBOE_CODE code )
{
  return code.waza_no;
}




//�|�P�����p�[�\�i������֐��n
extern POKEMON_PERSONAL_DATA*  POKE_PERSONAL_OpenHandle( u16 mons_no, u16 form_no, HEAPID heapID );
extern void POKE_PERSONAL_CloseHandle( POKEMON_PERSONAL_DATA* handle );
extern u32 POKE_PERSONAL_GetParam( POKEMON_PERSONAL_DATA *ppd, PokePersonalParamID paramID );

// ���U���ڂ��e�[�u�����[�h�idst �ɂ� POKEPER_WAZAOBOE_CODE * POKEPER_WAZAOBOETABLE_ELEMS �̔z���n�����Ɓj
extern void POKE_PERSONAL_LoadWazaOboeTable( u16 mons_no, u16 form_no, POKEPER_WAZAOBOE_CODE* dst );

// ���ʎ擾
extern  u8    PokePersonal_SexGet( POKEMON_PERSONAL_DATA* personalData, u16 monsno, u32 rnd );
extern POKEPER_SEX_TYPE PokePersonal_SexVecTypeGet( POKEPER_SEX sex_vec );

#endif
