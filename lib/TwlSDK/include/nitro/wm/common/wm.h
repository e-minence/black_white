/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WM - include
  File:     wm.h

  Copyright 2003-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author:$
 *---------------------------------------------------------------------------*/
#ifndef NITRO_WM_COMMON_WM_H_
#define NITRO_WM_COMMON_WM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include <nitro/types.h>
#include <nitro/os.h>
#include <nitro_wl/common/version_wl.h>


/*---------------------------------------------------------------------------*
    定数定義
 *---------------------------------------------------------------------------*/

#undef WM_GAMEINFO_TYPE_OLD            // WMGameInfo の旧形式への対応は廃止されました

// Data Sharing の最大サイズを 252 バイトにして WMDataSharingInfo 構造体をスリムにするスイッチ
//#define WM_DS_DATA_SIZE_252

// TESTモード有効化スイッチ
//#define WM_ENABLE_TESTMODE

// 使用可能チャンネルを先に計算しておくコンパイルスイッチ
#if ( SDK_VERSION_WL >= 15600 )
#ifdef  SDK_TEG
#undef  WM_PRECALC_ALLOWEDCHANNEL
#else
#define WM_PRECALC_ALLOWEDCHANNEL
#endif
#else
#undef  WM_PRECALC_ALLOWEDCHANNEL
#endif

#define WM_SSID_MASK_CUSTOMIZE  1


// WM API関連 ===========================================================================

// 各APIのID
typedef enum WMApiid
{
    WM_APIID_INITIALIZE = 0,           //  0 : WM_Initialize()
    WM_APIID_RESET,                    //  1 : WM_Reset()
    WM_APIID_END,                      //  2 : WM_End()

    WM_APIID_ENABLE,                   //  3 : WM_Enable()
    WM_APIID_DISABLE,                  //  4 : WM_Disable()
    WM_APIID_POWER_ON,                 //  5 : WM_PowerOn()
    WM_APIID_POWER_OFF,                //  6 : WM_PowerOff()

    WM_APIID_SET_P_PARAM,              //  7 : WM_SetParentParameter()
    WM_APIID_START_PARENT,             //  8 : WM_StartParent()
    WM_APIID_END_PARENT,               //  9 : WM_EndParent()
    WM_APIID_START_SCAN,               // 10 : WM_StartScan()
    WM_APIID_END_SCAN,                 // 11 : WM_EndScan()
    WM_APIID_START_CONNECT,            // 12 : WM_StartConnect()
    WM_APIID_DISCONNECT,               // 13 : WM_Disconnect()
    WM_APIID_START_MP,                 // 14 : WM_StartMP()
    WM_APIID_SET_MP_DATA,              // 15 : WM_SetMPData()
    WM_APIID_END_MP,                   // 16 : WM_EndMP()
    WM_APIID_START_DCF,                // 17 : WM_StartDCF()
    WM_APIID_SET_DCF_DATA,             // 18 : WM_SetDCFData()
    WM_APIID_END_DCF,                  // 19 : WM_EndDCF()
    WM_APIID_SET_WEPKEY,               // 20 : WM_SetWEPKey()
    WM_APIID_START_KS,                 // 21 : WM_StartKeySharing() 下位互換のため残しています
    WM_APIID_END_KS,                   // 22 : WM_EndKeySharing()   下位互換のため残しています
    WM_APIID_GET_KEYSET,               // 23 : WM_GetKeySet()
    WM_APIID_SET_GAMEINFO,             // 24 : WM_SetGameInfo()
    WM_APIID_SET_BEACON_IND,           // 25 : WM_SetBeaconIndication()
    WM_APIID_START_TESTMODE,           // 26 : WM_StartTestMode()
    WM_APIID_STOP_TESTMODE,            // 27 : WM_StopTestMode()
    WM_APIID_VALARM_MP,                // 28 : ARM7内のVAlarmからのMP起動用(特殊)
    WM_APIID_SET_LIFETIME,             // 29 : WM_SetLifeTime()
    WM_APIID_MEASURE_CHANNEL,          // 30 : WM_MeasureChannel()
    WM_APIID_INIT_W_COUNTER,           // 31 : WM_InitWirelessCounter()
    WM_APIID_GET_W_COUNTER,            // 32 : WM_GetWirelessCounter()
    WM_APIID_SET_ENTRY,                // 33 : WM_SetEntry()
    WM_APIID_AUTO_DEAUTH,              // 34 : ARM7内のindicateからの自動切断起動用(特殊)
    WM_APIID_SET_MP_PARAMETER,         // 35 : WM_SetMPParameter()
    WM_APIID_SET_BEACON_PERIOD,        // 36 : WM_SetBeaconPeriod()
    WM_APIID_AUTO_DISCONNECT,          // 37 : ARM7内のindicateからの自動切断起動用(特殊)
    WM_APIID_START_SCAN_EX,            // 38 : WM_StartScanEx()
    WM_APIID_SET_WEPKEY_EX,            // 39 : WM_SetWEPKeyEx()
    WM_APIID_SET_PS_MODE,              // 40 : WM_SetPowerSaveMode()
    WM_APIID_START_TESTRXMODE,         // 41 : WM_StartTestRxMode()
    WM_APIID_STOP_TESTRXMODE,          // 42 : WM_StopTestRxMode()

    WM_APIID_KICK_MP_PARENT,           // 43 : MP起動用(ARM7内部使用)
    WM_APIID_KICK_MP_CHILD,            // 44 : MP起動用(ARM7内部使用)
    WM_APIID_KICK_MP_RESUME,           // 45 : MP起動用(ARM7内部使用)

    WM_APIID_ASYNC_KIND_MAX,           // 46 : 非同期処理の種類

    WM_APIID_INDICATION = 128,         // 128 : Indicationコールバック用
    WM_APIID_PORT_SEND,                // 129 : port へのデータ送信通知
    WM_APIID_PORT_RECV,                // 130 : port へのデータ受信通知
    WM_APIID_READ_STATUS,              // 131 : WM_ReadStatus()

    WM_APIID_UNKNOWN = 255             // 255 : 不明のコマンド番号時にARM7から返す値
}
WMApiid;

// for backward compatibility
//#define WM_APIID_SET_MP_FREQ    WM_APIID_SET_MP_PARAMETER

// ARM7側でのAPI要求受付完了フラグ
#define WM_API_REQUEST_ACCEPTED 0x8000

// WEPモード
#define WM_WEPMODE_NO           0
#define WM_WEPMODE_40BIT        1
#define WM_WEPMODE_104BIT       2
#define WM_WEPMODE_128BIT       3

// AUTHモード (WM_StartConnectExで使用する)
#define WM_AUTHMODE_OPEN_SYSTEM 0
#define WM_AUTHMODE_SHARED_KEY  1

// ScanType
#define WM_SCANTYPE_ACTIVE      0
#define WM_SCANTYPE_PASSIVE     1

#if WM_SSID_MASK_CUSTOMIZE
#define WM_SCANTYPE_ACTIVE_CUSTOM   2
#define WM_SCANTYPE_PASSIVE_CUSTOM  3
#endif

// BSSひとつにつき接続可能な子機最大数
#define WM_NUM_MAX_CHILD        15     // NITRO 親機から与えられうる AssociationID の最大値
#define WM_NUM_MAX_AP_AID       2007   // 無線ルータから与えられうる AssociationID の最大値

// port 数
#define WM_NUM_OF_PORT          16     // port の個数
#define WM_NUM_OF_SEQ_PORT      8      // sequential な処理を行う port の個数
typedef enum WMPort
{
    // Sequential な制御を行わないポート群
    WM_PORT_RAWDATA = 0,               // 下位互換用のポート
    WM_PORT_BT = 1,                    // ブロック転送用ポート
    WM_PORT_RESERVE_02 = 2,            // 予約
    WM_PORT_RESERVE_03 = 3,            // 予約

    // Sequential な制御を行うポート群
    WM_PORT_RESERVE_10 = 8,            // 予約
    WM_PORT_RESERVE_11 = 9,            // 予約
    WM_PORT_RESERVE_12 = 10,           // 予約
    WM_PORT_RESERVE_13 = 11            // 予約
}
WMPort;

// 送信 queue 関連
#define WM_SEND_QUEUE_NUM       32     // 合計 32 段の queue list
#define WM_SEND_QUEUE_END       ((u16)0xffff)   // tail を表す値
#define WM_PRIORITY_LEVEL       4      // 優先度は 4 段階
typedef enum WMPriorityLevel
{
    WM_PRIORITY_URGENT = 0,
    WM_PRIORITY_HIGH,
    WM_PRIORITY_NORMAL,
    WM_PRIORITY_LOW
}
WMPriorityLevel;

// ARM7 側の MP 送信処理が始まるデフォルトの V Count 値
#define WM_VALARM_COUNT_CHILD_MP  240  // 子機MP処理
#define WM_VALARM_COUNT_PARENT_MP 260  // 親機MP処理
#define WM_VALARM_COUNT_RANGE_TOP    220        // parentVCount, childVCount の設定可能上限 (220〜262, 0〜190)
#define WM_VALARM_COUNT_RANGE_BOTTOM 190        // parentVCount, childVCount の設定可能下限

// 送信間隔
#define WM_MP_FREQ_CONT         16     // 連続送信になる送信頻度
#define WM_MP_COUNT_LIMIT       256    // 残り送信回数カウンタの上限値
#define WM_DEFAULT_MP_FREQ_LIMIT 6     // デフォルトの MP 頻度上限
#define WM_DEFAULT_MP_PARENT_INTERVAL 1000      // デフォルトの親機の MP 間隔 (us)
#define WM_DEFAULT_MP_CHILD_INTERVAL  0 // デフォルトの子機の MP 間隔 (us)

// 各種データサイズ(サイズの単位はバイト)
#define WM_SIZE_BSSID           6
#define WM_SIZE_SSID            32

#define WM_SIZE_GAMEINFO        128    // 最大サイズ

#define WM_SIZE_SCAN_EX_BUF     1024   // StartScanEx用バッファサイズ

#define WM_GAMEINFO_LENGTH_MIN  16     // 有効な GameInfo の最低サイズ
#define WM_GAMEINFO_MAGIC_NUMBER 0x0001 // DS 用 GameInfo の識別用 magic number
#define WM_GAMEINFO_VERSION_NUMBER 1   // GameInfo のバージョン番号
#define WM_GAMEINFO_PLATFORM_ID_NITRO       0   // 親機は DS である
#define WM_GAMEINFO_PLATFORM_ID_REVOLUTION  8   // 親機は Wii である
#define WM_SIZE_SYSTEM_GAMEINFO 16     // userGameInfoLengthまでのサイズ
#define WM_SIZE_USER_GAMEINFO   112    // GameInfoのユーザー領域の最大サイズ

#define WM_SIZE_SCAN_PARAM      32

#define WM_PARENT_PARAM_SIZE    64
#define WM_SIZE_USERNAME        8
#define WM_SIZE_GAMENAME        16
#define WM_SIZE_GGID            4

#define WM_SIZE_WL_VERSION      8
#define WM_SIZE_BBP_VERSION     4
#define WM_SIZE_MACADDR         6
#define WM_SIZE_WEPKEY          80

#define WM_SIZE_CHILD_SSID      24

#if ( SDK_VERSION_WL >= 21100 )
#define WM_SIZE_MP_DATA_MAX     512
#else
#define WM_SIZE_MP_DATA_MAX     508
#endif

#define WM_SIZE_MADATA_HEADER   44

// 親機が複数のパケットを1回のMP通信に入れる際にパケット間に必要なバイト数
#define WM_SIZE_MP_PARENT_PADDING   (WM_HEADER_SIZE + WM_HEADER_PARENT_MAX_SIZE)
// 子機が複数のパケットを1回のMP通信に入れる際にパケット間に必要なバイト数
#define WM_SIZE_MP_CHILD_PADDING    (WM_HEADER_SIZE + WM_HEADER_CHILD_MAX_SIZE)

#define WM_SIZE_DS_PARENT_HEADER    4
#define WM_SIZE_KS_PARENT_DATA  (2*16+WM_SIZE_DS_PARENT_HEADER)
#define WM_SIZE_KS_CHILD_DATA   2

// アトリビュートフラグ

#define WM_ATTR_ENTRY_SHIFT     0
#define WM_ATTR_MB_SHIFT        1
#define WM_ATTR_KS_SHIFT        2
#define WM_ATTR_CS_SHIFT        3

#define WM_ATTR_FLAG_ENTRY      (1 << WM_ATTR_ENTRY_SHIFT)
#define WM_ATTR_FLAG_MB         (1 << WM_ATTR_MB_SHIFT)
#define WM_ATTR_FLAG_KS         (1 << WM_ATTR_KS_SHIFT)
#define WM_ATTR_FLAG_CS         (1 << WM_ATTR_CS_SHIFT)

// コールバック排他制御用
#define WM_EXCEPTION_CB_MASK    0x0001

// wmHeader
#define WM_HEADER_SIZE          2      // 親子共通 wmHeader のバイト数

//#define WM_HEADER_MP        0x0001
#define WM_HEADER_KS        0x4000
#define WM_HEADER_VSYNC     0x8000

// packetHeader
#define WM_HEADER_PARENT_MAX_SIZE   4  // ヘッダとして最大で付加されるバイト数 (wmHeader 2byte 分は除く)
                                        // 現在は Sequence Number と Destination Bitmap が付加される場合がある

#define WM_HEADER_CHILD_MAX_SIZE    2  // ヘッダとして最大で付加されるバイト数 (wmHeader 2byte 分は除く)
                                        // 現在は Sequence Number が付加される場合がある

#define WM_HEADER_PORT_MASK     0x0f00
#define WM_HEADER_PORT_SHIFT    8
#define WM_HEADER_SEQ_FLAG      0x0800
#define WM_HEADER_DEST_BITMAP   0x1000
#define WM_HEADER_LENGTH_MASK   0x00ff
#define WM_HEADER_LENGTH_SCALE  2

#define WM_SEQ_PORT_FLAG        0x0008
#define WM_SEQ_PORT_MASK        0x0007

// packetHeader Seq No Field ( 2byte目 )
#define WM_HEADER_SEQ_RETRY     0x8000
#define WM_HEADER_SEQ_NUM_MASK  0x7fff

// 1つのエンベローブにパック可能な最大パケット数(>= WM_SIZE_MP_DATA_MAX/WM_SIZE_MP_PARENT_PADDING + 1)
#define WM_PACKED_PACKET_MAX    128


// APIのリザルトコード
typedef enum WMErrCode
{
    WM_ERRCODE_SUCCESS = 0,
    WM_ERRCODE_FAILED = 1,
    WM_ERRCODE_OPERATING = 2,
    WM_ERRCODE_ILLEGAL_STATE = 3,
    WM_ERRCODE_WM_DISABLE = 4,
    WM_ERRCODE_NO_KEYSET = 5,
    WM_ERRCODE_NO_DATASET = 5,         // NO_KEYSET と同じ値
    WM_ERRCODE_INVALID_PARAM = 6,
    WM_ERRCODE_NO_CHILD = 7,
    WM_ERRCODE_FIFO_ERROR = 8,
    WM_ERRCODE_TIMEOUT = 9,
    WM_ERRCODE_SEND_QUEUE_FULL = 10,
    WM_ERRCODE_NO_ENTRY = 11,
    WM_ERRCODE_OVER_MAX_ENTRY = 12,
    WM_ERRCODE_INVALID_POLLBITMAP = 13,
    WM_ERRCODE_NO_DATA = 14,
    WM_ERRCODE_SEND_FAILED = 15,

    WM_ERRCODE_DCF_TEST,               // デバッグ用
    WM_ERRCODE_WL_INVALID_PARAM,       // デバッグ用 (削除可)
    WM_ERRCODE_WL_LENGTH_ERR,          // デバッグ用 (削除可)

    WM_ERRCODE_FLASH_ERROR,            // WLのフラッシュパラメータエラー(致命的なのでフリーズする)
    WM_ERRCODE_MAX
}
WMErrCode;

// APIのコールバックで返すステートコード
typedef enum WMStateCode
{
    WM_STATECODE_PARENT_START = 0,     // StartParent用 親機動作開始
    WM_STATECODE_BEACON_SENT = 2,      // StartParent用 Beacon送信完了

    WM_STATECODE_SCAN_START = 3,       // StartScan用 Scan動作開始
    WM_STATECODE_PARENT_NOT_FOUND = 4, // StartScan用 親機を発見できない
    WM_STATECODE_PARENT_FOUND = 5,     // StartScan用 親機を発見した

    WM_STATECODE_CONNECT_START = 6,    // StartConnect用 接続処理開始
    WM_STATECODE_BEACON_LOST = 8,      // StartConnect用 親機のビーコンを見失った

    WM_STATECODE_CONNECTED = 7,        // StartParent及びStartConnect, port 用 接続完了
    WM_STATECODE_CHILD_CONNECTED = 7,  // WM_STATECODE_CONNECTED と同じ値
    WM_STATECODE_DISCONNECTED = 9,     // StartParent及びStartConnect, port 用 切断通知
    WM_STATECODE_DISCONNECTED_FROM_MYSELF = 26, // StartParent及びStartConnect, port 用 自分からの切断通知

    WM_STATECODE_MP_START = 10,        // StartMP用 MP通信モード開始
    WM_STATECODE_MPEND_IND = 11,       // StartMP用 親機MPシーケンス完了(受信)
    WM_STATECODE_MP_IND = 12,          // StartMP用 子機MP受信
    WM_STATECODE_MPACK_IND = 13,       // StartMP用 子機MPACK受信

    WM_STATECODE_DCF_START = 14,       // StartDCF用 DCF通信モード開始
    WM_STATECODE_DCF_IND = 15,         // StartDCF用 DCFデータ受信

    WM_STATECODE_BEACON_RECV = 16,     // Beacon受信indicate
    WM_STATECODE_DISASSOCIATE = 17,    // 接続切断indicate
    WM_STATECODE_REASSOCIATE = 18,     // 再接続indicate
    WM_STATECODE_AUTHENTICATE = 19,    // 認証確認indicate

    WM_STATECODE_PORT_INIT = 25,       // port 初期化
    WM_STATECODE_PORT_SEND = 20,       // port へデータ送信
    WM_STATECODE_PORT_RECV = 21,       // port へデータ受信

    WM_STATECODE_FIFO_ERROR = 22,      // ARM7でFIFOエラー発生通知
    WM_STATECODE_INFORMATION = 23,     // 情報通知
    WM_STATECODE_UNKNOWN = 24,         // 不特定の通知

    WM_STATECODE_MAX = 27
}
WMStateCode;

// WMのステートコード
typedef enum WMState
{
    WM_STATE_READY = 0,                // READYステート(Init前、LED通常点灯状態)
    WM_STATE_STOP,                     // STOPステート (Initialize前、LED点滅状態)
    WM_STATE_IDLE,                     // IDLEステート (Initialize後, Reset後など)
    WM_STATE_CLASS1,                   // CLASS1ステート
    WM_STATE_TESTMODE,                 // TESTMODE(申請用)
    WM_STATE_SCAN,                     // StartScan中
    WM_STATE_CONNECT,                  // StartConnect中
    WM_STATE_PARENT,                   // StartParent中
    WM_STATE_CHILD,                    // 接続完了後の子機
    WM_STATE_MP_PARENT,                // StartMP後の親機
    WM_STATE_MP_CHILD,                 // StartMP後の子機
    WM_STATE_DCF_CHILD,                // StartDCF後の子機
    WM_STATE_TESTMODE_RX,              // TESTMODE_RX

    WM_STATE_MAX
}
WMState;

// リンク強度
typedef enum WMLinkLevel
{
    WM_LINK_LEVEL_0 = 0,
    WM_LINK_LEVEL_1,
    WM_LINK_LEVEL_2,
    WM_LINK_LEVEL_3,
    WM_LINK_LEVEL_MAX
}
WMLinkLevel;

// Data Sharing の内部状態
typedef enum WMDataSharingState
{
    WM_DS_STATE_READY = 0,             // WM_StartDataSharing 前
    WM_DS_STATE_START,                 // WM_StepDataSharing 中
    WM_DS_STATE_PAUSING,               // Pause 状態への遷移中
    WM_DS_STATE_PAUSED,                // WM_StartDataSharing 直後または Pause 中
    WM_DS_STATE_RETRY_SEND,            // SEND_QUEUE_FULL だったので再送が必要
    WM_DS_STATE_ERROR                  // Error 発生
}
WMDataSharingState;

// 切断時の reason コード
typedef enum WMDisconnectReason
{
    // 外部要因による切断に関する reason 値
    WM_DISCONNECT_REASON_RESERVED = 0, // 予約
    WM_DISCONNECT_REASON_UNSPECIFIED = 1,       // 特定できないエラー
    WM_DISCONNECT_REASON_PREV_AUTH_INVALID = 2, // 直前の認証はもはや有効でありません
    WM_DISCONNECT_REASON_DEAUTH_LEAVING = 3,    // BSS から離れたため認証を解除しました
    WM_DISCONNECT_REASON_INACTIVE = 4, // 不活性のため接続を解除しました
    WM_DISCONNECT_REASON_UNABLE_HANDLE = 5,     // AP に十分な資源がないため接続を解除しました
    WM_DISCONNECT_REASON_RX_CLASS2_FROM_NONAUTH_STA = 6,        // 認証されていない STA から Class2 のフレームを受信しました
    WM_DISCONNECT_REASON_RX_CLASS3_FROM_NONASSOC_STA = 7,       // 接続されていない STA から Class3 のフレームを受信しました
    WM_DISCONNECT_REASON_DISASSOC_LEAVING = 8,  // BSS から離れたためアソシエーションを解除しました
    WM_DISCONNECT_REASON_ASSOC_STA_NOTAUTHED = 9,       // 接続要求した STA はまだ認証を受けていません
    // DS のローカル通信固有の reason 値
    WM_DISCONNECT_REASON_NO_ENTRY = 19, // DS の親機は現在エントリーを受け付けていません
    // ライブラリ内部からの自動切断に関する reason 値
    WM_DISCONNECT_REASON_MP_LIFETIME = 0x8001,  // MP 通信ライフタイムが切れました
    WM_DISCONNECT_REASON_TGID_CHANGED = 0x8002, // 受信 beacon 中の TGID が変化しました
    WM_DISCONNECT_REASON_FATAL_ERROR = 0x8003,  // ライブラリ内で回復不能なエラーが発生しました
    // ライブラリによる切断に関する reason 値
    WM_DISCONNECT_REASON_FROM_MYSELF = 0xf001   // WM の API を使用し、自分から切断しました
}
WMDisconnectReason;

// 各種通知
typedef enum WMInfoCode
{
    WM_INFOCODE_NONE = 0,
    WM_INFOCODE_FATAL_ERROR            // Fatal Error 発生通知
}
WMInfoCode;

// WM_SetMPParameter 関数の mask 用定数
#define WM_MP_PARAM_MIN_FREQUENCY              0x0001
#define WM_MP_PARAM_FREQUENCY                  0x0002
#define WM_MP_PARAM_MAX_FREQUENCY              0x0004
#define WM_MP_PARAM_PARENT_SIZE                0x0008
#define WM_MP_PARAM_CHILD_SIZE                 0x0010
#define WM_MP_PARAM_PARENT_INTERVAL            0x0020
#define WM_MP_PARAM_CHILD_INTERVAL             0x0040
#define WM_MP_PARAM_PARENT_VCOUNT              0x0080
#define WM_MP_PARAM_CHILD_VCOUNT               0x0100
#define WM_MP_PARAM_DEFAULT_RETRY_COUNT        0x0200
#define WM_MP_PARAM_MIN_POLL_BMP_MODE          0x0400
#define WM_MP_PARAM_SINGLE_PACKET_MODE         0x0800
#define WM_MP_PARAM_IGNORE_FATAL_ERROR_MODE    0x1000
#define WM_MP_PARAM_IGNORE_SIZE_PRECHECK_MODE  0x2000

// MP 通信中には設定できないパラメータのリスト
#define WM_MP_PARAM_MUST_SET_BEFORE_MP      (WM_MP_PARAM_MIN_POLL_BMP_MODE|WM_MP_PARAM_SINGLE_PACKET_MODE|WM_MP_PARAM_IGNORE_SIZE_PRECHECK_MODE)

// WMMPTmpParam.mask 用定数
#define WM_MP_TMP_PARAM_MIN_FREQUENCY              0x0001
#define WM_MP_TMP_PARAM_FREQUENCY                  0x0002
#define WM_MP_TMP_PARAM_MAX_FREQUENCY              0x0004
#define WM_MP_TMP_PARAM_DEFAULT_RETRY_COUNT        0x0200
#define WM_MP_TMP_PARAM_MIN_POLL_BMP_MODE          0x0400
#define WM_MP_TMP_PARAM_SINGLE_PACKET_MODE         0x0800
#define WM_MP_TMP_PARAM_IGNORE_FATAL_ERROR_MODE    0x1000

// パラメータの限界値
#define WM_MP_PARAM_INTERVAL_MAX 10000 // interval に設定可能な値は 10000us 以下

// miscFlags 用定数
#define WM_MISC_FLAG_LISTEN_ONLY               0x0001
#define WM_MISC_FLAG_NO_BLINK                  0x0002

// WM APIのコールバックの型
typedef void (*WMcallbackFunc) (void *arg);     // WM APIのコールバック型
typedef void (*WMCallbackFunc) (void *arg);     // WM APIのコールバック型

// バッファサイズ =======================================================================

//┌──────────┐
//│ WM9 variable       │ 512
//├──────────┤
//│ WM7 variable       │ 256 + 512
//├──────────┤
//│ WM status          │ 2048
//├──────────┤
//│ FIFO buf (9 → 7)  │ 256
//├──────────┤
//│ FIFO buf (9 ← 7)  │ 256
//└──────────┘

// WMで使用するバッファのサイズ -------------------

#define WM_ARM9WM_BUF_SIZE      512    // ARM9側WMの変数領域のサイズ
#define WM_ARM7WM_BUF_SIZE      ( 256 + 512 )   // ARM7側WMの変数領域のサイズ
#define WM_STATUS_BUF_SIZE      2048   // WMのステータス領域のサイズ
#define WM_FIFO_BUF_SIZE        256    // ARM9,7間のFIFOコマンドバッファサイズ

// ユーザーが確保するバッファのサイズ -------------
//   WMシステムで使用する全バッファのサイズ 512 + 256 + 512 + 2048 + 256 + 256 = 0xf00 (FIFO_BIF * 2 であることに注意)
#define WM_SYSTEM_BUF_SIZE      (WM_ARM9WM_BUF_SIZE + WM_ARM7WM_BUF_SIZE + WM_STATUS_BUF_SIZE + WM_FIFO_BUF_SIZE + WM_FIFO_BUF_SIZE)
#define WM_BSS_DESC_SIZE        192    // WM_StartScan()で引き渡す、親機情報格納用バッファのサイズ

#define WM_DCF_MAX_SIZE         1508   // DCFで受信できる最大サイズ
#define WM_KEYSET_SIZE          36     // KeySetのサイズ
#define WM_KEYDATA_SIZE         2      // KeyDataのサイズ

#define WM_DS_HEADER_SIZE       4

#ifdef WM_DS_DATA_SIZE_252             // WM_DS_DATA_SIZE == 252 で構わない場合
#define WM_DS_DATA_SIZE         252
#else  // WM_DS_DATA_SIZE == 508 の場合（デフォルト）
#define WM_DS_DATA_SIZE         508    // 1つの WMDataSet の最大データサイズ(512-4)
                                       // WM_DS_DATA_SIZE+WM_DS_HEADER_SIZE が 32 の倍数の必要有
#endif

#define WM_DS_DATASET_NUM       4      // WMDataSetBuf にいくつの WMDataSet を保持するか(固定値)
#define WM_DS_INFO_SIZE         (sizeof(WMDataSharingInfo))     // DataSharing用バッファサイズ

#define WM_SCAN_EX_PARENT_MAX   16     // 1回のScanExで発見可能な最大親機台数
#define WM_SCAN_OTHER_ELEMENT_MAX 16   // 通知可能なotherElementの最大数

// 無線通信の使用可否
#ifdef SDK_TWL
#define WM_WIRELESS_COMM_FLAG_OFF       0  // 無線通信 オンモード
#define WM_WIRELESS_COMM_FLAG_ON        1  // 無線通信 オフモード
#define WM_WIRELESS_COMM_FLAG_UNKNOWN   2  // DS 上で実行しているため判別不能 
#endif

/*---------------------------------------------------------------------------*
    構造体定義
 *---------------------------------------------------------------------------*/

// バッファ構造体 =======================================================================

// データセット構造体
typedef struct WMDataSet
{
//    u16 seqNum;                 // データセットのシーケンシャルナンバー
    u16     aidBitmap;
    u16     receivedBitmap;            // 受信状態のビットマップ
    u16     data[WM_DS_DATA_SIZE / sizeof(u16)];        // 共有データ

}
WMDataSet;

// DataSharing 情報構造体
typedef struct WMDataSharingInfo
{
    WMDataSet ds[WM_DS_DATASET_NUM];
    u16     seqNum[WM_DS_DATASET_NUM]; // Sequential Number
    u16     writeIndex;                // 次に書き込みを行う Index
    u16     sendIndex;                 // 現在送信中の Index
    u16     readIndex;                 // 次に読み出される Index
    u16     aidBitmap;                 // データ共有の仲間の bitmap（自分含む）
    u16     dataLength;                // 1台辺りの共有データ量
    u16     stationNumber;             // データ共有に参加している台数 (aidBitmap の立っているビットの数)
    u16     dataSetLength;             // dataLength * stationNumber
    u16     port;                      // 使用している port (8番以上)
    u16     doubleMode;                // 毎フレーム送受信可能な交互通信モードかどうか
    u16     currentSeqNum;             // 直前に読み出した DataSet の SeqNum
    u16     state;                     // 現在の DataSharing の状態(WMDataSharingState)
    u16     reserved[1];               // for 32Byte Alignment
}
WMDataSharingInfo;

// キーセット構造体
typedef struct WMKeySet
{
    u16     seqNum;                    // キーセットのシーケンシャルナンバー
    u16     rsv;
    u16     key[16];                   // キーデータ

}
WMKeySet, WMkeySet;

// キーセット保存用リングバッファ
typedef WMDataSharingInfo WMKeySetBuf, WMkeySetBuf;

//---------------------------------------
// 親子共通 MP port 送信キュー構造体
typedef struct
{
    u16     next;
    u16     port;
    u16     destBitmap;
    u16     restBitmap;
    u16     sentBitmap;
    u16     sendingBitmap;
    u16     padding;
    u16     size;
    u16     seqNo;
    u16     retryCount;
    const u16 *data;
    WMCallbackFunc callback;
    void   *arg;
}
WMPortSendQueueData;

typedef struct
{
    u16     head;
    u16     tail;
}
WMPortSendQueue;

//---------------------------------------
// 子機 MP受信バッファ構造体
typedef struct WMMpRecvBuf
{
    u16     rsv1[3];
    u16     length;

    u16     rsv2[1];
    u16     ackTimeStamp;
    u16     timeStamp;
    u16     rate_rssi;
    u16     rsv3[2];

    u16     rsv4[2];
    u8      destAdrs[6];
    u8      srcAdrs[6];
    u16     rsv5[3];
    u16     seqCtrl;

    u16     txop;
    u16     bitmap;
    u16     wmHeader;
    u16     data[2];

}
WMMpRecvBuf, WMmpRecvBuf;

//---------------------------------------
// 親機 MP受信バッファ構造体
typedef struct WMMpRecvData
{
    u16     length;
    u16     rate_rssi;
    u16     aid;
    u16     noResponse;
    u16     wmHeader;
    u16     cdata[1];

}
WMMpRecvData, WMmpRecvData;

typedef struct WMMpRecvHeader
{
    u16     bitmap;
#if SDK_VERSION_WL >= 20500
    u16     errBitmap;
#endif
    u16     count;
    u16     length;
    u16     txCount;
    WMMpRecvData data[1];

}
WMMpRecvHeader, WMmpRecvHeader;

//---------------------------------------
// 子機 DCF受信バッファ構造体
typedef struct WMDcfRecvBuf
{
    u16     frameID;
    u16     rsv1[2];
    u16     length;

    u16     rsv2[3];
    u16     rate_rssi;
    u16     rsv3[4];
    u8      destAdrs[6];
    u8      srcAdrs[6];
    u16     rsv4[4];

    u16     data[2];

}
WMDcfRecvBuf, WMdcfRecvBuf;

//---------------------------------------
// WM 親機パラメータ構造体
typedef struct WMParentParam
{
    u16    *userGameInfo;              // ユーザー領域にセットするデータのポインタ
    u16     userGameInfoLength;        // ユーザー領域の長さ
    u16     padding;

    u32     ggid;
    u16     tgid;                      // テンポラリのGameID(ユーザーが毎回親機になるときに生成する)
    u16     entryFlag;                 // 1:接続可, 0:接続不可
    u16     maxEntry;                  // 接続可能子機台数
    u16     multiBootFlag;             // 1:マルチブート可, 0:マルチブート不可
    u16     KS_Flag;                   // 1:KeySharingあり, 0:KeySharing無し
    u16     CS_Flag;                   // 1:連続送信, 0:1フレーム毎の送信
    u16     beaconPeriod;              // Beacon間隔(ms)
    u16     rsv1[WM_SIZE_USERNAME / sizeof(u16)];       //
    u16     rsv2[WM_SIZE_GAMENAME / sizeof(u16)];       //
    u16     channel;                   // チャンネル
    u16     parentMaxSize;             // 親機最大送信サイズ
    u16     childMaxSize;              // 子機最大送信サイズ

    u16     rsv[4];                    // 32バイトアライメントのため

}
WMParentParam, WMpparam;

//---------------------------------------
// WM GameInfo構造体

typedef struct WMGameInfo
{
    u16     magicNumber;               // == 0x0001
    u8      ver;                       // GameInfo Version 1
    u8      platform;                  // DS: 0, Wii: 8
    u32     ggid;
    u16     tgid;
    u8      userGameInfoLength;        // ユーザー領域の長さ
    union
    {
        // (CS_Flag):1, (KS_Flag):1, multibootFlag:1, entryFlag:1
        u8      gameNameCount_attribute;        // 互換のための旧名
        u8      attribute;
    };
    u16     parentMaxSize;             //親機最大送信サイズ
    u16     childMaxSize;              //子機最大送信サイズ
    union
    {
        u16     userGameInfo[WM_SIZE_USER_GAMEINFO / sizeof(u16)];      // ユーザー領域(最大112バイト (暫定))
        struct
        {
            u16     userName[WM_SIZE_USERNAME / sizeof(u16)];   // WM_SIZE_USERNAME -> 8
            u16     gameName[WM_SIZE_GAMENAME / sizeof(u16)];   // WM_SIZE_GAMENAME -> 16
            u16     padd1[44];         // pad -> 88 
        }
        old_type;                      // 計 112バイト
    };
}
WMGameInfo, WMgameInfo;

//---------------------------------------
// BSSDescription 親機情報構造体(Scanで獲得し、Connectでの指定に使用する)
#if SDK_VERSION_WL < 17100
typedef struct WMBssDesc
{
    u16     length;                    // 2
    u16     rssi;                      // 4
    u8      bssid[WM_SIZE_BSSID];      // 10
    u16     ssidLength;                // 12
    u8      ssid[WM_SIZE_SSID];        // 44
    u16     capaInfo;                  // 46
    struct
    {
        u16     basic;                 // 48
        u16     support;               // 50
    }
    rateSet;
    u16     beaconPeriod;              // 52
    u16     dtimPeriod;                // 54
    u16     channel;                   // 56
    u16     cfpPeriod;                 // 58
    u16     cfpMaxDuration;            // 60
    u16     gameInfoLength;            // 62
    WMGameInfo gameInfo;               // 190
    u16     rsv;                       // 192

}
WMBssDesc, WMbssDesc;
#else
typedef struct WMBssDesc
{
    u16     length;                    // 2
    u16     rssi;                      // 4
    u8      bssid[WM_SIZE_BSSID];      // 10
    u16     ssidLength;                // 12
    u8      ssid[WM_SIZE_SSID];        // 44
    u16     capaInfo;                  // 46
    struct
    {
        u16     basic;                 // 48
        u16     support;               // 50
    }
    rateSet;
    u16     beaconPeriod;              // 52
    u16     dtimPeriod;                // 54
    u16     channel;                   // 56
    u16     cfpPeriod;                 // 58
    u16     cfpMaxDuration;            // 60
    u16     gameInfoLength;            // 62
    u16     otherElementCount;         // 64
    WMGameInfo gameInfo;               // 192

}
WMBssDesc, WMbssDesc;
#endif

//---------------------------------------
// WM ビーコン内拡張エレメント構造体
typedef struct WMOtherElements
{
    u8      count;
    u8      rsv[3];
    struct
    {
        u8      id;
        u8      length;
        u8      rsv[2];
        u8     *body;
    }
    element[WM_SCAN_OTHER_ELEMENT_MAX];
}
WMOtherElements;

//---------------------------------------
// WM Scanパラメータ構造体
typedef struct WMScanParam
{
    WMBssDesc *scanBuf;                // 親機情報を格納するためのバッファ
    u16     channel;                   // Scanを行うchannel
    u16     maxChannelTime;            // 最大Scan時間
    u8      bssid[WM_SIZE_BSSID];      // Scan対象親機MacAddress(0xffなら全親機対象)
    u16     rsv[9];

}
WMScanParam, WMscanParam;

//---------------------------------------
// WM ScanExパラメータ構造体
typedef struct WMScanExParam
{
    WMBssDesc *scanBuf;                // 親機情報を格納するためのバッファ
    u16     scanBufSize;               // scanBufのサイズ
    u16     channelList;               // Scanを行うchannelリスト(複数指定可 WM_GetAllowedChannelの戻り値と同様の形式)
    u16     maxChannelTime;            // 最大Scan時間
    u8      bssid[WM_SIZE_BSSID];      // Scan対象親機MacAddress(0xffなら全親機対象)
    u16     scanType;                  // 通常はWM_SCANTYPE_PASSIVE。ActiveScan時はWM_SCANTYPE_ACTIVE。
    u16     ssidLength;                // 対象親機のSSID長(0の場合は全親機が対象になる)
    u8      ssid[WM_SIZE_SSID];        // Scan対象親機のSSID
#if WM_SSID_MASK_CUSTOMIZE
    u16     ssidMatchLength;
    u16     rsv2[7];
#else
    u16     rsv2[8];
#endif

}
WMScanExParam, WMscanExParam;

//---------------------------------------
// WM SetMPParameter パラメータ構造体
typedef struct WMMPParam
{
    u32     mask;                      // 以下のどのフィールドが有効かをビットで指定する

    u16     minFrequency;              // 1ピクチャフレームあたりの MP 回数(最小値)(未実装)
    u16     frequency;                 // 1ピクチャフレームあたりの MP 回数
    u16     maxFrequency;              // 1ピクチャフレームあたりの MP 回数(上限値)
    u16     parentSize;                // 現在の親機送信サイズ
    u16     childSize;                 // 現在の子機送信サイズ
    u16     parentInterval;            // 1フレーム中に連続して行う場合の親機の MP 通信間隔 (us)
    u16     childInterval;             // 1フレーム中に連続して行う場合の子機が MP 通信を準備する間隔 (us)
    u16     parentVCount;              // フレーム同期通信時の親機処理の開始 V Count
    u16     childVCount;               // フレーム同期通信時の子機処理の開始 V Count
    u16     defaultRetryCount;         // Raw 通信 port での通信のデフォルトリトライ回数
    u8      minPollBmpMode;            // PollBitmap を送信するパケットの送信先に合わせて最小にするモード
    u8      singlePacketMode;          // 1MP に 1packet しか送らなくなるモード
    u8      ignoreFatalErrorMode;      // Fatal Error を無視するモード
    u8      ignoreSizePrecheckMode;    // 通信開始時の送受信サイズの事前チェックを無視するモード
}
WMMPParam;

//---------------------------------------
// WM StartMP 用一時パラメータ構造体
typedef struct WMMPTmpParam
{
    u32     mask;                      // 以下のどのフィールドが有効かをビットで指定する

    u16     minFrequency;              // 1ピクチャフレームあたりの MP 回数(最小値)(未実装)
    u16     frequency;                 // 1ピクチャフレームあたりの MP 回数
    u16     maxFrequency;              // 1ピクチャフレームあたりの MP 回数(上限値)
    u16     defaultRetryCount;         // Raw 通信 port での通信のデフォルトリトライ回数
    u8      minPollBmpMode;            // PollBitmap を送信するパケットの送信先に合わせて最小にするモード
    u8      singlePacketMode;          // 1MP に 1packet しか送らなくなるモード
    u8      ignoreFatalErrorMode;      // Fatal Error を無視するモード
    u8      reserved[1];
}
WMMPTmpParam;

//---------------------------------------
// WM ステータスバッファ構造体 (書き換えはARM7で行う。ARM9は読み出しのみ可)
typedef struct WMStatus
{
    // ステート情報 (4Byte)
    u16     state;                     // WMのステート情報
    u16     BusyApiid;                 // 実行中APIID

    // フラグ (32Byte) 36
    BOOL    apiBusy;                   // API実行中フラグ(FIFOによるAPI発行禁止フラグ)
    BOOL    scan_continue;             // Scan継続フラグ(2004/07/19現在、未使用)
    BOOL    mp_flag;                   // MP実行中フラグ
    BOOL    dcf_flag;                  // DCF実行中フラグ
    BOOL    ks_flag;                   // KeySharing実行中フラグ
    BOOL    dcf_sendFlag;              // DCF送信待ちフラグ
    BOOL    VSyncFlag;                 // VBlank同期OKフラグ

    // バージョン情報 (16Byte) 52
    u8      wlVersion[WM_SIZE_WL_VERSION];      // WLのバージョン
    u16     macVersion;                // MACのバージョン
    u16     rfVersion;                 // RFのバージョン
    u16     bbpVersion[WM_SIZE_BBP_VERSION / sizeof(u16)];      // BBPのバージョン

    // MP関係 (106Byte) 158
    u16     mp_parentSize;             // 親機の1回の MP 通信での送信サイズ
    u16     mp_childSize;              // 子機の1回の MP 通信での送信サイズ
    u16     mp_parentMaxSize;          // 親機の1回の MP 通信での送信サイズの最大値
    u16     mp_childMaxSize;           // 子機の1回の MP 通信での送信サイズの最大値
    u16     mp_sendSize;               // 現在の接続での最大送信サイズ
    u16     mp_recvSize;               // 現在の接続での最大受信サイズ
    u16     mp_maxSendSize;            // 現在の接続での最大送信サイズ (parentMaxSize+4 or childMaxSize+2)
    u16     mp_maxRecvSize;            // 現在の接続での最大受信サイズ (childMaxSize+2 or parentMaxSize+4)
    u16     mp_parentVCount;           // 親機側の MP 処理開始 VCount
    u16     mp_childVCount;            // 子機側の MP 処理開始 VCount
    u16     mp_parentInterval;         // 親機が使用する MP 通信間隔 (us)
    u16     mp_childInterval;          // 子機が使用する MP 通信間隔 (us)

    OSTick  mp_parentIntervalTick;     // 親機が使用する MP 通信間隔 (tick)
    OSTick  mp_childIntervalTick;      // 子機が使用する MP 通信間隔 (tick)

    u16     mp_minFreq;                // 1ピクチャフレームあたりの MP 回数（最小値）(未実装)
    u16     mp_freq;                   // 1ピクチャフレームあたりの MP 回数（標準値）
    u16     mp_maxFreq;                // 1ピクチャフレームあたりの MP 回数（最大値）

    u16     mp_vsyncOrderedFlag;       // 次の MP が vsync と同期して行うよう親機から指示を受けたか
    u16     mp_vsyncFlag;              // 次の MP は vsync と同期して行われるのか
    s16     mp_count;                  // 現在のピクチャフレームであと何回 MP をするのか
    s16     mp_limitCount;             // 現在のピクチャフレームで再送も含めてあと何回 MP をしてもいいのか
    u16     mp_resumeFlag;             // Resume 実行中フラグ
    u16     mp_prevPollBitmap;         // 直前の MP での PollBitmap
    u16     mp_prevWmHeader;           // 直前の MP での wmHeader
    u16     mp_prevTxop;               // 直前の MP での txop
    u16     mp_prevDataLength;         // 直前の MP での dataLength
    u16     mp_recvBufSel;             // MP受信バッファの選択フラグ
    u16     mp_recvBufSize;            // MP受信バッファのサイズ
    WMMpRecvBuf *mp_recvBuf[2];        // MP受信バッファのポインタ
    u32    *mp_sendBuf;                // MP送信バッファのポインタ
    u16     mp_sendBufSize;            // MP送信バッファのサイズ

    u16     mp_ackTime;                // MPACK の到着予定時間
    u16     mp_waitAckFlag;            // MPACK を待っている

    u16     mp_readyBitmap;            // MP の準備が整っている子機の bitmap (親機用)

    u16     mp_newFrameFlag;           // 新しいピクチャフレームが来ると TRUE になるフラグ
    u8      reserved_b[2];
    u16     mp_sentDataFlag;           // 返信データ送信済みフラグ（子機用）
    u16     mp_bufferEmptyFlag;        // 返信データ用バッファ空フラグ（子機用）
    u16     mp_isPolledFlag;           // 直前の MP は自分が poll されていたか（子機用）

    u16     mp_minPollBmpMode;         // PollBitmap を送信するパケットの送信先に合わせて最小にするモード
    u16     mp_singlePacketMode;       // 1MP に 1packet しか送らなくなるモード
    u8      reserved_c[2];
    u16     mp_defaultRetryCount;      // Seq 制御をしないポートでの通信のデフォルトリトライ回数
    u16     mp_ignoreFatalErrorMode;   // FatalErrorを無視するかどうか
    u16     mp_ignoreSizePrecheckMode; // 通信開始時の送受信サイズの事前チェックを無効にする

    u16     mp_pingFlag;               // ping を打つタイミングで立つフラグ
    u16     mp_pingCounter;            // ping までの残りフレーム数のカウンタ

    // DCF関係 (26Byte) 184
    u8      dcf_destAdr[WM_SIZE_MACADDR];       // DCF送信先MACアドレス
    u16    *dcf_sendData;              // DCF送信データのポインタ
    u16     dcf_sendSize;              // DCF送信データサイズ
    u16     dcf_recvBufSel;            // DCF受信バッファの選択フラグ
    WMDcfRecvBuf *dcf_recvBuf[2];      // DCF受信バッファのポインタ
    u16     dcf_recvBufSize;           // DCF受信バッファのサイズ

    u16     curr_tgid;                 // 現在接続している親機のTGID(BeaconRecv.Indで使用)

    // RSSI関係 (4Byte) 206
    u16     linkLevel;
    u16     minRssi;
    u16     rssiCounter;

    // 追加のパラメータ群
    u16     beaconIndicateFlag;        // ビーコン関連通知許可フラグ
    u16     wepKeyId;                  // WEP key ID (0〜3)
    u16     pwrMgtMode;                // パワーマネジメントモード
    u32     miscFlags;                 // 細々としたフラグ

    u16     VSyncBitmap;               // V-Blank同期が取れている子機のビットマップ
    u16     valarm_queuedFlag;         // V-Alarm処理が実行キューに積まれている

    // V-Blank同期関係 (14Byte) 218
    u32     v_tsf;                     // V-Blank同期用 V_TSF値
    u32     v_tsf_bak;                 // V-Blank同期用 前回取得したV_TSF値
    u32     v_remain;                  // V-Blank同期用 調整残り
    u16     valarm_counter;            // V-Alarm割り込みの発生回数カウント用

    // 読み出し待ちフラグ (2Byte) 220
//    u16             indbuf_read_wait;       // Indicationバッファ読み出し待ちフラグ

    u8      reserved_e[2];

    // 自身のステータス (8Byte) 228
    u8      MacAddress[WM_SIZE_MACADDR];
    u16     mode;                      // 親機:WL_CMDLABEL_MODE_PARENT 子機:WL_CMDLABEL_MODE_CHILD

    // 親機のときに使用する情報 ( 64 + 90 + 2 = 156Byte) 384
    WMParentParam pparam;              // 親機パラメータ
    u8      childMacAddress[WM_NUM_MAX_CHILD][WM_SIZE_MACADDR];
    u16     child_bitmap;              // 子機接続状況(poll bitmapとして使用する)

    // 子機のときに使用する情報 (14Byte) 398
    WMBssDesc *pInfoBuf;               // Scanした親機情報格納用バッファのポインタ(ARM9APIの引数でもらう)
    u16     aid;                       // 現在のAID
    u8      parentMacAddress[WM_SIZE_MACADDR];
    u16     scan_channel;              // 現在Scan中のチャンネル

    u8      reserved_f[4];

    // WEP 関係(86Byte) 488
    u16     wepMode;                   // WEP mode (何ビットのWEPか)
    BOOL    wep_flag;                  // 相手がWEPを要求しているかどうかを示す
    u16     wepKey[WM_SIZE_WEPKEY / sizeof(u16)];       // WEP key (20Byte * 4)
    /* wepKeyIdについては40行ほど上に配置し直しました(2005/02/07 terui) */

    // その他(4Byte) 492
    u16     rate;                      // 使用rate(WL_CMDLABEL_RATE_AUTO, 1M, 2M)
    u16     preamble;                  // Long:0, Short:1

    // 各種パラメータ (4Byte) 496
    u16     tmptt;
    u16     retryLimit;

    // 利用可能チャンネル (2Byte) 498
    u16     enableChannel;

    // 利用許可チャンネル(2Byte) 500
    u16     allowedChannel;

    // port 関係 ( 1316+256+20+4+256Byte ) 1816+256+20+4+256
    u16     portSeqNo[WM_NUM_MAX_CHILD + 1][WM_NUM_OF_SEQ_PORT];        // 16*8*2=256Byte

    WMPortSendQueueData sendQueueData[WM_SEND_QUEUE_NUM];       // 32*32=1024Byte
    WMPortSendQueue sendQueueFreeList; // 4Byte
    WMPortSendQueue sendQueue[WM_PRIORITY_LEVEL];       // 16Byte
    WMPortSendQueue readyQueue[WM_PRIORITY_LEVEL];      // 16Byte
    OSMutex sendQueueMutex;            // 20 byte
    BOOL    sendQueueInUse;            // MP送信待ちフラグ

    // Null Ack タイムアウト ( 128 + 8 Byte )
    OSTick  mp_lastRecvTick[1 + WM_NUM_MAX_CHILD];
    OSTick  mp_lifeTimeTick;

    // 現在有効な MP 設定値
    // WM_StartMPEx 関数で指定した追加パラメータに
    // 永続性を持たせないようにするために、これらの設定値は
    // WM_SetMPParameter 関数で設定される永続的な値と
    // WM_StartMPEx 関数で設定される一時的な値と、二重に持たれている
    u16     mp_current_minFreq;                // 1ピクチャフレームあたりの MP 回数（最小値）(未実装)
    u16     mp_current_freq;                   // 1ピクチャフレームあたりの MP 回数（標準値）
    u16     mp_current_maxFreq;                // 1ピクチャフレームあたりの MP 回数（最大値）
    u16     mp_current_minPollBmpMode;         // PollBitmap を送信するパケットの送信先に合わせて最小にするモード
    u16     mp_current_singlePacketMode;       // 1MP に 1packet しか送らなくなるモード
    u16     mp_current_defaultRetryCount;      // Seq 制御をしないポートでの通信のデフォルトリトライ回数
    u16     mp_current_ignoreFatalErrorMode;   // FatalErrorを無視するかどうか
    u8      reserved_g[2];
}
WMStatus, WMstatus;

//---------------------------------------
// WM ARM7側の変数領域用バッファ構造体
typedef struct WMArm7Buf
{
    WMStatus *status;                  // WMのステータスバッファ
//    u32         *indbuf;        // Indicationバッファのポインタ
    u8      reserved_a[4];
    u32    *fifo7to9;                  // FIFO(ARM7→9)用バッファのポインタ
//    BOOL        *fifoFlag;      // ARM7→ARM9のFIFO送信許可フラグ
    u8      reserved_b[4];

    WMBssDesc connectPInfo;            // 接続先親機情報(190byte)

    // ARM7内部から発行するリクエスト用バッファ
    u32     requestBuf[512 / sizeof(u32)];

}
WMArm7Buf, WMarm7Buf;

//---------------------------------------
// CallbackTableの要素数
#define WM_NUM_OF_CALLBACK      ( WM_APIID_ASYNC_KIND_MAX - 2 )
// WM ARM9側の変数領域用バッファ構造体
typedef struct WMArm9Buf
{
    // 各種ポインタ(20Byte)
    WMArm7Buf *WM7;                    // ARM7側WMの変数領域 (ARM9からはアクセスしない)
    WMStatus *status;                  // WMのステータスバッファ(ARM9からはリードオンリー)
    u32    *indbuf;                    // Indicationバッファ
    u32    *fifo9to7;                  // FIFO(ARM9→7)用バッファ
    u32    *fifo7to9;                  // FIFO(ARM7→9)用バッファ

    // その他(4Byte)
    u16     dmaNo;                     // WMが使用するDMA番号
    u16     scanOnlyFlag;              // Scan しかしない動作モードフラグ

    // Callback 関係(172Byte)
    WMCallbackFunc CallbackTable[WM_NUM_OF_CALLBACK];   // コールバックテーブル(42*4 = 168Byte)
    WMCallbackFunc indCallback;

    // port 関係(134Byte)
    WMCallbackFunc portCallbackTable[WM_NUM_OF_PORT];   // 16*4 = 64Byte
    void   *portCallbackArgument[WM_NUM_OF_PORT];       // 16*4 = 64Byte
    u32     connectedAidBitmap;        // 接続先一覧(ビット操作が多いため u32 で確保)
    u16     myAid;                     // 現在の AID

    /* 計 20+4+172+134 Byte */

    u8      reserved1[WM_ARM9WM_BUF_SIZE -
                      (20 + 4 + 4 * WM_NUM_OF_CALLBACK + 4 + (4 + 4) * WM_NUM_OF_PORT + 6)];

}
WMArm9Buf, WMarm9Buf;

//==========================================================================================

// APIの引数
typedef struct WMStartScanReq
{
    u16     apiid;
    u16     channel;
    WMBssDesc *scanBuf;
    u16     maxChannelTime;
    u8      bssid[WM_SIZE_BSSID];

}
WMStartScanReq, WMstartScanReq;

typedef struct WMStartScanExReq
{
    u16     apiid;
    u16     channelList;
    WMBssDesc *scanBuf;
    u16     scanBufSize;
    u16     maxChannelTime;
    u8      bssid[WM_SIZE_BSSID];
    u16     scanType;
    u16     ssidLength;
    u8      ssid[WM_SIZE_SSID];
#if WM_SSID_MASK_CUSTOMIZE
    u16     ssidMatchLength;
    u16     rsv[2];
#else
    u16     rsv[3];
#endif
}
WMStartScanExReq, WMstartScanExReq;

typedef struct WMStartConnectReq
{
    u16     apiid;
    u16     reserved;
    WMBssDesc *pInfo;
    u8      ssid[WM_SIZE_CHILD_SSID];
    BOOL    powerSave;
    u16     reserved2;
    u16     authMode;

}
WMStartConnectReq, WMstartConnectReq;

typedef struct WMMeasureChannelReq
{
    u16     apiid;
    u16     ccaMode;
    u16     edThreshold;
    u16     channel;
    u16     measureTime;

}
WMMeasureChannelReq, WMmeasureChannelReq;

typedef struct WMSetMPParameterReq
{
    u16     apiid;
    u16     reserved;

    WMMPParam param;
}
WMSetMPParameterReq;

typedef struct WMStartMPReq
{
    u16     apiid;
    u16     rsv1;
    u32    *recvBuf;
    u32     recvBufSize;
    u32    *sendBuf;
    u32     sendBufSize;

    WMMPParam param; // unused
    WMMPTmpParam tmpParam;
}
WMStartMPReq;

typedef struct WMStartTestModeReq
{
    u16     apiid;
    u16     control;
    u16     signal;
    u16     rate;
    u16     channel;
}
WMStartTestModeReq, WMstartTestModeReq;

typedef struct WMStartTestRxModeReq
{
    u16     apiid;
    u16     channel;
}
WMStartTestRxModeReq, WMstartTestRxModeReq;


//==========================================================================================

// 通常のコールバックの引数
typedef struct WMCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;

}
WMCallback;

// WM_StartParent()のコールバックの引数
typedef struct WMStartParentCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     state;                     // PARENT_START, BEACON_SENT, CHILD_CONNECTED, DISCONNECTED
    u8      macAddress[WM_SIZE_MACADDR];        // 接続した各子機のMACアドレス
    u16     aid;
    u16     reason;                    // 切断時のreasonCode
    u8      ssid[WM_SIZE_CHILD_SSID];  // SSID (子機情報)
    u16     parentSize;
    u16     childSize;

}
WMStartParentCallback, WMstartParentCallback;

// WM_StartScan()のコールバックの引数
typedef struct WMStartScanCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     state;                     // SCAN_START, PARENT_NOT_FOUND, PARENT_FOUND
    u8      macAddress[WM_SIZE_MACADDR];        // 見つかった親機のMACアドレス
    u16     channel;
    u16     linkLevel;
    u16     ssidLength;
    u16     ssid[WM_SIZE_SSID / sizeof(u16)];
    u16     gameInfoLength;
    WMGameInfo gameInfo;

}
WMStartScanCallback, WMstartScanCallback;

// WM_StartScanEx()のコールバックの引数
typedef struct WMStartScanExCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     state;                     // SCAN_START, PARENT_NOT_FOUND, PARENT_FOUND
    u16     channelList;               // 発見・未発見に関わらず、スキャンしたチャンネルリスト
    u8      reserved[2];               // padding
    u16     bssDescCount;              // 発見された親機の数
    WMBssDesc *bssDesc[WM_SCAN_EX_PARENT_MAX];  // 親機情報の先頭アドレス
    u16     linkLevel[WM_SCAN_EX_PARENT_MAX];   // 受信電波強度

}
WMStartScanExCallback, WMstartScanExCallback;

// WM_StartConnect()のコールバックの引数
typedef struct WMStartConnectCallback
{
    u16     apiid;
    u16     errcode;                   // PARENT_NOT_FOUND(タイムアウト)
    u16     wlCmdID;
    u16     wlResult;
    u16     state;                     // CONNECT_START, BEACON_LOST, CONNECTED, DISCONNECTED
    u16     aid;                       // CONNECTEDのときのみ、自身に割り当てられたAID
    u16     reason;                    // 切断時のreasonCode
    u16     wlStatus;
    u8      macAddress[WM_SIZE_MACADDR];
    u16     parentSize;
    u16     childSize;

}
WMStartConnectCallback, WMstartConnectCallback;

// WM_Disconnect()及びWM_DisconnectChildrenのコールバック
typedef struct WMDisconnectCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     tryBitmap;
    u16     disconnectedBitmap;
}
WMDisconnectCallback;

// WM_SetMPParameter()のコールバックの引数
typedef struct WMSetMPParameterCallback
{
    u16     apiid;
    u16     errcode;
    u32     mask;
    WMMPParam oldParam;
}
WMSetMPParameterCallback;

// WM_StartMP()のコールバックの引数
typedef struct WMStartMPCallback
{
    u16     apiid;
    u16     errcode;
    u16     state;
    u8      reserved[2];               // for padding 4byte align
    WMMpRecvBuf *recvBuf;              // 受信バッファ

    // 以下、MPACK.Indのときのみ使用
    u16     timeStamp;
    u16     rate_rssi;
    u8      destAdrs[6];
    u8      srcAdrs[6];
    u16     seqNum;
    u16     tmptt;
    u16     pollbmp;
    u16     reserved2;
}
WMStartMPCallback, WMstartMPCallback;

// WM_StartDCF()のコールバックの引数
typedef struct WMStartDCFCallback
{
    u16     apiid;
    u16     errcode;
    u16     state;
    u8      reserved[2];               // for padding 4byte align
    WMDcfRecvBuf *recvBuf;             // 受信バッファ

}
WMStartDCFCallback, WMstartDCFCallback;

// WM_MeasureChannel()のコールバックの引数
typedef struct WMMeasureChannelCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     channel;
    u16     ccaBusyRatio;
}
WMMeasureChannelCallback, WMmeasureChannelCallback;

// WM_GetWirelessCounter()のコールバックの引数
typedef struct WMGetWirelessCounterCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u32     TX_Success;                // 正常に送信が行えた回数
    u32     TX_Failed;                 // 正常に送信が行えなかった回数
    u32     TX_Retry;                  // 再送を行った回数
    u32     TX_AckError;               // 希望するACKフレームを受信できなかった回数
    u32     TX_Unicast;                // 正常に送信が行えたUnicast宛フレームの数
    u32     TX_Multicast;              // 正常に送信が行えたMulticast宛フレームの数
    u32     TX_WEP;                    // 正常に送信が行えた暗号フレームの数
    u32     TX_Beacon;                 // 正常に送信が行えたBeaconフレームの数
    u32     RX_RTS;                    // (注) RTSを受信してCTSの応答を行った回数
    u32     RX_Fragment;               // フラグメントされたフレームを受信した回数
    u32     RX_Unicast;                // Unicast宛のフレームを受信した回数
    u32     RX_Multicast;              // Multicast宛のフレームを受信した回数
    u32     RX_WEP;                    // (注) 正常に復号を行えた暗号フレームの受信回数
    u32     RX_Beacon;                 // Beaconフレームを受信した回数
    u32     RX_FCSError;               // (注) FCSエラーが発生した回数
    u32     RX_DuplicateError;         // (注) 重複エラーが発生した回数
    u32     RX_MPDuplicateError;       // (注) MPフレームの重複エラーが発生した回数
    u32     RX_ICVError;               // (注) ICVエラーが発生した回数
    u32     RX_FrameCtrlError;         // (注) 不正なFrameCtrlを持つフレームを受信した回数
    u32     RX_LengthError;            // (注) 規定の長さより短いあるいは長いフレームを受信した回数
    u32     RX_PLCPError;              // (注) PLCPのCRCエラーが発生した回数
    u32     RX_BufferOverflowError;    // (注) 受信バッファがオーバーフローした回数
    u32     RX_PathError;              // (注) 受信ブロック内部のエラーが発生した回数
    u32     RX_RateError;              // (注) 受信レートが不正なフレームを受信した回数
    u32     RX_FCSOK;                  // (注) 正常なFCSを持つフレームを受信した回数
    u32     TX_MP;                     // MPフレームを送信した回数
    u32     TX_KeyData;                // MP-KeyDataフレームを送信した回数
    u32     TX_NullKey;                // MP-NullKeyフレームを送信した回数
    u32     RX_MP;                     // 正常なMPフレームを受信した回数
    u32     RX_MPACK;                  // 正常なMPACKフレームを受信した回数
    u32     MPKeyResponseError[15];    // Key要求に対して正常に応答しなかった回数
    /*
     * (注) 無線ハードウェア内のレジスタが不正にクリアされることがあるため、
     * ハードウェアのカウントレジスタを加算していくソフトカウンタは実際の合計値に
     * 満たない場合がある。(注)のついているカウンタにのみ影響がある。
     */
}
WMGetWirelessCounterCallback, WMgetWirelessCounterCallback;

// Indicationコールバックの引数
typedef struct WMIndCallback
{
    u16     apiid;
    u16     errcode;
    u16     state;
    u16     reason;

}
WMIndCallback, WMindCallback;

// MP port 通信送信コールバックの引数
typedef struct WMPortSendCallback
{
    u16     apiid;
    u16     errcode;
    u16     wlCmdID;
    u16     wlResult;
    u16     state;
    u16     port;
    u16     destBitmap;
    u16     restBitmap;
    u16     sentBitmap;
    u16     rsv;
    const u16 *data;
    union
    {
        u16     size;
        u16     length;
    };
    u16     seqNo;
    WMCallbackFunc callback;
    void   *arg;
    u16     maxSendDataSize;
    u16     maxRecvDataSize;

}
WMPortSendCallback;

// MP port 通信受信コールバックの引数
typedef struct WMPortRecvCallback
{
    u16     apiid;
    u16     errcode;
    u16     state;
    u16     port;                      // ポート番号
    WMMpRecvBuf *recvBuf;              // 受信バッファ // WMStartMPCallback との互換性のため
    u16    *data;                      // 受信データ (port 宛ての受信データはこちらを参照すること)
    u16     length;                    // 受信データ長
    u16     aid;                       // 送信元 AID
    u8      macAddress[WM_SIZE_MACADDR];
    u16     seqNo;                     // seqNo
    void   *arg;                       // callback に渡す引数 (WM9 側で追記)
    u16     myAid;                     // 自分の AID
    u16     connectedAidBitmap;        // 接続中 AID Bitmap
    u8      ssid[WM_SIZE_CHILD_SSID];  // SSID (子機情報)
    u16     reason;                    // 切断時のreasonCode
    u16     rssi;                      // RSSI
    u16     maxSendDataSize;           // 現在の送信可能サイズ
    u16     maxRecvDataSize;           // 現在の受信可能サイズ

}
WMPortRecvCallback;

// BeaconRecv.Indコールバックの引数
typedef struct WMBeaconRecvIndCallback
{
    u16     apiid;
    u16     errcode;
    u16     state;
    u16     tgid;
    u16     wmstate;
    u16     gameInfoLength;
    WMGameInfo gameInfo;
}
WMBeaconRecvIndCallback;

// WM_StartTestMode()のコールバックの引数
typedef struct WMStartTestModeCallback
{
    u16     apiid;
    u16     errcode;
    u32     RFadr5;
    u32     RFadr6;
    u16     PllLockCheck;
    u16     RFMDflag;

}
WMStartTestModeCallback;

// WM_StopTestRxMode()のコールバックの引数
typedef struct WMStopTestRxModeCallback
{
    u16     apiid;
    u16     errcode;
    u32     fcsOk;
    u32     fcsErr;
}
WMStopTestRxModeCallback;

/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* NITRO_WM_COMMON_WM_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
