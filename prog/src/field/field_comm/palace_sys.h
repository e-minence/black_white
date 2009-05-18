//==============================================================================
/**
 * @file    palace_sys.h
 * @brief   �t�B�[���h�ʐM�F�p���X�̃w�b�_
 * @author  matsuda
 * @date    2009.05.01(��)
 */
//==============================================================================
#pragma once

//==============================================================================
//  �^��`
//==============================================================================
///�p���X�V�X�e�����[�N�\���̂ւ̕s��`�|�C���^
typedef struct _PALACE_SYS_WORK * PALACE_SYS_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern PALACE_SYS_PTR PALACE_SYS_Alloc(HEAPID heap_id);
extern void PALACE_SYS_InitWork(PALACE_SYS_PTR palace);
extern void PALACE_SYS_Free(PALACE_SYS_PTR palace);
extern void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply);
extern void PALACE_SYS_SetArea(PALACE_SYS_PTR palace, u8 palace_area);
extern u8 PALACE_SYS_GetArea(PALACE_SYS_PTR palace);
extern void PALACE_SYS_FriendPosConvert(PALACE_SYS_PTR palace, int friend_area, 
  PLAYER_WORK *my_plwork, PLAYER_WORK *friend_plwork);

//--------------------------------------------------------------
//  �f�o�b�O�p
//--------------------------------------------------------------
extern void PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id);
extern void PALACE_DEBUG_DeleteNumberAct(PALACE_SYS_PTR palace);
