
#ifndef __PRINTSYS_H__
#define __PRINTSYS_H__

#include "gf_font.h"

// �X�g���[�����
typedef enum {
	PRINTSTREAM_STATE_RUNNING = 0,	///< �������s���i�����񂪗���Ă���j
	PRINTSTREAM_STATE_PAUSE,		///< ��~���i�y�[�W�؂�ւ��҂����j
	PRINTSTREAM_STATE_DONE,			///< ������I�[�܂ŕ\������
}PRINTSTREAM_STATE;


// �X�g���[����~���
typedef enum {
	PRINTSTREAM_PAUSE_LINEFEED = 0,	///< �y�[�W�؂�ւ��҂��i���s�j
	PRINTSTREAM_PAUSE_CLEAR,		/// <�y�[�W�؂�ւ��҂��i�\���N���A�j
}PRINTSTREAM_PAUSE_TYPE;

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���i����镶����j�֘A
 */
//--------------------------------------------------------------
typedef struct _PRINT_STREAM_WORK*	PRINT_STREAM_HANDLE;	///< 
typedef void (*pPrintCallBack)(u32);


extern void PRINTSYS_Init( HEAPID heapID );
extern void PRINTSYS_Print( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font );
extern PRINT_STREAM_HANDLE PRINTSYS_PrintStream(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, u16 heapID, u16 clearColor, pPrintCallBack callback );
extern PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM_HANDLE handle );
extern PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamDelete( PRINT_STREAM_HANDLE handle );
extern void PRINTSYS_PrintStreamShortWait( PRINT_STREAM_HANDLE handle, u16 wait );
extern u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin );


#endif
