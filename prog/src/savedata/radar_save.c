//==============================================================================
/**
 * @file    radar_save.c
 * @brief   �������[�_�[�Z�[�u�f�[�^
 * @author  matsuda
 * @date    2010.01.06(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/radar_save.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�g���[�i�[ID�ۑ���
#define RADAR_TRAINER_ID_MAX      (50)

///�������ڐ�
#define RADAR_ITEM_MAX            (30)

///�����̓������ڐ�
#define RADAR_TODAY_ANSWER_MAX    (150)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�������[�_�[�Z�[�u�f�[�^
typedef struct{
  u32 trainer_id[RADAR_TRAINER_ID_MAX];       ///<�g���[�i�[ID
  
  //�����̓��v�f�[�^
  u16 today_count[RADAR_ITEM_MAX];            ///<�e30���ڂ̍����̒����l��
  u8 today_answer[RADAR_TODAY_ANSWER_MAX];    ///<�e30���ڂ̍����̓���
  
  //�����̓��v�f�[�^
  u32 total_today_count[RADAR_ITEM_MAX];      ///<�e30���ڂ̍��v�̒����l��
  u16 total_today_answer[RADAR_TODAY_ANSWER_MAX]; ///<�e30���ڂ̍��v�̓���150����
  
  //�����̓��v�f�[�^
  u8 mine_answer[RADAR_ITEM_MAX];             ///<�����̉񓚃f�[�^�e30����
}RADAR_SAVE_WORK;




//------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�T�C�Y��Ԃ�
 *
 * @retval  int		
 */
//------------------------------------------------------------------
u32 RadarSave_GetWorkSize( void )
{
	return sizeof(RADAR_SAVE_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void RadarSave_WorkInit(void *work)
{
  RADAR_SAVE_WORK *radar_save = work;
  
  GFL_STD_MemClear(radar_save, sizeof(RADAR_SAVE_WORK));
}

