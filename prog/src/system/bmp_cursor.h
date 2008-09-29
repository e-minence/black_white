//======================================================================
/**
 * @file	bmp_cursor.h
 * @brief	BitmapWindow�Ƀ��j���[�I��p�̃J�[�\����`�悷��d�g��
 * @author	taya
 * @date	2006.01.12
 */
//======================================================================
#ifndef __BMP_CURSOR_H__
#define __BMP_CURSOR_H__

//#include "gflib\msg_print.h"

#include <tcbl.h>
#include "print\printsys.h"
#include "print\gf_font.h"

//======================================================================
//
//======================================================================
typedef struct _BMPCURSOR	BMPCURSOR;

//======================================================================
//
//======================================================================
//------------------------------------------------------------------
/**
 * Bitmap�J�[�\�����쐬
 * @param   heapID		�쐬��q�[�v�h�c
 * @retval  BMPCURSOR*		�쐬���ꂽ�J�[�\���I�u�W�F�N�g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern BMPCURSOR * BmpCursor_Create( u32 heapID );

//------------------------------------------------------------------
/**
 * Bitmap�J�[�\����j��
 * @param   bmpCursor		Bitmap�J�[�\���I�u�W�F�N�g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void BmpCursor_Delete( BMPCURSOR* bmpCursor );

//------------------------------------------------------------------
/**
 * Bitmap�J�[�\���̃J���[�ύX
 * @param   bmpCursor		Bitmap�J�[�\���I�u�W�F�N�g�ւ̃|�C���^
 * @param   color			�J���[�l�iGF_PRINTCOLOR_MAKE�}�N�����g�p�j
 */
//------------------------------------------------------------------
#if 0
extern void BmpCursor_ChangeColor(
	BMPCURSOR* bmpCursor, GF_PRINTCOLOR color );
#endif

//------------------------------------------------------------------
/**
 * Bitmap�J�[�\����BitmapWindow�ɕ`�悷��
 * @param   bmpCursor	Bitmap�J�[�\���I�u�W�F�N�g�ւ̃|�C���^
 * @param   win			BitmapWindow
 * @param   x			�`��w���W�i�h�b�g�P�ʁj
 * @param   y			�`��x���W�i�h�b�g�P�ʁj
 */
//------------------------------------------------------------------
#if 0
extern void BmpCursor_Print(
	const BMPCURSOR *bmpCursor, GFL_BMPWIN *win, u32 x, u32 y );
#endif
extern void BmpCursor_Print( const BMPCURSOR *bmpCursor, u32 x, u32 y,
	PRINT_UTIL *printUtil, PRINT_QUE *printQue, GFL_FONT *fontHandle );


extern void BmpCursor_SetCursorFont(
	BMPCURSOR *bmpCursor, const STRBUF *str, u32 heap_id );
extern void BmpCursor_SetCursorFontMsg(
	BMPCURSOR *bmpCursor, GFL_MSGDATA *msgdata, u32 strID );

#endif	//__BMP_CURSOR_H__
