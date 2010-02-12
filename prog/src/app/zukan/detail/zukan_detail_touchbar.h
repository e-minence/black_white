//============================================================================
/**
 *  @file   zukan_detail_touchbar.h
 *  @brief  �}�ӏڍ׉�ʋ��ʂ̃^�b�`�o�[
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *
 *  ���W���[�����FZUKAN_DETAIL_TOUCHBAR
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "zukan_detail_def.h"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_detail);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR,
  ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR_TO_APPEAR,
  ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR,
  ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR_TO_DISAPPEAR,
}
ZUKAN_DETAIL_TOUCHBAR_STATE;

typedef enum
{
  ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
  ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP,
  ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM,
  ZUKAN_DETAIL_TOUCHBAR_TYPE_MAX,
}
ZUKAN_DETAIL_TOUCHBAR_TYPE;

typedef enum
{
  ZUKAN_DETAIL_TOUCHBAR_DISP_INFO,
  ZUKAN_DETAIL_TOUCHBAR_DISP_MAP,
  ZUKAN_DETAIL_TOUCHBAR_DISP_VOICE,
  ZUKAN_DETAIL_TOUCHBAR_DISP_FORM,
}
ZUKAN_DETAIL_TOUCHBAR_DISP;

typedef enum
{
  ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE,  // �O�̃y�[�W����(�ւ�)�ړ�
  ZUKAN_DETAIL_TOUCHBAR_SPEED_INSIDE,   // �����ł̃y�[�W�ړ�
  ZUKAN_DETAIL_TOUCHBAR_SPEED_MAX,
}
ZUKAN_DETAIL_TOUCHBAR_SPEED;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _ZUKAN_DETAIL_TOUCHBAR_WORK ZUKAN_DETAIL_TOUCHBAR_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
extern ZUKAN_DETAIL_TOUCHBAR_WORK* ZUKAN_DETAIL_TOUCHBAR_Init( HEAPID heap_id );
extern void ZUKAN_DETAIL_TOUCHBAR_Exit( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
extern void ZUKAN_DETAIL_TOUCHBAR_Main( ZUKAN_DETAIL_TOUCHBAR_WORK* work );

extern void ZUKAN_DETAIL_TOUCHBAR_SetType(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   ZUKAN_DETAIL_TOUCHBAR_TYPE  type, 
                   ZUKAN_DETAIL_TOUCHBAR_DISP  disp );
extern ZUKAN_DETAIL_TOUCHBAR_STATE ZUKAN_DETAIL_TOUCHBAR_GetState( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
extern void ZUKAN_DETAIL_TOUCHBAR_Appear( ZUKAN_DETAIL_TOUCHBAR_WORK* work, ZUKAN_DETAIL_TOUCHBAR_SPEED speed );
extern void ZUKAN_DETAIL_TOUCHBAR_Disappear( ZUKAN_DETAIL_TOUCHBAR_WORK* work, ZUKAN_DETAIL_TOUCHBAR_SPEED speed );

extern ZKNDTL_COMMAND ZUKAN_DETAIL_TOUCHBAR_GetTrg( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
extern void ZUKAN_DETAIL_TOUCHBAR_Unlock( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
