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
#include "map\dp3format.h"
//============================================================================================
/**
 * @brief	マップデータ読み込み
 */
//============================================================================================
extern BOOL FieldLoadMapData_MapEditorFile( GFL_G3D_MAP* g3Dmap, void * exWork );
//============================================================================================
/**
 * @brief	３Ｄマップ情報取得
 */
//============================================================================================
extern void FieldGetAttr_MapEditorFile( GFL_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );

