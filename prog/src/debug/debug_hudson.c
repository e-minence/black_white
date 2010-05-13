//=============================================================================
/**
 *
 *	@file		debug_hudson.c
 *	@brief  hudson�p���[�e�B���e�B
 *	@author		hosaka genya
 *	@data		2009.11.25
 *
 */
//=============================================================================

#include "debug/debug_hudson.h"

#ifdef DEBUG_ONLY_FOR_hudson

//-----------------------------------------------------------------------------
/**
 *	@brief  �^�C�g�����X�L�b�v���邩����
 *
 *	@param	void 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
BOOL HUDSON_IsSkipTitle( void )
{
  if( OS_GetArgc() < 2 )
  {
    return FALSE;
  }

  if ( STD_CompareString( OS_GetArgv(1), HUDSON_TESTCODE_MAP_JUMP ) )
  {
    return TRUE;
  }

  return FALSE;
}


#endif // DEBUG_ONLY_FOR_hudson

