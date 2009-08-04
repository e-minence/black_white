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
//�㔼�͌����̖߂�l�ɂ��g��
typedef enum
{
	
  PMIT_STATSU,  //����������
  PMIT_HIDEN,   //��`�Z�B�����Ō����ǉ����܂�
  PMIT_CHANGE,  //����ւ�
  PMIT_ITEM,    //������
  PMIT_CLOSE,   //����
  PMIT_LEAVE,   //�a����(��Ă�
  PMIT_SET_JOIN,    //�Q������(�Q�����Ȃ�)

  PMIT_GIVE,    //��������
  PMIT_TAKE,    //�a����
  
  PMIT_YES,    //�͂�
  PMIT_NO,     //������

  PMIT_END_LIST,    //���X�g�I�[�p
  
  //�ȉ��߂�l��p
  PMIT_WAZA_1 = PMIT_END_LIST,   //��`�p�Z�X���b�g�P
  PMIT_WAZA_2,   //��`�p�Z�X���b�g�Q
  PMIT_WAZA_3,   //��`�p�Z�X���b�g�R
  PMIT_WAZA_4,   //��`�p�Z�X���b�g�S
  
  PMIT_RET_JOIN,      //�Q������
  PMIT_JOIN_CANCEL,   //�Q�����Ȃ�
  
  PMIT_MAX,
  
  PMIT_NONE,    //�I��
  
}PLIST_MENU_ITEM_TYPE;

extern PLIST_MENU_WORK* PLIST_MENU_CreateSystem( PLIST_WORK *work );
extern void PLIST_MENU_DeleteSystem( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );

extern void PLIST_MENU_OpenMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr );
extern void PLIST_MENU_OpenMenu_YesNo( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
extern void PLIST_MENU_CloseMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
extern void PLIST_MENU_UpdateMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );

extern const PLIST_MENU_ITEM_TYPE PLIST_MENU_IsFinish( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
//�o�g���Q�����莞���ɏo����
extern GFL_BMPWIN* PLIST_MENU_CreateMenuWin_BattleMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , u32 strId , u8 charX , u8 charY , const BOOL isReturn );
