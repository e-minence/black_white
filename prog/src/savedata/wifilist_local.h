//============================================================================================
/**
 * @file	wifilist_local.h
 * @brief	Wifi�Ƃ������R�[�h���X�g��`�i�u���肠���O���[�v�v�͕ʁAwifi��ł̏��̂݁j
 * @author	mori GAME FREAK inc.
 * @date	2006.03.02
 */
//============================================================================================
#ifndef __WIFILIST_LOCAL_H__
#define __WIFILIST_LOCAL_H__

#include "buflen.h"

typedef struct{
    STRCODE         groupName[BUFLEN_PERSON_NAME];
    STRCODE 		name[BUFLEN_PERSON_NAME];
	u32				id;
	u16		battle_win;
	u16		battle_lose;
	u16     trade_num;
    u16     year;    //�Ō�ɑΐ킵���N���� �܂��̏ꍇ�͂O�O�O  //46
    u8     month;	                             //47								
    u8     day;
    u8     sex;
    u8     unionGra;
	u16		pofin_num;
	u16		minigame_ballslow;
	u16		minigame_balanceball;
	u16		minigame_balloon;
  u8 isIN;        // ���̃f�[�^���L���̏ꍇ1
  u8 dummy;
}WIFI_FRIEND;

struct _WIFI_LIST{
	DWCUserData		my_dwcuser;							
	DWCFriendData	friend_dwc[WIFILIST_FRIEND_MAX];	
	WIFI_FRIEND	friendData[WIFILIST_FRIEND_MAX];		
};



#endif
