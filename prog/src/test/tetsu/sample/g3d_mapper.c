//============================================================================================
/**
 * @file	g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "include\system\gfl_use.h"	//乱数用

#include "g3d_mapper.h"

//============================================================================================
/**
 *
 *
 *
 * @brief	３Ｄマップコントロール
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	型宣言
 */
//------------------------------------------------------------------
#define MAP_BLOCK_COUNT		(9)
#define MAPID_NULL			(0xffffffff)
#define MAPARC_NULL			(0xffffffff)
#define LOCAL_OBJID_NULL	(0)
#define DDRAW_OBJID_NULL	(0)

#define COMMON_OBJ_RESCOUNT	(64)
#define LOCAL_OBJRES_COUNT	(16)
#define LOCAL_OBJRND_COUNT	(32)
#define LOCAL_DIRECTDRAW_OBJ_COUNT	(64)

//#define MAP_BLOCK_LEN		(FX32_ONE * 256 * 2 )

typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCK_IDX;

typedef struct {
	NNSG3dRenderObj*	NNSrnd;	//レンダー
	u32					id;
	VecFx32				trans;
}MAP_OBJECT_DATA;

typedef struct {
	u32			id;
	VecFx32		trans;
}DIRECT_OBJ;

typedef struct {
	u32				idx;
	VecFx32			trans;
	ARCHANDLE*		arc;

	u32				loadSeq;
	u32				loadCount;

	NNSG3dRenderObj*	NNSrnd;	//地形レンダー

	BOOL			dataLoadReq;
	u32				dataID;
	GFL_G3D_RES*	g3DgroundResMdl;	//地形モデルリソース
	u8				data[MAPDATA_SIZE];

	BOOL			texLoadReq;
	u32				texID;
	GFL_G3D_RES*	g3DgroundResTex;	//ローカル地形テクスチャリソース
	u8				tex[MAPTEX_SIZE];

//	BOOL			attrLoadReq;
//	u32				attrID;
//	u8				attr[MAPATTR_SIZE];

	NNSGfdTexKey	groundTexKey;
	NNSGfdPlttKey	groundPlttKey;

	MAP_OBJECT_DATA	object[LOCAL_OBJRND_COUNT];	//配置オブジェクト

	BOOL			objLoadReq;
	u32				objID;
	GFL_G3D_RES*	g3DobjectResMdl[LOCAL_OBJRES_COUNT];//ローカルオブジェクトモデルリソース
	u8				obj[MAPOBJ_SIZE];

	NNSGfdTexKey	objectTexKey;
	NNSGfdPlttKey	objectPlttKey;

	DIRECT_OBJ		directDrawObject[LOCAL_DIRECTDRAW_OBJ_COUNT];
}MAP_BLOCK_DATA;

struct _G3D_MAPPER {
	HEAPID				heapID;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];

	BLOCK_IDX			mapBlockIdx[MAP_BLOCK_COUNT];
	u32					nowBlockIdx;				
	VecFx32				posCont;
	u16					sizex;	//横ブロック数
	u16					sizez;	//縦ブロック数
	fx32				width;	//ブロック１辺の幅
	u32					arcID;	//グラフィックアーカイブＩＤ
	const G3D_MAPPER_DATA*	data;	//実マップデータ

	GFL_G3D_RES*		objectG3DresMdl[COMMON_OBJ_RESCOUNT];	//共通オブジェクトモデルリソース
	u32					objectG3DresCount;
	//-----------------
	GFL_G3D_RES*	tree_g3Dres;
	u32				tree_texDataAdrs;
	u32				tree_texPlttAdrs;
	//-----------------
};

static void	Load3Dmap( G3D_MAPPER* g3Dmapper );

static void	CreateMapGraphicResource( G3D_MAPPER* g3Dmapper );
static void	DeleteMapGraphicResource( G3D_MAPPER* g3Dmapper );

static void GetMapperBlockIndex( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static BOOL	ReloadMapperBlock( G3D_MAPPER* g3Dmapper, BLOCK_IDX* new );

static BOOL	GetMapBlockInMap( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockIdx, fx32* inBlockx, fx32* inBlockz ); 
static BOOL	GetMapBlockInBuffer( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockIdx, fx32* inBlockx, fx32* inBlockz );
static void	GetMapGrid( G3D_MAPPER* g3Dmapper, const fx32 inBlockx, const fx32 inBlockz,
						u32* gridIdx, fx32* inGridx, fx32* inGridz );
static u32 GetMapTriangleID( G3D_MAPPER* g3Dmapper, 
					const fx32 inGridx, const fx32 inGridz, const u32 gridTryPattern );
static void	DirectDraw3Dmapper
	( G3D_MAPPER* g3Dmapper, MAP_BLOCK_DATA* mapBlock, GFL_G3D_CAMERA* g3Dcamera );
//------------------------------------------------------------------
/**
 * @brief	セットアップ
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステム作成
 */
//------------------------------------------------------------------
G3D_MAPPER*	Create3Dmapper( HEAPID heapID )
{
	G3D_MAPPER* g3Dmapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(G3D_MAPPER) );
	int i;

	g3Dmapper->heapID = heapID;

	CreateMapGraphicResource( g3Dmapper );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		g3Dmapper->mapBlockIdx[i].blockIdx = MAPID_NULL;
	}
	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->width = 0;
	g3Dmapper->arcID = MAPARC_NULL;
	g3Dmapper->data = NULL;
	
	g3Dmapper->objectG3DresCount = 0;
	for( i=0; i<COMMON_OBJ_RESCOUNT; i++ ){
		g3Dmapper->objectG3DresMdl[i] = NULL;
	}

	//---------------------
	{
		NNSG3dTexKey	texDataKey;
		NNSG3dPlttKey	texPlttKey;
		u32				arcID = 3;	//ARCID_SAMPLEMAP
		u32				datID = 48;	//NARC_sample_map_sample_tree_nsbtx

		GFL_G3D_RES* g3DresTex = GFL_G3D_CreateResourceArc( arcID, datID ); 

		g3Dmapper->tree_g3Dres = g3DresTex;

		if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
			GF_ASSERT(0);
		}
		texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
		texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

		g3Dmapper->tree_texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
		g3Dmapper->tree_texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );

		//OS_Printf("tree_texDataAdrs = %x, tree_texPlttAdrs = %x\n",
		//			g3Dmapper->tree_texDataAdrs, g3Dmapper->tree_texPlttAdrs );
	}	
	//---------------------
	OS_Printf("g3DmapperSize = %x\n", sizeof(G3D_MAPPER));
	OS_Printf("mapBlockDataSize = %x\n", sizeof(MAP_BLOCK_DATA));
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムメイン
 */
//------------------------------------------------------------------
void	Main3Dmapper( G3D_MAPPER* g3Dmapper )
{
	BLOCK_IDX nowBlockIdx[MAP_BLOCK_COUNT];

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->data == NULL ){
		return;
	}
 	GetMapperBlockIndex( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
	if( ReloadMapperBlock( g3Dmapper, &nowBlockIdx[0] ) == TRUE ){
#if 0
		BLOCK_IDX*	p = g3Dmapper->mapBlockIdx;
		OS_Printf("---------------\n" );
		OS_Printf("%x, %x, %x \n", p[0].blockIdx,p[1].blockIdx,p[2].blockIdx );
		OS_Printf("%x, %x, %x \n", p[3].blockIdx,p[4].blockIdx,p[5].blockIdx );
		OS_Printf("%x, %x, %x \n", p[6].blockIdx,p[7].blockIdx,p[8].blockIdx );
#endif
	}
	Load3Dmap( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムディスプレイ
 */
static BOOL CheckCullingBoxTest( NNSG3dRenderObj* rnd );
static BOOL CheckCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos );
//------------------------------------------------------------------
void	Draw3Dmapper( G3D_MAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	MAP_BLOCK_DATA* mapBlock;
	BOOL			cullResult; 
	VecFx32			scale = { FX32_ONE, FX32_ONE, FX32_ONE };
	MtxFx33			rotate = { FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE };
	int				i, j, count;

	//描画開始
	GFL_G3D_DRAW_Start();
	//カメラグローバルステート設定		
 	GFL_G3D_DRAW_SetLookAt();

	count = 0;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];
		if( NNS_G3dRenderObjGetResMdl( mapBlock->NNSrnd ) != NULL ){
			
			NNS_G3dGlbSetBaseTrans( &mapBlock->trans );	// 位置設定
			NNS_G3dGlbSetBaseRot( &rotate );	// 角度設定
			NNS_G3dGlbSetBaseScale( &scale );	// スケール設定
			NNS_G3dGlbFlush();					//グローバルステート反映

			if( CheckCullingBoxTest( mapBlock->NNSrnd ) == TRUE ){
				//地形描画
				NNS_G3dDraw( mapBlock->NNSrnd );	

				//配置オブジェクト描画
				for( j=0; j<LOCAL_OBJRND_COUNT; j++ ){
					if(	mapBlock->object[j].id != LOCAL_OBJID_NULL ){
						if( NNS_G3dRenderObjGetResMdl( mapBlock->object[j].NNSrnd ) != NULL){
							VecFx32* transp = &mapBlock->object[j].trans;	// 位置設定
						
							NNS_G3dGlbSetBaseTrans( transp );		// 位置設定
							NNS_G3dGlbSetBaseRot( &rotate );		// 角度設定
							NNS_G3dGlbSetBaseScale( &scale );		// スケール設定
							NNS_G3dGlbFlush();						//グローバルステート反映

							if( ( CheckCullingBoxTest(mapBlock->object[j].NNSrnd) == TRUE )
							  &&( CheckCullingLength(g3Dcamera, transp) == TRUE ) ){
								//オブジェクト描画
								count++;
								NNS_G3dDraw( mapBlock->object[j].NNSrnd );
							}
						}
					}
				}
			}
			DirectDraw3Dmapper( g3Dmapper, mapBlock, g3Dcamera );
		}
	}
	OS_Printf("objectDrawCount = %x\n",count);
	//描画終了（バッファスワップ）
	//GFL_G3D_DRAW_End();							
}

//--------------------------------------------------------------------------------------------
//ボックス視体積内外テストによるカリングチェック
//（オブジェクトのグローバルステータス反映後に呼ぶこと）
static s32 TestBoundingBox( const GXBoxTestParam* boundingBox );

static BOOL CheckCullingBoxTest( NNSG3dRenderObj* rnd )
{
	NNSG3dResMdlInfo*	modelInfo;		// モデルデータからのボックステスト用パラメータ取得用
	BOOL				result = TRUE;
	GXBoxTestParam		boundingBox;

	//ボックステスト用パラメータ取得
	modelInfo = NNS_G3dGetMdlInfo( NNS_G3dRenderObjGetResMdl(rnd) );

	//バウンディングボックス作成
	boundingBox.x		= modelInfo->boxX;
	boundingBox.y		= modelInfo->boxY;
	boundingBox.z		= modelInfo->boxZ;
	boundingBox.width	= modelInfo->boxW;
	boundingBox.height	= modelInfo->boxH;
	boundingBox.depth	= modelInfo->boxD;

	//上のボックステストの箱の値をposScaleでかけることにより本当の値になる
	NNS_G3dGePushMtx();
	NNS_G3dGeScale( modelInfo->boxPosScale, modelInfo->boxPosScale, modelInfo->boxPosScale );
	
	//チェック
	if( !TestBoundingBox( &boundingBox ) ){
		result = FALSE;
	}
	NNS_G3dGePopMtx(1);

	return result;
}

static s32 TestBoundingBox( const GXBoxTestParam* boundingBox )
{
	s32 testResult = 1;

	//ポリゴン属性の「FAR面交差クリッピングフラグ」と「１ドットポリゴン表示フラグ」を1に設定
	//※ライブラリからの要求
	NNS_G3dGePolygonAttr( GX_LIGHTMASK_0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE,           
					0, 0, GX_POLYGON_ATTR_MISC_FAR_CLIPPING | GX_POLYGON_ATTR_MISC_DISP_1DOT );

	//ポリゴンアトリビュート反映 ※ライブラリ指定手順
	NNS_G3dGeBegin( GX_BEGIN_TRIANGLES );							
	NNS_G3dGeEnd();	

	NNS_G3dGeBoxTest( boundingBox );	// ボックステスト
	NNS_G3dGeFlushBuffer();				// 同期を取る

	//チェック本体（返り値が0の場合テスト終了）。 0:視体積内、それ以外の値は視体積外
	while ( G3X_GetBoxTestResult( &testResult ) != 0 ) ;

	return testResult;
}

//--------------------------------------------------------------------------------------------
// カメラとの距離によるカリングチェック
static BOOL CheckCullingLength( GFL_G3D_CAMERA* g3Dcamera, VecFx32* pos )
{
	VecFx32 camPos, vecLength;
	fx32 limitLength;

	GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
	GFL_G3D_CAMERA_GetFar( g3Dcamera, &limitLength );

	VEC_Subtract( pos, &camPos, &vecLength );
	if( VEC_Mag( &vecLength ) < limitLength ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステム破棄
 */
//------------------------------------------------------------------
void	Delete3Dmapper( G3D_MAPPER* g3Dmapper )
{
	GF_ASSERT( g3Dmapper );

	//---------------------
	GFL_G3D_FreeVramTexture( g3Dmapper->tree_g3Dres );
	GFL_G3D_DeleteResource( g3Dmapper->tree_g3Dres );
	//---------------------
	ReleaseObjRes3Dmapper( g3Dmapper );	//登録されたままの場合を想定して削除
	DeleteMapGraphicResource( g3Dmapper );

	GFL_HEAP_FreeMemory( g3Dmapper );
}


//------------------------------------------------------------------
/**
 * @brief	マップデータセット
 */
//------------------------------------------------------------------
void ResistData3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData )
{
	MAP_BLOCK_DATA*	mapBlock;
	int i, j;

	GF_ASSERT( g3Dmapper );

	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->width = resistData->width;
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->data = resistData->data;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		//読み込みステータスリセット
		mapBlock->loadSeq = 0;
		mapBlock->loadCount = 0;
		mapBlock->dataLoadReq = FALSE;
		mapBlock->dataID = 0;
		mapBlock->texLoadReq = FALSE;
		mapBlock->texID = 0;
//		mapBlock->attrLoadReq = FALSE;
//		mapBlock->attrID = 0;
		mapBlock->objLoadReq = FALSE;
		mapBlock->objID = 0;

		//アーカイブハンドル作成
		if( mapBlock->arc != NULL ){
			GFL_ARC_CloseDataHandle( mapBlock->arc );
		}
		mapBlock->NNSrnd->resMdl = NULL; 
		mapBlock->arc = GFL_ARC_OpenDataHandle( g3Dmapper->arcID, g3Dmapper->heapID );
		GF_ASSERT( mapBlock->arc );

		for( j=0; j<LOCAL_OBJRND_COUNT; j++ ){
			mapBlock->object[j].id = LOCAL_OBJID_NULL;
			VEC_Set( &mapBlock->object[j].trans, 0, 0, 0 );
		}
		for( j=0; j<LOCAL_DIRECTDRAW_OBJ_COUNT; j++ ){
			mapBlock->directDrawObject[j].id = DDRAW_OBJID_NULL;
			VEC_Set( &mapBlock->directDrawObject[j].trans, 0, 0, 0 );
		}
	}
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		g3Dmapper->mapBlockIdx[i].blockIdx = MAPID_NULL;
	}
}

//------------------------------------------------------------------
/**
 * @brief	オブジェクトリソースセット
 */
//------------------------------------------------------------------
void ResistObjRes3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPEROBJ_RESIST* resistData )
{
	int i;

	GF_ASSERT( resistData->count <= COMMON_OBJ_RESCOUNT );
	
	ReleaseObjRes3Dmapper( g3Dmapper );

	g3Dmapper->objectG3DresCount = resistData->count;
	for( i=0; i< g3Dmapper->objectG3DresCount; i++ ){
		g3Dmapper->objectG3DresMdl[i] = 
			GFL_G3D_CreateResourceArc( resistData->arcID, resistData->mdlID[i] );
		GFL_G3D_TransVramTexture( g3Dmapper->objectG3DresMdl[i] ); 
	}
}

void ReleaseObjRes3Dmapper( G3D_MAPPER* g3Dmapper )
{
	int i;

	if( g3Dmapper->objectG3DresCount ){
		for( i=0; i<g3Dmapper->objectG3DresCount; i++ ){
			if( g3Dmapper->objectG3DresMdl[i] != NULL ){
				GFL_G3D_FreeVramTexture( g3Dmapper->objectG3DresMdl[i] ); 
				GFL_G3D_DeleteResource( g3Dmapper->objectG3DresMdl[i] ); 
				g3Dmapper->objectG3DresMdl[i] = NULL;
			}
		}
		g3Dmapper->objectG3DresCount = 0;
	}
}

//------------------------------------------------------------------
/**
 * @brief	マップ位置セット
 */
//------------------------------------------------------------------
void SetPos3Dmapper( G3D_MAPPER* g3Dmapper, const VecFx32* pos )
{
	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップデータセットアップ
 */
//------------------------------------------------------------------
static void	CreateMapGraphicResource( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i, j;
	u32				g3DresHeaderSize = GFL_G3D_GetResourceHeaderSize();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		//初期化
		mapBlock->idx = MAPID_NULL;
		VEC_Set( &mapBlock->trans, 0, 0, 0 );
		mapBlock->loadSeq = 0;
		mapBlock->loadCount = 0;

		mapBlock->dataLoadReq = FALSE;
		mapBlock->dataID = 0;
		mapBlock->texLoadReq = FALSE;
		mapBlock->texID = 0;
//		mapBlock->attrLoadReq = FALSE;
//		mapBlock->attrID = 0;

		mapBlock->NNSrnd = NNS_G3dAllocRenderObj( GFL_G3D_GetAllocaterPointer() );
		mapBlock->NNSrnd->resMdl = NULL; 

		//アーカイブハンドル作成
		//mapBlock->arc = GFL_ARC_OpenDataHandle( g3Dmapper->arcID, g3Dmapper->heapID );
		//GF_ASSERT( mapBlock->arc );
		mapBlock->arc = NULL;
		//モデルリソースヘッダ作成
		mapBlock->g3DgroundResMdl = GFL_HEAP_AllocClearMemory(g3Dmapper->heapID,g3DresHeaderSize);
		//テクスチャリソースヘッダ作成
		mapBlock->g3DgroundResTex = GFL_HEAP_AllocClearMemory(g3Dmapper->heapID,g3DresHeaderSize);

		//モデルリソースハンドル内容設定
		GFL_G3D_CreateResource( mapBlock->g3DgroundResMdl, 
								GFL_G3D_RES_CHKTYPE_MDL,(void*)&mapBlock->data[0] );
		//テクスチャリソースハンドル内容設定
		GFL_G3D_CreateResource( mapBlock->g3DgroundResTex, 
								GFL_G3D_RES_CHKTYPE_TEX, (void*)&mapBlock->tex[0] );

		//テクスチャＶＲＡＭ確保
		mapBlock->groundTexKey = NNS_GfdAllocTexVram( MAPTEX_SIZE, FALSE, 0 );
		//パレットＶＲＡＭ確保
		mapBlock->groundPlttKey = NNS_GfdAllocPlttVram( MAPPLTT_SIZE, FALSE, 0 );

		GF_ASSERT( mapBlock->groundTexKey != NNS_GFD_ALLOC_ERROR_TEXKEY );
		GF_ASSERT( mapBlock->groundPlttKey != NNS_GFD_ALLOC_ERROR_PLTTKEY );

		for( j=0; j<LOCAL_OBJRND_COUNT; j++ ){
			mapBlock->object[j].id = LOCAL_OBJID_NULL;
			mapBlock->object[j].NNSrnd = NNS_G3dAllocRenderObj( GFL_G3D_GetAllocaterPointer() );
			VEC_Set( &mapBlock->object[j].trans, 0, 0, 0 );
		}
		for( j=0; j<LOCAL_DIRECTDRAW_OBJ_COUNT; j++ ){
			mapBlock->directDrawObject[j].id = DDRAW_OBJID_NULL;
			VEC_Set( &mapBlock->directDrawObject[j].trans, 0, 0, 0 );
		}
	}
}

//------------------------------------------------------------------
static void	DeleteMapGraphicResource( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i, j;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		for( j=0; j<LOCAL_OBJRND_COUNT; j++ ){
			NNS_G3dFreeRenderObj( GFL_G3D_GetAllocaterPointer(), mapBlock->object[j].NNSrnd );
		}
		NNS_G3dFreeRenderObj( GFL_G3D_GetAllocaterPointer(), mapBlock->NNSrnd );

		NNS_GfdFreePlttVram( mapBlock->groundPlttKey );
		NNS_GfdFreeTexVram( mapBlock->groundTexKey );

		GFL_HEAP_FreeMemory( mapBlock->g3DgroundResTex );
		GFL_HEAP_FreeMemory( mapBlock->g3DgroundResMdl );
		if( mapBlock->arc != NULL ){
			GFL_ARC_CloseDataHandle( mapBlock->arc );
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップレンダー作成
 */
//------------------------------------------------------------------
static void MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex )
{
	NNSG3dResFileHeader*	mdlFH = GFL_G3D_GetResourceFileHeader( mdl );
	NNSG3dResFileHeader*	texFH = GFL_G3D_GetResourceFileHeader( tex );
	//モデル＆テクスチャリソースポインタ取得
	NNSG3dResMdl*			pMdl = NNS_G3dGetMdlByIdx( NNS_G3dGetMdlSet( mdlFH ), 0 );	//0th mdl
	NNSG3dResTex*			pTex = NNS_G3dGetTex( texFH );

	GF_ASSERT(pMdl);

	//モデルとテクスチャリソースとの関連付け
	if( pTex ){
		NNS_G3dBindMdlTex( pMdl, pTex );
		NNS_G3dBindMdlPltt( pMdl, pTex );
	}
	//レンダリングオブジェクト初期化
	NNS_G3dRenderObjInit( rndObj, pMdl );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップロード
 */
//------------------------------------------------------------------
enum {
	MDL_LOAD_START = 0,
	MDL_LOAD,
	TEX_LOAD_START,
	TEX_LOAD,
	RND_CREATE,
	TEX_TRANS,
	PLTT_TRANS,
	ATTR_LOAD_START,
	ATTR_LOAD,
	LOAD_END,
};

static void	Load3Dmap( G3D_MAPPER* g3Dmapper )
{
	MAP_BLOCK_DATA*	mapBlock;
	int				i;
	u32				dataAdrs;

	GF_ASSERT( g3Dmapper );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		mapBlock = &g3Dmapper->mapBlock[i];

		if( mapBlock->dataLoadReq == TRUE ){
			switch( mapBlock->loadSeq ){
			case MDL_LOAD_START:
				mapBlock->NNSrnd->resMdl = NULL; 

				//モデルデータロード開始
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->dataID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->data );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = MDL_LOAD;
				break;
			case MDL_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPDATA_SIZE){
					//ロード継続
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->data[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//最終ロード
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPDATA_SIZE - dataAdrs,
													(void*)&mapBlock->data[dataAdrs] );
				mapBlock->loadSeq = TEX_LOAD_START;
				break;
			case TEX_LOAD_START:
				if( mapBlock->texLoadReq == FALSE ){
					mapBlock->loadSeq = RND_CREATE;
					break;
				}
				//テクスチャロード開始
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->texID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->tex );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = TEX_LOAD;
				break;
			case TEX_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPTEX_SIZE){
					//ロード継続
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->tex[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//最終ロード
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPTEX_SIZE - dataAdrs,
													(void*)&mapBlock->tex[dataAdrs] );
				//テクスチャリソースへのＶＲＡＭキーの設定
				NNS_G3dTexSetTexKey( GFL_G3D_GetResTex(mapBlock->g3DgroundResTex), 
										mapBlock->groundTexKey, 0 );
				NNS_G3dPlttSetPlttKey( GFL_G3D_GetResTex(mapBlock->g3DgroundResTex), 
										mapBlock->groundPlttKey );

				mapBlock->loadSeq = RND_CREATE;
				break;
			case RND_CREATE:
				//レンダー作成
				MakeMapRender
					( mapBlock->NNSrnd, mapBlock->g3DgroundResMdl, mapBlock->g3DgroundResTex );
				//--------------------
				{
					int j, r;
					fx32 randTransx, randTransz;
					fx32 cOffs, rOffs, gWidth;

					cOffs = -g3Dmapper->width/2;
					gWidth = g3Dmapper->width/MAP_GRIDCOUNT;

					for( j=0; j<8; j++ ){
						mapBlock->object[j].id = 1;

						MakeMapRender(	mapBlock->object[j].NNSrnd,
										g3Dmapper->objectG3DresMdl[0],
										g3Dmapper->objectG3DresMdl[0] );

						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->object[j].trans.x = mapBlock->trans.x + rOffs + cOffs;
						mapBlock->object[j].trans.y = 0;
						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->object[j].trans.z = mapBlock->trans.z + rOffs + cOffs;
						Get3DmapperHeight_fromROM(	g3Dmapper, 
													&mapBlock->object[j].trans, 
													&mapBlock->object[j].trans.y );
					}
					for( j=0; j<LOCAL_DIRECTDRAW_OBJ_COUNT; j++ ){
						mapBlock->directDrawObject[j].id = 1;

						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->directDrawObject[j].trans.x = mapBlock->trans.x + rOffs + cOffs;
						mapBlock->directDrawObject[j].trans.y = 0;
						rOffs = GFUser_GetPublicRand( MAP_GRIDCOUNT ) * gWidth;
						mapBlock->directDrawObject[j].trans.z = mapBlock->trans.z + rOffs + cOffs;
						Get3DmapperHeight_fromROM(	g3Dmapper, 
													&mapBlock->directDrawObject[j].trans,
													&mapBlock->directDrawObject[j].trans.y );
					}
				}
				//--------------------
				mapBlock->loadCount = 0;
				mapBlock->loadSeq = TEX_TRANS;
				break;
			case TEX_TRANS:
				dataAdrs = mapBlock->loadCount * MAPTRANS_SIZE;

				if( (dataAdrs + MAPTRANS_SIZE) < MAPTEX_SIZE){
					NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
						NNS_GfdGetTexKeyAddr( mapBlock->groundTexKey ) + dataAdrs,
						(void*)(GFL_G3D_GetAdrsTextureData
									( mapBlock->g3DgroundResTex ) + dataAdrs),
						MAPTRANS_SIZE );
					//転送継続
					mapBlock->loadCount++;
					break;
				}
				//最終転送
				NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_VRAM, 
						NNS_GfdGetTexKeyAddr( mapBlock->groundTexKey ) + dataAdrs,
						(void*)(GFL_G3D_GetAdrsTextureData
									( mapBlock->g3DgroundResTex ) + dataAdrs),
						MAPTEX_SIZE - dataAdrs );
				mapBlock->loadSeq = PLTT_TRANS;
				break;
			case PLTT_TRANS:
				NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, 
						NNS_GfdGetPlttKeyAddr( mapBlock->groundPlttKey ),
						(void*)GFL_G3D_GetAdrsTexturePltt( mapBlock->g3DgroundResTex ),
						MAPPLTT_SIZE );
				//mapBlock->loadSeq = ATTR_LOAD_START;
				mapBlock->loadSeq = LOAD_END;
				break;
#if 0
			case ATTR_LOAD_START:
				//アトリビュートデータロード開始
				if( mapBlock->attrID == NON_ATTR ){
					GFL_STD_MemClear32( (void*)mapBlock->attr, MAPATTR_SIZE );
					mapBlock->loadSeq = LOAD_END;
					break;
				}
				GFL_ARC_LoadDataOfsByHandle( mapBlock->arc, mapBlock->attrID, 
												0, MAPLOAD_SIZE, (void*)mapBlock->attr );
				mapBlock->loadCount = 0;
				mapBlock->loadCount++;
				mapBlock->loadSeq = ATTR_LOAD;
				break;
			case ATTR_LOAD:
				dataAdrs = mapBlock->loadCount * MAPLOAD_SIZE;

				if( (dataAdrs + MAPLOAD_SIZE) < MAPATTR_SIZE){
					//ロード継続
					GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPLOAD_SIZE, 
														(void*)&mapBlock->attr[dataAdrs] );
					mapBlock->loadCount++;
					break;
				}
				//最終ロード
				GFL_ARC_LoadDataByHandleContinue(	mapBlock->arc, MAPATTR_SIZE - dataAdrs,
													(void*)&mapBlock->attr[dataAdrs] );
				mapBlock->loadSeq = LOAD_END;
				break;
#endif
			case LOAD_END:
				mapBlock->dataLoadReq = FALSE;
				mapBlock->texLoadReq = FALSE;
//				mapBlock->attrLoadReq = FALSE;
				mapBlock->loadSeq = 0;
				break;
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップロードリクエスト設定
 */
//------------------------------------------------------------------
static void	Set3DmapLoadReq( G3D_MAPPER* g3Dmapper, const int blockID, const VecFx32* trans, 
								const u32 datID, const u32 texID, const u32 attrID )
{
	MAP_BLOCK_DATA* mapBlock;

	GF_ASSERT( blockID < MAP_BLOCK_COUNT );

	mapBlock = &g3Dmapper->mapBlock[blockID];

	if( mapBlock->texID != texID ){
		mapBlock->texID = texID;
		mapBlock->texLoadReq = TRUE;
	}
//	mapBlock->attrID = attrID;
//	mapBlock->attrLoadReq = TRUE;
	mapBlock->dataID = datID;
	mapBlock->dataLoadReq = TRUE;

	mapBlock->loadCount = 0;
	mapBlock->loadSeq = 0;

	mapBlock->idx = datID;
	VEC_Set( &mapBlock->trans, trans->x, trans->y, trans->z );
}


//------------------------------------------------------------------
/**
 * @brief	マップ更新ブロック取得
 */
//------------------------------------------------------------------
typedef struct {
	s16	z;
	s16	x;
}BLOCK_OFFS;

static const BLOCK_OFFS blockPat_Around[] = {//自分のいるブロックから周囲方向に９ブロックとる
	{-1,-1},{-1, 0},{-1, 1},{ 0,-1},{ 0, 0},{ 0, 1},{ 1,-1},{ 1, 0},{ 1, 1},
};

static void GetMapperBlockIndex( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u16		sizex, sizez, idxmax;
	u32		idx, blockx, blockz;
	fx32	inBlockx, inBlockz, width;
	int		i, offsx, offsz;
	
	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	width = g3Dmapper->width;

	//現在ブロックのXZindex取得
	GetMapBlockInMap( g3Dmapper, pos, &g3Dmapper->nowBlockIdx, &inBlockx, &inBlockz );
	blockx = FX_Whole( FX_Div( pos->x, width ) );
	blockz = FX_Whole( FX_Div( pos->z, width ) );

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		offsx = blockx + blockPat_Around[i].x;
		offsz = blockz + blockPat_Around[i].z;

		if((offsx < 0)||(offsx >= sizex)||(offsz < 0)||(offsz >= sizez)){
			idx = MAPID_NULL;
		} else {
			idx = offsz * sizex + offsx;

			if( idx >= idxmax ){
				idx = MAPID_NULL;
			}
		}
		blockIdx[i].blockIdx = idx;
		blockIdx[i].trans.x = offsx * width + width/2;
		blockIdx[i].trans.y = 0;
		blockIdx[i].trans.z = offsz * width + width/2;
	}
}

//------------------------------------------------------------------
/**
 * @brief	マップブロック更新チェック
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( G3D_MAPPER* g3Dmapper, BLOCK_IDX* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;

	reloadFlag = FALSE;

	//更新ブロックチェック
	delProcFlag = FALSE;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( g3Dmapper->mapBlockIdx[i].blockIdx != MAPID_NULL ){
			delFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( g3Dmapper->mapBlockIdx[i].blockIdx == new[j].blockIdx )&&(delFlag == FALSE )){
					new[j].blockIdx = MAPID_NULL;
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
				g3Dmapper->mapBlockIdx[i].blockIdx = MAPID_NULL;
				delProcFlag = TRUE;
			}
		}
	}
	//更新
	addProcFlag = FALSE;
	c = 0;
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( new[i].blockIdx != MAPID_NULL ){
			addFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( g3Dmapper->mapBlockIdx[j].blockIdx == MAPID_NULL )&&(addFlag == FALSE )){

					Set3DmapLoadReq( g3Dmapper, j, &new[i].trans, 
										(u32)g3Dmapper->data[new[i].blockIdx].datID,
										(u32)g3Dmapper->data[new[i].blockIdx].texID,
										(u32)g3Dmapper->data[new[i].blockIdx].attrID );
					g3Dmapper->mapBlockIdx[j] = new[i];
					addFlag = TRUE;
					addProcFlag = TRUE;
				}
			}
			if( addFlag == FALSE ){
				GF_ASSERT(0);
			}
		}
	}
	if( addProcFlag == TRUE ){
		reloadFlag = TRUE;
	}
	return reloadFlag;
}


//------------------------------------------------------------------
/**
 * @brief	ブロック情報取得１
 */
//------------------------------------------------------------------
static BOOL	GetMapBlockInMap( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockIdx, fx32* inBlockx, fx32* inBlockz ) 
{
	u32		blockx, blockz;
	fx32	width;
	VecFx32	mapTop, vecInBlock;
	
	width = g3Dmapper->width;

	//現在ブロックのXZindex取得
	blockx = FX_Whole( FX_Div( pos->x, width ) );
	blockz = FX_Whole( FX_Div( pos->z, width ) );

	mapTop.x = blockx * width;
	mapTop.y = 0;
	mapTop.z = blockz * width;

	VEC_Subtract( pos, &mapTop, &vecInBlock );

	*blockIdx = blockz * g3Dmapper->sizex + blockx;
	*inBlockx = vecInBlock.x;
	*inBlockz = vecInBlock.z;

	//OS_Printf("pos {%x,%x,%x}, blockNum = %x, blockx = %x, blockz = %x\n", 
	//			pos->x, pos->y, pos->z, *blockIdx, blockx, blockz );

	if( *blockIdx >= g3Dmapper->sizex * g3Dmapper->sizez ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	ブロック情報取得２
 */
//------------------------------------------------------------------
static BOOL	GetMapBlockInBuffer( G3D_MAPPER* g3Dmapper, const VecFx32* pos, 
							u32* blockIdx, fx32* inBlockx, fx32* inBlockz ) 
{
	VecFx32 vecTop, vecGrid, inBlockPos;
	VecFx32 vecDefault = {0,0,0};
	fx32	width = g3Dmapper->width;
	int i;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		vecTop.x = g3Dmapper->mapBlock[i].trans.x - width/2;
		vecTop.y = 0;
		vecTop.z = g3Dmapper->mapBlock[i].trans.z - width/2;

		if(	(pos->x >= vecTop.x)&&(pos->x < vecTop.x + width)
			&&(pos->z >= vecTop.z)&&(pos->z < vecTop.z + width) ){

			*blockIdx = i;
			VEC_Subtract( &vecDefault, &vecTop, &vecGrid );
			VEC_Add( &vecGrid, pos, &inBlockPos );
			*inBlockx = inBlockPos.x;
			*inBlockz = inBlockPos.z;
			return TRUE;
		}
	}
	*blockIdx = 0;
	*inBlockx = 0;
	*inBlockz = 0;
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	グリッド情報取得
 */
//------------------------------------------------------------------
static void	GetMapGrid( G3D_MAPPER* g3Dmapper, const fx32 inBlockx, const fx32 inBlockz,
						u32* gridIdx, fx32* inGridx, fx32* inGridz ) 
{
	u32		gridx, gridz;
	fx32	gridWidth = g3Dmapper->width / MAP_GRIDCOUNT;

	gridx = inBlockx / gridWidth;
	gridz = inBlockz / gridWidth;
	*inGridx = inBlockx % gridWidth;
	*inGridz = inBlockz & gridWidth;

	*gridIdx = gridz * MAP_GRIDCOUNT + gridx;

	//OS_Printf("gridIdx = %x, gridx = %x, gridz = %x\n", *gridIdx, gridx, gridz );
}

//------------------------------------------------------------------
/**
 * @brief	グリッド内三角形パターン取得
 */
//------------------------------------------------------------------
static u32 GetMapTriangleID( G3D_MAPPER* g3Dmapper, 
					const fx32 inGridx, const fx32 inGridz, const u32 gridTryPattern )
{
	fx32	gridWidth = g3Dmapper->width / MAP_GRIDCOUNT;

	//グリッド内三角形の判定
	if( gridTryPattern == 0 ){
		//0-2-1,3-1-2のパターン
		if( inGridx + inGridz < gridWidth ){
			return 0;
		} else {
			return 1;
		}
	} else {
		//2-3-0,1-0-3のパターン
		if( inGridx > inGridz ){
			return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief	現在位置地形の法線ベクトル取得
 */
//------------------------------------------------------------------
void Get3DmapperVecN( G3D_MAPPER* g3Dmapper, const VecFx32* pos, VecFx32* vecN )
{
#if 0
	u32		blockIdx;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt* mapData;

	if( GetMapBlockInBuffer( g3Dmapper, pos, &blockIdx, &inBlockx, &inBlockz )  == FALSE ){
		vecN->x = 0; 
		vecN->y = FX32_ONE;
		vecN->z = 0; 
		return;
	}
	mapData = (NormalVtxSt*)&g3Dmapper->mapBlock[blockIdx].attr;

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData->tryangleType ) == 0 ){
		vecN->x = mapData->vecN1_x; 
		vecN->y = mapData->vecN1_y; 
		vecN->z = -mapData->vecN1_z; 
	} else {
		vecN->x = mapData->vecN2_x; 
		vecN->y = mapData->vecN2_y; 
		vecN->z = -mapData->vecN2_z; 
	}
#endif
}

//------------------------------------------------------------------
/**
 * @brief	現在位置地形の高さ取得
 */
//------------------------------------------------------------------
void Get3DmapperHeight( G3D_MAPPER* g3Dmapper, const VecFx32* pos, fx32* height )
{
#if 0
	u32		blockIdx, blockNum;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt* mapData;
	fx32	by, valD;
	VecFx32 vecN, posLocalMap;

	if( GetMapBlockInBuffer( g3Dmapper, pos, &blockIdx, &inBlockx, &inBlockz )  == FALSE ){
		*height = 0;
		return;
	}
	blockNum = g3Dmapper->mapBlock[blockIdx].idx;
	mapData = (NormalVtxSt*)&g3Dmapper->mapBlock[blockIdx].attr;
	VEC_Subtract( pos, &g3Dmapper->mapBlock[blockIdx].trans, &posLocalMap );

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData[gridIdx].tryangleType ) == 0 ){
		vecN.x = mapData[gridIdx].vecN1_x; 
		vecN.y = mapData[gridIdx].vecN1_y; 
		vecN.z = -mapData[gridIdx].vecN1_z;
		valD = mapData[gridIdx].vecN1_D;		//軸の取り方が違うので反転
	} else {
		vecN.x = mapData[gridIdx].vecN2_x; 
		vecN.y = mapData[gridIdx].vecN2_y; 
		vecN.z = -mapData[gridIdx].vecN2_z;
		valD = mapData[gridIdx].vecN2_D;		//軸の取り方が違うので反転
	}
	by = -( FX_Mul( vecN.x, posLocalMap.x ) + FX_Mul( vecN.z, posLocalMap.z ) + valD );
	*height = FX_Div( by, vecN.y );
#endif
}

//------------------------------------------------------------------
/**
 * @brief	現在位置地形の高さ取得２
 */
//------------------------------------------------------------------
void Get3DmapperHeight_fromROM( G3D_MAPPER* g3Dmapper, const VecFx32* pos, fx32* height )
{
	u32		blockNum;
	fx32	inBlockx, inBlockz;
	u32		gridIdx;
	fx32	inGridx, inGridz;
	NormalVtxSt mapData;
	fx32	by, valD, width;
	VecFx32 vecN, posMap, posLocalMap;
	u16		attrID;

	if( GetMapBlockInMap( g3Dmapper, pos, &blockNum, &inBlockx, &inBlockz )  == FALSE ){
		*height = 0;
		return;
	}
	width = g3Dmapper->width;
	posMap.x = FX_Whole( FX_Div( pos->x, width ) ) * width + width/2;
	posMap.y = 0;
	posMap.z = FX_Whole( FX_Div( pos->z, width ) ) * width + width/2;
	VEC_Subtract( pos, &posMap, &posLocalMap );
	//OS_Printf("blockNum = %x, inBlockx = %x, inBlockz = %x ", blockNum, inBlockx, inBlockz );

	GetMapGrid( g3Dmapper, inBlockx, inBlockz, &gridIdx, &inGridx, &inGridz ); 

	attrID = g3Dmapper->data[blockNum].attrID;

	if( attrID == NON_ATTR ){
		*height = 0;
		return;
	}
	GFL_ARC_LoadDataOfs( (void*)&mapData, g3Dmapper->arcID, (u32)attrID,
							sizeof(NormalVtxSt)*gridIdx, sizeof(NormalVtxSt) );

	if( GetMapTriangleID( g3Dmapper, inGridx, inGridz, mapData.tryangleType ) == 0 ){
		vecN.x = mapData.vecN1_x; 
		vecN.y = mapData.vecN1_y; 
		vecN.z = -mapData.vecN1_z;
		valD = mapData.vecN1_D;		//軸の取り方が違うので反転
	} else {
		vecN.x = mapData.vecN2_x; 
		vecN.y = mapData.vecN2_y; 
		vecN.z = -mapData.vecN2_z;
		valD = mapData.vecN2_D;		//軸の取り方が違うので反転
	}
	by = -( FX_Mul( vecN.x, posLocalMap.x ) + FX_Mul( vecN.z, posLocalMap.z ) + valD );
	*height = FX_Div( by, vecN.y );
}





//============================================================================================
/**
 *
 *
 *
 * @brief	ジオメトリ直書き関数
 *
 *
 *
 */
//============================================================================================
typedef void	(DIRECT_DRAW_FUNC)( void );

typedef struct {
	GXTexFmt			texFmt;				//SDK用データフォーマット定義
	GXTexSizeS			s;					//SDK用データサイズX定義
	GXTexSizeT			t;					//SDK用データサイズY定義
	GXRgb				diffuse;
    GXRgb				ambient;
	GXRgb				specular;
    GXRgb				emission;
	u8					polID;
	u8					alpha;
	DIRECT_DRAW_FUNC*	func;

}DIRECT_DRAW_DATA;

static const DIRECT_DRAW_DATA drawTreeData;

static const DIRECT_DRAW_DATA* directDrawDataTable[] = {
	NULL,
	&drawTreeData,
};

static void	DirectDraw3Dmapper
	( G3D_MAPPER* g3Dmapper, MAP_BLOCK_DATA* mapBlock, GFL_G3D_CAMERA* g3Dcamera )
{
	const DIRECT_DRAW_DATA* ddData;
	DIRECT_OBJ*		ddObject;
	MtxFx33	mtxBillboard;
	VecFx32	trans;
	VecFx16	vecN;
	int		scaleVal = 8;
	int		i;

	G3X_Reset();

	//カメラ設定取得
	{
		VecFx32		camPos, camUp, target, vecNtmp;
		MtxFx43		mtxCamera, mtxCameraInv;

		GFL_G3D_CAMERA_GetPos( g3Dcamera, &camPos );
		GFL_G3D_CAMERA_GetCamUp( g3Dcamera, &camUp );
		GFL_G3D_CAMERA_GetTarget( g3Dcamera, &target );

		G3_LookAt( &camPos, &camUp, &target, &mtxCamera );	//mtxCameraには行列計算結果が返る
		MTX_Inverse43( &mtxCamera, &mtxCameraInv );			//カメラ逆行列取得

		MTX_Identity33( &mtxBillboard );					//カメラ逆行列から回転行列を取り出す
		mtxBillboard._00 = mtxCameraInv._00;				//（Ｙ軸回転要素のみ）
		mtxBillboard._02 = mtxCameraInv._02;
		mtxBillboard._20 = mtxCameraInv._20;
		mtxBillboard._22 = mtxCameraInv._22;

		VEC_Subtract( &camPos, &target, &vecNtmp );
		VEC_Normalize( &vecNtmp, &vecNtmp );
		VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );
	}
	//GFL_G3D_LIGHT_Switching( g3Dlightset );

	//グローバルスケール設定
	G3_Scale( FX32_ONE * scaleVal, FX32_ONE * scaleVal, FX32_ONE * scaleVal );

	for( i=0; i<LOCAL_DIRECTDRAW_OBJ_COUNT; i++ ){
		ddObject = &mapBlock->directDrawObject[i];

		if( ddObject->id != DDRAW_OBJID_NULL ){
			GF_ASSERT( ddObject->id  < NELEMS(directDrawDataTable) );

			if( CheckCullingLength( g3Dcamera, &ddObject->trans ) == TRUE ){
				G3_PushMtx();

				ddData = directDrawDataTable[ ddObject->id ]; 

				//回転、平行移動パラメータ設定
				trans.x = ddObject->trans.x / scaleVal;
				trans.y = ddObject->trans.y / scaleVal;
				trans.z = ddObject->trans.z / scaleVal;
				G3_MultTransMtx33( &mtxBillboard, &trans );

				G3_TexImageParam(	ddData->texFmt, GX_TEXGEN_TEXCOORD, ddData->s, ddData->t,
									GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
									GX_TEXPLTTCOLOR0_TRNS, g3Dmapper->tree_texDataAdrs );
				G3_TexPlttBase( g3Dmapper->tree_texPlttAdrs, ddData->texFmt );

				//マテリアル設定
				G3_MaterialColorDiffAmb( ddData->diffuse, ddData->ambient, TRUE );
				G3_MaterialColorSpecEmi( ddData->specular, ddData->emission, FALSE );
				G3_PolygonAttr(	0/*ライトマスク*/, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 
							ddData->polID, ddData->alpha, GX_POLYGON_ATTR_MISC_FOG );

				if( ddData->func != NULL ){
					ddData->func();
				}

				G3_PopMtx(1);
			}
		}
	}
	//↓後にG3D_Systemで行うので、ここではやらない
	//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
}

//------------------------------------------------------------------
/**
 *
 * @brief	木
 *
 */
//------------------------------------------------------------------
static void	_drawTree0( void )
{
	fx32 smax = 64 * FX32_ONE;
	fx32 tmax = 64 * FX32_ONE;

	G3_Begin( GX_BEGIN_QUADS );

	G3_Normal( 0.000 * FX16_ONE, 1.000 * FX16_ONE, 0.000 * FX16_ONE );
	G3_Color( GX_RGB( 31, 31, 31 ) );
	G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
	G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, -1.500 * FX16_ONE );	//vtx0
	G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
	G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );	//vtx2
	G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
	G3_Vtx( 1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );		//vtx3
	G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
	G3_Vtx( 1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, -1.500 * FX16_ONE );	//vtx1

	G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
	G3_Color( GX_RGB( 31, 31, 31 ) );
	G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
	G3_Vtx( -2.000 * FX16_ONE, 2.675 * FX16_ONE, -1.793 * FX16_ONE );	//vtx0
	G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
	G3_Vtx( -2.000 * FX16_ONE, 0.725 * FX16_ONE, 0.993 * FX16_ONE );	//vtx2
	G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
	G3_Vtx( 2.000 * FX16_ONE, 0.725 * FX16_ONE, 0.993 * FX16_ONE );		//vtx3
	G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
	G3_Vtx( 2.000 * FX16_ONE, 2.675 * FX16_ONE, -1.793 * FX16_ONE );	//vtx1

	G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
	G3_Color( GX_RGB( 31, 31, 31 ) );
	G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
	G3_Vtx( -1.650 * FX16_ONE, 3.560 * FX16_ONE, -1.729 * FX16_ONE );	//vtx0
	G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.469 * FX32_ONE,tmax) );
	G3_Vtx( -1.650 * FX16_ONE, 1.840 * FX16_ONE, 0.729 * FX16_ONE );	//vtx2
	G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.469 * FX32_ONE,tmax) );
	G3_Vtx( 1.650 * FX16_ONE, 1.840 * FX16_ONE, 0.729 * FX16_ONE );		//vtx3
	G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
	G3_Vtx( 1.650 * FX16_ONE, 3.560 * FX16_ONE, -1.729 * FX16_ONE );	//vtx1

	G3_Normal( 0.000 * FX16_ONE, 0.819 * FX16_ONE, 0.574 * FX16_ONE );
	G3_Color( GX_RGB( 31, 31, 31 ) );
	G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.656 * FX32_ONE,tmax) );
	G3_Vtx( -1.100 * FX16_ONE, 4.231 * FX16_ONE, -1.601 * FX16_ONE );	//vtx0
	G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
	G3_Vtx( -1.100 * FX16_ONE, 2.969 * FX16_ONE, 0.201 * FX16_ONE );	//vtx2
	G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
	G3_Vtx( 1.100 * FX16_ONE, 2.969 * FX16_ONE, 0.201 * FX16_ONE );		//vtx3
	G3_TexCoord( FX_Mul(0.344 * FX32_ONE,smax), FX_Mul(0.656 * FX32_ONE,tmax) );
	G3_Vtx( 1.100 * FX16_ONE, 4.231 * FX16_ONE, -1.601 * FX16_ONE );	//vtx1

	G3_End();
}

static const DIRECT_DRAW_DATA drawTreeData = {
	GX_TEXFMT_PLTT16, GX_TEXSIZE_S64, GX_TEXSIZE_T64,
	GX_RGB(31,31,31), GX_RGB(16,16,16), GX_RGB(16,16,16), GX_RGB(0,0,0),
	63, 31,
	_drawTree0,
};

