/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ギミック登録関数 ( H01 )
 * @file   field_gimmick_h01.h
 * @author obata
 * @date   2009.10.21
 */
/////////////////////////////////////////////////////////////////////////////
#pragma once

//------------------------------------------------------------------------------------------
/**
 * @brief 初期化関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void H01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief 終了関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void H01_GIMMICK_End( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief 動作関数
 *
 * @param fieldmap ギミック動作フィールドマップ
 */
//------------------------------------------------------------------------------------------
extern void H01_GIMMICK_Move( FIELDMAP_WORK* fieldmap );
