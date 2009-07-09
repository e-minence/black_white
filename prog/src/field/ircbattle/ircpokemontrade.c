//=============================================================================
/**
 * @file	  ircpokmeontrade.c
 * @bfief	  �|�P�����������ĒʐM���I������
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================

#include <gflib.h>
#include "arc_def.h"
#include "net/network_define.h"

#include "ircpokemontrade.h"
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
#include "ircbattle.naix"
#include "net_app/connect_anm.h"
#include "../event_ircbattle.h"
#include "net/dwc_rapfriend.h"
#include "savedata/wifilist.h"


#define _TIMING_ENDNO (12)

typedef enum {
    _NETCMD_MYSTATUS = GFL_NET_CMD_IRCBATTLE,
    _NETCMD_FRIENDCODE,
} _BATTLEIRC_SENDCMD;

typedef void (StateFunc)(IRC_POKEMON_TRADE* pState);

struct _IRC_POKEMON_TRADE {
    EVENT_IRCBATTLE_WORK* pParentWork;
    StateFunc* state;      ///< �n���h���̃v���O�������
    int selectType;   // �ڑ��^�C�v
    HEAPID heapID;
//    GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// �E�C���h�E�Ǘ�
//    GFL_MSGDATA *pMsgData;  //
//    WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
//    GFL_FONT* pFontHandle;
//    STRBUF* pStrBuf;
//    BMPWINFRAME_AREAMANAGER_POS aPos;
    int windowNum;
    BOOL IsIrc;
    u32 connect_bit;
    BOOL connect_ok;
    BOOL receive_ok;
    u32 receive_result_param;
    u32 receive_first_param;
    DWCFriendData friendData;
    BOOL bParent;
};



static void _changeState(IRC_POKEMON_TRADE* pWork,StateFunc* state);
static void _changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc* state, int line);
static void _recvFriendCode(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvMystatus(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
    {_recvMystatus,          NULL},  ///_NETCMD_MYSTATUS
    {_recvFriendCode,         NULL},    ///_NETCMD_FRIENDCODE
};



//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(IRC_POKEMON_TRADE* pWork,StateFunc state)
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
static void _changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc state, int line)
{
    NET_PRINT("ircmatch: %d\n",line);
    _changeState(pWork, state);
}
#endif



static void _recvFriendCode(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
	IRC_POKEMON_TRADE *pWork = pWk;

    if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if(netID == GFL_NET_SystemGetCurrentID()){
		return;	//�����̃f�[�^�͖���
	}

    GFL_STD_MemCopy(pData,&pWork->friendData, sizeof(DWCFriendData));
}

static void _recvMystatus(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
	IRC_POKEMON_TRADE *pWork = pWk;

    if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if(netID == GFL_NET_SystemGetCurrentID()){
		return;	//�����̃f�[�^�͖���
	}

    GFL_NET_DWC_FriendDataAdd(IrcBattle_GetSAVE_CONTROL_WORK(pWork->pParentWork),
                              (MYSTATUS*)pData, &pWork->friendData, HEAPID_IRCBATTLE);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�I��������������̂Ńv���Z�X�I��
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendTimingCheck(IRC_POKEMON_TRADE* pWork)
{
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ENDNO)){
        _CHANGE_STATE(pWork,NULL);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I�����������
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendTiming(IRC_POKEMON_TRADE* pWork)
{
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMING_ENDNO);
    _CHANGE_STATE(pWork,_sendTimingCheck);
}

//----------------------------------------------------------------------------
/**
 *	@brief	MyStatus�𑗂�
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendMystatus(IRC_POKEMON_TRADE* pWork)
{
    DWCFriendData friendData;

    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_MYSTATUS,
                     MyStatus_GetWorkSize(),
                     GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(IrcBattle_GetGAMESYS_WORK(pWork->pParentWork))));

    _CHANGE_STATE(pWork,_sendTiming);
}

//----------------------------------------------------------------------------
/**
 *	@brief	FriendData�𑗂�
 *	@param	IRC_POKEMON_TRADE		���[�N
 */
//-----------------------------------------------------------------------------

static void _sendFriendCode(IRC_POKEMON_TRADE* pWork)
{
    DWCFriendData friendData;

    GFL_NET_DWC_GetMySendedFriendCode(
        SaveData_GetWifiListData(IrcBattle_GetSAVE_CONTROL_WORK(pWork->pParentWork)) ,&friendData);
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_FRIENDCODE,
                     sizeof(DWCFriendData),&friendData);


    _CHANGE_STATE(pWork,_sendMystatus);

}

//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x10000 );

    {
        IRC_POKEMON_TRADE *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_POKEMON_TRADE ), HEAPID_IRCBATTLE );
        GFL_STD_MemClear(pWork, sizeof(IRC_POKEMON_TRADE));
        pWork->heapID = HEAPID_IRCBATTLE;
        pWork->selectType =  EVENT_IrcBattleGetType((EVENT_IRCBATTLE_WORK*) pwk);
        pWork->pParentWork = pwk;
        // �ʐM�e�[�u���ǉ�
        GFL_NET_AddCommandTable(GFL_NET_CMD_IRCBATTLE,_PacketTbl,NELEMS(_PacketTbl), pWork);
        _CHANGE_STATE( pWork, _sendFriendCode);
    }
    return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
    IRC_POKEMON_TRADE* pWork = mywk;
    GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

    StateFunc* state = pWork->state;
    if(state != NULL){
        state(pWork);
        retCode = GFL_PROC_RES_CONTINUE;
    }
//	ConnectBGPalAnm_Main(&pWork->cbp);

    return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleFriendProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
    IRC_POKEMON_TRADE* pWork = mywk;
    EVENT_IRCBATTLE_WORK* pParentWork = pwk;

   // _workEnd(pWork);

//	ConnectBGPalAnm_End(&pWork->cbp);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);

    EVENT_IrcBattle_SetEnd(pParentWork);


    return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA IrcPokemonTradeProcData = {
	IrcBattleFriendProcInit,
	IrcBattleFriendProcMain,
	IrcBattleFriendProcEnd,
};


