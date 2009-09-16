//======================================================================
/**
 * @file  dressup_system.h
 * @brief �X�e�[�W ���Z���C��
 * @author  ariizumi
 * @data  09/03/02
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "font/font.naix"

#include "arc_def.h"
#include "musical_item.naix"
#include "musical_script.naix"
#include "stage_gra.naix"
#include "message.naix"
#include "print/printsys.h"

#include "sound/snd_strm.h"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"
#include "musical/musical_camera_def.h"

#include "musical/musical_stage_sys.h"
#include "sta_local_def.h"
#include "script/sta_act_script_def.h"
#include "sta_act_bg.h"
#include "sta_act_button.h"
#include "sta_act_audience.h"

#include "eff_def/mus_eff.h"

#include "debug/debugwin_sys.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define


#define ACT_PAL_INFO    (0xE)
#define ACT_PAL_FONT    (0xF)

#define ACT_OBJECT_IVALID (-1)

#define ACT_BG_SCROLL_MAX (256)

#define ACT_SCROLL_SPEED (4)

//�ꔭ�t�]�{�^���������D����(���o���ԂƂ͓������Ȃ�)
#define ACT_USEITEM_EFF_TIME (3*60)

//Msg�n
#define ACT_MSG_POS_X ( 4 )
#define ACT_MSG_POS_Y ( 20 )
#define ACT_MSG_POS_WIDTH  ( 24 )
#define ACT_MSG_POS_HEIGHT ( 4 )

//�X�N���v�g�p
enum
{
  STA_SCRIPT_MAIN = 0,
  //�P�͖��g�p
  STA_SCRIPT_POKE_1 = 2,
  STA_SCRIPT_POKE_2 = 3,
  STA_SCRIPT_POKE_3 = 4,
  STA_SCRIPT_POKE_4 = 5,
};


//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  //�t�F�[�h
  AMS_FADEIN,
  AMS_WAIT_FADEIN,
  AMS_FADEOUT,
  AMS_WAIT_FADEOUT,

  //���C��
  AMS_ACTING_START,
  AMS_ACTING_MAIN,

  //�ʐM�p
  AMS_COMM_START_SYNC,
}ACTING_MAIN_SEQ;


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct

struct _ACTING_WORK
{
  HEAPID heapId;
  
  u16   scrollOffset;
  u16   scrollOffsetTrget;
  u16   makuOffset;
  u8    playerIdx;
  
  ACTING_MAIN_SEQ mainSeq;
  
  GFL_TCB   *vblankFuncTcb;

  STAGE_INIT_WORK *initWork;

  MUS_POKE_DRAW_SYSTEM  *drawSys;
  MUS_ITEM_DRAW_SYSTEM  *itemDrawSys;
  
  STA_BG_SYS      *bgSys;

  STA_POKE_SYS    *pokeSys;
  STA_POKE_WORK   *pokeWork[MUSICAL_POKE_MAX];
  
  STA_OBJ_SYS     *objSys;
  STA_OBJ_WORK    *objWork[ACT_OBJECT_MAX];

  STA_EFF_SYS     *effSys;
  STA_EFF_WORK    *effWork[ACT_EFFECT_MAX];
  
  STA_LIGHT_SYS   *lightSys;
  STA_LIGHT_WORK    *lightWork[ACT_LIGHT_MAX];

  STA_BUTTON_SYS   *buttonSys;

  GFL_G3D_CAMERA    *camera;
  GFL_BBD_SYS     *bbdSys;
  
  STA_SCRIPT_SYS    *scriptSys;
  void        *scriptData;
  
  STA_AUDI_SYS  *audiSys;
  
  STA_EFF_WORK   *transEffWork[MUSICAL_POKE_MAX];
  
  //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT      *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  STRBUF          *msgStr;
  
  //���ڃ|�P�p
  BOOL    isUpdateAttention;
  u8      lightUpPoke;
  u8      attentionPoke;  //�J�����̑Ώ�
  
  //�A�C�e���g�p�`
  BOOL    useItemFlg[MUSICAL_POKE_MAX];
  MUS_POKE_EQUIP_POS useItemPos[MUSICAL_POKE_MAX];
  u8      useItemPoke;  //�ꔭ�t�]���|�P
  u16     useItemCnt;
  
  //�ʐM���ɑ��郊�N�G�X�g
  BOOL    useItemReq;
  MUS_POKE_EQUIP_POS useItemReqPos;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
ACTING_WORK*  STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId);
void  STA_ACT_TermActing( ACTING_WORK *work );
ACTING_RETURN STA_ACT_LoopActing( ACTING_WORK *work );
static void STA_ACT_VBlankFunc(GFL_TCB *tcb,void *work);

static void STA_ACT_SetupGraphic( ACTING_WORK *work );
static void STA_ACT_SetupBg( ACTING_WORK *work );
static void STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane  , u8 mode );
static void STA_ACT_SetupPokemon( ACTING_WORK *work );
static void STA_ACT_SetupItem( ACTING_WORK *work );
static void STA_ACT_SetupEffect( ACTING_WORK *work );
static void STA_ACT_SetupMessage( ACTING_WORK *work );

static void STA_ACT_UpdateScroll( ACTING_WORK *work );
static void STA_ACT_UpdateMessage( ACTING_WORK *work );

static void STA_ACT_StartScript( ACTING_WORK *work );

static void STA_ACT_UpdateAttention( ACTING_WORK *work );

static void STA_ACT_UpdateUseItem( ACTING_WORK *work );

static void STA_ACT_InitTransEffect( ACTING_WORK *work );
static void STA_ACT_TermTransEffect( ACTING_WORK *work );
static void STA_ACT_InitItemUseEffect( ACTING_WORK *work );
static void STA_ACT_TermItemUseEffect( ACTING_WORK *work );


static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_D,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_23_G,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_32_H,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_16_F,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_012_ABC,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_0123_E,     // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};
//  A �e�N�X�`��
//  B �e�N�X�`��
//  C MainBg
//  D SubObj
//  E �e�N�X�`���p���b�g
//  F MainObj
//  G MainBgExPlt
//  H SubBG
//  I SubBG

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
ACTING_WORK*  STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId )
{
  u8 i;
  ACTING_WORK *work = GFL_HEAP_AllocClearMemory( heapId , sizeof( ACTING_WORK ));
  
  work->heapId = heapId;
  work->initWork = initWork;
  work->scrollOffset = 0;
  work->makuOffset = 0;
  work->scriptData = NULL;
  work->mainSeq = AMS_FADEIN;
  
  work->vblankFuncTcb = GFUser_VIntr_CreateTCB( STA_ACT_VBlankFunc , (void*)work , 64 );
  
  //���L�����̑I��
  if( work->initWork->commWork == NULL )
  {
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      if( work->initWork->musPoke[i]->charaType == MUS_CHARA_PLAYER )
      {
        work->playerIdx = i;
        break;
      }
    }
    if( MUSICAL_POKE_MAX == i )
    {
      GF_ASSERT_MSG( MUSICAL_POKE_MAX != i , "Musical stage:Player poke is not found!!!\n");
      work->playerIdx = 1;
    }
  }
  else
  {
    work->playerIdx = MUS_COMM_GetSelfMusicalIndex(work->initWork->commWork);
  }
  
  //���ڃ|�P�n������
  work->isUpdateAttention = TRUE;
  work->lightUpPoke = MUSICAL_POKE_MAX;
  work->attentionPoke = MUSICAL_POKE_MAX; //�J�����̓f�t�H���g
  //�A�C�e���g�p�n������
  work->useItemPoke = MUSICAL_POKE_MAX;
  work->useItemCnt = 0;
  work->useItemReq = FALSE;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->useItemFlg[i] = FALSE;
  }

  STA_ACT_SetupGraphic( work );
  STA_ACT_SetupBg( work );
  STA_ACT_SetupMessage( work);
  STA_ACT_SetupPokemon( work );
  STA_ACT_SetupItem( work );
  STA_ACT_SetupEffect( work );
  
  work->lightSys = STA_LIGHT_InitSystem(work->heapId , work );
  work->audiSys = STA_AUDI_InitSystem( work->heapId , work , work->initWork );
  work->buttonSys = STA_BUTTON_InitSystem( work->heapId , work , work->initWork->musPoke[work->playerIdx] );

  work->scriptSys = STA_SCRIPT_InitSystem( work->heapId , work );

  STA_ACT_InitTransEffect( work );
  
  INFOWIN_Init( ACT_FRAME_SUB_INFO,ACT_PAL_INFO,NULL,work->heapId);

#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_InitProc( ACT_FRAME_MAIN_CURTAIN , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 31,31,31 );
#endif  //USE_DEBUGWIN_SYSTEM
//  OS_TPrintf("FreeHeap:[%x]\n", GFL_HEAP_GetHeapFreeSize( work->heapId ) );
  return work;
}


void  STA_ACT_TermActing( ACTING_WORK *work )
{
#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_ExitProc();
#endif  //USE_DEBUGWIN_SYSTEM
  //�t�F�[�h�Ȃ��̂ŉ�����
  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE , GX_BLEND_PLANEMASK_NONE , 31 , 31 );
  
  INFOWIN_Exit();

  STA_ACT_TermTransEffect( work );
  
  STA_SCRIPT_ExitSystem( work->scriptSys );
  if( work->scriptData != NULL )
  {
    GFL_HEAP_FreeMemory( work->scriptData );
  }

  if( SND_STRM_CheckSetUp() == TRUE &&
      SND_STRM_CheckPlay() == TRUE )
  {
    STA_ACT_StopBgm( work );
  }

  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
  }
  GFL_MSG_Delete( work->msgHandle );
  GFL_BMPWIN_Delete( work->msgWin );
  GFL_FONT_Delete( work->fontHandle );
  GFL_TCBL_Exit( work->tcblSys );
  
  STA_BUTTON_ExitSystem( work->buttonSys );
  STA_AUDI_ExitSystem( work->audiSys );
  STA_BG_ExitSystem( work->bgSys );
  STA_LIGHT_ExitSystem( work->lightSys );
  STA_EFF_ExitSystem( work->effSys );
  STA_OBJ_ExitSystem( work->objSys );
  STA_POKE_ExitSystem( work->pokeSys );

  MUS_POKE_DRAW_TermSystem( work->drawSys );
  MUS_ITEM_DRAW_TermSystem( work->itemDrawSys );

  GFL_CLACT_SYS_Delete();

  GFL_BBD_DeleteSys( work->bbdSys );
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_3D );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_FONT );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_CURTAIN );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_BG );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_INFO );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GFL_TCB_DeleteTask( work->vblankFuncTcb );

  GFL_HEAP_FreeMemory( work );
  
}

ACTING_RETURN STA_ACT_LoopActing( ACTING_WORK *work )
{
  static BOOL stopScript = FALSE;
#if DEB_ARI|1
  if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      
      //������
      //STA_SCRIPT_SetScript( work->scriptSys , (void*)musicalScriptTestData );
      if( work->scriptData != NULL )
      {
        GFL_HEAP_FreeMemory( work->scriptData );
      }
      work->scriptData = GFL_ARC_UTIL_Load( ARCID_MUSICAL_SCRIPT , NARC_musical_script_we_001_bin , FALSE , work->heapId );
      STA_ACT_StartScript( work );
    }
  }
  else
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      stopScript = TRUE;
    }
  }
/*
  if( work->initWork->commWork != NULL )
  {
    static u32 isStartCnt = 0;
    static BOOL isWait = FALSE;
    MUS_COMM_WORK *commWork = work->initWork->commWork;
    //if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 &&
        isWait == FALSE )
    {
      MUS_COMM_SendTimingCommand( commWork , MUS_COMM_TIMMING_START_SCRIPT+isStartCnt );
      isWait = TRUE;
    }
    
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
    {
      static u32 vcount = 0;
      if( vcount != 0 )
      {
        u32 nowVCount = OS_GetVBlankCount();
        OS_TPrintf("Script count[%d]\n",nowVCount-vcount);
        vcount = 0;
      }
      if( MUS_COMM_CheckTimingCommand( commWork , MUS_COMM_TIMMING_START_SCRIPT+isStartCnt ) == TRUE )
      {
        if( work->scriptData != NULL )
        {
          GFL_HEAP_FreeMemory( work->scriptData );
        }
        work->scriptData = GFL_ARC_UTIL_Load( ARCID_MUSICAL_SCRIPT , NARC_musical_script_we_001_bin , FALSE , work->heapId );
        STA_ACT_StartScript( work );
        vcount = OS_GetVBlankCount(); 
        isStartCnt++;
        isWait = FALSE;
        OS_TPrintf("-------------------------------\n");
        OS_TPrintf("Start script[%d]\n",isStartCnt);
        OS_TPrintf("-------------------------------\n");
      }
    }
  }
*/

#endif

  switch( work->mainSeq )
  {
  case AMS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = AMS_WAIT_FADEIN;
    
    break;
    
  case AMS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( work->initWork->commWork != NULL )
      {
        work->mainSeq = AMS_COMM_START_SYNC;
        MUS_COMM_SendTimingCommand( work->initWork->commWork , MUS_COMM_TIMMING_START_SCRIPT );
      }
      else
      {
        work->mainSeq = AMS_ACTING_START;
      }
    }
    break;
    
  case AMS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = AMS_WAIT_FADEOUT;
    break;
    
  case AMS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return ACT_RET_GO_END;
    }
    break;
  
  case AMS_ACTING_START:
    work->scriptData = GFL_ARC_UTIL_Load( ARCID_MUSICAL_SCRIPT , NARC_musical_script_we_001_bin , FALSE , work->heapId );
    STA_ACT_StartScript( work );
    work->mainSeq = AMS_ACTING_MAIN;
    break;
  
  case AMS_ACTING_MAIN:
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
    {
      GFL_HEAP_FreeMemory( work->scriptData );
      work->scriptData = NULL;
      work->mainSeq = AMS_FADEOUT;
    }
    break;
    
  case AMS_COMM_START_SYNC:
    if( MUS_COMM_CheckTimingCommand( work->initWork->commWork , MUS_COMM_TIMMING_START_SCRIPT ) == TRUE )
    {
      work->mainSeq = AMS_ACTING_START;
    }
    break;
    
  }

  INFOWIN_Update();
  STA_ACT_UpdateScroll(work);

  STA_BUTTON_UpdateSystem( work->buttonSys );
  STA_ACT_UpdateUseItem( work );

  if( stopScript == FALSE )
  {
    STA_SCRIPT_UpdateSystem( work->scriptSys );
  }

  STA_BG_UpdateSystem( work->bgSys );
  STA_POKE_UpdateSystem( work->pokeSys );
  STA_OBJ_UpdateSystem( work->objSys );
  STA_EFF_UpdateSystem( work->effSys );
  STA_ACT_UpdateMessage( work );
  
  MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 

  //�|�P�Ƃ��ɒǏ]����\��������̂ōŌ�
  STA_LIGHT_UpdateSystem( work->lightSys );

  STA_ACT_UpdateAttention( work );
  STA_AUDI_UpdateSystem( work->audiSys );

  //3D�`��  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    STA_LIGHT_DrawSystem( work->lightSys );
    STA_POKE_DrawSystem( work->pokeSys );
    MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
//    STA_OBJ_DrawSystem( work->objSys );
    STA_POKE_UpdateSystem_Item( work->pokeSys );  //�|�P�̕`���ɓ���邱��
    STA_BG_DrawSystem( work->bgSys );
    GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
    STA_EFF_DrawSystem( work->effSys );

  }
  GFL_G3D_DRAW_End();
  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

#if DEB_ARI|0
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT &&
    GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
  {
    return ACT_RET_GO_END;
  }
#endif //DEB_ARI
#if DEB_ARI|1
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ , -8 );
    STA_BUTTON_SetCanUseButton( work->buttonSys , TRUE );
  }
#endif

  return ACT_RET_CONTINUE;
}

//------------------------------------------------------------------
//  VBLank Function
//------------------------------------------------------------------
static void STA_ACT_VBlankFunc(GFL_TCB *tcb,void *work)
{
  GFL_CLACT_SYS_VBlankFunc();
}

//--------------------------------------------------------------
//  �`��n������
//--------------------------------------------------------------
static void STA_ACT_SetupGraphic( ACTING_WORK *work )
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
  
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );
  
  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    
    // BG1 MAIN (����
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (��
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x00000,0x4000,
      GX_BG_EXTPLTT_01, 1, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i 256*16�F
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x2000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x10000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    
    
    //SUB bg 32K
    // BG1 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000,0x4000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (�ϋq�̎�
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x0000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG0 SUB (�ϋq�̊�
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x0000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (Info
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x04000,0x1000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );
    STA_ACT_SetupBgFunc( &header_main1, ACT_FRAME_MAIN_FONT , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main2, ACT_FRAME_MAIN_CURTAIN , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main3, ACT_FRAME_MAIN_BG , GFL_BG_MODE_256X16);
    STA_ACT_SetupBgFunc( &header_sub1 , ACT_FRAME_SUB_BG , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_sub2 , ACT_FRAME_SUB_AUDI_NECK , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_sub0 , ACT_FRAME_SUB_AUDI_FACE , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_sub3 , ACT_FRAME_SUB_INFO , GFL_BG_MODE_TEXT);
    
    GFL_BG_SetVisible( ACT_FRAME_MAIN_3D , TRUE );
  }
  
  { //3D�n�̐ݒ�
    static const VecFx32 cam_pos = MUSICAL_CAMERA_POS;
    static const VecFx32 cam_target = MUSICAL_CAMERA_TRG;
    static const VecFx32 cam_up = MUSICAL_CAMERA_UP;
    //�G�b�W�}�[�L���O�J���[
    static  const GXRgb stage_edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT80K,
            0, work->heapId, NULL );
    GFL_BG_SetBGControl3D( 2 ); //NNS_g3dInit�̒��ŕ\���D�揇�ʕς��E�E�E
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
#if 0 //�����ˉe�J����
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
                       FX32_SIN13,
                       FX32_COS13,
                       FX_F32_TO_FX32( 1.33f ),
                       NULL,
                       FX32_ONE,
                       FX32_ONE * 300,
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
#else
    //���ˉe�J����
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       FX32_ONE*12.0f,
                       0,
                       0,
                       FX32_ONE*16.0f,
                       MUSICAL_CAMERA_NEAR,
                       MUSICAL_CAMERA_FAR,
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
#endif
    
    GFL_G3D_CAMERA_Switching( work->camera );
    //�G�b�W�}�[�L���O�J���[�Z�b�g
    G3X_SetEdgeColorTable( &stage_edge_color_table[0] );
    G3X_EdgeMarking( TRUE );
    G3X_AntiAlias( FALSE );
    G3X_AlphaBlend( TRUE );
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
  }
  {
    GFL_BBD_SETUP bbdSetup = {
      128,128,
      {FX32_ONE,FX32_ONE,FX32_ONE},
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      ACT_POLYID_SHADOW, //�e�p��
			GFL_BBD_ORIGIN_CENTER,
    };
    VecFx32 scale ={FX32_ONE*4,FX32_ONE*4,FX32_ONE};
    //�r���{�[�h�V�X�e���\�z
    work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
    //�w�i�̂��߂ɑS�̂��S�{����
    GFL_BBD_SetScale( work->bbdSys , &scale );
  }
  
  //CELL������(���C�g�E�ϋq�p)
  {
    GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  //���C�g�p�̃A���t�@�ݒ�
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_OBJ , GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG3 , 4 , 31 );
}

//--------------------------------------------------------------
//  BG�̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupBg( ACTING_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );
/*
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_stage_bg01_NCLR , 
                    PALTYPE_MAIN_BG_EX , 0x6000 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_stage_bg01_NCGR ,
                    ACT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_stage_bg01_NSCR , 
                    ACT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );
*/
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_maku_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_maku_NCGR ,
                    ACT_FRAME_MAIN_CURTAIN , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_maku_NSCR , 
                    ACT_FRAME_MAIN_CURTAIN ,  0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_sub_bg_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_sub_bg_NCGR ,
                    ACT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_sub_back_NSCR , 
                    ACT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_bg_audience_NSCR , 
                    ACT_FRAME_SUB_AUDI_NECK ,  0 , 0, FALSE , work->heapId );

  GFL_ARC_CloseDataHandle(arcHandle);

  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_BG);
  GFL_BG_LoadScreenReq(ACT_FRAME_SUB_BG);
  
  work->bgSys = STA_BG_InitSystem( work->heapId , work );

}

void  STA_ACT_LoadBg( ACTING_WORK *work , const u8 bgNo )
{
/*
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_stage_bg00_NCLR + bgNo, 
                    PALTYPE_MAIN_BG_EX , 0x6000 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_stage_bg00_NCGR  + bgNo,
                    ACT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_stage_bg00_NSCR  + bgNo, 
                    ACT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );
  GFL_ARC_CloseDataHandle(arcHandle);
  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_BG);
*/

  STA_BG_CreateBg( work->bgSys , bgNo );

}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------
//  �\���|�P�����̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupPokemon( ACTING_WORK *work )
{
  u8 i;
  VecFx32 pos = {0,FX32_CONST(160.0f),FX32_CONST(170.0f)};
  const fx32 XBase = FX32_CONST(512.0f/(MUSICAL_POKE_MAX+1));
  
  work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
  MUS_POKE_DRAW_SetTexAddres( work->drawSys , 0x40000 );
  work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , MUSICAL_POKE_MAX*MUS_POKE_EQUIP_MAX, work->heapId );
  
  work->pokeSys = STA_POKE_InitSystem( work->heapId , work , work->drawSys , work->itemDrawSys , work->bbdSys );

  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->pokeWork[i] = NULL;
  }

  
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    pos.x = XBase*(i+1);
    work->pokeWork[i] = STA_POKE_CreatePoke( work->pokeSys , work->initWork->musPoke[i] );
    STA_POKE_SetPosition( work->pokeSys , work->pokeWork[i] , &pos );

    pos.z -= FX32_CONST(30.0f); //�ЂƃL�����̌��݂�30�ƌ���
  }
}

//--------------------------------------------------------------
//�A�C�e���̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupItem( ACTING_WORK *work )
{
  int i;
  VecFx32 pos = {0,0,0};
  
  for( i=0;i<ACT_OBJECT_MAX;i++ )
  {
    work->objWork[i] = NULL;
  }

  //�w�iObj
  {
    work->objSys = STA_OBJ_InitSystem( work->heapId , work->bbdSys );
/*
    pos.x = FX32_CONST( 64.0f );
    pos.y = FX32_CONST( 96.0f );
    pos.z = FX32_CONST( 50.0f);
    work->objWork[0] = STA_OBJ_CreateObject( work->objSys , 0 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[0] , &pos );

    pos.x = FX32_CONST( 448.0f );
    pos.y = FX32_CONST( 100.0f );
    pos.z = FX32_CONST( 50.0f );
    work->objWork[1] = STA_OBJ_CreateObject( work->objSys , 1 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[1] , &pos );

    pos.x = FX32_CONST( 192.0f );
    pos.y = FX32_CONST( 160.0f );
    pos.z = FX32_CONST( 180.0f );
    work->objWork[2] = STA_OBJ_CreateObject( work->objSys , 1 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[2] , &pos );
*/
  }


}

static void STA_ACT_SetupEffect( ACTING_WORK *work )
{
  int i;
  work->effSys = STA_EFF_InitSystem( work->heapId);

  for( i=0;i<ACT_EFFECT_MAX;i++ )
  {
    work->effWork[i] = NULL;
  }

//  work->effWork[0] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_00_spa );
//  work->effWork[1] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_01_spa );
}

static void STA_ACT_UpdateScroll( ACTING_WORK *work )
{
  const u8 spd = 2;
#if DEB_ARI|1
  if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    if( work->scrollOffsetTrget + spd < ACT_BG_SCROLL_MAX )
    {
      work->scrollOffsetTrget += spd;
    }
    else
    {
      work->scrollOffsetTrget = ACT_BG_SCROLL_MAX;
    }
  }
  if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    if( work->scrollOffsetTrget - spd > 0 )
    {
      work->scrollOffsetTrget -= spd;
    }
    else
    {
      work->scrollOffsetTrget = 0;
    }
  }
  
  //�v���C���[�Ǐ]����
  if( (GFL_UI_KEY_GetCont() & (PAD_KEY_RIGHT|PAD_KEY_LEFT)) == 0 )
#endif
  {
    s16 scroll;
    VecFx32 pos;
    u8 lookTarget;
    
    if( work->attentionPoke == MUSICAL_POKE_MAX )
    {
      lookTarget = work->playerIdx;
    }
    else
    {
      lookTarget = work->attentionPoke;
    }
    
    if( work->pokeWork[lookTarget] != NULL )
    {
      STA_POKE_GetPosition( work->pokeSys , work->pokeWork[lookTarget] , &pos );
      scroll = FX_FX32_TO_F32( pos.x )-128;
      if( scroll > ACT_BG_SCROLL_MAX )
      {
        scroll = ACT_BG_SCROLL_MAX;
      }
      else if( scroll < 0 )
      {
        scroll = 0;
      }
      if( scroll != work->scrollOffset )
      {
        work->scrollOffsetTrget = scroll;
      }
    }
  }
  
  
  if( work->scrollOffsetTrget != work->scrollOffset )
  {
    
    VecFx32 cam_pos = MUSICAL_CAMERA_POS;
    VecFx32 cam_target = MUSICAL_CAMERA_TRG;
    
    //�Ǐ]����
    if( work->scrollOffset > work->scrollOffsetTrget )
    {
      if( work->scrollOffset > ACT_SCROLL_SPEED &&
          work->scrollOffset-ACT_SCROLL_SPEED > work->scrollOffsetTrget )
      {
        work->scrollOffset -= ACT_SCROLL_SPEED;
      }
      else
      {
        work->scrollOffset = work->scrollOffsetTrget;
      }
    }
    else
    if( work->scrollOffset < work->scrollOffsetTrget )
    {
      if( work->scrollOffset + ACT_SCROLL_SPEED < work->scrollOffsetTrget )
      {
        work->scrollOffset += ACT_SCROLL_SPEED;
      }
      else
      {
        work->scrollOffset = work->scrollOffsetTrget;
      }
    }
    
    cam_pos.x = MUSICAL_POS_X( work->scrollOffset );
    cam_target.x = MUSICAL_POS_X( work->scrollOffset );
    
    GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
    GFL_G3D_CAMERA_SetTarget( work->camera , &cam_target );
    GFL_G3D_CAMERA_Switching( work->camera );
    
    GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_X_SET , work->scrollOffset );
    STA_BG_SetScrollOffset( work->bgSys , work->scrollOffset );
    STA_POKE_System_SetScrollOffset( work->pokeSys , work->scrollOffset ); 
    STA_OBJ_System_SetScrollOffset( work->objSys , work->scrollOffset ); 
    /*
    //GFL_BG_SetScroll( ACT_FRAME_MAIN_BG , GFL_BG_SCROLL_X_SET , work->scrollOffset );
    */
  }
  
#if DEB_ARI
  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
  {
    if( work->makuOffset + spd < ACT_CURTAIN_SCROLL_MAX )
    {
      work->makuOffset += spd;
    }
    else
    {
      work->makuOffset = ACT_CURTAIN_SCROLL_MAX;
    }
  }
  if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
  {
    if( work->makuOffset - spd > 0 )
    {
      work->makuOffset -= spd;
    }
    else
    {
      work->makuOffset = 0;
    }
    
  }
#endif
  GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_Y_SET , work->makuOffset );
}

static void STA_ACT_StartScript( ACTING_WORK *work )
{
  u8 i;
  const u32 *headData = (u32*)work->scriptData;
  const u8 pokeScriptArr[4] = 
  {
    STA_SCRIPT_POKE_1,
    STA_SCRIPT_POKE_2,
    STA_SCRIPT_POKE_3,
    STA_SCRIPT_POKE_4
  };

  STA_SCRIPT_SetScript( work->scriptSys , (u8*)work->scriptData+headData[STA_SCRIPT_MAIN] , TRUE );
  for( i=0; i<4 ; i++ )
  {
    if( headData[pokeScriptArr[i]] != headData[STA_SCRIPT_MAIN] )
    {
      STA_SCRIPT_SetScript( work->scriptSys , (u8*)work->scriptData+headData[pokeScriptArr[i]] , TRUE );
    }
  }
}

#pragma mark [> message func
//--------------------------------------------------------------
//  ���b�Z�[�W�֌W
//--------------------------------------------------------------
static void STA_ACT_SetupMessage( ACTING_WORK *work )
{
  //���b�Z�[�W�p����
  work->msgWin = GFL_BMPWIN_Create( ACT_FRAME_MAIN_FONT , ACT_MSG_POS_X , ACT_MSG_POS_Y ,
                  ACT_MSG_POS_WIDTH , ACT_MSG_POS_HEIGHT , ACT_PAL_FONT ,
                  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
  GFL_BMPWIN_MakeScreen( work->msgWin );
  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_FONT);
  
  
  //�t�H���g�ǂݍ���
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //���b�Z�[�W
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_00_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , ACT_PAL_FONT*0x20, 16*2, work->heapId );
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
  work->msgStr = NULL;
}

static void STA_ACT_UpdateMessage( ACTING_WORK *work )
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
}

void STA_ACT_ShowMessage( ACTING_WORK *work , const u16 msgNo , const u8 msgSpd )
{
  if( work->printHandle != NULL )
  {
    //�f�o�b�O���ɂ�����ʂ̕����؂�ւ��ɂŕ������o�O�鎖������������
//    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
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
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
    work->msgStr = GFL_MSG_CreateString( work->msgHandle , msgNo );
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, work->msgStr ,work->fontHandle ,
                        msgSpd , work->tcblSys , 2 , work->heapId , 0 );
  }
}

void STA_ACT_HideMessage( ACTING_WORK *work )
{
  if( work->printHandle != NULL )
  {
    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
    work->msgStr = NULL;
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
}

#pragma mark [> attention func
//--------------------------------------------------------------
//  ���ڃ|�P�����X�V
//--------------------------------------------------------------
static void STA_ACT_UpdateAttention( ACTING_WORK *work )
{
  if( work->isUpdateAttention == TRUE )
  {
    u8 i;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      STA_AUDI_SetAttentionPoke( work->audiSys , i , FALSE );
    }
    
    if( work->useItemPoke < MUSICAL_POKE_MAX )
    {
      //�ꔭ�t�]��
      work->attentionPoke = work->useItemPoke;
      STA_AUDI_SetAttentionPoke( work->audiSys , work->useItemPoke , TRUE );
    }
    else
    if( work->lightUpPoke < MUSICAL_POKE_MAX )
    {
      //�l���Z��
      work->attentionPoke = work->lightUpPoke;
      STA_AUDI_SetAttentionPoke( work->audiSys , work->lightUpPoke , TRUE );
    }
    else
    {
      u8 maxPoint = 0;
      //���_�Ń`�F�b�N
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        if( maxPoint < work->initWork->musPoke[i]->point )
        {
          maxPoint = work->initWork->musPoke[i]->point;
        }
      }
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        if( maxPoint == work->initWork->musPoke[i]->point )
        {
          STA_AUDI_SetAttentionPoke( work->audiSys , i , TRUE );
        }
      }
      
      work->attentionPoke = MUSICAL_POKE_MAX; //�J�����̓f�t�H���g
    }
    work->isUpdateAttention = FALSE;
  }
}

void STA_ACT_SetLightUpFlg( ACTING_WORK *work , const u8 pokeIdx , const BOOL flg )
{
  if( flg == TRUE )
  {
    work->lightUpPoke = pokeIdx;
  }
  else
  {
    work->lightUpPoke = MUSICAL_POKE_MAX;
  }
  work->isUpdateAttention = TRUE;
}
#pragma mark [> BGM func
//--------------------------------------------------------------
//  BGM�֌W
//--------------------------------------------------------------
void  STA_ACT_StartBgm( ACTING_WORK *work )
{
  //SND_STRM_SetUp( ARCID_SNDSTRM, NARC_snd_strm_poketari_swav, SND_STRM_PCM8, SND_STRM_8KHZ, work->heapId );
  SND_STRM_SetUpStraightData( SND_STRM_PCM8, 
                               SND_STRM_8KHZ, 
                               work->heapId , 
                               work->initWork->distData->strmData , 
                               work->initWork->distData->strmDataSize );
  OS_TPrintf("[%d]\n",work->initWork->distData->strmDataSize);
  SND_STRM_Play();
}

void  STA_ACT_StopBgm( ACTING_WORK *work )
{
  SND_STRM_Stop();
  SND_STRM_Release();
}

#pragma mark [> itemUse func
//--------------------------------------------------------------
//�A�C�e���̎g�p���N�G�X�g
//--------------------------------------------------------------
void STA_ACT_UseItemRequest( ACTING_WORK *work , MUS_POKE_EQUIP_POS ePos )
{
  if( work->initWork->commWork == NULL )
  {
    //��ʐM��
    STA_ACT_UseItem( work , work->playerIdx , ePos );
  }
  else
  {
    //�ʐM��
    work->useItemReq = TRUE;
    work->useItemReqPos = ePos;
  }
}
//--------------------------------------------------------------
//�A�C�e���̎g�p
//--------------------------------------------------------------
void STA_ACT_UseItem( ACTING_WORK *work , u8 pokeIdx , MUS_POKE_EQUIP_POS ePos )
{
  work->useItemFlg[pokeIdx] = TRUE;
  work->useItemPos[pokeIdx] = ePos;
}
//--------------------------------------------------------------
//�A�C�e���̎g�p�̍X�V
//--------------------------------------------------------------
static void STA_ACT_UpdateUseItem( ACTING_WORK *work )
{
  u8 i;
  u8 usePokeArr[MUSICAL_POKE_MAX];
  u8 usePokeNum = 0;

  //�ʐM�����M����
  if( work->initWork->commWork != NULL )
  {
    if( work->useItemReq == TRUE )
    {
      const BOOL ret = MUS_COMM_Send_UseButtonFlg( work->initWork->commWork , work->useItemReqPos );
      if( ret == TRUE )
      {
        work->useItemReq = FALSE;
      }
    }
  }

  //�g�p����
  if( work->initWork->commWork == NULL )
  {
    //��ʐM��
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      if( work->useItemFlg[i] == TRUE )
      {
        STA_POKE_UseItemFunc( work->pokeSys , work->pokeWork[i] , work->useItemPos[i] );
        usePokeArr[usePokeNum] = i;
        usePokeNum++;
        work->useItemFlg[i] = FALSE;
      }
    }
    if( usePokeNum > 0 )
    {
      work->useItemCnt = ACT_USEITEM_EFF_TIME;
      work->useItemPoke = usePokeArr[ GFL_STD_MtRand0(usePokeNum)];
      work->isUpdateAttention = TRUE;
    }
  }
  else
  {
    //�ʐM��
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      const u8 usePos = MUS_COMM_GetUseButtonPos( work->initWork->commWork , i );
      if( usePos != MUS_POKE_EQU_INVALID )
      {
        OS_TPrintf("Use item!![%d][%d]\n",i,usePos);
        STA_POKE_UseItemFunc( work->pokeSys , work->pokeWork[i] , usePos );
        MUS_COMM_ResetUseButtonPos( work->initWork->commWork , i );
      }
    }
    {
      const u8 AttentionIdx = MUS_COMM_GetUseButtonAttention( work->initWork->commWork );
      if( AttentionIdx < MUSICAL_POKE_MAX )
      {
        work->useItemCnt = ACT_USEITEM_EFF_TIME;
        work->useItemPoke = AttentionIdx;
        work->isUpdateAttention = TRUE;
        MUS_COMM_ResetUseButtonAttention( work->initWork->commWork );
      }
    }
  }
  
  //���Ԕ���
  if( work->useItemCnt > 0 )
  {
    work->useItemCnt--;
    if( work->useItemCnt == 0 )
    {
      work->useItemPoke = MUSICAL_POKE_MAX;
      work->isUpdateAttention = TRUE;
    }
  }
}

#pragma mark [> trans effect func
static void STA_ACT_InitTransEffect( ACTING_WORK *work )
{
  u8 i;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    //TODO �X�e�[�^�X�ŃG�t�F�N�g�̎�ޕς��H
    work->transEffWork[i] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_trans_1_spa );
  }
}

static void STA_ACT_TermTransEffect( ACTING_WORK *work )
{
  u8 i;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    STA_EFF_DeleteEffect( work->effSys , work->transEffWork[i] );
  }
}

void STA_ACT_PlayTransEffect( ACTING_WORK *work , const u8 idx )
{
  //TODO �G�t�F�N�g�̎�ޕς������Đ��G�~�b�^���ς��B����1��
  u8 i;
  VecFx32 pos;
  const VecFx32 *ofs = STA_POKE_GetRotOffset( work->pokeSys , work->pokeWork[idx] );
  STA_POKE_GetPosition( work->pokeSys , work->pokeWork[idx] , &pos );
  pos.x = ACT_POS_X_FX(pos.x + ofs->x );
  pos.y = ACT_POS_Y_FX(pos.y + ofs->y );
  pos.z = FX32_CONST(180.0f);
  for( i=0;i<1;i++ )
  {
    STA_EFF_CreateEmitter( work->transEffWork[idx] , i , &pos );
  }
}

#pragma mark [> offer func
//--------------------------------------------------------------
//  �X�N���v�g�p�ɊO���񋟊֐�
//--------------------------------------------------------------
void STA_ACT_StartMainPart( ACTING_WORK *work )
{
  //���C���p�[�g�̊J�n
  STA_BUTTON_SetCanUseButton( work->buttonSys , TRUE );
}
void STA_ACT_FinishMainPart( ACTING_WORK *work )
{
  //���C���p�[�g�̏I��
  STA_BUTTON_SetCanUseButton( work->buttonSys , FALSE );
}

STA_POKE_SYS* STA_ACT_GetPokeSys( ACTING_WORK *work )
{
  return work->pokeSys;
}
STA_POKE_WORK* STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < MUSICAL_POKE_MAX );
  return work->pokeWork[idx];
}
void STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx )
{
  GF_ASSERT( idx < MUSICAL_POKE_MAX );
  work->pokeWork[idx] = pokeWork;
}

STA_OBJ_SYS* STA_ACT_GetObjectSys( ACTING_WORK *work )
{
  return work->objSys;
}
STA_OBJ_WORK* STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_OBJECT_MAX );
  return work->objWork[idx];
}
void STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_OBJECT_MAX );
  work->objWork[idx] = objWork;
}

STA_EFF_SYS* STA_ACT_GetEffectSys( ACTING_WORK *work )
{
  return work->effSys;
}
STA_EFF_WORK* STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_EFFECT_MAX );
  return work->effWork[idx];
}
void STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_EFFECT_MAX );
  work->effWork[idx] = effWork;
}

STA_LIGHT_SYS* STA_ACT_GetLightSys( ACTING_WORK *work )
{
  return work->lightSys;
}
STA_LIGHT_WORK* STA_ACT_GetLightWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_LIGHT_MAX );
  return work->lightWork[idx];
}
void STA_ACT_SetLightWork( ACTING_WORK *work , STA_LIGHT_WORK *lightWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_LIGHT_MAX );
  work->lightWork[idx] = lightWork;
}

STA_AUDI_SYS* STA_ACT_GetAudienceSys( ACTING_WORK *work )
{
  return work->audiSys;
}

u16   STA_ACT_GetCurtainHeight( ACTING_WORK *work )
{
  return work->makuOffset;
}
void  STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height )
{
  work->makuOffset = height;
}
u16   STA_ACT_GetStageScroll( ACTING_WORK *work )
{
  return work->scrollOffset;
}
void  STA_ACT_SetStageScroll( ACTING_WORK *work , const u16 scroll )
{
  work->scrollOffset = scroll;
}

MUS_ITEM_DATA_SYS* STA_ACT_GetItemDataSys( ACTING_WORK *work )
{
  return MUS_ITEM_DRAW_GetItemDataSys( work->itemDrawSys );
}

//�A�C�e���g�p�Ŗڗ����Ă���|�P�����̎擾
const u8 STA_ACT_GetUseItemPoke( ACTING_WORK *work )
{
  return work->useItemPoke;
}
#pragma mark [> editor func
//--------------------------------------------------------------
//�G�f�B�^�p
//--------------------------------------------------------------
void  STA_ACT_EDITOR_SetScript( ACTING_WORK *work , void* data )
{
  if( work->scriptData != NULL )
  {
    GFL_HEAP_FreeMemory( work->scriptData );
  }
  work->scriptData = data;
}

void  STA_ACT_EDITOR_StartScript( ACTING_WORK *work )
{
  if( work->scriptData != NULL )
  {
    STA_ACT_StartScript( work );
  }
}


#pragma mark [> photo func
//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�p������
//--------------------------------------------------------------
ACTING_WORK*  STA_ACT_InitActingPhoto( HEAPID heapId )
{
  u8 i;
  ACTING_WORK *work = GFL_HEAP_AllocClearMemory( heapId , sizeof( ACTING_WORK ));
  
  work->heapId = heapId;
  work->initWork = NULL;
  work->scrollOffset = 0;
  work->makuOffset = 0;
  work->scriptData = NULL;
  work->mainSeq = AMS_FADEIN;
  
  work->vblankFuncTcb = GFUser_VIntr_CreateTCB( STA_ACT_VBlankFunc , (void*)work , 64 );
  
  //���L�����̑I��
  if( work->initWork->commWork == NULL )
  {
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      if( work->initWork->musPoke[i]->charaType == MUS_CHARA_PLAYER )
      {
        work->playerIdx = i;
        break;
      }
    }
    if( MUSICAL_POKE_MAX == i )
    {
      GF_ASSERT_MSG( MUSICAL_POKE_MAX != i , "Musical stage:Player poke is not found!!!\n");
      work->playerIdx = 1;
    }
  }
  else
  {
    work->playerIdx = MUS_COMM_GetSelfMusicalIndex(work->initWork->commWork);
  }
  
  //���ڃ|�P�n������
  work->isUpdateAttention = TRUE;
  work->lightUpPoke = MUSICAL_POKE_MAX;
  work->attentionPoke = MUSICAL_POKE_MAX; //�J�����̓f�t�H���g
  //�A�C�e���g�p�n������
  work->useItemPoke = MUSICAL_POKE_MAX;
  work->useItemCnt = 0;
  work->useItemReq = FALSE;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->useItemFlg[i] = FALSE;
  }

//  STA_ACT_SetupBg( work );
//  STA_ACT_SetupMessage( work);
//  STA_ACT_SetupPokemon( work );
//  STA_ACT_SetupItem( work );
//  STA_ACT_SetupEffect( work );
  
//  work->lightSys = STA_LIGHT_InitSystem(work->heapId , work );
//  work->audiSys = STA_AUDI_InitSystem( work->heapId , work , work->initWork );
//  work->buttonSys = STA_BUTTON_InitSystem( work->heapId , work , &work->initWork->musPoke[work->playerIdx] );

  return work;
}

//--------------------------------------------------------------
//  �~���[�W�J���V���b�g�p�J��
//--------------------------------------------------------------
void  STA_ACT_TermActingPhoto( ACTING_WORK *work )
{

//  if( work->msgStr != NULL )
//  {
//    GFL_STR_DeleteBuffer( work->msgStr );
//  }
//  GFL_MSG_Delete( work->msgHandle );
//  GFL_BMPWIN_Delete( work->msgWin );
//  GFL_FONT_Delete( work->fontHandle );
//  GFL_TCBL_Exit( work->tcblSys );
  
//  STA_BUTTON_ExitSystem( work->buttonSys );
//  STA_AUDI_ExitSystem( work->audiSys );
//  STA_BG_ExitSystem( work->bgSys );
//  STA_LIGHT_ExitSystem( work->lightSys );
//  STA_EFF_ExitSystem( work->effSys );
//  STA_OBJ_ExitSystem( work->objSys );
//  STA_POKE_ExitSystem( work->pokeSys );

//  MUS_POKE_DRAW_TermSystem( work->drawSys );
//  MUS_ITEM_DRAW_TermSystem( work->itemDrawSys );

  GFL_HEAP_FreeMemory( work );
  
}
