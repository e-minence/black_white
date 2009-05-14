//==============================================================================
/**
 * @file    game_comm.h
 * @brief   �Q�[���ʐM�Ǘ��̃w�b�_
 * @author  matsuda
 * @date    2009.05.12(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
typedef enum{
  GAME_COMM_NO_NULL,                  ///<�����N�����Ă��Ȃ����
  
  GAME_COMM_NO_FIELD_BEACON_SEARCH,   ///<�t�B�[���h��Ńr�[�R���T�[�`
  GAME_COMM_NO_INVASION,              ///<�N��
}GAME_COMM_NO;


//==============================================================================
//  �^��`
//==============================================================================
///�Q�[���ʐM�Ǘ����[�N�̕s��`�|�C���^
typedef struct _GAME_COMM_SYS * GAME_COMM_SYS_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �V�X�e���g�p    ���ʏ탆�[�U�[�͎g�p���Ȃ�
//--------------------------------------------------------------
extern GAME_COMM_SYS_PTR GameCommSys_Alloc(HEAPID heap_id);
extern void GameCommSys_Free(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_Main(GAME_COMM_SYS_PTR gcsp);

//--------------------------------------------------------------
//  ���[�U�[�g�p
//--------------------------------------------------------------
extern void GameCommSys_Boot(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO comm_game_no);
extern void GameCommSys_ExitReq(GAME_COMM_SYS_PTR gcsp);
extern void GameCommSys_ChangeReq(GAME_COMM_SYS_PTR gcsp, GAME_COMM_NO comm_game_no);
extern GAME_COMM_NO GameCommSys_BootCheck(GAME_COMM_SYS_PTR gcsp);
extern GAME_COMM_NO GameCommSys_GetCommGameNo(GAME_COMM_SYS_PTR gcsp);
extern BOOL GameCommSys_CheckSystemWaiting(GAME_COMM_SYS_PTR gcsp);
extern void *GameCommSys_GetAppWork(GAME_COMM_SYS_PTR gcsp);


