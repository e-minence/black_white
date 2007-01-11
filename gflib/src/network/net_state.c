//=============================================================================
/**
 * @file	comm_state.c
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 *          �t�B�[���h�Ǘ������Ȃ�������������񂱂��Ɉ��z��...2006.01.12
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#include "gflib.h"

#include "device/wh_config.h"

#include "net.h"
#include "net_def.h"
#include "device/net_wireless.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"


/*
#include "common.h"
#include "communication/communication.h"
#include "wh.h"
#include "comm_local.h"
#include "system/gamedata.h"
#include "system/pm_str.h"

#include "wifi/dwc_rap.h"   //WIFI
#include "system/snd_tool.h"  //sndTOOL

#include  "communication/wm_icon.h"
*/
//==============================================================================
//	�^�錾
//==============================================================================
// �R�[���o�b�N�֐��̏���
//typedef void (*PTRStateFunc)(void);

//==============================================================================
// ���[�N
//==============================================================================




struct _NET_PARENTSYS_t{
    u8 negoCheck[GFL_NET_MACHINE_MAX];     ///< �e�q�@�̃l�S�V�G�[�V�������
};


#if 0 //�g��Ȃ�
typedef struct{
    void* pWifiFriendStatus;
    u8 select[6];
    MATHRandContext32 sRand; ///< �e�q�@�l�S�V�G�[�V�����p�����L�[
    PTRStateFunc state;
    const REGULATION* pReg;
    int errorCode;
    u16 reConnectTime;  // �Đڑ����Ɏg�p����^�C�}�[
    u16 timer;
#ifdef GFL_NET_DEBUG		// Debug ROM
    u16 debugTimer;
#endif
    u8 limitNum;      // ��t�����������ꍇ��LIMIT��
    u8 negotiation;   // �ڑ��F��
    u8 connectIndex;   // �q�@���ڑ�����e�@��index�ԍ�
    u8 serviceNo;      // �ʐM�T�[�r�X�ԍ�
    u8 regulationNo;   // �R���e�X�g��ʂ����߂�ԍ�
#ifdef GFL_NET_DEBUG		// Debug ROM
    u8 soloDebugNo;
    u8 bDebugStart;
#endif
    u8 disconnectIndex;  //wifi�ؒf���� �e�@�q�@
    u8 wifiTargetNo;   // WIFI�ڑ����s���l�̔ԍ�
    u8 bFirstParent;   // �J��Ԃ��e�q�؂�ւ����s���ꍇ�̍ŏ��̐e���
    u8 bDisconnectError; // �ؒf�G���[���G���[�ɂ���Ƃ�TRUE
    u8 bErrorAuto;     // �����G���[�����N��TRUE
    u8 bWorldWifi;     // DPW�������Ă���ꍇ
    u8 ResetStateType;    // �G���[��Ԃɓ���\�t�g�E�G�A���Z�b�g��҂��
    u8 bUnionPause;    // ���j�I�����[���Őڑ���h�������ꍇ
    u8 partyGameBit;   // ���Ђ�GGID���E��
    u8 bParentOnly;   // �e�@��Ԃɂ����Ȃ�Ȃ�
    u8 bChildOnly;   // �q�@��Ԃɂ����Ȃ�Ȃ�
    u8 bNotConnect;   // �ʐM��ԂɑJ�ڂ��Ȃ�
    u8 bWifiDisconnect; //WIFI�ؒf�p �R�}���h�󂯎�����ꍇ��1
    u8 stateError;         //�G���[�����ɂ���ꍇ1�ȏ�
    u8 bPauseFlg;
} _COMM_STATE_WORK;
#endif
//static _COMM_STATE_WORK* _pCommState = NULL;  ///<�@���[�N�\���̂̃|�C���^

//==============================================================================
// ��`
//==============================================================================

#define _HEAPSIZE_BATTLE           (0x7080)  // �o�g���@�̈�
#define _HEAPSIZE_UNDERGROUND      (0xE000)  // �n���̈�
#define _HEAPSIZE_UNION            (0x7080)  // ���j�I�����[���̈�
#define _HEAPSIZE_POKETCH          (0x7000)  // �ۂ������@�̈�
#define _HEAPSIZE_PARTY            (0x7000)  // �p�[�e�B�[�T�[�`
#define _HEAPSIZE_WIFI            (0x2A000+0x7000)  //DWCRAP ���g�p����̈�
#define _HEAPSIZE_DPW              (0x100)   // ���E�����@�^���[�p

#define _PACKETSIZE_BATTLE         (512)//(1376)  // �o�g���@�̈�
#define _PACKETSIZE_UNDERGROUND     (500)  // �n���̈�
#define _PACKETSIZE_UNION          (512)//(1280)  // ���j�I�����[���̈�
#define _PACKETSIZE_POKETCH          (32)  // �ۂ������̈�
#define _PACKETSIZE_PARTY         (32)  // �ۂ������̈�


#define _START_TIME (50)     // �J�n����
#define _CHILD_P_SEARCH_TIME (32) ///�q�@�Ƃ��Đe��T������
#define _PARENT_WAIT_TIME (40) ///�e�Ƃ��Ă̂�т�҂���
#define _FINALIZE_TIME (2)
#define _EXIT_SENDING_TIME (5)
#define _EXIT_SENDING_TIME2 (15)
#define _PARENT_END_TIME (2)
#define _SEND_NAME_TIME (10)
#define _PARENTSCAN_PA (3)  // �e�@�Ƃ��Č�������m����1/3

#define _RETRY_COUNT_UNION  (3)  // ���j�I�����[���Ŏq�@���ڑ��ɗv�����

#define _TCB_COMMCHECK_PRT   (10)    ///< �t�B�[���h������ʐM�̊Ď����[�`����PRI




//==============================================================================
// static�錾
//==============================================================================

// �X�e�[�g�̏�����

static void _changeStateDebug(GFL_NETHANDLE* pHandle, PTRStateFunc state, int time, int line);  // �X�e�[�g��ύX����
static void _changeState(GFL_NETHANDLE* pHandle, PTRStateFunc state, int time);  // �X�e�[�g��ύX����

#ifdef GFL_NET_DEBUG
#if 1
#define   _CHANGE_STATE(state, time)  _changeStateDebug(pNetHandle ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(pNetHandle ,state, time)
#endif
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state, time)  _changeState(pNetHandle ,state, time)
#endif //GFL_NET_DEBUG


// �n���֘A�X�e�[�g
static void _underStart(void);           // �q�@�̏����� + �V�X�e���̏�����
static void _underChildPInit(void);      // �q�@�̏�����
static void _underChildFInit(void);
static void _underChildPSearching(void); // �q�@���e�@��T���Ă���
static void _underChildForceConnect(void); // �q�@���e�@�ɋ����ɐڑ���
static void _underChildConnecting(void);   // �q�@���e�@�ɐڑ���
static void _underChildConnect(void);    // �q�@�Ƃ��Đڑ���
static void _underChildReset(void);
static void _underChildFinalize(void);   // �q�@�ɂȂ�Ȃ������̂ŏI��������
static void _underParentFinalize(void);  // �e�@�ɂȂ�Ȃ������̂ŏI��������
static void _underParentInit(void);      // �e�@�Ƃ��ď�����
static void _underParentWait(void);      // �e�@�Ƃ��đҋ@���
static void _underParentConnectInit(void); // �ڑ������̂ŏ�������
static void _underParentConnect(void);   // �e�@�Ƃ��Đڑ���
static void _stateUnderGroundConnectEnd(void);  // �n���ؒf
static void _underSBReset(void);


// �o�g���֘A�X�e�[�g
static void _battleParentInit(void);     // �퓬�p�e�@�Ƃ��ď�����
static void _battleParentWaiting(void);  // �퓬�p�e�@�Ƃ��đҋ@��
static void _battleParentMoveRoomEnter(void);  // �퓬�O�̕����ɓ����đҋ@��
static void _battleParentMoveRoom(void);  // �퓬�O�̕����ɓ����đҋ@��
static void _battleParentSendName(void);  // �����̖��O���݂�Ȃɑ��M
static void _battleParentReTry(void);   // �퓬�p�e�@�𒆒f

static void _battleChildInit(void);     // �퓬�p�q�@�Ƃ��ď�����
static void _battleChildBconScanning(void);  // �퓬�p�q�@�Ƃ��Đe�@�I��
static void _battleChildConnecting(void);  // �ڑ������炢��
static void _battleChildSendName(void);  // �����̖��O���݂�Ȃɑ��M
static void _battleChildWaiting(void);  // �_�C���N�g�p�q�@�ҋ@��
static void _battleChildMoveRoomEnter(void); // �퓬�O�̕����ɓ����đҋ@��
static void _battleChildMoveRoom(void); // �퓬�O�̕����ɓ����đҋ@��
static void _battleChildReTry(void);   // �q�@�𒆒f
static void _battleChildReInit(void);   // �q�@���ċN��
static void _battleChildReset(void);
static void _battleChildReConnect(void);
static void _underChildOnline(void);


// UNION���[���֘A�X�e�[�g
static void _unionStart(void);
static void _unionChildSearching(void);
static void _unionChildFinalize(void);
static void _unionParentInit(void);
static void _unionParentWait(void);
static void _unionChildRestart(void);
static void _unionChildNegotiation(void);

static void _unionForceConnectStart(void);
static void _unionForceConnectStart2(void);
static void _unionForceConnect(void);
static void _unionChildConnecting(void);
static void _unionChildConnectSuccess(void);
static void _unionChildConnectFailed(void);
static void _unionChildReset(void);
static void _unionParentConnect(void);
static void _unionParentPause(void);

static void _mysteryParentInit(void);
static void _mysteryChildInit(void);

// ���̑���ʓI�ȃX�e�[�g
static void _stateNone(void);            // �������Ȃ�
static void _stateConnectError(void);    // �ڑ��G���[���
static void _stateEnd(void);             // �I������
static void _stateConnectChildEndWait(void);   // �q�@�̏I����҂��ďI���
static void _stateConnectChildEnd(void);
static void _stateConnectEnd(void);      // �ؒf�����J�n
static void _stateConnectAutoEnd(void);  // �����ؒf�����J�n


// WIFI�p
static void _wifiBattleLogin(void);
static void _stateWifiLogout(void);


// �l�S�V�G�[�V�����p�m�FKEY
static u8 _negotiationMsg[]={"FREAK"};
static u8 _negotiationMsgReturnOK[]={" GAME"};
static u8 _negotiationMsgReturnNG[]={" FULL"};


//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(GFL_NETHANDLE* pHandle,PTRStateFunc state, int time)
{
    pHandle->state = state;
    pHandle->timer = time;
}

//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(GFL_NETHANDLE* pHandle,PTRStateFunc state, int time, int line)
{
    NET_PRINT("net_state: %d\n",line);
    _changeState(pHandle, state, time);
}
#endif

//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̏���������
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

static void _commStateInitialize(GFL_NETHANDLE* pNetHandle,int serviceNo)
{
    void* pWork;

    if(pNetHandle!=NULL){   // ���łɓ��쒆�̏ꍇ�K�v�Ȃ�
        return;
    }
//    CommVRAMDInitialize();
    // ������

    pNetHandle->timer = _START_TIME;
    pNetHandle->bFirstParent = TRUE;  // �e�̏��߂Ă̋N���̏ꍇTRUE
    pNetHandle->limitNum = 2;         // ��l�͍Œ�ł��ڑ��\
    pNetHandle->negotiation = _NEGOTIATION_CHECK;
    pNetHandle->serviceNo = serviceNo;

    //CommRandSeedInitialize(&_pCommState->sRand);  //@@OO����������������
    GFL_NET_CommandInitialize(NULL, 0, NULL);
/*
    if((serviceNo != COMM_MODE_UNION) && (serviceNo != COMM_MODE_PARTY) &&
       (serviceNo != COMM_MODE_MYSTERY)){
        WirelessIconEasy();  // �A�C�R��������
    }
*/
}


//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̏I������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateFinalize(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle==NULL){  // ���łɏI�����Ă���
        return;
    }
    GFL_NET_CommandFinalize();
//    WirelessIconEasyEnd();   //@@OO ��œ����\��
    GFL_NET_WLVRAMDFinalize();
    pNetHandle->state = NULL;
}

//==============================================================================
/**
 * �����Ă��邩�ǂ���
 * @param   none
 * @retval  �����Ă���ꍇTRUE
 */
//==============================================================================

static BOOL _stateIsMove(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle->state){
        return TRUE;
    }
    return FALSE;
}



static void _handleDelete(GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_DeleteNetHandle(pNetHandle);
}


//==============================================================================
/**
 * �C�N�j���[������������ʐM�N��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _deviceInitialize(GFL_NETHANDLE* pNetHandle)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
    GFL_NETWL* pWL;
    
    if(!GFL_NET_WLIsVRAMDInitialize()){
        return;  //
    }

    NET_PRINT("%x - %x %x\n",(u32)pNetIni,(u32)pNetIni->beaconGetFunc,(u32)pNetIni->beaconGetSizeFunc);
    
    pWL = GFL_NET_WLGetHandle(pNetIni->allocNo, pNetIni->gsid, pNetIni->maxConnectNum);
    _GFL_NET_SetNETWL(pWL);

    GFL_NET_WLInitialize(pNetIni->allocNo, pNetIni->beaconGetFunc, pNetIni->beaconGetSizeFunc);
//    GFL_NET_WLStealth(FALSE);
    GFL_NET_SystemReset();         // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���

    NET_PRINT("�ċN��    -- \n");
    _CHANGE_STATE(_handleDelete, 0);
}


//==============================================================================
/**
 * @brief   �ʐM�f�o�C�X������������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateDeviceInitialize(GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_WLVRAMDInitialize();
    GFL_NET_CommandInitialize(NULL, 0, NULL);

    _commStateInitialize(pNetHandle, 0);
    _CHANGE_STATE(_deviceInitialize, 0);
}

//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�ɏ��𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childSendNego(GFL_NETHANDLE* pNetHandle)
{

    if(GFL_NET_SystemIsError()){
        //NET_PRINT("�G���[�̏ꍇ�߂�\n");
     //   _CHANGE_STATE(_battleChildReset, 0);
    }
  //  if(CommIsConnect(CommGetCurrentID()) && ( COMM_PARENT_ID != CommGetCurrentID())){
   //     _CHANGE_STATE(_battleChildWaiting, 0);
   // }
}

//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�ɋ����炢��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childConnecting(GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_WLParentBconCheck();

    if(GFL_NET_WLChildMacAddressConnect(pNetHandle->aMacAddress)){

        _CHANGE_STATE(_childSendNego, _SEND_NAME_TIME);
    }

}

//==============================================================================
/**
 * �}�b�N�A�h���X���w�肵�Ďq�@�ڑ��J�n
 * @param   connectIndex �ڑ�����e�@��Index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectMacAddress(GFL_NETHANDLE* pNetHandle)
{

    GFL_NET_SystemChildModeInit(TRUE,TRUE,512);

    _CHANGE_STATE(_childConnecting, 0);
}

//==============================================================================
/**
 * �ʐM�Ǘ��X�e�[�g�̏���
 * @param
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateMainProc(GFL_NETHANDLE* pHandle)
{
    if(pHandle){
        PTRStateFunc state = GFL_NET_GetStateFunc(pHandle);
        if(state != NULL){
            state(pHandle);
        }
    }
}

//==============================================================================
/**
 * �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentWait(GFL_NETHANDLE* pHandle)
{
}

//==============================================================================
/**
 * �e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentInit(GFL_NETHANDLE* pNetHandle)
{
    if(!GFL_NET_WLIsVRAMDInitialize()){
        return;
    }

    if(GFL_NET_SystemParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        GFL_NET_SystemSetTransmissonTypeDS();
        _CHANGE_STATE(_parentWait, 0);
    }
}


//==============================================================================
/**
 * �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectParent(GFL_NETHANDLE* pNetHandle,int heapID)
{
    pNetHandle->pParent = GFL_HEAP_AllocMemory(heapID, sizeof(NET_PARENTSYS));
    GFL_STD_MemClear(pNetHandle->pParent, sizeof(NET_PARENTSYS));

    _CHANGE_STATE(_parentInit, 0);
}


//==============================================================================
/**
 * WIFI�I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� CS_WIFI_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    NET_PRINT("EXIT�R�}���h��M\n");
#if 0
    if(CommGetCurrentID() == COMM_PARENT_ID){
        _pCommState->disconnectIndex = 0;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    else{
        _pCommState->disconnectIndex = 1;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    _pCommState->bWifiDisconnect = TRUE;
#endif
}

//==============================================================================
/**
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
//�e�@���󂯎��킯�Ȃ̂Ńt���O�𗧂Ă�̂�����
    u8 retCmd = FALSE;

    if(pNetHandle->pParent->negoCheck[netID] == FALSE){
        pNetHandle->pParent->negoCheck[netID] = TRUE;
        retCmd = TRUE;
    }
    OS_TPrintf("------NegoRet �𑗐M %d\n",retCmd);
    GFL_NET_SendData(pNetHandle, GFL_NET_CMD_COMM_NEGOTIATION_RETURN, &retCmd);
    
#if 0
    int i;
    u8* pMsg = pData;
    BOOL bMatch = TRUE;

    NET_PRINT("------CommRecvNegotiation\n");

    if(CommGetCurrentID() != COMM_PARENT_ID){  // �e�@�̂ݔ��f�\
        return;
    }
    bMatch = TRUE;
    for(i = 0; i < sizeof(_negotiationMsg); i++){
        if(pMsg[i] != _negotiationMsg[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch  && (!_pCommState->bUnionPause)){   // �q�@����ڑ��m�F������
//        if(CommGetConnectNum() <= _pCommState->limitNum){  // �w��ڑ��l����艺��邱��
            NET_PRINT("------�����𑗐M \n");
            _negotiationMsgReturnOK[0] = netID;
            CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnOK);
            return;
//        }
    }
    NET_PRINT("------���s�𑗐M %d %d\n",bMatch,_pCommState->bUnionPause);
    _negotiationMsgReturnNG[0] = netID;
    CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnNG);
#endif
}

//==============================================================================
/**
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pMsg = pData;

    if(pMsg[0]==1){   // �e�@����ڑ��F�؂�����
        pNetHandle->negotiation = _NEGOTIATION_OK;
    }
    else{
        pNetHandle->negotiation = _NEGOTIATION_NG;
    }
}


#if 0   //state�C����

//==============================================================================
/**
 * �o�g�����̎q�Ƃ��Ă̒ʐM�����J�n
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

#ifdef GFL_NET_DEBUG
void GFL_NET_StateEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg,BOOL bWifi, int soloDebugNo)
#else
void GFL_NET_StateEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, const REGULATION* pReg,BOOL bWifi)
#endif
{
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pReg = pReg;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    _CHANGE_STATE(_battleChildInit, 0);
}

//==============================================================================
/**
 * �o�g�����̎q�Ƃ��Ă̒ʐM�ċN������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRebootBattleChild(void)
{
    CommSystemResetBattleChild();
    _CHANGE_STATE(_battleChildReTry, 0);
}

//==============================================================================
/**
 * �o�g�����̒ʐM�����I���葱��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateExitBattle(void)
{
    if(_pCommState==NULL){
        return;      // ���łɏI�����Ă���ꍇ�͏��O
    }
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
    //_CHANGE_STATE(_stateConnectAutoEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * �q�@��t�ł��邩�ǂ���
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateSetEntryChildEnable(BOOL bEntry)
{
    WHSetEntry(bEntry);
}


//==============================================================================
/**
 * �o�g���ڑ���STATE�Ȃ̂��ǂ����Ԃ�
 * @param   none
 * @retval  �ڑ���TRUE
 */
//==============================================================================

BOOL CommIsBattleConnectingState(void)
{
    int i;
    u32 funcTbl[]={
        (u32)_battleParentWaiting,
        (u32)_battleChildWaiting,
        0,
    };
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return FALSE;
    }
    for(i = 0; funcTbl[i] != 0; i++ ){
        if(stateAddr == funcTbl[i]){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * �r�[�R�����W���J�n����
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionBconCollection(SAVEDATA* pSaveData)
{
    if(_pCommState!=NULL){ // �Ȃ����Ă���ꍇ���͏��O����
        return;
    }
    // �ʐM�q�[�v�쐬
    if(sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_UNION )==0){
		// �ʐM�q�[�v���쐬�ł��Ȃ������B
		// ���s�s�\(�d���؂�G���[�ցj
		CommFatalErrorFunc_NoNumber();

	}
    _commStateInitialize(pSaveData,COMM_MODE_UNION);
    _pCommState->serviceNo = COMM_MODE_UNION;
    _pCommState->regulationNo = 0;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = SOLO_DEBUG_NO;
#endif
    // �X�e�[�g�̑J�ڂ̂��ߏ�����
    _CHANGE_STATE(_unionStart, 0);
}


//==============================================================================
/**
 * ��b���J�n�����̂Ŏq�@�ڑ�
 * @param   �ڑ�����e�@index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionConnectStart(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    WHParentConnectPause(TRUE);  // �e�@�ɂ͂Ȃ�Ȃ�
    WirelessIconEasy();
    _CHANGE_STATE(_unionForceConnectStart, 0);
}

//==============================================================================
/**
 * �q�@�ڑ��ɐ����������ǂ���
 * @param   none
 * @retval  �ڑ������O�@�������P�@���s���|�P
 */
//==============================================================================

int GFL_NET_StateIsUnionConnectSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return -1;
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionChildConnectSuccess){
        return 1;
    }
    if(stateAddr == (u32)_unionChildConnectFailed){
        return -1;
    }
    return 0;
}

//==============================================================================
/**
 * �e�@�ڑ��ɐ����������ǂ���
 * @param   none
 * @retval  �ڑ����Ȃ�TRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsUnionParentConnectSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){  // ���łɏI�����Ă��� �������͏��������܂�
        return FALSE;
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionParentConnect){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * �r�[�R�����W���ĊJ����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionBconCollectionRestart(void)
{
	OS_TPrintf( "���X�^�[�g���܂����I\n" );

    WHParentConnectPause(FALSE);

	if( !(_pCommState->bDisconnectError == TRUE && _pCommState->bErrorAuto == TRUE) ||
		_pCommState->serviceNo == COMM_MODE_UNION ){
	   GFL_NET_StateSetErrorCheck(FALSE,FALSE);
	}

    WirelessIconEasyEnd();
    _pCommState->serviceNo = COMM_MODE_UNION;
    NET_PRINT(">> UNION�r�[�R�����W�ĊJ\n");
    _pCommState->bUnionPause = FALSE;
    WH_SetMaxEntry(_pCommState->limitNum);

    if(CommGetCurrentID() == COMM_PARENT_ID){
        WHParentConnectPauseSystem(TRUE);
        _CHANGE_STATE(_stateConnectChildEndWait, _EXIT_SENDING_TIME2);
    }
    else{
        _CHANGE_STATE(_stateConnectChildEnd, _EXIT_SENDING_TIME);
    }
}

//==============================================================================
/**
 * �r�[�R�����W���ĊJ���m�F����
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_StateUnionIsRestartSuccess(void)
{
    u32 stateAddr;

    if(_pCommState==NULL){
        return TRUE;  // 
    }
    stateAddr = (u32)_pCommState->state;
    if(stateAddr == (u32)_unionChildFinalize){
        return FALSE;
    }
    return TRUE;
}

//==============================================================================
/**
 * UNIONROOM���o��ꍇ�̒ʐM����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateExitUnion(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    OHNO_SP_PRINT(">> UNION��������ؒf\n");
    // �ؒf�X�e�[�g�Ɉڍs����  �����ɐ؂�Ȃ�
    
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * �q�@�Ƃ��ĂȂ���\��Ȃ̂ŁA�e�@�ɂ͂Ȃ�Ȃ�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateChildReserve(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    _pCommState->bUnionPause = TRUE;
    WHParentConnectPause(TRUE);
}

//==============================================================================
/**
 * UNIONROOM�ꎞ��~  �ĊJ��ReStart
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionPause(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    NET_PRINT(">> UNIONROOM�ꎞ��~\n");
    _pCommState->bUnionPause = TRUE;
    WHParentConnectPause(TRUE);
    _CHANGE_STATE(_unionParentPause, 0);
}

//==============================================================================
/**
 * UNIONROOM�A�v���P�[�V�����X�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionAppStart(void)
{
    _pCommState->serviceNo = COMM_MODE_UNION_APP;
}

//==============================================================================
/**
 * UNIONROOM�A�v���P�[�V�����G���h
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionAppEnd(void)
{
    _pCommState->serviceNo = COMM_MODE_UNION;
}

//==============================================================================
/**
 * �s�v�c�ʐM�Ƃ��Ă̒ʐM�����J�n
 * @param   pMyStatus  mystatus
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @param   regulationNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateEnterMysteryParent(SAVEDATA* pSaveData, int serviceNo)
{
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_mysteryParentInit, 0);
}

//==============================================================================
/**
 * �s�v�c�ʐM�̎q�Ƃ��Ă̒ʐM�����J�n
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateEnterMysteryChild(SAVEDATA* pSaveData, int serviceNo)
{
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_mysteryChildInit, 0);
}




void GFL_NET_StateClientConnect(GFL_NETHANDLE* pHandle)
{

    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    _commStateInitialize(pHandle , serviceNo);
    _CHANGE_STATE(_battleChildInit, 0);
}



//==============================================================================
/**
 * �n���X�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underStart(void)
{
    void* pWork;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommIsVRAMDInitialize()){
        return;  //
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    CommSetAloneMode(TRUE);
    CommEnableSendMoveData(); //commsystem�ɂ����Ă���L�[�f�[�^�̑��M������

    if(_pCommState->bParentOnly){
        if(CommParentModeInit(TRUE, _pCommState->bFirstParent, _PACKETSIZE_UNDERGROUND, TRUE))  {
            u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME/2);
            _pCommState->bFirstParent = FALSE;
            _CHANGE_STATE(_underParentWait, _PARENT_WAIT_TIME/2+rand);
        }
    }
    else{
        // �܂��q�@�ɂȂ��Ă݂āA�e�@��T��  ���R�F�e�@�������炷�΂₭�Ȃ��邩��
        if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_UNDERGROUND)){
            if(_pCommState->bNotConnect){
                _CHANGE_STATE(_stateNone,0);
            }
            else{
                _CHANGE_STATE(_underChildPSearching, _CHILD_P_SEARCH_TIME*2);
            }
        }
    }
}

//==============================================================================
/**
 * ���Z�b�g��̍ăX�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildFInit(void)
{
    BOOL bSuccess;

    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    CommSetAloneMode(TRUE);  // ��ڑ����
    CommEnableSendMoveData(); //commsystem�ɂ����Ă���L�[�f�[�^�̑��M������

    if(_pCommState->bParentOnly){
        _CHANGE_STATE(_underParentInit, 0);
    }
    else{
        bSuccess = CommChildModeInit(FALSE, TRUE, _PACKETSIZE_UNDERGROUND);
        if(bSuccess){
            u32 rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME*2);
            if(_pCommState->reConnectTime > 0){
                //NET_PRINT("�Đڑ����ԍl�� %d \n", _pCommState->reConnectTime);
                // �Đڑ����ԍl��
                rand = 2 * _CHILD_P_SEARCH_TIME * _pCommState->reConnectTime;
                _pCommState->reConnectTime = 0;
            }
            _CHANGE_STATE(_underChildPSearching, rand);
        }
    }
}

//==============================================================================
/**
 * �q�@�ăX�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildPInit(void)
{
    BOOL bSuccess;
    
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    bSuccess = CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNDERGROUND);
    if(bSuccess){
        u32 rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME);
        _CHANGE_STATE(_underChildPSearching, _CHILD_P_SEARCH_TIME/2 + rand);
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đe�@��T����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildPSearching(void)
{
    int realParent;
    
    CommMPParentBconCheck();  // bcon�̌���

    if(_pCommState->bPauseFlg || _pCommState->bChildOnly){
        return;
    }
    
    realParent = CommMPGetFastConnectIndex();  //�Ȃ��������Ƃ�����l��������
    if(realParent != -1){
        _pCommState->connectIndex = realParent;  // �L��
        _CHANGE_STATE(_underChildForceConnect, _CHILD_P_SEARCH_TIME);
        NET_PRINT("�{�e�ɐڑ�\n");
        return;
    }
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }
    realParent = CommMPGetNextConnectIndex();  //�Ȃ��������Ƃ�����l��������
    if(realParent != -1){
        _pCommState->connectIndex = realParent;  // �L��
        _CHANGE_STATE(_underChildForceConnect, _CHILD_P_SEARCH_TIME);
        return;
    }


    // �e�����Ȃ��ꍇ ���������e�ɂȂ�
    _CHANGE_STATE(_underChildFinalize, 0);
}


//==============================================================================
/**
 * �ڕW�ƂȂ�e�@�ɑ΂��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildForceConnect(void)
{
    //NET_PRINT("_underChildForceConnect...\n");
    CommMPParentBconCheck();  // bcon�̌���
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // �e���������Ă���ꍇ
        //NET_PRINT("�����ڑ���\n");
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // �ڑ�����܂ŌĂё�����
            _CHANGE_STATE(_underChildConnecting, 100);
            return;
        }
    }
    if(CommIsError()){
        //NET_PRINT("�G���[�̏ꍇ�߂�\n");
        _CHANGE_STATE(_underChildFinalize, 0);
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
    else{
        NET_PRINT("�ڑ����Ԑ؂�̏ꍇ�܂����ǂ�\n");
        _CHANGE_STATE(_underChildFinalize, 0);
    }
}



//==============================================================================
/**
 * �q�@�ƂȂ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _underChildConnecting(void)
{
    if(CommIsConnect(CommGetCurrentID())){   // �������g���ڑ����Ă��邱�Ƃ��m�F�ł�����
        CommSystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
        CommSetAloneMode(FALSE);
        CommEnableSendMoveData();
        //NET_PRINT("�q�@�ڑ�\n");
        _CHANGE_STATE(_underChildConnect, 0);
        return;
    }
    
    if(CommIsError()){
        NET_PRINT("�G���[�̏ꍇchange�ɂ��Ă݂�\n");
        _CHANGE_STATE(_underChildFinalize, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    // ���Ԑ؂�
    _CHANGE_STATE(_underChildFinalize, 0);
}

//==============================================================================
/**
 * �q�@�I�� ���e�ɕς��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_underParentInit, 0);
//    NET_PRINT("�e�@�ɂȂ�ҋ@ \n");
}

//==============================================================================
/**
 * �e�@�I���@�m���Őe�@�ɂȂ邩�q�@�ɂȂ邩���߂�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_underChildPInit, 0);
//    NET_PRINT("�q�@�ƂȂ茟�� %d\n", rand);
}

//==============================================================================
/**
 * �e�@�J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �e�@�ɂȂ��Ă݂�
    CommSetAloneMode(TRUE); // 
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNDERGROUND, TRUE))  {
        u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME/2);
        _pCommState->bFirstParent = FALSE;
        _CHANGE_STATE(_underParentWait, _PARENT_WAIT_TIME/2+rand);
    }
}

//==============================================================================
/**
 * �e�@�ɂȂ�A�q�@���ڑ����Ă���̂�҂�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentWait(void)
{
   if(CommIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�
       NET_PRINT("�����ȊO���Ȃ�������e�@�Œ�\n");
        _pCommState->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
        _CHANGE_STATE(_underParentConnectInit, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(_pCommState->bParentOnly){
        return;
    }
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_underParentFinalize, _FINALIZE_TIME);
    }
}

//==============================================================================
/**
 * �e�@�Ƃ��Đڑ����������̂ŏ��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnectInit(void)
{
    CommSetAloneMode(FALSE);
    CommMPSetNoChildError(TRUE);  // �q�@�����Ȃ��Ȃ�����Č������邽�߂�ERR�����ɂ���
    CommEnableSendMoveData();

    _CHANGE_STATE(_underParentConnect, 0);
}

//==============================================================================
/**
 * �e�@�Ƃ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underParentConnect(void)
{
//    NET_PRINT("�e�@�Ƃ��Đڑ���\n");
    //    if(CommIsError()){
        // �e�q�����ɖ߂�
//        CommMPSwitchParentChild();
//        _CHANGE_STATE(_underChildFinalize, _FINALIZE_TIME);
//    }
}

//==============================================================================
/**
 * �q�@�Ƃ��Đڑ�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildConnect(void)
{
    /*
    // �e�@�����Ȃ��Ȃ�����I�����������ĕʐe��T���ɂ���
    if(CommMPParentDisconnect() || CommIsError()){
        if(CommMPParentDisconnect()){
            NET_PRINT("�e�@�ؒf ??  \n");
        }
        else{
            NET_PRINT("�G���[�ؒf   \n");
        }
//        _stateUnderGroundConnectEnd();
        CommSystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
        // ���邮��܂킷
        _pCommState->reConnectTime = CommGetCurrentID();
        _CHANGE_STATE(_underChildReset, 0);  // �G���[�I���̏ꍇRESET����
    }
       */
}


static void _underSBBoot(void)
{
    NET_PRINT("�I���̊m�F \n");
    if(_pCommState->serviceNo < COMM_MODE_BATTLE_SINGLE_WIFI){
        if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
            return;
        }
        CommSetAloneMode(TRUE);  // ��ڑ����
        CommEnableSendMoveData(); //commsystem�ɂ����Ă���L�[�f�[�^�̑��M������
        CommMPStealth(TRUE);  // �閧���ɐؒf
        NET_PRINT("�ʐM�؂ꂽ�͂� \n");
    }
    else{
    }
    _CHANGE_STATE(_stateNone, 0);
}

//==============================================================================
/**
 * �閧��nOFFLINE�p�Ƀ��Z�b�g�������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underSBReset(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    NET_PRINT("�ċN��    -- %d \n",CommGetCurrentID());
    CommSystemReset();         // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
    _CHANGE_STATE(_underSBBoot, 0);
}

//==============================================================================
/**
 * ���Z�b�g�������s���A�q�@�Ƃ��čċN��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _underChildReset(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    NET_PRINT("�ċN��    -- %d \n",CommGetCurrentID());
    _CHANGE_STATE(_underChildFInit, 0);
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@��I��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleParentSendName(void)
{
    if(!CommIsConnect(CommGetCurrentID())){
        return;
    }
    _CHANGE_STATE(_battleParentWaiting, 0);
}

//==============================================================================
/**
 * �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleParentWaiting(void)
{
    if(!CommIsInitialize()){
        _CHANGE_STATE(_stateEnd,0);
    }
}

//==============================================================================
/**
 * �q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, _pCommState->pReg);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, 0);
    }
}

//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�r�[�R�����W��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildBconScanning(void)
{
    CommMPParentBconCheck();
}

//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�ɋ����炢��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildConnecting(void)
{
    CommMPParentBconCheck();
    if(CommChildIndexConnect(_pCommState->connectIndex)){

//GFL_NET_WLChildMacAddressConnect

        _CHANGE_STATE(_battleChildSendName, _SEND_NAME_TIME);
    }

}

//==============================================================================
/**
 * �q�@���Z�b�g   
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReset(void)
{
    CommMPSwitchParentChild();
    _CHANGE_STATE(_battleChildReConnect, _FINALIZE_TIME);
}

//==============================================================================
/**
 * �q�@����������I�����Đڑ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReConnect(void)
{
    MYSTATUS* pMyStatus;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommMPIsStateIdle()){  /// �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    if(CommChildModeInit(FALSE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildConnecting, _SEND_NAME_TIME);
    }
}


//==============================================================================
/**
 * �q�@�ҋ@���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildWaiting(GFL_NETHANDLE* pNetHandle)
{
    if(!CommIsInitialize()){
        _CHANGE_STATE(_stateEnd,0);
    }
}

//==============================================================================
/**
 * �q�@����������I��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReTry(void)
{
    CommMPSwitchParentChild();
    _CHANGE_STATE(_battleChildReInit, _FINALIZE_TIME);
}

//==============================================================================
/**
 * �q�@����������I�����ċN����������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _battleChildReInit(void)
{
    MYSTATUS* pMyStatus;

    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    if(!CommMPIsStateIdle()){  /// �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    if(CommChildModeInit(FALSE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, _SEND_NAME_TIME);
    }
}

//==============================================================================
/**
 * �������Ȃ��X�e�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateNone(void)
{
    // �Ȃɂ����Ă��Ȃ�
}

//==============================================================================
/**
 * @brief �G���[����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectError(void)
{
}

//==============================================================================
/**
 * @brief  �I��������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEnd(void)
{
    if(CommIsInitialize()){
        return;
    }
    _stateFinalize();
}

//==============================================================================
/**
 * @brief  �n�����E�����J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateUnderGroundConnectEnd(void)
{
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * @brief  �����I�������J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectAutoEnd(void)
{
    if(CommSendFixData(CS_AUTO_EXIT)){
        _CHANGE_STATE(_stateEnd, 0);
    }
}

//==============================================================================
/**
 * @brief   �ڑ��؂�ւ��̑O�ɐe�@�Ȃ�Α���̐ؒf���m�F����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectChildEndWait(void)
{
    if(CommGetConnectNum() <= 1){  // ���������̐ڑ��ɂȂ�����
        WHParentConnectPauseSystem(FALSE);
        CommSystemResetDS();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
        _CHANGE_STATE(_unionChildFinalize, 0);
    }
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }
    // ����
    WHParentConnectPauseSystem(FALSE);
    CommSystemResetDS();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
    _CHANGE_STATE(_unionChildFinalize, 0);
}

//==============================================================================
/**
 * @brief   �ڑ��؂�ւ��̑O�Ɏq�@�Ȃ�ΐ؂ꂽ���ɏ�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectChildEnd(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    CommSystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
    _CHANGE_STATE(_unionChildFinalize, 0);
}

//==============================================================================
/**
 * @brief  �I�������J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateConnectEnd(void)
{
    if(_pCommState->timer != 0){
        _pCommState->timer--;
    }
    if(!CommMPSwitchParentChild()){
        return;
    }
    if(_pCommState->timer != 0){
        return;
    }
    NET_PRINT("�ؒf����");
    CommFinalize();
    _CHANGE_STATE(_stateEnd, 0);
}



//==============================================================================
/**
 * UNION�X�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionStart(void)
{
    void* pWork;

    if(!CommIsVRAMDInitialize()){
        return;  //
    }
    CommMPInitialize(_pCommState->pMyStatus);
    WH_SetMaxEntry(_pCommState->limitNum);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    // �܂��q�@�ɂȂ��Ă݂āA�e�@��T��   ���邮��񂵂�bcon�����߂�
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_unionChildSearching, _CHILD_P_SEARCH_TIME*2);
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đe�@��T����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildSearching(void)
{
    int realParent;

    CommMPParentBconCheck();  // bcon�̌���
    if(_pCommState->timer != 0){
        _pCommState->timer--;
        return;
    }

    if(!CommMPSwitchParentChild()){
        return;
    }
    NET_PRINT("�q�@�����\n");
    _CHANGE_STATE(_unionParentInit, 0);
}

//==============================================================================
/**
 * �q�@�I�� ���e�ɕς��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildFinalize(void)
{
    if(!CommMPSwitchParentChild()){
        return;
    }
    _CHANGE_STATE(_unionParentInit, 0);
}

//==============================================================================
/**
 * �e�@�J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentInit(void)
{
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �e�@�ɂȂ��Ă݂�
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNION,TRUE))  {
        u32 rand = MATH_Rand32(&_pCommState->sRand, _PARENT_WAIT_TIME*2);
        CommSetTransmissonTypeDS();
        _pCommState->bFirstParent = FALSE;
        NET_PRINT("�e�@\n");
        _CHANGE_STATE(_unionParentWait, 10000);
    }
}

//==============================================================================
/**
 * �e�@�ɂȂ�bcon���o
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentWait(void)
{
    if(CommMPIsParentBeaconSent()){  // �r�[�R���𑗂�I�������q�@�ɐ؂�ւ��
    }
    else{
        if(CommIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�
            NET_PRINT("�e�@ -- �Ȃ���\n");
            _pCommState->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
            WirelessIconEasy();
            _CHANGE_STATE(_unionParentConnect, 0);
            return;
        }
        if(_pCommState->timer!=0){
            _pCommState->timer--;
            return;
        }
    }
    NET_PRINT("�e�@�����\n");
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_unionChildRestart, 0);
    }
}

//==============================================================================
/**
 * �q�@�ăX�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionChildRestart(void)
{
    u32 rand;
    
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // ���x�̓r�[�R�����c�����܂�
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        rand = MATH_Rand32(&_pCommState->sRand, _CHILD_P_SEARCH_TIME);
        NET_PRINT("�q�@�J�n %d \n",rand);
        _CHANGE_STATE(_unionChildSearching, rand);
    }
}

//==============================================================================
/**
 * �b�������J�n�ŁA�q�@�ҋ@��ԂɂȂ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionForceConnectStart(void)
{
    if(!CommMPSwitchParentChild()){  // ���̏�Ԃ��I��
        return;
    }
    _CHANGE_STATE(_unionForceConnectStart2, 0);
}


static void _unionForceConnectStart2(void)
{
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �q�@�ɂȂ�
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_unionForceConnect, 100);
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��ċ����ڑ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionForceConnect(void)
{
    NET_PRINT("_unionForceConnect...\n");
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // �e���������Ă���ꍇ
        NET_PRINT("�����ڑ���\n");
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // �ڑ�����܂ŌĂё�����
            _CHANGE_STATE(_unionChildConnecting, 100);
            return;
        }
    }
    if(CommIsError()){
        NET_PRINT("�G���[�̏ꍇ�ēx�d�؂�Ȃ���\n");
        _CHANGE_STATE(_unionChildReset, 0);
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
    else{
        NET_PRINT("�ڑ����Ԑ؂�̏ꍇ�܂����ǂ�\n");
        _CHANGE_STATE(_unionChildReset, 0);
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _unionChildConnecting(void)
{
    if(CommIsError()){
        NET_PRINT("------�G���[�̏ꍇReset\n");
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
    if(CommIsConnect(CommGetCurrentID())){   // �������g���ڑ����Ă��邱�Ƃ��m�F�ł�����
        NET_PRINT("�q�@�ڑ�  �F�ؑ��M\n");
        _pCommState->negotiation = _NEGOTIATION_CHECK;
        CommSendFixSizeData(CS_COMM_NEGOTIATION, _negotiationMsg);
        _CHANGE_STATE(_unionChildNegotiation, 120);
        return;
    }
    
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    // ���Ԑ؂�
    _CHANGE_STATE(_unionChildReset, 0);
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đڑ���  �e�@�F�ؑ҂�
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _unionChildNegotiation(void)
{
    if(CommIsError()){
        NET_PRINT("------�G���[�̏ꍇReset\n");
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
    if(_pCommState->negotiation == _NEGOTIATION_NG){
        NET_PRINT("�ڑ����s-----------------------\n");
        _CHANGE_STATE(_unionChildConnectFailed, 0);
        return;
    }
    if(_pCommState->negotiation == _NEGOTIATION_OK){
        NET_PRINT("�q�@�ڑ�  �F�؂����\n");
        CommInfoSendPokeData();
        _CHANGE_STATE(_unionChildConnectSuccess, 0);
        return;
    }
    
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }
    NET_PRINT(" ���Ԑ؂� \n");
    _CHANGE_STATE(_unionChildReset, 0);
}



static void _unionChildConnectSuccess(void)
{
    if(CommIsError()){
        _CHANGE_STATE(_unionChildReset, 0);
        return;
    }
}

static void _unionChildConnectFailed(void)
{
}

static void _unionChildReset(void)
{
    _pCommState->negotiation = _NEGOTIATION_CHECK;
    if(!CommMPSwitchParentChild()){
        return;
    }

    if(_pCommState->reConnectTime != 0){
        _pCommState->reConnectTime--;
        _CHANGE_STATE(_unionForceConnectStart2, 0);
    }
    else{  // �Ȃ���Ȃ������̂Ŏ��s�X�e�[�g��
        _CHANGE_STATE(_unionChildConnectFailed, 0);
    }
}

//==============================================================================
/**
 * �e�Ƃ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _unionParentConnect(void)
{
    NET_PRINT("commisConnect %d\n",CommIsConnect(1));

    if(!CommIsChildsConnecting()){   // �����ȊO���Ȃ����ĂȂ��΂����������
        if(!CommStateGetErrorCheck()){
            NET_PRINT("�e�@�����ς�\n");
            if( CommMPSwitchParentChild() ){
                _CHANGE_STATE(_unionChildRestart, 0);
            }
        }
    }
    else{
        if(CommGetCurrentID() == COMM_PARENT_ID){
            CommInfoSendArray_ServerSide();  // �q�@����₢���킹����������info�𑗐M
        }
    }
    if(CommIsError()){
        if(!CommStateGetErrorCheck()){
            NET_PRINT("------�G���[�̏ꍇReset\n");
            _CHANGE_STATE(_unionChildReset, 0);
            return;
        }
    }
}

//==============================================================================
/**
 * �e�@�̂܂܈ꎞ��~
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _unionParentPause(void)
{
    u32 rand;
    
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �Ȃ���Ȃ��e�@�ɂȂ�
    if(CommParentModeInit(FALSE, _pCommState->bFirstParent, _PACKETSIZE_UNION, FALSE))  {
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_stateNone,0);
    }
}




static void _pictureBoardChangeParent(void);
static void _pictureBoardInitParent(void);
static void _pictureBoardParentWait(void);

static void _pictureBoardChangeChild(void);
static void _pictureBoardInitChild(void);
static void _pictureBoardForceConnect(void);
static void _pictureBoardChildConnecting(void);

//==============================================================================
/**
 * ���G�������[�h�Ɍq���Ȃ���
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionPictureBoardChange(void)
{
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommGetCurrentID() == COMM_PARENT_ID){
        u8 bDSMode = FALSE;
        CommSendFixSizeData(CS_DSMP_CHANGE, &bDSMode);  //�ړ����[�h�ɕύX����
//        _pCommState->limitNum = COMM_MODE_PICTURE_NUM_MAX+1;
    }
    else{
        u8 bDSMode = FALSE;
        CommSendFixSizeData(CS_DSMP_CHANGE, &bDSMode);  //�ړ����[�h�ɕύX����
    }
}

//==============================================================================
/**
 * ���G�������[�h�q�@�Ƃ��Čq��
 * @param   �ڑ�����e�@index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionPictureBoardChild(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    _CHANGE_STATE(_pictureBoardChangeChild, 0);
    NET_PRINT("���G�����{�[�h�q�@�ڑ�\n");
}

//==============================================================================
/**
 * ���R�[�h�R�[�i�[�̏�Ԃɐ؂�ւ���
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionRecordCornerChange(void)
{
    _pCommState->serviceNo = COMM_MODE_RECORD;
}

//==============================================================================
/**
 * ���R�[�h�R�[�i�[�q�@�Ƃ��Čq��
 * @param   �ڑ�����e�@index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateUnionRecordCornerChild(int index)
{
    _pCommState->connectIndex = index;
    _pCommState->serviceNo = COMM_MODE_RECORD;
    _pCommState->reConnectTime = _RETRY_COUNT_UNION;
    WirelessIconEasy();
    _CHANGE_STATE(_unionForceConnectStart, 0);
}

//==============================================================================
/**
 * ���G����state�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  ���G����state�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsUnionPictureBoardState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(CommIsTransmissonDSType()){
        return FALSE;
    }
    if(stateAddr == (u32)_unionParentConnect){
        return TRUE;
    }
    if(stateAddr == (u32)_unionChildConnectSuccess){
        return TRUE;
    }
    return FALSE;
}


static void _pictureBoardChangeParent(void)
{
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_pictureBoardInitParent, 0);
    }
}

static void _pictureBoardInitParent(void)
{
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    NET_PRINT("���G�����e�ɕύX\n");
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommParentModeInit(FALSE, FALSE, _PACKETSIZE_UNION,TRUE)) {
        CommSetTransmissonTypeMP();
        _CHANGE_STATE(_pictureBoardParentWait, 0);
    }
}


static void _pictureBoardParentWait(void)
{
    if(CommIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�
        _pCommState->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
        CommInfoSendPokeData();
        _CHANGE_STATE(_unionParentConnect, 0);
    }
}


static void _pictureBoardChangeChild(void)
{
    if( CommMPSwitchParentChild() ){
        _CHANGE_STATE(_pictureBoardInitChild, 0);
    }
}

static void _pictureBoardInitChild(void)
{
    if(!CommMPIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    _pCommState->serviceNo = COMM_MODE_PICTURE;
    if(CommChildModeInit(FALSE, FALSE, _PACKETSIZE_UNION)){
        CommSetTransmissonTypeMP();
        _CHANGE_STATE(_pictureBoardForceConnect, 100);
    }
}


//==============================================================================
/**
 * �q�@�ƂȂ��ċ����ڑ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _pictureBoardForceConnect(void)
{
    NET_PRINT("_pictureBoardForceConnect...\n");
    if(CommMPGetParentConnectionNum(_pCommState->connectIndex)!=0){ // �e���������Ă���ꍇ
        NET_PRINT("BOARD�����ڑ���\n");
        if(CommChildIndexConnect(_pCommState->connectIndex)){  // �ڑ�����܂ŌĂё�����
            _CHANGE_STATE(_pictureBoardChildConnecting, 100);
            return;
        }
    }
    if(CommIsError() || (_pCommState->timer == 0)){
        _pCommState->reConnectTime--;
        if(_pCommState->reConnectTime==0){
            NET_PRINT("�ڑ����s %d \n",_pCommState->reConnectTime);
            _CHANGE_STATE(_unionChildConnectFailed, 0);
        }
        else{
            _CHANGE_STATE(_pictureBoardChangeChild, 0);
        }
    }
    else if(_pCommState->timer!=0){
        _pCommState->timer--;
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _pictureBoardChildConnecting(void)
{
    if(_pCommState->timer > 90){
        _pCommState->timer--;
        return;
    }
    
    if(CommIsError()){
        _pCommState->reConnectTime--;
        if(_pCommState->reConnectTime==0){
            NET_PRINT("�ڑ����s %d \n",_pCommState->reConnectTime);
            _CHANGE_STATE(_unionChildConnectFailed, 0);
        }
        else{
            NET_PRINT("------�G���[�̏ꍇReset\n");
            _CHANGE_STATE(_pictureBoardChangeChild, 0);
        }
        return;
    }
    
    if(CommIsConnect(CommGetCurrentID())){   // �������g���ڑ����Ă��邱�Ƃ��m�F�ł�����
        NET_PRINT("�q�@�ڑ�\n");
        _CHANGE_STATE(_unionChildConnectSuccess, 0);
        return;
    }
    if(_pCommState->timer!=0){
        _pCommState->timer--;
        return;
    }

    _pCommState->reConnectTime--;
    NET_PRINT("�ڑ����s %d \n",_pCommState->reConnectTime);
    if(_pCommState->reConnectTime==0){
        _CHANGE_STATE(_unionChildConnectFailed, 0);
    }
    else{
        // ���Ԑ؂�
        _CHANGE_STATE(_pictureBoardChangeChild, 0);
    }
}

//==============================================================================
/**
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void CommRecvNegotiation(int netID, int size, void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    int i;
    u8* pMsg = pData;
    BOOL bMatch = TRUE;

    NET_PRINT("------CommRecvNegotiation\n");

    if(CommGetCurrentID() != COMM_PARENT_ID){  // �e�@�̂ݔ��f�\
        return;
    }
    bMatch = TRUE;
    for(i = 0; i < sizeof(_negotiationMsg); i++){
        if(pMsg[i] != _negotiationMsg[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch  && (!_pCommState->bUnionPause)){   // �q�@����ڑ��m�F������
//        if(CommGetConnectNum() <= _pCommState->limitNum){  // �w��ڑ��l����艺��邱��
            NET_PRINT("------�����𑗐M \n");
            _negotiationMsgReturnOK[0] = netID;
            CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnOK);
            return;
//        }
    }
    NET_PRINT("------���s�𑗐M %d %d\n",bMatch,_pCommState->bUnionPause);
    _negotiationMsgReturnNG[0] = netID;
    CommSendFixSizeData_ServerSide(CS_COMM_NEGOTIATION_RETURN, _negotiationMsgReturnNG);
}

//==============================================================================
/**
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void CommRecvNegotiationReturn(int netID, int size, void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    u8 id;
    int i;
    u8* pMsg = pData;
    BOOL bMatch = TRUE;

    for(i = 1; i < sizeof(_negotiationMsgReturnOK); i++){
        if(pMsg[i] != _negotiationMsgReturnOK[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch){   // �e�@����ڑ��F�؂�����
        id = pMsg[0];
        if(id == CommGetCurrentID()){
            _pCommState->negotiation = _NEGOTIATION_OK;
        }
        return;
    }
    bMatch = TRUE;
    for(i = 1; i < sizeof(_negotiationMsgReturnNG); i++){
        if(pMsg[i] != _negotiationMsgReturnNG[i]){
            bMatch = FALSE;
            break;
        }
    }
    if(bMatch){   // �e�@����ڑ��ے肪����
        id = pMsg[0];
        NET_PRINT("�ڑ����ۃR�}���h������%d %d\n",id , CommGetCurrentID());
        if(id == (u8)CommGetCurrentID()){
            _pCommState->negotiation = _NEGOTIATION_NG;
        }
        return;
    }
}

//==============================================================================
/**
 * �l�S�V�G�[�V�����p�R�[���o�b�N
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

int CommRecvGetNegotiationSize(void)
{
    return sizeof(_negotiationMsg);
}


//==============================================================================
/**
 * �����Ă���l���𐧌�����
 * @param   �l��
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateSetLimitNum(int num)
{
    if(_pCommState){
        _pCommState->limitNum = num;
        WH_SetMaxEntry(num);
    }
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@�r�[�R�����W��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _poketchBconScanning(void)
{
    CommMPParentBconCheck();
}

//==============================================================================
/**
 * �|�P�b�`�q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _poketchChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_POKETCH)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_poketchBconScanning, 0);
    }
}

//==============================================================================
/**
 * �ۂ������Ƃ��Ă̒ʐM�����J�n�i�q�@��Ԃ̂݁j
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateEnterPockchChild(SAVEDATA* pSaveData)
{
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_POKETCH );
    _commStateInitialize(pSaveData,COMM_MODE_POKETCH);
//    _pCommState->serviceNo = COMM_MODE_POKETCH;
    _pCommState->regulationNo = 0;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_poketchChildInit, 0);
}

//==============================================================================
/**
 * �ۂ������Ƃ��Ă̏I������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateExitPoketch(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    NET_PRINT("��������ؒf\n");
    // �ؒf�X�e�[�g�Ɉڍs����  �����ɐ؂�Ȃ�
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * �ʐM�T�[�`���[�ɂ����ď����WSTATE�Ȃ̂��ǂ����Ԃ�
 * @param   none
 * @retval  ���W���Ȃ�TRUE
 */
//==============================================================================

BOOL CommIsPoketchSearchingState(void)
{
    int i;
    u32 funcTbl[]={
        (u32)_poketchBconScanning,
        0,
    };
    u32 stateAddr = (u32)_pCommState->state;

    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return FALSE;
    }
    for(i = 0; funcTbl[i] != 0; i++ ){
        if(stateAddr == funcTbl[i]){
            return TRUE;
        }
    }
    return FALSE;
}


//==============================================================================
/**
 * PARTY�T�[�`callback
 * @param   ggid
 * @retval  none
 */
//==============================================================================

static void _callbackGGIDScan(u32 ggid,int serviceNo)
{
    switch(ggid){
      case _MYSTERY_GGID:
        if(serviceNo == COMM_MODE_MYSTERY){
            _pCommState->partyGameBit |= PARTYGAME_MYSTERY_BCON;
        }
        break;
      case _BCON_DOWNLOAD_GGID:
        _pCommState->partyGameBit |= PARTYGAME_MYSTERY_BCON;
        break;
      case _RANGER_GGID:
        _pCommState->partyGameBit |= PARTYGAME_RANGER_BCON;
        break;
      case _WII_GGID:
        _pCommState->partyGameBit |= PARTYGAME_WII_BCON;
        break;
    }
}

//==============================================================================
/**
 * PARTY�T�[�`�q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _partySearchChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    WHSetGGIDScanCallback(_callbackGGIDScan);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_PARTY)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_poketchBconScanning, 0);
    }
}

//==============================================================================
/**
 * �p�[�e�B�[�Q�[�������̒ʐM�����J�n�i�q�@��Ԃ̂݁j
 * @param   SAVEDATA  savedata
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateEnterPartyGameScanChild(SAVEDATA* pSaveData)
{
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_PARTY );
    _commStateInitialize(pSaveData,COMM_MODE_PARTY);
    _pCommState->regulationNo = 0;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = 0;
#endif
    _CHANGE_STATE(_partySearchChildInit, 0);
}

//==============================================================================
/**
 * �p�[�e�B�[�Q�[���T�[�`�̏I������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateExitPartyGameScan(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    NET_PRINT("��������ؒf\n");
    // �ؒf�X�e�[�g�Ɉڍs����  �����ɐ؂�Ȃ�
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * �E�����r�[�R����BIT��Ԃ�
 * @param   none
 * @retval  none
 */
//==============================================================================

u8 GFL_NET_StateGetPartyGameBit(void)
{
  return _pCommState->partyGameBit;
}

//==============================================================================
/**
 * ���Z�b�g�I������
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_StateExitReset(void)
{
    CommSystemShutdown();
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return TRUE;
    }
    if(_pCommState->serviceNo == COMM_MODE_DPW_WIFI || _pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI){
        mydwc_Logout();
        return TRUE;
    }
    else if(CommStateIsWifiConnect()){
        OHNO_SP_PRINT(" ���O�A�E�g����\n");
        _CHANGE_STATE(_stateWifiLogout,0);  // ���O�A�E�g����
    }
    else{
        _CHANGE_STATE(_underSBReset, 0);  // �G���[�I���̏ꍇRESET����
    }
    return FALSE;
}

//==============================================================================
/**
 * �s�v�c�ʐM�e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _mysteryParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, NULL);

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleParentSendName, 0);
    }
}

//==============================================================================
/**
 * �s�v�c�ʐM�q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _mysteryChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    CommMPInitialize(_pCommState->pMyStatus);
    CommInfoInitialize(_pCommState->pSaveData, NULL);
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_battleChildBconScanning, 0);
    }
}




//---------------------wifi


//==============================================================================
/**
 * �G���[�������������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleError(void){}

static void _wifiBattleTimeout(void)
{
   int ret;
    ret = mydwc_step();

	if( ret < 0 ){
        // �G���[�����B
        _CHANGE_STATE(_wifiBattleError, 0);
    }		
}

static void _wifiBattleDisconnect(void){}

static void _wifiBattleFailed(void){}

static int _wifiLinkLevel(void)
{
    return WM_LINK_LEVEL_3 - DWC_GetLinkLevel();
}

//==============================================================================
/**
 * �ڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleConnect(void)
{
    int ret,errCode;

    CommSetWifiConnect(TRUE);

    ret = mydwc_step();

    if(ret >= DWC_ERROR_FRIENDS_SHORTAGE){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    else if( ret < 0 ){
        // �G���[�����B
        _CHANGE_STATE(_wifiBattleError, 0);
    }		
    else if( ret == 1 ) {     // �^�C���A�E�g�i���肩��P�O�b�ȏチ�b�Z�[�W���͂��Ȃ��j
        _CHANGE_STATE(_wifiBattleTimeout, 0);
    }
    else if(ret == 2){
        NET_PRINT("�ؒf�G���[\n");
        if(_pCommState->bDisconnectError){
            _CHANGE_STATE(_wifiBattleError, 0);
        }
        else{
            _CHANGE_STATE(_wifiBattleDisconnect, 0);
        }
    }
}

//==============================================================================
/**
 * �G���[�R�[�h�R���o�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _errcodeConvert(int ret)
{
    // �G���[�����B	
    int errorcode;
    if( (ret == ERRORCODE_HEAP) || (ret == ERRORCODE_0)){
        errorcode = ret;
    }
    else {
        errorcode = -ret;
    }
    _pCommState->errorCode = errorcode;
    OS_TPrintf("�G���[���������܂����B�G���[�R�[�h(%d)\n", errorcode);
}


//==============================================================================
/**
 * �ΐ푊���T����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleMaching(void)
{
    int ret = mydwc_stepmatch( 0 );

    if(ret >= DWC_ERROR_FRIENDS_SHORTAGE){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    else if( ret < 0 ){
        _errcodeConvert(ret);
        _CHANGE_STATE(_wifiBattleError, 0);
    }
    else if ( ret == STEPMATCH_SUCCESS )  {
        OS_TPrintf("�ΐ푊�肪������܂����B\n");
        _CHANGE_STATE(_wifiBattleConnect, 0);
    }
    else if ( ret == STEPMATCH_CANCEL ){
        OS_TPrintf("�L�����Z�����܂����B\n");	
        _CHANGE_STATE(_wifiBattleDisconnect, 0);
    }
    else if( ret == STEPMATCH_FAIL){
        _CHANGE_STATE(_wifiBattleFailed, 0);
    }
    
}



//==============================================================================
/**
 * �L�����Z����  �e����߂鎞��؂�ւ��鎞�Ɏg�p
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleCanceling(void)
{

    int ret = mydwc_stepmatch( 1 );  // �L�����Z����
				
    if( ret < 0 ){
        // �G���[�����B
        _errcodeConvert(ret);
        _CHANGE_STATE(_wifiBattleError, 0);
    }
    else{
        if( mydwc_startgame( _pCommState->wifiTargetNo ) ){
            CommSystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
            if( _pCommState->wifiTargetNo < 0 ){
                OS_TPrintf("�Q�[���Q���҂��W���܂��B\n");
            } else {
                OS_TPrintf(" %d�Ԗڂ̗F�B�ɐڑ����܂��B\n", _pCommState->wifiTargetNo);	        
            }
            _CHANGE_STATE(_wifiBattleMaching, 0);
        }
        else{
            NET_PRINT(" ��Ԃ�ύX�ł��Ȃ����� ������������\n");
//            _CHANGE_STATE(_wifiBattleLogin, 0);  // ��Ԃ�ύX�ł��Ȃ�����
        }
    }
}

#define _VCT_TEST (1)

//==============================================================================
/**
 * �Q�[�����J�n����B// 2006.4.13 �g���ǉ�
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. ���s
 */
//==============================================================================
int CommWifiBattleStart( int target )
{
#if _VCT_TEST
	mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
    _pCommState->wifiTargetNo = target;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // ���̏�Ԃ�j��
    return 1;
#else

    if( _pCommState->state != _wifiBattleLogin ) return 0;
	
	mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
	
    if( mydwc_startgame( target ) ){
	    if( target < 0 ){
	        OS_TPrintf("�Q�[���Q���҂��W���܂��B\n");
        } else {
	        OS_TPrintf("%d�Ԗڂ̗F�B�ɐڑ����܂��B\n", target);	        
        }
        _CHANGE_STATE(_wifiBattleMaching, 0);
        return 1;
    }	
    return 0;
#endif
}

//==============================================================================
/**
 * �}�b�`���O�������������ǂ����𔻒�
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. �ڑ���   2. �G���[��L�����Z���Œ��f
 */
//==============================================================================
int CommWifiIsMatched(void)
{
	if( _pCommState->state == _wifiBattleMaching ) return 0;
	if( _pCommState->state == _wifiBattleConnect ) return 1;
    if(  _pCommState->state == _wifiBattleTimeout ) return 3;
    if(  _pCommState->state == _wifiBattleDisconnect ) return 4;
    if(  _pCommState->state == _wifiBattleFailed ) return 5;
//	if( _pCommState->state == _wifiBattleError ) return 5;
    return 2;
}

//==============================================================================
/**
 * �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleLogin(void)
{
    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
    if( mydwc_startgame( -1 ) ){   //�ŏ���VCT�҂���ԂɂȂ�
        _pCommState->bWifiDisconnect = FALSE;
        CommSystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
        OS_TPrintf("VCT�Q���҂��W���܂��B\n");
        _CHANGE_STATE(_wifiBattleMaching, 0);
    }
}

//==============================================================================
/**
 * �}�b�`���O���I������
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _stateWifiMatchEnd(void)
{
    int ret;

    CommSetWifiConnect(FALSE);
    if(mydwc_disconnect( _pCommState->disconnectIndex )){
        mydwc_returnLobby();
        _CHANGE_STATE(_wifiBattleLogin, 0);
        return;
    }
    ret = mydwc_step();
    if( ret < 0 ){
        NET_PRINT(" �G���[�����B");
        _CHANGE_STATE(_wifiBattleError, 0);
    }
}

//==============================================================================
/**
 * Wifi�ؒf�R�}���h���󂯎�����ꍇTRUE
 * @param   none
 * @retval  WifiLogin�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiDisconnect(void)
{
    return _pCommState->bWifiDisconnect;
}

//==============================================================================
/**
 * WifiLogin�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLogin�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiLoginState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(stateAddr == (u32)_wifiBattleLogin){
        return TRUE;
    }
    return FALSE;
}


BOOL GFL_NET_StateIsWifiLoginMatchState(void)
{
    u32 stateAddr = (u32)_pCommState->state;

    if(stateAddr == (u32)_wifiBattleMaching){
        return TRUE;
    }
    else if(stateAddr == (u32)_wifiBattleLogin){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * WifiError�̏ꍇ���̔ԍ���Ԃ�  �G���[�ɂȂ��Ă邩�ǂ������m�F���Ă�������o������
 * @param   none
 * @retval  Error�ԍ�
 */
//==============================================================================

int GFL_NET_StateGetWifiErrorNo(void)
{
    return _pCommState->errorCode;
}



//==============================================================================
/**
 * WIFI�Ń��O�A�E�g���s���ꍇ�̏���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateWifiLogout(void)
{
    int ret;
    
    CommSetWifiConnect(FALSE);
    if(mydwc_disconnect( 0 )){
        mydwc_returnLobby();
        _CHANGE_STATE(_stateConnectEnd, 0);
    }
    ret = mydwc_step();
    if( ret < 0 ){
        // �G���[�����B
        _CHANGE_STATE(_wifiBattleError, 0);
    }
}

//==============================================================================
/**
 * WIFI�Ń��O�A�E�g���s���ꍇ�̏�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiLogout(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
    OHNO_SP_PRINT("��������ؒf\n");
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * WIFI�Ń}�b�`���O��؂�ꍇ�̏���
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiMatchEnd(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    _pCommState->disconnectIndex = 0;
    _CHANGE_STATE(_stateWifiMatchEnd, 0);
}

//==============================================================================
/**
 * WIFI�Ń}�b�`���O��؂�ꍇ�̏��� ��������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiTradeMatchEnd(void)
{
    if(_pCommState==NULL){  // ���łɏI�����Ă���
        return;
    }
    if(CommGetCurrentID() == COMM_PARENT_ID){
        NET_PRINT("�e�͎����Ő؂�\n");
        _pCommState->disconnectIndex = 0;
    }
    else{
        NET_PRINT("�q�͑ҋ@����\n");
        _pCommState->disconnectIndex = 1;
    }
    _CHANGE_STATE(_stateWifiMatchEnd, 0);
}





void GFL_NET_StateWifiBattleMatchEnd(void)
{
    u8 id = CommGetCurrentID();
    
    NET_PRINT("CS_WIFI_EXIT %d\n",id);
    CommSendFixSizeData(CS_WIFI_EXIT,&id);
}


//==============================================================================
/**
 * WifiError���ǂ���
 * @param   none
 * @retval  Error
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiError(void)
{
    if(_pCommState){
        u32 stateAddr = (u32)_pCommState->state;
        if(stateAddr == (u32)_wifiBattleError){
            return TRUE;
        }
        if((stateAddr == (u32)_wifiBattleTimeout) && _pCommState->bDisconnectError){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * ���E�����A���E�o�g���^���[�̊J�n  �G���[�Ǘ��p
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiDPWStart(SAVEDATA* pSaveData)
{
    if(!_pCommState){
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_DPW );
        _pCommState = (_COMM_STATE_WORK*)GFL_HEAP_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
        MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
        _pCommState->serviceNo = COMM_MODE_DPW_WIFI;
        _pCommState->bWorldWifi = TRUE;
        _pCommState->pSaveData = pSaveData;
        GFL_NET_StateSetErrorCheck(FALSE,TRUE);
        sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    }
}

//==============================================================================
/**
 * ���E�����A���E�o�g���^���[�̏I��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiDPWEnd(void)
{
    if(_pCommState){
        sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
        GFL_NET_StateSetErrorCheck(FALSE,FALSE);
        GFL_HEAP_FreeMemory(_pCommState);
        _pCommState = NULL;
        GFL_HEAP_DeleteHeap(HEAPID_COMMUNICATION);
    }
}

//==============================================================================
/**
 * ���E�ӂ�������  �G���[�Ǘ��p
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiFusigiStart(SAVEDATA* pSaveData)
{
    if(!_pCommState){
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_DPW );
        _pCommState = (_COMM_STATE_WORK*)GFL_HEAP_AllocMemory(HEAPID_COMMUNICATION, sizeof(_COMM_STATE_WORK));
        MI_CpuFill8(_pCommState, 0, sizeof(_COMM_STATE_WORK));
        _pCommState->serviceNo = COMM_MODE_FUSIGI_WIFI;
        _pCommState->bWorldWifi = TRUE;
        _pCommState->pSaveData = pSaveData;
        GFL_NET_StateSetErrorCheck(FALSE,TRUE);
        sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    }
}

//==============================================================================
/**
 * ���E�����A���E�o�g���^���[�̏I��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiFusigiEnd(void)
{
    if(_pCommState){
        sys_SoftResetOK(SOFTRESET_TYPE_WIFI);
        GFL_NET_StateSetErrorCheck(FALSE,FALSE);
        GFL_HEAP_FreeMemory(_pCommState);
        _pCommState = NULL;
        GFL_HEAP_DeleteHeap(HEAPID_COMMUNICATION);
    }
}




//==============================================================================
/**
 * ���E�����A���E�o�g���^���[�ŃG���[���ǂ���
 * @param   none
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_StateGetWifiDPWError(void)
{
	int errorCode,ret;
	DWCErrorType myErrorType;
    
    if(!_pCommState){
        return FALSE;
    }

    if((_pCommState->serviceNo == COMM_MODE_DPW_WIFI)){
        ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
        if(ret!=0){
            DWC_ClearError();
            return TRUE;
        }
    } else if(_pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI){
        ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
        if(ret!=0){
	  if(myErrorType == DWC_ETYPE_FATAL){
	    OS_TPrintf("�G���[��ߊl: %d, %d\n", errorCode, myErrorType);
            return TRUE;
	  }
	}
    }
    return FALSE;
}


//==============================================================================
/**
 * �q�@�ҋ@���  �e�@��I��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleConnecting(void)
{
    // �ڑ���
    int ret = mydwc_connect();

    if( ret < 0 ){
        _errcodeConvert(ret);
        _CHANGE_STATE(_wifiBattleError, 0);
    }
    else if ( ret > 0 ) 
    {
        OS_TPrintf("WiFi�R�l�N�V�����ɐڑ����܂����B\n	");
        _CHANGE_STATE(_wifiBattleLogin, 0);
    }
}


//==============================================================================
/**
 * �e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleParentInit(void)
{
    MYSTATUS* pMyStatus;
    
    if(!CommIsVRAMDInitialize()){
        return;
    }
    {
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_WIFIMENU, _HEAPSIZE_WIFI);
    }

    if(CommParentModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE,TRUE)){
        mydwc_startConnect(_pCommState->pSaveData, HEAPID_WIFIMENU);
        mydwc_setFetalErrorCallback(CommFatalErrorFunc);

        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_wifiBattleConnecting, 0);
    }
}

//==============================================================================
/**
 * �q�@�̏�����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiBattleChildInit(void)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }

    {
        sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_WIFIMENU, _HEAPSIZE_WIFI);
    }
    
    if(CommChildModeInit(TRUE, TRUE, _PACKETSIZE_BATTLE)){
        mydwc_startConnect(_pCommState->pSaveData, HEAPID_WIFIMENU);
        CommSetTransmissonTypeDS();
        _CHANGE_STATE(_wifiBattleConnecting, 0);
    }
}

//==============================================================================
/**
 * �o�g�����̎q�Ƃ��Ă̒ʐM�����J�n
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

#ifdef GFL_NET_DEBUG
void GFL_NET_StateWifiEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo, int soloDebugNo)
#else
void GFL_NET_StateWifiEnterBattleChild(SAVEDATA* pSaveData, int serviceNo, int regulationNo)
#endif
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    // �ʐM�q�[�v�쐬
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pSaveData = pSaveData;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    
    _CHANGE_STATE(_wifiBattleChildInit, 0);
}


//==============================================================================
/**
 * �o�g�����̐e�Ƃ��Ă̒ʐM�����J�n
 * @param   pMyStatus  mystatus
 * @param   serviceNo  �ʐM�T�[�r�X�ԍ�
 * @param   regulationNo  �ʐM�T�[�r�X�ԍ�
 * @retval  none
 */
//==============================================================================

#ifdef GFL_NET_DEBUG
void GFL_NET_StateWifiEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo, int soloDebugNo)
#else
void GFL_NET_StateWifiEnterBattleParent(SAVEDATA* pSaveData, int serviceNo, int regulationNo)
#endif
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,serviceNo);
//    _pCommState->serviceNo = serviceNo;
    _pCommState->regulationNo = regulationNo;
    _pCommState->pSaveData = pSaveData;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = soloDebugNo;
#endif
    _CHANGE_STATE(_wifiBattleParentInit, 0);
}

//==============================================================================
/**
 * WIFI���Ƀ��O�C������ׂ̒ʐM�J�n
 * @param   pSaveData
 * @retval  none
 */
//==============================================================================

void* GFL_NET_StateWifiEnterLogin(SAVEDATA* pSaveData, int wifiFriendStatusSize)
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(CommIsInitialize()){
        return NULL;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    // �ʐM�q�[�v�쐬
    sys_SoftResetNG(SOFTRESET_TYPE_WIFI);
    sys_CreateHeapLo( HEAPID_BASE_APP, HEAPID_COMMUNICATION, _HEAPSIZE_BATTLE );
    _commStateInitialize(pSaveData,COMM_MODE_LOGIN_WIFI);
    _pCommState->pWifiFriendStatus = GFL_HEAP_AllocMemory( HEAPID_COMMUNICATION, wifiFriendStatusSize );
    MI_CpuFill8( _pCommState->pWifiFriendStatus, 0, wifiFriendStatusSize );
//    _pCommState->serviceNo = COMM_MODE_LOGIN_WIFI;
    _pCommState->regulationNo = 0;
    _pCommState->pSaveData = pSaveData;
#ifdef GFL_NET_DEBUG
    _pCommState->soloDebugNo = 0;
#endif

    
    _CHANGE_STATE(_wifiBattleParentInit, 0);
    return _pCommState->pWifiFriendStatus;
}

//==============================================================================
/**
 * WIFI�t�����h�pWORK��n��
 * @param   pSaveData
 * @retval  none
 */
//==============================================================================

void* GFL_NET_StateGetMatchWork(void)
{
    return _pCommState->pWifiFriendStatus;
}

//==============================================================================
/**
 * ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateSetErrorCheck(BOOL bFlg,BOOL bAuto)
{
    if(_pCommState){
        _pCommState->bDisconnectError = bFlg;
        _pCommState->bErrorAuto = bAuto;
        NET_PRINT("CommStateSetErrorCheck %d %d\n",_pCommState->bDisconnectError,_pCommState->bErrorAuto);
    }
    CommMPSetNoChildError(bFlg);  // �q�@�����Ȃ��Ȃ�����Č������邽�߂�ERR�����ɂ���
    CommMPSetDisconnectOtherError(bFlg);
}


//==============================================================================
/**
 * ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_StateGetErrorCheck(void)
{
    if(_pCommState){
        if(_pCommState->stateError!=0){
            return TRUE;
        }
        return _pCommState->bErrorAuto;
    }
    return FALSE;
//    CommMPSetDisconnectOtherError(bFlg);
}


#ifdef GFL_NET_DEBUG
//==============================================================================
/**
 * �f�o�b�O�p�ʐM�ڑ��J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvDebugStart(int netID, int size, void* pData, void* pWork)
{
    if(_pCommState){
        _pCommState->bDebugStart = TRUE;
    }
    NET_PRINT("�R�}���h����\n");
}

BOOL GFL_NET_StateDBattleIsReady(void)
{
    if(_pCommState){
        return _pCommState->bDebugStart;
    }
    return FALSE;
}

static void _commConnectChildDebug(TCB_PTR tcb, void* work)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    if(CommMPGetGFBss(_pCommState->connectIndex)!=NULL){
        NET_PRINT("�f�o�b�O�q�@�ڑ��J�n\n");
        GFL_NET_StateConnectBattleChild(_pCommState->connectIndex);  // �ڑ�
        TCB_Delete(tcb);
    }
}

static void _commConnectParentDebug(TCB_PTR tcb, void* work)
{
    if(!CommIsVRAMDInitialize()){
        return;
    }
    if(CommGetConnectNum() == (CommLocalGetServiceMaxEntry(CommStateGetServiceNo())+1) ){
        NET_PRINT("�ڑ��l���ɒB����\n");
        CommSendFixData(CS_DEBUG_START);  // �S���ɃR�}���h���M
        TCB_Delete(tcb);
    }
}

void GFL_NET_StateDBattleConnect(BOOL bParent, int gameMode, SAVEDATA* pSaveData)
{
    MYSTATUS* pMyStatus = SaveData_GetMyStatus(pSaveData);
    if(COMM_MODE_BATTLE_SINGLE_WIFI <= gameMode){
/*  ���͕���
        DwcOverlayStart();
        if(!bParent){
            GFL_NET_StateWifiEnterBattleChild(pSaveData, gameMode, 0, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            _pCommState->connectIndex = 0;
            TCB_Add(_commConnectChildDebug, NULL, _TCB_COMMCHECK_PRT-2);
        }
        else{
            GFL_NET_StateWifiEnterBattleParent(pSaveData, gameMode, 0, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            TCB_Add(_commConnectParentDebug, NULL, _TCB_COMMCHECK_PRT-2);
        }
   */
    }
    else{
        if(!bParent){
            GFL_NET_StateEnterBattleChild(pSaveData, gameMode, 0, NULL, FALSE, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            _pCommState->connectIndex = 0;
            TCB_Add(_commConnectChildDebug, NULL, _TCB_COMMCHECK_PRT-1);
        }
        else{
            GFL_NET_StateEnterBattleParent(pSaveData, gameMode, 0, NULL, FALSE, SOLO_DEBUG_NO + COMMDIRECT_DEBUG_NO);
            TCB_Add(_commConnectParentDebug, NULL, _TCB_COMMCHECK_PRT-1);
        }
    }
    _pCommState->bDebugStart = FALSE;
}

#endif //GFL_NET_DEBUG



int GFL_NET_StateGetServiceNo(void)
{
    if(_pCommState){
        return _pCommState->serviceNo;
    }
    return COMM_MODE_BATTLE_SINGLE;
}

int GFL_NET_StateGetRegulationNo(void)
{
    if(_pCommState){
        return _pCommState->regulationNo;
    }
    return COMM_REG_NORMAL;
}

void GFL_NET_StateSetPokemon(u8* sel)
{
    MI_CpuCopy8(sel,_pCommState->select,NELEMS(_pCommState->select));

    OHNO_SP_PRINT("%d %d %d %d %d %d\n",sel[0],sel[1],sel[2],sel[3],sel[4],sel[5]);
}

void GFL_NET_StateGetPokemon(u8* sel)
{
    MI_CpuCopy8(_pCommState->select,sel,NELEMS(_pCommState->select));
}


#ifdef GFL_NET_DEBUG
int GFL_NET_StateGetSoloDebugNo(void)
{
    if(_pCommState){
        return _pCommState->soloDebugNo;
    }
    return 0;
}
#endif


//==============================================================================
/**
 * �ʐM��WIFI�ڑ����Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �ڑ����Ă�����TRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiConnect(void)
{
    if(CommStateGetServiceNo() < COMM_MODE_BATTLE_SINGLE_WIFI){
        return FALSE;
    }
    return TRUE;
}


//==============================================================================
/**
 * �G���[��Ԃɓ���
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommSetErrorReset(u8 type)
{
    if(_pCommState){
        _pCommState->ResetStateType = type;
    }
}

//==============================================================================
/**
 * ���Z�b�g�G���[��ԂɂȂ������ǂ����m�F����
 * @param   none
 * @retval  ���Z�b�g�G���[��ԂȂ�TRUE
 */
//==============================================================================

u8 CommIsResetError(void)
{
    if(_pCommState){
        return _pCommState->ResetStateType;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   fatal�G���[�p�֐�  ���̊֐������甲�����Ȃ�
 * @param   no   �G���[�ԍ�
 * @retval  none
 */
//--------------------------------------------------------------

void CommFatalErrorFunc(int no)
{
    int i=0;
    
    ComErrorWarningResetCall(HEAPID_BASE_SYSTEM,COMM_ERRORTYPE_POWEROFF, no);
    while(1){
        i++;
    }
}

void CommFatalErrorFunc_NoNumber( void )
{
    int i=0;
    
    ComErrorWarningResetCall( HEAPID_BASE_SYSTEM, 4, 0 );
    while(1){
        i++;
    }
}

//--------------------------------------------------------------
/**
 * @brief   �G���[�����m���� �ʐM�G���[�p�E�C���h�E���o��
 * @param   heapID    �������[�m�ۂ���HEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------

void CommErrorCheck(int heapID, GF_BGL_INI* bgl)
{
// ���̊֐��ł͏������Ȃ����ƂɂȂ�܂���
}


//--------------------------------------------------------------
/**
 * @brief   �G���[�����m���� �ʐM�G���[�p�E�C���h�E���o��
 * @param   heapID    �������[�m�ۂ���HEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------

void CommErrorDispCheck(int heapID)
{
    if(CommStateGetErrorCheck()){
        if(CommIsError() || GFL_NET_StateIsWifiError() || GFL_NET_StateGetWifiDPWError()
           || (_pCommState->stateError!=0)){
            if(!CommIsResetError()){   // ���Z�b�g�G���[��ԂŖ����ꍇ
                NET_PRINT("�T�E���h�X�g�b�v\n");
                Snd_Stop();
                NET_PRINT("�Z�[�u�X�g�b�v\n");
                SaveData_DivSave_Cancel(_pCommState->pSaveData); // �Z�[�u���Ă���~�߂�
                sys.tp_auto_samp = 1;  // �T���v�����O���~�߂�

                NET_PRINT("�G���[�\��\n");
                if(_pCommState->stateError == COMM_ERROR_RESET_GTS){
                    CommSetErrorReset(COMM_ERROR_RESET_GTS);  // �G���[���Z�b�g��ԂɂȂ�
                }
                else if((_pCommState->serviceNo == COMM_MODE_FUSIGI_WIFI)
                   || (_pCommState->serviceNo == COMM_MODE_MYSTERY)){
                    CommSetErrorReset(COMM_ERROR_RESET_TITLE);  // �G���[���Z�b�g��ԂɂȂ�
                }
                else{
                    CommSetErrorReset(COMM_ERROR_RESET_SAVEPOINT);  // �G���[���Z�b�g��ԂɂȂ�
                }
            }
        }
    }
}

//--------------------------------------------------------------
/**
 * @brief   ���Z�b�g�ł����Ԃ��ǂ���
 * @param   none
 * @retval  TRUE �Ȃ烊�Z�b�g
 */
//--------------------------------------------------------------

BOOL GFL_NET_StateIsResetEnd(void)
{
    if(CommMPIsConnectStalth() || !_stateIsMove()){ // �ʐM�I��
        return TRUE;
    }
    if(!CommMPIsConnect()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * �G���[�ɂ���ꍇ���̊֐����ĂԂƃG���[�ɂȂ�܂�
 * @param   �G���[���
 * @retval  �󂯕t�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateSetError(int no)
{

    if(_pCommState){
        _pCommState->stateError = no;
        CommSystemShutdown();
        return TRUE;
    }
    return FALSE;
}


#endif  //state�C����
