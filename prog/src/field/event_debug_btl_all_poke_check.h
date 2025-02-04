//============================================================================
/**
 *
 *	@file		event_debug_btl_all_poke_check.h
 *	@brief  バトルで全ポケモン表示
 *	@author hosaka genya
 *	@data		2010.06.10
 *
 */
//============================================================================
#ifdef PM_DEBUG
#pragma once

FS_EXTERN_OVERLAY( debug_all_poke_check );

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  全ポケモンチェックイベントを生成する
 *
 *	@param	GAMESYS_WORK* gsys
 *	@param	wk_adrs 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
extern GMEVENT* EVENT_DEBUG_BtlAllPokeCheck( GAMESYS_WORK* gsys, void* wk_adrs );

#endif // PM_DEBUG
