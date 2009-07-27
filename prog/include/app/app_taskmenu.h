//======================================================================
/**
 * @file	bmp_taskmenu.h
 * @brief	�^�X�N�o�[�p ���ʃ��j���[
 * @author	ariizumi
 * @data	09/07/27
 *
 * ���W���[�����FBMP_TASKMENU
 */
//======================================================================

#pragma once
#include "print/printsys.h"

//�^�X�N���j���[ ���[�N
typedef struct _APP_TASKMENU_WORK APP_TASKMENU_WORK;


//�^�X�N���j���[ ���������[�N �P����
typedef struct
{
  STRBUF  *str;
  PRINTSYS_LSB msgColor;
}APP_TASKMENU_ITEMWORK;

//�^�X�N���j���[ ���������[�N
typedef struct
{
  int heapId;
  
  u8  itemNum;
  APP_TASKMENU_ITEMWORK *itemWork;
  
  //BG�֌W
  u8  bgFrame;
  u8  palNo;    //�p���b�g2�{�g���܂�
  
  //���b�Z�[�W�֌W
  GFL_MSGDATA *msgHandle;
  GFL_FONT    *fontHandle;
  PRINT_QUE   *printQue;
}APP_TASKMENU_INITWORK;


extern APP_TASKMENU_WORK* APP_TASKMENU_OpenMenu( APP_TASKMENU_INITWORK *initWork );
extern void APP_TASKMENU_CloseMenu( APP_TASKMENU_WORK *work );
extern void APP_TASKMENU_UpdateMenu( APP_TASKMENU_WORK *work );
extern const BOOL APP_TASKMENU_IsFinish( APP_TASKMENU_WORK *work );
extern const u8 APP_TASKMENU_GetCursorPos( APP_TASKMENU_WORK *work );

