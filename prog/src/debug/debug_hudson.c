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
 *	@brief  �^�C�g�����X�L�b�v���邩����B�������w�肳��Ă����ꍇ�A�X�L�b�v�B
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

  return TRUE;
}


#endif // DEBUG_ONLY_FOR_hudson

