//============================================================================================
/**
 * @file    wifi_p2pmatch_message.c
 * @bfief   WIFIP2P�}�b�`���O���  ���b�Z�[�W�����؂�o��
 * @author  k.ohno
 * @date    10.01.09
 */
//============================================================================================


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
  if(wk->timeWaitWork){
    TimeWaitIconTaskDel(wk->timeWaitWork);  // �^�C�}�[�~��
    wk->timeWaitWork = NULL;
    wk->MsgWin = _BmpWinDel(wk->MsgWin);

  }
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
  _timeWaitIconDel(wk);
  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
  }
  wk->MsgWin = _BmpWinDel(wk->MsgWin);
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
  // ������擾
  u8 speed = MSGSPEED_GetWait();

  // TimeWaitIcon�j��
  _timeWaitIconDel( wk );

  wk->SysMsgWin = _BmpWinDel(wk->SysMsgWin);
  wk->MsgWin = _BmpWinDel(wk->MsgWin);

  if(!PRINTSYS_QUE_IsFinished(wk->SysMsgQue)){
    PRINTSYS_QUE_Clear(wk->SysMsgQue);
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


  // ��b�E�C���h�E�g�`��
  //GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 15 );
  //    BmpTalkWinWrite(&wk->MsgWin, WINDOW_TRANS_OFF,COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  //GFL_BMPWIN_MakeFrameScreen(wk->MsgWin, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL);


  //------

  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_TALK_WIN_CGX_NUM, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->MsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(wk->MsgWin);
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);

  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );

  //------

  // ������`��J�n
  //  MsgPrintSkipFlagSet( MSG_SKIP_ON );   // ���b�Z�[�W�X�L�b�vON
  //  MsgPrintAutoFlagSet( MSG_AUTO_OFF );  // ���b�Z�[�W��������OFF
  //    wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString,
  //                                     0, 0, speed, NULL);

#if 1
  GFL_FONTSYS_SetDefaultColor();
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->MsgWin), 2, 2, wk->TalkString, wk->fontHandle);
  GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
#endif


  //    PRINT_UTIL_Print(&wk->SysMsgPrintUtil, wk->SysMsgQue,
  //                   0, 0,wk->TalkString,wk->fontHandle);

  //GFL_BMPWIN_TransVramCharacter(wk->MsgWin);
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

  BmpWinFrame_CgxSet(GFL_BG_FRAME2_M,COMM_SYS_WIN_CGX, MENU_TYPE_SYSTEM, HEAPID_WIFIP2PMATCH);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*COMM_MESFONT_PAL, 0x20, HEAPID_WIFIP2PMATCH);

  // �V�X�e���E�C���h�E�g�`��
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->SysMsgWin), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_ClearScreen(wk->SysMsgWin);
  GFL_BMPWIN_MakeScreen(wk->SysMsgWin);
  BmpWinFrame_Write( wk->SysMsgWin, WINDOW_TRANS_ON, COMM_SYS_WIN_CGX, COMM_MESFONT_PAL );

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
 * $brief   �J�[�\���R�[���o�b�N
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _curCallBack(BMPMENULIST_WORK * wk,u32 param,u8 mode)
{
  if(mode == 0){
    Snd_SePlay(_SE_CURSOR);
  }
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
#define WIFI_PARENTINFO_MENULIST_COMMON   ( 5 ) // �ϓ����Ȃ������̐�

_infoMenu _parentInfoMenuList[  ] = {
  { msg_wifilobby_027, (u32)WIFI_GAME_VCT }, //VCT
  { msg_wifilobby_076, (u32)WIFI_GAME_TVT }, //TVT
  { msg_wifilobby_022, (u32)WIFI_GAME_BATTLE_SINGLE_ALL }, //�ΐ�
  { msg_wifilobby_025, (u32)WIFI_GAME_TRADE }, //CHANGE
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL }
};


_infoMenu _parentSingleInfoMenuList[] = {
  { msg_wifilobby_0571, (u32)WIFI_GAME_BATTLE_SINGLE_ALL }, //�V���O��
  { msg_wifilobby_0572, (u32)WIFI_GAME_BATTLE_DOUBLE_ALL },   //�_�u��
  { msg_wifilobby_0573, (u32)WIFI_GAME_BATTLE_TRIPLE_ALL },  //�g���v��
  { msg_wifilobby_0574, (u32)WIFI_GAME_BATTLE_ROTATION_ALL },  //���[�e�[�V����
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

#define _NORIMIT (0)
#define _FLAT (1)

_infoMenu _parentDoubleInfoMenuList[] = {
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


///�I�����j���[�̃��X�g
static BMPMENULIST_HEADER _parentInfoMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  _curCallBack,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
  NULL,         // ���\�����Ƃ̃R�[���o�b�N�֐�
  NULL,                   //
  WIFI_PARENTINFO_MENULIST_COMMON,// ���X�g���ڐ�
  WIFI_PARENTINFO_MENULIST_COMMON,// �\���ő區�ڐ�
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
static const BMPMENULIST_HEADER _parentInfoBattleMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  _curCallBack,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
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


#define PARENTMENU_Y  ( 3 )
static void _ParentModeSelectMenu( WIFIP2PMATCH_WORK *wk )
{
  int i,length;
  BMPMENULIST_HEADER list_h;
  _infoMenu* p_menu;

  // ���X�g��{���ݒ�
  length = WIFI_PARENTINFO_MENULIST_COMMON;
  list_h = _parentInfoMenuListHeader;
  p_menu = _parentInfoMenuList;



  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, p_menu[i].str_id, p_menu[i].param, HEAPID_WIFIP2PMATCH );
  }

  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMP�E�B���h�E����
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 16, PARENTMENU_Y, 15, length * 2, FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  COMM_TALK_WIN_CGX_NUM, MENU_WIN_PAL );
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  list_h.list = wk->submenulist;
  list_h.win = wk->SubListWin;

  list_h.print_que = wk->SysMsgQue;
  PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
  list_h.print_util = &wk->SysMsgPrintUtil;
  list_h.font_handle = wk->fontHandle;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->SubListWin), WINCLR_COL(FBMP_COL_WHITE) );

  wk->sublw = BmpMenuList_Set(&list_h, 0, wk->battleCur, HEAPID_WIFIP2PMATCH);
	BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );

  GFL_BMPWIN_TransVramCharacter(wk->SubListWin);
  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);
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


static const struct {
  u32  str_id;
  u32  param;
}_childMatchMenuList[] = {
  { msg_wifilobby_030, (u32)_CONNECTING },
  //    { msg_wifilobby_031, (u32)_INFOVIEW },
  { msg_wifilobby_032, (u32)BMPMENULIST_CANCEL },
};

///�I�����j���[�̃��X�g
static const BMPMENULIST_HEADER _childMatchMenuListHeader = {
  NULL,     // �\�������f�[�^�|�C���^
  _curCallBack,         // �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
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
#if 0
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
    wk->keepVChat = vchat;
    vchat += WIFI_STATUS_GetVChatStatus( wk->pMatch );

    // ���̐l�̏���\��
    //  WifiP2PMatch_UserDispOn_MyAcces( wk, friendNo, HEAPID_WIFIP2PMATCH );


    wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
    i = 0;

    for(i = 0; i < NELEMS(_childMatchMenuList);i++){
      if(i == 0){
        if(WIFI_STATUS_PLAYING == status || status >= WIFI_STATUS_UNKNOWN){  // ���������ނ��o���Ȃ�����
          list_h.line -= 1;
          list_h.count -= 1;
          length -= 1;
        }
        else if(status == WIFI_STATUS_WAIT){
          if(vchat == 2){
            BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                              msg_wifilobby_027,
                                              _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
          }
          else{
            list_h.line -= 1;
            list_h.count -= 1;
            length -= 1;
          }
        }
        else{
          BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager,
                                            _childMatchMenuList[i].str_id,
                                            _childMatchMenuList[i].param,HEAPID_WIFIP2PMATCH );
        }
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
      FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
    //    GFL_BMPWIN_MakeFrameScreen(wk->SubListWin,  MENU_WIN_CGX_NUM, MENU_WIN_PAL );
    BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
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
 * $brief   �o�g���̏ڍו��������߂� WIFIP2PMATCH_MODE_SUBBATTLE_INIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _battleSubMenuInit( WIFIP2PMATCH_WORK *wk, int ret )
{
  int i,length;
  BMPMENULIST_HEADER list_h;
  _infoMenu* pMenu;

  switch(ret){
  case WIFI_GAME_BATTLE_SINGLE_ALL:
    pMenu = _parentSingleInfoMenuList;
    length = NELEMS(_parentSingleInfoMenuList);
    wk->bSingle = 1;
    list_h = _parentInfoBattleMenuListHeader;
    break;
/*
  case WIFI_STSET_DOUBLEBATTLE:
    pMenu = _parentDoubleInfoMenuList;
    length = NELEMS(_parentDoubleInfoMenuList);
    wk->bSingle = 0;
    list_h = _parentInfoBattleMenuListHeader;
    break;
*/
  }

  wk->submenulist = BmpMenuWork_ListCreate( length , HEAPID_WIFIP2PMATCH );
  for(i=0; i< length ; i++){
    BmpMenuWork_ListAddArchiveString( wk->submenulist, wk->MsgManager, pMenu[i].str_id, pMenu[i].param,HEAPID_WIFIP2PMATCH );
  }
  wk->SubListWin = _BmpWinDel(wk->SubListWin);
  //BMP�E�B���h�E����
  wk->SubListWin = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M, 16, 9, 15, length * 2, FLD_SYSFONT_PAL, GFL_BMP_CHRAREA_GET_B);
  BmpWinFrame_Write( wk->SubListWin, WINDOW_TRANS_ON, COMM_TALK_WIN_CGX_NUM, COMM_MESFRAME_PAL );
  list_h.list = wk->submenulist;
  list_h.win = wk->SubListWin;

  list_h.print_que = wk->SysMsgQue;
  PRINT_UTIL_Setup( &wk->SysMsgPrintUtil , wk->SubListWin );
  list_h.print_util = &wk->SysMsgPrintUtil;
  list_h.font_handle = wk->fontHandle;

  wk->sublw = BmpMenuList_Set(&list_h, 0, wk->singleCur[wk->bSingle], HEAPID_WIFIP2PMATCH);
	BmpMenuList_SetCursorBmp( wk->sublw, HEAPID_WIFIP2PMATCH );
  GFL_BMPWIN_TransVramCharacter(wk->SubListWin);

  GFL_BMPWIN_MakeScreen(wk->SubListWin);
  GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);

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
                                        _NUKI_FONT_PALNO,  GFL_BMP_CHRAREA_GET_B );
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


  GFL_STR_DeleteBuffer( wk->TitleString );
  GFL_STR_DeleteBuffer( wk->TalkString );
  GFL_STR_DeleteBuffer( wk->pTemp );

  GFL_STR_DeleteBuffer(wk->pExpStrBuf);
  PRINTSYS_QUE_Clear(wk->SysMsgQue);
  PRINTSYS_QUE_Delete(wk->SysMsgQue);

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

  if(sex == PM_FEMALE){
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_GIRL;
  }else{
    pal = MCV_PAL_BTTN+MCV_PAL_BTTNST_MAN;
  }
  GFL_BG_LoadScreen( GFL_BG_FRAME2_S, wk->view.p_userscrn[0]->rawData,
                     wk->view.p_userscrn[0]->szByte,0 );
  //  GFL_BG_ScreenBufSet( GFL_BG_FRAME2_S, wk->view.p_userscrn[wk->view.user_dispno]->rawData,
  //      wk->view.p_userscrn[wk->view.user_dispno]->szByte );

  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S, 0, 0,
                              32, 24, pal );

  // BG�R�ʂ̃X�N���[�����N���A
  GFL_BG_ClearFrame( GFL_BG_FRAME3_S);

  MCVSys_UserDispDrawFrontierOffScrn( wk ); // ��\���X�N���[���ݒ�


  // ���̐l�̂��Ƃ�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->view.userWin), 0x0 );

  // �`��
  MCVSys_UserDispDrawType00( wk, heapID );

  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
  GFL_BMPWIN_TransVramCharacter(wk->view.userWin);

  // ���b�Z�[�W�ʂ̕\���ݒ�
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
}

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

  friendNo = wk->view.touch_friendNo - 1;

  sex = WifiList_GetFriendInfo( wk->pList, friendNo, WIFILIST_FRIEND_SEX );

  // �g���[�i�[��
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

  // ���
  msg_id = MCVSys_StatusMsgIdGet( status, gamemode, &col );
  GFL_MSG_GetString(wk->MsgManager, msg_id, wk->pExpStrBuf);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_ST_X, MCV_USERD_ST_Y,
                  wk->pExpStrBuf, wk->fontHandle);

  // �O���[�v
  {
    int sex = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_SEX);
    WifiList_GetFriendName(wk->pList, friendNo, wk->pExpStrBuf);
    WORDSET_RegisterWord( wk->WordSet, 0, wk->pExpStrBuf, sex, TRUE, PM_LANG);
    //        GFL_HEAP_FreeMemory(pTarget);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_034, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_GR_X, MCV_USERD_GR_Y,
                    wk->TitleString, wk->fontHandle);
  }

  // �ΐ퐬��
  {
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_035, wk->TitleString );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_X, MCV_USERD_VS_Y,
                    wk->TitleString, wk->fontHandle);
    // ����
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_WIN);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_036, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_WIN_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
    // �܂�
    num = WifiList_GetFriendInfo(wk->pList, friendNo, WIFILIST_FRIEND_BATTLE_LOSE);
    WORDSET_RegisterNumber(wk->view.p_wordset, 0, num, 4, STR_NUM_DISP_SPACE, NUMBER_CODETYPE_DEFAULT);
    GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_037, wk->pExpStrBuf );
    WORDSET_ExpandStr( wk->view.p_wordset, wk->TitleString, wk->pExpStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->view.userWin), MCV_USERD_VS_LOS_X, MCV_USERD_VS_WIN_Y,
                    wk->TitleString, wk->fontHandle);
  }
  // �|�P��������
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


  // �Ō�ɗV�񂾓��t
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

  // �A�C�R��
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


//----------------------------------------------------------------------------
/**
 *  @brief  �t�����e�B�A��\�����[�h�̃X�N���[���ɕύX
 *
 *  @param  wk    ���[�N
 */
//-----------------------------------------------------------------------------
static void MCVSys_UserDispDrawFrontierOffScrn( WIFIP2PMATCH_WORK *wk )
{
  int i;
  int roop;

  // �t�����e�B�A�{�ݐ��擾
  roop = 1;//WF_USERDISPTYPE_NUM - WF_USERDISPTYPE_BLTW;

  for( i=0; i<roop; i++ ){
    GFL_BG_WriteScreenExpand( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X+(MCV_USERD_NOFR_SCRN_SIZX*i), MCV_USERD_NOFR_SCRN_Y,
                              MCV_USERD_NOFR_SCRN_SIZX, MCV_USERD_NOFR_SCRN_SIZY,
                              wk->view.p_useretcscrn->rawData,
                              0, 0,
                              wk->view.p_useretcscrn->screenWidth/8, wk->view.p_useretcscrn->screenHeight/8 );
  }

  // �p���b�g�ύX
  GFL_BG_ChangeScreenPalette( GFL_BG_FRAME2_S,
                              MCV_USERD_NOFR_SCRN_X, MCV_USERD_NOFR_SCRN_Y,
                              (MCV_USERD_NOFR_SCRN_SIZX*roop), MCV_USERD_NOFR_SCRN_SIZY,
                              MCV_PAL_BTTN+MCV_PAL_BTTN_NONE );
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
    u32 message[]={msg_wifilobby_046,msg_wifilobby_046,msg_wifilobby_027, msg_wifilobby_076,
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

