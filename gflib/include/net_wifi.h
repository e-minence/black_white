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

#ifdef __cplusplus
extern "C" {
#endif


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

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif //__NET_WIFI_H__
