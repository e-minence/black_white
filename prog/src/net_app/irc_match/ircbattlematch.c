//=============================================================================
/**
 * @file	ircbattlematch.c
 * @bfief	�ԊO���}�b�`���O
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	09/02/24
 */
//=============================================================================

#include <gflib.h>
#include "arc_def.h"
#include "net/network_define.h"
#include "system/net_err.h"
#include "ircbattlematch.h"
#include "system/main.h"
#include "system/wipe.h"

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
#include "ircbattle.naix"
#include "ircbattlecommon.h"
#include "cg_comm.naix"
#include "net_app/connect_anm.h"
//#include "../../field/event_ircbattle.h"
#include "ir_ani_NANR_LBLDEFS.h"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "net_app/irc_match.h"

#include "ircbattlematch.cdat"

#include "net/dwc_rapfriend.h"
#include "poke_tool/status_rcv.h"
#include "savedata/etc_save.h"



enum _IBMODE_SELECT {
  _SELECTMODE_EXIT,
  _SELECTMODE_DECIDE,
};


typedef enum
{
  PLT_DS,
  PLT_RESOURCE_MAX,
  CHAR_DS = PLT_RESOURCE_MAX,
  CHAR_RESOURCE_MAX,
  ANM_DS = CHAR_RESOURCE_MAX,
  ANM_RESOURCE_MAX,
  CEL_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;

typedef enum
{
  CELL_IRDS1,
  CELL_IRDS2,
  CELL_IRDS3,
  CELL_IRDS4,
  CELL_IRWAVE1,
  CELL_IRWAVE2,
  CELL_IRWAVE3,
  CELL_IRWAVE4,
  _CELL_DISP_NUM,
} _CELL_WK_ENUM;



#if DEBUG_ONLY_FOR_ohno
#define _NET_DEBUG (1)
#else
#define _NET_DEBUG (0)
#endif
#define _WORK_HEAPSIZE (0x1000)  // �������K�v
#define _BRIGHTNESS_SYNC (2)  // �t�F�[�h�̂r�x�m�b�͗v����



//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------
#define _WINDOW_MAXNUM (3)   //�E�C���h�E�̃p�^�[����

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // �^��
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (30)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (5)    // �E�C���h�E����
#define _BUTTON_WIN_PAL   (14)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (13)  // ���b�Z�[�W�t�H���g
#define _START_PAL   (12)  // �X�^�[�g�E�C���h�E

#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
#define	_BUTTON_WIN_CGX	( 2 )


#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// �ʐM�V�X�e���E�B���h�E�]����

#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g


#define	FBMP_COL_WHITE		(15)

#define _FULL_TIMER   (180)  //�l���������ς��Ō����ł��Ȃ��ꍇ�̃��b�Z�[�W�\������


//-------------------------------------------------------------------------
///	�����\���F��`(default)	-> gflib/fntsys.h�ֈړ�
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))

typedef void (StateFunc)(IRC_BATTLE_MATCH* pState);

//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(IRC_BATTLE_MATCH* pWork,StateFunc* state);
static void _changeStateDebug(IRC_BATTLE_MATCH* pWork,StateFunc* state, int line);
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void _endCallBack(void* pWork);
static void _buttonWindowCreate(int num,int* pMsgBuff,IRC_BATTLE_MATCH* pWork);
static void _ircMatchStart(IRC_BATTLE_MATCH* pWork);
static void _fadeInWait(IRC_BATTLE_MATCH* pWork);
static void _ircInitWait(IRC_BATTLE_MATCH* pWork);
static void _ircMatchWait(IRC_BATTLE_MATCH* pWork);
static void _modeSelectEntryNumInit(IRC_BATTLE_MATCH* pWork);
static void _modeSelectEntryNumWait(IRC_BATTLE_MATCH* pWork);
static void _modeReportInit(IRC_BATTLE_MATCH* pWork);
static void _modeReportWait(IRC_BATTLE_MATCH* pWork);
static BOOL _modeSelectEntryNumButtonCallback(int bttnid,IRC_BATTLE_MATCH* pWork);
static void _connectCallBack(void* pWork, int netID);
static void _RecvModeCheckData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _ircPreConnect(IRC_BATTLE_MATCH* pWork);
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work );
static BOOL _cancelButtonCallback2(int bttnid,IRC_BATTLE_MATCH* pWork);
static BOOL _cancelButtonCallback(int bttnid,IRC_BATTLE_MATCH* pWork);
static void _RecvMyStatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvPokePartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvFriendCode(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvMacAddr(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvMultiPartyNo(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static u8* _getPokePartyBuff(int netID, void* pWork, int size);
static void _firstConnectMessage(IRC_BATTLE_MATCH* pWork);
static void _msgWindowCreate(int* pMsgBuff,IRC_BATTLE_MATCH* pWork);
static void _friendNumWindowCreate(int msgno,IRC_BATTLE_MATCH* pWork);
static void _returnMessage(IRC_BATTLE_MATCH* pWork);
static void _modeCheckStartTok(IRC_BATTLE_MATCH* pWork);
static void _ReturnButtonStart(IRC_BATTLE_MATCH* pWork,BOOL bDecide );
static void ircExitStart2(IRC_BATTLE_MATCH* pWork);
static void _buttonWindowDelete(IRC_BATTLE_MATCH* pWork);
static void _ReturnButtonStart(IRC_BATTLE_MATCH* pWork ,BOOL bDecide);
static void _modeCheckStart2(IRC_BATTLE_MATCH* pWork);
static void _YesNoStart(IRC_BATTLE_MATCH* pWork);
static void _waitFinish(IRC_BATTLE_MATCH* pWork);


///�ʐM�R�}���h
typedef enum {
  _NETCMD_TYPESEND = GFL_NET_CMD_IRCBATTLE,
  _NETCMD_MYSTATUSSEND,
  _NETCMD_POKEPARTY_SEND,
  _NETCMD_FRIENDCODE,
  _NETCMD_MACADDR,
  _NETCMD_MULTIPARTY_NO,
} _BATTLEIRC_SENDCMD;


#define _TIMINGNO_CS (120)
#define _TIMINGNO_DS (121)
#define _TIMINGNO_POKEP (122)
#define _TIMINGNO_DSF (123)
#define _TIMINGNO_TYPECHECK (124)
#define _TIMINGNO_MACADDR (125)
#define _TIMINGNO_MULTISEND (126)
#define _TIMINGNO_ENDFRIEND (127)

//--------------------------------------------
// �������[�N
//--------------------------------------------



typedef struct{
  int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };


//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------

static void* IrcBattleBeaconGetFunc(void* pWork)
{
  return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int IrcBattleBeaconGetSizeFunc(void* pWork)
{
  return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo, void* pWork)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}



///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_RecvModeCheckData,          NULL},  ///_NETCMD_TYPESEND  ����̐ԊO���^�C�v����M
  {_RecvMyStatusData,          NULL},  ///_NETCMD_MYSTATUSSEND
  {_RecvPokePartyData,       _getPokePartyBuff},  ///_NETCMD_POKEPARTY_SEND
  {_recvFriendCode,         NULL},    ///_NETCMD_FRIENDCODE
  {_recvMacAddr,         NULL},  //  _NETCMD_MACADDR,
  {_recvMultiPartyNo,         NULL},  //  _NETCMD_MULTIPARTY_NO,

  
};


#define _IRCMATCH_MAX (4)  //�S�l�Ƃ�������

#define _MAXNUM   (2)         // �ő�ڑ��l��
#define _MAXSIZE  (100)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��

static GFLNetInitializeStruct aGFLNetInit = {
  NULL,//_PacketTbl,  // ��M�֐��e�[�u��
  0,//NELEMS(_PacketTbl), // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  _connectCallBack,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  IrcBattleBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
  IrcBattleBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  IrcBattleBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  SYACHI_NETWORK_GGID,  //ggid  
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_IRC,   //�ԊO���ʐM�p��create�����HEAPID
  GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  _MAXNUM,     // �ő�ڑ��l��
  _MAXSIZE,  //�ő呗�M�o�C�g��
  _BCON_GET_NUM,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_COMPATI_CHECK,  //GameServiceID
  0xfffe,	// �ԊO���^�C���A�E�g����
  0,//MP�ʐM�e�@���M�o�C�g��
  0,//dummy
};


#define _SUBMENU_LISTMAX (2)

typedef BOOL (TouchFunc)(int no, IRC_BATTLE_MATCH* pState);


struct _IRC_BATTLE_MATCH {
  IRC_BG_WORK aIrcBgWork;
  IRC_MATCH_WORK* pBattleWork;
  StateFunc* state;      ///< �n���h���̃v���O�������
  int selectType;   // �ڑ��^�C�v
  int selectTypeNet[_IRCMATCH_MAX];   // �ڑ��^�C�v
  u8 macadd[4][6];
  HEAPID heapID;
  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// �E�C���h�E�Ǘ�
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_FONT* pFontHandle;
  STRBUF* pExStrBuf;
  STRBUF* pStrBuf;
  BMPWINFRAME_AREAMANAGER_POS aPos;
  int windowNum;
  BOOL IsIrc;
  BMPMENU_WORK* pYesNoWork;
  //    GAMESYS_WORK *gameSys_;
  //    FIELD_MAIN_WORK *fieldWork_;
  
  GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
  GFL_CLUNIT	*cellUnit;
  u32 cellRes[CEL_RESOURCE_MAX];
  GFL_CLWK* curIcon[_CELL_DISP_NUM];

  u32 connect_bit;
  BOOL connect_ok;
  BOOL receive_ok;
  u32 receive_result_param;
  u32 receive_first_param;
  GFL_ARCUTIL_TRANSINFO mainchar;
  GFL_ARCUTIL_TRANSINFO bgchar;
  GFL_ARCUTIL_TRANSINFO bgchar2;
  GFL_ARCUTIL_TRANSINFO subchar;
  CONNECT_BG_PALANM cbp;		// Wifi�ڑ���ʂ�BG�p���b�g�A�j������\����
  BOOL bParent;
  BOOL ircmatchflg;
  BOOL ircmatchanim;
  int ircmatchanimCount;

  BOOL irccenterflg;
  BOOL bFriendAdd;
  BOOL ircCenterAnim;
  int ircCenterAnimCount;
  int yoffset;
  BOOL SecondStep;

  GFL_BUTTON_MAN* pButton;
  TouchFunc* touch;
  int timer;
  int messageBackup;
  void* pVramBG;
  PRINT_QUE*            SysMsgQue;
  BOOL bReturnButton;
  APP_TASKMENU_WORK* pAppTask;
	APP_TASKMENU_RES* pAppTaskRes;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_WIN_WORK* pAppWin;
  APP_TASKMENU_WIN_WORK* pAppWinLeader;

  int musicalNum;
  BOOL bBGBlack;

};



enum{
  _START_TIMING=12,
};




#if _NET_DEBUG
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

static void _changeState(IRC_BATTLE_MATCH* pWork,StateFunc state)
{
//  NET_PRINT("irch: %x\n",(u32)state);
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#if _NET_DEBUG
static void _changeStateDebug(IRC_BATTLE_MATCH* pWork,StateFunc state, int line)
{
  NET_PRINT("ircmatch: %d\n",line);
  _changeState(pWork, state);
}
#endif

static void _endCallBack(void* pWork)
{
  IRC_BATTLE_MATCH *commsys = pWork;

  NET_PRINT("endCallBack�I��\n");
  commsys->connect_ok = FALSE;
  commsys->connect_bit = 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM���f
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _netBreakOff(IRC_BATTLE_MATCH* pWork)
{
  pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
  GFL_NET_IRCWIRELESS_ResetSystemError();
  GFL_NET_Exit(NULL);
}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(IRC_BATTLE_MATCH* pWork)
{
  if(WIPE_SYS_EndCheck()){
    if(pWork->pBattleWork->netInitWork){
      //����Ă��鏉�����\���̂ɕύX
//      GFL_NET_ChangeInitStruct(pWork->pBattleWork->netInitWork);
    }
    _CHANGE_STATE(pWork, NULL);        // �I���
  }
}


#if 1


static void _PaletteFadeSingle(IRC_BATTLE_MATCH* pWork, int type, int palettenum)
{
  u32 addr;
  PALETTE_FADE_PTR pP = PaletteFadeInit(pWork->heapID);
  PaletteFadeWorkAllocSet(pP, type, 16 * 32, pWork->heapID);
  PaletteWorkSet_VramCopy( pP, type, 0, palettenum*32);
  SoftFadePfd(pP, type, 0, 16 * palettenum, 6, 0);
  addr = (u32)PaletteWorkTransWorkGet( pP, type );

  DC_FlushRange((void*)addr, palettenum * 32);
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



static void _PaletteFade(IRC_BATTLE_MATCH* pWork,BOOL bFade)
{
  u32 addr;

  if(bFade){
    {
      GFL_STD_MemCopy((void*)HW_DB_BG_PLTT, pWork->pVramBG, 16*2*16);
      _PaletteFadeSingle( pWork,  FADE_SUB_BG, 9);
    }
  }
  else{
    GXS_LoadBGPltt((void*)pWork->pVramBG, 0, 9 * 32);
  }

}
#endif


//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeoutStart(IRC_BATTLE_MATCH* pWork)
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  _CHANGE_STATE(pWork, _modeFadeout);        // �I���
}

//----------------------------------------------------------------------------
/**
 *	@brief	�g��OBJ�\��
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


static void _CLACT_SetResource(IRC_BATTLE_MATCH* pWork)
{
  int i=0;

  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );

  pWork->cellRes[CHAR_DS] =
    GFL_CLGRP_CGR_Register( p_handle , NARC_ircbattle_ir_demo_ani_NCGR ,
                            FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
  pWork->cellRes[PLT_DS] =
    GFL_CLGRP_PLTT_RegisterEx(
      p_handle ,NARC_ircbattle_ir_demo_obj_NCLR , CLSYS_DRAW_MAIN, 0, 0, 3, pWork->heapID  );
  pWork->cellRes[ANM_DS] =
    GFL_CLGRP_CELLANIM_Register(
      p_handle , NARC_ircbattle_ir_ani_NCER, NARC_ircbattle_ir_ani_NANR , pWork->heapID  );
  GFL_ARC_CloseDataHandle(p_handle);

}

static void _CLACT_SetAnim(IRC_BATTLE_MATCH* pWork,int x,int y,int no,int anm)
{
  if(pWork->curIcon[no]==NULL){
    GFL_CLWK_DATA cellInitData;

    cellInitData.pos_x = x;
    cellInitData.pos_y = y;
    cellInitData.anmseq = anm;
    cellInitData.softpri = no;
    cellInitData.bgpri = 2;
    pWork->curIcon[no] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->cellRes[CHAR_DS],
                                              pWork->cellRes[PLT_DS],
                                              pWork->cellRes[ANM_DS],
                                              &cellInitData ,CLSYS_DRAW_MAIN , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[no] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[no], TRUE );
    GFL_CLACT_WK_SetAffineParam(pWork->curIcon[no], CLSYS_AFFINETYPE_DOUBLE);
  }
}



static void _CLACT_AddPos(IRC_BATTLE_MATCH* pWork,int x,int y,int no)
{
  GFL_CLACTPOS apos;

  if(pWork->curIcon[no]){
    GFL_CLACT_WK_GetPos(pWork->curIcon[no],&apos,CLSYS_DEFREND_MAIN);
    apos.x+=x;
    apos.y+=y;
    GFL_CLACT_WK_SetPos(pWork->curIcon[no],&apos,CLSYS_DEFREND_MAIN);
  }
}

static void _CLACT_SetPos(IRC_BATTLE_MATCH* pWork,int x,int y,int no)
{
  GFL_CLACTPOS apos;

  if(pWork->curIcon[no]){
    apos.x=x;
    apos.y=y;
    GFL_CLACT_WK_SetPos(pWork->curIcon[no],&apos,CLSYS_DEFREND_MAIN);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLACT�J��
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

static void _CLACT_Release(IRC_BATTLE_MATCH* pWork)
{
  int i=0;

  for(i = 0 ; i < _CELL_DISP_NUM ;i++){
    if(pWork->curIcon[i]!=NULL){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
      pWork->curIcon[i]=NULL;
    }
  }
  for(i=0;i<PLT_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_PLTT_Release(pWork->cellRes[i] );
    }
  }
  for(;i<CHAR_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CGR_Release(pWork->cellRes[i] );
    }
  }
  for(;i<ANM_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CELLANIM_Release(pWork->cellRes[i] );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWk, int netID)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  u32 temp;

  OS_TPrintf("�l�S�V�G�[�V�������� netID = %d\n", netID);
  pWork->connect_bit |= 1 << netID;
  temp = pWork->connect_bit;
  if(MATH_CountPopulation(temp) >= 2){
    OS_TPrintf("�S���̃l�S�V�G�[�V�������� �l��bit=%x\n", pWork->connect_bit);
    pWork->connect_ok = TRUE;
  }
}



//--------------------------------------------------------------
/**
 * @brief   �ԊO���I�����C�����X�J�n�R�[���o�b�N
            ���̃R�[���o�b�N�͐ԊO���̈��̃}�b�`���O��ɕK���Ă΂��B
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _ircConnectEndCallback(void* pWk)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  int no;

  OS_TPrintf("_ircConnectEndCallback\n");

  if(pWork->selectType!=EVENTIRCBTL_ENTRYMODE_MULTH){
    pWork->ircmatchflg=TRUE;
    pWork->ircmatchanim=TRUE;
    pWork->SecondStep = TRUE;
  }
  else if(pWork->bParent){
    if(pWork->ircCenterAnimCount!=0){
      pWork->ircmatchflg=TRUE;
      pWork->ircmatchanim=TRUE;
    }
  }
}




//--------------------------------------------------------------
/**
 * @brief   �����Ă�������̃��[�h�̊m�F  _NETCMD_TYPESEND
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvModeCheckData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  const int* pType = pData;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }

  ///�����ɑ����Ă�����̂́A���S�ɒʐM�Ⴂ�͂���ꂽ��
  pWork->selectTypeNet[netID]=pType[0];
}

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS��M�֐� _NETCMD_MYSTATUSSEND
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvMyStatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  MYSTATUS* pTargetSt;
  const u16 *pRecvData = pData;
  IRC_BATTLE_MATCH* pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  //�O�C�P�C�Q�C�R���ʐM�p�ɓ���Ă��ǂ�
  pTargetSt = GAMEDATA_GetMyStatusPlayer(pWork->pBattleWork->gamedata, netID);  //netID�ő��
  NET_PRINT("MYSTATUS GET\n");
  MyStatus_Copy(pData, pTargetSt);

  {
    GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
    ETC_SAVE_WORK * pETC = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(pGameData) );
    EtcSave_SetAcquaintance(pETC, MyStatus_GetID(pTargetSt));
  }

//  void EtcSave_SetAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id)
  
}

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY��M�֐� _NETCMD_POKEPARTY_SEND
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvPokePartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  // ���łɃo�b�t�@�ɓ����Ă���
}

static u8* _getPokePartyBuff(int netID, void* pWk, int size)
{
  IRC_BATTLE_MATCH* pWork = pWk;
  if(netID >= 0 && netID < 4){
    return (u8*)pWork->pBattleWork->pNetParty[netID];
  }
  return NULL;
}


//--------------------------------------------------------------
/**
 * @brief   POKEPARTY��M�֐� _NETCMD_POKEPARTY_SEND
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _recvFriendCode(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  MYSTATUS* pTargetSt;
  GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
  
  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }

  pTargetSt = GAMEDATA_GetMyStatusPlayer(pGameData, netID);  //netID�ő��

  pWork->bFriendAdd = GFL_NET_DWC_FriendDataAdd(pGameData, pTargetSt, (DWCFriendData*)pData, pWork->heapID);

  {
//    ETC_SAVE_WORK * pETC = SaveData_GetEtc( GAMEDATA_GetSaveControlWork(pGameData) );
//    EtcSave_SetAcquaintance(pETC, MyStatus_GetID(pTargetSt));
  }
}


//--------------------------------------------------------------
/**
 * @brief   �}�b�N�A�h���X��M�֐� _NETCMD_MACADDR
 */
//--------------------------------------------------------------
static void _recvMacAddr(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{

  IRC_BATTLE_MATCH *pWork = pWk;
  MYSTATUS* pTargetSt;
  GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
  int no,i;
  const u8* macadd = pData;
  
  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(GFL_NET_IsParentMachine()){

      OS_TPrintf("MAC %2x%2x%2x%2x%2x%2x\n",
                 macadd[0],
                 macadd[1],
                 macadd[2],
                 macadd[3],
                 macadd[4],
                 macadd[5]
                 );

    for(i=0;i<4;i++){
      if(0==GFL_STD_MemComp(pWork->macadd[i], macadd, 6)){
        pWork->pBattleWork->MultiNo[ netID ] = i;
        return;
      }
    }
    GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * @brief   �}���`�ł̃p�[�e�B�[���я���M�֐� _NETCMD_MULTIPARTY_NO
 */
//--------------------------------------------------------------
static void _recvMultiPartyNo(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{

  IRC_BATTLE_MATCH *pWork = pWk;
  MYSTATUS* pTargetSt;
  GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
  int no;
  
  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }
  GFL_STD_MemCopy(pData , pWork->pBattleWork->MultiNo, 4);
}




static void _modeSuccessMessageKeyWait(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_UI_TP_GetTrg()){
    _netBreakOff(pWork);
    _CHANGE_STATE(pWork,_modeFadeoutStart);
  }

}

static void _modeSuccessTimingEnd2(IRC_BATTLE_MATCH* pWork)
{
  pWork->timer++;
  if(pWork->timer>60){
    GFL_NET_Exit(NULL);
    _CHANGE_STATE(pWork,_modeSuccessMessageKeyWait);
  }
}


static void _modeSuccessTimingEnd(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_ENDFRIEND, WB_NET_IRCBATTLE)){
    GFL_NET_SetAutoErrorCheck(FALSE);
    GFL_NET_SetNoChildErrorCheck(FALSE);
    pWork->timer=0;
    _CHANGE_STATE(pWork,_modeSuccessTimingEnd2);
  }
}

static void _modeSuccessMessage(IRC_BATTLE_MATCH* pWork)
{
  if(pWork->bFriendAdd){
    int aMsgBuff[]={IRCBTL_STR_43};
    _msgWindowCreate(aMsgBuff, pWork);
  }
  else{
    int aMsgBuff[]={IRCBTL_STR_44};
    _msgWindowCreate(aMsgBuff, pWork);
  }
  _friendNumWindowCreate(IRCBTL_STR_35, pWork);
  PMSND_PlaySystemSE(SEQ_SE_FLD_124);
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_ENDFRIEND, WB_NET_IRCBATTLE);
  _CHANGE_STATE(pWork,_modeSuccessTimingEnd);
}

static int _searchPokeParty(IRC_BATTLE_MATCH* pWork)
{
  int num = PokeParty_GetPokeCountBattleEnable(pWork->pBattleWork->pNetParty[0]);
  int num2 = PokeParty_GetPokeCountBattleEnable(pWork->pBattleWork->pNetParty[1]);

  OS_TPrintf("%d %d\n",num,num2);
  if(num > num2){
    return num2;
  }
  return num;
}



static void _modeCheckStart5(IRC_BATTLE_MATCH* pWork)
{
 if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP, WB_NET_IRCBATTLE)){
   switch(pWork->selectType){
   case EVENTIRCBTL_ENTRYMODE_FRIEND:
#if DEBUG_ONLY_FOR_ohno
     {
       GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
       MYSTATUS* pFriend = GAMEDATA_GetMyStatusPlayer(pGameData, 1-GFL_NET_SystemGetCurrentID());
       WIFI_NEGOTIATION_SV_SetFriend(GAMEDATA_GetWifiNegotiation(pGameData), pFriend);
     }
#endif
     _CHANGE_STATE(pWork,_modeSuccessMessage);
     break;
   case EVENTIRCBTL_ENTRYMODE_SINGLE:
   case EVENTIRCBTL_ENTRYMODE_DOUBLE:
   case EVENTIRCBTL_ENTRYMODE_TRI:
   case EVENTIRCBTL_ENTRYMODE_ROTATE:
     STATUS_RCV_PokeParty_RecoverAll(pWork->pBattleWork->pNetParty[0]);  //�S��
     STATUS_RCV_PokeParty_RecoverAll(pWork->pBattleWork->pNetParty[1]);  //�S��
     switch(_searchPokeParty(pWork)){
     case 0:
       GF_ASSERT(0);
       break;
     case 1:
       pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_SINGLE;
       break;
     case 2:
       if(pWork->pBattleWork->selectType > EVENTIRCBTL_ENTRYMODE_DOUBLE){
         pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_DOUBLE;
       }
       break;
       
     }
     _CHANGE_STATE(pWork,_modeFadeoutStart);
   default:
     _CHANGE_STATE(pWork,_modeFadeoutStart);
     break;
   }
 }
}



static void _modeCheckStart42(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_MULTISEND, WB_NET_IRCBATTLE)){
    if(GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, _NETCMD_POKEPARTY_SEND,
                          PokeParty_GetWorkSize(), pWork->pBattleWork->pParty,
                          FALSE,FALSE,TRUE)){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP,WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart5);
    }
  }
}


static void _modeCheckStart41(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_MACADDR, WB_NET_IRCBATTLE)){

    if(GFL_NET_IsParentMachine()){
      if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_MULTIPARTY_NO,
                          sizeof(pWork->pBattleWork->MultiNo), pWork->pBattleWork->MultiNo)){
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_MULTISEND,WB_NET_IRCBATTLE);
        _CHANGE_STATE(pWork,_modeCheckStart42);
      }
    }
    else{
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_MULTISEND,WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart42);
    }
  }
}

static void _modeCheckStart4(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS, WB_NET_IRCBATTLE)){
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
    case EVENTIRCBTL_ENTRYMODE_MUSICAL:
    case EVENTIRCBTL_ENTRYMODE_SUBWAY:
      _CHANGE_STATE(pWork,_modeFadeoutStart);
      break;
    default:
      {
        u8 macaddr[6];
        OS_GetMacAddress(macaddr);
        if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_MACADDR, sizeof(macaddr), macaddr)){
          GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_MACADDR,WB_NET_IRCBATTLE);
          _CHANGE_STATE(pWork,_modeCheckStart41);
        }
      }
      break;
    }
  }
}


static void _modeCheckStart33(IRC_BATTLE_MATCH* pWork)
{

  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DSF, WB_NET_IRCBATTLE)){
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_FRIEND:
      {
        DWCFriendData friendData;

        GFL_NET_DWC_GetMySendedFriendCode(
          GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ,&friendData);
        
        if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_FRIENDCODE,
                            sizeof(DWCFriendData),&friendData)){
          GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS,WB_NET_IRCBATTLE);
          _CHANGE_STATE(pWork,_modeCheckStart4);
          //���j���[�E�B���h�E�����邩������Ȃ��̂ŁA�����ŃN���[�Y���� 201009604 add Saito
          if (pWork->pAppTask)
          {
            NOZOMU_Printf("���j���[���܂�\n");
            APP_TASKMENU_CloseMenu(pWork->pAppTask);
            pWork->pAppTask=NULL;
          }
        }
      }
      break;
    default:
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS,WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart4);
      break;
    }
  }
}



static void _modeCheckStart3(IRC_BATTLE_MATCH* pWork)
{
  
  
  OS_TPrintf("mystatus %x\n",(u32)GAMEDATA_GetMyStatus(pWork->pBattleWork->gamedata));
  OS_TPrintf("gamedata %x\n",(u32)pWork->pBattleWork->gamedata);
  
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_MYSTATUSSEND,
                      MyStatus_GetWorkSize(), GAMEDATA_GetMyStatus(pWork->pBattleWork->gamedata))){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMINGNO_DSF, WB_NET_IRCBATTLE);
    _CHANGE_STATE(pWork,_modeCheckStart33);
  }
}


static void _modeCheckStart31(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_TYPECHECK, WB_NET_IRCBATTLE)){
    int id = GFL_NET_SystemGetCurrentID();
    
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_SINGLE:
    case EVENTIRCBTL_ENTRYMODE_DOUBLE:
    case EVENTIRCBTL_ENTRYMODE_TRI:
    case EVENTIRCBTL_ENTRYMODE_ROTATE:
      //���[�������ɂ��낦��d�l
      if(pWork->selectType > pWork->selectTypeNet[1-id] ){
        pWork->pBattleWork->selectType = pWork->selectTypeNet[1-id];
      }
      break;
    }

    _CHANGE_STATE(pWork,_modeCheckStart3);
  }
}




static void _ircExitWaitNO_TS2(IRC_BATTLE_MATCH* pWork)
{
  _buttonWindowDelete(pWork);
  {
    int aMsgBuff[1];
    aMsgBuff[0] = pWork->messageBackup;
    _msgWindowCreate(aMsgBuff, pWork);
  }
  _ReturnButtonStart(pWork,FALSE);
  _CHANGE_STATE(pWork,_modeCheckStart2);

}

//------------------------------------------------------------------------------
/**
 * @brief   �h�q�b�ڑ��ҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircExitWait_TS2(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    if(selectno == 0)
    { // �͂���I�������ꍇ
      _buttonWindowDelete(pWork);
      _netBreakOff(pWork);
      
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
    else
    {  // ��������I�������ꍇ
      _CHANGE_STATE(pWork,_ircExitWaitNO_TS2);
    }
    _PaletteFade(pWork,FALSE);
    
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ���f����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void ircExitStart_TS2(IRC_BATTLE_MATCH* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_16};
  int backup = pWork->messageBackup;

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*12, 0x20, pWork->heapID);
  _msgWindowCreate(aMsgBuff, pWork);
  pWork->messageBackup = backup;
  _YesNoStart(pWork);
  GFL_FONTSYS_SetDefaultColor();
  _CHANGE_STATE(pWork,_ircExitWait_TS2);
}




static void _modeCheckStart2(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    _CHANGE_STATE(pWork,ircExitStart_TS2);
    return;
  }

  
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_CS,WB_NET_IRCBATTLE)){
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_TYPESEND, sizeof(pWork->selectType), &pWork->selectType)){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), _TIMINGNO_TYPECHECK, WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart31);
    }
  }
}


//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _modeCheckStart(IRC_BATTLE_MATCH* pWork)
{
  GFL_NET_AddCommandTable(GFL_NET_CMD_IRCBATTLE, _PacketTbl, NELEMS(_PacketTbl), pWork);
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_CS, WB_NET_IRCBATTLE);

  GFL_NET_SetAutoErrorCheck(TRUE);
  GFL_NET_SetNoChildErrorCheck(TRUE);

  _CHANGE_STATE(pWork,_modeCheckStart2);
}

//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _wirelessConnectCallback(void* pWk)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  int i;

  if(GFL_NET_IsParentMachine()){
    for(i=0;i<4;i++){
      GFL_NET_IRCWIRELESS_GetMacAddress(pWork->macadd[i], i);
    }
  }
  _CHANGE_STATE(pWork, _modeCheckStart);
}


static void _wirelessPreConnectCallback(void* pWk,BOOL bParent)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  pWork->bParent = bParent;
  pWork->irccenterflg=TRUE;
  pWork->ircCenterAnim=TRUE;

  OS_TPrintf("-_wirelessPreConnectCallback-\n",bParent);
  
  if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MULTH){
    if(!bParent){
      pWork->ircmatchflg=TRUE;
      pWork->ircmatchanim=TRUE;
    }
  }
}


static const GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_128K,		// �T�uOBJ�}�b�s���O���[�h
};


//------------------------------------------------------------------------------
/**
 * @brief   BG�̈�ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _createBg(IRC_BATTLE_MATCH* pWork)
{
  {
    GFL_DISP_SetBank( &_defVBTbl );
  }
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }
  GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  {
    int frame = GFL_BG_FRAME0_M;
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl( frame, &bgcntText, GFL_BG_MODE_TEXT );
    //  GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME1_M;
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl( frame, &bgcntText, GFL_BG_MODE_TEXT );
    //    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    // GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    //		GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME1_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    //		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME2_S;
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
  }
  {
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

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
  GFL_CLACT_SYS_VBlankFunc();	//�Z���A�N�^�[VBlank

}


static void _YesNoStart(IRC_BATTLE_MATCH* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 13;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_27, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_28, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  pWork->appitem[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;


  pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);


  switch(pWork->selectType){
  case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
  case EVENTIRCBTL_ENTRYMODE_MUSICAL:
  case EVENTIRCBTL_ENTRYMODE_SUBWAY:
    break;
  default:
    APP_TASKMENU_SetDisableKey(pWork->pAppTask, TRUE);  //�L�[�}��
    break;
  }

  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  _PaletteFade(pWork,TRUE);
//  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , -8 );
}


static void _ReturnButtonStart(IRC_BATTLE_MATCH* pWork ,BOOL bDecide)
{
  int i;
  APP_TASKMENU_INITWORK appinit;


  appinit.heapId = pWork->heapID;
  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 24;
  appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
  appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  if(bDecide){
    appinit.itemNum =  2;
    appinit.itemWork =  &pWork->appitem[0];
    pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_36, pWork->appitem[0].str);
    pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
    pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_03, pWork->appitem[1].str);
    pWork->appitem[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    pWork->appitem[1].type = APP_TASKMENU_WIN_TYPE_RETURN;
    pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
    GFL_STR_DeleteBuffer(pWork->appitem[0].str);
    GFL_STR_DeleteBuffer(pWork->appitem[1].str);
    pWork->bReturnButton=FALSE;
  }
  else{
    appinit.itemNum = 1;
    appinit.itemWork =  &pWork->appitem[0];
    pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_03, pWork->appitem[0].str);
    pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_RETURN;

    pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
    GFL_STR_DeleteBuffer(pWork->appitem[0].str);
    pWork->bReturnButton=TRUE;
  }
  switch(pWork->selectType){
  case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
  case EVENTIRCBTL_ENTRYMODE_MUSICAL:
  case EVENTIRCBTL_ENTRYMODE_SUBWAY:
    break;
  default:
    APP_TASKMENU_SetDisableKey(pWork->pAppTask, TRUE);  //�L�[�}��
    break;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �󂯎�������̃E�C���h�E�𓙊Ԋu�ɍ�� ����3char IRCBTL_STR_16
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _msgWindowCreate(int* pMsgBuff,IRC_BATTLE_MATCH* pWork)
{
  int i=0;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  if(pWork->buttonWin[i]==NULL){
    pWork->buttonWin[i] = GFL_BMPWIN_Create(
      frame,
      1, 1, _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT,
      _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
    GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
    BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );
  }
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
//  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

  pWork->messageBackup = pMsgBuff[i];
  
  GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );
  //    GFL_FONTSYS_SetColor( 1, 1, 1 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 4, 4, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

}


//------------------------------------------------------------------------------
/**
 * @brief   �Ƃ������̐l���\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _friendNumWindowCreate(int msgno,IRC_BATTLE_MATCH* pWork)
{
  int i=1;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

  if(pWork->buttonWin[i]==NULL){
    pWork->buttonWin[i] = GFL_BMPWIN_Create(
      frame,
      1, 8, 18, 4,
      _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
    GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
    BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );
  }
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
//  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

  GFL_MSG_GetString(  pWork->pMsgData, msgno, pWork->pExStrBuf );
  {
    int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
    int num2 = WIFILIST_FRIEND_MAX;
    WORDSET_RegisterNumber(pWork->pWordSet, 0, num1, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_RegisterNumber(pWork->pWordSet, 1, num2, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);  
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  }
  
  //    GFL_FONTSYS_SetColor( 1, 1, 1 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 2, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

}



//------------------------------------------------------------------------------
/**
 * @brief   �~���[�W�J���̐l���\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _musicalNumWindowCreate(int msgno,IRC_BATTLE_MATCH* pWork)
{
  int i=1;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

  if(!pWork->buttonWin[i]){
    pWork->buttonWin[i] = GFL_BMPWIN_Create(
      frame,
      1, 8, 23, 4,
      _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  }
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
  BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );

  GFL_MSG_GetString(  pWork->pMsgData, msgno, pWork->pExStrBuf );
  {
    int num1 = GFL_NET_GetNowConnectNum_IRCWIRELESS();
    WORDSET_RegisterNumber(pWork->pWordSet, 0, num1, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  }
  
  //    GFL_FONTSYS_SetColor( 1, 1, 1 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 2, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

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

static void _buttonWindowDelete(IRC_BATTLE_MATCH* pWork)
{
  int i;

  if(pWork->buttonWin[0]){
    GFL_BMPWIN_Delete(pWork->buttonWin[0]);
  }
  if(pWork->buttonWin[1]){
    GFL_BMPWIN_Delete(pWork->buttonWin[1]);
  }
  pWork->buttonWin[0]=NULL;
  pWork->buttonWin[1]=NULL;
}


static void _graphicInit(IRC_BATTLE_MATCH* pWork)
{
  GFL_BG_Init( pWork->heapID );
  GFL_BMPWIN_Init( pWork->heapID );
  GFL_FONTSYS_Init();
}


static void _graphicEnd(IRC_BATTLE_MATCH* pWork)
{
  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

}


static void clactSafeRemove(IRC_BATTLE_MATCH* pWork,int i)
{
  if(pWork->curIcon[i]){
    GFL_CLACT_WK_Remove(pWork->curIcon[i]);
    pWork->curIcon[i]=NULL;
  }
}


static void _firstConnectMessage(IRC_BATTLE_MATCH* pWork)
{
  if( pWork->SecondStep == TRUE){
    int aMsgBuff[]={IRCBTL_STR_12};
    _msgWindowCreate(aMsgBuff, pWork);
  }
  else{
    if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_FRIEND)
    {
      int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
      if(num1==WIFILIST_FRIEND_MAX){
        int aMsgBuff[]={IRCBTL_STR_34};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_33};
        _msgWindowCreate(aMsgBuff, pWork);
        _friendNumWindowCreate(IRCBTL_STR_35, pWork);
      }
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_TRADE)
    {
      int aMsgBuff[]={IRCBTL_STR_17};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MULTH)
    {
      int aMsgBuff[]={IRCBTL_STR_10};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER)
    {
      int aMsgBuff[]={IRCBTL_STR_38};
      _msgWindowCreate(aMsgBuff, pWork);
      _musicalNumWindowCreate(IRCBTL_STR_40, pWork);
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MUSICAL)
    {
      int aMsgBuff[]={IRCBTL_STR_39};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_SUBWAY)  //20100605 add Saito
    {
      int aMsgBuff[]={IRCBTL_STR_47};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else
    {
      int aMsgBuff[]={IRCBTL_STR_09};
      _msgWindowCreate(aMsgBuff, pWork);
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g�S�̂̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeInit(IRC_BATTLE_MATCH* pWork)
{
  GFL_FONTSYS_SetDefaultColor();
  pWork->pWordSet = WORDSET_CreateEx( 11, 200, pWork->heapID );
  _graphicInit(pWork);
  _createBg(pWork);

  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pExStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_ircbattle_dat, pWork->heapID );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );



    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircbattle_ir_demo_NCLR,
                                      PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);
    pWork->mainchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_ircbattle_ir_demo_NCGR,
                                                                   GFL_BG_FRAME0_M, 0, 0, pWork->heapID);

    if(EVENTIRCBTL_ENTRYMODE_MULTH!=pWork->selectType){ //1vs1
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo1_NSCR,
                                                GFL_BG_FRAME1_M, 0,
                                                GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                                pWork->heapID);
    }
    else{
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo1_NSCR,
                                                GFL_BG_FRAME0_M, 0,
                                                GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                                pWork->heapID);
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo2_NSCR,
                                                GFL_BG_FRAME1_M, 0,
                                                GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                                pWork->heapID);
    }
    GFL_ARC_CloseDataHandle( p_handle );
  }

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_CG_COMM, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_cg_comm_comm_bg_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    // �T�u���BG0�L�����]��
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_cg_comm_comm_bg_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // �T�u���BG0�X�N���[���]��
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_base_NSCR,
                                              GFL_BG_FRAME0_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);
    GFL_ARC_CloseDataHandle(p_handle);
  }


  pWork->bgchar2 = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  {
    _firstConnectMessage(pWork);
  }

  GFL_CLACT_SYS_Create(	&GFL_CLSYSINIT_DEF_DIVSCREEN, &_defVBTbl, pWork->heapID );
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 40 , 0 , pWork->heapID );
  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

  pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
  pWork->pAppTaskRes =
    APP_TASKMENU_RES_Create( GFL_BG_FRAME2_S, _SUBLIST_NORMAL_PAL,
                             pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID  );
  


  _CLACT_SetResource(pWork);

  //  if(EVENTIRCBTL_ENTRYMODE_MULTH)

  if(EVENTIRCBTL_ENTRYMODE_MULTH!=pWork->selectType){ //1vs1
//    int keisu=4;

    _CLACT_SetAnim(pWork,57,154,CELL_IRDS3,NANR_ir_ani_CellAnime10);  //��
    _CLACT_SetAnim(pWork,128,113,CELL_IRWAVE1,NANR_ir_ani_CellAnime6);

    _CLACT_SetAnim(pWork,200,  38,CELL_IRDS1,NANR_ir_ani_CellAnime11); //�E
    _CLACT_SetAnim(pWork,128, 77,CELL_IRWAVE2,NANR_ir_ani_CellAnime7);

  }
  else{ //2vs2

    _CLACT_SetAnim(pWork,  57, 154,CELL_IRDS1,NANR_ir_ani_CellAnime2);  //P1
    _CLACT_SetAnim(pWork, 198, 154,CELL_IRDS2,NANR_ir_ani_CellAnime3); //P2
    _CLACT_SetAnim(pWork, 200,  38,CELL_IRDS3,NANR_ir_ani_CellAnime4); //P3
    _CLACT_SetAnim(pWork,  55,  38,CELL_IRDS4,NANR_ir_ani_CellAnime5); //P4
    _CLACT_SetAnim(pWork, 152,  146,CELL_IRWAVE3,NANR_ir_ani_CellAnime0);
    _CLACT_SetAnim(pWork, 103,  146,CELL_IRWAVE4,NANR_ir_ani_CellAnime1);
    _CLACT_SetAnim(pWork, 152,  46,CELL_IRWAVE1,NANR_ir_ani_CellAnime0);
    _CLACT_SetAnim(pWork, 103,  46,CELL_IRWAVE2,NANR_ir_ani_CellAnime1);
  }


  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_OBJ );

  pWork->aIrcBgWork.heapID = pWork->heapID;

  ircBGAnimInit(&pWork->aIrcBgWork);

  _CHANGE_STATE(pWork,_fadeInWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�C���҂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _fadeInWait(IRC_BATTLE_MATCH* pWork)
{
  // ���C�v�I���҂�
  if( GFL_FADE_CheckFade() ){
    if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_FRIEND){
      int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
      if(num1==WIFILIST_FRIEND_MAX){
        _CHANGE_STATE(pWork,_waitFinish);        // �I���()
        pWork->timer = _FULL_TIMER;
        return;
      }
    }
    _CHANGE_STATE(pWork,_ircMatchStart);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �~���[�W�J����p���[�h��r
 * @retval  �����Ă����TRUE
 */
//------------------------------------------------------------------------------
static BOOL _gsidcallback(u8 mygsid, u8 targetgsid)
{
  if((WB_NET_MUSICAL_LEADER==mygsid) && (WB_NET_MUSICAL_CHILD==targetgsid)){
    return TRUE;
  }
  else if((WB_NET_MUSICAL_LEADER==targetgsid) && (WB_NET_MUSICAL_CHILD==mygsid)){
    return TRUE;
  }
  return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircMatchStart(IRC_BATTLE_MATCH* pWork)
{

  {
    GFLNetInitializeStruct net_ini_data;
    net_ini_data = aGFLNetInit;
    net_ini_data.bNetType = GFL_NET_TYPE_IRC_WIRELESS;
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_SINGLE:
    case EVENTIRCBTL_ENTRYMODE_DOUBLE:
    case EVENTIRCBTL_ENTRYMODE_TRI:
    case EVENTIRCBTL_ENTRYMODE_ROTATE:
      net_ini_data.gsid = WB_NET_IRCBATTLE;
      break;
    case EVENTIRCBTL_ENTRYMODE_MULTH:
      net_ini_data.gsid = WB_NET_IRCBATTLE_MULTI_IRC;
      net_ini_data.maxConnectNum = 4;
      break;
    case EVENTIRCBTL_ENTRYMODE_FRIEND:
      net_ini_data.gsid = WB_NET_IRCFRIEND;
      break;
    case EVENTIRCBTL_ENTRYMODE_TRADE:
      net_ini_data.gsid = WB_NET_IRCTRADE;
      break;
    case EVENTIRCBTL_ENTRYMODE_SUBWAY:
      net_ini_data.gsid = WB_NET_BSUBWAY;
      break;
    case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
      net_ini_data.GsidOverwrite = WB_NET_MUSICAL;
      net_ini_data.gsid = WB_NET_MUSICAL_LEADER;
      net_ini_data.bMPMode = TRUE;
      net_ini_data.maxConnectNum = pWork->pBattleWork->netInitWork->maxConnectNum;         ///< �ő�ڑ��l��
      net_ini_data.maxSendSize = pWork->pBattleWork->netInitWork->maxSendSize;           ///< ���M�T�C�Y
      net_ini_data.maxMPParentSize = pWork->pBattleWork->netInitWork->maxMPParentSize;           ///< ���M�T�C�Y
      pWork->musicalNum = -1;
      break;
    case EVENTIRCBTL_ENTRYMODE_MUSICAL:
      net_ini_data.GsidOverwrite = WB_NET_MUSICAL;
      net_ini_data.gsid = WB_NET_MUSICAL_CHILD;
      net_ini_data.bMPMode = TRUE;
      net_ini_data.maxConnectNum = pWork->pBattleWork->netInitWork->maxConnectNum;         ///< �ő�ڑ��l��
      net_ini_data.maxSendSize = pWork->pBattleWork->netInitWork->maxSendSize;           ///< ���M�T�C�Y
      net_ini_data.maxMPParentSize = pWork->pBattleWork->netInitWork->maxMPParentSize;           ///< ���M�T�C�Y
      pWork->musicalNum = -1;
      break;
    default:
      GF_ASSERT(0);
      break;
    }
    GFL_NET_Init(&net_ini_data, NULL, pWork);	//�ʐM������

    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_MUSICAL:
      GFL_NET_IRCWIRELESS_SetGSIDCallback(_gsidcallback);
      GFL_NET_ReserveNetID_IRCWIRELESS(1);
      GFL_NET_IRCWIRELESS_SetChangeGSID(WB_NET_MUSICAL);
      break;
    case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
      GFL_NET_IRCWIRELESS_SetGSIDCallback(_gsidcallback);
      GFL_NET_ReserveNetID_IRCWIRELESS(0);
      GFL_NET_IRCWIRELESS_SetChangeGSID(WB_NET_MUSICAL);
      break;
    case EVENTIRCBTL_ENTRYMODE_MULTH:
      GFL_NET_IRCWIRELESS_SetChangeGSID(WB_NET_IRCBATTLE_MULTI);
      break;
    }
  }
  _ReturnButtonStart(pWork,FALSE);
  
  _CHANGE_STATE(pWork,_ircInitWait);
}

static void _workEnd(IRC_BATTLE_MATCH* pWork)
{
  GFL_FONTSYS_SetDefaultColor();

  _buttonWindowDelete(pWork);
  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar2));
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_MSG_Delete( pWork->pMsgData );
  GFL_FONT_Delete(pWork->pFontHandle);
  GFL_STR_DeleteBuffer(pWork->pExStrBuf);
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
  APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );

  _graphicEnd(pWork);

}

//------------------------------------------------------------------------------
/**
 * @brief   �h�q�b�ڑ��ҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircInitWait(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_IsInit() == TRUE){	//�������I���҂�
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_MUSICAL:
    case EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER:
      GFL_NET_IRC_SetGSIDCallback(_gsidcallback);
      break;
    }
    GFL_NET_ChangeoverConnect_IRCWIRELESS(_wirelessConnectCallback,_wirelessPreConnectCallback,_ircConnectEndCallback); // ��p�̎����ڑ�
    _CHANGE_STATE(pWork,_ircMatchWait);
  }
}


static void _ircExitWaitNO(IRC_BATTLE_MATCH* pWork)
{
  _buttonWindowDelete(pWork);

//  _firstConnectMessage(pWork);
  {
    int aMsgBuff[1];
    aMsgBuff[0] = pWork->messageBackup;
    _msgWindowCreate(aMsgBuff, pWork);
  }
  
  _ReturnButtonStart(pWork,FALSE);

  _CHANGE_STATE(pWork,_ircMatchWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   �h�q�b�ڑ��ҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircExitWait(IRC_BATTLE_MATCH* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    if(selectno == 0)
    { // �͂���I�������ꍇ
      _buttonWindowDelete(pWork);
      _netBreakOff(pWork);
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
    else
    {  // ��������I�������ꍇ
      _CHANGE_STATE(pWork,_ircExitWaitNO);
    }
    _PaletteFade(pWork,FALSE);
    
  }
}




static void _nop(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask = NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    _CHANGE_STATE(pWork,ircExitStart2);
  }
}


static void _ircstarLoop(IRC_BATTLE_MATCH* pWork)
{
 if(GFL_NET_ForceParentStart_IRCWIRELESS()){  //����
    _CHANGE_STATE(pWork,_nop);
  }
  else if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    _CHANGE_STATE(pWork,ircExitStart2);
  }

}


static void _ircstarStart(IRC_BATTLE_MATCH* pWork)
{
  _ReturnButtonStart(pWork,FALSE);
  _CHANGE_STATE(pWork,_ircstarLoop);
}

//------------------------------------------------------------------------------
/**
 * @brief   �h�q�b�ڑ��ҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircActionWait(IRC_BATTLE_MATCH* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    if(selectno == 0)
    { // �͂���I�������ꍇ
      _buttonWindowDelete(pWork);
      GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
      _CHANGE_STATE(pWork, _ircstarStart);
    }
    else
    {  // ��������I�������ꍇ
      _buttonWindowDelete(pWork);
//      _firstConnectMessage(pWork);

      {
        int aMsgBuff[1];
        aMsgBuff[0] = pWork->messageBackup;
        _msgWindowCreate(aMsgBuff, pWork);
      }
      
      _ReturnButtonStart(pWork,FALSE);
      //pWork->pButton = GFL_BMN_Create( btn_irmain, _BttnCallBack, pWork,  pWork->heapID );
      //pWork->touch = &_cancelButtonCallback;
      _CHANGE_STATE(pWork,_ircMatchWait);
    }
  }
}






//------------------------------------------------------------------------------
/**
 * @brief   �L�[��҂��ďI��
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _ircEndKeyWait(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_UI_TP_GetTrg()){
    _netBreakOff(pWork);
    pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_RETRY;
    _CHANGE_STATE(pWork,_modeFadeoutStart);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �l���I����ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
#define	FLD_YESNO_WIN_PX	( 25 )
#define	FLD_YESNO_WIN_PY	( 13 )

static const BMPWIN_YESNO_DAT _yesNoBmpDatSys2 = {
  GFL_BG_FRAME2_S, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY+6,
  12, 512
  };



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
  IRC_BATTLE_MATCH *pWork = p_work;
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
 * @brief   ���f����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void ircExitStart(IRC_BATTLE_MATCH* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_16};
  int backup = pWork->messageBackup;
    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                  0x20*12, 0x20, pWork->heapID);
  
  _msgWindowCreate(aMsgBuff, pWork);
  pWork->messageBackup = backup;
  
    _YesNoStart(pWork);

    GFL_FONTSYS_SetDefaultColor();

    if(pWork->pButton){
      GFL_BMN_Delete(pWork->pButton);
    }
    pWork->pButton = NULL;

    _CHANGE_STATE(pWork,_ircExitWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlash(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_WIN_IsFinish(pWork->pAppWin)){
    _buttonWindowDelete(pWork);

    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;
    if(pWork->pAppWinLeader){
      APP_TASKMENU_WIN_Delete(pWork->pAppWinLeader);
      pWork->pAppWinLeader = NULL;
    }
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    _CHANGE_STATE(pWork,ircExitStart);
    
  }
}




//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _ircActionCheck(IRC_BATTLE_MATCH* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_37};
  int backup = pWork->messageBackup;
  
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*12, 0x20, pWork->heapID);
  _msgWindowCreate(aMsgBuff, pWork);

  pWork->messageBackup = backup;

  _YesNoStart(pWork);
  
  GFL_FONTSYS_SetDefaultColor();
  
  _CHANGE_STATE(pWork,_ircActionWait);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlashLeader(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_WIN_IsFinish(pWork->pAppWinLeader)){
    int aMsgBuff[]={IRCBTL_STR_37};
    _buttonWindowDelete(pWork);
    APP_TASKMENU_WIN_Delete(pWork->pAppWinLeader);
    pWork->pAppWinLeader = NULL;
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;
    if(pWork->pButton){
      GFL_BMN_Delete(pWork->pButton);
    }
    pWork->pButton = NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );

    _CHANGE_STATE(pWork,_ircActionCheck);
  }
}


static void _waitFinish(IRC_BATTLE_MATCH* pWork)
{
  pWork->timer--;
  if(pWork->timer!=0){
    return;
  }
  _buttonWindowDelete(pWork);
  _netBreakOff(pWork);
  _CHANGE_STATE(pWork,_modeFadeoutStart);

}







static void _ircExitWaitNO2(IRC_BATTLE_MATCH* pWork)
{
  _buttonWindowDelete(pWork);

  {
    int aMsgBuff[1];
    aMsgBuff[0] = IRCBTL_STR_13;  //pWork->messageBackup;
    _msgWindowCreate(aMsgBuff, pWork);
  }
  
  _ReturnButtonStart(pWork,FALSE);

  _CHANGE_STATE(pWork,_nop);

}



//------------------------------------------------------------------------------
/**
 * @brief   �h�q�b�ڑ��ҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircExitWait2(IRC_BATTLE_MATCH* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    if(selectno == 0)
    { // �͂���I�������ꍇ
      _buttonWindowDelete(pWork);
      _netBreakOff(pWork);
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
    else
    {  // ��������I�������ꍇ
      _CHANGE_STATE(pWork,_ircExitWaitNO2);
    }
    _PaletteFade(pWork,FALSE);
    
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ԊO���I���X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------

static void ircExitStart2(IRC_BATTLE_MATCH* pWork)
{
  int aMsgBuff[]={IRCBTL_STR_16};
  int backup = pWork->messageBackup;
    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                  0x20*12, 0x20, pWork->heapID);
  
  _msgWindowCreate(aMsgBuff, pWork);
  pWork->messageBackup = backup;
  
  _YesNoStart(pWork);

  GFL_FONTSYS_SetDefaultColor();

  if(pWork->pButton){
    GFL_BMN_Delete(pWork->pButton);
  }
  pWork->pButton = NULL;

  _CHANGE_STATE(pWork,_ircExitWait2);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlash2(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_WIN_IsFinish(pWork->pAppWin)){
    _buttonWindowDelete(pWork);

    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
    _CHANGE_STATE(pWork,ircExitStart2);
    
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�����Z���{�^���^�b�`�����Q  �~���[�W�J�����[�_�[�p
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _cancelButtonCallback2(int bttnid, IRC_BATTLE_MATCH* pWork)
{
  BOOL ret = FALSE;

  PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
  APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
  _CHANGE_STATE(pWork,_modeAppWinFlash2);        // �I���
  return ret;
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�����Z���{�^���^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _cancelButtonCallback(int bttnid, IRC_BATTLE_MATCH* pWork)
{
  BOOL ret = FALSE;

  if(EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER != pWork->selectType){
    switch( bttnid ){
    case _SELECTMODE_EXIT:
      PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
      APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
      _CHANGE_STATE(pWork,_modeAppWinFlash);        // �I���
      break;
    default:
      break;
    }
  }
  else{
    switch( bttnid ){
    case _SELECTMODE_EXIT:
      PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
      APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
      _CHANGE_STATE(pWork,_modeAppWinFlash);        // �I���
      break;
    case _SELECTMODE_DECIDE:
      if(0!= GFL_NET_GetNowConnectNum_IRCWIRELESS()){
        PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
        APP_TASKMENU_WIN_SetDecide(pWork->pAppWinLeader, TRUE);
        _CHANGE_STATE(pWork,_modeAppWinFlashLeader);
      }
      break;
    default:
      break;
    }
  }
  return ret;
}





static void _modeFlashCallback1(u32 param, fx32 currentFrame )
{
  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;

  if(NANR_ir_ani_CellAnime13==GFL_CLACT_WK_GetAnmSeq(pWork->curIcon[CELL_IRDS1])){
    GFL_CLACT_WK_SetDrawEnable(pWork->curIcon[CELL_IRDS1],FALSE);
  }
  else{
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS1], NANR_ir_ani_CellAnime13);
  }
}
static void _modeFlashCallback2(u32 param, fx32 currentFrame )
{
  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;

  if(NANR_ir_ani_CellAnime12==GFL_CLACT_WK_GetAnmSeq(pWork->curIcon[CELL_IRDS3])){
    GFL_CLACT_WK_SetDrawEnable(pWork->curIcon[CELL_IRDS3],FALSE);
  }
  else{
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS3], NANR_ir_ani_CellAnime12);
  }
}


static void _modeFlashCallback5(u32 param, fx32 currentFrame )
{
  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;
  GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS1], NANR_ir_ani_CellAnime11);
  _CLACT_SetPos(pWork, 200, 38, CELL_IRDS1);
  GFL_CLACT_WK_StopAnmCallBack(pWork->curIcon[CELL_IRDS1]);
  _CLACT_SetAnim(pWork,128,113,CELL_IRWAVE1,NANR_ir_ani_CellAnime6);
}
static void _modeFlashCallback6(u32 param, fx32 currentFrame )
{
  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;
  GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS3], NANR_ir_ani_CellAnime10);
  _CLACT_SetPos(pWork, 57,154, CELL_IRDS3);
  GFL_CLACT_WK_StopAnmCallBack(pWork->curIcon[CELL_IRDS3]);
  _CLACT_SetAnim(pWork,128, 77,CELL_IRWAVE2,NANR_ir_ani_CellAnime7);
}



static void _modeFlashCallback3(u32 param, fx32 currentFrame )
{
  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;
  int seqno = GFL_CLACT_WK_GetAnmSeq(pWork->curIcon[CELL_IRDS1]);

  switch(seqno){
  case NANR_ir_ani_CellAnime11:
    break;
  case NANR_ir_ani_CellAnime8:
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS1], NANR_ir_ani_CellAnime11);
    _CLACT_SetPos(pWork, 200, 38, CELL_IRDS1);
    _CLACT_SetAnim(pWork,128,113,CELL_IRWAVE1,NANR_ir_ani_CellAnime6);
    break;
  default:
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS1], NANR_ir_ani_CellAnime8);
    break;
  }
}
static void _modeFlashCallback4(u32 param, fx32 currentFrame )
{


  IRC_BATTLE_MATCH* pWork = (IRC_BATTLE_MATCH*)param;
  int seqno = GFL_CLACT_WK_GetAnmSeq(pWork->curIcon[CELL_IRDS3]);

  switch(seqno){
  case NANR_ir_ani_CellAnime10:
    break;
  case NANR_ir_ani_CellAnime9:
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS3], NANR_ir_ani_CellAnime10);
    _CLACT_SetPos(pWork, 57,154, CELL_IRDS3);
    _CLACT_SetAnim(pWork,128,77,CELL_IRWAVE2,NANR_ir_ani_CellAnime7);
    break;
  default:
    GFL_CLACT_WK_SetAnmSeq(pWork->curIcon[CELL_IRDS3], NANR_ir_ani_CellAnime9);
    break;
  }

}


static void _returnMessage(IRC_BATTLE_MATCH* pWork)
{

  int aMsgBuff[]={IRCBTL_STR_25};
  _buttonWindowDelete(pWork);
  _msgWindowCreate(aMsgBuff, pWork);
  _CHANGE_STATE(pWork,_ircEndKeyWait);

}



static void _ircFourMatch2TwoMatch(IRC_BATTLE_MATCH* pWork)
{

  _buttonWindowDelete(pWork);
  if(pWork->irccenterflg == TRUE){

    if(pWork->bParent){  //���[�_�[
      int aMsgBuff[]={IRCBTL_STR_12};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else{
      int aMsgBuff[]={IRCBTL_STR_13};
      _msgWindowCreate(aMsgBuff, pWork);
      clactSafeRemove(pWork,CELL_IRDS1);
      clactSafeRemove(pWork,CELL_IRDS3);
    }
    
    clactSafeRemove(pWork,CELL_IRWAVE3);
    clactSafeRemove(pWork,CELL_IRWAVE4);
    clactSafeRemove(pWork,CELL_IRWAVE1);
    clactSafeRemove(pWork,CELL_IRWAVE2);
    clactSafeRemove(pWork,CELL_IRDS2);
    clactSafeRemove(pWork,CELL_IRDS4);

    if(pWork->bParent){
      GFL_CLWK_ANM_CALLBACK cbwk;
      cbwk.callback_type = CLWK_ANM_CALLBACK_TYPE_LAST_FRM ;  // CLWK_ANM_CALLBACK_TYPE
      cbwk.param = (u32)pWork;          // �R�[���o�b�N���[�N
      cbwk.p_func = _modeFlashCallback3; // �R�[���o�b�N�֐�
      GFL_CLACT_WK_StartAnmCallBack( pWork->curIcon[CELL_IRDS1], &cbwk );
      cbwk.p_func = _modeFlashCallback4; // �R�[���o�b�N�֐�
      GFL_CLACT_WK_StartAnmCallBack( pWork->curIcon[CELL_IRDS3], &cbwk );
    }

    pWork->irccenterflg =FALSE;
  }
  pWork->ircCenterAnimCount++;

  if((pWork->ircCenterAnimCount > 8) &&  pWork->bBGBlack){
    pWork->ircCenterAnim=FALSE;
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
  }

}

static int _ansmessagebuff[]={
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_NONE=0,
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_SINGLE,
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_DOUBLE,
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_TRI,
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_ROTATE,
  IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_MULTH,
  IRCBTL_STR_30,//EVENTIRCBTL_ENTRYMODE_TRADE,
  IRCBTL_STR_30,//EVENTIRCBTL_ENTRYMODE_FRIEND,
	IRCBTL_STR_24,//EVENTIRCBTL_ENTRYMODE_COMPATIBLE,
	IRCBTL_STR_24,//EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER,
	IRCBTL_STR_24,//EVENTIRCBTL_ENTRYMODE_MUSICAL,
	IRCBTL_STR_31,//EVENTIRCBTL_ENTRYMODE_SUBWAY,
  IRCBTL_STR_24,//EVENTIRCBTL_ENTRYMODE_EXIT,
  IRCBTL_STR_24,//EVENTIRCBTL_ENTRYMODE_RETRY,
};




static void _ircMatchWait(IRC_BATTLE_MATCH* pWork)
{
  if(pWork->ircCenterAnim && (EVENTIRCBTL_ENTRYMODE_MULTH==pWork->selectType)){ //2vs2//4��̎�2���^�񒆂�
    _ircFourMatch2TwoMatch(pWork);
  }
  else if((pWork->ircmatchanim==TRUE) && (pWork->selectType!=EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER)){  //2��̎����݂�������
    if(pWork->ircmatchflg==TRUE){
      _buttonWindowDelete(pWork);
      _msgWindowCreate(&_ansmessagebuff[pWork->selectType], pWork);
      clactSafeRemove(pWork,CELL_IRWAVE1);
      clactSafeRemove(pWork,CELL_IRWAVE2);
      pWork->ircmatchflg=FALSE;
      if(pWork->curIcon[CELL_IRDS1]){
        GFL_CLACT_WK_SetAutoAnmFlag(pWork->curIcon[CELL_IRDS1],FALSE);
        GFL_CLACT_WK_SetAutoAnmFlag(pWork->curIcon[CELL_IRDS3],FALSE);
      }
      //�A�j���I���R�[���o�b�N�o�^
    }
    _CLACT_AddPos(pWork,0,-2,CELL_IRDS1);
    _CLACT_AddPos(pWork,0, 2,CELL_IRDS3);
    pWork->ircmatchanimCount++;
    if(pWork->ircmatchanimCount > 30){;
      if(pWork->curIcon[CELL_IRDS1]){
        GFL_CLACT_WK_SetDrawEnable(pWork->curIcon[CELL_IRDS1],FALSE);
        GFL_CLACT_WK_SetDrawEnable(pWork->curIcon[CELL_IRDS3],FALSE);
      }
      pWork->ircmatchanim=FALSE;
    }
  }


  if(GFL_NET_IsInit()){
    if(GFL_NET_NEG_TYPE_TYPE_ERROR==GFL_NET_HANDLE_GetNegotiationType(GFL_NET_HANDLE_GetCurrentHandle())){  ///< ���[�h���قȂ�ڑ��G���[
      _CHANGE_STATE(pWork, _returnMessage);
      return;
    }
  }


  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );

    if(pWork->bReturnButton){ //�{�^����̏�
      _CHANGE_STATE(pWork,ircExitStart);
        return;
    }
    else{
      if(selectno == 0){   // �����ނ�I�������ꍇ
        _CHANGE_STATE(pWork,_ircActionCheck);
        return;
      }
      else
      {  // ���ǂ��I�������ꍇ
        _CHANGE_STATE(pWork,ircExitStart);
        return;
      }
    }
    return;
  }

  if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MUSICAL_LEADER){
    if(pWork->musicalNum != GFL_NET_GetNowConnectNum_IRCWIRELESS()){
      _musicalNumWindowCreate(IRCBTL_STR_40, pWork);
      pWork->musicalNum = GFL_NET_GetNowConnectNum_IRCWIRELESS();
    }
    if((pWork->musicalNum > 0) && (pWork->bReturnButton==TRUE)){
      APP_TASKMENU_CloseMenu(pWork->pAppTask);
      pWork->pAppTask=NULL;
      _ReturnButtonStart(pWork,TRUE);
    }
  }

#if 1
#else
 // GFL_BMN_Main( pWork->pButton );
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircPreConnect(IRC_BATTLE_MATCH* pWork)
{
}

//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }

  //GFL_STD_MemCopy(pData, pWork->receive_first_param, size);
  pWork->receive_ok = TRUE;
  OS_TPrintf("�ŏ��̃f�[�^��M�R�[���o�b�N netID = %d\n", netID);
}

//--------------------------------------------------------------
/**
 * @brief   �L�[���R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }

  //GFL_STD_MemCopy(pData, pWork->receive_result_param, size);
  pWork->receive_ok = TRUE;
  OS_TPrintf("�Ō�̃f�[�^��M�R�[���o�b�N netID = %d\n", netID);
}




//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x20000 );

  {
    IRC_BATTLE_MATCH *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_BATTLE_MATCH ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(IRC_BATTLE_MATCH));
    pWork->pBattleWork = pwk;
    pWork->heapID = HEAPID_IRCBATTLE;
    pWork->selectType =  pWork->pBattleWork->selectType; //EVENT_IrcBattleGetType((EVENT_IRCBATTLE_WORK*) pwk);

    pWork->pVramBG = GFL_HEAP_AllocMemory(pWork->heapID, 16*2*16);  //�o�b�N�A�b�v
    _modeInit(pWork);
//    _CHANGE_STATE( pWork, _modeInit);
  }
  GFL_NET_IRCWIRELESS_ResetSystemError();  //�ԊO��WIRLESS�ؒf

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_OBJ,  GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG0, 9, 14);
  
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MATCH* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;
  GAMEDATA* pGameData = pWork->pBattleWork->gamedata;
  StateFunc* state = pWork->state;

  
  pWork->bBGBlack = ircBGAnimMain(&pWork->aIrcBgWork);

  if(state != NULL){
    state(pWork);
    if(pWork->state){
      retCode = GFL_PROC_RES_CONTINUE;
    }
  }
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Update( pWork->pAppWin );
  }
  if(pWork->pAppWinLeader){
    APP_TASKMENU_WIN_Update( pWork->pAppWinLeader );
  }


  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, pWork->yoffset );
  pWork->yoffset--;
  ConnectBGPalAnm_Main(&pWork->cbp);
  GFL_CLACT_SYS_Main();
  PRINTSYS_QUE_Main(pWork->SysMsgQue);


  if(NET_ERR_CHECK_NONE != NetErr_App_CheckError()){
    NetErr_App_ReqErrorDisp();
    pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
    retCode = GFL_PROC_RES_FINISH;
    WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
    WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  }
  
  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MATCH* pWork = mywk;

  if(!WIPE_SYS_EndCheck()){
    return GFL_PROC_RES_CONTINUE;
  }

  if(GFL_NET_IsInit()){
    GFL_NET_DelCommandTable(GFL_NET_CMD_IRCBATTLE);
  }

  if(pWork->pAppTask){
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  }
  if(pWork->pAppWinLeader){
    APP_TASKMENU_WIN_Delete(pWork->pAppWinLeader);
  }
	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
  GFL_HEAP_FreeMemory(pWork->pVramBG);

  pWork->pButton = NULL;
  WORDSET_Delete(pWork->pWordSet);

  _workEnd(pWork);
  GFL_TCB_DeleteTask( pWork->g3dVintr );
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);

  ConnectBGPalAnm_End(&pWork->cbp);
  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);


  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA IrcBattleMatchProcData = {
  IrcBattleMatchProcInit,
  IrcBattleMatchProcMain,
  IrcBattleMatchProcEnd,
};


