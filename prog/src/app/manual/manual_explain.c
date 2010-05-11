//============================================================================
/**
 *  @file   manual_explain.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_EXPLAIN
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_touchbar.h"
#include "manual_explain.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_manual.h"
#include "manual.naix"

// �_�~�[
#include "msg/msg_zkn.h"
#include "zukan_gra.naix"


// �T�E���h

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���͏��
typedef enum
{
  INPUT_STATE_NONE,  // ���̓��̓A�j���[�V�������s���Ă��Ȃ��̂ŁA���͉\�B
  INPUT_STATE_END,   // �������̓��͂��󂯕t���Ȃ��B
  INPUT_STATE_TB,    // �^�b�`�o�[�̓��̓A�j���[�V�������s���Ă���Œ��Ȃ̂ŁA���͕s�\�B
}
INPUT_STATE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_EXPLAIN_WORK
{
  // �p�����[�^
  MANUAL_EXPLAIN_PARAM*       param;
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  u8                          page_no;  // 0<= <work->param->page_num

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // ���͂̏��
  INPUT_STATE                 input_state;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_EXPLAIN_WORK*  MANUAL_EXPLAIN_Init(
    MANUAL_EXPLAIN_PARAM*   param,
    MANUAL_COMMON_WORK*     cmn_wk
)
{
  // ���[�N
  MANUAL_EXPLAIN_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_EXPLAIN_WORK) );

  // �p�����[�^
  work->param  = param;
  {
    // out
    work->param->result   = MANUAL_EXPLAIN_RESULT_RETURN;
  }
  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  {
    work->page_no = 3;//0;
  }

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Explain_VBlankFunc, work, 1 );

  // ���͏��
  work->input_state = INPUT_STATE_NONE;

  // �}�j���A���^�b�`�o�[
  MANUAL_TOUCHBAR_SetType( work->cmn_wk->mtb_wk, MANUAL_TOUCHBAR_TYPE_EXPLAIN );
  MANUAL_TOUCHBAR_SetPage( work->cmn_wk->mtb_wk, work->param->page_num, work->page_no );

  return work;
}

// �I������
void  MANUAL_EXPLAIN_Exit(
    MANUAL_EXPLAIN_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  {
  }

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
BOOL  MANUAL_EXPLAIN_Main(
    MANUAL_EXPLAIN_WORK*     work
)
{
  // �}�j���A���^�b�`�o�[
  MANUAL_TOUCHBAR_Main( work->cmn_wk->mtb_wk );

  work->param->result = MANUAL_EXPLAIN_RESULT_RETURN;
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ) return TRUE;
  else return FALSE;
  return TRUE;
}


//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in]       
 *  @param[in,out]       
 *
 *  @retval          
 */
//------------------------------------------------------------------

//-------------------------------------
/// 
//=====================================


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Manual_Explain_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_EXPLAIN_WORK* work = (MANUAL_EXPLAIN_WORK*)wk;
}

