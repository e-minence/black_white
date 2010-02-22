/////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  育て屋ワーク
 * @file   sodateya_work.c
 * @author obata
 * @date   2009.10.01
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "poke_tool/pokeparty.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"
#include "savedata/sodateya_work.h"


//===========================================================================================
// ■ 定数
//===========================================================================================
#define MAX_EXP (0xffffffff) // 経験値の最大値

//===========================================================================================
// ■ 飼育ポケモン一体分のデータ
//===========================================================================================
typedef struct 
{
  BOOL           valid;  // 有効フラグ
  POKEMON_PARAM  param;  // パラメータ
  u32        growUpExp;  // 成長経験値
} SODATEYA_POKE_DATA;

//===========================================================================================
// ■ タマゴのデータ
//===========================================================================================
typedef struct
{
  BOOL           valid;  // 有効フラグ
} SODATEYA_EGG_DATA;

//===========================================================================================
// ■ 育て屋ワーク
//===========================================================================================
struct _SODATEYA_WORK
{
  SODATEYA_POKE_DATA poke[SODATEYA_POKE_MAX]; // 飼育ポケモン
  SODATEYA_EGG_DATA  egg;                     // タマゴ
};


//===========================================================================================
// ■ ポケモン, タマゴの取得・設定・削除
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief タマゴを設定する
 *
 * @param work 育て屋ワーク
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_SetEgg( SODATEYA_WORK* work )
{
  if( work->egg.valid == TRUE )
  {
    OBATA_Printf( "------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_SetEgg: すでにタマゴが存在します。\n" );
    OBATA_Printf( "------------------------------------------------\n" );
    return;
  }
  work->egg.valid = TRUE;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief タマゴを削除する
 *
 * @param work 育て屋ワーク
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_ClrEgg( SODATEYA_WORK* work )
{
  if( work->egg.valid != TRUE )
  {
    OBATA_Printf( "------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_ClrEgg: タマゴがありません。\n" );
    OBATA_Printf( "------------------------------------------\n" );
  }
  work->egg.valid = FALSE; 
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンを取得する
 *
 * @param work  育て屋ワーク
 * @param index 対象ポケモンを指定
 * @return 指定したポケモンのパラメータ
 */
//-------------------------------------------------------------------------------------------
const POKEMON_PARAM* SODATEYA_WORK_GetPokemon( const SODATEYA_WORK* work, int index )
{
  // インデックスの有効判定
  if( (index < 0) || 
      (SODATEYA_POKE_MAX <= index) ||
      (work->poke[index].valid != TRUE) )
  {
    OBATA_Printf( "--------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_GetPokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "--------------------------------------------------------------\n" );
    index = 0;
  }

  return &(work->poke[index].param);
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンを設定する
 *
 * @param work  育て屋ワーク
 * @param index 格納先インデックスを指定
 * @param poke  設定するポケモンのパラメータ
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_SetPokemon( SODATEYA_WORK* work, int index, const POKEMON_PARAM* poke )
{
  // インデックスの有効判定
  if( (index < 0) || 
      (SODATEYA_POKE_MAX <= index) ||
      (work->poke[index].valid == TRUE) )
  {
    OBATA_Printf( "--------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_SetPokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "--------------------------------------------------------------\n" );
    return;
  }

  // 飼育ポケモンをセット
  work->poke[index].param     = *poke;
  work->poke[index].valid     = TRUE;
  work->poke[index].growUpExp = 0;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンを削除する
 *
 * @param work  育て屋ワーク
 * @param index 対象ポケモンを指定
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_ClrPokemon( SODATEYA_WORK* work, int index )
{
  // インデックスの有効判定
  if( (index < 0) || 
      (SODATEYA_POKE_MAX <= index) ||
      (work->poke[index].valid != TRUE) )
  {
    OBATA_Printf( "--------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_ClrPokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "--------------------------------------------------------------\n" );
    return;
  }

  // 指定ポケモンを削除
  work->poke[index].valid = FALSE;
}


//===========================================================================================
// ■ 有効かどうかの判定
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief タマゴの有無を調べる
 *
 * @param work 育て屋ワーク
 * @return タマゴがある場合 TRUE
 */
//-------------------------------------------------------------------------------------------
BOOL SODATEYA_WORK_IsValidEgg( const SODATEYA_WORK* work )
{
  return work->egg.valid;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの有無を調べる
 *
 * @param work  育て屋ワーク
 * @param index 調べるインデックス
 * @return ポケモンがいる場合 TRUE
 */
//-------------------------------------------------------------------------------------------
BOOL SODATEYA_WORK_IsValidPokemon( const SODATEYA_WORK* work, int index )
{
  // インデックスの有効判定
  if( (index < 0) || (SODATEYA_POKE_MAX <= index) )
  {
    OBATA_Printf( "------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_IsValidPokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "------------------------------------------------------------------\n" );
    return FALSE;
  }

  return work->poke[index].valid;
}


//===========================================================================================
// ■ 取得関数
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの数を取得する
 *
 * @param work 育て屋ワーク
 * @return 飼育ポケモンの数
 */
//-------------------------------------------------------------------------------------------
int SODATEYA_WORK_GetPokemonNum( const SODATEYA_WORK* work )
{
  int i;
  int num = 0;

  // 飼育ポケモンの数を数える
  for( i=0; i<SODATEYA_POKE_MAX; i++ )
  {
    if( work->poke[i].valid == TRUE ) num++;
  }
  return num;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの成長経験値を取得する
 *
 * @param work  育て屋ワーク
 * @param index 対象ポケモンを指定
 * @return 指定した飼育ポケモンの成長経験値
 */
//-------------------------------------------------------------------------------------------
u32 SODATEYA_WORK_GetGrowUpExp( const SODATEYA_WORK* work, int index )
{
  // インデックスの有効判定
  if( (index < 0) || (SODATEYA_POKE_MAX <= index) )
  {
    OBATA_Printf( "------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_IsValidPokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "------------------------------------------------------------------\n" );
    return 0;
  }

  return work->poke[index].growUpExp;
}


//===========================================================================================
// ■ 設定関数
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンを並べ替える
 *
 * @param work   育て屋ワーク
 * @param index1 入れ替える飼育ポケモンのインデックス
 * @param index2 入れ替える飼育ポケモンのインデックス
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_ExchangePokemon( SODATEYA_WORK* work, int index1, int index2 )
{
  SODATEYA_POKE_DATA temp;

  // インデックスの有効判定
  if( (index1 < 0) || (SODATEYA_POKE_MAX <= index1) ||
      (index2 < 0) || (SODATEYA_POKE_MAX <= index2) )
  {
    OBATA_Printf( "-------------------------------------------------------------------\n" );
    OBATA_Printf( "SODATEYA_WORK_ExchangePokemon: インデックス指定に誤りがあります。。\n" );
    OBATA_Printf( "-------------------------------------------------------------------\n" );
    return;
  }

  // 入れ替える
  temp               = work->poke[index1];
  work->poke[index1] = work->poke[index2];
  work->poke[index2] = temp;
}

//-------------------------------------------------------------------------------------------
/**
 * @brief 成長経験値を増やす
 *
 * @param work 育て屋ワーク
 * @param exp  加算する経験値
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_AddGrowUpExp( SODATEYA_WORK* work, u32 exp )
{
  int i;

  // 有効な飼育ポケモンデータのみ, 経験値を加算する
  for( i=0; i<SODATEYA_POKE_MAX; i++ )
  {
    if( (work->poke[i].valid == TRUE) )
    {
      // 加算後の経験値を最大値で補正
      u64 addedExp = (u64)( work->poke[i].growUpExp ) + (u64)exp;
      if( MAX_EXP < addedExp ){ addedExp = MAX_EXP; }

      // 経験値を設定
      work->poke[i].growUpExp = addedExp;
    }
  }
}


//===========================================================================================
// ■ セーブデータ登録関数
//===========================================================================================

//-------------------------------------------------------------------------------------------
/**
 * @brief	育て屋ワークのサイズ取得
 *
 * @return 育て屋ワークのサイズ
 */
//-------------------------------------------------------------------------------------------
u32 SODATEYA_WORK_GetWorkSize(void)
{
  return sizeof(SODATEYA_WORK);
}

//-------------------------------------------------------------------------------------------
/**
 * @brief	育て屋ワークの初期化
 *
 * @param	work 初期化するワーク
 */
//-------------------------------------------------------------------------------------------
void SODATEYA_WORK_InitWork( void* work )
{
  SODATEYA_WORK* sodateya = (SODATEYA_WORK*)work;
	GFL_STD_MemClear( sodateya, sizeof(SODATEYA_WORK) ); 
}

//-------------------------------------------------------------------------------------------
/**
 * @brief ワークへのポインタを取得する
 *
 * @param scw セーブデータ保持ワークへのポインタ
 *
 * @return 育て屋ワークへのポインタ
 */
//-------------------------------------------------------------------------------------------
SODATEYA_WORK* SODATEYA_WORK_GetSodateyaWork( SAVE_CONTROL_WORK* scw )
{
  return SaveControl_DataPtrGet( scw, GMDATA_ID_SODATEYA_WORK ); 
}
