//============================================================================
/**
 *  @file   zukan_toroku.h
 *  @brief  図鑑登録
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   ui_template.hを元に作成しました。
 *
 *  モジュール名：ZUKAN_TOROKU
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>


#include "gamesystem/gamedata_def.h"


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern const GFL_PROC_DATA ZUKAN_TOROKU_ProcData;

//=============================================================================
/**
 *  定数定義
 */
//=============================================================================
//-------------------------------------
/// 起動方法
//=====================================
typedef enum
{
  ZUKAN_TOROKU_LAUNCH_TOROKU,    ///< 図鑑登録
  ZUKAN_TOROKU_LAUNCH_NICKNAME,  ///< ニックネーム命名確認
}
ZUKAN_TOROKU_LAUNCH;

//-------------------------------------
/// 結果 
//=====================================
typedef enum
{
  ZUKAN_TOROKU_RESULT_END,       ///< 終了
  ZUKAN_TOROKU_RESULT_NICKNAME,  ///< ニックネームを付ける
}
ZUKAN_TOROKU_RESULT;

//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// PROCに渡す引数
//=====================================
typedef struct _ZUKAN_TOROKU_PARAM
{
  ZUKAN_TOROKU_LAUNCH launch;          ///< [in]  起動方法
  POKEMON_PARAM*      pp;              ///< [in]  表示するポケモン  // 他のところのを覚えているだけで生成や破棄はしない。
  BOOL                b_zenkoku_flag;  ///< [in]  TRUEのとき全国図鑑Noで表示する
  const STRBUF*       box_strbuf;      ///< [in]  !=NULLのとき、ボックスに転送しましたというメッセージを表示する
  const BOX_MANAGER*  box_manager;     ///< [in]  box_strbuf!=NULLのときのみ有効
  u32                 box_tray;        ///< [in]  box_strbuf!=NULLのときのみ有効
  GAMEDATA*           gamedata;
  ZUKAN_TOROKU_RESULT result;          ///< [out] 結果
}
ZUKAN_TOROKU_PARAM;

//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// PROCに渡す引数を生成する
//=====================================
extern ZUKAN_TOROKU_PARAM* ZUKAN_TOROKU_AllocParam( HEAPID              heap_id,
                                                    ZUKAN_TOROKU_LAUNCH launch,
                                                    POKEMON_PARAM*      pp,
                                                    BOOL                b_zenkoku_flag,
                                                    const STRBUF*       box_strbuf,
                                                    const BOX_MANAGER*  box_manager,
                                                    u32 box_tray );

//-------------------------------------
/// ZUKAN_TOROKU_AllocParamで生成したPROCに渡す引数を破棄する
//=====================================
extern void ZUKAN_TOROKU_FreeParam( ZUKAN_TOROKU_PARAM* param );

//-------------------------------------
/// PROCに渡す引数の設定
//=====================================
extern void ZUKAN_TOROKU_SetParam( ZUKAN_TOROKU_PARAM* param,
                                   ZUKAN_TOROKU_LAUNCH launch,
                                   POKEMON_PARAM*      pp,
                                   BOOL                b_zenkoku_flag,
                                   const STRBUF*       box_strbuf,
                                   const BOX_MANAGER*  box_manager,
                                   u32 box_tray );

//-------------------------------------
/// 結果を得る
//=====================================
extern ZUKAN_TOROKU_RESULT ZUKAN_TOROKU_GetResult( ZUKAN_TOROKU_PARAM* param );

