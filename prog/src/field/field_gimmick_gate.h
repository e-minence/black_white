/////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  ギミック登録関数(ゲート)
 * @file   field_gimmick_gate.h
 * @author obata
 * @date   2009.10.21
 *
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "fieldmap.h"
#include "gimmick_obj_elboard.h"


//===========================================================================
// ■ギミック登録関数
//===========================================================================
extern void GATE_GIMMICK_Setup( FIELDMAP_WORK* fieldmap );
extern void GATE_GIMMICK_End( FIELDMAP_WORK* fieldmap );
extern void GATE_GIMMICK_Move( FIELDMAP_WORK* fieldmap );


//---------------------------------------------------------------------------
/**
 * @brief 電光掲示板の向きを取得する
 *
 * @param fieldmap
 *
 * @return 電光掲示板の向き( DIR_xxxx )
 */
//---------------------------------------------------------------------------
extern u8 GATE_GIMMICK_GetElboardDir( FIELDMAP_WORK* fieldmap );


//===========================================================================
// ■カメラ
//===========================================================================

//---------------------------------------------------------------------------
/**
 * @brief カメラを電光掲示板に向ける
 *
 * @param fieldmap フィールドマップ
 * @param frame    カメラの動作に要するフレーム数
 */
//---------------------------------------------------------------------------
extern void GATE_GIMMICK_Camera_LookElboard( FIELDMAP_WORK* fieldmap, u16 frame );

//---------------------------------------------------------------------------
/**
 * @brief カメラを元に戻す
 *
 * @param fieldmap フィールドマップ
 * @param frame    カメラの動作に要するフレーム数
 */
//---------------------------------------------------------------------------
extern void GATE_GIMMICK_Camera_Reset( FIELDMAP_WORK* fieldmap, u16 frame );
