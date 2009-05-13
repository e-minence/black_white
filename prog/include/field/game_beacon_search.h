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
  u8 padding[2];
}GBS_BEACON;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void * GameBeacon_Init(void);
extern BOOL GameBeacon_InitWait(void *pWork);
extern void GameBeacon_Exit(void *pWork);
extern BOOL GameBeacon_ExitWait(void *pWork);
extern void GameBeacon_Update(void *pWork);

extern int GameBeacon_GetBeaconSize(void *pWork);

