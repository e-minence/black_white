//============================================================================================
/**
 * @file	b_app_tool.h
 * @brief	�퓬�p�A�v�����ʏ���
 * @author	Hiroyuki Nakamura
 * @date	06.03.30
 */
//============================================================================================
#ifndef B_APP_TOOL_H
#define B_APP_TOOL_H

//#include "battle_cursor.h"
#include "print/printsys.h"
#include "system/cursor_move.h"


//============================================================================================
//	�萔��`
//============================================================================================
//typedef struct _BAPP_CURSOR_MVWK	BAPP_CURSOR_MVWK;

typedef struct _BAPP_CURSOR_PUT_WORK	BAPP_CURSOR_PUT_WORK;

//#define	BAPP_CMV_CANCEL		( 0xfffffffe )	// �L�����Z��
//#define	BAPP_CMV_NONE		( 0xffffffff )	// ���삵

//#define	BAPP_CMV_RETBIT		( 0x80 )		// �J�[�\���ړ��őO��̈ʒu�֖߂�

/*
typedef struct {
	GFL_BMPWIN * win;
	BOOL	flg;
}BAPP_BMPWIN_QUE;
*/
/*
typedef struct {
	u8	px;
	u8	py;
	u8	sx;
	u8	sy;
}BAPP_CURSOR_PUT;
*/

#define	BAPPTOOL_CURSOR_MAX		( 4 )		// �J�[�\���Ŏg�p����Z���A�N�^�[�̐�


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�쐬
 *
 * @param	heap	�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 */
//--------------------------------------------------------------------------------------------
//extern BAPP_CURSOR_MVWK * BAPP_CursorMoveWorkAlloc( u32 heap );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N���
 *
 * @param	heap	�q�[�vID
 *
 * @return	�J�[�\���ړ����[�N
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMoveWorkFree( BAPP_CURSOR_MVWK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\���f�[�^
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\���f�[�^
 */
//--------------------------------------------------------------------------------------------
//extern BCURSOR_PTR BAPP_CursorMvWkGetBCURSOR_PTR( BAPP_CURSOR_MVWK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\���ʒu
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\���ʒu
 */
//--------------------------------------------------------------------------------------------
//extern u8 BAPP_CursorMvWkGetPos( BAPP_CURSOR_MVWK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�擾�F�J�[�\��ON/OFF
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	�J�[�\��ON/OFF
 */
//--------------------------------------------------------------------------------------------
//extern u8 BAPP_CursorMvWkGetFlag( BAPP_CURSOR_MVWK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\��ON/OFF
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	flg		ON/OFF�t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkSetFlag( BAPP_CURSOR_MVWK * wk, u8 flg );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���f�[�^
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	flg		�J�[�\���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkSetBCURSOR_PTR( BAPP_CURSOR_MVWK * wk, BCURSOR_PTR cur );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���ʒu
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	pos		�J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkSetPos( BAPP_CURSOR_MVWK * wk, u8 pos );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ʒu������
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkPosInit( BAPP_CURSOR_MVWK * wk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�J�[�\���ʒu���[�N
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	pwk		�J�[�\���ʒu���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkSetPoint( BAPP_CURSOR_MVWK * wk, const POINTSEL_WORK * pwk );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ����[�N�Z�b�g�F�ړ��e�[�u��
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 * @param	mv_tbl	�ړ��e�[�u��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BAPP_CursorMvWkSetMvTbl( BAPP_CURSOR_MVWK * wk, u32 mv_tbl );

//--------------------------------------------------------------------------------------------
/**
 * �I���J�[�\���ړ�
 *
 * @param	wk		�I���J�[�\���ړ����[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
//extern u32 BAPP_CursorMove( BAPP_CURSOR_MVWK * wk );



extern BAPP_CURSOR_PUT_WORK * BAPPTOOL_CreateCursor( HEAPID heapID );
extern void BAPPTOOL_FreeCursor( BAPP_CURSOR_PUT_WORK * wk );
//extern void BAPPTOOL_SetCursorPutData( BAPP_CURSOR_PUT_WORK * wk, const BAPP_CURSOR_PUT * put );
extern void BAPPTOOL_AddCursor( BAPP_CURSOR_PUT_WORK * wk, GFL_CLUNIT * clunit, u32 chrRes, u32 palRes, u32 celRes );
extern void BAPPTOOL_DelCursor( BAPP_CURSOR_PUT_WORK * wk );
extern void BAPPTOOL_VanishCursor( BAPP_CURSOR_PUT_WORK * wk, BOOL flg );
//extern void BAPPTOOL_MoveCursor( BAPP_CURSOR_PUT_WORK * wk, u32 point );
extern void BAPPTOOL_MoveCursorPoint( BAPP_CURSOR_PUT_WORK * wk, const CURSORMOVE_DATA * dat );



//extern void BAPPTOOL_PrintQueOn( BAPP_BMPWIN_QUE * dat );
//extern void BAPPTOOL_PrintQueTrans( PRINT_UTIL * util, PRINT_QUE * que, u32 max );
extern void BAPPTOOL_PrintScreenTrans( PRINT_UTIL * util );
extern void BAPPTOOL_PrintUtilTrans( PRINT_UTIL * util, PRINT_QUE * que, u32 max );

#define	BAPPTOOL_SET_STR_SCRN_END		( 0xff )		// �]���E�B���h�E�I��

extern void BAPPTOOL_SetStrScrn( PRINT_UTIL * util, const u8 * tbl );


#endif	// B_APP_TOOL_H
