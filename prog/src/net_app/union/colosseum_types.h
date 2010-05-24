//==============================================================================
/**
 * @file    colosseum_types.h
 * @brief   �R���V�A���Ŏg�p�����`��
 * @author  matsuda
 * @date    2009.07.17(��)
 */
//==============================================================================
#pragma once

#include "net_app/union/comm_player.h"
#include "app\trainer_card.h"
#include "field/field_msgbg.h"
#include "net_app/comm_entry_menu.h"
#include "savedata/mystatus_local.h"
#include "savedata/mystatus.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�R���V�A���ɎQ���o����ő�l��
#define COLOSSEUM_MEMBER_MAX    (4)

///�����ʒu�ɂ��Ă��Ȃ�(COLOSSEUM_PARENT_WORK.stand_position�̒l)
#define COLOSSEUM_STANDING_POSITION_NULL    (0xff)

///�ǂ�POKEPARTY���g�p���邩
enum{
  COLOSSEUM_SELECT_PARTY_TEMOTI,    ///<�莝��
  COLOSSEUM_SELECT_PARTY_BOX,       ///<�o�g���{�b�N�X
  COLOSSEUM_SELECT_PARTY_CANCEL,    ///<��߂�
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�e�v���C���[�̊�{���
typedef struct{
  MYSTATUS myst;
  u8 mac_address[6];
  u8 occ;               ///< TRUE:�f�[�^�L��
  u8 force_entry;       ///< TRUE:�����G���g���[
  u8 battle_server_version;    ///< �o�g���T�[�o�[�o�[�W����
  u8 padding[3];
}COLOSSEUM_BASIC_STATUS;

///�e���������V�X�e���f�[�^
typedef struct{
  u8 stand_position[COLOSSEUM_MEMBER_MAX];  ///<�e�����o�[���ǂ̗����ʒu�ɂ��邩
  u8 answer_stand_position[COLOSSEUM_MEMBER_MAX];   ///<�����ʒu�m�F�𑗂��Ă��������o�[�ւ̕Ԏ�
  u8 battle_ready[COLOSSEUM_MEMBER_MAX];    ///<�e�����o�[���퓬�����ł�����
  u8 battle_ready_cancel[COLOSSEUM_MEMBER_MAX];    ///<�e�����o�[����퓬�����L�����Z���v��
}COLOSSEUM_PARENT_WORK;

///�����̃��[�J���f�[�^
typedef struct{
  u8 stand_position;          ///<�����̗����ʒu
  u8 answer_stand_position;   ///<�����ʒu�g�p���̕Ԏ���M�o�b�t�@
  u8 entry_answer;            ///<�G���g���[����
  u8 battle_ready;            ///<TRUE:�퓬��������
}COLOSSEUM_MINE_WORK;

///��M�o�b�t�@
typedef struct{
  COMM_PLAYER_PACKAGE comm_player_pack[COLOSSEUM_MEMBER_MAX];     ///<�ʐM����̍��W
  u8 comm_player_pack_update[COLOSSEUM_MEMBER_MAX];               ///<TRUE:�X�V����

  TR_CARD_DATA *tr_card[COLOSSEUM_MEMBER_MAX];         ///<�ʐM����̃g���[�i�[�J�[�h���
  u8 tr_card_occ[COLOSSEUM_MEMBER_MAX];                ///<TRUE:�g���[�i�[�J�[�h�f�[�^�L��

  POKEPARTY *pokeparty[COLOSSEUM_MEMBER_MAX];          ///<�ʐM����̃|�P�����p�[�e�B
  u8 pokeparty_occ[COLOSSEUM_MEMBER_MAX];              ///<TRUE:POKEPARTY�L��
  
  u8 stand_position[COLOSSEUM_MEMBER_MAX];  ///<�e�����o�[���ǂ̗����ʒu�ɂ��邩
  u8 stand_position_occ;                    ///<TRUE:�f�[�^�L��
  
  u8 pokelist_selected_num;                 ///<�|�P�������X�g�I�������l��
  
  u8 leave[COLOSSEUM_MEMBER_MAX];           ///<TRUE:�N�����ޏo���悤�Ƃ��Ă���
}COLOSSEUM_RECV_BUF;

///�R���V�A���V�X�e�����[�N
typedef struct _COLOSSEUM_SYSTEM{
  COMM_PLAYER_SYS_PTR cps;                  ///<�ʐM�v���C���[����V�X�e��
  COMM_PLAYER_PACKAGE send_mine_package;    ///<�������W���M�o�b�t�@

  COLOSSEUM_BASIC_STATUS basic_status[COLOSSEUM_MEMBER_MAX];  ///<�e�v���C���[�̊�{���
  u8 comm_ready;                            ///<TRUE:��{���̌������ς񂾂̂Ŏ��R�ɒʐM����OK
  u8 colosseum_leave;                       ///<TRUE:�ޏo�����ɓ����Ă���
  u8 entry_all_ready;                       ///<TRUE:�S���̃G���g���[������
  u8 all_battle_ready;                      ///<TRUE:�S���̐퓬��������
  
  COLOSSEUM_PARENT_WORK parentsys;          ///<�e�f�[�^
  COLOSSEUM_MINE_WORK mine;                 ///<�����f�[�^
  COLOSSEUM_RECV_BUF recvbuf;               ///<��M�o�b�t�@
  
  COMM_ENTRY_MENU_PTR entry_menu;           ///<�Q����W���j���[�Ǘ�
  
  u8 talk_obj_id;                           ///<�b�����������OBJ_ID
  u8 select_pokeparty;                      ///<�I������POKEPARTY(COLOSSEUM_SELECT_PARTY_???)
  u8 entry_bit;                             ///<�ŏI�I�ɃG���g���[�����������l��NetID(bit�Ǘ�)
  u8 padding;
}COLOSSEUM_SYSTEM;


