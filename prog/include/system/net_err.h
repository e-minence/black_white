//==============================================================================
/**
 * @file	net_err.h
 * @brief	通信エラー画面制御のヘッダ
 * @author	matsuda
 * @date	2008.11.17(月)
 */
//==============================================================================
#ifndef __NET_ERR_H__
#define __NET_ERR_H__


//==============================================================================
//	定数定義
//==============================================================================
///エラー画面システムの状況
typedef enum{
	NET_ERR_STATUS_NULL,			///<何もなし
	NET_ERR_STATUS_REQ,				///<エラー画面呼び出しリクエスト発生中
	NET_ERR_STATUS_DISP,			///<エラー画面表示中
}NET_ERR_STATUS;


//==============================================================================
//	外部関数宣言
//==============================================================================
extern void NetErr_SystemInit(void);
extern void NetErr_SystemCreate(int heap_id);
extern void NetErr_SystemExit(void);
extern NET_ERR_STATUS NetErr_Main(void);
extern void NetErr_ErrorSet(void);


#endif	//__NET_ERR_H__
