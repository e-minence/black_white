//=============================================================================
/**
 * @file	net_ring_buff.h
 * @brief	�����O�o�b�t�@�̎d�g�݂��Ǘ�����֐�
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================
#ifdef __cplusplus
extern "C" {
#endif


#ifndef __NET_RING_BUFF_H__
#define __NET_RING_BUFF_H__

#include "common_def.h"

/// @brief �����O�o�b�t�@�\����
typedef struct{
    u8* pWork;                   ///< �ۑ��̈�̐擪�|�C���^
    int size;                    ///< �ۑ��̈�T�C�Y
    s16 startPos;                ///< �ǂݏo���Ă��Ȃ��f�[�^�̈ʒu
    volatile s16 backupEndPos;   ///< �Ō�̈ʒu�̃o�b�N�A�b�v
} RingBuffWork;

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�Ǘ��\���̏�����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �����O�o�b�t�@�������[
 * @param   size         �����O�o�b�t�@�������[�T�C�Y
 * @return  none
 */
//==============================================================================
extern void GFL_NET_RingInitialize(RingBuffWork* pRing, u8* pDataArea, const int size);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�ɏ�������
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �������ރf�[�^
 * @param   size         �������݃T�C�Y
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_RingPuts(RingBuffWork* pRing, u8* pDataArea, const int size);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@����f�[�^�𓾂�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �ǂݍ��݃o�b�t�@
 * @param   size         �ǂݍ��݃T�C�Y
 * @param   worksize     �ǂݍ��݃o�b�t�@�T�C�Y
 * @return  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
extern int GFL_NET_RingGets(RingBuffWork* pRing, u8* pDataArea, const int size, const int worksize);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@����1byte�f�[�^�𓾂�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @return  1byte�̃f�[�^ �����O�Ƀf�[�^���Ȃ��Ƃ���0
 */
//==============================================================================
extern u8 GFL_NET_RingGetByte(RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@����2byte�f�[�^�𓾂�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @return  2byte�̃f�[�^ �����O�Ƀf�[�^���Ȃ��Ƃ���0
 */
//==============================================================================
extern u16 GFL_NET_RingGetShort(RingBuffWork* pRing);
//==============================================================================
/**
 * @brief   �����O�o�b�t�@��2byte��������
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   data        u16�f�[�^
 * @return  none
 */
//==============================================================================
extern void GFL_NET_RingPutShort(RingBuffWork* pRing, u16 data);
//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^����  �ǂݍ��ނ����ňʒu��i�߂Ȃ�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �ǂݍ��݃o�b�t�@
 * @param   size         �ǂݍ��݃o�b�t�@�T�C�Y
 * @retval  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
extern int GFL_NET_RingChecks(const RingBuffWork* pRing, u8* pDataArea, const int size, const int worksize);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^�����������Ă��邩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  �f�[�^�T�C�Y
 */
//==============================================================================
extern int GFL_NET_RingDataSize(const RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^���ǂ̂��炢���܂��Ă��邩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
extern int GFL_NET_RingDataRestSize(const RingBuffWork* pRing);

//==============================================================================
/**
 * @brief   �J�E���^�[�ꏊ�����肩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  none
 */
//==============================================================================
extern void GFL_NET_RingEndChange(RingBuffWork* pRing);

#endif// __NET_RING_BUFF_H__


#ifdef __cplusplus
} /* extern "C" */
#endif
