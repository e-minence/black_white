//======================================================================
/**
 * @file	comm_tvt_sys
 * @brief	�ʐMTVT�V�X�e��
 * @author	ariizumi
 * @data	09/12/16
 *
 * ���W���[�����FCOMM_TVT
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/net_err.h"
#include "system/ds_system.h"
#include "system/time_icon.h"
#include "sound/pm_sndsys.h"
#include "app/app_menu_common.h"
#include "savedata/etc_save.h"
#include "field/field_sound.h"

#include "arc_def.h"
#include "comm_tvt.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_comm_tvt.h"

#include "net_app/comm_tvt_sys.h"
#include "ctvt_camera.h"
#include "ctvt_comm.h"
#include "ctvt_talk.h"
#include "ctvt_draw.h"
#include "ctvt_call.h"
#include "comm_tvt_local_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define COMM_TVT_PAL_ANM_NUM (10)   //�p���b�g10�{�g��
#define COMM_TVT_PAL_ANM_SPD (0x200)

#define COMM_TVT_NAME_WIDTH (12)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _COMM_TVT_WORK
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;

  COMM_TVT_MODE mode;
  COMM_TVT_MODE nextMode;
  BOOL isUpperFade;
  BOOL isCommError;

  u8 connectNum;
  u8 selfIdx;
  BOOL isDouble;
  BOOL isSusspend;
  BOOL isPause;
  BOOL isSusspendDraw;  //���G�`�����O��M���̑ҋ@
  BOOL isReqFinish; //�e����̏I���ʒm
  BOOL isReqDispWarn; //�y�A�R���x���\�����N�G�X�g
  BOOL isUseSelfBUffer; //���g�̃o�b�t�@�̎g�p���b�N(�J�����E�ʐM)
  u16 palAnmCnt;
  u16 palAnmIdx;
  u16 palAnmData[COMM_TVT_PAL_ANM_NUM][16];
  
  ARCHANDLE *arcHandle;
  //�Z���n
  u32         cellResIdx[CTOR_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkRec;

  //���ʖ��O
  u8         nameDrawBit;   //netId�ɑΉ�
  u8         nameWinUpdateBit; //bmpWin�̔z��ɑΉ�
  GFL_BMPWIN *nameWin[CTVT_MEMBER_NUM];

  //���b�Z�[�W�n
  GFL_FONT        *fontHandle;
  GFL_MSGDATA     *msgHandle;
  PRINT_QUE *printQue;

  APP_TASKMENU_RES *taskMenuRes;

  GFL_TCBLSYS   *tcblSys;
  TIMEICON_WORK *timeIcon;

  //�e���[�N
  CTVT_CAMERA_WORK *camWork;
  CTVT_COMM_WORK   *commWork;
  
  CTVT_TALK_WORK   *talkWork;
  CTVT_DRAW_WORK   *drawWork;
  CTVT_CALL_WORK   *callWork;

  DRAW_SYS_WORK   *drawSys;

  COMM_TVT_INIT_WORK *initWork;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void COMM_TVT_Init( COMM_TVT_WORK *work );
static void COMM_TVT_Term( COMM_TVT_WORK *work );
static const BOOL COMM_TVT_Main( COMM_TVT_WORK *work );
static void COMM_TVT_VBlankFunc(GFL_TCB *tcb, void *wk );

static void COMM_TVT_InitGraphic( COMM_TVT_WORK *work );
static void COMM_TVT_TermGraphic( COMM_TVT_WORK *work );
static void COMM_TVT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );

static void COMM_TVT_LoadResource( COMM_TVT_WORK *work );
static void COMM_TVT_ReleaseResource( COMM_TVT_WORK *work );
static void COMM_TVT_InitMessage( COMM_TVT_WORK *work );
static void COMM_TVT_TermMessage( COMM_TVT_WORK *work );

static void COMM_TVT_ChangeMode( COMM_TVT_WORK *work );
static void COMM_TVT_UpdateUpperName( COMM_TVT_WORK *work );
static void COMM_TVT_UpdateUpperNameFunc( COMM_TVT_WORK *work , MYSTATUS *myStatus , GFL_BMPWIN *bmpWin );

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_256_AB,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_16_F,            // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,        // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_NONE,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,         // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
static void COMM_TVT_Init( COMM_TVT_WORK *work )
{
  u8 i;
  
  work->connectNum = 1;
  work->selfIdx = 0;
  work->isDouble = FALSE;


  COMM_TVT_InitGraphic( work );
  COMM_TVT_LoadResource( work );
  COMM_TVT_InitMessage( work );
  
  work->taskMenuRes = APP_TASKMENU_RES_Create( CTVT_FRAME_SUB_MSG , 
                                               CTVT_PAL_BG_SUB_TASKMENU ,
                                               work->fontHandle ,
                                               work->printQue , 
                                               work->heapId );
  
  work->camWork = CTVT_CAMERA_Init( work , work->heapId );
  work->commWork = CTVT_COMM_InitSystem( work , work->heapId );
  
  work->talkWork = CTVT_TALK_InitSystem( work , work->heapId );
  work->drawWork = CTVT_DRAW_InitSystem( work , work->heapId );
  work->callWork = CTVT_CALL_InitSystem( work , work->heapId );
  
  {
    DRAW_SYS_INIT_WORK initWork;
    initWork.heapId = work->heapId;
    initWork.frame = CTVT_FRAME_MAIN_DRAW;
    initWork.bufferNum = CTVT_DRAW_BUFFER_NUM;
    initWork.areaLeft = 0;
    initWork.areaRight = 256;
    initWork.areaTop = 0;
    initWork.areaBottom = 192;

    
    work->drawSys = DRAW_SYS_CreateSystem( &initWork );
  }
  
  work->vBlankTcb = GFUser_VIntr_CreateTCB( COMM_TVT_VBlankFunc , work , 8 );

  work->mode = CTM_NONE;
  work->palAnmCnt = 0;
  work->palAnmIdx = 0;

  work->isUpperFade = TRUE;
  work->isSusspend = FALSE;
  work->isPause = FALSE;
  work->isSusspendDraw = FALSE;
  work->isReqFinish = FALSE;
  work->isReqDispWarn = FALSE;
  work->isUseSelfBUffer = FALSE;

  if( DS_SYSTEM_IsRestrictPhotoExchange() == TRUE )
  {
    work->isReqDispWarn = TRUE;
  }

  GFL_NET_WirelessIconEasy_HoldLCD( FALSE , work->heapId );
  GFL_NET_ReloadIcon();
  
  switch( work->initWork->mode )
  {
  case CTM_TEST:   //�e�X�g�p(�e
    CTVT_COMM_SetMode( work , work->commWork , CCIM_PARENT );
    work->nextMode = CTM_TALK;
    break;
  case CTM_PARENT: //�e�N��
    CTVT_COMM_SetMode( work , work->commWork , CCIM_SCAN );
    work->nextMode = CTM_CALL;
    break;
  case CTM_CHILD:  //�q�N��
    CTVT_COMM_SetMode( work , work->commWork , CCIM_CHILD );
    CTVT_COMM_SetMacAddress( work , work->commWork , work->initWork->macAddress );
    work->nextMode = CTM_CALL;
    {
      FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( work->initWork->gameData );
      FSND_StopTVTRingTone( fsnd );
    }
    break;
  case CTM_WIFI:   //Wifi�N��
    CTVT_COMM_SetMode( work , work->commWork , CCIM_CONNECTED );
    work->nextMode = CTM_CALL;
    break;
  }

  work->nameWin[0] = GFL_BMPWIN_Create( CTVT_FRAME_MAIN_MSG , 
                                         (16-COMM_TVT_NAME_WIDTH)/2 , 10 , 
                                         COMM_TVT_NAME_WIDTH , 2 ,
                                        CTVT_PAL_BG_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  work->nameWin[1] = GFL_BMPWIN_Create( CTVT_FRAME_MAIN_MSG , 
                                        16+(16-COMM_TVT_NAME_WIDTH)/2 , 10 , 
                                        COMM_TVT_NAME_WIDTH , 2 ,
                                        CTVT_PAL_BG_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  work->nameWin[2] = GFL_BMPWIN_Create( CTVT_FRAME_MAIN_MSG , 
                                         (16-COMM_TVT_NAME_WIDTH)/2 , 22 , 
                                         COMM_TVT_NAME_WIDTH , 2 ,
                                        CTVT_PAL_BG_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  work->nameWin[3] = GFL_BMPWIN_Create( CTVT_FRAME_MAIN_MSG , 
                                        16+(16-COMM_TVT_NAME_WIDTH)/2 , 22 , 
                                        COMM_TVT_NAME_WIDTH , 2 ,
                                        CTVT_PAL_BG_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  for( i=0;i<CTVT_MEMBER_NUM;i++ )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->nameWin[i] ) , 0x0 );
  }
  work->nameDrawBit = 0;
  work->nameWinUpdateBit = 0;
  
  //PMSND_FadeOutBGM( PMSND_FADE_FAST );
  PMSND_ChangeBGMVolume( 0xFFFF , 0 );

}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
static void COMM_TVT_Term( COMM_TVT_WORK *work )
{
  u8 i;
  //PMSND_ChangeBGMVolume( 0xFFFF , 127 );

  for( i=0;i<CTVT_MEMBER_NUM;i++ )
  {
    GFL_BMPWIN_Delete( work->nameWin[i] );
  }

  GFL_TCB_DeleteTask( work->vBlankTcb );
  
  DRAW_SYS_DeleteSystem( work->drawSys );
  CTVT_CALL_TermSystem( work , work->callWork );
  CTVT_DRAW_TermSystem( work , work->drawWork );
  CTVT_TALK_TermSystem( work , work->talkWork );

  CTVT_COMM_TermSystem( work , work->commWork );
  CTVT_CAMERA_Term( work , work->camWork );

  APP_TASKMENU_RES_Delete( work->taskMenuRes );

  COMM_TVT_TermMessage( work );
  COMM_TVT_ReleaseResource( work );
  COMM_TVT_TermGraphic( work );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
static const BOOL COMM_TVT_Main( COMM_TVT_WORK *work )
{
  CTVT_COMM_Main( work , work->commWork );

  switch( work->mode )
  {
  case CTM_TALK: //��b
    work->nextMode = CTVT_TALK_Main( work , work->talkWork );
    break;
  case CTM_CALL: //�Ăяo��
    work->nextMode = CTVT_CALL_Main( work , work->callWork );
    break;
  case CTM_DRAW: //������
    work->nextMode = CTVT_DRAW_Main( work , work->drawWork );
    break;
  case CTM_END:
    if( CTVT_COMM_IsExit(work,work->commWork) == TRUE )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
    break;
  case CTM_ERROR: //�I��
    if( CTVT_COMM_IsExit(work,work->commWork) == TRUE )
    {
      if( WIPE_SYS_EndCheck() == TRUE )
      {
        return TRUE;
      }
    }
    break;
  }
  
  if( work->isCommError == TRUE )
  {
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->nextMode = CTM_ERROR;
    }
  }
  if( work->mode != work->nextMode )
  {
    COMM_TVT_ChangeMode( work );
  }
  
  CTVT_CAMERA_Main( work , work->camWork );
  
  DRAW_SYS_UpdateSystem( work->drawSys );

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  PRINTSYS_QUE_Main( work->printQue );
  
  COMM_TVT_UpdateUpperName( work );

  //�t�F�[�h
  if( work->palAnmCnt + COMM_TVT_PAL_ANM_SPD < 0x10000 )
  {
    work->palAnmCnt += COMM_TVT_PAL_ANM_SPD;
  }
  else
  {
    work->palAnmCnt += COMM_TVT_PAL_ANM_SPD-0x10000;
  }
  {
    const fx32 sin = (FX_SinIdx( work->palAnmCnt )+FX32_ONE)/2;
    u8 anmIdx = FX_FX32_TO_F32(COMM_TVT_PAL_ANM_NUM*sin);
    if( anmIdx >= COMM_TVT_PAL_ANM_NUM )
    {
      anmIdx = COMM_TVT_PAL_ANM_NUM-1;
    }
    if( anmIdx != work->palAnmIdx )
    {
      work->palAnmIdx = anmIdx;
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                          (CTVT_PAL_BG_SUB_BG+2) * 32,
                                          &work->palAnmData[anmIdx] , 32 );
    }
  }

  GFL_TCBL_Main( work->tcblSys );

  //���ʃm�C�Y
  GFL_BG_SetScroll( CTVT_FRAME_MAIN_BG , GFL_BG_SCROLL_X_SET , GFUser_GetPublicRand0(256) );
  GFL_BG_SetScroll( CTVT_FRAME_MAIN_BG , GFL_BG_SCROLL_Y_SET , GFUser_GetPublicRand0(256) );

  return FALSE;
}
//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void COMM_TVT_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  COMM_TVT_WORK *work = wk;

  //���ʂ̍X�V
  CTVT_CAMERA_VBlank( work , work->camWork );
  //OBJ�̍X�V
  GFL_CLACT_SYS_VBlankFunc();
  //���G�`���̍X�V
  DRAW_SYS_UpdateSystemVBlank( work->drawSys );
  
  CTVT_TALK_Main_VBlank( work , work->talkWork );
}

//--------------------------------------------------------------
//	�O���t�B�b�N������
//--------------------------------------------------------------
static void COMM_TVT_InitGraphic( COMM_TVT_WORK *work )
{
  GFL_DISP_SetDispOn();
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  G2_BlendNone();
  G2S_BlendNone();
  
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  GFL_DISP_SetBank( &vramBank );
  //BG�n�̏�����
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BGMODE_0, GX_BG0_AS_2D,
    };

    // BG0 MSG (����
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x0800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 BAR (�o�[
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x0800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x05000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (���̑�(��ʕ�BG
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x0800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    //Main
    // BG0 MSG (����
    static const GFL_BG_BGCNT_HEADER header_main0 = {
      0, 0, 0x0800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x07000,
      GX_BG_EXTPLTT_23, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 BG (BG�m�C�Y
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x0800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );
    
    COMM_TVT_SetupBgFunc( &header_main0 , CTVT_FRAME_MAIN_MSG  , GFL_BG_MODE_TEXT );
    COMM_TVT_SetupBgFunc( &header_main1 , CTVT_FRAME_MAIN_BG  , GFL_BG_MODE_TEXT );

    COMM_TVT_SetupBgFunc( &header_sub0 , CTVT_FRAME_SUB_MSG  , GFL_BG_MODE_TEXT );
    COMM_TVT_SetupBgFunc( &header_sub1 , CTVT_FRAME_SUB_BAR  , GFL_BG_MODE_TEXT );
    COMM_TVT_SetupBgFunc( &header_sub2 , CTVT_FRAME_SUB_MISC , GFL_BG_MODE_TEXT );
    COMM_TVT_SetupBgFunc( &header_sub3 , CTVT_FRAME_SUB_BG   , GFL_BG_MODE_TEXT );
    
    //�_�C���N�gBMP��GFL���Ή����ĂȂ��̂łׂ�����

    G2_SetBG2ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x10000);
    G2_SetBG3ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU, GX_BG_BMPSCRBASE_0x28000);

    GFL_STD_MemClear( G2_GetBG2ScrPtr() , 256*192*sizeof(u16) );
    GFL_STD_MemClear( G2_GetBG3ScrPtr() , 256*192*sizeof(u16) );

    G2_SetBG2Priority(1);
    G2_SetBG3Priority(2);
    GFL_BG_SetVisible( CTVT_FRAME_MAIN_DRAW, VISIBLE_ON );
    GFL_BG_SetVisible( CTVT_FRAME_MAIN_CAMERA, VISIBLE_ON );
    
  }
  //OBJ�n�̏�����
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );

    work->cellUnit = GFL_CLACT_UNIT_Create( 64 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
  }
  
  //Window(�{�p�\�����X�V�p
  {
    GX_SetVisibleWnd( GX_WNDMASK_W0|GX_WNDMASK_W1 );
    G2_SetWnd0InsidePlane( 0x1F , TRUE );
    G2_SetWnd1InsidePlane( 0x1F , TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_OBJ , TRUE );
    G2_SetWnd0Position( 0,0,255,192 );
    G2_SetWnd1Position( 128,0,0,192 );

  }
}

//--------------------------------------------------------------
//	�O���t�B�b�N�J��
//--------------------------------------------------------------
static void COMM_TVT_TermGraphic( COMM_TVT_WORK *work )
{
  GX_SetVisibleWnd( GX_WNDMASK_NONE );

  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();

  GFL_BG_FreeBGControl( CTVT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( CTVT_FRAME_SUB_MISC );
  GFL_BG_FreeBGControl( CTVT_FRAME_SUB_BAR );
  GFL_BG_FreeBGControl( CTVT_FRAME_SUB_MSG );
  GFL_BG_FreeBGControl( CTVT_FRAME_MAIN_BG );
  GFL_BG_FreeBGControl( CTVT_FRAME_MAIN_MSG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void COMM_TVT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------------------
static void COMM_TVT_LoadResource( COMM_TVT_WORK *work )
{
  work->arcHandle = GFL_ARC_OpenDataHandle( ARCID_COMM_TVT_GRA , work->heapId );
  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( work->arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCLR , 
                    PALTYPE_SUB_BG , CTVT_PAL_BG_SUB_BG*32 , 32*6 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCGR ,
                    CTVT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_comm_tvt_tv_t_common_bg_NSCR , 
                    CTVT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_BG );

  //����
  GFL_ARCHDL_UTIL_TransVramPalette( work->arcHandle , NARC_comm_tvt_noise_bg_NCLR , 
                    PALTYPE_MAIN_BG , CTVT_PAL_BG_MAIN_BG*32 , 32*1 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_comm_tvt_noise_bg_NCGR ,
                    CTVT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_comm_tvt_noise_bg_NSCR , 
                    CTVT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );
  GFL_BG_LoadScreenReq( CTVT_FRAME_MAIN_BG );

  //����� BGMisc(�L�����͋��ʂȂ̐�ǂ�
  //�e�X�œǂނ悤�ɕύX
//  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCGR ,
//                    CTVT_FRAME_SUB_MISC , 0 , 0, FALSE , work->heapId );
//  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCGR ,
//                    CTVT_FRAME_SUB_MSG , 0 , 0, FALSE , work->heapId );

  //OBJ
  work->cellResIdx[CTOR_COMMON_M_PLT] = GFL_CLGRP_PLTT_RegisterComp( work->arcHandle , 
        NARC_comm_tvt_tv_t_obj_NCLR , CLSYS_DRAW_MAIN , 
        CTVT_PAL_OBJ_MAIN_COMMON*32 , work->heapId  );
  work->cellResIdx[CTOR_COMMON_M_NCG] = GFL_CLGRP_CGR_Register( work->arcHandle , 
        NARC_comm_tvt_tv_t_upper_obj_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellResIdx[CTOR_COMMON_M_ANM] = GFL_CLGRP_CELLANIM_Register( work->arcHandle , 
        NARC_comm_tvt_tv_t_upper_obj_NCER , NARC_comm_tvt_tv_t_upper_obj_NANR, work->heapId  );

  work->cellResIdx[CTOR_COMMON_S_PLT] = GFL_CLGRP_PLTT_RegisterComp( work->arcHandle , 
        NARC_comm_tvt_tv_t_obj_NCLR , CLSYS_DRAW_SUB , 
        CTVT_PAL_OBJ_MAIN_COMMON*32 , work->heapId  );
  work->cellResIdx[CTOR_COMMON_S_NCG] = GFL_CLGRP_CGR_Register( work->arcHandle , 
        NARC_comm_tvt_tv_t_lower_obj_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
  work->cellResIdx[CTOR_COMMON_S_ANM] = GFL_CLGRP_CELLANIM_Register( work->arcHandle , 
        NARC_comm_tvt_tv_t_lower_obj_NCER , NARC_comm_tvt_tv_t_lower_obj_NANR, work->heapId  );

  //���ʑf��
  {
    ARCHANDLE *commonArcHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );
    
    //BG
    GFL_ARCHDL_UTIL_TransVramPalette( commonArcHandle , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_MAIN_BG , CTVT_PAL_BG_MAIN_BAR*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( commonArcHandle , APP_COMMON_GetBarCharArcIdx() ,
                      CTVT_FRAME_MAIN_MSG , 0 , 0, FALSE , work->heapId );
    //GFL_ARCHDL_UTIL_TransVramScreen( commonArcHandle , APP_COMMON_GetBarScrnArcIdx() , 
    //                  CTVT_FRAME_MAIN_MSG ,  0 , 0, FALSE , work->heapId );
    //GFL_BG_ChangeScreenPalette( CTVT_FRAME_MAIN_MSG , 0 , 21 , 32 , 3 , CTVT_PAL_BG_MAIN_BAR );
    //GFL_BG_LoadScreenReq( CTVT_FRAME_MAIN_BAR );

    GFL_ARCHDL_UTIL_TransVramPalette( commonArcHandle , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_SUB_BG , CTVT_PAL_BG_SUB_BAR*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( commonArcHandle , APP_COMMON_GetBarCharArcIdx() ,
                      CTVT_FRAME_SUB_BAR , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( commonArcHandle , APP_COMMON_GetBarScrnArcIdx() , 
                      CTVT_FRAME_SUB_BAR ,  0 , 0, FALSE , work->heapId );
    GFL_BG_ChangeScreenPalette( CTVT_FRAME_SUB_BAR , 0 , 21 , 32 , 3 , CTVT_PAL_BG_SUB_BAR );
    GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_BAR );
    
    //OBJ
    work->cellResIdx[CTOR_BAR_BUTTON_M_PLT] = GFL_CLGRP_PLTT_RegisterEx( commonArcHandle , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          CTVT_PAL_OBJ_MAIN_BARICON*32 , 0 , APP_COMMON_BARICON_PLT_NUM , work->heapId  );
    work->cellResIdx[CTOR_BAR_BUTTON_M_NCG] = GFL_CLGRP_CGR_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );

    work->cellResIdx[CTOR_BAR_BUTTON_S_PLT] = GFL_CLGRP_PLTT_RegisterEx( commonArcHandle , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_SUB , 
          CTVT_PAL_OBJ_SUB_BARICON*32 , 0 , APP_COMMON_BARICON_PLT_NUM , work->heapId  );
    work->cellResIdx[CTOR_BAR_BUTTON_S_NCG] = GFL_CLGRP_CGR_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellResIdx[CTOR_BAR_BUTTON_ANM] = GFL_CLGRP_CELLANIM_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    
    GFL_ARC_CloseDataHandle( commonArcHandle );
  }
  
  //��b�A�C�R��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = CTOAM_TALK;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;

    work->clwkRec = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( work->clwkRec , FALSE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkRec , TRUE );
  }

  //�p���b�g�A�j���f�[�^�̎擾
  {
    NNSG2dPaletteData* palData;
    void *resData = GFL_ARCHDL_UTIL_LoadPalette(work->arcHandle,NARC_comm_tvt_tv_t_tuuwa_bg_NCLR, &palData, work->heapId );
    
    GFL_STD_MemCopy32( (void*)((u32)palData->pRawData+32*(16-COMM_TVT_PAL_ANM_NUM)) ,
                       work->palAnmData , 32*COMM_TVT_PAL_ANM_NUM );
    GFL_HEAP_FreeMemory( resData );
  }

}

//--------------------------------------------------------------------------
//  ���\�[�X�J��
//--------------------------------------------------------------------------
static void COMM_TVT_ReleaseResource( COMM_TVT_WORK *work )
{
  u8 i;
  
  GFL_CLACT_WK_Remove( work->clwkRec );
  
  for( i=CTOR_PLT_TOP;i<CTOR_NCG_TOP;i++ )
  {
    GFL_CLGRP_PLTT_Release( work->cellResIdx[i] );
  }
  for( i=CTOR_NCG_TOP;i<CTOR_ANM_TOP;i++ )
  {
    GFL_CLGRP_CGR_Release( work->cellResIdx[i] );
  }
  for( i=CTOR_ANM_TOP;i<CTOR_MAX;i++ )
  {
    GFL_CLGRP_CELLANIM_Release( work->cellResIdx[i] );
  }
  GFL_ARC_CloseDataHandle( work->arcHandle );

}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n������
//--------------------------------------------------------------------------
static void COMM_TVT_InitMessage( COMM_TVT_WORK *work )
{
  //�t�H���g�ǂݍ���
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  //���b�Z�[�W
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_comm_tvt_dat , work->heapId );
  work->printQue = PRINTSYS_QUE_CreateEx( 1024*2 , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , CTVT_PAL_BG_MAIN_FONT*0x20, 16*2, work->heapId );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG  , CTVT_PAL_BG_SUB_FONT *0x20, 16*2, work->heapId );
  GFL_FONTSYS_SetDefaultColor();

  BmpWinFrame_GraphicSet( CTVT_FRAME_SUB_MSG , CTVT_BMPWIN_CGX , CTVT_PAL_BG_SUB_WINFRAME ,
                          0 , work->heapId );
  BmpWinFrame_GraphicSet( CTVT_FRAME_MAIN_MSG , CTVT_BMPWIN_CGX_MAIN , CTVT_PAL_BG_MAIN_WINFRAME ,
                          0 , work->heapId );
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 1 , 0 );
  work->timeIcon = NULL;
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n�J��
//--------------------------------------------------------------------------
static void COMM_TVT_TermMessage( COMM_TVT_WORK *work )
{
  if( work->timeIcon != NULL )
  {
    TIMEICON_Exit( work->timeIcon );
    work->timeIcon = NULL;
  }
  GFL_TCBL_Exit( work->tcblSys );
  PRINTSYS_QUE_Clear( work->printQue );
  PRINTSYS_QUE_Delete( work->printQue );
  GFL_MSG_Delete( work->msgHandle );
  GFL_FONT_Delete( work->fontHandle );

}

//--------------------------------------------------------------------------
//  ���[�h�̕ύX
//--------------------------------------------------------------------------
static void COMM_TVT_ChangeMode( COMM_TVT_WORK *work )
{
  //�J��
  switch( work->mode )
  {
  case CTM_TALK: //��b
    CTVT_TALK_TermMode( work , work->talkWork );
    break;
  case CTM_CALL: //�Ăяo��
    CTVT_CALL_TermMode( work , work->callWork );
    break;
  case CTM_DRAW: //������
    CTVT_DRAW_TermMode( work , work->drawWork );
    break;
  }


  work->mode = work->nextMode;
  
  //������
  switch( work->nextMode )
  {
  case CTM_TALK: //��b
    CTVT_TALK_InitMode( work , work->talkWork );
    break;
  case CTM_CALL: //�Ăяo��
    CTVT_CALL_InitMode( work , work->callWork );
    break;
  case CTM_DRAW: //������
    CTVT_DRAW_InitMode( work , work->drawWork );
    break;
  case CTM_ERROR: //�I��
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    //break throught;
  case CTM_END: //�I��
    CTVT_COMM_ExitComm( work , work->commWork );
    break;
  }
}

//--------------------------------------------------------------------------
//  ���ʂւ̖��O�\��
//--------------------------------------------------------------------------
static void COMM_TVT_UpdateUpperName( COMM_TVT_WORK *work )
{
  u8 i;

  if( work->mode == CTM_DRAW )
  {
    //���������͍X�V���Ȃ�
    return;
  }
  /*
  if( COMM_TVT_GetConnectNum(work) < 2 )
  {
    //��l�̎��͎Q�Əꏊ���Ⴄ
    if( CTVT_COMM_IsEnableMember( work , work->commWork , COMM_TVT_GetSelfIdx(work) ) == TRUE )
    {
      if( work->nameDrawBit == 0 )
      {
        CTVT_COMM_MEMBER_DATA *memData = CTVT_COMM_GetSelfMemberData( work , work->commWork );
        COMM_TVT_UpdateUpperNameFunc( work , (MYSTATUS*)memData->myStatusBuff , work->nameWin[2] );
        work->nameDrawBit = 1;
        work->nameWinUpdateBit = 1<<2;
      }
    }
  }
  else
  */
  {
    const COMM_TVT_DISP_MODE mode = COMM_TVT_GetDispMode( work );
    for( i=0;i<CTVT_MEMBER_NUM;i++ )
    {
      if( CTVT_COMM_IsEnableMember( work , work->commWork , i ) == TRUE &&
          CTVT_COMM_IsEnableMemberData( work , work->commWork , i ) == TRUE &&
          CTVT_CAMERA_IsUpdateCameraAnime( work , work->camWork , i ) == FALSE &&
          ( work->nameDrawBit & 1<<i ) == 0 )
      {
        u8 bmpIdx;
        CTVT_COMM_MEMBER_DATA *memData = CTVT_COMM_GetMemberData( work , work->commWork , i );
        if( mode == CTDM_DOUBLE )
        {
          if( i == 0 )
          {
            bmpIdx = 2;
          }
          else
          {
            bmpIdx = 3;
          }
        }
        else
        {
          bmpIdx = i;
        }
        COMM_TVT_UpdateUpperNameFunc( work , (MYSTATUS*)memData->myStatusBuff , work->nameWin[bmpIdx] );
        
        work->nameDrawBit += (1<<i);
        work->nameWinUpdateBit += 1<<bmpIdx;
      }
    }
  }
  
  for( i=0;i<CTVT_MEMBER_NUM;i++ )
  {
    if( (1<<i) & work->nameWinUpdateBit )
    {
      if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( work->nameWin[i] )) == FALSE )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->nameWin[i] );
        work->nameWinUpdateBit -= (1<<i);
      }
    }
  }
}

static void COMM_TVT_UpdateUpperNameFunc( COMM_TVT_WORK *work , MYSTATUS *myStatus , GFL_BMPWIN *bmpWin )
{
  //���O����
  
  STRBUF *str = MyStatus_CreateNameString( myStatus , work->heapId );
  const u8 nameLen = PRINTSYS_GetStrWidth( str , work->fontHandle , 0 );

  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) ,
                          ((COMM_TVT_NAME_WIDTH*8) - nameLen)/2+1 , 1 , str ,
                          work->fontHandle , PRINTSYS_LSB_Make( 1,0,0 ) );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) ,
                          ((COMM_TVT_NAME_WIDTH*8) - nameLen)/2-1 , 1 , str ,
                          work->fontHandle , PRINTSYS_LSB_Make( 1,0,0 ) );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) ,
                          ((COMM_TVT_NAME_WIDTH*8) - nameLen)/2 , 1+1 , str ,
                          work->fontHandle , PRINTSYS_LSB_Make( 1,0,0 ) );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) ,
                          ((COMM_TVT_NAME_WIDTH*8) - nameLen)/2 , 1-1 , str ,
                          work->fontHandle , PRINTSYS_LSB_Make( 1,0,0 ) );

  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) ,
                          ((COMM_TVT_NAME_WIDTH*8) - nameLen)/2 , 1 , str ,
                          work->fontHandle , PRINTSYS_LSB_Make( 0xf,0,0 ) );


  //    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( charaWork->nameWin ) ,
  //                            ((COMM_TVT_NAME_WIDTH*8) - charaWork->nameLen)/2 , 0 , str ,
  //                            work->fontHandle , PRINTSYS_LSB_Make( 1,0xf,0 ) );

  GFL_STR_DeleteBuffer( str );
}

#pragma mark [>outer func (system
CTVT_CAMERA_WORK* COMM_TVT_GetCameraWork( COMM_TVT_WORK *work )
{
  return work->camWork;
}
CTVT_COMM_WORK* COMM_TVT_GetCommWork( COMM_TVT_WORK *work )
{
  return work->commWork;
}
CTVT_TALK_WORK* COMM_TVT_GetTalkWork( COMM_TVT_WORK *work )
{
  return work->talkWork;
}
CTVT_MIC_WORK* COMM_TVT_GetMicWork( COMM_TVT_WORK *work )
{
  return CTVT_TALK_GetMicWork( work , work->talkWork );
}
DRAW_SYS_WORK* COMM_TVT_GetDrawSys( COMM_TVT_WORK *work )
{
  return work->drawSys;
}
const COMM_TVT_INIT_WORK* COMM_TVT_GetInitWork( const COMM_TVT_WORK *work )
{
  return work->initWork;
}

#pragma mark [>outer func (value
//--------------------------------------------------------------------------
//  �V�X�e�����ۂ�����
//--------------------------------------------------------------------------
const HEAPID COMM_TVT_GetHeapId( const COMM_TVT_WORK *work )
{
  return work->heapId;
}
ARCHANDLE* COMM_TVT_GetArcHandle( const COMM_TVT_WORK *work )
{
  return work->arcHandle;
}
const u32 COMM_TVT_GetObjResIdx( const COMM_TVT_WORK *work , const COMM_TVT_OBJ_RES resIdx )
{
  return work->cellResIdx[resIdx];
}
GFL_CLUNIT* COMM_TVT_GetCellUnit( const COMM_TVT_WORK *work )
{
  return work->cellUnit;
}
const BOOL COMM_TVT_GetUpperFade( const COMM_TVT_WORK *work )
{
  return work->isUpperFade;
}
void COMM_TVT_SetUpperFade( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isUpperFade = flg;
}

GFL_FONT* COMM_TVT_GetFontHandle( COMM_TVT_WORK *work )
{
  return work->fontHandle;
}

GFL_MSGDATA* COMM_TVT_GetMegHandle( COMM_TVT_WORK *work )
{
  return work->msgHandle;
}

PRINT_QUE* COMM_TVT_GetPrintQue( COMM_TVT_WORK *work )
{
  return work->printQue;
}
APP_TASKMENU_RES* COMM_TVT_GetTaskMenuRes( COMM_TVT_WORK *work )
{
  return work->taskMenuRes;
}


//--------------------------------------------------------------------------
//  ���[�h�Ƃ��l��
//--------------------------------------------------------------------------
const u8   COMM_TVT_GetConnectNum( const COMM_TVT_WORK *work )
{
  return work->connectNum;
}
void   COMM_TVT_SetConnectNum( COMM_TVT_WORK *work , const u8 num)
{
  work->connectNum = num;
}

const COMM_TVT_DISP_MODE COMM_TVT_GetDispMode( const COMM_TVT_WORK *work )
{
  const u8 num = COMM_TVT_GetConnectNum( work );
  if( num == 1 )
  {
    return CTDM_DOUBLE;
//    return CTDM_SINGLE;
  }
  else
  if( num == 2 )
  {
    return CTDM_DOUBLE;
  }
  return CTDM_FOUR;
}

const BOOL COMM_TVT_IsDoubleMode( const COMM_TVT_WORK *work )
{
  return work->isDouble;
}
//�t���O���M���N�G�X�g
void   COMM_TVT_SetDoubleMode( COMM_TVT_WORK *work , const BOOL flg )
{
  CTVT_COMM_SendFlgReq_Double( work , work->commWork , flg );
}
void   COMM_TVT_FlipDoubleMode( COMM_TVT_WORK *work )
{
  COMM_TVT_SetDoubleMode( work , !work->isDouble );
}

//�t���O�ύX
void   COMM_TVT_SetDoubleMode_Flag( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isDouble = flg;
}

const u8   COMM_TVT_GetSelfIdx( const COMM_TVT_WORK *work )
{
  return work->selfIdx;
}
void COMM_TVT_SetSelfIdx( COMM_TVT_WORK *work , const u8 idx )
{
  work->selfIdx = idx;
}
//--------------------------------------------------------------
//	�ꎞ��~(�V�X�e����
//--------------------------------------------------------------
const BOOL COMM_TVT_GetSusspend( COMM_TVT_WORK *work )
{
  return work->isSusspend;
}

void COMM_TVT_SetSusspend( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isSusspend = flg;
}

//--------------------------------------------------------------
//	�ꎞ��~(�@�\��
//--------------------------------------------------------------
const BOOL COMM_TVT_GetPause( COMM_TVT_WORK *work )
{
  return work->isPause;
}

void COMM_TVT_SetPause( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isPause = flg;
}

void COMM_TVT_FlipPause( COMM_TVT_WORK *work )
{
  work->isPause = !work->isPause;
}
//--------------------------------------------------------------
//	���G�`���ꎞ��~
//--------------------------------------------------------------
const BOOL COMM_TVT_GetSusspendDraw( COMM_TVT_WORK *work )
{
  return work->isSusspendDraw;
}

void COMM_TVT_SetSusspendDraw( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isSusspendDraw = flg;
}

//--------------------------------------------------------------
//	�I�����N�G�X�g
//--------------------------------------------------------------
const BOOL COMM_TVT_GetFinishReq( COMM_TVT_WORK *work )
{
  return work->isReqFinish;
}

void COMM_TVT_SetFinishReq( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isReqFinish = flg;
}

const BOOL COMM_TVT_IsWifi( COMM_TVT_WORK *work )
{
  if( work->initWork->mode == CTM_WIFI )
  {
    return TRUE;
  }
  return FALSE;
}

//�y�A�R���\�����N�G�X�g
const BOOL COMM_TVT_IsReqWarn( COMM_TVT_WORK *work )
{
  return work->isReqDispWarn;
}
void COMM_TVT_ResetIsReqWarn( COMM_TVT_WORK *work )
{
  work->isReqDispWarn = FALSE;
}

//�o�b�t�@�d�l���b�N
const BOOL COMM_TVT_IsUseSelfBuffer( COMM_TVT_WORK *work )
{
  return work->isUseSelfBUffer;
}
void COMM_TVT_SetUseSelfBuffer( COMM_TVT_WORK *work , const BOOL flg )
{
  work->isUseSelfBUffer = flg;
}
#pragma mark [>util func
//--------------------------------------------------------------------------
//  TWL�N�����`�F�b�N
//--------------------------------------------------------------------------
const BOOL COMM_TVT_IsTwlMode( void )
{
  return OS_IsRunOnTwl();
}

const BOOL COMM_TVT_CanUseCamera( void )
{
#if (defined(SDK_TWL))
  if( DS_SYSTEM_IsRunOnTwl() == TRUE &&
      DS_SYSTEM_IsRestrictPhotoExchange() == FALSE )
#else
  if( FALSE )
#endif
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  YesNo���o��
//--------------------------------------------------------------------------
APP_TASKMENU_WORK* COMM_TVT_OpenYesNoMenu( COMM_TVT_WORK *work )
{
  APP_TASKMENU_WORK *menuWork;
  APP_TASKMENU_INITWORK initWork = {0};
  APP_TASKMENU_ITEMWORK itemWork[2] = {0};
  
  itemWork[0].str = GFL_MSG_CreateString( work->msgHandle , COMM_TVT_SYS_03 );
  itemWork[1].str = GFL_MSG_CreateString( work->msgHandle , COMM_TVT_SYS_04 );
  itemWork[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  itemWork[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  itemWork[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  itemWork[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  
  initWork.heapId = work->heapId;
  initWork.itemNum = 2;
  initWork.itemWork = itemWork;
  initWork.posType = ATPT_LEFT_UP;
  initWork.charPosX = 32-APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  initWork.charPosY = 6;
  initWork.w = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  initWork.h = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  menuWork = APP_TASKMENU_OpenMenu( &initWork , work->taskMenuRes );
  
  GFL_STR_DeleteBuffer( itemWork[0].str );
  GFL_STR_DeleteBuffer( itemWork[1].str );
  
  return menuWork;
}


//--------------------------------------------------------------------------
//  ���O����������
//--------------------------------------------------------------------------
void COMM_TVT_RedrawName( COMM_TVT_WORK *work )
{
  u8 i;
  work->nameDrawBit = 0;
  work->nameWinUpdateBit = 0;
  for( i=0;i<CTVT_MEMBER_NUM;i++ )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->nameWin[i] ) , 0x0 );
    if( work->mode != CTM_DRAW )
    {
      GFL_BMPWIN_ClearScreen( work->nameWin[i] );
    }
  }
  //GFL_BG_LoadScreenV_Req( CTVT_FRAME_MAIN_MSG );
}

//--------------------------------------------------------------------------
//  ��b�A�C�R���̕\��
//--------------------------------------------------------------------------
void COMM_TVT_DispTalkIcon( COMM_TVT_WORK *work , const u8 idx )
{
  const COMM_TVT_DISP_MODE mode = COMM_TVT_GetDispMode( work );
  u8 pos = idx;
  //���G�`�����̓}�[�N���o���Ȃ�
  if( work->mode != CTM_DRAW )
  {
    if( mode == CTDM_DOUBLE )
    {
      if( idx == 0 )
      {
        pos = 2;
      }
      else
      {
        pos = 3;
      }
    }
    
    {
      //X�͖��O�̕�����
      const u8 posX[4] = { 64+8, 128+64+8 , 64+8 , 128+64+8 };
      const u8 posY[4] = { 96-8 , 96-8 , 192-8 , 192-8 };
      GFL_CLACTPOS cellPos;

      CTVT_COMM_MEMBER_DATA *memData = CTVT_COMM_GetMemberData( work , work->commWork , idx );
      MYSTATUS *myStatus = (MYSTATUS*)memData->myStatusBuff;
      STRBUF *str = MyStatus_CreateNameString( myStatus , work->heapId );
      const u8 nameLen = PRINTSYS_GetStrWidth( str , work->fontHandle , 0 );
      GFL_STR_DeleteBuffer( str );

      
      cellPos.x = posX[pos] + nameLen/2 ;
      cellPos.y = posY[pos];
      
      GFL_CLACT_WK_SetPos( work->clwkRec , &cellPos , CLSYS_DRAW_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->clwkRec , TRUE );
      GFL_CLACT_WK_ResetAnm( work->clwkRec );
    }
  }
}

void COMM_TVT_EraseTalkIcon( COMM_TVT_WORK *work )
{
  GFL_CLACT_WK_SetDrawEnable( work->clwkRec , FALSE );
}

void COMM_TVT_RegistPerson( COMM_TVT_WORK *work , MYSTATUS *myStatus )
{
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( work->initWork->gameData );
  ETC_SAVE_WORK *etcSave = SaveData_GetEtc( svWork );
  
  const u32 id = MyStatus_GetID( myStatus );
  
  EtcSave_SetAcquaintance( etcSave, id );
  CTVT_TPrintf("set person[%8x(%5d)]\n",id,(id&0x0000FFFF));
}

void COMM_TVT_CreateTimerIcon( COMM_TVT_WORK *work , GFL_BMPWIN *bmpWin )
{
  if( work->timeIcon != NULL )
  {
    COMM_TVT_DeleteTimerIcon( work );
  }
  work->timeIcon = TIMEICON_CreateTcbl( work->tcblSys , bmpWin , 0x0F , TIMEICON_DEFAULT_WAIT , work->heapId );
}

void COMM_TVT_DeleteTimerIcon( COMM_TVT_WORK *work )
{
  if( work->timeIcon != NULL )
  {
    TIMEICON_Exit( work->timeIcon );
    work->timeIcon = NULL;
  }
}

#pragma mark [>proc func
static GFL_PROC_RESULT COMM_TVT_Proc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT COMM_TVT_Proc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT COMM_TVT_Proc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA COMM_TVT_ProcData =
{
  COMM_TVT_Proc_Init,
  COMM_TVT_Proc_Main,
  COMM_TVT_Proc_Term
};

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
static GFL_PROC_RESULT COMM_TVT_Proc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  COMM_TVT_WORK *work;
  COMM_TVT_INIT_WORK *initWork = pwk;
  
  CTVT_TPrintf("Least heap[%x]\n",GFL_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
//  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMM_TVT, 0x100000 );
  if( pwk != NULL && 
      initWork->mode == CTM_WIFI )
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMM_TVT, 0xe8000-0x18000 );
  }
  else
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_COMM_TVT, 0xe8000 );
  }
  
  work = GFL_PROC_AllocWork( proc, sizeof(COMM_TVT_WORK), HEAPID_COMM_TVT );
  work->heapId = HEAPID_COMM_TVT;
  if( pwk == NULL )
  {
    work->initWork = GFL_HEAP_AllocClearMemory( GFL_HEAPID_APP , sizeof(COMM_TVT_INIT_WORK) );
    work->initWork->gameData = GAMEDATA_Create( GFL_HEAPID_APP );
    work->initWork->mode = CTM_TEST;

    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      work->initWork->mode = CTM_PARENT;
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
    {
      work->initWork->mode = CTM_CHILD;
    }
  }
  else
  {
    work->initWork = pwk;
  }
  work->isCommError = FALSE;
  
  COMM_TVT_Init( work );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
static GFL_PROC_RESULT COMM_TVT_Proc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  COMM_TVT_WORK *work = mywk;
  
  COMM_TVT_Term( work );

  if( pwk == NULL )
  {
    GFL_HEAP_FreeMemory( work->initWork );
  }

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_COMM_TVT );

  CTVT_TPrintf("FinishCamera\n");

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
static GFL_PROC_RESULT COMM_TVT_Proc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  COMM_TVT_WORK *work = mywk;

  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isCommError == FALSE )
  {
    NetErr_App_ReqErrorDisp();
    work->isCommError = TRUE;
  }

  if( COMM_TVT_Main( work ) == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}


