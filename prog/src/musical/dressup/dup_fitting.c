//======================================================================
/**
 * @file  dressup_system.h
 * @brief �h���X�A�b�v �������C��
 * @author  ariizumi
 * @data  09/02/10
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "sound/pm_sndsys.h"

#include "arc_def.h"
#include "musical_item.naix"
#include "dressup_gra.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "savedata/musical_save.h"
#include "musical/musical_local.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"
#include "musical/mus_item_data.h"
#include "musical/musical_camera_def.h"
#include "dup_local_def.h"
#include "dup_fitting.h"
#include "dup_fitting_item.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define fitting

#define FIT_FRAME_MAIN_3D   GFL_BG_FRAME0_M
#define FIT_FRAME_MAIN_INFO   GFL_BG_FRAME1_M
#define FIT_FRAME_MAIN_CASE   GFL_BG_FRAME2_M //�ǂݑւ��ċ��p�H��������3D
#define FIT_FRAME_MAIN_MIRROR GFL_BG_FRAME2_M //�ǂݑւ��ċ��p�H
#define FIT_FRAME_MAIN_BG   GFL_BG_FRAME3_M
#define FIT_FRAME_SUB_BG    GFL_BG_FRAME3_S
#define FIT_FRAME_SUB_CURTAIN_L    GFL_BG_FRAME2_S
#define FIT_FRAME_SUB_CURTAIN_R    GFL_BG_FRAME1_S
#define FIT_FRAME_SUB_TOP    GFL_BG_FRAME0_S

#define FIT_PAL_INFO    (0xE)

#define FIT_ANIME_SPD (6)
#define FIT_ANIME_MAX (192)

#define DEG_TO_U16(val) ((val)*0x10000/360)
#define U16_TO_DEG(val) ((val)*360/0x10000)

#define FIT_CHECK_BUTTON_TOP    (192-24)
#define FIT_CHECK_BUTTON_BOTTOM (192)
#define FIT_CHECK_BUTTON_LEFT   (128-32)
#define FIT_CHECK_BUTTON_RIGHT  (128+32)

//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define FIT_POS_X(val)    MUSICAL_POS_X(val)  
#define FIT_POS_Y(val)    MUSICAL_POS_Y(val)  
#define FIT_POS_X_FX(val) MUSICAL_POS_X_FX(val)
#define FIT_POS_Y_FX(val) MUSICAL_POS_Y_FX(val)

//�|�P�����\���ʒu
static const int FIT_POKE_POS_X = 128;
static const int FIT_POKE_POS_Y = 112;
static const fx32 FIT_POKE_POS_X_FX = FIT_POS_X( FIT_POKE_POS_X );
static const fx32 FIT_POKE_POS_Y_FX = FIT_POS_Y( FIT_POKE_POS_Y );
static const fx32 FIT_POKE_POS_Z_FX = FX32_CONST(40.0f);

//�A�C�e���\����
static const u16 ITEM_LIST_NUM = 14;
//static const u16 ITEM_LIST_NUM = 48;
static const u16 ITEM_FIELD_NUM = 64;
static const u16 ITEM_EQUIP_NUM = MUS_POKE_EQUIP_MAX;//9
static const u16 ITEM_DISP_NUM = ITEM_LIST_NUM+ITEM_FIELD_NUM+ITEM_EQUIP_NUM+1; //�y���Ŏ���

//�A�C�e�����X�g�n��`
//���S�_
static const int LIST_CENTER_X = 128;
static const int LIST_CENTER_Y = 80;
static const int LIST_CENTER_X_FX = FX32_CONST(LIST_CENTER_X);
static const int LIST_CENTER_Y_FX = FX32_CONST(LIST_CENTER_Y);
//�T�C�Y
static const u16 LIST_SIZE_X = 72;
static const u16 LIST_SIZE_Y = 48;
static const float LIST_SIZE_RATIO = ((float)LIST_SIZE_Y/(float)LIST_SIZE_X);

//�T�C�Y
static const int LIST_TPHIT_MAX_X = LIST_SIZE_X+20;
static const int LIST_TPHIT_MIN_X = LIST_SIZE_X-20;
static const int LIST_TPHIT_MAX_Y = LIST_SIZE_Y+20;
static const int LIST_TPHIT_MIN_Y = LIST_SIZE_Y-20;
static const float LIST_TPHIT_RATIO_MAX = (float)LIST_TPHIT_MAX_Y/(float)LIST_TPHIT_MAX_X;
static const float LIST_TPHIT_RATIO_MIN = (float)LIST_TPHIT_MIN_Y/(float)LIST_TPHIT_MIN_X;
//�k�����n�܂�ʒu(90�x)/�����n�߂�ʒu(155�x)/������ʒu(170�x)
static const u16 LIST_SIZE_DEPTH[3] = {0x4000,0x6E00,0x78E0};
//���X�g�̉�]����
static const u16 LIST_ROTATE_ANGLE = DEG_TO_U16(1.25);
static const u16 LIST_ROTATE_LIMIT = DEG_TO_U16(90);

//�A�C�e��1�̊Ԋu
static const u16 LIST_ONE_ANGLE = 0x10000/ITEM_LIST_NUM;
    //�A�C�e���̃}�b�N�X���ϓ�����̂Ŕp�~
//static const u32 LIST_FULL_ANGLE = LIST_ONE_ANGLE*MUSICAL_ITEM_MAX;

//���X�g��Z���W
static const fx32 LIST_DEPTH_BASE = FX32_CONST( 20.0f );

//�t�B�[���h(�u���Ă���)�A�C�e���n
static const fx32 FIELD_ITEM_DEPTH = FX32_CONST(10.0f); //���X�g��0�`8

//�����Ă�A�C�e���n
static const fx32 HOLD_ITEM_DEPTH = FX32_CONST(60.0f);
static const fx32 RETURN_LIST_ITEM_DEPTH = FX32_CONST(30.0f);

//�����A�C�e���n
static const u16 HOLD_ITEM_SNAP_LENGTH = 12;
static const fx32 EQUIP_ITEM_DEPTH = FX32_CONST(50.5f);
static const fx32 EQUIP_ITEM_DEPTH_BACK = FX32_CONST(20.5f);


//���X�g�ɖ߂�A�j���[�V����
static const u16 ITEM_RETURN_ANIME_CNT = 30;
static const fx32 ITEM_RETURN_DEPTH = FX32_CONST(70.0f);
//�ړ��L�����Z���Ńt�B�[���h�ɖ߂�Ƃ�
static const u16 ITEM_RETURN_POS_CNT = 5;
//�\���A�C�e���̓_�ŊԊu
static const u8 ITEM_LIST_NEW_BLINK = 45;


//�\�����ʃ���
//��
//�ێ��A�C�e��      10
//List�߂蒆       9
//�����i         8.5
//�A�C�e�����X�g    8�`0
//�t�B�[���h�A�C�e��  -10�`-10-0.1*�\����
//�|�P����        -40000  //���Ԃ�mcss�ō��W�ϊ��������Ă�E�E�E����ň�ԉ��ɏo��
//��

//�X�N���[���n
#define DUP_CURTAIN_SCROLL_MAX (128)
#define DUP_CURTAIN_SCROLL_VALUE (8)

#pragma mark [> define check
//�`�F�b�N���
#define BUTTON_SIZE_X (64)
#define BUTTON_SIZE_Y (64)

#define BUTTON_ASSEPT_POS_X (BUTTON_SIZE_X/2)
#define BUTTON_ASSEPT_POS_Y (192-BUTTON_SIZE_Y/2)
#define BUTTON_RETURN_POS_X (256-BUTTON_SIZE_X/2)
#define BUTTON_RETURN_POS_Y (192-BUTTON_SIZE_Y/2)

#pragma mark [> define demo
#define DEMO_ROT_ITEM_NUM (5)

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  DUS_FITTING_DEMO,
  DUS_FITTING_MAIN,
  DUS_GO_CHECK,
  DUS_CHECK_MAIN,
  DUS_RETURN_FITTING,
  
  DUS_FADEIN_WAIT,
  DUS_FADEOUT_WAIT,
  
}DUP_STATE;

typedef enum
{
  IG_NONE,
  IG_LIST,
  IG_FIELD,
  IG_EQUIP,
  IG_ANIME,
}ITEM_GROUPE;

typedef enum
{
  DOR_BUTTON_PLT,
  DOR_BUTTON_NCG,
  DOR_BUTTON_ANM,
  
  DUP_OBJ_RES_MAX,
}DUP_OBJ_RES;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct

struct _FITTING_WORK
{
  HEAPID heapId;
  FITTING_INIT_WORK *initWork;
  MUS_POKE_DATA_WORK  *pokeData;
  GFL_TCB *vBlankTcb;
  
  DUP_STATE state;
  int     animeCnt; 
  
  //���ʃJ�[�e���n
  u8      scrollCnt;
  BOOL    isOpenCurtain;
  
  //TP�n
  BOOL tpIsTrg;
  BOOL tpIsTouch;
  u32 tpx,tpy;
  u32 befTpx,befTpy;
  u16 befAngle;
  BOOL befAngleEnable;  //���̕ϐ��̗L����
  BOOL isOpenList;    //���X�g�̕\�����
  MUS_POKE_EQUIP_POS  snapPos;  //�����ɋz�����Ă���
  
  FIT_ITEM_WORK *holdItem;  //�ێ����Ă���A�C�e��
  ITEM_GROUPE   holdItemType;
  ITEM_GROUPE   befItemType;
  GFL_POINT     befItemPos;

  MUS_ITEM_DRAW_WORK *shadowItem; //�e�p
  u16         shadowItemId;
  
  //new�̕����p
  u32         newPicResIdx;
  u32         newPicBbdIdx[ITEM_LIST_NUM];
  u16         newPicBlinkCnt;

  //�\�[�g�n
  BOOL isSortAnime;   //�\�[�g������
  u32  sortAnimeMoveAngle;  //�\�[�g�A�j���ړ���
  u32  sortAnimeEndAngle;   //�\�[�g�A�j�������ʒu
  MUS_POKE_EQUIP_USER sortType; //���̃\�[�g���

  //�A�C�e���n
  u16             totalItemNum;
  ITEM_STATE      itemStateBase[MUSICAL_ITEM_MAX];
  ITEM_STATE      *itemState[MUSICAL_ITEM_MAX];
  FIT_ITEM_GROUP  *itemGroupList;
  FIT_ITEM_GROUP  *itemGroupField;
  FIT_ITEM_GROUP  *itemGroupEquip;
  //���X�g�ɖ߂�Ƃ��ȂǑ���s�ŃA�j������ł��镨
  FIT_ITEM_GROUP  *itemGroupAnime;

  u16 listAngle;
  s16 listSpeed;
  u16 listHoldMove; //�����Ă���Ƃ��̈ړ���
  s32 listTotalMove;  //���X�g�S�̂̈ړ���
  
  //�f���p
  BOOL isDemo;
  u16  demoCnt;
  u8   demoPhase;
  u8   demoMoveValue; //��]�������(�A�C�e������
  
  //Obj�n
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK  *buttonCell[2];
  GFL_CLWK  *demoPen;
  u32     objResIdx[DUP_OBJ_RES_MAX];

  
  //3D�`��Ɋւ�镨
  MUS_POKE_DRAW_SYSTEM  *drawSys;
  MUS_POKE_DRAW_WORK    *drawWork;
  MUS_ITEM_DRAW_SYSTEM  *itemDrawSys;
  GFL_G3D_CAMERA      *camera;
  GFL_BBD_SYS       *bbdSys;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto Fitting

static void DUP_FIT_FittingMain(  FITTING_WORK *work );

static void DUP_FIT_VBlankFunc(GFL_TCB *, void * );

static void DUP_FIT_SetupGraphic( FITTING_WORK *work );
static void DUP_FIT_SetupBgObj( FITTING_WORK *work );
static void DUP_FIT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );
static void DUP_FIT_SetupPokemon( FITTING_WORK *work );
static void DUP_FIT_SetupItem( FITTING_WORK *work );
static void DUP_FIT_SetupStartItem( FITTING_WORK *work , const u16 itemId );
static void DUP_FIT_TermItem( FITTING_WORK *work );
static void DUP_FIT_CalcItemListAngle( FITTING_WORK *work , u16 angle , s16 moveAngle , u16 sizeX , u16 sizeY );
static void DUP_FIT_CheckItemListPallet( FITTING_WORK *work );

static void DUP_FIT_UpdateTpMain( FITTING_WORK *work );
static void DUP_FIT_UpdateTpList( FITTING_WORK *work , s16 subX , s16 subY );
static void DUP_FIT_UpdateTpHoldItem( FITTING_WORK *work );
static void DUP_FIT_UpdateTpHoldingItem( FITTING_WORK *work );
static void DUP_FIT_UpdateTpDropItemToField( FITTING_WORK *work );
static void DUP_FIT_UpdateTpDropItemToList( FITTING_WORK *work , const BOOL isMove );
static void DUP_FIT_UpdateTpDropItemToEquip(  FITTING_WORK *work );
static void DUP_FIT_UpdateTpCancelDropItem( FITTING_WORK *work );
static void DUP_FIT_UpdateTpPokeCheck( FITTING_WORK *work );
static void DUP_FIT_UpdateSortAnime( FITTING_WORK *work );

static const BOOL DUP_FIT_CheckIsEquipItem( FITTING_WORK *work , const MUS_POKE_EQUIP_POS pos);
static MUS_POKE_EQUIP_POS DUP_FIT_SearchEquipPosition( FITTING_WORK *work , MUS_ITEM_DRAW_WORK *itemDrawWork , GFL_POINT *pos , u16 *len );
static void DUP_FIT_UpdateItemAnime( FITTING_WORK *work );
static void DUP_FIT_UpdateShadow( FITTING_WORK *work );
static void DUP_FIT_CreateItemListToField( FITTING_WORK *work );
static void DUP_FIT_SortItemIdx( FITTING_WORK *work , const MUS_POKE_EQUIP_USER ePos , const u16 startIdx );
static const u16 DUP_FIT_CalcSortPriority( FITTING_WORK *work , ITEM_STATE *itemState , const MUS_POKE_EQUIP_USER ePos );

static BOOL DUP_FIT_CheckPointInOval( s16 subX , s16 subY , s16 size , float ratioYX );

static void DUP_FIT_ChangeStateAnime( FITTING_WORK *work );

#pragma mark [> proto Check
static FITTING_RETURN DUP_CHECK_CheckMain( FITTING_WORK *work );
static void DUP_CHECK_UpdateTpMain( FITTING_WORK *work );
static void DUP_CHECK_UpdateTpHoldingItem( FITTING_WORK *work );
static void DUP_CHECK_ResetItemAngle( FITTING_WORK *work );
static void DUP_CHECK_SaveNowEquip( FITTING_WORK *work );

#pragma mark [> proto Demo
static void DUP_DEMO_DemoMain( FITTING_WORK *work );
static void DUP_DEMO_DemoStart( FITTING_WORK *work );
static void DUP_DEMO_DemoEnd( FITTING_WORK *work );

static void DUP_DEMO_DemoPhaseListRot( FITTING_WORK *work , const u16 start , const s32 value , const u16 cnt);
static void DUP_DEMO_DemoPhaseWait( FITTING_WORK *work , const u16 cnt );
static void DUP_DEMO_DemoPhaseDragPen( FITTING_WORK *work , const GFL_POINT *start , const GFL_POINT *end , const u16 cnt );

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_D,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
  GX_VRAM_OBJ_64_E,       // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
  GX_VRAM_SUB_OBJ_NONE,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_01_AB,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_01_FG,      // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};
//  A �e�N�X�`��
//  B �e�N�X�`��
//  C SubBg
//  D MainBg
//  E MainObj
//  F �e�N�X�`���p���b�g
//  G �e�N�X�`���p���b�g
//  H None
//  I None


//--------------------------------------------------------------
//  �������C�� ������
//--------------------------------------------------------------
FITTING_WORK* DUP_FIT_InitFitting( FITTING_INIT_WORK *initWork , HEAPID heapId )
{
  FITTING_WORK *work = GFL_HEAP_AllocClearMemory( heapId , sizeof( FITTING_WORK ));

  work->heapId = heapId;
  work->initWork = initWork;
  work->state = DUS_FADEIN_WAIT;
  work->animeCnt = 0;
  work->tpIsTrg = FALSE;
  work->tpIsTouch = FALSE;
  work->befAngleEnable = FALSE;
  work->holdItem = NULL;
  work->holdItemType = IG_NONE;
  work->befItemType = IG_NONE;
  work->isSortAnime = FALSE;
  work->sortType = MUS_POKE_EQUIP_USER_MAX;
  work->listAngle = 0;
  work->isDemo = FALSE;
  work->scrollCnt = 0;
  work->isOpenCurtain = FALSE;

  work->listSpeed = 0;
  work->snapPos = MUS_POKE_EQU_INVALID;
  DUP_FIT_SetupGraphic( work );
  DUP_FIT_SetupBgObj( work );
  DUP_FIT_SetupPokemon( work );
  DUP_FIT_SetupItem( work );
  
  INFOWIN_Init( FIT_FRAME_MAIN_INFO,FIT_PAL_INFO,NULL,work->heapId);
  work->vBlankTcb = GFUser_VIntr_CreateTCB( DUP_FIT_VBlankFunc , work , 8 );
  
  WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                  WIPE_FADE_WHITE , 18 , WIPE_DEF_SYNC , work->heapId );
  
  return work;
}

//--------------------------------------------------------------
//  �������C�� �J��
//--------------------------------------------------------------
void  DUP_FIT_TermFitting( FITTING_WORK *work )
{
  //�t�F�[�h�Ȃ��̂ŉ�����
  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  
  INFOWIN_Exit();

  GFL_CLACT_WK_Remove( work->buttonCell[0] );
  GFL_CLACT_WK_Remove( work->buttonCell[1] );
  GFL_CLACT_WK_Remove( work->demoPen );
  GFL_CLGRP_PLTT_Release( work->objResIdx[DOR_BUTTON_PLT] );
  GFL_CLGRP_CGR_Release( work->objResIdx[DOR_BUTTON_NCG] );
  GFL_CLGRP_CELLANIM_Release( work->objResIdx[DOR_BUTTON_ANM] );
  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();

  DUP_FIT_TermItem( work );
  MUS_POKE_DRAW_Del( work->drawSys , work->drawWork );
  MUS_POKE_DRAW_TermSystem( work->drawSys );

  GFL_TCB_DeleteTask( work->vBlankTcb );
  GFL_BBD_DeleteSys( work->bbdSys );
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();
  GFL_BG_FreeBGControl( FIT_FRAME_MAIN_3D );
  GFL_BG_FreeBGControl( FIT_FRAME_MAIN_INFO );
  GFL_BG_FreeBGControl( FIT_FRAME_MAIN_CASE );
  GFL_BG_FreeBGControl( FIT_FRAME_MAIN_BG );
  GFL_BG_FreeBGControl( FIT_FRAME_SUB_TOP );
  GFL_BG_FreeBGControl( FIT_FRAME_SUB_CURTAIN_L );
  GFL_BG_FreeBGControl( FIT_FRAME_SUB_CURTAIN_R );
  GFL_BG_FreeBGControl( FIT_FRAME_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//  �������C�� ���[�v
//--------------------------------------------------------------
FITTING_RETURN  DUP_FIT_LoopFitting( FITTING_WORK *work )
{
  FITTING_RETURN ret = FIT_RET_CONTINUE;
  switch( work->state )
  {
  case DUS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( DUP_FIT_ITEMGROUP_GetItemNum(work->itemGroupField) == 0 )
      {
        //�t�B�[���h�ɃA�C�e������������f��
        DUP_DEMO_DemoStart( work );
      }
      else
      {
        work->state = DUS_FITTING_MAIN;
      }
    }
    break;
    
  case DUS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return FIT_RET_GO_END;
    }
    break;
    
  case DUS_FITTING_DEMO:
    DUP_DEMO_DemoMain(work);
    break;

  case DUS_FITTING_MAIN:
    DUP_FIT_FittingMain(work);
    break;

  case DUS_GO_CHECK:
    work->animeCnt += FIT_ANIME_SPD;
    if( work->animeCnt >= FIT_ANIME_MAX )
    {
      work->animeCnt = FIT_ANIME_MAX;
      work->state = DUS_CHECK_MAIN;
    }
    DUP_FIT_ChangeStateAnime(work);
    
    break;

  case DUS_RETURN_FITTING:
    work->animeCnt -= FIT_ANIME_SPD;
    if( work->animeCnt < 0 )
    {
      work->animeCnt = 0;
      work->state = DUS_FITTING_MAIN;
    }
    DUP_FIT_ChangeStateAnime(work);
    
    break;

  case DUS_CHECK_MAIN:
    if( DUP_CHECK_CheckMain( work ) == FIT_RET_GO_END )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_WHITE , 18 , WIPE_DEF_SYNC , work->heapId );
      work->state = DUS_FADEOUT_WAIT;
      
    }
    break;
  }

  DUP_FIT_UpdateItemAnime( work );
  MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 
  MUS_ITEM_DRAW_UpdateSystem( work->itemDrawSys ); 
  
  INFOWIN_Update();

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  //3D�`��  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
    GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
  }
  GFL_G3D_DRAW_End();


  //�X�N���[���n
  GFL_BG_SetScrollReq( FIT_FRAME_MAIN_BG , GFL_BG_SCROLL_X_INC , 1 );
  GFL_BG_SetScrollReq( FIT_FRAME_MAIN_BG , GFL_BG_SCROLL_Y_DEC , 1 );
  
  if( work->isOpenCurtain == FALSE )
  {
    if( work->scrollCnt < DUP_CURTAIN_SCROLL_MAX )
    {
      work->scrollCnt += DUP_CURTAIN_SCROLL_VALUE;
      GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_L , GFL_BG_SCROLL_X_SET , 128 - work->scrollCnt );
      GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_R , GFL_BG_SCROLL_X_SET , -128 + work->scrollCnt );
    }
  }
  else
  {
    if( work->scrollCnt > 0 )
    {
      work->scrollCnt -= DUP_CURTAIN_SCROLL_VALUE;
      GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_L , GFL_BG_SCROLL_X_SET , 128 - work->scrollCnt );
      GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_R , GFL_BG_SCROLL_X_SET , -128 + work->scrollCnt );
    }
  }

#if DEB_ARI
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT &&
    GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
  {
    return FIT_RET_GO_END;
  }
#endif //DEB_ARI
  return ret;
}

static void DUP_FIT_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  FITTING_WORK *work = wk;
  FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
  BOOL isChangeItem = FALSE;
  while( item != NULL )
  {
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    const ITEM_STATE *itemState = DUP_FIT_ITEM_GetItemState( item );
    ITEM_STATE *newItemState = DUP_FIT_ITEM_GetNewItemState( item );
    
    if( itemState->itemId != newItemState->itemId )
    {
      DUP_FIT_ITEM_ChengeGraphic( item, work->itemDrawSys , newItemState );
      isChangeItem = TRUE;
    }
    
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
  if( isChangeItem == TRUE )
  {
    DUP_FIT_CheckItemListPallet( work );
  }

  if( work->holdItem != NULL )
  {
    const ITEM_STATE *itemState = DUP_FIT_ITEM_GetItemState( work->holdItem );
    if( work->shadowItemId != itemState->itemId )
    {
      work->shadowItemId = itemState->itemId;
      
      MUS_ITEM_DRAW_ChengeGraphic( work->itemDrawSys , work->shadowItem , itemState->itemId , itemState->itemRes );
      MUS_ITEM_DRAW_SetShadowPallet( work->itemDrawSys , work->shadowItem , itemState->itemRes );
    }
  }
  GFL_CLACT_SYS_VBlankFunc();
  MUS_ITEM_DRAW_UpdateSystem_VBlank(work->itemDrawSys);
}



//--------------------------------------------------------------
//  �`��n������
//--------------------------------------------------------------
static void DUP_FIT_SetupGraphic( FITTING_WORK *work )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
  GFL_DISP_SetBank( &vramBank );
  

  { //3D�n�̐ݒ�
    static const VecFx32 cam_pos = MUSICAL_CAMERA_POS;
    static const VecFx32 cam_target = MUSICAL_CAMERA_TRG;
    static const VecFx32 cam_up = MUSICAL_CAMERA_UP;
    //�G�b�W�}�[�L���O�J���[
    static  const GXRgb stage_edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT80K,
            0, work->heapId, NULL );
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
#if 0 //�����ˉe�J����
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
                       FX32_SIN13,
                       FX32_COS13,
                       FX_F32_TO_FX32( 1.33f ),
                       NULL,
                       FX32_ONE,
                       FX32_ONE * 180,
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
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
  }
  
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );
  
  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    
    // BG1 MAIN (Info�o�[
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000,0x1000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (�W�E��
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x6000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG0 SUB (�g�b�v�̃J�[�e��
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x0000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 SUB (�J�[�e���E
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000,0x0000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (�J�[�e����
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x08000,0x0000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );
    GFL_BG_SetBGControl3D( 1 );
    DUP_FIT_SetupBgFunc( &header_main1, FIT_FRAME_MAIN_INFO);
    DUP_FIT_SetupBgFunc( &header_main2, FIT_FRAME_MAIN_CASE);
    DUP_FIT_SetupBgFunc( &header_main3, FIT_FRAME_MAIN_BG);

    DUP_FIT_SetupBgFunc( &header_sub3 , FIT_FRAME_SUB_BG );
    DUP_FIT_SetupBgFunc( &header_sub0 , FIT_FRAME_SUB_TOP );
    DUP_FIT_SetupBgFunc( &header_sub1 , FIT_FRAME_SUB_CURTAIN_R );
    DUP_FIT_SetupBgFunc( &header_sub2 , FIT_FRAME_SUB_CURTAIN_L );
    
    GFL_BG_SetVisible( FIT_FRAME_MAIN_3D , TRUE );
  }

  { //OBJ�̏�����
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.oamst_main = 0x10;  //�f�o�b�O���[�^�̕�
    cellSysInitData.oamnum_main = 128-0x10;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    work->cellUnit  = GFL_CLACT_UNIT_Create( 3 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
  }

  {
    GFL_BBD_SETUP bbdSetup = {
      128,128,
      {FX32_ONE,FX32_ONE,0},
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      0
    };
    //�r���{�[�h�V�X�e���\�z
    work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
  }
}

//--------------------------------------------------------------
//  BG�̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupBgObj( FITTING_WORK *work )
{
  
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_DRESSUP_GRA , work->heapId );

  //�����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_dressup_gra_test_bg_d_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_dressup_gra_test_bg_NCGR ,
                    FIT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_test_bg_d_NSCR , 
                    FIT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_dressup_gra_bg_mirror_NCGR ,
                    FIT_FRAME_MAIN_MIRROR , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_bg_mirror_NSCR , 
                    FIT_FRAME_MAIN_MIRROR ,  0 , 0, FALSE , work->heapId );

  //����
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_dressup_gra_test_bg_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_dressup_gra_test_bg_u3_NCGR ,
                    FIT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_test_bg_u3_NSCR , 
                    FIT_FRAME_SUB_TOP ,  0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_test_bg_u2_2_NSCR , 
                    FIT_FRAME_SUB_CURTAIN_L ,  0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_test_bg_u2_NSCR , 
                    FIT_FRAME_SUB_CURTAIN_R ,  0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_dressup_gra_test_bg_u_NSCR , 
                    FIT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );

  GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_L , GFL_BG_SCROLL_X_SET ,  128 );
  GFL_BG_SetScrollReq( FIT_FRAME_SUB_CURTAIN_R , GFL_BG_SCROLL_X_SET , -128 );

  //Obj�pRes
  work->objResIdx[DOR_BUTTON_PLT] = GFL_CLGRP_PLTT_Register( arcHandle , NARC_dressup_gra_obj_main_NCLR , CLSYS_DRAW_MAIN , 0 , work->heapId  );
  work->objResIdx[DOR_BUTTON_NCG] = GFL_CLGRP_CGR_Register( arcHandle , NARC_dressup_gra_obj_main_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->objResIdx[DOR_BUTTON_ANM] = GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_dressup_gra_obj_main_NCER , NARC_dressup_gra_obj_main_NANR, work->heapId  );
  
  {
  //�Z���̐���
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = BUTTON_ASSEPT_POS_X;
    cellInitData.pos_y = BUTTON_ASSEPT_POS_Y+BUTTON_SIZE_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    work->buttonCell[0] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->objResIdx[DOR_BUTTON_NCG],
              work->objResIdx[DOR_BUTTON_PLT],
              work->objResIdx[DOR_BUTTON_ANM],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    cellInitData.pos_x = BUTTON_RETURN_POS_X;
    cellInitData.pos_y = BUTTON_RETURN_POS_Y+BUTTON_SIZE_Y;
    cellInitData.anmseq = 1;
    work->buttonCell[1] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->objResIdx[DOR_BUTTON_NCG],
              work->objResIdx[DOR_BUTTON_PLT],
              work->objResIdx[DOR_BUTTON_ANM],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    cellInitData.pos_x = 128;
    cellInitData.pos_y = 96;
    cellInitData.anmseq = 2;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    work->demoPen = GFL_CLACT_WK_Create( work->cellUnit ,
              work->objResIdx[DOR_BUTTON_NCG],
              work->objResIdx[DOR_BUTTON_PLT],
              work->objResIdx[DOR_BUTTON_ANM],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetDrawEnable( work->buttonCell[0], TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->buttonCell[1], TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->demoPen, FALSE );
  }

  GFL_ARC_CloseDataHandle(arcHandle);

  GFL_BG_LoadScreenReq(FIT_FRAME_MAIN_BG);
  GFL_BG_LoadScreenReq(FIT_FRAME_MAIN_MIRROR);
  GFL_BG_LoadScreenReq(FIT_FRAME_SUB_BG);


}
//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void DUP_FIT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------
//  �\���|�P�����̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupPokemon( FITTING_WORK *work )
{
  VecFx32 pos = {FIT_POKE_POS_X_FX,FIT_POKE_POS_Y_FX,FIT_POKE_POS_Z_FX};
//  VecFx32 pos = {FX32_ONE*10,FX32_ONE*10,FX32_ONE*-7};  //�ʒu�͓K��
  work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
  work->drawWork = MUS_POKE_DRAW_Add( work->drawSys , work->initWork->musPoke );
  work->pokeData = MUS_POKE_DRAW_GetPokeData( work->drawWork);
  MUS_POKE_DRAW_SetPosition( work->drawWork , &pos);
  
}

//--------------------------------------------------------------
//�A�C�e���̏�����
//--------------------------------------------------------------
static void DUP_FIT_SetupItem( FITTING_WORK *work )
{
  int i;
  //�����A�C�e���̃`�F�b�N
  work->totalItemNum = 0;
  for( i=0;i<MUSICAL_ITEM_MAX;i++ )
  {
    //  FIXME:�����������A�C�e���`�F�b�N
#if DEB_ARI
    if( i<33 )
#else
    if( i<33 )
#endif
    {
      work->itemStateBase[work->totalItemNum].itemId = i;
      work->itemStateBase[work->totalItemNum].isOutList = FALSE;
      if( i<33 && GFL_STD_MtRand0(4) == 0 )
      {
        work->itemStateBase[work->totalItemNum].isNew = TRUE;
      }
      else
      {
        work->itemStateBase[work->totalItemNum].isNew = FALSE;
      }
      work->itemStateBase[work->totalItemNum].pltWork = NULL;
      work->totalItemNum++;
    }
  }
  //�c��ɋ�f�[�^���߂�
  i=0;
  while( work->totalItemNum+i < MUSICAL_ITEM_MAX )
  {
    work->itemStateBase[work->totalItemNum+i].itemId = MUSICAL_ITEM_INVALID;
    work->itemStateBase[work->totalItemNum+i].isOutList = FALSE;
    work->itemStateBase[work->totalItemNum].isNew = TRUE;
    work->itemStateBase[work->totalItemNum].pltWork = NULL;
    i++;
  }
  
  //�O���t�B�b�N�n������
  work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , ITEM_DISP_NUM , work->heapId );
  for( i=0;i<work->totalItemNum;i++ )
  {
    work->itemStateBase[i].itemRes = MUS_ITEM_DRAW_LoadResource( work->itemStateBase[i].itemId );
  }
  //�f�t�H���g�Ń\�[�g
  DUP_FIT_SortItemIdx( work , MUS_POKE_EQUIP_USER_MAX , 0 );
  
  work->itemGroupList = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , ITEM_LIST_NUM );
  work->itemGroupField = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , ITEM_FIELD_NUM );
  work->itemGroupEquip = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , MUS_POKE_EQUIP_MAX );
  work->itemGroupAnime = DUP_FIT_ITEMGROUP_CreateGroup( work->heapId , 4 );
  for( i=0;i<ITEM_LIST_NUM;i++ )
  {
    FIT_ITEM_WORK* item;
    VecFx32 pos = {0,0,0};
    
    item = DUP_FIT_ITEM_CreateItem( work->heapId , work->itemDrawSys ,
                                    &work->itemStateBase[0], &pos );
    DUP_FIT_ITEMGROUP_AddItem( work->itemGroupList,item );
    MUS_ITEM_DRAW_SetUseOffset( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork( item ) , FALSE );
  }
  //�e�p
  {
    VecFx32 pos = {0,0,0};
    work->shadowItem = MUS_ITEM_DRAW_AddResource( work->itemDrawSys , work->itemStateBase[0].itemId , work->itemStateBase[0].itemRes , &pos );
    MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , work->shadowItem , FALSE );
    MUS_ITEM_DRAW_SetSize( work->itemDrawSys , work->shadowItem , FX16_ONE , FX16_ONE );
    work->shadowItemId = 0;
  }

  //New�̕\���p
  {
    u8 i;
    work->newPicResIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_DRESSUP_GRA , 
                      NARC_dressup_gra_new_str_nsbtx , GFL_BBD_TEXFMT_PAL4 ,
                      GFL_BBD_TEXSIZ_32x32 , 32 , 32 );
    for( i=0;i<ITEM_LIST_NUM;i++ )
    {
      const VecFx32 pos ={0,0,0};
      const BOOL flg = FALSE;
      work->newPicBbdIdx[i] = GFL_BBD_AddObject( work->bbdSys , work->newPicResIdx ,
                                  FX32_ONE, FX32_ONE , &pos , 31 , GFL_BBD_LIGHT_NONE );
    	GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->newPicBbdIdx[i] , &flg );
    }

    work->newPicBlinkCnt = 0;
  }
  
  //�Z�[�u����A�C�e�����U�炩��
  {
    u8 i;
    MUSICAL_EQUIP_SAVE *mus_bef_save = MUSICAL_SAVE_GetBefEquipData( work->initWork->mus_save );
    
    for( i=0;i<MUSICAL_ITEM_EQUIP_MAX;i++ )
    {
      if( mus_bef_save->equipData[i].pos != MUS_POKE_EQU_INVALID )
      {
        DUP_FIT_SetupStartItem( work , mus_bef_save->equipData[i].data.itemNo );
      }
    }
  }

  
  //����]�̈ʒu����X�^�[�g
  work->listTotalMove = (LIST_ONE_ANGLE*work->totalItemNum)-0x8000; 
  DUP_FIT_CalcItemListAngle( work , 0 , 0 , LIST_SIZE_X , LIST_SIZE_Y );
  work->isOpenList = TRUE;

}

static void DUP_FIT_SetupStartItem( FITTING_WORK *work , const u16 itemId )
{
  u8 i;
  ITEM_STATE *itemState = NULL;
  
  for( i=0;i<work->totalItemNum;i++ )
  {
    if( work->itemStateBase[i].itemId == itemId )
    {
      itemState = &work->itemStateBase[i];
      break;
    }
  }
  if( itemState == NULL )
  {
    //�A�C�e�����̂Ă�ꂽ�E�E�E�H
    OS_TPrintf("ItemId[%d] is not found...\n",itemId);
    return;
  }
  
  while( TRUE )
  {
    GFL_POINT pos;
    GFL_POINT subPos;
    BOOL hitOval;
    
    pos.x = 8  +GFL_STD_MtRand0(256-( 8+ 8));
    pos.y = 32 +GFL_STD_MtRand0(192-(32+32));
    
    subPos.x = pos.x - LIST_CENTER_X;
    subPos.y = pos.y - LIST_CENTER_Y;
    
    hitOval = DUP_FIT_CheckPointInOval( subPos.x,subPos.y,LIST_TPHIT_MAX_Y,LIST_TPHIT_RATIO_MAX );
    if( hitOval == FALSE )
    {
      FIT_ITEM_WORK* item;
      VecFx32 vecPos;
      MUS_ITEM_DRAW_WORK *itemDrawWork;

      vecPos.x = FX32_CONST(pos.x);
      vecPos.y = FX32_CONST(pos.y);
      vecPos.z = FIELD_ITEM_DEPTH - FX32_CONST(0.1f)*DUP_FIT_ITEMGROUP_GetItemNum(work->itemGroupField);
      
      item = DUP_FIT_ITEM_CreateItem( work->heapId , work->itemDrawSys , itemState , &vecPos );
      DUP_FIT_ITEMGROUP_AddItem( work->itemGroupField,item );
      DUP_FIT_ITEM_SetPosition( item , &pos );

      itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
      MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork ,FX16_ONE,FX16_ONE );

      //�A�C�e�����������Ԃ�
      itemState->isOutList = TRUE;
      break;
    }
  }
}


static void DUP_FIT_TermItem( FITTING_WORK *work )
{
  int i;
  FIT_ITEM_WORK *nextItem;
  FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
  while( item != NULL )
  {
    nextItem = DUP_FIT_ITEM_GetNextItem(item);
    DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
    item = nextItem;
  }

  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
  while( item != NULL )
  {
    nextItem = DUP_FIT_ITEM_GetNextItem(item);
    DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
    item = nextItem;
  }

  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  while( item != NULL )
  {
    nextItem = DUP_FIT_ITEM_GetNextItem(item);
    DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
    item = nextItem;
  }

  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupAnime );
  while( item != NULL )
  {
    nextItem = DUP_FIT_ITEM_GetNextItem(item);
    DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
    item = nextItem;
  }
  
  //New�p
  for( i=0;i<ITEM_LIST_NUM;i++ )
  {
    GFL_BBD_RemoveObject( work->bbdSys , work->newPicBbdIdx[i] );
  }
  GFL_BBD_RemoveResource( work->bbdSys , work->newPicResIdx );

  //�e�p
  MUS_ITEM_DRAW_DelItem( work->itemDrawSys , work->shadowItem );


  for( i=0;i<work->totalItemNum;i++ )
  {
    MUS_ITEM_DRAW_DeleteResource( work->itemStateBase[i].itemRes );
  }
  
  DUP_FIT_ITEMGROUP_DeleteGroup( work->itemGroupList );
  DUP_FIT_ITEMGROUP_DeleteGroup( work->itemGroupField );
  DUP_FIT_ITEMGROUP_DeleteGroup( work->itemGroupEquip );
  DUP_FIT_ITEMGROUP_DeleteGroup( work->itemGroupAnime );
  
  MUS_ITEM_DRAW_TermSystem( work->itemDrawSys );
}

#pragma mark [> FittingFunc

static void DUP_FIT_FittingMain(  FITTING_WORK *work )
{

#if DEB_ARI
  if(GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    work->listAngle += 0x100;
    DUP_FIT_CalcItemListAngle( work , work->listAngle , -0x100 , LIST_SIZE_X , LIST_SIZE_Y );
  }
  if(GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    work->listAngle -= 0x100;
    DUP_FIT_CalcItemListAngle( work , work->listAngle , +0x100 , LIST_SIZE_X , LIST_SIZE_Y );
  }
  if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
    OS_TPrintf("---DumpResIdx---\n");
    while( item != NULL )
    {
      MUS_ITEM_DRAW_Debug_DumpResData( work->itemDrawSys,DUP_FIT_ITEM_GetItemDrawWork( item ) );

      item = DUP_FIT_ITEM_GetNextItem(item);
    }
    MUS_ITEM_DRAW_Debug_DumpResData( work->itemDrawSys,work->shadowItem );
    OS_TPrintf("---DumpResIdx---\n");
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A &&
      work->isDemo == FALSE )
  {
    //�f�o�O�p�Ƀf�t�H���g�ʒu�ɖ߂��ăX�^�[�g
    //����]�̈ʒu����X�^�[�g
    work->listAngle = 0;
    work->listTotalMove = (LIST_ONE_ANGLE*work->totalItemNum)-0x8000; 
    DUP_FIT_CalcItemListAngle( work , 0 , 0 , LIST_SIZE_X , LIST_SIZE_Y );

    DUP_DEMO_DemoStart( work );
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
  {
    OS_TPrintf("[%d:%d]\n",work->tpx,work->tpy);
  }

#endif

  if( work->isSortAnime == TRUE )
  {
    DUP_FIT_UpdateSortAnime( work );
  }
  else
  {
    DUP_FIT_UpdateTpMain( work );
  }
  DUP_FIT_UpdateShadow( work );

  //New!�̓_�ōX�V
  work->newPicBlinkCnt++;
  if( work->newPicBlinkCnt >= ITEM_LIST_NEW_BLINK*2 )
  {
    work->newPicBlinkCnt = 0;
    DUP_FIT_CalcItemListAngle( work , work->listAngle , 0 , LIST_SIZE_X , LIST_SIZE_Y );
  }
  if( work->newPicBlinkCnt == ITEM_LIST_NEW_BLINK )
  {
    DUP_FIT_CalcItemListAngle( work , work->listAngle , 0 , LIST_SIZE_X , LIST_SIZE_Y );
  }
  
  //VBlank�ɂ��Plt�̓]�����I����Ă����炱���Ń��������J��
  //VBlank�����Ɗ��荞�݂��|���̂ł����ŁE�E�E
  if( NNS_GfdGetVramTransferTaskTotalSize() == 0 )
  {
    int i;
    for( i=0;i<work->totalItemNum;i++ )
    {
      if(work->itemStateBase[i].pltWork != NULL )
      {
        GFL_HEAP_FreeMemory( work->itemStateBase[i].pltWork );
        work->itemStateBase[i].pltWork = NULL;
      }
    }
  }

}


//--------------------------------------------------------------
//�A�C�e�����X�g�̈ʒu�E�T�C�Y�v�Z
//--------------------------------------------------------------
static void DUP_FIT_CalcItemListAngle( FITTING_WORK *work , u16 angle , s16 moveAngle , u16 sizeX , u16 sizeY )
{
  //�~�͉�����O��0�x�Ƃ��āAdepth�͉���0x8000 �オ0x0000
  int i = 0;
  FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
  
  work->listTotalMove += moveAngle;
  
  if( work->listTotalMove < 0 )
  {
    work->listTotalMove += (LIST_ONE_ANGLE*work->totalItemNum);
  }
  if( work->listTotalMove >= (LIST_ONE_ANGLE*work->totalItemNum) )
  {
    work->listTotalMove -= (LIST_ONE_ANGLE*work->totalItemNum);
  }
  

  while( item != NULL )
  {
    GFL_POINT dispPos;
    VecFx32 pos;
    u16 oneAngle = (i*0x10000/ITEM_LIST_NUM+angle)%0x10000;
    u16 depth;
    fx16 size = 0;
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    const fx32 sin = (fx32)FX_SinIdx( oneAngle );
    const fx32 cos = (fx32)FX_CosIdx( oneAngle );
    const fx32 posX = LIST_CENTER_X_FX+sin*sizeX;
    const fx32 posY = LIST_CENTER_Y_FX+cos*sizeY;
    ITEM_STATE *nowItemState = DUP_FIT_ITEM_GetItemState( item );
    ITEM_STATE *newItemState = nowItemState;
    
    //�G�̓ǂݑւ������Ƃ�
    {
      u16 newIdx;
      s32 subAngle;
      s32 itemTotalAngle;

      subAngle = (oneAngle + 0x8000 )%0x10000;
      itemTotalAngle = (work->listTotalMove+subAngle);
      while( itemTotalAngle < 0 )
      {
        itemTotalAngle += (LIST_ONE_ANGLE*work->totalItemNum);
      }
      while( itemTotalAngle >= (LIST_ONE_ANGLE*work->totalItemNum) )
      {
        itemTotalAngle -= (LIST_ONE_ANGLE*work->totalItemNum);
      }
      if( work->isSortAnime == FALSE ||
          work->sortAnimeMoveAngle > (0x10000-oneAngle+0x8000)%0x10000 )
      {
        newIdx = (itemTotalAngle+(LIST_ONE_ANGLE/2))/LIST_ONE_ANGLE;
        //�v�Z�덷���N����E�E�E(�A�C�e�������X�g�̌�(14)�ȉ����ƋN����
        if( newIdx >= work->totalItemNum )
        {
          newIdx -= work->totalItemNum;
        }
        newItemState = work->itemState[newIdx];
        DUP_FIT_ITEM_SetNewItemState( item , newItemState );
      }
    }
    
    //�ʒu�̌v�Z
    if( oneAngle < 0x8000 )
    {
      depth = oneAngle;
    }
    else
    {
      depth = 0x8000-(oneAngle-0x8000);
    }
    dispPos.x = F32_CONST(posX);
    dispPos.y = F32_CONST(posY);
    pos.x = posX;
    pos.y = posY;
    pos.z = LIST_DEPTH_BASE + (0x8000-depth);
  
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
    DUP_FIT_ITEM_SetPosition( item , &dispPos );
    //�T�C�Y�̌v�Z
    {
      if( depth < LIST_SIZE_DEPTH[0] )
      {
        size = FX16_ONE;
        MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , FX16_ONE , FX16_ONE );
        MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
        DUP_FIT_ITEM_SetScale( item , FX32_ONE );
      }
      else if( depth < LIST_SIZE_DEPTH[1] )
      {
        //100%��50%
        const u16 subAngle = LIST_SIZE_DEPTH[1]-depth;
        size = (FX16_CONST(0.5f)*subAngle/(LIST_SIZE_DEPTH[1]-LIST_SIZE_DEPTH[0]))+FX16_CONST(0.5f);
        MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , size,size );
        MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
        DUP_FIT_ITEM_SetScale( item , size );
      }
      else if( depth < LIST_SIZE_DEPTH[2] )
      {
        //50%��0%
        const u16 subAngle = LIST_SIZE_DEPTH[2]-depth;
        size = FX16_CONST(0.5f)*subAngle/(LIST_SIZE_DEPTH[2]-LIST_SIZE_DEPTH[1]);
        MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork , size,size );
        MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , TRUE );
        DUP_FIT_ITEM_SetScale( item , size );
      }
      else
      {
        MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , itemDrawWork , FALSE );
        DUP_FIT_ITEM_SetScale( item , 0 );
        size = 0;
      }
    }
    
    //new!����
    if( newItemState->isNew == TRUE &&
        work->newPicBlinkCnt < ITEM_LIST_NEW_BLINK )
    {
      const BOOL isShow = TRUE;
      VecFx32 newPos;
      newPos.x = MUSICAL_POS_X_FX(pos.x);
      newPos.y = MUSICAL_POS_Y_FX(pos.y + FX32_CONST(8.0f));
      newPos.z = pos.z+0x600; //0x10000/14�����0x1200�Ȃ̂ł��̔���
      GFL_BBD_SetObjectTrans( work->bbdSys , work->newPicBbdIdx[i] , &newPos );
      GFL_BBD_SetObjectSiz( work->bbdSys , work->newPicBbdIdx[i] , &size,&size );
      GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->newPicBbdIdx[i] , &isShow );
    }
    else
    {
      const BOOL isShow = FALSE;
      GFL_BBD_SetObjectDrawEnable( work->bbdSys , work->newPicBbdIdx[i] , &isShow );
    }
    
    item = DUP_FIT_ITEM_GetNextItem(item);
    i++;
  }
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
}

//--------------------------------------------------------------
//���X�g�̃A�C�e���Ó]����
//--------------------------------------------------------------
static void DUP_FIT_CheckItemListPallet( FITTING_WORK *work )
{
  FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );

  while( item != NULL )
  {
    ITEM_STATE *nowItemState = DUP_FIT_ITEM_GetItemState( item );
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    
    //�e������
    if( nowItemState->isOutList == TRUE )
    {
      MUS_ITEM_DRAW_SetDarkPallet( work->itemDrawSys , itemDrawWork , nowItemState->itemRes , &nowItemState->pltWork );
    }
    else
    {
      MUS_ITEM_DRAW_ResetShadowPallet( work->itemDrawSys , itemDrawWork , nowItemState->itemRes );
    }
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
}

//--------------------------------------------------------------
//�^�b�`�y������X�V���C��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpMain( FITTING_WORK *work )
{
  BOOL isTouchList = FALSE;
  //�f���̏ꍇ�͊O���琔�l�������Ă���
  if( work->isDemo == FALSE )
  {
    work->tpIsTrg = GFL_UI_TP_GetTrg();
    work->tpIsTouch = GFL_UI_TP_GetPointCont( &work->tpx,&work->tpy );
  }
  if( work->tpIsTrg || work->tpIsTouch )
  {
    if( work->tpIsTrg && 
        work->tpy > FIT_CHECK_BUTTON_TOP &&
        work->tpx > FIT_CHECK_BUTTON_LEFT &&
        work->tpx < FIT_CHECK_BUTTON_RIGHT )
    {
      //�`�F�b�N�{�^��
      work->state = DUS_GO_CHECK;
      work->animeCnt = 0;
      PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    }
    else
    {
      BOOL hitMinOval = FALSE;
      BOOL hitMaxOval = FALSE;
      
      //��荇����InfoBar�ƃ`�F�b�N�{�^���ɂ��Ԃ�Ȃ�����
      if( work->tpy > 192-16 )
      {
        work->tpy = 192-16;
      }
      if( work->tpy < 16 )
      {
        work->tpy = 16;
      }
      
      if( work->holdItemType == IG_FIELD && work->holdItem != NULL )
      {
        //�A�C�e���ێ����̏���
        DUP_FIT_UpdateTpHoldingItem( work );
      }
      else if( work->isOpenList == TRUE )
      {
        //�A�C�e�����X�g�̒����H
        //X���W�ɂ͔��������Y�̃T�C�Y�Ōv�Z����
        const s16 subX = work->tpx - LIST_CENTER_X;
        const s16 subY = work->tpy - LIST_CENTER_Y;
        const u32 centerLen = F32_CONST(FX_Sqrt(FX32_CONST(subX*subX+subY*subY)));
        //�����̉~�ɋ��邩�H
        hitMinOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MIN_Y,LIST_TPHIT_RATIO_MIN );
        hitMaxOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MAX_Y,LIST_TPHIT_RATIO_MAX );
        if( hitMinOval == FALSE && hitMaxOval == TRUE )
        {
          isTouchList = TRUE;
          DUP_FIT_UpdateTpList( work , subX , subY );
        }
        else if( (hitMinOval == FALSE && hitMaxOval == FALSE )||
             (hitMinOval == TRUE && hitMaxOval == TRUE ))
        {
          //���X�g�����������o������
          if( DUP_FIT_ITEMGROUP_IsItemMax(work->itemGroupField) == FALSE )
          {
            //�~�̓���������
            if( work->holdItemType == IG_LIST &&
              work->holdItem != NULL && 
              MATH_ABS(work->listSpeed) < DEG_TO_U16(2) )
            {
              ARI_TPrintf("[%d]\n",U16_TO_DEG(MATH_ABS(work->listSpeed)));
              DUP_FIT_CreateItemListToField( work );
            }
          }
        }
      }
      if( isTouchList == FALSE && work->tpIsTrg == TRUE )
      {
        //���X�g�E�t�B�[���h����E������
        DUP_FIT_UpdateTpHoldItem(work);
        
        //�\�[�g����
        if( hitMinOval == TRUE && work->holdItem == NULL )
        {
          DUP_FIT_UpdateTpPokeCheck( work );
        }

      }
    }
  }
  else
  {
    //�A�C�e�����X�g�̒����H
    //X���W�ɂ͔��������Y�̃T�C�Y�Ōv�Z����
    const s16 subX = work->befTpx - LIST_CENTER_X;
    const s16 subY = work->befTpy - LIST_CENTER_Y;
    const u32 centerLen = F32_CONST(FX_Sqrt(FX32_CONST(subX*subX+subY*subY)));
    //�����̉~�ɋ��邩�H
    BOOL hitMinOval,hitMaxOval;
    hitMinOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MIN_Y,LIST_TPHIT_RATIO_MIN );
    hitMaxOval = DUP_FIT_CheckPointInOval( subX,subY,LIST_TPHIT_MAX_Y,LIST_TPHIT_RATIO_MAX );
    if( work->holdItemType == IG_FIELD && work->holdItem != NULL )
    {
      if( work->snapPos != MUS_POKE_EQU_INVALID )
      {
        DUP_FIT_UpdateTpDropItemToEquip( work );
      }
      else
      if( hitMinOval == FALSE && hitMaxOval == TRUE &&
        work->isOpenList == TRUE )
      {
        DUP_FIT_UpdateTpDropItemToList( work , FALSE );
      }
      else
      if( hitMinOval == TRUE )
      {
        DUP_FIT_UpdateTpCancelDropItem( work );
      }
      else
      {
        DUP_FIT_UpdateTpDropItemToField( work );
      }
    }
    
    work->befAngleEnable = FALSE;
    work->holdItem = NULL;
    work->holdItemType = IG_NONE;
    work->befItemType = IG_NONE;
    work->snapPos = MUS_POKE_EQU_INVALID;
  }
  //�����ŉ��`
  if( work->listSpeed != 0 && isTouchList == FALSE )
  {
    s32 tempAngle = (s32)work->listAngle+0x10000-work->listSpeed;
    if( tempAngle < 0 )
      tempAngle += 0x10000;
    if( tempAngle >= 0x10000 )
      tempAngle -= 0x10000;
    work->listAngle = tempAngle;
    DUP_FIT_CalcItemListAngle( work , work->listAngle , work->listSpeed , LIST_SIZE_X , LIST_SIZE_Y );
    if( work->listSpeed > 0x80 || work->listSpeed < -0x80 )
    {
      work->listSpeed *= 0.75f;
    }
    else
    { 
      work->listSpeed = 0;
    }
  }
  work->befTpx = work->tpx;
  work->befTpy = work->tpy;
  
}
//--------------------------------------------------------------
//�^�b�`�y������X�V���X�g
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpList( FITTING_WORK *work , s16 subX , s16 subY )
{
  //��]�͑ȉ~�Ȃ̂Ŋp�x�Ō��Ȃ��ق����ǂ�����
  //�ł��p�x����Ȃ��Ɖ񂹂Ȃ��E�E�E
  const u16 angle = FX_Atan2Idx(FX32_CONST(subX),FX32_CONST(subY));
  const s32 subAngle = work->befAngle - angle;

  work->listHoldMove += MATH_ABS(subAngle);
  
  //���݃`�F�b�N
  //�ړ����������̂ŃA�C�e���ێ�����
  if( work->tpIsTrg == TRUE ||
    work->listHoldMove >= DEG_TO_U16(10) )
  {
    FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupList );
    //���łɏ�����
    work->listSpeed = 0;
    work->listHoldMove = 0;
    work->holdItem = NULL;

    while( item != NULL )
    {
      const ITEM_STATE *nowItemState = DUP_FIT_ITEM_GetItemState( item );
      //�T�C�Y���C�ɂ��邩�H
//      if( DUP_FIT_ITEM_GetScale( item ) == FX32_ONE &&
//        DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
      if( DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE &&
          nowItemState->isOutList == FALSE )
      {
        work->holdItem = item;
        work->holdItemType = IG_LIST;
        //DUP_FIT_ITEM_CheckLengthSqrt( item , work->tpx,work->tpy );
        item = NULL;
      }
      else
      {
        item = DUP_FIT_ITEM_GetNextItem(item);
      }
    }
  }
  //��]����
  if( work->befAngleEnable == TRUE )
  {
    u8 i;
    //�O��Ƃ̃��X�g�ɑ΂���p�x��
    //�O��̍��W����ǂ����ɓ��������H�̊p�x
    //const u16 moveAngle = FX_Atan2Idx((work->tpx-work->befTpx)*FX32_ONE,(work->tpy-work->befTpy)*FX32_ONE);
    //��]�`�F�b�N
    if( MATH_ABS(subAngle) > LIST_ROTATE_ANGLE &&
      MATH_ABS(subAngle) < LIST_ROTATE_LIMIT )
    {
      s32 tempAngle = (s32)work->listAngle+0x10000-subAngle;
      if( tempAngle < 0 )
        tempAngle += 0x10000;
      if( tempAngle >= 0x10000 )
        tempAngle -= 0x10000;
      work->listAngle = tempAngle;
      DUP_FIT_CalcItemListAngle( work , work->listAngle , subAngle , LIST_SIZE_X , LIST_SIZE_Y );
      
      if( ( work->listSpeed>0 && work->listSpeed<0) ||
        ( work->listSpeed<0 && work->listSpeed>0) )
      {
        work->listSpeed = 0;
      }
      else
      {
        work->listSpeed = subAngle;
      }
/*
      //�ړ�����͂�����悤�ɏC���B�������A�A�C�e���͑I�тȂ������߁A���Trg�����Ɠ���
      work->listHoldMove += MATH_ABS(subAngle);
      if( work->listHoldMove >= DEG_TO_U16(10) )
      {
        //�ړ����������̂ŃA�C�e���ێ�����
        work->holdItem = NULL;
        work->holdItemType = IG_NONE;
      }
*/
    }
    else
    {
      work->listSpeed = 0;
    }
  }
  work->befAngle = angle;
  work->befAngleEnable = TRUE;
}

//--------------------------------------------------------------
//�t�B�[���h�E�����̃A�C�e�����E��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpHoldItem( FITTING_WORK *work )
{
  BOOL isEquipList = TRUE;
  FIT_ITEM_WORK* item;
  
  //�����A�C�e���̃��X�g�𒲂ׂ���ɁA�������[�`���Ńt�B�[���h�̃��X�g�𒲂ׂ�
  
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  if( item == NULL || DUP_FIT_ITEMGROUP_IsItemMax(work->itemGroupField) == TRUE )
  {
    //�t�B�[���h�̃A�C�e���������ς��Ȃ瑕���A�C�e���͏E���Ȃ�
    //���t�B�[���h�̃��X�g��
    item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
    isEquipList = FALSE;
  }
  while( item != NULL )
  {
    if( DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
    {
      const GFL_POINT *itemPos = DUP_FIT_ITEM_GetPosition( item );
      work->holdItem = item;
      work->holdItemType = IG_FIELD;
      //DUP_FIT_ITEM_CheckLengthSqrt( item , work->tpx,work->tpy );
      ARI_TPrintf("Item Hold[%d]\n",DUP_FIT_ITEM_GetItemState(item)->itemId);
      
      //�߂����p�̏���
      if( isEquipList == TRUE )
      {
        work->befItemType = IG_EQUIP;
        work->befItemPos.x = itemPos->x;
        work->befItemPos.y = itemPos->y;
      }
      else
      {
        work->befItemType = IG_FIELD;
        work->befItemPos.x = itemPos->x;
        work->befItemPos.y = itemPos->y;
      }

      item = NULL;
    }
    else
    {
      item = DUP_FIT_ITEM_GetNextItem(item);
      if( item == NULL && isEquipList == TRUE )
      {
        //�����̃��X�g���Ȃ��Ȃ����̂Ńt�B�[���h�̃��X�g�ɐ؂�ւ�
        item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
        isEquipList = FALSE;
      }
    }
    
  }
  
  if( isEquipList == TRUE && item != work->holdItem )
  {
    DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupEquip , work->holdItem );
    DUP_FIT_ITEMGROUP_AddItem( work->itemGroupField , work->holdItem );
    //���W���킹�Ɛ[�x�ݒ�
    DUP_FIT_UpdateTpHoldingItem( work );
  }

}

//--------------------------------------------------------------
//�t�B�[���h�̃A�C�e���������Ă���
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpHoldingItem( FITTING_WORK *work )
{
  u16 snapLen = HOLD_ITEM_SNAP_LENGTH;
  GFL_POINT dispPos;
  GFL_POINT pokePosSub;
  VecFx32 pos = {0,0,HOLD_ITEM_DEPTH};
  fx16 scaleX,scaleY;
  u16 rotZ = 0;
  MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  
  pokePosSub.x = work->tpx - FIT_POKE_POS_X;
  pokePosSub.y = work->tpy - FIT_POKE_POS_Y;
  work->snapPos = DUP_FIT_SearchEquipPosition( work ,itemDrawWork, &pokePosSub , &snapLen );
  if( work->snapPos != MUS_POKE_EQU_INVALID &&
    DUP_FIT_CheckIsEquipItem( work , work->snapPos ) == FALSE )
  {
    MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork , work->snapPos );
    dispPos.x = (int)F32_CONST(equipData->pos.x+equipData->ofs.x)+128;
    dispPos.y = (int)F32_CONST(equipData->pos.y+equipData->ofs.y)+ 96;
    rotZ = equipData->itemRot;
    MUS_ITEM_DRAW_SetUseOffset( work->itemDrawSys , itemDrawWork , TRUE );
    if( MUS_ITEM_DRAW_IsBackItem( itemDrawWork ) == TRUE )
    {
      pos.z = EQUIP_ITEM_DEPTH_BACK;
    }
    
    //��x����������L�����Z����������
    work->befItemType = IG_EQUIP;
    work->befItemPos.x = dispPos.x;
    work->befItemPos.y = dispPos.y;
    DUP_FIT_ITEM_SetEquipPos( work->holdItem , work->snapPos );
    
  }
  else
  {
    work->snapPos = MUS_POKE_EQU_INVALID;
    dispPos.x = work->tpx;
    dispPos.y = work->tpy;
    MUS_ITEM_DRAW_SetUseOffset( work->itemDrawSys , itemDrawWork , FALSE );
  }

  pos.x = FX32_CONST(dispPos.x);
  pos.y = FX32_CONST(dispPos.y);
  MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
  DUP_FIT_ITEM_SetPosition( work->holdItem , &dispPos );
  MUS_ITEM_DRAW_SetRotation( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) , rotZ );
  
  MUS_ITEM_DRAW_GetSize( work->itemDrawSys , itemDrawWork ,&scaleX,&scaleY );
  if( scaleX < FX16_ONE || scaleY < FX16_ONE )
  {
    scaleX += FX16_CONST( 0.1f );
    scaleY += FX16_CONST( 0.1f );
    if( scaleX > FX16_ONE )
    {
      scaleX = FX16_ONE;
    }
    if( scaleY > FX16_ONE )
    {
      scaleY = FX16_ONE;
    }
    MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork ,scaleX,scaleY );
  }
}


//--------------------------------------------------------------
//���X�g����t�B�[���h�փA�C�e����u��(���̂܂ܕێ���Ԃ�
//--------------------------------------------------------------
static void DUP_FIT_CreateItemListToField( FITTING_WORK *work )
{
  FIT_ITEM_WORK* item;
  VecFx32 pos = {0,0,HOLD_ITEM_DEPTH};
  ITEM_STATE *itemState = DUP_FIT_ITEM_GetItemState( work->holdItem );
  MUS_ITEM_DRAW_WORK *itemDrawWork,*holdDrawWork;
  GFL_POINT dispPos;
  GFL_POINT *befPos;
  fx16 scaleX,scaleY;

  dispPos.x = work->tpx;
  dispPos.y = work->tpy;
  pos.x = FX32_CONST(work->tpx);
  pos.y = FX32_CONST(work->tpy);

  item = DUP_FIT_ITEM_CreateItem( work->heapId , work->itemDrawSys , itemState , &pos );
  DUP_FIT_ITEMGROUP_AddItem( work->itemGroupField,item );

  DUP_FIT_ITEM_SetPosition( item , &dispPos );
  //�T�C�Y�̈��p��
  holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
  MUS_ITEM_DRAW_GetSize( work->itemDrawSys , holdDrawWork ,&scaleX,&scaleY );
  MUS_ITEM_DRAW_SetSize( work->itemDrawSys , itemDrawWork ,scaleX,scaleY );

  //���W�L��
  befPos = DUP_FIT_ITEM_GetPosition( work->holdItem );
  work->befItemType = IG_LIST;
  work->befItemPos.x = befPos->x;
  work->befItemPos.y = befPos->y;

  //�ێ��A�C�e�����������������̂ɕς���
  work->holdItem = item;
  work->holdItemType = IG_FIELD;
  work->befItemType = IG_LIST;
  
  //�A�C�e�����������Ԃ�
  itemState->isOutList = TRUE;
  DUP_FIT_CheckItemListPallet( work );
}

//--------------------------------------------------------------
//�A�C�e���𗣂� �t�B�[���h��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToField( FITTING_WORK *work )
{
  fx32 depth = FIELD_ITEM_DEPTH;
  FIT_ITEM_WORK *item;
  MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  //��x�A�C�e�������X�g������A���ɂȂ�����
  DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
  DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupField , work->holdItem );
  
  MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );
  //�[�x�̍Đݒ�
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
  while( item != NULL )
  {
    VecFx32 pos;
    MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
    pos.z = depth;
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
    
    depth -= FX32_CONST(0.1f);
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
  
#if DEB_ARI
  DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
}
//--------------------------------------------------------------
//�A�C�e���𗣂� ���X�g��
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToList( FITTING_WORK *work , const BOOL isMove )
{
  VecFx32 pos;
  MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  ITEM_STATE *itemState = DUP_FIT_ITEM_GetItemState( work->holdItem );

  //���X�g��t���ւ��č��W���Đݒ�
  DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
  DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupAnime , work->holdItem );
  
  MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
  pos.z = RETURN_LIST_ITEM_DEPTH;
  MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );

  MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );
  
  if( isMove == FALSE )
  {
    //�����Ȃ��悤�ɑO����W�����̂��̂�
    GFL_POINT *befPos = DUP_FIT_ITEM_GetPosition( work->holdItem );
    DUP_FIT_ITEM_SetBefPosition( work->holdItem , befPos );
  }

  DUP_FIT_ITEM_SetCount( work->holdItem , ITEM_RETURN_ANIME_CNT );

  //�A�C�e����߂�����Ԃ�
  itemState->isOutList = FALSE;
  DUP_FIT_CheckItemListPallet( work );
#if DEB_ARI
  DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
}

//--------------------------------------------------------------
//�A�C�e���𗣂� ������
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpDropItemToEquip(  FITTING_WORK *work )
{
  VecFx32 pos;
  fx32 depth = EQUIP_ITEM_DEPTH;
  FIT_ITEM_WORK *item;
  MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  MUS_ITEM_DRAW_WORK *holdDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
  //���X�g��t���ւ��č��W���Đݒ�
  DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupField , work->holdItem );
  DUP_FIT_ITEMGROUP_AddItemTop( work->itemGroupEquip , work->holdItem );

  MUS_ITEM_DRAW_SetSize( work->itemDrawSys , holdDrawWork ,FX16_ONE,FX16_ONE );

  DUP_FIT_ITEM_SetEquipPos( work->holdItem , work->snapPos );
  
  //�[�x�̍Đݒ�
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  while( item != NULL )
  {
    VecFx32 pos;
    MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
    if( MUS_ITEM_DRAW_IsBackItem( drawWork ) == TRUE )
    {
      pos.z = depth-EQUIP_ITEM_DEPTH+EQUIP_ITEM_DEPTH_BACK;
    }
    else
    {
      pos.z = depth;
    }
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
    
    depth -= FX32_CONST(0.1f);
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
  
#if DEB_ARI
  DUP_FIT_ITEM_DumpList( work->itemGroupField ,2 );
#endif
  
}

//--------------------------------------------------------------
//�A�C�e����������ʒu�ɖ߂�
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpCancelDropItem( FITTING_WORK *work )
{
  if( work->befItemType == IG_FIELD )
  {
      //���t�B�[���h�A�C�e��
    VecFx32 pos = {0,0,HOLD_ITEM_DEPTH};
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
    GFL_POINT dispPos;
    //����J�n�n�_�ݒ�
    dispPos.x = work->tpx;
    dispPos.y = work->tpy;
    pos.x = FX32_CONST(work->befItemPos.x);
    pos.y = FX32_CONST(work->befItemPos.y);
    DUP_FIT_ITEM_SetBefPosition( work->holdItem , &dispPos );
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
    //����I���n�_�ݒ�
    DUP_FIT_ITEM_SetPosition( work->holdItem , &work->befItemPos );
    DUP_FIT_ITEM_SetCount( work->holdItem , ITEM_RETURN_POS_CNT );
    
    DUP_FIT_UpdateTpDropItemToField( work );
  }
  else
  if( work->befItemType == IG_EQUIP )
  {
    //�������A�C�e��
    VecFx32 pos;
    GFL_POINT dispPos;
    GFL_POINT befPos;
    u16 rotZ;
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );
    const MUS_POKE_EQUIP_POS ePos = DUP_FIT_ITEM_GetEquipPos( work->holdItem);
    MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork , ePos );
    
    //����I���n�_�ݒ�
    befPos.x = (int)F32_CONST(equipData->pos.x+equipData->ofs.x)+128;
    befPos.y = (int)F32_CONST(equipData->pos.y+equipData->ofs.y)+96;
    
    pos.x = FX32_CONST(befPos.x);
    pos.y = FX32_CONST(befPos.y);
    if( MUS_ITEM_DRAW_IsBackItem( itemDrawWork ) == TRUE )
    {
      pos.z = EQUIP_ITEM_DEPTH_BACK;
    }
    else
    {
      pos.z = EQUIP_ITEM_DEPTH;
    }
    rotZ = equipData->itemRot;
    
    MUS_ITEM_DRAW_SetUseOffset( work->itemDrawSys , itemDrawWork , TRUE );
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );
    MUS_ITEM_DRAW_SetRotation( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) , rotZ );
    DUP_FIT_ITEM_SetPosition( work->holdItem , &befPos );

    //����J�n�n�_�ݒ�
    dispPos.x = work->tpx;
    dispPos.y = work->tpy;
    DUP_FIT_ITEM_SetBefPosition( work->holdItem , &dispPos );
    DUP_FIT_ITEM_SetCount( work->holdItem , ITEM_RETURN_POS_CNT );

    //�z���n�_���_�~�[�ݒ�B�ꉞ�I�������߂�
    work->snapPos = ePos;
    DUP_FIT_UpdateTpDropItemToEquip( work );
    work->snapPos = MUS_POKE_EQU_INVALID;
  }
  else
  if( work->befItemType == IG_LIST )
  {
    //�����X�g�A�C�e��
    GFL_POINT *befPos = DUP_FIT_ITEM_GetBefPosition( work->holdItem );
    GFL_POINT dispPos;
    VecFx32 pos = {0,0,ITEM_RETURN_DEPTH};
    MUS_ITEM_DRAW_WORK *itemDrawWork = DUP_FIT_ITEM_GetItemDrawWork( work->holdItem );

   //����I���n�_�ݒ�
    DUP_FIT_ITEM_SetPosition( work->holdItem , &work->befItemPos );
    DUP_FIT_ITEM_SetCount( work->holdItem , ITEM_RETURN_POS_CNT );
    
    //����J�n�n�_�ݒ�
    dispPos.x = work->tpx;
    dispPos.y = work->tpy;
    pos.x = FX32_CONST(work->befItemPos.x);
    pos.y = FX32_CONST(work->befItemPos.y);
    DUP_FIT_ITEM_SetBefPosition( work->holdItem , &dispPos );
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , itemDrawWork , &pos );

    DUP_FIT_UpdateTpDropItemToList( work , TRUE);
  }
  else
  {
    GF_ASSERT_MSG(0,"Invalid befItemType!![%d]\n",work->befItemType);
    DUP_FIT_UpdateTpDropItemToList( work , FALSE);
  }
}

//--------------------------------------------------------------
//�|�P�����̃^�b�`�`�F�b�N�i�\�[�g�p)
//--------------------------------------------------------------
static void DUP_FIT_UpdateTpPokeCheck( FITTING_WORK *work )
{
  u16 snapLen = 16;
  GFL_POINT pokePosSub;
  MUS_POKE_EQUIP_POS touchPos;
  u16 startIdx;
  //1�T����1��̂Ƃ����擪�ɂ��ă\�[�g(�X�^�[�g�͉�
//  const u32 startAngle = work->listTotalMove + 0x18000+LIST_ONE_ANGLE;
  //1��1/4�T��1��̂Ƃ����擪�ɂ��ă\�[�g(�X�^�[�g�͍�
  const u32 startAngle = work->listTotalMove + 0x14000+LIST_ONE_ANGLE;
  
  if( startAngle >= work->totalItemNum * LIST_ONE_ANGLE )
  {
    startIdx = (startAngle-(work->totalItemNum * LIST_ONE_ANGLE)) / LIST_ONE_ANGLE;
  }
  else
  {
    startIdx = startAngle / LIST_ONE_ANGLE;
  }
  
  pokePosSub.x = work->tpx - FIT_POKE_POS_X;
  pokePosSub.y = work->tpy - FIT_POKE_POS_Y;
  touchPos = DUP_FIT_SearchEquipPosition( work ,NULL, &pokePosSub , &snapLen );

  if( touchPos != MUS_POKE_EQU_INVALID )
  {
    MUS_POKE_EQUIP_USER userType = MUS_ITEM_DATA_EquipPosToUserType( touchPos );
    //��͏o�Ă�����͓̂��������ǁA���E�o�������Ȃ̂�
    if( userType == MUS_POKE_EQU_USER_HAND_L )
    {
      userType = MUS_POKE_EQU_USER_HAND_R;
    }
    if( userType == work->sortType )
    {
      //�f�t�H���g�Ń\�[�g
      DUP_FIT_SortItemIdx( work , MUS_POKE_EQUIP_USER_MAX , startIdx );
      work->sortType = MUS_POKE_EQUIP_USER_MAX;
    }
    else
    {
      DUP_FIT_SortItemIdx( work , userType , startIdx );
      work->sortType = userType;
    }
  }
  else
  {
    //�f�t�H���g�Ń\�[�g
    DUP_FIT_SortItemIdx( work , MUS_POKE_EQUIP_USER_MAX , startIdx );
    work->sortType = MUS_POKE_EQUIP_USER_MAX;
  }
  work->isSortAnime = TRUE;
  work->sortAnimeMoveAngle = 0;
  work->sortAnimeEndAngle = 0x10000 + work->listAngle%LIST_ONE_ANGLE;
  //listAngle�̂��܂肪�O��������
  /*  //1�T���̂Ƃ������L���ɂ���
  if( work->sortAnimeEndAngle == 0x10000 )
  {
    work->sortAnimeEndAngle += LIST_ONE_ANGLE;
  }
  */
  
  DUP_FIT_CalcItemListAngle( work , work->listAngle , 0 , LIST_SIZE_X , LIST_SIZE_Y );
  
}

//--------------------------------------------------------------
//�\�[�g���A�j��
//--------------------------------------------------------------
static void DUP_FIT_UpdateSortAnime( FITTING_WORK *work )
{
  static const u16 SORT_ANIME_MIN_SPD = 256;
  u32 leastAngle;
  u32 moveAngle;

  leastAngle = work->sortAnimeEndAngle - work->sortAnimeMoveAngle;
  
  moveAngle = leastAngle/10;
  if( moveAngle < SORT_ANIME_MIN_SPD )
  {
    if( leastAngle < SORT_ANIME_MIN_SPD )
    {
      moveAngle = leastAngle;
      work->isSortAnime = FALSE;
    }
    else
    {
      moveAngle = SORT_ANIME_MIN_SPD;
    }
  }
  
  work->sortAnimeMoveAngle += moveAngle;
  work->listAngle -= moveAngle;
  DUP_FIT_CalcItemListAngle( work , work->listAngle , moveAngle , LIST_SIZE_X , LIST_SIZE_Y );
}

//--------------------------------------------------------------
//�w��ӏ��ɑ��������邩�H
//--------------------------------------------------------------
static const BOOL DUP_FIT_CheckIsEquipItem( FITTING_WORK *work , const MUS_POKE_EQUIP_POS pos)
{
  const MUS_POKE_EQUIP_USER uPos = MUS_ITEM_DATA_EquipPosToUserType( pos );
  FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  while( item != NULL )
  {
    const MUS_POKE_EQUIP_USER checkUPos = MUS_ITEM_DATA_EquipPosToUserType( DUP_FIT_ITEM_GetEquipPos( item ) );
    if( uPos == checkUPos )
    {
      return TRUE;
    }
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
  return FALSE;
}
//--------------------------------------------------------------
//�w��_����߂��ʒu��T��(���΍��W�ɂ��ēn���Ă�������
//--------------------------------------------------------------
static MUS_POKE_EQUIP_POS DUP_FIT_SearchEquipPosition(  FITTING_WORK *work , MUS_ITEM_DRAW_WORK *itemDrawWork , GFL_POINT *pos , u16 *len )
{
  MUS_POKE_EQUIP_POS i;
  u16 minLen = (*len)*(*len); //���炩���ߍŏ������Ɏw�苗�������Ă���
  MUS_POKE_EQUIP_POS minPos = MUS_POKE_EQU_INVALID;
  for( i=0; i<MUS_POKE_EQUIP_MAX ;i++ )
  {
    const BOOL canEquipPos = ( itemDrawWork == NULL ? TRUE : MUS_ITEM_DRAW_CanEquipPos( itemDrawWork , i ) );
    MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork , i );
    if( equipData->isEnable == TRUE && 
       canEquipPos == TRUE )
//    if( pokeData->isEquip[i] == TRUE )
    {
      const int equipPosX = (int)F32_CONST(equipData->pos.x+equipData->ofs.x)+128-FIT_POKE_POS_X;
      const int equipPosY = (int)F32_CONST(equipData->pos.y+equipData->ofs.y)+96 -FIT_POKE_POS_Y;
      const int subX = pos->x - equipPosX;
      const int subY = pos->y - equipPosY;
      const int subLen = (subX*subX)+(subY*subY);
      if( subLen < minLen )
      {
        minLen = subLen;
        minPos = i;
      }
    }
  }
  *len = minLen;
  return minPos;
}

//--------------------------------------------------------------
//�A�C�e���̃A�j���[�V����
//--------------------------------------------------------------
static void DUP_FIT_UpdateItemAnime( FITTING_WORK *work )
{
  //itemGroupAnime�ɓo�^����Ă��镨�̏���
  {
    FIT_ITEM_WORK *nextItem = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupAnime );
    while( nextItem != NULL )
    {
      FIT_ITEM_WORK *item = nextItem;
      MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
      u16 cnt = DUP_FIT_ITEM_GetCount( item );
      GFL_POINT *befPos = DUP_FIT_ITEM_GetBefPosition( item );
      GFL_POINT *endPos = DUP_FIT_ITEM_GetPosition( item );

      //remove����\��������̂ŁA��Ɏ����Ƃ��Ă���
      nextItem = DUP_FIT_ITEM_GetNextItem(item);

      cnt--;
      if( cnt > 0 )
      {
        VecFx32 pos;
        fx16 size = FX16_ONE * (cnt) / ITEM_RETURN_ANIME_CNT;
        MUS_ITEM_DRAW_SetSize( work->itemDrawSys , drawWork , size , size );
        MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
        
        if( cnt >= ITEM_RETURN_ANIME_CNT-ITEM_RETURN_POS_CNT )
        {
          //�k����30���[��
          //�߂�̏����͑��Ƃ��킹��5�t���[��
          const u16 tempCnt = cnt - (ITEM_RETURN_ANIME_CNT-ITEM_RETURN_POS_CNT);
          pos.x = FX32_CONST((endPos->x-befPos->x)*(ITEM_RETURN_POS_CNT-tempCnt)/ITEM_RETURN_POS_CNT+befPos->x);
          pos.y = FX32_CONST((endPos->y-befPos->y)*(ITEM_RETURN_POS_CNT-tempCnt)/ITEM_RETURN_POS_CNT+befPos->y);
        }
        else
        {
          pos.x = FX32_CONST(endPos->x);
          pos.y = FX32_CONST(endPos->y);
        }
        pos.z = ITEM_RETURN_DEPTH;
        MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
        DUP_FIT_ITEM_SetCount( item , cnt );
      }
      else
      {
        DUP_FIT_ITEMGROUP_RemoveItem( work->itemGroupAnime , item );
        DUP_FIT_ITEM_DeleteItem( item , work->itemDrawSys );
      }
    }
  }
  
  //�t�B�[���h�E�����̃A�C�e���L�����Z���߂菈��
  {
    BOOL isEquipList = FALSE;
    FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
    if( item == NULL )
    {
      //�t�B�[���h�����������瑕����
      isEquipList = TRUE;
      item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
    }
    while( item != NULL )
    {
      u16 cnt = DUP_FIT_ITEM_GetCount( item );
      if( cnt > 0 )
      {
        MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
        GFL_POINT *befPos = DUP_FIT_ITEM_GetBefPosition( item );
        GFL_POINT *endPos = DUP_FIT_ITEM_GetPosition( item );
        cnt--;
        if( cnt > 0 )
        {
          VecFx32 pos;
          MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
          pos.x = FX32_CONST((endPos->x-befPos->x)*(ITEM_RETURN_POS_CNT-cnt)/ITEM_RETURN_POS_CNT+befPos->x);
          pos.y = FX32_CONST((endPos->y-befPos->y)*(ITEM_RETURN_POS_CNT-cnt)/ITEM_RETURN_POS_CNT+befPos->y);
          MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
        }
        else
        {
          VecFx32 pos;
          MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );
          pos.x = FX32_CONST(endPos->x);
          pos.y = FX32_CONST(endPos->y);
          MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
          DUP_FIT_ITEM_SetBefPosition( item , endPos );
        }
        
        DUP_FIT_ITEM_SetCount( item , cnt );
      }
      
      item = DUP_FIT_ITEM_GetNextItem(item);
      
      if( item == NULL && isEquipList == FALSE )
      {
          //�t�B�[���h�����I������瑕����
        isEquipList = TRUE;
        item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
      }
    }
  }
}

//--------------------------------------------------------------
//�e�̍X�V
//--------------------------------------------------------------
static void DUP_FIT_UpdateShadow( FITTING_WORK *work )
{
  if( work->holdItem == NULL )
  {
    MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , work->shadowItem , FALSE );
  }
  else  //VBlank�ŊG�̓]�����ł��Ȃ��ƕ\�����Ȃ�
  {
    const ITEM_STATE *itemState = DUP_FIT_ITEM_GetItemState(work->holdItem);
    if( work->shadowItemId == itemState->itemId )
    {
      VecFx32 pos;
      MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , work->shadowItem , TRUE );
      /*
      MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) , &pos );
  //    OS_Printf("[%.2f][%.2f]\n",F32_CONST(pos.x),F32_CONST(pos.y));
      pos.x += FX32_CONST(32.0f );
      pos.y += FX32_CONST(32.0f );
      pos.z -= FX32_CONST( 0.5f );
      MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , work->shadowItem , &pos );
      MUS_ITEM_DRAW_SetUseOffset( work->itemDrawSys , work->shadowItem
              , MUS_ITEM_DRAW_GetUseOffset( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) ) );
      */
      MUS_ITEM_DRAW_CopyItemDataToShadow( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) , work->shadowItem );
    }
  }
}


//--------------------------------------------------------------
//�ȉ~�̓����蔻��
//--------------------------------------------------------------
static BOOL DUP_FIT_CheckPointInOval( s16 subX , s16 subY , s16 size , float ratioYX )
{
  const s16 newSubX = (s16)(subX*ratioYX);
  if( newSubX * newSubX + subY * subY < size*size )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------
//��̕��בւ�
//--------------------------------------------------------------
static void DUP_FIT_SortItemIdx( FITTING_WORK *work , const MUS_POKE_EQUIP_USER ePos , const u16 startIdx)
{
  u16 i,j;
  for( i=0 ; i<work->totalItemNum ; i++ )
  {
    const u16 idx = (startIdx+i)%work->totalItemNum;
    work->itemState[idx] = &work->itemStateBase[i];
  }
  for( i=0 ; i<work->totalItemNum ; i++ )
  {
    const u16 idx_i = (startIdx+i)%work->totalItemNum;

    u16 biggerIdx = idx_i;
    u16 biggerPri;
    biggerPri = DUP_FIT_CalcSortPriority( work , work->itemState[idx_i] , ePos );
    for( j=i+1 ; j<work->totalItemNum ; j++ )
    {
      const u16 idx_j = (startIdx+j)%work->totalItemNum;
      const u16 jPir = DUP_FIT_CalcSortPriority( work , work->itemState[idx_j] , ePos );
      if( biggerPri < jPir )
      {
        biggerPri = jPir;
        biggerIdx = idx_j;
      }
    }
    if( biggerIdx != idx_i )
    {
      ITEM_STATE *temp;
      temp = work->itemState[idx_i];
      work->itemState[idx_i] = work->itemState[biggerIdx];
      work->itemState[biggerIdx] = temp;
    }
  }
  
  //dump
#if 0
  for( i=0 ; i<work->totalItemNum ; i++ )
  {
    u16 biggerPri;
    biggerPri = DUP_FIT_CalcSortPriority( work , work->itemState[i] , ePos );
    OS_TPrintf("[%2d:%4d]\n",work->itemState[i]->itemId,biggerPri);
  }
#endif
}

static const u16 DUP_FIT_CalcSortPriority( FITTING_WORK *work , ITEM_STATE *itemState , const MUS_POKE_EQUIP_USER uPos )
{
  const BOOL isMainPos = MUS_ITEM_DRAW_CheckMainPosUserDataItemNo(work->itemDrawSys , itemState->itemId , uPos );
  const BOOL isTrgPos = MUS_ITEM_DRAW_CanEquipPosUserDataItemNo(work->itemDrawSys , itemState->itemId , uPos );
  return ( isTrgPos == TRUE  ? 3000 : 0 ) +
         ( itemState->isNew  ? 2000 : 0 ) +
         ( (isMainPos==TRUE && itemState->isNew==FALSE) ? 1000 : 0 ) +
         ( 999 - itemState->itemId );

  //�\����
  // New + Trg(+Main)
  // Trg + Main
  // Trg
  // New
  // Normal
}

#pragma mark [> StateChangeAnime
//--------------------------------------------------------------
//�X�e�[�g�ύX�̃A�j��
//--------------------------------------------------------------
static void DUP_FIT_ChangeStateAnime( FITTING_WORK *work )
{
  FIT_ITEM_WORK *item;
  //���X�g�̃A�C�e��
  DUP_FIT_CalcItemListAngle( work , work->listAngle , 0 , LIST_SIZE_X+work->animeCnt , LIST_SIZE_Y+work->animeCnt*LIST_SIZE_RATIO );
  
  //�t�B�[���h�̃A�C�e��
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupField );
  while( item != NULL )
  {
    VecFx32 pos;
    GFL_POINT* dispPos = DUP_FIT_ITEM_GetPosition( item );
    MUS_ITEM_DRAW_WORK *drawWork = DUP_FIT_ITEM_GetItemDrawWork( item );
    
    //���S����̊p�x�����߂�
    const s16 subX = dispPos->x - LIST_CENTER_X;
    const s16 subY = dispPos->y - LIST_CENTER_Y;
    const fx32 len = FX_Sqrt( FX32_CONST( subX*subX+subY*subY ));
    const u16 angle = FX_Atan2Idx(FX32_CONST(subY),FX32_CONST(subX));
    
    MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , drawWork , &pos );

    pos.x = (FX_Mul(FX_CosIdx( angle ),( len + FX32_CONST(work->animeCnt) ))          + LIST_CENTER_X_FX);
    pos.y = (FX_Mul(FX_SinIdx( angle ),( len + FX32_CONST(work->animeCnt*LIST_SIZE_RATIO ) )) + LIST_CENTER_Y_FX);
    //Z�͂��̂܂�
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , drawWork , &pos );
    
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
  
  //BG
  GFL_BG_SetScroll( FIT_FRAME_MAIN_MIRROR , GFL_BG_SCROLL_Y_SET , -work->animeCnt );

  //OBJ(�{�^��)
  {
    GFL_CLACTPOS cellPos;
    cellPos.x = BUTTON_ASSEPT_POS_X;
    cellPos.y = BUTTON_ASSEPT_POS_Y+( FIT_ANIME_MAX-work->animeCnt < BUTTON_SIZE_Y ?
              FIT_ANIME_MAX-work->animeCnt : BUTTON_SIZE_Y );
    GFL_CLACT_WK_SetPos( work->buttonCell[0] , &cellPos , CLSYS_DEFREND_MAIN );

    cellPos.x = BUTTON_RETURN_POS_X;
    cellPos.y = BUTTON_RETURN_POS_Y+( FIT_ANIME_MAX-work->animeCnt < BUTTON_SIZE_Y ?
              FIT_ANIME_MAX-work->animeCnt : BUTTON_SIZE_Y );
    GFL_CLACT_WK_SetPos( work->buttonCell[1] , &cellPos , CLSYS_DEFREND_MAIN );
  }

}

#pragma mark [> CheckFunc
//--------------------------------------------------------------
//�`�F�b�N���
//--------------------------------------------------------------

static FITTING_RETURN DUP_CHECK_CheckMain(  FITTING_WORK *work )
{
  GFL_UI_TP_HITTBL buttonHitTbl[3] =
  {
    {
      BUTTON_ASSEPT_POS_Y - BUTTON_SIZE_Y/2 ,
      BUTTON_ASSEPT_POS_Y + BUTTON_SIZE_Y/2 ,
      BUTTON_ASSEPT_POS_X - BUTTON_SIZE_X/2 ,
      BUTTON_ASSEPT_POS_X + BUTTON_SIZE_X/2 ,
    },{
      BUTTON_RETURN_POS_Y - BUTTON_SIZE_Y/2 ,
      BUTTON_RETURN_POS_Y + BUTTON_SIZE_Y/2 ,
      BUTTON_RETURN_POS_X - BUTTON_SIZE_X/2 ,
      BUTTON_RETURN_POS_X + BUTTON_SIZE_X/2 ,
    },{
      GFL_UI_TP_HIT_END,0,0,0
    }
  };
  int hitRet;
  
  hitRet = GFL_UI_TP_HitTrg( buttonHitTbl );
  switch( hitRet )
  {
  case 0: //����{�^��
    DUP_CHECK_SaveNowEquip( work );
    PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    work->isOpenCurtain = TRUE;
    return FIT_RET_GO_END;
    break;
  case 1: //�߂�{�^��
    work->state = DUS_RETURN_FITTING;
    work->animeCnt = FIT_ANIME_MAX;
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
    {
      DUP_CHECK_ResetItemAngle( work );
    }
    break;
  case GFL_UI_TP_HIT_NONE:
    DUP_CHECK_UpdateTpMain( work );
    break;
  }
  
  DUP_FIT_UpdateShadow( work );
  return FIT_RET_CONTINUE;
}

static void DUP_CHECK_ResetItemAngle( FITTING_WORK *work )
{
  //�p�x��߂�
  u8 i;
  FIT_ITEM_WORK *item;
  for( i=0;i<MUS_POKE_EQUIP_MAX;i++ )
  {
    work->initWork->musPoke->equip[i].angle = 0;
  }
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  while( item != NULL )
  {
    u16 equipPos = DUP_FIT_ITEM_GetEquipPos( item );
    MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork , equipPos );
    MUS_ITEM_DRAW_SetRotation( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(item) , equipData->itemRot );
    item = DUP_FIT_ITEM_GetNextItem(item);
  }
}

static void DUP_CHECK_UpdateTpMain( FITTING_WORK *work )
{
  work->befTpx = work->tpx;
  work->befTpy = work->tpy;
  work->tpIsTrg = GFL_UI_TP_GetTrg();
  work->tpIsTouch = GFL_UI_TP_GetPointCont( &work->tpx,&work->tpy );
  
  if( work->tpIsTrg == TRUE )
  {
    //�����i�Ƃ̓����蔻��
    FIT_ITEM_WORK *item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
    while( item != NULL )
    {
      if( DUP_FIT_ITEM_CheckHit( item , work->tpx,work->tpy ) == TRUE )
      {
        work->holdItem = item;
        ARI_TPrintf("Equip Item Hold[%d]\n",DUP_FIT_ITEM_GetItemState(item)->itemId);
      }
      item = DUP_FIT_ITEM_GetNextItem(item);
    }
  }
  
  if( work->tpIsTouch == TRUE && work->holdItem != NULL )
  {
    DUP_CHECK_UpdateTpHoldingItem( work );
    
  }
  if( work->tpIsTouch == FALSE )
  {
    work->befAngleEnable = FALSE;
    work->holdItem = NULL;
  }
}

static void DUP_CHECK_UpdateTpHoldingItem( FITTING_WORK *work )
{
  fx32 subX;
  fx32 subY;
  u16 angle;

  GFL_POINT *itemPos = DUP_FIT_ITEM_GetPosition( work->holdItem );
  subX = FX32_CONST((s32)work->tpx - itemPos->x);
  subY = FX32_CONST((s32)work->tpy - itemPos->y);
  angle = FX_Atan2Idx(subX,subY);
  
  if( work->befAngleEnable == TRUE )
  {
    u16 rot;
    s32 subAngle = work->befAngle - angle;
    u16 equipPos = DUP_FIT_ITEM_GetEquipPos( work->holdItem );
    s16 angle = work->initWork->musPoke->equip[equipPos].angle;
    MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( work->drawWork , equipPos );
    
    angle += subAngle;
    
    if( angle < -MUS_POKE_EQUIP_ANGLE_MAX )
    {
      angle = -MUS_POKE_EQUIP_ANGLE_MAX;
    }
    if( angle >  MUS_POKE_EQUIP_ANGLE_MAX )
    {
      angle =  MUS_POKE_EQUIP_ANGLE_MAX;
    }
      
    rot = equipData->itemRot+angle;
    MUS_ITEM_DRAW_SetRotation( work->itemDrawSys , DUP_FIT_ITEM_GetItemDrawWork(work->holdItem) , rot );
    
    work->initWork->musPoke->equip[equipPos].angle = angle;
  }
  work->befAngle = angle;
  work->befAngleEnable = TRUE;
  
}

static void DUP_CHECK_SaveNowEquip( FITTING_WORK *work )
{
  
  MUSICAL_EQUIP_SAVE *mus_bef_save = MUSICAL_SAVE_GetBefEquipData( work->initWork->mus_save );
  FIT_ITEM_WORK *item;
  u8 i;
  u8 save_pos = 0;
  
  MUSICAL_SAVE_ResetBefEquip(work->initWork->mus_save);
  
  item = DUP_FIT_ITEMGROUP_GetStartItem( work->itemGroupEquip );
  while( item != NULL && save_pos < MUSICAL_ITEM_EQUIP_MAX )
  {
    u16 equip_pos = DUP_FIT_ITEM_GetEquipPos( item );
    MUS_POKE_EQUIP_DATA *equip_data = MUS_POKE_DRAW_GetEquipData( work->drawWork , equip_pos );
    
    GF_ASSERT_MSG( save_pos < MUSICAL_ITEM_EQUIP_MAX , "�����i���������I\n" );

    mus_bef_save->equipData[save_pos].pos = equip_pos;
    mus_bef_save->equipData[save_pos].data.itemNo = DUP_FIT_ITEM_GetItemState( item )->itemId;
    mus_bef_save->equipData[save_pos].data.angle = work->initWork->musPoke->equip[equip_pos].angle;
    work->initWork->musPoke->equip[equip_pos].itemNo = DUP_FIT_ITEM_GetItemState( item )->itemId;
    
    item = DUP_FIT_ITEM_GetNextItem(item);

    save_pos++;
  }
  
}

#pragma mark [> DemoFunc
//--------------------------------------------------------------
//�f�����
//--------------------------------------------------------------
static void DUP_DEMO_DemoStart( FITTING_WORK *work )
{
  u16 idx;
  work->isDemo = TRUE;
  work->demoCnt = 0;
  work->demoPhase = 1;
  
  //�ړ��ʂ̐ݒ�
  //�����l�̈ʒu�������Ă��Ȃ��A�C�e����T��
  idx = DEMO_ROT_ITEM_NUM;
  while( TRUE )
  {
    if( work->itemState[idx]->isOutList == TRUE )
    {
      idx++;
      if( idx == work->totalItemNum )
      {
        idx = 0;
      }
      if( idx == DEMO_ROT_ITEM_NUM )
      {
        //1�T�����E�E�E�ꉞ�����悤��
        GF_ASSERT_MSG(0,"Demo:Item is out of list all !!\n");
        break;
      }
    }
    else
    {
      break;
    }
  }

  if( idx == 0 )
  {
    work->demoMoveValue = work->totalItemNum-1;
  }
  else
  {
    work->demoMoveValue = idx-1;
  }
  work->state = DUS_FITTING_DEMO;
}

static void DUP_DEMO_DemoEnd( FITTING_WORK *work )
{
  work->isDemo = FALSE;
  work->state = DUS_FITTING_MAIN;
  GFL_CLACT_WK_SetDrawEnable( work->demoPen, FALSE );
}

static void DUP_DEMO_DemoMain( FITTING_WORK *work )
{
  switch( work->demoPhase )
  {
  case 1:
    //�^�b�`�y�����E���牺�փ��X�g����
    {
      const s32 angle = work->demoMoveValue*LIST_ONE_ANGLE;
      DUP_DEMO_DemoPhaseListRot( work , angle , angle , work->demoMoveValue*15 );
    }
    break;
  case 2:
    DUP_DEMO_DemoPhaseWait( work , 30 );
    break;
  case 3:
    //�^�b�`�y���������牺�փ��X�g����
    {
      const s32 angle = work->demoMoveValue*LIST_ONE_ANGLE;
      DUP_DEMO_DemoPhaseListRot( work , -(u16)angle , -angle , work->demoMoveValue*15 );
    }
    break;
  case 4:
    DUP_DEMO_DemoPhaseWait( work , 30 );
    break;
  case 5:
    {
      const GFL_POINT start ={ 162,120 };
      const GFL_POINT end   ={ 208,160 };
      DUP_DEMO_DemoPhaseDragPen( work , &start , &end , 60 );
    }
    break;
  case 6:
    DUP_DEMO_DemoPhaseWait( work , 30 );
    break;
  case 7:
    {
      const GFL_POINT start ={ 208,160 };
      const GFL_POINT end   ={ 162,120 };
      DUP_DEMO_DemoPhaseDragPen( work , &start , &end , 60 );
    }
    break;
  case 8:
    DUP_DEMO_DemoPhaseWait( work , 30 );
    break;
  case 9:
    DUP_DEMO_DemoEnd( work );
    return;
    break;
  }
  
//  if( work->tpIsTouch == TRUE )
  {
    GFL_CLACTPOS cellPos;
    cellPos.x = work->tpx;
    cellPos.y = work->tpy;
    GFL_CLACT_WK_SetPos( work->demoPen , &cellPos , CLSYS_DEFREND_MAIN );
    //GFL_CLACT_WK_SetDrawEnable( work->demoPen, TRUE );
  }
//  else
  {
    //GFL_CLACT_WK_SetDrawEnable( work->demoPen, FALSE );
  }
  
  DUP_FIT_FittingMain( work );
  
#if DEB_ARI
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    DUP_DEMO_DemoEnd( work );
  }
#endif //DEB_ARI
}

//�^�b�`�y�������X�g����
static void DUP_DEMO_DemoPhaseListRot( FITTING_WORK *work , const u16 start , const s32 value , const u16 cnt)
{
  work->tpIsTouch = FALSE;
  work->tpIsTrg = FALSE;
  GFL_CLACT_WK_SetDrawEnable( work->demoPen, TRUE );
  //���W
  {
    const s32 oneAngle = value/cnt;
    const u16 Angle = (start-oneAngle*work->demoCnt + 0x10000)%0x10000;
    const fx32 sin = (fx32)FX_SinIdx( Angle );
    const fx32 cos = (fx32)FX_CosIdx( Angle );
    const fx32 posX = LIST_CENTER_X_FX+sin*LIST_SIZE_X;
    const fx32 posY = LIST_CENTER_Y_FX+cos*LIST_SIZE_Y;
    work->tpx = F32_CONST( posX );
    work->tpy = F32_CONST( posY );
    
    work->listAngle -= oneAngle;
    DUP_FIT_CalcItemListAngle( work , work->listAngle , oneAngle , LIST_SIZE_X , LIST_SIZE_Y );
  }

  work->demoCnt++;
  if( work->demoCnt > cnt )
  {
    work->demoCnt = 0;
    work->demoPhase++;
  }

}

//�E�F�C�g
static void DUP_DEMO_DemoPhaseWait( FITTING_WORK *work , const u16 cnt )
{
  work->tpIsTouch = FALSE;
  work->tpIsTrg = FALSE;
  work->demoCnt++;
  GFL_CLACT_WK_SetDrawEnable( work->demoPen, FALSE );
  if( work->demoCnt > cnt )
  {
    work->demoCnt = 0;
    work->demoPhase++;
  }
}

//�^�b�`�y���̈ړ�
static void DUP_DEMO_DemoPhaseDragPen( FITTING_WORK *work , const GFL_POINT *start , const GFL_POINT *end , const u16 cnt )
{
  GFL_POINT pos;
  
  work->tpIsTouch = TRUE;
  if( work->demoCnt == 0 )
  {
    work->tpIsTrg = TRUE;
  }
  else
  {
    work->tpIsTrg = FALSE;
  }
  pos.x = start->x+(end->x-start->x)*work->demoCnt/cnt;
  pos.y = start->y+(end->y-start->y)*work->demoCnt/cnt;

  work->tpx = pos.x;
  work->tpy = pos.y;

  GFL_CLACT_WK_SetDrawEnable( work->demoPen, TRUE );
  work->demoCnt++;
  if( work->demoCnt > cnt )
  {
    work->demoCnt = 0;
    work->demoPhase++;
  }
  
}

