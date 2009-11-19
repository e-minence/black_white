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
//  �\���̒�`
//==============================================================================
///�~�b�V�����v���f�[�^
typedef struct{
  u16 zone_id;                ///<�~�b�V�����N���Ɏg�p�����~�j���m���X���������]�[��ID
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding;
}MISSION_REQ;

///�~�b�V�����f�[�^
typedef struct{
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 mission_no;              ///<�~�b�V�����ԍ�(�~�b�V�����������ꍇ��MISSION_NO_NULL)
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 target_netid;            ///<�~�b�V�������e�ɂ���ă^�[�Q�b�g�ƂȂ�v���C���[��NetID
  
  u16 zone_id;                ///<�~�b�V�����N���Ɏg�p�����~�j���m���X���������]�[��ID
  u8 padding[2];
}MISSION_DATA;

///�~�b�V��������
typedef struct{
  MISSION_DATA mission_data;  ///<�B�������~�b�V�����f�[�^
  u8 achieve_netid;           ///<�B���҂�NetID
  u8 padding[3];
}MISSION_RESULT;

///�~�b�V�����V�X�e���\����
typedef struct{
  MISSION_DATA data;          ///<���s���Ă���~�b�V����
  MISSION_RESULT result;      ///<�~�b�V��������
  u32 timer;                  ///<�~�b�V�������s�܂ł̃^�C�}�[
  u8 data_send_req;           ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 result_send_req;         ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 parent_data_recv;        ///<TRUE:�e����~�b�V�����f�[�^����M
  u8 parent_result_recv;      ///<TRUE:�e���猋�ʂ���M
  u8 padding;
}MISSION_SYSTEM;


//--------------------------------------------------------------
//  �R���o�[�^����o�͂����f�[�^
//--------------------------------------------------------------
///�~�b�V�����f�[�^
typedef struct{
  u8 mission_no;                  ///<�~�b�V�����ԍ�
  u8 level;                       ///<�������x��
  u8 type;                        ///<�~�b�V�����n��
  u8 odds;                        ///<������
  u16 msg_id_contents;            ///<�~�b�V�������egmm��msg_id
  u16 msg_id_contents_monolith;   ///<�~�b�V�������e���m���Xgmm��msg_id
  u16 obj_id[4];      ///<�ω�OBJ ID
  u16 data[3];        ///<�f�[�^(�~�b�V�����n�����Ɉ������ω�)
  u16 time;           ///<����(�b)
  u16 reward[4];      ///<��V 0(1��) �` 3(4��)
  u8  confirm_type;   ///<�~�b�V�����m�F�^�C�v
  u8 limit_run;       ///<��������@TRUE:����֎~
  u8 limit_talk;      ///<�����b�@TRUE:�b�֎~
  u8 padding;
}MISSION_CONV_DATA;
