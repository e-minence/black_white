//==============================================================================
/**
 * @file    game_beacon_search.h
 * @brief   
 * @author  matsuda
 * @date    2009.05.12(��)
 */
//==============================================================================
#pragma once

#include "buflen.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"

//==============================================================================
//  �^��`
//==============================================================================
///�Q�[���ʐM�Ǘ����[�N�̕s��`�|�C���^
typedef struct _GAME_BEACON_SYS * GAME_BEACON_SYS_PTR;

enum
{
  GBS_BEACONN_TYPE_NONE,
  GBS_BEACONN_TYPE_PALACE,        ///<�p���X�̃r�[�R��
  GBS_BEACONN_TYPE_INFO,          ///<�C���t�H���[�V����
  GBS_BEACONN_TYPE_MESSAGE,       ///<���b�Z�[�W�̃r�[�R��

  GBS_BEACONN_TYPE_MAX,
};

#define BEACON_MESSAGE_DATA_NAME_LENGTH (PERSON_NAME_SIZE+EOM_SIZE)
#define BEACON_MESSAGE_DATA_WORD_NUM (4)

//==============================================================================
//  �\���̒�`
//==============================================================================
///����M����r�[�R���f�[�^
typedef struct
{
  GameServiceID gsid; ///<Game Service ID
  
  u8 member_num:4;      ///<���݂̎Q���l��
  u8 member_max:4;      ///<�ő�l��
  
  u8 error:1;           ///<�G���[��
  u8 beacon_type:5;     ///<�r�[�R���̎�� GBS_BEACONN_TYPE
  u8 restrictPhoto:1;   ///<TRUE:�ʐ^�f�[�^�����ɐ������������Ă���
  u8 isTwl:1;           ///<TRUE:TWL���[�h�œ��삵�Ă���
  
  u8 padding;
  
  union
  {
    GAMEBEACON_INFO info;
    struct MESSAGE_DATA
    {
      u16 word[BEACON_MESSAGE_DATA_WORD_NUM];
      STRCODE name[BEACON_MESSAGE_DATA_NAME_LENGTH];  //8
      
      u8  msgCnt;
      u8  pad[3];
    }bmData;
    struct DUMMY_DATA
    {
      u32 data[8];
    }dummyData;
  };
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

