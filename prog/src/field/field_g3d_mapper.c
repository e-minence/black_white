//============================================================================================
/**
 * @file	field_g3d_mapper.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "arc\arc_def.h"	// アーカイブデータ

#include "mapdatafunc/field_func_mapeditor_file.h"
#include "mapdatafunc/field_func_pmcustom_file.h"
#include "mapdatafunc/field_func_bridge_file.h"
#include "mapdatafunc/field_func_wbnormal_file.h"
#include "mapdatafunc/field_func_random_generate.h"


#include "field_g3d_mapper.h"
#include "fieldmap_resist.h"
#include "field_g3dmap_exwork.h"
#include "field_ground_anime.h"

#include "system/g3d_tool.h"

#include "field_hit_check.h"

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
#define MAPID_NULL			(0xffffffff)
#define MAPARC_NULL			(0xffffffff)
//------------------------------------------------------------------
//------------------------------------------------------------------




//------------------------------------------------------------------
typedef struct {
	u32			blockIdx;
	VecFx32		trans;
}BLOCKINFO;

//-------------------------------------
// GFL_G3D_MAP拡張ワーク
struct _FLD_G3D_MAP_EXWORK{
	FIELD_GRANM_WORK* p_granm_wk;		// 地面アニメーション
	FIELD_BMODEL_MAN * bmodel_man;	// 
	u32               mapIndex;     //マップのindex(ランダムマップで使用
};


//-------------------------------------
///	ブロック管理ワーク
typedef struct {
	GFL_G3D_MAP*		g3Dmap;       // マップ情報
	FLD_G3D_MAP_EXWORK	g3DmapExWork; // 拡張ワーク
	BLOCKINFO				blockInfo;    //  今の状態
} BLOCK_WORK;

//-------------------------------------
///	ブロック更新リクエストワーク
typedef struct {
  BLOCKINFO       newBlockInfo; //  リクエスト情報
} BLOCK_NEWREQ;




//------------------------------------------------------------------
struct _FLD_G3D_MAPPER {
	HEAPID					heapID;

	FLDMAPPER_FILETYPE	g3DmapFileType;	//g3Dmapファイル識別タイプ（仮）
	fx32				blockWidth;		//ブロック１辺の幅
	fx32				blockHeight;		//ブロック高さ
	FLDMAPPER_MODE		mode;		//動作モード
	u32					arcID;		//グラフィックアーカイブＩＤ

	u16					sizex;		//横ブロック数
	u16					sizez;		//縦ブロック数
	u32					totalSize;	//配列サイズ
	const FLDMAPPER_MAPDATA*	blocks;	//実マップデータ
	
  FLDMAPPER_TEXTYPE gtexType;

  BLOCK_WORK*   blockWk;      // ブロックワーク
  BLOCK_NEWREQ* blockNew;     // ブロック更新リクエストワーク
  u8            blockXNum;    // メモリ配置横ブロック数
  u8            blockZNum;    // メモリ配置縦ブロック数
  u16           blockNum;     // メモリ配置ブロック総数
  
	u32					nowBlockIdx;				
	VecFx32				posCont;

	VecFx32 globalDrawOffset;		//共通座標オフセット

	GFL_G3D_RES*			globalTexture;					//共通地形テクスチャ

  FIELD_BMODEL_MAN* bmodel_man; //配置モデルマネジャー
	FIELD_GRANM*	granime;	// 地面アニメーションシステム
};


//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData );
static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper );


static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new );
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new );

static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos );
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos );



static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID );
// FLD_G3D_MAP_EXWORK　操作関数
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index );
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk );
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk );
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk );
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk );

static void BLOCKINFO_init(BLOCKINFO * info);
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx);
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info);
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info);
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z );
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans);
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans);

//------------------------------------------------------------------
/**
 * @brief	セットアップ
 */
//------------------------------------------------------------------
static const GFL_G3D_MAP_FILE_FUNC mapFileFuncTbl[] = {
	{ WBGRIDPACK_HEADER, FieldLoadMapData_WBNormalFile, FieldGetAttr_WBNormalFile },
	{ DP3PACK_HEADER, FieldLoadMapData_MapEditorFile, FieldGetAttr_MapEditorFile },
	{ BRIDGEPACK_HEADER, FieldLoadMapData_BridgeFile, FieldGetAttr_BridgeFile },
	{ MAPFILE_FUNC_DEFAULT, FieldLoadMapData_PMcustomFile, FieldGetAttr_PMcustomFile },	//TableEnd&default	
};

static const GFL_G3D_MAP_FILE_FUNC randommapFileFuncTbl[] = {
	{ WBGRIDPACK_HEADER, FieldLoadMapData_RandomGenerate, FieldGetAttr_RandomGenerate },
	{ DP3PACK_HEADER, FieldLoadMapData_MapEditorFile, FieldGetAttr_MapEditorFile },
	{ BRIDGEPACK_HEADER, FieldLoadMapData_BridgeFile, FieldGetAttr_BridgeFile },
	{ MAPFILE_FUNC_DEFAULT, FieldLoadMapData_PMcustomFile, FieldGetAttr_PMcustomFile },	//TableEnd&default	
};

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステム作成
 */
//------------------------------------------------------------------
FLDMAPPER*	FLDMAPPER_Create( HEAPID heapID )
{
	FLDMAPPER* g3Dmapper = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMAPPER) );

	g3Dmapper->heapID = heapID;

	VEC_Set( &g3Dmapper->posCont, 0, 0, 0 );
	g3Dmapper->sizex = 0;
	g3Dmapper->sizez = 0;
	g3Dmapper->totalSize = 0;
	g3Dmapper->blocks = NULL;
	g3Dmapper->blockWidth = 0;
	g3Dmapper->blockHeight = 0;
	g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
	g3Dmapper->arcID = MAPARC_NULL;
	
  g3Dmapper->granime = NULL;
  //  配置モデルマネジャー生成
  g3Dmapper->bmodel_man = FIELD_BMODEL_MAN_Create(g3Dmapper->heapID);
	return g3Dmapper;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステム破棄
 */
//------------------------------------------------------------------
void	FLDMAPPER_Delete( FLDMAPPER* g3Dmapper )
{
	GF_ASSERT( g3Dmapper );

	FLDMAPPER_ReleaseData( g3Dmapper );	//登録されたままの場合を想定して削除

  // 配置モデルマネジャー破棄
  if(g3Dmapper->bmodel_man) 
  { 
    FIELD_BMODEL_MAN_Delete(g3Dmapper->bmodel_man);
    g3Dmapper->bmodel_man = NULL;
  }
	GFL_HEAP_FreeMemory( g3Dmapper );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムメイン
 */
//------------------------------------------------------------------
void	FLDMAPPER_Main( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

	if( g3Dmapper->blocks == NULL ){
		return;
	}
	for( i=0; i<g3Dmapper->blockNum; i++ ){
    BLOCKINFO_init(&g3Dmapper->blockNew[i].newBlockInfo);
	}

	switch( g3Dmapper->mode ){
	case FLDMAPPER_MODE_SCROLL_NONE: 
		GetMapperBlockIdxAll( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	default:
	case FLDMAPPER_MODE_SCROLL_XZ: 
		GetMapperBlockIdxXZ( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	case FLDMAPPER_MODE_SCROLL_Y: 
		GetMapperBlockIdxY( g3Dmapper, &g3Dmapper->posCont, g3Dmapper->blockNew );
		break;
	}
	ReloadMapperBlock( g3Dmapper, g3Dmapper->blockNew );

	//ブロック制御メイン
	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_Main( g3Dmapper->blockWk[i].g3Dmap );
	}

	//現在ブロックのindex取得
	{
		u32 blockx = FX_Whole( FX_Div( g3Dmapper->posCont.x, g3Dmapper->blockWidth ) );
		u32 blockz = FX_Whole( FX_Div( g3Dmapper->posCont.z, g3Dmapper->blockWidth ) );

		g3Dmapper->nowBlockIdx = blockz * g3Dmapper->sizex + blockx;
	}
  //配置モデル更新処理（現在は電光掲示板のみ）
  FIELD_BMODEL_MAN_Main( g3Dmapper->bmodel_man );

	// 地面アニメーション管理
	if( g3Dmapper->granime ){
		FIELD_GRANM_Main( g3Dmapper->granime );
	}
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップコントロールシステムディスプレイ
 */
//------------------------------------------------------------------
void	FLDMAPPER_Draw( const FLDMAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera )
{
	int i;
	VecFx32 org_pos,draw_pos;
	
	GF_ASSERT( g3Dmapper );
	
	GFL_G3D_MAP_StartDraw();

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
		draw_pos.x = org_pos.x + g3Dmapper->globalDrawOffset.x;
		draw_pos.y = org_pos.y + g3Dmapper->globalDrawOffset.y;
		draw_pos.z = org_pos.z + g3Dmapper->globalDrawOffset.z;
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &draw_pos );
		GFL_G3D_MAP_Draw( g3Dmapper->blockWk[i].g3Dmap, g3Dcamera );
		GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[i].g3Dmap, &org_pos );
	}

  FIELD_BMODEL_MAN_Draw( g3Dmapper->bmodel_man );
	GFL_G3D_MAP_EndDraw();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckTrans( const FLDMAPPER* g3Dmapper )
{
	int i;
	for ( i=0; i<g3Dmapper->blockNum; i++ ){
		GFL_G3D_MAP_LOAD_STATUS *ldst;
		GFL_G3D_MAP_GetLoadStatusPointer( g3Dmapper->blockWk[i].g3Dmap, &ldst );
		if (ldst->seq != GFL_G3D_MAP_LOAD_IDLING) {
			return FALSE;
		}
	}
	return TRUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_G3D_MAP_GLOBALOBJ_ST * FLDMAPPER_CreateObjStatusList
( const FLDMAPPER* g3Dmapper, const FLDHIT_RECT * rect, HEAPID heapID, u32 * num )
{
  enum { MAPOBJ_MAX = 32 };
  int i, j, k;
  int count = 0;
  GFL_G3D_MAP_GLOBALOBJ_ST *status;
  VecFx32 map_pos;
  u8 * set = GFL_HEAP_AllocClearMemory(heapID, g3Dmapper->blockNum * MAPOBJ_MAX );
  GFL_G3D_MAP_GLOBALOBJ_ST * st;

	for ( i=0; i<g3Dmapper->blockNum; i++ ){
    GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &map_pos);
    for ( j=0; j<MAPOBJ_MAX; j++)
    {
      VecFx32 trans;
		  status = GFL_G3D_MAP_GetGlobalObj( g3Dmapper->blockWk[i].g3Dmap, j );
      if (status == NULL)
      {
        continue;
      }
      VEC_Add( &status->trans, &map_pos, &trans );
      if (rect->top <= trans.z && trans.z <= rect->bottom
          && rect->left <= trans.x && trans.x <= rect->right)
      {
        set[MAPOBJ_MAX * i + j] = 1;
      }
    }
  }

  for (i = 0; i < MAPOBJ_MAX * g3Dmapper->blockNum; i++)
  {
    if (set[i] ) count ++;
  }
  if (count == 0)
  {
    *num = 0;
    GFL_HEAP_FreeMemory(set);
    return NULL;
  }

  st = GFL_HEAP_AllocClearMemory(heapID, sizeof(GFL_G3D_MAP_GLOBALOBJ_ST) * count);
  for (i=0, k=0; i<MAPOBJ_MAX * g3Dmapper->blockNum; i++)
  {
    if(set[i])
    {
      GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i / MAPOBJ_MAX].g3Dmap, &map_pos);
      j = i % MAPOBJ_MAX;
		  status = GFL_G3D_MAP_GetGlobalObj( g3Dmapper->blockWk[i / MAPOBJ_MAX].g3Dmap, j );
      if (status == NULL)
      {
        GF_ASSERT(0);
      }
      st[k] = *status;
      VEC_Add( &st[k].trans, &map_pos, &st[k].trans );
      k ++;
    }
  }
  *num = count;

  GFL_HEAP_FreeMemory(set);
  return st;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	マップデータ登録
 */
//------------------------------------------------------------------
void FLDMAPPER_ResistData( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	GF_ASSERT( g3Dmapper );

	g3Dmapper->g3DmapFileType = resistData->g3DmapFileType;
	g3Dmapper->blockWidth = resistData->blockWidth;
	g3Dmapper->blockHeight = resistData->blockHeight;
	g3Dmapper->arcID = resistData->arcID;
	g3Dmapper->sizex = resistData->sizex;
	g3Dmapper->sizez = resistData->sizez;
	g3Dmapper->totalSize = resistData->totalSize;
	g3Dmapper->blocks = resistData->blocks;
	g3Dmapper->mode = resistData->mode;
  g3Dmapper->blockXNum  = resistData->blockXNum;
  g3Dmapper->blockZNum  = resistData->blockZNum;
  g3Dmapper->blockNum   = resistData->blockXNum * resistData->blockZNum;

  {//mode異常の対処 
    switch( g3Dmapper->mode ){
    case FLDMAPPER_MODE_SCROLL_NONE:
      if( g3Dmapper->totalSize > g3Dmapper->blockNum ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    case FLDMAPPER_MODE_SCROLL_Y:
      if( g3Dmapper->sizex * g3Dmapper->sizez > g3Dmapper->blockNum/2 ){
        OS_Printf("mapper mode set Error\n");
        g3Dmapper->mode = FLDMAPPER_MODE_SCROLL_XZ;
      }
      break;
    }
  }

  g3Dmapper->gtexType = resistData->gtexType;
	//グローバルテクスチャ作成
	CreateGlobalTexture( g3Dmapper, resistData );
	//グローバルオブジェクト作成
	//CreateGlobalObject( g3Dmapper, resistData );
  

	// 地面アニメーション作成
  g3Dmapper->granime = createGroundAnime(g3Dmapper->blockNum, g3Dmapper->globalTexture, &resistData->ground_anime, g3Dmapper->heapID );
	
	{
		int i;
		GFL_G3D_MAP_SETUP setup;

		if (g3Dmapper->globalTexture != NULL) {
			setup.mapDataHeapSize = resistData->memsize;
			setup.texVramSize = 0;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
		} else {
			setup.mapDataHeapSize = resistData->memsize;
			setup.texVramSize = FLD_MAPPER_MAPTEX_SIZE;
			setup.mapFileFunc = mapFileFuncTbl;
			setup.externalWork = NULL;
		}
		
		if( resistData->g3DmapFileType == FLDMAPPER_FILETYPE_RANDOM )
		{
      OS_TPrintf("C8!!!\n");
			setup.mapFileFunc = randommapFileFuncTbl;
    }


		//ブロック制御ハンドル作成
    GF_ASSERT( resistData->blockXNum > 0 );
    GF_ASSERT( resistData->blockZNum > 0 );
    g3Dmapper->blockWk = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_WORK) * g3Dmapper->blockNum );
    g3Dmapper->blockNew = GFL_HEAP_AllocClearMemory( g3Dmapper->heapID, sizeof(BLOCK_NEWREQ) * g3Dmapper->blockNum );
		for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP * g3dmap;

      BLOCKINFO_init(&g3Dmapper->blockWk[i].blockInfo);

			// 拡張ワークの初期化
			FLD_G3D_MAP_ExWork_Init( &g3Dmapper->blockWk[i].g3DmapExWork, g3Dmapper, i );
			setup.externalWork = &g3Dmapper->blockWk[i].g3DmapExWork;

			// ブロック情報生成
      g3dmap = GFL_G3D_MAP_Create( &setup, g3Dmapper->heapID );
			g3Dmapper->blockWk[i].g3Dmap = g3dmap;

      //新アーカイブＩＤを登録
      GFL_G3D_MAP_ResistArc( g3dmap, g3Dmapper->arcID, g3Dmapper->heapID );

      //グローバルリソース登録
      if( g3Dmapper->globalTexture != NULL ){
        GFL_G3D_MAP_ResistGlobalTexResource( g3dmap, g3Dmapper->globalTexture );
      }
      GFL_G3D_MAP_ResistGlobalObjResource( g3dmap,
          FIELD_BMODEL_MAN_GetGlobalObjects(g3Dmapper->bmodel_man) );

      //ファイル識別設定（仮）
      GFL_G3D_MAP_ResistFileType( g3dmap, resistData->g3DmapFileType );
    }

	}

	//グローバルオブジェクト作成
	//CreateGlobalObject( g3Dmapper, resistData );

}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FLDMAPPER_ReleaseData( FLDMAPPER* g3Dmapper )
{
	int i;

	GF_ASSERT( g3Dmapper );

  if( g3Dmapper->blockWk ){

    //マップブロック制御解除
    for( i=0; i<g3Dmapper->blockNum; i++ ){
      GFL_G3D_MAP_ReleaseGlobalObjResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseGlobalTexResource( g3Dmapper->blockWk[i].g3Dmap );
      GFL_G3D_MAP_ReleaseArc( g3Dmapper->blockWk[i].g3Dmap );

      GFL_G3D_MAP_Delete( g3Dmapper->blockWk[i].g3Dmap );

      // 拡張ワークの破棄
      FLD_G3D_MAP_ExWork_Exit( &g3Dmapper->blockWk[i].g3DmapExWork );
    }


    GFL_HEAP_FreeMemory( g3Dmapper->blockWk );
    GFL_HEAP_FreeMemory( g3Dmapper->blockNew );
    g3Dmapper->blockNew = NULL;
    g3Dmapper->blockWk = NULL;
    g3Dmapper->blockNum = 0;
  }

	// 地面アニメーション破棄
	if( g3Dmapper->granime != NULL ){
		FIELD_GRANM_Delete( g3Dmapper->granime );
		g3Dmapper->granime = NULL;
	}
	
	//DeleteGlobalObject( g3Dmapper, FIELD_BMODEL_MAN_GetGlobalObjects(g3Dmapper->bmodel_man) );
	DeleteGlobalTexture( g3Dmapper );
}

#include "system/palanm.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_comm.h"
//--------------------------------------------------------------
/**
 * デバッグ：侵入フィールド白黒化テスト
 *
 * @param   g3Dres		
 *
 * @retval  BOOL		
 */
//--------------------------------------------------------------
BOOL DEBUG_Field_Grayscale(GFL_G3D_RES *g3Dres)
{
	NNSG3dResFileHeader*	header;
	NNSG3dResTex*			texture;
  u32 sz;
  void* pData;
  
  if(GameCommSys_BootCheck( GAMESYSTEM_GetGameCommSysPtr( DEBUG_GameSysWorkPtrGet() ) ) != GAME_COMM_NO_INVASION){
    return FALSE;
  }
  
	header = GFL_G3D_GetResourceFileHeader(g3Dres);
	texture = NNS_G3dGetTex( header ); 

	if( texture ){
    sz = (u32)texture->plttInfo.sizePltt << 3;
    pData = (u8*)texture + texture->plttInfo.ofsPlttData;
    PaletteGrayScale(pData, sz / sizeof(u16));
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	グローバルテクスチャ作成
 */
//------------------------------------------------------------------
static void CreateGlobalTexture( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData )
{
	switch (resistData->gtexType) {
	case FLDMAPPER_TEXTYPE_USE:
		{
			const FLDMAPPER_RESIST_TEX* gtexData = &resistData->gtexData;
			g3Dmapper->globalTexture = GFL_G3D_CreateResourceArc( gtexData->arcID, gtexData->datID );
			DEBUG_Field_Grayscale(g3Dmapper->globalTexture);
			GFL_G3D_TransVramTexture( g3Dmapper->globalTexture );
		}
		break;
	case FLDMAPPER_TEXTYPE_NONE:
		g3Dmapper->globalTexture = NULL;
	}
}

static void DeleteGlobalTexture( FLDMAPPER* g3Dmapper )
{
	if( g3Dmapper->globalTexture != NULL ){
		GFL_G3D_FreeVramTexture( g3Dmapper->globalTexture );
		GFL_G3D_DeleteResource( g3Dmapper->globalTexture );
		g3Dmapper->globalTexture = NULL;
	}
}



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	マップ位置セット
 */
//------------------------------------------------------------------
void FLDMAPPER_SetPos( FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	GF_ASSERT( g3Dmapper );

	VEC_Set( &g3Dmapper->posCont, pos->x, pos->y, pos->z );
}

//------------------------------------------------------------------
/**
 * @brief	マップ更新ブロック取得
 */
//------------------------------------------------------------------
static void GetMapperBlockIdxAll( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u32		idx, county, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, j, offsx, offsz;

	countxz = g3Dmapper->sizex * g3Dmapper->sizez;
	county = g3Dmapper->totalSize / countxz;
	if( g3Dmapper->totalSize % countxz ){
		county++;
	}
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	idx = 0;

	for( i=0; i<county; i++ ){
		for( j=0; j<countxz; j++ ){
			offsx = j % g3Dmapper->sizex;
			offsz = j / g3Dmapper->sizex;

			if( idx >= g3Dmapper->totalSize ){
				idx = MAPID_NULL;
			}
      BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, idx );
      BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
         FX_Mul(offsx<<FX32_SHIFT, blockWidth) + blockHalfWidth,
         FX_Mul( i<<FX32_SHIFT,blockHeight ),
         FX_Mul(offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
			idx++;
		}
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GetMapperBlockIdxXZ( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, idxmax;
	fx32	blockWidth, blockHalfWidth;
	int		i, j;
  int   start_x, start_z;
  int   set_x, set_z;


	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	idxmax = sizex * sizez;
	blockWidth = g3Dmapper->blockWidth;
  blockHalfWidth = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );

  // XZ方向ブロック開始位置を求める
  if( (g3Dmapper->blockXNum % 2) == 0 ){
    // 偶数の場合
    start_x = GetMapperBlockStartIdxEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }else{
    // 奇数の場合
    start_x = GetMapperBlockStartIdxUnEven( g3Dmapper->blockXNum, blockWidth, pos->x );
  }
  if( (g3Dmapper->blockXNum % 2) == 0 ){
    // 偶数の場合
    start_z = GetMapperBlockStartIdxEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }else{
    // 奇数の場合
    start_z = GetMapperBlockStartIdxUnEven( g3Dmapper->blockZNum, blockWidth, pos->z );
  }

  // 読み込みリクエスト情報を設定する
	for( i=0; i<g3Dmapper->blockZNum; i++ )
  {
    // 設定するブロックのZ値を求める 
    // ブロックデータの範囲内かチェックする
    set_z = start_z + i;
    if( (set_z >= 0) && (set_z < sizez) )
    {
  	  for( j=0; j<g3Dmapper->blockXNum; j++ )
      {
        // 設定するブロックのX値を求める 
        // ブロックデータの範囲内かチェックする
        set_x = start_x + j;
        if( (set_x >= 0) && (set_x < sizex) )
        {
          idx = (i*g3Dmapper->blockXNum) + j;
          BLOCKINFO_SetBlockIdx( &new[idx].newBlockInfo, (set_z * sizex) + set_x );
          BLOCKINFO_SetBlockTrans( &new[idx].newBlockInfo, 
             FX_Mul(set_x<<FX32_SHIFT, blockWidth) + blockHalfWidth,
             0,
             FX_Mul(set_z<<FX32_SHIFT, blockWidth) + blockHalfWidth );
        }
      }
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GetMapperBlockIdxY( const FLDMAPPER* g3Dmapper, const VecFx32* pos, BLOCK_NEWREQ* new )
{
	u16		sizex, sizez;
	u32		idx, blocky, countxz;
	fx32	blockWidth, blockHeight;
  fx32  blockHalfWidth;
	int		i, p, offsx, offsy, offsz;

	sizex = g3Dmapper->sizex;
	sizez = g3Dmapper->sizez;
	countxz = sizex * sizez;
	blockWidth      = g3Dmapper->blockWidth;
  blockHalfWidth  = FX_Div( g3Dmapper->blockWidth, 2*FX32_ONE );
	blockHeight     = g3Dmapper->blockHeight;

	blocky = FX_Whole( FX_Div( pos->y, blockHeight ) );
	if( pos->y - ( blocky * blockHeight ) > blockHeight/2 ){
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
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( blocky<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth) + blockHalfWidth );
		p++;
	}
	for( i=0; i<countxz; i++ ){
		offsx = i % g3Dmapper->sizex;
		offsz = i / g3Dmapper->sizex;

		idx = (blocky + offsy) * countxz + offsz * sizex + offsx;

		if( idx >= g3Dmapper->totalSize ){
			idx = MAPID_NULL;
		}
    BLOCKINFO_SetBlockIdx( &new[p].newBlockInfo, idx );
    BLOCKINFO_SetBlockTrans( &new[p].newBlockInfo, 
       FX_Mul( offsx<<FX32_SHIFT, blockWidth ) + blockHalfWidth,
       FX_Mul( (blocky + offsy)<<FX32_SHIFT, blockHeight ),
       FX_Mul( offsz<<FX32_SHIFT, blockWidth ) + blockHalfWidth );
		p++;
	}
}

//------------------------------------------------------------------
/**
 * @brief	マップブロック更新チェック
 */
//------------------------------------------------------------------
static BOOL	ReloadMapperBlock( FLDMAPPER* g3Dmapper, BLOCK_NEWREQ* new )
{
	BOOL addFlag, delFlag, delProcFlag, addProcFlag, reloadFlag;
	int i, j, c;
  u32 now_blockIdx, new_blockIdx;

	reloadFlag = FALSE;

	//更新ブロックチェック
	delProcFlag = FALSE;
	for( i=0; i<g3Dmapper->blockNum; i++ ){
		if( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[i].blockInfo)  ){
			delFlag = FALSE;
      now_blockIdx = BLOCKINFO_GetBlockIdx( &g3Dmapper->blockWk[i].blockInfo );
			for( j=0; j<g3Dmapper->blockNum; j++ ){
        new_blockIdx = BLOCKINFO_GetBlockIdx( &new[j].newBlockInfo );
				if(( now_blockIdx == new_blockIdx )&&(delFlag == FALSE )){
          
          BLOCKINFO_init( &new[j].newBlockInfo );
					delFlag = TRUE;
				}
			}
			if( delFlag == FALSE ){
        BLOCKINFO_init( &g3Dmapper->blockWk[i].blockInfo );
				GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[i].g3Dmap, FALSE );

        FIELD_BMODEL_MAN_ReleaseAllMapObjects( g3Dmapper->bmodel_man, g3Dmapper->blockWk[i].g3Dmap );
				// 拡張ワークの情報もクリア
				FLD_G3D_MAP_ExWork_ClearBlockData( &g3Dmapper->blockWk[i].g3DmapExWork );
				delProcFlag = TRUE;
			}
		}
	}
	//更新
	addProcFlag = FALSE;
	c = 0;
	for( i=0; i<g3Dmapper->blockNum; i++ ){
      
		if( BLOCKINFO_IsInBlockData( &new[i].newBlockInfo) ){
			addFlag = FALSE;
			for( j=0; j<g3Dmapper->blockNum; j++ ){

				if(( BLOCKINFO_IsInBlockData( &g3Dmapper->blockWk[j].blockInfo ) == FALSE )&&(addFlag == FALSE )){
          u32 blockIdx = BLOCKINFO_GetBlockIdx( &new[i].newBlockInfo );
					u32 mapdatID = g3Dmapper->blocks[blockIdx].datID;
          VecFx32 trans;

          BLOCKINFO_GetBlockTrans( &new[i].newBlockInfo, &trans );

					if( mapdatID != FLDMAPPER_MAPDATA_NULL ){

						GFL_G3D_MAP_SetLoadReq( g3Dmapper->blockWk[j].g3Dmap, mapdatID );
						GFL_G3D_MAP_SetTrans( g3Dmapper->blockWk[j].g3Dmap, &trans );
						GFL_G3D_MAP_SetDrawSw( g3Dmapper->blockWk[j].g3Dmap, TRUE );
					}
					g3Dmapper->blockWk[j].blockInfo = new[i].newBlockInfo;
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



//----------------------------------------------------------------------------
/**
 *	@brief  ブロック数偶数のときの、ブロック開始位置を求める
 *
 *	@param	blockNum    ブロック数
 *	@param	width       １ブロック幅
 *	@param	pos         自分の位置
 *
 *	@return ブロック開始位置  （負の値の場合もあり）
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 offs;
  fx32 block;
  fx32 width_half;
  s32 block_start;

  block_start = -((blockNum/2)-1); // ブロックの端の位置を求める、あらかじめ自分を含めるために-1している

  block = pos / width;
  offs  = pos % width;
  width_half = FX_Div( width, 2*FX32_ONE );

  if( offs < width_half ){
    block_start += block-1;
  }else{
    block_start += block;
  }

  return block_start;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロック数奇数のときの、ブロック開始位置を求める
 *
 *	@param	blockNum    ブロック数
 *	@param	width       １ブロック幅
 *	@param	pos         自分の位置
 *
 *	@return ブロック開始位置  （負の値の場合もあり）
 */
//-----------------------------------------------------------------------------
static s32 GetMapperBlockStartIdxUnEven( u32 blockNum, fx32 width, fx32 pos )
{
  fx32 block;
  s32 block_start;

  block_start = -(blockNum/2);  // ブロックの端の位置を求める。
                                // 奇数は、２で割ることで、勝手に自分の分が減る

  block = pos / width;
  block_start += block;

  return block_start;
}



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
void FLDMAPPER_GRIDINFODATA_Init( FLDMAPPER_GRIDINFODATA* gridInfoData )
{
	VEC_Fx16Set( &gridInfoData->vecN, 0, 0, 0 );
	gridInfoData->attr = 0;
	gridInfoData->height = 0;
}

void FLDMAPPER_GRIDINFO_Init( FLDMAPPER_GRIDINFO* gridInfo )
{
	int i;

	for( i=0; i<FLDMAPPER_GRIDINFO_MAX; i++ ){
		FLDMAPPER_GRIDINFODATA_Init( &gridInfo->gridData[i] );
	}
	gridInfo->count = 0;
}

//------------------------------------------------------------------
/**
 * @brief	アトリビュート情報取得
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_GetGridInfo
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFO* gridInfo )
{
	GFL_G3D_MAP_ATTRINFO attrInfo;
	VecFx32 trans;
	int		i, p;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		OS_Printf("データが読み込まれていない\n");
		return FALSE;
	}

	FLDMAPPER_GRIDINFO_Init( gridInfo );
	
	p = 0;

	for( i=0; i<g3Dmapper->blockNum; i++ ){
		if( GFL_G3D_MAP_GetDrawSw( g3Dmapper->blockWk[i].g3Dmap ) == TRUE ){
			fx32 min_x, min_z, max_x, max_z;

			GFL_G3D_MAP_GetTrans( g3Dmapper->blockWk[i].g3Dmap, &trans );

			min_x = trans.x - g3Dmapper->blockWidth/2;
			min_z = trans.z - g3Dmapper->blockWidth/2;
			max_x = trans.x + g3Dmapper->blockWidth/2;
			max_z = trans.z + g3Dmapper->blockWidth/2;

			//ブロック範囲内チェック（マップブロックのＸＺ平面上地点）
			if(	(pos->x >= min_x)&&(pos->x < max_x)&&(pos->z >= min_z)&&(pos->z < max_z) ){

				GFL_G3D_MAP_GetAttr( &attrInfo, g3Dmapper->blockWk[i].g3Dmap, pos, g3Dmapper->blockWidth );
				if( attrInfo.mapAttrCount ){
					int j;

					if( (p + attrInfo.mapAttrCount) >= FLDMAPPER_GRIDINFO_MAX ){
						GF_ASSERT("height count over\n");
					}
					for( j=0; j<attrInfo.mapAttrCount; j++ ){
						gridInfo->gridData[p+j].vecN = attrInfo.mapAttr[j].vecN;
						gridInfo->gridData[p+j].attr = attrInfo.mapAttr[j].attr;
						gridInfo->gridData[p+j].height = attrInfo.mapAttr[j].height;
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
//	OS_Printf("データが存在していない\n");
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	範囲外チェック
 */
//------------------------------------------------------------------
BOOL FLDMAPPER_CheckOutRange( const FLDMAPPER* g3Dmapper, const VecFx32* pos )
{
	fx32 widthx, widthz;

	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		return TRUE;
	}

	widthx = g3Dmapper->sizex * g3Dmapper->blockWidth;
	widthz = g3Dmapper->sizez * g3Dmapper->blockWidth;

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
void FLDMAPPER_GetSize( const FLDMAPPER* g3Dmapper, fx32* x, fx32* z )
{
	GF_ASSERT( g3Dmapper );
	if( g3Dmapper->blocks == NULL ){
		*x = 0;
		*z = 0;
		return;
	}
	*x = g3Dmapper->sizex * g3Dmapper->blockWidth;
	*z = g3Dmapper->sizez * g3Dmapper->blockWidth;
	return;
}

#if 0
//--------------------------------------------------------------
/**
 * マップデータファイルタイプを取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
FLDMAPPER_FILETYPE FLDMAPPER_GetFileType( const FLDMAPPER *g3Dmapper )
{
	return( g3Dmapper->g3DmapFileType );
}
#endif

//--------------------------------------------------------------
//	描画オフセット
//--------------------------------------------------------------
void FLDMAPPER_SetDrawOffset( FLDMAPPER *g3Dmapper, const VecFx32 *offs )
{
	g3Dmapper->globalDrawOffset = *offs;
}

void FLDMAPPER_GetDrawOffset( const FLDMAPPER *g3Dmapper, VecFx32 *offs )
{
	*offs = g3Dmapper->globalDrawOffset;
}


//--------------------------------------------------------------
/**
 * @brief 配置モデルマネジャーを取得する
 */
//--------------------------------------------------------------
FIELD_BMODEL_MAN * FLDMAPPER_GetBuildModelManager( FLDMAPPER* g3Dmapper)
{ 
  return g3Dmapper->bmodel_man;
}



//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static FIELD_GRANM * createGroundAnime( u32 blockNum, GFL_G3D_RES* globalTexture,
    const FLDMAPPER_RESIST_GROUND_ANIME * resistGroundAnimeData, u32 heapID )
{ 
  static FIELD_GRANM_SETUP granm_setup = {
    FALSE, FALSE,
    0,
    ARCID_AREA_ITA, 0,
    ARCID_AREA_ITP_TBL, 0, 
    ARCID_AREA_ITP, ARCID_AREA_ITP_TEX,
  };

  //  ブロック数
  granm_setup.block_num = blockNum;

  // ITAアニメーションの設定
  if( resistGroundAnimeData->ita_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.ita_use			= FALSE;
  }else{
    granm_setup.ita_use			= TRUE;
    granm_setup.ita_dataID	= resistGroundAnimeData->ita_datID;
  }

  // ITPアニメーションの設定
  if( resistGroundAnimeData->itp_anm_datID == FLDMAPPER_MAPDATA_NULL ){
    granm_setup.itp_use			= FALSE;
  }else{
    granm_setup.itp_use			= TRUE;
    granm_setup.itp_tblID		= resistGroundAnimeData->itp_anm_datID;
  }
	return FIELD_GRANM_Create( &granm_setup, globalTexture, heapID );
}


//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
/**
 *			GFL_G3D_MAP拡張ワーク操作
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	拡張ワークから、地面アニメーションの有無を取得
 *
 *	@param	cp_wk			ワーク
 *
 *	@retval	TRUE	ある
 *	@retval	FALSE	ない
 */
//-----------------------------------------------------------------------------
BOOL FLD_G3D_MAP_EXWORK_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_IsGranm( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	拡張ワークから、地面アニメーションワークを取得
 *
 *	@param	cp_wk		ワーク
 *	
 *	@return	地面アニメーションわーく
 */
//-----------------------------------------------------------------------------
FIELD_GRANM_WORK* FLD_G3D_MAP_EXWORK_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return FLD_G3D_MAP_ExWork_GetGranmWork( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	配置モデルの制御ワーク取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	配置モデルの制御ワーク
 */
//-----------------------------------------------------------------------------
FIELD_BMODEL_MAN* FLD_G3D_MAP_EXWORK_GetBModelMan( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return  cp_wk->bmodel_man;
}

//----------------------------------------------------------------------------
/**
 *	@brief	マップのインデックス取得
 *
 *	@param	cp_wk		ワーク
 *
 *	@return	マップのインデックス
 */
//-----------------------------------------------------------------------------
const u32 FLD_G3D_MAP_EXWORK_GetMapIndex( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return  cp_wk->mapIndex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAPの拡張ワーク初期化
 *
 *	@param	p_wk				ワーク
 *	@param	g3Dmapper		マッパーワーク
 *	@param	index				ブロックインデックス
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Init( FLD_G3D_MAP_EXWORK* p_wk, FLDMAPPER* g3Dmapper, u32 index )
{
	// 地面アニメ
	p_wk->p_granm_wk = FIELD_GRANM_GetWork( g3Dmapper->granime, index );
  p_wk->bmodel_man = g3Dmapper->bmodel_man;
  p_wk->mapIndex = index;
}

//----------------------------------------------------------------------------
/**
 *	@brief	GFL_G3D_MAPの拡張ワーク破棄
 *
 *	@param	p_wk		ワーク
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_Exit( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// ブロックモデルとのリンクを解消
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
		p_wk->p_granm_wk = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	ブロックに割り当てていた情報をクリアする
 *
 *	@param	p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void FLD_G3D_MAP_ExWork_ClearBlockData( FLD_G3D_MAP_EXWORK* p_wk )
{
	if( p_wk->p_granm_wk ){
		// ブロックモデルとのリンクを解消
		FIELD_GRANM_WORK_Release( p_wk->p_granm_wk );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションがあるかチェック
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	TRUE	ある
 *	@retval	FALSE	ない
 */
//-----------------------------------------------------------------------------
static BOOL FLD_G3D_MAP_ExWork_IsGranm( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	if( cp_wk->p_granm_wk ){
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	地面アニメーションのワークを取得する
 *
 *	@param	cp_wk		ワーク
 *
 *	@retval	地面アニメーションのワーク
 *	@retval	NULL 地面アニメーションがない
 */
//-----------------------------------------------------------------------------
static FIELD_GRANM_WORK* FLD_G3D_MAP_ExWork_GetGranmWork( const FLD_G3D_MAP_EXWORK* cp_wk )
{
	return cp_wk->p_granm_wk;
}

//============================================================================================
//============================================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void BLOCKINFO_init(BLOCKINFO * info)
{ 
		info->blockIdx = MAPID_NULL;
		VEC_Set( &info->trans, 0, 0, 0 );
}


//----------------------------------------------------------------------------
/**
 *	@brief  ブロックインデックスの設定
 *
 *	@param	info      インフォワーク
 *	@param	blockIdx  ブロックインデックス
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockIdx(BLOCKINFO * info, u32 blockIdx)
{
  info->blockIdx = blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロック情報が入っているかチェック
 *
 *	@param	info  インフォワーク
 *
 *	@retval TRUE  入っている
 *	@retval FALSE 入っていない
 */
//-----------------------------------------------------------------------------
static BOOL BLOCKINFO_IsInBlockData(const BLOCKINFO * info)
{
  if( info->blockIdx == MAPID_NULL ){
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	info
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u32 BLOCKINFO_GetBlockIdx(const BLOCKINFO * info)
{
  return info->blockIdx;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロック位置を設定
 *  
 *	@param	info      ワーク
 *	@param	trans     位置
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_SetBlockTrans(BLOCKINFO * info, fx32 x, fx32 y, fx32 z )
{
	VEC_Set( &info->trans, x, y, z );
}
static void BLOCKINFO_SetBlockTransVec(BLOCKINFO * info, const VecFx32* trans)
{
  info->trans = *trans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ブロック位置の取得
 *
 *	@param	info      ワーク
 *	@param	trans     位置格納先
 */
//-----------------------------------------------------------------------------
static void BLOCKINFO_GetBlockTrans(const BLOCKINFO * info, VecFx32* trans)
{
   *trans = info->trans;
}





