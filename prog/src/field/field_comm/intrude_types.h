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
#include "field/field_comm/mission_types.h"


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
  INTRUDE_COMM_STATUS_RESTART,      ///<�ʐM�ċN����
  INTRUDE_COMM_STATUS_EXIT_START,   ///<�I�������J�n
  INTRUDE_COMM_STATUS_EXIT,         ///<�I����������
  INTRUDE_COMM_STATUS_ERROR,        ///<�G���[
};

///�A�N�V�����X�e�[�^�X(�v���C���[�����݉����s���Ă��邩)
typedef enum{
  INTRUDE_ACTION_FIELD,                   ///<�t�B�[���h
  INTRUDE_ACTION_TALK,                    ///<�b��
  INTRUDE_ACTION_BATTLE,                  ///<�퓬��
  INTRUDE_ACTION_BINGO_BATTLE,            ///<�r���S�o�g����
  INTRUDE_ACTION_BINGO_BATTLE_INTRUSION,  ///<�r���S�o�g�������Q��
}INTRUDE_ACTION;

///��b�^�C�v
typedef enum{
  INTRUDE_TALK_TYPE_NORMAL,         ///<�ʏ��b
  INTRUDE_TALK_TYPE_MISSION,        ///<�~�b�V�����p�̉�b
}INTRUDE_TALK_TYPE;

///��b�X�e�[�^�X
typedef enum{
  INTRUDE_TALK_STATUS_NULL,         ///<�����l(�������Ă��Ȃ����)
  INTRUDE_TALK_STATUS_OK,           ///<��bOK
  INTRUDE_TALK_STATUS_NG,           ///<��bNG
  INTRUDE_TALK_STATUS_CANCEL,       ///<��b�L�����Z��
  INTRUDE_TALK_STATUS_BATTLE,       ///<�ΐ�
  INTRUDE_TALK_STATUS_ITEM,         ///<�A�C�e���n��
  INTRUDE_TALK_STATUS_CARD,         ///<�g���[�i�[�J�[�h������
}INTRUDE_TALK_STATUS;

///�������ԍ�
enum{
    INTRUDE_TIMING_EXIT = 20,
    INTRUDE_TIMING_BATTLE_COMMAND_ADD_BEFORE,   ///<�ʐM�ΐ�̃R�}���hADD�O
    INTRUDE_TIMING_BATTLE_COMMAND_ADD_AFTER,    ///<�ʐM�ΐ�̃R�}���hADD��
};

///�ΔŃ^�C�v
enum{
  MONOLITH_TYPE_WHITE,
  MONOLITH_TYPE_BLACK,
  
  MONOLITH_TYPE_MAX,
};

///�~�b�V�������������Ă��Ȃ����̃~�b�V�����ԍ�
#define MISSION_NO_NULL     (0xff)


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
  u8 action_status; ///<���s���̃A�N�V����(INTRUDE_ACTION_???)
  u16 disguise_no;   ///<�ϑ��ԍ�
  u8 padding[2];
}INTRUDE_STATUS;

///�v���t�B�[���f�[�^(���M�݂̂Ɏg�p�B��M�͊e�X�̃o�b�t�@�ɕ������)
typedef struct{
  MYSTATUS mystatus;
  OCCUPY_INFO occupy;
  INTRUDE_STATUS status;
}INTRUDE_PROFILE;

///�b��������ŏ��̃f�[�^
typedef struct{
  u8 talk_type;               ///<INTRUDE_TALK_TYPE
  u8 padding[3];
  union{  //talk_type�ɂ���ĕω�������e
    MISSION_DATA mdata;
  };
}INTRUDE_TALK_FIRST_ATTACK;

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
  COMM_PLAYER_PACKAGE backup_player_pack[FIELD_COMM_MEMBER_MAX];  ///<Backup���W�f�[�^(���W�ϊ������Ă��Ȃ�)
  INTRUDE_TALK_FIRST_ATTACK recv_talk_first_attack;
  INTRUDE_TALK   talk;
  INTRUDE_PROFILE send_profile;   ///<�����v���t�B�[�����M�o�b�t�@(��M��gamedata���Ȃ̂ŕs�v)
  
  MISSION_SYSTEM mission;     ///<�~�b�V�����V�X�e��
  BINGO_SYSTEM bingo;         ///<�r���S�V�X�e�����[�N
  
//  BOOL comm_act_vanish[FIELD_COMM_MEMBER_MAX];   ///<TRUE:��\��
  u8 invalid_netid;           ///<�N����ROM��net_id
  u8 exit_recv;               ///<TRUE:�I���t���O
  u8 recv_profile;            ///<�v���t�B�[����M�t���O(bit�Ǘ�)
  u8 mission_no;              ///<�~�b�V�����ԍ�

//  FIELD_COMM_MENU *commMenu_;
  u8 comm_status;
  u8 profile_req;             ///<TRUE:�v���t�B�[���v�����N�G�X�g���󂯂Ă���
  u8 profile_req_wait;        ///<�v���t�B�[���ėv������܂ł̃E�F�C�g
  u8 send_status;             ///<TRUE:�����̌��ݏ�񑗐M���N�G�X�g
  
  u8 answer_talk_ng_bit;      ///<�b��������ꂽ���A�Ή��ł��Ȃ��ꍇ��NG�Ԏ���Ԃ�(bit�Ǘ�)
  u8 connect_map_count;       ///<�A�������}�b�v�̐�
  u8 member_num;              ///<�Q���l��(�e�@�����M)
  u8 member_send_req;         ///<TRUE:�Q���l���̑��M���s��
  
  u8 warp_town_tblno;         ///<���[�v��̃e�[�u���ԍ�
  u8 area_occupy_update;      ///<TRUE:�N�����Ă���G���A�̐苒������M����(����ʂ����)
  u8 send_occupy;             ///<TRUE:�����̐苒���𑗐M���N�G�X�g
  u8 recv_target_timing_no;   ///<����w��^�C�~���O�R�}���h�̓����ԍ��̎�M�o�b�t�@
  
  u16 other_player_timeout;   ///<������l�ɂȂ����ꍇ�A�ʐM�I���֑J�ڂ���܂ł̃^�C���A�E�g
  u8 recv_target_timing_netid;  ///<����w��^�C�~���O�R�}���h�̑��M�҂�NetID
  u8 wfbc_req;                ///<WFBC�p�����[�^�v���t���O(bit�Ǘ�)
  
  u8 wfbc_recv;               ///<TRUE:WFBC�p�����[�^����M����
  u8 padding[3];
}INTRUDE_COMM_SYS;

