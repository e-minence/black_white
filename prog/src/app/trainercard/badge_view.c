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

#include "system/main.h"            // HEAPID_TR_CARD
#include "system/cursor_move.h"     // CURSOR_MOVE
#include "system/gfl_use.h"         // GFUser_VIntr_CreateTCB
#include "system/wipe.h"            // WIPE_SYS_Start,
#include "system/bmp_winframe.h"            // WIPE_SYS_Start,
#include "sound/pm_sndsys.h"        // PMSND_PlaySE
#include "savedata/shortcut.h"      // SHORTCUT_SetRegister
#include "savedata/misc.h"          // MISC_GetBadgeFlag
#include "system/shortcut_data.h"   // 
#include "print/wordset.h"          // WORDSET
#include "print/printsys.h"         // PRINT_UTIL,PRINT_QUE...
#include "app/leader_board.h"       // proc
#include "app/app_menu_common.h"
#include "arc/arc_def.h"            // ARCID_LEADER_BOARD
#include "arc/trainer_case.naix"    // leaderboard graphic
#include "app/trainer_card.h"

#include "font/font.naix"           // NARC_font_large_gftr
#include "arc/message.naix"         // NARC_message_leader_board_dat
#include "msg/msg_trainercard.h"

#include "trcard_sys.h"
#include "trcard_snd_def.h"

#include "badge_view_def.h"       // �e���`��

#ifdef PM_DEBUG
#define SPECIAL_FEBRUARY
#endif

//--------------------------------------------------------------------------------------------
// �萔��`
//--------------------------------------------------------------------------------------------

// �R�c�J�����ݒ�
#define BADGE3D_CAMERA_POS_Y     ( FX32_ONE*1.4  )  ///< �W�����[�_�[�����O�̓|��
#define BADGE3D_CAMERA_POS_Z     ( FX32_ONE*5.49 )  ///< �W�����[�_�[�ƃJ�����̋����i�h�b�g�̂䂪�ݒ����j
#define BADGE3D_CAMERA_TARGET_Y  ( FX32_ONE*-1.49 )  ///< ���W(0,0,0)�̃��f�����O�f�[�^��ʏ�̈ʒu

// ���ʕ����ʃE�C���h�E�I�t�Z�b�g
#define SYSTEMWIN_FRM_CGX_OFFSET  (  1 )
#define SYSTEMWIN_FRM_PAL_OFFSET  ( 14 )

// ���E�C���h�E������
#define BADGEVIEW_MSG_LEN   ( 26*2*2 )    // ��b�E�C���h�E�����Q

#define BADGE_NUM   ( 8 )

// 48���ԂŖ����������P�i�K������̂ł��̔{���ŊǗ�
//#define BADGE_POLISH_RATE   ( 48*3 )  

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
#define BV_BGFRAME_U_MARK   ( GFL_BG_FRAME1_S )
#define BV_BGFRAME_U_BG     ( GFL_BG_FRAME2_S )
#define BV_BGFRAME_D_MSG    ( GFL_BG_FRAME1_M )
#define BV_BGFRAME_D_BUTTON ( GFL_BG_FRAME2_M )
#define BV_BGFRAME_D_BG     ( GFL_BG_FRAME3_M )

// ���E�C���h�E�p��`
enum{
  INFO_TYPE_BADGE =0,
  INFO_TYPE_GYMLEADER,
};

// �o�b�W�̃p���b�g�������ׂ̃p���b�g�f�[�^��
#define BADGE_POLISH_PAL_NUM    ( 3 )
#define LEADER_PAL_NUM          ( 3 )
#define BADGE_KIRAKIRA_NUM      ( 3 )

//--------------------------------------------------------------------------------------------
// �\���̒�`��`
//--------------------------------------------------------------------------------------------

// �W�����[�_�[�X���C�h���
typedef struct{
  u16 on;        ///< ���݂̃^�b�`��
  u16 old_on;    ///< �P�t���[���O�̃^�b�`��
  int x, old_x, old2_x;  ///< X���W�A�P�t���[���O��X���W
  int start_x;   ///< ��]�J�n�����W 
  int enable;    ///< 
}SLIDE_WORK;

// �o�b�W�������
typedef struct{
  int polish[BADGE_NUM];     ///< �������
  u8  grade[BADGE_NUM];      ///< �����t���O
  u8  old_grade[BADGE_NUM];  ///< �����t���O�ߋ�
}BADGE_POLISH;

// �o�b�W��������p���[�N
typedef struct {
  s8 OldDirX;   
  s8 OldDirY;   
  s8 DirX;      
  s8 DirY;
  u8 Snd;
  int BeforeX;
  int BeforeY;
}SCRATCH_WORK;

  

typedef struct {
  ARCHANDLE     *g_handle;  ///< �O���t�B�b�N�f�[�^�t�@�C���n���h��
  
  GFL_FONT      *font;      ///< �t�H���g�f�[�^
  GFL_BMPWIN    *InfoWin;
  PRINT_UTIL    printUtil;
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;
  GFL_TCBLSYS   *pMsgTcblSys;

  GFL_MSGDATA   *mman;    ///< ���b�Z�[�W�f�[�^�}�l�[�W��
  WORDSET       *wset;    ///< �P��Z�b�g
  STRBUF        *expbuf;  ///< ���b�Z�[�W�W�J�̈�
  STRBUF        *strbuf[BV_STR_MAX];

  GFL_CLUNIT    *clUnit;            ///< �Z���A�N�^�[���j�b�g
  GFL_CLWK      *clwk[BV_OBJ_MAX];  ///< �Z���A�N�^�[���[�N
  u32           clres[BV_RES_MAX];  ///< �Z���A�N�^�[���\�[�X�p�C���f�b�N�X

  GFL_TCB       *VblankTcb;         ///< �o�^����VblankFunc�̃|�C���^

  // 3D�S��
  GFL_G3D_UTIL  *g3dUtil;
  u16           unit_idx;           ///< GFL_G3D_UTIL������U��ԍ�
  GFL_G3D_LIGHTSET *light3d;
  GFL_G3D_CAMERA   *camera;
  
  TRCARD_CALL_PARAM *param;         ///< �Ăяo���p�����[�^
  int seq,next_seq;                 ///< �T�u�V�[�P���X����p���[�N
  u16 page,page_max;                ///< ���݂̕\���y�[�W

  int trainer_num;                  ///< �o�g���T�u�E�F�C�f�[�^�ɑ��݂��Ă����l��
  int scrol_bg;                     ///< BG�X�N���[���p�̃��[�N

  SLIDE_WORK  slide;          ///< �W�����[�_�[����p�̃��[�N
  int         old_animeFrame; ///< �W�����[�_�[�A�j���t���[���ʒu
  int         animeFrame;     ///< �W�����[�_�[�A�j���t���[���ʒu
  int         animeSpeed;     ///< �W�����[�_�[�p�l���̑������ރX�s�[�h
  int         targetSet;      ///< �ړ�����w�肵�Ă��邩�H
  int         targetFrame;    ///< �ړ���t���[��

  void              *badgePalBuf[BADGE_POLISH_PAL_NUM]; ///< �o�b�WOBJ�p���b�g�f�[�^�ۑ��p���[�N
  NNSG2dPaletteData *badgePal[BADGE_POLISH_PAL_NUM];    ///< �o�b�W�p�p���b�g�f�[�^
  void              *leaderPalBuf[LEADER_PAL_NUM];      ///< �W�����[�_�[��O���p���b�g�f�[�^�ۑ��p���[�N
  NNSG2dPaletteData *leaderPal[LEADER_PAL_NUM];          ///< �W�����[�_�[��O���p���b�g�f�[�^

  u8          badgeflag[BADGE_NUM];

  TR_CARD_SV_PTR trCard;
  BADGE_POLISH   badge;
  SCRATCH_WORK   scratch;
  
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
static void Graphic3DInit( BADGEVIEW_WORK *wk );
static void Graphic3DExit( BADGEVIEW_WORK *wk );
static void Resource3DInit( BADGEVIEW_WORK *wk );
static void Resource3DExit( BADGEVIEW_WORK *wk );
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
static BOOL SEQ_Main( BADGEVIEW_WORK *wk );
static void InfoWinPrint( BADGEVIEW_WORK *wk, int type, int no );
static void InfoWinPut( BADGEVIEW_WORK *wk );
static  int TouchBar_KeyControl( BADGEVIEW_WORK *wk );
static void ExecFunc( BADGEVIEW_WORK *wk, int trg );
static void ContFunc( BADGEVIEW_WORK *wk, int cont );
static void _page_max_init( BADGEVIEW_WORK *wk );
static void _page_clact_set( BADGEVIEW_WORK *wk, int page, int max );
static  int _page_move_check( BADGEVIEW_WORK *wk, int touch );
static void SetupPage( BADGEVIEW_WORK *wk, int page );
static void NamePlatePrint_1Page( BADGEVIEW_WORK *wk );
static void CellActorPosSet( BADGEVIEW_WORK *wk, int id, int x, int y );
static void Draw3D( BADGEVIEW_WORK* wk );
static void SlideFunc( BADGEVIEW_WORK *wk, int trg, int flag, int x );
static void SetRtcData( BADGEVIEW_WORK *wk );
static void RefreshPolishData( BADGEVIEW_WORK *wk );
static void Trans_BadgePalette( BADGEVIEW_WORK *wk );
static void BrushBadge( BADGEVIEW_WORK *wk, u32 touch );


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

  OS_Printf( "�����������@�o�b�W��ʏ����J�n�@����������\n" );

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TR_CARD, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(BADGEVIEW_WORK), HEAPID_TR_CARD );
  MI_CpuClearFast( wk, sizeof(BADGEVIEW_WORK) );

//  WIPE_SetBrightness( WIPE_DISP_MAIN,WIPE_FADE_BLACK );
//  WIPE_SetBrightness( WIPE_DISP_SUB,WIPE_FADE_BLACK );

  InitWork( wk, pwk );  // ���[�N������
  VramBankSet();        // VRAM�ݒ�
  BgInit(wk);           // BG�V�X�e��������
  BgGraphicInit(wk);    // BG�O���t�B�b�N�]��
  ClActInit(wk);        // �Z���A�N�^�[�V�X�e��������
  ClActSet(wk);         // �Z���A�N�^�[�o�^
  Graphic3DInit(wk);    // 3D������
  Resource3DInit(wk);   // 3D���\�[�X�ǂݍ���
  BmpWinInit(wk);       // BMPWIN������
  PrintSystemInit(wk);  // �`��V�X�e��������

  // �����y�[�W�\��
  SetupPage( wk, wk->page );

  GFL_NET_WirelessIconEasy_HoldLCD( FALSE, HEAPID_TR_CARD );


  // �t�F�[�h�C���J�n
  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
                  WIPE_FADE_BLACK, 16, 1, HEAPID_TR_CARD );

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

  if( SEQ_Main( wk ) == FALSE ){
    return GFL_PROC_RES_FINISH;
  }
  
  Draw3D(wk);
  PrintSystem_Main( wk );         // ������`�惁�C��
  GFL_CLACT_SYS_Main();           // �Z���A�N�^�[���C��

  if(++wk->scrol_bg >= 256){
    wk->scrol_bg=0;
  }
  
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
  Resource3DExit(wk);     // 3D���\�[�X���
  Graphic3DExit(wk);      // 3D�I��
  ClActExit(wk);          // �Z���A�N�^�[�V�X�e�����
  BgGraphicExit(wk);      // BG�O���t�B�b�N�֘A�I��
  BgExit(wk);             // BG�V�X�e���I��
  FreeWork(wk);           // ���[�N���
  SetRtcData(wk);           // ���t��ۑ�

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_CheckHeapSafe( HEAPID_TR_CARD );
  GFL_HEAP_DeleteHeap( HEAPID_TR_CARD );


  OS_Printf( "�����������@�o�b�W��ʏ����I���@����������\n" );

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
  BADGEVIEW_WORK * wk = (BADGEVIEW_WORK*)work;
  int scr;
  
  scr = -(wk->scrol_bg/2);
  GFL_BG_SetScroll( BV_BGFRAME_U_BG, GFL_BG_SCROLL_X_SET, scr );
  GFL_BG_SetScroll( BV_BGFRAME_U_BG, GFL_BG_SCROLL_Y_SET, scr );
  GFL_BG_SetScroll( BV_BGFRAME_D_BG, GFL_BG_SCROLL_X_SET, scr);
  GFL_BG_SetScroll( BV_BGFRAME_D_BG, GFL_BG_SCROLL_Y_SET, scr);

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
  int i;
  wk->param     = pwk;
  wk->next_seq  = SUBSEQ_FADEIN_WAIT;

  // �O���t�B�b�N�f�[�^�n���h���I�[�v��
  wk->g_handle  = GFL_ARC_OpenDataHandle( ARCID_TRAINERCARD, HEAPID_TR_CARD);

  // ���b�Z�[�W�}�l�[�W���[�m��  
  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_trainercard_dat, HEAPID_TR_CARD );


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
  wk->expbuf = GFL_STR_CreateBuffer( BADGEVIEW_MSG_LEN, HEAPID_TR_CARD );

  // ���[�h�Z�b�g���[�N�m��
  wk->wset = WORDSET_CreateEx( 8, WORDSET_COUNTRY_BUFLEN, HEAPID_TR_CARD );
  
  
  // VBlank�֐��Z�b�g
  wk->VblankTcb = GFUser_VIntr_CreateTCB( VBlankFunc, wk, 0 );  
  


#ifdef PM_DEBUG
  wk->debugname = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_trname_dat, HEAPID_TR_CARD );

  // �f�o�b�O�p�f�[�^�Z�b�g
  _debug_data_set( wk );

  GFL_MSG_Delete( wk->debugname );

#endif
  
  // �g���[�i�[�������グ
  wk->trainer_num = 8;

  // �W�����[�_�[��]���[�N������
  wk->targetSet    = -1;
  // �ő�y�[�W���擾
  _page_max_init(wk);

  // �g���[�i�[�J�[�h�Z�[�u�f�[�^
  wk->trCard = GAMEDATA_GetTrainerCardPtr( wk->param->gameData);

  // ���Ԏ擾���s���o�b�W�����f�[�^�̍X�V���s��
  RefreshPolishData( wk );

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
  BV_BGFRAME_U_MARK,
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
  GFL_BG_Init( HEAPID_TR_CARD );

  // BG���[�h�ݒ�
  { 
    GFL_BG_SYS_HEADER sysh = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
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
      { // �T�u��ʁF���[�O�}�[�N�w�i
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,    1, 0, 0, FALSE
      },
      { // �T�u��ʁF�w�i
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,    2, 0, 0, FALSE
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
      GFL_BG_SetClearCharacter( bgframe_init_tbl[i], 0x20, 0, HEAPID_TR_CARD );
      GFL_BG_SetVisible(  bgframe_init_tbl[i], VISIBLE_ON     );
    }

    // �c���BG�ʂ�OFF��
    GFL_BG_SetVisible( GFL_BG_FRAME3_S, VISIBLE_OFF );
  }
  
  // �������ݒ�
  G2_BlendNone();
  G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG2,7,16);

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

  G2_BlendNone();
  G2S_BlendNone();
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
  int i;
  ARCHANDLE * handle = wk->g_handle;

  // �T�u��ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg01_NCGR,
                                        BV_BGFRAME_U_BG, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg02_NSCR,
                                        BV_BGFRAME_U_BG, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_trainer_case_badge_bg02_NCLR, 
                                        PALTYPE_SUB_BG, 0, 0, HEAPID_TR_CARD );

  // �T�u��ʔw�i(���[�O�}�[�N�j�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg_02_NCGR,
                                        BV_BGFRAME_U_MARK, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg03_NSCR,
                                        BV_BGFRAME_U_MARK, 0, 0, FALSE, HEAPID_TR_CARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg01_NCGR,
                                        BV_BGFRAME_D_BG, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg02_NSCR,
                                        BV_BGFRAME_D_BG, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_trainer_case_badge_bg01_NCLR, 
                                        PALTYPE_MAIN_BG, 0, 0, HEAPID_TR_CARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_trainer_case_badge_bg01_NCGR,
                                        BV_BGFRAME_D_MSG, 0, 0, FALSE, HEAPID_TR_CARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_trainer_case_badge_bg01_NSCR,
                                        BV_BGFRAME_D_MSG, 0, 0, FALSE, HEAPID_TR_CARD );
  
  // �^�b�`�o�[BG�]��
  {
    ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_TR_CARD);

    GFL_ARCHDL_UTIL_TransVramBgCharacter( c_handle, APP_COMMON_GetBarCharArcIdx(),
                                          BV_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_TR_CARD );
    GFL_ARCHDL_UTIL_TransVramScreen(      c_handle, APP_COMMON_GetBarScrnArcIdx(),
                                          BV_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_TR_CARD );
    GFL_ARCHDL_UTIL_TransVramPaletteEx(   c_handle, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
                                          0, TOUCHBAR_PAL*32, 32, HEAPID_TR_CARD );
    
    // �X�N���[���̃p���b�g�w���ύX
    GFL_BG_ChangeScreenPalette( BV_BGFRAME_D_BUTTON, TOUCHBAR_X, TOUCHBAR_Y,
                                                     TOUCHBAR_W, TOUCHBAR_H, TOUCHBAR_PAL );
    GFL_BG_LoadScreenReq( BV_BGFRAME_D_BUTTON );

    GFL_ARC_CloseDataHandle( c_handle );
  }
  
  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_TR_CARD );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_TR_CARD );

  // �V�X�e���E�C���h�E���\�[�X�]��
  BmpWinFrame_PalSet( GFL_BG_FRAME0_S, SYSTEMWIN_FRM_PAL_OFFSET, MENU_TYPE_SYSTEM, HEAPID_TR_CARD );
  BmpWinFrame_CgxSet( GFL_BG_FRAME0_S, SYSTEMWIN_FRM_CGX_OFFSET, MENU_TYPE_SYSTEM, HEAPID_TR_CARD);


  // �o�b�WOBJ�]���p�p���b�g�ێ�
  for(i=0;i<BADGE_POLISH_PAL_NUM;i++){
    wk->badgePalBuf[i] = GFL_ARCHDL_UTIL_LoadPalette( handle, NARC_trainer_case_badge_obj03_NCLR-i, 
                                                      &wk->badgePal[i], HEAPID_TR_CARD );
  }

  for(i=0;i<LEADER_PAL_NUM;i++){
    wk->leaderPalBuf[i] = GFL_ARCHDL_UTIL_LoadPalette( handle, NARC_trainer_case_gym_leaders2_NCLR-i, 
                                                       &wk->leaderPal[i], HEAPID_TR_CARD );
  }

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
  int i;
  
  // ���[�_�[��O���p�p���b�g�f�[�^���
  for(i=0;i<LEADER_PAL_NUM;i++){
    GFL_HEAP_FreeMemory(  wk->leaderPalBuf[i] );
  }
  // OBJ�p���b�g���
  for(i=0;i<BADGE_POLISH_PAL_NUM;i++){
   GFL_HEAP_FreeMemory( wk->badgePalBuf[i] );
  }
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
  ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_TR_CARD);

  // �Z���A�N�^�[������
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &VramTbl, HEAPID_TR_CARD );
  wk->clUnit  = GFL_CLACT_UNIT_Create( BV_CLACT_NUM, 1,  HEAPID_TR_CARD );

  // OBJ�ʕ\��ON
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // �Z���A�N�^�[���\�[�X�ǂݍ���
  
  // ---�o�b�W��ʗp�����---
  wk->clres[BV_RES_CHR]  = GFL_CLGRP_CGR_Register(      wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCGR, 0, 
                                                        CLSYS_DRAW_MAIN, HEAPID_TR_CARD );
  wk->clres[BV_RES_PLTT] = GFL_CLGRP_PLTT_Register(     wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCLR, 
                                                        CLSYS_DRAW_MAIN, 0, HEAPID_TR_CARD );
  wk->clres[BV_RES_CELL] = GFL_CLGRP_CELLANIM_Register( wk->g_handle, 
                                                        NARC_trainer_case_badge_obj01_NCER, 
                                                        NARC_trainer_case_badge_obj01_NANR, 
                                                        HEAPID_TR_CARD );

  // ���ʃ��j���[�f��
  wk->clres[BV_RES_COMMON_CHR] = GFL_CLGRP_CGR_Register(       c_handle, 
                                                               APP_COMMON_GetBarIconCharArcIdx(), 0, 
                                                               CLSYS_DRAW_MAIN, 
                                                               HEAPID_TR_CARD );

  wk->clres[BV_RES_COMMON_PLTT] = GFL_CLGRP_PLTT_RegisterEx(   c_handle, 
                                                               APP_COMMON_GetBarIconPltArcIdx(), 
                                                               CLSYS_DRAW_MAIN, 
                                                               11*32, 0, 3, HEAPID_TR_CARD );

  wk->clres[BV_RES_COMMON_CELL] = GFL_CLGRP_CELLANIM_Register( c_handle, 
                                                               APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K), 
                                                               APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                                               HEAPID_TR_CARD );
  GFL_ARC_CloseDataHandle( c_handle );

}

//-------------------------------------------
// �Z���A�N�^�[�\���p�e�[�u��
//-------------------------------------------

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
                                       &add, CLSYS_DEFREND_MAIN, HEAPID_TR_CARD );
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
static void ClActExit( BADGEVIEW_WORK *wk )
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


//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light_data[] = {
  { 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
  { 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light3d_setup = { light_data, NELEMS(light_data) };

// �J�����ݒ�֘A
#define cameraPerspway  ( 0x0b60 )
#define cameraAspect    ( FX32_ONE * 4 / 3 )
#define cameraNear      ( 1 << FX32_SHIFT )
#define cameraFar       ( 1024 << FX32_SHIFT )

//-------------------------------------------------------------------------------------------
/**
 * @brief 3D�������֐�
 */
//-------------------------------------------------------------------------------------------
static void Graphic3DInit( BADGEVIEW_WORK *wk )
{

  // 3D�V�X�e����������
  GFL_G3D_Init( 
      GFL_G3D_VMANLNK, GFL_G3D_TEX128K, 
      GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0x1000, HEAPID_TR_CARD, NULL );

  // ���C�g�쐬
  wk->light3d = GFL_G3D_LIGHT_Create( &light3d_setup, HEAPID_TR_CARD );
  GFL_G3D_LIGHT_Switching( wk->light3d );

  // �J���������ݒ�
  {
    GFL_G3D_PROJECTION proj = { GFL_G3D_PRJPERS, 0, 0, cameraAspect, 0, cameraNear, cameraFar, 0 }; 
    proj.param1 = FX_SinIdx( cameraPerspway ); 
    proj.param2 = FX_CosIdx( cameraPerspway ); 
    GFL_G3D_SetSystemProjection( &proj ); 
  }
  
  // 3D�ʂ�ON
  GFL_BG_SetVisible(  GFL_BG_FRAME0_M, VISIBLE_ON );
  
  // 3D��������L����
  G3X_AlphaBlend(TRUE);
}

//-------------------------------------------------------------------------------------------
/**
 * @brief �I������
 */
//-------------------------------------------------------------------------------------------
static void Graphic3DExit( BADGEVIEW_WORK *wk )
{
  // ���C�g�Z�b�g�j��
  GFL_G3D_LIGHT_Delete( wk->light3d );

  // 3D�V�X�e���I��
  GFL_G3D_Exit();

}



//============================================================================================
/**
 * @brief 3D�f�[�^
 */
//============================================================================================

// �W�����[�_�[�p�l��
static const GFL_G3D_UTIL_RES res_table_gymleader[] = 
{
  { ARCID_TRAINERCARD, NARC_trainer_case_gym_leader_NSBMD, GFL_G3D_UTIL_RESARC },
  { ARCID_TRAINERCARD, NARC_trainer_case_gym_leader_NSBCA, GFL_G3D_UTIL_RESARC },
};


static const GFL_G3D_UTIL_ANM anm_table_gymleader[] = 
{
  { 1, 0 },
};
static const GFL_G3D_UTIL_OBJ obj_table_gymleader[] = 
{
  {
    0,                           // ���f�����\�[�XID
    0,                           // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                           // �e�N�X�`�����\�[�XID
    anm_table_gymleader,         // �A�j���e�[�u��(�����w��̂���)
    NELEMS(anm_table_gymleader), // �A�j�����\�[�X��
  },
}; 

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_gymleader, NELEMS(res_table_gymleader), obj_table_gymleader, NELEMS(obj_table_gymleader) },
};

#define SETUP_INDEX_MAX          ( 1 )


//--------------------------------------------------------------------------------------------
/**
 * @breif 3D���\�[�X�ǂݍ���
 */
//-------------------------------------------------------------------------------------------- 
static void Resource3DInit( BADGEVIEW_WORK *wk )
{
  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  wk->g3dUtil = GFL_G3D_UTIL_Create( 10, 10, HEAPID_TR_CARD );

  // ���j�b�g�ǉ�
  wk->unit_idx = GFL_G3D_UTIL_AddUnit( wk->g3dUtil, &setup[0] );

  // �A�j���[�V�����L����
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3dUtil, wk->unit_idx );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // �J�����쐬
  {
    VecFx32    pos = { 0, BADGE3D_CAMERA_POS_Y,    BADGE3D_CAMERA_POS_Z };
    VecFx32 target = { 0, BADGE3D_CAMERA_TARGET_Y, 0                    };
    wk->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_TR_CARD );
  }


}

//--------------------------------------------------------------------------------------------
/**
 * @breif 3D���\�[�X���
 */
//-------------------------------------------------------------------------------------------- 
static void Resource3DExit( BADGEVIEW_WORK *wk )
{ 
  // �J�����j��
  GFL_G3D_CAMERA_Delete( wk->camera );

  // ���j�b�g�j��
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_UTIL_DelUnit( wk->g3dUtil, wk->unit_idx );
    }
  }

  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( wk->g3dUtil );
}


#ifdef PM_DEBUG
static VecFx32 test_pos    = { 0, BADGE3D_CAMERA_POS_Y, BADGE3D_CAMERA_POS_Z };
static VecFx32 test_target = { 0, BADGE3D_CAMERA_TARGET_Y, 0 };
static int     moveflag;
#endif
//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//-------------------------------------------------------------------------------------------- 
static void Draw3D( BADGEVIEW_WORK* wk )
{
  static fx32 frame = 0;
//  static fx32 anime_speed = FX32_ONE;
  static fx32 anime_speed = 0;
  GFL_G3D_OBJSTATUS status;

  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  // �J�����X�V
  GFL_G3D_CAMERA_Switching( wk->camera );

#ifdef PM_DEBUG
  // TEMP: �J�����ݒ�
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L)
  {
    
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
      test_pos.z -= FX32_ONE /10;
    }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_Y){
      test_pos.z += FX32_ONE /10;
    }else if(GFL_UI_KEY_GetCont()&PAD_KEY_UP){
      test_pos.y -= FX32_ONE /10;
    }else if(GFL_UI_KEY_GetCont()&PAD_KEY_DOWN){
      test_pos.y += FX32_ONE /10;
    }else if(GFL_UI_KEY_GetCont()&PAD_KEY_LEFT){
      test_target.y -= FX32_ONE /10;
    }else if(GFL_UI_KEY_GetCont()&PAD_KEY_RIGHT){
      test_target.y += FX32_ONE /10;
    }
    GFL_G3D_CAMERA_SetPos( wk->camera, &test_pos );
    GFL_G3D_CAMERA_SetTarget( wk->camera, &test_target );
    OS_Printf("pos.y=%d pos.z=%d, target.y=%d \n", test_pos.y, test_pos.z, test_target.y);
  }
//  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
//    anime_speed = FX32_ONE;
//  }else{
//    anime_speed = 0;
//  }

#endif

  
  // �A�j���[�V�����X�V
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j,frame;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3dUtil, wk->unit_idx );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      GFL_G3D_OBJECT_GetAnimeFrame( obj, 0, &frame );
//      OS_Printf("anime_frame=%d\n", frame/FX32_ONE);

      for( j=0; j<anime_count; j++ )
      {
//        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, wk->anime_speed );
        GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &wk->animeFrame );
      }
    }
  }

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( wk->g3dUtil, wk->unit_idx );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
  GFL_G3D_DRAW_End();

  frame += anime_speed;
}



// BMPWIN�������p�\����
typedef struct{
  u32 frame;
  u8  x,y,w,h;
  u16 pal, chr;
}BMPWIN_DAT;


static const BMPWIN_DAT bmpwin_info_dat={
  BV_BGFRAME_U_MSG,
   2, 10,
  28, 12,
  15,  
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
  GFL_BMPWIN_Init( HEAPID_TR_CARD );
 
  // �T�u��ʁi��j��BMPWIN���m��
  windat = &bmpwin_info_dat;
  wk->InfoWin = GFL_BMPWIN_Create( windat->frame,
                                   windat->x, windat->y,
                                   windat->w, windat->h,
                                   windat->pal, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin), 0x0f0f );

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


//----------------------------------------------------------------------------------
/**
 * @brief ���E�C���h�E�̕\��ON�i��x�\��������OFF���Ȃ��j
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void InfoWinPut( BADGEVIEW_WORK *wk )
{
  // �E�C���h�E�g�`��
  BmpWinFrame_Write( wk->InfoWin, 0, SYSTEMWIN_FRM_CGX_OFFSET, SYSTEMWIN_FRM_PAL_OFFSET );
  // ���E�C���h�EON
  GFL_BMPWIN_MakeTransWindow( wk->InfoWin );

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
                              GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_TR_CARD );

  // �`��ҋ@�V�X�e��������
  wk->printQue = PRINTSYS_QUE_Create( HEAPID_TR_CARD );

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
static BOOL SEQ_Main( BADGEVIEW_WORK *wk )
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
  Trans_BadgePalette(wk);
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
  switch(wk->next_seq)
  {
  case TOUCH_END:
    if( GFL_CLACT_WK_CheckAnmActive( wk->clwk[BV_OBJ_END] )==FALSE){
      wk->seq = SUBSEQ_FADEOUT;
    } 
  case TOUCH_RETURN:
    if( GFL_CLACT_WK_CheckAnmActive( wk->clwk[BV_OBJ_RETURN] )==FALSE){
      wk->seq = SUBSEQ_FADEOUT;
    } 
  case TOUCH_CHANGE_CARD:
    if( GFL_CLACT_WK_CheckAnmActive( wk->clwk[BV_OBJ_CARD] )==FALSE){
      wk->seq = SUBSEQ_FADEOUT;
    } 
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
                  WIPE_FADE_BLACK, 16, 1, HEAPID_TR_CARD );
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
    if(wk->next_seq==TOUCH_END){
      wk->param->value     = CALL_NONE;
      wk->param->next_proc = TRAINERCARD_NEXTPROC_EXIT;
      wk->seq = 0;
    }else if(wk->next_seq==TOUCH_RETURN){
      wk->param->value = CALL_NONE;
      wk->param->next_proc = TRAINERCARD_NEXTPROC_RETURN;
      wk->seq = 0;
    }else if(wk->next_seq==TOUCH_CHANGE_CARD){
      wk->param->value = CALL_CARD;
      wk->seq = 0;
    }
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
 * @brief ���E�C���h�E�Ƀe�L�X�g��������
 *
 * @param   wk    
 * @param   type  INFO_TYPE_BADGE or INFO_TYPE_GYMLEADER
 * @param   no    0-7 (�o�b�W�ł��W�����[�_�[�ł��j
 */
//----------------------------------------------------------------------------------
static void InfoWinPrint( BADGEVIEW_WORK *wk, int type, int no )
{
  int i;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BADGEVIEW_MSG_LEN, HEAPID_TR_CARD );
  GF_ASSERT( type==INFO_TYPE_BADGE || type==INFO_TYPE_GYMLEADER );
  GF_ASSERT( no>=0 && no<8 );

  // �`��̈�N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin), 0x0f0f );

  switch(type){
  // �o�b�W���
  case INFO_TYPE_BADGE:   
    {
      // �擾����
      SHORT_DATE date = TRCSave_GetBadgeDate( wk->trCard, no );
    
      WORDSET_RegisterNumber( wk->wset, 0, date.year+2000,  4, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU);
      WORDSET_RegisterNumber( wk->wset, 1, date.month, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU);
      WORDSET_RegisterNumber( wk->wset, 2, date.day,   2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU);
    }
    // �`��i�P�s�ځj
    GFL_MSG_GetString( wk->mman, MSG_LEAGUE_BADGE1_01+no*3, strbuf );
    WORDSET_ExpandStr( wk->wset, wk->expbuf, strbuf );
    PRINT_UTIL_PrintColor( &wk->printUtil, wk->printQue, 
                           0, 0, wk->expbuf, wk->font, BV_COL_BLACK );
    
    // �`��i�Q�s�ځE�R�s�ځj
    for(i=1;i<3;i++){
      GFL_MSG_GetString( wk->mman, MSG_LEAGUE_BADGE1_01+no*3+i, strbuf );
      PRINT_UTIL_PrintColor( &wk->printUtil, wk->printQue, 
                             0, 32*i, strbuf, wk->font, BV_COL_BLACK );
    }
    break;

  // �W�����[�_�[���
  case INFO_TYPE_GYMLEADER:  
    for(i=0;i<3;i++){
      GFL_MSG_GetString( wk->mman, MSG_BADGE_LEADER1_01+no*3+i, strbuf );
      PRINT_UTIL_PrintColor( &wk->printUtil, wk->printQue, 
                             0, 32*i, strbuf, wk->font, BV_COL_BLACK );
    }
    break;
  }
  GFL_STR_DeleteBuffer( strbuf );

}

#define GYMREADER_FRAME_MAX ( 256*FX32_ONE)

//----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�t���[����max�ŉ�荞�܂��鏈���i�{�ɂ��|�ɂ��j
 *
 * @param   frame   
 * @param   max   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _round_frame( int frame, int max )
{
  int w;
  if(frame >= max){
    w = frame%max;
    frame = w;
  }else if(frame<0){
    w = frame;
    frame = GYMREADER_FRAME_MAX-frame;
  }
  return frame;
}


#define FRAME_SPEED_MIN     ( 4096 )
#define FRAME_SPEED_MINUS   ( 400 )


static void _speed_down( BADGEVIEW_WORK *wk )
{
  // ��]�X�s�[�h��������悤�Ɍv�Z(�ł��O�ɂ͂��Ȃ��j
  if(wk->animeSpeed>FRAME_SPEED_MIN){
    wk->animeSpeed-=FRAME_SPEED_MINUS;
  }else if(wk->animeSpeed<(-1*FRAME_SPEED_MIN)){
    wk->animeSpeed+=FRAME_SPEED_MINUS;
  }
  
}

static const int target_frame_tbl[]={
    0*FX32_ONE,  32*FX32_ONE,  64*FX32_ONE,  96*FX32_ONE,
  128*FX32_ONE, 160*FX32_ONE, 192*FX32_ONE, 224*FX32_ONE,
};

//----------------------------------------------------------------------------------
/**
 * @brief �W�����[�_�[�̃X�N���b�`�ړ�����
 *
 * @param   wk    
 * @param   flag    
 * @param   x   
 */
//----------------------------------------------------------------------------------
static void SlideFunc( BADGEVIEW_WORK *wk, int trg, int flag, int x )
{
  int i;
    // �^�b�`�J�n
    if(wk->slide.old_on==0 && flag){
      wk->old_animeFrame = wk->animeFrame;
      wk->slide.start_x        = x;
    //�^�b�`�p��  
    }else if(wk->slide.old_on==1 && flag==1){
      wk->animeFrame = wk->old_animeFrame + (x - wk->slide.start_x)*1024;
      wk->animeFrame = _round_frame( wk->animeFrame, GYMREADER_FRAME_MAX );
      
    // �^�b�`�I���i�����擾�j
    }else if(wk->slide.old_on==1 && flag==0){
      if(wk->slide.old2_x!=0){
        wk->animeSpeed = (wk->slide.old_x - wk->slide.old2_x)*2048;
      }
      if(wk->animeSpeed > 40000){
        wk->animeSpeed = 40000;
      }else if(wk->animeSpeed<-40000){
        wk->animeSpeed = -40000;
      }
    // �^�b�`���ĂȂ�
    }

  if(wk->slide.old_on==0 && flag==0){
    // �A�j���t���[���v�Z�i��荞�݂��j
    wk->animeFrame += wk->animeSpeed;
    wk->animeFrame = _round_frame( wk->animeFrame, GYMREADER_FRAME_MAX );

    // �ړ��悪�w�肳��Ă��鎞�̋���
    if(wk->targetSet>=0){
      if(wk->targetFrame > (wk->animeFrame-50000) && wk->targetFrame < (wk->animeFrame+50000)){
        wk->animeSpeed = 0;
        wk->animeFrame = wk->targetFrame;
        wk->targetSet = -1;
      }
    // �ړ���͎w�肳��Ă��Ȃ�
    }else{
      // �ړ��X�s�[�h���������Ă����Ƃ��Ɉ�ԋ߂����[�_�[�Ŏ~�܂�
      for(i=0;i<BADGE_NUM;i++){
        if(target_frame_tbl[i] > (wk->animeFrame-10000) && target_frame_tbl[i] < (wk->animeFrame+10000)){
          if(MATH_IAbs(wk->animeSpeed) < 10000){
            wk->animeFrame = target_frame_tbl[i];
            wk->animeSpeed = 0;
          }
        }
      }
      _speed_down(wk);
    }
  }

//  OS_Printf(" flag=%d, x=%d, old_x=%d, old2_x=%d, Frame=%d, speed=%d startx=%d\n", flag, x, wk->slide.old_x, wk->slide.old2_x,wk->animeFrame, wk->animeSpeed, wk->slide.start_x);
  
  wk->slide.old2_x  = wk->slide.old_x;
  wk->slide.old_x  = x;
  wk->slide.old_on = flag;

  

}

// -----------------------------------------
// �^�b�`���W��`
// -----------------------------------------
#define BADGE_TP_X    (  0 )
#define BADGE_TP_Y    ( 88 )
#define BADGE_TP_W    ( 32 )
#define BADGE_TP_H    ( 80 )
#define GR0_SX   (  95 )
#define GR0_SY   (  33 )
#define GR0_EX   ( 159 )
#define GR0_EY   (  87 )
#define GR1_SX   (  40 )
#define GR1_SY   (  23 )
#define GR1_EX   (  88 )
#define GR1_EY   (  71 )
#define GR2_SX   (  32 )
#define GR2_SY   (  16 )
#define GR2_EX   (  80 )
#define GR2_EY   (  23 )
#define GR3_SX   (  80 )
#define GR3_SY   (   0 )
#define GR3_EX   ( 103 )
#define GR3_EY   (  23 )
#define GR4_SX   ( 112 )
#define GR4_SY   (   0 )
#define GR4_EX   ( 143 )
#define GR4_EY   (  31 )
#define GR5_SX   ( 152 )
#define GR5_SY   (   0 )
#define GR5_EX   ( 176 )
#define GR5_EY   (  22 )
#define GR6_SX   ( 176 )
#define GR6_SY   (  15 )
#define GR6_EX   ( 223 )
#define GR6_EY   (  23 )
#define GR7_SX   ( 168 )
#define GR7_SY   (  23 )
#define GR7_EX   ( 215 )
#define GR7_EY   (  71 )

static const GFL_UI_TP_HITTBL TouchButtonHitTbl[] =
{
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*0,  BADGE_TP_X+BADGE_TP_W*1 },   // �o�b�W�O
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*1,  BADGE_TP_X+BADGE_TP_W*2 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*2,  BADGE_TP_X+BADGE_TP_W*3 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*3,  BADGE_TP_X+BADGE_TP_W*4 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*4,  BADGE_TP_X+BADGE_TP_W*5 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*5,  BADGE_TP_X+BADGE_TP_W*6 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*6,  BADGE_TP_X+BADGE_TP_W*7 },   // 
  { BADGE_TP_Y, BADGE_TP_Y+BADGE_TP_H,   BADGE_TP_X+BADGE_TP_W*7,  BADGE_TP_X+BADGE_TP_W*8 },   // 
  { GR0_SY, GR0_EY, GR0_SX, GR0_EX },   // �W�����[�_�[
  { GR1_SY, GR1_EY, GR1_SX, GR1_EX },   // 
  { GR2_SY, GR2_EY, GR2_SX, GR2_EX },   // 
  { GR3_SY, GR3_EY, GR3_SX, GR3_EX },   // 
  { GR4_SY, GR4_EY, GR4_SX, GR4_EX },   // 
  { GR5_SY, GR5_EY, GR5_SX, GR5_EX },   // 
  { GR6_SY, GR6_EY, GR6_SX, GR6_EX },   // 
  { GR7_SY, GR7_EY, GR7_SX, GR7_EX },   // 
  { 168, 191,    8,  8+32  },   // TOUCH_CHANGE_CARD
  { 168, 191, 20*8, 24*8-1 },   // TOUCH_SHORTCUT
  { 168, 191, 24*8, 28*8-1 },   // TOUCH_END
  { 168, 191, 28*8, 32*8-1 },   // TOUCH_RETURN
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};

// �W�����[�_�[�p�l�����h���b�O�ł���̈�`�F�b�N�p
static const GFL_UI_TP_HITTBL SlideTbl[]={
  { 32, 87, 40, 216, },
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
  int  trg, trg2, cont,cont2;
  u32  tp_x=0, tp_y=0;
  trg    = GFL_UI_TP_HitTrg( TouchButtonHitTbl );   // �^�b�`�g���K�[���
  cont2  = GFL_UI_TP_HitCont( TouchButtonHitTbl ); // �^�b�`�g���K�[���
  trg2   = GFL_UI_TP_HitTrg( SlideTbl );          // �^�b�`���
  cont   = GFL_UI_TP_HitCont( SlideTbl );         // �^�b�`���
  GFL_UI_TP_GetPointCont( &tp_x, &tp_y );         // �^�b�`���W

  // �L�[�ŃL�����Z��
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
    PMSND_PlaySE( SEQ_SE_CANCEL1 );
    trg = TOUCH_RETURN;
  }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X){
    PMSND_PlaySE( SEQ_SE_CANCEL1 );
    trg = TOUCH_END;
  }
  // ���y�[�W�E�E�y�[�W�E�߂�@�\�̌Ăяo��
  ExecFunc(wk, trg);
//  ContFunc(wk, cont2);

  // �W�����[�_�[���X���C�h�����鏈��
  SlideFunc( wk, trg2, cont+1, tp_x );

  // �o�b�W�𖁂�����
  BrushBadge( wk, cont2 );
  
  return trg;
}

//----------------------------------------------------------------------------------
/**
 * @brief �ǂ̃W�����[�_�[����Ԃ܂��ɂ��邩�H
 *
 * @param   frame   
 *
 * @retval  int   0-7
 */
//----------------------------------------------------------------------------------
static int _get_front_gymreader( int frame )
{
  int i;
  for(i=0;i<BADGE_NUM;i++){
    if(target_frame_tbl[i] > (frame-10000) && target_frame_tbl[i] < (frame+10000)){
      break;
    }
  }
  if(i==8){
    i = 0;
  }
  
  return i;
}


//----------------------------------------------------------------------------------
/**
 * @brief �e�@�\�i���y�[�W�E�E�y�[�W�E�߂�j�̎��s
 *
 * @param   wk    
 * @param   trg 
 */
//----------------------------------------------------------------------------------
static void ExecFunc( BADGEVIEW_WORK *wk, int trg )
{
  if(trg==GFL_UI_TP_HIT_NONE){
    return;
  }
  
  OS_Printf("trg=%d\n",trg);
  switch(trg){
  case TOUCH_BADGE_0: // �o�b�W�Ƀ^�b�`
  case TOUCH_BADGE_1:
  case TOUCH_BADGE_2:
  case TOUCH_BADGE_3:
  case TOUCH_BADGE_4:
  case TOUCH_BADGE_5:
  case TOUCH_BADGE_6:
  case TOUCH_BADGE_7:
    if(wk->badgeflag[trg-TOUCH_BADGE_0]){
      PMSND_PlaySE( SEQ_SE_SYS_32 );
      InfoWinPrint( wk, INFO_TYPE_BADGE, trg-TOUCH_BADGE_0 );
      InfoWinPut( wk );
      wk->targetSet   = TOUCH_BADGE_0;   // �W�����[�_�[�ړ���w��
      wk->targetFrame = target_frame_tbl[trg-TOUCH_BADGE_0];
      wk->animeSpeed  = 40000;
    }
    break;
  case TOUCH_GYM_READER_0:  // �W�����[�_�[�Ƀ^�b�`
  case TOUCH_GYM_READER_1:
  case TOUCH_GYM_READER_2:
  case TOUCH_GYM_READER_3:
  case TOUCH_GYM_READER_4:
  case TOUCH_GYM_READER_5:
  case TOUCH_GYM_READER_6:
  case TOUCH_GYM_READER_7:
    {
      int pos = _get_front_gymreader(wk->animeFrame);
      PMSND_PlaySE( SEQ_SE_SYS_31 );
      
      InfoWinPrint( wk, INFO_TYPE_GYMLEADER, (trg-TOUCH_GYM_READER_0+pos)%BADGE_NUM );
      InfoWinPut( wk );
    }
    break;
  case TOUCH_CHANGE_CARD:   // �J�[�h��ʂɈړ�
    wk->seq      = SUBSEQ_ICON_WAIT;
    wk->next_seq = trg;
    PMSND_PlaySE( SND_TRCARD_DECIDE );
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_CARD], 12 );
    break;
  case TOUCH_BOOKMARK:
    {
      int flag = GAMEDATA_GetShortCut( wk->param->gameData, SHORTCUT_ID_TRCARD_BADGE );
      flag ^= 1;
      GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_BOOKMARK], 6+flag );
      GAMEDATA_SetShortCut( wk->param->gameData, SHORTCUT_ID_TRCARD_BADGE, flag );
      PMSND_PlaySE( SND_TRCARD_BOOKMARK );
    }
    break;
  case TOUCH_END:           // �g���[�i�[�J�[�h�I�������ڃt�B�[���h��
    PMSND_PlaySE( SND_TRCARD_END );
    wk->seq = SUBSEQ_ICON_WAIT;
    wk->next_seq = trg;
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_END], 8 );
    break;
  case TOUCH_RETURN:        // �g���[�i�[�J�[�h�I�������j���[��
    PMSND_PlaySE( SND_TRCARD_CANCEL );
    wk->seq = SUBSEQ_ICON_WAIT;
    wk->next_seq = trg;
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_RETURN], 9 );
    break;
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief �^�b�`�������Ă�Ƃ��̏���
 *
 * @param   wk    
 * @param   cont    
 */
//----------------------------------------------------------------------------------
static void ContFunc( BADGEVIEW_WORK *wk, int cont )
{
  switch(cont){
  case TOUCH_BADGE_0:
  case TOUCH_BADGE_1:
  case TOUCH_BADGE_2:
  case TOUCH_BADGE_3:
  case TOUCH_BADGE_4:
  case TOUCH_BADGE_5:
  case TOUCH_BADGE_6:
  case TOUCH_BADGE_7:
    if(wk->badge.polish[cont-TOUCH_BADGE_0]!=255){
      wk->badge.polish[cont-TOUCH_BADGE_0]++;
    }
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

#define BADGE_EFFECT_NUM  ( 3 )

//----------------------------------------------------------------------------------
/**
 * @brief �o�b�W�̃L���L���̕\���E��\���ݒ�
 *
 * @param   wk    
 * @param   badge   
 * @param   enable    
 */
//----------------------------------------------------------------------------------
static void _badge_effect_enable( BADGEVIEW_WORK *wk, int badge, BOOL enable )
{
  int i;
  for(i=0;i<BADGE_EFFECT_NUM;i++){
    if(enable){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_KIRAKIRA0_0+badge*BADGE_EFFECT_NUM+i], enable );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_KIRAKIRA0_0+badge*BADGE_EFFECT_NUM+i], enable );
    }
  }
  
}

#define ISSHU_BADGE_NUM   ( 8 )

static int test_badge=0;

//----------------------------------------------------------------------------------
/**
 * @brief   �o�b�W�̕\���E��펞�ݒ�
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
  MISC *misc = GAMEDATA_GetMiscWork( wk->param->gameData );

#ifdef PM_DEBUG
  // L�{�^���������Ă���ƂЂƂ��o�b�W�t���O�������čs��
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    MISC_SetBadgeFlag(misc,test_badge);
    test_badge = (test_badge+1)%8;
  }
#endif

  for(i=0;i<ISSHU_BADGE_NUM;i++){
    if(  MISC_GetBadgeFlag(misc,i)){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_BADGE0+i],  TRUE );
      _badge_effect_enable( wk, i, TRUE );
      wk->badgeflag[i] = 1;
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[BV_OBJ_BADGE0+i],  FALSE );
      _badge_effect_enable( wk, i, FALSE );
      wk->badgeflag[i] = 0;
    }
  }
  // Y�{�^�����j���[�̓o�^��Ԃ𔽉f
  {
    int flag = GAMEDATA_GetShortCut( wk->param->gameData, SHORTCUT_ID_TRCARD_BADGE );
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[BV_OBJ_BOOKMARK], 6+flag );
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
  if(touch==TOUCH_BADGE_0){
    if( wk->page>0 ){   // 0�y�[�W�łȂ���΁|�P
      wk->page--;
      result=TRUE;
    }

  // �E�ړ��̎�
  }else if(touch==TOUCH_BADGE_1){
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

  // �o�b�W���W�����[�_�[�p���b�g�]��
  Trans_BadgePalette( wk );
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
  int num;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, HEAPID_TR_CARD );

  // �\�����鑍���擾
  num = _get_print_num( wk->page, wk->page_max, wk->trainer_num );

  // �v���[�g�`��
  //BgFramePrint( wk, i, strbuf, strbuf, strbuf );

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
/**
 * @brief �o�b�W��ʂ��������t��ۑ�
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void SetRtcData( BADGEVIEW_WORK *wk )
{
  RTCDate date;
  RTCTime time;
  s64     digit;
  int     i;

  // ���t�E���Ԏ擾
  GFL_RTC_GetDateTime( &date, &time);

  digit = RTC_ConvertDateTimeToSecond( &date, &time );

  // �ŏI�o�b�W�m�F���Ԃ��Z�[�u�f�[�^�ɃZ�b�g
  TRCSave_SetLastTime( wk->trCard, digit );

  // �����f�[�^���Z�b�g
  for(i=0;i<BADGE_NUM;i++){
    TRCSave_SetBadgePolish( wk->trCard, i, wk->badge.polish[i] );
  }
}



//----------------------------------------------------------------------------------
/**
 * @brief �o�b�W�̖����f�[�^���X�V����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void RefreshPolishData( BADGEVIEW_WORK *wk )
{
  RTCDate now_date, last_date;
  RTCTime now_time, last_time;
  s64 digit = TRCSave_GetLastTime( wk->trCard );
  s64 now_digit;
  
  RTC_ConvertSecondToDateTime( &last_date , &last_time, digit );

  OS_Printf("�Ō�̉{���F%02d/%02d/%02d %02d:%02d.%02d\n", last_date.year, last_date.month,  last_date.day, 
                                                           last_time.hour, last_time.minute, last_time.second);

  // ���t�E���Ԏ擾
  GFL_RTC_GetDateTime( &now_date, &now_time);

  OS_Printf("���F%02d/%02d/%02d %02d:%02d.%02d\n", now_date.year, now_date.month,  now_date.day, 
                                                   now_time.hour, now_time.minute, now_time.second);

  // �o�ߎ��Ԏ擾
  now_digit  = RTC_ConvertDateTimeToSecond( &now_date, &now_time );

  OS_Printf("��-�ߋ� %ld�b\n", now_digit-digit);
  
  now_digit -= digit;   // �����擾
  now_digit /= 60*60;   // ���Ԃɕϊ�

  // �����f�[�^�̎擾�E���Ԃ̍�������X�V( 2���łP�i�K������j
  {
    int i;
    for(i=0;i<BADGE_NUM;i++){
      wk->badge.polish[i] = TRCSave_GetBadgePolish( wk->trCard, i );
      wk->badge.polish[i] -= now_digit*3;
      if(wk->badge.polish[i]<0){
        wk->badge.polish[i] = 0;
      }
    }

    // �J�������ɕK�����f�����悤�ɂ���
    for(i=0;i<BADGE_NUM;i++){
      wk->badge.old_grade[i] = -1;
    }

  }
  

}

//----------------------------------------------------------------------------------
/**
 * @brief �o�b�W�̖������Ńp���b�g��������
 *
 * @param   wk    
 * @param   no    
 * @param   grade   
 */
//----------------------------------------------------------------------------------
static void _trans_badge_pal( BADGEVIEW_WORK *wk, int no, int grade )
{
  int index;
  NNSG2dPaletteData *pltt;
  u16 *buf;

  if(grade>2){
    grade=2;
  }
  
  pltt  = wk->badgePal[grade];
  buf   = (u16*)pltt->pRawData;
  index = no*16;
  
  GX_LoadOBJPltt( &buf[index], 32*no, 32);
  
}


//----------------------------------------------------------------------------------
/**
 * @brief �W�����[�_�[�̊�̃p���b�g������������
 *
 * @param   wk    
 * @param   no    
 * @param   grade   
 */
//----------------------------------------------------------------------------------
static void _trans_gymleader_pal( BADGEVIEW_WORK *wk, int no, int grade )
{
  const GFL_G3D_RES  *mdl_res;
  NNSG3dResTex *tex;
  NNSG3dResDictPlttData *texpal;
  u16 *pal;
  
  // 2�ȏ�̃O���[�h��2�ɂ���
  if( grade > 2 ){
    grade = 2;
  }
  pal = wk->leaderPal[grade]->pRawData; //�p���b�g�f�[�^�擾
  
  // ���f�����\�[�X�擾
  mdl_res = GFL_G3D_UTIL_GetResHandle( wk->g3dUtil, 0 );
  // �e�N�X�`�����\�[�X�擾
  tex     = GFL_G3D_GetResTex( mdl_res );       
  // �e�N�X�`������p���b�g�ʒu�擾
  texpal  = NNS_G3dGetPlttDataByIdx( tex, no );

  // �p���b�g�]��(�]���ʒu�̓e�N�X�`���p���b�g�{�I�t�Z�b�g�Ŋm�肳����j
  NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT, 
                                      tex->plttInfo.vramKey+(texpal->offset<<3), &pal[no*16], 32);

}

//----------------------------------------------------------------------------------
/**
 * @brief �o�b�W�̖������ɍ��킹�ăL���L�����o������
 *
 * @param   wk    
 * @param   no    
 * @param   grade 
 */
//----------------------------------------------------------------------------------
static void _set_clact_visible( BADGEVIEW_WORK *wk, int no, int grade )
{
  int num = grade-BADGE_POLISH_PAL_NUM;
  int i;
  
  for(i=0;i<BADGE_KIRAKIRA_NUM;i++){
    if(grade<3){
      GFL_CLACT_WK_SetDrawEnable( 
        wk->clwk[BV_OBJ_KIRAKIRA0_0+no*BADGE_KIRAKIRA_NUM+i],  FALSE );
    }else{
      if( i<=num ){
        GFL_CLACT_WK_SetDrawEnable( 
          wk->clwk[BV_OBJ_KIRAKIRA0_0+no*BADGE_KIRAKIRA_NUM+i],  TRUE );
      }else{
        GFL_CLACT_WK_SetDrawEnable( 
          wk->clwk[BV_OBJ_KIRAKIRA0_0+no*BADGE_KIRAKIRA_NUM+i],  FALSE );
      }
    }
  }
}



//----------------------------------------------------------------------------------
/**
 * @brief �o�b�W�̖�����ԂŃp���b�g�]����OBJ�\��������s��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Trans_BadgePalette( BADGEVIEW_WORK *wk )
{
  int i;
  for(i=0;i<BADGE_NUM;i++){
#ifdef PM_DEBUG
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_START){
      if(wk->badge.polish[i]!=0){
        wk->badge.polish[i]--;
      }
    }
#endif
    // �����l����O���[�h�Z�o
    wk->badge.grade[i] = wk->badge.polish[i]/BADGE_POLISH_RATE;
    if(wk->badge.grade[i]>5){
      wk->badge.grade[i] = 5;
    }
    
    // �ύX���|�����Ă����ꍇ�̓p���b�g�]���L���L��OBJ�\��������s��
    if(wk->badge.grade[i]!=wk->badge.old_grade[i]){
      _trans_badge_pal( wk, i, wk->badge.grade[i] );
      _trans_gymleader_pal( wk, i, wk->badge.grade[i] );
      _set_clact_visible( wk, i, wk->badge.grade[i] );
    }
    wk->badge.old_grade[i] = wk->badge.grade[i];
  }
#ifdef PM_DEBUG
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_START){
    OS_Printf("badge:");
    for(i=0;i<BADGE_NUM;i++){
      OS_Printf("%02d,",wk->badge.polish[i]);
    }
    OS_Printf("\n,");
  }
#endif
}


#define MIN_SCRUCH  (3)
#define MAX_SCRUCH  (40)
#define REV_SPEED (FX32_SHIFT - wk->RevSpeed)

static void _play_scratch_se( SCRATCH_WORK *sc )
{

  PMSND_PlaySE( SEQ_SE_SYS_33 );
  
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�W�����艹���䂷�邽�߂̌��ݕ������N���A
 *
 * @param *outScruchSnd �o�b�W�����艹�\���̂ւ̃|�C���^
 *
 * @return  none  
 */
//--------------------------------------------------------------------------------------------
static void ClearScratchSndNow( SCRATCH_WORK *sc )
{
  sc->DirX = 0;
  sc->DirY = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�W�����艹���䂷�邽�߂̃f�[�^���N���A
 *
 * @param *outScruchSnd �o�b�W�����艹�\���̂ւ̃|�C���^
 *
 * @return  none  
 */
//--------------------------------------------------------------------------------------------
static void ClearScratchSnd( SCRATCH_WORK *sc )
{
  sc->OldDirX = 0;
  sc->OldDirY = 0;
  sc->DirX = 0;
  sc->DirY = 0;
  sc->Snd = 0;
}


static void _badge_polish_up( BADGEVIEW_WORK *wk, int no )
{
  if(wk->badge.polish[no]<(BADGE_POLISH_RATE*6-1)){
    wk->badge.polish[no]++;
  }
  
}

//--------------------------------------------------------------------------------------------
/**
 * �o�b�W����
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BrushBadge( BADGEVIEW_WORK *wk, u32 touch )
{
  BOOL scruch=FALSE;
  int sub;
  u32 tp_x = 0xffff;
  u32 tp_y = 0xffff;
  
  GFL_UI_TP_GetPointCont( &tp_x, &tp_y );         // �^�b�`���W


  //�ێ����W�ƃ^�b�`���W�̍��������
  if( (tp_x!=0xffff)&&(tp_y!=0xffff)&&
      (wk->scratch.BeforeX!=0xffff)&&(wk->scratch.BeforeY!=0xffff) ){ //�l�L�����H
    //�ێ����W�ƃ^�b�`���W�������o�b�W���ɂ��邩�𒲂ׂ�
    if ( (touch!=GFL_UI_TP_HIT_NONE)&&(touch<BADGE_NUM)){
      //�o�b�W���L����
      if (wk->badgeflag[touch]){
        //�������K��l�ȏ�̏ꍇ�͖��������Ƃɂ���
        if ( wk->scratch.BeforeX > tp_x ){    //�O��̂ق����l���傫�����H
          sub = wk->scratch.BeforeX - tp_x;
          wk->scratch.DirX = -1;
        }else{
          sub = tp_x - wk->scratch.BeforeX;
          wk->scratch.DirX = 1;
        }
        if ( (sub>=MIN_SCRUCH)&&(sub<=MAX_SCRUCH) ){
          if ( wk->scratch.BeforeY > tp_y ){  //�O��̂ق����l���傫�����H
            sub = wk->scratch.BeforeY - tp_y;
            wk->scratch.DirY = -1;
          }else{
            sub = tp_y - wk->scratch.BeforeY;
            wk->scratch.DirY = 1;
          }
          if (sub<=MAX_SCRUCH){
            scruch = TRUE;      //��������
            _play_scratch_se( &wk->scratch );
          }else{
            ClearScratchSndNow(&wk->scratch);
          }
        }else if (sub<=MAX_SCRUCH){
          if ( wk->scratch.BeforeY > tp_y ){    //�O��̂ق����l���傫�����H
            sub = wk->scratch.BeforeY - tp_y;
            wk->scratch.DirY = -1;
          }else{
            sub = tp_y - wk->scratch.BeforeY;
            wk->scratch.DirY = 1;
          }
          if ((sub>=MIN_SCRUCH)&&(sub<=MAX_SCRUCH)){
            scruch = TRUE;    //��������
            _play_scratch_se( &wk->scratch );
          }else{
            ClearScratchSndNow(&wk->scratch);
          }
        }

        if (scruch){
          OS_Printf("scruch_%d\n",touch);
          OS_Printf("sc=%d,%d-%d,%d\n",wk->scratch.BeforeX,wk->scratch.BeforeY,tp_x,tp_y);
          //���������ԍX�V
          //���x���X�V
          _badge_polish_up(wk, touch);
        }
      }
    }
  }
    //�ێ����W�X�V
  wk->scratch.BeforeX = tp_x;
  wk->scratch.BeforeY = tp_y;
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