//======================================================================
/**
 * @file  pl_boat_def.h
 * @brief  遊覧船関連定義 スクリプト参照可
 * @author  Saito
 */
//======================================================================

#pragma once

#define PL_TR_SEARCH_TYPE_LEFT  (0)     //船内左側
#define PL_TR_SEARCH_TYPE_RIGHT  (1)    //船内右側
#define PL_TR_SEARCH_TYPE_SINGLE  (2)   //シングルルーム
#define PL_TR_SEARCH_TYPE_DOUBLE  (3)   //ダブルルーム

#define PL_BOAT_ROOM_INFO_OBJ_CODE  (4)
#define PL_BOAT_ROOM_INFO_NPC_TYPE  (5)
#define PL_BOAT_ROOM_INFO_TR_ID     (6)
#define PL_BOAT_ROOM_INFO_MSG       (7)
#define PL_BOAT_ROOM_INFO_BTL_FLG   (8)

//ＮＰＣ種類
#define NPC_TYPE_TRAINER (0)
#define NPC_TYPE_SHORT_TALKER (1)
#define NPC_TYPE_LONG_TALKER  (2)

//戦闘したことあるかチェック用
#define PL_BOAT_TR_BTL_YET (0)      //まだ戦っていない
#define PL_BOAT_TR_BTL_ALREADY (1)  //既に戦った

