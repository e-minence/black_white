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
#include "field/intrude_common.h"
#include "field/field_comm/bingo_types.h"
#include "savedata/intrude_save.h"  //OCCUPY_INFO

//==============================================================================
//  �萔��`
//==============================================================================
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

///�A�N�V�����X�e�[�^�X(�v���C���[�����݉����s���Ă��邩)
typedef enum{
  INTRUDE_ACTION_FIELD,                   ///<�t�B�[���h
  INTRUDE_ACTION_TALK,                    ///<�b��
  INTRUDE_ACTION_BINGO_BATTLE,            ///<�r���S�o�g����
  INTRUDE_ACTION_BINGO_BATTLE_INTRUSION,  ///<�r���S�o�g�������Q��
}INTRUDE_ACTION;

///��b�X�e�[�^�X
typedef enum{
  INTRUDE_TALK_STATUS_NULL,         ///<�����l(�������Ă��Ȃ����)
  INTRUDE_TALK_STATUS_OK,           ///<��bOK
  INTRUDE_TALK_STATUS_NG,           ///<��bNG
  INTRUDE_TALK_STATUS_CARD,         ///<�g���[�i�[�J�[�h������
}INTRUDE_TALK_STATUS;

///�������ԍ�
enum{
    INTRUDE_TIMING_EXIT = 20,
};

//==============================================================================
//  �^��`
//==============================================================================
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
  
  u8 action_status; ///<���s���̃A�N�V����(INTRUDE_ACTION_???)
  u8 padding[3];
}INTRUDE_STATUS;

///�v���t�B�[���f�[�^(���M�݂̂Ɏg�p�B��M�͊e�X�̃o�b�t�@�ɕ������)
typedef struct{
  MYSTATUS mystatus;
  OCCUPY_INFO occupy;
  INTRUDE_STATUS status;
}INTRUDE_PROFILE;

///��b�p���[�N
typedef struct{
  u8 talk_netid;              ///<�b�������������NetID
  u8 answer_talk_netid;       ///<�b���������Ă��鑊���NetID
  u8 talk_status;             ///<INTRUDE_TALK_STATUS_???
  u8 answer_talk_status;      ///<�b���������Ă��鑊���INTRUDE_TALK_STATUS_???
}INTRUDE_TALK;

///�N���V�X�e�����[�N
typedef struct _INTRUDE_COMM_SYS{
  GAME_COMM_SYS_PTR game_comm;
  COMM_PLAYER_SYS_PTR cps;                          ///<�ʐM�v���C���[���䃏�[�N�ւ̃|�C���^
  
  GBS_BEACON send_beacon;
  INTRUDE_STATUS intrude_status_mine;               ///<�����̌��ݏ��
  INTRUDE_STATUS intrude_status[FIELD_COMM_MEMBER_MAX]; ///<�S���̌��ݏ��
  INTRUDE_TALK   talk;
  INTRUDE_PROFILE send_profile;   ///<�����v���t�B�[�����M�o�b�t�@(��M��gamedata���Ȃ̂ŕs�v)
  
  BINGO_SYSTEM bingo;         ///<�r���S�V�X�e�����[�N
  
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
  
  u8 answer_talk_ng_bit;      ///<�b��������ꂽ���A�Ή��ł��Ȃ��ꍇ��NG�Ԏ���Ԃ�(bit�Ǘ�)
  u8 connect_map_count;       ///<�A�������}�b�v�̐�
  u8 padding[2];
}INTRUDE_COMM_SYS;

