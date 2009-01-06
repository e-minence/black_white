//============================================================================================
/**
 * @file	sample_net.c
 * @brief	�ʐM����p�֐�
 */
//============================================================================================
#include "gflib.h"
#include "net\network_define.h"
#include "system\main.h"
#include "field_net.h"

//------------------------------------------------------------------
// 
//	�V�X�e���\����
//
//------------------------------------------------------------------
typedef struct {
	int				_connectSeqNo;
	int				_exitSeqNo;
	GFL_NETHANDLE*	_pHandle;
}GAMENET_SYS;

static GAMENET_SYS	gNetSys;

extern const NetRecvFuncTable NetSamplePacketTbl[1];
//------------------------------------------------------------------
// 
//	������
//
//------------------------------------------------------------------
#define _BCON_GET_NUM  (1)

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

#define _TEST_TIMING (12)

typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_FIELDMOVE_SERVICEID };

///< �r�[�R���f�[�^�擾�֐�
static void* _netBeaconGetFunc(void* pWork)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
	return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^��r�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}


static void _initCallback(void* pWork)
{
}

// �ʐM�������\����  wifi�p
static GFLNetInitializeStruct aGFLNetInit = {
    NetSamplePacketTbl,  // ��M�֐��e�[�u��
    1,//NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,   //���ʂ̒ʐM�G���[
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_FIELDMOVE_SERVICEID,  //GameServiceID
};

enum{
    _CONNECT_START = 0,
    _CONNECT,
    _CONNECT_CHECK,
    _CONNECT_NEGO,
    _CONNECT_NEGOCHECK,
    _CONNECT_TIMINGSTART,
    _CONNECT_TIMINGCHECK,
    _CONNECT_NONE,
};

enum{
    _EXIT_START = 0,
    _EXIT_WAIT,
    _EXIT_END,
    _EXIT_NONE,
};

void FieldInitGameNet(void)
{
#ifdef NET_WORK_ON
    GFL_NET_Init(&aGFLNetInit, _initCallback, NULL);
#endif
	gNetSys._connectSeqNo = _CONNECT_START;
	gNetSys._exitSeqNo = _EXIT_NONE;
	gNetSys._pHandle = NULL;
}


//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _connectCallBack(void* pWork)
{
    gNetSys._connectSeqNo = _CONNECT_TIMINGSTART;
}

//--------------------------------------------------------------
/**
 * @brief   �ؒf�����R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _disconnectCallBack(void* pWork)
{
    gNetSys._pHandle = NULL;
}

//------------------------------------------------------------------
// 
//	�ڑ�
//
//------------------------------------------------------------------


BOOL FieldConnectGameNet(void)
{
#ifdef NET_WORK_ON
	BOOL result = FALSE;

    //OS_TPrintf("ConnectGameNet%d\n",gNetSys._connectSeqNo);
	switch( gNetSys._connectSeqNo ){

	case _CONNECT_START:
		if( GFL_NET_IsInit() == TRUE ){
			gNetSys._connectSeqNo = _CONNECT;
		}
		break;

	case _CONNECT:
		GFL_NET_ChangeoverConnect(_connectCallBack); // �����ڑ�
		gNetSys._connectSeqNo = _CONNECT_NEGO;
		break;

	case _CONNECT_NEGO:
		break;
	case _CONNECT_NEGOCHECK:
		break;

	case _CONNECT_TIMINGSTART:
        gNetSys._pHandle = GFL_NET_HANDLE_GetCurrentHandle();
		GFL_NET_TimingSyncStart(gNetSys._pHandle, _TEST_TIMING);
		gNetSys._connectSeqNo = _CONNECT_TIMINGCHECK;
		break;

	case _CONNECT_TIMINGCHECK:
		if(GFL_NET_IsTimingSync(gNetSys._pHandle,_TEST_TIMING)){
			OS_Printf("�^�C�~���O��ꂽ\n");
			result = TRUE;
		}
		break;
	}
	return result;
#else
	return TRUE;
#endif
}

//------------------------------------------------------------------
// 
//	�I��
//
//------------------------------------------------------------------

void FieldEndGameNet(void)
{
#ifdef NET_WORK_ON
    if(GFL_NET_IsInit()){
        gNetSys._connectSeqNo = _CONNECT_NONE;
        gNetSys._exitSeqNo = _EXIT_START;
    }
    else{
        gNetSys._exitSeqNo = _EXIT_END;
    }
#endif
}

static void _endCallback(void* work)
{
    gNetSys._exitSeqNo = _EXIT_END;
}


BOOL FieldExitGameNet(void)
{
#ifdef NET_WORK_ON

    switch(gNetSys._exitSeqNo){
      case _EXIT_START:
        GFL_NET_Exit(_endCallback);
        gNetSys._exitSeqNo = _EXIT_WAIT;
        break;
      case _EXIT_WAIT:
        break;
      case _EXIT_END:
        return TRUE;
        break;
    }
	return FALSE;
#else
	return TRUE;
#endif
}

//------------------------------------------------------------------
// 
//	�f�[�^���M
//
//------------------------------------------------------------------
void FieldSendGameNet( int comm, int size, void* commWork )
{
#ifdef NET_WORK_ON
    if(gNetSys._pHandle){
        GFL_NET_SendData( gNetSys._pHandle, comm, size, commWork );
    }
#endif
}

//------------------------------------------------------------------
// 
//	�l�b�g�h�c�擾�@
//
//------------------------------------------------------------------
NetID FieldGetNetID(void)
{
#ifdef NET_WORK_ON
	return GFL_NET_GetNetID( gNetSys._pHandle );
#else
	return 1;		//1orgin
#endif
}



