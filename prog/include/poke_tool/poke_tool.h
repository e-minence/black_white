//============================================================================================
/**
 * @file  poke_tool.h
 * @bfief �|�P�����p�����[�^�c�[���S�iWB����Łj
 * @author  HisashiSogabe
 * @date  08.11.12
 */
//============================================================================================
#ifndef __POKE_TOOL_H_
#define __POKE_TOOL_H_

#include <heapsys.h>

#include "poke_tool\poketype.h"
#include "poke_tool\poke_personal.h"

// ����
#define PTL_SEX_MALE    (0)   ///<�I�X
#define PTL_SEX_FEMALE  (1)   ///<���X
#define PTL_SEX_UNKNOWN (2)   ///<���ʂȂ�

#define PTL_LEVEL_MAX         (100)   //���x��MAX
#define PTL_WAZA_MAX          (4)     ///<1�̂̃|�P���������Ă�Z�̍ő�l
#define PTL_WAZAPP_COUNT_MAX  (3)     //pp_countMAX�l

#define PTL_FRIEND_MAX      (255)   //�Ȃ��xMAX�l
#define PTL_STYLE_MAX       (255)   //�������悳MAX�l
#define PTL_BEAUTIFUL_MAX   (255)   //��������MAX�l
#define PTL_CUTE_MAX        (255)   //���킢��MAX�l
#define PTL_CLEVER_MAX      (255)   //��������MAX�l
#define PTL_STRONG_MAX      (255)   //�����܂���MAX�l
#define PTL_FUR_MAX         (255)   //�щ�MAX�l

#define PTL_FORM_NONE      ( 0 )   //�ʃt�H�����Ȃ�

//�\�͒l
#define PTL_ABILITY_HP      (0) ///<�̗�
#define PTL_ABILITY_ATK     (1) ///<�U����
#define PTL_ABILITY_DEF     (2) ///<�h���
#define PTL_ABILITY_SPATK   (3) ///<���U
#define PTL_ABILITY_SPDEF   (4) ///<���h
#define PTL_ABILITY_AGI     (5) ///<�f����
#define PTL_ABILITY_MAX     (6)
//#define ABILITY_HIT     (6) ///<������
//#define ABILITY_AVOID   (7) ///<���

//�w�͒l�Ɋւ����`
#define PARA_EXP_TOTAL_MAX  ( 510 )   //6�̃p�����[�^�̍��v�l��MAX
#define PARA_EXP_ITEM_MAX ( 100 )   //�A�C�e���ŏ㏸�ł�����E�l
#define PARA_EXP_MAX    ( 255 )   //6�̃p�����[�^��MAX

// �}�[�L���O�Ɋւ����`
#define POKEPARA_MARKING_ELEMS_MAX  ( 6 ) ///< �}�[�L���O�Ɏg�p����L���̑���

//PokeItemSet��range������`
#define ITEM_RANGE_NORMAL   ( 0 ) //�ʏ�m��
#define ITEM_RANGE_HUKUGAN    ( 1 ) //�����ӂ�������ʊm��

//�|�P�����̐��i��`
#if 0
#define POKE_PERSONALITY_GANBARIYA    ( 0 )   //����΂��
#define POKE_PERSONALITY_SAMISHIGARI  ( 1 )   //���݂�����
#define POKE_PERSONALITY_YUUKAN     ( 2 )   //�䂤����
#define POKE_PERSONALITY_IJIPPARI   ( 3 )   //�������ς�
#define POKE_PERSONALITY_YANTYA     ( 4 )   //��񂿂�
#define POKE_PERSONALITY_ZUBUTOI    ( 5 )   //���ԂƂ�
#define POKE_PERSONALITY_SUNAO      ( 6 )   //���Ȃ�
#define POKE_PERSONALITY_NONKI      ( 7 )   //�̂�
#define POKE_PERSONALITY_WANPAKU    ( 8 )   //���ς�
#define POKE_PERSONALITY_NOUTENKI   ( 9 )   //�̂��Ă�
#define POKE_PERSONALITY_OKUBYOU    ( 10 )  //�����т傤
#define POKE_PERSONALITY_SEKKATI    ( 11 )  //��������
#define POKE_PERSONALITY_MAJIME     ( 12 )  //�܂���
#define POKE_PERSONALITY_YOUKI      ( 13 )  //�悤��
#define POKE_PERSONALITY_MUJYAKI    ( 14 )  //�ނ��Ⴋ
#define POKE_PERSONALITY_HIKAEME    ( 15 )  //�Ђ�����
#define POKE_PERSONALITY_OTTORI     ( 16 )  //�����Ƃ�
#define POKE_PERSONALITY_REISEI     ( 17 )  //�ꂢ����
#define POKE_PERSONALITY_TEREYA     ( 18 )  //�Ă��
#define POKE_PERSONALITY_UKKARIYA   ( 19 )  //���������
#define POKE_PERSONALITY_ODAYAKA    ( 20 )  //�����₩
#define POKE_PERSONALITY_OTONASHII    ( 21 )  //���ƂȂ���
#define POKE_PERSONALITY_NAMAIKI    ( 22 )  //�Ȃ܂���
#define POKE_PERSONALITY_SINTYOU    ( 23 )  //���񂿂傤
#define POKE_PERSONALITY_KIMAGURE   ( 24 )  //���܂���
#else
#define PTL_SEIKAKU_GANBARIYA    ( 0 )   ///< ����΂��
#define PTL_SEIKAKU_SAMISHIGARI  ( 1 )   //���݂�����
#define PTL_SEIKAKU_YUUKAN       ( 2 )   //�䂤����
#define PTL_SEIKAKU_IJIPPARI     ( 3 )   //�������ς�
#define PTL_SEIKAKU_YANTYA       ( 4 )   //��񂿂�
#define PTL_SEIKAKU_ZUBUTOI      ( 5 )   //���ԂƂ�
#define PTL_SEIKAKU_SUNAO        ( 6 )   //���Ȃ�
#define PTL_SEIKAKU_NONKI        ( 7 )   //�̂�
#define PTL_SEIKAKU_WANPAKU      ( 8 )   //���ς�
#define PTL_SEIKAKU_NOUTENKI     ( 9 )   //�̂��Ă�
#define PTL_SEIKAKU_OKUBYOU      ( 10 )  //�����т傤
#define PTL_SEIKAKU_SEKKATI      ( 11 )  //��������
#define PTL_SEIKAKU_MAJIME       ( 12 )  //�܂���
#define PTL_SEIKAKU_YOUKI        ( 13 )  //�悤��
#define PTL_SEIKAKU_MUJYAKI      ( 14 )  //�ނ��Ⴋ
#define PTL_SEIKAKU_HIKAEME      ( 15 )  //�Ђ�����
#define PTL_SEIKAKU_OTTORI       ( 16 )  //�����Ƃ�
#define PTL_SEIKAKU_REISEI       ( 17 )  //�ꂢ����
#define PTL_SEIKAKU_TEREYA       ( 18 )  //�Ă��
#define PTL_SEIKAKU_UKKARIYA     ( 19 )  //���������
#define PTL_SEIKAKU_ODAYAKA      ( 20 )  //�����₩
#define PTL_SEIKAKU_OTONASHII    ( 21 )  //���ƂȂ���
#define PTL_SEIKAKU_NAMAIKI      ( 22 )  //�Ȃ܂���
#define PTL_SEIKAKU_SINTYOU      ( 23 )  //���񂿂傤
#define PTL_SEIKAKU_KIMAGURE     ( 24 )  //���܂���
#define PTL_SEIKAKU_MAX          ( 25 )
#endif

//=========================================================================
//	�|�P�����R���f�B�V�����r�b�g
//=========================================================================

#define	PTL_CONDITION_NEMURI			      ( 0x00000007 )				//����
#define	PTL_CONDITION_DOKU				      ( 0x00000008 )				//��
#define	PTL_CONDITION_YAKEDO			      ( 0x00000010 )				//�₯��
#define	PTL_CONDITION_KOORI				      ( 0x00000020 )				//������
#define	PTL_CONDITION_MAHI				      ( 0x00000040 )				//�܂�
#define	PTL_CONDITION_DOKUDOKU			    ( 0x00000080 )				//�ǂ��ǂ�
#define	PTL_CONDITION_DOKUDOKU_CNT	    ( 0x00000f00 )				//�ǂ��ǂ��J�E���^
 
#define	PTL_CONDITION_DOKUALL			      ( 0x00000f88 )				//�ǂ��S��

#define	PTL_CONDITION_NEMURI_OFF		    ( 0x00000007 ^ 0xffffffff )		//����
#define	PTL_CONDITION_DOKU_OFF			    ( 0x00000008 ^ 0xffffffff )		//��
#define	PTL_CONDITION_YAKEDO_OFF		    ( 0x00000010 ^ 0xffffffff )		//�₯��
#define	PTL_CONDITION_KOORI_OFF			    ( 0x00000020 ^ 0xffffffff )		//������
#define	PTL_CONDITION_MAHI_OFF			    ( 0x00000040 ^ 0xffffffff )		//�܂�
#define	PTL_CONDITION_DOKUDOKU_CNT_OFF	( 0x00000f00 ^ 0xffffffff )		//�ǂ��ǂ��J�E���^
#define	PTL_CONDITION_POISON_OFF		    ( ( CONDITION_DOKU | CONDITION_DOKUDOKU | CONDITION_DOKUDOKU_CNT ) ^ 0xffffffff )

#define	PTL_DOKUDOKU_COUNT				      ( 0x00000100 )

#define	PTL_CONDITION_BAD				        ( 0x000000ff )

//=========================================================================
//  �|�P�����̃t�H�����i���o�[��`
//=========================================================================
#if 0
//�|������
#define FORMNO_POWARUN_NORMAL   (0)   ///<�m�[�}���|������
#define FORMNO_POWARUN_SUN      (1)   ///<���z�|������
#define FORMNO_POWARUN_RAIN     (2)   ///<�J�|������
#define FORMNO_POWARUN_SNOW     (3)   ///<�����|������

#define FORMNO_POWARUN_MAX      (3)   ///<�|�������̕ʃt�H����MAX

//�~�m���b�`
#define FORMNO_MINOMUTTI_KUSA   (0)   ///<�����݂�
#define FORMNO_MINOMUTTI_SUNA   (1)   ///<���Ȃ݂�
#define FORMNO_MINOMUTTI_TETSU    (2)   ///<�Ă݂�

#define FORMNO_MINOMUTTI_MAX    (2)   ///<�~�m���b�`�̕ʃt�H����MAX

//�`�F���V��
#define FORMNO_THERISYU_NORMAL    (0)   ///<�ڂ݃`�F���V��
#define FORMNO_THERISYU_SUN     (1)   ///<�炢�Ă�`�F���V��

#define FORMNO_THERISYU_MAX     (1)   ///<�`�F���V���̕ʃt�H����MAX

//�V�[�E�V�E�V�[�h���S
#define FORMNO_SII_WEST       (0)   ///<���V�[�E�V�E�V�[�h���S
#define FORMNO_SII_EAST       (1)   ///<���V�[�E�V�E�V�[�h���S

#define FORMNO_SII_MAX        (1)   ///<�V�[�E�V�E�V�[�h���S�̕ʃt�H����MAX

//�V�F�C�~
#define FORMNO_SHEIMI_NORMAL    (0)   ///<�V�F�C�~�m�[�}��
#define FORMNO_SHEIMI_FLOWER    (1)   ///<�V�F�C�~�ԍ炫

#define FORMNO_SHEIMI_MAX     (1)   ///<�V�F�C�~�̕ʃt�H����MAX

//�f�I�L�V�X
#define FORMNO_DEOKISISU_NORMAL   (0)   ///<�m�[�}���t�H����
#define FORMNO_DEOKISISU_ATTACK   (1)   ///<�A�^�b�N�t�H����
#define FORMNO_DEOKISISU_DEFENCE  (2)   ///<�f�B�t�F���X�t�H����
#define FORMNO_DEOKISISU_SPEED    (3)   ///<�X�s�[�h�t�H����

#define FORMNO_DEOKISISU_MAX    (3)   ///<�f�I�L�V�X�̕ʃt�H����MAX

// ���g���̃t�H������`
#define FORMNO_ROTOMU_NORMAL  (0)     // �m�[�}���t�H����
#define FORMNO_ROTOMU_HOT   (1)     // �d�q�����W�t�H����
#define FORMNO_ROTOMU_WASH    (2)     // ����@�t�H����
#define FORMNO_ROTOMU_COLD    (3)     // �①�Ƀt�H����
#define FORMNO_ROTOMU_FAN   (4)     // ��@�t�H����
#define FORMNO_ROTOMU_CUT   (5)     // ������@�t�H����

#define FORMNO_ROTOMU_MAX   (5)     // ���g���̕ʃt�H����MAX

// ���g���̃t�H������`(BIT�w��)
#define FORMNO_ROTOMU_BIT_NORMAL    (1 << FORMNO_ROTOMU_NORMAL)
#define FORMNO_ROTOMU_BIT_HOT     (1 << FORMNO_ROTOMU_HOT)
#define FORMNO_ROTOMU_BIT_WASH      (1 << FORMNO_ROTOMU_WASH)
#define FORMNO_ROTOMU_BIT_COLD      (1 << FORMNO_ROTOMU_COLD)
#define FORMNO_ROTOMU_BIT_FAN     (1 << FORMNO_ROTOMU_FAN)
#define FORMNO_ROTOMU_BIT_CUT     (1 << FORMNO_ROTOMU_CUT)

// �M���e�B�i�t�H������`
#define FORMNO_GIRATINA_NORMAL  (0)     // �m�[�}��
#define FORMNO_GIRATINA_ORIGIN  (1)     // �I���W��

#define FORMNO_GIRATINA_MAX   (1)
#endif


//�Ȃ��v�Z
enum{
  FRIEND_LEVELUP = 0,   //���x���A�b�v
  FRIEND_PARAEXP_ITEM,  //�w�͒l�A�C�e��
  FRIEND_BATTLE_ITEM,   //�퓬���A�C�e��
  FRIEND_BOSS_BATTLE,   //�{�X�퓬
  FRIEND_WAZA_MACHINE,  //�Z�}�V���g�p
  FRIEND_TSUREARUKI,    //�A�����
  FRIEND_HINSHI,      //�m��
  FRIEND_DOKU_HINSHI,   //�łɂ��m��
  FRIEND_LEVEL30_HINSHI,  //���x����30�ȏ�̕m��
  FRIEND_CONTEST_VICTORY, //�R���e�X�g�D��
};

///<�{�b�N�X�|�P�������̍\����
typedef struct pokemon_paso_param POKEMON_PASO_PARAM;
typedef struct pokemon_paso_param1 POKEMON_PASO_PARAM1;
typedef struct pokemon_paso_param2 POKEMON_PASO_PARAM2;
typedef struct pokemon_paso_param3 POKEMON_PASO_PARAM3;
typedef struct pokemon_paso_param4 POKEMON_PASO_PARAM4;

typedef struct pokemon_para_data11 POKEMON_PARA_DATA11;
typedef struct pokemon_para_data12 POKEMON_PARA_DATA12;
typedef struct pokemon_para_data13 POKEMON_PARA_DATA13;
typedef struct pokemon_para_data14 POKEMON_PARA_DATA14;
typedef struct pokemon_para_data15 POKEMON_PARA_DATA15;
typedef struct pokemon_para_data16 POKEMON_PARA_DATA16;
typedef struct pokemon_para_data21 POKEMON_PARA_DATA21;
typedef struct pokemon_para_data22 POKEMON_PARA_DATA22;
typedef struct pokemon_para_data23 POKEMON_PARA_DATA23;
typedef struct pokemon_para_data24 POKEMON_PARA_DATA24;
typedef struct pokemon_para_data25 POKEMON_PARA_DATA25;
typedef struct pokemon_para_data26 POKEMON_PARA_DATA26;
typedef struct pokemon_para_data31 POKEMON_PARA_DATA31;
typedef struct pokemon_para_data32 POKEMON_PARA_DATA32;
typedef struct pokemon_para_data33 POKEMON_PARA_DATA33;
typedef struct pokemon_para_data34 POKEMON_PARA_DATA34;
typedef struct pokemon_para_data35 POKEMON_PARA_DATA35;
typedef struct pokemon_para_data36 POKEMON_PARA_DATA36;
typedef struct pokemon_para_data41 POKEMON_PARA_DATA41;
typedef struct pokemon_para_data42 POKEMON_PARA_DATA42;
typedef struct pokemon_para_data43 POKEMON_PARA_DATA43;
typedef struct pokemon_para_data44 POKEMON_PARA_DATA44;
typedef struct pokemon_para_data45 POKEMON_PARA_DATA45;
typedef struct pokemon_para_data46 POKEMON_PARA_DATA46;

///<�{�b�N�X�|�P�������ȊO�̍\����
typedef struct pokemon_calc_param POKEMON_CALC_PARAM;

///<�莝���|�P�����̍\����
typedef struct pokemon_param POKEMON_PARAM;

///<�|�P�����p�����[�^�f�[�^�擾�̂��߂̃C���f�b�N�X
enum{
  ID_PARA_personal_rnd = 0,         //������
  ID_PARA_pp_fast_mode,           //
  ID_PARA_ppp_fast_mode,            //
  ID_PARA_fusei_tamago_flag,          //�_���^�}�S�t���O
  ID_PARA_checksum,             //�`�F�b�N�T��

  ID_PARA_monsno,               //�����X�^�[�i���o�[
  ID_PARA_item,               //�����A�C�e���i���o�[
  ID_PARA_id_no,                //IDNo
  ID_PARA_exp,                //�o���l
  ID_PARA_friend,               //�Ȃ��x
  ID_PARA_speabino,             //����\��
  ID_PARA_mark,               //�|�P�����ɂ���}�[�N�i�{�b�N�X�j
  ID_PARA_country_code,           //���R�[�h
  ID_PARA_hp_exp,               //HP�w�͒l
  ID_PARA_pow_exp,              //�U���͓w�͒l
  ID_PARA_def_exp,              //�h��͓w�͒l
  ID_PARA_agi_exp,              //�f�����w�͒l
  ID_PARA_spepow_exp,             //���U�w�͒l
  ID_PARA_spedef_exp,             //���h�w�͒l
  ID_PARA_style,                //�������悳
  ID_PARA_beautiful,              //��������
  ID_PARA_cute,               //���킢��
  ID_PARA_clever,               //��������
  ID_PARA_strong,               //�����܂���
  ID_PARA_fur,                //�щ�
  ID_PARA_sinou_champ_ribbon,         //�V���I�E�`�����v���{��
  ID_PARA_sinou_battle_tower_ttwin_first,   //�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
  ID_PARA_sinou_battle_tower_ttwin_second,  //�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
  ID_PARA_sinou_battle_tower_2vs2_win50,    //�V���I�E�o�g���^���[�^���[�_�u��50�A��
  ID_PARA_sinou_battle_tower_aimulti_win50, //�V���I�E�o�g���^���[�^���[AI�}���`50�A��
  ID_PARA_sinou_battle_tower_siomulti_win50,  //�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
  ID_PARA_sinou_battle_tower_wifi_rank5,    //�V���I�E�o�g���^���[Wifi�����N�T����
  ID_PARA_sinou_syakki_ribbon,        //�V���I�E����������{��
  ID_PARA_sinou_dokki_ribbon,         //�V���I�E�ǂ������{��
  ID_PARA_sinou_syonbo_ribbon,        //�V���I�E�����ڃ��{��
  ID_PARA_sinou_ukka_ribbon,          //�V���I�E���������{��
  ID_PARA_sinou_sukki_ribbon,         //�V���I�E���������{��
  ID_PARA_sinou_gussu_ribbon,         //�V���I�E���������{��
  ID_PARA_sinou_nikko_ribbon,         //�V���I�E�ɂ������{��
  ID_PARA_sinou_gorgeous_ribbon,        //�V���I�E�S�[�W���X���{��
  ID_PARA_sinou_royal_ribbon,         //�V���I�E���C�������{��
  ID_PARA_sinou_gorgeousroyal_ribbon,     //�V���I�E�S�[�W���X���C�������{��
  ID_PARA_sinou_ashiato_ribbon,       //�V���I�E�������ƃ��{��
  ID_PARA_sinou_record_ribbon,        //�V���I�E���R�[�h���{��
  ID_PARA_sinou_history_ribbon,       //�V���I�E�q�X�g���[���{��
  ID_PARA_sinou_legend_ribbon,        //�V���I�E���W�F���h���{��
  ID_PARA_sinou_red_ribbon,         //�V���I�E���b�h���{��
  ID_PARA_sinou_green_ribbon,         //�V���I�E�O���[�����{��
  ID_PARA_sinou_blue_ribbon,          //�V���I�E�u���[���{��
  ID_PARA_sinou_festival_ribbon,        //�V���I�E�t�F�X�e�B�o�����{��
  ID_PARA_sinou_carnival_ribbon,        //�V���I�E�J�[�j�o�����{��
  ID_PARA_sinou_classic_ribbon,       //�V���I�E�N���V�b�N���{��
  ID_PARA_sinou_premiere_ribbon,        //�V���I�E�v���~�A���{��
  ID_PARA_sinou_amari_ribbon,         //���܂�

  ID_PARA_waza1,                //�����Z1
  ID_PARA_waza2,                //�����Z2
  ID_PARA_waza3,                //�����Z3
  ID_PARA_waza4,                //�����Z4
  ID_PARA_pp1,                //�����ZPP1
  ID_PARA_pp2,                //�����ZPP2
  ID_PARA_pp3,                //�����ZPP3
  ID_PARA_pp4,                //�����ZPP4
  ID_PARA_pp_count1,              //�����ZPP_COUNT1
  ID_PARA_pp_count2,              //�����ZPP_COUNT2
  ID_PARA_pp_count3,              //�����ZPP_COUNT3
  ID_PARA_pp_count4,              //�����ZPP_COUNT4
  ID_PARA_pp_max1,              //�����ZPPMAX1
  ID_PARA_pp_max2,              //�����ZPPMAX2
  ID_PARA_pp_max3,              //�����ZPPMAX3
  ID_PARA_pp_max4,              //�����ZPPMAX4
  ID_PARA_hp_rnd,               //HP����
  ID_PARA_pow_rnd,              //�U���͗���
  ID_PARA_def_rnd,              //�h��͗���
  ID_PARA_agi_rnd,              //�f��������
  ID_PARA_spepow_rnd,             //���U����
  ID_PARA_spedef_rnd,             //���h����
  ID_PARA_tamago_flag,            //�^�}�S�t���O

  ID_PARA_stylemedal_normal,          //�������悳�M��(�m�[�}��)AGB�R���e�X�g
  ID_PARA_stylemedal_super,         //�������悳�M��(�X�[�p�[)AGB�R���e�X�g
  ID_PARA_stylemedal_hyper,         //�������悳�M��(�n�C�p�[)AGB�R���e�X�g
  ID_PARA_stylemedal_master,          //�������悳�M��(�}�X�^�[)AGB�R���e�X�g
  ID_PARA_beautifulmedal_normal,        //���������M��(�m�[�}��)AGB�R���e�X�g
  ID_PARA_beautifulmedal_super,       //���������M��(�X�[�p�[)AGB�R���e�X�g
  ID_PARA_beautifulmedal_hyper,       //���������M��(�n�C�p�[)AGB�R���e�X�g
  ID_PARA_beautifulmedal_master,        //���������M��(�}�X�^�[)AGB�R���e�X�g
  ID_PARA_cutemedal_normal,         //���킢���M��(�m�[�}��)AGB�R���e�X�g
  ID_PARA_cutemedal_super,          //���킢���M��(�X�[�p�[)AGB�R���e�X�g
  ID_PARA_cutemedal_hyper,          //���킢���M��(�n�C�p�[)AGB�R���e�X�g
  ID_PARA_cutemedal_master,         //���킢���M��(�}�X�^�[)AGB�R���e�X�g
  ID_PARA_clevermedal_normal,         //���������M��(�m�[�}��)AGB�R���e�X�g
  ID_PARA_clevermedal_super,          //���������M��(�X�[�p�[)AGB�R���e�X�g
  ID_PARA_clevermedal_hyper,          //���������M��(�n�C�p�[)AGB�R���e�X�g
  ID_PARA_clevermedal_master,         //���������M��(�}�X�^�[)AGB�R���e�X�g
  ID_PARA_strongmedal_normal,         //�����܂����M��(�m�[�}��)AGB�R���e�X�g
  ID_PARA_strongmedal_super,          //�����܂����M��(�X�[�p�[)AGB�R���e�X�g
  ID_PARA_strongmedal_hyper,          //�����܂����M��(�n�C�p�[)AGB�R���e�X�g
  ID_PARA_strongmedal_master,         //�����܂����M��(�}�X�^�[)AGB�R���e�X�g
  ID_PARA_champ_ribbon,           //�`�����v���{��
  ID_PARA_winning_ribbon,           //�E�B�j���O���{��
  ID_PARA_victory_ribbon,           //�r�N�g���[���{��
  ID_PARA_bromide_ribbon,           //�u���}�C�h���{��
  ID_PARA_ganba_ribbon,           //����΃��{��
  ID_PARA_marine_ribbon,            //�}�������{��
  ID_PARA_land_ribbon,            //�����h���{��
  ID_PARA_sky_ribbon,             //�X�J�C���{��
  ID_PARA_country_ribbon,           //�J���g���[���{��
  ID_PARA_national_ribbon,          //�i�V���i�����{��
  ID_PARA_earth_ribbon,           //�A�[�X���{��
  ID_PARA_world_ribbon,           //���[���h���{��
  ID_PARA_event_get_flag,           //�C�x���g�Ŕz�z���ꂽ���Ƃ������t���O
  ID_PARA_sex,                //����
  ID_PARA_form_no,              //�`��i���o�[�i�A���m�[���A�f�I�L�V�X�A�~�m���X�ȂǗp�j
  ID_PARA_dummy_p2_1,             //���܂�
  ID_PARA_dummy_p2_2,             //���܂�
  ID_PARA_new_get_place,            //1eh �߂܂����ꏊ�i�Ȃ��̏ꏊ�Ή��p�j
  ID_PARA_new_birth_place,          //20h ���܂ꂽ�ꏊ�i�Ȃ��̏ꏊ�Ή��p�j

  ID_PARA_nickname,             //�j�b�N�l�[���iSTRBUF�g�p�j
  ID_PARA_nickname_raw,           //�j�b�N�l�[���iSTRCODE�z��g�p �������j
  ID_PARA_nickname_flag,            //�j�b�N�l�[�����������ǂ����t���O

  ID_PARA_pref_code,              //�s���{���R�[�h
  ID_PARA_get_cassette,           //�߂܂����J�Z�b�g�i�J���[�o�[�W�����j
  ID_PARA_trial_stylemedal_normal,      //�������悳�M��(�m�[�}��)�g���C�A��
  ID_PARA_trial_stylemedal_super,       //�������悳�M��(�X�[�p�[)�g���C�A��
  ID_PARA_trial_stylemedal_hyper,       //�������悳�M��(�n�C�p�[)�g���C�A��
  ID_PARA_trial_stylemedal_master,      //�������悳�M��(�}�X�^�[)�g���C�A��
  ID_PARA_trial_beautifulmedal_normal,    //���������M��(�m�[�}��)�g���C�A��
  ID_PARA_trial_beautifulmedal_super,     //���������M��(�X�[�p�[)�g���C�A��
  ID_PARA_trial_beautifulmedal_hyper,     //���������M��(�n�C�p�[)�g���C�A��
  ID_PARA_trial_beautifulmedal_master,    //���������M��(�}�X�^�[)�g���C�A��
  ID_PARA_trial_cutemedal_normal,       //���킢���M��(�m�[�}��)�g���C�A��
  ID_PARA_trial_cutemedal_super,        //���킢���M��(�X�[�p�[)�g���C�A��
  ID_PARA_trial_cutemedal_hyper,        //���킢���M��(�n�C�p�[)�g���C�A��
  ID_PARA_trial_cutemedal_master,       //���킢���M��(�}�X�^�[)�g���C�A��
  ID_PARA_trial_clevermedal_normal,     //���������M��(�m�[�}��)�g���C�A��
  ID_PARA_trial_clevermedal_super,      //���������M��(�X�[�p�[)�g���C�A��
  ID_PARA_trial_clevermedal_hyper,      //���������M��(�n�C�p�[)�g���C�A��
  ID_PARA_trial_clevermedal_master,     //���������M��(�}�X�^�[)�g���C�A��
  ID_PARA_trial_strongmedal_normal,     //�����܂����M��(�m�[�}��)�g���C�A��
  ID_PARA_trial_strongmedal_super,      //�����܂����M��(�X�[�p�[)�g���C�A��
  ID_PARA_trial_strongmedal_hyper,      //�����܂����M��(�n�C�p�[)�g���C�A��
  ID_PARA_trial_strongmedal_master,     //�����܂����M��(�}�X�^�[)�g���C�A��
  ID_PARA_amari_ribbon,           //�]�胊�{��

  ID_PARA_oyaname,              //�e�̖��O�iSTRBUF�g�p�j
  ID_PARA_oyaname_raw,            //�e�̖��O�iSTRCODE�z��g�p �������j
  ID_PARA_get_year,             //�߂܂����N
  ID_PARA_get_month,              //�߂܂�����
  ID_PARA_get_day,              //�߂܂�����
  ID_PARA_birth_year,             //���܂ꂽ�N
  ID_PARA_birth_month,            //���܂ꂽ��
  ID_PARA_birth_day,              //���܂ꂽ��
  ID_PARA_get_place,              //�߂܂����ꏊ
  ID_PARA_birth_place,            //���܂ꂽ�ꏊ
  ID_PARA_pokerus,              //�|�P���X
  ID_PARA_get_ball,             //�߂܂����{�[��
  ID_PARA_get_level,              //�߂܂������x��
  ID_PARA_oyasex,               //�e�̐���
  ID_PARA_get_ground_id,            //�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
  ID_PARA_dummy_p4_1,             //���܂�

  ID_PARA_condition,              //�R���f�B�V����
  ID_PARA_level,                //���x��
  ID_PARA_cb_id,                //�J�X�^���{�[��ID
  ID_PARA_hp,                 //HP
  ID_PARA_hpmax,                //HPMAX
  ID_PARA_pow,                //�U����
  ID_PARA_def,                //�h���
  ID_PARA_agi,                //�f����
  ID_PARA_spepow,               //���U
  ID_PARA_spedef,               //���h
  ID_PARA_mail_data,              //���[���f�[�^
  ID_PARA_cb_core,              //�J�X�^���{�[���f�[�^

  ID_PARA_poke_exist,             //�|�P�������݃t���O
  ID_PARA_tamago_exist,           //�^�}�S���݃t���O

  ID_PARA_monsno_egg,             //�^�}�S���ǂ����������Ƀ`�F�b�N

  ID_PARA_power_rnd,              //�p���[������u32�ň���
  ID_PARA_nidoran_nickname,         //�����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N

  ID_PARA_type1,                //�|�P�����̃^�C�v�P���擾�i�A�E�X�A�}���`�^�C�v�`�F�b�N������j
  ID_PARA_type2,                //�|�P�����̃^�C�v�Q���擾�i�A�E�X�A�}���`�^�C�v�`�F�b�N������j

  ID_PARA_end                 //�p�����[�^��ǉ�����Ƃ��́A���������ɒǉ�
};


#define PTL_WAZASET_FAIL    (0xffff)  //�Z�̃Z�b�g���ł��Ȃ������Ƃ��̕Ԃ�l
#define PTL_WAZASET_SAME    (0xfffe)  //���łɊo���Ă����Z�̂Ƃ��̕Ԃ�l

///<�|�P�����i���f�[�^�̍\����
typedef struct pokemon_shinka_data POKEMON_SHINKA_DATA;

typedef struct pokemon_shinka_table POKEMON_SHINKA_TABLE;

enum{
  ID_EVO_Cond=0,
  ID_EVO_Data,
  ID_EVO_Mons
};

//�|�P�����p�����[�^�擾�n�̒�`
enum{
  ID_POKEPARADATA11=0,  //0
  ID_POKEPARADATA12,    //1
  ID_POKEPARADATA13,    //2
  ID_POKEPARADATA14,    //3
  ID_POKEPARADATA15,    //4
  ID_POKEPARADATA16,    //5
  ID_POKEPARADATA21,    //6
  ID_POKEPARADATA22,    //7
  ID_POKEPARADATA23,    //8
  ID_POKEPARADATA24,    //9
  ID_POKEPARADATA25,    //10
  ID_POKEPARADATA26,    //11
  ID_POKEPARADATA31,    //12
  ID_POKEPARADATA32,    //13
  ID_POKEPARADATA33,    //14
  ID_POKEPARADATA34,    //15
  ID_POKEPARADATA35,    //16
  ID_POKEPARADATA36,    //17
  ID_POKEPARADATA41,    //18
  ID_POKEPARADATA42,    //19
  ID_POKEPARADATA43,    //20
  ID_POKEPARADATA44,    //21
  ID_POKEPARADATA45,    //22
  ID_POKEPARADATA46,    //23

  ID_POKEPARADATA51,    //24
  ID_POKEPARADATA52,    //25
  ID_POKEPARADATA53,    //26
  ID_POKEPARADATA54,    //27
  ID_POKEPARADATA55,    //28
  ID_POKEPARADATA56,    //29

  ID_POKEPARADATA61,    //30
  ID_POKEPARADATA62,    //31
};

enum{
  ID_POKEPARA1=0,
  ID_POKEPARA2,
  ID_POKEPARA3,
  ID_POKEPARA4,
};

enum{
  UNK_A=0,
  UNK_B,
  UNK_C,
  UNK_D,
  UNK_E,
  UNK_F,
  UNK_G,
  UNK_H,
  UNK_I,
  UNK_J,
  UNK_K,
  UNK_L,
  UNK_M,
  UNK_N,
  UNK_O,
  UNK_P,
  UNK_Q,
  UNK_R,
  UNK_S,
  UNK_T,
  UNK_U,
  UNK_V,
  UNK_W,
  UNK_X,
  UNK_Y,
  UNK_Z,
  UNK_ENC,
  UNK_QUE,
  UNK_END
};

//-----------------------------------------------------------------------------
/**
 *  SetupEx�n�֐��̈���
 */
//-----------------------------------------------------------------------------

/*---------------------------------------------*/
/* pow = �̒l                                */
/*---------------------------------------------*/

typedef  u32  PtlSetupPow;

// �̒l�������_���Ō���
#define PTL_SETUP_POW_AUTO    (0xffffffff)

// �̒l��C�ӂ̒l�ɐݒ肷�邽�߂̕ϐ��p�b�L���O���� (HP�C�U���A�h��A���U�A���h�A�f�����̏��Ɏw��j
static inline u32 PTL_SETUP_POW_PACK( u8 hp, u8 atk, u8 def, u8 sp_atk, u8 sp_def, u8 agi )
{
  return ( (hp&0x1f)|((atk&0x1f)<<5)|((def&0x1f)<<10)|((sp_atk&0x1f)<<15)|((sp_def&0x1f)<<20)|((agi&0x1f)<<25) );
}

// �p�b�L���O���ꂽ�̒l����C�ӎ�ނ̒l�����o���A���p�b�N�����iSetupEx�n�֐������Ŏg�p�j
static inline u8 PTL_SETUP_POW_UNPACK( u32 powrnd, u8 abilityType )
{
  return (powrnd >> (abilityType*5)) & 0x1f;
}

/*---------------------------------------------*/
/* ID                                          */
/*---------------------------------------------*/

// �ȉ��̒l�ȊO���w�肳���΁A���̎w�肳�ꂽ�l���g��
#define PTL_SETUP_ID_AUTO   (0xffffffffffffffff)
#define PTL_SETUP_ID_NOT_RARE  (0xffffffff00000000)  ///< �K���m�[�}���J���[�ɂȂ�悤�ɒ���


/*---------------------------------------------*/
/* rnd = ������                              */
/*---------------------------------------------*/

// �ȉ��̒l�ȊO���w�肳���΁A���̎w�肳�ꂽ�l���g��

#define PTL_SETUP_RND_AUTO    (0xffffffff00000000)  ///< �����������S�Ƀ����_���Ō���
#define PTL_SETUP_RND_RARE    (0xffffffff00000001)  ///< �K�����A�J���[�ɂȂ�悤�ɒ���
#define PTL_SETUP_RND_NOT_RARE  (0xffffffff00000002)  ///< �K���m�[�}���J���[�ɂȂ�悤�ɒ���





extern  void  POKETOOL_InitSystem( HEAPID heapID ); ///< �V�X�e���������i�v���O�����N����ɂP�񂾂��j
extern  u32   POKETOOL_GetWorkSize( void );     ///< POKEMON_PARAM�P�̕��̃T�C�Y�擾
extern u32   POKETOOL_GetPPPWorkSize( void );  ///< POKEMON_PASO_PARAM�̃T�C�Y


//�|�P�����p�����[�^����֐��n


extern  POKEMON_PARAM*  PP_Create( u16 mons_no, u16 level, u64 id, HEAPID heapID ); ///<
extern  POKEMON_PARAM*  PP_CreateByPPP( const POKEMON_PASO_PARAM* ppp, HEAPID heapID );

// ���썂����
// ������� Get, Put ���s���O�� FastModeOn ���Ă����ƁA
// �Í��E��������������s���Ȃ��Ȃ�̂œ��삪�y���Ȃ�܂��B
// �I�������K�� FastModeOff ���邱�ƁI
extern  BOOL  PP_FastModeOn( POKEMON_PARAM *pp );
extern  BOOL  PP_FastModeOff( POKEMON_PARAM *pp, BOOL flag );
extern  BOOL  PPP_FastModeOn( POKEMON_PASO_PARAM *ppp );
extern  BOOL  PPP_FastModeOff( POKEMON_PASO_PARAM *ppp, BOOL flag );


extern  void  PP_Setup( POKEMON_PARAM *pp, u16 mons_no, u16 level, u64 ID );
extern  void  PP_SetupEx( POKEMON_PARAM *pp, u16 mons_no, u16 level, u64 ID, PtlSetupPow pow, u64 rnd );
extern  void  PPP_Setup( POKEMON_PASO_PARAM *ppp, u16 mons_no, u16 level, u64 ID );
extern  void  PPP_SetupEx( POKEMON_PASO_PARAM *ppp, u16 mons_no, u16 level, u64 id, PtlSetupPow pow, u64 rnd );

// �p�����[�^�擾�n
extern  u32   PP_Get( const POKEMON_PARAM *pp, int id, void *buf);
extern  u32   PPP_Get( const POKEMON_PASO_PARAM *ppp, int id, void *buf);
extern  u32   PP_CalcLevel( const POKEMON_PARAM *pp );
extern  u32   PPP_CalcLevel( const POKEMON_PASO_PARAM *ppp );
extern  u8    PP_GetSex( const POKEMON_PARAM *pp );
extern  u8    PPP_GetSex( const POKEMON_PASO_PARAM *ppp );
extern  u32   PP_GetMinExp( const POKEMON_PARAM *pp );
extern  u32   PPP_GetMinExp( const POKEMON_PASO_PARAM *ppp );
extern  u8    PP_GetSeikaku( const POKEMON_PARAM *pp );
extern  u8    PPP_GetSeikaku( const POKEMON_PASO_PARAM *ppp );
extern  BOOL  PP_CheckRare( const POKEMON_PARAM *pp );
extern  BOOL  PPP_CheckRare( const POKEMON_PASO_PARAM *pp );
extern  PokeSick PP_GetSick( const POKEMON_PARAM* pp );

// �p�����[�^�ݒ�n�i�ėp�j
extern  void  PP_Put( POKEMON_PARAM *pp, int id, u32 arg );
extern  void  PPP_Put( POKEMON_PASO_PARAM *ppp, int id, u32 arg );
extern  void  PP_Add( POKEMON_PARAM *pp, int id, int value);
extern  void  PPP_Add( POKEMON_PASO_PARAM *ppp, int id, int value);

// �p�����[�^�ݒ�n�i���U�j
extern  u16   PP_SetWaza( POKEMON_PARAM *pp, u16 wazano );
extern  u16   PPP_SetWaza( POKEMON_PASO_PARAM *ppp, u16 wazano );
extern  void  PP_SetWazaDefault( POKEMON_PARAM *pp );
extern  void  PPP_SetWazaDefault( POKEMON_PASO_PARAM *ppp );
extern  void  PP_SetWazaPush( POKEMON_PARAM *pp, u16 wazano );
extern  void  PPP_SetWazaPush( POKEMON_PASO_PARAM *ppp, u16 wazano );
extern  void  PP_SetWazaPos( POKEMON_PARAM *pp, u16 wazano, u8 pos );
extern  void  PPP_SetWazaPos( POKEMON_PASO_PARAM *ppp, u16 wazano, u8 pos );
extern  void  PP_RecoverWazaPPAll( POKEMON_PARAM* pp );

// �p�����[�^�ݒ�n�i���̑��j
extern  void  PP_SetSick( POKEMON_PARAM* pp, PokeSick sick );


// �̈揉����
extern  void  PP_Clear( POKEMON_PARAM *pp );
extern  void  PPP_Clear( POKEMON_PASO_PARAM *ppp );

// �����X�^�[�i���o�[���������i�i�����Ŏg�p�j
extern void PP_ChangeMonsNo( POKEMON_PARAM* ppp, u16 monsno );
extern void PPP_ChangeMonsNo( POKEMON_PASO_PARAM* ppp, u16 monsno );

// PPP��������PP�������Čv�Z
extern  void  PP_Renew( POKEMON_PARAM *pp );

// PPP�|�C���^���擾
extern  const POKEMON_PASO_PARAM *PP_GetPPPPointerConst( const POKEMON_PARAM *pp );

// �჌�x���֐��Q
extern  u32   POKETOOL_CalcLevel( u16 mons_no, u16 form_no, u32 exp );
extern  u32   POKETOOL_GetMinExp( u16 mons_no, u16 form_no, u16 level );
extern  u8    POKETOOL_GetSex( u16 mons_no, u16 form_no, u32 personal_rnd );
extern  u8    POKETOOL_GetSeikaku( u32 personal_rnd );
extern  BOOL  POKETOOL_CheckRare( u32 id, u32 personal_rnd );
extern  u32   POKETOOL_GetPersonalParam( u16 mons_no, u16 form_no, PokePersonalParamID param );
extern  u32   POKETOOL_CalcPersonalRand( u16 mons_no, u16 form_no, u8 chr, u8 sex );



#endif
