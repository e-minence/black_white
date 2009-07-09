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
  BPC_EVENT_STATUS_NULL,      ///<�������Ă��Ȃ�
  BPC_EVENT_STATUS_ENTER,     ///<���j�I�����[���֐i��
  BPC_EVENT_STATUS_LEAVE,     ///<���j�I�����[������ޏo
  
  BPC_EVENT_STATUS_MAX,
};

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern MMDL * UNION_CHAR_AddOBJ(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata, u8 trainer_view, u16 char_index);
extern void UNION_CHAR_Update(UNION_SYSTEM_PTR unisys, GAMEDATA *gdata);
extern BOOL UNION_CHAR_EventReq(UNION_BEACON_PC *bpc, int event_status);

