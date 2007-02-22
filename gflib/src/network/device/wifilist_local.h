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

#include "strbuf.h"


struct _WIFI_LIST{
	DWCUserData		my_dwcuser;							// 64
	DWCFriendData	friend_dwc[WIFILIST_FRIEND_MAX];	// 64+12*32 = 448
};



#endif