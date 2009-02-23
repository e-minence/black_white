//==============================================================================
/**
 *@file		ui.h
 *@brief	ユーザーインターフェイス用公開関数 公開テーブル
 *@author	k.ohno
 *@date		2006.11.16
 */
//==============================================================================

#ifndef __UI_H__
#define	__UI_H__

#include <heap.h>

#ifdef __cplusplus
extern "C" {
#endif

// デバッグ用決まり文句----------------------
#define GFL_UI_DEBUG   (0)   ///< ユーザーインターフェイスデバッグ用 0:無効 1:有効

#if defined(DEBUG_ONLY_FOR_ohno)
#undef GFL_UI_DEBUG
#define GFL_UI_DEBUG   (1)
#endif  //DEBUG_ONLY_FOR_ohno

#ifndef GFL_UI_PRINT
#if GFL_UI_DEBUG
#define GFL_UI_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#else   //GFL_UI_DEBUG
#define GFL_UI_PRINT(...)           ((void) 0)
#endif  // GFL_UI_DEBUG
#endif  //GFL_UI_PRINT
// デバッグ用決まり文句----------------------


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
    GFL_UI_SOFTRESET_USER = 0x04,  ///< ここからは外部定義になる
} GF_UI_SOFTRESET_e;


/**
 * @brief スリープ非許可用
 */
typedef enum {
  GFL_UI_SLEEP_SVLD = 0x01,      ///< セーブロード
  GFL_UI_SLEEP_AGBROM = 0x02,    ///< AGBアクセス
  GFL_UI_SLEEP_NET = 0x04,       ///< 通信全般
  GFL_UI_SLEEP_USER = 0x08,      ///< ここからは外部定義になる
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
    GFL_UI_TP_HIT_NONE = -1	///< あたったテーブルなし
} GF_UI_TP_CUSTOM_e;


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

//==============================================================================
/**
 * @brief UI起動処理(UIには初期化がboot時に行うものしかありません)
 * @param   heapID    ヒープ確保を行うID
 * @return  UISYS  workハンドル
 */
//==============================================================================
extern void GFL_UI_Boot(const HEAPID heapID);

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
 * @brief   スリープ解除時に呼ばれる関数をセットする
 * @param   pFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFL_UI_SleepReleaseSetFunc(GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

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
 * @brief   渡されたキートリガが押されたか判定する
 * @param   keyBit  キーBIT
 * @return  渡されたものだけが押されていたらTRUE
 */
//==============================================================================
extern BOOL GFL_UI_KEY_CheckTrg( int keyBit );

//==============================================================================
/**
 * @brief   渡されたキーコントが押されたか判定する
 * @param   keyBit  キーBIT
 * @return  渡されたものだけが押されていたらTRUE
 */
//==============================================================================
extern BOOL GFL_UI_KEY_CheckCont( int keyBit );

//==============================================================================
/**
 * @brief   渡されたキーリピートが押されたか判定する
 * @param   keyBit  キーBIT
 * @return  渡されたものだけが押されていたらTRUE
 */
//==============================================================================
extern BOOL GFL_UI_KEY_CheckRepeat( int keyBit );

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
#define TP_REQUEST_CHECK_MAX		5		// 命令を出して失敗したらエラーを返す回数
											
#define		TP_ONE_SYNC_BUFF (9)			// １シンクにサンプリングできる最大の数を
											// サンプリングするのに必要なバッファサイズ
											
#define		TP_ONE_SYNC_DATAMAX	(8)			// １シンクに取得できるデータ最大数


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
 * @param	sync：１フレームに何回サンプリングするのか(MAX4)
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStart( TPData* p_buff, u32 size, u32 sync);

//----------------------------------------------------------------------------
/**
 * @brief	タッチパネルのAUTOサンプリング開始	バッファリングなし
 * @param	sync：１フレームに何回サンプリングするのか(MAX4)
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFL_UI_TP_AutoStartNoBuff(u32 sync);

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



#ifdef __cplusplus
}/* extern "C" */
#endif

#endif //__UI_H__

