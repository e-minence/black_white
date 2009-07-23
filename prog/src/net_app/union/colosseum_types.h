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


//==============================================================================
//  �萔��`
//==============================================================================
///�R���V�A���ɎQ���o����ő�l��
#define COLOSSEUM_MEMBER_MAX    (4)

///�����ʒu�ɂ��Ă��Ȃ�(COLOSSEUM_PARENT_WORK.stand_position�̒l)
#define COLOSSEUM_STANDING_POSITION_NULL    (0xff)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�e�v���C���[�̊�{���
typedef struct{
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<���O
  u32 id;               ///< ID(�g���[�i�[�J�[�h�ɂ�����Ă���ID)
  u8 mac_address[6];
  u8 sex;
  u8 trainer_view;
  u8 occ;               ///< TRUE:�f�[�^�L��
  u8 padding[3];
}COLOSSEUM_BASIC_STATUS;

///�e���������V�X�e���f�[�^
typedef struct{
  u8 stand_position[COLOSSEUM_MEMBER_MAX];  ///<�e�����o�[���ǂ̗����ʒu�ɂ��邩
  u8 answer_stand_position[COLOSSEUM_MEMBER_MAX];   ///<�����ʒu�m�F�𑗂��Ă��������o�[�ւ̕Ԏ�
}COLOSSEUM_PARENT_WORK;

///�����̃��[�J���f�[�^
typedef struct{
  u8 stand_position;          ///<�����̗����ʒu
  u8 answer_stand_position;   ///<�����ʒu�g�p���̕Ԏ���M�o�b�t�@
}COLOSSEUM_MINE_WORK;

///��M�o�b�t�@
typedef struct{
  COMM_PLAYER_PACKAGE comm_player_pack;     ///<�ʐM����̍��W
  u8 comm_player_pack_update;               ///<TRUE:�X�V����
}COLOSSEUM_RECV_BUF;

///�R���V�A���V�X�e�����[�N
typedef struct _COLOSSEUM_SYSTEM{
  COMM_PLAYER_SYS_PTR cps;                  ///<�ʐM�v���C���[����V�X�e��
  COMM_PLAYER_PACKAGE send_mine_package;    ///<�������W���M�o�b�t�@

  COLOSSEUM_BASIC_STATUS basic_status[COLOSSEUM_MEMBER_MAX];  ///<�e�v���C���[�̊�{���
  BOOL comm_ready;                          ///<TRUE:��{���̌������ς񂾂̂Ŏ��R�ɒʐM����OK
  
  TR_CARD_DATA *tr_card[COLOSSEUM_MEMBER_MAX];  ///<�ʐM����̃g���[�i�[�J�[�h���
  u8 tr_card_occ[COLOSSEUM_MEMBER_MAX];     ///<TRUE:�g���[�i�[�J�[�h�f�[�^�L��
  
  COLOSSEUM_PARENT_WORK parentsys;          ///<�e�f�[�^
  COLOSSEUM_MINE_WORK mine;                 ///<�����f�[�^
  COLOSSEUM_RECV_BUF recvbuf[COLOSSEUM_MEMBER_MAX]; ///<��M�o�b�t�@
}COLOSSEUM_SYSTEM;


