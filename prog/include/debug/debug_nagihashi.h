//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_nagihashi.h
 *	@brief  �f�o�b�O�p�֗��֐��Q
 *	@author	Toru=Nagihashi
 *	@date		2010.02.13
 *	@note   �I�[�o�[���C�𖳎����邽�߂Ƀw�b�_�ɑS�ċL�q�B
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//���C�u����
#include <gflib.h>

//���W���[��
#include "debug/debug_str_conv.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  STRBUF�̒��g��\���i�����͖����j
 *
 *	@param	const STRBUF *cp_str STRBUF
 */
//-----------------------------------------------------------------------------
static inline void DEBUG_STRBUF_Print( const STRBUF *cp_str )
{

  const int len     = GFL_STR_GetBufferLength(cp_str);
  const STRCODE *cp_code = GFL_STR_GetStringCodePointer( cp_str );

  { 
    int i;
    char str[3] = {0};
    for( i = 0; i < len; i++ )
    {
      DEB_STR_CONV_StrcodeToSjis( &cp_code[i] , str , 1 );
      NAGI_Printf( "%s ", str );
    }
    NAGI_Printf("\n");
  }

}

//=============================================================================
/**
 *					�������Ԍv�Z�}�N��
*/
//=============================================================================
static OSTick s_DUN_TICK_DRAW_start;
#define DEBUG_TICK_PrintStart  (s_DUN_TICK_DRAW_start = OS_GetTick())
#define DEBUG_TICK_PrintEnd    NAGI_Printf("line[%d] time=%dms\n",__LINE__,OS_TicksToMicroSeconds(OS_GetTick() - s_DUN_TICK_DRAW_start) )
