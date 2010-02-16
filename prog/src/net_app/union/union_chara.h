//==============================================================================
/**
 * @file    union_chara.h
 * @brief   ���j�I�����[���ł̐l���֘A�̃w�b�_
 * @author  matsuda
 * @date    2009.07.06(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �萔��`
//==============================================================================
///�r�[�R��PC�F�C�x���g�X�e�[�^�X
enum{
  BPC_EVENT_STATUS_NORMAL,    ///<�������Ă��Ȃ�
  BPC_EVENT_STATUS_ENTER,     ///<���j�I�����[���֐i��
  BPC_EVENT_STATUS_LEAVE,     ///<���j�I�����[������ޏo
  
  BPC_EVENT_STATUS_MAX,
};

///�L�����N�^�C���f�b�N�X���
typedef enum{
  UNION_CHARA_INDEX_PARENT,
  UNION_CHARA_INDEX_CHILD,
}UNION_CHARA_INDEX;

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void UNION_CHAR_Update(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata);
extern UNION_CHARA_INDEX UNION_CHARA_CheckCharaIndex(u16 chara_index);
extern u16 UNION_CHARA_GetCharaIndex_to_ParentNo(u16 chara_index);
extern BOOL UNION_CHARA_CheckCommPlayer(u16 chara_index);

