#include <nitro.h>
#include <dwc.h>
#ifndef SDK_TWL
#include  <cstring>
#endif

#include "main.h"
#include "init.h"
#include "utility.h"
#include "userdata.h"
#include "screen.h"

#include "dbs.h"

/** --------------------------------------------------------------------
  defines
  ----------------------------------------------------------------------*/
#define INITIAL_CODE    'NTRJ'     // このサンプルが仕様するイニシャルコード
#define GAME_NAME        "dwctest" // このサンプルが使用するゲーム名
#define GAME_SECRET_KEY  "d4q9GZ"  // このサンプルが使用するシークレットキー
#define GAME_PRODUCTID   10824      // このサンプルが使用するプロダクトID
#define	MAX_PLAYERS		 4	       // 自分も含めた接続人数

#define APP_CONNECTION_KEEPALIVE_TIME 30000 // キープアライブ時間
#define KEEPALIVE_INTERVAL (APP_CONNECTION_KEEPALIVE_TIME/5) // キー入力を待たずデータを転送する時間
//#define USE_RELIABLE	// Reliable 通信を行う場合は定義しておいてください
//#define USE_AUTHSERVER_PRODUCTION // 製品向け認証サーバを使用する場合有効にする

/** --------------------------------------------------------------------
  globals
  ----------------------------------------------------------------------*/
KeyControl		g_KeyCtrl;

/** --------------------------------------------------------------------
  statics
  ----------------------------------------------------------------------*/
static DWCInetControl			s_ConnCtrl;
static int  s_state  = state_init;
static BOOL s_logined;
static BOOL s_matched;
static BOOL s_canceled;
static BOOL s_automatch = FALSE;
static u32  s_groupID = 0;

// 接続形態
static DWCTopologyType s_topologyType = DWC_TOPOLOGY_TYPE_HYBRID;
//static DWCTopologyType s_topologyType = DWC_TOPOLOGY_TYPE_STAR;

static int  s_profileID = 0;

const int	SIZE_SEND_BUFFER = 512;
const int	SIZE_RECV_BUFFER = 4 * 1024;

static u8	s_send_buf[ SIZE_SEND_BUFFER ];
static u8	s_recv_buf[ MAX_PLAYERS ][ SIZE_RECV_BUFFER ];
static u8   userData[4]={ 0,5,120,254 };

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

/** --------------------------------------------------------------------
  static functions
  ----------------------------------------------------------------------*/
static int update_init( void );
static int update_connect( void );
static int update_login( void );
static int update_online( void );
static int update_matching( void );
static int update_matched( void );
static int update_disconnect( void );
static int update_save( void );
static int update_error( void );

static void cb_login( DWCError error, int profileID, void* param );
static void cb_updateServers(DWCError error, BOOL isChanged, void* param);
static void cb_newclient(int index, void* param );
static void cb_connect(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param );
static void cb_send( int size, u8 aid, void* param );
static void cb_recv( u8 aid, u8* buffer, int size, void* param );
static void cb_close( DWCError error, BOOL isLocal, BOOL isServer, u8  aid, int index, void* param );
static void cb_suspend( DWCSuspendResult result, BOOL suspend, void* data );

static void DispMessage( const char* main_msg, const char* msg_a, const char* msg_b, const char* msg_x, const char* msg_y,  const char* msg_l );

/*---------------------------------------------------------------------------*
  Vブランク割り込み関数
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    //---- upload pseudo screen to VRAM
    DC_FlushRange(gScreen, sizeof(gScreen));
    GX_LoadBG0Scr(gScreen, 0, sizeof(gScreen));

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


/** --------------------------------------------------------------------
  初期化処理
  ----------------------------------------------------------------------*/
static void
initFunctions( void )
{
	OS_Init();
    CARD_Init();
    OS_SetThreadStackWarningOffset(OS_GetCurrentThread(), 0x100);
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

    Heap_Init();
    Heap_SetDebug( TRUE );

    // 出力初期化
    InitScreen();
    dbs_DemoInit();
    DTDB_SetPrintOverride( TRUE );

    // キー入力初期化
    InitKeyData( &g_KeyCtrl );

    // 表示開始
    OS_WaitVBlankIntr();              // Waiting the end of VBlank interrupt
    GX_DispOn();
    GXS_DispOn();
}

/** --------------------------------------------------------------------
  
  ----------------------------------------------------------------------*/

/** --------------------------------------------------------------------
  1フレーム1回呼ばれる関数
  ----------------------------------------------------------------------*/
static void
update( void )
{
    int errcode;
    DWCError err;
    DWCErrorType errtype;

    switch ( s_state )
    {
    case state_init:			s_state = update_init();			break;
    case state_connect:			s_state = update_connect();			break;
    case state_login:			s_state = update_login();			break;
    case state_online:			s_state = update_online();			break;
    case state_matching:		s_state = update_matching();		break;
    case state_matched:			s_state = update_matched();			break;
    case state_disconnect:		s_state = update_disconnect();		break;
    case state_save:            s_state = update_save();            break;
    case state_error:			s_state = update_error();			break;
    }

    err = DWC_GetLastErrorEx(&errcode, &errtype);

    if ( err != DWC_ERROR_NONE && s_state != state_error )
    {
        // エラーが発生した場合、ここでは状態をエラー解除待ちにするだけ
        OS_TPrintf( "\n*******************************\n" );
        OS_TPrintf( "ERROR : %d : %d\n", err, errcode );
        OS_TPrintf( "Push START BUTTON to continue\n" );
        OS_TPrintf( "*******************************\n\n" );
        s_state = state_error;
    }
}

/** --------------------------------------------------------------------
  インターネットに接続
  ----------------------------------------------------------------------*/
static int
update_init( void )
{
    // DS本体とDSカードのユーザデータが一致するか
    if ( DWC_CheckValidConsole( DTUD_GetUserData() ) )
    {
        DispMessage( "STATE : CONNECTING", NULL, NULL, NULL, NULL, NULL);
        
        DWC_InitInet( &s_ConnCtrl );
        
        DWC_ConnectInetAsync();
        
        return state_connect;
    }
    else
    {
        // 一致していない場合はインターネットに接続できない
        OS_TPrintf( "UserData is not Valid Console.\n" );
        return state_init;
    }
    
    return state_init;
    
}

/** --------------------------------------------------------------------
  接続中
  ----------------------------------------------------------------------*/
static int
update_connect( void )
{
    DWC_ProcessInet();

    if ( DWC_CheckInet() )
    {
        int status;

        status = DWC_GetLastError(NULL);

        if ( status == DWC_ERROR_NONE )
        {
            OS_TPrintf("Connected!..start FriendsMatch.\n");

            s_logined = FALSE;

            DWC_InitFriendsMatch(DTUD_GetUserData(),
                                 GAME_PRODUCTID, GAME_SECRET_KEY,
                                 0, 0,
                                 DTUD_GetFriendList(), FRIEND_LIST_LEN);

            // 認証用関数を使ってログイン
            DWC_LoginAsync(L"なまえ", NULL, cb_login, NULL);

            return state_login;
        }
        else
        {
            OS_TPrintf("Error\n");
            return state_disconnect;
        }
    }

    return state_connect;
}

/** --------------------------------------------------------------------
  ログイン中
  ----------------------------------------------------------------------*/
int
update_login( void )
{
    DWC_ProcessFriendsMatch();  // DWC通信処理更新

    if ( s_logined )
    {
        // ログイン成功
        DispMessage( "STATE : ONLINE", "friend matching", "shutdown", 
        	s_groupID ? "groupID reconnect" : NULL,
        	"friend state", "save firned data");

        // ingamesnチェックの結果を取得する
	    if( DWC_GetIngamesnCheckResult() == DWC_INGAMESN_INVALID )
        {
            // 不適切な名前と判断された場合は特別な処理が必要
		    OS_TPrintf("BAD ingamesn is detected by NAS.\n");
        }

        s_matched  = FALSE;
        s_canceled = FALSE;

        return state_online;
    }

    return state_login;
}

/** --------------------------------------------------------------------
  オンライン中
  ----------------------------------------------------------------------*/
int
update_online( void )
{
    
    DWC_ProcessFriendsMatch();  // DWC通信処理更新

	// これはテスト用
	// 最初はキーを押さずにマッチング開始します
	if(s_automatch)
	{
		g_KeyCtrl.trg = PAD_BUTTON_A;
		s_automatch = FALSE;
	}

    if ( g_KeyCtrl.trg & PAD_BUTTON_A )
    {
        // Aボタンで友達指定ピアマッチメイク開始
//        DWC_ConnectToFriendsAsync(NULL,
//                                  0,
//                                  MAX_PLAYERS,
//                                  TRUE,//FALSE,
//                                  cb_connect, NULL, NULL, NULL );

	    DWC_ConnectToFriendsAsync(	s_topologyType,	// 接続形態
	    							NULL,
	    							0,
	                               (u8)MAX_PLAYERS,
	                               cb_connect,
	                               NULL,
	                               cb_newclient,
	                               NULL,
	                               NULL,
	                               NULL,
	                               NULL,
	                               NULL,
	                               NULL );



        DispMessage( "STATE : MATCHING", NULL, "cancel", NULL, NULL, NULL );

        return state_matching;
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_B )
    {
        // Bボタンで通信を切断し、初期状態に戻る。
        DWC_ShutdownFriendsMatch();
        return state_disconnect;
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_X )
    {
    	// X ボタンでグループID 接続を試す
    	if(s_groupID)
		{
            if( DWC_ConnectToGameServerByGroupID( s_topologyType,	// 接続形態
                                              s_groupID,		// グループID
                                              cb_connect,
                                              NULL,
                                              cb_newclient,
                                              NULL,
                                              NULL,
                                              userData,
                                              NULL ))
            {
                DispMessage( "STATE : MATCHING", NULL, "cancel", NULL, NULL, NULL );
                return state_matching;
            }
		}
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_Y )
    {
        DTUD_DispFriendList();
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_L )
    {
        DispMessage( "SAVE DATA?", "save", "cancel", NULL, NULL, NULL );
        return state_save;
    }
    return state_online;
}

/** --------------------------------------------------------------------
  マッチメイク中
  ----------------------------------------------------------------------*/
static int
update_matching( void )
{
    DWC_ProcessFriendsMatch();  // DWC通信処理更新

    if ( g_KeyCtrl.trg & PAD_BUTTON_B )
    {
        // Bボタンでマッチメイクをキャンセルする
        DWC_CloseAllConnectionsHard();
        s_canceled = TRUE;
    }

    if ( s_matched )
    {
        DispMessage( "STATE : MATCHED\n CrossKey : send position", 
                     "send all", "shutdown", "suspend ON", "suspend OFF", NULL );
        
        return state_matched;
    }
    else if ( s_canceled )
    {
        // キャンセルされたらオンライン状態に戻る
		DispMessage( "STATE : ONLINE", "friend matching", "shutdown",
			s_groupID ? "groupID reconnect" : NULL,
			"friend state", "save firned data");                     
        OS_TPrintf("Canceled connection.\n");

        s_canceled = FALSE;

        return state_online;
    }

    return state_matching;
}

/** --------------------------------------------------------------------
  マッチメイク終了
  ----------------------------------------------------------------------*/
static int
update_matched( void )
{
    struct Position sendPos;
    u8 me = DWC_GetMyAID();
    
    DWC_ProcessFriendsMatch();  // DWC通信処理更新

    {
        // キープアライブでのタイムアウトがあるため、
        // 受信タイムアウト時間より短い間隔で、強制的にデータ送信する。
        static int lastKeepAliveSent = 0;
        int now = (int)DWCi_Np_TicksToMilliSeconds(DWCi_Np_GetTick());
        if (lastKeepAliveSent == 0 || (now - lastKeepAliveSent) > KEEPALIVE_INTERVAL)
        {
            g_KeyCtrl.trg |= PAD_BUTTON_A;
            lastKeepAliveSent = now;
        }
    }
    
    if ( g_KeyCtrl.trg & (PAD_KEY_UP | PAD_BUTTON_A))	s_position[ me ].ypos--;
    if ( g_KeyCtrl.trg & PAD_KEY_DOWN )    				s_position[ me ].ypos++;
    if ( g_KeyCtrl.trg & PAD_KEY_LEFT )    				s_position[ me ].xpos--;
    if ( g_KeyCtrl.trg & PAD_KEY_RIGHT )   				s_position[ me ].xpos++;

    if ( g_KeyCtrl.trg & (PAD_BUTTON_A |
                          PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT))
    {
        sendPos.xpos = (s32)DWCi_HtoLEl((u32)s_position[ me ].xpos);
        sendPos.ypos = (s32)DWCi_HtoLEl((u32)s_position[ me ].ypos);
        // すべての相手に自分の位置を送信
#ifdef USE_RELIABLE
        DWC_SendReliableBitmap( DWC_GetAIDBitmap(),
                                (const void*)&sendPos,
                                sizeof( s32 ) * 2 );
#else
        DWC_SendUnreliableBitmap( DWC_GetAIDBitmap(),
                                  (const void*)&sendPos,
                                  sizeof( s32 ) * 2 );
#endif
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_B )
    {
        // Bボタンで通信を切断し、初期状態に戻る。
        DWC_ShutdownFriendsMatch();
        return state_disconnect;
    }
    else if ( g_KeyCtrl.trg & (PAD_BUTTON_Y | PAD_BUTTON_X))
    {
    	BOOL suspend = g_KeyCtrl.trg & PAD_BUTTON_X ? TRUE : FALSE;
    	
    	// X, Yボタンで締め切り処理開始
    	if(DWC_RequestSuspendMatchAsync(suspend, cb_suspend, NULL))
    	{
    		OS_TPrintf( "Suspend %s Start.\n", suspend ? "TRUE" : "FALSE" );
    	}
    }

    return state_matched;
}

/** --------------------------------------------------------------------
  切断された。
  ----------------------------------------------------------------------*/
static int
update_disconnect( void )
{
    if ( DWC_CleanupInetAsync() )
    {
        return state_init;
    }

    return state_disconnect;
}

/** --------------------------------------------------------------------
  データをセーブする。
  ----------------------------------------------------------------------*/
static int
update_save( void )
{
    DWC_ProcessFriendsMatch();  // DWC通信処理更新
    
    if ( g_KeyCtrl.trg & PAD_BUTTON_A )
    {
        if( DWC_CheckDirtyFlag( DTUD_GetUserData() ) ){
            DTUD_SaveUserData();
            OS_TPrintf( "Saved User Data.\n" );
        }
        
        DTUD_SaveFriendList();
        OS_TPrintf( "Saved Friend Data.\n" );

		DispMessage( "STATE : ONLINE", "friend matching", "shutdown",
			s_groupID ? "groupID reconnect" : NULL,
			"friend state", "save firned data");                     
        return state_online;
    }
    else if ( g_KeyCtrl.trg & PAD_BUTTON_B )
    {
		DispMessage( "STATE : ONLINE", "friend matching", "shutdown",
			s_groupID ? "groupID reconnect" : NULL,
			"friend state", "save firned data");                     
        return state_online;
    }
    return state_save;
}

/** --------------------------------------------------------------------
  エラー解除待ち
  ----------------------------------------------------------------------*/
static int update_error( void )
{
    int errcode;
    DWCError err;
    DWCErrorType errtype;
    
    if ( g_KeyCtrl.trg & PAD_BUTTON_START )
    {
        // スタートボタンでエラー解除
        err = DWC_GetLastErrorEx(&errcode, &errtype);

        DWC_ClearError();
        
        if ( errtype == DWC_ETYPE_DISCONNECT )
        {
            // 通信切断時はFriendsMatch処理の終了とインターネット接続の
            // Clearupを行なう
            DWC_ShutdownFriendsMatch();
            return state_disconnect;
        }
        else if ( errtype == DWC_ETYPE_FATAL )
        {
            // Fatal Errorなので終了
            OS_Panic("++FATAL ERROR\n");
            return state_init;  // not reached;
        }
        else if ( errtype == DWC_ETYPE_LIGHT )
        {
            // 軽いエラーは何もせずにオンライン状態に戻る
            DispMessage( "STATE : ONLINE", "friend matching", "shutdown",
                s_groupID ? "groupID reconnect" : NULL,
                "friend state", "save firned data");
            return state_online;
        }
        else {
            // FriendsMatch処理のみ終了したら、再びFrientsMatch初期化を行なう
            DWC_ShutdownFriendsMatch();
            return state_connect;
        }
    }

    return state_error;
}


/** --------------------------------------------------------------------
  ----------------------------------------------------------------------*/
/** --------------------------------------------------------------------
  コールバック関数
  ----------------------------------------------------------------------*/
static void
cb_login(DWCError error, int profileID, void* param)
{
    (void)param;
    if (error == DWC_ERROR_NONE)
    {
        // 認証成功、プロファイルID取得
        OS_TPrintf("Login succeeded. profileID = %u\n\n", profileID);

        s_profileID = profileID;  // プロファイルIDを記録

        // stUserDataが更新されているかどうかを確認。
        if ( DTUD_CheckUpdateUserData() )
        {
            DTUD_SaveUserData();
        }

        // 友達リストとGameSpyサーバ上のバディリストの同期処理
        DWC_UpdateServersAsync(NULL,
                               cb_updateServers, NULL,
                               NULL, NULL,
                               NULL, NULL);

		// キープアライブ時間の設定        
        DWC_SetConnectionKeepAliveTime(APP_CONNECTION_KEEPALIVE_TIME);
        
        // コネクションクローズコールバックを設定
        DWC_SetConnectionClosedCallback(cb_close, NULL);

        // 送信コールバックを設定
        DWC_SetUserSendCallback( cb_send, &s_commData );

        // 受信コールバックを設定
        DWC_SetUserRecvCallback( cb_recv, &s_commData );
    }
    else
    {
        OS_TPrintf("Login Error\n");
    }
}

static void
cb_updateServers(DWCError error, BOOL isChanged, void* param)
{
    (void)param;
    (void)isChanged;

    if ( error == DWC_ERROR_NONE )
    {
        // 友達リスト同期処理成功
        OS_TPrintf( "Update Servers succeeded.\n" );
        
        s_logined = TRUE;
    }
    else
    {
        OS_TPrintf("Error\n");
    }
}

static void
cb_newclient(int index, void* param )
{
    (void)param;

    OS_TPrintf( "New Client is connecting(idx %d)...\n", index);
}

static void
cb_connect(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param )
{
    (void)param;
    (void)isServer;
    (void)index;
    
    OS_TPrintf( "Matching err:%d, cancel:%d\n", error, cancel );
    
    if ( error == DWC_ERROR_NONE )
    {
        // 受信バッファを設定
        if ( !cancel )
        {
            int i, num;
            u8* pAidList;

            OS_TPrintf("Connected to friend!\n");
            
            num = DWC_GetAIDList(&pAidList);
            for (i = 0; i < num; i++)
            {
                if (pAidList[i] == DWC_GetMyAID())
                {
                    continue;
                }

                // 自分のAID以外の受信バッファをセットする
                DWC_SetRecvBuffer( pAidList[i], &s_recv_buf[pAidList[i]], SIZE_RECV_BUFFER );
            }

            s_matched  = TRUE;
            
            // グループID を保存
            s_groupID = DWC_GetGroupID();
        }
        else
        {
            s_canceled = TRUE;
        }
    }
    else
    {
        OS_TPrintf("Matching Error\n");
    }
}

static void
cb_send( int size, u8 aid, void* param )
{
    CommunicationData* commData = (CommunicationData*)param;
    commData->count++;  // 送信したら加算
    OS_TPrintf( "Send Callback aid:%d, size:%d mycount:%d\n", aid, size, commData->count );
}

static void
cb_recv( u8 aid, u8* buffer, int size, void* param )
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

static void
cb_close(DWCError error, BOOL isLocal, BOOL isServer, u8  aid, int index, void* param)
{
    (void)isLocal;
    (void)isServer;
    (void)param;

    OS_TPrintf( "Closed Callback err:%d, aid:%d, idx:%d\n",
               error, aid, index );
}

static void
cb_suspend( DWCSuspendResult result, BOOL suspend, void* data )
{
	(void)data;
	OS_TPrintf( "Suspend Callback result:%d suspend:%s\n",
	            result, suspend ? "TRUE" : "FALSE" );
}

/** -------------------------------------------------------------------------
  ---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         DispMessage

  Description:  汎用のメッセージを表示する。

  Arguments:    main_msg - 現在のモード
                msg_a    - Aボタンが押された時の処理
                msg_b    - Bボタンが押された時の処理
                msg_x    - Xボタンが押された時の処理
                msg_y    - Yボタンが押された時の処理
                msg_l    - Lボタンが押された時の処理

  Returns:      NONE.
 *---------------------------------------------------------------------------*/
static void
DispMessage( const char* main_msg, const char* msg_a, const char* msg_b, const char* msg_x, const char* msg_y,  const char* msg_l )
{
    s16 x = 1;
    s16 y = 5;

    ClearScreen();
    
    PrintString( 0, 0, 0xf, "================================" );
    PrintString( 1, 1, 0xf, "*** FRIEND MATCHING SAMPLE ***" );
    PrintString( 0, 2, 0xf, "================================" );
    
    if ( main_msg == NULL ) return;

    PrintString( 0, y, 0xf, "--------------------------------" );
    y++;
    // メインメッセージ表示
    while ( *main_msg != '\0' )
    {
        if ( *main_msg == '\n' )
        {
            x = 1;  y++;
        }
        else
        {
            PrintString( x, y, 0xf, "%c", *main_msg );
            x++;
        }
        main_msg++;
    }
    y++;
    PrintString( 0, y, 0xf, "--------------------------------" );

    y += 2;

    // Aボタン説明、Bボタン、Xボタン、Yボタン説明表示
    if ( msg_a != NULL )
    {
        PrintString( 1, y, 0xf, "A BUTTON:%s", msg_a );
    }
    y++;
    if ( msg_b != NULL )
    {
        PrintString( 1, y, 0xf, "B BUTTON:%s", msg_b );
    }
    y += 2;
    if ( msg_x != NULL )
    {
        PrintString( 1, y, 0xf, "X BUTTON:%s", msg_x );
    }
    y++;
    if ( msg_y != NULL )
    {
        PrintString( 1, y, 0xf, "Y BUTTON:%s", msg_y );
    }
    y += 2;
    if ( msg_l != NULL )
    {
        PrintString( 1, y, 0xf, "L BUTTON:%s", msg_l );
    }
}

/** --------------------------------------------------------------------
  毎フレーム呼び出す、状態表示関数
  ----------------------------------------------------------------------*/
void
drawConsole( void )
{
    static int i = 0;
    char loop[] = "/|\\-";

    dbs_Print( 0, 0, "s:%d", DWC_GetMatchState() );
    dbs_Print( 13, 0, "n:%d", DWC_GetNumConnectionHost() );
    dbs_Print( 0, 1, "w:%d", DWC_GetLinkLevel() );
    dbs_Print( 13, 1, "p:%d", s_profileID );
    dbs_Print( 30,0, "%c", loop[((i)>>2)&3] );
    dbs_Print( 0, 2, "g:%d", s_groupID );

    i++;
}

/** --------------------------------------------------------------------
  エントリポイント
  ----------------------------------------------------------------------*/
void NitroMain ()
{
    int ret;
    
    initFunctions();

    // デバッグ表示レベル指定
    DWC_SetReportLevel((u32)( DWC_REPORTFLAG_ALL /*&
                              ~DWC_REPORTFLAG_QR2_REQ*/ ));
    
    // DWCライブラリ初期化
#if defined( USE_AUTHSERVER_PRODUCTION )
    ret = DWC_InitForProduction( GAME_NAME, INITIAL_CODE, DTDB_GetAllocFunc(), DTDB_GetFreeFunc() );
#else
    ret = DWC_InitForDevelopment( GAME_NAME, INITIAL_CODE, DTDB_GetAllocFunc(), DTDB_GetFreeFunc() );
#endif
    
    OS_TPrintf( "DWC_InitFor*() result = %d\n", ret );

    if ( ret == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        OS_TPrintf( "Wi-Fi setting might be broken.\n" );
    }

    // ユーザデータ読み込み
    DTUD_Init();

    while (1)
    {
        drawConsole();

        ReadKeyData();

        update();

        dbs_DemoUpdate();
        OS_WaitIrq(TRUE, OS_IE_V_BLANK);
        Heap_Debug();
        dbs_DemoLoad();
    }

    OS_Terminate();
}
