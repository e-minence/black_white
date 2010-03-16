//============================================================================================
/**
 * @file  d_soga.c
 * @brief �f�o�b�O�p�֐�
 * @author  soga
 * @date  2008.09.09
 */
//============================================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>

#include "print/printsys.h"
#include "arc_def.h"
#include "msg/msg_d_soga.h"
#include "system/main.h"
#include "font/font.naix"
#include "message.naix"
#include "poke_tool/monsno_def.h"

#include "demo/shinka_demo.h"

#include "app/pdc.h"
#include "field/zonedata.h"

enum{
  BACK_COL = 0,
  SHADOW_COL = 2,
  LETTER_COL_NORMAL = 1,
  LETTER_COL_SELECT,
  LETTER_COL_CURSOR,
};

#define G2D_BACKGROUND_COL  (0x0000)
#define G2D_FONT_COL    (0x7fff)
#define G2D_FONTSELECT_COL  (0x001f)

#define PAD_BUTTON_EXIT ( PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START )

enum{
  BMPWIN_MENU = 0,

  BMPWIN_MAX
};

typedef struct
{
  int         seq_no;
  int         pos;
  HEAPID        heapID;
  GFL_BMPWIN      *bmpwin[ BMPWIN_MAX ];
  GFL_MSGDATA     *msg;
  GFL_FONT      *font;
  int         key_repeat_speed;
  int         key_repeat_wait;
  void*       param;
  GFL_PROCSYS*  local_procsys;
  PDC_SETUP_PARAM*    psp;
  BTL_FIELD_SITUATION bfs;
}SOGA_WORK;

static  void  TextPrint( SOGA_WORK *wk );

typedef struct
{
  const u32     menu_str;
  const GFL_PROC_DATA *gpd;
}SOGA_PROC_TABLE;

extern const GFL_PROC_DATA EffectViewerProcData;
extern const GFL_PROC_DATA PokemonViewerProcData;
extern const GFL_PROC_DATA TrainerViewerProcData;
extern const GFL_PROC_DATA DebugBattleTestProcData;
extern const GFL_PROC_DATA CaptureTestProcData;

static	const	SOGA_PROC_TABLE	spt[]={
	{ DSMSG_EFFECT_VIEWER,	&EffectViewerProcData },
	{ DSMSG_POKEMON_VIEWER,	&PokemonViewerProcData },
	{ DSMSG_TRAINER_VIEWER,	&TrainerViewerProcData },
	{ DSMSG_BATTLE_TEST,	&DebugBattleTestProcData },
	{ DSMSG_CAPTURE,		&CaptureTestProcData },
	{ DSMSG_CAPTURE,		&PDC_ProcData },
};

//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugSogabeMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SOGA_WORK* wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SOGABE_DEBUG, 0xc0000 );
  wk = GFL_PROC_AllocWork( proc, sizeof( SOGA_WORK ), HEAPID_SOGABE_DEBUG );
  MI_CpuClearFast( wk, sizeof( SOGA_WORK ) );
  wk->heapID = HEAPID_SOGABE_DEBUG;

  {
    static const GFL_DISP_VRAM dispvramBank = {
      GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
      GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
      GX_VRAM_SUB_BG_32_H,      // �T�u2D�G���W����BG
      GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
      GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
      GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
      GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
      GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
      GX_VRAM_TEX_01_BC,        // �e�N�X�`���C���[�W�X���b�g
      GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g
      GX_OBJVRAMMODE_CHAR_1D_64K,   // ���C��OBJ�}�b�s���O���[�h
      GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
    };
    GFL_DISP_SetBank( &dispvramBank );

    //VRAM�N���A
    MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
    MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
    MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
    MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
    MI_CpuFill16((void*)HW_BG_PLTT, 0x0000, HW_BG_PLTT_SIZE);

  }

  G2_BlendNone();
  GFL_BG_Init( wk->heapID );
  GFL_BMPWIN_Init( wk->heapID );

  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }

  {
    ///< main
    GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

    ///< sub
    GFL_BG_SetVisible( GFL_BG_FRAME0_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );

  }
  GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  wk->seq_no = 0;

  //2D��ʏ�����
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
      ///<FRAME1_M
      {
        0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
      ///<FRAME2_M
      {
        0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
      ///<FRAME3_M
      {
        0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
    };
    GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME1_M );
    GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
    GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M );
  }

  {
    //�t�H���g�p���b�g�쐬���]��
    static  u16 plt[16] = { G2D_BACKGROUND_COL, G2D_FONT_COL, G2D_FONTSELECT_COL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    GFL_BG_LoadPalette( GFL_BG_FRAME2_M, &plt, 16*2, 0 );
  }

  wk->bmpwin[ BMPWIN_MENU ] = GFL_BMPWIN_Create( GFL_BG_FRAME2_M, 0, 0, 32, 24, 0, GFL_BG_CHRAREA_GET_F );

  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );

  //�t�F�[�h�C��
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 2 );

  GFL_UI_KEY_GetRepeatSpeed( &wk->key_repeat_speed, &wk->key_repeat_wait );
  GFL_UI_KEY_SetRepeatSpeed( wk->key_repeat_speed / 4, wk->key_repeat_wait );

  //���b�Z�[�W�n������
  GFL_FONTSYS_Init();
  wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_soga_dat, wk->heapID );
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, TRUE, wk->heapID );

  TextPrint( wk );

  (*seq) = 0;

  wk->local_procsys = GFL_PROC_LOCAL_boot( wk->heapID );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugSogabeMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  int pad = GFL_UI_KEY_GetCont();
  int trg = GFL_UI_KEY_GetTrg();
  int rep = GFL_UI_KEY_GetRepeat();
  int tp = GFL_UI_TP_GetTrg();
  SOGA_WORK* wk = mywk;

  GFL_PROC_MAIN_STATUS  local_proc_status = GFL_PROC_LOCAL_Main( wk->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch( wk->seq_no ){ 
  case 0:
    if( ( trg & PAD_KEY_UP ) && ( wk->pos > 0 ) ){
      wk->pos--;
    }
    else if( ( trg & PAD_KEY_DOWN ) && ( wk->pos < NELEMS( spt ) - 1 ) ){
      wk->pos++;
    }
    else if( trg & PAD_BUTTON_A ){
      if( wk->pos == 5 )
      { 
        wk->seq_no = 2;
      }
      else
      { 
        wk->seq_no = 1;
        return GFL_PROC_RES_FINISH;
      }
    }
    break;
  case 1:
    break;
  case 2:
    { 
      POKEMON_PARAM*  pp = PP_Create( MONSNO_HUSIGIDANE, 5, 0, wk->heapID );
      BTL_FIELD_SITUATION bfs = { 
        0, 0, 0, 0, 0, 12, 0,
      };
      wk->bfs = bfs;
      GFL_OVERLAY_Load( FS_OVERLAY_ID(pdc) );
      wk->psp = PDC_MakeSetUpParam( pp, &wk->bfs, wk->heapID );
      ZONEDATA_Open( wk->heapID );
      GFL_PROC_LOCAL_CallProc( wk->local_procsys, NO_OVERLAY_ID, spt[ wk->pos ].gpd, wk->psp );
    }
    break;
  }

  if( trg ){
    TextPrint( wk );
  }

  if( pad == PAD_BUTTON_EXIT ){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, 2 );
    return GFL_PROC_RES_FINISH;
  }
  else{
    return GFL_PROC_RES_CONTINUE;
  }
}

//--------------------------------------------------------------------------
/**
 * PROC Exit
 */
//--------------------------------------------------------------------------
FS_EXTERN_OVERLAY(sogabe_debug);

static GFL_PROC_RESULT DebugSogabeMainProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  SOGA_WORK* wk = mywk;

  if( GFL_FADE_CheckFade() == TRUE ){
    return GFL_PROC_RES_CONTINUE;
  }

  if( wk->seq_no ){
    GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(sogabe_debug), spt[ wk->pos ].gpd, NULL );
  }

  GFL_UI_KEY_SetRepeatSpeed( wk->key_repeat_speed, wk->key_repeat_wait );

  GFL_BMPWIN_Delete( wk->bmpwin[ BMPWIN_MENU ] );

  GFL_MSG_Delete( wk->msg );
  GFL_FONT_Delete( wk->font );

  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

  GFL_PROC_LOCAL_Exit( wk->local_procsys );
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_SOGABE_DEBUG );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA   DebugSogabeMainProcData = {
  DebugSogabeMainProcInit,
  DebugSogabeMainProcMain,
  DebugSogabeMainProcExit,
};

//======================================================================
//  �e�L�X�g�\��
//======================================================================
static  void  TextPrint( SOGA_WORK *wk )
{
  int i;
  STRBUF  *strbuf;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->bmpwin[ BMPWIN_MENU ] ), 0 );

  for( i = 0 ; i < NELEMS( spt ) ; i++ ){
    if( wk->pos == i ){
      GFL_FONTSYS_SetColor( LETTER_COL_SELECT, SHADOW_COL, BACK_COL );
    }
    else{
      GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
    }

    strbuf = GFL_MSG_CreateString( wk->msg,  spt[ i ].menu_str );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( wk->bmpwin[ BMPWIN_MENU ] ),
            0,
            i * 12,
            strbuf, wk->font );
    GFL_HEAP_FreeMemory( strbuf );
  }
  GFL_BMPWIN_TransVramCharacter( wk->bmpwin[ BMPWIN_MENU ] );
  GFL_BMPWIN_MakeScreen( wk->bmpwin[ BMPWIN_MENU ] );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
}

