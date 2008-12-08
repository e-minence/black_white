//=============================================================================
/**
 * @file	net_ds.h
 * @brief	DSデバイスとのアクセス関数
 * @author	GAME FREAK Inc.
 * @date    2006.12.5
 */
//=============================================================================


#ifndef __NET_WIRELESS_H__
#define __NET_WIRELESS_H__

#include "wih.h"

/// 接続や非接続時に呼ばれるコールバック定義
typedef void (*GFL_NET_ConnectionCallBack) (int aid);

typedef struct _WM_INFO_STRUCT  GFL_NETWM;



// 関数切り出し自動生成 funccut.rb  k.ohno 2006.12.5 
//==============================================================================
/**
 * @brief   接続クラスのワーク確保
 * @param   heapID   ワーク確保ID
 * @retval  _COMM_WORKのポインタ
 */
//==============================================================================
extern void* GFL_NET_WLGetHandle(HEAPID heapID, GameServiceID serviceNo, u8 num);
//==============================================================================
/**
 * @brief   接続クラスの初期化
 * @param   heapID   ワーク確保ID
 * @retval  _COMM_WORKのポインタ
 */
//==============================================================================
extern void GFL_NET_WLInitialize(HEAPID heapID,NetBeaconGetFunc getFunc,NetBeaconGetSizeFunc getSize, NetBeaconCompFunc getComp);

//==============================================================================
/**
 * @brief   受信コールバック関数を指定
 * @param   recvCallback   受信コールバック関数
 * @return  none
 */
//==============================================================================
extern void GFL_NET_WLSetRecvCallback( PTRCommRecvLocalFunc recvCallback);
//==============================================================================
/**
 * @brief   接続しているかどうか
 * @retval  TRUE  接続している
 */
//==============================================================================
extern BOOL GFL_NET_WLIsConnect(void);
//==============================================================================
/**
 * @brief   子機の使用しているデータの初期化
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLChildBconDataInit(void);
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
extern BOOL GFL_NET_WLParentInit(BOOL bTGIDChange, BOOL bEntry, GFL_NET_ConnectionCallBack pConnectFunc );
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
extern BOOL GFL_NET_WLChildInit(BOOL bBconInit);

//==============================================================================
/**
 * @brief   通信切り替えを行う（親子反転に必要な処理）
 * @param   none
 * @retval  リセットしたらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLSwitchParentChild(void);
//==============================================================================
/**
 * @brief   通信切断を行う  ここではあくまで通信終了手続きに入るだけ
 *  ホントに消すのは下の_commEnd
 * @param   none
 * @retval  終了処理に移った場合TRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLFinalize(void);
//==============================================================================
/**
 * @brief   通信切断を行う  ただしメモリー開放を行わない
 * @param   切断の場合TRUE
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLStealth(BOOL bStalth);
//==============================================================================
/**
 * @brief   探すことができた親の数を返す
 * @param   none
 * @retval  親機の数
 */
//==============================================================================
extern int GFL_NET_WLGetParentCount(void);
//--------------------------------------------------------------
/**
 * @brief   BmpListの位置からBconの存在するIndex位置を取得する
 * @param   index		BmpList位置
 * @retval  Index
 */
//--------------------------------------------------------------
extern int CommBmpListPosBconIndexGet(int index);
//==============================================================================
/**
 * @brief   親機リストに変化があった場合TRUE
 * @param   none
 * @retval  親機リストに変化があった場合TRUE なければFALSE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsScanListChange(void);
//==============================================================================
/**
 * @brief   親機の変化を知らせるフラグをリセットする
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLResetScanChangeFlag(void);
//==============================================================================
/**
 * @brief   この親機がいくつとコネクションをもっているのかを得る
 * @param   index   親のリストのindex
 * @retval  コネクション数 0-16
 */
//==============================================================================
extern int GFL_NET_WLGetParentConnectionNum(int index);
//==============================================================================
/**
 * @brief   すぐに接続していい人が見つかった場合indexを返す
 * @param   none
 * @retval  該当したらindexを返す
 */
//==============================================================================
extern int GFL_NET_WLGetFastConnectIndex(void);
//==============================================================================
/**
 * @brief   次のレベルで繋いでいい人がいたらそのindexを返します
 * @param   none
 * @retval  該当したらindexを返す
 */
//==============================================================================
extern int GFL_NET_WLGetNextConnectIndex(void);
//==============================================================================
/**
 * @brief   子機　MP状態で接続
 * @param   index   親のリストのindex
 * @retval  子機接続を親機に送ったらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLChildIndexConnect(u16 index, _PARENTFIND_CALLBACK pCallback, GFL_NETHANDLE* pNetHandle);
extern BOOL GFL_NET_WLChildMacAddressConnect(u8* macAddress, _PARENTFIND_CALLBACK pCallback);
extern void GFI_NET_BeaconSetScanCallback(_PARENTFIND_CALLBACK pCallback);

//==============================================================================
/**
 * @brief   ビーコンデータの定期確認
 *  接続が完了する間での間、この関数を呼び、タイムアウト処理を行う
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLParentBconCheck(void);
//==============================================================================
/**
 * @brief   通信ライブラリー内部の状態を見て、処理をする関数
 * VBlankとは関係ないのでprocessの時に処理すればいい
 * 子機はお互いの接続がわからないので、通信結果をcommsystemからもらってエラー検査する
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void CommMpProcess(u16 bitmap);
//==============================================================================
/**
 * @brief   デバイスが通信可能状態なのかどうかを返す
 * @param   親子機のnetID
 * @retval  TRUE  通信可能    FALSE 通信切断
 */
//==============================================================================
extern BOOL GFL_NET_WL_IsConnectLowDevice(u16 netID);
//==============================================================================
/**
 * @brief  通信切断モードにはいったかどうか
 * @param   none
 * @retval  接続人数
 */
//==============================================================================
extern BOOL GFL_NET_WLIsConnectStalth(void);
//==============================================================================
/**
 * @brief  初期化しているかどうかを返す
 * @param   none
 * @retval  初期が終わっていたらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsInitialize(void);
//==============================================================================
/**
 * @brief  WHライブラリで　状態がIDLEになっているか確認する
 * @param   none
 * @retval  IDLEになっている=TRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsStateIdle(void);
//==============================================================================
/**
 * @brief  WHライブラリで　通信状態のBITを確認  子機がつながっているかどうか
 * @param   none
 * @retval  つながったらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsChildsConnecting(void);
//==============================================================================
/**
 * @brief  親機が落ちたかどうか
 * @param   none
 * @retval  落ちた場合TRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLParentDisconnect(void);
//==============================================================================
/**
 * @brief  エラー状態かどうか
 * @param   none
 * @retval  エラーの時TRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsError(void);
//==============================================================================
/**
 * @brief  子機がいないのをエラー扱いにするかどうかをSET
 * @param   bOn  有効時にTRUE
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLSetNoChildError(BOOL bOn);
//==============================================================================
/**
 * @brief 誰かが落ちたのをエラー扱いにするかどうかをSET
 * @param   bOn  有効時にTRUE
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLSetDisconnectOtherError(BOOL bOn);
//==============================================================================
/**
 * @brief   サービス番号に対応したビーコン間隔を得ます
 *          サービス番号は include/communication/comm_def.hにあります
 * @param   serviceNo サービス番号
 * @retval  beacon間隔 msec
 */
//==============================================================================
extern u16 _getServiceBeaconPeriod(u16 serviceNo);
//==============================================================================
/**
 * @brief   ビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   WMBssDesc*  ビーコンバッファポインタ
 */
//==============================================================================
extern WMBssDesc* GFL_NET_WLGetWMBssDesc(int index);
//==============================================================================
/**
 * @brief    ビーコン強度を得る
 * @param    index ビーコンバッファに対するindex
 * @retval   u16
 */
//==============================================================================
extern u16 GFL_NET_WL_GetRssi(int index);
//==============================================================================
/**
 * @brief   GFビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
 */
//==============================================================================
extern void* GFL_NET_WLGetGFBss(int index);
//==============================================================================
/**
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
 * @retval   none
 */
//==============================================================================
extern void GFL_NET_WLResetWMBssDesc(int index);
//==============================================================================
/**
 * @brief    ビーコンデータを消す
 * @param    index ビーコンバッファに対するindex
 * @retval   none
 */
//==============================================================================
extern void GFL_NET_WLResetGFBss(int index);
//==============================================================================
/**
 * @brief   User定義のビーコンデータを得る
 * @param   index ビーコンバッファに対するindex
 * @retval   GF_BSS_DATA_INFO*  ビーコンバッファポインタ
 */
//==============================================================================

extern void* GFL_NET_WLGetUserBss(int index);

//==============================================================================
/**
 * @brief   ビーコンの中にあるマックアドレスを得る
 * @param   index ビーコンバッファに対するindex
 * @return  maccaddressの配列
 */
//==============================================================================

extern void* GFL_NET_WLGetUserMacAddress(int index);

//==============================================================================
/**
 * @brief   macアドレスをバックアップする
 * @param   pMac   mac address
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLSetBackupMacAddress(u8* pMac, int netID);
//==============================================================================
/**
 * @brief   自動切断モードに入ったかどうかを返す
 * @param   none
 * @retval  入っているならばTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WLIsAutoExit(void);
//==============================================================================
/**
 * @brief   自動切断モードON
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLSetAutoExit(void);
//==============================================================================
/**
 * @brief   ビーコンデータに現在の状況を反映させる
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLFlashMyBss(void);
//==============================================================================
/**
 * @brief   ライフタイムを小さくする または元に戻す
 * @param   bMinimum TRUEなら小さくする
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WLSetLifeTime(BOOL bMinimum);
//------------------------------------------------------
/**
 * @brief   サービス番号のビーコン数を返します
 * @param   serviceNo   comm_def.hにあるサービス番号
 * @retval  利用者数
 */
//------------------------------------------------------
extern int GFL_NET_WLGetServiceNumber(int serviceNo);
//------------------------------------------------------
/**
 * @brief   親機が一回でもビーコンを送信し終わったかどうか
 * @retval  送信完了=TRUE
 */
//------------------------------------------------------
extern BOOL GFL_NET_WLIsParentBeaconSent(void);
//------------------------------------------------------
/**
 * @brief   スキャン状態かどうか
 * @retval  送信完了=TRUE
 */
//------------------------------------------------------
extern BOOL GFL_NET_WLIsChildStateScan(void);
//------------------------------------------------------
/**
 * @brief   チャンネルを返す
 * @retval  接続チャンネル
 */
//------------------------------------------------------
extern int GFL_NET_WLGetChannel(void);
//==============================================================================
/**
 * @brief	自機ビーコン送信内容の取得
 * @param none
 * @return ビーコンの中身
 */
//==============================================================================
extern void* GFL_NET_WLGetMyGFBss(void);
//------------------------------------------------------
/**
 * @brief	ビーコンを拾ってからのカウントダウンタイマ取得
 * @param	index	位置
 */
//------------------------------------------------------
extern int GFL_NET_WLGetBConUncacheTime(int index);
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
extern BOOL GFL_NET_WL_SendData(void* data,int size,PTRSendDataCallback callback);
//------------------------------------------------------
/**
 * @brief	接続状態をBITMAPで返す
 * @return  接続状態
 */
//------------------------------------------------------
extern u16 GFL_NET_WL_GetBitmap(void);
//------------------------------------------------------
/**
 * @brief	自分の接続IDを返す
 * @return  接続ID
 */
//------------------------------------------------------
extern u16 GFL_NET_WL_GetCurrentAid(void);
//------------------------------------------------------
/**
 * @brief	WHのワークポインタを返す
 * @return  GFL_NETWMのポインタ
 */
//------------------------------------------------------
extern GFL_NETWM* _GFL_NET_WLGetNETWH(void);

//==============================================================================
/**
 *  @brief   ユーザ定義の親機情報を設定します。
 *  _GF_BSS_DATA_INFO構造体の中身を送ります
 *  @param   userGameInfo  ユーザ定義の親機情報へのポインタ
 *  @param   length        ユーザ定義の親機情報のサイズ
 *  @retval  none
 */
//==============================================================================
extern void GFI_NET_BeaconSetInfo( void );


//==============================================================================
/**
 *  @brief   親機になる
 *  @param   channelChange  親機のチャンネルを変更する場合TRUE 基本は変更で
 *  @retval  うまくいったらTRUE
 */
//==============================================================================
extern BOOL GFL_NET_WL_ParentConnect(BOOL channelChange);
extern void GFL_NET_WHPipeFree(void);


#endif  //__NET_WIRELESS_H__

