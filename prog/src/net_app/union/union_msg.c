//==============================================================================
/**
 * @file    union_msg.c
 * @brief   ���j�I�����[���F�c�[����(�I�[�o�[���C�̈�ɔz�u����܂�)
 * @author  matsuda
 * @date    2009.07.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "net_app/union/union_msg.h"
#include "message.naix"
#include "msg/msg_union_room.h"
#include "system/main.h"
#include "savedata/wifihistory.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���M�����[�V�����F����
#define UNION_BATTLE_REGULATION_DECIDE    (100)
///���M�����[�V�����F���[�����݂�
#define UNION_BATTLE_REGULATION_RULE      (150)

///���M�����[�V�����F�ΐ�`��
enum{
  UNION_BATTLE_REGULATION_MODE_SINGLE,      ///<�V���O��
  UNION_BATTLE_REGULATION_MODE_DOUBLE,      ///<�_�u��
  UNION_BATTLE_REGULATION_MODE_TRIPLE,      ///<�g���v��
  UNION_BATTLE_REGULATION_MODE_ROTATION,    ///<���[�e�[�V����
  UNION_BATTLE_REGULATION_MODE_MULTI,       ///<�}���`
  
  UNION_BATTLE_REGULATION_MODE_MAX,
};

///���M�����[�V�����F���[��
enum{
  UNION_BATTLE_REGULATION_RULE_FREE,        ///<��������
  UNION_BATTLE_REGULATION_RULE_FLAT,    ///<�t���b�g
  
  UNION_BATTLE_REGULATION_RULE_MAX,
};

///���M�����[�V�����F�V���[�^�[
enum{
  UNION_BATTLE_REGULATION_SHOOTER_TRUE,     ///<�V���[�^�[�g�p����
  UNION_BATTLE_REGULATION_SHOOTER_FALSE,    ///<�V���[�^�[�֎~
  
  UNION_BATTLE_REGULATION_SHOOTER_MAX,
};

///���M�����[�V�����F�m�[�}���F
#define REG_NORMAL_COLOR      PRINTSYS_MACRO_LSB(1, 2, 0)
///���M�����[�V�����FNG�F
#define REG_FAIL_COLOR        PRINTSYS_MACRO_LSB(4, 2, 0)

///union_msg��ł�WORDSET��buf_id�Ǘ��ԍ�
enum{
  _WORDSET_BUFID_TARGET_NAME,     ///<�ڑ�����̖��O
  _WORDSET_BUFID_MINE_NAME,       ///<�����̖��O
  _WORDSET_BUFID_TARGET_AISATSU,  ///<����ɍ��킹�����A
  _WORDSET_BUFID_NATION,          ///<��
  _WORDSET_BUFID_AREA,            ///<�n��
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void _SetPmsAisatsu(GAMEDATA * gamedata, u8 region);
static void UnionMsg_Menu_WindowDel(UNION_SYSTEM_PTR unisys);

//==============================================================================
//  �v���g�^�C�v�錾�F�f�[�^
//==============================================================================
static const FLDMENUFUNC_LIST BattleMenuList_Number[3];
static const FLDMENUFUNC_LIST BattleMenuList_Custom[6];
static const FLDMENUFUNC_LIST BattleMenuList_Mode[5];
static const FLDMENUFUNC_LIST BattleMenuList_Rule[3];
static const FLDMENUFUNC_LIST BattleMenuList_Shooter[3];
static const FLDMENUFUNC_LIST BattleMenuList_CustomMulti[5];
static const FLDMENUFUNC_LIST BattleMenuList_RuleMulti[3];
static const FLDMENUFUNC_LIST BattleMenuList_ShooterMulti[3];


//==============================================================================
//  �f�[�^
//==============================================================================
//--------------------------------------------------------------
//  �e�Q�[���̍ő�Q���l��
//--------------------------------------------------------------
static const struct{
  u8 member_max;
  u8 padding[3];
}MenuMemberMax[] = {
  {2},      //UNION_PLAY_CATEGORY_UNION,          ///<���j�I�����[��
  {2},      //UNION_PLAY_CATEGORY_TALK,           ///<��b��
  {2},      //UNION_PLAY_CATEGORY_TRAINERCARD,    ///<�g���[�i�[�J�[�h
  {4},      //UNION_PLAY_CATEGORY_PICTURE,        ///<���G����
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE,       ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER,   ///<�R���V�A��
  {2},      //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT,   ///<�R���V�A��
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER,
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE,
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER,
  {4},      //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT,
  {2},      //UNION_PLAY_CATEGORY_TRADE,          ///<�|�P��������
  {4},      //UNION_PLAY_CATEGORY_GURUGURU,       ///<���邮�����
  {5},      //UNION_PLAY_CATEGORY_RECORD,         ///<���R�[�h�R�[�i�[
};

//--------------------------------------------------------------
//  ���j���[�w�b�_�[
//--------------------------------------------------------------
///���C�����j���[���X�g
static const FLDMENUFUNC_LIST MainMenuList[] =
{
  {msg_union_select_01, (void*)UNION_PLAY_CATEGORY_TRAINERCARD},   //���A
  {msg_union_select_03, (void*)UNION_MENU_NO_SEND_BATTLE},   //�ΐ�
  {msg_union_select_04, (void*)UNION_PLAY_CATEGORY_TRADE},   //����
  {msg_union_select_02, (void*)UNION_PLAY_CATEGORY_PICTURE},   //���G����
  {msg_union_select_06, (void*)UNION_PLAY_CATEGORY_GURUGURU}, //���邮�����
  {msg_union_select_05, (void*)UNION_MENU_SELECT_CANCEL},   //��߂�
};

///���j���[�w�b�_�[(���C�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_MainMenu =
{
	6,		//���X�g���ڐ�
	6,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	0,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	1,//32-14,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	13,		//�\���T�C�YX �L�����P��
	12,		//�\���T�C�YY �L�����P��
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);

//--------------------------------------------------------------
//  �ΐ탁�j���[
//--------------------------------------------------------------
///��l�ΐ�A�l�l�ΐ�A��߂�
static const FLDMENUFUNC_LIST BattleMenuList_Number[] =
{
  {msg_union_battle_01_01, (void*)BattleMenuList_Custom},   //��l�ΐ�
  {msg_union_battle_01_02, (void*)BattleMenuList_CustomMulti},    //�l�l�ΐ�
  {msg_union_battle_01_03, (void*)FLDMENUFUNC_CANCEL},    //��߂�
};

///��l�ΐ�F�J�X�^�����j���[
static const FLDMENUFUNC_LIST BattleMenuList_Custom[] =
{
  {msg_union_battle_01_04, (void*)BattleMenuList_Mode},      //�ΐ�`��(�V���O���A�_�u���c)
  {msg_union_battle_01_05, (void*)BattleMenuList_Rule},       //���[��(���x��50�A���������c)
  {msg_union_battle_01_06, (void*)BattleMenuList_Shooter},   //�V���[�^�[�L��
  {msg_union_battle_01_07_01, (void*)UNION_BATTLE_REGULATION_DECIDE},    //����
  {msg_union_battle_01_07_02, (void*)UNION_BATTLE_REGULATION_RULE},    //���[�����݂�
  {msg_union_battle_01_08, (void*)BattleMenuList_Number},    //���ǂ�
};

///��l�ΐ�F�V���O���A�_�u���A�g���v���A���[�e�[�V�����A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_Mode[] =
{
  {msg_union_battle_01_04, (void*)UNION_BATTLE_REGULATION_MODE_SINGLE},   //�V���O��
  {msg_union_battle_01_05, (void*)UNION_BATTLE_REGULATION_MODE_DOUBLE},   //�_�u��
  {msg_union_battle_01_06, (void*)UNION_BATTLE_REGULATION_MODE_TRIPLE},   //�g���v��
  {msg_union_battle_01_07, (void*)UNION_BATTLE_REGULATION_MODE_ROTATION}, //���[�e�[�V����
  {msg_union_battle_01_08, (void*)BattleMenuList_Custom},   //���ǂ�
};

///��l�ΐ�F�����Ȃ��A�t���b�g�A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_Rule[] =
{
  {msg_union_battle_01_10, (void*)UNION_BATTLE_REGULATION_RULE_FREE},     //�����Ȃ�
  {msg_union_battle_01_11, (void*)UNION_BATTLE_REGULATION_RULE_FLAT}, //�t���b�g
  {msg_union_battle_01_12, (void*)BattleMenuList_Custom},   //���ǂ�
};

///��l�ΐ�F�V���[�^�[����A�V���[�^�[����
static const FLDMENUFUNC_LIST BattleMenuList_Shooter[] =
{
  {msg_union_battle_01_11_01, (void*)UNION_BATTLE_REGULATION_SHOOTER_TRUE}, //�L
  {msg_union_battle_01_11_02, (void*)UNION_BATTLE_REGULATION_SHOOTER_FALSE}, //��
  {msg_union_battle_01_12,    (void*)BattleMenuList_Custom},   //���ǂ�
};

///�}���`�F�J�X�^�����j���[
static const FLDMENUFUNC_LIST BattleMenuList_CustomMulti[] =
{
  {msg_union_battle_01_05, (void*)BattleMenuList_RuleMulti},       //���[��(���x��50�A���������c)
  {msg_union_battle_01_06, (void*)BattleMenuList_ShooterMulti},   //�V���[�^�[�L��
  {msg_union_battle_01_07_01, (void*)UNION_BATTLE_REGULATION_DECIDE},    //����
  {msg_union_battle_01_07_02, (void*)UNION_BATTLE_REGULATION_RULE},    //���[�����݂�
  {msg_union_battle_01_08, (void*)BattleMenuList_Number},    //���ǂ�
};

///�}���`�F�����Ȃ��A�t���b�g�A���ǂ�
static const FLDMENUFUNC_LIST BattleMenuList_RuleMulti[] =
{
  {msg_union_battle_01_10, (void*)UNION_BATTLE_REGULATION_RULE_FREE},     //�����Ȃ�
  {msg_union_battle_01_11, (void*)UNION_BATTLE_REGULATION_RULE_FLAT}, //�t���b�g
  {msg_union_battle_01_12, (void*)BattleMenuList_CustomMulti},   //���ǂ�
};

///�}���`�F�V���[�^�[����A�V���[�^�[����
static const FLDMENUFUNC_LIST BattleMenuList_ShooterMulti[] =
{
  {msg_union_battle_01_11_01, (void*)UNION_BATTLE_REGULATION_SHOOTER_TRUE}, //�L
  {msg_union_battle_01_11_02, (void*)UNION_BATTLE_REGULATION_SHOOTER_FALSE}, //��
  {msg_union_battle_01_12,    (void*)BattleMenuList_CustomMulti},   //���ǂ�
};

///�ΐ탁�j���[�̃f�[�^�e�[�u�� ��BATTLE_MENU_INDEX_???�ƕ��т𓯂��ɂ��Ă������ƁI
static const struct{
  const FLDMENUFUNC_LIST *list;
  u8 list_max;
}BattleMenuDataTbl[] = {
  {
    BattleMenuList_Number,
    3,
  },
  {
    BattleMenuList_Custom,
    6,
  },
  {
    BattleMenuList_Mode,
    5,
  },
  {
    BattleMenuList_Rule,
    3,
  },
  {
    BattleMenuList_Shooter,
    3,
  },
  
  //�}���`�p���j���[
  {
    BattleMenuList_CustomMulti,
    5,
  },
  {
    BattleMenuList_RuleMulti,
    3,
  },
  {
    BattleMenuList_ShooterMulti,
    3,
  },
};

///���j���[�w�b�_�[(�ΐ탁�j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_Battle =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	0,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	1,//32-20,		//�\�����WX �L�����P��
	1,		//�\�����WY �L�����P��
	19,		//�\���T�C�YX �L�����P��
	14,		//�\���T�C�YY �L�����P��
};
//SDK_COMPILER_ASSERT(NELEMS(MainMenuList) == MenuHeader_MainMenu.line);

//--------------------------------------------------------------
//  POKEPARTY�I�����j���[�w�b�_�[
//--------------------------------------------------------------
///POKEPARTY�I�����j���[���X�g
static const FLDMENUFUNC_LIST PokePartySelectMenuList[] =
{
  {msg_union_battle_01_23, (void*)COLOSSEUM_SELECT_PARTY_TEMOTI},  //�莝��
  {msg_union_battle_01_24, (void*)COLOSSEUM_SELECT_PARTY_BOX},     //�o�g���{�b�N�X
  {msg_union_battle_01_25, (void*)COLOSSEUM_SELECT_PARTY_CANCEL},  //��߂�
};

///���j���[�w�b�_�[(POKEPARTY�I�����j���[�p)
static const FLDMENUFUNC_HEADER MenuHeader_PokePartySelect =
{
	3,		//���X�g���ڐ�
	3,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
	0,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_LRKEY,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	16,		//�����T�C�YY(�h�b�g
	32-14,		//�\�����WX �L�����P��
	24-6-7,		//�\�����WY �L�����P��
	13,		//�\���T�C�YX �L�����P��
	6,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
///  ���j���[�I�����M�����[�V�������Q�[���J�e�S���[�ɕϊ�����e�[�u��
//--------------------------------------------------------------
ALIGN4 static const u8 MenuReg_to_PlayCategory[UNION_BATTLE_REGULATION_MODE_MAX][UNION_BATTLE_REGULATION_RULE_MAX][UNION_BATTLE_REGULATION_SHOOTER_MAX] = {
  {//UNION_BATTLE_REGULATION_MODE_SINGLE
    {//UNION_BATTLE_REGULATION_RULE_FREE
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,
    },
    {//UNION_BATTLE_REGULATION_RULE_FLAT
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT,
    },
  },
  {//UNION_BATTLE_REGULATION_MODE_DOUBLE
    {//UNION_BATTLE_REGULATION_RULE_FREE
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,
    },
    {//UNION_BATTLE_REGULATION_RULE_FLAT
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT,
    },
  },
  {//UNION_BATTLE_REGULATION_MODE_TRIPLE
    {//UNION_BATTLE_REGULATION_RULE_FREE
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE,
    },
    {//UNION_BATTLE_REGULATION_RULE_FLAT
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT,
    },
  },
  {//UNION_BATTLE_REGULATION_MODE_ROTATION
    {//UNION_BATTLE_REGULATION_RULE_FREE
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE,
    },
    {//UNION_BATTLE_REGULATION_RULE_FLAT
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT,
    },
  },
  {//UNION_BATTLE_REGULATION_MODE_MULTI
    {//UNION_BATTLE_REGULATION_RULE_FREE
      UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE,
    },
    {//UNION_BATTLE_REGULATION_RULE_FLAT
      UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER,
      UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT,
    },
  },
};


//--------------------------------------------------------------
//  
//--------------------------------------------------------------
#define TALK_CARD_MAX		( 2 )
#define TALK_TRADE_MAX		( 2 )
#define TALK_FIGHT_MAX    (4)
#define TALK_BATTLE_MAX		( 4 )
#define TALK_OEKAKI_MAX		( 4 )
#define TALK_RECORD_MAX		( 4 )
#define TALK_GURUGURU_MAX	( 4 )

///���A����OBJ�ɘb�����������̃����_�����b�Z�[�W(�g���[�i�[�J�[�h��)
ALIGN4 static const u16 talk_table_card[TALK_CARD_MAX][2]={
	{	msg_union_talkboy_01_03,msg_union_talkgirl_01_03},
	{	msg_union_talkboy_01_04,msg_union_talkgirl_01_04},
};
///���G��������OBJ�ɘb�����������̃����_�����b�Z�[�W
ALIGN4 static const u16 talk_table_oekaki[TALK_OEKAKI_MAX][2]={
	{	msg_union_talkboy_02_09,msg_union_talkgirl_02_09},
	{	msg_union_talkboy_02_10,msg_union_talkgirl_02_10},
	{	msg_union_talkboy_02_11,msg_union_talkgirl_02_11},
	{	msg_union_talkboy_02_12,msg_union_talkgirl_02_12},
};
///�o�g������OBJ�ɘb�����������̃����_�����b�Z�[�W
ALIGN4 static const u16 talk_table_fight[TALK_FIGHT_MAX][2] = {
  {msg_union_talkboy_03_10, msg_union_talkgirl_03_10,},
  {msg_union_talkboy_03_11, msg_union_talkgirl_03_11},
  {msg_union_talkboy_03_12, msg_union_talkgirl_03_12},
  {msg_union_talkboy_03_13, msg_union_talkgirl_03_13},
};

//�}���`�o�g���̓r���Q���A�J�n�O�̑҂��u��[���A������Ƃ܂��ĂāI�v
ALIGN4 static const u16 talk_table_battle_wait[2]={
	msg_union_talkboy_03_06,msg_union_talkgirl_03_06,
};
//�}���`�o�g���̓r���Q���������u�܂������ˁI���ꂶ�Ⴂ������Ƀ}���`�o�g�����悤�I�v
ALIGN4 static const u16 talk_table_battle_start[2] = {
	msg_union_talkboy_03_07,msg_union_talkgirl_03_07,
};
//�}���`�o�g���̓r���Q�����s�����������u�������߂�A����ς肢�ܓs���������v
ALIGN4 static const u16 talk_table_battle_cancel[2] = {
	msg_union_talkboy_03_08,msg_union_talkgirl_03_08,
};

//���G�����̓r���Q���A�J�n�O�̑҂��u��[���A������Ƃ܂��ĂāI�v
ALIGN4 static const u16 talk_table_picture_wait[2]={
	msg_union_talkboy_02_06_01,msg_union_talkgirl_02_06_01,
};
//���G�����̓r���Q��������
ALIGN4 static const u16 talk_table_picture_start[2] = {
	msg_union_talkboy_02_07,msg_union_talkgirl_02_07,
};
//���G�����̓r���Q�����s�����������u�������߂�A����ς肢�ܓs���������v
ALIGN4 static const u16 talk_table_picture_cancel[2] = {
	msg_union_talkboy_02_08,msg_union_talkgirl_02_08,
};

//���邮������̓r���Q���A�J�n�O�̑҂��u��[���A������Ƃ܂��ĂāI�v
ALIGN4 static const u16 talk_table_guruguru_wait[2]={
	msg_union_talkboy_07_06_01,msg_union_talkgirl_07_06_01,
};
//���邮������̓r���Q��������
ALIGN4 static const u16 talk_table_guruguru_start[2] = {
	msg_union_talkboy_07_07,msg_union_talkgirl_07_07,
};
//���邮������̓r���Q�����s�����������u�������߂�A����ς肢�ܓs���������v
ALIGN4 static const u16 talk_table_guruguru_cancel[2] = {
	msg_union_talkboy_07_09,msg_union_talkgirl_07_09,
};
//�|�P������������OBJ�ɘb�����������̃����_�����b�Z�[�W
ALIGN4 static const u16 talk_table_trade[TALK_TRADE_MAX][2]={
	{	msg_union_talkboy_04_04,msg_union_talkgirl_04_04},
	{	msg_union_talkboy_04_05,msg_union_talkgirl_04_05},
};
///���邮���������OBJ�ɘb�����������̃����_�����b�Z�[�W
ALIGN4 static const u16 talk_table_guruguru[TALK_GURUGURU_MAX][2]={
	{	msg_union_talkboy_07_10,msg_union_talkgirl_07_10},
	{	msg_union_talkboy_07_11,msg_union_talkgirl_07_11},
	{	msg_union_talkboy_07_12,msg_union_talkgirl_07_12},
	{	msg_union_talkboy_07_13,msg_union_talkgirl_07_13},
};

// ���C�����j���[���荀�ڂ��e���瑗���Ă�����̓��e�\��
ALIGN4 static const u16 talk_child_mainmenu_view[] = {
  msg_union_connect_01_01, //UNION_PLAY_CATEGORY_TRAINERCARD
  msg_union_connect_02_01, //UNION_PLAY_CATEGORY_PICTURE
  msg_union_connect_03_01_02, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER
  msg_union_connect_03_01_02, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE
  msg_union_connect_03_01_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER
  msg_union_connect_03_01_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT
  msg_union_connect_03_01_05, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER
  msg_union_connect_03_01_05, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE
  msg_union_connect_03_01_06, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER
  msg_union_connect_03_01_06, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT
  msg_union_connect_03_01_08, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER
  msg_union_connect_03_01_08, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE
  msg_union_connect_03_01_09, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER
  msg_union_connect_03_01_09, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT
  msg_union_connect_03_01_11, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER
  msg_union_connect_03_01_11, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE
  msg_union_connect_03_01_12, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER
  msg_union_connect_03_01_12, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT
  msg_union_connect_03_01_13, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER
  msg_union_connect_03_01_13, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE
  msg_union_connect_03_01_14, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER
  msg_union_connect_03_01_14, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT
  msg_union_connect_04_01,    //UNION_PLAY_CATEGORY_TRADE
  msg_union_connect_06_01,    //UNION_PLAY_CATEGORY_GURUGURU
  msg_union_connect_05_01,    //UNION_PLAY_CATEGORY_RECORD
};

// �b�����������������Ƃ�
ALIGN4 static const u16 talk_start_child_table[2]={
	msg_union_talkboy_00_02,msg_union_talkgirl_00_02  //�u�������ł��I�@�Ȃɂ����邩���H�v
};
// �b�������������ς������Ƃ�(���C�����j���[�I�𒆂̑���ɘb��������)
ALIGN4 static const u16 talk_failed_child_table[2]={
	msg_union_talkboy_00_05,msg_union_talkgirl_00_05
};
// �b�������^�C���A�E�g
ALIGN4 static const u16 talk_parent_timeout[2] = {
  msg_union_talkboy_00_06, msg_union_talkgirl_00_06,
};

///��b���p�����đ����鎞�̃��b�Z�[�W�u���ɂ���������H�v
ALIGN4 static const u16 talk_continue[2] = {
  msg_union_talkboy_00_04, msg_union_talkgirl_00_04,
};

// �V�т�f��ꂽ�Ƃ�
ALIGN4 static const u16 talk_failed_table[][2]={
  {msg_union_talkboy_01_02,msg_union_talkgirl_01_02}, //UNION_PLAY_CATEGORY_TRAINERCARD
  {msg_union_talkboy_02_03,msg_union_talkgirl_02_03}, //UNION_PLAY_CATEGORY_PICTURE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER
  {msg_union_talkboy_03_03,msg_union_talkgirl_03_03}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT
  {msg_union_talkboy_04_03,msg_union_talkgirl_04_03}, //UNION_PLAY_CATEGORY_TRADE
  {msg_union_talkboy_07_03,msg_union_talkgirl_07_03,}, //UNION_PLAY_CATEGORY_GURUGURU
  {msg_union_talkboy_06_03,msg_union_talkgirl_06_03,}, //UNION_PLAY_CATEGORY_RECORD
};

// �qMSG:�V�т�f�����Ƃ�
ALIGN4 static const u16 talk_child_no_table[]={
  msg_union_connect_01_03, //UNION_PLAY_CATEGORY_TRAINERCARD
  msg_union_connect_02_03, //UNION_PLAY_CATEGORY_PICTURE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER
  msg_union_connect_03_03, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT
  msg_union_connect_04_03, //UNION_PLAY_CATEGORY_TRADE
  msg_union_connect_06_03, //UNION_PLAY_CATEGORY_GURUGURU
  msg_union_connect_06_03, //UNION_PLAY_CATEGORY_RECORD
};

///�}���`�o�g���J�n(�ŏ��̓�l)
ALIGN4 static const u16 talk_multi_start[2] = {
  msg_union_talkboy_03_02, msg_union_talkgirl_03_02,
};

// ���邮������F�莝���Ƀ_���^�}�S������ꍇ��NG���b�Z�[�W
ALIGN4 static const u16 guruguru_ng_dametamago[2] = {
  msg_union_talkboy_07_14, msg_union_talkgirl_07_14,
};

// �|�P����������I�񂾂������̎莝����1�C�������Ȃ�
ALIGN4 static const u16 talk_trade_mine_poke_ng[2] = {
	msg_union_talkboy_04_06,msg_union_talkgirl_04_06,	// 2�C�ȏ�Ă��������Ȃ��̂Ō����ł��Ȃ�
};
// ���邮�������I�񂾂��������^�}�S�������Ă��Ȃ�
ALIGN4 static const u16 talk_guruguru_mine_egg_ng[2] = {
	msg_union_talkboy_07_04,msg_union_talkgirl_07_04,
};

// ���C�����j���[�I�����ʂ𑊎�ɑ��M���̑҂�
ALIGN4 static const u16 talk_answer_wait_table[][2]={
  { msg_union_talkboy_01_01,msg_union_talkgirl_01_01,}, //UNION_PLAY_CATEGORY_TRAINERCARD
  { msg_union_talkboy_02_01,msg_union_talkgirl_02_01,}, //UNION_PLAY_CATEGORY_PICTURE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FLAT
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FLAT
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FLAT
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FLAT
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FREE
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT_SHOOTER
  { msg_union_talkboy_03_01,msg_union_talkgirl_03_01,}, //UNION_PLAY_CATEGORY_COLOSSEUM_MULTI_FLAT
  { msg_union_talkboy_04_01,msg_union_talkgirl_04_01,}, //UNION_PLAY_CATEGORY_TRADE
  { msg_union_talkboy_07_01,msg_union_talkgirl_07_01,}, //UNION_PLAY_CATEGORY_GURUGURU
  { msg_union_talkboy_07_01,msg_union_talkgirl_07_01,}, //UNION_PLAY_CATEGORY_RECORD
};

///���邮������ɓr���Q�����邩���H
ALIGN4 static const u16 talk_guruguru_intrude[2] = {
  msg_union_talkboy_07_05, msg_union_talkgirl_07_05,
};
///���G�����ɓr���Q�����邩���H
ALIGN4 static const u16 talk_picture_intrude[2] = {
  msg_union_talkboy_02_05,msg_union_talkgirl_02_05,
};
///���G�����̓r���Q����f����
ALIGN4 static const u16 talk_picture_intrude_no_table[2]={
  msg_union_talkboy_02_06,msg_union_talkgirl_02_06,
};
///���邮������̓r���Q����f����
ALIGN4 static const u16 talk_guruguru_intrude_no_table[2]={
  msg_union_talkboy_07_06,msg_union_talkgirl_07_06,
};
///�}���`�o�g���̓r���Q����f����
ALIGN4 static const u16 talk_multi_intrude_no_table[2]={
  msg_union_talkboy_03_05,msg_union_talkgirl_03_05,
};
///���邮������̓r���Q���Ń^�}�S�������Ă��Ȃ�����
ALIGN4 static const u16 talk_guruguru_intrude_no_egg[2] = {
  msg_union_talkboy_07_08, msg_union_talkgirl_07_08,
};
///���邮������̓r���Q���Ń_���^�}�S���莝���ɂ�����
ALIGN4 static const u16 talk_guruguru_intrude_dametamago[2] = {
  msg_union_talkboy_07_14, msg_union_talkgirl_07_14,
};
//�܂������������Ă����
ALIGN4 static const u16 talk_connect_cancel_table[2]={
	msg_union_talkboy_05_01,msg_union_talkgirl_05_01
};
//���I���߂�@���ɗp�������邩��܂����x��
ALIGN4 static const u16 talk_connect_canceled_table[2]={
	msg_union_talkboy_00_03,msg_union_talkgirl_00_03
};

///�}���`�o�g���ɓr���Q�����邩���H
ALIGN4 static const u16 talk_multil_intrude_table[2] = {
  msg_union_talkboy_03_04, msg_union_talkgirl_03_04,
};



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �b�����������^�C���A�E�g�ɂȂ����Ƃ�
 * @param   unisys		
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TalkTimeout(UNION_SYSTEM_PTR unisys)
{
  return talk_parent_timeout[MyStatus_GetMySex(unisys->uniparent->mystatus)];
}

//==================================================================
/**
 * �b����������
 * @param   sex		�b������������̐���
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_ParentConnectStart(int target_sex)
{
  return talk_start_child_table[target_sex];
}

//==================================================================
/**
 * �������b����߂�����s������
 * @param   target_sex		
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TalkCancel(int target_sex)
{
  return talk_connect_cancel_table[target_sex];
}

//==================================================================
/**
 * ���肩��b����߂�����s���ꂽ��
 * @param   target_sex		
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TalkCancelPartner(int target_sex)
{
  return talk_connect_canceled_table[target_sex];
}

//==================================================================
/**
 * �eMSG�F���C�����j���[�I����A�q�̕Ԏ��҂����̃��b�Z�[�W
 * @param   mainmenu_select		���C�����j���[�I������
 * @retval  u16		�\��MsgID
 */
//==================================================================
u16 UnionMsg_GetMsgID_ParentMainMenuSelectAnswerWait(u32 mainmenu_select, int target_sex)
{
  return talk_answer_wait_table[mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD][target_sex];
}

//==================================================================
/**
 * �eMSG�F���C�����j���[�I����A�q����V�т�f��ꂽ
 * @param   mainmenu_select		���C�����j���[�I������
 * @retval  u16		�\��MsgID
 */
//==================================================================
u16 UnionMsg_GetMsgID_ParentMainMenuSelectAnswerNG(u32 mainmenu_select, int target_sex)
{
  return talk_failed_table[mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD][target_sex];
}

//==================================================================
/**
 * �qMSG�F���C�����j���[�I�����e���瑗���Ă������̓��e�\��
 * @param   mainmenu_select		���C�����j���[�I������
 * @retval  u16		�\��MsgID
 */
//==================================================================
u16 UnionMsg_GetMsgID_ChildMainMenuSelectView(u32 mainmenu_select)
{
  return talk_child_mainmenu_view[mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD];
}

//==================================================================
/**
 * �qMSG�F�e���瑗���Ă������C�����j���[�̍��ڂ�f����
 * @param   mainmenu_select		���C�����j���[�I������
 * @retval  u16		�\��MsgID
 */
//==================================================================
u16 UnionMsg_GetMsgID_ChildMainMenuSelectRefuse(u32 mainmenu_select)
{
  return talk_child_no_table[mainmenu_select - UNION_PLAY_CATEGORY_TRAINERCARD];
}

//==================================================================
/**
 * ��b���p������Ƃ�
 * @param   sex		�b������������̐���
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TalkContinue(int target_sex)
{
  return talk_continue[target_sex];
}

//==================================================================
/**
 * �}���`�o�g���J�n(�ŏ��̓�l)
 * @param   target_sex		
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiStart(int target_sex)
{
  return talk_multi_start[target_sex];
}

//==================================================================
/**
 * �|�P����������I�񂾂������̎莝����1�C�������Ȃ�
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TradeMinePokeNG(int target_sex)
{
  return talk_trade_mine_poke_ng[target_sex];
}

//==================================================================
/**
 * ���邮�������I�񂾂��������^�}�S�������Ă��Ȃ�
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruMineEggNG(int target_sex)
{
  return talk_guruguru_mine_egg_ng[target_sex];
}

//==================================================================
/**
 * ���邮�������I�񂾂��������_���^�}�S�������Ă���
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruMineDametamago(UNION_SYSTEM_PTR unisys)
{
  return talk_guruguru_intrude_dametamago[MyStatus_GetMySex(unisys->uniparent->mystatus)];
}

//==================================================================
/**
 * ���邮����������悤�Ƃ������莝���Ƀ_���^�}�S���������ꍇ��NG���b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruDameTamagoNG(UNION_SYSTEM_PTR unisys)
{
  return guruguru_ng_dametamago[MyStatus_GetMySex(unisys->uniparent->mystatus)];
}

//==================================================================
/**
 * ���C�����j���[�I�𒆂̑���ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGameBattle(int target_sex)
{
  return talk_failed_child_table[target_sex];
}

//==================================================================
/**
 * �o�g�����̑���ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGameMainMenuSelect(int target_sex)
{
  return talk_table_fight[GFUser_GetPublicRand(TALK_FIGHT_MAX)][target_sex];
}

//==================================================================
/**
 * �g���[�i�[�J�[�h���Ă��鑊��ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGameTrainerCard(int target_sex)
{
  return talk_table_card[GFUser_GetPublicRand(TALK_CARD_MAX)][target_sex];
}

//==================================================================
/**
 * ���G�������Ă��鑊��ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGamePicture(int target_sex)
{
  return talk_table_oekaki[GFUser_GetPublicRand(TALK_OEKAKI_MAX)][target_sex];
}

//==================================================================
/**
 * �|�P�����������Ă��鑊��ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGameTrade(int target_sex)
{
  return talk_table_trade[GFUser_GetPublicRand(TALK_TRADE_MAX)][target_sex];
}

//==================================================================
/**
 * ���邮��������Ă��鑊��ɘb�����������̃����_�����b�Z�[�W
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PlayGameGuruguru(int target_sex)
{
  return talk_table_guruguru[GFUser_GetPublicRand(TALK_GURUGURU_MAX)][target_sex];
}

//==================================================================
/**
 * �}���`�o�g���ɓr���Q�����邩���H
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiIntrude(int target_sex)
{
  return talk_multil_intrude_table[target_sex];
}

//==================================================================
/**
 * ���邮������ɓr���Q�����邩���H
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GurugurIntrude(int target_sex)
{
  return talk_guruguru_intrude[target_sex];
}

//==================================================================
/**
 * ���G�����ɓr���Q�����邩���H
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PictureIntrude(int target_sex)
{
  return talk_picture_intrude[target_sex];
}

//==================================================================
/**
 * �}���`�o�g���̓r���Q����f����
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiIntrudeRefuses(int target_sex)
{
  return talk_multi_intrude_no_table[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q����f����
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeRefuses(int target_sex)
{
  return talk_guruguru_intrude_no_table[target_sex];
}

//==================================================================
/**
 * ���G�����̓r���Q����f����
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PictureIntrudeRefuses(int target_sex)
{
  return talk_picture_intrude_no_table[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q���Ń^�}�S�������Ă��Ȃ�����
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeNoEgg(int target_sex)
{
  return talk_guruguru_intrude_no_egg[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q���Ń_���^�}�S���莝���ɂ�����
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeDametamago(int target_sex)
{
  return talk_guruguru_intrude_dametamago[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q���A�e�̏����҂�
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeWait(int target_sex)
{
  return talk_table_guruguru_wait[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q���A�r���Q��������
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeSuccess(int target_sex)
{
  return talk_table_guruguru_start[target_sex];
}

//==================================================================
/**
 * ���邮������̓r���Q���A�s����(�e����NG)
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_GuruguruIntrudeParentNG(int target_sex)
{
  return talk_table_guruguru_cancel[target_sex];
}

//==================================================================
/**
 * ���G�����̓r���Q���A�e�̏����҂�
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PictureIntrudeWait(int target_sex)
{
  return talk_table_picture_wait[target_sex];
}

//==================================================================
/**
 * ���G�����̓r���Q���A�r���Q��������
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PictureIntrudeSuccess(int target_sex)
{
  return talk_table_picture_start[target_sex];
}

//==================================================================
/**
 * ���G�����̓r���Q���A�s����(�e����NG)
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_PictureIntrudeParentNG(int target_sex)
{
  return talk_table_picture_cancel[target_sex];
}

//==================================================================
/**
 * �}���`�o�g���̓r���Q���A�e�̏����҂�
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiIntrudeWait(int target_sex)
{
  return talk_table_battle_wait[target_sex];
}

//==================================================================
/**
 * �}���`�o�g���̓r���Q���A�r���Q��������
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiIntrudeSuccess(int target_sex)
{
  return talk_table_battle_start[target_sex];
}

//==================================================================
/**
 * �}���`�o�g���̓r���Q���A�s����(�e����NG)
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_MultiIntrudeParentNG(int target_sex)
{
  return talk_table_battle_cancel[target_sex];
}

//==================================================================
/**
 * �g���[�i�[�J�[�h�J�n���̃��b�Z�[�WID�擾
 *
 * @param   unisys		
 * @param   target_nation		�ڑ�����̍�
 * @param   target_area		  �ڑ�����̒n��
 *
 * @retval  u16		
 */
//==================================================================
u16 UnionMsg_GetMsgID_TrainerCardStart(UNION_SYSTEM_PTR unisys, int target_nation, int target_area)
{
  MYSTATUS *mystatus;
  int my_nation, my_area;
  
  mystatus = GAMEDATA_GetMyStatus(unisys->uniparent->game_data);
  my_nation = MyStatus_GetMyNation(mystatus);
  my_area = MyStatus_GetMyArea(mystatus);

  if(target_nation == 0){ //�����Ȃ�
  	return msg_union_connect_01_02_3;
  }
  
	// �����Ⴄ
	if(my_nation != target_nation){
		// �n�悪�Ȃ�
		if(target_area == 0){
			return msg_union_connect_01_02_1;
		}
		// �n�悪����
		return msg_union_connect_01_02;
	}

	// ��������
	if(target_area == 0 || my_area == target_area){
  	return msg_union_connect_01_02_3; //�n�悪�������w�肪����
  }
  return msg_union_connect_01_02_2; //�n�悪�Ⴄ
}


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * WORDSET�Ƀf�t�H���g�ŃZ�b�g����P���o�^����
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void _WordsetSetDefaultData(UNION_SYSTEM_PTR unisys)
{
  UNION_MY_SITUATION *situ = &unisys->my_situation;
  const MYSTATUS *mine_myst = unisys->uniparent->mystatus;
  const STRCODE *target_name;
  u8 target_region, target_sex;
  STRBUF *strbuf_targetname, *greeting;
  u16 msgno;
  
  GF_ASSERT(unisys->msgdata != NULL);
  GF_ASSERT(unisys->wordset != NULL);
  
  if(situ->mycomm.talk_pc != NULL){
    target_name = situ->mycomm.talk_pc->beacon.name;
    target_region = situ->mycomm.talk_pc->beacon.language;
    target_sex = situ->mycomm.talk_pc->beacon.sex;
  }
  else{
    GF_ASSERT(0);
    //������΂Ȃ��悤�Ɏ����̃f�[�^�����Ă���
    target_name = MyStatus_GetMyName(mine_myst);
    target_region = PM_LANG;
    target_sex = MyStatus_GetMySex(mine_myst);
  }
  
  //����̖��O
  strbuf_targetname = GFL_STR_CreateBuffer(32, HEAPID_UNION);
  GFL_STR_SetStringCode(strbuf_targetname, target_name);
	WORDSET_RegisterWord( unisys->wordset, _WORDSET_BUFID_TARGET_NAME, 
	  strbuf_targetname, target_sex, TRUE, target_region );
	GFL_STR_DeleteBuffer(strbuf_targetname);
	
	//�����̖��O
	WORDSET_RegisterPlayerName( 
	  unisys->wordset, _WORDSET_BUFID_MINE_NAME, mine_myst );

	// ����̌���ɂ���Ĉ��A���ς��
	switch(target_region){
	case LANG_JAPAN: 	msgno = msg_union_greeting_01;	break;
	case LANG_ENGLISH: 	msgno = msg_union_greeting_02;	break;
	case LANG_FRANCE: 	msgno = msg_union_greeting_03;	break;
	case LANG_ITALY	: 	msgno = msg_union_greeting_04;	break;
	case LANG_GERMANY: 	msgno = msg_union_greeting_05;	break;
	case LANG_SPAIN	: 	msgno = msg_union_greeting_06;	break;
	case LANG_KOREA :   msgno = msg_union_greeting_08;  break;
	default:
		msgno = msg_union_greeting_07;
		break;
	}
  greeting = GFL_MSG_CreateString( unisys->msgdata, msgno );
	// �e���́u����ɂ��́v���i�[����
	WORDSET_RegisterWord( unisys->wordset, _WORDSET_BUFID_TARGET_AISATSU, greeting, 
	  target_sex, TRUE, target_region );
	GFL_STR_DeleteBuffer(greeting);
	
	if(situ->mycomm.talk_pc != NULL && situ->mycomm.talk_pc->beacon.my_nation > 0){
  	//��
  	WORDSET_RegisterCountryName( unisys->wordset, _WORDSET_BUFID_NATION, 
  	  situ->mycomm.talk_pc->beacon.my_nation );
  	if(situ->mycomm.talk_pc->beacon.my_area > 0){
    	//�n��
    	WORDSET_RegisterLocalPlaceName(unisys->wordset, _WORDSET_BUFID_AREA, 
    	  situ->mycomm.talk_pc->beacon.my_nation, situ->mycomm.talk_pc->beacon.my_area);
    }
  }
	
	//�ȈՉ�b�V�X�e���t���O�Z�b�g
	_SetPmsAisatsu( unisys->uniparent->game_data, target_region );
}

//--------------------------------------------------------------
/**
 * �e����J�[�g���b�W�ɂ��킹�ĊȈՉ�b�t���O��ǉ�����
 *
 * @param   gamedata		
 * @param   region		  ����R�[�h
 */
//--------------------------------------------------------------
static void _SetPmsAisatsu(GAMEDATA * gamedata, u8 region)
{
	if(region>=LANG_JAPAN && region <=LANG_KOREA){
		static const s32 AisatsuFlagTable[] = {
			PMSW_AISATSU_JP,	// ���{��
			PMSW_AISATSU_EN,	// �p��
			PMSW_AISATSU_FR,	// �t�����X��
			PMSW_AISATSU_IT,	// �C�^���A��
			PMSW_AISATSU_GE,	// �h�C�c��
			-1,					// ��������i���o�[
			PMSW_AISATSU_SP,	// �X�y�C����
			-1,               // �؍���
		};

		u16 lang = region - LANG_JAPAN;
		if( (lang < NELEMS(AisatsuFlagTable)) && (AisatsuFlagTable[lang] >= 0) )
		{
      SAVE_CONTROL_WORK *sv_ctrl = GAMEDATA_GetSaveControlWork(gamedata);
      PMSW_SAVEDATA *pmssave = SaveData_GetPMSW( sv_ctrl );
      
			PMSW_SetAisatsuFlag( pmssave,  AisatsuFlagTable[lang] );
			OS_Printf("�ȈՉ�b�� %d�@��ǉ�\n", PMSW_AISATSU_JP+( region - LANG_JAPAN ) );
		}
	}
}

//--------------------------------------------------------------
/**
 * WORDSET���쐬���A�f�t�H���g�f�[�^�̓o�^���s��
 *
 * @param   unisys		
 *
 * msgdata���Z�b�g���Ă���Ăяo������
 */
//--------------------------------------------------------------
static void _WordsetCreate( UNION_SYSTEM_PTR unisys )
{
  if(unisys->wordset != NULL){
    return;
  }
  unisys->wordset = WORDSET_Create( HEAPID_UNION );
  _WordsetSetDefaultData(unisys);
}

//--------------------------------------------------------------
/**
 * WORDSET���폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void _WordsetDelete( UNION_SYSTEM_PTR unisys )
{
  if(unisys->wordset != NULL){
    WORDSET_Delete(unisys->wordset);
    unisys->wordset = NULL;
  }
}


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �e���j���[�őI�������Q�[���̒ʐM�ő�Q���l�����擾����
 *
 * @param   menu_index		UNION_MSG_MENU_SELECT_???
 *
 * @retval  int		�ő�Q���l��
 */
//==================================================================
int UnionMsg_GetMemberMax(UNION_PLAY_CATEGORY menu_index)
{
  if(menu_index >= UNION_PLAY_CATEGORY_MAX){
    GF_ASSERT(menu_index < UNION_PLAY_CATEGORY_MAX);
    return 0;
  }
  return MenuMemberMax[menu_index].member_max;
}

//==================================================================
/**
 * UnionMsg�n�̂��̂�S�Ĉꊇ�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_AllDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_RegulationDel(unisys);
  UnionMsg_Menu_BattleMenuMultiTitleDel(unisys);
  UnionMsg_YesNo_Del(unisys);
  UnionMsg_Menu_WindowDel(unisys);
  UnionMsg_TalkStream_WindowDel(unisys);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;
  
  if(unisys->fld_msgwin_stream == NULL){
    GF_ASSERT(unisys->msgdata == NULL);
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->msgdata = FLDMSGBG_CreateMSGDATA( fldmsg_bg, NARC_message_union_room_dat );
    unisys->strbuf_temp = GFL_STR_CreateBuffer(256, HEAPID_UNION);
    unisys->strbuf_talk_expand = GFL_STR_CreateBuffer(256, HEAPID_UNION);
    _WordsetCreate(unisys);
    unisys->fld_msgwin_stream = FLDPLAINMSGWIN_Add(fldmsg_bg, unisys->msgdata,
     TALKMSGWIN_TYPE_NORMAL, 1, 19, 30, 4);
  }
  else{ //���ɃE�B���h�E���\������Ă���ꍇ�̓��b�Z�[�W�G���A�̃N���A���s��
    FLDPLAINMSGWIN_ClearMessage(unisys->fld_msgwin_stream);
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_TalkStream_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream != NULL){
    FLDPLAINMSGWIN_ClearWindow(unisys->fld_msgwin_stream);
    FLDPLAINMSGWIN_Delete(unisys->fld_msgwin_stream);
    unisys->fld_msgwin_stream = NULL;
    unisys->fld_msgwin_stream_print_on = FALSE;
    
    GFL_STR_DeleteBuffer(unisys->strbuf_temp);
    GFL_STR_DeleteBuffer(unisys->strbuf_talk_expand);
    unisys->strbuf_temp = NULL;
    unisys->strbuf_talk_expand = NULL;
  }

  if(unisys->msgdata != NULL){
    _WordsetDelete(unisys);
    FLDMSGBG_DeleteMSGDATA(unisys->msgdata);
    unisys->msgdata = NULL;
  }
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n
 *
 * @param   unisys		
 * @param   str_id		���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_Print(UNION_SYSTEM_PTR unisys, u32 str_id)
{
  GF_ASSERT(unisys->fld_msgwin_stream != NULL);

  GFL_MSG_GetString(unisys->msgdata, str_id, unisys->strbuf_temp);
  WORDSET_ExpandStr(unisys->wordset, unisys->strbuf_talk_expand, unisys->strbuf_temp);
  FLDPLAINMSGWIN_PrintStreamStartStrBuf(
    unisys->fld_msgwin_stream, 0, 0, unisys->strbuf_talk_expand);

  unisys->fld_msgwin_stream_print_on = TRUE;
}

//==================================================================
/**
 * ��b�E�B���h�E�F�I���`�F�b�N
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:�I���B�@FALSE:�p����
 */
//==================================================================
BOOL UnionMsg_TalkStream_Check(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fld_msgwin_stream == NULL || unisys->fld_msgwin_stream_print_on == FALSE){
    return TRUE;
  }
  return FLDPLAINMSGWIN_PrintStream(unisys->fld_msgwin_stream);
}

//==================================================================
/**
 * ��b�E�B���h�E�F�o�͊J�n�ɉ����A���b�Z�[�W�E�B���h�E�������ꍇ�͐������s��
 *
 * @param   unisys		  
 * @param   fieldWork		
 * @param   str_id		  ���b�Z�[�WID
 */
//==================================================================
void UnionMsg_TalkStream_PrintPack(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, u32 str_id)
{
  UnionMsg_TalkStream_WindowSetup(unisys, fieldWork);
  _WordsetSetDefaultData(unisys);
  UnionMsg_TalkStream_Print(unisys, str_id);
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_YesNo_Setup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg;

  if(unisys->fldmenu_yesno_func == NULL){
    fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->fldmenu_yesno_func = FLDMENUFUNC_AddYesNoMenu(fldmsg_bg, FLDMENUFUNC_YESNO_YES);
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I���E�B���h�E�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_YesNo_Del(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_yesno_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_yesno_func);
    unisys->fldmenu_yesno_func = NULL;
  }
}

//==================================================================
/**
 * �u�͂��v�u�������v�I��҂�
 *
 * @param   unisys		
 * @param   result		���ʑ����(TRUE:�͂��AFALSE:������)
 *
 * @retval  BOOL		TRUE:�I�������B�@FALSE:�I��
 */
//==================================================================
BOOL UnionMsg_YesNo_SelectLoop(UNION_SYSTEM_PTR unisys, BOOL *result)
{
  FLDMENUFUNC_YESNO yes_no;
  
  GF_ASSERT(unisys->fldmenu_yesno_func != NULL);
  
  yes_no = FLDMENUFUNC_ProcYesNoMenu(unisys->fldmenu_yesno_func);
  if(yes_no == FLDMENUFUNC_YESNO_NULL){
    return FALSE;
  }
  else if(yes_no == FLDMENUFUNC_YESNO_YES){
    *result = TRUE;
  }
  else{
    *result = FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   menulist		
 * @param   list_max		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, const FLDMENUFUNC_LIST *menulist, int list_max, const FLDMENUFUNC_HEADER *menuhead)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  FLDMENUFUNC_LISTDATA *fldmenu_listdata;
  
  if(unisys->fldmenu_func == NULL){
    fldmenu_listdata = FLDMENUFUNC_CreateMakeListData(
      menulist, list_max, unisys->msgdata, HEAPID_UNION);
    unisys->fldmenu_func = FLDMENUFUNC_AddEventMenuList( fldmsg_bg, menuhead, fldmenu_listdata,
      NULL, NULL, 0, 0, TRUE);
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�폜
 *
 * @param   unisys		
 */
//--------------------------------------------------------------
static void UnionMsg_Menu_WindowDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmenu_func != NULL){
    FLDMENUFUNC_DeleteMenu(unisys->fldmenu_func);
    unisys->fldmenu_func = NULL;
  }
}

//--------------------------------------------------------------
/**
 * ���j���[���X�g�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//--------------------------------------------------------------
static u32 UnionMsg_Menu_SelectLoop(UNION_SYSTEM_PTR unisys)
{
  GF_ASSERT(unisys->fldmenu_func != NULL);
  return FLDMENUFUNC_ProcMenu(unisys->fldmenu_func);
}

//==================================================================
/**
 * ���C�����j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_MainMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    MainMenuList, NELEMS(MainMenuList), &MenuHeader_MainMenu);
}

//==================================================================
/**
 * ���C�����j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_MainMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * ���C�����j���[�F�I��҂�
 *
 * @param   unisys		
 * 
 * @retval  u32		����
 */
//==================================================================
u32 UnionMsg_Menu_MainMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

//==================================================================
/**
 * �ΐ탁�j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, int menu_index, UNION_MENU_REGULATION *menu_reg)
{
  FLDMENUFUNC_HEADER head;
  
  menu_reg->menu_index = menu_index;
  
  head = MenuHeader_Battle;
  head.count = BattleMenuDataTbl[menu_index].list_max;
  head.line = BattleMenuDataTbl[menu_index].list_max;
  head.bmpsize_y = head.count * 2;
  
  if(menu_index == BATTLE_MENU_INDEX_CUSTOM || menu_index == BATTLE_MENU_INDEX_CUSTOM_MULTI){
    head.bmpsize_x = 30;
  }
  if(menu_index >= BATTLE_MENU_INDEX_CUSTOM_MULTI){
    head.bmppos_y = 5;
  }

  if(menu_index == BATTLE_MENU_INDEX_CUSTOM){
    FLDMENUFUNC_LIST *list;
    
    list = GFL_HEAP_AllocClearMemory(
      HEAPID_UNION, sizeof(FLDMENUFUNC_LIST) * NELEMS(BattleMenuList_Custom));
    GFL_STD_MemCopy(BattleMenuList_Custom, list, 
      sizeof(FLDMENUFUNC_LIST) * NELEMS(BattleMenuList_Custom));
    
    list[0].str_id = msg_union_battle_01_12_01 + menu_reg->mode;     //�ΐ�`��(�V���O���A�_�u���c)
    list[1].str_id = msg_union_battle_01_12_06 + menu_reg->rule;     //���[��(���x��50�A���������c)
    list[2].str_id = msg_union_battle_01_12_08 + menu_reg->shooter;  //�V���[�^�[�L��
    
    UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
      list, BattleMenuDataTbl[menu_index].list_max, &head);
    GFL_HEAP_FreeMemory(list);
  }
  else if(menu_index == BATTLE_MENU_INDEX_CUSTOM_MULTI){
    FLDMENUFUNC_LIST *list;
    
    list = GFL_HEAP_AllocClearMemory(
      HEAPID_UNION, sizeof(FLDMENUFUNC_LIST) * NELEMS(BattleMenuList_CustomMulti));
    GFL_STD_MemCopy(BattleMenuList_CustomMulti, list, 
      sizeof(FLDMENUFUNC_LIST) * NELEMS(BattleMenuList_CustomMulti));
    
    list[0].str_id = msg_union_battle_01_12_06 + menu_reg->rule;     //���[��(���x��50�A���������c)
    list[1].str_id = msg_union_battle_01_12_08 + menu_reg->shooter;  //�V���[�^�[�L��
    
    UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
      list, BattleMenuDataTbl[menu_index].list_max, &head);
    GFL_HEAP_FreeMemory(list);
  }
  else{
    UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
      BattleMenuDataTbl[menu_index].list, BattleMenuDataTbl[menu_index].list_max, &head);
  }
}

//==================================================================
/**
 * �ΐ탁�j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * �ΐ탁�j���[�F�I��҂�
 *
 * @param   unisys		
 * @param   next_sub_menu		TRUE:���̃T�u���j���[������@FALSE:�ŏI����
 *
 * @retval  u32		next_sub_menu��TRUE�̏ꍇ�A���̃��j���[Index
 * @retval  u32		next_sub_menu��FALSE�̏ꍇ�A�ŏI���ʔԍ�(�����ʐM�ő���)
 */
//==================================================================
u32 UnionMsg_Menu_BattleMenuSelectLoop(UNION_SYSTEM_PTR unisys, BOOL *next_sub_menu, UNION_MENU_REGULATION *menu_reg, BOOL *reg_look)
{
  u32 menu_ret;
  int i;
  
  *next_sub_menu = FALSE;
  *reg_look = FALSE;
  
  menu_ret = UnionMsg_Menu_SelectLoop(unisys);
  switch(menu_ret){
  case FLDMENUFUNC_NULL:
    return menu_ret;
  case FLDMENUFUNC_CANCEL:
    if(menu_reg->menu_index == BATTLE_MENU_INDEX_NUMBER){
      return menu_ret;
    }
    //��ԉ��̃��j���[���L�����Z������̂͂��Ȃ̂ŁA�����Ԃ�
    menu_ret = (u32)(BattleMenuDataTbl[menu_reg->menu_index].list[BattleMenuDataTbl[menu_reg->menu_index].list_max-1].selectParam);
    break;
  case UNION_BATTLE_REGULATION_DECIDE:
    return MenuReg_to_PlayCategory[menu_reg->mode][menu_reg->rule][menu_reg->shooter];
  case UNION_BATTLE_REGULATION_RULE:
    *reg_look = TRUE; //���M�����[�V����������
    return MenuReg_to_PlayCategory[menu_reg->mode][menu_reg->rule][menu_reg->shooter];
  }
  if(menu_reg->menu_index == BATTLE_MENU_INDEX_NUMBER){
    if(menu_ret == (u32)(&BattleMenuList_Custom)){  //��l�ΐ�
      if(menu_reg->mode == UNION_BATTLE_REGULATION_MODE_MULTI){
        menu_reg->mode = UNION_BATTLE_REGULATION_MODE_SINGLE;
      }
    }
    else if(menu_ret == (u32)(&BattleMenuList_CustomMulti)){  //�}���`
      menu_reg->mode = UNION_BATTLE_REGULATION_MODE_MULTI;
    }
  }

  for(i = 0; i < NELEMS(BattleMenuDataTbl); i++){
    if(BattleMenuDataTbl[i].list == (void*)menu_ret){
      *next_sub_menu = TRUE;
      return i;
    }
    else{
      switch(menu_reg->menu_index){
      case BATTLE_MENU_INDEX_MODE:
        if(menu_ret < UNION_BATTLE_REGULATION_MODE_MAX){
          menu_reg->mode = menu_ret;
          *next_sub_menu = TRUE;
          return BATTLE_MENU_INDEX_CUSTOM;
        }
        break;
      case BATTLE_MENU_INDEX_RULE:
      case BATTLE_MENU_INDEX_RULE_MULTI:
        if(menu_ret < UNION_BATTLE_REGULATION_RULE_MAX){
          menu_reg->rule = menu_ret;
          *next_sub_menu = TRUE;
          if(menu_reg->menu_index == BATTLE_MENU_INDEX_RULE_MULTI){
            return BATTLE_MENU_INDEX_CUSTOM_MULTI;
          }
          return BATTLE_MENU_INDEX_CUSTOM;
        }
        break;
      case BATTLE_MENU_INDEX_SHOOTER:
      case BATTLE_MENU_INDEX_SHOOTER_MULTI:
        if(menu_ret < UNION_BATTLE_REGULATION_SHOOTER_MAX){
          menu_reg->shooter = menu_ret;
          *next_sub_menu = TRUE;
          if(menu_reg->menu_index == BATTLE_MENU_INDEX_SHOOTER_MULTI){
            return BATTLE_MENU_INDEX_CUSTOM_MULTI;
          }
          return BATTLE_MENU_INDEX_CUSTOM;
        }
        break;
      
      }
    }
  }
  
  GF_ASSERT_MSG(0, "menu_ret=%d\n", menu_ret);
  return 0;
}

//==================================================================
/**
 * �ΐ탁�j���[�Ń}���`�o�g����I�񂾂Ƃ��ɏo���Ă����\����Z�b�g�A�b�v
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuMultiTitleSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  if(unisys->fldmsgwin == NULL){
    FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
    unisys->fldmsgwin = FLDPLAINMSGWIN_Add(fldmsg_bg, unisys->msgdata, TALKMSGWIN_TYPE_NORMAL, 1, 1, 30, 2);
    FLDPLAINMSGWIN_Print( unisys->fldmsgwin, 0, 0, msg_union_battle_01_12_05 );
  }
}

//==================================================================
/**
 * �ΐ탁�j���[�Ń}���`�o�g����I�񂾂Ƃ��ɏo���Ă����\����폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_BattleMenuMultiTitleDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->fldmsgwin != NULL){
    FLDPLAINMSGWIN_Delete(unisys->fldmsgwin);
    unisys->fldmsgwin = NULL;
  }
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 */
//==================================================================
void UnionMsg_Menu_PokePartySelectMenuSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  UnionMsg_Menu_WindowSetup(unisys, fieldWork, 
    PokePartySelectMenuList, NELEMS(PokePartySelectMenuList), &MenuHeader_PokePartySelect);
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_PokePartySelectMenuDel(UNION_SYSTEM_PTR unisys)
{
  UnionMsg_Menu_WindowDel(unisys);
}

//==================================================================
/**
 * POKEPARTY�I�����j���[�F�I��҂�
 *
 * @param   unisys		
 *
 * @retval  u32		����
 */
//==================================================================
u32 UnionMsg_Menu_PokePartySelectMenuSelectLoop(UNION_SYSTEM_PTR unisys)
{
  return UnionMsg_Menu_SelectLoop(unisys);
}

//==================================================================
/**
 * NG���M�����[�V�����F�Z�b�g�A�b�v
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   fail_bit    ���M�����[�V����NGbit
 * @param   shooter_type  TRUE:�V���[�^�[�L�@FALSE:�V���[�^�[����
 * @param   regwin_type    NGREG_TYPE_???
 */
//==================================================================
void UnionMsg_Menu_RegulationSetup(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork, u32 fail_bit, BOOL shooter_type, REGWIN_TYPE regwin_type)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  int category;
  PRINTSYS_LSB color;
  u16 title_msgid;
  
  GF_ASSERT(unisys->wordset != NULL);
  GF_ASSERT(unisys->fldmsgwin == NULL);
  GF_ASSERT(unisys->alloc.rpm == NULL);
  
  if(shooter_type == TRUE){
    shooter_type = REGULATION_SHOOTER_VALID;
  }
  else{
    shooter_type = REGULATION_SHOOTER_INVALID;
  }
  
//  _WordsetCreate(unisys);
  unisys->fldmsgwin = FLDPLAINMSGWIN_Add(fldmsg_bg, unisys->msgdata, 
    TALKMSGWIN_TYPE_NORMAL, 1, 1, 26, (REGULATION_CATEGORY_MAX+2)*2);
  unisys->alloc.rpm = PokeRegulation_CreatePrintMsg(unisys->alloc.regulation,
    unisys->wordset, HEAPID_UNION, shooter_type);
  _WordsetSetDefaultData(unisys); //���M�����[�V������WORDSET���㏑�����ꂽ�̂ŕ��A
  
  if(regwin_type == REGWIN_TYPE_RULE){
    STRBUF* cupname = Regulation_CreateCupName(unisys->alloc.regulation, HEAPID_UNION);
    FLDPLAINMSGWIN_PrintStrBuf( unisys->fldmsgwin, 0, 0, cupname );
    GFL_STR_DeleteBuffer(cupname);
  }
  else{
    if(regwin_type == REGWIN_TYPE_NG_TEMOTI){
      title_msgid = msg_union_ng_temoti;
    }
    else{
      title_msgid = msg_union_ng_bbox;
    }
    FLDPLAINMSGWIN_Print( unisys->fldmsgwin, 0, 0, title_msgid );
  }
  
  for(category = 0; category < REGULATION_CATEGORY_MAX; category++){
    if(fail_bit & (1 << category)){
      color = REG_FAIL_COLOR;
    }
    else{
      color = REG_NORMAL_COLOR;
    }
    FLDPLAINMSGWIN_PrintStrBufColor( unisys->fldmsgwin, 0, (4 + 2*category)*8, 
      unisys->alloc.rpm->category[category], color );
    FLDPLAINMSGWIN_PrintStrBufColor( unisys->fldmsgwin, 14*8, (4 + 2*category)*8, 
      unisys->alloc.rpm->prerequisite[category], color );
  }
}

//==================================================================
/**
 * NG���M�����[�V�����F�`�抮���҂�
 *
 * @param   unisys		
 * @param   fieldWork		
 *
 * @retval  BOOL		TRUE:�`�抮���@FALSE:�`�撆
 */
//==================================================================
BOOL UnionMsg_Menu_RegulationWait(UNION_SYSTEM_PTR unisys, FIELDMAP_WORK *fieldWork)
{
  FLDMSGBG *fldmsg_bg = FIELDMAP_GetFldMsgBG(fieldWork);
  PRINT_QUE *print_que = FLDMSGBG_GetPrintQue( fldmsg_bg );

  if(unisys->alloc.rpm == NULL){
    return TRUE;
  }

  return PRINTSYS_QUE_IsFinished( print_que );
}

//==================================================================
/**
 * NG���M�����[�V�����F�폜
 *
 * @param   unisys		
 */
//==================================================================
void UnionMsg_Menu_RegulationDel(UNION_SYSTEM_PTR unisys)
{
  if(unisys->alloc.rpm == NULL){
    return;
  }
  
//  _WordsetDelete(unisys);
  
  FLDPLAINMSGWIN_Delete(unisys->fldmsgwin);
  unisys->fldmsgwin = NULL;

  PokeRegulation_DeletePrintMsg(unisys->alloc.rpm);
  unisys->alloc.rpm = NULL;
}

