//==============================================================================
/**
 * @file    intrude_save_local.h
 * @brief   �N���Z�[�u�f�[�^�F���[�J���w�b�_
 * @author  matsuda
 * @date    2010.02.01(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �萔��`
//==============================================================================
///�N���B���A�C�e���ێ���
#define INTRUDE_SECRET_ITEM_SAVE_MAX    (20)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�N���Z�[�u���[�N
struct _INTRUDE_SAVE_WORK{
  OCCUPY_INFO occupy;
  INTRUDE_SECRET_ITEM_SAVE secret_item[INTRUDE_SECRET_ITEM_SAVE_MAX]; ///<�N���B���A�C�e��
  u8 gpower_id;           ///<���g���������Ă���G�p���[ID(�Z�b�g���Ă��Ȃ��ꍇ��GPOWER_ID_NULL)
  u8 gpower_distribution_bit[INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX]; ///<G�p���[�z�z��Mbit
  u8 padding;
  u32 clear_mission_count;       ///<�~�b�V�����N���A��
  s64 palace_sojourn_time;       ///<�p���X�؍ݎ���
};
