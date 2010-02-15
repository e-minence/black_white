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

#ifdef PM_DEBUG

//���C�u����
#include <gflib.h>

//���W���[��
#include "debug/debug_str_conv.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define   DEBUG_NAGI_Printf( ... )      OS_TPrintf( __VA_ARGS__ )

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
      DEBUG_NAGI_Printf( "%s ", str );
    }
    DEBUG_NAGI_Printf("\n");
  }

}

//=============================================================================
/**
 *					�������Ԍv�Z�}�N��
 *					  PrintStart��PrintEnd�ŋ��񂾉ӏ����}�C�N���b�Ő}��܂�
*/
//=============================================================================
static OSTick s_DUN_TICK_DRAW_start;
#define DEBUG_TICK_PrintStart  (s_DUN_TICK_DRAW_start = OS_GetTick())
#define DEBUG_TICK_PrintEnd    DEBUG_NAGI_Printf("file[%s] line[%d] time=%dms\n", __FILE__, __LINE__,OS_TicksToMicroSeconds(OS_GetTick() - s_DUN_TICK_DRAW_start) )

//=============================================================================
/**
 *					�q�[�v�c��T�C�Y�v�����g�֐�
 *					  �ȉ��֐��𖈃t���[���܂킵�Ă�������
*/
//=============================================================================
static inline void DEBUG_HEAP_PrintRestUse( HEAPID heapID )
{ 
  static u32 s_max = 0xFFFFFFFF;
  int rest;

  rest  = GFL_HEAP_GetHeapFreeSize( heapID );
  if( s_max > rest )
  { 
    s_max = rest;
  }
  DEBUG_NAGI_Printf( "HEAPID[0x%x] most low rest size=0x%x\n", heapID, s_max );
}

#else //PM_DEBUG
#define   DEBUG_NAGI_Printf( ... )  /* */
#define DEBUG_STRBUF_Print(x)       /* */
#define DEBUG_TICK_PrintStart       /* */
#define DEBUG_TICK_PrintEnd         /* */
#define DEBUG_HEAP_PrintRestUse(x)  /* */
#endif //PM_DEBUG
