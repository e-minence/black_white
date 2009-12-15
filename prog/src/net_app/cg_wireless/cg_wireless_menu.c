//=============================================================================
/**
 * @file	  cg_wireless_menu.c
 * @bfief	  CGEARのワイヤレスボタンを押した時
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/12/14
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"

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
#include "sound/pm_sndsys.h"

#include "net_app/cg_wireless_menu.h"
#include "../../field/event_cg_wireless.h"

#include "cg_comm.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK

#include "cg_wireless_menu.cdat"

#include "msg/msg_cg_wireless.h"


#define _NET_DEBUG (1)  //デバッグ時は１
#define _WORK_HEAPSIZE (0x1000)  // 調整が必要

// サウンドが出来るまでの仮想
#define _SE_DESIDE (0)
#define _SE_CANCEL (0)
static void Snd_SePlay(int a){}


//--------------------------------------------
// 画面構成定義
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //ウインドウのパターン数

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _SUBLIST_NORMAL_PAL   (9)   //サブメニューの通常パレット
#define _BUTTON_WIN_CENTERX (16)   // 真ん中
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (3)    // ウインドウ高さ
#define _BUTTON_WIN_PAL   (12)  // ウインドウ
#define _BUTTON_MSG_PAL   (11)  // メッセージフォント

//BG面とパレット番号(仮設定
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME1_S)
#define _SUBSCREEN_PALLET	(0xE)




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



//--------------------------------------------
// 内部ワーク
//--------------------------------------------


enum _IBMODE_SELECT {
  _SELECTMODE_GSYNC = 0,
  _SELECTMODE_UTIL,
  _SELECTMODE_EXIT
};


#define _SUBMENU_LISTMAX (2)


typedef void (StateFunc)(CG_WIRELESS_MENU* pState);
typedef BOOL (TouchFunc)(int no, CG_WIRELESS_MENU* pState);


struct _CG_WIRELESS_MENU {
  StateFunc* state;      ///< ハンドルのプログラム状態
  TouchFunc* touch;
  int selectType;   // 接続タイプ
  HEAPID heapID;
  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// ウインドウ管理
  GFL_BUTTON_MAN* pButton;
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// メッセージ展開用ワークマネージャー
  GFL_FONT* pFontHandle;
	STRBUF*  pExpStrBuf;
  STRBUF* pStrBuf;
  u32 bgchar;  //GFL_ARCUTIL_TRANSINFO
	u32 subchar;
  //    BMPWINFRAME_AREAMANAGER_POS aPos;



  GFL_BMPWIN* infoDispWin;
  PRINT_STREAM* pStream;
	GFL_TCBLSYS *pMsgTcblSys;
  PRINT_QUE*            SysMsgQue;
  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
	APP_TASKMENU_RES* pAppTaskRes;
  EVENT_CG_WIRELESS_WORK * dbw;
  int windowNum;
  GAMEDATA* gamedata;
  GAMESYS_WORK *gameSys_;
  FIELDMAP_WORK *fieldWork_;
  GMEVENT* event_;
  int yoffset;
};



//-----------------------------------------------
//static 定義
//-----------------------------------------------
static void _changeState(CG_WIRELESS_MENU* pWork,StateFunc* state);
static void _changeStateDebug(CG_WIRELESS_MENU* pWork,StateFunc* state, int line);
//static void _buttonWindowCreate(int num,int* pMsgBuff,CG_WIRELESS_MENU* pWork);
static void _buttonWindowCreate(int num,int* pMsgBuff,CG_WIRELESS_MENU* pWork, _WINDOWPOS* pos);
static void _modeSelectMenuInit(CG_WIRELESS_MENU* pWork);
static void _modeSelectMenuWait(CG_WIRELESS_MENU* pWork);

static void _modeReportInit(CG_WIRELESS_MENU* pWork);
static void _modeReportWait(CG_WIRELESS_MENU* pWork);
static BOOL _modeSelectMenuButtonCallback(int bttnid,CG_WIRELESS_MENU* pWork);
static void _modeSelectBattleTypeInit(CG_WIRELESS_MENU* pWork);

static void _buttonWindowDelete(CG_WIRELESS_MENU* pWork);
static void _ReturnButtonStart(CG_WIRELESS_MENU* pWork);



#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG




//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(CG_WIRELESS_MENU* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(CG_WIRELESS_MENU* pWork,StateFunc state, int line)
{
  NET_PRINT("ircbtl: %d\n",line);
  _changeState(pWork, state);
}
#endif



static void _createSubBg(CG_WIRELESS_MENU* pWork)
{

  // 背景面
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );
    GFL_BG_SetPriority( GFL_BG_FRAME0_M, 0 );
		GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );

  }

  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME1_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
  }
  {
    int frame = GFL_BG_FRAME2_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   受け取った数のウインドウを等間隔に作る 幅は3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _buttonWindowCreate(int num,int* pMsgBuff,CG_WIRELESS_MENU* pWork, _WINDOWPOS* pos)
{
  int i;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

	//_buttonWindowDelete(pWork);

  pWork->windowNum = num;
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  for(i = 0;i < _WINDOW_MAXNUM;i++){
		if(pWork->buttonWin[i]){
			GFL_BMPWIN_ClearScreen(pWork->buttonWin[i]);
			BmpWinFrame_Clear(pWork->buttonWin[i], WINDOW_TRANS_OFF);
			GFL_BMPWIN_Delete(pWork->buttonWin[i]);
		}
    pWork->buttonWin[i] = NULL;
  }

  for(i=0;i < num;i++){
//    GFL_FONTSYS_SetColor(0xe,0xf,3);
    pWork->buttonWin[i] = GFL_BMPWIN_Create(
      frame,
      pos[i].leftx, pos[i].lefty,
      pos[i].width, pos[i].height,
      _SUBLIST_NORMAL_PAL,GFL_BMP_CHRAREA_GET_F);
//      _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0 );
    // システムウインドウ枠描画
    GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 0, pWork->pStrBuf, pWork->pFontHandle);

    PRINTSYS_PrintColor(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 0,
                        pWork->pStrBuf, pWork->pFontHandle, APP_TASKMENU_ITEM_MSGCOLOR);
//void PRINTSYS_PrintColor( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font, PRINTSYS_LSB color )

    GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
    GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  }


	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
  pWork->pButton = NULL;
	 
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
	 
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------

static void _buttonWindowDelete(CG_WIRELESS_MENU* pWork)
{
  int i;

	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
  pWork->pButton = NULL;
  for(i=0;i < _WINDOW_MAXNUM;i++){
		if(pWork->buttonWin[i]){
			GFL_BMPWIN_ClearScreen(pWork->buttonWin[i]);
			GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
			BmpWinFrame_Clear(pWork->buttonWin[i], WINDOW_TRANS_OFF);
			
//			GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0 );
//			GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
			GFL_BMPWIN_Delete(pWork->buttonWin[i]);
		}
    pWork->buttonWin[i] = NULL;
  }
  pWork->windowNum = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  CG_WIRELESS_MENU *pWork = p_work;
  u32 friendNo;

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< 触れた瞬間
    if(pWork->touch!=NULL){
      if(pWork->touch(bttnid, pWork)){
        return;
      }
    }
    break;

  default:
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト全体の初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeInit(CG_WIRELESS_MENU* pWork)
{

  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_cg_wireless_dat, pWork->heapID );
  //    GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_CG_COMM, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_cg_comm_comm_bg_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    // サブ画面BG0キャラ転送
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_cg_comm_comm_bg_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // サブ画面BG0スクリーン転送
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_base_NSCR,
                                              GFL_BG_FRAME0_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_wireless_btn_NSCR,
                                              GFL_BG_FRAME2_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);
    
		GFL_ARC_CloseDataHandle(p_handle);
	}


  
  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
	
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
																0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);


}

//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuInit(CG_WIRELESS_MENU* pWork)
{

  int aMsgBuff[]={CGEAR_WIRLESS_001,CGEAR_WIRLESS_002};

  _buttonWindowCreate(NELEMS(aMsgBuff), aMsgBuff, pWork,wind_wireless);

	pWork->pButton = GFL_BMN_Create( btn_wireless, _BttnCallBack, pWork,  pWork->heapID );
	pWork->touch = &_modeSelectMenuButtonCallback;

  _ReturnButtonStart(pWork);

	_CHANGE_STATE(pWork,_modeSelectMenuWait);

}

static void _workEnd(CG_WIRELESS_MENU* pWork)
{
  GFL_FONTSYS_SetDefaultColor();

	_buttonWindowDelete(pWork);
  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
  GFL_MSG_Delete( pWork->pMsgData );
  GFL_FONT_Delete(pWork->pFontHandle);
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );

}

//------------------------------------------------------------------------------
/**
 * @brief   フェードアウト処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(CG_WIRELESS_MENU* pWork)
{
	if(WIPE_SYS_EndCheck()){
		_CHANGE_STATE(pWork, NULL);        // 終わり
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面タッチ処理
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectMenuButtonCallback(int bttnid,CG_WIRELESS_MENU* pWork)
{
  BOOL ret=FALSE;
  
  switch( bttnid ){
  case _SELECTMODE_GSYNC:
    pWork->selectType = CG_WIRELESS_RETURNMODE_PALACE;
    ret = TRUE;
    break;
  case _SELECTMODE_UTIL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = CG_WIRELESS_RETURNMODE_TV;
    ret = TRUE;
    break;
  case _SELECTMODE_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    pWork->selectType = CG_WIRELESS_RETURNMODE_NONE;
		WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
										WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
    _CHANGE_STATE(pWork,_modeFadeout);        // 終わり
    ret = FALSE;
    break;
  default:
    break;
  }

  if(ret==TRUE){
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    if(WIRELESSSAVE_ON == CONFIG_GetWirelessSaveMode(SaveData_GetConfig(pWork->dbw->ctrl))){
      _CHANGE_STATE(pWork,_modeReportInit);
    }
    else{
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
      _CHANGE_STATE(pWork,_modeFadeout);        // 終わり
    }
  }

  APP_TASKMENU_CloseMenu(pWork->pAppTask);
  pWork->pAppTask = NULL;

  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(CG_WIRELESS_MENU* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    if(selectno==0){
      pWork->selectType = CG_WIRELESS_RETURNMODE_PALACE;
      _CHANGE_STATE(pWork,NULL);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    return;
  }
	if(WIPE_SYS_EndCheck()){
		GFL_BMN_Main( pWork->pButton );
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   はいいいえウインドウ
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _YesNoStart(CG_WIRELESS_MENU* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 14;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, CGEAR_WIRLESS_008, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, CGEAR_WIRLESS_009, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , -8 );
}


static void _ReturnButtonStart(CG_WIRELESS_MENU* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  1;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 23;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, CGEAR_WIRLESS_010, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_RETURN;

  pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);

}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの終了待ち
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _infoMessageEndCheck(CG_WIRELESS_MENU* pWork)
{
  if(pWork->pStream){
    int state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
      }
      break;
    default:
      break;
    }
    return FALSE;  //まだ終わってない
  }
  return TRUE;// 終わっている
}

//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _infoMessageDisp(CG_WIRELESS_MENU* pWork)
{
  GFL_BMPWIN* pwin;

  
  if(pWork->infoDispWin==NULL){
    pWork->infoDispWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME1_S ,
      1 , 3, 30 ,4 ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = pWork->infoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pStrBuf, pWork->pFontHandle,
                                        MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);

  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
}



//------------------------------------------------------------------------------
/**
 * @brief   セーブ確認画面初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(CG_WIRELESS_MENU* pWork)
{


  GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME1_S,0);
  GFL_BG_SetVisible(GFL_BG_FRAME2_S,VISIBLE_OFF);

  GFL_MSG_GetString( pWork->pMsgData, CGEAR_WIRLESS_011, pWork->pStrBuf );
  
  _infoMessageDisp(pWork);

  
  _CHANGE_STATE(pWork,_modeReportWait);
}


static void _FadeWait(CG_WIRELESS_MENU* pWork)
{
  if(GFL_FADE_CheckFade()){
    return;
  }
  _CHANGE_STATE(pWork,NULL);
}



//------------------------------------------------------------------------------
/**
 * @brief   セーブ中
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReporting(CG_WIRELESS_MENU* pWork)
{

  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  {
    SAVE_RESULT svr = GAMEDATA_SaveAsyncMain(pWork->gamedata);
    
    if(svr == SAVE_RESULT_OK){
			BmpWinFrame_Clear(pWork->infoDispWin, WINDOW_TRANS_OFF);
      GFL_BMPWIN_ClearScreen(pWork->infoDispWin);
      GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);

      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1);

      _CHANGE_STATE(pWork,_FadeWait);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セーブ確認画面待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait2(CG_WIRELESS_MENU* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      GFL_MSG_GetString( pWork->pMsgData, CGEAR_WIRLESS_012, pWork->pStrBuf );
      _infoMessageDisp(pWork);
      //セーブ開始
      GAMEDATA_SaveAsyncStart(pWork->gamedata);
      _CHANGE_STATE(pWork,_modeReporting);
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      pWork->selectType = CG_WIRELESS_RETURNMODE_NONE;
      _CHANGE_STATE(pWork,NULL);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   セーブ確認画面待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait(CG_WIRELESS_MENU* pWork)
{
  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  _YesNoStart(pWork);
  _CHANGE_STATE(pWork,_modeReportWait2);
}

static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット

  GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

  //        GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
  GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット

  GX_VRAM_SUB_OBJ_128_D,			// サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

  GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
  GX_VRAM_TEXPLTT_NONE,			// テクスチャパレットスロット

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,

};


//------------------------------------------------------------------------------
/**
 * @brief   PROCスタート
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT CG_WirelessMenuProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_CG_WIRELESS_WORK* pParentWork =pwk;
	
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x18000 );

  {
    CG_WIRELESS_MENU *pWork = GFL_PROC_AllocWork( proc, sizeof( CG_WIRELESS_MENU ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(CG_WIRELESS_MENU));
    pWork->heapID = HEAPID_IRCBATTLE;
    pWork->gamedata = pParentWork->gameData;

    GFL_DISP_SetDispSelect(GFL_DISP_3D_TO_MAIN);
    GXS_DispOn();
    GX_DispOn();

		GFL_BG_Init(pWork->heapID);
		GFL_BMPWIN_Init(pWork->heapID);
		GFL_FONTSYS_Init();
		GFL_DISP_SetBank( &_defVBTbl );
		{
			GFL_BG_SYS_HEADER BGsys_data = {
				GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
			};
			GFL_BG_SetBGMode( &BGsys_data );
		}
		GFL_DISP_GX_SetVisibleControlDirect(0);		//全BG&OBJの表示OFF
		GFL_DISP_GXS_SetVisibleControlDirect(0);

		_createSubBg(pWork);
		_modeInit(pWork);
    pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 1 , 0 );
    pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
    pWork->pAppTaskRes =
      APP_TASKMENU_RES_Create( GFL_BG_FRAME1_S, _SUBLIST_NORMAL_PAL,
                               pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID  );

		WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
									WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
		_CHANGE_STATE(pWork,_modeSelectMenuInit);
    pWork->dbw = pwk;
	}
  
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT CG_WirelessMenuProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  CG_WIRELESS_MENU* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }

  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }

  //	ConnectBGPalAnm_Main(&pWork->cbp);
  GFL_TCBL_Main( pWork->pMsgTcblSys );
  PRINTSYS_QUE_Main(pWork->SysMsgQue);
  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, pWork->yoffset );
  pWork->yoffset--;

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT CG_WirelessMenuProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  CG_WIRELESS_MENU* pWork = mywk;
  EVENT_CG_WIRELESS_WORK* pParentWork =pwk;

  _workEnd(pWork);
  pParentWork->selectType = pWork->selectType;

  GFL_PROC_FreeWork(proc);

  GFL_TCBL_Exit(pWork->pMsgTcblSys);
	GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  if(pWork->infoDispWin){
    GFL_BMPWIN_Delete(pWork->infoDispWin);
  }
  APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );


	GFL_BMPWIN_Exit();
	GFL_BG_Exit();


	GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);


	
  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA CG_WirelessMenuProcData = {
  CG_WirelessMenuProcInit,
  CG_WirelessMenuProcMain,
  CG_WirelessMenuProcEnd,
};


