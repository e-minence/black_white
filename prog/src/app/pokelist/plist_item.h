//======================================================================
/**
 * @file	plist_item.c
 * @brief	�|�P�������X�g �A�C�e���`�F�b�N�n
 * @author	ariizumi
 * @data	09/08/05
 *
 * ���W���[�����FPLIST_ITEM
 */
//======================================================================

#pragma once

#include "poke_tool\poke_tool.h"
#include "plist_local_def.h"

//�A�C�e�����g���邩�̃`�F�b�N
typedef enum
{
  PIUC_OK,  //�g����
  PIUC_NG,  //�g���Ȃ�
  PIUC_SELECT_SKILL,  //�X�L���I����
}PLIST_ITEM_USE_CHECK;

const BOOL PLIST_ITEM_IsDeathRecoverAllItem( PLIST_WORK *work , u16 itemNo );
const PLIST_ITEM_USE_CHECK PLIST_ITEM_CanUseRecoverItem( PLIST_WORK *work , u16 itemNo , POKEMON_PARAM *pp );

void PLIST_ITEM_UseRecoverItem( PLIST_WORK *work );
void PLIST_ITEM_UseAllDeathRecoverItem( PLIST_WORK *work );

void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work );
