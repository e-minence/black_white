//============================================================================================
/**
 * @file	field_func_wbcross_file.h
 *
 *
 *
 * @brief	ＧＦマップエディタより作成したデータを処理する関数郡
 *
 *  @auther Miyuki Iwasawa
 *  @date 09.09.09
 *
 */
//============================================================================================
#include "map\dp3format.h"
#include "../field_g3d_map.h"

//============================================================================================
/**
 * @brief	マップデータ読み込み
 */
//============================================================================================
extern BOOL FieldLoadMapData_WBCrossFile( FLD_G3D_MAP* g3Dmap, void * exWork );
//============================================================================================
/**
 * @brief	３Ｄマップ情報取得
 */
//============================================================================================
extern void FieldGetAttr_WBCrossFile( FLD_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );
extern void FieldGetAttr_WBCrossFileForEffEnc( FLD_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );

