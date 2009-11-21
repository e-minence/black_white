
#pragma once

#include <gflib.h>
#include "ui/touchbar.h"



typedef struct _GTSNEGO_MESSAGE_WORK  GTSNEGO_MESSAGE_WORK;
typedef struct _GTSNEGO_DISP_WORK  GTSNEGO_DISP_WORK;

#define _BUTTON_WIN_PAL   (12)  // ウインドウ
#define _BUTTON_MSG_PAL   (11)  // メッセージフォント
#define _SUBLIST_NORMAL_PAL   (9)   //サブメニューの通常パレット
#define _TOUCHBAR_PAL  (8)  //タッチバーのパレット

// はいいいえのタイプ
#define GTSNEGO_YESNOTYPE_INFO (0)
#define GTSNEGO_YESNOTYPE_SYS  (1)


typedef enum
{
  _ARROW_LEVEL_U,
  _ARROW_LEVEL_D,
  _ARROW_MY_U,
  _ARROW_MY_D,
  _ARROW_FRIEND_U,
  _ARROW_FRIEND_D,
} _ARROW_TYPE;


//→座標
#define _ARROW_LEVEL_XU  (12)
#define _ARROW_LEVEL_YU  (5)
#define _ARROW_LEVEL_XD  (30)
#define _ARROW_LEVEL_YD  (5)

#define _ARROW_MY_XU  (15)
#define _ARROW_MY_YU  (12)
#define _ARROW_MY_XD  (30)
#define _ARROW_MY_YD  (12)

#define _ARROW_FRIEND_XU  (15)
#define _ARROW_FRIEND_YU  (17)
#define _ARROW_FRIEND_XD  (30)
#define _ARROW_FRIEND_YD  (17)


extern GTSNEGO_MESSAGE_WORK* GTSNEGO_MESSAGE_Init(HEAPID id,int msg_dat);
extern void GTSNEGO_MESSAGE_Main(GTSNEGO_MESSAGE_WORK* pWork);
extern void GTSNEGO_MESSAGE_End(GTSNEGO_MESSAGE_WORK* pWork);
extern void GTSNEGO_MESSAGE_InfoMessageDisp(GTSNEGO_MESSAGE_WORK* pWork,int msgid);
extern BOOL GTSNEGO_MESSAGE_InfoMessageEndCheck(GTSNEGO_MESSAGE_WORK* pWork);
extern void GTSNEGO_MESSAGE_InfoMessageEnd(GTSNEGO_MESSAGE_WORK* pWork);
extern APP_TASKMENU_WORK* GTSNEGO_MESSAGE_YesNoStart(GTSNEGO_MESSAGE_WORK* pWork,int type);
extern void GTSNEGO_MESSAGE_ButtonWindowCreate(int num,int* pMsgBuff,GTSNEGO_MESSAGE_WORK* pWork,pBmnCallBackFunc callback,void* pParentWork);
extern void GTSNEGO_MESSAGE_ButtonWindowDelete(GTSNEGO_MESSAGE_WORK* pWork);
extern void GTSNEGO_MESSAGE_ButtonWindowMain(GTSNEGO_MESSAGE_WORK* pWork);

extern void GTSNEGO_MESSAGE_SystemMessageDisp(GTSNEGO_MESSAGE_WORK* pWork,int msgid);
extern void GTSNEGO_MESSAGE_SystemMessageEnd(GTSNEGO_MESSAGE_WORK* pWork);
extern void GTSNEGO_MESSAGE_ErrorMessageDisp(GTSNEGO_MESSAGE_WORK* pWork,int msgid,int no);

extern void GTSNEGO_MESSAGE_DispAnyoneOrFriend(GTSNEGO_MESSAGE_WORK* pWork,pBmnCallBackFunc callback,void* pParentWork);
extern void GTSNEGO_MESSAGE_DispLevel(GTSNEGO_MESSAGE_WORK* pWork,pBmnCallBackFunc callback,void* pParentWork);
extern void GTSNEGO_MESSAGE_DispClear(GTSNEGO_MESSAGE_WORK* pWork);


extern void GTSNEGO_MESSAGE_DispLevelChange(GTSNEGO_MESSAGE_WORK* pWork,int no);
extern void GTSNEGO_MESSAGE_DispMyChange(GTSNEGO_MESSAGE_WORK* pWork,int no);
extern void GTSNEGO_MESSAGE_DispFriendChange(GTSNEGO_MESSAGE_WORK* pWork,int no);

extern APP_TASKMENU_WORK* GTSNEGO_MESSAGE_SearchButtonStart(GTSNEGO_MESSAGE_WORK* pWork);


extern GTSNEGO_DISP_WORK* GTSNEGO_DISP_Init(HEAPID id);
extern void GTSNEGO_DISP_Main(GTSNEGO_DISP_WORK* pWork);
extern void GTSNEGO_DISP_End(GTSNEGO_DISP_WORK* pWork);
extern void GTSNEGO_DISP_SettingSubBgControl(GTSNEGO_DISP_WORK* pWork);
extern void GTSNEGO_DISP_LevelInputInit(GTSNEGO_DISP_WORK* pWork);
extern TOUCHBAR_WORK* GTSNEGO_DISP_GetTouchWork(GTSNEGO_DISP_WORK* pWork);

