//==============================================================================
/**
 * @file    colosseum_comm_command.h
 * @brief   �R���V�A���F�ʐM�R�}���h
 * @author  matsuda
 * @date    2009.07.14(��)
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "net/network_define.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Colosseum_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum COLOSSEUM_CMD{
  COLOSSEUM_CMD_START = GFL_NET_CMD_COLOSSEUM,
  
  COLOSSEUM_CMD_SHUTDOWN = COLOSSEUM_CMD_START,     ///<�ؒf
  COLOSSEUM_CMD_MAINMENU_LIST_RESULT,           ///<���C�����j���[�I������
  COLOSSEUM_CMD_MAINMENU_LIST_RESULT_ANSWER,    ///<���C�����j���[�I�����ʂ̕Ԏ�
  COLOSSEUM_CMD_TRAINERCARD_PARAM,              ///<�g���[�i�[�J�[�h���
  
  COLOSSEUM_CMD_MAX,
  COLOSSEUM_CMD_NUM = COLOSSEUM_CMD_MAX - COLOSSEUM_CMD_START,
};


//==============================================================================
//  �O���f�[�^
//==============================================================================
extern const NetRecvFuncTable Colosseum_CommPacketTbl[];


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void Colosseum_AddCommandTable(UNION_SYSTEM_PTR unisys);
extern void Colosseum_DelCommandTable(void);

extern BOOL ColosseumSend_MainMenuListResult(u32 select_list);
extern BOOL ColosseumSend_MainMenuListResultAnswer(BOOL yes_no);
extern BOOL ColosseumSend_TrainerCardParam(UNION_SYSTEM_PTR unisys);
