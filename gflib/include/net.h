//=============================================================================
/**
 * @file	net.h
 * @brief	通信ライブラリの外部公開関数
 * @author	k.ohno
 * @date    2006.11.4
 */
//=============================================================================
#ifndef __NET_H__
#define __NET_H__


#undef GLOBAL
#ifdef __NET_SYS_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

// デバッグ用決まり文句----------------------
#define GFL_NET_DEBUG   (0)   ///< ユーザーインターフェイスデバッグ用 0:無効 1:有効

#if defined(DEBUG_ONLY_FOR_ohno)
#undef GFL_NETDEBUG
#define GFL_NET_DEBUG   (1)
#endif  //DEBUG_ONLY_FOR_ohno

#ifndef NET_PRINT
#if GFL_NET_DEBUG
#define NET_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#else   //GFL_NET_DEBUG
#define NET_PRINT(...)           ((void) 0)
#endif  // GFL_NET_DEBUG
#endif  //GFL_NET_PRINT
// デバッグ用決まり文句----------------------
// データダンプ
#ifdef GFL_NET_DEBUG
extern void CommDump_Debug(u8* adr, int length, char* pInfoStr);
#define DEBUG_DUMP(a,l,s)  CommDump_Debug(a,l,s)
#else
#define DEBUG_DUMP(a,l,s)       ((void) 0)
#endif


/// @brief 通信管理構造体
typedef struct _GFL_NETSYS GFL_NETSYS;
/// @brief ネットワーク単体のハンドル
typedef struct _GFL_NETHANDLE GFL_NETHANDLE;

// define 
#define NET_NETID_ALLUSER (0xff)  ///< NetID:全員へ送信する場合
#define NET_NETID_SERVER (0xfe)   ///< NetID:サーバーの場合これ 後は0からClientID

#define GFL_NET_TOOL_INVALID_LIST_NO  (-1) ///<無効な選択ID


/// @brief  コマンド関連の定義

/// コールバック関数の書式
typedef void (*PTRCommRecvFunc)(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
/// サイズが固定の場合サイズを関数で返す
typedef int (*PTRCommRecvSizeFunc)(void);
/// 受信バッファを外部で持っている場合そのポインタ
typedef u8* (*PTRCommRecvBuffAddr)(int netID, void* pWork, int size);

/// コマンドパケットテーブル定義
typedef struct {
    PTRCommRecvFunc callbackFunc;    ///< コマンドがきた時に呼ばれるコールバック関数
    PTRCommRecvSizeFunc getSizeFunc; ///< コマンドの送信データサイズが固定なら書いてください
    PTRCommRecvBuffAddr getAddrFunc;
} NetRecvFuncTable;

/// 可変データ送信であることを示している
#define   GFL_NET_COMMAND_SIZE_VARIABLE   (0x00ffffff)
/// コマンドのサイズを関数か実数か判断する為の定義 (使用しないでください)
#define   _GFL_NET_COMMAND_SIZE_H         (0xff000000)
/// コマンドのサイズを定義する為のマクロ
#define   GFL_NET_COMMAND_SIZE( num )     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + num)
/// コマンドのサイズを定義する為のマクロ
#define   GFL_NET_COMMAND_VARIABLE     (PTRCommRecvSizeFunc)(_GFL_NET_COMMAND_SIZE_H + num)


typedef enum {
  GFL_NET_ERROR_RESET_SAVEPOINT = 1,  ///< 直前のセーブに戻すエラー
  GFL_NET_ERROR_RESET_TITLE,          ///< タイトルに戻すエラー
  GFL_NET_ERROR_RESET_OTHER,          ///< その他のエラー
} GFL_NET_ERROR_ENUM;

typedef u8 GameServiceID;  ///< ゲームサービスID  通信の種類
typedef u8 ConnectID;      ///< 接続するためのID  0-16 まで
typedef u8 NetID;          ///< 通信ID  0-200 まで

/// @brief 送信完了を受け取るコールバック型
typedef void (*CBSendEndFunc)(u16 command);

/// @brief 常に送る送信データを得る
typedef u8* (*CBGetEveryTimeData)(void);
/// @brief 常に送られる受信データを得る 送られていないとpWorkにNULLが入る
typedef void (*CBRecvEveryTimeData)(int netID, void* pWork, int size);


typedef void* (*NetBeaconGetFunc)(void);    ///< ビーコンデータ取得関数
typedef int (*NetBeaconGetSizeFunc)(void);    ///< ビーコンデータサイズ取得関数
typedef BOOL (*NetBeaconCompFunc)(int GameServiceID1, int GameServiceID2);  ///< ビーコンのサービスを比較して繋いで良いかどうか判断する
typedef void (*NetErrorFunc)(GFL_NETHANDLE* pNet,int errNo);    ///< 通信不能なエラーが起こった場合呼ばれる 切断するしかない
typedef void (*NetConnectEndFunc)(GFL_NETHANDLE* pNet);  ///< 通信切断時に呼ばれる関数
typedef u8* (*NetGetSSID)(void);  ///< 親子接続時に認証する為のバイト列 24byte



/// @brief 通信の初期化用構造体
typedef struct{
  GameServiceID gsid;       ///< ゲームサービスID  通信の種類
  int ggid;                 ///< ＤＳでゲームソフトを区別する為のID
  u32  allocNo;             ///< allocするための番号
  NetRecvFuncTable recvFuncTable;  ///< 受信関数テーブル
  NetBeaconGetFunc beaconGetFunc;  ///< ビーコンデータ取得関数
  NetBeaconGetSizeFunc beaconGetSizeFunc;  ///< ビーコンデータサイズ取得関数
  NetBeaconCompFunc beaconCompFunc; ///< ビーコンのサービスを比較して繋いで良いかどうか判断する
  NetErrorFunc errorFunc;           ///< 通信不能なエラーが起こった場合呼ばれる 切断するしかない
  NetConnectEndFunc connectEndFunc;  ///< 通信切断時に呼ばれる関数
  NetGetSSID getSSID;        ///< 親子接続時に認証する為のバイト列  
  u8 maxConnectNum;   ///< 最大接続人数
  u8 maxBeaconNum;    ///< 最大ビーコン収集数
} GFLNetInitializeStruct;

//-------------------------------
//  関数 〜外部公開関数は全てここに定義
//-------------------------------

//==============================================================================
/**
 * @brief 通信初期化
 * @param[in]   NetInitializeStruct*  pNetInit  通信初期化構造体のポインタ
 * @retval  GFL_NETSYS 通信システムのポインタ
 */
//==============================================================================
extern GFL_NETSYS* GFL_NET_Initialize(const GFLNetInitializeStruct* pNetInit);

//==============================================================================
/**
 * @brief  通信終了
 * @param[in,out]   pNetSYS     通信システムのポインタ
 * @retval  void
 */
//==============================================================================
extern void GFL_NET_Finalize(GFL_NETSYS* pNetSYS);

//-----ビーコン関連
//==============================================================================
/**
 * @brief ビーコンを受け取るサービスを追加指定  （いくつかのサービスのビーコンを拾いたい時に）
 * @param[in,out]   NetHandle* pNet     通信ハンドルのポインタ
 * @param   GameServiceID gsid  ゲームサービスID  通信の種類
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_AddBeaconServiceID(GFL_NETHANDLE* pNet, GameServiceID gsid);

//==============================================================================
/**
 * @brief 受信ビーコンデータを得る
 * @param[in,out]   pNet     通信ハンドルのポインタ
 * @param   index  ビーコンバッファindex
 * @return  ビーコンデータの先頭ポインタ なければNULL
 */
//==============================================================================
extern void* GFL_NET_GetBeaconData(GFL_NETHANDLE* pNet, int index);

//==============================================================================
/**
 * @brief 受信ビーコンに対応したMACアドレスを得る
 * @param[in,out]   pNet     通信ハンドルのポインタ
 * @param   index  ビーコンバッファindex
 * @return  ビーコンデータの先頭ポインタ なければNULL
 */
//==============================================================================
extern u8* GFL_NET_GetBeaconMacAddress(GFL_NETHANDLE* pNet, int index);


//--------接続・切断
//==============================================================================
/**
 * @brief 子機になり接続する
 * @return  GFL_NETHANDLE  通信ハンドルのポインタ
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ClientConnect(GFL_NETSYS* pNetSYS);

//==============================================================================
/**
 * @brief 子機になり指定した親機に接続する
 * @param   macAddress     マックアドレスのバッファ
 * @return  GFL_NETHANDLE  通信ハンドルのポインタ
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ClientConnectTo(GFL_NETSYS* pNetSYS,u8* macAddress);

//==============================================================================
/**
 * @brief 親機になり待ち受ける
 * @return  GFL_NETHANDLE  通信ハンドルのポインタ
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ServerConnect(GFL_NETSYS* pNetSYS);

//==============================================================================
/**
 * @brief 親機になり指定された子機の接続を待ち受ける
 * @param   macAddress     マックアドレスのバッファ
 * @param   num            子機人数
 * @return  GFL_NETHANDLE  通信ハンドルのポインタ
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_ServerConnectTo(GFL_NETSYS* pNetSYS,const u8* macAddress, const int num);

//==============================================================================
/**
 * @brief 親子切り替え接続を行う
 * @return  GFL_NETHANDLE  通信ハンドルのポインタ
 */
//==============================================================================
extern GFL_NETHANDLE* GFL_NET_SwitchConnect(GFL_NETSYS* pNetSYS);

//==============================================================================
/**
 * @brief このハンドルの通信番号を得る
 * @param[in,out]  NetHandle* pNet     通信ハンドルのポインタ
 * @retval  NetID     通信ID
 */
//==============================================================================
extern NetID GFL_NET_GetNetID(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief 現在の接続人数を得る
 * @param[in,out]   NetHandle* pNet     通信ハンドルのポインタ
 * @retval  int  接続数
 */
//==============================================================================
extern int GFL_NET_GetConnectNum(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief IDの通信と接続しているかどうかを返す
 * @param[in,out]   NetHandle* pNet     通信ハンドルのポインタ
 * @param   NetID id            ネットID
 * @retval  BOOL  接続していたらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_IsConnectMember(GFL_NETHANDLE* pNet,NetID id);

//==============================================================================
/**
 * @brief 通信切断する
 * @param   NetHandle* pNet     通信ハンドルのポインタ
 * @retval  none
 */
//==============================================================================
//
extern void GFL_NET_Disconnect(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief 接続中かどうか
 * @param   NetHandle* pNet     通信ハンドルのポインタ
 * @retval  none
 */
//==============================================================================
extern BOOL GFL_NET_IsConnect(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief 接続数変更
 * @param   NetHandle* pNet     通信ハンドルのポインタ
 * @param   u8 num     変更数
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnectNum(GFL_NETHANDLE* pNet,u8 num);

//==============================================================================
/**
 * @brief 新規接続禁止＆許可を設定
 * @param   NetHandle* pNet     通信ハンドルのポインタ
 * @param   BOOL bEnable     TRUE=接続許可 FALSE=禁止
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SetClientConnect(GFL_NETHANDLE* pNet,BOOL bEnable);


//--------送信

//==============================================================================
/**
 * @brief 送信開始を子機に伝える（親機専用関数）
 *        この関数以降設定した送信データが子機で使用される
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @param[in]   NetID id            子機ID
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SendEnableCommand(GFL_NETHANDLE* pNet,const NetID id);

//==============================================================================
/**
 * @brief 送信許可が下りたかどうかを検査する（子機専用関数）
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @retval  TRUE   許可
 * @retval  FALSE  禁止
 */
//==============================================================================
extern BOOL GFL_NET_IsSendEnable(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief 送信開始
 *        全員に無条件で送信  送信サイズは初期化時のテーブルから引き出す
 *        データは必ずコピーする
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @param[in]   u16 sendCommand                送信するコマンド
 * @param[in]   void* data                     送信データポインタ
 * @retval  TRUE   成功した
 * @retval  FALSE  失敗の場合
 */
//==============================================================================
extern BOOL GFL_NET_SendData(GFL_NETHANDLE* pNet,const u16 sendCommand,const void* data);

//==============================================================================
/**
 * @brief 送信開始
 * @param[in,out]  pNet  通信ハンドル
 * @param[in]   sendID                     送信相手 全員へ送信する場合 NET_SENDID_ALLUSER
 * @param[in]   sendCommand                送信するコマンド
 * @param[in]   pCBSendEndFunc  送信完了をつたえるコールバック関数の登録
 * @param[in]   size                       送信データサイズ
 * @param[in]   data                     送信データポインタ
 * @param[in]   bDataCopy                 データをコピーする場合TRUE
 * @retval  TRUE   成功した
 * @retval  FALSE  失敗の場合
 */
//==============================================================================
extern BOOL GFL_NET_SendDataEx(GFL_NETHANDLE* pNet,const NetID sendID,const u16 sendCommand,
                    const CBSendEndFunc* pCBSendEndFunc,const u32 size,
                    const void* data, const BOOL bDataCopy);

//==============================================================================
/**
 * @brief 送信データが無いかどうか
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @retval  TRUE   無い場合
 * @retval  FALSE  残っている場合
 */
//==============================================================================
extern BOOL GFL_NET_IsEmptySendData(GFL_NETHANDLE* pNet);

//==============================================================================
/**
 * @brief   毎フレーム送りたいデータを登録する
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @param   pGet      データ取得関数
 * @param   pRecv     受信関数
 * @param   size      サイズ
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SetEveryTimeSendData(GFL_NETHANDLE* pNet, CBGetEveryTimeData* pGet, CBRecvEveryTimeData* pRecv,const int size);



//--------その他、ツール類
//==============================================================================
/**
 * @brief タイミングコマンドを発行する
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @param[in]   u8 no   タイミング取りたい番号
 * @return      none
 */
//==============================================================================
extern void GFL_NET_TimingSyncStart(GFL_NETHANDLE* pNet, const u8 no);
//==============================================================================
/**
 * @brief タイミングコマンドが届いたかどうかを確認する
 * @param[in,out]   NetHandle* pNet  通信ハンドル
 * @param[in]   no   届く番号
 * @retval  TRUE    タイミングが合致
 * @retval  FALSE   タイミングがそろっていない
 */
//==============================================================================
extern BOOL GFL_NET_IsTimingSync(GFL_NETHANDLE* pNet, const u8 no);


#endif // __NET_H__

