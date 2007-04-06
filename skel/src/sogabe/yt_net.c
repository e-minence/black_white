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
#include "fallchr.h"
#include "player.h"
#include "../ohno/fatal_error.h"


// static �֐���`
static void _recvCLACTPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTPlayerAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTDelete(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);


//�ʐM�p�\����
struct _NET_PARAM
{
    GFL_NETHANDLE* pNetHandle[2];  //�ʐM�p�n���h�� �e�Ǝq�̃n���h�����Ǘ����邩��Q�K�v
    CLWK* pCLACT[YT_CLACT_MAX];  // �`�悵�Ă�clact
    PLAYER_PARAM* pp;
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

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (120)  // ���M�ő�T�C�Y

//���ʂ̃��C�����X�ʐM


/// �o�g����p�ʐM�R�}���h�̒�`�B
enum CommCommandBattle_e {
    YT_NET_COMMAND_ANIM = GFL_NET_CMD_COMMAND_MAX,  ///< �A�j���R�}���h���M
    YT_NET_COMMAND_POS,                             ///< �ʒu�R�}���h���M
    YT_NET_COMMAND_PLAYERANIM,            ///< �v���[���[�̃A�j���[�V�������̑��M
    YT_NET_COMMAND_DELETE,            /// CLACT����������
    //------------------------------------------------�����܂�
    YT_NET_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};

//----------------------------------------------------------------------------
// @brief	�A�j���[�V�����ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 clactNo;
    u16 anmseq;
} COMM_ANIM_ST;

//----------------------------------------------------------------------------
// @brief	�v���C���[�A�j���[�V�����ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u16 anm_no;
    u8 line_no;
    u8 rot;
    u8 actno;
} COMM_PLAYER_ANIM_ST;

//----------------------------------------------------------------------------
// @brief	�ʒu�ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 clactNo;
    s16 x;
    s16 y;
} COMM_POS_ST;

// @brief	��������
//-----------------------------------------------------------------------------
typedef struct {
    u8 clactNo;
} COMM_DELETE_ST;



// ���[�J���ʐM�e�[�u��
static const NetRecvFuncTable _CommPacketTbl[] = {
    {_recvCLACTAnim, GFL_NET_COMMAND_SIZE(sizeof(COMM_ANIM_ST)), NULL},
    {_recvCLACTPos, GFL_NET_COMMAND_SIZE(sizeof(COMM_POS_ST)), NULL},
    {_recvCLACTPlayerAnim, GFL_NET_COMMAND_SIZE(sizeof(COMM_PLAYER_ANIM_ST)), NULL},
    {_recvCLACTDelete, GFL_NET_COMMAND_SIZE(sizeof(COMM_DELETE_ST)), NULL},
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



//----------------------------------------------------------------------------
/**
 *	@brief	�ʒu���߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvCLACTPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_POS_ST* pPos = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;
	CLSYS_POS	pos;

    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    
    //OS_TPrintf("testPos %d %d %d\n",pPos->clactNo, pPos->x, pPos->y);
    pos.x = pPos->x;
    pos.y = pPos->y;
    if(pNet->pCLACT[pPos->clactNo] != NULL){
        GFL_CLACT_WkSetWldPos(pNet->pCLACT[pPos->clactNo],&pos);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�������߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvCLACTDelete(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_POS_ST* pPos = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;
	CLSYS_POS	pos;

    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    if(pNet->pCLACT[pPos->clactNo] != NULL){
        GFL_CLACT_WkDel(pNet->pCLACT[pPos->clactNo]);
        pNet->pCLACT[pPos->clactNo] = NULL;
    }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�������߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvCLACTAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_ANIM_ST* pAnim = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;


    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }

    //OS_TPrintf("testAnim %d %d\n",pAnim->clactNo,pAnim->anmseq);

    GF_ASSERT(pAnim->clactNo < YT_CLACT_MAX);
    
    if(pNet->pCLACT[pAnim->clactNo]==NULL){
        pNet->pCLACT[pAnim->clactNo] = YT_InitNetworkFallChr(gp,netID,pAnim->anmseq,0);
    }
    else{
        GFL_CLACT_WkSetAnmSeq(pNet->pCLACT[pAnim->clactNo],pAnim->anmseq);
    }

}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���C���[�A�j���[�V�������߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvCLACTPlayerAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_PLAYER_ANIM_ST* pAnim = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }

    if(pNet->pp == NULL){
        pNet->pp = YT_InitPlayer(gp,netID,netID);
    }
    else{
        YT_PlayerAnimeNetSet(gp, pNet->pp, netID, pAnim->anm_no, pAnim->line_no, pAnim->rot,pAnim->actno);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e�@���ǂ����𔻒f����
 */
//-----------------------------------------------------------------------------

BOOL YT_NET_IsParent(NET_PARAM* pNet)
{
    return (pNet->pNetHandle[0] != NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�̏ꍇ�̓A�j���[�V�����m�n�𑗐M
 */
//-----------------------------------------------------------------------------

void YT_NET_SendPlayerAnmReq(int anm_no,u16 line_no,int rot, int actno,NET_PARAM* pNet)
{
    COMM_PLAYER_ANIM_ST CommPlayerAnmSt;

    if(pNet){
        CommPlayerAnmSt.anm_no = anm_no;
        CommPlayerAnmSt.line_no = line_no;
        CommPlayerAnmSt.rot = rot;
        CommPlayerAnmSt.actno = actno;

        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_PLAYERANIM, &CommPlayerAnmSt);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ΐ펞�ɂ́A�A�j���[�V�����i���o�[�𑗐M����
 */
//-----------------------------------------------------------------------------

void YT_NET_SendAnmReq(int clactno,u16 anmseq,NET_PARAM* pNet)
{
    COMM_ANIM_ST CommAnmSt;
    if(pNet){

    CommAnmSt.clactNo = clactno;
    CommAnmSt.anmseq = anmseq;

    GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_ANIM, &CommAnmSt);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLACT������
 */
//-----------------------------------------------------------------------------

void YT_NET_DeleteReq(int clactno,NET_PARAM* pNet)
{
    COMM_DELETE_ST CommDelSt;

    if(pNet){

    CommDelSt.clactNo = clactno;
    GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_DELETE, &CommDelSt);
    }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ΐ펞�ɂ́A�ʒu�𑗐M����
 */
//-----------------------------------------------------------------------------

void YT_NET_SendPosReq(int clactno,s16 x, s16 y,NET_PARAM* pNet)
{
    COMM_POS_ST CommPosSt;

    if(pNet){
    CommPosSt.clactNo = clactno;
    CommPosSt.x = x;
    CommPosSt.y = y;
    //OS_TPrintf("�ʒu���M %d %d\n",x,y);
    GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_POS, &CommPosSt);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------

BOOL YT_NET_Main(NET_PARAM* pNet)
{

    switch(pNet->seq){
      case _INIT_WAIT_PARENT:
        if(GFL_NET_IsInit()){
            pNet->pNetHandle[0] = GFL_NET_CreateHandle();   // �n���h���쐬
            GFL_NET_InitServer(pNet->pNetHandle[0]); // �����ڑ�
            pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �n���h���쐬
            _SEQCHANGE(_CONNECT_WAIT);
        }
        break;
      case _INIT_WAIT_CHILD:
        if(GFL_NET_IsInit()){
            pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �n���h���쐬
            GFL_NET_StateBeaconScan(pNet->pNetHandle[1]); // �����ڑ�
            _SEQCHANGE( _SEARCH_CHILD );
        }
        break;
      case _SEARCH_CHILD:
        {
            u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
            if(pData){
                GFL_NET_ConnectToParent(pNet->pNetHandle[1], pData);
                _SEQCHANGE( _CONNECT_WAIT );
            }
        }
        break;
      case _CONNECT_WAIT:
        _SEQCHANGE( _NEGO_START );
        break;
      case _NEGO_START:
        if(YT_NET_IsParent(pNet)){  //�e�@�̏ꍇ
            if(GFL_NET_GetNegotiationConnectNum( pNet->pNetHandle[1]) != 0){
                if(GFL_NET_RequestNegotiation( pNet->pNetHandle[1] )){
                    _SEQCHANGE( _TIM_START );
                }
            }
        }
        else{
//            if(GFL_NET_GetNegotiationConnectNum( pNet->pNetHandle[1]) != 0){
                if(GFL_NET_RequestNegotiation( pNet->pNetHandle[1] )){
                    _SEQCHANGE( _TIM_START );
                }
  //          }
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
    aGFLNetInit.pWork = gp;
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



