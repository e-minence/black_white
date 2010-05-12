//=============================================================================
/**
 *
 *	@file		debug_hudson.c
 *	@brief  hudson用ユーティリティ
 *	@author		hosaka genya
 *	@data		2009.11.25
 *
 */
//=============================================================================

#include "debug/debug_hudson.h"

#ifdef DEBUG_ONLY_FOR_hudson

//-----------------------------------------------------------------------------
/**
 *	@brief  タイトルをスキップするか判定
 *
 *	@param	void 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL HUDSON_IsSkipTitle( void )
{
  if ( OS_GetArgv(1) == HUDSON_TESTCODE_MAP_JUMP )
  {
    return TRUE;
  }

  return FALSE;
}


#endif // DEBUG_ONLY_FOR_hudson

