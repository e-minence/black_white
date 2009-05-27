//==============================================================================
/**
 * @file    game_beacon_search.h
 * @brief   
 * @author  matsuda
 * @date    2009.05.12(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
///�Q�[���ʐM�Ǘ����[�N�̕s��`�|�C���^
typedef struct _GAME_BEACON_SYS * GAME_BEACON_SYS_PTR;


//==============================================================================
//  �\���̒�`
//==============================================================================
///����M����r�[�R���f�[�^
typedef struct
{
  GameServiceID gsid; ///<Game Service ID
  u8 member_num;      ///<���݂̎Q���l��
  u8 member_max;      ///<�ő�l��
  u8 error;           ///<�G���[��
  u8 padding;
}GBS_BEACON;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void * GameBeacon_Init(int *seq, void *pwk);
extern BOOL GameBeacon_InitWait(int *seq, void *pwk, void *pWork);
extern BOOL GameBeacon_Exit(int *seq, void *pwk, void *pWork);
extern BOOL GameBeacon_ExitWait(int *seq, void *pwk, void *pWork);
extern void GameBeacon_Update(int *seq, void *pwk, void *pWork);

extern int GameBeacon_GetBeaconSize(void *pWork);

