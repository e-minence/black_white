//============================================================================================
/**
 * @file	map3d.c
 * @brief	マップ生成
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"

//------------------------------------------------------------------
/**
 * @brief	型宣言
 */
//------------------------------------------------------------------
#define EXOBJ_MAX	(64)
#define EXOBJ_NULL	(0xffff)

typedef struct {
	u16	id;
	u16	count;
}MAPOBJ_HEADER;

struct _SCENE_MAP {
	GFL_G3D_SCENE*		g3Dscene;
	u16					unitIdx;
	u16					objIdx;
	u16*				mapAttr;
	MAPOBJ_HEADER		defaultFloor;
	MAPOBJ_HEADER		extraObject[EXOBJ_MAX];
};

typedef struct {
	const char*				data;
	GFL_G3D_SCENEOBJ_DATA	floor;
}MAPDATA;

typedef struct {
	const GFL_G3D_SCENEOBJ_DATA*	data;
	int								count;
}EXOBJ_DATATABLE;

typedef struct { 
	int			seq;

}EXMAPOBJ_WORK;//setup.cで指定されているサイズに収めるように注意

#define mapSizeX	(32)
#define mapSizeZ	(32)
#define mapGrid		(FX32_ONE*64)
#define defaultMapX	(-mapGrid*16+mapGrid/2)
#define defaultMapZ	(-mapGrid*16+mapGrid/2)

static void	MapDataCreate( SCENE_MAP* sceneMap, const MAPDATA* map, HEAPID heapID );
static void MapDataDelete( SCENE_MAP* sceneMap );
static void AddExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj, int objID, VecFx32* trans );
static void RemoveExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj );
static const	MAPDATA mapDataTbl;
static const	EXOBJ_DATATABLE extraObjectDataTbl[5];

//------------------------------------------------------------------
/**
 * @brief	セットアップ
 */
//------------------------------------------------------------------
#include "src/sample_graphic/haruka.naix"
static const char g3DarcPath2[] = {"src/sample_graphic/haruka.narc"};

enum {
	G3DRES_MAP_FLOOR = 0,
	G3DRES_EFFECT_WALL,
};
//３Ｄグラフィックリソーステーブル
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ (u32)g3DarcPath2, NARC_haruka_test_floor2_nsbmd,  GFL_G3D_UTIL_RESPATH },
	{ (u32)g3DarcPath2, NARC_haruka_test_wall_nsbmd,  GFL_G3D_UTIL_RESPATH },
};

//---------------------
enum {
	G3DOBJ_MAP_FLOOR = 0,
	G3DOBJ_EFFECT_WALL,
};
//３Ｄオブジェクト定義テーブル
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_MAP_FLOOR, 0, G3DRES_MAP_FLOOR, NULL, 0 },
	{ G3DRES_EFFECT_WALL, 0, G3DRES_EFFECT_WALL, NULL, 0 },
};

//---------------------
//g3Dscene 初期設定テーブルデータ
static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl, NELEMS(g3Dutil_resTbl),
	g3Dutil_objTbl, NELEMS(g3Dutil_objTbl),
};

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ生成
 */
//------------------------------------------------------------------
SCENE_MAP*	Create3Dmap( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SCENE_MAP* sceneMap = GFL_HEAP_AllocMemory( heapID, sizeof(SCENE_MAP) );
	GFL_G3D_SCENEOBJ_DATA* data;
	int	i;

	//３Ｄデータセットアップ
	sceneMap->unitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3Dutil_setup );
	sceneMap->objIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, sceneMap->unitIdx );

	//マップ作成
	sceneMap->g3Dscene = g3Dscene;
	MapDataCreate( sceneMap, &mapDataTbl, heapID );

	for( i=0; i<EXOBJ_MAX; i++ ){
		sceneMap->extraObject[i].id = EXOBJ_NULL;
		sceneMap->extraObject[i].count = 0;
	}
	return sceneMap;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ破棄
 */
//------------------------------------------------------------------
void	Delete3Dmap( SCENE_MAP* sceneMap )
{
	int	i;

	for( i=0; i<EXOBJ_MAX; i++ ){
		if( sceneMap->extraObject[i].id != EXOBJ_NULL ){
			RemoveExtraObject( sceneMap, &sceneMap->extraObject[i] );
		}
	}
	MapDataDelete( sceneMap );
	GFL_G3D_SCENE_DelG3DutilUnit( sceneMap->g3Dscene, sceneMap->unitIdx );
	GFL_HEAP_FreeMemory( sceneMap );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップオブジェクト追加
 */
//------------------------------------------------------------------
int		AddObject3Dmap( SCENE_MAP* sceneMap, int objID, VecFx32* trans )
{
	int	i;

	for( i=0; i<EXOBJ_MAX; i++ ){
		if( sceneMap->extraObject[i].id == EXOBJ_NULL ){
			AddExtraObject( sceneMap, &sceneMap->extraObject[i], objID, trans );
			return i;
		}
	}
	GF_ASSERT(0);
	return 0;
}	

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップオブジェクト破棄
 */
//------------------------------------------------------------------
void	RemoveObject3Dmap( SCENE_MAP* sceneMap, int mapobjID )
{
	if( sceneMap->extraObject[mapobjID].id != EXOBJ_NULL ){
		RemoveExtraObject( sceneMap, &sceneMap->extraObject[mapobjID] );
		sceneMap->extraObject[mapobjID].id = EXOBJ_NULL;
	}
}	

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップオブジェクト描画スイッチ設定
 */
//------------------------------------------------------------------
void	Set3DmapDrawSw( SCENE_MAP* sceneMap, int mapobjID, BOOL* sw )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneMap->g3Dscene, 
													sceneMap->extraObject[mapobjID].id );
	GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, sw );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップオブジェクト描画スイッチ取得
 */
//------------------------------------------------------------------
void	Get3DmapDrawSw( SCENE_MAP* sceneMap, int mapobjID, BOOL* sw )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneMap->g3Dscene, 
													sceneMap->extraObject[mapobjID].id );
	GFL_G3D_SCENEOBJ_GetDrawSW( g3DsceneObj, sw );
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップアトリビュート取得
 */
//------------------------------------------------------------------
BOOL	Get3DmapAttr( SCENE_MAP* sceneMap, VecFx32* pos, u16* attr )
{
	fx32 wx = pos->x + mapGrid*16;
	fx32 wz = pos->z + mapGrid*16;
	int x = wx/mapGrid;
	int z = wz/mapGrid;
	if(( wx < 0 )||( wx >= mapGrid * mapSizeX )||( wz < 0 )||( wz >= mapGrid * mapSizeZ )){
		return FALSE;
	}
	*attr = sceneMap->mapAttr[ z * mapSizeX + x ];
	return TRUE;
}


//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ生成メイン
 */
//------------------------------------------------------------------
static const  GFL_G3D_OBJSTATUS defaultStatus = {
	{ 0, 0, 0 },
	{ FX32_ONE, FX32_ONE, FX32_ONE },
	{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
};
static void GetWallData
			( GFL_G3D_SCENEOBJ_DATA* data, u16 chkCode, u16 up, u16 down, u16 left, u16 right ); 

static inline u16 GET_MAPCODE( u16* mapdata, int x, int z )
{
	u16	tmpdata;

	if(( x < 0 )||( x >= mapSizeX )||( z < 0 )||( z >= mapSizeZ )){
		return  '■';
	}
	tmpdata = mapdata[ z * mapSizeX + x ];
	return  (( tmpdata & 0x00ff ) << 8 ) + (( tmpdata & 0xff00 ) >> 8 );
}

//回転マトリクス変換
static inline void rotateCalc( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotX33(	rotDst, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );

	MTX_RotY33(	&tmp, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)rotSrc->z), FX_CosIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

//------------------------------------------------------------------
/**
 * @brief		３Ｄマップデータセットアップ
 */
//------------------------------------------------------------------
static void MapDataCreate( SCENE_MAP* sceneMap, const MAPDATA* map, HEAPID heapID )
{
	GFL_G3D_SCENEOBJ_DATA*			pdata;
	GFL_G3D_SCENEOBJ_DATA			data;
	u16*							mapAttr;
	int		count = 0;
	int		x,z;
	u16*	mapData = (u16*)map->data;	//めんどいので全角のみに制限
	int		sizeX = mapSizeX;
	int		sizeZ = mapSizeZ;
	u16		mapCode;

	mapAttr = GFL_HEAP_AllocMemory( heapID, 2*sizeX*sizeZ );
	pdata = GFL_HEAP_AllocMemoryLo( heapID, sizeof(GFL_G3D_SCENEOBJ_DATA)*sizeX*sizeZ+1 );

	pdata[ count ] = map->floor;
	count++;

	for( z=0; z<sizeZ; z++ ){
		for( x=0; x<sizeX; x++ ){
			mapCode = GET_MAPCODE( mapData, x, z );
			switch( mapCode ){
			default:
			case '　':
				mapAttr[ z * sizeX + x ] = 0;
				break;
			case '■':	//壁
				mapAttr[ z * sizeX + x ] = 1;
				break;
			case '○':	//配置人物１
#if 0
				data.objID = G3DOBJ_HUMAN2_STOP; 
				data.movePriority = 0;
				data.drawPriority = 250;
				data.cullingFlag = TRUE;
				data.drawSW = TRUE;
				data.status = defaultStatus;
				data.blendAlpha = 31;
				data.status.trans.x = defaultMapX + (mapGrid * x);
				data.status.trans.z = defaultMapZ + (mapGrid * z);
				data.status.scale.x = FX32_ONE/4;
				data.status.scale.y = FX32_ONE/4;
				data.status.scale.z = FX32_ONE/4;
				data.func = moveStopHaruka;
				pdata[ count ] = data;
				count++;
#endif
				mapAttr[z*sizeX+x] = 0;
				break;
			case '◎':	//プレーヤー
#if 0
				tetsuWork->contPos.x = defaultMapX + (mapGrid * x);
				tetsuWork->contPos.y = 0;
				tetsuWork->contPos.z = defaultMapZ + (mapGrid * z);
#endif
				mapAttr[z*sizeX+x] = 0;
				break;
			}
		}
	}

	//マップオブジェクト追加
	sceneMap->defaultFloor.id = GFL_G3D_SCENEOBJ_Add
								( sceneMap->g3Dscene, pdata, count, sceneMap->objIdx );
	sceneMap->defaultFloor.count = count;
	sceneMap->mapAttr = mapAttr;

	//マップオブジェクト設定後、セットアップ用配列は破棄
	GFL_HEAP_FreeMemory( pdata );
}

//------------------------------------------------------------------
static void MapDataDelete( SCENE_MAP* sceneMap )
{
	GFL_G3D_SCENEOBJ_Remove
			(sceneMap->g3Dscene, sceneMap->defaultFloor.id, sceneMap->defaultFloor.count ); 
	GFL_HEAP_FreeMemory( sceneMap->mapAttr );
}

//------------------------------------------------------------------
/**
 * @brief		３Ｄマップオブジェクトデータセットアップ
 */
//------------------------------------------------------------------
static void AddExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj, int objID, VecFx32* trans )
{
	int	i;
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	VecFx32	transTmp;

	exobj->count = extraObjectDataTbl[objID].count;
	exobj->id = GFL_G3D_SCENEOBJ_Add
			( sceneMap->g3Dscene, extraObjectDataTbl[objID].data, exobj->count, sceneMap->objIdx );

	for( i=0; i<exobj->count; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneMap->g3Dscene, exobj->id + i );
		GFL_G3D_SCENEOBJ_GetPos( g3DsceneObj, &transTmp );
		transTmp.x = trans->x;
		transTmp.z = trans->z;
		GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, &transTmp );
	}
}

//------------------------------------------------------------------
static void RemoveExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj )
{
	GFL_G3D_SCENEOBJ_Remove( sceneMap->g3Dscene, exobj->id, exobj->count ); 
}


//------------------------------------------------------------------
/**
 * @brief		３Ｄマップオブジェクトデータ動作
 */
//------------------------------------------------------------------
static void moveExtraObject( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	EXMAPOBJ_WORK*	exw = (EXMAPOBJ_WORK*)work;

	if( exw->seq == 3 ){
		return;
	}
	if( exw->seq == 2 ){
		u8	blendAlphaEnd = 31;
		GFL_G3D_SCENEOBJ_SetBlendAlpha( sceneObj, &blendAlphaEnd );
		exw->seq++;
	}
	{
		VecFx32	transTmp;
		fx32	heightLimit = 0;

		GFL_G3D_SCENEOBJ_GetPos( sceneObj, &transTmp );
		if( transTmp.y < heightLimit ){	//高さチェック
			transTmp.y += FX32_ONE/16;
			if( transTmp.y >= heightLimit ){
				transTmp.y = heightLimit;
				exw->seq++;
			}
			GFL_G3D_SCENEOBJ_SetPos( sceneObj, &transTmp );
		}
	}
	{
		u8	blendAlphaTmp;
		u8	alphaLimit = 20;

		GFL_G3D_SCENEOBJ_GetBlendAlpha( sceneObj, &blendAlphaTmp );
		if( blendAlphaTmp < alphaLimit ){	//透明度チェック
			blendAlphaTmp++;
			if( blendAlphaTmp >= alphaLimit ){
				blendAlphaTmp = alphaLimit;
				exw->seq++;
			}
			GFL_G3D_SCENEOBJ_SetBlendAlpha( sceneObj, &blendAlphaTmp );
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief		データ
 */
//------------------------------------------------------------------
static const char mapData0[] = {
"　　　　■■■■■■■■　　　　　　　　■■■■■■■■　　　　"	//16
"　　　　■■■■■■■■　　　　　　　　■■■■■■■■　　　　"	//
"　　　　■■■■■■■■　　　　　　　　■■■■■■■■　　　　"	//
"　　　　■■■■■■■■　　　　　　　　■■■■■■■■　　　　"	//
"　　　　■■■■■■■■　　　　　　　　■■■■■■■■　　　　"	//
"　　　　■■■■■■■■　　　　　　　　■■■■　　　■　　　　"	//
"　　　　■■■■■■■■　　　　　　　　■　　■　　　■　　　　"	//
"　　　　■■■■■■■■■■　　　　■■■　　■■　　■　　　　"	//
"　　　　■■　　　　　　　　　　　　　　　　　■　　　■　　　　"	//
"　　　　■■　　　　　　　　　　　　　　　　　■　　　■　　　　"	//
"　　　　■■　　　　　　　　　　　　　　　　　■　　　　　　　　"	//
"　　　　■■　　　　　　　　　　　　　　　　　■　　　　　　　　"	//
"　　　　■■■■■■　　　■■■■■■■■■■■　■■■■■■■"	//
"　　　　■■　　　　　　　■　　　　　　　　■　　　　　　　　　"	//
"　　　　■■　　　　　　　■　　　　　　　　■　　　　　　　　　"	//
"　　　　■■　　　　　　　■　　　　■　　　■　　　　　　　　　"	//
"　　　　■■　　　　　　　■　　　　■　　　■　　　　　　　　　"	//
"　　　　■■■■■■　　　■■■■■■■　　■　　　　　　　　　"	//
"　　　　　　　　　　　　　　　　　　　■　　■■　■■■■　　　"	//
"　　　　　　　　　　　　　　　　　　　■　　　　　■　　　　　　"	//
"　　　　　　　　　　　　　　　　　　　■　　　　　■　　　　　　"	//
"　　　　■■■■■　　■■■■■　　　■　　　　　■　　　　　　"	//
"　　　　■■　　　　　　　　　■　　　■　　　　　■　　　　　　"	//
"　　　　■■　　　　　　　　　■　　　■　　　　　■　　　　　　"	//
"　　　　■■　　　　　　　■　■■■■■■■■■■■■■■　　　"	//
"　　　　■■　　　　　　　■　■■■■　　　　　　　　　■　　　"	//
"　　　　■■■■■■　　　■　■■■■　　　　　　　　　■　　　"	//
"　　　　■■■■■■■■■■　　　　■　　■■■■■　　　　　　"	//
"　　　　■■■■■■■■■■　　　　■　　■■■■■　　　　　　"	//
"　　　　■■■■■■■■■■■■　　　　　■■■■■　　　　　　"	//
"　　　　■■■■■■■■■■■■　　　　　　　　　　　　　　　　"	//
"　　　　■■■■■■■■■■■■　　　　　　　　　　　　　　　　"	//
};

static const MAPDATA mapDataTbl = {
	mapData0,
	{	G3DOBJ_MAP_FLOOR, 0, 1, 31, FALSE, TRUE,
		{	{ 0, 0, 0 },
			{ FX32_ONE*8, FX32_ONE*8, FX32_ONE*8 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

static const GFL_G3D_SCENEOBJ_DATA extraObject1[] = {
	{	G3DOBJ_EFFECT_WALL, 0, 1, 8, TRUE, TRUE,
		{	{ 0, -FX32_ONE*64, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const	EXOBJ_DATATABLE extraObjectDataTbl[5] = {
	{ extraObject1, NELEMS(extraObject1) },
	{ extraObject1, NELEMS(extraObject1) },
	{ extraObject1, NELEMS(extraObject1) },
	{ extraObject1, NELEMS(extraObject1) },
	{ extraObject1, NELEMS(extraObject1) },
};



//------------------------------------------------------------------
/**
 * @brief		地面との当たり判定
 */
//------------------------------------------------------------------
BOOL	CheckHitMapGround( VecFx32* posNow, VecFx32* vecMove, VecFx32* posResult )
{
	VecFx32 vecRay, posRef, vecN;
	GFL_G3D_CALC_RESULT result;

	//レイ進行ベクトルの算出
	vecRay = *vecMove;
	VEC_Normalize( &vecRay, &vecRay );	//正規化

	//原点(0,0,0)を通るy=0水平面を暫定の地形とする
	posRef.x = 0;
	posRef.y = 0;
	posRef.z = 0;
	vecN.x = 0;
	vecN.y = FX32_ONE;
	vecN.z = 0;

	result = GFL_G3D_Calc_GetClossPointRayPlane
				( posNow, &vecRay, &posRef, &vecN, posResult, 0 );
	if( result == GFL_G3D_CALC_TRUE ){
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief		ベクトル移動の際の地面との当たり判定
 */
//------------------------------------------------------------------
BOOL	CheckHitMapGroundLimit( VecFx32* posNow, VecFx32* posNext, VecFx32* posResult )
{
	VecFx32 vecMove, posRef, vecN;
	fx32 scalar;
	BOOL retFlag;
	GFL_G3D_CALC_RESULT result;

	//原点(0,0,0)を通るy=0水平面を暫定の地形とする
	posRef.x = 0;
	posRef.y = 0;
	posRef.z = 0;
	vecN.x = 0;
	vecN.y = FX32_ONE;
	vecN.z = 0;

	result = GFL_G3D_Calc_GetClossPointRayPlaneLimit
				( posNow, posNext, &posRef, &vecN, posResult, 0 );

	if( result == GFL_G3D_CALC_OUTRANGE ){
		return FALSE;
	}
	if( result == GFL_G3D_CALC_FALSE ){
		*posResult = *posNext;
		return TRUE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief		マップ上でのベクトル移動（重力あり）
 *
 *	gravityTimerはここでしか操作しないようにする
 */
//------------------------------------------------------------------
void	InitMoveMapGround( int* gravityTimer )
{
	*gravityTimer = 0;
}

BOOL	MoveMapGround( VecFx32* posNow, VecFx32* vecMove, int* gravityTimer )
{
	VecFx32 posNext, posLimit;
	fx32	gravity = (9.8f * FX32_ONE)/8;

	VEC_Add( posNow, vecMove, &posNext );

	if( *gravityTimer ){
		//Y軸移動（重力コントロール）
		posNext.y -= ( gravity * (*gravityTimer) / 2 );
	}
	if( CheckHitMapGroundLimit( posNow, &posNext, &posLimit ) == TRUE ){
		//接地
		*gravityTimer = 0;
		posNext = posLimit;
	} else {
		(*gravityTimer)++;
	}
	*posNow = posNext;

	return TRUE;
}

