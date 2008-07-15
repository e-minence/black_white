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

#include "gflib.h"

#include "device/wih.h"

#include "net_def.h"
#include "device/net_whpipe.h"
#include "device/dwc_rap.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "net_auto.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"
#include "wm_icon.h"

//==============================================================================
// ���[�N
//==============================================================================



///< �e�@�݂̂ŕK�v�ȃf�[�^
struct _NET_PARENTSYS_t{
    u8 negoCheck[GFL_NET_MACHINE_MAX];     ///< �e�q�@�̃l�S�V�G�[�V�������
};

typedef struct _NET_WORK_STATE  GFL_NETSTATE;


/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*PTRStateFunc)(GFL_NETSTATE* pState);

struct _NET_WORK_STATE{
  NET_PARENTSYS sParent;  ///< �e�̏���ێ�����|�C���^
  PTRStateFunc state;      ///< �n���h���̃v���O�������
  GFL_STD_RandContext sRand; ///< �e�q�@�l�S�V�G�[�V�����p�����L�[
  PSyncroCallback pSCallBack;  ///< �����ڑ����ɓ��������������ꍇ�Ă΂��
  u8 aMacAddress[6];       ///< �ڑ���MAC�A�h���X�i�[�o�b�t�@  �A���C�����g���K�v!!
  u16 timer;           ///< �i�s�^�C�}�[
  u8 autoConnectNum;     ///< �����ڑ��ɕK�v�Ȑl��
  u8 bFirstParent;     ///< �J��Ԃ��e�q�؂�ւ����s���ꍇ�̍ŏ��̐e���
  u8 stateError;
  u8 bErrorAuto;
  u8 bDisconnectError;
  u8 bDisconnectState;  ///< �ؒf��Ԃɓ����Ă���ꍇTRUE
  u8 ResetStateType;   ///< �ʐM���Z�b�g��������Ƃ��̎��
};

static GFL_NETSTATE* _pNetState = NULL;

//==============================================================================
// ��`
//==============================================================================

#define _PACKETSIZE_DEFAULT         (512)  // �p�P�b�g�T�C�Y�̍ő�T�C�Y

#define _START_TIME (50)          /// �J�n����
#define _CHILD_P_SEARCH_TIME (32) /// �q�@�Ƃ��Đe��T������
#define _EXIT_SENDING_TIME (5)
#define _SEND_NAME_TIME (10)

#define _WIFIMODE_PARENT (-1)  // �e�Ƃ���wifi�N��
#define _WIFIMODE_RANDOM (-2)  // �����_���}�b�`�ڑ�

//==============================================================================
// static�錾
//==============================================================================

// �X�e�[�g�̏�����

static void _changeStateDebug(GFL_NETSTATE* pState, PTRStateFunc state, int time, int line);  // �X�e�[�g��ύX����
static void _changeState(GFL_NETSTATE* pState, PTRStateFunc state, int time);  // �X�e�[�g��ύX����
static void _changeoverChildSearching(GFL_NETSTATE* pState);
static void _errorDispCheck(GFL_NETSTATE* pState);

#ifdef GFL_NET_DEBUG
#if 1
#define   _CHANGE_STATE(state, time)  _changeStateDebug(_pNetState ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(_pNetState ,state, time)
#endif
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state, time)  _changeState(_pNetState ,state, time)
#endif //GFL_NET_DEBUG


// ���̑���ʓI�ȃX�e�[�g
static void _stateEnd(GFL_NETSTATE* pState);             // �I������
static void _stateConnectEnd(GFL_NETSTATE* pState);      // �ؒf�����J�n
static void _stateNone(GFL_NETSTATE* pState);            // �������Ȃ�


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(GFL_NETSTATE* pState,PTRStateFunc state, int time)
{
    pState->state = state;
    pState->timer = time;
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
static void _changeStateDebug(GFL_NETSTATE* pState,PTRStateFunc state, int time, int line)
{
    NET_PRINT("net_state: %d\n",line);
    _changeState(pState, state, time);
}
#endif

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̏���������
 * @param   MYSTATUS* pMyStatus
 * @retval  none
 */
//==============================================================================

static void _commStateInitialize(HEAPID heapID)
{
    // ������
    _pNetState = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_NETSTATE));
    _pNetState->timer = _START_TIME;
    _pNetState->bFirstParent = TRUE;  // �e�̏��߂Ă̋N���̏ꍇTRUE
    GFL_NET_COMMAND_Init(NULL, 0, NULL);
}

static void _endIchneumon(void* pWork, BOOL bRet)
{

}

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̏I������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateFinalize(GFL_NETSTATE* pState)
{
    if(pState==NULL){  // ���łɏI�����Ă���
        return;
    }
    GFL_NET_COMMAND_Exit();
    GFL_NET_WirelessIconEasyEnd();
    GFL_NET_ExitIchneumon(_endIchneumon,NULL);
    _GFL_NET_SetNETWL(NULL);
    GFL_HEAP_FreeMemory(_pNetState);
    _pNetState = NULL;
}

//==============================================================================
/**
 * @brief   �����Ă��邩�ǂ���
 * @param   none
 * @retval  �����Ă���ꍇTRUE
 */
//==============================================================================

static BOOL _stateIsMove(GFL_NETSTATE* pState)
{
    if(pState->state){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �C�N�j���[������������ʐM�N��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _deviceInitialize(GFL_NETSTATE* pState)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
    GFL_NETWL* pWL;
    
    if(!GFL_NET_IsIchneumon()){
        return;  //
    }

    NET_PRINT("%x - %x %x\n",(u32)pNetIni,(u32)pNetIni->beaconGetFunc,(u32)pNetIni->beaconGetSizeFunc);
    if(!pNetIni->bWiFi){
        pWL = GFL_NET_WLGetHandle(pNetIni->netHeapID, pNetIni->gsid, pNetIni->maxConnectNum);
        _GFL_NET_SetNETWL(pWL);

        GFL_NET_WLInitialize(pNetIni->netHeapID, pNetIni->beaconGetFunc, pNetIni->beaconGetSizeFunc,
                             pNetIni->beaconCompFunc);
    }
    GFL_NET_SystemReset();         // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���

    NET_PRINT("�ċN��    -- \n");
    _CHANGE_STATE(_stateNone, 0);
}

static void _initIchneumon(void* pWork, BOOL bRet)
{
    if(bRet){
        _CHANGE_STATE(_deviceInitialize, 0);
    }
}

//==============================================================================
/**
 * @brief   �ʐM�f�o�C�X������������
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateDeviceInitialize(HEAPID heapID)
{
    GFL_UI_SleepDisable(GFL_UI_SLEEP_NET);  // �X���[�v�֎~

    GFL_NET_InitIchneumon(_initIchneumon, NULL);
    _commStateInitialize(heapID);
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �e�@�ɏ��𑗐M
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childSendNego(GFL_NETSTATE* pState)
{
    //@@OO�G���[����
    if(GFL_NET_SystemIsError()){
        //NET_PRINT("�G���[�̏ꍇ�߂�\n");
     //   _CHANGE_STATE(_battleChildReset, 0);
    }
  //  if(CommIsConnect(CommGetCurrentID()) && ( GFL_NET_PARENT_NETID != CommGetCurrentID())){
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

static void _parentFindCallback(void)
{
    _pNetState->timer+=60;  //�q�@���Ԃ�����
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �e�@�ɋ����炢��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childConnecting(GFL_NETSTATE* pState)
{
    GFL_NET_WLParentBconCheck();

    if(GFL_NET_SystemChildModeInitAndConnect(FALSE, pState->aMacAddress, _PACKETSIZE_DEFAULT,_parentFindCallback)){
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

static void _childAutoConnect(GFL_NETSTATE* pState)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    if(GFL_NET_SystemChildModeInit(TRUE, 512)){
        if(pNetIni->bMPMode){
 //           GFL_NET_SystemSetTransmissonTypeMP();
        }
        
        _CHANGE_STATE(_childConnecting, 0);
    }
}

//==============================================================================
/**
 * @brief   �}�b�N�A�h���X���w�肵�Ďq�@�ڑ��J�n
 * @param   macAddress    �A�h���X
 * @param   bInit         ����������̂��ǂ���
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectMacAddress(u8* macAddress, BOOL bInit)
{
    GF_ASSERT(((u32)_pNetState->aMacAddress%2)==0);
    GFL_STD_MemCopy(macAddress, _pNetState->aMacAddress, sizeof(_pNetState->aMacAddress));
    if(bInit){
        _CHANGE_STATE(_childAutoConnect, 0);
    }
    else{
        _CHANGE_STATE(_childConnecting, 0);
    }
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �r�[�R�����W
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childScanning(GFL_NETSTATE* pState)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();
    GFL_NET_WLParentBconCheck();

    if(pNetIni->bMPMode){
//        GFL_NET_SystemSetTransmissonTypeMP();
    }
}

//==============================================================================
/**
 * @brief   �q�@�ҋ@���  �������҂�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _childIniting(GFL_NETSTATE* pState)
{
    if(GFL_NET_SystemChildModeInit(TRUE,512)){
        _CHANGE_STATE(_childScanning, 0);
    }
}

//==============================================================================
/**
 * @brief   �q�@�J�n  �r�[�R���̎��W�ɓ���
 * @param   connectIndex �ڑ�����e�@��Index
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateBeaconScan(void)
{
    _CHANGE_STATE(_childIniting, 0);
}


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̏���
 * @param
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateMainProc(void)
{
    PTRStateFunc state = _pNetState->state;
    if(state != NULL){
        state(_pNetState);
    }
    _errorDispCheck(_pNetState);
}

//==============================================================================
/**
 * @brief   �e�@�Ƃ��đҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentWait(GFL_NETSTATE* pState)
{
}

//==============================================================================
/**
 * @brief   �e�@�������ҋ@��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentInitWait(GFL_NETSTATE* pState)
{
    if(GFI_NET_SystemParentModeInitProcess()){
        _CHANGE_STATE(_parentWait, 0);
    }
}

//==============================================================================
/**
 * @brief   �e�@�Ƃ��ď��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _parentInit(GFL_NETSTATE* pState)
{
    GFLNetInitializeStruct* pNetIni = _GFL_NET_GetNETInitStruct();

    if(!GFL_NET_IsIchneumon()){
        return;
    }

    if(GFI_NET_SystemParentModeInit(TRUE, _PACKETSIZE_DEFAULT)){
//        pState->negotiation = _NEGOTIATION_OK;  // �����͔F�؊���
//        pState->creatureNo = 0;
        _CHANGE_STATE(_parentInitWait, 0);
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

void GFL_NET_StateCreateParent(HEAPID heapID)
{
//    _pNetState->pParent = GFL_HEAP_AllocClearMemory( heapID, sizeof(NET_PARENTSYS));

    //_pNetState->negotiation = _NEGOTIATION_OK;  // �����͔F�؊���
   // _pNetState->creatureNo = 0;
    _CHANGE_STATE(_parentInitWait, 0);
}

//==============================================================================
/**
 * @brief   �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectParent(HEAPID heapID)
{
    _CHANGE_STATE(_parentInit, 0);
}


//==============================================================================
/**
 * @brief   �q�@�ăX�^�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _changeoverChildRestart(GFL_NETSTATE* pState)
{
    u32 rand;
    
    if(!GFL_NET_WLIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // ���x�̓r�[�R�����c�����܂�

    
    if(GFL_NET_SystemChildModeInitAndConnect(TRUE, NULL,_PACKETSIZE_DEFAULT,_parentFindCallback)){
        rand = GFL_STD_Rand(&pState->sRand, (_CHILD_P_SEARCH_TIME));
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


static void _changeoverParentRestart(GFL_NETSTATE* pState)
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

static BOOL _getErrorCheck(GFL_NETSTATE* pState)
{
    if(pState->stateError!=0){
        return TRUE;
    }
    return pState->bErrorAuto;
}


//==============================================================================
/**
 * @brief   �e�Ƃ��Đڑ���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _changeoverParentConnect(GFL_NETSTATE* pState)
{

    if(!_getErrorCheck(pState)){
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


static void _changeoverParentWait(GFL_NETSTATE* pState)
{
    if(GFL_NET_SystemIsChildsConnecting()){   // �����ȊO���Ȃ�������e�@�Œ�

        NET_PRINT("�e�@ -- �Ȃ���\n");
//        pNetHandle->bFirstParent = TRUE;  // �e�@�Ƃ��Čq�������̂Ńt���O��߂��Ă���
        //WirelessIconEasy();  //@@OO
        {
//            GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
//            GFL_NETHANDLE* pHandleServer;
  //          pHandleServer = GFL_NET_CreateHandle();
            GFL_NET_CreateServer();   // �T�[�o
            GFI_NET_AutoParentConnectFunc();
        }
        _CHANGE_STATE(_changeoverParentConnect, 0);
        return;
    }

    if(GFL_NET_WLIsParentBeaconSent()){  // �r�[�R���𑗂�I������炵�΂炭�҂�
        if(pState->timer!=0){
            pState->timer--;
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

static void _changeoverParentInit(GFL_NETSTATE* pState)
{
    if(!GFL_NET_WLIsStateIdle()){  // �I��������������ƏI����Ă��邱�Ƃ��m�F
        return;
    }
    // �e�@�ɂȂ��Ă݂�
    if(GFI_NET_SystemParentModeInit(FALSE,  _PACKETSIZE_DEFAULT))  {
        pState->bFirstParent = FALSE;
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

static void _changeoverChildSearching(GFL_NETSTATE* pState)
{
    int realParent;

    GFL_NET_WLParentBconCheck();  // bcon�̌���

    if(GFL_NET_SystemGetCurrentID()!=GFL_NET_PARENT_NETID){  // �q�@�Ƃ��Đڑ�����������
        NET_PRINT("�q�@�ɂȂ���\n");
        _CHANGE_STATE(_stateNone, 0);
        return;
    }

    if(pState->timer != 0){
        NET_PRINT("time %d\n",pState->timer);
        pState->timer--;
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

void GFL_NET_StateChangeoverConnect(HEAPID heapID)
{
    GFL_NET_SystemChildModeInitAndConnect(TRUE, NULL,_PACKETSIZE_DEFAULT,_parentFindCallback);
    _CHANGE_STATE(_changeoverChildSearching, 30);
}

//==============================================================================
/**
 * @brief   �q�@�̏I����҂��A�e�@���I������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEndParentWait(GFL_NETSTATE* pState)
{
    if(!GFL_NET_WLIsChildsConnecting()){
        _stateFinalize(pState);
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

    GFL_NET_SendData(pNetHandle, GFL_NET_CMD_EXIT, NULL);

    _CHANGE_STATE(_stateEndParentWait, 0);


#if 0
    if(CommGetCurrentID() == GFL_NET_PARENT_NETID){
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
    if(GFL_NET_SystemGetCurrentID() != GFL_NET_PARENT_NETID){
        GFL_NET_Disconnect();
    }
}

//==============================================================================
/**
 * @brief  �I��������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateEnd(GFL_NETSTATE* pState)
{
    if(GFL_NET_SystemIsInitialize()){
        return;
    }
    _stateFinalize(pState);
}

//==============================================================================
/**
 * @brief  �I�������J�n
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _stateConnectEnd(GFL_NETSTATE* pState)
{
    if(pState->timer != 0){
        pState->timer--;
    }
    if(!GFL_NET_WLSwitchParentChild()){
        return;
    }
    if(pState->timer != 0){
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

void GFL_NET_StateExit(void)
{
    if(_pNetState && _pNetState->bDisconnectState){
        return;
    }
    _pNetState->bDisconnectState = TRUE;
    _CHANGE_STATE(_stateConnectEnd, _EXIT_SENDING_TIME);
}

//==============================================================================
/**
 * @brief   �������Ȃ��X�e�[�g
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _stateNone(GFL_NETSTATE* pState)
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
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_StateRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();

    pInit->bMPMode = pBuff[0];
    NET_PRINT("CommRecvDSMPChange ��M %d\n",pBuff[0]);
}

//--------------------------wifi------------------------------------------------
#if GFL_NET_WIFI

//==============================================================================
/**
 * @brief   �G���[�������������
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiErrorLoop(GFL_NETSTATE* pState)
{
}
static void _wifiDisconnectLoop(GFL_NETSTATE* pState)
{
}
static void _wifiFailedLoop(GFL_NETSTATE* pState)
{
}

//==============================================================================
/**
 * @brief   �^�C���A�E�g�G���[�������������
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiTimeoutLoop(GFL_NETSTATE* pState)
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

static void _wifiApplicationConnect(GFL_NETSTATE* pState)
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

static void _errcodeConvert(GFL_NETSTATE* pState,int ret)
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

static void _wifiMachingLoop(GFL_NETSTATE* pState)
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

static void _wifiBattleCanceling(GFL_NETSTATE* pState)
{
    int ret2;
    int ret = mydwc_stepmatch( 1 );  // �L�����Z����
				
    if( ret < 0 ){
        // �G���[�����B
        _errcodeConvert(pNetHandle, ret);
        _CHANGE_STATE(_wifiErrorLoop, 0);
    }
    else{
        if(pNetHandle->wifiTargetNo == _WIFIMODE_RANDOM ){
            ret2 = mydwc_startmatch();
        }
        else{
            ret2 = mydwc_startgame( pNetHandle->wifiTargetNo );
        }
        if(ret2 != 0 ){
            GFL_NET_SystemReset();   // ���܂ł̒ʐM�o�b�t�@���N���[���ɂ���
            NET_PRINT(" %d�Ԗڂ̗F�B�������̓T�[�o�ɂɐڑ�\n", pNetHandle->wifiTargetNo);
            _CHANGE_STATE(_wifiMachingLoop, 0);
        }
        else{
            NET_PRINT(" ��Ԃ�ύX�ł��Ȃ����� ������������\n");
        }
    }
}

//==============================================================================
/**
 * @brief   WIFI�w��ڑ����J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target    �Ȃ��ɂ����e�@�̔ԍ�
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
int GFL_NET_StateStartWifiPeerMatch( GFL_NETSTATE* pState, int target )
{
    GF_ASSERT(!(target < 0));

    if( !GFL_NET_SystemWifiApplicationStart(target) ){
        return FALSE;
    }
    pNetHandle->wifiTargetNo = target;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // ���̏�Ԃ�j��
    return TRUE;
}

//==============================================================================
/**
 * @brief   WIFI�����_���}�b�`���J�n����
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  TRUE      ����
 * @retval  FALSE     ���s
 */
//==============================================================================
int GFL_NET_StateStartWifiRandomMatch(void )
{
    _pNetState->wifiTargetNo = _WIFIMODE_RANDOM;
    _CHANGE_STATE(_wifiBattleCanceling,0);  // ���̏�Ԃ�j��
    return TRUE;
}

//==============================================================================
/**
 * @brief   �e�Ƃ��Ă̒ʐM�����J�n
 * @param   pNetHandle  �n���h��
 * @param   heapID      HEAPID
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateConnectWifiParent(HEAPID heapID)
{
    _CHANGE_STATE(_stateNone, 0);
}



//==============================================================================
/**
 * @brief   �}�b�`���O�������������ǂ����𔻒�
 * @param   pNetHandle   GFL_NETHANDLE
 * @param   target: ���Ȃ�e�A�O�ȏ�Ȃ�Ȃ��ɂ����e�@�̔ԍ�
 * @retval  1. �����@�@0. �ڑ���   2. �G���[��L�����Z���Œ��f
 */
//==============================================================================
int GFL_NET_StateWifiIsMatched(void)
{
	if( _pNetState->state == _wifiMachingLoop ) return 0;
	if( _pNetState->state == _wifiApplicationConnect ) return 1;
    if( _pNetState->state == _wifiTimeoutLoop ) return 3;
    if( _pNetState->state == _wifiDisconnectLoop ) return 4;
    if( _pNetState->state == _wifiFailedLoop ) return 5;
    return 2;
}

//==============================================================================
/**
 * @brief   �e�@�Ƃ��đҋ@��
 * @param   pNetHandle   GFL_NETHANDLE
 * @retval  none
 */
//==============================================================================

static void _wifiLoginLoop(GFL_NETSTATE* pState)
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


static void _stateWifiMatchEnd(GFL_NETSTATE* pState)
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

void GFL_NET_StateWifiMatchEnd(BOOL bAuto, GFL_NETSTATE* pState)
{
    if(pNetHandle == NULL){  // ���łɏI�����Ă���
        return;
    }
    if(bAuto){
        if(GFL_NET_SystemGetCurrentID() == GFL_NET_PARENT_NETID){
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

void GFL_NET_StateRecvWifiExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETSTATE* pState)
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
void GFL_NET_StateSendWifiMatchEnd(GFL_NETSTATE* pState)
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

BOOL GFL_NET_StateIsWifiDisconnect(GFL_NETSTATE* pState)
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

BOOL GFL_NET_StateIsWifiLoginState(GFL_NETSTATE* pState)
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

BOOL GFL_NET_StateIsWifiLoginMatchState(GFL_NETSTATE* pState)
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

int GFL_NET_StateGetWifiErrorNo(GFL_NETSTATE* pState)
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

void GFL_NET_StateWifiLogout(GFL_NETSTATE* pState)
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

BOOL GFL_NET_StateIsWifiError(GFL_NETSTATE* pState)
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

static void _wifiApplicationConnecting(GFL_NETSTATE* pState)
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

static void _wifiLoginInit(GFL_NETSTATE* pState)
{
    if(GFL_NET_SystemWiFiModeInit(_PACKETSIZE_DEFAULT,
                                  pNetHandle->netHeapID, pNetHandle->wifiHeapID)){
        _CHANGE_STATE(_wifiApplicationConnecting, 0);
    }
}

//==============================================================================
/**
 * @brief   wifi�}�b�`���O��Ԃփ��O�C������
 * @param   pNetHandle  GFL_NETHANDLE
 * @param   netHeapID   netLib�Ŋm�ێg�p����ID
 * @param   wifiHeapID  NitroWifi&NitroDWC&VCT�Ŋm�ۂ��郁����
 * @retval  none
 */
//==============================================================================

void GFL_NET_StateWifiEnterLogin(HEAPID netHeapID, HEAPID wifiHeapID)
{
    if(GFL_NET_SystemIsInitialize()){
        return;      // �Ȃ����Ă���ꍇ���͏��O����
    }
    GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_WIFI);
    _commStateInitialize(netHeapID);
    _CHANGE_STATE(_wifiLoginInit, 0);
}
#endif  //GFL_NET_WIFI

//--------------------------errfunc------------------------------------------------

//==============================================================================
/**
 * @brief   �����I�ɃG���[���o���s�����ǂ���
 *          (TRUE�̏ꍇ�G���[���݂���ƃu���[�X�N���[���{�ċN���ɂȂ�)
 * @param   bAuto  TRUE�Ō����J�n FALSE�ŃG���[�������s��Ȃ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_STATE_SetAutoErrorCheck(BOOL bAuto)
{
    if(_pNetState){
        _pNetState->bErrorAuto = bAuto;
    }
}

//==============================================================================
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ肷��
 * @param   bFlg    �ؒf=�G���[�ɂ���
 * @param   bAuto  TRUE�Ō����J�n
 * @retval  none
 */
//==============================================================================

void GFL_NET_STATE_SetNoChildErrorCheck(BOOL bFlg)
{
    if(_pNetState){
        _pNetState->bDisconnectError = bFlg;
    }
    GFL_NET_WLSetDisconnectOtherError(bFlg);
}

//==============================================================================
/**
 * @brief   �G���[��Ԃɓ���
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _setErrorReset(GFL_NETSTATE* pState,u8 type)
{
    if(pState){
        pState->ResetStateType = type;
    }
}

//==============================================================================
/**
 * @brief   ���Z�b�g�G���[��ԂɂȂ������ǂ����m�F����
 * @param   none
 * @retval  ���Z�b�g�G���[��ԂȂ�TRUE
 */
//==============================================================================

static u8 _isResetError(GFL_NETSTATE* pState)
{
    if(pState){
        return pState->ResetStateType;
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �G���[�����m���� �ʐM�G���[�p�E�C���h�E���o��
 * @param   heapID    �������[�m�ۂ���HEAPID
 * @param   bgl       GF_BGL_INI
 * @retval  none
 */
//--------------------------------------------------------------

static void _errorDispCheck(GFL_NETSTATE* pState)
{
    if(_getErrorCheck(pState)){
#if GFL_NET_WIFI
        if(GFL_NET_SystemIsError() || GFL_NET_StateGetWifiErrorNo(pState) || (pState->stateError!=0)){
#else
        if(GFL_NET_SystemIsError() || (pState->stateError!=0)){
#endif
            if(!_isResetError(pState)){   // ���Z�b�g�G���[��ԂŖ����ꍇ
                //Snd_Stop();
                //SaveData_DivSave_Cancel(pNetHandle->pSaveData); // �Z�[�u���Ă���~�߂�
                //sys.tp_auto_samp = 1;  // �T���v�����O���~�߂�

                _setErrorReset(pState,GFL_NET_ERROR_RESET_SAVEPOINT);  // �G���[���Z�b�g��ԂɂȂ�
                GFI_NET_FatalErrorFunc(GFL_NET_ERROR_RESET_SAVEPOINT);
                while(1){            }
            }
        }
    }
}


#define DEBUG_OHNO 0
#if DEBUG_OHNO



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

BOOL CommStateSetError(GFL_NETSTATE* pState,int no)
{

    if(pNetHandle){
        pNetHandle->stateError = no;
        CommSystemShutdown();
        return TRUE;
    }
    return FALSE;
}

#endif  //DEBUG_OHNO

