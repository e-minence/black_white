#include <nitro.h>
#include <dwc.h>
#include  <cstring>

#include "main.h"
#include "init.h"
#include "utility.h"
#include "mp.h"

#include "dbs.h"



//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define GAME_NAME        "dwctest" // このサンプルが使用するゲーム名
#define INITIAL_CODE     'NTRJ'    // このサンプルが仕様するイニシャルコード
#define GAME_SECRET_KEY  "d4q9GZ"  // このサンプルが使用するシークレットキー
#define GAME_PRODUCTID   10824     // このサンプルが使用するプロダクトID
#define	MAX_PLAYERS		 4	       // 自分も含めた接続人数
#define APP_CONNECTION_KEEPALIVE_TIME 300000 // キープアライブ時間
#define KEEPALIVE_INTERVAL (APP_CONNECTION_KEEPALIVE_TIME/5) // キー入力を待たずデータを転送する時間
#define FILTER_STRING "sample_filter"	// フィルタ文字列. 適宜変更してください
#define FILTER_KEY "str_key"
#define GAME_FRAME       1         // 想定するゲームフレーム（1/60を1とする）
#define GAME_NUM_MATCH_KEYS 3      // マッチメイク用追加キー個数
#define NETCONFIG_USE_HEAP 1
//#define GAME_USE_STORAGE_SERVER   // ストレージサーバへのセーブ・ロードを試すスイッチ
//#define USE_AUTHSERVER_PRODUCTION // 製品向け認証サーバを使用する場合有効にする

#undef DEBUG_OPTION_FILL_HEAP // メモリ確保/開放時にメモリを埋めるデバッグオプション

//----------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------
typedef struct tagRegFriendParam
{
    int step;    // 0:コマンド選択中、1:登録・削除友達情報入力中
    u32 figure;  // 入力中のプロファイルIDの桁
    char  value[20]; // 入力中の値
    //u32 value;   // 入力中の値
} RegFriendParam;

// 通信でやり取りする位置データ構造
static struct Position
{
    s32  xpos;
    s32  ypos;
}
s_position[ MAX_PLAYERS ];  // ローカルで保持する位置データ

// 通信コールバックに渡すアプリケーション固有データサンプル
typedef struct tagCommunicationData
{
    s32 count;
}CommunicationData;
CommunicationData s_commData;


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
static GameControl stGameCnt;  // ゲーム制御情報構造体
KeyControl stKeyCnt;    // キー入力制御構造体

static int stNumEntry  = 2;    // ネットワーク構成人数指定
static int stServerIdx = 0;    // 接続したいサーバDSの友達リストインデックス

static BOOL stCloseFlag = FALSE;    // 自分でクローズした場合はTRUE

// 友達無指定ピアマッチメイク用追加キーデータ構造体
static GameMatchExtKeys stMatchKeys[GAME_NUM_MATCH_KEYS] = { 0, };

// ユーザデータを格納する構造体。
static DWCUserData stUserData;
static DWCInetControl stConnCtrl;

// デバッグ出力のオーバーライド
static BOOL sPrintOverride;

static u8 stNumBackup = 0;  /* バックアップデバイスの種類 */
static u16 card_lock_id;

static u32 loopcnt = 0;
static char loopstr[] = "/|\\-";
static char addFilter[128] = "";
static u8   userData[4]={ 0,5,120,254 };
static u32  s_groupID = 0;
static DWCTopologyType s_topologyType = DWC_TOPOLOGY_TYPE_HYBRID;
static BOOL reliableUse = FALSE;  // リライアブル通信を使用するかどうか
static u64  s_friendkey = 0;      // 自分のフレンドキー

// Wi-Fiコネクション設定の使用言語
static int utilityLangage = DWC_LANGUAGE_JAPANESE;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void InitGameData(void);
static void SetGameMatchKeys(void);

static GameMode GameMain(void);
static GameMode GameRegisterFriendMain(void);
static GameMode GameLogonMain(void);
static GameMode GameMatchMain(void);
static GameMode GameConnectedMain(void);
static GameMode NetConfigLanguageMain(void);
static GameMode NetConfigMain(void);
static GameMode StartIPMain(void);
static GameMode StopIPMain(void);
static GameMode BackupUserdataMain(void);
static GameMode SetTopologyMain(void);

int GetAvailableFriendListIndex(void);

static void LoginCallback(DWCError error, int profileID, void* param);

static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param);
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param);
static void BuddyFriendCallback(int index, void* param);

static void LoginToStorageCallback(DWCError error, void* param);
static void SaveToServerCallback(BOOL success, BOOL isPublic, void* param);
static void LoadFromServerCallback(BOOL success, int index, char* data, int len, void* param);

static void ConnectToAnybodyCallback(DWCError error, 
                                     BOOL cancel, 
                                     BOOL self,
                                     BOOL isServer,
                                     int  index,
                                     void* param);
static void ConnectToFriendsCallback(DWCError error, 
                                     BOOL cancel, 
                                     BOOL self,
                                     BOOL isServer,
                                     int  index,
                                     void* param);
static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int index,
                                    void* param);
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int index,
                                        void* param);
static void ConnectionClosedCallback(DWCError error,
                                     BOOL isLocal,
                                     BOOL isServer,
                                     u8 aid,
                                     int index,
                                     void* param);
static void NewClientCallback(int index, void* param);
static void GenericNewClientCallback(int index, void* param);
static int  EvaluateAnybodyCallback(int index, void* param);
static int  EvaluateFriendCallback(int index, void* param);

static DWCError HandleDWCError(GameMode* gameMode);

static void DispErrorMessage(DWCError error, int errorCode, DWCErrorType errorType);
static void DispMenuMsgWithCursor(const GameSequence* gameSeq, int index, void* param);
static void DispFriendList(BOOL overLoad);
static void DispFriendListData(void);

static void RegFriendModeDispCallback(int curIdx, void* param);
static void LogonModeDispCallback(int curIdx, void* param);
static void MatchModeDispCallback(int curIdx, void* param);
static void BackupModeDispCallback(int curIdx, void* param);

static void SendCallback( int size, u8 aid, void* param );
static void RecvCallback( u8 aid, u8* buffer, int size, void* param );
static void SuspendCallback( DWCSuspendResult result, BOOL suspend, void* data );

static void GameWaitVBlankIntr(void);
static void VBlankIntr(void);

static DWCFriendData* DTUD_GetFriendList( void );
static const char* GetTopologyToString(void);


//----------------------------------------------------------------------------
// initialized variable
//----------------------------------------------------------------------------
// ゲームシーケンスリスト
static GameSequence gameSeqList[GAME_MODE_NUM] =
{
    {
        "GAME MAIN MODE",
        GameMain,
        NULL,
        6,
        {
            "Login", GAME_MODE_LOGIN, TRUE,
            "Register friend", GAME_MODE_REG_FRIEND, FALSE,
            "NetConfig", GAME_MODE_NETCONFIG_LANGUAGE, FALSE,
            "StartIP", GAME_MODE_STARTIP, FALSE,
            "StopIP", GAME_MODE_STOPIP, FALSE,
            "UserData Backup", GAME_MODE_BACKUP, FALSE,
        },
    },
    {
        "REGISTER FRIEND MODE",
        GameRegisterFriendMain,
        RegFriendModeDispCallback,
        4,
        {
            "Set(Manual)", GAME_MODE_NUM, FALSE,
            "Delete", GAME_MODE_NUM, FALSE,
            "Set(MP)", GAME_MODE_NUM, FALSE,
            "Return", GAME_MODE_MAIN, FALSE,
        },
    },
    {
        "GAME LOGON MODE",
        GameLogonMain,
        LogonModeDispCallback,
        7,
        {
            "Connect to anybody", GAME_MODE_CONNECTED, TRUE,
            "Connect to friends", GAME_MODE_CONNECTED, TRUE,
            "Setup game server", GAME_MODE_CONNECTED, TRUE,
            "Connect to game server", GAME_MODE_CONNECTED, TRUE,
            "GroupID reconnect", GAME_MODE_CONNECTED, TRUE,
            "Set Topology type", GAME_MODE_TOPOLOGY, FALSE,
            "Logout", GAME_MODE_MAIN, FALSE
        },
    },
    {
        "GAME CONNECTED MODE",
        GameConnectedMain,
        NULL,
        5,
        {
            "DWC_SendReliable", GAME_MODE_NUM, FALSE,
            "DWC_SendUnreliable", GAME_MODE_NUM, FALSE,   
            "Suspend start(TRUE)", GAME_MODE_NUM, FALSE,
            "Suspend start(FALSE)", GAME_MODE_NUM, FALSE,
            "Close connections", GAME_MODE_LOGIN, FALSE
        },
    },
    {
        "LANGUAGE",
        NetConfigLanguageMain,
        NULL,
        8,
        {
            "Japanese", GAME_MODE_NETCONFIG, FALSE,
            "English", GAME_MODE_NETCONFIG, FALSE,
            "French", GAME_MODE_NETCONFIG, FALSE,
            "German", GAME_MODE_NETCONFIG, FALSE,
            "Italian", GAME_MODE_NETCONFIG, FALSE,
            "Spanish", GAME_MODE_NETCONFIG, FALSE,
            "Hangul", GAME_MODE_NETCONFIG, FALSE,
            "Return", GAME_MODE_MAIN, FALSE
        },
    },
    {
        "NET CONFIG GUI",
        NetConfigMain,        NULL,        0,
    },
    {
        "START TO CONNECT INTERNET",
        StartIPMain,        NULL,        0,
    },
    {
        "STOP TO CONNECT INTERNET",
        StopIPMain,        NULL,        0,
    },
    {
        "BACKUP USERDATA",
        BackupUserdataMain,
        BackupModeDispCallback,
        4,
        {
            "Load UserData", GAME_MODE_NUM, FALSE,
            "Save UserData", GAME_MODE_NUM, FALSE,
            "Report UserData", GAME_MODE_NUM, FALSE,
            "return", GAME_MODE_MAIN, FALSE,
        }
    },
    {
        "SET TOPOLOGY TYPE",
        SetTopologyMain,
        NULL,
        4,
        {
            "Hybrid Mode", GAME_MODE_NUM, FALSE,
            "Fullmesh Mode", GAME_MODE_NUM, FALSE,
            "Star Mode", GAME_MODE_NUM, FALSE,
            "return", GAME_MODE_LOGIN, FALSE,
        },
    },
};


// サンプルではセーブできないのでこれをセーブデータの代わりとみなす
static GameSaveData saveData =
{
    {
        "Player1", "Player2", "Player3", "Player4"
    },
    0, FALSE,
    {
        0,
    }
};

// GameSpy Statusデータとしてセットするサンプルデータ
static const char gsStatusSample[16] = { 10, 9, 8, 0, 0, 1, 2, 3, };

static const struct
{
    CARDBackupType type;
    const char *comment;
} stBackupTypeTable[] = {
    { CARD_BACKUP_TYPE_EEPROM_4KBITS,   "EEPROM    4 kb" },
    { CARD_BACKUP_TYPE_EEPROM_64KBITS,  "EEPROM   64 kb" },
    { CARD_BACKUP_TYPE_EEPROM_512KBITS, "EEPROM  512 kb" },
    { CARD_BACKUP_TYPE_FLASH_2MBITS,    "FLASH     2 Mb" },
    { CARD_BACKUP_TYPE_FLASH_4MBITS,    "FLASH     4 Mb" },
} ;
enum { BACKUP_TABLE_MAX = sizeof(stBackupTypeTable) / sizeof(*stBackupTypeTable) } ;


/*---------------------------------------------------------------------------*
  メモリ確保関数
 *---------------------------------------------------------------------------*/
static void*
AllocFunc( DWCAllocType name, u32   size, int align )
{
    void * ptr;
    OSIntrMode old;
    (void)name;
    (void)align;

    old = OS_DisableInterrupts();

    ptr = OS_AllocFromMain( size );

#ifdef DEBUG_OPTION_FILL_HEAP
    MI_CpuFill8( ptr, 0xac, size );
#endif

    //OS_TPrintf( "alloc: ptr=0x%08X size=%d\n", ptr, size );

    OS_RestoreInterrupts( old );

    return ptr;
}

/*---------------------------------------------------------------------------*
  メモリ開放関数
 *---------------------------------------------------------------------------*/
static void
FreeFunc( DWCAllocType name, void* ptr,  u32 size  )
{
    OSIntrMode old;
    (void)name;
    (void)size;

    if ( !ptr ) return;

    old = OS_DisableInterrupts();

#ifdef DEBUG_OPTION_FILL_HEAP
    MI_CpuFill8( ptr, 0xfe, size );
#endif

    OS_FreeToMain( ptr );

    //OS_TPrintf( "free : ptr=0x%08X size=%d\n", ptr, size );

    OS_RestoreInterrupts( old );
}

/** --------------------------------------------------------------------
  OS_TPrintf()のコンソール出力対応
  ----------------------------------------------------------------------*/
#ifdef SDK_FINALROM
#undef OS_TVPrintf
#undef OS_TPrintf
#endif
extern void OS_TVPrintf(const char *fmt, va_list vlist );
static char common_buffer[256];        // thread unsafe, but less use of stack

void OS_TVPrintf(const char *fmt, va_list vlist )
{
    (void)OS_VSNPrintf(common_buffer, sizeof(common_buffer), fmt, vlist);
#ifndef SDK_FINALROM
    OS_PutString(common_buffer);
#endif

    if ( sPrintOverride )
    {
        dbs_CVPrintf( NULL, fmt, vlist );
    }
}

extern void OS_TPrintf(const char *fmt, ...);
void OS_TPrintf(const char *fmt, ...)
{
    va_list vlist;

    va_start(vlist, fmt);
    OS_TVPrintf(fmt, vlist);
    va_end(vlist);
}

/*---------------------------------------------------------------------------*
  メインルーチン
 *---------------------------------------------------------------------------*/
void NitroMain ()
{
    int friendIdx = 0;
    int ret;

	OS_Init();
    CARD_Init();
    ret = OS_GetLockID();
    if (ret == OS_LOCK_ID_ERROR)
    {
        OS_Panic("demo fatal error! OS_GetLockID() failed");
    }
    card_lock_id = (u16)ret;

    // スタック溢れチェック初期設定
    OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);
    
    Heap_Init();
    
    // ヒープ使用量表示ON
    //Heap_SetDebug( TRUE );

    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    // Vブランク割り込み許可
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
    (void)GX_VBlankIntr(TRUE);
	GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

    FS_Init( MI_DMA_MAX_NUM ); 

    // 出力初期化
    dbs_DemoInit();
    sPrintOverride = TRUE; // OS_TPrintf()の出力をconsoleにつなぐ.

    // 表示開始
    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();

    // キー入力初期化
    InitKeyData(&stKeyCnt);

    InitGameData();

    // デバッグ用にコンソールの情報を削除する。
    //DWC_Debug_DWCInitError( s_Work, DWC_INIT_RESULT_DESTROY_OTHER_SETTING );

    // デバッグ表示レベル指定
    DWC_SetReportLevel((unsigned long)(DWC_REPORTFLAG_ALL));

    // DWCライブラリ初期化
#if defined( USE_AUTHSERVER_PRODUCTION )
    ret = DWC_InitForProduction( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#else
    ret = DWC_InitForDevelopment( GAME_NAME, INITIAL_CODE, AllocFunc, FreeFunc );
#endif
    
    OS_TPrintf( "DWC_InitFor*() result = %d\n", ret );

    if ( ret == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        OS_TPrintf( "Wi-Fi setting might be broken.\n" );
    }
    
    // ユーザデータ作成
    DWC_CreateUserData( &stUserData );

    // ユーザデータ表示
    DWC_ReportUserData( &stUserData );
    
    // メインループ
    while (1){
        // メニュー表示
        DispMenuMsgWithCursor(&gameSeqList[stGameCnt.mode], 0, NULL);

        // シーケンスに対応するループ関数を呼び出す
        stGameCnt.mode = gameSeqList[stGameCnt.mode].seqFunc();
    }

	OS_Terminate();
}


/*---------------------------------------------------------------------------*
  ゲームデータとホスト情報初期化関数
 *---------------------------------------------------------------------------*/
static void InitGameData(void)
{

    // ゲーム制御構造体初期化
    stGameCnt.mode     = GAME_MODE_MAIN;
    stGameCnt.blocking = FALSE;
    MI_CpuClear8(stGameCnt.friendStatus, sizeof(stGameCnt.friendStatus));

    // セーブデータからプレイヤーデータをロードしたと想定。
    // とりあえずどの本体でもインデックス0のデータを使う。
    stGameCnt.userData.playerIdx = 0;
    MI_CpuCopy32(saveData.playerData[stGameCnt.userData.playerIdx].playerName,
                 stGameCnt.userData.playerName, sizeof(stGameCnt.userData.playerName));
    stGameCnt.userData.profileID = saveData.profileID;
    stGameCnt.userData.isValidProfile = saveData.isValidProfile;
    MI_CpuCopy32(&saveData.friendList, &stGameCnt.friendList, sizeof(GameFriendList));
}


/*---------------------------------------------------------------------------*
  ゲーム定義のマッチメイク用追加キーセット関数
 *---------------------------------------------------------------------------*/
static void SetGameMatchKeys(void)
{
    int ranking = (int)((OS_GetTick()*1592653589UL+453816691UL) & 0xff);

    // ランキングを整数キーとして設定
    stMatchKeys[0].isStr  = 0;
    strcpy(stMatchKeys[0].keyStr, "game_rank");
    stMatchKeys[0].ivalue = ranking;
    stMatchKeys[0].keyID  =
        DWC_AddMatchKeyInt(stMatchKeys[0].keyID,
                           stMatchKeys[0].keyStr,
                           &stMatchKeys[0].ivalue);
    if (stMatchKeys[0].keyID == 0) OS_TPrintf("AddMatchKey failed 0.\n");

    // マッチメイクに用いる評価タイプを文字列キーとして設定
    stMatchKeys[1].isStr  = 1;
    strcpy(stMatchKeys[1].keyStr, FILTER_KEY);
    strcpy(stMatchKeys[1].svalue, FILTER_STRING);
    stMatchKeys[1].keyID  =
        DWC_AddMatchKeyString(stMatchKeys[1].keyID,
                              stMatchKeys[1].keyStr,
                              stMatchKeys[1].svalue);
    if (stMatchKeys[1].keyID == 0) OS_TPrintf("AddMatchKey failed 1.\n");

    // とりあえずもう一つぐらい整数キーを設定
    stMatchKeys[2].isStr  = 0;
    strcpy(stMatchKeys[2].keyStr, "game_manner");
    stMatchKeys[2].ivalue = ranking%11;
    stMatchKeys[2].keyID  =
        DWC_AddMatchKeyInt(stMatchKeys[2].keyID,
                           stMatchKeys[2].keyStr,
                           &stMatchKeys[2].ivalue);
    if (stMatchKeys[2].keyID == 0) OS_TPrintf("AddMatchKey failed 2.\n");
}


/*---------------------------------------------------------------------------*
  オフライン時メイン関数
 *---------------------------------------------------------------------------*/
static GameMode GameMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_MAIN];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;

#if 0
// サンプルのMP交換用友達情報表示テスト
{
    DWCFriendData friendData;
    int i;
    
    DWC_CreateExchangeToken(&stUserData, &friendData);
    for (i = 0; i < 12; i++){
        OS_TPrintf("0x%02x, ", ((u8 *)&friendData)[i]);
    }
    OS_TPrintf("\n");
}
#endif


    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",   loopstr[((loopcnt)>>2)&3] );
        dbs_Print( 20, 2, "g:%d", s_groupID );

		loopcnt++;
		ReadKeyData();  // キーデータ取得

        DWC_ProcessFriendsMatch();  // FriendsMatch通信処理更新

        HandleDWCError(&returnSeq);  // エラー処理

        if (stGameCnt.blocking){
            // DWC処理中はキー操作を禁止する
            // Vブランク待ち処理
            GameWaitVBlankIntr();
            continue;
        }

        // 次に進むべきモードがセットされていたら認証完了
        if (returnSeq == GAME_MODE_LOGIN){
            // ログインが選択されていた場合
            if (stGameCnt.userData.isValidProfile){
                // 認証成功でプロファイルIDを取得できた場合はループを抜ける
                break;
            }
            else {
                // 認証失敗の場合はモードを進めないようにする
                returnSeq = GAME_MODE_NUM;
            }
        }
        else if (returnSeq != GAME_MODE_NUM){
            // それ以外の場合は必ず抜ける
            break;
        }
            
        ////////// 以下キー操作処理
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // Aボタンでメニュー決定
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

            switch (returnSeq){
            case GAME_MODE_LOGIN:  // ログイン
                if (DWC_CheckHasProfile(&stUserData))
                {
                    OS_TPrintf("DWC_CheckHasProfile:TRUE\n");
                    if (DWC_CheckValidConsole(&stUserData))
                    {
                        OS_TPrintf("DWC_CheckValidConsole:TRUE\n");
                    }
                    else
                    {
                        OS_TPrintf("DWC_CheckValidConsole:FALSE\n");
                        // このままログイン処理に進めても認証エラーになるため、
                        // 通常はこのまま先に進めてはいけません。
                    }
                }
                else
                {
                    OS_TPrintf("DWC_CheckHasProfile:FALSE\n");
                }

                // FriendsMatchライブラリ初期化
                // ライブラリが使うソケットの送受信バッファサイズは
                // デフォルトのまま（8KByte）にしておく
                DWC_InitFriendsMatch(&stUserData, GAME_PRODUCTID,
                                     GAME_SECRET_KEY, 0, 0,
                                     stGameCnt.friendList.keyList, GAME_MAX_FRIEND_LIST);

                // 認証用関数を使ってログイン
                if (!DWC_LoginAsync(L"_INGAMESN_", NULL, LoginCallback, &returnSeq)){
                    // ログインを開始できなかった場合はブロッキング状態にしない。
                    // エラー発生中か二重ログインでなければFALSEにはならない。
                    OS_TPrintf("Can't call DWC_LoginAsync().\n");
                    returnSeq          = GAME_MODE_NUM;
                    stGameCnt.blocking = 0;
                }
                break;
            default:
                break;
            }
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            // 十字キー上でカーソル移動
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu-1;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // 十字キー下でカーソル移動
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if ( stKeyCnt.trg & PAD_BUTTON_Y){
            Heap_Dump();
        }
        ////////// キー操作処理ここまで

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    return returnSeq;
}


/*---------------------------------------------------------------------------*
  友達登録メイン関数
 *---------------------------------------------------------------------------*/
static GameMode GameRegisterFriendMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_REG_FRIEND];
    GameMode returnSeq = GAME_MODE_NUM;
    RegFriendParam cntParam;
    int curIdx = 0;
    int maxFigure;
    int validFlg;
    char* stFriendKey = cntParam.value;

    cntParam.step   = 0;
    cntParam.figure = 0;
    
    while (1){
		{
			char friendKeyString[DWC_FRIENDKEY_STRING_BUFSIZE];
			int i, x;
			DWC_FriendKeyToString(friendKeyString, s_friendkey);
	        dbs_Print( 0, 0, "friendkey:xxxx-xxxx-xxxx");
	        x = 10;
	        for(i = 0; i < 12; i++) {
				dbs_Print(x, 0, "%c", friendKeyString[i]);
				x++;
				if(i == 3 || i == 7) {
			        dbs_Print(x, 0, "-");
					x++;
				}
			}
		}

        ReadKeyData();  // キーデータ取得

        // 次に進むべきモードがセットされていたら抜ける
        if (returnSeq != GAME_MODE_NUM){
            break;
        }
            
        ////////// 以下キー操作処理
        if (cntParam.step == 0){
            // コマンド選択中
            if (stKeyCnt.trg & PAD_BUTTON_A){
                // Aボタンでメニュー決定
                returnSeq = gameSeq->menuList[curIdx].mode;
                stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

                if ((curIdx == 0) || (curIdx == 1)){
                    // 新規入力の場合、登録空きがあるかどうかのチェックが必要
                    
                    // 友達のプロファイルID登録（今は生のプロファイルID）、
                    // もしくはインデックスを指定して友達情報を削除
                    cntParam.step   = 1;
                    cntParam.figure = 0;
                    MI_CpuFill8( stFriendKey, '0', sizeof(cntParam.value));
                    
                    // メニューリスト再表示
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);

                    if (curIdx == 0){
                        maxFigure = 12;  // フレンドキーは12桁
                    }
                    else {
                        maxFigure = 2;   // 友達リストインデックスは最高２桁
                    }
                }
                else if(curIdx == 2)
                {
					// MP通信による友達情報交換
					OS_TPrintf( "MP\n" );

			        // 無線初期化
			        DWC_CleanupInet();
					
					{
						// サンプルMP交換用友達データ
						static DWCFriendData stSampleFriendData ATTRIBUTE_ALIGN(32);
						//データ通信用バッファ
						static u16* gSendBuf ATTRIBUTE_ALIGN(32);//送信用バッファ
					    DWCUserData*   s_UserData;
					    DWCFriendData* s_FriendList;
						int friendIdx;
						int i, j;

				        s_UserData = &stUserData;
				        s_FriendList = DTUD_GetFriendList();

				        // MP通信用データ作成
				        DWC_CreateExchangeToken(s_UserData, &stSampleFriendData);
				        gSendBuf = (u16*)(&stSampleFriendData);
				        
				        mp_match(gSendBuf, s_FriendList);//MP通信

				        friendIdx = GetAvailableFriendListIndex();// 同一のMPデータを省く
				        for(i = 0; i < friendIdx; i++){
				            for(j = i+1; j < friendIdx; j++){
				                if(DWC_IsEqualFriendData( (const DWCFriendData*)&s_FriendList[i], (const DWCFriendData*)&s_FriendList[j])){
				                    MI_CpuClear8(&s_FriendList[j], sizeof(DWCFriendData));
				                }
				            }
				        }

					}

                    // メニューリスト再表示
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
                }
            }
            else if (stKeyCnt.trg & PAD_KEY_UP){
                // 十字キー上でカーソル移動
                curIdx--;
                if (curIdx < 0) curIdx = gameSeq->numMenu-1;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_DOWN){
                // 十字キー下でカーソル移動
                curIdx++;
                if (curIdx >= gameSeq->numMenu) curIdx = 0;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
        }
        else {
            // 数値入力中
            if (stKeyCnt.trg & PAD_BUTTON_A){
                // Aボタンで友達の登録・削除を行う
                if (curIdx == 0){
                    // 友達登録
                    if( DWC_CheckFriendKey( &stUserData, DWC_StringToFriendKey( stFriendKey )))
                    {
                        DWCFriendData* s_FriendList;
                        int friendIdx = -1;
                      
                        // 友達登録鍵による友達登録
                        friendIdx = GetAvailableFriendListIndex();
                        s_FriendList = DTUD_GetFriendList();
                        DWC_CreateFriendKeyToken( &s_FriendList[friendIdx], DWC_StringToFriendKey( stFriendKey ) );
                        validFlg = 1;
                    }
                    else
                    {
                        OS_TPrintf( "Invalid Friend Code!\n" );
                        validFlg = 0;
                    }
                    //DTUD_DispFriendList();
                }
                else {
                    // 友達削除
                    int friendIdx = (int)((stFriendKey[0]-48)*10+(stFriendKey[1]-48));
                    if ((friendIdx < GAME_MAX_FRIEND_LIST) &&
                        DWC_CanChangeFriendList()){
                        // 友達リストから友達を削除
                        DWC_DeleteBuddyFriendData(&stGameCnt.friendList.keyList[friendIdx]);
                        validFlg = 1;
                    }
                    else {
                        validFlg = 0;  // 友達リスト範囲外
                    }
                }

                if (validFlg){
                    // 有効な入力であればコマンド選択に戻る
                    cntParam.step = 0;
                    // メニューリスト再表示
                    DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
                }
            }
            else if (stKeyCnt.trg & PAD_BUTTON_B){
                // Bボタンでコマンド選択に戻る
                cntParam.step = 0;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_UP){
                // 十字キー上下で選択中の数値の増減
                stFriendKey[cntParam.figure]++;
                if(stFriendKey[cntParam.figure] > '9')
                {
                    stFriendKey[cntParam.figure] = '0';
                }
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_DOWN){
                // 十字キー上下で選択中の数値の増減
                stFriendKey[cntParam.figure]--;
                if(stFriendKey[cntParam.figure] < '0')
                {
                    stFriendKey[cntParam.figure] = '9';
                }
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_LEFT){
                // 十字キー左右で選択桁数の移動
                //if (cntParam.figure < maxFigure-1) cntParam.figure++;
                if (cntParam.figure > 0 && cntParam.figure <= maxFigure-1 ) cntParam.figure--;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
            else if (stKeyCnt.trg & PAD_KEY_RIGHT){
                // 十字キー左右で選択桁数の移動
                //if (cntParam.figure > 0) cntParam.figure--;
                if (cntParam.figure >= 0 && cntParam.figure < maxFigure-1 ) cntParam.figure++;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, &cntParam);
            }
        }
        ////////// キー操作処理ここまで

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    return returnSeq;
}


/*---------------------------------------------------------------------------*
  ログイン後メイン関数
 *---------------------------------------------------------------------------*/
static GameMode GameLogonMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_LOGIN];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;
    BOOL boolRet;

    stNumEntry  = 2;
    stServerIdx = 0;

#if 0
// サンプルの友達登録鍵表示テスト
{
    char tmpstr[DWC_FRIENDKEY_STRING_BUFSIZE];
    
    DWC_FriendKeyToString(tmpstr, DWC_CreateFriendKey(&stUserData));
    OS_TPrintf("friendKeyToken '%s'\n", tmpstr);
}
#endif

#ifdef GAME_USE_STORAGE_SERVER
// GameSpyストレージサーバへのセーブ・ロードを行なうテストのためのログイン処理
{
    if (!DWC_LoginToStorageServerAsync(LoginToStorageCallback, NULL)){
        OS_TPrintf("DWC_LoginToStorageServerAsync() failed.\n");
    }
}
#endif

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",   loopstr[((loopcnt)>>2)&3] );
        dbs_Print( 0,2, "AID:%d(0x%08X)", DWC_GetMyAID(), DWC_GetAIDBitmap() );
        dbs_Print( 20, 2, "g:%d", s_groupID );
		loopcnt++;

        ReadKeyData();  // キーデータ取得

        DWC_ProcessFriendsMatch();  // DWC通信処理更新

        HandleDWCError(&returnSeq);  // エラー処理

        if (stGameCnt.blocking){
            // DWC処理中はキー操作を禁止する（キャンセルのみ可）
            if (stKeyCnt.trg & PAD_BUTTON_B){
                // Bボタンでマッチメイクをキャンセルする
                DWC_CloseAllConnectionsHard();
                stGameCnt.blocking = FALSE;
                returnSeq = GAME_MODE_LOGIN;
            }

            // Vブランク待ち処理
            GameWaitVBlankIntr();
            continue;
        }

        // 次に進むべきモードがセットされていたらループを抜ける
        if (returnSeq != GAME_MODE_NUM) break;

        ////////// 以下キー操作処理
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // Aボタンでメニュー決定
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;
            boolRet   = TRUE;

            switch (curIdx){
            case 0:  // 友達無指定ピアマッチメイク呼び出し
                if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                    // Xボタンが押されていなければ、マッチメイク用追加キーデータ設定
                    SetGameMatchKeys();
                }
                
                boolRet =
                    DWC_ConnectToAnybodyAsync(s_topologyType,
                                              (u8)stNumEntry,
                                              addFilter,
                                              ConnectToAnybodyCallback,
                                              &returnSeq,
                                              GenericNewClientCallback,
                                              NULL,
                                              EvaluateAnybodyCallback,
                                              NULL,
                                              NULL,
                                              userData,
                                              NULL);
                break;
            case 1:  // 友達指定ピアマッチメイク呼び出し
                if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                    // Xボタンが押されていなければ、マッチメイク用追加キーデータ設定
                    SetGameMatchKeys();
                }

                // 引数friendIdxListにNULLを指定すると
                // 友達リスト内の全員から検索する
                boolRet = 
                    DWC_ConnectToFriendsAsync(s_topologyType,
                                              NULL,
                                              0,
                                              (u8)stNumEntry,
                                              ConnectToFriendsCallback,
                                              &returnSeq,
                                              GenericNewClientCallback,
                                              NULL,
                                              EvaluateFriendCallback,
                                              NULL,
                                              NULL,
                                              userData,
                                              NULL);
                break;
            case 2:  // サーバDSとしてサーバクライアントマッチメイク開始
                boolRet =
                    DWC_SetupGameServer(s_topologyType,
                                        (u8)stNumEntry,
                                        SetupGameServerCallback,
                                        &returnSeq,
                                        NewClientCallback,
                                        NULL,
                                        NULL,
                                        userData,
                                        NULL);
                break;
            case 3:  // クライアントDSとしてサーバクライアントマッチメイク開始
                boolRet =
                    DWC_ConnectToGameServerAsync(s_topologyType,
                                                 stServerIdx,
                                                 ConnectToGameServerCallback,
                                                 &returnSeq,
                                                 NewClientCallback,
                                                 NULL,
                                                 NULL,
                                                 userData,
                                                 NULL);
                break;
            case 4:  // グループID で再接続を試みる
                if(s_groupID)
                {
                     boolRet =
                     DWC_ConnectToGameServerByGroupID( s_topologyType,	// 接続形態
                                                       s_groupID,		// グループID
                                                       ConnectToGameServerCallback,
                                                       &returnSeq,
                                                       NewClientCallback,
                                                       NULL,
                                                       NULL,
                                                       userData,
                                                       NULL );
                }
                else
                {
                    boolRet = FALSE;
                }
                break;
                
            case 6:  // ログアウト
                DWC_ShutdownFriendsMatch();   // DWC FriendsMatchライブラリ終了
                break;
            default:
                break;
            }

            if (!boolRet){
                // マッチメイクを開始できなかった場合はブロッキング状態にしない。
                // エラー発生中かログイン前、接続後、二重呼び出し時しか
                // FALSEにはならない
                OS_TPrintf("Can't call matching function.\n");
                returnSeq          = GAME_MODE_NUM;
                stGameCnt.blocking = 0;
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_Y){
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // Xボタンが押されていなければ、
                // Yボタンで現在の友達の状態を取得表示（statusString）
                DispFriendList(TRUE);
                OS_TPrintf("\n");
            }
            else {
                // Xボタンが押されていれば、
                // Yボタンで現在の友達の状態を取得表示（statusData）
                DispFriendListData();
                OS_TPrintf("\n");
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_SELECT){
#ifndef GAME_USE_STORAGE_SERVER  // GameSpyのStatusセットを試す場合
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // Xボタンが押されていなければ、
                // セレクトボタンでGameSpy Status文字列をセット
                if (DWC_SetOwnStatusString("SampleString-00")){
                    OS_TPrintf("Set GsStatusString.\n");
                }
                else {
                    OS_TPrintf("Failed to set GsStatusString.\n");
                }
            }
            else {
                // Xボタンが押されていれば、
                // セレクトボタンでGameSpy Statusデータをセット
                if (DWC_SetOwnStatusData(gsStatusSample, sizeof(gsStatusSample))){
                    OS_TPrintf("Set GsStatusData.\n");
                }
                else {
                    OS_TPrintf("Failed to set GsStatusData.\n");
                }
            }

#else  // ストレージサーバへのセーブを試す場合
            // セレクトボタンでストレージサーバにテストキーをセーブ
            char keyvalues[30];
            BOOL boolResult;

            if (stKeyCnt.cont & PAD_BUTTON_X){
                // Xボタンが押されていればPrivateデータをセーブ
                snprintf(keyvalues, sizeof(keyvalues), "\\test_key_prv\\%lld", OS_TicksToSeconds(OS_GetTick()));
                boolResult = DWC_SavePrivateDataAsync(keyvalues, NULL);
            }
            else {
                // Xボタンが押されていなければPublicデータをセーブ
                snprintf(keyvalues, sizeof(keyvalues), "\\test_key_pub\\%lld", OS_TicksToSeconds(OS_GetTick()));
                boolResult = DWC_SavePublicDataAsync(keyvalues, NULL);
            }

            if (boolResult){
                OS_TPrintf("Saved '%s'.\n", keyvalues);
            }
            else {
                OS_TPrintf("Can't save to storage server.\n");
            }
#endif
        }
        else if (stKeyCnt.trg & PAD_BUTTON_START){
#ifndef GAME_USE_STORAGE_SERVER  // GameSpyのStatusセットを試す場合
            char statusData[DWC_FRIEND_STATUS_STRING_LEN];
            
            if (!(stKeyCnt.cont & PAD_BUTTON_X)){
                // Xボタンが押されていなければ、
                // スタートボタンでGameSpy Status文字列を取得
                if (DWC_GetOwnStatusString(statusData)){
                    OS_TPrintf("Got my GsStatusString : %s\n", statusData);
                }
                else {
                    OS_TPrintf("Failed to get GsStatusString.\n");
                }
            }
            else {
                // Xボタンが押されていれば、
                // スタートボタンでGameSpy Statusデータを取得
                int size, i;

                size = DWC_GetOwnStatusData(statusData);
                if (size != -1){
                    OS_TPrintf("Got my GsStatusData\n");
                    for (i = 0; i < size; i++){
                        OS_TPrintf("%02d, ", statusData[i]);
                    }
                    OS_TPrintf("\n");
                }
                else {
                    OS_TPrintf("Failed to get GsStatusData.\n");
                }
            }

#else  // ストレージサーバからのロードを試す場合
            // スタートボタンでストレージサーバからテストデータをロード
            BOOL boolResult;

#if 1
            if (stKeyCnt.cont & PAD_BUTTON_X){
                // Xボタンが押されていれば自分のPrivateデータをロード
                boolResult = DWC_LoadOwnPrivateDataAsync("\\test_key_prv\\test_key_pub", NULL);
            }
            else {
                // Xボタンが押されていなければ自分のPublicデータをロード
                boolResult = DWC_LoadOwnPublicDataAsync("\\test_key_prv\\test_key_pub", NULL);
            }
#else
            // 友達リスト中のホストのデータをロード
            boolResult = DWC_LoadOthersDataAsync("\\test_key", stServerIdx, NULL);
#endif
            if (!boolResult) OS_TPrintf("Can't load from storage server.\n");
#endif
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            // 十字キー上でカーソル移動
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu-1;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // 十字キー下でカーソル移動
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_LEFT){
            // 十字キー左でエントリー人数減少
            if (stNumEntry > 2){
                stNumEntry--;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if (stKeyCnt.trg & PAD_KEY_RIGHT){
            // 十字キー右でエントリー人数増加
            if (stNumEntry < DWC_MAX_CONNECTIONS){
                stNumEntry++;
                // メニューリスト再表示
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_L){
            // LRボタンでサーバクライアントマッチメイクのサーバDSの
            // 友達リストインデックスを選択
            if (curIdx == 3){
                if (stServerIdx > 0){
                    stServerIdx--;
                    // メニューリスト再表示
                    DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
                }
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_R){
            // LRボタンでサーバクライアントマッチメイクのサーバDSの
            // 友達リストインデックスを選択
            if (curIdx == 3){
                if (stServerIdx < GAME_MAX_FRIEND_LIST-1){
                    stServerIdx++;
                    // メニューリスト再表示
                    DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
                }
            }
        }
        ////////// キー操作処理ここまで

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    return returnSeq;
}


/** -------------------------------------------------------------------------
  DWC Transport
  ---------------------------------------------------------------------------*/
const int		SIZE_SEND_BUFFER = 512;
const int		SIZE_RECV_BUFFER = 4 * 1024;

static u8		sSendBuffer[ SIZE_SEND_BUFFER ];
static u8		sRecvBuffer[ DWC_MAX_CONNECTIONS ][ SIZE_RECV_BUFFER ];

/** -------------------------------------------------------------------------
  送信完了コールバック  
  ---------------------------------------------------------------------------*/
static void
SendDoneCallback( int size, u8 aid, void* param )
{
    CommunicationData* commData = (CommunicationData*)param;
    commData->count++;  // 送信したら加算
    OS_TPrintf( "Send Callback aid:%d, size:%d mycount:%d\n", aid, size, commData->count );
}

/** -------------------------------------------------------------------------
  受信完了コールバック  
  ---------------------------------------------------------------------------*/
static void
UserRecvCallback( u8 aid, u8* buffer, int size, void* param )
{
    if ( size == 8 )
    {
        CommunicationData* commData = (CommunicationData*)param;
        commData->count--;	// 受信したら減算
        s_position[ aid ].xpos = (s32)DWCi_LEtoHl(((u32*)buffer)[0]);
        s_position[ aid ].ypos = (s32)DWCi_LEtoHl(((u32*)buffer)[1]);

        OS_TPrintf( "[aid:%d] x:% 8d y:% 8d mycount:%d\n",
                       aid,
                       s_position[ aid ].xpos,
                       s_position[ aid ].ypos,
                       commData->count);
    }
    else
    {
        OS_TPrintf( "invalid recv data size\n" );
    }
}

/** -------------------------------------------------------------------------
  DWC Transport用　受信バッファを設定  
  ---------------------------------------------------------------------------*/
static void
SetRecvBuffer( void )
{
    u8* pAidList;
	int i;
	int connectingNum = DWC_GetAIDList(&pAidList);

    OS_TPrintf( "connection host = %d\n", connectingNum );
    
    for ( i = 0; i < connectingNum; ++i )
    {
		if ( pAidList[i] == DWC_GetMyAID() )
        {
			continue;
        }

        DWC_SetRecvBuffer( pAidList[i], &sRecvBuffer[pAidList[i]], SIZE_RECV_BUFFER );
    }
    
    // ついでにここでグループID も保存
    s_groupID = DWC_GetGroupID();
}

static struct Position sendPos;
/** -------------------------------------------------------------------------
  Reliabe送信
  ---------------------------------------------------------------------------*/
static void
BroadCastReliable( void )
{
#if 1
    // AIDリストを使って全員送信を行う場合
    u8* pAidList;
    int numHost;
    int i;

    numHost = DWC_GetAIDList(&pAidList);

    for (i = 0; i < numHost; i++){
        if (pAidList[i] == DWC_GetMyAID()) continue;

        DWC_SendReliable( pAidList[i], 
                          (const void*)&sendPos, sizeof(s32)*2 );
    }
    
    reliableUse = TRUE;
#else
    // AIDビットマップを使って全員送信を行う場合
    u32 bitmap = DWC_SendReliableBitmap( DWC_GetAIDBitmap(), sSendBuffer, SIZE_SEND_BUFFER );
    OS_TPrintf("Sent to %x.\n", bitmap);
#endif
}

/** -------------------------------------------------------------------------
  Unreliable送信
  ---------------------------------------------------------------------------*/
static void
BroadCastUnreliable( void )
{
#if 1
    // AIDリストを使って全員送信を行う場合
    u8* pAidList;
    int numHost;
    int i;

    numHost = DWC_GetAIDList(&pAidList);

    for (i = 0; i < numHost; i++){
        if (pAidList[i] == DWC_GetMyAID()) continue;

        DWC_SendUnreliable( pAidList[i], 
                            (const void*)&sendPos, sizeof(s32)*2 );
    }

    reliableUse = FALSE;
#else
    // AIDビットマップを使って全員送信を行う場合
    u32 bitmap = DWC_SendUnreliableBitmap( DWC_GetAIDBitmap(), sSendBuffer, SIZE_SEND_BUFFER );
    OS_TPrintf("Sent to %x.\n", bitmap);
#endif
}

/*---------------------------------------------------------------------------*
  メッシュ接続完了後メイン関数
 *---------------------------------------------------------------------------*/
static GameMode GameConnectedMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_CONNECTED];
    GameMode returnSeq = GAME_MODE_NUM;
    int closeResult;
    int curIdx = 0;
    u8 me = DWC_GetMyAID();
    static BOOL autoSend = FALSE;
    
    // 送信コールバックの設定	
    DWC_SetUserSendCallback( SendDoneCallback, &s_commData ); 

    // 受信コールバックの設定	
    DWC_SetUserRecvCallback( UserRecvCallback, &s_commData ); 
    
    // 受信バッファの設定
    // ここでセットするとサーバクライアントマッチメイクの場合に
    // 後から入ってくるクライアントの受信バッファがセットされない！
    //SetRecvBuffer();

    // コネクションクローズコールバックを設定
    DWC_SetConnectionClosedCallback(ConnectionClosedCallback, &returnSeq);

#if 0
	// パケットロス率を設定
	if ( DWC_GetMyAID() != 0 )
    {
		DWC_SetSendDrop( 30, 0 );
		DWC_SetRecvDrop( 30, 0 );        
    }
#endif
#if 0
    // 遅延を設定（単位: ms）
	if ( DWC_GetMyAID() != 0 )
    {
		DWC_SetSendDelay( 500, 0 );
		DWC_SetRecvDelay( 500, 0 );        
    }
#endif    

    OS_TPrintf("AID bitmap = %x\n", DWC_GetAIDBitmap());
    OS_TPrintf( "my aid = %d\n", DWC_GetMyAID() );

    while (1){        
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",   loopstr[((loopcnt)>>2)&3] );
        dbs_Print( 0,2, "AID:%d(0x%08X)", DWC_GetMyAID(), DWC_GetAIDBitmap() );
        dbs_Print( 20, 2, "g:%d", s_groupID );
		loopcnt++;

        ReadKeyData();  // キーデータ取得

        DWC_ProcessFriendsMatch();  // DWC通信処理更新

        HandleDWCError(&returnSeq);  // エラー処理

        if (stGameCnt.blocking){
            // DWC処理中はキー操作を禁止する
            // Vブランク待ち処理
            GameWaitVBlankIntr();
            continue;
	    }

        // 次に進むべきモードがセットされていたらループを抜ける
        if (returnSeq != GAME_MODE_NUM) break;

        {
            // キープアライブでのタイムアウトがあるため、
            // 受信タイムアウト時間より短い間隔で、強制的にデータ送信する。
            static int lastKeepAliveSent = 0;
            int now = (int)DWCi_Np_TicksToMilliSeconds(DWCi_Np_GetTick());
            if (lastKeepAliveSent == 0 || (now - lastKeepAliveSent) > KEEPALIVE_INTERVAL)
            {
              autoSend = TRUE;
              lastKeepAliveSent = now;
            }
        }
    
        if ( stKeyCnt.trg & PAD_KEY_LEFT )  s_position[ me ].xpos--;
        if ( stKeyCnt.trg & PAD_KEY_RIGHT ) s_position[ me ].xpos++;
        
        ////////// 以下キー操作処理
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // Aボタンでメニュー決定
            returnSeq = gameSeq->menuList[curIdx].mode;
            stGameCnt.blocking = gameSeq->menuList[curIdx].blocking;

            switch (curIdx){
            case 0:             // Reliable送信
                BroadCastReliable();
				break;
            case 1:             // Unreliable送信
				BroadCastUnreliable();
                break;
            case 2:             // Suspend(TRUE)開始
                DWC_RequestSuspendMatchAsync(TRUE, SuspendCallback, NULL);
                break;
            case 3:             // Suspend(FALSE)開始
                DWC_RequestSuspendMatchAsync(FALSE, SuspendCallback, NULL);
                break;
            case 4:             // コネクションクローズ
                stCloseFlag = TRUE;
                closeResult = DWC_CloseAllConnectionsHard();
                break;
            default:
                break;
            }
        }
        else if (stKeyCnt.trg & PAD_BUTTON_SELECT){
            // セレクトボタンでaid = 1のホストを強制切断する。
            // 通信中に電源を切ったホストを切断する時のテスト
            u32 bitmap = 0x02;
#if 1
            // AID指定版
            closeResult = DWC_CloseConnectionHardFromServer(2);
#else
            // AIDビットマップ指定版
            closeResult = DWC_CloseConnectionHardBitmapFromServer(&bitmap);
#endif

            OS_TPrintf("Closed connection to aid = 1, result = %d, bitmap = %x\n",
                       closeResult, bitmap);

            // 結果に関わらず処理を続けるのでブロッキングしない。
            // また、クローズコールバックはCloseConnectionHard関数内で呼ばれる。
            stGameCnt.blocking = FALSE;
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            // 十字キー上でカーソル移動
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu-1;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // 十字キー下でカーソル移動
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if(stKeyCnt.trg & PAD_KEY_LEFT || stKeyCnt.trg & PAD_KEY_RIGHT || autoSend)
        {
            sendPos.xpos = (s32)DWCi_HtoLEl((u32)s_position[ me ].xpos);
            sendPos.ypos = (s32)DWCi_HtoLEl((u32)s_position[ me ].ypos);
            // すべての相手に自分の位置を送信
            if(reliableUse)
            {
                BroadCastReliable();
            }
            else
            {
                BroadCastUnreliable();
            }
            autoSend = FALSE;
        }
        ////////// キー操作処理ここまで

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    return returnSeq;
}


/*---------------------------------------------------------------------------*
  友達リストの空きインデックス取得関数
 *---------------------------------------------------------------------------*/
int GetAvailableFriendListIndex(void)
{
    int i;

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i]))
            return i;
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  ログインコールバック関数
 *---------------------------------------------------------------------------*/
static void LoginCallback(DWCError error, int profileID, void *param)
{
    BOOL result;
	RTCTime time;
	RTCDate date;

	if (error == DWC_ERROR_NONE){
        // フィルタ文字列を作成する
        char* filterString = FILTER_STRING;
		OS_SNPrintf(addFilter, sizeof(addFilter), "%s='%s'", FILTER_KEY, filterString);
        
        // 認証成功、プロファイルID取得
        OS_TPrintf("Login succeeded. profileID = %u\n\n", profileID);

        // ingamesnチェックの結果を取得する
		if(DWC_GetIngamesnCheckResult() == DWC_INGAMESN_INVALID)
			OS_TPrintf("BAD ingamesn is detected by NAS.\n");
		else if(DWC_GetIngamesnCheckResult() == DWC_INGAMESN_VALID)
			OS_TPrintf("GOOD ingamesn is detected by NAS.\n");
		else
			OS_TPrintf("ingamesn is not checked yet.\n");
		
		// 認証サーバの時刻を表示する
		if(DWC_GetDateTime(&date, &time) == TRUE)
			OS_TPrintf("NasTime = %02d/%02d/%02d %02d:%02d:%02d\n", date.year+2000, date.month, date.day, time.hour, time.minute, time.second);
		else
			DWC_Printf(DWC_REPORTFLAG_AUTH, "DWC_GetNasTime failed\n");


        // stUserDataが更新されているかどうかを確認。
        if ( DWC_CheckDirtyFlag( &stUserData ) )
        {
            DWC_ClearDirtyFlag( &stUserData );

            OS_TPrintf("*******************************\n");
            OS_TPrintf("You must save the DWCUserData!!\n");
            OS_TPrintf("*******************************\n");

            // 必ずこのタイミングでチェックして、dirty flagが有効になっていたら、
            // DWCUserDataをDSカードのバックアップに保存するようにしてください。
            // saveUserDataToDSCard( &stUserData );
        }

        stGameCnt.userData.profileID = profileID;
        stGameCnt.userData.isValidProfile = TRUE;

        // GameSpy Status文字列セットテスト
        //DWC_SetGsStatusData(gsStatusSample, sizeof(gsStatusSample));
        

        // 友達リスト同期処理開始。
        result = DWC_UpdateServersAsync(NULL,//stGameCnt.userData.playerName,
                                        UpdateServersCallback, param,
                                        FriendStatusCallback, param,
                                        DeleteFriendListCallback, param);
        if (result == FALSE){
            // アップデートを開始できなかった場合はブロッキング状態を解除する。
            // エラー発生中かログイン前、二重呼び出し時しかFALSEにはならない。
            OS_TPrintf("Can't call DWC_UpdateServersAsync().\n");
             *(GameMode *)param = GAME_MODE_NUM;
            stGameCnt.blocking  = 0;
        }
        else {
            // GameSpyサーバ上バディ成立コールバックを登録する
            DWC_SetBuddyFriendCallback(BuddyFriendCallback, NULL);

            // ストレージサーバセーブ・ロード完了通知コールバックを登録する
            DWC_SetStorageServerCallback(SaveToServerCallback,
                                         LoadFromServerCallback);
        }
        
		// キープアライブ時間の設定        
        DWC_SetConnectionKeepAliveTime(APP_CONNECTION_KEEPALIVE_TIME);
        
        // フレンドキーを取得
        s_friendkey = DWC_CreateFriendKey( &stUserData );
    }
    else {
        // 認証失敗
        OS_TPrintf("Login failed. %d\n\n", error);

        stGameCnt.blocking = FALSE;  // ブロッキング解除
    }
}


/*---------------------------------------------------------------------------*
  友達リスト同期処理完了コールバック関数
 *---------------------------------------------------------------------------*/
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)

    if (error == DWC_ERROR_NONE){
        // 友達リスト同期処理完了
        OS_TPrintf("Updating servers succeeded.\n");

        if (isChanged){
            // 友達リストが変更されていたらセーブする
            // （セーブのつもり）
            MI_CpuCopy32(&stGameCnt.friendList, &saveData.friendList, sizeof(GameFriendList));
        }

        // 友達リストを表示する
        DispFriendList(TRUE);
    }
    else {
        // 失敗しても特に何もしない
        OS_TPrintf("Failed to update GameSpy servers. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // ブロッキング解除
}


/*---------------------------------------------------------------------------*
  友達状態変化通知コールバック関数
 *---------------------------------------------------------------------------*/
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param)
{
#pragma unused(param)

    OS_TPrintf("Friend[%d] changed status -> %d (statusString : %s).\n",
               index, status, statusString);
}


/*---------------------------------------------------------------------------*
  友達リスト削除コールバック関数
 *---------------------------------------------------------------------------*/
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param)
{
#pragma unused(param)

    OS_TPrintf("friend[%d] was deleted (equal friend[%d]).\n",
               deletedIndex, srcIndex);
}


/*---------------------------------------------------------------------------*
  GameSpyバディ成立コールバック関数
 *---------------------------------------------------------------------------*/
static void BuddyFriendCallback(int index, void* param)
{
#pragma unused(param)

    OS_TPrintf("I was authorized by friend[%d].\n", index);
}


/*---------------------------------------------------------------------------*
  ストレージサーバへのログイン完了コールバック関数
 *---------------------------------------------------------------------------*/
static void LoginToStorageCallback(DWCError error, void* param)
{
#pragma unused(param)

    OS_TPrintf("Login to storage server: result %d\n", error);
}


/*---------------------------------------------------------------------------*
  ストレージサーバへのデータセーブ完了コールバック関数
 *---------------------------------------------------------------------------*/
static void SaveToServerCallback(BOOL success, BOOL isPublic, void* param)
{
#pragma unused(param)

    OS_TPrintf("Saved data to server.\n");
    OS_TPrintf("result %d, isPublic %d.\n", success, isPublic);
}


/*---------------------------------------------------------------------------*
  ストレージサーバからのデータロード完了コールバック関数
 *---------------------------------------------------------------------------*/
static void LoadFromServerCallback(BOOL success, int index, char* data, int len, void* param)
{
#pragma unused(param)

    OS_TPrintf("Loaded data from server.\n");
    OS_TPrintf("result %d, index %d, data '%s', len %d\n",
               success, index, data, len);
}


/*---------------------------------------------------------------------------*
  友達無指定接続コールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectToAnybodyCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer,
                                     int index, void* param)
{
    (void)self;
    (void)isServer;
    (void)index;
    (void)param;
    
    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // 見知らぬ人たちとのコネクション設立完了
            OS_TPrintf("Succeeded to connect to anybody\n\n");

            // 受信バッファセット
            SetRecvBuffer();
        }
        else {
            OS_TPrintf("Canceled matching.\n");
        }
    }
    else {
        OS_TPrintf("Failed to connect to anybody. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // ブロッキング解除
}


/*---------------------------------------------------------------------------*
  友達指定接続コールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectToFriendsCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer,
                                     int index, void* param)
{
    (void)self;
    (void)isServer;
    (void)index;
    (void)param;

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // 友達とのコネクション設立完了
            OS_TPrintf("Succeeded to connect to friends\n\n");

            // 受信バッファセット
            SetRecvBuffer();
        }
        else {
            OS_TPrintf("Canceled matching.\n");
        }
    }
    else {
        OS_TPrintf("Failed to connect to friends. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // ブロッキング解除
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク・サーバ起動コールバック関数
 *---------------------------------------------------------------------------*/
static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int  index,
                                    void* param)
{
#pragma unused(isServer)

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            // ネットワークに新規クライアントが加わった
            OS_TPrintf("A friend of mine joined the game.\n");
            OS_TPrintf("friendListIndex = %d.\n\n", index);

            // 受信バッファセット
            SetRecvBuffer();
        }
        else {
            if (self){
                // 自分がマッチメイクをキャンセルした
                OS_TPrintf("Canceled matching.\n\n");
                // ログイン後状態に戻る
                // paramの参照先はauto変数returnSeqだが、自分がマッチメイクを
                // キャンセルできる時はスコープ内
                *(GameMode *)param = GAME_MODE_LOGIN;
            }
            else {
                // クライアントDSがマッチメイクをキャンセルした
                OS_TPrintf("Client (friendListIndex = %d) canceled matching.\n\n", index);

                if (DWC_GetNumConnectionHost() == 1){
                    // まだ誰とも接続していなければブロッキングを解除しない。
                    // 一度でもマッチメイクが完了した後に接続数が1になった場合は
                    // 元々ブロッキング解除状態なので問題ない。
                    return;
                }
            }
        }
    }
    else {
        // エラーが発生した
        OS_TPrintf("Game server error occured. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // ブロッキング解除
}


/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク・サーバ接続コールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int  index,
                                        void* param)
{

    if (error == DWC_ERROR_NONE){
        if (!cancel){
            if (self){
                // 自分がサーバDSとそこにできているネットワークへの接続に
                // 成功した場合
                OS_TPrintf("Succeeded to join the game.\n\n");
            }
            else {
                // ネットワークに新規クライアントが加わった場合
                OS_TPrintf("New client joined the game.\n");
                if (index != -1){
                    OS_TPrintf("he is my friend [%d].\n\n", index);
                }
                else {
                    OS_TPrintf("he is not my friend.\n\n");
                }
            }

            // 受信バッファセット
            SetRecvBuffer();
        }
        else {
            if (self){
                // 自分がマッチメイクをキャンセルした
                OS_TPrintf("Canceled matching.\n\n");
                // ログイン後状態に戻る
                *(GameMode *)param = GAME_MODE_LOGIN;
            }
            else {
                if (isServer){
                    // サーバDSがマッチメイクをキャンセルした
                    OS_TPrintf("GameServer canceled matching.\n\n");
                    // マッチメイクを終了してログイン後状態に戻る
                    *(GameMode *)param = GAME_MODE_LOGIN;
                }
                else {
                    // 他のクライアントDSがマッチメイクをキャンセルした
                    OS_TPrintf("Host (friendListIndex = %d) canceled matching.\n\n", index);
                }
            }
        }
    }
    else {
        // エラーが発生した
        OS_TPrintf("Failed to join the game. %d\n\n", error);
    }

    stGameCnt.blocking = FALSE;  // ブロッキング解除
}


/*---------------------------------------------------------------------------*
  コネクションクローズコールバック関数
 *---------------------------------------------------------------------------*/
static void ConnectionClosedCallback(DWCError error,
                                     BOOL isLocal,
                                     BOOL isServer,
                                     u8  aid,
                                     int index,
                                     void* param)
{
#pragma unused(isServer)

    if (error == DWC_ERROR_NONE){
        if (isLocal){
            OS_TPrintf("Closed connection to aid %d (friendListIndex = %d) Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
            // 自分でクローズした場合はログイン状態に戻す
            *(GameMode *)param = GAME_MODE_LOGIN;
            stCloseFlag = FALSE;
        }
        else {
            OS_TPrintf("Connection to aid %d (friendListIndex = %d) was closed. Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
        }
    }
    else {
        OS_TPrintf("Failed to close connections. %d\n\n", error);
    }
}


/*---------------------------------------------------------------------------*
  新規接続クライアント通知コールバック関数
 *---------------------------------------------------------------------------*/
static void GenericNewClientCallback(int index, void* param )
{
    (void)param;

    OS_TPrintf( "New Client is connecting(idx %d)...\n", index);
}

/*---------------------------------------------------------------------------*
  サーバクライアントマッチメイク時の新規接続クライアント通知コールバック関数
 *---------------------------------------------------------------------------*/
static void NewClientCallback(int index, void* param)
{
#pragma unused(param)

    // 新規接続クライアントの接続処理が終わるまでは、
    // コントローラ操作を禁止する
    // →実際のゲームの処理に即してブロッキングしないようにする
    //stGameCnt.blocking = TRUE;
    
    OS_TPrintf("New client started to connect.\n");
    if (index != -1){
        OS_TPrintf("He is my friend [%d].\n", index);
    }
    else {
        OS_TPrintf("He is not my friend.\n");
    }
}


/*---------------------------------------------------------------------------*
  友達無指定ピアマッチメイク時プレイヤー評価コールバック関数
 *---------------------------------------------------------------------------*/
static int  EvaluateAnybodyCallback(int index, void* param)
{
#pragma unused(param)
    const char* matchType = DWC_GetMatchStringValue(index, stMatchKeys[1].keyStr, "NONE");
    int rank;
    int diff;
#if 0  // 評価のテストを行いたい場合はここを1にする
    int full = 200;
#else  // 普段はマッチメイクしないことがあると面倒なのでこちらにする
    int full = 300;
#endif
    
    if (!strncmp(matchType, "eval_rank", strlen("eval_rank"))){
        // 相手もマッチメイクタイプとして"eval_rank"をセットしている場合
        rank = DWC_GetMatchIntValue(index, stMatchKeys[0].keyStr, -1);
        if (rank == -1){
            return 0;  // "game_rank"キーがなければマッチメイクしない
        }

        diff = (stMatchKeys[0].ivalue > rank) ? stMatchKeys[0].ivalue-rank : rank-stMatchKeys[0].ivalue;

        OS_TPrintf("Player[%d]'s rank is %d (mine %d). %d point.\n",
                   index, rank, stMatchKeys[0].ivalue, full-diff);

        // 自分との順位差が近いほど高得点になるように、200点満点で点を付ける。
        // 200位差以上ならマッチメイクしない（0以下の値を返す）
        return full-diff;
    }
    else if (!strncmp(matchType, "NONE", strlen("NONE"))){
        return 1;  // "game_mtype"キーがなければとりあえず平等にマッチメイクする
    }

    return 1;
}


/*---------------------------------------------------------------------------*
  友達指定ピアマッチメイク時プレイヤー評価コールバック関数
 *---------------------------------------------------------------------------*/
static int  EvaluateFriendCallback(int index, void* param)
{
#pragma unused(param)
    const char* matchType = DWC_GetMatchStringValue(index, stMatchKeys[1].keyStr, "NONE");
    
    return 1;  // 今は受け入れてみる
    
    if (!strncmp(matchType, "eval_rank", strlen("eval_rank"))){
        // 相手もマッチメイクタイプとして"eval_rank"をセットしている場合
        return 1;  // マッチメイクOK
    }
    else {
        return 0;  // "game_mtype"キーがなければマッチメイクしない
    }
}


/*---------------------------------------------------------------------------*
  DWCライブラリエラー処理関数
 *---------------------------------------------------------------------------*/
static DWCError HandleDWCError(GameMode* gameMode)
{
    int errorCode;
    DWCError error;
    DWCErrorType errorType;

    // DWCエラー取得
    error = DWC_GetLastErrorEx(&errorCode, &errorType);

    // エラーがなければすぐに戻る
    if (error == DWC_ERROR_NONE) return DWC_ERROR_NONE;

    // エラーに対応したメッセージを表示
    DispErrorMessage(error, errorCode, errorType);

    DWC_ClearError();        // エラーをクリア
    stGameCnt.blocking = 0;  // ブロッキング解除

    switch (errorType){
    case DWC_ETYPE_LIGHT:
        // 今のところマッチメイク中にしか出ないエラータイプなので、
        // 必ずログイン後状態に戻る
        *gameMode = GAME_MODE_LOGIN;
        break;

    case DWC_ETYPE_SHUTDOWN_FM:
        // FriendsMatch処理を終了し、GameSpyのヒープを解放する
        DWC_ShutdownFriendsMatch();

        // ログイン前の状態に戻る
        *gameMode = GAME_MODE_MAIN;
        break;

    case DWC_ETYPE_DISCONNECT:
        // FriendsMatch処理を終了し、GameSpyのヒープを解放する
        DWC_ShutdownFriendsMatch();

        // インターネット接続を完全に初期化する
        DWC_CleanupInet();

        // ログイン前の状態に戻る
        *gameMode = GAME_MODE_MAIN;
        break;

    case DWC_ETYPE_FATAL:
        // Fatal Error なので停止する
        OS_Panic("========== Stop process ==========\n");
        break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
  通信エラー表示関数
 *---------------------------------------------------------------------------*/
static void DispErrorMessage(DWCError error, int errorCode, DWCErrorType errorType)
{

    if (error == DWC_ERROR_NONE) return;

    OS_TPrintf("ERROR: %d, code:%d, type:%d\n", error, -1*errorCode, errorType);
    
    switch (error){
    case DWC_ERROR_DS_MEMORY_ANY:
        OS_TPrintf("ERROR: DS_MEMORY_ANY.\n");
        break;
    case DWC_ERROR_AUTH_ANY:
        OS_TPrintf("ERROR: AUTH_ANY.\n");
        break;
    case DWC_ERROR_AUTH_OUT_OF_SERVICE:
        OS_TPrintf("ERROR: This game's network service has ended.\n");
        break;
    case DWC_ERROR_AUTH_STOP_SERVICE:
        OS_TPrintf("ERROR: Now server is temporarily unavailable.\nPlease re-login later.\n");
        break;
    case DWC_ERROR_AC_ANY:
        OS_TPrintf("ERROR: AC ANY.\n");
        break;
    case DWC_ERROR_NETWORK:
        OS_TPrintf("ERROR: Network error.\n");
        break;
    case DWC_ERROR_DISCONNECTED:
        OS_TPrintf("ERROR: Connection to AP was lost.\n");
        break;
    case DWC_ERROR_FATAL:
        OS_TPrintf("FATAL ERROR: Please turn the power off.\n");
        break;
    case DWC_ERROR_FRIENDS_SHORTAGE:
        OS_TPrintf("ERROR: Your friends seem not to login yet.\n");
        break;
    case DWC_ERROR_NOT_FRIEND_SERVER:
        OS_TPrintf("ERROR: He is not buddy or not game server.\n");
        break;
    case DWC_ERROR_SC_CONNECT_BLOCK:
        OS_TPrintf("ERROR: Game server is busy now.\n");
        break;
    case DWC_ERROR_SERVER_FULL:
        OS_TPrintf("ERROR: Game server is fully occupied.\n");
        break;
    case DWC_ERROR_NETWORK_LIGHT:
        OS_TPrintf("ERROR: Network light error.\n");
        break;
    default:
        OS_TPrintf("ERROR: Unexpected error(%d).\n", error);
        break;
    }
}


/*---------------------------------------------------------------------------*
  メッセージリスト表示（カーソル付き）関数
 *---------------------------------------------------------------------------*/
static void DispMenuMsgWithCursor(const GameSequence* gameSeq, int index, void* param)
{
    int i;

    OS_TPrintf("===============================\n");
    OS_TPrintf("%s\n", gameSeq->modeName);

    for (i = 0; i < gameSeq->numMenu; i++){
        if (i == index) OS_TPrintf("> ");
        else OS_TPrintf("  ");

        OS_TPrintf("%s\n", gameSeq->menuList[i].msg);
    }

    // モードごとの追加表示用コールバック関数を呼び出す
    if (gameSeq->dispCallback) gameSeq->dispCallback(index, param);

    OS_TPrintf("===============================\n\n");
    OS_TPrintf("+++VALID CONSOLE %d +++\n", DWC_CheckValidConsole( &stUserData ) );
    Heap_Print();
}


/*---------------------------------------------------------------------------*
  友達リスト表示関数（GameSpy Status文字列表示版）
 *---------------------------------------------------------------------------*/
static void DispFriendList(BOOL overLoad)
{
    u8   maxEntry = 0;
    u8   numEntry;
    char statusString[DWC_FRIEND_STATUS_STRING_LEN];
    int  i;

    statusString[0] = '\0';

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i]))
            continue;
        
        if (overLoad){
            // 上書き指定の場合は友達情報の更新も行う
            stGameCnt.friendStatus[i] =
                DWC_GetFriendStatusSC(&stGameCnt.friendList.keyList[i],
                                      &maxEntry, &numEntry, statusString);
        }
        
        if (maxEntry){
            // 友達がサーバクライアントマッチメイクのサーバDSの場合は、
            // （現在接続人数／最大接続人数）を表示する
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, server (%d/%d), statusString : %s\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i],
                       numEntry, maxEntry, statusString);
        }
        else {
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, statusString : %s\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i], statusString);
        }
    }
}


/*---------------------------------------------------------------------------*
  友達リスト表示関数（GameSpy Statusデータ表示版）
 *---------------------------------------------------------------------------*/
static void DispFriendListData(void)
{
    u8   maxEntry = 0;
    u8   numEntry;
    char statusData[DWC_FRIEND_STATUS_STRING_LEN];
    int  size;
    int  i, j;

    for (i = 0; i < GAME_MAX_FRIEND_LIST; i++){
        if (!DWC_IsValidFriendData(&stGameCnt.friendList.keyList[i]))
            continue;
        
        // 友達情報の更新
        stGameCnt.friendStatus[i] =
            DWC_GetFriendStatusDataSC(&stGameCnt.friendList.keyList[i],
                                      &maxEntry, &numEntry, statusData, &size);
        
        if (maxEntry){
            // 友達がサーバクライアントマッチメイクのサーバDSの場合は、
            // （現在接続人数／最大接続人数）を表示する
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d, server (%d/%d)\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i],
                       numEntry, maxEntry);
        }
        else {
            OS_TPrintf("Friend[%d] '%s' : profileID %u, status %d\n",
                       i, stGameCnt.friendList.playerName[i],
                       DWC_GetGsProfileId(&stUserData, &stGameCnt.friendList.keyList[i]),
                       stGameCnt.friendStatus[i]);
        }

        // GameSpy Status文字列を表示する
        OS_TPrintf("StatusData : ");
        for (j = 0; j < size; j++){
            OS_TPrintf("%02d, ", statusData[j]);
        }
        OS_TPrintf("\n");
    }
}


/*---------------------------------------------------------------------------*
  友達登録・消去モードの追加表示用コールバック関数
 *---------------------------------------------------------------------------*/
static void RegFriendModeDispCallback(int curIdx, void* param)
{
    RegFriendParam cntParam;
    int i;
    char* stFriendKey;
    u32 figure;
    int maxFigure = 12; // 今は固定で

    if (!param){
        // 初回はparam = NULLで呼ばれる
        cntParam.step = 0;
    }
    else {
        cntParam = *(RegFriendParam *)param;
    }

    OS_TPrintf("\n");

    stFriendKey = cntParam.value;
    figure = cntParam.figure;
    
    if (cntParam.step == 0){
        // コマンド選択中は友達リストを表示
        DispFriendList(FALSE);
    }
    else {
        // 数値入力中
        if (curIdx == 0){
            // 友達登録。プロファイルID入力中
            OS_TPrintf("input> ");
            for (i=0; i<maxFigure; ++i){
                OS_TPrintf("%c", stFriendKey[i]);
                if(i == 3 || i == 7)
	                OS_TPrintf("-");
            }
            OS_TPrintf("\n");

            // 選択中の数字を示す為の下線を表示する
            for (i = 0; i < cntParam.figure; i++){
                OS_TPrintf(" ");
            }

            if(cntParam.figure > 3)
            	OS_TPrintf(" ");
            	
            if(cntParam.figure > 7)
            	OS_TPrintf(" ");

            OS_TPrintf("       -\n");
        }
        else {
            // 友達削除。友達リストインデックス入力中
            DispFriendList(FALSE);  // 友達リストを表示
            
            OS_TPrintf("Delete index : %c%c\n", stFriendKey[0], stFriendKey[1]);

            for (i = 0; i < cntParam.figure; i++){
                OS_TPrintf(" ");
            }
            OS_TPrintf("               -\n");
        }
    }
}


/*---------------------------------------------------------------------------*
  ログイン後モードの追加表示用コールバック関数
 *---------------------------------------------------------------------------*/
static void LogonModeDispCallback(int curIdx, void* param)
{
#pragma unused(param)

    // 自分以外のエントリー人数へのポインタを表示する
    OS_TPrintf("\nnumber of entry = ");
    if (stNumEntry == 2){
        OS_TPrintf("   %d >>\n", stNumEntry);
    }
    else if (stNumEntry == DWC_MAX_CONNECTIONS){
        OS_TPrintf("<< %d\n", stNumEntry);
    }
    else {
        OS_TPrintf("<< %d >>\n", stNumEntry);
    }

    if (curIdx == 3){
        // サーバクライアントマッチメイクのサーバDSへの接続時は、
        // 友達リストインデックスも表示する
        OS_TPrintf("GameServerIndex = ");
        if (DWC_GetNumFriend(stGameCnt.friendList.keyList, GAME_MAX_FRIEND_LIST) == 1){
            OS_TPrintf("   %d\n", stServerIdx);
        }
        else if (stServerIdx == 0){
            OS_TPrintf("   %d >>\n", stServerIdx);
        }
        else if (stServerIdx == GAME_MAX_FRIEND_LIST-1){
            OS_TPrintf("<< %d\n", stServerIdx);
        }
        else {
            OS_TPrintf("<< %d >>\n", stServerIdx);
        }
    }
}


/*---------------------------------------------------------------------------*
  マッチメイクモードの追加表示用コールバック関数
 *---------------------------------------------------------------------------*/
static void MatchModeDispCallback(int curIdx, void* param)
{
#pragma unused(curIdx)
#pragma unused(param)
    
}


/*---------------------------------------------------------------------------*
  Wi-Fiコネクション設定の使用言語選択
 *---------------------------------------------------------------------------*/
static GameMode NetConfigLanguageMain(void)
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_NETCONFIG_LANGUAGE];
    int curIdx = 0;
    const int languageTable[] = {
        DWC_LANGUAGE_JAPANESE,
        DWC_LANGUAGE_ENGLISH,
        DWC_LANGUAGE_FRENCH,
        DWC_LANGUAGE_GERMAN,
        DWC_LANGUAGE_ITALIAN,
        DWC_LANGUAGE_SPANISH,
        DWC_LANGUAGE_HANGUL
    };

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState());
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost());
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel());
        dbs_Print(10, 1, "p:%d", stGameCnt.userData.profileID);
        dbs_Print(30, 0, "%c",  loopstr[((loopcnt++) >> 2) & 3]);
        ReadKeyData();
        if (stKeyCnt.trg & PAD_BUTTON_A){
            utilityLangage = languageTable[curIdx];
            break;
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu - 1;
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        GameWaitVBlankIntr();
    }

    return gameSeq->menuList[curIdx].mode;
}


/*---------------------------------------------------------------------------*
  Wi-Fiコネクション設定
 *---------------------------------------------------------------------------*/
FS_EXTERN_OVERLAY(main_overlay_1);

static GameMode NetConfigMain(void)
{
    GameMode returnSeq = GAME_MODE_NETCONFIG_LANGUAGE;

    (void)FS_LoadOverlay( MI_PROCESSOR_ARM9, FS_OVERLAY_ID(main_overlay_1) ) ;

    DWC_CleanupInet(); // ネットワークから切断する。
    sPrintOverride = FALSE; // OS_TPrintf()の出力を一時的に元に戻す。
    dbs_DemoFinalize();

#if defined( NETCONFIG_USE_HEAP )
    {
        void* work = OS_Alloc(DWC_UTILITY_WORK_SIZE);

        (void)DWC_StartUtility(
            work,
            utilityLangage,
            utilityLangage == DWC_LANGUAGE_JAPANESE ? DWC_UTILITY_TOP_MENU_FOR_JPN : DWC_UTILITY_TOP_MENU_COMMON
        );

        OS_Free(work);
    }
#else
    (void)DWC_StartUtilityEx(
        utilityLangage,
        utilityLangage == DWC_LANGUAGE_JAPANESE ? DWC_UTILITY_TOP_MENU_FOR_JPN : DWC_UTILITY_TOP_MENU_COMMON
    );
#endif

    (void)FS_UnloadOverlay( MI_PROCESSOR_ARM9, FS_OVERLAY_ID(main_overlay_1) ) ;

    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
    (void)GX_VBlankIntr( TRUE );

    dbs_DemoInit();
    sPrintOverride = TRUE; // OS_TPrintf()の出力をconsoleにつなぐ.
    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();

    return returnSeq;
}

static GameMode StartIPMain(void)
{
	DWCApInfo apinfo;
	
    DWC_InitInet( &stConnCtrl );
    
    DWC_ConnectInetAsync();
    // 安定なステートまで待つ。
    while ( !DWC_CheckInet() )
    {
        DWC_ProcessInet();

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    switch ( DWC_GetInetStatus() )
    {
    case DWC_CONNECTINET_STATE_ERROR:
        {
            // エラー表示
            DWCError err;
            int errcode;
            err = DWC_GetLastError( &errcode );

            OS_TPrintf("   Error occurred %d %d.\n", err, errcode );
        }
        DWC_ClearError();

        DWC_CleanupInet();
        break;
    case DWC_CONNECTINET_STATE_FATAL_ERROR:
        {
            // エラー表示
            DWCError err;
            int errcode;
            err = DWC_GetLastError( &errcode );

            OS_Panic("   Error occurred %d %d.\n", err, errcode );
        }
        break;

    case DWC_CONNECTINET_STATE_CONNECTED:
        OS_TPrintf("   CONNECTED!!!.\n");
		if(DWC_GetApInfo(&apinfo) == TRUE) {
			OS_TPrintf("DWCApInfo.aptype = %d\n", apinfo.aptype);
			OS_TPrintf("DWCApInfo.area   = %d\n", apinfo.area);
			OS_TPrintf("DWCApInfo.spotinfo = %s\n", apinfo.spotinfo);
			OS_TPrintf("DWCApInfo.essid = %s\n", apinfo.essid);
			OS_TPrintf("DWCApInfo.bssid = %02x:%02x:%02x:%02x:%02x:%02x\n", apinfo.bssid[0], apinfo.bssid[1], apinfo.bssid[2], apinfo.bssid[3], apinfo.bssid[4], apinfo.bssid[5]);
		}
		else {
			OS_TPrintf("Failed to get DWCApInfo\n");
		}
			
        break;
    }

    return GAME_MODE_MAIN;
}

static GameMode StopIPMain(void)
{
    DWC_CleanupInet();

    return GAME_MODE_MAIN;
}

static GameMode BackupUserdataMain( void )
{
    GameSequence *gameSeq = &gameSeqList[GAME_MODE_BACKUP];
    int curIdx = 0;
    GameMode returnSeq = GAME_MODE_NUM;

    while (1){
        dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
        dbs_Print( 7, 0, "n:%d", DWC_GetNumConnectionHost() );
        dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
        dbs_Print( 10,1, "p:%d", stGameCnt.userData.profileID );
	    dbs_Print( 30, 0, "%c",   loopstr[((loopcnt)>>2)&3] );
		loopcnt++;

        ReadKeyData();  // キーデータ取得

        HandleDWCError(&returnSeq);  // エラー処理

        // 次に進むべきモードがセットされていたらループを抜ける
        if (returnSeq != GAME_MODE_NUM) break;

        ////////// 以下キー操作処理
        if (stKeyCnt.trg & PAD_BUTTON_A){
            // Aボタンでメニュー決定
            BOOL end = FALSE;
            BOOL error = FALSE;
            int last_result;
            CARD_LockBackup((u16)card_lock_id);
            end = CARD_IdentifyBackup(stBackupTypeTable[stNumBackup].type);
            if(!end)
            {
                error = TRUE;
                last_result = CARD_GetResultCode();
            }
            CARD_UnlockBackup(card_lock_id);


            switch (curIdx){
            case 0:  // Load
                if ( error ) {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                CARD_LockBackup((u16)card_lock_id);
                if (CARD_IsBackupEeprom())
                {
                    CARD_ReadEeprom(0, &stUserData, sizeof(stUserData) );
					CARD_ReadEeprom(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_ReadFlash(0, &stUserData, sizeof(stUserData) );
					CARD_ReadFlash(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                CARD_UnlockBackup(card_lock_id);
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                else
                {
                    OS_TPrintf("Load OK!\n");
                    // ロードできたらフレンドキーを取得してみる
                    s_friendkey = DWC_CreateFriendKey( &stUserData );
                    returnSeq = GAME_MODE_MAIN;
                }
                break;
            case 1:  // Save
                if ( error ) {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                CARD_LockBackup((u16)card_lock_id);
                if (CARD_IsBackupEeprom())
                {
                    CARD_WriteAndVerifyEeprom(0, &stUserData, sizeof(stUserData) );
					CARD_WriteAndVerifyEeprom(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_WriteAndVerifyFlash(0, &stUserData, sizeof(stUserData) );
					CARD_WriteAndVerifyFlash(sizeof(stUserData), &stGameCnt.friendList, sizeof(GameFriendList));
                }
                CARD_UnlockBackup(card_lock_id);
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    OS_TPrintf("Error occur with %d\n", last_result);
                    continue;
                }
                else
                {
                    OS_TPrintf("Save OK!\n");
                    returnSeq = GAME_MODE_MAIN;
                }
                break;
            case 2: // report
                DWC_ReportUserData( &stUserData );
                break;
            case 3:
                returnSeq = GAME_MODE_MAIN;
                break;
            }
        }
        else if (stKeyCnt.trg & PAD_KEY_UP){
            // 十字キー上でカーソル移動
            curIdx--;
            if (curIdx < 0) curIdx = gameSeq->numMenu-1;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // 十字キー下でカーソル移動
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_LEFT){
            // 十字キー左でバックアップデバイスの種類変更
            stNumBackup--;
            stNumBackup %= BACKUP_TABLE_MAX;

            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_RIGHT){
            // 十字キー右でバックアップデバイスの種類変更
            stNumBackup++;
            stNumBackup %= BACKUP_TABLE_MAX;

            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        ////////// キー操作処理ここまで

        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }

    return returnSeq;
}

static GameMode SetTopologyMain( void )
{
    GameSequence* gameSeq = &gameSeqList[GAME_MODE_TOPOLOGY];
    GameMode returnSeq = GAME_MODE_NUM;
    int curIdx = 0;
    
    while(1){
        ReadKeyData(); // キーデータ取得
        
        // 次に進むべきモードがセットされていたら抜ける
        if(returnSeq != GAME_MODE_NUM) break;
        
        if(stKeyCnt.trg & PAD_BUTTON_A){
            // 接続形態決定
            returnSeq = gameSeq->menuList[curIdx].mode;
            
            if(curIdx < 3){
                switch(curIdx){
                case 0: s_topologyType = DWC_TOPOLOGY_TYPE_HYBRID;   break;
                case 1: s_topologyType = DWC_TOPOLOGY_TYPE_FULLMESH; break;
                case 2: s_topologyType = DWC_TOPOLOGY_TYPE_STAR;     break;
                }
                OS_TPrintf("Set topology type[%s]!!\n\n", GetTopologyToString());
                DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
            }
        }
        else if(stKeyCnt.trg & PAD_KEY_UP){
            curIdx--;
            if(curIdx < 0) curIdx = gameSeq->numMenu - 1;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        else if (stKeyCnt.trg & PAD_KEY_DOWN){
            // 十字キー下でカーソル移動
            curIdx++;
            if (curIdx >= gameSeq->numMenu) curIdx = 0;
            // メニューリスト再表示
            DispMenuMsgWithCursor(gameSeq, curIdx, NULL);
        }
        
        // Vブランク待ち処理
        GameWaitVBlankIntr();
    }
    
    return returnSeq;
}

/*---------------------------------------------------------------------------*
  接続形態を文字列で取得
 *---------------------------------------------------------------------------*/
static const char* GetTopologyToString(void)
{
    if(s_topologyType == DWC_TOPOLOGY_TYPE_HYBRID) return "Hybrid";
    else if(s_topologyType == DWC_TOPOLOGY_TYPE_FULLMESH) return "Fullmesh";
    else if(s_topologyType == DWC_TOPOLOGY_TYPE_STAR) return "Star";
    
    return "Unknown";  // こないと思うのだが...
}

/*---------------------------------------------------------------------------*
  ログイン後モードの追加表示用コールバック関数
 *---------------------------------------------------------------------------*/
static void BackupModeDispCallback(int curIdx, void* param)
{
#pragma unused(param)
#pragma unused(curIdx)
    OS_TPrintf("\n BACKUP: %s\n", stBackupTypeTable[stNumBackup].comment);
}

/*---------------------------------------------------------------------------*
  締め切り完了コールバック
 *---------------------------------------------------------------------------*/
static void SuspendCallback( DWCSuspendResult result, BOOL suspend, void* data )
{
	(void)data;
	OS_TPrintf( "Suspend Callback result:%d suspend:%s\n",
	            result, suspend ? "TRUE" : "FALSE" );
}

/*---------------------------------------------------------------------------*
  Vブランク待ち関数
 *---------------------------------------------------------------------------*/
static void GameWaitVBlankIntr(void)
{
    int i;

    dbs_DemoUpdate();

    // 想定するゲームフレームに合わせてVブランク待ちの回数を変える
    for (i = 0; i < GAME_FRAME; i++){
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }

    Heap_Debug();
    dbs_DemoLoad();

    // スタック溢れチェック
    OS_CheckStack(OS_GetCurrentThread());
}


/*---------------------------------------------------------------------------*
  Vブランク割り込み関数
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  本デモ用 DTUD 関数(必要なものだけ)
 *---------------------------------------------------------------------------*/
static DWCFriendData* DTUD_GetFriendList( void )
{
    return stGameCnt.friendList.keyList;
}
