//=============================================================================
/**
 * @file	net_queue.h
 * @brief	�i���M�j�L���[�̎d�g�݂��Ǘ�����֐�
 * @author	katsumi ohno
 * @date	06/01/29
 */
//=============================================================================

#ifndef __NET_QUEUE_H__
#define __NET_QUEUE_H__

#include "net_ring_buff.h"

/// @brief �L���[�\���̒�`
typedef struct _SEND_QUEUE SEND_QUEUE;

/// @brief �L���[�\����
struct _SEND_QUEUE{
    u8* pData;     ///< �f�[�^�A�h���X
    SEND_QUEUE* prev;      ///< ��O�̃L���[
    SEND_QUEUE* next;      ///< ���̃L���[
    u16 size;     ///< �T�C�Y
    u8 command;   ///< �R�}���h
    u8 sendNo;    ///< ����l
    u8 recvNo;    ///< �󂯎��l
    u8 bHeadSet;  ///< �w�b�_�[�𑗐M�����ꍇ�P �܂��̏ꍇ�O
    u8 bRing;     ///< �����O�o�b�t�@�g�p�̏ꍇ�P
} ;

///  @brief  �T�C�Y���܂񂾃L���[�̃w�b�_
#define _GFL_NET_QUEUE_HEADERBYTE_SIZEPLUS (5)
///  @brief  �T�C�Y���܂܂Ȃ��L���[�̃w�b�_
#define _GFL_NET_QUEUE_HEADERBYTE (3)



/// @brief ����f�[�^�̊Ǘ�
typedef struct{
    u8* pData;      ///<  �f�[�^
    int size;       ///<  �T�C�Y
} SEND_BUFF_DATA;


/// @brief ���M�L���[�Ǘ� �ŏ��ƍŌ�
typedef struct{
    SEND_QUEUE* pTop;     ///< ���M�L���[�̏���
    SEND_QUEUE* pLast;    ///< ���M�L���[�̍Ō�
} SEND_TERMINATOR;


/// @brief ���M�L���[�Ǘ�
typedef struct{
    SEND_TERMINATOR fast;     ///< �������鑗�M�L���[
    SEND_TERMINATOR stock;    ///< ��ő���΂����L���[
    SEND_QUEUE* pNow;   ///< �������Ă���Œ��̃L���[
    RingBuffWork* pSendRing;  ///< �����O�o�b�t�@���[�N�|�C���^
    void* heapTop;   ///< �L���[HEAP
    int max;         ///< �L���[�̐�
} SEND_QUEUE_MANAGER;


//==============================================================================
/**
 * @brief   �L���[�ɉ��������Ă��邩�ǂ������m�F����
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  TRUE   ��
 * @retval  FALSE  �����Ă���
 */
//==============================================================================
extern BOOL GFL_NET_QueueIsEmpty(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   �L���[MANAGER�̏�����
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @param   queueMax  �L���[��
 * @param   pSendRing ���f�[�^��ۑ�����ꍇ�̃����O�o�b�t�@���[�N
 * @param   heapid    �������[�m��ID
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerInitialize(SEND_QUEUE_MANAGER* pQueueMgr, int queueMax, RingBuffWork* pSendRing,int heapid);

//==============================================================================
/**
 * @brief   �L���[MANAGER�̃��Z�b�g
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerReset(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   �L���[MANAGER�̏I��
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_QueueManagerFinalize(SEND_QUEUE_MANAGER* pQueueMgr);

//==============================================================================
/**
 * @brief   �L���[�Ƀf�[�^������
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @param   command   ���M�R�}���h
 * @param   pDataArea  ���M�f�[�^
 * @param   size    �T�C�Y
 * @param   bFast  �D��x�������f�[�^?
 * @param   bSave  �ۑ����邩�ǂ���
 * @param   send      ����l
 * @param   recv      �󂯎��l
 * @retval  TRUE �~����
 * @retval  FALSE �L���[�ɓ���Ȃ�����
 */
//==============================================================================
extern BOOL GFL_NET_QueuePut(SEND_QUEUE_MANAGER* pQueueMgr,int command, u8* pDataArea, int size, BOOL bFast, BOOL bSave,int send,int recv);

//==============================================================================
/**
 * @brief   �w��o�C�g���o�b�t�@�ɓ����
 * @param   pQueueMgr  �L���[�}�l�[�W���[�̃|�C���^
 * @param   pSendBuff  ���M�o�b�t�@�\���̂̃|�C���^
 * @param   bNextPlus   �f�[�^�����Ƀw�b�_�[�������܂߂邩�ǂ���
 * @retval  TRUE  �����f�[�^���Ȃ��ꍇ
 * @retval  FALSE �f�[�^���A�����Ă���ꍇ
 */
//==============================================================================
extern BOOL GFL_NET_QueueGetData(SEND_QUEUE_MANAGER* pQueueMgr, SEND_BUFF_DATA *pSendBuff, BOOL bNextPlus);

//==============================================================================
/**
 * @brief   �L���[�����݂��邩�ǂ���
 * @param   pQueueMgr  �L���[�}�l�[�W���[�̃|�C���^
 * @param   command    ���ׂ�R�}���h
 * @retval  TRUE ����
 * @retval  FALSE �Ȃ�
 */
//==============================================================================
extern BOOL GFL_NET_QueueIsCommand(SEND_QUEUE_MANAGER* pQueueMgr, int command);

//==============================================================================
/**
 * @brief   �L���[�̐��𓾂�
 * @param   pQueueMgr �L���[�}�l�[�W���[�̃|�C���^
 * @return  �g�p���Ă���L���[�̐�
 */
//==============================================================================
extern int GFL_NET_QueueGetNowNum(SEND_QUEUE_MANAGER* pQueueMgr);

#ifdef PM_DEBUG
extern void GFL_NET_QueueDebugTest(void);
#endif

#endif// __COMM_QUEUE_H__

