#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================================
/**
 * @file	g3d_scene.h
 * @brief	３Ｄシーン管理プログラム
 */
//=============================================================================================
#ifndef _G3D_SCENE_H_
#define _G3D_SCENE_H_

//=============================================================================================
//	型宣言
//=============================================================================================
typedef struct _GFL_G3D_SCENE		GFL_G3D_SCENE;
typedef struct _GFL_G3D_SCENEOBJ	GFL_G3D_SCENEOBJ;	
typedef void						(GFL_G3D_SCENEOBJFUNC)( GFL_G3D_SCENEOBJ*, void* );

typedef struct {
	u32						objID;
	u8						movePriority;				
	u8						drawPriority;				
	u8						blendAlpha;
	BOOL					cullingFlag;
	BOOL					drawSW;
	GFL_G3D_OBJSTATUS		status; 
	GFL_G3D_SCENEOBJFUNC*	func;
}GFL_G3D_SCENEOBJ_DATA;

typedef struct {
	u32						objID;
	u8						blendAlpha;
	GFL_G3D_OBJSTATUS		status; 
	int						jntID;
}GFL_G3D_SCENEACCESORY_DATA;

#define GFL_G3D_SCENEOBJ_ALPHA_OFF	( 255 )
//=============================================================================================
/**
 * 管理システム
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * システム作成
 *
 * @param	g3Dutil			依存するg3Dutil
 * @param	SceneObjMax		配置可能なオブジェクト数
 * @param	SceneObjWkSiz	１オブジェクトに割り当てるワークのサイズ
 * @param	GFL_PTC_Enable	パーティクルシステムの起動フラグ
 * @param	heapID			ヒープＩＤ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_SCENE*
	GFL_G3D_SCENE_Create
		( GFL_G3D_UTIL* g3Dutil, const u16 sceneObjMax, const u32 sceneObjWkSiz, 
			const u16 sceneAccesoryMax, BOOL GFL_PTC_Enable, const HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * システムメイン（動作）
 *
 * @param	g3Dscene		システムポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Main
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * システムメイン（描画）
 *
 * @param	g3Dscene		システムポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Draw
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * システム破棄
 *
 * @param	g3Dscene		システムポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_Delete
		( GFL_G3D_SCENE* g3Dscene );  

//--------------------------------------------------------------------------------------------
/**
 * 依存しているg3Dutilの逆引き
 *
 * @param	g3Dscene		システムポインタ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_UTIL*
	GFL_G3D_SCENE_GetG3Dutil
		( GFL_G3D_SCENE* g3Dscene );

//--------------------------------------------------------------------------------------------
/**
 * G3Dutil追加セットアップ用インライン
 *
 * @param	g3Dscene		システムポインタ
 * @param	setup			セットアップ配列
 * @param	unitIdx			ユニットＩＮＤＥＸ
 * @param	objIdx			オブジェクトＩＮＤＥＸ
 */
//--------------------------------------------------------------------------------------------
inline u16	GFL_G3D_SCENE_AddG3DutilUnit
		( GFL_G3D_SCENE* g3Dscene, const GFL_G3D_UTIL_SETUP* setup )
{
	return GFL_G3D_UTIL_AddUnit( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), setup );
}

inline void	GFL_G3D_SCENE_DelG3DutilUnit
		( GFL_G3D_SCENE* g3Dscene, u16 unitIdx )
{
	GFL_G3D_UTIL_DelUnit( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), unitIdx );
}

inline u16	GFL_G3D_SCENE_GetG3DutilUnitResIdx
		( GFL_G3D_SCENE* g3Dscene, u16 unitIdx )
{
	return GFL_G3D_UTIL_GetUnitResIdx( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), unitIdx );
}

inline u16	GFL_G3D_SCENE_GetG3DutilUnitResCount
		( GFL_G3D_SCENE* g3Dscene, u16 unitIdx )
{
	return GFL_G3D_UTIL_GetUnitResCount( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), unitIdx );
}

inline u16	GFL_G3D_SCENE_GetG3DutilUnitObjIdx
		( GFL_G3D_SCENE* g3Dscene, u16 unitIdx )
{
	return GFL_G3D_UTIL_GetUnitObjIdx( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), unitIdx );
}

inline u16	GFL_G3D_SCENE_GetG3DutilUnitObjCount
		( GFL_G3D_SCENE* g3Dscene, u16 unitIdx )
{
	return GFL_G3D_UTIL_GetUnitObjCount( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), unitIdx );
}

inline GFL_G3D_RES*	GFL_G3D_SCENE_GetG3DutilResHandle
		( GFL_G3D_SCENE* g3Dscene, u16 resIdx )
{
	return GFL_G3D_UTIL_GetResHandle( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), resIdx );
}

inline GFL_G3D_OBJ*	GFL_G3D_SCENE_GetG3DutilObjHandle
		( GFL_G3D_SCENE* g3Dscene, u16 objIdx )
{
	return GFL_G3D_UTIL_GetObjHandle( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ), objIdx );
}

inline u16	GFL_G3D_SCENE_GetG3DutilObjCount
		( GFL_G3D_SCENE* g3Dscene )
{
	return GFL_G3D_UTIL_GetObjCount( GFL_G3D_SCENE_GetG3Dutil( g3Dscene ) );
}

//--------------------------------------------------------------------------------------------
/**
 * 配置オブジェクトポインタをＩＮＤＥＸより取得
 *
 * @param	g3Dscene		システムポインタ
 * @param	idx				オブジェクト配置ＩＮＤＥＸ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_SCENEOBJ*
	GFL_G3D_SCENEOBJ_Get
		( GFL_G3D_SCENE* g3Dscene, u32 idx );

//--------------------------------------------------------------------------------------------
/**
 * パーティクル描画のＯＮ/ＯＦＦ(パーティクルシステムが起動されている場合のみ有効)
 *
 * @param	BOOL			パーティクル描画スイッチ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENE_SetDrawParticleSW
		( GFL_G3D_SCENE* g3Dscene, BOOL sw );


//=============================================================================================
/**
 * 配置オブジェクト
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * 作成
 *
 * @param	g3Dscene		システムポインタ
 * @param	sceneObjTbl		配置オブジェクト設定データ
 * @param	sceneObjCount	配置オブジェクト数
 * @param	objIdxOffset	参照オブジェクトＩＮＤＥＸオフセット
 *
 * @return	idx				配置オブジェクト先頭ＩＮＤＥＸ
 */
//--------------------------------------------------------------------------------------------
extern u32
	GFL_G3D_SCENEOBJ_Add
		( GFL_G3D_SCENE* g3Dscene, const GFL_G3D_SCENEOBJ_DATA* sceneObjTbl, 
			const u16 sceneObjCount, u16 objIdxOffset );

//--------------------------------------------------------------------------------------------
/**
 * 破棄
 *
 * @param	g3Dscene		システムポインタ
 * @param	idx				配置オブジェクト先頭ＩＮＤＥＸ
 * @param	sceneObjCount	配置オブジェクト数
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENEOBJ_Remove
		( GFL_G3D_SCENE* g3Dscene, u32 idx, const u16 sceneObjCount );

//--------------------------------------------------------------------------------------------
/**
 * オブジェクトハンドル(G3D_OBJ)の取得
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_SCENEOBJ_GetG3DobjHandle
		( GFL_G3D_SCENEOBJ* g3DsceneObj );

//--------------------------------------------------------------------------------------------
/**
 * ユーザーワークエリアポインタの取得
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 */
//--------------------------------------------------------------------------------------------
extern void*
	GFL_G3D_SCENEOBJ_GetWork
		( GFL_G3D_SCENEOBJ* g3DsceneObj );

//--------------------------------------------------------------------------------------------
/**
 * オブジェクトＩＤの取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	objID			オブジェクトＩＤの格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetObjID( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID );
extern void GFL_G3D_SCENEOBJ_SetObjID( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID );

//--------------------------------------------------------------------------------------------
/**
 * 描画プライオリティーの取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	drawPri			描画プライオリティーの格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetDrawPri( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri );
extern void GFL_G3D_SCENEOBJ_SetDrawPri( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri );

//--------------------------------------------------------------------------------------------
/**
 * αブレンド値の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	blendAlpha		αブレンドの格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetBlendAlpha( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha );
extern void GFL_G3D_SCENEOBJ_SetBlendAlpha( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha );

//--------------------------------------------------------------------------------------------
/**
 * 描画スイッチの取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	drawSW			描画スイッチの格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetDrawSW( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW );
extern void GFL_G3D_SCENEOBJ_SetDrawSW( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（位置情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	trans			位置情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetPos( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans );
extern void GFL_G3D_SCENEOBJ_SetPos( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（スケール情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	scale			スケール情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetScale( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale );
extern void GFL_G3D_SCENEOBJ_SetScale( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（回転情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	rotate			回転情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetRotate( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate );
extern void GFL_G3D_SCENEOBJ_SetRotate( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate );

//--------------------------------------------------------------------------------------------
/**
 * 動作関数の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	scale			スケール情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_GetFunc( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func );
extern void GFL_G3D_SCENEOBJ_SetFunc( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func );

//--------------------------------------------------------------------------------------------
/**
 * アニメーション用インライン
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	anmIdx			登録されているアニメーションインデックス
 */
//--------------------------------------------------------------------------------------------
inline BOOL GFL_G3D_SCENEOBJ_EnableAnime
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx )
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_EnableAnime( g3Dobj, anmIdx );
}

inline BOOL GFL_G3D_SCENEOBJ_DisableAnime
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx )
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_DisableAnime( g3Dobj, anmIdx );
}

inline BOOL GFL_G3D_SCENEOBJ_ResetAnimeFrame
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx )
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_ResetAnimeFrame( g3Dobj, anmIdx );
}

inline BOOL GFL_G3D_SCENEOBJ_GetAnimeFrame
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx, int* anmFrm )
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_GetAnimeFrame( g3Dobj, anmIdx, anmFrm );
}

inline BOOL GFL_G3D_SCENEOBJ_SetAnimeFrame
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx, int* anmFrm )
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_SetAnimeFrame( g3Dobj, anmIdx, anmFrm );
}

inline BOOL GFL_G3D_SCENEOBJ_IncAnimeFrame
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx, const fx32 count ) 
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_IncAnimeFrame( g3Dobj, anmIdx, count );
}

inline BOOL GFL_G3D_SCENEOBJ_LoopAnimeFrame
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 anmIdx, const fx32 count ) 
{
	GFL_G3D_OBJ* g3Dobj = GFL_G3D_SCENEOBJ_GetG3DobjHandle( g3DsceneObj );
	return GFL_G3D_OBJECT_LoopAnimeFrame( g3Dobj, anmIdx, count );
}

//--------------------------------------------------------------------------------------------
/**
 *
 * アクセサリー関連
 *　破棄に関しては配置オブジェクトを削除した際にも自動で消える。
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * アクセサリー登録
 *
 * @param	g3DsceneObj			配置オブジェクトポインタ
 * @param	accesoryTbl			アクセサリー設定データテーブルポインタ
 * @param	accesoryCount		アクセサリー数
 *
 * @return	BOOL
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_SCENEOBJ_ACCESORY_Add
		( GFL_G3D_SCENEOBJ* g3DsceneObj, const GFL_G3D_SCENEACCESORY_DATA* accesoryTbl, 
			const u16 accesoryCount );

//--------------------------------------------------------------------------------------------
/**
 * 破棄
 *
 * @param	g3DsceneObj			配置オブジェクトポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENEOBJ_ACCESORY_Remove
		( GFL_G3D_SCENEOBJ* g3DsceneObj );

//--------------------------------------------------------------------------------------------
/**
 * オブジェクトＩＤの取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	accesoryIdx		アクセサリーインデックス
 * @param	objID			オブジェクトＩＤの格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_ACCESORY_GetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, u16* objID );
extern void GFL_G3D_SCENEOBJ_ACCESORY_SetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, u16* objID );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（位置情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	accesoryIdx		アクセサリーインデックス
 * @param	trans			位置情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_SCENEOBJ_ACCESORY_GetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* trans );
extern void
	GFL_G3D_SCENEOBJ_ACCESORY_SetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* trans );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（スケール情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	accesoryIdx		アクセサリーインデックス
 * @param	scale			スケール情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_ACCESORY_GetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* scale );
extern void GFL_G3D_SCENEOBJ_ACCESORY_SetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* scale );

//--------------------------------------------------------------------------------------------
/**
 * ステータス（回転情報）の取得と変更
 *
 * @param	g3DsceneObj		配置オブジェクトポインタ
 * @param	accesoryIdx		アクセサリーインデックス
 * @param	rotate			回転情報の格納もしくは参照ワークポインタ
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_SCENEOBJ_ACCESORY_GetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, MtxFx33* rotate );
extern void
	GFL_G3D_SCENEOBJ_ACCESORY_SetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, MtxFx33* rotate );





#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
