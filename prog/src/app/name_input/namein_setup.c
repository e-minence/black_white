//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		src\app\name_input\namein_setup.c
 *	@brief  ���O���͐ݒ�֐�
 *	@author	Toru=Nagihashi
 *	@data		2010.03.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//  lib
#include <gflib.h>

//  SYSTEM
#include "system/main.h"  //HEAPID
#include "system/gfl_use.h"

//  module
#include "app/app_menu_common.h"
#include "pokeicon/pokeicon.h"

//  savedata
#include "savedata/misc.h"
#include "app/name_input.h" //�O���Q��
#include "namein_local.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *    �O���Q��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  NAMEIN�ɓn��PARAM�\���̍쐬
 *
 *  @param  HEAPID heapId         �q�[�vID
 *  @param  mode                  ���͂̎��
 *  @param  param1                ��ނ��ƂɈႤ�����P
 *  @param  param2                ��ނ��ƂɈႤ�����Q
 *  @param  wordmax               ���͕����ő吔
 *  @param  STRBUF *default_str   �f�t�H���g�œ��͂���Ă��镶����
 *
 *  @retura NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
NAMEIN_PARAM *NAMEIN_AllocParam( HEAPID heapId, NAMEIN_MODE mode, int param1, int param2, int wordmax, const STRBUF *default_str, MISC *p_misc )
{ 
  NAMEIN_PARAM *p_param;
  p_param = GFL_HEAP_AllocMemory( heapId, sizeof(NAMEIN_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(NAMEIN_PARAM) );
  p_param->mode     = mode;
  p_param->wordmax  = wordmax;
  p_param->param1 = param1;
  p_param->param2 = param2;
  p_param->p_misc = p_misc;

  //�o�b�t�@�쐬
  p_param->strbuf = GFL_STR_CreateBuffer( wordmax + 1, heapId );

  //�f�t�H���g���͕����R�s�[
  if( default_str )
  { 
    GFL_STR_CopyBuffer( p_param->strbuf, default_str );
  }

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *  @brief  NAMEIN�ɓn��PARAM�\���̍쐬 �|�P�������[�h�Œ聕PP�w���
 *
 *  @param  HEAPID heapId         �q�[�vID
 *  @param  POKEMON_PARAM *pp     �|�P�����p����
 *  @param  wordmax               ���͕����ő吔
 *  @param  STRBUF *default_str   �f�t�H���g�œ��͂���Ă��镶����
 *
 *  @return NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
NAMEIN_PARAM *NAMEIN_AllocParamPokemonByPP( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str, MISC *p_misc )
{ 
  NAMEIN_PARAM *p_param;
  p_param = GFL_HEAP_AllocMemory( heapId, sizeof(NAMEIN_PARAM) );
  GFL_STD_MemClear( p_param, sizeof(NAMEIN_PARAM) );
  p_param->mode     = NAMEIN_POKEMON;
  p_param->wordmax  = wordmax;
  p_param->pp       = pp;
  p_param->p_misc = p_misc;

  //PP�̏ꍇ������ݒ�
  if( p_param->pp )
  { 
    p_param->mons_no  = PP_Get( pp, ID_PARA_monsno, NULL );
    p_param->form     = PP_Get( pp, ID_PARA_form_no, NULL ) | ( PP_Get( pp, ID_PARA_sex, NULL ) << 8 );
  }

  //�o�b�t�@�쐬
  p_param->strbuf = GFL_STR_CreateBuffer( wordmax + 1, heapId );

  //�f�t�H���g���͕����R�s�[
  if( default_str )
  { 
    GFL_STR_CopyBuffer( p_param->strbuf, default_str );
  }

  return p_param;
}

//----------------------------------------------------------------------------
/**
 *  @brief  NAMEIN�ɓn��PARAM�\���̍쐬 �|�P�������[�h�Œ聕PP�w���
 *          �|�P�����ߊl���͂�������g�p���Ă�������
 *
 *  @param  HEAPID heapId         �q�[�vID
 *  @param  POKEMON_PARAM *pp     �|�P�����p����
 *  @param  wordmax               ���͕����ő吔
 *  @param  STRBUF *default_str   �f�t�H���g�œ��͂���Ă��镶����
 *  @param  box_strbuf   [�|�P�����j�b�N�l�[��]�́����̃p�\�R����[�{�b�N�X��]�ɓ]�����ꂽ�I
 *  @param  box_manager  �{�b�N�X�}�l�[�W��(box_strbuf!=NULL�̂Ƃ������g���Ȃ�) 
 *  @param  box_tray     �{�b�N�X�g���C�i���o�[(box_strbuf!=NULL�̂Ƃ������g���Ȃ�) 
 *
 *  @return NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
NAMEIN_PARAM *NAMEIN_AllocParamPokemonCapture( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str,
                                               const STRBUF *box_strbuf, const BOX_MANAGER *box_manager, u32 box_tray, MISC *p_misc )
{
  NAMEIN_PARAM *p_param = NAMEIN_AllocParamPokemonByPP( heapId, pp, wordmax, default_str, p_misc );
  p_param->box_strbuf = box_strbuf;
  p_param->box_manager = box_manager;
  p_param->box_tray = box_tray;
  return p_param;
}

//----------------------------------------------------------------------------
/**
 *  @brief  NAMEIN_PARAM��j��
 *
 *  @param  *param  ���[�N
 */
//-----------------------------------------------------------------------------
void NAMEIN_FreeParam(NAMEIN_PARAM *param)
{ 
  GFL_STR_DeleteBuffer( param->strbuf );
  GFL_HEAP_FreeMemory( param );
}
//----------------------------------------------------------------------------
/**
 *  @brief  NAMEINPARAM����A���͂��ꂽ�������R�s�[
 *
 *  @param  const NAMEIN_PARAM *param   ���[�N
 *  @param  *strbuf   �R�s�[�󂯎��o�b�t�@
 */
//-----------------------------------------------------------------------------
void NAMEIN_CopyStr( const NAMEIN_PARAM *param, STRBUF *strbuf )
{ 
  GFL_STR_CopyBuffer( strbuf, param->strbuf );
}
//----------------------------------------------------------------------------
/**
 *  @brief  NAMEINPARAM����A���͂��ꂽ������Alloc���R�s�[���ĕԂ�
 *
 *  @param  const NAMEIN_PARAM *param ���[�N
 *  @param  heapID                    �q�[�vID
 *
 *  @return �R�s�[���ꂽ������
 */
//-----------------------------------------------------------------------------
STRBUF* NAMEIN_CreateCopyStr( const NAMEIN_PARAM *param, HEAPID heapID )
{ 
  return GFL_STR_CreateCopyBuffer( param->strbuf, heapID );
}
//----------------------------------------------------------------------------
/**
 *  @brief  NAMEINPARAM����A�L�����Z�����ꂽ���̃t���O���擾
 *
 *  @param  const NAMEIN_PARAM *param   ���[�N
 *
 *  @retval TRUE�Ȃ��0������OK�������ꂽor�f�t�H���g�����Ɠ���
 *  @retval FALSE�Ȃ�΃L�����Z������Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_IsCancel( const NAMEIN_PARAM *param )
{ 
  return param->cancel;
}


