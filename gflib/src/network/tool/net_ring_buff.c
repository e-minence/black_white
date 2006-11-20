//=============================================================================
/**
 * @file	net_ring_buff.c
 * @brief	�����O�o�b�t�@�̎d�g�݂��Ǘ�����֐�
 * @author	katsumi ohno
 * @date	05/09/16
 */
//=============================================================================

#include "gflib.h"
#include "net_ring_buff.h"



static int _ringPos(const RingBuffWork* pRing,const int i);

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�Ǘ��\���̏�����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �����O�o�b�t�@�������[
 * @param   size         �����O�o�b�t�@�������[�T�C�Y
 * @return  none
 */
//==============================================================================
void GFL_NET_RingInitialize(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    pRing->pWork = pDataArea;
    pRing->size = (s16)size;
    pRing->startPos = 0;
    pRing->endPos = 0;
    pRing->backupEndPos = 0;
//    pRing->backupStartPos = 0;
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�ɏ�������
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �������ރf�[�^
 * @param   size         �������݃T�C�Y
 * @retval  none
 */
//==============================================================================
void GFL_NET_RingPuts(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i,j;

    // ������ASSERT�Ɉ���������Ƃ������Ƃ�
    // �������x�����Ă���̂ɁA�ʐM�͖�sync����̂�
    // �X�g�b�N����o�b�t�@���܂ł��ӂ��ƁA�����Ŏ~�܂�܂��B
    // �ʐM�̃o�b�t�@�𑝂₷���A���̕����̏����𕪒f���邩�A�Ώ����K�v�ł��B
    // �ŏI�I�ɂ͂��̃G���[������ƒʐM��ؒf���܂��B
    if(GFL_NET_RingDataRestSize(pRing) <= size){
#ifdef DEBUG_ONLY_FOR_ohno
        OHNO_PRINT("%d %d line %d \n",GFL_NET_RingDataRestSize(pRing),size,line);
        GF_ASSERT_MSG(0,"GFL_NET_RingOVER %d %d",GFL_NET_RingDataRestSize(pRing),size);
#endif
//        GFL_NET_SetError();
        return;
    }
    j = 0;
    for(i = pRing->backupEndPos; i < pRing->backupEndPos + size; i++,j++){
        GF_ASSERT(pDataArea);
        pRing->pWork[_ringPos( pRing, i )] = pDataArea[j];
    }
    pRing->backupEndPos = (s16)_ringPos( pRing, i );
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@����f�[�^�𓾂�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �ǂݍ��݃o�b�t�@
 * @param   size         �ǂݍ��݃o�b�t�@�T�C�Y
 * @return  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
int GFL_NET_RingGets(RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i;

    i = GFL_NET_RingChecks(pRing, pDataArea, size);
    pRing->startPos = (s16)_ringPos( pRing, pRing->startPos + i);

    return i;
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@����1byte�f�[�^�𓾂�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @return  1byte�̃f�[�^ �����O�Ƀf�[�^���Ȃ��Ƃ���0
 */
//==============================================================================
u8 GFL_NET_RingGetByte(RingBuffWork* pRing)
{
    u8 byte;

    if(1==GFL_NET_RingGets(pRing, &byte, 1)){
        return byte;
    }
    return 0;
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^����  �ǂݍ��ނ����ňʒu��i�߂Ȃ�
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @param   pDataArea    �ǂݍ��݃o�b�t�@
 * @param   size         �ǂݍ��݃o�b�t�@�T�C�Y
 * @retval  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
int GFL_NET_RingChecks(const RingBuffWork* pRing, u8* pDataArea, const int size)
{
    int i,j;

    j = 0;
    for(i = pRing->startPos; i < pRing->startPos + size; i++,j++){
        if(pRing->endPos == _ringPos( pRing,i )){
            return j;
        }
        pDataArea[j] = pRing->pWork[_ringPos( pRing,i )];
    }
    return j;
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^�����������Ă��邩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  �f�[�^�T�C�Y
 */
//==============================================================================
int GFL_NET_RingDataSize(const RingBuffWork* pRing)
{
    if(pRing->startPos > pRing->endPos){
        return (pRing->size + pRing->endPos - pRing->startPos);
    }
    return (pRing->endPos - pRing->startPos);
}

//==============================================================================
/**
 * @brief   �����O�o�b�t�@�̃f�[�^���ǂ̂��炢���܂��Ă��邩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  ���ۂɓǂݍ��񂾃f�[�^
 */
//==============================================================================
int GFL_NET_RingDataRestSize(const RingBuffWork* pRing)
{
    return (pRing->size - GFL_NET_RingDataSize(pRing));
}

//==============================================================================
/**
 * @brief   ring�T�C�Y�̏ꏊ�𓾂�
 * @param   pRing       �����O�o�b�t�@�Ǘ��|�C���^
 * @param   i           �o�C�g�ʒu
 * @return  �␳�����o�C�g�ʒu
 */
//==============================================================================
static int _ringPos(const RingBuffWork* pRing, const int i)
{
    return i % pRing->size;
}

//==============================================================================
/**
 * @brief   �J�E���^�[�ꏊ�����肩����
 * @param   pRing        �����O�o�b�t�@�Ǘ��|�C���^
 * @retval  none
 */
//==============================================================================
void GFL_NET_RingEndChange(RingBuffWork* pRing)
{
    pRing->endPos = pRing->backupEndPos;
}

