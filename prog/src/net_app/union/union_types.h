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


//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R���f�[�^���L���Ȃ��̂ł��鎖���������l
#define UNION_BEACON_VALID        (0x7a)

///���j�I���F�X�e�[�^�X
enum{
  UNION_STATUS_NORMAL,      ///<�ʏ���(�������Ă��Ȃ�)
  UNION_STATUS_ENTER,       ///<���j�I�����[���֐i��
  UNION_STATUS_LEAVE,       ///<���j�I�����[������ޏo
  
  UNION_STATUS_CONNECT_REQ,     ///<�ڑ����N�G�X�g���s��
  UNION_STATUS_CONNECT_ANSWER,  ///<�ڑ����N�G�X�g���󂯂������ԐM�Ƃ��Đڑ����ɂ����Ă�����
  UNION_STATUS_TALK,        ///<�ڑ��m����̉�b
  
  UNION_STATUS_BATTLE,      ///<�퓬
  UNION_STATUS_TRADE,       ///<����
  UNION_STATUS_RECORD,      ///<���R�[�h�R�[�i�[
  UNION_STATUS_PICTURE,     ///<���G����
  UNION_STATUS_GURUGURU,    ///<���邮�����
  
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
  u8 trainer_view;            ///<�g���[�i�[�^�C�v(���j�I�����[�����ł̌�����)
  u8 sex;                     ///<����
  u8 padding;
  
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
///���j�I�����[�����ł̎����̏�
typedef struct{
  u8 connect_mac_address[6];  ///<�ڑ����ė~�����l�ւ�MacAddress
  u8 union_status;            ///<�v���C���[�̏�(UNION_STATUS_???)
  u8 appeal_no;               ///<�A�s�[���ԍ�(UNION_APPEAL_???)
  
  //���������牺�͒ʐM�ł͑���Ȃ��f�[�^
  u8 answer_mac_address[6];  ///<�ڑ��������l�ւ�MacAddress
  s16 wait;
  u8 next_union_status;       ///<���Ɏ��s����v���C���[�̏�(UNION_STATUS_???)
  u8 func_proc;
  u8 func_seq;
  u8 padding;
}UNION_MY_SITUATION;

