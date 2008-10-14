/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     base64codec.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include "poketool.h"
#include "dpw_assert.h"
#include "monsno.h"
#include "trans_pokecode.h"
#include "tokusyu_def.h"
#include "itemsym.h"

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/

#define	NELEMS(array)	( sizeof(array) / sizeof(array[0]) )

#define	ARC_HEAD_SIZE_POS	(12)
#define	FAT_HEAD_SIZE		(12)
#define	IMG_HEAD_SIZE		(8)
#define	SIZE_OFFSET			(4)

/*======== �|�P�����̃\�[�X����R�s�[���Ă����A�p�����[�^�擾�̂��߂̌^ ========*/

#define	OFS_NO_SET		(0)		// ArchiveDataLoadOfs,ArchiveDataLoadOfs�pOFS�l�Ȃ��̒萔
#define	SIZE_NO_SET		(0)		// ArchiveDataLoadOfs,ArchiveDataLoadOfs�pSIZE�l�Ȃ��̒萔

// �|�P�����p�[�\�i���f�[�^�̍\����
typedef struct pokemon_personal_data POKEMON_PERSONAL_DATA;

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

enum {
	ID_POKEPARA1 = 0,
	ID_POKEPARA2,
	ID_POKEPARA3,
	ID_POKEPARA4,
};

// �|�P�����p�[�\�i���f�[�^�̍\���̐錾
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
	u8		_friend;			//�Ȃ��x�����l
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

// �|�P�����p�����[�^�擾�n�̒�`
enum{
	ID_POKEPARADATA11=0,	//0
	ID_POKEPARADATA12,		//1
	ID_POKEPARADATA13,		//2
	ID_POKEPARADATA14,		//3
	ID_POKEPARADATA15,		//4
	ID_POKEPARADATA16,		//5
	ID_POKEPARADATA21,		//6
	ID_POKEPARADATA22,		//7
	ID_POKEPARADATA23,		//8
	ID_POKEPARADATA24,		//9
	ID_POKEPARADATA25,		//10
	ID_POKEPARADATA26,		//11
	ID_POKEPARADATA31,		//12
	ID_POKEPARADATA32,		//13
	ID_POKEPARADATA33,		//14
	ID_POKEPARADATA34,		//15
	ID_POKEPARADATA35,		//16
	ID_POKEPARADATA36,		//17
	ID_POKEPARADATA41,		//18
	ID_POKEPARADATA42,		//19
	ID_POKEPARADATA43,		//20
	ID_POKEPARADATA44,		//21
	ID_POKEPARADATA45,		//22
	ID_POKEPARADATA46,		//23

	ID_POKEPARADATA51,		//24
	ID_POKEPARADATA52,		//25
	ID_POKEPARADATA53,		//26
	ID_POKEPARADATA54,		//27
	ID_POKEPARADATA55,		//28
	ID_POKEPARADATA56,		//29

	ID_POKEPARADATA61,		//30
	ID_POKEPARADATA62,		//31
};

struct pokemon_para_data11
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data12
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data13
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data14
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data15
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data16
{
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data21
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data22
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data23
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data24
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data25
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data26
{
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data31
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data32
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data33
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM4	ppp4;
};

struct pokemon_para_data34
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data35
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data36
{
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data41
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data42
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data43
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM3	ppp3;
};

struct pokemon_para_data44
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
};

struct pokemon_para_data45
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM1	ppp1;
	POKEMON_PASO_PARAM2	ppp2;
};

struct pokemon_para_data46
{
	POKEMON_PASO_PARAM4	ppp4;
	POKEMON_PASO_PARAM3	ppp3;
	POKEMON_PASO_PARAM2	ppp2;
	POKEMON_PASO_PARAM1	ppp1;
};

// �|�P�����p�[�\�i���f�[�^�擾�̂��߂̃C���f�b�N�X
enum{
	ID_PER_basic_hp=0,			//��{�g�o
	ID_PER_basic_pow,			//��{�U����
	ID_PER_basic_def,			//��{�h���
	ID_PER_basic_agi,			//��{�f����
	ID_PER_basic_spepow,		//��{����U����
	ID_PER_basic_spedef,		//��{����h���
	ID_PER_type1,				//�����P
	ID_PER_type2,				//�����Q
	ID_PER_get_rate,			//�ߊl��
	ID_PER_give_exp,			//���^�o���l
	ID_PER_pains_hp,			//���^�w�͒l�g�o
	ID_PER_pains_pow,			//���^�w�͒l�U����
	ID_PER_pains_def,			//���^�w�͒l�h���
	ID_PER_pains_agi,			//���^�w�͒l�f����
	ID_PER_pains_spepow,		//���^�w�͒l����U����
	ID_PER_pains_spedef,		//���^�w�͒l����h���
	ID_PER_item1,				//�A�C�e���P
	ID_PER_item2,				//�A�C�e���Q
	ID_PER_sex,					//���ʃx�N�g��
	ID_PER_egg_birth,			//�^�}�S�̛z������
	ID_PER_friend,				//�Ȃ��x�����l
	ID_PER_grow,				//�����Ȑ�����
	ID_PER_egg_group1,			//���Â���O���[�v1
	ID_PER_egg_group2,			//���Â���O���[�v2
	ID_PER_speabi1,				//����\�͂P
	ID_PER_speabi2,				//����\�͂Q
	ID_PER_escape,				//�����闦
	ID_PER_color,				//�F�i�}�ӂŎg�p�j
	ID_PER_reverse,				//���]�t���O
	ID_PER_machine1,			//�Z�}�V���t���O�P
	ID_PER_machine2,			//�Z�}�V���t���O�Q
	ID_PER_machine3,			//�Z�}�V���t���O�R
	ID_PER_machine4				//�Z�}�V���t���O�S
};

/*-----------------------------------------------------------------------*
					�֐��v���g�^�C�v�錾
 *-----------------------------------------------------------------------*/

static u16 PokeParaCheckSum(void *data, u32 size);
static void PokeParaDecoded(void *data, u32 size, u32 code);
static void PokeParaCoded(void *data, u32 size, u32 code);
static u16 CodeRand(u32 *code);
static u32 PokeParaGetAct(POKEMON_PARAM *pp, int id, void *buf);
static u32 PokePasoParaGetAct(POKEMON_PASO_PARAM *ppp,int id, void *buf);
static void *PokeParaAdrsGet(POKEMON_PASO_PARAM *ppp, u32 rnd, u8 id);
static u32 PokeLevelCalc(u16 mons_no, u32 exp);
static POKEMON_PERSONAL_DATA* PokePersonalPara_Open(int mons_no);
static void PokePersonalDataGet(int mons_no, POKEMON_PERSONAL_DATA *ppd);
static void ArchiveDataLoad(void *data, const char* file, int datID);
static void ArchiveDataLoadIndex(void *data, const char *name, int index, int ofs, int ofs_size);
static u32 PokePersonal_LevelCalc(POKEMON_PERSONAL_DATA* personalData, u16 monsno, u32 exp);
static u32 PokePersonalPara_Get(POKEMON_PERSONAL_DATA *ppd, int para);
static void PokeGrowDataGet(int para, u32 *grow_tbl);
static void PokePersonalPara_Close(POKEMON_PERSONAL_DATA *ppd);
static void PokeParaPutAct(POKEMON_PARAM *pp, int id, const void *buf);
static void PokePasoParaPutAct(POKEMON_PASO_PARAM *ppp, int id, const void *buf);
static u8 PokeRareGetPara(u32 id,u32 rnd);

/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/

#ifdef WIN32

static const wchar_t* pokemon_default_name[MONSNO_END][COUNTRY_CODE_MAX] = {
	{ L"�t�V�M�_�l",	L"BULBASAUR",	L"BULBIZARRE",	L"BULBASAUR",	L"BISASAM",	L"",	L"BULBASAUR",	L"BULBASAUR" },
	{ L"�t�V�M�\�E",	L"IVYSAUR",	L"HERBIZARRE",	L"IVYSAUR",	L"BISAKNOSP",	L"",	L"IVYSAUR",	L"IVYSAUR" },
	{ L"�t�V�M�o�i",	L"VENUSAUR",	L"FLORIZARRE",	L"VENUSAUR",	L"BISAFLOR",	L"",	L"VENUSAUR",	L"VENUSAUR" },
	{ L"�q�g�J�Q",	L"CHARMANDER",	L"SALAMECHE",	L"CHARMANDER",	L"GLUMANDA",	L"",	L"CHARMANDER",	L"CHARMANDER" },
	{ L"���U�|�h",	L"CHARMELEON",	L"REPTINCEL",	L"CHARMELEON",	L"GLUTEXO",	L"",	L"CHARMELEON",	L"CHARMELEON" },
	{ L"���U�|�h��",	L"CHARIZARD",	L"DRACAUFEU",	L"CHARIZARD",	L"GLURAK",	L"",	L"CHARIZARD",	L"CHARIZARD" },
	{ L"�[�j�K��",	L"SQUIRTLE",	L"CARAPUCE",	L"SQUIRTLE",	L"SCHIGGY",	L"",	L"SQUIRTLE",	L"SQUIRTLE" },
	{ L"�J���|��",	L"WARTORTLE",	L"CARABAFFE",	L"WARTORTLE",	L"SCHILLOK",	L"",	L"WARTORTLE",	L"WARTORTLE" },
	{ L"�J���b�N�X",	L"BLASTOISE",	L"TORTANK",	L"BLASTOISE",	L"TURTOK",	L"",	L"BLASTOISE",	L"BLASTOISE" },
	{ L"�L���^�s�|",	L"CATERPIE",	L"CHENIPAN",	L"CATERPIE",	L"RAUPY",	L"",	L"CATERPIE",	L"CATERPIE" },
	{ L"�g�����Z��",	L"METAPOD",	L"CHRYSACIER",	L"METAPOD",	L"SAFCON",	L"",	L"METAPOD",	L"METAPOD" },
	{ L"�o�^�t���|",	L"BUTTERFREE",	L"PAPILUSION",	L"BUTTERFREE",	L"SMETTBO",	L"",	L"BUTTERFREE",	L"BUTTERFREE" },
	{ L"�r�|�h��",	L"WEEDLE",	L"ASPICOT",	L"WEEDLE",	L"HORNLIU",	L"",	L"WEEDLE",	L"WEEDLE" },
	{ L"�R�N�|��",	L"KAKUNA",	L"COCONFORT",	L"KAKUNA",	L"KOKUNA",	L"",	L"KAKUNA",	L"KAKUNA" },
	{ L"�X�s�A�|",	L"BEEDRILL",	L"DARDARGNAN",	L"BEEDRILL",	L"BIBOR",	L"",	L"BEEDRILL",	L"BEEDRILL" },
	{ L"�|�b�|",	L"PIDGEY",	L"ROUCOOL",	L"PIDGEY",	L"TAUBSI",	L"",	L"PIDGEY",	L"PIDGEY" },
	{ L"�s�W����",	L"PIDGEOTTO",	L"ROUCOUPS",	L"PIDGEOTTO",	L"TAUBOGA",	L"",	L"PIDGEOTTO",	L"PIDGEOTTO" },
	{ L"�s�W���b�g",	L"PIDGEOT",	L"ROUCARNAGE",	L"PIDGEOT",	L"TAUBOSS",	L"",	L"PIDGEOT",	L"PIDGEOT" },
	{ L"�R���b�^",	L"RATTATA",	L"RATTATA",	L"RATTATA",	L"RATTFRATZ",	L"",	L"RATTATA",	L"RATTATA" },
	{ L"���b�^",	L"RATICATE",	L"RATTATAC",	L"RATICATE",	L"RATTIKARL",	L"",	L"RATICATE",	L"RATICATE" },
	{ L"�I�j�X�Y��",	L"SPEAROW",	L"PIAFABEC",	L"SPEAROW",	L"HABITAK",	L"",	L"SPEAROW",	L"SPEAROW" },
	{ L"�I�j�h����",	L"FEAROW",	L"RAPASDEPIC",	L"FEAROW",	L"IBITAK",	L"",	L"FEAROW",	L"FEAROW" },
	{ L"�A�|�{",	L"EKANS",	L"ABO",	L"EKANS",	L"RETTAN",	L"",	L"EKANS",	L"EKANS" },
	{ L"�A�|�{�b�N",	L"ARBOK",	L"ARBOK",	L"ARBOK",	L"ARBOK",	L"",	L"ARBOK",	L"ARBOK" },
	{ L"�s�J�`���E",	L"PIKACHU",	L"PIKACHU",	L"PIKACHU",	L"PIKACHU",	L"",	L"PIKACHU",	L"PIKACHU" },
	{ L"���C�`���E",	L"RAICHU",	L"RAICHU",	L"RAICHU",	L"RAICHU",	L"",	L"RAICHU",	L"RAICHU" },
	{ L"�T���h",	L"SANDSHREW",	L"SABELETTE",	L"SANDSHREW",	L"SANDAN",	L"",	L"SANDSHREW",	L"SANDSHREW" },
	{ L"�T���h�p��",	L"SANDSLASH",	L"SABLAIREAU",	L"SANDSLASH",	L"SANDAMER",	L"",	L"SANDSLASH",	L"SANDSLASH" },
	{ L"�j�h������",	L"NIDORAN>",	L"NIDORAN>",	L"NIDORAN>",	L"NIDORAN>",	L"",	L"NIDORAN>",	L"NIDORAN>" },
	{ L"�j�h���|�i",	L"NIDORINA",	L"NIDORINA",	L"NIDORINA",	L"NIDORINA",	L"",	L"NIDORINA",	L"NIDORINA" },
	{ L"�j�h�N�C��",	L"NIDOQUEEN",	L"NIDOQUEEN",	L"NIDOQUEEN",	L"NIDOQUEEN",	L"",	L"NIDOQUEEN",	L"NIDOQUEEN" },
	{ L"�j�h������",	L"NIDORAN<",	L"NIDORAN<",	L"NIDORAN<",	L"NIDORAN<",	L"",	L"NIDORAN<",	L"NIDORAN<" },
	{ L"�j�h���|�m",	L"NIDORINO",	L"NIDORINO",	L"NIDORINO",	L"NIDORINO",	L"",	L"NIDORINO",	L"NIDORINO" },
	{ L"�j�h�L���O",	L"NIDOKING",	L"NIDOKING",	L"NIDOKING",	L"NIDOKING",	L"",	L"NIDOKING",	L"NIDOKING" },
	{ L"�s�b�s",	L"CLEFAIRY",	L"MELOFEE",	L"CLEFAIRY",	L"PIEPI",	L"",	L"CLEFAIRY",	L"CLEFAIRY" },
	{ L"�s�N�V�|",	L"CLEFABLE",	L"MELODELFE",	L"CLEFABLE",	L"PIXI",	L"",	L"CLEFABLE",	L"CLEFABLE" },
	{ L"���R��",	L"VULPIX",	L"GOUPIX",	L"VULPIX",	L"VULPIX",	L"",	L"VULPIX",	L"VULPIX" },
	{ L"�L���E�R��",	L"NINETALES",	L"FEUNARD",	L"NINETALES",	L"VULNONA",	L"",	L"NINETALES",	L"NINETALES" },
	{ L"�v����",	L"JIGGLYPUFF",	L"RONDOUDOU",	L"JIGGLYPUFF",	L"PUMMELUFF",	L"",	L"JIGGLYPUFF",	L"JIGGLYPUFF" },
	{ L"�v�N����",	L"WIGGLYTUFF",	L"GRODOUDOU",	L"WIGGLYTUFF",	L"KNUDDELUFF",	L"",	L"WIGGLYTUFF",	L"WIGGLYTUFF" },
	{ L"�Y�o�b�g",	L"ZUBAT",	L"NOSFERAPTI",	L"ZUBAT",	L"ZUBAT",	L"",	L"ZUBAT",	L"ZUBAT" },
	{ L"�S���o�b�g",	L"GOLBAT",	L"NOSFERALTO",	L"GOLBAT",	L"GOLBAT",	L"",	L"GOLBAT",	L"GOLBAT" },
	{ L"�i�]�m�N�T",	L"ODDISH",	L"MYSTHERBE",	L"ODDISH",	L"MYRAPLA",	L"",	L"ODDISH",	L"ODDISH" },
	{ L"�N�T�C�n�i",	L"GLOOM",	L"ORTIDE",	L"GLOOM",	L"DUFLOR",	L"",	L"GLOOM",	L"GLOOM" },
	{ L"���t���V�A",	L"VILEPLUME",	L"RAFFLESIA",	L"VILEPLUME",	L"GIFLOR",	L"",	L"VILEPLUME",	L"VILEPLUME" },
	{ L"�p���X",	L"PARAS",	L"PARAS",	L"PARAS",	L"PARAS",	L"",	L"PARAS",	L"PARAS" },
	{ L"�p���Z�N�g",	L"PARASECT",	L"PARASECT",	L"PARASECT",	L"PARASEK",	L"",	L"PARASECT",	L"PARASECT" },
	{ L"�R���p��",	L"VENONAT",	L"MIMITOSS",	L"VENONAT",	L"BLUZUK",	L"",	L"VENONAT",	L"VENONAT" },
	{ L"�����t�H��",	L"VENOMOTH",	L"AEROMITE",	L"VENOMOTH",	L"OMOT",	L"",	L"VENOMOTH",	L"VENOMOTH" },
	{ L"�f�B�O�_",	L"DIGLETT",	L"TAUPIQUEUR",	L"DIGLETT",	L"DIGDA",	L"",	L"DIGLETT",	L"DIGLETT" },
	{ L"�_�O�g���I",	L"DUGTRIO",	L"TRIOPIKEUR",	L"DUGTRIO",	L"DIGDRI",	L"",	L"DUGTRIO",	L"DUGTRIO" },
	{ L"�j���|�X",	L"MEOWTH",	L"MIAOUSS",	L"MEOWTH",	L"MAUZI",	L"",	L"MEOWTH",	L"MEOWTH" },
	{ L"�y���V�A��",	L"PERSIAN",	L"PERSIAN",	L"PERSIAN",	L"SNOBILIKAT",	L"",	L"PERSIAN",	L"PERSIAN" },
	{ L"�R�_�b�N",	L"PSYDUCK",	L"PSYKOKWAK",	L"PSYDUCK",	L"ENTON",	L"",	L"PSYDUCK",	L"PSYDUCK" },
	{ L"�S���_�b�N",	L"GOLDUCK",	L"AKWAKWAK",	L"GOLDUCK",	L"ENTORON",	L"",	L"GOLDUCK",	L"GOLDUCK" },
	{ L"�}���L�|",	L"MANKEY",	L"FEROSINGE",	L"MANKEY",	L"MENKI",	L"",	L"MANKEY",	L"MANKEY" },
	{ L"�I�R���U��",	L"PRIMEAPE",	L"COLOSSINGE",	L"PRIMEAPE",	L"RASAFF",	L"",	L"PRIMEAPE",	L"PRIMEAPE" },
	{ L"�K�|�f�B",	L"GROWLITHE",	L"CANINOS",	L"GROWLITHE",	L"FUKANO",	L"",	L"GROWLITHE",	L"GROWLITHE" },
	{ L"�E�C���f�B",	L"ARCANINE",	L"ARCANIN",	L"ARCANINE",	L"ARKANI",	L"",	L"ARCANINE",	L"ARCANINE" },
	{ L"�j������",	L"POLIWAG",	L"PTITARD",	L"POLIWAG",	L"QUAPSEL",	L"",	L"POLIWAG",	L"POLIWAG" },
	{ L"�j�����]",	L"POLIWHIRL",	L"TETARTE",	L"POLIWHIRL",	L"QUAPUTZI",	L"",	L"POLIWHIRL",	L"POLIWHIRL" },
	{ L"�j�����{��",	L"POLIWRATH",	L"TARTARD",	L"POLIWRATH",	L"QUAPPO",	L"",	L"POLIWRATH",	L"POLIWRATH" },
	{ L"�P�|�V�B",	L"ABRA",	L"ABRA",	L"ABRA",	L"ABRA",	L"",	L"ABRA",	L"ABRA" },
	{ L"�����Q���|",	L"KADABRA",	L"KADABRA",	L"KADABRA",	L"KADABRA",	L"",	L"KADABRA",	L"KADABRA" },
	{ L"�t�|�f�B��",	L"ALAKAZAM",	L"ALAKAZAM",	L"ALAKAZAM",	L"SIMSALA",	L"",	L"ALAKAZAM",	L"ALAKAZAM" },
	{ L"�������L�|",	L"MACHOP",	L"MACHOC",	L"MACHOP",	L"MACHOLLO",	L"",	L"MACHOP",	L"MACHOP" },
	{ L"�S�|���L�|",	L"MACHOKE",	L"MACHOPEUR",	L"MACHOKE",	L"MASCHOCK",	L"",	L"MACHOKE",	L"MACHOKE" },
	{ L"�J�C���L�|",	L"MACHAMP",	L"MACKOGNEUR",	L"MACHAMP",	L"MACHOMEI",	L"",	L"MACHAMP",	L"MACHAMP" },
	{ L"�}�_�c�{�~",	L"BELLSPROUT",	L"CHETIFLOR",	L"BELLSPROUT",	L"KNOFENSA",	L"",	L"BELLSPROUT",	L"BELLSPROUT" },
	{ L"�E�c�h��",	L"WEEPINBELL",	L"BOUSTIFLOR",	L"WEEPINBELL",	L"ULTRIGARIA",	L"",	L"WEEPINBELL",	L"WEEPINBELL" },
	{ L"�E�c�{�b�g",	L"VICTREEBEL",	L"EMPIFLOR",	L"VICTREEBEL",	L"SARZENIA",	L"",	L"VICTREEBEL",	L"VICTREEBEL" },
	{ L"���m�N���Q",	L"TENTACOOL",	L"TENTACOOL",	L"TENTACOOL",	L"TENTACHA",	L"",	L"TENTACOOL",	L"TENTACOOL" },
	{ L"�h�N�N���Q",	L"TENTACRUEL",	L"TENTACRUEL",	L"TENTACRUEL",	L"TENTOXA",	L"",	L"TENTACRUEL",	L"TENTACRUEL" },
	{ L"�C�V�c�u�e",	L"GEODUDE",	L"RACAILLOU",	L"GEODUDE",	L"KLEINSTEIN",	L"",	L"GEODUDE",	L"GEODUDE" },
	{ L"�S���|��",	L"GRAVELER",	L"GRAVALANCH",	L"GRAVELER",	L"GEOROK",	L"",	L"GRAVELER",	L"GRAVELER" },
	{ L"�S���|�j��",	L"GOLEM",	L"GROLEM",	L"GOLEM",	L"GEOWAZ",	L"",	L"GOLEM",	L"GOLEM" },
	{ L"�|�j�|�^",	L"PONYTA",	L"PONYTA",	L"PONYTA",	L"PONITA",	L"",	L"PONYTA",	L"PONYTA" },
	{ L"�M�����b�v",	L"RAPIDASH",	L"GALOPA",	L"RAPIDASH",	L"GALLOPA",	L"",	L"RAPIDASH",	L"RAPIDASH" },
	{ L"���h��",	L"SLOWPOKE",	L"RAMOLOSS",	L"SLOWPOKE",	L"FLEGMON",	L"",	L"SLOWPOKE",	L"SLOWPOKE" },
	{ L"���h����",	L"SLOWBRO",	L"FLAGADOSS",	L"SLOWBRO",	L"LAHMUS",	L"",	L"SLOWBRO",	L"SLOWBRO" },
	{ L"�R�C��",	L"MAGNEMITE",	L"MAGNETI",	L"MAGNEMITE",	L"MAGNETILO",	L"",	L"MAGNEMITE",	L"MAGNEMITE" },
	{ L"���A�R�C��",	L"MAGNETON",	L"MAGNETON",	L"MAGNETON",	L"MAGNETON",	L"",	L"MAGNETON",	L"MAGNETON" },
	{ L"�J���l�M",	L"FARFETCH'D",	L"CANARTICHO",	L"FARFETCH'D",	L"PORENTA",	L"",	L"FARFETCH'D",	L"FARFETCH'D" },
	{ L"�h�|�h�|",	L"DODUO",	L"DODUO",	L"DODUO",	L"DODU",	L"",	L"DODUO",	L"DODUO" },
	{ L"�h�|�h���I",	L"DODRIO",	L"DODRIO",	L"DODRIO",	L"DODRI",	L"",	L"DODRIO",	L"DODRIO" },
	{ L"�p�E���E",	L"SEEL",	L"OTARIA",	L"SEEL",	L"JUROB",	L"",	L"SEEL",	L"SEEL" },
	{ L"�W���S��",	L"DEWGONG",	L"LAMANTINE",	L"DEWGONG",	L"JUGONG",	L"",	L"DEWGONG",	L"DEWGONG" },
	{ L"�x�g�x�^�|",	L"GRIMER",	L"TADMORV",	L"GRIMER",	L"SLEIMA",	L"",	L"GRIMER",	L"GRIMER" },
	{ L"�x�g�x�g��",	L"MUK",	L"GROTADMORV",	L"MUK",	L"SLEIMOK",	L"",	L"MUK",	L"MUK" },
	{ L"�V�F���_�|",	L"SHELLDER",	L"KOKIYAS",	L"SHELLDER",	L"MUSCHAS",	L"",	L"SHELLDER",	L"SHELLDER" },
	{ L"�p���V�F��",	L"CLOYSTER",	L"CRUSTABRI",	L"CLOYSTER",	L"AUSTOS",	L"",	L"CLOYSTER",	L"CLOYSTER" },
	{ L"�S�|�X",	L"GASTLY",	L"FANTOMINUS",	L"GASTLY",	L"NEBULAK",	L"",	L"GASTLY",	L"GASTLY" },
	{ L"�S�|�X�g",	L"HAUNTER",	L"SPECTRUM",	L"HAUNTER",	L"ALPOLLO",	L"",	L"HAUNTER",	L"HAUNTER" },
	{ L"�Q���K�|",	L"GENGAR",	L"ECTOPLASMA",	L"GENGAR",	L"GENGAR",	L"",	L"GENGAR",	L"GENGAR" },
	{ L"�C���|�N",	L"ONIX",	L"ONIX",	L"ONIX",	L"ONIX",	L"",	L"ONIX",	L"ONIX" },
	{ L"�X���|�v",	L"DROWZEE",	L"SOPORIFIK",	L"DROWZEE",	L"TRAUMATO",	L"",	L"DROWZEE",	L"DROWZEE" },
	{ L"�X���|�p�|",	L"HYPNO",	L"HYPNOMADE",	L"HYPNO",	L"HYPNO",	L"",	L"HYPNO",	L"HYPNO" },
	{ L"�N���u",	L"KRABBY",	L"KRABBY",	L"KRABBY",	L"KRABBY",	L"",	L"KRABBY",	L"KRABBY" },
	{ L"�L���O���|",	L"KINGLER",	L"KRABBOSS",	L"KINGLER",	L"KINGLER",	L"",	L"KINGLER",	L"KINGLER" },
	{ L"�r�����_�}",	L"VOLTORB",	L"VOLTORBE",	L"VOLTORB",	L"VOLTOBAL",	L"",	L"VOLTORB",	L"VOLTORB" },
	{ L"�}���}�C��",	L"ELECTRODE",	L"ELECTRODE",	L"ELECTRODE",	L"LEKTROBAL",	L"",	L"ELECTRODE",	L"ELECTRODE" },
	{ L"�^�}�^�}",	L"EXEGGCUTE",	L"NOEUNOEUF",	L"EXEGGCUTE",	L"OWEI",	L"",	L"EXEGGCUTE",	L"EXEGGCUTE" },
	{ L"�i�b�V�|",	L"EXEGGUTOR",	L"NOADKOKO",	L"EXEGGUTOR",	L"KOKOWEI",	L"",	L"EXEGGUTOR",	L"EXEGGUTOR" },
	{ L"�J���J��",	L"CUBONE",	L"OSSELAIT",	L"CUBONE",	L"TRAGOSSO",	L"",	L"CUBONE",	L"CUBONE" },
	{ L"�K���K��",	L"MAROWAK",	L"OSSATUEUR",	L"MAROWAK",	L"KNOGGA",	L"",	L"MAROWAK",	L"MAROWAK" },
	{ L"�T�������|",	L"HITMONLEE",	L"KICKLEE",	L"HITMONLEE",	L"KICKLEE",	L"",	L"HITMONLEE",	L"HITMONLEE" },
	{ L"�G�r�����|",	L"HITMONCHAN",	L"TYGNON",	L"HITMONCHAN",	L"NOCKCHAN",	L"",	L"HITMONCHAN",	L"HITMONCHAN" },
	{ L"�x�������K",	L"LICKITUNG",	L"EXCELANGUE",	L"LICKITUNG",	L"SCHLURP",	L"",	L"LICKITUNG",	L"LICKITUNG" },
	{ L"�h�K�|�X",	L"KOFFING",	L"SMOGO",	L"KOFFING",	L"SMOGON",	L"",	L"KOFFING",	L"KOFFING" },
	{ L"�}�^�h�K�X",	L"WEEZING",	L"SMOGOGO",	L"WEEZING",	L"SMOGMOG",	L"",	L"WEEZING",	L"WEEZING" },
	{ L"�T�C�z�|��",	L"RHYHORN",	L"RHINOCORNE",	L"RHYHORN",	L"RIHORN",	L"",	L"RHYHORN",	L"RHYHORN" },
	{ L"�T�C�h��",	L"RHYDON",	L"RHINOFEROS",	L"RHYDON",	L"RIZEROS",	L"",	L"RHYDON",	L"RHYDON" },
	{ L"���b�L�|",	L"CHANSEY",	L"LEVEINARD",	L"CHANSEY",	L"CHANEIRA",	L"",	L"CHANSEY",	L"CHANSEY" },
	{ L"�����W����",	L"TANGELA",	L"SAQUEDENEU",	L"TANGELA",	L"TANGELA",	L"",	L"TANGELA",	L"TANGELA" },
	{ L"�K���|��",	L"KANGASKHAN",	L"KANGOUREX",	L"KANGASKHAN",	L"KANGAMA",	L"",	L"KANGASKHAN",	L"KANGASKHAN" },
	{ L"�^�b�c�|",	L"HORSEA",	L"HYPOTREMPE",	L"HORSEA",	L"SEEPER",	L"",	L"HORSEA",	L"HORSEA" },
	{ L"�V�|�h��",	L"SEADRA",	L"HYPOCEAN",	L"SEADRA",	L"SEEMON",	L"",	L"SEADRA",	L"SEADRA" },
	{ L"�g�T�L���g",	L"GOLDEEN",	L"POISSIRENE",	L"GOLDEEN",	L"GOLDINI",	L"",	L"GOLDEEN",	L"GOLDEEN" },
	{ L"�A�Y�}�I�E",	L"SEAKING",	L"POISSOROY",	L"SEAKING",	L"GOLKING",	L"",	L"SEAKING",	L"SEAKING" },
	{ L"�q�g�f�}��",	L"STARYU",	L"STARI",	L"STARYU",	L"STERNDU",	L"",	L"STARYU",	L"STARYU" },
	{ L"�X�^�|�~�|",	L"STARMIE",	L"STAROSS",	L"STARMIE",	L"STARMIE",	L"",	L"STARMIE",	L"STARMIE" },
	{ L"�o�����|�h",	L"MR. MIME",	L"M. MIME",	L"MR. MIME",	L"PANTIMOS",	L"",	L"MR. MIME",	L"MR. MIME" },
	{ L"�X�g���C�N",	L"SCYTHER",	L"INSECATEUR",	L"SCYTHER",	L"SICHLOR",	L"",	L"SCYTHER",	L"SCYTHER" },
	{ L"���|�W����",	L"JYNX",	L"LIPPOUTOU",	L"JYNX",	L"ROSSANA",	L"",	L"JYNX",	L"JYNX" },
	{ L"�G���u�|",	L"ELECTABUZZ",	L"ELEKTEK",	L"ELECTABUZZ",	L"ELEKTEK",	L"",	L"ELECTABUZZ",	L"ELECTABUZZ" },
	{ L"�u�|�o�|",	L"MAGMAR",	L"MAGMAR",	L"MAGMAR",	L"MAGMAR",	L"",	L"MAGMAR",	L"MAGMAR" },
	{ L"�J�C���X",	L"PINSIR",	L"SCARABRUTE",	L"PINSIR",	L"PINSIR",	L"",	L"PINSIR",	L"PINSIR" },
	{ L"�P���^���X",	L"TAUROS",	L"TAUROS",	L"TAUROS",	L"TAUROS",	L"",	L"TAUROS",	L"TAUROS" },
	{ L"�R�C�L���O",	L"MAGIKARP",	L"MAGICARPE",	L"MAGIKARP",	L"KARPADOR",	L"",	L"MAGIKARP",	L"MAGIKARP" },
	{ L"�M�����h�X",	L"GYARADOS",	L"LEVIATOR",	L"GYARADOS",	L"GARADOS",	L"",	L"GYARADOS",	L"GYARADOS" },
	{ L"���v���X",	L"LAPRAS",	L"LOKHLASS",	L"LAPRAS",	L"LAPRAS",	L"",	L"LAPRAS",	L"LAPRAS" },
	{ L"���^����",	L"DITTO",	L"METAMORPH",	L"DITTO",	L"DITTO",	L"",	L"DITTO",	L"DITTO" },
	{ L"�C�|�u�C",	L"EEVEE",	L"EVOLI",	L"EEVEE",	L"EVOLI",	L"",	L"EEVEE",	L"EEVEE" },
	{ L"�V�����|�Y",	L"VAPOREON",	L"AQUALI",	L"VAPOREON",	L"AQUANA",	L"",	L"VAPOREON",	L"VAPOREON" },
	{ L"�T���_�|�X",	L"JOLTEON",	L"VOLTALI",	L"JOLTEON",	L"BLITZA",	L"",	L"JOLTEON",	L"JOLTEON" },
	{ L"�u�|�X�^�|",	L"FLAREON",	L"PYROLI",	L"FLAREON",	L"FLAMARA",	L"",	L"FLAREON",	L"FLAREON" },
	{ L"�|���S��",	L"PORYGON",	L"PORYGON",	L"PORYGON",	L"PORYGON",	L"",	L"PORYGON",	L"PORYGON" },
	{ L"�I���i�C�g",	L"OMANYTE",	L"AMONITA",	L"OMANYTE",	L"AMONITAS",	L"",	L"OMANYTE",	L"OMANYTE" },
	{ L"�I���X�^�|",	L"OMASTAR",	L"AMONISTAR",	L"OMASTAR",	L"AMOROSO",	L"",	L"OMASTAR",	L"OMASTAR" },
	{ L"�J�u�g",	L"KABUTO",	L"KABUTO",	L"KABUTO",	L"KABUTO",	L"",	L"KABUTO",	L"KABUTO" },
	{ L"�J�u�g�v�X",	L"KABUTOPS",	L"KABUTOPS",	L"KABUTOPS",	L"KABUTOPS",	L"",	L"KABUTOPS",	L"KABUTOPS" },
	{ L"�v�e��",	L"AERODACTYL",	L"PTERA",	L"AERODACTYL",	L"AERODACTYL",	L"",	L"AERODACTYL",	L"AERODACTYL" },
	{ L"�J�r�S��",	L"SNORLAX",	L"RONFLEX",	L"SNORLAX",	L"RELAXO",	L"",	L"SNORLAX",	L"SNORLAX" },
	{ L"�t���|�U�|",	L"ARTICUNO",	L"ARTIKODIN",	L"ARTICUNO",	L"ARKTOS",	L"",	L"ARTICUNO",	L"ARTICUNO" },
	{ L"�T���_�|",	L"ZAPDOS",	L"ELECTHOR",	L"ZAPDOS",	L"ZAPDOS",	L"",	L"ZAPDOS",	L"ZAPDOS" },
	{ L"�t�@�C���|",	L"MOLTRES",	L"SULFURA",	L"MOLTRES",	L"LAVADOS",	L"",	L"MOLTRES",	L"MOLTRES" },
	{ L"�~�j�����E",	L"DRATINI",	L"MINIDRACO",	L"DRATINI",	L"DRATINI",	L"",	L"DRATINI",	L"DRATINI" },
	{ L"�n�N�����|",	L"DRAGONAIR",	L"DRACO",	L"DRAGONAIR",	L"DRAGONIR",	L"",	L"DRAGONAIR",	L"DRAGONAIR" },
	{ L"�J�C�����|",	L"DRAGONITE",	L"DRACOLOSSE",	L"DRAGONITE",	L"DRAGORAN",	L"",	L"DRAGONITE",	L"DRAGONITE" },
	{ L"�~���E�c�|",	L"MEWTWO",	L"MEWTWO",	L"MEWTWO",	L"MEWTU",	L"",	L"MEWTWO",	L"MEWTWO" },
	{ L"�~���E",	L"MEW",	L"MEW",	L"MEW",	L"MEW",	L"",	L"MEW",	L"MEW" },
	{ L"�`�R���|�^",	L"CHIKORITA",	L"GERMIGNON",	L"CHIKORITA",	L"ENDIVIE",	L"",	L"CHIKORITA",	L"CHIKORITA" },
	{ L"�x�C���|�t",	L"BAYLEEF",	L"MACRONIUM",	L"BAYLEEF",	L"LORBLATT",	L"",	L"BAYLEEF",	L"BAYLEEF" },
	{ L"���K�j�E��",	L"MEGANIUM",	L"MEGANIUM",	L"MEGANIUM",	L"MEGANIE",	L"",	L"MEGANIUM",	L"MEGANIUM" },
	{ L"�q�m�A���V",	L"CYNDAQUIL",	L"HERICENDRE",	L"CYNDAQUIL",	L"FEURIGEL",	L"",	L"CYNDAQUIL",	L"CYNDAQUIL" },
	{ L"�}�O�}���V",	L"QUILAVA",	L"FEURISSON",	L"QUILAVA",	L"IGELAVAR",	L"",	L"QUILAVA",	L"QUILAVA" },
	{ L"�o�N�t�|��",	L"TYPHLOSION",	L"TYPHLOSION",	L"TYPHLOSION",	L"TORNUPTO",	L"",	L"TYPHLOSION",	L"TYPHLOSION" },
	{ L"���j�m�R",	L"TOTODILE",	L"KAIMINUS",	L"TOTODILE",	L"KARNIMANI",	L"",	L"TOTODILE",	L"TOTODILE" },
	{ L"�A���Q�C�c",	L"CROCONAW",	L"CROCRODIL",	L"CROCONAW",	L"TYRACROC",	L"",	L"CROCONAW",	L"CROCONAW" },
	{ L"�I�|�_�C��",	L"FERALIGATR",	L"ALIGATUEUR",	L"FERALIGATR",	L"IMPERGATOR",	L"",	L"FERALIGATR",	L"FERALIGATR" },
	{ L"�I�^�`",	L"SENTRET",	L"FOUINETTE",	L"SENTRET",	L"WIESOR",	L"",	L"SENTRET",	L"SENTRET" },
	{ L"�I�I�^�`",	L"FURRET",	L"FOUINAR",	L"FURRET",	L"WIESENIOR",	L"",	L"FURRET",	L"FURRET" },
	{ L"�z�|�z�|",	L"HOOTHOOT",	L"HOOTHOOT",	L"HOOTHOOT",	L"HOOTHOOT",	L"",	L"HOOTHOOT",	L"HOOTHOOT" },
	{ L"�����m�Y�N",	L"NOCTOWL",	L"NOARFANG",	L"NOCTOWL",	L"NOCTUH",	L"",	L"NOCTOWL",	L"NOCTOWL" },
	{ L"���f�B�o",	L"LEDYBA",	L"COXY",	L"LEDYBA",	L"LEDYBA",	L"",	L"LEDYBA",	L"LEDYBA" },
	{ L"���f�B�A��",	L"LEDIAN",	L"COXYCLAQUE",	L"LEDIAN",	L"LEDIAN",	L"",	L"LEDIAN",	L"LEDIAN" },
	{ L"�C�g�}��",	L"SPINARAK",	L"MIMIGAL",	L"SPINARAK",	L"WEBARAK",	L"",	L"SPINARAK",	L"SPINARAK" },
	{ L"�A���A�h�X",	L"ARIADOS",	L"MIGALOS",	L"ARIADOS",	L"ARIADOS",	L"",	L"ARIADOS",	L"ARIADOS" },
	{ L"�N���o�b�g",	L"CROBAT",	L"NOSTENFER",	L"CROBAT",	L"IKSBAT",	L"",	L"CROBAT",	L"CROBAT" },
	{ L"�`�����`�|",	L"CHINCHOU",	L"LOUPIO",	L"CHINCHOU",	L"LAMPI",	L"",	L"CHINCHOU",	L"CHINCHOU" },
	{ L"�����^�|��",	L"LANTURN",	L"LANTURN",	L"LANTURN",	L"LANTURN",	L"",	L"LANTURN",	L"LANTURN" },
	{ L"�s�`���|",	L"PICHU",	L"PICHU",	L"PICHU",	L"PICHU",	L"",	L"PICHU",	L"PICHU" },
	{ L"�s�B",	L"CLEFFA",	L"MELO",	L"CLEFFA",	L"PII",	L"",	L"CLEFFA",	L"CLEFFA" },
	{ L"�v�v����",	L"IGGLYBUFF",	L"TOUDOUDOU",	L"IGGLYBUFF",	L"FLUFFELUFF",	L"",	L"IGGLYBUFF",	L"IGGLYBUFF" },
	{ L"�g�Q�s�|",	L"TOGEPI",	L"TOGEPI",	L"TOGEPI",	L"TOGEPI",	L"",	L"TOGEPI",	L"TOGEPI" },
	{ L"�g�Q�`�b�N",	L"TOGETIC",	L"TOGETIC",	L"TOGETIC",	L"TOGETIC",	L"",	L"TOGETIC",	L"TOGETIC" },
	{ L"�l�C�e�B",	L"NATU",	L"NATU",	L"NATU",	L"NATU",	L"",	L"NATU",	L"NATU" },
	{ L"�l�C�e�B�I",	L"XATU",	L"XATU",	L"XATU",	L"XATU",	L"",	L"XATU",	L"XATU" },
	{ L"�����|�v",	L"MAREEP",	L"WATTOUAT",	L"MAREEP",	L"VOLTILAMM",	L"",	L"MAREEP",	L"MAREEP" },
	{ L"���R�R",	L"FLAAFFY",	L"LAINERGIE",	L"FLAAFFY",	L"WAATY",	L"",	L"FLAAFFY",	L"FLAAFFY" },
	{ L"�f�������E",	L"AMPHAROS",	L"PHARAMP",	L"AMPHAROS",	L"AMPHAROS",	L"",	L"AMPHAROS",	L"AMPHAROS" },
	{ L"�L���C�n�i",	L"BELLOSSOM",	L"JOLIFLOR",	L"BELLOSSOM",	L"BLUBELLA",	L"",	L"BELLOSSOM",	L"BELLOSSOM" },
	{ L"�}����",	L"MARILL",	L"MARILL",	L"MARILL",	L"MARILL",	L"",	L"MARILL",	L"MARILL" },
	{ L"�}������",	L"AZUMARILL",	L"AZUMARILL",	L"AZUMARILL",	L"AZUMARILL",	L"",	L"AZUMARILL",	L"AZUMARILL" },
	{ L"�E�\�b�L�|",	L"SUDOWOODO",	L"SIMULARBRE",	L"SUDOWOODO",	L"MOGELBAUM",	L"",	L"SUDOWOODO",	L"SUDOWOODO" },
	{ L"�j�����g�m",	L"POLITOED",	L"TARPAUD",	L"POLITOED",	L"QUAXO",	L"",	L"POLITOED",	L"POLITOED" },
	{ L"�n�l�b�R",	L"HOPPIP",	L"GRANIVOL",	L"HOPPIP",	L"HOPPSPROSS",	L"",	L"HOPPIP",	L"HOPPIP" },
	{ L"�|�|�b�R",	L"SKIPLOOM",	L"FLORAVOL",	L"SKIPLOOM",	L"HUBELUPF",	L"",	L"SKIPLOOM",	L"SKIPLOOM" },
	{ L"���^�b�R",	L"JUMPLUFF",	L"COTOVOL",	L"JUMPLUFF",	L"PAPUNGHA",	L"",	L"JUMPLUFF",	L"JUMPLUFF" },
	{ L"�G�C�p��",	L"AIPOM",	L"CAPUMAIN",	L"AIPOM",	L"GRIFFEL",	L"",	L"AIPOM",	L"AIPOM" },
	{ L"�q�}�i�b�c",	L"SUNKERN",	L"TOURNEGRIN",	L"SUNKERN",	L"SONNKERN",	L"",	L"SUNKERN",	L"SUNKERN" },
	{ L"�L�}����",	L"SUNFLORA",	L"HELIATRONC",	L"SUNFLORA",	L"SONNFLORA",	L"",	L"SUNFLORA",	L"SUNFLORA" },
	{ L"���������}",	L"YANMA",	L"YANMA",	L"YANMA",	L"YANMA",	L"",	L"YANMA",	L"YANMA" },
	{ L"�E�p�|",	L"WOOPER",	L"AXOLOTO",	L"WOOPER",	L"FELINO",	L"",	L"WOOPER",	L"WOOPER" },
	{ L"�k�I�|",	L"QUAGSIRE",	L"MARAISTE",	L"QUAGSIRE",	L"MORLORD",	L"",	L"QUAGSIRE",	L"QUAGSIRE" },
	{ L"�G�|�t�B",	L"ESPEON",	L"MENTALI",	L"ESPEON",	L"PSIANA",	L"",	L"ESPEON",	L"ESPEON" },
	{ L"�u���b�L�|",	L"UMBREON",	L"NOCTALI",	L"UMBREON",	L"NACHTARA",	L"",	L"UMBREON",	L"UMBREON" },
	{ L"���~�J���X",	L"MURKROW",	L"CORNEBRE",	L"MURKROW",	L"KRAMURX",	L"",	L"MURKROW",	L"MURKROW" },
	{ L"���h�L���O",	L"SLOWKING",	L"ROIGADA",	L"SLOWKING",	L"LASCHOKING",	L"",	L"SLOWKING",	L"SLOWKING" },
	{ L"���E�}",	L"MISDREAVUS",	L"FEUFOREVE",	L"MISDREAVUS",	L"TRAUNFUGIL",	L"",	L"MISDREAVUS",	L"MISDREAVUS" },
	{ L"�A���m�|��",	L"UNOWN",	L"ZARBI",	L"UNOWN",	L"ICOGNITO",	L"",	L"UNOWN",	L"UNOWN" },
	{ L"�\�|�i���X",	L"WOBBUFFET",	L"QULBUTOKE",	L"WOBBUFFET",	L"WOINGENAU",	L"",	L"WOBBUFFET",	L"WOBBUFFET" },
	{ L"�L�������L",	L"GIRAFARIG",	L"GIRAFARIG",	L"GIRAFARIG",	L"GIRAFARIG",	L"",	L"GIRAFARIG",	L"GIRAFARIG" },
	{ L"�N�k�M�_�}",	L"PINECO",	L"POMDEPIK",	L"PINECO",	L"TANNZA",	L"",	L"PINECO",	L"PINECO" },
	{ L"�t�H���g�X",	L"FORRETRESS",	L"FORETRESS",	L"FORRETRESS",	L"FORSTELLKA",	L"",	L"FORRETRESS",	L"FORRETRESS" },
	{ L"�m�R�b�`",	L"DUNSPARCE",	L"INSOLOURDO",	L"DUNSPARCE",	L"DUMMISEL",	L"",	L"DUNSPARCE",	L"DUNSPARCE" },
	{ L"�O���C�K�|",	L"GLIGAR",	L"SCORPLANE",	L"GLIGAR",	L"SKORGLA",	L"",	L"GLIGAR",	L"GLIGAR" },
	{ L"�n�K�l�|��",	L"STEELIX",	L"STEELIX",	L"STEELIX",	L"STAHLOS",	L"",	L"STEELIX",	L"STEELIX" },
	{ L"�u���|",	L"SNUBBULL",	L"SNUBBULL",	L"SNUBBULL",	L"SNUBBULL",	L"",	L"SNUBBULL",	L"SNUBBULL" },
	{ L"�O�����u��",	L"GRANBULL",	L"GRANBULL",	L"GRANBULL",	L"GRANBULL",	L"",	L"GRANBULL",	L"GRANBULL" },
	{ L"�n���|�Z��",	L"QWILFISH",	L"QWILFISH",	L"QWILFISH",	L"BALDORFISH",	L"",	L"QWILFISH",	L"QWILFISH" },
	{ L"�n�b�T��",	L"SCIZOR",	L"CIZAYOX",	L"SCIZOR",	L"SCHEROX",	L"",	L"SCIZOR",	L"SCIZOR" },
	{ L"�c�{�c�{",	L"SHUCKLE",	L"CARATROC",	L"SHUCKLE",	L"POTTROTT",	L"",	L"SHUCKLE",	L"SHUCKLE" },
	{ L"�w���N���X",	L"HERACROSS",	L"SCARHINO",	L"HERACROSS",	L"SKARABORN",	L"",	L"HERACROSS",	L"HERACROSS" },
	{ L"�j���|��",	L"SNEASEL",	L"FARFURET",	L"SNEASEL",	L"SNIEBEL",	L"",	L"SNEASEL",	L"SNEASEL" },
	{ L"�q���O�}",	L"TEDDIURSA",	L"TEDDIURSA",	L"TEDDIURSA",	L"TEDDIURSA",	L"",	L"TEDDIURSA",	L"TEDDIURSA" },
	{ L"�����O�}",	L"URSARING",	L"URSARING",	L"URSARING",	L"URSARING",	L"",	L"URSARING",	L"URSARING" },
	{ L"�}�O�}�b�O",	L"SLUGMA",	L"LIMAGMA",	L"SLUGMA",	L"SCHNECKMAG",	L"",	L"SLUGMA",	L"SLUGMA" },
	{ L"�}�O�J���S",	L"MAGCARGO",	L"VOLCAROPOD",	L"MAGCARGO",	L"MAGCARGO",	L"",	L"MAGCARGO",	L"MAGCARGO" },
	{ L"�E�����|",	L"SWINUB",	L"MARCACRIN",	L"SWINUB",	L"QUIEKEL",	L"",	L"SWINUB",	L"SWINUB" },
	{ L"�C�m���|",	L"PILOSWINE",	L"COCHIGNON",	L"PILOSWINE",	L"KEIFEL",	L"",	L"PILOSWINE",	L"PILOSWINE" },
	{ L"�T�j�|�S",	L"CORSOLA",	L"CORAYON",	L"CORSOLA",	L"CORASONN",	L"",	L"CORSOLA",	L"CORSOLA" },
	{ L"�e�b�|�E�I",	L"REMORAID",	L"REMORAID",	L"REMORAID",	L"REMORAID",	L"",	L"REMORAID",	L"REMORAID" },
	{ L"�I�N�^��",	L"OCTILLERY",	L"OCTILLERY",	L"OCTILLERY",	L"OCTILLERY",	L"",	L"OCTILLERY",	L"OCTILLERY" },
	{ L"�f���o�|�h",	L"DELIBIRD",	L"CADOIZO",	L"DELIBIRD",	L"BOTOGEL",	L"",	L"DELIBIRD",	L"DELIBIRD" },
	{ L"�}���^�C��",	L"MANTINE",	L"DEMANTA",	L"MANTINE",	L"MANTAX",	L"",	L"MANTINE",	L"MANTINE" },
	{ L"�G�A�|���h",	L"SKARMORY",	L"AIRMURE",	L"SKARMORY",	L"PANZAERON",	L"",	L"SKARMORY",	L"SKARMORY" },
	{ L"�f���r��",	L"HOUNDOUR",	L"MALOSSE",	L"HOUNDOUR",	L"HUNDUSTER",	L"",	L"HOUNDOUR",	L"HOUNDOUR" },
	{ L"�w���K�|",	L"HOUNDOOM",	L"DEMOLOSSE",	L"HOUNDOOM",	L"HUNDEMON",	L"",	L"HOUNDOOM",	L"HOUNDOOM" },
	{ L"�L���O�h��",	L"KINGDRA",	L"HYPOROI",	L"KINGDRA",	L"SEEDRAKING",	L"",	L"KINGDRA",	L"KINGDRA" },
	{ L"�S�}�]�E",	L"PHANPY",	L"PHANPY",	L"PHANPY",	L"PHANPY",	L"",	L"PHANPY",	L"PHANPY" },
	{ L"�h���t�@��",	L"DONPHAN",	L"DONPHAN",	L"DONPHAN",	L"DONPHAN",	L"",	L"DONPHAN",	L"DONPHAN" },
	{ L"�|���S���Q",	L"PORYGON2",	L"PORYGON2",	L"PORYGON2",	L"PORYGON2",	L"",	L"PORYGON2",	L"PORYGON2" },
	{ L"�I�h�V�V",	L"STANTLER",	L"CERFROUSSE",	L"STANTLER",	L"DAMHIRPLEX",	L"",	L"STANTLER",	L"STANTLER" },
	{ L"�h�|�u��",	L"SMEARGLE",	L"QUEULORIOR",	L"SMEARGLE",	L"FARBEAGLE",	L"",	L"SMEARGLE",	L"SMEARGLE" },
	{ L"�o���L�|",	L"TYROGUE",	L"DEBUGANT",	L"TYROGUE",	L"RABAUZ",	L"",	L"TYROGUE",	L"TYROGUE" },
	{ L"�J�|�G���|",	L"HITMONTOP",	L"KAPOERA",	L"HITMONTOP",	L"KAPOERA",	L"",	L"HITMONTOP",	L"HITMONTOP" },
	{ L"���`���|��",	L"SMOOCHUM",	L"LIPPOUTI",	L"SMOOCHUM",	L"KUSSILLA",	L"",	L"SMOOCHUM",	L"SMOOCHUM" },
	{ L"�G���L�b�h",	L"ELEKID",	L"ELEKID",	L"ELEKID",	L"ELEKID",	L"",	L"ELEKID",	L"ELEKID" },
	{ L"�u�r�B",	L"MAGBY",	L"MAGBY",	L"MAGBY",	L"MAGBY",	L"",	L"MAGBY",	L"MAGBY" },
	{ L"�~���^���N",	L"MILTANK",	L"ECREMEUH",	L"MILTANK",	L"MILTANK",	L"",	L"MILTANK",	L"MILTANK" },
	{ L"�n�s�i�X",	L"BLISSEY",	L"LEUPHORIE",	L"BLISSEY",	L"HEITEIRA",	L"",	L"BLISSEY",	L"BLISSEY" },
	{ L"���C�R�E",	L"RAIKOU",	L"RAIKOU",	L"RAIKOU",	L"RAIKOU",	L"",	L"RAIKOU",	L"RAIKOU" },
	{ L"�G���e�C",	L"ENTEI",	L"ENTEI",	L"ENTEI",	L"ENTEI",	L"",	L"ENTEI",	L"ENTEI" },
	{ L"�X�C�N��",	L"SUICUNE",	L"SUICUNE",	L"SUICUNE",	L"SUICUNE",	L"",	L"SUICUNE",	L"SUICUNE" },
	{ L"���|�M���X",	L"LARVITAR",	L"EMBRYLEX",	L"LARVITAR",	L"LARVITAR",	L"",	L"LARVITAR",	L"LARVITAR" },
	{ L"�T�i�M���X",	L"PUPITAR",	L"YMPHECT",	L"PUPITAR",	L"PUPITAR",	L"",	L"PUPITAR",	L"PUPITAR" },
	{ L"�o���M���X",	L"TYRANITAR",	L"TYRANOCIF",	L"TYRANITAR",	L"DESPOTAR",	L"",	L"TYRANITAR",	L"TYRANITAR" },
	{ L"���M�A",	L"LUGIA",	L"LUGIA",	L"LUGIA",	L"LUGIA",	L"",	L"LUGIA",	L"LUGIA" },
	{ L"�z�E�I�E",	L"HO-OH",	L"HO-OH",	L"HO-OH",	L"HO-OH",	L"",	L"HO-OH",	L"HO-OH" },
	{ L"�Z���r�B",	L"CELEBI",	L"CELEBI",	L"CELEBI",	L"CELEBI",	L"",	L"CELEBI",	L"CELEBI" },
	{ L"�L����",	L"TREECKO",	L"ARCKO",	L"TREECKO",	L"GECKARBOR",	L"",	L"TREECKO",	L"TREECKO" },
	{ L"�W���v�g��",	L"GROVYLE",	L"MASSKO",	L"GROVYLE",	L"REPTAIN",	L"",	L"GROVYLE",	L"GROVYLE" },
	{ L"�W���J�C��",	L"SCEPTILE",	L"JUNGKO",	L"SCEPTILE",	L"GEWALDRO",	L"",	L"SCEPTILE",	L"SCEPTILE" },
	{ L"�A�`����",	L"TORCHIC",	L"POUSSIFEU",	L"TORCHIC",	L"FLEMMLI",	L"",	L"TORCHIC",	L"TORCHIC" },
	{ L"���J�V����",	L"COMBUSKEN",	L"GALIFEU",	L"COMBUSKEN",	L"JUNGGLUT",	L"",	L"COMBUSKEN",	L"COMBUSKEN" },
	{ L"�o�V���|��",	L"BLAZIKEN",	L"BRASEGALI",	L"BLAZIKEN",	L"LOHGOCK",	L"",	L"BLAZIKEN",	L"BLAZIKEN" },
	{ L"�~�Y�S���E",	L"MUDKIP",	L"GOBOU",	L"MUDKIP",	L"HYDROPI",	L"",	L"MUDKIP",	L"MUDKIP" },
	{ L"�k�}�N���|",	L"MARSHTOMP",	L"FLOBIO",	L"MARSHTOMP",	L"MOORABBEL",	L"",	L"MARSHTOMP",	L"MARSHTOMP" },
	{ L"���O���|�W",	L"SWAMPERT",	L"LAGGRON",	L"SWAMPERT",	L"SUMPEX",	L"",	L"SWAMPERT",	L"SWAMPERT" },
	{ L"�|�`�G�i",	L"POOCHYENA",	L"MEDHYENA",	L"POOCHYENA",	L"FIFFYEN",	L"",	L"POOCHYENA",	L"POOCHYENA" },
	{ L"�O���G�i",	L"MIGHTYENA",	L"GRAHYENA",	L"MIGHTYENA",	L"MAGNAYEN",	L"",	L"MIGHTYENA",	L"MIGHTYENA" },
	{ L"�W�O�U�O�}",	L"ZIGZAGOON",	L"ZIGZATON",	L"ZIGZAGOON",	L"ZIGZACHS",	L"",	L"ZIGZAGOON",	L"ZIGZAGOON" },
	{ L"�}�b�X�O�}",	L"LINOONE",	L"LINEON",	L"LINOONE",	L"GERADAKS",	L"",	L"LINOONE",	L"LINOONE" },
	{ L"�P���b�\",	L"WURMPLE",	L"CHENIPOTTE",	L"WURMPLE",	L"WAUMPEL",	L"",	L"WURMPLE",	L"WURMPLE" },
	{ L"�J���T���X",	L"SILCOON",	L"ARMULYS",	L"SILCOON",	L"SCHALOKO",	L"",	L"SILCOON",	L"SILCOON" },
	{ L"�A�Q�n���g",	L"BEAUTIFLY",	L"CHARMILLON",	L"BEAUTIFLY",	L"PAPINELLA",	L"",	L"BEAUTIFLY",	L"BEAUTIFLY" },
	{ L"�}�����h",	L"CASCOON",	L"BLINDALYS",	L"CASCOON",	L"PANEKON",	L"",	L"CASCOON",	L"CASCOON" },
	{ L"�h�N�P�C��",	L"DUSTOX",	L"PAPINOX",	L"DUSTOX",	L"PUDOX",	L"",	L"DUSTOX",	L"DUSTOX" },
	{ L"�n�X�{�|",	L"LOTAD",	L"NENUPIOT",	L"LOTAD",	L"LOTURZEL",	L"",	L"LOTAD",	L"LOTAD" },
	{ L"�n�X�u����",	L"LOMBRE",	L"LOMBRE",	L"LOMBRE",	L"LOMBRERO",	L"",	L"LOMBRE",	L"LOMBRE" },
	{ L"�����p�b�p",	L"LUDICOLO",	L"LUDICOLO",	L"LUDICOLO",	L"KAPPALORES",	L"",	L"LUDICOLO",	L"LUDICOLO" },
	{ L"�^�l�{�|",	L"SEEDOT",	L"GRAINIPIOT",	L"SEEDOT",	L"SAMURZEL",	L"",	L"SEEDOT",	L"SEEDOT" },
	{ L"�R�m�n�i",	L"NUZLEAF",	L"PIFEUIL",	L"NUZLEAF",	L"BLANAS",	L"",	L"NUZLEAF",	L"NUZLEAF" },
	{ L"�_�|�e���O",	L"SHIFTRY",	L"TENGALICE",	L"SHIFTRY",	L"TENGULIST",	L"",	L"SHIFTRY",	L"SHIFTRY" },
	{ L"�X�o��",	L"TAILLOW",	L"NIRONDELLE",	L"TAILLOW",	L"SCHWALBINI",	L"",	L"TAILLOW",	L"TAILLOW" },
	{ L"�I�I�X�o��",	L"SWELLOW",	L"HELEDELLE",	L"SWELLOW",	L"SCHWALBOSS",	L"",	L"SWELLOW",	L"SWELLOW" },
	{ L"�L������",	L"WINGULL",	L"GOELISE",	L"WINGULL",	L"WINGULL",	L"",	L"WINGULL",	L"WINGULL" },
	{ L"�y���b�p�|",	L"PELIPPER",	L"BEKIPAN",	L"PELIPPER",	L"PELIPPER",	L"",	L"PELIPPER",	L"PELIPPER" },
	{ L"�����g�X",	L"RALTS",	L"TARSAL",	L"RALTS",	L"TRASLA",	L"",	L"RALTS",	L"RALTS" },
	{ L"�L�����A",	L"KIRLIA",	L"KIRLIA",	L"KIRLIA",	L"KIRLIA",	L"",	L"KIRLIA",	L"KIRLIA" },
	{ L"�T�|�i�C�g",	L"GARDEVOIR",	L"GARDEVOIR",	L"GARDEVOIR",	L"GUARDEVOIR",	L"",	L"GARDEVOIR",	L"GARDEVOIR" },
	{ L"�A���^�}",	L"SURSKIT",	L"ARAKDO",	L"SURSKIT",	L"GEHWEIHER",	L"",	L"SURSKIT",	L"SURSKIT" },
	{ L"�A�����|�X",	L"MASQUERAIN",	L"MASKADRA",	L"MASQUERAIN",	L"MASKEREGEN",	L"",	L"MASQUERAIN",	L"MASQUERAIN" },
	{ L"�L�m�R�R",	L"SHROOMISH",	L"BALIGNON",	L"SHROOMISH",	L"KNILZ",	L"",	L"SHROOMISH",	L"SHROOMISH" },
	{ L"�L�m�K�b�T",	L"BRELOOM",	L"CHAPIGNON",	L"BRELOOM",	L"KAPILZ",	L"",	L"BRELOOM",	L"BRELOOM" },
	{ L"�i�}�P��",	L"SLAKOTH",	L"PARECOOL",	L"SLAKOTH",	L"BUMMELZ",	L"",	L"SLAKOTH",	L"SLAKOTH" },
	{ L"�����L���m",	L"VIGOROTH",	L"VIGOROTH",	L"VIGOROTH",	L"MUNTIER",	L"",	L"VIGOROTH",	L"VIGOROTH" },
	{ L"�P�b�L���O",	L"SLAKING",	L"MONAFLEMIT",	L"SLAKING",	L"LETARKING",	L"",	L"SLAKING",	L"SLAKING" },
	{ L"�c�`�j��",	L"NINCADA",	L"NINGALE",	L"NINCADA",	L"NINCADA",	L"",	L"NINCADA",	L"NINCADA" },
	{ L"�e�b�J�j��",	L"NINJASK",	L"NINJASK",	L"NINJASK",	L"NINJASK",	L"",	L"NINJASK",	L"NINJASK" },
	{ L"�k�P�j��",	L"SHEDINJA",	L"MUNJA",	L"SHEDINJA",	L"NINJATOM",	L"",	L"SHEDINJA",	L"SHEDINJA" },
	{ L"�S�j���j��",	L"WHISMUR",	L"CHUCHMUR",	L"WHISMUR",	L"FLURMEL",	L"",	L"WHISMUR",	L"WHISMUR" },
	{ L"�h�S�|��",	L"LOUDRED",	L"RAMBOUM",	L"LOUDRED",	L"KRAKEELO",	L"",	L"LOUDRED",	L"LOUDRED" },
	{ L"�o�N�I���O",	L"EXPLOUD",	L"BROUHABAM",	L"EXPLOUD",	L"KRAWUMMS",	L"",	L"EXPLOUD",	L"EXPLOUD" },
	{ L"�}�N�m�V�^",	L"MAKUHITA",	L"MAKUHITA",	L"MAKUHITA",	L"MAKUHITA",	L"",	L"MAKUHITA",	L"MAKUHITA" },
	{ L"�n���e���}",	L"HARIYAMA",	L"HARIYAMA",	L"HARIYAMA",	L"HARIYAMA",	L"",	L"HARIYAMA",	L"HARIYAMA" },
	{ L"������",	L"AZURILL",	L"AZURILL",	L"AZURILL",	L"AZURILL",	L"",	L"AZURILL",	L"AZURILL" },
	{ L"�m�Y�p�X",	L"NOSEPASS",	L"TARINOR",	L"NOSEPASS",	L"NASGNET",	L"",	L"NOSEPASS",	L"NOSEPASS" },
	{ L"�G�l�R",	L"SKITTY",	L"SKITTY",	L"SKITTY",	L"ENECO",	L"",	L"SKITTY",	L"SKITTY" },
	{ L"�G�l�R����",	L"DELCATTY",	L"DELCATTY",	L"DELCATTY",	L"ENEKORO",	L"",	L"DELCATTY",	L"DELCATTY" },
	{ L"���~���~",	L"SABLEYE",	L"TENEFIX",	L"SABLEYE",	L"ZOBIRIS",	L"",	L"SABLEYE",	L"SABLEYE" },
	{ L"�N�`�|�g",	L"MAWILE",	L"MYSDIBULE",	L"MAWILE",	L"FLUNKIFER",	L"",	L"MAWILE",	L"MAWILE" },
	{ L"�R�R�h��",	L"ARON",	L"GALEKID",	L"ARON",	L"STOLLUNIOR",	L"",	L"ARON",	L"ARON" },
	{ L"�R�h��",	L"LAIRON",	L"GALEGON",	L"LAIRON",	L"STOLLRAK",	L"",	L"LAIRON",	L"LAIRON" },
	{ L"�{�X�S�h��",	L"AGGRON",	L"GALEKING",	L"AGGRON",	L"STOLLOSS",	L"",	L"AGGRON",	L"AGGRON" },
	{ L"�A�T�i��",	L"MEDITITE",	L"MEDITIKKA",	L"MEDITITE",	L"MEDITIE",	L"",	L"MEDITITE",	L"MEDITITE" },
	{ L"�`���|����",	L"MEDICHAM",	L"CHARMINA",	L"MEDICHAM",	L"MEDITALIS",	L"",	L"MEDICHAM",	L"MEDICHAM" },
	{ L"���N���C",	L"ELECTRIKE",	L"DYNAVOLT",	L"ELECTRIKE",	L"FRIZELBLIZ",	L"",	L"ELECTRIKE",	L"ELECTRIKE" },
	{ L"���C�{���g",	L"MANECTRIC",	L"ELECSPRINT",	L"MANECTRIC",	L"VOLTENSO",	L"",	L"MANECTRIC",	L"MANECTRIC" },
	{ L"�v���X��",	L"PLUSLE",	L"POSIPI",	L"PLUSLE",	L"PLUSLE",	L"",	L"PLUSLE",	L"PLUSLE" },
	{ L"�}�C�i��",	L"MINUN",	L"NEGAPI",	L"MINUN",	L"MINUN",	L"",	L"MINUN",	L"MINUN" },
	{ L"�o���r�|�g",	L"VOLBEAT",	L"MUCIOLE",	L"VOLBEAT",	L"VOLBEAT",	L"",	L"VOLBEAT",	L"VOLBEAT" },
	{ L"�C���~�|�[",	L"ILLUMISE",	L"LUMIVOLE",	L"ILLUMISE",	L"ILLUMISE",	L"",	L"ILLUMISE",	L"ILLUMISE" },
	{ L"���[���A",	L"ROSELIA",	L"ROSELIA",	L"ROSELIA",	L"ROSELIA",	L"",	L"ROSELIA",	L"ROSELIA" },
	{ L"�S�N����",	L"GULPIN",	L"GLOUPTI",	L"GULPIN",	L"SCHLUPPUCK",	L"",	L"GULPIN",	L"GULPIN" },
	{ L"�}���m�|��",	L"SWALOT",	L"AVALTOUT",	L"SWALOT",	L"SCHLUKWECH",	L"",	L"SWALOT",	L"SWALOT" },
	{ L"�L�o�j�A",	L"CARVANHA",	L"CARVANHA",	L"CARVANHA",	L"KANIVANHA",	L"",	L"CARVANHA",	L"CARVANHA" },
	{ L"�T���n�_�|",	L"SHARPEDO",	L"SHARPEDO",	L"SHARPEDO",	L"TOHAIDO",	L"",	L"SHARPEDO",	L"SHARPEDO" },
	{ L"�z�G���R",	L"WAILMER",	L"WAILMER",	L"WAILMER",	L"WAILMER",	L"",	L"WAILMER",	L"WAILMER" },
	{ L"�z�G���I�|",	L"WAILORD",	L"WAILORD",	L"WAILORD",	L"WAILORD",	L"",	L"WAILORD",	L"WAILORD" },
	{ L"�h������",	L"NUMEL",	L"CHAMALLOT",	L"NUMEL",	L"CAMAUB",	L"",	L"NUMEL",	L"NUMEL" },
	{ L"�o�N�|�_",	L"CAMERUPT",	L"CAMERUPT",	L"CAMERUPT",	L"CAMERUPT",	L"",	L"CAMERUPT",	L"CAMERUPT" },
	{ L"�R�|�^�X",	L"TORKOAL",	L"CHARTOR",	L"TORKOAL",	L"QURTEL",	L"",	L"TORKOAL",	L"TORKOAL" },
	{ L"�o�l�u�|",	L"SPOINK",	L"SPOINK",	L"SPOINK",	L"SPOINK",	L"",	L"SPOINK",	L"SPOINK" },
	{ L"�u�|�s�b�O",	L"GRUMPIG",	L"GRORET",	L"GRUMPIG",	L"GROINK",	L"",	L"GRUMPIG",	L"GRUMPIG" },
	{ L"�p�b�`�|��",	L"SPINDA",	L"SPINDA",	L"SPINDA",	L"PANDIR",	L"",	L"SPINDA",	L"SPINDA" },
	{ L"�i�b�N���|",	L"TRAPINCH",	L"KRAKNOIX",	L"TRAPINCH",	L"KNACKLION",	L"",	L"TRAPINCH",	L"TRAPINCH" },
	{ L"�r�u���|�o",	L"VIBRAVA",	L"VIBRANINF",	L"VIBRAVA",	L"VIBRAVA",	L"",	L"VIBRAVA",	L"VIBRAVA" },
	{ L"�t���C�S��",	L"FLYGON",	L"LIBEGON",	L"FLYGON",	L"LIBELLDRA",	L"",	L"FLYGON",	L"FLYGON" },
	{ L"�T�{�l�A",	L"CACNEA",	L"CACNEA",	L"CACNEA",	L"TUSKA",	L"",	L"CACNEA",	L"CACNEA" },
	{ L"�m�N�^�X",	L"CACTURNE",	L"CACTURNE",	L"CACTURNE",	L"NOKTUSKA",	L"",	L"CACTURNE",	L"CACTURNE" },
	{ L"�`���b�g",	L"SWABLU",	L"TYLTON",	L"SWABLU",	L"WABLU",	L"",	L"SWABLU",	L"SWABLU" },
	{ L"�`���^���X",	L"ALTARIA",	L"ALTARIA",	L"ALTARIA",	L"ALTARIA",	L"",	L"ALTARIA",	L"ALTARIA" },
	{ L"�U���O�|�X",	L"ZANGOOSE",	L"MANGRIFF",	L"ZANGOOSE",	L"SENGO",	L"",	L"ZANGOOSE",	L"ZANGOOSE" },
	{ L"�n�u�l�|�N",	L"SEVIPER",	L"SEVIPER",	L"SEVIPER",	L"VIPITIS",	L"",	L"SEVIPER",	L"SEVIPER" },
	{ L"���i�g�|��",	L"LUNATONE",	L"SELEROC",	L"LUNATONE",	L"LUNASTEIN",	L"",	L"LUNATONE",	L"LUNATONE" },
	{ L"�\�����b�N",	L"SOLROCK",	L"SOLAROC",	L"SOLROCK",	L"SONNFEL",	L"",	L"SOLROCK",	L"SOLROCK" },
	{ L"�h�W���b�`",	L"BARBOACH",	L"BARLOCHE",	L"BARBOACH",	L"SCHMERBE",	L"",	L"BARBOACH",	L"BARBOACH" },
	{ L"�i�}�Y��",	L"WHISCASH",	L"BARBICHA",	L"WHISCASH",	L"WELSAR",	L"",	L"WHISCASH",	L"WHISCASH" },
	{ L"�w�C�K�j",	L"CORPHISH",	L"ECRAPINCE",	L"CORPHISH",	L"KREBSCORPS",	L"",	L"CORPHISH",	L"CORPHISH" },
	{ L"�V�U���K�|",	L"CRAWDAUNT",	L"COLHOMARD",	L"CRAWDAUNT",	L"KREBUTACK",	L"",	L"CRAWDAUNT",	L"CRAWDAUNT" },
	{ L"���W����",	L"BALTOY",	L"BALBUTO",	L"BALTOY",	L"PUPPANCE",	L"",	L"BALTOY",	L"BALTOY" },
	{ L"�l���h�|��",	L"CLAYDOL",	L"KAORINE",	L"CLAYDOL",	L"LEPUMENTAS",	L"",	L"CLAYDOL",	L"CLAYDOL" },
	{ L"�����|��",	L"LILEEP",	L"LILIA",	L"LILEEP",	L"LILIEP",	L"",	L"LILEEP",	L"LILEEP" },
	{ L"�����C�h��",	L"CRADILY",	L"VACILYS",	L"CRADILY",	L"WIELIE",	L"",	L"CRADILY",	L"CRADILY" },
	{ L"�A�m�v�X",	L"ANORITH",	L"ANORITH",	L"ANORITH",	L"ANORITH",	L"",	L"ANORITH",	L"ANORITH" },
	{ L"�A�|�}���h",	L"ARMALDO",	L"ARMALDO",	L"ARMALDO",	L"ARMALDO",	L"",	L"ARMALDO",	L"ARMALDO" },
	{ L"�q���o�X",	L"FEEBAS",	L"BARPAU",	L"FEEBAS",	L"BARSCHWA",	L"",	L"FEEBAS",	L"FEEBAS" },
	{ L"�~���J���X",	L"MILOTIC",	L"MILOBELLUS",	L"MILOTIC",	L"MILOTIC",	L"",	L"MILOTIC",	L"MILOTIC" },
	{ L"�|������",	L"CASTFORM",	L"MORPHEO",	L"CASTFORM",	L"FORMEO",	L"",	L"CASTFORM",	L"CASTFORM" },
	{ L"�J�N���I��",	L"KECLEON",	L"KECLEON",	L"KECLEON",	L"KECLEON",	L"",	L"KECLEON",	L"KECLEON" },
	{ L"�J�Q�{�E�Y",	L"SHUPPET",	L"POLICHOMBR",	L"SHUPPET",	L"SHUPPET",	L"",	L"SHUPPET",	L"SHUPPET" },
	{ L"�W���y�b�^",	L"BANETTE",	L"BRANETTE",	L"BANETTE",	L"BANETTE",	L"",	L"BANETTE",	L"BANETTE" },
	{ L"���}����",	L"DUSKULL",	L"SKELENOX",	L"DUSKULL",	L"ZWIRRLICHT",	L"",	L"DUSKULL",	L"DUSKULL" },
	{ L"�T�}���|��",	L"DUSCLOPS",	L"TERACLOPE",	L"DUSCLOPS",	L"ZWIRRKLOP",	L"",	L"DUSCLOPS",	L"DUSCLOPS" },
	{ L"�g���s�E�X",	L"TROPIUS",	L"TROPIUS",	L"TROPIUS",	L"TROPIUS",	L"",	L"TROPIUS",	L"TROPIUS" },
	{ L"�`���|��",	L"CHIMECHO",	L"EOKO",	L"CHIMECHO",	L"PALIMPALIM",	L"",	L"CHIMECHO",	L"CHIMECHO" },
	{ L"�A�u�\��",	L"ABSOL",	L"ABSOL",	L"ABSOL",	L"ABSOL",	L"",	L"ABSOL",	L"ABSOL" },
	{ L"�\�|�i�m",	L"WYNAUT",	L"OKEOKE",	L"WYNAUT",	L"ISSO",	L"",	L"WYNAUT",	L"WYNAUT" },
	{ L"���L�����V",	L"SNORUNT",	L"STALGAMIN",	L"SNORUNT",	L"SCHNEPPKE",	L"",	L"SNORUNT",	L"SNORUNT" },
	{ L"�I�j�S�|��",	L"GLALIE",	L"ONIGLALI",	L"GLALIE",	L"FIRNONTOR",	L"",	L"GLALIE",	L"GLALIE" },
	{ L"�^�}�U���V",	L"SPHEAL",	L"OBALIE",	L"SPHEAL",	L"SEEMOPS",	L"",	L"SPHEAL",	L"SPHEAL" },
	{ L"�g�h�O���|",	L"SEALEO",	L"PHOGLEUR",	L"SEALEO",	L"SEEJONG",	L"",	L"SEALEO",	L"SEALEO" },
	{ L"�g�h�[���K",	L"WALREIN",	L"KAIMORSE",	L"WALREIN",	L"WALRAISA",	L"",	L"WALREIN",	L"WALREIN" },
	{ L"�p�|����",	L"CLAMPERL",	L"COQUIPERL",	L"CLAMPERL",	L"PERLU",	L"",	L"CLAMPERL",	L"CLAMPERL" },
	{ L"�n���e�|��",	L"HUNTAIL",	L"SERPANG",	L"HUNTAIL",	L"AALABYSS",	L"",	L"HUNTAIL",	L"HUNTAIL" },
	{ L"�T�N���r�X",	L"GOREBYSS",	L"ROSABYSS",	L"GOREBYSS",	L"SAGANABYSS",	L"",	L"GOREBYSS",	L"GOREBYSS" },
	{ L"�W�|�����X",	L"RELICANTH",	L"RELICANTH",	L"RELICANTH",	L"RELICANTH",	L"",	L"RELICANTH",	L"RELICANTH" },
	{ L"���u�J�X",	L"LUVDISC",	L"LOVDISC",	L"LUVDISC",	L"LIEBISKUS",	L"",	L"LUVDISC",	L"LUVDISC" },
	{ L"�^�c�x�C",	L"BAGON",	L"DRABY",	L"BAGON",	L"KINDWURM",	L"",	L"BAGON",	L"BAGON" },
	{ L"�R�����|",	L"SHELGON",	L"DRACKHAUS",	L"SHELGON",	L"DRASCHEL",	L"",	L"SHELGON",	L"SHELGON" },
	{ L"�{�|�}���_",	L"SALAMENCE",	L"DRATTAK",	L"SALAMENCE",	L"BRUTALANDA",	L"",	L"SALAMENCE",	L"SALAMENCE" },
	{ L"�_���o��",	L"BELDUM",	L"TERHAL",	L"BELDUM",	L"TANHEL",	L"",	L"BELDUM",	L"BELDUM" },
	{ L"���^���O",	L"METANG",	L"METANG",	L"METANG",	L"METANG",	L"",	L"METANG",	L"METANG" },
	{ L"���^�O���X",	L"METAGROSS",	L"METALOSSE",	L"METAGROSS",	L"METAGROSS",	L"",	L"METAGROSS",	L"METAGROSS" },
	{ L"���W���b�N",	L"REGIROCK",	L"REGIROCK",	L"REGIROCK",	L"REGIROCK",	L"",	L"REGIROCK",	L"REGIROCK" },
	{ L"���W�A�C�X",	L"REGICE",	L"REGICE",	L"REGICE",	L"REGICE",	L"",	L"REGICE",	L"REGICE" },
	{ L"���W�X�`��",	L"REGISTEEL",	L"REGISTEEL",	L"REGISTEEL",	L"REGISTEEL",	L"",	L"REGISTEEL",	L"REGISTEEL" },
	{ L"���e�B�A�X",	L"LATIAS",	L"LATIAS",	L"LATIAS",	L"LATIAS",	L"",	L"LATIAS",	L"LATIAS" },
	{ L"���e�B�I�X",	L"LATIOS",	L"LATIOS",	L"LATIOS",	L"LATIOS",	L"",	L"LATIOS",	L"LATIOS" },
	{ L"�J�C�I�|�K",	L"KYOGRE",	L"KYOGRE",	L"KYOGRE",	L"KYOGRE",	L"",	L"KYOGRE",	L"KYOGRE" },
	{ L"�O���|�h��",	L"GROUDON",	L"GROUDON",	L"GROUDON",	L"GROUDON",	L"",	L"GROUDON",	L"GROUDON" },
	{ L"���b�N�E�U",	L"RAYQUAZA",	L"RAYQUAZA",	L"RAYQUAZA",	L"RAYQUAZA",	L"",	L"RAYQUAZA",	L"RAYQUAZA" },
	{ L"�W���|�`",	L"JIRACHI",	L"JIRACHI",	L"JIRACHI",	L"JIRACHI",	L"",	L"JIRACHI",	L"JIRACHI" },
	{ L"�f�I�L�V�X",	L"DEOXYS",	L"DEOXYS",	L"DEOXYS",	L"DEOXYS",	L"",	L"DEOXYS",	L"DEOXYS" },
	{ L"�i�G�g��",	L"TURTWIG",	L"TORTIPOUSS",	L"TURTWIG",	L"CHELAST",	L"",	L"TURTWIG",	L"TURTWIG" },
	{ L"�n���V�K��",	L"GROTLE",	L"BOSKARA",	L"GROTLE",	L"CHELCARAIN",	L"",	L"GROTLE",	L"GROTLE" },
	{ L"�h�_�C�g�X",	L"TORTERRA",	L"TORTERRA",	L"TORTERRA",	L"CHELTERRAR",	L"",	L"TORTERRA",	L"TORTERRA" },
	{ L"�q�R�U��",	L"CHIMCHAR",	L"OUISTICRAM",	L"CHIMCHAR",	L"PANFLAM",	L"",	L"CHIMCHAR",	L"CHIMCHAR" },
	{ L"���E�J�U��",	L"MONFERNO",	L"CHIMPENFEU",	L"MONFERNO",	L"PANPYRO",	L"",	L"MONFERNO",	L"MONFERNO" },
	{ L"�S�E�J�U��",	L"INFERNAPE",	L"SIMIABRAZ",	L"INFERNAPE",	L"PANFERNO",	L"",	L"INFERNAPE",	L"INFERNAPE" },
	{ L"�|�b�`���}",	L"PIPLUP",	L"TIPLOUF",	L"PIPLUP",	L"PLINFA",	L"",	L"PIPLUP",	L"PIPLUP" },
	{ L"�|�b�^�C�V",	L"PRINPLUP",	L"PRINPLOUF",	L"PRINPLUP",	L"PLIPRIN",	L"",	L"PRINPLUP",	L"PRINPLUP" },
	{ L"�G���y���g",	L"EMPOLEON",	L"PINGOLEON",	L"EMPOLEON",	L"IMPOLEON",	L"",	L"EMPOLEON",	L"EMPOLEON" },
	{ L"���b�N��",	L"STARLY",	L"ETOURMI",	L"STARLY",	L"STARALILI",	L"",	L"STARLY",	L"STARLY" },
	{ L"���N�o�|�h",	L"STARAVIA",	L"ETOURVOL",	L"STARAVIA",	L"STARAVIA",	L"",	L"STARAVIA",	L"STARAVIA" },
	{ L"���N�z�|�N",	L"STARAPTOR",	L"ETOURAPTOR",	L"STARAPTOR",	L"STARAPTOR",	L"",	L"STARAPTOR",	L"STARAPTOR" },
	{ L"�r�b�p",	L"BIDOOF",	L"KEUNOTOR",	L"BIDOOF",	L"BIDIZA",	L"",	L"BIDOOF",	L"BIDOOF" },
	{ L"�r�|�_��",	L"BIBAREL",	L"CASTORNO",	L"BIBAREL",	L"BIDIFAS",	L"",	L"BIBAREL",	L"BIBAREL" },
	{ L"�R���{�|�V",	L"KRICKETOT",	L"CRIKZIK",	L"KRICKETOT",	L"ZIRPURZE",	L"",	L"KRICKETOT",	L"KRICKETOT" },
	{ L"�R���g�b�N",	L"KRICKETUNE",	L"MELOKRIK",	L"KRICKETUNE",	L"ZIRPEISE",	L"",	L"KRICKETUNE",	L"KRICKETUNE" },
	{ L"�R�����N",	L"SHINX",	L"LIXY",	L"SHINX",	L"SHEINUX",	L"",	L"SHINX",	L"SHINX" },
	{ L"���N�V�I",	L"LUXIO",	L"LUXIO",	L"LUXIO",	L"LUXIO",	L"",	L"LUXIO",	L"LUXIO" },
	{ L"�����g���|",	L"LUXRAY",	L"LUXRAY",	L"LUXRAY",	L"LUXTRA",	L"",	L"LUXRAY",	L"LUXRAY" },
	{ L"�X�{�~�|",	L"BUDEW",	L"ROZBOUTON",	L"BUDEW",	L"KNOSPI",	L"",	L"BUDEW",	L"BUDEW" },
	{ L"���Y���C�h",	L"ROSERADE",	L"ROSERADE",	L"ROSERADE",	L"ROSERADE",	L"",	L"ROSERADE",	L"ROSERADE" },
	{ L"�Y�K�C�h�X",	L"CRANIDOS",	L"KRANIDOS",	L"CRANIDOS",	L"KOKNODON",	L"",	L"CRANIDOS",	L"CRANIDOS" },
	{ L"�����p���h",	L"RAMPARDOS",	L"CHARKOS",	L"RAMPARDOS",	L"RAMEIDON",	L"",	L"RAMPARDOS",	L"RAMPARDOS" },
	{ L"�^�e�g�v�X",	L"SHIELDON",	L"DINOCLIER",	L"SHIELDON",	L"SCHILTERUS",	L"",	L"SHIELDON",	L"SHIELDON" },
	{ L"�g���f�v�X",	L"BASTIODON",	L"BASTIODON",	L"BASTIODON",	L"BOLLTERUS",	L"",	L"BASTIODON",	L"BASTIODON" },
	{ L"�~�m���b�`",	L"BURMY",	L"CHENITI",	L"BURMY",	L"BURMY",	L"",	L"BURMY",	L"BURMY" },
	{ L"�~�m�}�_��",	L"WORMADAM",	L"CHENISELLE",	L"WORMADAM",	L"BURMADAME",	L"",	L"WORMADAM",	L"WORMADAM" },
	{ L"�K�|���C��",	L"MOTHIM",	L"PAPILORD",	L"MOTHIM",	L"MOTERPEL",	L"",	L"MOTHIM",	L"MOTHIM" },
	{ L"�~�c�n�j�|",	L"COMBEE",	L"APITRINI",	L"COMBEE",	L"WADRIBIE",	L"",	L"COMBEE",	L"COMBEE" },
	{ L"�r�|�N�C��",	L"VESPIQUEN",	L"APIREINE",	L"VESPIQUEN",	L"HONWEISEL",	L"",	L"VESPIQUEN",	L"VESPIQUEN" },
	{ L"�p�`���X",	L"PACHIRISU",	L"PACHIRISU",	L"PACHIRISU",	L"PACHIRISU",	L"",	L"PACHIRISU",	L"PACHIRISU" },
	{ L"�u�C�[��",	L"BUIZEL",	L"MUSTEBOUEE",	L"BUIZEL",	L"BAMELIN",	L"",	L"BUIZEL",	L"BUIZEL" },
	{ L"�t���|�[��",	L"FLOATZEL",	L"MUSTEFLOTT",	L"FLOATZEL",	L"BOJELIN",	L"",	L"FLOATZEL",	L"FLOATZEL" },
	{ L"�`�F�����{",	L"CHERUBI",	L"CERIBOU",	L"CHERUBI",	L"KIKUGI",	L"",	L"CHERUBI",	L"CHERUBI" },
	{ L"�`�F����",	L"CHERRIM",	L"CERIFLOR",	L"CHERRIM",	L"KINOSO",	L"",	L"CHERRIM",	L"CHERRIM" },
	{ L"�J���i�N�V",	L"SHELLOS",	L"SANCOKI",	L"SHELLOS",	L"SCHALELLOS",	L"",	L"SHELLOS",	L"SHELLOS" },
	{ L"�g���g�h��",	L"GASTRODON",	L"TRITOSOR",	L"GASTRODON",	L"GASTRODON",	L"",	L"GASTRODON",	L"GASTRODON" },
	{ L"�G�e�{�|�X",	L"AMBIPOM",	L"CAPIDEXTRE",	L"AMBIPOM",	L"AMBIDIFFEL",	L"",	L"AMBIPOM",	L"AMBIPOM" },
	{ L"�t�����e",	L"DRIFLOON",	L"BAUDRIVE",	L"DRIFLOON",	L"DRIFTLON",	L"",	L"DRIFLOON",	L"DRIFLOON" },
	{ L"�t�����C�h",	L"DRIFBLIM",	L"GRODRIVE",	L"DRIFBLIM",	L"DRIFZEPELI",	L"",	L"DRIFBLIM",	L"DRIFBLIM" },
	{ L"�~�~����",	L"BUNEARY",	L"LAPOREILLE",	L"BUNEARY",	L"HASPIROR",	L"",	L"BUNEARY",	L"BUNEARY" },
	{ L"�~�~���b�v",	L"LOPUNNY",	L"LOCKPIN",	L"LOPUNNY",	L"SCHLAPOR",	L"",	L"LOPUNNY",	L"LOPUNNY" },
	{ L"���E�}�|�W",	L"MISMAGIUS",	L"MAGIREVE",	L"MISMAGIUS",	L"TRAUNMAGIL",	L"",	L"MISMAGIUS",	L"MISMAGIUS" },
	{ L"�h���J���X",	L"HONCHKROW",	L"CORBOSS",	L"HONCHKROW",	L"KRAMSHEF",	L"",	L"HONCHKROW",	L"HONCHKROW" },
	{ L"�j�����}�|",	L"GLAMEOW",	L"CHAGLAM",	L"GLAMEOW",	L"CHARMIAN",	L"",	L"GLAMEOW",	L"GLAMEOW" },
	{ L"�u�j���b�g",	L"PURUGLY",	L"CHAFFREUX",	L"PURUGLY",	L"SHNURGARST",	L"",	L"PURUGLY",	L"PURUGLY" },
	{ L"���|�V����",	L"CHINGLING",	L"KORILLON",	L"CHINGLING",	L"KLINGPLIM",	L"",	L"CHINGLING",	L"CHINGLING" },
	{ L"�X�J���v�|",	L"STUNKY",	L"MOUFOUETTE",	L"STUNKY",	L"SKUNKAPUH",	L"",	L"STUNKY",	L"STUNKY" },
	{ L"�X�J�^���N",	L"SKUNTANK",	L"MOUFFLAIR",	L"SKUNTANK",	L"SKUNTANK",	L"",	L"SKUNTANK",	L"SKUNTANK" },
	{ L"�h�|�~���|",	L"BRONZOR",	L"ARCHEOMIRE",	L"BRONZOR",	L"BRONZEL",	L"",	L"BRONZOR",	L"BRONZOR" },
	{ L"�h�|�^�N��",	L"BRONZONG",	L"ARCHEODONG",	L"BRONZONG",	L"BRONZONG",	L"",	L"BRONZONG",	L"BRONZONG" },
	{ L"�E�\�n�`",	L"BONSLY",	L"MANZAI",	L"BONSLY",	L"MOBAI",	L"",	L"BONSLY",	L"BONSLY" },
	{ L"�}�l�l",	L"MIME JR.",	L"MIME JR.",	L"MIME JR.",	L"PANTIMIMI",	L"",	L"MIME JR.",	L"MIME JR." },
	{ L"�s���v�N",	L"HAPPINY",	L"PTIRAVI",	L"HAPPINY",	L"WONNEIRA",	L"",	L"HAPPINY",	L"HAPPINY" },
	{ L"�y���b�v",	L"CHATOT",	L"PIJAKO",	L"CHATOT",	L"PLAUDAGEI",	L"",	L"CHATOT",	L"CHATOT" },
	{ L"�~�J���Q",	L"SPIRITOMB",	L"SPIRITOMB",	L"SPIRITOMB",	L"KRYPPUK",	L"",	L"SPIRITOMB",	L"SPIRITOMB" },
	{ L"�t�J�}��",	L"GIBLE",	L"GRIKNOT",	L"GIBLE",	L"KAUMALAT",	L"",	L"GIBLE",	L"GIBLE" },
	{ L"�K�o�C�g",	L"GABITE",	L"CARMACHE",	L"GABITE",	L"KNARKSEL",	L"",	L"GABITE",	L"GABITE" },
	{ L"�K�u���A�X",	L"GARCHOMP",	L"CARCHACROK",	L"GARCHOMP",	L"KNAKRACK",	L"",	L"GARCHOMP",	L"GARCHOMP" },
	{ L"�S���x",	L"MUNCHLAX",	L"GOINFREX",	L"MUNCHLAX",	L"MAMPFAXO",	L"",	L"MUNCHLAX",	L"MUNCHLAX" },
	{ L"���I��",	L"RIOLU",	L"RIOLU",	L"RIOLU",	L"RIOLU",	L"",	L"RIOLU",	L"RIOLU" },
	{ L"���J���I",	L"LUCARIO",	L"LUCARIO",	L"LUCARIO",	L"LUCARIO",	L"",	L"LUCARIO",	L"LUCARIO" },
	{ L"�q�|�|�^�X",	L"HIPPOPOTAS",	L"HIPPOPOTAS",	L"HIPPOPOTAS",	L"HIPPOPOTAS",	L"",	L"HIPPOPOTAS",	L"HIPPOPOTAS" },
	{ L"�J�o���h��",	L"HIPPOWDON",	L"HIPPODOCUS",	L"HIPPOWDON",	L"HIPPOTERUS",	L"",	L"HIPPOWDON",	L"HIPPOWDON" },
	{ L"�X�R���s",	L"SKORUPI",	L"RAPION",	L"SKORUPI",	L"PIONSKORA",	L"",	L"SKORUPI",	L"SKORUPI" },
	{ L"�h���s�I��",	L"DRAPION",	L"DRASCORE",	L"DRAPION",	L"PIONDRAGI",	L"",	L"DRAPION",	L"DRAPION" },
	{ L"�O���b�O��",	L"CROAGUNK",	L"CRADOPAUD",	L"CROAGUNK",	L"GLIBUNKEL",	L"",	L"CROAGUNK",	L"CROAGUNK" },
	{ L"�h�N���b�O",	L"TOXICROAK",	L"COATOX",	L"TOXICROAK",	L"TOXIQUAK",	L"",	L"TOXICROAK",	L"TOXICROAK" },
	{ L"�}�X�L�b�p",	L"CARNIVINE",	L"VORTENTE",	L"CARNIVINE",	L"VENUFLIBIS",	L"",	L"CARNIVINE",	L"CARNIVINE" },
	{ L"�P�C�R�E�I",	L"FINNEON",	L"ECAYON",	L"FINNEON",	L"FINNEON",	L"",	L"FINNEON",	L"FINNEON" },
	{ L"�l�I�����g",	L"LUMINEON",	L"LUMINEON",	L"LUMINEON",	L"LUMINEON",	L"",	L"LUMINEON",	L"LUMINEON" },
	{ L"�^�}���^",	L"MANTYKE",	L"BABIMANTA",	L"MANTYKE",	L"MANTIRPS",	L"",	L"MANTYKE",	L"MANTYKE" },
	{ L"���L�J�u��",	L"SNOVER",	L"BLIZZI",	L"SNOVER",	L"SHNEBEDECK",	L"",	L"SNOVER",	L"SNOVER" },
	{ L"���L�m�I�|",	L"ABOMASNOW",	L"BLIZZAROI",	L"ABOMASNOW",	L"REXBLISAR",	L"",	L"ABOMASNOW",	L"ABOMASNOW" },
	{ L"�}�j���|��",	L"WEAVILE",	L"DIMORET",	L"WEAVILE",	L"SNIBUNNA",	L"",	L"WEAVILE",	L"WEAVILE" },
	{ L"�W�o�R�C��",	L"MAGNEZONE",	L"MAGNEZONE",	L"MAGNEZONE",	L"MAGNEZONE",	L"",	L"MAGNEZONE",	L"MAGNEZONE" },
	{ L"�x���x���g",	L"LICKILICKY",	L"COUDLANGUE",	L"LICKILICKY",	L"SCHLURPLEK",	L"",	L"LICKILICKY",	L"LICKILICKY" },
	{ L"�h�T�C�h��",	L"RHYPERIOR",	L"RHINASTOC",	L"RHYPERIOR",	L"RIHORNIOR",	L"",	L"RHYPERIOR",	L"RHYPERIOR" },
	{ L"���W�����{",	L"TANGROWTH",	L"BOULDENEU",	L"TANGROWTH",	L"TANGOLOSS",	L"",	L"TANGROWTH",	L"TANGROWTH" },
	{ L"�G���L�u��",	L"ELECTIVIRE",	L"ELEKABLE",	L"ELECTIVIRE",	L"ELEVOLTEK",	L"",	L"ELECTIVIRE",	L"ELECTIVIRE" },
	{ L"�u�|�o�|��",	L"MAGMORTAR",	L"MAGANON",	L"MAGMORTAR",	L"MAGBRANT",	L"",	L"MAGMORTAR",	L"MAGMORTAR" },
	{ L"�g�Q�L�b�X",	L"TOGEKISS",	L"TOGEKISS",	L"TOGEKISS",	L"TOGEKISS",	L"",	L"TOGEKISS",	L"TOGEKISS" },
	{ L"���K�����}",	L"YANMEGA",	L"YANMEGA",	L"YANMEGA",	L"YANMEGA",	L"",	L"YANMEGA",	L"YANMEGA" },
	{ L"���|�t�B�A",	L"LEAFEON",	L"PHYLLALI",	L"LEAFEON",	L"FOLIPURBA",	L"",	L"LEAFEON",	L"LEAFEON" },
	{ L"�O���C�V�A",	L"GLACEON",	L"GIVRALI",	L"GLACEON",	L"GLAZIOLA",	L"",	L"GLACEON",	L"GLACEON" },
	{ L"�O���C�I��",	L"GLISCOR",	L"SCORVOL",	L"GLISCOR",	L"SKORGRO",	L"",	L"GLISCOR",	L"GLISCOR" },
	{ L"�}�����|",	L"MAMOSWINE",	L"MAMMOCHON",	L"MAMOSWINE",	L"MAMUTEL",	L"",	L"MAMOSWINE",	L"MAMOSWINE" },
	{ L"�|���S���y",	L"PORYGON-Z",	L"PORYGON-Z",	L"PORYGON-Z",	L"PORYGON-Z",	L"",	L"PORYGON-Z",	L"PORYGON-Z" },
	{ L"�G�����C�h",	L"GALLADE",	L"GALLAME",	L"GALLADE",	L"GALAGLADI",	L"",	L"GALLADE",	L"GALLADE" },
	{ L"�_�C�m�|�Y",	L"PROBOPASS",	L"TARINORME",	L"PROBOPASS",	L"VOLUMINAS",	L"",	L"PROBOPASS",	L"PROBOPASS" },
	{ L"���m���|��",	L"DUSKNOIR",	L"NOCTUNOIR",	L"DUSKNOIR",	L"ZWIRRFINST",	L"",	L"DUSKNOIR",	L"DUSKNOIR" },
	{ L"���L���m�R",	L"FROSLASS",	L"MOMARTIK",	L"FROSLASS",	L"FROSDEDJE",	L"",	L"FROSLASS",	L"FROSLASS" },
	{ L"���g��",	L"ROTOM",	L"MOTISMA",	L"ROTOM",	L"ROTOM",	L"",	L"ROTOM",	L"ROTOM" },
	{ L"���N�V�|",	L"UXIE",	L"CREHELF",	L"UXIE",	L"SELFE",	L"",	L"UXIE",	L"UXIE" },
	{ L"�G�����b�g",	L"MESPRIT",	L"CREFOLLET",	L"MESPRIT",	L"VESPRIT",	L"",	L"MESPRIT",	L"MESPRIT" },
	{ L"�A�O�m��",	L"AZELF",	L"CREFADET",	L"AZELF",	L"TOBUTZ",	L"",	L"AZELF",	L"AZELF" },
	{ L"�f�B�A���K",	L"DIALGA",	L"DIALGA",	L"DIALGA",	L"DIALGA",	L"",	L"DIALGA",	L"DIALGA" },
	{ L"�p���L�A",	L"PALKIA",	L"PALKIA",	L"PALKIA",	L"PALKIA",	L"",	L"PALKIA",	L"PALKIA" },
	{ L"�q�|�h����",	L"HEATRAN",	L"HEATRAN",	L"HEATRAN",	L"HEATRAN",	L"",	L"HEATRAN",	L"HEATRAN" },
	{ L"���W�M�K�X",	L"REGIGIGAS",	L"REGIGIGAS",	L"REGIGIGAS",	L"REGIGIGAS",	L"",	L"REGIGIGAS",	L"REGIGIGAS" },
	{ L"�M���e�B�i",	L"GIRATINA",	L"GIRATINA",	L"GIRATINA",	L"GIRATINA",	L"",	L"GIRATINA",	L"GIRATINA" },
	{ L"�N���Z���A",	L"CRESSELIA",	L"CRESSELIA",	L"CRESSELIA",	L"CRESSELIA",	L"",	L"CRESSELIA",	L"CRESSELIA" },
	{ L"�t�B�I�l",	L"PHIONE",	L"PHIONE",	L"PHIONE",	L"PHIONE",	L"",	L"PHIONE",	L"PHIONE" },
	{ L"�}�i�t�B",	L"MANAPHY",	L"MANAPHY",	L"MANAPHY",	L"MANAPHY",	L"",	L"MANAPHY",	L"MANAPHY" },
	{ L"�_�|�N���C",	L"DARKRAI",	L"DARKRAI",	L"DARKRAI",	L"DARKRAI",	L"",	L"DARKRAI",	L"DARKRAI" },
	{ L"�V�F�C�~",	L"SHAYMIN",	L"SHAYMIN",	L"SHAYMIN",	L"SHAYMIN",	L"",	L"SHAYMIN",	L"SHAYMIN" },
	{ L"�A���Z�E�X",	L"ARCEUS",	L"ARCEUS",	L"ARCEUS",	L"ARCEUS",	L"",	L"ARCEUS",	L"ARCEUS" }
};

#endif

/*-----------------------------------------------------------------------*
					�O���[�o���֐���`
 *-----------------------------------------------------------------------*/

/*!
	�|�P�����f�[�^���Í������܂��B

	@param src	�Í������s���|�P�����f�[�^
	@param dst	�Í������s�����f�[�^�̏o�͐�
*/
void PokeTool_EncodePokemonParam(const POKEMON_PARAM* src, POKEMON_PARAM* dst)
{
	*dst = *src;

	PokeParaCoded(&dst->pcp, sizeof(POKEMON_CALC_PARAM), dst->ppp.personal_rnd);
	dst->ppp.checksum=PokeParaCheckSum(&dst->ppp.paradata, sizeof(POKEMON_PASO_PARAM1)*4);
	PokeParaCoded(&dst->ppp.paradata, sizeof(POKEMON_PASO_PARAM1)*4, dst->ppp.checksum);
}

/*!
	�|�P�����f�[�^�̈Í����������܂��B

	@param src	�Í��������s���|�P�����f�[�^
	@param dst	�Í��������s�����f�[�^�̏o�͐�

	@retval TRUE	�Í������ɐ�������
	@retval FALSE	�Í������Ɏ��s�����B�����̃`�F�b�N�T��������Ȃ������B
*/
BOOL PokeTool_DecodePokemonParam(const POKEMON_PARAM* src, POKEMON_PARAM* dst)
{
	u16	sum;
	*dst = *src;

	PokeParaDecoded(&dst->pcp, sizeof(POKEMON_CALC_PARAM), dst->ppp.personal_rnd);
	PokeParaDecoded(&dst->ppp.paradata, sizeof(POKEMON_PASO_PARAM1)*4, dst->ppp.checksum);
	sum = PokeParaCheckSum(&dst->ppp.paradata, sizeof(POKEMON_PASO_PARAM1)*4);
	if (sum != dst->ppp.checksum) {
		return FALSE;
	}

	return TRUE;
}

/*!
	�|�P�����f�[�^���̃p�����[�^���擾���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	�擾���ʂ��Ԓl�ɂȂ邩�Abuf�ɓ��邩�̓p�����[�^�ɂ���ĈقȂ�܂��̂ŁA
	�\�[�X��ǂ�Ŕ��f���Ă��������B

	@param pp	�p�����[�^���擾����|�P�����f�[�^
	@param id	�擾�������p�����[�^��ID
	@param buf	�擾�����p�����[�^�̏o�͐�

	@return	�擾�����p�����[�^�A�������͎擾���������������s�������B
*/
u32 PokeTool_GetPokeParam(POKEMON_PARAM *pp, PokeToolParamID id, void *buf)
{
	return PokeParaGetAct(pp, id, buf);
}

/*!
	�|�P�����f�[�^���̃p�����[�^��ݒ肵�܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	�f�[�^�̗^�����̓p�����[�^�ɂ���ĈقȂ�܂��̂ŁA�\�[�X��ǂ�Ŕ��f
	���Ă��������B

	@param pp	�p�����[�^��ݒ肷��|�P�����f�[�^
	@param id	�ݒ肵�����p�����[�^��ID
	@param buf	�ݒ肵�����p�����[�^�̓��͌�
*/
void PokeTool_SetPokeParam(POKEMON_PARAM *pp, PokeToolParamID id, void *buf)
{
	PokeParaPutAct(pp, id, buf);
}

/*!
	�|�P�����f�[�^���̃j�b�N�l�[���t���O���擾���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�j�b�N�l�[���t���O���擾����|�P�����f�[�^

	@retval TRUE	�j�b�N�l�[�����ݒ肳��Ă���
	@retval FALSE	�j�b�N�l�[�����ݒ肳��Ă��Ȃ�
*/
BOOL PokeTool_GetNickNameFlag(POKEMON_PARAM *pp)
{
	return PokeTool_GetPokeParam(pp, ID_PARA_nickname_flag, NULL);
}

/*!
	�|�P�����f�[�^���̃j�b�N�l�[���t���O��ݒ肵�܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�j�b�N�l�[���t���O��ݒ肷��|�P�����f�[�^
	@param flag	�ݒ肷��t���O�BTRUE�Ńj�b�N�l�[�����ݒ肳��Ă���AFALSE�Ńj�b�N�l�[�����ݒ�
				����Ă��Ȃ��Ƃ����Ӗ��ɂȂ�܂��B
*/
void PokeTool_SetNickNameFlag(POKEMON_PARAM *pp, BOOL flag)
{
	PokeTool_SetPokeParam(pp, ID_PARA_nickname_flag, &flag);
}

/*!
	�|�P�����f�[�^���̃j�b�N�l�[�����擾���܂��B���ʂ�UTF-16LE�ŏo�͂���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�j�b�N�l�[�����擾����|�P�����f�[�^
	@param buf	�擾�����j�b�N�l�[���̕ۑ���B(MONS_NAME_SIZE+EOM_SIZE)*sizeof(STRCODE)�ȏ��
				�傫���Ƃ��Ă��������B

	@retval TRUE	�擾�����������B
	@retval FALSE	�擾�����s�����B�j�b�N�l�[�����������|�P�����R�[�h�ł͂Ȃ������B
*/
BOOL PokeTool_GetNickName(POKEMON_PARAM *pp, u16* buf)
{
	u16 tempbuf[MONS_NAME_SIZE+EOM_SIZE];

	memset(tempbuf, 0, sizeof(tempbuf));
	PokeTool_GetPokeParam(pp, ID_PARA_nickname, tempbuf);
	return (pokecode_to_unicode(tempbuf, (wchar_t*)buf, sizeof(u16)*(MONS_NAME_SIZE+EOM_SIZE)) != -1);
}

/*!
	�|�P�����f�[�^���̃j�b�N�l�[����ݒ肵�܂��B���O��UTF-16LE�ŗ^���Ă��������B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�j�b�N�l�[����ݒ肷��|�P�����f�[�^
	@param buf	�ݒ肷��j�b�N�l�[���BMONS_NAME_SIZE�ȉ��̒����Ƃ��A�K��0�ŏI�[���Ă��������B

	@retval TRUE	�ݒ肪���������B
	@retval FALSE	�ݒ肪���s�����B�^����ꂽ���O���|�P�����R�[�h�ɕϊ��ł��Ȃ������B
*/
BOOL PokeTool_SetNickName(POKEMON_PARAM *pp, const u16* buf)
{
	u16 tempbuf[MONS_NAME_SIZE+EOM_SIZE];

	memset(tempbuf, 0, sizeof(tempbuf));
	if (unicode_to_pokecode((const wchar_t*)buf, tempbuf, sizeof(tempbuf)) == -1) {
		return FALSE;
	}
	PokeTool_SetPokeParam(pp, ID_PARA_nickname, tempbuf);

	return TRUE;
}

/*!
	�|�P�����f�[�^���̐e�̖��O���擾���܂��B���ʂ�UTF-16LE�ŏo�͂���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�e�̖��O���擾����|�P�����f�[�^
	@param buf	�擾�����e�̖��O�̕ۑ���B(PERSON_NAME_SIZE+EOM_SIZE)*sizeof(STRCODE)�ȏ��
				�傫���Ƃ��Ă��������B

	@retval TRUE	�擾�����������B
	@retval FALSE	�擾�����s�����B�e�̖��O���������|�P�����R�[�h�ł͂Ȃ������B
*/
BOOL PokeTool_GetParentName(POKEMON_PARAM *pp, u16* buf)
{
	u16 tempbuf[PERSON_NAME_SIZE+EOM_SIZE];

	memset(tempbuf, 0, sizeof(tempbuf));
	PokeTool_GetPokeParam(pp, ID_PARA_oyaname, tempbuf);
	return (pokecode_to_unicode(tempbuf, (wchar_t*)buf, sizeof(u16)*(PERSON_NAME_SIZE+EOM_SIZE)) != -1);
}

/*!
	�|�P�����f�[�^���̐e�̖��O��ݒ肵�܂��B���O��UTF-16LE�ŗ^���Ă��������B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�e�̖��O��ݒ肷��|�P�����f�[�^
	@param buf	�ݒ肷��e�̖��O�BPERSON_NAME_SIZE�ȉ��̒����Ƃ��A�K��0�ŏI�[���Ă��������B

	@retval TRUE	�ݒ肪���������B
	@retval FALSE	�ݒ肪���s�����B�^����ꂽ���O���|�P�����R�[�h�ɕϊ��ł��Ȃ������B
*/
BOOL PokeTool_SetParentName(POKEMON_PARAM *pp, const u16* buf)
{
	u16 tempbuf[PERSON_NAME_SIZE+EOM_SIZE];

	memset(tempbuf, 0, sizeof(tempbuf));
	if (unicode_to_pokecode((const wchar_t*)buf, tempbuf, sizeof(tempbuf)) == -1) {
		return FALSE;
	}
	PokeTool_SetPokeParam(pp, ID_PARA_oyaname, tempbuf);

	return TRUE;
}

/*!
	�|�P�����f�[�^���̃��[���̖��O���擾���܂��B���ʂ�UTF-16LE�ŏo�͂���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	���[���̖��O���擾����|�P�����f�[�^
	@param buf	�擾�������[���̖��O�̕ۑ���B(PERSON_NAME_SIZE+EOM_SIZE)*sizeof(STRCODE)�ȏ��
				�傫���Ƃ��Ă��������B

	@retval TRUE	�擾�����������B
	@retval FALSE	�擾�����s�����B���[���̖��O���������|�P�����R�[�h�ł͂Ȃ������B
*/
BOOL PokeTool_GetMailName(POKEMON_PARAM *pp, u16* buf)
{
	MAIL_DATA mail_data;

	PokeTool_GetPokeParam(pp, ID_PARA_mail_data, &mail_data);
	return (pokecode_to_unicode(mail_data.name, (wchar_t*)buf, sizeof(u16)*(PERSON_NAME_SIZE+EOM_SIZE)) != -1);
}

/*!
	�|�P�����f�[�^���̃��[���̖��O��ݒ肵�܂��B���O��UTF-16LE�ŗ^���Ă��������B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	���[���̖��O��ݒ肷��|�P�����f�[�^
	@param buf	�ݒ肷�郁�[���̖��O�BPERSON_NAME_SIZE�ȉ��̒����Ƃ��A�K��0�ŏI�[���Ă��������B

	@retval TRUE	�ݒ肪���������B
	@retval FALSE	�ݒ肪���s�����B�^����ꂽ���O���|�P�����R�[�h�ɕϊ��ł��Ȃ������B
*/
BOOL PokeTool_SetMailName(POKEMON_PARAM *pp, const u16* buf)
{
	MAIL_DATA mail_data;
	u16 tempbuf[PERSON_NAME_SIZE+EOM_SIZE];

	memset(tempbuf, 0, sizeof(tempbuf));
	if (unicode_to_pokecode((const wchar_t*)buf, tempbuf, sizeof(tempbuf)) == -1) {
		return FALSE;
	}
	PokeTool_GetPokeParam(pp, ID_PARA_mail_data, &mail_data);
	memcpy(mail_data.name, tempbuf, sizeof(u16)*(PERSON_NAME_SIZE+EOM_SIZE));
	PokeTool_SetPokeParam(pp, ID_PARA_mail_data, &mail_data);

	return TRUE;
}

/*!
	�o�g���^���[�̃|�P�����f�[�^���̃j�b�N�l�[�����擾���܂��B���ʂ�UTF-16LE�ŏo�͂���܂��B

	@param btp	�j�b�N�l�[�����擾����|�P�����f�[�^
	@param buf	�擾�����j�b�N�l�[���̕ۑ���B(MONS_NAME_SIZE+EOM_SIZE)*sizeof(STRCODE)�ȏ��
				�傫���Ƃ��Ă��������B

	@retval TRUE	�擾�ɐ��������B
	@retval FALSE	�擾�Ɏ��s�����B�j�b�N�l�[�����������|�P�����R�[�h�ł͂Ȃ������B
*/
BOOL PokeTool_BtGetNickName(B_TOWER_POKEMON* btp, u16* buf) {

	return (pokecode_to_unicode(btp->nickname, (wchar_t*)buf, sizeof(u16)*(MONS_NAME_SIZE+EOM_SIZE)) != -1);
}

/*!
	�o�g���^���[�̃|�P�����f�[�^���̃j�b�N�l�[����ݒ肵�܂��B�j�b�N�l�[����UTF-16LE�ŗ^���Ă��������B

	@param btp	�j�b�N�l�[����ݒ肷��|�P�����f�[�^
	@param buf	�ݒ肷��j�b�N�l�[���BMONS_NAME_SIZE�ȉ��̒����Ƃ��A�K��0�ŏI�[���Ă��������B

	@retval TRUE	�ݒ肪���������B
	@retval FALSE	�ݒ肪���s�����B�^����ꂽ���O���|�P�����R�[�h�ɕϊ��ł��Ȃ������B
*/
BOOL PokeTool_BtSetNickName(B_TOWER_POKEMON* btp, const u16* buf) {

	u16 tempbuf[MONS_NAME_SIZE+EOM_SIZE];

	if (unicode_to_pokecode((const wchar_t*)buf, tempbuf, sizeof(tempbuf)) == -1) {
		return FALSE;
	}
	memcpy(btp->nickname, tempbuf, sizeof(u16)*(MONS_NAME_SIZE+EOM_SIZE));

	return TRUE;
}

/*!
	�w�肵���|�P���������A�i�F�Ⴂ�j�ł��邩�ۂ���Ԃ��܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp ���A���ǂ����𔻒肵�����|�P�����f�[�^

	@retval	TRUE	�w�肵���|�P�����̓��A�i�F�Ⴂ�j�ł���
	@retval	FALSE	�w�肵���|�P�����̓��A�łȂ�
*/
BOOL PokeTool_IsRare(POKEMON_PARAM *pp) {

	return (PokeRareGetPara(PokeTool_GetPokeParam(pp, ID_PARA_id_no, NULL),
		PokeTool_GetPokeParam(pp, ID_PARA_personal_rnd, NULL)) != 0);
}

/*!
	�w�肵���o�g���^���[�̃|�P���������A�i�F�Ⴂ�j�ł��邩�ۂ���Ԃ��܂��B

	@param btp ���A���ǂ����𔻒肵�����o�g���^���[�̃|�P�����f�[�^

	@retval	TRUE	�w�肵���|�P�����̓��A�i�F�Ⴂ�j�ł���
	@retval	FALSE	�w�肵���|�P�����̓��A�łȂ�
*/
BOOL PokeTool_BtIsRare(B_TOWER_POKEMON *btp) {

	return (PokeRareGetPara(btp->id_no, btp->personal_rnd) != 0);
}

/*!
	�w�肵���|�P���������[���������Ă��邩��Ԃ��܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp ���[���������Ă��邩�ǂ����𔻒肵�����|�P�����f�[�^

	@retval	TRUE	�w�肵���|�P�����̓��[���������Ă���
	@retval	FALSE	�w�肵���|�P�����̓��[���������Ă��Ȃ�
*/
BOOL PokeTool_HasMail(POKEMON_PARAM *pp) {

	u16 item = (u16)PokeTool_GetPokeParam(pp, ID_PARA_item, NULL);

	if (ITEM_GURASUMEERU <= item && item <= ITEM_BURIKKUMEERU) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*!
	�w�肵���|�P�����̃��[���̌���R�[�h��Ԃ��܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	���̊֐��́A�f�[�^���̐��̐��l�����̂܂ܕԂ��܂��B�K��PokeTool_HasMail��
	���[����ێ����Ă��邩�ǂ������m�F���Ă���g�p���Ă��������B

	@param pp ���[���̌���R�[�h���擾�������|�P�����f�[�^

	@return	���[���̌���R�[�h�B����ȃf�[�^�ł́ACOUNTRY_CODE�Œ�`���ꂽ�l�ɂȂ�܂��B
*/
u8 PokeTool_GetMailLang(POKEMON_PARAM *pp) {

	MAIL_DATA mail_data;

	PokeTool_GetPokeParam(pp, ID_PARA_mail_data, &mail_data);

	return mail_data.region;
}

/*!
	�w�肵�������X�^�[�i���o�[�̃|�P�����̃f�t�H���g����Ԃ��܂��B

	@param monsno	�f�t�H���g�����擾�������|�P�����̃����X�^�[�i���o�[�B1�ȏ�MONSNO_END�ȉ��̒l�ɂ��Ă��������B
	@param countryCode	�f�t�H���g�����擾���������R�[�h�B1�ȏ�COUNTRY_CODE�ȉ��̒l�ɂ��Ă��������B

	@return �|�P�����̃f�t�H���g���BUTF-16LE�ŗ^�����܂��B

	@bug ���̊֐��́AUNIX���ł͐���ɓ��삵�܂���B
*/
const wchar_t* PokeTool_GetDefaultName(u32 monsno, u8 countryCode) {

	const wchar_t* result;

	DPW_MINMAX_TASSERT(monsno, 1, MONSNO_END);
	DPW_MINMAX_TASSERT(countryCode, 1, COUNTRY_CODE_MAX);
    if(monsno < 1 || monsno > MONSNO_END)
    {
        return POKEMON_NONE_NAME;
    }
    if(countryCode < 1 || countryCode > COUNTRY_CODE_MAX)
    {
        return POKEMON_NONE_NAME;
    }

#ifdef WIN32
	result = pokemon_default_name[monsno - 1][countryCode - 1];
#else
	result = L"";
#endif

	return wcscmp(result, L"") != 0 ? result : POKEMON_NONE_NAME;
}

/*!
	�|�P�����̖��O���f�t�H���g�ɐݒ肵�܂��B
	��̓I�ɂ́A�j�b�N�l�[���l�[���t���O��OFF�ɂ��A�j�b�N�l�[�����f�t�H���g���ɕύX���܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	���O���f�t�H���g�ɐݒ肷��|�P�����f�[�^

	@retval TRUE	�ݒ�ɐ��������B
	@retval FALSE	�ݒ�Ɏ��s�����B�|�P�����f�[�^���̃����X�^�[�i���o�[���s���Ȓl�������B

	@bug ���̊֐��́AUNIX���ł͐���ɓ��삵�܂���B
*/
BOOL PokeTool_SetNickNameDefault(POKEMON_PARAM *pp) {

	u32 monsno = PokeTool_GetPokeParam(pp, ID_PARA_monsno, NULL);
	u8 countryCode = (u8)PokeTool_GetPokeParam(pp, ID_PARA_country_code, NULL);

	if (monsno == 0 || monsno > MONSNO_END) {
		return FALSE;
	}

	if (countryCode == 0 || countryCode > COUNTRY_CODE_MAX || countryCode == COUNTRY_CODE_RESERVE) {
		return FALSE;
	}

	PokeTool_SetNickNameFlag(pp, FALSE);
	(void)PokeTool_SetNickName(pp, (const u16*)PokeTool_GetDefaultName(monsno, countryCode));

	return TRUE;
}


/*-----------------------------------------------------------------------*
					���[�J���֐���`
 *-----------------------------------------------------------------------*/

/**
 *	�|�P�����p�����[�^�̃`�F�b�N�T���𐶐�
 *
 * @param[in]	data	�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	size	�`�F�b�N�T���𐶐�����f�[�^�̃T�C�Y
 *
 * @return	���������`�F�b�N�T��
 */
static u16 PokeParaCheckSum(void *data, u32 size)
{
	u32	i;
	u16* data_p = (u16*)data;
	u16	sum = 0;

	for (i=0; i < size/2; i++) {
		sum += data_p[i];
	}

	return sum;
}

/**
 *	��������
 *
 * @param[in]	data	��������f�[�^�̃|�C���^
 * @param[in]	size	��������f�[�^�̃T�C�Y
 * @param[in]	code	�Í����L�[�̏����l
 */
static void PokeParaDecoded(void *data, u32 size, u32 code)
{
	PokeParaCoded(data, size, code);
}

/**
 *	�Í�����
 *
 * @param[in]	data	�Í�������f�[�^�̃|�C���^
 * @param[in]	size	�Í�������f�[�^�̃T�C�Y
 * @param[in]	code	�Í����L�[�̏����l
 */
static void PokeParaCoded(void *data, u32 size, u32 code)
{
	u32	i;
	u16* data_p = (u16*)data;

	//�Í��́A�����Í��L�[�Ń}�X�N
	for (i=0; i < size/2; i++) {
		data_p[i] ^= CodeRand(&code);
	}
}

/**
 *	�����Í��L�[�������[�`��
 *
 * @param[in,out]	code	�Í��L�[�i�[���[�N�̃|�C���^
 *
 * @return	�Í��L�[�i�[���[�N�̏��2�o�C�g���Í��L�[�Ƃ��ĕԂ�
 */
static u16 CodeRand(u32 *code)
{
    code[0] = code[0]*1103515245L + 24691;
    return (u16)(code[0] / 65536L);
}

/**
 *	�|�P�����p�����[�^�\���̂���C�ӂŒl���擾
 *
 * @param[in]	pp	�擾����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	id	�擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]	buf	�擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return		�擾�����f�[�^
 */
static u32 PokeParaGetAct(POKEMON_PARAM *pp, int id, void *buf)
{
	u32	ret=0;

	switch(id){
	case ID_PARA_condition:
		ret=pp->pcp.condition;
		break;
	case ID_PARA_level:
		ret=pp->pcp.level;
		break;
	case ID_PARA_cb_id:
		ret=pp->pcp.cb_id;
		break;
	case ID_PARA_hp:
		ret=pp->pcp.hp;
		break;
	case ID_PARA_hpmax:
		ret=pp->pcp.hpmax;
		break;
	case ID_PARA_pow:
		ret=pp->pcp.pow;
		break;
	case ID_PARA_def:
		ret=pp->pcp.def;
		break;
	case ID_PARA_agi:
		ret=pp->pcp.agi;
		break;
	case ID_PARA_spepow:
		ret=pp->pcp.spepow;
		break;
	case ID_PARA_spedef:
		ret=pp->pcp.spedef;
		break;
	case ID_PARA_mail_data:
		memcpy(buf, &pp->pcp.mail_data, sizeof(MAIL_DATA));
		ret = TRUE;
		break;
	case ID_PARA_cb_core:
		memcpy(buf, &pp->pcp.cb_core,  sizeof(CB_CORE));
		ret = TRUE;
		break;
	default:
		ret = PokePasoParaGetAct((POKEMON_PASO_PARAM *)&pp->ppp, id, buf);
		break;
	}

	return	ret;
}

/**
 *	�{�b�N�X�|�P�����p�����[�^�\���̂���C�ӂŒl���擾
 *
 * @param[in]	pp	�擾����{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	id	�擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]	buf	�擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return		�擾�����f�[�^
 */
static u32 PokePasoParaGetAct(POKEMON_PASO_PARAM *ppp,int id, void *buf)
{
	u32	ret=0;
	u64	bit;
	POKEMON_PASO_PARAM1	*ppp1;
	POKEMON_PASO_PARAM2	*ppp2;
	POKEMON_PASO_PARAM3	*ppp3;
	POKEMON_PASO_PARAM4	*ppp4;

	ppp1=(POKEMON_PASO_PARAM1 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA1);
	ppp2=(POKEMON_PASO_PARAM2 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA2);
	ppp3=(POKEMON_PASO_PARAM3 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA3);
	ppp4=(POKEMON_PASO_PARAM4 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA4);

	switch(id){
		default:
			ret=0;
			break;
//PARAM
		case ID_PARA_personal_rnd:
			ret=ppp->personal_rnd;
			break;
		case ID_PARA_pp_fast_mode:
			ret=ppp->pp_fast_mode;
			break;
		case ID_PARA_ppp_fast_mode:
			ret=ppp->ppp_fast_mode;
			break;
		case ID_PARA_checksum:
			ret=ppp->checksum;
			break;
		case ID_PARA_poke_exist:
			ret=(ppp1->monsno!=0);
			break;
		case ID_PARA_tamago_exist:
			ret=ppp2->tamago_flag;
			break;
		case ID_PARA_monsno_egg:
			ret=ppp1->monsno;
			if(ret==0){
				break;
			}
			if(ppp2->tamago_flag){
				ret=MONSNO_TAMAGO;
			}
			break;
		case ID_PARA_level:
			DPW_TASSERTMSG(FALSE, "get ID_PARA_level is not implemented\n");
//			ret=PokeLevelCalc(ppp1->monsno,ppp1->exp);
			break;
//PARAM1
		case ID_PARA_monsno:
			ret=ppp1->monsno;
			break;
		case ID_PARA_item:
			ret=ppp1->item;
			break;
		case ID_PARA_id_no:
			ret=ppp1->id_no;
			break;
		case ID_PARA_exp:
			ret=ppp1->exp;
			break;
		case ID_PARA_friend:
			ret=ppp1->_friend;
			break;
		case ID_PARA_speabino:
			ret=ppp1->speabino;
			break;
		case ID_PARA_mark:
			ret=ppp1->mark;
			break;
		case ID_PARA_country_code:
			ret=ppp1->country_code;
			break;
		case ID_PARA_hp_exp:
			ret=ppp1->hp_exp;
			break;
		case ID_PARA_pow_exp:
			ret=ppp1->pow_exp;
			break;
		case ID_PARA_def_exp:
			ret=ppp1->def_exp;
			break;
		case ID_PARA_agi_exp:
			ret=ppp1->agi_exp;
			break;
		case ID_PARA_spepow_exp:
			ret=ppp1->spepow_exp;
			break;
		case ID_PARA_spedef_exp:
			ret=ppp1->spedef_exp;
			break;
		case ID_PARA_style:
			ret=ppp1->style;
			break;
		case ID_PARA_beautiful:
			ret=ppp1->beautiful;
			break;
		case ID_PARA_cute:
			ret=ppp1->cute;
			break;
		case ID_PARA_clever:
			ret=ppp1->clever;
			break;
		case ID_PARA_strong:
			ret=ppp1->strong;
			break;
		case ID_PARA_fur:
			ret=ppp1->fur;
			break;
		case ID_PARA_sinou_champ_ribbon:				//�V���I�E�`�����v���{��
		case ID_PARA_sinou_battle_tower_ttwin_first:	//�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
		case ID_PARA_sinou_battle_tower_ttwin_second:	//�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
		case ID_PARA_sinou_battle_tower_2vs2_win50:		//�V���I�E�o�g���^���[�^���[�_�u��50�A��
		case ID_PARA_sinou_battle_tower_aimulti_win50:	//�V���I�E�o�g���^���[�^���[AI�}���`50�A��
		case ID_PARA_sinou_battle_tower_siomulti_win50:	//�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
		case ID_PARA_sinou_battle_tower_wifi_rank5:		//�V���I�E�o�g���^���[Wifi�����N�T����
		case ID_PARA_sinou_syakki_ribbon:				//�V���I�E����������{��
		case ID_PARA_sinou_dokki_ribbon:				//�V���I�E�ǂ������{��
		case ID_PARA_sinou_syonbo_ribbon:				//�V���I�E�����ڃ��{��
		case ID_PARA_sinou_ukka_ribbon:					//�V���I�E���������{��
		case ID_PARA_sinou_sukki_ribbon:				//�V���I�E���������{��
		case ID_PARA_sinou_gussu_ribbon:				//�V���I�E���������{��
		case ID_PARA_sinou_nikko_ribbon:				//�V���I�E�ɂ������{��
		case ID_PARA_sinou_gorgeous_ribbon:				//�V���I�E�S�[�W���X���{��
		case ID_PARA_sinou_royal_ribbon:				//�V���I�E���C�������{��
		case ID_PARA_sinou_gorgeousroyal_ribbon:		//�V���I�E�S�[�W���X���C�������{��
		case ID_PARA_sinou_ashiato_ribbon:				//�V���I�E�������ƃ��{��
		case ID_PARA_sinou_record_ribbon:				//�V���I�E���R�[�h���{��
		case ID_PARA_sinou_history_ribbon:				//�V���I�E�q�X�g���[���{��
		case ID_PARA_sinou_legend_ribbon:				//�V���I�E���W�F���h���{��
		case ID_PARA_sinou_red_ribbon:					//�V���I�E���b�h���{��
		case ID_PARA_sinou_green_ribbon:				//�V���I�E�O���[�����{��
		case ID_PARA_sinou_blue_ribbon:					//�V���I�E�u���[���{��
		case ID_PARA_sinou_festival_ribbon:				//�V���I�E�t�F�X�e�B�o�����{��
		case ID_PARA_sinou_carnival_ribbon:				//�V���I�E�J�[�j�o�����{��
		case ID_PARA_sinou_classic_ribbon:				//�V���I�E�N���V�b�N���{��
		case ID_PARA_sinou_premiere_ribbon:				//�V���I�E�v���~�A���{��
		case ID_PARA_sinou_amari_ribbon:				//���܂�
			bit=1;
			ret=((ppp1->sinou_ribbon&(bit<<(id-ID_PARA_sinou_champ_ribbon)))!=0);
			break;
//PARAM2
		case ID_PARA_waza1:
		case ID_PARA_waza2:
		case ID_PARA_waza3:
		case ID_PARA_waza4:
			ret=ppp2->waza[id-ID_PARA_waza1];
			break;
		case ID_PARA_pp1:
		case ID_PARA_pp2:
		case ID_PARA_pp3:
		case ID_PARA_pp4:
			ret=ppp2->pp[id-ID_PARA_pp1];
			break;
		case ID_PARA_pp_count1:
		case ID_PARA_pp_count2:
		case ID_PARA_pp_count3:
		case ID_PARA_pp_count4:
			ret=ppp2->pp_count[id-ID_PARA_pp_count1];
			break;
		case ID_PARA_pp_max1:
		case ID_PARA_pp_max2:
		case ID_PARA_pp_max3:
		case ID_PARA_pp_max4:
			DPW_TASSERTMSG(FALSE, "get ID_PARA_pp_max? is not implemented\n");
//			ret=WT_PPMaxGet(ppp2->waza[id-ID_PARA_pp_max1],ppp2->pp_count[id-ID_PARA_pp_max1]);
			break;
		case ID_PARA_hp_rnd:
			ret=ppp2->hp_rnd;
			break;
		case ID_PARA_pow_rnd:
			ret=ppp2->pow_rnd;
			break;
		case ID_PARA_def_rnd:
			ret=ppp2->def_rnd;
			break;
		case ID_PARA_agi_rnd:
			ret=ppp2->agi_rnd;
			break;
		case ID_PARA_spepow_rnd:
			ret=ppp2->spepow_rnd;
			break;
		case ID_PARA_spedef_rnd:
			ret=ppp2->spedef_rnd;
			break;
		case ID_PARA_tamago_flag:
			ret=ppp2->tamago_flag;
			break;
		case ID_PARA_nickname_flag:
			ret=ppp2->nickname_flag;
			break;
		case ID_PARA_stylemedal_normal:
		case ID_PARA_stylemedal_super:
		case ID_PARA_stylemedal_hyper:
		case ID_PARA_stylemedal_master:
		case ID_PARA_beautifulmedal_normal:
		case ID_PARA_beautifulmedal_super:
		case ID_PARA_beautifulmedal_hyper:
		case ID_PARA_beautifulmedal_master:
		case ID_PARA_cutemedal_normal:
		case ID_PARA_cutemedal_super:
		case ID_PARA_cutemedal_hyper:
		case ID_PARA_cutemedal_master:
		case ID_PARA_clevermedal_normal:
		case ID_PARA_clevermedal_super:
		case ID_PARA_clevermedal_hyper:
		case ID_PARA_clevermedal_master:
		case ID_PARA_strongmedal_normal:
		case ID_PARA_strongmedal_super:
		case ID_PARA_strongmedal_hyper:
		case ID_PARA_strongmedal_master:
		case ID_PARA_champ_ribbon:
		case ID_PARA_winning_ribbon:
		case ID_PARA_victory_ribbon:
		case ID_PARA_bromide_ribbon:
		case ID_PARA_ganba_ribbon:
		case ID_PARA_marine_ribbon:
		case ID_PARA_land_ribbon:
		case ID_PARA_sky_ribbon:
		case ID_PARA_country_ribbon:
		case ID_PARA_national_ribbon:
		case ID_PARA_earth_ribbon:
		case ID_PARA_world_ribbon:
			bit=1;
			ret=((ppp2->old_ribbon&(bit<<(id-ID_PARA_stylemedal_normal)))!=0);
			break;
		case ID_PARA_event_get_flag:
			ret=ppp2->event_get_flag;
			break;
		case ID_PARA_sex:
			ret=ppp2->sex;
			break;
		case ID_PARA_form_no:
			if((ppp1->monsno==MONSNO_AUSU)&&(ppp1->speabino==TOKUSYU_MARUTITAIPU)){
				DPW_TASSERTMSG(FALSE, "get ID_PARA_form_no for MONSNO_AUSU is not implemented\n");
//				ret=AusuTypeGet(ItemParamGet(ppp1->item,ITEM_PRM_EQUIP,HEAPID_BASE_SYSTEM));
			}
			else{
				ret=ppp2->form_no;
			}
			break;
		case ID_PARA_dummy_p2_1:
			ret=ppp2->dummy_p2_1;
			break;
		case ID_PARA_dummy_p2_2:
			ret=ppp2->dummy_p2_2;
			break;
//PARAM3
		case ID_PARA_nickname:
		{
			u16	*buf16=(u16*)buf;
			for(ret=0;ret<MONS_NAME_SIZE;ret++){
				buf16[ret]=ppp3->nickname[ret];
			}
			buf16[ret]=EOM_;
		}
		break;
		case ID_PARA_nickname_buf_flag:
			ret=ppp2->nickname_flag;
		case ID_PARA_nickname_buf:
			DPW_TASSERTMSG(FALSE, "get ID_PARA_nickname_buf is not implemented\n");
//			STRBUF_SetStringCode((STRBUF*)buf, ppp3->nickname);
#if 0
			if( ppp2->nickname_flag )
			{
				STRBUF_SetStringCode((STRBUF*)buf, ppp3->nickname);
			}
			else
			{
				STRBUF*  default_monsname = MSGDAT_UTIL_GetMonsName(ppp1->monsno, HEAPID_BASE_SYSTEM);
				STRBUF_Copy((STRBUF*)buf, default_monsname);
				STRBUF_Delete(default_monsname);
			}
#endif
			break;

		case ID_PARA_pref_code:
			ret=ppp3->pref_code;
			break;
		case ID_PARA_get_cassette:
			ret=ppp3->get_cassette;
			break;
		case ID_PARA_trial_stylemedal_normal:
		case ID_PARA_trial_stylemedal_super:
		case ID_PARA_trial_stylemedal_hyper:
		case ID_PARA_trial_stylemedal_master:
		case ID_PARA_trial_beautifulmedal_normal:
		case ID_PARA_trial_beautifulmedal_super:
		case ID_PARA_trial_beautifulmedal_hyper:
		case ID_PARA_trial_beautifulmedal_master:
		case ID_PARA_trial_cutemedal_normal:
		case ID_PARA_trial_cutemedal_super:
		case ID_PARA_trial_cutemedal_hyper:
		case ID_PARA_trial_cutemedal_master:
		case ID_PARA_trial_clevermedal_normal:
		case ID_PARA_trial_clevermedal_super:
		case ID_PARA_trial_clevermedal_hyper:
		case ID_PARA_trial_clevermedal_master:
		case ID_PARA_trial_strongmedal_normal:
		case ID_PARA_trial_strongmedal_super:
		case ID_PARA_trial_strongmedal_hyper:
		case ID_PARA_trial_strongmedal_master:
		case ID_PARA_amari_ribbon:
			bit=1;
			ret=((ppp3->new_ribbon&(bit<<(id-ID_PARA_trial_stylemedal_normal)))!=0);
			break;
//PARAM4
		case ID_PARA_oyaname:
		{
			u16 *buf16=(u16*)buf;
			for(ret=0;ret<PERSON_NAME_SIZE;ret++){
				buf16[ret]=ppp4->oyaname[ret];
			}
			buf16[ret]=EOM_;
			break;
		}

		case ID_PARA_oyaname_buf:
			DPW_TASSERTMSG(FALSE, "get ID_PARA_oyaname_buf is not implemented\n");
//			STRBUF_SetStringCode((STRBUF*)buf, ppp4->oyaname);
			break;

		case ID_PARA_get_year:							//�߂܂����N
			ret=ppp4->get_year;							//�߂܂����N
			break;
		case ID_PARA_get_month:							//�߂܂�����
			ret=ppp4->get_month;						//�߂܂�����
			break;
		case ID_PARA_get_day:							//�߂܂�����
			ret=ppp4->get_day;							//�߂܂�����
			break;
		case ID_PARA_birth_year:						//���܂ꂽ�N
			ret=ppp4->birth_year;						//���܂ꂽ�N
			break;
		case ID_PARA_birth_month:						//���܂ꂽ��
			ret=ppp4->birth_month;						//���܂ꂽ��
			break;
		case ID_PARA_birth_day:							//���܂ꂽ��
			ret=ppp4->birth_day;						//���܂ꂽ��
			break;
		case ID_PARA_get_place:							//�߂܂����ꏊ
			ret=ppp4->get_place;						//�߂܂����ꏊ
			break;
		case ID_PARA_birth_place:						//���܂ꂽ�ꏊ
			ret=ppp4->birth_place;						//���܂ꂽ�ꏊ
			break;
		case ID_PARA_pokerus:							//�|�P���X
			ret=ppp4->pokerus;							//�|�P���X
			break;
		case ID_PARA_get_ball:							//�߂܂����{�[��
			ret=ppp4->get_ball;							//�߂܂����{�[��
			break;
		case ID_PARA_get_level:							//�߂܂������x��
			ret=ppp4->get_level;						//�߂܂������x��
			break;
		case ID_PARA_oyasex:							//�e�̐���
			ret=ppp4->oyasex;							//�e�̐���
			break;
		case ID_PARA_get_ground_id:						//�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
			ret=ppp4->get_ground_id;					//�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
			break;
		case ID_PARA_dummy_p4_1:						//���܂�
			ret=ppp4->dummy_p4_1;						//���܂�
			break;

		case ID_PARA_power_rnd:
			ret=(ppp2->hp_rnd		<< 0)|
				(ppp2->pow_rnd		<< 5)|
				(ppp2->def_rnd		<<10)|
				(ppp2->agi_rnd		<<15)|
				(ppp2->spepow_rnd	<<20)|
				(ppp2->spedef_rnd	<<25);
			break;
		//�����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N
		case ID_PARA_nidoran_nickname:
			if(((ppp1->monsno==MONSNO_NIDORAN_F)||(ppp1->monsno==MONSNO_NIDORAN_M))&&(ppp2->nickname_flag==0)){
				ret=FALSE;
			}
			else{
				ret=TRUE;
			}
			break;
		case ID_PARA_type1:
		case ID_PARA_type2:
			DPW_TASSERTMSG(FALSE, "get ID_PARA_type? is not implemented\n");
//			if((ppp1->monsno==MONSNO_AUSU)&&(ppp1->speabino==TOKUSYU_MARUTITAIPU)){
//				ret=AusuTypeGet(ItemParamGet(ppp1->item,ITEM_PRM_EQUIP,HEAPID_BASE_SYSTEM));
//			}
//			else{
//				ret=PokeFormNoPersonalParaGet(ppp1->monsno,ppp2->form_no,ID_PER_type1+(id-ID_PARA_type1));
//			}
			break;
		case ID_PARA_default_name:						//�|�P�����̃f�t�H���g��
			DPW_TASSERTMSG(FALSE, "get ID_PARA_default_name is not implemented\n");
//			MSGDAT_MonsNameGet(ppp1->monsno,HEAPID_BASE_SYSTEM,buf);
			break;
	}

	return	ret;
}

/**
 *	�{�b�N�X�|�P�����p�����[�^�\���̓��̃����o�̃A�h���X���擾
 *
 * @param[in]	ppp	�擾�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	rnd	�\���̓���ւ��̃L�[
 * @param[in]	id	���o�����������o�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return	�擾�����A�h���X
 */
static void *PokeParaAdrsGet(POKEMON_PASO_PARAM *ppp, u32 rnd, u8 id)
{
	void	*ret;
	rnd=(rnd&0x0003e000)>>13;

	DPW_TASSERTMSG(rnd<=ID_POKEPARADATA62,"PokeParaAdrsGet:Index Over!");

	switch(rnd){
	case ID_POKEPARADATA11:
	case ID_POKEPARADATA51:
		{
			POKEMON_PARA_DATA11 *PPD11=(POKEMON_PARA_DATA11 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD11->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD11->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD11->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD11->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA12:
	case ID_POKEPARADATA52:
		{
			POKEMON_PARA_DATA12 *PPD12=(POKEMON_PARA_DATA12 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD12->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD12->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD12->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD12->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA13:
	case ID_POKEPARADATA53:
		{
			POKEMON_PARA_DATA13 *PPD13=(POKEMON_PARA_DATA13 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD13->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD13->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD13->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD13->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA14:
	case ID_POKEPARADATA54:
		{
			POKEMON_PARA_DATA14 *PPD14=(POKEMON_PARA_DATA14 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD14->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD14->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD14->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD14->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA15:
	case ID_POKEPARADATA55:
		{
			POKEMON_PARA_DATA15 *PPD15=(POKEMON_PARA_DATA15 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD15->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD15->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD15->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD15->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA16:
	case ID_POKEPARADATA56:
		{
			POKEMON_PARA_DATA16 *PPD16=(POKEMON_PARA_DATA16 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD16->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD16->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD16->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD16->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA21:
	case ID_POKEPARADATA61:
		{
			POKEMON_PARA_DATA21 *PPD21=(POKEMON_PARA_DATA21 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD21->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD21->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD21->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD21->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA22:
	case ID_POKEPARADATA62:
		{
			POKEMON_PARA_DATA22 *PPD22=(POKEMON_PARA_DATA22 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD22->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD22->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD22->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD22->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA23:
		{
			POKEMON_PARA_DATA23 *PPD23=(POKEMON_PARA_DATA23 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD23->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD23->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD23->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD23->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA24:
		{
			POKEMON_PARA_DATA24 *PPD24=(POKEMON_PARA_DATA24 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD24->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD24->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD24->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD24->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA25:
		{
			POKEMON_PARA_DATA25 *PPD25=(POKEMON_PARA_DATA25 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD25->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD25->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD25->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD25->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA26:
		{
			POKEMON_PARA_DATA26 *PPD26=(POKEMON_PARA_DATA26 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD26->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD26->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD26->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD26->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA31:
		{
			POKEMON_PARA_DATA31 *PPD31=(POKEMON_PARA_DATA31 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD31->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD31->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD31->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD31->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA32:
		{
			POKEMON_PARA_DATA32 *PPD32=(POKEMON_PARA_DATA32 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD32->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD32->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD32->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD32->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA33:
		{
			POKEMON_PARA_DATA33 *PPD33=(POKEMON_PARA_DATA33 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD33->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD33->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD33->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD33->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA34:
		{
			POKEMON_PARA_DATA34 *PPD34=(POKEMON_PARA_DATA34 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD34->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD34->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD34->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD34->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA35:
		{
			POKEMON_PARA_DATA35 *PPD35=(POKEMON_PARA_DATA35 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD35->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD35->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD35->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD35->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA36:
		{
			POKEMON_PARA_DATA36 *PPD36=(POKEMON_PARA_DATA36 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD36->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD36->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD36->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD36->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA41:
		{
			POKEMON_PARA_DATA41 *PPD41=(POKEMON_PARA_DATA41 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD41->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD41->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD41->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD41->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA42:
		{
			POKEMON_PARA_DATA42 *PPD42=(POKEMON_PARA_DATA42 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD42->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD42->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD42->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD42->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA43:
		{
			POKEMON_PARA_DATA43 *PPD43=(POKEMON_PARA_DATA43 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD43->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD43->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD43->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD43->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA44:
		{
			POKEMON_PARA_DATA44 *PPD44=(POKEMON_PARA_DATA44 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD44->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD44->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD44->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD44->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA45:
		{
			POKEMON_PARA_DATA45 *PPD45=(POKEMON_PARA_DATA45 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD45->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD45->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD45->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD45->ppp4;
					break;
			}
			break;
		}
	case ID_POKEPARADATA46:
		{
			POKEMON_PARA_DATA46 *PPD46=(POKEMON_PARA_DATA46 *)&ppp->paradata;
			switch(id){
				case ID_POKEPARA1:
					ret=&PPD46->ppp1;
					break;
				case ID_POKEPARA2:
					ret=&PPD46->ppp2;
					break;
				case ID_POKEPARA3:
					ret=&PPD46->ppp3;
					break;
				case ID_POKEPARA4:
					ret=&PPD46->ppp4;
					break;
			}
			break;
		}
	}
	return	ret;
}

/**
 *	�|�P�����i���o�[�A�o���l����|�P�����̃��x�����v�Z
 *	�i�����Ńp�[�\�i���f�[�^�����[�h����j
 *
 * @param[in]	mons_no	�擾�������|�P�����i���o�[
 * @param[in]	exp		�擾�������|�P�����̌o���l
 *
 * @return	�擾�������x��
 */
static u32 PokeLevelCalc(u16 mons_no, u32 exp)
{
	u32	level;

	POKEMON_PERSONAL_DATA* personalData = PokePersonalPara_Open(mons_no);

	level = PokePersonal_LevelCalc( personalData, mons_no, exp );

	PokePersonalPara_Close( personalData );

	return level;
}

/**
 * �p�[�\�i���f�[�^�I�[�v��
 *
 * ���̊֐��ŃI�[�v�������f�[�^��PokePersonalPara_Get�Ŏ擾���܂�
 * PokePersonalPara_Close�ŉ��
 *
 * @param   mons_no		
 * @param   HeapID		
 *
 * @retval  POKEMON_PERSONAL_DATA *		
 */
static POKEMON_PERSONAL_DATA *PokePersonalPara_Open(int mons_no)
{
	POKEMON_PERSONAL_DATA *ppd;
	
	ppd = (POKEMON_PERSONAL_DATA*)malloc(sizeof(POKEMON_PERSONAL_DATA));
	PokePersonalDataGet(mons_no, ppd);
	
	return ppd;
}

/**
 * �p�[�\�i���f�[�^���
 *
 * PokePersonalPara_Open�Ŋm�ۂ�����������������܂�
 *
 * @param   ppd		
 *
 * @retval  none		
 */
static void PokePersonalPara_Close(POKEMON_PERSONAL_DATA *ppd)
{
	DPW_NULL_TASSERT( ppd );

	free(ppd);
}

/**
 *	�|�P�����p�[�\�i���\���̃f�[�^���擾
 *
 * @param[in]	mons_no	�擾�������|�P�����i���o�[
 * @param[out]	ppd		�擾�����p�[�\�i���f�[�^�̊i�[����w��
 */
static void PokePersonalDataGet(int mons_no, POKEMON_PERSONAL_DATA *ppd)
{
	ArchiveDataLoad(ppd, "personal.narc", mons_no);
}

/**
 * �A�[�J�C�u�t�@�C���f�[�^���[�h
 *
 * @param[in]	data		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�̃|�C���^
 * @param[in]	file		�ǂݍ��ރA�[�J�C�u�t�@�C���̃t�@�C����
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
static void ArchiveDataLoad(void *data, const char* file, int datID)
{
	ArchiveDataLoadIndex(data, file, datID, OFS_NO_SET, SIZE_NO_SET);
}

/**
 *
 *	nnsarc�ō쐬�����A�[�J�C�u�t�@�C���ɑ΂���Index���w�肵�ĔC�ӂ̃f�[�^�����o��
 *
 * @param[in]	data		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�̃|�C���^
 * @param[in]	name		�ǂݍ��ރA�[�J�C�u�t�@�C����
 * @param[in]	index		�ǂݍ��ރf�[�^�̃A�[�J�C�u��̃C���f�b�N�X�i���o�[
 * @param[in]	ofs			�ǂݍ��ރf�[�^�̐擪����̃I�t�Z�b�g
 * @param[in]	ofs_size	�ǂݍ��ރf�[�^�T�C�Y
 *
 */
static void ArchiveDataLoadIndex(void *data, const char *name, int index, int ofs, int ofs_size)
{
	FILE*		p_file;
	u32			size=0;
	u32			fat_top=0;
	u32			fnt_top=0;
	u32			img_top=0;
	u32			top=0;
	u32			bottom=0;
	u16			file_cnt=0;

	p_file = fopen(name, "rb");
	DPW_NULL_TASSERT(p_file);

	fseek(p_file, ARC_HEAD_SIZE_POS, SEEK_SET);			// �A�[�J�C�u�w�b�_�̃T�C�Y�i�[�ʒu�Ɉړ�
	fread(&size, 2, 1, p_file);							// �A�[�J�C�u�w�b�_�T�C�Y�����[�h
	fat_top = size;

	fseek(p_file, fat_top + SIZE_OFFSET, SEEK_SET);		// FAT�̃T�C�Y�i�[�ʒu�Ɉړ�
	fread(&size, 4, 1, p_file);							// FAT�T�C�Y�����[�h
	fread(&file_cnt, 2, 1, p_file);							// FileCount�����[�h
	DPW_TASSERTMSG(file_cnt > index, "ServerArchiveDataLoadIndex file=%s, fileCnt=%d, index=%d", name, file_cnt, index);
	fnt_top = fat_top + size;

	fseek(p_file, fnt_top + SIZE_OFFSET, SEEK_SET);		// FNT�̃T�C�Y�i�[�ʒu�Ɉړ�
	fread(&size, 4, 1, p_file);							// FNT�T�C�Y�����[�h
	img_top = fnt_top + size;

	fseek(p_file, fat_top + FAT_HEAD_SIZE + index*8, SEEK_SET);	// ���o������FAT�e�[�u���Ɉړ�
	fread(&top, 4, 1, p_file);									// FAT�e�[�u��top�����[�h
	fread(&bottom, 4, 1, p_file);								// FAT�e�[�u��bottom�����[�h

	fseek(p_file, img_top + IMG_HEAD_SIZE + top + ofs, SEEK_SET);	// ���o������IMG�̐擪�Ɉړ�
	
	if (ofs_size) {
		size = ofs_size;
	} else{
		size = bottom - top;
	}

	DPW_TASSERTMSG(size!=0,"ServerArchiveDataLoadIndex:ReadDataSize=0!");

	fread(&data, size, 1, p_file);								// FAT�e�[�u��bottom�����[�h

	fclose(p_file);
}

/**
 *	�|�P�����i���o�[�A�o���l����|�P�����̃��x�����v�Z
 *	�i���[�h�ς݃p�[�\�i���f�[�^�𗘗p����j
 *
 * @param[in]	personalData	�|�P�����p�[�\�i���f�[�^
 * @param[in]	mons_no			�|�P�����i���o�[
 * @param[in]	exp				�o���l
 *
 * @return	�擾�������x��
 */
static u32 PokePersonal_LevelCalc(POKEMON_PERSONAL_DATA* personalData, u16 monsno, u32 exp)
{
	static u32 grow_tbl[101];
	int grow, level;

	grow = PokePersonalPara_Get( personalData, ID_PER_grow );
	PokeGrowDataGet( grow, grow_tbl );

	for(level=1;level<101;level++){
		if(grow_tbl[level]>exp) break;
	}

	return level-1;
}

/**
 * �p�[�\�i���f�[�^�擾
 *
 * PokePersonalPara_Open�Ŋ֐��ŏ������Ă���f�[�^���擾���܂�
 * PokePersonalPara_Close�ŉ��
 *
 * @param   ppd		
 * @param   para		
 *
 * @retval  u32		
 */
u32 PokePersonalPara_Get(POKEMON_PERSONAL_DATA *ppd, int para)
{
	u32 ret;

	DPW_NULL_TASSERT( ppd );
	
	switch(para){
	case ID_PER_basic_hp:		//��{�g�o
		ret=ppd->basic_hp;
		break;
	case ID_PER_basic_pow:		//��{�U����
		ret=ppd->basic_pow;
		break;
	case ID_PER_basic_def:		//��{�h���
		ret=ppd->basic_def;
		break;
	case ID_PER_basic_agi:		//��{�f����
		ret=ppd->basic_agi;
		break;
	case ID_PER_basic_spepow:	//��{����U����
		ret=ppd->basic_spepow;
		break;
	case ID_PER_basic_spedef:	//��{����h���
		ret=ppd->basic_spedef;
		break;
	case ID_PER_type1:			//�����P
		ret=ppd->type1;
		break;
	case ID_PER_type2:			//�����Q
		ret=ppd->type2;
		break;
	case ID_PER_get_rate:		//�ߊl��
		ret=ppd->get_rate;
		break;
	case ID_PER_give_exp:		//���^�o���l
		ret=ppd->give_exp;
		break;
	case ID_PER_pains_hp:		//���^�w�͒l�g�o
		ret=ppd->pains_hp;
		break;
	case ID_PER_pains_pow:		//���^�w�͒l�U����
		ret=ppd->pains_pow;
		break;
	case ID_PER_pains_def:		//���^�w�͒l�h���
		ret=ppd->pains_def;
		break;
	case ID_PER_pains_agi:		//���^�w�͒l�f����
		ret=ppd->pains_agi;
		break;
	case ID_PER_pains_spepow:	//���^�w�͒l����U����
		ret=ppd->pains_spepow;
		break;
	case ID_PER_pains_spedef:	//���^�w�͒l����h���
		ret=ppd->pains_spedef;
		break;
	case ID_PER_item1:			//�A�C�e���P
		ret=ppd->item1;
		break;
	case ID_PER_item2:			//�A�C�e���Q
		ret=ppd->item2;
		break;
	case ID_PER_sex:			//���ʃx�N�g��
		ret=ppd->sex;
		break;
	case ID_PER_egg_birth:		//�^�}�S�̛z������
		ret=ppd->egg_birth;
		break;
	case ID_PER_friend:			//�Ȃ��x�����l
		ret=ppd->_friend;
		break;
	case ID_PER_grow:			//�����Ȑ�����
		ret=ppd->grow;
		break;
	case ID_PER_egg_group1:		//���Â���O���[�v1
		ret=ppd->egg_group1;
		break;
	case ID_PER_egg_group2:		//���Â���O���[�v2
		ret=ppd->egg_group2;
		break;
	case ID_PER_speabi1:		//����\�͂P
		ret=ppd->speabi1;
		break;
	case ID_PER_speabi2:		//����\�͂Q
		ret=ppd->speabi2;
		break;
	case ID_PER_escape:			//�����闦
		ret=ppd->escape;
		break;
	case ID_PER_color:			//�F�i�}�ӂŎg�p�j
		ret=ppd->color;
		break;
	case ID_PER_reverse:		//���]�t���O
		ret=ppd->reverse;
		break;
	case ID_PER_machine1:		//�Z�}�V���t���O�P
		ret=ppd->machine1;
		break;
	case ID_PER_machine2:		//�Z�}�V���t���O�Q
		ret=ppd->machine2;
		break;
	case ID_PER_machine3:		//�Z�}�V���t���O�R
		ret=ppd->machine3;
		break;
	case ID_PER_machine4:		//�Z�}�V���t���O�S
		ret=ppd->machine4;
		break;
	}
	return ret;
}

/**
 *	�|�P���������e�[�u���f�[�^���擾
 *
 * @param[in]	para		�擾���鐬���e�[�u���̃C���f�b�N�X�i0�`7�j
 * @param[out]	grow_tbl	�擾���������e�[�u���̊i�[��
 */
static void PokeGrowDataGet(int para, u32 *grow_tbl)
{
	DPW_TASSERTMSG(para < 8, "PokeGrowDataGet:TableIndexOver!");
	ArchiveDataLoad(grow_tbl, "growtbl.narc", para);
}

/**
 *	�|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[
 *
 * @param[in]	pp	�i�[�������|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	id	�i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in]	buf	�i�[�������f�[�^�̃|�C���^
 */
static void PokeParaPutAct(POKEMON_PARAM *pp, int id, const void *buf)
{
	u32	*buf32=(u32 *)buf;
	u16	*buf16=(u16 *)buf;
	u8	*buf8=(u8 *)buf;

	switch(id){
	case ID_PARA_condition:
		pp->pcp.condition=buf32[0];
		break;
	case ID_PARA_level:
		pp->pcp.level=buf8[0];
		break;
	case ID_PARA_cb_id:
		pp->pcp.cb_id=buf8[0];	
		break;
	case ID_PARA_hp:
		pp->pcp.hp=buf16[0];
		break;
	case ID_PARA_hpmax:
		pp->pcp.hpmax=buf16[0];
		break;
	case ID_PARA_pow:
		pp->pcp.pow=buf16[0];
		break;
	case ID_PARA_def:
		pp->pcp.def=buf16[0];
		break;
	case ID_PARA_agi:
		pp->pcp.agi=buf16[0];
		break;
	case ID_PARA_spepow:
		pp->pcp.spepow=buf16[0];
		break;
	case ID_PARA_spedef:
		pp->pcp.spedef=buf16[0];
		break;
	case ID_PARA_mail_data:
		memcpy(&pp->pcp.mail_data, buf, sizeof(MAIL_DATA));
		break;
	case ID_PARA_cb_core:
		memcpy(&pp->pcp.cb_core, buf, sizeof(CB_CORE));
		break;
	default:
		PokePasoParaPutAct((POKEMON_PASO_PARAM *)&pp->ppp,id,buf);
		break;
	}
}

/**
 *	�{�b�N�X�|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[
 *
 * @param[in]	ppp	�i�[�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]	id	�i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in]	buf	�i�[�������f�[�^�̃|�C���^
 */
static void PokePasoParaPutAct(POKEMON_PASO_PARAM *ppp, int id, const void *buf)
{
	int	i;
	u64	bit;
	u32	*buf32=(u32 *)buf;
	u16	*buf16=(u16 *)buf;
	u8	*buf8=(u8 *)buf;
	POKEMON_PASO_PARAM1	*ppp1;
	POKEMON_PASO_PARAM2	*ppp2;
	POKEMON_PASO_PARAM3	*ppp3;
	POKEMON_PASO_PARAM4	*ppp4;

	ppp1=(POKEMON_PASO_PARAM1 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA1);
	ppp2=(POKEMON_PASO_PARAM2 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA2);
	ppp3=(POKEMON_PASO_PARAM3 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA3);
	ppp4=(POKEMON_PASO_PARAM4 *)PokeParaAdrsGet(ppp,ppp->personal_rnd,ID_POKEPARA4);

	switch(id){
//PARAM
		case ID_PARA_personal_rnd:
			ppp->personal_rnd=buf32[0];
			break;
		case ID_PARA_pp_fast_mode:
			DPW_TASSERTMSG(FALSE, "fast_mode�֕s���ȏ�������\n");
			ppp->pp_fast_mode=buf8[0];
			break;
		case ID_PARA_ppp_fast_mode:
			DPW_TASSERTMSG(FALSE, "fast_mode�֕s���ȏ�������\n");
			ppp->ppp_fast_mode=buf8[0];
			break;
		case ID_PARA_checksum:
			ppp->checksum=buf16[0];
			break;
//PARAM1
		case ID_PARA_monsno:
			ppp1->monsno=buf16[0];
			break;
		case ID_PARA_item:
			ppp1->item=buf16[0];
			break;
		case ID_PARA_id_no:
			ppp1->id_no=buf32[0];
			break;
		case ID_PARA_exp:
			ppp1->exp=buf32[0];
			break;
		case ID_PARA_friend:
			ppp1->_friend=buf8[0];
			break;
		case ID_PARA_speabino:
			ppp1->speabino=buf8[0];
			break;
		case ID_PARA_mark:
			ppp1->mark=buf8[0];		
			break;
		case ID_PARA_country_code:
			ppp1->country_code=buf8[0];		
			break;
		case ID_PARA_hp_exp:
			ppp1->hp_exp=buf8[0];
			break;
		case ID_PARA_pow_exp:
			ppp1->pow_exp=buf8[0];
			break;
		case ID_PARA_def_exp:
			ppp1->def_exp=buf8[0];
			break;
		case ID_PARA_agi_exp:
			ppp1->agi_exp=buf8[0];
			break;
		case ID_PARA_spepow_exp:
			ppp1->spepow_exp=buf8[0];
			break;
		case ID_PARA_spedef_exp:
			ppp1->spedef_exp=buf8[0];
			break;
		case ID_PARA_style:
			ppp1->style=buf8[0];
			break;
		case ID_PARA_beautiful:
			ppp1->beautiful=buf8[0];
			break;
		case ID_PARA_cute:
			ppp1->cute=buf8[0];
			break;
		case ID_PARA_clever:
			ppp1->clever=buf8[0];
			break;
		case ID_PARA_strong:
			ppp1->strong=buf8[0];	
			break;
		case ID_PARA_fur:
			ppp1->fur=buf8[0];		
			break;
		case ID_PARA_sinou_champ_ribbon:				//�V���I�E�`�����v���{��
		case ID_PARA_sinou_battle_tower_ttwin_first:	//�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
		case ID_PARA_sinou_battle_tower_ttwin_second:	//�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
		case ID_PARA_sinou_battle_tower_2vs2_win50:		//�V���I�E�o�g���^���[�^���[�_�u��50�A��
		case ID_PARA_sinou_battle_tower_aimulti_win50:	//�V���I�E�o�g���^���[�^���[AI�}���`50�A��
		case ID_PARA_sinou_battle_tower_siomulti_win50:	//�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
		case ID_PARA_sinou_battle_tower_wifi_rank5:		//�V���I�E�o�g���^���[Wifi�����N�T����
		case ID_PARA_sinou_syakki_ribbon:				//�V���I�E����������{��
		case ID_PARA_sinou_dokki_ribbon:				//�V���I�E�ǂ������{��
		case ID_PARA_sinou_syonbo_ribbon:				//�V���I�E�����ڃ��{��
		case ID_PARA_sinou_ukka_ribbon:					//�V���I�E���������{��
		case ID_PARA_sinou_sukki_ribbon:				//�V���I�E���������{��
		case ID_PARA_sinou_gussu_ribbon:				//�V���I�E���������{��
		case ID_PARA_sinou_nikko_ribbon:				//�V���I�E�ɂ������{��
		case ID_PARA_sinou_gorgeous_ribbon:				//�V���I�E�S�[�W���X���{��
		case ID_PARA_sinou_royal_ribbon:				//�V���I�E���C�������{��
		case ID_PARA_sinou_gorgeousroyal_ribbon:		//�V���I�E�S�[�W���X���C�������{��
		case ID_PARA_sinou_ashiato_ribbon:				//�V���I�E�������ƃ��{��
		case ID_PARA_sinou_record_ribbon:				//�V���I�E���R�[�h���{��
		case ID_PARA_sinou_history_ribbon:				//�V���I�E�q�X�g���[���{��
		case ID_PARA_sinou_legend_ribbon:				//�V���I�E���W�F���h���{��
		case ID_PARA_sinou_red_ribbon:					//�V���I�E���b�h���{��
		case ID_PARA_sinou_green_ribbon:				//�V���I�E�O���[�����{��
		case ID_PARA_sinou_blue_ribbon:					//�V���I�E�u���[���{��
		case ID_PARA_sinou_festival_ribbon:				//�V���I�E�t�F�X�e�B�o�����{��
		case ID_PARA_sinou_carnival_ribbon:				//�V���I�E�J�[�j�o�����{��
		case ID_PARA_sinou_classic_ribbon:				//�V���I�E�N���V�b�N���{��
		case ID_PARA_sinou_premiere_ribbon:				//�V���I�E�v���~�A���{��
		case ID_PARA_sinou_amari_ribbon:				//���܂�
			bit=buf8[0];
			bit=bit<<(id-ID_PARA_sinou_champ_ribbon);
			if(buf8[0]){
				ppp1->sinou_ribbon|=bit;
			}
			else{
				ppp1->sinou_ribbon&=(bit^0xffffffff);
			}
			break;
//PARAM2
		case ID_PARA_waza1:
		case ID_PARA_waza2:
		case ID_PARA_waza3:
		case ID_PARA_waza4:
			ppp2->waza[id-ID_PARA_waza1]=buf16[0];
			break;
		case ID_PARA_pp1:
		case ID_PARA_pp2:
		case ID_PARA_pp3:
		case ID_PARA_pp4:
			ppp2->pp[id-ID_PARA_pp1]=buf8[0];
			break;
		case ID_PARA_pp_count1:
		case ID_PARA_pp_count2:
		case ID_PARA_pp_count3:
		case ID_PARA_pp_count4:
			ppp2->pp_count[id-ID_PARA_pp_count1]=buf8[0];
			break;
		case ID_PARA_pp_max1:
		case ID_PARA_pp_max2:
		case ID_PARA_pp_max3:
		case ID_PARA_pp_max4:
			DPW_TASSERTMSG(FALSE, "�s���ȏ�������\n");
			break;
		case ID_PARA_hp_rnd:
			ppp2->hp_rnd=buf8[0];
			break;
		case ID_PARA_pow_rnd:
			ppp2->pow_rnd=buf8[0];
			break;
		case ID_PARA_def_rnd:
			ppp2->def_rnd=buf8[0];
			break;
		case ID_PARA_agi_rnd:
			ppp2->agi_rnd=buf8[0];
			break;
		case ID_PARA_spepow_rnd:
			ppp2->spepow_rnd=buf8[0];
			break;
		case ID_PARA_spedef_rnd:
			ppp2->spedef_rnd=buf8[0];
			break;
		case ID_PARA_tamago_flag:
			ppp2->tamago_flag=buf8[0];
			break;
		case ID_PARA_nickname_flag:
			ppp2->nickname_flag=buf8[0];
			break;
		case ID_PARA_stylemedal_normal:
		case ID_PARA_stylemedal_super:
		case ID_PARA_stylemedal_hyper:
		case ID_PARA_stylemedal_master:
		case ID_PARA_beautifulmedal_normal:
		case ID_PARA_beautifulmedal_super:
		case ID_PARA_beautifulmedal_hyper:
		case ID_PARA_beautifulmedal_master:
		case ID_PARA_cutemedal_normal:
		case ID_PARA_cutemedal_super:
		case ID_PARA_cutemedal_hyper:
		case ID_PARA_cutemedal_master:
		case ID_PARA_clevermedal_normal:
		case ID_PARA_clevermedal_super:
		case ID_PARA_clevermedal_hyper:
		case ID_PARA_clevermedal_master:
		case ID_PARA_strongmedal_normal:
		case ID_PARA_strongmedal_super:
		case ID_PARA_strongmedal_hyper:
		case ID_PARA_strongmedal_master:
		case ID_PARA_champ_ribbon:
		case ID_PARA_winning_ribbon:
		case ID_PARA_victory_ribbon:
		case ID_PARA_bromide_ribbon:
		case ID_PARA_ganba_ribbon:
		case ID_PARA_marine_ribbon:
		case ID_PARA_land_ribbon:
		case ID_PARA_sky_ribbon:
		case ID_PARA_country_ribbon:
		case ID_PARA_national_ribbon:
		case ID_PARA_earth_ribbon:
		case ID_PARA_world_ribbon:
			bit=buf8[0];
			bit=bit<<(id-ID_PARA_stylemedal_normal);
			if(buf8[0]){
				ppp2->old_ribbon|=bit;
			}
			else{
				ppp2->old_ribbon&=(bit^0xffffffff);
			}
			break;
		case ID_PARA_event_get_flag:
			ppp2->event_get_flag=buf8[0];
			break;
		case ID_PARA_sex:
			ppp2->sex=buf8[0];
			break;
		case ID_PARA_form_no:
			ppp2->form_no=buf8[0];
			break;
		case ID_PARA_dummy_p2_1:
			ppp2->dummy_p2_1=buf8[0];
			break;
		case ID_PARA_dummy_p2_2:
			ppp2->dummy_p2_2=buf16[0];
			break;
//PARAM3
		case ID_PARA_nickname:
			for(i=0;i<NELEMS(ppp3->nickname);i++){
				ppp3->nickname[i]=buf16[i];
			}
			break;
		case ID_PARA_nickname_buf_flag:
			ppp2->nickname_flag=1;
		case ID_PARA_nickname_buf:
			DPW_TASSERTMSG(FALSE, "put ID_PARA_nickname_buf is not implemented\n");
//			STRBUF_GetStringCode((STRBUF*)buf, ppp3->nickname, NELEMS(ppp3->nickname));
			break;
		case ID_PARA_pref_code:
			ppp3->pref_code=buf8[0];
			break;
		case ID_PARA_get_cassette:
			ppp3->get_cassette=buf8[0];
			break;
		case ID_PARA_trial_stylemedal_normal:
		case ID_PARA_trial_stylemedal_super:
		case ID_PARA_trial_stylemedal_hyper:
		case ID_PARA_trial_stylemedal_master:
		case ID_PARA_trial_beautifulmedal_normal:
		case ID_PARA_trial_beautifulmedal_super:
		case ID_PARA_trial_beautifulmedal_hyper:
		case ID_PARA_trial_beautifulmedal_master:
		case ID_PARA_trial_cutemedal_normal:
		case ID_PARA_trial_cutemedal_super:
		case ID_PARA_trial_cutemedal_hyper:
		case ID_PARA_trial_cutemedal_master:
		case ID_PARA_trial_clevermedal_normal:
		case ID_PARA_trial_clevermedal_super:
		case ID_PARA_trial_clevermedal_hyper:
		case ID_PARA_trial_clevermedal_master:
		case ID_PARA_trial_strongmedal_normal:
		case ID_PARA_trial_strongmedal_super:
		case ID_PARA_trial_strongmedal_hyper:
		case ID_PARA_trial_strongmedal_master:
		case ID_PARA_amari_ribbon:
			bit=buf8[0];
			bit=bit<<(id-ID_PARA_trial_stylemedal_normal);
			if(buf8[0]){
				ppp3->new_ribbon|=bit;
			}
			else{
				ppp3->new_ribbon&=~bit;
			}
			break;
//PARAM4
		case ID_PARA_oyaname:
			for(i=0;i<NELEMS(ppp4->oyaname);i++)
				ppp4->oyaname[i]=buf16[i];
			break;
		case ID_PARA_oyaname_buf:
			DPW_TASSERTMSG(FALSE, "put ID_PARA_oyaname_buf is not implemented\n");
//			STRBUF_GetStringCode((STRBUF*)buf, ppp4->oyaname, NELEMS(ppp4->oyaname));
			break;
		case ID_PARA_get_year:							//�߂܂����N
			ppp4->get_year=buf8[0];						//�߂܂����N
			break;
		case ID_PARA_get_month:							//�߂܂�����
			ppp4->get_month=buf8[0];					//�߂܂�����
			break;
		case ID_PARA_get_day:							//�߂܂�����
			ppp4->get_day=buf8[0];						//�߂܂�����
			break;
		case ID_PARA_birth_year:						//���܂ꂽ�N
			ppp4->birth_year=buf8[0];					//���܂ꂽ�N
			break;
		case ID_PARA_birth_month:						//���܂ꂽ��
			ppp4->birth_month=buf8[0];					//���܂ꂽ��
			break;
		case ID_PARA_birth_day:							//���܂ꂽ��
			ppp4->birth_day=buf8[0];					//���܂ꂽ��
			break;
		case ID_PARA_get_place:							//�߂܂����ꏊ
			ppp4->get_place=buf16[0];					//�߂܂����ꏊ
			break;
		case ID_PARA_birth_place:						//���܂ꂽ�ꏊ
			ppp4->birth_place=buf16[0];					//���܂ꂽ�ꏊ
			break;
		case ID_PARA_pokerus:							//�|�P���X
			ppp4->pokerus=buf8[0];						//�|�P���X
			break;
		case ID_PARA_get_ball:							//�߂܂����{�[��
			ppp4->get_ball=buf8[0];						//�߂܂����{�[��
			break;
		case ID_PARA_get_level:							//�߂܂������x��
			ppp4->get_level=buf8[0];					//�߂܂������x��
			break;
		case ID_PARA_oyasex:							//�e�̐���
			ppp4->oyasex=buf8[0];						//�e�̐���
			break;
		case ID_PARA_get_ground_id:						//�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
			ppp4->get_ground_id=buf8[0];				//�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
			break;
		case ID_PARA_dummy_p4_1:						//���܂�
			ppp4->dummy_p4_1=buf16[0];					//���܂�
			break;

		case ID_PARA_power_rnd:
			ppp2->hp_rnd	=(buf32[0]>> 0)&0x0000001f;
			ppp2->pow_rnd	=(buf32[0]>> 5)&0x0000001f;
			ppp2->def_rnd	=(buf32[0]>>10)&0x0000001f;
			ppp2->agi_rnd	=(buf32[0]>>15)&0x0000001f;
			ppp2->spepow_rnd=(buf32[0]>>20)&0x0000001f;
			ppp2->spedef_rnd=(buf32[0]>>25)&0x0000001f;
			break;
		//�����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N
		case ID_PARA_nidoran_nickname:
		case ID_PARA_type1:
		case ID_PARA_type2:
			DPW_TASSERTMSG(FALSE, "�s���ȏ�������\n");
			break;
		case ID_PARA_default_name:						//�|�P�����̃f�t�H���g��
			DPW_TASSERTMSG(FALSE, "put ID_PARA_default_name is not implemented\n");
//			{
//				STRBUF	*name_buf;
//
//				name_buf=MSGDAT_UTIL_GetMonsName(ppp1->monsno, HEAPID_BASE_SYSTEM);
//				STRBUF_GetStringCode(name_buf, ppp3->nickname, NELEMS(ppp3->nickname));
//				STRBUF_Delete(name_buf);
//			}
			break;
	}
}

//==============================================================================
/**
 * @brief   �h�c�ƌ��������烌�A���ǂ����𔻒肷��
 *
 * @param   id		ID
 * @param   rnd		������
 *
 * @retval  u8		0:���A����Ȃ�	0�ȊO:���A
 */
//==============================================================================
static u8 PokeRareGetPara(u32 id,u32 rnd)
{
	return((((id&0xffff0000)>>16)^(id&0xffff)^((rnd&0xffff0000)>>16)^(rnd&0xffff))<8);
}
