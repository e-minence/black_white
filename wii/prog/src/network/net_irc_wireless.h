//==============================================================================
/**
 * @file	net_irc_wireless.h
 * @brief	�ԊO���ʐM�Ń}�b�`���O��A���C�����X�ʐM�ֈڍs�̃w�b�_
 * @author	matsuda
 * @date	2008.11.11(��)
 */
//==============================================================================
#ifndef __NET_IRC_WIRELESS_H__
#define __NET_IRC_WIRELESS_H__

//==============================================================================
//	�O���֐��錾
//==============================================================================
extern const GFLNetInitializeStruct * GFL_NET_IW_SystemCreate(const GFLNetInitializeStruct* pNetInit, void* pWork);
extern void GFL_NET_IW_SystemExit(void);
extern BOOL GFL_NET_IW_SystemCheck(void);
extern NetStepEndCallback GFL_NET_IW_ConnectCallbackSetting(NetStepEndCallback callback);
extern void GFL_NET_IW_ParentMacAddressSet(const u8 *mac_address);
extern void GFL_NET_IW_Main(void);

#endif	//__NET_IRC_WIRELESS_H__
