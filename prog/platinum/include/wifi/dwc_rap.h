#ifndef __DWC_RAP_H__
#define  __DWC_RAP_H__

#include <dwc.h>

#include "savedata/savedata.h"
#include "savedata/wifilist.h"

// 受信コールバック型。WHReceiverFuncと同形
typedef void (*MYDWCReceiverFunc) (u16 aid, u16 *data, u16 size);

// 切断コールバック型
typedef void (*MYDWCDisconnectFunc) (u16 aid,void* pWork);
// 接続コールバック型
//  友達無指定の場合aidは自分のaid
//  友達指定の場合aidは相手ののaid
typedef void (*MYDWCConnectFunc) (u16 aid,void* pWork);


#define ERRORCODE_0 (-1)
#define ERRORCODE_HEAP (-2)

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
extern void mydwc_setVchat(int codec);
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
 * @param   pSaveData      セーブデータへのポインタ
 * @param   heapID         WIFI用HEAPID
 * @param   heapSize       確保サイズ
 * @param   maxConnectNum  最大接続人数  
 * @retval  MYDWC_STARTCONNECT_OK … OK
 * @retval  MYDWC_STARTCONNECT_FIRST … 初めて接続する場合。（メッセージ表示の必要有
 * @retval  MYDWC_STARTCONNECT_DIFFERENTDS … 異なるＤＳで接続しようしてる場合。（要警告）
 */
//==============================================================================
extern int mydwc_startConnect(SAVEDATA* pSaveData, int heapid, int heapSize ,int maxConnectNum);
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
extern int mydwc_connect();

//==============================================================================
/**
 * DWCエラー番号に対応するメッセージのIDを返します。
 * @param code - エラーコード（正の値を入れてください
 * @retval メッセージタイプ
 */
//==============================================================================
extern int mydwc_errorType(int code);

//==============================================================================
/**
 * エラーが起こっていないかチェックし、エラーに応じて通信処理を閉じる処理をする。
 *  世界交換、世界バトルタワーで呼び出すために多少改造 k.ohno 06.06.08
 * @retval  ０…エラーなし。負…エラー
 * （エラーコードが返る。エラーコード０の場合はERRORCODE_0）
 */
//==============================================================================
extern int mydwc_HandleError(void);

//==============================================================================
/**
 * ランダムマッチを行う関数。
 * @target   負…自分でゲームを開催。０以上…接続しに行く相手の、フレンドリスト上の位置
 * @param	timelimit	時間制限（秒）この制限時間たったら、2人つながってたら通信を開始する
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
extern int mydwc_startmatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit );

//==============================================================================
/**
 * ゲーム募集・参加を行う関数。
 * @target   負…自分でゲームを開催。０以上…接続しに行く相手の、フレンドリスト上の位置
 * @retval  正…成功。０…失敗。
 */
//==============================================================================
int mydwc_startgame( int target, int maxno );

#define STEPMATCH_CONTINUE 0
#define STEPMATCH_SUCCESS  (DWC_ERROR_NUM)
#define STEPMATCH_CANCEL (STEPMATCH_SUCCESS+1)
// 2006.7.4 yoshihara追加
#define STEPMATCH_FAIL (STEPMATCH_SUCCESS+2)
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
extern int mydwc_stepmatch( int isCancel );

//==============================================================================
/**
 * 受信したときのコールバックを設定する関数
 * @param client … クライアントとしてのコールバック関数
 * @param server … サーバとしてのコールバック関数
 * @retval  none
 */
//==============================================================================
extern void mydwc_setReceiver( MYDWCReceiverFunc server, MYDWCReceiverFunc client );

//==============================================================================
/**
 * サーバにデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int mydwc_sendToServer(void *data, int size);

//==============================================================================
/**
 * 親機が子機にデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int mydwc_sendToClient(void *data, int size);

//==============================================================================
/**
 * 相互にデータ送信を行う関数
 * @param   data - 送信するデータへのポインタ。size - 送信するデータのサイズ
 * @retval  1 - 成功　 0 - 失敗（送信バッファが詰まっている等）
 */
//==============================================================================
extern int mydwc_sendToOther(void *data, int size);


//==============================================================================
/**
 * DWC通信処理更新を行う
 * @param none
 * @retval 0…正常, 負…エラー発生 1…タイムアウト 2…相手から切断された
 */
//==============================================================================
//extern int mydwc_step();

//==============================================================================
/**
 * DWCボイスチャット起動を行う
 * @param  heapID
 * @retval none
 */
//==============================================================================
extern void mydwc_startvchat(int heapID);
//==============================================================================
/**
 * @brief   ボイスチャットを停止します
 * @param   void
 * @retval  void
 */
//==============================================================================
extern void mydwc_stopvchat(void);

//==============================================================================
/**
 * @brief   ボイスチャットの一時停止＋解除を行います
 * @param   bPause   一時停止=TRUE  解除=FALSE
 * @retval  void
 */
//==============================================================================
extern void mydwc_pausevchat(BOOL bPause);

//==============================================================================
/**
 * aidを返します。接続するまでは-1を返します。
 * @retval  aid。ただし接続前は-1
 */
//==============================================================================
extern int mydwc_getaid();
#define MYDWC_NONE_AID (-1)
extern BOOL mydwc_canSendToServer();
extern BOOL mydwc_canSendToClient();

//==============================================================================
/**
 * dwc_rap.cが確保した領域を開放する。
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void mydwc_free();

//==============================================================================
/**
 * 通信を切断します。
 * @param sync … 0 = 自分から切断しにいく。1 = 相手が切断するのを待つ。
 * @retval 　　　 1 = 接続処理完了。0 = 切断処理中。
 */
//==============================================================================
extern int mydwc_disconnect( int sync );

//==============================================================================
/**
 * 通信が切断した後、この関数を呼ぶことで内部状態をログイン直後の状態にします。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
extern int mydwc_returnLobby();

//==============================================================================
/**
 * フェータルエラー発生時に呼ばれる関数、このコールバックの中で処理をとめて下さい。
 * @param 	nonte
 * @retval  1 = 成功。0 = 失敗。
 */
//==============================================================================
extern void mydwc_setFetalErrorCallback( void (*func)(int) );

//-----2006.04.11 k.ohno
//==============================================================================
/**
 * ログアウトする
 * @param 	none
 * @retval  none
 */
//==============================================================================
extern void mydwc_Logout(void);



//==============================================================================
/**
 * オンラインの友達がサーバにアップしている情報をおさめる配列を指定します。
 * この配列はmydwc_step, mydwc_stepmatchを呼び出した際に更新される可能性があります。
 * @param 	array - データをおさめる配列の先頭
 * @param 	size  - 一人あたりのデータサイズ
 * @retval  none
 */
//==============================================================================
extern void mydwc_setFriendStateBuffer( void *array, int size );

//==============================================================================
/**
 * 自分の情報をサーバにアップします。
 * @param 	data - データへのポインタ
 * @param 	size  - データサイズ
 * @retval  成功の可否
 */
//==============================================================================
extern BOOL mydwc_setMyInfo( const void *data, int size );

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  データへのポインタ。中身は書き換えないで下さい。
 */
//==============================================================================
extern void *mydwc_getFriendInfo( int index );

//==============================================================================
/**
 * 友達がサーバにアップしている情報を取得します（ローカルにキャッシュしてあるものを表示します）。
 * @param 	index フレンドリスト上の番号
 * @retval  友達の状態。DWC_GetFriendStatusDataの返り値と同じ
 */
//==============================================================================
extern u8 mydwc_getFriendStatus( int index );

extern int mydwc_init(int heapID);
extern void mydwc_createUserData( WIFI_LIST *pWifiList );
extern void mydwc_callUtility( int heapID );

//==============================================================================
/**
 * 現在のフレンドリストを表示します。（デバッグ用）
 * @param 	none
 * @retval  none
 */
//==============================================================================
extern void mydwc_showFriendInfo();

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
extern int mydwc_getFriendIndex();

//==============================================================================
/**
 * 自分のGSIDを取得する
 * @param   WIFI_LIST  
 * @retval  ０と−１は失敗   正の値は成功
 */
//==============================================================================

extern int mydwc_getMyGSID(WIFI_LIST *pWifiList);

//==============================================================================
/**
 * 自分のGSIDが正しいのか検査する
 * @param   SAVEDATA*  
 * @retval  TRUEが成功
 */
//==============================================================================

extern BOOL mydwc_checkMyGSID(SAVEDATA *pSV);



extern BOOL mydwc_IsSendVoiceAndInc(void);

//==============================================================================
/**
 * ボイスチャット状態かどうかを返します   k.ohno 06.05.23
 * @retval  TRUE…ボイスチャット   FALSE…ボイスチャットではない 
 */
//==============================================================================
extern BOOL mydwc_IsVChat(void);


//==============================================================================
/**
 * 接続してきたかどうかを返します    k.ohno 06.05.24
 * @retval  フレンドインデックス  無効=-1
 */
//==============================================================================
extern int mydwc_IsNewPlayer(void);
extern void mydwc_ResetNewPlayer(void);

//==============================================================================
/**
 * ボイスチャットのONOFF設定    k.ohno 06.05.24
 * @retval  ON = TRUE
 */
//==============================================================================
extern void mydwc_setVChat(BOOL bVChat);
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
 * 切断を検知した時に呼ばれる関数をセットする
 * @param 	pFunc  切断時に呼ばれる関数
 * @param 	pWork  ワークエリア
 * @retval  none
 */
//==============================================================================
extern void mydwc_setDisconnectCallback( MYDWCDisconnectFunc pFunc, void* pWork );

//==============================================================================
/**
 * 接続を検知した時に呼ばれる関数をセットする
 * @param 	pFunc  接続時に呼ばれる関数
 * @param 	pWork  ワークエリア
 * @retval  none
 */
//==============================================================================
extern void mydwc_setConnectCallback( MYDWCConnectFunc pFunc, void* pWork );

//==============================================================================
/**
 * @brief   クライアント接続を一時的に中止します
 * @param   none
 * @retval  none
 */
//==============================================================================
extern int mydwc_SetClientBlock(void);

//==============================================================================
/**
 * @brief   クライアント接続を許可に戻します
 * @param   none
 * @retval  none
 */
//==============================================================================
extern void mydwc_ResetClientBlock(void);
//==============================================================================
/**
 * @brief   評価状態中の接続数を返す
 * @param   none
 * @retval  数
 */
//==============================================================================
extern int mydwc_AnybodyEvalNum(void);


#include "dwc_overlay.h"

#endif
