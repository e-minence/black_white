//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file	  gts_tool.h
 *	@brief  �O�����{��
 *	@author	Toru=Nagihashi
 *	@date		2010.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/poke_tool.h"

//=============================================================================
/**
 *					�O���z�z���{���f�[�^
*/
//=============================================================================
static const int sc_distribution_ribbon_tbl[] =
{ 
  ID_PARA_country_ribbon,           //�|�P�������[�O�D���L�O(�S�����n��D����
  ID_PARA_national_ribbon,          //�_�[�N�|�P���� �����C�u�����̏�
  ID_PARA_earth_ribbon,             //�o�g���R 100�l�����B��
  ID_PARA_world_ribbon,             //�|�P�������[�O�D���L�O(�S�����œ��{��ɂȂ����҂̂�)
  ID_PARA_sinou_classic_ribbon,     //�O���z�z�p
  ID_PARA_sinou_premiere_ribbon,    //�O���z�z�p
  ID_PARA_sinou_history_ribbon,     //�O���z�z�p�F
  ID_PARA_sinou_green_ribbon,       //�O���z�z�p�F
  ID_PARA_sinou_blue_ribbon,        //�O���z�z�p�F
  ID_PARA_sinou_festival_ribbon,    //�O���z�z�p�F
  ID_PARA_sinou_carnival_ribbon,    //�O���z�z�p�F
  ID_PARA_marine_ribbon,            //�ėp�o�g�����
  ID_PARA_land_ribbon,              //WCS�n�����D���Ҕz�z�p
  ID_PARA_sky_ribbon,               //WCS�����D���Ҕz�z�p
  ID_PARA_sinou_red_ribbon,         //WCS���E���D���Ҕz�z�p                                  
};

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �֎~���{���������`�F�b�N
 *
 *	@param	POKEMON_PARAM *p_pp   �|�P�p��
 *
 *	@return TRUE�Ȃ�΋֎~���{������  FALSE�Ȃ�΂���ȊO
 */
//-----------------------------------------------------------------------------
static inline BOOL GTS_TOOL_IsForbidRibbonPPP( POKEMON_PASO_PARAM *p_ppp )
{


  BOOL ret;

  { 
    int i;
    BOOL temp;

    temp = PPP_FastModeOn( p_ppp );
    for( i = 0; i < NELEMS(sc_distribution_ribbon_tbl ); i++ )
    { 
      ret = PPP_Get( p_ppp, sc_distribution_ribbon_tbl[i], NULL );
      if( ret == TRUE )
      { 
        break;
      }
    }
    PPP_FastModeOff( p_ppp, temp );
  }

  return ret;
}

static inline BOOL GTS_TOOL_IsForbidRibbonPP( POKEMON_PARAM *p_pp )
{ 
  return GTS_TOOL_IsForbidRibbonPPP( PP_GetPPPPointer( p_pp ) );
}

static inline u32 GTS_TOOL_GetDistributeRibbonMax( void )
{ 
  return NELEMS(sc_distribution_ribbon_tbl);
}

static inline u32 GTS_TOOL_GetDistributeRibbon( u32 idx )
{ 
  GF_ASSERT( idx < GTS_TOOL_GetDistributeRibbonMax() );
  return sc_distribution_ribbon_tbl[ idx ];
}
