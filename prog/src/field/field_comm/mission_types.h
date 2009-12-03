//==============================================================================
/**
 * @file    mission_types.h
 * @brief   �~�b�V�����֘A�̒�`
 * @author  matsuda
 * @date    2009.10.27(��)
 */
//==============================================================================
#pragma once


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
  MISSION_TYPE_SIZE,        ///<�傫��
  MISSION_TYPE_ATTRIBUTE,   ///<����
  MISSION_TYPE_ITEM,        ///<����
  MISSION_TYPE_OCCUR,       ///<����(�G���J�E���g)
  MISSION_TYPE_PERSONALITY, ///<���i
  
  MISSION_TYPE_MAX,
}MISSION_TYPE;


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

  u16 data[3];        ///<�f�[�^(�~�b�V�����n�����Ɉ������ω�)
  u16 time;           ///<����(�b)

  u16 reward[4];      ///<��V 0(1��) �` 3(4��)

  u8  confirm_type;   ///<�~�b�V�����m�F�^�C�v (CONFIRM_???)
  u8 limit_run;       ///<��������@TRUE:����֎~
  u8 limit_talk;      ///<�����b�@TRUE:�b�֎~
  u8 padding;
}MISSION_CONV_DATA;

///�p���[�f�[�^
typedef struct{
  u8 no;                  ///<�p���[�ԍ�
  u8 level_w;             ///<�K�vWhite���x��
  u8 level_b;             ///<�K�vBlack���x��
  u8 time;                ///<�p������(��)

  u16 point;              ///<�g�p�|�C���g
  u16 msg_id_title;       ///<�p���[����msg_id

  u16 msg_id_explain;     ///<�p���[����msg_id
  u8 type;                ///<���ʌn��
  u8 data;                ///<���ʃf�[�^
}POWER_CONV_DATA;

//--------------------------------------------------------------
//  MISSION_CONV_DATA.data�����̍\����
//--------------------------------------------------------------
///�~�b�V�����n�����ɈقȂ�data�̍\���́FVICTORY
typedef struct{
  u16 disguise_objcode;   ///<�ϑ�OBJCODE
  u16 battle_level;       ///<�ΐ탌�x��
  u16 padding;
}MISSION_TYPEDATA_VICTORY;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FSKILL
typedef struct{
  u16 pms_id;            ///<�ȈՉ�b�p�P��ID
  u16 num;               ///<��
  u16 padding;
}MISSION_TYPEDATA_SKILL;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FBASIC
typedef struct{
  u16 padding[3];
}MISSION_TYPEDATA_BASIC;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FATTRIBUTE
typedef struct{
  u16 zone_id;          ///<�ړI�n��zone_id
  u16 padding[2];
}MISSION_TYPEDATA_ATTRIBUTE;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FSIZE
typedef struct{
  u16 padding[3];
}MISSION_TYPEDATA_SIZE;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FITEM
typedef struct{
  u16 item_no;            ///<�n���A�C�e���ԍ�
  u16 padding[2];
}MISSION_TYPEDATA_ITEM;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FOCCUR
typedef struct{
  u16 padding[3];
}MISSION_TYPEDATA_OCCUR;

///�~�b�V�����n�����ɈقȂ�data�̍\���́FPERSONALITY
typedef struct{
  u16 padding[3];
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

///�~�b�V�����f�[�^
#if 0
typedef struct{
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 mission_no;              ///<�~�b�V�����ԍ�(�~�b�V�����������ꍇ��MISSION_NO_NULL)
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 target_netid;            ///<�~�b�V�������e�ɂ���ă^�[�Q�b�g�ƂȂ�v���C���[��NetID
  
  u16 zone_id;                ///<�~�b�V�����N���Ɏg�p�����~�j���m���X���������]�[��ID
  u8 padding[2];
}MISSION_DATA;
#else
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
      MISSION_TARGET_INFO target_info;
    }victory;
    struct{
      u32 work;
    }skill;
    struct{
      MISSION_TARGET_INFO target_info;
    }basic;
    struct{
      u32 work;
    }size;
    struct{
      u16 zone_id;
      u8 padding[2];
    }attr;
    struct{
      MISSION_TARGET_INFO target_info;
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
  MISSION_TYPE_WORK exwork;   ///<�^�C�v���ɈقȂ�g�����[�N
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID

  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 mission_no;              ///<�~�b�V�����ԍ�(�~�b�V�����������ꍇ��MISSION_NO_NULL)
  u8 target_netid;            ///<�~�b�V�������e�ɂ���ă^�[�Q�b�g�ƂȂ�v���C���[��NetID
  
  u16 zone_id;                ///<�~�b�V�����N���Ɏg�p�����~�j���m���X���������]�[��ID
  u8 padding[2];
}MISSION_DATA;
#endif

///�~�b�V��������
typedef struct{
  MISSION_DATA mission_data;  ///<�B�������~�b�V�����f�[�^
  u8 achieve_netid;           ///<�B���҂�NetID
  u8 padding[3];
}MISSION_RESULT;

///�~�b�V�����I����⃊�X�g�\����
typedef struct{
  MISSION_DATA md[MISSION_TYPE_MAX];  ///<�I�����̃~�b�V�����f�[�^
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 palace_area;             ///<���̃��X�g���ǂ̃p���X�G���A�̃��X�g�Ȃ̂�
  u8 padding[2];
}MISSION_CHOICE_LIST;

///�~�b�V�����V�X�e���\����
typedef struct{
  MISSION_CHOICE_LIST list[FIELD_COMM_MEMBER_MAX];  ///<�I�����̃~�b�V�����ꗗ
  MISSION_DATA data;          ///<���s���Ă���~�b�V����
  MISSION_RESULT result;      ///<�~�b�V��������
  u32 timer;                  ///<�~�b�V�������s�܂ł̃^�C�}�[
  u8 list_send_req[FIELD_COMM_MEMBER_MAX];  ///<TRUE:�~�b�V�������X�g�̑��M���s��
  u8 data_send_req;           ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 result_send_req;         ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 parent_data_recv;        ///<TRUE:�e����~�b�V�����f�[�^����M
  u8 parent_result_recv;      ///<TRUE:�e���猋�ʂ���M
  u8 padding;
}MISSION_SYSTEM;

