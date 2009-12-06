//======================================================================
/**
 * @file	mb_util_msg.h
 * @brief	マルチブート・メッセージ表示
 * @author	ariizumi
 * @data	09/11/16
 *
 * モジュール名：MB_MSG
 */
//======================================================================
#pragma once

#include "app/app_taskmenu.h"
#include "print/wordset.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MB_MSG_PLT_MAIN_TASKMENU (12)  //2本
#define MB_MSG_PLT_MAIN_MSGWIN (14)
#define MB_MSG_PLT_MAIN_FONT   (15)

#define MB_MSG_MSGWIN_CGX    (1)
#define MB_MSG_MSGWIN_TOP    (8)
#define MB_MSG_MSGWIN_LEFT   (3)
#define MB_MSG_MSGWIN_HEIGHT (8)
#define MB_MSG_MSGWIN_WIDTH  (26)

#define MB_MSG_MSGWIN_SEL_TOP    (21)
#define MB_MSG_MSGWIN_SEL_LEFT   (1)
#define MB_MSG_MSGWIN_SEL_HEIGHT (2)
#define MB_MSG_MSGWIN_SEL_WIDTH  (30)

#define MB_MSG_MSGWIN_SEL_L_TOP    (19)
#define MB_MSG_MSGWIN_SEL_L_LEFT   (1)
#define MB_MSG_MSGWIN_SEL_L_HEIGHT (4)
#define MB_MSG_MSGWIN_SEL_L_WIDTH  (30)

#define MB_MSG_YESNO_X ( 32-APP_TASKMENU_PLATE_WIDTH_YN_WIN )
#define MB_MSG_YESNO_Y ( 24-APP_TASKMENU_PLATE_HEIGHT*2 )
#define MB_MSG_YESNO_COLOR (APP_TASKMENU_ITEM_MSGCOLOR)
//選択画面用
#define MB_MSG_YESNO_SEL_X ( 24 )
#define MB_MSG_YESNO_SEL_Y ( 12 )

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MMWT_NONE ,
  MMWT_NORMAL,  //画面中央
  MMWT_1LINE,   //画面下１行
  MMWT_2LINE,   //画面下２行
}MB_MSG_WIN_TYPE;

typedef enum
{
  MMYR_NONE,
  MMYR_RET1,  //上のほう
  MMYR_RET2,  //下のほう
}MB_MSG_YESNO_RET;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_MSG_WORK MB_MSG_WORK;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

extern MB_MSG_WORK* MB_MSG_MessageInit( HEAPID heapId , const u8 frame , const u32 datId );
extern void MB_MSG_MessageTerm( MB_MSG_WORK *work );
extern void MB_MSG_MessageMain( MB_MSG_WORK *work );
extern void MB_MSG_MessageCreateWindow( MB_MSG_WORK *msgWork , MB_MSG_WIN_TYPE type );
extern void MB_MSG_MessageDisp( MB_MSG_WORK *work , const u16 msgId , const int msgSpeed );
extern void MB_MSG_MessageDispNoWait( MB_MSG_WORK *msgWork , const u16 msgId );
extern void MB_MSG_MessageHide( MB_MSG_WORK *work );
extern void MB_MSG_MessageCreateWordset( MB_MSG_WORK *work );
extern void MB_MSG_MessageDeleteWordset( MB_MSG_WORK *work );
extern void MB_MSG_MessageWordsetName( MB_MSG_WORK *msgWork , const u32 bufId , MYSTATUS *myStatus);
extern void MB_MSG_DispYesNo( MB_MSG_WORK *work );
extern void MB_MSG_DispYesNo_Select( MB_MSG_WORK *msgWork );
extern void MB_MSG_ClearYesNo( MB_MSG_WORK *msgWork );
extern const MB_MSG_YESNO_RET MB_MSG_UpdateYesNo( MB_MSG_WORK *work );

extern GFL_MSGDATA* MB_MSG_GetMsgHandle( MB_MSG_WORK *msgWork );
extern WORDSET* MB_MSG_GetWordSet( MB_MSG_WORK *msgWork );
extern GFL_FONT* MB_MSG_GetFont( MB_MSG_WORK *msgWork );
extern PRINT_QUE* MB_MSG_GetPrintQue( MB_MSG_WORK *msgWork );
extern const BOOL MB_MSG_CheckPrintQueIsFinish( MB_MSG_WORK *msgWork );


