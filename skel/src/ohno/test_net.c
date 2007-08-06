//============================================================================================
/**
 * @file	test_net.c
 * @brief	�ʐM����e�X�g�p�֐�
 * @author	ohno
 * @date	2006.12.19
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "gf_standard.h"
#include "test_net.h"
#include "main.h"
#include "fatal_error.h"
#include "net_icondata.h"
#include "gfl_use.h"

#define _BCON_GET_NUM  (6)   //�e�@�r�[�R�����擾����䐔 ���R�Ɍ��߂���
#define _USERNAME_MAXLEN (8) //�������l���̖��O�̒��� �r�[�R�����Ɋ܂߂�

//------------------------------------------------------------------
// NET�̃e�X�g
//------------------------------------------------------------------

// ���[�J���ʐM�R�}���h�̒�`
enum _testCommand_e {
    _TEST_VARIABLE = GFL_NET_CMD_COMMAND_MAX,
    _TEST_GETSIZE,
    _TEST_HUGE,
    _TEST_VARIABLE_HUGE,
};

#define _TEST_HUGE_SIZE (5000)
static u8 _dataPool[_TEST_HUGE_SIZE];  //��e�ʎ�M�o�b�t�@ �e�X�g�p
static u8 _dataSend[_TEST_HUGE_SIZE];  //��e�ʑ��M�o�b�t�@ �e�X�g�p

typedef struct{
    int gameNo;      ///< �Q�[�����
    u8 userName[_USERNAME_MAXLEN];  ///< ���[�U�[�̖��O
} _testBeaconStruct;

//�_�~�[�r�[�R���f�[�^
static _testBeaconStruct _testBeacon = { 1,{'G','F','L','I',0,0,0,0} };


static void* _netBeaconGetFunc(void)    ///< �r�[�R���f�[�^�擾�֐�
{
    return &_testBeacon;
}

static int _netBeaconGetSizeFunc(void)    ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
{
    return sizeof(_testBeacon);
}

static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
{
 //   OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
    return TRUE;
}



//----------------------------------------------------------------
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


static int _testNo = 0;
static GFL_NETHANDLE* _pHandle=NULL;
static GFL_NETHANDLE* _pHandleServer=NULL;
#define _TEST_TIMING (12)

#if GFL_NET_WIFI //GFL_NET_WIFI WIFI�ʐM�e�X�g

void TEST_NET_Main(void)
{
    int i;
    
    // �N���C�A���g���̃e�X�g
    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        switch(_testNo){
          case _TEST_CONNECT:
            {
                _pHandle = GFL_NET_CreateHandle();
                GFL_NET_WifiLogin(_pHandle );    // wifiLogin �}�b�`���O�ҋ@�ֈړ�
            }
            _testNo++;
            break;
          case _TEST_CONNECT2:
            _testNo++;
            break;
          case _TEST_1:
            _testNo++;
            break;
          case _TEST_4:
            _testNo++;
            break;
          case _TEST_2:
            _testNo++;
            break;
          case _TEST_3:
            break;
          case _TEST_END:
            break;
        }
        OS_TPrintf("c %d\n",_testNo);
    }

}

#else  //GFL_NET_WIFI ���ʂ̃��C�����X�ʐM

void TEST_NET_Main(void)
{
    u8 mac[6] = {0x00,0x09,0xbf,0x08,0x2e,0x6e};
    u8 beacon;
    int i;
    
//    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
//        for(i = 0;i < _BCON_GET_NUM; i++){
//            u8* pData = GFL_NET_GetBeaconMacAddress(i);
//            if(pData){
//                OS_TPrintf("%d: mac %x%x%x%x%x%x\n",i,pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
////            }
//        }
//    }
    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
        _testBeaconStruct* pBS; // �r�[�R���ɐe�@�̏�񂪂���̂ŉ��H���Ďg�p����
        for(i = 0;i < _BCON_GET_NUM; i++){
            pBS = GFL_NET_GetBeaconData(i);
            if(pBS){  //���������疼�O��\�����Ă���
                OS_TPrintf("%d: Find %s\n",i, pBS->userName);
            }
        }
    }

    // �N���C�A���g���̃e�X�g
    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        switch(_testNo){
          case _TEST_CONNECT:
            {
                _pHandle = GFL_NET_CreateHandle();
                GFL_NET_StartBeaconScan(_pHandle );    // �r�[�R����҂�
//                GFL_NET_InitClientAndConnectToParent(_pHandle, mac);  //mac�A�h���X�֐ڑ�
//                GFL_NET_ChangeoverConnect(_pHandle); // �����ڑ�
            }
            _testNo++;
            break;
          case _TEST_CONNECT2:
            {
                u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
                if(pData){
                    GFL_NET_ConnectToParent(_pHandle, pData);
                }
            }
            _testNo++;
            break;
          case _TEST_1:
            GFL_NET_RequestNegotiation( _pHandle );
            _testNo++;
            break;

          case _TEST_4:
            {
                const u8 buff[10]={1,2,3,4,5,6,7,8,9,10};
                int i;
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE, 10, buff, FALSE, FALSE);
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_GETSIZE, 0, buff, FALSE, FALSE);
//                for(i=0;i<_TEST_HUGE_SIZE;i++){
//                    _dataSend[i] = (u8)i;
//                }
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_HUGE, 0, _dataSend, FALSE, FALSE);
//                GFL_NET_SendDataEx(_pHandle,GFL_NET_SENDID_ALLUSER,
//                                   _TEST_VARIABLE_HUGE, 10, _dataSend, FALSE, FALSE);


                GFL_NET_ChangeMpMode(_pHandle);

            }
            _testNo++;
            break;
            


          case _TEST_2:
            {
                u8 send = _TEST_TIMING;
//                BOOL ret = GFL_NET_SendData(_pHandle, GFL_NET_CMD_TIMING_SYNC,&send);
//                OS_TPrintf("send %d\n",ret);
                GFL_NET_TimingSyncStart(_pHandle, send);
            }
            _testNo++;
            break;
          case _TEST_3:
            if(GFL_NET_IsTimingSync(_pHandle,_TEST_TIMING)){
                NET_PRINT("�^�C�~���O��ꂽ\n");
                _testNo++;
            }
            else{
                NET_PRINT("�^�C�~���O�͎��ĂȂ�\n");
            }
            break;
          case _TEST_END:
            // ���̏�Őؒf
         //   GFL_NET_Disconnect();
            // �ʐM�őS����ؒf
            GFL_NET_SendData(_pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
            _testNo++;
            break;
        }
        OS_TPrintf("c %d\n",_testNo);
    }
    // �T�[�o�[���̃e�X�g
    if(PAD_BUTTON_R == GFL_UI_KEY_GetTrg()){
        switch(_testNo){
          case _TEST_CONNECT:
            {
                _pHandleServer = GFL_NET_CreateHandle();
                GFL_NET_InitServer(_pHandleServer);   // �T�[�o
                _pHandle = GFL_NET_CreateHandle();  // �N���C�A���g
//                GFL_NET_ChangeoverConnect(_pHandle); // �����ڑ�
            }
            _testNo++;
            break;
          case _TEST_CONNECT2:
            GFL_NET_RequestNegotiation( _pHandle );
            _testNo++;
            break;
          case _TEST_1:
            GFL_NET_TimingSyncStart(_pHandle, _TEST_TIMING);
            _testNo = _TEST_END;
            break;
          case _TEST_END:
            GFL_NET_Disconnect();
            _testNo++;
            break;
          case _TEST_EXIT:
            GFL_NET_Exit();
            _testNo++;
            break;
        }
        OS_TPrintf("p %d\n",_testNo);
    }


}

#endif//GFL_NET_WIFI 


#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// �σT�C�Y��M
static void _testRecvVariable(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvVariable %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}


static void _testRecvGetSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvGetSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}

static int _getTestCommandSize(void)
{
    return 12;
}

static void _testRecvHugeSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvHugeSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[size-1]);
}

static void _testRecvVariableHugeSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvVariableHugeSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[size-1]);
}

static u8* _getHugeMemoryPoolAddress(int netID, void* pWork, int size)
{
    return _dataPool;
}


// ���[�J���ʐM�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    // �σT�C�Y�e�X�g
    {_testRecvVariable,          GFL_NET_COMMAND_VARIABLE, NULL},
    // �T�C�Y�擾�֐��e�X�g
    {_testRecvGetSize,           _getTestCommandSize, NULL},
    // �傫�ȃT�C�Y�e�X�g
    {_testRecvHugeSize,          GFL_NET_COMMAND_SIZE( _TEST_HUGE_SIZE ), _getHugeMemoryPoolAddress},
    // ����ȉσT�C�Y�̃e�X�g
    {_testRecvVariableHugeSize,  GFL_NET_COMMAND_VARIABLE,      _getHugeMemoryPoolAddress},
};

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

#if GFL_NET_WIFI //GFL_NET_WIFI WIFI�ʐM�e�X�g

// ���̓�̃f�[�^���u�Ƃ��������[�ǁv�ɂȂ�܂��B�{���̓Z�[�u����K�v������܂�
static DWCUserData _testUserData;
static DWCFriendData _testFriendData[_BCON_GET_NUM];

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
    NULL,  // wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL,  // wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    &_testFriendData[0],  // DWC�`���̗F�B���X�g	
    &_testUserData,  // DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid  �ʐM�Q�[���ɂ���ĕς��鐔��
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    TRUE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �ʐM���J�n���邩�ǂ���
};

#else  //GFL_NET_WIFI ���ʂ̃��C�����X�ʐM

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
    GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    _MAXNUM,     // �ő�ڑ��l��
    _MAXSIZE,  //�ő呗�M�o�C�g��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �ʐM���J�n���邩�ǂ���
};

#endif //GFL_NET_WIFI

void TEST_NET_Init(void)
{
    //�C�N�j���[�������g�p����O�� VRAMD��disable�ɂ���K�v������̂���
    //VRAMD�����Ɏg���Ă����̂����킩��Ȃ��ƁA�������Ƃ��ł��Ȃ�

    if(GX_VRAM_LCDC_D == GX_GetBankForLCDC()){
        GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����
    }
    else if(GX_VRAM_D & GX_GetBankForBG()){
        GX_DisableBankForBG();
    }
    else if(GX_VRAM_D & GX_GetBankForTex()){
        GX_DisableBankForTex();
    }
    else if(GX_VRAM_D & GX_GetBankForClearImage()){
        GX_DisableBankForClearImage();
    }

    GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����
    GX_DisableBankForBG();
    GX_DisableBankForTex();
    GX_DisableBankForClearImage();

	GFLUser_VIntr();
	GFLUser_VIntr();
	GFLUser_VIntr();
	GFLUser_VIntr();
	GFLUser_VIntr();
	GFLUser_VIntr();
//    SkeltonVBlankFunc();
    
    GFL_NET_Init(&aGFLNetInit);

}



