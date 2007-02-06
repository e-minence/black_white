//=============================================================================
/**
 * @file	comm_state.c
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          �X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          �J�n��I�����Ǘ�����
 * @author	k.ohno
 * @date    2006.01.12
 */
//=============================================================================

#include "gflib.h"

#include "device/wh_config.h"

#include "net_def.h"
#include "device/net_wireless.h"
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
    _GFL_NET_SetNETWL(NULL);
    pNetHandle->state = NULL;
    GFL_UI_SleepEnable(GFL_UI_SLEEP_NET);  // �X���[�v����
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
    GFI_NET_DeleteNetHandle(pNetHandle);
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

    GFL_NET_WLInitialize(pNetIni->allocNo, pNetIni->beaconGetFunc, pNetIni->beaconGetSizeFunc,
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
 * �e�@�r�[�R�����E�����ꍇ�̃R�[���o�b�N
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
 * �q�@�ҋ@���  �e�@�ɋ����炢��
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
 * �}�b�N�A�h���X���w�肵�Ďq�@�ڑ��J�n
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
 * �q�@�ҋ@���  �r�[�R�����W
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
 * �q�@�J�n  �r�[�R���̎��W�ɓ���
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
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    if(!GFL_NET_WLIsVRAMDInitialize()){
        return;
    }

    if(GFL_NET_SystemParentModeInit(TRUE, _PACKETSIZE_BATTLE,TRUE)){
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
 * �e�Ƃ��Ă̒ʐM�����J�n
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
 * �q�@�ăX�^�[�g
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
//    if(GFL_NET_SystemChildModeInit(FALSE,512)){
        rand = MATH_Rand32(&pNetHandle->sRand, (_CHILD_P_SEARCH_TIME/2))+(_CHILD_P_SEARCH_TIME/2);
        NET_PRINT("�q�@�J�n %d \n",rand);
        _CHANGE_STATE(_changeoverChildSearching, rand);
    }
}

//==============================================================================
/**
 * �e�@�ɂȂ�bcon���o
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
 * ���������G���[�̌�����ʐM���������邩�ǂ�����ݒ�
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
 * �e�Ƃ��Đڑ���
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
 * �e�@�ɂȂ�bcon���o
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _changeoverParentWait(GFL_NETHANDLE* pNetHandle)
{
    if(GFL_NET_SystemIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�

        NET_PRINT("�e�@ -- �Ȃ���\n");
//        _pCommState->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
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
 * �e�@�J�n
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
    if(GFL_NET_SystemParentModeInit(FALSE,  _PACKETSIZE_UNION,TRUE))  {
        pNetHandle->bFirstParent = FALSE;
        NET_PRINT("�e�@\n");
        _CHANGE_STATE(_changeoverParentWait, 30);
    }
}

//==============================================================================
/**
 * �q�@�ƂȂ��Đe�@��T����
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
 * �e�q�����؂�ւ��ʐM�����J�n
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
 * �q�@�̏I����҂��A�e�@���I������
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
 * �I���R�}���h �q�@���e�@�ɂ�߂�悤�ɑ��M  �S���̎q�@�ɑ���Ԃ�GFL_NET_CMD_EXIT_REQ
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
 * �I���R�}���h   �q�@���e�@�ɂ�߂�悤�ɑ��M �Ԃ��肫�� GFL_NET_CMD_EXIT
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
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION
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
 * �l�S�V�G�[�V�����p�R�[���o�b�N CS_COMM_NEGOTIATION_RETURN
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
 * �ʐM�����I���葱���J�n
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
 * �������Ȃ��X�e�[�g
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

