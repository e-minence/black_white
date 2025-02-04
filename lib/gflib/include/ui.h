//==============================================================================
/**
 *@file		ui.h
 *@brief	ユーザーインターフェイス用公開関数 公開テーブル
 *@author	k.ohno
 *@date		2006.11.16
 */
//==============================================================================

#pragma once

#include "heap.h"

#ifdef __cplusplus
extern "C" {
#endif



//-----------------------------------------------------------------------------
/**
 * @brief 定数宣言
 */
//-----------------------------------------------------------------------------


#define _GFI_FADE_BASESPEED  (6)  //ソフトリセットの際のフェードスピード

/**
 * @brief ソフトウエアリセット非許可用
 */
typedef enum {
    GFL_UI_SOFTRESET_WIFI = 0x01,   ///< WIFI通信時
    GFL_UI_SOFTRESET_SVLD = 0x02,   ///< セーブロード交換時
    GFL_UI_SOFTRESET_MB   = 0x04,   ///< マルチブート子機
    GFL_UI_SOFTRESET_USER = 0x08,  ///< ここからは外部定義になる
} GF_UI_SOFTRESET_e;


/**
 * @brief スリープ非許可用
 */
typedef enum {
  GFL_UI_SLEEP_SVLD = 0x01,      ///< セーブロード
  GFL_UI_SLEEP_AGBROM = 0x02,    ///< AGBアクセス
  GFL_UI_SLEEP_NET = 0x04,       ///< 通信全般
  GFL_UI_SLEEP_MIC = 0x08,       ///< MIC
  GFL_UI_SLEEP_MB  = 0x10,       ///< マルチブート子機
  GFL_UI_SLEEP_NETIRC = 0x20,    ///< 赤外線ワイヤレス
  GFL_UI_SLEEP_USER = 0x40,      ///< ここからは外部定義になる
} GF_UI_SLEEP_e;


/**
 * @brief キーカスタマイズテーブル用
 */
typedef enum {
    GFL_UI_KEY_END = 0,     ///< テーブルの終了
    GFL_UI_KEY_COPY,     ///< １から２へキーをコピーします
    GFL_UI_KEY_CHANGE,             ///< １と２を交換します
    GFL_UI_KEY_RESET          ///< １を消します
} GF_UI_KEY_CUSTOM_e;

/**
 * @brief タッチパネルテーブル用
 */
typedef enum {
    GFL_UI_TP_HIT_END = 0xff,			///< テーブル終了コード
    GFL_UI_TP_USE_CIRCLE = 0xfe,			///< 円形として使う
    GFL_UI_TP_SKIP = 0xfd,			///< スキップする
    GFL_UI_TP_HIT_NONE = -1	///< あたったテーブルなし
} GF_UI_TP_CUSTOM_e;

/**
 * @brief バッテリー用
 */
typedef enum{
  GFL_UI_BATTLEVEL_EMP,    ///< バッテリーが空っぽ
  GFL_UI_BATTLEVEL_LO2,
  GFL_UI_BATTLEVEL_LO,
  GFL_UI_BATTLEVEL_MID2,
  GFL_UI_BATTLEVEL_MID,
  GFL_UI_BATTLEVEL_HI  ///< バッテリーが十分
}GFL_UI_BATTLEVEL_e;





#define	PAD_BUTTON_DECIDE	( PAD_BUTTON_A )	///< 決定ボタン定義
#define	PAD_BUTTON_CANCEL	( PAD_BUTTON_B )	///< キャンセルボタン定義
#define PAD_BUTTON_SOFTRESET   (PAD_BUTTON_START|PAD_BUTTON_SELECT|PAD_BUTTON_L|PAD_BUTTON_R) //ソフトリセットボタン定義

//-----------------------------------------------------------------------------
/**
 *	@brief 構造体宣言
 */
//-----------------------------------------------------------------------------

/**
 * @struct GFL_UI_KEY_CUSTOM_TBL
 * @brief  キーデータ構造体  キーコンフィグパターンを指定するテーブル.
 * @note使用例
 <BR> 配列要素優先順位:0>1>2>3>4>.....です
 <BR> GFL_UI_KEY_CUSTOM_TBL key_data[] ={
 <BR>		{PAD_BUTTON_SELECT,PAD_BUTTON_START,GFL_UI_KEY_CHANGE},		//START SELECT 交換
 <BR>		{PAD_BUTTON_L,PAD_BUTTON_A,GFL_UI_KEY_COPY},		//LをA扱いに
 <BR>		{PAD_BUTTON_R,0,GFL_UI_KEY_RESET},		//LをA扱いに
 <BR>		{GFL_UI_TP_HIT_END,0,0},		// 終了データ
 <BR> };
 */
typedef struct{
    int keySource;
    int keyDist;
    u8 mode;
}GFL_UI_KEY_CUSTOM_TBL;

/**
 * @struct GFL_UI_TP_HITTBL
 * @brief  タッチパネルあたり範囲を矩形、円形で指定するテーブル.
 * @note  使用例
 <BR>  配列要素優先順位:0>1>2>3>4>.....です 
 <BR>  GFL_UI_TP_HITTBL tp_data[] ={
 <BR>		{128,191,0,64},
 <BR>		{32,94,129,200},
 <BR>		{0,191,0,255},
 <BR>       {TP_USE_CIRCLE, 100, 80, 16 },	 円形として使う。
 <BR>		{GFL_UI_TP_HIT_END,0,0,0},		 終了データ
 <BR>  };
 * 
 */
typedef union{
	struct {
		u8	top;	///< 上（もしくは特殊コード）
		u8	bottom;	///< 下
		u8	left;	///< 左
		u8	right;	///< 右
	}rect;

	struct {
		u8	code;	///< TP_USE_CIRCLE を指定
		u8	x;      ///< 中心x座標
		u8	y;      ///< 中心y座標
		u8	r;      ///< 半径
	}circle;
}GFL_UI_TP_HITTBL;

typedef struct _UI_SYS UISYS;  ///< UISYS 型宣言
typedef struct _UI_TPSYS UI_TPSYS; ///< UIタッチパネルSYS型宣言
typedef void (GFL_UI_SLEEPRELEASE_FUNC)(void* pWork);  ///< リセット復帰時に呼ばれる関数
typedef void (GFL_UI_SOFTRESET_CALLBACK)( void *work ); ///<ソフトリセットコールバック関数
typedef BOOL (GFL_UI_NOTSLEEPCHK_FUNC)(void* pWork);  ///< スリープ直前に呼ばれる関数


//==============================================================================
/**
 * @brief UI起動処理(UIには初期化がboot時に行うものしかありません)
 * @param   heapID    ヒープ確保を行うID
 * @param   isMbBoot    マルチブートの子機起動か？(中でソフリとスリープの禁止フラグを立てる
 * @return  UISYS  workハンドル
 */
//==============================================================================
extern void GFL_UI_Boot(const HEAPID heapID , const BOOL isMbBoot );

//==============================================================================
/**
 * @brief UIMain処理
 * @return  none
 */
//==============================================================================
extern void GFL_UI_Main(void);

//==============================================================================
/**
 * @brief   UI終了処理
 * @return  none
 */
//==============================================================================

extern void GFL_UI_Exit(void);

//==============================================================================
/**
 * @brief	ソフトウエアリセットの呼び出し
 * @param	resetNo   OS_ResetSystemに渡すリセット種類
 */
//==============================================================================
extern void GFL_UI_SoftReset(int resetNo);

//------------------------------------------------------------------
/**
 * @brief   スリープ状態を禁止する
 * @param   sleepTypeBit スリープ管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepDisable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   スリープ状態を許可する
 * @param   sleepTypeBit スリープ管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepEnable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   スリープ管理BITに禁止フラグが立っているか調べる
 * @param   sleepTypeBit スリープ管理BIT
 * @return  TRUE:禁止フラグが立っている
 * @return  TRUE:禁止フラグは立っていない
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_CheckSleepDisable(const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   スリープ解除時に呼ばれる関数をセットする
 * @param   pFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   スリープ直前時に呼ばれる関数をセットする
 * @param   pFunc   スリープ直前時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepGoSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);


//------------------------------------------------------------------
/**
 * @brief   スリープ直前時・スリープ解除時に呼ばれるサウンド専用関数をセットする
 * @param   pGoFunc       スリープ直前時に呼ばれる関数
 * @param   pReleaseFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepSoundSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pGoFunc, GFL_UI_SLEEPRELEASE_FUNC* pReleaseFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   現在のスリープ解除時に呼ばれるサウンド専用関数を取得する
 * @return  スリープ解除時に呼ばれる関数
 */
//------------------------------------------------------------------
extern GFL_UI_SLEEPRELEASE_FUNC* GFL_UI_SleepSoundReleaseGetFunc( void );

//------------------------------------------------------------------
/**
 * @brief   スリープ直前時に呼ばれるサウンド専用関数を取得する
 * @return  スリープ直前時に呼ばれる関数
 */
//------------------------------------------------------------------
extern GFL_UI_SLEEPRELEASE_FUNC* GFL_UI_SleepSoundGoGetFunc( void );

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット状態を禁止する
 * @param   softResetBit リセット管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetDisable(const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット状態を許可する
 * @param   softResetBit リセット管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetEnable( const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット管理BITに禁止フラグが立っているか調べる
 * @param   softResetBit リセット管理BIT
 * @return  TRUE:禁止フラグが立っている
 * @return  FALSE:禁止フラグは立っていない
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_CheckSoftResetDisable(const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット直前時に呼ばれる関数をセットする
 * @param   pFunc   ソフトウエアリセット直前時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SoftResetSetFunc(GFL_UI_SOFTRESET_CALLBACK* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   スリープ解除時に呼ばれるバッテリー切れ専用関数をセットする
 * @param   pFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_Batt10SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//------------------------------------------------------------------
/**
 * @brief   スリープ禁止にする専用関数をセットする
 * @param   pFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_NotSleepSetFunc(GFL_UI_NOTSLEEPCHK_FUNC* pFunc, void* pWork);


//----------------------------------------------------------------------------
/**
 * @brief	  ふたが開いたかどうか
 * @param	  GFL_UI_FRAMERATE_ENUM  定義のフレーム
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern BOOL GFL_UI_GetOpenTrg(void);

//---------------------------------------------------------------------------
/**
 * @brief	ふたが閉じているかどうかを返す
 * @param	ふたが閉じている=TRUE
 */
//---------------------------------------------------------------------------
extern BOOL GFL_UI_CheckCoverOff(void);

//---------------------------------------------------------------------------
/**
 * @brief	バッテリーが低レベルかどうかを返す
 * @param	DSでもDSIでもバッテリーが低レベルの場合TRUE
 */
//---------------------------------------------------------------------------
extern BOOL GFL_UI_CheckLowBatt(void);

//----------------------------------------------------------------------------
//プロトタイプ宣言 touchpanel.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief  タッチパネル初期化
 * @param   heapID    ヒープ確保を行うID
 * @return  UI_TPSYS  タッチパネルシステムワーク
 */
//==============================================================================

extern UI_TPSYS* GFL_UI_TP_Init(const HEAPID heapID);

//==============================================================================
/**
 * @brief タッチパネル読み取り処理
 * @param   none
 * @return  none
 */
//==============================================================================

extern void GFL_UI_TP_Main(void);

//==============================================================================
/**
 * @brief タッチパネル終了処理
 * @param   none
 * @return  none
 */
//==============================================================================

extern void GFL_UI_TP_Exit(void);


//==============================================================================
/**
 * @brief オートサンプリングを行っているかどうかを得る
 * @param   none
 * @retval  TRUE  オートサンプリングである
 * @retval  FALSE  していない
 */
//==============================================================================
extern int GFL_UI_TP_GetAutoSamplingFlg(void);

//==============================================================================
/**
 * @brief オートサンプリング設定
 * @param[in]    bAuto   オートサンプリングするならTRUE
 * @return  none
 */
//==============================================================================
extern void GFL_UI_TP_SetAutoSamplingFlg( const BOOL bAuto);

//------------------------------------------------------------------
/**
 * @brief 両タイプ（矩形・円形）を見ながら判定する（ベタ入力）
 * @param[in]   tbl		当たり判定テーブル（終端コードあり）
 * @return  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TP_HitCont( const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief 両タイプ（矩形・円形）を見ながら判定する（トリガ入力）
 * @param[in]   tbl		当たり判定テーブル（終端コードあり）
 * @retval  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFL_UI_TP_HitTrg( const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief  タッチパネルに触れているか
 * @param   none
 * @retval  BOOL		TRUEで触れている
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetCont( void );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているか（トリガ）
 * @param   none
 * @retval  BOOL		TRUEで触れた
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetTrg( void );

//----------------------------------------------------------------------------
/**
 *	@brief	指定した座標で、当たり判定を行います。	複数
 *	@param	tbl		当たり判定テーブル（配列）
 *	@param	x		判定ｘ座標
 *	@param	y		判定ｙ座標
 *	@retval  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
extern int GFL_UI_TP_HitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているならその座標取得（ベタ入力）
 * @param[out]   x		Ｘ座標受け取り変数アドレス
 * @param[out]   y		Ｙ座標受け取り変数アドレス
 * @retval  TRUE  触れている
 * @retval  FALSE 触れていない。引数には何もしない。
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetPointCont( u32* x, u32* y );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているならその座標取得（トリガ入力）
 * @param[out]   x		Ｘ座標受け取り変数アドレス
 * @param[out]   y		Ｙ座標受け取り変数アドレス
 * @retval  TRUE  触れている
 * @retval  FALSE 触れていない。引数には何もしない。
 */
//------------------------------------------------------------------
extern BOOL GFL_UI_TP_GetPointTrg( u32* x, u32* y );

//----------------------------------------------------------------------------
//プロトタイプ宣言 key.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief キーリピートの速度とウェイトをセット
 * @param[in]	speed	速度
 * @param[in]	wait	ウェイト
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetRepeatSpeed(const int speed, const int wait );

//==============================================================================
/**
 * @brief キーコンフィグテーブルを登録
 * @param[in]	pTbl   コントロールテーブル配列のポインタ
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetControlModeTbl(const GFL_UI_KEY_CUSTOM_TBL* pTbl );

//==============================================================================
/**
 * @brief キートリガゲット
 * @param   none
 * @return  キートリガ
 */
//==============================================================================
extern int GFL_UI_KEY_GetTrg( void );

//==============================================================================
/**
 * @brief キーコントゲット
 * @param   none
 * @return  キーコント
 */
//==============================================================================
extern int GFL_UI_KEY_GetCont(void);

//==============================================================================
/**
 * @brief キーリピートゲット
 * @param   none
 * @return  キーコント
 */
//==============================================================================
extern int GFL_UI_KEY_GetRepeat( void );

//==============================================================================
/**
 * @brief キーリピートの速度とウェイトをゲット
 * @param[out]	speed	速度
 * @param[out]	wait	ウェイト
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_GetRepeatSpeed(int* speed, int* wait );

//==============================================================================
/**
 * @brief   キーコンフィグのモードの値を設定する
 * @param[in]   mode    キーコンフィグモード
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_SetControlMode(const int mode);

//==============================================================================
/**
 * @brief キーコンフィグのモードの値を得る
 * @param   none
 * @return  コントロールモード
 */
//==============================================================================
extern int GFL_UI_KEY_GetControlMode( void );






/*-----------------------------------------------------------------------------
 *					定数宣言
 ----------------------------------------------------------------------------*/
#define UI_REPEAT_SPEED_DEF (8)   //キーリピートのデフォルト
#define UI_REPEAT_WAIT_DEF (15)   //キーリピートのデフォルト



#define TP_REQUEST_CHECK_MAX		5		// 命令を出して失敗したらエラーを返す回数

// １シンクに取得できるデータ最大数
//  1/60なら4 1/30なら8
#define		TP_ONE_SYNC_DATAMAX	(8)

// １シンクにサンプリングできる最大の数を
// サンプリングするのに必要なバッファサイズ
//  TP_ONE_SYNC_DATAMAX + 1
#define		TP_ONE_SYNC_BUFF (TP_ONE_SYNC_DATAMAX + 1)



//-------------------------------------
/// エラーの列挙
enum
{
	TP_END_BUFF = 0xffffffff,	// サンプリング用バッファが一杯
	TP_ERR = 0,					// ARM7転送以外のエラー
	TP_OK,						// Touchパネル関数のOK
	TP_FIFO_ERR,				// ARM7のFIFO命令転送エラー	
	TP_SAMP_NOT_START			// サンプリング開始されていません
};

//-------------------------------------
/// サンプリング種別列挙
enum{
	TP_SAMP_NONE,			// サンプリングしていない
	TP_BUFFERING,			// バッファ格納モード
	TP_NO_BUFF,				// 格納処理なしモード
	TP_NO_LOOP,				// バッファをループさせない
	TP_BUFFERING_JUST,		// そのままバッファリングする(離していてもバッファリング)
	TP_NO_LOOP_JUST,		// そのままバッファリングする(離していてもバッファリング)
							// ループさせない
};
// 今のバージョンではTP_BUFFERING_JUST,	TP_NO_LOOP_JUSTを使用したデータで内外判定はできない

/*-----------------------------------------------------------------------------
 *					構造体宣言
 ----------------------------------------------------------------------------*/
//-------------------------------------
/// １フレームのタッチ情報格納構造体
typedef struct
{
	u16		Size;			// このフレームの有効サンプリング数
	TPData	TPDataTbl[ TP_ONE_SYNC_DATAMAX ];	// このフレームのサンプリングデータ
} TP_ONE_DATA;


//----------------------------------------------------------------------------
/**
 *
 * @brief	サンプリング情報を管理し、今の状態を返す	
 * @param	pData：このフレームの情報(initで指定したサンプリング回数分の情報)
 * @param	type：サンプリング種別の番号
 * @param	comp_num：バッファに格納するときに、comp_num位のさがあったら格納する
 * @return	u32：サンプリング種別による変化
					type：TP_BUFFERING		サンプリングされたバッファサイズ
					type：TP_NO_LOOP		サンプリングされたバッファサイズ
											バッファが一杯になったとき TP_END_BUFF
					type：TP_NO_BUFF		TP_OK

					type：TP_SAMP_NOT_START	サンプリング開始されていません
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoSamplingMain( TP_ONE_DATA* pData, u32 type, u32 comp_num );

//----------------------------------------------------------------------------
/**
 * @brief	スリープ処理後の再開処理
 * @param   none
 * @return	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_TP_AutoSamplingReStart( void );

//----------------------------------------------------------------------------
/**
 * @brief	スリープ処理前の停止処理
 * @param   none
 * @return  none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_TP_AutoSamplingStop( void );

//----------------------------------------------------------------------------
/**
 * @brief	タッチパネルのAUTOサンプリング開始
 * @param	p_buff：サンプリングデータを入れるバッファ
 * @param	size：バッファのサイズ
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStart( TPData* p_buff, u32 size);

//----------------------------------------------------------------------------
/**
 * @brief	タッチパネルのAUTOサンプリング開始	バッファリングなし
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStartNoBuff(void);

//----------------------------------------------------------------------------
/**
 *@brief	サンプリングを終了する
 *@param	none
 *@retval	TP_OK：成功
 *@retval   TP_FIFO_ERR：転送失敗
 *@retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStop( void );



//----------------------------------------------------------------------------
//キーとタッチパネル同時使用時判定用
//----------------------------------------------------------------------------

// GSのcommon.hから移植 090116Ariizumi
// アプリケーションの終了をキーで行ったか、タッチで行ったかを保持するための定義
#define GFL_APP_END_KEY			( 0 )		// キーで終了
#define GFL_APP_END_TOUCH		( 1 )		// タッチで終了
#define GFL_APP_KTST_KEY		(GFL_APP_END_KEY)	//キーモードで動作中
#define GFL_APP_KTST_TOUCH		(GFL_APP_END_TOUCH)	//タッチモードで動作中

//タッチからキー操作への切り替わりの対象になるキー
#define GFL_PAD_BUTTON_KTST_CHG	( PAD_BUTTON_A | PAD_BUTTON_B | \
	PAD_BUTTON_X | PAD_BUTTON_Y | \
	PAD_KEY_LEFT | PAD_KEY_RIGHT | PAD_KEY_UP | PAD_KEY_DOWN )

// インターフェース
//==============================================================================
/**
 * @brief   アプリケーション終了の制御をキーとタッチのどっちでで行ったか？取得
 * @retval  BOOL		GFL_APP_END_KEY(=0) か GFL_APP_END_TOUCH(=1) (common.hなのでインクルード無し）
 */
//==============================================================================
extern int  GFL_UI_CheckTouchOrKey(void);

//------------------------------------------------------------------
/**
 * @brief	アプリケーション終了の制御をキーとタッチのどっちでで行ったか？設定
 * @param   param		GFL_APP_END_KEY(=0) か　GFL_APP_END_TOUCH(=1)
 */
//------------------------------------------------------------------
extern void GFL_UI_SetTouchOrKey(int param);


//----------------------------------------------------------------------------
/*
    フレームレートの変更の仕組み

  GFL_UI_ChangeFrameRateを呼ぶとその時点から一旦リセットして、
  そのフレームを開始フレームと認識した動作を開始します

----------------------------------------フレームの境目
  UI_Main  key_aサンプリング

   上位プログラム内{
   GFL_UI_ChangeFrameRate( 30 )   count=0

  key_aサンプリング=GetKey()
  
   }

----------------------------------------フレームの境目
  UI_Main  count++   ここはあまり1だから一回休み、キーは同じ物を返す
  
   下位プログラム内{
  key_aサンプリング=GetKey()

  }


----------------------------------------フレームの境目
  UI_Main  count++   ここはあまり０だからサンプリング  key_bサンプリング、キーは新しくなる
  
  key_bサンプリング=GetKey()

  
以降繰り返し・・・・・

  GFL_UI_ResetFrameRate関数で内部カウンタを強制的に０にする事ができる

  

 */
//----------------------------------------------------------------------------

typedef enum{
  GFL_UI_FRAMERATE_60 = 60,
  GFL_UI_FRAMERATE_30 = 30,
  GFL_UI_FRAMECOUNT_LCM = 2, //最小公倍数 丸めに使用
} GFL_UI_FRAMERATE_ENUM;


//----------------------------------------------------------------------------
/**
 * @brief	  フレームレートの変更
 * @param	  GFL_UI_FRAMERATE_ENUM  定義のフレーム
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_ChangeFrameRate( const GFL_UI_FRAMERATE_ENUM framerate );

//----------------------------------------------------------------------------
/**
 * @brief	  フレームレートの取得
 * @param	  none
 * @retval	GFL_UI_FRAMERATE_ENUM  定義のフレーム
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_GetFrameRate( void );

//----------------------------------------------------------------------------
/**
 * @brief	  フレームレートカウンタリセット
 * @param	  none
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_ResetFrameRate( void );

//----------------------------------------------------------------------------
/**
 * @brief	  フレームレートの開始
 * @param	  GFL_UI_FRAMERATE_ENUM  定義のフレーム
 * @retval	none
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_StartFrameRateMode( const GFL_UI_FRAMERATE_ENUM framerate );

//----------------------------------------------------------------------------
/**
 * @brief	  バッテリーレベルの取得
 * @param	  none
 * @retval	GFL_UI_BATTLEVEL_e
 */
//-----------------------------------------------------------------------------
extern int GFL_UI_GetBattLevel(void);


//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているか（トリガ） + GFL_UI_SetTouchOrKey(GFL_APP_END_TOUCH)つき
 * @param   none
 * @retval  BOOL		TRUEで触れた
 */
//------------------------------------------------------------------
inline static BOOL GFL_UI_TP_GetTrgAndSet( void )
{
  BOOL flg = GFL_UI_TP_GetTrg();
  if(flg){
    GFL_UI_SetTouchOrKey(GFL_APP_END_TOUCH);
  }
  return flg;
}

//------------------------------------------------------------------
/**
 * @brief キーのトリガ + GFL_UI_SetTouchOrKey(GFL_APP_END_KEY)つき
 * @param   none
 * @retval  int		キーのBIT
 */
//------------------------------------------------------------------
inline static int GFL_UI_KEY_GetTrgAndSet( void )
{
  int flg = GFL_UI_KEY_GetTrg();
  if(flg){
    GFL_UI_SetTouchOrKey(GFL_APP_END_KEY);
  }
  return flg;
}


//-----------------------------------------------------------------------------
/**
 *      UIデバックシステム
 *
 *      UI情報上書きコールバック
 */
//-----------------------------------------------------------------------------
#define	GFL_UI_DEBUGSYS_ENABLE

#ifdef GFL_UI_DEBUGSYS_ENABLE

/**
 * @brief UI 上書き　データ構造体
 */
typedef struct {
  u16 trg;
  u16 cont;
  u16 repeat;
  u16 tp_x;
  u16 tp_y;
  u8 tp_trg;
  u8 tp_cont;
} GFL_UI_DEBUG_OVERWRITE;

/**
 * @brief UI 上書き　コールバック型　
 *
 * @param GFL_UI_DEBUG_OVERWRITE* データ格納先
 * @param GFL_UI_DEBUG_OVERWRITE* データ格納先 30フレーム時の上書き情報
 * @retval  TRUE    上書き する
 * @retval  FALSE   上書き しない
 */
typedef BOOL (GFL_UI_DEBUG_OVERWRITE_FUNC)( GFL_UI_DEBUG_OVERWRITE* pOverWrite, GFL_UI_DEBUG_OVERWRITE* pOverWrite30 );

//----------------------------------------------------------------------------
/**
 * @brief	  UI 上書き　コールバック　設定
 * @param	  GFL_UI_DEBUG_OVERWRITE_FUNC*
 */
//-----------------------------------------------------------------------------
extern void GFL_UI_DEBUG_OVERWRITE_SetCallBack( GFL_UI_DEBUG_OVERWRITE_FUNC* pFunc );

#endif // GFL_UI_DEBUGSYS_ENABLE







#ifdef __cplusplus
}/* extern "C" */
#endif


