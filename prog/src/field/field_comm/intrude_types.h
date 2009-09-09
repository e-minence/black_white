//==============================================================================
/**
 * @file    intrude_types.h
 * @brief   �N���F���ʃw�b�_
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#pragma once

#include "field\game_beacon_search.h"
#include "net_app/union/comm_player.h"

//==============================================================================
//  �萔��`
//==============================================================================
///�N���F�ʐM�ő�ڑ��l��
#define FIELD_COMM_MEMBER_MAX (4)

///�ʐM�X�e�[�^�X
enum{
  INTRUDE_COMM_STATUS_NULL,         ///<�������삵�Ă��Ȃ�
  INTRUDE_COMM_STATUS_INIT_START,   ///<�������J�n
  INTRUDE_COMM_STATUS_INIT,         ///<����������
  INTRUDE_COMM_STATUS_BOOT_PARENT,  ///<�e�Ƃ��ċN��
  INTRUDE_COMM_STATUS_BOOT_CHILD,   ///<�q�Ƃ��ċN��
  INTRUDE_COMM_STATUS_HARD_CONNECT, ///<�n�[�h�͐ڑ�����(�l�S�V�G�[�V�����͂܂�)
  INTRUDE_COMM_STATUS_BASIC_SEND,   ///<��{��񑗐M��
  INTRUDE_COMM_STATUS_UPDATE,       ///<���C���X�V���s��
  INTRUDE_COMM_STATUS_EXIT_START,   ///<�I�������J�n
  INTRUDE_COMM_STATUS_EXIT,         ///<�I����������
  INTRUDE_COMM_STATUS_ERROR,        ///<�G���[
};

///�������ԍ�
enum{
    INTRUDE_TIMING_EXIT = 20,
};

//==============================================================================
//  �^��`
//==============================================================================
///_INTRUDE_COMM_SYS�\���̂̕s��`�|�C���^�^
typedef struct _INTRUDE_COMM_SYS * INTRUDE_COMM_SYS_PTR;

///�p���X�V�X�e�����[�N�\���̂ւ̕s��`�|�C���^
typedef struct _PALACE_SYS_WORK * PALACE_SYS_PTR;

//==============================================================================
//  �\���̒�`
//==============================================================================
///�N���X�e�[�^�X(�����̌��ݏ��)
typedef struct{
  COMM_PLAYER_PACKAGE player_pack;    ///<�v���C���[���W�f�[�^�p�b�P�[�W
  u16 zone_id;      ///<�]�[��ID
  u8 palace_area;   ///<�p���X�G���A
  u8 mission_no;    ///<�~�b�V�����ԍ�
}INTRUDE_STATUS;

///�N���V�X�e�����[�N
typedef struct _INTRUDE_COMM_SYS{
  GAME_COMM_SYS_PTR game_comm;
  COMM_PLAYER_SYS_PTR cps;                          ///<�ʐM�v���C���[���䃏�[�N�ւ̃|�C���^
  
  GBS_BEACON send_beacon;
  INTRUDE_STATUS intrude_status_mine;               ///<�����̌��ݏ��
  INTRUDE_STATUS intrude_status[FIELD_COMM_MEMBER_MAX]; ///<�S���̌��ݏ��
  
//  BOOL comm_act_vanish[FIELD_COMM_MEMBER_MAX];   ///<TRUE:��\��
  u8 invalid_netid;           ///<�N����ROM��net_id
  u8 exit_recv;               ///<TRUE:�I���t���O
  u8 recv_profile;            ///<�v���t�B�[����M�t���O(bit�Ǘ�)
  u8 mission_no;              ///<�~�b�V�����ԍ�

  PALACE_SYS_PTR palace;    ///<�p���X�V�X�e�����[�N�ւ̃|�C���^
//  FIELD_COMM_MENU *commMenu_;
  u8 comm_status;
  u8 profile_req;             ///<TRUE:�v���t�B�[���v�����N�G�X�g���󂯂Ă���
  u8 profile_req_wait;        ///<�v���t�B�[���ėv������܂ł̃E�F�C�g
  u8 send_status;             ///<TRUE:�����̌��ݏ�񑗐M���N�G�X�g
  
  u8 recv_status;             ///<��x�ł���M������������N���X�e�[�^�X��c��(bit�Ǘ�)
  u8 padding[3];
}INTRUDE_COMM_SYS;

