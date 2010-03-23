//==============================================================================
/**
 * @file    symbol_save_field.c
 * @brief   シンボルエンカウント用セーブデータ：fieldmapオーバーレイに配置
 * @author  matsuda
 * @date    2010.03.03(水)
 *
 * フィールドでしか使用しないシンボルエンカウントセーブアクセス群
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/symbol_save.h"
#include "savedata/symbol_save_notwifi.h"
#include "savedata/symbol_save_field.h"
#include "symbol_save_local.h"


//==================================================================
/**
 * シンボルポケモンの移動
 *
 * @param   symbol_save
 * @param   now_no          移動対象ポケモンの配置No.
 *
 * 配置No.から現在のゾーンタイプを自動判別し、適切な移動を行う
 */
//==================================================================
BOOL SymbolSave_Field_MoveAuto( SYMBOL_SAVE_WORK *symbol_save, u32 now_no )
{
  SYMBOL_ZONE_TYPE now_type = SYMBOLZONE_GetZoneTypeFromNumber( now_no );
  SYMBOL_ZONE_TYPE new_type;
  u32 new_no;

  switch ( now_type )
  {
  case SYMBOL_ZONE_TYPE_KEEP_LARGE: //キープゾーン（大）→フリーゾーン（大）
    new_type = SYMBOL_ZONE_TYPE_FREE_LARGE;
    break;
  case SYMBOL_ZONE_TYPE_KEEP_SMALL: //キープゾーン（小）→フリーゾーン（小）
    new_type = SYMBOL_ZONE_TYPE_FREE_SMALL;
    break;
  case SYMBOL_ZONE_TYPE_FREE_LARGE: //フリーゾーン（大）→キープゾーン（大）
    new_type = SYMBOL_ZONE_TYPE_KEEP_LARGE;
    break;
  case SYMBOL_ZONE_TYPE_FREE_SMALL: //フリーゾーン（小）→キープゾーン（小）
    new_type = SYMBOL_ZONE_TYPE_KEEP_SMALL;
    break;
  default:
    GF_ASSERT( 0 );
    return FALSE;
  }

  new_no = SymbolSave_CheckSpace(symbol_save, new_type);
#ifdef  PM_DEBUG
  {
    static const char * const typenames[] = { "Keep Large", "Keep Small", "Free Large", "Free Small" };
    TAMADA_Printf("%s(%3d)-->%s(%3d)", typenames[now_type], now_no, typenames[new_type], new_no);
  }
#endif

  if(new_no == SYMBOL_SPACE_NONE){
    TAMADA_Printf(":Failure\n");
    return FALSE;

  } else {
    //新しい場所に代入する
    symbol_save->symbol_poke[new_no] = symbol_save->symbol_poke[now_no];
    //元のゾーンを前詰め
    SymbolSave_DataShift(symbol_save, now_no);
    TAMADA_Printf(":Success\n");
    return TRUE;
  }
}

