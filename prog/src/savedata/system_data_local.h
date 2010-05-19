//============================================================================================
/**
 * @file	  system_data_local.h
 * @brief	  未分類セーブデータの構造体定義
 * @author	k.ohnop
 * @date	  2009.12.05
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/system_data.h"
#include "savedata/save_tbl.h"
#include "savedata/gametime.h"


//============================================================================================
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	SYSTEMDATA構造体の定義
 *
 */
//---------------------------------------------------------------------------
struct SYSTEMDATA {
	s64 RTCOffset;			///<DS本体のRTCオフセット情報（RTCを操作した値）
	u8 MacAddress[6];		///<DS本体のMACアドレス
	u8 birth_month;			///<DS本体にプレイヤーが入力した誕生月
	u8 birth_day;			///<DS本体にプレイヤーが入力した誕生日
	GMTIME gametime;		///<ゲーム内時間データへのポインタ
	u8 WiFiMPOpenFlag;		///<Wi-Fi不思議な贈り物オープンフラグ
	u8 BeaconMPOpenFlag;	///<ビーコン不思議な贈り物オープンフラグ
	s32 dummy_profileId;			///< GTS・Wifiバトルタワーログイン用GameSpyID（初めて取得したGameSpyIdを保存する） 使われていない
	u32 reserve[3];			///<予約領域
};



