//============================================================================================
/**
 * @file	game_net.c
 * @brief	�ʐM����p�֐�
 */
//============================================================================================
#include "gflib.h"
#include "main.h"
#include "net_icondata.h"

#include "game_net.h"

#define _BCON_GET_NUM  (1)
//------------------------------------------------------------------
// 
//	������
//
//------------------------------------------------------------------
// ���[�J���ʐM�R�}���h�̒�`
enum _testCommand_e {
    _TEST_VARIABLE = GFL_NET_CMD_COMMAND_MAX,
    _TEST_GETSIZE,
    _TEST_HUGE,
    _TEST_VARIABLE_HUGE,
};

typedef enum {
	gNetSysParent = 0,
	gNetSysChild = 1,
}GAMENET_SYSMODE;

#define _TEST_HUGE_SIZE (5000)

typedef struct {
	u8				_dataPool[_TEST_HUGE_SIZE];  //��e�ʎ�M�o�b�t�@ �e�X�g�p
	u8				_dataSend[_TEST_HUGE_SIZE];  //��e�ʑ��M�o�b�t�@ �e�X�g�p
	int				_connectSeqNo;
	int				_exitSeqNo;
	GFL_NETHANDLE*	_pHandle;
	GFL_NETHANDLE*	_pHandleServer;
	GAMENET_SYSMODE	mode;

}GAMENET_SYS;

static GAMENET_SYS	gNetSys;

#define _TEST_TIMING (12)

typedef struct{
    int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { 1 };

static void* _netBeaconGetFunc(void)    ///< �r�[�R���f�[�^�擾�֐�
{
    return &_testBeacon;
}

static int _netBeaconGetSizeFunc(void)    ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
{
    return sizeof(_testBeacon);
}

static BOOL _netBeaconCompFunc(int myNo,int beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
{
    OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
    return TRUE;
}

#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// �σT�C�Y��M
static void _testRecvVariable(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    OS_Printf("_testRecvVariable %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}


static void _testRecvGetSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    OS_Printf("_testRecvGetSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}

static int _getTestCommandSize(void)
{
    return 12;
}

static void _testRecvHugeSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    OS_Printf("_testRecvHugeSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[size-1]);
}

static void _testRecvVariableHugeSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    OS_Printf("_testRecvVariableHugeSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[size-1]);
}

static u8* _getHugeMemoryPoolAddress(int netID, void* pWork, int size)
{
    return gNetSys._dataPool;
}

static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo)
{
	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
}

// ���[�J���ʐM�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    // �σT�C�Y�e�X�g
    { _testRecvVariable, GFL_NET_COMMAND_VARIABLE, NULL },
    // �T�C�Y�擾�֐��e�X�g
    { _testRecvGetSize, _getTestCommandSize, NULL },
    // �傫�ȃT�C�Y�e�X�g
    { _testRecvHugeSize, GFL_NET_COMMAND_SIZE( _TEST_HUGE_SIZE ), _getHugeMemoryPoolAddress },
    // ����ȉσT�C�Y�̃e�X�g
    { _testRecvVariableHugeSize, GFL_NET_COMMAND_VARIABLE, _getHugeMemoryPoolAddress },
};

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

// �ʐM�������\����  wifi�p
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,   // ���[�N�|�C���^
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NET_ICONDATA_GetTableData,   // �ʐM�A�C�R���̃t�@�C��ARC�e�[�u����Ԃ��֐�
    NET_ICONDATA_GetNoBuff,      // �ʐM�A�C�R���̃t�@�C��ARC�̔ԍ���Ԃ��֐�
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �ʐM���J�n���邩�ǂ���
};

void InitGameNet(void)
{
    GFL_NET_Init(&aGFLNetInit);

	gNetSys._connectSeqNo = 0;
	gNetSys._exitSeqNo = 0;
	gNetSys._pHandle = NULL;
	gNetSys._pHandleServer = NULL;
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
	BOOL result = FALSE;

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
			if( gNetSys.mode == gNetSysChild ){
				//���[�h�Z�b�g
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
}

#if 0
enum{
    _TEST_CONNECT,
    _TEST_CONNECT2,
    _TEST_1,
    _TEST_4,
    _TEST_2,
    _TEST_3,
    _TEST_END,
    _TEST_EXIT,

};

BOOL ConnectGameNet(void)
{
    // �N���C�A���g���̃e�X�g
    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        switch( gNetSys._connectSeqNo){
          case _TEST_CONNECT:
            {
                gNetSys._pHandle = GFL_NET_CreateHandle();
                GFL_NET_StartBeaconScan(gNetSys._pHandle );    // �r�[�R����҂�
//                GFL_NET_InitClientAndConnectToParent(gNetSys._pHandle, mac);  //mac�A�h���X�֐ڑ�
//                GFL_NET_ChangeoverConnect(gNetSys._pHandle); // �����ڑ�
            }
             gNetSys._connectSeqNo++;
            break;
          case _TEST_CONNECT2:
            {
                u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
                if(pData){
                    GFL_NET_ConnectToParent(gNetSys._pHandle, pData);
                }
            }
             gNetSys._connectSeqNo++;
            break;
          case _TEST_1:
            GFL_NET_RequestNegotiation( gNetSys._pHandle );
             gNetSys._connectSeqNo++;
            break;

          case _TEST_4:
            {
                const u8 buff[10]={1,2,3,4,5,6,7,8,9,10};
                int i;
//                GFL_NET_SendDataEx(gNetSys._pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE, 10, buff, FALSE, FALSE);
//                GFL_NET_SendDataEx(gNetSys._pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_GETSIZE, 0, buff, FALSE, FALSE);
//                for(i=0;i<_TEST_HUGE_SIZE;i++){
//                    gNetSys._dataSend[i] = (u8)i;
//                }
//                GFL_NET_SendDataEx(gNetSys._pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_HUGE, 0, gNetSys._dataSend, FALSE, FALSE);
//                GFL_NET_SendDataEx(gNetSys._pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE_HUGE, 10, gNetSys._dataSend, FALSE, FALSE);


                GFL_NET_ChangeMpMode(gNetSys._pHandle);

            }
             gNetSys._connectSeqNo++;
            break;
            


          case _TEST_2:
            {
                u8 send = _TEST_TIMING;
//                BOOL ret = GFL_NET_SendData(gNetSys._pHandle, GFL_NET_CMD_TIMING_SYNC,&send);
//                OS_TPrintf("send %d\n",ret);
                GFL_NET_TimingSyncStart(gNetSys._pHandle, send);
            }
             gNetSys._connectSeqNo++;
            break;
          case _TEST_3:
            if(GFL_NET_IsTimingSync(gNetSys._pHandle,_TEST_TIMING)){
                OS_Printf("�^�C�~���O��ꂽ\n");
				gNetSys.mode = gNetSysChild;	//�q
	
                 gNetSys._connectSeqNo++;
            }
            else{
                OS_Printf("�^�C�~���O�͎��ĂȂ�\n");
            }
            break;

          case _TEST_END:
            // ���̏�Őؒf
         //   GFL_NET_Disconnect();
            // �ʐM�őS����ؒf
            GFL_NET_SendData(gNetSys._pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
             gNetSys._connectSeqNo++;
            break;
        }
        OS_TPrintf("c %d\n", gNetSys._connectSeqNo);
    }
    // �T�[�o�[���̃e�X�g
    if(PAD_BUTTON_R == GFL_UI_KEY_GetTrg()){
        switch( gNetSys._connectSeqNo){
          case _TEST_CONNECT:
            {
                gNetSys._pHandleServer = GFL_NET_CreateHandle();
                GFL_NET_InitServer(gNetSys._pHandleServer);   // �T�[�o
                gNetSys._pHandle = GFL_NET_CreateHandle();  // �N���C�A���g
//                GFL_NET_ChangeoverConnect(gNetSys._pHandle); // �����ڑ�
            }
             gNetSys._connectSeqNo++;
            break;
          case _TEST_CONNECT2:
            GFL_NET_RequestNegotiation( gNetSys._pHandle );
             gNetSys._connectSeqNo++;
            break;
          case _TEST_1:
            GFL_NET_TimingSyncStart(gNetSys._pHandle, _TEST_TIMING);
			gNetSys.mode = gNetSysParent;	//�e
	
             gNetSys._connectSeqNo = _TEST_END;
            break;
          case _TEST_END:
            GFL_NET_Disconnect();
             gNetSys._connectSeqNo++;
            break;
          case _TEST_EXIT:
            GFL_NET_Exit();
             gNetSys._connectSeqNo++;
            break;
        }
        OS_TPrintf("p %d\n", gNetSys._connectSeqNo);
    }
}
#endif

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
	BOOL result = FALSE;

	switch( gNetSys._exitSeqNo ){

	case _EXIT_START:
		if( gNetSys.mode == gNetSysParent ){
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
}



