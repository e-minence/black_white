//*****************************************************************************
/**
 * @file	net_def.h
 * @brief	�l�b�g���[�N�p�O���[�v����`
 * @author	k.ohno
 * @date	2006.11.20
 */
//*****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NET_DEF_H__
#define	__NET_DEF_H__

#include "common_def.h"
#include "net.h"

//------------------------------------------------------------------
/**
 * @brief	��`
 */
//------------------------------------------------------------------

//typedef struct _NET_TOOLSYS_t NET_TOOLSYS;
typedef struct _NET_PARENTSYS_t NET_PARENTSYS;

/// @brief WIRELESS�Ǘ��\����
typedef struct _NET_WL_WORK GFL_NETWL;


/// @brief �R�[���o�b�N�֐��̏��� local�����p
typedef void (*PTRCommRecvLocalFunc)(u16 aid, u16 *data, u16 size);
/// @brief ���M�����R�[���o�b�N
typedef void (*PTRSendDataCallback)(BOOL result);

/// �r�[�R�����E�����Ƃ��ɌĂ΂��R�[���o�b�N�֐�
typedef void (*_PARENTFIND_CALLBACK)(void);


#define _SCAN_ALL_CHANNEL  (0)  ///< �X�L��������`�����l���������_���ōs���ꍇ�O�ɐݒ肷��


// ������R�}���h�̎�M�ő�T�C�Y
#define  COMM_COMMAND_RECV_SIZE_MAX  (256)
// ������R�}���h�̑��M�ő�T�C�Y  (ringbuff�Ɠ����傫��)
#define  COMM_COMMAND_SEND_SIZE_MAX  (264)



// �ʐM�Ń��j���[���o�����ꍇ�̖߂�l
#define  COMM_RETVAL_NULL    (0)     ///< �I��
#define  COMM_RETVAL_CANCEL  (1)   ///< user�L�����Z��
#define  COMM_RETVAL_OK      (2)              ///< �I������
#define  COMM_RETVAL_ERROR   (3)              ///< �ʐM�G���[
#define  COMM_RETVAL_DIFFER_REGULATION   (4)              ///< ���M�����[�V�������قȂ�


#define CURRENT_PLAYER_WORD_IDX    (0)    // �����̖��O������o�b�t�@�̏ꏊ
#define TARGET_PLAYER_WORD_IDX     (1)     // �ΏۂƂȂ閼�O������o�b�t�@�̏ꏊ
#define ASSAILANT_PLAYER_WORD_IDX  (2)     // ��Q�҂ƂȂ閼�O������o�b�t�@�̏ꏊ


#define CC_NOT_FOUND_PARENT_INFO (0) // �e�@��񂪂Ȃ��ꍇ  
#define CC_BUSY_STATE  (1)  //�ʐM�̓�����Ԃ��J�ڒ��ŁA�ڑ��ɂ����Ȃ��ꍇ
#define CC_CONNECT_STARTING (2) // �ڑ��֐����Ăяo������




enum _negotiationType {
    _NEGOTIATION_CHECK,   // �ڑ��m�F��
    _NEGOTIATION_OK,      // �ڑ��m�F����
    _NEGOTIATION_NG,     // �ڑ����s
};


// pNetHandle���󂯎��
extern GFL_NETHANDLE* _NETHANDLE_GetSYS(void);
// pNetHandle����Toolsys���󂯎��
extern NET_TOOLSYS* _NETHANDLE_GetTOOLSYS(GFL_NETHANDLE* pNetHandle);

extern GFL_NETWL* _GFL_NET_GetNETWL(void);

extern GFLNetInitializeStruct* _GFL_NET_GetNETInitStruct(void);

extern void _GFL_NET_SetNETWL(GFL_NETWL* pWL);

extern GFL_NETHANDLE* GFL_NET_GetNetHandle(int netID);

extern void GFI_NET_DeleteNetHandle(GFL_NETHANDLE* pHandle);

extern void GFI_NET_FatalErrorFunc(int errorNo);
extern void GFI_NET_ErrorFunc(int errorNo);

extern int GFI_NET_GetConnectNumMax(void);

extern int GFI_NET_GetSendSizeMax(void);

extern void GFI_NET_AutoParentConnectFunc(void);



#if GFL_NET_WIFI //wifi

extern void GFI_NET_NetWifiSaveUserDataFunc(void);
extern void GFI_NET_NetWifiMargeFrinedDataFunc(int deletedIndex,int srcIndex);
extern DWCUserData* GFI_NET_GetMyDWCUserData(void);
extern DWCFriendData* GFI_NET_GetMyDWCFriendData(void);
extern int GFI_NET_GetFriendNumMax(void);

#endif //GFL_NET_WIFI
 
#endif	//__NET_DEF_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
