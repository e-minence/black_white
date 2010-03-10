//==============================================================================
/**
 * @file    intrude_save_field.h
 * @brief   �N���̃Z�[�u�f�[�^�֘A�Ńt�B�[���h�ł����A�N�Z�X���Ȃ�����
 * @author  matsuda
 * @date    2010.02.01(��)
 */
//==============================================================================
#pragma once

#include "savedata/intrude_save.h"
#include "field/intrude_secret_item.h"


//==============================================================================
//  �萔��`
//==============================================================================
///ISC_SAVE_CheckItem�Ńq�b�g���Ȃ������ꍇ�̖߂�l
#define ISC_SAVE_SEARCH_NONE    (0xff)



//==============================================================================
//  �O���֐��錾
//==============================================================================
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
extern int ISC_SAVE_CheckItem(INTRUDE_SAVE_WORK *intsave, u16 zone_id, s16 grid_x, s16 grid_y, s16 grid_z);

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
extern void ISC_SAVE_GetItem(INTRUDE_SAVE_WORK *intsave, int work_no, INTRUDE_SECRET_ITEM_SAVE *dest);

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
extern const INTRUDE_SECRET_ITEM_POSDATA * ISC_SAVE_GetItemPosData(INTRUDE_SAVE_WORK *intsave, int *work_no);


//==============================================================================
//  
//==============================================================================
extern void INTRUDE_OCCUPY_FIELD_LevelUpWhite(OCCUPY_INFO *occupy, int add_level);
extern void INTRUDE_OCCUPY_FIELD_LevelUpBlack(OCCUPY_INFO *occupy, int add_level);
