//======================================================================
/**
 * @file	field_status.c
 * @brief フィールドマップ管理情報
 * @author  tamada GAMEFREAK inc.
 * @date    2009.10.03
 */
//======================================================================

#include <gflib.h>
#include "field/field_status.h"


#include "field_status_local.h"   //MAPMODE
//--------------------------------------------------------------
//--------------------------------------------------------------

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
struct _FIELD_STATUS{
  MAPMODE map_mode;
  u8 isFieldInitFlag;

  // フィールド技関連
  u8  fs_flash;      // フラッシュON
  u16 fs_mapeffect;  // 見た目の状態

  // 季節表示
  BOOL seasonDispFlag;  // 表示フラグ
  u8   seasonDispLast;  // 最後に表示した季節 PMSEASON_xxxx
};

//======================================================================
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
FIELD_STATUS * FIELD_STATUS_Create(HEAPID heapID)
{
  FIELD_STATUS * fldstatus;
  fldstatus = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_STATUS) );
  return fldstatus;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_STATUS_Delete(FIELD_STATUS * fldstatus)
{
  GFL_HEAP_FreeMemory( fldstatus );
}


//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * マップモード取得
 *
 * @param   gamedata		GAMEDATAへのポインタ
 *
 * @retval  MAPMODE		マップモード
 */
//------------------------------------------------------------------
MAPMODE FIELD_STATUS_GetMapMode(const FIELD_STATUS * fldstatus)
{
  return fldstatus->map_mode;
}

//------------------------------------------------------------------
/**
 * マップモード設定
 *
 * @param   gamedata		GAMEDATAへのポインタ
 * @param   map_mode		マップモード
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetMapMode(FIELD_STATUS * fldstatus, MAPMODE map_mode)
{
  fldstatus->map_mode = map_mode;
}

//======================================================================
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_STATUS_SetFieldInitFlag( FIELD_STATUS * fldstatus, BOOL flag )
{
  fldstatus->isFieldInitFlag = flag;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL FIELD_STATUS_GetFieldInitFlag( const FIELD_STATUS * fldstatus )
{
  return fldstatus->isFieldInitFlag;
}

//------------------------------------------------------------------
/**
 * @brief 季節表示フラグの設定
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetSeasonDispFlag( FIELD_STATUS * fldstatus, BOOL flag )
{
  fldstatus->seasonDispFlag = flag;
}

//------------------------------------------------------------------
/**
 * @brief 季節表示フラグの取得
 */
//------------------------------------------------------------------
BOOL FIELD_STATUS_GetSeasonDispFlag( const FIELD_STATUS * fldstatus )
{
  return fldstatus->seasonDispFlag;
}

//------------------------------------------------------------------
/**
 * @brief 最後に表示した季節の設定
 */
//------------------------------------------------------------------
void FIELD_STATUS_SetSeasonDispLast( FIELD_STATUS * fldstatus, u8 season )
{
  fldstatus->seasonDispLast = season;
}

//------------------------------------------------------------------
/**
 * @brief 最後に表示した季節の取得
 */
//------------------------------------------------------------------
u8 FIELD_STATUS_GetSeasonDispLast( const FIELD_STATUS * fldstatus )
{
  return fldstatus->seasonDispLast;
}


//------------------------------------------------------------------
// フィールド技　フラッシュのON・OFF
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  フィールド技　フラッシュ　ON・OFF
 *
 *	@param	fldstatus   ワーク
 *	@param	flag        TRUE:ON   FALSE：OFF
 */
//-----------------------------------------------------------------------------
void FIELD_STATUS_SetFieldSkillFlash( FIELD_STATUS * fldstatus, BOOL flag )
{
  GF_ASSERT( fldstatus );
  fldstatus->fs_flash = flag;
}


//----------------------------------------------------------------------------
/**
 *	@brief  フィールド技　フラッシュ　チェック
 *
 *	@param	fldstatus   ワーク
 *
 *	@retval TRUE    フラッシュ状態
 *	@retval FALSE   フラッシュしてない
 */
//-----------------------------------------------------------------------------
BOOL FIELD_STATUS_IsFieldSkillFlash( const FIELD_STATUS * fldstatus )
{
  GF_ASSERT( fldstatus );
  return fldstatus->fs_flash;
}

//------------------------------------------------------------------
// フィールド技　マップ効果状態
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  フィールド技　マップ効果状態マスク　を設定
 *
 *	@param	fldstatus   ワーク
 *	@param	msk         状態マスク
 */
//-----------------------------------------------------------------------------
void FIELD_STATUS_SetFieldSkillMapEffectMsk( FIELD_STATUS * fldstatus, u32 msk )  
{
  GF_ASSERT( fldstatus );
  fldstatus->fs_mapeffect = msk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  フィールド技　マップ効果状態マスクを　取得
 *
 *	@param	fldstatus   ワーク
 *
 *	@return マスク
 */
//-----------------------------------------------------------------------------
u32 FIELD_STATUS_GetFieldSkillMapEffectMsk( const FIELD_STATUS * fldstatus )
{
  GF_ASSERT( fldstatus );
  return fldstatus->fs_mapeffect;
}




