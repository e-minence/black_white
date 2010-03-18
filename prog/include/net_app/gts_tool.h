//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		gts_tool.h
 *	@brief  GTSNEGO�Ƃ̋��L�c�[��
 *	@author	Toru=Nagihashi
 *	@date		2010.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "poke_tool/poke_tool.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

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
static inline BOOL GTS_TOOL_IsForbidRibbon( POKEMON_PARAM *p_pp )
{ 
  return PP_Get( p_pp, ID_PARA_country_ribbon, NULL )          //�|�P�������[�O�D���L�O(�S�����n��D����
    || PP_Get( p_pp, ID_PARA_national_ribbon, NULL )           //�_�[�N�|�P���� �����C�u�����̏�
    || PP_Get( p_pp, ID_PARA_earth_ribbon, NULL )              //�o�g���R 100�l�����B��
    || PP_Get( p_pp, ID_PARA_world_ribbon, NULL )              //�|�P�������[�O�D���L�O(�S�����œ��{��ɂȂ����҂̂�)
    || PP_Get( p_pp, ID_PARA_sinou_classic_ribbon, NULL )      //�O���z�z�p
    || PP_Get( p_pp, ID_PARA_sinou_premiere_ribbon, NULL )     //�O���z�z�p
    || PP_Get( p_pp, ID_PARA_sinou_history_ribbon, NULL )      //�O���z�z�p�F
    || PP_Get( p_pp, ID_PARA_sinou_green_ribbon, NULL )        //�O���z�z�p�F
    || PP_Get( p_pp, ID_PARA_sinou_blue_ribbon, NULL )         //�O���z�z�p�F
    || PP_Get( p_pp, ID_PARA_sinou_festival_ribbon, NULL )     //�O���z�z�p�F
    || PP_Get( p_pp, ID_PARA_sinou_carnival_ribbon, NULL )     //�O���z�z�p�F
    || PP_Get( p_pp, ID_PARA_marine_ribbon, NULL )             //�ėp�o�g�����
    || PP_Get( p_pp, ID_PARA_land_ribbon, NULL )               //WCS�n�����D���Ҕz�z�p
    || PP_Get( p_pp, ID_PARA_sky_ribbon, NULL )                //WCS�����D���Ҕz�z�p
    || PP_Get( p_pp, ID_PARA_sinou_red_ribbon, NULL );         //WCS���E���D���Ҕz�z�p                                   

}
