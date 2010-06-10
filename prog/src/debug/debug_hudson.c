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

//-----------------------------------------------------------------------------
/**
 *	@brief  �e�X�g�R�[�h���`�F�b�N
 *
 *	@param	const char* str 
 *
 *	@retval TRUE:�w�肵���e�X�g�R�[�h�ƈ�v
 */
//-----------------------------------------------------------------------------
BOOL HUDSON_IsTestCode( const char* str )
{ 
  return ( OS_GetArgc() >= 2 && STD_StrCmp( OS_GetArgv(1), str ) == 0 );
}

#endif // DEBUG_ONLY_FOR_hudson

