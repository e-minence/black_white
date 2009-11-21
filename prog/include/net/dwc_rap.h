//==============================================================================
/**
 * dwc_rap.h
 * WiFiP2Pに関するライブラリ
 */
//==============================================================================

#pragma once


#include <dwc.h>
#include "net_wifi.h"
#include "savedata/save_control.h"

#define MYDWC_STATUS_DATA_SIZE_MAX (128)   /// WIFIフレンド情報サイズ DWCに189バイト可能とあるが、定義が無いので控えめサイズで決めうち


//==============================================================================
/**
 * ボイスチャットのコーデックを指定します。
 * コネクション確立前にコーデックを指定しておくと、
 * コネクション確立時に自動的にボイスチャットを開始します。
 * mydwc_startConnectを呼び出した後に、呼び出してください。
 * @param コーデックのタイプ
 * @retval none
 */
//==============================================================================
extern void GFL_NET_DWC_SetVchat(int codec);
// ボイスチャットなし
#define VCHAT_NONE 0
#define VCHAT_8BIT_RAW 1
#define VCHAT_G711_ULAW 2
#define VCHAT_2BIT_ADPCM 3
#define VCHAT_3BIT_ADPCM 4
#define VCHAT_4BIT_ADPCM 5

//==============================================================================
/**
 * インターネットへ接続開始
 * @param   pMyUserData  DWCUserData
 * @param   pFriendList  DWCFriendData
 * @param   heapID       wifiライブラリ用メモリのID
 * @param   friendMax    友達リストの最大
 * @retval  MYDWC_STARTCONNECT_OK … OK
 * @retval  MYDWC_STARTCONNECT_FIRST … 初めて接続する場合。（メッセージ表示の必要有
 * @retval  MYDWC_STARTCONNECT_DIFFERENTDS … 異なるＤＳで接続しようしてる場合。（要警告）
 */
//==============================================================================
extern int GFL_NET_DWC_startConnect(DWCUserData* pUserData, DWCFriendData* pFriendData);

#define MYDWC_STARTCONNECT_OK 0
#define MYDWC_STARTCONNECT_FIRST 1
#define MYDWC_STARTCONNECT_DIFFERENTDS 2

//==============================================================================
/**
 * インターネット接続中に毎フレーム呼び出される関数
 * @param   userdata 自分のログインデータへのポインタ
 * @param   list フレンドリストの先頭ポインタ
 * @retval  正…接続完了。０…接続中。負…エラー（エラーコードが返る）
 */
//==============================================================================
extern int GFL_NET_DWC_connect();

//==============================================================================
/**
 * DWCエラー番号に対応するメッセージのIDを返します。
 * @param code - エラーコード（正の値を入れてください
 * @retval メッセージタイプ
 */
//==============================================================================
extern int GFL_NET_DWC_ErrorType(int code, int type);

//==============================================================================
/**
 * エラーが起こっていないかチェックし、エラーに応じて通信処理を閉じる処理をする。
 *  世界交換、世界バトルタワーで呼び出すために多少改造 k.ohno 06.06.08
 * @retval  ０…エラーなし。負…エラー
 * （エラーコードが返る。エラーコード０の場合はERRORCODE_0）
 */
//==============================================================================
extern int mydwc_HandleError(void);


//----------------------------------------------------------------------------
/**
 *	@brief	通信クローズ処理で、切断処理に遷移するのかのフラグ
 *
 *	@param	flag	フラグ		TRUEなら切断処理に遷移する（初期値TRUE）
 *
 * *Wi-Fiクラブ４人募集画面用に作成		
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_DWC_SetClosedDisconnectFlag( BOOL flag );

//==============================================================================
/**
 * 対戦マッチング中に毎フレーム呼ばれる関数。
 * @retval  
 	STEPMATCH_CONTINUE…マッチング中
 	STEPMATCH_SUCCESS…成功
 	STEPMATCH_CANCEL…キャンセル
 	STEPMATCH_FAIL  …相手が親をやめたため、接続を中断
 	負…エラー発生
 */
//==============================================================================
extern int GFL_NET_DWC_stepmatch( int isCancel );

//==============================================================================
/**
 * 受信したときのコールバックを設定する関数
 * @param client … クライアントとしてのコールバック関数
 * @param server … サーバとしてのコールバック関数
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_DWC_setReceiver( GFL_NET_MYDWCReceiverFunc server, GFL_NET_MYDWCReceiverFunc client );

//==============================================================================
/**
 * サーバにデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int GFL_NET_DWC_sendToServer(void *data, int size);

//==============================================================================
/**
 * 親機が子機にデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int GFL_NET_DWC_sendToClient(void *data, int size);

//==============================================================================
/**
 * @brief   ボイスチャットの一時停止＋解除を行います
 * @param   bPause   一時停止=TRUE  解除=FALSE
 * @retval  void
 */
//==============================================================================
extern void GFL_NET_DWC_pausevchat(BOOL bPause);


extern BOOL GFL_NET_DWC_canSendToServer();
extern BOOL GFL_NET_DWC_canSendToClient();

//==============================================================================
/**
 * dwc_rap.cが確保した領域を開放する。
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_DWC_free();

//==============================================================================
/**
 * 通信を切断します。
 * @param sync … 0 = 自分から切断しにいく。1 = 相手が切断するのを待つ。
 * @retval 　　　 1 = 接続処理完了。0 = 切断処理中。
 */
//==============================================================================
extern int GFL_NET_DWC_disconnect( int sync );

//==============================================================================
/**
 * 通信が切断した後、この関数を呼ぶことで内部状態をログイン直後の状態にします。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
extern int GFL_NET_DWC_returnLobby();

//==============================================================================
/**
 * フェータルエラー発生時に呼ばれる関数、このコールバックの中で処理をとめて下さい。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
extern void GFL_NET_DWC_setFetalErrorCallback( void (*func)(int) );

//-----2006.04.11 k.ohno
//==============================================================================
/**
 * ログアウトする
 * @param 	none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_DWC_Logout(void);




//==============================================================================
/**
 * 自分の情報をサーバにアップします。
 * @param 	data - データへのポインタ
 * @param 	size  - データサイズ
 * @retval  成功の可否
 */
//==============================================================================
extern BOOL GFL_NET_DWC_SetMyInfo( const void *data, int size );

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  データへのポインタ。中身は書き換えないで下さい。
 */
//==============================================================================
extern u8 *GFL_NET_DWC_GetFriendInfo( int index );

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  友達の状態。DWC_GetFriendStatusDataの返り値と同じ
 */
//==============================================================================
extern u8 GFL_NET_DWC_getFriendStatus( int index );

//==============================================================================
/**
 * 現在のフレンドリストを表示します。（デバッグ用）
 * @param 	none
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_DWC_showFriendInfo();

//==============================================================================
/**
 * 音声のノイズカットレベルを調整します（外部からアクセスしたいので、こちらに）
 * @param   d … 今より閾値を下げるか、上げるか（下げるほど拾いやすくなる）
 * @retval  none
 */
//==============================================================================
extern void mydwc_changeVADLevel(int d);

//==============================================================================
/**
 * 今接続している友達のフレンドリスト上の位置を返します。
 * まだ、接続が完了していなくても友達番号を返すことがありますので、接続が
 * 完了したかの判定には使わないでください。
 * @retval  ０以上…友達番号。−１…まだ接続してない。
 */
//==============================================================================
extern int GFL_NET_DWC_GetFriendIndex(void);

//==============================================================================
/**
 * 自分のGSIDを取得する
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  ０と−１は失敗   正の値は成功
 */
//==============================================================================

//extern int mydwc_getMyGSID(void);

//----------------------------------------------------------------------------
/**
 *	@brief	セーブして良いかどうか確認するフラグ
 *	@param	TRUE セーブしてよい
 */
//-----------------------------------------------------------------------------
extern u8 GFL_NET_DWC_GetSaving(void);


extern BOOL GFL_NET_DWC_IsSendVoiceAndInc(void);

//==============================================================================
/**
 * @brief   ボイスチャット状態かどうかを返します
 * @retval  TRUE…ボイスチャット
 * @retval  FALSE…ボイスチャットではない 
 */
//==============================================================================
extern BOOL GFL_NET_DWC_IsVChat(void);

//==============================================================================
/**
 * @brief     接続BITMAPを返す
 * @return    BITMAP
 */
//==============================================================================
extern u32 mydwc_GetBitmap(void);

//==============================================================================
/**
 * 接続してきたかどうかを返します    k.ohno 06.05.24
 * @retval  フレンドインデックス  無効=-1
 */
//==============================================================================
extern int GFL_NET_DWC_IsNewPlayer(void);
extern void GFL_NET_DWC_ResetNewPlayer(void);

//==============================================================================
/**
 * ボイスチャットのONOFF設定    k.ohno 06.05.24
 * @retval  ON = TRUE
 */
//==============================================================================
extern void GFL_NET_DWC_SetVChat(BOOL bVChat);
//==============================================================================
/**
 * 遅延機能ののONOFFができる    k.ohno 06.05.30
 * @retval  ON = TRUE
 */
//==============================================================================
extern BOOL mydwc_toggleDelay(void);

//==============================================================================
/**
 * ボイスチャットを通信中にオンオフする関数    yoshihara 06.6.15
 * こちらは、通信確立後に呼び出して下さい。
 */
//==============================================================================
extern void mydwc_VChatPause();
extern void mydwc_VChatRestart();

//----------------------------------------------------------------------------
/**
 *	@brief	VCHATデータ送信設定
 *
 *	@param	flag	TRUE:音声データを送信する	FALSE:音声データを送信しない
 */
//-----------------------------------------------------------------------------
extern void mydwc_VChatSetSend( BOOL flag );
extern BOOL mydwc_VChatGetSend( void );

//==============================================================================
/**
 * @brief   クライアント接続を一時的に中止します
            あくまでコールバックが呼ばれるまではブロックされません
 * @retval  成功か失敗
 */
//==============================================================================
extern BOOL GFL_NET_DWC_SetClientBlock(void);

//==============================================================================
/**
 * @brief   クライアント接続を許可に戻します
            あくまでコールバックが呼ばれるまではブロック解除されません
 * @retval  成功か失敗
 */
//==============================================================================
extern BOOL GFL_NET_DWC_ResetClientBlock(void);
//==============================================================================
/**
 * @brief   MYDWCが初期化済みかどうか
 * @param   void
 * @retval  TRUE  初期化済み
 */
//==============================================================================

extern BOOL GFL_NET_DWC_IsInit(void);

//==============================================================================
/**
 * @brief   接続のコールバック内で判定する関数セット
 * @param 	pFunc  接続時に呼ばれる関数
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_DWC_SetConnectModeCheckCallback( GFL_NET_MYDWCConnectModeCheckFunc pFunc );
//==============================================================================
/**
 * 他の相手に送信を行う関数
 * @param   data - 送信するデータへのポインタ
 * @param   size - 送信するデータのサイズ
 * @param   callback - 送信完了時のコールバック
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int GFL_NET_DWC_SendToOther(void *data, int size );

//----------------------------------------------------------------------------
/**
 *	@brief	セーブして良いかどうか確認するフラグを消す
 */
//-----------------------------------------------------------------------------
extern void GFL_NET_DWC_ResetSaving(void);
//----------------------------------------------------------------------------
/**
 *	@brief	キャンセル処理を行って良いかどうか
 *	@param	FALSE キャンセルしてよい TRUEキャンセル禁止
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_NET_DWC_CancelDisable(void);
//----------------------------------------------------------------------------
/**
 *	@brief	LOGIN状態かどうか
 *	@param	TRUE LOGIN状態である
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_NET_DWC_IsLogin(void);


//--------------------------------------------------------------
/**
 * @brief   通常データの分割セーブ初期化
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  none
 */
//--------------------------------------------------------------
extern void GFL_NET_DWC_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl);
//--------------------------------------------------------------
/**
 * @brief   通常データの分割セーブメイン処理
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GFL_NET_DWC_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl);

//==============================================================================
/**
 * ランダム対戦を行う関数。
 * キーはあらかじめ指定 フィルターだけわたす フィルターは製品とデバッグの判別が必ず必要
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
extern int GFL_NET_DWC_StartMatchFilter( char* filterStr,int numEntry,DWCEvalPlayerCallback evalcallback,void* pWork);


extern void GFL_NET_DWC_MainStep( void );
extern BOOL GFL_NET_DWC_SetCancelState(void);

extern int GFL_NET_DWC_GetStepMatchResult(void);


//#include <ppwlobby/ppw_lobby.h>

