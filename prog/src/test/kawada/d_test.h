//============================================================================
/**
 *  @file   d_test.h
 *  @brief  テスト
 *  @author Koji Kawada
 *  @data   2010.03.31
 *  @note   
 *
 *  モジュール名：D_KAWADA_TEST, D_TEST
 */
//============================================================================
#pragma once


// インクルード
#include <gflib.h>


// オーバーレイ
FS_EXTERN_OVERLAY(kawada_debug);


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  進化デモカメラ
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


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
extern const GFL_PROC_DATA    D_KAWADA_TEST_ShinkaProcData;


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
  u32                  dummy;         ///< [in] ダミー
}
D_KAWADA_TEST_SHINKA_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  タマゴ孵化デモカメラ
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


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
extern const GFL_PROC_DATA    D_KAWADA_TEST_EggProcData;


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
  u32                  dummy;         ///< [in] ダミー
}
D_KAWADA_TEST_EGG_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================




//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
/**
*  図鑑フォルムカメラ
*/
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================


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
extern const GFL_PROC_DATA    D_KAWADA_TEST_ZukanFormProcData;


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
  u8                  mode;         ///< [in] 0=center; 1=left; 2=right;
}
D_KAWADA_TEST_ZUKAN_FORM_PARAM;


//=============================================================================
/**
*  関数のプロトタイプ宣言
*/
//=============================================================================


