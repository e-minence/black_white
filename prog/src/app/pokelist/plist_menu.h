//======================================================================
/**
 * @file	plist_menu.c
 * @brief	�|�P�������X�g ���j���[
 * @author	ariizumi
 * @data	09/06/18
 *
 * ���W���[�����FPLIST_MENU
 */
//======================================================================
#pragma once

#include "plist_local_def.h"

//���j���[������������ۂɓn���A�C�e���Q
//�ォ����ׂ�
typedef enum
{
  PMIT_STATSU,  //����������
  PMIT_HIDEN,   //��`�Z�B�����Ō����ǉ����܂�
  PMIT_CHANGE,  //����ւ�
  PMIT_ITEM,    //������
  PMIT_CLOSE,   //����
  PMIT_LEAVE,   //�a����(��Ă�
  PMIT_JOIN,    //�Q������E�Q�����Ȃ�

  PMIT_END_LIST,    //���X�g�I�[�p
  
}PLIST_MENU_ITEM_TYPE;

extern PLIST_MENU_WORK* PLIST_MENU_CreateSystem( PLIST_WORK *work );
extern void PLIST_MENU_DeleteSystem( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );

extern void PLIST_MENU_OpenMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr );
extern void PLIST_MENU_CloseMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
void PLIST_MENU_UpdateMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );

