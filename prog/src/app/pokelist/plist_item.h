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
//GS�����`�ڐA
typedef enum
{
  ITEM_TYPE_BTL_ST_UP = 0,  // �퓬�p�X�e�[�^�X�A�b�v�n
  ITEM_TYPE_ALLDETH_RCV,    // �S���m����
  ITEM_TYPE_LV_UP,      // LvUp�n
  ITEM_TYPE_NEMURI_RCV,   // �����
  ITEM_TYPE_DOKU_RCV,     // �ŉ�
  ITEM_TYPE_YAKEDO_RCV,   // �Ώ���
  ITEM_TYPE_KOORI_RCV,    // �X��
  ITEM_TYPE_MAHI_RCV,     // ��჉�
  ITEM_TYPE_KONRAN_RCV,   // ������
  ITEM_TYPE_ALL_ST_RCV,   // �S��
  ITEM_TYPE_MEROMERO_RCV,   // ����������
  ITEM_TYPE_HP_RCV,     // HP��
  ITEM_TYPE_DEATH_RCV,  // �m���� (WB�ǉ�
  
  //���b�Z�[�W�̃X�e�[�^�X�Ə��Ԉˑ��I�I
  ITEM_TYPE_HP_UP,      // HP�w�͒lUP
  ITEM_TYPE_ATC_UP,     // �U���w�͒lUP
  ITEM_TYPE_DEF_UP,     // �h��w�͒lUP
  ITEM_TYPE_AGL_UP,     // �f�����w�͒lUP
  ITEM_TYPE_SPA_UP,     // ���U�w�͒lUP
  ITEM_TYPE_SPD_UP,     // ���h�w�͒lUP

  //���b�Z�[�W�̃X�e�[�^�X�Ə��Ԉˑ��I�I
  ITEM_TYPE_HP_DOWN,      // HP�w�͒lDOWN
  ITEM_TYPE_ATC_DOWN,     // �U���w�͒lDOWN
  ITEM_TYPE_DEF_DOWN,     // �h��w�͒lDOWN
  ITEM_TYPE_AGL_DOWN,     // �f�����w�͒lDOWN
  ITEM_TYPE_SPA_DOWN,     // ���U�w�͒lDOWN
  ITEM_TYPE_SPD_DOWN,     // ���h�w�͒lDOWN

  ITEM_TYPE_EVO,        // �i���n
  ITEM_TYPE_PP_UP,      // ppUp�n
  ITEM_TYPE_PP_3UP,     // pp3Up�n
  ITEM_TYPE_PP_RCV,     // pp�񕜌n
  ITEM_TYPE_ETC,        // ���̑�
}PLIST_ITEM_USE_TYPE;


extern void PLIST_UpdateDispParam( PLIST_WORK *work );

extern const BOOL PLIST_ITEM_IsDeathRecoverAllItem( PLIST_WORK *work , u16 itemNo );
extern const BOOL PLIST_ITEM_IsNeedSelectSkill( PLIST_WORK *work , u16 itemNo );
//-1�őΏۖ�������ȊO�͑Ώ۔ԍ�
extern const int PLIST_ITEM_CanUseDeathRecoverAllItem( PLIST_WORK *work );

extern u32 PLIST_ITEM_GetWazaListMessage( PLIST_WORK *work , u16 itemNo );
extern void PLIST_ITEM_CangeAruseusuForm( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo );

extern void PLIST_ITEM_MSG_CanNotUseItem( PLIST_WORK *work );
extern const PLIST_ITEM_USE_TYPE PLIST_ITEM_MSG_UseItemFunc( PLIST_WORK *work );

extern void PLIST_MSGCB_LvUp_EvoCheck( PLIST_WORK *work );
extern void PLIST_HPANMCB_ReturnRecoverHp( PLIST_WORK *work );
extern void PLIST_HPANMCB_ReturnRecoverAllDeath( PLIST_WORK *work );
extern void PLIST_MSGCB_RecoverAllDeath_NextPoke( PLIST_WORK *work );
