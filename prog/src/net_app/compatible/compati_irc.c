//==============================================================================
/**
 * @file	compati_irc.c
 * @brief	�ԊO���Ōq�����Ă��邩
 * @author	matsuda
 * @date	2009.02.13(��)
 */
//==============================================================================
#include <gflib.h>
#include "system\main.h"
#include "net\network_define.h"
#include "system/gfl_use.h"

#include "compati_types.h"
#include "compati_tool.h"
#include "compati_comm.h"
#include "compati_irc.h"


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value);

static COMPATI_IRC_SYS *glb_ircsys = NULL;


//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   ircsys		
 */
//--------------------------------------------------------------
void CompatiIrc_Init(COMPATI_IRC_SYS *ircsys)
{
	glb_ircsys = ircsys;

	IRC_Init();
	IRC_SetRecvCallback(IRC_ReceiveCallback);
}

//--------------------------------------------------------------
/**
 * @brief   �����`�F�b�N�Q�[�����̐ԊO���ڑ��m�F
 *
 * @param   ircsys		
 *
 * @retval  TRUE:�ڑ����Ă���B�@FALSE:�ڑ����Ă��Ȃ�
 */
//--------------------------------------------------------------
BOOL CompatiIrc_Main(COMPATI_IRC_SYS *ircsys)
{
	BOOL irc_ret;
	
	IRC_Move();
	irc_ret = IRC_IsConnect();
	if(irc_ret == TRUE){
		//OS_TPrintf("�ԊO���ڑ�����Ă���\n");
		if(ircsys->shutdown_req == TRUE){
			//OS_TPrintf("�ԊO��shutdown\n");
			IRC_Shutdown();
			return irc_ret;
		}
	}
	else{
		//OS_TPrintf("�ԊO���q�����Ă��Ȃ�\n");
		if(ircsys->shutdown_req == TRUE){
			//OS_TPrintf("shutdown���������Ă���\n");
			return irc_ret;
		}
		else if(ircsys->seq == 100){
			//OS_TPrintf("�ԊO��seq���O\n");
			ircsys->seq = 0;
		}
	}
	
	switch(ircsys->seq){
	case 0:
		ircsys->connect = FALSE;
		IRC_Init();
		IRC_SetRecvCallback(IRC_ReceiveCallback);
		ircsys->wait = GFUser_GetPublicRand(30);	//�e�q�����Ƀ��N�G�X�g�𑗂�Ȃ��悤�Ƀ����_��
		//OS_TPrintf("�ԊO��������\n");
		ircsys->seq++;
		break;
	case 1:
		if(ircsys->wait > 0){
			ircsys->wait--;
			break;
		}
		IRC_Connect();
		//OS_TPrintf("�ԊO���ڑ����N�G�X�g\n");
		ircsys->seq++;
		break;
	case 2:
		if(IRC_IsConnect() == TRUE){
			if(IRC_IsSender() == FALSE){
				IRC_Send(NULL, 0, 8, 10);
			}
			//OS_TPrintf("�ԊO���q������\n");
			ircsys->seq = 100;
		}
		else{
			ircsys->wait++;
			if(ircsys->wait > 60*2){	//������x�҂��Ă��ڑ��o���Ȃ��ꍇ�͂�����x����������
				ircsys->wait = 0;
				ircsys->seq = 0;
				//OS_TPrintf("�ԊO���Ȃ��Ȃ��ڑ����Ȃ��̂ōŏ�����\n");
			}
			break;
		}
		break;
	default:	//�ڑ���
		break;
	}
	
	return irc_ret;
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO���ؒf���N�G�X�g
 *
 * @param   ircsys		
 */
//--------------------------------------------------------------
void CompatiIRC_Shoutdown(COMPATI_IRC_SYS *ircsys)
{
	ircsys->shutdown_req = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   IRC��M���ɌĂ΂��R�[���o�b�N
 *
 * @param   data		
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value)
{
	//OS_TPrintf("�ԊO����M command = %d\n", command);

	//-- �ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h --//
	if(command >= 0xf0){
		if(command == 0xf4){
			OS_TPrintf("IRC�ؒf�R�}���h����M\n");
		}
		return;	//�ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h�ׁ̈A�����ł͖���
	}
	
	IRC_Send(NULL, 0, 8, 0);	//�ʐM�p���ׁ̈A�Ԏ���Ԃ�
	glb_ircsys->connect = TRUE;
	glb_ircsys->seq = 100;
}
