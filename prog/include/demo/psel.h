//============================================================================
/**
 *  @file   psel.h
 *  @brief  三匹選択
 *  @author Koji Kawada
 *  @data   2010.03.12
 *  @note   
 *
 *  モジュール名：PSEL
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>


// オーバーレイ
FS_EXTERN_OVERLAY(psel);


//=============================================================================
/**
*  定数定義
*/
//=============================================================================
// 選択結果
typedef enum
{
  PSEL_RESULT_KUSA      = 0,  // 左から順に並べておく  // この並び順はpsel.cで利用されているので重要
  PSEL_RESULT_HONOO,
  PSEL_RESULT_MIZU,
}
PSEL_RESULT;


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern const GFL_PROC_DATA    PSEL_ProcData;


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
  u16*                 evwk;          ///< [out] 選択結果(スクリプトの番号 0=くさ, 1=ほのお, 2=みず に一致させる)
  PSEL_RESULT          result;        ///< [out] 選択結果 
}
PSEL_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// パラメータ生成
//=====================================
extern PSEL_PARAM*  PSEL_AllocParam(
                                      HEAPID               heap_id,
                                      u16*                 evwk
                                  );

//-------------------------------------
/// パラメータ解放
//=====================================
extern void  PSEL_FreeParam(
                      PSEL_PARAM*  param );

//-------------------------------------
/// パラメータを設定する
//=====================================
extern void  PSEL_InitParam(
                  PSEL_PARAM*      param,
                  u16*             evwk
             );

//-------------------------------------
/// 選択結果を得る
//=====================================
extern PSEL_RESULT  PSEL_GetResult(
                  const PSEL_PARAM*      param );

