//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_season_time.c
 *	@brief  季節による　時間帯変化
 *	@author	tomoya takahashi
 *	@date		2010.01.28
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_season_time.h"

#include "system/rtc_tool.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	季節による時間帯変化管理ワーク
//=====================================
struct _FLD_SEASON_TIME 
{
  u32 dummy;  // @TODO 後は季節ごとの時間帯切り替え時間
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  季節の時間帯管理ワーク　生成
 *
 *	@param	season_id   季節ID
 *	@param	heapID      ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
FLD_SEASON_TIME* FLD_SEASON_TIME_Create( u8 season_id, HEAPID heapID )
{
  FLD_SEASON_TIME* p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SEASON_TIME) );
  return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief  季節の時間帯管理ワーク　破棄
 *
 *	@param	p_sys   システム
 */
//-----------------------------------------------------------------------------
void FLD_SEASON_TIME_Delete( FLD_SEASON_TIME* p_sys )
{
  GFL_HEAP_FreeMemory( p_sys );
}



//----------------------------------------------------------------------------
/**
 *	@brief  季節ごとの時間帯取得
 *
 *	@param	cp_sys  システム
 *
 *	@return 時間帯
 */
//-----------------------------------------------------------------------------
u32 FLD_SEASON_TIME_GetTimeZone( const FLD_SEASON_TIME* cp_sys )
{
  GF_ASSERT( cp_sys );
  return GFL_RTC_GetTimeZone();
}

