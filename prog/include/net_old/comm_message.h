//=============================================================================
/**
 * @file	comm_message.h
 * @brief	�ʐM�p ���b�Z�[�W���ȒP�Ɉ������߂̃N���X
 * @author	k.ohno
 * @date    2006.02.05
 */
//=============================================================================

#pragma once

#include "gflib.h"


/// �ʐM�G���[�p�E�C���h�E���o��
extern void CommErrorMessageStart(int heapID);

/// �ʐM�G���[���������Ă��ăG���[�ɂȂ�Ǝ~�܂�
extern void CommErrorCheck(int heapID);

/// �ʐM�G���[���������Ă��ăG���[�ɂȂ�Ǝ~�܂�
extern void CommErrorDispCheck(int heapID);


