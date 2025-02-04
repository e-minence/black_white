//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		my_pms_data.c
 *	@brief  自己紹介文や対戦前挨拶などの、PMS情報セーブデータ
 *	@author	Toru=Nagihashi
 *	@data		2010.01.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "savedata/save_tbl.h"

#include "savedata/my_pms_data.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	PMS情報セーブデータ
//=====================================
struct _MYPMS_DATA
{
  PMS_DATA  pms[MYPMS_PMS_TYPE_MAX];
} ;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================

//=============================================================================
/**
 *          外部公開
 */
//=============================================================================
//----------------------------------------------------------
//セーブデータシステムに依存する関数
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  構造体サイズを取得
 *
 *	@param	void 
 *
 *	@return 構造体サイズ
 */
//-----------------------------------------------------------------------------
int MYPMS_GetWorkSize( void )
{ 
  return sizeof( MYPMS_DATA );
}

//----------------------------------------------------------------------------
/**
 *	@brief  構造体を初期化
 *
 *	@param	MYPMS_DATA *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void MYPMS_Init( MYPMS_DATA *p_wk )
{
  PMSDAT_SetupDefaultIntroductionMessage( &p_wk->pms[MYPMS_PMS_TYPE_INTRODUCTION] );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_READY], PMS_BATTLE_TYPE_PLAYER_READY );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_WON], PMS_BATTLE_TYPE_PLAYER_WIN );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_LOST], PMS_BATTLE_TYPE_PLAYER_LOSE );
  PMSDAT_SetupDefaultBattleMessage( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_TOP], PMS_BATTLE_TYPE_LEADER );

  /*
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_INTRODUCTION], PMS_TYPE_UNION );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_READY], PMS_TYPE_BATTLE_READY );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_WON], PMS_TYPE_BATTLE_WON );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_LOST], PMS_TYPE_BATTLE_LOST );
  PMSDAT_Init( &p_wk->pms[MYPMS_PMS_TYPE_BATTLE_TOP], PMS_TYPE_BATTLE_WON );
  */
}

//----------------------------------------------------------
//セーブデータ取得のための関数
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ワーク取得  CONST版
 *
 *	@param	const SAVE_CONTROL_WORK * cp_sv セーブデータポインタ
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
const MYPMS_DATA * SaveData_GetMyPmsDataConst( const SAVE_CONTROL_WORK * cp_sv)
{ 
	return (const MYPMS_DATA *)SaveData_GetMyPmsData( (SAVE_CONTROL_WORK *)cp_sv);
}
//----------------------------------------------------------------------------
/**
 *	@brief  ワーク取得
 *
 *	@param	SAVE_CONTROL_WORK * p_sv  セーブデータポインタ
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
MYPMS_DATA * SaveData_GetMyPmsData( SAVE_CONTROL_WORK * p_sv)
{ 
  return SaveControl_DataPtrGet(p_sv, GMDATA_ID_MYPMS);
}

//----------------------------------------------------------
//それぞれの取得関数
//----------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  PMSを取得
 *
 *	@param	const MYPMS_DATA *cp_wk ワーク
 *	@param	type                    挨拶文の種類
 *	@param	*p_pms                  受け取り変数
 */
//-----------------------------------------------------------------------------
void MYPMS_GetPms( const MYPMS_DATA *cp_wk, MYPMS_PMS_TYPE type, PMS_DATA *p_pms )
{ 
  GF_ASSERT( type < MYPMS_PMS_TYPE_MAX );
  *p_pms  = cp_wk->pms[ type ];
}
//----------------------------------------------------------------------------
/**
 *	@brief  PMSを設定
 *
 *	@param	MYPMS_DATA *p_wk  ワーク
 *	@param	type              挨拶文の種類
 *	@param	PMS_DATA *cp_pms  設定する値
 *
 */
//-----------------------------------------------------------------------------
void MYPMS_SetPms( MYPMS_DATA *p_wk, MYPMS_PMS_TYPE type, const PMS_DATA *cp_pms )
{ 
  GF_ASSERT( type < MYPMS_PMS_TYPE_MAX );
  p_wk->pms[ type ] = *cp_pms;
}
