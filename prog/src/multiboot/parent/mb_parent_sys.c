//======================================================================
/**
 * @file	mb_parent_sys.c
 * @brief	�}���`�u�[�g�E�e�@���C��
 * @author	ariizumi
 * @data	09/11/13
 *
 * ���W���[�����FMP_PARENT
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "app/app_taskmenu.h"
#include "gamesystem/msgspeed.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "font/font.naix"
#include "mb_parent.naix"
#include "message.naix"
#include "msg/msg_multiboot_parent.h"

#include "multiboot/mb_parent_sys.h"
#include "multiboot/comm/mb_comm_sys.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_PARENT_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_PARENT_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_PARENT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define MB_PARENT_PLT_MAIN_TASKMENU (12)  //2�{
#define MB_PARENT_PLT_MAIN_MSGWIN (14)
#define MB_PARENT_PLT_MAIN_FONT   (15)

#define MB_PARENT_MSGWIN_CGX    (1)
#define MB_PARENT_MSGWIN_TOP    (8)
#define MB_PARENT_MSGWIN_LEFT   (3)
#define MB_PARENT_MSGWIN_HEIGHT (8)
#define MB_PARENT_MSGWIN_WIDTH  (26)

#define MB_PARENT_YESNO_X ( 32-APP_TASKMENU_PLATE_WIDTH_YN_WIN )
#define MB_PARENT_YESNO_Y ( 24-APP_TASKMENU_PLATE_HEIGHT*2 )
#define MB_PARENT_YESNO_COLOR (APP_TASKMENU_ITEM_MSGCOLOR)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MPS_FADEIN,
  MPS_WAIT_FADEIN,
  MPS_FADEOUT,
  MPS_WAIT_FADEOUT,
  
  MPS_SEND_IMAGE_INIT,
  MPS_SEND_IMAGE_MAIN,
  MPS_SEND_IMAGE_TERM,
}MB_PARENT_STATE;

typedef enum
{
  MPSS_SEND_IMAGE_INIT,
  MPSS_SEND_IMAGE_SEARCH_CHILD,
  MPSS_SEND_IMAGE_SEND_IMAGE,
  MPSS_SEND_IMAGE_WAIT_BOOT,
//-------------------------------

  
}MB_PARENT_SUB_STATE;


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  MB_PARENT_INIT_WORK *initWork;
  MB_COMM_WORK *commWork;
  
  MB_PARENT_STATE state;
  u8              subState;
  
  //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT        *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  STRBUF          *msgStr;
  WORDSET         *wordSet;
  
  PRINT_QUE *printQue;
  APP_TASKMENU_WORK *yesNoWork;
	APP_TASKMENU_RES	*takmenures;
}MB_PARENT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_128_B ,           // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,        // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,         // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


static void MB_PARENT_Init( MB_PARENT_WORK *work );
static void MB_PARENT_Term( MB_PARENT_WORK *work );
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work );

static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work );

static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work );
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work );

static void MB_PARENT_MessageInit( MB_PARENT_WORK *work );
static void MB_PARENT_MessageTerm( MB_PARENT_WORK *work );
static void MB_PARENT_MessageMain( MB_PARENT_WORK *work );
static void MB_PARENT_MessageDisp( MB_PARENT_WORK *work , const u16 msgId );
static void MB_PARENT_MessageHide( MB_PARENT_WORK *work );
static void MB_PARENT_MessageCreateWordset( MB_PARENT_WORK *work );
static void MB_PARENT_MessageDeleteWordset( MB_PARENT_WORK *work );
static void MB_PARENT_MessageWordsetName( MB_PARENT_WORK *work , const u32 bufId );
static void MB_PARENT_DispYesNo( MB_PARENT_WORK *work );

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static void MB_PARENT_Init( MB_PARENT_WORK *work )
{
  work->state = MPS_FADEIN;
  
  MB_PARENT_InitGraphic( work );
  MB_PARENT_LoadResource( work );
  MB_PARENT_MessageInit( work );
  
  work->commWork = MB_COMM_CreateSystem( work->heapId );
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_PARENT_Term( MB_PARENT_WORK *work )
{
  MB_COMM_DeleteSystem( work->commWork );

  MB_PARENT_MessageTerm( work );
  MB_PARENT_TermGraphic( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work )
{

  switch( work->state )
  {
  case MPS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    work->state = MPS_WAIT_FADEIN;
    break;
    
  case MPS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->state = MPS_SEND_IMAGE_INIT;
    }
    break;
    
  case MPS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MPS_WAIT_FADEIN;
    break;
  case MPS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  //-----------------------------------------------
  //ROM�̑��M����
  case MPS_SEND_IMAGE_INIT:
    MP_PARENT_SendImage_Init( work );
    work->state = MPS_SEND_IMAGE_MAIN;
    break;
    
  case MPS_SEND_IMAGE_MAIN:
    {
      const BOOL ret = MP_PARENT_SendImage_Main( work );
      if( ret == TRUE )
      {
        work->state = MPS_SEND_IMAGE_TERM;
      }
    }
    break;
    
  case MPS_SEND_IMAGE_TERM:
    MP_PARENT_SendImage_Term( work );
    break;
    
  }

  MB_PARENT_MessageMain( work );
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START &&
      GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    return TRUE;
  }
  return FALSE;
}


//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work )
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
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    // BG1 MAIN (���b�Z�[�W
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (�L����
    //static const GFL_BG_BGCNT_HEADER header_main2 = {
    //  0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
    //  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    //  GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x0C000,
    //  GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    //};
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_PARENT_SetupBgFunc( &header_main1 , MB_PARENT_FRAME_MSG , GFL_BG_MODE_TEXT );
    //MB_PARENT_SetupBgFunc( &header_main2 , LTVT_FRAME_CHARA , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main3 , MB_PARENT_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub3  , MB_PARENT_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  
}

static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work )
{
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_PARENT , work->heapId );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_sub_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_sub_NCGR ,
                    MB_PARENT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_sub_NSCR , 
                    MB_PARENT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  
  //����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_main_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_main_NCGR ,
                    MB_PARENT_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_main_NSCR , 
                    MB_PARENT_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  
  GFL_ARC_CloseDataHandle( arcHandle );
}


#pragma mark [>SendImage func
//--------------------------------------------------------------
//  ROM���M���� ������
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work )
{
  work->subState = MPSS_SEND_IMAGE_INIT;
}

//--------------------------------------------------------------
//  ROM���M���� �J��
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work )
{
}

//--------------------------------------------------------------
//  ROM���M���� �X�V
//--------------------------------------------------------------
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SEND_IMAGE_INIT:
    MB_PARENT_MessageCreateWordset( work );
    MB_PARENT_MessageWordsetName( work , 0 );
    MB_PARENT_MessageDisp( work , MSG_MB_PAERNT_01 );
    MB_PARENT_MessageDeleteWordset( work );

    work->subState = MPSS_SEND_IMAGE_SEARCH_CHILD;
    break;
  case MPSS_SEND_IMAGE_SEARCH_CHILD:
    break;
  case MPSS_SEND_IMAGE_SEND_IMAGE:
    break;
  case MPSS_SEND_IMAGE_WAIT_BOOT:
    break;
  }
  
  return FALSE;
}


#pragma mark [>Message func
//--------------------------------------------------------------
//  ���b�Z�[�W�n ������
//--------------------------------------------------------------
static void MB_PARENT_MessageInit( MB_PARENT_WORK *work )
{
  //���b�Z�[�W�p����
  work->msgWin = GFL_BMPWIN_Create( MB_PARENT_FRAME_MSG , 
                                    MB_PARENT_MSGWIN_LEFT , 
                                    MB_PARENT_MSGWIN_TOP ,
                                    MB_PARENT_MSGWIN_WIDTH , 
                                    MB_PARENT_MSGWIN_HEIGHT , 
                                    MB_PARENT_PLT_MAIN_FONT ,
                                    GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
  GFL_BMPWIN_MakeScreen( work->msgWin );
  GFL_BG_LoadScreenReq(MB_PARENT_FRAME_MSG);
  
  //�t�H���g�ǂݍ���
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //���b�Z�[�W
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_multiboot_parent_dat , work->heapId );

  BmpWinFrame_GraphicSet( MB_PARENT_FRAME_MSG , MB_PARENT_MSGWIN_CGX , MB_PARENT_PLT_MAIN_MSGWIN , 0 , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , MB_PARENT_PLT_MAIN_FONT*0x20, 16*2, work->heapId );
  GFL_FONTSYS_SetDefaultColor();
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
  work->msgStr = NULL;
  work->wordSet = NULL;
  
  //YesNo�p
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
	work->takmenures	= APP_TASKMENU_RES_Create( MB_PARENT_FRAME_MSG, MB_PARENT_PLT_MAIN_TASKMENU, work->fontHandle, work->printQue, work->heapId );
}

//--------------------------------------------------------------
//  ���b�Z�[�W�n �J��
//--------------------------------------------------------------
static void MB_PARENT_MessageTerm( MB_PARENT_WORK *work )
{
  APP_TASKMENU_RES_Delete( work->takmenures );
  PRINTSYS_QUE_Delete( work->printQue );
  if( work->printHandle != NULL )
  {
    PRINTSYS_PrintStreamDelete( work->printHandle );
  }
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
  }
  if( work->wordSet != NULL )
  {
    MB_PARENT_MessageDeleteWordset( work );
  }
  GFL_MSG_Delete( work->msgHandle );
  GFL_BMPWIN_Delete( work->msgWin );
  GFL_FONT_Delete( work->fontHandle );
  GFL_TCBL_Exit( work->tcblSys );
}

//--------------------------------------------------------------
//  ���b�Z�[�W�n �X�V
//--------------------------------------------------------------
static void MB_PARENT_MessageMain( MB_PARENT_WORK *work )
{
  GFL_TCBL_Main( work->tcblSys );
  if( work->printHandle != NULL  )
  {
    if( PRINTSYS_PrintStreamGetState( work->printHandle ) == PRINTSTREAM_STATE_DONE )
    {
      PRINTSYS_PrintStreamDelete( work->printHandle );
      work->printHandle = NULL;
    }
  }
  PRINTSYS_QUE_Main( work->printQue );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�\��
//--------------------------------------------------------------------------
static void MB_PARENT_MessageDisp( MB_PARENT_WORK *work , const u16 msgId )
{
  if( work->printHandle != NULL )
  {
    OS_TPrintf( NULL , "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }

  {
    if( work->msgStr != NULL )
    {
      GFL_STR_DeleteBuffer( work->msgStr );
      work->msgStr = NULL;
    }
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0xf );
    work->msgStr = GFL_MSG_CreateString( work->msgHandle , msgId );
    
    if( work->wordSet != NULL )
    {
      STRBUF *workStr = GFL_STR_CreateBuffer( 128 , work->heapId );
      WORDSET_ExpandStr( work->wordSet , workStr , work->msgStr );
      GFL_STR_DeleteBuffer( work->msgStr );
      work->msgStr = workStr;
    }
    
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, work->msgStr ,work->fontHandle ,
                        MSGSPEED_GetWait() , work->tcblSys , 2 , work->heapId , 0 );
  }
  BmpWinFrame_Write( work->msgWin , WINDOW_TRANS_ON_V , MB_PARENT_MSGWIN_CGX , MB_PARENT_PLT_MAIN_MSGWIN );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W����
//--------------------------------------------------------------------------
static void MB_PARENT_MessageHide( MB_PARENT_WORK *work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  BmpWinFrame_Clear( work->msgWin , WINDOW_TRANS_ON_V );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET �쐬
//--------------------------------------------------------------------------
static void MB_PARENT_MessageCreateWordset( MB_PARENT_WORK *work )
{
  if( work->wordSet == NULL )
  {
    work->wordSet = WORDSET_Create( work->heapId );
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET �폜
//--------------------------------------------------------------------------
static void MB_PARENT_MessageDeleteWordset( MB_PARENT_WORK *work )
{
  if( work->wordSet != NULL )
  {
    WORDSET_Delete( work->wordSet );
    work->wordSet = NULL;
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET ���O�ݒ�
//--------------------------------------------------------------------------
static void MB_PARENT_MessageWordsetName( MB_PARENT_WORK *work , const u32 bufId )
{
  MYSTATUS *myStatus = GAMEDATA_GetMyStatus(work->initWork->gameData);

  WORDSET_RegisterPlayerName( work->wordSet , bufId , myStatus );
}

//--------------------------------------------------------------------------
//  �I�����\��
//--------------------------------------------------------------------------
static void MB_PARENT_DispYesNo( MB_PARENT_WORK *work )
{
  APP_TASKMENU_ITEMWORK itemWork[2];
  APP_TASKMENU_INITWORK initWork;
  
  itemWork[0].str = GFL_MSG_CreateString( work->msgHandle , MSG_MB_PAERNT_SYS_01 );
  itemWork[1].str = GFL_MSG_CreateString( work->msgHandle , MSG_MB_PAERNT_SYS_02 );
  itemWork[0].msgColor = MB_PARENT_YESNO_COLOR;
  itemWork[1].msgColor = MB_PARENT_YESNO_COLOR;
  itemWork[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  itemWork[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  initWork.heapId = work->heapId;
  initWork.itemNum = 2;
  initWork.itemWork = itemWork;
//  initWork.bgFrame = MB_PARENT_FRAME_MSG;
 // initWork.palNo = MUS_INFO_PAL_YESNO;
  initWork.posType = ATPT_LEFT_UP;
  initWork.charPosX = MB_PARENT_YESNO_X;
  initWork.charPosY = MB_PARENT_YESNO_Y;
  //initWork.msgHandle = work->msgHandle;
 // initWork.fontHandle = work->fontHandle;
//  initWork.printQue = work->printQue;
  initWork.w = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  initWork.h = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  work->yesNoWork = APP_TASKMENU_OpenMenu( &initWork, work->takmenures );
  
  GFL_STR_DeleteBuffer( itemWork[0].str );
  GFL_STR_DeleteBuffer( itemWork[1].str );
}

#pragma mark [>proc func
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBoot_ProcData =
{
  MB_PARENT_ProcInit,
  MB_PARENT_ProcMain,
  MB_PARENT_ProcTerm
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_INIT_WORK *initWork;
  MB_PARENT_WORK *work;
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x150000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_PARENT_WORK), HEAPID_MULTIBOOT );
  if( pwk == NULL )
  {
    initWork = GFL_HEAP_AllocMemory( HEAPID_MULTIBOOT , sizeof( MB_PARENT_INIT_WORK ));
    initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
  }
  else
  {
    initWork = pwk;
  }
  work->heapId = HEAPID_MULTIBOOT;
  work->initWork = initWork;
  
  MB_PARENT_Init( work );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  
  MB_PARENT_Term( work );

  if( pwk == NULL )
  {
    GAMEDATA_Delete( work->initWork->gameData );
    GFL_HEAP_FreeMemory( work->initWork );
  }
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  const BOOL ret = MB_PARENT_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

