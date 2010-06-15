//==============================================================================
/**
 * @file    save_outside.c
 * @brief   �Z�[�u�Ǘ��O�̈�̃Z�[�u�V�X�e��
 * @author  matsuda
 * @date    2010.04.10(�y)
 */
//==============================================================================
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/save_outside.h"

#include "savedata/mystery_data.h"
#include "savedata/mystery_data_local.h"


//==============================================================================
//  �萔��`
//==============================================================================

///�Ǘ��O�Z�[�u�F�j���
typedef enum{
  OUTSIDE_BREAK_OK,     ///<���ʐ���
  OUTSIDE_BREAK_A,      ///<A�ʔj��
  OUTSIDE_BREAK_B,      ///<B�ʔj��
  OUTSIDE_BREAK_ALL,    ///<���ʔj��
}OUTSIDE_BREAK;

///�Ǘ��O�Z�[�u�F�}�W�b�N�i���o�[
#define OUTSIDE_MAGIC_NUMBER    (0xa10f49ae)

///�~���[�����O�̐�
enum{
  _MIRROR_A,
  _MIRROR_B,
  
  _MIRROR_NUM,
};


//==============================================================================
//  �\���̒�`
//==============================================================================
///�Ǘ��O�Z�[�u�F���ۂɃt���b�V���ɋL�^�����f�[�^(���ꂪ���̂܂܃t���b�V���ɏ������)
typedef struct{
  u32 magic_number;   ///<�f�[�^�̑��ݗL���������}�W�b�N�i���o�[
                      ///< (�폜�͂��������������ׁA�K���擪�ɔz�u���鎖�I�I)
  u32 global_count;
  u16 crc;            ///<OUTSIDE_MYSTERY��CRC�l
  u16 padding;
  OUTSIDE_MYSTERY mystery;
}OUTSIDE_MYSTERY_SAVEDATA;

///�Ǘ��O�Z�[�u�R���g���[���V�X�e��
struct _OUTSIDE_SAVE_CONTROL{
  OUTSIDE_MYSTERY_SAVEDATA mystery_save;
  OUTSIDE_BREAK break_flag;        ///<�t���b�V���̔j���(OUTSIDE_BREAK_xxx)
  u8 data_exists;                  ///<TRUE:�f�[�^�����݂���  FALSE:���݂��Ȃ�
  u8 save_seq;
  u8 write_side;
  u8 padding;
  u16 lock_id;
  u16 padding2;
};



//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * �Ǘ��O�Z�[�u�̃V�X�e�����쐬���A���[�h
 *
 * @param   heap_id		
 *
 * @retval  OUTSIDE_SAVE_CONTROL *		�쐬���ꂽ�Ǘ��O�Z�[�u�V�X�e���ւ̃|�C���^
 */
//==================================================================
OUTSIDE_SAVE_CONTROL * OutsideSave_SystemLoad(HEAPID heap_id)
{
  OUTSIDE_SAVE_CONTROL *outsv_ctrl;
  OUTSIDE_MYSTERY_SAVEDATA *buf[_MIRROR_NUM];
  BOOL load_ret[_MIRROR_NUM];  //FALSE:�f�[�^�����݂��Ȃ�
  BOOL crc_break[_MIRROR_NUM]; //TRUE:CRC����v���Ȃ�
  int no;
  int ok_side = -1;
  
  outsv_ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(OUTSIDE_SAVE_CONTROL));
  buf[_MIRROR_A] = &outsv_ctrl->mystery_save;  //A�ʂ̓V�X�e���̃o�b�t�@�����̂܂܎g�p
  buf[_MIRROR_B] = GFL_HEAP_AllocClearMemory(heap_id, sizeof(OUTSIDE_MYSTERY_SAVEDATA));
  
  for(no = 0; no < _MIRROR_NUM; no++){
    load_ret[no] = FALSE;
    crc_break[no] = FALSE;
  }
  
  load_ret[_MIRROR_A] = GFL_BACKUP_DirectFlashLoad(OUTSIDE_MM_MYSTERY, buf[_MIRROR_A], sizeof(OUTSIDE_MYSTERY_SAVEDATA));
  load_ret[_MIRROR_B] = GFL_BACKUP_DirectFlashLoad(OUTSIDE_MM_MYSTERY_MIRROR, buf[_MIRROR_B], sizeof(OUTSIDE_MYSTERY_SAVEDATA));
  
  for(no = 0; no < _MIRROR_NUM; no++){
    if(load_ret[no] == TRUE){
      //�}�W�b�N�i���o�[�Ńf�[�^�̑��݃`�F�b�N
      if(buf[no]->magic_number != OUTSIDE_MAGIC_NUMBER){
        load_ret[no] = FALSE;
        continue;
      }
      
      //CRC�`�F�b�N
      if(GFL_STD_CrcCalc(&buf[no]->mystery, sizeof(OUTSIDE_MYSTERY)) != buf[no]->crc){
        crc_break[no] = TRUE;
      }
    }
  }
  
  //���[�h���ʂ�CRC���`�F�b�N���āA�g�p����ʂƔj��󋵂𒲂ׂ�
  outsv_ctrl->data_exists = FALSE;
  if(load_ret[_MIRROR_A] == FALSE && load_ret[_MIRROR_B] == FALSE){
    //���ʃf�[�^�Ȃ�
    ;
  }
  else if(load_ret[_MIRROR_A] == TRUE && load_ret[_MIRROR_B] == FALSE){
    if(crc_break[_MIRROR_A] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //A�ʂ݂̂��邪�A�f�[�^�j��
    }
    else{
      ok_side = _MIRROR_A;
    }
  }
  else if(load_ret[_MIRROR_A] == FALSE && load_ret[_MIRROR_B] == TRUE){
    if(crc_break[_MIRROR_B] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //B�ʂ݂̂��邪�A�f�[�^�j��
    }
    else{
      ok_side = _MIRROR_B;
    }
  }
  else if(load_ret[_MIRROR_A] == TRUE && load_ret[_MIRROR_B] == TRUE){
    if(crc_break[_MIRROR_A] == TRUE && crc_break[_MIRROR_B] == TRUE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_ALL; //���ʃf�[�^�j��
    }
    else if(crc_break[_MIRROR_A] == FALSE && crc_break[_MIRROR_B] == FALSE){
      if(buf[_MIRROR_A]->global_count >= buf[_MIRROR_B]->global_count
          || (buf[_MIRROR_A]->global_count == 0 && buf[_MIRROR_B]->global_count == 0xffffffff)){
        ok_side = _MIRROR_A;
      }
      else{
        ok_side = _MIRROR_B;
      }
    }
    else if(crc_break[_MIRROR_A] == FALSE){
      outsv_ctrl->break_flag = OUTSIDE_BREAK_B;
      ok_side = _MIRROR_A;
    }
    else{
      outsv_ctrl->break_flag = OUTSIDE_BREAK_A;
      ok_side = _MIRROR_B;
    }
  }
  
  //B�ʃo�b�t�@�͔j������̂ŁA����ȃf�[�^��B�ʂɂ������ꍇ��A�ʃo�b�t�@�ɃR�s�[
  if(ok_side == _MIRROR_B){
    GFL_STD_MemCopy(buf[_MIRROR_B], buf[_MIRROR_A], sizeof(OUTSIDE_MYSTERY_SAVEDATA));
  }
  GFL_HEAP_FreeMemory(buf[_MIRROR_B]);

  if(ok_side != -1){
    outsv_ctrl->data_exists = TRUE;
  }
  return outsv_ctrl;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�V�X�e����j������
 *
 * @param   outsv_ctrl		
 */
//==================================================================
void OutsideSave_SystemUnload(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  GFL_HEAP_FreeMemory(outsv_ctrl);
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F�����Z�[�u����������
 *
 * @param   outsv_ctrl		
 */
//==================================================================
void OutsideSave_SaveAsyncInit(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  outsv_ctrl->save_seq = 0;
  if(outsv_ctrl->break_flag == OUTSIDE_BREAK_B){
    outsv_ctrl->write_side = _MIRROR_B; //B�ʂ����Ă���ꍇ��B�ʂ���
  }
  else{
    outsv_ctrl->write_side = _MIRROR_A; //����ȊO�͑S��A�ʂ���
  }
  
  outsv_ctrl->mystery_save.magic_number = OUTSIDE_MAGIC_NUMBER;
  outsv_ctrl->mystery_save.global_count++;
  outsv_ctrl->mystery_save.crc 
    = GFL_STD_CrcCalc(&outsv_ctrl->mystery_save.mystery, sizeof(OUTSIDE_MYSTERY));
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�F�����Z�[�u���C������
 *
 * @param   outsv_ctrl		
 *
 * @retval  BOOL		TRUE:�Z�[�u�����@FALSE:�Z�[�u�����p����
 */
//==================================================================
BOOL OutsideSave_SaveAsyncMain(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  u32 flash_src;
  BOOL err_result;
  
  switch(outsv_ctrl->save_seq){
  case 0:
  case 2:
    OS_TPrintf("�Ǘ��O�Z�[�u write_side = %d\n", outsv_ctrl->write_side);
    flash_src = outsv_ctrl->write_side == _MIRROR_A ? OUTSIDE_MM_MYSTERY : OUTSIDE_MM_MYSTERY_MIRROR;
    outsv_ctrl->lock_id = GFL_BACKUP_DirectFlashSaveAsyncInit(
      flash_src, &outsv_ctrl->mystery_save, sizeof(OUTSIDE_MYSTERY_SAVEDATA) );
    outsv_ctrl->save_seq++;
    break;
  case 1:
  case 3:
    if(GFL_BACKUP_DirectFlashSaveAsyncMain(outsv_ctrl->lock_id, &err_result) == TRUE){
      OS_TPrintf("�Ǘ��O�Z�[�u result(1=����) = %d\n", err_result);
      outsv_ctrl->write_side ^= 1;
      outsv_ctrl->save_seq++;
    }
    break;
  default:
    outsv_ctrl->break_flag = OUTSIDE_BREAK_OK;
    outsv_ctrl->data_exists = TRUE;
    return TRUE;
  }
  return FALSE;
}


//==============================================================================
//  �c�[��
//==============================================================================
//==================================================================
/**
 * �Z�[�u�f�[�^�̑��݃t���O�̎擾
 *
 * @param   outsv_ctrl		
 *
 * @retval  BOOL		TRUE:�f�[�^�����݂���@FALSE:�f�[�^������
 */
//==================================================================
BOOL OutsideSave_GetExistFlag(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  return outsv_ctrl->data_exists;
}

//==================================================================
/**
 * �Z�[�u�f�[�^�̔j��t���O�̎擾
 *
 * @param   outsv_ctrl		
 *
 * @retval  BOOL		TRUE:���ʃf�[�^�j��@FALSE:����Ȗʂ�����
 */
//==================================================================
BOOL OutsideSave_GetBreakFlag(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  if(outsv_ctrl->break_flag != OUTSIDE_BREAK_ALL){
    return FALSE;
  }
  return TRUE;
}

//==================================================================
/**
 * �s�v�c�ȑ��蕨�f�[�^�ւ̃|�C���^���擾����
 *
 * @param   outsv_ctrl		
 *
 * @retval  OUTSIDE_MYSTERY *		
 */
//==================================================================
OUTSIDE_MYSTERY * OutsideSave_GetMysterPtr(OUTSIDE_SAVE_CONTROL *outsv_ctrl)
{
  return &outsv_ctrl->mystery_save.mystery;
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�̃f�[�^��ʏ�Z�[�u�̈�ɃR�s�[����
 *
 * @param   outsv_ctrl		�Ǘ��O�Z�[�u�V�X�e���ւ̃|�C���^
 * @param   normal_ctrl		�ʏ�Z�[�u�V�X�e���ւ̃|�C���^
 */
//==================================================================
void OutsideSave_MysteryData_Outside_to_Normal(OUTSIDE_SAVE_CONTROL *outsv_ctrl, SAVE_CONTROL_WORK *normal_ctrl)
{
  MYSTERY_ORIGINAL_DATA *normal_mystery = SaveControl_DataPtrGet(normal_ctrl, GMDATA_ID_MYSTERYDATA);
  OUTSIDE_MYSTERY *outside_mystery = &outsv_ctrl->mystery_save.mystery;
  int i;

  //������
  MYSTERYDATA_Decoded( normal_mystery, MYSTERY_DATA_TYPE_ORIGINAL );
  MYSTERYDATA_Decoded( outside_mystery, MYSTERY_DATA_TYPE_OUTSIDE );

  //�f�[�^������
  for(i = 0; i < MYSTERY_DATA_MAX_EVENT / 8; i++){
    normal_mystery->recv_flag[i] = outside_mystery->recv_flag[i];
  }
  for(i = 0; i < OUTSIDE_MYSTERY_MAX; i++){
    normal_mystery->card[i] = outside_mystery->card[i];
  }

  //�Í���
  MYSTERYDATA_Coded( normal_mystery, MYSTERY_DATA_TYPE_ORIGINAL );
  MYSTERYDATA_Coded( outside_mystery, MYSTERY_DATA_TYPE_OUTSIDE );
}

//==================================================================
/**
 * �Ǘ��O�Z�[�u�S�̂��N���A����
 *
 * @param   heap_id		
 */
//==================================================================
void OutsideSave_FlashAllErase(HEAPID heap_id)
{
  u32 erase_size = SAVESIZE_OUTSIDE_MYSTERY * 2;  //*2=�~���[������
  u8 *erase_buffer = GFL_HEAP_AllocMemory(heap_id, erase_size);
  
  GFL_STD_MemFill32(erase_buffer, 0xffffffff, erase_size);
  GFL_BACKUP_DirectFlashSave(OUTSIDE_MM_MYSTERY, erase_buffer, erase_size);
  
  GFL_HEAP_FreeMemory(erase_buffer);
}

//==============================================================================
//  �f�o�b�O�p
//==============================================================================
#ifdef PM_DEBUG
//==================================================================
/**
 * �f�o�b�O�p�F�Z�[�u�f�[�^�̔j��t���O�̎擾
 *
 * @param   outsv_ctrl		
 * @param   ret_a		      A�ʌ��ʑ����(TRUE:�f�[�^�j��)
 * @param   ret_b		      B�ʌ��ʑ����(TRUE:�f�[�^�j��)
 *
 * @retval  BOOL		TRUE:�f�[�^����@FALSE:�f�[�^����
 */
//==================================================================
BOOL DEBUG_OutsideSave_GetBreak(OUTSIDE_SAVE_CONTROL *outsv_ctrl, BOOL *ret_a, BOOL *ret_b)
{
  *ret_a = FALSE;
  *ret_b = FALSE;
  switch(outsv_ctrl->break_flag){
  case OUTSIDE_BREAK_OK:     //���ʐ���
    return outsv_ctrl->data_exists;
  case OUTSIDE_BREAK_A:      //A�ʔj��
    *ret_a = TRUE;
    break;
  case OUTSIDE_BREAK_B:      //B�ʔj��
    *ret_b = TRUE;
    break;
  case OUTSIDE_BREAK_ALL:    //���ʔj��
    *ret_a = TRUE;
    *ret_b = TRUE;
    break;
  }
  
  return TRUE;
}
#endif  //PM_DEBUG

