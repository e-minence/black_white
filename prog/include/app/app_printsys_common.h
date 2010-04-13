//=============================================================================
/**
 * @file	  app_printsys_common.h
 * @brief	  PRINTSYS_PrintStreamGetState�̏��������ʉ�����v���O����
 * @author  k.ohno
 * @date	  10/02/22
 */
//=============================================================================

#pragma once

#include "print/printsys.h"
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#include "sound/wb_sound_data.sadl"
#else
#include "sound/pm_sndsys.h"
#endif

//-------------------------------------
///	��`
//=====================================
//���̓L�[
#define APP_PRINTSYS_COMMON_TRG_KEY (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)

//�y�[�W���艹
#define APP_PRINTSYS_COMMON_SE_TRG  (SEQ_SE_MESSAGE)

//-------------------------------------
///	�^�b�`����
//=====================================
typedef enum
{
  APP_PRINTSYS_COMMON_TYPE_THROUGH,
  APP_PRINTSYS_COMMON_TYPE_KEY      = 1<<1,
  APP_PRINTSYS_COMMON_TYPE_TOUCH    = 1<<2,
  APP_PRINTSYS_COMMON_TYPE_BOTH     = APP_PRINTSYS_COMMON_TYPE_KEY|APP_PRINTSYS_COMMON_TYPE_TOUCH,

  APP_PRINTSYS_COMMON_TYPE_NOTSKIP  = 1<<10,  //�������L�ƃ}�X�N����Ƌ��p���Ďg���܂�
}APP_PRINTSYS_COMMON_TYPE;

//-------------------------------------
///	���䃏�[�N
//=====================================
typedef struct 
{
  u8                         trg;  //���������m
  u8                         dummy[3];
  APP_PRINTSYS_COMMON_TYPE   type; //�g���K�Ŏg�p�������
} APP_PRINTSYS_COMMON_WORK;


//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����䏉����
 *	@param	�g���K�[�𐧌䂷�胏�[�N u8
 */
//-----------------------------------------------------------------------------
extern void APP_PRINTSYS_COMMON_PrintStreamInit(APP_PRINTSYS_COMMON_WORK *wk, APP_PRINTSYS_COMMON_TYPE type);
//----------------------------------------------------------------------------
/**
 *	@brief	�v�����g�V�X�e�����s�֐�
 *  @param  PRINT_STREAM* �X�g���[���n���h��
 *	@param	�������œn�����g���K�[�𐧌䂷�胏�[�N u8
 */
//-----------------------------------------------------------------------------
extern BOOL APP_PRINTSYS_COMMON_PrintStreamFunc( APP_PRINTSYS_COMMON_WORK *wk, PRINT_STREAM* handle );

