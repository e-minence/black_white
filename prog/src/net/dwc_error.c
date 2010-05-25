//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		dwc_error.c
 *	@brief  DWC汎用エラーチェック検知関数
 *	@author	Toru=Nagihashi
 *	@data		2010.02.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>
#include <dwc.h>

//ネットワーク
#include "system/net_err.h"
#include "net/dwc_rap.h"

//外部公開
#include "net/dwc_error.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief  DWCでエラーをチェックしエラーの軽度によって表示を切り分ける
 *	        軽いエラーならば、その場で表示
 *	        重いエラーならば、プロセス移動時に表示
 *	        Fatalならば、その場で表示し無限ループになる
 *
 *	@param	is_heavy  TRUEならば軽度判別する　FALSEならば軽度も重度として返す
 *	@param  is_timeout  タイムアウトを検知するか
 *
 *	@return TRUEエラー発生し表示  FALSEならばエラーではない
 */
//-----------------------------------------------------------------------------
GFL_NET_DWC_ERROR_RESULT GFL_NET_DWC_ERROR_ReqErrorDisp( BOOL is_heavy, BOOL is_timeout )
{ 
  if( GFL_NET_IsInit() )
  { 
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

    if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
    { 
      switch( cp_error->errorType )
      { 
      case DWC_ETYPE_LIGHT:
        /* fallthru */
      case DWC_ETYPE_SHOW_ERROR:
        //エラーコードorメッセージを表示するだけ
        if( !is_heavy )
        { 
          NetErr_DispCallPushPop();
          GFL_NET_StateClearWifiError();
          NetErr_ErrWorkInit();
          GFL_NET_StateResetError();
          return GFL_NET_DWC_ERROR_RESULT_PRINT_MSG;
        }
        /* fallthru */

      case DWC_ETYPE_SHUTDOWN_FM:
      case DWC_ETYPE_SHUTDOWN_GHTTP:
      case DWC_ETYPE_SHUTDOWN_ND:
        //シャットダウン
        NetErr_App_ReqErrorDisp();
        return GFL_NET_DWC_ERROR_RESULT_RETURN_PROC;

      case DWC_ETYPE_DISCONNECT:
        //切断
        NetErr_App_ReqErrorDisp();
        return GFL_NET_DWC_ERROR_RESULT_RETURN_PROC;

      case DWC_ETYPE_FATAL:
        //Fatal
        NetErr_DispCallFatal();
        return GFL_NET_DWC_ERROR_RESULT_FATAL;
      }
    }
    if( (cp_error->errorUser == ERRORCODE_TIMEOUT 
        || cp_error->errorUser == ERRORCODE_DISCONNECT )
        && is_timeout )
    { 
      NetErr_DispCallPushPop();       //エラーメッセージ表示
      GFL_NET_StateClearWifiError();  //WIFIエラー詳細情報クリア
      NetErr_ErrWorkInit();           //エラーシステム情報クリア
      GFL_NET_StateResetError();      //NET内エラー情報クリア

      GFL_NET_StateWifiMatchEnd(TRUE);  //エラーをクリアしてもずっとタイムアウト中になってしまうので
                                        //解消する
      return GFL_NET_DWC_ERROR_RESULT_TIMEOUT;
    }
    else if( cp_error->errorUser == ERRORCODE_CRC
        || cp_error->errorUser == ERRORCODE_SYSTEM 
        || cp_error->errorUser == ERRORCODE_SENDQUEUE)
    {
      if( !is_heavy )
      { 
        NetErr_DispCallPushPop();
        GFL_NET_StateClearWifiError();
        NetErr_ErrWorkInit();
        GFL_NET_StateResetError();
        return GFL_NET_DWC_ERROR_RESULT_PRINT_MSG;
      }
      else
      {
        NetErr_App_ReqErrorDisp();
        return GFL_NET_DWC_ERROR_RESULT_RETURN_PROC;
      }
    }
  }

  return GFL_NET_DWC_ERROR_RESULT_NONE;
}
