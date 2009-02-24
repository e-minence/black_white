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
#include "system/main.h"

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

#include "msg/msg_ircbattle.h"
#include "../event_fieldmap_control.h"	//EVENT_FieldSubProc

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
#define _WINDOW_MAXNUM (3)   //�E�C���h�E�̃p�^�[����

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // �^��
#define _BUTTON_WIN_CENTERY (13)   // 
#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (3)    // �E�C���h�E����
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (13)  // ���b�Z�[�W�t�H���g

#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
#define	_BUTTON_WIN_CGX	( 2 )


#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// �ʐM�V�X�e���E�B���h�E�]����



#define	FBMP_COL_WHITE		(15)


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



//--------------------------------------------
// �������[�N
//--------------------------------------------


enum _IBMODE_SELECT {
    _SELECTMODE_BATTLE = 0,
    _SELECTMODE_POKE_CHANGE,
    _SELECTMODE_EXIT
};

enum _IBMODE_ENTRY {
    _ENTRYMODE_SINGLE = 0,
    _ENTRYMODE_DOUBLE,
    _ENTRYMODE_MULTI,
    _ENTRYMODE_EXIT
};





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
    STRBUF* pStrBuf;
    u32 bgchar;  //GFL_ARCUTIL_TRANSINFO
//    BMPWINFRAME_AREAMANAGER_POS aPos;
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
    // �t�B�[���h�ɐݒ肪���������̂ŉ��ݒ� 
    {
        int frame = GFL_BG_FRAME1_S;
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		
        GFL_BG_SetBGControl(
            frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( frame, VISIBLE_ON );
		GFL_BG_SetPriority( frame, 0 );
		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
        
		GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
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

    pWork->windowNum = num;
    
    for(i=0;i < num;i++){
        _WINDOWPOS* pos = wind4;
        
        pWork->buttonWin[i] = GFL_BMPWIN_Create(
            frame,
            pos[i].leftx, pos[i].lefty,
            pos[i].width, pos[i].height,
            _BUTTON_WIN_PAL, GFL_BMP_CHRAREA_GET_F);
        GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0 );
        GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
        GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
        BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

        // �V�X�e���E�C���h�E�g�`��

        GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );
        GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
        PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 4, 4, pWork->pStrBuf, pWork->pFontHandle);
        GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);


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

    GFL_BMN_Delete(pWork->pButton);
    pWork->pButton = NULL;
    for(i=0;i < pWork->windowNum;i++){
        GFL_BMPWIN_Delete(pWork->buttonWin[i]);
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
                Snd_SePlay( _SE_DESIDE );
                return;
            }
            else{
            }
        }
        Snd_SePlay( _SE_CANCEL );
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
    _createSubBg(pWork);
    pWork->IsIrc=FALSE;

    pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
	pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
    pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_ircbattle_dat, pWork->heapID );
//    GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
    pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuInit(IRC_BATTLE_MENU* pWork)
{
    int aMsgBuff[]={IRCBTL_STR_01,IRCBTL_STR_02,IRCBTL_STR_03};
    
    _buttonWindowCreate(3, aMsgBuff, pWork);

    pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );
    pWork->touch = &_modeSelectMenuButtonCallback;

    _CHANGE_STATE(pWork,_modeSelectMenuWait);
}

static void _workEnd(IRC_BATTLE_MENU* pWork)
{
    GFL_FONTSYS_SetDefaultColor();

//    _buttonWindowDelete(pWork);
    GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
    GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));
    GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
    GFL_MSG_Delete( pWork->pMsgData );
	GFL_FONT_Delete(pWork->pFontHandle);
    GFL_STR_DeleteBuffer(pWork->pStrBuf);
    GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );

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
      case _SELECTMODE_POKE_CHANGE:
        _CHANGE_STATE(pWork,_modeSelectEntryNumInit);
        _buttonWindowDelete(pWork);
        return TRUE;
      case _SELECTMODE_EXIT:
        _CHANGE_STATE(pWork,NULL);        // �I���
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
      case _ENTRYMODE_SINGLE:
      case _ENTRYMODE_DOUBLE:
      case _ENTRYMODE_MULTI:
        pWork->selectType = bttnid;
        _buttonWindowDelete(pWork);
        _CHANGE_STATE(pWork,_modeReportInit);
        return TRUE;
      case _ENTRYMODE_EXIT:
        _buttonWindowDelete(pWork);
        _CHANGE_STATE(pWork,_modeSelectMenuInit);
        return TRUE;
      default:
        break;
    }
    return FALSE;
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
 * @brief   �Z�[�u�m�F��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(IRC_BATTLE_MENU* pWork)
{

//    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));

    _CHANGE_STATE(pWork,_modeReportWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait(IRC_BATTLE_MENU* pWork)
{
    pWork->IsIrc = TRUE;  //�ԊO���ڑ��J�n
    _CHANGE_STATE(pWork,NULL);
}

//--------------------------------------------------------------
//	���[�N�T�C�Y�擾
//--------------------------------------------------------------
int IRCBATTLE_MENU_GetWorkSize(void)
{
	return sizeof( IRC_BATTLE_MENU );
}
//--------------------------------------------------------------
//	���[�N������
//--------------------------------------------------------------
void IRCBATTLE_MENU_InitWork( const HEAPID heapID , GAMESYS_WORK *gameSys ,
                              FIELD_MAIN_WORK *fieldWork , GMEVENT *event , IRC_BATTLE_MENU *pWork )
{

    GFL_STD_MemClear(pWork,sizeof(IRC_BATTLE_MENU));
    pWork->heapID = heapID;
    _CHANGE_STATE( pWork, _modeInit);
//    _CHANGE_STATE( pWork, NULL);

	pWork->gameSys_ = gameSys;
	pWork->fieldWork_ = fieldWork;
	pWork->event_ = event;
}

//--------------------------------------------------------------
//	�C�x���g���C��
//--------------------------------------------------------------
GMEVENT_RESULT IRCBATTLE_MENU_Main( GMEVENT *event , int *seq , void *work )
{
    IRC_BATTLE_MENU* pWork = work;

    StateFunc* state = pWork->state;
    if(state != NULL){
        state(pWork);
        return GMEVENT_RES_CONTINUE;
    }
    _workEnd(pWork);

    if(pWork->IsIrc){
//		GAMESYSTEM_EVENT_EntryCheckFunc(pWork->gameSys_, EventIrcBattleCheck, pWork->fieldWork_);

        GMEVENT * newEvent;
        newEvent = EVENT_FieldSubProc(pWork->gameSys_, pWork->fieldWork_,
                                      FS_OVERLAY_ID(ircbattlematch), &IrcBattleMatchProcData, NULL);
        GMEVENT_CallEvent(event, newEvent);

    }
    
    return GMEVENT_RES_CONTINUE; //change���邩��finish���������Ȃ�
}

