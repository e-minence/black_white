//=============================================================================
/**
 * @file	net_ring_buff.h
 * @brief	�����O�o�b�t�@�̎d�g�݂��Ǘ�����֐�
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================


#ifndef __NET_RING_BUFF_H__
#define __NET_RING_BUFF_H__

/// @brief �����O�o�b�t�@�\����
typedef struct{
    u8* pWork;                   ///< �ۑ��̈�̐擪�|�C���^
    s16 startPos;                ///< �ǂݏo���Ă��Ȃ��f�[�^�̈ʒu
 //   volatile s16 endPos;         ///< ������Ă���f�[�^�̍Ō�̈ʒu
    volatile s16 backupEndPos;   ///< �Ō�̈ʒu�̃o�b�N�A�b�v
    s16 size;                    ///< �ۑ��̈�T�C�Y
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
 * @param   size         �ǂݍ��݃o�b�t�@�T�C�Y
 * @return  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
extern int GFL_NET_RingGets(RingBuffWork* pRing, u8* pDataArea, const int size);

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
 * @brief   �����O�o�b�t�@�̃f�[�^����  �ǂݍ��ނ����ňʒu��i�߂Ȃ�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �ǂݍ��݃o�b�t�@
 * @param   size         �ǂݍ��݃o�b�t�@�T�C�Y
 * @retval  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
extern int GFL_NET_RingChecks(const RingBuffWork* pRing, u8* pDataArea, const int size);

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

