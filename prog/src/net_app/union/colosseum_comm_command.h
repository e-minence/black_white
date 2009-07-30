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
#include "colosseum.h"
#include "net_app/union/union_main.h"
#include "poke_tool/pokeparty.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Colosseum_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum COLOSSEUM_CMD{
  COLOSSEUM_CMD_START = GFL_NET_CMD_COLOSSEUM,
  
  COLOSSEUM_CMD_SHUTDOWN = COLOSSEUM_CMD_START, ///<�ؒf
  COLOSSEUM_CMD_BASIC_STATUS,               ///<��{���
  COLOSSEUM_CMD_TRAINERCARD,                ///<�g���[�i�[�J�[�h���
  COLOSSEUM_CMD_POS_PACKAGE,                ///<���W�p�b�P�[�W
  COLOSSEUM_CMD_STANDPOS_CONFIRM,           ///<�e�Ɏ����̍��̗����ʒu���g���Ă����Ȃ����m�F����
  COLOSSEUM_CMD_ANSWER_STANDPOS,            ///<�����ʒu�g�p���ʂ̕Ԏ�
  COLOSSEUM_CMD_POKEPARTY,                  ///<POKEPARTY��M
  COLOSSEUM_CMD_STANDING_POS,               ///<�S�����̗����ʒu����M
  COLOSSEUM_CMD_BATTLE_READY,               ///<�퓬��������
  COLOSSEUM_CMD_BATTLE_READY_CANCEL,        ///<�퓬�����L�����Z��
  COLOSSEUM_CMD_BATTLE_READY_CANCEL_OK,     ///<�퓬�����L�����Z��OK
  COLOSSEUM_CMD_ALL_BATTLE_READY,           ///<�S���퓬��������
  COLOSSEUM_CMD_LEAVE,                      ///<�ޏo
  
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

extern BOOL ColosseumSend_Shutdown(COMM_PLAYER_PACKAGE *pos_package);
extern BOOL ColosseumSend_EntryStatus(COLOSSEUM_BASIC_STATUS *basic_status);
extern BOOL ColosseumSend_BasicStatus(COLOSSEUM_BASIC_STATUS *basic_status, BOOL parent_only);
extern BOOL ColosseumSend_TrainerCard(TR_CARD_DATA *send_card);
extern BOOL ColosseumSend_PosPackage(COMM_PLAYER_PACKAGE *pos_package);
extern BOOL ColosseumSend_StandingPositionConfirm(COLOSSEUM_SYSTEM_PTR clsys);
extern BOOL ColosseumSend_AnswerStandingPosition(COLOSSEUM_SYSTEM_PTR clsys, int send_net_id, BOOL result);
extern BOOL ColosseumSend_Pokeparty(POKEPARTY *pokeparty);
extern BOOL ColosseumSend_StandingPos(u8 *standing_pos);
extern BOOL ColosseumSend_BattleReady(void);
extern BOOL ColosseumSend_BattleReadyCancel(void);
extern BOOL ColosseumSend_BattleReadyCancelOK(int send_netid);
extern BOOL ColosseumSend_AllBattleReady(void);
extern BOOL ColosseumSend_Leave(void);
extern BOOL ColosseumSend_EntryAnswer(int send_netid, COMM_ENTRY_ANSWER answer);

