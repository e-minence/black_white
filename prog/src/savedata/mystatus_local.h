//============================================================================================
/**
 * @file	mystatus_local.h
 * @author	tamada
 * @date	2006.01.27
 * @brief	自分状態データ型
 *
 * セーブデータ関連の共有ヘッダ。外部公開はしない。
 */
//============================================================================================
#ifndef	__MYSTATUS_LOCAL_H__
#define	__MYSTATUS_LOCAL_H__

#include "buflen.h"

//----------------------------------------------------------
/**
 * @brief	自分状態データ型定義
 */
//----------------------------------------------------------
#if 0
struct _MYSTATUS {
	STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];		// 16
	u32 id;											// 20
	u32 gold;										// 24

	u8 sex;	
	u8 region_code;									//26

	u8 badge;										
	u8 trainer_view;	// ユニオンルーム内での見た目フラグ //28
	u8 rom_code;		// 0:ダイヤ  1:パール		// 29

	u8 dp_clear:1;		// DPクリアしているかフラグ
	u8 dp_zenkoku:1;	// DP全国図鑑手に入れているかフラグ
	u8 dummy1:6;									// 30
	u8 dummy2;										// 31
	u8 dummy3;		//金銀用にキープ　2007.08.09(木) matsuda	// 32
};
#else
struct _MYSTATUS {
	STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];		// 16
	u32 id;								// 20
	u32 profileID;				// 24
	u8 region_code;				//PM_LANG   25
	u8 rom_code;	       	// PM_VERSION   26
  u8 sex:2;	   //27
  u8 trainer_view:4;	// ユニオンルーム内での見た目フラグ //27
	u8 dummy1:2;									// 27
	u8 dummybuf[5];										// 32
};
#endif

#endif	/* __MYSTATUS_LOCAL_H__ */
