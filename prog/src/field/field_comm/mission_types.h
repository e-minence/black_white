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
  MISSION_ENTRY_RESULT_NG_TARGET_REVERSE,  ///<�^�[�Q�b�g�����t�B�[���h�֗��Ă���
  MISSION_ENTRY_RESULT_NG_TWIN, ///<��l��p�~�b�V�����̏����𖞂����Ă��Ȃ�
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

///�p���[�K�v���x������
enum{
  POWER_LEVEL_DISTRIBUTION = 0xfe,    ///<�z�z�p���[������
  POWER_LEVEL_PALACE = 0xff,          ///<�p���X�p�p���[������
};

///�~�b�V�����f�[�^�őI�o�\�ȃo�[�W����
typedef enum{
  MISSION_DATA_VERSION_WHITE = 1 << 0,
  MISSION_DATA_VERSION_BLACK = 1 << 1,
  MISSION_DATA_VERSION_NEXT = 1 << 2,
}MISSION_DATA_VERSION;

///�~�b�V�����B���ŏオ�郌�x����
#define MISSION_ACHIEVE_ADD_LEVEL   (1)

///�~�b�V�����B���҂����邩�ǂ����̌���
typedef enum{
  MISSION_ACHIEVE_USER_NULL,    ///<�Ԏ��Ȃ�
  MISSION_ACHIEVE_USER_USE,     ///<�B���҂�����
  MISSION_ACHIEVE_USER_NONE,    ///<�B���҂͂��Ȃ�
}MISSION_ACHIEVE_USER;

//==============================================================================
//  �R���o�[�^����o�͂����f�[�^�̍\����
//==============================================================================
///�~�b�V�����f�[�^
typedef struct{
  u32 level:8;                       ///<�������x��
  u32 odds:7;                        ///<������
  u32 reward:7;                      ///<��V
  u32 time:10;                       ///<����(�b)
  
  u32 type:4;                        ///<�~�b�V�����n��(MISSION_TYPE_???)
  u32 version_bit:3;                 ///<�I�o�\�ȃo�[�W����(MISSION_DATA_VERSION)
  u32 msg_id_contents:11;            ///<�~�b�V�������egmm��msg_id
  u32 msg_id_contents_monolith:11;   ///<�~�b�V�������e���m���Xgmm��msg_id
  u8 obj_sex_bit:3;                  ///<�ω�OBJ�̐���(bit�P��) 0bit:NetID0 1bit:NetID1...
  
  u16 obj_id[3];                     ///<�ω�OBJ ID
  u8 talk_type[3];                   ///<�ω�OBJ�̉�b�^�C�v(TALK_TYPE_xxx)

  u16 data[2];                       ///<�f�[�^(�~�b�V�����n�����Ɉ������ω�)
}MISSION_CONV_DATA;

///�~�b�V�����f�[�^���烊�X�g�쐬�p�ɍŒ���̃f�[�^�����𒊏o������
typedef struct{
  u8 type;                        ///<�~�b�V�����n��(MISSION_TYPE_???)
  u8 level;                       ///<�������x��
  u8 odds;                        ///<������
  u8 version_bit;                 ///<�I�o�\�ȃo�[�W����(MISSION_DATA_VERSION)
}MISSION_CONV_DATA_LISTPARAM;

///�p���[�f�[�^
typedef struct{
  u8 level_w;             ///<�K�vWhite���x��(�z�zor�p���X�̏ꍇ��POWER_LEVEL_xxx)
  u8 level_b;             ///<�K�vBlack���x��(�z�zor�p���X�̏ꍇ��POWER_LEVEL_xxx)
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
  u16 gpower_id;
  u16 padding[2];
}MISSION_TYPEDATA_SKILL;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FBASIC
typedef struct{
  u16 gpower_id;
  u16 padding[2];
}MISSION_TYPEDATA_BASIC;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FATTRIBUTE
typedef struct{
  u16 item_no;
  u16 price;
  u16 padding;
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
  u8 sex:1;
  u8    :7;
  u8 padding[2];
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

///�~�b�V�����f�[�^�Ɋ܂܂�邪�A�������̃`�F�b�N����͏����p�����[�^
///(�~�b�V�����J�n��A�ς��Ă����l)
typedef struct{
  u16 ready_timer;            ///<�J�n�O�̃G���g���[�҂��̎c�莞��(�b)
  u16 exe_timer;              ///<�~�b�V�����̏I������
}MISSION_VARIABLE_PARAM;

///�~�b�V�����f�[�^
typedef struct{
  MISSION_CONV_DATA cdata;       ///<�~�b�V�����f�[�^
  MISSION_TARGET_INFO target_info;  ///<�~�b�V�����^�[�Q�b�g
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 palace_area;             ///<�ǂ̃p���X�G���A�Ŏ󒍂����~�b�V�����Ȃ̂�
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding;
  
  //�������`�F�b�N����Ȃ��ׁA�\���̂̍Ō�ɔz�u���鎖�I
  MISSION_VARIABLE_PARAM variable;
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
  u8 complete;                ///<�S�苒�B�����FTRUE
  u8 padding[2];
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
  u8 start_client_bit;        ///<�q����u�~�b�V�����J�n�錾�v����M����(bit�Ǘ�)
  u8 send_start_client_bit;   ///<�q�ւ́u�~�b�V�����J�n�錾�v�̕Ԏ����Mbit
  u8 send_answer_achieve_check_usenone_bit; ///<�q�ւ́u�~�b�V�����B���҂��܂���v�̕Ԏ�
  u8 send_answer_achieve_check_use_bit;     ///<�q�ւ́u�~�b�V�����B���҂��܂��v�̕Ԏ�
  
  //�q�����f�[�^
  u8 parent_data_recv;        ///<TRUE:�e����~�b�V�����f�[�^����M
  u8 parent_result_recv;      ///<TRUE:�e���猋�ʂ���M
  u8 recv_entry_answer_result;  ///<�~�b�V�����󒍂̕Ԏ�(MISSION_ENTRY_RESULT_???)
  u8 parent_achieve_recv;     ///<�e����~�b�V�����B���񍐂���M(MISSION_ACHIEVE_???)
  u8 recv_result:1;           ///<TRUE:�~�b�V�����̌��ʂ���M����
  u8 mission_start_ok:1;      ///<TRUE:�e����u�~�b�V�����J�n�錾�v�̕ԓ����󂯎����
  u8 mine_entry:1;            ///<TRUE:�~�b�V�����Q�����Ă���
  u8 mission_complete:1;      ///<TRUE:�����s���Ă���~�b�V�����͊�������
  u8 mission_timeend_success:1;   ///<TRUE:�~�b�V�����̐������ԂɒB����
  u8 recv_answer_achieve_check_use:2; ///<MISSION_ACHIEVE_USER
  u8        :1;
}MISSION_SYSTEM;

#endif  //__CONVERT_TYPES__

