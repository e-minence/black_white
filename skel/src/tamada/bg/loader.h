//============================================================================================
/**
 * @file	loader.h
 * @brief	�ȈՉ摜���[�_
 * @author	taya
 * @date	2006.10.13
 */
//============================================================================================
#ifndef __LOADER_H__
#define __LOADER_H__

//------------------------------------------------------------------
/**
 * �������B��ʈÓ]���ɂP�񂾂��ĂԁB
 */
//------------------------------------------------------------------
extern void LDR_Init( void );


//------------------------------------------------------------------
/**
 * �]��
 *
 * @param   cgx			CGX�f�[�^
 * @param   cgx_size	CGX�f�[�^�T�C�Y�i�o�C�g�j
 * @param   scrn		SCR�f�[�^
 * @param   scrn_size	SCR�f�[�^�T�C�Y�i�o�C�g�j
 * @param   pltt		PAL�f�[�^
 * @param   pltt_size	PAL�f�[�^�T�C�Y�i�o�C�g�j
 * @param   main_sub_flag	TRUE:MAIN-LCD,  FALSE:SUB-LCD
 *
 */
//------------------------------------------------------------------
extern void LDR_Trans( const u8* cgx, u32 cgx_size,
	const u16* scrn, u32 scrn_size,
	const u16* pltt, u32 pltt_size,
	BOOL  main_sub_flag );


#endif
