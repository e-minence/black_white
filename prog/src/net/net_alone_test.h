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
//�f���p���[�z�MROM�͐��i�ł�make���Ȃ��O��̂��߂����łO�w��
#define _DEBUG_ALONETEST_DEFAULT  (0)

#define _DEBUG_ALONETEST (_DEBUG_ALONETEST_DEFAULT)

#endif  //PM_DEBUG

