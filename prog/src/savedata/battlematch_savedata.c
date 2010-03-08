//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		battlematch_savedata.h
 *	@brief  バトル大会用セーブデータ
 *	@author	Toru=Nagihashi
 *	@date		2010.03.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "savedata/battlematch_savedata.h"
#include "battlematch_savedata_def.h"
#include "savedata/save_tbl.h"
//=============================================================================
/**
 *        構造体
 */
//=============================================================================
//-------------------------------------
///	セーブデータ不完全型
//=====================================
struct _BATTLEMATCH_DATA
{ 
  LIVEMATCH_DATA  livematch;
  RNDMATCH_DATA   rndmatch;
};

//=============================================================================
/**
 *				外部公開
*/
//=============================================================================
//----------------------------------------------------------
//セーブデータシステムに依存する関数
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ワークサイズ取得
 */
//-----------------------------------------------------------------------------
int BATTLEMATCH_GetWorkSize( void )
{ 
  return sizeof( BATTLEMATCH_DATA );
}
//----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	BATTLEMATCH_DATA *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void BATTLEMATCH_Init( BATTLEMATCH_DATA *p_wk )
{ 
  LIVEMATCH_Init( &p_wk->livematch );
  RNDMATCH_Init( &p_wk->rndmatch );
}

//----------------------------------------------------------
//セーブデータ取得のための関数
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ワーク取得  CONST版
 *
 *	@param	SAVE_CONTROL_WORK * p_sv  セーブデータ
 *
 *	@return ワーク取得
 */
//-----------------------------------------------------------------------------
const BATTLEMATCH_DATA * SaveData_GetBattleMatchConst( const SAVE_CONTROL_WORK * cp_sv )
{ 
  return SaveData_GetBattleMatch( (SAVE_CONTROL_WORK*)cp_sv );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ワーク取得
 *
 *	@param	SAVE_CONTROL_WORK * p_sv  セーブデータ
 *
 *	@return ワーク取得
 */
//-----------------------------------------------------------------------------
BATTLEMATCH_DATA * SaveData_GetBattleMatch( SAVE_CONTROL_WORK * p_sv )
{ 
  return SaveControl_DataPtrGet( p_sv , GMDATA_ID_BATTLEMATCH );
}

//----------------------------------------------------------
//大会データからの取得
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチのデータ取得  CONST
 *
 *	@param	const BATTLEMATCH_DATA * cp_sv  ワーク
 *
 *	@return ランダムマッチのデータを取得
 */
//-----------------------------------------------------------------------------
const RNDMATCH_DATA * BATTLEMATCH_GetRndMatchConst( const BATTLEMATCH_DATA * cp_sv )
{ 
  return &cp_sv->rndmatch;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチのデータ取得
 *
 *	@param	const BATTLEMATCH_DATA * cp_sv  ワーク
 *
 *	@return ランダムマッチのデータを取得
 */
//-----------------------------------------------------------------------------
RNDMATCH_DATA * BATTLEMATCH_GetRndMatch( BATTLEMATCH_DATA * p_sv )
{ 
  return &p_sv->rndmatch;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ライブ大会のデータ取得  CONST版
 *
 *	@param	const LIVEMATCH_DATA * cp_sv  ワーク
 *
 *	@return ライブ大会のデータ取得
 */
//-----------------------------------------------------------------------------
const LIVEMATCH_DATA * BATTLEMATCH_GetLiveMatchConst( const BATTLEMATCH_DATA * cp_sv )
{ 
  return &cp_sv->livematch;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ライブ大会のデータ取得
 *
 *	@param	LIVEMATCH_DATA * p_sv   ワーク
 *
 *	@return ライブ大会のデータ取得
 */
//-----------------------------------------------------------------------------
LIVEMATCH_DATA * BATTLEMATCH_GetLiveMatch( BATTLEMATCH_DATA * p_sv )
{ 
  return &p_sv->livematch;
}
