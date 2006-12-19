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

/// @brief WIRELESS�Ǘ��\����
typedef struct _NET_WL_WORK GFL_NETWL;


/// @brief �R�[���o�b�N�֐��̏��� local�����p
typedef void (*PTRCommRecvLocalFunc)(u16 aid, u16 *data, u16 size);
/// @brief ���M�����R�[���o�b�N
typedef void (*PTRSendDataCallback)(BOOL result);

/// �R�[���o�b�N�֐��̏��� �����X�e�[�g�J�ڗp
typedef void (*PTRStateFunc)(GFL_NETHANDLE* pNetHandle);



#define _SCAN_ALL_CHANNEL  (0)  ///< �X�L��������`�����l���������_���ōs���ꍇ�O�ɐݒ肷��


// �����Ŏg�p����DMA�ԍ�
#define COMM_DMA_NO                 (2)
//WM�̃p���[���[�h
#define COMM_POWERMODE       (1)
//SSL�����̃X���b�h�D�揇��
#define COMM_SSL_PRIORITY     (20)



// �e�@��I���ł��鐔�B
#define  SCAN_PARENT_COUNT_MAX ( 16 )

// �q�@�ő吔
#define  COMM_CHILD_MAX  ( 7 )

// ���肦�Ȃ�ID
#define COMM_INVALID_ID  (0xff)


// �@�ő吔
#define  GFL_NET_MACHINE_MAX  (COMM_CHILD_MAX+1)

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
    PTRStateFunc state; ///< �n���h���̃v���O�������
    MATHRandContext32 sRand; ///< �e�q�@�l�S�V�G�[�V�����p�����L�[
    u8 aMacAddress[6];  ///< �ڑ���MAC�A�h���X�i�[�o�b�t�@
    u8 machineNo;       ///< �}�V���ԍ�
    u8 serviceNo;       ///< �ʐM�T�[�r�X�ԍ�
    u16 timer;          ///< �i�s�^�C�}�[
    u8 bFirstParent;    ///< �J��Ԃ��e�q�؂�ւ����s���ꍇ�̍ŏ��̐e���
    u8 limitNum;        ///< ��t�����������ꍇ��LIMIT��
    u8 negotiation;     ///< �ʐM��t�̏��
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

#endif

