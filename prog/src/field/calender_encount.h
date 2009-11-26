#pragma once 

/////////////////////////////////////////////////////////////////////////////////// 
/**
 *
 * @brief  �J�����_�[�̃G���J�E���g���f�[�^
 * @author obata
 * @date   2009.08.05
 *
 */
/////////////////////////////////////////////////////////////////////////////////// 

#include <gflib.h>


// �s���S�^�̐錾
typedef struct _CALENDER_ENCOUNT_INFO CALENDER_ENCOUNT_INFO;


// �o�����ԍ�
enum 
{
  ENCOUNT_NO_0,
  ENCOUNT_NO_1,
  ENCOUNT_NO_2,
  ENCOUNT_NO_3,
  ENCOUNT_NO_4,
  ENCOUNT_NO_MAX
};


//---------------------------------------------------------------------------------
/**
 * @brief �G���J�E���g�E�f�[�^���쐬����
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����G���J�E���g�E�f�[�^
 */
//---------------------------------------------------------------------------------
extern CALENDER_ENCOUNT_INFO* CALENDER_ENCOUNT_INFO_Create( HEAPID heap_id );

//---------------------------------------------------------------------------------
/**
 * @brief �w����̏o�����ω��ԍ����擾����
 *
 * @param p_info �f�[�^
 * @parma p_date ���t
 *
 * @return �w�肵�����t�̏o�����ω��ԍ�( ENCOUNT_NO_x )
 */
//---------------------------------------------------------------------------------
extern u8 CALENDER_ENCOUNT_INFO_GetEncountNo( CALENDER_ENCOUNT_INFO* p_info, const RTCDate* p_date ); 

//---------------------------------------------------------------------------------
/**
 * @brief �G���J�E���g�E�f�[�^��j������
 *
 * @param p_info �j������f�[�^
 */ 
//---------------------------------------------------------------------------------
extern void CALENDER_ENCOUNT_INFO_Delete( CALENDER_ENCOUNT_INFO* p_info ); 
