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

//	オブジェクト描画タイプ定義（各種別の詳細は関数宣言コメント部を参照）
typedef enum {
	GFL_G3D_FLUSH_P = 0,	///<標準タイプ
	GFL_G3D_FLUSH_VP,		///<VPタイプ
	GFL_G3D_FLUSH_WVP,		///<WVPタイプ
}GFL_G3D_DRAWFLUSH_TYPE;

//	リソースタイプ確認用
typedef enum {
	GFL_G3D_RES_CHKTYPE_UKN = 0,//不明
	GFL_G3D_RES_CHKTYPE_MDL,	//モデリングデータが存在するリソース
	GFL_G3D_RES_CHKTYPE_TEX,	//テクスチャデータが存在するリソース
	GFL_G3D_RES_CHKTYPE_ANM,	//アニメーションデータが存在するリソース
}GFL_G3D_RES_CHKTYPE;

//	グローバルステート構造体（射影）
//		param1		PRJPERS			→fovySin :縦(Y)方向の視界角度(画角)/2の正弦をとった値
//					PRJFRST,PRJORTH	→top	  :nearクリップ面上辺のY座標
//		param2		PRJPERS			→fovyCos :縦(Y)方向の視界角度(画角)/2の余弦をとった値	
//					PRJFRST,PRJORTH	→bottom  :nearクリップ面下辺のY座標
//		param3		PRJPERS			→aspect  :縦に対する視界の割合(縦横比：視界での幅／高さ)
//					PRJFRST,PRJORTH	→left	  :nearクリップ面左辺のX座標
//		param4		PRJPERS			→未使用 
//					PRJFRST,PRJORTH	→right	  :nearクリップ面右辺のX座標
typedef struct {
	GFL_G3D_PROJECTION_TYPE	type;	///<射影行列タイプ
	fx32		param1;				///<パラメータ１（タイプによって使用法は異なる：上記参照）
	fx32		param2;				///<パラメータ２（タイプによって使用法は異なる：上記参照）
	fx32		param3;				///<パラメータ３（タイプによって使用法は異なる：上記参照）
	fx32		param4;				///<パラメータ４（タイプによって使用法は異なる：上記参照）
	fx32		near;				///<視点からnearクリップ面までの距離
	fx32		far;				///<視点からfarクリップ面までの距離
	fx32		scaleW;				///<ビューボリュームの精度調整パラメータ（使用しないときは0）
}GFL_G3D_PROJECTION;

//	グローバルステート構造体（カメラ）
typedef struct {
	VecFx32		camPos;		///<カメラの位置(＝視点)
	VecFx32		camUp;		///<カメラの上方向
	VecFx32		target;		///<カメラの焦点(＝注視点)
} GFL_G3D_LOOKAT;

//	グローバルステート構造体（レンダリングバッファ）
typedef struct {
	GXSortMode		aw;
	GXBufferMode	zw;
} GFL_G3D_SWAPBUFMODE;

//	グローバルステート構造体（ライト）
typedef struct {
	VecFx16		vec;				///<ライト方向
	u16			color;				///<色
}GFL_G3D_LIGHT;

//	オブジェクト描画情報構造体
typedef struct {
	VecFx32		trans;
	VecFx32		scale;
	MtxFx33		rotate;
}GFL_G3D_OBJSTATUS;

#define GEBUF_SIZE_MAX			(0x1800)	//ジオメトリバッファ最大サイズ。暫定

#define PERSPWAY_COEFFICIENT	0x10000/360	//画角計算係数（度数にこれを掛けるといい感じかも）

//	計算結果定義
typedef enum {
	GFL_G3D_CALC_FALSE = 0,	///<失敗
	GFL_G3D_CALC_TRUE,		///<成功
	GFL_G3D_CALC_OUTRANGE,	///<範囲外
}GFL_G3D_CALC_RESULT;




//=============================================================================================
//=============================================================================================
/**
 *
 *
 * システムマネージャ関数
 *
 *
 */
//=============================================================================================
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * システムマネージャ設定
 *
 * @param	texmanMode		テクスチャマネージャ使用モード
 * @param	texmanSize		テクスチャマネージャ使用領域サイズ
 * @param	palmanMode		パレットマネージャ使用モード
 * @param	palmanSize		パレットマネージャ使用領域サイズ
 * @param	GeBufEnable		ジオメトリバッファの使用サイズ(0:使用しない)
 * @param	heapID			メモリ取得用ヒープエリア
 * @param	setUp			セットアップ関数(NULLの時はDefaultSetUp)
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_Init
		( GFL_G3D_VMAN_MODE texmanMode, GFL_G3D_VMAN_TEXSIZE texmanSize, 
			GFL_G3D_VMAN_MODE pltmanMode, GFL_G3D_VMAN_PLTSIZE pltmanSize,
				u16 GeBufSize, HEAPID heapID, GFL_G3D_SETUP_FUNC setup );

//--------------------------------------------------------------------------------------------
/**
 *
 * マネージャ終了
 *
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_Exit
		( void );

//--------------------------------------------------------------------------------------------
/**
 *
 * 内部保持情報（グローバルステート）へのアクセス
 *
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 *
 * システム起動チェック
 *
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_CheckBootSystem
		( void );

//--------------------------------------------------------------------------------------------
/**
 * 射影行列の取得と設定
 *	保存しておく必要があるかわわからないが、とりあえず。
 *	直接射影行列を直接設定する場合、その行列は外部で持つ
 *
 * @param	projection	取得or設定用射影パラメータポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_GetSystemProjection
		( GFL_G3D_PROJECTION* projection );
extern void
	GFL_G3D_SetSystemProjection
		( const GFL_G3D_PROJECTION* projection );
extern void
	GFL_G3D_GetSystemProjectionDirect
		( MtxFx44* projectionMtx );
extern void
	GFL_G3D_SetSystemProjectionDirect
		( const MtxFx44* projectionMtx );

//--------------------------------------------------------------------------------------------
/**
 * ライトの取得と設定
 *
 * @param	lightID		ライトＩＤ
 * @param	light		取得or設定用ライトパラメータポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_GetSystemLight
		( const u8 lightID, GFL_G3D_LIGHT* light );

extern void
	GFL_G3D_SetSystemLight
		( const u8 lightID, const GFL_G3D_LIGHT* light );

//--------------------------------------------------------------------------------------------
/**
 * カメラ行列の取得と設定
 *
 * @param	lookAt		取得or設定用カメラ行列パラメータポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_GetSystemLookAt
		( GFL_G3D_LOOKAT* lookAt );
extern void
	GFL_G3D_SetSystemLookAt
		( const GFL_G3D_LOOKAT* lookAt );

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
	GFL_G3D_SetSystemSwapBufferMode
		( const GXSortMode sortMode, const GXBufferMode bufferMode );

//--------------------------------------------------------------------------------------------
/**
 * オブジェクト描画フラッシュ（グローバル状態送信）タイプの設定
 *
 * @param	type	描画タイプ
 *
 * GFL_G3D_FLUSH_N:		カレント射影行列に射影変換行列が、
 *						カレント位置座標行列と方向ベクトル行列に
 *						カメラ行列とモデリング行列が合成された行列が設定されます。
 *
 * GFL_G3D_FLUSH_VP:	カレント射影行列に射影変換行列とカメラ行列が合成された行列が、
 *						カレント位置座標行列と方向ベクトル行列にモデリング行列
 *						が設定されます。
 *
 * GFL_G3D_FLUSH_WVP:	カレント射影行列に
 *						射影変換行列とカメラ行列とモデリング行列が合成された行列が、
 *						カレント位置座標行列と方向ベクトル行列に単位行列が設定されます。
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SetDrawFlushMode
		( GFL_G3D_DRAWFLUSH_TYPE type );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャおよびパレットマネージャ状態の取得
 *
 * return GFL_G3D_VMAN_MODE		マネージャモード
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_VMAN_MODE
	GFL_G3D_GetTextureManagerMode
		( void );

extern GFL_G3D_VMAN_MODE
	GFL_G3D_GetPaletteManagerMode
		( void );





//=============================================================================================
//=============================================================================================
/**
 *
 *
 * ３Ｄデータ管理関数
 *
 *	３Ｄデータは３階層に区分する。
 *	作成手順としては以下のとおり
 *
 *	１）ファイルで配置されるＲＯＭデータの読み込み		：→リソース管理及びリソース転送参照
 *	２）１のリソースより各構成要素の作成				：→レンダー及びアニメーション管理参照
 *	３）２の要素を組み合わせてデータオブジェクトを作成	：→オブジェクト管理参照
 *
 */
//=============================================================================================
//=============================================================================================
//=============================================================================================
/**
 *
 * ３Ｄリソース管理関数
 *
 * 　各種リソースファイル（bmd,btx,bca,bva,bma,btp,bta）を一つの型で一元管理する。
 * 　リソースタイプはハンドルに内包。
 *　 全体サイズはファイルに依存するので、もちろん不定。
 *
 */
//=============================================================================================
//	リソース管理構造体型
typedef struct _GFL_G3D_RES	GFL_G3D_RES;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースヘッダのサイズ取得
 *
 * @return	ヘッダサイズ
 */
//--------------------------------------------------------------------------------------------
extern u32
	GFL_G3D_GetResourceHeaderSize
		( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの作成
 *
 * @param	g3Dres		g3Dリソースヘッダ読み込みポインタ
 * @param	header		binリソースヘッダ読み込みポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_CreateResource
		( GFL_G3D_RES* g3Dres, GFL_G3D_RES_CHKTYPE resType, void* header );

extern void
	GFL_G3D_CreateResourceAuto
		( GFL_G3D_RES* g3Dres, void* header );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの読み込み
 *		＊関数内でメモリ確保。終了時にGFL_G3D_DeleteResourceをよんで開放
 *
 * @param	arcID or path	アーカイブＩＤ or ファイルパス
 * @param	datID			データＩＤ
 *
 * @return	g3Dres	３Ｄリソースポインタ(失敗=NULL)
 */
//--------------------------------------------------------------------------------------------
// アーカイブＩＤによる読み込み
extern GFL_G3D_RES*
	GFL_G3D_CreateResourceArc
		( int arcID, int datID );

// アーカイブファイルパスによる読み込み
extern GFL_G3D_RES*
	GFL_G3D_CreateResourcePath
		( const char* path, int datID ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの破棄
 *
 * @param	g3Dres	３Ｄリソースポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DeleteResource
		( GFL_G3D_RES* g3Dres ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースを指定領域に読み込み
 * 　　領域は外部管理。ファイルデータはG3DRESヘッダの直後に置かれる
 *
 * @param	arcID or path	アーカイブＩＤ or ファイルパス
 * @param	datID			データＩＤ
 * @param	g3Dres			g3Dリソース読み込みポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_LoadResourceArc
		( int arcID, int datID, GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースヘッダポインタの取得
 *
 * @param	g3Dres		３Ｄリソースポインタ
 *
 * @return	NNSG3dResFileHeader*
 *
 * 　ＮＮＳ関数を外部で使用したい場合などに使用する
 */
//--------------------------------------------------------------------------------------------
extern NNSG3dResFileHeader*
	GFL_G3D_GetResourceFileHeader
		( GFL_G3D_RES* g3Dres );
	
//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースタイプの確認
 *
 * @param	g3Dres		３Ｄリソースポインタ
 * @param	checkType	チェックするタイプ
 *
 * @return	BOOL		TRUEで一致
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_CheckResourceType
		( GFL_G3D_RES* g3Dres, GFL_G3D_RES_CHKTYPE checkType ); 


//=============================================================================================
/**
 *
 *
 * ３Ｄリソース転送関数（事実上テクスチャデータ、テクスチャパレットのみ）
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * テクスチャリソースのＶＲＡＭ領域確保と転送
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 */
//--------------------------------------------------------------------------------------------
extern BOOL 
	GFL_G3D_TransVramTexture
		( GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャデータリソースのＶＲＡＭ領域確保と転送
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
	GFL_G3D_TransVramTextureDataOnly
		( GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャパレットリソースのＶＲＡＭ領域確保と転送
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
	GFL_G3D_TransVramTexturePlttOnly
		( GFL_G3D_RES* g3Dres );

//--------------------------------------------------------------------------------------------
/**
 * テクスチャリソースをＶＲＡＭから解放
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 */
//--------------------------------------------------------------------------------------------
extern BOOL 
	GFL_G3D_FreeVramTexture
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	テクスチャリソースの取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 */
//-----------------------------------------------------------------------------
NNSG3dResTex*
	GFL_G3D_GetResTex
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	テクスチャキーが割り振られているかチェック
 *
 *@param	g3Dobj		３Ｄオブジェクトハンドル
 *	
 *@return	BOOL		TRUE:割り振られてる、FALSE:割り振られていない
 */
//-----------------------------------------------------------------------------
extern BOOL
	GFL_G3D_CheckTextureKeyLive
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	テクスチャキーのポインタ取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 */
//-----------------------------------------------------------------------------
extern NNSG3dTexKey
	GFL_G3D_GetTextureDataVramKey
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	パレットキーのポインタ取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 */
//-----------------------------------------------------------------------------
extern NNSG3dPlttKey
	GFL_G3D_GetTexturePlttVramKey
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	テクスチャの実データアドレス取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 */
//-----------------------------------------------------------------------------
extern u32
	GFL_G3D_GetAdrsTextureData
		( GFL_G3D_RES* g3Dres );

//----------------------------------------------------------------------------
/**
 *
 *@brief	パレットの実データアドレス取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 */
//-----------------------------------------------------------------------------
extern u32
	GFL_G3D_GetAdrsTexturePltt
		( GFL_G3D_RES* g3Dres );


//=============================================================================================
/**
 *
 * ３Ｄレンダー管理関数
 *
 * 　モデリングデータとテクスチャーでデータを管理する。
 * 　モデルリソースに関しては、テクスチャ貼り付けの際、その内部を書き換えるため
 * 　１つのレンダーにつき１つのモデルリソースを持つ必要がある。
 *　 全体サイズは一定。
 *
 */
//=============================================================================================
///	レンダー管理構造体型
typedef struct _GFL_G3D_RND	GFL_G3D_RND;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーの作成
 *
 * @param	mdl			参照モデルセットリソース構造体ポインタ
 * @param	mdlidx		mdl内データインデックス(複数登録されている場合。１つの場合は0)
 * @param	tex			参照テクスチャリソース構造体ポインタ(使用しない場合はNULL)
 *
 * @return	GFL_3D_RND	３Ｄレンダーハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RND*
	GFL_G3D_RENDER_Create
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーの破棄
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_RENDER_Delete
		( GFL_G3D_RND* g3Drnd ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーのモデルリソースポインタを取得
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_RENDER_GetG3DresMdl
		( GFL_G3D_RND* g3Drnd ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーのモデルリソースポインタを取得
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_RENDER_GetG3DresTex
		( GFL_G3D_RND* g3Drnd ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーのレンダリングオブジェクトポインタを取得
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 *
 * @return	NNSG3dRenderObj*
 *
 * 　ＮＮＳ関数を外部で使用したい場合などに使用する
 */
//--------------------------------------------------------------------------------------------
extern NNSG3dRenderObj*
	GFL_G3D_RENDER_GetRenderObj
		( GFL_G3D_RND* g3Drnd );


//=============================================================================================
/**
 *
 * ３Ｄアニメーション管理関数
 *
 * 　テクスチャアニメーション＋ジョイントアニメーション等
 * 　各種アニメーションを管理する。
 * 　このアニメーションは一つのレンダーに一種類というわけではないので
 * 　レンダーとは別の構成要素として管理する。
 * 　作成の際、対象のレンダーハンドル（アロケートバッファサイズに関係する）を
 * 　必要とするので、作成手順に注意すること。
 *
 */
//=============================================================================================
///	アニメーション管理構造体型
typedef struct _GFL_G3D_ANM	GFL_G3D_ANM;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションの作成
 *
 * @param	g3Drnd		付加される対象の３Ｄレンダーハンドル
 * @param	anm			参照アニメーションリソース構造体ポインタ
 * @param	anmidx		anm内データインデックス(複数登録されている場合。１つの場合は0)
 *
 * @return	GFL_3D_ANM	３Ｄアニメーションハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_ANM*
	GFL_G3D_ANIME_Create
		( GFL_G3D_RND* g3Drnd, GFL_G3D_RES* anm, int anmidx ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションの破棄
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ANIME_Delete
		( GFL_G3D_ANM* g3Danm );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションリソースポインタを取得
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_ANIME_GetG3Dres
		( GFL_G3D_ANM* g3Danm ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションのアニメオブジェクトポインタを取得
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 *
 * @return	NNSG3dAnmObj*
 *
 * 　ＮＮＳ関数を外部で使用したい場合などに使用する
 */
//--------------------------------------------------------------------------------------------
extern NNSG3dAnmObj*
	GFL_G3D_ANIME_GetAnmObj
		( GFL_G3D_ANM* g3Danm );


//=============================================================================================
/**
 *
 * ３Ｄオブジェクト管理関数
 *
 * 　レンダー及びアニメーションによって構成される。
 * 　ステータス（座標、スケール、回転）はオブジェクトハンドルには内包しない。
 * 　※一つのオブジェクトはあくまでも描画リソースデータ郡であり
 * 　　それが１物体とは限らないため。
 * 　　　例）森を描画する際、木のオブジェクトを一つ用意し、任意のステータス配置で複数描画する。
 * 　またアニメーションは複数登録できるように構成されている。
 * 　　　例）テクスチャアニメーション＋ジョイントアニメーション
 *　 そのためハンドルサイズは不定になっていることに注意すること。
 *
 */
//=============================================================================================
///	オブジェクト管理構造体型
typedef struct _GFL_G3D_OBJ	GFL_G3D_OBJ;

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの作成
 *
 * @param	g3Drnd		レンダーハンドル
 * @param	anmTbl		付加するアニメーションハンドルテーブル（複数設定を可能にするため）
 * @param	anmCount	付加する、もしくは付加予定のアニメーションの数
 *
 * @return	GFL_3D_OBJ	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_OBJECT_Create
		( GFL_G3D_RND* g3Drnd, GFL_G3D_ANM** anmTbl, u16 anmCount );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの破棄
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_OBJECT_Delete
		( GFL_G3D_OBJ* g3Dobj );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのレンダーハンドルポインタを取得
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * @return	GFL_G3D_RND*
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RND*
	GFL_G3D_OBJECT_GetG3Drnd
		( GFL_G3D_OBJ* g3Dobj ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションハンドルポインタを取得
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 *
 * @return	GFL_G3D_ANM*
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_ANM*
	GFL_G3D_OBJECT_GetG3Danm
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーション設定数を取得
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * @return	u16
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_OBJECT_GetAnimeCount
		( GFL_G3D_OBJ* g3Dobj ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションを有効にする
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_EnableAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションを無効にする
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_DisableAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトにアニメーションを追加する
 *
 * @param	g3Dobj		３Ｄオブジェクトハンドル
 * @param	g3Danm		３Ｄアニメーションハンドル
 *
 * @return	u16			登録位置
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_OBJECT_AddAnime
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_ANM* g3Danm );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトからアニメーションを削除する
 *
 * @param	g3Dobj		３Ｄオブジェクトハンドル
 * @param	anmIdx		登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_OBJECT_RemoveAnime
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションフレームをリセット
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_ResetAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションフレームの取得と設定
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 * @param	anmFrm	取得、設定に用いる値格納ポインタ
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_GetAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, int* anmFrm );

extern BOOL
	GFL_G3D_OBJECT_SetAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, int* anmFrm );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションフレームを進める
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 * @param	count	増加分（FX32_ONEで１フレーム進める）
 *
 * @return	BOOL	FALSEで終了検出
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_IncAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションオートループ
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 * @param	count	増加分（FX32_ONEで１フレーム進める）
 *
 * @return	BOOL	FALSEで１ループ終了検出
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_OBJECT_LoopAnimeFrame
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count );


//=============================================================================================
//=============================================================================================
/**
 *
 * 描画関数
 *
 * 　３Ｄオブジェクトハンドルとそれらのステータス（座標、スケール、回転）を引数として描画を行う。
 *
 *	SAMPLE
 *	{
 *		GFL_G3D_DRAW_Start();							//描画開始
 *		GFL_G3D_DRAW_SetLookAt();						//カメラグローバルステート設定		
 *		{
 *			GFL_G3D_DRAW_DrawObject( g3Dobj, status );	//各オブジェクト描画
 *		}
 *		GFL_G3D_DRAW_End();								//描画終了（バッファスワップ）
 *	}
 *
 */
//=============================================================================================
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ３Ｄ描画の開始
 *
 * 全体描画関数内、描画開始時に呼び出される
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DRAW_Start
		( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄ描画の終了
 *
 * 全体描画関数内、描画を終了時に呼び出される
 * 内部でレンダリングバッファのスワップを行う。
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DRAW_End
		( void );

//--------------------------------------------------------------------------------------------
/**
 * カメラグローバルステートの設定
 *
 * 全体描画関数内に呼び出される
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_DRAW_SetLookAt
		( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの描画
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// 通常描画
//--------------------------------------------------------------------------------
//カリングなし
extern void
	GFL_G3D_DRAW_DrawObject
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status );
//カリングあり
extern BOOL
	GFL_G3D_DRAW_DrawObjectCullingON
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status );
//アクセサリー用
//	対象GFL_G3D_OBJのレンダリングリソース(imd)はg3dcvtr -s で出力されている必要がある。
//	※これにより各Node行列が行列スタックに格納された状態で描画を終えるため、
//	　本体直後にこの関数を呼ぶことにより、指定Node行列を参照した状態で描画することができる。
//	　Node数の最大は30まで。コンバートの際に制限があるのもこのためらしい。
//	  また、NNS_G3DGlbFlashを呼ぶ必要はなし。
//　　カリングＯＮについては本体に依存していいかなと思っているのでとりあえず作らない。
extern void
	GFL_G3D_DRAW_DrawAccesory
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_OBJ* g3Dobj_Accesory,
			const GFL_G3D_OBJSTATUS* status, const int jntID );

extern void
	GFL_G3D_DRAW_DrawAccesoryByName
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_OBJ* g3Dobj_Accesory,
			const GFL_G3D_OBJSTATUS* status, const char* jntName );

//--------------------------------------------------------------------------------
// １つのモデルに１つのマテリアルのみ設定されているときに高速描画するための関数
//		matID	描画するマテリアルへのインデックス 
//		shpID	描画するシェイプへのインデックス 
//		sendMat	マテリアル情報をジオメトリエンジンに送信するかどうか 
//--------------------------------------------------------------------------------
//カリングなし
extern void
	GFL_G3D_DRAW_DrawObject1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status );
//カリングあり
extern BOOL
	GFL_G3D_DRAW_DrawObject1mat1shpCullingON
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status );


//=============================================================================================
/**
 *
 *
 * 計算
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * 平面計算
 * 　点が平面上にあるかどうかの判定
 *
 *		指定された点が平面の方程式 (P - P1).N = 0（内積計算）
 *			※P1:平面上の任意の点,N:法線ベクトル
 *		をみたすかどうか判定
 *
 * @param	pos			指定位置
 * @param	posRef		平面上の一点の位置
 * @param	vecN		平面の法線ベクトル
 * @param	margin		許容する計算誤差幅
 *
 * @return	GFL_G3D_CALC_RESULT		判定結果
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_CALC_RESULT
	GFL_G3D_Calc_CheckPointOnPlane
		( const VecFx32* pos, const VecFx32* posRef, const VecFx32* vecN, const fx32 margin );
//--------------------------------------------------------------------------------------------
/**
 * レイトレース計算
 * 　レイと平面の交点ベクトルを算出
 *
 *		直線の方程式 P = P0 + t * V		
 *			※P:現在位置,P0:初期位置,t:経過オフセット（時間）,V:進行ベクトル 
 *		と平面の方程式 (P - P0).N = 0（内積計算）
 *			※PおよびP0:平面上の任意の２点,N:法線ベクトル
 *		を同時にみたす点Pが交点であることを利用して算出
 *
 * @param	posRay		レイの発射位置
 * @param	vecRay		レイの進行ベクトル
 * @param	posRef		平面上の一点の位置
 * @param	vecN		平面の法線ベクトル
 * @param	dest		交点の位置
 * @param	margin		許容する計算誤差幅
 *
 * @return	GFL_G3D_CALC_RESULT		算出結果
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_CALC_RESULT
	GFL_G3D_Calc_GetClossPointRayPlane
		( const VecFx32* posRay, const VecFx32* vecRay, 
			const VecFx32* posRef, const VecFx32* vecN, VecFx32* dest, const fx32 margin );
//--------------------------------------------------------------------------------------------
/**
 * レイトレース計算（距離制限つき）
 * 　レイと平面の交点ベクトルを算出
 *
 *		直線の方程式 P = P0 + t * V		
 *			※P:現在位置,P0:初期位置,t:経過オフセット（時間）,V:進行ベクトル 
 *		と平面の方程式 (P - P1).N = 0（内積計算）
 *			※PおよびP1:平面上の任意の２点,N:法線ベクトル
 *		を同時にみたす点Pが交点であることを利用して算出
 *
 *		両式を連立させ P = P1 + ((P1 - P0 ).N / V.N) * V
 *		という方程式を得る
 *
 * @param	posRay		レイの発射位置
 * @param	posRayEnd	レイの最終到達位置
 * @param	posRef		平面上の一点の位置
 * @param	vecN		平面の法線ベクトル
 * @param	dest		交点の位置
 * @param	margin		許容する計算誤差幅
 *
 * @return	GFL_G3D_CALC_RESULT		算出結果
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_CALC_RESULT
	GFL_G3D_Calc_GetClossPointRayPlaneLimit
		( const VecFx32* posRay, const VecFx32* posRayEnd, 
			const VecFx32* posRef, const VecFx32* vecN, VecFx32* dest, const fx32 margin );

//=============================================================================================
/**
 *
 *
 * データ
 *
 *
 */
//=============================================================================================



#endif
