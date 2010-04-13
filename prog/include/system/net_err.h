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
	NET_ERR_STATUS_ERROR,			///<エラー発生
	NET_ERR_STATUS_REQ,				///<エラー画面呼び出しリクエスト発生中
}NET_ERR_STATUS;

///エラーチェック状況
typedef enum
{
  NET_ERR_CHECK_NONE,   //エラーは発生していない
  NET_ERR_CHECK_HEAVY,  //重度のエラーが発生    切断orシャットダウンor電源切断が必要
  NET_ERR_CHECK_LIGHT,  //軽度のエラーが発生    復帰可能 wifiしかでません
} NET_ERR_CHECK;



//--------------------------------------------------------------
//  使用ヒープサイズ
//--------------------------------------------------------------
///キャラクタVRAM退避サイズ
#define NETERR_PUSH_CHARVRAM_SIZE		(0x4000)
///スクリーンVRAM退避サイズ
#define NETERR_PUSH_SCRNVRAM_SIZE		(0x800)
///パレットVRAM退避サイズ
#define NETERR_PUSH_PLTTVRAM_SIZE		(0x20)


//==============================================================================
//	外部関数宣言
//==============================================================================

//--------------------------------------------------------------
//	システム側で使用する関数
//--------------------------------------------------------------
extern void NetErr_SystemInit(void);
extern void NetErr_SystemCreate(int heap_id);
extern void NetErr_SystemExit(void);
extern void NetErr_Main(void);
extern void NetErr_ErrorSet(void);
extern void NetErr_ErrWorkInit(void);
extern BOOL NetErr_DispCall(BOOL fatal_error);
extern void NetErr_DispCallPushPop(void);
extern void NetErr_DispCallFatal(void);
extern void NetErr_ExitNetSystem( void );

extern void NetErr_GetTempArea( u8** charArea , u16** scrnArea , u16** plttArea );


//--------------------------------------------------------------
//	アプリ側で使用する関数
//--------------------------------------------------------------
extern NET_ERR_CHECK NetErr_App_CheckError(void);
extern NET_ERR_CHECK NetErr_App_CheckConnectError(u32 net_bit);
extern void NetErr_App_ReqErrorDisp(void);
extern BOOL NetErr_App_FatalDispCall(void);

//--------------------------------------------------------------
//  デバッグ用
//--------------------------------------------------------------
#if PM_DEBUG
extern void NetErr_DEBUG_ErrorSet(void);
#endif  //PM_DEBUG

#endif	//__NET_ERR_H__
