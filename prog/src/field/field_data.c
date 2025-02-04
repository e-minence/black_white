//============================================================================================
/**
 * @file  field_data.c
 * @brief 各マップのデータサイズ・プログラム指定
 * @author  GAMEFREAK inc.
 * @date  2008.09
 */
//============================================================================================
#include <gflib.h>
#include "./fieldmap.h"
#include "arc_def.h"
#include "field_data.h"

#include "./fieldmap_resist.h"
#include "field/zonedata.h"
#include "field/areadata.h"
#include "field/map_matrix.h"

#include "fieldmap/area_id.h"

#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid.h"
#include "fieldmap_ctrl_hybrid.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  HEAPSIZE_FIELDMAP_NORMAL = 0x130000,
  HEAPSIZE_FIELDMAP_COMM   = 0x128000,
  HEAPSIZE_FIELDMAP_MUSICAL = 0xa8000,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	//横ブロック数, 縦ブロック数, ブロック１辺の幅, グラフィックアーカイブＩＤ, 実マップデータ
	FLDMAPPER_RESISTDATA	mapperData;   ///<各タイプごとのデータ
	const DEPEND_FUNCTIONS * dep_funcs; ///<使用するフィールドメイン制御プログラム指定
	BOOL isMatrixMapFlag;               ///<マトリックスデータを読み込むか？のチェック（現在未使用）
  u32 heap_size;                      ///<フィールドマップが確保するヒープのサイズ
}SCENE_DATA;


//------------------------------------------------------------------
//------------------------------------------------------------------
const SCENE_DATA	resistMapTbl[];
const unsigned int resistMapTblCount;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 MapID2ResistID(u16 mapid)
{
	u16 resid;
	GF_ASSERT(mapid < ZONEDATA_GetZoneIDMax());
	resid = ZONEDATA_GetMapRscID(mapid);
	GF_ASSERT(resid < resistMapTblCount);
	return resid;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELDDATA_SetMapperData(u16 mapid, const AREADATA * areadata,
    FLDMAPPER_RESISTDATA * map_res, MAP_MATRIX *matrix_buf )
{
	u16 resid = MapID2ResistID(mapid);
	*map_res = resistMapTbl[resid].mapperData;

	if (resistMapTbl[resid].isMatrixMapFlag){
		map_res->sizex = MAP_MATRIX_GetMapBlockSizeWidth( matrix_buf );
		map_res->sizez = MAP_MATRIX_GetMapBlockSizeHeight( matrix_buf );
		map_res->totalSize = MAP_MATRIX_GetMapBlockTotalSize( matrix_buf );
		map_res->blocks = (const FLDMAPPER_MAPDATA*)MAP_MATRIX_GetMapResIDTable( matrix_buf );
	}

  map_res->gtexType = FLDMAPPER_TEXTYPE_USE;
  map_res->gtexData.arcID = ARCID_AREA_MAPTEX;
  map_res->gtexData.datID = AREADATA_GetTextureSetID(areadata);

  map_res->ground_anime.ita_datID = AREADATA_GetGroundITAID(areadata);
  map_res->ground_anime.itp_anm_datID = AREADATA_GetGroundITPID(areadata);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const DEPEND_FUNCTIONS * FIELDDATA_GetFieldFunctions(u16 mapid)
{
	u16 resid = MapID2ResistID(mapid);
	return resistMapTbl[resid].dep_funcs;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ZONEIDのマップコントロールタイプを取得
 *
 *	@param	mapid ゾーンID
 *
 *	@return マップコントロールタイプ
 */
//-----------------------------------------------------------------------------
FLDMAP_CTRLTYPE FIELDDATA_GetFieldCtrlType(u16 mapid)
{
	u16 resid = MapID2ResistID(mapid);
	return resistMapTbl[resid].dep_funcs->type;
}

//-----------------------------------------------------------------------------
/**
 * @brief フィールド用ヒープサイズの取得
 * @param zone_id ゾーンID
 * @return  u32 フィールド用ヒープのサイズ
 */
//-----------------------------------------------------------------------------
u32 FIELDDATA_GetFieldHeapSize( u16 zone_id )
{
  u16 resid = MapID2ResistID( zone_id );
  return resistMapTbl[resid].heap_size;
}

//============================================================================================
//============================================================================================




//------------------------------------------------------------------
//------------------------------------------------------------------
#define MAP_XZ_SIZE (512 * FX32_ONE)	//16 unit * 32 grid
#define MAP_HEIGHT (128 * FX32_ONE)

const SCENE_DATA resistMapTbl[] = {
  //RSC_GRID_DEFAULT = 0,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_XZ,
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_CROSSBLOCK_MEMSIZE,	// 1ブロックメモリサイズ(立体交差用アトリビュートサイズ込み) by iwasawa 090910
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_NOGRID_BRIDGE = 1,
	{	//実験マップ　橋
		{
			2048*FX32_ONE, 2048*FX32_ONE, 
      2,3,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			2,	3, 1,
			NULL,

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{FLDMAPPER_MAPDATA_NULL,FLDMAPPER_MAPDATA_NULL},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      1,  // topフレームのブロック描画数
		}, 
		&FieldMapCtrl_NoGridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_NOGRID_C3 = 2,
	{	//実験マップ　C3
		{	
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			2,  2, 1,
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{FLDMAPPER_MAPDATA_NULL,FLDMAPPER_MAPDATA_NULL},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		}, 
		&FieldMapCtrl_NoGridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_PALACE = 3,
	{	//実験マップ グリッド移動   パレス
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      3,2,
      FLDMAPPER_MODE_SCROLL_XZ_LOOP, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_RANDOMMAP = 4,
	{	//実験マップ グリッド移動   ランダム生成マップ
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_NOGRID_LEAGUE = 5,
	{	//実験マップ チャンピオンリーグ
		{
			1024*FX32_ONE, 1024*FX32_ONE, 
      1,1,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE*4,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_UNION = 6,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      1,1,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,
			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_COMM,
	},
  //RSC_NOGRID_DEFAULT = 7,
	{	//実験マップ レール移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,3,
      FLDMAPPER_MODE_SCROLL_XZ, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_NoGridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_NOGRID_C03P02 = 8,
	{	//実験マップ レール移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      3,2,
      FLDMAPPER_MODE_SCROLL_XZ, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_NoGridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_MUSICAL = 9,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      1,1,
      FLDMAPPER_MODE_SCROLL_XZ, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_MUSICAL,
	},
  //RSC_GRID_GYM_ELEC = 10,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_WIDE = 11,
	{	//横に長い
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      3,2,
      FLDMAPPER_MODE_SCROLL_XZ, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_CROSSBLOCK_MEMSIZE,	// 1ブロックメモリサイズ(立体交差用アトリビュートサイズ込み) by iwasawa 090910
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_NOGRID_D09 = 12,
	{	//チャンピオンロード　外側
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_NoGridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_BRIDGE_H03 = 13,
	{	//H03 縦長　常駐マップ
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      1,6,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_HYBRID = 14,
	{	//グリッド、レールハイブリッド
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,3,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_HybridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_FOURKINGS = 15,
	{	//四天王部屋 グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      1,1,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_CROSSBLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_NOSCROLL = 16,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_CROSSBLOCK_MEMSIZE,	// 1ブロックメモリサイズ(立体交差用アトリビュートサイズ込み) by iwasawa 090910
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_CABIN = 17,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      5,1,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ(立体交差用アトリビュートサイズ込み) by iwasawa 090910
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

  //RSC_GRID_H04 = 18,
	{	//実験マップ グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      3,2,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      2,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_HybridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_DEF_TAILDRAW = 19,
	{ //トップ描画しないテイル描画のみ通常マップ
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,2,
      FLDMAPPER_MODE_SCROLL_XZ,
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_CROSSBLOCK_MEMSIZE,	// 1ブロックメモリサイズ(立体交差用アトリビュートサイズ込み) by iwasawa 090910
      0,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},
  //RSC_GRID_LEAGIN02 = 20,
	{	//ポケモンリーグフロント２専用　グリッド移動
		{
			MAP_XZ_SIZE, 1024*FX32_ONE, 
      2,3,
      FLDMAPPER_MODE_SCROLL_NONE, 
      ARCID_FLDMAP_LANDDATA,

			1,  1, 1,		//dummy map matrix data
			NULL, 

			FLDMAPPER_TEXTYPE_NONE,	{ 0, 0 },
			{0,2},	// 地面アニメーション

			FLD_MAPPER_BLOCK_MEMSIZE,	// 1ブロックメモリサイズ
      1,  // topフレームのブロック描画数
		},
		&FieldMapCtrl_GridFunctions,
		TRUE,
    HEAPSIZE_FIELDMAP_NORMAL,
	},

};

const unsigned int resistMapTblCount = NELEMS(resistMapTbl);


