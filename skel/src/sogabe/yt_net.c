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
#include "net_icondata.h"

#if GFL_NET_WIFI //GFL_NET_WIFI WIFI�ʐM�e�X�g
// ���̓�̃f�[�^���u�Ƃ��������[�ǁv�ɂȂ�܂��B�{���̓Z�[�u����K�v������܂�
#define _BCON_GET_NUM  (1)
static DWCUserData _testUserData;
static DWCFriendData _testFriendData[_BCON_GET_NUM];
#endif

// static �֐���`
static void _recvCLACTPos(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTPlayerAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTDelete(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvYossyAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvCLACTAnimCreate(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvScreenMake(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvScreenMakeRot(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvPlaySe(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvPlayGameover(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _recvPlayEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

//�ʐM�p�\����
struct _NET_PARAM
{
    GFL_NETHANDLE* pNetHandle[2];  //�ʐM�p�n���h�� �e�Ǝq�̃n���h�����Ǘ����邩��Q�K�v
    GFL_CLWK* pCLACT[YT_CLACT_MAX];  // �`�悵�Ă�clact
    PLAYER_PARAM* pp;
    u32 seq;
    BOOL bGameStart;
};


#define _BCON_GET_NUM  (1)

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

static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)    ///< �r�[�R���f�[�^�擾�֐�
{
    OS_TPrintf("��r���Ă܂�%d\n",beaconNo);
    return TRUE;
}

#define _MAXNUM (2)     // �ڑ��ő�
#define _MAXSIZE (200)  // ���M�ő�T�C�Y

//���ʂ̃��C�����X�ʐM


/// �o�g����p�ʐM�R�}���h�̒�`�B
enum CommCommandBattle_e {
    YT_NET_COMMAND_ANIM = GFL_NET_CMD_COMMAND_MAX,  ///< �A�j���R�}���h���M  b
    YT_NET_COMMAND_POS,                             ///< �ʒu�R�}���h���Mc
    YT_NET_COMMAND_PLAYERANIM,            ///< �v���[���[�̃A�j���[�V�������̑��Md
    YT_NET_COMMAND_DELETE,            /// CLACT����������e
    YT_NET_COMMAND_YOSSYANIM,     ///���b�V�[�̃A�j���[�V����f
    YT_NET_COMMAND_ANIM_CREATE,   ///< �A�j���[�V�����쐬10
    TY_NET_COMMAND_SCREENMAKE,
    TY_NET_COMMAND_SCREENMAKE_ROT,
    YT_NET_COMMAND_PLAY_SE,
    YT_NET_COMMAND_GAMERESULT,     ///< �Q�[������
    YT_NET_COMMAND_END,
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
// @brief	���������𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 player;
    u8 bWin;
} COMM_RESULT_ST;


//----------------------------------------------------------------------------
// @brief	�X�N���[���ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 player_no;
    u8 old_line_no;
    u8 new_line_no;
} COMM_SCREENMAKE_ST;

//----------------------------------------------------------------------------
// @brief	�X�N���[���ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 player_no;
    u8 line_no;
    u8 flag;
} COMM_SCREENMAKE_ROT_ST;

//----------------------------------------------------------------------------
// @brief	�A�j���[�V�����쐬�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 clactNo;
    u16 type;
} COMM_ANIM_CREATE_ST;

//----------------------------------------------------------------------------
// @brief	�v���C���[�A�j���[�V�����ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u16 anm_no;
    u8 player_no;
    u8 pat_no;
    u8 line_no;
    u8 rot;
} COMM_PLAYER_ANIM_ST;

//----------------------------------------------------------------------------
// @brief	�ʒu�ݒ�𑗐M����
//-----------------------------------------------------------------------------
typedef struct {
    u8 clactNo;
    s16 x;
    s16 y;
} COMM_POS_ST;

//----------------------------------------------------------------------------
// @brief	���b�V�[�A�j�����M
//-----------------------------------------------------------------------------
typedef struct {
    u8 count;
    u8 posx;
    u8 posy;
} COMM_YOSSY_ANIM_ST;

//-----------------------------------------------------------------------------
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
    {_recvYossyAnim, GFL_NET_COMMAND_SIZE(sizeof(COMM_YOSSY_ANIM_ST)), NULL},
    {_recvCLACTAnimCreate, GFL_NET_COMMAND_SIZE(sizeof(COMM_ANIM_CREATE_ST)), NULL},
    {_recvScreenMake, GFL_NET_COMMAND_SIZE(sizeof(COMM_SCREENMAKE_ST)), NULL},
    {_recvScreenMakeRot, GFL_NET_COMMAND_SIZE(sizeof(COMM_SCREENMAKE_ROT_ST)), NULL},
    {_recvPlaySe, GFL_NET_COMMAND_SIZE(sizeof(int)), NULL},
    {_recvPlayGameover, GFL_NET_COMMAND_SIZE(sizeof(COMM_RESULT_ST)), NULL},
    {_recvPlayEnd, GFL_NET_COMMAND_SIZE(0), NULL},
    
};



// �ʐM�������\����  WIFI���p
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // ��M�֐��e�[�u��
    NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
    NULL,   // ���[�N�|�C���^
    _netBeaconGetFunc,  // Infomation�f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // Infomation�f�[�^�T�C�Y�擾�֐�
    _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL, //_netAutoParent,    //�����ڑ��̍ۂɐe�ɂȂ�ꍇ�ɌĂ΂��֐�
    NET_ICONDATA_GetTableData,   // �ʐM�A�C�R���̃t�@�C��ARC�e�[�u����Ԃ��֐�
    NET_ICONDATA_GetNoBuff,      // �ʐM�A�C�R���̃t�@�C��ARC�̔ԍ���Ԃ��֐�
#if GFL_NET_WIFI
    NULL,  // wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL,  // wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    &_testFriendData[0],  // DWC�`���̗F�B���X�g	
    &_testUserData,  // DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
#endif
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
    TRUE,  //CRC�������v�Z���邩�ǂ��� TRUE�̏ꍇ���ׂČv�Z����
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    FALSE,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
};



enum{
    _CONNECT_WIFI,
    _CONNECTING_WIFI,
    _WIFI_MATCHING,
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
	GFL_CLACTPOS	pos;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    //OS_TPrintf("testPos %d %d %d\n",pPos->clactNo, pPos->x, pPos->y);
    pos.x = pPos->x;
    pos.y = pPos->y;
    if(pNet->pCLACT[pPos->clactNo] != NULL){
        GFL_CLACT_WK_SetWldPos(pNet->pCLACT[pPos->clactNo],&pos);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	SE���߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvPlaySe(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const int* pSe_no = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    GFL_SOUND_PlaySE(*pSe_no);
}

//----------------------------------------------------------------------------
/**
 *	@brief	GAMEOVER���߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvPlayGameover(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_RESULT_ST* pResult = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){  // ���M�Җ{�l
        YT_PlayerSetWinLoseFlag(&gp->ps[pResult->player], pResult->bWin);
    }
    else{
        YT_PlayerSetWinLoseFlag(&gp->ps[pResult->player^1], !pResult->bWin);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	END���߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvPlayEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const int* pSe_no = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    // �q�@����ɒʐM���I��������
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
	GFL_CLACTPOS	pos;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    if(pNet->pCLACT[pPos->clactNo] != NULL){
        GFL_AREAMAN_Release(gp->clact_area,pPos->clactNo,1);
        GFL_CLACT_WK_Remove(pNet->pCLACT[pPos->clactNo]);
        pNet->pCLACT[pPos->clactNo] = NULL;
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�V�[�̃A�j���[�V�����������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvYossyAnim(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_YOSSY_ANIM_ST* pYossy = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;
    
    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    YT_YossyBirthAnimeTaskSet(gp,NULL,pYossy->posx,pYossy->posy,pYossy->count);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���쐬�������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvScreenMake(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_SCREENMAKE_ST* pSMake = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    if(pNet->pp){
        YT_PlayerScreenMakeNetFunc(pNet->pp, pSMake->player_no, pSMake->old_line_no, pSMake->new_line_no);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	Rot���̃X�N���[���쐬�������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvScreenMakeRot(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_SCREENMAKE_ROT_ST* pSMake = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }
    if(pNet->pp){
        YT_PlayerRotateScreenMakeNetFunc(pNet->pp, pSMake->player_no, pSMake->line_no, pSMake->flag);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�A�j���[�V�����쐬���߂������Ă���
 */
//-----------------------------------------------------------------------------

static void _recvCLACTAnimCreate(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const COMM_ANIM_CREATE_ST* pAnim = pData;
    GAME_PARAM* gp = pWork;
    NET_PARAM* pNet = gp->pNetParam;


    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }

    if(pNet->pCLACT[pAnim->clactNo]==NULL){
        pNet->pCLACT[pAnim->clactNo] = YT_InitNetworkFallChr(gp,( netID - 1),pAnim->type,0);
        GF_ASSERT(pNet->pCLACT[pAnim->clactNo]);
    }
    else{
        GF_ASSERT(0);
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


    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }

    //OS_TPrintf("testAnim %d %d\n",pAnim->clactNo,pAnim->anmseq);

    GF_ASSERT(pAnim->clactNo < YT_CLACT_MAX);
    
    if(pNet->pCLACT[pAnim->clactNo]==NULL){
        GF_ASSERT(0);
    }
    else{
        GFL_CLACT_WK_SetAnmSeq(pNet->pCLACT[pAnim->clactNo],pAnim->anmseq);
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

    if(pNet->pNetHandle[1]!=pNetHandle){
        return;
    }
    if(netID==GFL_NET_GetNetID(pNet->pNetHandle[1])){
        return;
    }

    if(pNet->pp == NULL){
        pNet->pp = YT_InitPlayer(gp,pAnim->player_no,pAnim->player_no,FALSE);
    }
    else{
        YT_NetPlayerChrTrans(gp, pNet->pp, pAnim->player_no, pAnim->anm_no, pAnim->line_no, pAnim->rot, pAnim->pat_no);
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

void YT_NET_SendPlayerAnmReq(int player_no,int anm_no,int pat_no,u16 line_no,int rot,NET_PARAM* pNet)
{
    COMM_PLAYER_ANIM_ST CommPlayerAnmSt;

    if(pNet){
        CommPlayerAnmSt.player_no = player_no;
        CommPlayerAnmSt.anm_no = anm_no;
        CommPlayerAnmSt.line_no = line_no;
        CommPlayerAnmSt.rot = rot;
        CommPlayerAnmSt.pat_no = pat_no;

        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_PLAYERANIM, &CommPlayerAnmSt);
    }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ΐ펞�ɂ́A���b�V�[�A�j���[�V�����𑗐M
 */
//-----------------------------------------------------------------------------

void YT_NetSendYossyBirthAnime(u8 posx,u8 posy,u8 count,NET_PARAM* pNet)
{
    COMM_YOSSY_ANIM_ST sYossy;
    if(pNet){
        sYossy.posx = posx;
        sYossy.posy = posy;
        sYossy.count = count;

        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_YOSSYANIM, &sYossy);
    }
}




//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ΐ펞�ɂ́A�A�j���[�V����type�𑗐M����
 */
//-----------------------------------------------------------------------------

void YT_NET_SendAnmCreate(int clactno,u16 type,NET_PARAM* pNet)
{
    COMM_ANIM_CREATE_ST CommAnmCreateSt;
    if(pNet){

        CommAnmCreateSt.clactNo = clactno;
        CommAnmCreateSt.type = type;

        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_ANIM_CREATE, &CommAnmCreateSt);
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
 *	@brief	�ʐM�ΐ펞�ɂ́A�X�N���[��Make�𑗐M����
 */
//-----------------------------------------------------------------------------

void YT_NET_SendPlayerScreenMake(int player_no, int old_line_no, int new_line_no,NET_PARAM* pNet)
{
    COMM_SCREENMAKE_ST CommScreenMakeSt;
    if(pNet){

        CommScreenMakeSt.player_no = player_no;
        CommScreenMakeSt.old_line_no = old_line_no;
        CommScreenMakeSt.new_line_no = new_line_no;

        GFL_NET_SendData(pNet->pNetHandle[1], TY_NET_COMMAND_SCREENMAKE, &CommScreenMakeSt);
    }

}

GLOBAL void	YT_PlayerRotateScreenMakeNetFunc(PLAYER_PARAM *pp, u8 player_no, u8 line_no, u8 flag);



//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�ΐ펞�ɂ́A�X�N���[��Make�𑗐M����
 */
//-----------------------------------------------------------------------------

void YT_NET_SendPlayerRotateScreenMake(u8 player_no, u8 line_no, u8 flag,NET_PARAM* pNet)
{
    COMM_SCREENMAKE_ROT_ST CommScreenMakeRotSt;
    if(pNet){

        CommScreenMakeRotSt.player_no = player_no;
        CommScreenMakeRotSt.line_no = line_no;
        CommScreenMakeRotSt.flag = flag;

        GFL_NET_SendData(pNet->pNetHandle[1], TY_NET_COMMAND_SCREENMAKE_ROT, &CommScreenMakeRotSt);
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
    //OS_TPrintf("m %d\n", pNet->seq);
    switch(pNet->seq){
#if GFL_NET_WIFI
      case _CONNECT_WIFI:
        GFL_NET_WIFI_InitUserData(aGFLNetInit.myUserData);
        pNet->pNetHandle[1] = GFL_NET_CreateHandle();
        GFL_NET_WifiLogin( );    // wifiLogin �}�b�`���O�ҋ@�ֈړ�
        _SEQCHANGE( _CONNECTING_WIFI );
        break;
      case _CONNECTING_WIFI:
        if( GFL_NET_IsWifiLobby() ){
            GFL_NET_StartRandomMatch(); // �����_���}�b�`��Ԃɂ���
            _SEQCHANGE( _WIFI_MATCHING );
        }
        break;
      case _WIFI_MATCHING:
        {
            int localConnect= GFL_NET_WIFI_GetLocalConnectNo();
            if(localConnect != -1){
                if(localConnect==0){
                    pNet->pNetHandle[0] = GFL_NET_CreateHandle();   // �n���h���쐬
                    GFL_NET_InitServer(pNet->pNetHandle[0]);       // �T�[�o�[�ɂ���
                }
                _SEQCHANGE( _CONNECT_WAIT );
            }
        }
        break;
#endif
      case _INIT_WAIT_PARENT:
        if(GFL_NET_IsInit()){
            GFL_NET_InitServer(); // �����ڑ�
            _SEQCHANGE(_CONNECT_WAIT);
        }
        break;
      case _INIT_WAIT_CHILD:
        if(GFL_NET_IsInit()){
//            pNet->pNetHandle[1] = GFL_NET_CreateHandle();   // �n���h���쐬
            GFL_NET_StartBeaconScan(); // �����ڑ�
            _SEQCHANGE( _SEARCH_CHILD );
        }
        break;
      case _SEARCH_CHILD:
        {
            u8* pData = GFL_NET_GetBeaconMacAddress(0);//�r�[�R�����X�g��0�Ԗڂ𓾂�
            if(pData){
                GFL_NET_ConnectToParent(pData);
                _SEQCHANGE( _CONNECT_WAIT );
            }
        }
        break;
      case _CONNECT_WAIT:
        _SEQCHANGE( _NEGO_START );
        break;
      case _NEGO_START:
        if(YT_NET_IsParent(pNet)){  //�e�@�̏ꍇ
            if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
                if(GFL_NET_HANDLE_RequestNegotiation(  )){
                    _SEQCHANGE( _TIM_START );
                }
            }
        }
        else{
//            if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
                if(GFL_NET_HANDLE_RequestNegotiation()){
                    _SEQCHANGE( _TIM_START );
                }
  //          }
        }
        break;
      case _TIM_START:
        if(GFL_NET_HANDLE_GetNumNegotiation()==2){
            pNet->pNetHandle[1] = GFL_NET_HANDLE_GetCurrentHandle();   // �n���h���쐬
            GFL_NET_TimingSyncStart(pNet->pNetHandle[1] , _TEST_TIMING);
            _SEQCHANGE( _TIM_OK );
        }
        break;
      case _TIM_OK:
        if(GFL_NET_IsTimingSync(pNet->pNetHandle[1] , _TEST_TIMING)){
            pNet->bGameStart = TRUE;
#if GFL_NET_WIFI
            GFL_NET_SetWifiBothNet(FALSE);  // WIFI��default�Ŕ񓯊��ڑ��ɂ��Ă���
#endif
            GFL_NET_SetNoChildErrorCheck(TRUE);
            GFL_NET_SetAutoErrorCheck(TRUE);
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
    GFL_TCBSYS* tcbsys;
    NET_PARAM* pNet = GFL_HEAP_AllocMemory(gp->heapID, sizeof(NET_PARAM));

    GFL_STD_MemClear(pNet, sizeof(NET_PARAM));
    aGFLNetInit.pWork = gp;
    gp->pNetParam = pNet;

    //�C�N�j���[�������g�p����O�� VRAMD��disable�ɂ���K�v������̂���
    //VRAMD�����Ɏg���Ă����̂����킩��Ȃ��ƁA�������Ƃ��ł��Ȃ�
    GX_DisableBankForLCDC(); // LCDC��VRAMD�����蓖�Ă��Ă�悤�Ȃ̂őł�����

    GFL_NET_Init(&aGFLNetInit);

    if(aGFLNetInit.bWiFi){
        if(bParent){
            pNet->seq = _CONNECT_WIFI;
        }
        else{
            pNet->seq = _CONNECT_WIFI;
        }
    }
    else{
        if(bParent){
            pNet->seq = _INIT_WAIT_PARENT;
        }
        else{
            pNet->seq = _INIT_WAIT_CHILD;
        }
    }

    
//	GFL_TCB_AddTask(gp->tcbsys,YT_NET_Main,pNet,1);
}


//----------------------------------------------------------------------------
/**
 *	@brief	SE��炷(+�ʐM�ő���)
 */
//-----------------------------------------------------------------------------
void YT_NET_PlaySE(int se_no, NET_PARAM* pNet)
{
    GFL_SOUND_PlaySE(se_no);
    if(pNet){
        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_PLAY_SE, &se_no);
    }
}


//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[�����ʂ𑗂�
 */
//-----------------------------------------------------------------------------
void YT_NET_SendGameResult(int player, int bWin, NET_PARAM* pNet)
{
    COMM_RESULT_ST sResult;
    if(pNet){
        sResult.player = player;
        sResult.bWin = bWin;
        GFL_NET_SendData(pNet->pNetHandle[1], YT_NET_COMMAND_GAMERESULT, &sResult);
    }
}


