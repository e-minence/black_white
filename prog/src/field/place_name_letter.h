////////////////////////////////////////////////////////////////////////////
/**
 * @file   place_name_letter.h
 * @brief  �n���\���̕����I�u�W�F�N�g
 * @author obata_toshihiro
 * @date   2010.03.28
 */
////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "print/gf_font.h"  // for GFL_FONT
#include "system/bmp_oam.h" // for BmpOam_xxxx


//=========================================================================
// ���s���S�^
//=========================================================================
typedef struct _PN_LETTER PN_LETTER;


//=========================================================================
// ���Z�b�g�A�b�v�E�p�����[�^
//=========================================================================
typedef struct {

	float x;		// ���W
	float y;		// 
	float sx;		// ���x
	float sy;		// 
	float dx;		// �ڕW�ʒu
	float dy;		// 
	float dsx;	// �ڕW�ʒu�ł̑��x
	float dsy;	// 

  GFL_FONT* font; // �t�H���g
  STRCODE   code; // ����

} PN_LETTER_SETUP_PARAM;


//=========================================================================
// ��public functions
//=========================================================================
extern PN_LETTER* PN_LETTER_Create( HEAPID heapID, BMPOAM_SYS_PTR bmpOamSys, u16 surface, u32 plttRegIdx );
extern void PN_LETTER_Delete( PN_LETTER* letter );
extern void PN_LETTER_Setup( PN_LETTER* letter, const PN_LETTER_SETUP_PARAM* param );
extern void PN_LETTER_Main( PN_LETTER* letter );
extern void PN_LETTER_MoveStart( PN_LETTER* letter );
extern const GFL_BMP_DATA* PN_LETTER_GetBitmap( const PN_LETTER* letter );
extern void PN_LETTER_SetDrawEnable( PN_LETTER* letter, BOOL enable );
extern int PN_LETTER_GetWidth( const PN_LETTER* letter );
extern int PN_LETTER_GetHeight( const PN_LETTER* letter );
extern int PN_LETTER_GetLeft( const PN_LETTER* letter );
extern int PN_LETTER_GetTop( const PN_LETTER* letter );
extern BOOL PN_LETTER_IsMoving( const PN_LETTER* letter );
