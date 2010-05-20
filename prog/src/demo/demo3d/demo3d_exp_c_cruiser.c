/*
 *  @file   demo3d_exp_c_cruiser.c
 *  @brief  �N���[�U�[�f���@��O����
 *  @author hosaka,iwasawa
 *  @date   10.02.25
 */
//�K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

//�A�v�����ʑf��
#include "app/app_menu_common.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

//�Ȉ�CLWK�ǂݍ��݁��J�����[�e�B���e�B�[
#include "ui/ui_easy_clwk.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//INFOWIN
#include "infowin/infowin.h"

#include "field/field_light_status.h"

//�`��ݒ�
#include "demo3d_graphic.h"

//�A�[�J�C�u
#include "arc_def.h"

#include "message.naix"
#include "townmap_gra.naix"

#include "demo3d_local.h"
#include "demo3d_graphic.h"
#include "demo3d_engine.h"
#include "demo3d_exp.h"

//-----------------------------------------------------------
// �V���D�����
//-----------------------------------------------------------
#define C_CRUISER_POS_FLASH_SYNC (100) ///< �X�V�Ԋu

//--------------------------------------------------------------
///	���W�e�[�u��
//==============================================================
static const GFL_CLACTPOS g_c_cruiser_postbl[] = 
{
  { 15*8,  17*8, },		//0F
  { 15*8,  17*8-4, },
  { 15*8,  16*8, },
  { 15*8,  16*8-4, },
  { 15*8,  15*8, },
  { 15*8,  15*8-4, },
  { 15*8,  14*8, },
  { 15*8,  14*8-4, },
  { 15*8,  13*8, },
  { 15*8,  13*8-4, },
  { 15*8,  12*8, },		//1000F	�X�J�C�A���[�u���b�W
  { 15*8,  12*8-4, },
  { 15*8,  11*8, },
  { 15*8,  11*8-4, },
  { 15*8,  10*8, },
  { 15*8,  10*8-4, },
  { 15*8,  9*8, },
  { 15*8,  9*8-4, },
  { 15*8,  8*8, },
  { 15*8,  8*8-4, },
  { 15*8,  7*8, },		//2000F
  { 15*8,  7*8-4, },
  { 15*8,  6*8, },
  { 15*8,  6*8-4, },
  { 15*8,  5*8, },		//2400F	�����_�[�u���b�W
  { 15*8+4,  5*8-4, },
  { 16*8+4,  5*8-4, },
  { 17*8,  5*8, },		//2700F	�܂�Ԃ��n�_
  { 17*8,  6*8-4, },
  { 17*8,  6*8, },
  { 17*8,  7*8-4, },		//3000F
  { 17*8,  7*8, },
  { 17*8,  8*8-4, },
  { 17*8,  8*8, },
  { 17*8,  9*8-4, },
  { 17*8,  9*8, },
  { 17*8,  10*8-4, },
  { 17*8,  10*8, },
  { 17*8,  11*8-4, },
  { 17*8,  11*8, },
  { 17*8,  12*8-4, },		//4000F	�X�J�C�A���[�u���b�W
  { 17*8,  12*8, },
  { 17*8,  13*8-4, },
  { 17*8,  13*8, },
  { 17*8,  14*8-2, },
  { 17*8,  14*8+4, },
  { 17*8,  15*8+2, },
  { 17*8,  16*8, },
  { 17*8,  17*8-2, },
  { 17*8,  17*8+4, },
  { 17*8,  18*8+2, },		//5000F
  { 17*8,  19*8, },
  { 17*8,  20*8-2, },
  { 17*8,  21*8-4, },
};

//--------------------------------------------------------------
///	�V���D�p���[�N(��O�����̃��[�U�[���[�N)
//==============================================================
typedef struct {
  u8 pos_id;
  u8 padding[3];
  UI_EASY_CLWK_RES clwk_res;
  GFL_CLWK* clwk_marker;
} EXP_C_CRUISER_WORK;

static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk );
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk );

const APP_EXCEPTION_FUNC_SET DATA_c_exp_funcset_c_cruiser = 
{
  EXP_C_CRUISER_Init,
  EXP_C_CRUISER_Main,
  EXP_C_CRUISER_End,
};

//-----------------------------------------------------------------------------
// �V���D �����
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *  @brief  BG�Ǘ����W���[�� ���\�[�X�ǂݍ���
 *
 *  @param  DEMO3D_BG_WORK* wk BG�Ǘ����[�N
 *  @param  heapID  �q�[�vID 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void _C_CRUISER_LoadBG( HEAPID heapID )
{
  ARCHANDLE *handle;

  handle  = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );

  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_townmap_gra_tmap_ship_NCLR,
      PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0, heapID );
  
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_townmap_gra_tmap_ship_NCGR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID );
  GFL_ARCHDL_UTIL_TransVramScreen(  handle, NARC_townmap_gra_tmap_ship_NSCR,
            BG_FRAME_BACK_S, 0, 0, 0, heapID ); 

  GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D ������
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Init( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk;

  wk->userwork = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(EXP_C_CRUISER_WORK) );
  
  uwk = wk->userwork;

  _C_CRUISER_LoadBG( wk->heapID );

  {
    GFL_CLUNIT* clunit;
    UI_EASY_CLWK_RES_PARAM prm;

    clunit  = DEMO3D_GRAPHIC_GetClunit( wk->graphic );

    prm.draw_type = CLSYS_DRAW_SUB;
    prm.comp_flg  = UI_EASY_CLWK_RES_COMP_NONE;
    prm.arc_id    = ARCID_TOWNMAP_GRAPHIC;
    prm.pltt_id   = NARC_townmap_gra_tmap_ship_obj_NCLR;
    prm.ncg_id    = NARC_townmap_gra_tmap_ship_obj_NCGR;
    prm.cell_id   = NARC_townmap_gra_tmap_ship_obj_NCER;
    prm.anm_id    = NARC_townmap_gra_tmap_ship_obj_NANR;
    prm.pltt_line = PLTID_OBJ_COMMON_S;
    prm.pltt_src_ofs = 0;
    prm.pltt_src_num = 1;

    UI_EASY_CLWK_LoadResource( &uwk->clwk_res, &prm, clunit, wk->heapID );

    uwk->clwk_marker = UI_EASY_CLWK_CreateCLWK( &uwk->clwk_res, clunit, 40, 40, 0, wk->heapID );
      
    GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( uwk->clwk_marker, TRUE );

  }
  
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D �又��
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_Main( APP_EXCEPTION_WORK* wk )
{
  int frame;
  EXP_C_CRUISER_WORK* uwk = wk->userwork;
  
  frame = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT;
  if( uwk->pos_id < NELEMS(g_c_cruiser_postbl) )

  {
    if( uwk->pos_id * C_CRUISER_POS_FLASH_SYNC <= frame )
    {
      // ���W�X�V
      GFL_CLACT_WK_SetPos( uwk->clwk_marker, &g_c_cruiser_postbl[ uwk->pos_id ], CLSYS_DRAW_SUB );
//      GFL_CLACT_WK_SetAnmFrame( uwk->clwk_marker, 0 );

      OS_TPrintf("frame=%d marker pos_id=%d x=%d, y=%d\n", 
          frame, uwk->pos_id, 
          g_c_cruiser_postbl[uwk->pos_id].x,
          g_c_cruiser_postbl[uwk->pos_id].y );

      uwk->pos_id++;

      // �I������
      if( uwk->pos_id == NELEMS(g_c_cruiser_postbl) )
      {
        // ��\����
        GFL_CLACT_WK_SetDrawEnable( uwk->clwk_marker, FALSE );
        OS_TPrintf("marker visible off \n");
      }
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  ��O���� �V���D �������
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void EXP_C_CRUISER_End( APP_EXCEPTION_WORK* wk )
{
  EXP_C_CRUISER_WORK* uwk = wk->userwork;

  // OBJ���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( &uwk->clwk_res );

  // ���[�U�[���[�N���
  GFL_HEAP_FreeMemory( wk->userwork );
}

