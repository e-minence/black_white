//=============================================================================
/**
 * @file	ircbattlemenu.c
 * @bfief	�ԊO���ʐM���疳���ɕς���ăo�g�����s�����j���[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	09/02/19
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"

#include "ircbattlemenu.h"
#include "ircbattlematch.h"
#include "infowin/infowin.h"
#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

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

#include "msg/msg_ircbattle.h"
#include "../event_fieldmap_control.h"	//EVENT_FieldSubProc
#include "../event_ircbattle.h"
#include "ircbattle.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK


#define _NET_DEBUG (1)  //�f�o�b�O���͂P
#define _WORK_HEAPSIZE (0x1000)  // �������K�v

// �T�E���h���o����܂ł̉��z
#define _SE_DESIDE (0)
#define _SE_CANCEL (0)
static void Snd_SePlay(int a){}

FS_EXTERN_OVERLAY(ircbattlematch);

//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------
#define _WINDOW_MAXNUM (4)   //�E�C���h�E�̃p�^�[����

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // �^��
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (3)    // �E�C���h�E����
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (11)  // ���b�Z�[�W�t�H���g

//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME3_S)
#define _SUBSCREEN_PALLET	(0xE)


//#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
//#define	_BUTTON_WIN_CGX	( 2 )


//#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// �ʐM�V�X�e���E�B���h�E�]����



typedef struct {
  int leftx;
  int lefty;
  int width;
  int height;
} _WINDOWPOS;


static _WINDOWPOS wind4[]={
  { ((0x20-_BUTTON_WIN_WIDTH)/2), (0x18-(2+_BUTTON_WIN_HEIGHT)*4), _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT},
  { ((0x20-_BUTTON_WIN_WIDTH)/2), (0x18-(2+_BUTTON_WIN_HEIGHT)*3), _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT},
  { ((0x20-_BUTTON_WIN_WIDTH)/2), (0x18-(2+_BUTTON_WIN_HEIGHT)*2), _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT},
  { ((0x20-_BUTTON_WIN_WIDTH)/2), (0x18-(2+_BUTTON_WIN_HEIGHT)), _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT},
};


static const GFL_UI_TP_HITTBL bttndata[] = {
  //�㉺���E
  {	((0x18-(2+_BUTTON_WIN_HEIGHT)*4)*8),(((0x18-(2+_BUTTON_WIN_HEIGHT)*4)*8)+_BUTTON_WIN_HEIGHT*8)-1,
    (((0x20-_BUTTON_WIN_WIDTH)/2)*8),     ((((0x20-_BUTTON_WIN_WIDTH)/2)*8)+_BUTTON_WIN_WIDTH*8)-1  },
  {	((0x18-(2+_BUTTON_WIN_HEIGHT)*3)*8),(((0x18-(2+_BUTTON_WIN_HEIGHT)*3)*8)+_BUTTON_WIN_HEIGHT*8)-1,
    (((0x20-_BUTTON_WIN_WIDTH)/2)*8),     ((((0x20-_BUTTON_WIN_WIDTH)/2)*8)+_BUTTON_WIN_WIDTH*8)-1  },
  {	((0x18-(2+_BUTTON_WIN_HEIGHT)*2)*8),(((0x18-(2+_BUTTON_WIN_HEIGHT)*2)*8)+_BUTTON_WIN_HEIGHT*8)-1,
    (((0x20-_BUTTON_WIN_WIDTH)/2)*8),     ((((0x20-_BUTTON_WIN_WIDTH)/2)*8)+_BUTTON_WIN_WIDTH*8)-1  },
  {	((0x18-(2+_BUTTON_WIN_HEIGHT)*1)*8),(((0x18-(2+_BUTTON_WIN_HEIGHT)*1)*8)+_BUTTON_WIN_HEIGHT*8)-1,
    (((0x20-_BUTTON_WIN_WIDTH)/2)*8),     ((((0x20-_BUTTON_WIN_WIDTH)/2)*8)+_BUTTON_WIN_WIDTH*8)-1  },
  {GFL_UI_TP_HIT_END,0,0,0},		 //�I���f�[�^
};



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



//--------------------------------------------
// �������[�N
//--------------------------------------------

enum _BATTLETYPE_SELECT {
  _SELECTBT_SINGLE = 0,
  _SELECTBT_DOUBLE,
  _SELECTBT_TRI,
  _SELECTBT_EXIT
};


enum _IBMODE_SELECT {
  _SELECTMODE_BATTLE = 0,
  _SELECTMODE_POKE_CHANGE,
	_SELECTMODE_COMPATIBLE,	//�����`�F�b�N
  _SELECTMODE_EXIT
};

enum _IBMODE_ENTRY {
  _ENTRYNUM_DOUBLE = 0,
  _ENTRYNUM_FOUR,
  _ENTRYNUM_EXIT,
};

enum _IBMODE_CHANGE {
	_CHANGE_TRADE=0,
  _CHANGE_FRIENDCHANGE,
  _CHANGE_EXIT,
};


#define _SUBMENU_LISTMAX (2)


typedef void (StateFunc)(IRC_BATTLE_MENU* pState);
typedef BOOL (TouchFunc)(int no, IRC_BATTLE_MENU* pState);


struct _IRC_BATTLE_MENU {
  StateFunc* state;      ///< �n���h���̃v���O�������
  TouchFunc* touch;
  int selectType;   // �ڑ��^�C�v
  HEAPID heapID;
  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// �E�C���h�E�Ǘ�
  GFL_BUTTON_MAN* pButton;
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
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
  EVENT_IRCBATTLE_WORK * dbw;
  int windowNum;
  BOOL IsIrc;
  GAMESYS_WORK *gameSys_;
  FIELD_MAIN_WORK *fieldWork_;
  GMEVENT* event_;
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(IRC_BATTLE_MENU* pWork,StateFunc* state);
static void _changeStateDebug(IRC_BATTLE_MENU* pWork,StateFunc* state, int line);
static void _buttonWindowCreate(int num,int* pMsgBuff,IRC_BATTLE_MENU* pWork);
static void _modeSelectMenuInit(IRC_BATTLE_MENU* pWork);
static void _modeSelectMenuWait(IRC_BATTLE_MENU* pWork);
static void _modeSelectEntryNumInit(IRC_BATTLE_MENU* pWork);
static void _modeSelectEntryNumWait(IRC_BATTLE_MENU* pWork);
static void _modeReportInit(IRC_BATTLE_MENU* pWork);
static void _modeReportWait(IRC_BATTLE_MENU* pWork);
static BOOL _modeSelectMenuButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork);
static BOOL _modeSelectEntryNumButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork);
static BOOL _modeSelectBattleTypeButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork);
static void _modeSelectBattleTypeInit(IRC_BATTLE_MENU* pWork);
static BOOL _modeSelectChangeButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork);
static void _modeSelectChangWait(IRC_BATTLE_MENU* pWork);
static void _modeSelectChangeInit(IRC_BATTLE_MENU* pWork);
static void _buttonWindowDelete(IRC_BATTLE_MENU* pWork);



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

static void _changeState(IRC_BATTLE_MENU* pWork,StateFunc state)
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
static void _changeStateDebug(IRC_BATTLE_MENU* pWork,StateFunc state, int line)
{
  NET_PRINT("ircbtl: %d\n",line);
  _changeState(pWork, state);
}
#endif



static void _createSubBg(IRC_BATTLE_MENU* pWork)
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
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_ON );
    GFL_BG_LoadScreenReq( frame );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �󂯎�������̃E�C���h�E�𓙊Ԋu�ɍ�� ����3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _buttonWindowCreate(int num,int* pMsgBuff,IRC_BATTLE_MENU* pWork)
{
  int i;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

	//_buttonWindowDelete(pWork);

  pWork->windowNum = num;
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);
	
  for(i=0;i < num;i++){
    _WINDOWPOS* pos = wind4;

		GFL_FONTSYS_SetDefaultColor();
		if(pWork->buttonWin[i]==NULL){
			pWork->buttonWin[i] = GFL_BMPWIN_Create(
				frame,
				pos[i].leftx, pos[i].lefty,
				pos[i].width, pos[i].height,
				_BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
		}
    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
    GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
//    GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
		BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

    // �V�X�e���E�C���h�E�g�`��
    GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 4, 4, pWork->pStrBuf, pWork->pFontHandle);
    GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

  }


	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
  pWork->pButton = NULL;
	 
  for(i = num;i < _WINDOW_MAXNUM;i++){
		if(pWork->buttonWin[i]){
			GFL_BMPWIN_ClearScreen(pWork->buttonWin[i]);
			GFL_BG_LoadScreenV_Req(GFL_BG_FRAME1_S);
			BmpWinFrame_Clear(pWork->buttonWin[i], WINDOW_TRANS_OFF);
			GFL_BMPWIN_Delete(pWork->buttonWin[i]);
		}
    pWork->buttonWin[i] = NULL;
  }
	 
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

static void _buttonWindowDelete(IRC_BATTLE_MENU* pWork)
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
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  IRC_BATTLE_MENU *pWork = p_work;
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

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g�S�̂̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeInit(IRC_BATTLE_MENU* pWork)
{
  pWork->IsIrc=FALSE;

  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_ircbattle_dat, pWork->heapID );
  //    GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );

	{
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );

    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircbattle_connect_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    // �T�u���BG0�L�����]��
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_ircbattle_connect_sub_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // �T�u���BG0�X�N���[���]��
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_connect_sub_NSCR,
                                              GFL_BG_FRAME0_S, 0,
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
 * @brief   ���[�h�Z���N�g��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuInit(IRC_BATTLE_MENU* pWork)
{
	int aMsgBuff[]={IRCBTL_STR_01,IRCBTL_STR_02,IRCBTL_STR_15,IRCBTL_STR_03};
	
	_buttonWindowCreate(NELEMS(aMsgBuff), aMsgBuff, pWork);

	pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );
	pWork->touch = &_modeSelectMenuButtonCallback;

	_CHANGE_STATE(pWork,_modeSelectMenuWait);

}

static void _workEnd(IRC_BATTLE_MENU* pWork)
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
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(IRC_BATTLE_MENU* pWork)
{
	if(WIPE_SYS_EndCheck()){
		_CHANGE_STATE(pWork, NULL);        // �I���
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectMenuButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork)
{
  switch( bttnid ){
  case _SELECTMODE_BATTLE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
		_CHANGE_STATE(pWork,_modeSelectEntryNumInit);
    return TRUE;
  case _SELECTMODE_POKE_CHANGE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _CHANGE_STATE(pWork,_modeSelectChangeInit);
    return TRUE;
	case _SELECTMODE_COMPATIBLE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_COMPATIBLE;
    _CHANGE_STATE(pWork,NULL);        // �����f�f���[�h�ֈڂ邽�߂ɏI��
		return TRUE;
  case _SELECTMODE_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
		WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
										WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
    _CHANGE_STATE(pWork,_modeFadeout);        // �I���
    return TRUE;
  default:
    break;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(IRC_BATTLE_MENU* pWork)
{
	if(WIPE_SYS_EndCheck()){
		GFL_BMN_Main( pWork->pButton );
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   ������ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectChangeInit(IRC_BATTLE_MENU* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_18, IRCBTL_STR_14, IRCBTL_STR_03};

  _buttonWindowCreate(NELEMS(aMsgBuff),aMsgBuff,pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );
  pWork->touch = &_modeSelectChangeButtonCallback;

  _CHANGE_STATE(pWork,_modeSelectChangWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   ������ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectChangeButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork)
{
  switch(bttnid){
	case _CHANGE_TRADE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_TRADE;
    _CHANGE_STATE(pWork,_modeReportInit);
    return TRUE;
	case _CHANGE_FRIENDCHANGE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_FRIEND;
    _CHANGE_STATE(pWork,_modeReportInit);
    return TRUE;
  case _ENTRYNUM_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
	default:
    break;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ������ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectChangWait(IRC_BATTLE_MENU* pWork)
{
  GFL_BMN_Main( pWork->pButton );

}






//------------------------------------------------------------------------------
/**
 * @brief   �l���I����ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectEntryNumInit(IRC_BATTLE_MENU* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_04,IRCBTL_STR_05,IRCBTL_STR_03};

  _buttonWindowCreate(3,aMsgBuff,pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );
  pWork->touch = &_modeSelectEntryNumButtonCallback;

  _CHANGE_STATE(pWork,_modeSelectEntryNumWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectEntryNumButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork)
{
  switch(bttnid){
  case _ENTRYNUM_DOUBLE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _CHANGE_STATE(pWork,_modeSelectBattleTypeInit);
    return TRUE;
  case _ENTRYNUM_FOUR:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_MULTH;
    _CHANGE_STATE(pWork,_modeReportInit);
    return TRUE;
  case _ENTRYNUM_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
    return TRUE;
  default:
    break;
  }
  return FALSE;
}



//------------------------------------------------------------------------------
/**
 * @brief   �l���I����ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectBattleTypeInit(IRC_BATTLE_MENU* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_06,IRCBTL_STR_07,IRCBTL_STR_08,IRCBTL_STR_03};

  _buttonWindowCreate(4,aMsgBuff,pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );
  pWork->touch = &_modeSelectBattleTypeButtonCallback;

  _CHANGE_STATE(pWork,_modeSelectEntryNumWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectBattleTypeButtonCallback(int bttnid,IRC_BATTLE_MENU* pWork)
{
  switch(bttnid){
  case _SELECTBT_SINGLE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_SINGLE;
  //  _buttonWindowDelete(pWork);
    _CHANGE_STATE(pWork,_modeReportInit);
    break;
  case _SELECTBT_DOUBLE:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_DOUBLE;
    //_buttonWindowDelete(pWork);
    _CHANGE_STATE(pWork,_modeReportInit);
    break;
  case _SELECTBT_TRI:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    pWork->selectType = EVENTIRCBTL_ENTRYMODE_TRI;
    //_buttonWindowDelete(pWork);
    _CHANGE_STATE(pWork,_modeReportInit);
    break;
  default:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    //_buttonWindowDelete(pWork);
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
    break;
  }
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �l���I����ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectEntryNumWait(IRC_BATTLE_MENU* pWork)
{
  GFL_BMN_Main( pWork->pButton );
}


//------------------------------------------------------------------------------
/**
 * @brief   �͂��������E�C���h�E
 * @retval  none
 */
//------------------------------------------------------------------------------

#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g


static void _YesNoStart(IRC_BATTLE_MENU* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];
  appinit.bgFrame =  GFL_BG_FRAME1_S;
  appinit.palNo = _SUBLIST_NORMAL_PAL;

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 14;

  appinit.msgHandle = pWork->pMsgData;
  appinit.fontHandle = pWork->pFontHandle;
  appinit.printQue = pWork->SysMsgQue;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_27, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_28, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , -8 );
}

//------------------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�̏I���҂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _infoMessageEndCheck(IRC_BATTLE_MENU* pWork)
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

static void _infoMessageDisp(IRC_BATTLE_MENU* pWork)
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
 * @brief   �Z�[�u�m�F��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(IRC_BATTLE_MENU* pWork)
{

  //    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));

  GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME1_S,0);
  
  GFL_MSG_GetString( pWork->pMsgData, IRCBTL_STR_26, pWork->pStrBuf );
  
  _infoMessageDisp(pWork);

  
  _CHANGE_STATE(pWork,_modeReportWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReporting(IRC_BATTLE_MENU* pWork)
{

  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  {
    SAVE_RESULT svr = SaveControl_SaveAsyncMain(IrcBattle_GetSAVE_CONTROL_WORK(pWork->dbw));

    if(svr == SAVE_RESULT_OK){
      pWork->IsIrc = TRUE;  //�ԊO���ڑ��J�n
      _CHANGE_STATE(pWork,NULL);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait2(IRC_BATTLE_MENU* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){


#if 1
      GFL_MSG_GetString( pWork->pMsgData, IRCBTL_STR_29, pWork->pStrBuf );
      _infoMessageDisp(pWork);
      //�Z�[�u�J�n
      SaveControl_SaveAsyncInit( IrcBattle_GetSAVE_CONTROL_WORK(pWork->dbw) );
      _CHANGE_STATE(pWork,_modeReporting);
#else
      _CHANGE_STATE(pWork,NULL);
#endif
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      pWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
      _CHANGE_STATE(pWork,NULL);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait(IRC_BATTLE_MENU* pWork)
{
  if(!_infoMessageEndCheck(pWork)){
    return;
  }
  _YesNoStart(pWork);
  _CHANGE_STATE(pWork,_modeReportWait2);
}

static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

  //        GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_32K,

};


//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMenuProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x18000 );

  {
    IRC_BATTLE_MENU *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_BATTLE_MENU ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(IRC_BATTLE_MENU));
    pWork->heapID = HEAPID_IRCBATTLE;


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

		_createSubBg(pWork);
		_modeInit(pWork);
    pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 1 , 0 );
    pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );

		{
			GAME_COMM_SYS_PTR pGC = GAMESYSTEM_GetGameCommSysPtr(IrcBattle_GetGAMESYS_WORK(pwk));
			INFOWIN_Init( _SUBSCREEN_BGPLANE , _SUBSCREEN_PALLET , pGC , pWork->heapID);
		}
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
static GFL_PROC_RESULT IrcBattleMenuProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MENU* pWork = mywk;
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
	INFOWIN_Update();
  GFL_TCBL_Main( pWork->pMsgTcblSys );
  PRINTSYS_QUE_Main(pWork->SysMsgQue);

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMenuProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MENU* pWork = mywk;
  EVENT_IRCBATTLE_WORK* pParentWork =pwk;

  _workEnd(pWork);
  EVENT_IrcBattleSetType(pParentWork, pWork->selectType);

  //	ConnectBGPalAnm_End(&pWork->cbp);
  GFL_PROC_FreeWork(proc);

  GFL_TCBL_Exit(pWork->pMsgTcblSys);
	INFOWIN_Exit();
	GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  if(pWork->infoDispWin){
    GFL_BMPWIN_Delete(pWork->infoDispWin);
  }


	GFL_BMPWIN_Exit();
	GFL_BG_Exit();


	GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);
  EVENT_IrcBattle_SetEnd(pParentWork);


	
  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA IrcBattleMenuProcData = {
  IrcBattleMenuProcInit,
  IrcBattleMenuProcMain,
  IrcBattleMenuProcEnd,
};


