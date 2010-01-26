//==============================================================================
/**
 * @file    union_subdisp.h
 * @brief   ���j�I������ʂ̃w�b�_
 * @author  matsuda
 * @date    2009.07.27(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
///�I�����Ă���A�s�[���ԍ�
enum{
  UNISUB_APPEAL_NO_BATTLE,
  UNISUB_APPEAL_NO_TRADE,
  UNISUB_APPEAL_NO_RECORD,
  UNISUB_APPEAL_NO_PICTURE,
  UNISUB_APPEAL_NO_GURUGURU,

  UNISUB_APPEAL_NO_NONE = 0xff,
};

//==============================================================================
//  �^��`
//==============================================================================
typedef struct _UNION_SUBDISP * UNION_SUBDISP_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern UNION_SUBDISP_PTR UNION_SUBDISP_Init(GAMESYS_WORK *gsys);
extern void UNION_SUBDISP_Exit(UNION_SUBDISP_PTR unisub);
extern void UNION_SUBDISP_Update(UNION_SUBDISP_PTR unisub, BOOL bActive);
extern void UNION_SUBDISP_Draw(UNION_SUBDISP_PTR unisub, BOOL bActive);

