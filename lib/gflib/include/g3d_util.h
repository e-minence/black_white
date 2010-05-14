#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================================
/**
 * @file	g3d_util.h                                                  
 * @brief	３Ｄデータ管理ユーティリティープログラム
 */
//=============================================================================================
#ifndef _G3D_UTIL_H_
#define _G3D_UTIL_H_

typedef struct _GFL_G3D_UTIL	GFL_G3D_UTIL;

typedef enum {
	GFL_G3D_UTIL_RESARC = 0,		//アーカイブＩＤを指定
	GFL_G3D_UTIL_RESPATH,			//アーカイブパスを指定
  GFL_G3D_UTIL_RESARC_UTILHEAP,		//アーカイブＩＤを指定　ユーティリティのヒープを使用
  GFL_G3D_UTIL_RESPATH_UTILHEAP,			//アーカイブパスを指定 ユーティリティのヒープを使用

}GFL_G3D_UTIL_RESTYPE;	//指定アーカイブタイプ

//	リソース配列設定用テーブル構造体
typedef struct {
	u32						arcive;		//指定アーカイブＩＤもしくはアーカイブパスポインタ
	u16						datID;		//アーカイブ内データＩＤ
	GFL_G3D_UTIL_RESTYPE		arcType;	//指定アーカイブタイプ
}GFL_G3D_UTIL_RES;

typedef struct {
	u16				anmresID;	//アニメリソースＩＤ
	u16				anmdatID;	//アニメデータＩＤ(リソース内部ＩＮＤＥＸ)
}GFL_G3D_UTIL_ANM;

typedef struct {
	u16				mdlresID;	//モデルリソースＩＤ
	u16				mdldatID;	//モデルデータＩＤ(リソース内部ＩＮＤＥＸ)
	u16				texresID;	//テクスチャリソースＩＤ
	const GFL_G3D_UTIL_ANM*	anmTbl;		//アニメテーブル（複数設定のため）
	u16				anmCount;	//アニメリソース数
}GFL_G3D_UTIL_OBJ;

typedef struct {
	const GFL_G3D_UTIL_RES*	resTbl;		//リソーステーブル
	u16				resCount;	//リソース数

	const GFL_G3D_UTIL_OBJ*	objTbl;		//オブジェクト設定テーブル
	u16				objCount;	//オブジェクト数

}GFL_G3D_UTIL_SETUP;

#define GFL_G3D_UTIL_NORESOURCE_ID (0xff)
//=============================================================================================
/**
 *
 *
 * ３Ｄ関連ハンドルの管理ユーティリティー
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * セットアップ
 *
 * @param	resCountMax			設定リソース最大数定
 * @param	objCountMax			設定オブジェクト最大数
 * @param	heapID				ヒープＩＤ
 *
 * @return	GFL_G3D_UTIL*		データセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_UTIL*
	GFL_G3D_UTIL_Create
		( u16 resCountMax, u16 objCountMax, HEAPID heapID );
//--------------------------------------------------------------------------------------------
/**
 * 破棄
 *
 * @param	GFL_G3D_UTIL*		データセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_UTIL_Delete
		( GFL_G3D_UTIL* g3Dutil );
//--------------------------------------------------------------------------------------------
/**
 * ユニットセットアップ
 *
 * @param	GFL_G3D_UTIL*		データセットハンドル
 * @param	setup				設定データ
 *
 * @return	unitIdx				ユニットインデックス
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_AddUnit
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup );

extern u16
	GFL_G3D_UTIL_AddUnitResShare
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup );

extern u16
	GFL_G3D_UTIL_AddUnitResShareByHandle
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup, ARCHANDLE *handle );
    
//--------------------------------------------------------------------------------------------
/**
 * ユニット破棄
 *
 * @param	GFL_G3D_UTIL*		データセットハンドル
 * @param	unitIdx				ユニットインデックス
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_UTIL_DelUnit
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ユニットリソース先頭インデックス取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitResIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ユニットリソース数取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitResCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ユニットオブジェクト先頭インデックス取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitObjIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ユニットオブジェクト数取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitObjCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * リソースハンドル取得
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_UTIL_GetResHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx );
//--------------------------------------------------------------------------------------------
/**
 * オブジェクトハンドル取得
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_UTIL_GetObjHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx );
//--------------------------------------------------------------------------------------------
/**
 * 総オブジェクト数取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetObjCount
		( GFL_G3D_UTIL* g3Dutil );
//--------------------------------------------------------------------------------------------
/**
 * オブジェクトにアニメーションを追加設定する（別ユニットのアニメを設定するなど）
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_UTIL_SetObjAnotherUnitAnime
		( GFL_G3D_UTIL* g3Dutil, u16 objUnitIdx, u16 objIdx, 
			u16 anmUnitIdx, const GFL_G3D_UTIL_ANM* anmTbl, u16 anmCount );

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
