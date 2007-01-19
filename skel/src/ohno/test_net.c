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
#include "net.h"
#include "ui.h"

#define _BCON_GET_NUM  (1)

//------------------------------------------------------------------
// NET�̃e�X�g
//------------------------------------------------------------------

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



//----------------------------------------------------------------
enum{
    _TEST_CONNECT,
    _TEST_CONNECT2,
    _TEST_1,
    _TEST_2,
    _TEST_END,

};


static int _testNo = 0;
static GFL_NETHANDLE* _pHandle=NULL;

void TEST_NET_Main(void)
{
    u8 mac[6] = {0x00,0x09,0xbf,0x08,0x2e,0x6e};
    u8 beacon;
    int i;
    
    if(PAD_BUTTON_X == GFL_UI_KeyGetTrg()){
        for(i = 0;i < _BCON_GET_NUM; i++){
            u8* pData = GFL_NET_GetBeaconMacAddress(i);
            if(pData){
                OS_TPrintf("%d: mac %x%x%x%x%x%x\n",i,pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
            }
        }
    }
    if(PAD_BUTTON_B == GFL_UI_KeyGetTrg()){
        switch(_testNo){
          case _TEST_CONNECT:
            {
                _pHandle = GFL_NET_CreateHandle();
                GFL_NET_ClientConnect(_pHandle );
//                GFL_NET_ClientConnectTo(_pHandle, mac, TRUE);
            }
            _testNo++;
            break;
          case _TEST_CONNECT2:
            {
                u8* pData = GFL_NET_GetBeaconMacAddress(0);
                if(pData){
                    GFL_NET_ClientConnectTo(_pHandle, pData,FALSE);
                }
            }
            _testNo++;
            break;
          case _TEST_1:
            {
                BOOL ret = GFL_NET_SendData(_pHandle, GFL_NET_CMD_NEGOTIATION,NULL);
                OS_TPrintf("send %d\n",ret);
            }
            _testNo++;
            break;

          case _TEST_2:
            {
                u8 send=12;
                BOOL ret = GFL_NET_SendData(_pHandle, GFL_NET_CMD_TIMING_SYNC,&send);
                OS_TPrintf("send %d\n",ret);
            }
            _testNo++;
            break;
          case _TEST_END:
            GFL_NET_Disconnect();
            _testNo++;
            break;
        }
        OS_TPrintf("c %d\n",_testNo);
    }
    if(PAD_BUTTON_R == GFL_UI_KeyGetTrg()){
        switch(_testNo){
          case _TEST_CONNECT:
            {
                GFL_NETHANDLE* pHandle = GFL_NET_CreateHandle();
                GFL_NET_ServerConnect(pHandle);
                pHandle = GFL_NET_CreateHandle();

                
                
            }
            _testNo++;
            break;
          case _TEST_CONNECT2:
          case _TEST_1:
            GFL_NET_Disconnect();
            _testNo++;
            break;
        }
        OS_TPrintf("p %d\n",_testNo);
    }
    GFL_NET_MainProc();


}

#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// �ʐM�������\����
GFLNetInitializeStruct aGFLNetInit = {
    NULL,  // ��M�֐��e�[�u��
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    _netGetSSID,  // �e�q�ڑ����ɔF�؂���ׂ̃o�C�g��  
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_SYSTEM,  //allocNo
    2,     // �ő�ڑ��l��
    _BCON_GET_NUM,    // �ő�r�[�R�����W��
    TRUE,     // �ʐM���J�n���邩�ǂ���
};


void TEST_NET_Init(void)
{

    GFL_NET_Initialize(&aGFLNetInit, GFL_HEAPID_SYSTEM);

}



