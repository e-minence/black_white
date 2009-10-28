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
  u8 monolith_type;   ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 padding[3];
}MISSION_REQ;

///�~�b�V�����f�[�^
typedef struct{
  u8 monolith_type;           ///<�ΔŃ^�C�v  MONOLITH_TYPE_???
  u8 mission_no;              ///<�~�b�V�����ԍ�(�~�b�V�����������ꍇ��MISSION_NO_NULL)
  u8 accept_netid;            ///<�~�b�V�����󒍎҂�NetID
  u8 target_netid;            ///<�~�b�V�������e�ɂ���ă^�[�Q�b�g�ƂȂ�v���C���[��NetID
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
  u8 data_send_req;           ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 result_send_req;         ///<TRUE:�~�b�V�����f�[�^�̑��M���s��
  u8 parent_data_recv;        ///<TRUE:�e����~�b�V�����f�[�^����M
  u8 parent_result_recv;      ///<TRUE:�e���猋�ʂ���M
  u8 padding;
}MISSION_SYSTEM;

