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


//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R���f�[�^���L���Ȃ��̂ł��鎖���������l
#define UNION_BEACON_VALID        (0x7a)

///���s���̃Q�[���J�e�S���[
enum{
  UNION_PLAY_CATEGORY_UNION,          ///<���j�I�����[��
  UNION_PLAY_CATEGORY_TRAINERCARD,    ///<�g���[�i�[�J�[�h
  UNION_PLAY_CATEGORY_COLOSSEUM,      ///<�R���V�A��
};

///���j�I���F�X�e�[�^�X
enum{
  UNION_STATUS_NORMAL,      ///<�ʏ���(�������Ă��Ȃ�)
  UNION_STATUS_ENTER,       ///<���j�I�����[���֐i��
  UNION_STATUS_LEAVE,       ///<���j�I�����[������ޏo
  
  UNION_STATUS_CONNECT_REQ,     ///<�ڑ����N�G�X�g���s��
  UNION_STATUS_CONNECT_ANSWER,  ///<�ڑ����N�G�X�g���󂯂������ԐM�Ƃ��Đڑ����ɂ����Ă�����
  UNION_STATUS_TALK_PARENT,     ///<�ڑ��m����̉�b(�e)
  UNION_STATUS_TALK_LIST_SEND_PARENT, ///<�ڑ��m����A�e�̑I�񂾍��ڑ��M���q�̕Ԏ��҂�
  UNION_STATUS_TALK_CHILD,      ///<�ڑ��m����̉�b(�q)
  UNION_STATUS_TALK_BATTLE_PARENT,  ///<��b�F�ΐ탁�j���[(�e)
  
  UNION_STATUS_TRAINERCARD, ///<�g���[�i�[�J�[�h
  UNION_STATUS_PICTURE,     ///<���G����
  UNION_STATUS_BATTLE,      ///<�퓬
  UNION_STATUS_TRADE,       ///<����
  UNION_STATUS_GURUGURU,    ///<���邮�����
  UNION_STATUS_RECORD,      ///<���R�[�h�R�[�i�[
  UNION_STATUS_SHUTDOWN,    ///<�ؒf
  
  UNION_STATUS_COLOSSEUM_MEMBER_WAIT,   ///<�R���V�A���F�����o�[�W���҂�
  
  UNION_STATUS_CHAT,        ///<�`���b�g�ҏW��
  
  UNION_STATUS_MAX,
};

///���j�I���F�A�s�[���ԍ�
enum{
  UNION_APPEAL_NULL,        ///<�A�s�[������
  
  UNION_APPEAL_BATTLE,      ///<�퓬
  UNION_APPEAL_TRADE,       ///<����
  UNION_APPEAL_RECORD,      ///<���R�[�h�R�[�i�[
  UNION_APPEAL_PICTURE,     ///<���G����
  UNION_APPEAL_GURUGURU,    ///<���邮�����
};

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
  
  UNION_TIMING_COLOSSEUM_PROC_BEFORE,   ///<�R���V�A���J�ڑO�̓������
  UNION_TIMING_COLOSSEUM_MEMBER_ENTRY_AFTER,  ///<�R���V�A���F�����o�[�W�܂�����̓������
  UNION_TIMING_COLOSSEUM_ADD_CMD_TBL_AFTER,   ///<�R���V�A���F�ʐM�e�[�u����ǉ���
  UNION_TIMING_COLOSSEUM_PROC_AFTER,    ///<�R���V�A���I����̓������
};

///�T�uPROC�Ăяo��ID     ��SubProc_PlayCategoryTbl�ƕ��т𓯂��ɂ��Ă������ƁI
typedef enum{
  UNION_SUBPROC_ID_NULL,              ///<�T�uPROC����
  UNION_SUBPROC_ID_TRAINERCARD,       ///<�g���[�i�[�J�[�h
  UNION_SUBPROC_ID_COLOSSEUM_WARP,    ///<�R���V�A���J��
  
  UNION_SUBPROC_ID_MAX,
}UNION_SUBPROC_ID;


//==============================================================================
//  �\���̒�`
//==============================================================================
///UNION_SYSTEM�\���̂̕s��`�|�C���^�^
typedef struct _UNION_SYSTEM * UNION_SYSTEM_PTR;


//--------------------------------------------------------------
//  �r�[�R��
//--------------------------------------------------------------
///�r�[�R���F�o�g��
typedef struct{
  u8 battle_mode;           ///<�퓬���[�h(UNION_BATTLE_MODE_???)
  u8 regulation;            ///<���M�����[�V�����ԍ��@��check �\���̊ۂ��Ƃ̕�����������
  u8 padding[2];
}UNION_BEACON_BATTLE;

///�r�[�R���F�`���b�g
typedef struct{
  u8 chat_no;
  u8 padding[3];
  u16 word[2];
}UNION_BEACON_CHAT;


///���j�I���ő���M����r�[�R���f�[�^
typedef struct{
  u8 connect_mac_address[6];  ///<�ڑ��������l�ւ�MacAddress

  u8 pm_version;              ///<PM_VERSION
  u8 language;                ///<PM_LANG
  u8 union_status;            ///<�v���C���[�̏�(UNION_STATUS_???)
  u8 appeal_no;               ///<�A�s�[���ԍ�(UNION_APPEAL_???)
  
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<���O
  
  u8 data_valid;              ///<UNION_BEACON_VALID:���̃r�[�R���f�[�^�͗L���Ȃ��̂ł���
  u8 play_category;           ///<���s���̃Q�[���J�e�S���[
  u8 trainer_view;            ///<�g���[�i�[�^�C�v(���j�I�����[�����ł̌�����)
  u8 sex;                     ///<����
  
  union{
    UNION_BEACON_CHAT chat;
    UNION_BEACON_BATTLE battle;
  };

  u8 reserve[16];             ///<�����ׂ̗̈\��
}UNION_BEACON;

///��M�����r�[�R���f�[�^����쐬���ꂽPC�p�����[�^
typedef struct{
  UNION_BEACON beacon;
  u8 mac_address[6];          ///<���M�����MacAddress
  u8 update_flag;             ///<�r�[�R���f�[�^��M��(UNION_BEACON_???)
  u8 padding;
  
  u16 life;                    ///<����(�t���[���P��)�@�V�����r�[�R������M���Ȃ���Ώ�����
  u8 padding2[2];
  
  u8 event_status;            ///<�C�x���g�X�e�[�^�X(BPC_EVENT_STATUS_???)
  u8 next_event_status;       ///<���Ɏ��s����C�x���g�X�e�[�^�X(BPC_EVENT_STATUS_???)
  u8 func_proc;               ///<����v���Z�XNo(BPC_SUBPROC_???)
  u8 func_seq;                ///<����v���Z�X�V�[�P���X
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
  u8 force_exit;              ///<TRUE:�ʐM���肩�狭���ؒf����M
  u8 mainmenu_select;         ///<���C�����j���[�ł̑I������
  u8 mainmenu_yesno_result;   ///<�u�͂�(TRUE)�v�u������(FALSE)�v�I������
  u8 submenu_select;          ///<���C�����j���[��̃T�u���j���[�̑I������
  UNION_TRCARD trcard;        ///<�g���[�i�[�J�[�h���
}UNION_MY_COMM;

///���j�I�����[�����ł̎����̏�
typedef struct{
  //�����M�r�[�R���Ɋ܂߂�f�[�^
  u8 play_category;           ///<���s���̃Q�[���J�e�S���[(UNION_PLAY_CATEGORY_???)
  u8 union_status;            ///<�v���C���[�̏�(UNION_STATUS_???)
  u8 appeal_no;               ///<�A�s�[���ԍ�(UNION_APPEAL_???)
  u8 padding2;
  
  //���\���̓��̈ꕔ�̃f�[�^�݂̂𑗐M�f�[�^�Ɋ܂߂�
  UNION_BEACON_PC *calling_pc; ///<�ڑ����ė~�����l��receive_beacon�ւ̃|�C���^
  UNION_MY_COMM mycomm;         ///<����M�ŕύX����p�����[�^��
  
  //���������牺�͒ʐM�ł͑���Ȃ��f�[�^
  UNION_BEACON_PC *answer_pc;  ///<�ڑ��������l��receive_beacon�ւ̃|�C���^
  UNION_BEACON_PC *connect_pc; ///<�ڑ����̐l��receive_beacon�ւ̃|�C���^
  s16 wait;
  s16 work;
  u8 next_union_status;       ///<���Ɏ��s����v���C���[�̏�(UNION_STATUS_???)
  u8 func_proc;
  u8 func_seq;
  u8 padding;
}UNION_MY_SITUATION;

//--------------------------------------------------------------
//  �V�X�e��
//--------------------------------------------------------------
typedef struct{
  BOOL active;               ///<TRUE:�T�uPROC���s��
  UNION_SUBPROC_ID id;       ///<�T�uPROC�Ăяo��ID
  void *parent_work;         ///<�T�uPROC�ɓn��ParentWork�ւ̃|�C���^
}UNION_SUB_PROC;

