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
#include "stage_gra.naix"
#include "mididl.naix"
#include "message.naix"
#include "print/printsys.h"



#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"
#include "musical/musical_camera_def.h"

#include "musical/musical_stage_sys.h"
#include "sta_local_def.h"
#include "sta_snd_def.h"
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

#define ACT_START_WAIT_TIME (60)

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
  AMS_ACTING_START_WAIT,  //�{�ԑO�̃E�F�C�g
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

  u16   startWait;
  
  u16   scrollOffset;
  u16   scrollOffsetTrget;
  u16   makuOffset;
  u8    playerIdx;
  BOOL  scrollLockFirst;
  BOOL  lockScroll;
  BOOL  forceScroll;
  ARCHANDLE *arcHandle;
  
  ACTING_MAIN_SEQ mainSeq;
  
  GFL_TCB   *vblankFuncTcb;
  
  //BGM�`�F�b�N�n
  BOOL isReadyExBgm;
  BOOL isPlayExBgm;
  BOOL isPlayBgm;

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
  BOOL        updateQue;
  PRINT_QUE   *printQue;
  
  //���ڃ|�P�p
  BOOL    isUpdateAttention;
  u8      lightUpPoke;
  u8      attentionPoke;  //�J�����̑Ώ�
  u8      attentionLight;  //�J�����̑Ώ�
  u8      pokeRank[MUSICAL_POKE_MAX];
  u8      tempPoint[MUSICAL_POKE_MAX];  //�A�s�[���{�[�i�X(�{�v�Z�͊O�ŁI
  
  //�A�C�e���g�p�`
  BOOL    useItemFlg[MUSICAL_POKE_MAX];
  MUS_POKE_EQUIP_POS useItemPos[MUSICAL_POKE_MAX];
  u8      useItemPoke;  //�ꔭ�t�]���|�P
  u8      useItemPokePos;   //�ꔭ�t�]���|�P(�g�p�ӏ�
  u16     useItemCnt;
  BOOL    isUseItemSolo;  //�{�[�i�X��薳���`�F�b�N
  u16     npcUseItemCnt[MUSICAL_POKE_MAX];
  MUS_POKE_EQUIP_POS     npcUseItemPos[MUSICAL_POKE_MAX];
  BOOL    isUsedItemPos[MUSICAL_POKE_MAX][2]; //(0:R,1:L)a
  
  //�ʐM���ɑ��郊�N�G�X�g
  BOOL    useItemReq;
  MUS_POKE_EQUIP_POS useItemReqPos;
  
  BOOL isEditorMode;
  
  //�ϋq��p���b�g�t�F�[�h
  u16 palAnmCnt;
  u16 basePal[16];
  s8 ofsCol[3][16];
  u16 transPal[16];
  
  void* bgmSeqData;
  void* bgmBankData;
  void* bgmWaveData;
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
static void STA_ACT_UpdatePalAnm( ACTING_WORK *work );

static void STA_ACT_UpdateScroll( ACTING_WORK *work );
static void STA_ACT_UpdateMessage( ACTING_WORK *work );

static void STA_ACT_StartScript( ACTING_WORK *work );

static void STA_ACT_UpdateAttention( ACTING_WORK *work );

static void STA_ACT_UpdateUseItem( ACTING_WORK *work );
static u32 STA_ACT_GetUseItemSe( ACTING_WORK *work , const u8 usePoke , const u8 usePos );
static const BOOL STA_ACT_IsUsingItem( ACTING_WORK *work , const u8 idx );
static void STA_ACT_UpdateUseItemNpc( ACTING_WORK *work );

static void STA_ACT_InitTransEffect( ACTING_WORK *work );
static void STA_ACT_TermTransEffect( ACTING_WORK *work );
static void STA_ACT_InitItemUseEffect( ACTING_WORK *work );
static void STA_ACT_TermItemUseEffect( ACTING_WORK *work );

static void STA_ACT_DebugPolyDraw( ACTING_WORK *work );


static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_D,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_16_F,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_01_AB,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_0123_E,     // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};
//  A �e�N�X�`��
//  B �e�N�X�`��
//  C SubBg
//  D MainBgj
//  E �e�N�X�`���p���b�g
//  F MainObj
//  G None
//  H None
//  I SubObj
extern BOOL	GFL_MCS_Resident_SendHeapStatus(void);

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
ACTING_WORK*  STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId )
{
  u8 i;
  ACTING_WORK *work = GFL_HEAP_AllocClearMemory( heapId , sizeof( ACTING_WORK ));
  
  work->heapId = heapId;
  work->initWork = initWork;
  work->startWait = 0;
  work->scrollOffset = 127; //����X�V�̂��߂ɂ��炵�Ă���
  work->scrollOffsetTrget = 128;
  work->makuOffset = 0;
  work->scriptData = NULL;
  work->mainSeq = AMS_FADEIN;
  work->isEditorMode = FALSE;
  work->scrollLockFirst = TRUE; //�����オ�肫��܂ł͌Œ肳����
  work->forceScroll = FALSE;
  work->lockScroll = FALSE;

  work->isReadyExBgm = FALSE;
  work->isPlayExBgm = FALSE;
  work->isPlayBgm = FALSE;

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

  //���ʂ̌v�Z
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->tempPoint[i] = 0;
  }
  STA_ACT_CalcRank( work );
  
  //���ڃ|�P�n������
  work->isUpdateAttention = TRUE;
  work->lightUpPoke = MUSICAL_POKE_MAX;
  work->attentionPoke = MUSICAL_POKE_MAX; //�J�����̓f�t�H���g
  work->attentionLight = ACT_LIGHT_MAX; //�J�����̓f�t�H���g
  //�A�C�e���g�p�n������
  work->useItemPoke = MUSICAL_POKE_MAX;
  work->useItemCnt = 0;
  work->useItemReq = FALSE;
  
  work->palAnmCnt = 0;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->useItemFlg[i] = FALSE;
    work->npcUseItemCnt[i] = 0;
    work->isUsedItemPos[i][0] = FALSE;
    work->isUsedItemPos[i][1] = FALSE;
  }

  work->arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );

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
  
  //INFOWIN_Init( ACT_FRAME_SUB_INFO,ACT_PAL_INFO,NULL,work->heapId);

  GFL_NET_ReloadIconTopOrBottom(FALSE , work->heapId );

#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_InitProc( ACT_FRAME_MAIN_CURTAIN , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 31,31,31 );
#endif  //USE_DEBUGWIN_SYSTEM
//  ARI_TPrintf("FreeHeap:[%x]\n", GFL_HEAP_GetHeapFreeSize( work->heapId ) );

  STA_ACT_LoadBg( work , 0 );
  
  
  work->bgmSeqData = work->initWork->distData->midiSeqData;
  work->bgmBankData = work->initWork->distData->midiBnkData;
  work->bgmWaveData = work->initWork->distData->midiWaveData;
  
  return work;
}


void  STA_ACT_TermActing( ACTING_WORK *work )
{
#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_ExitProc();
#endif  //USE_DEBUGWIN_SYSTEM
  
  if( work->isPlayBgm == TRUE )
  {
    PMSND_StopBGM();
  }

  if( work->isPlayExBgm == TRUE )
  {
    PMDSND_StopExtraMusic();
  }
  
  if( work->isReadyExBgm == TRUE )
  {
  	PMDSND_ReleaseExtraMusic();
  }

  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE , GX_BLEND_PLANEMASK_NONE , 31 , 31 );
  GX_SetVisibleWnd( GX_WNDMASK_NONE ); 

  //INFOWIN_Exit();

  STA_ACT_TermTransEffect( work );
  
  STA_SCRIPT_ExitSystem( work->scriptSys );

  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
  }
  PRINTSYS_QUE_Clear( work->printQue );
  PRINTSYS_QUE_Delete( work->printQue );
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
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_MASK );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_INFO );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GFL_TCB_DeleteTask( work->vblankFuncTcb );

  GFL_ARC_CloseDataHandle(work->arcHandle);
  GFL_HEAP_FreeMemory( work );
  
}

ACTING_RETURN STA_ACT_LoopActing( ACTING_WORK *work )
{
  static BOOL stopScript = FALSE;
#if PM_DEBUG
  if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      
      //������
      //STA_SCRIPT_SetScript( work->scriptSys , (void*)musicalScriptTestData , FALSE );
      if( work->scriptData != NULL )
      {
        STA_ACT_StartScript( work );
      }
    }
  }
  else
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      stopScript = !stopScript;
      if( stopScript == TRUE )
      {
        PMSND_PauseBGM( TRUE );
      }
      else
      {
        PMSND_PauseBGM( FALSE );
      }
    }
    /*
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      void *scriptData = GFL_ARCHDL_UTIL_Load( work->arcHandle, NARC_stage_gra_sub_script_01_bin , FALSE , work->heapId );
      STA_SCRIPT_SetSubScript( work->scriptSys , scriptData , 0 , 5 );
    }
    */
  }
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
        work->mainSeq = AMS_ACTING_START_WAIT;
      }
    }
    break;
    
  case AMS_FADEOUT:
    if( PMSND_CheckPlaySE() == FALSE )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      work->mainSeq = AMS_WAIT_FADEOUT;
    }
    break;
    
  case AMS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return ACT_RET_GO_END;
    }
    break;
  case AMS_ACTING_START_WAIT:
    if( work->startWait++ >= ACT_START_WAIT_TIME )
    {
      work->mainSeq = AMS_ACTING_START;
    }
    break;
  case AMS_ACTING_START:
    if( work->isEditorMode == FALSE )
    {
      work->scriptData = work->initWork->distData->scriptData;
      STA_ACT_StartScript( work );
      work->mainSeq = AMS_ACTING_MAIN;
    }
    break;
  
  case AMS_ACTING_MAIN:
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
    {
      work->scriptData = NULL;
      if( work->isEditorMode == FALSE )
      {
        work->mainSeq = AMS_FADEOUT;
      }
      else
      {
        work->mainSeq = 0xFF;
      }
    }
    break;
    
  case AMS_COMM_START_SYNC:
    if( MUS_COMM_CheckTimingCommand( work->initWork->commWork , MUS_COMM_TIMMING_START_SCRIPT ) == TRUE )
    {
      work->mainSeq = AMS_ACTING_START_WAIT;
    }
    break;
    
  }

  //INFOWIN_Update();
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

  STA_ACT_UpdatePalAnm( work );

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
    
    STA_ACT_DebugPolyDraw( work );
  }
  GFL_G3D_DRAW_End();
  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  if( work->initWork->commWork != NULL )
  {
    if( work->mainSeq != AMS_FADEIN &&
        work->mainSeq != AMS_WAIT_FADEIN &&
        work->mainSeq != AMS_FADEOUT &&
        work->mainSeq != AMS_WAIT_FADEOUT )
    {
      if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE )
      {
        //���N�G�X�g�͂��Ȃ�
        work->mainSeq = AMS_FADEOUT;
      }
    }
  }


#if PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT &&
    GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
  {
    return ACT_RET_GO_END;
  }
#endif //DEB_ARI
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
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
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
      0, 0, 0x2000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x18000,0x8000,
      GX_BG_EXTPLTT_01, 1, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (MASK
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    
    
    //SUB bg 32K
    // BG1 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (�ϋq�̎�
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG0 SUB (�ϋq�̊�
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x18000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (Info
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x00000,0x1000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );
    STA_ACT_SetupBgFunc( &header_main1, ACT_FRAME_MAIN_FONT , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main2, ACT_FRAME_MAIN_CURTAIN , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main3, ACT_FRAME_MAIN_MASK , GFL_BG_MODE_TEXT);
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
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
            0, work->heapId, NULL );
    GFL_BG_SetBGControl3D( 3 ); //NNS_g3dInit�̒��ŕ\���D�揇�ʕς��E�E�E
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
    G3X_AntiAlias( TRUE );
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
      MUS_ITEM_POLIGON_ID, //�e�p��
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
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG3 , GX_BLEND_PLANEMASK_BG0 , 0 , 10 );

  GX_SetVisibleWnd( GX_WNDMASK_OW );
  G2_SetWndOBJInsidePlane( GX_WND_PLANEMASK_BG0 | 
                           GX_WND_PLANEMASK_BG1 | 
                           GX_WND_PLANEMASK_BG2 | 
                           GX_WND_PLANEMASK_OBJ , TRUE );
  G2_SetWndOutsidePlane(   GX_WND_PLANEMASK_BG0 | 
                           GX_WND_PLANEMASK_BG1 | 
                           GX_WND_PLANEMASK_BG2 | 
                           GX_WND_PLANEMASK_BG3 | 
                           GX_WND_PLANEMASK_OBJ , TRUE );
}

//--------------------------------------------------------------
//  BG�̏�����
//--------------------------------------------------------------
static void STA_ACT_SetupBg( ACTING_WORK *work )
{

  GFL_ARCHDL_UTIL_TransVramPaletteEx( work->arcHandle , NARC_stage_gra_dark_mask_NCLR , 
                    PALTYPE_MAIN_BG , ACT_PLT_BG_MAIN_MASK*32 , ACT_PLT_BG_MAIN_MASK*32 , 32 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_stage_gra_dark_mask_NCGR ,
                    ACT_FRAME_MAIN_MASK , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_stage_gra_dark_mask_NSCR , 
                    ACT_FRAME_MAIN_MASK ,  0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramPaletteEx( work->arcHandle , NARC_stage_gra_maku_NCLR , 
                    PALTYPE_MAIN_BG , ACT_PLT_BG_MAIN_MAKU*32 , ACT_PLT_BG_MAIN_MAKU*32 , 32 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_stage_gra_maku_NCGR ,
                    ACT_FRAME_MAIN_CURTAIN , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_stage_gra_maku_NSCR , 
                    ACT_FRAME_MAIN_CURTAIN ,  0 , 0, FALSE , work->heapId );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( work->arcHandle , NARC_stage_gra_sub_bg_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  //�w�i
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_stage_gra_sub_back_NCGR ,
                    ACT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_stage_gra_sub_back_NSCR , 
                    ACT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  //��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_stage_gra_sub_audi_neck_NCGR ,
                    ACT_FRAME_SUB_AUDI_NECK , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( work->arcHandle , NARC_stage_gra_sub_audi_neck_NSCR , 
                    ACT_FRAME_SUB_AUDI_NECK ,  0 , 0, FALSE , work->heapId );
  //��
  GFL_ARCHDL_UTIL_TransVramBgCharacter( work->arcHandle , NARC_stage_gra_sub_audi_face_NCGR ,
                    ACT_FRAME_SUB_AUDI_FACE , 0 , 0, FALSE , work->heapId );

  //�A�j���p�ɁA�p���b�g�����炤
  {
    u8 i;
    u16 endPal[16];
    GFL_STD_MemCopy16( (void*)(HW_DB_BG_PLTT), work->basePal , 16*2 );
    GFL_STD_MemCopy16( (void*)(HW_DB_BG_PLTT+32), endPal , 16*2 );
    
    for( i=0;i<16;i++ )
    {
      const u8 sr = (work->basePal[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      const u8 sg = (work->basePal[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      const u8 sb = (work->basePal[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      const u8 er = (endPal[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      const u8 eg = (endPal[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      const u8 eb = (endPal[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      work->ofsCol[0][i] = er-sr;
      work->ofsCol[1][i] = eg-sg;
      work->ofsCol[2][i] = eb-sb;
    }
    
    GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_MASK);
    GFL_BG_LoadScreenReq(ACT_FRAME_SUB_BG);
  }
  
  work->bgSys = STA_BG_InitSystem( work->heapId , work );

}

void  STA_ACT_LoadBg( ACTING_WORK *work , const u8 bgNo )
{
  const u8 bgIdx = MUSICAL_PROGRAM_GetBgNo( work->initWork->progWork );
  
  STA_BG_CreateBg( work->bgSys , bgIdx );

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
  VecFx32 pos = {FX32_CONST(256.0f),FX32_CONST(160.0f),FX32_CONST(170.0f)};
  
  work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
  MUS_POKE_DRAW_SetTexAddres( work->drawSys , 0x20000 );
  work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , MUSICAL_POKE_MAX*MUS_POKE_EQUIP_MAX, work->heapId );
  
  work->pokeSys = STA_POKE_InitSystem( work->heapId , work , work->drawSys , work->itemDrawSys , work->bbdSys );

  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->pokeWork[i] = NULL;
  }

  
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
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
    if( work->forceScroll == FALSE &&
        work->lockScroll == FALSE &&
        work->scrollLockFirst == FALSE ) 
    {
      s16 scroll;
      VecFx32 pos;
      u8 lookTarget;
      
      if( work->attentionLight == ACT_LIGHT_MAX )
      {
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
        }
      }
      else
      {
        STA_LIGHT_GetPosition( work->lightSys , work->lightWork[work->attentionLight] , &pos );
        scroll = FX_FX32_TO_F32( pos.x )-128;
      }
      
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
    STA_SUDI_SetScrollOffset( work->audiSys , work->scrollOffset ); 
    /*
    //GFL_BG_SetScroll( ACT_FRAME_MAIN_MASK , GFL_BG_SCROLL_X_SET , work->scrollOffset );
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
  if( work->makuOffset == ACT_CURTAIN_SCROLL_MAX )
  {
    work->scrollLockFirst = FALSE;
  }
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

void STA_ACT_StartAppealScript( ACTING_WORK *work , const u8 scriptNo , const u8 pokeNo )
{
  void *scriptData = GFL_ARCHDL_UTIL_Load( work->arcHandle , NARC_stage_gra_appeal_script_00_bin + scriptNo , FALSE , work->heapId );
  
  if( work->initWork->musPoke[pokeNo]->point >= 70 )
  {
    STA_SCRIPT_SetSubScript( work->scriptSys , scriptData , 0 , 1<<pokeNo );
  }
  else
  if( work->initWork->musPoke[pokeNo]->point >= 40 )
  {
    STA_SCRIPT_SetSubScript( work->scriptSys , scriptData , 1 , 1<<pokeNo );
  }
  else
  {
    STA_SCRIPT_SetSubScript( work->scriptSys , scriptData , 2 , 1<<pokeNo );
  }
  
}
void STA_ACT_StartSubScript( ACTING_WORK *work , const u8 scriptNo , const u8 pokeTrgBit )
{
  void *scriptData = GFL_ARCHDL_UTIL_Load( work->arcHandle , NARC_stage_gra_sub_script_00_bin + scriptNo , FALSE , work->heapId );
  STA_SCRIPT_SetSubScript( work->scriptSys , scriptData , 0 , pokeTrgBit );
  
}

static void STA_ACT_UpdatePalAnm( ACTING_WORK *work )
{
  static const u16 anmSpd = 0x160;
  
  if( work->palAnmCnt + anmSpd >= 0x10000 )
  {
    work->palAnmCnt = work->palAnmCnt+anmSpd-0x10000;
  }
  else
  {
    work->palAnmCnt += anmSpd;
  }
  
  {
    u8 i;
    const fx32 sin = (FX_SinIdx(work->palAnmCnt)+FX32_ONE)/2;
    for( i=0;i<16;i++ )
    {
      const u8 sr = (work->basePal[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      const u8 sg = (work->basePal[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      const u8 sb = (work->basePal[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      const s8 or = (work->ofsCol[0][i] * sin)>>FX32_SHIFT;
      const s8 og = (work->ofsCol[1][i] * sin)>>FX32_SHIFT;
      const s8 ob = (work->ofsCol[2][i] * sin)>>FX32_SHIFT;
      work->transPal[i] = GX_RGB( sr+or , sg+og , sb+ob );
    }
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                        0 ,
                                        work->transPal , 2*16 );
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
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //���b�Z�[�W
//  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_00_dat , work->heapId );
  work->msgHandle = GFL_MSG_Construct( work->initWork->distData->messageData , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , ACT_PAL_FONT*0x20, 16*2, work->heapId );
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
  work->msgStr = NULL;
  work->updateQue = FALSE;
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
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
  
  if( work->updateQue == TRUE )
  {
    PRINTSYS_QUE_Main( work->printQue );
    if( PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE )
    {
      GFL_BMPWIN_TransVramCharacter( work->msgWin );
      work->updateQue = FALSE;
    }
  }
}

void STA_ACT_ShowMessage( ACTING_WORK *work , const u16 msgNo , const s32 msgSpd )
{
  if( work->printHandle != NULL )
  {
    //�f�o�b�O���ɂ�����ʂ̕����؂�ւ��ɂŕ������o�O�鎖������������
//    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
    ARI_TPrintf( NULL , "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }
  
  if( work->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( work->msgStr );
    work->msgStr = NULL;
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  work->msgStr = GFL_MSG_CreateString( work->msgHandle , msgNo );

  if( msgSpd != 255 )
  {
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, work->msgStr ,work->fontHandle ,
                        msgSpd , work->tcblSys , 2 , work->heapId , 0 );
  }
  else
  {
    PRINTSYS_PrintQue( work->printQue , GFL_BMPWIN_GetBmp( work->msgWin ) , 
            0 , 0 , work->msgStr , work->fontHandle );
    work->updateQue = TRUE;
    GFL_STR_DeleteBuffer( work->msgStr );
    work->msgStr = NULL;
  }
}

void STA_ACT_HideMessage( ACTING_WORK *work )
{
  if( work->printHandle != NULL )
  {
    //GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
//    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
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
    u8 lightNum = 0;
    u8 lightNo;
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      STA_AUDI_SetAttentionPoke( work->audiSys , i , FALSE );
    }
    STA_AUDI_SetAttentionLight( work->audiSys , STA_AUDI_NO_TARGET );

    work->attentionPoke = MUSICAL_POKE_MAX; //�J�����̓f�t�H���g
    work->attentionLight = ACT_LIGHT_MAX; //�J�����̓f�t�H���g

    for( i=0;i<ACT_LIGHT_MAX;i++ )
    {
      if( work->lightWork[i] != NULL )
      {
        lightNo = i;
        lightNum++;
      }
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
    if( lightNum == 1 )
    {
      //���C�g��1����
      work->attentionLight = lightNo;
      STA_AUDI_SetAttentionLight( work->audiSys , lightNo );
    }
    else
    {
      u8 maxPoint = 0;
      /*
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
      */
      //�����N�`�F�b�N�ɕύX
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        if( work->pokeRank[i] == 0 )
        {
          STA_AUDI_SetAttentionPoke( work->audiSys , i , TRUE );
        }
      }
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
void STA_ACT_SetUpdateAttention( ACTING_WORK *work )
{
  work->isUpdateAttention = TRUE;
}

void STA_ACT_CalcRank( ACTING_WORK *work )
{
  u8 i;
  //���ʂ̌v�Z
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->pokeRank[i] = 0;
  }

  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    u8 j;
    const u16 selfPoint = work->initWork->musPoke[i]->point + work->tempPoint[i];
    for( j=0;j<MUSICAL_POKE_MAX;j++ )
    {
      if( i != j )
      {
        const u16 point = work->initWork->musPoke[j]->point + work->tempPoint[j];
        if( point > selfPoint )
        {
          work->pokeRank[i]++;
        }
      }
    }
    ARI_TPrintf("Rank[%d:%3d(+%2d)][%d]\n",i,work->initWork->musPoke[i]->point,work->tempPoint[i],work->pokeRank[i]);
  }
}

#pragma mark [> BGM func
//--------------------------------------------------------------
//  BGM�֌W
//--------------------------------------------------------------
void  STA_ACT_ReadyBgm( ACTING_WORK *work , const u16 seqNo )
{
  ARI_TPrintf("ReadyLinkSeqWaveData[%d]\n",seqNo);
  work->isReadyExBgm = TRUE;
  PMDSND_PresetExtraMusic( work->bgmSeqData , work->bgmBankData , seqNo );
}

void  STA_ACT_StartBgm( ACTING_WORK *work , const u16 seqNo )
{
  GF_ASSERT_MSG( work->isReadyExBgm == TRUE , "Download bgm is not ready!!!\nPlz [SCRIPT_ENUM_BgmReady]!!\n" );
  ARI_TPrintf("PlayLinkSeqWaveData[%d]\n",seqNo);
  work->isPlayExBgm = TRUE;
  PMDSND_PlayExtraMusic(seqNo);
  //NNS_SndArcPlayerStartSeq( SOUNDMAN_GetHierarchyPlayerSndHandle(), SEQ_BGM_MSL_DL_01 );
}

void  STA_ACT_StopBgm( ACTING_WORK *work )
{
  work->isPlayExBgm = FALSE;
  PMDSND_StopExtraMusic();
  //NNS_SndPlayerStopSeq(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0);
  //SOUNDMAN_UnloadHierarchyPlayer();
}

void  STA_ACT_SetBgmLinkNumber(  ACTING_WORK *work , const u16 arcNum , const u16 dstNum , const u16 srcNum )
{
  ARI_TPrintf("LinkSeqWaveData[%d][%d][%d]\n",arcNum,dstNum,srcNum);
  PMDSND_ChangeWaveData( arcNum , dstNum , work->bgmWaveData , srcNum );
}

void  STA_ACT_StartSeqBgm( ACTING_WORK *work , const u32 seqNo )
{
  work->isPlayBgm = TRUE;
  PMSND_PlayBGM( seqNo );
}

void  STA_ACT_StopSeqBgm( ACTING_WORK *work )
{
  work->isPlayBgm = FALSE;
  PMSND_StopBGM( );
}

void STA_ACT_PlayCurtainCloseSe( ACTING_WORK *work )
{
  u8 i;
  u8 minPoint = 255;
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    const u16 point = work->initWork->musPoke[i]->point + work->tempPoint[i];
    if( minPoint > point )
    {
      minPoint = point;
    }
  }
  ARI_TPrintf("Curtain cloase SE point[%d]\n",minPoint);
  if( minPoint >= 70 )
  {
    PMSND_PlaySE( STA_SE_CLAP_3 );
  }
  else
  if( minPoint >= 30 )
  {
    PMSND_PlaySE( STA_SE_CLAP_2 );
  }
  else
  if( minPoint >= 1 )
  {
    PMSND_PlaySE( STA_SE_CLAP_1 );
  }
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
//�A�C�e���̎g�p(��ʐM��
//--------------------------------------------------------------
void STA_ACT_UseItem( ACTING_WORK *work , u8 pokeIdx , MUS_POKE_EQUIP_POS ePos )
{
  work->useItemFlg[pokeIdx] = TRUE;
  work->useItemPos[pokeIdx] = ePos;
}

//--------------------------------------------------------------
//�����̃A�C�e���̎g�p��Ԃ̃`�F�b�N
//--------------------------------------------------------------
const BOOL STA_ACT_IsUsingItemSelf( ACTING_WORK *work )
{
  return STA_ACT_IsUsingItem( work , work->playerIdx );
}
static const BOOL STA_ACT_IsUsingItem( ACTING_WORK *work , const u8 idx )
{
  //���o�ƌ��ʎ��Ԃ𗼕�����
  if( STA_POKE_IsUsingItem( work->pokeSys , work->pokeWork[idx] ) == TRUE )
  {
    return TRUE;
  }
  return work->useItemFlg[idx]; //�������͔�ʐM���̂ݗL��
}
//--------------------------------------------------------------
//�A�C�e���̎g�p�̍X�V
//--------------------------------------------------------------
static void STA_ACT_UpdateUseItem( ACTING_WORK *work )
{
  u8 i;
  u8 usePokeArr[MUSICAL_POKE_MAX];
  u8 usePokeNum = 0;
  
  //NPC�g�p����(�����g�p�����ă��N�G�X�g�҂���Ԃɂ͂����Ȃ�(���肪���G�����邩��
  STA_ACT_UpdateUseItemNpc( work );

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

        if( work->useItemPos[i] == MUS_POKE_EQU_HAND_R )
        {
          work->isUsedItemPos[i][0] = TRUE;
        }
        else
        if( work->useItemPos[i] == MUS_POKE_EQU_HAND_L )
        {
          work->isUsedItemPos[i][1] = TRUE;
        }
        STA_ACT_CheckUseItemNpc( work , ANIT_DISTURB , i );
      }
    }
    if( usePokeNum > 0 )
    {
      //���łɃA�C�e���g���Ă��������FALSE
      if( work->useItemPoke == MUSICAL_POKE_MAX )
      {
        work->isUseItemSolo = TRUE;
      }
      else
      {
        work->isUseItemSolo = FALSE;
      }
      work->useItemCnt = ACT_USEITEM_EFF_TIME;
      work->useItemPoke = usePokeArr[ GFUser_GetPublicRand0(usePokeNum)];
      work->useItemPokePos = work->useItemPos[work->useItemPoke];
      work->isUpdateAttention = TRUE;
    }
  }
  else
  {
    //�ʐM��
    {
      //�ڗ���̏���
      const u8 AttentionIdx = MUS_COMM_GetUseButtonAttention( work->initWork->commWork );
      if( AttentionIdx < MUSICAL_POKE_MAX )
      {
        //���łɃA�C�e���g���Ă��������FALSE
        if( work->useItemPoke == MUSICAL_POKE_MAX )
        {
          work->isUseItemSolo = TRUE;
        }
        else
        {
          work->isUseItemSolo = FALSE;
        }
        work->useItemCnt = ACT_USEITEM_EFF_TIME;
        work->useItemPoke = AttentionIdx;
        work->useItemPokePos = MUS_COMM_GetUseButtonPos( work->initWork->commWork , AttentionIdx );
        work->isUpdateAttention = TRUE;
        MUS_COMM_ResetUseButtonAttention( work->initWork->commWork );
      }
    }
    //�g�p�����S���̏���
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      const u8 usePos = MUS_COMM_GetUseButtonPos( work->initWork->commWork , i );
      if( usePos != MUS_POKE_EQU_INVALID )
      {
        ARI_TPrintf("Use item!![%d][%d]\n",i,usePos);
        STA_POKE_UseItemFunc( work->pokeSys , work->pokeWork[i] , usePos );
        MUS_COMM_ResetUseButtonPos( work->initWork->commWork , i );
        
        if( usePos == MUS_POKE_EQU_HAND_R )
        {
          work->isUsedItemPos[i][0] = TRUE;
        }
        else
        if( usePos == MUS_POKE_EQU_HAND_L )
        {
          work->isUsedItemPos[i][1] = TRUE;
        }
        STA_ACT_CheckUseItemNpc( work , ANIT_DISTURB , i );
      }
    }
  }
  //���Ԕ���
  if( work->useItemCnt > 0 )
  {
    work->useItemCnt--;
    if( work->useItemCnt == 0 )
    {
      //�P�ƃA�s�[���ɐ����I
      if( work->isUseItemSolo == TRUE )
      {
        ARI_TPrintf("<ItemBonus!!Player[%d][%d]>\n",work->useItemPoke,work->useItemPokePos);
        if( work->initWork->commWork == NULL )
        {
          const u32 seNo = STA_ACT_GetUseItemSe( work , work->useItemPoke , work->useItemPokePos );
          work->initWork->musPoke[work->useItemPoke]->isApeerBonus[work->useItemPokePos] = TRUE;
          PMSND_PlaySE( seNo );
        }
        else
        {
          if( GFL_NET_IsParentMachine() == TRUE )
          {
            //�e�̏�ԂŃA�C�e���g�p��]��
            //SE�͑����Ń`�F�b�N���Ȃ�
            const u32 seNo = STA_ACT_GetUseItemSe( work , work->useItemPoke , work->useItemPokePos );
            if( seNo == STA_SE_CLAP_1 )
            {
              MUS_COMM_ReqSendAppealBonusPoke( work->initWork->commWork , work->useItemPoke , work->useItemPokePos , 1 );
            }
            else
            {
              MUS_COMM_ReqSendAppealBonusPoke( work->initWork->commWork , work->useItemPoke , work->useItemPokePos , 2 );
            }
          }
        }
      }
      work->useItemPoke = MUSICAL_POKE_MAX;
      work->isUpdateAttention = TRUE;
      
    }
  }
}

//�A�s�[��������(��薳��)��������SE��炷
static u32 STA_ACT_GetUseItemSe( ACTING_WORK *work , const u8 usePoke , const u8 usePos )
{
  MUS_ITEM_DATA_SYS *itemDataSys = STA_ACT_GetItemDataSys( work );
  const u16 itemNo = work->initWork->musPoke[usePoke]->equip[usePos].itemNo;
  const MUSICAL_CONDITION_TYPE conType = MUS_ITEM_DATA_GetItemConditionType( itemDataSys , itemNo );
  const u8 conPoint = MUSICAL_PROGRAM_GetConOnePoint( work->initWork->progWork , conType );
  
  //���łɑ��₷
  //�{�v�Z�͊O�ł��
  work->tempPoint[usePoke] += conPoint;
  STA_ACT_CalcRank( work );
  //work->initWork->musPoke[usePoke]->point += conPoint;
  if( conPoint >= 7 )
  {
    return STA_SE_CLAP_2;
  }
  else
  {
    return STA_SE_CLAP_1;
  }
}

static void STA_ACT_UpdateUseItemNpc( ACTING_WORK *work )
{
  u8 i;
  if( work->initWork->commWork == NULL ||
      GFL_NET_IsParentMachine() == TRUE )
  {
    //��ʐM���ʐM�e�̂�
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      if( work->npcUseItemCnt[i] > 0 )
      {
        work->npcUseItemCnt[i]--;
        if( work->npcUseItemCnt[i] == 0 )
        {
          //�����I
          if( work->initWork->commWork == NULL )
          {
            STA_ACT_UseItem( work , i , work->npcUseItemPos[i] );
          }
          else
          {
            MUS_COMM_SetReqUseItem_NPC( work->initWork->commWork , i , work->npcUseItemPos[i] );
          }
        }
      }
    }
  }
}

void STA_ACT_CheckUseItemNpc( ACTING_WORK *work , const ACTING_NPC_ITEMUSE_TYPE type , const u8 selfIdx )
{
  u8 i;
  if( work->initWork->commWork == NULL ||
      GFL_NET_IsParentMachine() == TRUE )
  {
    //��ʐM���ʐM�e�̂ݔ���
    for( i=0;i<MUSICAL_POKE_MAX;i++ )
    {
      BOOL isHaveItem[2]={FALSE,FALSE};
      MUS_POKE_EQUIP_POS usePos;
      MUSICAL_POKE_PARAM *musPoke = work->initWork->musPoke[i];
      if( musPoke->charaType != MUS_CHARA_NPC )
      {
        //NPC����Ȃ�
        ARI_TPrintf("NPC[%d] Use item[%d] Not NPC\n",i,type);
        continue;
      }
      if( STA_ACT_IsUsingItem( work , i ) == TRUE )
      {
        //�A�C�e���g�p��
        ARI_TPrintf("NPC[%d] Use item[%d] Not Using item\n",i,type);
        continue;
      }
      if( work->npcUseItemCnt[i] > 0 )
      {
        //�g�p�\��ς�
        ARI_TPrintf("NPC[%d] Use item[%d] Ready use\n",i,type);
        continue;
      }
      //�A�C�e���`�F�b�N
      if( musPoke->equip[MUS_POKE_EQU_HAND_R].itemNo != MUSICAL_ITEM_INVALID &&
          work->isUsedItemPos[i][0] == FALSE )
      {
        isHaveItem[0] = TRUE;
      }
      if( musPoke->equip[MUS_POKE_EQU_HAND_L].itemNo != MUSICAL_ITEM_INVALID &&
          work->isUsedItemPos[i][1] == FALSE )
      {
        isHaveItem[1] = TRUE;
      }
      if( isHaveItem[0] == FALSE && isHaveItem[1] == FALSE )
      {
        //�A�C�e������!
        ARI_TPrintf("NPC[%d] Use item[%d] No item\n",i,type);
        continue;
      }
      else
      if( isHaveItem[0] == TRUE && isHaveItem[1] == FALSE )
      {
        usePos = MUS_POKE_EQU_HAND_R;
      }
      else
      if( isHaveItem[1] == TRUE && isHaveItem[0] == FALSE )
      {
        usePos = MUS_POKE_EQU_HAND_L;
      }
      else
      {
        const u8 rand = GFL_STD_MtRand(2);
        usePos = (rand==0?MUS_POKE_EQU_HAND_R:MUS_POKE_EQU_HAND_L);
      }
      
      switch( type )
      {
      case ANIT_ACTION:  //�l���Z�ɂ��킹��(0.5�`1�b��)
        if( musPoke->npcAppealTime == NPC_APPEAL_TIME_ACTION )
        {
          if( i == selfIdx )
          {
            const u16 cnt = GFL_STD_MtRand(30) + 30;
            work->npcUseItemCnt[i] = cnt;
            work->npcUseItemPos[i] = usePos;
            ARI_TPrintf("NPC[%d] Use item[ACTION][%d]\n",i,cnt);
          }
        }
        break;
      case ANIT_DISTURB: //�W�Q(1�`2�b��)
        if( musPoke->npcAppealTime == NPC_APPEAL_TIME_DISTURB_H )
        {
          if( GFL_STD_MtRand(100) < 50 )  // 1/2�Ŕ���
          {
            const u16 cnt = GFL_STD_MtRand(60) + 60;
            work->npcUseItemCnt[i] = cnt;
            work->npcUseItemPos[i] = usePos;
            ARI_TPrintf("NPC[%d] Use item[DISTURB][%d]\n",i,cnt);
          }
        }
        if( musPoke->npcAppealTime == NPC_APPEAL_TIME_DISTURB_L )
        {
          if( GFL_STD_MtRand(100) < 25 )  // 1/4�Ŕ���
          {
            const u16 cnt = GFL_STD_MtRand(60) + 60;
            work->npcUseItemCnt[i] = cnt;
            work->npcUseItemPos[i] = usePos;
            ARI_TPrintf("NPC[%d] Use item[DISTURB][%d]\n",i,cnt);
          }
        }
        break;
      case ANIT_START:   //�C�ӕb��
        if( musPoke->npcAppealTime > 0 &&
            musPoke->npcAppealTime < NPC_APPEAL_TIME_ACTION )
        {
          const s8  sub = GFL_STD_MtRand(121)-60;
          if( musPoke->npcAppealTime + sub < 0 )
          {
            work->npcUseItemCnt[i] = 0;
          }
          else
          {
            work->npcUseItemCnt[i] = musPoke->npcAppealTime + sub;
          }
          work->npcUseItemPos[i] = usePos;
          ARI_TPrintf("NPC[%d] Use item[START ][%d]\n",i,work->npcUseItemCnt[i]);
        }
        break;
      }
      
    }
  }
}

#pragma mark [> trans effect func
static void STA_ACT_InitTransEffect( ACTING_WORK *work )
{
  u8 i;
//  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  for( i=0;i<1;i++ )
  {
    //�X�e�[�^�X�ŃG�t�F�N�g�̎�ޕς��H
    work->transEffWork[i] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_trans_1_spa );
  }
}

static void STA_ACT_TermTransEffect( ACTING_WORK *work )
{
  u8 i;
//  for( i=0;i<MUSICAL_POKE_MAX;i++ )
    for( i=0;i<1;i++ )
  {
    STA_EFF_DeleteEffect( work->effSys , work->transEffWork[i] );
  }
}

void STA_ACT_PlayTransEffect( ACTING_WORK *work , const u8 idx )
{
  //�G�t�F�N�g�̎�ޕς������Đ��G�~�b�^���ς��B����3��
  u8 i;
  VecFx32 pos;
  //const VecFx32 *ofs = STA_POKE_GetRotOffset( work->pokeSys , work->pokeWork[idx] );
  STA_POKE_GetPosition( work->pokeSys , work->pokeWork[idx] , &pos );
//  pos.x = ACT_POS_X_FX(pos.x + ofs->x );
//  pos.y = ACT_POS_Y_FX(pos.y + ofs->y );
  pos.x = ACT_POS_X_FX(pos.x );
  pos.y = ACT_POS_Y_FX(pos.y + FX32_CONST(-32.0f) );
  pos.z = FX32_CONST(180.0f);
  for( i=0;i<3;i++ )
  {
    //STA_EFF_CreateEmitter( work->transEffWork[idx] , i , &pos );
    STA_EFF_CreateEmitter( work->transEffWork[0] , i , &pos );
  }
  
  PMSND_PlaySE( STA_SE_TRANS_SE );
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
  work->scrollOffsetTrget = scroll;
}

MUS_ITEM_DATA_SYS* STA_ACT_GetItemDataSys( ACTING_WORK *work )
{
  return MUS_ITEM_DRAW_GetItemDataSys( work->itemDrawSys );
}

//�A�C�e���g�p�Ŗڗ����Ă���|�P�����̎擾
const u8 STA_ACT_GetUseItemAttentionPoke( ACTING_WORK *work )
{
  return work->useItemPoke;
}

const u8 STA_ACT_GetPokeEquipPoint( ACTING_WORK *work , const u8 pokeNo )
{
  return work->initWork->musPoke[pokeNo]->point;
}
const u8 STA_ACT_GetPokeTempPoint( ACTING_WORK *work , const u8 pokeNo )
{
  return work->tempPoint[pokeNo];
}

void STA_ACT_SetLockScroll( ACTING_WORK *work , const BOOL flg )
{
  work->lockScroll = flg;
}
void STA_ACT_SetForceScroll( ACTING_WORK *work , const BOOL flg )
{
  work->forceScroll = flg;
}
#pragma mark [> editor func
//--------------------------------------------------------------
//�G�f�B�^�p
//--------------------------------------------------------------
void	STA_ACT_EDITOR_SetEditorMode( ACTING_WORK *work )
{
  work->isEditorMode = TRUE;
}

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

static void STA_ACT_DebugPolyDraw( ACTING_WORK *work )
{
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    VecFx32 pos;
    G3_PushMtx();
    //�J�����ݒ�擾
    {
      MtxFx33       mtxBillboard;
      VecFx16       vecN;
      VecFx32   vecNtmp;
      MtxFx43   mtxCamera, mtxCameraInv;
      static const VecFx32 cam_pos = MUSICAL_CAMERA_POS;
      static const VecFx32 cam_target = MUSICAL_CAMERA_TRG;
      static const VecFx32 cam_up = MUSICAL_CAMERA_UP;
    
      G3_LookAt( &cam_pos, &cam_up, &cam_target, &mtxCamera );  //mtxCamera�ɂ͍s��v�Z���ʂ��Ԃ�
      MTX_Inverse43( &mtxCamera, &mtxCameraInv );     //�J�����t�s��擾
      MTX_Copy43To33( &mtxCameraInv, &mtxBillboard );   //�J�����t�s�񂩂��]�s������o��

      VEC_Subtract( &cam_pos, &cam_target, &vecNtmp );
      VEC_Normalize( &vecNtmp, &vecNtmp );
      VEC_Fx16Set( &vecN, vecNtmp.x, vecNtmp.y, vecNtmp.z );
    }
    {
      static u8 polyCol = 0;
      static fx32 polyOfs = FX32_CONST(100.0f);
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
      {
        polyOfs += FX32_ONE;
        ARI_Printf( "PolyOfs[%.1f]\n",FX_FX32_TO_F32(polyOfs));
      }
      if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
      {
        polyOfs -= FX32_ONE;
        ARI_Printf( "PolyOfs[%.1f]\n",FX_FX32_TO_F32(polyOfs));
      }
      //�|��
      G3_PolygonAttr(GX_LIGHTMASK_NONE,       // no lights
               GX_POLYGONMODE_MODULATE,     // modulation mode
               GX_CULL_NONE,          // cull back
               0,                // polygon ID(0 - 63)
               31,     // alpha(0 - 31)
               0                  // OR of GXPolygonAttrMisc's value
               );
      G3_TexImageParam( GX_TEXFMT_NONE ,
                        GX_TEXGEN_NONE ,
                        0 ,
                        0 ,
                        0 ,
                        0 ,
                        GX_TEXPLTTCOLOR0_USE ,
                        0 );
      pos.x = MUSICAL_POS_X(128.0f);
      pos.y = MUSICAL_POS_Y(96.0f);
      pos.z = polyOfs;

      G3_Translate( pos.x, pos.y, pos.z );

      {
        const fx32 size = FX32_ONE;
        G3_Begin(GX_BEGIN_QUADS);
        G3_Color( GX_RGB( polyCol, polyCol, polyCol ) );
        polyCol++;
        if( polyCol > 31 )
        {
          polyCol = 0;
        }

        G3_Vtx( FX32_CONST(-7.8), FX32_CONST( 5.8), 0 );
        G3_Vtx( FX32_CONST( 7.8), FX32_CONST( 5.8), 0 );
        G3_Vtx( FX32_CONST( 7.8), FX32_CONST(-5.8), 0 );
        G3_Vtx( FX32_CONST(-7.8), FX32_CONST(-5.8), 0 );

        G3_End();
      }
    }
    G3_PopMtx(1);
    
  }
#endif //DEB_ARI
  
}
