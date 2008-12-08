//==============================================================================
/**
 * @file	net_irc.c
 * @brief	IRC����
 * @author	matsuda
 * @date	2008.10.10(��)
 */
//==============================================================================
#include "gflib.h"

#include "ui/ui_def.h"
#include "net_def.h"
#include "net_local.h"
#include "device/net_whpipe.h"
#include "device/dwc_rapcommon.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "wm_icon.h"
#include "device/dwc_rap.h"


#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"
#include "net_irc.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�ؒf�Ƃ��Ĕ��肷��܂ł̎���(���̎��ԓ��Ȃ�ؒf���Ă��Đڑ������݂�)
#define NET_IRC_SHOUT_DOWN_TIME		(60 * 5)	//0���w�肷��Ύ��Ԑ؂ꖳ��

///�Đڑ�����ۂɁA�Đڑ����N�G�X�g�J�n����܂ł̃E�F�C�g
///(������̏I���A�����������Ȃǂ�����̂ŏ����Ԃ��󂯂�)
#define NET_IRC_RETRY_START_WAIT	(5)

///�Đڑ�����ۂɍs��IRC_Connect�����t���[���󂯂čēx���N�G�X�g�������邩
///���Ń��N�G�X�g���͂����Ƃ͂����Ă���ۂ����ǈꉞ
#define NET_IRC_CONNECT_REQ_WAIT	(5)

/*----------------------
!!value�l�̈���!!

����4�r�b�g���u���Ԃ̃f�[�^�܂ő��M���܂����v�Ƃ������M����̒l�A
���4�r�b�g���u���Ԃ̃f�[�^�܂Ŏ󂯎��܂����v�Ƃ�����M����̒l�Ƃ���B
���M�������͎�M������value�l�ł����Ɓu���Ԃ̃f�[�^���󂯎��܂����v�Ƃ����Ԏ���
�����Ȃ����莟�̃f�[�^�͑��M�o���Ȃ��悤�ɂ���
��IRC_Send�͎�ň�u�B�����肷��Ɛؒf�͂��Ă��Ȃ��̂ɑ��M�������s���Ă��܂��悤�Ȃ̂ŁB
------------------------*/
///���M�f�[�^�Ǘ�value�l�̃����W
#define SEND_CHECK_VALUE_RANGE		(0xf)	//4bit���̐��l�ɂ��鎖!!


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	IrcRecvFunc recieve_func;
	u8 initialize;		///<TRUE:����������
	u8 send_use;		///<���t���[���Ŋ���Send���g�p�������ǂ���(TRUE:�g�p����)
	u8 send_turn;		///<TRUE:���M�\�B�@FALSE:���M�͒ʐM����̃^�[��
	u8 send_lock;		///<TRUE:���M���b�N��(���葤������f�[�^���M��)
	
	u8 parent_MacAddress[6];	///<�e��MacAddress
	u8 isSender;		///<TRUE:�ŏ��ɐڑ��v������(�Đڑ����͒l�͕ς��܂���)
	u8 connect;			///<TRUE:�q�����Ă���(�Đڑ�����TRUE)
	
	s32 retry_time;		///<�Đڑ����̃^�C���J�E���^
	
	void *send_buf;		///<���M�f�[�^�ւ̃|�C���^
	u8 send_size;		///<���M�T�C�Y
	u8 send_command;	///<���M�R�}���h
	u8 send_value;		///<���M�o�����[
	u8 send_action;		///<TRUE:���M���s��(send_buf==NULL�ł̓R�}���h�݂̂̑��M�̉\��������̂�)
	
	u8 my_value;		///<���������M���Ă���value�l�B�������M����x�ɃC���N�������g
	u8 last_value;		///<�Ō�Ɏ�M����value�l
	u8 retry_send_reserve;	///<�Đڑ��������ɑ��M���̃f�[�^���������ꍇ�A���M�\�񂷂�
	u8 pading;
}NET_IRC_SYS;


//==============================================================================
//	���[�J���ϐ�
//==============================================================================
static NET_IRC_SYS NetIrcSys = {0,0};


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
void GFL_NET_IRC_Init(void);
BOOL GFL_NET_IRC_SendTurnGet(void);
BOOL GFL_NET_IRC_SendLockFlagGet(void);
BOOL GFL_NET_IRC_SendCheck(void);
void GFL_NET_IRC_UpdateSendData(void);
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value);
static int IRC_TargetIDGet(void);


//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   none		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Init(void)
{
	GFL_STD_MemClear(&NetIrcSys, sizeof(NET_IRC_SYS));
	IRC_Init();
	IRC_SetRecvCallback(IRC_ReceiveCallback);
//	GFL_NET_SystemIrcModeInit();
}

void GFL_NET_IRC_RecieveFuncSet(IrcRecvFunc recieve_func)
{
	NetIrcSys.recieve_func = recieve_func;
}

//--------------------------------------------------------------
/**
 * @brief   �����������t���O���Z�b�g����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_InitializeFlagSet(void)
{
	GF_ASSERT(NetIrcSys.initialize == FALSE);
	NetIrcSys.initialize = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �����������t���O���擾����
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_InitializeFlagGet(void)
{
	return NetIrcSys.initialize;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�\�^�[���t���O���擾
 *
 * @retval  TRUE:���M�\�B�@FALSE:�s�\
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendTurnGet(void)
{
	return NetIrcSys.send_turn;
}

//--------------------------------------------------------------
/**
 * @brief   ���M���b�N�t���O���擾
 *
 * @retval  TRUE:���M���b�N���������Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendLockFlagGet(void)
{
	return NetIrcSys.send_lock;
}

//--------------------------------------------------------------
/**
 * @brief   ���M�\�����ׂ�
 *
 * @retval  TRUE:���M�\�B�@FALSE:�s�\
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendCheck(void)
{
	if(NetIrcSys.send_turn == TRUE && NetIrcSys.send_lock == FALSE 
			&& NetIrcSys.retry_time == 0 && NetIrcSys.send_action == 0){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�t�@���M
 *
 * @param   buf			
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Send(u8 *buf, u8 size, u8 command)
{
	u8 value;
	
	GF_ASSERT(NetIrcSys.send_turn == TRUE);
	
#if GFL_IRC_DEBUG
	if(command != GF_NET_COMMAND_CONTINUE){
		IRC_PRINT("IRC_SEND size=%d, command=%d, value=%d\n", size, command, value);
		{
			int i;
			for(i = 0; i < size; i++){
//				IRC_PRINT("%d, ", buf[i]);
			}
//			IRC_PRINT("\n");
		}
	}
#endif
	
	value = NetIrcSys.my_value | (NetIrcSys.last_value << 4);
	IRC_Send(buf, size, command, value);
	NetIrcSys.send_buf = buf;
	NetIrcSys.send_size = size;
	NetIrcSys.send_command = command;
	NetIrcSys.send_value = NetIrcSys.my_value;
	NetIrcSys.send_action = TRUE;
	
	NetIrcSys.send_turn = 0;
}

//--------------------------------------------------------------
/**
 * @brief   send_turn������Ă��Ă���̂ɉ����f�[�^�𑗂��Ă��Ȃ��ꍇ�A���̊֐����g�p����
 * 			�ʐM�p���ׂ̈̃R�}���h�𑗐M����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_CommandContinue(void)
{
	NET_IRC_SYS *nis = &NetIrcSys;

	if(nis->send_turn == TRUE){
		if(nis->retry_send_reserve == TRUE){	//�Đڑ���̗\�񂪂���ꍇ�͂�����𑗐M
			IRC_Send(nis->send_buf, nis->send_size, nis->send_command, 
				nis->send_value | (NetIrcSys.last_value << 4));
			OS_TPrintf("�\�񑗐M value = %d\n", nis->send_value);
			nis->retry_send_reserve = 0;
		}
		else{
			//�ʐM�p���ׂ̈�send_turn������
			IRC_Send(NULL, 0, GF_NET_COMMAND_CONTINUE, 
				nis->send_value | (NetIrcSys.last_value << 4));
		}
		nis->send_turn = 0;
	}
}

//--------------------------------------------------------------
/**
 * @brief   IRC�l�b�gID�擾
 *
 * @retval  ID
 */
//--------------------------------------------------------------
int GFL_NET_IRC_System_GetCurrentAid(void)
{
	if(NetIrcSys.isSender == TRUE){
		return GFL_NET_NO_PARENTMACHINE;
	}
	return 1;
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
	int send_id;
	int send_value, receive_value;
	
	size -= IRC_HEADER_SIZE;
	
	send_value = value & 0xf;
	receive_value = value >> 4;
	
	if(command != GF_NET_COMMAND_CONTINUE){
		IRC_PRINT("IRC��M�R�[���o�b�N�Ăяo��, size=%d, command=%d, send_value=%d, receive_value=%d\n", size,command,send_value,receive_value);
	}
	
	NetIrcSys.send_turn = TRUE;

	//-- �ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h --//
	if(command >= 0xf0){
		if(command == 0xf4){
			OS_TPrintf("IRC�ؒf�R�}���h����M\n");
		}
		return;	//�ԊO�����C�u���������Ŏg�p���Ă���V�X�e���R�}���h�ׁ̈A�����ł͖���
	}
	
	if(NetIrcSys.send_action == TRUE && NetIrcSys.retry_send_reserve == 0){
		//���炩�̕Ԏ�������������Ԃ��Ă��Ă���Ȃ��قǑ��M�����f�[�^�͌������ɓn�����Ƃ�����
		//�A�����C�u���������̃V�X�e���R�}���h�͐ؒf�R�}���h�������肷��̂ŁA
		//�V�X�e���R�}���h�̕Ԏ��͖�������
		IRC_PRINT("action�Ԏ���M my_value = %d, receive_value=%d\n", NetIrcSys.my_value, receive_value);
		if(receive_value == NetIrcSys.my_value){
			if(NetIrcSys.my_value < SEND_CHECK_VALUE_RANGE){
				NetIrcSys.my_value++;
			}
			else{
				NetIrcSys.my_value = 0;
			}
			NetIrcSys.send_action = 0;
		}
		else{
			NetIrcSys.retry_send_reserve = TRUE;	//�ēx���M�\��
		}
	}
	
	//�ԊO����p�̃V�X�e���R�}���h����
	switch(command){
	case GF_NET_COMMAND_CONTINUE:
		return;		//�ʐM�p���ׂ̈̉����R�}���h�ׁ̈A�����ŏI��
	default:
		break;
	}

	if(NetIrcSys.last_value == send_value){
		//�Ō�Ɏ󂯎����value�l�Ɠ���value�l�̏ꍇ�A�����f�[�^��2�x�����Ă��Ă���̂Ŗ�������
		//���Đڑ��������ɂ��̃P�[�X����������B
		OS_TPrintf("�ԊO���F����value����M\n");
		return;
	}
	NetIrcSys.last_value = value;

	send_id = IRC_TargetIDGet();
	if(NetIrcSys.recieve_func != NULL){
		NetIrcSys.recieve_func(send_id, (u16*)data, size);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �ʐM�����ID���擾
 *
 * @retval  �����ID
 */
//--------------------------------------------------------------
static int IRC_TargetIDGet(void)
{
	if(GFL_NET_IRC_IsSender() == TRUE){
		return 1;	//�������e�Ȃ�q���炵���f�[�^�����Ȃ��̂�2�Œ�
	}
	
	//�������q�Ȃ�A�e���炵�����Ȃ��̂�GFL_NET_NO_PARENTMACHINE�Œ�
	return GFL_NET_NO_PARENTMACHINE;
}

//--------------------------------------------------------------
/**
 * @brief   �e��MacAddress�����[�N�ɕۑ�
 *
 * @param   mac_address		�e��MacAddress
 */
//--------------------------------------------------------------
void GFL_NET_IRC_ParentMacAddressSet(const u8 *mac_address)
{
	GFL_STD_MemCopy(mac_address, NetIrcSys.parent_MacAddress, 6);
}

//--------------------------------------------------------------
/**
 * @brief   �e��MacAddress�����o��
 *
 * @retval  �e��MacAddress��ێ����Ă��郏�[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
u8 * GFL_NET_IRC_ParentMacAddressGet(void)
{
	return NetIrcSys.parent_MacAddress;
}

//--------------------------------------------------------------
/**
 * @brief   ���߂Ă̐ڑ���̃��[�N�ݒ�
 */
//--------------------------------------------------------------
void GFL_NET_IRC_FirstConnect(void)
{
	NetIrcSys.connect = TRUE;
	NetIrcSys.isSender = IRC_IsSender();
	NetIrcSys.my_value = 0;
	NetIrcSys.last_value = SEND_CHECK_VALUE_RANGE-1;	//�ŏ��̃f�[�^�������Ǝ󂯎���悤��my_value�ƈႦ�Ή��ł��悢
	if(NetIrcSys.isSender == TRUE){
		OS_TPrintf("�ԊO���F�e�@�ɂȂ���\n");
	}
	else{
		OS_TPrintf("�ԊO���F�q�@�ɂȂ���\n");
		//���V�[�o�[����ŏ��̒ʐM���n�߂�
		NetIrcSys.send_turn = TRUE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   �ŏ��ɐڑ��v�������[�������ׂ�
 *
 * @retval  TRUE:�ŏ��ɐڑ��v�������[���ł���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsSender(void)
{
	return NetIrcSys.isSender;
}

//--------------------------------------------------------------
/**
 * @brief   �ڑ������ǂ���(�Đڑ����͌q�����Ă���ƕԂ��܂�)
 *
 * @retval  TRUE:�ڑ����Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnect(void)
{
	if(IRC_IsConnect() == TRUE){
		return TRUE;
	}
	if(NetIrcSys.retry_time < NET_IRC_SHOUT_DOWN_TIME){
		return TRUE;	//�Đڑ���
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �Đڑ����ȂǊ֌W�Ȃ��A�����ɍ��A�q�����Ă��邩
 *
 * @retval  TRUE:�ڑ����Ă���
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnectSystem(void)
{
	return IRC_IsConnect();
}

//--------------------------------------------------------------
/**
 * @brief   �ԊO�����s����
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Move(void)
{
	NET_IRC_SYS *nis = &NetIrcSys;

	if(nis->connect == FALSE && nis->retry_time >= NET_IRC_SHOUT_DOWN_TIME){
		//�Đڑ������s���Ċ��S�ɏI����ԁBIRC_Move�𓮂����Ƃ܂����肪���g���C�������Ă����ꍇ
		//������͊��S�I���Ɉڍs�����̂Ɍ��������q����A�ƂȂ��Ă��܂��̂ł�����return����
		return;
	}
	
	if(nis->retry_time == 0){
		IRC_Move();
	}
	
	//�q�����Ă���ꍇ�A�ؒf���Ă��Ȃ������ׁA�ؒf���Ă���΍Đڑ������։�
	if(nis->connect == TRUE){
		if(nis->retry_time > 0){
			//�Đڑ���
			if(IRC_IsConnect() == TRUE){
				//�Đڑ�����
				nis->retry_time = 0;
				OS_TPrintf("�ԊO���F�Đڑ�OK!\n");
				if(IRC_IsSender() == TRUE){
					nis->send_turn = 0;
				}
				else{
					nis->send_turn = TRUE;
				}
				if(nis->send_action == TRUE){
					nis->retry_send_reserve = TRUE;	//�ؒf�O�ɑ������f�[�^���ēx���M�\��
				}
			}
			else{
				//�Đڑ��҂�
				if(nis->retry_time < NET_IRC_RETRY_START_WAIT){
					//���ۂɃ��g���C��������܂ŁA������̏����������Ȃǂ�����̂�
					//�����Ԃ��󂯂�
					nis->retry_time++;
					return;
				}
				else if(nis->retry_time == NET_IRC_RETRY_START_WAIT){
					IRC_Init();
					IRC_SetRecvCallback(IRC_ReceiveCallback);
					nis->retry_time++;
					return;
				}

				if(nis->retry_time < NET_IRC_SHOUT_DOWN_TIME || NET_IRC_SHOUT_DOWN_TIME == 0){
					if(nis->isSender == TRUE && nis->retry_time % NET_IRC_CONNECT_REQ_WAIT == 0){
						IRC_Connect();
					}
				#if (NET_IRC_SHOUT_DOWN_TIME != 0)
					nis->retry_time++;
				#endif
				}
				else{	//�Đڑ��̎��Ԑ؂�
					OS_TPrintf("�ԊO���F�Đڑ��̎��Ԑ؂�\n");
					nis->connect = FALSE;
					IRC_Shutdown();
				}
				IRC_Move();
			}
		}
		else{
			//�q�����Ă���
			if(IRC_IsConnect() == FALSE){
				OS_TPrintf("�ԊO���F�ؒf�����B�Đڑ����܂�\n");
				IRC_Shutdown();
				nis->retry_time = 1;	//�Đڑ���
			}
		}
	}
}
