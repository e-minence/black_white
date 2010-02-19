//======================================================================
/**
 * @file	battle_championship.c
 * @brief	���j���[
 * @author	ariizumi
 * @author  Toru=Nagihashi ���p
 * @data	09/10/08 -> 100112
 *
 * ���W���[�����FBATTLE_CHAMPINONSHIP
 */
//======================================================================
#include <gflib.h>
#include "buflen.h"

#include "app/app_taskmenu.h"
#include "gamesystem/msgspeed.h"
#include "print/printsys.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_regulation.h"
#include "system/bmp_winframe.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/bmp_menulist.h"
#include "app/app_keycursor.h"

#include "arc_def.h"
#include "battle_championship.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_battle_championship.h"

#include "title/title.h"
#include "net_app/irc_battle.h"
#include "net_app/wifibattlematch.h"
#include "net_app/digitalcardcheck.h"

#include "battle_championship/battle_championship.h"
#include "battle_championship/battle_championship_def.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define BATTLE_CHAMPIONSHIP_FRAME_MENU ( GFL_BG_FRAME0_M )
#define BATTLE_CHAMPIONSHIP_FRAME_STR ( GFL_BG_FRAME1_M )
#define BATTLE_CHAMPIONSHIP_FRAME_BG  ( GFL_BG_FRAME3_M )

#define BATTLE_CHAMPIONSHIP_FRAME_SUB_BG  ( GFL_BG_FRAME3_S )

#define BATTLE_CHAMPIONSHIP_BG_PAL_WINFRAME (0xD)
#define BATTLE_CHAMPIONSHIP_BG_PAL_FONT (0xE)

#define BATTLE_CHAMPIONSHIP_FRAME_MENU_CGX (1)
#define BATTLE_CHAMPIONSHIP_FRAME_STR_CGX (1)

#define BATTLE_CHAMPIONSHIP_MENU_ITEM_NUM (4)

#define BATTLE_CHAMPIONSHIP_MENU_WIDTH (24)
#define BATTLE_CHAMPIONSHIP_MENU_TOP  (4)
#define BATTLE_CHAMPIONSHIP_MENU_LEFT (4)

#define BATTLE_CHAMPIONSHIP_MEMBER_NUM (2)
//-------------------------------------
/// LIST
//=====================================
#define BC_LIST_SELECT_NULL  (BMPMENULIST_NULL)
#define BC_LIST_WINDOW_MAX   (5)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//�؂�ւ���Proc���
typedef enum
{
  BCNP_TITLE,
  BCNP_WIFI_BATTLE,
  BCNP_EVENT_BATTLE,
  
}BATTLE_CHAMPIONSHIP_NEXT_PROC;

//-------------------------------------
///	�`�����
//=====================================
typedef enum
{
  BC_TEXT_TYPE_QUE,     //�v�����g�L���[���g��
  BC_TEXT_TYPE_STREAM,  //�X�g���[�����g��

  BC_TEXT_TYPE_MAX,    //c�����ɂĎg�p
} BC_TEXT_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//-------------------------------------
///	���b�Z�[�W�E�B���h�E
//=====================================
typedef struct _BC_TEXT_WORK BC_TEXT_WORK;

//-------------------------------------
///	�I�����X�g
//=====================================
typedef enum
{ 
  BC_LIST_POS_CENTER,
  BC_LIST_POS_RIGHTDOWN,
}BC_LIST_POS;
typedef struct 
{
  GFL_MSGDATA *p_msg;
  GFL_FONT    *p_font;
  PRINT_QUE   *p_que;
  u32 strID[BC_LIST_WINDOW_MAX];
  u32 list_max;

  u16 frm;
  u16 font_plt;
  u16 frm_plt;
  u16 frm_chr;
  BC_LIST_POS pos;
} BC_LIST_SETUP;
typedef struct _BC_LIST_WORK BC_LIST_WORK;

//-------------------------------------
///	�V�[�P���X���[�N
//=====================================
typedef struct _BC_SEQ_WORK BC_SEQ_WORK;
typedef void (*BC_SEQ_FUNCTION)( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
struct _BC_SEQ_WORK
{
	BC_SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
	int seq;											//���s���̃V�[�P���X�֐��̒��̃V�[�P���X
	void *p_wk_adrs;							//���s���̃V�[�P���X�֐��ɓn�����[�N
  int reserv_seq;               //�\��V�[�P���X
} ;

//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct
{
  HEAPID heapId;
  BATTLE_CHAMPIONSHIP_NEXT_PROC nextProcType;
  
  //�����
  BATTLE_CHAMPIONSHIP_DATA csData;
  
  //���b�Z�[�W�p
  BC_TEXT_WORK    *text;
  GFL_FONT        *fontHandle;
  GFL_MSGDATA     *msgHandle;
  PRINT_QUE       *taskMenuQue;

  //�V�[�P���X�Ǘ�
  BC_SEQ_WORK     seq;
  
  //taskmenu
  BC_LIST_WORK    *list;

}BATTLE_CHAMPIONSHIP_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};

//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

//-------------------------------------
///	�O���t�B�b�N
//=====================================
static void BATTLE_CHAMPIONSHIP_InitGraphic( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void BATTLE_CHAMPIONSHIP_TermGraphic( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_LoadResource( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_ReleaseResource( BATTLE_CHAMPIONSHIP_WORK *p_wk );

//-------------------------------------
///	���b�Z�[�W
//=====================================
static void BATTLE_CHAMPIONSHIP_InitMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_TermMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_UpdateMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static void BATTLE_CHAMPIONSHIP_ShowMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk  , const u16 msgNo );
static const BOOL BATTLE_CHAMPIONSHIP_IsFinishMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk );

//-------------------------------------
///	���j���[
//=====================================
typedef enum
{ 
  BC_MENU_TYPE_FIRST,
  BC_MENU_TYPE_WIFI,
  BC_MENU_TYPE_LIVE,
  BC_MENU_TYPE_YESNO,
}BC_MENU_TYPE;
static void BATTLE_CHAMPIONSHIP_OpenFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk, BC_MENU_TYPE type );
static void BATTLE_CHAMPIONSHIP_CloseFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk );
static const u32 BATTLE_CHAMPIONSHIP_UpdateFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk );

//-------------------------------------
///	�e�L�X�g
//=====================================
static BC_TEXT_WORK * BC_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
static void BC_TEXT_Exit( BC_TEXT_WORK* p_wk );
static void BC_TEXT_Main( BC_TEXT_WORK* p_wk );
static void BC_TEXT_Print( BC_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, BC_TEXT_TYPE type );
static void BC_TEXT_PrintBuf( BC_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, BC_TEXT_TYPE type );
static BOOL BC_TEXT_IsEndPrint( const BC_TEXT_WORK *cp_wk );
static void BC_TEXT_WriteWindowFrame( BC_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt );

//-------------------------------------
///	LIST
//=====================================
static BC_LIST_WORK * BC_LIST_Init( const BC_LIST_SETUP *cp_setup, HEAPID heapID );
static void BC_LIST_Exit( BC_LIST_WORK *p_wk );
static u32 BC_LIST_Main( BC_LIST_WORK *p_wk );

//-------------------------------------
///	�V�[�P���X
//=====================================
static void BC_SEQ_Init( BC_SEQ_WORK *p_wk, void *p_wk_adrs, BC_SEQ_FUNCTION seq_function, HEAPID heapID );
static void BC_SEQ_Exit( BC_SEQ_WORK *p_wk );
static void BC_SEQ_Main( BC_SEQ_WORK *p_wk );
static BOOL BC_SEQ_IsEnd( const BC_SEQ_WORK *cp_wk );
static void BC_SEQ_SetNext( BC_SEQ_WORK *p_wk, BC_SEQ_FUNCTION seq_function );
static void BC_SEQ_End( BC_SEQ_WORK *p_wk );
static void BC_SEQ_SetReservSeq( BC_SEQ_WORK *p_wk, int seq );
static void BC_SEQ_NextReservSeq( BC_SEQ_WORK *p_wk );

//-------------------------------------
///	�V�[�P���X�֐�
//=====================================
static void SEQFUNC_Start( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeIn( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeOut( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainMenu( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Info( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_WifiMenu( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_WifiInfo( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_DigitalCard( BC_SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );

//--------------------------------------------------------------
//  �O�����J�v���Z�X
//--------------------------------------------------------------
GFL_PROC_DATA BATTLE_CHAMPIONSHIP_ProcData =
{
  BATTLE_CHAMPIONSHIP_ProcInit,
  BATTLE_CHAMPIONSHIP_ProcMain,
  BATTLE_CHAMPIONSHIP_ProcTerm
};
//--------------------------------------------------------------
//	
//--------------------------------------------------------------
#pragma mark [>proc 
static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  BATTLE_CHAMPIONSHIP_WORK *p_wk;
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_CHAMPIONSHIP, 0x18000 );

  p_wk = GFL_PROC_AllocWork( proc, sizeof(BATTLE_CHAMPIONSHIP_WORK), HEAPID_BATTLE_CHAMPIONSHIP );
  GFL_STD_MemClear( p_wk, sizeof(BATTLE_CHAMPIONSHIP_WORK) );
  p_wk->nextProcType  = BCNP_TITLE;
  p_wk->heapId = HEAPID_BATTLE_CHAMPIONSHIP;

  BATTLE_CHAMPIONSHIP_InitGraphic( p_wk );
  BATTLE_CHAMPIONSHIP_LoadResource( p_wk );
  BATTLE_CHAMPIONSHIP_InitMessage( p_wk );

  BC_SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeIn, HEAPID_BATTLE_CHAMPIONSHIP );
  
  //���f�[�^�쐬
  {
    BATTLE_CHAMPIONSHIP_SetDebugData( &p_wk->csData , p_wk->heapId );
  }
  
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  BATTLE_CHAMPIONSHIP_WORK *p_wk = mywk;

  //���f�[�^�j��
  BATTLE_CHAMPIONSHIP_TermDebugData( &p_wk->csData );

  BC_SEQ_Exit( &p_wk->seq);

  BATTLE_CHAMPIONSHIP_TermMessage( p_wk );
  BATTLE_CHAMPIONSHIP_ReleaseResource( p_wk );
  BATTLE_CHAMPIONSHIP_TermGraphic( p_wk );

  switch( p_wk->nextProcType )
  { 
  default:
    GF_ASSERT(0);
  case BCNP_TITLE:
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
    break;
  case BCNP_WIFI_BATTLE:
    { 
      WIFIBATTLEMATCH_PARAM *p_param;
      p_param = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(WIFIBATTLEMATCH_PARAM) );
      GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_PARAM) );
      p_param->mode             = WIFIBATTLEMATCH_MODE_WIFI;
      p_param->is_auto_release  = TRUE;
      GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(wifibattlematch_sys), &WifiBattleMatch_ProcData, p_param );
    }
    break;

  case BCNP_EVENT_BATTLE:
    GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &IRC_BATTLE_ProcData, NULL);
    break;
  }

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_BATTLE_CHAMPIONSHIP );

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT BATTLE_CHAMPIONSHIP_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  BATTLE_CHAMPIONSHIP_WORK *p_wk = mywk;
  
  BC_SEQ_Main( &p_wk->seq );

  if( BC_SEQ_IsEnd( &p_wk->seq ) )
  { 
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

#pragma mark [>graphic func
static void BATTLE_CHAMPIONSHIP_InitGraphic( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  GFL_DISP_SetBank( &vramBank );

  //BG�n�̏�����
  GFL_BG_Init( p_wk->heapId );
  GFL_BMPWIN_Init( p_wk->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };

    // BG0 MAIN (���j���[
    static const GFL_BG_BGCNT_HEADER header_main0 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 MAIN (����
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
/*
    // BG2 MAIN (
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
*/
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x04000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );

    BATTLE_CHAMPIONSHIP_SetupBgFunc( &header_main3 , BATTLE_CHAMPIONSHIP_FRAME_BG , GFL_BG_MODE_TEXT );
    BATTLE_CHAMPIONSHIP_SetupBgFunc( &header_main1 , BATTLE_CHAMPIONSHIP_FRAME_STR , GFL_BG_MODE_TEXT );
    BATTLE_CHAMPIONSHIP_SetupBgFunc( &header_main0 , BATTLE_CHAMPIONSHIP_FRAME_MENU , GFL_BG_MODE_TEXT );

    BATTLE_CHAMPIONSHIP_SetupBgFunc( &header_sub3 , BATTLE_CHAMPIONSHIP_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
  }
}
//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void BATTLE_CHAMPIONSHIP_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

static void BATTLE_CHAMPIONSHIP_TermGraphic( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  GFL_BG_FreeBGControl( BATTLE_CHAMPIONSHIP_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( BATTLE_CHAMPIONSHIP_FRAME_MENU );
  GFL_BG_FreeBGControl( BATTLE_CHAMPIONSHIP_FRAME_STR );
  GFL_BG_FreeBGControl( BATTLE_CHAMPIONSHIP_FRAME_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

static void BATTLE_CHAMPIONSHIP_LoadResource( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_BATTLE_CHAMPIONSHIP , p_wk->heapId );

  ////BG���\�[�X
  //���ʔw�i
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_battle_championship_connect_NCLR , 
                    PALTYPE_MAIN_BG , 0, 0x20 * 10, p_wk->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_battle_championship_connect_sub_NCGR ,
                    BATTLE_CHAMPIONSHIP_FRAME_BG , 0 , 0, FALSE , p_wk->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_battle_championship_connect_sub_NSCR , 
                    BATTLE_CHAMPIONSHIP_FRAME_BG ,  0 , 0, FALSE , p_wk->heapId );
  //����ʔw�i
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_battle_championship_connect_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , p_wk->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_battle_championship_connect_NCGR ,
                    BATTLE_CHAMPIONSHIP_FRAME_SUB_BG , 0 , 0, FALSE , p_wk->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_battle_championship_connect_01_NSCR , 
                    BATTLE_CHAMPIONSHIP_FRAME_SUB_BG ,  0 , 0, FALSE , p_wk->heapId );

  GFL_ARC_CloseDataHandle( arcHandle );

  //�t���[���g
  GFL_BG_FillCharacter( BATTLE_CHAMPIONSHIP_FRAME_STR, 0, 1, 0 );
  BmpWinFrame_GraphicSet( BATTLE_CHAMPIONSHIP_FRAME_STR, BATTLE_CHAMPIONSHIP_FRAME_MENU_CGX, BATTLE_CHAMPIONSHIP_BG_PAL_WINFRAME, MENU_TYPE_SYSTEM, p_wk->heapId );
  GFL_BG_FillCharacter( BATTLE_CHAMPIONSHIP_FRAME_MENU, 0, 1, 0 );
  BmpWinFrame_GraphicSet( BATTLE_CHAMPIONSHIP_FRAME_MENU, BATTLE_CHAMPIONSHIP_FRAME_STR_CGX, BATTLE_CHAMPIONSHIP_BG_PAL_WINFRAME, MENU_TYPE_SYSTEM, p_wk->heapId );
  

  //�t�H���g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , BATTLE_CHAMPIONSHIP_BG_PAL_FONT*0x20, 0x20, p_wk->heapId );
  
}

static void BATTLE_CHAMPIONSHIP_ReleaseResource( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  GFL_BG_FillCharacterRelease( BATTLE_CHAMPIONSHIP_FRAME_MENU, 1, 0 );
  GFL_BG_FillCharacterRelease( BATTLE_CHAMPIONSHIP_FRAME_STR, 1, 0 );
}

#pragma mark [>message func
static void BATTLE_CHAMPIONSHIP_InitMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  p_wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , p_wk->heapId );
  
  p_wk->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_battle_championship_dat , p_wk->heapId );
  
  p_wk->taskMenuQue = PRINTSYS_QUE_Create( p_wk->heapId );
  
  //���b�Z�[�W
  p_wk->text    = BC_TEXT_Init( BATTLE_CHAMPIONSHIP_FRAME_STR, BATTLE_CHAMPIONSHIP_BG_PAL_FONT, p_wk->taskMenuQue, p_wk->fontHandle, p_wk->heapId );
  BC_TEXT_WriteWindowFrame( p_wk->text, BATTLE_CHAMPIONSHIP_FRAME_STR_CGX, BATTLE_CHAMPIONSHIP_BG_PAL_WINFRAME );
}

static void BATTLE_CHAMPIONSHIP_TermMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  BC_TEXT_Exit( p_wk->text );

  PRINTSYS_QUE_Delete( p_wk->taskMenuQue );
  
  GFL_MSG_Delete( p_wk->msgHandle );
  GFL_FONT_Delete( p_wk->fontHandle );
}

static void BATTLE_CHAMPIONSHIP_UpdateMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  BC_TEXT_Main( p_wk->text );
  PRINTSYS_QUE_Main( p_wk->taskMenuQue );
}
static void BATTLE_CHAMPIONSHIP_ShowMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk  , const u16 msgNo )
{
  BC_TEXT_Print( p_wk->text, p_wk->msgHandle, msgNo, BC_TEXT_TYPE_STREAM );
}

static const BOOL BATTLE_CHAMPIONSHIP_IsFinishMessage( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  return BC_TEXT_IsEndPrint( p_wk->text );
}

#pragma mark [>menu func
static void BATTLE_CHAMPIONSHIP_OpenFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk, BC_MENU_TYPE type )
{
  BC_LIST_SETUP  setup;
  GFL_STD_MemClear( &setup, sizeof(BC_LIST_SETUP) );
  setup.p_msg   = p_wk->msgHandle;
  setup.p_font  = p_wk->fontHandle;
  setup.p_que   = p_wk->taskMenuQue;


  switch( type )
  { 
  case BC_MENU_TYPE_FIRST:
    setup.strID[0]= BC_SELECT_01;
    setup.strID[1]= BC_SELECT_02;
    setup.strID[2]= BC_SELECT_04;
    setup.strID[3]= BC_SELECT_05;
    setup.list_max= 4;
    setup.pos     = BC_LIST_POS_CENTER;
    break;
  case BC_MENU_TYPE_WIFI:
    setup.strID[0]= BC_SELECT_06;
    setup.strID[1]= BC_SELECT_04;
    setup.strID[2]= BC_SELECT_03;
    setup.strID[3]= BC_SELECT_05;
    setup.list_max= 4;
    setup.pos     = BC_LIST_POS_CENTER;
    break;
  case BC_MENU_TYPE_LIVE:
    setup.strID[0]= BC_SELECT_05;
    setup.list_max= 1;
    setup.pos     = BC_LIST_POS_CENTER;
    break;
  case BC_MENU_TYPE_YESNO:
    setup.strID[0]= BC_SELECT_07;
    setup.strID[1]= BC_SELECT_08;
    setup.list_max= 2;
    setup.pos     = BC_LIST_POS_RIGHTDOWN;
    break;
  }
  setup.frm     = BATTLE_CHAMPIONSHIP_FRAME_MENU;
  setup.font_plt= BATTLE_CHAMPIONSHIP_BG_PAL_FONT;
  setup.frm_plt = BATTLE_CHAMPIONSHIP_BG_PAL_WINFRAME;
  setup.frm_chr = BATTLE_CHAMPIONSHIP_FRAME_MENU_CGX;
  p_wk->list  = BC_LIST_Init( &setup, p_wk->heapId );
}

static void BATTLE_CHAMPIONSHIP_CloseFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  BC_LIST_Exit( p_wk->list );
}

static const u32 BATTLE_CHAMPIONSHIP_UpdateFirstMenu( BATTLE_CHAMPIONSHIP_WORK *p_wk )
{
  return BC_LIST_Main( p_wk->list );;
}

#pragma mark [>debug
//�f�o�b�O�p
void BATTLE_CHAMPIONSHIP_SetDebugData( BATTLE_CHAMPIONSHIP_DATA *csData , const HEAPID heapId )
{
  u8 i;
  csData->name[0] = L'��';
  csData->name[1] = L'��';
  csData->name[2] = L'��';
  csData->name[3] = L'��';
  csData->name[4] = L'��';
  csData->name[5] = L'�[';
  csData->name[6] = L'��';
  csData->name[7] = 0xFFFF;
  csData->number = 10;
  csData->league = 8;
  csData->regulation = (REGULATION*)PokeRegulation_LoadDataAlloc( REG_DEBUG_BATTLE , heapId );
  
  for( i=0;i<6;i++ )
  {
    csData->ppp[i] = GFL_HEAP_AllocMemory( heapId , POKETOOL_GetPPPWorkSize() );
    PPP_Clear( csData->ppp[i] );
//    if( i<5 )
    {
      PPP_Setup( csData->ppp[i] , i+1 , 10 , PTL_SETUP_ID_AUTO );
      {
        const u16 oyaName[5] = {L'�u',L'��',L'�b',L'�N',0xFFFF};
        PPP_Put( csData->ppp[i] , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
        PPP_Put( csData->ppp[i] , ID_PARA_oyasex , PTL_SEX_MALE );
      }
    }
  }
}

void BATTLE_CHAMPIONSHIP_TermDebugData( BATTLE_CHAMPIONSHIP_DATA *csData )
{
  u8 i;
  GFL_HEAP_FreeMemory( csData->regulation );
  for( i=0;i<6;i++ )
  {
    GFL_HEAP_FreeMemory( csData->ppp[i] );
  }
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�e�L�X�g�`��\����
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
#define BC_TEXT_TYPE_NONE  (BC_TEXT_TYPE_MAX)

//-------------------------------------
/// ���b�Z�[�W�E�B���h�E
//=====================================
struct _BC_TEXT_WORK
{
  GFL_MSGDATA       *p_msg;
  GFL_FONT          *p_font;
  PRINT_STREAM      *p_stream;
  GFL_TCBLSYS       *p_tcbl;
  GFL_BMPWIN*       p_bmpwin;
  STRBUF*           p_strbuf;
  u16               clear_chr;
  u16               heapID;
  PRINT_UTIL        util;
  PRINT_QUE         *p_que;
  u32               print_update;
  BOOL              is_end_print;
  APP_KEYCURSOR_WORK* p_keycursor;
} ;

//-------------------------------------
///	�v���g�^�C�v
//=====================================
static void BC_TEXT_PrintInner( BC_TEXT_WORK* p_wk, BC_TEXT_TYPE type );

//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  ������
 *
 *	@param	u16 frm       BG��
 *	@param	font_plt      �t�H���g�p���b�g
 *	@param  PRINT_QUE     �v�����gQ
 *	@param  GFL_FONT      �t�H���g
 *	@param	heapID        �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BC_TEXT_WORK * BC_TEXT_Init( u16 frm, u8 font_plt, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{ 
  BC_TEXT_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BC_TEXT_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BC_TEXT_WORK) );
  p_wk->clear_chr = 0xF;
  p_wk->p_font    = p_font;
  p_wk->p_que     = p_que;
  p_wk->print_update  = BC_TEXT_TYPE_NONE;

  //��������J�[�\���쐬
  p_wk->p_keycursor  = APP_KEYCURSOR_Create( p_wk->clear_chr, TRUE, FALSE, heapID );

  //�o�b�t�@�쐬
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 512, heapID );

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( frm, 1, 19, 30, 4, font_plt, GFL_BMP_CHRAREA_GET_B );

	//�v�����g�L���[�ݒ�
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//�]��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

  p_wk->p_tcbl    = GFL_TCBL_Init( heapID, heapID, 1, 32 );

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �j��
 *
 *	@param	BC_TEXT_WORK* p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BC_TEXT_Exit( BC_TEXT_WORK* p_wk )
{ 
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  GFL_TCBL_Exit( p_wk->p_tcbl );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );

  GFL_STR_DeleteBuffer( p_wk->p_strbuf );

  APP_KEYCURSOR_Delete( p_wk->p_keycursor );

  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �`�揈��
 *
 *	@param	BC_TEXT_WORK* p_wk   ���[�N
 *
 */
//-----------------------------------------------------------------------------
void BC_TEXT_Main( BC_TEXT_WORK* p_wk )
{ 
  switch( p_wk->print_update )
  { 
  default:
    /* fallthor */
  case BC_TEXT_TYPE_NONE:
    break;

  case BC_TEXT_TYPE_QUE:
    p_wk->is_end_print  = PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
    break;

  case BC_TEXT_TYPE_STREAM:
    if( p_wk->p_stream )
    { 
      PRINTSTREAM_STATE state;
      state  = PRINTSYS_PrintStreamGetState( p_wk->p_stream );

      APP_KEYCURSOR_Main( p_wk->p_keycursor, p_wk->p_stream, p_wk->p_bmpwin );

      switch( state )
      { 
      case PRINTSTREAM_STATE_RUNNING:  ///< �������s���i�����񂪗���Ă���j

        // ���b�Z�[�W�X�L�b�v
        if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
        {
          PRINTSYS_PrintStreamShortWait( p_wk->p_stream, 0 );
        }
        break;

      case PRINTSTREAM_STATE_PAUSE:    ///< �ꎞ��~���i�y�[�W�؂�ւ��҂����j

        //���s
        if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL) )
        { 
          PRINTSYS_PrintStreamReleasePause( p_wk->p_stream );
        }
        break;

      case PRINTSTREAM_STATE_DONE:     ///< ������I�[�܂ŕ\������
        p_wk->is_end_print  = TRUE;
        break;
      }
    }
    break;
  }

  GFL_TCBL_Main( p_wk->p_tcbl );

}
//----------------------------------------------------------------------------
/*
 *	@brief  �e�L�X�g �v�����g�J�n
 *
 *	@param	BC_TEXT_WORK* p_wk ���[�N
 *	@param	*p_msg            ���b�Z�[�W
 *	@param	strID             ���b�Z�[�WID
 *	@param  type              �`��^�C�v
 */
//-----------------------------------------------------------------------------
void BC_TEXT_Print( BC_TEXT_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, BC_TEXT_TYPE type )
{ 
  //������쐬
  GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //�����`��
  BC_TEXT_PrintInner( p_wk, type );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�J�n  �o�b�t�@��
 *
 *	@param	BC_TEXT_WORK* p_wk ���[�N
 *	@param	STRBUF *cp_strbuf       �����o�b�t�@
 *	@param	type                    �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
void BC_TEXT_PrintBuf( BC_TEXT_WORK* p_wk, const STRBUF *cp_strbuf, BC_TEXT_TYPE type )
{ 
  //������쐬
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //�����`��
  BC_TEXT_PrintInner( p_wk, type );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�����`��v���C�x�[�g
 *
 *	@param	BC_TEXT_WORK* p_wk ���[�N
 *	@param	type              �`��^�C�v
 *
 */
//-----------------------------------------------------------------------------
static void BC_TEXT_PrintInner( BC_TEXT_WORK* p_wk, BC_TEXT_TYPE type )
{ 
  //��[����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), p_wk->clear_chr );

  //�X�g���[���j��
  if( p_wk->p_stream )
  {
    PRINTSYS_PrintStreamDelete( p_wk->p_stream );
    p_wk->p_stream  = NULL;
  }

  //�^�C�v���Ƃ̕����`��
  switch( type )
  { 
  case BC_TEXT_TYPE_QUE:     //�v�����g�L���[���g��
    PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, 0, 0, p_wk->p_strbuf, p_wk->p_font );
    p_wk->print_update  = BC_TEXT_TYPE_QUE;
    break;

  case BC_TEXT_TYPE_STREAM:  //�X�g���[�����g��
    p_wk->p_stream  = PRINTSYS_PrintStream( p_wk->p_bmpwin, 0, 0, p_wk->p_strbuf,
        p_wk->p_font, MSGSPEED_GetWait(), p_wk->p_tcbl, 0, p_wk->heapID, p_wk->clear_chr );
    p_wk->print_update  = BC_TEXT_TYPE_STREAM;
    break;
  }

  p_wk->is_end_print  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �v�����g�I���҂�  �iQUE�̏ꍇ��QUE�I���ASTREAM�̂Ƃ��͍Ō�܂ŕ����`��I���j
 *
 *	@param	const BC_TEXT_WORK *cp_wk ���[�N
 *
 *	@return TRUE�Ȃ�Ε����`�抮��  FALSE�Ȃ�΍Œ��B
 */
//-----------------------------------------------------------------------------
BOOL BC_TEXT_IsEndPrint( const BC_TEXT_WORK *cp_wk )
{ 
  return cp_wk->is_end_print;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g  �g��`��
 *
 *	@param	BC_TEXT_WORK *p_wk   ���[�N
 *	@param  �t���[���̃L����
 *	@param  �t���[���̃p���b�g
 */
//-----------------------------------------------------------------------------
void BC_TEXT_WriteWindowFrame( BC_TEXT_WORK *p_wk, u16 frm_chr, u8 frm_plt )
{ 
  BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_ON, frm_chr, frm_plt );
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *				  ���X�g
 *				    �E�ȒP�Ƀ��X�g���o�����߂ɍő�l���߂���
 *            �E�\���������X�g�ő吔
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================/
#define BC_LIST_W  (28)
//-------------------------------------
///	�I�����X�g
//=====================================
struct _BC_LIST_WORK
{ 
  GFL_BMPWIN        *p_bmpwin;
  PRINT_QUE         *p_que;
  PRINT_UTIL        print_util;
  BMPMENULIST_WORK  *p_list;
  BMP_MENULIST_DATA *p_list_data;
};
//-------------------------------------
///	�p�u���b�N
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	BC_LIST_SETUP *cp_setup  ���X�g�ݒ胏�[�N
 *	@param	heapID                    heapID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
BC_LIST_WORK * BC_LIST_Init( const BC_LIST_SETUP *cp_setup, HEAPID heapID )
{ 
  BC_LIST_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(BC_LIST_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(BC_LIST_WORK) );
  p_wk->p_que = cp_setup->p_que;

  //BMPWIN�쐬
  { 
    u8 w,h,x,y;
    switch( cp_setup->pos)
    { 
    case BC_LIST_POS_CENTER:
      //�����Ɉʒu���邽��
      //�\�����ڂ���ʒu�A�������v�Z
      w  = BC_LIST_W;
      h  = cp_setup->list_max * 2;
      x  = 32 / 2 - w / 2; 
      y  = (24 - 6) / 2 - h / 2;
      break;
    case BC_LIST_POS_RIGHTDOWN:
      //�E���A�e�L�X�g�{�b�N�X�̏�Ɉʒu���邽��
      //�\�����ڂ���ʒu�A�������v�Z
      w  = 8;
      h  = cp_setup->list_max * 2;
      x  = 32 - w - 1;
      y  = 24 - 6 - h - 1;
      break;
    }

    p_wk->p_bmpwin  = GFL_BMPWIN_Create( cp_setup->frm, x, y, w, h, cp_setup->font_plt, GFL_BMP_CHRAREA_GET_B );
    BmpWinFrame_Write( p_wk->p_bmpwin, WINDOW_TRANS_OFF, cp_setup->frm_chr, cp_setup->frm_plt );
    GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

    PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
  }
  //���X�g�f�[�^�쐬
  { 
    int i;
    p_wk->p_list_data = BmpMenuWork_ListCreate( cp_setup->list_max, heapID );
    for( i = 0; i < cp_setup->list_max; i++ )
    { 
      BmpMenuWork_ListAddArchiveString( p_wk->p_list_data,
          cp_setup->p_msg, cp_setup->strID[ i ], i, heapID );
    }
  }
  //���X�g�쐬
  { 
    static const BMPMENULIST_HEADER sc_def_header =
    { 
      NULL,
      NULL,
      NULL,
      NULL,

      0,  //count
      0,  //line
      0,  //rabel_x
      13, //data_x  
      0,  //cursor_x
      3,  //line_y
      1,  //f
      15, //b
      2,  //s
      0,  //msg_spc
      1,  //line_spc
      BMPMENULIST_NO_SKIP,  //page_skip
      0,  //font
      0,  //c_disp_f
      NULL,
      12,
      12,

      NULL,
      NULL,
      NULL,
      NULL,
      
    };
    BMPMENULIST_HEADER  header;
    header  = sc_def_header;

    header.list         = p_wk->p_list_data;
    header.count        = cp_setup->list_max;
    header.line         = cp_setup->list_max;
    header.win          = p_wk->p_bmpwin;
    header.msgdata      = cp_setup->p_msg;
    header.print_util   = &p_wk->print_util;
    header.print_que    = cp_setup->p_que;
    header.font_handle  = cp_setup->p_font;
    p_wk->p_list  = BmpMenuList_Set( &header, 0, 0, heapID );

    BmpMenuList_SetCursorBmp( p_wk->p_list, heapID );
    BmpMenuList_SetCancelMode( p_wk->p_list, BMPMENULIST_CANCELMODE_NOT );
    
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j������
 *
 *	@param	BC_LIST_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BC_LIST_Exit( BC_LIST_WORK *p_wk )
{ 
  BmpMenuList_Exit( p_wk->p_list, NULL, NULL );
  BmpMenuWork_ListDelete( p_wk->p_list_data );

  BmpWinFrame_Clear( p_wk->p_bmpwin, WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearScreen( p_wk->p_bmpwin );
  GFL_BMPWIN_Delete( p_wk->p_bmpwin );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g���C������
 *
 *	@param	BC_LIST_WORK *p_wk   ���[�N
 *
 *	@return �I�����Ă��Ȃ��Ȃ��BC_LIST_SELECT_NULL ����ȊO�Ȃ�ΑI���������X�g�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
u32 BC_LIST_Main( BC_LIST_WORK *p_wk )
{ 
  PRINT_UTIL_Trans( &p_wk->print_util, p_wk->p_que );
  return BmpMenuList_Main( p_wk->p_list );
}

//-------------------------------------
///	�V�[�P���X
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	BC_SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void BC_SEQ_Init( BC_SEQ_WORK *p_wk, void *p_wk_adrs, BC_SEQ_FUNCTION seq_function, HEAPID heapID )
{	
	//�쐬
	GFL_STD_MemClear( p_wk, sizeof(BC_SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	BC_SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	BC_SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
void BC_SEQ_Exit( BC_SEQ_WORK *p_wk )
{
	GFL_STD_MemClear( p_wk, sizeof(BC_SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	BC_SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void BC_SEQ_Main( BC_SEQ_WORK *p_wk )
{	
	if( p_wk->seq_function )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const BC_SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
BOOL BC_SEQ_IsEnd( const BC_SEQ_WORK *cp_wk )
{	
	return cp_wk->seq_function == NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	BC_SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
void BC_SEQ_SetNext( BC_SEQ_WORK *p_wk, BC_SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	BC_SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
void BC_SEQ_End( BC_SEQ_WORK *p_wk )
{	
  BC_SEQ_SetNext( p_wk, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ ���̃V�[�P���X��\��
 *
 *	@param	BC_SEQ_WORK *p_wk  ���[�N
 *	@param	seq             ���̃V�[�P���X
 */
//-----------------------------------------------------------------------------
void BC_SEQ_SetReservSeq( BC_SEQ_WORK *p_wk, int seq )
{ 
  p_wk->reserv_seq  = seq;
}
//----------------------------------------------------------------------------
/**
 *	@brief  SEQ �\�񂳂ꂽ�V�[�P���X�֔��
 *
 *	@param	BC_SEQ_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BC_SEQ_NextReservSeq( BC_SEQ_WORK *p_wk )
{ 
  p_wk->seq = p_wk->reserv_seq;
}
//=============================================================================
/**
 *  �V�[�P���X�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_seqwk_adrs )
{ 
  BC_SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_seqwk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_seqwk_adrs )
{ 
  //�I��
  BC_SEQ_End( p_seqwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�C��
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_seqwk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_seqwk_adrs )
{ 
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		BC_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
		break;

	default:
		GF_ASSERT(0);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t�F�[�h�A�E�g
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		BC_SEQ_SetNext( p_seqwk, SEQFUNC_End );
		break;

	default:
		GF_ASSERT(0);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�����j���[
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainMenu( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    SEQ_START_FIRSTMSG,
    SEQ_WAIT_FIRSTMSG,
    SEQ_START_MENU,
    SEQ_WAIT_MENU,
  };
  BATTLE_CHAMPIONSHIP_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FIRSTMSG:
    BATTLE_CHAMPIONSHIP_ShowMessage( p_wk, BC_STR_01 );
    *p_seq = SEQ_WAIT_FIRSTMSG;
    break;

  case SEQ_WAIT_FIRSTMSG:
    BATTLE_CHAMPIONSHIP_UpdateMessage( p_wk );
    if( BATTLE_CHAMPIONSHIP_IsFinishMessage( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_MENU;
    }
    break;

  case SEQ_START_MENU:
    BATTLE_CHAMPIONSHIP_OpenFirstMenu( p_wk, BC_MENU_TYPE_FIRST );
    *p_seq = SEQ_WAIT_MENU;
    break;

  case SEQ_WAIT_MENU:
    {
      const u32 ret = BATTLE_CHAMPIONSHIP_UpdateFirstMenu( p_wk );
      if( ret != BC_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //WiFi���
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
          break;
        case 1: //�C�x���g���
          //BC_SEQ_SetNext( p_seqwk, SEQFUNC_LiveMenu );
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
          break;
        case 2: //����������
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_Info );
          break;
        case 3: //��߂�
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
          break;
        }
        BATTLE_CHAMPIONSHIP_CloseFirstMenu( p_wk );
      }
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�������
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Info( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_INFOMSG,
    SEQ_WAIT_INFOMSG,
  };

  BATTLE_CHAMPIONSHIP_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_INFOMSG:
    BATTLE_CHAMPIONSHIP_ShowMessage( p_wk, BC_STR_02 );
    *p_seq  = SEQ_WAIT_INFOMSG;
    break;
  
    break;
  case SEQ_WAIT_INFOMSG:
    BATTLE_CHAMPIONSHIP_UpdateMessage( p_wk );
    if( BATTLE_CHAMPIONSHIP_IsFinishMessage( p_wk ) == TRUE )
    {
      BC_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	Wifi���j���[
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiMenu( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_WIFIMSG,
    SEQ_WAIT_WIFIMSG,
    SEQ_START_WIFIMENU,
    SEQ_WAIT_WIFIMENU,

    SEQ_START_CANCELMSG,
    SEQ_WAIT_CANCELMSG,
    SEQ_START_CANCELMENU,
    SEQ_WAIT_CANCELMENU,
  };
  BATTLE_CHAMPIONSHIP_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_WIFIMSG:
    BATTLE_CHAMPIONSHIP_ShowMessage( p_wk, BC_STR_04 );
    *p_seq = SEQ_WAIT_WIFIMSG;
    break;

  case SEQ_WAIT_WIFIMSG:
    BATTLE_CHAMPIONSHIP_UpdateMessage( p_wk );
    if( BATTLE_CHAMPIONSHIP_IsFinishMessage( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_WIFIMENU;
    }
    break;

  case SEQ_START_WIFIMENU:
    BATTLE_CHAMPIONSHIP_OpenFirstMenu( p_wk, BC_MENU_TYPE_WIFI );
    *p_seq = SEQ_WAIT_WIFIMENU;
    break;

  case SEQ_WAIT_WIFIMENU:
    {
      const u32 ret = BATTLE_CHAMPIONSHIP_UpdateFirstMenu( p_wk );
      if( ret != BC_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //���񂩂���
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
          p_wk->nextProcType = BCNP_WIFI_BATTLE;
          break;
        case 1: //�����𕷂�
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_WifiInfo );
          break;
        case 2: //�f�W�^���I��؂��݂�
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_DigitalCard );
          break;
        case 3: //��߂�
          *p_seq  = SEQ_START_CANCELMSG;
          break;
        }
        BATTLE_CHAMPIONSHIP_CloseFirstMenu( p_wk );
      }
    }
    break;

  case SEQ_START_CANCELMSG:
    BATTLE_CHAMPIONSHIP_ShowMessage( p_wk, BC_STR_05 );
    *p_seq = SEQ_WAIT_CANCELMSG;
    break;

  case SEQ_WAIT_CANCELMSG:
    BATTLE_CHAMPIONSHIP_UpdateMessage( p_wk );
    if( BATTLE_CHAMPIONSHIP_IsFinishMessage( p_wk ) == TRUE )
    {
      *p_seq = SEQ_START_CANCELMENU;
    }
    break;

  case SEQ_START_CANCELMENU:
    BATTLE_CHAMPIONSHIP_OpenFirstMenu( p_wk, BC_MENU_TYPE_YESNO );
    *p_seq = SEQ_WAIT_CANCELMENU;
    break;

  case SEQ_WAIT_CANCELMENU:
    {
      const u32 ret = BATTLE_CHAMPIONSHIP_UpdateFirstMenu( p_wk );
      if( ret != BC_LIST_SELECT_NULL )
      {
        switch( ret )
        {
        case 0: //�͂�
          BC_SEQ_SetNext( p_seqwk, SEQFUNC_MainMenu );
          break;
        case 1: //������
          *p_seq  =SEQ_START_WIFIMSG;
          break;
        }
        BATTLE_CHAMPIONSHIP_CloseFirstMenu( p_wk );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	Wifi�������
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiInfo( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_INFOMSG,
    SEQ_WAIT_INFOMSG,
  };

  BATTLE_CHAMPIONSHIP_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_INFOMSG:
    BATTLE_CHAMPIONSHIP_ShowMessage( p_wk, BC_STR_03 );
    *p_seq  = SEQ_WAIT_INFOMSG;
    break;
  
    break;
  case SEQ_WAIT_INFOMSG:
    BATTLE_CHAMPIONSHIP_UpdateMessage( p_wk );
    if( BATTLE_CHAMPIONSHIP_IsFinishMessage( p_wk ) == TRUE )
    {
      BC_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�W�^���I���
 *
 *	@param	BC_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DigitalCard( BC_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_END,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
  };

  BATTLE_CHAMPIONSHIP_WORK *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    (*p_seq)++;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			(*p_seq)++;
		}
		break;

  case SEQ_INIT:
    BATTLE_CHAMPIONSHIP_TermMessage( p_wk );
    BATTLE_CHAMPIONSHIP_ReleaseResource( p_wk );
    BATTLE_CHAMPIONSHIP_TermGraphic( p_wk );

    GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifibattlematch_core), &DigitalCard_ProcData, NULL );
    (*p_seq)++;
    break;

  case SEQ_MAIN:
    (*p_seq)++;
    break;

  case SEQ_END:

    BATTLE_CHAMPIONSHIP_InitGraphic( p_wk );
    BATTLE_CHAMPIONSHIP_LoadResource( p_wk );
    BATTLE_CHAMPIONSHIP_InitMessage( p_wk );

    (*p_seq)++;
    break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
      BC_SEQ_SetNext( p_seqwk, SEQFUNC_WifiMenu );
		}
		break;
  }
}
