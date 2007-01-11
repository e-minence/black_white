//=============================================================================
/**
 * @file	net_wireless.c
 * @brief	DSのデバイスへのアクセスをラップする関数
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
// extern宣言
//==============================================================================


/**
 *  @brief _BEACON_SIZE_FIXには 固定でほしいビーコンパラメータの合計を手で書く
 */
#define _BEACON_SIZE_FIX (3)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)
/**
 *  @brief ビーコン構造体
 */
// WM_SIZE_USER_GAMEINFO  最大 112byte
// _BEACON_SIZE_FIXには 固定でほしいビーコンパラメータの合計を手で書く
typedef struct{
  GameServiceID  		serviceNo;   	///< 通信サービス番号
  u8  		connectNum;    	///< つながっている台数  --> 本親かどうか識別	
  u8        pause;          ///< 接続を禁止したい時に使用する
  u8       aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
} _GF_BSS_DATA_INFO;


//==============================================================================
// 定義
//==============================================================================

/// 拾ったビーコンを保存しておく時間
#define _DEFAULT_TIMEOUT_FRAME (60 * 10)  //60frame * 10sec

#define _PORT_DATA_RETRANSMISSION   (14)    // 切断するまで無限再送を行う  こちらを使用している
#define _PORT_DATA_PARENT           _PORT_DATA_RETRANSMISSION
#define _PORT_DATA_CHILD            _PORT_DATA_RETRANSMISSION

#define _KEEP_CHANNEL_TIME_MAX   (5)

#define _NOT_INIT_BITMAP  (0xffff)   // 接続人数を固定に指定ない場合の値

typedef enum{    // 切断状態
    _DISCONNECT_NONE,
    _DISCONNECT_END,
    _DISCONNECT_SECRET,
    _DISCONNECT_STEALTH,
} _DisconnectState_e;


//管理構造体定義
struct _NET_WL_WORK{
    PTRCommRecvLocalFunc recvCallback; ///< 受信コールバック解決用
    NetBeaconCompFunc beaconCompFunc;  ///< ビーコンの比較を行うコールバック
    NetBeaconGetFunc beaconGetFunc;    ///< ビーコンデータ取得関数
    NetBeaconGetSizeFunc beaconGetSizeFunc;  ///< ビーコンデータサイズ取得関数
    WMBssDesc sBssDesc[SCAN_PARENT_COUNT_MAX];  ///< 親機の情報を記憶している構造体
    u8  backupBssid[GFL_NET_MACHINE_MAX][WM_SIZE_BSSID];   // 今まで接続していた
    u16 bconUnCatchTime[SCAN_PARENT_COUNT_MAX]; ///< 親機のビーコンを拾わなかった時間+データがあるかどうか
    void* _pWHWork;                           ///whライブラリが使用するワークのポインタ
    int heapID;
    u8 bScanCallBack;  ///< 親のスキャンがかかった場合TRUE, いつもはFALSE
  //  u8 regulationNo;   ///< ゲームレギュレーション
    GameServiceID serviceNo;
    u8 maxConnectNum;
    u32 ggid;
    u8 gameInfoBuff[WM_SIZE_USER_GAMEINFO];  //送信するビーコンデータ
    u16 keepChannelNo;
    u16 errCheckBitmap;      ///< このBITMAPが食い違うとエラーになる
    u8 channel;
    u8 keepChannelTime;
    u8 disconnectType;    ///< 切断状況
    u8 bSetReceiver;
    u8 bEndScan;  // endscan
    u8 bErrorState:1;     ///< エラーを引き起こしている場合その状態をもちます
    u8 bErrorDisconnectOther:1;     ///< 誰かが落ちるとエラーになります
    u8 bErrorNoChild:1;  ///< 子機が無い場合エラー扱いするかどうか
    u8 bTGIDChange:1;
    u8 bAutoExit:1;
    u8 bEntry:1;   // 子機の新規参入
//    u8 bStalth:1;
} ;

//==============================================================================
// ワーク
//==============================================================================

///< ワーク構造体のポインタ
////static _COMM_WORK* _pCommMP = NULL;

// 親機になる場合のTGID 構造体に入れていないのは
// 通信ライブラリーを切ったとしてもインクリメントしたいため
/// TGID管理
static u16 _sTgid = 0;


// コンポーネント転送終了の確認用
// イクニューモンコンポーネント処理を移動させるときはこれも移動
static volatile int   startCheck;	

//==============================================================================
// static宣言
//==============================================================================

static void _whInitialize(int heapID,GFL_NETWL* pNetWL);
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
 * @brief   接続クラスのワーク確保
 * @param   heapID   ワーク確保ID
 * @retval  _COMM_WORKのポインタ
 */
//==============================================================================
void* GFL_NET_WLGetHandle(int heapID, GameServiceID serviceNo, u8 num)
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
 * @brief   接続クラスの初期化
 * @param   heapID   ワーク確保ID
 * @retval  _COMM_WORKのポインタ
 */
//==============================================================================

void GFL_NET_WLInitialize(int heapID,NetBeaconGetFunc getFunc,NetBeaconGetSizeFunc getSize)
{
    int i;
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    pNetWL->beaconGetFunc = getFunc;    ///< ビーコンデータ取得関数
    pNetWL->beaconGetSizeFunc = getSize;  ///< ビーコンデータサイズ取得関数
    // 無線ライブラリ駆動開始
    _whInitialize(heapID, pNetWL);
}

//==============================================================================
/**
 * @brief   接続しているかどうか
 * @retval  TRUE  接続している
 */
//==============================================================================

BOOL GFL_NET_WLIsConnect(GFL_NETWL* pNetWL)
{
    if(pNetWL==NULL){
        return FALSE;
    }
    return TRUE;
}

//==============================================================================
/**
 * @brief   子機が親機を探し出した時に呼ばれるコールバック関数
 * 親機を拾うたびに呼ばれる
 * @param   bssdesc   グループ情報
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
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    
    int serviceNo = pNetWL->serviceNo;

    // catchした親データ
    pGF = (_GF_BSS_DATA_INFO*)bssdesc->gameInfo.userGameInfo;
    if(pGF->pause){
        return;  // ポーズ中の親機はBEACON無視
    }
    if(FALSE == pNetWL->beaconCompFunc(serviceNo, pGF->serviceNo)){
        return;   // サービスが異なる場合は拾わない
    }
    
    // このループは同じものなのかどうか検査
    for (i = 0; i < SCAN_PARENT_COUNT_MAX; ++i) {
        if(pNetWL->bconUnCatchTime[i] == 0 ){
            // 親機情報が入っていない場合continue
            continue;
        }
        if (GFL_STD_MemComp(pNetWL->sBssDesc[i].bssid, bssdesc->bssid, WM_SIZE_BSSID)) {
            // もう一度拾った場合にタイマー加算
            pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
            // 新しい親情報を保存しておく。
            MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i], sizeof(WMBssDesc));
            return;
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
        return;
    }
    // 新しい親情報を保存しておく。
    pNetWL->bconUnCatchTime[i] = _DEFAULT_TIMEOUT_FRAME;
    MI_CpuCopy8( bssdesc, &pNetWL->sBssDesc[i],sizeof(WMBssDesc));
    pNetWL->bScanCallBack = TRUE;
}

//------------------------------------------------------------------
/**
 * @brief   無線駆動制御ライブラリの非同期的な処理終了が通知されるコールバック関数。
 * @param   arg		WVR_StartUpAsyncコール時に指定した引数。未使用。
 * @param   result	非同期関数の処理結果。
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
 * @brief   無線駆動制御ライブラリの非同期的な処理終了が通知されるコールバック関数。
 * @param   arg		WVR_StartUpAsyncコール時に指定した引数。未使用。
 * @param   result	非同期関数の処理結果。
 * @retval  none		
 */
//------------------------------------------------------------------
static void _endCallback(void *arg, WVRResult result)
{
    startCheck = 0;
    //sys_SleepOK(SLEEPTYPE_COMM);  // スリープを許可する  @@OO 後で埋め込む
}

//==============================================================================
/**
 * @brief   WVRをVRAMDに移動
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLVRAMDInitialize(void)
{
    int ans;
    //************************************
//	GX_DisableBankForTex();			// テクスチャイメージ

//    sys_SleepNG(SLEEPTYPE_COMM);  // スリープを禁止   @@OO 後で埋め込む
    // 無線ライブラリ駆動開始
	// イクニューモンコンポーネントをVRAM-Dに転送する
    startCheck = 1;
    ans = WVR_StartUpAsync(GX_VRAM_ARM7_128_D, _startUpCallback, NULL);
    if (WVR_RESULT_OPERATING != ans) {
        OS_TPanic("WVR_StartUpAsync failed. %d\n",ans);
    }
    else{
        OHNO_PRINT("WVRStart\n");
    }
}

//==============================================================================
/**
 * @brief   WVRをVRAMDに移動し終わったら1
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_WLIsVRAMDInitialize(void)
{
    return (startCheck==2);
}

//==============================================================================
/**
 * @brief   WVRをVRAMDに移動しはじめたら１
 * @retval  none
 */
//==============================================================================

BOOL GFL_NET_WLIsVRAMDStart(void)
{
    return (startCheck!=0);
}

//==============================================================================
/**
 * @brief   イクニューモン開放
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLVRAMDFinalize(void)
{
    OHNO_PRINT("VRAMD Finalize\n");
    WVR_TerminateAsync(_endCallback,NULL);  // イクニューモン切断
}


//==============================================================================
/**
 * @brief   WHライブラリの初期化
 * @retval  none
 */
//==============================================================================

static void _whInitialize(int heapID, GFL_NETWL* pNetWL)
{
    // 無線初期化
//    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();

    pNetWL->_pWHWork = WH_CreateHandle(heapID, pNetWL->_pWHWork);

    if(FALSE == WH_Initialize(_GFL_NET_WLGetNETWH())){
        OS_TPanic("not init");
    }
    // WH 初期設定
    WH_SetGgid(pNetWL->ggid);
}

//==============================================================================
/**
 * @brief   子機の使用しているデータの初期化
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
 * @brief   親機の使用しているデータの初期化
 * @param   bTGIDChange  新規のゲームの初期化の場合TRUE 古いビーコンでの誤動作を防ぐため用
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
 * @brief   親子共通、通信の初期化をまとめた
 * @retval  初期化に成功したらTRUE
 */
//==============================================================================

static void _commInit(GFL_NETWL* pNetWL)
{
    pNetWL->bScanCallBack = FALSE;
    pNetWL->bErrorState = FALSE;
    pNetWL->bErrorNoChild = FALSE;
    
    pNetWL->disconnectType = _DISCONNECT_NONE;
    pNetWL->bAutoExit = FALSE;
    pNetWL->bEndScan = 0;

    pNetWL->bSetReceiver = FALSE;
    
    return;
}


//==============================================================================
/**
 * @brief   受信コールバック
 * @param   aid    通信のlocalID
 * @param   *data  受信データ
 * @param   size   受信サイズ
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
 * @brief   親機の接続開始を行う
 * @param   bAlloc       allocするかどうか
 * @param   bTGIDChange  TGIDを変更するかどうか
 * @param   bEntry       
 * @param   bTGIDChange  新規のゲームの初期化の場合TRUE 古いビーコンでの誤動作を防ぐため用
 * @param  子機を受け付けるかどうか
 * @retval  初期化に成功したらTRUE
 */
//==============================================================================
BOOL GFL_NET_WLParentInit(BOOL bAlloc, BOOL bTGIDChange, BOOL bEntry, GFL_NET_ConnectionCallBack pConnectFunc )
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    _commInit(pNetWL);
    _parentDataInit(bTGIDChange);

    WH_ParentDataInit();
    
    WHSetConnectCallBack(pConnectFunc);
    
    if(!pNetWL->bSetReceiver){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_CHILD);
      pNetWL->bSetReceiver = TRUE;
    }
    pNetWL->bEntry = bEntry;

    // 電波使用率から最適なチャンネルを取得して接続する。非同期関数
    if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
        if(WH_StartMeasureChannel()){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   子機の接続開始を行う
 * @param   work_area 　システムで使うメモリー領域
 *                      NULLの場合はすでに初期化済みとして扱う
 * @param   serviceNo  ゲームの種類
 * @param   regulationNo  ゲームの種類
 * @param   bBconInit  ビーコンデータを初期化するのかどうか
 * @retval  初期化に成功したらTRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildInit(BOOL bAlloc, BOOL bBconInit)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    _commInit(pNetWL);
    if(bBconInit){
        OHNO_PRINT("ビーコンの初期化\n");
        GFL_NET_WLChildBconDataInit(); // データの初期化
    }
    if(!pNetWL->bSetReceiver ){
        WH_SetReceiver(_receiverFunc, _PORT_DATA_PARENT);
        pNetWL->bSetReceiver = TRUE;
    }
    // 親機検索スタート
    if(WH_GetSystemState() == WH_SYSSTATE_IDLE){
        // MACアドレス指定　　全部FFで全てを探しにいく
        const u8 sAnyParent[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        if(WH_StartScan(_scanCallback, sAnyParent, _SCAN_ALL_CHANNEL)){
            return TRUE;
        }
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   通信切り替えを行う（親子反転に必要な処理）
 * @param   none
 * @retval  リセットしたらTRUE
 */
//==============================================================================

BOOL GFL_NET_WLSwitchParentChild(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(!pNetWL){
        return TRUE;
    }
    switch(pNetWL->bEndScan){
      case 0:
        if(WH_IsSysStateScan()){
            WH_EndScan();
            pNetWL->bEndScan = 1;
            break;
        }
        else if(WH_IsSysStateBusy()){  //しばらく待つ
        }
        else{
            WH_Finalize();
            pNetWL->bEndScan = 2;
        }
        break;
      case 1:
        if(!WH_IsSysStateBusy()){
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
 * @brief   通信切断を行う  ここではあくまで通信終了手続きに入るだけ
 *  ホントに消すのは下の_commEnd
 * @param   none
 * @retval  終了処理に移った場合TRUE
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
 * @brief   通信切断を行う  ただしメモリー開放を行わない
 * @param   切断の場合TRUE
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
        _whInitialize(pNetWL->heapID, pNetWL);  // 無線駆動再開
    }
}

//==============================================================================
/**
 * @brief   通信の全てを消す
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _commEnd(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    GFL_HEAP_FreeMemory(pNetWL->_pWHWork);
    GFL_HEAP_FreeMemory(pNetWL);
    pNetWL = NULL;
}

//==============================================================================
/**
 * @brief   探すことができた親の数を返す
 * @param   none
 * @retval  親機の数
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
 * @brief   BmpListの位置からBconの存在するIndex位置を取得する
 * @param   index		BmpList位置
 * @retval  Index
 */
//--------------------------------------------------------------
int CommBmpListPosBconIndexGet(int index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
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
	GF_ASSERT(0 && "ここには来ないはず");
	return 0;
}

//==============================================================================
/**
 * @brief   親機リストに変化があった場合TRUE
 * @param   none
 * @retval  親機リストに変化があった場合TRUE なければFALSE
 */
//==============================================================================

BOOL GFL_NET_WLIsScanListChange(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return pNetWL->bScanCallBack;
}

//==============================================================================
/**
 * @brief   親機の変化を知らせるフラグをリセットする
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
 * @brief   この親機がいくつとコネクションをもっているのかを得る
 * @param   index   親のリストのindex
 * @retval  コネクション数 0-16
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
 * @brief   接続人数に該当する親を返す
 * @param   none
 * @retval  親のindex
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
 * @brief   すぐに接続していい人が見つかった場合indexを返す
 * @param   none
 * @retval  該当したらindexを返す
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
            if(_isMachBackupMacAddress(&pNetWL->sBssDesc[i].bssid[0])){  // 古いMACに合致
                num = GFL_NET_WLGetParentConnectionNum(i);
                if(( num > 1) && (num < GFL_NET_MACHINE_MAX)){      // 本親に該当した まだ参加可能
                    return i;
                }
            }
        }
    }
    return -1;
}

//==============================================================================
/**
 * @brief   次のレベルで繋いでいい人がいたらそのindexを返します
 * @param   none
 * @retval  該当したらindexを返す
 */
//==============================================================================

int GFL_NET_WLGetNextConnectIndex(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int i;
 
    if(GFL_NET_WLGetParentCount()==0){  // ビーコンが無い状態
        return -1;
    }
    for (i = SCAN_PARENT_COUNT_MAX-1; i >= 0; i--) {
        if(pNetWL->bconUnCatchTime[i] != 0){
            if(_isMachBackupMacAddress(&pNetWL->sBssDesc[i].bssid[0])){  // 古いMACに合致
                OHNO_PRINT("昔の親 %d\n",i);
                return i;
            }
        }
    }
    i = _getParentNum(1);
    if(i != -1 ){
        OHNO_PRINT("履歴なし本親 %d \n", i);
        return i;
    }
    i = _getParentNum(0);
    if(i != -1){
        OHNO_PRINT("履歴なし仮親 %d \n", i);
        return i;
    }
    return i;
}

//==============================================================================
/**
 * @brief   子機　MP状態で接続
 * @param   index   親のリストのindex
 * @retval  子機接続を親機に送ったらTRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildIndexConnect(u16 index)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    int serviceNo;
    
    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        OHNO_PRINT("子機 接続開始 WH_ChildConnect\n");
        serviceNo = pNetWL->serviceNo;
        pNetWL->channel = pNetWL->sBssDesc[index].channel;
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, pNetWL->sBssDesc[index].bssid,0);
//      WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &(pNetWL->sBssDesc[index]));
//        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, pNetWL->sBssDesc[index].bssid,0);
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   子機　MP状態で接続
 * @param   macAddress    マックアドレス
 * @retval  子機接続を親機に送ったらTRUE
 */
//==============================================================================
BOOL GFL_NET_WLChildMacAddressConnect(u8* macAddress)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    
    if (WH_GetSystemState() == WH_SYSSTATE_SCANNING) {
        (void)WH_EndScan();
        return FALSE;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
        NET_PRINT("子機 接続開始\n");
        WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, macAddress, 0);
        return TRUE;
    }
    return FALSE;
}

//==============================================================================
/**
 * @brief   ビーコンデータの定期確認
 *  接続が完了する間での間、この関数を呼び、タイムアウト処理を行う
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
                OHNO_PRINT("親機反応なし %d\n", id);
                pNetWL->bScanCallBack = TRUE;   // データを変更したのでTRUE
            }
        }
    }
}

//==============================================================================
/**
 *  @brief   ユーザ定義の親機情報を設定します。
 *  _GF_BSS_DATA_INFO構造体の中身を送ります
 *  @param   userGameInfo  ユーザ定義の親機情報へのポインタ
 *  @param   length        ユーザ定義の親機情報のサイズ
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
    pGF->serviceNo = pNetWL->serviceNo;  // ゲームの番号
    pGF->pause = WHGetParentConnectPause();    // 親機が受付を中止しているかどうか
    GFL_STD_MemCopy( pNetWL->beaconGetFunc(), pGF->aBeaconDataBuff, size);
    DC_FlushRange(pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
    WH_SetUserGameInfo((u16*)pNetWL->gameInfoBuff, size + _BEACON_SIZE_FIX);
}


//==============================================================================
/**
 *  @brief   ユーザ定義の親機情報の 人数部分を監視し常に更新する
 *  _GF_BSS_DATA_INFO構造体の中身を送ります
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
 * @brief   通信ライブラリー内部の状態を見て、処理をする関数
 * VBlankとは関係ないのでprocessの時に処理すればいい
 * @param   none
 * @retval  none
 */
//==============================================================================
static void _stateProcess(u16 bitmap)
{
    int state = WH_GetSystemState();
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
//    CommInfoFunc();
    _funcBconDataChange();      // ビーコンの中身を書き換え中
    if((WH_GetCurrentAid() == COMM_PARENT_ID) && (!GFL_NET_WLIsChildsConnecting())){
        if(pNetWL->bErrorNoChild){
            pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
//            OHNO_PRINT("エラー中 NOCHILD \n");
        }
    }
    if(pNetWL->errCheckBitmap == _NOT_INIT_BITMAP){
        pNetWL->errCheckBitmap = bitmap;  // このときの接続人数を保持
    }
    if(pNetWL->bErrorDisconnectOther){ // エラー検査を行う
        if(pNetWL->errCheckBitmap > bitmap){  // 切断した場合必ず数字が減る 増える分にはOK
            pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
//            OHNO_PRINT("エラー中 誰か落ちた \n");
        }
    }
    if(WH_ERRCODE_FATAL == WH_GetLastError()){
//        CommFatalErrorFunc(0);   @@OO エラー表示を入れる必要がある 06.12.5
    }
    switch (state) {
      case WH_SYSSTATE_STOP:
        if(pNetWL->disconnectType == _DISCONNECT_END){
            OHNO_SP_PRINT("WHEnd を呼んで終了しました\n");
            _commEnd();  // ワークから何から全て開放
            return;
        }
        if(pNetWL->disconnectType == _DISCONNECT_SECRET){
            OHNO_SP_PRINT("WHEnd を呼んで死んだふり開始\n");
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
        OHNO_PRINT("エラー中 %d \n",WH_GetLastError());
        if(pNetWL){
            pNetWL->bErrorState = TRUE;   ///< エラーを引き起こしている場合その状態をもちます
        }
        break;
      case WH_SYSSTATE_MEASURECHANNEL:
        {
            u16 channel;
            // 利用可能な中から一番使用率の低いチャンネルを返します。
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
//            OHNO_PRINT("親機接続開始   tgid=%d channel=%d \n",_sTgid, channel);
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
 * @brief   通信ライブラリー内部の状態を見て、処理をする関数
 * VBlankとは関係ないのでprocessの時に処理すればいい
 * 子機はお互いの接続がわからないので、通信結果をcommsystemからもらってエラー検査する
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
 * @brief   デバイスが通信可能状態なのかどうかを返す
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
 */
//==============================================================================
BOOL GFL_NET_WL_IsConnectLowDevice(u16 netID)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    if(!pNetWL){
        return FALSE;
    }
    if (WH_GetSystemState() != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }
    return TRUE;
}

//==============================================================================
/**
 * @brief   通信可能状態なのかどうかを返す
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
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
 * @brief   通信可能状態の人数を返す
 * @param   none
 * @retval  接続人数
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
 * @brief  通信切断モードにはいったかどうか
 * @param   none
 * @retval  接続人数
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
 * @brief  初期化しているかどうかを返す
 * @param   none
 * @retval  初期が終わっていたらTRUE
 */
//==============================================================================
BOOL GFL_NET_WLIsInitialize(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return (pNetWL!=NULL);
}

//==============================================================================
/**
 * @brief  WHライブラリで　状態がIDLEになっているか確認する
 * @param   none
 * @retval  IDLEになっている=TRUE
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
 * @brief  WHライブラリで　通信状態のBITを確認  子機がつながっているかどうか
 * @param   none
 * @retval  つながったらTRUE
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
 * @brief  親機が落ちたかどうか
 * @param   none
 * @retval  落ちた場合TRUE
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
 * @brief  エラー状態かどうか
 * @param   none
 * @retval  エラーの時TRUE
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
 * @brief  子機がいないのをエラー扱いにするかどうかをSET
 * @param   bOn  有効時にTRUE
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
 * @brief 誰かが落ちたのをエラー扱いにするかどうかをSET
 * @param   bOn  有効時にTRUE
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
 * @brief   サービス番号に対応したビーコン間隔を得ます
 *          サービス番号は include/communication/comm_def.hにあります
 * @param   serviceNo サービス番号
 * @retval  beacon間隔 msec
 */
//==============================================================================

u16 _getServiceBeaconPeriod(u16 serviceNo)
{
    u16 beaconPeriod = WM_GetDispersionBeaconPeriod();
/*  必要ならコールバック関数として定義する k.ohno 06.12.05
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
 * @brief   ビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   WMBssDesc*  ビーコンバッファポインタ
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
 * @brief   GFビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
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
 * @brief   User定義のビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
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
 * @brief   ビーコンの中にあるマックアドレスを得る
 * @param   index ビーコンバッファに対するindex
 * @return  maccaddressの配列
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
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
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
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
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
 * @brief   macアドレスをバックアップする
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
 * @brief   バックアップしたMACアドレスに該当するかどうかを得る
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
 * @brief   自動切断モードに入ったかどうかを返す
 * @param   none
 * @retval  入っているならばTRUE
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
 * @brief   自動切断モードON
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
 * @brief   ビーコンデータに現在の状況を反映させる
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
 * @brief   ライフタイムを小さくする または元に戻す
 * @param   bMinimum TRUEなら小さくする
 * @retval  none
 */
//==============================================================================

void GFL_NET_WLSetLifeTime(BOOL bMinimum)
{
    WHSetLifeTime(bMinimum);
}

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
    return WHIsParentBeaconSent();
}

//------------------------------------------------------
/**
 * @brief   スキャン状態かどうか
 * @retval  送信完了=TRUE
 */
//------------------------------------------------------

BOOL GFL_NET_WLIsChildStateScan(void)
{
    return WH_IsSysStateScan();
}

//------------------------------------------------------
/**
 * @brief   チャンネルを返す
 * @retval  接続チャンネル
 */
//------------------------------------------------------

int GFL_NET_WLGetChannel(void)
{
    GFL_NETWL* pNetWL = _GFL_NET_GetNETWL();
    return pNetWL->channel;
}

//==============================================================================
/**
 * @brief	自機ビーコン送信内容の取得
 * @param none
 * @return ビーコンの中身
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
 * @brief	ビーコンを拾ってからのカウントダウンタイマ取得
 * @param	index	位置
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
    return WH_SendData(data, size,
                       _PORT_DATA_RETRANSMISSION, callback);
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

//------------------------------------------------------
/**
 * @brief	WHのワークポインタを返す
 * @return  GFL_NETWMのポインタ
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

