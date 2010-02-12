//============================================================================
/**
 *  @file   zukan_detail_voice.h
 *  @brief  図鑑詳細画面の鳴き声画面
 *  @author Koji Kawada
 *  @data   2010.02.09
 *  @note   
 *
 *  モジュール名：ZUKAN_DETAIL_VOICE
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>

#include "zukan_detail_procsys.h"

// オーバーレイ
FS_EXTERN_OVERLAY(zukan_detail);


//=============================================================================
/**
 *  PROC
 */
//=============================================================================
extern const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_VOICE_ProcData;


//=============================================================================
/**
*  構造体宣言
*/
//=============================================================================
//-------------------------------------
/// PROC パラメータ
//=====================================
typedef struct _ZUKAN_DETAIL_VOICE_PARAM
{
  HEAPID heap_id;
}
ZUKAN_DETAIL_VOICE_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
/// パラメータを設定する
//=====================================
extern void         ZUKAN_DETAIL_VOICE_InitParam(
                            ZUKAN_DETAIL_VOICE_PARAM*  param,
                            HEAPID                     heap_id );

