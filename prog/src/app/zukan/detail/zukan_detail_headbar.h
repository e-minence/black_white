//============================================================================
/**
 *  @file   zukan_detail_headbar.h
 *  @brief  �}�ӏڍ׉�ʋ��ʂ̃^�C�g���o�[
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *
 *  ���W���[�����FZUKAN_DETAIL_HEADBAR
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_detail);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR,
  ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR_TO_APPEAR,
  ZUKAN_DETAIL_HEADBAR_STATE_APPEAR,
  ZUKAN_DETAIL_HEADBAR_STATE_APPEAR_TO_DISAPPEAR,
}
ZUKAN_DETAIL_HEADBAR_STATE;

typedef enum
{
  ZUKAN_DETAIL_HEADBAR_TYPE_INFO,
  ZUKAN_DETAIL_HEADBAR_TYPE_MAP,
  ZUKAN_DETAIL_HEADBAR_TYPE_VOICE,
  ZUKAN_DETAIL_HEADBAR_TYPE_FORM,
  ZUKAN_DETAIL_HEADBAR_TYPE_FORM_COMPARE,
  ZUKAN_DETAIL_HEADBAR_TYPE_MAX,

  ZUKAN_DETAIL_HEADBAR_TYPE_NONE   = ZUKAN_DETAIL_HEADBAR_TYPE_MAX,
}
ZUKAN_DETAIL_HEADBAR_TYPE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _ZUKAN_DETAIL_HEADBAR_WORK ZUKAN_DETAIL_HEADBAR_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
extern ZUKAN_DETAIL_HEADBAR_WORK* ZUKAN_DETAIL_HEADBAR_Init(
                                      HEAPID      heap_id,
                                      GFL_FONT*   font );
extern void ZUKAN_DETAIL_HEADBAR_Exit( ZUKAN_DETAIL_HEADBAR_WORK* work );
extern void ZUKAN_DETAIL_HEADBAR_Main( ZUKAN_DETAIL_HEADBAR_WORK* work );

extern void ZUKAN_DETAIL_HEADBAR_SetType(
                   ZUKAN_DETAIL_HEADBAR_WORK* work,
                   ZUKAN_DETAIL_HEADBAR_TYPE type );
extern ZUKAN_DETAIL_HEADBAR_STATE ZUKAN_DETAIL_HEADBAR_GetState( ZUKAN_DETAIL_HEADBAR_WORK* work );
extern void ZUKAN_DETAIL_HEADBAR_Appear( ZUKAN_DETAIL_HEADBAR_WORK* work );
extern void ZUKAN_DETAIL_HEADBAR_Disappear( ZUKAN_DETAIL_HEADBAR_WORK* work );

