//=============================================================================
/**
 * @file	net_alone_test.h
 * @brief	�ʐM�V�X�e�� �ڑ����萧���R�[�h��` 
 * @author	GAME FREAK Inc.
 * @date    2010.05.21
 *
 * @note  net_whpipe.c���������ꂽ�̂�.c���番�� by iwasawa
 */
//=============================================================================
#pragma once

#ifdef PM_DEBUG

// �v���O���}�͊�{���̐l�ƂȂ���Ȃ�  ��Ɏ����̃}�V���Ɛڑ����ĊJ���ł���悤��
// �f�o�b�O���j���[�������łŐ؂�ւ�����S���ƂȂ��邱�Ƃɂ���
// �S���ƂȂ���ԍ��͂O(���i��)

//�N�ł��q����(�f�o�b�O��)�@�p���X�̒ʐM�o�[�W�����Ƃ��Ďg�p �̂̒ʐM�Ɛڑ��Ŗ�肪����Ƃ��Ƀo�[�W�������グ�Ă���
#define _DEBUG_ALONETEST_DEFAULT  (116)

#ifdef DEBUG_ONLY_FOR_ohno
#define _DEBUG_ALONETEST (1)
#elif DEBUG_ONLY_FOR_sogabe
#define _DEBUG_ALONETEST (2)
#elif DEBUG_ONLY_FOR_matsuda
#define _DEBUG_ALONETEST (3)
#elif DEBUG_ONLY_FOR_gotou
#define _DEBUG_ALONETEST (4)
#elif DEBUG_ONLY_FOR_tomoya_takahashi
#define _DEBUG_ALONETEST (5)
#elif DEBUG_ONLY_FOR_tamada
#define _DEBUG_ALONETEST (6)
#elif DEBUG_ONLY_FOR_kagaya
#define _DEBUG_ALONETEST (7)
#elif DEBUG_ONLY_FOR_nohara
#define _DEBUG_ALONETEST (8)
#elif DEBUG_ONLY_FOR_taya
#define _DEBUG_ALONETEST (9)
#elif DEBUG_ONLY_FOR_hiro_nakamura
#define _DEBUG_ALONETEST (10)
#elif DEBUG_ONLY_FOR_mituhara
#define _DEBUG_ALONETEST (11)
#elif DEBUG_ONLY_FOR_watanabe
#define _DEBUG_ALONETEST (12)
#elif DEBUG_ONLY_FOR_genya_hosaka
#define _DEBUG_ALONETEST (13)
#elif DEBUG_ONLY_FOR_toru_nagihashi
#define _DEBUG_ALONETEST (14)
#elif DEBUG_ONLY_FOR_mori
#define _DEBUG_ALONETEST (15)
#elif DEBUG_ONLY_FOR_iwasawa
#define _DEBUG_ALONETEST (16)
#elif DEBUG_ONLY_FOR_ohmori
#define _DEBUG_ALONETEST (17)
//#elif DEBUG_ONLY_FOR_ariizumi_nobuhiko
//#define _DEBUG_ALONETEST (18)
#elif DEBUG_ONLY_FOR_palace_debug
#define _DEBUG_ALONETEST (19)
#elif DEBUG_ONLY_FOR_palace_debug_white
#define _DEBUG_ALONETEST (20)
#elif DEBUG_ONLY_FOR_palace_debug_black
#define _DEBUG_ALONETEST (21)
#elif DEBUG_ONLY_FOR_debug_comm
#define _DEBUG_ALONETEST (22)
#else
//�N�ł��q����A�A�A���A�p���X�̒ʐM�o�[�W�����Ƃ��Ďg�p �̂̒ʐM�Ɛڑ��Ŗ�肪����Ƃ��Ƀo�[�W�������グ�Ă���
#define _DEBUG_ALONETEST (_DEBUG_ALONETEST_DEFAULT)
#endif

#endif  //PM_DEBUG

