//============================================================================
/**
 *  @file   manual.c
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *  ���W���[�����FMANUAL_TOP
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
#include "app/app_taskmenu.h"

#include "manual_graphic.h"
#include "manual_def.h"
#include "manual_common.h"
#include "manual_top.h"

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
// APP_TASKMENU_WIN�̍���
typedef enum
{
  ATM_WIN_ITEM_CATEGORY,
  ATM_WIN_ITEM_INITIAL,
  ATM_WIN_ITEM_MAX,
}
ATM_WIN_ITEM;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
struct _MANUAL_TOP_WORK
{
  // ����
  MANUAL_COMMON_WORK*         cmn_wk;

  // �����ō쐬
  // APP_TASKMENU_WIN
  APP_TASKMENU_RES*           atm_res;
  APP_TASKMENU_WIN_WORK*      atm_win_wk[ATM_WIN_ITEM_MAX];

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Manual_Top_VBlankFunc( GFL_TCB* tcb, void* wk );

// ����
static void Manual_Top_Prepare( MANUAL_TOP_WORK* work );
static void Manual_Top_PrepareAppTaskmenuWin( MANUAL_TOP_WORK* work );
// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Top_Trans( MANUAL_TOP_WORK* work );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
// ����������
MANUAL_TOP_WORK*  MANUAL_TOP_Init(
    MANUAL_COMMON_WORK*  cmn_wk
)
{
  // ���[�N
  MANUAL_TOP_WORK*  work  = GFL_HEAP_AllocClearMemory( cmn_wk->heap_id, sizeof(MANUAL_TOP_WORK) );

  // ����
  work->cmn_wk = cmn_wk;

  // �����ō쐬
  Manual_Top_Prepare( work );
  Manual_Top_Trans( work );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Manual_Top_VBlankFunc, work, 1 );

  return work;
}

// �I������
void  MANUAL_TOP_Exit(
    MANUAL_TOP_WORK*     work
)
{
  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �����ō쐬
  // APP_TASKMENU_WIN
  {
    u8 i;
    // ���[�N
    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      APP_TASKMENU_WIN_Delete( work->atm_win_wk[i] );
    }
    // ���\�[�X
    APP_TASKMENU_RES_Delete( work->atm_res );
  }

  // ���[�N
  GFL_HEAP_FreeMemory( work );
}

// �又��
void  MANUAL_TOP_Main(
    MANUAL_TOP_WORK*     work
)
{
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
static void Manual_Top_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  MANUAL_TOP_WORK* work = (MANUAL_TOP_WORK*)wk;

  // VBlank�]������Ȃ炱���ŁB
  //Manual_Top_Trans( work );
}

// ����
static void Manual_Top_Prepare( MANUAL_TOP_WORK* work )
{
  // �T�uBG
  GFL_ARCHDL_UTIL_TransVramPalette(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCLR,
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_REAR * 0x20,
      BG_PAL_NUM_S_REAR * 0x20,
      work->cmn_wk->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      work->cmn_wk->handle_system,
      NARC_manual_bg_NCGR,
      BG_FRAME_S_REAR,
			0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      work->cmn_wk->handle_system,
      NARC_manual_bg1_NSCR,
      BG_FRAME_S_REAR,
      0,
      0,  // �S�]��
      FALSE,
      work->cmn_wk->heap_id );


  // APP_TASKMENU_WIN
  Manual_Top_PrepareAppTaskmenuWin( work );
}

static void Manual_Top_PrepareAppTaskmenuWin( MANUAL_TOP_WORK* work )
{
  // APP_TASKMENU_WIN
  // ���\�[�X
  {
    work->atm_res = APP_TASKMENU_RES_Create(
        BG_FRAME_S_MAIN,
        BG_PAL_POS_S_ATM,
        work->cmn_wk->font,
        work->cmn_wk->print_que_main,
        work->cmn_wk->heap_id );
  }

  // ���[�N
  {
    u8 i;

    // ���̐ݒ�
    APP_TASKMENU_ITEMWORK item[ATM_WIN_ITEM_MAX];

    item[ATM_WIN_ITEM_CATEGORY].str       = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_01_01 );
    item[ATM_WIN_ITEM_CATEGORY].msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
    item[ATM_WIN_ITEM_CATEGORY].type      = APP_TASKMENU_WIN_TYPE_NORMAL;

    item[ATM_WIN_ITEM_INITIAL].str        = GFL_MSG_CreateString( work->cmn_wk->msgdata_system, manual_01_02 );
    item[ATM_WIN_ITEM_INITIAL].msgColor   = APP_TASKMENU_ITEM_MSGCOLOR;
    item[ATM_WIN_ITEM_INITIAL].type       = APP_TASKMENU_WIN_TYPE_NORMAL;

    for( i=0; i<ATM_WIN_ITEM_MAX; i++ )
    {
      work->atm_win_wk[i] = APP_TASKMENU_WIN_Create(
          work->atm_res,
          &item[i],
          1, 1 + 3*i,
          30,
          work->cmn_wk->heap_id );

      // ��������
      GFL_STR_DeleteBuffer( item[i].str );
    }
  }
}

// �]��(VBlank�]���ł͂Ȃ�)
static void Manual_Top_Trans( MANUAL_TOP_WORK* work )
{
  GFL_BG_LoadScreenReq( BG_FRAME_S_REAR );
}

