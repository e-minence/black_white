//============================================================================================
/**
 * @file	yt_net.c
 * @brief	DS�Ń��b�V�[�̂��܂� �l�b�g���[�N�֘A�̃v���O����
 * @author	ohno
 * @date	2007.03.05
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"
#include "yt_common.h"
#include "yt_net.h"
#include "../ohno/fatal_error.h"


//�ʐM�p�\����
struct _NET_PARAM
{
    GFL_NETHANDLE* pNetHandle[2];  //�ʐM�p�n���h�� �e�Ǝq�̃n���h�����Ǘ����邩��Q�K�v
    u32 seq;
    BOOL bGameStart;
};


#define _BCON_GET_NUM  (1)

static void _netAutoParent(void* work)
{
//    NET_PARAM* pNet= (NET_PARAM*)work;
//    pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �N���C�A���g�n���h���쐬
}

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

#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}

static BOOL _netBeaconCompFunc(int myNo,int beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
{
    OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
    return TRUE;
}

static void _recvCLACTPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

//���ʂ̃��C�����X�ʐM

// ���[�J���ʐM�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    {_recvCLACTPos, GFL_NET_COMMAND_VARIABLE, NULL},
    {_recvCLACTPos, GFL_NET_COMMAND_VARIABLE, NULL},
    {_recvCLACTPos, GFL_NET_COMMAND_VARIABLE, NULL},
    {_recvCLACTPos, GFL_NET_COMMAND_VARIABLE, NULL},
};


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
    NULL, //_netAutoParent,    //�����ڑ��̍ۂɐe�ɂȂ�ꍇ�ɌĂ΂��֐�
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


enum{
    _INIT_WAIT_PARENT,
    _INIT_WAIT_CHILD,
    _SEARCH_CHILD,
    _CONNECT_WAIT,
    _NEGO_START,
    _TIM_START,
    _TIM_OK,
    _LOOP,
};


#define _TEST_TIMING (14)

static void _seqChange(NET_PARAM* pNet, int no)
{
    NET_PRINT("ytst change %d\n",no);
    pNet->seq = no;
}


#define _SEQCHANGE(no)   _seqChange(pNet,no)


BOOL YT_NET_Main(void *work)
{
    NET_PARAM* pNet = work;

    switch(pNet->seq){
      case _INIT_WAIT_PARENT:
        if(GFL_NET_IsInit()){
            pNet->pNetHandle[0] = GFL_NET_CreateHandle();   // �n���h���쐬
            GFL_NET_ServerConnect(pNet->pNetHandle[0]); // �����ڑ�
            pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �n���h���쐬
            _SEQCHANGE(_CONNECT_WAIT);
        }
        break;
      case _INIT_WAIT_CHILD:
        if(GFL_NET_IsInit()){
            pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �n���h���쐬
            GFL_NET_ClientConnect(pNet->pNetHandle[1]); // �����ڑ�
            _SEQCHANGE( _SEARCH_CHILD );
        }
        break;
      case _SEARCH_CHILD:
        {
            u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
            if(pData){
                GFL_NET_ClientToAccess(pNet->pNetHandle[1], pData);
                _SEQCHANGE( _CONNECT_WAIT );
            }
        }
        break;
      case _CONNECT_WAIT:
        _SEQCHANGE( _NEGO_START );
        break;
      case _NEGO_START:
        if(pNet->pNetHandle[0]){
            if(GFL_NET_NegotiationRequest( pNet->pNetHandle[1] )){
                _SEQCHANGE( _TIM_START );
            }
        }
        else{
            if(GFL_NET_GetNegotiationConnectNum( pNet->pNetHandle[1]) != 0){
                if(GFL_NET_NegotiationRequest( pNet->pNetHandle[1] )){
                    _SEQCHANGE( _TIM_START );
                }
            }
        }
        break;
      case _TIM_START:
        if(GFL_NET_GetNegotiationConnectNum(pNet->pNetHandle[1])==2){
            GFL_NET_TimingSyncStart(pNet->pNetHandle[1] , _TEST_TIMING);
            _SEQCHANGE( _TIM_OK );
        }
        break;
      case _TIM_OK:
        if(GFL_NET_IsTimingSync(pNet->pNetHandle[1] , _TEST_TIMING)){
            pNet->bGameStart = TRUE;
            _SEQCHANGE( _LOOP );
        }
        break;

    }
    return pNet->bGameStart;
}


//----------------------------------------------------------------------------
/**
 *	@brief	
 */
//-----------------------------------------------------------------------------

void YT_NET_Init(GAME_PARAM* gp, BOOL bParent)
{
    TCBSYS* tcbsys;
    NET_PARAM* pNet = GFL_HEAP_AllocMemory(gp->heapID, sizeof(NET_PARAM));

    GFL_STD_MemClear(pNet, sizeof(NET_PARAM));
    aGFLNetInit.pWork = pNet;
    gp->pNetParam = pNet;

    GFL_NET_sysInit(&aGFLNetInit);

    if(bParent){
        pNet->seq = _INIT_WAIT_PARENT;
    }
    else{
        pNet->seq = _INIT_WAIT_CHILD;
    }

//	GFL_TCB_AddTask(gp->tcbsys,YT_NET_Main,pNet,1);
}



