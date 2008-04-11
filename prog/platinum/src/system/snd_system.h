//==============================================================================================
/**
 * @file	snd_system.h
 * @brief	サウンドシステム
 * @author	Satoshi Nohara
 * @date	2005.11.08
 *
 * ●snd_tool,snd_play,(main)からのみアクセスされる
 * snd_system
 *     |
 *     |
 * ●データセットなどの処理をまとめた	●再生、停止などの処理をまとめた
 * snd_tool								snd_play
 *     |
 *     |
 * ●各種デモから呼ばれる(主にフィールドイベント)
 * fld_bgm
 */
//==============================================================================================
#ifndef __SND_SYSTEM_H__
#define __SND_SYSTEM_H__


#include "savedata/savedata_def.h"
#include "savedata/perapvoice.h"
#include "savedata/config.h"


//#define STREAM_ON	//ストリーム処理を有効にする


//==============================================================================================
//
//	サウンドシステムワークへの不完全型ポインタ
//
//==============================================================================================
typedef	struct _SND_WORK SND_WORK;


//==============================================================================================
//
//	(snd_system,snd_tool,snd_play)のみで使用する定義
//
//==============================================================================================
#define WAVE_ARC_GROUP_MAX		(4)							//波形アーカイブグループ最大数
#define SE_HANDLE_MAX			(4)							//SEのサウンドハンドル最大数

#define CAPTURE_THREAD_PRIO		(3)							//キャプチャスレッドの優先度
#define CAPTURE_BUFSIZE			(0x1000)					//キャプチャバッファサイズ
#define CAPTURE_FORMAT		(NNS_SND_CAPTURE_FORMAT_PCM16)	//キャプチャのフォーマット

#define STREAM_THREAD_PRIO		(10)						//

#define SWAVE_BUFFER_SIZE		(2000)						//2k * 8bit

//階層レベル取得定義
enum{
	SND_HEAP_SAVE_START = 0,						//全て消す時

	//常駐ロード

	SND_HEAP_SAVE_GLOBAL,							//常駐以外を消す時

	//フィールドのBGMバンクロード
	
	SND_HEAP_SAVE_BGM_BANK,							//常駐、フィールドBGMバンク以外を消す時

	//SEロード
	
	SND_HEAP_SAVE_SE,								//常駐、フィールドBGMバンク、SE以外を消す時

	//BGMロード
	
	SND_HEAP_SAVE_BGM,								//BGMの上に積んだものを消す時

	//------------------------------------------------------------------------------------------
	
	//図鑑鳴き声データをロードする時に、サブ画面の上に積む
	//再度ロードする時に開放、ロードするために必要(06.03.24)
	SND_HEAP_SAVE_SUB_SE,							//サブ画面SEの上に積んだものを消す時

	SND_HEAP_SAVE_ME,								//ME追加、削除用

	SND_HEAP_SAVE_MAX,								//最大数
};

//SND_WORKのメンバーにアクセスする定義
enum{
	SND_W_ID_WAVEOUT_HANDLE_NORMAL = 0,		//波形ハンドル
	SND_W_ID_WAVEOUT_HANDLE_CHORUS,			//波形ハンドル
	SND_W_ID_BANK_INFO,						//バンク情報構造体
	SND_W_ID_CAPTURE_BUF,					//キャプチャバッファ
	SND_W_ID_CALLBACK_INFO,					//エフェクトのコールバック変数

	SND_W_ID_CTRL_BGM_FLAG,					//フィールドBGM固定フラグ

	SND_W_ID_FADE_COUNT,					//フェードカウンター
	SND_W_ID_NEXT_WAIT,						//次のBGMを鳴らすまでのウェイト
	SND_W_ID_NEXT_FRAME,					//次のBGMのフェードインフレーム数

	SND_W_ID_NOW_BGM_NO,					//今のBGMナンバー(セーブする必要あり！あとで移動する)
	SND_W_ID_NEXT_BGM_NO,					//次のBGMナンバー(セーブする必要あり！あとで移動する)

	SND_W_ID_FIELD_PAUSE_FLAG,				//PLAYER_FIELDをポーズしているかフラグ
	SND_W_ID_BGM_PAUSE_FLAG,				//PLAYER_BGMをポーズしているかフラグ
	SND_W_ID_ME_WAIT,						//MEウェイト

	SND_W_ID_REVERSE_FLAG,					//逆再生使用フラグ
	SND_W_ID_WAVEOUT_CH_NORMAL_FLAG,		//波形で使用するCHを確保したかフラグ
	SND_W_ID_WAVEOUT_CH_CHORUS_FLAG,		//波形で使用するCHを確保したかフラグ(CHORUS)
	SND_W_ID_CHORUS_FLAG,					//コーラス使用フラグ

	SND_W_ID_BANK_FLAG,						//バンク切り替えるかフラグ
	SND_W_ID_FILTER_SIZE,					//フィルターサイズ
	SND_W_ID_SCENE_MAIN,					//現在のメインシーン
	SND_W_ID_SCENE_SUB,						//現在のサブシーン

	SND_W_ID_HEAP_SAVE_START,				//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_GLOBAL,				//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_BGM_BANK,			//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_SE,					//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_BGM,					//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_SUB_SE,				//サウンドヒープ階層レベル
	SND_W_ID_HEAP_SAVE_ME,					//サウンドヒープ階層レベル

	SND_W_ID_PERAP_PLAY_FLAG,				//ペラップの録音した鳴き声を再生しているかフラグ
	SND_W_ID_PERAP_DEFAULT_FLAG,			//ペラップのデフォルトの鳴き声を再生するフラグ
	SND_W_ID_ZONE_BGM,						//ゾーンのBGMナンバー

	SND_W_ID_WAVE_DATA,						//波形データのポインタ

	SND_W_ID_REVERSE_BUF,					//逆再生用のバッファのポインタ

	SND_W_ID_LENGTH_TCB,					//鳴き声再生時間TCB

	SND_W_ID_MY_PERAP_PTR,					//自分のぺラップボイスのポインタ

	SND_W_ID_PERAP_PTR1,					//ぺラップボイスのポインタ
	SND_W_ID_PERAP_PTR2,					//ぺラップボイスのポインタ
	SND_W_ID_PERAP_PTR3,					//ぺラップボイスのポインタ
	SND_W_ID_PERAP_PTR4,					//ぺラップボイスのポインタ

	//SND_PV_070213
	SND_W_ID_PV_PTN,						//パターン(snd_tool.h参照)
	SND_W_ID_PV_PAN,						//-128〜127
	SND_W_ID_PV_VOL,						//ボリューム(0-127)
	SND_W_ID_PV_HEAP_ID,					//ヒープID
	SND_W_ID_PV_NO,							//ポケモンナンバー
	SND_W_ID_PV_WAIT,						//再生するまでのウェイト
	SND_W_ID_PV_DOUBLE_FLAG,				//鳴き声を2つ再生出来るようにするフラグ

	SND_W_ID_MAX,							//最大数
};


//==============================================================================================
//
//	extern宣言
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	サウンド初期化
 *
 * @param	none
 *
 * @retval	none
 *
 * 他の全てのサウンド関数より先に呼びだしてください。
 * ただし、あらかじめ、OSを初期化しておく必要があります。 
 *
 * IRQ割り込みは有効にしてください。また、この関数の後で、
 * OS_SetIrqMask関数 などで割り込みの設定を上書きしないようにしてください。 
 * ●
 */
//--------------------------------------------------------------
extern void Snd_AllInit( PERAPVOICE* perap, CONFIG* config );

//--------------------------------------------------------------
/**
 * @brief	サウンドフレームワーク
 *
 * @param	none
 *
 * @retval	none
 *
 * 1フレームに1度呼び出しさえすれば、どこでコールしても構わない
 */
//--------------------------------------------------------------
extern void Snd_Main(void);

//--------------------------------------------------------------
/**
 * @brief	サウンドステータスセット
 *
 * @param	status		ステータス(snd_system.h参照)
 *
 * @retval	none
 * ●
 */
//--------------------------------------------------------------
extern void Snd_StatusSet( int status );

//--------------------------------------------------------------
/**
 * @brief	サウンドステータスチェック
 *
 * @param	chg_status	ステータス(snd_system.h参照)
 *
 * @retval	"TRUE=許可、FALSE=不可"
 */
//--------------------------------------------------------------
extern BOOL Snd_StatusCheck( int chg_status );


//==============================================================================================
//
//	サウンドシステムワークアクセス関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	サウンドシステムワークのアドレスを取得
 *
 * @param	none
 *
 * @retval	"サウンドシステムワークのアドレス"
 */
//--------------------------------------------------------------
extern SND_WORK* Snd_GetSystemAdrs();

//--------------------------------------------------------------
/**
 * @brief	サウンドシステムワークのメンバのアドレスを取得
 *
 * @param	type	メンバの種類
 *
 * @retval	"メンバのアドレス"
 */
//--------------------------------------------------------------
extern void* Snd_GetParamAdrs( int type );


//==============================================================================================
//
//	ヒープメモリ関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	メモリの状態を保存
 *
 * @param	heap_lv		保存後の状態の階層レベルをとっておくワークへのポインタ
 *						(NULLだと何もしない)
 *
 * @retval	"保存後の階層レベル、失敗すると-1"
 *
 * ヒープ作成直後の階層レベルは0で、この関数を呼ぶ毎に、階層レベルが１つ増えます。 
 * NNS_SndHeapLoadState関数を呼びだすことで、
 * 指定した階層レベルの保存直後の状態に復元させることができます。 
 *
 * 状態の保存には、ヒープを少し消費します。ヒープの空き容量が少ない場合は、関数に失敗します。 
 */
//--------------------------------------------------------------
extern int Snd_HeapSaveState(int* heap_lv);

//--------------------------------------------------------------
/**
 * @brief	メモリの状態を復元
 *
 * @param	level	復元する階層レベル
 *
 * @retval	none
 *
 * NNS_SndHeapSaveState関数を呼んだ直後の状態に戻すことが出来る
 * すなわち、NNS_SndHeapSaveState関数を呼んだ後にロードしたデータが削除される
 * この時、ロード済みサウンドデータを使って再生されている音は止まりません
 */
//--------------------------------------------------------------
extern void Snd_HeapLoadState( int level );

//--------------------------------------------------------------
/**
 * @brief	追加したサウンドヒープの容量と、空き容量を出力する
 *
 * @param	none
 *
 * @retval	none
 *
 * プレイヤーヒープを確保すると、その分容量が減っているので注意！
 */
//--------------------------------------------------------------
//extern void Snd_UseHeapSizeOsPrint(void);

//--------------------------------------------------------------
/**
 * @brief	サウンドヒープのアドレスを取得
 *
 * @param	none
 *
 * @retval	"サウンドヒープのアドレス"
 */
//--------------------------------------------------------------
extern NNSSndHeapHandle* Snd_HeapHandleGet(void);


//==============================================================================================
//
//	データロード関連
//
//	snd_tool.c
//	FILED,BATTLEなどのデータセット関数から主に呼ばれる
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	グループロード
 *
 * @param	no		グループナンバー
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadGroup( u16 no );

//--------------------------------------------------------------
/**
 * @brief	シーケンスデータ、バンクデータ、波形データを同時にロード
 *
 * @param	no		シーケンスナンバー
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadSeq( u16 no );

//--------------------------------------------------------------
/**
 * @brief	シーケンスデータ、バンクデータ、波形データを(選んで)ロード
 *
 * @param	no		シーケンスナンバー
 * @param	flag	どのデータをロードするか
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 *
 * ＜ロード定義＞
 * シーケンスデータ	NNS_SND_ARC_LOAD_SEQ 
 * バンクデータ		NNS_SND_ARC_LOAD_BANK
 * 波形アーカイブ	NNS_SND_ARC_LOAD_WAVE
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadSeqEx( u16 no, u32 flag );

//--------------------------------------------------------------
/**
 * @brief	シーケンスアーカイブをロード
 *
 * @param	no		シーケンスアーカイブナンバー
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 *
 * シーケンスアーカイブは、複数のバンクと関連があるため、
 * 自動的にバンクデータや波形データはロードされない
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadSeqArc( u16 no );

//--------------------------------------------------------------
/**
 * @brief	波形アーカイブをロード
 *
 * @param	no		波形アーカイブナンバー
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadWaveArc( u16 no );

//--------------------------------------------------------------
/**
 * @brief	バンクデータをロード(必要な波形データもロードされる)
 *
 * @param	no		バンクナンバー
 *
 * @retval	"成功=TRUE、失敗=FALSE"
 */
//--------------------------------------------------------------
extern BOOL Snd_ArcLoadBank( u16 no );


//==============================================================================================
//
//	サウンドハンドル、波形ハンドル関連
//
//	snd_play.c
//	BGM,ME,SE,PMVOICEの再生関数から主に呼ばれる
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	サウンドハンドルのアドレスを取得
 *
 * @param	type	サウンドハンドルタイプ
 *
 * @retval	"サウンドハンドルのアドレス"
 */
//--------------------------------------------------------------
extern NNSSndHandle * Snd_HandleGet(int type);

//--------------------------------------------------------------
/**
 * @brief	BGMのサウンドハンドルのアドレスを取得
 *
 * @param	scene	シーン(場面)ナンバー
 *
 * @retval	"BGMのサウンドハンドルのアドレス"
 */
//--------------------------------------------------------------
//extern NNSSndHandle * Snd_BgmHandleGetByScene( u8 scene );

//--------------------------------------------------------------
/**
 * @brief	プレイヤーナンバーから、ハンドルナンバー取得(ハンドルのアドレスではないので注意！)
 *
 * @param	player_no	プレイヤーナンバー
 *
 * @retval	"ハンドルナンバー"
 */
//--------------------------------------------------------------
extern int Snd_GetHandleNoByPlayerNo( int player_no );


//==============================================================================================
//
//	シーケンス関連
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	全ての再生中のシーケンスを一時停止または再開
 *
 * @param	flag	TRUE=一時停止、FALSE=再開
 *
 * @retval	none
 *
 * 全ての再生中のシーケンスではなく、サウンドハンドル指定、プレイヤー番号指定も出来るが保留
 */
//--------------------------------------------------------------
extern void Snd_PlayerPauseAll( BOOL flag );


#endif


