//==============================================================================
/**
 * @file    mystery_data_local.h
 * @brief   �s�v�c�ȑ��蕨���[�J���w�b�_
 * @author  matsuda
 * @date    2010.04.11(��)
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================
#pragma once


//------------------------------------------------------------------
/**
 * @brief	�ӂ����f�[�^�̒�`
 */
//------------------------------------------------------------------
typedef struct{
  u16 crc16ccitt_hash;  ///<CRC
  u16 coded_number;   ///<�Í����L�[
}RECORD_CRC;


struct MYSTERY_DATA{
  u8 recv_flag[MYSTERY_DATA_MAX_EVENT / 8];		//256 * 8 = 2048 bit
  GIFT_PACK_DATA card[GIFT_DATA_MAX];			// �J�[�h���
  RECORD_CRC crc;   //CRC & �Í����L�[   4 byte
};
