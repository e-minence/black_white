//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		gts_tool.h
 *	@brief  GTSNEGOとの共有ツール
 *	@author	Toru=Nagihashi
 *	@date		2010.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/poke_tool.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================

//=============================================================================
/**
 *					外部公開
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  禁止リボンもちかチェック
 *
 *	@param	POKEMON_PARAM *p_pp   ポケパラ
 *
 *	@return TRUEならば禁止リボンもち  FALSEならばそれ以外
 */
//-----------------------------------------------------------------------------
static inline BOOL GTS_TOOL_IsForbidRibbonPPP( POKEMON_PASO_PARAM *p_ppp )
{
  static const int sc_forbid_ribbon_tbl[] =
  { 
    ID_PARA_country_ribbon,           //ポケモンリーグ優勝記念(全国大会地区優勝者
    ID_PARA_national_ribbon,          //ダークポケモン リライブ完了の証
    ID_PARA_earth_ribbon,             //バトル山 100人抜き達成
    ID_PARA_world_ribbon,             //ポケモンリーグ優勝記念(全国大会で日本一になった者のみ)
    ID_PARA_sinou_classic_ribbon,     //外部配布用
    ID_PARA_sinou_premiere_ribbon,    //外部配布用
    ID_PARA_sinou_history_ribbon,     //外部配布用：
    ID_PARA_sinou_green_ribbon,       //外部配布用：
    ID_PARA_sinou_blue_ribbon,        //外部配布用：
    ID_PARA_sinou_festival_ribbon,    //外部配布用：
    ID_PARA_sinou_carnival_ribbon,    //外部配布用：
    ID_PARA_marine_ribbon,            //汎用バトル大会
    ID_PARA_land_ribbon,              //WCS地方大会優勝者配布用
    ID_PARA_sky_ribbon,               //WCS国大会優勝者配布用
    ID_PARA_sinou_red_ribbon,         //WCS世界大会優勝者配布用                                  
  };

  BOOL ret  = TRUE;

  { 
    int i;
    BOOL temp;

    temp = PPP_FastModeOn( p_ppp );
    for( i = 0; i < NELEMS(sc_forbid_ribbon_tbl ); i++ )
    { 
      ret &= PPP_Get( ppp, sc_forbid_ribbon_tbl[i], NULL );
    }
    PPP_FastModeOff( p_ppp, temp );
  }

  return ret;
}

static inline BOOL GTS_TOOL_IsForbidRibbonPP( POKEMON_PARAM *p_pp )
{ 
  return GTS_TOOL_IsForbidRibbonPPP( PP_GetPPPPointer( p_pp ) );
}
