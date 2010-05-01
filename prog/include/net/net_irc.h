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

//#include <irc.h>
#include <nitro/irc.h>


//==============================================================================
//	�萔��`
//==============================================================================
///�ؒf�Ƃ��Ĕ��肷��܂ł̎��Ԃ̕W���l(���̎��ԓ��Ȃ�ؒf���Ă��Đڑ������݂�)
#define IRC_TIMEOUT_STANDARD		(60 * 5)

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
typedef BOOL (*IrcRecvFunc)(u16 aid, u16 *data, u16 size);
/// ��r�R�[���o�b�N
typedef BOOL (IrcGSIDCallback)(u8 mygsid,u8 friendgsid);

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void GFL_NET_IRC_Init(u32 irc_timeout,u8 gsid);
extern void GFL_NET_IRC_ChangeTimeoutTime(u32 irc_timeout);
extern void GFL_NET_IRC_Exit(void);
extern void GFL_NET_IRC_SetGSIDCallback(IrcGSIDCallback* callback);
extern void GFL_NET_IRC_RecieveFuncSet(IrcRecvFunc recieve_func);
extern void GFL_NET_IRC_InitializeFlagSet(void);
extern BOOL GFL_NET_IRC_InitializeFlagGet(void);
extern BOOL GFL_NET_IRC_SendTurnGet(void);
extern BOOL GFL_NET_IRC_SendLockFlagGet(void);
extern BOOL GFL_NET_IRC_SendCheck(void);
extern int GFL_NET_IRC_System_GetCurrentAid(void);
extern void GFL_NET_IRC_Send(u8 *buf, u8 size, u8 command);
extern void GFL_NET_IRC_CommandContinue(void);
extern void GFL_NET_IRC_ParentMacAddressSet(const u8 *mac_address);
extern u8 * GFL_NET_IRC_ParentMacAddressGet(void);
extern BOOL GFL_NET_IRC_ErrorCheck(void);
extern void GFL_NET_IRC_FirstConnect(void);
extern BOOL GFL_NET_IRC_IsSender(void);
extern BOOL GFL_NET_IRC_IsConnect(void);
extern BOOL GFL_NET_IRC_IsConnectSystem(void);
extern void GFL_NET_IRC_Move(void);


#endif	//__NET_IRC_H__

