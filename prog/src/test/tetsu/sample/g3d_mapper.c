//============================================================================================
/**
 * @file	g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "system\gfl_use.h"	//乱数用

#include "g3d_mapper.h"

#include "g3dmapfunc\func_mapeditor_file.h"
#include "g3dmapfunc\func_pmcustom_file.h"
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

#define GROBAL_OBJ_COUNT	(32)
#define GROBAL_DDOBJ_COUNT	(32)
//------------------------------------------------------------------
typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCK_IDX;

struct _G3D_MAPPER {
	HEAPID				heapID;
	GFL_G3D_MAP*		g3Dmap[MAP_BLOCK_COUNT];
	BLOCK_IDX			blockIdx[MAP_BLOCK_COUNT];

	u32					nowBlockIdx;				
	VecFx32				posCont;
	u16					sizex;		//横ブロック数
	u16					sizez;		//縦ブロック数
	u32					totalSize;	//配列サイズ
	fx32				width;		//ブロック１辺の幅
	fx32				height;		//ブロック高さ
	G3D_MAPPER_MODE		mode;		//動作モード
	u32					arcID;		//グラフィックアーカイブＩＤ
	const G3D_MAPPER_DATA*	data;	//実マップデータ

	GFL_G3D_RES*			grobalTexture;					//共通地形テクスチャ
	GFL_G3D_MAP_GROBALOBJ	grobalObj;						//共通オブジェクト

//	GFL_G3D_MAP_OBJ		grobalObject[GROBAL_OBJ_COUNT];
//	GFL_G3D_MAP_DDOBJ	grobalDDobject[GROBAL_DDOBJ_COUNT];	//共通オブジェクト(直書き)
};

//------------------------------------------------------------------
static void CreateGrobalTexture( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData );
static void DeleteGrobalTexture( G3D_MAPPER* g3Dmapper );
static void CreateGrobalObject( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData );
static void DeleteGrobalObject( G3D_MAPPER* g3Dmapper );

static void CreateObjResource( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST_OBJ* resistData );
static void DeleteObjResource( G3D_MAPPER* g3Dmapper );
static void CreateDDobjResource( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST_DDOBJ* resistData );
static void DeleteDDobjResource( G3D_MAPPER* g3Dmapper );

static void MakeMapRender( NNSG3dRenderObj* rndObj, GFL_G3D_RES* mdl, GFL_G3D_RES* tex );

static void GetMapperBlockIdxAll( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static void GetMapperBlockIdxXZ( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static void GetMapperBlockIdxY( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx );
static BOOL	ReloadMapperBlock( G3D_MAPPER* g3Dmapper, BLOCK_IDX* new );

static const GFL_G3D_MAP_DDOBJ_DATA drawTreeData;
//------------------------------------------------------------------
/**
 * @brief	セットアップ
 */
//------------------------------------------------------------------
static const MAPFILE_FUNC mapFileFuncTbl[] = {
	{ LoadMapData_MapEditorFile, GetAttr_MapEditorFile },	//FILE_MAPEDITER_DATA
	{ LoadMapData_PMcustomFile, GetAttr_PMcustomFile },		//FILE_MAPEDITER_DATA
};

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

	{
		GFL_G3D_MAP_SETUP setup;

		setup.mapDataHeapSize = MAPMDL_SIZE + MAPTEX_SIZE + MAPATTR_SIZE;
		setup.texVramSize = MAPTEX_SIZE;
		setup.mapFileFunc = mapFileFuncTbl;

		//ブロック制御ハンドル作成
		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			g3Dmapper->g3Dmap[i] = GFL_G3D_MAP_Create( &setup, g3Dmapper->heapID );
		}
	}

	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->totalSize = 0;
	g3Dmapper->width = 0;
	g3Dmapper->height = 0;
	g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
	g3Dmapper->arcID = MAPARC_NULL;
	g3Dmapper->data = NULL;
	
	g3Dmapper->grobalObj.gobj = GFL_HEAP_AllocClearMemory
								( heapID, sizeof(GFL_G3D_MAP_OBJ) * GROBAL_OBJ_COUNT );
	g3Dmapper->grobalObj.gddobj = GFL_HEAP_AllocClearMemory
								( heapID, sizeof(GFL_G3D_MAP_DDOBJ) * GROBAL_DDOBJ_COUNT );
	g3Dmapper->grobalObj.gobjCount = 0;
	g3Dmapper->grobalObj.gddobjCount = 0;

	for( i=0; i<GROBAL_OBJ_COUNT; i++ ){
		g3Dmapper->grobalObj.gobj[i].NNSrnd_H = NULL;
		g3Dmapper->grobalObj.gobj[i].g3Dres_H = NULL;
		g3Dmapper->grobalObj.gobj[i].NNSrnd_L = NULL;
		g3Dmapper->grobalObj.gobj[i].g3Dres_L = NULL;
	}
	for( i=0; i<GROBAL_DDOBJ_COUNT; i++ ){
		g3Dmapper->grobalObj.gddobj[i].g3Dres = NULL;
		g3Dmapper->grobalObj.gddobj[i].texDataAdrs = 0;
		g3Dmapper->grobalObj.gddobj[i].texPlttAdrs = 0;
		g3Dmapper->grobalObj.gddobj[i].data = NULL;
	}

	//OS_Printf("g3DmapperSize = %x\n", sizeof(G3D_MAPPER));
	
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステム破棄
 */
//------------------------------------------------------------------
void	Delete3Dmapper( G3D_MAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	ReleaseData3Dmapper( g3Dmapper );	//登録されたままの場合を想定して削除

	GFL_HEAP_FreeMemory( g3Dmapper->grobalObj.gobj );
	GFL_HEAP_FreeMemory( g3Dmapper->grobalObj.gddobj );

	//ブロック制御ハンドル削除
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		GFL_G3D_MAP_Delete( g3Dmapper->g3Dmap[i] );
	}
	GFL_HEAP_FreeMemory( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムメイン
 */
//------------------------------------------------------------------
void	Main3Dmapper( G3D_MAPPER* g3Dmapper )
{
	BLOCK_IDX nowBlockIdx[MAP_BLOCK_COUNT];
	int i;

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->data == NULL ){
		return;
	}
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		nowBlockIdx[i].blockIdx = MAPID_NULL;
	}

	switch( g3Dmapper->mode ){
	case G3D_MAPPER_MODE_SCROLL_NONE: 
		GetMapperBlockIdxAll( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	default:
	case G3D_MAPPER_MODE_SCROLL_XZ: 
		GetMapperBlockIdxXZ( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	case G3D_MAPPER_MODE_SCROLL_Y: 
		GetMapperBlockIdxY( g3Dmapper, &g3Dmapper->posCont, &nowBlockIdx[0] );
		break;
	}
	ReloadMapperBlock( g3Dmapper, &nowBlockIdx[0] );

	//ブロック制御メイン
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		GFL_G3D_MAP_Main( g3Dmapper->g3Dmap[i] );
	}

	//現在ブロックのindex取得
	{
		u32 blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->width ) );
		u32 blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->width ) );

		g3Dmapper->nowBlockIdx = blockz * g3Dmapper->sizex + blockx;
	}
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムディスプレイ
 */
//------------------------------------------------------------------
void	Draw3Dmapper( G3D_MAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	int i;

	GF_ASSERT( g3Dmapper );

	GFL_G3D_MAP_StartDraw();

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		GFL_G3D_MAP_Draw( g3Dmapper->g3Dmap[i], g3Dcamera );
	}
	GFL_G3D_MAP_EndDraw();
}

//------------------------------------------------------------------
/**
 * @brief	マップデータ登録
 */
//------------------------------------------------------------------
void ResistData3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData )
{
	int i;

	GF_ASSERT( g3Dmapper );

	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->totalSize = resistData->totalSize;
	g3Dmapper->width = resistData->width;
	g3Dmapper->height = resistData->height;
	g3Dmapper->mode = resistData->mode;
	switch( g3Dmapper->mode ){
	case G3D_MAPPER_MODE_SCROLL_NONE:
		if( g3Dmapper->totalSize > MAP_BLOCK_COUNT ){
			OS_Printf("mapper mode set Error\n");
			g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
		}
		break;
	case G3D_MAPPER_MODE_SCROLL_Y:
		if( g3Dmapper->sizex * g3Dmapper->sizez > MAP_BLOCK_COUNT/2 ){
			OS_Printf("mapper mode set Error\n");
			g3Dmapper->mode = G3D_MAPPER_MODE_SCROLL_XZ;
		}
		break;
	}
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->data = resistData->data;

	//グローバルテクスチャ作成
	CreateGrobalTexture( g3Dmapper, resistData );
	//グローバルオブジェクト作成
	CreateGrobalObject( g3Dmapper, resistData );

	//マップブロック制御設定
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		//新アーカイブＩＤを登録
		GFL_G3D_MAP_ResistArc( g3Dmapper->g3Dmap[i], g3Dmapper->arcID, g3Dmapper->heapID );

		//グローバルリソース登録
		if( g3Dmapper->grobalTexture != NULL ){
			GFL_G3D_MAP_ResistGrobalTex( g3Dmapper->g3Dmap[i], g3Dmapper->grobalTexture );
		} else {
			GFL_G3D_MAP_ReleaseGrobalTex( g3Dmapper->g3Dmap[i] );
		}
		GFL_G3D_MAP_ResistGrobalObj( g3Dmapper->g3Dmap[i], &g3Dmapper->grobalObj );

		//ファイル識別設定（仮）
		GFL_G3D_MAP_ResistFileType( g3Dmapper->g3Dmap[i], resistData->g3DmapFileType );
	}
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		g3Dmapper->blockIdx[i].blockIdx = MAPID_NULL;
		VEC_Set( &g3Dmapper->blockIdx[i].trans, 0, 0, 0 );
	}
}

void ReleaseData3Dmapper( G3D_MAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	//マップブロック制御解除
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		GFL_G3D_MAP_ReleaseGrobalObj( g3Dmapper->g3Dmap[i] );
		GFL_G3D_MAP_ReleaseGrobalTex( g3Dmapper->g3Dmap[i] );
		GFL_G3D_MAP_ReleaseArc( g3Dmapper->g3Dmap[i] );
	}
	DeleteGrobalObject( g3Dmapper );
	DeleteGrobalTexture( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	グローバルテクスチャ作成
 */
//------------------------------------------------------------------
static void CreateGrobalTexture( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData )
{
	if( resistData->gtexArcID != NON_GROBAL_TEX ){
		g3Dmapper->grobalTexture = GFL_G3D_CreateResourceArc
										( resistData->gtexArcID, resistData->gtexDatID );
		GFL_G3D_TransVramTexture( g3Dmapper->grobalTexture );
	} else {
		g3Dmapper->grobalTexture = NULL;
	}
}

static void DeleteGrobalTexture( G3D_MAPPER* g3Dmapper )
{
	if( g3Dmapper->grobalTexture != NULL ){
		GFL_G3D_FreeVramTexture( g3Dmapper->grobalTexture );
		GFL_G3D_DeleteResource( g3Dmapper->grobalTexture );
		g3Dmapper->grobalTexture = NULL;
	}
}

//------------------------------------------------------------------
/**
 * @brief	グローバルオブジェクト作成
 */
//------------------------------------------------------------------
static void CreateGrobalObject( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData )
{
	if( resistData->gobjSetType != NON_GROBAL_OBJ ){
		if( resistData->gobjSetType != SET_BINDATA ){
			//テーブルデータより作成
			if( resistData->gobjSet != NULL ){
				G3D_MAPPER_RESIST_OBJ resistObj;
				G3D_MAPPER_RESIST_DDOBJ resistDDobj;
               
				resistObj.arcID = resistData->gobjSet->objArcID;
				resistObj.data = resistData->gobjSet->objData;
				resistObj.count = resistData->gobjSet->objCount;

				resistDDobj.arcID = resistData->gobjSet->ddobjArcID;
				resistDDobj.data = resistData->gobjSet->ddobjData;
				resistDDobj.count = resistData->gobjSet->ddobjCount;

				if( resistObj.count != 0 ){
					CreateObjResource( g3Dmapper, &resistObj );
				}
				if( resistDDobj.count != 0 ){
					CreateDDobjResource( g3Dmapper, &resistDDobj );
				}
			}
		} else {
			//バイナリデータより作成
		}
	}
}

static void DeleteGrobalObject( G3D_MAPPER* g3Dmapper )
{
	DeleteDDobjResource( g3Dmapper );
	DeleteObjResource( g3Dmapper );
}

//------------------------------------------------------------------
// オブジェクトリソースをテーブルから読み込んで作成
//通常MDL
static void CreateObjResource( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST_OBJ* resistData )
{
	int i;

	GF_ASSERT( g3Dmapper );
	GF_ASSERT( resistData->count <= GROBAL_OBJ_COUNT );
	
	for( i=0; i< resistData->count; i++ ){
		g3Dmapper->grobalObj.gobj[i].g3Dres_H = GFL_G3D_CreateResourceArc
										( resistData->arcID, resistData->data[i].highQ_ID );
		GFL_G3D_TransVramTexture( g3Dmapper->grobalObj.gobj[i].g3Dres_H ); 

		g3Dmapper->grobalObj.gobj[i].NNSrnd_H = NNS_G3dAllocRenderObj
												( GFL_G3D_GetAllocaterPointer() );

		MakeMapRender(	g3Dmapper->grobalObj.gobj[i].NNSrnd_H,
						g3Dmapper->grobalObj.gobj[i].g3Dres_H,
						g3Dmapper->grobalObj.gobj[i].g3Dres_H );

		if( resistData->data[i].lowQ_ID != NON_LOWQ ){
			g3Dmapper->grobalObj.gobj[i].g3Dres_L = GFL_G3D_CreateResourceArc
											( resistData->arcID, resistData->data[i].lowQ_ID );
			GFL_G3D_TransVramTexture( g3Dmapper->grobalObj.gobj[i].g3Dres_L ); 

			g3Dmapper->grobalObj.gobj[i].NNSrnd_L = NNS_G3dAllocRenderObj
													( GFL_G3D_GetAllocaterPointer() );

			MakeMapRender(	g3Dmapper->grobalObj.gobj[i].NNSrnd_L,
							g3Dmapper->grobalObj.gobj[i].g3Dres_L,
							g3Dmapper->grobalObj.gobj[i].g3Dres_L );
		}
	}
	g3Dmapper->grobalObj.gobjCount = resistData->count;
}

static void DeleteObjResource( G3D_MAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );
	for( i=0; i<g3Dmapper->grobalObj.gobjCount; i++ ){
		if( g3Dmapper->grobalObj.gobj[i].NNSrnd_L != NULL ){
			NNS_G3dFreeRenderObj(	GFL_G3D_GetAllocaterPointer(), 
									g3Dmapper->grobalObj.gobj[i].NNSrnd_L );
			g3Dmapper->grobalObj.gobj[i].NNSrnd_L = NULL;
		}
		if( g3Dmapper->grobalObj.gobj[i].g3Dres_L != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->grobalObj.gobj[i].g3Dres_L );
			GFL_G3D_DeleteResource( g3Dmapper->grobalObj.gobj[i].g3Dres_L );
			g3Dmapper->grobalObj.gobj[i].g3Dres_L = NULL;
		}
		if( g3Dmapper->grobalObj.gobj[i].NNSrnd_H != NULL ){
			NNS_G3dFreeRenderObj(	GFL_G3D_GetAllocaterPointer(), 
									g3Dmapper->grobalObj.gobj[i].NNSrnd_H );
			g3Dmapper->grobalObj.gobj[i].NNSrnd_H = NULL;
		}
		if( g3Dmapper->grobalObj.gobj[i].g3Dres_H != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->grobalObj.gobj[i].g3Dres_H );
			GFL_G3D_DeleteResource( g3Dmapper->grobalObj.gobj[i].g3Dres_H );
			g3Dmapper->grobalObj.gobj[i].g3Dres_H = NULL;
		}
	}
}

//DirectDraw
static void CreateDDobjResource( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST_DDOBJ* resistData )
{
	GFL_G3D_RES*	g3DresTex;
	NNSG3dTexKey	texDataKey;
	NNSG3dPlttKey	texPlttKey;
	int i;

	GF_ASSERT( g3Dmapper );
	GF_ASSERT( resistData->count <= GROBAL_DDOBJ_COUNT );
	
	for( i=0; i< resistData->count; i++ ){
		g3DresTex = GFL_G3D_CreateResourceArc( resistData->arcID, resistData->data[i] );

		g3Dmapper->grobalObj.gddobj[i].g3Dres = g3DresTex;
										
		if( GFL_G3D_TransVramTexture( g3DresTex ) == FALSE ){
			GF_ASSERT(0);
		}
		texDataKey = GFL_G3D_GetTextureDataVramKey( g3DresTex );
		texPlttKey = GFL_G3D_GetTexturePlttVramKey( g3DresTex );

		g3Dmapper->grobalObj.gddobj[i].texDataAdrs = NNS_GfdGetTexKeyAddr( texDataKey );
		g3Dmapper->grobalObj.gddobj[i].texPlttAdrs = NNS_GfdGetPlttKeyAddr( texPlttKey );
		g3Dmapper->grobalObj.gddobj[i].data = &drawTreeData;
	}
	g3Dmapper->grobalObj.gddobjCount = resistData->count;
}

static void DeleteDDobjResource( G3D_MAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );
	for( i=0; i<g3Dmapper->grobalObj.gddobjCount; i++ ){
		if( g3Dmapper->grobalObj.gddobj[i].g3Dres != NULL ){
			GFL_G3D_FreeVramTexture( g3Dmapper->grobalObj.gddobj[i].g3Dres );
			GFL_G3D_DeleteResource( g3Dmapper->grobalObj.gddobj[i].g3Dres );
			g3Dmapper->grobalObj.gddobj[i].g3Dres = NULL;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	マップ位置セット
 */
//------------------------------------------------------------------
void SetPos3Dmapper( G3D_MAPPER* g3Dmapper, const VecFx32* pos )
{
	GF_ASSERT( g3Dmapper );

	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
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
 * @brief	マップ更新ブロック取得
 */
//------------------------------------------------------------------
static void GetMapperBlockIdxAll( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u32		idx, county, countxz;
	fx32	width, height;
	int		i, j, offsx, offsz;

	countxz = g3Dmapper->sizex * g3Dmapper->sizez;
	county = g3Dmapper->totalSize / countxz;
	if( g3Dmapper->totalSize % countxz ){
		county++;
	}
	width = g3Dmapper->width;
	height = g3Dmapper->height;

	idx = 0;

	for( i=0; i<county; i++ ){
		for( j=0; j<countxz; j++ ){
			offsx = j % g3Dmapper->sizex;
			offsz = j / g3Dmapper->sizex;

			if( idx >= g3Dmapper->totalSize ){
				idx = MAPID_NULL;
			}
			blockIdx[idx].blockIdx = idx;
			blockIdx[idx].trans.x = offsx * width + width/2;
			blockIdx[idx].trans.y = i * height;
			blockIdx[idx].trans.z = offsz * width + width/2;
			idx++;
		}
	}
}

//------------------------------------------------------------------
typedef struct {
	s16	z;
	s16	x;
}BLOCK_OFFS;

static const BLOCK_OFFS blockPat_Around[] = {//自分のいるブロックから周囲方向に９ブロックとる
	{-1,-1},{-1, 0},{-1, 1},{ 0,-1},{ 0, 0},{ 0, 1},{ 1,-1},{ 1, 0},{ 1, 1},
};

static void GetMapperBlockIdxXZ( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u16		sizex, sizez;
	u32		idx, idxmax, blockx, blockz;
	fx32	inBlockx, inBlockz, width;
	int		i, offsx, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	width = g3Dmapper->width;

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
static void GetMapperBlockIdxY( G3D_MAPPER* g3Dmapper, const VecFx32* pos, BLOCK_IDX* blockIdx )
{
	u16		sizex, sizez;
	u32		idx, blocky, countxz;
	fx32	width, height;
	int		i, p, offsx, offsy, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	countxz = sizex * sizez;
	width = g3Dmapper->width;
	height = g3Dmapper->height;

	blocky = FX_Whole( FX_Div( pos->y, height ) );
	if( pos->y - ( blocky * height ) > height/2 ){
		offsy = 1;
	} else {
		offsy = -1;
	}
	p = 0;

	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = blocky * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
		blockIdx[p].blockIdx = idx;
		blockIdx[p].trans.x = offsx * width + width/2;
		blockIdx[p].trans.y = blocky * height;
		blockIdx[p].trans.z = offsz * width + width/2;
		p++;
	}
	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = (blocky + offsy) * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
		blockIdx[p].blockIdx = idx;
		blockIdx[p].trans.x = offsx * width + width/2;
		blockIdx[p].trans.y = (blocky + offsy) * height;
		blockIdx[p].trans.z = offsz * width + width/2;
		p++;
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
		if( g3Dmapper->blockIdx[i].blockIdx != MAPID_NULL ){
			delFlag = FALSE;
			for( j=0; j<MAP_BLOCK_COUNT; j++ ){
				if(( g3Dmapper->blockIdx[i].blockIdx == new[j].blockIdx )&&(delFlag == FALSE )){
					new[j].blockIdx = MAPID_NULL;
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
				g3Dmapper->blockIdx[i].blockIdx = MAPID_NULL;
				GFL_G3D_MAP_SetDrawSw( g3Dmapper->g3Dmap[i], FALSE );
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
				if(( g3Dmapper->blockIdx[j].blockIdx == MAPID_NULL )&&(addFlag == FALSE )){
					u32 mapdatID = g3Dmapper->data[new[i].blockIdx].datID;

					if( mapdatID != G3D_MAPPER_NOMAP ){
						GFL_G3D_MAP_SetLoadReq( g3Dmapper->g3Dmap[j], mapdatID );
						GFL_G3D_MAP_SetTrans( g3Dmapper->g3Dmap[j], &new[i].trans );
						GFL_G3D_MAP_SetDrawSw( g3Dmapper->g3Dmap[j], TRUE );
					}
					g3Dmapper->blockIdx[j] = new[i];
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
static void	_drawTree0( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecView, BOOL lodSt )
{
	GXTexFmt	texFmt = GX_TEXFMT_PLTT16;
	GXTexSizeS	s;
	GXTexSizeT	t;
	fx32		smax, tmax;

	if( lodSt == TRUE ){
		s = GX_TEXSIZE_S64;
		t = GX_TEXSIZE_T64;
		smax = 64 * FX32_ONE;
		tmax = 64 * FX32_ONE;

		G3_TexImageParam(	texFmt, GX_TEXGEN_TEXCOORD, s, t, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
						GX_TEXPLTTCOLOR0_TRNS, texDataAdrs + 0x200 );
		G3_TexPlttBase( texPlttAdrs + 0x20, texFmt );

		G3_Begin( GX_BEGIN_QUADS );

		G3_Normal( 0.000 * FX16_ONE, 1.000 * FX16_ONE, 0.000 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, -1.500 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.531 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( -1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.437 * FX32_ONE,tmax) );
		G3_Vtx( 1.600 * FX16_ONE, 0.200/*0.091*/ * FX16_ONE, 1.500 * FX16_ONE );	//vtx3
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
	} else {
		s = GX_TEXSIZE_S32;
		t = GX_TEXSIZE_T32;
		smax = 32 * FX32_ONE;
		tmax = 32 * FX32_ONE;

		G3_TexImageParam(	texFmt, GX_TEXGEN_TEXCOORD, s, t, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE,
						GX_TEXPLTTCOLOR0_TRNS, texDataAdrs + 0x000 );
		G3_TexPlttBase( texPlttAdrs + 0x00, texFmt );

		G3_Begin( GX_BEGIN_QUADS );

		G3_Normal( 0.000 * FX16_ONE, 1.000 * FX16_ONE, 0.000 * FX16_ONE );
		G3_Color( GX_RGB( 31, 31, 31 ) );
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 4.231 * FX16_ONE, 0 * FX16_ONE );	//vtx0
		G3_TexCoord( FX_Mul(0.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( -2.000 * FX16_ONE, 0.000 * FX16_ONE, 0 * FX16_ONE );	//vtx2
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(1.000 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 0.000 * FX16_ONE, 0 * FX16_ONE );	//vtx3
		G3_TexCoord( FX_Mul(1.000 * FX32_ONE,smax), FX_Mul(0.000 * FX32_ONE,tmax) );
		G3_Vtx( 2.000 * FX16_ONE, 4.231 * FX16_ONE, 0 * FX16_ONE );	//vtx1

		G3_End();
	}
}

static const GFL_G3D_MAP_DDOBJ_DATA drawTreeData = {
	LIGHT_NONE,
	GX_RGB(31,31,31), GX_RGB(16,16,16), GX_RGB(16,16,16), GX_RGB(0,0,0),
	63, 31, DRAW_YBILLBOARD, 8,
	_drawTree0,
};

//============================================================================================
/**
 *
 *
 *
 * @brief	３Ｄマップ情報取得
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ワーク初期化
 */
//------------------------------------------------------------------
void InitGet3DmapperGridInfoData( G3D_MAPPER_INFODATA* gridInfoData )
{
	VEC_Fx16Set( &gridInfoData->vecN, 0, 0, 0 );
	gridInfoData->attr = 0;
	gridInfoData->height = 0;
}

void InitGet3DmapperGridInfo( G3D_MAPPER_GRIDINFO* gridInfo )
{
	int i;

	for( i=0; i<G3D_MAPPER_ATTR_MAX; i++ ){
		InitGet3DmapperGridInfoData( &gridInfo->gridData[i] );
	}
	gridInfo->count = 0;
}

//------------------------------------------------------------------
/**
 * @brief	アトリビュート情報取得
 */
//------------------------------------------------------------------
BOOL Get3DmapperGridInfo
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, G3D_MAPPER_GRIDINFO* gridInfo )
{
	GFL_G3D_MAP_ATTRINFO attrInfo;
	u32 infoCount;
	VecFx32 trans;
	int		i, p;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->data == NULL ){
		return FALSE;
	}

	InitGet3DmapperGridInfo( gridInfo );
	
	p = 0;

	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		if( GFL_G3D_MAP_GetDrawSw( g3Dmapper->g3Dmap[i] ) == TRUE ){
			fx32 min_x, min_z, max_x, max_z;

			GFL_G3D_MAP_GetTrans( g3Dmapper->g3Dmap[i], &trans );

			min_x = trans.x - g3Dmapper->width/2;
			min_z = trans.z - g3Dmapper->width/2;
			max_x = trans.x + g3Dmapper->width/2;
			max_z = trans.z + g3Dmapper->width/2;

			//ブロック範囲内チェック（マップブロックのＸＺ平面上地点）
			if(	(pos->x >= min_x)&&(pos->x < max_x)&&(pos->z >= min_z)&&(pos->z < max_z) ){

				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmapper->g3Dmap[i], pos, g3Dmapper->width );
				if( attrInfo.mapAttrCount ){
					int j;

					if( (p + attrInfo.mapAttrCount) >= G3D_MAPPER_ATTR_MAX ){
						GF_ASSERT("height count over\n");
					}
					for( j=0; j<attrInfo.mapAttrCount; j++ ){
						gridInfo->gridData[p].vecN = attrInfo.mapAttr[j].vecN;
						gridInfo->gridData[p].attr = attrInfo.mapAttr[j].attr;
						gridInfo->gridData[p].height = attrInfo.mapAttr[j].height;
					}
					p += attrInfo.mapAttrCount;
				}
			}
		}
	}
	gridInfo->count = p;
	if( gridInfo->count ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	範囲外チェック
 */
//------------------------------------------------------------------
BOOL Check3DmapperOutRange( G3D_MAPPER* g3Dmapper, const VecFx32* pos )
{
	fx32 widthx, widthz;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->data == NULL ){
		return TRUE;
	}

	widthx = g3Dmapper->sizex * g3Dmapper->width;
	widthz = g3Dmapper->sizez * g3Dmapper->width;

	if( ( pos->x >= 0 )&&( pos->x < widthx )&&( pos->z >= 0 )&&( pos->z < widthz ) ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	サイズ取得
 */
//------------------------------------------------------------------
void Get3DmapperSize( G3D_MAPPER* g3Dmapper, fx32* x, fx32* z )
{
	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->data == NULL ){
		*x = 0;
		*z = 0;
		return;
	}
	*x = g3Dmapper->sizex * g3Dmapper->width;
	*z = g3Dmapper->sizez * g3Dmapper->width;
	return;
}





