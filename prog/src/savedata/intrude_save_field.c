//==============================================================================
/**
 * @file    intrude_save_field.c
 * @brief   �N���̃Z�[�u�f�[�^�֘A�Ńt�B�[���h�ł����A�N�Z�X���Ȃ�����
 * @author  matsuda
 * @date    2010.02.01(��)
 *
 * FIELDMAP�I�[�o�[���C�ɔz�u
 */
//==============================================================================
#include <gflib.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/intrude_save.h"
#include "savedata/intrude_save_field.h"
#include "savedata/intrude_save_local.h"
#include "field/intrude_secret_item.h"


///ISC_SAVE_CheckItem�Ńq�b�g���Ȃ������ꍇ�̖߂�l
#define ISC_SAVE_SEARCH_NONE    (0xff)


//==================================================================
/**
 * �Z�[�u���[�N�Ɏw����W�̈ʒu�ɉB���A�C�e�������邩���ׂ�
 *
 * @param   intsave		
 * @param   zone_id		�]�[��ID
 * @param   grid_x		�O���b�h���WX
 * @param   grid_y		�O���b�h���WY
 * @param   grid_z		�O���b�h���WZ
 *
 * @retval  int		    �q�b�g�����ꍇ�F���[�N�ʒu
 * @retval  int       �q�b�g���Ȃ������ꍇ�FISC_SAVE_SEARCH_NONE
 */
//==================================================================
int ISC_SAVE_CheckItem(INTRUDE_SAVE_WORK *intsave, u16 zone_id, s16 grid_x, s16 grid_y, s16 grid_z)
{
  int i;
  const INTRUDE_SECRET_ITEM_SAVE *isis = intsave->secret_item;
  const INTRUDE_SECRET_ITEM_POSDATA *posdata;
  
  for(i = 0; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    if(isis->item == 0){
      return ISC_SAVE_SEARCH_NONE;
    }
    posdata = &IntrudeSecretItemPosDataTbl[isis->tbl_no];
    if(posdata->zone_id == zone_id && posdata->grid_x == grid_x 
        && posdata->grid_y == grid_y && posdata->grid_z == grid_z){
      return i;
    }
    isis++;
  }
  return ISC_SAVE_SEARCH_NONE;
}

//==================================================================
/**
 * �w�胏�[�N�ʒu�̉B���A�C�e���f�[�^���擾���܂�
 *
 * @param   intsave		
 * @param   work_no		���[�N�ʒu(ISC_SAVE_CheckItem�̖߂�l)
 * @param   dest      �A�C�e���f�[�^�����ւ̃|�C���^
 *
 * ���̊֐����g�p����ƁA�擾�����f�[�^���폜���A�f�[�^�̑O�l�ߏ��������s����܂�
 */
//==================================================================
void ISC_SAVE_GetItem(INTRUDE_SAVE_WORK *intsave, int work_no, INTRUDE_SECRET_ITEM_SAVE *dest)
{
  int i;
  
  GF_ASSERT(work_no != ISC_SAVE_SEARCH_NONE);
  
  *dest = intsave->secret_item[work_no];
  GF_ASSERT(dest->item != 0);
  
  //�O�l��
  for(i = work_no + 1; i < INTRUDE_SECRET_ITEM_SAVE_MAX; i++){
    intsave->secret_item[i - 1] = intsave->secret_item[i];
  }
  intsave->secret_item[INTRUDE_SECRET_ITEM_SAVE_MAX - 1].item = 0;
}

//==================================================================
/**
 * �Z�[�u�f�[�^�ɂ���A�C�e���̔z�u���W�f�[�^�ւ̃|�C���^���擾����
 *
 * @param   intsave		
 * @param   work_no		�Z�[�u�f�[�^�̌����J�n���郏�[�N�ʒu
 *
 * @retval  const INTRUDE_SECRET_ITEM_POSDATA *		�z�u���W�ւ̃|�C���^(�q�b�g���Ȃ������ꍇ��NULL)
 *
 * �_�E�W���O�}�V�����A���炩���߉B���A�C�e�����z�u����Ă�����W�ꗗ��
 * �擾���Ă��������ꍇ�Ɏg�p����
 * 
 * �g�p��)
 *    work_no = 0;
 *    while(1){
 *      posdata = ISC_SAVE_GetItemPosData(intsave, &work_no);
 *      if(posdata == NULL){
 *        break;  //�Z�[�u�̏I�[�܂ŒT��������
 *      }
 *      my_buffer[count++] = *posdata;  //���W�f�[�^�������̃o�b�t�@�ɃR�s�[
 *    }
 */
//==================================================================
const INTRUDE_SECRET_ITEM_POSDATA * ISC_SAVE_GetItemPosData(INTRUDE_SAVE_WORK *intsave, int *work_no)
{
  const INTRUDE_SECRET_ITEM_POSDATA *posdata;
  
  for( ; *work_no < INTRUDE_SECRET_ITEM_SAVE_MAX; (*work_no)++){
    if(intsave->secret_item[*work_no].item != 0){
      posdata = &IntrudeSecretItemPosDataTbl[intsave->secret_item[*work_no].tbl_no];
      (*work_no)++; //���̌�����1�悩��i�ނ悤�ɃC���N�������g
      return posdata;
    }
  }
  return NULL;
}

