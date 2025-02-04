//=============================================================================
/**
 * @file	net_whpipe.c
 * @brief	通信システム
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

//接続相手制限コード定義
#include "net_alone_test.h"

#ifdef PM_DEBUG

#define DEBUG_WH_BEACON_PRINT_ON (0)  // beacon切り替えタイミングを表示

#if DEBUG_ONLY_FOR_ohno
#define DEBUG_WH_BEACON_PRINT_ON  (0) // beacon切り替えタイミングを表示
#endif  //DEBUG_ONLY_FOR_ohno

extern u8 DebugAloneTest;

#endif  //PM_DEBUG

#define POKEMONWB_BEACON_PRODUCT_NO (0)   //この番号のビーコンは製品版

/**
 *  @brief _BEACON_SIZE_FIXには 固定でほしいビーコンパラメータの合計を手で書く
 */
#define _BEACON_SIZE_FIX (8)
#define _BEACON_FIXHEAD_SIZE (2)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)


#define _BEACON_DOWNCOUNT  (20)   //すれ違いを一時停止するフレーム

/**
 *  @brief ビーコン構造体
 */
// WM_SIZE_USER_GAMEINFO  最大 112byte
// _BEACON_SIZE_FIXには 固定でほしいビーコンパラメータの合計を手で書く
typedef struct{
	u16        CRC;         ///< CRC
	u16        GGID;               ///< ゲーム固有のID  一致が必須
	GameServiceID  		serviceNo; ///< 通信サービス番号
	u8        ProductOrDevelopment;      ///< 開発と本番とがつながらないようにするため
	u8  	    connectNum;    	   ///< つながっている台数  --> 本親かどうか識別
	u8        pause;               ///< 接続を禁止したい時に使用する
	u8        aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
} _GF_BSS_DATA_INFO;


//-------------------------------------------------------------
// 定義
//-------------------------------------------------------------

//#define _PORT_DATA_RETRANSMISSION   (14)    // 切断するまで無限再送を行う  こちらを使用している
#define _NOT_INIT_BITMAP  (0xffff)   // 接続人数を固定に指定ない場合の値

typedef enum{    // 切断状態
	_DISCONNECT_NONE,
	_DISCONNECT_END,
	_DISCONNECT_SECRET,
} _DisconnectState_e;


typedef struct _NET_WL_WORK  GFL_NETWL;

/// コールバック関数の書式 内部ステート遷移用
typedef void (*PTRStateFunc)(GFL_NETWL* pState);

/// 時間取得
typedef int (*PTRTimeGet)(void);


//管理構造体定義
struct _NET_WL_WORK {
	PTRStateFunc state;      ///< ハンドルのプログラム状態
	PTRCommRecvLocalFunc recvCallback; ///< 受信コールバック解決用
	WMBssDesc sBssDesc[SCAN_PARENT_COUNT_MAX];  ///< 親機の情報を記憶している構造体
	u8  backupBssid[GFL_NET_MACHINE_MAX][WM_SIZE_BSSID];   // 今まで接続していた
  PIPESetBeaconCatchFunc beaconCatchFunc; //
  void* beaconCatchFuncWork;
  u16 bconUnCatchTime[SCAN_PARENT_COUNT_MAX]; ///< 親機のビーコンを拾わなかった時間+データがあるかどうか
	PTRPARENTFIND_CALLBACK pCallback;
	void* pUserWork;
	u16 _sTgid;
	u16 errCheckBitmap;      ///< このBITMAPが食い違うとエラーになる
  u16 crossTimer;
	u8 gameInfoBuff[WM_SIZE_USER_GAMEINFO];  //送信するビーコンデータ
	u8 connectMac[WM_SIZE_BSSID];
	u8 mineDebugNo;       ///< 通信識別子、デバッグ用 本番では０
	u8 channel;
	u8 keepChannel;     ///<記録しておくチャンネルの場所
	u8 disconnectType;    ///< 切断状況
	u8 bSetReceiver;
	u8 bEndScan;  // endscan
	u8 bPauseConnect;   ///< 子機の受付中止状態
	u8 bErrorState;   ///< エラーを引き起こしている場合その状態をもちます
	u8 bErrorCheck;   ///< 子機が無い場合+誰かが落ちた場合エラー扱いするかどうか
	u8 bTGIDChange;
	u8 bEntry;        ///< 子機の新規参入
	u8 bScanCallBack;  ///< 親のスキャンがかかった場合TRUE, いつもはFALSE
	u8 bChange;   ///<ビーコン変更フラグ
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
// ワーク
//-------------------------------------------------------------

///< ワーク構造体のポインタ
static GFL_NETWL* _pNetWL = NULL;

// 親機になる場合のTGID 構造体に入れていないのは
// 通信ライブラリーを切ったとしてもインクリメントしたいため
/// TGID管理


// コンポーネント転送終了の確認用
// イクニューモンコンポーネント処理を移動させるときはこれも移動
//static volatile int   startCheck;


//-------------------------------------------------------------
// static宣言
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


static void _changeState(GFL_NETWL* pState, PTRStateFunc state);  // ステートを変更する

#if PM_DEBUG
static void _changeStateDebug(GFL_NETWL* pState, PTRStateFunc state, int line);  // ステートを変更する
#define   _CHANGE_STATE(state)  _changeStateDebug(_pNetWL ,state, __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(_pNetWL ,state)
#endif //GFL_NET_DEBUG


//-------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//-------------------------------------------------------------

static void _changeState(GFL_NETWL* pState,PTRStateFunc state)
{
	pState->state = state;
}

//-------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
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
 * @brief   接続クラスの初期化
 * @param   heapID    ワーク確保ID
 * @param   getFunc   ビーコン作成関数
 * @param   getSize   ビーコンサイズ関数
 * @param   getComp   ビーコン比較関数
 * @param   bConnect  通信する場合TRUE
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
	// WH 初期設定
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
	GF_ASSERT_HEAVY(WM_SIZE_USER_GAMEINFO == sizeof(_GF_BSS_DATA_INFO));
	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   受信コールバック関数を指定
 * @param   recvCallback   受信コールバック関数
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
 * @brief   接続しているかどうか
 * @retval  TRUE  接続している
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
 * @brief   マックアドレス表示
 * @param   bssdesc   グループ情報
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
 * @brief   接続して良いかどうか判断する
 * @param   bssdesc   グループ情報
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

	// catchした親データ
	pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
	if(pGF->pause){
		return FALSE;  // ポーズ中の親機はBEACON無視
	}
//	NET_PRINT("debugNo %d %d\n",pGF->debugAloneTest , _DEBUG_ALONETEST);

#ifdef PM_DEBUG  // デバッグの時だけ、上に定義がある人は基本他の人とつながらない
	if(pGF->ProductOrDevelopment != pNetWL->mineDebugNo){
		return FALSE;   //パレスの為
	}
#else
  if(pGF->ProductOrDevelopment != POKEMONWB_BEACON_PRODUCT_NO){
		return FALSE;   //パレスの為
  }
#endif
//	GFLR_NET_GetBeaconHeader(sBuff,_BEACON_FIXHEAD_SIZE);
//	if(0 != GFL_STD_MemComp(sBuff, pGF->FixHead , _BEACON_FIXHEAD_SIZE)){
  {
    u16* pData = (u16*)pGF;
    if(pGF->CRC != GFL_STD_CrcCalc(&pData[1], sizeof(_GF_BSS_DATA_INFO) - 2) ){
      NET_PRINT("CRC不一致\n");
      return FALSE;
    }
  }
	if(pGF->GGID != ggid){
#if DEBUG_WH_BEACON_PRINT_ON
		NET_PRINT("beacon不一致\n");
#endif
    return FALSE;
	}
  if(_pNetWL->beaconCatchFunc){
    _pNetWL->beaconCatchFunc(_pNetWL->beaconCatchFuncWork ,bssdesc, pGF->serviceNo, pGF->connectNum);  //ユニオンの人数収集
  }

  if(pInit->beaconCompFunc){
		if(FALSE == pInit->beaconCompFunc(serviceNo, pGF->serviceNo,pNetWL->pUserWork)){
			return FALSE;   // サービスが異なる場合は拾わない
		}
	}
	else{
#if DEBUG_WH_BEACON_PRINT_ON
		NET_PRINT("ServiceID CMP %d %d",serviceNo, pGF->serviceNo);
#endif
		if(serviceNo != pGF->serviceNo){
			return FALSE;   // サービスが異なる場合は拾わない
		}
	}
  if(serviceNo != WB_NET_UNION){  //ユニオンルーム以外は
    if((pGF->connectNum >= pInit->maxConnectNum) && (pInit->maxConnectNum>1)){
      return FALSE;   // 接続人数いっぱいの場合拾わない
    }
  }
//	NET_PRINT("_scanCheckok\n");
	return TRUE;
}



//-------------------------------------------------------------
/**
 * @brief   子機が親機を探し出した時に呼ばれるコールバック関数
 * 親機を拾うたびに呼ばれる
 * @param   bssdesc   グループ情報
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

	// このループは同じものなのかどうか検査
	for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
		if(pNetWL->bconUnCatchTime[i] == 0 ){
			// 親機情報が入っていない場合continue
			continue;
		}
		if (0==GFL_STD_MemComp(pNetWL->sBssDesc[i].bssid, bssdesc->bssid, WM_SIZE_BSSID)) {
			// もう一度拾った場合にタイマー加算
#if DEBUG_WH_BEACON_PRINT_ON
			NET_PRINT("もう一度拾った場合にタイマー加算\n");
#endif
			pNetWL->bconUnCatchTime[i] = DEFAULT_TIMEOUT_FRAME;
			// 新しい親情報を保存しておく。
			MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i], sizeof(WMBssDesc));
			return TRUE;
		}
	}
	// このループは空きがあるかどうか検査
	for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
		if(pNetWL->bconUnCatchTime[i] == 0 ){
			// 親機情報が入っていない場合break;
			break;
		}
	}
	if(i >= SCAN_PARENT_COUNT_MAX){
		// 構造体がいっぱいの場合は親機を拾わない
		return FALSE;
	}
	// 新しい親情報を保存しておく。
	pNetWL->bconUnCatchTime[i] = DEFAULT_TIMEOUT_FRAME;
	MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i],sizeof(WMBssDesc));
	pNetWL->bScanCallBack = TRUE;
//	NET_PRINT("_scanCallback追加\n");

	return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   子機が親機を見つけたビーコンに対してかけるチェック
 * @param   bssdesc   グループ情報
 * @retval  TRUE:問題なし(使用OK)
 * @retval  FALSE:問題あり(使用NG)
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

	// catchした親データ
	pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
	if(check_bit & WL_SCAN_CHK_BIT_PAUSE){
  	if(pGF->pause){
  		return FALSE;  // ポーズ中の親機はBEACON無視
  	}
  }
//	NET_PRINT("debugNo %d %d\n",pGF->debugAloneTest , _DEBUG_ALONETEST);

  if(check_bit & WL_SCAN_CHK_BIT_DEVELOPMENT){
  #ifdef PM_DEBUG  // デバッグの時だけ、上に定義がある人は基本他の人とつながらない
  	if(pGF->ProductOrDevelopment != pNetWL->mineDebugNo){
  		return FALSE;   //パレスの為
  	}
  #else
    if(pGF->ProductOrDevelopment != POKEMONWB_BEACON_PRODUCT_NO){
  		return FALSE;   //パレスの為
    }
  #endif
  }
//	GFLR_NET_GetBeaconHeader(sBuff,_BEACON_FIXHEAD_SIZE);
//	if(0 != GFL_STD_MemComp(sBuff, pGF->FixHead , _BEACON_FIXHEAD_SIZE)){
  
  if(check_bit & WL_SCAN_CHK_BIT_CRC)
  {
    u16* pData = (u16*)pGF;
    if(pGF->CRC != GFL_STD_CrcCalc(&pData[1], sizeof(_GF_BSS_DATA_INFO) - 2) ){
      NET_PRINT("CRC不一致\n");
      return FALSE;
    }
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_GGID){
  	if(pGF->GGID != ggid){
  #if DEBUG_WH_BEACON_PRINT_ON
  		NET_PRINT("beacon不一致\n");
  #endif
      return FALSE;
  	}
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_BEACON_COMP){
  	if(pInit->beaconCompFunc){
  		if(FALSE == pInit->beaconCompFunc(serviceNo, pGF->serviceNo, pNetWL->pUserWork)){
  			return FALSE;   // サービスが異なる場合は拾わない
  		}
  	}
  	else{
  #if DEBUG_WH_BEACON_PRINT_ON
  		NET_PRINT("ServiceID CMP %d %d",serviceNo, pGF->serviceNo);
  #endif
  		if(serviceNo != pGF->serviceNo){
  			return FALSE;   // サービスが異なる場合は拾わない
  		}
  	}
  }
  
  if(check_bit & WL_SCAN_CHK_BIT_MAX_CONNECT){
  	if(pGF->connectNum >= pInit->maxConnectNum){
  		return FALSE;   // 接続人数いっぱいの場合拾わない
  	}
  }
//	NET_PRINT("_scanCheckok\n");
	return TRUE;
}


//-------------------------------------------------------------
/**
 * @brief   子機の使用しているデータの初期化
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
 * @brief   親機の使用しているデータの初期化
 * @param   bTGIDChange  新規のゲームの初期化の場合TRUE 古いビーコンでの誤動作を防ぐため用
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
 * @brief   親子共通、通信の初期化をまとめた
 * @retval  初期化に成功したらTRUE
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
 * @brief   受信コールバック
 * @param   aid    通信のlocalID
 * @param   *data  受信データ
 * @param   size   受信サイズ
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
 * @brief   子機の接続開始を行う
 * @param   work_area 　システムで使うメモリー領域
 *                      NULLの場合はすでに初期化済みとして扱う
 * @param   serviceNo  ゲームの種類
 * @param   regulationNo  ゲームの種類
 * @param   bBconInit  ビーコンデータを初期化するのかどうか
 * @retval  初期化に成功したらTRUE
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
		NET_PRINT("ビーコンの初期化\n");
#endif
    GFL_NET_WLChildBconDataInit(); // データの初期化
	}
	//if(!pNetWL->bSetReceiver )
	{
		WH_SetReceiver(_receiverFunc);
		pNetWL->bSetReceiver = TRUE;
	}
	// 親機検索スタート
	if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
		if(WH_StartScan(_scanCallback, NULL, 0)){
			return TRUE;
		}
	}
#if DEBUG_WH_BEACON_PRINT_ON
	NET_PRINT("IDLE状態ではないため初期化に失敗しました\n");
#endif
	return FALSE;
}

//-------------------------------------------------------------
/**
 * @brief   通信切り替えを行う（親子反転に必要な処理）
 * @param   none
 * @retval  リセットしたらTRUE
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
			NET_PRINT("スキャンを消す------WH_EndScan\n");
			WH_EndScan();
			pNetWL->bEndScan = 1;
			break;
		}
		else if(WH_SYSSTATE_BUSY == WH_GetSystemState()){  //しばらく待つ
		}
		else if(WH_Finalize()){
			pNetWL->bEndScan = 2;
		}
		break;
	case 1:
		if(WH_SYSSTATE_BUSY != WH_GetSystemState()){
			NET_PRINT("終了処理----2\n");
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
 * @brief   終了開放
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
 * @brief   終了開放
 * @param
 * @retval
 */
//-------------------------------------------------------------

static void _whEndErrResetFunc(GFL_NETWL* pNetWL)
{
  pNetWL->ErrorEndTimer--;  //通信システムがBUSYのまま処理変更が去れない場合があるので タイマー監視でWH_Resetを呼ぶように
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
 * @brief   通信切断を行う  ここではあくまで通信終了手続きに入るだけ
 *  ホントに消すのは下の_commEnd
 * @param   none
 * @retval  終了処理に移った場合TRUE
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
 * @brief   通信の全てを消す
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
 * @brief   探すことができた親の数を返す
 * @param   none
 * @retval  親機の数
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
 * @brief   BmpListの位置からBconの存在するIndex位置を取得する
 * @param   index		BmpList位置
 * @retval  Index
 */
//--------------------------------------------------------------
int CommBmpListPosBconIndexGet(int index)
{
	GFL_NETWL* pNetWL = _pNetWL;
	int i, count;

	//	for(i = 0; i < SCAN_PARENT_COUNT_MAX; i++){
	//		OS_TPrintf("Listのビーコンチェック %d = %d\n", i, pNetWL->bconUnCatchTime[i]);
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
	GF_ASSERT_HEAVY(0 && "ここには来ないはず");
	return 0;
}

//-------------------------------------------------------------
/**
 * @brief   親機リストに変化があった場合TRUE
 * @param   none
 * @retval  親機リストに変化があった場合TRUE なければFALSE
 */
//-------------------------------------------------------------

BOOL GFL_NET_WLIsScanListChange(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	return pNetWL->bScanCallBack;
}

//-------------------------------------------------------------
/**
 * @brief   親機の変化を知らせるフラグをリセットする
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
 * @brief   この親機がいくつとコネクションをもっているのかを得る
 * @param   index   親のリストのindex
 * @retval  コネクション数 0-16
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
 * @brief   接続人数に該当する親を返す
 * @param   none
 * @retval  親のindex
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
 * @brief   ビーコンデータの定期確認
 *  接続が完了する間での間、この関数を呼び、タイムアウト処理を行う
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
				NET_PRINT("親機反応なし %d\n", id);
#endif
				pNetWL->bScanCallBack = TRUE;   // データを変更したのでTRUE
			}
		}
	}
}

//-------------------------------------------------------------
/**
 *  @brief   ユーザ定義の親機情報を設定します。
 *  _GF_BSS_DATA_INFO構造体の中身を送ります
 *  @param   userGameInfo  ユーザ定義の親機情報へのポインタ
 *  @param   length        ユーザ定義の親機情報のサイズ
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
    pGF->serviceNo = pInit->gsid;    // ゲームの番号
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
 *  @brief   ユーザ定義の親機情報の 人数部分を監視し常に更新する
 *  _GF_BSS_DATA_INFO構造体の中身を送ります
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
    GF_ASSERT_HEAVY(size <= _BEACON_USER_SIZE_MAX);
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
 * @brief   通信ライブラリー内部の状態を見て、処理をする関数
 * VBlankとは関係ないのでprocessの時に処理すればいい
 * @param   none
 * @retval  none
 */
//-------------------------------------------------------------
static void _stateProcess(u16 bitmap)
{
	int state = WH_GetSystemState();
	GFL_NETWL* pNetWL = _pNetWL;

  if(!GFL_NET_WLIsError()){
    _funcBconDataChange();      // ビーコンの中身を書き換え中
  }
	if(pNetWL->errCheckBitmap == _NOT_INIT_BITMAP){
		pNetWL->errCheckBitmap = bitmap;  // このときの接続人数を保持
	}
	if(pNetWL->bErrorCheck){
		if((WH_GetCurrentAid() == GFL_NET_NO_PARENTMACHINE) && (!GFL_NET_WLIsChildsConnecting())){
			pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
		}
		if(pNetWL->errCheckBitmap > bitmap){  // 切断した場合必ず数字が減る 増える分にはOK
			pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
		}
	}
	if(WH_ERRCODE_FATAL == WH_GetLastError()){
		GFL_NET_FatalErrorFunc(0);
	}
	switch (state) {
	case WH_SYSSTATE_STOP:
		if(pNetWL->disconnectType == _DISCONNECT_END){
#if DEBUG_WH_BEACON_PRINT_ON
			NET_PRINT("WHEnd を呼んで終了しました\n");
#endif
      _commEnd();  // ワークから何から全て開放
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
		NET_PRINT("エラー中 %d \n",WH_GetLastError());
		if(pNetWL){
			if(!pNetWL->bErrorState){
				GFL_NET_ErrorFunc(WH_GetLastError());
			}
			pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
		}
		break;
	default:
		break;
	}
//  HW_Main();
  
}

//-------------------------------------------------------------
/**
 * @brief   通信ライブラリー内部の状態を見て、処理をする関数
 * VBlankとは関係ないのでprocessの時に処理すればいい
 * 子機はお互いの接続がわからないので、通信結果をcommsystemからもらってエラー検査する
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
 * @brief   デバイスが通信可能状態なのかどうかを返す
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
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
 * @brief   通信可能状態なのかどうかを返す
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
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
 * @brief   通信可能状態の人数を返す
 * @param   none
 * @retval  接続人数
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
 * @brief  初期化しているかどうかを返す
 * @param   none
 * @retval  初期が終わっていたらTRUE
 */
//-------------------------------------------------------------
BOOL GFL_NET_WLIsInitialize(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	return (pNetWL!=NULL);
}

//-------------------------------------------------------------
/**
 * @brief  WHライブラリで　状態がIDLEになっているか確認する
 * @param   none
 * @retval  IDLEになっている=TRUE
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
 * @brief  WHライブラリで　通信状態のBITを確認  子機がつながっているかどうか
 * @param   none
 * @retval  つながったらTRUE
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
 * @brief  親機が落ちたかどうか
 * @param   none
 * @retval  落ちた場合TRUE
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
 * @brief  エラー状態かどうか
 * @param   none
 * @retval  エラーの時TRUE
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
 * @brief   誰かが落ちたのをエラー扱いにするかどうかをSET
 * @param   bOn  有効時にTRUE
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
 * @brief   サービス番号に対応したビーコン間隔を得ます
 *          サービス番号は include/communication/comm_def.hにあります
 * @param   serviceNo サービス番号
 * @retval  beacon間隔 msec
 */
//-------------------------------------------------------------

u16 _getServiceBeaconPeriod(u16 serviceNo)
{
	u16 beaconPeriod = WM_GetDispersionBeaconPeriod();
	/*  必要ならコールバック関数として定義する
    基本的には必要としない方向で  k.ohno 06.12.05 */
	return beaconPeriod;
}

//-------------------------------------------------------------
/**
 * @brief   ビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   WMBssDesc*  ビーコンバッファポインタ
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
 * @brief    ビーコン強度を得る
 * @param    index ビーコンバッファに対するindex
 * @retval   u16  通信強度
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
 * @brief   GFビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
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
 * @brief   User定義のビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
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
 * @brief   ビーコンの中にあるマックアドレスを得る
 * @param   index ビーコンバッファに対するindex
 * @return  maccaddressの配列
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
 * @brief   ビーコンのGameserviceIDを取得する
 * @param   index ビーコンバッファに対するindex
 * @return  maccaddressの配列
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
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
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
 * @brief    ビーコンデータを全部消す
 * @param    index ビーコンバッファに対するindex
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
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
 * @retval   none
 */
//-------------------------------------------------------------

void GFL_NET_WLResetGFBss(int index)
{
  GFL_NET_WLResetWMBssDesc(index);
}

//-------------------------------------------------------------
/**
 * @brief   macアドレスをバックアップする
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
 * @brief   バックアップしたMACアドレスに該当するかどうかを得る
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
 * @brief   WMBssDescから直接GF_BSS_DATA_INFOバッファを得る
 * @param   
 * @retval   GFL_NET_WLGetUserBssで取れるのと同じビーコンバッファポインタ
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
 * @brief   自動切断モードに入ったかどうかを返す
 * @param   none
 * @retval  入っているならばTRUE
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
 * @brief   自動切断モードON
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
 * @brief   ビーコンデータに現在の状況を反映させる
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
 * @brief   ライフタイムを小さくする または元に戻す
 * @param   bMinimum TRUEなら小さくする
 * @retval  none
 */
//-------------------------------------------------------------

//void GFL_NET_WLSetLifeTime(BOOL bMinimum)
//{
//  WHSetLifeTime(bMinimum);
//}

//------------------------------------------------------
/**
 * @brief   サービス番号のビーコン数を返します
 * @param   serviceNo   comm_def.hにあるサービス番号
 * @retval  利用者数
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
 * @brief   親機が一回でもビーコンを送信し終わったかどうか
 * @retval  送信完了=TRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsParentBeaconSent(void)
{
	return (0!=WHGetBeaconSendNum());
}

//------------------------------------------------------
/**
 * @brief   スキャン状態かどうか
 * @retval  スキャン状態ならTRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsChildStateScan(void)
{
	return WH_GetSystemState() == WH_SYSSTATE_SCANNING;
}

//------------------------------------------------------
/**
 * @brief   チャンネルを返す
 * @retval  接続チャンネル
 */
//------------------------------------------------------

int GFL_NET_WLGetChannel(void)
{
	GFL_NETWL* pNetWL = _pNetWL;
	return pNetWL->channel;
}

//-------------------------------------------------------------
/**
 * @brief	自機ビーコン送信内容の取得
 * @param none
 * @return ビーコンの中身
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
 * @brief	ビーコンを拾ってからのカウントダウンタイマ取得
 * @param	index	位置
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
 * @brief	データ送信する
 * @param	data	送信データポインタ
 * @param	size    送信サイズ
 * @param	callback  送信完了コールバック
 * @retval  TRUE   送信できそうである
 * @retval  FALSE  送信できない
 */
//------------------------------------------------------
BOOL GFL_NET_WL_SendData(void* data,int size,PTRSendDataCallback callback)
{
	return WH_SendData(data, size, callback);
}

//------------------------------------------------------
/**
 * @brief	接続状態をBITMAPで返す
 * @return  接続状態
 */
//------------------------------------------------------

u16 GFL_NET_WL_GetBitmap(void)
{
	return WH_GetBitmap();
}

//------------------------------------------------------
/**
 * @brief	自分の接続IDを返す
 * @return  接続ID
 */
//------------------------------------------------------

u16 GFL_NET_WL_GetCurrentAid(void)
{
	return WH_GetCurrentAid();
}

//-------------------------------------------------------------
/**
 * @brief   親機開始処理
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
 * @brief   アイドル状態ならチャンネル検査を開始
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
 * @brief   親機開始
 * @retval  none
 */
//-------------------------------------------------------------

BOOL GFL_NET_WL_ParentConnect(BOOL channelChange)
{
	if(_pNetWL->state){
		return FALSE;
	}
	if(channelChange){
		_pNetWL->keepChannel = 0xff;  // チャンネルを変更する
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
 * @brief   ビーコンをセットする WHの状態で呼ぶ関数が異なる為パック
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
//	NET_PRINT("ビーコン設定\n");
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
 * @brief   子機　MAC指定もしくはインデックス指定で接続
 * @param   macAddress    マックアドレス
 * @retval  子機接続を親機に送ったらTRUE
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
 * @brief   終了開放
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
 * @brief   終了開放処理
 * @param   callback 終わった時のコールバック
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


// 接続許可判定関数型
static BOOL _connectEnable(WMStartParentCallback* pCallback, void* pWork)
{
	return TRUE;
}

// 接続許可判定関数型
static BOOL _connectDisable(WMStartParentCallback* pCallback, void* pWork)
{
	return FALSE;
}


//-------------------------------------------------------------
/**
 * @brief   接続許可非許可切り替え
 * @param   bEnable  つなぐ時はTRUE
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
 * @brief   すれ違い通信
 * @param   bEnable  つなぐ時はTRUE
 * @retval  TRUE
 */
//-------------------------------------------------------------

static void _crossScanShootStart(GFL_NETWL* pNetWL);


static void _crossScanWait(GFL_NETWL* pNetWL)
{
  if(!_pNetWL->PauseScan){   //親機固定＝ビーコン発信のみ
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
      _CHANGE_STATE(_crossScanWait);  // 親機になる
    }
  }
}



static void _crossScanShootWait(GFL_NETWL* pNetWL)
{
  pNetWL->CrossRand--;
  
  if( pNetWL->CrossRand == 0){
//			NET_PRINT("_crossScanShootWait WH_EndScan\n");
    WH_EndScan(); //スキャン終了
    _CHANGE_STATE(_crossScanShootEndWait);  // スキャン終了待ち
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
      _CHANGE_STATE(_crossScanShootWait);  // スキャン待ち
#if DEBUG_WH_BEACON_PRINT_ON
      NET_PRINT( "scan on\n" );
#endif
		}
	}
}

//-------------------------------------------------------------
/**
 * @brief   すれ違い通信開始
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

  GFL_NET_WLChildBconDataInit(); // データの初期化
	{
		WH_SetReceiver(_receiverFunc);
		pNetWL->bSetReceiver = TRUE;
	}

  _CHANGE_STATE(_crossScanShootStart);

  return TRUE;
}

//-------------------------------------------------------------
/**
 * @brief   切断コールバックがあったらエラーにする
 * @param   none
 * @retval  nene
 */
//-------------------------------------------------------------
void GFL_NET_WL_DisconnectError(void)
{
	GFL_NETWL* pNetWL = _pNetWL;

	if(pNetWL->bErrorCheck){
    pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
  }
}



//-------------------------------------------------------------
/**
 * @brief   保持しているビーコンのサービス番号を返す
 * @param   index  ビーコンindex
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
 * @brief   スキャンするビーコンをこのindexのビーコンに絞り込む
 * @param   index  ビーコンindex
 * @retval  GameServiceID
 */
//-------------------------------------------------------------

void GFL_NET_WLFIXScan(int index)
{
  WIH_FixScanMode(_pNetWL->sBssDesc[index].channel, _pNetWL->sBssDesc[index].bssid );
}


//-------------------------------------------------------------
/**
 * @brief   スキャンする速度の変更
 * @param   num   2:はやい(default) 1:おそい  0:とまっている
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
 * @brief   ビーコンは出しているが、一時きてほしく無い場合に操作する
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
//デバッグ用ルーチン
#ifdef PM_DEBUG

//-----------------------------------------------
/**
 *  @brief  接続相手限定コードを通信システムに反映　
 */
//-----------------------------------------------
void DEBUG_GFL_NET_AloneTestCodeReflect( void )
{
	GFL_NETWL* pNetWL = NULL;

	if( _pNetWL == NULL ){
		return; //通信未初期化なので変更しない
	}
	pNetWL = _pNetWL;
  
  pNetWL->mineDebugNo = DebugAloneTest;
}

#endif  //PM_DEBUG


//-------------------------------------------------------------
/**
 * @brief   ビーコンスキャンの一時停止
 * @param   bPause TRUEで停止 FALSEで解除
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

//-------------------------------------------------------------
/**
 * @brief   ビーコン収集関数セット
 */
//-------------------------------------------------------------
void NET_WHPIPE_SetBeaconCatchFunc(PIPESetBeaconCatchFunc func, void* pWork)
{
  if(_pNetWL){
    _pNetWL->beaconCatchFunc = func;
    _pNetWL->beaconCatchFuncWork = pWork;
  }
}
