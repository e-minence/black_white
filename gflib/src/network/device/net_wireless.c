//=============================================================================
/**
 * @file	net_ds.c
 * @brief	DS�̃f�o�C�X�ւ̃A�N�Z�X�����b�v����֐�
 * @author	Katsumi Ohno
 * @date    2006.01.25
 */
//=============================================================================


#include "gflib.h"
#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/cht.h>

#include "wh.h"
#include "wh_config.h"
#include "net.h"
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
typedef struct{
    PTRCommRecvLocalFunc recvCallback; ///< ��M�R�[���o�b�N�����p
    NetBeaconCompFunc beaconCompFunc;  ///< �r�[�R���̔�r���s���R�[���o�b�N
    NetBeaconGetFunc beaconGetFunc;    ///< �r�[�R���f�[�^�擾�֐�
    NetBeaconGetSizeFunc beaconGetSizeFunc;  ///< �r�[�R���f�[�^�T�C�Y�擾�֐�
    WMBssDesc sBssDesc[SCAN_PARENT_COUNT_MAX];  ///< �e�@�̏����L�����Ă���\����
    u8  backupBssid[GFL_NET_MACHINE_MAX][WM_SIZE_BSSID];   // ���܂Őڑ����Ă���
    u16 bconUnCatchTime[SCAN_PARENT_COUNT_MAX]; ///< �e�@�̃r�[�R�����E��Ȃ���������+�f�[�^�����邩�ǂ���
    void* _pWHWork;                           ///wh���C�u�������g�p���郏�[�N�̃|�C���^
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
} _COMM_WORK;

//==============================================================================
// ���[�N
//==============================================================================

///< ���[�N�\���̂̃|�C���^
static _COMM_WORK* _pCommMP = NULL;

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

static void _whInitialize(void);
static void _childDataInit(void);
static void _parentDataInit(BOOL bTGIDChange);
static void _commInit(void);
static void _setUserGameInfo( void );
static BOOL _isMachBackupMacAddress(u8* pMac);
static u16 _getServiceBeaconPeriod(u16 serviceNo);

static void _scanCallback(WMBssDesc *bssdesc);
static void _startUpCallback(void *arg, WVRResult result);
static void _indicateCallback(void *arg);
static int _connectNum(void);

//==============================================================================
/**
 * @brief   �ڑ��N���X�̏�����
 * @param   heapID   ���[�N�m��ID
 * @retval  _COMM_WORK�̃|�C���^
 */
//==============================================================================

void* CommMPInitialize(int heapID, GameServiceID serviceNo, u8 num)
{
    int i;
    _COMM_WORK* pCommMP = NULL;
    
    pCommMP = (_COMM_WORK*)GFL_HEAP_AllocMemory(heapID, sizeof(_COMM_WORK));
    MI_CpuClear8(pCommMP, sizeof(_COMM_WORK));
    pCommMP->_pWHWork = GFL_HEAP_AllocMemory(heapID, WH_GetHeapSize());
    MI_CpuClear8(_pCommMP->_pWHWork, WH_GetHeapSize());
    pCommMP->ggid = _DP_GGID;
    pCommMP->serviceNo = serviceNo;
    _pCommMP->maxConnectNum = num;
    // �������C�u�����쓮�J�n
    _whInitialize();
    return pCommMP;
}

//==============================================================================
/**
 * @brief   �ڑ����Ă��邩�ǂ��� (�폜�\��)
 * @retval  TRUE  �ڑ����Ă���
 */
//==============================================================================

BOOL CommMPIsConnect(void)
{
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
    OHNO_PRINT("%s %02x%02x%02x%02x%02x%02x\n",msg,
               bssdesc->bssid[0],bssdesc->bssid[1],bssdesc->bssid[2],
               bssdesc->bssid[3],bssdesc->bssid[4],bssdesc->bssid[5]);
}

static void _scanCallback(WMBssDesc *bssdesc)
{
    int i;
    _GF_BSS_DATA_INFO* pGF;
    int serviceNo = _pCommMP->serviceNo;

    // catch�����e�f�[�^
    pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
    if(pGF->pause){
        return;  // �|�[�Y���̐e�@��BEACON����
    }
    if(FALSE == _pCommMP->beaconCompFunc(serviceNo, pGF->serviceNo)){
        return;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
    }
    
    // ���̃��[�v�͓������̂Ȃ̂��ǂ�������
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(_pCommMP->bconUnCatchTime[i] == 0 ){
            // �e�@��񂪓����Ă��Ȃ��ꍇcontinue
            continue;
        }
        if (GFL_STD_MemComp(_pCommMP->sBssDesc[i].bssid, bssdesc->bssid, WM_SIZE_BSSID)) {
            // ������x�E�����ꍇ�Ƀ^�C�}�[���Z
            _pCommMP->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
            // �V�����e����ۑ����Ă����B
            MI_CpuCopy8( bssdesc, &_pCommMP->sBssDesc[i], sizeof(WMBssDesc));
            return;
        }
    }
    // ���̃��[�v�͋󂫂����邩�ǂ�������
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(_pCommMP->bconUnCatchTime[i] == 0 ){
            // �e�@��񂪓����Ă��Ȃ��ꍇbreak;
            break;
        }
    }
    if(i >= SCAN_PARENT_COUNT_MAX){
        // �\���̂������ς��̏ꍇ�͐e�@���E��Ȃ�
        return;
    }
    // �V�����e����ۑ����Ă����B
    _pCommMP->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
    MI_CpuCopy8( bssdesc, &_pCommMP->sBssDesc[i],sizeof(WMBssDesc));
    _pCommMP->bScanCallBack = TRUE;
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
		OS_Printf("WVR Trans VRAM-D.\n");
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
    //sys_SleepOK(SLEEPTYPE_COMM);  // �X���[�v��������  @@OO ��Ŗ��ߍ���
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ�
 * @retval  none
 */
//==============================================================================

void CommVRAMDInitialize(void)
{
    //************************************
//	GX_DisableBankForTex();			// �e�N�X�`���C���[�W

//    sys_SleepNG(SLEEPTYPE_COMM);  // �X���[�v���֎~   @@OO ��Ŗ��ߍ���
    // �������C�u�����쓮�J�n
	// �C�N�j���[�����R���|�[�l���g��VRAM-D�ɓ]������
    startCheck = 1;
    if (WVR_RESULT_OPERATING != WVR_StartUpAsync(GX_VRAM_ARM7_128_D, _startUpCallback, NULL)) {
        OS_TPanic("WVR_StartUpAsync failed. \n");
    }
    else{
        OHNO_PRINT("WVRStart\n");
    }
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ����I�������1
 * @retval  none
 */
//==============================================================================

BOOL CommIsVRAMDInitialize(void)
{
    return (startCheck==2);
}

//==============================================================================
/**
 * @brief   WVR��VRAMD�Ɉړ����͂��߂���P
 * @retval  none
 */
//==============================================================================

BOOL CommIsVRAMDStart(void)
{
    return (startCheck!=0);
}

//==============================================================================
/**
 * @brief   �C�N�j���[�����J��
 * @retval  none
 */
//==============================================================================

void CommVRAMDFinalize(void)
{
    OHNO_PRINT("VRAMD Finalize\n");
    WVR_TerminateAsync(_endCallback,NULL);  // �C�N�j���[�����ؒf
}


//==============================================================================
/**
 * @brief   WH���C�u�����̏�����
 * @retval  none
 */
//==============================================================================

static void _whInitialize(void)
{
    // ����������
    {
        u32 addr = (u32)_pCommMP->_pWHWork;
        addr = 32 - (addr % 32) + addr;   //32byte�A���C�����g
        (void)WH_Initialize((void*)addr);
    }

    // WH �����ݒ�
    WH_SetGgid(_pCommMP->ggid);

    // WEP Key �̎�p�̗��������@�̏�����
//    CommRandSeedInitialize(&_pCommMP->sRand);
}

//==============================================================================
/**
 * @brief   �q�@�̎g�p���Ă���f�[�^�̏�����
 * @retval  none
 */
//==============================================================================

void ChildBconDataInit(void)
{
    int i;

    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        _pCommMP->bconUnCatchTime[i] = 0;
    }
    MI_CpuClear8(_pCommMP->sBssDesc,sizeof(WMBssDesc)*SCAN_PARENT_COUNT_MAX);
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
    _pCommMP->bTGIDChange = bTGIDChange;
}

//==============================================================================
/**
 * @brief   �e�q���ʁA�ʐM�̏��������܂Ƃ߂�
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================

static void _commInit(void)
{
    _pCommMP->bScanCallBack = FALSE;
    _pCommMP->bErrorState = FALSE;
    _pCommMP->bErrorNoChild = FALSE;
    
    _pCommMP->disconnectType = _DISCONNECT_NONE;
    _pCommMP->bAutoExit = FALSE;
    _pCommMP->bEndScan = 0;

    _pCommMP->bSetReceiver = FALSE;
    
    return;
}


//==============================================================================
/**
 * @brief   �ʐM���C�u�����ɕK�v�ȃ��[�N�T�C�Y��Ԃ�
 * @retval  ���[�N�T�C�Y
 */
//==============================================================================
u32 CommGetWorkSize(void)
{
    OHNO_PRINT("_COMM_WORK size %d \n", sizeof(_COMM_WORK));
    return sizeof(_COMM_WORK);
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

void _receiverFunc(u16 aid, u16 *data, u16 size)
{
    _pCommMP->recvCallback(aid,data,size);
}

//==============================================================================
/**
 * @brief   �e�@�̐ڑ��J�n���s��
 * @param   bAlloc       alloc���邩�ǂ���
 * @param   bTGIDChange  TGID��ύX���邩�ǂ���
 * @param   bEntry       
 * @param   bTGIDChange  �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @param  �q�@���󂯕t���邩�ǂ���
 * @retval  �������ɐ���������TRUE
 */
//==============================================================================
BOOL CommMPParentInit(BOOL bAlloc, BOOL bTGIDChange, BOOL bEntry, GFL_NET_ConnectionCallBack pConnectFunc )
{
    _commInit();
    _parentDataInit(bTGIDChange);

    WH_ParentDataInit();
    
    WHSetConnectCallBack(pConnectFunc);
    
    if(!_pCommMP->bSetReceiver){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_CHILD);
      _pCommMP->bSetReceiver = TRUE;
    }
    _pCommMP->bEntry = bEntry;

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
BOOL CommMPChildInit(BOOL bAlloc, BOOL bBconInit)
{
    _commInit();
    if(bBconInit){
        OHNO_PRINT("�r�[�R���̏�����\n");
        ChildBconDataInit(); // �f�[�^�̏�����
    }
    if(!_pCommMP->bSetReceiver ){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_PARENT);
        _pCommMP->bSetReceiver = TRUE;
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

BOOL CommMPSwitchParentChild(void)
{
    if(!_pCommMP){
        return TRUE;
    }
    switch(_pCommMP->bEndScan){
      case 0:
        if(WH_IsSysStateScan()){
            WH_EndScan();
            _pCommMP->bEndScan = 1;
            break;
        }
        else if(WH_IsSysStateBusy()){  //���΂炭�҂�
        }
        else{
            WH_Finalize();
            _pCommMP->bEndScan = 2;
        }
        break;
      case 1:
        if(!WH_IsSysStateBusy()){
            WH_Finalize();
            _pCommMP->bEndScan = 2;
        }
        break;
      case 2:
        if(WH_IsSysStateIdle()){
            return TRUE;
        }
        if(WH_IsSysStateError()){
            _pCommMP->bEndScan = 1;
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
BOOL CommMPFinalize(void)
{
    if(_pCommMP){
        if(_pCommMP->disconnectType == _DISCONNECT_NONE){
            _pCommMP->disconnectType = _DISCONNECT_END;
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
void CommMPStealth(BOOL bStalth)
{
    if(!_pCommMP){
        return;
    }
    if(bStalth){
        _pCommMP->disconnectType = _DISCONNECT_SECRET;
    }
    else{
        _pCommMP->disconnectType = _DISCONNECT_NONE;
        _whInitialize();  // �����쓮�ĊJ
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
    GFL_HEAP_FreeMemory(_pCommMP->_pWHWork);
    GFL_HEAP_FreeMemory(_pCommMP);
    _pCommMP = NULL;
}

//==============================================================================
/**
 * @brief   �T�����Ƃ��ł����e�̐���Ԃ�
 * @param   none
 * @retval  �e�@�̐�
 */
//==============================================================================

int CommMPGetParentCount(void)
{
    int i, cnt;

    cnt = 0;
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(_pCommMP->bconUnCatchTime[i] != 0 ){
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
	int i, count;
	
//	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
//		OS_TPrintf("List�̃r�[�R���`�F�b�N %d = %d\n", i, _pCommMP->bconUnCatchTime[i]);
//	}
	
	count = 0;
	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
		if(_pCommMP->bconUnCatchTime[i] != 0){
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

BOOL CommMPIsScanListChange(void)
{
    return _pCommMP->bScanCallBack;
}

//==============================================================================
/**
 * @brief   �e�@�̕ω���m�点��t���O�����Z�b�g����
 * @param   none
 * @retval  none
 */
//==============================================================================

void CommMPResetScanChangeFlag(void)
{
    _pCommMP->bScanCallBack = FALSE;
}

//==============================================================================
/**
 * @brief   ���̐e�@�������ƃR�l�N�V�����������Ă���̂��𓾂�
 * @param   index   �e�̃��X�g��index
 * @retval  �R�l�N�V������ 0-16
 */
//==============================================================================

int CommMPGetParentConnectionNum(int index)
{
    int cnt;
    _GF_BSS_DATA_INFO* pGF;

    cnt = 0;
    if(_pCommMP->bconUnCatchTime[index] != 0){
        pGF = (_GF_BSS_DATA_INFO*)_pCommMP->sBssDesc[index].gameInfo.userGameInfo;
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
        num = CommMPGetParentConnectionNum(i);
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

int CommMPGetFastConnectIndex(void)
{
    int i,num;

    if(CommMPGetParentCount()==0){
        return -1;
    }
    for (i = SCAN_PARENT_COUNT_MAX -1; i >= 0; i--) {
        if(_pCommMP->bconUnCatchTime[i] != 0){
            if(_isMachBackupMacAddress(&_pCommMP->sBssDesc[i].bssid[0])){  // �Â�MAC�ɍ��v
                num = CommMPGetParentConnectionNum(i);
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

int CommMPGetNextConnectIndex(void)
{
    int i;
 
    if(CommMPGetParentCount()==0){  // �r�[�R�����������
        return -1;
    }
    for (i = SCAN_PARENT_COUNT_MAX-1; i >= 0; i--) {
        if(_pCommMP->bconUnCatchTime[i] != 0){
            if(_isMachBackupMacAddress(&_pCommMP->sBssDesc[i].bssid[0])){  // �Â�MAC�ɍ��v
                OHNO_PRINT("�̂̐e %d\n",i);
                return i;
            }
        }
    }
    i = _getParentNum(1);
    if(i != -1 ){
        OHNO_PRINT("�����Ȃ��{�e %d \n", i);
        return i;
    }
    i = _getParentNum(0);
    if(i != -1){
        OHNO_PRINT("�����Ȃ����e %d \n", i);
        return i;
    }
    return i;
}

//==============================================================================
/**
 * @brief   �q�@�@MP��ԂŐڑ�
 * @param   index   �e�̃��X�g��index
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//==============================================================================
BOOL CommMPChildIndexConnect(u16 index)
{
    int serviceNo;
    
    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        OHNO_PRINT("�q�@ �ڑ��J�n WH_ChildConnect\n");
        serviceNo = _pCommMP->serviceNo;
        _pCommMP->channel = _pCommMP->sBssDesc[index].channel;
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, _pCommMP->sBssDesc[index].bssid,0);
//      WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &(_pCommMP->sBssDesc[index]));
//        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, _pCommMP->sBssDesc[index].bssid,0);
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

void CommMPParentBconCheck(void)
{
    int id;

    for(id = 0; id < SCAN_PARENT_COUNT_MAX; id++){
        if(_pCommMP->bconUnCatchTime[id] == 0 ){
            continue;
        }
        if(_pCommMP->bconUnCatchTime[id] > 0){
            _pCommMP->bconUnCatchTime[id]--;
            if(_pCommMP->bconUnCatchTime[id] == 0){
                OHNO_PRINT("�e�@�����Ȃ� %d\n", id);
                _pCommMP->bScanCallBack = TRUE;   // �f�[�^��ύX�����̂�TRUE
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

    if(_pCommMP->beaconGetSizeFunc==NULL){
        OS_Panic("beaconGetSizeFunc none");
        return;
    }
    size = _pCommMP->beaconGetSizeFunc();
    if((WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX) <= size){
        OS_Panic("size over");
        return;
    }
    pGF = (_GF_BSS_DATA_INFO*)_pCommMP->gameInfoBuff;
    pGF->serviceNo = _pCommMP->serviceNo;  // �Q�[���̔ԍ�
    pGF->pause = WHGetParentConnectPause();    // �e�@����t�𒆎~���Ă��邩�ǂ���
    GFL_STD_MemCopy( _pCommMP->beaconGetFunc(), pGF->aBeaconDataBuff, size);
    DC_FlushRange(_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX);
    WH_SetUserGameInfo((u16*)_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX);
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
    _GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)_pCommMP->gameInfoBuff;
    int size = _pCommMP->beaconGetSizeFunc();

    if(_connectNum() != pGF->connectNum){
        pGF->connectNum = _connectNum();
        DC_FlushRange(_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX);
        WH_SetUserGameInfo((u16*)_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX);
        WHSetGameInfo(_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX,
                      _pCommMP->ggid,_sTgid);
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
//    CommInfoFunc();
    _funcBconDataChange();      // �r�[�R���̒��g������������
    if((WH_GetCurrentAid() == COMM_PARENT_ID) && (!CommMPIsChildsConnecting())){
        if(_pCommMP->bErrorNoChild){
            _pCommMP->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
//            OHNO_PRINT("�G���[�� NOCHILD \n");
        }
    }
    if(_pCommMP->errCheckBitmap == _NOT_INIT_BITMAP){
        _pCommMP->errCheckBitmap = bitmap;  // ���̂Ƃ��̐ڑ��l����ێ�
    }
    if(_pCommMP->bErrorDisconnectOther){ // �G���[�������s��
        if(_pCommMP->errCheckBitmap > bitmap){  // �ؒf�����ꍇ�K������������ �����镪�ɂ�OK
            _pCommMP->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
//            OHNO_PRINT("�G���[�� �N�������� \n");
        }
    }
    if(WH_ERRCODE_FATAL == WH_GetLastError()){
//        CommFatalErrorFunc(0);   @@OO �G���[�\��������K�v������ 06.12.5
    }
    switch (state) {
      case WH_SYSSTATE_STOP:
        if(_pCommMP->disconnectType == _DISCONNECT_END){
            OHNO_SP_PRINT("WHEnd ���Ă�ŏI�����܂���\n");
            _commEnd();  // ���[�N���牽����S�ĊJ��
            return;
        }
        if(_pCommMP->disconnectType == _DISCONNECT_SECRET){
            OHNO_SP_PRINT("WHEnd ���Ă�Ŏ��񂾂ӂ�J�n\n");
            _pCommMP->disconnectType = _DISCONNECT_STEALTH;
            return;
        }
        break;
      case WH_SYSSTATE_IDLE:
        if(_pCommMP->disconnectType == _DISCONNECT_END){
            if(WH_End()){
                return;
            }
        }
        if(_pCommMP->disconnectType == _DISCONNECT_SECRET){
            if(WH_End()){
                return;
            }
        }
        break;
      case WH_SYSSTATE_CONNECT_FAIL:
      case WH_SYSSTATE_ERROR:
        OHNO_PRINT("�G���[�� %d \n",WH_GetLastError());
        if(_pCommMP){
            _pCommMP->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
        }
        break;
      case WH_SYSSTATE_MEASURECHANNEL:
        {
            u16 channel;
            // ���p�\�Ȓ������Ԏg�p���̒Ⴂ�`�����l����Ԃ��܂��B
            channel = WH_GetMeasureChannel();  //WH_SYSSTATE_MEASURECHANNEL => WH_SYSSTATE_IDLE
            if(_pCommMP->keepChannelTime==0){
                _pCommMP->keepChannelNo = channel;
                _pCommMP->keepChannelTime = _KEEP_CHANNEL_TIME_MAX;
            }
            else{
                _pCommMP->keepChannelTime--;
            }
            channel = _pCommMP->keepChannelNo;  
            if(_pCommMP->bTGIDChange){
                _sTgid++;
            }
            _setUserGameInfo();
//            OHNO_PRINT("�e�@�ڑ��J�n   tgid=%d channel=%d \n",_sTgid, channel);
            (void)WH_ParentConnect(WH_CONNECTMODE_MP_PARENT,
                                   _sTgid, channel,
                                   _pCommMP->maxConnectNum,
                                   _getServiceBeaconPeriod(_pCommMP->serviceNo),
                                   _pCommMP->bEntry);
            _pCommMP->channel = channel;
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
    if(_pCommMP){
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
    if(!_pCommMP){
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
    if(!_pCommMP){
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

BOOL CommMPIsConnectStalth(void)
{
    if(_pCommMP && (_pCommMP->disconnectType == _DISCONNECT_STEALTH)){
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
BOOL CommMPIsInitialize(void)
{
    return (_pCommMP!=NULL);
}

//==============================================================================
/**
 * @brief  WH���C�u�����Ł@��Ԃ�IDLE�ɂȂ��Ă��邩�m�F����
 * @param   none
 * @retval  IDLE�ɂȂ��Ă���=TRUE
 */
//==============================================================================

BOOL CommMPIsStateIdle(void)
{
    if(_pCommMP){
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

BOOL CommMPIsChildsConnecting(void)
{
    if(_pCommMP){
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

BOOL CommMPParentDisconnect(void)
{
    if(CommMPIsError() && ( WH_ERRCODE_DISCONNECTED == WH_GetLastError())){
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

BOOL CommMPIsError(void)
{
    if(_pCommMP){
        if(_pCommMP->bErrorState){
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

void CommMPSetNoChildError(BOOL bOn)
{
    if(_pCommMP){
        _pCommMP->bErrorNoChild = bOn;
    }
}

//==============================================================================
/**
 * @brief �N�����������̂��G���[�����ɂ��邩�ǂ�����SET
 * @param   bOn  �L������TRUE
 * @retval  none
 */
//==============================================================================

void CommMPSetDisconnectOtherError(BOOL bOn)
{
    if(_pCommMP){
        _pCommMP->bErrorDisconnectOther = bOn;
        _pCommMP->errCheckBitmap = _NOT_INIT_BITMAP;
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
/*  �K�v�Ȃ�R�[���o�b�N�֐��Ƃ��Ē�`���� k.ohno 06.12.05
    GF_ASSERT_RETURN(serviceNo < COMM_MODE_MAX, beaconPeriod);

    if(COMM_MODE_UNDERGROUND == serviceNo){
        return beaconPeriod/4;
    }
    if((COMM_MODE_UNION == serviceNo) ||
       (COMM_MODE_PICTURE == serviceNo)){
        return (beaconPeriod / 4);
    }*/
    return beaconPeriod;
}

//==============================================================================
/**
 * @brief   �r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   WMBssDesc*  �r�[�R���o�b�t�@�|�C���^
 */
//==============================================================================

WMBssDesc* CommMPGetWMBssDesc(int index)
{
    if(_pCommMP && (_pCommMP->bconUnCatchTime[index]!=0)){
        return &_pCommMP->sBssDesc[index];
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

void* CommMPGetGFBss(int index)
{
    if(_pCommMP && (_pCommMP->bconUnCatchTime[index]!=0)){
        return (void*)_pCommMP->sBssDesc[index].gameInfo.userGameInfo;
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

void CommMPResetWMBssDesc(int index)
{
    if(_pCommMP && (_pCommMP->bconUnCatchTime[index]!=0)){
        _pCommMP->bconUnCatchTime[index] = 0;
    }
}

//==============================================================================
/**
 * @brief    �r�[�R���f�[�^������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//==============================================================================

void CommMPResetGFBss(int index)
{
    if(_pCommMP && (_pCommMP->bconUnCatchTime[index]!=0)){
        _pCommMP->bconUnCatchTime[index] = 0;
    }
}

//==============================================================================
/**
 * @brief   mac�A�h���X���o�b�N�A�b�v����
 * @param   pMac   mac address
 * @retval  none
 */
//==============================================================================

void CommMPSetBackupMacAddress(u8* pMac, int netID)
{
    if(_pCommMP==NULL){
        OS_Panic("no mem");
    }
    if(GFL_NET_MACHINE_MAX > netID){
        MI_CpuCopy8(pMac, _pCommMP->backupBssid[netID], WM_SIZE_BSSID);
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
    int i;
    
    for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
        if(WM_IsBssidEqual(_pCommMP->backupBssid[i], pMac)){
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

BOOL CommMPIsAutoExit(void)
{
    if(_pCommMP){
        return _pCommMP->bAutoExit;
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

void CommMPSetAutoExit(void)
{
    if(_pCommMP){
        _pCommMP->bAutoExit = TRUE;
    }
}

//==============================================================================
/**
 * @brief   �r�[�R���f�[�^�Ɍ��݂̏󋵂𔽉f������
 * @retval  none
 */
//==============================================================================

void CommMPFlashMyBss(void)
{
    int size = _pCommMP->beaconGetSizeFunc();

    _setUserGameInfo();
    WHSetGameInfo(_pCommMP->gameInfoBuff, size + _BEACON_SIZE_FIX,
                  _pCommMP->ggid,_sTgid);
}

//==============================================================================
/**
 * @brief   ���C�t�^�C�������������� �܂��͌��ɖ߂�
 * @param   bMinimum TRUE�Ȃ珬��������
 * @retval  none
 */
//==============================================================================

void CommMPSetLifeTime(BOOL bMinimum)
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

int CommMPGetServiceNumber(int serviceNo)
{
    int i,num=0;
    
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; i++) {
        _GF_BSS_DATA_INFO* pGF = CommMPGetGFBss(i);
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

BOOL CommMPIsParentBeaconSent(void)
{
    return WHIsParentBeaconSent();
}

//------------------------------------------------------
/**
 * @brief   �X�L������Ԃ��ǂ���
 * @retval  ���M����=TRUE
 */
//------------------------------------------------------

BOOL CommMPIsChildStateScan(void)
{
    return WH_IsSysStateScan();
}

//------------------------------------------------------
/**
 * @brief   �`�����l����Ԃ�
 * @retval  �ڑ��`�����l��
 */
//------------------------------------------------------

int CommMPGetChannel(void)
{
    return _pCommMP->channel;
}

//==============================================================================
/**
 * @brief	���@�r�[�R�����M���e�̎擾
 * @param none
 * @return �r�[�R���̒��g
 */
//==============================================================================
void* CommMPGetMyGFBss(void)
{
	if (_pCommMP == NULL) {
		return NULL;
	}
	return _pCommMP->gameInfoBuff;
}

//------------------------------------------------------
/**
 * @brief	�r�[�R�����E���Ă���̃J�E���g�_�E���^�C�}�擾
 * @param	index	�ʒu
 */
//------------------------------------------------------
int CommMPGetBConUncacheTime(int index)
{
	if (_pCommMP == NULL) {
		return 0;
	}
	return _pCommMP->bconUnCatchTime[index];
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
