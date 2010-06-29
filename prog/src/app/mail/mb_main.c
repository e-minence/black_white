//============================================================================================
/**
 * @file  mb_main.c
 * @brief ���[���{�b�N�X��� ���C������
 * @author  Hiroyuki Nakamura
 * @date  09.01.31
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "arc/arc_def.h"
#include "font/font.naix"
#include "print/printsys.h"
#include "poke_tool/pokeparty.h"
#include "arc/message.naix"
#include "msg/msg_mailbox.h"

#include "mb_main.h"
#include "mb_seq.h"
#include "mb_bmp.h"

#include "mail_gra.naix"

#ifdef PM_DEBUG
//#define MAIL_DEBUG    ///< �L���ɂ���ƂO�Ԗڂ̃��[�����Q�O�ʃR�s�[���Ė��߂܂�
#endif
//============================================================================================
//  �萔��`
//============================================================================================
#define BGWF_MAILBTN_SX     ( 14 )
#define BGWF_MAILBTN_SY     ( 4 )
#define BGWF_MAILBTN_PX     ( 1 )
#define BGWF_MAILBTN_PY     ( 0 )
#define BGWF_MAILBTN_SPACE_X  ( 16 )
#define BGWF_MAILBTN_SPACE_Y  ( 4 )

#define BGWF_SELMAIL_PX     ( 4 )   // �I���������[���^�u�\���w���W
#define BGWF_SELMAIL_PY     ( 7 )   // �I���������[���^�u�\���x���W

// �{�^���w�i�L�����ʒu
#define BTN_U_BACK_CGX_POS  ( 32+7 * 0x20 )
#define BTN_D_BACK_CGX_POS  ( 32+7 * 0x20 )

#define EXP_BUF_SIZE    ( 1024 )  // �ėp������W�J�̈�T�C�Y

#define SELBOX_LIST_NUM   ( MBMAIN_MENU_MAX )   // �I���{�b�N�X���ڐ�

#define SELBOX_PX ( 18 )  // ���j���[�\���w���W
#define SELBOX_PY_4 ( 5 ) // ���j���[�\���x���W�i�S���ځj
#define SELBOX_PY_3 ( 11 )  // ���j���[�\���x���W�i�Q���ځj
#define SELBOX_SX ( 11 )  // ���j���[�w�T�C�Y


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

// VRAM����U��
const GFL_DISP_VRAM MAILBOX_VramTbl = {
  GX_VRAM_BG_128_B,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_0_A,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_01_FG,     // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K,
};
/*
// �I���{�b�N�X�w�b�_�f�[�^
static const SELBOX_HEAD_PRM SelBoxHedParam = {
  TRUE,         // ���[�v
  SBOX_OFSTYPE_CENTER,  // �Z���^�����O
  0,            // �w���W�\���I�t�Z�b�g
  MBMAIN_BGF_MSG_M,   // �\���a�f
  MBMAIN_MBG_PAL_SELBOX,  // �p���b�g
  0,            // BG�v���C�I���e�B ( ? )
  0,            // �\�t�g�E�F�A�v���C�I���e�B ( ?? )
  MBMAIN_SELBOX_CGX_NUM,  // ������̈�cgx(�L�����P��)
  MBMAIN_SELBOX_FRM_NUM,  // �t���[���̈�cgx(�L�����P��)
  MBMAIN_SELBOX_CGX_SIZ,  // ��L����cgx�̈�T�C�Y(�L�����P��)
};
*/


//--------------------------------------------------------------------------------------------
/**
 * VBLANK�֐�
 *
 * @param work  ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_VBlank( GFL_TCB *tcb, void *work )
{
  MAILBOX_SYS_WORK * syswk = work;

  GFL_BG_VBlankFunc();
  
  GFL_CLACT_SYS_VBlankFunc();

}

//--------------------------------------------------------------------------------------------
/**
 * VRAM�ݒ�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_VramInit(void)
{
  GFL_DISP_SetBank( &MAILBOX_VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_BgInit( MAILBOX_SYS_WORK * syswk )
{
  GFL_BG_Init( HEAPID_MAILBOX_APP );
  { // BG SYSTEM
    GFL_BG_SYS_HEADER sysh = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysh );
  }
  { // ���C����ʁF���[�����b�Z�[�W
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( MBMAIN_BGF_MAILMES_S, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  MBMAIN_BGF_MAILMES_S );
    GFL_BG_SetClearCharacter( MBMAIN_BGF_MAILMES_S, 0x20, 0, HEAPID_MAILBOX_APP );
  }
  { // ���C����ʁF���[���w�i
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,    1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  MBMAIN_BGF_MAIL_S, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( MBMAIN_BGF_MAIL_S );
    GFL_BG_SetClearCharacter( MBMAIN_BGF_MAIL_S, 0x20, 0, HEAPID_MAILBOX_APP );
  }
  { // ���C����ʁF����
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  MBMAIN_BGF_STR_S, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( MBMAIN_BGF_STR_S );
    GFL_BG_SetClearCharacter( MBMAIN_BGF_STR_S, 0x20, 0, HEAPID_MAILBOX_APP );
  }
  { // ���C����ʁF�w�i
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( MBMAIN_BGF_BG_S, &cnth, GFL_BG_MODE_TEXT );
  }

  { // �T�u��ʁF���b�Z�[�W
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  MBMAIN_BGF_MSG_M, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( MBMAIN_BGF_MSG_M );
    GFL_BG_SetClearCharacter( MBMAIN_BGF_MSG_M, 0x20, 0, HEAPID_MAILBOX_APP );
  }
  { // �T�u��ʁF����
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( MBMAIN_BGF_STR_M, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( MBMAIN_BGF_STR_M );
    GFL_BG_SetClearCharacter( MBMAIN_BGF_STR_M, 0x20, 0, HEAPID_MAILBOX_APP );
  }
  { // �T�u��ʁF�{�^��
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  MBMAIN_BGF_BTN_M, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( MBMAIN_BGF_BTN_M );
  }
  { // �T�u��ʁF�w�i
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01,      3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( MBMAIN_BGF_BG_M, &cnth, GFL_BG_MODE_TEXT );
  }

  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG1, VISIBLE_ON );
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG2, VISIBLE_ON );
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG2, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_BgExit( MAILBOX_SYS_WORK * syswk )
{
  GFL_BG_FreeBGControl( MBMAIN_BGF_BG_M );
  GFL_BG_FreeBGControl( MBMAIN_BGF_BTN_M );
  GFL_BG_FreeBGControl( MBMAIN_BGF_STR_M );
  GFL_BG_FreeBGControl( MBMAIN_BGF_MSG_M );
  GFL_BG_FreeBGControl( MBMAIN_BGF_BG_S );
  GFL_BG_FreeBGControl( MBMAIN_BGF_STR_S );
  GFL_BG_FreeBGControl( MBMAIN_BGF_MAIL_S );
  GFL_BG_FreeBGControl( MBMAIN_BGF_MAILMES_S );

  GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * �a�f�O���t�B�b�N�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_BgGraphicLoad( MAILBOX_SYS_WORK * syswk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_MAIL_GRA, HEAPID_MAILBOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( ah, NARC_mail_gra_mailbox_u_lz_NCGR,
                                        MBMAIN_BGF_BG_S, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen( ah, NARC_mail_gra_mailbox_u_lz_NSCR,
                                   MBMAIN_BGF_BG_S, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramPalette( ah, NARC_mail_gra_mailbox_u_NCLR, 
                                    PALTYPE_SUB_BG, 0, 0, HEAPID_MAILBOX_APP );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( ah, NARC_mail_gra_mailbox_d_lz_NCGR,
                                        MBMAIN_BGF_BG_M, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( ah, NARC_mail_gra_mailbox_d_lz_NCGR,
                                        MBMAIN_BGF_STR_M, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen( ah, NARC_mail_gra_mailbox_d_lz_NSCR,
                                   MBMAIN_BGF_BG_M, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramPalette( ah, NARC_mail_gra_mailbox_d_NCLR, 
                                    PALTYPE_MAIN_BG, 0, 0, HEAPID_MAILBOX_APP );

  { // �{�^���w�i�擾
    NNSG2dCharacterData * chr;
    void * buf;
    u8 * cgx;

    buf = GFL_ARCHDL_UTIL_LoadBGCharacter(
        ah, NARC_mail_gra_mailbox_d_lz_NCGR, 1, &chr, HEAPID_MAILBOX_APP );
    cgx = chr->pRawData;
    MI_CpuCopyFast( &cgx[BTN_U_BACK_CGX_POS], &syswk->app->btn_back_cgx[0x00], 0x20 );
    MI_CpuCopyFast( &cgx[BTN_D_BACK_CGX_POS], &syswk->app->btn_back_cgx[0x20], 0x20 );
    GFL_HEAP_FreeMemory( buf );
  }

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * �E�B���h�E�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_WindowLoad( MAILBOX_SYS_WORK * syswk )
{
  MAILBOX_APP_WORK * appwk;

  appwk = syswk->app;

  // ��b�E�B���h�E
//  TalkWinGraphicSet(
//    appwk->bgl, MBMAIN_BGF_MSG_M,
//    MBMAIN_TALKWIN_CGX_POS, MBMAIN_MBG_PAL_TALKWIN, 0, HEAPID_MAILBOX_APP );
//  TalkFontPaletteLoad( PALTYPE_MAIN_BG, MBMAIN_MBG_PAL_TALKFNT * 32, HEAPID_MAILBOX_APP );
  BmpWinFrame_GraphicSet( MBMAIN_BGF_MSG_M, MBMAIN_TALKWIN_CGX_POS, MBMAIN_MBG_PAL_TALKWIN, 
                          MENU_TYPE_SYSTEM, HEAPID_MAILBOX_APP );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 MBMAIN_MBG_PAL_TALKFNT * 32, 32, HEAPID_MAILBOX_APP);

  // �V�X�e���t�H���g�p���b�g
//  SystemFontPaletteLoad( PALTYPE_MAIN_BG, MBMAIN_MBG_PAL_SYSFNT * 32, HEAPID_MAILBOX_APP );
//  SystemFontPaletteLoad( PALTYPE_SUB_BG, MBMAIN_SBG_PAL_SYSFNT * 32, HEAPID_MAILBOX_APP );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 MBMAIN_MBG_PAL_SYSFNT * 32, 32, HEAPID_MAILBOX_APP);
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 MBMAIN_SBG_PAL_SYSFNT * 32,32, HEAPID_MAILBOX_APP);

}

//--------------------------------------------------------------------------------------------
/**
 * BGWINFRM������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_BgWinFrmInit( MAILBOX_SYS_WORK * syswk )
{
  u16 * scrn1;
  u16 * scrn2;
  u32 i;

  syswk->app->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, MBMAIN_BGWF_MAX, HEAPID_MAILBOX_APP );

  // ���[���I���{�^��
  for( i=0; i<MBMAIN_MAILLIST_MAX; i++ ){
    BGWINFRM_Add(
      syswk->app->wfrm, MBMAIN_BGWF_BUTTON1+i,
      MBMAIN_BGF_BTN_M, BGWF_MAILBTN_SX, BGWF_MAILBTN_SY );
  }

  // �I�����ꂽ���[��
  BGWINFRM_Add(
    syswk->app->wfrm, MBMAIN_BGWF_SELMAIL,
    MBMAIN_BGF_STR_M, BGWF_MAILBTN_SX, BGWF_MAILBTN_SY );


  // �X�N���[���f�[�^���[�h
  BGWINFRM_FrameSetArc(
    syswk->app->wfrm, MBMAIN_BGWF_BUTTON1,
    ARCID_MAIL_GRA, NARC_mail_gra_mailbox_win_lz_NSCR, TRUE );

  scrn1 = BGWINFRM_FrameBufGet( syswk->app->wfrm, MBMAIN_BGWF_BUTTON1 );

  for( i=MBMAIN_BGWF_BUTTON1+1; i<=MBMAIN_BGWF_SELMAIL; i++ ){
    scrn2 = BGWINFRM_FrameBufGet( syswk->app->wfrm, i );
    MI_CpuCopy8( scrn1, scrn2, BGWF_MAILBTN_SX*BGWF_MAILBTN_SY*2 );
  }

  // �{�^���z�u
  for( i=0; i<MBMAIN_MAILLIST_MAX; i++ ){
    BGWINFRM_FramePut(
      syswk->app->wfrm, MBMAIN_BGWF_BUTTON1+i,
      BGWF_MAILBTN_PX + ( BGWF_MAILBTN_SPACE_X * (i&1) ),
      BGWF_MAILBTN_PY + ( BGWF_MAILBTN_SPACE_Y * (i>>1) ) );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * BGWINFRM���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_BgWinFrmExit( MAILBOX_SYS_WORK * syswk )
{
  BGWINFRM_Exit( syswk->app->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MsgInit( MAILBOX_APP_WORK * appwk )
{
  appwk->mman   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                  NARC_message_mailbox_dat, HEAPID_MAILBOX_APP );
  appwk->wset   = WORDSET_Create( HEAPID_MAILBOX_APP );
  appwk->expbuf = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_MAILBOX_APP );

}

//--------------------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�֘A���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MsgExit( MAILBOX_APP_WORK * appwk )
{
  GFL_STR_DeleteBuffer( appwk->expbuf );
  WORDSET_Delete( appwk->wset );
  GFL_MSG_Delete( appwk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E����������������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_YesNoWinInit( MAILBOX_SYS_WORK * syswk )
{
//  syswk->app->tsw = TOUCH_SW_AllocWork( HEAPID_MAILBOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_YesNoWinExit( MAILBOX_SYS_WORK * syswk )
{
//  TOUCH_SW_FreeWork( syswk->app->tsw );
}

//--------------------------------------------------------------------------------------------
/**
 * �͂��E�������Z�b�g
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_YesNoWinSet( MAILBOX_SYS_WORK * syswk )
{
  APP_TASKMENU_INITWORK init;

  init.heapId   = HEAPID_MAILBOX_APP;
  init.itemNum  = 2;
  init.itemWork = syswk->app->yn_menuitem;
  init.posType  = ATPT_LEFT_UP;
  init.charPosX = 21; //�E�B���h�E�J�n�ʒu(�L�����P��
  init.charPosY =  8;
  init.w = 10;  //�L�����P��
  init.h =  3;  //�L�����P��
  

  syswk->app->menuwork = APP_TASKMENU_OpenMenu( &init, syswk->app->menures );
/*
  TOUCH_SW_PARAM  prm;

  prm.bg_frame  = MBMAIN_BGF_MSG_M;
  prm.char_offs = MBMAIN_YNWIN_CGX_POS;
  prm.pltt_offs = MBMAIN_MBG_PAL_YNWIN;
  prm.x         = 25;
  prm.y         = 10;
  prm.kt_st     = 0;
  prm.key_pos   = 0;
  prm.type      = TOUCH_SW_TYPE_S;

  TOUCH_SW_Init( syswk->app->tsw, &prm );
*/
}


static const int menu_item[][2]={
  { msg_menu01, APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_menu02, APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_menu03, APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_menu04, APP_TASKMENU_WIN_TYPE_RETURN },
};

static const int yn_item[][2]={
  { msg_mailbox_yes,  APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_mailbox_no,   APP_TASKMENU_WIN_TYPE_NORMAL },
};

#define MAILBOX_MENU01_X  ( 19 )
#define MAILBOX_MENU01_Y  ( 19 )
#define MAILBOX_MENU01_W  (  3 )
//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�쐬
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_SelBoxInit( MAILBOX_SYS_WORK * syswk )
{
  u32 max;
  u16 i, j;

  // �^�X�N���j���[���\�[�X�]��
  syswk->app->menures = APP_TASKMENU_RES_Create( GFL_BG_FRAME0_M, 5, syswk->font, 
                                                 syswk->app->printQue, HEAPID_MAILBOX_APP );

  // ���j���[���ړo�^(��ށE�����E��������E��߂�j
  for(i=0;i<SELBOX_LIST_NUM;i++){
    syswk->app->menuitem[i].str      = GFL_MSG_CreateString( syswk->app->mman, menu_item[i][0] ); //���j���[�ɕ\�����镶����
    syswk->app->menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    syswk->app->menuitem[i].type     = menu_item[i][1]; //�߂�}�[�N�̕\��
  }
  
  // �͂��E������
  for(i=0;i<2;i++){
    syswk->app->yn_menuitem[i].str      = GFL_MSG_CreateString( syswk->app->mman, yn_item[i][0] ); //���j���[�ɕ\�����镶����
    syswk->app->yn_menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
    syswk->app->yn_menuitem[i].type     = yn_item[i][1];
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�폜
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_SelBoxExit( MAILBOX_SYS_WORK * syswk )
{
  int i;
//  BmpMenuWork_ListDelete( syswk->app->sblist );
//  SelectBoxSys_Free( syswk->app->selbox );


  for(i=0;i<SELBOX_LIST_NUM;i++){
    GFL_STR_DeleteBuffer(syswk->app->menuitem[i].str);
  }
  GFL_STR_DeleteBuffer(syswk->app->yn_menuitem[0].str);
  GFL_STR_DeleteBuffer(syswk->app->yn_menuitem[1].str);

  APP_TASKMENU_RES_Delete( syswk->app->menures );

}

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_SelBoxPut( MAILBOX_SYS_WORK * syswk )
{
  APP_TASKMENU_INITWORK init;

  init.heapId   = HEAPID_MAILBOX_APP;
  init.itemNum  = 4;
  init.itemWork = syswk->app->menuitem;
  init.posType  = ATPT_LEFT_UP;
  init.charPosX = 20; //�E�B���h�E�J�n�ʒu(�L�����P��
  init.charPosY = 12;
  init.w = 12;  //�L�����P��
  init.h =  3;  //�L�����P��
  

  syswk->app->menuwork = APP_TASKMENU_OpenMenu( &init, syswk->app->menures );
/*  
  SELBOX_HEADER hd;

  hd.prm   = SelBoxHedParam;
  hd.list  = syswk->app->sblist;

  syswk->app->sbwk = SelectBoxSet(
              syswk->app->selbox, &hd, 0,
              SELBOX_PX, SELBOX_PY_4, SELBOX_SX, 0 );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * �I���{�b�N�X��\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_SelBoxDel( MAILBOX_SYS_WORK * syswk )
{
//  SelectBoxExit( syswk->app->sbwk );
  APP_TASKMENU_CloseMenu( syswk->app->menuwork );
}

#ifdef MAIL_DEBUG
static const STRCODE debug_name[][20]={ 
  {0x3092,0x3093,0x3094,0x3095,0x3096,0xffff },
  {0x3097,0x3098,0x3099,0x309a,0x309b,0xffff },
  {0x309c,0x309d,0x309e,0x309f,0x30a0,0xffff },
  {0x30a1,0x30a2,0x30a3,0x30a4,0x30a5,0xffff },
  {0x3092,0x3093,0x3094,0x3095,0x3096,0xffff },
  {0x3097,0x3098,0x3099,0x309a,0x309b,0xffff },
  {0x309c,0x309d,0x309e,0x309f,0x30a0,0xffff },
  {0x30a1,0x30a2,0x30a3,0x30a4,0x30a5,0xffff },
  {0x3092,0x3093,0x3094,0x3095,0x3096,0xffff },
  {0x3097,0x3098,0x3099,0x309a,0x309b,0xffff },
  {0x309c,0x309d,0x309e,0x309f,0x30a0,0xffff },
  {0x30a1,0x30a2,0x30a3,0x30a4,0x30a5,0xffff },
  {0x3092,0x3093,0x3094,0x3095,0x3096,0xffff },
  {0x3097,0x3098,0x3099,0x309a,0x309b,0xffff },
  {0x309c,0x309d,0x309e,0x309f,0x30a0,0xffff },
  {0x30a1,0x30a2,0x30a3,0x30a4,0x30a5,0xffff },
  {0x3092,0x3093,0x3094,0x3095,0x3096,0xffff },
  {0x3097,0x3098,0x3099,0x309a,0x309b,0xffff },
  {0x309c,0x309d,0x309e,0x309f,0x30a0,0xffff },
  {0x30a1,0x30a2,0x30a3,0x30a4,0x30a5,0xffff },

};

static const u8 debug_sex[]={
  1,0,1,1,0,1,1,0,1,0, 1,1,0,0,1,0,1,0,1,0,0,1,0,1,0,
};
#endif

//--------------------------------------------------------------------------------------------
/**
 * ���[���f�[�^�擾
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MailDataInit( MAILBOX_SYS_WORK * syswk )
{
  u32 i;

  syswk->app->mail_max = 0;

  for( i=0; i<MAIL_STOCK_MAX; i++ ){
    syswk->app->mail_list[i] = MBMAIN_MAILLIST_NULL;

    syswk->app->mail[i] = MAIL_AllocMailData(
                syswk->sv_mail, MAILBLOCK_PASOCOM, i, HEAPID_MAILBOX_APP );

#ifdef MAIL_DEBUG
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
      MailData_Copy( syswk->app->mail[0],syswk->app->mail[i]);
      if(i>0){
          MailData_SetWriterSex( syswk->app->mail[i], debug_sex[i] );
          MailData_SetWriterName( syswk->app->mail[i], (STRCODE*)debug_name[i]);
      }
    }
#endif

    // ���[�����X�g�쐬
    if( MailData_IsEnable(syswk->app->mail[i]) == TRUE ){
      syswk->app->mail_list[syswk->app->mail_max] = i;
      syswk->app->mail_max++;
    }
  }

  if( syswk->app->mail_max == 0 ){
    syswk->app->page_max = 0;
  }else{
    syswk->app->page_max = (syswk->app->mail_max-1) / MBMAIN_MAILLIST_MAX;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���f�[�^���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MailDataExit( MAILBOX_SYS_WORK * syswk )
{
  u32 i;

  for( i=0; i<MAIL_STOCK_MAX; i++ ){
    GFL_HEAP_FreeMemory( syswk->app->mail[i] );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���I���{�^��������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MailButtonInit( MAILBOX_SYS_WORK * syswk )
{
  u8 * lst;
  u16 pos;
  u16 win;
  u16 frm;
  u16 i;

  pos = syswk->sel_page * MBMAIN_MAILLIST_MAX;
  lst = &syswk->app->mail_list[ pos ];
  win = MBMAIN_BMPWIN_ID_MAIL01 + pos;
  frm = MBMAIN_BGWF_BUTTON1;

  for( i=0; i<MBMAIN_MAILLIST_MAX; i++ ){
    if( lst[i] == MBMAIN_MAILLIST_NULL ){
      BGWINFRM_FrameOff( syswk->app->wfrm, frm );
    }else{
      MBBMP_MailButtonSet( syswk, frm, win, lst[i] );
      BGWINFRM_FrameOn( syswk->app->wfrm, frm );
    }
    win++;
    frm++;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �I���������[���̃^�u�\��
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_SelMailTabPut( MAILBOX_SYS_WORK * syswk )
{
  MBBMP_MailButtonSet(
    syswk, MBMAIN_BGWF_SELMAIL,
    MBMAIN_BMPWIN_ID_SELMAIL, syswk->lst_pos );

  BGWINFRM_FramePut( syswk->app->wfrm, MBMAIN_BGWF_SELMAIL, BGWF_SELMAIL_PX, BGWF_SELMAIL_PY );
}

//--------------------------------------------------------------------------------------------
/**
 * �{�^���A�j������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @retval  "TRUE = �A�j����"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL MBMAIN_ButtonAnmMain( MAILBOX_SYS_WORK * syswk )
{
  BUTTON_ANM_WORK * bawk = &syswk->app->bawk;

  switch( bawk->btn_seq ){
  case 0:
    if( bawk->btn_mode == MBMAIN_BTN_ANM_MODE_OBJ ){
      GFL_CLACT_WK_SetPlttOffs( syswk->app->clwk[bawk->btn_id], bawk->btn_pal1, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      // �_����������3�Ԗڂ̈�����CLWK_PLTTOFFS_MODE_OAM_COLOR��
    }else{
      GFL_BG_ChangeScreenPalette( bawk->btn_id, 
                                  bawk->btn_px, bawk->btn_py, bawk->btn_sx, bawk->btn_sy, 
                                  bawk->btn_pal1 );
      GFL_BG_LoadScreenV_Req( bawk->btn_id );
    }
    bawk->btn_seq++;
    break;

  case 1:
    bawk->btn_cnt++;
    if( bawk->btn_cnt != 4 ){ break; }
    if( bawk->btn_mode == MBMAIN_BTN_ANM_MODE_OBJ ){
      GFL_CLACT_WK_SetPlttOffs( syswk->app->clwk[bawk->btn_id], bawk->btn_pal2, CLWK_PLTTOFFS_MODE_PLTT_TOP );
      // �_����������3�Ԗڂ̈�����CLWK_PLTTOFFS_MODE_OAM_COLOR��
    }else{
      GFL_BG_ChangeScreenPalette( bawk->btn_id,
                                  bawk->btn_px, bawk->btn_py, bawk->btn_sx, bawk->btn_sy, 
                                  bawk->btn_pal2 );
      GFL_BG_LoadScreenV_Req( bawk->btn_id );
    }
    bawk->btn_cnt = 0;
    bawk->btn_seq++;
    break;

  case 2:
    bawk->btn_cnt++;
    if( bawk->btn_cnt == 2 ){
      return FALSE;
    }
  }

  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���O���t�B�b�N�ǂݍ���
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void MBMAIN_MailGraphcLoad( MAILBOX_SYS_WORK * syswk )
{
  ARCHANDLE * ah;
  u32 pat;

  ah  = GFL_ARC_OpenDataHandle( ARCID_MAIL_GRA, HEAPID_MAILBOX_APP );
  pat = MailData_GetDesignNo( syswk->app->mail[ syswk->lst_pos ] );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( ah, NARC_mail_gra_mail_000_lz_ncgr+pat,
                                        MBMAIN_BGF_MAIL_S, 0, 0, TRUE, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramPalette( ah, NARC_mail_gra_mail_000_nclr+pat, 
                                    PALTYPE_SUB_BG, 0, 0x20*2, HEAPID_MAILBOX_APP );
  GFL_ARCHDL_UTIL_TransVramScreen( ah, NARC_mail_gra_mail_000_lz_nscr+pat,
                                   MBMAIN_BGF_MAIL_S, 0, 0, TRUE, HEAPID_MAILBOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

#define PMS_WORD_CLACT_MAX    ( 8 )

//=============================================================================================
/**
 * @brief �ȈՉ�b�`��V�X�e��������
 *
 * @param   syswk   ���[���{�b�N�X�V�X�e�����[�N
 */
//=============================================================================================
void MBMAIN_PmsDrawInit( MAILBOX_SYS_WORK * syswk )
{
  syswk->app->pmsPrintque = PRINTSYS_QUE_Create( HEAPID_MAILBOX_APP );
  syswk->app->pmsClunit   = GFL_CLACT_UNIT_Create( PMS_WORD_CLACT_MAX, 2, HEAPID_MAILBOX_APP );
  syswk->app->pms_draw_work = PMS_DRAW_Init( syswk->app->pmsClunit, CLSYS_DRAW_SUB, 
                                           syswk->app->pmsPrintque, syswk->font, 
                                           4, 4, HEAPID_MAILBOX_APP );
  // �w�i�J���[�𓧖��F�w��ɕύX
  PMS_DRAW_SetNullColorPallet( syswk->app->pms_draw_work, 0 );
}


//=============================================================================================
/**
 * @brief �ȈՉ�b�`��V�X�e�����
 *
 * @param   syswk   ���[���{�b�N�X�V�X�e�����[�N
 */
//=============================================================================================
void MBMAIN_PmsDrawExit( MAILBOX_SYS_WORK * syswk )
{
  PMS_DRAW_Exit( syswk->app->pms_draw_work );
  PRINTSYS_QUE_Delete( syswk->app->pmsPrintque );
  GFL_CLACT_UNIT_Delete( syswk->app->pmsClunit );
  
}
