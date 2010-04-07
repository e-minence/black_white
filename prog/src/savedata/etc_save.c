//==============================================================================
/**
 * @file    etc_save.c
 * @brief   ��2��MISC�Z�[�u
 * @author  matsuda
 * @date    2010.04.06(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/etc_save.h"



//==============================================================================
//  �萔��`
//==============================================================================
///�m�荇���o�^�ő吔
#define ACQUAINTANCE_ID_MAX     (32)


//==============================================================================
//  �\���̒�`
//==============================================================================
struct _ETC_SAVE_WORK{
  u32 acquaintance_trainer_id[ACQUAINTANCE_ID_MAX];
  u32 acquaintance_occ_num;   ///<acquaintance_trainer_id�̉��Ԗڂ܂Ńf�[�^�������Ă��邩
  
  u8 dummy[120];    ///<�����p��1�u���b�N���̃A���C�����g�S�Ă��m��
};



//==============================================================================
//
//  
//
//==============================================================================
//------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�T�C�Y��Ԃ�
 *
 * @retval  int		
 */
//------------------------------------------------------------------
u32 EtcSave_GetWorkSize( void )
{
	return sizeof(ETC_SAVE_WORK);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec		
 */
//--------------------------------------------------------------
void EtcSave_WorkInit(void *work)
{
  ETC_SAVE_WORK *intsave = work;
  
  GFL_STD_MemClear(intsave, sizeof(ETC_SAVE_WORK));
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * �����ރf�[�^���̂Q�̃Z�[�u�f�[�^�|�C���^�擾
 *
 * @param   p_sv		
 *
 * @retval  ETC_SAVE_WORK *		
 */
//==================================================================
ETC_SAVE_WORK * SaveData_GetEtc( SAVE_CONTROL_WORK * p_sv )
{
	return SaveControl_DataPtrGet(p_sv, GMDATA_ID_ETC);
}


//==============================================================================
//  
//==============================================================================
//==================================================================
/**
 * ���j�I���m�荇���O���[�v�Ƃ��ēo�^���܂�
 *
 * @param   etcsave		
 * @param   trainer_id		�g���[�i�[ID
 */
//==================================================================
void EtcSave_SetAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id)
{
  if(etcsave->acquaintance_occ_num < ACQUAINTANCE_ID_MAX){
    etcsave->acquaintance_trainer_id[etcsave->acquaintance_occ_num] = trainer_id;
    etcsave->acquaintance_occ_num++;
  }
  else{
    GFL_STD_MemCopy(&etcsave->acquaintance_trainer_id[1], 
      etcsave->acquaintance_trainer_id, sizeof(u32) * (ACQUAINTANCE_ID_MAX - 1));
    etcsave->acquaintance_trainer_id[ACQUAINTANCE_ID_MAX - 1] = trainer_id;
  }
}

//==================================================================
/**
 * ���j�I���m�荇���O���[�v�ɓo�^����Ă���l�������ׂ�
 *
 * @param   etcsave		
 * @param   trainer_id		�g���[�i�[ID
 *
 * @retval  BOOL		TRUE:�o�^����Ă���@�@FALSE:�o�^����Ă��Ȃ�
 */
//==================================================================
BOOL EtcSave_CheckAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id)
{
  int i;
  for(i = 0; i < etcsave->acquaintance_occ_num; i++){
    if(etcsave->acquaintance_trainer_id[i] == trainer_id){
      return TRUE;
    }
  }
  return FALSE;
}

