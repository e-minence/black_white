//=============================================================================
/**
 * @file	comm_mystery_state.h
 * @brief	�ʐM��Ԃ��Ǘ�����T�[�r�X  �ʐM�̏�ʂɂ���
 *          	�X���b�h�̂ЂƂƂ��ē����A�����̒ʐM��Ԃ⑼�̋@���
 *          	�J�n��I�����Ǘ�����
 *		��comm_field_state.c�̐^�����q
 * @author	Satoshi Mitsuhara
 * @date    	2006.05.17
 */
//=============================================================================
#ifndef __COMM_MYSTERY_STATE_H__
#define __COMM_MYSTERY_STATE_H__

#include "comm_mystery_gift.h"
#include "net/network_define.h"


extern void CommMysteryStateEnterGiftParent(MYSTERYGIFT_WORK *pMSys, SAVE_CONTROL_WORK *sv, int serviceNo);
extern int CommMysteryGetCommChild(void);
extern void CommMysterySendGiftDataParent(const void *p, int size);


extern void CommMysteryStateEnterGiftChild(MYSTERYGIFT_WORK *pMSys, int serviceNo);
extern void CommMysteryStateConnectGiftChild(int connectIndex);
extern int CommMysteryCheckParentBeacon(MYSTERYGIFT_WORK *wk);
extern int CommMysteryCheckRecvData(void);
extern void CommMysteryResultRecvData(void);

extern int CommMysteryGetRecvPlaceSize(void);
extern u8* CommGetMysteryGiftRecvBuff( int netID, void* pWork, int size);
extern int CommMysteryGiftGetRecvCheck(void);
extern void CommMysteryExitGift(void);

//���̏ꏊ�Ń��C�u������������������K�v���o�Ă����̂�
extern void CommMysteryInitNetLib(void* pWork);

/// �ӂ����Ȃ�������̐�p�ʐM�R�}���h�̒�`�B
enum CommCommandField_e {
	CM_GIFT_DATA = GFL_NET_CMD_MYSTERY,	// �v���[���g�̃f�[�^�𑗂�
	CM_RECV_RESULT,						// �����Ǝ󂯎��܂�����ԓ�
	//------------------------------------------------�����܂�
	CM_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};

extern void CommCommandMysteryInitialize(void* pWork);

#endif	// __COMM_MYSTERY_STATE_H__
/*  */
