//============================================================================================
/**
 * @file	test_net.c
 * @brief	通信動作テスト用関数
 * @author	ohno
 * @date	2006.12.19
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "gf_standard.h"
#include "test_beacon.h"
#include "main.h"
#include "fatal_error.h"
#include "net_icondata.h"
#include "textprint.h"


#define _BCON_GET_NUM  (6)

//------------------------------------------------------------------
// NETのテスト
//------------------------------------------------------------------




// ローカル通信コマンドの定義
enum _testCommand_e {
    _TEST_VARIABLE = GFL_NET_CMD_COMMAND_MAX,
    _TEST_GETSIZE,
    _TEST_HUGE,
    _TEST_VARIABLE_HUGE,
};

typedef struct{
    int gameNo;   ///< ゲーム種類
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { 5 };

//-------------------------------------------------デバッグ表示

typedef struct {
	const char*	msg;
	u8 posx;
	u8 posy;
	u8 sizx;
	u8 sizy;
}TESTMODE_PRINTLIST;

static const char	test_index1[] = {"beacon catch"};
static const char	test_index2[] = {"scan mode"};
static const char	test_index3[] = {"point mode"};


enum {
	NUM_TITLE = 0,
	NUM_URL,
};

static const TESTMODE_PRINTLIST _childIndexList[] = {
	{ test_index1, 2, 1, 30, 2 },
	{ test_index2, 2, 22, 30, 1 },
};

static const TESTMODE_PRINTLIST _parentIndexList[] = {
	{ test_index1, 2, 1, 30, 2 },
	{ test_index3, 2, 22, 30, 1 },
};

//------------------------------------------------------------------
/**
 * @brief		２Ｄデータコントロール
 */
//------------------------------------------------------------------
static const char font_path[] = {"src/gfl_graphic/gfl_font.dat"};

#define G2D_BACKGROUND_COL	(0x0000)
#define G2D_FONT_COL		(0x7fff)
#define G2D_FONTSELECT_COL	(0x001f)

static const GFL_TEXT_PRINTPARAM default_param = 
{ NULL, 0, 0, 1, 1, 1, 0, GFL_TEXT_WRITE_16 };

#define TEXT_FRM			(GFL_BG_FRAME3_M)
#define TEXT_FRM_PRI		(0)

//------------------------------------------------------------------
/**
 * @brief		ＢＧ設定
 */
//------------------------------------------------------------------
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static const GFL_BG_BGCNT_HEADER bgCont3 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

static const GFL_BG_BGCNT_HEADER bgCont0 = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};


#define _BACKUP_NUM  (5)   //RSSIの平均を出すためにバックアップ
//------------------------------------------------------------------
/**
 * @brief		内部ワーク構造体
 */
//------------------------------------------------------------------
/// コールバック関数の書式 内部ステート遷移用
typedef void (*_PTRFunc)(SKEL_TEST_BEACON_WORK* pNetHandle);

struct _SKEL_TEST_BEACON_WORK{
    HEAPID heapID;
    BOOL bParent;   ///< 親機モードかどうか
    GFL_TEXT_PRINTPARAM* textParam;
    GFL_BMPWIN*	bmpwin[5];
    GFL_BMPWIN* pBmpwinBeacon[_BCON_GET_NUM];
    u16 rssiBackup[_BCON_GET_NUM][_BACKUP_NUM];
    u16 rssiDir[8];
    int dir;
    int testNo;
    _PTRFunc state;
    int timer;
    GFL_NETHANDLE* _pHandle;
    GFL_NETHANDLE* _pHandleServer;
};

#if defined(DEBUG_ONLY_FOR_ohno)
#define   _CHANGE_STATE(state, time)  _changeStateDebug(pWork ,state, time, __LINE__)
#else
#define   _CHANGE_STATE(state, time)  _changeState(pWork ,state, time)
#endif

//==============================================================================
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//==============================================================================

static void _changeState(SKEL_TEST_BEACON_WORK* pWork,_PTRFunc state, int time)
{
    pWork->state = state;
    pWork->timer = time;
}

//==============================================================================
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//==============================================================================
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(SKEL_TEST_BEACON_WORK* pWork,_PTRFunc state, int time, int line)
{
    NET_PRINT("net_state: %d\n",line);
    _changeState(pWork, state, time);
}
#endif

//==============================================================================
/**
 * @brief   なにもしない
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//==============================================================================
static void _noneFunc(SKEL_TEST_BEACON_WORK* pWork)
{
}

//------------------------------------------------------------------
/**
 * @brief		メッセージビットマップウインドウコントロール
 */
//------------------------------------------------------------------
static void msg_bmpwin_make
			(SKEL_TEST_BEACON_WORK * testmode, u8 bmpwinNum, const char* msg, u8 px, u8 py, u8 sx, u8 sy )
{
	//ビットマップ作成
	testmode->bmpwin[bmpwinNum] = GFL_BMPWIN_Create( TEXT_FRM, px, py, sx, sy, 0, 
														GFL_BG_CHRAREA_GET_B );

	//テキストをビットマップに表示
	testmode->textParam->bmp = GFL_BMPWIN_GetBmp( testmode->bmpwin[ bmpwinNum ] );
	GFL_TEXT_PrintSjisCode( msg, testmode->textParam );

	//ビットマップキャラクターをアップデート
	GFL_BMPWIN_TransVramCharacter( testmode->bmpwin[bmpwinNum] );
	//ビットマップスクリーン作成
	//GFL_BMPWIN_MakeScreen( testmode->bmpwin[bmpwinNum] );
}

//------------------------------------------------------------------
/**
 * @brief		２Ｄデータコントロール
 */
//------------------------------------------------------------------
static void	g2d_load( SKEL_TEST_BEACON_WORK* testmode )
{
	//フォント読み込み
	GFL_TEXT_CreateSystem( font_path );
	//パレット作成＆転送
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( testmode->heapID, 16*2 );
		plt[0] = G2D_BACKGROUND_COL;
		plt[1] = G2D_FONT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 0 );
		plt[1] = G2D_FONTSELECT_COL;
		GFL_BG_LoadPalette( TEXT_FRM, plt, 16*2, 16*2 );

		GFL_HEAP_FreeMemory( plt );
	}
	//文字表示パラメータワーク作成
	{
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocClearMemoryLo
										( testmode->heapID,sizeof(GFL_TEXT_PRINTPARAM));
		*param = default_param;
		testmode->textParam = param;
	}
	//文字表示ビットマップの作成
	{
		int i,num;
        const TESTMODE_PRINTLIST* pList;

        if(testmode->bParent){
            pList = &_parentIndexList[0];
            num = NELEMS(_parentIndexList);
        }
        else{
            pList = &_childIndexList[0];
            num = NELEMS(_childIndexList);
        }
        
		//表題ビットマップの作成
		for(i = 0 ; i < num ; i++,pList++){
			msg_bmpwin_make( testmode, i, pList->msg,
							pList->posx, pList->posy, 
							pList->sizx, pList->sizy );
		}
	}
	//ＢＧスクリーン転送リクエスト発行
	//GFL_BG_LoadScreenReq( TEXT_FRM );
}

//------------------------------------------------------------------
/**
 * @brief		ＢＧ設定＆データ転送
 */
//------------------------------------------------------------------
static void	bg_init( HEAPID heapID )
{
	//ＢＧシステム起動
	GFL_BG_Init( heapID );

	//ＶＲＡＭ設定
	GX_SetBankForTex(GX_VRAM_TEX_01_AB);
	GX_SetBankForBG(GX_VRAM_BG_64_E);
	GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G); 

	//ＢＧモード設定
	GFL_BG_InitBG( &bgsysHeader );

	//ＢＧコントロール設定
	GFL_BG_SetBGControl( TEXT_FRM, &bgCont3, GFL_BG_MODE_TEXT );
	GFL_BG_SetPriority( TEXT_FRM, TEXT_FRM_PRI );
	GFL_BG_SetVisible( TEXT_FRM, VISIBLE_ON );

	//ビットマップウインドウシステムの起動
	GFL_BMPWIN_Init( heapID );

	//ARCシステム初期化
//	GFL_ARC_Init(&GraphicFileTable[0],1);

	//ディスプレイ面の選択
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}


static void* _netBeaconGetFunc(void)    ///< ビーコンデータ取得関数
{
    return &_testBeacon;
}

static int _netBeaconGetSizeFunc(void)    ///< ビーコンデータサイズ取得関数
{
    return sizeof(_testBeacon);
}

static BOOL _netBeaconCompFunc(int myNo,int beaconNo)    ///< ビーコンデータ取得関数
{
    return TRUE;
}


//----------------------------------------------------------------


static u16 _backupRssi(SKEL_TEST_BEACON_WORK* pWork,int index, u16 rssi)
{
    u16 sum = 0;
    int i;

    if((pWork->rssiBackup[index][_BACKUP_NUM-1]==0) ||(pWork->rssiBackup[index][0] != rssi)){
        for(i = _BACKUP_NUM-1; (i-1) >= 0; i--){
            pWork->rssiBackup[index][i] = pWork->rssiBackup[index][i-1];
        }
        pWork->rssiBackup[index][0] = rssi;
    }
    for(i = 0; i < _BACKUP_NUM ; i++){
        sum  += pWork->rssiBackup[index][i];
    }
    if(sum!=0){
        sum /= _BACKUP_NUM;
    }
    return sum;
}


static void _stockRssi(SKEL_TEST_BEACON_WORK* pWork, u16 rssi)
{

    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
        GFL_STD_MemClear(pWork->rssiDir,sizeof(u16)*8);
        pWork->dir=0;
    }
    if(PAD_BUTTON_Y == GFL_UI_KEY_GetTrg()){


    }
    if(PAD_BUTTON_B == GFL_UI_KEY_GetTrg()){
        pWork->rssiDir[pWork->dir] = rssi;
        pWork->dir++;
        if(pWork->dir >= 8){
            pWork->dir=0;
        }
    }

}


//----------------------------------------------------------------


static void _dispBeacon(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;
    
    for(i = 0;i < _BCON_GET_NUM; i++){
        u8* pData = GFL_NET_GetBeaconMacAddress(i);
        
        if(pData){
//            OS_TPrintf("%d: mac %x\n",i,);
            char msg[100];
            u16 rssi = GFL_NET_WL_GetRssi(i);

            rssi = _backupRssi(pWork,i,rssi);

            _stockRssi(pWork,rssi);
            
            GFL_STD_MemClear(msg,100);
            OS_SPrintf(msg,"%d: %3d %3d %3d %3d %3d %3d %3d %3d",rssi,
                       pWork->rssiDir[0],
                       pWork->rssiDir[1],
                       pWork->rssiDir[2],
                       pWork->rssiDir[3],
                       pWork->rssiDir[4],
                       pWork->rssiDir[5],
                       pWork->rssiDir[6],
                       pWork->rssiDir[7]
                       );

            GFL_BMPWIN_ClearScreen(pWork->pBmpwinBeacon[ i ]);
            
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_TEXT_PrintSjisCode( msg, pWork->textParam );

            //ビットマップキャラクターをアップデート
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );
        }
        else{
            pWork->textParam->bmp = GFL_BMPWIN_GetBmp( pWork->pBmpwinBeacon[ i ] );
            GFL_BMP_Clear(pWork->textParam->bmp,0);
            GFL_BMPWIN_TransVramCharacter( pWork->pBmpwinBeacon[i] );
        }
    }
}


static void rssiWindowInit(SKEL_TEST_BEACON_WORK* pWork)
{
    int i;
    
    for(i = 0;i < _BCON_GET_NUM; i++){
        pWork->pBmpwinBeacon[i] = GFL_BMPWIN_Create( TEXT_FRM,
                                                       4, 5+i*2, 24, 1, 0, 
                                                       GFL_BG_CHRAREA_GET_B );
    }
}


static void _testChild(SKEL_TEST_BEACON_WORK* pWork)
{
    u8 beacon;
    int i;

    if(PAD_BUTTON_X == GFL_UI_KEY_GetTrg()){
        for(i = 0;i < _BCON_GET_NUM; i++){
            u8* pData = GFL_NET_GetBeaconMacAddress(i);
            if(pData){
                OS_TPrintf("%d: mac %x%x%x%x%x%x\n",i,pData[0],pData[1],pData[2],pData[3],pData[4],pData[5]);
            }
        }
    }

    bg_init( pWork->heapID );
    //画面作成
    g2d_load(pWork);	//２Ｄデータ作成


    rssiWindowInit(pWork);
        
    pWork->_pHandle = GFL_NET_CreateHandle();
    GFL_NET_StartBeaconScan(pWork->_pHandle );    // ビーコンを待つ
    _CHANGE_STATE(_dispBeacon, 0);
}

//            u8* pData = GFL_NET_GetBeaconMacAddress(0);//ビーコンリストの0番目を得る


static void _testParent(SKEL_TEST_BEACON_WORK* pWork)
{

    bg_init( pWork->heapID );    //画面作成
    g2d_load(pWork);	//２Ｄデータ作成
    // サーバー側のテスト  ビーコン出しているだけ
    pWork->_pHandleServer = GFL_NET_CreateHandle();
    GFL_NET_InitServer(pWork->_pHandleServer);   // サーバ
    pWork->_pHandle = GFL_NET_CreateHandle();  // クライアント
    _CHANGE_STATE(_noneFunc, 0);

}

void TEST_BEACON_Main(SKEL_TEST_BEACON_WORK* pWork)
{

    if(pWork){
        _PTRFunc state = pWork->state;
        if(state != NULL){
            state(pWork);
        }
    }
}



#define SSID "GF_ORG01"

static u8* _netGetSSID(void)
{
    return (u8*)SSID;
}


// 可変サイズ受信
static void _testRecvVariable(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvVariable %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}


static void _testRecvGetSize(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
    const u8* pBuff=pData;
    NET_PRINT("_testRecvGetSize %d:  %d : %d %d\n",__LINE__,size,pBuff[0],pBuff[9]);
}

static int _getTestCommandSize(void)
{
    return 12;
}


// ローカル通信テーブル
static const NetRecvFuncTable _CommPacketTbl[] = {
    // 可変サイズテスト
    {_testRecvVariable,          GFL_NET_COMMAND_VARIABLE, NULL},
    // サイズ取得関数テスト
    {_testRecvGetSize,           _getTestCommandSize, NULL},
};

#define _MAXNUM (2)     // 接続最大
#define _MAXSIZE (120)  // 送信最大サイズ

// 通信初期化構造体  wifi用
static GFLNetInitializeStruct aGFLNetInit = {
    _CommPacketTbl,  // 受信関数テーブル
    NELEMS(_CommPacketTbl), // 受信テーブル要素数
    NULL,   // ワークポインタ
    _netBeaconGetFunc,  // ビーコンデータ取得関数
    _netBeaconGetSizeFunc,  // ビーコンデータサイズ取得関数
    _netBeaconCompFunc,  // ビーコンのサービスを比較して繋いで良いかどうか判断する
    FatalError_Disp,  // 通信不能なエラーが起こった場合呼ばれる 切断するしかない
    NULL,  // 通信切断時に呼ばれる関数
    NULL,  // オート接続で親になった場合
    NET_ICONDATA_GetTableData,   // 通信アイコンのファイルARCテーブルを返す関数
    NET_ICONDATA_GetNoBuff,      // 通信アイコンのファイルARCの番号を返す関数
    _netGetSSID,  // 親子接続時に認証する為のバイト列  
    1,  //gsid
    0,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //元になるheapid
    HEAPID_NETWORK,  //通信用にcreateされるHEAPID
    HEAPID_WIFI,  //wifi用にcreateされるHEAPID
    _MAXNUM,     // 最大接続人数
    _MAXSIZE,  //最大送信バイト数
    _BCON_GET_NUM,    // 最大ビーコン収集数
    FALSE,     // MP通信＝親子型通信モードかどうか
    FALSE,  //wifi通信を行うかどうか
    TRUE,     // 通信を開始するかどうか
};

//----------------------------------------------------------------------------
/**
 *	@brief	初期化処理
 */
//-----------------------------------------------------------------------------

SKEL_TEST_BEACON_WORK* TEST_BEACON_Init(HEAPID heapID)
{

    SKEL_TEST_BEACON_WORK* pWork = GFL_HEAP_AllocMemory(heapID, sizeof(SKEL_TEST_BEACON_WORK));
    GFL_STD_MemClear(pWork, sizeof(SKEL_TEST_BEACON_WORK));
    pWork->heapID = heapID;

    //イクニューモンを使用する前に VRAMDをdisableにする必要があるのだが
    //VRAMDが何に使われていたのかがわからないと、消すことができない
    
    if(GX_VRAM_LCDC_D == GX_GetBankForLCDC()){
        GX_DisableBankForLCDC(); // LCDCにVRAMDが割り当てられてるようなので打ち消す
    }
    else if(GX_VRAM_D & GX_GetBankForBG()){
        GX_DisableBankForBG();
    }
    else if(GX_VRAM_D & GX_GetBankForTex()){
        GX_DisableBankForTex();
    }
    else if(GX_VRAM_D & GX_GetBankForClearImage()){
        GX_DisableBankForClearImage();
    }
    else if(GX_VRAM_D & GX_GetBankForSubOBJ()){
        GX_DisableBankForSubOBJ();
    }

    GX_DisableBankForLCDC(); // LCDCにVRAMDが割り当てられてるようなので打ち消す
    GX_DisableBankForBG();
    GX_DisableBankForTex();
    GX_DisableBankForClearImage();
    GX_DisableBankForSubOBJ();

    GFL_NET_Init(&aGFLNetInit);

    if(PAD_BUTTON_R & GFL_UI_KEY_GetCont()){
        OS_TPrintf("親になった\n");
        _CHANGE_STATE(_testParent,0);
    }
    else{
        OS_TPrintf("子になった\n");
        _CHANGE_STATE(_testChild,0);
    }

    return pWork;
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了処理
 */
//-----------------------------------------------------------------------------

void TEST_BEACON_End(SKEL_TEST_BEACON_WORK* pWork)
{
    GFL_HEAP_FreeMemory(pWork);
}
