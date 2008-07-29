//============================================================================================
/**
 * @file	game_net.c
 * @brief	�ʐM����p�֐�
 */
//============================================================================================
#include "gflib.h"
#include "include\net\network_define.h"
#include "include\system\main.h"
#include "include\test\net_icondata.h"

#include "game_net.h"
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

extern const NetRecvFuncTable _CommPacketTbl[1];
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
static void* _netBeaconGetFunc(void)
{
	return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void)
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

static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	OS_TPrintf("�ʐM�s�\�G���[������ ErrorNo = %d\n",errNo);
}


static void _initCallback(void* pWork)
{
}

// �ʐM�������\����  wifi�p
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    1,//NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,   // ���[�N�|�C���^
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NULL,//NET_ICONDATA_GetTableData,   // �ʐM�A�C�R���̃t�@�C��ARC�e�[�u����Ԃ��֐�
    NULL,//NET_ICONDATA_GetNoBuff,      // �ʐM�A�C�R���̃t�@�C��ARC�̔ԍ���Ԃ��֐�
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

void InitGameNet(void)
{
#ifdef NET_WORK_ON
    GFL_NET_Init(&aGFLNetInit, _initCallback);
#endif
	gNetSys._connectSeqNo = 0;
	gNetSys._exitSeqNo = 0;
	gNetSys._pHandle = NULL;
}


enum{
    _CONNECT_START = 0,
    _CONNECT,
    _CONNECT_CHECK,
    _CONNECT_NEGO,
    _CONNECT_NEGOCHECK,
    _CONNECT_TIMINGSTART,
    _CONNECT_TIMINGCHECK,
};

//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _connectCallBack(void* pWork)
{
    OS_TPrintf("�l�S�V�G�[�V��������\n");
    gNetSys._connectSeqNo = _CONNECT_TIMINGSTART;
}

//------------------------------------------------------------------
// 
//	�ڑ�
//
//------------------------------------------------------------------


BOOL ConnectGameNet(void)
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
		if( GFL_NET_HANDLE_RequestNegotiation() == TRUE ){
			gNetSys._connectSeqNo = _CONNECT_NEGOCHECK;
		}
		break;

	case _CONNECT_NEGOCHECK:
        gNetSys._pHandle = GFL_NET_HANDLE_GetCurrentHandle();
		if( GFL_NET_HANDLE_IsNegotiation( gNetSys._pHandle ) == TRUE ){
			if( GFL_NET_IsParentMachine() == FALSE ){
				//�q�@�̏ꍇ���[�h�Z�b�g�\(default��DS���[�h)
				//GFL_NET_ChangeMpMode(gNetSys._pHandle);
			}
			gNetSys._connectSeqNo = _CONNECT_TIMINGSTART;
		}
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
enum{
    _EXIT_START = 0,
    _EXIT_END,
};

BOOL ExitGameNet(void)
{
#ifdef NET_WORK_ON
	BOOL result = FALSE;

	switch( gNetSys._exitSeqNo ){

	case _EXIT_START:
		if( GFL_NET_IsParentMachine() == FALSE ){
			GFL_NET_Exit(NULL);
		}else{
			GFL_NET_SendData(gNetSys._pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
		}
		gNetSys._exitSeqNo = _EXIT_END;
		break;

	case _EXIT_END:
		GFL_NET_Exit(NULL);
		result = TRUE;
		break;
	}
	return result;
#else
	return TRUE;
#endif
}

//------------------------------------------------------------------
// 
//	�f�[�^���M
//
//------------------------------------------------------------------
void SendGameNet( int comm, void* commWork )
{
#ifdef NET_WORK_ON
	GFL_NET_SendData( gNetSys._pHandle, comm, commWork );
#endif
}

//------------------------------------------------------------------
// 
//	�l�b�g�h�c�擾�@
//
//------------------------------------------------------------------
NetID GetNetID(void)
{
#ifdef NET_WORK_ON
	return GFL_NET_GetNetID( gNetSys._pHandle );
#else
	return 1;		//1orgin
#endif
}



