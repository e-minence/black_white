//======================================================================
/**
 * @file	ctvt_beacon.c
 * @brief	通信トランシーバー：ビーコン
 * @author	ariizumi
 * @data	09/12/22
 *
 * モジュール名：CTVT_BCON
 */
//======================================================================
#include "buflen.h"
#include "savedata/wifilist.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define CTVT_COMM_NAME_LEN (SAVELEN_PLAYER_NAME+EOM_SIZE)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
//ビーコンデータ
typedef struct _CTVT_COMM_BEACON CTVT_COMM_BEACON;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//自身が呼ばれているかチェック
const BOOL CTVT_BCON_CheckCallSelf( CTVT_COMM_BEACON *beacon , WIFI_LIST *wifiList , u8* selfMacAdr );

//ビーコンから情報取得(MyStatusのラッパー
const STRCODE* CTVT_BCON_GetName( CTVT_COMM_BEACON *beacon );
const u16 CTVT_BCON_GetIDLow( CTVT_COMM_BEACON *beacon );
