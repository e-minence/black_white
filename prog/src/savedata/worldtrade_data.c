//============================================================================================
/**
 * @file	worldtrade_data.c
 * @brief	世界交換セーブデータ
 * @author	mori GAME FREAK inc.
 * @date	2006.04.30
 */
//============================================================================================
#include <gflib.h>

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK参照のため
#include "savedata/worldtrade_data.h"
#include "savedata/save_tbl.h"

//============================================================================================
//============================================================================================

struct WORLDTRADE_DATA{
	u8 PokemonData[WORLDTRADE_POKEMON_DATA_SIZE];	// 預けたポケモンのデータ
	u16 		flag;					// 預けているかフラグ
	u16 		toBox;					// ボックスＮＯ（手持ちの場合はRETURN_POSITION_TEMOTI）
	GFDATE		lastExchangeDate_Deposit;		// 最終交換日付(預けて成立した日付)
	GFDATE		lastExchangeDate_Search;		// 最終交換日付(検索して成立した日付)
};



//============================================================================================
//
//	セーブデータシステムが依存する関数
//
//============================================================================================
WORLDTRADE_DATA * SaveData_GetWorldTradeData( SAVE_CONTROL_WORK *sv )
{
	return SaveControl_DataPtrGet( sv , GMDATA_ID_WORLDTRADEDATA );
}


//----------------------------------------------------------
/**
 * @brief	自分状態保持ワークのサイズ取得
 * @return	int		サイズ（バイト単位）
 */
//----------------------------------------------------------
int WorldTradeData_GetWorkSize(void)
{
	return sizeof(WORLDTRADE_DATA);
}


//==============================================================================
/**
 * @brief   データ初期化
 *
 * @param   randomgroup		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTradeData_Init(WORLDTRADE_DATA * WorldTradeData )
{
	
	MI_CpuClearFast(WorldTradeData, sizeof(WORLDTRADE_DATA));
}


//==============================================================================
/**
 * @brief   預けているか？
 *
 * @param   WorldTradeData		
 *
 * @retval  int		0:預けていない	1:預けている
 */
//==============================================================================
int  WorldTradeData_GetFlag( WORLDTRADE_DATA* WorldTradeData )
{
	return WorldTradeData->flag;
}
//==============================================================================
/**
 * @brief   預けているフラグの操作
 *
 * @param   WorldTradeData		
 *
 * @retval  int		
 */
//==============================================================================
void  WorldTradeData_SetFlag( WORLDTRADE_DATA* WorldTradeData, int flag )
{
	WorldTradeData->flag = flag;
}


//==============================================================================
/**
 * @brief   ポケモンデータを取り出す
 *
 * @param   WorldTradeData		
 * @param   poke		
 *
 * @retval  none		
 */
//==============================================================================
void  WorldTradeData_GetPokemonData( WORLDTRADE_DATA* WorldTradeData, POKEMON_PARAM *poke )
{
	GFL_STD_MemCopy( WorldTradeData->PokemonData, (void*)poke, POKETOOL_GetWorkSize() );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンデータのポインタをもらう
 *
 *	@param	WORLDTRADE_DATA* WorldTradeData   ワーク
 *
 *	@return POKEMON_PARAMのアドrす
 */
//-----------------------------------------------------------------------------
POKEMON_PARAM *WorldTradeData_GetPokemonDataPtr( WORLDTRADE_DATA* WorldTradeData )
{
  return (POKEMON_PARAM *)WorldTradeData->PokemonData;
}
//==============================================================================
/**
 * @brief   ポケモンを預けたので、コピーを保存しておく
 *
 * @param   WorldTradeData		
 * @param   poke		
 *
 * @retval  none		
 */
//==============================================================================
void  WorldTradeData_SetPokemonData( WORLDTRADE_DATA* WorldTradeData, POKEMON_PARAM *poke, int boxno )
{
	GFL_STD_MemCopy( (void*)poke, WorldTradeData->PokemonData, POKETOOL_GetWorkSize() );
}


//==============================================================================
/**
 * @brief   預けて交換が成立した最後の日を得る
 *
 * @param   WorldTradeData		
 *
 * @retval  int		0:預けていない	1:預けている
 */
//==============================================================================
GFDATE  WorldTradeData_GetLastDate_Deposit( WORLDTRADE_DATA* WorldTradeData )
{
	return WorldTradeData->lastExchangeDate_Deposit;
}
//==============================================================================
/**
 * @brief   預けて交換が成立した最後の日を記録する
 *
 * @param   WorldTradeData		
 *
 * @retval  int		
 */
//==============================================================================
void  WorldTradeData_SetLastDate_Deposit( WORLDTRADE_DATA* WorldTradeData, GFDATE date )
{
	WorldTradeData->lastExchangeDate_Deposit = date;
}

//==============================================================================
/**
 * @brief   検索して交換が成立した最後の日を得る
 *
 * @param   WorldTradeData		
 *
 * @retval  int		0:預けていない	1:預けている
 */
//==============================================================================
GFDATE  WorldTradeData_GetLastDate_Search( WORLDTRADE_DATA* WorldTradeData )
{
	return WorldTradeData->lastExchangeDate_Search;
}
//==============================================================================
/**
 * @brief   検索して交換が成立した最後の日を記録する
 *
 * @param   WorldTradeData		
 *
 * @retval  int		
 */
//==============================================================================
void  WorldTradeData_SetLastDate_Search( WORLDTRADE_DATA* WorldTradeData, GFDATE date )
{
	WorldTradeData->lastExchangeDate_Search = date;
}

//==============================================================================
/**
 * @brief   ポケモンが預けられているBOXNO（てもちはRETURN_POSITION_TEMOTI）取得
 *
 * @param   WorldTradeData		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTradeData_GetBoxNo( WORLDTRADE_DATA* WorldTradeData )
{
	return WorldTradeData->toBox;
}

