//==============================================================================
/**
 * @file    bingo_types.h
 * @brief   �r���S���ʃw�b�_
 * @author  matsuda
 * @date    2009.10.10(�y)
 */
//==============================================================================
#pragma once

#include "field/intrude_common.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�r���S�o�g�������ő�l��(�����܂�)
#define BINGO_BATTLE_PLAYER_MAX       (4)

///�������N�G�X�g�̕Ԏ�(intrusion_player_answer�ɑ�������l)
typedef enum{
  BINGO_INTRUSION_ANSWER_WAIT,    ///<�Ԏ��ۗ�
  BINGO_INTRUSION_ANSWER_OK,      ///<OK
  BINGO_INTRUSION_ANSWER_NG,      ///<NG
}BINGO_INTRUSION_ANSWER;

//==============================================================================
//  �\���̒�`
//==============================================================================
///�r���S�o�g��PROC�ɓn��ParentWork
typedef struct{
  //--- �ŏ�����Z�b�g����Ă���f�[�^
  
  //--- �r���S�ʐM�V�X�e�������琏���ύX�����f�[�^
  u8 intrusion_player[FIELD_COMM_MEMBER_MAX]; ///<�r���S������](��]����netID������)
                                                ///<��]�����̏ꍇ��INTRUDE_NETID_NULL
  
  //--- �r���S�o�g��PROC������ʐM�V�X�e�����֓`����f�[�^
  u8 intrusion_player_answer[FIELD_COMM_MEMBER_MAX]; ///<������]�̕Ԏ�(BINGO_INTRUSION_ANSWER_???)
                                                     ///<intrusion_player�Ɠ�������]���̕��тł�
}BINGO_PARENT_WORK;

///�r���S�J�[�h
typedef struct{
  u8 work[32];
}BINGO_CARD;

///�N���V�X�e��������������f�[�^
typedef struct{
  BINGO_CARD  card;
  u8 intrusion_answer;      ///<�����v���̕Ԏ�(BINGO_INTRUSION_ANSWER_???)
  u8 padding[3];
}BINGO_RECV_WORK;

///�N���V�X�e���֓`����f�[�^
typedef struct{
  u8 card_req;          ///<TRUE:�J�[�h���������v��
  u8 intrusion_netid;   ///<������](������̃v���C���[��netID)
  u8 padding[2];
}BINGO_SEND_WORK;

///�r���S�o�g�������p�p�����[�^
typedef struct{
  u8 occ;               ///<TRUE:�����p�p�����[�^�̃f�[�^�L��
  u8 dummy[127];
}BINGO_INTRUSION_PARAM;

///�r���S�V�X�e���\����
typedef struct{
  BINGO_PARENT_WORK parent;
  BINGO_RECV_WORK recv;
  BINGO_SEND_WORK send;
  BINGO_INTRUSION_PARAM intrusion_param;    ///<�r���S�o�g�������p�p�����[�^
  u8 intrusion_netid_bit;               ///<������]�҂�NetID(bit�Ǘ�) �F�Ԏ����m�肵�����_�ŃN���A
  u8 intrusion_answer_ok_netid_bit;     ///<������]�҂ւ̕Ԏ�(����OK) �F�Ԏ���Ԃ�����N���A���܂�
  u8 intrusion_answer_ng_netid_bit;     ///<������]�҂ւ̕Ԏ�(����NG) �F�Ԏ���Ԃ�����N���A���܂�
  u8 intrusion_recv_answer;             ///<�����v���̕Ԏ���M�o�b�t�@
  u8 param_req_bit;                     ///<�r���S�o�g�������p�����[�^��]�҂�NetID(bit�Ǘ�)
}BINGO_SYSTEM;

