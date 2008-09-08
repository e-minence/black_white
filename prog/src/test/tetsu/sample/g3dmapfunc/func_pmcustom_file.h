//============================================================================================
/**
 *
 *
 *
 * @brief	ポケモンＧＳで使用しているデータを処理する関数郡
 *
 *
 *
 */
//============================================================================================
//============================================================================================
/**
 * @brief	マップデータ読み込み
 */
//============================================================================================
extern BOOL LoadMapData_PMcustomFile( GFL_G3D_MAP* g3Dmap );
//============================================================================================
/**
 * @brief	３Ｄマップ情報取得
 */
//============================================================================================
extern void GetAttr_PMcustomFile( GFL_G3D_MAP_ATTRINFO* attrInfo,
									const void* mapdata, const VecFx32* posInBlock, 
									const fx32 map_width, const fx32 map_height );

