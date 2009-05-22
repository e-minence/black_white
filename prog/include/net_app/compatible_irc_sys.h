//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		compatible_irc_sys.h
 *	@brief	相性診断ゲーム用赤外線接続システム
 *	@author	Toru=Nagihashi
 *	@date		2009.05.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "net\network_define.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define COMPATIBLE_IRC_CONNEXT_NUM		(2)     // 最大接続人数
#define COMPATIBLE_IRC_SENDDATA_BYTE  (32)    // 最大送信バイト数
#define COMPATIBLE_IRC_GETBEACON_NUM	(16)    // 最大ビーコン収集数

//-------------------------------------
///	タイミングNOの重複は許されないため、
//	必要あるたび増やしていく。
//	255まで
//	ただし、メニューから戻ってきて再度タイミングを
//	行ったときに通過してしまうことに注意
//	（ネット接続中の同じタイミングNOでの同期は
//	一度OKになってしまえば、あとは通過してしまう）
//=====================================
typedef enum
{	
	COMPATIBLE_TIMING_NO_MENU_START,
}COMPATIBLE_TIMING_NO;

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
typedef struct _COMPATIBLE_IRC_SYS COMPATIBLE_IRC_SYS;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//IRC_COMPATIBLEのみ使用
extern COMPATIBLE_IRC_SYS * COMPATIBLE_IRC_CreateSystem( u32 irc_timeout, HEAPID heapID );
extern void COMPATIBLE_IRC_DeleteSystem( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_InitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ExitWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_ConnextWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_DisConnextWait( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_TimingSyncWait( COMPATIBLE_IRC_SYS *p_sys, COMPATIBLE_TIMING_NO timing_no );
extern BOOL COMPATIBLE_IRC_IsError( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_SendMenuData( COMPATIBLE_IRC_SYS *p_sys, u32 menu_id, u32 ms );
extern BOOL COMPATIBLE_IRC_RecvMenuData( COMPATIBLE_IRC_SYS *p_sys );
extern void COMPATIBLE_IRC_GetMenuData( const COMPATIBLE_IRC_SYS *cp_sys, u32 *p_menu_data, u32 *p_ms );
extern BOOL COMPATIBLE_IRC_SendReturnMenu( COMPATIBLE_IRC_SYS *p_sys );
extern BOOL COMPATIBLE_IRC_RecvReturnMenu( COMPATIBLE_IRC_SYS *p_sys );

extern BOOL COMPATIBLE_IRC_WaitStartProcTiming( COMPATIBLE_IRC_SYS *p_sys );
extern u32 COMPATIBLE_IRC_GetRandom( const COMPATIBLE_IRC_SYS *cp_sys );

//各赤外線ミニゲームで使用
extern BOOL COMPATIBLE_IRC_SendData( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data );
extern BOOL COMPATIBLE_IRC_SendDataEx( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data, const BOOL b_fast, const BOOL b_repeat, const BOOL b_send_buff_lock );
extern void COMPATIBLE_IRC_AddCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID, const NetRecvFuncTable *cp_net_recv_tbl, int tbl_max, void *p_param );
extern void COMPATIBLE_IRC_DelCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID );
