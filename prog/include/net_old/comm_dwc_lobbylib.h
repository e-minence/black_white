//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		dwc_lobbylib.h
 *	@brief		ロビーサーバ管理関数
 *	@author		tomoya takahashi
 *	@data		2007.12.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __DWC_LOBBYLIB_H__
#define __DWC_LOBBYLIB_H__

#include <ppwlobby/ppw_lobby.h>

#include "savedata/save_control.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	チャンネルステータス
//=====================================
typedef enum {
    OLDDWC_LOBBY_CHANNEL_STATE_NONE,           // チャンネルに入っていない。
    OLDDWC_LOBBY_CHANNEL_STATE_LOGINWAIT,		// チャンネルに入室中。
    OLDDWC_LOBBY_CHANNEL_STATE_CONNECT,		// チャンネルに入室済み。
    OLDDWC_LOBBY_CHANNEL_STATE_LOGOUTWAIT,     // チャンネルに退室中。
    OLDDWC_LOBBY_CHANNEL_STATE_ERROR           // チャンネル状態を取得できませんでした。
} OLDDWC_LOBBY_CHANNEL_STATE;



//-------------------------------------
///	部屋情報取得定数
//=====================================
typedef enum {
	OLDDWC_LOBBY_ROOMDATA_LOCKTIME,		// チャンネルをロックする時間
	OLDDWC_LOBBY_ROOMDATA_RANDOM,			// サーバから生成されるランダム値
	OLDDWC_LOBBY_ROOMDATA_ROOMTYPE,		// 部屋のタイプ
	OLDDWC_LOBBY_ROOMDATA_SEASON,			// 季節番号
	OLDDWC_LOBBY_ROOMDATA_ARCEUS,			// アルセウスの表示を許可するか 
	OLDDWC_LOBBY_ROOMDATA_CLOSETIME,		// 部屋のロックからクローズまでの時間
	OLDDWC_LOBBY_ROOMDATA_FIRESTART,		// 部屋のロックから花火開始までの時間
	OLDDWC_LOBBY_ROOMDATA_FIREEND,			// 部屋のロックから花火終了までの時間
	OLDDWC_LOBBY_ROOMDATA_MAX,				// システム内で使用します
} OLDDWC_LOBBY_ROOMDATA_TYPE;	

//-------------------------------------
///	サブチャンネルタイプ
//=====================================
typedef enum {
	OLDDWC_LOBBY_SUBCHAN_FOOT1,			// 足跡ボード１
	OLDDWC_LOBBY_SUBCHAN_FOOT2,			// 足跡ボード２
	OLDDWC_LOBBY_SUBCHAN_CLOCK,			// 世界時計
	OLDDWC_LOBBY_SUBCHAN_NEWS,				// ロビーニュース
	OLDDWC_LOBBY_SUBCHAN_NONE,				// システム内で使用します
} OLDDWC_LOBBY_SUBCHAN_TYPE;

//-------------------------------------
///	サブチャンネルログイン待ち
//=====================================
typedef enum {
	OLDDWC_LOBBY_SUBCHAN_LOGIN_WAIT,			// サブチャンネルにログイン待機中
	OLDDWC_LOBBY_SUBCHAN_LOGIN_OK,				// サブチャンネルにログイン完了
	OLDDWC_LOBBY_SUBCHAN_LOGIN_NG,				// サブチャンネルにログイン失敗
} OLDDWC_LOBBY_SUBCHAN_LOGIN_RESULT;

//-------------------------------------
///	ミニゲームタイプ
//=====================================
typedef enum {
	OLDDWC_LOBBY_MG_BALLSLOW,			// 玉いれ
	OLDDWC_LOBBY_MG_BALANCEBALL,		// 玉乗り
	OLDDWC_LOBBY_MG_BALLOON,			// 風船わり
	OLDDWC_LOBBY_MG_NUM,				// システム内で使用します
} OLDDWC_LOBBY_MG_TYPE;


//-------------------------------------
///	ミニゲームエントリー戻り値
//=====================================
typedef enum {
	OLDDWC_LOBBY_MG_ENTRYNONE,			// 何もしていない
	OLDDWC_LOBBY_MG_ENTRYWAIT,			// エントリー中
	OLDDWC_LOBBY_MG_ENTRYOK,			// エントリー完了
	OLDDWC_LOBBY_MG_ENTRYNG,			// エントリー失敗
} OLDDWC_LOBBY_MG_ENTRYRET;

//-------------------------------------
///	投票状態
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_STATE_NONE,	// 送信前	
	OLDDWC_LOBBY_ANKETO_STATE_SENDING,	// 送信中
	OLDDWC_LOBBY_ANKETO_STATE_SENDOK,	// 送信成功
	OLDDWC_LOBBY_ANKETO_STATE_SENDNG,	// 送信失敗
} OLDDWC_LOBBY_ANKETO_STATE;


//-------------------------------------
///	アンケートデータ
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SERIAL,	// 今の質問通し番号
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NO,		// 今の質問番号
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_SPECIAL,	// 今の質問がスペシャルか
	OLDDWC_LOBBY_ANKETO_DATA_NOW_QUESTION_NINI,	// 今の質問が任意質問か
	
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_SERIAL,	// 前回の質問通し番号
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_NO,		// 前回の質問番号
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_SPECIAL,// 前回の質問がスペシャルか
	OLDDWC_LOBBY_ANKETO_DATA_LAST_QUESTION_NINI,	// 前回の質問が任意質問か

	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_A,	// 前回の質問が結果A
	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_B,	// 前回の質問が結果B
	OLDDWC_LOBBY_ANKETO_DATA_LAST_RESULT_C,	// 前回の質問が結果C

	OLDDWC_LOBBY_ANKETO_DATA_MAX,	// DATA最大数
} OLDDWC_LOBBY_ANKETO_DATA;

//-------------------------------------
///	アンケートメッセージ情報
//	任意質問
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_QUESTION,	// 今の問題
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_00,	// 今の回答A
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_01,	// 今の回答B
	OLDDWC_LOBBY_ANKETO_MESSAGE_NOW_ANSWER_02,	// 今の回答C

	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_QUESTION,		// 前回の問題
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_00,	// 前回の回答A
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_01,	// 前回の回答B
	OLDDWC_LOBBY_ANKETO_MESSAGE_LAST_ANSWER_02,	// 前回の回答C

	OLDDWC_LOBBY_ANKETO_MESSAGE_MAX,	// MESSAGE最大数
} OLDDWC_LOBBY_ANKETO_MESSAGE;

//-------------------------------------
///	集計言語取得用
//=====================================
typedef enum {
	OLDDWC_LOBBY_ANKETO_LANGUAGE_NOW,
	OLDDWC_LOBBY_ANKETO_LANGUAGE_LAST,
	OLDDWC_LOBBY_ANKETO_LANGUAGE_MAX,
} OLDDWC_LOBBY_ANKETO_LANGUAGE_DATA;



//-------------------------------------
///	退室者世界地図データ
//=====================================
#define DWC_WLDDATA_MAX		(20)	// 最大格納地域数


//-------------------------------------
///	ユーザ列挙
//=====================================
#define OLDDWC_LOBBY_USERIDTBL_IDX_NONE	( 0xffffffff )		// ユーザIDからテーブルインデックス取得失敗戻り値


//-------------------------------------
///	ミニゲームタイムリミット
//=====================================
#ifdef PM_DEBUG
#define OLDDWC_LOBBY_MG_TIMELIMIT	20    //(60)
#else //PM_DEBUG
#define OLDDWC_LOBBY_MG_TIMELIMIT	(60)
#endif  //PM_DEBUG

//-------------------------------------
///	VIP情報
//=====================================
#define OLDDWC_LOBBY_VIP_BUFFNUM	( 128 )		// バッファ数
#define OLDDWC_LOBBY_VIP_KEYNONE	( 0 )		// 合言葉がないときのキー値


//-------------------------------------
/// アンケート　定数
//=====================================
#define OLDDWC_LOBBY_ANKETO_NOT_QUESTION	( PPW_LOBBY_INVALID_QUESTION_NO )	// 無効な質問



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/**
 *	@brief		プレイヤー入室コールバック関数型
 *
 *	@param		userid		チャンネルのユーザID
 *	@param		cp_profile	そのユーザのプロフィール
 *	@param		p_work		作業ワーク
 *	@param		mydata		自分のデータ
 */
//=====================================
typedef void (*pDWC_LOBBY_UserInCallBack)( s32 userid, const void* cp_profile, void* p_work, BOOL mydata );

//-------------------------------------
/**
 *	@brief		プレイヤー退室コールバック関数型
 *
 *	@param		userid		チャンネルのユーザID
 *	@param		p_work		作業ワーク
 */
//=====================================
typedef void (*pDWC_LOBBY_UserOutCallBack)( s32 userid, void* p_work );

//-------------------------------------
/**
 *	@brief		プロフィール更新コールバック関数型
 *
 *	@param		userid		チャンネルのユーザID
 *	@param		cp_profile	そのユーザのプロフィール
 *	@param		p_work		作業ワーク
 */
//=====================================
typedef void (*pDWC_LOBBY_UserProfileUpDateCallBack)( s32 userid, const void* cp_profile, void* p_work );

//-------------------------------------
/**
 *	@brief		時間イベント発生コールバック関数型
 *
 *	@param		event		イベント種類
 *	@param		p_work		作業ワーク
 */
//=====================================
typedef void (*pDWC_LOBBY_EventCallBack)( PPW_LOBBY_TIME_EVENT event, void* p_work );

//-------------------------------------
/**
 *	@brief		入室時不正チェック後プロフィール受信コールバック関数型
 *
 *	@param		cp_profile	チェック後のプロフィール
 *	@param		p_work		作業ワーク
 */
//=====================================
typedef void (*pDWC_LOBBY_CheckProfileCallBack)( const void* cp_profile, u32 profile_size, void* p_work );

//-------------------------------------
///	Wi-Fiロビーコールバックセット構造体
//=====================================
typedef struct {
	pDWC_LOBBY_UserInCallBack				p_user_in;			// プレイヤー入室
	pDWC_LOBBY_UserOutCallBack				p_user_out;			// プレイヤー退室
	pDWC_LOBBY_UserProfileUpDateCallBack	p_profile_update;	// プレイヤープロフィール更新
	pDWC_LOBBY_EventCallBack				p_event;			// イベント発生
	pDWC_LOBBY_CheckProfileCallBack			p_check_profile;	// 入室時不正チェック後プロフィール受信
} OLDDWC_LOBBY_CALLBACK;





//-------------------------------------
/**
 *	@brief		メッセージ受信コールバック関数型
 *
 *	@param		userid		チャンネルのユーザID
 *	@param		cp_data		受信データ
 *	@param		size		サイズ
 *	@param		p_work		ユーザ作業用ワーク
 */
//=====================================
typedef void (*pDWC_LOBBY_MsgDataRecvCallBack)( s32 userid, const void* cp_data, u32 size, void* p_work );
//-------------------------------------
///	メッセージデータコマンドデータ
//=====================================
typedef struct {
	pDWC_LOBBY_MsgDataRecvCallBack		p_func;			// 受信コールバック関数
	u32									size;			// データサイズ
} OLDDWC_LOBBY_MSGCOMMAND;






//-------------------------------------
///	チャンネル内ユーザID列挙
//=====================================
typedef struct {
	u32			num;
	const s32*	cp_tbl;
} OLDDWC_LOBBY_CHANNEL_USERID;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
// メインチャンネル
extern void OLDDWC_LOBBY_Init( u32 heapID, SAVE_CONTROL_WORK* p_save, u32 profilesize, const OLDDWC_LOBBY_CALLBACK* cp_callback,  void* p_callbackwork );	// ワークの初期化
extern void OLDDWC_LOBBY_Exit( void );												// ワークの破棄
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_Update( void );						// ロビーの更新関数
extern BOOL OLDDWC_LOBBY_UpdateErrorCheck( void );
extern PPW_LOBBY_ERROR OLDDWC_LOBBY_GetErr( void );								// ロビーエラー状態取得
extern s32 OLDDWC_LOBBY_GetErrNo( PPW_LOBBY_ERROR error );								// ロビーエラー状態取得
extern BOOL OLDDWC_LOBBY_Login( const void* cp_loginprofile );						// メインチャンネルへログイン
extern BOOL OLDDWC_LOBBY_LoginEx( const void* cp_loginprofile, u32 prefix );			// メインチャンネルへログイン	入室チャンネルを選択
extern BOOL OLDDWC_LOBBY_LoginWait( void );										// メインチャンネルログイン完了待ち
extern void OLDDWC_LOBBY_Logout( void );										// メインチャンネルログアウト
extern BOOL OLDDWC_LOBBY_LogoutWait( void );										// メインチャンネルログアウト完了待ち
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_GetState( void );						// メインチャンネルの状態
extern BOOL OLDDWC_LOBBY_CheckExcessFlood( void );	// 通信過剰状態取得	取得後フラグをOFFにします
extern BOOL OLDDWC_LOBBY_CheckProfileError( void );							// プロフィール情報不正チェック

// プロフィールデータ
extern s32 OLDDWC_LOBBY_GetMyUserID( void );							// 自分のユーザID
extern void OLDDWC_LOBBY_SetMyProfile( const void* cp_data );			// 自分のプロフィールを更新
extern const void* OLDDWC_LOBBY_GetMyProfile( void );					// 自分のプロフィールを取得
extern const void* OLDDWC_LOBBY_GetUserProfile( s32 userid );			// 人のプロフィールを取得
extern void OLDDWC_LOBBY_GetLoginTime( s32 userid, s64* p_time );		// ログインした時間を取得
extern s32	OLDDWC_LOBBY_GetOldUser( void );							// 一番昔からいるプレイヤーを取得
extern s32	OLDDWC_LOBBY_GetOldUser_UserDesc( s32 desc_user );			// 切断される人以外で一番昔からいるプレイヤーを取得

// サブチャンネルROOM
extern BOOL OLDDWC_LOBBY_SUBCHAN_Login( OLDDWC_LOBBY_SUBCHAN_TYPE channel );			// サブチャンネルへログイン
extern OLDDWC_LOBBY_SUBCHAN_LOGIN_RESULT OLDDWC_LOBBY_SUBCHAN_LoginWait( void );								// サブチャンネルへログイン待ち
extern BOOL OLDDWC_LOBBY_SUBCHAN_Logout( void );									// サブチャンネルログアウト
extern BOOL OLDDWC_LOBBY_SUBCHAN_LogoutWait( void );								// サブチャンネルログアウト待ち
extern OLDDWC_LOBBY_CHANNEL_STATE OLDDWC_LOBBY_SUBCHAN_GetState( void );				// サブチャンネルの状態
extern BOOL OLDDWC_LOBBY_SUBCHAN_LoginCheck( OLDDWC_LOBBY_SUBCHAN_TYPE channel );						// サブチャンネルの人数をチェックして入れる状態かチェック

// ユーザID列挙取得
// チャンネル内に誰がいるのかわかる
extern void OLDDWC_LOBBY_GetUserIDTbl( OLDDWC_LOBBY_CHANNEL_USERID* p_data );			// メインチャンネル内ユーザID列挙取得
extern void OLDDWC_LOBBY_SUBCHAN_GetUserIDTbl( OLDDWC_LOBBY_CHANNEL_USERID* p_data );	// サブチャンネル内ユーザID列挙取得
extern u32	OLDDWC_LOBBY_GetUserIDIdx( s32 userid );								// メインチャンネル内のユーザIDから列挙内のINDEXを求める
extern u32	OLDDWC_LOBBY_SUBCHAN_GetUserIDIdx( s32 userid );						// サブチャンネル内のユーザIDから列挙内のINDEXを求める
extern s32	OLDDWC_LOBBY_GetUserIdxID( u32 idx );								// メインチャンネル内のINDEXからユーザIDを求める
extern s32	OLDDWC_LOBBY_SUBCHAN_GetUserIdxID( u32 idx );						// サブチャンネル内のINDEXからユーザIDを求める

// ロビーデータ
extern void OLDDWC_LOBBY_GetTime( s64* p_time );		// 現在のGMT
extern void OLDDWC_LOBBY_GetOpenTime( s64* p_time );	// 部屋を作成した時間
extern void OLDDWC_LOBBY_GetLockTime( s64* p_time );	// 部屋をロックした時間
extern u32 OLDDWC_LOBBY_GetRoomData( OLDDWC_LOBBY_ROOMDATA_TYPE type );	// 各情報を取得


// 退室者の国データ
extern BOOL OLDDWC_LOBBY_WLDDATA_Send( void );
extern BOOL OLDDWC_LOBBY_WLDDATA_SendWait( void );
extern u16 OLDDWC_LOBBY_WLDDATA_GetNation( u8 index );		// 退室者の国ID
extern u8 OLDDWC_LOBBY_WLDDATA_GetArea( u8 index );		// 退室者の地域ID
extern BOOL OLDDWC_LOBBY_WLDDATA_GetDataFlag( u8 index );	// 退室者国データがあるか


// メッセージ送受信
// LOBBY_何もなし		メインチャンネル用
// LOBBY_SUBCHAN		サブチャンネル用
//						（足跡ボード、世界時計、ニュースのなかで今自分がいるチャンネル）
extern void OLDDWC_LOBBY_SetMsgCmd( const OLDDWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );			// コマンドの設定
extern void OLDDWC_LOBBY_SUBCHAN_SetMsgCmd( const OLDDWC_LOBBY_MSGCOMMAND* cp_tbl, u32 tblnum, void* p_work );
extern void OLDDWC_LOBBY_CleanMsgCmd( void );															// コマンドテーブルを破棄
extern void OLDDWC_LOBBY_SUBCHAN_CleanMsgCmd( void );
extern void OLDDWC_LOBBY_SendChannelMsg( u32 type, const void* cp_data, u32 size );					// チャンネル全体に送信
extern void OLDDWC_LOBBY_SendPlayerMsg( u32 type, s32 userid, const void* cp_data, u32 size );						// 個人に対して送信
extern void OLDDWC_LOBBY_SUBCHAN_SendMsg( u32 type, const void* cp_data, u32 size );					// サブチャンネルはチャンネル全体の送信しかない

// ミニゲーム募集
// この関数のアクセスはCOMM_STATEが行います。
// P2P関係の情報の取得はCOMM_〜関数で行ってください。
extern BOOL OLDDWC_LOBBY_MG_StartRecruit( OLDDWC_LOBBY_MG_TYPE type, u32 maxnum );	// 募集開始
extern void OLDDWC_LOBBY_MG_EndRecruit( void );					// 募集終了
extern void OLDDWC_LOBBY_MG_StartGame( void );						// 今の状態でゲームを開始させる
extern BOOL OLDDWC_LOBBY_MG_CheckStartGame( void );				// 参加しているゲームを開始するかチェック
extern void OLDDWC_LOBBY_MG_EndConnect( void );					// P2P通信の終了
extern BOOL OLDDWC_LOBBY_MG_ForceEnd( void );						// 募集を終了する必要があるか
extern BOOL OLDDWC_LOBBY_MG_CheckRecruit( OLDDWC_LOBBY_MG_TYPE type );// 募集情報の取得
extern BOOL OLDDWC_LOBBY_MG_CheckEntryOk( OLDDWC_LOBBY_MG_TYPE type );// エントリーできるかチェック
extern u32 OLDDWC_LOBBY_MG_GetRest( OLDDWC_LOBBY_MG_TYPE type );		// あと何人はいれるか
extern u32 OLDDWC_LOBBY_MG_GetEntryNum( OLDDWC_LOBBY_MG_TYPE type );	// 何人募集に参加しているか
extern BOOL OLDDWC_LOBBY_MG_Entry( OLDDWC_LOBBY_MG_TYPE type );		// 募集に参加
extern s32 OLDDWC_LOBBY_MG_GetTimeLimit( OLDDWC_LOBBY_MG_TYPE type );	// 募集の制限時間を取得する
extern OLDDWC_LOBBY_MG_ENTRYRET OLDDWC_LOBBY_MG_EntryWait( void );	// 募集参加完了待ち
extern OLDDWC_LOBBY_MG_TYPE OLDDWC_LOBBY_MG_GetConnectType( void );	// 接続中のミニゲームを取得
extern s32 OLDDWC_LOBBY_MG_GetParentUserID( void );				// 親のuseridを取得
extern BOOL OLDDWC_LOBBY_MG_MyParent( void );						// 自分が親かチェック
extern BOOL OLDDWC_LOBBY_MG_CheckConnect( void );					// 誰かとP2P状態にあるかチェック

// VIPデータ取得
extern BOOL OLDDWC_LOBBY_VIP_CheckVip( s32 userid );
extern s32 OLDDWC_LOBBY_VIP_GetAikotobaKey( s32 userid );


// アンケートデータ
extern void OLDDWC_LOBBY_ANKETO_SubMit( s32 select );					// 投票送信
extern OLDDWC_LOBBY_ANKETO_STATE OLDDWC_LOBBY_ANKETO_WaitSubMit( void );	// 投票完了待ち
extern s32 OLDDWC_LOBBY_ANKETO_GetData( OLDDWC_LOBBY_ANKETO_DATA type );	// アンケートデータ取得
extern u16* OLDDWC_LOBBY_ANKETO_GetMessage( OLDDWC_LOBBY_ANKETO_MESSAGE type );	// アンケートメッセージ取得
extern BOOL OLDDWC_LOBBY_ANKETO_GetLanguageSummarize( OLDDWC_LOBBY_ANKETO_LANGUAGE_DATA type, u32 lang );


#ifdef PM_DEBUG
// デバック用スケジュール設定
extern void OLDDWC_LOBBY_DEBUG_SetRoomData( u32 locktime, u32 random, u8 roomtype, u8 season );
extern void OLDDWC_LOBBY_DEBUG_PlayerIN( const void* cp_data, s32 userid );			// その人をいることにする
extern void OLDDWC_LOBBY_DEBUG_SetProfile( const void* cp_data, u32 userid );			// その人をいることにする

#endif

#endif		// __DWC_LOBBYLIB_H__

