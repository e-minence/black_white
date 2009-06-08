//============================================================================================
/**
 * @file	field_func_wbnormal_file.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "field_func_random_generate.h"

#include "../field_buildmodel.h"
#include "../field_g3dmap_exwork.h"	// GFL_G3D_MAP拡張ワーク

//============================================================================================
/**
 *
 *
 *
 * @brief	ＧＦマップエディタより作成したデータを処理する関数郡
 *  //ホワイトフォレスト・ブラックシティ用ランダム生成
 *
 *
 */
//============================================================================================
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
	FILE_LOAD_START = GFL_G3D_MAP_LOAD_START,
	FILE_LOAD,
	RND_CREATE,
	TEX_TRANS,
};

typedef enum
{
  RMM_LEFT_TOP,
  RMM_RIGHT_TOP,
  RMM_LEFT_BOTTOM,
  RMM_RIGHT_BOTTOM,
}RANDOM_MAP_MAPPOS;

static const RANDOM_MAP_MAPPOS FieldFuncRandom_CheckMapPos( GFL_G3D_MAP* g3Dmap );

BOOL FieldLoadMapData_RandomGenerate( GFL_G3D_MAP* g3Dmap, void * exWork )
{
	GFL_G3D_MAP_LOAD_STATUS* ldst;
	FLD_G3D_MAP_EXWORK* p_exwork;	// GFL_G3D_MAP拡張ワーク

	// 拡張ワーク取得
	p_exwork = exWork;

	GFL_G3D_MAP_GetLoadStatusPointer( g3Dmap, &ldst );

	switch( ldst->seq ){

	case FILE_LOAD_START:
		GFL_G3D_MAP_ResetLoadStatus(g3Dmap);

		//モデルデータロード開始
		{
			u32		datID;
			GFL_G3D_MAP_GetLoadDatID( g3Dmap, &datID );
			GFL_G3D_MAP_StartFileLoad( g3Dmap, datID );
		}
		ldst->seq = FILE_LOAD;
		break;

	case FILE_LOAD:
		if( GFL_G3D_MAP_ContinueFileLoad(g3Dmap) == FALSE ){
			ldst->mdlLoaded = TRUE;
			ldst->texLoaded = TRUE;
			ldst->attrLoaded = TRUE;

			ldst->seq = RND_CREATE;
		}
		break;

	case RND_CREATE:
		//レンダー作成
		{
			void*				mem;
			Dp3packHeaderSt*	fileHeader;

			//ヘッダー設定
			GFL_G3D_MAP_GetLoadMemoryPointer( g3Dmap, &mem );
			fileHeader = (Dp3packHeaderSt*)mem;
			//モデルリソース設定
			GFL_G3D_MAP_CreateResourceMdl(g3Dmap, (void*)((u32)mem + fileHeader->nsbmdOffset));
			//テクスチャリソース設定
			//>>GFL_G3D_MAP_CreateResourceTex(g3Dmap, (void*)((u32)mem + fileHeader->nsbtxOffset)); 
			//配置オブジェクト設定
			if( fileHeader->positionOffset != fileHeader->endPos )
			{
				LayoutFormat* layout = (LayoutFormat*)((u32)mem + fileHeader->positionOffset);
				PositionSt* objStatus = (PositionSt*)&layout->posData;
				GFL_G3D_MAP_GLOBALOBJ_ST status;
				int i, count = layout->count;
        FIELD_BMODEL_MAN * bm = FLD_G3D_MAP_EXWORK_GetBModelMan(p_exwork);
        const RANDOM_MAP_MAPPOS mapPos = FieldFuncRandom_CheckMapPos( g3Dmap );
        u8 x,z;
        fx32 top,left;
        
        switch(mapPos)
        {
        case RMM_LEFT_TOP:
          top = FX32_CONST(-64.0f);
          left = FX32_CONST(-64.0f);
          break;

        case RMM_RIGHT_TOP:
          top = FX32_CONST(-64.0f);
          left = FX32_CONST(-192.0f);
          break;

        case RMM_LEFT_BOTTOM:
          top = FX32_CONST(-192.0f);
          left = FX32_CONST(-64.0f);
          break;

        case RMM_RIGHT_BOTTOM:
          top = FX32_CONST(-192.0f);
          left = FX32_CONST(-192.0f);
          break;
        }
				for( x=0; x<3; x++ )
				{
  				for( z=0; z<3; z++ )
  				{
  					status.id = 0;
  					VEC_Set( &status.trans, 
  							left+FX32_CONST( x*128.0f ), 0, top+FX32_CONST( z*128.0f) );
					  status.rotate = 0;
					  GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, x*3+z );
          }
        }
        /*
				for( i=0; i<count; i++ )
				{
					//status.id = FIELD_BMODEL_MAN_GetEntryIndex(bm, objStatus[i].resourceID);
					status.id = GFL_STD_MtRand0(15);
					VEC_Set( &status.trans, 
							objStatus[i].xpos, objStatus[i].ypos, -objStatus[i].zpos );
					status.rotate = (u16)(objStatus[i].rotate);
					OS_Printf("[%f:%f:%f]\n",FX_FX32_TO_F32(status.trans.x),FX_FX32_TO_F32(status.trans.y),FX_FX32_TO_F32(status.trans.z));
					//OS_Printf("bm id = %d, rotate = %04x\n",i, status.rotate);
					GFL_G3D_MAP_ResistGlobalObj( g3Dmap, &status, i );
				}
				*/
			}
		}
		//>>GFL_G3D_MAP_SetTransVramParam( g3Dmap );	//テクスチャ転送設定
		GFL_G3D_MAP_MakeRenderObj( g3Dmap );

		// 地面アニメーションの設定
		if( FLD_G3D_MAP_EXWORK_IsGranm( p_exwork ) ){
			FIELD_GRANM_WORK* p_granm;

			p_granm = FLD_G3D_MAP_EXWORK_GetGranmWork( p_exwork );
			FIELD_GRANM_WORK_Bind( p_granm, 
					GFL_G3D_MAP_GetResourceMdl(g3Dmap), GFL_G3D_MAP_GetResourceTex(g3Dmap), 
					GFL_G3D_MAP_GetRenderObj(g3Dmap) );
		}

		ldst->seq = TEX_TRANS;
		break;

	case TEX_TRANS:
		//>>if( GFL_G3D_MAP_TransVram(g3Dmap) == FALSE ){
		{
			ldst->seq = GFL_G3D_MAP_LOAD_IDLING;
			return FALSE;
		}
		break;
	}
	return TRUE;
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
void FieldGetAttr_RandomGenerate( GFL_G3D_MAP_ATTRINFO* attrInfo, const void* mapdata, 
					const VecFx32* posInBlock, const fx32 map_width, const fx32 map_height )
{
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			pos, vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	Dp3packHeaderSt* fileHeader = (Dp3packHeaderSt*)mapdata;
	u32				attrAdrs = (u32)mapdata + fileHeader->vertexOffset;

	VEC_Set( &pos, posInBlock->x + map_width/2, posInBlock->y, posInBlock->z + map_width/2 );
	//グリッド内情報取得
	grid_w = map_width / MAP_GRIDCOUNT;	//マップ幅をグリッド数で分割
	grid_idx = ( pos.z / grid_w ) * MAP_GRIDCOUNT + ( pos.x / grid_w );
	grid_x = pos.x % grid_w;
	grid_z = pos.z % grid_w;

	//情報取得(軸の取り方が違うので法線ベクトルはZ反転)
	nvs = (NormalVtxSt*)(attrAdrs + sizeof(NormalVtxFormat) + grid_idx * sizeof(NormalVtxSt));

	//グリッド内三角形の判定
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2のパターン
		if( grid_x + grid_z < grid_w ){
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	} else {
		//2-3-0,1-0-3のパターン
		if( grid_x > grid_z ){
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Fx16Set( &attrInfo->mapAttr[0].vecN, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	}
	VEC_Set( &vecN, 
			attrInfo->mapAttr[0].vecN.x, attrInfo->mapAttr[0].vecN.y, attrInfo->mapAttr[0].vecN.z );
	by = -( FX_Mul(vecN.x, posInBlock->x) + FX_Mul(vecN.z, posInBlock->z) + valD );
	attrInfo->mapAttr[0].attr = nvs->attr;

	attrInfo->mapAttr[0].height = FX_Div( by, vecN.y ) + map_height;

	attrInfo->mapAttrCount = 1;
}


//MAPの位置をチェック
static const RANDOM_MAP_MAPPOS FieldFuncRandom_CheckMapPos( GFL_G3D_MAP* g3Dmap )
{
  //仮でマップの座標が6400,2304 6912,2304 6400,2816 6912,2816 だったからそれでチェック
  
  VecFx32 pos;
  GFL_G3D_MAP_GetTrans( g3Dmap , &pos );
  OS_Printf("[%f:%f:%f]\n",FX_FX32_TO_F32(pos.x),FX_FX32_TO_F32(pos.y),FX_FX32_TO_F32(pos.z));
  if( pos.x < FX32_CONST( 256.0f+256.0f ) )
  {
    if( pos.z < FX32_CONST( 256.0f+256.0f ) )
    {
      return RMM_LEFT_TOP;
    }
    else
    {
      return RMM_LEFT_BOTTOM;
    }
  }
  else
  {
    if( pos.z < FX32_CONST( 256.0f+256.0f ) )
    {
      return RMM_RIGHT_TOP;
    }
    else
    {
      return RMM_RIGHT_BOTTOM;
    }
  }
}
