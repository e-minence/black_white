//==============================================================================
/**
 * @file    union_eff.h
 * @brief   ���j�I�����[���F�G�t�F�N�g�ނ̃w�b�_
 * @author  matsuda
 * @date    2010.01.17(��)
 */
//==============================================================================
#pragma once

#include "field/fldmmdl.h"


//==============================================================================
//  �^��`
//==============================================================================
///���j�I���G�t�F�N�g����V�X�e���\����
typedef struct _UNION_EFF_SYSTEM UNION_EFF_SYSTEM;

///�F�B�}�[�N�̎��
typedef enum{
  FRIENDMARK_TYPE_FRIEND,         ///<�F�B
  FRIENDMARK_TYPE_ACQUAINTANCE,   ///<�m�荇��
}FRIENDMARK_TYPE;


//==============================================================================
//  �O���֐��錾
//==============================================================================
//--------------------------------------------------------------
//  �V�X�e���n
//--------------------------------------------------------------
extern UNION_EFF_SYSTEM * UNION_EFF_SystemSetup(HEAPID heap_id);
extern void UNION_EFF_SystemExit(UNION_EFF_SYSTEM *unieff);
extern void UNION_EFF_SystemUpdate(UNION_EFF_SYSTEM *unieff);
extern void UNION_EFF_SystemDraw(UNION_EFF_SYSTEM *unieff);

//--------------------------------------------------------------
//  �A�v���n
//--------------------------------------------------------------
extern void UnionEff_App_ReqFocus(UNION_EFF_SYSTEM *unieff, MMDL *mmdl);
extern void UnionEff_App_ReqFriendMark(UNION_EFF_SYSTEM *unieff, MMDL *mmdl, FRIENDMARK_TYPE mark_type);
