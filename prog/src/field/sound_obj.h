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
// ■関数一覧
//=========================================================================================
/*
SOUNDOBJ* SOUNDOBJ_Create( 
    FIELDMAP_WORK* fieldmap, ISS_3DS_SYS* iss_sys, GFL_G3D_OBJSTATUS* status ); 
void SOUNDOBJ_Delete( SOUNDOBJ* sobj );
void SOUNDOBJ_SetAnime( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id, int buf_interval );
void SOUNDOBJ_Set3DSUnitStatus( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id );
BOOL SOUNDOBJ_IncAnimeFrame( SOUNDOBJ* sobj, fx32 frame );
void SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* sobj, fx32 frame );
u32 SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* sobj );
*/


//=========================================================================================
// ■作成・破棄
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief 音源オブジェクトを作成する
 *
 * @param fieldmap 表示先フィールドマップ
 * @param iss_sys  登録先3Dサウンドシステム
 * @param status   操作対象ステータス
 */
//-----------------------------------------------------------------------------------------
SOUNDOBJ* SOUNDOBJ_Create( 
    FIELDMAP_WORK* fieldmap, ISS_3DS_SYS* iss_sys, GFL_G3D_OBJSTATUS* status );

//-----------------------------------------------------------------------------------------
/**
 * @brief 音源オブジェクトを破棄する
 *
 * @param sobj 破棄するオブジェクト
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_Delete( SOUNDOBJ* sobj );


//=========================================================================================
// ■初期化関数
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションを登録する
 *
 * @param arc_id       設定するアニメーションデータのアーカイブID
 * @param dat_id       設定するアニメーションデータのアーカイブ内データID
 * @param buf_interval ストリーミング間隔
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnime( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id, int buf_interval );

//-----------------------------------------------------------------------------------------
/**
 * @brief 3Dサウンドユニットの設定をファイルからロードする
 *
 * @param arc_id       設定するデータのアーカイブID
 * @param dat_id       設定するデータのアーカイブ内データID
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_Set3DSUnitStatus( SOUNDOBJ* sobj, ARCID arc_id, ARCDATID dat_id );


//=========================================================================================
// ■動作管理
//=========================================================================================

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションを進める
 *
 * @param sobj  更新対象オブジェクト
 * @param frame 進めるフレーム数
 *
 * @return ループしたら TRUE
 */
//-----------------------------------------------------------------------------------------
BOOL SOUNDOBJ_IncAnimeFrame( SOUNDOBJ* sobj, fx32 frame );

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションフレーム数を設定する
 *
 * @param sobj  更新対象オブジェクト
 * @param frame フレーム数を指定
 */
//-----------------------------------------------------------------------------------------
void SOUNDOBJ_SetAnimeFrame( SOUNDOBJ* sobj, fx32 frame );

//-----------------------------------------------------------------------------------------
/**
 * @brief アニメーションフレーム数を取得する
 *
 * @param sobj 取得対象オブジェクト
 *
 * @return 現在のアニメーションフレーム数
 */
//-----------------------------------------------------------------------------------------
u32 SOUNDOBJ_GetAnimeFrame( SOUNDOBJ* sobj );
