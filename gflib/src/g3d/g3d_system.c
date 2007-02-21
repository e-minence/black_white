//=============================================================================================
/**
 * @file	g3d_system.c                                                  
 * @brief	３Ｄ管理システムプログラム
 * @date	2006/4/26
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	定数宣言
//=============================================================================================
#define TEX_BLOCKNUM	(128)
#define PLT_BLOCKNUM	(256)

#define TEX_SLOTSIZ		(0x20000)
#define PLT_SLOTSIZ		(0x2000)

//=============================================================================================
//	型宣言
//=============================================================================================
//	グローバルステート構造体（射影）
typedef struct {
	GFL_G3D_PROJECTION_TYPE	type;	///<射影行列タイプ
	fx32		param1;				///<パラメータ１（タイプによって使用法は異なる）
	fx32		param2;				///<パラメータ２（タイプによって使用法は異なる）
	fx32		param3;				///<パラメータ３（タイプによって使用法は異なる）
	fx32		param4;				///<パラメータ４（タイプによって使用法は異なる）
	fx32		near;				///<視点からnearクリップ面までの距離
	fx32		far;				///<視点からfarクリップ面までの距離
	fx32		scaleW;				///<ビューボリュームの精度調整パラメータ（使用しないときは0）
}GFL_G3D_PROJECTION;

//	グローバルステート構造体（ライト）
typedef struct {
	VecFx16		vec;				///<ライト方向
	u16			color;				///<色
}GFL_G3D_LIGHT;

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

/**
 * @brief	３Ｄシステムマネージャ構造体
 */
typedef struct GFL_G3D_MAN_tag
{
	NNSFndAllocator		allocater;			///<メモリアロケータ(NNSで使用)
	void*				plt_memory;			///<パレットマネージャ用メモリ
	void*				tex_memory;			///<テクスチャマネージャ用メモリ
	HEAPID				heapID;				///<ヒープＩＤ

	GFL_G3D_PROJECTION	projection;			///<グローバルステート（射影）
	GFL_G3D_LIGHT		light[4];			///<グローバルステート（ライト）
	GFL_G3D_LOOKAT		lookAt;				///<グローバルステート（カメラ）
	GFL_G3D_SWAPBUFMODE	swapBufMode;		///<グローバルステート（レンダリングバッファ）

}GFL_G3D_MAN;

static GFL_G3D_MAN*  g3Dman = NULL;

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
void
	GFL_G3D_sysInit
		( GFL_G3D_VMAN_MODE texmanMode, GFL_G3D_VMAN_TEXSIZE texmanSize, 
			GFL_G3D_VMAN_MODE pltmanMode, GFL_G3D_VMAN_PLTSIZE pltmanSize,
				u16 GeBufSize, HEAPID heapID, GFL_G3D_SETUP_FUNC setup )
{
	int	tex_size,plt_size;

	GF_ASSERT( g3Dman == NULL );

	g3Dman = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_MAN) );

	g3Dman->heapID = heapID;

	// NitroSystem:３Ｄエンジンの初期化
	NNS_G3dInit();
	// マトリクススタックの初期化
    G3X_InitMtxStack();
	// グローバルステート初期化
	NNS_G3dGlbInit();

	// ジオメトリコマンドバッファの設定
	if( GeBufSize ){
		NNSG3dGeBuffer* geBuf;

		if( GeBufSize > GEBUF_SIZE_MAX ){
			GeBufSize = GEBUF_SIZE_MAX;
		}
		geBuf = ( NNSG3dGeBuffer* )GFL_HEAP_DTCM_AllocMemory( GeBufSize );
		NNS_G3dGeSetBuffer( geBuf );
	}
	// ジオメトリエンジン起動後必ず呼ばれなければならない
    G3_SwapBuffers( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );

	//ＮＮＳ関数で使用するアロケータのセットアップ
	GFL_HEAP_InitAllocator( &g3Dman->allocater, g3Dman->heapID, 4 ); 

	//ＶＲＡＭテクスチャマネージャの設定
	if( texmanMode == GFL_G3D_VMANLNK ){
		//リンクドモードの設定
		tex_size = NNS_GfdGetLnkTexVramManagerWorkSize( TEX_BLOCKNUM * texmanSize );
		g3Dman->tex_memory = GFL_HEAP_AllocMemory( g3Dman->heapID, tex_size );

		// マネージャがテクスチャイメージスロットを指定スロット分管理できるようにする
		NNS_GfdInitLnkTexVramManager
			( TEX_SLOTSIZ*texmanSize, 0, g3Dman->tex_memory, tex_size, TRUE);
	} else {
		//フレームモードの設定
		NNS_GfdInitFrmTexVramManager( texmanSize, TRUE);
		g3Dman->tex_memory = NULL;
	}

	//ＶＲＡＭパレットマネージャの設定
	if( pltmanMode == GFL_G3D_VMANLNK ){
		//リンクドモードの設定
		plt_size = NNS_GfdGetLnkPlttVramManagerWorkSize( PLT_BLOCKNUM * pltmanSize );
		g3Dman->plt_memory = GFL_HEAP_AllocMemory( g3Dman->heapID, plt_size );

		// マネージャがテクスチャイメージスロットを指定サイズ分管理できるようにする
		NNS_GfdInitLnkPlttVramManager
			( PLT_SLOTSIZ * pltmanSize, g3Dman->plt_memory, plt_size, TRUE);
	} else {
		//フレームモードの設定
		NNS_GfdInitFrmTexVramManager( PLT_SLOTSIZ * pltmanSize, TRUE);
		g3Dman->tex_memory = NULL;
	}

	//グローバルステート内部情報初期化
	{
		VecFx32 initVec32 = { 0, 0, 0 };
		VecFx16 initVec16 = { -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) };

		//射影
		GFL_G3D_sysProjectionSet( GFL_G3D_PRJPERS,
						FX_SinIdx( 0x1000 ), FX_CosIdx( 0x1000 ), ( FX32_ONE * 4 / 3 ), 0, 
						( 1 << FX32_SHIFT ), ( 1024 << FX32_SHIFT ), 0 );
		//ライト
		GFL_G3D_sysLightSet( 0, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 1, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 2, &initVec16, 0x7fff );
		GFL_G3D_sysLightSet( 3, &initVec16, 0x7fff );
		//カメラ
		GFL_G3D_sysLookAtSet( &initVec32, &initVec32, &initVec32 );
		g3Dman->lookAt.camPos.z	= ( 256 << FX32_SHIFT );
		g3Dman->lookAt.camUp.y	= FX32_ONE;
		//レンダリングスワップバッファ
		GFL_G3D_sysSwapBufferModeSet( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
	}

	if( setup != NULL ){
		setup();
	} else {	//各種モードデフォルトセットアップ
		// 各種描画モードの設定(シェード＆アンチエイリアス＆半透明)
		G3X_SetShading( GX_SHADING_TOON );
		G3X_AntiAlias( FALSE );
		G3X_AlphaTest( FALSE, 0 );	// アルファテスト　　オフ
		G3X_AlphaBlend( FALSE );		// アルファブレンド　オン
		G3X_EdgeMarking( FALSE );
		G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

		// クリアカラーの設定
		G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

		// ビューポートの設定
		G3_ViewPort(0, 0, 255, 191);
	}
}

//--------------------------------------------------------------------------------------------
/**
 *
 * マネージャ終了
 *
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysExit
		( void )
{
	GF_ASSERT( g3Dman != NULL );

	//ＶＲＡＭパレットマネージャの解放
	if( g3Dman->plt_memory != NULL ){
		GFL_HEAP_FreeMemory( g3Dman->plt_memory );
	}
	//ＶＲＡＭテクスチャマネージャの解放
	if( g3Dman->tex_memory != NULL ){
		GFL_HEAP_FreeMemory( g3Dman->tex_memory );
	}
	// ジオメトリコマンドバッファの解放
	if( NNS_G3dGeIsBufferExist() == TRUE ){
		NNSG3dGeBuffer* geBuf = NNS_G3dGeReleaseBuffer();
		GFL_HEAP_DTCM_FreeMemory( geBuf );
	}
	GFL_HEAP_FreeMemory( g3Dman );
	g3Dman = NULL;
}

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
void
	GFL_G3D_sysProjectionSet
		( const GFL_G3D_PROJECTION_TYPE type, 
			const fx32 param1, const fx32 param2, const fx32 param3, const fx32 param4, 
				const fx32 near, const fx32 far, const fx32 scaleW )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->projection.type		= type;
	g3Dman->projection.param1	= param1;
	g3Dman->projection.param2	= param2;
	g3Dman->projection.param3	= param3;
	g3Dman->projection.param4	= param4;
	g3Dman->projection.near		= near;
	g3Dman->projection.far		= far;
	g3Dman->projection.scaleW	= scaleW;

	switch( type ){
		case GFL_G3D_PRJPERS:	// 透視射影を設定
			if( !scaleW ){
				NNS_G3dGlbPerspective( param1, param2, param3, near, far );
			} else {
				NNS_G3dGlbPerspectiveW( param1, param2, param3, near, far, scaleW );
			}
			break;
		case GFL_G3D_PRJFRST:	// 透視射影を設定
			if( !scaleW ){
				NNS_G3dGlbFrustum( param1, param2, param3, param4, near, far );
			} else {
				NNS_G3dGlbFrustumW( param1, param2, param3, param4, near, far, scaleW );
			}
			break;
		case GFL_G3D_PRJORTH:	// 正射影を設定
			if( !scaleW ){
				NNS_G3dGlbOrtho( param1, param2, param3, param4, near, far );
			} else {
				NNS_G3dGlbOrthoW( param1, param2, param3, param4, near, far, scaleW );
			}
			break;
	}
}

void
	GFL_G3D_sysProjectionSetDirect
		( const MtxFx44* param )
{
	g3Dman->projection.type		= GFL_G3D_PRJMTX;
	g3Dman->projection.param1	= 0;
	g3Dman->projection.param2	= 0;
	g3Dman->projection.param3	= 0;
	g3Dman->projection.param4	= 0;
	g3Dman->projection.near		= 0;
	g3Dman->projection.far		= 0;

	NNS_G3dGlbSetProjectionMtx( param );
}

//--------------------------------------------------------------------------------------------
/**
 * ライトの設定
 *
 * @param	lightID			ライトＩＤ
 * @param	vec				ライトのベクトルポインタ
 * @param	color			色
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysLightSet
		( const u8 lightID, const VecFx16* vec, const u16 color )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->light[ lightID ].vec.x = vec->x;
	g3Dman->light[ lightID ].vec.y = vec->y;
	g3Dman->light[ lightID ].vec.z = vec->z;
	g3Dman->light[ lightID ].color = color;

	NNS_G3dGlbLightVector( lightID, vec->x, vec->y, vec->z );
	NNS_G3dGlbLightColor( lightID, color );
}

//--------------------------------------------------------------------------------------------
/**
 * カメラ行列の設定
 *
 * @param	camPos			カメラ位置ベクトルポインタ
 * @param	camUp			カメラの上方向のベクトルへのポインタ
 * @param	target			カメラ焦点へのポインタ
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysLookAtSet
		( const VecFx32* camPos, const VecFx32* camUp, const VecFx32* target )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->lookAt.camPos	= *camPos;
	g3Dman->lookAt.camUp	= *camUp;
	g3Dman->lookAt.target	= *target;
}

//--------------------------------------------------------------------------------------------
/**
 * レンダリングスワップバッファの設定
 *
 * @param	sortMode		ソートモード
 * @param	bufferMode		デプスバッファモード
 * 設定値についてはNitroSDK内GX_SwapBuffer関数を参照
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_sysSwapBufferModeSet
		( const GXSortMode sortMode, const GXBufferMode bufferMode )
{
	GF_ASSERT( g3Dman != NULL );

	g3Dman->swapBufMode.aw = sortMode;
	g3Dman->swapBufMode.zw = bufferMode;
}





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
#define G3DRES_MAGICNUM	(0x48BF)

enum {
	GFL_G3D_RES_TYPE_UNKNOWN = 0,	//不明なデータ
	GFL_G3D_RES_TYPE_MDLTEX,		//ファイル内にモデリングおよびテクスチャデータ内包
	GFL_G3D_RES_TYPE_MDL,			//ファイル内にモデリングデータ内包
	GFL_G3D_RES_TYPE_TEX,			//ファイル内にテクスチャデータ内包
	GFL_G3D_RES_TYPE_ANM,			//ファイル内にアニメーションデータ内包
};

///	３Ｄリソース構造体
struct _GFL_G3D_RES {
	u16		magicnum;
	u16		type;
	void* 	file;
};

static BOOL
	GFL_G3D_VramGetTexDataVramkey( NNSG3dResTex* res, NNSGfdTexKey* tex, NNSGfdTexKey* tex4x4 );
static BOOL
	GFL_G3D_VramGetTexPlttVramkey( NNSG3dResTex* res, NNSGfdPlttKey* pltt );

static inline BOOL G3DRES_FILE_CHECK( GFL_G3D_RES* g3Dres )
{
	if( g3Dres->magicnum == G3DRES_MAGICNUM ){
		return TRUE;
	}
	return FALSE;
}

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
static GFL_G3D_RES*
	GFL_G3D_ResCreate
		( NNSG3dResFileHeader* header )
{
	//リソース管理ハンドル作成
	GFL_G3D_RES* g3Dres = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_RES) );

	//OS_Printf("3D_resource check now...\n");
	//ファイルタイプの判別
	switch( *(u32*)&header[0] )
	{
		case NNS_G3D_SIGNATURE_NSBMD:
			//OS_Printf("nsbmd file check...\n");
			if( NNS_G3dGetTex( header ) == NULL ){
				//OS_Printf("this 3D_resource is model_data\n");
				g3Dres->type = GFL_G3D_RES_TYPE_MDL;	//モデリングデータ内包
			} else {
				//OS_Printf("this 3D_resource is model_data & texture_data\n");
				g3Dres->type = GFL_G3D_RES_TYPE_MDLTEX;	//モデリングおよびテクスチャデータ内包
			}
			break;
		case NNS_G3D_SIGNATURE_NSBTX:
			//OS_Printf("this 3D_resource is texture_data\n");
			g3Dres->type = GFL_G3D_RES_TYPE_TEX;		//テクスチャデータ内包
			break;
		case NNS_G3D_SIGNATURE_NSBCA:
		case NNS_G3D_SIGNATURE_NSBVA:
		case NNS_G3D_SIGNATURE_NSBMA:
		case NNS_G3D_SIGNATURE_NSBTP:
		case NNS_G3D_SIGNATURE_NSBTA:
			//OS_Printf("this 3D_resource is animetion_data\n");
			g3Dres->type = GFL_G3D_RES_TYPE_ANM;		//ファイル内にアニメーションデータ内包
			break;
		default:
			//OS_Printf("this 3D_resource is unknown\n");
			g3Dres->type = GFL_G3D_RES_TYPE_UNKNOWN;	//不明なデータ
			break;
	}
	g3Dres->magicnum = G3DRES_MAGICNUM;
	//ファイルポインタの設定
	g3Dres->file = ( void* )header;
	//OS_Printf("3D_resource is loaded\n");

	return g3Dres;
}

//-------------------------------
// アーカイブＩＤによる読み込み
GFL_G3D_RES*
	GFL_G3D_ResCreateArc
		( int arcID, int datID ) 
{
	NNSG3dResFileHeader* header;

	GF_ASSERT( g3Dman != NULL );

	//OS_Printf("3D_resource loading...\n");
	//対象アーカイブＩＮＤＥＸからヘッダデータを読み込み
	header = GFL_ARC_DataLoadMalloc( arcID, datID, g3Dman->heapID );

	return GFL_G3D_ResCreate( header );
}

//-------------------------------
// アーカイブファイルパスによる読み込み
GFL_G3D_RES*
	GFL_G3D_ResCreatePath
		( const char* path, int datID ) 
{
	NNSG3dResFileHeader* header;

	GF_ASSERT( g3Dman != NULL );

	//OS_Printf("3D_resource loading...\n");
	//対象アーカイブファイルからヘッダデータを読み込み
	header = GFL_ARC_DataLoadFilePathMalloc( path, datID, g3Dman->heapID );

	return GFL_G3D_ResCreate( header );
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄリソースの破棄
 *
 * @param	g3Dres	３Ｄリソースポインタ
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ResDelete
		( GFL_G3D_RES* g3Dres ) 
{
	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

	GFL_HEAP_FreeMemory( g3Dres->file );
	GFL_HEAP_FreeMemory( g3Dres );
}
	
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
NNSG3dResFileHeader*
	GFL_G3D_ResFileHeaderGet
		( GFL_G3D_RES* g3Dres ) 
{
	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

	return (NNSG3dResFileHeader*)g3Dres->file;
}
	
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
BOOL
	GFL_G3D_ResTypeCheck
		( GFL_G3D_RES* g3Dres, GFL_G3D_RES_CHKTYPE checkType ) 
{
	u16 resType;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );

	resType = g3Dres->type;

	switch( checkType ){
		case GFL_G3D_RES_CHKTYPE_UKN:
			if( resType == GFL_G3D_RES_TYPE_UNKNOWN ){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_MDL:
			if(( resType == GFL_G3D_RES_TYPE_MDLTEX )||( resType == GFL_G3D_RES_TYPE_MDL )){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_TEX:
			if(( resType == GFL_G3D_RES_TYPE_MDLTEX )||( resType == GFL_G3D_RES_TYPE_TEX )){
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case GFL_G3D_RES_CHKTYPE_ANM:
			if( resType == GFL_G3D_RES_TYPE_ANM ){
				return TRUE;
			} else {
				return FALSE;
			}
			break;
	}
	return FALSE;
}
	




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
BOOL 
	GFL_G3D_VramLoadTex
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//テクスチャリソースポインタの取得
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//リソースを転送するためのＶＲＡＭキーの取得
		if( GFL_G3D_VramGetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTex)\n");
			return FALSE;
		}
		if( GFL_G3D_VramGetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTex)\n");
			return FALSE;
		}
		//リソースへのＶＲＡＭキーの設定
		NNS_G3dTexSetTexKey( texture, texKey, tex4x4Key );
		NNS_G3dPlttSetPlttKey( texture, plttKey );

		//内部でＤＭＡ転送するので転送元をフラッシュする
		DC_FlushRange( header, header->fileSize );
		//キーを参照して、リソースをＶＲＡＭ転送
		NNS_G3dTexLoad( texture, TRUE );
		NNS_G3dPlttLoad( texture, TRUE );
		return TRUE;
	}
	return FALSE;
}

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
BOOL 
	GFL_G3D_VramLoadTexDataOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//テクスチャリソースポインタの取得
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//リソースを転送するためのＶＲＡＭキーの取得
		if( GFL_G3D_VramGetTexDataVramkey( texture, &texKey, &tex4x4Key ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadTexDataOnly)\n");
			return FALSE;
		}
		//リソースへのＶＲＡＭキーの設定
		NNS_G3dTexSetTexKey( texture, texKey, tex4x4Key );

		//内部でＤＭＡ転送するので転送元をフラッシュする
		DC_FlushRange( header, header->fileSize );
		//キーを参照して、リソースをＶＲＡＭ転送
		NNS_G3dTexLoad( texture, FALSE );
		return TRUE;
	}
	return FALSE;
}

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
BOOL 
	GFL_G3D_VramLoadTexPlttOnly
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//テクスチャリソースポインタの取得
	header = (NNSG3dResFileHeader*)g3Dres->file;
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
		//リソースを転送するためのＶＲＡＭキーの取得
		if( GFL_G3D_VramGetTexPlttVramkey( texture, &plttKey ) == FALSE ){
			OS_Printf("Vramkey cannot alloc (GFL_G3D_VramLoadPlttOnly)\n");
			return FALSE;
		}
		//リソースへのＶＲＡＭキーの設定
		NNS_G3dPlttSetPlttKey( texture, plttKey );

		//内部でＤＭＡ転送するので転送元をフラッシュする
		DC_FlushRange( header, header->fileSize );
		//キーを参照して、リソースをＶＲＡＭ転送
		NNS_G3dPlttLoad( texture, FALSE );
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * テクスチャリソースをＶＲＡＭから解放
 *
 * @param	g3Dres	３Ｄリソースポインタ
 *
 * @return	BOOL	結果(成功=TRUE)
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_VramUnloadTex
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
	NNSGfdTexKey			texKey, tex4x4Key;
	NNSGfdPlttKey			plttKey;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX ));

	//ＶＲＡＭ転送済みかどうか確認
	if( GFL_G3D_VramTexkeyLiveCheck( g3Dres ) == TRUE ){

		//テクスチャリソースポインタの取得
		header = (NNSG3dResFileHeader*)g3Dres->file;
		texture = NNS_G3dGetTex( header ); 

		//ＶＲＡＭキーの放棄フラグをリソースに設定
		NNS_G3dTexReleaseTexKey( texture, &texKey, &tex4x4Key );
		plttKey = NNS_G3dPlttReleasePlttKey( texture );

		if( NNS_GfdFreePlttVram( plttKey ) ){
			OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
		}
		if( NNS_GfdFreeLnkTexVram( tex4x4Key )){
			OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
		}
		if( NNS_GfdFreeLnkTexVram( texKey )){
			OS_Printf("Vramkey cannot free (GFL_G3D_VramUnloadTex)\n");
		}
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	テクスチャキーの取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 *	@param	tex		テクスチャキー保存ポインタ
 *	@param	tex4x4	4x4圧縮テクスチャ保存ポインタ
 *	@param	pltt	パレットキー保存ポインタ
 */
//-----------------------------------------------------------------------------
static BOOL
	GFL_G3D_VramGetTexDataVramkey
		( NNSG3dResTex* res, NNSGfdTexKey* tex, NNSGfdTexKey* tex4x4 )
{
	//各リソースサイズ取得
	u32 tex_size		= NNS_G3dTexGetRequiredSize( res );
	u32 tex_4x4_size	= NNS_G3dTex4x4GetRequiredSize( res );

	//キー取得(ＶＲＡＭ確保)
	if( tex_size != 0 ){
		*tex = NNS_GfdAllocTexVram( tex_size, FALSE, 0 );
		if( *tex == NNS_GFD_ALLOC_ERROR_TEXKEY ){
			return FALSE;
		}
	}
	if( tex_4x4_size != 0 ){
		*tex4x4 = NNS_GfdAllocTexVram( tex_size, TRUE, 0 );
		if( *tex4x4 == NNS_GFD_ALLOC_ERROR_TEXKEY ){
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *
 *@brief	パレットキーの取得
 *
 *	@param	res		テクスチャリソース参照ポインタ
 *	@param	pltt	パレットキー保存ポインタ
 */
//-----------------------------------------------------------------------------
static BOOL
	GFL_G3D_VramGetTexPlttVramkey
		( NNSG3dResTex* res, NNSGfdPlttKey* pltt )
{
	//リソースサイズ取得
	u32 pltt_size		= NNS_G3dPlttGetRequiredSize( res );

	//キー取得(ＶＲＡＭ確保)
	if( pltt_size != 0 ){
		BOOL b4Pltt = res->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4;
		*pltt = NNS_GfdAllocPlttVram( pltt_size, b4Pltt, 0 );
		if( *pltt == NNS_GFD_ALLOC_ERROR_PLTTKEY ){
			return FALSE;
		}
	}
	return TRUE;
}

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
BOOL
	GFL_G3D_VramTexkeyLiveCheck
		( GFL_G3D_RES* g3Dres )
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			restex;

	GF_ASSERT( g3Dres->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT(( g3Dres->type==GFL_G3D_RES_TYPE_MDLTEX )||( g3Dres->type==GFL_G3D_RES_TYPE_TEX )); 

	//テクスチャリソースポインタの取得
	header = (NNSG3dResFileHeader*)g3Dres->file;
	restex = NNS_G3dGetTex( header ); 

	// 普通のテクスチャか4*4テクセル圧縮テクスチャ
	// でVramに展開されているかをチェック
	if(( restex->texInfo.flag & NNS_G3D_RESTEX_LOADED ) ||
	   ( restex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED )||
	   ( restex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED )){
		return TRUE;
	}
	return FALSE;
}





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
#define G3DRND_MAGICNUM	(0x7A14)
///	３Ｄレンダー制御用構造体
struct _GFL_G3D_RND
{
	u16						magicnum;
	u16						dummy;

	NNSG3dRenderObj*		rndobj;
	GFL_G3D_RES*			mdl;
	GFL_G3D_RES*			tex;
};


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
GFL_G3D_RND*
	GFL_G3D_RndCreate
		( GFL_G3D_RES* mdl, int mdlidx, GFL_G3D_RES* tex )
{
	NNSG3dResMdlSet*		pMdlset;
	NNSG3dResMdl*			pMdl = NULL;
	NNSG3dResTex*			pTex = NULL;
	GFL_G3D_RND*			g3Drnd;

	//モデルデータリソースポインタ取得
	GF_ASSERT( mdl->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT( GFL_G3D_ResTypeCheck( mdl, GFL_G3D_RES_CHKTYPE_MDL ) == TRUE );
	pMdlset = NNS_G3dGetMdlSet( (NNSG3dResFileHeader*)mdl->file );
	pMdl = NNS_G3dGetMdlByIdx( pMdlset, mdlidx );
	GF_ASSERT( pMdl != NULL );

	//テクスチャリソースポインタ取得
	if( tex != NULL ){	//テクスチャーなしを指定することもできる
		GF_ASSERT( tex->magicnum == G3DRES_MAGICNUM );
		GF_ASSERT( GFL_G3D_ResTypeCheck( tex, GFL_G3D_RES_CHKTYPE_TEX ) == TRUE );
		pTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)tex->file );
		GF_ASSERT( pTex != NULL );
	}

	//レンダーハンドルの作成
	g3Drnd = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_RND) );
	g3Drnd->magicnum = G3DRND_MAGICNUM;
	g3Drnd->mdl = mdl;
	g3Drnd->tex = tex;

	//レンダリングオブジェクト領域の確保
	g3Drnd->rndobj = NNS_G3dAllocRenderObj( &g3Dman->allocater );
	GF_ASSERT( g3Drnd->rndobj != NULL );

	//テクスチャリソースとの関連付け
	if( pTex ){
		BOOL result = TRUE;
		result &= NNS_G3dBindMdlTex( pMdl, pTex );
		result &= NNS_G3dBindMdlPltt( pMdl, pTex );
		if( result == FALSE ){
			OS_Printf("texture bind failed\n");
		}
	}
	//レンダリングオブジェクト初期化
	NNS_G3dRenderObjInit( g3Drnd->rndobj, pMdl );

	return g3Drnd;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーの破棄
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_RndDelete
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	NNS_G3dFreeRenderObj( &g3Dman->allocater, g3Drnd->rndobj );
	GFL_HEAP_FreeMemory( g3Drnd );
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーのモデルリソースポインタを取得
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_RndG3DresMdlGet
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->mdl;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄレンダーのモデルリソースポインタを取得
 *
 * @param	g3Drnd	３Ｄレンダーハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_RndG3DresTexGet
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->tex;
}

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
NNSG3dRenderObj*
	GFL_G3D_RndRenderObjGet
		( GFL_G3D_RND* g3Drnd ) 
{
	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	return g3Drnd->rndobj;
}





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
#define G3DANM_MAGICNUM	(0x59d1)
///	３Ｄアニメーション制御用構造体
struct _GFL_G3D_ANM
{
	u16				magicnum;
	u16				dummy;

	NNSG3dAnmObj*	anmobj;
	GFL_G3D_RES*	anm;
};

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
GFL_G3D_ANM*
	GFL_G3D_AnmCreate
		( GFL_G3D_RND* g3Drnd, GFL_G3D_RES* anm, int anmidx )  
{
	GFL_G3D_ANM*	g3Danm;
	NNSG3dResMdl*	pMdl;
	NNSG3dResTex*	pTex;
	void*			pAnm;

	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );
	GF_ASSERT( anm->magicnum == G3DRES_MAGICNUM );
	GF_ASSERT( anm->type == GFL_G3D_RES_TYPE_ANM );

	//アニメーションハンドルの作成
	g3Danm = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_ANM) );
	g3Danm->magicnum = G3DANM_MAGICNUM;
	g3Danm->anm = anm;

	//アニメーションリソースポインタ取得
	pAnm = NNS_G3dGetAnmByIdx( anm->file, anmidx );
	GF_ASSERT( pAnm != NULL );

	pMdl = NNS_G3dRenderObjGetResMdl( g3Drnd->rndobj );
	pTex = NNS_G3dGetTex( (NNSG3dResFileHeader*)g3Drnd->tex->file );

	//アニメーションオブジェクト領域の確保
	g3Danm->anmobj = NNS_G3dAllocAnmObj( &g3Dman->allocater, pAnm, pMdl );
	GF_ASSERT( g3Danm->anmobj != NULL );
	//アニメーションオブジェクト初期化
	NNS_G3dAnmObjInit( g3Danm->anmobj, pAnm, pMdl, pTex );

	return g3Danm;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションの破棄
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_AnmDelete
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	NNS_G3dFreeAnmObj( &g3Dman->allocater, g3Danm->anmobj );
	GFL_HEAP_FreeMemory( g3Danm );
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄアニメーションリソースポインタを取得
 *
 * @param	g3Danm	３Ｄアニメーションハンドル
 *
 * @return	GFL_G3D_RES*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_AnmG3DresGet
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	return g3Danm->anm;
}

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
NNSG3dAnmObj*
	GFL_G3D_AnmAnmObjGet
		( GFL_G3D_ANM* g3Danm ) 
{
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	return g3Danm->anmobj;
}





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
 * 　またアニメーションは複数登録できるように構成されている。（なしも可能）
 * 　　　例）テクスチャアニメーション＋ジョイントアニメーション
 *　 そのためハンドルサイズは不定になっていることに注意すること。
 *
 */
//=============================================================================================
#define G3DOBJ_MAGICNUM	(0x2185)
///	３Ｄオブジェクト制御用構造体
struct _GFL_G3D_OBJ
{
	u16				magicnum;

	GFL_G3D_RND*	g3Drnd;
	GFL_G3D_ANM**	anmTbl;
	u16				anmCount;
};

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
GFL_G3D_OBJ*
	GFL_G3D_ObjCreate
		( GFL_G3D_RND* g3Drnd, GFL_G3D_ANM** anmTbl, u16 anmCount )  
{
	GFL_G3D_OBJ*	g3Dobj;
	GFL_G3D_ANM*	g3Danm;
	int	i;

	GF_ASSERT( g3Drnd->magicnum == G3DRND_MAGICNUM );

	//オブジェクトハンドルの作成
	g3Dobj = GFL_HEAP_AllocMemory( g3Dman->heapID, sizeof(GFL_G3D_OBJ) );
	g3Dobj->magicnum	= G3DOBJ_MAGICNUM;
	g3Dobj->g3Drnd		= g3Drnd;
	g3Dobj->anmCount	= anmCount;

	//アニメーション配列作成
	g3Dobj->anmTbl = GFL_HEAP_AllocMemoryClear( g3Dman->heapID, sizeof(GFL_G3D_ANM*) * anmCount );
	for( i=0; i<anmCount; i++ ){
		g3Danm = anmTbl[i];
		if( g3Danm != NULL ){
			GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

			NNS_G3dRenderObjAddAnmObj( g3Drnd->rndobj, g3Danm->anmobj );
		}
		g3Dobj->anmTbl[i] = g3Danm;
	}

	return g3Dobj;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの破棄
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjDelete
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	GFL_HEAP_FreeMemory( g3Dobj->anmTbl );
	GFL_HEAP_FreeMemory( g3Dobj );
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのレンダーハンドルポインタを取得
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 *
 * @return	GFL_G3D_RND*
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RND*
	GFL_G3D_ObjG3DrndGet
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	return g3Dobj->g3Drnd;
}

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
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	return g3Dobj->anmTbl[ anmIdx ];
}

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
		( GFL_G3D_OBJ* g3Dobj ) 
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );

	return g3Dobj->anmCount;
}

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
u16
	GFL_G3D_ObjAnmAdd
		( GFL_G3D_OBJ* g3Dobj, GFL_G3D_ANM* g3Danm )
{
	u16	i;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( g3Danm->magicnum == G3DANM_MAGICNUM );

	for( i=0; i<g3Dobj->anmCount; i++ ){
		if( g3Dobj->anmTbl[i] == NULL ){
			//レンダリングオブジェクトとの関連付け
			NNS_G3dRenderObjAddAnmObj( g3Dobj->g3Drnd->rndobj, g3Danm->anmobj );
			g3Dobj->anmTbl[ i ] = g3Danm;
			return i;
		}
	}
	GF_ASSERT(0);
	return 0xffff;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトからアニメーションを削除する
 *
 * @param	g3Dobj		３Ｄオブジェクトハンドル
 * @param	anmIdx		登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjAnmRemove
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx )
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	//レンダリングオブジェクトとの関連付けを解除
	NNS_G3dRenderObjRemoveAnmObj( g3Dobj->g3Drnd->rndobj, g3Dobj->anmTbl[ anmIdx ]->anmobj );
	g3Dobj->anmTbl[ anmIdx ] = NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトのアニメーションフレームをリセット
 *
 * @param	g3Dobj	３Ｄオブジェクトハンドル
 * @param	anmIdx	登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ObjContAnmFrameReset
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx )
{
	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	g3Dobj->anmTbl[ anmIdx ]->anmobj->frame = 0;
}

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
BOOL
	GFL_G3D_ObjContAnmFrameInc
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count ) 
{
	NNSG3dAnmObj* anmobj;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	anmobj = g3Dobj->anmTbl[ anmIdx ]->anmobj;
	anmobj->frame += count;
	
	if( anmobj->frame >= NNS_G3dAnmObjGetNumFrame( anmobj )){
		return FALSE;
	}
	return TRUE;
}

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
BOOL
	GFL_G3D_ObjContAnmFrameAutoLoop
		( GFL_G3D_OBJ* g3Dobj, u16 anmIdx, const fx32 count ) 
{
	NNSG3dAnmObj* anmobj;

	GF_ASSERT( g3Dobj->magicnum == G3DOBJ_MAGICNUM );
	GF_ASSERT( anmIdx < g3Dobj->anmCount );

	anmobj = g3Dobj->anmTbl[ anmIdx ]->anmobj;
	anmobj->frame += count;
	
	if( anmobj->frame >= NNS_G3dAnmObjGetNumFrame( anmobj )){
		anmobj->frame = 0;
		return FALSE;
	}
	return TRUE;
}





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
void
	GFL_G3D_DrawStart
		( void )
{
	G3X_Reset();
}

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄ描画の終了
 *
 * 全体描画関数内、描画を終了時に呼び出される
 * 内部でレンダリングバッファのスワップを行う。
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DrawEnd
		( void )
{
	G3_SwapBuffers( g3Dman->swapBufMode.aw, g3Dman->swapBufMode.zw );
}

//--------------------------------------------------------------------------------------------
/**
 * カメラグローバルステートの設定
 *
 * 全体描画関数内に呼び出される
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_DrawLookAt
		( void )
{
	NNS_G3dGlbLookAt( &g3Dman->lookAt.camPos, &g3Dman->lookAt.camUp, &g3Dman->lookAt.target );
}

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
// ３Ｄオブジェクトの情報設定
static inline void 
	statusSet( const VecFx32* pTrans, const MtxFx33* pRotate, const VecFx32* pScale )
{
	// 位置設定
	NNS_G3dGlbSetBaseTrans( pTrans );
	// 角度設定
	NNS_G3dGlbSetBaseRot( pRotate );
	// スケール設定
	NNS_G3dGlbSetBaseScale( pScale );
}

//--------------------------------------------------------------------------------
// 通常描画
//--------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status )
{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlush();
	NNS_G3dDraw( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawVP
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status )
{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlushVP();
	NNS_G3dDraw( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawWVP
		( GFL_G3D_OBJ* g3Dobj, const GFL_G3D_OBJSTATUS* status )
{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlushWVP();
	NNS_G3dDraw( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) );
	NNS_G3dGeFlushBuffer();
}

//--------------------------------------------------------------------------------
// １つのモデルに１つのマテリアルのみ設定されているときに高速描画するための関数
//		matID	描画するマテリアルへのインデックス 
//		shpID	描画するシェイプへのインデックス 
//		sendMat	マテリアル情報をジオメトリエンジンに送信するかどうか 
//--------------------------------------------------------------------------------
void
	GFL_G3D_ObjDraw1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat, 
			const GFL_G3D_OBJSTATUS* status )

{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlush();
	NNS_G3dDraw1Mat1Shp( NNS_G3dRenderObjGetResMdl
							( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) ), 
								matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat,
			const GFL_G3D_OBJSTATUS* status )
{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlushVP();
	NNS_G3dDraw1Mat1Shp( NNS_G3dRenderObjGetResMdl
							( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) ), 
								matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}

void
	GFL_G3D_ObjDrawWVP1mat1shp
		( GFL_G3D_OBJ* g3Dobj, u32 matID, u32 shpID, BOOL sendMat,
			const GFL_G3D_OBJSTATUS* status )
{
	statusSet( &status->trans, &status->rotate, &status->scale );
	NNS_G3dGlbFlushWVP();
	NNS_G3dDraw1Mat1Shp( NNS_G3dRenderObjGetResMdl
							( GFL_G3D_RndRenderObjGet( GFL_G3D_ObjG3DrndGet( g3Dobj ) ) ), 
								matID, shpID, sendMat );
	NNS_G3dGeFlushBuffer();
}





//=============================================================================================
/**
 *
 *
 * データ
 *
 *
 */
//=============================================================================================



