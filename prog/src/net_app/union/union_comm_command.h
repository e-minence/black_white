//==============================================================================
/**
 * @file    union_comm_command.h
 * @brief   ���j�I���F�ʐM�R�}���h
 * @author  matsuda
 * @date    2009.07.14(��)
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "net/network_define.h"
#include "colosseum_types.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Union_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum UNION_CMD{
  UNION_CMD_START = GFL_NET_CMD_UNION,
  
  UNION_CMD_TALK_SHUTDOWN = UNION_CMD_START,///<��b���̐ؒf
  UNION_CMD_MAINMENU_LIST_RESULT,           ///<���C�����j���[�I������
  UNION_CMD_MAINMENU_LIST_RESULT_ANSWER,    ///<���C�����j���[�I�����ʂ̕Ԏ�
  UNION_CMD_MYSTATUS,                       ///<MYSTATUS
  UNION_CMD_TRAINERCARD_PARAM,              ///<�g���[�i�[�J�[�h���
  UNION_CMD_COLOSSEUM_ENTRY,                ///<�R���V�A���G���g���[
  UNION_CMD_COLOSSEUM_ENTRY_ANSWER,         ///<�R���V�A���G���g���[����
  UNION_CMD_COLOSSEUM_ENTRY_ALL_READY,      ///<�R���V�A���G���g���[�S��������
  UNION_CMD_MINIGAME_ENTRY_REQ,             ///<�~�j�Q�[��������]
  UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_NG,   ///<�~�j�Q�[��������]�̕Ԏ�(NG)
  UNION_CMD_MINIGAME_ENTRY_REQ_ANSWER_OK,   ///<�~�j�Q�[��������]�̕Ԏ�(OK)
  UNION_CMD_MINIGAME_BASIC_STATUS_REQ,      ///<�~�j�Q�[����{���v��
  UNION_CMD_MINIGAME_BASIC_STATUS,          ///<�~�j�Q�[����{���
  UNION_CMD_MINIGAME_MYSTATUS_REQ,          ///<�~�j�Q�[���pMYSTATUS�v��
  UNION_CMD_MINIGAME_MYSTATUS,              ///<�~�j�Q�[���pMYSTATUS
  UNION_CMD_MINIGAME_INTRUDE_READY,         ///<�~�j�Q�[�������̏��������錾
  UNION_CMD_MINIGAME_LEAVE_CHILD,           ///<�~�j�Q�[���Ŏq�@�̒N�������E���܂���

  UNION_CMD_MAX,
  UNION_CMD_NUM = UNION_CMD_MAX - UNION_CMD_START,
};


//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const NetRecvFuncTable Union_CommPacketTbl[];


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BOOL UnionSend_TalkShutdown(void);
extern BOOL UnionSend_MainMenuListResult(u32 select_list);
extern BOOL UnionSend_MainMenuListResultAnswer(BOOL yes_no);
extern BOOL UnionSend_Mystatus(UNION_SYSTEM_PTR unisys);
extern BOOL UnionSend_TrainerCardParam(UNION_SYSTEM_PTR unisys);
extern BOOL UnionSend_ColosseumEntryStatus(COLOSSEUM_BASIC_STATUS *basic_status);
extern BOOL UnionSend_ColosseumEntryAnswer(int send_netid, COMM_ENTRY_ANSWER answer);
extern BOOL UnionSend_ColosseumEntryAllReady(void);
extern BOOL UnionSend_MinigameEntryReq(UNION_SYSTEM_PTR unisys);
extern BOOL UnionSend_MinigameEntryReqAnswerNG(u8 send_bit);
extern BOOL UnionSend_MinigameEntryReqAnswerOK(u8 send_bit);
extern BOOL UnionSend_MinigameBasicStatusReq(void);
extern BOOL UnionSend_MinigameBasicStatus(const UNION_APP_BASIC *app_basic, u8 send_bit);
extern BOOL UnionSend_MinigameMystatusReq(u8 send_bit, const MYSTATUS *myst);
extern BOOL UnionSend_MinigameMystatus(u8 send_bit, const UNION_APP_MY_PARAM *pAppmy);
extern BOOL UnionSend_MinigameIntrudeReady(u8 send_bit, const MYSTATUS *pMyst);
extern BOOL UnionSend_MinigameLeaveChild(u8 send_bit, NetID leave_netid);
