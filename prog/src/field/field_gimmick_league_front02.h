////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ギミック処理(ポケモンリーグフロント02)
 * @file   field_gimmick_league_front.h
 * @author obata
 * @date   2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "field/fieldmap_proc.h"


//==========================================================================================
// ■拡張オブジェクト
//==========================================================================================
//----------------------------------------
// アニメーション(リフト)
//----------------------------------------
typedef enum {
  LIFT_ANM_TA, // テクスチャアニメーション
  LIFT_ANM_NUM
} LIFT_ANM_INDEX;
//----------------------------------------
// アニメーション(リフトの稼動エフェクト)
//----------------------------------------
typedef enum {
  LIFT_EFFECT_ANM_TA, // テクスチャアニメーション
  LIFT_EFFECT_ANM_MA, // マテリアルアニメーション
  LIFT_EFFECT_ANM_NUM
} LIFT_EFFECT_ANM_INDEX;
//-------------
// オブジェクト
//-------------
typedef enum {
  LF02_EXOBJ_LIFT,         // リフト
  LF02_EXOBJ_LIFT_EFFECT,  // リフトの稼動エフェクト
  LF02_EXOBJ_NUM
} OBJ_INDEX;
//----------
// ユニット
//----------
typedef enum {
  LF02_EXUNIT_GIMMICK,
  LF02_EXUNIT_NUM
} UNIT_INDEX;


//==========================================================================================
// ■ギミック登録関数
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_Setup( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief 終了関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_End( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief 動作関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_Move( FIELDMAP_WORK* fieldmap );


//==========================================================================================
// ■ギミック操作関数
//==========================================================================================

extern void LEAGUE_FRONT_02_GIMMICK_StartLiftAnime( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_StopLiftAnime( FIELDMAP_WORK* fieldmap );
extern BOOL LEAGUE_FRONT_02_GIMMICK_CheckLiftAnimeEnd( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_ShowLiftEffect( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_HideLiftEffect( FIELDMAP_WORK* fieldmap );
extern BOOL LEAGUE_FRONT_02_GIMMICK_CheckLiftEffectEnd( FIELDMAP_WORK* fieldmap );
