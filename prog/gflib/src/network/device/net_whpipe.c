//=============================================================================
/**
 * @file	net_whpipe.c
 * @brief	�ʐM�V�X�e��
 * @author	GAME FREAK Inc.
 * @date    2006.12.05
 */
//=============================================================================

#include "gflib.h"

#include "../net_def.h"
#include "../net_system.h"
//#include "wm_icon.h"

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/cht.h>

#include "net_whpipe.h"
#include "gf_standard.h"

#include "../tool/net_ring_buff.h"




//==============================================================================
// extern�錾
//==============================================================================


/**
 *  @brief _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
 */
#define _BEACON_SIZE_FIX (10)
#define _BEACON_SSIDHEAD_SIZE (4)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)

//��l�Ńe�X�g���s���ꍇ�A�����̐��𑼂̐l�Ƃ킯��
#ifdef DEBUG_ONLY_FOR_ohno
#define _DEBUG_ALONETEST (2)
#else
#define _DEBUG_ALONETEST (0)
#endif

/**
 *  @brief �r�[�R���\����
 */
// WM_SIZE_USER_GAMEINFO  �ő� 112byte
// _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
typedef struct{
    u8        ssidHead[_BEACON_SSIDHEAD_SIZE];         ///< SSID�̂S�o�C�g����
    GameServiceID  		serviceNo; ///< �ʐM�T�[�r�X�ԍ�
    u8        debugAloneTest;      ///< �f�o�b�O�p �����Q�[���ł��r�[�R�����E��Ȃ��悤��
    u8  		connectNum;    	   ///< �Ȃ����Ă���䐔  --> �{�e���ǂ�������
    u8        pause;               ///< �ڑ����֎~���������Ɏg�p����
    u8       aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
} _GF_BSS_DATA_INFO;


//==============================================================================
// ��`
//==============================================================================

#define _DEFAULT_TIMEOUT_FRAME (60 * 10)  //�E�����r�[�R����ۑ����Ă������� 60frame * 10sec
//#define _PORT_DATA_RETRANSMISSION   (14)    // �ؒf����܂Ŗ����đ����s��  ��������g�p���Ă���
#define _NOT_INIT_BITMAP  (0xffff)   // �ڑ��l�����Œ�Ɏw��Ȃ��ꍇ�̒l

typedef enum{    // �ؒf���
    _DISCONNECT_NONE,
    _DISCONNECT_END,
    _DISCONNECT_SECRET,
    _DISCONNECT_STEALTH,
} _DisconnectState_e;


//�Ǘ��\���̒�`
struct _NET_WL_WORK{
    PTRCommRecvLocalFunc recvCallback; ///< ��M�R�[���o�b�N�����p
    NetBeaconCompFunc beaconCompFunc;  ///< �r�[�R���̔�r���s���R�[���o�b�N
    NetBeaconGetFunc beaconGetFunc;    ///< �r�[�R���f�[�^�擾�֐�
    NetBeaconGetSizeFunc beaconGetSizeFunc;  ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
    WMBssDesc sBssDesc[SCAN_PARENT_COUNT_MAX];  ///< �e�@�̏����L�����Ă���\����
    u8  backupBssid[GFL_NET_MACHINE_MAX][WM_SIZE_BSSID];   // ���܂Őڑ����Ă���
    u16 bconUnCatchTime[SCAN_PARENT_COUNT_MAX]; ///< �e�@�̃r�[�R�����E��Ȃ���������+�f�[�^�����邩�ǂ���
    void* _pWHWork;                           ///wh���C�u�������g�p���郏�[�N�̃|�C���^
    _PARENTFIND_CALLBACK pCallback;
    HEAPID heapID;
    u8 bScanCallBack;  ///< �e�̃X�L���������������ꍇTRUE, ������FALSE
  //  u8 regulationNo;   ///< �Q�[�����M�����[�V����
    GameServiceID serviceNo;
    u8 maxConnectNum;
    u32 ggid;
    u8 gameInfoBuff[WM_SIZE_USER_GAMEINFO];  //���M����r�[�R���f�[�^
    u16 errCheckBitmap;      ///< ����BITMAP���H���Ⴄ�ƃG���[�ɂȂ�
    u8 channel;
    u8 disconnectType;    ///< �ؒf��
    u8 bSetReceiver;
    u8 bEndScan;  // endscan
    u8 bPauseConnect;   ///< �q�@�̎�t���~���
    u8 bErrorState:1;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
    u8 bErrorCheck:1;   ///< �q�@�������ꍇ+�N�����������ꍇ�G���[�������邩�ǂ���
    u8 bTGIDChange:1;
    u8 bAutoExit:1;
    u8 bEntry:1;        ///< �q�@�̐V�K�Q��
} ;

//==============================================================================
// ���[�N
//==============================================================================

///< ���[�N�\���̂̃|�C���^
////static _COMM_WORK* _pCommMP = NULL;

// �e�@�ɂȂ�ꍇ��TGID �\���̂ɓ���Ă��Ȃ��̂�
// �ʐM���C�u�����[��؂����Ƃ��Ă��C���N�������g����������
/// TGID�Ǘ�
static u16 _sTgid = 0;


// �R���|�[�l���g�]���I���̊m�F�p
// �C�N�j���[�����R���|�[�l���g�������ړ�������Ƃ��͂�����ړ�
//static volatile int   startCheck;	

//==============================================================================
// static�錾
//==============================================================================

static void _whInitialize(HEAPID heapID,GFL_NETWL* pNetWL);
static void _childDataInit(void);
static void _parentDataInit(BOOL bTGIDChange);
static void _commInit(GFL_NETWL* pNetWL);
//static void _setUserGameInfo( void );
static BOOL _isMachBackupMacAddress(u8* pMac);
static u16 _getServiceBeaconPeriod(u16 serviceNo);

static BOOL _scanCallback(WMBssDesc *bssdesc);
static void _startUpCallback(void *arg, WVRResult result);
static void _indicateCallback(void *arg);
static int _connectNum(void);


//==============================================================================
/**
 * @brief   �ڑ��N���X�̃��[�N�m��
 * @param   heapID   ���[�N�m��ID
 * @retval  _COMM_WORK�̃|�C���^
 */
//==============================================================================
void* GFL_NET_WLGetHandle(HEAPID heapID, GameServiceID serviceNo, u8 num)
{
    int i;
    GFL_NETWL* pNetWL = NULL;
    GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
    
    pNetWL = (GFL_NETWL*)GFL_HEAP_AllocMemory(heapID, sizeof(GFL_NETWL));
    MI_CpuClear8(pNetWL, sizeof(GFL_NETWL));
    pNetWL->heapID = heapID;
    pNetWL->ggid = pInit->ggid;
    pNetWL->serviceNo = serviceNo;
    pNetWL->maxConnectNum = num;
    return pNetWL;
}

//==============================================================================
/**
 * @brief   �ڑ��N���X�̏�����
 * @param   heapID    ���[�N�m��ID
 * @param   getFunc   �r�[�R���쐬�֐�
 * @param   getSize   �r�[�R���T�C�Y�֐�
 * @param   getComp   �r�[�R����r�֐�
 * @param   bConnect  �ʐM����ꍇTRUE
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLInitialize(HEAPID heapID,NetBeaconGetFunc getFunc,NetBeaconGetSizeFunc getSize, NetBeaconCompFunc getComp)
{
    int i;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    pNetWL->beaconGetFunc = getFunc;    ///< �r�[�R���f�[�^�擾�֐�
    pNetWL->beaconGetSizeFunc = getSize;  ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
    pNetWL->beaconCompFunc = getComp;

    // �������C�u�����쓮�J�n
    _whInitialize(heapID, pNetWL);

    pNetWL->disconnectType = _DISCONNECT_NONE;
}

//==============================================================================
/**
 * @brief   ��M�R�[���o�b�N�֐����w��
 * @param   recvCallback   ��M�R�[���o�b�N�֐�
 * @return  none
 */
//==============================================================================

void GFL_NET_WLSetRecvCallback( PTRCommRecvLocalFunc recvCallback)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    pNetWL->recvCallback = recvCallback;
}

//==============================================================================
/**
 * @brief   �ڑ����Ă��邩�ǂ���
 * @retval  TRUE  �ڑ����Ă���
 */
//==============================================================================

BOOL GFL_NET_WLIsConnect(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL==NULL){
        return FALSE;
    }
    return TRUE;
}


//==============================================================================
/**
 * @brief   �}�b�N�A�h���X�\��
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//==============================================================================

static void DEBUG_MACDISP(char* msg,WMBssDesc *bssdesc)
{
    NET_PRINT("%s %02x%02x%02x%02x%02x%02x\n",msg,
               bssdesc->bssid[0],bssdesc->bssid[1],bssdesc->bssid[2],
               bssdesc->bssid[3],bssdesc->bssid[4],bssdesc->bssid[5]);
}

//==============================================================================
/**
 * @brief   �ڑ����ėǂ����ǂ������f����
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//==============================================================================

static BOOL _scanCheck(WMBssDesc *bssdesc)
{
    int i;
    _GF_BSS_DATA_INFO* pGF;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
    int serviceNo = pNetWL->serviceNo;

    // catch�����e�f�[�^
    pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
    if(pGF->pause){
        return FALSE;  // �|�[�Y���̐e�@��BEACON����
    }
    OS_TPrintf("debugNo %d %d\n",pGF->debugAloneTest , _DEBUG_ALONETEST);
    if(pGF->debugAloneTest != _DEBUG_ALONETEST){
        return FALSE;
    }

    if(0!=GFL_STD_MemComp( pInit->getSSID(), pGF->ssidHead, _BEACON_SSIDHEAD_SIZE)){
        OS_TPrintf("beacon�s��v\n");
        return FALSE;
    }
    if(FALSE == pNetWL->beaconCompFunc(serviceNo, pGF->serviceNo)){
        return FALSE;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
    }
    if(pGF->connectNum >= pInit->maxConnectNum){
        return FALSE;   // �ڑ��l�������ς��̏ꍇ�E��Ȃ�
    }
    NET_PRINT("_scanCheckok\n");
    return TRUE;
}



//==============================================================================
/**
 * @brief   �q�@���e�@��T���o�������ɌĂ΂��R�[���o�b�N�֐�
 * �e�@���E�����тɌĂ΂��
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//==============================================================================

static BOOL _scanCallback(WMBssDesc *bssdesc)
{
    int i;
    _GF_BSS_DATA_INFO* pGF;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
    
    int serviceNo = pNetWL->serviceNo;

    if(FALSE == _scanCheck(bssdesc)){
        return FALSE;
    }
    
    // ���̃��[�v�͓������̂Ȃ̂��ǂ�������
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(pNetWL->bconUnCatchTime[i] == 0 ){
            // �e�@��񂪓����Ă��Ȃ��ꍇcontinue
            continue;
        }
        if (0==GFL_STD_MemComp(pNetWL->sBssDesc[i].bssid, bssdesc->bssid, WM_SIZE_BSSID)) {
            // ������x�E�����ꍇ�Ƀ^�C�}�[���Z
            OS_TPrintf("������x�E�����ꍇ�Ƀ^�C�}�[���Z\n");
            pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
            // �V�����e����ۑ����Ă����B
            MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i], sizeof(WMBssDesc));
            return TRUE;
        }
    }
    // ���̃��[�v�͋󂫂����邩�ǂ�������
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(pNetWL->bconUnCatchTime[i] == 0 ){
            // �e�@��񂪓����Ă��Ȃ��ꍇbreak;
            break;
        }
    }
    if(i >= SCAN_PARENT_COUNT_MAX){
        // �\���̂������ς��̏ꍇ�͐e�@���E��Ȃ�
        return FALSE;
    }
    // �V�����e����ۑ����Ă����B
    pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
    MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i],sizeof(WMBssDesc));
    pNetWL->bScanCallBack = TRUE;
    return TRUE;
}



//==============================================================================
/**
 * @brief   WH���C�u�����̏�����
 * @retval  none
 */
//==============================================================================

static void _whInitialize(HEAPID heapID, GFL_NETWL* pNetWL)
{
    // ����������
//    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

//    pNetWL->_pWHWork = WH_CreateHandle(heapID, pNetWL->_pWHWork,
//                                       GFI_NET_GetConnectNumMax() , GFI_NET_GetSendSizeMax());

    if(FALSE == WH_Initialize( heapID)){
        OS_TPanic("not init");
    }
    // WH �����ݒ�
    WH_SetGgid(pNetWL->ggid);

//    WHSetConnectCheckCallBack();
    
}

//==============================================================================
/**
 * @brief   �q�@�̎g�p���Ă���f�[�^�̏�����
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLChildBconDataInit(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i;

    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        pNetWL->bconUnCatchTime[i] = 0;
    }
    MI_CpuClear8(pNetWL->sBssDesc,sizeof(WMBssDesc)*SCAN_PARENT_COUNT_MAX);
}

//==============================================================================
/**
 * @brief   �e�@�̎g�p���Ă���f�[�^�̏�����
 * @param   bTGIDChange  �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @retval  none
 */
//==============================================================================

static void _parentDataInit(BOOL bTGIDChange)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    pNetWL->bTGIDChange = bTGIDChange;
}

//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̏��������܂Ƃ߂�
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================

static void _commInit(GFL_NETWL* pNetWL)
{
    pNetWL->bPauseConnect = FALSE;
    pNetWL->bScanCallBack = FALSE;
    pNetWL->bErrorState = FALSE;
    pNetWL->bErrorCheck = FALSE;
    pNetWL->bAutoExit = FALSE;
    pNetWL->bEndScan = 0;
    pNetWL->bSetReceiver = FALSE;
    return;
}


//==============================================================================
/**
 * @brief   ��M�R�[���o�b�N
 * @param   aid    �ʐM��localID
 * @param   *data  ��M�f�[�^
 * @param   size   ��M�T�C�Y
 * @retval  none
 */
//==============================================================================

static void _receiverFunc(u16 aid, u16 *data, u16 size)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    pNetWL->recvCallback(aid,data,size);
}


//==============================================================================
/**
 * @brief   �q�@�̐ڑ��J�n���s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   serviceNo  �Q�[���̎��
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildInit(BOOL bBconInit)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if(pNetWL == NULL){
        return FALSE;
    }
    
    _commInit(pNetWL);
    if( pNetWL->disconnectType == _DISCONNECT_STEALTH){
        return TRUE;
    }

    if(bBconInit){
        NET_PRINT("�r�[�R���̏�����\n");
        GFL_NET_WLChildBconDataInit(); // �f�[�^�̏�����
    }
    if(!pNetWL->bSetReceiver ){
        WH_SetReceiver(_receiverFunc);
        pNetWL->bSetReceiver = TRUE;
    }
    // �e�@�����X�^�[�g
    if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
        if(WH_StartScan(_scanCallback, NULL, 0)){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�؂�ւ����s���i�e�q���]�ɕK�v�ȏ����j
 * @param   none
 * @retval  ���Z�b�g������TRUE
 */
//==============================================================================

BOOL GFL_NET_WLSwitchParentChild(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(!pNetWL){
        return TRUE;
    }
    if(pNetWL->disconnectType == _DISCONNECT_STEALTH){
        return TRUE;
    }
    switch(pNetWL->bEndScan){
      case 0:
        if(WH_SYSSTATE_SCANNING == WH_GetSystemState()){
            NET_PRINT("�X�L����������------1\n");
            WH_EndScan();
            pNetWL->bEndScan = 1;
            break;
        }
        else if(WH_SYSSTATE_BUSY == WH_GetSystemState()){  //���΂炭�҂�
        }
        else{
            WH_Finalize();
            pNetWL->bEndScan = 2;
        }
        break;
      case 1:
        if(WH_SYSSTATE_BUSY != WH_GetSystemState()){
            NET_PRINT("�I������----2\n");
            WH_Finalize();
            pNetWL->bEndScan = 2;
        }
        break;
      case 2:
        if(WH_SYSSTATE_BUSY != WH_GetSystemState()){
            return TRUE;
        }
        if(WH_SYSSTATE_ERROR == WH_GetSystemState()){
            pNetWL->bEndScan = 1;
        }
        break;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�ؒf���s��  �����ł͂����܂ŒʐM�I���葱���ɓ��邾��
 *  �z���g�ɏ����͉̂���_commEnd
 * @param   none
 * @retval  �I�������Ɉڂ����ꍇTRUE
 */
//==============================================================================
BOOL GFL_NET_WLFinalize(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        if(pNetWL->disconnectType == _DISCONNECT_NONE){
            pNetWL->disconnectType = _DISCONNECT_END;
            WH_Finalize();
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�ؒf���s��  �������������[�J�����s��Ȃ�
 * @param   �ؒf�̏ꍇTRUE
 * @retval  none
 */
//==============================================================================
void GFL_NET_WLStealth(BOOL bStalth)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if(!pNetWL){
        return;
    }
    if(bStalth){
        pNetWL->disconnectType = _DISCONNECT_SECRET;
    }
    else{
        pNetWL->disconnectType = _DISCONNECT_NONE;
        _whInitialize(pNetWL->heapID, pNetWL);  // �����쓮�ĊJ
    }
}

//==============================================================================
/**
 * @brief   �ʐM�̑S�Ă�����
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _commEnd(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

 //   WH_DestroyHandle(pNetWL->_pWHWork);
    GFL_HEAP_FreeMemory(pNetWL);
    _GFL_NET_SetNETWL( NULL );
}

//==============================================================================
/**
 * @brief   �T�����Ƃ��ł����e�̐���Ԃ�
 * @param   none
 * @retval  �e�@�̐�
 */
//==============================================================================

int GFL_NET_WLGetParentCount(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i, cnt;

    cnt = 0;
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(pNetWL->bconUnCatchTime[i] != 0 ){
            cnt++;
        }
    }
    return cnt;
}

//--------------------------------------------------------------
/**
 * @brief   BmpList�̈ʒu����Bcon�̑��݂���Index�ʒu���擾����
 * @param   index		BmpList�ʒu
 * @retval  Index
 */
//--------------------------------------------------------------
int CommBmpListPosBconIndexGet(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
	int i, count;
	
//	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
//		OS_TPrintf("List�̃r�[�R���`�F�b�N %d = %d\n", i, pNetWL->bconUnCatchTime[i]);
//	}
	
	count = 0;
	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
		if(pNetWL->bconUnCatchTime[i] != 0){
			if(count == index){
				return i;
			}
			count++;
		}
	}
	GF_ASSERT(0 && "�����ɂ͗��Ȃ��͂�");
	return 0;
}

//==============================================================================
/**
 * @brief   �e�@���X�g�ɕω����������ꍇTRUE
 * @param   none
 * @retval  �e�@���X�g�ɕω����������ꍇTRUE �Ȃ����FALSE
 */
//==============================================================================

BOOL GFL_NET_WLIsScanListChange(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return pNetWL->bScanCallBack;
}

//==============================================================================
/**
 * @brief   �e�@�̕ω���m�点��t���O�����Z�b�g����
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLResetScanChangeFlag(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    pNetWL->bScanCallBack = FALSE;
}

//==============================================================================
/**
 * @brief   ���̐e�@�������ƃR�l�N�V�����������Ă���̂��𓾂�
 * @param   index   �e�̃��X�g��index
 * @retval  �R�l�N�V������ 0-16
 */
//==============================================================================

int GFL_NET_WLGetParentConnectionNum(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int cnt;
    _GF_BSS_DATA_INFO* pGF;

    cnt = 0;
    if(pNetWL->bconUnCatchTime[index] != 0){
        pGF = (_GF_BSS_DATA_INFO*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
        if(pGF->connectNum==0){
            return 1;
        }
        return pGF->connectNum;
    }
    return 0;
}

//==============================================================================
/**
 * @brief   �ڑ��l���ɊY������e��Ԃ�
 * @param   none
 * @retval  �e��index
 */
//==============================================================================

static int _getParentNum(int machNum)
{
    int i,num;

    for (i = SCAN_PARENT_COUNT_MAX-1; i >= 0; i--) {
        num = GFL_NET_WLGetParentConnectionNum(i);
        if((num > machNum) && (num < GFL_NET_MACHINE_MAX)){
            return i;
        }
    }
    return -1;
}

//==============================================================================
/**
 * @brief   �����ɐڑ����Ă����l�����������ꍇindex��Ԃ�
 * @param   none
 * @retval  �Y��������index��Ԃ�
 */
//==============================================================================

int GFL_NET_WLGetFastConnectIndex(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i,num;

    if(GFL_NET_WLGetParentCount()==0){
        return -1;
    }
    for (i = SCAN_PARENT_COUNT_MAX -1; i >= 0; i--) {
        if(pNetWL->bconUnCatchTime[i] != 0){
            if(_isMachBackupMacAddress(&pNetWL->sBssDesc[i].bssid[0])){  // �Â�MAC�ɍ��v
                num = GFL_NET_WLGetParentConnectionNum(i);
                if(( num > 1) && (num < GFL_NET_MACHINE_MAX)){      // �{�e�ɊY������ �܂��Q���\
                    return i;
                }
            }
        }
    }
    return -1;
}

//==============================================================================
/**
 * @brief   ���̃��x���Ōq���ł����l�������炻��index��Ԃ��܂�
 * @param   none
 * @retval  �Y��������index��Ԃ�
 */
//==============================================================================

int GFL_NET_WLGetNextConnectIndex(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i;
 
    if(GFL_NET_WLGetParentCount()==0){  // �r�[�R�����������
        return -1;
    }
    for (i = SCAN_PARENT_COUNT_MAX-1; i >= 0; i--) {
        if(pNetWL->bconUnCatchTime[i] != 0){
            if(_isMachBackupMacAddress(&pNetWL->sBssDesc[i].bssid[0])){  // �Â�MAC�ɍ��v
                NET_PRINT("�̂̐e %d\n",i);
                return i;
            }
        }
    }
    i = _getParentNum(1);
    if(i != -1 ){
        NET_PRINT("�����Ȃ��{�e %d \n", i);
        return i;
    }
    i = _getParentNum(0);
    if(i != -1){
        NET_PRINT("�����Ȃ����e %d \n", i);
        return i;
    }
    return i;
}

static BOOL _parentFindCallback(WMBssDesc* pBeacon)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if(_scanCheck(pBeacon)==FALSE){
        return FALSE;
    }
    if(pNetWL->pCallback){
        pNetWL->pCallback();
    }
    return TRUE;
}

//==============================================================================
/**
 * @brief   �q�@�@MP��ԂŐڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
/*
BOOL GFL_NET_WLChildIndexConnect(u16 index, _PARENTFIND_CALLBACK pCallback, GFL_NETHANDLE* pNetHandle)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int serviceNo;
    
    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();
        u16 mode;
        NET_PRINT("�q�@ �ڑ��J�n WH_ChildConnect\n");
        serviceNo = pNetWL->serviceNo;
        pNetWL->channel = pNetWL->sBssDesc[index].channel;
        pNetWL->bEndScan = 0;
        pNetWL->pNetHandle = pNetHandle;
        pNetWL->pCallback = pCallback;
        if(pInit->bMPMode){
            mode = WH_CONNECTMODE_MP_PARENT;
        }
        else{
            mode = WH_CONNECTMODE_DS_PARENT;
        }
        WH_ChildConnectAuto(mode, pNetWL->sBssDesc[index].bssid, 0);
        WH_SetScanCallback(_parentFindCallback);
        return TRUE;
    }
    return FALSE;
}
*/

void GFI_NET_BeaconSetScanCallback(_PARENTFIND_CALLBACK pCallback)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    pNetWL->bEndScan = 0;
    pNetWL->pCallback = pCallback;
    WH_SetScanCallback(_parentFindCallback);
}



//==============================================================================
/**
 * @brief   �q�@�@MP��ԂŐڑ�
 * @param   macAddress    �}�b�N�A�h���X
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildMacAddressConnect(u8* macAddress, _PARENTFIND_CALLBACK pCallback)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        NET_PRINT("�q�@ �ڑ��J�n\n");
        pNetWL->bEndScan = 0;
        pNetWL->pCallback = pCallback;
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, macAddress, 0);
        WH_SetScanCallback(_parentFindCallback);
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �r�[�R���f�[�^�̒���m�F
 *  �ڑ�����������Ԃł̊ԁA���̊֐����ĂсA�^�C���A�E�g�������s��
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLParentBconCheck(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int id;

    for(id = 0; id < SCAN_PARENT_COUNT_MAX; id++){
        if(pNetWL->bconUnCatchTime[id] == 0 ){
            continue;
        }
        if(pNetWL->bconUnCatchTime[id] > 0){
            pNetWL->bconUnCatchTime[id]--;
            if(pNetWL->bconUnCatchTime[id] == 0){
                NET_PRINT("�e�@�����Ȃ� %d\n", id);
                pNetWL->bScanCallBack = TRUE;   // �f�[�^��ύX�����̂�TRUE
            }
        }
    }
}

//==============================================================================
/**
 *  @brief   ���[�U��`�̐e�@����ݒ肵�܂��B
 *  _GF_BSS_DATA_INFO�\���̂̒��g�𑗂�܂�
 *  @param   userGameInfo  ���[�U��`�̐e�@���ւ̃|�C���^
 *  @param   length        ���[�U��`�̐e�@���̃T�C�Y
 *  @retval  none
 */
//==============================================================================
void GFI_NET_BeaconSetInfo( void )
{
    u8 macBuff[6];
    _GF_BSS_DATA_INFO* pGF;
    int size;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    GFLNetInitializeStruct* pInit = _GFL_NET_GetNETInitStruct();

    if(pNetWL->beaconGetSizeFunc==NULL){
        OS_TPanic("beaconGetSizeFunc none");
        return;
    }
    {
        NetBeaconGetSizeFunc func = pNetWL->beaconGetSizeFunc;
        size = func();
    }
    if((WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX) <= size){
        OS_TPanic("size over");
        return;
    }
    pGF = (_GF_BSS_DATA_INFO*)pNetWL->gameInfoBuff;
    pGF->serviceNo = pNetWL->serviceNo;    // �Q�[���̔ԍ�
    pGF->debugAloneTest = _DEBUG_ALONETEST;
    GFL_STD_MemCopy( pInit->getSSID(), pGF->ssidHead, _BEACON_SSIDHEAD_SIZE);

    pGF->pause = pNetWL->bPauseConnect;
    GFL_STD_MemCopy( pNetWL->beaconGetFunc(), pGF->aBeaconDataBuff, size);
    DC_FlushRange(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
    WH_SetUserGameInfo((u16*)pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
}


//==============================================================================
/**
 *  @brief   ���[�U��`�̐e�@���� �l���������Ď�����ɍX�V����
 *  _GF_BSS_DATA_INFO�\���̂̒��g�𑗂�܂�
 *  @param   none
 *  @retval  none
 */
//==============================================================================
static void _funcBconDataChange( void )
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    _GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->gameInfoBuff;
    int size;

//    OS_TPrintf("%x \n",(u32)pNetWL);
    {
        NetBeaconGetSizeFunc func = pNetWL->beaconGetSizeFunc;
        size = func();
    }

    if(_connectNum() != pGF->connectNum){
        pGF->connectNum = _connectNum();
        DC_FlushRange(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
        WH_SetUserGameInfo((u16*)pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
        WHSetGameInfo(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX,
                      pNetWL->ggid,_sTgid);
    }
}

//==============================================================================
/**
 * @brief   �ʐM���C�u�����[�����̏�Ԃ����āA����������֐�
 * VBlank�Ƃ͊֌W�Ȃ��̂�process�̎��ɏ�������΂���
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _stateProcess(u16 bitmap)
{
    int state = WH_GetSystemState();
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    _funcBconDataChange();      // �r�[�R���̒��g������������
    if(pNetWL->errCheckBitmap == _NOT_INIT_BITMAP){
        pNetWL->errCheckBitmap = bitmap;  // ���̂Ƃ��̐ڑ��l����ێ�
    }
    if(pNetWL->bErrorCheck){
        if((WH_GetCurrentAid() == GFL_NET_PARENT_NETID) && (!GFL_NET_WLIsChildsConnecting())){
            pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
        }
        if(pNetWL->errCheckBitmap > bitmap){  // �ؒf�����ꍇ�K������������ �����镪�ɂ�OK
            pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
        }
    }
    if(WH_ERRCODE_FATAL == WH_GetLastError()){
        GFI_NET_FatalErrorFunc(0);
    }
    switch (state) {
      case WH_SYSSTATE_STOP:
        if(pNetWL->disconnectType == _DISCONNECT_END){
            NET_PRINT("WHEnd ���Ă�ŏI�����܂���\n");
            _commEnd();  // ���[�N���牽����S�ĊJ��
            return;
        }
        if(pNetWL->disconnectType == _DISCONNECT_SECRET){
            NET_PRINT("WHEnd ���Ă�Ŏ��񂾂ӂ�J�n\n");
            pNetWL->disconnectType = _DISCONNECT_STEALTH;
            return;
        }
        break;
      case WH_SYSSTATE_IDLE:
        if(pNetWL->disconnectType == _DISCONNECT_END){
            if(WH_End()){
                return;
            }
        }
        if(pNetWL->disconnectType == _DISCONNECT_SECRET){
            if(WH_End()){
                return;
            }
        }
        break;
      case WH_SYSSTATE_CONNECT_FAIL:
      case WH_SYSSTATE_ERROR:
        NET_PRINT("�G���[�� %d \n",WH_GetLastError());
        if(pNetWL){
            pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
        }
        break;
      default:
        break;
    }

}

//==============================================================================
/**
 * @brief   �ʐM���C�u�����[�����̏�Ԃ����āA����������֐�
 * VBlank�Ƃ͊֌W�Ȃ��̂�process�̎��ɏ�������΂���
 * �q�@�͂��݂��̐ڑ����킩��Ȃ��̂ŁA�ʐM���ʂ�commsystem���������ăG���[��������
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommMpProcess(u16 bitmap)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        _stateProcess(bitmap);
    }
}

//==============================================================================
/**
 * @brief   �f�o�C�X���ʐM�\��ԂȂ̂��ǂ�����Ԃ�
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//==============================================================================
BOOL GFL_NET_WL_IsConnectLowDevice(u16 netID)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(!pNetWL){
        return FALSE;
    }

    if((WH_GetCurrentAid() == GFL_NET_PARENT_NETID) && (!GFL_NET_WLIsChildsConnecting())){
        return FALSE;
    }
    
    if ((WH_GetSystemState() == WH_SYSSTATE_CONNECTED) || (WH_GetSystemState() == WH_SYSSTATE_DATASHARING)) {
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �ʐM�\��ԂȂ̂��ǂ�����Ԃ�
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//==============================================================================
static BOOL _isConnect(u16 netID)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(!pNetWL){
        return FALSE;
    }
    if (WH_GetSystemState() != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }
    {
        u16 bitmap = WH_GetBitmap();
        if( bitmap & (1<<netID)){
            return TRUE;
        }
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
static int _connectNum(void)
{
    int num = 0,i;

    for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
        if(_isConnect(i)){
            num++;
        }
    }
    return num;
}

//==============================================================================
/**
 * @brief  �ʐM�ؒf���[�h�ɂ͂��������ǂ���
 * @param   none
 * @retval  �ڑ��l��
 */
//==============================================================================

BOOL GFL_NET_WLIsConnectStalth(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->disconnectType == _DISCONNECT_STEALTH)){
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief  ���������Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �������I����Ă�����TRUE
 */
//==============================================================================
BOOL GFL_NET_WLIsInitialize(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return (pNetWL!=NULL);
}

//==============================================================================
/**
 * @brief  WH���C�u�����Ł@��Ԃ�IDLE�ɂȂ��Ă��邩�m�F����
 * @param   none
 * @retval  IDLE�ɂȂ��Ă���=TRUE
 */
//==============================================================================

BOOL GFL_NET_WLIsStateIdle(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        return WH_GetSystemState() == WH_SYSSTATE_IDLE;
    }
    return TRUE;
}


//==============================================================================
/**
 * @brief  WH���C�u�����Ł@�ʐM��Ԃ�BIT���m�F  �q�@���Ȃ����Ă��邩�ǂ���
 * @param   none
 * @retval  �Ȃ�������TRUE
 */
//==============================================================================

BOOL GFL_NET_WLIsChildsConnecting(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        return( WH_GetBitmap() & 0xfffe);
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief  �e�@�����������ǂ���
 * @param   none
 * @retval  �������ꍇTRUE
 */
//==============================================================================

BOOL GFL_NET_WLParentDisconnect(void)
{
    if(GFL_NET_WLIsError() && ( WH_ERRCODE_DISCONNECTED == WH_GetLastError())){
        return TRUE;
    }
    return FALSE;
}


//==============================================================================
/**
 * @brief  �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�̎�TRUE
 */
//==============================================================================

BOOL GFL_NET_WLIsError(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        if(pNetWL->bErrorState){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �N�����������̂��G���[�����ɂ��邩�ǂ�����SET
 * @param   bOn  �L������TRUE
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetDisconnectOtherError(BOOL bOn)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        pNetWL->bErrorCheck = bOn;
        pNetWL->errCheckBitmap = _NOT_INIT_BITMAP;
    }
}

//==============================================================================
/**
 * @brief   �T�[�r�X�ԍ��ɑΉ������r�[�R���Ԋu�𓾂܂�
 *          �T�[�r�X�ԍ��� include/communication/comm_def.h�ɂ���܂�
 * @param   serviceNo �T�[�r�X�ԍ�
 * @retval  beacon�Ԋu msec
 */
//==============================================================================

u16 _getServiceBeaconPeriod(u16 serviceNo)
{
    u16 beaconPeriod = WM_GetDispersionBeaconPeriod();
/*  �K�v�Ȃ�R�[���o�b�N�֐��Ƃ��Ē�`����
    ��{�I�ɂ͕K�v�Ƃ��Ȃ�������  k.ohno 06.12.05 */
    return beaconPeriod;
}

//==============================================================================
/**
 * @brief   �r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   WMBssDesc*  �r�[�R���o�b�t�@�|�C���^
 */
//==============================================================================

WMBssDesc* GFL_NET_WLGetWMBssDesc(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        return &pNetWL->sBssDesc[index];
    }
    return NULL;
}

//==============================================================================
/**
 * @brief    �r�[�R�����x�𓾂�
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   u16  �ʐM���x
 */
//==============================================================================

u16 GFL_NET_WL_GetRssi(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        u16 rate_rssi,rssi;
        return pNetWL->sBssDesc[index].rssi;
    }
    return 0;
}

//==============================================================================
/**
 * @brief   GF�r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   GF_BSS_DATA_INFO*  �r�[�R���o�b�t�@�|�C���^
 */
//==============================================================================

void* GFL_NET_WLGetGFBss(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        return (void*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
    }
    return NULL;
}

//==============================================================================
/**
 * @brief   User��`�̃r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   GF_BSS_DATA_INFO*  �r�[�R���o�b�t�@�|�C���^
 */
//==============================================================================

void* GFL_NET_WLGetUserBss(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        _GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
        return (void*)pGF->aBeaconDataBuff;
    }
    return NULL;
}

//==============================================================================
/**
 * @brief   �r�[�R���̒��ɂ���}�b�N�A�h���X�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @return  maccaddress�̔z��
 */
//==============================================================================

void* GFL_NET_WLGetUserMacAddress(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        return (void*)&pNetWL->sBssDesc[index].bssid[0];
    }
    return NULL;
}

//==============================================================================
/**
 * @brief    �r�[�R���f�[�^������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//==============================================================================

void GFL_NET_WLResetWMBssDesc(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        pNetWL->bconUnCatchTime[index] = 0;
    }
}

//==============================================================================
/**
 * @brief    �r�[�R���f�[�^������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//==============================================================================

void GFL_NET_WLResetGFBss(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
        pNetWL->bconUnCatchTime[index] = 0;
    }
}

//==============================================================================
/**
 * @brief   mac�A�h���X���o�b�N�A�b�v����
 * @param   pMac   mac address
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetBackupMacAddress(u8* pMac, int netID)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL==NULL){
        OS_Panic("no mem");
    }
    if(GFL_NET_MACHINE_MAX > netID){
        MI_CpuCopy8(pMac, pNetWL->backupBssid[netID], WM_SIZE_BSSID);
    }
}

//==============================================================================
/**
 * @brief   �o�b�N�A�b�v����MAC�A�h���X�ɊY�����邩�ǂ����𓾂�
 * @param   pMac   mac address
 * @retval  none
 */
//==============================================================================

static BOOL _isMachBackupMacAddress(u8* pMac)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i;
    
    for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
        if(WM_IsBssidEqual(pNetWL->backupBssid[i], pMac)){
            return TRUE;
        }
    }
    return FALSE;
}


//==============================================================================
/**
 * @brief   �����ؒf���[�h�ɓ��������ǂ�����Ԃ�
 * @param   none
 * @retval  �����Ă���Ȃ��TRUE
 */
//==============================================================================

BOOL GFL_NET_WLIsAutoExit(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        return pNetWL->bAutoExit;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �����ؒf���[�hON
 * @param   none
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetAutoExit(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        pNetWL->bAutoExit = TRUE;
    }
}

//==============================================================================
/**
 * @brief   �r�[�R���f�[�^�Ɍ��݂̏󋵂𔽉f������
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLFlashMyBss(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int size;
    {
        NetBeaconGetSizeFunc func = pNetWL->beaconGetSizeFunc;
        size = func();
    }

    GFI_NET_BeaconSetInfo();
    WHSetGameInfo(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX,
                  pNetWL->ggid,_sTgid);
}

//==============================================================================
/**
 * @brief   ���C�t�^�C�������������� �܂��͌��ɖ߂�
 * @param   bMinimum TRUE�Ȃ珬��������
 * @retval  none
 */
//==============================================================================

//void GFL_NET_WLSetLifeTime(BOOL bMinimum)
//{
  //  WHSetLifeTime(bMinimum);
//}

//------------------------------------------------------
/**
 * @brief   �T�[�r�X�ԍ��̃r�[�R������Ԃ��܂�
 * @param   serviceNo   comm_def.h�ɂ���T�[�r�X�ԍ�
 * @retval  ���p�Ґ�
 */
//------------------------------------------------------

int GFL_NET_WLGetServiceNumber(int serviceNo)
{
    int i,num=0;
    
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; i++) {
        _GF_BSS_DATA_INFO* pGF = GFL_NET_WLGetGFBss(i);
        if(pGF){
            if( pGF->serviceNo == serviceNo){
                num += pGF->connectNum;
            }
        }
    }
    //
    return num;
}

//------------------------------------------------------
/**
 * @brief   �e�@�����ł��r�[�R���𑗐M���I��������ǂ���
 * @retval  ���M����=TRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsParentBeaconSent(void)
{
    return (0!=WHGetBeaconSendNum());
}

//------------------------------------------------------
/**
 * @brief   �X�L������Ԃ��ǂ���
 * @retval  �X�L������ԂȂ�TRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsChildStateScan(void)
{
    return WH_GetSystemState() == WH_SYSSTATE_SCANNING;
}

//------------------------------------------------------
/**
 * @brief   �`�����l����Ԃ�
 * @retval  �ڑ��`�����l��
 */
//------------------------------------------------------

int GFL_NET_WLGetChannel(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return pNetWL->channel;
}

//==============================================================================
/**
 * @brief	���@�r�[�R�����M���e�̎擾
 * @param none
 * @return �r�[�R���̒��g
 */
//==============================================================================
void* GFL_NET_WLGetMyGFBss(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
	if (pNetWL == NULL) {
		return NULL;
	}
	return pNetWL->gameInfoBuff;
}

//------------------------------------------------------
/**
 * @brief	�r�[�R�����E���Ă���̃J�E���g�_�E���^�C�}�擾
 * @param	index	�ʒu
 */
//------------------------------------------------------
int GFL_NET_WLGetBConUncacheTime(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
	if (pNetWL == NULL) {
		return 0;
	}
	return pNetWL->bconUnCatchTime[index];
}


//------------------------------------------------------
/**
 * @brief	�f�[�^���M����
 * @param	data	���M�f�[�^�|�C���^
 * @param	size    ���M�T�C�Y
 * @param	callback  ���M�����R�[���o�b�N
 * @retval  TRUE   ���M�ł������ł���
 * @retval  FALSE  ���M�ł��Ȃ�
 */
//------------------------------------------------------
BOOL GFL_NET_WL_SendData(void* data,int size,PTRSendDataCallback callback)
{
    return WH_SendData(data, size, callback);
}

//------------------------------------------------------
/**
 * @brief	�ڑ���Ԃ�BITMAP�ŕԂ�
 * @return  �ڑ����
 */
//------------------------------------------------------

u16 GFL_NET_WL_GetBitmap(void)
{
    return WH_GetBitmap();
}

//------------------------------------------------------
/**
 * @brief	�����̐ڑ�ID��Ԃ�
 * @return  �ڑ�ID
 */
//------------------------------------------------------

u16 GFL_NET_WL_GetCurrentAid(void)
{
    return WH_GetCurrentAid();
}

