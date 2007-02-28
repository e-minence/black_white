//*****************************************************************************
/**
 * @file	net_def.h
 * @brief	�l�b�g���[�N�p�O���[�v����`
 * @author	k.ohno
 * @date	2006.11.20
 */
//*****************************************************************************

#ifndef __NET_DEF_H__
#define	__NET_DEF_H__

#include "net.h"

//------------------------------------------------------------------
/**
 * @brief	��`
 */
//------------------------------------------------------------------

typedef struct _NET_TOOLSYS_t NET_TOOLSYS;
typedef struct _NET_PARENTSYS_t NET_PARENTSYS;

/// @brief WIRELESS�Ǘ��\����
typedef struct _NET_WL_WORK GFL_NETWL;


/// @brief �R�[���o�b�N�֐��̏��� local�����p
typedef void (*PTRCommRecvLocalFunc)(u16 aid, u16 *data, u16 size);
/// @brief ���M�����R�[���o�b�N
typedef void (*PTRSendDataCallback)(BOOL result);

/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*PTRStateFunc)(GFL_NETHANDLE* pNetHandle);

/// �r�[�R�����E�����Ƃ��ɌĂ΂��R�[���o�b�N�֐�
typedef void (*_PARENTFIND_CALLBACK)(GFL_NETHANDLE* pHandle);


#define _SCAN_ALL_CHANNEL  (0)  ///< �X�L��������`�����l���������_���ōs���ꍇ�O�ɐݒ肷��


// �����Ŏg�p����DMA�ԍ�
#define _NETWORK_DMA_NO                 (2)
//WM�̃p���[���[�h
#define _NETWORK_POWERMODE       (1)
//SSL�����̃X���b�h�D�揇��
#define _NETWORK_SSL_PRIORITY     (20)



// �e�@��I���ł��鐔�B
#define  SCAN_PARENT_COUNT_MAX ( 16 )

// �q�@�ő吔
#define  GFL_NET_CHILD_MAX  ( 7 )

// ���肦�Ȃ�ID
#define COMM_INVALID_ID  (0xff)


/// �@�ő吔
#define  GFL_NET_MACHINE_MAX  (GFL_NET_CHILD_MAX+1)

///   �ʐM�n���h���ő吔  �q�@�S���{�e�@ ��
#define  GFL_NET_HANDLE_MAX  (GFL_NET_MACHINE_MAX+1)

/// �ʐM�̃f�[�^�T�C�Y  GFL_NET_MACHINE_MAX�����{�ɍ\��
#define GFL_NET_DATA_HEADER            (4)
#define GFL_NET_CHILD_DATA_SIZE           (40)
#define GFL_NET_PARENT_DATA_SIZE          (GFL_NET_CHILD_DATA_SIZE * GFL_NET_MACHINE_MAX + GFL_NET_DATA_HEADER)



// ������R�}���h�̎�M�ő�T�C�Y
#define  COMM_COMMAND_RECV_SIZE_MAX  (256)
// ������R�}���h�̑��M�ő�T�C�Y  (ringbuff�Ɠ����傫��)
#define  COMM_COMMAND_SEND_SIZE_MAX  (264)



// �e��ID
#define COMM_PARENT_ID    (0)

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


// MYSTATUS�𑗂邽�߂̗̈�
#define COMM_SEND_MYSTATUS_SIZE   (32)
#define COMM_SEND_REGULATION_SIZE (32)



/// �l�b�g���[�N�n���h��
struct _GFL_NETHANDLE{
  NET_TOOLSYS* pTool;      ///< netTool
  NET_PARENTSYS* pParent;  ///< �e�̏���ێ�����|�C���^
  PTRStateFunc state;      ///< �n���h���̃v���O�������
  MATHRandContext32 sRand; ///< �e�q�@�l�S�V�G�[�V�����p�����L�[
  HEAPID baseHeapID;       ///< �ʐM��create���邽�߂�ID
  HEAPID netHeapID;        ///< �ʐMlib���g�p����ID
  HEAPID wifiHeapID;       ///< wifiLib���g�p����ID
  u8 aMacAddress[6];       ///< �ڑ���MAC�A�h���X�i�[�o�b�t�@
  u8 negotiationID[(GFL_NET_MACHINE_MAX/8)+(0!=(GFL_NET_MACHINE_MAX%8))]; ///< �ڑ����Ă���n���h���̏��
  u8 wifiTargetNo;    ///< �ڑ��ċN�ԍ�(wifi�p)
  u8 disconnectType;  ///< �ؒf���̃^�C�v(wifi)
  u8 bWifiDisconnect;  ///< �ؒf�t���O(wifi)
  u8 bDisconnectError; ///< �ؒf�G���[(wifi)
  u8 errorCode;       ///< �ŐV�G���[�R�[�h�ۑ�
  u8 machineNo;       ///< �}�V���ԍ�
  u8 serviceNo;       ///< �ʐM�T�[�r�X�ԍ�
  u16 timer;          ///< �i�s�^�C�}�[
  u8 bFirstParent;    ///< �J��Ԃ��e�q�؂�ւ����s���ꍇ�̍ŏ��̐e���
  u8 limitNum;        ///< ��t�����������ꍇ��LIMIT��
  u8 negotiation;     ///< �ʐM��t�̏��
  u8 creatureNo;     ///< �q�@�Ƃ��Ă�
  u8 stateError;
  u8 bErrorAuto;
  u8 dsmpChange;        ///< �ʐM���[�h�ύX
  u8 dsmpChangeType;    ///< �ʐM���[�h�ύX�^�C�v
  u8 bDisconnectState;  ///< �ؒf��Ԃɓ����Ă���ꍇTRUE
};

enum _negotiationType {
    _NEGOTIATION_CHECK,   // �ڑ��m�F��
    _NEGOTIATION_OK,      // �ڑ��m�F����
    _NEGOTIATION_NG,     // �ڑ����s
};



// �l�I�ȃf�o�b�O�ׂ̈̃v�����g
#ifndef OHNO_PRINT
#if defined(DEBUG_ONLY_FOR_ohno)
//#ifdef  SDK_DEBUG

#if 0
#define OHNO_SP_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#define OHNO_PRINT(...)           ((void) 0)

#else
#define OHNO_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#define OHNO_SP_PRINT(...)           ((void) 0)
#endif

#else   //DEBUG_ONLY_FOR_ohno

#define OHNO_PRINT(...)           ((void) 0)
#define OHNO_SP_PRINT(...)           ((void) 0)

#endif  // DEBUG_ONLY_FOR_ohno
#endif  //OHNO_PRINT


// pNetHandle���󂯎��
extern GFL_NETHANDLE* _NETHANDLE_GetSYS(void);
// pNetHandle����Toolsys���󂯎��
extern NET_TOOLSYS* _NETHANDLE_GetTOOLSYS(GFL_NETHANDLE* pNetHandle);

extern GFL_NETWL* _GFL_NET_GetNETWL(void);

extern GFL_NETSYS* _GFL_NET_GetNETSYS(void);

extern GFLNetInitializeStruct* _GFL_NET_GetNETInitStruct(void);

extern PTRStateFunc GFL_NET_GetStateFunc(GFL_NETHANDLE* pHandle);

extern void _GFL_NET_SetNETWL(GFL_NETWL* pWL);

extern GFL_NETHANDLE* GFL_NET_GetNetHandle(int netID);

extern void GFI_NET_DeleteNetHandle(GFL_NETHANDLE* pHandle);


#if GFL_NET_WIFI //wifi

extern void GFI_NET_NetWifiSaveUserDataFunc(void);
extern void GFI_NET_NetWifiMargeFrinedDataFunc(int deletedIndex,int srcIndex);
extern DWCUserData* GFI_NET_GetMyDWCUserData(void);
extern DWCFriendData* GFI_NET_GetMyDWCFriendData(void);
extern int GFI_NET_GetFriendNumMax(void);

#endif //GFL_NET_WIFI
 
#endif	//__NET_DEF_H__
