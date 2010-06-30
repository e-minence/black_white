//=============================================================================
/**
 * @file	net_whpipe.c
 * @brief	�ʐM�V�X�e��
 * @author	GAME FREAK Inc.
 * @date    2006.12.05
 */
//=============================================================================

#include "gflib.h"
#include "net/network_define.h"

#include <nitro.h>
#include <nitro/wm.h>
#include <nitro/cht.h>

#include "net/net_whpipe.h"
#include "system/gfl_use.h"

//�ڑ����萧���R�[�h��`
#include "net_alone_test.h"

#ifdef PM_DEBUG

#define DEBUG_WH_BEACON_PRINT_ON (0)  // beacon�؂�ւ��^�C�~���O��\��

#if DEBUG_ONLY_FOR_ohno
#define DEBUG_WH_BEACON_PRINT_ON  (0) // beacon�؂�ւ��^�C�~���O��\��
#endif  //DEBUG_ONLY_FOR_ohno

extern u8 DebugAloneTest;

#endif  //PM_DEBUG

#ifdef  PLAYABLE_VERSION
#define POKEMONWB_BEACON_PRODUCT_NO (100)   //���̔ԍ��̃r�[�R���͐��i��
#else
#define POKEMONWB_BEACON_PRODUCT_NO (0)   //���̔ԍ��̃r�[�R���͐��i��
#endif

/**
 *  @brief _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
 */
#define _BEACON_SIZE_FIX (8)
#define _BEACON_FIXHEAD_SIZE (2)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)


#define _BEACON_DOWNCOUNT  (20)   //����Ⴂ���ꎞ��~����t���[��

/**
 *  @brief �r�[�R���\����
 */
// WM_SIZE_USER_GAMEINFO  �ő� 112byte
// _BEACON_SIZE_FIX�ɂ� �Œ�łق����r�[�R���p�����[�^�̍��v����ŏ���
typedef struct{
	u16        CRC;         ///< CRC
	u16        GGID;               ///< �Q�[���ŗL��ID  ��v���K�{
	GameServiceID  		serviceNo; ///< �ʐM�T�[�r�X�ԍ�
	u8        ProductOrDevelopment;      ///< �J���Ɩ{�ԂƂ��Ȃ���Ȃ��悤�ɂ��邽��
	u8  	    connectNum;    	   ///< �Ȃ����Ă���䐔  --> �{�e���ǂ�������
	u8        pause;               ///< �ڑ����֎~���������Ɏg�p����
	u8        aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
} _GF_BSS_DATA_INFO;


//-------------------------------------------------------------
// ��`
//-------------------------------------------------------------

//#define _PORT_DATA_RETRANSMISSION   (14)    // �ؒf����܂Ŗ����đ����s��  ��������g�p���Ă���
#define _NOT_INIT_BITMAP  (0xffff)   // �ڑ��l�����Œ�Ɏw��Ȃ��ꍇ�̒l

typedef enum{    // �ؒf���
	_DISCONNECT_NONE,
	_DISCONNECT_END,
	_DISCONNECT_SECRET,
} _DisconnectState_e;


typedef struct _NET_WL_WORK  GFL_NETWL;

/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*PTRStateFunc)(GFL_NETWL* pState);

/// ���Ԏ擾
typedef int (*PTRTimeGet)(void);


//�Ǘ��\���̒�`
struct _NET_WL_WORK {
	PTRStateFunc state;      ///< �n���h���̃v���O�������
	PTRCommRecvLocalFunc recvCallback; ///< ��M�R�[���o�b�N�����p
	WMBssDesc sBssDesc[SCAN_PARENT_COUNT_MAX];  ///< �e�@�̏����L�����Ă���\����
	u8  backupBssid[GFL_NET_MACHINE_MAX][WM_SIZE_BSSID];   // ���܂Őڑ����Ă���
	u16 bconUnCatchTime[SCAN_PARENT_COUNT_MAX]; ///< �e�@�̃r�[�R�����E��Ȃ���������+�f�[�^�����邩�ǂ���
	PTRPARENTFIND_CALLBACK pCallback;
 // PTRTimeGet parentTime;
//  PTRTimeGet childTime;
	void* pUserWork;
	u16 _sTgid;
	u16 errCheckBitmap;      ///< ����BITMAP���H���Ⴄ�ƃG���[�ɂȂ�
  u16 crossTimer;
	u8 gameInfoBuff[WM_SIZE_USER_GAMEINFO];  //���M����r�[�R���f�[�^
	u8 connectMac[WM_SIZE_BSSID];
	u8 mineDebugNo;       ///< �ʐM���ʎq�A�f�o�b�O�p �{�Ԃł͂O
	u8 channel;
	u8 keepChannel;     ///<�L�^���Ă����`�����l���̏ꏊ
	u8 disconnectType;    ///< �ؒf��
	u8 bSetReceiver;
	u8 bEndScan;  // endscan
	u8 bPauseConnect;   ///< �q�@�̎�t���~���
	u8 bErrorState;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
	u8 bErrorCheck;   ///< �q�@�������ꍇ+�N�����������ꍇ�G���[�������邩�ǂ���
	u8 bTGIDChange;
	u8 bEntry;        ///< �q�@�̐V�K�Q��
	u8 bScanCallBack;  ///< �e�̃X�L���������������ꍇTRUE, ������FALSE
	u8 bChange;   ///<�r�[�R���ύX�t���O
  u8 crossChannel;
  u8 CrossRand;
  u8 PauseScan;
  u8 crossTimerPause;
  u8 ErrorEndTimer;
  u8 dummy2;
  u8 dummy3;
  //u8 = 16byte
} ;

//#define _NET_WL_WORK  GFL_NETWL;


//-------------------------------------------------------------
// ���[�N
//-------------------------------------------------------------

///< ���[�N�\���̂̃|�C���^
static GFL_NETWL* _pNetWL = NULL;

// �e�@�ɂȂ�ꍇ��TGID �\���̂ɓ���Ă��Ȃ��̂�
// �ʐM���C�u�����[��؂����Ƃ��Ă��C���N�������g����������
/// TGID�Ǘ�


// �R���|�[�l���g�]���I���̊m�F�p
// �C�N�j���[�����R���|�[�l���g�������ړ�������Ƃ��͂�����ړ�
//static volatile int   startCheck;


//-------------------------------------------------------------
// static�錾
//-------------------------------------------------------------

//static void _whInitialize(HEAPID heapID,GFL_NETWL* pNetWL);
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
static void _setBeacon(void* buff, int size);


static void _changeState(GFL_NETWL* pState, PTRStateFunc state);  // �X�e�[�g��ύX����

#if PM_DEBUG
static void _changeStateDebug(GFL_NETWL* pState, PTRStateFunc state, int line);  // �X�e�[�g��ύX����
#define   _CHANGE_STATE(state)  _changeStateDebug(_pNetWL ,state, __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(_pNetWL ,state)
#endif //GFL_NET_DEBUG


//-------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//-------------------------------------------------------------

static void _changeState(GFL_NETWL* pState,PTRStateFunc state)
{
	pState->state = state;
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//-------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(GFL_NETWL* pState,PTRStateFunc state, int line)
{
#if DEBUG_WH_BEACON_PRINT_ON
  NET_PRINT("pipe: %d\n",line);
#endif
	_changeState(pState, state);
}
#endif


//-------------------------------------------------------------
/**
 * @brief   �ڑ��N���X�̏�����
 * @param   heapID    ���[�N�m��ID
 * @param   getFunc   �r�[�R���쐬�֐�
 * @param   getSize   �r�[�R���T�C�Y�֐�
 * @param   getComp   �r�[�R����r�֐�
 * @param   bConnect  �ʐM����ꍇTRUE
 * @retval  none
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLInitialize(HEAPID heapID,NetDevEndCallback callback, void* pUserWork, const BOOL isScanOnly)
{
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
	int i;
	GFL_NETWL* pNetWL = NULL;

	pInit = GFL_NET_GetNETInitStruct();
	if(_pNetWL){
		return FALSE;
	}
	pNetWL = GFL_HEAP_AllocClearMemory(heapID, sizeof(GFL_NETWL));
	_pNetWL = pNetWL;

	pNetWL->keepChannel = 0xff;

	if(FALSE == WH_Initialize( heapID, callback, isScanOnly)){
		return FALSE;
	}
	// WH �����ݒ�
	WH_SetGgid(pInit->ggid);
	pNetWL->pUserWork = pUserWork;
	pNetWL->disconnectType = _DISCONNECT_NONE;
#ifdef PM_DEBUG
	pNetWL->mineDebugNo = DebugAloneTest;
#else
	pNetWL->mineDebugNo = POKEMONWB_BEACON_PRODUCT_NO;
#endif
	pNetWL->_sTgid = WM_GetNextTgid();

#if DEBUG_WH_BEACON_PRINT_ON
	NET_PRINT("%d %d\n",WM_SIZE_USER_GAMEINFO , sizeof(_GF_BSS_DATA_INFO) );
#endif
	GF_ASSERT(WM_SIZE_USER_GAMEINFO == sizeof(_GF_BSS_DATA_INFO));
	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   ��M�R�[���o�b�N�֐����w��
 * @param   recvCallback   ��M�R�[���o�b�N�֐�
 * @return  none
 */
//-------------------------------------------------------------

void GFL_NET_WLSetRecvCallback( PTRCommRecvLocalFunc recvCallback)
{
	GFL_NETWL* pNetWL = _pNetWL;
	pNetWL->recvCallback = recvCallback;
}

//-------------------------------------------------------------
/**
 * @brief   �ڑ����Ă��邩�ǂ���
 * @retval  TRUE  �ڑ����Ă���
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsConnect(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL==NULL){
		return FALSE;
	}
	return TRUE;
}


//-------------------------------------------------------------
/**
 * @brief   �}�b�N�A�h���X�\��
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//-------------------------------------------------------------

static void DEBUG_MACDISP(char* msg,WMBssDesc *bssdesc)
{
	NET_PRINT("%s %02x%02x%02x%02x%02x%02x\n",msg,
						bssdesc->bssid[0],bssdesc->bssid[1],bssdesc->bssid[2],
						bssdesc->bssid[3],bssdesc->bssid[4],bssdesc->bssid[5]);
}

//-------------------------------------------------------------
/**
 * @brief   �ڑ����ėǂ����ǂ������f����
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//-------------------------------------------------------------

static BOOL _scanCheck(WMBssDesc *bssdesc)
{
	int i;
	_GF_BSS_DATA_INFO* pGF;
	GFL_NETWL* pNetWL = _pNetWL;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
	int serviceNo = pInit->gsid;
	u16 ggid = pInit->ggid;
	u8 sBuff[_BEACON_FIXHEAD_SIZE];

	// catch�����e�f�[�^
	pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
	if(pGF->pause){
		return FALSE;  // �|�[�Y���̐e�@��BEACON����
	}
//	NET_PRINT("debugNo %d %d\n",pGF->debugAloneTest , _DEBUG_ALONETEST);

#ifdef PM_DEBUG  // �f�o�b�O�̎������A��ɒ�`������l�͊�{���̐l�ƂȂ���Ȃ�
	if(pGF->ProductOrDevelopment != pNetWL->mineDebugNo){
		return FALSE;   //�p���X�̈�
	}
#else
  if(pGF->ProductOrDevelopment != POKEMONWB_BEACON_PRODUCT_NO){
		return FALSE;   //�p���X�̈�
  }
#endif
//	GFLR_NET_GetBeaconHeader(sBuff,_BEACON_FIXHEAD_SIZE);
//	if(0 != GFL_STD_MemComp(sBuff, pGF->FixHead , _BEACON_FIXHEAD_SIZE)){
  {
    u16* pData = (u16*)pGF;
    if(pGF->CRC != GFL_STD_CrcCalc(&pData[1], sizeof(_GF_BSS_DATA_INFO) - 2) ){
      NET_PRINT("CRC�s��v\n");
      return FALSE;
    }
  }
	if(pGF->GGID != ggid){
#if DEBUG_WH_BEACON_PRINT_ON
		NET_PRINT("beacon�s��v\n");
#endif
    return FALSE;
	}
	if(pInit->beaconCompFunc){
		if(FALSE == pInit->beaconCompFunc(serviceNo, pGF->serviceNo,pNetWL->pUserWork)){
			return FALSE;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
		}
	}
	else{
#if DEBUG_WH_BEACON_PRINT_ON
		NET_PRINT("ServiceID CMP %d %d",serviceNo, pGF->serviceNo);
#endif
		if(serviceNo != pGF->serviceNo){
			return FALSE;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
		}
	}
  if(serviceNo != WB_NET_UNION){  //���j�I�����[���ȊO��
    if((pGF->connectNum >= pInit->maxConnectNum) && (pInit->maxConnectNum>1)){
      return FALSE;   // �ڑ��l�������ς��̏ꍇ�E��Ȃ�
    }
  }
//	NET_PRINT("_scanCheckok\n");
	return TRUE;
}



//-------------------------------------------------------------
/**
 * @brief   �q�@���e�@��T���o�������ɌĂ΂��R�[���o�b�N�֐�
 * �e�@���E�����тɌĂ΂��
 * @param   bssdesc   �O���[�v���
 * @retval  none
 */
//-------------------------------------------------------------

static BOOL _scanCallback(WMBssDesc *bssdesc)
{
	int i;
	_GF_BSS_DATA_INFO* pGF;
	GFL_NETWL* pNetWL = _pNetWL;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();

	int serviceNo = pInit->gsid;

//	NET_PRINT("_scanCallback\n");

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
#if DEBUG_WH_BEACON_PRINT_ON
			NET_PRINT("������x�E�����ꍇ�Ƀ^�C�}�[���Z\n");
#endif
			pNetWL->bconUnCatchTime[i] = DEFAULT_TIMEOUT_FRAME;
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
	pNetWL->bconUnCatchTime[i] = DEFAULT_TIMEOUT_FRAME;
	MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i],sizeof(WMBssDesc));
	pNetWL->bScanCallBack = TRUE;
//	NET_PRINT("_scanCallback�ǉ�\n");

	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   �q�@���e�@���������r�[�R���ɑ΂��Ă�����`�F�b�N
 * @param   bssdesc   �O���[�v���
 * @retval  TRUE:���Ȃ�(�g�pOK)
 * @retval  FALSE:��肠��(�g�pNG)
 */
//-------------------------------------------------------------
BOOL GFL_NET_WL_scanCheck(WMBssDesc *bssdesc, u32 check_bit)
{
	int i;
	_GF_BSS_DATA_INFO* pGF;
	GFL_NETWL* pNetWL = _pNetWL;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
	int serviceNo = pInit->gsid;
	u16 ggid = pInit->ggid;
	u8 sBuff[_BEACON_FIXHEAD_SIZE];

	// catch�����e�f�[�^
	pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
	if(check_bit & WL_SCAN_CHK_BIT_PAUSE){
  	if(pGF->pause){
  		return FALSE;  // �|�[�Y���̐e�@��BEACON����
  	}
  }
//	NET_PRINT("debugNo %d %d\n",pGF->debugAloneTest , _DEBUG_ALONETEST);

  if(check_bit & WL_SCAN_CHK_BIT_DEVELOPMENT){
  #ifdef PM_DEBUG  // �f�o�b�O�̎������A��ɒ�`������l�͊�{���̐l�ƂȂ���Ȃ�
  	if(pGF->ProductOrDevelopment != pNetWL->mineDebugNo){
  		return FALSE;   //�p���X�̈�
  	}
  #else
    if(pGF->ProductOrDevelopment != POKEMONWB_BEACON_PRODUCT_NO){
  		return FALSE;   //�p���X�̈�
    }
  #endif
  }
//	GFLR_NET_GetBeaconHeader(sBuff,_BEACON_FIXHEAD_SIZE);
//	if(0 != GFL_STD_MemComp(sBuff, pGF->FixHead , _BEACON_FIXHEAD_SIZE)){
  
  if(check_bit & WL_SCAN_CHK_BIT_CRC)
  {
    u16* pData = (u16*)pGF;
    if(pGF->CRC != GFL_STD_CrcCalc(&pData[1], sizeof(_GF_BSS_DATA_INFO) - 2) ){
      NET_PRINT("CRC�s��v\n");
      return FALSE;
    }
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_GGID){
  	if(pGF->GGID != ggid){
  #if DEBUG_WH_BEACON_PRINT_ON
  		NET_PRINT("beacon�s��v\n");
  #endif
      return FALSE;
  	}
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_BEACON_COMP){
  	if(pInit->beaconCompFunc){
  		if(FALSE == pInit->beaconCompFunc(serviceNo, pGF->serviceNo, pNetWL->pUserWork)){
  			return FALSE;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
  		}
  	}
  	else{
  #if DEBUG_WH_BEACON_PRINT_ON
  		NET_PRINT("ServiceID CMP %d %d",serviceNo, pGF->serviceNo);
  #endif
  		if(serviceNo != pGF->serviceNo){
  			return FALSE;   // �T�[�r�X���قȂ�ꍇ�͏E��Ȃ�
  		}
  	}
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_MAX_CONNECT){
  	if(pGF->connectNum >= pInit->maxConnectNum){
  		return FALSE;   // �ڑ��l�������ς��̏ꍇ�E��Ȃ�
  	}
  }
//	NET_PRINT("_scanCheckok\n");
	return TRUE;
}


//-------------------------------------------------------------
/**
 * @brief   �q�@�̎g�p���Ă���f�[�^�̏�����
 * @retval  none
 */
//-------------------------------------------------------------

void GFL_NET_WLChildBconDataInit(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	int i;

	for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
		pNetWL->bconUnCatchTime[i] = 0;
	}
	MI_CpuClear8(pNetWL->sBssDesc,sizeof(WMBssDesc)*SCAN_PARENT_COUNT_MAX);
}

//-------------------------------------------------------------
/**
 * @brief   �e�@�̎g�p���Ă���f�[�^�̏�����
 * @param   bTGIDChange  �V�K�̃Q�[���̏������̏ꍇTRUE �Â��r�[�R���ł̌듮���h�����ߗp
 * @retval  none
 */
//-------------------------------------------------------------

static void _parentDataInit(BOOL bTGIDChange)
{
	GFL_NETWL* pNetWL = _pNetWL;
	pNetWL->bTGIDChange = bTGIDChange;
}

//-------------------------------------------------------------
/**
 * @brief   �e�q���ʁA�ʐM�̏��������܂Ƃ߂�
 * @retval  �������ɐ���������TRUE
 */
//-------------------------------------------------------------

static void _commInit(GFL_NETWL* pNetWL)
{
	pNetWL->bPauseConnect = FALSE;
	pNetWL->bScanCallBack = FALSE;
	pNetWL->bErrorState = FALSE;
	pNetWL->bErrorCheck = FALSE;
	//  pNetWL->bAutoExit = FALSE;
	pNetWL->bEndScan = 0;
	pNetWL->bSetReceiver = FALSE;
	return;
}


//-------------------------------------------------------------
/**
 * @brief   ��M�R�[���o�b�N
 * @param   aid    �ʐM��localID
 * @param   *data  ��M�f�[�^
 * @param   size   ��M�T�C�Y
 * @retval  none
 */
//-------------------------------------------------------------

static void _receiverFunc(u16 aid, u16 *data, u16 size)
{
	GFL_NETWL* pNetWL = _pNetWL;
	pNetWL->recvCallback(aid,data,size);
}


//-------------------------------------------------------------
/**
 * @brief   �q�@�̐ڑ��J�n���s��
 * @param   work_area �@�V�X�e���Ŏg���������[�̈�
 *                      NULL�̏ꍇ�͂��łɏ������ς݂Ƃ��Ĉ���
 * @param   serviceNo  �Q�[���̎��
 * @param   regulationNo  �Q�[���̎��
 * @param   bBconInit  �r�[�R���f�[�^������������̂��ǂ���
 * @retval  �������ɐ���������TRUE
 */
//-------------------------------------------------------------
BOOL GFL_NET_WLChildInit(BOOL bBconInit)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(pNetWL == NULL){
		return FALSE;
	}

	_commInit(pNetWL);

	if(bBconInit){
#if DEBUG_WH_BEACON_PRINT_ON
		NET_PRINT("�r�[�R���̏�����\n");
#endif
    GFL_NET_WLChildBconDataInit(); // �f�[�^�̏�����
	}
	//if(!pNetWL->bSetReceiver )
	{
		WH_SetReceiver(_receiverFunc);
		pNetWL->bSetReceiver = TRUE;
	}
	// �e�@�����X�^�[�g
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
		if(WH_StartScan(_scanCallback, NULL, 0)){
			return TRUE;
		}
	}
#if DEBUG_WH_BEACON_PRINT_ON
	NET_PRINT("IDLE��Ԃł͂Ȃ����ߏ������Ɏ��s���܂���\n");
#endif
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM�؂�ւ����s���i�e�q���]�ɕK�v�ȏ����j
 * @param   none
 * @retval  ���Z�b�g������TRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLSwitchParentChild(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(!pNetWL){
		NET_PRINT("pNetWLNONE\n");
		return TRUE;
	}
	NET_PRINT("scan %d\n",pNetWL->bEndScan);
	switch(pNetWL->bEndScan){
	case 0:
		if(WH_SYSSTATE_SCANNING == WH_GetSystemState()){
			NET_PRINT("�X�L����������------WH_EndScan\n");
			WH_EndScan();
			pNetWL->bEndScan = 1;
			break;
		}
		else if(WH_SYSSTATE_BUSY == WH_GetSystemState()){  //���΂炭�҂�
		}
		else if(WH_Finalize()){
			pNetWL->bEndScan = 2;
		}
		break;
	case 1:
		if(WH_SYSSTATE_BUSY != WH_GetSystemState()){
			NET_PRINT("�I������----2\n");
      if(WH_Finalize()){
        pNetWL->bEndScan = 2;
      }
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




//-------------------------------------------------------------
/**
 * @brief   �I���J��
 * @param
 * @retval
 */
//-------------------------------------------------------------

static void _whEndErrResetFunc(GFL_NETWL* pNetWL);

static void _whEndErrResetFunc2(GFL_NETWL* pNetWL)
{
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
    if(WH_Finalize()){
      pNetWL->ErrorEndTimer = 0xff;
      _CHANGE_STATE(_whEndErrResetFunc);
    }
	}
	if(WH_GetSystemState() == WH_SYSSTATE_ERROR){
      pNetWL->ErrorEndTimer = 0xff;
    _CHANGE_STATE(_whEndErrResetFunc);
  }
}



//-------------------------------------------------------------
/**
 * @brief   �I���J��
 * @param
 * @retval
 */
//-------------------------------------------------------------

static void _whEndErrResetFunc(GFL_NETWL* pNetWL)
{
  pNetWL->ErrorEndTimer--;  //�ʐM�V�X�e����BUSY�̂܂܏����ύX������Ȃ��ꍇ������̂� �^�C�}�[�Ď���WH_Reset���ĂԂ悤��
	if(WH_GetSystemState() == WH_SYSSTATE_STOP || WH_GetSystemState() == WH_SYSSTATE_IDLE){
    _CHANGE_STATE(NULL);
  }
	else if(WH_GetSystemState() == WH_SYSSTATE_ERROR){
		WH_Reset();
    _CHANGE_STATE(_whEndErrResetFunc2);
	}
  else if(pNetWL->ErrorEndTimer==0){
    WH_Reset();
    _CHANGE_STATE(_whEndErrResetFunc2);
  }
}



//-------------------------------------------------------------
/**
 * @brief   �ʐM�ؒf���s��  �����ł͂����܂ŒʐM�I���葱���ɓ��邾��
 *  �z���g�ɏ����͉̂���_commEnd
 * @param   none
 * @retval  �I�������Ɉڂ����ꍇTRUE
 */
//-------------------------------------------------------------
BOOL GFL_NET_WLFinalize(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(!pNetWL){
    return TRUE;
  }
    
  if(pNetWL->disconnectType == _DISCONNECT_NONE){
    if(WH_Finalize()){
      pNetWL->ErrorEndTimer = 0xff;
      _CHANGE_STATE(_whEndErrResetFunc);
      return TRUE;
    }
  }
  return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM�̑S�Ă�����
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------
static void _commEnd(void)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(pNetWL==NULL){
		return;
	}
	//   WH_DestroyHandle(pNetWL->_pWHWork);
	GFL_HEAP_FreeMemory(pNetWL);
	_pNetWL = NULL;
}
#if 0
void GFL_NET_WHPipeFree(void)
{
	_commEnd();
}
#endif
//-------------------------------------------------------------
/**
 * @brief   �T�����Ƃ��ł����e�̐���Ԃ�
 * @param   none
 * @retval  �e�@�̐�
 */
//-------------------------------------------------------------

int GFL_NET_WLGetParentCount(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
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
	GFL_NETWL* pNetWL = _pNetWL;
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

//-------------------------------------------------------------
/**
 * @brief   �e�@���X�g�ɕω����������ꍇTRUE
 * @param   none
 * @retval  �e�@���X�g�ɕω����������ꍇTRUE �Ȃ����FALSE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsScanListChange(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	return pNetWL->bScanCallBack;
}

//-------------------------------------------------------------
/**
 * @brief   �e�@�̕ω���m�点��t���O�����Z�b�g����
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------

void GFL_NET_WLResetScanChangeFlag(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	pNetWL->bScanCallBack = FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   ���̐e�@�������ƃR�l�N�V�����������Ă���̂��𓾂�
 * @param   index   �e�̃��X�g��index
 * @retval  �R�l�N�V������ 0-16
 */
//-------------------------------------------------------------

int GFL_NET_WLGetParentConnectionNum(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
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

//-------------------------------------------------------------
/**
 * @brief   �ڑ��l���ɊY������e��Ԃ�
 * @param   none
 * @retval  �e��index
 */
//-------------------------------------------------------------

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

static BOOL _parentFindCallback(WMBssDesc* pBeacon)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(_scanCheck(pBeacon)==FALSE){
		return FALSE;
	}
	if(pNetWL->pCallback){
		pNetWL->pCallback();
	}
	return TRUE;
}


//-------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�̒���m�F
 *  �ڑ�����������Ԃł̊ԁA���̊֐����ĂсA�^�C���A�E�g�������s��
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------

static void _WLParentBconCheck(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	int id;

	for(id = 0; id < SCAN_PARENT_COUNT_MAX; id++){
		if(pNetWL->bconUnCatchTime[id] == 0 ){
			continue;
		}
		if(pNetWL->bconUnCatchTime[id] > 0){
			pNetWL->bconUnCatchTime[id]--;
			if(pNetWL->bconUnCatchTime[id] == 0){
#if DEBUG_WH_BEACON_PRINT_ON
				NET_PRINT("�e�@�����Ȃ� %d\n", id);
#endif
				pNetWL->bScanCallBack = TRUE;   // �f�[�^��ύX�����̂�TRUE
			}
		}
	}
}

//-------------------------------------------------------------
/**
 *  @brief   ���[�U��`�̐e�@����ݒ肵�܂��B
 *  _GF_BSS_DATA_INFO�\���̂̒��g�𑗂�܂�
 *  @param   userGameInfo  ���[�U��`�̐e�@���ւ̃|�C���^
 *  @param   length        ���[�U��`�̐e�@���̃T�C�Y
 *  @retval  none
 */
//-------------------------------------------------------------
void NET_WHPIPE_BeaconSetInfo( void )
{
	u8 macBuff[6];
	u8 sBuff[_BEACON_FIXHEAD_SIZE];
	_GF_BSS_DATA_INFO* pGF;
	int size;

  if(!GFL_NET_IsInit()){
    return;
  }
  {
    //    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFL_NETWL* pNetWL = _pNetWL;
    GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();

    if(pInit->beaconGetSizeFunc==NULL){
#if DEBUG_WH_BEACON_PRINT_ON
      NET_PRINT("beaconGetSizeFunc none");
#endif
      return;
    }
    {
      NetBeaconGetSizeFunc func = pInit->beaconGetSizeFunc;
      size = func(pNetWL->pUserWork);
    }
    if((WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX) <= size){
      GF_PANIC("size over %d <= %d", (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX), size);
      return;
    }
    pGF = (_GF_BSS_DATA_INFO*)pNetWL->gameInfoBuff;
    pGF->serviceNo = pInit->gsid;    // �Q�[���̔ԍ�
    pGF->GGID = pInit->ggid;
#ifdef PM_DEBUG
    pGF->ProductOrDevelopment = pNetWL->mineDebugNo;
#else
    pGF->ProductOrDevelopment = POKEMONWB_BEACON_PRODUCT_NO;
#endif
//    GFLR_NET_GetBeaconHeader(sBuff,_BEACON_FIXHEAD_SIZE);
//    GFL_STD_MemCopy( sBuff, pGF->FixHead, _BEACON_FIXHEAD_SIZE);

    pGF->pause = pNetWL->bPauseConnect;
    GFL_STD_MemCopy( pInit->beaconGetFunc(pNetWL->pUserWork), pGF->aBeaconDataBuff, size);

    _setBeacon(pNetWL->gameInfoBuff, sizeof(_GF_BSS_DATA_INFO));

	//    DC_FlushRange(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
	//  WH_SetUserGameInfo((u16*)pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
  }
}


//-------------------------------------------------------------
/**
 *  @brief   ���[�U��`�̐e�@���� �l���������Ď�����ɍX�V����
 *  _GF_BSS_DATA_INFO�\���̂̒��g�𑗂�܂�
 *  @param   none
 *  @retval  none
 */
//-------------------------------------------------------------
static void _funcBconDataChange( void )
{
	//GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
	GFL_NETWL* pNetWL = _pNetWL;
	_GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->gameInfoBuff;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
	int size;

	if(pInit->beaconGetSizeFunc!=NULL){
		NetBeaconGetSizeFunc func = pInit->beaconGetSizeFunc;
		size = func(pNetWL->pUserWork);
    GF_ASSERT(size <= _BEACON_USER_SIZE_MAX);
    GFL_STD_MemCopy( pInit->beaconGetFunc(pNetWL->pUserWork), pGF->aBeaconDataBuff, size);
	}

	if(_connectNum() != pGF->connectNum ){
    pNetWL->bChange = 0;
		pGF->connectNum = _connectNum();
  }
  if(pNetWL->bChange == 0){
		_setBeacon(pNetWL->gameInfoBuff, sizeof(_GF_BSS_DATA_INFO));
    pNetWL->bChange = 2;
	}
  pNetWL->bChange--;
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM���C�u�����[�����̏�Ԃ����āA����������֐�
 * VBlank�Ƃ͊֌W�Ȃ��̂�process�̎��ɏ�������΂���
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------
static void _stateProcess(u16 bitmap)
{
	int state = WH_GetSystemState();
	GFL_NETWL* pNetWL = _pNetWL;


  if(!GFL_NET_WLIsError()){
    _funcBconDataChange();      // �r�[�R���̒��g������������
  }
	if(pNetWL->errCheckBitmap == _NOT_INIT_BITMAP){
		pNetWL->errCheckBitmap = bitmap;  // ���̂Ƃ��̐ڑ��l����ێ�
	}
	if(pNetWL->bErrorCheck){
		if((WH_GetCurrentAid() == GFL_NET_NO_PARENTMACHINE) && (!GFL_NET_WLIsChildsConnecting())){
			pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
		}
		if(pNetWL->errCheckBitmap > bitmap){  // �ؒf�����ꍇ�K������������ �����镪�ɂ�OK
			pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
		}
	}
	if(WH_ERRCODE_FATAL == WH_GetLastError()){
		GFL_NET_FatalErrorFunc(0);
	}
	switch (state) {
	case WH_SYSSTATE_STOP:
		if(pNetWL->disconnectType == _DISCONNECT_END){
#if DEBUG_WH_BEACON_PRINT_ON
			NET_PRINT("WHEnd ���Ă�ŏI�����܂���\n");
#endif
      _commEnd();  // ���[�N���牽����S�ĊJ��
			return;
		}
		break;
	case WH_SYSSTATE_IDLE:
		if(pNetWL->disconnectType == _DISCONNECT_END){
			if(WH_End(NULL)){
				return;
			}
		}
		if(pNetWL->disconnectType == _DISCONNECT_SECRET){
			if(WH_End(NULL)){
				return;
			}
		}
		break;
	case WH_SYSSTATE_CONNECT_FAIL:
	case WH_SYSSTATE_ERROR:
		NET_PRINT("�G���[�� %d \n",WH_GetLastError());
		if(pNetWL){
			if(!pNetWL->bErrorState){
				GFL_NET_ErrorFunc(WH_GetLastError());
			}
			pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
		}
		break;
	default:
		break;
	}
//  HW_Main();
  
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM���C�u�����[�����̏�Ԃ����āA����������֐�
 * VBlank�Ƃ͊֌W�Ȃ��̂�process�̎��ɏ�������΂���
 * �q�@�͂��݂��̐ڑ����킩��Ȃ��̂ŁA�ʐM���ʂ�commsystem���������ăG���[��������
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------

void GFI_NET_MLProcess(u16 bitmap)
{
	if(_pNetWL){
		PTRStateFunc state = _pNetWL->state;
		if(state != NULL){
			state(_pNetWL);
		}
	}
	if(_pNetWL){
		_stateProcess(bitmap);
	}
	if(_pNetWL && GFL_NET_WLIsChildStateScan() ){
		_WLParentBconCheck();
	}
  if(_pNetWL){
//    OS_TPrintf("%d --\n",_pNetWL->crossTimer);
    
    if(_pNetWL->crossTimer){
      WIH_SetBeaconPause(TRUE);
      _pNetWL->PauseScan=TRUE;
      if(!_pNetWL->crossTimerPause){
        _pNetWL->crossTimer--;
      }
    }
    else{
      WIH_SetBeaconPause(FALSE);
      _pNetWL->PauseScan=FALSE;
    }
   
  }
  
  WH_StepScan();
}

//-------------------------------------------------------------
/**
 * @brief   �f�o�C�X���ʐM�\��ԂȂ̂��ǂ�����Ԃ�
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//-------------------------------------------------------------
BOOL GFL_NET_WL_IsConnectLowDevice(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(!pNetWL){
		return FALSE;
	}

	if((WH_GetCurrentAid() == GFL_NET_NO_PARENTMACHINE) && (!GFL_NET_WLIsChildsConnecting())){
		return FALSE;
	}

	if ((WH_GetSystemState() == WH_SYSSTATE_CONNECTED) || (WH_GetSystemState() == WH_SYSSTATE_DATASHARING)) {
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   �ʐM�\��ԂȂ̂��ǂ�����Ԃ�
 * @param   �e�q�@��netID
 * @retval  TRUE  �ʐM�\    FALSE �ʐM�ؒf
 */
//-------------------------------------------------------------
static BOOL _isConnect(u16 netID)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(!pNetWL){
		return FALSE;
	}
	if ((WH_GetSystemState() == WH_SYSSTATE_CONNECTED) || (WH_GetSystemState() == WH_SYSSTATE_DATASHARING)) {
  }
  else{
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

//-------------------------------------------------------------
/**
 * @brief   �ʐM�\��Ԃ̐l����Ԃ�
 * @param   none
 * @retval  �ڑ��l��
 */
//-------------------------------------------------------------
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

//-------------------------------------------------------------
/**
 * @brief  ���������Ă��邩�ǂ�����Ԃ�
 * @param   none
 * @retval  �������I����Ă�����TRUE
 */
//-------------------------------------------------------------
BOOL GFL_NET_WLIsInitialize(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	return (pNetWL!=NULL);
}

//-------------------------------------------------------------
/**
 * @brief  WH���C�u�����Ł@��Ԃ�IDLE�ɂȂ��Ă��邩�m�F����
 * @param   none
 * @retval  IDLE�ɂȂ��Ă���=TRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsStateIdle(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		return WH_GetSystemState() == WH_SYSSTATE_IDLE;
	}
	return TRUE;
}


//-------------------------------------------------------------
/**
 * @brief  WH���C�u�����Ł@�ʐM��Ԃ�BIT���m�F  �q�@���Ȃ����Ă��邩�ǂ���
 * @param   none
 * @retval  �Ȃ�������TRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsChildsConnecting(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		return( WH_GetBitmap() & 0xfffe);
	}
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief  �e�@�����������ǂ���
 * @param   none
 * @retval  �������ꍇTRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLParentDisconnect(void)
{
	if(GFL_NET_WLIsError() && ( WH_ERRCODE_DISCONNECTED == WH_GetLastError())){
		return TRUE;
	}
	return FALSE;
}


//-------------------------------------------------------------
/**
 * @brief  �G���[��Ԃ��ǂ���
 * @param   none
 * @retval  �G���[�̎�TRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsError(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		if(pNetWL->bErrorState){
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   �N�����������̂��G���[�����ɂ��邩�ǂ�����SET
 * @param   bOn  �L������TRUE
 * @retval  none
 */
//-------------------------------------------------------------

void GFL_NET_WLSetDisconnectOtherError(BOOL bOn)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		pNetWL->bErrorCheck = bOn;
		pNetWL->errCheckBitmap = _NOT_INIT_BITMAP;
	}
}

//-------------------------------------------------------------
/**
 * @brief   �T�[�r�X�ԍ��ɑΉ������r�[�R���Ԋu�𓾂܂�
 *          �T�[�r�X�ԍ��� include/communication/comm_def.h�ɂ���܂�
 * @param   serviceNo �T�[�r�X�ԍ�
 * @retval  beacon�Ԋu msec
 */
//-------------------------------------------------------------

u16 _getServiceBeaconPeriod(u16 serviceNo)
{
	u16 beaconPeriod = WM_GetDispersionBeaconPeriod();
	/*  �K�v�Ȃ�R�[���o�b�N�֐��Ƃ��Ē�`����
    ��{�I�ɂ͕K�v�Ƃ��Ȃ�������  k.ohno 06.12.05 */
	return beaconPeriod;
}

//-------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   WMBssDesc*  �r�[�R���o�b�t�@�|�C���^
 */
//-------------------------------------------------------------

WMBssDesc* GFL_NET_WLGetWMBssDesc(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		return &pNetWL->sBssDesc[index];
	}
	return NULL;
}

//-------------------------------------------------------------
/**
 * @brief    �r�[�R�����x�𓾂�
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   u16  �ʐM���x
 */
//-------------------------------------------------------------

u16 GFL_NET_WL_GetRssi(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		u16 rate_rssi,rssi;
		return pNetWL->sBssDesc[index].rssi;
	}
	return 0;
}

//-------------------------------------------------------------
/**
 * @brief   GF�r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   GF_BSS_DATA_INFO*  �r�[�R���o�b�t�@�|�C���^
 */
//-------------------------------------------------------------

void* GFL_NET_WLGetGFBss(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		return (void*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
	}
	return NULL;
}

//-------------------------------------------------------------
/**
 * @brief   User��`�̃r�[�R���f�[�^�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   GF_BSS_DATA_INFO*  �r�[�R���o�b�t�@�|�C���^
 */
//-------------------------------------------------------------

void* GFL_NET_WLGetUserBss(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		_GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
    return (void*)pGF->aBeaconDataBuff;
	}
	return NULL;
}

//-------------------------------------------------------------
/**
 * @brief   �r�[�R���̒��ɂ���}�b�N�A�h���X�𓾂�
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @return  maccaddress�̔z��
 */
//-------------------------------------------------------------

void* GFL_NET_WLGetUserMacAddress(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		return (void*)&pNetWL->sBssDesc[index].bssid[0];
	}
	return NULL;
}

//-------------------------------------------------------------
/**
 * @brief   �r�[�R����GameserviceID���擾����
 * @param   index �r�[�R���o�b�t�@�ɑ΂���index
 * @return  maccaddress�̔z��
 */
//-------------------------------------------------------------
GameServiceID GFL_NET_WLGetUserGameServiceId(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		_GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
		return pGF->serviceNo;
	}
	return WB_NET_NOP_SERVICEID;
}

//-------------------------------------------------------------
/**
 * @brief    �r�[�R���f�[�^������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//-------------------------------------------------------------

void GFL_NET_WLResetWMBssDesc(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		pNetWL->bconUnCatchTime[index] = 0;
	}
}

//-------------------------------------------------------------
/**
 * @brief    �r�[�R���f�[�^��S������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//-------------------------------------------------------------

void GFL_NET_WLResetWMBssDescAll(void)
{
  int i;
  GFL_NETWL* pNetWL = _pNetWL;
  if(pNetWL){
    for(i=0;i<SCAN_PARENT_COUNT_MAX;i++){
      pNetWL->bconUnCatchTime[i] = 0;
    }
	}
}

//-------------------------------------------------------------
/**
 * @brief    �r�[�R���f�[�^������
 * @param    index �r�[�R���o�b�t�@�ɑ΂���index
 * @retval   none
 */
//-------------------------------------------------------------

void GFL_NET_WLResetGFBss(int index)
{
  GFL_NET_WLResetWMBssDesc(index);
}

//-------------------------------------------------------------
/**
 * @brief   mac�A�h���X���o�b�N�A�b�v����
 * @param   pMac   mac address
 * @retval  none
 */
//-------------------------------------------------------------

void GFL_NET_WLSetBackupMacAddress(u8* pMac, int netID)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
    if(GFL_NET_MACHINE_MAX > netID){
      MI_CpuCopy8(pMac, pNetWL->backupBssid[netID], WM_SIZE_BSSID);
    }
  }
}

//-------------------------------------------------------------
/**
 * @brief   �o�b�N�A�b�v����MAC�A�h���X�ɊY�����邩�ǂ����𓾂�
 * @param   pMac   mac address
 * @retval  none
 */
//-------------------------------------------------------------

static BOOL _isMachBackupMacAddress(u8* pMac)
{
	GFL_NETWL* pNetWL = _pNetWL;
	int i;

	for(i = 0; i < GFL_NET_MACHINE_MAX; i++){
		if(WM_IsBssidEqual(pNetWL->backupBssid[i], pMac)){
			return TRUE;
		}
	}
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   WMBssDesc���璼��GF_BSS_DATA_INFO�o�b�t�@�𓾂�
 * @param   
 * @retval   GFL_NET_WLGetUserBss�Ŏ���̂Ɠ����r�[�R���o�b�t�@�|�C���^
 */
//-------------------------------------------------------------
void* GFL_NET_WLGetDirectGFBss(WMBssDesc *pBss, GameServiceID *dest_gsid)
{
	_GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pBss->gameInfo.userGameInfo;
  *dest_gsid = pGF->serviceNo;
  return (void*)pGF->aBeaconDataBuff;
}


//-------------------------------------------------------------
/**
 * @brief   �����ؒf���[�h�ɓ��������ǂ�����Ԃ�
 * @param   none
 * @retval  �����Ă���Ȃ��TRUE
 */
//-------------------------------------------------------------
#if 0
BOOL GFL_NET_WLIsAutoExit(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		return pNetWL->bAutoExit;
	}
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   �����ؒf���[�hON
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------

void GFL_NET_WLSetAutoExit(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL){
		pNetWL->bAutoExit = TRUE;
	}
}
#endif
//-------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�Ɍ��݂̏󋵂𔽉f������
 * @retval  none
 */
//-------------------------------------------------------------
/*
void GFL_NET_WLFlashMyBss(void)
{
    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
    GFL_NETWL* pNetWL = _pNetWL;
    GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
    int size;

    if(pInit->beaconGetSizeFunc!=NULL){
        NetBeaconGetSizeFunc func = pInit->beaconGetSizeFunc;
        size = func(pNet->pWork);
    }

    NET_WHPIPE_BeaconSetInfo();
}
 */
//-------------------------------------------------------------
/**
 * @brief   ���C�t�^�C�������������� �܂��͌��ɖ߂�
 * @param   bMinimum TRUE�Ȃ珬��������
 * @retval  none
 */
//-------------------------------------------------------------

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
	GFL_NETWL* pNetWL = _pNetWL;
	return pNetWL->channel;
}

//-------------------------------------------------------------
/**
 * @brief	���@�r�[�R�����M���e�̎擾
 * @param none
 * @return �r�[�R���̒��g
 */
//-------------------------------------------------------------
void* GFL_NET_WLGetMyGFBss(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
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
	GFL_NETWL* pNetWL = _pNetWL;
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

//-------------------------------------------------------------
/**
 * @brief   �e�@�J�n����
 */
//-------------------------------------------------------------

static void _parentMeasurechanneling(GFL_NETWL* pNetWL)
{
	int state = WH_GetSystemState();
	u16 mode[]={WH_CONNECTMODE_DS_PARENT,WH_CONNECTMODE_MP_PARENT};
	//    GFL_NETWL* pNetWL = _pNetWL;
	BOOL bRet;

	if(WH_SYSSTATE_MEASURECHANNEL == state){
		u16 channel;
		GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();

		pNetWL->bPauseConnect = FALSE;
		pNetWL->bScanCallBack = FALSE;
		//    pNetWL->bAutoExit = FALSE;
		pNetWL->bEndScan = 0;
		pNetWL->bSetReceiver = FALSE;
		channel = WH_GetMeasureChannel();
		if(pNetWL->keepChannel == 0xff){
			pNetWL->keepChannel = channel;
#if DEBUG_WH_BEACON_PRINT_ON
			NET_PRINT("channelChange %d\n",channel);
#endif
    }
		else{
			channel = pNetWL->keepChannel;
		}
		if(pInit->bTGIDChange){
			pNetWL->_sTgid = WM_GetNextTgid();
		}
		NET_WHPIPE_BeaconSetInfo();
		bRet = WH_ParentConnect(mode[pInit->bMPMode], pNetWL->_sTgid, channel, pInit->maxConnectNum-1 );
		//   pNetWL->pCallback(bRet);
		_CHANGE_STATE(NULL);
	}
	//    return FALSE;
}


//-------------------------------------------------------------
/**
 * @brief   �A�C�h����ԂȂ�`�����l���������J�n
 */
//-------------------------------------------------------------

static void _parentMeasurechannel(GFL_NETWL* pNetWL)
{
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
		if(WH_StartMeasureChannel()){
			_CHANGE_STATE(_parentMeasurechanneling);
		}
	}
}

//-------------------------------------------------------------
/**
 * @brief   �e�@�J�n
 * @retval  none
 */
//-------------------------------------------------------------

BOOL GFL_NET_WL_ParentConnect(BOOL channelChange)
{
	if(_pNetWL->state){
		return FALSE;
	}
	if(channelChange){
		_pNetWL->keepChannel = 0xff;  // �`�����l����ύX����
	}
	//if(!_pNetWL->bSetReceiver )
	{
		WH_SetReceiver(_receiverFunc);
		_pNetWL->bSetReceiver = TRUE;
	}
	_CHANGE_STATE(_parentMeasurechannel);
	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   �r�[�R�����Z�b�g���� WH�̏�ԂŌĂԊ֐����قȂ�׃p�b�N
 * @retval  none
 */
//-------------------------------------------------------------

static void _setBeacon(void* buff, int size)
{
	GFL_NETWL* pNetWL = _pNetWL;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();

	if((pNetWL==NULL) || (pInit==NULL)){
		return;
	}

  {
    u16* pData = (u16*)buff;
    pData[0] = GFL_STD_CrcCalc(&pData[1], size - 2);
  }

  
	DC_FlushRange(buff, size);
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
		WH_SetUserGameInfo(buff, size);
	}
	else{
		WHSetGameInfo(buff, size, pInit->ggid, pNetWL->_sTgid);
	}
//	NET_PRINT("�r�[�R���ݒ�\n");
}




static void _connectAutoFunc(GFL_NETWL* pNetWL)
{
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();
	u16 mode[]={WH_CONNECTMODE_DS_CHILD,WH_CONNECTMODE_MP_CHILD};

	if (WH_GetSystemState() == WH_SYSSTATE_IDLE || WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
		WH_ChildConnectAuto(mode[pInit->bMPMode], _pNetWL->connectMac, 0);
		//        GFI_NET_BeaconSetScanCallback(pNetWL->pCallback);
		//    GFL_NETWL* pNetWL = _pNetWL;
		//    pNetWL->bEndScan = 0;
		//  pNetWL->pCallback = pCallback;
		WH_SetScanCallback(_parentFindCallback);
		{
			WH_SetReceiver(_receiverFunc);
			_pNetWL->bSetReceiver = TRUE;
		}


		_CHANGE_STATE(NULL);
	}
}

//-------------------------------------------------------------
/**
 * @brief   �q�@�@MAC�w��������̓C���f�b�N�X�w��Őڑ�
 * @param   macAddress    �}�b�N�A�h���X
 * @retval  �q�@�ڑ���e�@�ɑ�������TRUE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLChildMacAddressConnect(BOOL bBconInit,u8* macAddress, int macIndex, PTRPARENTFIND_CALLBACK pCallback)
{
	//    GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();
	BOOL ret = TRUE;
	GFLNetInitializeStruct* pInit = GFL_NET_GetNETInitStruct();

	if(_pNetWL->state){
		return FALSE;
	}
	if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
		_pNetWL->bEndScan = 0;
			NET_PRINT("GFL_NET_WLChildMacAddressConnect WH_EndScan\n");
		WH_EndScan();
	}
	_CHANGE_STATE(_connectAutoFunc);
	if(macAddress){
		GFL_STD_MemCopy(macAddress,_pNetWL->connectMac,WM_SIZE_BSSID);
	}
	else{
		GFL_STD_MemCopy(_pNetWL->sBssDesc[macIndex].bssid,_pNetWL->connectMac,WM_SIZE_BSSID);
	}
	_pNetWL->pCallback = pCallback;

	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   �I���J��
 * @param
 * @retval
 */
//-------------------------------------------------------------

static void _whEndFunc(GFL_NETWL* pNetWL)
{
	if(WH_GetSystemState() == WH_SYSSTATE_STOP){
		WH_FreeMemory();
		_commEnd();
	}
}

//-------------------------------------------------------------
/**
 * @brief   �I���J������
 * @param   callback �I��������̃R�[���o�b�N
 * @retval  TRUE
 */
//-------------------------------------------------------------

BOOL GFI_NET_WHPipeEnd(NetDevEndCallback callback)
{
  if(WH_End(callback)){
    _CHANGE_STATE(_whEndFunc);
    return TRUE;
  }
	return FALSE;
}


// �ڑ�������֐��^
static BOOL _connectEnable(WMStartParentCallback* pCallback, void* pWork)
{
	return TRUE;
}

// �ڑ�������֐��^
static BOOL _connectDisable(WMStartParentCallback* pCallback, void* pWork)
{
	return FALSE;
}


//-------------------------------------------------------------
/**
 * @brief   �ڑ����񋖉؂�ւ�
 * @param   bEnable  �Ȃ�����TRUE
 * @retval  TRUE
 */
//-------------------------------------------------------------

void GFL_NET_WHPipeSetClientConnect(BOOL bEnable)
{
	if(bEnable){
		WH_SetJudgeAcceptFunc(&_connectEnable);
	}
	else{
		WH_SetJudgeAcceptFunc(&_connectDisable);
	}
}

//-------------------------------------------------------------
/**
 * @brief   ����Ⴂ�ʐM
 * @param   bEnable  �Ȃ�����TRUE
 * @retval  TRUE
 */
//-------------------------------------------------------------

static void _crossScanShootStart(GFL_NETWL* pNetWL);


static void _crossScanWait(GFL_NETWL* pNetWL)
{
  if(!_pNetWL->PauseScan){   //�e�@�Œ聁�r�[�R�����M�̂�
    if(pNetWL->CrossRand!=0){
      pNetWL->CrossRand--;
    }
    if( pNetWL->CrossRand == 0){
      if(WH_Finalize()){
        _CHANGE_STATE(_crossScanShootStart);
      }
    }
  }
}


static void _crossScanShootEndWait(GFL_NETWL* pNetWL)
{
  if(GFL_NET_WLIsStateIdle()){
    u8 ch[]={1,7,13};
    pNetWL->crossChannel = GFUser_GetPublicRand(elementof(ch));
    if(pNetWL->crossChannel >= elementof(ch)){
      pNetWL->crossChannel=0;
    }
		NET_WHPIPE_BeaconSetInfo();
    if( WH_ParentConnect(WH_CONNECTMODE_MP_PARENT, pNetWL->_sTgid, ch[pNetWL->crossChannel], 1 )){
      pNetWL->CrossRand = 60 + GFUser_GetPublicRand(152);
      _CHANGE_STATE(_crossScanWait);  // �e�@�ɂȂ�
    }
  }
}



static void _crossScanShootWait(GFL_NETWL* pNetWL)
{
  pNetWL->CrossRand--;
  
  if( pNetWL->CrossRand == 0){
//			NET_PRINT("_crossScanShootWait WH_EndScan\n");
    WH_EndScan(); //�X�L�����I��
    _CHANGE_STATE(_crossScanShootEndWait);  // �X�L�����I���҂�
#if DEBUG_WH_BEACON_PRINT_ON
    NET_PRINT( "scan off\n" );
#endif
  }
}


static void _crossScanShootStart(GFL_NETWL* pNetWL)
{
  if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
    if(WH_StartScan(_scanCallback, NULL, 0)){
      pNetWL->CrossRand = 60+GFUser_GetPublicRand(40);
      _CHANGE_STATE(_crossScanShootWait);  // �X�L�����҂�
#if DEBUG_WH_BEACON_PRINT_ON
      NET_PRINT( "scan on\n" );
#endif
		}
	}
}

//-------------------------------------------------------------
/**
 * @brief   ����Ⴂ�ʐM�J�n
 * @param   none
 * @retval  nene
 */
//-------------------------------------------------------------
BOOL GFL_NET_WLCrossoverInit(void)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(pNetWL == NULL){
		return FALSE;
  }
  GFL_NET_WLChangeScanSpeed(GFL_NET_CROSS_SPEED_FAST);

  _commInit(pNetWL);

  GFL_NET_WLChildBconDataInit(); // �f�[�^�̏�����
	{
		WH_SetReceiver(_receiverFunc);
		pNetWL->bSetReceiver = TRUE;
	}

  _CHANGE_STATE(_crossScanShootStart);

  return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   �ؒf�R�[���o�b�N����������G���[�ɂ���
 * @param   none
 * @retval  nene
 */
//-------------------------------------------------------------
void GFL_NET_WL_DisconnectError(void)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(pNetWL->bErrorCheck){
    pNetWL->bErrorState = TRUE;   ///< �G���[�������N�����Ă���ꍇ���̏�Ԃ������܂�
  }
}



//-------------------------------------------------------------
/**
 * @brief   �ێ����Ă���r�[�R���̃T�[�r�X�ԍ���Ԃ�
 * @param   index  �r�[�R��index
 * @retval  GameServiceID
 */
//-------------------------------------------------------------

GameServiceID GFL_NET_WLGetGameServiceID(u8 index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	if(pNetWL && (pNetWL->bconUnCatchTime[index]!=0)){
		_GF_BSS_DATA_INFO* pGF = (_GF_BSS_DATA_INFO*)pNetWL->sBssDesc[index].gameInfo.userGameInfo;
		return pGF->serviceNo;
	}
	return WB_NET_NOP_SERVICEID;
}

//-------------------------------------------------------------
/**
 * @brief   �X�L��������r�[�R��������index�̃r�[�R���ɍi�荞��
 * @param   index  �r�[�R��index
 * @retval  GameServiceID
 */
//-------------------------------------------------------------

void GFL_NET_WLFIXScan(int index)
{
  WIH_FixScanMode(_pNetWL->sBssDesc[index].channel, _pNetWL->sBssDesc[index].bssid );
}


//-------------------------------------------------------------
/**
 * @brief   �X�L�������鑬�x�̕ύX
 * @param   num   2:�͂₢(default) 1:������  0:�Ƃ܂��Ă���
 */
//-------------------------------------------------------------

void GFL_NET_WLChangeScanSpeed(int num)
{

  if(_pNetWL){
    _pNetWL->crossTimer = _BEACON_DOWNCOUNT;
  }

}

//-------------------------------------------------------------
/**
 * @brief   �r�[�R���͏o���Ă��邪�A�ꎞ���Ăق��������ꍇ�ɑ��삷��
 * @param   
 */
//-------------------------------------------------------------

void GFL_NET_WL_PauseBeacon(int flg)
{
  if(_pNetWL){
    _pNetWL->bPauseConnect = flg;
    NET_WHPIPE_BeaconSetInfo();
  }
}

/////////////////////////////////////////////////////////////////////////
//�f�o�b�O�p���[�`��
#ifdef PM_DEBUG

//-----------------------------------------------
/**
 *  @brief  �ڑ��������R�[�h��ʐM�V�X�e���ɔ��f�@
 */
//-----------------------------------------------
void DEBUG_GFL_NET_AloneTestCodeReflect( void )
{
	GFL_NETWL* pNetWL = NULL;

	if( _pNetWL == NULL ){
		return; //�ʐM���������Ȃ̂ŕύX���Ȃ�
	}
	pNetWL = _pNetWL;
  
  pNetWL->mineDebugNo = DebugAloneTest;
}

#endif  //PM_DEBUG


//-------------------------------------------------------------
/**
 * @brief   �r�[�R���X�L�����̈ꎞ��~
 * @param   bPause TRUE�Œ�~ FALSE�ŉ���
 */
//-------------------------------------------------------------

void GFL_NET_WL_PauseScan(BOOL bPause)
{

  if(_pNetWL){
    _pNetWL->crossTimerPause = bPause;
    if(bPause){
      _pNetWL->crossTimer = _BEACON_DOWNCOUNT;
    }
    else{
      _pNetWL->crossTimer = 1;
    }
  }

}

