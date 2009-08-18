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

//=============================================================================
/**
 *					�v�����g�t�H���g����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	������̕���Ԃ�
 *
 *	@param	WT_PRINT *wk	�v�����g���
 *	@param	*buf		������
 *	@param	magin		�}�[�W��
 *
 *	@return	��
 */
//-----------------------------------------------------------------------------
int FontProc_GetPrintStrWidth( WT_PRINT *wk, u8 font_idx, STRBUF *buf, int magin )
{ 
	return PRINTSYS_GetStrWidth( buf, wk->font, magin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	������\���ɕK�v�ȏ���������
 *
 *	@param	WT_PRINT *wk	���[�N
 *	@param	CONFIG *cfg		�R���t�B�O
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Init( WT_PRINT *wk, const CONFIG *cfg )
{	
	wk->tcbsys	= GFL_TCBL_Init( HEAPID_WORLDTRADE, HEAPID_WORLDTRADE, 8, 32 );
	wk->cfg			= cfg;
	wk->font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WORLDTRADE );
	wk->que			= PRINTSYS_QUE_Create( HEAPID_WORLDTRADE );

	GFL_STD_MemClear( &wk->util, sizeof(PRINT_UTIL) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��������ɕK�v�ȏ���j��
 *
 *	@param	WT_PRINT *wk	���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Exit( WT_PRINT *wk )
{	
	PRINTSYS_QUE_Delete( wk->que );
	GFL_FONT_Delete( wk->font );
	GFL_TCBL_Exit( wk->tcbsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	������\��ɕK�v�ȏ��	���C������
 *
 *	@param	WT_PRINT *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Main( WT_PRINT *wk )
{	
	GFL_TCBL_Main( wk->tcbsys );
	PRINT_UTIL_Trans( &wk->util, wk->que );
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PRINT_STREAM *stream	�X�g���[��
 *
 *	@return	FALSE�Ȃ�Ε�����`��I��	TRUE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL GF_MSG_PrintEndCheck( WT_PRINT *setup )
{
	if( setup->stream == NULL )
	{	
		return FALSE;
	}

	if( PRINTSYS_PrintStreamGetState(setup->stream) == PRINTSTREAM_STATE_DONE )
	{	
		PRINTSYS_PrintStreamDelete( setup->stream );
		setup->stream	= NULL;
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_BMPWIN *bmpwin	BMPWIN
 *	@param	*str	������
 *	@param	x			�\���ʒuX
 *	@param	y			�\���ʒuY
 *	@param	WT_PRINT *setup		������ɕK�v�ȏ��
 *
 *	@return	�v�����g�X�g���[��
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintSimple( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, WT_PRINT *setup )
{
	GF_ASSERT( setup->stream == NULL );

	setup->stream	= PRINTSYS_PrintStream(
    bmpwin, x, y, str, setup->font,
    CONFIG_GetMsgPrintSpeed( setup->cfg ), setup->tcbsys, 0, HEAPID_WORLDTRADE, 0x0f );
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�F���ꊇ�\��
 *
 *	@param	GFL_BMPWIN *bmpwin	��������BMPWIN
 *	@param	*str	������
 *	@param	x			X
 *	@param	y			Y
 *	@param	color	�F
 *	@param	*setup	������p���
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintColor( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, int put_type, PRINTSYS_LSB color, WT_PRINT *setup )
{
	PRINT_UTIL_Setup( &setup->util, bmpwin );
	PRINT_UTIL_PrintColor( &setup->util, setup->que, x, y, str, setup->font, color );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(bmpwin) );
}




