//============================================================================================
/**
 * @file	field_status_local.h
 * @brief フィールドマップ管理情報
 * @author  tamada GAMEFREAK inc.
 * @date    2009.10.03
 *
 * @note
 * field_status.hとfield_status_local.hとは対になっている。
 * field_status.hは、FIELD_STATUSというオブジェクトをメンバに持ったり、
 * 取得したりするためだけのヘッダ。（現状gamedata.hのみを想定）
 * field_status_local.hは、FIELD_STATUSの操作関数が含まれるヘッダ。
 * src/field以下のソースファイルのみでインクルードされることを想定している。
 */
//============================================================================================
#pragma once

#include "field/field_status.h"

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * @briefマップモード
 */
//------------------------------------------------------------------
typedef enum{
  MAPMODE_NORMAL,     ///<通常状態
  MAPMODE_INTRUDE,    ///<侵入中
}MAPMODE;

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern MAPMODE FIELD_STATUS_GetMapMode(const FIELD_STATUS * fldstatus);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void FIELD_STATUS_SetMapMode(FIELD_STATUS * fldstatus, MAPMODE map_mode);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_STATUS_SetFieldInitFlag( FIELD_STATUS * fldstatus, BOOL flag );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern BOOL FIELD_STATUS_GetFieldInitFlag( const FIELD_STATUS * fldstatus );


//------------------------------------------------------------------
// フィールド技　マップ効果状態
// field/fieldskill_mapeff.h
//  FIELDSKILL_MAPEFF_MSK
//------------------------------------------------------------------
extern void FIELD_STATUS_SetFieldSkillMapEffectMsk( FIELD_STATUS * fldstatus, u32 msk );
extern u32 FIELD_STATUS_GetFieldSkillMapEffectMsk( const FIELD_STATUS * fldstatus );
