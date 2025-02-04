//============================================================================
/**
 *  @file   subway_map.h
 *  @brief  地下鉄路線図
 *  @author Koji Kawada
 *  @data   2010.03.15
 *  @note   
 *
 *  モジュール名：SUBWAY_MAP
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>


// オーバーレイ
FS_EXTERN_OVERLAY(subway_map);


//=============================================================================
/**
*  定数定義
*/
//=============================================================================


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern const GFL_PROC_DATA    SUBWAY_MAP_ProcData;


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// PROC パラメータ
//=====================================
typedef struct
{
  const MYSTATUS*      mystatus;      ///< [in] 自分状態データ(性別を使用)
}
SUBWAY_MAP_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// パラメータ生成
//=====================================
extern SUBWAY_MAP_PARAM*  SUBWAY_MAP_AllocParam(
                                      HEAPID               heap_id,
                                      const MYSTATUS*      mystatus
                                  );

//-------------------------------------
/// パラメータ解放
//=====================================
extern void  SUBWAY_MAP_FreeParam(
                      SUBWAY_MAP_PARAM*  param );

//-------------------------------------
/// パラメータを設定する
//=====================================
extern void  SUBWAY_MAP_InitParam(
                  SUBWAY_MAP_PARAM*      param,
                  const MYSTATUS*        mystatus
             );

