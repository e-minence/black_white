//==============================================================================
/**
 * @file  codein.h
 * @brief 文字入力インターフェース
 * @author  goto
 * @date  2007.07.11(水)
 *
 *  GSより移植 Ari090324
 *
 */
//==============================================================================
#ifndef __CODEIN_H__
#define __CODEIN_H__

#include "savedata/config.h"

enum{
  CODEIN_MODE_FRIEND=0, ///< ともだちコード入力
  CODEIN_MODE_TRAIN_NO, ///< Wifiバトルサブウェイ挑戦時
  CODEIN_MODE_RANK,     ///< Wifiバトルサブウェイトレーナーボードダウンロード時
  CODEIN_MODE_MAX,
};


#define CODE_BLOCK_MAX        ( 3 )   ///< 入力ブロック最大数

// -----------------------------------------
//
//  呼び出し側で管理するワーク
//
// -----------------------------------------
typedef struct {
  int     mode;                       ///< 入力モード（上画面の説明文章が変わります）CODEIN_MODE_****
  int     word_len;                   ///< 入力文字数 
  int     block[ CODE_BLOCK_MAX + 1 ];///< 入力ブロック　xx-xxxx-xxx とか
  int     end_state;                  ///< 終了時の状態
  int     max;                        ///< 入力できる数値の最大（ともだちコードの時は無視）
  STRBUF*   strbuf;                   ///< 空欄ならバッファそうでなければデフォルト値
  
} CODEIN_PARAM;

extern const GFL_PROC_DATA CodeInput_ProcData;

FS_EXTERN_OVERLAY(codein);

extern CODEIN_PARAM*  CodeInput_ParamCreate( int heap_id, int mode, int max, int word_len, int block[] );
extern void       CodeInput_ParamDelete( CODEIN_PARAM* codein_param );


//--------------------------------------------------------------
/**
 * @brief xxxx-xxxx-xxxx のブロック定義を作る
 *
 * @param block[] 
 *
 * @retval  static inline 
 *
 */
//--------------------------------------------------------------
static inline void CodeIn_BlockDataMake_4_4_4( int block[] )
{
#if 1
  block[ 0 ] = 4;
  block[ 1 ] = 4;
  block[ 2 ] = 4;
#else
  block[ 0 ] = 2;
  block[ 1 ] = 5;
  block[ 2 ] = 5;
#endif
}


//--------------------------------------------------------------
/**
 * @brief xx-xxxxx-xxxxx のブロック定義を作る
 *
 * @param block[] 
 *
 * @retval  static inline 
 *
 */
//--------------------------------------------------------------
static inline void CodeIn_BlockDataMake_2_5_5( int block[] )
{
  block[ 0 ] = 2;
  block[ 1 ] = 5;
  block[ 2 ] = 5;
}

#endif  ///< __CODEIN_H__
