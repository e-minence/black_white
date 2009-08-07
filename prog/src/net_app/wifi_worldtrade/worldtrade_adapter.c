//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		worldtrade_adapter.c
 *	@brief	GTS���ڐA����ۂ̃p�b�N�֐��Q
 *	@author	Toru=Nagihashi
 *	@data		2009.08.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>
//system
#include "system/main.h"
#include "system/bmp_winframe.h"

//mine
#include "worldtrade_adapter.h"
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
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PPP����j�b�N�l�[����o�^����֐�
 *
 *	@param	WORDSET* wordset	���[�h�Z�b�g
 *	@param	bufID							�o�b�t�@ID
 *	@param	POKEMON_PASO_PARAM * ppp	PPP
 */
//-----------------------------------------------------------------------------
void WT_WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM * ppp )
{	
	POKEMON_PARAM *pp	= PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
	WORDSET_RegisterPokeNickName( wordset, bufID, pp );
	GFL_HEAP_FreeMemory( pp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����p�����[�^���̃������m��
 *	
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	PP�\���̕��̃�����
 */
//-----------------------------------------------------------------------------
POKEMON_PARAM *PokemonParam_AllocWork( HEAPID heapID )
{	
	return GFL_HEAP_AllocMemory( heapID, POKETOOL_GetWorkSize() );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����p�����[�^�̃R�s�[
 *
 *	@param	const POKEMON_PARAM *pp_src
 *	@param	*pp_dest 
 */
//-----------------------------------------------------------------------------
void WT_PokeCopyPPtoPP( const POKEMON_PARAM *pp_src, POKEMON_PARAM *pp_dest )
{	
	GFL_STD_MemCopy( pp_src, pp_dest, POKETOOL_GetWorkSize() );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���A
 *
 *	@param	GFL_BMPWIN * winGFLBMPWIN
 *	@param	trans_sw				�]���t���O
 */
//-----------------------------------------------------------------------------
void BmpMenuWinClear( GFL_BMPWIN * win, u8 trans_sw )
{	
	GFL_BMPWIN_ClearScreen( win );
	switch( trans_sw )
	{	
	case WINDOW_TRANS_ON:
		GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(win) );
		break;
	case WINDOW_TRANS_ON_V:
		GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	������W�J����MSG���擾
 *
 *	@param	WORDSET *wordset	���[�h�Z�b�g
 *	@param	*MsgManager				���b�Z�[�W
 *	@param	strID							����ID
 *	@param	heapID						�e���|�����q�[�vID
 *
 *	@return	������W�J���STRBUF
 */
//-----------------------------------------------------------------------------
STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID )
{	
	STRBUF	*src;
	STRBUF	*dst;
	src	= GFL_MSG_CreateString( MsgManager, strID );
	dst	= GFL_STR_CreateBuffer( GFL_STR_GetBufferLength( src ), heapID );

	WORDSET_ExpandStr( wordset, src, dst );

	GFL_STR_DeleteBuffer( src );

	return dst;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ppp����pp���쐬���A�R�s�[����
 *
 *	@param	const POKEMON_PASO_PARAM *ppp
 *	@param	*pp 
 */
//-----------------------------------------------------------------------------
void PokeReplace( const POKEMON_PASO_PARAM *ppp, POKEMON_PARAM *pp )
{	
	POKEMON_PARAM*  src	= PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
	WT_PokeCopyPPtoPP( src, pp );
	GFL_HEAP_FreeMemory( src );
}

//----------------------------------------------------------------------------
/**
 *	@brief	������R�s�[
 *
 *	@param	STRCODE *p_dst	�R�s�[��
 *	@param	STRCODE *cp_src	�R�s�[��
 *	@param	size						������
 */
//-----------------------------------------------------------------------------
void WT_PM_strncpy( STRCODE *p_dst, const STRCODE *cp_src, int len )
{	
	u32	i;

	for( i=0; i<len; i++ ){
		p_dst[i] = cp_src[i];
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	PP2PPP
 *
 *	@param	POKEMON_PARAM *pp 
 *
 *	@return	PPP
 */
//-----------------------------------------------------------------------------
POKEMON_PASO_PARAM* PPPPointerGet( POKEMON_PARAM *pp )
{	
	return (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst( pp );
}
