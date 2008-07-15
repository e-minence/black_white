//============================================================================================
/**
 * @file	game_net.c
 * @brief	�ʐM����p�֐�
 */
//============================================================================================
#include "gflib.h"
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

static _testBeaconStruct _testBeacon = { 1 };

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

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
	OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
	return TRUE;
}

#define SSID "GF_TETSU"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}

static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	OS_TPrintf("�ʐM�s�\�G���[������ ErrorNo = %d\n",errNo);
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
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid
    2,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �ʐM���J�n���邩�ǂ���
};

void InitGameNet(void)
{
#ifdef NET_WORK_ON
    GFL_NET_Init(&aGFLNetInit);
#endif
	gNetSys._connectSeqNo = 0;
	gNetSys._exitSeqNo = 0;
	gNetSys._pHandle = NULL;
}


//------------------------------------------------------------------
// 
//	�ڑ�
//
//------------------------------------------------------------------
enum{
    _CONNECT_START = 0,
    _CONNECT,
    _CONNECT_CHECK,
    _CONNECT_NEGO,
    _CONNECT_NEGOCHECK,
    _CONNECT_TIMINGSTART,
    _CONNECT_TIMINGCHECK,
};

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
		gNetSys._pHandle = GFL_NET_CreateHandle();
		GFL_NET_ChangeoverConnect( gNetSys._pHandle ); // �����ڑ�
		gNetSys._connectSeqNo = _CONNECT_NEGO;
		break;

	case _CONNECT_NEGO:
		if( GFL_NET_RequestNegotiation( gNetSys._pHandle ) == TRUE ){
			gNetSys._connectSeqNo = _CONNECT_NEGOCHECK;
		}
		break;

	case _CONNECT_NEGOCHECK:
		if( GFL_NET_IsNegotiation( gNetSys._pHandle ) == TRUE ){
			if( GFL_NET_IsParentMachine() == FALSE ){
				//�q�@�̏ꍇ���[�h�Z�b�g�\(default��DS���[�h)
				//GFL_NET_ChangeMpMode(gNetSys._pHandle);
			}
			gNetSys._connectSeqNo = _CONNECT_TIMINGSTART;
		}
		break;

	case _CONNECT_TIMINGSTART:
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
		if( GFL_NET_IsParentMachine() == TRUE ){
			GFL_NET_Disconnect();
		}else{
			GFL_NET_SendData(gNetSys._pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
		}
		gNetSys._exitSeqNo = _EXIT_END;
		break;

	case _EXIT_END:
		GFL_NET_Exit();
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
	GFL_NET_SendData( gNetSys._pHandle, comm, commWork );
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



