//============================================================================================
/**
 * @file		print_tool.c
 * @brief		�����`��c�[��
 * @author	Hiroyuki Nakamura
 * @date		09.10.01
 *
 *	���W���[�����FPRINTTOOL
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "print/wordset.h"
#include "msg/msg_print_tool.h"

#include "print_tool.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	NUM_KETA				( 8 )							// �����\���̍ő包���@8��
#define	NUM_EXPBUF_SIZ	( NUM_KETA + 2 )	// ���\���̃o�b�t�@�T�C�Y�@8��+EOM+�Ȃ�ƂȂ������ŁB


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static u16 PutSizeGet( const STRBUF * buf, GFL_FONT * font, u32 mode );


//--------------------------------------------------------------------------------------------
/**
 * @brief		����W���w�肵�ĕ�����`��
 *
 * @param		wk			PRINT_UTIL
 * @param		que			PRINT_QUE
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		buf			������
 * @param		font		�t�H���g
 * @param		mode		�\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PRINTTOOL_Print(
				PRINT_UTIL * wk, PRINT_QUE * que, u16 x, u16 y,
				const STRBUF * buf, GFL_FONT * font, u32 mode )
{
	x -= PutSizeGet( buf, font, mode );
	PRINT_UTIL_Print( wk, que, x, y, buf, font );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		����W���w�肵�ĕ�����`��i�J���[�w��j
 *
 * @param		wk			PRINT_UTIL
 * @param		que			PRINT_QUE
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		buf			������
 * @param		font		�t�H���g
 * @param		color		�����F
 * @param		mode		�\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PRINTTOOL_PrintColor(
				PRINT_UTIL * wk, PRINT_QUE * que, u16 x, u16 y,
				const STRBUF * buf, GFL_FONT * font, PRINTSYS_LSB color, u32 mode )
{
	x -= PutSizeGet( buf, font, mode );
	PRINT_UTIL_PrintColor( wk, que, x, y, buf, font, color );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		[ ?? / ?? ] �\��
 *
 * @param		wk			PRINT_UTIL
 * @param		que			PRINT_QUE
 * @param		font		�t�H���g
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		nowPrm	���݂̒l
 * @param		maxPrm	�ő�l
 * @param		heapID	�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PRINTTOOL_PrintFraction(
					PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
					u16 x, u16 y, u32 nowPrm, u32 maxPrm, HEAPID heapID )
{
	GFL_MSGDATA * mman;
	WORDSET * wset;
	STRBUF * str;
	STRBUF * buf;
	u16	siz;

	mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_print_tool_dat, heapID );
	wset = WORDSET_Create( heapID );
	buf  = GFL_STR_CreateBuffer( NUM_EXPBUF_SIZ, heapID );

	//�u�^�v
	str = GFL_MSG_CreateString( mman, str_slash );
	siz = PRINTSYS_GetStrWidth( str, font, 0 );
	PRINTTOOL_Print( wk, que, x, y, str, font, PRINTTOOL_MODE_CENTER );
	GFL_STR_DeleteBuffer( str );
	// nowPrm
	str = GFL_MSG_CreateString( mman, str_now_param );
	WORDSET_RegisterNumber( wset, 0, nowPrm, NUM_KETA, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	PRINTTOOL_Print( wk, que, x-(siz/2), y, buf, font, PRINTTOOL_MODE_RIGHT );
	GFL_STR_DeleteBuffer( str );
	// maxPrm
	str = GFL_MSG_CreateString( mman, str_max_param );
	WORDSET_RegisterNumber( wset, 0, maxPrm, NUM_KETA, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	PRINTTOOL_Print( wk, que, x+siz, y, buf, font, PRINTTOOL_MODE_LEFT );
	GFL_STR_DeleteBuffer( str );

	GFL_STR_DeleteBuffer( buf );
	WORDSET_Delete( wset );
	GFL_MSG_Delete( mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		[ ?? / ?? ] �\���i�J���[�w��j
 *
 * @param		wk			PRINT_UTIL
 * @param		que			PRINT_QUE
 * @param		font		�t�H���g
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		color		�����F
 * @param		nowPrm	���݂̒l
 * @param		maxPrm	�ő�l
 * @param		heapID	�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PRINTTOOL_PrintFractionColor(
					PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
					u16 x, u16 y, PRINTSYS_LSB color, u32 nowPrm, u32 maxPrm, HEAPID heapID )
{
	GFL_MSGDATA * mman;
	WORDSET * wset;
	STRBUF * str;
	STRBUF * buf;
	u16	siz;

	mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_print_tool_dat, heapID );
	wset = WORDSET_Create( heapID );
	buf  = GFL_STR_CreateBuffer( NUM_EXPBUF_SIZ, heapID );

	//�u�^�v
	str = GFL_MSG_CreateString( mman, str_slash );
	siz = PRINTSYS_GetStrWidth( str, font, 0 );
	PRINTTOOL_PrintColor( wk, que, x, y, str, font, color, PRINTTOOL_MODE_CENTER );
	GFL_STR_DeleteBuffer( str );
	// nowPrm
	str = GFL_MSG_CreateString( mman, str_now_param );
	WORDSET_RegisterNumber( wset, 0, nowPrm, NUM_KETA, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	PRINTTOOL_PrintColor( wk, que, x-(siz/2), y, buf, font, color, PRINTTOOL_MODE_RIGHT );
	GFL_STR_DeleteBuffer( str );
	// maxPrm
	str = GFL_MSG_CreateString( mman, str_max_param );
	WORDSET_RegisterNumber( wset, 0, maxPrm, NUM_KETA, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wset, buf, str );
	PRINTTOOL_PrintColor( wk, que, x+siz, y, buf, font, color, PRINTTOOL_MODE_LEFT );
	GFL_STR_DeleteBuffer( str );

	GFL_STR_DeleteBuffer( buf );
	WORDSET_Delete( wset );
	GFL_MSG_Delete( mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		������\���T�C�Y����e���[�h�̕␳�l���擾
 *
 * @param		buf			������
 * @param		font		�t�H���g
 * @param		mode		�\�����[�h
 *
 * @return	�␳�l
 */
//--------------------------------------------------------------------------------------------
static u16 PutSizeGet( const STRBUF * buf, GFL_FONT * font, u32 mode )
{
	// �E�l
	if( mode == PRINTTOOL_MODE_RIGHT ){
		return PRINTSYS_GetStrWidth( buf, font, 0 );
	// ����
	}else if( mode == PRINTTOOL_MODE_CENTER ){
		return ( PRINTSYS_GetStrWidth( buf, font, 0 ) / 2 );
	}
	return 0;
}
