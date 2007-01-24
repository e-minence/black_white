//=============================================================================
/**
 * @file	net_system.h
 * @brief	通信システム
 * @author	Katsumi Ohno
 * @date    2005.07.08
 */
//=============================================================================

#ifndef _NET_SYSTEM_H_
#define _NET_SYSTEM_H_

// 関数切り出し自動生成 funccut.rb  k.ohno 2006.12.5 
//==============================================================================
/**
 * @brief   親機の初期化を行う
 * @param   work_area 　システムで使うメモリー領域
 *                      NULLの場合すでに初期化済みとして動作
 * @param   regulationNo  ゲームの種類
 * @param   bTGIDChange  新規のゲームの初期化の場合TRUE 古いビーコンでの誤動作を防ぐため用
 * @param   bEntry  子機を受け入れるかどうか
 * @retval  初期化に成功したらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemParentModeInit(BOOL bTGIDChange, int packetSizeMax, BOOL bEntry);
//==============================================================================
/**
 * @brief   子機の初期化を行う
 * @param   work_area 　システムで使うメモリー領域
 *                      NULLの場合はすでに初期化済みとして扱う
 * @param   regulationNo  ゲームの種類
 * @param   bBconInit  ビーコンデータを初期化するのかどうか
 * @retval  初期化に成功したらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemChildModeInit(BOOL bBconInit, int packetSizeMax);
//==============================================================================
/**
 * @brief   DSモードに切り替える
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetTransmissonTypeDS(void);
//==============================================================================
/**
 * @brief   MPモードに切り替える
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetTransmissonTypeMP(void);
//==============================================================================
/**
 * @brief   現在 DSモードかどうか
 * @param   none
 * @retval  TRUEならDS
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsTransmissonDSType(void);
//==============================================================================
/**
 * @brief   通信切断を行う
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemFinalize(void);
//==============================================================================
/**
 * @brief   子機 index接続
 * @param   index   親のリストのindex
 * @retval  子機接続を親機に送ったらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemChildIndexConnect(u16 index);
//==============================================================================
/**
 * @brief   通信データの更新処理  データを収集
 *    main.c   から  vblank後にすぐに呼ばれる
 * @param   none
 * @retval  データシェアリング同期が取れなかった場合FALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemUpdateData(void);
//==============================================================================
/**
 * @brief   通信バッファをクリアーする
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemReset(void);
//==============================================================================
/**
 * @brief   DSモードで通信バッファをクリアーする
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetDS(void);
//==============================================================================
/**
 * @brief   通信バッファをクリアーする+ビーコンの初期化を行う
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetBattleChild(void);
//==============================================================================
/**
 * @brief   通信を受信した時に呼ばれるコールバック
 * @param   result  成功か失敗
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvCallback(u16 aid, u16 *data, u16 size);
//==============================================================================
/**
 * 通信を受信した時に呼ばれるコールバック
 * @param   result  成功か失敗
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvParentCallback(u16 aid, u16 *data, u16 size);
//==============================================================================
/**
 * @brief   子機側のアクションが行われたことを設定
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemActionCommandSet(void);
//==============================================================================
/**
 * 子機送信メソッド  大きいサイズのデータを送信する
 *     バックアップしないので dataの中身を書き換えると、
 *     書き換えたものを送ってしまう可能性がある
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendHugeData(int command, const void* data, int size);
//==============================================================================
/**
 * 子機送信メソッド
 * 親機がデータを子機全員に送信するのは別関数
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   size       送信量    コマンドだけの場合0
 * @param   bFast       送信量    コマンドだけの場合0
 * @param   myID       送信する人のハンドルno
 * @param   sendID     送る相手のID   全員なら0xff
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendData(int command, const void* data, int size, BOOL bFast, int myID, int sendID);
//==============================================================================
/**
 * 親機送信メソッド  大きいサイズのデータを送信する  サイズ固定
 *     バックアップしないので dataの中身を書き換えると、
 *     書き換えたものを送ってしまう可能性がある
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendFixHugeSizeData_ServerSide(int command, const void* data);
//==============================================================================
/**
 * 親機送信メソッド  大きいサイズのデータを送信する
 *     バックアップしないので dataの中身を書き換えると、
 *     書き換えたものを送ってしまう可能性がある
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendHugeData_ServerSide(int command, const void* data, int size);
//==============================================================================
/**
 * 親機専用サーバー送信メソッド
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendData_ServerSide(int command, const void* data, int size);
//==============================================================================
/**
 * 親機専用サーバー送信メソッド サイズ固定版
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendFixSizeData_ServerSide(int command, const void* data);
//==============================================================================
/**
 * 送信バッファ残り容量
 * @retval  サイズ
 */
//==============================================================================
extern int GFL_NET_SystemGetSendRestSize(void);
//==============================================================================
/**
 * サーバ側の送信バッファ残り容量
 * @retval  サイズ
 */
//==============================================================================
extern int GFL_NET_SystemGetSendRestSize_ServerSide(void);
//==============================================================================
/**
 * 通信可能状態なのかどうかを返す ただしコマンドによるネゴシエーションがまだの状態
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsConnect(u16 netID);
//==============================================================================
/**
 * 通信可能状態の人数を返す
 * @param   none
 * @retval  接続人数
 */
//==============================================================================
extern int GFL_NET_SystemGetConnectNum(void);
//==============================================================================
/**
 * 初期化しているかどうかを返す
 * @param   none
 * @retval  初期が終わっていたらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsInitialize(void);
//==============================================================================
/**
 * サーバー側から子機に送る場合 送信キューへの追加
 * @param   command    comm_sharing.hに定義したラベル
 * @param   sendNetID
 * @param   data       送信したいデータ ない時はNULL
 *                     このデータは静的でなければならない  バッファに溜めないため
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSetSendQueue_ServerSide(int command, const void* data, int size);
//==============================================================================
/**
 * クライアント側から親機に送る場合 送信キューへの追加
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 *                     このデータは静的でなければならない  バッファに溜めないため
 * @param   byte       送信量    コマンドだけの場合0
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSetSendQueue(int command, const void* data, int size);
//==============================================================================
/**
 * DS通信MP通信の切り替え  GFL_NET_CMD_DSMP_CHANGE
 * @param   none
 * @retval  残り数
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChange(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * DS通信MP通信の切り替え
 * @param   none
 * @retval  残り数
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChangeReq(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * DS通信MP通信の切り替え 終了処理 GFL_NET_CMD_DSMP_CHANGE_END
 * @param   none
 * @retval  残り数
 */
//==============================================================================
extern void GFL_NET_SystemRecvDSMPChangeEnd(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * 自分の機のIDを返す
 * @param   
 * @retval  自分の機のID  つながっていない場合COMM_PARENT_ID
 */
//==============================================================================
extern u16 GFL_NET_SystemGetCurrentID(void);
//==============================================================================
/**
 * 汎用送信メソッド  送信サイズ固定でしかも大きい場合
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendFixHugeSizeData(int command, const void* data);
//==============================================================================
/**
 * 汎用送信メソッド  送信サイズ固定の場合
 * @param   command    comm_sharing.hに定義したラベル
 * @param   data       送信したいデータ ない時はNULL
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendFixSizeData(int command, const void* data);
//==============================================================================
/**
 * 汎用送信メソッド  コマンド以外存在しない場合
 * @param   command    comm_sharing.hに定義したラベル
 * @retval  送信キューに入ったかどうか
 */
//==============================================================================
extern BOOL GFL_NET_SystemSendFixData(int command);
//==============================================================================
/**
 * WHライブラリで　通信状態のBITを確認
 * @param   none
 * @retval  接続がわかるBIT配列
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsChildsConnecting(void);
//==============================================================================
/**
 * エラー状態かどうか
 * @param   none
 * @retval  エラーの時TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsError(void);
//==============================================================================
/**
 * 最大接続人数を得る
 * @param   none
 * @retval  最大接続人数
 */
//==============================================================================
extern int GFL_NET_SystemGetMaxEntry(int service);
//==============================================================================
/**
 * 最小接続人数を得る
 * @param   none
 * @retval  最小接続人数
 */
//==============================================================================
extern int GFL_NET_SystemGetMinEntry(int service);
//==============================================================================
/**
 * 一人通信モードを設定
 * @param   bAlone    一人通信モード
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetAloneMode(BOOL bAlone);
//==============================================================================
/**
 * 一人通信モードかどうかを取得
 * @param   none
 * @retval  一人通信モードの場合TRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemGetAloneMode(void);
//==============================================================================
/**
 * 自動終了コマンド受信
 * @param   netID  通信ID
 * @param   size   サイズ
 * @param   pData  データ
 * @param   pWork  ワーク
 * @return  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvAutoExit(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
//==============================================================================
/**
 * デバッグ用にダンプを表示する
 * @param   adr           表示したいアドレス
 * @param   length        長さ
 * @param   pInfoStr      表示したいメッセージ
 * @retval  サービス番号
 */
//==============================================================================
extern void GFL_NET_SystemDump_Debug(u8* adr, int length, char* pInfoStr);
//==============================================================================
/**
 * 特定のコマンドを送信し終えたかどうかを調べる クライアント側
 * @param   command 調べるコマンド
 * @retval  コマンドが在ったらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsSendCommand(int command, int myID);
//==============================================================================
/**
 * キューが空っぽかどうか サーバー側
 * @param   none
 * @retval  コマンドが在ったらFALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsEmptyQueue_ServerSize(void);
//==============================================================================
/**
 * キューが空っぽかどうか
 * @param   none
 * @retval  コマンドが在ったらFALSE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsEmptyQueue(void);
//==============================================================================
/**
 * wifi接続したかどうかを設定する
 * @param   none
 * @retval  コマンドが在ったらFALSE
 */
//==============================================================================
extern void GFL_NET_SystemSetWifiConnect(BOOL bConnect);
//==============================================================================
/**
 * 戦闘に入る前の敵味方の立ち位置を設定
 * @param   no   立っていた位置の番号に直したもの
 * @param   netID   通信のID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetStandNo(int no,int netID);
//==============================================================================
/**
 * 戦闘に入る前の敵味方の立ち位置を得る
 * @param   netID 通信のID
 * @retval  立っていた位置の番号に直したもの  0-3  0,2 vs 1,3
 */
//==============================================================================
extern int GFL_NET_SystemGetStandNo(int netID);
//==============================================================================
/**
 * VCHAT状態かどうか
 * @param   none
 * @retval  VCHAT状態ならTRUE
 */
//==============================================================================
extern BOOL GFL_NET_SystemIsVChat(void);
//==============================================================================
/**
 * WIFI通信を同期通信するか、非同期通信するかの切り替えを行う
 * @param   TRUE 同期 FALSE 非同期
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetWifiBothNet(BOOL bFlg);
//==============================================================================
/**
 * エラーにする場合TRUE
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemSetError(void);
//==============================================================================
/**
 * 通信を止める
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemShutdown(void);
//==============================================================================
/**
 * サーバー側の通信キューをリセットする
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemResetQueue_Server(void);
//==============================================================================
/**
 * 通信の受信を止めるフラグをセット
 * @param   bFlg  TRUEで止める  FALSEで許可
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_SystemRecvStop(BOOL bFlg);

extern GFL_NETHANDLE* GFL_NET_SystemGetHandle(int NetID);


#endif // _NET_SYSTEM_H_

