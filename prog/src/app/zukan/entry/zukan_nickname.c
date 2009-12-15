//============================================================================
/**
 *  @file   zukan_nickname.c
 *  @brief  �j�b�N�l�[����t���邩�ۂ�
 *  @author Koji Kawada
 *  @data   2009.12.10
 *  @note   
 *
 *  ���W���[�����FZUKAN_NICKNAME
 */
//============================================================================
// �K���K�v�ȃC���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"
#include "font/font.naix"

#include "print/printsys.h"

#include "ui/ui_easy_clwk.h"

#include "poke_tool/poke_tool.h"
#include "print/wordset.h"
#include "msgdata.h"
#include "strbuf.h"
#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"

#include "sound/pm_sndsys.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "battle/battgra_wb.naix"

// ���b�Z�[�W
#include "msg/msg_b_bag.h"
#include "msg/msg_yesnomenu.h"

#include "zukan_nickname.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
#define ZUKAN_NICKNAME_CURSOR_NUM (4)

typedef enum
{
  ZUKAN_NICKNAME_CURSOR_ORIGIN_UP,
  ZUKAN_NICKNAME_CURSOR_ORIGIN_DOWN,

  ZUKAN_NICKNAME_CURSOR_ORIGIN_MAX
}
ZUKAN_NICKNAME_CURSOR_ORIGIN;

typedef enum
{
  ZUKAN_NICKNAME_CURSOR_POS_X,
  ZUKAN_NICKNAME_CURSOR_POS_Y,

  ZUKAN_NICKNAME_CURSOR_POS_MAX
}
ZUKAN_NICKNAME_CURSOR_POS;

//=============================================================================
/**
 *  �\���̐錾
 */
//=============================================================================
typedef struct
{
  GFL_CLWK* clwk[ZUKAN_NICKNAME_CURSOR_NUM];
  UI_EASY_CLWK_RES res;
  ZUKAN_NICKNAME_CURSOR_ORIGIN origin;
}
ZUKAN_NICKNAME_CURSOR;

struct _ZUKAN_NICKNAME_WORK
{
  HEAPID heap_id;  ///< �q�[�v  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  GFL_CLUNIT* clunit;  ///< �Z���A�N�^�[���j�b�g  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  GFL_FONT* font;  ///< �t�H���g  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  PRINT_QUE* print_que;  ///< �����L���[  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B

  GFL_ARCUTIL_TRANSINFO bg_chara_info;  ///< BG�L�����̈�
  ZUKAN_NICKNAME_CURSOR cursor;  ///< �J�[�\��

  GFL_BMPWIN* yesno_bmpwin;  ///< Yes/No����

  GFL_BMPWIN* dummy_bmpwin;  ///< 0�Ԃ̃L�����N�^�[��1x1�ł����Ă���

  GFL_BMPWIN* msg_bmpwin;
  PRINT_STREAM* msg_stream;
  STRBUF* msg_strbuf;
  GFL_ARCUTIL_TRANSINFO msg_chara;
  GFL_TCBLSYS* msg_tcblsys;
};

//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static u8 Zukan_Nickname_GetCursorPos( ZUKAN_NICKNAME_CURSOR_ORIGIN origin, u8 idx, ZUKAN_NICKNAME_CURSOR_POS pos );

//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
ZUKAN_NICKNAME_WORK* ZUKAN_NICKNAME_Init( HEAPID a_heap_id, GFL_CLUNIT* a_clunit, GFL_FONT* a_font, PRINT_QUE* a_print_que )
{
  ZUKAN_NICKNAME_WORK* work;

  {
    u32 size = sizeof(ZUKAN_NICKNAME_WORK);
    work = GFL_HEAP_AllocMemory( a_heap_id, size );
    GFL_STD_MemClear( work, size );
  }

  {
    work->heap_id = a_heap_id;
    work->clunit = a_clunit;
    work->font = a_font;
    work->print_que = a_print_que;

    work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_UP;
  }

  {
    GFL_BG_SetPriority( GFL_BG_FRAME0_S, 1 );
    GFL_BG_SetPriority( GFL_BG_FRAME1_S, 0 );  // �őO��
  }

  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, work->heap_id );
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_battgra_wb_battle_w_bg_NCLR, PALTYPE_SUB_BG,
                                      0*0x20, 0, work->heap_id );
    work->bg_chara_info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, NARC_battgra_wb_battle_w_bg_NCGR,
                                                                       GFL_BG_FRAME0_S, 32*14*GFL_BG_1CHRDATASIZ, FALSE, work->heap_id );
    GF_ASSERT_MSG( work->bg_chara_info != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_NICKNAME : BG�L�����̈悪����܂���ł����B\n" );
    GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_battgra_wb_battle_w_bg1d_NSCR, GFL_BG_FRAME0_S,
                                     GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_chara_info ), 32*24*GFL_BG_1SCRDATASIZ, FALSE, work->heap_id );
    {
      NNSG2dScreenData* screen;
      void* buf = GFL_ARCHDL_UTIL_LoadScreen( handle, NARC_battgra_wb_battle_w_bg1d_NSCR, FALSE, &screen, work->heap_id );
      //GFL_BG_WriteScreenFree( GFL_BG_FRAME0_S, 0, 0, 32, 24, screen->rawData, 0, 24, 64, 64 ); 
      GFL_BG_WriteScreenFree( GFL_BG_FRAME0_S, 0, 0, 32, 24, screen->rawData, 32, 24, 64, 64 ); 
      GFL_HEAP_FreeMemory( buf );
    }
    GFL_ARC_CloseDataHandle( handle );
  }

  {
    u8 i;
    UI_EASY_CLWK_RES_PARAM param;
    
    param.draw_type = CLSYS_DRAW_SUB;
    param.comp_flg = UI_EASY_CLWK_RES_COMP_NONE;
    param.arc_id = ARCID_BATTGRA;
    param.pltt_id = NARC_battgra_wb_battle_w_obj_NCLR;
    param.ncg_id = NARC_battgra_wb_battle_w_cursor_NCGR;
    param.cell_id = NARC_battgra_wb_battle_w_cursor_NCER;
    param.anm_id = NARC_battgra_wb_battle_w_cursor_NANR;
    param.pltt_line = 5;//�K���Ȉʒu
    param.pltt_src_ofs = 0;  // �󂢂Ă���Ƃ���̓p���b�g�ԍ�0�ŕ`����Ă���A�J�[�\���̓p���b�g�ԍ�1�ŕ`����Ă���̂ŁA
    param.pltt_src_num = 2;  // �p���b�g��2�{�K�v�łȂ�B
  
    UI_EASY_CLWK_LoadResource( &(work->cursor.res), &param, work->clunit, work->heap_id );

    for( i=0; i<ZUKAN_NICKNAME_CURSOR_NUM; i++ )
    {
      work->cursor.clwk[i] = UI_EASY_CLWK_CreateCLWK( &(work->cursor.res), work->clunit,
                                                      Zukan_Nickname_GetCursorPos(work->cursor.origin, i, ZUKAN_NICKNAME_CURSOR_POS_X),
                                                      Zukan_Nickname_GetCursorPos(work->cursor.origin, i, ZUKAN_NICKNAME_CURSOR_POS_Y),
                                                      i,
                                                      work->heap_id );
      GFL_CLACT_WK_SetAutoAnmFlag( work->cursor.clwk[i], TRUE );
    }
  }

  {
    const u8 font_plt_no_sub = 7;
    PRINTSYS_LSB font_lsb = PRINTSYS_LSB_Make(8,0xA,0);

    const u8 bmpwin_chara_width = 16;
    const u32 bmpwin_width = 8 * bmpwin_chara_width;

    GFL_MSGDATA* msg_handle;
    STRBUF* yes_strbuf;
    u32 yes_width;
    STRBUF* no_strbuf;
    u32 no_width;

    //GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,  // �t�H���g��p�̃p���b�g�͓ǂݍ��܂Ȃ��ŁA
    //                               font_plt_no_sub * 0x20, 1 * 0x20, work->heap_id );   // ���ɓǂݍ��ݍς݂̃p���b�g�𗬗p����B
    
    work->yesno_bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME1_S,
                                      8, 4, bmpwin_chara_width, 12,
                                      font_plt_no_sub, GFL_BMP_CHRAREA_GET_B );
    
    msg_handle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_yesnomenu_dat, work->heap_id );

    yes_strbuf = GFL_MSG_CreateString( msg_handle, msgid_yesno_yes );
    yes_width = PRINTSYS_GetStrWidth( yes_strbuf, work->font, 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->yesno_bmpwin),
                            (u16)( (bmpwin_width - yes_width) / 2 ), 16,
                            yes_strbuf, work->font, font_lsb );
    GFL_STR_DeleteBuffer( yes_strbuf );

    no_strbuf = GFL_MSG_CreateString( msg_handle, msgid_yesno_no );
    no_width = PRINTSYS_GetStrWidth( yes_strbuf, work->font, 0 );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->yesno_bmpwin),
                            (u16)( (bmpwin_width - no_width) / 2 ), 64,
                            no_strbuf, work->font, font_lsb );
    GFL_STR_DeleteBuffer( no_strbuf );

    GFL_MSG_Delete( msg_handle );

    GFL_BMPWIN_MakeTransWindow_VBlank( work->yesno_bmpwin );
  }

  {
    const u8 win_plt_no = 8;
    const u8 font_plt_no = 9;
    const u8 clear_color = 15;

    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                   font_plt_no * 0x20, 1 * 0x20, work->heap_id );

    {
      u32 monsno = MONSNO_HUSIGIBANA;  // PPP_Get( ppp, ID_PARA_monno, NULL );  // POKEMON_PASO_PARAM* ppp
      u32 buf_id = 0;
      WORDSET* wordset;
      POKEMON_PASO_PARAM* ppp;
      GFL_MSGDATA* msg_handle;
      STRBUF* tmp_strbuf;

      ppp = (POKEMON_PASO_PARAM*)PP_Create( monsno, 0, 0, work->heap_id );
      wordset = WORDSET_Create( work->heap_id );
      WORDSET_RegisterPokeNickNamePPP( wordset, buf_id, ppp );

      msg_handle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_b_bag_dat, work->heap_id );
      tmp_strbuf = GFL_MSG_CreateString( msg_handle, mes_b_bag_m04 );
      work->msg_strbuf = GFL_STR_CreateBuffer( 64, work->heap_id );
      WORDSET_ExpandStr( wordset, work->msg_strbuf, tmp_strbuf );

      GFL_STR_DeleteBuffer( tmp_strbuf );
      GFL_MSG_Delete( msg_handle );

      WORDSET_Delete( wordset );
      GFL_HEAP_FreeMemory( ppp );
    }

    {
      work->dummy_bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 0, 0, 1, 1,
                                              font_plt_no, GFL_BMP_CHRAREA_GET_F );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->dummy_bmpwin), 0 );
      GFL_BMPWIN_TransVramCharacter(work->dummy_bmpwin);
	    GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(work->dummy_bmpwin) );
    }

    work->msg_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );

    work->msg_bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 1, 19, 30, 4,
                                          font_plt_no, GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->msg_bmpwin), clear_color );
    GFL_FONTSYS_SetColor( 1, 2, clear_color );

    work->msg_stream = PRINTSYS_PrintStream( work->msg_bmpwin, 0, 0, work->msg_strbuf, work->font,
                                             MSGSPEED_GetWait(), work->msg_tcblsys, 2, work->heap_id, clear_color );

    work->msg_chara = BmpWinFrame_GraphicSetAreaMan( GFL_BG_FRAME0_M, win_plt_no, MENU_TYPE_SYSTEM, work->heap_id );
    BmpWinFrame_Write( work->msg_bmpwin, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(work->msg_chara), win_plt_no );

    GFL_BMPWIN_MakeTransWindow_VBlank( work->msg_bmpwin );
  }

  GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );

  return work;
}

void ZUKAN_NICKNAME_Exit( ZUKAN_NICKNAME_WORK* work )
{
  {
    PRINTSYS_PrintStreamDelete( work->msg_stream );
    GFL_BG_FreeCharacterArea( GFL_BG_FRAME0_M, GFL_ARCUTIL_TRANSINFO_GetPos( work->msg_chara ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->msg_chara ) );
    GFL_BMPWIN_Delete( work->msg_bmpwin );
  	GFL_STR_DeleteBuffer( work->msg_strbuf );

    GFL_TCBL_Exit( work->msg_tcblsys );
  }
  {
    GFL_BMPWIN_Delete( work->dummy_bmpwin );
  }
  {
    GFL_BMPWIN_Delete( work->yesno_bmpwin );
  }
  {
    u8 i;
    for( i=0; i<ZUKAN_NICKNAME_CURSOR_NUM; i++ )
    {
      GFL_CLACT_WK_Remove( work->cursor.clwk[i] );
    }
    UI_EASY_CLWK_UnLoadResource( &(work->cursor.res) );
  }

  {
    GFL_BG_FreeCharacterArea( GFL_BG_FRAME0_S, GFL_ARCUTIL_TRANSINFO_GetPos( work->bg_chara_info ),
                              GFL_ARCUTIL_TRANSINFO_GetSize( work->bg_chara_info ) );
  }

  {
    GFL_HEAP_FreeMemory( work );
  }
}

ZUKAN_NICKNAME_RESULT ZUKAN_NICKNAME_Main( ZUKAN_NICKNAME_WORK* work )
{
  ZUKAN_NICKNAME_RESULT result = ZUKAN_NICKNAME_RESULT_SELECT;


  GFL_TCBL_Main( work->msg_tcblsys );


  {
    u8 i;
    int trg;

    GFL_UI_TP_HITTBL tp_data[] =
    {
      { 32,  80-1, 8, 248-1 },
      { 80, 128-1, 8, 248-1 },
      { GFL_UI_TP_HIT_END, 0,0,0 },
    };
    u32 tp_x, tp_y;
    BOOL tp_result;

    trg = GFL_UI_KEY_GetTrg();
    if( trg & PAD_BUTTON_A )
    {
      PMSND_PlaySE( SEQ_SE_DECIDE2 );
      if( work->cursor.origin == ZUKAN_NICKNAME_CURSOR_ORIGIN_UP )
      {
        result = ZUKAN_NICKNAME_RESULT_YES;
      }
      else
      {
        result = ZUKAN_NICKNAME_RESULT_NO;
      }
    }
    else if( trg & PAD_BUTTON_B )
    {
      work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_DOWN;
      PMSND_PlaySE( SEQ_SE_CANCEL2 );
      result = ZUKAN_NICKNAME_RESULT_NO;
    }
    else if( trg & PAD_KEY_UP )
    {
      if( work->cursor.origin != ZUKAN_NICKNAME_CURSOR_ORIGIN_UP )
      {
        work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_UP;
        PMSND_PlaySE( SEQ_SE_SELECT1 );
      }
    }
    else if( trg & PAD_KEY_DOWN )
    {
      if( work->cursor.origin != ZUKAN_NICKNAME_CURSOR_ORIGIN_DOWN )
      {
        work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_DOWN;
        PMSND_PlaySE( SEQ_SE_SELECT1 );
      }
    }

    tp_result = GFL_UI_TP_GetPointTrg( &tp_x, &tp_y );
    if( tp_result )
    {
      int hit = GFL_UI_TP_HitSelf( tp_data, tp_x, tp_y );
      if( hit != GFL_UI_TP_HIT_NONE )
      {
        PMSND_PlaySE( SEQ_SE_DECIDE2 );
        if( hit == 0 )
        {
          work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_UP;
          result = ZUKAN_NICKNAME_RESULT_YES;
        }
        else if( hit == 1 )
        {
          work->cursor.origin = ZUKAN_NICKNAME_CURSOR_ORIGIN_DOWN;
          result = ZUKAN_NICKNAME_RESULT_NO;
        }
      }
    }
 
    for( i=0; i<ZUKAN_NICKNAME_CURSOR_NUM; i++ )
    {
      GFL_CLACTPOS pos;
      pos.x = (s16)( Zukan_Nickname_GetCursorPos(work->cursor.origin, i, ZUKAN_NICKNAME_CURSOR_POS_X) );
      pos.y = (s16)( Zukan_Nickname_GetCursorPos(work->cursor.origin, i, ZUKAN_NICKNAME_CURSOR_POS_Y) );
      GFL_CLACT_WK_SetPos( work->cursor.clwk[i], &pos, CLSYS_DRAW_SUB );
    }
 
  }


  return result;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
static u8 Zukan_Nickname_GetCursorPos( ZUKAN_NICKNAME_CURSOR_ORIGIN origin, u8 idx, ZUKAN_NICKNAME_CURSOR_POS pos )
{
  const u8 Zukan_Nickname_CursorOrigin[ZUKAN_NICKNAME_CURSOR_ORIGIN_MAX][ZUKAN_NICKNAME_CURSOR_POS_MAX] =  // ����
  {
    { 0, 32 },
    { 0, 80 },
  };
  const u8 Zukan_Nickname_CursorPos[ZUKAN_NICKNAME_CURSOR_NUM][ZUKAN_NICKNAME_CURSOR_POS_MAX] =
  {
    {   0   ,  0    },  // ����
    {   0   , 48 -1 },  // ����
    { 256 -1,  0    },  // �E��
    { 256 -1, 48 -1 },  // �E��
  };
  return ( Zukan_Nickname_CursorOrigin[origin][pos] + Zukan_Nickname_CursorPos[idx][pos] );
}

