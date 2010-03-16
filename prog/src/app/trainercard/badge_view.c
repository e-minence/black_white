//============================================================================================
/**
 * @file    badge_view.c
 * @brief   �o�b�W�{�����
 * @author  Akito Mori
 * @date    10.03.02
 *
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"            // HEAPID_LEADERBOARD
#include "system/cursor_move.h"     // CURSOR_MOVE
#include "system/gfl_use.h"         // GFUser_VIntr_CreateTCB
#include "system/wipe.h"            // WIPE_SYS_Start,
#include "sound/pm_sndsys.h"        // PMSND_PlaySE
#include "print/wordset.h"          // WORDSET
#include "print/printsys.h"         // PRINT_UTIL,PRINT_QUE...
#include "app/leader_board.h"       // proc
#include "app/app_menu_common.h"
#include "arc/arc_def.h"            // ARCID_LEADER_BOARD
#include "arc/trainer_case.naix"    // leaderboard graphic

#include "font/font.naix"           // NARC_font_large_gftr
#include "arc/message.naix"         // NARC_message_leader_board_dat
#include "msg/msg_trainercard.h"

#include "badge_view_def.h"       // �e���`��

#ifdef PM_DEBUG
#define SPECIAL_FEBRUARY
#endif

//--------------------------------------------------------------------------------------------
// �萔��`
//--------------------------------------------------------------------------------------------


// �V�[�P���X����
enum{
  SUBSEQ_FADEIN_WAIT=0,
  SUBSEQ_MAIN,
  SUBSEQ_ICON_WAIT,
  SUBSEQ_FADEOUT,
  SUBSEQ_FADEOUT_WAIT,
};


// BG�w��
#define BV_BGFRAME_U_MSG    ( GFL_BG_FRAME0_S )
#define BV_BGFRAME_U_BG     ( GFL_BG_FRAME1_S )
#define BV_BGFRAME_D_MSG    ( GFL_BG_FRAME0_M )
#define BV_BGFRAME_D_BUTTON ( GFL_BG_FRAME1_M )
#define BV_BGFRAME_D_BG     ( GFL_BG_FRAME2_M )



//--------------------------------------------------------------------------------------------
// �\���̒�`��`
//--------------------------------------------------------------------------------------------
typedef struct {
  ARCHANDLE     *g_handle;  // �O���t�B�b�N�f�[�^�t�@�C���n���h��
  
  GFL_FONT      *font;                // �t�H���g�f�[�^
  GFL_BMPWIN    *InfoWin;
  PRINT_UTIL    printUtil;
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;
  GFL_TCBLSYS   *pMsgTcblSys;

  GFL_MSGDATA   *mman;    // ���b�Z�[�W�f�[�^�}�l�[�W��
  WORDSET       *wset;    // �P��Z�b�g
  STRBUF        *expbuf;  // ���b�Z�[�W�W�J�̈�
  STRBUF        *strbuf[BV_STR_MAX];

  GFL_CLUNIT    *clUnit;             // �Z���A�N�^�[���j�b�g
  GFL_CLWK      *clwk[BV_OBJ_MAX];   // �Z���A�N�^�[���[�N
  u32           clres[BV_RES_MAX];   // �Z���A�N�^�[���\�[�X�p�C���f�b�N�X

  GFL_TCB         *VblankTcb;       // �o�^����VblankFunc�̃|�C���^
  
  LEADERBOARD_PARAM *param;         // �Ăяo���p�����[�^
  int seq,next_seq;                 // �T�u�V�[�P���X����p���[�N
  u16 page,page_max;                // ���݂̕\���y�[�W

  int trainer_num;                  // �o�g���T�u�E�F�C�f�[�^�ɑ��݂��Ă����l��

#ifdef PM_DEBUG
  GFL_MSGDATA   *debugname;
#endif
} BADGEVIEW_WORK;

GFL_PROC_RESULT BadgeViewProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
GFL_PROC_RESULT BadgeViewProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
GFL_PROC_RESULT BadgeViewProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );


//--------------------------------------------------------------------------------------------
// ���f�[�^��`
//--------------------------------------------------------------------------------------------
// �v���Z�X��`�f�[�^
const GFL_PROC_DATA BadgeViewProcData = {
  BadgeViewProc_Init,
  BadgeViewProc_Main,
  BadgeViewProc_End,
};



//--------------------------------------------------------------------------------------------
// �֐��v���g�^�C�v�錾
//--------------------------------------------------------------------------------------------
/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb, void * work );
static void InitWork( BADGEVIEW_WORK *wk, void *pwk );
static void FreeWork( BADGEVIEW_WORK *wk );
static void VramBankSet(void);
static void BgInit(BADGEVIEW_WORK *wk);
static void BgExit(BADGEVIEW_WORK *wk) ;
static void BgGraphicInit(BADGEVIEW_WORK *wk);
static void BgGraphicExit( BADGEVIEW_WORK *wk );
static void ClActInit(BADGEVIEW_WORK *wk);
static void ClActSet(BADGEVIEW_WORK *wk) ;
static void ClActExit(BADGEVIEW_WORK *wk);
static void BmpWinInit(BADGEVIEW_WORK *wk);
static void BmpWinExit(BADGEVIEW_WORK *wk);
static void PrintSystemInit(BADGEVIEW_WORK *wk);
static void PrintSystemDelete(BADGEVIEW_WORK *wk);
static void PrintSystem_Main( BADGEVIEW_WORK *wk );
static void BgFramePrint( BADGEVIEW_WORK *wk, int id, STRBUF *str, STRBUF *str2, STRBUF *str3 );
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );
static BOOL LBSEQ_Main( BADGEVIEW_WORK *wk );
static void Rewrite_UpperInformation( BADGEVIEW_WORK *wk, int id );
static  int TouchBar_KeyControl( BADGEVIEW_WORK *wk );
static void ExecFunc( BADGEVIEW_WORK *wk, int touch );
static void _page_max_init( BADGEVIEW_WORK *wk );
static void _page_clact_set( BADGEVIEW_WORK *wk, int page, int max );
static int _page_move_check( BADGEVIEW_WORK *wk, int touch );
static void SetupPage( BADGEVIEW_WORK *wk, int page );
static void NamePlatePrint_1Page( BADGEVIEW_WORK *wk );
static void CellActorPosSet( BADGEVIEW_WORK *wk, int id, int x, int y );


#ifdef PM_DEBUG
static void _debug_data_set( BADGEVIEW_WORK *wk );
#endif


//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT BadgeViewProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  BADGEVIEW_WORK * wk;

  OS_Printf( "�����������@���[�_�[�{�[�h�����J�n�@����������\n" );

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_LEADERBOARD, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(BADGEVIEW_WORK), HEAPID_LEADERBOARD );
  MI_CpuClearFast( wk, sizeof(BADGEVIEW_WORK) );

  WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
  WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );

  InitWork( wk, pwk );  // ���[�N������
  VramBankSet();        // VRAM�ݒ�
  BgInit(wk);           // BG�V�X�e��������
  BgGraphicInit(wk);    // BG�O���t�B�b�N�]��
  ClActInit(wk);        // �Z���A�N�^�[�V�X�e��������
  ClActSet(wk);         // �Z���A�N�^�[�o�^
  BmpWinInit(wk);       // BMPWIN������
  PrintSystemInit(wk);  // �`��V�X�e��������

  // �����y�[�W�\��
  _page_clact_set( wk, wk->page, wk->page_max );
  SetupPage( wk, wk->page );


  // �t�F�[�h�C���J�n
  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
                  WIPE_FADE_BLACK, 16, 1, HEAPID_LEADERBOARD );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT BadgeViewProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  BADGEVIEW_WORK * wk = mywk;

  if( LBSEQ_Main( wk ) == FALSE ){
    return GFL_PROC_RES_FINISH;
  }
  
  PrintSystem_Main( wk );         // ������`�惁�C��
  GFL_CLACT_SYS_Main();           // �Z���A�N�^�[���C��
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT BadgeViewProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  BADGEVIEW_WORK * wk = mywk;
  
  PrintSystemDelete(wk);  // �`��V�X�e�����
  BmpWinExit(wk);         // BMPWIN���
  ClActExit(wk);          // �Z���A�N�^�[�V�X�e�����
  BgGraphicExit(wk);      // BG�O���t�B�b�N�֘A�I��
  BgExit(wk);             // BG�V�X�e���I��
  FreeWork(wk);           // ���[�N���

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_CheckHeapSafe( HEAPID_LEADERBOARD );
  GFL_HEAP_DeleteHeap( HEAPID_LEADERBOARD );


  OS_Printf( "�����������@���[�_�[�{�[�h�����I���@����������\n" );

  return GFL_PROC_RES_FINISH;
}


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * �֐�
 *
 * @param none
 *
 * @return  none
 */
//----------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb, void * work )
{
  // �Z���A�N�^�[
  GFL_CLACT_SYS_VBlankFunc();

  // BG�]��  
  GFL_BG_VBlankFunc();
  
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[�N������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void InitWork( BADGEVIEW_WORK *wk, void *pwk )
{
  wk->param     = pwk;
  wk->next_seq  = SUBSEQ_FADEIN_WAIT;

  // �O���t�B�b�N�f�[�^�n���h���I�[�v��
  wk->g_handle  = GFL_ARC_OpenDataHandle( ARCID_TRAINERCARD, HEAPID_LEADERBOARD);

  // ���b�Z�[�W�}�l�[�W���[�m��  
  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_trainercard_dat, HEAPID_LEADERBOARD );


  // leader_board.gmm�̕������S�ēǂݍ���
  {
    int i;
    const STRCODE *buf;
    for(i=0;i<BV_STR_MAX;i++){
      wk->strbuf[i] = GFL_MSG_CreateString( wk->mman, i );
      buf = GFL_STR_GetStringCodePointer( wk->strbuf[i] );
      OS_Printf("str%d=%02d, %02d, %02d, %02d, %02d,\n", i,buf[0],buf[1],buf[2],buf[3],buf[4]);
    }
  }
  // �W�J�p������m��
  wk->expbuf = GFL_STR_CreateBuffer( BUFLEN_PMS_WORD*2, HEAPID_LEADERBOARD );

  // ���[�h�Z�b�g���[�N�m��
  wk->wset = WORDSET_CreateEx( 8, WORDSET_COUNTRY_BUFLEN, HEAPID_LEADERBOARD );
  
  
  // VBlank�֐��Z�b�g
  wk->VblankTcb = GFUser_VIntr_CreateTCB( VBlankFunc, wk, 0 );  
  


#ifdef PM_DEBUG
  wk->debugname = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_trname_dat, HEAPID_LEADERBOARD );

  // �f�o�b�O�p�f�[�^�Z�b�g
  _debug_data_set( wk );

  GFL_MSG_Delete( wk->debugname );

#endif
  
  // �g���[�i�[�������グ
  wk->trainer_num = 8;

  // �ő�y�[�W���擾
  _page_max_init(wk);
}


//----------------------------------------------------------------------------------
/**
 * @brief ���[�N���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void FreeWork( BADGEVIEW_WORK *wk )
{
  int i;
  
  
  // ��������
  for(i=0;i<BV_STR_MAX;i++){
    GFL_STR_DeleteBuffer( wk->strbuf[i] );
  }
  GFL_STR_DeleteBuffer( wk->expbuf );

  // ���[�h�Z�b�g���
  WORDSET_Delete( wk->wset );

  // ���b�Z�[�W�f�[�^���
  GFL_MSG_Delete( wk->mman );


  // Vblank���Ԓ���BG�]���I��
  GFL_TCB_DeleteTask( wk->VblankTcb );

  GFL_ARC_CloseDataHandle( wk->g_handle );
}

//============================
// VRAM����U��
//============================
static const GFL_DISP_VRAM VramTbl = {
  GX_VRAM_BG_128_B,           // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_64_E,           // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_0_A,            // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_32K, // ���C���n�a�i�̈�
  GX_OBJVRAMMODE_CHAR_1D_32K, // �T�u�n�a�i�̈�
};

//----------------------------------------------------------------------------------
/**
 * @brief VRAM�ݒ�
 *
 * @param   none    
 */
//----------------------------------------------------------------------------------
static void VramBankSet(void)
{
  GFL_DISP_SetBank( &VramTbl );

  // ���C����ʂ����ɕ\��
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

}


// ���������s��BG��
static const int bgframe_init_tbl[]={
  BV_BGFRAME_U_MSG,
  BV_BGFRAME_U_BG,
  BV_BGFRAME_D_MSG,
  BV_BGFRAME_D_BUTTON,
  BV_BGFRAME_D_BG,
};

//----------------------------------------------------------------------------------
/**
 * @brief BG�V�X�e��������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgInit(BADGEVIEW_WORK *wk)
{
  int i;

  // BG SYSTEM������
  GFL_BG_Init( HEAPID_LEADERBOARD );

  // BG���[�h�ݒ�
  { 
    GFL_BG_SYS_HEADER sysh = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysh );
  }

  {
    // �eBG�������ݒ�
    const GFL_BG_BGCNT_HEADER header[]={
      { // �T�u��ʁF������
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01,      0, 0, 0, FALSE
      },
      { // �T�u��ʁF�w�i
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,    1, 0, 0, FALSE
      },
      { // ���C����ʁF����
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01,      0, 0, 0, FALSE
      },
      { // ���C����ʁF�{�^��
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,      2, 0, 0, FALSE
      },
      { // ���C����ʁF�w�i
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,      3, 0, 0, FALSE
      },
    };

    GF_ASSERT( NELEMS(header)==NELEMS(bgframe_init_tbl) );

    // BG�ݒ�E�X�N���[���N���A�E�O�L�����ڃN���A
    for(i=0;i<NELEMS(bgframe_init_tbl);i++)
    {
      GFL_BG_SetBGControl( bgframe_init_tbl[i], &header[i], GFL_BG_MODE_TEXT );
      GFL_BG_ClearScreen(  bgframe_init_tbl[i] );
      GFL_BG_SetClearCharacter( bgframe_init_tbl[i], 0x20, 0, HEAPID_LEADERBOARD );
      GFL_BG_SetVisible(  bgframe_init_tbl[i], VISIBLE_ON     );
    }
    
    // �c���BG�ʂ�OFF��
    GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
  }
}
//----------------------------------------------------------------------------------
/**
 * @brief  BG�V�X�e���I��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgExit(BADGEVIEW_WORK *wk) 
{
  int i;
  for(i=0;i<NELEMS(bgframe_init_tbl);i++){
    GFL_BG_FreeBGControl( bgframe_init_tbl[i] );
  }
  GFL_BG_Exit();

}

//----------------------------------------------------------------------------------
/**
 * @brief BG�O���t�B�b�N�]��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgGraphicInit(BADGEVIEW_WORK *wk)
{
  ARCHANDLE * handle = wk->g_handle;

  // �T�u��ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg_02_NCGR,
                                        BV_BGFRAME_U_BG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg03_NSCR,
                                        BV_BGFRAME_U_BG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_trainer_case_badge_bg02_NCLR, 
                                        PALTYPE_SUB_BG, 0, 0, HEAPID_LEADERBOARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg01_NCGR,
                                        BV_BGFRAME_D_BG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg02_NSCR,
                                        BV_BGFRAME_D_BG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_trainer_case_badge_bg01_NCLR, 
                                        PALTYPE_MAIN_BG, 0, 0, HEAPID_LEADERBOARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg01_NCGR,
                                        BV_BGFRAME_D_MSG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg01_NSCR,
                                        BV_BGFRAME_D_MSG, 0, 0, FALSE, HEAPID_LEADERBOARD );
  
  // �^�b�`�o�[BG�]��
  {
    ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_LEADERBOARD);

    GFL_ARCHDL_UTIL_TransVramBgCharacter( c_handle, APP_COMMON_GetBarCharArcIdx(),
                                          BV_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_LEADERBOARD );
    GFL_ARCHDL_UTIL_TransVramScreen(      c_handle, APP_COMMON_GetBarScrnArcIdx(),
                                          BV_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_LEADERBOARD );
    GFL_ARCHDL_UTIL_TransVramPaletteEx(   c_handle, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
                                          0, TOUCHBAR_PAL*32, 32, HEAPID_LEADERBOARD );
    
    // �X�N���[���̃p���b�g�w���ύX
    GFL_BG_ChangeScreenPalette( BV_BGFRAME_D_BUTTON, TOUCHBAR_X, TOUCHBAR_Y,
                                                     TOUCHBAR_W, TOUCHBAR_H, TOUCHBAR_PAL );
    GFL_BG_LoadScreenReq( BV_BGFRAME_D_BUTTON );

    GFL_ARC_CloseDataHandle( c_handle );
  }
  
  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_LEADERBOARD );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_LEADERBOARD );


}


//----------------------------------------------------------------------------------
/**
 * @brief BG�֘A�V�X�e���I������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgGraphicExit( BADGEVIEW_WORK *wk )
{
}

//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e��������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ClActInit(BADGEVIEW_WORK *wk)
{
  ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_LEADERBOARD);

  // �Z���A�N�^�[������
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &VramTbl, HEAPID_LEADERBOARD );
  wk->clUnit  = GFL_CLACT_UNIT_Create( BV_CLACT_NUM, 1,  HEAPID_LEADERBOARD );

  // OBJ�ʕ\��ON
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // �Z���A�N�^�[���\�[�X�ǂݍ���
  
  // ---���[�_�[�{�[�h�p�����---
  wk->clres[BV_RES_CHR]  = GFL_CLGRP_CGR_Register(      wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCGR, 0, 
                                                        CLSYS_DRAW_MAIN, HEAPID_LEADERBOARD );
  wk->clres[BV_RES_PLTT] = GFL_CLGRP_PLTT_Register(     wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCLR, 
                                                        CLSYS_DRAW_MAIN, 0, HEAPID_LEADERBOARD );
  wk->clres[BV_RES_CELL] = GFL_CLGRP_CELLANIM_Register( wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCER, 
                                                        NARC_trainer_case_badge_obj01_NANR, 
                                                        HEAPID_LEADERBOARD );

  // ���ʃ��j���[�f��
  wk->clres[BV_RES_COMMON_CHR] = GFL_CLGRP_CGR_Register(       c_handle, 
                                                               APP_COMMON_GetBarIconCharArcIdx(), 0, 
                                                               CLSYS_DRAW_MAIN, 
                                                               HEAPID_LEADERBOARD );

  wk->clres[BV_RES_COMMON_PLTT] = GFL_CLGRP_PLTT_RegisterEx(   c_handle, 
                                                               APP_COMMON_GetBarIconPltArcIdx(), 
                                                               CLSYS_DRAW_MAIN, 
                                                               11*32, 0, 3, HEAPID_LEADERBOARD );

  wk->clres[BV_RES_COMMON_CELL] = GFL_CLGRP_CELLANIM_Register( c_handle, 
                                                               APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K), 
                                                               APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                                               HEAPID_LEADERBOARD );
  GFL_ARC_CloseDataHandle( c_handle );

}


static const int clact_dat[][6]={
  //   X      Y  anm, chr,               pltt,               cell
  {  1*8,  21*8,  11, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �J�[�h�ɖ߂�
  { 28*8,  21*8,   1, BV_RES_COMMON_CHR, BV_RES_COMMON_CHR,  BV_RES_COMMON_CHR },// ���ǂ�
  { 24*8,  21*8,   0, BV_RES_COMMON_CHR, BV_RES_COMMON_CHR,  BV_RES_COMMON_CHR },// �~
  { 20*8,  21*8+4,   7, BV_RES_COMMON_CHR, BV_RES_COMMON_CHR,  BV_RES_COMMON_CHR },// �x�`�F�b�N
  {  2*8,  16*8,   0, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W0
  {  6*8,  16*8,   1, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W1
  { 10*8,  16*8,   2, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W2
  { 14*8,  16*8,   3, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W3
  { 18*8,  16*8,   4, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W4
  { 22*8,  16*8,   5, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W5
  { 26*8,  16*8,   6, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W6
  { 30*8,  16*8,   7, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �o�b�W7
  {  (4*0+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��0-�P
  {  (4*0+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��0-�Q
  {  (4*0+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��0-�R
  {  (4*1+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��1-�P
  {  (4*1+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��1-�Q
  {  (4*1+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��1-�R
  {  (4*2+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��2-�P
  {  (4*2+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��2-�Q
  {  (4*2+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��2-�R
  {  (4*3+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��3-�P
  {  (4*3+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��3-�Q
  {  (4*3+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��3-�R
  {  (4*4+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��4-�P
  {  (4*4+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��4-�Q
  {  (4*4+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��4-�R
  {  (4*5+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��5-�P
  {  (4*5+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��5-�Q
  {  (4*5+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��5-�R
  {  (4*6+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��6-�P
  {  (4*6+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��6-�Q
  {  (4*6+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��6-�R
  {  (4*7+2)*8,  14*8,   9, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��7-�P
  {  (4*7+4)*8,  17*8,  10, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��7-�Q
  {  (4*7+3)*8,  19*8,   8, BV_RES_CHR,        BV_RES_PLTT,        BV_RES_CELL,      },// �L���L��7-�R
};

//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�o�^
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ClActSet(BADGEVIEW_WORK *wk) 
{

  GFL_CLWK_DATA add;
  int i;

  // �Z���A�N�^�[�o�^
  for(i=0;i<BV_OBJ_MAX;i++){
    add.pos_x  = clact_dat[i][0];
    add.pos_y  = clact_dat[i][1];
    add.anmseq = clact_dat[i][2];
    add.bgpri    = 0;
    add.softpri  = BV_OBJ_MAX-i;

    wk->clwk[i] = GFL_CLACT_WK_Create( wk->clUnit,
                                       wk->clres[clact_dat[i][3]],
                                       wk->clres[clact_dat[i][4]],
                                       wk->clres[clact_dat[i][5]],
                                       &add, CLSYS_DEFREND_MAIN, HEAPID_LEADERBOARD );
    GFL_CLACT_WK_SetAutoAnmFlag( wk->clwk[i], TRUE );
  }
  
}



//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e�����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ClActExit(BADGEVIEW_WORK *wk)
{

  GFL_CLGRP_CGR_Release(      wk->clres[BV_RES_COMMON_CHR] );
  GFL_CLGRP_PLTT_Release(     wk->clres[BV_RES_COMMON_PLTT] );
  GFL_CLGRP_CELLANIM_Release( wk->clres[BV_RES_COMMON_CELL] );
  GFL_CLGRP_CGR_Release(      wk->clres[BV_RES_CHR] );
  GFL_CLGRP_PLTT_Release(     wk->clres[BV_RES_PLTT] );
  GFL_CLGRP_CELLANIM_Release( wk->clres[BV_RES_CELL] );


  // �Z���A�N�^�[���j�b�g�j��
  GFL_CLACT_UNIT_Delete( wk->clUnit );

  //OAM�����_���[�j��
  GFL_CLACT_SYS_Delete();

}

// BMPWIN�������p�\����
typedef struct{
  u32 frame;
  u8  x,y,w,h;
  u16 pal, chr;
}BMPWIN_DAT;


static const BMPWIN_DAT bmpwin_dat={
  BV_BGFRAME_U_MSG,
   2, 10,
  28, 12,
  4,  
};

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BmpWinInit(BADGEVIEW_WORK *wk)
{
  int i;
  const BMPWIN_DAT *windat;

  // BMPWIN�V�X�e���J�n
  GFL_BMPWIN_Init( HEAPID_LEADERBOARD );
 
  // �T�u��ʂ�BMPWIN���m��
  windat = &bmpwin_dat;
  wk->InfoWin = GFL_BMPWIN_Create( windat->frame,
                                   windat->x, windat->y,
                                   windat->w, windat->h,
                                   windat->pal, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin), 0x0f0f );
  GFL_BMPWIN_MakeTransWindow( wk->InfoWin );
  

}

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BmpWinExit(BADGEVIEW_WORK *wk)
{
  
  // �T�u��ʗpBMPWIN�����
  GFL_BMPWIN_Delete( wk->InfoWin );

  // BMPWIN�V�X�e���I��
  GFL_BMPWIN_Exit();

}


#define BV_COL_BLACK      ( PRINTSYS_LSB_Make(  1,  2, 15) )    // ��

//----------------------------------------------------------------------------------
/**
 * @brief �����`��V�X�e��������
 *

 */
//----------------------------------------------------------------------------------
static void PrintSystemInit(BADGEVIEW_WORK *wk)
{
  int i;
  // �t�H���g�m��
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr ,
                              GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_LEADERBOARD );

  // �`��ҋ@�V�X�e��������
  wk->printQue = PRINTSYS_QUE_Create( HEAPID_LEADERBOARD );

  // BMPWIN��PRINT_UTIL���֘A�t��
  PRINT_UTIL_Setup( &wk->printUtil, wk->InfoWin );
  
}

//----------------------------------------------------------------------------------
/**
 * @brief �����`��V�X�e�����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void PrintSystemDelete(BADGEVIEW_WORK *wk)
{
  
  // ���b�Z�[�W�\���p�V�X�e�����
  PRINTSYS_QUE_Delete( wk->printQue );

  // �t�H���g���
  GFL_FONT_Delete( wk->font );
  
}

//----------------------------------------------------------------------------------
/**
 * @brief �����`��V�X�e�����C��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void PrintSystem_Main( BADGEVIEW_WORK *wk )
{
  int i;

  // �`��҂�
  PRINTSYS_QUE_Main( wk->printQue );

  // �`��I���ςł���Γ]��
  PRINT_UTIL_Trans( &wk->printUtil, wk->printQue );

}















//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// �T�u�V�[�P���X
//----------------------------------------------------------------------------------

static BOOL SubSuq_FadeinWait( BADGEVIEW_WORK *wk );
static BOOL SubSuq_Main( BADGEVIEW_WORK *wk );
static BOOL SubSeq_IconWait( BADGEVIEW_WORK *wk );
static BOOL SubSuq_FadeOut( BADGEVIEW_WORK *wk );
static BOOL SubSuq_FadeOutWait( BADGEVIEW_WORK *wk );


static BOOL (*functable[])(BADGEVIEW_WORK *wk) = {
  SubSuq_FadeinWait,      // SUBSEQ_FADEIN_WAIT;
  SubSuq_Main,            // SUBSEQ_MAIN 
  SubSeq_IconWait,        // SUBSEQ_ICON_WAIT,
  SubSuq_FadeOut,         // SUBSEQ_FADEOUT
  SubSuq_FadeOutWait,     // SUBSEQ_FADEOUT_WAIT
};

//----------------------------------------------------------------------------------
/**
 * @brief �T�u�V�[�P���X���䃁�C��
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL LBSEQ_Main( BADGEVIEW_WORK *wk )
{
  // �T�u�V�[�P���X���s
  return functable[wk->seq](wk);
}


//----------------------------------------------------------------------------------
/**
 * @brief �ySUBSEQ�z�t�F�[�h�C���҂�
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL SubSuq_FadeinWait( BADGEVIEW_WORK *wk )
{
  if( WIPE_SYS_EndCheck() ){
    wk->seq = SUBSEQ_MAIN;
  }
  return TRUE;
}


//----------------------------------------------------------------------------------
/**
 * @brief �ySUBSEQ�z���C������
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL SubSuq_Main( BADGEVIEW_WORK *wk )
{
  u32 ret;
  // �^�b�`�o�[����
  if(TouchBar_KeyControl(wk)==GFL_UI_TP_HIT_NONE){

  }
  return TRUE;
}


//----------------------------------------------------------------------------------
/**
 * @brief �߂�A�C�R���̃A�j���҂�
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL SubSeq_IconWait( BADGEVIEW_WORK *wk )
{
  if( GFL_CLACT_WK_CheckAnmActive( wk->clwk[BV_OBJ_END] )==FALSE){
    wk->seq = SUBSEQ_FADEOUT;
  }
  return TRUE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �ySUBSEQ�z�t�F�[�h�A�E�g
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL SubSuq_FadeOut( BADGEVIEW_WORK *wk )
{
  // �t�F�[�h�C���J�n
  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
                  WIPE_FADE_BLACK, 16, 1, HEAPID_LEADERBOARD );
  wk->seq = SUBSEQ_FADEOUT_WAIT;

  return TRUE;
}


//----------------------------------------------------------------------------------
/**
 * @brief �ySUBSEQ�z�t�F�[�h�A�E�g�҂�
 *
 * @param   wk    
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL SubSuq_FadeOutWait( BADGEVIEW_WORK *wk )
{
  if( WIPE_SYS_EndCheck() ){
    wk->seq = 0;
    return FALSE;
  }
  return TRUE;
}














//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// �C���^�[�t�F�[�X����
//----------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
/**
 * �Z���^�N�^�[���W�ʒu�Z�b�g
 *
 * @param work    
 * @param id    �Z���A�N�^�[�C���f�b�N�X
 * @param x     �ʒuX
 * @param y     �ʒuY
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CellActorPosSet( BADGEVIEW_WORK *wk, int id, int x, int y )
{
  GFL_CLACTPOS clpos;
  clpos.x = x;
  clpos.y = y;

  GFL_CLACT_WK_SetPos( wk->clwk[id], &clpos, CLSYS_DEFREND_MAIN );

}





//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// �@�\�֐�


//----------------------------------------------------------------------------------
/**
 * @brief ���ʏ�񏑂�����
 *
 * @param   wk    BADGEVIEW_WORK
 * @param   id    ���Ԗڂ�
 */
//----------------------------------------------------------------------------------
static void Rewrite_UpperInformation( BADGEVIEW_WORK *wk,  int id )
{
  int nation     = 0;
  int local      = 0;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, HEAPID_LEADERBOARD );
  STRBUF *pmsstr;
  
  // �e����o�^

  // �����N�m�n
  WORDSET_RegisterNumber( wk->wset, 0, wk->param->rank_no,  2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU);
  // �g���C��NO
  WORDSET_RegisterNumber( wk->wset, 1, wk->param->train_no, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU);
  WORDSET_RegisterCountryName(    wk->wset, 2, nation );          // ��
  WORDSET_RegisterLocalPlaceName( wk->wset, 3, nation, local );   // �n��


  // �`��̈�N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin), 0x0f0f );

  // �`��

  PRINT_UTIL_PrintColor( &wk->printUtil, wk->printQue, 
                         0, 0, strbuf, wk->font, BV_COL_BLACK );
  
  GFL_STR_DeleteBuffer( strbuf );

}


static const GFL_UI_TP_HITTBL TouchButtonHitTbl[] =
{
  { 168, 191,   8,  8+24 },   // FUNC_LEFT_PAGE:  �����
  { 168, 191,  32,  32+24 },  // FUNC_RIGHT_PAGE: �E���
  { 168, 191, 232,  255 },    // FUNC_END:        ��߂�
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};

//=============================================================================================
/**
 * @brief �^�b�`�o�[����
 *
 * @param   wk    
 *
 * @retval  staitc BOOL   
 */
//=============================================================================================
static int TouchBar_KeyControl( BADGEVIEW_WORK *wk )
{
  int  touch;
  touch = GFL_UI_TP_HitTrg( TouchButtonHitTbl );

  // �L�[�ŃL�����Z��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
    PMSND_PlaySE( SEQ_SE_CANCEL1 );
    touch = FUNC_END;
  }
  
  // ���y�[�W�E�E�y�[�W�E�߂�@�\�̌Ăяo��
  ExecFunc(wk, touch);
  
  return touch;
}


//----------------------------------------------------------------------------------
/**
 * @brief �e�@�\�i���y�[�W�E�E�y�[�W�E�߂�j�̎��s
 *
 * @param   wk    
 * @param   touch   
 */
//----------------------------------------------------------------------------------
static void ExecFunc( BADGEVIEW_WORK *wk, int touch )
{
  switch(touch){
  case FUNC_LEFT_PAGE:
    if(_page_move_check( wk, touch )){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      SetupPage( wk, wk->page );
    }
    break;
  case FUNC_RIGHT_PAGE:
    if(_page_move_check( wk, touch )){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      SetupPage( wk, wk->page );
    }
    break;
  case FUNC_END:
    wk->seq = SUBSEQ_ICON_WAIT;
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_END], 9 );
    break;
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief �y�[�W�����Z�o
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void _page_max_init( BADGEVIEW_WORK *wk )
{
  wk->page = 0;
  wk->page_max = wk->trainer_num /BV_1PAGE_NUM;
  if(wk->trainer_num%BV_1PAGE_NUM){
    wk->page_max++;
  }
  OS_Printf("page=%d, trainer_num=%d, max=%d\n", wk->page, wk->trainer_num, wk->page_max);
}

#define ISSHU_BADGE_NUM   ( 8 )

//----------------------------------------------------------------------------------
/**
 * @brief   ���A�C�R���̏�Ԃ��y�[�W���ŕω�������
 * @todo �W���o�b�W�t���O������
 *
 *
 * @param   wk    
 * @param   page    
 * @param   max   
 */
//----------------------------------------------------------------------------------
static void _page_clact_set( BADGEVIEW_WORK *wk, int page, int max )
{
  int i;
  // �P�y�[�W�����Ȃ���������\�����Ȃ�
  for(i=0;i<ISSHU_BADGE_NUM;i++){
    if(1){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_BADGE0+i],  TRUE );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_BADGE0+i],  FALSE );
    }
  }

  for(i=0;i<ISSHU_BADGE_NUM*3;i++){
    if(1){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_KIRAKIRA0_0+i],  TRUE );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_KIRAKIRA0_0+i],  FALSE );
    }
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief �ړ��\���`�F�b�N
 *
 * @param   wk    
 * @param   touch   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _page_move_check( BADGEVIEW_WORK *wk, int touch )
{
  int result=FALSE;

  // ���ړ��̎�
  if(touch==FUNC_LEFT_PAGE){
    if( wk->page>0 ){   // 0�y�[�W�łȂ���΁|�P
      wk->page--;
      result=TRUE;
    }

  // �E�ړ��̎�
  }else if(touch==FUNC_RIGHT_PAGE){
    if(wk->page < wk->page_max-1){  // MAX�y�[�W������������΁{�P
      wk->page++;
      result=TRUE;
    }
  }

  return result;
}


//----------------------------------------------------------------------------------
/**
 * @brief �����y�[�W�\��
 *
 * @param   wk    
 * @param   page    
 */
//----------------------------------------------------------------------------------
static void SetupPage( BADGEVIEW_WORK *wk, int page )
{
  wk->page = page;

  // �^�b�`�o�[�\���ݒ�
  _page_clact_set( wk, page, wk->page_max );

  // �v���[�g��ԕύX
  NamePlatePrint_1Page( wk );

}
//----------------------------------------------------------------------------------
/**
 * @brief   �\������g���[�i�[�������߂�
 *
 * @param   page          ���݂̃y�[�W��
 * @param   max           �ő�y�[�W��
 * @param   trainer_num   �g���[�i�[�S���̐�
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_print_num( int page, int max, int trainer_num )
{

  int num;
  num = trainer_num-page*BV_1PAGE_NUM;

  if(num > BV_1PAGE_NUM){
    num = BV_1PAGE_NUM;
  }

  OS_Printf("1�y�[�W�\������=%d\n", num);
  return num;
}


//----------------------------------------------------------------------------------
/**
 * @brief 1�y�[�W���̖��O�v���[�g��`�悷��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void NamePlatePrint_1Page( BADGEVIEW_WORK *wk )
{
  int i,num;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, HEAPID_LEADERBOARD );

  // �\�����鑍���擾
  num = _get_print_num( wk->page, wk->page_max, wk->trainer_num );

  // �v���[�g�`��
  BgFramePrint( wk, i, strbuf, strbuf, strbuf );

  GFL_STR_DeleteBuffer( strbuf );
}

//----------------------------------------------------------------------------------
/**
 * @brief BGFRAME�ɕ������`�悵�A��ʂɔ��f
 *
 * @param   wk      
 * @param   id      �w��v���[�g
 * @param   str     ���O
 * @param   gender  ����
 */
//----------------------------------------------------------------------------------
static void BgFramePrint( BADGEVIEW_WORK *wk, int id, STRBUF *str, STRBUF *str2, STRBUF *str3 )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin), 0x0f0f );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->InfoWin), 0,  0, str, wk->font, BV_COL_BLACK );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->InfoWin), 0, 32, str2, wk->font, BV_COL_BLACK );
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->InfoWin), 0, 64, str3, wk->font, BV_COL_BLACK );
  GFL_BMPWIN_MakeTransWindow_VBlank( wk->InfoWin );
  

}





//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
// �f�o�b�O�p
//----------------------------------------------------------------------------------
#ifdef PM_DEBUG


  // �f�o�b�O�p�f�[�^�Z�b�g
static void _debug_data_set( BADGEVIEW_WORK *wk )
{
  int i, num=0;
  STRBUF *str;

#ifdef SPECIAL_FEBRUARY
  num = 23;
#endif

  if(GFL_UI_KEY_GetCont() & PAD_KEY_UP){
    num = 5;
  }else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_X){
    num = 13;
  }else if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT){
    num = 20;
  }else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
    num = 25;
  }else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_Y){
    num = 30;
  }

  
  for(i=0;i<num;i++){
    str = GFL_MSG_CreateString( wk->debugname, 20+i);
    GFL_STR_DeleteBuffer( str );
  }
}

#endif