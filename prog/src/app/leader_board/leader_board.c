//============================================================================================
/**
 * @file    leader_board.c
 * @brief   ���[�_�[�{�[�h��ʁiWifi�o�g���T�u�E�F�C�_�E�����[�h�f�[�^�j
 * @author  Akito Mori
 * @date    10.02.22
 *
 * @todo 2��ROM�p�̉��f�[�^�����鏈���������Ă���̂ŁAWifi�T�[�o�[�������悤�ɂȂ����������
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"            // HEAPID_LEADERBOARD
#include "system/bgwinfrm.h"        // BGWINFRM_WORK
#include "system/cursor_move.h"     // CURSOR_MOVE
#include "system/gfl_use.h"         // GFUser_VIntr_CreateTCB
#include "system/wipe.h"            // WIPE_SYS_Start,
#include "sound/pm_sndsys.h"        // PMSND_PlaySE
#include "print/wordset.h"          // WORDSET
#include "print/printsys.h"         // PRINT_UTIL,PRINT_QUE...
#include "system/pms_data.h"        // PMS_DATA
#include "app/leader_board.h"       // proc
#include "app/app_menu_common.h"
#include "arc/arc_def.h"            // ARCID_LEADER_BOARD
#include "arc/leader_board.naix"    // leaderboard graphic
#include "battle/bsubway_battle_data.h" // BSUBWAY_LEADER_DATA

#include "font/font.naix"           // NARC_font_large_gftr
#include "arc/message.naix"         // NARC_message_leader_board_dat
#include "msg/msg_leader_board.h"   // 

#include "leader_board_def.h"       // �e���`��

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
#define LB_BGFRAME_U_MSG    ( GFL_BG_FRAME0_S )
#define LB_BGFRAME_U_BG     ( GFL_BG_FRAME1_S )
#define LB_BGFRAME_D_MSG    ( GFL_BG_FRAME0_M )
#define LB_BGFRAME_D_BUTTON ( GFL_BG_FRAME1_M )
#define LB_BGFRAME_D_BG     ( GFL_BG_FRAME2_M )



//--------------------------------------------------------------------------------------------
// �\���̒�`��`
//--------------------------------------------------------------------------------------------
typedef struct {
  ARCHANDLE     *g_handle;  // �O���t�B�b�N�f�[�^�t�@�C���n���h��
  
  BGWINFRM_WORK * wfrm;   // BGWINFRM

  BSUBWAY_LEADER_DATA *bSubwayData;   // �o�g���T�u�E�F�C�f�[�^

  GFL_FONT      *font;                // �t�H���g�f�[�^
  GFL_BMPWIN    *win[LB_BMPWIN_ID_MAX];
  PRINT_UTIL    printUtil[LB_BMPWIN_ID_MAX];
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printStream;
  GFL_TCBLSYS   *pMsgTcblSys;

  GFL_MSGDATA   *mman;    // ���b�Z�[�W�f�[�^�}�l�[�W��
  WORDSET       *wset;    // �P��Z�b�g
  STRBUF        *expbuf;  // ���b�Z�[�W�W�J�̈�
  STRBUF        *strbuf[LB_STR_MAX];

  GFL_CLUNIT    *clUnit;             // �Z���A�N�^�[���j�b�g
  GFL_CLUNIT    *pmsUnit;            // �ȈՉ�b�`��p�Z���A�N�^�[���j�b�g
  GFL_CLWK      *clwk[LB_OBJ_MAX];   // �Z���A�N�^�[���[�N
  u32           clres[LB_RES_MAX];   // �Z���A�N�^�[���\�[�X�p�C���f�b�N�X

  CURSORMOVE_WORK *cmwk;            // �J�[�\���ړ����[�N

  GFL_TCB         *VblankTcb;       // �o�^����VblankFunc�̃|�C���^
  
  LEADERBOARD_PARAM *param;         // �Ăяo���p�����[�^
  int seq,next_seq;                 // �T�u�V�[�P���X����p���[�N
  u16 page,page_max;                // ���݂̕\���y�[�W

  int trainer_num;                  // �o�g���T�u�E�F�C�f�[�^�ɑ��݂��Ă����l��

#ifdef PM_DEBUG
  GFL_MSGDATA   *debugname;
#endif
} LEADERBOARD_WORK;

//--------------------------------------------------------------------------------------------
// ���f�[�^��`
//--------------------------------------------------------------------------------------------
// �v���Z�X��`�f�[�^
const GFL_PROC_DATA LeaderBoardProcData = {
  LeaderBoardProc_Init,
  LeaderBoardProc_Main,
  LeaderBoardProc_End,
};



//--------------------------------------------------------------------------------------------
// �֐��v���g�^�C�v�錾
//--------------------------------------------------------------------------------------------
/*** �֐��v���g�^�C�v ***/
static void VBlankFunc( GFL_TCB *tcb, void * work );
static void InitWork( LEADERBOARD_WORK *wk, void *pwk );
static void FreeWork( LEADERBOARD_WORK *wk );
static void VramBankSet(void);
static void BgInit(LEADERBOARD_WORK *wk);
static void BgExit(LEADERBOARD_WORK *wk) ;
static void BgGraphicInit(LEADERBOARD_WORK *wk);
static void BgGraphicExit( LEADERBOARD_WORK *wk );
static void ClActInit(LEADERBOARD_WORK *wk);
static void ClActSet(LEADERBOARD_WORK *wk) ;
static void ClActExit(LEADERBOARD_WORK *wk);
static void BmpWinInit(LEADERBOARD_WORK *wk);
static void BmpWinExit(LEADERBOARD_WORK *wk);
static void PrintSystemInit(LEADERBOARD_WORK *wk);
static void PrintSystemDelete(LEADERBOARD_WORK *wk);
static void PrintSystem_Main( LEADERBOARD_WORK *wk );
static void BgFramePrint( LEADERBOARD_WORK *wk, int id, STRBUF *str, int gender );
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );
static BOOL LBSEQ_Main( LEADERBOARD_WORK *wk );
static void CursorMoveSys_Init( LEADERBOARD_WORK *wk );
static void CursorMoveSys_Exit( LEADERBOARD_WORK *wk );
static void CursorPosSet( LEADERBOARD_WORK *wk, int pos );
static  int _countup_bsubway( BSUBWAY_LEADER_DATA *bData );
static void Rewrite_UpperInformation( LEADERBOARD_WORK *wk, BSUBWAY_LEADER_DATA *bData, int id );
static  int TouchBar_KeyControl( LEADERBOARD_WORK *wk );
static void ExecFunc( LEADERBOARD_WORK *wk, int touch );
static void _page_max_init( LEADERBOARD_WORK *wk );
static void _page_clact_set( LEADERBOARD_WORK *wk, int page, int max );
static int _page_move_check( LEADERBOARD_WORK *wk, int touch );
static void SetupPage( LEADERBOARD_WORK *wk, int page );
static void NamePlatePrint_1Page( LEADERBOARD_WORK *wk );


#ifdef PM_DEBUG
static void _debug_data_set( LEADERBOARD_WORK *wk, BSUBWAY_LEADER_DATA *bData );
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
GFL_PROC_RESULT LeaderBoardProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  LEADERBOARD_WORK * wk;

  OS_Printf( "�����������@���[�_�[�{�[�h�����J�n�@����������\n" );

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_LEADERBOARD, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(LEADERBOARD_WORK), HEAPID_LEADERBOARD );
  MI_CpuClearFast( wk, sizeof(LEADERBOARD_WORK) );

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
  CursorMoveSys_Init(wk); // �J�[�\������V�X�e��������

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
GFL_PROC_RESULT LeaderBoardProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  LEADERBOARD_WORK * wk = mywk;

  if( LBSEQ_Main( wk ) == FALSE ){
    return GFL_PROC_RES_FINISH;
  }
  
  PrintSystem_Main( wk );         // ������`�惁�C��
  BGWINFRM_MoveMain( wk->wfrm );  // ���zBG�t���[�����C��
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
GFL_PROC_RESULT LeaderBoardProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  LEADERBOARD_WORK * wk = mywk;
  
  CursorMoveSys_Exit(wk);
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
 * VBlank�֐�
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
static void InitWork( LEADERBOARD_WORK *wk, void *pwk )
{
  wk->param     = pwk;
  wk->next_seq  = SUBSEQ_FADEIN_WAIT;

  // �O���t�B�b�N�f�[�^�n���h���I�[�v��
  wk->g_handle  = GFL_ARC_OpenDataHandle( ARCID_LEADER_BOARD, HEAPID_LEADERBOARD);

  // ���b�Z�[�W�}�l�[�W���[�m��  
  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_leader_board_dat, HEAPID_LEADERBOARD );


  // leader_board.gmm�̕������S�ēǂݍ���
  {
    int i;
    const STRCODE *buf;
    for(i=0;i<LB_STR_MAX;i++){
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
  
  // �o�g���T�u�E�F�CWifi�g���[�i�[�f�[�^�擾
  {
    BSUBWAY_WIFI_DATA *wifiData = GAMEDATA_GetBSubwayWifiData( wk->param->gamedata );
    wk->bSubwayData = BSUBWAY_WIFIDATA_GetLeaderDataAlloc( wifiData, HEAPID_LEADERBOARD );
  }


#ifdef PM_DEBUG
  wk->debugname = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                             NARC_message_trname_dat, HEAPID_LEADERBOARD );

  // �f�o�b�O�p�f�[�^�Z�b�g
  _debug_data_set( wk, wk->bSubwayData );

  GFL_MSG_Delete( wk->debugname );

#endif
  
  // �g���[�i�[�������グ
  wk->trainer_num = _countup_bsubway(wk->bSubwayData);

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
static void FreeWork( LEADERBOARD_WORK *wk )
{
  int i;
  
  // �o�g���T�u�E�F�C�g���[�i�[�f�[�^���
  GFL_HEAP_FreeMemory( wk->bSubwayData );
  
  // ��������
  for(i=0;i<LB_STR_MAX;i++){
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
  LB_BGFRAME_U_MSG,
  LB_BGFRAME_U_BG,
  LB_BGFRAME_D_MSG,
  LB_BGFRAME_D_BUTTON,
  LB_BGFRAME_D_BG,
};

//----------------------------------------------------------------------------------
/**
 * @brief BG�V�X�e��������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgInit(LEADERBOARD_WORK *wk)
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
        GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
        GX_BG_EXTPLTT_01,      0, 0, 0, FALSE
      },
      { // ���C����ʁF�{�^��
        0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
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
static void BgExit(LEADERBOARD_WORK *wk) 
{
  int i;
  for(i=0;i<NELEMS(bgframe_init_tbl);i++){
    GFL_BG_FreeBGControl( bgframe_init_tbl[i] );
  }
  GFL_BG_Exit();

}

// �v���[�g�̕\���ʒu�e�[�u��
static const name_plate_tbl[][2]={
  { LB_PLATE0_X,  LB_PLATE0_Y },   { LB_PLATE1_X,  LB_PLATE0_Y },
  { LB_PLATE0_X,  LB_PLATE2_Y },   { LB_PLATE1_X,  LB_PLATE2_Y },
  { LB_PLATE0_X,  LB_PLATE4_Y },   { LB_PLATE1_X,  LB_PLATE4_Y },
  { LB_PLATE0_X,  LB_PLATE6_Y },   { LB_PLATE1_X,  LB_PLATE6_Y },
  { LB_PLATE0_X,  LB_PLATE8_Y },   { LB_PLATE1_X,  LB_PLATE8_Y },
};
//----------------------------------------------------------------------------------
/**
 * @brief BG�O���t�B�b�N�]��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgGraphicInit(LEADERBOARD_WORK *wk)
{
  ARCHANDLE * handle = wk->g_handle;

  // �T�u��ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_leader_board_leaderb_u_lz_NCGR,
                                        LB_BGFRAME_U_BG, 0, 0, TRUE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_leader_board_leaderb_u_lz_NSCR,
                                        LB_BGFRAME_U_BG, 0, 0, TRUE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_leader_board_leaderb_u_NCLR, 
                                        PALTYPE_SUB_BG, 0, 0, HEAPID_LEADERBOARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_leader_board_leaderb_d_lz_NCGR,
                                        LB_BGFRAME_D_BG, 0, 0, TRUE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_leader_board_leaderb_d_lz_NSCR,
                                        LB_BGFRAME_D_BG, 0, 0, TRUE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramPalette(     handle, NARC_leader_board_leaderb_d_NCLR, 
                                        PALTYPE_MAIN_BG, 0, 0, HEAPID_LEADERBOARD );

  // ���C����ʔw�i�]��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_leader_board_leaderb_d_lz_NCGR,
                                        LB_BGFRAME_D_BUTTON, 0, 0, TRUE, HEAPID_LEADERBOARD );
  GFL_ARCHDL_UTIL_TransVramScreen(      handle, NARC_leader_board_leaderb_d_lz_NSCR,
                                        LB_BGFRAME_D_BUTTON, 0, 0, TRUE, HEAPID_LEADERBOARD );
  
  // �^�b�`�o�[BG�]��
  {
    ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_LEADERBOARD);

    GFL_ARCHDL_UTIL_TransVramBgCharacter( c_handle, APP_COMMON_GetBarCharArcIdx(),
                                          LB_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_LEADERBOARD );
    GFL_ARCHDL_UTIL_TransVramScreen(      c_handle, APP_COMMON_GetBarScrnArcIdx(),
                                          LB_BGFRAME_D_BUTTON, 0, 0, FALSE, HEAPID_LEADERBOARD );
    GFL_ARCHDL_UTIL_TransVramPaletteEx(   c_handle, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
                                          0, TOUCHBAR_PAL*32, 32, HEAPID_LEADERBOARD );
    
    // �X�N���[���̃p���b�g�w���ύX
    GFL_BG_ChangeScreenPalette( LB_BGFRAME_D_BUTTON, TOUCHBAR_X, TOUCHBAR_Y, 
                                                     TOUCHBAR_W, TOUCHBAR_H, TOUCHBAR_PAL );
    GFL_BG_LoadScreenReq( LB_BGFRAME_D_BUTTON );

    GFL_ARC_CloseDataHandle( c_handle );
  }
  
  // ��b�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_LEADERBOARD );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_LEADERBOARD );

  // ���zBG�t���[���ǂݍ���
  wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, 10, HEAPID_LEADERBOARD );
  {
    int i;
    for(i=0;i<10;i++){
      BGWINFRM_Add( wk->wfrm, LB_PLATE_ID+i, LB_BGFRAME_D_BUTTON, LB_PLATE_W, LB_PLATE_H );
      BGWINFRM_FrameSetArcHandle( wk->wfrm, LB_PLATE_ID+i, handle, 
                                  NARC_leader_board_leaderb_win_lz_NSCR, TRUE );
      BGWINFRM_FramePut( wk->wfrm, LB_PLATE_ID+i,
                         name_plate_tbl[i][0],name_plate_tbl[i][1] );
    }
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief BG�֘A�V�X�e���I������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BgGraphicExit( LEADERBOARD_WORK *wk )
{
  // ���zBG�t���[�����
  BGWINFRM_Exit( wk->wfrm );
}

//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�V�X�e��������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ClActInit(LEADERBOARD_WORK *wk)
{
  ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_LEADERBOARD);

  // �Z���A�N�^�[������
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &VramTbl, HEAPID_LEADERBOARD );
  wk->clUnit  = GFL_CLACT_UNIT_Create( LB_CLACT_NUM, 1,  HEAPID_LEADERBOARD );
  wk->pmsUnit = GFL_CLACT_UNIT_Create( LB_PMSDRAW_OBJ_NUM, 1,  HEAPID_LEADERBOARD );

  // OBJ�ʕ\��ON
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  // �Z���A�N�^�[���\�[�X�ǂݍ���
  
  // ---���[�_�[�{�[�h�p�����---
  wk->clres[LB_RES_CHR]  = GFL_CLGRP_CGR_Register(      wk->g_handle, 
                                                        NARC_leader_board_leaderb_obj_lz_NCGR, 1, 
                                                        CLSYS_DRAW_MAX, HEAPID_LEADERBOARD );
  wk->clres[LB_RES_PLTT] = GFL_CLGRP_PLTT_Register(     wk->g_handle, 
                                                        NARC_leader_board_leaderb_obj_NCLR, 
                                                        CLSYS_DRAW_MAX, 0, HEAPID_LEADERBOARD );
  wk->clres[LB_RES_CELL] = GFL_CLGRP_CELLANIM_Register( wk->g_handle, 
                                                        NARC_leader_board_leaderb_obj_NCER, 
                                                        NARC_leader_board_leaderb_obj_NANR, 
                                                        HEAPID_LEADERBOARD );

  // ���ʃ��j���[�f��
  wk->clres[LB_RES_COMMON_CHR] = GFL_CLGRP_CGR_Register(       c_handle, 
                                                               APP_COMMON_GetBarIconCharArcIdx(), 0, 
                                                               CLSYS_DRAW_MAIN, 
                                                               HEAPID_LEADERBOARD );

  wk->clres[LB_RES_COMMON_PLTT] = GFL_CLGRP_PLTT_RegisterEx(   c_handle, 
                                                               APP_COMMON_GetBarIconPltArcIdx(), 
                                                               CLSYS_DRAW_MAIN, 
                                                               11*32, 0, 3, HEAPID_LEADERBOARD );

  wk->clres[LB_RES_COMMON_CELL] = GFL_CLGRP_CELLANIM_Register( c_handle, 
                                                               APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K), 
                                                               APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                                               HEAPID_LEADERBOARD );
  GFL_ARC_CloseDataHandle( c_handle );

}


static const int clact_dat[][6]={
  //   X      Y  anm, chr,               pltt,               cell
  {  8*8,   2*8,   0, LB_RES_CHR,        LB_RES_PLTT,        LB_RES_CELL,      },// �J�[�\��
  { 28*8,  21*8,   1, LB_RES_COMMON_CHR, LB_RES_COMMON_CHR,  LB_RES_COMMON_CHR },// ���ǂ�
  {    8,  21*8,   4, LB_RES_COMMON_CHR, LB_RES_COMMON_CHR,  LB_RES_COMMON_CHR },// �����
  {   32,  21*8,   5, LB_RES_COMMON_CHR, LB_RES_COMMON_CHR,  LB_RES_COMMON_CHR },// �E���
};

//----------------------------------------------------------------------------------
/**
 * @brief �Z���A�N�^�[�o�^
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void ClActSet(LEADERBOARD_WORK *wk) 
{

  GFL_CLWK_DATA add;
  int i;

  // �Z���A�N�^�[�o�^
  for(i=0;i<LB_OBJ_MAX;i++){
    add.pos_x  = clact_dat[i][0];
    add.pos_y  = clact_dat[i][1];
    add.anmseq = clact_dat[i][2];
    add.bgpri  = 0;
    add.softpri  = 0;

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
static void ClActExit(LEADERBOARD_WORK *wk)
{

  GFL_CLGRP_CGR_Release(      wk->clres[LB_RES_COMMON_CHR] );
  GFL_CLGRP_PLTT_Release(     wk->clres[LB_RES_COMMON_PLTT] );
  GFL_CLGRP_CELLANIM_Release( wk->clres[LB_RES_COMMON_CELL] );
  GFL_CLGRP_CGR_Release(      wk->clres[LB_RES_CHR] );
  GFL_CLGRP_PLTT_Release(     wk->clres[LB_RES_PLTT] );
  GFL_CLGRP_CELLANIM_Release( wk->clres[LB_RES_CELL] );


  // �Z���A�N�^�[���j�b�g�j��
  GFL_CLACT_UNIT_Delete( wk->pmsUnit );
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

#include "lb_bmpwin.cdat"


//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BmpWinInit(LEADERBOARD_WORK *wk)
{
  int i;
  // BMPWIN�V�X�e���J�n
  GFL_BMPWIN_Init( HEAPID_LEADERBOARD );
 
  // ���ʂ�BMPWIN���m��
  for(i=0;i<SUB_BMPWIN_NUM;i++){
    const BMPWIN_DAT *windat = Window0BmpDataTable[i];
    wk->win[LB_BMPWIN_S_REKIDAI_NO_TOREINAA+i] = GFL_BMPWIN_Create( 
                                                  windat->frame,
                                                  windat->x, windat->y,
                                                  windat->w, windat->h,
                                                  windat->pal, GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_REKIDAI_NO_TOREINAA+i]), 0 );
    GFL_BMPWIN_MakeTransWindow( wk->win[LB_BMPWIN_S_REKIDAI_NO_TOREINAA+i] );
  }
  
  // ����ʂ�BMPWIN���m��
  for(i=0;i<MAIN_BMPWIN_NUM;i++){
    wk->win[LB_BMPWIN_M_NAME0+i] = GFL_BMPWIN_Create( LB_BGFRAME_D_BUTTON,
                                                   2,   1,
                                                  10,   2,
                                                   1,   GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_M_NAME0+i]), NAME_PLATE_CLEAR_COL );
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void BmpWinExit(LEADERBOARD_WORK *wk)
{
  int i;
  
  // ����ʗpBMPWIN�����
  for(i=0;i<MAIN_BMPWIN_NUM;i++){
    GFL_BMPWIN_Delete( wk->win[LB_BMPWIN_M_NAME0+i] );
  }
  // ���ʗpBMPWIN�����
  for(i=0;i<SUB_BMPWIN_NUM;i++){
    GFL_BMPWIN_Delete( wk->win[LB_BMPWIN_S_REKIDAI_NO_TOREINAA+i] );
  }
  // BMPWIN�V�X�e���I��
  GFL_BMPWIN_Exit();

}


#define LB_COL_WHITE      ( PRINTSYS_LSB_Make( 15, 14, 0) )    // ��
#define LB_COL_BLACK      ( PRINTSYS_LSB_Make(  1,  2, 0) )    // ��
#define LB_COL_NAME_WHITE ( PRINTSYS_LSB_Make( 13, 12, 4) )    // ���O�v���[�g�p�̔�
#define LB_COL_NAME_M     ( PRINTSYS_LSB_Make( 13, 12, 4) )    // �j�̖��O
#define LB_COL_NAME_W     ( PRINTSYS_LSB_Make( 11, 10, 4) )    // ���̖��O

//----------------------------------------------------------------------------------
/**
 * @brief �����`��V�X�e��������
 *

 */
//----------------------------------------------------------------------------------
static void PrintSystemInit(LEADERBOARD_WORK *wk)
{
  int i;
  // �t�H���g�m��
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr ,
                              GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_LEADERBOARD );

  // �`��ҋ@�V�X�e��������
  wk->printQue = PRINTSYS_QUE_Create( HEAPID_LEADERBOARD );

  // BMPWIN��PRINT_UTIL���֘A�t��
  for(i=0;i<LB_BMPWIN_ID_MAX;i++){
    PRINT_UTIL_Setup( &wk->printUtil[i], wk->win[i] );
  }

  // ���ʃ��x�������O�ɕ`�悵�Ă���
  // �u�ꂫ�����́@�g���[�i�[�v
  PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_REKIDAI_NO_TOREINAA], wk->printQue, 
                         0, 0, wk->strbuf[LB_STR_REKIDAI], wk->font, LB_COL_WHITE );
  // �u����ł�΂���v
  PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_SUNDERUBASHO], wk->printQue, 
                         0, 0, wk->strbuf[LB_STR_SUNDERU], wk->font, LB_COL_WHITE );
  // �u�������傤�����v
  PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_JIKOSHOUKAI], wk->printQue, 
                         0, 0, wk->strbuf[LB_STR_JIKOSHOUKAI], wk->font, LB_COL_WHITE );



  
}

//----------------------------------------------------------------------------------
/**
 * @brief �����`��V�X�e�����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void PrintSystemDelete(LEADERBOARD_WORK *wk)
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
static void PrintSystem_Main( LEADERBOARD_WORK *wk )
{
  int i;

  // �`��҂�
  PRINTSYS_QUE_Main( wk->printQue );

  // �eBMP���`��I���ςł���Γ]��
  for(i=0;i<LB_BMPWIN_ID_MAX;i++){
    PRINT_UTIL_Trans( &wk->printUtil[i], wk->printQue );
  }

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

static BOOL SubSuq_FadeinWait( LEADERBOARD_WORK *wk );
static BOOL SubSuq_Main( LEADERBOARD_WORK *wk );
static BOOL SubSeq_IconWait( LEADERBOARD_WORK *wk );
static BOOL SubSuq_FadeOut( LEADERBOARD_WORK *wk );
static BOOL SubSuq_FadeOutWait( LEADERBOARD_WORK *wk );


static BOOL (*functable[])(LEADERBOARD_WORK *wk) = {
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
static BOOL LBSEQ_Main( LEADERBOARD_WORK *wk )
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
static BOOL SubSuq_FadeinWait( LEADERBOARD_WORK *wk )
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
static BOOL SubSuq_Main( LEADERBOARD_WORK *wk )
{
  u32 ret;
  // �^�b�`�o�[�������ŗD��
  if(TouchBar_KeyControl(wk)==GFL_UI_TP_HIT_NONE){
    // �J�[�\�����䃁�C��
    ret=CURSORMOVE_Main( wk->cmwk );    
    if(ret==CURSORMOVE_NO_MOVE_LEFT){
      ExecFunc( wk, FUNC_LEFT_PAGE );
    }else if(ret==CURSORMOVE_NO_MOVE_RIGHT){
      ExecFunc( wk, FUNC_RIGHT_PAGE );
    }
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
static BOOL SubSeq_IconWait( LEADERBOARD_WORK *wk )
{
  if( GFL_CLACT_WK_CheckAnmActive( wk->clwk[LB_OBJ_END] )==FALSE){
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
static BOOL SubSuq_FadeOut( LEADERBOARD_WORK *wk )
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
static BOOL SubSuq_FadeOutWait( LEADERBOARD_WORK *wk )
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
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );

// ���[���I��
static const CURSORMOVE_DATA CursorSelTbl[] =
{
  {  64,  16, 0, 0, 0,  2, 0, 1, {   0,  31,   8, 119 },},    // 00: ���[���P
  { 192,  16, 0, 0, 1,  3, 0, 1, {   0,  31, 136, 244 },},    // 01: ���[���Q
  {  64,  48, 0, 0, 0,  4, 2, 3, {  32,  63,   8, 119 },},    // 02: ���[���R
  { 192,  48, 0, 0, 1,  5, 2, 3, {  32,  63, 136, 244 },},    // 03: ���[���S
  {  64,  80, 0, 0, 2,  6, 4, 5, {  64,  95,   8, 119 },},    // 04: ���[���T
  { 192,  80, 0, 0, 3,  7, 4, 5, {  64,  95, 136, 244 },},    // 05: ���[���U
  {  64, 112, 0, 0, 4,  8, 6, 7, {  96, 127,   8, 119 },},    // 06: ���[���V
  { 192, 112, 0, 0, 5,  9, 6, 7, {  96, 127, 136, 244 },},    // 07: ���[���W
  {  64, 144, 0, 0, 6,  8, 8, 9, { 128, 159,   8, 119 },},    // 08: ���[���X
  { 192, 144, 0, 0, 7,  9, 8, 9, { 128, 159, 136, 244 },},    // 09: ���[���P�O
  {   0,   0, 0, 0, 0,  0, 0, 0, { GFL_UI_TP_HIT_END, 0, 0, 0 },},

//  { 224, 176, 0, 0, CURSORMOVE_RETBIT|9, 10, 10, 10, { 160, 191, 192, 255 }, },   // 10: ��߂�
};
static const CURSORMOVE_CALLBACK SelCallBack = {
  CursorMoveCallBack_On,
  CursorMoveCallBack_Off,
  CursorMoveCallBack_Move,
  CursorMoveCallBack_Touch
};



//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ�������
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveSys_Init( LEADERBOARD_WORK *wk )
{
  int pos;

  pos = 0;

  wk->cmwk = CURSORMOVE_Create(
              CursorSelTbl,
              &SelCallBack,
              wk,
              TRUE,
              pos,
              HEAPID_LEADERBOARD );

  CursorPosSet( wk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * �J�[�\���ړ��폜
 *
 * @param syswk ���[���{�b�N�X��ʃV�X�e�����[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveSys_Exit( LEADERBOARD_WORK * wk )
{
  CURSORMOVE_Exit( wk->cmwk );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\���\��
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos )
{
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\����\��
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos )
{
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�J�[�\���ړ�
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
  LEADERBOARD_WORK *wk = work;
  
  // ���ʏ�������
  Rewrite_UpperInformation( wk, wk->bSubwayData, wk->page*LB_1PAGE_NUM+now_pos );
  CursorPosSet( work, now_pos );
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//--------------------------------------------------------------------------------------------
/**
 * �R�[���o�b�N�֐��F�^�b�`
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param now_pos   ���݂̈ʒu
 * @param old_pos   �O��̈ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
  LEADERBOARD_WORK *wk = work;

  // ���ʏ�������
  Rewrite_UpperInformation( wk, wk->bSubwayData, wk->page*LB_1PAGE_NUM+now_pos );
  CursorPosSet( work, now_pos );
  PMSND_PlaySE( SEQ_SE_SELECT1 );
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���I���̃J�[�\���ʒu�Z�b�g
 *
 * @param work    ���[���{�b�N�X��ʃV�X�e�����[�N
 * @param pos     �ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void CursorPosSet( LEADERBOARD_WORK *wk, int pos )
{
  const CURSORMOVE_DATA *cd = CURSORMOVE_GetMoveData( wk->cmwk, pos );
  GFL_CLACTPOS clpos;
  clpos.x = cd->px;
  clpos.y = cd->py;

  GFL_CLACT_WK_SetPos( wk->clwk[LB_OBJ_CURSOR], &clpos, CLSYS_DEFREND_MAIN );

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
 * @brief �o�g���T�u�E�F�C�f�[�^�̒��ɉ��l�����Ă��邩�擾����
 *
 * @param   bData   BSUBWAY_LEADER_DATA�̔z��̃|�C���^
 *
 * @retval  int   �g���[�i�[�����݂��Ă���l��
 */
//----------------------------------------------------------------------------------
static int _countup_bsubway( BSUBWAY_LEADER_DATA *bData )
{
  int i,result=0;
  for(i=0;i<BSUBWAY_STOCK_WIFI_LEADER_MAX;i++){
    // ���O�̂P�����ڂ��O���ᖳ�������瑶�݂��Ă��鎖�ɂ���
//    if(bData[i].name[0]!=GFL_STR_GetEOMCode()){
    if(bData[i].name[0]!=0){
      result++;
    }
  }

  OS_Printf("�g���[�i�[�f�[�^��%d�C\n", result);
  return result;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���ʏ�񏑂�����
 *
 * @param   wk    LEADERBOARD_WORK
 * @param   bData BSUBWAY_LEADER_DATA
 * @param   id    ���Ԗڂ�
 */
//----------------------------------------------------------------------------------
static void Rewrite_UpperInformation( LEADERBOARD_WORK *wk, BSUBWAY_LEADER_DATA *bData, int id )
{
  int nation     = bData[id].country_code;
  int local      = bData[id].address;
  PMS_DATA *pms  = (PMS_DATA*)bData[id].leader_word;
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
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_RANK]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_TRAIN_NO]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_NAME]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_COUTRY]), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_S_PMS]), 0 );

  // �`��

  // �g���[�i�[���𒴂��Ă��Ȃ��Ȃ�`��
  if( id<(wk->trainer_num)){
  
    // �g���[�i�[��
    GFL_STR_SetStringCodeOrderLength( strbuf, bData[id].name, PERSON_NAME_SIZE );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_NAME], wk->printQue, 
                           0, 0, strbuf, wk->font, LB_COL_WHITE );
    // �����NNO
    WORDSET_ExpandStr( wk->wset, wk->expbuf, wk->strbuf[LB_STR_RANk] );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_RANK], wk->printQue, 
                           0, 0, wk->expbuf, wk->font, LB_COL_WHITE );
    // �g���C��NO
    WORDSET_ExpandStr( wk->wset, wk->expbuf, wk->strbuf[LB_STR_TRAINNO] );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_TRAIN_NO], wk->printQue, 
                           0, 0, wk->expbuf, wk->font, LB_COL_WHITE );
    // ��
    WORDSET_ExpandStr( wk->wset, wk->expbuf, wk->strbuf[LB_STR_COUNTRY] );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_COUTRY], wk->printQue, 
                           0, 0, wk->expbuf, wk->font, LB_COL_WHITE );
    OS_Printf("��������̒�����%d code=%d\n", GFL_STR_GetBufferLength(wk->expbuf), nation);
  
    // �Z��ł�ꏊ
    WORDSET_ExpandStr( wk->wset, wk->expbuf, wk->strbuf[LB_STR_LOCAL] );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_COUTRY], wk->printQue, 
                           0, 16, wk->expbuf, wk->font, LB_COL_WHITE );
    OS_Printf("�Z��ł�ꏊ������̒�����%d code=%d\n", GFL_STR_GetBufferLength(wk->expbuf),local);
    
    
    // �ȈՉ�b��W�J
    pmsstr = PMSDAT_ToString( pms, HEAPID_LEADERBOARD );
    PRINT_UTIL_PrintColor( &wk->printUtil[LB_BMPWIN_S_PMS], wk->printQue, 
                           0, 0, pmsstr, wk->font, LB_COL_WHITE );
    GFL_STR_DeleteBuffer( pmsstr );
  }else{
    // �͈͊O�������̂ŃN���A������Ԃ�]��
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[LB_BMPWIN_S_NAME] );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[LB_BMPWIN_S_COUTRY] );
    GFL_BMPWIN_MakeTransWindow_VBlank( wk->win[LB_BMPWIN_S_PMS] );
    
  }
  
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
static int TouchBar_KeyControl( LEADERBOARD_WORK *wk )
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
static void ExecFunc( LEADERBOARD_WORK *wk, int touch )
{
  switch(touch){
  case FUNC_LEFT_PAGE:
    if(_page_move_check( wk, touch )){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      GFL_CLACT_WK_SetAnmSeq( wk->clwk[LB_OBJ_LEFT],  12 );
      SetupPage( wk, wk->page );
    }
    break;
  case FUNC_RIGHT_PAGE:
    if(_page_move_check( wk, touch )){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      GFL_CLACT_WK_SetAnmSeq( wk->clwk[LB_OBJ_RIGHT], 13 );
      SetupPage( wk, wk->page );
    }
    break;
  case FUNC_END:
    wk->seq = SUBSEQ_ICON_WAIT;
    GFL_CLACT_WK_SetAnmSeq( wk->clwk[LB_OBJ_END], 9 );
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
static void _page_max_init( LEADERBOARD_WORK *wk )
{
  wk->page = 0;
  wk->page_max = wk->trainer_num /LB_1PAGE_NUM;
  if(wk->trainer_num%LB_1PAGE_NUM){
    wk->page_max++;
  }
  OS_Printf("page=%d, trainer_num=%d, max=%d\n", wk->page, wk->trainer_num, wk->page_max);
}

//----------------------------------------------------------------------------------
/**
 * @brief   ���A�C�R���̏�Ԃ��y�[�W���ŕω�������
 *
 * @param   wk    
 * @param   page    
 * @param   max   
 */
//----------------------------------------------------------------------------------
static void _page_clact_set( LEADERBOARD_WORK *wk, int page, int max )
{
  // �P�y�[�W�����Ȃ���������\�����Ȃ�
  if(max==1){
    GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_LEFT],  FALSE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_RIGHT], FALSE );
  }else{
  // �����y�[�W
    if(page==0){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_LEFT],  FALSE );
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_RIGHT], TRUE );
    }else if(page!=0 && page < (max-1)){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_LEFT],  TRUE );
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_RIGHT], TRUE );
    }else if(page==(max-1)){
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_LEFT],  TRUE );
      GFL_CLACT_WK_SetDrawEnable( wk->clwk[LB_OBJ_RIGHT], FALSE );
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
static int _page_move_check( LEADERBOARD_WORK *wk, int touch )
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
static void SetupPage( LEADERBOARD_WORK *wk, int page )
{
  wk->page = page;

  // �^�b�`�o�[�\���ݒ�
  _page_clact_set( wk, page, wk->page_max );

  // �v���[�g��ԕύX
  NamePlatePrint_1Page( wk );

  // ���ʕ\���ݒ�
  Rewrite_UpperInformation( wk, wk->bSubwayData, 
                            wk->page*LB_1PAGE_NUM+CURSORMOVE_PosGet( wk->cmwk ) );
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
  num = trainer_num-page*LB_1PAGE_NUM;

  if(num > LB_1PAGE_NUM){
    num = LB_1PAGE_NUM;
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
static void NamePlatePrint_1Page( LEADERBOARD_WORK *wk )
{
  int i,num;
  STRBUF *strbuf = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, HEAPID_LEADERBOARD );

  // �\�����鑍���擾
  num = _get_print_num( wk->page, wk->page_max, wk->trainer_num );

  // �v���[�g�`��
  for(i=0;i<num;i++){
    GFL_STR_SetStringCodeOrderLength( strbuf, wk->bSubwayData[i+wk->page*LB_1PAGE_NUM].name,
                                      PERSON_NAME_SIZE );
    BgFramePrint( wk, i, strbuf, wk->bSubwayData[i+wk->page*LB_1PAGE_NUM].gender );
  }
  // �\�����Ȃ������c��̓v���[�g��OFF�ɂ���
  for(;i<LB_1PAGE_NUM;i++){
    BGWINFRM_FrameOff( wk->wfrm, i );
  }
  
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
static void BgFramePrint( LEADERBOARD_WORK *wk, int id, STRBUF *str, int gender )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[LB_BMPWIN_M_NAME0+id]), 
                 NAME_PLATE_CLEAR_COL );
  if(gender==0)  {
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->win[LB_BMPWIN_M_NAME0+id]), 
                                          0, 0, str, wk->font, LB_COL_NAME_M );
  }else{
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->win[LB_BMPWIN_M_NAME0+id]), 
                                          0, 0, str, wk->font, LB_COL_NAME_W );
  }
  GFL_BMPWIN_TransVramCharacter( wk->win[LB_BMPWIN_M_NAME0+id] );
  BGWINFRM_BmpWinOn( wk->wfrm, LB_PLATE_ID+id, wk->win[LB_BMPWIN_M_NAME0+id] );
  BGWINFRM_FrameOn( wk->wfrm,  LB_PLATE_ID+id );
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

static PMS_DATA debug_pms[3]={
  {1,1,{1,1} },{2,2,{2,2} },{3,2,{3,3} },
};

  // �f�o�b�O�p�f�[�^�Z�b�g
static void _debug_data_set( LEADERBOARD_WORK *wk, BSUBWAY_LEADER_DATA *bData )
{
  int i, num=0;
  STRBUF *str;
  PMS_DATA *pms;

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
    GFL_STR_GetStringCode( str, bData[i].name, 6 );
    GFL_STR_DeleteBuffer( str );
    pms = (PMS_DATA*)bData[i].leader_word;
    *pms = debug_pms[i%3];
    bData[i].country_code = GFUser_GetPublicRand(20)+1;
    bData[i].address      = 0;
    bData[i].gender       = GFUser_GetPublicRand(2);
  }
}

#endif