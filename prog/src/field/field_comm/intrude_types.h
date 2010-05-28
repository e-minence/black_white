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
#include "savedata/intrude_save.h"  //OCCUPY_INFO
#include "field/field_comm/mission_types.h"
#include "field/field_wfbc_data.h"
#include "field/intrude_symbol.h"


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
  INTRUDE_ACTION_EFFECT,                  ///<���o��
}INTRUDE_ACTION;

///��b�^�C�v
typedef enum{
  INTRUDE_TALK_TYPE_NORMAL,               ///<�ʏ��b
  
  INTRUDE_TALK_TYPE_MISSION_N_to_M,       ///<���ʁF�T�ώҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_N_to_T,       ///<���ʁF�T�ώҁ��^�[�Q�b�g
  
  //�~�b�V������b�̃I�t�Z�b�g (���~�b�V�����ȊO�̉�b�͂�����O�ɔz�u���鎖!!)
  // ���~�b�V������b�ȍ~��EventCommFuncTalkTbl, EventCommFuncTalkedTbl�ƕ��т���v�����邱�ƁI�I
  INTRUDE_TALK_TYPE_MISSION_OFFSET_START,
  INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_T = INTRUDE_TALK_TYPE_MISSION_OFFSET_START,    ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_M,    ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_OFFSET_M_to_N,    ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_OFFSET_T_to_M,    ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_OFFSET_T_to_N,    ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_VICTORY_START,
  INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_T = INTRUDE_TALK_TYPE_MISSION_VICTORY_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_VICTORY_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_VICTORY_T_to_N, ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_SKILL_START,
  INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_T = INTRUDE_TALK_TYPE_MISSION_SKILL_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_SKILL_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_SKILL_T_to_N, ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_BASIC_START,
  INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_T = INTRUDE_TALK_TYPE_MISSION_BASIC_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_BASIC_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_BASIC_T_to_N, ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_START,
  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_T = INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_ATTRIBUTE_T_to_N, ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_ITEM_START,
  INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_T = INTRUDE_TALK_TYPE_MISSION_ITEM_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_ITEM_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_ITEM_T_to_N, ///<�^�[�Q�b�g���T�ώ�

  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_START,
  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_T = INTRUDE_TALK_TYPE_MISSION_PERSONALITY_START, ///<�~�b�V�������{�ҁ��^�[�Q�b�g
  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_M, ///<�~�b�V�������{�ҁ��~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_M_to_N, ///<�~�b�V�������{�ҁ��T�ώ�
  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_M, ///<�^�[�Q�b�g���~�b�V�������{��
  INTRUDE_TALK_TYPE_MISSION_PERSONALITY_T_to_N, ///<�^�[�Q�b�g���T�ώ�
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
  INTRUDE_TALK_STATUS_SHOP_OK,      ///<��������
  INTRUDE_TALK_STATUS_SHOP_NG,      ///<�����s����
  INTRUDE_TALK_STATUS_SHOP_SHOT_OF_MONEY,      ///<����������Ȃ�
  INTRUDE_TALK_STATUS_SHOP_ITEM_FULL,          ///<�A�C�e���������ς�
}INTRUDE_TALK_STATUS;

///�����ؒf��̃��b�Z�[�WID
typedef enum{
  MISSION_FORCEWARP_MSGID_NULL,       ///<�w��Ȃ�
  MISSION_FORCEWARP_MSGID_BATTLE_NG,  ///<�ΐ��f��ꂽ
  MISSION_FORCEWARP_MSGID_SHOP_NG,    ///<��������f��ꂽ
}MISSION_FORCEWARP_MSGID;

///�������ԍ�
enum{
    INTRUDE_TIMING_EXIT = 20,
    INTRUDE_TIMING_MISSION_BATTLE_BEFORE,       ///<�~�b�V�����o�g���J�n�O
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

///����f�[�^��M�o�b�t�@�T�C�Y
#define INTRUDE_HUGE_RECEIVE_BUF_SIZE   (644)

///�߂��ɂ���r�[�R���̐l����\������ő吔
#define INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX   (2)


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
  
  u16 zone_id;       ///<�]�[��ID
  u16 palace_area:3; ///<�p���X�G���A
  u16 symbol_mapid:6;
  u16 action_status:6; ///<���s���̃A�N�V����(INTRUDE_ACTION_???)
  u16 detect_fold:1;  ///<TRUE:�W����Ă���
  
  u16 disguise_no;   ///<�ϑ��ԍ�
  u8 disguise_type:4; ///<�ϑ��^�C�v(TALK_TYPE_xxx)
  u8 disguise_sex:1;  ///<�ϑ����̐���
  u8 mission_entry:1; ///<TRUE:�~�b�V�����ɎQ�����Ă���
  u8 season:2;        ///<�l�G(PMSEASON)
  u8 pm_version:7;    ///<PM_VERSION
  u8 tutorial:1;      ///<TRUE:�`���[�g���A����
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

///���m���X�X�e�[�^�X(�~�b�V�������X�g�ȊO�Ń��m���X��ʍ\�z�ɕK�v�ȃf�[�^)
typedef struct{
  u32 clear_mission_count;       ///<�~�b�V�����N���A��
  s64 palace_sojourn_time;       ///<�p���X�؍ݎ���
  u8 gpower_distribution_bit[INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX]; ///<G�p���[�z�z��Mbit
  u8 occ;                        ///<TRUE:�f�[�^�L��
  u8 padding;
}MONOLITH_STATUS;

///�~�b�V�����B����̐苒����
typedef struct{
  u8 add_white;               ///<�����_
  u8 add_black;               ///<�����_
  u8 occ;                     ///<TRUE:�f�[�^�L��
  u8 padding;
}INTRUDE_OCCUPY_RESULT;

///�N���V�X�e�����[�N
typedef struct _INTRUDE_COMM_SYS{
  GAME_COMM_SYS_PTR game_comm;
  COMM_PLAYER_SYS_PTR cps;                          ///<�ʐM�v���C���[���䃏�[�N�ւ̃|�C���^
  
  u8 huge_receive_buf[INTRUDE_HUGE_RECEIVE_BUF_SIZE];  ///<����f�[�^��M�o�b�t�@

  GBS_BEACON send_beacon;
  INTRUDE_STATUS intrude_status_mine;               ///<�����̌��ݏ��
  INTRUDE_STATUS intrude_status[FIELD_COMM_MEMBER_MAX]; ///<�S���̌��ݏ��
  COMM_PLAYER_PACKAGE backup_player_pack[FIELD_COMM_MEMBER_MAX];  ///<Backup���W�f�[�^(���W�ϊ������Ă��Ȃ�)
  INTRUDE_TALK_FIRST_ATTACK recv_talk_first_attack;
  INTRUDE_TALK   talk;
  INTRUDE_PROFILE send_profile;   ///<�����v���t�B�[�����M�o�b�t�@(��M��gamedata���Ȃ̂ŕs�v)
  
  MISSION_SYSTEM mission;     ///<�~�b�V�����V�X�e��
  INTRUDE_OCCUPY_RESULT send_occupy_result;     ///<�~�b�V������̐苒����
  INTRUDE_OCCUPY_RESULT recv_occupy_result;     ///<�~�b�V������̐苒���ʎ�M�o�b�t�@
  
  WFBC_COMM_DATA wfbc_comm_data;  ///<WFBC�ʐM���[�N
  
  INTRUDE_SYMBOL_WORK intrude_symbol;        ///<�N����̃V���{�����[�N
  INTRUDE_SYMBOL_WORK intrude_send_symbol;   ///<���M�o�b�t�@�F�V���{�����[�N
  SYMBOL_DATA_REQ req_symbol_data[FIELD_COMM_MEMBER_MAX]; ///<�V���{���f�[�^���N�G�X�g
  SYMBOL_DATA_CHANGE send_symbol_change;     ///<���M�o�b�t�@�F�V���{���f�[�^�ύX
  INTRUDE_SECRET_ITEM_SAVE recv_secret_item;  ///<��M�o�b�t�@�B���A�C�e��
  u8 recv_secret_item_flag:1;                 ///<TRUE:�B���A�C�e������M����
  u8 recv_symbol_flag:1;            ///<TRUE:�V���{���f�[�^����M����
  u8 recv_symbol_change_flag:1;     ///<TRUE:�V���{���f�[�^�̕ύX�ʒm���󂯎����
  u8        :5;
  
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
  u8 new_mission_recv;        ///<TRUE:�V�K�Ƀ~�b�V��������M����
  u8 palace_in;               ///<TRUE:�p���X���ɖK�ꂽ
  u8 warp_player_netid;       ///<���[�v��̃v���C���[NetID
  
  MONOLITH_STATUS monolith_status;  ///<���m���X�X�e�[�^�X
  s16 exit_wait;              ///<�ʐM�ؒf���̃^�C���A�E�g�J�E���^
  u8 monost_req;              ///<���m���X�X�e�[�^�X�v�����N�G�X�g(bit�Ǘ�)
  u8 search_count;
  
  STRBUF *search_child[INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX];
  u32 search_child_trainer_id[INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX];
  u8 search_child_sex[INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX];
  u8 search_child_lang[INTRUDE_BCON_PLAYER_PRINT_SEARCH_MAX];
  
  s8 member_fix;              ///<TRUE:�����֎~
  s8 other_monolith_count;    ///<���l�̃��m���X��ʂ����Ă���v���C���[�l��
  u8 send_occupy_result_send_req; ///<�苒���ʑ��M���N�G�X�g
  u8 member_is_tutorial:1;    //�����ȊO�̒ʐM���肪�`���[�g���A����
  u8 error:1;                 ///<TRUE:�G���[����
  u8 live_comm_status:1;      ///<���C�u�ʐM��ʗp�t���O
  u8 mission_start_event_ended:1; ///<TRUE:�~�b�V�����J�n��ʂ�\������
  u8      :4;
  
  u32 mission_start_timeout:31;  ///<�~�b�V�����J�n�O�܂ł̃^�C���A�E�g�������J�E���g
  u32 mission_start_timeout_warning:1;  ///<�u�~�b�V�����J�n�O�̃^�C���A�E�g�������v�x��������

  u8 player_status_update;    ///<�v���C���[�X�e�[�^�X�X�V���(bit�Ǘ�)
  u8 padding[3];
}INTRUDE_COMM_SYS;


