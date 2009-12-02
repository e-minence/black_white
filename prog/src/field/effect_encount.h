/**
 *  @file   effect_encount.h
 *  @brief  エンカウントエフェクト
 *  @author Miyuki Iwasawa
 *  @date   09.11.17
 */

#pragma once

////////////////////////////////////////////////////////////////////////////
//
typedef struct _TAG_EFFECT_ENCOUNT EFFECT_ENCOUNT;

/**
 *  @brief  エンカウントエフェクトワーク生成
 */
extern EFFECT_ENCOUNT* EFFECT_ENC_CreateWork( HEAPID heapID );

/*
 *  @brief  エンカウントエフェクトワーク破棄
 */
extern void EFFECT_ENC_DeleteWork( EFFECT_ENCOUNT* eff_wk );

/*
 *  @brief  エフェクトエンカウント　システム終了処理
 */
extern void EFFECT_ENC_End( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );

/*
 *  @brief  エフェクトエンカウント システム初期化
 */
extern void EFFECT_ENC_Init( FIELD_ENCOUNT* enc, EFFECT_ENCOUNT* eff_wk );



/*
 *  @brief  エンカウントエフェクト起動チェック
 */
extern void EFFECT_ENC_CheckEffectEncountStart( FIELD_ENCOUNT* enc );

/*
 *  @brief  エフェクトエンカウント  強制破棄　
 */
extern void EFFECT_ENC_EffectDelete( FIELD_ENCOUNT* enc );

/*
 *  @brief  エフェクトエンカウント　OBJとの接触によるエフェクト破棄チェック
 */
extern void EFFECT_ENC_CheckObjHit( FIELD_ENCOUNT* enc );

/**
 *  @brief  エフェクトエンカウント　マップチェンジ時の状態クリア
 */
extern void EFFECT_ENC_MapChangeUpdate( FIELD_ENCOUNT* enc );

/*
 *  @brief  エフェクトエンカウント　座標チェック
 */
extern BOOL EFFECT_ENC_CheckEffectPos( const FIELD_ENCOUNT* enc, MMDL_GRIDPOS* pos );

/*
 *  @brief  エフェクトエンカウント　イベント侵入起動チェック
 */
extern GMEVENT* EFFECT_ENC_CheckEventApproch( FIELD_ENCOUNT* enc );

/*
 *  @brief  エフェクトエンカウント　フィールド生成時エフェクト復帰強制キャンセル
 */
extern void EFFECT_ENC_EffectRecoverCancel( FIELD_ENCOUNT* enc );

/**
 * @brief   エフェクトエンカウトアイテム取得イベントで手に入れるItemNoを取得する
 * @retval itemno
 */
extern u16 EFFECT_ENC_GetEffectEncountItem( FIELD_ENCOUNT* enc );

