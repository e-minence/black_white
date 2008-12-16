#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	net_wifi.h
 * @brief	WIFI通信ライブラリの外部公開関数
 * @author	k.ohno
 * @date    2007.2.28
 */
//=============================================================================
#ifndef __NET_WIFI_H__
#define __NET_WIFI_H__


//==============================================================================
/**
 * @brief   Wi-Fi接続初期化 初期化時に呼び出しておく必要がある
 * @param   heapID     メモリ領域
 * @param   errorFunc  エラー表示関数
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiStart( int heapID, NetErrorFunc errorFunc );

//==============================================================================
/**
 * @brief   Wi-Fi設定画面を呼び出す
 * @param   heapID  メモリ領域
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_WifiUtility( int heapID );

//==============================================================================
/**
 * ランダムマッチを行う関数
 * @target   負…自分でゲームを開催。０以上…接続しに行く相手の、フレンドリスト上の位置
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
extern int GFL_NET_DWC_StartMatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit );

//==============================================================================
/**
 * aidを返します。接続するまでは-1を返します。
 * @retval  aid。ただし接続前は-1
 */
//==============================================================================
extern int GFL_NET_DWC_GetAid(void);
#define MYDWC_NONE_AID (-1)

//==============================================================================
/**
 * ゲーム募集・参加を行う関数。
 * @target   負…自分でゲームを開催。０以上…接続しに行く相手の、フレンドリスト上の位置
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
extern int GFL_NET_DWC_StartGame( int target,int maxnum, BOOL bVCT );
//==============================================================================
/**
 * 接続を検知した時に呼ばれる関数をセットする
 * @param 	pFunc  接続時に呼ばれる関数
 * @param 	pWork  ワークエリア
 * @retval  none
 */
//==============================================================================
// 接続コールバック型
//  友達無指定の場合aidは自分のaid
//  友達指定の場合aidは相手ののaid
typedef void (*MYDWCConnectFunc) (u16 aid,void* pWork);
extern void GFL_NET_DWC_SetConnectCallback( MYDWCConnectFunc pFunc, void* pWork );
//==============================================================================
/**
 * @brief   評価状態中の接続数を返す
 * @param   none
 * @retval  数
 */
//==============================================================================
extern int GFL_NET_DWC_AnybodyEvalNum(void);


#endif //__NET_WIFI_H__
#ifdef __cplusplus
} /* extern "C" */
#endif
