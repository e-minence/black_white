//*****************************************************************************
/**
 * @file	ui_def.h
 * @brief	ユーザーインターフェイス用グループ内定義
 * @author	k.ohno
 * @date	2006.11.16
 */
//*****************************************************************************

#ifndef __UI_DEF_H__
#define	__UI_DEF_H__


//------------------------------------------------------------------
/**
 * @brief   UI管理構造体を引き出す グループ内関数
 * @param   none
 * @return  ユーザーインターフェイス管理構造体
 */
//------------------------------------------------------------------
extern UISYS* _UI_GetUISYS(void);

//------------------------------------------------------------------
/**
 * @brief   キー管理構造体を引き出す グループ内関数
 * @param   pUI		ユーザーインターフェイス管理構造体
 * @return  キー管理構造体
 */
//------------------------------------------------------------------
extern UI_KEYSYS* _UI_GetKEYSYS(const UISYS* pUI);

//==============================================================================
/**
 * @brief  キー初期化
 * @param   heapID    ヒープ確保を行うID
 * @return  UI_KEYSYS  キーシステムワーク
 */
//==============================================================================
extern UI_KEYSYS* GFL_UI_KEY_Init(const HEAPID heapID);

//==============================================================================
/**
 * @brief キー読み取り処理
 * @param   none
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_Main(void);

//==============================================================================
/**
 * @brief キー終了処理
 * @param   none
 * @return  none
 */
//==============================================================================
extern void GFL_UI_KEY_Exit(void);

//
//==============================================================================
/**
 * @brief UIMain処理
 * @return  none
 */
//==============================================================================
extern void GFI_UI_Main(UISYS* pUI);

//==============================================================================
/**
 * @brief   UI終了処理
 * @return  none
 */
//==============================================================================

extern void GFI_UI_Exit(UISYS* pUI);

//------------------------------------------------------------------
/**
 * @brief   スリープ状態を禁止する
 * @param   sleepTypeBit スリープ管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFI_UI_SleepDisable(UISYS* pUI,const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   スリープ状態を許可する
 * @param   sleepTypeBit スリープ管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFI_UI_SleepEnable(UISYS* pUI,const u8 sleepTypeBit);

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット状態を禁止する
 * @param   softResetBit リセット管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFI_UI_SoftResetDisable(UISYS* pUI,const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   ソフトウエアリセット状態を許可する
 * @param   softResetBit リセット管理BIT
 * @return  none
 */
//------------------------------------------------------------------
extern void GFI_UI_SoftResetEnable(UISYS* pUI, const u8 softResetBit);

//------------------------------------------------------------------
/**
 * @brief   スリープ解除時に呼ばれる関数をセットする
 * @param   pUI		ユーザーインターフェイス管理構造体
 * @param   pFunc   スリープ解除時に呼ばれる関数
 * @param   pWork   呼ばれる際に渡すワーク
 * @return  none
 */
//------------------------------------------------------------------
extern void GFI_UI_SleepReleaseSetFunc(UISYS* pUI, GFL_UI_SLEEPRELEASE_FUNC* pFunc, void* pWork);

//----------------------------------------------------------------------------
//プロトタイプ宣言 touchpanel.c
//----------------------------------------------------------------------------


//==============================================================================
/**
 * @brief オートサンプリングを行っているかどうかを得る
 * @param[in]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @retval  TRUE  オートサンプリングである
 * @retval  FALSE  していない
 */
//==============================================================================
extern int GFI_UI_TP_GetAutoSamplingFlg(const UISYS* pUI);

//==============================================================================
/**
 * @brief オートサンプリング設定
 * @param[out]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @param[in]    bAuto   オートサンプリングするならTRUE
 * @return  none
 */
//==============================================================================
extern void GFI_UI_TP_SetAutoSamplingFlg(UISYS* pUI, const BOOL bAuto);

//------------------------------------------------------------------
/**
 * @brief 両タイプ（矩形・円形）を見ながら判定する（ベタ入力）
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @param[in]   tbl		当たり判定テーブル（終端コードあり）
 * @return  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFI_UI_TP_HitCont( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief 両タイプ（矩形・円形）を見ながら判定する（トリガ入力）
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @param[in]   tbl		当たり判定テーブル（終端コードあり）
 * @retval  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//------------------------------------------------------------------
extern int GFI_UI_TP_HitTrg( const UISYS* pUI, const GFL_UI_TP_HITTBL *tbl );

//------------------------------------------------------------------
/**
 * @brief  タッチパネルに触れているか
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @retval  BOOL		TRUEで触れている
 */
//------------------------------------------------------------------
extern BOOL GFI_UI_TP_GetCont( const UISYS* pUI );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているか（トリガ）
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @retval  BOOL		TRUEで触れた
 */
//------------------------------------------------------------------
extern BOOL GFI_UI_TP_GetTrg( const UISYS* pUI );

//----------------------------------------------------------------------------
/**
 *	@brief	指定した座標で、当たり判定を行います。	複数
 *	@param	tbl		当たり判定テーブル（配列）
 *	@param	x		判定ｘ座標
 *	@param	y		判定ｙ座標
 *	@retval  int		当たりがあればその要素番号、なければ TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
extern int GFI_UI_TP_HitSelf( const GFL_UI_TP_HITTBL *tbl, u32 x, u32 y );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているならその座標取得（ベタ入力）
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @param[out]   x		Ｘ座標受け取り変数アドレス
 * @param[out]   y		Ｙ座標受け取り変数アドレス
 * @retval  TRUE  触れている
 * @retval  FALSE 触れていない。引数には何もしない。
 */
//------------------------------------------------------------------
extern BOOL GFI_UI_TP_GetPointCont( const UISYS* pUI, u32* x, u32* y );

//------------------------------------------------------------------
/**
 * @brief タッチパネルに触れているならその座標取得（トリガ入力）
 * @param[in]   pUI	    ユーザーインターフェイスシステム
 * @param[out]   x		Ｘ座標受け取り変数アドレス
 * @param[out]   y		Ｙ座標受け取り変数アドレス
 * @retval  TRUE  触れている
 * @retval  FALSE 触れていない。引数には何もしない。
 */
//------------------------------------------------------------------
extern BOOL GFI_UI_TP_GetPointTrg( const UISYS* pUI, u32* x, u32* y );

//----------------------------------------------------------------------------
//プロトタイプ宣言 key.c
//----------------------------------------------------------------------------

//==============================================================================
/**
 * @brief キーリピートの速度とウェイトをセット
 * @param[in,out]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @param[in]	speed	速度
 * @param[in]	wait	ウェイト
 * @return  none
 */
//==============================================================================
extern void GFI_UI_KEY_SetRepeatSpeed(UISYS* pUI, const int speed, const int wait );

//==============================================================================
/**
 * @brief キーコンフィグテーブルを登録
 * @param[in,out]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @param[in]	pTbl   コントロールテーブル配列のポインタ
 * @return  none
 */
//==============================================================================
extern void GFI_UI_KEY_SetControlModeTbl(UISYS* pUI, const GFL_UI_KEY_CUSTOM_TBL* pTbl );

//==============================================================================
/**
 * @brief キートリガゲット
 * @param[in]   pKey     ユーザーインターフェイスハンドルのポインタ
 * @return  キートリガ
 */
//==============================================================================
extern GFI_UI_KEY_GetTrg( const UI_KEYSYS* pKey );

//==============================================================================
/**
 * @brief キーコントゲット
 * @param[in]   pKey     ユーザーインターフェイスハンドルのポインタ
 * @return  キーコント
 */
//==============================================================================
extern GFI_UI_KEY_GetCont( const UI_KEYSYS* pKey );

//==============================================================================
/**
 * @brief キーリピートゲット
 * @param[in]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @return  キーコント
 */
//==============================================================================
extern GFI_UI_KEY_GetRepeat( const UISYS* pUI );

//==============================================================================
/**
 * @brief キーリピートの速度とウェイトをゲット
 * @param[in]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @param[out]	speed	速度
 * @param[out]	wait	ウェイト
 * @return  none
 */
//==============================================================================
extern void GFI_UI_KEY_GetRepeatSpeed(const UISYS* pUI, int* speed, int* wait );

//==============================================================================
/**
 * @brief   キーコンフィグのモードの値を設定する
 * @param[in,out]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @param[in]   mode    キーコンフィグモード
 * @return  none
 */
//==============================================================================
extern void GFI_UI_KEY_SetControlMode(UISYS* pUI,const int mode);

//==============================================================================
/**
 * @brief キーコンフィグのモードの値を得る
 * @param[in]   pUI     ユーザーインターフェイスハンドルのポインタ
 * @return  コントロールモード
 */
//==============================================================================
extern int GFI_UI_KEY_GetControlMode(const UISYS* pUI);


//----------------------------------------------------------------------------
/**
 *
 * @brief	サンプリング情報を管理し、今の状態を返す	
 * @param   pUI  UIワーク
 * @param	pData：このフレームの情報(initで指定したサンプリング回数分の情報)
 * @param	type：サンプリング種別の番号
 * @param	comp_num：バッファに格納するときに、comp_num位のさがあったら格納する
 *
 * @return	u32：サンプリング種別による変化
					type：TP_BUFFERING		サンプリングされたバッファサイズ
					type：TP_NO_LOOP		サンプリングされたバッファサイズ
											バッファが一杯になったとき TP_END_BUFF
					type：TP_NO_BUFF		TP_OK

					type：TP_SAMP_NOT_START	サンプリング開始されていません
 */
//-----------------------------------------------------------------------------
extern u32 GFI_UI_TPAutoSamplingMain( UISYS* pUI, TP_ONE_DATA* pData, u32 type, u32 comp_num );

//----------------------------------------------------------------------------
/**
 * @brief	スリープ処理後の再開処理
 * @param   pUI             UIのワーク
 * @return	none
 */
//-----------------------------------------------------------------------------
extern void GFI_UI_TPAutoSamplingReStart( UISYS* pUI );

//----------------------------------------------------------------------------
/**
 * @brief	スリープ処理前の停止処理
 * @param   pUI             UIのワーク
 * @return  none
 */
//-----------------------------------------------------------------------------
extern void GFI_UI_TPAutoSamplingStop( UISYS* pUI );

//----------------------------------------------------------------------------
/**
 * @brief	タッチパネルのAUTOサンプリング開始
 * @param	pUI： UIworkポインタ
 * @param	p_buff：サンプリングデータを入れるバッファ
 * @param	size：バッファのサイズ
 * @param	sync：１フレームに何回サンプリングするのか(MAX4)
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFI_UI_TPAutoStart(UISYS* pUI, TPData* p_buff, u32 size, u32 sync);

//----------------------------------------------------------------------------
/**
 * @brief	タッチパネルのAUTOサンプリング開始	バッファリングなし
 * @param	pUI： UIworkポインタ
 * @param	sync：１フレームに何回サンプリングするのか(MAX4)
 * @retval	TP_OK：成功
 * @retval	TP_FIFO_ERR：転送失敗
 * @retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFI_UI_TPAutoStartNoBuff(UISYS* pUI, u32 sync);

//----------------------------------------------------------------------------
/**
 *@brief	サンプリングを終了する
 *@param	pUI： UIworkポインタ
 *@retval	TP_OK：成功
 *@retval　	TP_FIFO_ERR：転送失敗
 *@retval	TP_ERR：転送以外の失敗
 */
//-----------------------------------------------------------------------------
extern u32 GFI_UI_TPAutoStop( UISYS* pUI );




#endif  //__UI_DEF_H__

