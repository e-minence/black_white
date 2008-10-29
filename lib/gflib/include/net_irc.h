//==============================================================================
/**
 * @file	net_irc.h
 * @brief	IRC����w�b�_
 * @author	matsuda
 * @date	2008.10.15(��)
 */
//==============================================================================
#ifndef __NET_IRC_H__
#define __NET_IRC_H__

#include "irc.h"


//==============================================================================
//	�萔��`
//==============================================================================
///IRC���C�u���������Ŏg�p���Ă���w�b�_�[�̃T�C�Y
#define IRC_HEADER_SIZE		(4)

///�ԊO���V�X�e���R�}���h
enum{
	GF_NET_COMMAND_CONTINUE = 0xee,	///<�ԊO���ʐM�p���ׂ̈̃R�}���h�̂ݑ��M
};

//==============================================================================
//	�^��`
//==============================================================================
/// ��M�R�[���o�b�N�̏����w��
typedef void (*IrcRecvFunc)(u16 aid, u16 *data, u16 size);


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void GFL_NET_IRC_Init(void);
extern void GFL_NET_IRC_RecieveFuncSet(IrcRecvFunc recieve_func);
extern void GFL_NET_IRC_InitializeFlagSet(void);
extern BOOL GFL_NET_IRC_InitializeFlagGet(void);
extern void GFL_NET_IRC_SendTurnSet(void);
extern BOOL GFL_NET_IRC_SendTurnGet(void);
extern BOOL GFL_NET_IRC_SendLockFlagGet(void);
extern BOOL GFL_NET_IRC_SendCheck(void);
extern int GFL_NET_IRC_GetCurrentAid(void);
extern int GFL_NET_IRC_System_GetCurrentAid(void);
extern void GFL_NET_IRC_Send(u8 *buf, u8 size, u8 command, u8 value);
extern void GFL_NET_IRC_CommandContinue(void);


#endif	//__NET_IRC_H__

