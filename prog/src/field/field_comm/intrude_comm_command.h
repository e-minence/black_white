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
#include "field\field_wfbc_data.h"


//==============================================================================
//  �萔��`
//==============================================================================

///���j�I����M�R�}���h�ԍ��@�@��Union_CommPacketTbl�ƕ��т𓯂��ɂ��Ă������ƁI�I
enum INTRUDE_CMD{
  INTRUDE_CMD_START = GFL_NET_CMD_PALACE,
  
  INTRUDE_CMD_SHUTDOWN = INTRUDE_CMD_START,   ///<�ؒf
  INTRUDE_CMD_MEMBER_NUM,                     ///<�N���Q���l��
  INTRUDE_CMD_PROFILE_REQ,                    ///<�v���t�B�[���v��
  INTRUDE_CMD_PROFILE,                        ///<�v���t�B�[���f�[�^
  INTRUDE_CMD_DELETE_PROFILE,                 ///<���E�҂̃v���t�B�[���폜
  INTRUDE_CMD_PLAYER_STATUS,                  ///<�N���X�e�[�^�X(���ݏ��)
  INTRUDE_CMD_TALK,                           ///<�b��������
  INTRUDE_CMD_TALK_ANSWER,                    ///<�b��������ꂽ�̂Ŏ����̏󋵂�Ԃ�
  INTRUDE_CMD_TALK_CANCEL,                    ///<�b���������L�����Z��
  INTRUDE_CMD_BINGO_INTRUSION,                ///<�r���S�o�g���������v��
  INTRUDE_CMD_BINGO_INTRUSION_ANSWER,         ///<�r���S�o�g���������v���̕Ԏ�
  INTRUDE_CMD_REQ_BINGO_INTRUSION_PARAM,      ///<�r���S�o�g�������p�̃p�����[�^�v��
  INTRUDE_CMD_BINGO_INTRUSION_PARAM,          ///<�r���S�o�g�������p�̃p�����[�^
  INTRUDE_CMD_MISSION_LIST_REQ,               ///<�~�b�V�����I����⃊�X�g�v�����N�G�X�g
  INTRUDE_CMD_MISSION_LIST,                   ///<�~�b�V�����I����⃊�X�g
  INTRUDE_CMD_MISSION_ORDER_CONFIRM,          ///<�~�b�V�����󒍂��܂�
  INTRUDE_CMD_MISSION_ENTRY_ANSWER,           ///<�u�~�b�V�����󒍂��܂��v�̕Ԏ�
  INTRUDE_CMD_MISSION_REQ,                    ///<�~�b�V�����v�����N�G�X�g
  INTRUDE_CMD_MISSION_DATA,                   ///<�~�b�V�����f�[�^
  INTRUDE_CMD_MISSION_START,                  ///<�~�b�V�����J�n
  INTRUDE_CMD_MISSION_ACHIEVE,                ///<�~�b�V�����B����
  INTRUDE_CMD_MISSION_ACHIEVE_ANSWER,         ///<�~�b�V�����B���񍐂̕Ԏ�
  INTRUDE_CMD_MISSION_RESULT,                 ///<�~�b�V��������
  INTRUDE_CMD_OCCUPY_INFO,                    ///<�苒���
  INTRUDE_CMD_TARGET_TIMING,                  ///<����w��̃^�C�~���O�R�}���h
  INTRUDE_CMD_PLAYER_SUPPORT,                 ///<�ʐM������T�|�[�g
  INTRUDE_CMD_SECRET_ITEM,                    ///<�B���A�C�e��
  INTRUDE_CMD_GPOWER_EQUIP,                   ///<G�p���[�𑕔����܂���
  INTRUDE_CMD_WFBC_REQ,                       ///<WFBC�p�����[�^�v��
  INTRUDE_CMD_WFBC,                           ///<WFBC�p�����[�^
  INTRUDE_CMD_WFBC_NPC_ANS,                   ///<WFBC�p�����[�^
  INTRUDE_CMD_WFBC_NPC_REQ,                   ///<WFBC�p�����[�^
  INTRUDE_CMD_SYMBOL_DATA_REQ,                ///<�V���{���G���J�E���g�f�[�^��v��
  INTRUDE_CMD_SYMBOL_DATA,                    ///<�V���{���G���J�E���g�f�[�^
  INTRUDE_CMD_SYMBOL_DATA_CHANGE,             ///<�����̃V���{���G���J�E���g�f�[�^�ύX�ʒm
  
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
extern BOOL IntrudeSend_Shutdown(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL IntrudeSend_MemberNum(INTRUDE_COMM_SYS_PTR intcomm, u8 member_num);
extern BOOL IntrudeSend_ProfileReq(void);
extern BOOL IntrudeSend_Profile(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL IntrudeSend_DeleteProfile(INTRUDE_COMM_SYS_PTR intcomm, int leave_netid);
extern BOOL IntrudeSend_PlayerStatus(INTRUDE_COMM_SYS_PTR intcomm, INTRUDE_STATUS *send_status);
extern BOOL IntrudeSend_Talk(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id, const MISSION_DATA *mdata, INTRUDE_TALK_TYPE intrude_talk_type);
extern BOOL IntrudeSend_TalkAnswer(INTRUDE_COMM_SYS_PTR intcomm, int send_net_id, INTRUDE_TALK_STATUS answer);
extern BOOL IntrudeSend_TalkCancel(int send_net_id);
extern BOOL IntrudeSend_BingoIntrusion(int send_net_id);
extern BOOL IntrudeSend_BingoIntrusionAnswer(int send_net_id, BINGO_INTRUSION_ANSWER answer);
extern BOOL IntrudeSend_ReqBingoBattleIntrusionParam(int send_net_id);
extern BOOL IntrudeSend_BingoBattleIntrusionParam(BINGO_SYSTEM *bingo, int send_net_id);
extern BOOL IntrudeSend_MissionListReq(INTRUDE_COMM_SYS_PTR intcomm, u32 palace_area);
extern BOOL IntrudeSend_MissionList(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission, int palace_area);
extern BOOL IntrudeSend_MissionOrderConfirm(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_ENTRY_REQ *entry_req);
extern BOOL IntrudeSend_MissionEntryAnswer(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_ENTRY_ANSWER *entry_answer, int send_netid);
extern BOOL IntrudeSend_MissionReq(INTRUDE_COMM_SYS_PTR intcomm, int monolith_type, u16 zone_id);
extern BOOL IntrudeSend_MissionData(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern BOOL IntrudeSend_MissionStart(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern BOOL IntrudeSend_MissionAchieve(
  INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern BOOL IntrudeSend_MissionAchieveAnswer(INTRUDE_COMM_SYS_PTR intcomm, 
  MISSION_ACHIEVE achieve, int send_netid);
extern BOOL IntrudeSend_MissionResult(INTRUDE_COMM_SYS_PTR intcomm, const MISSION_SYSTEM *mission);
extern BOOL IntrudeSend_OccupyInfo(INTRUDE_COMM_SYS_PTR intcomm);
extern BOOL IntrudeSend_TargetTiming(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, u8 timing_no);
extern BOOL IntrudeSend_PlayerSupport(
  INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, SUPPORT_TYPE support_type);
extern BOOL IntrudeSend_SecretItem(NetID send_netid, const INTRUDE_SECRET_ITEM_SAVE *itemdata);
extern BOOL IntrudeSend_GPowerEquip(u8 palace_area);
extern BOOL IntrudeSend_WfbcReq(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid);
extern BOOL IntrudeSend_Wfbc(INTRUDE_COMM_SYS_PTR intcomm, u32 send_netid_bit, const FIELD_WFBC_CORE *wfbc_core);
extern BOOL IntrudeSend_WfbcNpcAns(const FIELD_WFBC_COMM_NPC_ANS *npc_ans, NetID send_netid);
extern BOOL IntrudeSend_WfbcNpcReq(const FIELD_WFBC_COMM_NPC_REQ *npc_req, NetID send_netid);
extern BOOL IntrudeSend_SymbolDataReq(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid, const SYMBOL_DATA_REQ *p_sdr);
extern BOOL IntrudeSend_SymbolData(INTRUDE_COMM_SYS_PTR intcomm, NetID send_netid);
extern BOOL IntrudeSend_SymbolDataChange(const SYMBOL_DATA_CHANGE *p_sdc);

