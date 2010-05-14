//======================================================================
/**
 * @file  musical_shot_sys.h
 * @brief �~���[�W�J���̋L�O�ʐ^Proc
 * @author  ariizumi
 * @data  09/09/14
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"

#include "musical/musical_system.h"
#include "musical/musical_shot_sys.h"
#include "musical/mus_shot_photo.h"
#include "mus_shot_info.h"

#include "poke_tool/monsno_def.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================
enum  //���C���V�[�P���X
{
  MUS_SHOT_SEQ_INIT,
  MUS_SHOT_SEQ_LOOP,
  MUS_SHOT_SEQ_TERM,
};

//======================================================================
//  typedef struct
//======================================================================

typedef struct
{
  HEAPID heapId;
  GFL_TCB   *vblankFuncTcb;
  
  MUS_SHOT_INIT_WORK *shotInitWork;
  
  MUS_SHOT_PHOTO_WORK *photoWork;
  MUS_SHOT_INFO_WORK *infoWork;
}SHOT_LOCAL_WORK;

//======================================================================
//  proto
//======================================================================

static GFL_PROC_RESULT MusicalShotProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalShotProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MusicalShotProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static void MUSICAL_SHOT_VBlankFunc(GFL_TCB *tcb,void *work);
static void MUSICAL_SHOT_InitGraphic( SHOT_LOCAL_WORK *work );
static void MUSICAL_SHOT_ExitGraphic( SHOT_LOCAL_WORK *work );

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
//  C �e�N�X�`��
//  D MainBg
//  E �e�N�X�`���p���b�g
//  F MainObj
//  G MainBgExPlt
//  H SubBG
//  I SubOBJ

GFL_PROC_DATA MusicalShot_ProcData =
{
  MusicalShotProc_Init,
  MusicalShotProc_Main,
  MusicalShotProc_Term
};

//�f�o�b�O�p
FS_EXTERN_OVERLAY(musical);
//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalShotProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  SHOT_LOCAL_WORK *work;
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MUSICAL_SHOT, 0x60000 );

  work = GFL_PROC_AllocWork( proc, sizeof(SHOT_LOCAL_WORK), HEAPID_MUSICAL_SHOT );
  work->heapId = HEAPID_MUSICAL_SHOT;
  ARI_TPrintf("MUS_SHOT_DATA_SIZE[%d]\n",sizeof(MUSICAL_SHOT_DATA));
  if( pwk == NULL )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID(musical) );
    work->shotInitWork = GFL_HEAP_AllocMemory( HEAPID_MUSICAL_SHOT , sizeof( MUS_SHOT_INIT_WORK ));
    work->shotInitWork->musShotData = GFL_HEAP_AllocClearMemory( HEAPID_MUSICAL_SHOT , sizeof( MUSICAL_SHOT_DATA ));
    work->shotInitWork->isLoadOverlay = FALSE;
    {
      u8 i;
      RTCDate date;
      MUSICAL_SHOT_DATA *shotData = work->shotInitWork->musShotData;
      GFL_RTC_GetDate( &date );
      shotData->bgNo = 1;
      shotData->spotBit = 2;
      shotData->year = date.year;
      shotData->month = date.month;
      shotData->day = date.day;
      shotData->title[0] = L'�|';
      shotData->title[1] = L'�P';
      shotData->title[2] = L'�b';
      shotData->title[3] = L'�^';
      shotData->title[4] = L'�[';
      shotData->title[5] = L'��';
      shotData->title[6] = L'��';
      shotData->title[7] = L'��';
      shotData->title[8] = L'�X';
      shotData->title[9] = L'�^';
      shotData->title[10] = L'�[';
      shotData->title[11] = L'��';
      shotData->title[12] = GFL_STR_GetEOMCode();

      shotData->shotPoke[0].monsno = MONSNO_PIKATYUU;
      shotData->shotPoke[1].monsno = MONSNO_PIKUSII;
      shotData->shotPoke[2].monsno = MONSNO_PURUNSU;
      shotData->shotPoke[3].monsno = MONSNO_509;
      
      for( i=0;i<MUSICAL_POKE_MAX;i++ )
      {
        u8 j;
        shotData->shotPoke[i].trainerName[0] = L'�g';
        shotData->shotPoke[i].trainerName[1] = L'��';
        shotData->shotPoke[i].trainerName[2] = L'�[';
        shotData->shotPoke[i].trainerName[3] = L'�i';
        shotData->shotPoke[i].trainerName[4] = L'�P'+i;
        shotData->shotPoke[i].trainerName[5] = 0;
        
        for( j=0;j<MUSICAL_ITEM_EQUIP_MAX;j++ )
        {
          shotData->shotPoke[i].equip[j].itemNo = MUSICAL_ITEM_INVALID;
          shotData->shotPoke[i].equip[j].angle = 0;
          shotData->shotPoke[i].equip[j].equipPos = MUS_POKE_EQU_INVALID;
        }
        shotData->shotPoke[i].equip[0].itemNo = 11;
        shotData->shotPoke[i].equip[0].angle = 0;
        shotData->shotPoke[i].equip[0].equipPos = MUS_POKE_EQU_HAND_R;
        shotData->shotPoke[i].equip[1].itemNo = 11;
        shotData->shotPoke[i].equip[1].angle = 0;
        shotData->shotPoke[i].equip[1].equipPos = MUS_POKE_EQU_HAND_L;
        shotData->shotPoke[i].equip[2].itemNo = 11;
        shotData->shotPoke[i].equip[2].angle = 0;
        shotData->shotPoke[i].equip[2].equipPos = MUS_POKE_EQU_WAIST;
        shotData->shotPoke[i].equip[3].itemNo = 11;
        shotData->shotPoke[i].equip[3].angle = 0;
        shotData->shotPoke[i].equip[3].equipPos = MUS_POKE_EQU_FACE;
        shotData->shotPoke[i].equip[4].itemNo = 11;
        shotData->shotPoke[i].equip[4].angle = 0;
        shotData->shotPoke[i].equip[4].equipPos = MUS_POKE_EQU_HEAD;
        shotData->shotPoke[i].equip[5].itemNo = 11;
        shotData->shotPoke[i].equip[5].angle = 0;
        shotData->shotPoke[i].equip[5].equipPos = MUS_POKE_EQU_EAR_L;
        shotData->shotPoke[i].equip[6].itemNo = 11;
        shotData->shotPoke[i].equip[6].angle = 0;
        shotData->shotPoke[i].equip[6].equipPos = MUS_POKE_EQU_EAR_R;
      }
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      work->shotInitWork->isCheckMode = TRUE;
    }
    else
    {
      work->shotInitWork->isCheckMode = FALSE;
    }
    work->shotInitWork->musicalSave = NULL;
  }
  else
  {
    work->shotInitWork = pwk;
  }
  
  if( work->shotInitWork->isLoadOverlay == TRUE )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID(musical) );
  }
  
  work->vblankFuncTcb = GFUser_VIntr_CreateTCB( MUSICAL_SHOT_VBlankFunc , (void*)work , 64 );


  MUSICAL_SHOT_InitGraphic( work );

  work->photoWork = MUS_SHOT_PHOTO_InitSystem( work->shotInitWork->musShotData , work->heapId );
  work->infoWork = MUS_SHOT_INFO_InitSystem( work->shotInitWork->musShotData , 
                                             work->shotInitWork->musicalSave , 
                                             work->shotInitWork->isCheckMode ,
                                             work->shotInitWork->commWork ,
                                             work->heapId );
  
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalShotProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  SHOT_LOCAL_WORK *work = mywk;
  if( WIPE_SYS_EndCheck() == FALSE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE , GX_BLEND_PLANEMASK_NONE , 31 , 31 );
  
  MUS_SHOT_INFO_ExitSystem( work->infoWork );
  MUS_SHOT_PHOTO_ExitSystem( work->photoWork );
  MUSICAL_SHOT_ExitGraphic( work );

  GFL_TCB_DeleteTask( work->vblankFuncTcb );

  if( work->shotInitWork->isLoadOverlay == TRUE )
  {
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(musical) );
  }
  
  if( pwk == NULL )
  {
    GFL_HEAP_FreeMemory( work->shotInitWork->musShotData );
    GFL_HEAP_FreeMemory( work->shotInitWork );

    GFL_OVERLAY_Unload( FS_OVERLAY_ID(musical) );

  }

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_MUSICAL_SHOT );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
static GFL_PROC_RESULT MusicalShotProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  SHOT_LOCAL_WORK *work = mywk;

  MUS_SHOT_PHOTO_UpdateSystem( work->photoWork );
  MUS_SHOT_INFO_UpdateSystem( work->infoWork );

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  if( MUS_SHOT_INFO_IsFinish(work->infoWork) == TRUE )
  {
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
//  VBLank Function
//------------------------------------------------------------------
static void MUSICAL_SHOT_VBlankFunc(GFL_TCB *tcb,void *work)
{
  GFL_CLACT_SYS_VBlankFunc();
}


//--------------------------------------------------------------
//  �`��n������
//--------------------------------------------------------------
static void MUSICAL_SHOT_InitGraphic( SHOT_LOCAL_WORK *work )
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
  
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &sys_data );
  }
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &vramBank ,work->heapId );
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
}

//--------------------------------------------------------------
//  �`��n�J��
//--------------------------------------------------------------
static void MUSICAL_SHOT_ExitGraphic( SHOT_LOCAL_WORK *work )
{
  GFL_CLACT_SYS_Delete();
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

