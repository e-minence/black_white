//=============================================================================
/**
 * @file	comm_command_mystery.h
 * @brief	�ʐM�̃R�}���h�ꗗ  �ӂ����Ȃ�������̗p
 * @author	Satoshi Mitsuhara
 * @date	2006.05.16
 */
//=============================================================================

#ifndef __COMM_COMMAND_MYSTERY_H__
#define __COMM_COMMAND_MYSTERY_H__

#include "net/network_define.h"

/// �ӂ����Ȃ�������̐�p�ʐM�R�}���h�̒�`�B
enum CommCommandField_e {
	CM_GIFT_DATA = GFL_NET_CMD_MYSTERY,	// �v���[���g�̃f�[�^�𑗂�
	CM_RECV_RESULT,						// �����Ǝ󂯎��܂�����ԓ�
	//------------------------------------------------�����܂�
	CM_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};

extern void CommCommandMysteryInitialize(void* pWork);

#endif	// __COMM_COMMAND_MYSTERY_H__

/*  */
