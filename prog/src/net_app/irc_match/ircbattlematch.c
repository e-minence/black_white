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
#include "cg_comm.naix"
#include "net_app/connect_anm.h"
//#include "../../field/event_ircbattle.h"
#include "ir_ani_NANR_LBLDEFS.h"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "net_app/irc_match.h"

#include "ircbattlematch.cdat"


enum _IBMODE_SELECT {
  _SELECTMODE_EXIT,
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

#define _FULL_TIMER   (120)  //�l���������ς��Ō����ł��Ȃ��ꍇ�̃��b�Z�[�W�\������


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
static BOOL _cancelButtonCallback(int bttnid,IRC_BATTLE_MATCH* pWork);
static void _RecvMyStatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvPokePartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static u8* _getPokePartyBuff(int netID, void* pWork, int size);
static void _firstConnectMessage(IRC_BATTLE_MATCH* pWork);


///�ʐM�R�}���h
typedef enum {
  _NETCMD_TYPESEND = GFL_NET_CMD_IRCBATTLE,
  _NETCMD_MYSTATUSSEND,
  _NETCMD_POKEPARTY_SEND,
} _BATTLEIRC_SENDCMD;


#define _TIMINGNO_CS (120)
#define _TIMINGNO_DS (121)
#define _TIMINGNO_POKEP (122)

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
static BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}



///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_RecvModeCheckData,          NULL},  ///_NETCMD_TYPESEND
  {_RecvMyStatusData,          NULL},  ///_NETCMD_MYSTATUSSEND
  {_RecvPokePartyData,       _getPokePartyBuff},  ///_NETCMD_POKEPARTY_SEND
};

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
  FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
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
  IRC_MATCH_WORK* pBattleWork;
  StateFunc* state;      ///< �n���h���̃v���O�������
  int selectType;   // �ڑ��^�C�v
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

  BOOL ircCenterAnim;
  int ircCenterAnimCount;
  int yoffset;
  BOOL SecondStep;

  GFL_BUTTON_MAN* pButton;
  TouchFunc* touch;
  int timer;
  
  PRINT_QUE*            SysMsgQue;
  APP_TASKMENU_WORK* pAppTask;
	APP_TASKMENU_RES* pAppTaskRes;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_WIN_WORK* pAppWin;

};



enum{
  _START_TIMING=12,
};




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

static void _changeState(IRC_BATTLE_MATCH* pWork,StateFunc state)
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
static void _changeStateDebug(IRC_BATTLE_MATCH* pWork,StateFunc state, int line)
{
  NET_PRINT("ircmatch: %d\n",line);
  _changeState(pWork, state);
}
#endif

static void _endCallBack(void* pWork)
{
  IRC_BATTLE_MATCH *commsys = pWork;

  OS_TPrintf("endCallBack�I��\n");
  commsys->connect_ok = FALSE;
  commsys->connect_bit = 0;
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
    _CHANGE_STATE(pWork, NULL);        // �I���
  }
}

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
      p_handle ,NARC_ircbattle_ir_demo_NCLR , CLSYS_DRAW_MAIN, 0, 0, 3, pWork->heapID  );
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
    cellInitData.bgpri = 3;
    pWork->curIcon[no] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->cellRes[CHAR_DS],
                                              pWork->cellRes[PLT_DS],
                                              pWork->cellRes[ANM_DS],
                                              &cellInitData ,CLSYS_DRAW_MAIN , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[no] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[no], TRUE );
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
 * @brief   �ړ��R�}���h��M�֐�
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

  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }
  if(pWork->selectType > pType[0] ){
    NET_PRINT("BattleChange %d ->%d\n",pWork->selectType, pType[0]);
    pWork->pBattleWork->selectType = pType[0];
    //pWork->selectType = pType[0];
  }
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



static void _modeCheckStart5(IRC_BATTLE_MATCH* pWork)
{
 if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP, WB_NET_IRCBATTLE)){
   _CHANGE_STATE(pWork,_modeFadeoutStart);
  }
}


static void _modeCheckStart4(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS, WB_NET_IRCBATTLE)){
    if(GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, _NETCMD_POKEPARTY_SEND,
                        PokeParty_GetWorkSize(), pWork->pBattleWork->pParty,
                          FALSE,FALSE,TRUE)){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP,WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart5);
    }
  }
}


static void _modeCheckStart3(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_MYSTATUSSEND,
                      MyStatus_GetWorkSize(), GAMEDATA_GetMyStatus(pWork->pBattleWork->gamedata))){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS,WB_NET_IRCBATTLE);
    _CHANGE_STATE(pWork,_modeCheckStart4);
  }
}

static void _modeCheckStart31(IRC_BATTLE_MATCH* pWork)
{
  switch(pWork->selectType){
  case EVENTIRCBTL_ENTRYMODE_SINGLE:
  case EVENTIRCBTL_ENTRYMODE_DOUBLE:
  case EVENTIRCBTL_ENTRYMODE_TRI:
  case EVENTIRCBTL_ENTRYMODE_ROTATE:
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_TYPESEND,
                        sizeof(pWork->selectType), &pWork->selectType)){
      _CHANGE_STATE(pWork,_modeCheckStart3);
    }
    break;
  default:
    _CHANGE_STATE(pWork,_modeCheckStart3);
    break;
  }
}

static void _modeCheckStart2(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_CS,WB_NET_IRCBATTLE)){
    _CHANGE_STATE(pWork,_modeCheckStart31);
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

  _CHANGE_STATE(pWork, _modeCheckStart);
}


static void _wirelessPreConnectCallback(void* pWk,BOOL bParent)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  pWork->bParent = bParent;
  pWork->irccenterflg=TRUE;
  pWork->ircCenterAnim=TRUE;


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
  appinit.charPosY = 14;
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
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , -8 );
}


static void _ReturnButtonStart(IRC_BATTLE_MATCH* pWork)
{
  int i;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_03, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_RETURN;
  pWork->pAppWin =APP_TASKMENU_WIN_Create( pWork->pAppTaskRes,
                                           pWork->appitem, 32-10, 24-4, 10, pWork->heapID);

  
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);

}



//------------------------------------------------------------------------------
/**
 * @brief   �󂯎�������̃E�C���h�E�𓙊Ԋu�ɍ�� ����3char
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


  pWork->buttonWin[i] = GFL_BMPWIN_Create(
    frame,
    1, 1, _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
  BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );

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

  pWork->buttonWin[i] = GFL_BMPWIN_Create(
    frame,
    2, 8, 16, 4,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
  BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );

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

    if(EVENTIRCBTL_ENTRYMODE_MULTH==pWork->selectType){ //1vs1
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo1_NSCR,
                                                GFL_BG_FRAME1_M, 0,
                                                GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                                pWork->heapID);
    }

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo2_NSCR,
                                              GFL_BG_FRAME0_M, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                              pWork->heapID);
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
    int keisu=4;

    _CLACT_SetAnim(pWork,88,72,CELL_IRDS2,NANR_ir_ani_CellAnime3);  //��
    _CLACT_SetAnim(pWork,88+keisu,72+keisu,CELL_IRWAVE1,NANR_ir_ani_CellAnime0);


    _CLACT_SetAnim(pWork,168,      110,CELL_IRDS1,NANR_ir_ani_CellAnime2); //�E
    _CLACT_SetAnim(pWork,168-keisu,110-keisu,CELL_IRWAVE2,NANR_ir_ani_CellAnime1);

  }
  else{ //2vs2
    int keisu=4;
    int kei2=16;

    _CLACT_SetAnim(pWork, kei2+ 88,        -kei2+72,CELL_IRDS2,NANR_ir_ani_CellAnime3);  //��
    _CLACT_SetAnim(pWork, kei2+ 88+keisu,  -kei2+72+keisu,CELL_IRWAVE2,NANR_ir_ani_CellAnime0);
    // _CLACT_SetAnim(pWork, -kei2+ 88,        kei2+72,CELL_IRDS4,NANR_ir_ani_CellAnime3);  //��
    _CLACT_SetAnim(pWork, -kei2+ 88+keisu,  kei2+72+keisu,CELL_IRWAVE3,NANR_ir_ani_CellAnime0);

    //   _CLACT_SetAnim(pWork, kei2+ 168,       -kei2+   110,CELL_IRDS3,NANR_ir_ani_CellAnime2); //�E
    _CLACT_SetAnim(pWork, kei2+ 168-keisu, -kei2+   110-keisu,CELL_IRWAVE4,NANR_ir_ani_CellAnime1);
    _CLACT_SetAnim(pWork,-kei2+168,         kei2+   110,CELL_IRDS1,NANR_ir_ani_CellAnime2); //�E
    _CLACT_SetAnim(pWork,-kei2+168-keisu,   kei2+   110-keisu,CELL_IRWAVE1,NANR_ir_ani_CellAnime1);
  }


  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2 );


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
    _CHANGE_STATE(pWork,_ircMatchStart);
  }

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
      net_ini_data.gsid = WB_NET_IRCBATTLE_MULTI;
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
    default:
//      GF_ASSERT(0);
      break;
    }
    if(pWork->selectType!=EVENTIRCBTL_ENTRYMODE_MUSICAL){
      GFL_NET_Init(&net_ini_data, NULL, pWork);	//�ʐM������
    }
  }
  _ReturnButtonStart(pWork);
  
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
    GFL_NET_ChangeoverConnect_IRCWIRELESS(_wirelessConnectCallback,_wirelessPreConnectCallback,_ircConnectEndCallback); // ��p�̎����ڑ�


    pWork->pButton = GFL_BMN_Create( btn_irmain, _BttnCallBack, pWork,  pWork->heapID );
    pWork->touch = &_cancelButtonCallback;


    _CHANGE_STATE(pWork,_ircMatchWait);
  }
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
    if(selectno == 0)
    { // �͂���I�������ꍇ
      pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
//      EVENT_IrcBattleSetType(pWork->pBattleWork,EVENTIRCBTL_ENTRYMODE_EXIT);
      _buttonWindowDelete(pWork);
      GFL_NET_Exit(NULL);
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
    else
    {  // ��������I�������ꍇ
      _buttonWindowDelete(pWork);

      _firstConnectMessage(pWork);

      _ReturnButtonStart(pWork);
      pWork->pButton = GFL_BMN_Create( btn_irmain, _BttnCallBack, pWork,  pWork->heapID );
      pWork->touch = &_cancelButtonCallback;

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
  if(GFL_UI_KEY_GetTrg() != 0){


    pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_RETRY;
//    EVENT_IrcBattleSetType(pWork->pBattleWork,EVENTIRCBTL_ENTRYMODE_RETRY);
    GFL_NET_Exit(NULL);
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
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlash(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_WIN_IsFinish(pWork->pAppWin)){
    int aMsgBuff[]={IRCBTL_STR_16};
    _buttonWindowDelete(pWork);

    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;

    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                  0x20*12, 0x20, pWork->heapID);
    _msgWindowCreate(aMsgBuff, pWork);

    _YesNoStart(pWork);

    GFL_FONTSYS_SetDefaultColor();

    if(pWork->pButton){
      GFL_BMN_Delete(pWork->pButton);
    }
    pWork->pButton = NULL;

    _CHANGE_STATE(pWork,_ircExitWait);
  }
}


static void _waitFinish(IRC_BATTLE_MATCH* pWork)
{
  pWork->timer--;
  if(pWork->timer!=0){
    return;
  }
  _buttonWindowDelete(pWork);
  GFL_NET_Exit(NULL);
  _CHANGE_STATE(pWork,_modeFadeoutStart);

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

//  pWork->bttnid = bttnid;
  switch( bttnid ){
  case _SELECTMODE_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
//  pWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
    _CHANGE_STATE(pWork,_modeAppWinFlash);        // �I���
    break;
  default:
    break;
  }
  return ret;
}





static void _ircMatchWait(IRC_BATTLE_MATCH* pWork)
{
  int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
  if(num1==WIFILIST_FRIEND_MAX){
    _CHANGE_STATE(pWork,_waitFinish);        // �I���()
    pWork->timer = _FULL_TIMER;
    return;
  }

  
  if(pWork->ircCenterAnim){  //4��̎�2���^�񒆂�

    _buttonWindowDelete(pWork);
    if(pWork->irccenterflg == TRUE){

      if(pWork->bParent){  //���[�_�[
        int aMsgBuff[]={IRCBTL_STR_12};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_13};
        _msgWindowCreate(aMsgBuff, pWork);
      }

      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE3]);
      pWork->curIcon[CELL_IRWAVE3]=NULL;
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE4]);
      pWork->curIcon[CELL_IRWAVE4]=NULL;
      pWork->irccenterflg =FALSE;
    }


    pWork->ircCenterAnimCount++;

    _CLACT_AddPos(pWork, -2,+2,CELL_IRDS2);
    _CLACT_AddPos(pWork, 2,-2,CELL_IRDS1);
    _CLACT_AddPos(pWork, -2,+2,CELL_IRWAVE2);
    _CLACT_AddPos(pWork, 2,-2,CELL_IRWAVE1);

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BD , 16-(pWork->ircCenterAnimCount*2), 16);

    if(pWork->ircCenterAnimCount>8){
      pWork->ircCenterAnim=FALSE;
      GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
    }



  }
  else if(pWork->ircmatchanim==TRUE){  //2��̎����݂�������
    if(pWork->ircmatchflg==TRUE){
      _buttonWindowDelete(pWork);
      if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_FRIEND || pWork->selectType==EVENTIRCBTL_ENTRYMODE_TRADE)
      {
        int aMsgBuff[]={IRCBTL_STR_30};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_31};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE1]);
      pWork->curIcon[CELL_IRWAVE1]=NULL;
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE2]);
      pWork->curIcon[CELL_IRWAVE2]=NULL;
      pWork->ircmatchflg=FALSE;
    }
    pWork->ircmatchanimCount++;
    //if(EVENTIRCBTL_ENTRYMODE_MULTH!=pWork->selectType){ //1vs1
    _CLACT_AddPos(pWork, 2, 1,CELL_IRDS1);
    _CLACT_AddPos(pWork,-2,-1,CELL_IRDS2);
    //    }
    if(pWork->ircmatchanimCount > 25){;
      pWork->ircmatchanim=FALSE;
    }
  }


  if(GFL_NET_IsInit()){
    if(GFL_NET_NEG_TYPE_TYPE_ERROR==GFL_NET_HANDLE_GetNegotiationType(GFL_NET_HANDLE_GetCurrentHandle())){  ///< ���[�h���قȂ�ڑ��G���[
      int aMsgBuff[]={IRCBTL_STR_25};
      _buttonWindowDelete(pWork);
      _msgWindowCreate(aMsgBuff, pWork);
      _CHANGE_STATE(pWork,_ircEndKeyWait);
      return;
    }
  }


  GFL_BMN_Main( pWork->pButton );

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

    _CHANGE_STATE( pWork, _modeInit);
  }
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

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Update( pWork->pAppWin );
  }
  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, pWork->yoffset );
  pWork->yoffset--;
  ConnectBGPalAnm_Main(&pWork->cbp);
  GFL_CLACT_SYS_Main();
  PRINTSYS_QUE_Main(pWork->SysMsgQue);

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


  if(pWork->pAppTask){
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  }
	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
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


