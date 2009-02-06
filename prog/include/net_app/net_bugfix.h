//==============================================================================
/**
 * @file	net_bugfix.h
 * @brief	PL����̈ڐA�v���O������make��ʂ��ׂɖ������Ȏ�������
 * @author	matsuda
 * @date	2008.12.18(��)
 *
 * �ŏI�I�ɂ͂��̃t�@�C���͍폜����
 *
 */
//==============================================================================
#ifndef __NET_BUGFIX_H__
#define __NET_BUGFIX_H__


//==============================================================================
//	�}�N����`
//==============================================================================
/// �F�ԍ��w�� PRINT �̂��߂̒萔�E�}�N����`

typedef u32		GF_PRINTCOLOR;

#define  GF_PRINTCOLOR_MASK			(0xff)
#define  GF_PRINTCOLOR_LETTER_SHIFT	(16)
#define  GF_PRINTCOLOR_SHADOW_SHIFT	(8)
#define  GF_PRINTCOLOR_GROUND_SHIFT	(0)

// GF_MSG_PrintColor �̈��� GF_PRINT_COLOR ���쐬���邽�߂̃}�N���i �����F�A�e�F�A�w�i�F�̏��ɔԍ���n�� �j
#define  GF_PRINTCOLOR_MAKE(letter, shadow, ground)	(GF_PRINTCOLOR)(	(((letter)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_LETTER_SHIFT)|\
																		(((shadow)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_SHADOW_SHIFT)|\
																		(((ground)&GF_PRINTCOLOR_MASK) << GF_PRINTCOLOR_GROUND_SHIFT)	)

// GF_PRINTCOLOR_MAKE �ō�����l����e�F�ԍ������o���}�N��
#define GF_PRINTCOLOR_GET_LETTER(c)		(((c)>>GF_PRINTCOLOR_LETTER_SHIFT)&GF_PRINTCOLOR_MASK)
#define GF_PRINTCOLOR_GET_SHADOW(c)		(((c)>>GF_PRINTCOLOR_SHADOW_SHIFT)&GF_PRINTCOLOR_MASK)
#define GF_PRINTCOLOR_GET_GROUND(c)		(((c)>>GF_PRINTCOLOR_GROUND_SHIFT)&GF_PRINTCOLOR_MASK)




#endif	//__NET_BUGFIX_H__

