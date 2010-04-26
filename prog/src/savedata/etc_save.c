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

///�}�j���A���t���O�p
#define MANUAL_SECTOR_BIT       ( 32 )
#define MANUAL_FLAG_WORK_MAX    ( MANUAL_FLAG_MAX/MANUAL_SECTOR_BIT ) ///< 1bit�Ή��Ȃ̂�

//==============================================================================
//  �\���̒�`
//==============================================================================
struct _ETC_SAVE_WORK{
  u32 acquaintance_trainer_id[ACQUAINTANCE_ID_MAX];
  u32 acquaintance_occ_num;   ///<acquaintance_trainer_id�̉��Ԗڂ܂Ńf�[�^�������Ă��邩
  
  u32 manual_flag[MANUAL_FLAG_WORK_MAX]; ///< �Q�[�����}�j���A���̍��ڂ�ǂ񂾂��ǂ���(1bit�P�ʁj
  
  u8 dummy[88];    ///<�����p��1�u���b�N���̃A���C�����g�S�Ă��m��
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
 * @param   trainer_id    �g���[�i�[ID
 */
//==================================================================
void EtcSave_SetAcquaintance(ETC_SAVE_WORK *etcsave, u32 trainer_id)
{
  int i, copy_num;
  
  for(i = 0; i < etcsave->acquaintance_occ_num; i++){
    if(etcsave->acquaintance_trainer_id[etcsave->acquaintance_occ_num] == trainer_id){
      //���ɓo�^�ς݂̐l�Ȃ̂ōŌ���ɕt������(�����X�V)
      copy_num = (etcsave->acquaintance_occ_num - 1 - i);
      if(copy_num <= 0){
        return; //���ɍŌ��
      }
      GFL_STD_MemCopy(&etcsave->acquaintance_trainer_id[i + 1], 
        &etcsave->acquaintance_trainer_id[i], sizeof(u32) * copy_num);
      etcsave->acquaintance_trainer_id[etcsave->acquaintance_occ_num - 1] = trainer_id;
      return;
    }
  }
  
  //�V�K�o�^
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
 * @param   trainer_id    �g���[�i�[ID
 *
 * @retval  BOOL    TRUE:�o�^����Ă���@�@FALSE:�o�^����Ă��Ȃ�
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

//=============================================================================================
/**
 * @brief �}�j���A���t���O�̎擾
 *
 * @param   etcsave   ETC_SAVE_WORK
 * @param   id        �t���O�ԍ��imax��MANUAL_FLAG_MAX(256)
 *
 * @retval  BOOL      TRUE:�ǂ� FALSE:�܂�
 */
//=============================================================================================
BOOL EtcSave_GetManualFlag( const ETC_SAVE_WORK *etcsave, int id )
{
  int sector, bit;
  GF_ASSERT( id < MANUAL_FLAG_MAX );

  sector = id/MANUAL_SECTOR_BIT;
  bit    = id%MANUAL_SECTOR_BIT;

  return (etcsave->manual_flag[sector]>>bit)&1; 

}

//=============================================================================================
/**
 * @brief �}�j���A���t���O�𗧂Ă�
 *
 * @param   etcsave   ETC_SAVE_WORK
 * @param   id        �t���O�ԍ��imax��MANUAL_FLAG_MAX(256)
 *
 * @retval  BOOL      TRUE:�ǂ� FALSE:�܂�
 */
//=============================================================================================
void EtcSave_SetManualFlag( ETC_SAVE_WORK *etcsave, int id )
{
  int sector, bit;
  
  // �t���O��1bit�P�ʂŊǗ�
  if(id<MANUAL_FLAG_MAX){

    sector = id/MANUAL_SECTOR_BIT;
    bit    = id%MANUAL_SECTOR_BIT;
  
    etcsave->manual_flag[sector] |= 1<<bit; 

  }
}