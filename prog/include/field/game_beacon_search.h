//==============================================================================
/**
 * @file    game_beacon_search.h
 * @brief   
 * @author  matsuda
 * @date    2009.05.12(火)
 */
//==============================================================================
#pragma once

#include "buflen.h"

//==============================================================================
//  型定義
//==============================================================================
///ゲーム通信管理ワークの不定形ポインタ
typedef struct _GAME_BEACON_SYS * GAME_BEACON_SYS_PTR;

enum
{
  GBS_BEACONN_TYPE_NONE,
  GBS_BEACONN_TYPE_PLACE,
  GBS_BEACONN_TYPE_MESSAGE,

  GBS_BEACONN_TYPE_MAX,
};

#define BEACON_MESSAGE_DATA_NAME_LENGTH (PERSON_NAME_SIZE+EOM_SIZE)
#define BEACON_MESSAGE_DATA_WORD_NUM (4)

//==============================================================================
//  構造体定義
//==============================================================================
///送受信するビーコンデータ
typedef struct
{
  GameServiceID gsid; ///<Game Service ID
  u8 member_num;      ///<現在の参加人数
  u8 member_max;      ///<最大人数
  u8 error;           ///<エラー状況
  u8 beacon_type;     ///<ビーコンの種類 GBS_BEACONN_TYPE
  union
  {
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
//  外部関数宣言
//==============================================================================
extern void * GameBeacon_Init(int *seq, void *pwk);
extern BOOL GameBeacon_InitWait(int *seq, void *pwk, void *pWork);
extern BOOL GameBeacon_Exit(int *seq, void *pwk, void *pWork);
extern BOOL GameBeacon_ExitWait(int *seq, void *pwk, void *pWork);
extern void GameBeacon_Update(int *seq, void *pwk, void *pWork);

extern int GameBeacon_GetBeaconSize(void *pWork);

