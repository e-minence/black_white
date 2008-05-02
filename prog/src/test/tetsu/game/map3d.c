//============================================================================================
/**
 * @file	map3d.c
 * @brief	マップ生成
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"
#include "calc_ph.h"

//------------------------------------------------------------------
/**
 * @brief	型宣言
 */
//------------------------------------------------------------------
#define MAP_BLOCK_COUNT	(4)
#define EXOBJ_MAX		(64)
#define EXOBJ_NULL		(0xffff)

typedef struct {
	u16	id;
	u16	count;
}MAPOBJ_HEADER;

typedef struct {
	u8	vtxY0;	
	u8	vtxY1;
	u8	vtxY2;
	u8	vtxY3;
}MAP_GRIDMAKE_DATA;

typedef struct {
	u16		type;
	VecFx16	vtx0;
	VecFx16	vtx1;
	VecFx16	vtx2;
	VecFx32	vecN;
	fx32	valD;
}MAP_PLANE_DATA;

typedef struct {
	MAP_PLANE_DATA	triangle[2];
	u32				planeType;
	u8				passage;
}MAP_GRID_DATA;

typedef struct {
	int					idx;
	MAPOBJ_HEADER		floor;
	MAP_GRID_DATA*		floorGrid;
	VecFx32				trans;
}MAP_BLOCK_DATA;

struct _SCENE_MAP {
	GFL_G3D_SCENE*		g3Dscene;
	u16					unitIdx;
	u16					resIdx;
	u16					objIdx;
	MAP_BLOCK_DATA		mapBlock[MAP_BLOCK_COUNT];
//	MAPOBJ_HEADER		floor;
//	MAP_GRID_DATA*		floorGrid;
	MAPOBJ_HEADER		extraObject[EXOBJ_MAX];
	u16					anmTimer;
};

typedef struct {
	const GFL_G3D_SCENEOBJ_DATA*	data;
	u16								dataCount;
	const char*						attr;
	const MAP_GRIDMAKE_DATA*		grid;
}MAPDATA;

typedef struct { 
	int			seq;

}EXMAPOBJ_WORK;//setup.cで指定されているサイズに収めるように注意

enum {
	QUAD_TYPE_PLANE = 0,
	QUAD_TYPE_SLIDE,
	TRIANGLE_TYPE_021_312,
	TRIANGLE_TYPE_230_103,
};

SCENE_MAP*		DEBUG_sceneMap;
#define mapScale		(FX32_ONE*1)
#define mapSizeX		(32)
#define mapSizeZ		(32)
#define mapGrid			(mapScale*16)
#define defaultMapX		(-mapGrid*(mapSizeX/2)+mapGrid/2)
#define defaultMapZ		(-mapGrid*(mapSizeZ/2)+mapGrid/2)
#define mapHeight		(FX16_ONE)

static void AddExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj, int objID, VecFx32* trans );
static void RemoveExtraObject( SCENE_MAP* sceneMap, MAPOBJ_HEADER* exobj );
static const	GFL_G3D_SCENEOBJ_DATA mapTest[4];
static const	MAPDATA extraObjectDataTbl[5];

static void	CreateMapGraphicData( SCENE_MAP* sceneMap, MAP_BLOCK_DATA* mapBlock, int mapID );
static void	DeleteMapGraphicData( SCENE_MAP* sceneMap, MAP_BLOCK_DATA* mapBlock );
static void CreateMapGridData( MAP_BLOCK_DATA* mapBlock, int mapID, HEAPID heapID );
static void DeleteMapGridData( MAP_BLOCK_DATA* mapBlock );

static void moveMapDebug( GFL_G3D_SCENEOBJ* sceneObj, void* work );
//------------------------------------------------------------------
/**
 * @brief	セットアップ
 */
//------------------------------------------------------------------
#include "graphic_data/test_graphic/test3d.naix"
#include "graphic_data/test_graphic/fld_map.naix"
#include "graphic_data/test_graphic/build_model_id.h"

enum {
	G3DRES_MAP_FLOOR = 0,
	G3DRES_EFFECT_WALL,
	G3DRES_FIELD_TEX1,
	G3DRES_FIELD_TEX2,
	G3DRES_TOWER_RED,
	G3DRES_TOWER_BLUE,
	G3DRES_TOWER_GREEN,
	G3DRES_TOWER_YELLOW,
	G3DRES_MAP_FLOOR0,
	G3DRES_MAP_FLOOR1,
	G3DRES_MAP_FLOOR2,
	G3DRES_MAP_FLOOR3,
	G3DRES_BLD_BUILD0,
	G3DRES_BLD_BUILD1,
	G3DRES_BLD_BUILD2,
	G3DRES_BLD_BUILD3,
	G3DRES_BLD_DOOR0,
	G3DRES_BLD_DOOR1,
	G3DRES_BLD_DOOR2,
	G3DRES_BLD_DOOR3,
};
//３Ｄグラフィックリソーステーブル
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_FLDMAP, NARC_fld_map_map14_16c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_test_wall_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_field_tex1_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_field_tex2_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_tower_red_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_tower_blue_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_tower_green_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_SAMPLE, NARC_haruka_tower_yellow_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map21_18c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map21_19c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map22_18c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDMAP, NARC_fld_map_map22_19c_nsbmd, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_PC, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_S01, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_C4_H01A, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_S02, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_O01, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_T5_O01B, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_C4_DOOR1, GFL_G3D_UTIL_RESARC },
	{ ARCID_FLDBLD, BMID_P_DOOR, GFL_G3D_UTIL_RESARC },
};

//---------------------
enum {
	G3DOBJ_MAP_FLOOR = 0,
	G3DOBJ_EFFECT_WALL,
	G3DOBJ_FIELD_TEX1,
	G3DOBJ_FIELD_TEX2,
	G3DOBJ_TOWER_RED,
	G3DOBJ_TOWER_BLUE,
	G3DOBJ_TOWER_GREEN,
	G3DOBJ_TOWER_YELLOW,
	G3DOBJ_MAP_FLOOR0,
	G3DOBJ_MAP_FLOOR1,
	G3DOBJ_MAP_FLOOR2,
	G3DOBJ_MAP_FLOOR3,
	G3DOBJ_BLD_BUILD0,
	G3DOBJ_BLD_BUILD1,
	G3DOBJ_BLD_BUILD2,
	G3DOBJ_BLD_BUILD3,
	G3DOBJ_BLD_DOOR0,
	G3DOBJ_BLD_DOOR1,
	G3DOBJ_BLD_DOOR2,
	G3DOBJ_BLD_DOOR3,
};
//３Ｄオブジェクト定義テーブル
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{ G3DRES_MAP_FLOOR, 0, G3DRES_MAP_FLOOR, NULL, 0 },
	{ G3DRES_EFFECT_WALL, 0, G3DRES_EFFECT_WALL, NULL, 0 },
	{ G3DRES_FIELD_TEX1, 0, G3DRES_FIELD_TEX1, NULL, 0 },
	{ G3DRES_FIELD_TEX2, 0, G3DRES_FIELD_TEX2, NULL, 0 },
	{ G3DRES_TOWER_RED, 0, G3DRES_TOWER_RED, NULL, 0 },
	{ G3DRES_TOWER_BLUE, 0, G3DRES_TOWER_BLUE, NULL, 0 },
	{ G3DRES_TOWER_GREEN, 0, G3DRES_TOWER_GREEN, NULL, 0 },
	{ G3DRES_TOWER_YELLOW, 0, G3DRES_TOWER_YELLOW, NULL, 0 },
	{ G3DRES_MAP_FLOOR0, 0, G3DRES_MAP_FLOOR0, NULL, 0 },
	{ G3DRES_MAP_FLOOR1, 0, G3DRES_MAP_FLOOR1, NULL, 0 },
	{ G3DRES_MAP_FLOOR2, 0, G3DRES_MAP_FLOOR2, NULL, 0 },
	{ G3DRES_MAP_FLOOR3, 0, G3DRES_MAP_FLOOR3, NULL, 0 },
	{ G3DRES_BLD_BUILD0, 0, G3DRES_BLD_BUILD0, NULL, 0 },
	{ G3DRES_BLD_BUILD1, 0, G3DRES_BLD_BUILD1, NULL, 0 },
	{ G3DRES_BLD_BUILD2, 0, G3DRES_BLD_BUILD2, NULL, 0 },
	{ G3DRES_BLD_BUILD3, 0, G3DRES_BLD_BUILD3, NULL, 0 },
	{ G3DRES_BLD_DOOR0, 0, G3DRES_BLD_DOOR0, NULL, 0 },
	{ G3DRES_BLD_DOOR1, 0, G3DRES_BLD_DOOR1, NULL, 0 },
	{ G3DRES_BLD_DOOR2, 0, G3DRES_BLD_DOOR2, NULL, 0 },
	{ G3DRES_BLD_DOOR3, 0, G3DRES_BLD_DOOR3, NULL, 0 },
};

//---------------------
//g3Dscene 初期設定テーブルデータ
static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl, NELEMS(g3Dutil_resTbl),
	g3Dutil_objTbl, NELEMS(g3Dutil_objTbl),
};

static const GFL_G3D_SCENEOBJ_DATA mapGraphicData0[] = {
	{	G3DOBJ_MAP_FLOOR, 0, 1, 31, FALSE, TRUE,
		{	{ 0, -FX32_ONE*80, 0 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD0, 0, 1, 31, FALSE, TRUE,
		{	{ 0x89000, 0x21800, 0x93000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD1, 0, 1, 31, FALSE, TRUE,
		{	{ 0xffff7000, 0x21800, 0xfff29000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff34000, 0x21800, 0xfff2d000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xa0000, 0x21800, 0xfff2d000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff6d000, 0x21800, 0x99000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_BUILD3, 0, 1, 31, FALSE, TRUE,
		{	{ 0, 0x1000, 0 },
			{ mapScale/2, mapScale/2, mapScale/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR0, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfffc8000, 0, 0xfff90000 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR1, 0, 1, 31, FALSE, TRUE,
		{	{ 0x28000, 0, 0xfff90000 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0x1000, 0x22000, 0xfff36000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff36000, 0x22000, 0xfff3a000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xa2000, 0x22000, 0xfff3a000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ 0xfff6f000, 0x22000, 0xa6000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
	{	G3DOBJ_BLD_DOOR3, 0, 1, 31, FALSE, TRUE,
		{	{ 0x88000, 0x22000, 0xa6000 },
			{ mapScale, mapScale/2, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},NULL,
	},
};

static const GFL_G3D_SCENEOBJ_DATA mapGraphicData1[] = {
	{	G3DOBJ_MAP_FLOOR0, 0, 1, 31, FALSE, TRUE,
		{	{ -FX32_ONE*256, -FX32_ONE*32, -FX32_ONE*256 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveMapDebug,
	},
	{	G3DOBJ_MAP_FLOOR1, 0, 1, 31, FALSE, TRUE,
		{	{ -FX32_ONE*256, -FX32_ONE*32, FX32_ONE*256 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveMapDebug,
	},
	{	G3DOBJ_MAP_FLOOR2, 0, 1, 31, FALSE, TRUE,
		{	{ FX32_ONE*256, -FX32_ONE*32, -FX32_ONE*256 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveMapDebug,
	},
	{	G3DOBJ_MAP_FLOOR3, 0, 1, 31, FALSE, TRUE,
		{	{ FX32_ONE*256, -FX32_ONE*32, FX32_ONE*256 },
			{ mapScale, mapScale, mapScale },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveMapDebug,
	},
};

static const char mapAttr0[] = {
"■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"	//16
"■◆◆◆◆２□□□□□２◆◆◆◆◆◆◆２２２２２◆◆◆◆■■■■"	//
"■◆◆◆◆２□□□□□２◆◆◆◆◆◆◆２２２２２◆◆◆◆■■■■"	//
"■◆◆◆◆２□□□□□２◆◆◆◆◆◆◆２２２２２◆◆◆◆■■■■"	//
"■２２２２２２２２２２２２２２２２２２２２２２２２２２２■■■■"	//
"■２２２２２２２２２２２２２２２２２２２２２２２２２２２■■■■"	//
"■２２２２２２２２２２２２２２２２２２２２２２２２２２２■■■■"	//
"■２２２２２２２２２２２２２２２２２２２２２２２２２２２■■■■"	//
"■２２２２２２２２■■□□□■■■□□□■■２２２２２２■■■■"	//
"■２２２２２２２２■■□□□■□■□□□■■２２２２２２■■■■"	//
"■２２２２■■■■■■０００００００００■■■■■■２２■■■■"	//
"■２２２２■１１１１■０００００００００■１１１１■２２■■■■"	//
"■２２２２■１■■■■０００００００００■■■■１■２２■■■■"	//
"■２２２２■１■０００００００００００００００■１■２２■■■■"	//
"２２２２２■１■０００００００００００００００■１■２２■■■■"	//
"２２２２２■１→００００００◆◆◆００００００←１■２２■■■■"	//
"２２２２２■１→００００００◆◆◆００００００←１■２２２２２２"	//
"２２２２２■１→００００００◆◆◆００００００←１■２２２２２２"	//
"２２２２２■１■０００００００００００００００■１■２２２２２２"	//
"■■■２２■１■０００００００００００００００■１■２２２２２２"	//
"■■■２２■１■■■■０００００００００■■■■１■２２２２２２"	//
"■■■２２■１１１１■０００００００００■１１１１■２２２■■■"	//
"■■■２２■■■■■■０００００００００■■■■■■２２２■■■"	//
"■■■２２２２２２■■■■↑↑↑↑↑■■■■◆◆◆◆◆２２■■■"	//
"■■■２２◆◆◆◆■■■■↑↑↑↑↑■■■■◆◆◆◆◆２２■■■"	//
"■■■２２◆◆◆◆□□□□２２２２２２２２２◆◆◆◆◆２２■■■"	//
"■■■２２◆◆◆◆□□□□□２２２２２２２２◆◆◆◆◆２２■■■"	//
"■■■２２２２２２□□□□□２２２２２２２２２２２２２２２■■■"	//
"■■■２２２２２２２□□□□２２２２２２２２２２２２２２２■■■"	//
"■■■２２２２２２２２２２２２２２２２２２２２２２２２２２■■■"	//
"■■■■■２２２２２２２２２２２２２２２２２２２２２２２２■■■"	//
"■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"	//
};

static const MAP_GRIDMAKE_DATA mapGrid0[] = {
//0//
{3,3,3,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},
{3,3,3,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},
{3,3,3,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},
{3,3,3,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//1//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//2//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//3//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//4//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//5//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//6//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//7//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//8//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},
{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,1},{2,2,1,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//9//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,1,2,1},{1,1,1,0},{1,1,0,0},{1,1,0,0},{1,1,0,0},{1,1,0,0},{1,1,0,0},
{1,1,0,0},{1,1,0,0},{1,1,0,0},{1,1,0,0},{1,1,0,0},{1,2,1,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//10//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,1},{2,2,1,1},{2,2,1,1},
{2,2,1,1},{2,1,1,1},{1,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},{1,2,1,1},{2,2,1,1},{2,2,1,1},
{2,2,1,1},{2,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//11//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,1,1,1},
{1,1,1,1},{1,1,1,1},{1,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},{1,1,1,1},{1,1,1,1},{1,1,1,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//12//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,1,1,0},
{1,1,0,0},{1,1,0,0},{1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,0},{1,1,0,0},{1,1,0,0},{1,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//13//
{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//14//
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},
//15//
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,3,2,2},{3,3,2,2},{3,3,2,2},{3,3,2,2},
//16//
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
//17//
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
//18//
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
//19//
{2,2,3,3},{2,2,3,3},{2,2,3,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
//20//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,0,1,1},
{0,0,1,1},{0,0,1,1},{0,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,1},{0,0,1,1},{0,0,1,1},{0,1,1,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
//21//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,1,2,1},{1,1,1,1},{1,1,1,1},
{1,1,1,1},{1,1,1,1},{1,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},{1,1,1,1},{1,1,1,1},{1,1,1,1},
{1,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,3},{2,2,3,3},{2,2,3,3},
//22//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,1,2,2},{1,1,2,2},{1,1,2,2},
{1,1,2,2},{1,1,2,1},{1,0,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,1,0,1},{1,1,1,2},{1,1,2,2},{1,1,2,2},
{1,1,2,2},{1,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//23//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,1,2,1},{1,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},
{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,1,1,1},{1,2,1,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//24//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},
{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,1,2,2},{1,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//25//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//26//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//27//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//28//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//29//
{3,3,3,3},{3,3,3,3},{3,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//30//
{3,3,3,3},{3,3,3,3},{3,2,3,3},{2,2,3,3},{2,2,3,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},
{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,2,2,2},{2,3,2,3},{3,3,3,3},{3,3,3,3},
//31//
{3,3,3,3},{3,3,3,3},{3,3,3,3},{3,3,3,3},{3,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},
{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},
{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},
{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,2,3,3},{2,3,3,3},{3,3,3,3},{3,3,3,3},
};

static const MAPDATA mapData[] = {
	{ mapGraphicData0, NELEMS(mapGraphicData0), mapAttr0, mapGrid0 },
	{ mapGraphicData1, NELEMS(mapGraphicData1), mapAttr0, mapGrid0 },
};

static const VecFx32 mapTransOffs[] = {
	{ -FX32_ONE*256, 0, -FX32_ONE*256 },
	{  FX32_ONE*256, 0, -FX32_ONE*256 },
	{ -FX32_ONE*256, 0,  FX32_ONE*256 },
	{  FX32_ONE*256, 0,  FX32_ONE*256 },
};

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ生成
 */
//------------------------------------------------------------------
SCENE_MAP*	Create3Dmap( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SCENE_MAP* sceneMap = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCENE_MAP) );
	int	i;

	//３Ｄデータセットアップ
	sceneMap->g3Dscene = g3Dscene;
	sceneMap->unitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3Dutil_setup );
	sceneMap->resIdx = GFL_G3D_SCENE_GetG3DutilUnitResIdx( g3Dscene, sceneMap->unitIdx );
	sceneMap->objIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, sceneMap->unitIdx );

	//マップ作成
	{
		int mapID = 0;

		for( i=0; i<MAP_BLOCK_COUNT; i++ ){
			sceneMap->mapBlock[i].idx = i;
			CreateMapGraphicData( sceneMap, &sceneMap->mapBlock[i], mapID );
			CreateMapGridData( &sceneMap->mapBlock[i], mapID, heapID );
			sceneMap->mapBlock[i].trans.x = mapTransOffs[i].x;
			sceneMap->mapBlock[i].trans.y = mapTransOffs[i].y;
			sceneMap->mapBlock[i].trans.z = mapTransOffs[i].z;
			{
				GFL_G3D_SCENEOBJ* g3DsceneObj;
				VecFx32 trans;
				int j;

				for( j=0; j<sceneMap->mapBlock[i].floor.count; j++ ){
					g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneMap->g3Dscene, 
													sceneMap->mapBlock[i].floor.id + j );
					GFL_G3D_SCENEOBJ_GetPos( g3DsceneObj, &trans );
					VEC_Add( &trans, &sceneMap->mapBlock[i].trans, &trans );
					GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, &trans );
				}
			}
		}
	}

	for( i=0; i<EXOBJ_MAX; i++ ){
		sceneMap->extraObject[i].id = EXOBJ_NULL;
		sceneMap->extraObject[i].count = 0;
	}
	sceneMap->anmTimer = 0;

	DEBUG_sceneMap = sceneMap;
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
	for( i=0; i<MAP_BLOCK_COUNT; i++ ){
		DeleteMapGridData( &sceneMap->mapBlock[i] );
		DeleteMapGraphicData( sceneMap, &sceneMap->mapBlock[i] );
	}
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
 * @brief	３Ｄマップ生成メイン
 */
//------------------------------------------------------------------
static inline u16 GET_MAPCODE( const u16* mapdata, int x, int z )
{
	u16	tmpdata;

	if(( x < 0 )||( x >= mapSizeX )||( z < 0 )||( z >= mapSizeZ )){
		return  '■';
	}
	tmpdata = mapdata[ z * mapSizeX + x ];
	return  (( tmpdata & 0x00ff ) << 8 ) + (( tmpdata & 0xff00 ) >> 8 );
}

//------------------------------------------------------------------
/**
 * @brief		３Ｄマップデータセットアップ
 */
//------------------------------------------------------------------
static void	CreateMapGraphicData( SCENE_MAP* sceneMap, MAP_BLOCK_DATA* mapBlock, int mapID )
{
	GFL_G3D_SCENEOBJ_DATA*	pdata;

	GF_ASSERT( mapID < NELEMS(mapData) );

	mapBlock->floor.count = mapData[mapID].dataCount;
	mapBlock->floor.id = GFL_G3D_SCENEOBJ_Add(	sceneMap->g3Dscene, 
												mapData[mapID].data,
												mapData[mapID].dataCount,
												sceneMap->objIdx );
}

//------------------------------------------------------------------
static void	DeleteMapGraphicData( SCENE_MAP* sceneMap, MAP_BLOCK_DATA* mapBlock )
{
	GFL_G3D_SCENEOBJ_Remove(	sceneMap->g3Dscene, 
								mapBlock->floor.id, 
								mapBlock->floor.count ); 
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

	exobj->count = extraObjectDataTbl[objID].dataCount;
	exobj->id = GFL_G3D_SCENEOBJ_Add(	sceneMap->g3Dscene, 
										extraObjectDataTbl[objID].data,
										exobj->count, 
										sceneMap->objIdx );

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

static void moveMapDebug( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	VecFx32	trans;
	VecFx32	vecMove = { 0, 0, 0 };
	BOOL mvf = FALSE;

	if( GFL_UI_KEY_CheckCont( PAD_BUTTON_A ) == TRUE ){
		vecMove.y = -FX32_ONE;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_BUTTON_B ) == TRUE ){
		vecMove.y = FX32_ONE;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_UP ) == TRUE ){
		vecMove.z = -FX32_ONE;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_DOWN ) == TRUE ){
		vecMove.z = FX32_ONE;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_LEFT ) == TRUE ){
		vecMove.x = -FX32_ONE;
		mvf = TRUE;
	} else if( GFL_UI_KEY_CheckCont( PAD_KEY_RIGHT ) == TRUE ){
		vecMove.x = FX32_ONE;
		mvf = TRUE;
	}
	if( mvf == TRUE ){
		GFL_G3D_SCENEOBJ_GetPos( sceneObj, &trans );
		VEC_Add( &trans, &vecMove, &trans );
		GFL_G3D_SCENEOBJ_SetPos( sceneObj, &trans );
		OS_Printf("mvpos  {%x,%x,%x}\n", trans.x, trans.y, trans.z );
	}
}

//------------------------------------------------------------------
/**
 * @brief		データ
 */
//------------------------------------------------------------------
static const GFL_G3D_SCENEOBJ_DATA extraObject1[] = {
	{	G3DOBJ_EFFECT_WALL, 0, 1, 8, TRUE, TRUE,
		{	{ 0, -FX32_ONE*64, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const GFL_G3D_SCENEOBJ_DATA towerRed[] = {
	{	G3DOBJ_TOWER_RED, 0, 1, 8, TRUE, TRUE,
		{	{ 0, 0, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const GFL_G3D_SCENEOBJ_DATA towerBlue[] = {
	{	G3DOBJ_TOWER_BLUE, 0, 1, 8, TRUE, TRUE,
		{	{ 0, 0, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const GFL_G3D_SCENEOBJ_DATA towerGreen[] = {
	{	G3DOBJ_TOWER_GREEN, 0, 1, 8, TRUE, TRUE,
		{	{ 0, 0, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const GFL_G3D_SCENEOBJ_DATA towerYellow[] = {
	{	G3DOBJ_TOWER_YELLOW, 0, 1, 8, TRUE, TRUE,
		{	{ 0, 0, 0 },
			{ FX32_ONE, FX32_ONE, FX32_ONE },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveExtraObject,
	},
};

static const	MAPDATA extraObjectDataTbl[5] = {
	{ extraObject1, NELEMS(extraObject1) },
	{ towerRed, NELEMS(towerRed) },
	{ towerBlue, NELEMS(towerBlue) },
	{ towerGreen, NELEMS(towerGreen) },
	{ towerYellow, NELEMS(towerYellow) },
};

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ高さデータ作成
 */
//------------------------------------------------------------------
static void MakeGridData( MAP_GRID_DATA* floor, int gridx, int gridz, 
							int y0, int y1, int y2, int y3 );
static void MakeTriangleData
		( MAP_PLANE_DATA* triData, VecFx32* posRef, VecFx16* vtx0, VecFx16* vtx1, VecFx16* vtx2 );
static BOOL GetPlaneVtxHeight( u16 mapCode, int* y );

static void CreateMapGridData( MAP_BLOCK_DATA* mapBlock, int mapID, HEAPID heapID )
{
	int	i, x, z;
	u16	mapCode;
	int	height, height2;
	const u16* attr = (const u16*)mapData[mapID].attr;
	const MAP_GRIDMAKE_DATA* grid = mapData[mapID].grid;

	mapBlock->floorGrid = GFL_HEAP_AllocClearMemory( heapID, 
												sizeof(MAP_GRID_DATA)*mapSizeX*mapSizeZ );

	for( z=0; z<mapSizeZ; z++ ){
		for( x=0; x<mapSizeX; x++ ){
			i = z * mapSizeX + x;

			mapCode = GET_MAPCODE( attr, x, z );

			MakeGridData( mapBlock->floorGrid, x, z, 
					grid[i].vtxY0, grid[i].vtxY1, grid[i].vtxY2, grid[i].vtxY3 );

			switch( mapCode ){
			default:
			case '０':
			case '１':
			case '２':
			case '３':
			case '→':	//階段
			case '←':	//階段
			case '↑':	//階段
			case '↓':	//階段
				mapBlock->floorGrid[i].passage = 0;
				mapBlock->floorGrid[i].passage = 0;
				break;
			case '■':	//崖
			case '◆':	//家
			case '□':	//水
				mapBlock->floorGrid[i].passage = 1;
				break;
			}
		}
	}
}

static void DeleteMapGridData( MAP_BLOCK_DATA* mapBlock )
{
	GFL_HEAP_FreeMemory( mapBlock->floorGrid );
}

static void	GetGroundGridData( const VecFx32* pos, int* blockID, int* x, int* z, u16* offset )
{
	if((pos->x < 0 )&&(pos->z < 0 )){
		*blockID = 0;
		*x = (pos->x + mapSizeX*mapGrid)/mapGrid;
		*z = (pos->z + mapSizeZ*mapGrid)/mapGrid;
	} else if((pos->x >= 0 )&&(pos->z < 0 )){
		*blockID = 1;
		*x = pos->x/mapGrid;
		*z = (pos->z + mapSizeZ*mapGrid)/mapGrid;
	} else if((pos->x < 0 )&&(pos->z >= 0 )){
		*blockID = 2;
		*x = (pos->x + mapSizeX*mapGrid)/mapGrid;
		*z = pos->z/mapGrid;
	} else {
		*blockID = 3;
		*x = pos->x/mapGrid;
		*z = pos->z/mapGrid;
	}
	//GF_ASSERT_MSG( *x >= 0, "posx = %x, x = %x\n", pos->x, *x );
	//GF_ASSERT_MSG( *x < mapSizeX, "posx = %x, x = %x\n", pos->x, *x );
	//GF_ASSERT_MSG( *z >= 0, "posz = %x, z = %x\n", pos->z, *z );
	//GF_ASSERT_MSG( *z < mapSizeZ, "posz = %x, z = %x\n", pos->z, *z );
	*offset = *z * mapSizeX + *x;
}

//------------------------------------------------------------------
static void inline posGridCalc( int x, int z, VecFx16* vtx, VecFx32* dst )
{
	VecFx32	tmp;

	tmp.x = (x - mapSizeX/2) * FX16_ONE + vtx->x;
	tmp.y = vtx->y;
	tmp.z = (z - mapSizeZ/2) * FX16_ONE + vtx->z;
	//tmp.x = x * FX16_ONE + vtx->x;
	//tmp.y = vtx->y;
	//tmp.z = z * FX16_ONE + vtx->z;

	dst->x = FX_Mul( mapGrid, tmp.x );
	dst->y = FX_Mul( mapGrid, tmp.y );
	dst->z = FX_Mul( mapGrid, tmp.z );
}

static void MakeGridData( MAP_GRID_DATA* floor, int gridx, int gridz, 
							int y0, int y1, int y2, int y3 )
{
	MAP_GRID_DATA* grid = &floor[ gridz * mapSizeX + gridx ];
	VecFx32	posRef;
	VecFx16	vtx0, vtx1, vtx2, vtx3;

	//法線ベクトル算出用、頂点方向データ作成（描画にも使える）
	vtx0.x = 0;
	vtx0.y = y0 * mapHeight;
	vtx0.z = 0;

	vtx1.x = FX16_ONE;
	vtx1.y = y1 * mapHeight;
	vtx1.z = 0;

	vtx2.x = 0;
	vtx2.y = y2 * mapHeight;
	vtx2.z = FX16_ONE;

	vtx3.x = FX16_ONE;
	vtx3.y = y3 * mapHeight;
	vtx3.z = FX16_ONE;

	if(( y0 == y3 )&&( y1 == y2 )&&( y0 == y1 )){
		grid->planeType = QUAD_TYPE_PLANE;
	} else if( y0 == y3 ){
		grid->planeType = TRIANGLE_TYPE_230_103;
	} else if( y1 == y2 ){
		grid->planeType = TRIANGLE_TYPE_021_312;
	} else {
		grid->planeType = QUAD_TYPE_SLIDE;
	}
	//データ格納(法線が上の場合は左回りに頂点設定)
	if( grid->planeType == TRIANGLE_TYPE_021_312 ){
		//0-2-1 triangle設定
		posGridCalc( gridx, gridz, &vtx0, &posRef );	//vtx0のローカル実座標計算
		MakeTriangleData( &grid->triangle[0], &posRef, &vtx0, &vtx2, &vtx1 );
		//3-1-2 triangle設定
		posGridCalc( gridx, gridz, &vtx3, &posRef );	//vtx3のローカル実座標計算
		MakeTriangleData( &grid->triangle[1], &posRef, &vtx3, &vtx1, &vtx2 );
	} else {
		//2-3-0 triangle設定
		posGridCalc( gridx, gridz, &vtx2, &posRef );	//vtx2のローカル実座標計算
		MakeTriangleData( &grid->triangle[0], &posRef, &vtx2, &vtx3, &vtx0 );
		//1-0-3 triangle設定
		posGridCalc( gridx, gridz, &vtx1, &posRef );	//vtx1のローカル実座標計算
		MakeTriangleData( &grid->triangle[1], &posRef, &vtx1, &vtx0, &vtx3 );
	}
}

//------------------------------------------------------------------
static void MakeTriangleData
		( MAP_PLANE_DATA* triData, VecFx32* posRef, VecFx16* vtx0, VecFx16* vtx1, VecFx16* vtx2 )
{
	VecFx16	vec0, vec1, vecN;

	triData->vtx0 = *vtx0;
	triData->vtx1 = *vtx1;
	triData->vtx2 = *vtx2;

	VEC_Fx16Subtract( vtx1, vtx0, &vec0 );
	VEC_Fx16Subtract( vtx2, vtx0, &vec1 );
	VEC_Fx16CrossProduct( &vec0, &vec1, &vecN );
	VEC_Fx16Normalize( &vecN, &vecN );

	VEC_Set( &triData->vecN, (fx32)vecN.x, (fx32)vecN.y, (fx32)vecN.z );

	triData->valD = -(	  FX_Mul( triData->vecN.x, posRef->x ) 
						+ FX_Mul( triData->vecN.y, posRef->y ) 
						+ FX_Mul( triData->vecN.z, posRef->z ) ); 
}

//------------------------------------------------------------------
static BOOL GetPlaneVtxHeight( u16 mapCode, int* y )
{
	switch( mapCode ){
	case '０':
	case '１':
	case '２':
	case '３':
		*y = mapCode - '０';
		return TRUE;
	}
	*y = 0;
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップ高さデータ取得
 */
//------------------------------------------------------------------
static BOOL	CheckGetGroundOutSideData( const int x, const int z )
{
	if(( x < 0 )||( x >= mapSizeX )||( z < 0 )||( z >= mapSizeZ ) ){ 
		return TRUE;
	} else {
		return FALSE;
	}
}

static void	GetGroundTriangleID( MAP_GRID_DATA* floor, int gridx, int gridz, fx32 fx, fx32 fz, 
									u16* gridOffs, u16* ID )
{
	//グリッド内三角形の判定
	if( floor[ *gridOffs ].planeType == TRIANGLE_TYPE_021_312 ){
		//0-2-1,3-1-2のパターン
		if( fx + fz < mapGrid ){
			*ID = 0;
		} else {
			*ID = 1;
		}
	} else {
		//2-3-0,1-0-3のパターン
		if( fx < fz ){
			*ID = 0;
		} else {
			*ID = 1;
		}
	}
}

BOOL	CheckGroundOutRange( SCENE_MAP* sceneMap, const VecFx32* pos )
{
	int	gridx, gridz;
	u16	gridOffs;
	int blockID;

	GetGroundGridData( pos, &blockID, &gridx, &gridz, &gridOffs );

	if( CheckGetGroundOutSideData( gridx, gridz ) == TRUE ){
		return FALSE;
	}
	if( sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].passage != 0 ){
		return FALSE;
	}
	return TRUE;
}

BOOL	GetGroundPlaneData( SCENE_MAP* sceneMap, const VecFx32* pos, VecFx32* vecN, fx32* valD )
{
	int		gridx, gridz;
	fx32	fx, fz;
	u16		gridOffs, triangleID;
	int		blockID;

	GetGroundGridData( pos, &blockID, &gridx, &gridz, &gridOffs );

	if( CheckGetGroundOutSideData( gridx, gridz ) == TRUE ){
		return FALSE;
	}
	fx = pos->x%mapGrid;
	fz = pos->z%mapGrid;

	GetGroundTriangleID( sceneMap->mapBlock[blockID].floorGrid, 
							gridx, gridz, fx, fz, &gridOffs, &triangleID );

	*vecN	= sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].triangle[ triangleID ].vecN;
	*valD	= sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].triangle[ triangleID ].valD;

	return TRUE;
}

void	GetGroundPlaneVecN( SCENE_MAP* sceneMap, const VecFx32* pos, VecFx32* vecN )
{
	int		gridx, gridz;
	fx32	fx, fz;
	u16		gridOffs, triangleID;
	int		blockID;

	GetGroundGridData( pos, &blockID, &gridx, &gridz, &gridOffs );

	if( CheckGetGroundOutSideData( gridx, gridz ) == TRUE ){
		vecN->x = 0;
		vecN->y = FX32_ONE;
		vecN->z = 0;
		return;
	}
	fx = pos->x%mapGrid;
	fz = pos->z%mapGrid;

	GetGroundTriangleID( sceneMap->mapBlock[blockID].floorGrid, 
							gridx, gridz, fx, fz, &gridOffs, &triangleID );

	*vecN = sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].triangle[ triangleID ].vecN;
}

void	GetGroundPlaneHeight( SCENE_MAP* sceneMap, const VecFx32* pos, fx32* height ) 
{
	int		gridx, gridz;
	fx32	fx, fz;
	u16		gridOffs, triangleID;
	int		blockID;
	VecFx32 posTmp, vecN;
	fx32	by, valD;

	GetGroundGridData( pos, &blockID, &gridx, &gridz, &gridOffs );

	if( CheckGetGroundOutSideData( gridx, gridz ) == TRUE ){
		*height = 0;
		return;
	}
	fx = pos->x%mapGrid;
	fz = pos->z%mapGrid;

	GetGroundTriangleID( sceneMap->mapBlock[blockID].floorGrid, 
							gridx, gridz, fx, fz, &gridOffs, &triangleID );

	vecN = sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].triangle[ triangleID ].vecN;
	valD = sceneMap->mapBlock[blockID].floorGrid[ gridOffs ].triangle[ triangleID ].valD;

	VEC_Subtract( pos, &sceneMap->mapBlock[blockID].trans, &posTmp );
	by = -( FX_Mul( vecN.x, posTmp.x ) + FX_Mul( vecN.z, posTmp.z ) + valD );
	*height = FX_Div( by, vecN.y );
}

void	GetGroundMoveVec
	( SCENE_MAP* sceneMap, const VecFx32* pos, const VecFx32* vecDir, VecFx32* vecMove )
{
	VecFx32 vecN, vecH, vecV;

	//平面の法線ベクトルにより移動ベクトルに垂直で斜面に並行なベクトルを算出
	GetGroundPlaneVecN( sceneMap, pos, &vecN );				//平面の法線を取得
	VEC_CrossProduct( &vecN, vecDir, &vecH );		//平面上の水平ベクトル算出
	VEC_CrossProduct( &vecN, &vecH, &vecV );		//平面上の斜面ベクトル算出

	if( VEC_DotProduct( &vecV, vecDir ) < 0 ){
		//逆方向へのベクトルが出てしまった場合修正
		VEC_Set( vecMove, -vecV.x, -vecV.y, -vecV.z );
	} else {
		VEC_Set( vecMove, vecV.x, vecV.y, vecV.z );
	}
	VEC_Normalize( vecMove, vecMove );
}

void	GetGroundMovePos
	( SCENE_MAP* sceneMap, const VecFx32* posNow, const VecFx32* vecMoveXZ, VecFx32* posNext ) 
{
	VecFx32	vecMove;

	//移動方向の地形に沿った単位ベクトル取得
	GetGroundMoveVec( sceneMap, posNow, vecMoveXZ, &vecMove );
	//移動ベクトル取得
	VEC_Mult( VEC_Mag( vecMoveXZ ), &vecMove, &vecMove );
	//移動位置の計算
	VEC_Add( posNow, &vecMove, posNext );
	GetGroundPlaneHeight( sceneMap, posNext, &posNext->y );
}

//------------------------------------------------------------------
//仮。いずれ消す
BOOL	DEBUG_CheckGroundOutRange( const VecFx32* pos )
{
	return CheckGroundOutRange( DEBUG_sceneMap, pos );
}
BOOL	DEBUG_CheckGroundMove( const VecFx32* posNow, const VecFx32* vecMove, VecFx32* posNext )
{
	GetGroundMovePos( DEBUG_sceneMap, posNow, vecMove, posNext ); 
	//範囲外コントロール（本当は外部関数でやる）
	return CheckGroundOutRange( DEBUG_sceneMap, posNext );
}
void	GLOBAL_GetGroundPlaneVecN( const VecFx32* pos, VecFx32* vecN )
{
	GetGroundPlaneVecN( DEBUG_sceneMap, pos, vecN );
}

void	GLOBAL_GetGroundPlaneHeight( const VecFx32* pos, fx32* height )
{
	GetGroundPlaneHeight( DEBUG_sceneMap, pos, height );
	//*height = FX32_ONE*40;
}

//------------------------------------------------------------------
/**
 * @brief	３Ｄマップとレイとの当たり座標取得
 */
//------------------------------------------------------------------
static BOOL checkOnTriangle( VecFx32* pos, 
		VecFx32* posRef0, VecFx32* posRef1, VecFx32* posRef2, VecFx32* vecN )
{
	VecFx32 vecEdge, vecVtx, vecTmp;
	fx32	scalar0, scalar1, scalar2;
			
	//一定回りに頂点を結び符号を確認する
	VEC_Subtract( pos, posRef0, &vecVtx );
	VEC_Subtract( posRef1, posRef0, &vecEdge );
	VEC_CrossProduct( &vecVtx, &vecEdge, &vecTmp );
	scalar0 = VEC_DotProduct( &vecTmp, vecN );

	VEC_Subtract( pos, posRef1, &vecVtx );
	VEC_Subtract( posRef2, posRef1, &vecEdge );
	VEC_CrossProduct( &vecVtx, &vecEdge, &vecTmp );
	scalar1 = VEC_DotProduct( &vecTmp, vecN );

	VEC_Subtract( pos, posRef2, &vecVtx );
	VEC_Subtract( posRef0, posRef2, &vecEdge );
	VEC_CrossProduct( &vecVtx, &vecEdge, &vecTmp );
	scalar2 = VEC_DotProduct( &vecTmp, vecN );

	if(( scalar0 > 0 )&&( scalar1 > 0 )&&( scalar2 > 0 )){
		return TRUE;
	}
	if(( scalar0 < 0 )&&( scalar1 < 0 )&&( scalar2 < 0 )){
		return TRUE;
	}
	return FALSE;
}

BOOL GetRayPosOnMap
	( SCENE_MAP* sceneMap, const VecFx32* posRay, const VecFx32* vecRay, VecFx32* dst )
{
	VecFx32 vecRayXZ, vecLength, posStart, pos;
	fx32	limitLength;
	int		x, z;
	u16		offset, prevOffset; 
	int		blockID, prevBlockID;

	//レイの方向成分をXZ方向だけ抜き出して正規化
	VEC_Set( &vecRayXZ, vecRay->x, 0, vecRay->z );
	VEC_Normalize( &vecRayXZ, &vecRayXZ );

	VEC_Set( &posStart, posRay->x, 0, posRay->z );		//検出開始地点
	prevBlockID = 0xffff;	//検出初期化
	prevOffset = 0xffff;	//検出初期化
	vecLength.x = 0;
	vecLength.y = 0;
	vecLength.z = 0;
	{
		fx32 x_x = FX_Mul( mapSizeX * FX32_ONE, mapSizeX * FX32_ONE ); 
		fx32 z_z = FX_Mul( mapSizeZ * FX32_ONE, mapSizeZ * FX32_ONE ); 
		fx32 xz = FX_Sqrt( x_x + z_z );

		limitLength = FX_Mul( mapGrid, xz );
	}
	while( VEC_Mag( &vecLength ) <= limitLength ){
		VEC_Add( &posStart, &vecLength, &pos );

		GetGroundGridData( &pos, &blockID, &x, &z, &offset );
	
		if( CheckGetGroundOutSideData( x, z ) == FALSE ){

			if(( blockID != prevBlockID )||( offset != prevOffset )){	//同じデータはパス
				MAP_GRID_DATA*	grid = &sceneMap->mapBlock[blockID].floorGrid[offset];
				VecFx32 posRef0, posRef1, posRef2, vecN, posCalc;
				VecFx32* transOffs =  &sceneMap->mapBlock[blockID].trans;
				BOOL result;
				int i;

				for( i=0; i<2; i++ ){
					//グリッド内三角形１が作る平面との交差判定
					posGridCalc( x, z, &grid->triangle[i].vtx0, &posRef0 );
					posGridCalc( x, z, &grid->triangle[i].vtx1, &posRef1 );
					posGridCalc( x, z, &grid->triangle[i].vtx2, &posRef2 );
					vecN = grid->triangle[i].vecN;
					VEC_Add( &posRef0, transOffs, &posRef0 );
					VEC_Add( &posRef1, transOffs, &posRef1 );
					VEC_Add( &posRef2, transOffs, &posRef2 );
	
					result = GFL_G3D_Calc_GetClossPointRayPlane
								( posRay, vecRay, &posRef0, &vecN, &posCalc, 0 );
					if( result == GFL_G3D_CALC_TRUE ){
						if( checkOnTriangle(&posCalc, &posRef0, &posRef1, &posRef2, &vecN)==TRUE ){
							*dst = posCalc;
							//OS_Printf("pos {%x,%x,%x}, blockID %x, gridx %x, gridz %x\n", 
							//			pos.x, pos.y, pos.z, blockID, x, z );
							return TRUE;
						}
					}
				}
				prevBlockID = blockID;
				prevOffset = offset;
			}
		}
		VEC_Add( &vecLength, &vecRayXZ, &vecLength );
	}
	//レンジオーバーで検出失敗
	//意味はないが念のためdstに問題が発生しないような値を入れる
	dst->x = 0;
	dst->y = 0;
	dst->z = 0;
	return FALSE;
}


