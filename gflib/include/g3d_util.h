//=============================================================================================
/**
 * @file	g3d_util.h                                                  
 * @brief	３Ｄ描画管理システム使用プログラム
 */
//=============================================================================================
//=============================================================================================
//	型宣言
//=============================================================================================
//=============================================================================================
/**
 *
 *
 * ３Ｄリソースおよびオブジェクトハンドルの配列管理ユーティリティー
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * セットアップ
 *
 * @param	resourceCount	管理リソース最大数	
 * @param	objectCount		管理オブジェクト最大数
 * @param	heapID			ヒープＩＤ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_UtilsysInit( u32 resourceCount, u32 objectCount, HEAPID heapID );  
//--------------------------------------------------------------------------------------------
/**
 * 破棄
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_UtilsysExit( void );  

//=============================================================================================
/**
 *
 *
 * ３Ｄリソース管理
 *
 *
 */
//=============================================================================================
typedef enum {
	GFL_G3D_UTIL_RESARC = 0,		//アーカイブＩＤを指定
	GFL_G3D_UTIL_RESPATH,			//アーカイブパスを指定

}GFL_G3D_UTIL_RESTYPE;	//指定アーカイブタイプ

//	リソース配列設定用テーブル構造体
typedef struct {
	u32						arcive;		//指定アーカイブ
	u16						datID;		//アーカイブ内データＩＤ
	GFL_G3D_UTIL_RESTYPE	arcType;	//指定アーカイブタイプ
	BOOL					trans;		//VRAM転送フラグ(テクスチャリソースの際に参照。TRUE=転送)
}GFL_G3D_UTIL_RES;

//--------------------------------------------------------------------------------------------
/**
 * リソースを配列に追加
 *
 * @param	resTable		リソース配列ポインタ
 * @param	resCount		リソース数
 *
 * @return	idx				リソース配置先頭ＩＮＤＥＸ
 */
//--------------------------------------------------------------------------------------------
extern u16 GFL_G3D_UtilResLoad( const GFL_G3D_UTIL_RES* resTable, u16 resCount ); 
//--------------------------------------------------------------------------------------------
/**
 * リソースを配列から削除
 *
 * @param	idx				リソース配置先頭ＩＮＤＥＸ
 * @param	resCount		リソース数
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_UtilResUnload( u16 idx, u16 resCount ); 
//--------------------------------------------------------------------------------------------
/**
 * リソースハンドルを配列から取得
 *
 * @param	idx				リソース格納ＩＮＤＥＸ
 *
 * @return	GFL_G3D_RES*	リソースポインタ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES* GFL_G3D_UtilResGet( u16 idx ); 
//--------------------------------------------------------------------------------------------
/**
 * テクスチャリソースを配列から転送(→ＶＲＡＭ)
 *
 * @param	idx		リソース格納ＩＮＤＥＸ
 *
 * @return	BOOL	結果(成功=TRUE)
 */
//--------------------------------------------------------------------------------------------
extern BOOL GFL_G3D_UtilVramLoadTex ( u16 idx ); 

//=============================================================================================
/**
 *
 *
 * ３Ｄオブジェクト管理
 *
 *
 */
//=============================================================================================
//	オブジェクト配列設定用テーブル構造体
typedef struct {
	u16					mdlresID;		//モデルリソースＩＮＤＥＸ
	u8					mdldatID;		//モデルデータＩＮＤＥＸ
	u16					texresID;		//テクスチャリソースＩＮＤＥＸ
	u16					anmresID;		//アニメーションリソースＩＮＤＥＸ
	u8					anmdatID;		//アニメーションデータＩＮＤＥＸ
	VecFx32				trans;			//座標
	VecFx32				scale;			//スケール
	MtxFx33				rotate;			//回転
	u8					priority;		//プライオリティー
	BOOL				drawSW;			//描画フラグ(TRUE=描画)
}GFL_G3D_UTIL_OBJ;
//--------------------------------------------------------------------------------------------
/**
 * オブジェクトを配列に追加
 *
 * @param	objTable		オブジェクト配列ポインタ
 * @param	objCount		オブジェクト数
 *
 * @return	idx				オブジェクト配置先頭ＩＮＤＥＸ
 */
//--------------------------------------------------------------------------------------------
extern u16 GFL_G3D_UtilObjLoad( const GFL_G3D_UTIL_OBJ* objTable, u16 objCount );  
//--------------------------------------------------------------------------------------------
/**
 * オブジェクトを配列から削除
 *
 * @param	idx				オブジェクト配置先頭ＩＮＤＥＸ
 * @param	objCount		オブジェクト数
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_UtilObjUnload( u16 idx, u16 objCount );
//--------------------------------------------------------------------------------------------
/**
 * オブジェクトハンドルを配列から取得
 *
 * @param	idx				オブジェクト格納ＩＮＤＥＸ
 *
 * @return	GFL_G3D_OBJ*	オブジェクトポインタ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ* GFL_G3D_UtilObjGet( u16 idx ); 

//=============================================================================================
/**
 *
 *
 * 描画管理
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの描画
 *
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_UtilDraw( void );

//--------------------------------------------------------------------------------------------
/**
 * ３Ｄオブジェクトの回転行列の作成
 *
 * @param	rotSrc	計算前の回転ベクトルポインタ
 * @param	rotDst	計算後の回転行列格納ポインタ
 *
 * この関数等を使用し、オブジェクト毎に適切な回転行列を作成したものを、描画に流す。
 */
//--------------------------------------------------------------------------------------------
// Ｘ→Ｙ→Ｚの順番で計算
extern void GFL_G3D_UtilObjDrawRotateCalcXY( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｘ→Ｙ→Ｚの順番で計算（相対）
extern void GFL_G3D_UtilObjDrawRotateCalcXY_Rev( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｙ→Ｘ→Ｚの順番で計算
extern void GFL_G3D_UtilObjDrawRotateCalcYX( VecFx32* rotSrc, MtxFx33* rotDst );

// Ｙ→Ｘ→Ｚの順番で計算（相対）
extern void GFL_G3D_UtilObjDrawRotateCalcYX_Rev( VecFx32* rotSrc, MtxFx33* rotDst );







