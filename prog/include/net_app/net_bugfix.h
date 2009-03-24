//==============================================================================
/**
 * @file	net_bugfix.h
 * @brief	PL����̈ڐA�v���O������make��ʂ��ׂɖ������Ȏ�������
 * @author	matsuda
 * @date	2008.12.18(��)
 *
 * �ŏI�I�ɂ͂��̃t�@�C���͍폜����	��check
 *
 */
//==============================================================================
#ifndef __NET_BUGFIX_H__
#define __NET_BUGFIX_H__


//==============================================================================
//	�}�N����`
//==============================================================================
/// �F�ԍ��w�� PRINT �̂��߂̒萔�E�}�N����`

typedef u16		GF_PRINTCOLOR;

#define  GF_PRINTCOLOR_MASK			(0xff)
#define  GF_PRINTCOLOR_LETTER_SHIFT	(10)
#define  GF_PRINTCOLOR_SHADOW_SHIFT	(5)
#define  GF_PRINTCOLOR_GROUND_SHIFT	(0)

// GF_MSG_PrintColor �̈��� GF_PRINT_COLOR ���쐬���邽�߂̃}�N���i �����F�A�e�F�A�w�i�F�̏��ɔԍ���n�� �j
#define  GF_PRINTCOLOR_MAKE(letter, shadow, ground)	(GF_PRINTCOLOR)(	(((letter)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_LETTER_SHIFT)|\
																		(((shadow)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_SHADOW_SHIFT)|\
																		(((ground)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_GROUND_SHIFT)	)

// GF_PRINTCOLOR_MAKE �ō�����l����e�F�ԍ������o���}�N��
#define GF_PRINTCOLOR_GET_LETTER(c)		(((c)>>GF_PRINTCOLOR_LETTER_SHIFT)&GF_PRINTCOLOR_MASK)
#define GF_PRINTCOLOR_GET_SHADOW(c)		(((c)>>GF_PRINTCOLOR_SHADOW_SHIFT)&GF_PRINTCOLOR_MASK)
#define GF_PRINTCOLOR_GET_GROUND(c)		(((c)>>GF_PRINTCOLOR_GROUND_SHIFT)&GF_PRINTCOLOR_MASK)


// �ʏ�̃t�H���g�J���[
#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(5)
#define	FBMP_COL_GRN_SDW	(6)
#define	FBMP_COL_BLUE		(7)
#define	FBMP_COL_BLU_SDW	(8)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE		(15)


// FONT_SYSTEM, FONT_BUTTON �������Ȃ����̂ŁA�Ƃ肠����MAKE��ʂ��ׂɉ��u���p
#define NET_FONT_SYSTEM			(0)
#define NET_FONT_BUTTON			(0)


//==============================================================================
//	�\���̒�`
//==============================================================================
///BMPWIN_DAT��WB�ł͑��݂��Ȃ��ׁA�u���p�̍\����
typedef struct{
	u8	frm_num;	///<�E�C���h�E�g�p�t���[��
	u8	pos_x;		///<�E�C���h�E�̈�̍����X���W�i�L�����P�ʂŎw��j
	u8	pos_y;		///<�E�C���h�E�̈�̍����Y���W�i�L�����P�ʂŎw��j
	u8	siz_x;		///<�E�C���h�E�̈��X�T�C�Y�i�L�����P�ʂŎw��j
	u8	siz_y;		///<�E�C���h�E�̈��Y�T�C�Y�i�L�����P�ʂŎw��j
	u8	palnum;		///<�E�C���h�E�̈�̃p���b�g�i���o�[
	u16	chrnum;		///<�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
}NET_BMPWIN_DAT;


//==============================================================================
//	inline�֐�
//==============================================================================
#include "arc_def.h"
#include "font/font.naix"

//--------------------------------------------------------------
/**
 * @brief   �V�X�e���t�H���g�p���b�g�]��
 *
 * @param   type		
 * @param   offs		
 * @param   heap		
 *
 * �����_�ł̓V�X�e���t�H���g���b�t�H���g�Ƃ��������̂����݂��邩�͕s���Ȃ̂�
 * �ڐA����ꍇ�͂����ɏW�񂵂Ă���
 */
//--------------------------------------------------------------
static inline void SystemFontPaletteLoad(u32 type, u32 offs, u32 heap)
{
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
		offs, 0x20, heap);
}

//--------------------------------------------------------------
/**
 * @brief   ��b�t�H���g�p���b�g�]��
 *
 * @param   type		
 * @param   offs		
 * @param   heap		
 *
 * �����_�ł̓V�X�e���t�H���g���b�t�H���g�Ƃ��������̂����݂��邩�͕s���Ȃ̂�
 * �ڐA����ꍇ�͂����ɏW�񂵂Ă���
 */
//--------------------------------------------------------------
static inline void TalkFontPaletteLoad(u32 type, u32 offs, u32 heap)
{
	GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
		offs, 0x20, heap);
}


#endif	//__NET_BUGFIX_H__

