//============================================================================================
/**
 * @file  pdc_main.c
 * @brief �|�P�����h���[���L���b�`���C���i�p���X�ł̕ߊl�Q�[���j
 * @author  soga
 * @date  2010.03.12
 */
//============================================================================================
#include <gflib.h>

#include "pdc_main.h"
#include "pdc_msg.h"
#include "arc_def.h"

#include "battle/btlv/btlv_effect.h"
#include "battle/btlv/btlv_input.h"
#include "battle/btlv/data/pdc_sel.cdat"
#include "battle/app/b_bag.h"
#include "tr_tool/trtype_def.h"
#include "item/itemsym.h"
#include "sound/pm_sndsys.h"

#include "font/font.naix"

FS_EXTERN_OVERLAY(battle_b_app);
FS_EXTERN_OVERLAY(battle_bag);

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================
typedef PDC_RESULT  ( * PDC_FUNC )( PDC_MAIN_WORK* );

enum
{
  BTLIN_STD_FADE_WAIT = 2,
  PDC_KEY_WAIT = 80,
};

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================
struct _PDC_MAIN_WORK
{ 
  PDC_SETUP_PARAM*  psp;
  POKEMON_PARAM*    pp;
  GFL_FONT*         large_font;
  GFL_FONT*         small_font;

  BTLV_INPUT_WORK*  biw;
  PDC_MSG_WORK*     msg;

  GFL_TCBLSYS*      tcbl;

  BBAG_DATA         bagData;

  int               seq_no;
  int               wait;

  PDC_FUNC          pFunc;

  HEAPID            heapID;

  BAG_CURSOR*       bag_cursor;
  u8                cursor_flag;
};

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  void  screen_init( HEAPID heapID );
static  void  screen_exit( void );
static  void  pdc_change_seq( PDC_MAIN_WORK* pmw, PDC_FUNC func );
static  void  btlin_startFade( int wait );

static  PDC_RESULT  PDC_SEQ_Encount( PDC_MAIN_WORK* pmw );
static  PDC_RESULT  PDC_SEQ_CheckInput( PDC_MAIN_WORK* pmw );
static  PDC_RESULT  PDC_SEQ_Capture( PDC_MAIN_WORK* pmw );
static  PDC_RESULT  PDC_SEQ_Escape( PDC_MAIN_WORK* pmw );


//--------------------------------------------------------------------------
/**
 * @brief �V�X�e��������
 */
//--------------------------------------------------------------------------
PDC_MAIN_WORK* PDC_MAIN_Init( PDC_SETUP_PARAM* psp, HEAPID heapID )
{ 
  PDC_MAIN_WORK* pmw = GFL_HEAP_AllocClearMemory( heapID, sizeof( PDC_MAIN_WORK ) );

  pmw->heapID = heapID;
  pmw->psp    = psp;

  screen_init( pmw->heapID );

  pmw->large_font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, pmw->heapID );
  pmw->small_font = GFL_FONT_Create( ARCID_FONT, NARC_font_small_batt_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, pmw->heapID );

  pmw->seq_no = 0;
  {
    u16 tr_type[] = { 
      TRTYPE_HERO, TRTYPE_HERO, 0xffff, 0xffff,
    };
    BTLV_EFFECT_SETUP_PARAM* besp = BTLV_EFFECT_MakeSetUpParam( BTL_RULE_SINGLE, PDC_GetBFS( psp ), FALSE,
                                                                tr_type, NULL, pmw->heapID );
    BTLV_EFFECT_Init( besp, pmw->small_font, pmw->heapID );
    GFL_HEAP_FreeMemory( besp );
  }

  pmw->biw = BTLV_INPUT_InitEx( BTLV_INPUT_TYPE_SINGLE, BTLV_EFFECT_GetPfd(), pmw->large_font, &pmw->cursor_flag, pmw->heapID );
  pmw->tcbl = GFL_TCBL_Init( heapID, heapID, 64, 128 );

  pmw->pp = PDC_GetPP( psp );

  pmw->msg = PDC_MSG_Create( pmw->large_font, PDC_GetConfig( psp ), pmw->tcbl, pmw->heapID );
  pmw->bag_cursor = MYITEM_BagCursorAlloc( pmw->heapID );

  pdc_change_seq( pmw, &PDC_SEQ_Encount );

  return pmw;
}

//--------------------------------------------------------------------------
/**
 * @brief �V�X�e�����C��
 */
//--------------------------------------------------------------------------
PDC_RESULT  PDC_MAIN_Main( PDC_MAIN_WORK* pmw )
{ 
  GFL_TCBL_Main( pmw->tcbl );
  BTLV_EFFECT_Main();
  BTLV_INPUT_Main( pmw->biw );

  if( pmw->pFunc )
  { 
    return pmw->pFunc( pmw );
  }

  return PDC_RESULT_NONE;
}

//--------------------------------------------------------------------------
/**
 * @brief �V�X�e���I��
 */
//--------------------------------------------------------------------------
void  PDC_MAIN_Exit( PDC_MAIN_WORK* pmw )
{ 
  GFL_TCBL_Exit( pmw->tcbl );

  PDC_MSG_Delete( pmw->msg );

  BTLV_EFFECT_Exit();
  BTLV_INPUT_Exit( pmw->biw );

  GFL_FONT_Delete( pmw->large_font );
  GFL_FONT_Delete( pmw->small_font );

  screen_exit();

  GFL_HEAP_FreeMemory( pmw->bag_cursor );
  GFL_HEAP_FreeMemory( pmw );
}

//--------------------------------------------------------------------------
/**
 * @brief �V�[�P���X�؂�ւ�
 */
//--------------------------------------------------------------------------
static  void  pdc_change_seq( PDC_MAIN_WORK* pmw, PDC_FUNC func )
{ 
  pmw->seq_no = 0;
  pmw->pFunc = func;
}

//--------------------------------------------------------------------------
/**
 * @brief ��ʏ�����
 */
//--------------------------------------------------------------------------
static  void  screen_init( HEAPID heapID )
{ 
  static const GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_D,           // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_64_E,           // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_01_AB,          // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_64K, // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K, // �T�uOBJ�}�b�s���O���[�h
  };

  // BGsystem������
  GFL_BG_Init( heapID );
  GFL_BMPWIN_Init( heapID );
  GFL_FONTSYS_Init();

  // VRAM�o���N�ݒ�
  GFL_DISP_SetBank( &vramBank );

  // �e����ʃ��W�X�^������
  G2_BlendNone();
  G2S_BlendNone();

  // �㉺��ʐݒ�
  GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  //�a�f���[�h�ݒ�
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }

  //3D�֘A������ soga
  {
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, heapID, NULL );
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
    G3X_AlphaBlend( TRUE );
    G3X_EdgeMarking( TRUE );
    G3X_AntiAlias( FALSE );
    G3X_SetFog( FALSE, 0, 0, 0 );
    G2_SetBG0Priority( 1 );
  }
  //�E�C���h�}�X�N�ݒ�i��ʗ��[�̃G�b�W�}�[�L���O�̃S�~�������jsoga
  {
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 |
                 GX_WND_PLANEMASK_BG1 |
                 GX_WND_PLANEMASK_BG2 |
                 GX_WND_PLANEMASK_BG3 |
                 GX_WND_PLANEMASK_OBJ,
                 TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, TRUE );
    G2_SetWnd0Position( 1, 0, 255, 192 );
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, 3 );
  }

  {
    static  const GFL_CLSYS_INIT clsysinit = {
      0, 0,
      0, 512,
      4, 124,
      4, 124,
      0,
      56,48,48,48,
      16, 16,
    };
    GFL_CLACT_SYS_Create( &clsysinit, &vramBank, heapID );
  }
  GFL_BG_SetBGControl3D( 1 );
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------------------
/**
 * @brief ��ʏI��
 */
//--------------------------------------------------------------------------
static  void  screen_exit( void )
{ 
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  //�Z���A�N�^�[�폜
  GFL_CLACT_SYS_Delete();

  //3D�֘A�폜 soga
  GFL_G3D_Exit();
}

//--------------------------------------------------------------------------
/**
 * @brief �G���J�E���g����
 */
//--------------------------------------------------------------------------
static  PDC_RESULT  PDC_SEQ_Encount( PDC_MAIN_WORK* pmw )
{ 
  switch( pmw->seq_no ){ 
  case 0:
    PDC_MSG_HideWindow( pmw->msg );
    pmw->seq_no++;
    break;
  case 1:
    if( PDC_MSG_WaitHideWindow( pmw->msg ) )
    { 
      BTLV_EFFECT_SetPokemon( pmw->pp, BTLV_MCSS_POS_BB );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      pmw->seq_no++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() )
    { 
      PDC_MSG_SetEncountMsg( pmw->msg, pmw->psp );
      pmw->seq_no++;
    }
    break;
  case 3:
    if( PDC_MSG_Wait( pmw->msg ) )
    { 
      BTLV_EFFECT_SetGaugePP( PDC_GetZukanWork( pmw->psp ), PDC_GetPP( pmw->psp ), BTLV_MCSS_POS_BB );
      PDC_MSG_HideWindow( pmw->msg );
      pmw->seq_no++;
    }
    break;
  case 4:
    if( PDC_MSG_WaitHideWindow( pmw->msg ) )
    { 
      BTLV_EFFECT_Add( BTLEFF_PDC_ENCOUNT );
      pmw->seq_no++;
    }
    break;
  case 5:
    if( !BTLV_EFFECT_CheckExecute() )
    { 
      pdc_change_seq( pmw, &PDC_SEQ_CheckInput );
    }
    break;
  }
  return PDC_RESULT_NONE;
}

//--------------------------------------------------------------------------
/**
 * @brief �L�[�`�F�b�N����
 */
//--------------------------------------------------------------------------
static  PDC_RESULT  PDC_SEQ_CheckInput( PDC_MAIN_WORK* pmw )
{ 
  switch( pmw->seq_no ){ 
  case 0:
    PDC_MSG_SetDousuruMsg( pmw->msg, pmw->psp );
    pmw->seq_no++;
    break;
  case 1:
    if( PDC_MSG_Wait( pmw->msg ) )
    { 
      BTLV_INPUT_CreateScreen( pmw->biw, BTLV_INPUT_SCRTYPE_PDC, NULL );
      pmw->seq_no++;
    }
    break;
  case 2:
    { 
      int tp = BTLV_INPUT_CheckInput( pmw->biw, &PDCTouchData, PDCKeyData );

      if( tp != GFL_UI_TP_HIT_NONE )
      { 
        if( tp )
        { 
          pdc_change_seq( pmw, &PDC_SEQ_Escape );
        }
        else
        { 
          pdc_change_seq( pmw, &PDC_SEQ_Capture );
        }
      }
    }
    break;
  }
  return PDC_RESULT_NONE;
}

//--------------------------------------------------------------------------
/**
 * @brief �ߊl����
 */
//--------------------------------------------------------------------------
static  PDC_RESULT  PDC_SEQ_Capture( PDC_MAIN_WORK* pmw )
{ 
  switch( pmw->seq_no ){ 
  case 0:
    pmw->bagData.myitem = PDC_GetMyItem( pmw->psp );
    pmw->bagData.bagcursor = pmw->bag_cursor;
    pmw->bagData.mode = BBAG_MODE_PDC;
    pmw->bagData.font = pmw->large_font;
    pmw->bagData.heap = pmw->heapID;
    pmw->bagData.energy = 0;
    pmw->bagData.reserved_energy = 0;
    pmw->bagData.end_flg = FALSE;
    pmw->bagData.ret_item = ITEM_DUMMY_DATA;
    pmw->bagData.cursor_flg = &pmw->cursor_flag;
    pmw->bagData.time_out_flg = FALSE;
    BTLV_INPUT_SetFadeOut( pmw->biw );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_b_app ) );
    pmw->seq_no++;
    break;
  case 1:
    if( !BTLV_INPUT_CheckFadeExecute( pmw->biw ) )
    { 
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_bag ) );
      BattleBag_TaskAdd( &pmw->bagData );
      pmw->seq_no++;
    }
    break;
  case 2:
    if( pmw->bagData.end_flg ){
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_b_app ) );
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_bag ) );
      BTLV_INPUT_SetFadeIn( pmw->biw );
      if( pmw->bagData.ret_item == ITEM_DUMMY_DATA )
      { 
        pmw->seq_no = 3;
      }
      else
      { 
        pmw->seq_no = 4;
      }
    }
    break;
  case 3:
    if( !BTLV_INPUT_CheckFadeExecute( pmw->biw ) )
    { 
      pdc_change_seq( pmw, &PDC_SEQ_CheckInput );
    }
    break;
  case 4:
    if( !BTLV_INPUT_CheckFadeExecute( pmw->biw ) )
    { 
      PDC_MSG_SetThrowMsg( pmw->msg, pmw->psp );
      pmw->seq_no++;
    }
    break;
  case 5:
    if( PDC_MSG_Wait( pmw->msg ) )
    { 
      BTLV_EFFECT_BallThrow( BTLV_MCSS_POS_BB, pmw->bagData.ret_item, 4, TRUE, FALSE );
      pmw->seq_no++;
    }
    break;
  case 6:
    if( !BTLV_EFFECT_CheckExecute() )
    { 
      PDC_MSG_SetCaptureMsg( pmw->msg, pmw->psp );
      pmw->seq_no++;
    }
    break;
  case 7:
    if( PDC_MSG_IsJustDone( pmw->msg ) )
    {
      PMSND_PlayBGM( SEQ_ME_POKEGET );
    }
    if( PDC_MSG_Wait( pmw->msg ) )
    { 
      pmw->seq_no++;
    }
    break;
  case 8:
    if( !PMSND_CheckPlayBGM() )
    { 
      //�퓬�G�t�F�N�g�ꎞ��~������
      BTLV_EFFECT_Restart();
      PMSND_PlayBGM( SEQ_BGM_WIN1 );
      pmw->seq_no++;
    }
    break;
  case 9:
    if( !BTLV_EFFECT_CheckExecute() )
    { 
      pmw->wait = PDC_KEY_WAIT;
      pmw->seq_no++;
    }
    break;
  case 10:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) ||
        ( GFL_UI_TP_GetTrg() ) ||
        ( --pmw->wait == 0 ) )
    { 
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, BTLIN_STD_FADE_WAIT );
      //PMSND_FadeOutBGM( 8 );  // �ߊl�����Ƃ��́A�}�ӓo�^�▼�O���֑͂J�ڂ���̂ŁABGM�͂��̂܂܂ɂ���
      pmw->seq_no++;
    }
    break;
  case 11:
    if( !GFL_FADE_CheckFade() )
    { 
      return PDC_RESULT_CAPTURE;
    }
    break;
  }
  return PDC_RESULT_NONE;
}

//--------------------------------------------------------------------------
/**
 * @brief �����鏈��
 */
//--------------------------------------------------------------------------
static  PDC_RESULT  PDC_SEQ_Escape( PDC_MAIN_WORK* pmw )
{ 
  switch( pmw->seq_no ){ 
  case 0:
    PDC_MSG_SetEscapeMsg( pmw->msg, pmw->psp );
    pmw->seq_no++;
    break;
  case 1:
    if( PDC_MSG_Wait( pmw->msg ) )
    { 
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, BTLIN_STD_FADE_WAIT );
      PMSND_FadeOutBGM( 8 );
      pmw->seq_no++;
    }
    break;
  case 2:
    if( !GFL_FADE_CheckFade() )
    { 
      return PDC_RESULT_ESCAPE;
    }
    break;
  }
  return PDC_RESULT_NONE;
}

/**
 *  ����̃}�X�^�[�P�x�l���Q�Ƃ��ăt�F�[�h�p�����[�^�Ăѕ���
 */
static void btlin_startFade( int wait )
{
  if( GX_GetMasterBrightness() <= 0 ){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, wait );
  }else{
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, wait );
  }
}
