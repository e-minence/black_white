//============================================================================================
/**
 * @file	game_net.c
 * @brief	通信動作用関数
 */
//============================================================================================
#include "gflib.h"
#include "include\net\network_define.h"
#include "include\system\main.h"

#include "game_net.h"
//------------------------------------------------------------------
// 
//	システム構造体
//
//------------------------------------------------------------------
typedef struct {
	int				_connectSeqNo;
	int				_exitSeqNo;
	GFL_NETHANDLE*	_pHandle;

}GAMENET_SYS;

static GAMENET_SYS	gNetSys;

extern const NetRecvFuncTable _CommPacketTbl[1];
//------------------------------------------------------------------
// 
//	初期化
//
//------------------------------------------------------------------
#define _BCON_GET_NUM  (1)

#define _MAXNUM (2)     // 接続最大
#define _MAXSIZE (120)  // 送信最大サイズ

#define _TEST_TIMING (12)

typedef struct{
    int gameNo;   ///< ゲーム種類
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_FIELDMOVE_SERVICEID };

///< ビーコンデータ取得関数
static void* _netBeaconGetFunc(void)
{
	return &_testBeacon;
}

///< ビーコンデータサイズ取得関数
static int _netBeaconGetSizeFunc(void)
{
	return sizeof(_testBeacon);
}

///< ビーコンデータ比較関数
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
    if(myNo != beaconNo){
        return FALSE;
    }
    return TRUE;
}

static void _initCallback(void* pWork)
{
}

// 通信初期化構造体  wifi用
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // 受信関数テーブル
    1,//NELEMS(_CommPacketTbl), // 受信テーブル要素数
    NULL,   // ワークポインタ
    NULL,   // ユーザー同士が交換するデータのポインタ取得関数
    NULL,   // ユーザー同士が交換するデータのサイズ取得関数
    _netBeaconGetFunc,  // ビーコンデータ取得関数
    _netBeaconGetSizeFunc,  // ビーコンデータサイズ取得関数
    _netBeaconCompFunc,  // ビーコンのサービスを比較して繋いで良いかどうか判断する
    FatalError_Disp,  // 通信不能なエラーが起こった場合呼ばれる 切断するしかない
    NULL,  // 通信切断時に呼ばれる関数
    NULL,  // オート接続で親になった場合
    0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //元になるheapid
    HEAPID_NETWORK,  //通信用にcreateされるHEAPID
    HEAPID_WIFI,  //wifi用にcreateされるHEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,        // 通信アイコンXY位置
    _MAXNUM,     // 最大接続人数
    _MAXSIZE,  //最大送信バイト数
    _BCON_GET_NUM,    // 最大ビーコン収集数
    TRUE,     // CRC計算
    FALSE,     // MP通信＝親子型通信モードかどうか
    FALSE,  //wifi通信を行うかどうか
    TRUE,     // 親が再度初期化した場合、つながらないようにする場合TRUE
    WB_NET_FIELDMOVE_SERVICEID,  //GameServiceID
};

void InitGameNet(void)
{
#ifdef NET_WORK_ON
    GFL_NET_Init(&aGFLNetInit, _initCallback);
#endif
	gNetSys._connectSeqNo = 0;
	gNetSys._exitSeqNo = 0;
	gNetSys._pHandle = NULL;
}


enum{
    _CONNECT_START = 0,
    _CONNECT,
    _CONNECT_CHECK,
    _CONNECT_NEGO,
    _CONNECT_NEGOCHECK,
    _CONNECT_TIMINGSTART,
    _CONNECT_TIMINGCHECK,
};

//--------------------------------------------------------------
/**
 * @brief   接続完了コールバック
 * @param   pCtl    デバッグワーク
 * @retval  none
 */
//--------------------------------------------------------------

static void _connectCallBack(void* pWork)
{
    OS_TPrintf("ネゴシエーション完了\n");
    gNetSys._connectSeqNo = _CONNECT_TIMINGSTART;
}

//------------------------------------------------------------------
// 
//	接続
//
//------------------------------------------------------------------


BOOL ConnectGameNet(void)
{
#ifdef NET_WORK_ON
	BOOL result = FALSE;

    //OS_TPrintf("ConnectGameNet%d\n",gNetSys._connectSeqNo);
	switch( gNetSys._connectSeqNo ){

	case _CONNECT_START:
		if( GFL_NET_IsInit() == TRUE ){
			gNetSys._connectSeqNo = _CONNECT;
		}
		break;

	case _CONNECT:
		GFL_NET_ChangeoverConnect(_connectCallBack); // 自動接続
		gNetSys._connectSeqNo = _CONNECT_NEGO;
		break;

	case _CONNECT_TIMINGSTART:
        gNetSys._pHandle = GFL_NET_HANDLE_GetCurrentHandle();
		GFL_NET_TimingSyncStart(gNetSys._pHandle, _TEST_TIMING);
		gNetSys._connectSeqNo = _CONNECT_TIMINGCHECK;
		break;

	case _CONNECT_TIMINGCHECK:
		if(GFL_NET_IsTimingSync(gNetSys._pHandle,_TEST_TIMING)){
			OS_Printf("タイミング取れた\n");
			result = TRUE;
		}
		break;
	}
	return result;
#else
	return TRUE;
#endif
}

//------------------------------------------------------------------
// 
//	終了
//
//------------------------------------------------------------------
enum{
    _EXIT_START = 0,
    _EXIT_END,
};

BOOL ExitGameNet(void)
{
#ifdef NET_WORK_ON
	BOOL result = FALSE;

	switch( gNetSys._exitSeqNo ){

	case _EXIT_START:
		if( GFL_NET_IsParentMachine() == FALSE ){
			GFL_NET_Exit(NULL);
		}else{
			GFL_NET_SendData(gNetSys._pHandle, GFL_NET_CMD_EXIT_REQ, NULL);
		}
		gNetSys._exitSeqNo = _EXIT_END;
		break;

	case _EXIT_END:
		GFL_NET_Exit(NULL);
		result = TRUE;
		break;
	}
	return result;
#else
	return TRUE;
#endif
}

//------------------------------------------------------------------
// 
//	データ送信
//
//------------------------------------------------------------------
void SendGameNet( int comm, void* commWork )
{
#ifdef NET_WORK_ON
	GFL_NET_SendData( gNetSys._pHandle, comm, commWork );
#endif
}

//------------------------------------------------------------------
// 
//	ネットＩＤ取得　
//
//------------------------------------------------------------------
NetID GetNetID(void)
{
#ifdef NET_WORK_ON
	return GFL_NET_GetNetID( gNetSys._pHandle );
#else
	return 1;		//1orgin
#endif
}



