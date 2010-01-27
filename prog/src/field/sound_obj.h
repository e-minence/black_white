#pragma once 
///////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  音源オブジェクト
 * @file   sound_obj.h
 * @author obata
 * @date   2009.10.19
 *
 * ■概要: 以下3つの特性を持つオブジェクトを扱う構造体とその関数群
 * □ICAアニメーションで動作を管理
 * □拡張オブジェクトとして表示
 * □3Dサウンドの音源となる
 *
 */
///////////////////////////////////////////////////////////////////////////////////////////


//=========================================================================================
// ■不完全型の宣言
//=========================================================================================
typedef struct _SOUNDOBJ SOUNDOBJ;


//=========================================================================================
// ■作成・破棄
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief 音源オブジェクトを作成する
 *
 * @param fieldmap
 * @param g3dObjStatus   操作対象3Dオブジェクトのステータス
 * @param iss3dsUnitIdx  登録する3Dサウンドユニットを指定
 * @param effectiveRange 音が届く距離
 * @param maxVolume      最大ボリューム
 */
//-----------------------------------------------------------------------------------------
extern SOUNDOBJ* SOUNDOBJ_Create( FIELDMAP_WORK* fieldmap, GFL_G3D_OBJSTATUS* g3dObjStatus,
                                  ISS3DS_UNIT_INDEX iss3dsUnitIdx, 
                                  fx32 effectiveRange, int maxVolume );

//-----------------------------------------------------------------------------------------
/**
 * @brief ダミー音源オブジェクトを作成する
 *
 * @param fieldmap
 * @param g3dObjStatus 操作対象3Dオブジェクトのステータス
 *
 * ※BGMの操作を行わない音源オブジェクトを生成する。
 */
//-----------------------------------------------------------------------------------------
extern SOUNDOBJ* SOUNDOBJ_CreateDummy( FIELDMAP_WORK* fieldmap, GFL_G3D_OBJSTATUS* g3dObjStatus );

//-----------------------------------------------------------------------------------------
/**
 * @brief 音源オブジェクトを破棄する
 *
 * @param soundObj
 */
//-----------------------------------------------------------------------------------------
extern void SOUNDOBJ_Delete( SOUNDOBJ* soundObj );


//=========================================================================================
// ■初期化関数
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションを登録する
 *
 * @param arcID       設定するアニメーションデータのアーカイブID
 * @param datID       設定するアニメーションデータのアーカイブ内データID
 * @param bufInterval ストリーミング間隔
 */
//-----------------------------------------------------------------------------------------
extern void SOUNDOBJ_SetAnime( SOUNDOBJ* soundObj, ARCID arcID, ARCDATID datID, int bufInterval );


//=========================================================================================
// ■動作管理
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションを進める
 *
 * @param soundObj
 * @param frame    進めるフレーム数
 *
 * @return ループしたら TRUE
 */
//-----------------------------------------------------------------------------------------
extern BOOL SOUNDOBJ_IncAnimeFrame( SOUNDOBJ* soundObj, fx32 frame );

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションフレーム数を設定する
 *
 * @param soundObj
 * @param frame    設定するフレーム数を指定
 */
//-----------------------------------------------------------------------------------------
extern void SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* soundObj, fx32 frame );

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションフレーム数を取得する
 *
 * @param soundObj
 *
 * @return 現在のアニメーションフレーム数
 */
//-----------------------------------------------------------------------------------------
extern fx32 SOUNDOBJ_GetAnimeFrame( SOUNDOBJ* soundObj );
