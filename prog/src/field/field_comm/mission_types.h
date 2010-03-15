//==============================================================================
/**
 * @file    mission_types.h
 * @brief   �~�b�V�����֘A�̒�`
 * @author  matsuda
 * @date    2009.10.27(��)
 */
//==============================================================================
#pragma once

#ifndef __CONVERT_TYPES__
#include "savedata\intrude_save.h"
#endif  //__CONVERT_TYPES__


//==============================================================================
//  �萔��`
//==============================================================================
///�~�b�V�����m�F�^�C�v
typedef enum{
  CONFIRM_NUM,
  CONFIRM_PEOPLE,
}CONFIRM_TYPE;

///�~�b�V�����n��
typedef enum{
  MISSION_TYPE_VICTORY,     ///<����(LV)
  MISSION_TYPE_SKILL,       ///<�Z
  MISSION_TYPE_BASIC,       ///<��b
  MISSION_TYPE_ATTRIBUTE,   ///<����
  MISSION_TYPE_ITEM,        ///<����
  MISSION_TYPE_PERSONALITY, ///<���i

  MISSION_TYPE_MAX,   //��MISSION_LIST_MAX�Ɠ����l�ł���K�v������܂�
  MISSION_TYPE_NONE = MISSION_TYPE_MAX, ///<�~�b�V�������s���Ă��Ȃ�
}MISSION_TYPE;

///�u�~�b�V�����󒍂��܂��v�̕Ԏ�
typedef enum{
  MISSION_ENTRY_RESULT_NULL,    ///<�Ԏ��Ȃ�
  MISSION_ENTRY_RESULT_OK,      ///<��OK
  MISSION_ENTRY_RESULT_NG,      ///<��NG
}MISSION_ENTRY_RESULT;

///�~�b�V�����B���錾�̕Ԏ�
typedef enum{
  MISSION_ACHIEVE_NULL,       ///<�Ԏ�����
  MISSION_ACHIEVE_OK,         ///<�B��OK
  MISSION_ACHIEVE_NG,         ///<�B��NG
}MISSION_ACHIEVE;

///��b�^�C�v
typedef enum{
  TALK_TYPE_CUTE,         ///<�����n�̃|�P����
  TALK_TYPE_FRIGHTENING,  ///<�|���n�̃|�P����
  TALK_TYPE_RETICENCE,    ///<�����ȃ|�P����
  TALK_TYPE_MAN,          ///<��l���F�j
  TALK_TYPE_WOMAN,        ///<��l���F��
  
  TALK_TYPE_MAX,
}TALK_TYPE;

///�~�b�V�����B���ŏオ�郌�x����
#define MISSION_ACHIEVE_ADD_LEVEL   (1)

//==============================================================================
//  �R���o�[�^����o�͂����f�[�^�̍\����
//==============================================================================
///�~�b�V�����f�[�^
typedef struct{
  u8 mission_no;                  ///<�~�b�V�����ԍ�
  u8 type;                        ///<�~�b�V�����n��(MISSION_TYPE_???)
  u8 level;                       ///<�������x��
  u8 odds;                        ///<������

  u16 msg_id_contents;            ///<�~�b�V�������egmm��msg_id
  u16 msg_id_contents_monolith;   ///<�~�b�V�������e���m���Xgmm��msg_id

  u16 obj_id[4];      ///<�ω�OBJ ID
  u8 obj_sex[4];         ///<�ω�OBJ�̐���
  u8 talk_type[4];       ///<�ω�OBJ�̉�b�^�C�v(TALK_TYPE_xxx)

  u16 data[3];        ///<�f�[�^(�~�b�V�����n�����Ɉ������ω�)
  u16 time;           ///<����(�b)

  u16 reward[4];      ///<��V 0(1��) �` 3(4��)

  u8 confirm_type;    ///<�~�b�V�����m�F�^�C�v (CONFIRM_???)
  u8 limit_run;       ///<��������@TRUE:����֎~
  u8 limit_talk;      ///<�����b�@TRUE:�b�֎~
  u8 padding;
}MISSION_CONV_DATA;

///�~�b�V�����f�[�^���烊�X�g�쐬�p�ɍŒ���̃f�[�^�����𒊏o������
typedef struct{
  u8 type;                        ///<�~�b�V�����n��(MISSION_TYPE_???)
  u8 level;                       ///<�������x��
  u8 odds;                        ///<������
  u8 padding;
}MISSION_CONV_DATA_LISTPARAM;

///�p���[�f�[�^
typedef struct{
  u8 level_w;             ///<�K�vWhite���x��
  u8 level_b;             ///<�K�vBlack���x��
  u16 point;              ///<�g�p�|�C���g

  u16 msg_id_title;       ///<�p���[����msg_id
  u16 msg_id_explain;     ///<�p���[����msg_id

  u16 data;               ///<���ʃf�[�^
  u8 type;                ///<���ʌn��
  u8 time;                ///<�p������(��)
}POWER_CONV_DATA;

//--------------------------------------------------------------
//  MISSION_CONV_DATA.data�����̍\����
//--------------------------------------------------------------
///�~�b�V�����n�����ɈقȂ�data�̍\���́FVICTORY
typedef struct{
  u16 battle_level;       ///<�ΐ탌�x��
  u16 padding[2];
}MISSION_TYPEDATA_VICTORY;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FSKILL
typedef struct{
  u16 item;
  u16 padding[2];
}MISSION_TYPEDATA_SKILL;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FBASIC
typedef struct{
  u16 item;
  u16 padding[2];
}MISSION_TYPEDATA_BASIC;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FATTRIBUTE
typedef struct{
  u16 zone_id;          ///<�ړI�n��zone_id
  u16 padding[2];
}MISSION_TYPEDATA_ATTRIBUTE;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FITEM
typedef struct{
  u16 secret_pos_tblno;   ///<�B���A�C�e���ݒu���W�e�[�u���ԍ��ʒu(IntrudeSecretItemPosDataTbl)
  u16 item_no;            ///<�n���A�C�e���ԍ�
  u16 padding;
}MISSION_TYPEDATA_ITEM;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FPERSONALITY
typedef struct{
  u16 num;                ///<�b�����������
  u16 padding[2];
}MISSION_TYPEDATA_PERSONALITY;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�~�b�V�����v���f�[�^
typedef struct{
  u16 zone_id;                ///<�~�b�V�����N���Ɏg�p�����~�j���m���X���������]�[��ID
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding;
}MISSION_REQ;

///�~�b�V�����Ń^�[�Q�b�g�ƂȂ����v���C���[�̏��
typedef struct{
  //�^�[�Q�b�g�ƂȂ�������̃v���C���[��
  //(��M�����q�@�̃��[�J���ł̓v���t�B�[�����܂���M���Ă��Ȃ��A
  //�r�����E���ăv���t�B�[�����������Ƃ��AWORDSET���o���Ȃ��A�Ȃǂ�����ׁA���O�𒼐ڑ���)
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];

  u8 net_id;
  u8 padding[3];
}MISSION_TARGET_INFO;

///�~�b�V�����^�C�v���ɈقȂ�g���p�����[�^
typedef struct{
  union{
    struct{
      u32 work;
    }victory;
    struct{
      u32 work;
    }skill;
    struct{
      u32 work;
    }basic;
    struct{
      u32 work;
    }size;
    struct{
      u16 zone_id;
      u8 padding[2];
    }attr;
    struct{
      u32 work;
    }item;
    struct{
      u32 work;
    }occur;
    struct{
      u32 work;
    }personal;
  };
}MISSION_TYPE_WORK;

///�~�b�V�����f�[�^
typedef struct{
  MISSION_CONV_DATA cdata;       ///<�~�b�V�����f�[�^
  MISSION_TARGET_INFO target_info;  ///<�~�b�V�����^�[�Q�b�g
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 palace_area;             ///<�ǂ̃p���X�G���A�Ŏ󒍂����~�b�V�����Ȃ̂�
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding;
  
  u16 ready_timer;            ///<�J�n�O�̃G���g���[�҂��̎c�莞��(�b)
  u8 padding2[2];
}MISSION_DATA;


#ifndef __CONVERT_TYPES__

///�u�~�b�V�����󒍂��܂��v��e�@�֑��M
typedef struct{
  MISSION_CONV_DATA cdata;
  MISSION_TARGET_INFO target_info;
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding[3];
}MISSION_ENTRY_REQ;

///�u�~�b�V�����󒍂��܂��v�̕Ԏ��̑��M�f�[�^
typedef struct{
  u8 result;                    ///<MISSION_ENTRY_RESULT_???
  u8 padding[3];
  MISSION_DATA mdata;           ///<��OK�̏ꍇ�A���������~�b�V�����f�[�^���Z�b�g
}MISSION_ENTRY_ANSWER;

///�~�b�V��������
typedef struct{
  MISSION_DATA mission_data;  ///<�B�������~�b�V�����f�[�^
  u8 achieve_netid[FIELD_COMM_MEMBER_MAX];  ///<�B���҂�NetID(���ʏ�)
  u8 mission_fail;            ///<�~�b�V�������s���FTRUE
  u8 padding[3];
}MISSION_RESULT;

///�~�b�V�����I����⃊�X�g�\����
typedef struct{
  OCCUPY_INFO occupy;                 ///<���݂̐苒���
  u8 occ;                             ///<TRUE:���X�g���쐬����Ă���
  u8 monolith_type;                   ///<MONOLITH_TYPE_BLACK or MONOLITH_TYPE_WHITE
  u8 accept_netid;                    ///<�~�b�V�����󒍎҂�NetID
  u8 padding;
  MISSION_TARGET_INFO target_info;    ///<�^�[�Q�b�g���
}MISSION_CHOICE_LIST;

///�~�b�V�����V�X�e���\����
typedef struct{
  MISSION_CHOICE_LIST list[FIELD_COMM_MEMBER_MAX];  ///<�I�����̃~�b�V�����ꗗ
  MISSION_DATA data;          ///<���s���Ă���~�b�V����
  MISSION_RESULT result;      ///<�~�b�V��������
  MISSION_ENTRY_ANSWER entry_answer[FIELD_COMM_MEMBER_MAX]; ///<�u�~�b�V������M���܂��v�̕Ԏ�
  u32 start_timer;            ///<�~�b�V�����J�n���̃^�C��(GFL_RTC_GetTimeBySecond)
  
  //�e�����f�[�^
  u8 list_send_req[FIELD_COMM_MEMBER_MAX];  ///<TRUE:�~�b�V�������X�g�̑��M���s��
  u8 result_mission_achieve[FIELD_COMM_MEMBER_MAX];  ///<�~�b�V�����B���錾�̕Ԏ�(MISSION_ACHIEVE_)
  u8 data_send_req;           ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 result_send_req;         ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 send_mission_start;      ///<�������ԏI���B�~�b�V�����J�n�𑗐M(_SEND_MISSION_START_xxx)

  //�q�����f�[�^
  u8 parent_data_recv;        ///<TRUE:�e����~�b�V�����f�[�^����M
  u8 parent_result_recv;      ///<TRUE:�e���猋�ʂ���M
  u8 recv_entry_answer_result;  ///<�~�b�V�����󒍂̕Ԏ�(MISSION_ENTRY_RESULT_???)
  u8 parent_achieve_recv;     ///<�e����~�b�V�����B���񍐂���M(MISSION_ACHIEVE_???)
  u8 mine_entry;              ///<TRUE:�~�b�V�����Q�����Ă���
  u8 mission_complete;        ///<TRUE:�����s���Ă���~�b�V�����͊�������
}MISSION_SYSTEM;

#endif  //__CONVERT_TYPES__

