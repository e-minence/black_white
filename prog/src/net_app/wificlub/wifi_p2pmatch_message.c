//============================================================================================
/**
 * @file    wifi_p2pmatch_message.c
 * @bfief   WIFIP2Pマッチング画面  メッセージ部分切り出し
 * @author  k.ohno
 * @date    10.01.09
 */
//============================================================================================

static const BMPWIN_YESNO_DAT _yesNoBmpDat = {
  GFL_BG_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
  FLD_YESNO_WIN_PAL, FLD_YESNO_WIN_CGX
  };



#define TALK_MESSAGE_BUF_NUM  ( 190*2 )
#define TITLE_MESSAGE_BUF_NUM ( 90*2 )

static const BMPWIN_YESNO_DAT _yesNoBmpDatSys = {
  GFL_BG_FRAME2_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY,
  FLD_YESNO_WIN_PAL, YESNO_WIN_CGX
  };
static const BMPWIN_YESNO_DAT _yesNoBmpDatSys2 = {
  GFL_BG_FRAME1_M, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY+6,
  FLD_YESNO_WIN_PAL, FRAME1_YESNO_WIN_CGX
  };

//------------------------------------------------------------------
/**
 * $brief   yesnoウインドウを出す
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateM2(WIFIP2PMATCH_WORK *wk)
{
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDat, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}

//------------------------------------------------------------------
/**
 * $brief   yesnoウインドウを出す
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateM1(WIFIP2PMATCH_WORK *wk)
{
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDatSys2, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m1), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}


//------------------------------------------------------------------
/**
 * $brief   yesnoウインドウを出す
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateSys(WIFIP2PMATCH_WORK *wk)
{
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDatSys, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}


//------------------------------------------------------------------
/**
 * $brief   友人番号の名前をexpandする
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------
static void _friendNameExpand( WIFIP2PMATCH_WORK *wk, int friendNo)
{
  if(friendNo != -1){
    int sex = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(wk->pList, friendNo, wk->pExpStrBuf);
    WORDSET_RegisterWord( wk->WordSet, 0, wk->pExpStrBuf, sex, TRUE, PM_LANG);
  }
}

//------------------------------------------------------------------
/**
 * $brief   BMPウィンドウを消す
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static GFL_BMPWIN* _BmpWinDel(GFL_BMPWIN* pBmpWin)
{
  if(pBmpWin){
    BmpWinFrame_Clear( pBmpWin, WINDOW_TRANS_ON );
    GFL_BMPWIN_Delete(pBmpWin);
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * $brief   タイマーアイコンを消す
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk)
{
  if(wk->timeWaitWork){
    TimeWaitIconTaskDel(wk->timeWaitWork);  // タイマー止め
    wk->timeWaitWork = NULL;
    wk->MsgWin = _BmpWinDel(wk->MsgWin);

  }
}


//------------------------------------------------------------------
/**
 * $brief   メッセージの停止
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk )
{
  _timeWaitIconDel(wk);
  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  wk->MsgWin = _BmpWinDel(wk->MsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   メッセージを停止してよいかどうか
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL WifiP2PMatchMessageEndCheck(WIFIP2PMATCH_WORK* wk)
{
  if(wk->pStream){
    int state = PRINTSYS_PrintStreamGetState( wk->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( wk->pStream );
      wk->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
        PRINTSYS_PrintStreamReleasePause( wk->pStream );
      }
      break;
    default:
      break;
    }
    return FALSE;  //まだ終わってない
  }
  return TRUE;// 終わっている
}


//------------------------------------------------------------------
/**
 * $brief   会話ウインドウ表示
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem )
{
  // 文字列取得
  u8 speed = MSGSPEED_GetWait();

  // TimeWaitIcon破棄
  _timeWaitIconDel( wk );

  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  if(wk->pStream){
    PRINTSYS_PrintStreamDelete( wk->pStream );
  }

  wk->MsgWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_MSG_WIN_PX, COMM_MSG_WIN_PY,
    COMM_MSG_WIN_SX, COMM_MSG_WIN_SY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  if(bSystem){
    GFL_MSG_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
  }
  else{
    GFL_MSG_GetString(  wk->MsgManager, msgno, wk->pExpStrBuf );
  }
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );


  // 会話ウインドウ枠描画
  //GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 15 );
  //    BmpTalkWinWrite(&wk->MsgWin, WINDOW_TRANS_OFF,COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  //GFL_BMPWIN_MakeFrameScreen(wk->MsgWin, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL);


  //------

  //BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // システムウインドウ枠描画
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->MsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(wk->MsgWin);
//  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);

  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );

  //------

  // 文字列描画開始
  //  MsgPrintSkipFlagSet( MSG_SKIP_ON );   // メッセージスキップON
  //  MsgPrintAutoFlagSet( MSG_AUTO_OFF );  // メッセージ自動送りOFF
  //    wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString,
  //                                     0, 0, speed, NULL);




#if 0
  GFL_FONTSYS_SetDefaultColor();
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MsgWin), 2, 2, wk->TalkString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
#else
  wk->pStream = PRINTSYS_PrintStream(wk->MsgWin ,0,0, wk->TalkString, wk->fontHandle,
                                        MSGSPEED_GetWait(), wk->pMsgTcblSys, 2,HEAPID_WIFIP2PMATCH, 15);

  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
  GFL_BMPWIN_MakeScreen(wk->MsgWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);

#endif
  //GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
}

//==============================================================================
/**
 * @brief   描画開始位置がマイナス方向にあっても描画できるBmpWinPrintラッパー
 * @retval  none
 */
//==============================================================================
static void _BmpWinPrint_Rap(
  GFL_BMPWIN * win, void * src,
  int src_x, int src_y, int src_dx, int src_dy,
  int win_x, int win_y, int win_dx, int win_dy,HEAPID heapID )
{
  GFL_BMP_DATA *srcBmp = GFL_BMP_CreateWithData( (u8*)src , 1,1,GFL_BMP_16_COLOR,heapID );
  if(win_x < 0){
    int diff;
    diff = win_x*-1;
    if(diff>win_dx){
      diff = win_dx;
    }
    win_x   = 0;
    src_x  += diff;
    src_dx -= diff;
    win_dx -= diff;
  }

  if(win_y < 0){
    int diff;
    diff = win_y*-1;
    if(diff>win_dy){
      diff = win_dy;
    }
    win_y   = 0;
    src_y  += diff;
    src_dy -= diff;
    win_dy -= diff;
  }

  GFL_BMP_Print( srcBmp, GFL_BMPWIN_GetBmp(win), src_x, src_y, win_x, win_y, win_dx, win_dy, 0);

  GFL_BMP_Delete( srcBmp );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ビットマップにアイコンデータを書き込む
 *
 *  @param  p_bmp   ビットマップ
 *  @param  p_data    アイコンデータ
 *  @param  x     書き込みｘ座標
 *  @param  y     書き込みｙ座標
 *  @param  icon_type アイコンタイプ
 *  @param  col     カラー
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal )
{
  //  int pal;

  // 書き込み
  _BmpWinPrint_Rap( p_bmp,
                    p_data->p_char->pRawData,
                    (PLAYER_DISP_ICON_SCRN_X*8)*icon_type, 0,
                    p_data->p_char->W*8, p_data->p_char->H*8,
                    x, y,
                    PLAYER_DISP_ICON_SCRN_X*8, PLAYER_DISP_ICON_SCRN_Y*8,
                    HEAPID_WIFIP2PMATCH);

  //  // パレットNo取得
  //  pal = WifiP2PMatchBglFrmIconPalGet( p_bmp->frmnum );

  // パレットナンバーをあわせる
  GFL_BMPWIN_SetPalette( p_bmp, pal+col );
}


//------------------------------------------------------------------
/**
 * $brief   システムメッセージウインドウ表示
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno )
{
  _timeWaitIconDel(wk);
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  //   if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
  //     PRINTSYS_QUE_Clear(wk->SysMsgQue);
  //    }

  wk->SysMsgWin= GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_SYS_WIN_PX, COMM_SYS_WIN_PY,
    COMM_SYS_WIN_SX, COMM_SYS_WIN_SY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B);

//  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_SYS_WIN_CGX, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // システムウインドウ枠描画
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON_V,
                     GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFONT_PAL );

  //    GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  //   GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SysMsgWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  // 文字列描画開始
  //    wk->MsgIndex = GF_STR_PrintSimple( &wk->SysMsgWin, FONT_TALK,
  //                                     wk->TalkString, 0, 0, MSG_NO_PUT, NULL);

  //    PRINT_UTIL_Print(&wk->SysMsgPrintUtil, wk->SysMsgQue, 0, 0,wk->TalkString,wk->fontHandle);
#if 1
  GFL_MSG_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );
  GFL_FONTSYS_SetDefaultColor();
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 4, 4, wk->TalkString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->SysMsgWin);
#endif
  //  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   待機状態になる為の選択メニュー WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

typedef struct {
  u32  str_id;
  u32  param;
} _infoMenu;

//  メニューリスト
//#define WIFI_PARENTINFO_MENULIST_MAX    ( 10 )
#define WIFI_PARENTINFO_MENULIST_COMMON   ( 4 ) // 変動しない部分の数

_infoMenu _parentInfoMenuList[  ] = {
  { msg_wifilobby_027, (u32)WIFI_GAME_VCT }, //VCT
  { msg_wifilobby_076, (u32)WIFI_GAME_TVT }, //TVT
  { msg_wifilobby_022, (u32)WIFI_GAME_BATTLE_SINGLE_ALL }, //対戦
  { msg_wifilobby_025, (u32)WIFI_GAME_TRADE }, //CHANGE
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL }
};


_infoMenu _parentSingleInfoMenuList[] = {
  { msg_wifilobby_0571, (u32)0 }, //シングル
  { msg_wifilobby_0572, (u32)1 },   //ダブル
  { msg_wifilobby_0573, (u32)2 },  //トリプル
  { msg_wifilobby_0574, (u32)3 },  //ローテーション
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};



_infoMenu _parentCustomInfoMenuList[] = {
  { msg_wifilobby_0571, (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE }, //バトル
  { msg_wifilobby_060,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE },   //ルール
  { msg_wifilobby_062,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER },  //シューター
  { msg_wifilobby_096,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE },  //決定
  { msg_wifilobby_097,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH },  //ルールを見る 
  { msg_wifilobby_032,  (u32)BMPMENULIST_CANCEL },
};


#define _NORIMIT (0)
#define _FLAT (1)

_infoMenu _parentRuleInfoMenuList[] = {
  { msg_wifilobby_060, (u32)_NORIMIT },  //せいげんなし
  { msg_wifilobby_061, (u32)_FLAT },  //フラットルール
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};


#define _SHOOTER_ON (0)
#define _SHOOTER_OFF (1)

_infoMenu _parentShooterInfoMenuList[] = {
  { msg_wifilobby_062, (u32)_SHOOTER_ON },  //シューターあり
  { msg_wifilobby_063, (u32)_SHOOTER_OFF },  //シューターなし
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

#define _TEMOTI (0)
#define _BATTLEBOX (1)


_infoMenu _parentPokePartyMenuList[] = {
  { msg_wifilobby_098, (u32)_TEMOTI },  //てもち
  { msg_wifilobby_099, (u32)_BATTLEBOX },  //バトルボックス
};

static void	_print_callback(BMPMENULIST_WORK * wk,u32 param,u8 y);


///選択メニューのリスト
static BMPMENULIST_HEADER _parentInfoMenuListHeader = {
  NULL,     // 表示文字データポインタ
  NULL,         // カーソル移動ごとのコールバック関数
  _print_callback,         // 一列表示ごとのコールバック関数
  NULL,                   //
  elementof(_parentInfoMenuList),// リスト項目数
  elementof(_parentInfoMenuList),// 表示最大項目数
  0,            // ラベル表示Ｘ座標
  16,            // 項目表示Ｘ座標
  0,            // カーソル表示Ｘ座標
  0,            // 表示Ｙ座標
  FBMP_COL_BLACK,     // 文字色
  FBMP_COL_WHITE,     // 背景色
  FBMP_COL_BLK_SDW,   // 文字影色
  0,            // 文字間隔Ｘ
  16,           // 文字間隔Ｙ
  BMPMENULIST_NO_SKIP,    // ページスキップタイプ
  0,//FONT_SYSTEM,        // 文字指定
  0,            // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,      //文字サイズX(ドット
  _BMPMENULIST_FONTSIZE,      //文字サイズY(ドット
  NULL,   //表示に使用するメッセージバッフ
  NULL,   //表示に使用するプリントユーティ
  NULL,   //表示に使用するプリントキュー
  NULL,   //表示に使用するフォントハンドル
};

///選択メニューのリスト
static BMPMENULIST_HEADER _parentInfoBattleMenuListHeader = {
  NULL,     // 表示文字データポインタ
  NULL,         // カーソル移動ごとのコールバック関数
  NULL,         // 一列表示ごとのコールバック関数
  NULL,                   //
  NELEMS(_parentSingleInfoMenuList),  // リスト項目数
  NELEMS(_parentSingleInfoMenuList),  // 表示最大項目数
  0,            // ラベル表示Ｘ座標
  16,            // 項目表示Ｘ座標
  0,            // カーソル表示Ｘ座標
  0,            // 表示Ｙ座標
  FBMP_COL_BLACK,     // 文字色
  FBMP_COL_WHITE,     // 背景色
  FBMP_COL_BLK_SDW,   // 文字影色
  0,            // 文字間隔Ｘ
  16,           // 文字間隔Ｙ
  BMPMENULIST_NO_SKIP,    // ページスキップタイプ
  0,//FONT_SYSTEM,        // 文字指定
  0,            // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //文字サイズX(ドット
  _BMPMENULIST_FONTSIZE,      //文字サイズY(ドット
  NULL,   //表示に使用するメッセージバッフ
  NULL,   //表示に使用するプリントユーティ
  NULL,   //表示に使用するプリントキュー
  NULL,   //表示に使用するフォントハンドル
};


///選択メニューのリスト
static BMPMENULIST_HEADER _parentCustomMenuListHeader = {
  NULL,     // 表示文字データポインタ
  NULL,         // カーソル移動ごとのコールバック関数
  NULL,         // 一列表示ごとのコールバック関数
  NULL,                   //
  NELEMS(_parentCustomInfoMenuList),  // リスト項目数
  NELEMS(_parentCustomInfoMenuList),  // 表示最大項目数
  0,            // ラベル表示Ｘ座標
  16,            // 項目表示Ｘ座標
  0,            // カーソル表示Ｘ座標
  0,            // 表示Ｙ座標
  FBMP_COL_BLACK,     // 文字色
  FBMP_COL_WHITE,     // 背景色
  FBMP_COL_BLK_SDW,   // 文字影色
  0,            // 文字間隔Ｘ
  16,           // 文字間隔Ｙ
  BMPMENULIST_NO_SKIP,    // ページスキップタイプ
  0,//FONT_SYSTEM,        // 文字指定
  0,            // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //文字サイズX(ドット
  _BMPMENULIST_FONTSIZE,      //文字サイズY(ドット
  NULL,   //表示に使用するメッセージバッフ
  NULL,   //表示に使用するプリントユーティ
  NULL,   //表示に使用するプリントキュー
  NULL,   //表示に使用するフォントハンドル
};


#define PARENTMENU_Y  ( 5 )

//------------------------------------------------------------------
/**
 * $brief   一行コールバック
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void	_print_callback(BMPMENULIST_WORK * wk,u32 param,u8 y)
{
  WIFIP2PMATCH_WORK *pWork = BmpMenuList_GetWorkPtr(wk);


  if(WIFI_STATUS_GetVChatStatus( pWork->pMatch )==FALSE){
    switch(param){
    case WIFI_GAME_VCT:
    case WIFI_GAME_TVT:
      BmpMenuList_ColorControl(wk,FBMP_COL_BLK_SDW,FBMP_COL_WHITE,FBMP_COL_BLK_SDW);
      break;
    default:
      BmpMenuList_ColorControl(wk,FBMP_COL_BLACK,FBMP_COL_WHITE,FBMP_COL_BLK_SDW);
      break;
    }
  }
}




static void _modeSelectMenuBase( WIFIP2PMATCH_WORK *wk,BMPMENULIST_HEADER* plisth, _infoMenu* p_menu,int length, int menutype, int width )
{
  int i;
  //BMPMENULIST_HEADER list_h;

 // GFL_STD_MemCopy(plisth,&list_h,sizeof(BMPMENULIST_HEADER));

  PRINTSYS_QUE_Clear(wk->SysMsgQue);


  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, p_menu[i].str_id, p_menu[i].param, HEAPID_WIFIP2PMATCH );
  }

  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMPウィンドウ生成
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 32-width-1, PARENTMENU_Y, width, length * 2, FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  COMM_TALK_WIN_CGX_NUM, MENU_WIN_PAL );
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_OFF, GFL_ARCUTIL_TRANSINFO_GetPos(wk->talkwin_m2), COMM_MESFRAME_PAL );
  GFL_BMPWIN_MakeTransWindow(wk->SubListWin);


  plisth->list = wk->submenulist;
  plisth->win = wk->SubListWin;

  plisth->print_que = wk->SysMsgQue;
  PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
  plisth->print_util = &wk->SysMsgPrintUtil;
  plisth->font_handle = wk->fontHandle;
  plisth->work = wk;

  //GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->SubListWin), WINCLR_COL(FBMP_COL_WHITE) );

  wk->sublw = BmpMenuList_Set(plisth, 0, wk->singleCur[menutype], HEAPID_WIFIP2PMATCH);
	BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );

//  GFL_BMPWIN_TransVramCharacter(wk->SubListWin);
//  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  //GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);
}




static void _ParentModeSelectMenu( WIFIP2PMATCH_WORK *wk )
{
  _modeSelectMenuBase(wk, &_parentInfoMenuListHeader,
                      _parentInfoMenuList, elementof(_parentInfoMenuList), _MENUTYPE_GAME, 17);
}

//------------------------------------------------------------------
/**
 * $brief   マッチング申し込みメニュー
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

enum{
  _CONNECTING = 1,
  _INFOVIEW,
};


static const _infoMenu _childMatchMenuList[] = {
  { msg_wifilobby_081, (u32)_CONNECTING },
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

///選択メニューのリスト
static const BMPMENULIST_HEADER _childMatchMenuListHeader = {
  NULL,     // 表示文字データポインタ
  NULL,         // カーソル移動ごとのコールバック関数
  NULL,         // 一列表示ごとのコールバック関数
  NULL,                   //
  NELEMS(_childMatchMenuList),  // リスト項目数
  NELEMS(_childMatchMenuList),  // 表示最大項目数
  0,            // ラベル表示Ｘ座標
  16,            // 項目表示Ｘ座標
  0,            // カーソル表示Ｘ座標
  0,            // 表示Ｙ座標
  FBMP_COL_BLACK,     // 文字色
  FBMP_COL_WHITE,     // 背景色
  FBMP_COL_BLK_SDW,   // 文字影色
  0,            // 文字間隔Ｘ
  16,           // 文字間隔Ｙ
  BMPMENULIST_NO_SKIP,    // ページスキップタイプ
  0,//FONT_SYSTEM,        // 文字指定
  0,            // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //文字サイズX(ドット
  _BMPMENULIST_FONTSIZE,      //文字サイズY(ドット
  NULL,   //表示に使用するメッセージバッフ
  NULL,   //表示に使用するプリントユーティ
  NULL,   //表示に使用するプリントキュー
  NULL,   //表示に使用するフォントハンドル
};


static void _ChildModeMatchMenuDisp( WIFIP2PMATCH_WORK *wk )
{
#if 1
    int i,length;
    BMPMENULIST_HEADER list_h;
    u16 friendNo,status,vchat;
    WIFI_STATUS* p_status;

    length = NELEMS(_childMatchMenuList);
    list_h = _childMatchMenuListHeader;
    friendNo = WIFI_MCR_PlayerSelect( &wk->matchroom );
    p_status = WifiFriendMatchStatusGet( friendNo - 1 );
    status = _WifiMyStatusGet( wk, p_status );
    vchat = WIFI_STATUS_GetVChatStatus(p_status);

//  wk->keepVChat = vchat;


  vchat += WIFI_STATUS_GetVChatStatus( wk->pMatch );

    // その人の情報を表示
    //  WifiP2PMatch_UserDispOn_MyAcces( wk, friendNo, HEAPID_WIFIP2PMATCH );


    wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
    i = 0;

    for(i = 0; i < NELEMS(_childMatchMenuList);i++){
      if(i == 0 && (WIFI_STATUS_RECRUIT != status && status != WIFI_STATUS_PLAY_AND_RECRUIT && status != WIFI_STATUS_WAIT)){  // もうしこむを出さない条件
        list_h.line -= 1;
        list_h.count -= 1;
        length -= 1;
      }
      else{
        BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                          _childMatchMenuList[i].str_id,
                                          _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
      }
    }
    //BMPウィンドウ生成
    wk->SubListWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME2_M, 16, 11+ ((3-length)*2), 15  , length * 2,
      FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
    //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );
    list_h.list = wk->submenulist;
    list_h.win = wk->SubListWin;

    list_h.print_que = wk->SysMsgQue;
    PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
    list_h.print_util = &wk->SysMsgPrintUtil;
    list_h.font_handle = wk->fontHandle;

    //wk->sublw = BmpMenuWork_ListCreate(NELEMS(_childMatchMenuList),HEAPID_WIFIP2PMATCH);
    wk->sublw = BmpMenuList_Set(&list_h, 0, 0, HEAPID_WIFIP2PMATCH);
	BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );
    GFL_BMPWIN_TransVramCharacter(wk->SubListWin);
    GFL_BMPWIN_MakeScreen(wk->SubListWin);
    GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);
#endif
}

//------------------------------------------------------------------
/**
 * $brief   バトルの詳細部分を決める WIFIP2PMATCH_MODE_SUBBATTLE_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret )
{

  _parentInfoBattleMenuListHeader.count = elementof(_parentSingleInfoMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentSingleInfoMenuList);

  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentSingleInfoMenuList,
                      elementof(_parentSingleInfoMenuList), _MENUTYPE_BATTLE_MODE,17);

}




//------------------------------------------------------------------
/**
 * $brief   自分の状態を表示
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _userDataInfoDisp(WIFIP2PMATCH_WORK * wk)
{
  int col,sex, vct_icon;
  int msg_id;
  MYSTATUS* pMy = SaveData_GetMyStatus(wk->pSaveData);
  u32 status,gamemode;

  if(!wk->MyInfoWin){
    return;
  }
  GFL_FONTSYS_SetDefaultColor();
  {
    STRBUF* pSTRBUF = MyStatus_CreateNameString(pMy, HEAPID_WIFIP2PMATCH);
    // 初期化
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WINCLR_COL(FBMP_COL_WHITE) );
    sex = MyStatus_GetMySex(pMy);
    if( sex == PM_MALE ){
      _COL_N_BLUE;
    }
    else{
      _COL_N_RED;
    }
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WIFIP2PMATCH_PLAYER_DISP_NAME_X, 0,
                    pSTRBUF, wk->fontHandle);
    GFL_STR_DeleteBuffer(pSTRBUF);
  }
  status = _WifiMyStatusGet( wk, wk->pMatch );
  gamemode = _WifiMyGameModeGet( wk, wk->pMatch );
  // msg_idとカラーを取得
  msg_id = MCVSys_StatusMsgIdGet( status, gamemode, &col );
  if( gamemode == WIFI_GAME_VCT && status == WIFI_STATUS_PLAYING ){      // VCT中
    if(GFL_NET_DWC_GetFriendIndex() == -1){
      GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
    }
    else{
      sex = WifiList_GetFriendInfo(wk->pList, GFL_NET_DWC_GetFriendIndex(), WIFILIST_FRIEND_SEX);
      if( PM_MALE == sex){
        _COL_N_BLUE;
      }
      else if( PM_FEMALE == sex){
        _COL_N_RED;
      }
      WifiList_GetFriendName(wk->pList,GFL_NET_DWC_GetFriendIndex(),wk->pExpStrBuf);
    }
  }else{
    GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  }
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWin), WIFIP2PMATCH_PLAYER_DISP_ST_X, 0,
                  wk->pExpStrBuf, wk->fontHandle);

  //状態アイコン設定
  // Frame1の対応する位置にアイコンを転送
  WifiP2PMatchFriendListStIconWrite( &wk->icon, GFL_BG_FRAME1_M,
                                     PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y,
                                     status,gamemode );
  if( WIFI_STATUS_GetVChatStatus(wk->pMatch) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }

  WifiP2PMatchFriendListIconWrite(  &wk->icon, GFL_BG_FRAME1_M,
                                    PLAYER_DISP_VCTICON_POS_X, PLAYER_DISP_VCTICON_POS_Y,
                                    vct_icon, 0 );

  GFL_BMPWIN_ClearScreen(wk->MyInfoWin);
  GFL_BMPWIN_MakeScreen(wk->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_M);
}


//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void MainMenuMsgInit(WIFIP2PMATCH_WORK *wk)
{
  int i,col;

  _BmpWinDel(wk->MyInfoWinBack);
  wk->MyInfoWinBack = GFL_BMPWIN_Create(GFL_BG_FRAME3_M, 5, 1, 22, 2,
                                        _NUKI_FONT_PALNO,  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( wk->MyInfoWinBack );
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_018, wk->TitleString );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), 0, 0, wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);
}


//------------------------------------------------------------------
/**
 * $brief   確保したBMPWINを解放
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BmpWinDelete( WIFIP2PMATCH_WORK *wk )
{
  int i;

  // メッセージ停止
  EndMessageWindowOff( wk );

  if(wk->MyInfoWinBack){
    GFL_BMPWIN_Delete(wk->MyInfoWinBack);
    wk->MyInfoWinBack = NULL;
  }
  if(wk->MsgWin){
    GFL_BMPWIN_Delete(wk->MsgWin);
    wk->MsgWin=NULL;
  }
  if(wk->MyWin){
    GFL_BMPWIN_Delete(wk->MyWin);
    wk->MyWin=NULL;
  }
  if(wk->MyInfoWin){
    GFL_BMPWIN_Delete(wk->MyInfoWin);
    wk->MyInfoWin=NULL;
  }
  if(wk->ListWin){
    GFL_BMPWIN_Delete(wk->ListWin);
    wk->ListWin=NULL;
  }
  if(wk->SysMsgWin){
    GFL_BMPWIN_Delete(wk->SysMsgWin);
    wk->SysMsgWin = NULL;
  }
  if(wk->lw){
    BmpMenuList_Exit(wk->lw, NULL, NULL);
    wk->lw = NULL;
  }
  if(wk->menulist){
    BmpMenuWork_ListDelete( wk->menulist );
    wk->menulist = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief   Messageワーク初期化
 *
 * @param   wk    WIFIP2PMATCH_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitMessageWork( WIFIP2PMATCH_WORK *wk )
{
  int i;
  int flag;

  wk->TalkString =  GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->pTemp = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );        // 入力登録時の一時バッファ
  wk->pExpStrBuf = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->TitleString = GFL_STR_CreateBuffer( TITLE_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->SysMsgQue = PRINTSYS_QUE_Create( HEAPID_WIFIP2PMATCH );
  wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_WIFIP2PMATCH , HEAPID_WIFIP2PMATCH , 1 , 0 );


}


//------------------------------------------------------------------
/**
 * @brief    Message用ワーク解放
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeMessageWork( WIFIP2PMATCH_WORK *wk )
{
  int i;


  GFL_STR_DeleteBuffer( wk->TitleString );
  GFL_STR_DeleteBuffer( wk->TalkString );
  GFL_STR_DeleteBuffer( wk->pTemp );
  if(wk->pStream){
    PRINTSYS_PrintStreamDelete( wk->pStream );
    wk->pStream=NULL;
  }

  GFL_STR_DeleteBuffer(wk->pExpStrBuf);
  GFL_TCBL_Exit(wk->pMsgTcblSys);
  PRINTSYS_QUE_Clear(wk->SysMsgQue);
  PRINTSYS_QUE_Delete(wk->SysMsgQue);
  wk->SysMsgQue=NULL;
}





//----------------------------------------------------------------------------
/**
 *  @brief  ユーザーデータ表示
 *
 *  @param  wk  システムワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int sex;
  int pal;

  sex = WifiList_GetFriendInfo( wk->pList,
                                wk->view.touch_friendNo - 1, WIFILIST_FRIEND_SEX );

  if(sex == PM_FEMALE){
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_GIRL;
  }else{
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_MAN;
  }
  GFL_BG_LoadScreenBufferOfs( GFL_BG_FRAME2_S, wk->view.p_userscrn[0]->rawData,
                     wk->view.p_userscrn[0]->szByte,0 );
  //  GFL_BG_ScreenBufSet( GFL_BG_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
  //      wk->view.p_userscrn[wk->view.user_dispno]->szByte );

  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, 0, 0,
                              32, 24, pal );

  // BG３面のスクリーンをクリア
  GFL_BG_ClearFrame( GFL_BG_FRAME3_S);

  MCVSys_UserDispDrawFrontierOffScrn( wk ); // 非表示スクリーン設定


  // その人のことを描画
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0x0 );

  // 描画
  MCVSys_UserDispDrawType00( wk, heapID );

//  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );

  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);

  // メッセージ面の表示設定
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}


// PAGE 1
#define MCV_USERD_NAME_X  ( 32-8 )
#define MCV_USERD_NAME_Y  ( 8 )
#define MCV_USERD_ST_X  ( 104-8 )
#define MCV_USERD_ST_Y  ( 8 )
#define MCV_USERD_GR_X  ( 8-8 )
#define MCV_USERD_GR_Y  ( 32 )
#define MCV_USERD_VS_X  ( 8-8 )
#define MCV_USERD_VS_Y  ( 56 )
#define MCV_USERD_VS_WIN_X  ( 120-8 )
#define MCV_USERD_VS_WIN_Y  ( 56 )
#define MCV_USERD_VS_LOS_X  ( 184-8 )
#define MCV_USERD_TR_X    ( 8-8 )
#define MCV_USERD_TR_Y    ( 80 )
#define MCV_USERD_TRNUM_X ( 152 )
#define MCV_USERD_DAY_X   ( 8-8 )
#define MCV_USERD_DAY_Y   ( 128 )
#define MCV_USERD_DAYNUM_X  ( 152 )
#define MCV_USERD_ICON_X  ( 2 )
#define MCV_USERD_VCTICON_X ( 28 )
#define MCV_USERD_ICON_Y  ( 2 )


// 通常
static void MCVSys_UserDispDrawType00( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int sex;
  int col;
  int msg_id;
  int friendNo;
  int num;
  int vct_icon;
  WIFI_STATUS* p_status;
  u32 status,gamemode;

  GFL_FONTSYS_SetDefaultColor();

  friendNo = wk->view.touch_friendNo - 1;

  sex = WifiList_GetFriendInfo( wk->pList, friendNo, WIFILIST_FRIEND_SEX );

  // トレーナー名
  if( sex == PM_MALE ){
    _COL_N_BLUE;
  }else{
    _COL_N_RED;
  }
  MCVSys_FriendNameSet(wk, friendNo);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_NAME_X, MCV_USERD_NAME_Y,
                  wk->TitleString, wk->fontHandle);

  p_status = WifiFriendMatchStatusGet( friendNo );
  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  // 状態
  msg_id = MCVSys_StatusMsgIdGet( status, gamemode, &col );
  GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_ST_X, MCV_USERD_ST_Y,
                  wk->pExpStrBuf, wk->fontHandle);


  // 対戦成績
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_X, MCV_USERD_VS_Y,
                    wk->TitleString, wk->fontHandle);
    // かち
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_WIN_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
    // まけ
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_LOS_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
  }
  // ポケモン交換
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_038, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TR_X,  MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_TRADE_NUM);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_039, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TRNUM_X, MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle);
  }


  // 最後に遊んだ日付
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_040, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAY_X, MCV_USERD_DAY_Y,
                    wk->TitleString, wk->fontHandle);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_DAY);
    if(num!=0){
      WORDSET_RegisterNumber(wk->view.p_wordset, 2, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_YEAR);
      WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_MONTH);
      WORDSET_RegisterNumber(wk->view.p_wordset, 1, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_041, wk->pExpStrBuf );
      WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAYNUM_X, MCV_USERD_DAY_Y,
                      wk->TitleString, wk->fontHandle);
    }
  }
  GFL_BMPWIN_MakeScreen(wk->view.userWin);
  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_S);

  // アイコン
  WifiP2PMatchFriendListStIconWrite(  &wk->icon, GFL_BG_FRAME2_S,
                                      MCV_USERD_ICON_X, MCV_USERD_ICON_Y,
                                      status,gamemode );
  if( WIFI_STATUS_GetVChatStatus(p_status) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }
  WifiP2PMatchFriendListIconWrite( &wk->icon, GFL_BG_FRAME2_S,
                                   MCV_USERD_VCTICON_X, MCV_USERD_ICON_Y,
                                   vct_icon, 0 );

}

// フロンティア非表示スクリーン描画
#define MCV_USERD_NOFR_SCRN_X   ( 0x1a )
#define MCV_USERD_NOFR_SCRN_Y   ( 0 )
#define MCV_USERD_NOFR_SCRN_SIZX  ( 0x1 )
#define MCV_USERD_NOFR_SCRN_SIZY  ( 0x1 )

//----------------------------------------------------------------------------
/**
 *  @brief  フロンティア非表示モードのスクリーンに変更
 *
 *  @param  wk    ワーク
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk )
{
  int i;
  int roop;

  // フロンティア施設数取得
  roop = 1;//WF_USERDISPTYPE_NUM - WF_USERDISPTYPE_BLTW;

  for( i=0; i<roop; i++ ){
    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X+(MCV_USERD_NOFR_SCRN_SIZX*i), MCV_USERD_NOFR_SCRN_Y,
                              MCV_USERD_NOFR_SCRN_SIZX, MCV_USERD_NOFR_SCRN_SIZY,
                              wk->view.p_useretcscrn->rawData,
                              0, 0,
                              wk->view.p_useretcscrn->screenWidth/8, wk->view.p_useretcscrn->screenHeight/8 );
  }

  // パレット変更
  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X, MCV_USERD_NOFR_SCRN_Y,
                              (MCV_USERD_NOFR_SCRN_SIZX*roop), MCV_USERD_NOFR_SCRN_SIZY,
                              MCV_PAL_BTTN+MCV_PAL_BTTN_NONE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  状態に対応したメッセージデータ表示
 *
 *  @param  status  状態
 *
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------
static u32 MCVSys_StatusMsgIdGet( u32 status, u32 gamemode, int* col )
{
  u32 msg_id;
  //  *col = _COL_N_BLACK;
  _COL_N_BLACK;


  if(status == WIFI_STATUS_PLAYING){
    _COL_N_GRAY;
  }
  else{
    _COL_N_BLACK;
  }

  {//WIFI_GAME_e参照
    u32 message[]={msg_wifilobby_046,msg_wifilobby_046,msg_wifilobby_1014,msg_wifilobby_027, msg_wifilobby_076,
    msg_wifilobby_025, msg_wifilobby_0571,msg_wifilobby_0571,msg_wifilobby_0572,msg_wifilobby_0572,
    msg_wifilobby_0573,msg_wifilobby_0573,msg_wifilobby_0574,msg_wifilobby_0574,msg_wifilobby_056};

    msg_id = gamemode;
    if(gamemode > WIFI_GAME_UNKNOWN){
      msg_id = WIFI_GAME_UNKNOWN;
    }
    
    msg_id = message[msg_id];
  }

  return msg_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルのモード設定
 *  @param  status  状態
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------

static void _battleCustomSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentCustomInfoMenuList[0].str_id = msg_wifilobby_0571 + wk->battleMode;
  _parentCustomInfoMenuList[1].str_id =  msg_wifilobby_060 + wk->battleRule;
  _parentCustomInfoMenuList[2].str_id =  msg_wifilobby_062 + (1-wk->battleShooter);

  _modeSelectMenuBase(wk, &_parentCustomMenuListHeader, _parentCustomInfoMenuList,
                      elementof(_parentCustomInfoMenuList), _MENUTYPE_BATTLE_CUSTOM,19);
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルのモード設定
 *  @param  status  状態
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------

static void _battleModeSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentInfoBattleMenuListHeader.count = elementof(_parentSingleInfoMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentSingleInfoMenuList);

  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentSingleInfoMenuList,
                      elementof(_parentSingleInfoMenuList), _MENUTYPE_BATTLE_MODE,17);
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルのルール設定
 *  @param  status  状態
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------

static void _battleRuleSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentInfoBattleMenuListHeader.count = elementof(_parentRuleInfoMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentRuleInfoMenuList);

  
  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentRuleInfoMenuList,
                      elementof(_parentRuleInfoMenuList), _MENUTYPE_BATTLE_RULE,17);
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルのシューター設定
 *  @param  status  状態
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------

static void _battleShooterSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentInfoBattleMenuListHeader.count = elementof(_parentShooterInfoMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentShooterInfoMenuList);

  
  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentShooterInfoMenuList,
                      elementof(_parentShooterInfoMenuList), _MENUTYPE_BATTLE_SHOOTER,18);
}




//----------------------------------------------------------------------------
/**
 *  @brief  ポケパーティー選択
 *  @param  status  状態
 *  @return メッセージデータ
 */
//-----------------------------------------------------------------------------

static void _battlePokePartySelectMenu( WIFIP2PMATCH_WORK *wk )
{
  _parentInfoBattleMenuListHeader.count = elementof(_parentPokePartyMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentPokePartyMenuList);

  NET_PRINT("elementof(_parentPokePartyMenuList) %d\n",elementof(_parentPokePartyMenuList));
  
  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentPokePartyMenuList,
                      elementof(_parentPokePartyMenuList), _MENUTYPE_POKEPARTY,18);
}

//==================================================================
/**
 * レギュレーション：表示
 *
 * @param   unisys		
 * @param   fieldWork		
 * @param   fail_bit    レギュレーションNGbit
 * @param   shooter_type  TRUE:シューター有　FALSE:シューター無し
 * @param   regwin_type    NGREG_TYPE_???
 */
//==================================================================


static void _Menu_RegulationSetup(WIFIP2PMATCH_WORK* wk, u32 fail_bit, BOOL shooter_type, REGWIN_TYPE regwin_type)
{
  int category;
  PRINTSYS_LSB color;
  u16 title_msgid;
  
  if(shooter_type == TRUE){
    shooter_type = REGULATION_SHOOTER_VALID;
  }
  else{
    shooter_type = REGULATION_SHOOTER_INVALID;
  }


  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);

  wk->SysMsgWin= GFL_BMPWIN_Create(  GFL_BG_FRAME2_M,
    REGWINDOW_WIN_PX, REGWINDOW_WIN_PY,
    REGWINDOW_WIN_SX, REGWINDOW_WIN_SY,
    COMM_MESFONT_PAL, GFL_BMP_CHRAREA_GET_B);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // システムウインドウ枠描画
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON_V,
                     GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFONT_PAL );


  wk->rpm = PokeRegulation_CreatePrintMsg( wk->pRegulation, wk->WordSet, HEAPID_WIFIP2PMATCH, shooter_type);

  if(regwin_type == REGWIN_TYPE_RULE){
    STRBUF* cupname = Regulation_CreateCupName(wk->pRegulation, HEAPID_WIFIP2PMATCH);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 0, 0, cupname, wk->fontHandle );
    GFL_STR_DeleteBuffer(cupname);
  }
  else{
    if(regwin_type == REGWIN_TYPE_NG_TEMOTI){
      title_msgid = msg_wifilobby_098;
    }
    else{
      title_msgid = msg_wifilobby_099;
    }
    GFL_MSG_GetString(  wk->MsgManager, title_msgid, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 0, 0, wk->pExpStrBuf , wk->fontHandle);
  }
  
  for(category = 0; category < REGULATION_CATEGORY_MAX; category++){
    if(fail_bit & (1 << category)){
      color = REG_FAIL_COLOR;
    }
    else{
      color = REG_NORMAL_COLOR;
    }
    PRINTSYS_PrintColor(GFL_BMPWIN_GetBmp(wk->SysMsgWin),0, (4 + 2*category)*8,
                        wk->rpm->category[category], wk->fontHandle, color);
    PRINTSYS_PrintColor(GFL_BMPWIN_GetBmp(wk->SysMsgWin), 14*8, (4 + 2*category)*8,
                        wk->rpm->prerequisite[category], wk->fontHandle, color);
  }
  GFL_BMPWIN_TransVramCharacter(wk->SysMsgWin);
}

