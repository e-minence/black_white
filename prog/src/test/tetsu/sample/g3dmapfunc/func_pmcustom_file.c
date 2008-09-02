//============================================================================================
/**
 * @file	func_mapeditor_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "..\g3d_map.h"
#include "func_pmcustom_file.h"

//============================================================================================
/**
 *
 *
 *
 * @brief	ＧＦマップエディタより作成したデータを処理する関数郡
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
//------------------------------------------------------------------
//高さデータ取得用
typedef struct {
	fx16	vecN1_x;
	fx16	vecN1_y;
	fx16	vecN1_z;

	fx16	vecN2_x;
	fx16	vecN2_y;
	fx16	vecN2_z;

	fx32	vecN1_D;
	fx32	vecN2_D;

	u32		attr:31;
	u32		tryangleType:1;
}NormalVtxSt;

//ポリゴンデータ部
typedef struct SPLIT_GRID_DATA_tag{
	u16		NumX;		//グリッドＸ数
	u16		NumZ;		//グリッドＺ数

	fx32	StartX;		//始点位置Ｘ
	fx32	StartZ;		//始点位置Ｚ
	fx32	EndX;		//終点位置Ｘ
	fx32	EndZ;		//終点位置Ｚ
	fx32	SizeX;		//グリッドＸサイズ
	fx32	SizeZ;		//グリッドＺサイズ
}SPLIT_GRID_DATA;

typedef struct POLYGON_DATA_tag{
	u16 vtx_idx0;
	u16 vtx_idx1;
	u16 vtx_idx2;	//3角ポリゴンを形成する、頂点データ配列へのインデックスNo
	u16	nrm_idx;	//法線データ配列(正規化済み)へのインデックスNo
	
	fx32	paramD;		//平面の方程式から導いた、補正値D
}POLYGON_DATA;

typedef struct HEIGHT_ARRAY_tag
{
	fx32 Height;
	int Prev;
	int Next;
}HEIGHT_ARRAY;

typedef struct MAP_HEIGHT_INFO_tag{
	SPLIT_GRID_DATA	*SplitGridData;
	POLYGON_DATA	*PolygonData;
	u16				*GridDataTbl;
	u16				*LineDataTbl;
	VecFx32			*VertexArray;
	VecFx32			*NormalArray;
	u16				*PolyIDList;
//	BOOL			LoadOK;
	BOOL			DataValid;		//データ有効有無
}MAP_HEIGHT_INFO;

typedef struct {
	int VtxNum;
	int NrmNum;
	int PolygonNum;
	int GridNum;
	int TotalPolyIDListSize;
	int TotalLineListSize;
	int LineEntryMax;
}READ_INFO;

//============================================================================================
/**
 *
 *
 *
 * @brief	マップデータ読み込み
 *				※シーケンス0番はIDLING 1番はスタートＩＤＸで固定
 *
 *
 */
//============================================================================================
enum {
	L2_MDL_LOAD_START = LOAD_START,
	L2_MDL_LOAD,
	L2_TEX_LOAD_START,
	L2_TEX_LOAD,
	L2_RND_CREATE,
	L2_TEX_TRANS,
	L2_ATTR_LOAD_START,
	L2_ATTR_LOAD,
	L2_LOAD_END,
};

BOOL LoadMapData_PMcustomFile( GFL_G3D_MAP* g3Dmap )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	void*	mem;
	u32		datID;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );
	GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
#if 0
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	void*	mem;
	u32		datID;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );
	GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );

	switch( ldst->seq ){
	case L1_MDL_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);
		//モデルデータロード開始
		GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + ldst->mOffs));

		GFL_G3D_MAP_GetLoadDatIDMdl( g3Dmap, &datID );
		GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );

		ldst->seq = L1_MDL_LOAD;
		break;
	case L1_MDL_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->mdlLoaded = TRUE;
			ldst->seq = L1_TEX_LOAD_START;
		}
		break;

	case L1_TEX_LOAD_START:
		GFL_G3D_MAP_GetLoadDatIDTex( g3Dmap, &datID );

		if( datID == NON_TEX ){
			ldst->seq = L1_RND_CREATE;
		} else {
			//テクスチャロード開始
			GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + ldst->mOffs));

			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
			ldst->seq = L1_TEX_LOAD;
		}
		break;
	case L1_TEX_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->texLoaded = TRUE;
			GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//テクスチャ転送設定
			ldst->seq = L1_TEX_TRANS;
		}
		break;

	case L1_TEX_TRANS:
		if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
			ldst->seq = L1_RND_CREATE;
		}
		break;

	case L1_RND_CREATE:
		//レンダー作成
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );
		ldst->seq = L1_ATTR_LOAD_START;
		break;

	case L1_ATTR_LOAD_START:
		GFL_G3D_MAP_GetLoadDatIDAttr( g3Dmap, &datID );

		if( datID == NON_ATTR ){
			ldst->seq = L1_LOAD_END;
		} else {
		//アトリビュートデータロード開始
			GFL_G3D_MAP_CreateResourceAttr(g3Dmap, (void*)((u32)mem + ldst->mOffs));
			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
			ldst->seq = L1_ATTR_LOAD;
		}
		break;
	case L1_ATTR_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->attrLoaded = TRUE;
			ldst->seq = L1_LOAD_END;
		}
		break;

	case L1_LOAD_END:
		GFL_G3D_MAP_MakeTestPos(g3Dmap);

		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
#else
	switch( ldst->seq ){
	case L2_MDL_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		ldst->seq = L2_MDL_LOAD;
		break;

	case L2_LOAD_END:
		ldst->mdlLoaded = TRUE;
		ldst->texLoaded = TRUE;
		ldst->attrLoaded = TRUE;
		ldst->seq = LOAD_IDLING;
		return FALSE;
		break;
	}
	return TRUE;
#endif
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
void GetAttr_PMcustomFile( GFL_G3D_MAP_ATTRINFO* attrInfo, const u8* attr,
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
#if 0
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u8				triangleType;

	//グリッド内情報取得
	grid_w = map_width / MAP_GRIDCOUNT;	//マップ幅をグリッド数で分割
	grid_idx = ( posInBlock->z / grid_w ) * MAP_GRIDCOUNT + ( posInBlock->x / grid_w );
	grid_x = posInBlock->x % grid_w;
	grid_z = posInBlock->z % grid_w;

	//情報取得(軸の取り方が違うので法線ベクトルはZ反転)
	nvs = (NormalVtxSt*)((u32)attr + grid_idx * sizeof(NormalVtxSt));

	//グリッド内三角形の判定
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2のパターン
		if( grid_x + grid_z < grid_w ){
			triangleType = 0;
		} else {
			triangleType = 1;
		}
	} else {
		//2-3-0,1-0-3のパターン
		if( grid_x > grid_z ){
			triangleType = 0;
		} else {
			triangleType = 1;
		}
	}
	if( triangleType == 0 ){
		VEC_Fx16Set( &attrInfo->vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
		valD = nvs->vecN1_D;
	} else {
		VEC_Fx16Set( &attrInfo->vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
		valD = nvs->vecN2_D;
	}
	VEC_Set( &vecN, attrInfo->vecN.x, attrInfo->vecN.y, attrInfo->vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x - map_width/2) 
			+ FX_Mul(vecN.z, posInBlock->z - map_width/2) + valD );
	attrInfo->attr = nvs->attr;

	attrInfo->height = FX_Div( by, attrInfo->vecN.y ) + map_height;
#endif
}


