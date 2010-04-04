//============================================================================
/**
 *  @file   zukan_toroku.c
 *  @brief  �}�ӓo�^
 *  @author Koji Kawada
 *  @data   2009.12.03
 *  @note   ui_template.c�����ɍ쐬���܂����B
 *
 *  ���W���[�����FZUKAN_TOROKU
 */
//============================================================================
// �K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// �C���N���[�h
#include "print/gf_font.h"
#include "font/font.naix"

#include "print/printsys.h"

// zukan
#include "../info/zukan_info.h"
#include "zukan_nickname.h"

#include "zukan_toroku_graphic.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "zukan_gra.naix"

// zukan_toroku
#include "app/zukan_toroku.h"

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define ZUKAN_TOROKU_HEAP_SIZE (0x80000)  ///< �q�[�v�T�C�Y

#define ZUKAN_TOROKU_FADE_IN_WAIT  (2)  ///< �t�F�[�h�C���̃X�s�[�h
#define ZUKAN_TOROKU_FADE_OUT_WAIT (2)  ///< �t�F�[�h�A�E�g�̃X�s�[�h

#define ZUKAN_TOROKU_INFO_BG_PRIORITY (0)  ///< �}�ӏ���ʂ̍őO�ʂ̃v���C�I���e�B(0��1) 

#define ZUKAN_TOROKU_VBLANK_TCB_PRI (1)

//-------------------------------------
/// �X�e�[�g 
//=====================================
typedef enum
{
  ZUKAN_TOROKU_STATE_TOROKU,
  ZUKAN_TOROKU_STATE_NICKNAME,
}
ZUKAN_TOROKU_STATE;

//-------------------------------------
/// TOROKU�X�e�b�v
//=====================================
typedef enum
{
  ZUKAN_TOROKU_TOROKU_STEP_FADE_IN,
  ZUKAN_TOROKU_TOROKU_STEP_WAIT_KEY,
  ZUKAN_TOROKU_TOROKU_STEP_MOVE,
  ZUKAN_TOROKU_TOROKU_STEP_FADE_OUT_BG,
  ZUKAN_TOROKU_TOROKU_STEP_FADE_IN_BG,
  ZUKAN_TOROKU_TOROKU_STEP_CENTER_STILL,
}
ZUKAN_TOROKU_TOROKU_STEP;

//-------------------------------------
/// NICKNAME�X�e�b�v
//=====================================
typedef enum
{
  ZUKAN_TOROKU_NICKNAME_STEP_FADE_IN,
  ZUKAN_TOROKU_NICKNAME_STEP_START,
  ZUKAN_TOROKU_NICKNAME_STEP_SELECT,
  ZUKAN_TOROKU_NICKNAME_STEP_FADE_OUT,
  ZUKAN_TOROKU_NICKNAME_STEP_END,
}
ZUKAN_TOROKU_NICKNAME_STEP;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  HEAPID                          heap_id;
  ZUKAN_TOROKU_GRAPHIC_WORK*      graphic;
  GFL_FONT*                       font;
  PRINT_QUE*                      print_que;

  ZUKAN_TOROKU_STATE              state;
  s32                             step;
  s32                             brightness;
  s32                             brightness_wait;

  ZUKAN_INFO_WORK*                zukan_info_work;
  ZUKAN_NICKNAME_WORK*            zukan_nickname_work;
  
  GFL_TCB*                        vblank_tcb;  ///< VBlank����TCB
  BOOL                            bg_visible_off;
  
  GFL_ARCUTIL_TRANSINFO           bg_chara_info;  ///< BG�L�����̈�
}
ZUKAN_TOROKU_WORK;

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void Zukan_Toroku_CreateBG( ZUKAN_TOROKU_WORK* work );
static void Zukan_Toroku_DeleteBG( ZUKAN_TOROKU_WORK* work );
static void Zukan_Toroku_VBlankFunc( GFL_TCB* tcb, void* wk );

//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA ZUKAN_TOROKU_ProcData =
{
  Zukan_Toroku_ProcInit,
  Zukan_Toroku_ProcMain,
  Zukan_Toroku_ProcExit,
};

//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     PROC�ɓn�������𐶐�����
 *
 *  @param[in] heap_id         �q�[�vID
 *  @param[in] launch          �N�����@
 *  @param[in] pp              �|�P�����p�����[�^
 *  @param[in] b_zenkoku_flag  TRUE�̂Ƃ��S���}��No�ŕ\������
 *  @param[in] bos_strbuf      !=NULL�̂Ƃ��A�{�b�N�X�ɓ]�����܂����Ƃ������b�Z�[�W��\������
 *  @param[in] box_manager     box_strbuf!=NULL�̂Ƃ��̂ݗL��
 *  @param[in] box_tray        box_strbuf!=NULL�̂Ƃ��̂ݗL��
 *
 *  @retval    �������m�ۂ�����������ZUKAN_TOROKU_PARAM
 */
//-----------------------------------------------------------------------------
ZUKAN_TOROKU_PARAM* ZUKAN_TOROKU_AllocParam( HEAPID              heap_id,
                                             ZUKAN_TOROKU_LAUNCH launch,
                                             POKEMON_PARAM*      pp,
                                             BOOL                b_zenkoku_flag,
                                             const STRBUF*       box_strbuf,
                                             const BOX_MANAGER*  box_manager,
                                             u32 box_tray )
{
  ZUKAN_TOROKU_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof(ZUKAN_TOROKU_PARAM) );
  GFL_STD_MemClear( param, sizeof(ZUKAN_TOROKU_PARAM) );

  ZUKAN_TOROKU_SetParam( param,
                         launch,
                         pp,
                         b_zenkoku_flag,
                         box_strbuf,
                         box_manager,
                         box_tray );
  
  return param;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ZUKAN_TOROKU_AllocParam�Ő�������PROC�ɓn��������j������
 *
 *  @param[in,out] param ZUKAN_TOROKU_AllocParam�̖߂�l
 *
 *  @retval        �Ȃ�
 */
//-----------------------------------------------------------------------------
void ZUKAN_TOROKU_FreeParam( ZUKAN_TOROKU_PARAM* param )
{
  GFL_HEAP_FreeMemory( param );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         PROC�ɓn�������̐ݒ�
 *
 *  @param[in,out] param           ������������ZUKAN_TOROKU_PARAM
 *  @param[in]     launch          �N�����@
 *  @param[in]     pp              �|�P�����p�����[�^
 *  @param[in]     b_zenkoku_flag  TRUE�̂Ƃ��S���}��No�ŕ\������
 *  @param[in]     bos_strbuf      !=NULL�̂Ƃ��A�{�b�N�X�ɓ]�����܂����Ƃ������b�Z�[�W��\������
 *  @param[in]     box_manager     box_strbuf!=NULL�̂Ƃ��̂ݗL��
 *  @param[in]     box_tray        box_strbuf!=NULL�̂Ƃ��̂ݗL��
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void ZUKAN_TOROKU_SetParam( ZUKAN_TOROKU_PARAM* param,
                            ZUKAN_TOROKU_LAUNCH launch,
                            POKEMON_PARAM*      pp,
                            BOOL                b_zenkoku_flag,
                            const STRBUF*       box_strbuf,
                            const BOX_MANAGER*  box_manager,
                            u32 box_tray )
{
  param->launch          = launch;
  param->pp              = pp;
  param->b_zenkoku_flag  = b_zenkoku_flag;
  param->box_strbuf      = box_strbuf;
  param->box_manager     = box_manager;
  param->box_tray        = box_tray;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ���ʂ𓾂�
 *
 *  @param[in,out] param ZUKAN_TOROKU_PARAM
 *
 *  @retval        ���� 
 */
//-----------------------------------------------------------------------------
ZUKAN_TOROKU_RESULT ZUKAN_TOROKU_GetResult( ZUKAN_TOROKU_PARAM* param )
{
  return param->result; 
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Zukan_Toroku_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work;

  // �I�[�o�[���C�ǂݍ���
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(zukan_info) );

  // �q�[�v
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_TOROKU, ZUKAN_TOROKU_HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_TOROKU_WORK), HEAPID_ZUKAN_TOROKU );
    GFL_STD_MemClear( work, sizeof(ZUKAN_TOROKU_WORK) );
  }

  // ���� 
  {
    work->heap_id = HEAPID_ZUKAN_TOROKU;
    work->graphic = ZUKAN_TOROKU_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que = PRINTSYS_QUE_Create( work->heap_id );
  }

  // �N�����@����X�e�[�g�A�X�e�b�v�����߂�
  {
    if(param->launch==ZUKAN_TOROKU_LAUNCH_TOROKU)
    {
      work->state = ZUKAN_TOROKU_STATE_TOROKU;
      work->step = ZUKAN_TOROKU_TOROKU_STEP_FADE_IN;
    }
    else
    {
      work->state = ZUKAN_TOROKU_STATE_NICKNAME;
      work->step = ZUKAN_TOROKU_NICKNAME_STEP_FADE_IN;
    }
  }

  // �ʐM�A�C�R��
  //GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );  // ���̈ʒu�ł͂��܂��\������Ȃ������̂ŁAZUKAN_NICKNAME_Init��
  //GFL_NET_ReloadIcon();                                      // ����ʂ𐶐�������ɍs���悤�ɂ����B

  // ����
  work->zukan_info_work = ZUKAN_INFO_Init( work->heap_id, param->pp, param->b_zenkoku_flag, TRUE,
                                           (param->launch==ZUKAN_TOROKU_LAUNCH_TOROKU)?(ZUKAN_INFO_LAUNCH_TOROKU):(ZUKAN_INFO_LAUNCH_NICKNAME),
                                           ZUKAN_INFO_DISP_M, 0,
                                           ZUKAN_TOROKU_GRAPHIC_GetClunit(work->graphic),
                                           work->font,
                                           work->print_que );

  // ���ʁ������
  work->zukan_nickname_work = ZUKAN_NICKNAME_Init( work->heap_id, param->pp,
                                                   param->box_strbuf, param->box_manager, param->box_tray,
                                                   ZUKAN_TOROKU_GRAPHIC_GetClunit(work->graphic),
                                                   work->font,
                                                   work->print_que );
  
  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, work->heap_id );
  GFL_NET_ReloadIcon();
  
  // ����
  if( work->state == ZUKAN_TOROKU_STATE_NICKNAME )
  {
    Zukan_Toroku_CreateBG( work );
  }

  // VBlank��TBC
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Toroku_VBlankFunc, work, ZUKAN_TOROKU_VBLANK_TCB_PRI );

  // �t�F�[�h�C��(����������)
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, ZUKAN_TOROKU_FADE_IN_WAIT );

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Zukan_Toroku_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work = (ZUKAN_TOROKU_WORK*)mywk;

  // VBlank��TBC
  GFL_TCB_DeleteTask( work->vblank_tcb );
 
  // ����
  Zukan_Toroku_DeleteBG( work );
 
  // ���ʁ������
  ZUKAN_NICKNAME_Exit( work->zukan_nickname_work );
  // ����
  ZUKAN_INFO_Exit( work->zukan_info_work );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

  // ��Еt��
  {
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    ZUKAN_TOROKU_GRAPHIC_Exit( work->graphic );
  }

  // �q�[�v
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_TOROKU );
  }
  
  // �I�[�o�[���C�j��
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(zukan_info) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

  return GFL_PROC_RES_FINISH;
} 

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Zukan_Toroku_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  ZUKAN_TOROKU_PARAM* param = (ZUKAN_TOROKU_PARAM*)pwk;
  ZUKAN_TOROKU_WORK* work = (ZUKAN_TOROKU_WORK*)mywk;

  GFL_PROC_RESULT proc_res = GFL_PROC_RES_CONTINUE;
  
  ZUKAN_NICKNAME_RESULT zukan_nickname_result;

  ZUKAN_INFO_Main( work->zukan_info_work );
  zukan_nickname_result = ZUKAN_NICKNAME_Main( work->zukan_nickname_work );

  // �X�e�[�g����
  switch(work->state)
  {
  // TOROKU�X�e�[�g
  case ZUKAN_TOROKU_STATE_TOROKU:
    {
      // �X�e�b�v����
      switch(work->step)
      {
      case ZUKAN_TOROKU_TOROKU_STEP_FADE_IN:
        {
          // �t�F�[�h�C����
          if( !GFL_FADE_CheckFade() )
          {
            ZUKAN_INFO_Start( work->zukan_info_work );
            work->step = ZUKAN_TOROKU_TOROKU_STEP_WAIT_KEY;
          }
        }
        break;
      case ZUKAN_TOROKU_TOROKU_STEP_WAIT_KEY:
        {
          // �L�[���͑҂�
          BOOL next = FALSE;
          BOOL tp_trg = GFL_UI_TP_GetTrg();
          int trg = GFL_UI_KEY_GetTrg();
          if( trg & ( PAD_BUTTON_A | PAD_BUTTON_B ) )
          {
            GFL_UI_SetTouchOrKey( GFL_APP_KTST_KEY );
            next = TRUE;
          }
          if( tp_trg )
          {
            GFL_UI_SetTouchOrKey( GFL_APP_KTST_TOUCH );
            next = TRUE;
          }

          if( next )
          {
            ZUKAN_INFO_Move( work->zukan_info_work );
            work->step = ZUKAN_TOROKU_TOROKU_STEP_MOVE;
          }
        }
        break;
      case ZUKAN_TOROKU_TOROKU_STEP_MOVE:
        {
          // �|�P�����������ֈړ����n�߂�̂�҂�
          if( ZUKAN_INFO_IsMove(work->zukan_info_work) || ZUKAN_INFO_IsCenterStill(work->zukan_info_work) )  // IsCenterStill�͔O�̂���
          {
            work->brightness_wait = 0;
            work->brightness = 16;
            work->step = ZUKAN_TOROKU_TOROKU_STEP_FADE_OUT_BG;
          }
        }
        break;
      case ZUKAN_TOROKU_TOROKU_STEP_FADE_OUT_BG:
        {
          // �|�P�����̔w�i���Â�����
          work->brightness_wait++;
          if( work->brightness_wait == 4 )
          {
            work->brightness_wait = 0;
            work->brightness--;
          }
          G2_SetBlendBrightnessExt( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3,
                                    GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3,
                                    work->brightness,
                                    0,
                                    work->brightness - 16 );
          if( work->brightness == 0 )
          {
            ZUKAN_INFO_DeleteOthers( work->zukan_info_work );
            Zukan_Toroku_CreateBG( work );
            work->step = ZUKAN_TOROKU_TOROKU_STEP_FADE_IN_BG;
          }
        }
        break;
      case ZUKAN_TOROKU_TOROKU_STEP_FADE_IN_BG:
        {
          // �|�P�����̔w�i�𖾂邭����
          work->brightness_wait++;
          if( work->brightness_wait == 4 )
          {
            work->brightness_wait = 0;
            work->brightness++;
          }
          G2_SetBlendBrightnessExt( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3,
                                    GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3,
                                    work->brightness,
                                    0,
                                    work->brightness - 16 );
          if( work->brightness == 16 )
          {
            work->step = ZUKAN_TOROKU_TOROKU_STEP_CENTER_STILL;
          }
        }
        break;
      case ZUKAN_TOROKU_TOROKU_STEP_CENTER_STILL:
        {
          // �|�P�����������Ɉړ���
          if( ZUKAN_INFO_IsCenterStill(work->zukan_info_work) )
          {
            // NICKNAME�X�e�[�g��
            work->state = ZUKAN_TOROKU_STATE_NICKNAME;
            work->step = ZUKAN_TOROKU_NICKNAME_STEP_START;
          }
        }
        break;
      }
    }
    break;
  // NICKNAME�X�e�[�g
  case ZUKAN_TOROKU_STATE_NICKNAME:
    {
      // �X�e�b�v����
      switch(work->step)
      {
      case ZUKAN_TOROKU_NICKNAME_STEP_FADE_IN:
        {
          // �t�F�[�h�C����
          if( !GFL_FADE_CheckFade() )
          {
            ZUKAN_INFO_Cry( work->zukan_info_work );
            work->step = ZUKAN_TOROKU_NICKNAME_STEP_START;
          }
        }
        break;
      case ZUKAN_TOROKU_NICKNAME_STEP_START:
        {
          // �j�b�N�l�[���𖽖����邩���������̊J�n
          ZUKAN_NICKNAME_Start( work->zukan_nickname_work );
          work->step = ZUKAN_TOROKU_NICKNAME_STEP_SELECT;
        }
        break;
      case ZUKAN_TOROKU_NICKNAME_STEP_SELECT:
        {
          // �I��
          if( zukan_nickname_result == ZUKAN_NICKNAME_RES_FINISH )
          {
            // �t�F�[�h�A�E�g(�����遨��)
            GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, ZUKAN_TOROKU_FADE_OUT_WAIT );
            work->step = ZUKAN_TOROKU_NICKNAME_STEP_FADE_OUT;
          }
        }
        break;
      case ZUKAN_TOROKU_NICKNAME_STEP_FADE_OUT:
        {
          // �t�F�[�h�A�E�g��
          if( !GFL_FADE_CheckFade() )
          {
            ZUKAN_NICKNAME_SELECT zukan_nickname_select = ZUKAN_NICKNAME_GetSelect( work->zukan_nickname_work );
            switch( zukan_nickname_select )
            {
            case ZUKAN_NICKNAME_SELECT_NO:
              param->result = ZUKAN_TOROKU_RESULT_END;
              break;
            case ZUKAN_NICKNAME_SELECT_YES:
              param->result = ZUKAN_TOROKU_RESULT_NICKNAME;
              break;
            }
            work->step = ZUKAN_TOROKU_NICKNAME_STEP_END;
            proc_res = GFL_PROC_RES_FINISH;
          }
        }
        break;
      }
    }
    break;
  }

  PRINTSYS_QUE_Main( work->print_que );

  // 2D�`��
  ZUKAN_TOROKU_GRAPHIC_2D_Draw( work->graphic );
  // 3D�`��
  ZUKAN_TOROKU_GRAPHIC_3D_StartDraw( work->graphic );
  ZUKAN_TOROKU_GRAPHIC_3D_EndDraw( work->graphic );

  return proc_res;
}

//-------------------------------------
/// ���ʔw�i�̐���
//=====================================
static void Zukan_Toroku_CreateBG( ZUKAN_TOROKU_WORK* work )
{
  {
    work->bg_visible_off = TRUE;

    GFL_BG_ClearScreen( GFL_BG_FRAME3_M );  // GFL_BG_FRAME1_M�͂��ꂩ��g�p����B
                                            // GFL_BG_FRAME0_M, GFL_BG_FRAME2_M�ɂ��Ă�Zukan_Toroku_VBlankFunc�ŁB

    GFL_BG_SetPriority( GFL_BG_FRAME1_M, 3 );  // �Ŕw��
    GFL_BG_SetPriority( GFL_BG_FRAME3_M, 0 );  // �őO��
  }

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, work->heap_id );
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_zukan_gra_info_name_bgu_NCLR, PALTYPE_MAIN_BG,
                                      0*0x20, 1*0x20,  // ���̌�A�{�����������炱�����C��
                                      work->heap_id );
    work->bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle,
                                                                       NARC_zukan_gra_info_name_bgu_NCGR,
                                                                       GFL_BG_FRAME1_M,
                                                                       32*2*GFL_BG_1CHRDATASIZ,  // ���̌�A�K�v�ȃL�����̈悪�������炱�����C��
                                                                       FALSE, work->heap_id );
    GF_ASSERT_MSG( work->bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_TOROKU : BG�L�����̈悪����܂���ł����B\n" );
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_zukan_gra_info_name_bgu_NSCR,
                                     GFL_BG_FRAME1_M,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_chara_info ),
                                     32*24*GFL_BG_1SCRDATASIZ,
                                     FALSE, work->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
}

//-------------------------------------
/// ���ʔw�i�̔j��
//=====================================
static void Zukan_Toroku_DeleteBG( ZUKAN_TOROKU_WORK* work )
{
  // �ǂݍ��񂾃��\�[�X�̔j��
  {
    GFL_BG_FreeCharacterArea( GFL_BG_FRAME1_M,
                              GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->bg_chara_info ) );
  }
}

//-------------------------------------
/// VBlank���ɌĂяo�����֐�
//=====================================
static void Zukan_Toroku_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_TOROKU_WORK* work = (ZUKAN_TOROKU_WORK*)wk;

  if( work->bg_visible_off )
  {
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, FALSE );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, FALSE );
    work->bg_visible_off = FALSE;
  }
}
