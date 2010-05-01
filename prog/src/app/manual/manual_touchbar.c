//============================================================================
/**
 *  @file   manual.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_TOUCHBAR
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
FS_EXTERN_OVERLAY(ui_common);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_TOUCHBAR_WORK
{
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  TOUCHBAR_WORK*              tb;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����
static void Manual_Touchbar_Prepare( MANUAL_TOUCHBAR_WORK* work );
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Touchbar_Trans( MANUAL_TOUCHBAR_WORK* work );
static void Manual_Touchbar_TransVBlank( MANUAL_TOUCHBAR_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_TOUCHBAR_WORK*  MANUAL_TOUCHBAR_Init(
    MANUAL_COMMON_WORK*  cmn_wk
)
{
  // ���[�N
  MANUAL_TOUCHBAR_WORK*  work;

  // �I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

  // ���[�N
  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_TOUCHBAR_WORK) );

  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  Manual_Touchbar_Prepare( work );
  Manual_Touchbar_Trans( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Touchbar_VBlankFunc, work, 1 );

  return work;
}

// �I������
void  MANUAL_TOUCHBAR_Exit(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  // �^�b�`�o�[�j��
  TOUCHBAR_Exit( work->tb );

  // ���[�N
  GFL_HEAP_FreeMemory( work );

  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );
}

// �又��
void  MANUAL_TOUCHBAR_Main(
    MANUAL_TOUCHBAR_WORK*     work
)
{
  // �^�b�`�o�[
  TOUCHBAR_Main( work->tb );
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
static void Manual_Touchbar_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_TOUCHBAR_WORK* work = (MANUAL_TOUCHBAR_WORK*)wk;

  // VBlank�]������Ȃ炱���ŁB
  //Manual_Touchbar_Trans( work );
}

// ����
static void Manual_Touchbar_Prepare( MANUAL_TOUCHBAR_WORK* work )
{
  // �^�b�`�o�[�̐ݒ�
  {
    // �A�C�R���̐ݒ�
    // �������
    TOUCHBAR_SETUP      tb_setup;

    TOUCHBAR_ITEM_ICON  tb_item_icon_tbl[] =
    {
      {
        TOUCHBAR_ICON_RETURN,
        { TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
      },
      {
        TOUCHBAR_ICON_CLOSE,
        { TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
      },
      {
        TOUCHBAR_ICON_CUR_R,
        { TOUCHBAR_ICON_X_02, TOUCHBAR_ICON_Y },
      },
      {
        TOUCHBAR_ICON_CUR_L,
        { TOUCHBAR_ICON_X_00, TOUCHBAR_ICON_Y },
      },
    };

    // �ݒ�\����
    // �����قǂ̑����{���\�[�X���������
    GFL_STD_MemClear( &tb_setup, sizeof(TOUCHBAR_SETUP) );
    tb_setup.p_item     = tb_item_icon_tbl;            //��̑����
    tb_setup.item_num   = NELEMS(tb_item_icon_tbl);    //�����������邩
    tb_setup.p_unit     = MANUAL_GRAPHIC_GetClunit( work->cmn_wk->graphic );  //OBJ�ǂݍ��݂̂��߂�CLUNIT
    tb_setup.bar_frm    = BG_FRAME_S_TB_BAR;           //BG�ǂݍ��݂̂��߂�BG��
    tb_setup.bg_plt     = BG_PAL_POS_S_TB_BAR;         //BG��گ�
    tb_setup.obj_plt    = OBJ_PAL_POS_S_TB_ICON;       //OBJ��گ�
    tb_setup.mapping    = APP_COMMON_MAPPING_32K;      //�}�b�s���O���[�h

    work->tb = TOUCHBAR_Init( &tb_setup, work->cmn_wk->heap_id );
  }
}

// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Touchbar_Trans( MANUAL_TOUCHBAR_WORK* work )
{
}

