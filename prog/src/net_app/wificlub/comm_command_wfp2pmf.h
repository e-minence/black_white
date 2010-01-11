#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file	    comm_command_wfp2pmf.h
 *	@brief		wifi�N���u�S�� ���O���M
 *	@author		tomoya takahashi -> k.ohno
 *	@data		  2007.05.24 -> 2010.01.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "net/network_define.h"

enum CommCommandTradeList_e {

	//-------------------------------------
	//�@�Q�[���p
	//=====================================
	// �����p	
	CNM_WFP2PMF_RESULT = GFL_NET_CMD_WIFICLUB,	///< �ʐMKONG		�e�[���q
	CNM_WFP2PMF_START,						///< �Q�[���J�n		�e�[���q
	CNM_WFP2PMF_VCHAT,						///< VCHAT�f�[�^	�e�[���q
  CNM_WFP2PMF_STATUS,   ///< WIFI�X�e�[�^�X�f�[�^
  CNM_WFP2PMF_MYSTATUS,   ///< MY�X�e�[�^�X�f�[�^
	//-------------------------------------
	//	
	//=====================================
	
	//------------------------------------------------�����܂�
	CNM_COMMAND_MAX   // �I�[--------------����͈ړ������Ȃ��ł�������
};

extern const NetRecvFuncTable* WFP2PMF_CommCommandTclGet( void );
extern int WFP2PMF_CommCommandTblNumGet( void );


