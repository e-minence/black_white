//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf.h
 *	@brief		�Q�`�S�l��p�ҍ���	�ʐM�R�}���h
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __COMM_COMMAND_WFP2PMF_H__
#define __COMM_COMMAND_WFP2PMF_H__

#include "net/network_define.h"

enum CommCommandTradeList_e {

	//-------------------------------------
	//�@�Q�[���p
	//=====================================
	// �����p	
	CNM_WFP2PMF_RESULT = GFL_NET_CMD_WIFICLUB,	///< �ʐMKONG		�e�[���q
	CNM_WFP2PMF_START,						///< �Q�[���J�n		�e�[���q
	CNM_WFP2PMF_VCHAT,						///< VCHAT�f�[�^	�e�[���q

	//-------------------------------------
	//	
	//=====================================
	
	//------------------------------------------------�����܂�
	CNM_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};

extern const NetRecvFuncTable* WFP2PMF_CommCommandTclGet( void );
extern int WFP2PMF_CommCommandTblNumGet( void );

#endif		// __COMM_COMMAND_WFP2PMF_H__

