//======================================================================
/**
 * @file	mus_shot_data.c
 * @brief	�~���[�W�J���V���b�g�@����ʏ��
 * @author	ariizumi
 * @data	09/09/16
 *
 * ���W���[�����FMUS_SHOT_INFO
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "musical_shot.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_musical_shot.h"

#include "app/app_taskmenu.h"
#include "gamesystem/msgspeed.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/wipe.h"
#include "mus_shot_info.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MUS_INFO_FRAME_MSG ( GFL_BG_FRAME0_S )
#define MUS_INFO_FRAME_INFO ( GFL_BG_FRAME2_S )
#define MUS_INFO_FRAME_BG ( GFL_BG_FRAME3_S )

#define MUS_INFO_PAL_FONT ( 0x0a )
#define MUS_INFO_PAL_WIN ( 0x0b )
#define MUS_INFO_PAL_YESNO ( 0x0c ) //�Q�{

#define MUS_INFO_MSGWIN_X ( 1 )
#define MUS_INFO_MSGWIN_Y ( 1 )
#define MUS_INFO_MSGWIN_WIDTH ( 30 )
#define MUS_INFO_MSGWIN_HEIGHT ( 4 )

#define MUS_INFO_YESNO_X ( 32-13 )
#define MUS_INFO_YESNO_Y ( 6 )
#define MSU_INFO_YESNO_COLOR (PRINTSYS_LSB_Make(0xE,0xF,0))

#define MUS_INFO_CGX_WIN (1)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MSIS_FADE_WAIT,
  //�ۑ��m�F���[�h
  MSIS_MSG_WAIT,
  MSIS_YESNO_WAIT,
  //���邾�����[�h
  MSIS_KEY_WAIT,
  
  MSIS_FINISH,
  
}MUSICAL_SHOT_INFO_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MUS_SHOT_INFO_WORK
{
  HEAPID heapId;
  MUSICAL_SHOT_INFO_STATE state;
  
  BOOL              isChackMode;
  MUSICAL_SHOT_DATA *shotData;
  MUSICAL_SAVE      *musicalSave;
  //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT      *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  STRBUF          *msgStr;
  
  PRINT_QUE *printQue;
  APP_TASKMENU_WORK *yesNoWork;
	APP_TASKMENU_RES	*takmenures;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void MUS_SHOT_INFO_InitGraphic( MUS_SHOT_INFO_WORK *work );
static void MUS_SHOT_INFO_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MUS_SHOT_INFO_InitMessage( MUS_SHOT_INFO_WORK *work );
static void MUS_SHOT_INFO_ExitMessage( MUS_SHOT_INFO_WORK *work );
static void MUS_SHOT_INFO_DispMessage( MUS_SHOT_INFO_WORK *work , const u16 msgId );
static void MUS_SHOT_INFO_HideMessage( MUS_SHOT_INFO_WORK *work );
static void MUS_SHOT_INFO_DispYesNo( MUS_SHOT_INFO_WORK *work );

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
MUS_SHOT_INFO_WORK* MUS_SHOT_INFO_InitSystem( MUSICAL_SHOT_DATA *shotData , MUSICAL_SAVE *musicalSave , const BOOL isChackMode , HEAPID heapId )
{
  MUS_SHOT_INFO_WORK* infoWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MUS_SHOT_INFO_WORK ));
  
  infoWork->heapId = heapId;
  infoWork->isChackMode = isChackMode;
  infoWork->shotData = shotData;
  infoWork->musicalSave = musicalSave;
  infoWork->msgStr = NULL;

  infoWork->state = MSIS_FADE_WAIT;
  
  MUS_SHOT_INFO_InitGraphic( infoWork );
  MUS_SHOT_INFO_InitMessage( infoWork );

	infoWork->takmenures	= APP_TASKMENU_RES_Create( MUS_INFO_FRAME_MSG, MUS_INFO_PAL_YESNO, infoWork->fontHandle, infoWork->printQue, infoWork->heapId );

  return infoWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void MUS_SHOT_INFO_ExitSystem( MUS_SHOT_INFO_WORK *infoWork )
{
	APP_TASKMENU_RES_Delete( infoWork->takmenures );

  MUS_SHOT_INFO_ExitMessage( infoWork );
  GFL_BG_FreeBGControl( MUS_INFO_FRAME_MSG );
  GFL_BG_FreeBGControl( MUS_INFO_FRAME_INFO );
  GFL_BG_FreeBGControl( MUS_INFO_FRAME_BG );
  GFL_HEAP_FreeMemory( infoWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void MUS_SHOT_INFO_UpdateSystem( MUS_SHOT_INFO_WORK *infoWork )
{
  switch( infoWork->state )
  {
  case MSIS_FADE_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( infoWork->isChackMode == TRUE )
      {
        MUS_SHOT_INFO_DispMessage( infoWork , MUSICAL_SHOT_INFO_01 );
        infoWork->state = MSIS_MSG_WAIT;
      }
      else
      {
        infoWork->state = MSIS_KEY_WAIT;
      }
    }
    break;

  case MSIS_MSG_WAIT:
    if( infoWork->printHandle == NULL )
    {
      MUS_SHOT_INFO_DispYesNo( infoWork );
      infoWork->state = MSIS_YESNO_WAIT;
    }
    break;

  case MSIS_YESNO_WAIT:
    APP_TASKMENU_UpdateMenu( infoWork->yesNoWork );
    if( APP_TASKMENU_IsFinish( infoWork->yesNoWork ) == TRUE )
    {
      const u8 retPos = APP_TASKMENU_GetCursorPos( infoWork->yesNoWork );
      APP_TASKMENU_CloseMenu( infoWork->yesNoWork );
      if( retPos == 0 )
      {
        //�Z�[�u����I
        MUSICAL_SHOT_DATA *shotData = MUSICAL_SAVE_GetMusicalShotData( infoWork->musicalSave );
        GFL_STD_MemCopy( infoWork->shotData , shotData , sizeof(MUSICAL_SHOT_DATA) );
      }
      
      infoWork->state = MSIS_FINISH;
    }
    break;
  
  case MSIS_KEY_WAIT:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      infoWork->state = MSIS_FINISH;
    }
    break;
  
  case MSIS_FINISH:
    break;
  }

  GFL_TCBL_Main( infoWork->tcblSys );
  if( infoWork->printHandle != NULL  )
  {
    if( PRINTSYS_PrintStreamGetState( infoWork->printHandle ) == PRINTSTREAM_STATE_DONE )
    {
      PRINTSYS_PrintStreamDelete( infoWork->printHandle );
      infoWork->printHandle = NULL;
    }
  }
  PRINTSYS_QUE_Main( infoWork->printQue );
}

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
const BOOL MUS_SHOT_INFO_IsFinish( MUS_SHOT_INFO_WORK *infoWork )
{
  if( infoWork->state == MSIS_FINISH )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//	�`��n������
//--------------------------------------------------------------
static void MUS_SHOT_INFO_InitGraphic( MUS_SHOT_INFO_WORK *work )
{
  //BG�n
  {
    // BG0 SUB (���b�Z�[�WWin
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x04000,0x2000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    /*
    // BG1 SUB (�J�[�e���E
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000,0x0000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    */
    // BG2 SUB (���
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x4000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x0000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    MUS_SHOT_INFO_SetupBgFunc( &header_sub0 , MUS_INFO_FRAME_MSG  , GFL_BG_MODE_TEXT );
    MUS_SHOT_INFO_SetupBgFunc( &header_sub2 , MUS_INFO_FRAME_INFO  , GFL_BG_MODE_TEXT );
    MUS_SHOT_INFO_SetupBgFunc( &header_sub3 , MUS_INFO_FRAME_BG , GFL_BG_MODE_TEXT );
  }
  //BG�ǂݍ���
  {
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MUSICAL_SHOT , work->heapId );
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_musical_shot_info_bg_NCLR , 
                      PALTYPE_SUB_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_musical_shot_info_bg_NCGR ,
                      MUS_INFO_FRAME_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_musical_shot_info_bg_NSCR , 
                      MUS_INFO_FRAME_BG ,  0 , 0, FALSE , work->heapId );
    GFL_BG_LoadScreenReq(MUS_INFO_FRAME_BG);
    GFL_ARC_CloseDataHandle(arcHandle);
  }
}
//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n������
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_InitMessage( MUS_SHOT_INFO_WORK *work )
{
  //���b�Z�[�W�p����
  work->msgWin = GFL_BMPWIN_Create( MUS_INFO_FRAME_MSG , MUS_INFO_MSGWIN_X , MUS_INFO_MSGWIN_Y ,
                  MUS_INFO_MSGWIN_WIDTH , MUS_INFO_MSGWIN_HEIGHT , MUS_INFO_PAL_FONT ,
                  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
  GFL_BMPWIN_MakeScreen( work->msgWin );
  GFL_BG_LoadScreenReq(MUS_INFO_FRAME_MSG);
  
  
  //�t�H���g�ǂݍ���
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //���b�Z�[�W
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_shot_dat , work->heapId );

  BmpWinFrame_GraphicSet( MUS_INFO_FRAME_MSG , MUS_INFO_CGX_WIN , MUS_INFO_PAL_WIN , 0 , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , MUS_INFO_PAL_FONT*0x20, 16*2, work->heapId );
  GFL_FONTSYS_SetDefaultColor();
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
  work->msgStr = NULL;
  
  //YesNo�p
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n�J��
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_ExitMessage( MUS_SHOT_INFO_WORK *work )
{
  PRINTSYS_QUE_Delete( work->printQue );
  if( work->printHandle != NULL )
  {
    PRINTSYS_PrintStreamDelete( work->printHandle );
  }
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
  }
  GFL_MSG_Delete( work->msgHandle );
  GFL_BMPWIN_Delete( work->msgWin );
  GFL_FONT_Delete( work->fontHandle );
  GFL_TCBL_Exit( work->tcblSys );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�\��
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_DispMessage( MUS_SHOT_INFO_WORK *work , const u16 msgId )
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
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, work->msgStr ,work->fontHandle ,
                        MSGSPEED_GetWait() , work->tcblSys , 2 , work->heapId , 0 );
  }
  BmpWinFrame_Write( work->msgWin , WINDOW_TRANS_ON_V , MUS_INFO_CGX_WIN , MUS_INFO_PAL_WIN );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�\��
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_HideMessage( MUS_SHOT_INFO_WORK *work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  BmpWinFrame_Clear( work->msgWin , WINDOW_TRANS_ON_V );
}

//--------------------------------------------------------------------------
//  �I�����\��
//--------------------------------------------------------------------------
static void MUS_SHOT_INFO_DispYesNo( MUS_SHOT_INFO_WORK *work )
{
  APP_TASKMENU_ITEMWORK itemWork[2];
  APP_TASKMENU_INITWORK initWork;
  
  itemWork[0].str = GFL_MSG_CreateString( work->msgHandle , MUSICAL_SHOT_INFO_02 );
  itemWork[1].str = GFL_MSG_CreateString( work->msgHandle , MUSICAL_SHOT_INFO_03 );
  itemWork[0].msgColor = MSU_INFO_YESNO_COLOR;
  itemWork[1].msgColor = MSU_INFO_YESNO_COLOR;
  itemWork[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  itemWork[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  initWork.heapId = work->heapId;
  initWork.itemNum = 2;
  initWork.itemWork = itemWork;
//  initWork.bgFrame = MUS_INFO_FRAME_MSG;
 // initWork.palNo = MUS_INFO_PAL_YESNO;
  initWork.posType = ATPT_LEFT_UP;
  initWork.charPosX = MUS_INFO_YESNO_X;
  initWork.charPosY = MUS_INFO_YESNO_Y;
  //initWork.msgHandle = work->msgHandle;
 // initWork.fontHandle = work->fontHandle;
//  initWork.printQue = work->printQue;

  work->yesNoWork = APP_TASKMENU_OpenMenu( &initWork, work->takmenures );
  
  GFL_STR_DeleteBuffer( itemWork[0].str );
  GFL_STR_DeleteBuffer( itemWork[1].str );
}
