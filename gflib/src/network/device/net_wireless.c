//=============================================================================
/**
 * @file	net_wireless.c
 * @brief	DS�̃f�o�C�X�ւ̃A�N�Z�X�����b�v����֐�
 * @author	Katsumi Ohno
 * @date    2006.01.25
 */
//=============================================================================


#include "gflib.h"
#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/cht.h>

#include "net_wireless.h"
#include "gf_standard.h"

#include "../tool/net_ring_buff.h"

//==============================================================================
// extern�錾
//==============================================================================


/**
 *  @brief _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
 */
#define _BEACON_SIZE_FIX (3)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)
/**
 *  @brief �r�[�R���\����
 */
// WM_SIZE_USER_GAMEINFO  �ő� 112byte
// _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
typedef struct{
  GameServiceID  		serviceNo;   	///< �ʐM�T�[�r�X�ԍ�
  u8  		connectNum;    	///< �Ȃ����Ă���䐔  --> �{�e���ǂ�������	
  u8        pause;          ///< �ڑ����֎~���������Ɏg�p����
  u8       aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
} _GF_BSS_DATA_INFO;


//==============================================================================
// ��`
//==============================================================================

/// �E�����r�[�R����ۑ����Ă�������
#define _DEFAULT_TIMEOUT_FRAME (60 * 10)  //60frame * 10sec

#define _PORT_DATA_RETRANSMISSION   (14)    // �ؒf����܂Ŗ����đ����s��  ��������g�p���Ă���
#define _PORT_DATA_PARENT           _PORT_DATA_RETRANSMISSION
#define _PORT_DATA_CHILD            _PORT_DATA_RETRANSMISSION

#define _KEEP_CHANNEL_TIME_MAX   (5)

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
    GFL_NETHANDLE* pNetHandle;
    _PARENTFIND_CALLBACK pCallback;
    HEAPID heapID;
    u8 bScanCallBack;  ///< �e�̃X�L���������������ꍇTRUE, ������FALSE
  //  u8 regulationNo;   ///< �Q�[�����M�����[�V����
    GameServiceID serviceNo;
    u8 maxConnectNum;
    u32 ggid;
    u8 gameInfoBuff[WM_SIZE_USER_GAMEINFO];  //���M����r�[�R���f�[�^
    u16 keepChannelNo;
    u16 errCheckBitmap;      ///< ����BITMAP���H���Ⴄ�ƃG���[�ɂȂ�
    u8 channel;
    u8 keepChannelTime;
    u8 disconnectType;    ///< �ؒf��
    u8 bSetReceiver;
    u8 bEndScan;  // endscan
    u8 bErrorState:1;     ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
    u8 bErrorDisconnectOther:1;     ///< �N����������ƃG���[�ɂȂ�܂�
    u8 bErrorNoChild:1;  ///< �q�@�������ꍇ�G���[�������邩�ǂ���
    u8 bTGIDChange:1;
    u8 bAutoExit:1;
    u8 bEntry:1;   // �q�@�̐V�K�Q��
//    u8 bStalth:1;
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
static volatile int   startCheck;	

//==============================================================================
// static�錾
//==============================================================================

static void _whInitialize(HEAPID heapID,GFL_NETWL* pNetWL, BOOL bConnect);
static void _childDataInit(void);
static void _parentDataInit(BOOL bTGIDChange);
static void _commInit(GFL_NETWL* pNetWL);
static void _setUserGameInfo( void );
static BOOL _isMachBackupMacAddress(u8* pMac);
static u16 _getServiceBeaconPeriod(u16 serviceNo);

static void _scanCallback(WMBssDesc *bssdesc);
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
    
    pNetWL = (GFL_NETWL*)GFL_HEAP_AllocMemory(heapID, sizeof(GFL_NETWL));
    MI_CpuClear8(pNetWL, sizeof(GFL_NETWL));
    pNetWL->heapID = heapID;
    pNetWL->ggid = _DP_GGID;
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

void GFL_NET_WLInitialize(HEAPID heapID,NetBeaconGetFunc getFunc,NetBeaconGetSizeFunc getSize, NetBeaconCompFunc getComp, BOOL bConnect)
{
    int i;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    pNetWL->beaconGetFunc = getFunc;    ///< �r�[�R���f�[�^�擾�֐�
    pNetWL->beaconGetSizeFunc = getSize;  ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
    pNetWL->beaconCompFunc = getComp;

    // �������C�u�����쓮�J�n
    _whInitialize(heapID, pNetWL, bConnect);

    if(bConnect){
        pNetWL->disconnectType = _DISCONNECT_NONE;
    }
    else{
        pNetWL->disconnectType = _DISCONNECT_STEALTH;
    }
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
 * @brief   �q�@���e�@��T���o�������ɌĂ΂��R�[���o�b�N�֐�
 * �e�@���E�����тɌĂ΂��
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

static void _scanCallback(WMBssDesc *bssdesc)
{
    int i;
    _GF_BSS_DATA_INFO* pGF;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    
    int serviceNo = pNetWL->serviceNo;

    // catch�����e�f�[�^
    pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
    if(pGF->pause){
        return;  // �|�[�Y���̐e�@��BEACON����
    }
    if(FALSE == pNetWL->beaconCompFunc(serviceNo, pGF->serviceNo)){
        return;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
    }
    
    // ���̃��[�v�͓������̂Ȃ̂��ǂ�������
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(pNetWL->bconUnCatchTime[i] == 0 ){
            // �e�@��񂪓����Ă��Ȃ��ꍇcontinue
            continue;
        }
        if (GFL_STD_MemComp(pNetWL->sBssDesc[i].bssid, bssdesc->bssid, WM_SIZE_BSSID)) {
            // ������x�E�����ꍇ�Ƀ^�C�}�[���Z
            pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
            // �V�����e����ۑ����Ă����B
            MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i], sizeof(WMBssDesc));
            return;
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
        return;
    }
    // �V�����e����ۑ����Ă����B
    pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
    MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i],sizeof(WMBssDesc));
    pNetWL->bScanCallBack = TRUE;
}

//------------------------------------------------------------------
/**
 * @brief   �����쓮���䃉�C�u�����̔񓯊��I�ȏ����I�����ʒm�����R�[���o�b�N�֐��B
 * @param   arg		WVR_StartUpAsync�R�[�����Ɏw�肵�������B���g�p�B
 * @param   result	�񓯊��֐��̏������ʁB
 * @retval  none		
 */
//------------------------------------------------------------------
static void _startUpCallback(void *arg, WVRResult result)
{
    if (result != WVR_RESULT_SUCCESS) {
        OS_TPanic("WVR_StartUpAsync error.[%08xh]\n", result);
    }
    else{
		NET_PRINT("WVR Trans VRAM-D.\n");
	}
    startCheck = 2;
}

//------------------------------------------------------------------
/**
 * @brief   �����쓮���䃉�C�u�����̔񓯊��I�ȏ����I�����ʒm�����R�[���o�b�N�֐��B
 * @param   arg		WVR_StartUpAsync�R�[�����Ɏw�肵�������B���g�p�B
 * @param   result	�񓯊��֐��̏������ʁB
 * @retval  none		
 */
//------------------------------------------------------------------
static void _endCallback(void *arg, WVRResult result)
{
    startCheck = 0;
    GFL_UI_SleepEnable(GFL_UI_SLEEP_NET);  // �X���[�v����
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ�
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLVRAMDInitialize(void)
{
    int ans;
    //************************************
//	GX_DisableBankForTex();			// �e�N�X�`���C���[�W

    GFL_UI_SleepDisable(GFL_UI_SLEEP_NET);  // �X���[�v�֎~
    // �������C�u�����쓮�J�n
	// �C�N�j���[�����R���|�[�l���g��VRAM-D�ɓ]������
    startCheck = 1;
    ans = WVR_StartUpAsync(GX_VRAM_ARM7_128_D, _startUpCallback, NULL);
    if (WVR_RESULT_OPERATING != ans) {
        OS_TPanic("WVR_StartUpAsync failed. %d\n",ans);
    }
    else{
        NET_PRINT("WVRStart\n");
    }
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ����I�������1
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_WLIsVRAMDInitialize(void)
{
    return (startCheck==2);
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ����͂��߂���P
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_WLIsVRAMDStart(void)
{
    return (startCheck!=0);
}

//==============================================================================
/**
 * @brief   �C�N�j���[�����J��
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLVRAMDFinalize(void)
{
    NET_PRINT("VRAMD Finalize\n");
    WVR_TerminateAsync(_endCallback,NULL);  // �C�N�j���[�����ؒf
}


//==============================================================================
/**
 * @brief   WH���C�u�����̏�����
 * @retval  none
 */
//==============================================================================

static void _whInitialize(HEAPID heapID, GFL_NETWL* pNetWL, BOOL bConnect)
{
    // ����������
//    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    pNetWL->_pWHWork = WH_CreateHandle(heapID, pNetWL->_pWHWork,
                                       GFI_NET_GetConnectNumMax() , GFI_NET_GetSendSizeMax());

    if(FALSE == WH_Initialize(_GFL_NET_WLGetNETWH(), bConnect)){
        OS_TPanic("not init");
    }
    // WH �����ݒ�
    WH_SetGgid(pNetWL->ggid);
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
    pNetWL->bScanCallBack = FALSE;
    pNetWL->bErrorState = FALSE;
    pNetWL->bErrorNoChild = FALSE;
    
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
 * @brief   �e�@�̐ڑ��J�n���s��
 * @param   bTGIDChange  TGID��ύX���邩�ǂ���
 * @param   bEntry       
 * @param   bTGIDChange  �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @param  �q�@���󂯕t���邩�ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL GFL_NET_WLParentInit(BOOL bTGIDChange, BOOL bEntry, GFL_NET_ConnectionCallBack pConnectFunc )
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    _commInit(pNetWL);
    if( pNetWL->disconnectType == _DISCONNECT_STEALTH){
        return TRUE;
    }
    _parentDataInit(bTGIDChange);

    WH_ParentDataInit();
    
    WHSetConnectCallBack(pConnectFunc);
    
    if(!pNetWL->bSetReceiver){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_CHILD);
      pNetWL->bSetReceiver = TRUE;
    }
    pNetWL->bEntry = bEntry;

    // �d�g�g�p������œK�ȃ`�����l�����擾���Đڑ�����B�񓯊��֐�
    if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
        if(WH_StartMeasureChannel()){
            return TRUE;
        }
    }
    return FALSE;
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
    
    _commInit(pNetWL);
    if( pNetWL->disconnectType == _DISCONNECT_STEALTH){
        return TRUE;
    }

    if(bBconInit){
        NET_PRINT("�r�[�R���̏�����\n");
        GFL_NET_WLChildBconDataInit(); // �f�[�^�̏�����
    }
    if(!pNetWL->bSetReceiver ){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_PARENT);
        pNetWL->bSetReceiver = TRUE;
    }
    // �e�@�����X�^�[�g
    if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
        // MAC�A�h���X�w��@�@�S��FF�őS�Ă�T���ɂ���
        const u8 sAnyParent[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        if(WH_StartScan(_scanCallback, sAnyParent, _SCAN_ALL_CHANNEL)){
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
        if(WH_IsSysStateScan()){
            NET_PRINT("�X�L����������------1\n");
            WH_EndScan();
            pNetWL->bEndScan = 1;
            break;
        }
        else if(WH_IsSysStateBusy()){  //���΂炭�҂�
        }
        else{
            WH_Finalize();
            pNetWL->bEndScan = 2;
        }
        break;
      case 1:
        if(!WH_IsSysStateBusy()){
            NET_PRINT("�I������----2\n");
            WH_Finalize();
            pNetWL->bEndScan = 2;
        }
        break;
      case 2:
        if(WH_IsSysStateIdle()){
            return TRUE;
        }
        if(WH_IsSysStateError()){
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
        _whInitialize(pNetWL->heapID, pNetWL, TRUE);  // �����쓮�ĊJ
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

    WH_DestroyHandle(pNetWL->_pWHWork);
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

static void _parentFindCallback(WMBssDesc* pBeacon)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if(pNetWL->pCallback){
        pNetWL->pCallback(pNetWL->pNetHandle);
    }
}

//==============================================================================
/**
 * @brief   �q�@�@MP��ԂŐڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildIndexConnect(u16 index, _PARENTFIND_CALLBACK pCallback, GFL_NETHANDLE* pNetHandle)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int serviceNo;
    
    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        NET_PRINT("�q�@ �ڑ��J�n WH_ChildConnect\n");
        serviceNo = pNetWL->serviceNo;
        pNetWL->channel = pNetWL->sBssDesc[index].channel;
        pNetWL->bEndScan = 0;
        pNetWL->pNetHandle = pNetHandle;
        pNetWL->pCallback = pCallback;
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, pNetWL->sBssDesc[index].bssid, 0, _parentFindCallback);
//      WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &(pNetWL->sBssDesc[index]));
//        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, pNetWL->sBssDesc[index].bssid,0);
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   �q�@�@MP��ԂŐڑ�
 * @param   macAddress    �}�b�N�A�h���X
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildMacAddressConnect(u8* macAddress, _PARENTFIND_CALLBACK pCallback, GFL_NETHANDLE* pNetHandle)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        NET_PRINT("�q�@ �ڑ��J�n\n");
        pNetWL->bEndScan = 0;
        pNetWL->pNetHandle = pNetHandle;
        pNetWL->pCallback = pCallback;
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, macAddress, 0, _parentFindCallback);
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
static void _setUserGameInfo( void )
{
    u8 macBuff[6];
    _GF_BSS_DATA_INFO* pGF;
    int size;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

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
    pGF->serviceNo = pNetWL->serviceNo;  // �Q�[���̔ԍ�
    pGF->pause = WHGetParentConnectPause();    // �e�@����t�𒆎~���Ă��邩�ǂ���
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
    if((WH_GetCurrentAid() == COMM_PARENT_ID) && (!GFL_NET_WLIsChildsConnecting())){
        if(pNetWL->bErrorNoChild){
            pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
//            NET_PRINT("�G���[�� NOCHILD \n");
        }
    }
    if(pNetWL->errCheckBitmap == _NOT_INIT_BITMAP){
        pNetWL->errCheckBitmap = bitmap;  // ���̂Ƃ��̐ڑ��l����ێ�
    }
    if(pNetWL->bErrorDisconnectOther){ // �G���[�������s��
        if(pNetWL->errCheckBitmap > bitmap){  // �ؒf�����ꍇ�K������������ �����镪�ɂ�OK
            pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
//            NET_PRINT("�G���[�� �N�������� \n");
        }
    }
    if(WH_ERRCODE_FATAL == WH_GetLastError()){
        GFI_NET_FatalErrorFunc(NULL, 0);
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
      case WH_SYSSTATE_MEASURECHANNEL:
        {
            u16 channel;
            // ���p�\�Ȓ������Ԏg�p���̒Ⴂ�`�����l����Ԃ��܂��B
            channel = WH_GetMeasureChannel();  //WH_SYSSTATE_MEASURECHANNEL => WH_SYSSTATE_IDLE
            if(pNetWL->keepChannelTime==0){
                pNetWL->keepChannelNo = channel;
                pNetWL->keepChannelTime = _KEEP_CHANNEL_TIME_MAX;
            }
            else{
                pNetWL->keepChannelTime--;
            }
            channel = pNetWL->keepChannelNo;  
            if(pNetWL->bTGIDChange){
                _sTgid++;
            }
            _setUserGameInfo();
//            NET_PRINT("�e�@�ڑ��J�n   tgid=%d channel=%d \n",_sTgid, channel);
            (void)WH_ParentConnect(WH_CONNECTMODE_MP_PARENT,
                                   _sTgid, channel,
                                   pNetWL->maxConnectNum,
                                   _getServiceBeaconPeriod(pNetWL->serviceNo),
                                   pNetWL->bEntry);
            pNetWL->channel = channel;
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

    if((WH_GetCurrentAid() == COMM_PARENT_ID) && (!GFL_NET_WLIsChildsConnecting())){
        return FALSE;
    }
    
    if (WH_GetSystemState() != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }

    
    return TRUE;
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
        return WH_IsSysStateIdle();
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
 * @brief  �q�@�����Ȃ��̂��G���[�����ɂ��邩�ǂ�����SET
 * @param   bOn  �L������TRUE
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetNoChildError(BOOL bOn)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        pNetWL->bErrorNoChild = bOn;
    }
}

//==============================================================================
/**
 * @brief �N�����������̂��G���[�����ɂ��邩�ǂ�����SET
 * @param   bOn  �L������TRUE
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetDisconnectOtherError(BOOL bOn)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(pNetWL){
        pNetWL->bErrorDisconnectOther = bOn;
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

    _setUserGameInfo();
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

void GFL_NET_WLSetLifeTime(BOOL bMinimum)
{
    WHSetLifeTime(bMinimum);
}

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
    return WHIsParentBeaconSent();
}

//------------------------------------------------------
/**
 * @brief   �X�L������Ԃ��ǂ���
 * @retval  ���M����=TRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsChildStateScan(void)
{
    return WH_IsSysStateScan();
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
    return WH_SendData(data, size,
                       _PORT_DATA_RETRANSMISSION, callback);
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

//------------------------------------------------------
/**
 * @brief	WH�̃��[�N�|�C���^��Ԃ�
 * @return  GFL_NETWM�̃|�C���^
 */
//------------------------------------------------------

GFL_NETWM* _GFL_NET_WLGetNETWH(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    u32 addr;
    if(pNetWL == NULL){
        return NULL;
    }
    addr = (u32)pNetWL->_pWHWork;
    if(addr % 32){
        addr += 32 - (addr % 32);
    }
    return (GFL_NETWM*)addr;
}

