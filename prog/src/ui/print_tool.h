//============================================================================================
/**
 * @file		print_tool.h
 * @brief		�����`��c�[��
 * @author	Hiroyuki Nakamura
 * @date		09.10.01
 *
 *	���W���[�����FPRINTTOOL
 */
//============================================================================================
#pragma	once

#include "print/printsys.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �����\�����[�h
enum {
	PRINTTOOL_MODE_LEFT = 0,	// ���l
	PRINTTOOL_MODE_RIGHT,			// �E�l
	PRINTTOOL_MODE_CENTER			// ����
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

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
extern void PRINTTOOL_Print(
				PRINT_UTIL * wk, PRINT_QUE * que, u16 x, u16 y,
				const STRBUF * buf, GFL_FONT * font, u32 mode );


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
extern void PRINTTOOL_PrintColor(
				PRINT_UTIL * wk, PRINT_QUE * que, u16 x, u16 y,
				const STRBUF * buf, GFL_FONT * font, PRINTSYS_LSB color, u32 mode );

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
extern void PRINTTOOL_PrintFraction(
							PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
							u16 x, u16 y, u32 nowPrm, u32 maxPrm, HEAPID heapID );

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
extern void PRINTTOOL_PrintFractionColor(
							PRINT_UTIL * wk, PRINT_QUE * que, GFL_FONT * font,
							u16 x, u16 y, PRINTSYS_LSB color, u32 nowPrm, u32 maxPrm, HEAPID heapID );
