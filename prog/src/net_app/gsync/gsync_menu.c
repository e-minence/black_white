//=============================================================================
/**
 * @file	  gsync_menu.c
 * @bfief	  �Q�[���V���N�ŏ��̕\������
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/11/15
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
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

#include "msg/msg_gsync.h"
#include "msg/msg_wifi_system.h"
#include "../../field/event_gsync.h"
#include "gsync.naix"
#include "cg_comm.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "app/app_keycursor.h"  //APP_TASKMENU_INITWORK
#include "system/palanm.h"

#include "net/wih_dwc.h"
#include "gsync_poke.cdat"
#define _PALETTE_R(pal)  (pal & 0x001f)
#define _PALETTE_G(pal)  ((pal & 0x03e0) >> 5)
#define _PALETTE_B(pal)  ((pal & 0x7c00) >> 10)


typedef enum
{
  PLT_OBJ,
  PLT_RESOURCE_MAX,
  CHAR_OBJ = PLT_RESOURCE_MAX,
  CHAR_RESOURCE_MAX,
  ANM_OBJ = CHAR_RESOURCE_MAX,
  ANM_RESOURCE_MAX,
  CEL_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;


#if DEBUG_ONLY_FOR_ohno
#define _NET_DEBUG (1)
#else
#define _NET_DEBUG (0)
#endif
#define _WORK_HEAPSIZE (0x1000)  // �������K�v

// SE
#define _SE_DESIDE (SEQ_SE_SYS_69)
#define _SE_CANCEL (SEQ_SE_SYS_70)


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //�E�C���h�E�̃p�^�[����

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // �^��
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (3)    // �E�C���h�E����

#define _BUTTON_MSG_PAL2   (8)  // ���b�Z�[�W�t�H���g
#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g 9 10 11
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (13)  // ���b�Z�[�W�t�H���g

//BG�ʂƃp���b�g�ԍ�
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME1_S)
#define _SUBSCREEN_PALLET	(0xE)




//-------------------------------------------------------------------------
///	�����\���F��`(default)	-> gflib/fntsys.h�ֈړ�
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))
//------------------------------------------------------------------
//	�J���[�֘A�_�~�[��`
//------------------------------------------------------------------
// �ʏ�̃t�H���g�J���[
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

#define _PALETTE_CHANGE_NUM (7)  //CGEAR�{�^���Ɏg���Ă���p���b�g


//--------------------------------------------
// �������[�N
//--------------------------------------------


enum _IBMODE_SELECT {
  _SELECTMODE_GSYNC = 0,
  _SELECTMODE_UTIL,
  _SELECTMODE_EXIT,
  _SELECTMODE_MAX,
};


#define _SUBMENU_LISTMAX (2)


typedef void (StateFunc)(GAMESYNC_MENU* pState);
typedef BOOL (TouchFunc)(int no, GAMESYNC_MENU* pState);


struct _GAMESYNC_MENU {
  StateFunc* state;      ///< �n���h���̃v���O�������
  TouchFunc* touch;
  int selectType;   // �ڑ��^�C�v
  HEAPID heapID;
  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// �E�C���h�E�Ǘ�
  GFL_BUTTON_MAN* pButton;
  GFL_MSGDATA *pMsgData;  //
  GFL_MSGDATA *pMsgWiFiData;  //
  WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_FONT* pFontHandle;
  STRBUF*  pExpStrBuf;
  STRBUF* pStrBuf;
  u32 bgchar;  //GFL_ARCUTIL_TRANSINFO
  u32 subchar;

  u8 BackupPalette[16 * _PALETTE_CHANGE_NUM *2];
  u8 LightPalette[16 * _PALETTE_CHANGE_NUM *2];
  u16 TransPalette[16 ];

  GFL_BMPWIN* infoDispWin;
  PRINT_STREAM* pStream;
  APP_KEYCURSOR_WORK* pKeyCursor;
  GFL_TCBLSYS *pMsgTcblSys;
  PRINT_QUE*            SysMsgQue;
  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_RES* pAppTaskRes;
  //  APP_TASKMENU_WIN_WORK* pAppWin;
  EVENT_GSYNC_WORK * dbw;
  int windowNum;
  GAMEDATA* gamedata;
  GAMESYS_WORK *gsys;
  int yoffset;
  int anmCnt;  //���莞�A�j���J�E���g
  int bttnid;
  u16 anmCos;
  GAME_COMM_STATUS_BIT bit;
  GAME_COMM_STATUS_BIT bitold;
  void* pVramOBJ;
  void* pVramBG;
  TIMEICON_WORK* pTimeIcon;

  u32 cellRes[CEL_RESOURCE_MAX];
  GFL_CLWK* buttonObj[_SELECTMODE_MAX];
  GFL_CLUNIT	*cellUnit;
  GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��


};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(GAMESYNC_MENU* pWork,StateFunc* state);
static void _changeStateDebug(GAMESYNC_MENU* pWork,StateFunc* state, int line);
static void _buttonWindowCreate(int num,int* pMsgBuff,GAMESYNC_MENU* pWork, _WINDOWPOS* pos);
static void _modeSelectMenuInit(GAMESYNC_MENU* pWork);
static void _modeSelectMenuWait(GAMESYNC_MENU* pWork);

static void _modeReportInit(GAMESYNC_MENU* pWork);
static void _modeReportWait(GAMESYNC_MENU* pWork);
static BOOL _modeSelectMenuButtonCallback(int bttnid,GAMESYNC_MENU* pWork);
static void _modeSelectBattleTypeInit(GAMESYNC_MENU* pWork);

static void _buttonWindowDelete(GAMESYNC_MENU* pWork);
static void _modeFadeout(GAMESYNC_MENU* pWork);
static void _modeFadeoutStart(GAMESYNC_MENU* pWork);


static void _infoMessageDispClear(GAMESYNC_MENU* pWork);
static void _infoMessageEnd(GAMESYNC_MENU* pWork);
static BOOL _infoMessageEndCheck(GAMESYNC_MENU* pWork);
static void _infoMessageDisp(GAMESYNC_MENU* pWork);
static void _infoMessageDispHeight(GAMESYNC_MENU* pWork,int height,BOOL bStream);
static void _hitAnyKey(GAMESYNC_MENU* pWork);



#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG




//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(GAMESYNC_MENU* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(GAMESYNC_MENU* pWork,StateFunc state, int line)
{
  NET_PRINT("gsy: %d\n",line);
  _changeState(pWork, state);
}
#endif



static void _createSubBg(GAMESYNC_MENU* pWork)
{

  // �w�i��
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
      0, 0, 0, FALSE
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
  {
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �^�C���A�C�R�����o��
 * @param   POKEMON_TRADE_WORK
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _MESSAGE_WindowTimeIconStart(GAMESYNC_MENU* pWork)
{
  if(pWork->pTimeIcon){
    TILEICON_Exit(pWork->pTimeIcon);
    pWork->pTimeIcon=NULL;
  }
  pWork->pTimeIcon =
    TIMEICON_CreateTcbl(pWork->pMsgTcblSys,pWork->infoDispWin, 15, TIMEICON_DEFAULT_WAIT, pWork->heapID);
}



//------------------------------------------------------------------------------
/**
 * @brief   �n���ꂽ�e�[�u�������ɖ��邢�p���b�g�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _lightPaletteMake(u16* pal, u16* PaletteTable, int num)
{
  int i,j;
  u16 palr,palg,palb;

  for(i =0 ;i< num;i++){
    for(j=0;j<16;j++){
      palr = pal[i*16+j] & 0x001f;
      palg = (pal[i*16+j] & 0x03e0) >> 5;
      palb = (pal[i*16+j] & 0x7c00) >> 10;
      palr += PaletteTable[j];
      palg += PaletteTable[j];
      palb += PaletteTable[j];
      if(palr>0x1f){
        palr=0x1f;
      }
      if(palg>0x1f){
        palg=0x1f;
      }
      if(palb>0x1f){
        palb=0x1f;
      }
      pal[i*16+j] = palr + (palg<<5) + (palb<<10);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeoutStart(GAMESYNC_MENU* pWork)
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

  if(GFL_NET_IsInit()){
    GameCommSys_ExitReq( GAMESYSTEM_GetGameCommSysPtr(pWork->gsys) );
  }


  _CHANGE_STATE(pWork, _modeFadeout);        // �I���
}



static void _modeAppWinFlashCallback(u32 param, fx32 currentFrame )
{
  GAMESYNC_MENU* pWork = (GAMESYNC_MENU*)param;
  {

    GFL_CLACT_WK_SetAutoAnmFlag( pWork->buttonObj[pWork->bttnid] , FALSE );

    if(pWork->selectType == GAMESYNC_RETURNMODE_UTIL){
      if( OS_IsRunOnTwl() ){//DSI�͌ĂԂ��Ƃ��o���Ȃ�
        GFL_MSG_GetString( pWork->pMsgWiFiData, dwc_message_0017, pWork->pStrBuf );
        _infoMessageDispHeight(pWork,10, FALSE);
        _CHANGE_STATE(pWork,_hitAnyKey);
        return;
      }
    }
#if DEBUG_ONLY_FOR_ohno
    if((pWork->selectType == GAMESYNC_RETURNMODE_UTIL)||
       (pWork->selectType == GAMESYNC_RETURNMODE_SYNC))
#else
    if((pWork->selectType == GAMESYNC_RETURNMODE_UTIL)||
       (pWork->selectType == GAMESYNC_RETURNMODE_SYNC))
#endif
    {
      _CHANGE_STATE(pWork, _modeReportInit);
    }
    else{
      _CHANGE_STATE(pWork, _modeFadeoutStart);        // �I���
    }
  }
}

static void _modeAppWinFlash2(GAMESYNC_MENU* pWork)
{
}



//------------------------------------------------------------------------------
/**
 * @brief   �^�b�`�����ۂɉ�ʂ��_��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeButtonFlash(GAMESYNC_MENU* pWork)
{
  //���莞�A�j��
  int pltNo = Btn_PalettePos[pWork->bttnid];
  const u8 isBlink = (pWork->anmCnt/APP_TASKMENU_ANM_INTERVAL)%2;
  if( isBlink == 0 )
  {
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                        pltNo * 32 ,
                                        &pWork->BackupPalette[32*pltNo] , 32 );
  }
  else
  {
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                        pltNo * 32 ,
                                        &pWork->LightPalette[32*pltNo] , 32 );
  }
  pWork->anmCnt++;

  if( pWork->anmCnt >= APP_TASKMENU_ANM_CNT )
  {
    //    APP_TASKMENU_WIN_Delete( pWork->pAppWin );
    //   pWork->pAppWin = NULL;

    if(WIRELESSSAVE_ON == CONFIG_GetWirelessSaveMode(SaveData_GetConfig(pWork->dbw->ctrl))){
      _CHANGE_STATE(pWork, _modeReportInit);
    }
    else{
      _CHANGE_STATE(pWork, _modeFadeoutStart);        // �I���
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �󂯎�������̃E�C���h�E�𓙊Ԋu�ɍ�� ����3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _buttonWindowCreate(int num,int* pMsgBuff,GAMESYNC_MENU* pWork, _WINDOWPOS* pos)
{
  int i;
  u32 cgx;
  int frame = GFL_BG_FRAME3_S;

  pWork->windowNum = num;

  GFL_FONTSYS_SetDefaultColor();


  for(i = 0;i < _WINDOW_MAXNUM;i++){
    if(pWork->buttonWin[i]){
      GFL_BMPWIN_ClearScreen(pWork->buttonWin[i]);
      BmpWinFrame_Clear(pWork->buttonWin[i], WINDOW_TRANS_OFF);
      GFL_BMPWIN_Delete(pWork->buttonWin[i]);
    }
    pWork->buttonWin[i] = NULL;
  }

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL2, 0x20, pWork->heapID);


  
  for(i=0;i < num;i++){
    pWork->buttonWin[i] = GFL_BMPWIN_Create(
      frame,
      pos[i].leftx, pos[i].lefty,
      pos[i].width, pos[i].height,
      _BUTTON_MSG_PAL2,GFL_BMP_CHRAREA_GET_F);
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0 );
    // �V�X�e���E�C���h�E�g�`��
    GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );

    GFL_FONTSYS_SetColor(15, 2, 0);

    PRINTSYS_Print(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 0,
                   pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
    GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  }
  if(pWork->pButton){
    GFL_BMN_Delete(pWork->pButton);
  }
  pWork->pButton = NULL;
  GFL_BG_LoadScreenReq(frame);
  //  GFL_BG_SetVisible(frame,VISIBLE_ON);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _buttonWindowDelete(GAMESYNC_MENU* pWork)
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

      GFL_BMPWIN_Delete(pWork->buttonWin[i]);
    }
    pWork->buttonWin[i] = NULL;
  }
  pWork->windowNum = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  GAMESYNC_MENU *pWork = p_work;
  u32 friendNo;

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< �G�ꂽ�u��
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

static void _CreateButtonObj(GAMESYNC_MENU* pWork)
{
  int i;
  u8 buffx[]={ 128,    128,  224};
  u8 buffy[]={ 192/2 , 192/2, 177};
  u8 buttonno[]={14,13, 0};


  for(i=0;i<_SELECTMODE_MAX;i++){
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = buffx[i];
    cellInitData.pos_y = buffy[i];
    cellInitData.anmseq = buttonno[i];
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    pWork->buttonObj[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                               pWork->cellRes[CHAR_OBJ],
                                               pWork->cellRes[PLT_OBJ],
                                               pWork->cellRes[ANM_OBJ],
                                               &cellInitData ,CLSYS_DRAW_SUB , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->buttonObj[i] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( pWork->buttonObj[i], TRUE );
    GFL_CLACT_WK_SetObjMode(pWork->buttonObj[i],GX_OAM_MODE_XLU);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g�S�̂̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeInit(GAMESYNC_MENU* pWork)
{

  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_gsync_dat, pWork->heapID );
  pWork->pMsgWiFiData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, pWork->heapID );

  //    GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_CG_COMM, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_cg_comm_comm_bg_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_cg_comm_background_NCLR,
                                      PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);

    {
      NNSG2dPaletteData* pPal;
      void* pData;

      pData = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_cg_comm_comm_bg_NCLR, &pPal,  pWork->heapID);
      GFL_STD_MemCopy(pPal->pRawData,pWork->BackupPalette,_PALETTE_CHANGE_NUM*32);
      GFL_STD_MemCopy(pPal->pRawData,pWork->LightPalette,_PALETTE_CHANGE_NUM*32);
      _lightPaletteMake((u16*)pWork->LightPalette, Btn_PaletteTable, _PALETTE_CHANGE_NUM);

      GFL_HEAP_FreeMemory(pData);
    }
    // �T�u���BG0�L�����]��
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_cg_comm_comm_bg_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // �T�u���BG0�X�N���[���]��
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_base_NSCR,
                                              GFL_BG_FRAME0_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_wifi_btn_NSCR,
                                              GFL_BG_FRAME2_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);

    GFL_ARC_CloseDataHandle(p_handle);
  }



  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);



  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_CG_COMM, pWork->heapID );
    pWork->cellRes[CHAR_OBJ] =
      GFL_CLGRP_CGR_Register( p_handle , NARC_cg_comm_comm_btn_NCGR ,
                              FALSE , CLSYS_DRAW_SUB , pWork->heapID );
    pWork->cellRes[PLT_OBJ] =
      GFL_CLGRP_PLTT_RegisterEx(
        p_handle ,NARC_cg_comm_comm_btn_NCLR , CLSYS_DRAW_SUB ,    0, 0, 10, pWork->heapID  );
    pWork->cellRes[ANM_OBJ] =
      GFL_CLGRP_CELLANIM_Register(
        p_handle , NARC_cg_comm_comm_btn_NCER, NARC_cg_comm_comm_btn_NANR , pWork->heapID  );

    GFL_ARC_CloseDataHandle(p_handle);
  }
  _CreateButtonObj(pWork);

}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuInit(GAMESYNC_MENU* pWork)
{

  int aMsgBuff[]={GAMESYNC_001};

  _buttonWindowCreate(NELEMS(aMsgBuff), aMsgBuff, pWork,wind_wifi);

  pWork->pButton = GFL_BMN_Create( btn_wifi, _BttnCallBack, pWork,  pWork->heapID );
  pWork->touch = &_modeSelectMenuButtonCallback;


  _CHANGE_STATE(pWork,_modeSelectMenuWait);

}

static void _workEnd(GAMESYNC_MENU* pWork)
{
  GFL_FONTSYS_SetDefaultColor();

  _buttonWindowDelete(pWork);
  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_BG_FreeBGControl(GFL_BG_FRAME0_S);
  GFL_MSG_Delete( pWork->pMsgData );
  GFL_MSG_Delete( pWork->pMsgWiFiData );
  GFL_FONT_Delete(pWork->pFontHandle);
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  //  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );

}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g���� + �ʐM�I���m�F
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(GAMESYNC_MENU* pWork)
{
  if(WIPE_SYS_EndCheck()){
    switch(pWork->selectType){
    case GAMESYNC_RETURNMODE_SYNC:
    case GAMESYNC_RETURNMODE_UTIL:
      if(GAME_COMM_NO_NULL==GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(pWork->gsys))){
        _CHANGE_STATE(pWork, NULL);        // �I���
      }
      break;
    default:
      _CHANGE_STATE(pWork, NULL);        // �I���
      break;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �L�[����������ŏ��ɖ߂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _hitAnyKey(GAMESYNC_MENU* pWork)
{
  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  if(GFL_UI_TP_GetTrg()){
    _infoMessageEnd(pWork);
    PMSND_PlaySystemSE(_SE_DESIDE);
    //    APP_TASKMENU_WIN_Delete( pWork->pAppWin );
    //    pWork->pAppWin = NULL;
    _CHANGE_STATE(pWork, _modeSelectMenuInit); //�߂�
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlash(GAMESYNC_MENU* pWork)
{
  GFL_CLWK_ANM_CALLBACK cbwk;

  cbwk.callback_type = CLWK_ANM_CALLBACK_TYPE_LAST_FRM ;  // CLWK_ANM_CALLBACK_TYPE
  cbwk.param = (u32)pWork;          // �R�[���o�b�N���[�N
  cbwk.p_func = _modeAppWinFlashCallback; // �R�[���o�b�N�֐�
  GFL_CLACT_WK_SetAutoAnmFlag(pWork->buttonObj[pWork->bttnid],TRUE);
  GFL_CLACT_WK_StartAnmCallBack( pWork->buttonObj[pWork->bttnid], &cbwk );
  GFL_CLACT_WK_ResetAnm( pWork->buttonObj[pWork->bttnid] );
  GFL_CLACT_WK_StartAnm( pWork->buttonObj[pWork->bttnid] );

  _CHANGE_STATE(pWork,_modeAppWinFlash2);

}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectMenuButtonCallback(int bttnid,GAMESYNC_MENU* pWork)
{
  pWork->bttnid=bttnid;

  switch( bttnid ){
  case _SELECTMODE_GSYNC:
    if(GAME_COMM_SBIT_WIFI_ALL & pWork->bit){
      PMSND_PlaySystemSE(_SE_DESIDE);
      GFL_CLACT_WK_SetAnmSeq( pWork->buttonObj[0], 14 );
      _CHANGE_STATE(pWork,_modeAppWinFlash);
      pWork->selectType = GAMESYNC_RETURNMODE_SYNC;
    }
    //    else{
    //      PMSND_PlaySystemSE(_SE_CANCEL);
    //    }
    break;
  case _SELECTMODE_UTIL:
    PMSND_PlaySystemSE(_SE_DESIDE);
    pWork->selectType = GAMESYNC_RETURNMODE_UTIL;
    _CHANGE_STATE(pWork,_modeAppWinFlash);
    break;
  case _SELECTMODE_EXIT:
    PMSND_PlaySystemSE(_SE_CANCEL);
    //    APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
    pWork->selectType = GAMESYNC_RETURNMODE_NONE;
    _CHANGE_STATE(pWork,_modeAppWinFlash);        // �I���
    break;
  default:
    break;
  }


  return TRUE;
}

static void _UpdatePalletAnimeSingle(GAMESYNC_MENU* pWork , u16 anmCnt , u8 srcPltNo,  u8 distPltNo)
{
  int i;
  const u16* pal = (u16*)&pWork->BackupPalette[32*srcPltNo];
  const u16* lpal = (u16*)&pWork->LightPalette[32*distPltNo];

  for(i = 0;i<16;i++){
    //1�`0�ɕϊ�
    const u32 br = _PALETTE_R(pal[i]);
    const u32 bg = _PALETTE_G(pal[i]);
    const u32 bb = _PALETTE_B(pal[i]);

    const fx16 cos = (FX_CosIdx(anmCnt)+FX16_ONE)/2;
    const u8 r = br + (((_PALETTE_R(lpal[i])-br)*cos)>>FX16_SHIFT);
    const u8 g = bg + (((_PALETTE_G(lpal[i])-bg)*cos)>>FX16_SHIFT);
    const u8 b = bb + (((_PALETTE_B(lpal[i])-bb)*cos)>>FX16_SHIFT);
    u16 palx[16];

    pWork->TransPalette[i] = GX_RGB(r, g, b);
    //    OS_TPrintf("%d pal %x  %x\n",i,pal[i],pWork->TransPalette[i]);
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                          srcPltNo * 32,  pWork->TransPalette , 32 );
    }
  }
}

static void _changeGsyncAnime(GAMESYNC_MENU* pWork , int no)
{
  if(no != GFL_CLACT_WK_GetAnmSeq(pWork->buttonObj[0])){
    GFL_CLACT_WK_SetAutoAnmFlag(pWork->buttonObj[0],TRUE);
    GFL_CLACT_WK_SetAnmSeq( pWork->buttonObj[0], no );
  }
}


//--------------------------------------------------------------
//	�p���b�g�A�j���[�V�����̍X�V
//--------------------------------------------------------------
static void _UpdatePalletAnime(GAMESYNC_MENU* pWork )
{
  //�v���[�g�A�j��
  if(pWork->bit != pWork->bitold){
    if(GAME_COMM_STATUS_BIT_WIFI & pWork->bit){  //�� wep�o�^
      _changeGsyncAnime(pWork,24);
    }
    else if(GAME_COMM_STATUS_BIT_WIFI_FREE & pWork->bit){  //���F  �t���[
      _changeGsyncAnime(pWork,23);
    }
    else if(GAME_COMM_STATUS_BIT_WIFI_ZONE & pWork->bit){  //�� �X�|�b�g
      _changeGsyncAnime(pWork,22);
    }
    else if(GAME_COMM_STATUS_BIT_WIFI_LOCK & pWork->bit){   //��  ������
      _changeGsyncAnime(pWork,21);
    }
    else{
      _changeGsyncAnime(pWork,14);
    }
  }



}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(GAMESYNC_MENU* pWork)
{
  if(WIPE_SYS_EndCheck()){
    GFL_BMN_Main( pWork->pButton );
  }
  _UpdatePalletAnime(pWork);
}



#if 1


static void _PaletteFadeSingle(GAMESYNC_MENU* pWork, int type, int palettenum)
{
  u32 addr;
  PALETTE_FADE_PTR pP = PaletteFadeInit(pWork->heapID);
  PaletteFadeWorkAllocSet(pP, type, 16 * 32, pWork->heapID);
  PaletteWorkSet_VramCopy( pP, type, 0, palettenum*32);
  SoftFadePfd(pP, type, 0, 16 * palettenum, 6, 0);
  addr = (u32)PaletteWorkTransWorkGet( pP, type );

  switch(type){
  case FADE_SUB_OBJ:
    GXS_LoadOBJPltt((void*)addr, 0, palettenum * 32);
    break;
  case FADE_SUB_BG:
    GXS_LoadBGPltt((void*)addr, 0, palettenum * 32);
    break;
  }
  PaletteFadeWorkAllocFree(pP,type);
  PaletteFadeFree(pP);
}



static void _PaletteFade(GAMESYNC_MENU* pWork,BOOL bFade)
{
  u32 addr;

  if(bFade){
    {
      GFL_STD_MemCopy((void*)HW_DB_OBJ_PLTT, pWork->pVramOBJ, 16*2*16);
      GFL_STD_MemCopy((void*)HW_DB_BG_PLTT, pWork->pVramBG, 16*2*16);
      _PaletteFadeSingle( pWork,  FADE_SUB_OBJ, 14);
      _PaletteFadeSingle( pWork,  FADE_SUB_BG, 9);
    }
  }
  else{
    GXS_LoadOBJPltt((void*)pWork->pVramOBJ, 0, 14 * 32);
    GXS_LoadBGPltt((void*)pWork->pVramBG, 0, 9 * 32);
  }

}
#endif


//------------------------------------------------------------------------------
/**
 * @brief   �͂��������E�C���h�E
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _YesNoStart(GAMESYNC_MENU* pWork)
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
  GFL_MSG_GetString(pWork->pMsgData, GAMESYNC_005, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, GAMESYNC_006, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  APP_TASKMENU_SetDisableKey(pWork->pAppTask, TRUE);  //�L�[�}��
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);

  _PaletteFade(pWork, TRUE);
  //G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 |GX_BLEND_PLANEMASK_BG2 |GX_BLEND_PLANEMASK_BG3 , -8 );
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̉�ʏ���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _infoMessageDispClear(GAMESYNC_MENU* pWork)
{
  if(pWork->pTimeIcon){
    TILEICON_Exit(pWork->pTimeIcon);
    pWork->pTimeIcon=NULL;
  }
  BmpWinFrame_Clear(pWork->infoDispWin, WINDOW_TRANS_OFF);
  GFL_BMPWIN_ClearScreen(pWork->infoDispWin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _infoMessageEnd(GAMESYNC_MENU* pWork)
{
  _infoMessageDispClear(pWork);
  GFL_BMPWIN_Delete(pWork->infoDispWin);
  pWork->infoDispWin=NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I���҂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _infoMessageEndCheck(GAMESYNC_MENU* pWork)
{
  int state;

  if(pWork->pStream){
    if(pWork->infoDispWin){
      APP_KEYCURSOR_Main( pWork->pKeyCursor, pWork->pStream, pWork->infoDispWin );
    }
    state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    switch(state){
    case PRINTSTREAM_STATE_DONE:
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
      break;
    case PRINTSTREAM_STATE_PAUSE:
      if(GFL_UI_TP_GetTrg()){
        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
      }
      break;
    default:
      break;
    }
    return FALSE;  //�܂��I����ĂȂ�
  }
  return TRUE;// �I����Ă���
}


//------------------------------------------------------------------------------
/**
 * @brief   �����E�C���h�E�\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _infoMessageDispHeight(GAMESYNC_MENU* pWork,int height,BOOL bStream)
{
  GFL_BMPWIN* pwin;


  if(pWork->infoDispWin==NULL){
    pWork->infoDispWin = GFL_BMPWIN_Create(
      GFL_BG_FRAME1_S ,
      1 , 3, 30 , height ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = pWork->infoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  if(bStream){
    pWork->pStream = PRINTSYS_PrintStream(pwin ,0,0, pWork->pStrBuf, pWork->pFontHandle,
                                          MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);
  }
  else{
    PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin), 0, 0, pWork->pStrBuf, pWork->pFontHandle);
  }

  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
}


static void _infoMessageDisp(GAMESYNC_MENU* pWork)
{
  _infoMessageDispHeight( pWork,4,TRUE);
}


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(GAMESYNC_MENU* pWork)
{

  //    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));

  GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME1_S,0);

  GFL_MSG_GetString( pWork->pMsgData, GAMESYNC_004, pWork->pStrBuf );

  _infoMessageDisp(pWork);


  _CHANGE_STATE(pWork,_modeReportWait);
}


static void _FadeWait(GAMESYNC_MENU* pWork)
{
  if(GFL_FADE_CheckFade()){
    return;
  }
  _CHANGE_STATE(pWork,NULL);
}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReporting(GAMESYNC_MENU* pWork)
{

  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  {
    SAVE_RESULT svr = GAMEDATA_SaveAsyncMain(pWork->gamedata);

    if(svr == SAVE_RESULT_OK){
      _infoMessageEnd(pWork);
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait2(GAMESYNC_MENU* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      if(SaveControl_IsOverwritingOtherData( GAMEDATA_GetSaveControlWork(pWork->gamedata))){
        GFL_MSG_GetString( pWork->pMsgData, GSYNC_027, pWork->pStrBuf );
        _infoMessageDisp(pWork);
        _CHANGE_STATE(pWork,  _hitAnyKey );
      }
      else{
        GFL_MSG_GetString( pWork->pMsgData, GAMESYNC_007, pWork->pStrBuf );
        _infoMessageDisp(pWork);
        _MESSAGE_WindowTimeIconStart(pWork);
        //�Z�[�u�J�n
        GAMEDATA_SaveAsyncStart(pWork->gamedata);
        _CHANGE_STATE(pWork,_modeReporting);
      }
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME1_S);
      pWork->selectType = GAMESYNC_RETURNMODE_NONE;
      _CHANGE_STATE(pWork,  _modeFadeoutStart );
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    _PaletteFade(pWork, FALSE);
    //      G2S_BlendNone();
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait(GAMESYNC_MENU* pWork)
{
  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  _YesNoStart(pWork);
  _CHANGE_STATE(pWork,_modeReportWait2);
}

//--------------------------------------------------------------
/**
 * G3D VBlank����
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	_VBlank( GFL_TCB *tcb, void *work )
{
  //  GAMESYNC_MENU *pWork=work;

  GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank
}


static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,

};
//--------------------------------------------------------------
///	�Z���A�N�^�[�@�������f�[�^
//--------------------------------------------------------------
static const GFL_CLSYS_INIT _CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //�ʐM�A�C�R������
  0,
  100,
  100,
  100,
  100,
  16, 16,
};


//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GSYNC_WORK* pParentWork =pwk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x18000 );

  {
    GAMESYNC_MENU *pWork = GFL_PROC_AllocWork( proc, sizeof( GAMESYNC_MENU ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(GAMESYNC_MENU));
    pWork->heapID = HEAPID_IRCBATTLE;
    pWork->gamedata = pParentWork->gameData;
    pWork->gsys = pParentWork->gsys;

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
    GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
    GFL_DISP_GXS_SetVisibleControlDirect(0);


    GFL_CLACT_SYS_Create(	&_CLSYS_Init, &_defVBTbl, pWork->heapID );
    pWork->cellUnit = GFL_CLACT_UNIT_Create( 40 , 0 , pWork->heapID );
    pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

    _createSubBg(pWork);
    _modeInit(pWork);

    pWork->pVramOBJ = GFL_HEAP_AllocMemory(pWork->heapID, 16*2*16);  //�o�b�N�A�b�v
    pWork->pVramBG = GFL_HEAP_AllocMemory(pWork->heapID, 16*2*16);  //�o�b�N�A�b�v
    pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 3 , 0 );
    pWork->pKeyCursor = APP_KEYCURSOR_Create( 15, FALSE, TRUE, pWork->heapID );
    pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
    pWork->pAppTaskRes =
      APP_TASKMENU_RES_Create( GFL_BG_FRAME1_S, _SUBLIST_NORMAL_PAL,
                               pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID  );

    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG0 , 15, 4 );
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
    GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ );
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
    pWork->dbw = pwk;
  }
  GFL_NET_ReloadIcon();

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESYNC_MENU* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }

  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
  //  if(pWork->pAppWin){
  //    APP_TASKMENU_WIN_Update( pWork->pAppWin );
  //  }
  if(GFL_NET_IsInit()){
    pWork->bitold =  pWork->bit;
    pWork->bit = WIH_DWC_GetAllBeaconTypeBit(NULL);
  }

  GFL_TCBL_Main( pWork->pMsgTcblSys );
  PRINTSYS_QUE_Main(pWork->SysMsgQue);
  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, pWork->yoffset );
  pWork->yoffset--;
  GFL_CLACT_SYS_Main();

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GAMESYNC_MENU* pWork = mywk;
  EVENT_GSYNC_WORK* pParentWork =pwk;
  int i;

  for(i=0;i<_SELECTMODE_MAX; i++){
    GFL_CLACT_WK_Remove(pWork->buttonObj[i]);
  }
  GFL_CLGRP_PLTT_Release(pWork->cellRes[PLT_OBJ] );
  GFL_CLGRP_CGR_Release(pWork->cellRes[CHAR_OBJ] );
  GFL_CLGRP_CELLANIM_Release(pWork->cellRes[ANM_OBJ] );
  GFL_TCB_DeleteTask( pWork->g3dVintr );
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  if(pWork->pTimeIcon){
    TILEICON_Exit(pWork->pTimeIcon);
    pWork->pTimeIcon=NULL;
  }

  _workEnd(pWork);
  pParentWork->selectType = pWork->selectType;

  GFL_PROC_FreeWork(proc);

  GFL_HEAP_FreeMemory(pWork->pVramOBJ);
  GFL_HEAP_FreeMemory(pWork->pVramBG);
  APP_KEYCURSOR_Delete( pWork->pKeyCursor );
  GFL_TCBL_Exit(pWork->pMsgTcblSys);
  GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  if(pWork->infoDispWin){
    GFL_BMPWIN_Delete(pWork->infoDispWin);
  }
  //  if(pWork->pAppWin){
  //    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  //  }

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
const GFL_PROC_DATA GameSyncMenuProcData = {
  GameSyncMenuProcInit,
  GameSyncMenuProcMain,
  GameSyncMenuProcEnd,
};


