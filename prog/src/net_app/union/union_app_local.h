//==============================================================================
/**
 * @file    union_app_local.h
 * @brief   ���j�I�����[���ł̃~�j�Q�[������n�̃��[�J���w�b�_
 * @author  matsuda
 * @date    2010.01.08(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �萔��`
//==============================================================================
///�~�j�Q�[���ő�Q���l��
#define UNION_APP_MEMBER_MAX    (5)

//==============================================================================
//  �\���̒�`
//==============================================================================
///��{���F�S�v���C���[���Q�Ƃ���p�����[�^
typedef struct _UNION_APP_BASIC_STATUS{
  u8 member_max;        ///<�Q���ő吔
  u8 member_bit;        ///<�Q�[���ɎQ�����Ă��郆�[�U�[��NetID�Ǘ�(bit�P��)
  u8 padding[2];
}UNION_APP_BASIC_STATUS;

struct _UNION_APP_WORK{
  UNION_APP_BASIC_STATUS basic_status;  ///<��{���
  
  //�e�@��������������p�����[�^
  u8 entry_reserve_bit; ///<�����\��҂�NetID(bit�Ǘ�) �����R�[���o�b�N�������O��member_bit�ɓo�^����Abit��OFF����܂�
  u8 send_entry_answer_ok_bit;  ///<������]�҂ւ̕Ԏ��FOK(bit�Ǘ�)
  u8 send_entry_answer_ng_bit;  ///<������]�҂ւ̕Ԏ��FNG(bit�Ǘ�)
  u8 recv_intrude_ready_bit;    ///<�������������҂�NetID(bit�Ǘ�)
  u8 recv_leave_bit;            ///<���E�҂�NetID(bit�Ǘ�)
  
  BOOL entry_block;             ///<TRUE:�����֎~
  BOOL leave_block;             ///<TRUE:�ޏo�֎~
  BOOL send_leave_req;          ///<TRUE:�ޏo�����N�G�X�g���������Ă���
  UNION_APP_LEAVE recv_leave_req_result;   ///<�ޏo�����N�G�X�g�̕Ԏ�
  BOOL shutdown_req;            ///<TRUE:�ؒf���N�G�X�g
  BOOL shutdown_exe;            ///<TRUE:�ؒf���s��
  BOOL shutdown;                ///<TRUE:�ؒf����
  
  MYSTATUS *mystatus[UNION_APP_MEMBER_MAX];
  
  u8 recv_basic_status_req_bit;   ///<��{���v�����N�G�X�g����(bit�Ǘ�)
  u8 recv_mystatus_req_bit;       ///<MYSTATUS�v�����N�G�X�g����(bit�Ǘ�)
  u8 recv_mystatus_bit;           ///<MYSTATUS��M�ς�(bit�Ǘ�)
  
  //�A�v��������Z�b�g���Ă��炤�p�����[�^
  UNION_APP_CALLBACK_ENTRY_FUNC entry_callback;   ///<�����Ҕ������̃R�[���o�b�N
  UNION_APP_CALLBACK_LEAVE_FUNC leave_callback;   ///<�ޏo�Ҕ������̃R�[���o�b�N
  void *userwork;
};
