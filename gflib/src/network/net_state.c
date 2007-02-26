//=============================================================================
/**
 * @file	net_state.c
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
#include <dwc.h>
#include "gf_macro.h"
#include "gf_standard.h"
#include "assert.h"
#include "heapsys.h"
#include "strbuf.h"
#include "net.h"
#include "ui.h"

#include "device/wh_config.h"

#include "net_def.h"
#include "device/net_wireless.h"
#include "device/dwc_rap.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"

//==============================================================================
// ���[�N
//==============================================================================



///< �e�@�݂̂ŕK�v�ȃf�[�^
struct _NET_PARENTSYS_t{
    u8 negoCheck[GFL_NET_MACHINE_MAX];     ///< �e�q�@�̃l�S�V�G�[�V�������
    u8 dsmpChangeBuff[GFL_NET_MACHINE_MAX];   ///< �ʐM����̒ʐM���[�h�ύX�o�b�t�@
};

//==============================================================================
// ��`
//==============================================================================

#define _PACKETSIZE_DEFAULT         (512)  // �p�P�b�g�T�C�Y�̍ő�T�C�Y

#define _START_TIME (50)          /// �J�n����
#define _CHILD_P_SEARCH_TIME (32) /// �q�@�Ƃ��Đe��T������
#define _EXIT_SENDING_TIME (5)
#define _SEND_NAME_TIME (10)

#define _WIFIMODE_PARENT (-1)  // �e�Ƃ���wifi�N��

//==============================================================================
// static�錾
//==============================================================================

// �X�e�[�g�̏�����

static void _changeStateDebug(GFL_NETHANDLE* pHandle, PTRStateFunc state, int time, int line);  // �X�e�[�g��ύX����
static void _changeState(GFL_NETHANDLE* pHandle, PTRStateFunc state, int time);  // �X�e�[�g��ύX����
static void _changeoverChildSearching(GFL_NETHANDLE* pNetHandle);

#ifdef GFL_NET_DEBUG
#if 1
#define   _CHANGE_STATE(state, time)  _changeStateDebug(pNetHandle ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(pNetHandle ,state, time)
#endif
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state, time)  _changeState(pNetHandle ,state, time)
#endif //GFL_NET_DEBUG


// ���̑���ʓI�ȃX�e�[�g
static void _stateEnd(GFL_NETHANDLE* pNetHandle);             // �I������
static void _stateConnectEnd(GFL_NETHANDLE* pNetHandle);      // �ؒf�����J�n
static void _stateNone(GFL_NETHANDLE* pNetHandle);            // �������Ȃ�


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
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
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
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
 * @brief   �ʐM�Ǘ��X�e�[�g�̏���������
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

    //CommRandSeedInitialize(&pNetHandle->sRand);  //@@OO����������������
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
 * @brief   �ʐM�Ǘ��X�e�[�g�̏I������
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
    _GFL_NET_SetNETWL(NULL);
    pNetHandle->state = NULL;
    GFL_UI_SleepEnable(GFL_UI_SLEEP_NET);  // �X���[�v����
}

//==============================================================================
/**
 * @brief   �����Ă��邩�ǂ���
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
    GFI_NET_DeleteNetHandle(pNetHandle);
}


//==============================================================================
/**
 * @brief   �C�N�j���[������������ʐM�N��
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
    
    pWL = GFL_NET_WLGetHandle(pNetIni->netHeapID, pNetIni->gsid, pNetIni->maxConnectNum);
    _GFL_NET_SetNETWL(pWL);

    GFL_NET_WLInitialize(pNetIni->netHeapID, pNetIni->beaconGetFunc, pNetIni->beaconGetSizeFunc,
                         pNetIni->beaconCompFunc, pNetIni->bNetwork);

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
    GFL_UI_SleepDisable(GFL_UI_SLEEP_NET);  // �X���[�v�֎~

    GFL_NET_WLVRAMDInitialize();
    GFL_NET_CommandInitialize(NULL, 0, NULL);

    _commStateInitialize(pNetHandle, 0);
    _CHANGE_STATE(_deviceInitialize, 0);
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �e�@�ɏ��𑗐M
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
 * @brief   �e�@�r�[�R�����E�����ꍇ�̃R�[���o�b�N
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentFindCallback(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle){
        pNetHandle->timer+=60;  //�q�@���Ԃ�����
    }
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �e�@�ɋ����炢��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childConnecting(GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_WLParentBconCheck();

    if(GFL_NET_WLChildMacAddressConnect(pNetHandle->aMacAddress,_parentFindCallback,pNetHandle)){
        _CHANGE_STATE(_childSendNego, _SEND_NAME_TIME);
    }

}

//==============================================================================
/**
 * @brief   �}�b�N�A�h���X���w�肵�Ďq�@�ڑ��J�n
 * @param   connectIndex �ڑ�����e�@��Index
 * @param   bAlloc       �������[�̊m��
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectMacAddress(GFL_NETHANDLE* pNetHandle)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    GFL_NET_SystemChildModeInit(TRUE, 512);

    if(pNetIni->bMPMode){
        GFL_NET_SystemSetTransmissonTypeMP();
    }
    
    _CHANGE_STATE(_childConnecting, 0);
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �r�[�R�����W
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childScanning(GFL_NETHANDLE* pNetHandle)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
    GFL_NET_WLParentBconCheck();

    if(pNetIni->bMPMode){
        GFL_NET_SystemSetTransmissonTypeMP();
    }
}

//==============================================================================
/**
 * @brief   �q�@�J�n  �r�[�R���̎��W�ɓ���
 * @param   connectIndex �ڑ�����e�@��Index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateBeaconScan(GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_SystemChildModeInit(TRUE,512);

    _CHANGE_STATE(_childScanning, 0);
}



//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̏���
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
 * @brief   �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentWait(GFL_NETHANDLE* pHandle)
{
}

//==============================================================================
/**
 * @brief   �e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentInit(GFL_NETHANDLE* pNetHandle)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    if(!GFL_NET_WLIsVRAMDInitialize()){
        return;
    }

    if(GFL_NET_SystemParentModeInit(TRUE, _PACKETSIZE_DEFAULT,TRUE)){
        if(pNetIni->bMPMode){
            GFL_NET_SystemSetTransmissonTypeMP();
        }
        else{
            GFL_NET_SystemSetTransmissonTypeDS();
        }
        pNetHandle->negotiation = _NEGOTIATION_OK;  // �����͔F�؊���
        pNetHandle->creatureNo = 0;
        _CHANGE_STATE(_parentWait, 0);
    }
}

//==============================================================================
/**
 * @brief   �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectParent(GFL_NETHANDLE* pNetHandle,HEAPID heapID)
{
    pNetHandle->pParent = GFL_HEAP_AllocMemory(heapID, sizeof(NET_PARENTSYS));
    GFL_STD_MemClear(pNetHandle->pParent, sizeof(NET_PARENTSYS));

    _CHANGE_STATE(_parentInit, 0);
}
//==============================================================================
/**
 * @brief   �q�@�ăX�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _changeoverChildRestart(GFL_NETHANDLE* pNetHandle)
{
    u32 rand;
    
    if(!GFL_NET_WLIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // ���x�̓r�[�R�����c�����܂�

    if(GFL_NET_SystemChildModeInitAndConnect(512,_parentFindCallback,pNetHandle)){
        rand = MATH_Rand32(&pNetHandle->sRand, (_CHILD_P_SEARCH_TIME/2))+(_CHILD_P_SEARCH_TIME/2);
        NET_PRINT("�q�@�J�n %d \n",rand);
        _CHANGE_STATE(_changeoverChildSearching, rand);
    }
}

//==============================================================================
/**
 * @brief   �e�@�ɂȂ�bcon���o
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _changeoverParentRestart(GFL_NETHANDLE* pNetHandle)
{
    if( GFL_NET_WLSwitchParentChild() ){
        _CHANGE_STATE(_changeoverChildRestart, 0);
        NET_PRINT("�e�@�����\n");
    }
}


//==============================================================================
/**
 * @brief   ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

static BOOL _getErrorCheck(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle->stateError!=0){
        return TRUE;
    }
    return pNetHandle->bErrorAuto;
}


//==============================================================================
/**
 * @brief   �e�Ƃ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _changeoverParentConnect(GFL_NETHANDLE* pNetHandle)
{

    if(!_getErrorCheck(pNetHandle)){
        if(!GFL_NET_SystemIsChildsConnecting()){   // �����ȊO���Ȃ����ĂȂ��΂����������
            NET_PRINT("�e�@�����ς�\n");
            if( GFL_NET_WLSwitchParentChild() ){
                _CHANGE_STATE(_changeoverChildRestart, 0);
            }
        }
    }
    else{
        if(GFL_NET_SystemIsError()){
            NET_PRINT("------�G���[�̏ꍇReset\n");
//            _CHANGE_STATE(_ChildReset, 0);   //@@OO
            return;
        }
    }
}



//==============================================================================
/**
 * @brief   �e�@�ɂȂ�bcon���o
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _changeoverParentWait(GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_SystemIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�

        NET_PRINT("�e�@ -- �Ȃ���\n");
//        pNetHandle->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
        //WirelessIconEasy();  //@@OO
        _CHANGE_STATE(_changeoverParentConnect, 0);
        return;
    }

    if(GFL_NET_WLIsParentBeaconSent()){  // �r�[�R���𑗂�I������炵�΂炭�҂�
        if(pNetHandle->timer!=0){
            pNetHandle->timer--;
            return;
        }
    }
    else{
        return;
    }
    _CHANGE_STATE(_changeoverParentRestart, 0);
}


//==============================================================================
/**
 * @brief   �e�@�J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _changeoverParentInit(GFL_NETHANDLE* pNetHandle)
{
    if(!GFL_NET_WLIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �e�@�ɂȂ��Ă݂�
    if(GFL_NET_SystemParentModeInit(FALSE,  _PACKETSIZE_DEFAULT,TRUE))  {
        pNetHandle->bFirstParent = FALSE;
        NET_PRINT("�e�@\n");
        _CHANGE_STATE(_changeoverParentWait, 30);
    }
}

//==============================================================================
/**
 * @brief   �q�@�ƂȂ��Đe�@��T����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _changeoverChildSearching(GFL_NETHANDLE* pNetHandle)
{
    int realParent;

    GFL_NET_WLParentBconCheck();  // bcon�̌���

    if(GFL_NET_SystemGetCurrentID()!=COMM_PARENT_ID){  // �q�@�Ƃ��Đڑ�����������
        NET_PRINT("�q�@�ɂȂ���\n");
        _CHANGE_STATE(_stateNone, 0);
        return;
    }

    if(pNetHandle->timer != 0){
        NET_PRINT("time %d\n",pNetHandle->timer);
        pNetHandle->timer--;
        return;
    }

    if(!GFL_NET_WLSwitchParentChild()){
        return;
    }
    NET_PRINT("�q�@�����\n");
    _CHANGE_STATE(_changeoverParentInit, 0);
}

//==============================================================================
/**
 * @brief   �e�q�����؂�ւ��ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateChangeoverConnect(GFL_NETHANDLE* pNetHandle,HEAPID heapID)
{


//    GFL_NET_SystemChildModeInit(TRUE,512);
    GFL_NET_SystemChildModeInitAndConnect(512,_parentFindCallback,pNetHandle);
    
//    if(GFL_NET_WLChildMacAddressConnect(pNetHandle->aMacAddress)){


    _CHANGE_STATE(_changeoverChildSearching, 30);


}

//==============================================================================
/**
 * @brief   �q�@�̏I����҂��A�e�@���I������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEndParentWait(GFL_NETHANDLE* pNetHandle)
{
    if(!GFL_NET_WLIsChildsConnecting()){
        _stateFinalize(pNetHandle);
    }
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * @brief   �I���R�}���h �q�@���e�@�ɂ�߂�悤�ɑ��M  �S���̎q�@�ɑ���Ԃ�GFL_NET_CMD_EXIT_REQ
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    NET_PRINT("EXIT�R�}���h��M\n");
    if(!pNetHandle->pParent){
        return;
    }
    NET_PRINT("EXIT�R�}���h��M\n");

    GFL_NET_SendData(pNetHandle, GFL_NET_CMD_EXIT, NULL);

    _CHANGE_STATE(_stateEndParentWait, 0);


#if 0
    if(CommGetCurrentID() == COMM_PARENT_ID){
        pNetHandle->disconnectType = 0;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    else{
        pNetHandle->disconnectType = 1;
        _CHANGE_STATE(_stateWifiMatchEnd, 0);
    }
    pNetHandle->bWifiDisconnect = TRUE;
#endif
}

//==============================================================================
/**
 * @brief   �I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� GFL_NET_CMD_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvExitStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
        GFL_NET_Disconnect();
    }
}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvNegotiation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    // �e�@���󂯎���ăt���O�𗧂Ă�
    u8 retCmd[2];
    const u8 *pGet = (const u8*)pData;
    int i;

    if(!pNetHandle->pParent){
        return;
    }
    retCmd[1] = pGet[0];
    retCmd[0] = 0;

    for(i = 1 ; i < GFL_NET_MACHINE_MAX;i++){
        if(pNetHandle->pParent->negoCheck[i] == FALSE){
            pNetHandle->pParent->negoCheck[i] = TRUE;
            retCmd[0] = i;         //����i��ID�ɂȂ�
            break;
        }
    }
    NET_PRINT("------NegoRet �𑗐M %d\n",retCmd[0]);
    GFL_NET_SendData(pNetHandle, GFL_NET_CMD_NEGOTIATION_RETURN, retCmd);
    pNetHandle->negotiationID[i] = TRUE;

}

//==============================================================================
/**
 * @brief   �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION_RETURN
 * @param   callback�p����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvNegotiationReturn(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pMsg = pData;

    NET_PRINT("�ڑ��F��\n");
    if(pNetHandle->creatureNo == pMsg[1]){   // �e�@����ڑ��F�؂�����
        NET_PRINT("�ڑ��F�� OK\n");
        pNetHandle->negotiation = _NEGOTIATION_OK;
        pNetHandle->creatureNo = pMsg[0];
    }
    GF_ASSERT(pMsg[0] < GFL_NET_MACHINE_MAX);
    pNetHandle->negotiationID[pMsg[0]]=TRUE;
}

//==============================================================================
/**
 * @brief  �I��������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEnd(GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_SystemIsInitialize()){
        return;
    }
    _stateFinalize(pNetHandle);
}

//==============================================================================
/**
 * @brief  �I�������J�n
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _stateConnectEnd(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle->timer != 0){
        pNetHandle->timer--;
    }
    if(!GFL_NET_WLSwitchParentChild()){
        return;
    }
    if(pNetHandle->timer != 0){
        return;
    }
    NET_PRINT("�ؒf����");
    GFL_NET_SystemFinalize();
    _CHANGE_STATE(_stateEnd, 0);
}

//==============================================================================
/**
 * @brief   �ʐM�����I���葱���J�n
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateExit(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle->bDisconnectState){
        return;
    }
    pNetHandle->bDisconnectState = TRUE;
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * @brief   �������Ȃ��X�e�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateNone(GFL_NETHANDLE* pNetHandle)
{
    // �Ȃɂ����Ă��Ȃ�
}


///< DSMP��ԑJ�ڂ̒�`
enum{
    _TRANS_NONE,
    _TRANS_LOAD,
    _TRANS_LOAD_END,
    _TRANS_SEND,
    _TRANS_SEND_END,
};

//==============================================================================
/**
 * @brief   DSMP���[�h�ύX��Main�֐�
 * @param   pNet  �ʐM�n���h��
 * @return  noen
 */
//==============================================================================

void GFL_NET_StateTransmissonMain(GFL_NETHANDLE* pNet)
{
    BOOL bCatch=FALSE;

    switch(pNet->dsmpChange){
      case _TRANS_LOAD:
        bCatch = GFL_NET_SendData(pNet,GFL_NET_CMD_DSMP_CHANGE_REQ,&pNet->dsmpChangeType);
        if(bCatch){
            pNet->dsmpChange = _TRANS_LOAD_END;
        }
        break;
      case _TRANS_SEND:
        if(GFL_NET_SendData(pNet,GFL_NET_CMD_DSMP_CHANGE_END,&pNet->dsmpChangeType)){
            GFL_NET_SystemSetTransmissonType(pNet->dsmpChangeType);  // �؂�ւ���  �e�@�͂����Ő؂�ւ��Ȃ�
            pNet->dsmpChange = _TRANS_NONE;
        }
        break;
    }
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_StateRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(!pNetHandle->pParent){
        return;
    }
    NET_PRINT("CommRecvDSMPChange ��M\n");
    pNetHandle->dsmpChange = _TRANS_LOAD;
    pNetHandle->dsmpChangeType = pBuff[0];
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_StateRecvDSMPChangeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(pNetHandle->pParent){
        return;
    }
    pNetHandle->dsmpChangeType = pBuff[0];
    pNetHandle->dsmpChange = _TRANS_SEND;
    NET_PRINT("CommRecvDSMPChangeReq ��M\n");
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ� �I������ GFL_NET_CMD_DSMP_CHANGE_END
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_StateRecvDSMPChangeEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(!pNetHandle->pParent){
        return;
    }
    NET_PRINT("CommRecvDSMPChangeEND ��M\n");

    if(pNetHandle->dsmpChange == _TRANS_LOAD_END){
        GFL_NET_SystemSetTransmissonType(pBuff[0]);  // �؂�ւ���
        pNetHandle->dsmpChange = _TRANS_NONE;
    }
}

//--------------------------wifi------------------------------------------------

//==============================================================================
/**
 * @brief   �G���[�������������
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiErrorLoop(GFL_NETHANDLE* pNetHandle)
{
}
static void _wifiDisconnectLoop(GFL_NETHANDLE* pNetHandle)
{
}
static void _wifiFailedLoop(GFL_NETHANDLE* pNetHandle)
{
}

//==============================================================================
/**
 * @brief   �^�C���A�E�g�G���[�������������
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiTimeoutLoop(GFL_NETHANDLE* pNetHandle)
{
    int ret;
    ret = mydwc_step();
    
	if( ret < 0 ){
        // �^�C���A�E�g���ł��G���[������������G���[���[�v�Ɉړ�����
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
}

//==============================================================================
/**
 * @brief   �A�v���P�[�V������ł̐ڑ����̏���
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiApplicationConnect(GFL_NETHANDLE* pNetHandle)
{
    int ret,errCode;

    GFL_NET_SystemSetWifiConnect(TRUE);

    ret = mydwc_step();

    if(ret >= DWC_ERROR_FRIENDS_SHORTAGE){
        _CHANGE_STATE(_wifiFailedLoop, 0);
    }
    else if( ret < 0 ){        // �G���[�����B
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }		
    else if( ret == 1 ) {     // �^�C���A�E�g�i���肩��P�O�b�ȏチ�b�Z�[�W���͂��Ȃ��j
        _CHANGE_STATE(_wifiTimeoutLoop, 0);
    }
    else if(ret == 2){
        if(pNetHandle->bDisconnectError){
            _CHANGE_STATE(_wifiErrorLoop, 0);
        }
        else{
            _CHANGE_STATE(_wifiDisconnectLoop, 0);
        }
    }
}

//==============================================================================
/**
 * @brief   �G���[�R�[�h�R���o�[�g
 * @param   ret   �G���[�R�[�h
 * @retval  none
 */
//==============================================================================

static void _errcodeConvert(GFL_NETHANDLE* pNetHandle,int ret)
{
    // �G���[�����B	
    int errorcode;
    if( (ret == ERRORCODE_HEAP) || (ret == ERRORCODE_0)){
        errorcode = ret;
    }
    else {
        errorcode = -ret;
    }
    pNetHandle->errorCode = errorcode;
    NET_PRINT("�G���[���������܂����B�G���[�R�[�h(%d)\n", errorcode);
}

//==============================================================================
/**
 * @brief   �}�b�`���O�����T����
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiMachingLoop(GFL_NETHANDLE* pNetHandle)
{
    int ret = mydwc_stepmatch( 0 );

    if(ret >= DWC_ERROR_FRIENDS_SHORTAGE){
        _CHANGE_STATE(_wifiFailedLoop, 0);
    }
    else if( ret < 0 ){
        _errcodeConvert(pNetHandle, ret);
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
    else if ( ret == STEPMATCH_SUCCESS )  {
        NET_PRINT("�ΐ푊�肪������܂����B\n");
        _CHANGE_STATE(_wifiApplicationConnect, 0);
    }
    else if ( ret == STEPMATCH_CANCEL ){
        NET_PRINT("�L�����Z�����܂����B\n");	
        _CHANGE_STATE(_wifiDisconnectLoop, 0);
    }
    else if( ret == STEPMATCH_FAIL){
        _CHANGE_STATE(_wifiFailedLoop, 0);
    }
    
}

//==============================================================================
/**
 * @brief   �L�����Z����  �e����߂鎞��؂�ւ��鎞�Ɏg�p
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiBattleCanceling(GFL_NETHANDLE* pNetHandle)
{

    int ret = mydwc_stepmatch( 1 );  // �L�����Z����
				
    if( ret < 0 ){
        // �G���[�����B
        _errcodeConvert(pNetHandle, ret);
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
    else{
        if( mydwc_startgame( pNetHandle->wifiTargetNo ) ){
            GFL_NET_SystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
            if( pNetHandle->wifiTargetNo < 0 ){
                OS_TPrintf("�Q�[���Q���҂��W���܂��B\n");
            } else {
                OS_TPrintf(" %d�Ԗڂ̗F�B�ɐڑ����܂��B\n", pNetHandle->wifiTargetNo);
            }
            _CHANGE_STATE(_wifiMachingLoop, 0);
        }
        else{
            NET_PRINT(" ��Ԃ�ύX�ł��Ȃ����� ������������\n");
        }
    }
}

//==============================================================================
/**
 * @brief   �A�v���P�[�V�������J�n����ׂɑ���ɂȂ�
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target    ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
int GFL_NET_StateWifiApplicationStart( GFL_NETHANDLE* pNetHandle, int target )
{
    if( !GFL_NET_SystemWifiApplicationStart(target) ){
        return FALSE;
    }
    pNetHandle->wifiTargetNo = target;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // ���̏�Ԃ�j��
    return TRUE;
}

//==============================================================================
/**
 * @brief   �}�b�`���O�������������ǂ����𔻒�
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. �ڑ���   2. �G���[��L�����Z���Œ��f
 */
//==============================================================================
int GFL_NET_StateWifiIsMatched(GFL_NETHANDLE* pNetHandle)
{
	if( pNetHandle->state == _wifiMachingLoop ) return 0;
	if( pNetHandle->state == _wifiApplicationConnect ) return 1;
    if( pNetHandle->state == _wifiTimeoutLoop ) return 3;
    if( pNetHandle->state == _wifiDisconnectLoop ) return 4;
    if( pNetHandle->state == _wifiFailedLoop ) return 5;
    return 2;
}

//==============================================================================
/**
 * @brief   �e�@�Ƃ��đҋ@��
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiLoginLoop(GFL_NETHANDLE* pNetHandle)
{
    if( mydwc_startgame( _WIFIMODE_PARENT ) ){   //�ŏ���VCT�҂���ԂɂȂ�̂ŁA�e��ԋN��
        pNetHandle->bWifiDisconnect = FALSE;
        GFL_NET_SystemReset();
        NET_PRINT("VCT�Q���҂��W���܂�\n");
        _CHANGE_STATE(_wifiMachingLoop, 0);
    }
}

//==============================================================================
/**
 * @brief   �}�b�`���O���I������
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================


static void _stateWifiMatchEnd(GFL_NETHANDLE* pNetHandle)
{
    int ret;

    GFL_NET_SystemSetWifiConnect(FALSE);
    if(mydwc_disconnect( pNetHandle->disconnectType )){
        mydwc_returnLobby();
        _CHANGE_STATE(_wifiLoginLoop, 0);
        return;
    }
    ret = mydwc_step();
    if( ret < 0 ){
        NET_PRINT(" �G���[�����B");
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
}

//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏���
 * @param   bAuto  �������狭���ؒf����Ƃ���FALSE �ڑ��󋵂ɂ��킹��ꍇTRUE
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiMatchEnd(BOOL bAuto, GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle == NULL){  // ���łɏI�����Ă���
        return;
    }
    if(bAuto){
        if(GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID){
            pNetHandle->disconnectType = 0;
        }
        else{
            pNetHandle->disconnectType = 1;
        }
    }
    else{
        pNetHandle->disconnectType = 0;
    }
    _CHANGE_STATE(_stateWifiMatchEnd, 0);
}

//==============================================================================
/**
 * @brief   WIFI�I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� CS_WIFI_EXIT
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateRecvWifiExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    GFL_NET_StateWifiMatchEnd(TRUE, pNetHandle);
    pNetHandle->bWifiDisconnect = TRUE;
}


//==============================================================================
/**
 * @brief   WIFI�Ń}�b�`���O��؂�ꍇ�̏��� �ؒf�R�}���h�𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateSendWifiMatchEnd(GFL_NETHANDLE* pNetHandle)
{
    u8 id = GFL_NET_SystemGetCurrentID();

    GFL_NET_SendData(pNetHandle, GFL_NET_CMD_WIFI_EXIT,&id);
}


//==============================================================================
/**
 * @brief   Wifi�ؒf�R�}���h���󂯎�����ꍇTRUE
 * @param   none
 * @retval  WifiLogout�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiDisconnect(GFL_NETHANDLE* pNetHandle)
{
    return pNetHandle->bWifiDisconnect;
}

//==============================================================================
/**
 * @brief   WifiLogin�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLogin�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiLoginState(GFL_NETHANDLE* pNetHandle)
{
    u32 stateAddr = (u32)pNetHandle->state;

    if(stateAddr == (u32)_wifiLoginLoop){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   WifiLoginMatch�Ɉڍs�������ǂ����̊m�F
 * @param   none
 * @retval  WifiLoginMatch�Ɉڍs�����ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiLoginMatchState(GFL_NETHANDLE* pNetHandle)
{
    u32 stateAddr = (u32)pNetHandle->state;

    if((stateAddr == (u32)_wifiMachingLoop) || (stateAddr == (u32)_wifiLoginLoop)){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   WifiError�̏ꍇ���̔ԍ���Ԃ�  �G���[�ɂȂ��Ă邩�ǂ������m�F���Ă�������o������
 * @param   none
 * @retval  Error�ԍ�
 */
//==============================================================================

int GFL_NET_StateGetWifiErrorNo(GFL_NETHANDLE* pNetHandle)
{
    return pNetHandle->errorCode;
}

//==============================================================================
/**
 * @brief   WIFI�Ń��O�A�E�g���s���ꍇ�̏�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiLogout(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle==NULL){  // ���łɏI�����Ă���
        return;
    }
    GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_WIFI);
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}


//==============================================================================
/**
 * @brief   WifiError���ǂ���
 * @param   none
 * @retval  Error
 */
//==============================================================================

BOOL GFL_NET_StateIsWifiError(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle){
        u32 stateAddr = (u32)pNetHandle->state;
        if(stateAddr == (u32)_wifiErrorLoop){
            return TRUE;
        }
        if((stateAddr == (u32)_wifiTimeoutLoop) && pNetHandle->bDisconnectError){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �e�@��I��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiApplicationConnecting(GFL_NETHANDLE* pNetHandle)
{
    // �ڑ���
    int ret = mydwc_connect();

    if( ret < 0 ){
        _errcodeConvert(pNetHandle, ret);
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
    else if ( ret > 0 ) 
    {
        NET_PRINT("WiFi�R�l�N�V�����ɐڑ����܂����B\n	");
        _CHANGE_STATE(_wifiLoginLoop, 0);
    }
}


//==============================================================================
/**
 * @brief   wifi�Ŗ������[�^�[�ɐڑ����āA���O�C���J�n����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _wifiLoginInit(GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_SystemWiFiModeInit(_PACKETSIZE_DEFAULT,
                                  pNetHandle->netHeapID, pNetHandle->wifiHeapID,
                                  pNetHandle->pWiFiList)){
        _CHANGE_STATE(_wifiApplicationConnecting, 0);
    }
}

//==============================================================================
/**
 * @brief   wifi�}�b�`���O��Ԃփ��O�C������
 * @param   pNetHandle  GFL_NETHANDLE
 * @param   pWifiList   GFL_WIFI_FRIENDLIST
 * @param   baseHeapID  ���ɂȂ�heapid
 * @param   netHeapID   netLib�Ŋm�ێg�p����ID
 * @param   wifiHeapID  NitroWifi&NitroDWC&VCT�Ŋm�ۂ��郁����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiEnterLogin(GFL_NETHANDLE* pNetHandle, GFL_WIFI_FRIENDLIST* pWiFiList, HEAPID baseHeapID, HEAPID netHeapID, HEAPID wifiHeapID)
{
    if(GFL_NET_SystemIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_WIFI);

    _commStateInitialize(pNetHandle, 0);

    pNetHandle->baseHeapID = baseHeapID;
    pNetHandle->netHeapID = netHeapID;
    pNetHandle->wifiHeapID = wifiHeapID;
    pNetHandle->pWiFiList = pWiFiList;

    _CHANGE_STATE(_wifiLoginInit, 0);
}

//--------------------------errfunc------------------------------------------------
#define DEBUG_OHNO 0
#if DEBUG_OHNO

//==============================================================================
/**
 * @brief   ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

void CommStateSetErrorCheck(GFL_NETHANDLE* pNetHandle,BOOL bFlg,BOOL bAuto)
{
    if(pNetHandle){
        pNetHandle->bDisconnectError = bFlg;
        pNetHandle->bErrorAuto = bAuto;
        OHNO_PRINT("CommStateSetErrorCheck %d %d\n",pNetHandle->bDisconnectError,pNetHandle->bErrorAuto);
    }
    CommMPSetNoChildError(bFlg);  // �q�@�����Ȃ��Ȃ�����Č������邽�߂�ERR�����ɂ���
    CommMPSetDisconnectOtherError(bFlg);
}


//==============================================================================
/**
 * @brief   ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
 * @param   bFlg  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

BOOL CommStateGetErrorCheck(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle){
        if(pNetHandle->stateError!=0){
            return TRUE;
        }
        return pNetHandle->bErrorAuto;
    }
    return FALSE;
//    CommMPSetDisconnectOtherError(bFlg);
}

//==============================================================================
/**
 * @brief   �G���[��Ԃɓ���
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommSetErrorReset(GFL_NETHANDLE* pNetHandle,u8 type)
{
    if(pNetHandle){
        pNetHandle->ResetStateType = type;
    }
}

//==============================================================================
/**
 * @brief   ���Z�b�g�G���[��ԂɂȂ������ǂ����m�F����
 * @param   none
 * @retval  ���Z�b�g�G���[��ԂȂ�TRUE
 */
//==============================================================================

u8 CommIsResetError(GFL_NETHANDLE* pNetHandle)
{
    if(pNetHandle){
        return pNetHandle->ResetStateType;
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

void CommErrorDispCheck(GFL_NETHANDLE* pNetHandle,int heapID)
{
    if(CommStateGetErrorCheck()){
        if(CommIsError() || CommStateIsWifiError() || CommStateGetWifiDPWError()
           || (pNetHandle->stateError!=0)){
            if(!CommIsResetError()){   // ���Z�b�g�G���[��ԂŖ����ꍇ
                Snd_Stop();
                SaveData_DivSave_Cancel(pNetHandle->pSaveData); // �Z�[�u���Ă���~�߂�
                sys.tp_auto_samp = 1;  // �T���v�����O���~�߂�

                if(pNetHandle->stateError == COMM_ERROR_RESET_GTS){
                    CommSetErrorReset(COMM_ERROR_RESET_GTS);  // �G���[���Z�b�g��ԂɂȂ�
                }
                else if((pNetHandle->serviceNo == COMM_MODE_FUSIGI_WIFI)
                   || (pNetHandle->serviceNo == COMM_MODE_MYSTERY)){
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

BOOL CommStateIsResetEnd(void)
{
    if(CommMPIsConnectStalth() || !CommStateIsInitialize()){ // �ʐM�I��
        return TRUE;
    }
    if(!CommMPIsConnect()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �G���[�ɂ���ꍇ���̊֐����ĂԂƃG���[�ɂȂ�܂�
 * @param   �G���[���
 * @retval  �󂯕t�����ꍇTRUE
 */
//==============================================================================

BOOL CommStateSetError(GFL_NETHANDLE* pNetHandle,int no)
{

    if(pNetHandle){
        pNetHandle->stateError = no;
        CommSystemShutdown();
        return TRUE;
    }
    return FALSE;
}

#endif  //DEBUG_OHNO

