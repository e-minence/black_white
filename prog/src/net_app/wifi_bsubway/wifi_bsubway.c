//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   wifi_bsubway.c
 *  @brief  Wi-Fiバトルサブウェイ
 *  @author tomoya takahashi
 *  @date   2010.02.17
 *
 *  モジュール名：WIFI_BSUBWAY
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include <dpw_bt.h>

#include "arc_def.h"


#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_error.h"
#include "net/dpw_rap.h"
#include "net/nhttp_rap.h"
#include "net/nhttp_rap_evilcheck.h"
#include "net/dwc_tool.h"

#include "system/net_err.h"
#include "system/main.h"
#include "system/time_icon.h"


#include "sound/pm_sndsys.h"

#include "msg/msg_wifi_system.h"

#include "savedata/wifilist.h"
#include "savedata/wifihistory.h"
#include "savedata/mystatus.h"
#include "savedata/system_data.h"
#include "savedata/bsubway_savedata.h"
#include "savedata/my_pms_data.h"

#include "app/codein.h"

#include "net_app/union/union_beacon_tool.h"


#include "net_app/wifi_bsubway.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "net_app/wifi_country.h"
#include "net_app/connect_anm.h"


#include "wifi_bsubway_graphic.h"


#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "print/global_msg.h"

#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"


#include "message.naix"
#include "msg/msg_wifi_bsubway.h"


#include "wifi_login.naix"

#include "battle/battle.h"

#include "battle/bsubway_battle_data.h"


FS_EXTERN_OVERLAY( dpw_common );

#ifdef PM_DEBUG

#define WIFI_BSUBWAY_Printf( ... )  OS_TPrintf( __VA_ARGS__ )

//#define DOWNLOAD_PRINT  // だうんろーど情報を出力

#else

#define WIFI_BSUBWAY_Printf( ... )  ((void)0)

#endif

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// メインシーケンス
//=====================================
enum {

  BSUBWAY_SEQ_BGMOUT,        // BGM変更処理
  BSUBWAY_SEQ_BGMCHANGE,        // BGM変更処理
  
  BSUBWAY_SEQ_LOGIN,        // Login処理
  BSUBWAY_SEQ_LOGIN_WAIT,   // Login処理
  
  BSUBWAY_SEQ_FADEIN,       // フェードイン
  BSUBWAY_SEQ_FADEINWAIT,   // フェードインウエイト

  BSUBWAY_SEQ_INIT,       // 初期化
  BSUBWAY_SEQ_INIT_WAIT,  // 初期化ウエイト
  BSUBWAY_SEQ_INIT_MSG_WAIT, // 初期化メッセージウエイト
  BSUBWAY_SEQ_PERSON_NAME_SETUP,// 名前認証
  BSUBWAY_SEQ_PERSON_NAME_SETUP_WAIT,// 名前認証
  BSUBWAY_SEQ_PERSON_SETUP,// Email認証
  BSUBWAY_SEQ_PERSON_SETUP_WAIT,// Email認証
  BSUBWAY_SEQ_MAIN,       // メイン処理
  BSUBWAY_SEQ_END,        // 終了　＆　フェードアウト
  BSUBWAY_SEQ_END_WAIT,   // 終了ウエイト

  BSUBWAY_SEQ_LOGOUT,     // Logout処理

  BSUBWAY_SEQ_BGMFADE,      // BGMFADE
  BSUBWAY_SEQ_BGMFADE_WAIT, // BGMFADE WAIT

  BSUBWAY_SEQ_ALL_END,   // 終了

  BSUBWAY_SEQ_ERROR,      // エラー処理
  BSUBWAY_SEQ_ERROR_WAIT, // エラーウエイト
} ;


//-------------------------------------
/// サブシーケンス
//=====================================
enum{
  // 記録のアップロード
  SCORE_UPLOAD_SEQ_PERSON_MSG_00 = 0,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP01,
  SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT01,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE,
  SCORE_UPLOAD_SEQ_PERSON_UPDATE_WAIT,
  SCORE_UPLOAD_SEQ_SAVE,
  SCORE_UPLOAD_SEQ_SAVE_WAIT,
  SCORE_UPLOAD_SEQ_PERSON_MSG_01,
  SCORE_UPLOAD_SEQ_PERSON_END,

  // ゲーム情報のダウンロード
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK = 0,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_SAVE,
  SCORE_UPLOAD_SEQ_GAMEDATA_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_MSG_02,
  SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END,

  SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG,
  SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG_WAIT,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO,
  SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_WAIT,

  // 歴代情報のダウンロード
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00 = 0,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_01,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEIN_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_SAVE,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_03,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END,

  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG_WAIT,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO,
  SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_WAIT,
};

//-------------------------------------
/// メイン処理　戻り値
//=====================================
typedef enum {
  BSUBWAY_MAIN_RESULT_CONTINUE,
  BSUBWAY_MAIN_RESULT_OK,
  BSUBWAY_MAIN_RESULT_CANCEL,

  BSUBWAY_MAIN_RESULT_MAX,
} BSUBWAY_MAIN_RESULT;



//-------------------------------------
/// NHTTPのエラー分岐
//=====================================
typedef enum{
  BSUBWAY_NHTTP_ERROR_NONE,
  BSUBWAY_NHTTP_ERROR_POKE_ERROR,   // ポケモンが不正
  BSUBWAY_NHTTP_ERROR_DISCONNECTED, // 正常終了しなかった
  BSUBWAY_NHTTP_ERROR_EVILERROR,   //不正検査サーバのエラー

  BSUBWAY_NHTTP_ERROR_MAX,
} BSUBWAY_NHTTP_ERROR;


///強制タイムアウトまでの時間
#define BSUBWAY_TIMEOUT_TIME      (30*60*2) //2分


//-------------------------------------
///	
//=====================================
typedef enum {
  BSUBWAY_UPLOADCHECK_RESULT_WAIT,  // 認証中
  BSUBWAY_UPLOADCHECK_RESULT_OK,    // 結果OK
  BSUBWAY_UPLOADCHECK_RESULT_NG,    // 結果NG
} BSUBWAY_UPLOADCHECK_RESULT;



//-------------------------------------
/// 描画関係
//=====================================
// 描画フレーム
enum
{
  VIEW_FRAME_SUB_BACKGROUND = GFL_BG_FRAME0_S,
  VIEW_FRAME_MAIN_BACKGROUND = GFL_BG_FRAME0_M,
  VIEW_FRAME_MAIN_BACKGROUND_BASE = GFL_BG_FRAME3_M,
  VIEW_FRAME_MAIN_WIN = GFL_BG_FRAME1_M,
  VIEW_FRAME_MAIN_YESNO = GFL_BG_FRAME2_M,
};

// 数値桁
enum
{
  VIEW_NUMBER_KETA_ROOM_NO  = 3,
  VIEW_NUMBER_KETA_RANK     = 2,
};


// BGパレット
enum
{
  VIEW_PAL_MSG = 12,
  VIEW_PAL_WIN_FRAME = 13,
};

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// PersonalData
//=====================================
typedef struct 
{
  BOOL setup;
  Dpw_Common_Profile        dc_profile;         // 自分の情報登録用構造体
  Dpw_Common_ProfileResult  dc_profile_result;  // 自分の情報登録レスポンス用構造体
  Dpw_Bt_Player bt_player;              // 勝ち抜いた自分のデータ
  const BSUBWAY_WIFI_DATA* cp_bsubway_wifi; // バトルサブウェイ　WiFiセーブ情報
  const BSUBWAY_SCOREDATA* cp_bsubway_score; // バトルサブウェイ　WiFiセーブ情報


  // ポケモン認証　署名
  NHTTP_RAP_WORK* p_nhttp;  // ポケモン認証ワーク
  u16 poke_num;
  u8 sign[NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN];

  //以下不正文字チェック用
  DWC_TOOL_BADWORD_WORK badword;
  MYSTATUS              *p_use_mystatus;

} WIFI_BSUBWAY_PERSONAL;


//-------------------------------------
/// RoomData
//=====================================
typedef struct 
{
  BOOL setup;
  BOOL in_roomdata;
  
  u16 rank;
  u16 room_no;
  u16 room_num;

  Dpw_Bt_Room   bt_roomdata;            // ダウンロード部屋情報


} WIFI_BSUBWAY_ROOM;

//-------------------------------------
/// dpw_bt Error
//=====================================
typedef struct {
  DpwBtError          error_code;
  DpwBtServerStatus   server_status;

  u16 timeout_flag;
  s16 timeout;

  BSUBWAY_NHTTP_ERROR nhttp_error;
  s32 response;
} WIFI_BSUBWAY_ERROR;

//-------------------------------------
/// 描画管理ワーク
//=====================================
typedef struct {
  BOOL setup;

  HEAPID heapID;
  
  // 描画システム
  WIFI_BSUBWAY_GRAPHIC_WORK * p_graphic;

  // 背景
  CONNECT_BG_PALANM bg_palanm;

  // テキスト表示
  GFL_BMPWIN* p_win;
  BMPMENU_WORK* p_yesno;
  u32 frame_bgchar;
  u32 frame_bgchar_sys;
  TIMEICON_WORK* p_timeicon;
  
  // 文字列情報
  GFL_MSGDATA* p_msgdata;
  WORDSET* p_wordset;
  GFL_FONT* p_font;
  STRBUF* p_strtmp;
  STRBUF* p_str;

  // 文字列描画
  PRINT_STREAM* p_msg_stream;
  GFL_TCBLSYS*  p_msg_tcbsys;

} WIFI_BSUBWAY_VIEW;


//-------------------------------------
/// バトルサブウェイ　WiFi部分ワーク
//=====================================
typedef struct 
{
  // パラメータ
  WIFI_BSUBWAY_PARAM* p_param;

  // 見た目関係
  WIFI_BSUBWAY_VIEW view;

  // Login Logout
  WIFILOGIN_PARAM   login;
  WIFILOGOUT_PARAM  logout;

  // 認証用ワーク
  DWCSvlResult svl_result;

  // DpwMainをまわすフラグ
  u16 dpw_main_do;

  // セーブ中フラグ
  u16 save_do;

  // サブシーケンス
  s32 seq;
  u32 code_input;

  // 部屋情報
  WIFI_BSUBWAY_ROOM roomdata;

  // 人物情報
  WIFI_BSUBWAY_PERSONAL personaldata;

  // DPW BT エラー状態
  WIFI_BSUBWAY_ERROR  bt_error;

  // 数値入力情報
  CODEIN_PARAM* p_codein;

  // 各種情報
  GAMEDATA*     p_gamedata;
  SYSTEMDATA*   p_systemdata;   // システムセーブデータ（DPWライブラリ用ともだちコードを保存）
  WIFI_LIST*    p_wifilist;     // ともだち手帳
  MYSTATUS*     p_mystatus;     // トレーナー情報
  BSUBWAY_WIFI_DATA* p_bsubway_wifi; // バトルサブウェイ　WiFiセーブ情報
  BSUBWAY_SCOREDATA* p_bsubway_score; // バトルサブウェイ　スコアセーブ情報
  
} WIFI_BSUBWAY;

//-----------------------------------------------------------------------------
/**
 *          プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// process
//=====================================
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );
static GFL_PROC_RESULT WiFiBsubway_ProcEnd( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work );

static BOOL WiFiBsubway_IsErrorCheck( const WIFI_BSUBWAY* cp_wk, int seq );


//-------------------------------------
/// RoomData 
//=====================================
// 初期化
static void ROOM_DATA_Init( WIFI_BSUBWAY_ROOM* p_wk );
// ルーム数の取得処理
static void ROOM_DATA_SetUp( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 rank );
static BOOL ROOM_DATA_SetUpWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// ルーム情報の取得処理
static void ROOM_DATA_LoadRoomData( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 room_no );
static BOOL ROOM_DATA_LoadRoomDataWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error );

// 情報の取得
static u16 ROOM_DATA_GetRank( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomNo( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomNum( const WIFI_BSUBWAY_ROOM* cp_wk );
static u16 ROOM_DATA_GetRoomKeta( const WIFI_BSUBWAY_ROOM* cp_wk );
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk );

// ダウンロードデータ　チェック
static void ROOM_DATA_CheckDownloadData( WIFI_BSUBWAY_ROOM* p_wk );

// セーブ処理
static void ROOM_DATA_SavePlayerData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save );
static void ROOM_DATA_SaveLeaderData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save );


//-------------------------------------
/// PersonalData
//=====================================
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk, HEAPID heapID );
static void PERSONAL_DATA_Exit( WIFI_BSUBWAY_PERSONAL* p_wk );
// 人名認証
static void PERSONAL_DATA_SetUpMystatus( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, HEAPID heapID );
static BSUBWAY_UPLOADCHECK_RESULT PERSONAL_DATA_SetUpMystatusWait( WIFI_BSUBWAY_PERSONAL* p_wk, HEAPID heapID );
// 人情報認証
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error );
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error );
static void PERSONAL_DATA_InitDpwPlayerData( Dpw_Bt_Player* p_player, GAMEDATA* p_gamedata, const MYSTATUS* cp_mystatus );

// ポケモン認証
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, DWCSvlResult* p_svl_result, HEAPID heapID );
static BSUBWAY_UPLOADCHECK_RESULT PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, BOOL poke_error_check );
static void PERSONAL_DATA_SetDefaultPokeName( WIFI_BSUBWAY_PERSONAL* p_wk, int index );
static void PERSONAL_DATA_ClearNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk );

// 人情報アップデート
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  );

//-------------------------------------
/// ErrorData
//=====================================
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk );
static BOOL ERROR_DATA_GetAsyncServerResult( WIFI_BSUBWAY_ERROR* p_wk );
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result );
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk );
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk );
static BOOL ERROR_DATA_ReqServerResponseMessage( const WIFI_BSUBWAY_ERROR* cp_wk );
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error );
static void ERROR_DATA_SetResponseError( WIFI_BSUBWAY_ERROR* p_wk, s32 response );
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk );
static void ERROR_DATA_OnlyTimeOutCount( WIFI_BSUBWAY_ERROR* p_wk );


//-------------------------------------
/// CodeIn
//=====================================
static void CODEIN_StartRoomNoProc( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static void CODEIN_StartRankProc( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk );
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str );
static int CODEIN_GetCode( const WIFI_BSUBWAY* cp_wk );

//-------------------------------------
/// Login
//=====================================
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk );
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk );

//-------------------------------------
/// Logout
//=====================================
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk );
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk );

//-------------------------------------
/// メイン処理処理
//=====================================
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID );


//-------------------------------------
/// 接続・切断・エラー
//=====================================
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk );
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk );
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk );



//-------------------------------------
/// View
//=====================================
static void VIEW_Init( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID );
static void VIEW_Exit( WIFI_BSUBWAY_VIEW* p_wk );
static void VIEW_Main( WIFI_BSUBWAY_VIEW* p_wk );

// グラフィックリソースセットアップ
static void VIEW_InitResource( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID );
static void VIEW_ExitResource( WIFI_BSUBWAY_VIEW* p_wk );

// メッセージ描画
static void VIEW_PrintStream( WIFI_BSUBWAY_VIEW* p_wk, u32 msg_id );
static BOOL VIEW_PrintMain( WIFI_BSUBWAY_VIEW* p_wk );

// ワードセット
static void VIEW_SetWordNumber( WIFI_BSUBWAY_VIEW* p_wk, u32 idx, u32 number, u32 keta );

// YesNo
static void VIEW_StartYesNo( WIFI_BSUBWAY_VIEW* p_wk );
static u32 VIEW_MainYesNo( WIFI_BSUBWAY_VIEW* p_wk );

// TimeIcon
static void VIEW_StartTimeIcon( WIFI_BSUBWAY_VIEW* p_wk );
static void VIEW_EndTimeIcon( WIFI_BSUBWAY_VIEW* p_wk );


//-------------------------------------
/// Save
//=====================================
static void SAVE_Start( WIFI_BSUBWAY* p_wk );
static BOOL SAVE_Main( WIFI_BSUBWAY* p_wk );

//-----------------------------------------------------------------------------
/**
 *      process情報
 */
//-----------------------------------------------------------------------------
const GFL_PROC_DATA WIFI_BSUBWAY_Proc = {
  WiFiBsubway_ProcInit,
  WiFiBsubway_ProcMain,
  WiFiBsubway_ProcEnd,
};









//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ  process 初期化
 *
 *  @param  p_proc      processワーク
 *  @param  p_seq       シーケンス
 *  @param  p_param     パラメータ
 *  @param  p_work      ワーク
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<動作継続中
 *  @retval GFL_PROC_RES_FINISH           ///<動作終了
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcInit( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk;

  // DPW_COMMON
  GFL_OVERLAY_Load( FS_OVERLAY_ID( dpw_common ) );


  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFI_BSUBWAY, 0x18000 );

  p_wk = GFL_PROC_AllocWork( p_proc, sizeof(WIFI_BSUBWAY), HEAPID_WIFI_BSUBWAY );
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY) );

  // Wi-FiLoginしてない常態かチェック
  GF_ASSERT( GFL_NET_IsInit() == FALSE );


  // ワーク初期化
  p_wk->p_param = p_param;
  p_wk->p_param->result = WIFI_BSUBWAY_RESULT_NG; // 戻り値をNGで初期化

  // 各種情報の取得
  {
    GAMEDATA* p_gamedata = GAMESYSTEM_GetGameData( p_wk->p_param->p_gamesystem );
    SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
    
    p_wk->p_gamedata    = p_gamedata;
    p_wk->p_systemdata  = SaveData_GetSystemData( p_savedata );
    p_wk->p_wifilist    = GAMEDATA_GetWiFiList( p_gamedata );
    p_wk->p_mystatus    = GAMEDATA_GetMyStatus( p_gamedata );
    p_wk->p_bsubway_wifi  = GAMEDATA_GetBSubwayWifiData( p_gamedata );
    p_wk->p_bsubway_score = GAMEDATA_GetBSubwayScoreData( p_gamedata );
  }

  // システムワーク初期化
  ROOM_DATA_Init( &p_wk->roomdata );
  PERSONAL_DATA_Init( &p_wk->personaldata, HEAPID_WIFI_BSUBWAY );
  ERROR_DATA_Init( &p_wk->bt_error );
  
  // DEBUG出力ON
#ifdef PM_DEBUG
  GFL_NET_DebugPrintOn();
#endif

  return GFL_PROC_RES_FINISH;  
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ  process メイン
 *
 *  @param  p_proc      processワーク
 *  @param  p_seq       シーケンス
 *  @param  p_param     パラメータ
 *  @param  p_work      ワーク
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<動作継続中
 *  @retval GFL_PROC_RES_FINISH           ///<動作終了
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcMain( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk = p_work;

  // 通信エラーチェック
  if( WiFiBsubway_IsErrorCheck( p_wk, (*p_seq) ) ){
    if( ERROR_DATA_IsError( &p_wk->bt_error ) || (NetErr_App_CheckError() != NET_ERR_CHECK_NONE) )
    {
      WIFI_BSUBWAY_Printf( "ProcMain Error Hit\n" );
      WIFI_BSUBWAY_Printf( "BtError %d\n", ERROR_DATA_IsError( &p_wk->bt_error ) );
      WIFI_BSUBWAY_Printf( "NetError %d\n", NetErr_App_CheckError() );
      (*p_seq) = BSUBWAY_SEQ_ERROR;
    }
  }
  
  // メイン処理
  switch( (*p_seq) ){
  case BSUBWAY_SEQ_BGMOUT:        // BGM変更処理
    // BGM
    PMSND_FadeOutBGM( PMSND_FADE_FAST );
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_BGMCHANGE:        // BGM変更処理
    if( PMSND_CheckFadeOnBGM() == FALSE ){
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      PMSND_PlayBGM( SEQ_BGM_WIFI_ACCESS );
      PMSND_FadeInBGM( PMSND_FADE_FAST );
      (*p_seq)++;
    }
    break;
    
  // Login処理
  case BSUBWAY_SEQ_LOGIN:        
    LOGIN_StartProc( p_wk );
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_LOGIN_WAIT:
    {
      WIFILOGIN_RESULT result;
      result = LOGIN_GetResult( p_wk );
      if( result == WIFILOGIN_RESULT_LOGIN ){
        (*p_seq)++;
      }else{
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_CANCEL; //CANCEL！
        (*p_seq) = BSUBWAY_SEQ_BGMFADE;
      }
    }
    break;

  case BSUBWAY_SEQ_FADEIN:       // フェードイン
    VIEW_Init( &p_wk->view, HEAPID_WIFI_BSUBWAY );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
    (*p_seq) ++;
    break;

  case BSUBWAY_SEQ_FADEINWAIT:   // フェードインウエイト
    if( GFL_FADE_CheckFade() == FALSE ){
      (*p_seq) ++;
    }
    break;

  // 初期化
  case BSUBWAY_SEQ_INIT:       
    WiFiBsubway_Connect( p_wk );
    
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_013 );
    VIEW_StartTimeIcon( &p_wk->view );

    (*p_seq) ++;
    break;
 
  // 初期化ウエイト
  case BSUBWAY_SEQ_INIT_WAIT:  
    if( WiFiBsubway_ConnectWait( p_wk ) )
    {
      (*p_seq) ++;
    }
    break;

  // 初期化メッセージウエイト
  case BSUBWAY_SEQ_INIT_MSG_WAIT: 
    if( VIEW_PrintMain( &p_wk->view ) )
    {
      (*p_seq) ++;
    }
    break;

  // 名前認証
  case BSUBWAY_SEQ_PERSON_NAME_SETUP:
    PERSONAL_DATA_SetUpMystatus( &p_wk->personaldata, p_wk->p_gamedata, HEAPID_WIFI_BSUBWAY );
    (*p_seq) ++;
    break;

  // 名前認証
  case BSUBWAY_SEQ_PERSON_NAME_SETUP_WAIT:
    {
      BSUBWAY_UPLOADCHECK_RESULT result;
      result = PERSONAL_DATA_SetUpMystatusWait( &p_wk->personaldata, HEAPID_WIFI_BSUBWAY );
      if( result != BSUBWAY_UPLOADCHECK_RESULT_WAIT ){
        (*p_seq) ++;
      }
    }
    break;

  //Email認証
  case BSUBWAY_SEQ_PERSON_SETUP:
    PERSONAL_DATA_SetUpProfile( &p_wk->personaldata, p_wk->p_gamedata, &p_wk->bt_error );
    (*p_seq) ++;
    break;

  //Email認証ウエイト
  case BSUBWAY_SEQ_PERSON_SETUP_WAIT:
    if( PERSONAL_DATA_SetUpProfileWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      (*p_seq) ++;
    }
    break;

  // メイン処理
  case BSUBWAY_SEQ_MAIN:
    {
      BSUBWAY_MAIN_RESULT result;

      result = WiFiBsubway_Main( p_wk, HEAPID_WIFI_BSUBWAY );
      
      if( result == BSUBWAY_MAIN_RESULT_OK ){
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_OK; //処理成功！
        (*p_seq) ++;
        
      }else if( result == BSUBWAY_MAIN_RESULT_CANCEL ){
        p_wk->p_param->result = WIFI_BSUBWAY_RESULT_CANCEL; //CANCEL終了！
        (*p_seq) ++;
      }
    }
    break;

  // 終了
  case BSUBWAY_SEQ_END:        
    WiFiBsubway_Disconnect( p_wk );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    (*p_seq) ++;
    break;

  // 終了ウエイト
  case BSUBWAY_SEQ_END_WAIT:   
    if( WiFiBsubway_DisconnectWait( p_wk ) && (GFL_FADE_CheckFade() == FALSE) )
    {
      VIEW_Exit( &p_wk->view );
      (*p_seq) ++;
    }
    break;

  // Logout処理
  case BSUBWAY_SEQ_LOGOUT:     
    //エラー表示
    if( NetErr_DispCall( FALSE ) == FALSE ){
      // エラー表示がなければLOGOUT
      LOGOUT_StartProc( p_wk );
    }
    (*p_seq)++;
    break;

  case BSUBWAY_SEQ_BGMFADE:      // BGMFADE
    PMSND_FadeOutBGM( PMSND_FADE_FAST );
    (*p_seq)++;
    break;
    
  case BSUBWAY_SEQ_BGMFADE_WAIT: // BGMFADE WAIT
    if( PMSND_CheckFadeOnBGM() == FALSE ){
      (*p_seq)++;
    }
    break;

  // 終了
  case BSUBWAY_SEQ_ALL_END:   
    return GFL_PROC_RES_FINISH;

  // エラー処理
  case BSUBWAY_SEQ_ERROR:      
    if( WiFiBsubway_Error( p_wk ) ){
      (*p_seq) = BSUBWAY_SEQ_ERROR_WAIT;
    }else{
      (*p_seq) = BSUBWAY_SEQ_END;
    }

    // Error終了のばあい、一応クリーン
    PERSONAL_DATA_ClearNhttpPokemon( &p_wk->personaldata );
    break;

  // エラーウエイト
  case BSUBWAY_SEQ_ERROR_WAIT: 
    if( WiFiBsubway_ErrorWait(p_wk) ){
      (*p_seq) = BSUBWAY_SEQ_END;
    }
    break;
    
  default:
    GF_ASSERT( 0 );
    break;
  }

  // Main dpw_main_doがTRUE
  // のときのみ動作
  if( p_wk->dpw_main_do ){
    Dpw_Bt_Main();
  }
  VIEW_Main( &p_wk->view );


  return GFL_PROC_RES_CONTINUE;  
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ  process 破棄
 *
 *  @param  p_proc      processワーク
 *  @param  p_seq       シーケンス
 *  @param  p_param     パラメータ
 *  @param  p_work      ワーク
 *
 *  @retval GFL_PROC_RES_CONTINUE = 0,    ///<動作継続中
 *  @retval GFL_PROC_RES_FINISH           ///<動作終了
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiBsubway_ProcEnd( GFL_PROC * p_proc, int * p_seq, void * p_param, void * p_work )
{
  WIFI_BSUBWAY* p_wk = p_work;
  
  // 数値入力ワーク破棄
  CODEIN_EndProc( p_wk );

  LOGIN_EndProc( p_wk );
  LOGOUT_EndProc( p_wk );

  // システムワーク破棄
  PERSONAL_DATA_Exit( &p_wk->personaldata );
  VIEW_Exit( &p_wk->view );
  
  // procワーク破棄
  GFL_PROC_FreeWork( p_proc );

  // HEAPID破棄
  GFL_HEAP_DeleteHeap( HEAPID_WIFI_BSUBWAY );

  // DPW_COMMON
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( dpw_common ) );

  //BGM復元
  PMSND_PopBGM();
  PMSND_PauseBGM( FALSE );
  PMSND_FadeInBGM( PMSND_FADE_NORMAL );
  
  // DEBUG出力ON
#ifdef PM_DEBUG
  GFL_NET_DebugPrintOff();
#endif

  return GFL_PROC_RES_FINISH;
}


//-------------------------------------
/// Errorチェック　を行うか？
//=====================================
static BOOL WiFiBsubway_IsErrorCheck( const WIFI_BSUBWAY* cp_wk, int seq )
{
  if( (seq <= BSUBWAY_SEQ_MAIN) && (GFL_FADE_CheckFade() == FALSE) && (cp_wk->dpw_main_do) && (cp_wk->save_do == FALSE) ){
    return TRUE;
  }
  return FALSE;
}


//-------------------------------------
/// RoomData 
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief　RoomData初期化
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_Init( WIFI_BSUBWAY_ROOM* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ROOM) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData  Rankの部屋情報をセットアップ
 *
 *  @param  p_wk    ワーク
 *  @param  rank    ランク
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SetUp( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 rank )
{
  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  p_wk->rank = rank;

  WIFI_BSUBWAY_Printf( "RoomNum Get\n" );

  // ルーム数を取得
  Dpw_Bt_GetRoomNumAsync( rank );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData Rankの部屋情報セットアップ待ち
 *
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL ROOM_DATA_SetUpWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;
  GF_ASSERT( p_wk->setup == FALSE );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  // 受信完了待ち
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // エラー終了でなければ完了
    if( !ERROR_DATA_IsError( p_error ) ){
      // 完了
      p_wk->room_num  = result;
      p_wk->setup     = TRUE;
      return TRUE;
    }
  }

  return FALSE;
}


// ルーム情報の取得処理
//----------------------------------------------------------------------------
/**
 *  @brief  RoomData 部屋情報の取得処理 
 *
 *  @param  p_wk      ワーク
 *  @param  room_no   部屋ナンバー
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_LoadRoomData( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error, u16 room_no )
{
  GF_ASSERT( p_wk->setup );
  GF_ASSERT( p_wk->in_roomdata == FALSE );

  WIFI_BSUBWAY_Printf( "Room Load \n" );

  p_wk->room_no = room_no;
  GF_ASSERT( p_wk->room_no <= p_wk->room_num );
  Dpw_Bt_DownloadRoomAsync( p_wk->rank, p_wk->room_no, &p_wk->bt_roomdata );
  ERROR_DATA_GetAsyncStart( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData 部屋情報　取得ウエイト
 *
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL ROOM_DATA_LoadRoomDataWait( WIFI_BSUBWAY_ROOM* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup );
  if( p_wk->in_roomdata )
  {
    return TRUE;
  }

  // 受信完了待ち
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // エラー終了でなければ完了
    if( !ERROR_DATA_IsError( p_error ) ){
      // 完了
      p_wk->in_roomdata = TRUE;

#ifdef DOWNLOAD_PRINT
      {
        int i;
        int size;
        u8* p_buff;

        
        size = sizeof(Dpw_Bt_Room);

        p_buff = (u8*)&p_wk->bt_roomdata;
        for( i=0; i<size; i++ ){
          if( (i % 4) == 0 ){
            OS_Printf( "\n" );
          }
          // binaryデータの出力
          OS_Printf( "%02x ", p_buff[i] );
        }
      }
#endif  // DOWNLOAD_PRINT
      
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData  rankの取得
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRank( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->rank;
}
//----------------------------------------------------------------------------
/**
 *  @brief  RoomData roomNoの取得
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomNo( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->room_no;
}

//----------------------------------------------------------------------------
/**
 *  @brief  RoomData roomNumの取得
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomNum( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  return cp_wk->room_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  RoomData  桁を取得
 *
 *	@param	cp_wk 
 */
//-----------------------------------------------------------------------------
static u16 ROOM_DATA_GetRoomKeta( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  if( cp_wk->room_num < 100 ){
    return VIEW_NUMBER_KETA_ROOM_NO-1;
  }
  return VIEW_NUMBER_KETA_ROOM_NO;
}


//----------------------------------------------------------------------------
/**
 *  @brief  RoomData 部屋情報の取得
 */
//-----------------------------------------------------------------------------
static const Dpw_Bt_Room* ROOM_DATA_GetRoomData( const WIFI_BSUBWAY_ROOM* cp_wk )
{
  GF_ASSERT( cp_wk );
  GF_ASSERT( cp_wk->setup );
  GF_ASSERT( cp_wk->in_roomdata );
  return &cp_wk->bt_roomdata;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ダウンロードデータをマネージメントする。
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_CheckDownloadData( WIFI_BSUBWAY_ROOM* p_wk )
{
  int i;
  Dpw_Bt_Player* p_player = (Dpw_Bt_Player*)p_wk->bt_roomdata.player;
  Dpw_Bt_Leader* p_leader = (Dpw_Bt_Leader*)p_wk->bt_roomdata.leader;

  for( i=0; i<BSUBWAY_STOCK_WIFI_PLAYER_MAX; i++ ){
    TOMOYA_Printf( "B player %d country %d local %d trainerType %d\n", i, p_player[i].countryCode, p_player[i].localCode, p_player[i].trainerType );
    p_player[i].countryCode = WIFI_COUNTRY_GetNGTestCountryCode( p_player[i].countryCode, p_player[i].localCode, p_player[i].langCode );
    p_player[i].localCode = WIFI_COUNTRY_GetNGTestLocalCode( p_player[i].countryCode, p_player[i].localCode, p_player[i].langCode );

    p_player[i].trainerType = UnionView_GetTrType( UnionView_TrType_to_Index( p_player[i].trainerType ) );//不正な見た目はすべてindex0の見た目になる。
    TOMOYA_Printf( "A player %d country %d local %d trainerType %d\n", i, p_player[i].countryCode, p_player[i].localCode, p_player[i].trainerType );
  }

  for( i=0; i<BSUBWAY_STOCK_WIFI_LEADER_MAX; i++ ){
    TOMOYA_Printf( "B leader %d country %d local %d \n", i, p_leader[i].countryCode, p_leader[i].localCode );
    p_leader[i].countryCode = WIFI_COUNTRY_GetNGTestCountryCode( p_leader[i].countryCode, p_leader[i].localCode, p_leader[i].langCode );
    p_leader[i].localCode = WIFI_COUNTRY_GetNGTestLocalCode( p_leader[i].countryCode, p_leader[i].localCode, p_leader[i].langCode );
    TOMOYA_Printf( "A leader %d country %d local %d \n", i, p_leader[i].countryCode, p_leader[i].localCode );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  プレイヤー情報のセーブ
 *
 *  @param  cp_wk
 *  @param  p_save 
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SavePlayerData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save )
{
  RTCTime time;
  RTCDate date;

  // GameSpyサーバー時間・日付を取得
  DWC_GetDateTime( &date, &time);

  // Playerデータ 
  BSUBWAY_WIFIDATA_SetPlayerData( p_save, (const BSUBWAY_WIFI_PLAYER*)cp_wk->bt_roomdata.player, cp_wk->rank, cp_wk->room_no );

  // 挑戦済み設定
  BSUBWAY_WIFIDATA_SetRoomDataFlag( p_save, cp_wk->rank, cp_wk->room_no, &date );
}

//----------------------------------------------------------------------------
/**
 *  @brief  リーダー履歴のセーブ
 *
 *  @param  cp_wk
 *  @param  p_save 
 */
//-----------------------------------------------------------------------------
static void ROOM_DATA_SaveLeaderData( const WIFI_BSUBWAY_ROOM* cp_wk, BSUBWAY_WIFI_DATA* p_save )
{
#ifdef DEBUG_ONLY_FOR_kagaya
  {
    u32 id;
    int i,j;
    const Dpw_Bt_Leader *pLeader = cp_wk->bt_roomdata.leader;
    
    for( i = 0; i < 30; i++, pLeader++ ){
      WIFI_BSUBWAY_Printf( "----DOWNLOAD LEADER DATA No.%u----\n", i );
      
      WIFI_BSUBWAY_Printf( "playerName" );
      for( j = 0; j < 8; j++ ){
        WIFI_BSUBWAY_Printf( "[%u]=%u ", j, pLeader->playerName[j] );
      }
      WIFI_BSUBWAY_Printf( "\n" );
      
      WIFI_BSUBWAY_Printf( "versionCode=%u\n", pLeader->versionCode );
      WIFI_BSUBWAY_Printf( "langCode=%u\n", pLeader->langCode );
      WIFI_BSUBWAY_Printf( "countryCode=%u\n", pLeader->countryCode );
      WIFI_BSUBWAY_Printf( "localCode=%u\n", pLeader->localCode );
      
      WIFI_BSUBWAY_Printf( "playerId" );
      for( j = 0; j < 4; j++ ){
        WIFI_BSUBWAY_Printf( "[%u]=%u ", j, pLeader->playerId[j] );
      }
      id = (u32)((pLeader->playerId[0])|
            (pLeader->playerId[1]<<8)|
            (pLeader->playerId[2]<<16)|
            (pLeader->playerId[3]<<24));
      WIFI_BSUBWAY_Printf( ":(u32)%d\n", id );
      
      WIFI_BSUBWAY_Printf( "leaderMessage" );
      for( j = 0; j < 8; j++ ){
        WIFI_BSUBWAY_Printf( "[%u]=%u,", j, pLeader->playerId[j] );
      }
      WIFI_BSUBWAY_Printf( "\n" );
      
      WIFI_BSUBWAY_Printf( "flags=%u\n", pLeader->flags );
    }
  }
#endif

  // Leaderデータ 
  BSUBWAY_WIFIDATA_SetLeaderData( p_save, (const BSUBWAY_LEADER_DATA*)cp_wk->bt_roomdata.leader, cp_wk->rank, cp_wk->room_no );
}

//-------------------------------------
/// PersonalData
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  初期化
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_Init( WIFI_BSUBWAY_PERSONAL* p_wk, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_PERSONAL) );

  p_wk->p_use_mystatus = MyStatus_AllocWork( heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  破棄
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_Exit( WIFI_BSUBWAY_PERSONAL* p_wk )
{
  if(p_wk->p_use_mystatus){
    GFL_HEAP_FreeMemory( p_wk->p_use_mystatus );
    p_wk->p_use_mystatus = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Mystatus をセットアップ　（名前チェック込み）
 *
 *	@param	p_wk          ワーク
 *	@param	p_gamedata    ゲームデータ
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpMystatus( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, HEAPID heapID )
{
  MYSTATUS* p_mystatus          = GAMEDATA_GetMyStatus( p_gamedata );
  MyStatus_Copy( p_mystatus, p_wk->p_use_mystatus );

  // 不正名チェック
  {
    STRBUF *p_word_check  = MyStatus_CreateNameString( p_wk->p_use_mystatus, heapID );
    DWC_TOOL_BADWORD_Start( &p_wk->badword, p_word_check, heapID );
    GFL_STR_DeleteBuffer( p_word_check );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  不正名称チェック  待ち
 *
 *	@param	p_wk  ワーク
 *	@param  heapID
 *
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_WAIT,  // 認証中
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_OK,    // 結果OK
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_NG,    // 結果NG
 */
//-----------------------------------------------------------------------------
static BSUBWAY_UPLOADCHECK_RESULT PERSONAL_DATA_SetUpMystatusWait( WIFI_BSUBWAY_PERSONAL* p_wk, HEAPID heapID )
{
  BOOL result;
  BOOL bad_word;
  result = DWC_TOOL_BADWORD_Wait( &p_wk->badword, &bad_word );

  if( result == FALSE )
  {
    return BSUBWAY_UPLOADCHECK_RESULT_WAIT;
  }
  else if( (result == TRUE) && (bad_word == TRUE) )
  { 
    STRBUF  *p_modifiname = DWC_TOOL_CreateBadNickName( heapID );
    //名前置き換え
    MyStatus_SetMyNameFromString( p_wk->p_use_mystatus, p_modifiname);
    GFL_STR_DeleteBuffer(p_modifiname);
    TOMOYA_Printf( "BadName Change\n" );
    return BSUBWAY_UPLOADCHECK_RESULT_NG;
  }

  return BSUBWAY_UPLOADCHECK_RESULT_OK;
}



//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  プロフィールセットアップ
 *
 *  @param  p_wk        ワーク
 *  @param  p_gamedata  ゲームData
 *  @param  p_error     エラーワーク
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpProfile( WIFI_BSUBWAY_PERSONAL* p_wk, GAMEDATA* p_gamedata, WIFI_BSUBWAY_ERROR* p_error )
{
  SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
  BSUBWAY_WIFI_DATA* p_bsubway_wifi  = GAMEDATA_GetBSubwayWifiData( p_gamedata );
  BSUBWAY_SCOREDATA* p_bsubway_score  = GAMEDATA_GetBSubwayScoreData( p_gamedata );
  MYSTATUS* p_mystatus          = p_wk->p_use_mystatus;

  GF_ASSERT( p_wk->setup == FALSE );
  
  // DC_PROFILE作成
  DPW_RAP_CreateProfile( &p_wk->dc_profile, p_mystatus );
  
  // 認証
  Dpw_Bt_SetProfileAsync( &p_wk->dc_profile, &p_wk->dc_profile_result );
  ERROR_DATA_GetAsyncStart( p_error );

  
  // ユーザーのゲーム情報を生成
  PERSONAL_DATA_InitDpwPlayerData( &p_wk->bt_player, p_gamedata, p_mystatus );

  // Mystatus保存
  p_wk->cp_bsubway_wifi   = p_bsubway_wifi;
  p_wk->cp_bsubway_score  = p_bsubway_score;


  WIFI_BSUBWAY_Printf("Dpw Bsubway Email 認証\n");
}

//----------------------------------------------------------------------------
/**
 *  @brief  PersonalData  プロフィールセットアップ完了待ち
 *
 *  @param  p_wk      ワーク
 *  @param  p_error   エラーワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_SetUpProfileWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error )
{
  s32 result;

  GF_ASSERT( p_wk->setup == FALSE );

  // 受信完了待ち
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // エラー終了でなければ完了
    if( !ERROR_DATA_IsError( p_error ) ){
      // 完了
      p_wk->setup = TRUE;
      return TRUE;
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Dpw_Bt_Player情報の生成
 *
 *  @param  p_player  情報格納先
 *  @param  p_gamedata  ゲームデータ
 *  @param  cp_mystatus MYSTATUS
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_InitDpwPlayerData( Dpw_Bt_Player* p_player, GAMEDATA* p_gamedata, const MYSTATUS* cp_mystatus )
{
  SAVE_CONTROL_WORK* p_savedata = GAMEDATA_GetSaveControlWork( p_gamedata );
  WIFI_HISTORY* p_history       = SaveData_GetWifiHistory( p_savedata );
  WIFI_LIST* p_wifilist         = GAMEDATA_GetWiFiList( p_gamedata );
  BSUBWAY_SCOREDATA* p_score    = GAMEDATA_GetBSubwayScoreData( p_gamedata );
  MYPMS_DATA* p_pms             = SaveData_GetMyPmsData( p_savedata );
  int i;

  GFL_STD_MemClear( p_player, sizeof(Dpw_Bt_Player) );

  //name
  GFL_STD_MemCopy( MyStatus_GetMyName( cp_mystatus ), p_player->playerName, 8*2 );
  //playerid
  *((u32*)p_player->playerId) = MyStatus_GetID( cp_mystatus );
  //version
  p_player->versionCode = CasetteVersion;
  //language
  p_player->langCode = CasetteLanguage;
  //countryCode
  p_player->countryCode = (u8)MyStatus_GetMyNation( cp_mystatus );
  //localCode
  p_player->localCode = (u8)MyStatus_GetMyArea( cp_mystatus );
  //gender
  p_player->gender = MyStatus_GetMySex( cp_mystatus );
  //tr_type BTS:4115への対処　トレーナー見た目からトレーナタイプへ
  p_player->trainerType = MyStatus_GetTrainerView( cp_mystatus );
  p_player->trainerType = UnionView_GetTrType( p_player->trainerType );
  
  TOMOYA_Printf( "trainer type %d\n", p_player->trainerType );
 
  //メッセージデータ取得
  for(i = 0;i < 3;i++){
    MYPMS_GetPms( p_pms, MYPMS_PMS_TYPE_BATTLE_READY+i, (PMS_DATA*)&(p_player->message[8*i]) );
  }
  MYPMS_GetPms( p_pms, MYPMS_PMS_TYPE_BATTLE_TOP, (PMS_DATA*)p_player->leaderMessage );

  //タワーセーブデータ取得
  p_player->result =  BSUBWAY_SCOREDATA_GetWifiScore( p_score );
  //ポケモンデータ取得
  BSUBWAY_SCOREDATA_GetUsePokeData( p_score, BSWAY_SCORE_POKE_WIFI, (BSUBWAY_POKEMON*)p_player->pokemon );
}



// ポケモン認証

//----------------------------------------------------------------------------
/**
 *  @brief  NHTTPエラーコールバック
 *  @param  p_wk      ワーク
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_NHTTPErrorCallback(void* pUserwork, int code, int type, int ret)
{
  WIFI_BSUBWAY_PERSONAL* p_wk = pUserwork;
  if(p_wk->p_nhttp){
    NHTTP_RAP_ErrorClean(p_wk->p_nhttp);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン認証　
 *
 *  @param  p_wk      ワーク
 *  @param  p_error
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetUpNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, DWCSvlResult* p_svl_result, HEAPID heapID )
{
  BOOL result;
  
  WIFI_BSUBWAY_Printf( "Pokemon　Upload\n" );
  p_wk->p_nhttp = NHTTP_RAP_Init( heapID,MyStatus_GetProfileID( p_wk->p_use_mystatus ), p_svl_result );

  GF_ASSERT( p_wk->p_nhttp );

  GFL_NET_DWC_SetErrDisconnectCallback(PERSONAL_DATA_NHTTPErrorCallback, p_wk);  //NHTTPエラーの時に呼ぶコールバック
  
  NHTTP_RAP_PokemonEvilCheckCreate( p_wk->p_nhttp, heapID, sizeof(Dpw_Bt_PokemonData)*BSUBWAY_STOCK_WIFI_MEMBER_MAX, NHTTP_POKECHK_SUBWAY);

  // ポケモンを設定する
  NHTTP_RAP_PokemonEvilCheckAdd( p_wk->p_nhttp, (BSUBWAY_POKEMON*)p_wk->bt_player.pokemon, sizeof(Dpw_Bt_PokemonData)*BSUBWAY_STOCK_WIFI_MEMBER_MAX );

  // デバック出力
#ifdef PM_DEBUG
  {
    int i;
    u16* p_bin = (u16*)p_wk->bt_player.pokemon;

    OS_TPrintf( "size %d\n", sizeof(BSUBWAY_POKEMON) );
    for( i=0; i<(sizeof(Dpw_Bt_PokemonData)/2); i++ ){
      OS_TPrintf( "%04x ", p_bin[i] );
    }
    OS_TPrintf( "\n" );
  }
#endif

  // 設定ポケモン数
  p_wk->poke_num = BSUBWAY_STOCK_WIFI_MEMBER_MAX;

  result = NHTTP_RAP_PokemonEvilCheckConectionCreate( p_wk->p_nhttp );
  GF_ASSERT( result );

  result = NHTTP_RAP_StartConnect( p_wk->p_nhttp )==NHTTP_ERROR_NONE;
  GF_ASSERT( result );


  // タイムアウトチェック開始
  ERROR_DATA_StartOnlyTimeOut( p_error );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン認証　完了待ち
 *
 *  @param  p_wk      ワーク
 *  @param  p_error
 *
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_WAIT,  // 認証中
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_OK,    // 結果OK
 *  @retval BSUBWAY_UPLOADCHECK_RESULT_NG,    // 結果NG
 */
//-----------------------------------------------------------------------------
static BSUBWAY_UPLOADCHECK_RESULT PERSONAL_DATA_SetUpNhttpPokemonWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error, BOOL poke_error_check )
{
  NHTTPError error;
  BSUBWAY_UPLOADCHECK_RESULT result = BSUBWAY_UPLOADCHECK_RESULT_WAIT;
  int response;

  GF_ASSERT( p_wk->p_nhttp );

  WIFI_BSUBWAY_Printf( "." );

  response = NHTTP_RAP_GetGetResultCode( p_wk->p_nhttp  );
  error = NHTTP_RAP_Process( p_wk->p_nhttp );
  if((response != 200) && (error==NHTTP_ERROR_NONE)){
    OS_TPrintf("サーバエラー\n");
    ERROR_DATA_SetResponseError( p_error, response );
    NHTTP_RAP_ErrorClean(p_wk->p_nhttp);
    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;
    GFL_NET_DWC_SetErrDisconnectCallback(NULL, NULL);  //NHTTPエラーの時に呼ぶコールバック削除
    return BSUBWAY_UPLOADCHECK_RESULT_NG;
  }
  if( NHTTP_ERROR_NONE != error )
  { 
    // タイムアウトカウント
    ERROR_DATA_OnlyTimeOutCount( p_error );
  }
  else
  {
    void *p_data;
    int i;
    int poke_result;
    p_data  = NHTTP_RAP_GetRecvBuffer( p_wk->p_nhttp );


    //送られてきた状態は正常か
    if( NHTTP_RAP_EVILCHECK_GetStatusCode( p_data ) == 0 )
    {
      BOOL pokecheck_ok = TRUE;
      // 正常

      // 署名を取得
      { 
        const s8 *cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_data, p_wk->poke_num );
        GFL_STD_MemCopy( cp_sign, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
      }

      for( i=0; i<p_wk->poke_num; i++ ){
        poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
        if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
          pokecheck_ok = FALSE;
          if( poke_error_check ){
            ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_POKE_ERROR );
          }else{
            // ポケモン名をデフォルトに変更
            PERSONAL_DATA_SetDefaultPokeName( p_wk, i );
          }
        }
      }
      if( pokecheck_ok ){
        WIFI_BSUBWAY_Printf( "認証　OK\n" );
        result = BSUBWAY_UPLOADCHECK_RESULT_OK;
      }else{
        WIFI_BSUBWAY_Printf( "認証　Error %d\n", i );
        result = BSUBWAY_UPLOADCHECK_RESULT_NG;
      }
    }
    else
    {
      if( poke_error_check ){
        // 認証確認失敗
        ERROR_DATA_SetNhttpError( p_error, BSUBWAY_NHTTP_ERROR_DISCONNECTED );
      }
      
      result = BSUBWAY_UPLOADCHECK_RESULT_NG;

      WIFI_BSUBWAY_Printf( "認証　Error %d\n", NHTTP_RAP_EVILCHECK_GetStatusCode( p_data ) );

      for( i=0; i<p_wk->poke_num; i++ ){
        poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, i );
        if( poke_result != NHTTP_RAP_EVILCHECK_RESULT_OK ){
          // ポケモン名をデフォルトに変更
          PERSONAL_DATA_SetDefaultPokeName( p_wk, i );
        }
        WIFI_BSUBWAY_Printf( "認証　Error pokenum %d  result %d\n", i, poke_result );
      }
    }

    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;
    GFL_NET_DWC_SetErrDisconnectCallback(NULL, NULL);  //NHTTPエラーの時に呼ぶコールバック削除


    WIFI_BSUBWAY_Printf( "\n" );
  }

  return result;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン名をデフォルト名に変更する。
 *
 *	@param	p_wk      ワーク
 *	@param	index     インデックス
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_SetDefaultPokeName( WIFI_BSUBWAY_PERSONAL* p_wk, int index )
{
  BSUBWAY_POKEMON* p_pokedata;
  GF_ASSERT(index < 3);
  p_pokedata = (BSUBWAY_POKEMON*)&p_wk->bt_player.pokemon[index];
  GFL_MSG_GetStringRaw( GlobalMsg_PokeName, p_pokedata->mons_no, p_pokedata->nickname, MONS_NAME_SIZE+EOM_SIZE );
  TOMOYA_Printf( "MonsName Change\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief  NHTTPのワークがのこっていたらあとしまつ。
 *
 *	@param	p_wk 
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_ClearNhttpPokemon( WIFI_BSUBWAY_PERSONAL* p_wk )
{
  if(p_wk->p_nhttp){
    GFL_NET_DWC_SetErrDisconnectCallback(NULL, NULL);  //NHTTPエラーの時に呼ぶコールバック削除
    NHTTP_RAP_PokemonEvilCheckDelete( p_wk->p_nhttp );
    NHTTP_RAP_End( p_wk->p_nhttp );
    p_wk->p_nhttp = NULL;
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  Personal Data アップデート
 */
//-----------------------------------------------------------------------------
static void PERSONAL_DATA_UploadPersonalData( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  GF_ASSERT( p_wk->setup );

  WIFI_BSUBWAY_Printf( "人物情報　Upload\n" );
  {
    s32 rank, room_no;
    s32 win;
    // 挑戦ランク・部屋・勝ち抜きすう取得
    rank = BSUBWAY_WIFIDATA_GetPlayerRank( p_wk->cp_bsubway_wifi );
    room_no = BSUBWAY_WIFIDATA_GetPlayerRoomNo( p_wk->cp_bsubway_wifi );
    win = BSUBWAY_SCOREDATA_GetWifiNum( p_wk->cp_bsubway_score );

    Dpw_Bt_UploadPlayerAsync(  rank,  room_no,  win, &p_wk->bt_player, p_wk->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
    WIFI_BSUBWAY_Printf( "Start\n" );
    ERROR_DATA_GetAsyncStart( p_error );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  Personal Data アップデート完了待ち
 *
 *  @param  p_wk      ワーク
 *  @param  p_error   エラーワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL PERSONAL_DATA_UploadPersonalDataWait( WIFI_BSUBWAY_PERSONAL* p_wk, WIFI_BSUBWAY_ERROR* p_error  )
{
  s32 result;

  // 受信完了待ち
  if( ERROR_DATA_GetAsyncResult( p_error, &result ) ){
    
    // エラー終了でなければ完了
    if( !ERROR_DATA_IsError( p_error ) ){
      return TRUE;
    }
  }
  return FALSE;
}







//-------------------------------------
/// ErrorData
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  ErrorData初期化
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_Init( WIFI_BSUBWAY_ERROR* p_wk )
{
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_ERROR) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Erro検知機能つき　情報受信処理開始
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_GetAsyncStart( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Error検知機能つき SERVERSTATUS　受信処理
 *
 *  @param  p_wk  エラーワーク
 *
 *  @retval TRUE    受信完了
 *  @retval FALSE   受信中
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_GetAsyncServerResult( WIFI_BSUBWAY_ERROR* p_wk )
{
  s32 result = 0;
  BOOL complete = FALSE;
  
  if( !Dpw_Bt_IsAsyncEnd() ){
    // タイムアウトチェック
    p_wk->timeout ++;
    if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
      // 過去策↓
      // CommFatalErrorFunc_NoNumber(); //強制ふっとばし
      // 今作では、SERVERタイムアウトと同様の処理を行う。
      // キャンセルコール
      Dpw_Bt_CancelAsync();

      // タイムアウトへ
      p_wk->timeout_flag = TRUE;
    }
  }else{
    result    = Dpw_Bt_GetAsyncResult();
    complete  = TRUE;
    switch (result){
    case DPW_BT_STATUS_SERVER_OK:   // 正常に動作している
      WIFI_BSUBWAY_Printf(" server ok\n");
      p_wk->server_status = result;
      break;
    case DPW_BT_STATUS_SERVER_STOP_SERVICE: // サービス停止中
    case DPW_BT_STATUS_SERVER_FULL:     // サーバーが満杯
      WIFI_BSUBWAY_Printf(" server ng\n");
      p_wk->server_status = result;
      break;

    case DPW_BT_ERROR_SERVER_FULL:
    case DPW_BT_ERROR_CANCEL:
    case DPW_BT_ERROR_FAILURE:
    case DPW_BT_ERROR_SERVER_TIMEOUT:
    case DPW_BT_ERROR_DISCONNECTED: 
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      WIFI_BSUBWAY_Printf(" server ng\n");
      p_wk->error_code = result;
      break;

    case DPW_BT_ERROR_FATAL:      //!< 通信致命的エラー。電源の再投入が必要です
    default:
      NetErr_DispCall(TRUE);
      break;
    }
  }

  return complete;
}


//----------------------------------------------------------------------------
/**
 *  @brief  Error検知機能つき　情報受信処理
 *
 *  @param  p_wk        ワーク
 *  @param  p_result    結果格納先
 *
 *  @retval TRUE    受信完了
 *  @retval FALSE   受信中
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_GetAsyncResult( WIFI_BSUBWAY_ERROR* p_wk, s32* p_result )
{
  s32 result = 0;
  BOOL complete = FALSE;
  
  if( !Dpw_Bt_IsAsyncEnd() ){
    // タイムアウトチェック
    p_wk->timeout ++;
    if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
      // 過去策↓
      // CommFatalErrorFunc_NoNumber(); //強制ふっとばし
      // 今作では、SERVERタイムアウトと同様の処理を行う。
      // キャンセルコール
      Dpw_Bt_CancelAsync();

      // タイムアウトへ
      p_wk->timeout_flag = TRUE;
    }
  }else{
    result    = Dpw_Bt_GetAsyncResult();
    complete  = TRUE;
    switch (result){
    case DPW_BT_ERROR_SERVER_FULL:
    case DPW_BT_ERROR_CANCEL:
    case DPW_BT_ERROR_FAILURE:
    case DPW_BT_ERROR_SERVER_TIMEOUT:
    case DPW_BT_ERROR_DISCONNECTED: 
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      WIFI_BSUBWAY_Printf(" recv ng\n");
      p_wk->error_code = result;
      break;

    case DPW_BT_ERROR_FATAL:      //!< 通信致命的エラー。電源の再投入が必要です
      NetErr_DispCall(TRUE);
      break;

    default:
      if( result >= 0 )
      {
        WIFI_BSUBWAY_Printf(" recv ok\n");
        *p_result = result;
      }
      else
      {
        // マイナスはError
        NetErr_DispCall(TRUE);
      }
      break;
    }
  }

  return complete;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Dpw Btのエラー状態チェック
 *
 *  @param  cp_wk   ワーク
 *
 *  @retval TRUE    エラー状態
 *  @retval FALSE   エラー状態ではない
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_IsError( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  // BTエラー
  if( (cp_wk->error_code < 0) ){
    return TRUE;
  }
  // SERVERエラー
  if( cp_wk->server_status != DPW_BT_STATUS_SERVER_OK ){
    return TRUE;
  }
  // nhttpエラー
  if( cp_wk->nhttp_error != BSUBWAY_NHTTP_ERROR_NONE ){
    return TRUE;
  }
  // タイムアウト
  if( cp_wk->timeout_flag ){
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ErrorData Dpw BTのエラーメッセージ取得
 *
 *  @param  cp_wk   ワーク
 *    
 *  @return エラーメッセージID
 */
//-----------------------------------------------------------------------------
static s32 ERROR_DATA_GetPrintMessageID( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  int msgno =0;

  // BtError
  if( cp_wk->error_code < 0 ){
    switch( cp_wk->error_code ){
    case DPW_BT_ERROR_SERVER_TIMEOUT:
      msgno = msg_wifi_bt_error_003;
      break;
    case DPW_BT_ERROR_DISCONNECTED:
      msgno = msg_wifi_bt_error_003_01;
      break;
    case DPW_BT_ERROR_SERVER_FULL:
      msgno = msg_wifi_bt_error_002_01;
      break;
    case DPW_BT_ERROR_FAILURE:
      msgno = msg_wifi_bt_error_004_01;
      break;
    case DPW_BT_ERROR_CANCEL:
      msgno = msg_wifi_bt_error_004;
      break;
    case DPW_BT_ERROR_ILLIGAL_REQUEST:
      msgno = msg_wifi_bt_error_005;
      break;

    default:
      GF_ASSERT(0);
      msgno = msg_wifi_bt_error_007;
      break;
    }
  // SERVERError
  }else if( cp_wk->server_status != DPW_BT_STATUS_SERVER_OK ){
    switch( cp_wk->server_status ){
    case DPW_BT_STATUS_SERVER_STOP_SERVICE:
      msgno = msg_wifi_bt_error_001;
      break;
    case DPW_BT_STATUS_SERVER_FULL:
      msgno = msg_wifi_bt_error_002;
      break;

    default:
      GF_ASSERT(0);
      break;
    }
  // nhttp エラー
  }else if( cp_wk->nhttp_error != BSUBWAY_NHTTP_ERROR_NONE ){
    switch( cp_wk->nhttp_error ){
    case BSUBWAY_NHTTP_ERROR_POKE_ERROR:
      msgno = msg_wifi_bt_error_008;
      break;
    case BSUBWAY_NHTTP_ERROR_DISCONNECTED:
      msgno = msg_wifi_bt_error_008;
      break;
    default:
      GF_ASSERT(0);
      break;
    }
  }
  else if( cp_wk->timeout_flag ){
    // タイムアウトエラー
    msgno = msg_wifi_bt_error_003;
  }
  else{
    GF_ASSERT(0);
  }
  

  return msgno;
}

//----------------------------------------------------------------------------
/**
 *	@brief  サーバーレスポンスエラーメッセージ設定
 *
 *	@param	const WIFI_BSUBWAY_ERROR* cp_wk   ワーク
 *
 *	@return TRUEでリクエスト  FALSEでなにもしない
 */
//-----------------------------------------------------------------------------
static BOOL ERROR_DATA_ReqServerResponseMessage( const WIFI_BSUBWAY_ERROR* cp_wk )
{
  if( cp_wk->nhttp_error == BSUBWAY_NHTTP_ERROR_EVILERROR )
  {
    switch( cp_wk->response )
    {
    case 200:
      GF_ASSERT( 0 );
      
    case 400:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_400 );
      break;

    case 401: 
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_401 );
      break;

    case 408: 
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_TIMEOUT );
      break;

    default:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_ETC );
      break;
    }

    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  NhttpError  設定
 *
 *  @param  p_wk    ワーク
 *  @param  error   エラータイプ
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_SetNhttpError( WIFI_BSUBWAY_ERROR* p_wk, BSUBWAY_NHTTP_ERROR error )
{
  p_wk->nhttp_error = error;
}

//----------------------------------------------------------------------------
/**
 *	@brief  NHTTPERRORのレスポンスエラー設定
 *
 *	@param	WIFI_BSUBWAY_ERROR* p_wk  ワーク
 *	@param	response  レスポンスエラー
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_SetResponseError( WIFI_BSUBWAY_ERROR* p_wk, s32 response )
{
  p_wk->nhttp_error = BSUBWAY_NHTTP_ERROR_EVILERROR;
  p_wk->response    = response;
}

//----------------------------------------------------------------------------
/**
 *  @brief  タイムアウトを開始
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_StartOnlyTimeOut( WIFI_BSUBWAY_ERROR* p_wk )
{
  p_wk->timeout_flag  = FALSE;
  p_wk->timeout = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  タイムアウトカウント
 *
 *  @param  p_wk  ワーク
 *
 *  タイムアウトになったら自動でエラー処理へ行きます。
 */
//-----------------------------------------------------------------------------
static void ERROR_DATA_OnlyTimeOutCount( WIFI_BSUBWAY_ERROR* p_wk )
{
  // タイムアウトチェック
  p_wk->timeout ++;
  if( p_wk->timeout == BSUBWAY_TIMEOUT_TIME ){
    p_wk->timeout_flag = TRUE;
  }
}


//-------------------------------------
/// CodeIn
//=====================================
//----------------------------------------------------------------------------
/**
 *  @briefCodeIn開始
 *
 *  @param  p_wk      ワーク
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void CODEIN_StartRoomNoProc( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  static s_CODEIN_BLOCK[CODE_BLOCK_MAX] = {
    VIEW_NUMBER_KETA_ROOM_NO,
    0,
    0,
  };

  s_CODEIN_BLOCK[ 0 ] = ROOM_DATA_GetRoomKeta( &p_wk->roomdata );
  p_wk->p_codein = CodeInput_ParamCreate( heapID, CODEIN_MODE_TRAIN_NO, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), VIEW_NUMBER_KETA_ROOM_NO, s_CODEIN_BLOCK );
  
  // プロックコール
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(codein), &CodeInput_ProcData, p_wk->p_codein );
}

//----------------------------------------------------------------------------
/**
 *  @briefCodeIn開始
 *
 *  @param  p_wk      ワーク
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void CODEIN_StartRankProc( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  static int s_CODEIN_BLOCK[CODE_BLOCK_MAX] = {
    VIEW_NUMBER_KETA_RANK,
    0,
    0,
  };

  p_wk->p_codein = CodeInput_ParamCreate( heapID, CODEIN_MODE_RANK, DPW_BT_RANK_NUM, VIEW_NUMBER_KETA_RANK, s_CODEIN_BLOCK );
  
  // プロックコール
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(codein), &CodeInput_ProcData, p_wk->p_codein );
}

//----------------------------------------------------------------------------
/**
 *  @brief  CodeIn終了
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void CODEIN_EndProc( WIFI_BSUBWAY* p_wk )
{
  if( p_wk->p_codein )
  {
    WIFI_BSUBWAY_Printf( "CodeIn return %d\n", p_wk->p_codein->end_state );
    
    CodeInput_ParamDelete( p_wk->p_codein );
    p_wk->p_codein = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  CodeIn　入力文字列取得
 *
 *  @param  cp_wk   ワーク
 *  @param  p_str 　格納バッファ
 */
//-----------------------------------------------------------------------------
static void CODEIN_GetString( const WIFI_BSUBWAY* cp_wk, STRBUF* p_str )
{
  GF_ASSERT( cp_wk->p_codein );

  GFL_STR_CopyBuffer( p_str, cp_wk->p_codein->strbuf );
}

//----------------------------------------------------------------------------
/**
 *  @brief  入力数値を取得
 *
 *  @param  cp_wk 
 */
//-----------------------------------------------------------------------------
static int CODEIN_GetCode( const WIFI_BSUBWAY* cp_wk )
{
  const STRCODE eonCode = GFL_STR_GetEOMCode();
  const STRCODE *code = GFL_STR_GetStringCodePointer( cp_wk->p_codein->strbuf );

  int num = 0;
  u32 digit = 1;
  u16 arrDigit = 0;
  u16 tempNo;

  static const u8 MAX_DIGIT = 6;

  while( code[arrDigit] != eonCode )
  {
    if( arrDigit >= MAX_DIGIT )
    {
      //桁オーバー
      GF_ASSERT(0);
    }

    tempNo = code[arrDigit] - L'0';
    if( tempNo >= 10 )
    {
      tempNo = code[arrDigit] - L'０';
      if( tempNo >= 10 )
      {
        //想定外の文字が入っている
        GF_ASSERT(0);
      }
    }
    /*
    num += tempNo * digit;
    digit *= 10;
    */
    num *= 10;
    num += tempNo;
    arrDigit++;
  }
  return num;
}




//-------------------------------------
/// Login
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  Login処理 開始
 *
 *  @param  p_wk      ワーク  
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void LOGIN_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->login.gamedata  = p_wk->p_gamedata;
  p_wk->login.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->login.bgm       = WIFILOGIN_BGM_NONE;
  p_wk->login.display   = WIFILOGIN_DISPLAY_UP;
  p_wk->login.nsid      = WB_NET_BSUBWAY;
  p_wk->login.pSvl      = &p_wk->svl_result;


  // プロックコール
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &p_wk->login );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Login処理 後始末
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void LOGIN_EndProc( WIFI_BSUBWAY* p_wk )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  Login処理 結果
 *
 *  @param  cp_wk   ワーク
 *
 *  @retval WIFILOGIN_RESULT_LOGIN, //ログインした
 *  @retval WIFILOGIN_RESULT_CANCEL,//キャンセルした
 */
//-----------------------------------------------------------------------------
static WIFILOGIN_RESULT LOGIN_GetResult( const WIFI_BSUBWAY* cp_wk )
{
  return cp_wk->login.result;
}



//-------------------------------------
/// Logout
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  Logout処理　開始
 *
 *  @param  p_wk      ワーク
 *  @param  heapID    ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void LOGOUT_StartProc( WIFI_BSUBWAY* p_wk )
{
  p_wk->logout.gamedata  = p_wk->p_gamedata;
  p_wk->logout.bg        = WIFILOGIN_BG_NORMAL;
  p_wk->logout.display   = WIFILOGIN_DISPLAY_UP;
  p_wk->logout.bgm       = WIFILOGIN_BGM_NONE;

  // プロックコール
  GAMESYSTEM_CallProc( p_wk->p_param->p_gamesystem, FS_OVERLAY_ID(wifi_login), &WiFiLogout_ProcData, &p_wk->logout );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Logout処理  終了
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void LOGOUT_EndProc( WIFI_BSUBWAY* p_wk )
{
}




//-------------------------------------
/// メイン処理処理
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ　メイン処理
 *
 *  @param  p_wk  ワーク
 *
 *  @retval WIFI_BSUBWAY_RESULT_OK,                   //アップデート・ダウンロード成功
 *  @retval WIFI_BSUBWAY_RESULT_NG,                   //失敗
 *  @retval WIFI_BSUBWAY_RESULT_CANCEL,               //キャンセルした
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  BSUBWAY_MAIN_RESULT (*pFunc[ WIFI_BSUBWAY_MODE_MAX ])( WIFI_BSUBWAY* p_wk, HEAPID heapID ) = 
  {
    WiFiBsubway_Main_ScoreUpload,
    WiFiBsubway_Main_GamedataDownload,
    WiFiBsubway_Main_SuccessdataDownload,
  };
  GF_ASSERT( p_wk->p_param->mode < WIFI_BSUBWAY_MODE_MAX );
  
  return pFunc[ p_wk->p_param->mode ]( p_wk, heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  前回情報のアップロード
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_ScoreUpload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_PERSON_MSG_00:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_008 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP:
    PERSONAL_DATA_SetUpNhttpPokemon( &p_wk->personaldata, &p_wk->bt_error, &p_wk->svl_result, heapID );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT:
    {
      BSUBWAY_UPLOADCHECK_RESULT result;
      // この中でエラーだったポケモンの名前をデフォルトに変更
      result = PERSONAL_DATA_SetUpNhttpPokemonWait( &p_wk->personaldata, &p_wk->bt_error, FALSE );
      if( result == BSUBWAY_UPLOADCHECK_RESULT_OK ){
        p_wk->seq = SCORE_UPLOAD_SEQ_PERSON_UPDATE;
      }else if( result == BSUBWAY_UPLOADCHECK_RESULT_NG ){
        p_wk->seq = SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP01;
      }
    }
    break;
  // ポケモンの名前をデフォルト名にして再挑戦
  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP01:
    PERSONAL_DATA_SetUpNhttpPokemon( &p_wk->personaldata, &p_wk->bt_error, &p_wk->svl_result, heapID );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_POKE_SETUP_WAIT01:
    {
      BSUBWAY_UPLOADCHECK_RESULT result;
      result = PERSONAL_DATA_SetUpNhttpPokemonWait( &p_wk->personaldata, &p_wk->bt_error, TRUE );
      if( result == BSUBWAY_UPLOADCHECK_RESULT_OK ){
        p_wk->seq = SCORE_UPLOAD_SEQ_PERSON_UPDATE;
      }else if( result == BSUBWAY_UPLOADCHECK_RESULT_NG ){
        // 自動でエラー処理に遷移
        TOMOYA_Printf( "Auto Error Wait\n" );
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_PERSON_UPDATE:
    PERSONAL_DATA_UploadPersonalData( &p_wk->personaldata, &p_wk->bt_error );
    p_wk->seq++;
    break;
  case SCORE_UPLOAD_SEQ_PERSON_UPDATE_WAIT:
    if( PERSONAL_DATA_UploadPersonalDataWait( &p_wk->personaldata, &p_wk->bt_error ) ){
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SAVE:
    // バトルTower 未アップロードフラグをクリア
    BSUBWAY_SCOREDATA_SetFlag( p_wk->p_bsubway_score, BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD, BSWAY_SETMODE_reset );

    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SAVE_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_009 );
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_PERSON_MSG_01:
    if( VIEW_PrintMain( &p_wk->view ) )
    {
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_PERSON_END:
    return BSUBWAY_MAIN_RESULT_OK;

  default:
    GF_ASSERT(0);
    break;
  }

  return BSUBWAY_MAIN_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ゲーム情報のダウンロード
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_GamedataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK:
    {
      u32 rank = BSUBWAY_SCOREDATA_GetWifiRank( p_wk->p_bsubway_score );

      GF_ASSERT( (rank >= 1) && (rank <= DPW_BT_RANK_NUM) );
      
      // 入れる。
      ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, rank );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){

      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00:
    VIEW_EndTimeIcon( &p_wk->view );
    VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_001 );
    p_wk->seq++;  
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO:
    CODEIN_StartRoomNoProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_ROOM_NO_WAIT:

    p_wk->code_input = CODEIN_GetCode( p_wk );
    
    // 文字列を取得
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CODEIN_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      RTCTime time;
      RTCDate date;
      u32 rank = ROOM_DATA_GetRank( &p_wk->roomdata );
      u32 roomno = p_wk->code_input;
      u32 roomnum = ROOM_DATA_GetRoomNum( &p_wk->roomdata );


      // GameSpyサーバー時間・日付を取得
      DWC_GetDateTime( &date, &time);

      // 情報は範囲内か？
      if( (roomno < 1) || (roomno > roomnum) ){
        
        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG;

      }
      // 今日は既にダウンロードしているか？
      else if( BSUBWAY_WIFIDATA_CheckRoomDataFlag( p_wk->p_bsubway_wifi, rank, roomno, &date )){
        
        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG;
        
      }else{
        
        // 入れる。
        ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );

        // メッセージ表示
        VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNo( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
        VIEW_SetWordNumber( &p_wk->view, 1, ROOM_DATA_GetRank( &p_wk->roomdata ), VIEW_NUMBER_KETA_RANK );
        VIEW_PrintStream( &p_wk->view, msg_wifi_bt_002 );

        VIEW_StartTimeIcon( &p_wk->view );

        p_wk->seq++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_SAVE:
    // セーブデータ反映
    {
      ROOM_DATA_CheckDownloadData( &p_wk->roomdata );
      ROOM_DATA_SavePlayerData( &p_wk->roomdata, p_wk->p_bsubway_wifi );
      ROOM_DATA_SaveLeaderData( &p_wk->roomdata, p_wk->p_bsubway_wifi );
    }

    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_004 );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_MSG_02:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_DOWNLOAD_END:
    return BSUBWAY_MAIN_RESULT_OK;


  // 本日ダウンロード済みメッセージ表示
  case SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_003 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_ROOM_DOWNLOADED_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_005 );
    p_wk->seq++;
    break;
    
  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO:
    VIEW_StartYesNo( &p_wk->view );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_GAMEDATA_CHALLENGE_END_YESNO_WAIT:
    {
      u32 result = VIEW_MainYesNo( &p_wk->view );
      if( result == 0 ){
        return BSUBWAY_MAIN_RESULT_CANCEL;
      }else if( result == BMPMENU_CANCEL ){

        p_wk->seq = SCORE_UPLOAD_SEQ_GAMEDATA_MSG_00;
      }
    }
    break;

  default:
    GF_ASSERT(0);
    break;
  }
  return BSUBWAY_MAIN_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  歴代情報のダウンロード
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static BSUBWAY_MAIN_RESULT WiFiBsubway_Main_SuccessdataDownload( WIFI_BSUBWAY* p_wk, HEAPID heapID )
{
  switch( p_wk->seq )
  {
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00:
    VIEW_EndTimeIcon( &p_wk->view );
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_006 );
    p_wk->seq++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK:
    CODEIN_StartRankProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_WAIT:
    p_wk->code_input = CODEIN_GetCode( p_wk );
    // 文字列を取得
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_RANK_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      // 情報が範囲内か？
      if( (p_wk->code_input >= 1) && (p_wk->code_input <= DPW_BT_RANK_NUM) ){
        
        // 入れる。
        ROOM_DATA_SetUp( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );
        VIEW_PrintStream( &p_wk->view, msg_wifi_bt_013 );
        VIEW_StartTimeIcon( &p_wk->view );
        p_wk->seq ++;
      }else{

        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_RANK_WAIT:
    if( ROOM_DATA_SetUpWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNum( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_007 );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_01:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT:
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 1);
    p_wk->seq ++;
    break;
    
  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEOUT_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      VIEW_Exit( &p_wk->view );
      p_wk->seq ++;
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO:
    CODEIN_StartRoomNoProc( p_wk, heapID );
    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_WAIT:
    p_wk->code_input = CODEIN_GetCode( p_wk );
    // 文字列を取得
    CODEIN_EndProc( p_wk );

    VIEW_Init( &p_wk->view, heapID );
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);

    p_wk->seq ++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_CODEIN_ROOM_NO_FADEIN_WAIT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      u32 room_num = ROOM_DATA_GetRoomNum( &p_wk->roomdata );
      
      //値が範囲内か？
      if( (p_wk->code_input < 1) || (p_wk->code_input > room_num) ){
        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG;
      }else{
      
        // 入れる。
        ROOM_DATA_LoadRoomData( &p_wk->roomdata, &p_wk->bt_error, p_wk->code_input );

        // メッセージ表示
        VIEW_SetWordNumber( &p_wk->view, 0, ROOM_DATA_GetRoomNo( &p_wk->roomdata ), ROOM_DATA_GetRoomKeta( &p_wk->roomdata ) );
        VIEW_SetWordNumber( &p_wk->view, 1, ROOM_DATA_GetRank( &p_wk->roomdata ), VIEW_NUMBER_KETA_RANK );
        VIEW_PrintStream( &p_wk->view, msg_wifi_bt_002 );

        VIEW_StartTimeIcon( &p_wk->view );

        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_ROOM_WAIT:
    if( ROOM_DATA_LoadRoomDataWait( &p_wk->roomdata, &p_wk->bt_error ) ){
      if( VIEW_PrintMain( &p_wk->view ) ){
        p_wk->seq ++;
      }
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_SAVE:
    // セーブデータ反映
    {
      ROOM_DATA_CheckDownloadData( &p_wk->roomdata );
      ROOM_DATA_SaveLeaderData( &p_wk->roomdata, p_wk->p_bsubway_wifi );
    }
    SAVE_Start(p_wk);
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_WAIT:
    if( SAVE_Main(p_wk) ){
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_wifi_bt_004 );
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_03:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;


  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END:
    return BSUBWAY_MAIN_RESULT_OK;


  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG:
    VIEW_PrintStream( &p_wk->view, msg_wifi_bt_010 );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_MSG_WAIT:
    if( VIEW_PrintMain( &p_wk->view ) ){
      p_wk->seq++;  
    }
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO:
    VIEW_StartYesNo( &p_wk->view );
    p_wk->seq++;
    break;

  case SCORE_UPLOAD_SEQ_SUCCESSDATA_DOWNLOAD_END_YESNO_WAIT:
    {
      u32 result = VIEW_MainYesNo( &p_wk->view );
      if( result == 0 ){
        return BSUBWAY_MAIN_RESULT_CANCEL;
      }else if( result == BMPMENU_CANCEL )
      {

        // ROOM_DATAリセット
        ROOM_DATA_Init( &p_wk->roomdata );
        p_wk->seq = SCORE_UPLOAD_SEQ_SUCCESSDATA_MSG_00;
      }
    }
    break;


  default:
    GF_ASSERT(0);
    break;
  }
  return BSUBWAY_MAIN_RESULT_CONTINUE;
}



  
//-------------------------------------
/// 接続・切断・エラー
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  接続開始
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Connect( WIFI_BSUBWAY* p_wk )
{
  // 世界交換接続初期化
  DWCUserData* p_userdata;    // 認証済みのDWCUSERデータしかこないはず
  s32 profileId;

  WIFI_BSUBWAY_Printf( "Connect Bsuway Start\n" );

  // DWCUser構造体取得
  p_userdata = WifiList_GetMyUserInfo( p_wk->p_wifilist );

  MyStatus_SetProfileID( p_wk->p_mystatus, WifiList_GetMyGSID( p_wk->p_wifilist ) );

  // 正式なデータを取得
  profileId = MyStatus_GetProfileID( p_wk->p_mystatus );
  WIFI_BSUBWAY_Printf("Dpwサーバーログイン情報 profileId=%08x\n", profileId);

  // DPW_BT初期化
  Dpw_Bt_Init( profileId, DWC_CreateFriendKey( p_userdata ),LIBDPW_SERVER_TYPE );

  WIFI_BSUBWAY_Printf("Dpw Bsubway 初期化\n");

  Dpw_Bt_GetServerStateAsync();
  ERROR_DATA_GetAsyncStart( &p_wk->bt_error );

  WIFI_BSUBWAY_Printf("Dpw Bsubway サーバー状態取得開始\n");

  p_wk->dpw_main_do = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  接続ウエイト
 *
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE    接続完了
 *  @retval FALSE   接続途中
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ConnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
  return ERROR_DATA_GetAsyncResult( &p_wk->bt_error, &result );
}

//----------------------------------------------------------------------------
/**
 *  @brief  切断開始
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void WiFiBsubway_Disconnect( WIFI_BSUBWAY* p_wk )
{
  if( Dpw_Bt_IsAsyncEnd() == FALSE ){

    WIFI_BSUBWAY_Printf("Dpw Bsubway 切断\n");
    // キャンセルコール
    Dpw_Bt_CancelAsync();
    ERROR_DATA_GetAsyncStart( &p_wk->bt_error );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  切断ウエイト
 *  
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_DisconnectWait( WIFI_BSUBWAY* p_wk )
{
  s32 result;
  // すでに切断済み？
  if(p_wk->dpw_main_do == FALSE){
    return TRUE;
  }


  if( ERROR_DATA_GetAsyncResult( &p_wk->bt_error, &result ) ){
    // 終了
    Dpw_Bt_End();

    p_wk->dpw_main_do = FALSE;
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  エラー処理
 *
 *  @param  p_wk  ワーク
 *
 *  @retval BOOL  ErrorWaitの有無
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_Error( WIFI_BSUBWAY* p_wk )
{
  // Netエラーチェック
  if( GFL_NET_DWC_ERROR_ReqErrorDisp(TRUE,FALSE) )
  {
    
    // 終了へ
    return FALSE;
  }

  // その他 Netエラーの場合
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE ){
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

    WIFI_BSUBWAY_Printf( "not message error %d\n", cp_error->errorRet );

#if 0
    //GDBとSCの切断とFatalエラー以外は、個別処理とするため、検知しない
    if( (cp_error->errorRet == DWC_ERROR_GDB_ANY
          || cp_error->errorRet == DWC_ERROR_SCL_ANY)
        && (cp_error->errorType != DWC_ETYPE_DISCONNECT
          && cp_error->errorType != DWC_ETYPE_FATAL) )
    { 
      return FALSE;
    }
#endif
    return FALSE;
  }
  
  // Btエラーチェック
  if( ERROR_DATA_IsError( &p_wk->bt_error ) )
  {
    //　レスポンスエラーならば表示処理
    if( ERROR_DATA_ReqServerResponseMessage( &p_wk->bt_error ) )
    {
      return FALSE;
    }
    else
    {
      s32 msg_id;

      //　エラー表示処理
      // エラー表示終了を待つ
      msg_id = ERROR_DATA_GetPrintMessageID( &p_wk->bt_error );

      // TimeWait Off
      VIEW_EndTimeIcon( &p_wk->view );
      VIEW_PrintStream( &p_wk->view, msg_id );
      return TRUE;
    }
  }


  // Not Error
  GF_ASSERT(0);
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  エラー処理　ウエイト
 *
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE    完了
 *  @retval FALSE   途中
 */
//-----------------------------------------------------------------------------
static BOOL WiFiBsubway_ErrorWait( WIFI_BSUBWAY* p_wk )
{
  return VIEW_PrintMain( &p_wk->view );
}





//-------------------------------------
/// View
//=====================================
//----------------------------------------------------------------------------
/**
 *  @brief  描画処理  初期化
 *
 *  @param  p_wk      ワーク
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void VIEW_Init( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID )
{
  GF_ASSERT( p_wk->setup == FALSE );
  
  // 描画システム生成
  p_wk->p_graphic = WIFI_BSUBWAY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, heapID );

  // ヒープID保存
  p_wk->heapID = heapID;

  // 基本グラフィック転送
  VIEW_InitResource( p_wk, heapID );

  // ビットマップ生成
  p_wk->p_win = GFL_BMPWIN_Create(
    VIEW_FRAME_MAIN_WIN,
    1 , 19, 30 ,4,
    VIEW_PAL_MSG, GFL_BMP_CHRAREA_GET_B );

  BmpWinFrame_Write( p_wk->p_win, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar), VIEW_PAL_WIN_FRAME );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_win), 15);
  GFL_BMPWIN_MakeScreen(p_wk->p_win);
  GFL_BMPWIN_TransVramCharacter(p_wk->p_win);
  GFL_BG_LoadScreenV_Req( VIEW_FRAME_MAIN_WIN );

  // メッセージデータ生成
  p_wk->p_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_bsubway_dat, heapID );
  p_wk->p_wordset = WORDSET_Create( heapID );
  p_wk->p_font = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , heapID );
  p_wk->p_strtmp  = GFL_STR_CreateBuffer( 128, heapID );
  p_wk->p_str  = GFL_STR_CreateBuffer( 128, heapID );


  // プリントタスク
  p_wk->p_msg_tcbsys = GFL_TCBL_Init( heapID , heapID , 1 , 0 );

  p_wk->setup = TRUE;

  // OAM面の表示ON
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // 通信アイコンON
  GFL_NET_ReloadIconTopOrBottom( TRUE, heapID );

}

//----------------------------------------------------------------------------
/**
 *  @brief  描画処理  破棄
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void VIEW_Exit( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->setup == FALSE )
  {
    return ;
  }
  
  // 
  if(p_wk->p_msg_stream)
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
  }

  // プリントタスク
  GFL_TCBL_Exit( p_wk->p_msg_tcbsys );

  // メッセージデータ生成
  GFL_MSG_Delete( p_wk->p_msgdata );
  WORDSET_Delete( p_wk->p_wordset );
  GFL_FONT_Delete( p_wk->p_font );
  GFL_STR_DeleteBuffer( p_wk->p_strtmp );
  GFL_STR_DeleteBuffer( p_wk->p_str );

  // 基本グラフィック転送
  VIEW_ExitResource( p_wk );

  // TIMEICONを消しておく
  VIEW_EndTimeIcon( p_wk );

  // YESNO強制破棄
  if( p_wk->p_yesno ){
    BmpMenu_YesNoMenuExit( p_wk->p_yesno );
    p_wk->p_yesno = NULL;
  }


  // win破棄
  GFL_BMPWIN_Delete( p_wk->p_win );
  
  // 描画システム生成
  WIFI_BSUBWAY_GRAPHIC_Exit( p_wk->p_graphic );


  GFL_FONTSYS_SetDefaultColor();
  GFL_STD_MemClear( p_wk, sizeof(WIFI_BSUBWAY_VIEW) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  描画処理  メイン
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void VIEW_Main( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->setup == FALSE ){
    return ;
  }
  
  GFL_TCBL_Main( p_wk->p_msg_tcbsys );
  ConnectBGPalAnm_Main( &p_wk->bg_palanm );
}


//----------------------------------------------------------------------------
/**
 *  @brief  グラフィックResourceのセットアップ
 *
 *  @param  p_wk      ワーク
 *  @param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void VIEW_InitResource( WIFI_BSUBWAY_VIEW* p_wk, HEAPID heapID )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFI_LOGIN, heapID );
  u32 char_ofs;
  
  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifi_login_connect_win_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  heapID);
  // サブ画面BG0キャラ転送
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_SUB_BACKGROUND, 0, 0, heapID);

  // サブ画面BG0スクリーン転送
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win2_d_NSCR,
                                            VIEW_FRAME_SUB_BACKGROUND, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);


  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifi_login_connect_win_NCLR,
                                    PALTYPE_MAIN_BG, 0, 0,  heapID);
  // メイン画面BG0キャラ転送
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_MAIN_BACKGROUND, 0, 0, heapID);

  // メイン画面BG0スクリーン転送
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win1_u_NSCR,
                                            VIEW_FRAME_MAIN_BACKGROUND, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);

  // メイン画面BG3キャラ転送
  char_ofs = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_wifi_login_connect_win_NCGR,
                                                                VIEW_FRAME_MAIN_BACKGROUND_BASE, 0, 0, heapID);

  // メイン画面BG3スクリーン転送
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_wifi_login_connect_win2_u_NSCR,
                                            VIEW_FRAME_MAIN_BACKGROUND_BASE, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(char_ofs), 0, 0,
                                            heapID);


  //パレットアニメシステム作成
  ConnectBGPalAnm_InitBg(&p_wk->bg_palanm, p_handle, NARC_wifi_login_connect_ani_NCLR, heapID, VIEW_FRAME_MAIN_BACKGROUND, VIEW_FRAME_MAIN_BACKGROUND_BASE );

  GFL_ARC_CloseDataHandle(p_handle);



  // フレーム生成
  GFL_BG_AllocCharacterArea( VIEW_FRAME_MAIN_WIN, GFL_BG_1CHRDATASIZ, GFL_BG_CHRAREA_GET_F ); //0charをあけとく
  GFL_BG_AllocCharacterArea( VIEW_FRAME_MAIN_YESNO, GFL_BG_1CHRDATASIZ, GFL_BG_CHRAREA_GET_F ); //0charをあけとく
  p_wk->frame_bgchar = BmpWinFrame_GraphicSetAreaMan( VIEW_FRAME_MAIN_WIN, VIEW_PAL_WIN_FRAME, MENU_TYPE_SYSTEM, heapID);
  p_wk->frame_bgchar_sys = BmpWinFrame_GraphicSetAreaMan( VIEW_FRAME_MAIN_YESNO, VIEW_PAL_WIN_FRAME, MENU_TYPE_SYSTEM, heapID);

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                              0x20*VIEW_PAL_MSG, 0x20, heapID);
}


//----------------------------------------------------------------------------
/**
 *  @brief  Resource破棄
 */
//-----------------------------------------------------------------------------
static void VIEW_ExitResource( WIFI_BSUBWAY_VIEW* p_wk )
{
  GFL_BG_FreeCharacterArea( VIEW_FRAME_MAIN_WIN,
      GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar),
      GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_bgchar));

  GFL_BG_FreeCharacterArea( VIEW_FRAME_MAIN_YESNO,
      GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar_sys),
      GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_bgchar_sys));

  ConnectBGPalAnm_End( &p_wk->bg_palanm );
  G2_BlendNone();

}




//----------------------------------------------------------------------------
/**
 *  @brief  メッセージ描画開始
 *
 *  @param  p_wk    ワーク
 *  @param  msg_id  メッセージID
 */
//-----------------------------------------------------------------------------
static void VIEW_PrintStream( WIFI_BSUBWAY_VIEW* p_wk, u32 msg_id )
{
  if( p_wk->p_msg_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
    p_wk->p_msg_stream = NULL;
  }

  GFL_MSG_GetString( p_wk->p_msgdata, msg_id, p_wk->p_strtmp );
  WORDSET_ExpandStr( p_wk->p_wordset, p_wk->p_str, p_wk->p_strtmp );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_win), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  p_wk->p_msg_stream = PRINTSYS_PrintStream(p_wk->p_win,0,0, p_wk->p_str, p_wk->p_font,
                                        MSGSPEED_GetWait(), p_wk->p_msg_tcbsys, 2, p_wk->heapID, 15);

  BmpWinFrame_Write( p_wk->p_win, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_bgchar), VIEW_PAL_WIN_FRAME );

  GFL_BMPWIN_TransVramCharacter(p_wk->p_win);
}

//----------------------------------------------------------------------------
/**
 *  @brief  メッセージ描画完了待ち
 *
 *  @param  cp_wk   ワーク
 *
 *  @retval TRUE  完了
 *  @retval FALSE 途中
 */
//-----------------------------------------------------------------------------
static BOOL VIEW_PrintMain( WIFI_BSUBWAY_VIEW* p_wk )
{
  int state;
  
  if( p_wk->p_msg_stream == NULL ){
    return TRUE;
  }
  
  state = PRINTSYS_PrintStreamGetState( p_wk->p_msg_stream );
  
  if( state == PRINTSTREAM_STATE_DONE ){
    PRINTSYS_PrintStreamDelete( p_wk->p_msg_stream );
    p_wk->p_msg_stream = NULL;
    return TRUE;
  }else if ( state == PRINTSTREAM_STATE_PAUSE )
  {
    if( (GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE) || GFL_UI_TP_GetTrg() ){
      PRINTSYS_PrintStreamReleasePause( p_wk->p_msg_stream );
    }
  }
  
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ワードセットに数値を設定
 *
 *  @param  p_wk      ワーク
 *  @param  idx       インデックス
 *  @param  number    ナンバー
 *  @param  keta      桁
 *  @param  dispType  表示タイプ
 */
//-----------------------------------------------------------------------------
static void VIEW_SetWordNumber( WIFI_BSUBWAY_VIEW* p_wk, u32 idx, u32 number, u32 keta )
{
  WORDSET_RegisterNumber( p_wk->p_wordset, idx, number, keta, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YES NOウィンドウの表示 ON
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void VIEW_StartYesNo( WIFI_BSUBWAY_VIEW* p_wk )
{
  static BMPWIN_YESNO_DAT s_BMPWIN_YESNO_DAT = {
    VIEW_FRAME_MAIN_YESNO,
    24, 13,   // X Y
    VIEW_PAL_MSG,        // PAL
    0,                  // CHR
  };
  
  GF_ASSERT( p_wk->p_yesno == NULL );

  s_BMPWIN_YESNO_DAT.chrnum = GFL_ARCUTIL_TRANSINFO_GetPos( p_wk->frame_bgchar_sys );
  
  p_wk->p_yesno = BmpMenu_YesNoSelectInit( &s_BMPWIN_YESNO_DAT, 
      GFL_ARCUTIL_TRANSINFO_GetPos( p_wk->frame_bgchar_sys ), VIEW_PAL_WIN_FRAME, 0, p_wk->heapID );
}

//----------------------------------------------------------------------------
/**
 *  @brief  YesNoまち
 *
 *  @param  p_wk 
 *
 * @retval  "BMPMENU_NULL 選択されていない"
 * @retval  "0        はいを選択"
 * @retval  "BMPMENU_CANCEL いいえorキャンセル"
 */
//-----------------------------------------------------------------------------
static u32 VIEW_MainYesNo( WIFI_BSUBWAY_VIEW* p_wk )
{
  u32 result = BMPMENU_CANCEL;

  if( p_wk->p_yesno )
  {
    result = BmpMenu_YesNoSelectMain( p_wk->p_yesno );
    if( result != BMPMENU_NULL )
    {
      p_wk->p_yesno = NULL;
    }
  }
   
  return result;
}

//----------------------------------------------------------------------------
/**
 *	@brief  タイムアイコン　開始
 *
 *	@param	p_wk    ワーク 
 */
//-----------------------------------------------------------------------------
static void VIEW_StartTimeIcon( WIFI_BSUBWAY_VIEW* p_wk )
{
  GF_ASSERT( p_wk->p_timeicon == NULL );
  p_wk->p_timeicon = TIMEICON_Create( GFUser_VIntr_GetTCBSYS(), p_wk->p_win, 15, TIMEICON_DEFAULT_WAIT, p_wk->heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  タイムアイコン　終了
 *
 *	@param	p_wk    ワーク 
 */
//-----------------------------------------------------------------------------
static void VIEW_EndTimeIcon( WIFI_BSUBWAY_VIEW* p_wk )
{
  if( p_wk->p_timeicon )
  {
    TIMEICON_Exit( p_wk->p_timeicon );
    p_wk->p_timeicon =NULL;
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief  セーブ開始
 *
 *  @param  p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void SAVE_Start( WIFI_BSUBWAY* p_wk )
{
  GF_ASSERT( p_wk->save_do == FALSE );
  GAMEDATA_SaveAsyncStart(p_wk->p_gamedata);
  p_wk->save_do = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  セーブメイン
 *
 *  @param  p_wk  ワーク
 *
 *  @retval TRUE  完了
 *  @retval FALSE 途中
 */
//-----------------------------------------------------------------------------
static BOOL SAVE_Main( WIFI_BSUBWAY* p_wk )
{
  SAVE_RESULT result;

  if( p_wk->save_do == FALSE ){
    return TRUE;
  }

  result = GAMEDATA_SaveAsyncMain(p_wk->p_gamedata);
  if( (result == SAVE_RESULT_OK) ){
    p_wk->save_do = FALSE;
    return TRUE;
  }
  return FALSE;
}


