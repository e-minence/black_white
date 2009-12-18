//==============================================================================
/**
 * @file    field_oam_pal.h
 * @brief   �t�B�[���h�}�b�v�F���C����ʂ�OAM�p���b�g�g�p�ʒu��`�t�@�C��
 * @author  matsuda
 * @date    2009.12.18(��)
 */
//==============================================================================
#pragma once

//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v�F���C����ʂ�OAM�p���b�g�ԍ�
 */
//--------------------------------------------------------------
typedef enum{
//--------------------------------------------------------------
//  �Œ�G���A
//--------------------------------------------------------------
  FLDOAM_PALNO_WEATHER,       ///<�V�C
  FLDOAM_PALNO_PLACENAME,     ///<�n���\��
  FLDOAM_PALNO_Y_MENU,        ///<Y�{�^���V���[�g�J�b�g���j���[
  
//--------------------------------------------------------------
//  �t���[�G���A
//--------------------------------------------------------------
  FLDOAM_PALNO_FREE_START,    ///<�C�x���g���Ɏg�p�o����t���[�G���A�J�n�ʒu
  
  //�莝�� or �o�g���{�b�N�X�I�����X�g �C�x���g��
  FLDOAM_PALNO_FREE_PSL_POKEICON = FLDOAM_PALNO_FREE_START,
  FLDOAM_PALNO_FREE_PSL_ITEM,
  FLDOAM_PALNO_FREE_PSL_FONTOAM,

  //�n���S�X�e�[�^�X �C�x���g��(��)�@���t���[�G���A�̃C���[�W��͂�ł��炤�����̃_�~�[
  FLDOAM_PALNO_FREE_SUBWAY_SHADOW = FLDOAM_PALNO_FREE_START,
  FLDOAM_PALNO_FREE_SUBWAY_ICON,
}FLDOAM_PALNO;

