//======================================================================
/**
 * @file	mb_capture_sys.h
 * @brief	�}���`�u�[�g�E�ߊl�Q�[��
 * @author	ariizumi
 * @data	09/11/24
 *
 * ���W���[�����FMB_CAPTURE
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/wordset.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/net_err.h"

#include "arc_def.h"
#include "mb_capture_gra.naix"
#include "font/font.naix"

#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_poke_icon.h"
#include "multiboot/mb_local_def.h"
#include "multiboot/mb_capture_sys.h"
#include "./mb_cap_obj.h"
#include "./mb_cap_poke.h"
#include "./mb_cap_down.h"
#include "./mb_cap_ball.h"
#include "./mb_cap_effect.h"
#include "./mb_cap_demo.h"
#include "./mb_cap_local_def.h"
#include "./mb_cap_snd_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CAP_TARGET_ANM_SPEED (6)
#define MB_CAP_TARGET_ANM_FRAME (6)

#define MB_CAP_TIME_GAUGE_POS_X (7)
#define MB_CAP_TIME_GAUGE_POS_Y (2)
#define MB_CAP_TIME_GAUGE_WIDTH_CHARA (18)
#define MB_CAP_TIME_GAUGE_WIDTH (MB_CAP_TIME_GAUGE_WIDTH_CHARA*8)
#define MB_CAP_TIME_GAUGE_CHARA_G (0x01 + 0x2000)
#define MB_CAP_TIME_GAUGE_CHARA_Y (0x21 + 0x2000)
#define MB_CAP_TIME_GAUGE_CHARA_R (0x41 + 0x2000)

#define MB_CAP_BONUS_FOG_COLOR (0x7e5f)
#define MB_CAP_BONUS_FOG_ALPHA_MIN (16)
#define MB_CAP_BONUS_FOG_ALPHA_MAX (31)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MSS_FADEIN,
  MSS_WAIT_FADEIN,
  MSS_FADEOUT,
  MSS_WAIT_FADEOUT,

  MSS_INIT_DEMO,
  MSS_GAME_MAIN,
  MSS_FINISH_DEMO,
  
}MB_CAPTURE_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_CAPTURE_WORK
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  MB_CAPTURE_INIT_WORK *initWork;
  
  MB_CAPTURE_STATE  state;
  BOOL isNetErr;
  
  GFL_G3D_CAMERA    *camera;
  GFL_BBD_SYS     *bbdSys;
  int             bbdRes[MCBR_MAX];
  int             bbdResBgMask;
  int             bbdObjBgMask[2];  //128�����E�Ȃ̂�2��.
  
  u16 score;
  
  MB_CAP_POKE *pokeWork[MB_CAP_POKE_NUM];
  MB_CAP_OBJ  *objWork[MB_CAP_OBJ_NUM];
  MB_CAP_OBJ  *starWork;
  MB_CAP_BALL  *ballWork[MB_CAP_BALL_NUM];
  MB_CAP_EFFECT  *effWork[MB_CAP_EFFECT_NUM];
  MB_CAP_DOWN *downWork;
  MB_CAP_DEMO *demoWork;
  
  //���ʌn
  BOOL isShotBall;
  int resIdxTarget;
  int objIdxTarget;
  u8 targetAnmCnt;
  u16 targetAnmFrame;
  
  u16  randLen;  //�Ə�����p
  fx32 befPullLen;
  u16  befRotAngle;
  
  //Fog
  u8  fogAlpha;

  u16 gameTime;
  u16 bonusTime;
  
  BOOL isCreateStar;
  BOOL isUpdateStar;
  
  VecFx32 targetPos;
  
  void* sndData;

  //�|�P�����ӂ��p
#if MB_CAPTURE_USE_EDGE
  u16           edgePlttAnmCnt;
  u16           edgePltt[16];
  NNSGfdPlttKey edgePltKey;
#endif //MB_CAPTURE_USE_EDGE

  GFL_FONT    *fontHandle;
  PRINT_QUE   *printQue;
  GFL_MSGDATA *msgHandle;

#if MB_CAP_DEB
  BOOL isTimeFreeze;
#endif
};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MB_CAPTURE_Init( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_Term( MB_CAPTURE_WORK *work );
static const BOOL MB_CAPTURE_Main( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_VBlankFunc(GFL_TCB *tcb, void *wk );

static void MB_CAPTURE_InitGraphic( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_TermGraphic( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_CAPTURE_LoadResource( MB_CAPTURE_WORK *work );

static void MB_CAPTURE_InitObject( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_TermObject( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_InitPoke( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_TermPoke( MB_CAPTURE_WORK *work );

static void MB_CAPTURE_InitUpper( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_TermUpper( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_UpdateUpper( MB_CAPTURE_WORK *work );

static void MB_CAPTURE_InitTime( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_UpdateTime( MB_CAPTURE_WORK *work );

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_64_E,             // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,       // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,         // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_NONE ,           // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_128_D,        // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_01_AB,             // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_01_FG,         // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};

#if MB_CAP_DEB
static void MB_CAPTURE_InitDebug( MB_CAPTURE_WORK *work );
static void MB_CAPTURE_TermDebug( MB_CAPTURE_WORK *work );
#endif

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static void MB_CAPTURE_Init( MB_CAPTURE_WORK *work )
{
  u8 i;
  work->isNetErr = FALSE;
  work->state = MSS_FADEIN;
  
  work->sndData = GFL_ARCHDL_UTIL_Load( work->initWork->arcHandle ,
                                        NARC_mb_capture_gra_wb_sound_palpark_game_sdat ,
                                        FALSE ,
                                        work->heapId );
  MB_CAPTURE_InitGraphic( work );
  MB_CAPTURE_LoadResource( work );

  PMSND_InitMultiBoot(work->sndData);
  
  work->vBlankTcb = GFUser_VIntr_CreateTCB( MB_CAPTURE_VBlankFunc , work , 8 );

  MB_CAPTURE_InitObject( work );
  MB_CAPTURE_InitPoke( work );
  MB_CAPTURE_InitUpper( work );

  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE_MB , NARC_message_multiboot_child_dat , work->heapId );
#else                    //DL�q�@������
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE_MB , NARC_message_dl_multiboot_child_dat , work->heapId );
#endif //MULTI_BOOT_MAKE
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
  GFL_FONTSYS_SetDefaultColor();

  BmpWinFrame_GraphicSet( MB_CAPTURE_FRAME_MSG , MB_CAPTURE_MSGWIN_CGX , MB_CAPTURE_PAL_MAIN_BG_MSGWIN , 0 , work->heapId );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , FILE_FONT_PLT_MB , PALTYPE_MAIN_BG , MB_CAPTURE_PAL_MAIN_BG_FONT*0x20, 16*2, work->heapId );

  work->downWork = MB_CAP_DOWN_InitSystem( work );
  work->demoWork = MB_CAP_DEMO_InitSystem( work );
  work->isShotBall = FALSE;
  work->targetAnmCnt = 0;
  work->targetAnmFrame = 0;
  work->fogAlpha = 0;
  work->score = 0;
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    work->initWork->isCapture[i] = FALSE;
  }

  for( i=0;i<MB_CAP_BALL_NUM;i++ )
  {
    work->ballWork[i] = NULL;
  }
  for( i=0;i<MB_CAP_EFFECT_NUM;i++ )
  {
    work->effWork[i] = NULL;
  }
  
  MB_CAPTURE_InitTime( work );
  
  PMSND_PlayBGM( SEQ_BGM_PALPARK_GAME );

  GFL_NET_WirelessIconEasyXY( 256-16 , 0 , FALSE , work->heapId );

  GFL_NET_WirelessIconEasy_HoldLCD( FALSE , work->heapId );
  GFL_NET_ReloadIcon();

#if MB_CAP_DEB
  work->isTimeFreeze = FALSE;
  MB_CAPTURE_InitDebug( work );
#endif
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static void MB_CAPTURE_Term( MB_CAPTURE_WORK *work )
{
  u8 i;
#if MB_CAP_DEB
  MB_CAPTURE_TermDebug( work );
#endif

  GFL_MSG_Delete( work->msgHandle );
  PRINTSYS_QUE_Delete( work->printQue );
  GFL_FONT_Delete( work->fontHandle );

  GFL_NET_WirelessIconEasyEnd();
  PMSND_StopBGM();

  MB_TPrintf( "CaptureCheck:" );
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    if( MB_CAP_POKE_GetState( work->pokeWork[i] ) == MCPS_CAPTURE )
    {
      work->initWork->isCapture[i] = TRUE;
      MB_TPrintf( "[o]" );
    }
    else
    {
      work->initWork->isCapture[i] = FALSE;
      MB_TPrintf( "[x]" );
    }
  }
  MB_TPrintf( "\n" );

#ifdef PM_DEBUG
  if( ( GFL_UI_KEY_GetCont() & PAD_BUTTON_START  )&&
      ( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ))
  {
    for( i=0;i<MB_CAP_POKE_NUM;i++ )
    {
      work->initWork->isCapture[i] = TRUE;
    }
    MB_TPrintf( "Debug all capture!!\n" );
  }
#endif

  work->initWork->score = work->score;
  MB_TPrintf( "Score[%d]\n",work->score );

  for( i=0;i<MB_CAP_BALL_NUM;i++ )
  {
    if( work->ballWork[i] != NULL )
    {
      MB_CAP_BALL_DeleteObject( work , work->ballWork[i] );
    }
  }
  for( i=0;i<MB_CAP_EFFECT_NUM;i++ )
  {
    if( work->effWork[i] != NULL )
    {
      MB_CAP_EFFECT_DeleteObject( work , work->effWork[i] );
    }
  }

  MB_CAP_DEMO_DeleteSystem( work , work->demoWork );
  MB_CAP_DOWN_DeleteSystem( work , work->downWork );
  MB_CAPTURE_TermUpper( work );
  MB_CAPTURE_TermObject( work );
  MB_CAPTURE_TermPoke( work );

  GFL_TCB_DeleteTask( work->vBlankTcb );

  GFL_BBD_RemoveResource( work->bbdSys , work->bbdResBgMask );
  for( i=0;i<MCBR_MAX;i++ )
  {
    GFL_BBD_RemoveResource( work->bbdSys , work->bbdRes[i] );
  }

  MB_CAPTURE_TermGraphic( work );

  PMSND_Exit();
  GFL_HEAP_FreeMemory( work->sndData );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static const BOOL MB_CAPTURE_Main( MB_CAPTURE_WORK *work )
{
  int i;
  switch( work->state )
  {
  case MSS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    work->state = MSS_WAIT_FADEIN;
    break;
    
  case MSS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->state = MSS_INIT_DEMO;
      MC_CAP_DEMO_StartDemoInit( work , work->demoWork );
    }
    break;

  case MSS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MSS_WAIT_FADEOUT;
    break;
    
  case MSS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  case MSS_INIT_DEMO:
    if( MC_CAP_DEMO_StartDemoMain( work , work->demoWork ) == TRUE )
    {
      u8 i;
      MC_CAP_DEMO_StartDemoTerm( work , work->demoWork );

      //�o���ݒ�
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        BOOL isLoop = TRUE;
        while( isLoop == TRUE )
        {
          u8 j;
          const u8 idxX = GFUser_GetPublicRand0( MB_CAP_OBJ_X_NUM );
          const u8 idxY = GFUser_GetPublicRand0( MB_CAP_OBJ_Y_NUM );
          isLoop = FALSE;
          for( j=0;j<i;j++ )
          {
            if( MB_CAP_POKE_CheckPos( work->pokeWork[j] , idxX , idxY ) == TRUE )
            {
              isLoop = TRUE;
              break;
            }
          }
          if( isLoop == FALSE )
          {
            MB_CAP_POKE_SetHide( work , work->pokeWork[j] , idxX , idxY );
            MB_TPrintf("[%d][%d]\n",idxX,idxY);
          }
        }
      }
      work->state = MSS_GAME_MAIN;
    }
    break;

  case MSS_GAME_MAIN:
    {
      u8 i;
      BOOL isAllCap = TRUE;
      MB_CAPTURE_UpdateTime( work );
      
      //�S�ߊl�`�F�b�N
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        if( MB_CAP_POKE_GetState( work->pokeWork[i] ) != MCPS_CAPTURE )
        {
          isAllCap = FALSE;
          break;
        }
      }
      
      if( isAllCap == TRUE || work->gameTime == 0 )
      {
        BOOL isHighScore = FALSE;
        if( work->score > work->initWork->highScore )
        {
          isHighScore = TRUE;
        }
        MC_CAP_DEMO_FinishDemoInit( work , work->demoWork , isAllCap , isHighScore );
        work->state = MSS_FINISH_DEMO;
      }
    }
    break;
  case MSS_FINISH_DEMO:
    if( MC_CAP_DEMO_FinishDemoMain( work , work->demoWork ) == TRUE )
    {
      MC_CAP_DEMO_FinishDemoTerm( work , work->demoWork );
      work->state = MSS_FADEOUT;
    }
    
    break;
  }

  for( i=0;i<MB_CAP_OBJ_NUM;i++ )
  {
    MB_CAP_OBJ_UpdateObject( work , work->objWork[i] );
  }
  if( work->state == MSS_GAME_MAIN )
  {
    //�f������DemoMain���瑀�삷��
    MB_CAP_DOWN_UpdateSystem( work , work->downWork );
  }
  MB_CAPTURE_UpdateUpper( work );

  
  //�p���b�g�A�j��
#if MB_CAPTURE_USE_EDGE
  {
    const u16 anmSpd = 0x400;
    if( work->edgePlttAnmCnt + anmSpd < 0x10000 )
    {
      work->edgePlttAnmCnt += anmSpd;
    }
    else
    {
      work->edgePlttAnmCnt += anmSpd-0x10000;
    }
    
    for( i=0;i<16;i++ )
    {
      const fx32 sin = FX_SinIdx( work->edgePlttAnmCnt );
      //31,31,24 �� 31,24,24
      const u8 g = 24 + FX_FX32_TO_F32( (31-24) * sin );
      work->edgePltt[i] = GX_RGB( 31 , g , 24 );
    }
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_3D_TEX_PLTT ,
                                        NNS_GfdGetPlttKeyAddr( work->edgePltKey ) ,
                                        work->edgePltt , 2*16 );
  }
#endif //MB_CAPTURE_USE_EDGE

  //3D�`��  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
  }
  GFL_G3D_DRAW_End();

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  PRINTSYS_QUE_Main( work->printQue );

#ifdef PM_DEBUG
  if( ( GFL_UI_KEY_GetCont() & PAD_BUTTON_START  )&&
      ( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ))
  {
    return TRUE;
  }
#endif
  return FALSE;
}


//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void MB_CAPTURE_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  //OBJ�̍X�V
  GFL_CLACT_SYS_VBlankFunc();
}

//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void MB_CAPTURE_InitGraphic( MB_CAPTURE_WORK *work )
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
  
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  GFL_DISP_SetBank( &vramBank );

  //BG�n�̏�����
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_1, GX_BG0_AS_3D,
    };
    // BG1 MAIN (�g
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000,0x0000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (�f�o�b�O
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 0, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 SUB (�|�̌�
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (�|�E�A�t�B��
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x10000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_CAPTURE_SetupBgFunc( &header_main1 , MB_CAPTURE_FRAME_FRAME , GFL_BG_MODE_TEXT );
    MB_CAPTURE_SetupBgFunc( &header_main2 , MB_CAPTURE_FRAME_DEBUG , GFL_BG_MODE_TEXT );
    MB_CAPTURE_SetupBgFunc( &header_main3 , MB_CAPTURE_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_CAPTURE_SetupBgFunc( &header_sub1  , MB_CAPTURE_FRAME_SUB_BOW_LINE , GFL_BG_MODE_TEXT );
    MB_CAPTURE_SetupBgFunc( &header_sub2  , MB_CAPTURE_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    MB_CAPTURE_SetupBgFunc( &header_sub3  , MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_MODE_AFFINE );
    
  }

  //OBJ�n�̏�����
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
  //3D�̏�����
  
  { //3D�n�̐ݒ�
    static const VecFx32 cam_pos = {0,0,FX32_CONST(512.0f)};
    static const VecFx32 cam_target = {0,0,0};
    static const VecFx32 cam_up = {0,FX32_ONE,0};
    //�G�b�W�}�[�L���O�J���[
    static  const GXRgb stage_edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
            0, work->heapId, NULL );
    GFL_BG_SetBGControl3D( 2 ); //NNS_g3dInit�̒��ŕ\���D�揇�ʕς��E�E�E
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
    //���ˉe�J����
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       0,
                       FX32_ONE*192.0f,
                       0,
                       FX32_ONE*256.0f,
                       FX32_ONE,
                       FX32_ONE*600,
                       0,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
    
    GFL_G3D_CAMERA_Switching( work->camera );
    //�G�b�W�}�[�L���O�J���[�Z�b�g
    G3X_SetEdgeColorTable( &stage_edge_color_table[0] );
    G3X_EdgeMarking( TRUE );
    G3X_AntiAlias( TRUE );
    G3X_AlphaBlend( TRUE );
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
    
    //3D��2D�̃u�����h
    G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,TRUE);
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0 , GX_BLEND_PLANEMASK_BG3 , 31 , 31 );
  }
  {
    //�������o�̂���2�{(16->32)
    VecFx32 bbdScale = {FX32_CONST(32.0f),FX32_CONST(32.0f),FX32_CONST(32.0f)};
    GFL_BBD_SETUP bbdSetup = {
      128,128,
      {FX32_ONE,FX32_ONE,FX32_ONE},
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      63,
      GFL_BBD_ORIGIN_CENTER,
    };
    //�r���{�[�h�V�X�e���\�z
    work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
    GFL_BBD_SetScale( work->bbdSys , &bbdScale );
  }
}

static void MB_CAPTURE_TermGraphic( MB_CAPTURE_WORK *work )
{
  GFL_BBD_DeleteSys( work->bbdSys );
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_CLACT_SYS_Delete();
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_FRAME );
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_DEBUG );
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_BG );
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_SUB_BOW_LINE );
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( MB_CAPTURE_FRAME_SUB_BOW );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void MB_CAPTURE_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  ���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void MB_CAPTURE_LoadResource( MB_CAPTURE_WORK *work )
{
  //����ʂ�mb_cap_down��
  
  //����
  GFL_ARCHDL_UTIL_TransVramPalette( work->initWork->arcHandle , NARC_mb_capture_gra_cap_bgu_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->initWork->arcHandle , NARC_mb_capture_gra_cap_bgu_NCGR ,
                    MB_CAPTURE_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->initWork->arcHandle , NARC_mb_capture_gra_cap_bgu_field_NSCR , 
                    MB_CAPTURE_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->initWork->arcHandle , NARC_mb_capture_gra_cap_bgu_gage_NSCR , 
                    MB_CAPTURE_FRAME_FRAME ,  0 , 0, FALSE , work->heapId );

  GFL_BG_LoadScreenV_Req( MB_CAPTURE_FRAME_BG );
  GFL_BG_LoadScreenV_Req( MB_CAPTURE_FRAME_FRAME );
  
  //BBD���\�[�X
  {
    static const u32 resListArr[MCBR_MAX] = {
      NARC_mb_capture_gra_cap_obj_shadow_nsbtx ,
      NARC_mb_capture_gra_cap_obj_ballnormal_nsbtx ,
      NARC_mb_capture_gra_cap_obj_ballbonus_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kusamura1_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kusamura2_nsbtx ,
      NARC_mb_capture_gra_cap_obj_wood_nsbtx ,
      NARC_mb_capture_gra_cap_obj_water_nsbtx ,
      NARC_mb_capture_gra_cap_obj_star_nsbtx ,
      NARC_mb_capture_gra_cap_obj_hit_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kemuri_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kemuri2_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kemuri3_nsbtx ,
      NARC_mb_capture_gra_cap_obj_getefect_nsbtx ,
      NARC_mb_capture_gra_cap_obj_memai_nsbtx ,
      NARC_mb_capture_gra_cap_obj_kemuri_bonus_nsbtx ,  //�{�[�i�X��
      NARC_mb_capture_gra_cap_obj_zzz_nsbtx ,
      //���̑�
      NARC_mb_capture_gra_cap_obj_ready_nsbtx ,
      NARC_mb_capture_gra_cap_obj_start_nsbtx ,
      NARC_mb_capture_gra_cap_obj_finish_nsbtx ,
      NARC_mb_capture_gra_cap_obj_timeup_nsbtx ,
      NARC_mb_capture_gra_cap_obj_highscore_nsbtx ,
      };
    static const u32 resSizeArr[MCBR_MAX] = {
      GFL_BBD_TEXSIZDEF_32x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_32x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_64x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_256x32 ,  //�{�[�i�X��
      GFL_BBD_TEXSIZDEF_128x32 ,
      //���̑�
      GFL_BBD_TEXSIZDEF_128x128 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      GFL_BBD_TEXSIZDEF_128x32 ,
      
      };
    u8 i;
    
    for( i=0;i<MCBR_MAX;i++ )
    {
      GFL_G3D_RES* res = GFL_G3D_CreateResourceHandle( work->initWork->arcHandle , resListArr[i] );
      GFL_G3D_TransVramTexture( res );
      if( i < MCBR_DEMO_READY )
      {
        work->bbdRes[i] = GFL_BBD_AddResource( work->bbdSys , 
                                         res , 
                                         GFL_BBD_TEXFMT_PAL16 ,
                                         resSizeArr[i] ,
                                         32 , 32 );
      }
      else
      {
        work->bbdRes[i] = GFL_BBD_AddResource( work->bbdSys , 
                                         res , 
                                         GFL_BBD_TEXFMT_PAL16 ,
                                         resSizeArr[i] ,
                                         128 , 32 );
      }
      GFL_BBD_CutResourceData( work->bbdSys , work->bbdRes[i] );
    }
  }
  {
    GFL_G3D_RES* res = GFL_G3D_CreateResourceHandle( work->initWork->arcHandle , NARC_mb_capture_gra_cap_bg_mask_nsbtx );
    GFL_G3D_TransVramTexture( res );
    work->bbdResBgMask = GFL_BBD_AddResource( work->bbdSys , 
                                         res , 
                                         GFL_BBD_TEXFMT_PAL16 ,
                                         GFL_BBD_TEXSIZDEF_256x32 ,
                                         128 , 32 );
  }
}

//--------------------------------------------------------------
//  �I�u�W�F�N�g�쐬
//--------------------------------------------------------------
static void MB_CAPTURE_InitObject( MB_CAPTURE_WORK *work )
{
  int i;
  MB_CAP_OBJ_INIT_WORK initWork;
  for( i=0;i<MB_CAP_OBJ_MAIN_NUM;i++ )
  {
    const u8 idxX = (i%MB_CAP_OBJ_X_NUM);
    const u8 idxY = (i/MB_CAP_OBJ_X_NUM);
    initWork.type = MCOT_GRASS;
    MB_CAPTURE_GetGrassObjectPos( idxX , idxY , &initWork.pos );
    work->objWork[i] = MB_CAP_OBJ_CreateObject( work , &initWork );
  }
  for( i=MB_CAP_OBJ_SUB_U_START;i<MB_CAP_OBJ_SUB_D_START;i++ )
  {
    const int idx = i-MB_CAP_OBJ_SUB_U_START;
    initWork.type = MCOT_WOOD;
    MB_CAPTURE_GetGrassObjectPos( idx , -1 , &initWork.pos );
    initWork.pos.y = FX32_CONST( MB_CAP_OBJ_SUB_U_TOP );
    initWork.pos.z -= FX32_CONST(MB_CAP_OBJ_LINEOFS_Z);
    work->objWork[i] = MB_CAP_OBJ_CreateObject( work , &initWork );
  }
  for( i=MB_CAP_OBJ_SUB_D_START;i<MB_CAP_OBJ_SUB_R_START;i++ )
  {
    const int idx = i-MB_CAP_OBJ_SUB_D_START;
    initWork.type = MCOT_GRASS_SIDE;
    MB_CAPTURE_GetGrassObjectPos( idx , MB_CAP_OBJ_Y_NUM , &initWork.pos );
    initWork.pos.y = FX32_CONST( MB_CAP_OBJ_SUB_D_TOP );
    work->objWork[i] = MB_CAP_OBJ_CreateObject( work , &initWork );
  }
  for( i=MB_CAP_OBJ_SUB_R_START;i<MB_CAP_OBJ_SUB_L_START;i++ )
  {
    const int idx = i-MB_CAP_OBJ_SUB_R_START;
    initWork.type = MCOT_GRASS_SIDE;
    MB_CAPTURE_GetGrassObjectPos( MB_CAP_OBJ_X_NUM , idx , &initWork.pos );
    initWork.pos.x = FX32_CONST( MB_CAP_OBJ_SUB_R_LEFT );
    work->objWork[i] = MB_CAP_OBJ_CreateObject( work , &initWork );
  }
  for( i=MB_CAP_OBJ_SUB_L_START;i<MB_CAP_OBJ_NUM;i++ )
  {
    const int idx = i-MB_CAP_OBJ_SUB_L_START;
    initWork.type = MCOT_GRASS_SIDE;
    MB_CAPTURE_GetGrassObjectPos( -1 , idx , &initWork.pos );
    initWork.pos.x = FX32_CONST( MB_CAP_OBJ_SUB_L_LEFT );
    work->objWork[i] = MB_CAP_OBJ_CreateObject( work , &initWork );
  }
  
  initWork.type = MCOT_STAR;
  initWork.pos.x = FX32_CONST( 300 );
  initWork.pos.y = FX32_CONST( 64 );
  work->starWork = MB_CAP_OBJ_CreateObject( work , &initWork );
  MB_CAP_OBJ_SetEnable( work , work->starWork , FALSE );
  
  //BG�}�X�N
  {
    BOOL flg = TRUE;
    VecFx32 pos = {FX32_CONST(64.0),FX32_CONST(72.0),FX32_CONST(MB_CAP_OBJ_BASE_Z- MB_CAP_OBJ_LINEOFS_Z)};
    //�������o�̂��߃T�C�Y��2�{�ɂ��Ă�̂Ńf�t�H�͔���
    work->bbdObjBgMask[0] = GFL_BBD_AddObject( work->bbdSys , 
                                       work->bbdResBgMask ,
                                       FX32_HALF*4 , 
                                       FX32_HALF , 
                                       &pos ,
                                       31 ,
                                       GFL_BBD_LIGHT_NONE );
    pos.x = FX32_CONST(192.0);
    work->bbdObjBgMask[1] = GFL_BBD_AddObject( work->bbdSys , 
                                       work->bbdResBgMask ,
                                       FX32_HALF*4 , 
                                       FX32_HALF , 
                                       &pos ,
                                       31 ,
                                       GFL_BBD_LIGHT_NONE );
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[0] , &flg );
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[1] , &flg );
    //3D�ݒ�̊֌W�Ŕ��]������E�E�E
    GFL_BBD_SetObjectFlipT( work->bbdSys , work->bbdObjBgMask[0] , &flg );
    GFL_BBD_SetObjectFlipT( work->bbdSys , work->bbdObjBgMask[1] , &flg );
  }
}

//--------------------------------------------------------------
//  �I�u�W�F�N�g�J��
//--------------------------------------------------------------
static void MB_CAPTURE_TermObject( MB_CAPTURE_WORK *work )
{
  int i;
  GFL_BBD_RemoveObject( work->bbdSys , work->bbdObjBgMask[0] );
  GFL_BBD_RemoveObject( work->bbdSys , work->bbdObjBgMask[1] );
  MB_CAP_OBJ_DeleteObject( work , work->starWork );
  for( i=0;i<MB_CAP_OBJ_NUM;i++ )
  {
    MB_CAP_OBJ_DeleteObject( work , work->objWork[i] );
  }
}

//--------------------------------------------------------------
//  �|�P�����쐬
//--------------------------------------------------------------
static void MB_CAPTURE_InitPoke( MB_CAPTURE_WORK *work )
{
  int i,j;
  MB_CAP_POKE_INIT_WORK initWork;
  initWork.pokeArcHandle = MB_ICON_GetArcHandle(GFL_HEAP_LOWID(work->heapId),work->initWork->cardType);

#if MB_CAPTURE_USE_EDGE
  //�ӂ��p�p���b�g�m��
  work->edgePltKey = NNS_GfdAllocPlttVram( 2*16 , FALSE , 0 );
  work->edgePlttAnmCnt = 0;
  for( i=0;i<16;i++ )
  {
    work->edgePltt[i] = 0x63ff;  //����
  }
  initWork.edgePlttKey = work->edgePltKey;
#endif //MB_CAPTURE_USE_EDGE
  
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    initWork.ppp = work->initWork->ppp[i];
    work->pokeWork[i] = MB_CAP_POKE_CreateObject( work , &initWork );
  }
  GFL_ARC_CloseDataHandle( initWork.pokeArcHandle );

}

//--------------------------------------------------------------
//  �|�P�����J��
//--------------------------------------------------------------
static void MB_CAPTURE_TermPoke( MB_CAPTURE_WORK *work )
{
  int i;
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    MB_CAP_POKE_DeleteObject( work , work->pokeWork[i] );
  }
  
#if MB_CAPTURE_USE_EDGE
  NNS_GfdFreePlttVram( work->edgePltKey );
#endif //MB_CAPTURE_USE_EDGE
}

#pragma mark [>upper func
//--------------------------------------------------------------
//  ���ʌn������
//--------------------------------------------------------------
static void MB_CAPTURE_InitUpper( MB_CAPTURE_WORK *work )
{
  GFL_G3D_RES* res = GFL_G3D_CreateResourceHandle( work->initWork->arcHandle , NARC_mb_capture_gra_cap_obj_target_nsbtx );
  VecFx32 pos = {0,0,0};
  const BOOL flg = TRUE;
  GFL_G3D_TransVramTexture( res );
  work->resIdxTarget = GFL_BBD_AddResource( work->bbdSys , 
                                       res , 
                                       GFL_BBD_TEXFMT_PAL16 ,
                                       GFL_BBD_TEXSIZDEF_256x32 ,
                                       32 , 32 );
  GFL_BBD_CutResourceData( work->bbdSys , work->resIdxTarget );
  
    //�������o�̂��߃T�C�Y��2�{�ɂ��Ă�̂Ńf�t�H�͔���
  work->objIdxTarget = GFL_BBD_AddObject( work->bbdSys , 
                                     work->resIdxTarget ,
                                     FX32_HALF , 
                                     FX32_HALF , 
                                     &pos ,
                                     31 ,
                                     GFL_BBD_LIGHT_NONE );

  //3D�ݒ�̊֌W�Ŕ��]������E�E�E
  GFL_BBD_SetObjectFlipT( work->bbdSys , work->objIdxTarget , &flg );

}

//--------------------------------------------------------------
//  ���ʌn�J��
//--------------------------------------------------------------
static void MB_CAPTURE_TermUpper( MB_CAPTURE_WORK *work )
{
  GFL_BBD_RemoveObject( work->bbdSys , work->resIdxTarget );
  GFL_BBD_RemoveResource( work->bbdSys , work->objIdxTarget );

}

//--------------------------------------------------------------
//  ���ʌn�X�V
//--------------------------------------------------------------
static void MB_CAPTURE_UpdateUpper( MB_CAPTURE_WORK *work )
{
  u8 i,j;
  u8 activeBallNum = 0;
  const MB_CAP_DOWN_STATE downState = MB_CAP_DOWN_GetState( work->downWork );

  if( downState == MCDS_DRAG )
  {
    const BOOL flg = TRUE;
    VecFx32 pos;
    VecFx32 ofs = {0,0,0};
    const fx32 pullLen = MB_CAP_DOWN_GetPullLen( work->downWork );
    const u16 rotAngle = MB_CAP_DOWN_GetRotAngle( work->downWork );
    
    pos.x = FX32_CONST(128);
    pos.y = FX32_CONST(MB_CAP_UPPER_BALL_POS_BASE_Y);
    pos.z = FX32_CONST(MB_CAP_UPPER_TARGET_BASE_Z);
    {
      //�����v�Z
      const fx32 rate = FX_Div(pullLen,MB_CAP_DOWN_BOW_PULL_LEN_MAX);
      const fx32 len = (MB_CAP_UPPER_BALL_LEN_MAX-MB_CAP_UPPER_BALL_LEN_MIN)*rate + FX32_CONST(MB_CAP_UPPER_BALL_LEN_MIN);
      ofs.y = len;
    }
    {
      //��]�v�Z
      const fx32 sin = FX_SinIdx( rotAngle );
      const fx32 cos = FX_CosIdx( rotAngle );
      
      ofs.x = -FX_Mul( ofs.y , sin );
      ofs.y = FX_Mul( ofs.y , cos );
    }
    //�덷�p�~
    /*
    {
      //�����_���덷����
      fx32 lenSub = work->befPullLen - pullLen;
      u16 rotSub = work->befRotAngle - rotAngle;
      
      if( lenSub < 0 )
      {
        lenSub *= -1;
      }
      if( rotSub > 0x8000 )
      {
        rotSub = 0x10000-rotSub;
      }

      //�␳�l
      lenSub *= 8;
      rotSub /= 10;
      
      work->randLen += (int)F32_CONST(lenSub) + rotSub;
      
      if( work->randLen < MB_CAP_TARGET_RAND_DOWN )
      {
        work->randLen = 0;
      }
      else
      if( work->randLen > MB_CAP_TARGET_RAND_MAX )
      {
        work->randLen = MB_CAP_TARGET_RAND_MAX;
      }
      else
      {
        work->randLen -= MB_CAP_TARGET_RAND_DOWN;
      }
      
      work->befPullLen = pullLen;
      work->befRotAngle = rotAngle;
    }
    if( work->randLen > 0 )
    {
      //�����_���덷
      const u16 len = GFUser_GetPublicRand0(work->randLen);
      const u16 rot = GFUser_GetPublicRand0(0x10000);
      const fx32 ofsX = FX_SinIdx( rot ) * len / 10;
      const fx32 ofsY = FX_CosIdx( rot ) * len / 10;
      ofs.x += ofsX;
      ofs.y += ofsY;
      
    }
    */
    VEC_Subtract( &pos , &ofs , &work->targetPos );
    
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->objIdxTarget , &flg );
    GFL_BBD_SetObjectTrans( work->bbdSys , work->objIdxTarget , &work->targetPos );
    work->isShotBall = FALSE;
  }
  else
  if( downState == MCDS_SHOT_WAIT )
  {
    if( work->isShotBall == FALSE )
    {
      u8 i;
      for( i=0;i<MB_CAP_BALL_NUM;i++ )
      {
        if( work->ballWork[i] == NULL )
        {
          MB_CAP_BALL_INIT_WORK initWork;
          
          initWork.isBonus = MB_CAP_DOWN_GetIsBonusBall( work->downWork );
          initWork.targetPos = work->targetPos;
          initWork.rotAngle = MB_CAP_DOWN_GetShotAngle( work->downWork );
          work->ballWork[i] = MB_CAP_BALL_CreateObject( work , &initWork );
          break;
        }
      }
      work->isShotBall = TRUE;
    }
  }
  else
  {
    const BOOL flg = FALSE;
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->objIdxTarget , &flg );
  }

  //�A�j���̍X�V
  {
    work->targetAnmCnt++;
    if( work->targetAnmCnt >= MB_CAP_TARGET_ANM_SPEED )
    {
      work->targetAnmCnt = 0;
      work->targetAnmFrame++;
      if( work->targetAnmFrame >= MB_CAP_TARGET_ANM_FRAME )
      {
        work->targetAnmFrame = 0;
      }
      GFL_BBD_SetObjectCelIdx( work->bbdSys , work->objIdxTarget , &work->targetAnmFrame );
    }
  }

  for( i=0;i<MB_CAP_BALL_NUM;i++ )
  {
    if( work->ballWork[i] != NULL )
    {
      MB_CAP_BALL_UpdateObject( work , work->ballWork[i] );
      if( MB_CAP_BALL_IsFinish( work->ballWork[i] ) == TRUE )
      {
        MB_CAP_BALL_DeleteObject( work , work->ballWork[i] );
        work->ballWork[i] = NULL;
      }
      else
      {
        if( MB_CAP_BALL_GetState( work->ballWork[i] ) == MCBS_FLYING )
        {
          activeBallNum++;
        }
      }
    }
  }
  for( i=0;i<MB_CAP_EFFECT_NUM;i++ )
  {
    if( work->effWork[i] != NULL )
    {
      MB_CAP_EFFECT_UpdateObject( work , work->effWork[i] );
      if( MB_CAP_EFFECT_IsFinish( work->effWork[i] ) == TRUE )
      {
        MB_CAP_EFFECT_DeleteObject( work , work->effWork[i] );
        work->effWork[i] = NULL;
      }
    }
  }
  
  //�|�P���m�̃q�b�g�`�F�b�N
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    const MB_CAP_POKE_STATE pokeState1 = MB_CAP_POKE_GetState( work->pokeWork[i] );
    if( pokeState1 == MCPS_RUN_LOOK )
    {
      for( j=i+1;j<MB_CAP_POKE_NUM;j++ )
      {
        const MB_CAP_POKE_STATE pokeState2 = MB_CAP_POKE_GetState( work->pokeWork[j] );
        if( pokeState2 == MCPS_RUN_LOOK )
        {
          MB_CAP_HIT_WORK pokeHit1,pokeHit2;
          MB_CAP_POKE_GetHitWork( work->pokeWork[i] , &pokeHit1 );
          MB_CAP_POKE_GetHitWork( work->pokeWork[j] , &pokeHit2 );
          if( MB_CAPTURE_CheckHit( &pokeHit1,&pokeHit2 ) == TRUE )
          {
            if( MB_CAP_POKE_GetBefHitPoke( work->pokeWork[i] ) != j &&
                MB_CAP_POKE_GetBefHitPoke( work->pokeWork[j] ) != i )
            {
              if( MB_CAP_POKE_GetPokeDir( work->pokeWork[i] ) == MB_CAP_POKE_GetPokeDir( work->pokeWork[j] ) )
              {
                MB_TPrintf("FLIP DIR!!!\n");
                MB_CAP_POKE_FlipDir( work->pokeWork[j] );
              }
              MB_CAP_POKE_SetBefHitPoke( work->pokeWork[i] , j );
              MB_CAP_POKE_SetBefHitPoke( work->pokeWork[j] , i );
              MB_CAP_POKE_SetDown( work , work->pokeWork[i] );
              MB_CAP_POKE_SetDown( work , work->pokeWork[j] );
              PMSND_PlaySE( MB_SND_POKE_DOWN );
            }
            else
            {
              MB_TPrintf("CHAIN HIT!!!\n");
            }
          }
        }
      }
    }
  }
  
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    MB_CAP_POKE_UpdateObject( work , work->pokeWork[i] );
  }
  
  //�e�̕�[(�{�[�i�X���[�hOFF
  //if( (activeBallNum == 0 || work->bonusTime > 0 )&&
  if( activeBallNum == 0 &&
      downState == MCDS_SHOT_WAIT &&
      work->isShotBall == TRUE )
  {
    //MB_CAP_DOWN_ReloadBall( work->downWork , MB_CAPTURE_IsBonusTime(work) );
    MB_CAP_DOWN_ReloadBall( work->downWork , FALSE );
    work->isShotBall = FALSE;
    work->randLen = MB_CAP_TARGET_RAND_MAX;
  }
}

#pragma mark [>time func
//--------------------------------------------------------------
//  ���ԏ�����
//--------------------------------------------------------------
static void MB_CAPTURE_InitTime( MB_CAPTURE_WORK *work )
{
  u8 i;
  u16 transScr[MB_CAP_TIME_GAUGE_WIDTH_CHARA];
  
  work->bonusTime = 0;
  work->gameTime = MB_CAP_GAMETIME;
  work->isCreateStar = FALSE;
  work->isUpdateStar = FALSE;

  for( i=0;i<MB_CAP_TIME_GAUGE_WIDTH_CHARA;i++ )
  {
    transScr[i] = MB_CAP_TIME_GAUGE_CHARA_G;
  }
  GFL_BG_WriteScreen( MB_CAPTURE_FRAME_FRAME , 
                      &transScr ,
                      MB_CAP_TIME_GAUGE_POS_X ,
                      MB_CAP_TIME_GAUGE_POS_Y ,
                      MB_CAP_TIME_GAUGE_WIDTH_CHARA ,
                      1 );
}

//--------------------------------------------------------------
//  ���ԍX�V
//--------------------------------------------------------------
static void MB_CAPTURE_UpdateTime( MB_CAPTURE_WORK *work )
{
  u16 transScr[MB_CAP_TIME_GAUGE_WIDTH_CHARA];
  const u16 befBonusTime = work->bonusTime;
  const u16 befGameTime = work->gameTime;
  
#if MB_CAP_DEB
  if( work->isTimeFreeze == TRUE )
  {
    work->gameTime++;
  }
#endif

  if( work->gameTime > 0 )
  {
    work->gameTime--;
  }
  
  if( work->bonusTime > 0 )
  {
    work->bonusTime--;
    
    {
      /*
      //Fog�h�炵(���s���ꂢ�Ɍ����Ȃ��̂ŕۗ�
      const u16 rad = (work->bonusTime*0x200)%0x10000;
      const fx16 cos = FX_CosIdx(rad);
      const u8 alphaTarget = FX_FX32_TO_F32(cos*(MB_CAP_BONUS_FOG_ALPHA_MAX-MB_CAP_BONUS_FOG_ALPHA_MIN))+MB_CAP_BONUS_FOG_ALPHA_MIN;
      */
      const u8 alphaTarget = MB_CAP_BONUS_FOG_ALPHA_MAX;
      if( work->fogAlpha < alphaTarget )
      {
        work->fogAlpha++;
        G3X_SetFogColor( MB_CAP_BONUS_FOG_COLOR , work->fogAlpha );
      }
      else
      if( work->fogAlpha > alphaTarget )
      {
        work->fogAlpha--;
        G3X_SetFogColor( MB_CAP_BONUS_FOG_COLOR , work->fogAlpha );
      }
    }
  }
  else
  {
    if( work->fogAlpha > 0 )
    {
      work->fogAlpha--;
      G3X_SetFogColor( MB_CAP_BONUS_FOG_COLOR , work->fogAlpha );
      if( work->fogAlpha == 0 )
      {
        BOOL flg = TRUE;
        G3X_SetFog( FALSE , GX_FOGBLEND_COLOR_ALPHA , GX_FOGSLOPE_0x0800 , 0 );
        GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[0] , &flg );
        GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[1] , &flg );
      }
    }
  
  }
  if( work->bonusTime == 0 && befBonusTime != 0 )
  {
    //�{�[�i�X�^�C���I������
    if( work->gameTime <= MB_CAP_RED_TIME )
    {
      PMSND_SetStatusBGM( MB_CAP_HURRY_BGM_TEMPO , MB_CAP_HURRY_BGM_PITCH , PMSND_NOEFFECT );
    }
    else
    {
      PMSND_SetStatusBGM( 256 , 0 , PMSND_NOEFFECT );
    }
  }
  if( befGameTime > MB_CAP_RED_TIME &&
      work->gameTime <= MB_CAP_RED_TIME &&
      work->bonusTime == 0 )
  {
    PMSND_SetStatusBGM( MB_CAP_HURRY_BGM_TEMPO , MB_CAP_HURRY_BGM_PITCH , PMSND_NOEFFECT );
  }
  
  //�]���f�[�^�̍X�V
  {
    u8 i;
    u16 baseChara;
    u16 len = work->gameTime * MB_CAP_TIME_GAUGE_WIDTH / MB_CAP_GAMETIME;
    len += 1; //�␳
    if( work->gameTime <= MB_CAP_RED_TIME )
    {
      baseChara = MB_CAP_TIME_GAUGE_CHARA_R;
    }
    else
    if( work->gameTime <= MB_CAP_YELLOW_TIME )
    {
      baseChara = MB_CAP_TIME_GAUGE_CHARA_Y;
    }
    else
    {
      baseChara = MB_CAP_TIME_GAUGE_CHARA_G;
    }
    
    for( i=0;i<MB_CAP_TIME_GAUGE_WIDTH_CHARA;i++ )
    {
      if( len >= 8 )
      {
        transScr[i] = baseChara;
        len -= 8;
      }
      else
      {
        transScr[i] = baseChara + (8-len);
        len = 0;
      }
    }
  }

  //�G�̍X�V
  GFL_BG_WriteScreen( MB_CAPTURE_FRAME_FRAME , 
                      &transScr ,
                      MB_CAP_TIME_GAUGE_POS_X ,
                      MB_CAP_TIME_GAUGE_POS_Y ,
                      MB_CAP_TIME_GAUGE_WIDTH_CHARA ,
                      1 );
  GFL_BG_LoadScreenReq( MB_CAPTURE_FRAME_FRAME );
  
  //�{�[�i�X
  if( work->isCreateStar == FALSE &&
      work->gameTime <= MB_CAP_YELLOW_TIME )
  {
    work->isCreateStar = TRUE;
    work->isUpdateStar = TRUE;
    MB_CAP_OBJ_SetEnable( work , work->starWork , TRUE );
  }
  if( work->isUpdateStar == TRUE )
  {
    MB_CAP_OBJ_UpdateObject_Star( work , work->starWork );
  }
}

//--------------------------------------------------------------
//  �X�^�[�Ɠ�����������
//--------------------------------------------------------------
void MB_CAPTURE_HitStarFunc( MB_CAPTURE_WORK *work , MB_CAP_OBJ *starWork )
{
  u8 i;
  work->bonusTime = MB_CAP_BONUSTIME;
  MB_CAP_OBJ_SetEnable( work , starWork , FALSE );
  work->isUpdateStar = FALSE;
  PMSND_SetStatusBGM( MB_CAP_BONUS_BGM_TEMPO , MB_CAP_BONUS_BGM_PITCH , PMSND_NOEFFECT );

  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    const MB_CAP_POKE_STATE pokeState = MB_CAP_POKE_GetState( work->pokeWork[i] );
    if( pokeState == MCPS_HIDE ||
        pokeState == MCPS_LOOK ||
        pokeState == MCPS_RUN_HIDE ||
        pokeState == MCPS_RUN_LOOK )
    {
      MB_CAP_POKE_SetSleep( work , work->pokeWork[i] );
    }
    else
    if( pokeState == MCPS_DOWN_MOVE ||
        pokeState == MCPS_DOWN_WAIT )
    {
      MB_CAP_POKE_SetDownToSleep( work , work->pokeWork[i] );
    }
  }
  {
    //static const u32 fogTable[8] = {0x40404040,0x40404040,0x40404040,0x40404040,0x40404040,0x40404040,0x40404040,0x40404040};
    static const u32 fogTable[8] = {0,0,0,0,0x40404040,0x40404040,0x40404040,0x40404040};
    G3X_SetFog( TRUE , GX_FOGBLEND_COLOR_ALPHA , GX_FOGSLOPE_0x8000 , 0 );
    G3X_SetFogTable( fogTable );
  }
  
  {
    BOOL flg = FALSE;
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[0] , &flg );
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->bbdObjBgMask[1] , &flg );
  }
//  PMSND_SetStatusBGM( 256 , 0 , PMSND_NOEFFECT );
//  PMSND_PlayBGM( SEQ_BGM_PALPARK_BONUS );
}


#pragma mark [>proc func
static GFL_PROC_RESULT MB_CAPTURE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CAPTURE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CAPTURE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBootCapture_ProcData =
{
  MB_CAPTURE_ProcInit,
  MB_CAPTURE_ProcMain,
  MB_CAPTURE_ProcTerm
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CAPTURE_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CAPTURE_WORK *work;
  MB_CAPTURE_INIT_WORK *initWork;
  
  if( pwk == NULL )
  {
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    const HEAPID parentHeap = GFL_HEAPID_APP;
#else                    //DL�q�@������
    const HEAPID parentHeap = HEAPID_MULTIBOOT;
#endif //MULTI_BOOT_MAKE
    u8 i;
    GFL_HEAP_CreateHeap( parentHeap , HEAPID_MB_BOX, 0x60000 );
    
    work = GFL_PROC_AllocWork( proc, sizeof(MB_CAPTURE_WORK), parentHeap );
    initWork = GFL_HEAP_AllocClearMemory( HEAPID_MB_BOX , sizeof(MB_CAPTURE_INIT_WORK) );

    initWork->parentHeap = parentHeap;
    initWork->cardType = CARD_TYPE_DUMMY;
    for( i=0;i<MB_CAP_POKE_NUM;i++ )
    {
      initWork->ppp[i] = GFL_HEAP_AllocClearMemory( parentHeap , POKETOOL_GetPPPWorkSize() );
      PPP_Setup( initWork->ppp[i] , 
                 GFUser_GetPublicRand0(493)+1 ,
                 GFUser_GetPublicRand0(100)+1 ,
                 PTL_SETUP_ID_AUTO );
    }
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    initWork->arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_CAPTER , parentHeap );
    PMSND_Exit();
#endif //MULTI_BOOT_MAKE
    initWork->highScore = 600;
    work->initWork = initWork;
  }
  else
  {
    initWork = pwk;
    GFL_HEAP_CreateHeap( initWork->parentHeap , HEAPID_MB_BOX, 0x60000 );
    work = GFL_PROC_AllocWork( proc, sizeof(MB_CAPTURE_WORK), HEAPID_MULTIBOOT );
    work->initWork = pwk;
  }
  

  work->heapId = HEAPID_MB_BOX;
  
  MB_CAPTURE_Init( work );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CAPTURE_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CAPTURE_WORK *work = mywk;
  
  MB_CAPTURE_Term( work );

  if( pwk == NULL )
  {
    u8 i;
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
    GFL_ARC_CloseDataHandle( work->initWork->arcHandle );
    PMSND_Init();
#endif
    for( i=0;i<MB_CAP_POKE_NUM;i++ )
    {
      GFL_HEAP_FreeMemory( work->initWork->ppp[i] );
    }
    GFL_HEAP_FreeMemory( work->initWork );
  }

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MB_BOX );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ���[�v
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CAPTURE_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CAPTURE_WORK *work = mywk;
  const BOOL ret = MB_CAPTURE_Main( work );
  
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isNetErr == FALSE )
  {
    work->isNetErr = TRUE;
    work->state = MSS_FADEOUT;
    OS_TPrintf("�G���[�����I\n");
  }
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}



#pragma mark [>outer func
//--------------------------------------------------------------
//  �O���񋟊֐��Q
//--------------------------------------------------------------
const HEAPID MB_CAPTURE_GetHeapId( const MB_CAPTURE_WORK *work )
{
  return work->heapId;
}
GFL_BBD_SYS* MB_CAPTURE_GetBbdSys( MB_CAPTURE_WORK *work )
{
  return work->bbdSys;
}
ARCHANDLE* MB_CAPTURE_GetArcHandle( MB_CAPTURE_WORK *work )
{
  return work->initWork->arcHandle;
}
ARCHANDLE* MB_CAPTURE_GetPokeArcHandle( MB_CAPTURE_WORK *work )
{
  return MB_ICON_GetArcHandle(GFL_HEAP_LOWID(work->heapId),work->initWork->cardType);
}
const DLPLAY_CARD_TYPE MB_CAPTURE_GetCardType( const MB_CAPTURE_WORK *work )
{
  return work->initWork->cardType;
}
const int MB_CAPTURE_GetBbdResIdx( const MB_CAPTURE_WORK *work , const MB_CAP_BBD_RES resType )
{
  return work->bbdRes[resType];
}
MB_CAP_OBJ* MB_CAPTURE_GetObjWork( MB_CAPTURE_WORK *work , const u8 idx )
{
  if( idx >= MB_CAP_OBJ_NUM )
  {
    return NULL;
  }
  return work->objWork[idx];
}
MB_CAP_OBJ* MB_CAPTURE_GetStarWork( MB_CAPTURE_WORK *work )
{
  return work->starWork;
}

MB_CAP_POKE* MB_CAPTURE_GetPokeWork( MB_CAPTURE_WORK *work , const u8 idx )
{
  return work->pokeWork[idx];
}
const BOOL MB_CAPTURE_IsBonusTime( MB_CAPTURE_WORK *work )
{
  if( work->bonusTime > 0 )
  {
    return TRUE;
  }
  return FALSE;
}
const u16 MB_CAPTURE_GetScore( MB_CAPTURE_WORK *work )
{
  return work->score;
}
void MB_CAPTURE_AddScore( MB_CAPTURE_WORK *work , const u16 addScore , const BOOL isAddTime )
{
  work->score += addScore;
  if( isAddTime == TRUE )
  {
    work->score += work->gameTime/(2*60);
  }
}
//--------------------------------------------------------------
//  �|�P�����ߊl����
//--------------------------------------------------------------
void MB_CAPTURE_GetPokeFunc( MB_CAPTURE_WORK *work , MB_CAP_BALL *ballWork , const u8 pokeIdx )
{
  MB_CAP_POKE *pokeWork = work->pokeWork[pokeIdx];
  
  MB_CAP_POKE_SetCapture( work , pokeWork );
  MB_CAP_DOWN_GetPoke( work , work->downWork , pokeIdx );
}

//--------------------------------------------------------------
//  �G�t�F�N�g�����
//--------------------------------------------------------------
MB_CAP_EFFECT* MB_CAPTURE_CreateEffect( MB_CAPTURE_WORK *work , VecFx32 *pos , const MB_CAP_EFFECT_TYPE type )
{
  u8 i;
  for( i=0;i<MB_CAP_EFFECT_NUM;i++ )
  {
    if( work->effWork[i] == NULL )
    {
      MB_CAP_EFFECT_INIT_WORK initWork;
      
      initWork.pos = *pos;
      initWork.type = type;
      work->effWork[i] = MB_CAP_EFFECT_CreateObject( work , &initWork );
      return work->effWork[i];
      break;
    }
  }
  return NULL;
}
POKEMON_PASO_PARAM* MB_CAPTURE_GetPPP( MB_CAPTURE_WORK *work , const u8 idx )
{
  return work->initWork->ppp[idx];
}

#pragma mark [>outer func
//--------------------------------------------------------------
//  �֗��֐�
//--------------------------------------------------------------
void MB_CAPTURE_GetGrassObjectPos( const s8 idxX , const s8 idxY , VecFx32 *ret )
{
  ret->x = FX32_CONST( idxX * MB_CAP_OBJ_MAIN_X_SPACE + MB_CAP_OBJ_MAIN_LEFT);
  ret->y = FX32_CONST( idxY * MB_CAP_OBJ_MAIN_Y_SPACE + MB_CAP_OBJ_MAIN_TOP);
  ret->z = FX32_CONST( MB_CAP_OBJ_BASE_Z + (idxY)*MB_CAP_OBJ_LINEOFS_Z );
}

//--------------------------------------------------------------
//  �����蔻��̃`�F�b�N
//--------------------------------------------------------------
const BOOL MB_CAPTURE_CheckHit( const MB_CAP_HIT_WORK *work1 , MB_CAP_HIT_WORK *work2 )
{
  if((work1->pos->x - work1->size.x < work2->pos->x + work2->size.x) &&
     (work1->pos->x + work1->size.x > work2->pos->x - work2->size.x) &&
     (work1->pos->y - work1->size.y < work2->pos->y + work2->size.y) &&
     (work1->pos->y + work1->size.y > work2->pos->y - work2->size.y) &&
     (work1->height - work1->size.z < work2->height + work2->size.z) &&
     (work1->height + work1->size.z > work2->height - work2->size.z) )
  {
//    OS_Printf("A[%6.2f][%6.2f][%6.2f]\n",F32_CONST(work1->pos->x),F32_CONST(work1->pos->y),F32_CONST(work1->height));
//    OS_Printf(" [%6.2f][%6.2f][%6.2f]\n",F32_CONST(work1->size.x),F32_CONST(work1->size.y),F32_CONST(work1->size.z));
//    OS_Printf("B[%6.2f][%6.2f][%6.2f]\n",F32_CONST(work2->pos->x),F32_CONST(work2->pos->y),F32_CONST(work2->height));
//    OS_Printf(" [%6.2f][%6.2f][%6.2f]\n",F32_CONST(work2->size.x),F32_CONST(work2->size.y),F32_CONST(work2->size.z));
    return TRUE;
  }
  return FALSE;
}

MB_CAP_DOWN* MB_CAPTURE_GetDownWork( MB_CAPTURE_WORK *work )
{
  return work->downWork;
}

GFL_FONT* MB_CAPTURE_GetFontHandle( MB_CAPTURE_WORK *work )
{
  return work->fontHandle;
}
GFL_MSGDATA* MB_CAPTURE_GetMsgHandle( MB_CAPTURE_WORK *work )
{
  return work->msgHandle;
}
PRINT_QUE* MB_CAPTURE_GetPrintQue( MB_CAPTURE_WORK *work )
{
  return work->printQue;
}


#pragma mark [>debug func
#if MB_CAP_DEB

#include "debug/debugwin_sys.h"
#include "font/font.naix"

#define MB_CAP_DEBUG_GROUP_TOP  (50)
#define MB_CAP_DEBUG_GROUP_BALL (51)
#define MB_CAP_DEBUG_GROUP_POKE (52)
#define MB_CAP_DEBUG_GROUP_BGM  (53)

int MB_CAP_UPPER_BALL_POS_BASE_Y = 256;
fx32 MB_CAP_BALL_SHOT_SPEED = FX32_CONST(4.8f);

u16 MB_CAP_TARGET_RAND_MAX = 320;
u16 MB_CAP_TARGET_RAND_DOWN = 10;


int MB_CAP_POKE_HIDE_LOOK_TIME = (120);
int MB_CAP_POKE_HIDE_HIDE_TIME = (60);
int MB_CAP_POKE_RUN_LOOK_TIME = (80);
int MB_CAP_POKE_RUN_HIDE_TIME = (40);
int MB_CAP_POKE_DOWN_TIME = (60*3);

int BgmTemp = 256;
int BgmPitch = 0;

static const BOOL MB_CAPTURE_Debug_UpdateValue_u16( u16 *val );
static const BOOL MB_CAPTURE_Debug_UpdateValue_fx32( fx32 *val );
static const BOOL MB_CAPTURE_Debug_UpdateValue_int( int *val );
static void MCD_U_GameTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_GameTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_GameTimeFreeze( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_GameTimeFreeze( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BonusTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BonusTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_Score( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_Score( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_ResetPoke( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BallBaseY( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BallBaseY( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BallSpd( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BallSpd( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BallRandWidth( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BallRandWidth( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BallRandDown( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BallRandDown( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_PokeHideTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_PokeHideTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_PokeLookTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_PokeLookTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_PokeRunHideTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_PokeRunHideTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_PokeRunLookTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_PokeRunLookTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_PokeDownTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_PokeDownTime( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BgmTempo( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BgmTempo( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_U_BgmPitch( void* userWork , DEBUGWIN_ITEM* item );
static void MCD_D_BgmPitch( void* userWork , DEBUGWIN_ITEM* item );

static void MB_CAPTURE_InitDebug( MB_CAPTURE_WORK *work )
{

  DEBUGWIN_InitProc( MB_CAPTURE_FRAME_DEBUG , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 0,0,0 );
  
  DEBUGWIN_AddGroupToTop( MB_CAP_DEBUG_GROUP_TOP , "�p��" , work->heapId );
  DEBUGWIN_AddGroupToGroup( MB_CAP_DEBUG_GROUP_BALL , "BALL" , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddGroupToGroup( MB_CAP_DEBUG_GROUP_POKE , "POKE" , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddGroupToGroup( MB_CAP_DEBUG_GROUP_BGM  , "BGM" , MB_CAP_DEBUG_GROUP_TOP , work->heapId );

  DEBUGWIN_AddItemToGroupEx( MCD_U_GameTime ,MCD_D_GameTime , (void*)work , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_GameTimeFreeze ,MCD_D_GameTimeFreeze , (void*)work , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_BonusTime ,MCD_D_BonusTime , (void*)work , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_Score ,MCD_D_Score , (void*)work , MB_CAP_DEBUG_GROUP_TOP , work->heapId );
  DEBUGWIN_AddItemToGroup( "ResetPoke" , MCD_U_ResetPoke , (void*)work , MB_CAP_DEBUG_GROUP_TOP , work->heapId );

  DEBUGWIN_AddItemToGroupEx( MCD_U_BallBaseY ,MCD_D_BallBaseY , (void*)work , MB_CAP_DEBUG_GROUP_BALL , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_BallSpd   ,MCD_D_BallSpd   , (void*)work , MB_CAP_DEBUG_GROUP_BALL , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_BallRandWidth ,MCD_D_BallRandWidth , (void*)work , MB_CAP_DEBUG_GROUP_BALL , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_BallRandDown   ,MCD_D_BallRandDown   , (void*)work , MB_CAP_DEBUG_GROUP_BALL , work->heapId );

  DEBUGWIN_AddItemToGroupEx( MCD_U_PokeHideTime   ,MCD_D_PokeHideTime   , (void*)work , MB_CAP_DEBUG_GROUP_POKE , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_PokeLookTime   ,MCD_D_PokeLookTime   , (void*)work , MB_CAP_DEBUG_GROUP_POKE , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_PokeRunHideTime   ,MCD_D_PokeRunHideTime   , (void*)work , MB_CAP_DEBUG_GROUP_POKE , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_PokeRunLookTime   ,MCD_D_PokeRunLookTime   , (void*)work , MB_CAP_DEBUG_GROUP_POKE , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_PokeDownTime   ,MCD_D_PokeDownTime   , (void*)work , MB_CAP_DEBUG_GROUP_POKE , work->heapId );

  DEBUGWIN_AddItemToGroupEx( MCD_U_BgmTempo   ,MCD_D_BgmTempo   , (void*)work , MB_CAP_DEBUG_GROUP_BGM , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MCD_U_BgmPitch   ,MCD_D_BgmPitch   , (void*)work , MB_CAP_DEBUG_GROUP_BGM , work->heapId );
}

static void MB_CAPTURE_TermDebug( MB_CAPTURE_WORK *work )
{
  DEBUGWIN_RemoveGroup( MB_CAP_DEBUG_GROUP_TOP );
  DEBUGWIN_ExitProc();

}


static const BOOL MB_CAPTURE_Debug_UpdateValue_u16( u16 *val )
{
  u16 value;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){ value = 10; }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){ value = 100; }
  else{ value = 1; }

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    *val += value;
    return TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    *val -= value;
    return TRUE;
  }
  return FALSE;
}
static const BOOL MB_CAPTURE_Debug_UpdateValue_int( int *val )
{
  int value;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){ value = 10; }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){ value = 100; }
  else{ value = 1; }

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    *val += value;
    return TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    *val -= value;
    return TRUE;
  }
  return FALSE;
}
static const BOOL MB_CAPTURE_Debug_UpdateValue_fx32( fx32 *val )
{
  fx32 value;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){value = FX32_ONE;}
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){ value = FX32_ONE*100; }
  else{value = FX32_CONST(0.1f);}

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    *val += value;
    return TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    *val -= value;
    return TRUE;
  }
  return FALSE;
}

static void MCD_U_GameTime( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_u16( &work->gameTime );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    work->gameTime = 60*60;
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_GameTime( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;

  DEBUGWIN_ITEM_SetNameV( item , "GameTime[%d]",work->gameTime );
}

static void MCD_U_GameTimeFreeze( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT|PAD_KEY_RIGHT) )
  {
    work->isTimeFreeze = !work->isTimeFreeze;
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_GameTimeFreeze( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  if( work->isTimeFreeze == TRUE )
  {
    DEBUGWIN_ITEM_SetName( item , "TimeStop[ON]");
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "TimeStop[OFF]");
  }
}

static void MCD_U_BonusTime( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  const u16 befTime = work->bonusTime;
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_u16( &work->bonusTime );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BonusTime( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;

  DEBUGWIN_ITEM_SetNameV( item , "BonusTime[%d]",work->bonusTime );
}

static void MCD_U_Score( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_u16( &work->score );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_Score( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;

  DEBUGWIN_ITEM_SetNameV( item , "Score[%d]",work->score );
}

static void MCD_U_ResetPoke( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  u8 i,j;
  //�o���ݒ�
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    for( i=0;i<MB_CAP_POKE_NUM;i++ )
    {
      BOOL isLoop = TRUE;
      while( isLoop == TRUE )
      {
        const u8 idxX = GFUser_GetPublicRand0( MB_CAP_OBJ_X_NUM );
        const u8 idxY = GFUser_GetPublicRand0( MB_CAP_OBJ_Y_NUM );
        isLoop = FALSE;
        for( j=0;j<i;j++ )
        {
          if( MB_CAP_POKE_CheckPos( work->pokeWork[j] , idxX , idxY ) == TRUE )
          {
            isLoop = TRUE;
            break;
          }
        }
        if( isLoop == FALSE )
        {
          MB_CAP_POKE_SetHide( work , work->pokeWork[j] , idxX , idxY );
          MB_TPrintf("[%d][%d]\n",idxX,idxY);
        }
      }
    }  
  }
}

static void MCD_U_BallBaseY( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_UPPER_BALL_POS_BASE_Y );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BallBaseY( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;

  DEBUGWIN_ITEM_SetNameV( item , "posY[%d]",MB_CAP_UPPER_BALL_POS_BASE_Y );
}


static void MCD_U_BallSpd( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_fx32( &MB_CAP_BALL_SHOT_SPEED );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BallSpd( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  const int upVal= (int)F32_CONST(MB_CAP_BALL_SHOT_SPEED);
  const int downVal= ((int)F32_CONST(MB_CAP_BALL_SHOT_SPEED*10))%10;
  
  DEBUGWIN_ITEM_SetNameV( item , "speed[%d.%d]",upVal,downVal );
  
}
static void MCD_U_BallRandWidth( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_u16( &MB_CAP_TARGET_RAND_MAX );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BallRandWidth( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "rand max[%d.%d]",MB_CAP_TARGET_RAND_MAX/10 , MB_CAP_TARGET_RAND_MAX%10 );
}

static void MCD_U_BallRandDown( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_u16( &MB_CAP_TARGET_RAND_DOWN );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BallRandDown( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_CAPTURE_WORK *work = (MB_CAPTURE_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "rand down[%d]",MB_CAP_TARGET_RAND_DOWN );
}

static void MCD_U_PokeHideTime( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_POKE_HIDE_HIDE_TIME );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_PokeHideTime( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Stay HideTime[%d]",MB_CAP_POKE_HIDE_HIDE_TIME );
}

static void MCD_U_PokeLookTime( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_POKE_HIDE_LOOK_TIME );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_PokeLookTime( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Stay LookTime[%d]",MB_CAP_POKE_HIDE_LOOK_TIME );
}

static void MCD_U_PokeRunHideTime( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_POKE_RUN_HIDE_TIME );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_PokeRunHideTime( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Move HideTime[%d]",MB_CAP_POKE_RUN_HIDE_TIME );
}

static void MCD_U_PokeRunLookTime( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_POKE_RUN_LOOK_TIME );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_PokeRunLookTime( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Move LookTime[%d]",MB_CAP_POKE_RUN_LOOK_TIME );
}

static void MCD_U_PokeDownTime( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &MB_CAP_POKE_DOWN_TIME );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_PokeDownTime( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "DownTime[%d]",MB_CAP_POKE_DOWN_TIME );
}

static void MCD_U_BgmTempo( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &BgmTemp );
  if( ret == TRUE )
  {
    PMSND_SetStatusBGM( BgmTemp , BgmPitch , PMSND_NOEFFECT );
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BgmTempo( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Tempo[%d]",BgmTemp );
}

static void MCD_U_BgmPitch( void* userWork , DEBUGWIN_ITEM* item )
{
  const BOOL ret = MB_CAPTURE_Debug_UpdateValue_int( &BgmPitch );
  if( ret == TRUE )
  {
    PMSND_SetStatusBGM( BgmTemp , BgmPitch , PMSND_NOEFFECT );
    DEBUGWIN_RefreshScreen();
  }
}

static void MCD_D_BgmPitch( void* userWork , DEBUGWIN_ITEM* item )
{
  DEBUGWIN_ITEM_SetNameV( item , "Pitch[%d]",BgmPitch );
}


#endif  //MB_CAP_DEB
