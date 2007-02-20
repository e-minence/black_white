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

//	リソースタイプ確認用
typedef enum {
	GFL_G3D_RES_CHKTYPE_UKN = 0,//不明
	GFL_G3D_RES_CHKTYPE_MDL,	//モデリングデータが存在するリソース
	GFL_G3D_RES_CHKTYPE_TEX,	//テクスチャデータが存在するリソース
	GFL_G3D_RES_CHKTYPE_ANM,	//アニメーションデータが存在するリソース
}GFL_G3D_RES_CHKTYPE;

//	オブジェクト描画情報構造体
typedef struct {
	VecFx32		trans;
	VecFx32		scale;
	MtxFx33		rotate;
}GFL_G3D_OBJSTATUS;

#define GEBUF_SIZE_MAX	(0x1800)	//ジオメトリバッファ最大サイズ。暫定





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
	GFL_G3D_sysInit
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
	GFL_G3D_sysExit
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
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW );
extern void
	GFL_G3D_sysProjectionSetDirect
		( const MtxFx44* param );

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
		( const u8 lightID, const VecFx16* vec, const u16 color );

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
		( const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target );

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
		( const GXSortMode sortMode, const GXBufferMode bufferMode );





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
 * ３Ｄリソースの読み込み
 *
 * @param	arcID or path	アーカイブＩＤ or ファイルパス
 * @param	datID			データＩＤ
 *
 * @return	g3Dres	３Ｄリソースポインタ(失敗=NULL)
 */
//--------------------------------------------------------------------------------------------
//-------------------------------
// アーカイブＩＤによる読み込み
extern GFL_G3D_RES*
	GFL_G3D_ResCreateArc
		( int arcID, int datID );

//-------------------------------
// アーカイブファイルパスによる読み込み
extern GFL_G3D_RES*
	GFL_G3D_ResCreatePath
		( const char* path, int datID ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの破棄
 *
 * @param	g3Dres	３Ｄリソースポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ResDelete
		( GFL_G3D_RES* g3Dres ); 
	
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
	GFL_G3D_ResFileHeaderGet
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
	GFL_G3D_ResTypeCheck
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
	GFL_G3D_VramLoadTex
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
	GFL_G3D_VramLoadTexDataOnly
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
	GFL_G3D_VramLoadTexPlttOnly
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
	GFL_G3D_VramUnloadTex
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
	GFL_G3D_VramTexkeyLiveCheck
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
	GFL_G3D_RndCreate
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーの破棄
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_RndDelete
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
	GFL_G3D_RndG3DresMdlGet
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
	GFL_G3D_RndG3DresTexGet
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
	GFL_G3D_RndRenderObjGet
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
	GFL_G3D_AnmCreate
		( GFL_G3D_RND* g3Drnd, GFL_G3D_RES* anm, int anmidx ); 

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションの破棄
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_AnmDelete
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
	GFL_G3D_AnmG3DresGet
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
	GFL_G3D_AnmAnmObjGet
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
	GFL_G3D_ObjCreate
		( GFL_G3D_RND* g3Drnd, GFL_G3D_ANM** anmTbl, u16 anmCount );

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
	GFL_G3D_ObjG3DrndGet
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
GFL_G3D_ANM*
	GFL_G3D_ObjG3DanmGet
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
u16
	GFL_G3D_ObjAnmCountGet
		( GFL_G3D_OBJ* g3Dobj ); 

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
	GFL_G3D_ObjAnmAdd
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
	GFL_G3D_ObjAnmRemove
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションフレームをリセット
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjContAnmFrameReset
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx );

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
	GFL_G3D_ObjContAnmFrameInc
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
	GFL_G3D_ObjContAnmFrameAutoLoop
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
 *		GFL_G3D_DrawStart();							//描画開始
 *		GFL_G3D_DrawLookAt();							//カメラグローバルステート設定		
 *		{
 *			GFL_G3D_ObjDraw( g3Dobj, status );			//各オブジェクト描画
 *		}
 *		GFL_G3D_DrawEnd();								//描画終了（バッファスワップ）
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
	GFL_G3D_DrawStart
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
 * ３Ｄオブジェクトの描画
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * 〜Draw	:カレント射影行列に射影変換行列が、
 *			 カレント位置座標行列と方向ベクトル行列にカメラ行列とモデリング行列が合成された行列
 *			 が設定されます。
 *
 * 〜DrawVP	:カレント射影行列に射影変換行列とカメラ行列が合成された行列が、
 *			 カレント位置座標行列と方向ベクトル行列にモデリング行列
 *			 が設定されます。
 *
 * 〜DrawWVP:カレント射影行列に射影変換行列とカメラ行列とモデリング行列が合成された行列が、
 *			 カレント位置座標行列と方向ベクトル行列に単位行列
 *			 が設定されます。
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// 通常描画
//--------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDraw
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status );

extern void
	GFL_G3D_ObjDrawVP
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status );

extern void
	GFL_G3D_ObjDrawWVP
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status );

//--------------------------------------------------------------------------------
// １つのモデルに１つのマテリアルのみ設定されているときに高速描画するための関数
//		matID	描画するマテリアルへのインデックス 
//		shpID	描画するシェイプへのインデックス 
//		sendMat	マテリアル情報をジオメトリエンジンに送信するかどうか 
//--------------------------------------------------------------------------------
extern void
	GFL_G3D_ObjDraw1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status );

extern void
	GFL_G3D_ObjDrawVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat,
			const GFL_G3D_OBJSTATUS* status );

extern void
	GFL_G3D_ObjDrawWVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat,
			const GFL_G3D_OBJSTATUS* status );


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
