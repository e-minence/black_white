//============================================================================================
/**
 * @file	field_func_wbnormal_file.h
 *
 *
 *
 * @brief	ＧＦマップエディタより作成したデータを処理する関数郡
 *  //ホワイトフォレスト・ブラックシティ用ランダム生成
 *
 *
 */
//============================================================================================
#include "map\dp3format.h"
//============================================================================================
/**
 * @brief	マップデータ読み込み
 */
//============================================================================================
extern BOOL FieldLoadMapData_RandomGenerate( GFL_G3D_MAP* g3Dmap, void * exWork );
//============================================================================================
/**
 * @brief	３Ｄマップ情報取得
 */
//============================================================================================
extern void FieldGetAttr_RandomGenerate( GFL_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );

extern void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId );
extern void FIELD_FUNC_RANDOM_GENERATE_TermDebug();
