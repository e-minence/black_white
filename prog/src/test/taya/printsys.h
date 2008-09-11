//=============================================================================================
/**
 * @file	printsys.c
 * @brief	Bitmap���p�����`��@�V�X�e������
 * @author	taya
 *
 * @date	2007.02.06	�쐬
 * @date	2008.09.11	�ʐM�Ή��L���[�\�����쐬
 */
//=============================================================================================
#ifndef __PRINTSYS_H__
#define __PRINTSYS_H__

#include "gf_font.h"



//======================================================================================
// Typedefs
//======================================================================================

//--------------------------------------------------------------
/**
 *	Print Queue
 *	�ʐM���ɕ`�揈���������݂Ɏ��s�ł���悤�A�������e��~�ς��邽�߂̋@�\�B
 */
//--------------------------------------------------------------
typedef struct _PRINT_QUE	PRINT_QUE;

//--------------------------------------------------------------------------
/**
 *	Print Stream
 *	��b�E�B���h�E���A�P��Ԋu�łP�������`��C�\�����s�����߂̋@�\
 */
//--------------------------------------------------------------------------
typedef struct _PRINT_STREAM	PRINT_STREAM;

//--------------------------------------------------------------------------
/**
 *	Print Stream Callback
 *	Print Stream �łP�����`�悲�ƂɌĂяo�����R�[���o�b�N�֐��̌^
 */
//--------------------------------------------------------------------------
typedef void (*pPrintCallBack)(u32);


//======================================================================================
// Consts
//======================================================================================

//--------------------------------------------------------------------------
/**
 *	Print Stream �̏�Ԃ�����
 */
//--------------------------------------------------------------------------
typedef enum {
	PRINTSTREAM_STATE_RUNNING = 0,	///< �������s���i�����񂪗���Ă���j
	PRINTSTREAM_STATE_PAUSE,		///< ��~���i�y�[�W�؂�ւ��҂����j
	PRINTSTREAM_STATE_DONE,			///< ������I�[�܂ŕ\������
}PRINTSTREAM_STATE;

//--------------------------------------------------------------------------
/**
 *	Print Stream �̈ꎞ��~��Ԃ�����
 */
//--------------------------------------------------------------------------
typedef enum {
	PRINTSTREAM_PAUSE_LINEFEED = 0,	///< �y�[�W�؂�ւ��҂��i���s�j
	PRINTSTREAM_PAUSE_CLEAR,		/// <�y�[�W�؂�ւ��҂��i�\���N���A�j
}PRINTSTREAM_PAUSE_TYPE;



//==============================================================================================
/**
 * �V�X�e���������i�v���O�����N�����ɂP�x�����Ăяo���j
 *
 * @param   heapID		
 *
 */
//==============================================================================================
extern void PRINTSYS_Init( HEAPID heapID );

//==============================================================================================
/**
 * �v�����g�L���[���f�t�H���g�̃o�b�t�@�T�C�Y�Ő�������B
 *
 * @param   heapID			�����p�q�[�vID
 *
 * @retval  PRINT_QUE*		�������ꂽ�v�����g�L���[
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_Create( HEAPID heapID );

//==============================================================================================
/**
 * �v�����g�L���[���T�C�Y�w�肵�Đ�������
 *
 * @param   size			�o�b�t�@�T�C�Y
 * @param   heapID			�����p�q�[�vID
 *
 * @retval  PRINT_QUE*		�������ꂽ�v�����g�L���[
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_CreateEx( u16 buf_size, HEAPID heapID );

//==============================================================================================
/**
 * �v�����g�L���[���C�������i���C�����[�v���łP��Ăяo���j
 *
 * @param   que		�v�����g�L���[
 *
 * @retval  BOOL	�L���[�������c���Ă��Ȃ��ꍇ��TRUE�^�ɂ܂��������c���Ă���ꍇ��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_Main( PRINT_QUE* que );

//==============================================================================================
/**
 * �v�����g�L���[�ɏ������c���Ă��邩����
 *
 * @param   que		�v�����g�L���[
 *
 * @retval  BOOL	�L���[�������c���Ă��Ȃ��ꍇ��TRUE�^�ɂ܂��������c���Ă���ꍇ��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsFinished( const PRINT_QUE* que );

//==============================================================================================
/**
 * �v�����g�L���[�ɁA����Bitmap���o�͑ΏۂƂ����������c���Ă��邩����
 *
 * @param   que				�v�����g�L���[
 * @param   targetBmp		�o�͑Ώۂ�Bitmap
 *
 * @retval  BOOL			targetBmp�ɑ΂��鏈�����c���Ă���ꍇ��TRUE�^����ȊO��FALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsExistTarget( const PRINT_QUE* que, const GFL_BMP_DATA* targetBmp );


//==============================================================================================
/**
 * BITMAP�ɑ΂��镶����`��
 *
 * @param   que		[out] �`�揈�����e���L�^���邽�߂̃v�����g�L���[
 * @param   dst		[out] �`���Bitmap
 * @param   xpos	[in]  �`��J�n�w���W�i�h�b�g�j
 * @param   ypos	[in]  �`��J�n�x���W�i�h�b�g�j
 * @param   str		[in]  ������
 * @param   font	[in]  �t�H���g
 *
 */
//==============================================================================================
extern void PRINTSYS_Print( PRINT_QUE* que, GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font );




extern PRINT_STREAM_HANDLE PRINTSYS_PrintStream(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, u16 heapID, u16 clearColor, pPrintCallBack callback );

extern PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM_HANDLE handle );

extern PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamDelete( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamShortWait( PRINT_STREAM_HANDLE handle, u16 wait );
extern u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin );



//--------------------------------------------------------------
/**
 * �v�����g�L���[�p���[�e�B���e�B
 */
//--------------------------------------------------------------

typedef struct {
	GFL_BMPWIN*		win;
	u8				transReq;
}PRINT_UTIL;


inline void PRINT_UTIL_Setup( PRINT_UTIL* wk, GFL_BMPWIN* win )
{
	wk->win = win;
	wk->transReq = FALSE;
}

inline void PRINT_UTIL_Print( PRINT_UTIL* wk, PRINT_QUE* que, u16 xpos, u16 ypos, const STRBUF* buf, GFL_FONT* font )
{
	PRINTSYS_Print( que, GFL_BMPWIN_GetBmp(wk->win), xpos, ypos, buf, font );
	wk->transReq = TRUE;
}


inline BOOL PRINT_UTIL_Trans( PRINT_UTIL* wk, PRINT_QUE* que )
{
	if( wk->transReq )
	{
		if( !PRINTSYS_QUE_IsExistTarget(que, GFL_BMPWIN_GetBmp(wk->win)) )
		{
			GFL_BMPWIN_TransVramCharacter( wk->win );
			wk->transReq = FALSE;
		}
	}
	return wk->transReq;
}






#endif
