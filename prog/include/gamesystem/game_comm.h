//==============================================================================
/**
 * @file    game_comm.h
 * @brief   �Q�[���ʐM�Ǘ��̃w�b�_
 * @author  matsuda
 * @date    2009.05.12(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_data.h"


//==============================================================================
//  �萔��`
//==============================================================================
///���s���Ă���ʐM���[�h
typedef enum{
  GAME_COMM_STATUS_NULL,              ///<�������Ă��Ȃ�
  
  GAME_COMM_STATUS_WIRELESS,          ///<���C�����X�ʐM
  GAME_COMM_STATUS_WIFI,              ///<Wi-Fi�ʐM
  GAME_COMM_STATUS_IRC,               ///<�ԊO���ʐM
}GAME_COMM_STATUS;


///���s���Ă���Q�[�����
typedef enum{
  GAME_COMM_NO_NULL,                  ///<�����N�����Ă��Ȃ����
  
  GAME_COMM_NO_FIELD_BEACON_SEARCH,   ///<�t�B�[���h��Ńr�[�R���T�[�`
  GAME_COMM_NO_INVASION,              ///<�N��
  GAME_COMM_NO_UNION,                 ///<���j�I�����[��
}GAME_COMM_NO;


///�C���t�H���[�V�������b�Z�[�W����wordset�ő吔
#define INFO_WORDSET_MAX      (4)

//==============================================================================
//  �^��`
//==============================================================================
///�Q�[���ʐM�Ǘ����[�N�̕s��`�|�C���^
typedef struct _GAME_COMM_SYS * GAME_COMM_SYS_PTR;


//==============================================================================
//  �\���̒�`
//==============================================================================
///�C���t�H���[�V�������b�Z�[�W�\����(�L���[���擾���鎞�A���̍\���̂ɕϊ����Ď擾����)
typedef struct{
  STRBUF *name[INFO_WORDSET_MAX];     ///<���g�p�̏ꍇ��NULL�������Ă��܂�
  u8 wordset_no[INFO_WORDSET_MAX];
  u16 message_id;
  u8 padding[2];
}GAME_COMM_INFO_MESSAGE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �V�X�e���g�p    ���ʏ탆�[�U�[�͎g�p���Ȃ�
//--------------------------------------------------------------
extern GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id, GAMEDATA *gamedata);
extern void GameCommSys_Free(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_Main(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_Callback_FieldCreate(GAME_COMM_SYS_PTR gcsp, void *fieldWork);
extern void GameCommSys_Callback_FieldDelete(GAME_COMM_SYS_PTR gcsp, void *fieldWork);

//--------------------------------------------------------------
//  ���[�U�[�g�p
//--------------------------------------------------------------
extern void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work);
extern void GameCommSys_ExitReq(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO game_comm_no, void *parent_work);
extern GAME_COMM_STATUS GameCommSys_GetCommStatus(GAME_COMM_SYS_PTR gcsp);
extern GAME_COMM_NO GameCommSys_BootCheck(GAME_COMM_SYS_PTR gcsp);
extern GAME_COMM_NO GameCommSys_GetCommGameNo(GAME_COMM_SYS_PTR gcsp);
extern BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp);
extern void *GameCommSys_GetAppWork(GAME_COMM_SYS_PTR gcsp);
extern GAMEDATA * GameCommSys_GetGameData(GAME_COMM_SYS_PTR gcsp);
extern void GameCommStatus_SetPlayerStatus(GAME_COMM_SYS_PTR gcsp, int comm_net_id, ZONEID zone_id, u8 invasion_netid, u8 mission_no);
extern u8 GameCommStatus_GetPlayerStatus_MissionNo(GAME_COMM_SYS_PTR gcsp, int comm_net_id);
extern u8 GameCommStatus_GetPlayerStatus_InvasionNetID(GAME_COMM_SYS_PTR gcsp, int comm_net_id);
extern BOOL GameCommInfo_GetMessage(GAME_COMM_SYS_PTR gcsp, GAME_COMM_INFO_MESSAGE *dest_msg);


