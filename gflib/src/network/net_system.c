//=============================================================================
/**
 * @file	net_system.c
 * @brief	�ʐM�V�X�e��
 * @author	GAME FREAK Inc.
 * @date    2006.12.05
 */
//=============================================================================

#include "gflib.h"

#include "device/wh_config.h"

#include "net_def.h"
#include "device/net_wireless.h"
#include "net_system.h"
#include "net_command.h"

#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "tool/net_tool.h"

//#include "wifi/dwc_rap.h"   //WIFI

#define _SENDRECV_LIMIT  (3)  // ���M�Ǝ�M�̐������ꂽ�ꍇ���M��}������

//==============================================================================
// extern�錾
//==============================================================================




//==============================================================================
// ��`
//==============================================================================

/// �N���C�A���g���M�p�L���[�̃o�b�t�@�T�C�Y
#define _SENDQUEUE_NUM_MAX  (100)
/// �T�[�o�[���M�p�L���[�̃o�b�t�@�T�C�Y
#define _SENDQUEUE_SERVER_NUM_MAX      (280)

//�q�@���M�o�b�t�@�̃T�C�Y    8��Ȃ��ꍇ�̔C�V���̐����o�C�g��
#define _SEND_BUFF_SIZE_CHILD  GFL_NET_CHILD_DATA_SIZE
//�q�@���M�o�b�t�@�̃T�C�Y    �S��ȉ��ڑ����̑��M�o�C�g��
#define _SEND_BUFF_SIZE_4CHILD  GFL_NET_CHILD_DATA_SIZE
// �q�@RING���M�o�b�t�@�T�C�Y
#define _SEND_RINGBUFF_SIZE_CHILD  (_SEND_BUFF_SIZE_CHILD * 22)
//�e�@���M�o�b�t�@�̃T�C�Y
#define _SEND_BUFF_SIZE_PARENT  GFL_NET_PARENT_DATA_SIZE
// �e�@RING���M�o�b�t�@�T�C�Y
#define _SEND_RINGBUFF_SIZE_PARENT  (_SEND_BUFF_SIZE_PARENT * 2)

// �q�@��M�o�b�t�@�̃T�C�Y
#define _RECV_BUFF_SIZE_CHILD  (_SEND_BUFF_SIZE_PARENT-1)
// �e�@��M�o�b�t�@�T�C�Y
#define _RECV_BUFF_SIZE_PARENT (_SEND_BUFF_SIZE_CHILD-1)



// ����������Ă��Ȃ��C�e���[�^�[�̐�
#define _NULL_ITERATE (-1)
// ���肦�Ȃ�ID
#define _INVALID_ID  (COMM_INVALID_ID)
// ���M�f�[�^���܂����炦�Ă��Ȃ�����
//#define _NODATA_SEND  (0x0e)

//VBlank�����̃^�X�N�̃v���C�I���e�B�[
#define _PRIORITY_VBLANKFUNC (0)

/** @brief �ʐM�擪�o�C�g��BIT�̈Ӗ� */
#define _SEND_NONE  (0x00)  ///< ���ő����ꍇ
#define _SEND_NEXT  (0x01)  ///< ���ő���Ȃ��ꍇ
#define _MP_DATA_HEADER (0x08)  ///< MP�f�[�^�̏ꍇBITON  DS���ɂ�OFF
#define _INVALID_HEADER  (0xff)  ///< �����������M�f�[�^���܂��Z�b�g����Ă��Ȃ��ꍇ


typedef enum{   // ���M���
    _SEND_CB_NONE,           // �Ȃɂ����Ă��Ȃ�
    _SEND_CB_FIRST_SEND,     // ����̒��ł̍ŏ��̑��M
    _SEND_CB_FIRST_SENDEND,  // �ŏ��̑��M�̃R�[���o�b�N������
} _sendCallbackCondition_e;

typedef enum{   // ���M�`��
    _MP_MODE,    // �e�q�^
    _DS_MODE,    // ����^
    _CHANGE_MODE_DSMP,  // DS����MP�ɐ؂�ւ���
    _CHANGE_MODE_MPDS,  // MP����DS�ɐ؂�ւ���
} _connectMode_e;


typedef enum {
    _DEVICE_WIRELESS,
    _DEVICE_WIFI,
} _deviceMode_e;

//==============================================================================
// ���[�N
//==============================================================================


typedef struct{
    int dataPoint; // ��M�o�b�t�@�\�񂪂���R�}���h�̃X�^�[�g�ʒu
    u8* pRecvBuff; // ��M�o�b�t�@�\�񂪂���R�}���h�̃X�^�[�g�ʒu
    u16 valSize;
    u8 valCommand;
    u8 sendID;
    u8 recvID;
} _RECV_COMMAND_PACK;


typedef struct{
    /// ----------------------------�q�@�p���e�@�pBUFF
    u8 sSendBuf[_SEND_BUFF_SIZE_4CHILD];          ///<  �q�@�̑��M�p�o�b�t�@
    u8 sSendBufRing[_SEND_RINGBUFF_SIZE_CHILD];  ///<  �q�@�̑��M�����O�o�b�t�@
    u8 sSendServerBuf[_SEND_BUFF_SIZE_PARENT];          ///<  �e�@�̑��M�p�o�b�t�@
    u8 sSendServerBufRing[_SEND_RINGBUFF_SIZE_PARENT];
    u8* pMidRecvBufRing;          ///< �󂯎��o�b�t�@���o�b�N�A�b�v���� DS��p
    u8* pServerRecvBufRing;       ///< �e�@����M�o�b�t�@
    u8* pRecvBufRing;             ///< �q�@���󂯎��o�b�t�@
    u8* pTmpBuff;                 ///< ��M�󂯓n���̂��߂̈ꎞ�o�b�t�@�|�C���^
    //----ringbuff manager
    RingBuffWork sendRing;
    RingBuffWork recvRing;                      ///< �q�@�̎�M�����O�o�b�t�@
    RingBuffWork recvMidRing[GFL_NET_MACHINE_MAX];
    RingBuffWork sendServerRing;
    RingBuffWork recvServerRing[GFL_NET_MACHINE_MAX];
    ///---quemanager �֘A
    SEND_QUEUE_MANAGER sendQueueMgr;
    SEND_QUEUE_MANAGER sendQueueMgrServer;
    ///---��M�֘A
    _RECV_COMMAND_PACK recvCommServer[GFL_NET_MACHINE_MAX];
    _RECV_COMMAND_PACK recvCommClient;
    
    //---------  �����֘A
    BOOL bWifiSendRecv;   // WIFI�̏ꍇ��������鎞�ƂƂ�Ȃ��Ƃ����K�v�Ȃ̂� �؂蕪����
    volatile int countSendRecv;   // ��������{�󂯎������| ��
    volatile int countSendRecvServer[GFL_NET_MACHINE_MAX];   // ��������{�󂯎������| ��

    //-------
    int packetSizeMax;
    u16 bitmap;   // �ڑ����Ă���@���BIT�Ǘ�
    
    //-------------------
    NET_TOOLSYS* pTool;  ///< netTOOL�̃��[�N
    GFL_NETHANDLE* pNetHandle[GFL_NET_MACHINE_MAX];
    u8 device;   ///< �f�o�C�X�؂�ւ�

//    u8 DSCountRecv[GFL_NET_MACHINE_MAX];  // ���Ԋm�F�p
    u8 DSCount; // ���Ԋm�F�p
    u8 recvDSCatchFlg[GFL_NET_MACHINE_MAX];  // �ʐM������������Ƃ��L�� DS�����p
    u8 bFirstCatch[GFL_NET_MACHINE_MAX];  // �R�}���h���͂��߂Ă���������p

    u8 transmissionNum;
    u8 transmissionSend;
    u8 transmissionType;  // �ʐM�`�� DS��MP���̊Ǘ�
  //  u8 changeService;    // �ʐM�`�Ԃ̕ύX
   // u8 sendSwitch;   // ���M�o�b�t�@�̃X�C�b�`�t���O
    u8 sendServerSwitch;   // ���M�o�b�t�@�̃X�C�b�`�t���O�i�T�[�o�p�j
    u8 bFirstCatchP2C;
   // u8 bSendNoneSend;        // �����R�}���h�𑗂�
    u8 bNextSendData;  ///
    u8 bNextSendDataServer;  ///
    u8 bAlone;    // ��l�ŒʐM�ł���悤�ɂ��郂�[�h�̎�TRUE
    u8 bWifiConnect; //WIFI�ʐM�\�ɂȂ�����TRUE
    u8 bResetState;
    u8 bError;  // �����s�\�Ȏ���TRUE
    u8 bShutDown;
    u8 bNotRecvCheck;
} _COMM_WORK_SYSTEM;

static _COMM_WORK_SYSTEM* _pComm = NULL;  ///<�@���[�N�\���̂̃|�C���^
// �e�@�ɂȂ�ꍇ��TGID �\���̂ɓ���Ă��Ȃ��̂�
// �ʐM���C�u�����[��؂����Ƃ��Ă��C���N�������g����������
static u16 _sTgid = 0;

// ���M�������Ƃ��m�F���邽�߂̃t���O
static volatile u8 _sendCallBackServer = _SEND_CB_FIRST_SENDEND;
// ����
static volatile u8 _sendCallBack = _SEND_CB_FIRST_SENDEND;


//==============================================================================
// static�錾
//==============================================================================

static void _commCommandInit(void);
static void _dataMpStep(void);
static void _updateMpDataServer(void);
static void _dataMpServerStep(void);
static void _sendCallbackFunc(BOOL result);
static void _sendServerCallback(BOOL result);
static void _commRecvCallback(u16 aid, u16 *data, u16 size);
static void _commRecvParentCallback(u16 aid, u16 *data, u16 size);

static void _updateMpData(void);

static void _recvDataFunc(void);
static void _recvDataServerFunc(void);
static BOOL _setSendData(u8* pSendBuff);
static void _setSendDataServer(u8* pSendBuff);

static BOOL _sendQueueSet(void);
static BOOL _sendServerQueueSet(void);
static void _queueSet(int restSize);
static void _queueSetServer(int restSize);
static void _spritDataSendFunc(void);

//static void _transmission(void);
static u16 _getUserMaxSendByte(void);


//==============================================================================
/**
 * @brief   �ő�ڑ��l���𓾂�
 * @return  �ő�ڑ��l��
 */
//==============================================================================

static int _getUserMaxNum(void)
{
    return 5;  // �ő�ڑ��l�� @@OO �������\���̂��瓾��
}

//==============================================================================
/**
 * @brief   �ŏ��ڑ��l���𓾂�
 * @return  �ő�ڑ��l��
 */
//==============================================================================

static int _getUserMinNum(void)
{
    return 5;  // �ŏ��ڑ��l�� @@OO �������\���̂��瓾��
}

//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̏��������܂Ƃ߂�
 * @param   packetSizeMax   �m�ۂ������p�P�b�g�T�C�Y
 * @param   heapID          �m�ۂ��Ă�����heapID
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================

static BOOL _commInit(int packetSizeMax, int heapID)
{
    void* pWork;
    int i;

    if(_pComm==NULL){
        int machineMax = _getUserMaxNum();
        NET_PRINT("_COMM_WORK_SYSTEM size %d \n", sizeof(_COMM_WORK_SYSTEM));
        _pComm = (_COMM_WORK_SYSTEM*)GFL_HEAP_AllocMemory(heapID, sizeof(_COMM_WORK_SYSTEM));
        MI_CpuClear8(_pComm, sizeof(_COMM_WORK_SYSTEM));
//        _pComm->pTool = GFL_NET_Tool_sysInit(heapID, machineMax);
        
        _pComm->packetSizeMax = packetSizeMax + 64;
        _pComm->transmissionType = _DS_MODE;
        
        _pComm->pRecvBufRing = GFL_HEAP_AllocMemory(heapID, _pComm->packetSizeMax*2); ///< �q�@���󂯎��o�b�t�@
        _pComm->pTmpBuff = GFL_HEAP_AllocMemory(heapID, _pComm->packetSizeMax);  ///< ��M�󂯓n���̂��߂̈ꎞ�o�b�t�@
        _pComm->pServerRecvBufRing = GFL_HEAP_AllocMemory(heapID, machineMax * _pComm->packetSizeMax);   ///< �󂯎��o�b�t�@���o�b�N�A�b�v����
        _pComm->pMidRecvBufRing = GFL_HEAP_AllocMemory(heapID, machineMax * _pComm->packetSizeMax);   ///< �󂯎��o�b�t�@���o�b�N�A�b�v���� DS��p
        // �L���[�̏�����

        GFL_NET_QueueManagerInitialize(&_pComm->sendQueueMgr, _SENDQUEUE_NUM_MAX, &_pComm->sendRing, heapID);
        GFL_NET_QueueManagerInitialize(&_pComm->sendQueueMgrServer, _SENDQUEUE_SERVER_NUM_MAX, &_pComm->sendServerRing, heapID);

        _commCommandInit();
    }
    
    _pComm->bitmap = 0;
    _pComm->bWifiConnect = FALSE;
    return TRUE;
}


//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̃R�}���h�Ǘ��̏��������܂Ƃ߂�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandInit(void)
{
    void* pWork;
    int i;

    // ���[�N�̏�����
//    NET_PRINT("�R�}���h�ď�����\n");
    
//    _pComm->sendSwitch = 0;
   // _pComm->sendServerSwitch = 0;
    
    // �e�@�݂̂̑���M
    {
        int machineMax = _getUserMaxNum();

        NET_PRINT("packet %d %d\n",_pComm->packetSizeMax,machineMax);
        MI_CpuFill8(_pComm->pServerRecvBufRing, 0, _pComm->packetSizeMax * machineMax);
        for(i = 0; i< machineMax;i++){
            GFL_NET_RingInitialize(&_pComm->recvServerRing[i],
                               &_pComm->pServerRecvBufRing[i*_pComm->packetSizeMax],
                               _pComm->packetSizeMax);
//            GFL_NET_RingInitialize(&_pComm->recvServerRingUndo[i],
//                               &_pComm->pServerRecvBufRing[i*_pComm->packetSizeMax],
//                               _pComm->packetSizeMax);
        }

        MI_CpuFill8(_pComm->pMidRecvBufRing, 0, _pComm->packetSizeMax * machineMax );
        for(i = 0; i < machineMax; i++){
            GFL_NET_RingInitialize(&_pComm->recvMidRing[i],
                               &_pComm->pMidRecvBufRing[i * _pComm->packetSizeMax],
                               _pComm->packetSizeMax);
        }
    }
    MI_CpuFill8(_pComm->sSendServerBufRing, 0, _SEND_RINGBUFF_SIZE_PARENT);
    GFL_NET_RingInitialize(&_pComm->sendServerRing, _pComm->sSendServerBufRing,
                       _SEND_RINGBUFF_SIZE_PARENT);
    for(i = 0; i < _SEND_BUFF_SIZE_PARENT; i++){
        _pComm->sSendServerBuf[i] = GFL_NET_CMD_NONE;
    }
    // �q�@�̑���M
    MI_CpuFill8(_pComm->sSendBufRing, 0, _SEND_RINGBUFF_SIZE_CHILD);
    GFL_NET_RingInitialize(&_pComm->sendRing, _pComm->sSendBufRing, _SEND_RINGBUFF_SIZE_CHILD);

    _pComm->sSendBuf[0] = _INVALID_HEADER;
    for(i = 1;i < _SEND_BUFF_SIZE_4CHILD;i++){
        _pComm->sSendBuf[i] = GFL_NET_CMD_NONE;
    }
    MI_CpuFill8(_pComm->pRecvBufRing, 0, _pComm->packetSizeMax*2);
    GFL_NET_RingInitialize(&_pComm->recvRing, _pComm->pRecvBufRing, _pComm->packetSizeMax*2);

    _pComm->bNextSendData = FALSE;
    _pComm->bNextSendDataServer = FALSE;
    for(i = 0; i< GFL_NET_MACHINE_MAX;i++){
  //      _pComm->DSCountRecv[i] = 0xff;
        _pComm->recvDSCatchFlg[i] = 0;  // �ʐM������������Ƃ��L��
        _pComm->bFirstCatch[i] = TRUE;
         _pComm->recvCommServer[i].valCommand = GFL_NET_CMD_NONE;
        _pComm->recvCommServer[i].valSize = 0xffff;
        _pComm->recvCommServer[i].pRecvBuff = NULL;
        _pComm->recvCommServer[i].dataPoint = 0;
        _pComm->countSendRecvServer[i] = 0;
    }
    _pComm->countSendRecv = 0;
    _pComm->bWifiSendRecv = TRUE;
    _pComm->recvCommClient.valCommand = GFL_NET_CMD_NONE;
    _pComm->recvCommClient.valSize = 0xffff;
    _pComm->recvCommClient.pRecvBuff = NULL;
    _pComm->recvCommClient.dataPoint = 0;

    _pComm->bFirstCatchP2C = TRUE;
    //_pComm->bSendNoneSend = TRUE;
    
    _sendCallBackServer = _SEND_CB_FIRST_SENDEND;
    _sendCallBack = _SEND_CB_FIRST_SENDEND;

        // �L���[�̃��Z�b�g
    GFL_NET_QueueManagerReset(&_pComm->sendQueueMgr);
    GFL_NET_QueueManagerReset(&_pComm->sendQueueMgrServer);
    _pComm->bResetState = FALSE;
}

//==============================================================================
/**
 * @brief   �e�q���ʁADSMP����������ꍇ�ɌĂ΂��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _commCommandInitChange2(void)
{
    void* pWork;
    int i;

//    _pComm->randPadType = 0;
//    _pComm->randPadStep = 0;
//    _pComm->sendSwitch = 0;
//    _pComm->sendServerSwitch = 0;
    
    // �e�@�݂̂̑���M
    {
        int machineMax = _getUserMaxNum();

        NET_PRINT("packet %d %d\n",_pComm->packetSizeMax,machineMax);
        MI_CpuFill8(_pComm->pServerRecvBufRing, 0, _pComm->packetSizeMax * machineMax);
        for(i = 0; i< machineMax;i++){
            GFL_NET_RingInitialize(&_pComm->recvServerRing[i],
                               &_pComm->pServerRecvBufRing[i*_pComm->packetSizeMax],
                               _pComm->packetSizeMax);
        }

        MI_CpuFill8(_pComm->pMidRecvBufRing, 0, _pComm->packetSizeMax * machineMax );
        for(i = 0; i < machineMax; i++){
            GFL_NET_RingInitialize(&_pComm->recvMidRing[i],
                               &_pComm->pMidRecvBufRing[i * _pComm->packetSizeMax],
                               _pComm->packetSizeMax);
        }
    }
    MI_CpuFill8(_pComm->sSendServerBufRing, 0, _SEND_RINGBUFF_SIZE_PARENT);
    GFL_NET_RingInitialize(&_pComm->sendServerRing, _pComm->sSendServerBufRing,
                       _SEND_RINGBUFF_SIZE_PARENT);
    for(i = 0; i < _SEND_BUFF_SIZE_PARENT; i++){
        _pComm->sSendServerBuf[i] = GFL_NET_CMD_NONE;
    }
    // �q�@�̑���M
    MI_CpuFill8(_pComm->sSendBufRing, 0, _SEND_RINGBUFF_SIZE_CHILD);
    GFL_NET_RingInitialize(&_pComm->sendRing, _pComm->sSendBufRing, _SEND_RINGBUFF_SIZE_CHILD);

    _pComm->sSendBuf[0] = _INVALID_HEADER;
    for(i = 1;i < _SEND_BUFF_SIZE_4CHILD;i++){
        _pComm->sSendBuf[i] = GFL_NET_CMD_NONE;
    }
    MI_CpuFill8(_pComm->pRecvBufRing, 0, _pComm->packetSizeMax*2);
    GFL_NET_RingInitialize(&_pComm->recvRing, _pComm->pRecvBufRing, _pComm->packetSizeMax*2);

    _pComm->bNextSendData = FALSE;
    _pComm->bNextSendDataServer = FALSE;
    for(i = 0; i< GFL_NET_MACHINE_MAX;i++){
        _pComm->recvDSCatchFlg[i] = 0;  // �ʐM������������Ƃ��L��
        _pComm->bFirstCatch[i] = TRUE;
        _pComm->recvCommServer[i].valCommand = GFL_NET_CMD_NONE;
        _pComm->recvCommServer[i].valSize = 0xffff;
        _pComm->recvCommServer[i].pRecvBuff = NULL;
        _pComm->recvCommServer[i].dataPoint = 0;
    }
    _pComm->bWifiSendRecv = TRUE;
    _pComm->recvCommClient.valCommand = GFL_NET_CMD_NONE;
    _pComm->recvCommClient.valSize = 0xffff;
    _pComm->recvCommClient.pRecvBuff = NULL;
    _pComm->recvCommClient.dataPoint = 0;

    _pComm->bFirstCatchP2C = TRUE;
    //_pComm->bSendNoneSend = TRUE;
    
//    _sendCallBackServer = _SEND_CB_SECOND_SENDEND;
//    _sendCallBack = _SEND_CB_SECOND_SENDEND;

        // �L���[�̃��Z�b�g
    GFL_NET_QueueManagerReset(&_pComm->sendQueueMgr);
    GFL_NET_QueueManagerReset(&_pComm->sendQueueMgrServer);
    _pComm->bResetState = FALSE;
}

//==============================================================================
/**
 * @brief   �w�肳�ꂽ�q�@�̗̈���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _clearChildBuffers(int netID)
{
    _pComm->recvDSCatchFlg[netID] = 0;  // �ʐM������������Ƃ��L�� DS�����p
    _pComm->bFirstCatch[netID] = TRUE;  // �R�}���h���͂��߂Ă���������p
    _pComm->countSendRecvServer[netID]=0;  //SERVER��M

    GFL_NET_RingInitialize(&_pComm->recvMidRing[netID],
                       &_pComm->pMidRecvBufRing[netID * _pComm->packetSizeMax],
                       _pComm->packetSizeMax);

    GFL_NET_RingInitialize(&_pComm->recvServerRing[netID],
                       &_pComm->pServerRecvBufRing[netID * _pComm->packetSizeMax],
                       _pComm->packetSizeMax);

    _pComm->recvCommServer[netID].valCommand = GFL_NET_CMD_NONE;
    _pComm->recvCommServer[netID].valSize = 0xffff;
    _pComm->recvCommServer[netID].pRecvBuff = NULL;
    _pComm->recvCommServer[netID].dataPoint = 0;
}

//==============================================================================
/**
 * @brief   �w�肳�ꂽ�q�@�̗̈���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _connectFunc(void)
{
    int i;

    for(i = 1 ; i < GFL_NET_MACHINE_MAX ; i++){
        if((!GFL_NET_SystemIsConnect(i)) && !_pComm->bFirstCatch[i]){
            if(!GFL_NET_SystemGetAloneMode()){
                NET_PRINT("��ڑ��ɂȂ������� %d\n",i);
                _clearChildBuffers(i);  // ��ڑ��ɂȂ�������
            }
        }
    }

}

//==============================================================================
/**
 * @brief   �e�@�̏��������s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ���łɏ������ς݂Ƃ��ē���
 * @param   regulationNo  �Q�[���̎��
 * @param   bTGIDChange  �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @param   bEntry  �q�@���󂯓���邩�ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemParentModeInit(BOOL bTGIDChange, int packetSizeMax, BOOL bEntry)
{
    BOOL ret = TRUE;

    ret = GFL_NET_WLParentInit(bTGIDChange, bEntry, _clearChildBuffers);
    GFL_NET_WLSetRecvCallback( _commRecvParentCallback );
    _commInit(packetSizeMax, GFL_HEAPID_SYSTEM); //@@OO��ŊO���ɏo��
    return ret;
}

//==============================================================================
/**
 * @brief   �q�@�̏��������s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildModeInit(BOOL bBconInit, int packetSizeMax)
{
    BOOL ret = TRUE;

    ret = GFL_NET_WLChildInit(bBconInit);
    GFL_NET_WLSetRecvCallback( _commRecvCallback );

    _commInit(packetSizeMax, GFL_HEAPID_SYSTEM);
    _sendCallBack = _SEND_CB_FIRST_SENDEND;

    return ret;
}

//==============================================================================
/**
 * @brief   �q�@�̏��������s��+ �ڑ��ɂ����s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildModeInitAndConnect(int packetSizeMax,_PARENTFIND_CALLBACK pCallback,GFL_NETHANDLE* pHandle)
{
    BOOL ret = TRUE;
    u8 mac[6]={0xff,0xff,0xff,0xff,0xff,0xff};

//    ret = GFL_NET_WLChildInit(bBconInit);
//    ret = GFL_NET_WLChildMacAddressConnect(mac);

    GFL_NET_WLSetRecvCallback( _commRecvCallback );

    _commInit(packetSizeMax, GFL_HEAPID_SYSTEM);
    _sendCallBack = _SEND_CB_FIRST_SENDEND;

    ret = GFL_NET_WLChildMacAddressConnect(mac,pCallback,pHandle);
    return ret;
}


//==============================================================================
/**
 * @brief   �ʐM���[�h�ؑ�
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _transmissonTypeChange(void)
{
    int i;
    BOOL bChange = FALSE;

    if(_pComm->transmissionType == _CHANGE_MODE_DSMP){
        _pComm->transmissionType = _MP_MODE;
        bChange=TRUE;
        NET_PRINT("MP���[�h�ɂȂ�܂���\n");
    }
    if(_pComm->transmissionType == _CHANGE_MODE_MPDS){
        _pComm->transmissionType = _DS_MODE;
        bChange=TRUE;
        NET_PRINT("DS���[�h�ɂȂ�܂���\n");
    }

//    _transmission();

}

//==============================================================================
/**
 * @brief   DS���[�h�ɐ؂�ւ���
 * @param   DS���[�h�œ����ꍇ_DS_MODE MP�œ����ꍇ_MP_MODE
 * @retval  none
 */
//==============================================================================

static void _commSetTransmissonType(int type)
{
    if((_pComm->transmissionType == _MP_MODE) && (type == _DS_MODE)){
        _pComm->transmissionType = _CHANGE_MODE_MPDS;
        return;
    }
    if((_pComm->transmissionType == _DS_MODE) && (type == _MP_MODE)){
        _pComm->transmissionType = _CHANGE_MODE_DSMP;
        return;
    }
}

//==============================================================================
/**
 * @brief   DS���[�h�ɐ؂�ւ���
 * @retval  none
 */
//==============================================================================
void GFL_NET_SystemSetTransmissonTypeDS(void)
{
    _commSetTransmissonType(_DS_MODE);
}

//==============================================================================
/**
 * @brief   MP���[�h�ɐ؂�ւ���
 * @retval  none
 */
//==============================================================================
void GFL_NET_SystemSetTransmissonTypeMP(void)
{
    _commSetTransmissonType(_MP_MODE);
   // _pComm->transmissionType = _MP_MODE;
}

//==============================================================================
/**
 * @brief   �w�胂�[�h�ɐ؂�ւ���
 * @param   DS��MP���̎w�胂�[�h
 * @retval  none
 */
//==============================================================================
void GFL_NET_SystemSetTransmissonType(int type)
{
    _commSetTransmissonType(type);
}

//==============================================================================
/**
 * @brief   ���݂̃��[�h�̎擾
 * @param   none
 * @retval  _DS_MODE��_MP_MODE
 */
//==============================================================================

static int _transmissonType(void)
{
    if(_pComm->transmissionType == _CHANGE_MODE_DSMP){
        return _DS_MODE;
    }
    if(_pComm->transmissionType == _CHANGE_MODE_MPDS){
        return _MP_MODE;
    }
    return _pComm->transmissionType;
}

//==============================================================================
/**
 * @brief   ���� DS���[�h���ǂ���
 * @param   none
 * @retval  TRUE�Ȃ�DS
 */
//==============================================================================

BOOL GFL_NET_SystemIsTransmissonDSType(void)
{
    if(_DS_MODE == _transmissonType()){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�ؒf���s��
 * @param   none
 * @retval  none
 */
//==============================================================================
void GFL_NET_SystemFinalize(void)
{
    BOOL bEnd = FALSE;

    if(_pComm){
        if( _pComm->device == _DEVICE_WIFI){
            //mydwc_Logout();  // �ؒf
            bEnd = TRUE;
        }
        else{
            if(GFL_NET_WLFinalize()){
                bEnd = TRUE;
            }
        }
    }
    if(bEnd){
        NET_PRINT("�ؒf----�J������--\n");
        GFL_NET_Tool_sysEnd(_pComm->pTool);
        _pComm->pTool = NULL;
        GFL_HEAP_FreeMemory(_pComm->pRecvBufRing);
        GFL_HEAP_FreeMemory(_pComm->pTmpBuff);
        GFL_HEAP_FreeMemory(_pComm->pServerRecvBufRing);
        GFL_HEAP_FreeMemory(_pComm->pMidRecvBufRing);
        GFL_NET_QueueManagerFinalize(&_pComm->sendQueueMgrServer);
        GFL_NET_QueueManagerFinalize(&_pComm->sendQueueMgr);
        GFL_HEAP_FreeMemory(_pComm);
        _pComm = NULL;
    }
}

//==============================================================================
/**
 * @brief   �q�@ index�ڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemChildIndexConnect(u16 index,_PARENTFIND_CALLBACK pCallback,GFL_NETHANDLE* pHandle)
{
    return GFL_NET_WLChildIndexConnect(index, pCallback, pHandle);
}


//==============================================================================
/**
 * @brief   �I���R�}���h����M�����烂�[�h�ɉ����ď������s�������ؒf����
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _autoExitSystemFunc(void)
{
    if(!GFL_NET_WLIsAutoExit()){
        return;
    }
    if(GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID){   // �������e�̏ꍇ�݂�Ȃɋt�ԐM����
        if(GFL_NET_WLIsChildsConnecting()){
            return;
        }
        GFL_NET_SystemFinalize();  // �I�������ɓ���
    }
    else{   //�q�@�̏ꍇ
        GFL_NET_SystemFinalize();  // �I�������ɓ���
    }
}

//==============================================================================
/**
 * @brief   �ʐM�f�[�^�̍X�V����  �f�[�^�����W
 *    main.c   ����  vblank��ɂ����ɌĂ΂��
 * @param   none
 * @retval  �f�[�^�V�F�A�����O���������Ȃ������ꍇFALSE
 */
//==============================================================================


BOOL GFL_NET_SystemUpdateData(void)
{
    int j;

#if 0 //�ʐM�A�C�R���@�\���ł�����o�� @@OO 2006.12.13
    if(CommStateIsWifiConnect()){
        WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - DWC_GetLinkLevel());
    }
    else if(CommMPIsInitialize()){
        WirelessIconEasy_SetLevel(WM_LINK_LEVEL_3 - WM_GetLinkLevel());
    }
#endif

    if(_pComm != NULL){
        
        if(!_pComm->bShutDown){

            _transmissonTypeChange();  //�ʐM�؂�ւ�
            _dataMpStep();
            if(_transmissonType() == _MP_MODE){
                _recvDataFunc();    // �q�@�Ƃ��Ă̎󂯎�菈��
            }
            if((GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID) && (GFL_NET_SystemIsConnect(COMM_PARENT_ID)) || GFL_NET_SystemGetAloneMode() ){
                // �T�[�o�[�Ƃ��Ă̏���
                _dataMpServerStep();
            }
            if((GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID) || (_transmissonType() == _DS_MODE) || GFL_NET_SystemGetAloneMode() ){
                _recvDataServerFunc();  // �T�[�o�[���̎�M����
            }
        }
        CommMpProcess(_pComm->bitmap);
        if(GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID){
            _connectFunc();
        }
        _autoExitSystemFunc();  // �����ؒf _pComm=NULL�ɂȂ�̂Œ���
    }
    else{
        CommMpProcess(0);
    }
    //�G���[�̕\���̎d�g�݂��ł���������  @@OO  2006.12.13
//    CommErrorDispCheck(GFL_HEAPID_SYSTEM);
    return TRUE;
}


//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemReset(void)
{

    NET_PRINT("CommSystemReset\n");
    if(_pComm){
        _commCommandInit();
    }
}

//==============================================================================
/**
 * @brief   DS���[�h�ŒʐM�o�b�t�@���N���A�[����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetDS(void)
{

    NET_PRINT("CommSystemReset\n");
    if(_pComm){
        _pComm->transmissionType = _DS_MODE;
        _commCommandInit();
    }
}

//==============================================================================
/**
 * @brief   �ʐM�o�b�t�@���N���A�[����+�r�[�R���̏��������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetBattleChild(void)
{

    NET_PRINT("CommSystemReset\n");
    if(_pComm){
        _commCommandInit();
        GFL_NET_WLChildBconDataInit();
    }
}

//==============================================================================
/**
 * @brief   �f�[�^���M����
 * @param   none
 * @retval  none
 */
//==============================================================================


static void _dataMpStep(void)
{
#if 0
    if(_pComm->bSendNoneSend){
        if( _pComm->device == _DEVICE_WIFI){
#if 0        // wifi�͕���
            if( _pComm->bWifiConnect ){
                if( mydwc_sendToServer( _pComm->sSendBuf, _SEND_BUFF_SIZE_4CHILD )){
                    _pComm->bSendNoneSend = FALSE;
                }
            }
#endif
        }
        else if((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID()) &&
                 (GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID()))) || GFL_NET_SystemGetAloneMode()){
            _sendCallBack = _SEND_CB_NONE;
            _updateMpData();     // �f�[�^����M
            if(_sendCallBack != _SEND_CB_NONE){
                _pComm->bSendNoneSend = FALSE;
            }
        }
        return;
    }
#endif

    if( _pComm->device == _DEVICE_WIFI){
#if 0  //wifi����
        if( _pComm->bWifiConnect ){
            if( _pComm->bWifiSendRecv ){  // ����������Ă���ꍇ
                if( _pComm->countSendRecv > _SENDRECV_LIMIT ){  //���肷��
                    return;
                }
                if(_sendCallBack == _SEND_CB_FIRST_SENDEND){
                    _setSendData(_pComm->sSendBuf);   // ����f�[�^�������O�o�b�t�@���獷���ւ���
                    _sendCallBack = _SEND_CB_NONE;
                }
            }
            else{
                if(_sendCallBack == _SEND_CB_FIRST_SENDEND){
                    if(!_setSendData(_pComm->sSendBuf)){  // ����f�[�^�������O�o�b�t�@���獷���ւ���
                        return;  // �{���ɑ�����̂����������ꍇ
                    }
                    _sendCallBack = _SEND_CB_NONE;
                }
            }
            if( mydwc_sendToServer( _pComm->sSendBuf, _SEND_BUFF_SIZE_4CHILD )){
//                if( !_pComm->bWifiSendRecv ){  // ����������ĂȂ��ꍇ
//                    DEBUG_DUMP(&_pComm->sSendBuf, 16,"sp0");
//                }
                _sendCallBack = _SEND_CB_FIRST_SENDEND;
                _pComm->countSendRecv++;  //wifi client
            }
            else{
            }
        }
#endif //wifi����
    }
    else if(((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) &&
        (GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID()))) || GFL_NET_SystemGetAloneMode()){
        if(_sendCallBack != _SEND_CB_FIRST_SENDEND){  // 1�����������M�������Ă��Ȃ�
            NET_PRINT("�����Ƃ��ĂȂ� _sendCallBack\n");
            return;
        }
        if( _pComm->countSendRecv > _SENDRECV_LIMIT ){  //���肷��
//            NET_PRINT("�e�̓����҂�\n");
            return;
        }
        _setSendData(_pComm->sSendBuf);  // ����f�[�^�������O�o�b�t�@���獷���ւ���
        _sendCallBack = _SEND_CB_NONE;
        _updateMpData();     // �f�[�^����M
    }
}

//==============================================================================
/**
 * @brief   ����RINGBUFF����q�@�S���ɋt���M���邽��buff�ɃR�s�[
 * @param   none
 * @retval  ��������TRUE
 */
//==============================================================================

static BOOL _copyDSData(void)
{
    int mcSize;
    int machineMax;
    int i,num,bSend = FALSE;
    u8* pBuff;

    mcSize = _getUserMaxSendByte();
    machineMax = _getUserMaxNum();

    /// ����RINGBUFF����q�@�S���ɋt���M���邽��buff�ɃR�s�[
    for(i = 0; i < machineMax; i++){
        GFL_NET_RingEndChange(&_pComm->recvMidRing[i]);
        pBuff = &_pComm->sSendServerBuf[GFL_NET_DATA_HEADER + (i * mcSize)];
        pBuff[0] = _INVALID_HEADER;
        num = GFL_NET_RingGets(&_pComm->recvMidRing[i] ,
                               pBuff,
                               mcSize);
        if(pBuff[0] != _INVALID_HEADER){
            bSend = TRUE;
        }
    }
    if(bSend == FALSE){
        _pComm->sSendServerBuf[0] = _INVALID_HEADER;
        return FALSE;
    }
    _pComm->sSendServerBuf[0] = _SEND_NONE;
    _pComm->sSendServerBuf[1] = GFL_NET_WL_GetBitmap();
    _pComm->sSendServerBuf[2] = mcSize;
    return TRUE;
}


//==============================================================================
/**
 * @brief   �f�[�^���M����  �T�[�o�[��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _updateMpDataServer(void)
{
    int i;
    int debug=0;
    int mcSize ,machineMax;

    if(!_pComm){
        return;
    }
    if( _pComm->device == _DEVICE_WIFI){
        return;
    }
    mcSize = _getUserMaxSendByte();
    machineMax = _getUserMaxNum();
    if(_sendCallBackServer == _SEND_CB_NONE){
        _sendCallBackServer++;
        
        if(_transmissonType() == _DS_MODE){
            _copyDSData();
        }
        if( (GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID()))  && !GFL_NET_SystemGetAloneMode()){
            if(!GFL_NET_WL_SendData(_pComm->sSendServerBuf,
                            _SEND_BUFF_SIZE_PARENT,
                            _sendServerCallback)){
                _sendCallBackServer--;
            }
        }
        // ���M����
        if((_sendCallBackServer == _SEND_CB_FIRST_SEND) ){

            for(i = 0; i < machineMax; i++){
                if(GFL_NET_SystemIsConnect(i)){
                    _pComm->countSendRecvServer[i]++; // �eMP���M
                }
                else if(GFL_NET_SystemGetAloneMode() && (i == 0)){
                    _pComm->countSendRecvServer[i]++; // �eMP���M
                }
            }

            // �e�@���g�Ɏq�@�̓����������邽�߂����ŃR�[���o�b�N���Ă�
            _commRecvCallback(COMM_PARENT_ID,
                              (u16*)_pComm->sSendServerBuf,
                              _SEND_BUFF_SIZE_PARENT);
        }

        if( !(GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID()))  || GFL_NET_SystemGetAloneMode() ){
            // ���荞�݂������󋵂ł��������ׂ����ŃJ�E���g
            _sendCallBackServer++;
        }
    }
}

//==============================================================================
/**
 * @brief   �f�[�^���M����  �T�[�o�[��
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _dataMpServerStep(void)
{
    int i;
#if 0
    if(_pComm->bSendNoneSend){
        if( _pComm->device == _DEVICE_WIFI){
#if 0  //wifi lock
            if( GFL_NET_SystemIsConnect(COMM_PARENT_ID) ){
                if( mydwc_sendToClient( _pComm->sSendServerBuf, WH_MP_4CHILD_DATA_SIZE*2 )){
                    _pComm->bSendNoneSend = FALSE;
                    return;
                }
            }
#endif //wifi lock
        }
        else if((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) || (GFL_NET_SystemGetAloneMode()) ){
            _updateMpDataServer();
            if(_sendCallBackServer == _SEND_CB_FIRST_SENDEND){
                _pComm->bSendNoneSend = FALSE;
                return;
            }
        }
    }
#endif
    if( _pComm->device == _DEVICE_WIFI){
#if 0 //wifi����
        if( GFL_NET_SystemIsConnect(COMM_PARENT_ID) ){
            if( _pComm->bWifiSendRecv ){  // ����������Ă���ꍇ
                if(_pComm->countSendRecvServer[1] > _SENDRECV_LIMIT){ // ���M�������̏ꍇ
                    return;
                }
                if(_pComm->countSendRecvServer[0] > _SENDRECV_LIMIT){ // ���M�������̏ꍇ
                    return;
                }
                if(_sendCallBackServer == _SEND_CB_FIRST_SENDEND){
                    if(_transmissonType() == _DS_MODE){
                        _copyDSData();  //DS�ʐM�Ȃ�R�s�[
                    }
                    _sendCallBackServer = _SEND_CB_NONE;
                }
            }
            else{
                if(_sendCallBackServer == _SEND_CB_FIRST_SENDEND){
                    if(_transmissonType() == _DS_MODE){
                        if(_copyDSData()){  //DS�ʐM�Ȃ�R�s�[
                            return;
                        }
                    }
                }
                _sendCallBackServer = _SEND_CB_NONE;
            }

            if( mydwc_sendToClient( _pComm->sSendServerBuf, WH_MP_4CHILD_DATA_SIZE*2 )){
                OHNO_SP_PRINT("send %d \n",_pComm->sSendServerBuf[0]);
                _sendCallBackServer = _SEND_CB_FIRST_SENDEND;
                _pComm->countSendRecvServer[0]++; // wifi server
                _pComm->countSendRecvServer[1]++; // wifi server
            }
            else{
                NET_PRINT("mydwc_sendToClient�Ɏ��s\n");
            }
        }
#endif  //wifi����
    }
    else if((GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) || (GFL_NET_SystemGetAloneMode()) ){
        for(i = 1; i < GFL_NET_MACHINE_MAX; i++){
            if(GFL_NET_SystemIsConnect(i)){
                if(_pComm->countSendRecvServer[i] > _SENDRECV_LIMIT){ // ���M�������̏ꍇ
//                    NET_PRINT("���M������%d \n",i);
                    return;
                }
            }
            else if((i == 0) && GFL_NET_SystemGetAloneMode()){
                if(_pComm->countSendRecvServer[0] > _SENDRECV_LIMIT){ // ���M�������̏ꍇ
                    return;
                }
            }
        }
//        if(_sendCallBackServer != _SEND_CB_FIRST_SENDEND){
  //          NET_PRINT("�����Ƃ��ĂȂ�_sendCallBackServer %d\n",_sendCallBackServer);
    //        return;
      //  }
//        if( _pComm->countSendRecv > _SENDRECV_LIMIT ){  //���肷��
  //          return;
    //    }
        if(_sendCallBackServer == _SEND_CB_FIRST_SENDEND){
            if(_transmissonType() == _MP_MODE){  // DS���ɂ͂��ł�sSendServerBuf�Ƀf�[�^������
                _setSendDataServer(_pComm->sSendServerBuf);  // ����f�[�^�������O�o�b�t�@���獷���ւ���
            }
        }
        _sendCallBackServer = _SEND_CB_NONE;
        // �ŏ��̑��M����
        _updateMpDataServer();
    }
}


//#define WIFI_DUMP_TEST


//==============================================================================
/**
 * @brief   �ʐM����M�������ɌĂ΂��R�[���o�b�N �����炪�{�� ��̂�rapper
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================
static void _commRecvCallback(u16 aid, u16 *data, u16 size)
{
    u8* adr = (u8*)data;
    int i;
    int recvSize = size;

    _pComm->countSendRecv--;  //��M
#ifdef PM_DEBUG
    _pComm->countRecvNum++;
#endif
    if(adr==NULL){
        return;
    }
    if(adr[0] == _INVALID_HEADER){
        return;
    }

#ifdef WIFI_DUMP_TEST
    DEBUG_DUMP(&adr[0], _SEND_BUFF_SIZE_CHILD,"cr0");
    DEBUG_DUMP(&adr[_SEND_BUFF_SIZE_CHILD], _SEND_BUFF_SIZE_CHILD,"cr1");
#endif

    if(adr[0] & _MP_DATA_HEADER){   ///MP�f�[�^�̏ꍇ
        if(_transmissonType() == _DS_MODE){
            NET_PRINT("DS�Ȃ̂�MP�f�[�^������ \n");
            return;
        }
    }
    else if(_transmissonType() == _MP_MODE){  //DS�f�[�^�̏ꍇ
        NET_PRINT("MP�Ȃ̂�DEP�f�[�^������ \n");
        return;
    }
    _pComm->bFirstCatchP2C = FALSE;

    _pComm->bitmap = adr[1];

    if(_transmissonType() == _DS_MODE){
        int mcSize = _getUserMaxSendByte();
        int machineMax = _getUserMaxNum();
        adr += GFL_NET_DATA_HEADER;      // �w�b�_�[�P�o�C�g�ǂݔ�΂� + Bitmap�Ł[�� + �T�C�Y�f�[�^

        for(i = 0; i < machineMax; i++){
            if(adr[0] == _INVALID_HEADER){
                adr += mcSize;
                continue;
            }
            if((_pComm->bFirstCatch[i]) && (adr[0] & _SEND_NEXT)){ // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
                NET_PRINT("�A���f�[�^ parent %d \n",aid);
                adr += mcSize;
                continue;
            }
            adr++;
            GFL_NET_RingPuts(&_pComm->recvServerRing[i], adr, mcSize-1);
            adr += (mcSize-1);
            _pComm->bFirstCatch[i]=FALSE;
        }
    }
    else{   //MP�f�[�^

        recvSize = adr[2]*256;  // MP��M�T�C�Y
        recvSize += adr[3];  // MP��M�T�C�Y

        if((_pComm->bFirstCatchP2C) && (adr[0] & _SEND_NEXT)){
            // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
            NET_PRINT("�A���f�[�^ child %d \n",aid);
            DEBUG_DUMP((u8*)data,24,"cr");
            return;
        }
        if(recvSize!=0){
            DEBUG_DUMP(&adr[0], _SEND_BUFF_SIZE_CHILD,"MP");
        }
        adr += GFL_NET_DATA_HEADER;      // �w�b�_�[�P�o�C�g�ǂݔ�΂� + Bitmap�Ł[�� + �T�C�Y�f�[�^
        GFL_NET_RingPuts(&_pComm->recvRing , adr, recvSize);
    }
}


//==============================================================================
/**
 * @brief   �e�@���Ŏq�@�̒ʐM����M�������ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================

static void _commRecvParentCallback(u16 aid, u16 *data, u16 size)
{
    u8* adr = (u8*)data;
    int i;

    _pComm->countSendRecvServer[aid]--;  //SERVER��M
    if(adr==NULL){
        return;
    }
    if(adr[0] == _INVALID_HEADER){
        return;
    }

#ifdef WIFI_DUMP_TEST
    if(aid == 1){
        DEBUG_DUMP(&adr[0], _SEND_BUFF_SIZE_CHILD,"pr1");
    }
    else{
        DEBUG_DUMP(&adr[0], _SEND_BUFF_SIZE_CHILD,"pr0");
    }
#endif
    
    if((_pComm->bFirstCatch[aid]) && (adr[0] & _SEND_NEXT)){
        // �܂������f�[�^������������Ƃ��Ȃ���ԂȂ̂ɘA���f�[�^������
        NET_PRINT("�A���f�[�^ parent %d \n",aid);
        i = 0;
        DEBUG_DUMP(adr,12,"�A���f�[�^");
        return;
    }
    _pComm->bFirstCatch[aid] = FALSE;  //�����M�t���O��OFF����

    if(_transmissonType() == _DS_MODE){  //DS
        int mcSize = _getUserMaxSendByte();
        int machineMax = _getUserMaxNum();
        GFL_NET_RingPuts(&_pComm->recvMidRing[aid] , adr, mcSize);
        _pComm->recvDSCatchFlg[aid]++;  // �ʐM������������Ƃ��L��
    }else{   // MP���[�h
        adr++;
        if(_RECV_BUFF_SIZE_PARENT > GFL_NET_RingDataRestSize(&_pComm->recvServerRing[aid])){
            NET_PRINT("Error Throw:��M�I�[�o�[\n");
            return;
        }
        GFL_NET_RingPuts(&_pComm->recvServerRing[aid] , adr, _RECV_BUFF_SIZE_PARENT);
    }
}

//==============================================================================
/**
 * @brief   �ʐM�𑗐M�����Ƃ��ɌĂ΂��R�[���o�b�N
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================

static void _sendCallbackFunc(BOOL result)
{
    if(result){
        _sendCallBack++;
    }
    else{
        GF_ASSERT_MSG(0,"send failed");
    }
}

//==============================================================================
/**
 * @brief   �ʐM�𑗐M�����Ƃ��ɌĂ΂��R�[���o�b�N 
 * @param   result  ���������s
 * @retval  none
 */
//==============================================================================

static void _sendServerCallback(BOOL result)
{
    int i;
    int mcSize = _getUserMaxSendByte();
    int machineMax = _getUserMaxNum();
    
    if(result){
        _sendCallBackServer++;
//        NET_PRINT("���Mcallback server��\n");
    }
    else{
        GF_ASSERT_MSG(0,"send failed");
    }

    for(i = 0; i < machineMax; i++){
        if(_transmissonType() == _DS_MODE){             // ������
            _pComm->sSendServerBuf[GFL_NET_DATA_HEADER+(i*mcSize)] = _INVALID_HEADER;
        }
    }
}

//==============================================================================
/**
 * @brief   �f�[�^�̎��W
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _updateMpData(void)
{
    u16 i;
    u8 *adr;
    int size;

    if(!_pComm){
        return;
    }
    if( _pComm->device == _DEVICE_WIFI){
        return;
    }
    {
        int mcSize = _getUserMaxSendByte();
        int machineMax = _getUserMaxNum();
        if(GFL_NET_SystemGetAloneMode()){   // alone���[�h�̏ꍇ
            if(_sendCallBack == _SEND_CB_NONE){
                _sendCallBack++;
                _sendCallbackFunc(TRUE);
                // �q�@�̂ӂ�����镔��          // �e�@�͎����ŃR�[���o�b�N���Ă�
                _commRecvParentCallback(COMM_PARENT_ID, (u16*)_pComm->sSendBuf,
                                    mcSize);
                //_pComm->sendSwitch = 1 - _pComm->sendSwitch;
                _pComm->countSendRecv++; // MP���M�e
                return;
            }
        }
        if(GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())){
            if(!GFL_NET_SystemIsConnect(GFL_NET_SystemGetCurrentID())){
                if(GFL_NET_SystemGetCurrentID()==1){
                    NET_PRINT("�������g�̐ڑ����܂�\n");
                }
                return;
            }
            if(_sendCallBack == _SEND_CB_NONE){
                // �q�@�f�[�^���M
                if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
                    _sendCallBack++;
                    if(!GFL_NET_WL_SendData(_pComm->sSendBuf,
                                    mcSize, _sendCallbackFunc)){
                        _sendCallBack--;
                        NET_PRINT("failed WH_SendData\n");
                    }
                    else{
                        _pComm->countSendRecv++; // MP���M
                    }
                }
                else{
//                else if(GFL_NET_WLIsChildsConnecting()){         // �T�[�o�[�Ƃ��Ă̏��� �����ȊO�̒N���ɂȂ����Ă��鎞
                    _sendCallBack++;
                    _sendCallbackFunc(TRUE);
                    // �q�@�̂ӂ�����镔��          // �e�@�͎����ŃR�[���o�b�N���Ă�
                    _commRecvParentCallback(COMM_PARENT_ID, (u16*)_pComm->sSendBuf,
                                        mcSize);
                    _pComm->countSendRecv++; // MP���M
                }
            }
        }
    }
}

//==============================================================================
/**
 * @brief   �q�@���̃A�N�V�������s��ꂽ���Ƃ�ݒ�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemActionCommandSet(void)
{
//    _pComm->actionCount = _ACTION_COUNT_MOVE;
}

//==============================================================================
/**
 * @brief   ���M�L���[�ɂ��������̂𑗐M�o�b�t�@�ɓ����
 * @param   none
 * @retval  none
 */
//==============================================================================

static BOOL _setSendData(u8* pSendBuff)
{
    int i;
    int mcSize = _getUserMaxSendByte();
    int machineMax = _getUserMaxNum();
    if(_pComm->bNextSendData == FALSE){  // ���ő����ꍇ
        pSendBuff[0] = _SEND_NONE;
    }
    else{
        pSendBuff[0] = _SEND_NEXT;  // ���ő���Ȃ��ꍇ
    }
    _pComm->bNextSendData = FALSE;
#if 0
    if(GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgr) && (_transmissonType() == _MP_MODE)){
        pSendBuff[0] |= _SEND_NO_DATA;  // ����ۂȂ牽������Ȃ�
    }
#endif
    
    if(GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgr)){
        pSendBuff[0] = _INVALID_HEADER;  // ����ۂȂ牽������Ȃ�
        return FALSE;  // ������̂���������
    }
    else{
        SEND_BUFF_DATA buffData;
        buffData.size = mcSize - 1;
        buffData.pData = &pSendBuff[1];
        if(!GFL_NET_QueueGetData(&_pComm->sendQueueMgr, &buffData, TRUE)){
            _pComm->bNextSendData = TRUE;
        }
        if(_transmissonType() == _DS_MODE){
            _pComm->DSCount++;
//            NET_PRINT("DS�f�[�^�Z�b�g %d\n",_pComm->DSCount);
            pSendBuff[0] |= ((_pComm->DSCount << 4) & 0xf0);  //DS�ʐM���ԃJ�E���^
        }
    }
#if 0
    if(GFL_NET_SystemGetCurrentID()==0){
        DEBUG_DUMP(pSendBuff,mcSize,"_setSendData");
    }
#endif
    return TRUE;
}

//==============================================================================
/**
 * @brief   ���M�L���[�ɂ��������̂𑗐M�o�b�t�@�ɓ���� �T�[�o�[MP�ʐM�p
 * @param   pSendBuff ����鑗�M�o�b�t�@
 * @retval  none
 */
//==============================================================================

static void _setSendDataServer(u8* pSendBuff)
{
    int i;

    pSendBuff[0] = _MP_DATA_HEADER;


    if(_pComm->bNextSendDataServer != FALSE){
        pSendBuff[0] |= _SEND_NEXT;  // ���ő���Ȃ��ꍇ
    }

    {
        u16 bitmap = GFL_NET_WL_GetBitmap();
        int size;
        pSendBuff[1] = bitmap;

        {
            SEND_BUFF_DATA buffData;
            buffData.size = _SEND_BUFF_SIZE_PARENT - GFL_NET_DATA_HEADER;
            buffData.pData = &pSendBuff[GFL_NET_DATA_HEADER];
            if(GFL_NET_QueueGetData(&_pComm->sendQueueMgrServer, &buffData, FALSE)){
                _pComm->bNextSendDataServer = FALSE;
                size = (_SEND_BUFF_SIZE_PARENT - GFL_NET_DATA_HEADER) - buffData.size;
            }
            else{
                _pComm->bNextSendDataServer = TRUE;
                size = _SEND_BUFF_SIZE_PARENT - GFL_NET_DATA_HEADER;
            }
            pSendBuff[2] = (size >> 8) & 0xff;
            pSendBuff[3] = size & 0xff;
        }
    }
#if 0
    DEBUG_DUMP(pSendBuff, _SEND_BUFF_SIZE_PARENT, "MP SERVER SEND");
#endif
}


//==============================================================================
/**
 * @brief   ���M���\�b�h
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 * @param   size       ���M��    �R�}���h�����̏ꍇ0
 * @param   bFast       ���M��    �R�}���h�����̏ꍇ0
 * @param   myID       ���M����l�̃n���h��no
 * @param   sendID     ���鑊���ID   �S���Ȃ�0xff
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================

BOOL GFL_NET_SystemSendData(int command, const void* data, int size, BOOL bFast, int myID, int sendID)
{
    int bSave=TRUE;
    int cSize = GFL_NET_CommandGetPacketSize(command);
    SEND_QUEUE_MANAGER* pMgr;

    NET_PRINT("< ���M %d %d\n", command,GFL_NET_QueueGetNowNum(&_pComm->sendQueueMgr));

    if(GFL_NET_COMMAND_SIZE_VARIABLE == cSize){
        cSize = size;
    }
    
    if(cSize > 255){
        bSave = FALSE;
    }
    if((_transmissonType() == _MP_MODE) && (myID == 0)){
        pMgr = &_pComm->sendQueueMgrServer;
    }
    else{
        pMgr = &_pComm->sendQueueMgr;
    }
    if(GFL_NET_QueuePut(pMgr, command, (u8*)data, cSize, bFast, bSave, myID, sendID)){
        return TRUE;
    }
    NET_PRINT("-�L���[����-");
    GFL_NET_SystemSetError();
    return FALSE;
}


//==============================================================================
/**
 * @brief   ���M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================

int GFL_NET_SystemGetSendRestSize(void)
{
    return GFL_NET_RingDataRestSize(&_pComm->sendRing);
}

//==============================================================================
/**
 * @brief   �T�[�o���̑��M�o�b�t�@�c��e��
 * @retval  �T�C�Y
 */
//==============================================================================

int GFL_NET_SystemGetSendRestSize_ServerSide(void)
{
    return GFL_NET_RingDataRestSize(&_pComm->sendServerRing);
}


//==============================================================================
/**
 * @brief   �ʐM�R�[���o�b�N�̍Ō�ɌĂ� �R�}���h������
 * @param   netID   �l�b�gID
 * @retval  none
 */
//==============================================================================

static void _endCallBack(int netID,int command,int size,void* pTemp, _RECV_COMMAND_PACK* pRecvComm)
{
    GFL_NET_CommandCallBack(pRecvComm->sendID, pRecvComm->recvID, command, size, pTemp);
    pRecvComm->valCommand = GFL_NET_CMD_NONE;
    pRecvComm->valSize = 0xffff;
    pRecvComm->pRecvBuff = NULL;
    pRecvComm->dataPoint = 0;
}


//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   pRing  �����O�o�b�t�@�̃|�C���^
 * @param   netID     �������Ă���netID
 * @param   pTemp    �R�}���h���������邽�߂�temp�o�b�t�@
 * @retval  none
 */
//==============================================================================

static void _recvDataFuncSingle(RingBuffWork* pRing, int netID, u8* pTemp, _RECV_COMMAND_PACK* pRecvComm)
{
    int size;
    u8 command;
    int bkPos;
    int realbyte;
    int sendID;
    int recvID;

    
    while( GFL_NET_RingDataSize(pRing) != 0 ){
        bkPos = pRing->startPos;
        if(pRecvComm->valCommand != GFL_NET_CMD_NONE){
            command = pRecvComm->valCommand;
        }
        else{
            command = GFL_NET_RingGetByte(pRing);
            if(command == GFL_NET_CMD_NONE){
                continue;
            }
        }
        bkPos = pRing->startPos;
        pRecvComm->valCommand = command;
//        NET_PRINT("c %d\n",command);
        NET_PRINT(">>>cR %d %d %d\n", pRecvComm->valSize, GFL_NET_RingDataSize(pRing), command);
        if(pRecvComm->valSize != 0xffff){
            size = pRecvComm->valSize;
        }
        else{
            size = GFL_NET_CommandGetPacketSize(command);
            if(_pComm->bError){
                return;
            }
            if(GFL_NET_COMMAND_SIZE_VARIABLE == size){
                if( GFL_NET_RingDataSize(pRing) < 1 ){  // �c��f�[�^��1�ȉ�������
                    pRing->startPos = bkPos;
                    break;
                }
                // �T�C�Y���Ȃ��ʐM�f�[�^�͂����ɃT�C�Y�������Ă���
                size = GFL_NET_RingGetByte(pRing)*0x100;
                size += GFL_NET_RingGetByte(pRing);
                NET_PRINT("��M�T�C�Y  %d\n",size);
                bkPos = pRing->startPos; // �Q�i�߂�
            }
            pRecvComm->valSize = size;
            pRecvComm->sendID = GFL_NET_RingGetByte(pRing);
            pRecvComm->recvID = GFL_NET_RingGetByte(pRing);
            bkPos = pRing->startPos; // �Q�i�߂�
        }


        if(GFL_NET_CommandCreateBuffCheck(command)){  // ��M�o�b�t�@������ꍇ
            if(pRecvComm->pRecvBuff==NULL){
                pRecvComm->pRecvBuff = GFL_NET_CommandCreateBuffStart(command, netID, pRecvComm->valSize);
            }
            realbyte = GFL_NET_RingGets(pRing, pTemp, size - pRecvComm->dataPoint);
            NET_PRINT("id %d -- rest %d\n",netID, size - pRecvComm->dataPoint);
            if(pRecvComm->pRecvBuff){
                MI_CpuCopy8(pTemp, &pRecvComm->pRecvBuff[pRecvComm->dataPoint], realbyte);
            }
            pRecvComm->dataPoint += realbyte;
            if(pRecvComm->dataPoint >= size ){
                _endCallBack(netID, command, size, pRecvComm->pRecvBuff, pRecvComm);
            }
        }
        else{
            if( GFL_NET_RingDataSize(pRing) >= size ){
                NET_PRINT(">>>��M comm=%d id=%d -- size%d \n",command, netID, size);
                GFL_NET_RingGets(pRing, pTemp, size);
                _endCallBack(netID, command, size, (void*)pTemp, pRecvComm);
            }
            else{   // �܂��͂��Ă��Ȃ��傫���f�[�^�̏ꍇ�ʂ���
                NET_PRINT("�����҂� command %d size %d\n",command,size);
                pRing->startPos = bkPos;
                break;
            }
        }
    }
}

//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _recvDataFunc(void)
{
    int id = COMM_PARENT_ID;
    int size;
    u8 command;
    int bkPos;

    if(!_pComm){
        return;
    }
    if(_pComm->bNotRecvCheck){
        return;
    }

    GFL_NET_RingEndChange(&_pComm->recvRing);
    if(GFL_NET_RingDataSize(&_pComm->recvRing) > 0){
        // ��O�̈ʒu��ϐ��ɕۑ����Ă���
//        MI_CpuCopy8( &_pComm->recvRing,&_pComm->recvRingUndo, sizeof(RingBuffWork));
//        GFL_NET_RingStartPush(&_pComm->recvRingUndo); //start�ʒu��ۑ�
#if 0
        NET_PRINT("-��͊J�n %d %d-%d\n",id,
                   _pComm->recvRing.startPos,_pComm->recvRing.endPos);
#endif
//        NET_PRINT("�q�@��� %d \n",id);
        _recvDataFuncSingle(&_pComm->recvRing, id, _pComm->pTmpBuff, &_pComm->recvCommClient);
#if 0
        NET_PRINT("��� %d %d-%d\n",id,
                   _pComm->recvRing.startPos,_pComm->recvRing.endPos);
#endif
    }
}

//==============================================================================
/**
 * @brief   ��M�����f�[�^���v���Z�X���ɏ�������
 * @param   none
 * @retval  none
 */
//==============================================================================

static void _recvDataServerFunc(void)
{
    int id;
    int size;
    u8 command;
    int machineMax;

    if(!_pComm){
        return;
    }
    if(_pComm->bNotRecvCheck){
        return;
    }

    machineMax = _getUserMaxNum();

    for(id = 0; id < machineMax; id++){
        GFL_NET_RingEndChange(&_pComm->recvServerRing[id]);
        
        if(GFL_NET_RingDataSize(&_pComm->recvServerRing[id]) > 0){
#if 0
            NET_PRINT("��͊J�n %d %d-%d\n",id,
                       _pComm->recvServerRing[id].startPos,_pComm->recvServerRing[id].endPos);
#endif
#if 0
            NET_PRINT("�e�@���q�@%d�����\n",id);
#endif
#if 0
            NET_PRINT("DS��� %d\n",id);
#endif
            _recvDataFuncSingle(&_pComm->recvServerRing[id], id, _pComm->pTmpBuff, &_pComm->recvCommServer[id]);
        }
    }
}

//==============================================================================
/**
 * @brief   �ʐM�\��ԂȂ̂��ǂ�����Ԃ� �������R�}���h�ɂ��l�S�V�G�[�V�������܂��̏��
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//==============================================================================
BOOL GFL_NET_SystemIsConnect(u16 netID)
{
    if(!_pComm){
        return FALSE;
    }
    if( _pComm->device == _DEVICE_WIFI){
#if 0 //wifi
        if(_pComm->bWifiConnect){
            int id = mydwc_getaid();
            if(-1 != id){
                if(netID == 0){
                    return TRUE;
                }
                else if(netID == 1){
                    return TRUE;
                }
            }
        }
#endif
        return FALSE;
    }
    if(!GFL_NET_SystemIsInitialize()){
        return FALSE;
    }
    if (!GFL_NET_WL_IsConnectLowDevice(GFL_NET_SystemGetCurrentID())) {
        return FALSE;
    }
    if(GFL_NET_SystemGetCurrentID()==netID){// ������ONLINE
        return TRUE;
    }
    else if(GFL_NET_SystemGetCurrentID()==COMM_PARENT_ID){  // �e�@�̂ݎq�@����LIB�œ�����
        u16 bitmap = GFL_NET_WL_GetBitmap();
        if( bitmap & (1<<netID)){
            return TRUE;
        }
    }
    else if(_pComm->bitmap & (1<<netID)){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�\��Ԃ̐l����Ԃ�
 * @param   none
 * @retval  �ڑ��l��
 */
//==============================================================================
int GFL_NET_SystemGetConnectNum(void)
{
    int num = 0,i;

    for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
        if(GFL_NET_SystemIsConnect(i)){
            num++;
        }
    }
    return num;
}

//==============================================================================
/**
 * @brief   ���������Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �������I����Ă�����TRUE
 */
//==============================================================================
BOOL GFL_NET_SystemIsInitialize(void)
{
    if(_pComm){
        if( _pComm->device == _DEVICE_WIFI){
            return TRUE;
        }
    }
    return GFL_NET_WLIsInitialize();
}

//==============================================================================
/**
 * @brief   �T�[�o�[������q�@�ɑ���ꍇ ���M�L���[�ւ̒ǉ�
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   sendNetID
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 *                     ���̃f�[�^�͐ÓI�łȂ���΂Ȃ�Ȃ�  �o�b�t�@�ɗ��߂Ȃ�����
 * @param   byte       ���M��    �R�}���h�����̏ꍇ0
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================

#if 0
BOOL GFL_NET_SystemSetSendQueue_ServerSide(int command, const void* data, int size)
{
    if(_transmissonType() == _DS_MODE){
        return GFL_NET_QueuePut(&_pComm->sendQueueMgr, command, (u8*)data, size, TRUE, FALSE,0xf,0xf);
    }
    else{
        return GFL_NET_QueuePut(&_pComm->sendQueueMgrServer, command, (u8*)data, size, TRUE, FALSE,0xf,0xf);
    }
}
#endif

//==============================================================================
/**
 * @brief   �N���C�A���g������e�@�ɑ���ꍇ ���M�L���[�ւ̒ǉ�
 * @param   command    comm_sharing.h�ɒ�`�������x��
 * @param   data       ���M�������f�[�^ �Ȃ�����NULL
 *                     ���̃f�[�^�͐ÓI�łȂ���΂Ȃ�Ȃ�  �o�b�t�@�ɗ��߂Ȃ�����
 * @param   byte       ���M��    �R�}���h�����̏ꍇ0
 * @retval  ���M�L���[�ɓ��������ǂ���
 */
//==============================================================================

//BOOL GFL_NET_SystemSetSendQueue(int command, const void* data, int size)
//{
//    return GFL_NET_QueuePut(&_pComm->sendQueueMgr, command, (u8*)data, size, FALSE, FALSE);
//}
#if 0
enum{
    _TRANS_NONE,
    _TRANS_LOAD,
    _TRANS_LOAD_END,
    _TRANS_SEND,
    _TRANS_SEND_END,
};


static void _transmission(void)
{
    BOOL bCatch=FALSE;

    if(!_pComm){
        return;
    }
#if 0    /// @@OO ���[�h�ؑ֕����@�@���ݕ���
    switch(_pComm->transmissionNum){
      case _TRANS_LOAD:
        if(_transmissonType() == _DS_MODE){
            bCatch = GFL_NET_SystemSendFixSizeData(GFL_NET_CMD_DSMP_CHANGE_REQ,&_pComm->transmissionSend);
        }
        else{
            bCatch = GFL_NET_SystemSendData_ServerSide(GFL_NET_CMD_DSMP_CHANGE_REQ, &_pComm->transmissionSend, 1);
        }
        if(bCatch){
            _pComm->transmissionNum = _TRANS_LOAD_END;
        }
        break;
      case _TRANS_SEND:
        if(GFL_NET_SystemSendFixSizeData(GFL_NET_CMD_DSMP_CHANGE_END,&_pComm->transmissionSend)){
            _commSetTransmissonType(_pComm->transmissionSend);  // �؂�ւ���  �e�@�͂����Ő؂�ւ��Ȃ�
            _pComm->transmissionNum = _TRANS_NONE;
        }
        break;
    }
#endif
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_SystemRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
        return;
    }
    NET_PRINT("CommRecvDSMPChange ��M\n");
    _pComm->transmissionNum = _TRANS_LOAD;
    _pComm->transmissionSend = pBuff[0];
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ�
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_SystemRecvDSMPChangeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID){
        return;
    }
    _pComm->transmissionSend = pBuff[0];
    _pComm->transmissionNum = _TRANS_SEND;
    NET_PRINT("CommRecvDSMPChangeReq ��M\n");
}

//==============================================================================
/**
 * @brief   DS�ʐMMP�ʐM�̐؂�ւ� �I������ GFL_NET_CMD_DSMP_CHANGE_END
 * @param   none
 * @retval  �c�萔
 */
//==============================================================================

void GFL_NET_SystemRecvDSMPChangeEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff = pData;
    int i;

    if(GFL_NET_SystemGetCurrentID() != COMM_PARENT_ID){
        return;
    }
    NET_PRINT("CommRecvDSMPChangeEND ��M\n");

    if(_pComm->transmissionNum == _TRANS_LOAD_END){
        _commSetTransmissonType(pBuff[0]);  // �؂�ւ���
        _pComm->transmissionNum = _TRANS_NONE;
    }
}
#endif

//==============================================================================
/**
 * @brief   �����̋@��ID��Ԃ�
 * @param   
 * @retval  �����̋@��ID  �Ȃ����Ă��Ȃ��ꍇCOMM_PARENT_ID
 */
//==============================================================================

u16 GFL_NET_SystemGetCurrentID(void)
{
    if(_pComm){
        if( _pComm->device == _DEVICE_WIFI){
#if 0  //wifi
            int id = mydwc_getaid();
            if(id != -1){
                return id;
            }
#endif
        }
        else if(GFL_NET_SystemGetAloneMode()){
            return COMM_PARENT_ID;
        }
        else{
            return GFL_NET_WL_GetCurrentAid();
        }
    }
    return COMM_PARENT_ID;
}


//==============================================================================
/**
 * @brief   WH���C�u�����Ł@�ʐM��Ԃ�BIT���m�F
 * @param   none
 * @retval  �ڑ����킩��BIT�z��
 */
//==============================================================================

BOOL GFL_NET_SystemIsChildsConnecting(void)
{
    return GFL_NET_WLIsChildsConnecting();
}

//==============================================================================
/**
 * @brief   �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�̎�TRUE
 */
//==============================================================================

BOOL GFL_NET_SystemIsError(void)
{
    if(GFL_NET_SystemGetAloneMode()){  // ��l���[�h�̏ꍇ�����̓G���[�ɂ��Ȃ�
        return FALSE;
    }
    if(_pComm){
        if(_pComm->bError){
//            CommStateSetErrorCheck(TRUE,TRUE);
            return TRUE;
        }
    }
    return GFL_NET_WLIsError();
}

//==============================================================================
/**
 * @brief   �T�[�r�X�ԍ��ɑΉ������q�@���Mbyte���𓾂܂�
 * �T�[�r�X�ԍ��� include/communication/comm_def.h�ɂ���܂�
 * @param   serviceNo �T�[�r�X�ԍ�
 * @retval  �q�@�䐔
 */
//==============================================================================

static u16 _getUserMaxSendByte(void)
{
    if(_transmissonType() == _MP_MODE){
        return _SEND_BUFF_SIZE_CHILD;
    }
    return _SEND_BUFF_SIZE_4CHILD;
}

//==============================================================================
/**
 * @brief   �ő�ڑ��l���𓾂�
 * @param   none
 * @retval  �ő�ڑ��l��
 */
//==============================================================================

int GFL_NET_SystemGetMaxEntry(int service)
{
    return _getUserMaxNum();
}

//==============================================================================
/**
 * @brief   �ŏ��ڑ��l���𓾂�
 * @param   none
 * @retval  �ŏ��ڑ��l��
 */
//==============================================================================

int GFL_NET_SystemGetMinEntry(int service)
{
    return _getUserMinNum();
}

//==============================================================================
/**
 * @brief   ��l�ʐM���[�h��ݒ�
 * @param   bAlone    ��l�ʐM���[�h
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetAloneMode(BOOL bAlone)
{
    if(_pComm){
        _pComm->bAlone = bAlone;
    }
}

//==============================================================================
/**
 * @brief   ��l�ʐM���[�h���ǂ������擾
 * @param   none
 * @retval  ��l�ʐM���[�h�̏ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_SystemGetAloneMode(void)
{
    if(_pComm){
        return _pComm->bAlone;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �����I���R�}���h��M
 * @param   netID  �ʐMID
 * @param   size   �T�C�Y
 * @param   pData  �f�[�^
 * @param   pWork  ���[�N
 * @return  none
 */
//==============================================================================

void GFL_NET_SystemRecvAutoExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    u8 dummy;

    NET_PRINT("CommRecvAutoExit ��M \n");
    if(!GFL_NET_WLIsAutoExit()){
        if(GFL_NET_SystemGetCurrentID() == COMM_PARENT_ID){   // �������e�̏ꍇ�݂�Ȃɋt�ԐM����
            GFL_NET_SendData(pNetHandle,GFL_NET_CMD_EXIT_REQ, &dummy);
        }
    }
    GFL_NET_WLSetAutoExit();
}

#ifdef GFL_NET_DEBUG

//==============================================================================
/**
 * @brief   �f�o�b�O�p�Ƀ_���v��\������
 * @param   adr           �\���������A�h���X
 * @param   length        ����
 * @param   pInfoStr      �\�����������b�Z�[�W
 * @retval  �T�[�r�X�ԍ�
 */
//==============================================================================

void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr)
{
    int i,j = 0;

    NET_PRINT("%s \n",pInfoStr);
    while(length){
        NET_PRINT(">> ");
        for(i = 0 ; i < 8 ; i++){
            NET_PRINT("%2x ",adr[j]);
            j++;
            if(j == length){
                break;
            }
        }
        NET_PRINT("\n");
        if(j == length){
            break;
        }
    }
    NET_PRINT(" --end\n");
}

#endif

//==============================================================================
/**
 * @brief   ����̃R�}���h�𑗐M���I�������ǂ����𒲂ׂ�
 * @param   command ���ׂ�R�}���h
 * @retval  �R�}���h���݂�����TRUE
 */
//==============================================================================


BOOL GFL_NET_SystemIsSendCommand(int command,int myID)
{
    if((_transmissonType() == _MP_MODE) && (myID == 0)){
        return GFL_NET_QueueIsCommand(&_pComm->sendQueueMgrServer, command);
    }
    return GFL_NET_QueueIsCommand(&_pComm->sendQueueMgr, command);

}


//==============================================================================
/**
 * @brief   �L���[������ۂ��ǂ��� �T�[�o�[��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

BOOL GFL_NET_SystemIsEmptyQueue_ServerSize(void)
{
    return GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgrServer);

}

//==============================================================================
/**
 * @brief   �L���[������ۂ��ǂ���
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

BOOL GFL_NET_SystemIsEmptyQueue(void)
{
    return GFL_NET_QueueIsEmpty(&_pComm->sendQueueMgr);

}

//==============================================================================
/**
 * @brief   wifi�ڑ��������ǂ�����ݒ肷��
 * @param   none
 * @retval  �R�}���h���݂�����FALSE
 */
//==============================================================================

void GFL_NET_SystemSetWifiConnect(BOOL bConnect)
{
    // �ʐM�ҋ@��Ԃɂ��邩�ǂ�����`����
    _pComm->bWifiConnect = bConnect;
}

//==============================================================================
/**
 * @brief   VCHAT��Ԃ��ǂ���
 * @param   none
 * @retval  VCHAT��ԂȂ�TRUE
 */
//==============================================================================

BOOL GFL_NET_SystemIsVChat(void)
{
    if( _pComm->device == _DEVICE_WIFI){
#if 0  // wifi
        return mydwc_IsVChat();
#endif
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   WIFI�ʐM�𓯊��ʐM���邩�A�񓯊��ʐM���邩�̐؂�ւ����s��
 * @param   TRUE ���� FALSE �񓯊�
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetWifiBothNet(BOOL bFlg)
{
    if( _pComm->device == _DEVICE_WIFI){
        _pComm->bWifiSendRecv = bFlg;
        if(bFlg){
            _pComm->countSendRecv = 0;
            _pComm->countSendRecvServer[0] = 0;
            _pComm->countSendRecvServer[1] = 0;
        }
        NET_PRINT("oo�����؂�ւ� %d\n",bFlg);
    }
}

//==============================================================================
/**
 * @brief   �G���[�ɂ���ꍇTRUE
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemSetError(void)
{
    _pComm->bError=TRUE;
}

//==============================================================================
/**
 * @brief   �ʐM���~�߂�
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemShutdown(void)
{
    if(_pComm){
        _pComm->bShutDown = TRUE;
    }
}

//==============================================================================
/**
 * @brief   �T�[�o�[���̒ʐM�L���[�����Z�b�g����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemResetQueue_Server(void)
{
    GFL_NET_QueueManagerReset(&_pComm->sendQueueMgrServer);
}

//==============================================================================
/**
 * @brief   �ʐM�̎�M���~�߂�t���O���Z�b�g
 * @param   bFlg  TRUE�Ŏ~�߂�  FALSE�ŋ���
 * @retval  none
 */
//==============================================================================

void GFL_NET_SystemRecvStop(BOOL bFlg)
{
    if(_pComm){
        _pComm->bNotRecvCheck = bFlg;
    }
}

//==============================================================================
/**
 * @brief   �ʐM�̎�M���~�߂�t���O���Z�b�g
 * @param   bFlg  TRUE�Ŏ~�߂�  FALSE�ŋ���
 * @retval  none
 */
//==============================================================================

GFL_NETHANDLE* GFL_NET_SystemGetHandle(int NetID)
{
    return _pComm->pNetHandle[NetID];
}

