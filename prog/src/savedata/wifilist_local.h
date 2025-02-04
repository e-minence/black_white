//============================================================================================
/**
 * @file	wifilist_local.h
 * @brief	Wifiともだちコードリスト定義（「しりあいグループ」は別、wifi上での情報のみ）
 * @author	mori GAME FREAK inc.
 * @date	2006.03.02
 */
//============================================================================================
#pragma once

#include "buflen.h"

typedef struct{
  STRCODE 		name[PERSON_NAME_SIZE+EOM_SIZE];
  u32 dummy1;
  u32 dummy2;
  u32 ProfileID;    ///4
  u32  GameSyncID;   ///4
	u32				id;
	u16		battle_win;
	u16		battle_lose;
	u16     trade_num;
  u16     year;    //最後に対戦した年月日 まだの場合は０００  //46
  u8     month;	                             //47								
  u8     day;
  u8     unionGra;
  u8     sex:2;
  u8 isIN:1;        // このデータが有効の場合1
  u8 dummy:5;
}WIFI_FRIEND;

struct _WIFI_LIST{
	DWCUserData		my_dwcuser;							
	DWCFriendData	friend_dwc[WIFILIST_FRIEND_MAX];	
	WIFI_FRIEND	friendData[WIFILIST_FRIEND_MAX];
  u16 dummy;
  u16 crc;
};



