//==============================================================================
/**
 * @file    union_types.h
 * @brief   ���j�I�����[���ŋ��ʂŎg�p�����`��
 * @author  matsuda
 * @date    2009.07.02(��)
 */
//==============================================================================
#pragma once

#include "buflen.h"
#include "app/trainer_card.h"
#include "system\pms_data.h"
#include "savedata/regulation.h"


//==============================================================================
//  �萔��`
//==============================================================================
///��x�ɐڑ��ł���ő�l��
#define UNION_CONNECT_PLAYER_NUM      (5) //���R�[�h�R�[�i�[���ő�5�l�Ȃ̂�

///�r�[�R���f�[�^���L���Ȃ��̂ł��鎖���������l
#define UNION_BEACON_VALID        (0x7a)

///���s���̃Q�[���J�e�S���[
typedef enum{ //��MenuMemberMax, UNION_STATUS_TRAINERCARD�ȍ~�̃e�[�u���ƕ��т𓯂��ɂ��Ă������ƁI
  UNION_PLAY_CATEGORY_UNION,          ///<���j�I�����[��
  UNION_PLAY_CATEGORY_TALK,           ///<��b��
  UNION_PLAY_CATEGORY_TRAINERCARD,    ///<�g���[�i�[�J�[�h
  UNION_PLAY_CATEGORY_PICTURE,        ///<���G����
  
  UNION_PLAY_CATEGORY_COLOSSEUM_START,
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER = UNION_PLAY_CATEGORY_COLOSSEUM_START,
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50,                 ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER,       ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE,               ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER,   ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD,           ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER,         ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50,                 ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER,       ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE,               ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER,   ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD,           ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER,         ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50,                 ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER,       ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE,               ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER,   ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD,           ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER,         ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50,                 ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER,       ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE,               ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER,   ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD,           ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_MULTI,      ///<�R���V�A��
  UNION_PLAY_CATEGORY_COLOSSEUM_END = UNION_PLAY_CATEGORY_COLOSSEUM_MULTI,
  
  UNION_PLAY_CATEGORY_TRADE,          ///<�|�P��������
  UNION_PLAY_CATEGORY_GURUGURU,       ///<���邮�����
  UNION_PLAY_CATEGORY_RECORD,         ///<���R�[�h�R�[�i�[
  
  UNION_PLAY_CATEGORY_MAX,
}UNION_PLAY_CATEGORY;

///���j�I���F�X�e�[�^�X
enum{
  UNION_STATUS_NORMAL,      ///<�ʏ���(�������Ă��Ȃ�)
  UNION_STATUS_ENTER,       ///<���j�I�����[���֐i��
  UNION_STATUS_LEAVE,       ///<���j�I�����[������ޏo
  
  UNION_STATUS_CHAT_CALL,       ///<�ȈՉ�b���͉�ʌĂяo��
  
  UNION_STATUS_CONNECT_REQ,     ///<�ڑ����N�G�X�g���s��
  UNION_STATUS_CONNECT_ANSWER,  ///<�ڑ����N�G�X�g���󂯂������ԐM�Ƃ��Đڑ����ɂ����Ă�����
  UNION_STATUS_TALK_PARENT,     ///<�ڑ��m����̉�b(�e)
  UNION_STATUS_TALK_LIST_SEND_PARENT, ///<�ڑ��m����A�e�̑I�񂾍��ڑ��M���q�̕Ԏ��҂�
  UNION_STATUS_TALK_CHILD,      ///<�ڑ��m����̉�b(�q)
  UNION_STATUS_TALK_BATTLE_PARENT,  ///<��b�F�ΐ탁�j���[(�e)
  UNION_STATUS_TALK_PLAYGAME_PARENT,  ///<���ɗV��ł��鑊��(�e)�ɘb��������
  UNION_STATUS_TALK_PLAYGAME_CHILD,   ///<���ɗV��ł��鑊��(�q)�ɘb��������
  
  //PLAY_CATEGORY�ƕ��т𓯂��ɂ���K�v������@��������============
  UNION_STATUS_TRAINERCARD, ///<�g���[�i�[�J�[�h
  UNION_STATUS_PICTURE,     ///<���G����
  UNION_STATUS_BATTLE_1VS1_SINGLE_50_SHOOTER,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_SINGLE_50,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_SINGLE_FREE_SHOOTER,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_SINGLE_FREE,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_SINGLE_STANDARD_SHOOTER,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_SINGLE_STANDARD,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_50_SHOOTER,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_50,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_FREE_SHOOTER,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_FREE,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_STANDARD_SHOOTER,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_DOUBLE_STANDARD,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_50_SHOOTER,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_50,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_FREE_SHOOTER,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_FREE,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_STANDARD_SHOOTER,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_TRIPLE_STANDARD,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_50_SHOOTER,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_50,        ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_FREE_SHOOTER,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_FREE,      ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_STANDARD_SHOOTER,  ///<�퓬
  UNION_STATUS_BATTLE_1VS1_ROTATION_STANDARD,  ///<�퓬
  UNION_STATUS_BATTLE_MULTI,      ///<�퓬
  UNION_STATUS_TRADE,       ///<����
  UNION_STATUS_GURUGURU,    ///<���邮�����
  UNION_STATUS_RECORD,      ///<���R�[�h�R�[�i�[
  // �����܂�======================
  
  UNION_STATUS_INTRUDE,     ///<����
  UNION_STATUS_SHUTDOWN,    ///<�ؒf
  
  UNION_STATUS_COLOSSEUM_MEMBER_WAIT,   ///<�R���V�A���F�����o�[�W���҂�
  UNION_STATUS_COLOSSEUM_FIRST_DATA_SHARING,  ///<�R���V�A���F�ŏ��̃f�[�^���L
  UNION_STATUS_COLOSSEUM_NORMAL,        ///<�R���V�A���F�t���[�ړ�
  UNION_STATUS_COLOSSEUM_STANDPOSITION, ///<�R���V�A���F�����ʒu�ɂ�����
  UNION_STATUS_COLOSSEUM_STANDING_BACK, ///<�R���V�A���F�����ʒu������
  UNION_STATUS_COLOSSEUM_USE_PARTY_SELECT,  ///<�R���V�A���F�莝���A�o�g���{�b�N�X�I��
  UNION_STATUS_COLOSSEUM_POKELIST_READY,    ///<�R���V�A���F�|�P�������X�g�Ăяo���O�̑S���҂�
  UNION_STATUS_COLOSSEUM_POKELIST_BEFORE_DATA_SHARE,  ///<�R���V�A���F�|�P�������X�g�Ăяo���O�̑S���f�[�^����M
  UNION_STATUS_COLOSSEUM_POKELIST,      ///<�R���V�A���F�|�P�������X�g�Ăяo��
  UNION_STATUS_COLOSSEUM_BATTLE_READY_WAIT, ///<�R���V�A���F�S�����퓬�����ł���̂�҂�
  UNION_STATUS_COLOSSEUM_BATTLE,        ///<�R���V�A���F�퓬��ʌĂяo��
  UNION_STATUS_COLOSSEUM_LEAVE,         ///<�R���V�A���F�ޏo����
  UNION_STATUS_COLOSSEUM_TRAINER_CARD,  ///<�R���V�A���F�g���[�i�[�J�[�h�Ăяo��
  
  UNION_STATUS_CHAT,        ///<�`���b�g�ҏW��
  
  UNION_STATUS_MAX,
};

///���j�I���F�A�s�[���ԍ�
typedef enum{
  UNION_APPEAL_NULL,        ///<�A�s�[������
  
  UNION_APPEAL_BATTLE,      ///<�퓬
  UNION_APPEAL_TRADE,       ///<����
  UNION_APPEAL_RECORD,      ///<���R�[�h�R�[�i�[
  UNION_APPEAL_PICTURE,     ///<���G����
  UNION_APPEAL_GURUGURU,    ///<���邮�����
}UNION_APPEAL;

///�r�[�R���F�o�g���F�퓬���[�h
enum{
  UNION_BATTLE_MODE_SINGLE,     ///<1vs1 �V���O���o�g��
  UNION_BATTLE_MODE_DOUBLE,     ///<1vs1 �_�u���o�g��
  UNION_BATTLE_MODE_TRIPLE,     ///<1vs1 �g���v���o�g��
  UNION_BATTLE_MODE_ROTATION,   ///<1vs1 ���[�e�[�V�����o�g��
  
  UNION_BATTLE_MODE_MULTI,      ///<2vs2 �}���`�o�g��
};

///���j�I���r�[�R���F��M��
enum{
  UNION_BEACON_RECEIVE_NULL,    ///<�r�[�R���ω��Ȃ�
  UNION_BEACON_RECEIVE_NEW,     ///<�V�K�r�[�R��
  UNION_BEACON_RECEIVE_UPDATE,  ///<�r�[�R���X�V
};

///���j�I�����[���̃L�����N�^����(�t���[���P��)
#define UNION_CHAR_LIFE         (30 * 15)  //�t�B�[���h�ׁ̈A1/30

///�����p�̃^�C�~���O�R�}���h�ԍ�
enum{
  UNION_TIMING_SHUTDOWN = 1,            ///<�ؒf�O�̓������
  
  UNION_TIMING_TRAINERCARD_PARAM,       ///<�g���[�i�[�J�[�h�̏������O
  UNION_TIMING_TRAINERCARD_PROC_BEFORE, ///<�g���[�i�[�J�[�h��ʌĂяo���O
  UNION_TIMING_TRAINERCARD_PROC_AFTER,  ///<�g���[�i�[�J�[�h��ʏI����

  UNION_TIMING_TRADE_PROC_BEFORE,       ///<�|�P����������ʌĂяo���O
  UNION_TIMING_TRADE_PROC_AFTER,        ///<�|�P����������ʌĂяo����
  
  UNION_TIMING_COLOSSEUM_PROC_BEFORE,   ///<�R���V�A���J�ڑO�̓������
  UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER,  ///<�R���V�A���F�����o�[�W�܂�����̓������
  UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER,   ///<�R���V�A���F�ʐM�e�[�u����ǉ���
  UNION_TIMING_COLOSSEUM_CARD_BEFORE,   ///<�R���V�A���F�S�����̃g���[�i�[�J�[�h�����O
  UNION_TIMING_BATTLE_STANDINGPOS_BEFORE,     ///<�R���V�A���F�S���̗����ʒu���M�O
  UNION_TIMING_COLOSSEUM_PROC_AFTER,    ///<�R���V�A���I����̓������
  UNION_TIMING_COLOSSEUM_LEAVE,         ///<�R���V�A���F�ޏo
  
  UNION_TIMING_LIST_POKEPARTY_BEFORE,     ///<�|�P�������X�g�FPOKEPARTY�����O
  UNION_TIMING_LIST_POKEPARTY_AFTER,     ///<�|�P�������X�g�FPOKEPARTY������
  UNION_TIMING_BATTLE_POKEPARTY_BEFORE,   ///<�퓬�FPOKEPARTY�����O
  UNION_TIMING_BATTLE_ADD_CMD_TBL_AFTER,  ///<�퓬�F�ʐM�e�[�u����ǉ���
};

///�T�uPROC�Ăяo��ID     ��SubProc_PlayCategoryTbl�ƕ��т𓯂��ɂ��Ă������ƁI
typedef enum{
  UNION_SUBPROC_ID_NULL,              ///<�T�uPROC����
  UNION_SUBPROC_ID_TRAINERCARD,       ///<�g���[�i�[�J�[�h
  UNION_SUBPROC_ID_TRADE,             ///<�|�P��������
  
  UNION_SUBPROC_ID_COLOSSEUM_WARP_START,
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_50_SHOOTER = UNION_SUBPROC_ID_COLOSSEUM_WARP_START,
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_50,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FREE_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_FREE,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_STANDARD_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_SINGLE_STANDARD,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_50_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_50,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FREE_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_FREE,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_STANDARD_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_DOUBLE_STANDARD,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_50_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_50,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FREE_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_FREE,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_STANDARD_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_TRIPLE_STANDARD,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_50_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_50,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FREE_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_FREE,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_STANDARD_SHOOTER,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_1VS1_ROTATION_STANDARD,    ///<�R���V�A���J��
  UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI,    ///<�R���V�A���J��(�}���`����)
  UNION_SUBPROC_ID_COLOSSEUM_WARP_END = UNION_SUBPROC_ID_COLOSSEUM_WARP_MULTI,
  
  UNION_SUBPROC_ID_UNION_WARP,              ///<���j�I�����[���J��
  UNION_SUBPROC_ID_POKELIST,                ///<�|�P�������X�g�Ăяo��
  UNION_SUBPROC_ID_BATTLE,                  ///<�퓬�J��
  UNION_SUBPROC_ID_COLOSSEUM_TRAINERCARD,   ///<�R���V�A���ł̃g���[�i�[�J�[�h
  
  UNION_SUBPROC_ID_CHAT,                    ///<�ȈՉ�b���͉��
  
  UNION_SUBPROC_ID_MAX,
}UNION_SUBPROC_ID;


///�`���b�g���O�ێ���
#define UNION_CHAT_LOG_MAX      (30)


//==============================================================================
//  �\���̒�`
//==============================================================================
///UNION_SYSTEM�\���̂̕s��`�|�C���^�^
typedef struct _UNION_SYSTEM * UNION_SYSTEM_PTR;


///�ڑ������o�[�̏��
typedef struct{
  u8 mac_address[6];    ///<�ꏏ�ɗV��ł��鑊���MacAddress
  u8 trainer_view;      ///<�ꏏ�ɗV��ł��鑊��̌�����
  u8 sex:1;             ///<�ꏏ�ɗV��ł��鑊��̐���
  u8 occ:1;             ///<TRUE�F�f�[�^�L��
  u8    :6;
}UNION_MEMBER;

///�ڑ������o�[�̏W����
typedef struct{
  UNION_MEMBER member[UNION_CONNECT_PLAYER_NUM];
}UNION_PARTY;

//--------------------------------------------------------------
//  �L�����N�^
//--------------------------------------------------------------
///�L�����N�^���䃏�[�N
typedef struct{
  struct _UNION_BEACON_PC *parent_pc; ///<�ePC�ւ̃|�C���^
  u8 trainer_view;            ///<�g���[�i�[�^�C�v(���j�I�����[�����ł̌�����)
  u8 sex;                     ///<����
  u8 child_no;                ///<�q�ԍ�(�e:0�@�q:1�`)
  u8 occ;                     ///<TRUE:�f�[�^�L���@FALSE:����

  u8 event_status;            ///<�C�x���g�X�e�[�^�X(BPC_EVENT_STATUS_???)
  u8 next_event_status;       ///<���Ɏ��s����C�x���g�X�e�[�^�X(BPC_EVENT_STATUS_???)
  u8 func_proc;               ///<����v���Z�XNo(BPC_SUBPROC_???)
  u8 func_seq;                ///<����v���Z�X�V�[�P���X
}UNION_CHARACTER;

///��l�̐e���Ǘ�����L�����N�^���䃏�[�N�̃O���[�v
typedef struct{
  UNION_CHARACTER *character[UNION_CONNECT_PLAYER_NUM];
}UNION_CHARA_GROUP;

//--------------------------------------------------------------
//  �r�[�R��
//--------------------------------------------------------------
///���j�I���ő���M����r�[�R���f�[�^
typedef struct{
  u8 connect_mac_address[6];  ///<�ڑ��������l�ւ�MacAddress
  u8 connect_num;             ///<���݂̐ڑ��l��
  u8 padding;
  
  u8 pm_version;              ///<PM_VERSION
  u8 language;                ///<PM_LANG
  u8 union_status;            ///<�v���C���[�̏�(UNION_STATUS_???)
  u8 appeal_no;               ///<�A�s�[���ԍ�(UNION_APPEAL_???)
  
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<���O
  
  u8 data_valid;              ///<UNION_BEACON_VALID:���̃r�[�R���f�[�^�͗L���Ȃ��̂ł���
  u8 play_category;           ///<���s���̃Q�[���J�e�S���[
  u8 trainer_view;            ///<�g���[�i�[�^�C�v(���j�I�����[�����ł̌�����)
  u8 sex;                     ///<����
  
  PMS_DATA pmsdata;          ///<�`���b�g
  u16 pms_rand;               ///<�`���b�g�p���ʃR�[�h
  u8 padding2[2];
  
  UNION_PARTY party;          ///<�ڑ�����̏��
  
  u8 reserve[8];             ///<�����ׂ̗̈\��
}UNION_BEACON;

///��M�����r�[�R���f�[�^����쐬���ꂽPC�p�����[�^
typedef struct _UNION_BEACON_PC{
  UNION_BEACON beacon;
  u8 mac_address[6];          ///<���M�����MacAddress
  u8 update_flag;             ///<�r�[�R���f�[�^��M��(UNION_BEACON_???)
  u8 buffer_no;               ///<�o�b�t�@�ԍ�
  
  u16 life;                   ///<����(�t���[���P��)�@�V�����r�[�R������M���Ȃ���Ώ�����
  u8 seat;                    ///<�ڑ�����̔z�u�ꏊ(bit�Ǘ� 0bit��=��l�ڂ̐ȁA1bit��=��l�ڂ̐�)
  u8 my_seat_no;              ///<�������g�̐�
  
  UNION_CHARA_GROUP chara_group;   ///<�L�����N�^�O���[�v
  
  struct _UNION_BEACON_PC *link_parent_pc;   ///<�����N���Ă���PC(�e)�ւ̃|�C���^
  u8 link_parent_index;       ///<�����N���Ă���PC��index
}UNION_BEACON_PC;

//--------------------------------------------------------------
//  �����f�[�^
//--------------------------------------------------------------
///�g���[�i�[�J�[�h���
typedef struct{
  TRCARD_CALL_PARAM *card_param;  ///<�J�[�h��ʌĂяo���悤��ParentWork
  TR_CARD_DATA *my_card;      ///<�����̃J�[�h���(���M�o�b�t�@)
  TR_CARD_DATA *target_card;  ///<����̃J�[�h���(��M�o�b�t�@)
  u8 target_card_receive;     ///<TRUE:����̃J�[�h����M����
  u8 padding[3];
}UNION_TRCARD;

///����M�ŕύX����p�����[�^��(���@���t���[����̏�ԂɂȂ邽�тɏ����������)
typedef struct{
  UNION_BEACON_PC *calling_pc; ///<�ڑ����ė~�����l��receive_beacon�ւ̃|�C���^
  UNION_BEACON_PC *answer_pc;  ///<�ڑ��������l��receive_beacon�ւ̃|�C���^
  UNION_BEACON_PC *connect_pc; ///<�ڑ����̐l��receive_beacon�ւ̃|�C���^
  u8 force_exit;              ///<TRUE:�ʐM���肩�狭���ؒf����M
  u8 mainmenu_select;         ///<���C�����j���[�ł̑I������
  u8 mainmenu_yesno_result;   ///<�u�͂�(TRUE)�v�u������(FALSE)�v�I������
  u8 submenu_select;          ///<���C�����j���[��̃T�u���j���[�̑I������
  UNION_TRCARD trcard;        ///<�g���[�i�[�J�[�h���
  UNION_PARTY party;          ///<�ꏏ�ɗV��ł��鑊��̃p�����[�^
  u16 talk_obj_id;             ///<�b�������������CharacterIndex
  u8 intrude;                 ///<TRUE:�����Q��
  u8 padding;
}UNION_MY_COMM;

///���j���[�ł̑I�����M�����[�V��������
typedef struct{
  u8 mode;        ///<UNION_BATTLE_REGULATION_MODE_???
  u8 rule;        ///<UNION_BATTLE_REGULATION_RULE_???
  u8 shooter;     ///<UNION_BATTLE_REGULATION_SHOOTER_???
  
  u8 menu_index;  ///<BATTLE_MENU_INDEX_???
}UNION_MENU_REGULATION;

///���j�I�����[�����ł̎����̏�
typedef struct{
  //�����M�r�[�R���Ɋ܂߂�f�[�^
  u8 play_category;           ///<���s���̃Q�[���J�e�S���[(UNION_PLAY_CATEGORY_???)
  u8 union_status;            ///<�v���C���[�̏�(UNION_STATUS_???)
  u8 appeal_no;               ///<�A�s�[���ԍ�(UNION_APPEAL_???)
  u8 padding2;
  
  PMS_DATA chat_pmsdata;      ///<�����̔���(�`���b�g)
  u16 chat_pms_rand;          ///<�`���b�g�̎��ʃR�[�h
  u8 chat_upload;             ///<TRUE:�����̔����X�V����
  u8 padding3[3];
  
  //���\���̓��̈ꕔ�̃f�[�^�݂̂𑗐M�f�[�^�Ɋ܂߂�
  UNION_MY_COMM mycomm;         ///<����M�ŕύX����p�����[�^��(�t���[����ŏ����������)
  
  //���������牺�͒ʐM�ł͑���Ȃ��f�[�^
  UNION_MENU_REGULATION menu_reg;   ///<���j���[���M�����[�V����
  s16 wait;
  s16 work;
  u8 before_union_status;     ///<�O�Ɏ��s���Ă����v���C���[�̏�(UNION_STATUS_???)
  u8 next_union_status;       ///<���Ɏ��s����v���C���[�̏�(UNION_STATUS_???)
  u8 func_proc;
  u8 func_seq;
  UNION_BEACON_PC *last_calling_pc; ///<�Ō�ɘb���������l�̃|�C���^  ��check�@�b�菈��
}UNION_MY_SITUATION;

//--------------------------------------------------------------
//  �V�X�e��
//--------------------------------------------------------------
typedef struct{
  BOOL active;               ///<TRUE:�T�uPROC���s��
  UNION_SUBPROC_ID id;       ///<�T�uPROC�Ăяo��ID
  void *parent_work;         ///<�T�uPROC�ɓn��ParentWork�ւ̃|�C���^
  u8 seq;
  u8 padding[3];
}UNION_SUB_PROC;

///���j�I�����[�����ŕʓrAlloc����郏�[�N�Ǘ�
typedef struct{
  REGULATION *regulation;    ///<�o�g�����M�����[�V����
}UNION_ALLOC;

//--------------------------------------------------------------
//  �`���b�g
//--------------------------------------------------------------
///�`���b�g�f�[�^
typedef struct{
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];    ///<���O 16
  PMS_DATA pmsdata;                             ///<�ȈՉ�b�f�[�^ 12
  u16 rand;                                     ///<���d���O�\���h�~�p�̃����_���R�[�h 2
  u8 mac_address[6];                            ///<MacAddress 6
  u8 sex;                                       ///<����
  u8 padding[3];
}UNION_CHAT_DATA;

///�`���b�g���O�Ǘ�
typedef struct{
  UNION_CHAT_DATA chat[UNION_CHAT_LOG_MAX];     ///<�`���b�g�f�[�^
  u8 start_no;
  u8 end_no;
  u8 padding[2];
  s32 chat_log_count;
  s32 chat_view_no;
  s32 old_chat_view_no;
}UNION_CHAT_LOG;

