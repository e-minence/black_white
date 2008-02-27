//=============================================================================================
/**
 * @file	g3d_util.h                                                  
 * @brief	３Ｄデータ管理ユーティリティープログラム
 */
//=============================================================================================
#ifndef _G3D_UTIL_H_
#define _G3D_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GFL_G3D_UTIL	GFL_G3D_UTIL;

typedef enum {
	GFL_G3D_UTIL_RESARC = 0,		//アーカイブＩＤを指定
	GFL_G3D_UTIL_RESPATH,			//アーカイブパスを指定

}GFL_G3D_UTIL_RESTYPE;	//指定アーカイブタイプ

//	リソース配列設定用テーブル構造体
typedef struct {
	const u32						arcive;		//指定アーカイブＩＤもしくはアーカイブパスポインタ
	const u16						datID;		//アーカイブ内データＩＤ
	const GFL_G3D_UTIL_RESTYPE		arcType;	//指定アーカイブタイプ
}GFL_G3D_UTIL_RES;

typedef struct {
	const u16				anmresID;	//アニメリソースＩＤ
	const u16				anmdatID;	//アニメデータＩＤ(リソース内部ＩＮＤＥＸ)
}GFL_G3D_UTIL_ANM;

typedef struct {
	const u16				mdlresID;	//モデルリソースＩＤ
	const u16				mdldatID;	//モデルデータＩＤ(リソース内部ＩＮＤＥＸ)
	const u16				texresID;	//テクスチャリソースＩＤ
	const GFL_G3D_UTIL_ANM*	anmTbl;		//アニメテーブル（複数設定のため）
	const u16				anmCount;	//アニメリソース数
}GFL_G3D_UTIL_OBJ;

typedef struct {
	const GFL_G3D_UTIL_RES*	resTbl;		//リソーステーブル
	const u16				resCount;	//リソース数

	const GFL_G3D_UTIL_OBJ*	objTbl;		//オブジェクト設定テーブル
	const u16				objCount;	//オブジェクト数

}GFL_G3D_UTIL_SETUP;

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
 * @param	scene				設定データ
 * @param	heapID				ヒープＩＤ
 *
 * @return	GFL_G3D_UTIL*		データセットハンドル
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_UTIL*
	GFL_G3D_UTIL_Create
		( const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID );

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
 * オブジェクトハンドル取得
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_UTIL_GetObjHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx );

//--------------------------------------------------------------------------------------------
/**
 * オブジェクト数取得
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetObjCount
		( GFL_G3D_UTIL* g3Dutil );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
