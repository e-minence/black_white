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
typedef struct _FIELD_STATUS{
  MAPMODE map_mode;
  u8 isFieldInitFlag;
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
