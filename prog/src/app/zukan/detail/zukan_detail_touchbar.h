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

  ZUKAN_DETAIL_TOUCHBAR_DISP_CURR_ICON_NONE,  // ���A�C�R���Ȃ�
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
extern ZUKAN_DETAIL_TOUCHBAR_WORK* ZUKAN_DETAIL_TOUCHBAR_Init( HEAPID heap_id, BOOL form_version );
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
extern ZKNDTL_COMMAND ZUKAN_DETAIL_TOUCHBAR_GetTouch( ZUKAN_DETAIL_TOUCHBAR_WORK* work );
extern void ZUKAN_DETAIL_TOUCHBAR_Unlock( ZUKAN_DETAIL_TOUCHBAR_WORK* work );

extern void ZUKAN_DETAIL_TOUCHBAR_SetVisibleAll(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   BOOL                        is_visible );

extern void ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   ZUKAN_DETAIL_TOUCHBAR_DISP  disp );

extern void ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   BOOL                        is_visible );

extern void ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work,
                   BOOL                        is_flip );
extern BOOL ZUKAN_DETAIL_TOUCHBAR_GetCheckFlipOfGeneral(
                   ZUKAN_DETAIL_TOUCHBAR_WORK* work );

// �����ڂ��A�N�e�B�u��Ԃɂ����܂܁A���[�U�w��̑S�̐�p�̃A�N�e�B�u�t���O�̐؂�ւ����s��
extern void ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( ZUKAN_DETAIL_TOUCHBAR_WORK* work, BOOL is_active );  // TRUE�̂Ƃ��A�N�e�B�u  // �؂�ւ�����Ƃ��̃f�t�H���g��TRUE

// �J�X�^���{�^���̃p���b�g��ǂݍ��񂾏ꏊ�𓾂�
extern u32 ZUKAN_DETAIL_TOUCHBAR_GetCustomIconPlttRegIdx( ZUKAN_DETAIL_TOUCHBAR_WORK* work );

