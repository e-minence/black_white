//=============================================================================================
/**
 * @file	g3d_system.h
 * @brief	３Ｄ描画管理システムプログラム
 */
//=============================================================================================
#ifndef _G3D_SYSTEM_H_
#define _G3D_SYSTEM_H_

//=============================================================================================
/**
 *
 *
 * システムマネージャ関数
 *
 *
 */
//=============================================================================================
//	セットアップ関数型
typedef void (*GFL_G3D_SETUP_FUNC)( void );

//	各リソースマネージャモード定義	
typedef enum {
	GFL_G3D_VMANLNK = 0,		///<リンクドモード(取得解放が存在する断片化に注意)
	GFL_G3D_VMANFRM,			///<フレームモード(とりっぱなし)
}GFL_G3D_VMAN_MODE;

//	テクスチャマネージャサイズ定義	
typedef enum {
	GFL_G3D_TEX128K = 1,	///<128KB(1ブロック)
	GFL_G3D_TEX256K = 2,	///<256KB(2ブロック)
	GFL_G3D_TEX384K = 3,	///<384KB(3ブロック)
	GFL_G3D_TEX512K = 4,	///<512KB(4ブロック)
}GFL_G3D_VMAN_TEXSIZE;

//	パレットマネージャサイズ定義	
typedef enum {
	GFL_G3D_PLT16K = 1,		///<16KB
	GFL_G3D_PLT32K = 2,		///<32KB
	GFL_G3D_PLT64K = 4,		///<64KB
	GFL_G3D_PLT80K = 5,		///<80KB
	GFL_G3D_PLT96K = 6,		///<96KB
}GFL_G3D_VMAN_PLTSIZE;

//	グローバルステート定義（射影行列タイプ）
typedef enum {
	GFL_G3D_PRJPERS = 0,	///<透視射影を設定
	GFL_G3D_PRJFRST,		///<透視射影を設定
	GFL_G3D_PRJORTH,		///<正射影を設定
	GFL_G3D_PRJMTX,			///<射影行列を設定(設定の際は上記タイプとは別関数)
}GFL_G3D_PROJECTION_TYPE;

#define GEBUF_SIZE_MAX	(0x1800)	//ジオメトリバッファ最大サイズ。暫定

//--------------------------------------------------------------------------------------------
/**
 * システムマネージャ設定
 *
 * @param	texmanMode		テクスチャマネージャ使用モード
 * @param	texmanSize		テクスチャマネージャ使用領域サイズ（byte）
 * @param	palmanMode		パレットマネージャ使用モード
 * @param	palmanSize		パレットマネージャ使用領域サイズ（byte）
 * @param	GeBufEnable		ジオメトリバッファの使用サイズ(0:使用しない)
 * @param	heapID			メモリ取得用ヒープエリア
 * @param	setUp			セットアップ関数(NULLの時はDefaultSetUp)
 *
 * @return	マネージャワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysInit
		( GFL_G3D_VMAN_MODE texmanMode, GFL_G3D_VMAN_TEXSIZE texmanSize, 
			GFL_G3D_VMAN_MODE pltmanMode, GFL_G3D_VMAN_PLTSIZE pltmanSize,
				u16 GeBufSize, u16 heapID, GFL_G3D_SETUP_FUNC setup );

//--------------------------------------------------------------------------------------------
/**
 *
 * 終了
 *
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysExit
		( void );

//--------------------------------------------------------------------------------------------
/**
 * 内部保持情報（グローバルステート）へのアクセス
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * 射影行列の設定
 *	保存しておく必要があるかわわからないが、とりあえず。
 *	直接射影行列を直接設定する場合、その行列は外部で持つ
 *
 * @param	type		射影タイプ
 * @param	param1		PRJPERS			→fovySin :縦(Y)方向の視界角度(画角)/2の正弦をとった値
 *						PRJPERS,PRJORTH	→top	  :nearクリップ面上辺のY座標
 * @param	param2		PRJPERS			→fovyCos :縦(Y)方向の視界角度(画角)/2の余弦をとった値	
 *						PRJPERS,PRJORTH	→bottom  :nearクリップ面下辺のY座標
 * @param	param3		PRJPERS			→aspect  :縦に対する視界の割合(縦横比：視界での幅／高さ)
 *						PRJPERS,PRJORTH	→left	  :nearクリップ面左辺のX座標
 * @param	param4		PRJPERS			→未使用 
 *						PRJPERS,PRJORTH	→right	  :nearクリップ面右辺のX座標
 * @param	near		視点からnearクリップ面までの距離	
 * @param	far			視点からfarクリップ面までの距離	
 * @param	scaleW		ビューボリュームの精度調整パラメータ（使用しないときは0）
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysProjectionSet
		( GFL_G3D_PROJECTION_TYPE type, fx32 param1, fx32 param2, fx32 param3, fx32 param4, 
			fx32 near, fx32 far, fx32 scaleW );
extern void
	GFL_G3D_sysProjectionSetDirect
		( MtxFx44* param );
//--------------------------------------------------------------------------------------------
/**
 * ライトの設定
 *
 * @param	lightID			ライトＩＤ
 * @param	vec				ライトのベクトルポインタ
 * @param	color			色
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysLightSet
		( u8 lightID, VecFx16* vec, u16 color );
//--------------------------------------------------------------------------------------------
/**
 * カメラ行列の設定
 *
 * @param	camPos			カメラ位置ベクトルポインタ
 * @param	camUp			カメラの上方向のベクトルへのポインタ
 * @param	target			カメラ焦点へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysLookAtSet
		( VecFx32* camPos, VecFx32* camUp, VecFx32* target );
//--------------------------------------------------------------------------------------------
/**
 * レンダリングスワップバッファの設定
 *
 * @param	sortMode		ソートモード
 * @param	bufferMode		デプスバッファモード
 * 設定値についてはNitroSDK内GX_SwapBuffer関数を参照
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_sysSwapBufferModeSet
		( GXSortMode sortMode, GXBufferMode bufferMode );


//=============================================================================================
/**
 *
 *
 * ３Ｄリソース管理関数
 *
 *
 */
//=============================================================================================
//	リソース管理構造体型
typedef struct _GFL_G3D_RES	GFL_G3D_RES;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの読み込み
 *
 * @param	arcID or path	アーカイブＩＤ or ファイルパス
 * @param	datID			データＩＤ
 *
 * @return	g3Dres	３Ｄリソースポインタ(失敗=NULL)
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_ResourceCreateArc
		( int arcID, int datID ); 
extern GFL_G3D_RES*
	GFL_G3D_ResourceCreatePath
		( const char* path, int datID ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの破棄
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ResourceDelete
		( GFL_G3D_RES* g3Dres );
	
//--------------------------------------------------------------------------------------------
/**
 * テクスチャリソースの転送(→ＶＲＡＭ)
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 */
//--------------------------------------------------------------------------------------------
extern BOOL 
	GFL_G3D_TransVramTex
		( GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャデータリソースの転送(→ＶＲＡＭ)
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 *
 * 連続してテクスチャを読み込む場合に効率的に使用するための関数
 * 連続読み込みの際は、外部で
 *    開始時にGX_BeginLoadTex関数
 *    終了時にGX_EndLoadTex関数
 * を呼ぶ必要がある
 */
//--------------------------------------------------------------------------------------------
extern BOOL 
	GFL_G3D_TransVramTexDataOnly
		( GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャパレットリソースの転送(→ＶＲＡＭ)
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 *
 * 連続してパレットを読み込む場合に効率的に使用するための関数
 * 連続読み込みの際は、外部で
 *    開始時にGX_BeginLoadPltt関数
 *    終了時にGX_EndLoadPltt関数
 * を呼ぶ必要がある
 */
//--------------------------------------------------------------------------------------------
extern BOOL 
	GFL_G3D_TransVramTexPlttOnly
		( GFL_G3D_RES* g3Dres );


//=============================================================================================
/**
 *
 *
 * 各オブジェクト管理関数
 *
 *
 */
//=============================================================================================
//	オブジェクト管理構造体型
typedef struct _GFL_G3D_OBJ	GFL_G3D_OBJ;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの作成
 *
 * @param	mdl			参照モデルセットリソース構造体ポインタ
 * @param	mdlidx		mdl内データインデックス(複数登録されている場合。１つの場合は0)
 * @param	tex			参照テクスチャリソース構造体ポインタ(使用しない場合はNULL)
 * @param	anm			参照アニメーションリソース構造体ポインタ(使用しない場合はNULL)
 * @param	anmidx		anm内データインデックス(複数登録されている場合。１つの場合は0)
 *
 * @return	GFL_3D_OBJ	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_ObjCreate
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex, GFL_G3D_RES* anm, int anmidx );  

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの破棄
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDelete
		( GFL_G3D_OBJ* g3Dobj ); 
	

//=============================================================================================
/**
 *
 *
 * 各オブジェクト描画関数
 *
 *	SAMPLE	//オブジェクトは、vecTrans,vecRotate,vecScale の情報を持っているものとする
 *	{
 *		MtxFx33 mtxRotate;						//回転変数宣言
 *
 *		GFL_G3D_DrawStart();							//描画開始
 *		GFL_G3D_DrawLookAt();							//カメラグローバルステート設定		
 *		{
 *			//各オブジェクト描画
 *			GFL_G3D_ObjDrawStart();										//各オブジェクト描画開始
 *			GFL_G3D_ObjDrawRotateCalcXY( &vecRotate, &mtxRotate );		//各オブジェクト回転計算
 *			GFL_G3D_ObjDrawStatusSet( &vecTrans, mtxRotate, &vecScale);	//各オブジェクト情報転送
 *			GFL_G3D_ObjDraw( g3Dobj );									//各オブジェクト描画
 *		}
 *		GFL_G3D_DrawEnd();								//描画終了（バッファスワップ）
 *	}
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ３Ｄ描画の開始
 *
 * 全体描画関数内、描画開始時に呼び出される
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DrawStart
		( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄ描画の終了
 *
 * 全体描画関数内、描画を終了時に呼び出される
 * 内部でバッファのスワップを行う。
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DrawEnd
		( void );

//--------------------------------------------------------------------------------------------
/**
 * カメラグローバルステートの設定
 *
 * 全体描画関数内に呼び出される
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DrawLookAt
		( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの描画の開始
 *
 * グローバルステート
 * （射影変換行列、カメラ行列、モデリング行列、ビューポート、ライト設定、マテリアルカラー等）
 * をジオメトリエンジンに送信する。
 * 特にこのステートに外部加工をしていなければ、各オブジェクト描画関数(ObjDraw)直前に
 * いずれかの関数を毎回呼び出すことになる。
 */
//--------------------------------------------------------------------------------------------
// カレント射影行列に射影変換行列が、
// カレント位置座標行列と方向ベクトル行列にカメラ行列とモデリング行列が合成された行列
// が設定されます。
extern void
	GFL_G3D_ObjDrawStart
		( void );
// カレント射影行列に射影変換行列とカメラ行列が合成された行列が、
// カレント位置座標行列と方向ベクトル行列にモデリング行列
// が設定されます。
extern void
	GFL_G3D_ObjDrawStartVP
		( void ); 
// カレント射影行列に射影変換行列とカメラ行列とモデリング行列が合成された行列が、
// カレント位置座標行列と方向ベクトル行列に単位行列
// が設定されます。
extern void
	GFL_G3D_ObjDrawStartWVP
		( void ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの描画
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * 通常描画関数
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDraw
		( GFL_G3D_OBJ* g3Dobj );
	
//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの描画(1mat1shape)
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * １つのモデルに１つのマテリアルのみ設定されているときに高速描画するための関数
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDraw1mat1shape
		( GFL_G3D_OBJ* g3Dobj );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの情報設定
 *
 * @param	pTrans	位置情報ベクトルポインタ
 * @param	pRotate	回転情報マトリクスポインタ
 * @param	pScale	拡縮情報ベクトルポインタ
 *
 * オブジェクト情報を転送。各オブジェクト描画の前に設定される
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDrawStatusSet
		( VecFx32* pTrans, MtxFx33* pRotate, VecFx32* pScale );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの回転行列の作成
 *
 * @param	rotSrc	計算前の回転ベクトルポインタ
 * @param	rotDst	計算後の回転行列格納ポインタ
 *
 * この関数等を使用し、オブジェクト毎に適切な回転行列を作成したものを、描画に流す。
 */
//--------------------------------------------------------------------------------------------
// Ｘ→Ｙ→Ｚの順番で計算
extern void
	GFL_G3D_ObjDrawRotateCalcXY
		( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｘ→Ｙ→Ｚの順番で計算（相対）
extern void
	GFL_G3D_ObjDrawRotateCalcXY_Rev
		( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｙ→Ｘ→Ｚの順番で計算
extern void
	GFL_G3D_ObjDrawRotateCalcYX
		( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｙ→Ｘ→Ｚの順番で計算（相対）
extern void
	GFL_G3D_ObjDrawRotateCalcYX_Rev
		( VecFx32* rotSrc, MtxFx33* rotDst );

//=============================================================================================
/**
 *
 *
 * 各オブジェクトコントロール関数
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * アニメーションフレームをリセット
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjContAnmFrameReset
		( GFL_G3D_OBJ* g3Dobj );

//--------------------------------------------------------------------------------------------
/**
 * アニメーションフレームを進める
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * @return	BOOL	FALSEで終了検出
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_ObjContAnmFrameInc
		( GFL_G3D_OBJ* g3Dobj ); 


#endif

