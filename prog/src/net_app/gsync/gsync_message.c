//============================================================================================
/**
 * @file    gsync_message.c
 * @bfief   GAMESYNCメッセージ表示関連
 * @author  k.ohno
 * @date    2009.11.29
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>

#include "arc_def.h"
#include "net_app/gsync.h"

#include "infowin/infowin.h"
#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait
#include "savedata/config.h"  // WIRELESSSAVE_ON

#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "system/time_icon.h"
#include "sound/pm_sndsys.h"
#include "../../field/event_ircbattle.h"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "app/app_printsys_common.h"
#include "net_app/pdwacc.h"

#include "gsync_local.h"
#include "gtsnego.naix"
#include "msg/msg_gsync.h"
#include "app/app_keycursor.h"

#if PM_DEBUG
#include "debug/debugwin_sys.h"
#include "test/debug_pause.h"
#endif

//--------------------------------------------
// 画面構成定義
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //ウインドウのパターン数
#define _MESSAGE_BUF_NUM	( 200*2 )
#define _SUBMENU_LISTMAX (2)

#define _BUTTON_WIN_CENTERX (16)   // 真ん中
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (3)    // ウインドウ高さ


//-------------------------------------------------------------------------
///	文字表示色定義(default)	-> gflib/fntsys.hへ移動
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))
//------------------------------------------------------------------
//	カラー関連ダミー定義
//------------------------------------------------------------------
// 通常のフォントカラー
#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(5)
#define	FBMP_COL_GRN_SDW	(6)
#define	FBMP_COL_BLUE		(7)
#define	FBMP_COL_BLU_SDW	(8)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE		(15)


typedef enum{
  _MESSAGE_LEVEL,
  _MESSAGE_MY,
  _MESSAGE_FRIEND,
  _MESSAGE_INDEX_MAX
} _MESSAGE_INDEX;

typedef struct {
  int leftx;
  int lefty;
  int width;
  int height;
} _WINDOWPOS;

#define _BMP_WINDOW_NUM  (12)



struct _GSYNC_MESSAGE_WORK {
  u32 bgchar;  //GFL_ARCUTIL_TRANSINFO
  u32 bgchar2S;  //SystemMsg
  u32 bgchar1M; //info

  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// ウインドウ管理
  GFL_BUTTON_MAN* pButton;

  GFL_MSGDATA *pMsgData;  //

  WORDSET *pWordSet;								// メッセージ展開用ワークマネージャー
  GFL_FONT* pFontHandle;
  STRBUF* pStrBuf;
  STRBUF* pMessageStrBufEx;
  int msgidx[_MESSAGE_INDEX_MAX];
  APP_KEYCURSOR_WORK* pKeyCursor;

  GFL_BMPWIN* infoDispWin;
  GFL_BMPWIN* systemDispWin;
  GFL_BMPWIN* mainDispWin[_BMP_WINDOW_NUM];
  TIMEICON_WORK* pTimeIcon;

  PRINT_STREAM* pStream;
	GFL_TCBLSYS *pMsgTcblSys;
  PRINT_QUE*            SysMsgQue;

  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
	APP_TASKMENU_RES* pAppTaskRes;
  APP_PRINTSYS_COMMON_WORK aPrintWork;
//  int windowNum;
  HEAPID heapID;
  
};


static void _ButtonSafeDelete(GSYNC_MESSAGE_WORK* pWork)
{
  if(pWork->pButton){
    GFL_BMN_Delete(pWork->pButton);
    pWork->pButton = NULL;
  }
}


GSYNC_MESSAGE_WORK* GSYNC_MESSAGE_Init(HEAPID id,int msg_dat)
{
  GSYNC_MESSAGE_WORK* pWork = GFL_HEAP_AllocClearMemory(id, sizeof(GSYNC_MESSAGE_WORK));
  pWork->heapID = id;

  pWork->pWordSet    = WORDSET_Create( pWork->heapID );
  GFL_BMPWIN_Init(pWork->heapID);
  GFL_FONTSYS_Init();
  pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 2 , 0 );
  pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pMessageStrBufEx = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, msg_dat, pWork->heapID );
  pWork->pKeyCursor = APP_KEYCURSOR_Create( 15, FALSE, TRUE, pWork->heapID );

  pWork->pAppTaskRes =
    APP_TASKMENU_RES_Create( GFL_BG_FRAME1_S, _SUBLIST_NORMAL_PAL,
                             pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID  );

//  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  pWork->bgchar1M = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
  pWork->bgchar2S = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME2_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
	
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
																0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
																0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);


  
#if DEBUG_ONLY_FOR_ohno
//  DEBUGWIN_InitProc( GFL_BG_FRAME3_M , pWork->pFontHandle );
//  DEBUG_PAUSE_SetEnable( TRUE );
#endif
  return pWork;
}

void GSYNC_MESSAGE_Main(GSYNC_MESSAGE_WORK* pWork)
{
  GFL_TCBL_Main( pWork->pMsgTcblSys );
  PRINTSYS_QUE_Main(pWork->SysMsgQue);

}

void GSYNC_MESSAGE_End(GSYNC_MESSAGE_WORK* pWork)
{
  int i;

  GSYNC_MESSAGE_InfoMessageEnd(pWork);
  if(pWork->infoDispWin){
    GFL_BMPWIN_Delete(pWork->infoDispWin);
  }
  if(pWork->systemDispWin){
    GFL_BMPWIN_Delete(pWork->systemDispWin);
  }
  for(i=0;i< _BMP_WINDOW_NUM;i++){
    if(pWork->mainDispWin[i]){
      GFL_BMPWIN_Delete(pWork->mainDispWin[i]);
    }
  }
  
  
//  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
//                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar1M),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar1M));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME2_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2S),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar2S));
  _ButtonSafeDelete(pWork);
  APP_KEYCURSOR_Delete( pWork->pKeyCursor );

  WORDSET_Delete(pWork->pWordSet);
//  GSYNC_MESSAGE_ButtonWindowDelete(pWork);

  GFL_FONTSYS_SetDefaultColor();
  GFL_MSG_Delete( pWork->pMsgData );
  GFL_FONT_Delete(pWork->pFontHandle);
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pMessageStrBufEx);

  APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  GFL_TCBL_Exit(pWork->pMsgTcblSys);

	GFL_BMPWIN_Exit();
  GFL_HEAP_FreeMemory(pWork);
}



//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_MessageDisp(GSYNC_MESSAGE_WORK* pWork, BOOL bFast)
{
  GFL_BMPWIN* pwin;
  
  if(pWork->infoDispWin==NULL){
    pWork->infoDispWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME1_S ,
      1 , 1, 30 ,4 ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = pWork->infoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  if(bFast){
    PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,0,0, pWork->pStrBuf, pWork->pFontHandle );
  }
  else{
    pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pStrBuf, pWork->pFontHandle,
                                          MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);
    APP_PRINTSYS_COMMON_PrintStreamInit( &pWork->aPrintWork ,APP_PRINTSYS_COMMON_TYPE_BOTH);
  }
  
  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar1M), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
}

//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_InfoMessageDisp(GSYNC_MESSAGE_WORK* pWork,int msgid)
{
  GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pStrBuf );
  GSYNC_MESSAGE_MessageDisp(pWork, FALSE);
}


//------------------------------------------------------------------------------
/**
 * @brief   nickname表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_NickNameMessageDisp(GSYNC_MESSAGE_WORK* pWork,int msgid, int lvup,POKEMON_PARAM* pp)
{

  GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pMessageStrBufEx );
  WORDSET_RegisterPokeNickName( pWork->pWordSet, 0,  pp );

  WORDSET_RegisterNumber(pWork->pWordSet, 1, lvup,
                         3, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);

  WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pMessageStrBufEx  );
//  GSYNC_MESSAGE_MessageDisp(pWork);
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの終了待ち
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL GSYNC_MESSAGE_InfoMessageEndCheck(GSYNC_MESSAGE_WORK* pWork)
{
  if(pWork->pStream){
    APP_KEYCURSOR_Main( pWork->pKeyCursor, pWork->pStream, pWork->infoDispWin );

    if(!APP_PRINTSYS_COMMON_PrintStreamFunc(&pWork->aPrintWork,pWork->pStream)){
      return FALSE;  //まだ終わってない
    }
    PRINTSYS_PrintStreamDelete( pWork->pStream );
    pWork->pStream = NULL;
  }
  return TRUE;// 終わっている
}

//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ消去
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_InfoMessageEnd(GSYNC_MESSAGE_WORK* pWork)
{
  if(pWork->pTimeIcon){
    TIMEICON_Exit(pWork->pTimeIcon);
    pWork->pTimeIcon=NULL;
  }
  if(pWork->pStream){
    PRINTSYS_PrintStreamDelete( pWork->pStream );
    pWork->pStream = NULL;
  }
  if(pWork->infoDispWin){
    BmpWinFrame_Clear(pWork->infoDispWin, WINDOW_TRANS_OFF);
    GFL_BMPWIN_ClearScreen(pWork->infoDispWin);
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   はいいいえウインドウ
 * @retval  none
 */
//------------------------------------------------------------------------------



APP_TASKMENU_WORK* GSYNC_MESSAGE_YesNoStart(GSYNC_MESSAGE_WORK* pWork,int type)
{
  int i;
  APP_TASKMENU_INITWORK appinit;
  APP_TASKMENU_WORK* pAppTask;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];

  switch(type){
  case GSYNC_YESNOTYPE_INFO:
    appinit.charPosX = 32;
    appinit.charPosY = 12;
    appinit.posType = ATPT_RIGHT_DOWN;
    break;
  case GSYNC_YESNOTYPE_SYS:
    appinit.charPosX = 32;
    appinit.charPosY = 24;
    appinit.posType = ATPT_RIGHT_DOWN;
    break;
  }
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, GAMESYNC_005, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, GAMESYNC_006, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pAppTask = APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG3 , -8 );
  return pAppTask;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベント実行関数
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------

void GSYNC_MESSAGE_ButtonWindowMain(GSYNC_MESSAGE_WORK* pWork)
{
  GFL_BMN_Main( pWork->pButton );
}




//------------------------------------------------------------------------------
/**
 * @brief   メッセージをセット
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_SetNormalMessage(GSYNC_MESSAGE_WORK* pWork,int msgid)
{
  GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pStrBuf );
}

//------------------------------------------------------------------------------
/**
 * @brief   システムウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_SystemMessageDisp(GSYNC_MESSAGE_WORK* pWork, int start, int height)
{
  GFL_BMPWIN* pwin;

  if(pWork->systemDispWin){
    GFL_BMPWIN_Delete(pWork->systemDispWin);
  }

  pWork->systemDispWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_S , 1 , start, 30 , height ,  _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  pwin = pWork->systemDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,0,0, pWork->pStrBuf, pWork->pFontHandle );
  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2S), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
}

//------------------------------------------------------------------------------
/**
 * @brief   システムウインドウ（サインインを促す）表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_CMMessageDisp(GSYNC_MESSAGE_WORK* pWork)
{
  GFL_BMPWIN* pwin;

  if(pWork->systemDispWin){
    GFL_BMPWIN_Delete(pWork->systemDispWin);
  }
  pWork->systemDispWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_S , 1 , 9, 30 , 6 ,  _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  pwin = pWork->systemDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,0,0, pWork->pStrBuf, pWork->pFontHandle );
  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2S), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
}


//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ消去
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_SystemMessageEnd(GSYNC_MESSAGE_WORK* pWork)
{
  BmpWinFrame_Clear(pWork->systemDispWin, WINDOW_TRANS_OFF);
  GFL_BMPWIN_ClearScreen(pWork->systemDispWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
}


//------------------------------------------------------------------------------
/**
 * @brief   システムウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_ErrorMessageDisp(GSYNC_MESSAGE_WORK* pWork,int msgid,int no)
{
  WORDSET_RegisterNumber(pWork->pWordSet, 0, no,
                         5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pStrBuf );
  GSYNC_MESSAGE_SystemMessageDisp(pWork, 1,16);
}


//------------------------------------------------------------------------------
/**
 * @brief   メッセージ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

static void GSYNC_MESSAGE_DispInit(GSYNC_MESSAGE_WORK* pWork)
{
  GFL_BMPWIN* pwin;
  int i;

  for(i=0;i<_BMP_WINDOW_NUM;i++){
    if(pWork->mainDispWin[i]!=NULL){
      GFL_BMPWIN_Delete(pWork->mainDispWin[i]);
      pWork->mainDispWin[i]=NULL;
    }
  }
  GFL_FONTSYS_SetColor(1, 2, 0);
}

static int GSYNC_MESSAGE_Disp(GSYNC_MESSAGE_WORK* pWork,int msgid,int x,int y)
{
  GFL_BMPWIN* pwin;
  int i;

  for(i=0;i<_BMP_WINDOW_NUM;i++){
    if(pWork->mainDispWin[i]==NULL){
      pWork->mainDispWin[i] = GFL_BMPWIN_Create(
        GFL_BG_FRAME2_S , x , y, 20 , 2 ,  _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );

      pwin = pWork->mainDispWin[i];

      GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 0);
  
      GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pStrBuf );
      PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,1, 2, pWork->pStrBuf, pWork->pFontHandle );
      GFL_BMPWIN_TransVramCharacter(pwin);
      GFL_BMPWIN_MakeScreen(pwin);
      return i;
    }
  }
  return 0;
}


static void GSYNC_MESSAGE_DispMsgChange(GSYNC_MESSAGE_WORK* pWork,int msgid,int idx)
{
  GFL_BMPWIN* pwin;
  int i=idx;

  if(pWork->mainDispWin[i]!=NULL){
    pwin = pWork->mainDispWin[i];
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 0);
    GFL_MSG_GetString( pWork->pMsgData, msgid, pWork->pStrBuf );
    PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,1, 2, pWork->pStrBuf, pWork->pFontHandle );
    GFL_BMPWIN_TransVramCharacter(pwin);
  }
}


static void GSYNC_MESSAGE_DispTransReq(GSYNC_MESSAGE_WORK* pWork)
{

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
}

//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ消去
 * @retval  none
 */
//------------------------------------------------------------------------------

void GSYNC_MESSAGE_DispClear(GSYNC_MESSAGE_WORK* pWork)
{
  int i;
  
  for(i=0;i<_BMP_WINDOW_NUM;i++){
    if(pWork->mainDispWin[i]!=NULL){
      BmpWinFrame_Clear(pWork->mainDispWin[i], WINDOW_TRANS_OFF);
      GFL_BMPWIN_ClearScreen(pWork->mainDispWin[i]);
      GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   タイムアイコンを出す
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void GSYNC_MESSAGE_WindowTimeIconStart(GSYNC_MESSAGE_WORK* pWork)
{
  if(pWork->pTimeIcon){
    TIMEICON_Exit(pWork->pTimeIcon);
    pWork->pTimeIcon=NULL;
  }
  pWork->pTimeIcon =
    TIMEICON_CreateTcbl(pWork->pMsgTcblSys,pWork->infoDispWin, 15, TIMEICON_DEFAULT_WAIT, pWork->heapID);
}


//------------------------------------------------------------------------------
/**
 * @brief   パスワードを入れる
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void GSYNC_MESSAGE_SetPassword(GSYNC_MESSAGE_WORK* pWork,u32 profileID)
{
  PDWACC_MESSAGE_GetPassWord(profileID, pWork->pStrBuf);
}
