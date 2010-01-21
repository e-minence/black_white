//============================================================================
/**
 *  @file   yesno_menu.c
 *  @brief  ����ʂ��g���Ă̓�����j���[
 *  @author Koji Kawada
 *  @data   2010.01.14
 *  @note   
 *
 *  ���W���[�����FYESNO_MENU
 */
//============================================================================
//#define NO_USE_OBJ

// �K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// �C���N���[�h
#include "print/gf_font.h"
#include "print/printsys.h"
#ifndef NO_USE_OBJ
#include "ui/ui_easy_clwk.h"
#endif

// �T�E���h
#include "sound/pm_sndsys.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "battle/battgra_wb.naix"
#include "app_menu_common.naix"

// yesno_menu
#include "ui/yesno_menu.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// �J�[�\��OBJ
#define CURSOR_NUM (4)

typedef enum
{
  CURSOR_ORIGIN_UP,
  CURSOR_ORIGIN_DOWN,

  CURSOR_ORIGIN_MAX
}
CURSOR_ORIGIN;

typedef enum
{
  CURSOR_POS_X,
  CURSOR_POS_Y,

  CURSOR_POS_MAX
}
CURSOR_POS;

// �{�^���̃p���b�g�A�j���[�V����
typedef enum
{
  BTN_PAL_ANM_0,
  BTN_PAL_ANM_1,
  BTN_PAL_ANM_2,
  BTN_PAL_ANM_MAX,

  BTN_PAL_ANM_START   = BTN_PAL_ANM_0,
  BTN_PAL_ANM_END     = BTN_PAL_ANM_2,
}
BTN_PAL_ANM;

// �X�e�b�v
typedef enum
{
  STEP_BEFORE_OPEN,      // �J���O
  STEP_OPEN,             // �J��
  STEP_SELECT,           // �I��
  STEP_AFTER_DECIDE,     // �����̃{�^���̃p���b�g�A�j���[�V�������o��
  STEP_BEFORE_CLOSE,     // ����O
}
STEP;

//=============================================================================
/**
 *  �\���̐錾
 */
//=============================================================================
// �J�[�\��OBJ
typedef struct
{
  GFL_CLWK*               clwk[CURSOR_NUM];
#ifndef NO_USE_OBJ
  UI_EASY_CLWK_RES        res;
#endif
  CURSOR_ORIGIN           origin;
}
CURSOR_SET;

// ���[�N
struct _YESNO_MENU_WORK
{
  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  HEAPID                 heap_id;
  GFL_CLUNIT*            clunit;
  GFL_FONT*              font;
  PRINT_QUE*             print_que;

  // �����ŊǗ�����B
  PALTYPE                bg_paltype;

  u8                     btn_bg_frame;
  u8                     btn_bg_frame_prio;
  u8                     btn_yes_bg_pal;
  u8                     btn_no_bg_pal;

  u8                     mes_bg_frame;
  u8                     mes_bg_frame_prio;
  u8                     mes_bg_pal;

  CLSYS_DRAW_TYPE        cursor_obj_draw_type;
  u8                     cursor_obj_pal;         // cursor_obj_pal��cursor_obj_pal+1��2�{���g�p����

  BOOL                   key;                    // �L�[����̂Ƃ�TRUE  // �^�b�`�őI��������FALSE�ɂ���

  u16                    btn_pal_col[BTN_PAL_ANM_MAX][0x10];
  GFL_ARCUTIL_TRANSINFO  btn_transinfo;
  
  GFL_BMPWIN*            mes_yes_bmpwin;
  GFL_BMPWIN*            mes_no_bmpwin;

  STEP                   step;
  YESNO_MENU_SEL         sel;                    // �Ō�̑I������
  CURSOR_SET             cursor_set;
  u8                     btn_pal_anm;            // BTN_PAL_ANM  // ���肵���{�^���̃p���b�g�A�j���[�V����
  u16                    count;                  // ���肵�Ă��琔�t���[���҂ۂ̃J�E���g
};

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static u8 YesNo_Menu_GetCursorPos( CURSOR_ORIGIN origin, u8 idx, CURSOR_POS pos );
static void SetPaletteColor( PALTYPE paltype, u8 pal, const u16 pal_col[0x10] );

//=============================================================================
/**
 *  �O�����J�֐���`
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief     ���\�[�X�ǂݍ���
 *
 *  @param[in] heap_id     �q�[�vID
 *
 *  @retval    �����������[�N
 */
//-----------------------------------------------------------------------------
YESNO_MENU_WORK* YESNO_MENU_CreateRes(
                          HEAPID         heap_id,
                          u8             bg_frame,           // bg_frame��bg_frame+1��2���g�p����
                          u8             bg_frame_prio,      // bg_frame_prio��bg_frame_prio+1�ɐݒ肷��
                          u8             bg_pal,             // bg_pal����bg_pal+2��3�{���g�p����
                          u8             obj_pal,            // obj_pal��obj_pal+1��2�{���g�p����
                          GFL_CLUNIT*    clunit,
                          GFL_FONT*      font,
                          PRINT_QUE*     print_que,
                          BOOL           key )               // �L�[����̂Ƃ�TRUE
{
  YESNO_MENU_WORK* work;

  // ���[�N
  {
    u32 size = sizeof( YESNO_MENU_WORK );
    work = GFL_HEAP_AllocMemory( heap_id, size );
    GFL_STD_MemClear( work, size );
  }

  // �������烏�[�N�ɒl��ݒ肷��
  {
    work->heap_id               = heap_id;
    work->clunit                = clunit;
    work->font                  = font;
    work->print_que             = print_que;

    work->bg_paltype            = (bg_frame<=GFL_BG_FRAME3_M)?(PALTYPE_MAIN_BG):(PALTYPE_SUB_BG);
    
    work->btn_bg_frame          = bg_frame+1;
    work->mes_bg_frame          = bg_frame;
    GF_ASSERT_MSG( work->btn_bg_frame<=GFL_BG_FRAME3_M || GFL_BG_FRAME0_S<=work->mes_bg_frame, "YESNO_MENU : BG�t���[���̐ݒ肪�Ԉ���Ă��܂��B\n" );

    work->btn_bg_frame_prio     = bg_frame_prio+1;
    work->mes_bg_frame_prio     = bg_frame_prio;
    GF_ASSERT_MSG( work->btn_bg_frame_prio<4, "YESNO_MENU : BG�t���[���̃v���C�I���e�B�ݒ肪�Ԉ���Ă��܂��B\n" );

    work->btn_yes_bg_pal        = bg_pal;
    work->btn_no_bg_pal         = bg_pal+1;
    work->mes_bg_pal            = bg_pal+2;
    GF_ASSERT_MSG( work->mes_bg_pal<16, "YESNO_MENU : BG�p���b�g�̐ݒ肪�Ԉ���Ă��܂��B\n" );

    work->cursor_obj_draw_type  = (bg_frame<=GFL_BG_FRAME3_M)?(CLSYS_DRAW_MAIN):(CLSYS_DRAW_SUB);
    work->cursor_obj_pal        = obj_pal;  // cursor_obj_pal��cursor_obj_pal+1��2�{���g�p����
    GF_ASSERT_MSG( work->cursor_obj_pal+1<16, "YESNO_MENU : OBJ�p���b�g�̐ݒ肪�Ԉ���Ă��܂��B\n" );

    work->key                   = key;
  }

  // �N���A
  {
    GFL_BG_ClearFrame( work->btn_bg_frame );
    GFL_BG_ClearFrame( work->mes_bg_frame );
  }

  // �J���O�̒l�ݒ�
  {
    work->step                  = STEP_BEFORE_OPEN;
    work->sel                   = YESNO_MENU_SEL_SEL;
    work->cursor_set.origin     = CURSOR_ORIGIN_UP;
  } 

  // btn_bg
  {
    // �t�@�C���I�[�v��
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, work->heap_id );

    // �p���b�g�A�j���[�V����
    {
      NNSG2dPaletteData* pal_data;
      u16* pal_raw;
      u8 i, j;

      u32 size = GFL_ARC_GetDataSizeByHandle( handle, NARC_app_menu_common_sentaku_win_NCLR );
      void* buf = GFL_HEAP_AllocMemory( work->heap_id, size );
      GFL_ARC_LoadDataByHandle( handle, NARC_app_menu_common_sentaku_win_NCLR, buf );
      NNS_G2dGetUnpackedPaletteData( buf, &pal_data );
      pal_raw = (u16*)(pal_data->pRawData);

      for( i=0; i<BTN_PAL_ANM_MAX; i++ )
      {
        for( j=0; j<0x10; j++ )
        {
          work->btn_pal_col[i][j] = pal_raw[ (1+i)*0x10 + j ];  // 0�Ԗ�(yes�̕��ʐF)��1�Ԗ�(no�̕��ʐF)�͓����F�Ȃ̂ŁA1�Ԗڂ���F��ǂݍ���
        }
      }

      GFL_HEAP_FreeMemory( buf );
    }

    // �p���b�g
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_app_menu_common_sentaku_win_NCLR, work->bg_paltype,
                                      work->btn_yes_bg_pal*0x20, 2*0x20, work->heap_id );  // 0�Ԗ�(yes�̕��ʐF)��1�Ԗ�(no�̕��ʐF)�����]������΂���

    // �L����
    work->btn_transinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle,
                                                                       NARC_app_menu_common_sentaku_win_NCGR,
                                                                       work->btn_bg_frame,
                                                                       13*1*GFL_BG_1CHRDATASIZ,
                                                                       FALSE, work->heap_id );
    GF_ASSERT_MSG( work->btn_transinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "YESNO_MENU : BG�L�����̈悪����܂���ł����B\n" );

    // �X�N���[��
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_app_menu_common_sentaku_win_NSCR,
                                     work->btn_bg_frame,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->btn_transinfo ),
                                     32*24*GFL_BG_1SCRDATASIZ,
                                     FALSE, work->heap_id );
    GFL_BG_ChangeScreenPalette( work->btn_bg_frame, 0, 0, 32, 11, work->btn_yes_bg_pal );
    GFL_BG_ChangeScreenPalette( work->btn_bg_frame, 0, 11, 32, 13, work->btn_no_bg_pal );
    GFL_BG_LoadScreenReq( work->btn_bg_frame );
 
    // �t�@�C���N���[�Y 
    GFL_ARC_CloseDataHandle( handle );

    // �p���b�g�̐F��ݒ肷��
    SetPaletteColor( work->bg_paltype, work->btn_yes_bg_pal, work->btn_pal_col[BTN_PAL_ANM_START] );
    SetPaletteColor( work->bg_paltype, work->btn_no_bg_pal, work->btn_pal_col[BTN_PAL_ANM_START] );
    // ��\��
    GFL_BG_SetVisible( work->btn_bg_frame, VISIBLE_OFF );
  }

  // mes_bg
  {
    // �p���b�g
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, work->bg_paltype,
                                   work->mes_bg_pal*0x20, 1*0x20, work->heap_id );

    // BMPWIN
    work->mes_yes_bmpwin = GFL_BMPWIN_Create( work->mes_bg_frame,
                                              0, 5, 32, 6,
                                              work->mes_bg_pal, GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->mes_yes_bmpwin), 0 );
    
    work->mes_no_bmpwin  = GFL_BMPWIN_Create( work->mes_bg_frame,
                                              0, 11, 32, 6,
                                              work->mes_bg_pal, GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->mes_no_bmpwin), 0 );

    // ��\��
    GFL_BG_SetVisible( work->mes_bg_frame, VISIBLE_OFF );
  }

#ifndef NO_USE_OBJ
  // cursor_obj
  {
    u8 i;
    UI_EASY_CLWK_RES_PARAM param;
    
    param.draw_type       = work->cursor_obj_draw_type;
    param.comp_flg        = UI_EASY_CLWK_RES_COMP_NONE;
    param.arc_id          = ARCID_BATTGRA;
    param.pltt_id         = NARC_battgra_wb_battle_w_obj_NCLR;
    param.ncg_id          = NARC_battgra_wb_battle_w_cursor_NCGR;
    param.cell_id         = NARC_battgra_wb_battle_w_cursor_NCER;
    param.anm_id          = NARC_battgra_wb_battle_w_cursor_NANR;
    param.pltt_line       = work->cursor_obj_pal;
    param.pltt_src_ofs    = 0;  // �󂢂Ă���Ƃ���̓p���b�g�ԍ�0�ŕ`����Ă���A�J�[�\���̓p���b�g�ԍ�1�ŕ`����Ă���̂ŁA
    param.pltt_src_num    = 2;  // �p���b�g��2�{�K�v�łȂ�B
  
    UI_EASY_CLWK_LoadResource( &(work->cursor_set.res), &param, work->clunit, work->heap_id );

    for( i=0; i<CURSOR_NUM; i++ )
    {
      work->cursor_set.clwk[i] = UI_EASY_CLWK_CreateCLWK( &(work->cursor_set.res), work->clunit,
                                   YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_X),
                                   YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_Y),
                                   i,
                                   work->heap_id );
      GFL_CLACT_WK_SetBgPri( work->cursor_set.clwk[i], work->mes_bg_frame_prio );

      GFL_CLACT_WK_SetDrawEnable( work->cursor_set.clwk[i], FALSE );
      GFL_CLACT_WK_SetAutoAnmFlag( work->cursor_set.clwk[i], FALSE );
    }

    // �\��
    if( work->cursor_obj_draw_type == CLSYS_DRAW_MAIN )
    {
      GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    }
    else
    {
      GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    }
  }
#endif

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     ���\�[�X�j��
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
void YESNO_MENU_DeleteRes( YESNO_MENU_WORK* work )
{
#ifndef NO_USE_OBJ
  // cursor_obj 
  {
    u8 i;
    for( i=0; i<CURSOR_NUM; i++ )
    {
      GFL_CLACT_WK_Remove( work->cursor_set.clwk[i] );
    }
    UI_EASY_CLWK_UnLoadResource( &(work->cursor_set.res) );
  }
#endif

  // mes_bg
  {
    GFL_BMPWIN_Delete( work->mes_yes_bmpwin );
    GFL_BMPWIN_Delete( work->mes_no_bmpwin );
  }

  // btn_bg
  {
    GFL_BG_FreeCharacterArea( work->btn_bg_frame, GFL_ARCUTIL_TRANSINFO_GetPos( work->btn_transinfo ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->btn_transinfo ) );
  }

  // ���[�N
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �J��
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
void YESNO_MENU_OpenMenu( YESNO_MENU_WORK* work, STRBUF* yes, STRBUF* no )
{
  // �J�����Ƃ��̒l�ݒ�
  {
    work->step                  = STEP_OPEN;
    work->sel                   = YESNO_MENU_SEL_SEL;
    work->cursor_set.origin     = CURSOR_ORIGIN_UP;

#ifndef NO_USE_OBJ
    // cursor_obj
    {
      u8 i;
      for( i=0; i<CURSOR_NUM; i++ )
      {
        GFL_CLACTPOS pos;
        pos.x = (s16)( YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_X) );
        pos.y = (s16)( YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_Y) );
        GFL_CLACT_WK_SetPos( work->cursor_set.clwk[i], &pos, work->cursor_obj_draw_type );
      }
    }
#endif
  }

  // btn_bg
  {
    // �p���b�g�̐F��ݒ肷��
    SetPaletteColor( work->bg_paltype, work->btn_yes_bg_pal, work->btn_pal_col[BTN_PAL_ANM_START] );
    SetPaletteColor( work->bg_paltype, work->btn_no_bg_pal, work->btn_pal_col[BTN_PAL_ANM_START] );
    // �\��
    GFL_BG_SetVisible( work->btn_bg_frame, VISIBLE_ON );
  }

  // mes_bg
  {
    PRINTSYS_LSB lsb = PRINTSYS_LSB_Make(0xF,2,0);
    u32 yes_width, no_width;

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->mes_yes_bmpwin), 0 );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->mes_no_bmpwin), 0 );
    
    yes_width = PRINTSYS_GetStrWidth( yes, work->font, 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->mes_yes_bmpwin),
                            (u16)( (32*8 - yes_width) / 2 ), 16,
                            yes, work->font, lsb );

    no_width = PRINTSYS_GetStrWidth( no, work->font, 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->mes_no_bmpwin),
                            (u16)( (32*8 - no_width) / 2 ), 16,
                            no, work->font, lsb );

    // �\��
    GFL_BG_SetVisible( work->mes_bg_frame, VISIBLE_ON );

    // �]��
    GFL_BMPWIN_MakeTransWindow_VBlank(work->mes_yes_bmpwin);
    GFL_BMPWIN_MakeTransWindow_VBlank(work->mes_no_bmpwin);
  }

  if( work->key )
  {
#ifndef NO_USE_OBJ
    // cursor_obj
    {
      u8 i;
      for( i=0; i<CURSOR_NUM; i++ )
      {
        GFL_CLACT_WK_SetDrawEnable( work->cursor_set.clwk[i], TRUE );
        GFL_CLACT_WK_SetAutoAnmFlag( work->cursor_set.clwk[i], TRUE );
      }
    }
#endif
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief     ����
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
void YESNO_MENU_CloseMenu( YESNO_MENU_WORK* work )
{
  // btn_bg
  {
    // ��\��
    GFL_BG_SetVisible( work->btn_bg_frame, VISIBLE_OFF );
  }
 
  // mes_bg
  {
    // ��\��
    GFL_BG_SetVisible( work->mes_bg_frame, VISIBLE_OFF );
  }

#ifndef NO_USE_OBJ
  // cursor_obj
  {
    u8 i;
    for( i=0; i<CURSOR_NUM; i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( work->cursor_set.clwk[i], FALSE );
      GFL_CLACT_WK_SetAutoAnmFlag( work->cursor_set.clwk[i], FALSE );
    }
  }
#endif
  
  // �����Ƃ��̒l�ݒ�
  {
    work->step = STEP_BEFORE_OPEN;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �又��
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
void YESNO_MENU_Main( YESNO_MENU_WORK* work )
{
  BOOL btn_pal_anm_update = FALSE;

  u32 tp_x, tp_y;
  BOOL tp_trg;
  int key_trg = GFL_UI_KEY_GetTrg();
  tp_trg = GFL_UI_TP_GetPointTrg(&tp_x, &tp_y);

  switch( work->step )
  {
  case STEP_BEFORE_OPEN:
    {
      // �������Ȃ�
    }
    break;
  case STEP_OPEN:  // ����͂�h�����߂ɁA�J���������1�t���[���҂�
    {
      // ����
      work->step = STEP_SELECT;
    }
    break;
  case STEP_SELECT:
    {
      BOOL decide = FALSE;

      // �L�[����
      if( !decide )
      {
        if( work->key )
        {
          if( key_trg )
          {
            BOOL move = FALSE;

            if( key_trg & PAD_BUTTON_A )
            {
              PMSND_PlaySE( SEQ_SE_DECIDE2 );
              if( work->cursor_set.origin == CURSOR_ORIGIN_UP )
              {
                work->sel = YESNO_MENU_SEL_YES;
              }
              else
              {
                work->sel = YESNO_MENU_SEL_NO;
              }
              decide = TRUE;
            }
            else if( key_trg & PAD_BUTTON_B )
            {
              PMSND_PlaySE( SEQ_SE_CANCEL2 );
              work->sel = YESNO_MENU_SEL_NO;
              decide = TRUE;
            }
            else if( key_trg & PAD_KEY_UP )
            {
              if( work->cursor_set.origin != CURSOR_ORIGIN_UP )
              {
                work->cursor_set.origin = CURSOR_ORIGIN_UP;
                move = TRUE;
              }
            }
            else if( key_trg & PAD_KEY_DOWN )
            {
              if( work->cursor_set.origin != CURSOR_ORIGIN_DOWN )
              {
                work->cursor_set.origin = CURSOR_ORIGIN_DOWN;
                move = TRUE;
              }
            }
            
            if( move )
            {
#ifndef NO_USE_OBJ
              // cursor_obj
              {
                u8 i;
                for( i=0; i<CURSOR_NUM; i++ )
                {
                  GFL_CLACTPOS pos;
                  pos.x = (s16)( YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_X) );
                  pos.y = (s16)( YesNo_Menu_GetCursorPos(work->cursor_set.origin, i, CURSOR_POS_Y) );
                  GFL_CLACT_WK_SetPos( work->cursor_set.clwk[i], &pos, work->cursor_obj_draw_type );
                }
              }
#endif

              PMSND_PlaySE( SEQ_SE_SELECT1 );
            }
          }
        }
      }

      // �^�b�`
      if( !decide )
      {
        if( tp_trg )
        {
          const GFL_UI_TP_HITTBL tp_data[] =
          {
            { 32,  80-1, 8, 248-1 },
            { 80, 128-1, 8, 248-1 },
            { GFL_UI_TP_HIT_END, 0,0,0 },
          };

          int hit = GFL_UI_TP_HitSelf( tp_data, tp_x, tp_y );
          if( hit != GFL_UI_TP_HIT_NONE )
          {
            PMSND_PlaySE( SEQ_SE_DECIDE2 );
            if( hit == 0 )
            {
              work->sel = YESNO_MENU_SEL_YES;
            }
            else if( hit == 1 )
            {
              work->sel = YESNO_MENU_SEL_NO;
            }

            decide = TRUE;
            work->key = FALSE;  // �^�b�`�ŏI��
          }
        }
      }

      // ���߂ẴL�[����
      if( !decide )
      {
        if( !(work->key) )
        {
          if( key_trg )
          {
#ifndef NO_USE_OBJ
            // cursor_obj
            {
              u8 i;
              for( i=0; i<CURSOR_NUM; i++ )
              {
                GFL_CLACT_WK_SetDrawEnable( work->cursor_set.clwk[i], TRUE );
                GFL_CLACT_WK_SetAutoAnmFlag( work->cursor_set.clwk[i], TRUE );
              }
            }
#endif

            work->key = TRUE;
          }
        }
      }

      // ���肵�����H
      if( decide )
      {
        // ����
        work->step = STEP_AFTER_DECIDE;

        work->btn_pal_anm = BTN_PAL_ANM_START;
        work->btn_pal_anm += 1;
        work->count = 0;

        btn_pal_anm_update = TRUE;
      }
    }
    break;
  case STEP_AFTER_DECIDE:
    {
      if( work->count < 30 )
      {
        work->count++;

        if( work->btn_pal_anm < BTN_PAL_ANM_MAX )
        {
          work->btn_pal_anm += 1;
          if( work->btn_pal_anm <= BTN_PAL_ANM_END )
          {
            btn_pal_anm_update = TRUE;
          }
        }
      }
      else
      {
        // ����
        work->step = STEP_BEFORE_CLOSE;
      }
    }
    break;
  case STEP_BEFORE_CLOSE:
    {
      // �������Ȃ�
    }
    break;
  }

  // �X�V
  if( btn_pal_anm_update )
  {
    // btn_bg
    {
      // �p���b�g�̐F��ݒ肷��
      SetPaletteColor( work->bg_paltype,
        (work->sel==YESNO_MENU_SEL_YES)?(work->btn_yes_bg_pal):(work->btn_no_bg_pal),
        work->btn_pal_col[work->btn_pal_anm] );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �I�����ʂ𓾂�
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
YESNO_MENU_SEL YESNO_MENU_GetSelect( YESNO_MENU_WORK* work )
{
  if( work->step != STEP_BEFORE_OPEN && work->step != STEP_BEFORE_CLOSE )
    return YESNO_MENU_SEL_SEL;  // �A�j���[�V�������I���܂ł͑I�𒆂�Ԃ��Ă���
  return work->sel;
}

//-----------------------------------------------------------------------------
/**
 *  @brief     �L�[����őI��������
 *
 *  @param[in] work     YESNO_MENU_CreateRes�Ő����������[�N
 *
 *  @retval    
 */
//-----------------------------------------------------------------------------
BOOL YESNO_MENU_IsKey( YESNO_MENU_WORK* work )  // �L�[����őI�������Ƃ�TRUE��Ԃ�
{
  return work->key;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// �J�[�\��OBJ�̕\���ʒu�𓾂�
//=====================================
static u8 YesNo_Menu_GetCursorPos( CURSOR_ORIGIN origin, u8 idx, CURSOR_POS pos )
{
  const u8 cursor_origin[CURSOR_ORIGIN_MAX][CURSOR_POS_MAX] =  // ����
  {
    { 0, 40 },
    { 0, 88 },
  };
  const u8 cursor_pos[CURSOR_NUM][CURSOR_POS_MAX] =
  {
    {   0   ,  0    },  // ����
    {   0   , 48 -1 },  // ����
    { 256 -1,  0    },  // �E��
    { 256 -1, 48 -1 },  // �E��
  };
  return ( cursor_origin[origin][pos] + cursor_pos[idx][pos] );
}

//-------------------------------------
/// �p���b�g�̐F��ݒ肷��
//=====================================
static void SetPaletteColor( PALTYPE paltype, u8 pal, const u16 pal_col[0x10] )
{
  NNS_GFD_DST_TYPE dst_type;

  switch( paltype )
  {
  case PALTYPE_MAIN_BG:   dst_type = NNS_GFD_DST_2D_BG_PLTT_MAIN;    break; 
  case PALTYPE_MAIN_OBJ:  dst_type = NNS_GFD_DST_2D_OBJ_PLTT_MAIN;   break;
  case PALTYPE_SUB_BG:    dst_type = NNS_GFD_DST_2D_BG_PLTT_SUB;     break;
  case PALTYPE_SUB_OBJ:   dst_type = NNS_GFD_DST_2D_OBJ_PLTT_SUB;    break;
  default:                return;                                    break;
  }

  NNS_GfdRegisterNewVramTransferTask( dst_type,
    pal * 0x20,
    (void*)pal_col, 0x20 );
}

