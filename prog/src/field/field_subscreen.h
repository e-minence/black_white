//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_subscreen.h
 *	@brief		InforBer�̏������E�j���@
 *	@data		2009.03.26	fieldmap����ڐA
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_SUBSCREEN_H__
#define __FIELD_SUBSCREEN_H__

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define FIELD_SUBSCREEN_BGPLANE	(GFL_BG_FRAME3_S)
#define FIELD_SUBSCREEN_PALLET	(0xE)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern void FIELD_SUBSCREEN_Init( u32 heapID );
extern void FIELD_SUBSCREEN_Exit( void );
extern void FIELD_SUBSCREEN_Main( void );

#ifdef _cplusplus
}	// extern "C"{
#endif

#endif		// __FIELD_SUBSCREEN_H__


