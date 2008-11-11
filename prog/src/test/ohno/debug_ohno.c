//============================================================================================
/**
 * @file	debug_ohno.c
 * @brief	�f�o�b�O�p�֐�
 * @author	ohno
 * @date	2006.11.29
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "debug_ohno.h"
#include "debug_field.h"
#include "system/main.h"
#include "net\network_define.h"
#include "wmi.naix"

static DEBUG_OHNO_CONTROL * DebugOhnoControl;


//------------------------------------------------------------------
//  ��M�e�[�u��
//------------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvTalkData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);



static BOOL NetTestNone(void* pCtl);
static BOOL NetTestSendTiming(void* pCtl);
static BOOL NetTestRecvTiming(void* pCtl);
static BOOL NetTestEndStart(void* pCtl);
static BOOL NetTestMoveSend(void* pCtl);

static void _connectCallBack(void* pWork);
static void* _netBeaconGetFunc(void);
static int _netBeaconGetSizeFunc(void);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
//static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork);

static const NetRecvFuncTable _CommPacketTbl[] = {
    {_RecvMoveData,         NULL},    ///NET_CMD_MOVE
    {_RecvTalkData,         NULL},    ///NET_CMD_TALK
};


typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

typedef struct{
    int x;
    int y;
    int z;
    int rot;
    int anm;
} _testMoveStruct;

static _testBeaconStruct _testBeacon = { WB_NET_DEBUG_OHNO_SERVICEID };


static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_NETWORK,  //IRC�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,  //�ʐM���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_DEBUG_OHNO_SERVICEID,  //GameServiceID
};

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

static void* _netBeaconGetFunc(void)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

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
/*
static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	OS_TPrintf("�ʐM�s�\�G���[������ ErrorNo = %d\n",errNo);
}
*/
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

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    DEBUG_OHNO_CONTROL* pDOC = pWork;

    pDOC->bMoveRecv = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ��b�R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none  
 */
//--------------------------------------------------------------

static void _RecvTalkData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(DEBUG_OHNO_CONTROL* pDOC, NetTestFunc state)
{
    pDOC->funcNet = state;
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
static void _changeStateDebug(DEBUG_OHNO_CONTROL* pDOC,NetTestFunc state, int line)
{
    NET_PRINT("df_state: %d\n",line);
    _changeState(pDOC, state);
}
#endif

#ifdef GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pDOC ,state,  __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pDOC ,state )
#endif //GFL_NET_DEBUG



//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
#if 0
static BOOL _connectCallBack(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pWork;
 //   OS_TPrintf("GFL_NET_ChangeoverConnect OK\n");
    _CHANGE_STATE( NetTestSendTiming );
}
#endif
//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestAutoConnect(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pCtl;
    
    GFL_NET_ChangeoverConnect(NULL); // �����ڑ�
    _CHANGE_STATE( NetTestSendTiming );

    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestSendTiming(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pCtl;

    if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) ){
        int id = 1 - GFL_NET_HANDLE_GetNetHandleID( GFL_NET_HANDLE_GetCurrentHandle());
        if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_GetNetHandle(id) ) ){
            GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
            _CHANGE_STATE( NetTestRecvTiming );
        }
    }
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestRecvTiming(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pCtl;
    
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),15)){
        _testMoveStruct test={1,2,3,4,5};
        NET_PRINT("TIMOK\n");
        GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),NET_CMD_MOVE, sizeof(_testMoveStruct),&test);
        _CHANGE_STATE( NetTestMoveSend );
    }
    return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �\���̓]���e�X�g
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestMoveSend(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pCtl;
    
    if(pDOC->bMoveRecv){
        NET_PRINT(" MOVE Recv\n");
        _CHANGE_STATE( NetTestEndStart );
    }
    return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _endCallBack(void* pWork)
{
    DEBUG_OHNO_CONTROL* pDOC = pWork;
    NET_PRINT("endCallBack�I��\n");
    _CHANGE_STATE( NetTestEnd );
}

//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestEndStart(void* pCtl)
{
    DEBUG_OHNO_CONTROL* pDOC = pCtl;
//    GFL_NET_Exit(_endCallBack);
    GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_CMD_EXIT_REQ, 0, NULL);
//    GFL_NET_SendData();
    _CHANGE_STATE( NetTestNone );
    return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

void _initCallBack(void* pWork)
{
    DEBUG_OHNO_CONTROL* pDOC = pWork;
    _CHANGE_STATE( NetTestAutoConnect );
}

//--------------------------------------------------------------
/**
 * @brief   �Ȃɂ����Ȃ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestNone(void* pCtl)
{
    return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �I��
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
BOOL NetTestEnd(void* pCtl)
{
    return TRUE;
}








//------------------------------------------------------------------
//  �f�o�b�O�p�������֐�
//------------------------------------------------------------------

static GFL_PROC_RESULT DebugOhnoMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	DEBUG_OHNO_CONTROL * testmode;
	HEAPID			heapID = HEAPID_OHNO_DEBUG;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x30000 );

	testmode = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), heapID );
	GFL_STD_MemClear(testmode, sizeof(DEBUG_OHNO_CONTROL));
	testmode->debug_heap_id = heapID;

    if( GFL_NET_IsInit() == FALSE ){  // �����ʐM���Ă���ꍇ�I������
        GFL_NET_Init(&aGFLNetInit, _initCallBack, testmode);
    }
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**   �f�o�b�O�p���C��
 */  
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	DEBUG_OHNO_CONTROL * testmode = mywk;

    if(testmode->funcNet){
        if(testmode->funcNet(mywk)){
            return GFL_PROC_RES_FINISH;
        }
    }
    return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**  �f�o�b�O�pExit
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_OHNO_DEBUG );

    return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugOhnoMainProcData = {
	DebugOhnoMainProcInit,
	DebugOhnoMainProcMain,
	DebugOhnoMainProcEnd,
};

