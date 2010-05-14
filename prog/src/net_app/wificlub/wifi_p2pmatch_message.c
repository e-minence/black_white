//============================================================================================
/**
 * @file    wifi_p2pmatch_message.c
 * @bfief   WIFIP2P�}�b�`���O���  ���b�Z�[�W�����؂�o��
 * @author  k.ohno
 * @date    10.01.09
 */
//============================================================================================

#include "net_app/union/union_beacon_tool.h"
#include "system/tr2dgra.h"
#include "pokeicon/pokeicon.h"
#include "system/time_icon.h"


#define _TOUCHBAR_BG_PALPOS (12)     //�^�b�`�o�[�̐F   �i�P�j�{
#define _TOUCHBAR_OBJ_PALPOS (12)     //�^�b�`�o�[�̐F  �i�P�j�{

#define CLACT_PALNUM_TRGRA (0)

#define PARENTMENU_Y  ( 5 )
#define PARENTMENU2_Y  ( 13 )


enum{
  POKEICON_POS_START_X = 3*8,    ///<�|�P�����A�C�R���z�u�J�n�ʒuX
  POKEICON_POS_SPACE_X = 5*8,    ///<�|�P�����A�C�R�����̔z�u�ԊuX
  POKEICON_POS_Y = 4*8,          ///<�|�P�����A�C�R��Y���W

  ITEMICON_POS_START_X = POKEICON_POS_START_X+8,    ///<�A�C�e���A�C�R���z�u�J�n�ʒuX
  ITEMICON_POS_SPACE_X = POKEICON_POS_SPACE_X,    ///<�A�C�e���A�C�R�����̔z�u�ԊuX
  ITEMICON_POS_Y = POKEICON_POS_Y+8,          ///<�A�C�e���A�C�R��Y���W
  
  LV_POS_X = 0,
  LV_POS_SPACE_X = POKEICON_POS_SPACE_X,
  LV_POS_Y = 4*8,
  
  SEX_POS_X = 4*8,
  SEX_POS_SPACE_X = POKEICON_POS_SPACE_X,
  SEX_POS_Y = 2*8,
  
  POKESTATUS_BMP_X = 1,
  POKESTATUS_BMP_Y = 2,
  POKESTATUS_BMP_SIZE_X = 30,
  POKESTATUS_BMP_SIZE_Y = 6,
};



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

static void _TouchResExit( WIFIP2PMATCH_WORK *wk );
static void _TrainerOAMFree( WIFIP2PMATCH_WORK *wk );


//------------------------------------------------------------------
/**
 * $brief   yesno�E�C���h�E���o��
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateM2(WIFIP2PMATCH_WORK *wk)
{
  GFL_FONTSYS_SetDefaultColor();
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDat, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}

//------------------------------------------------------------------
/**
 * $brief   yesno�E�C���h�E���o��
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateM1(WIFIP2PMATCH_WORK *wk)
{
  GFL_FONTSYS_SetDefaultColor();
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDatSys2, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m1), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}


//------------------------------------------------------------------
/**
 * $brief   yesno�E�C���h�E���o��
 * @param   msg_index
 * @retval  int       int friend = GFL_NET_DWC_GetFriendIndex();

 */
//------------------------------------------------------------------

static void _yenowinCreateSys(WIFIP2PMATCH_WORK *wk)
{
  GFL_FONTSYS_SetDefaultColor();
  wk->pYesNoWork =
    BmpMenu_YesNoSelectInit( &_yesNoBmpDatSys, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), MENU_WIN_PAL, 0, HEAPID_WIFIP2PMATCH );
}


static int _bmpMenu_YesNoSelectMain(WIFIP2PMATCH_WORK *wk)
{
  int ret;

  ret = BmpMenu_YesNoSelectMain(wk->pYesNoWork);
  if( ret != BMPMENU_NULL ){
    wk->pYesNoWork=NULL;
  }
  return ret;
}

//------------------------------------------------------------------
/**
 * $brief   �F�l�ԍ��̖��O��expand����
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
 * $brief   BMP�E�B���h�E������
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
 * $brief   �^�C�}�[�A�C�R��������
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _timeWaitIconDel(WIFIP2PMATCH_WORK *wk)
{
  if(wk->pTimeIcon){
    TIMEICON_Exit(wk->pTimeIcon);
    wk->pTimeIcon=NULL;
  }
//  wk->MsgWin = _BmpWinDel(wk->MsgWin);
}


//------------------------------------------------------------------
/**
 * $brief   ���b�Z�[�W�̒�~
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( WIFIP2PMATCH_WORK *wk )
{
//  WifiP2PMatchMessage_CursorEnd(wk);
  _timeWaitIconDel(wk);
  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  wk->MsgWin = _BmpWinDel(wk->MsgWin);
}

//------------------------------------------------------------------
/**
 * $brief   ���b�Z�[�W���~���Ă悢���ǂ���
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static BOOL WifiP2PMatchMessageEndCheck(WIFIP2PMATCH_WORK* wk)
{
  int state;
  
  if(wk->pStream){
    if(wk->MsgWin){
      APP_KEYCURSOR_Main( wk->pKeyCursor, wk->pStream, wk->MsgWin );
    }
    state = PRINTSYS_PrintStreamGetState( wk->pStream );
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
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
}



//------------------------------------------------------------------
/**
 * $brief   ��b�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrintS( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem, BOOL bStream )
{
  // ������擾
  u8 speed = MSGSPEED_GetWait();

  // TimeWaitIcon�j��
  _timeWaitIconDel( wk );
  //WifiP2PMatchMessage_CursorEnd(wk);

  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  if(wk->pStream){
    PRINTSYS_PrintStreamDelete( wk->pStream );
    wk->pStream = NULL;
  }

  wk->MsgWin=GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_MSG_WIN_PX, COMM_MSG_WIN_PY,
    COMM_MSG_WIN_SX, COMM_MSG_WIN_SY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  if(bSystem){
    GFL_MSG_GetString(  wk->SysMsgManager, msgno, wk->pExpStrBuf );
  }
  else{
    GFL_MSG_GetString(  wk->MsgManager, msgno, wk->pExpStrBuf );
  }
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, wk->pExpStrBuf );

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->MsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(wk->MsgWin);

  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );

  if(!bStream){
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MsgWin), 0, 0, wk->TalkString, wk->fontHandle);
  }
  else{
    wk->pStream = PRINTSYS_PrintStream(wk->MsgWin ,0,0, wk->TalkString, wk->fontHandle,
                                       MSGSPEED_GetWait(), wk->pMsgTcblSys, 2,HEAPID_WIFIP2PMATCH, 15);
  }
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
  GFL_BMPWIN_MakeScreen(wk->MsgWin);
}


//------------------------------------------------------------------
/**
 * $brief   ��b�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem )
{
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);

  WifiP2PMatchMessagePrintS(wk,msgno, bSystem, TRUE);
}

//------------------------------------------------------------------
/**
 * $brief   ��b�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------


static void WifiP2PMatchMessagePrintDirect( WIFIP2PMATCH_WORK *wk, int msgno, BOOL bSystem )
{
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);

  WifiP2PMatchMessagePrintS(wk,msgno, bSystem, FALSE);
}


//==============================================================================
/**
 * @brief   �`��J�n�ʒu���}�C�i�X�����ɂ����Ă��`��ł���BmpWinPrint���b�p�[
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
 *  @brief  �r�b�g�}�b�v�ɃA�C�R���f�[�^����������
 *
 *  @param  p_bmp   �r�b�g�}�b�v
 *  @param  p_data    �A�C�R���f�[�^
 *  @param  x     �������݂����W
 *  @param  y     �������݂����W
 *  @param  icon_type �A�C�R���^�C�v
 *  @param  col     �J���[
 */
//-----------------------------------------------------------------------------
static void WifiP2PMatchFriendListBmpIconWrite(  GFL_BMPWIN* p_bmp, WIFIP2PMATCH_ICON* p_data, u16 x, u16 y, u32 icon_type, u32 col,int pal )
{
  //  int pal;

  // ��������
  _BmpWinPrint_Rap( p_bmp,
                    p_data->p_char->pRawData,
                    (PLAYER_DISP_ICON_SCRN_X*8)*icon_type, 0,
                    p_data->p_char->W*8, p_data->p_char->H*8,
                    x, y,
                    PLAYER_DISP_ICON_SCRN_X*8, PLAYER_DISP_ICON_SCRN_Y*8,
                    HEAPID_WIFIP2PMATCH);

  //  // �p���b�gNo�擾
  //  pal = WifiP2PMatchBglFrmIconPalGet( p_bmp->frmnum );

  // �p���b�g�i���o�[�����킹��
  GFL_BMPWIN_SetPalette( p_bmp, pal+col );
}


//------------------------------------------------------------------
/**
 * $brief   �V�X�e�����b�Z�[�W�E�C���h�E�\��
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------

static void _systemMessagePrint( WIFIP2PMATCH_WORK *wk, int msgno )
{
  _timeWaitIconDel(wk);
  //WifiP2PMatchMessage_CursorEnd(wk);
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);


  wk->SysMsgWin= GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    COMM_SYS_WIN_PX, COMM_SYS_WIN_PY,
    COMM_SYS_WIN_SX, COMM_SYS_WIN_SY,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);

  //  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_SYS_WIN_CGX, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*MCV_SYSFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON_V,
                     GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFONT_PAL );

  //    GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  //   GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SysMsgWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
  // ������`��J�n
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
 * $brief   �ҋ@��ԂɂȂ�ׂ̑I�����j���[ WIFIP2PMATCH_MODE_SELECT_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

typedef struct {
  u32  str_id;
  u32  param;
} _infoMenu;

//  ���j���[���X�g
//#define WIFI_PARENTINFO_MENULIST_MAX    ( 10 )
#define WIFI_PARENTINFO_MENULIST_COMMON   ( 4 ) // �ϓ����Ȃ������̐�

_infoMenu _parentInfoMenuList[  ] = {
  { msg_wifilobby_027, (u32)WIFI_GAME_VCT }, //VCT
  { msg_wifilobby_076, (u32)WIFI_GAME_TVT }, //TVT
  { msg_wifilobby_022, (u32)WIFI_GAME_BATTLE_SINGLE_ALL }, //�ΐ�
  { msg_wifilobby_025, (u32)WIFI_GAME_TRADE }, //CHANGE
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL }
};


_infoMenu _parentSingleInfoMenuList[] = {
  { msg_wifilobby_0571, (u32)0 }, //�V���O��
  { msg_wifilobby_0572, (u32)1 },   //�_�u��
  { msg_wifilobby_0573, (u32)2 },  //�g���v��
  { msg_wifilobby_0574, (u32)3 },  //���[�e�[�V����
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};



_infoMenu _parentCustomInfoMenuList[] = {
  { msg_wifilobby_0571, (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE }, //�o�g��
  { msg_wifilobby_060,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE },   //���[��
  { msg_wifilobby_062,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER },  //�V���[�^�[
  { msg_wifilobby_096,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE },  //����
  { msg_wifilobby_097,  (u32)WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH },  //���[��������
  { msg_wifilobby_032,  (u32)BMPMENULIST_CANCEL },
};


#define _NORIMIT (0)
#define _FLAT (1)

_infoMenu _parentRuleInfoMenuList[] = {
  { msg_wifilobby_060, (u32)_NORIMIT },  //��������Ȃ�
  { msg_wifilobby_061, (u32)_FLAT },  //�t���b�g���[��
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};


#define _SHOOTER_ON (0)
#define _SHOOTER_OFF (1)

_infoMenu _parentShooterInfoMenuList[] = {
  { msg_wifilobby_062, (u32)_SHOOTER_ON },  //�V���[�^�[����
  { msg_wifilobby_063, (u32)_SHOOTER_OFF },  //�V���[�^�[�Ȃ�
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

#define _TEMOTI (0)
#define _BATTLEBOX (1)


_infoMenu _parentPokePartyMenuList[] = {
  { msg_wifilobby_098, (u32)_TEMOTI },  //�Ă���
  { msg_wifilobby_099, (u32)_BATTLEBOX },  //�o�g���{�b�N�X
};

static void	_print_callback(BMPMENULIST_WORK * wk,u32 param,u8 y);


///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _parentInfoMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  NULL,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  _print_callback,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  elementof(_parentInfoMenuList),// ���X�g���ڐ�
  elementof(_parentInfoMenuList),// �\���ő區�ڐ�
  0,            // ���x���\���w���W
  16,            // ���ڕ\���w���W
  0,            // �J�[�\���\���w���W
  0,            // �\���x���W
  FBMP_COL_BLACK,     // �����F
  FBMP_COL_WHITE,     // �w�i�F
  FBMP_COL_BLK_SDW,   // �����e�F
  0,            // �����Ԋu�w
  16,           // �����Ԋu�x
  BMPMENULIST_NO_SKIP,    // �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,        // �����w��
  0,            // �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,      //�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,      //�����T�C�YY(�h�b�g
  NULL,   //�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,   //�\���Ɏg�p����v�����g���[�e�B
  NULL,   //�\���Ɏg�p����v�����g�L���[
  NULL,   //�\���Ɏg�p����t�H���g�n���h��
};

///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _parentInfoBattleMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  NULL,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  NELEMS(_parentSingleInfoMenuList),  // ���X�g���ڐ�
  NELEMS(_parentSingleInfoMenuList),  // �\���ő區�ڐ�
  0,            // ���x���\���w���W
  16,            // ���ڕ\���w���W
  0,            // �J�[�\���\���w���W
  0,            // �\���x���W
  FBMP_COL_BLACK,     // �����F
  FBMP_COL_WHITE,     // �w�i�F
  FBMP_COL_BLK_SDW,   // �����e�F
  0,            // �����Ԋu�w
  16,           // �����Ԋu�x
  BMPMENULIST_NO_SKIP,    // �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,        // �����w��
  0,            // �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,      //�����T�C�YY(�h�b�g
  NULL,   //�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,   //�\���Ɏg�p����v�����g���[�e�B
  NULL,   //�\���Ɏg�p����v�����g�L���[
  NULL,   //�\���Ɏg�p����t�H���g�n���h��
};


///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _parentCustomMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  NULL,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  NELEMS(_parentCustomInfoMenuList),  // ���X�g���ڐ�
  NELEMS(_parentCustomInfoMenuList),  // �\���ő區�ڐ�
  0,            // ���x���\���w���W
  16,            // ���ڕ\���w���W
  0,            // �J�[�\���\���w���W
  0,            // �\���x���W
  FBMP_COL_BLACK,     // �����F
  FBMP_COL_WHITE,     // �w�i�F
  FBMP_COL_BLK_SDW,   // �����e�F
  0,            // �����Ԋu�w
  16,           // �����Ԋu�x
  BMPMENULIST_NO_SKIP,    // �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,        // �����w��
  0,            // �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,      //�����T�C�YY(�h�b�g
  NULL,   //�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,   //�\���Ɏg�p����v�����g���[�e�B
  NULL,   //�\���Ɏg�p����v�����g�L���[
  NULL,   //�\���Ɏg�p����t�H���g�n���h��
};



//------------------------------------------------------------------
/**
 * $brief   ��s�R�[���o�b�N
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
  else{
    BmpMenuList_ColorControl(wk,FBMP_COL_BLACK,FBMP_COL_WHITE,FBMP_COL_BLK_SDW);
  }
}




static void _modeSelectMenuBase( WIFIP2PMATCH_WORK *wk,BMPMENULIST_HEADER* plisth, _infoMenu* p_menu,int length, int menutype, int width )
{
  int i,y;

  PRINTSYS_QUE_Clear(wk->SysMenuQue);

  GFL_FONTSYS_SetDefaultColor();

  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, p_menu[i].str_id, p_menu[i].param, HEAPID_WIFIP2PMATCH );
  }

  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMP�E�B���h�E����
  if(menutype!=_MENUTYPE_POKEPARTY){
    y = PARENTMENU_Y;
  }
  else{
    y = PARENTMENU2_Y;
  }
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 32-width-1, y, width, length * 2, MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_OFF, GFL_ARCUTIL_TRANSINFO_GetPos(wk->talkwin_m2), COMM_MESFRAME_PAL );


  plisth->list = wk->submenulist;
  plisth->win = wk->SubListWin;

  plisth->print_que = wk->SysMenuQue;
  plisth->print_util = &wk->SysMsgPrintUtil;
  plisth->font_handle = wk->fontHandle;
  plisth->work = wk;

  wk->sublw = BmpMenuList_Set(plisth, 0, wk->singleCur[menutype], HEAPID_WIFIP2PMATCH);
  while(!PRINTSYS_QUE_IsFinished(wk->SysMenuQue)){
    PRINTSYS_QUE_Main(wk->SysMenuQue);
  }
  BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_M);
}




static void _ParentModeSelectMenu( WIFIP2PMATCH_WORK *wk )
{
  _modeSelectMenuBase(wk, &_parentInfoMenuListHeader,
                      _parentInfoMenuList, elementof(_parentInfoMenuList), _MENUTYPE_GAME, 17);
}

//------------------------------------------------------------------
/**
 * $brief   �}�b�`���O�\�����݃��j���[
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

///�I�����j���[�̃��X�g
static const BMPMENULIST_HEADER _childMatchMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  NULL,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  NELEMS(_childMatchMenuList),  // ���X�g���ڐ�
  NELEMS(_childMatchMenuList),  // �\���ő區�ڐ�
  0,            // ���x���\���w���W
  16,            // ���ڕ\���w���W
  0,            // �J�[�\���\���w���W
  0,            // �\���x���W
  FBMP_COL_BLACK,     // �����F
  FBMP_COL_WHITE,     // �w�i�F
  FBMP_COL_BLK_SDW,   // �����e�F
  0,            // �����Ԋu�w
  16,           // �����Ԋu�x
  BMPMENULIST_NO_SKIP,    // �y�[�W�X�L�b�v�^�C�v
  0,//FONT_SYSTEM,        // �����w��
  0,            // �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
  NULL,                    // work
  _BMPMENULIST_FONTSIZE,      //�����T�C�YX(�h�b�g
  _BMPMENULIST_FONTSIZE,      //�����T�C�YY(�h�b�g
  NULL,   //�\���Ɏg�p���郁�b�Z�[�W�o�b�t
  NULL,   //�\���Ɏg�p����v�����g���[�e�B
  NULL,   //�\���Ɏg�p����v�����g�L���[
  NULL,   //�\���Ɏg�p����t�H���g�n���h��
};


static void _ChildModeMatchMenuDisp( WIFIP2PMATCH_WORK *wk )
{
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


  vchat += WIFI_STATUS_GetVChatStatus( wk->pMatch );

  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  i = 0;

  for(i = 0; i < NELEMS(_childMatchMenuList);i++){
    if(i == 0 && (WIFI_STATUS_RECRUIT != status && status != WIFI_STATUS_PLAY_AND_RECRUIT && status != WIFI_STATUS_WAIT)){  // ���������ނ��o���Ȃ�����
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
  //BMP�E�B���h�E����
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 16, 11+ ((3-length)*2), 15  , length * 2,
    MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_OFF, GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );
  list_h.list = wk->submenulist;
  list_h.win = wk->SubListWin;

  list_h.print_que = wk->SysMenuQue;
  list_h.print_util = &wk->SysMsgPrintUtil;
  list_h.font_handle = wk->fontHandle;

  wk->sublw = BmpMenuList_Set(&list_h, 0, 0, HEAPID_WIFIP2PMATCH);

  while(!PRINTSYS_QUE_IsFinished(wk->SysMenuQue)){
    PRINTSYS_QUE_Main(wk->SysMenuQue);
  }

  BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );
  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_M);
}

//------------------------------------------------------------------
/**
 * $brief   �o�g���̏ڍו��������߂� WIFIP2PMATCH_MODE_SUBBATTLE_INIT
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
 * $brief   �����̏�Ԃ�\��
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
    // ������
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
  // msg_id�ƃJ���[���擾
  msg_id = MCVSys_StatusMsgIdGet( status, gamemode, &col );
  if( gamemode == WIFI_GAME_VCT && status == WIFI_STATUS_PLAYING ){      // VCT��
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

  //��ԃA�C�R���ݒ�
  // Frame1�̑Ή�����ʒu�ɃA�C�R����]��
  WifiP2PMatchFriendListStIconWrite( &wk->icon, GFL_BG_FRAME1_M,
                                     PLAYER_DISP_ICON_POS_X, PLAYER_DISP_ICON_POS_Y,
                                     status,gamemode, PLAYER_DISP_ICON_MYMODE);
  if( WIFI_STATUS_GetVChatStatus(wk->pMatch) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT;
  }

  WifiP2PMatchFriendListIconWrite(  &wk->icon, GFL_BG_FRAME1_M,
                                    PLAYER_DISP_VCTICON_POS_X, PLAYER_DISP_VCTICON_POS_Y,
                                    vct_icon+PLAYER_DISP_ICON_MYMODE, 0 );

  GFL_BMPWIN_ClearScreen(wk->MyInfoWin);
  GFL_BMPWIN_MakeScreen(wk->MyInfoWin);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME3_M);
}


//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
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
                                        MCV_SYSFONT_PAL,  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( wk->MyInfoWinBack );
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_018, wk->TitleString );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MyInfoWinBack), 0, 0, wk->TitleString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MyInfoWinBack);
}


//------------------------------------------------------------------
/**
 * $brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BmpWinDelete( WIFIP2PMATCH_WORK *wk )
{
  int i;

  // ���b�Z�[�W��~
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
  if(wk->pYesNoWork){
    BmpMenu_YesNoMenuExit( wk->pYesNoWork );
    wk->pYesNoWork = NULL;
  }
}


//------------------------------------------------------------------
/**
 * @brief   Message���[�N������
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
  wk->pTemp = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );        // ���͓o�^���̈ꎞ�o�b�t�@
  wk->pExpStrBuf = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->TitleString = GFL_STR_CreateBuffer( TITLE_MESSAGE_BUF_NUM, HEAPID_WIFIP2PMATCH );
  wk->SysMsgQue = PRINTSYS_QUE_Create( HEAPID_WIFIP2PMATCH );
  wk->SysMenuQue = PRINTSYS_QUE_Create( HEAPID_WIFIP2PMATCH );
  wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_WIFIP2PMATCH , HEAPID_WIFIP2PMATCH , 2 , 0 );
  wk->pKeyCursor = APP_KEYCURSOR_Create( 15, TRUE, FALSE, HEAPID_WIFIP2PMATCH );


}


//------------------------------------------------------------------
/**
 * @brief    Message�p���[�N���
 *
 * @param   wk
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeMessageWork( WIFIP2PMATCH_WORK *wk )
{
  int i;

  _TouchResExit(wk);
  _TrainerOAMFree(wk);

  APP_KEYCURSOR_Delete( wk->pKeyCursor );

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
  PRINTSYS_QUE_Clear(wk->SysMenuQue);
  PRINTSYS_QUE_Delete(wk->SysMsgQue);
  PRINTSYS_QUE_Delete(wk->SysMenuQue);
  wk->SysMsgQue=NULL;
}





//----------------------------------------------------------------------------
/**
 *  @brief  ���[�U�[�f�[�^�\��
 *
 *  @param  wk  �V�X�e�����[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDraw( WIFIP2PMATCH_WORK *wk, u32 heapID )
{
  int sex;
  int pal;

  sex = WifiList_GetFriendInfo( wk->pList,
                                wk->view.touch_friendNo - 1, WIFILIST_FRIEND_SEX );

  //  if(sex == PM_FEMALE){
  //    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_GIRL;
  //  }else{
  //    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_MAN;
  //  }
  GFL_BG_LoadScreenBufferOfs( GFL_BG_FRAME2_S, wk->view.p_userscrn[0]->rawData,
                              wk->view.p_userscrn[0]->szByte,0 );
  //  GFL_BG_ScreenBufSet( GFL_BG_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
  //      wk->view.p_userscrn[wk->view.user_dispno]->szByte );

  //  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, 0, 0,
  //                            32, 24, pal );

  // BG�R�ʂ̃X�N���[�����N���A
  GFL_BG_ClearFrame( GFL_BG_FRAME3_S);

  // ���̐l�̂��Ƃ�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0x0 );

  // �`��
  MCVSys_UserDispDrawType00( wk, heapID );

  //  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );

  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);

  // ���b�Z�[�W�ʂ̕\���ݒ�
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}




//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR������
 *  @param  POKEMON_TRADE_WORK
 *  @return none
 */
//-----------------------------------------------------------------------------


//�g���[�i�[�O���t�B�b�N
static void _TrainerOAMDraw( WIFIP2PMATCH_WORK *wk, int friendNo, u32 heapID )
{
  ARCHANDLE * ah;
  u32 num;

  GFL_CLWK_DATA clearadd = {
    56,80,
    0,
    0,0,
  };
  if(wk->pTrgra){
    _TrainerOAMFree(wk);
  }
  
  num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_UNION_GRA );
  num = UnionView_GetTrType( num );

  ah = TR2DGRA_OpenHandle( heapID );
  wk->trRes[0] = TR2DGRA_OBJ_CGR_Register( ah, num, CLSYS_DRAW_SUB, heapID );
  wk->trRes[1] = TR2DGRA_OBJ_PLTT_Register( ah, num, CLSYS_DRAW_SUB, CLACT_PALNUM_TRGRA*0x20, heapID );
  wk->trRes[2] = TR2DGRA_OBJ_CELLANM_Register( num, APP_COMMON_MAPPING_128K, CLSYS_DRAW_SUB, heapID );
  GFL_ARC_CloseDataHandle( ah );

  wk->pTrgra = GFL_CLACT_WK_Create(
    wk->clactSet,
    wk->trRes[0],
    wk->trRes[1],
    wk->trRes[2],
    &clearadd, CLSYS_DEFREND_SUB, heapID );

}


static void _TrainerOAMFree( WIFIP2PMATCH_WORK *wk )
{
  if(wk->pTrgra){
    GFL_CLACT_WK_Remove(wk->pTrgra);
    GFL_CLGRP_CGR_Release(wk->trRes[0]);
    GFL_CLGRP_PLTT_Release(wk->trRes[1]);
    GFL_CLGRP_CELLANIM_Release(wk->trRes[2]);
    wk->pTrgra = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  TOUCHBAR������
 *  @param  POKEMON_TRADE_WORK
 *  @return none
 */
//-----------------------------------------------------------------------------

static void _TOUCHBAR_Init(WIFIP2PMATCH_WORK *wk,HEAPID heapID)
{
  //�A�C�R���̐ݒ�
  //�������
  TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]  =
  {
    {
      TOUCHBAR_ICON_RETURN,
      { TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
    },
  };

  TOUCHBAR_SETUP  touchbar_setup;

  if(wk->pTouchWork){
    return;
  }
  
  GFL_STD_MemClear( &touchbar_setup, sizeof(TOUCHBAR_SETUP) );

  touchbar_setup.p_item   = touchbar_icon_tbl;        //��̑����
  touchbar_setup.item_num = NELEMS(touchbar_icon_tbl);//�����������邩
  touchbar_setup.p_unit   = wk->clactSet;                    //OBJ�ǂݍ��݂̂��߂�CLUNIT
  touchbar_setup.is_notload_bg = FALSE;  //BG�͂Ȃ�
  touchbar_setup.bar_frm  = GFL_BG_FRAME0_S;            //BG�ǂݍ��݂̂��߂�BG�ʏ㉺��ʔ���ɂ��K�v
  touchbar_setup.bg_plt   = _TOUCHBAR_BG_PALPOS;      //BG��گ�
  touchbar_setup.obj_plt  = _TOUCHBAR_OBJ_PALPOS;      //OBJ��گ�
  touchbar_setup.mapping  = APP_COMMON_MAPPING_128K;  //�}�b�s���O���[�h

  wk->pTouchWork = TOUCHBAR_Init(&touchbar_setup, heapID);

}


static void _TouchResExit( WIFIP2PMATCH_WORK *wk )
{
  if(wk->pTouchWork){
    TOUCHBAR_Exit(wk->pTouchWork);
    wk->pTouchWork=NULL;
  }
}

// PAGE 1
#define MCV_USERD_NAME_X  ( 32-8 )
#define MCV_USERD_NAME_Y  ( 8-8 )
#define MCV_USERD_ST_X  ( 104-8 )
#define MCV_USERD_ST_Y  ( 8-8 )
#define MCV_USERD_GR_X  ( 8-8 )
#define MCV_USERD_GR_Y  ( 32 )
#define MCV_USERD_VS_X  ( 88+16 )
#define MCV_USERD_VS_Y  ( 56-24 )
#define MCV_USERD_VS_WIN_X  ( 192-8-8 )
#define MCV_USERD_VS_WIN_Y  ( 56 )
#define MCV_USERD_VS_LOS_X  ( 192-8-8 )
#define MCV_USERD_VS_LOS_Y  ( 56+16 )
#define MCV_USERD_TR_X    ( 88+16 )
#define MCV_USERD_TR_Y    ( 144-16*3 )
#define MCV_USERD_TRNUM_X ( 180+8 )
#define MCV_USERD_DAY_X   ( 8 )
#define MCV_USERD_DAY_Y   ( 128 )
#define MCV_USERD_DAYNUM_X  ( 152 )
#define MCV_USERD_ICON_X  ( 2 )
#define MCV_USERD_VCTICON_X ( 28 )
#define MCV_USERD_ICON_Y  ( 1 )


// �ʏ�
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

  // �g���[�i�[��
  // if( sex == PM_MALE ){
  //   _COL_N_BLUE;
  // }else{
  //   _COL_N_RED;
  // }
  _COL_N_WHITE;
  MCVSys_FriendNameSet(wk, friendNo);
  GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_033, wk->pExpStrBuf );
  WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_NAME_X, MCV_USERD_NAME_Y,
                  wk->TitleString, wk->fontHandle);

  p_status = WifiFriendMatchStatusGet( friendNo );
  status = _WifiMyStatusGet( wk, p_status );
  gamemode = _WifiMyGameModeGet( wk, p_status );

  // ���
  msg_id = MCVSys_StatusMsgIdGet( status, gamemode, &col );
  if((msg_id >= msg_wifilobby_1034) && (msg_wifilobby_1037 >= msg_id)){
    msg_id = msg_id - msg_wifilobby_1034 + msg_wifilobby_0571;
  }
  GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_ST_X, MCV_USERD_ST_Y,
                  wk->pExpStrBuf, wk->fontHandle,MSG_WHITE_COLOR);


  // �ΐ퐬��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_X, MCV_USERD_VS_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);
    // ����
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_WIN_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);
    // �܂�
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_LOS_X, MCV_USERD_VS_LOS_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);
  }
  // �|�P��������
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_038, wk->TitleString );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TR_X,  MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_TRADE_NUM);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_039, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );

    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_TRNUM_X, MCV_USERD_TR_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);
  }


  // �Ō�ɗV�񂾓��t
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_040, wk->TitleString );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAY_X, MCV_USERD_DAY_Y,
                    wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);

    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_DAY);
    if(num!=0){
      WORDSET_RegisterNumber(wk->view.p_wordset, 2, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_YEAR);
      WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_LASTBT_MONTH);
      WORDSET_RegisterNumber(wk->view.p_wordset, 1, num, 2, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
      GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_041, wk->pExpStrBuf );
      WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
      PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_DAYNUM_X, MCV_USERD_DAY_Y,
                      wk->TitleString, wk->fontHandle,MSG_WHITE_COLOR);
    }
  }
  GFL_BMPWIN_MakeScreen(wk->view.userWin);
  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_S);

  // �A�C�R��
  WifiP2PMatchFriendListStIconWrite(  &wk->icon, GFL_BG_FRAME2_S,
                                      MCV_USERD_ICON_X, MCV_USERD_ICON_Y,
                                      status, gamemode , PLAYER_DISP_ICON_CARDMODE);
  if( WIFI_STATUS_GetVChatStatus(p_status) ){
    vct_icon = PLAYER_DISP_ICON_IDX_NONE + PLAYER_DISP_ICON_CARDMODE;
  }else{
    vct_icon = PLAYER_DISP_ICON_IDX_VCTNOT + PLAYER_DISP_ICON_CARDMODE;
  }
  WifiP2PMatchFriendListIconWrite( &wk->icon, GFL_BG_FRAME2_S,
                                   MCV_USERD_VCTICON_X, MCV_USERD_ICON_Y,
                                   vct_icon, 0 );

  _TrainerOAMDraw( wk, friendNo,  heapID );

  _TOUCHBAR_Init(wk,heapID);

  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

}


//----------------------------------------------------------------------------
/**
 *  @brief  ��ԂɑΉ��������b�Z�[�W�f�[�^�\��
 *
 *  @param  status  ���
 *
 *  @return ���b�Z�[�W�f�[�^
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

  {//WIFI_GAME_e�Q��
    u32 message[]={msg_wifilobby_046,msg_wifilobby_046,msg_wifilobby_046,msg_wifilobby_1014,msg_wifilobby_027, msg_wifilobby_076,
      msg_wifilobby_025, msg_wifilobby_1034,msg_wifilobby_1034,msg_wifilobby_1035,msg_wifilobby_1035,
      msg_wifilobby_1036,msg_wifilobby_1036,msg_wifilobby_1037,msg_wifilobby_1037,msg_wifilobby_056};

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
 *  @brief  �o�g���̃��[�h�ݒ�
 *  @param  status  ���
 *  @return ���b�Z�[�W�f�[�^
 */
//-----------------------------------------------------------------------------

static void _battleCustomSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentCustomInfoMenuList[0].str_id = msg_wifilobby_1023 + wk->battleMode;
  _parentCustomInfoMenuList[1].str_id = msg_wifilobby_1028 + wk->battleRule;
  _parentCustomInfoMenuList[2].str_id = msg_wifilobby_1030 + (1-wk->battleShooter);

  _modeSelectMenuBase(wk, &_parentCustomMenuListHeader, _parentCustomInfoMenuList,
                      elementof(_parentCustomInfoMenuList), _MENUTYPE_BATTLE_CUSTOM, 30);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���̃��[�h�ݒ�
 *  @param  status  ���
 *  @return ���b�Z�[�W�f�[�^
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
 *  @brief  �o�g���̃��[���ݒ�
 *  @param  status  ���
 *  @return ���b�Z�[�W�f�[�^
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
 *  @brief  �o�g���̃V���[�^�[�ݒ�
 *  @param  status  ���
 *  @return ���b�Z�[�W�f�[�^
 */
//-----------------------------------------------------------------------------

static void _battleShooterSelectMenu( WIFIP2PMATCH_WORK *wk )
{

  _parentInfoBattleMenuListHeader.count = elementof(_parentShooterInfoMenuList);
  _parentInfoBattleMenuListHeader.line = elementof(_parentShooterInfoMenuList);


  _modeSelectMenuBase(wk, &_parentInfoBattleMenuListHeader, _parentShooterInfoMenuList,
                      elementof(_parentShooterInfoMenuList), _MENUTYPE_BATTLE_SHOOTER,20);
}




#if 1
//--------------------------------------------------------------
/**
 * �w��POKEPARTY�̃X�e�[�^�X������������
 *
 * @param   psl		
 * @param   party		
 */
//--------------------------------------------------------------
static void _Print_DrawPokeStatusBmpWin(WIFIP2PMATCH_WORK *wk, const POKEPARTY *party)
{
  STRBUF *buf_lv, *buf_sex, *tempbuf;
  int i, party_max;
  u32 monsno, level, sex, nidoran_nickname;
  POKEMON_PARAM *pp;
  
//  _Print_AddPokeStatusBmpWin(psl);
//  FLDMSGWIN_ClearWindow(psl->fldmsgwin_poke);

  //BMP�E�B���h�E����
  if(!wk->ListWin){
    wk->ListWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME2_M, 1, 5,  30, 6,
      MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  }
  BmpWinFrame_Write( wk->ListWin, WINDOW_TRANS_OFF,
                     GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFRAME_PAL );

  buf_lv = GFL_STR_CreateBuffer(64, HEAPID_WIFIP2PMATCH);
  tempbuf = GFL_STR_CreateBuffer(64, HEAPID_WIFIP2PMATCH);
  
  //���x�������ʕ`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->ListWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->ListWin);

  party_max = PokeParty_GetPokeCount(party);
  for(i = 0; i < party_max; i++){
    OS_Printf("-x-x-x  %d\n",i);
    pp = PokeParty_GetMemberPointer(party, i);
    monsno = PP_Get( pp, ID_PARA_monsno, NULL);
    nidoran_nickname = PP_Get(pp, ID_PARA_nidoran_nickname, NULL);
    level = PP_Get( pp, ID_PARA_level, NULL);
    sex = PP_Get( pp, ID_PARA_sex, NULL);

    //���x��
    GFL_MSG_GetString( wk->MsgManager, msg_wifilobby_1039, tempbuf );
    WORDSET_RegisterNumber( wk->WordSet, 0, level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    WORDSET_ExpandStr( wk->WordSet, buf_lv, tempbuf );

    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->ListWin), 1+SEX_POS_SPACE_X*i, 32,
                    buf_lv, wk->font_handle_num);
    
    //����
    if(sex == PM_NEUTRAL 
        || ((monsno==MONSNO_NIDORAN_F || monsno==MONSNO_NIDORAN_M) && nidoran_nickname==FALSE)){
      ; //�����`�悵�Ȃ�
    }
    else if(sex == PM_MALE){
      GFL_MSG_GetString( wk->MsgManager, msg_wifilobby_1040, tempbuf );
      PRINTSYS_Print(GFL_BMPWIN_GetBmp(wk->ListWin), SEX_POS_X+SEX_POS_SPACE_X*i, SEX_POS_Y, tempbuf, wk->fontHandle);
    }
    else{
      GFL_MSG_GetString( wk->MsgManager, msg_wifilobby_1041, tempbuf );
      PRINTSYS_Print(GFL_BMPWIN_GetBmp(wk->ListWin), SEX_POS_X+SEX_POS_SPACE_X*i, SEX_POS_Y, tempbuf, wk->fontHandle);
    }
  }
  
  GFL_STR_DeleteBuffer(buf_lv);
  GFL_STR_DeleteBuffer(tempbuf);

  GFL_BMPWIN_MakeScreen( wk->ListWin );
  GFL_BMPWIN_TransVramCharacter(wk->ListWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_M);
}


static void _DeletePokeStatus(WIFIP2PMATCH_WORK *wk)
{
  if(wk->ListWin){
    GFL_BMPWIN_ClearScreen(wk->ListWin);
    BmpWinFrame_Clear(wk->ListWin,WINDOW_TRANS_ON_V);
    GFL_BMPWIN_Delete(wk->ListWin);
    wk->ListWin=NULL;
  }
}

#endif

#define _POKEMON_SELECT1_CELLX (22)
#define _POKEMON_SELECT1_CELLY (54)

static void _pokemonIconCreate(WIFIP2PMATCH_WORK *wk, const POKEPARTY *party,int offset,ARCHANDLE *arcHandlePoke)
{
  int i, party_max;

  party_max = PokeParty_GetPokeCount(party);
  for(i = 0; i < party_max; i++){
    GFL_CLWK_DATA cellInitData;
    const POKEMON_PASO_PARAM* ppp = PP_GetPPPPointerConst(PokeParty_GetMemberPointer(party, i));

    cellInitData.pos_x = _POKEMON_SELECT1_CELLX + i*POKEICON_POS_SPACE_X;
    cellInitData.pos_y = _POKEMON_SELECT1_CELLY;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;

    wk->cellRes[i + offset] = GFL_CLGRP_CGR_Register( arcHandlePoke ,
                                                      POKEICON_GetCgxArcIndex(ppp),
                                                      FALSE , CLSYS_DRAW_MAIN , HEAPID_WIFIP2PMATCH );

    wk->pokemonIcon[i + offset] = GFL_CLACT_WK_Create( wk->clactSet,
                                             wk->cellRes[i + offset],
                                             wk->cellResPal,
                                             wk->cellResAnm,
                                             &cellInitData ,CLSYS_DRAW_MAIN , HEAPID_WIFIP2PMATCH );

    GFL_CLACT_WK_SetPlttOffs( wk->pokemonIcon[i + offset], POKEICON_GetPalNumGetByPPP(ppp), CLWK_PLTTOFFS_MODE_PLTT_TOP );

  }
}


static void _pokeIconResourceCreate(WIFIP2PMATCH_WORK *wk, const POKEPARTY *party, const POKEPARTY *battle)
{
  ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON ,HEAPID_WIFIP2PMATCH );
  wk->cellResPal =
    GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke ,
                                 POKEICON_GetPalArcIndex() , CLSYS_DRAW_MAIN ,
                                 _OBJPLT_POKEICON_OFFSET,
                                 HEAPID_WIFIP2PMATCH );

  wk->cellResAnm =
    GFL_CLGRP_CELLANIM_Register( arcHandlePoke ,
                                 POKEICON_GetCellSubArcIndex() , POKEICON_GetAnmArcIndex(), HEAPID_WIFIP2PMATCH  );

  _pokemonIconCreate(wk, party, 0,arcHandlePoke);
  _pokemonIconCreate(wk, battle, 6,arcHandlePoke);

  GFL_ARC_CloseDataHandle(arcHandlePoke);
}


static void _pokeIconResourceDelete(WIFIP2PMATCH_WORK *wk)
{
  int i;

  if(wk->pokemonIcon[0]==NULL){
    return;
  }
  for(i=0;i<12;i++){
    if(wk->pokemonIcon[i]){
      GFL_CLACT_WK_Remove(wk->pokemonIcon[i]);
      GFL_CLGRP_CGR_Release(wk->cellRes[i]);
    }
    wk->pokemonIcon[i] = NULL;
  }
  GFL_CLGRP_PLTT_Release(wk->cellResPal);
  GFL_CLGRP_CELLANIM_Release(wk->cellResAnm);
}


static void _pokeIconDispSwitch(WIFIP2PMATCH_WORK *wk,BOOL bTemochi)
{
  int i,j , party_max;

  for(i=0;i<6;i++){
    if(wk->pokemonIcon[i]){
      if(bTemochi){
        GFL_CLACT_WK_SetAutoAnmFlag( wk->pokemonIcon[i] , TRUE );
        GFL_CLACT_WK_SetDrawEnable( wk->pokemonIcon[i], TRUE );
      }
      else{
        GFL_CLACT_WK_SetAutoAnmFlag( wk->pokemonIcon[i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( wk->pokemonIcon[i], FALSE );
      }
    }
  }
  for(i=6;i<12;i++){
    if(wk->pokemonIcon[i]){
      if(bTemochi){
        GFL_CLACT_WK_SetAutoAnmFlag( wk->pokemonIcon[i] , FALSE );
        GFL_CLACT_WK_SetDrawEnable( wk->pokemonIcon[i], FALSE );
      }
      else{
        GFL_CLACT_WK_SetAutoAnmFlag( wk->pokemonIcon[i] , TRUE );
        GFL_CLACT_WK_SetDrawEnable( wk->pokemonIcon[i], TRUE );
      }
    }
  }
  
}

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�p�[�e�B�[�I��
 *  @param  status  ���
 *  @return ���b�Z�[�W�f�[�^
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
 * ���M�����[�V�����F�\��
 *
 * @param   unisys
 * @param   fieldWork
 * @param   fail_bit    ���M�����[�V����NGbit
 * @param   shooter_type  TRUE:�V���[�^�[�L�@FALSE:�V���[�^�[����
 * @param   regwin_type    NGREG_TYPE_???
 */
//==================================================================


static void _Menu_RegulationSetup(WIFIP2PMATCH_WORK* wk, u32 fail_bit, BOOL shooter_type, REGWIN_TYPE regwin_type)
{
  int category;
  PRINTSYS_LSB color;
  u16 title_msgid;
  int space = 4;

  if(shooter_type == TRUE){
    shooter_type = REGULATION_SHOOTER_VALID;
  }
  else{
    shooter_type = REGULATION_SHOOTER_INVALID;
  }


  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);

  if(REGWIN_TYPE_RULE_SHORT == regwin_type){
    wk->SysMsgWin= GFL_BMPWIN_Create(  GFL_BG_FRAME2_M,
                                       REGWINDOW_WIN_PX, REGWINDOW_WIN_PY,
                                       REGWINDOW_WIN_SX, REGWINDOW_WIN_SY-4,
                                       MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  }
  else{
    wk->SysMsgWin= GFL_BMPWIN_Create(  GFL_BG_FRAME2_M,
                                       REGWINDOW_WIN_PX, REGWINDOW_WIN_PY,
                                       REGWINDOW_WIN_SX, REGWINDOW_WIN_SY,
                                       MCV_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  }
  //  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
  //                             0x20*MCV_SYSFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON_V,
                     GFL_ARCUTIL_TRANSINFO_GetPos(wk->menuwin_m2), COMM_MESFONT_PAL );

  GFL_FONTSYS_SetDefaultColor();

  GF_ASSERT(!wk->rpm);
  wk->rpm = PokeRegulation_CreatePrintMsg( wk->pRegulation, wk->WordSet, HEAPID_WIFIP2PMATCH, shooter_type);

  if(regwin_type == REGWIN_TYPE_RULE){
    STRBUF* cupname = Regulation_CreateCupName(wk->pRegulation, HEAPID_WIFIP2PMATCH);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 0, 0, cupname, wk->fontHandle );
    GFL_STR_DeleteBuffer(cupname);
  }
  else if(regwin_type != REGWIN_TYPE_RULE_SHORT){
    if(regwin_type == REGWIN_TYPE_NG_TEMOTI){
      title_msgid = msg_wifilobby_098;
    }
    else{
      title_msgid = msg_wifilobby_099;
    }
    GFL_MSG_GetString(  wk->MsgManager, title_msgid, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->SysMsgWin), 0, 0, wk->pExpStrBuf , wk->fontHandle);
  }
  if(regwin_type == REGWIN_TYPE_RULE_SHORT){
    space = 0;
  }
  for(category = 0; category < REGULATION_CATEGORY_MAX; category++){
    if(fail_bit & (1 << category)){
      color = REG_FAIL_COLOR;
    }
    else{
      color = REG_NORMAL_COLOR;
    }
    PRINTSYS_PrintColor(GFL_BMPWIN_GetBmp(wk->SysMsgWin),0, (space + 2*category)*8,
                        wk->rpm->category[category], wk->fontHandle, color);
    PRINTSYS_PrintColor(GFL_BMPWIN_GetBmp(wk->SysMsgWin), 14*8, (space + 2*category)*8,
                        wk->rpm->prerequisite[category], wk->fontHandle, color);
  }
  GFL_BMPWIN_TransVramCharacter(wk->SysMsgWin);
}


static void _Menu_RegulationDelete(WIFIP2PMATCH_WORK* wk)
{
  PokeRegulation_DeletePrintMsg(wk->rpm);
  wk->rpm = NULL;
  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�C���A�C�R�����o��
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


void WifiP2PMatchMessage_TimeIconStart(WIFIP2PMATCH_WORK* wk)
{
  if(wk->pTimeIcon){
    TIMEICON_Exit(wk->pTimeIcon);
    wk->pTimeIcon=NULL;
  }
  wk->pTimeIcon =
    TIMEICON_CreateTcbl(wk->pMsgTcblSys, wk->MsgWin,
                        15, TIMEICON_DEFAULT_WAIT, HEAPID_WIFIP2PMATCH);
}

