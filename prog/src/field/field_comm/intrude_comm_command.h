//==============================================================================
/**
 * @file    intrude_comm_command.h
 * @brief   �N���F�ʐM�R�}���h
 * @author  matsuda
 * @date    2009.09.03(��)
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "net/network_define.h"
#include "intrude_types.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Union_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum INTRUDE_CMD{
  INTRUDE_CMD_START = GFL_NET_CMD_FIELD,
  
  INTRUDE_CMD_SHUTDOWN = INTRUDE_CMD_START,   ///<�ؒf
  INTRUDE_CMD_MEMBER_NUM,                     ///<�N���Q���l��
  INTRUDE_CMD_PROFILE_REQ,                    ///<�v���t�B�[���v��
  INTRUDE_CMD_PROFILE,                        ///<�v���t�B�[���f�[�^
  INTRUDE_CMD_PLAYER_STATUS,                  ///<�N���X�e�[�^�X(���ݏ��)
  INTRUDE_CMD_TALK,                           ///<�b��������
  INTRUDE_CMD_TALK_ANSWER,                    ///<�b��������ꂽ�̂Ŏ����̏󋵂�Ԃ�
  INTRUDE_CMD_BINGO_INTRUSION,                ///<�r���S�o�g���������v��
  INTRUDE_CMD_BINGO_INTRUSION_ANSWER,         ///<�r���S�o�g���������v���̕Ԏ�
  INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM,      ///<�r���S�o�g�������p�̃p�����[�^�v��
  INTRUDE_CMD_BINGO_INTRUSION_PARAM,          ///<�r���S�o�g�������p�̃p�����[�^
  
  INTRUDE_CMD_MAX,
  INTRUDE_CMD_NUM = INTRUDE_CMD_MAX - INTRUDE_CMD_START,
};


//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const NetRecvFuncTable Intrude_CommPacketTbl[];


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL IntrudeSend_Shutdown(void);
extern BOOL IntrudeSend_MemberNum(INTRUDE_COMM_SYS_PTR intcomm, u8 member_num);
extern BOOL IntrudeSend_ProfileReq(void);
extern BOOL IntrudeSend_Profile(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL IntrudeSend_PlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_STATUS *send_status);
extern BOOL IntrudeSend_Talk(int send_net_id);
extern BOOL IntrudeSend_TalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id, INTRUDE_TALK_STATUS answer);
extern BOOL IntrudeSend_BingoIntrusion(int send_net_id);
extern BOOL IntrudeSend_BingoIntrusionAnswer(int send_net_id, BINGO_INTRUSION_ANSWER answer);
extern BOOL IntrudeSend_ReqBingoBattleIntrusionParam(int send_net_id);
extern BOOL IntrudeSend_BingoBattleIntrusionParam(BINGO_SYSTEM *bingo, int send_net_id);

