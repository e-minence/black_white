//======================================================================
/**
 * @file	plist_sys.c
 * @brief	�|�P�������X�g ���C������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPLIST
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "pokelist_gra.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_pokelist.h"

#include "pokeicon/pokeicon.h"
#include "savedata/myitem_savedata.h"
#include "waza_tool/wazano_def.h"
#include "item/item.h"

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_message.h"
#include "plist_menu.h"
#include "plist_item.h"
#include "plist_snd_def.h"

#include "app/p_status.h" //Proc�؂�ւ��p
#include "app/app_menu_common.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PLIST_BARICON_Y (168)
#define PLIST_BARICON_EXIT_X   (200)  //������(�~�}�[�N
#define PLIST_BARICON_RETURN_X_MENU (224)  //�߂�(���
#define PLIST_BARICON_RETURN_X_BAR (224)  //�߂�(���

#define PLIST_CHANGE_ANM_COUNT (16)
#define PLIST_CHANGE_ANM_VALUE (1)    //�ړ���(�L�����P��

//�o�g�����j���[�p�{�^�����W
#define PLIST_BATTLE_BUTTON_DECIDE_X (6)
#define PLIST_BATTLE_BUTTON_DECIDE_Y (21)
#define PLIST_BATTLE_BUTTON_CANCEL_X (19)
#define PLIST_BATTLE_BUTTON_CANCEL_Y (21)

#define PLIST_PLATE_ACTIVE_ANM_CNT (12)
#define PLIST_PLATE_ACTIVE_ANM_CNT_HALF (6)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//�T�u�ƂȂ�V�[�P���X(work->subSeq
typedef enum
{
  PSSS_INIT,
  PSSS_MAIN,
  PSSS_TERM,
  
  //����ւ��A�j���p
  PSSS_MOVE_OUT,
  PSSS_MOVE_IN,

  //Proc�؂�ւ��p
  PSSS_FADEOUT,
  PSSS_FADEIN,

  PSSS_MAX,
}PLIST_SYS_SUB_SEQ;

//�I�����
typedef enum
{
  PSSEL_DECIDE, //�����������(�Q���I���Ƃ�)
  PSSEL_RETURN, //�L�����Z��(B)
  PSSEL_EXIT, //�߂�(X)

  PSSEL_SELECT, //�|�P������I��
  PSSEL_NONE, //�I��

  PSSEL_BUTTON_NUM = PSSEL_EXIT+1,  //TP����Ń^�b�`�e�[�u���Ɏg��
  PSSEL_MAX,  
}PLIST_SYS_SELECT_STATE;


//�J�[�\���̃A�j��
enum PLIST_CURCOR_ANIME
{
  PCA_NORMAL_A, //�ʏ�0�ԗp
  PCA_NORMAL_B, //�ʏ�
  PCA_CHANGE_A, //����ւ�0�ԗp
  PCA_CHANGE_B, //����ւ�
  PCA_SELECT_A, //�I��0�ԗp
  PCA_SELECT_B, //�I��
  PCA_MENU,   //���j���[�p
};

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

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
  GX_VRAM_SUB_OBJ_NONE,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
  GX_VRAM_TEX_0_D,        // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_0_F,     // �e�N�X�`���p���b�g�X���b�g
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};

FS_EXTERN_OVERLAY(poke_status);

static void PLIST_UpdatePlatePalletAnime( PLIST_WORK *work );
static void PLIST_VBlankFunc(GFL_TCB *tcb, void *wk );

//�O���t�B�b�N�n
static void PLIST_InitGraphic( PLIST_WORK *work );
static void PLIST_TermGraphic( PLIST_WORK *work );
static void PLIST_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void PLIST_InitBG0_2DMenu( PLIST_WORK *work );
static void PLIST_InitBG0_3DParticle( PLIST_WORK *work );

static void PLIST_InitCell( PLIST_WORK *work );
static void PLIST_TermCell( PLIST_WORK *work );

static void PLIST_InitMessage( PLIST_WORK *work );
static void PLIST_TermMessage( PLIST_WORK *work );

//���\�[�X�n
static void PLIST_LoadResource( PLIST_WORK *work );
static void PLIST_ReleaseResource( PLIST_WORK *work );

//���[�h�ʏ����n
static void PLIST_InitMode( PLIST_WORK *work );
static void PLIST_InitMode_Select( PLIST_WORK *work );
static void PLIST_TermMode_Select_Decide( PLIST_WORK *work );
static void PLIST_InitMode_Menu( PLIST_WORK *work );

//�|�P�����I��
static void PLIST_SelectPoke( PLIST_WORK *work );
static void PLIST_SelectPokeInit( PLIST_WORK *work );
static void PLIST_SelectPokeTerm( PLIST_WORK *work );
static void PLIST_SelectPokeTerm_Change( PLIST_WORK *work );
static void PLIST_SelectPokeTerm_Battle( PLIST_WORK *work );
static void PLIST_SelectPokeMain( PLIST_WORK *work );
static void PLIST_SelectPokeUpdateKey( PLIST_WORK *work );
static void PLIST_SelectPokeUpdateTP( PLIST_WORK *work );
static void PLIST_SelectPokeSetCursor( PLIST_WORK *work , const PL_SELECT_POS pos );
static void PLIST_SelectPokeSetCursor_Change( PLIST_WORK *work , const PL_SELECT_POS pos );
static void PLIST_PLATE_SetActivePlatePos( PLIST_WORK *work , const PL_SELECT_POS pos );
//���j���[�I��
static void PLIST_SelectMenu( PLIST_WORK *work );
static void PLIST_SelectMenuInit( PLIST_WORK *work );
static void PLIST_SelectMenuTerm( PLIST_WORK *work );
static void PLIST_SelectMenuExit( PLIST_WORK *work );

//����ւ��A�j��
static void PLIST_ChangeAnime( PLIST_WORK *work );
static void PLIST_ChangeAnimeInit( PLIST_WORK *work );
static void PLIST_ChangeAnimeTerm( PLIST_WORK *work );
static void PLIST_ChangeAnimeUpdatePlate( PLIST_WORK *work );

//���b�Z�[�W�҂�
static void PLIST_MessageWait( PLIST_WORK *work );

//�͂��E������
static void PLIST_YesNoWait( PLIST_WORK *work );
static void PLIST_YesNoWaitInit( PLIST_WORK *work , PSTATUS_CallbackFuncYesNo yesNoCallBack );

//Proc�؂�ւ�
static void PLIST_ChangeProcInit( PLIST_WORK *work , GFL_PROC_DATA *procData , FSOverlayID overlayId , void *parentWork );
static void PLIST_ChangeProcUpdate( PLIST_WORK *work );

//���b�Z�[�W�R�[���o�b�N
static void PSTATUS_MSGCB_ReturnSelectCommon( PLIST_WORK *work );
static void PSTATUS_MSGCB_ForgetSkill_ForgetCheck( PLIST_WORK *work );
static void PSTATUS_MSGCB_ForgetSkill_ForgetCheckCB( PLIST_WORK *work , const int retVal );
static void PSTATUS_MSGCB_ForgetSkill_SkillCancel( PLIST_WORK *work );
static void PSTATUS_MSGCB_ForgetSkill_SkillCancelCB( PLIST_WORK *work , const int retVal );
static void PSTATUS_MSGCB_ForgetSkill_SkillForget( PLIST_WORK *work );

static void PSTATUS_MSGCB_ItemSet_CheckChangeItem( PLIST_WORK *work );
static void PSTATUS_MSGCB_ItemSet_CheckChangeItemCB( PLIST_WORK *work , const int retVal );

//�O�����l����
static void PSTATUS_LearnSkillEmpty( PLIST_WORK *work , POKEMON_PARAM *pp );
static void PSTATUS_LearnSkillFull( PLIST_WORK *work  , POKEMON_PARAM *pp , u8 pos );
static void PSTATUS_SubItem( PLIST_WORK *work , u16 itemNo );
static void PSTATUS_AddItem( PLIST_WORK *work , u16 itemNo );

//�f�o�b�O���j���[
static void PLIST_InitDebug( PLIST_WORK *work );
static void PLIST_TermDebug( PLIST_WORK *work );
//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
const BOOL PLIST_InitPokeList( PLIST_WORK *work )
{
  u8 i;
  u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
  
  work->ktst = GFL_UI_CheckTouchOrKey();
  work->mainSeq = PSMS_FADEIN;
  work->subSeq = PSSS_INIT;
  work->selectPokePara = NULL;
  work->menuRet = PMIT_NONE;
  work->changeTarget = PL_SEL_POS_MAX;
  work->canExit = FALSE;
  work->isActiveWindowMask = FALSE;
  work->reqChangeProc = FALSE;
  work->changeProcSeq = PSCS_INIT;
  work->btlJoinNum = 0;
  work->platePalAnmCnt = 0;

  if( work->plData->mode == PL_MODE_WAZASET )
  {
    if( work->plData->waza == 0 )
    {
      work->plData->waza = ITEM_GetWazaNo(work->plData->item);
    }
  }

  
  PLIST_InitGraphic( work );
  PLIST_LoadResource( work );
  PLIST_InitMessage( work );
  

  //�v���[�g�̍쐬
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    if( i<partyMax )
    {
      POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, i);
      work->plateWork[i] = PLIST_PLATE_CreatePlate( work , i , pp );
    }
    else
    {
      work->plateWork[i] = PLIST_PLATE_CreatePlate_Blank( work , i );
    }
    
    if( PLIST_PLATE_GetBattleOrder( work->plateWork[i] ) <= PPBO_JOIN_6 )
    {
      work->btlJoinNum++;
    }
  }
  //�`�揇�̊֌W�ケ����
  PLIST_InitCell( work );
  
//  PLIST_PLATE_ChangeColor( work , work->plateWork[0] , PPC_NORMAL_SELECT );
  work->vBlankTcb = GFUser_VIntr_CreateTCB( PLIST_VBlankFunc , work , 8 );

  //���b�Z�[�W�V�X�e������
  work->msgWork = PLIST_MSG_CreateSystem( work );

  //���j���[�V�X�e������
  work->menuWork = PLIST_MENU_CreateSystem( work );

  
  //���[�h�ʏ�����
  PLIST_InitMode( work );

  //�܂Ƃ߂ēǂݍ���
  GFL_BG_LoadScreenV_Req(PLIST_BG_MAIN_BG);
  GFL_BG_LoadScreenV_Req(PLIST_BG_PLATE);
  GFL_BG_LoadScreenV_Req(PLIST_BG_SUB_BG);

#if USE_DEBUGWIN_SYSTEM
  PLIST_InitDebug( work );
#endif
  return TRUE;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
const BOOL PLIST_TermPokeList( PLIST_WORK *work )
{
  u8 i;
#if USE_DEBUGWIN_SYSTEM
  PLIST_TermDebug( work );
#endif

  GFL_TCB_DeleteTask( work->vBlankTcb );
  
  PLIST_MENU_DeleteSystem( work , work->menuWork );
  PLIST_MSG_DeleteSystem( work , work->msgWork );

  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    PLIST_PLATE_DeletePlate( work , work->plateWork[i] );
  }

  PLIST_TermMessage( work );
  PLIST_TermCell( work );
  PLIST_ReleaseResource( work );
  PLIST_TermGraphic( work );

  GFL_UI_SetTouchOrKey( work->ktst );
  
  return TRUE;
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
const BOOL PLIST_UpdatePokeList( PLIST_WORK *work )
{
  u8 i;

  switch( work->mainSeq )
  {
  case PSMS_FADEIN:
//    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_SHUTTERIN_DOWN , WIPE_TYPE_SHUTTERIN_DOWN , 
//                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = PSMS_FADEIN_WAIT;
    break;
  
  case PSMS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->mainSeq = work->nextMainSeq;
    }
    break;
    
  case PSMS_SELECT_POKE:
  case PSMS_CHANGE_POKE:  //��{�͈ꏏ�����瓯��������
    PLIST_SelectPoke( work );
    break;

  case PSMS_CHANGE_ANM:
    PLIST_ChangeAnime( work );
    break;
    
  case PSMS_MENU:
    PLIST_SelectMenu( work );
    break;
  
  case PSMS_MSG_WAIT:
    PLIST_MessageWait( work );
    break;

  case PSMS_YESNO_WAIT:
    PLIST_YesNoWait( work );
    break;

  case PSMS_FADEOUT:
//    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_SPLITOUT_VSIDE , WIPE_TYPE_SPLITOUT_VSIDE , 
//                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = PSMS_FADEOUT_WAIT;
    break;
  
  case PSMS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;

  case PSMS_CHANGEPROC:
    PLIST_ChangeProcUpdate( work );
    break;
  }
  
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    PLIST_PLATE_UpdatePlate( work , work->plateWork[i] );
  }
  PLIST_MSG_UpdateSystem( work , work->msgWork );
  
  PLIST_UpdatePlatePalletAnime( work );

  //���b�Z�[�W
  PRINTSYS_QUE_Main( work->printQue );

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();



  return FALSE;
}

//--------------------------------------------------------------
//	�I�𒆂̃p���b�g�̃t���b�V���A�j���X�V
//--------------------------------------------------------------
static void PLIST_UpdatePlatePalletAnime( PLIST_WORK *work )
{
  //�v���[�g�A�j��
  if( work->platePalAnmCnt > 0 )
  {
    u8 anmRate;
    u8 i;
    work->platePalAnmCnt--; //�ŏI�I��0�ɂ���̂Ő�Ƀf�N�������g
    if( work->platePalAnmCnt > PLIST_PLATE_ACTIVE_ANM_CNT_HALF )
    {
      anmRate = PLIST_PLATE_ACTIVE_ANM_CNT_HALF-(work->platePalAnmCnt-PLIST_PLATE_ACTIVE_ANM_CNT_HALF);
    }
    else
    {
      anmRate = work->platePalAnmCnt;
    }
    for( i=0;i<16;i++ )
    {
      u8 r = (work->platePalAnm[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      u8 g = (work->platePalAnm[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      u8 b = (work->platePalAnm[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
//      r = (r<anmRate*2?0:r-(anmRate*2));
//      g = (g<anmRate*2?0:g-(anmRate*2));
//      b = (b<anmRate*2?0:b-(anmRate*2));
      r = (r+anmRate*2 > 31?1:r+(anmRate*2));
      g = (g+anmRate*2 > 31?31:g+(anmRate*2));
      b = (b+anmRate*2 > 31?31:b+(anmRate*2));
      work->platePalTrans[i] = GX_RGB(r,g,b);
    }
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        PPC_NORMAL_SELECT * 32 ,
                                        work->platePalTrans , 2*16 );
  }
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void PLIST_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  PLIST_WORK *work = (PLIST_WORK*)wk;

  if( work->isActiveWindowMask == TRUE )
  {
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
  }
  else
  {
    GX_SetVisibleWnd( GX_WNDMASK_NONE );
  }

  //OBJ�̍X�V
  GFL_CLACT_SYS_VBlankFunc();
//  NNS_GfdDoVramTransfer();
}


#pragma mark [>graphic
//--------------------------------------------------------------
//�O���t�B�b�N�n������
//--------------------------------------------------------------
static void PLIST_InitGraphic( PLIST_WORK *work )
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

  //BG�n�̏�����
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram���蓖�Ă̐ݒ�
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    // BG1 MAIN (�p�����[�^
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (�v���[�g
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (�w�i
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x05000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG3 SUB (�w�i
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    PLIST_SetupBgFunc( &header_main3 , PLIST_BG_MAIN_BG , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_main2 , PLIST_BG_PLATE , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_main1 , PLIST_BG_PARAM , GFL_BG_MODE_TEXT );

    PLIST_SetupBgFunc( &header_sub3 , PLIST_BG_SUB_BG , GFL_BG_MODE_TEXT );
    
    //�Ƃ肠����2D�ŏ�����
    PLIST_InitBG0_2DMenu( work );
    
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                      GX_BLEND_PLANEMASK_BG3 ,
                      16 , 10 );

    //�y��ƃp�����[�^�͎�ւ��A�j���̂��߃X�N���[��������
    GFL_BG_SetScroll( PLIST_BG_PLATE , GFL_BG_SCROLL_X_SET , PLIST_BG_SCROLL_X_CHAR*8 );
    GFL_BG_SetScroll( PLIST_BG_PARAM , GFL_BG_SCROLL_X_SET , PLIST_BG_SCROLL_X_CHAR*8 );
  }
  
  //OBJ�n�̏�����
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
  //Vram�]���A�j��
  //machineuse �ŏ��������Ă�
//  NNS_GfdInitVramTransferManager( work->transTask , PLIST_VTRANS_TASK_NUM );
  NNS_GfdClearVramTransferManagerTask();
}

//--------------------------------------------------------------
//BG0�p������(BG0�ʂ�2D/3D
//--------------------------------------------------------------
static void PLIST_InitBG0_2DMenu( PLIST_WORK *work )
{
  // BG0 MAIN (���j���[
  static const GFL_BG_BGCNT_HEADER header_main0 = {
    0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x18000,0x8000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
  };
  PLIST_SetupBgFunc( &header_main0 , PLIST_BG_MENU , GFL_BG_MODE_TEXT );
}

static void PLIST_InitBG0_3DParticle( PLIST_WORK *work )
{
  GF_ASSERT_MSG( FALSE ,"�܂�����ĂȂ�!!\n");
}


//--------------------------------------------------------------
//�O���t�B�b�N�n�J��
//--------------------------------------------------------------
static void PLIST_TermGraphic( PLIST_WORK *work )
{
  NNS_GfdClearVramTransferManagerTask();
  GFL_CLACT_SYS_Delete();
  
  GFL_BG_FreeBGControl( PLIST_BG_MAIN_BG );
  GFL_BG_FreeBGControl( PLIST_BG_PLATE );
  GFL_BG_FreeBGControl( PLIST_BG_PARAM );
  GFL_BG_FreeBGControl( PLIST_BG_MENU );
  GFL_BG_FreeBGControl( PLIST_BG_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void PLIST_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  �Z���쐬
//--------------------------------------------------------------------------
static void PLIST_InitCell( PLIST_WORK *work )
{
  //TODO ���͓K��
  work->cellUnit  = GFL_CLACT_UNIT_Create( 10 , PLIST_CELLUNIT_PRI_MAIN, work->heapId );
  GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
  //�Z���̐���
  //�J�[�\��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 3;
    work->clwkCursor[0] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_CURSOR],
              work->cellRes[PCR_PLT_OBJ_COMMON],
              work->cellRes[PCR_ANM_CURSOR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    work->clwkCursor[1] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_CURSOR],
              work->cellRes[PCR_PLT_OBJ_COMMON],
              work->cellRes[PCR_ANM_CURSOR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkCursor[0] , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkCursor[1] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
  }
  //�o�[�A�C�R��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_BARICON_RETURN_X_BAR;
    cellInitData.pos_y = PLIST_BARICON_Y;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    //�������̓��j���[�̂Ƃ�����ɏo��̂�bgpri0
    work->clwkBarIcon[PBT_RETURN] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_BAR_ICON],
              work->cellRes[PCR_PLT_BAR_ICON],
              work->cellRes[PCR_ANM_BAR_ICON],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    
    //�������̓��j���[�̂Ƃ�������̂�bgpri1
    cellInitData.pos_x = PLIST_BARICON_EXIT_X-4;
    cellInitData.anmseq = APP_COMMON_BARICON_CLOSE;
    cellInitData.bgpri = 1;
    work->clwkBarIcon[PBT_EXIT] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_BAR_ICON],
              work->cellRes[PCR_PLT_BAR_ICON],
              work->cellRes[PCR_ANM_BAR_ICON],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
  }
}

//--------------------------------------------------------------------------
//  �Z���폜
//--------------------------------------------------------------------------
static void PLIST_TermCell( PLIST_WORK *work )
{
  GFL_CLACT_WK_Remove( work->clwkBarIcon[PBT_RETURN] );
  GFL_CLACT_WK_Remove( work->clwkBarIcon[PBT_EXIT] );
  GFL_CLACT_WK_Remove( work->clwkCursor[0] );
  GFL_CLACT_WK_Remove( work->clwkCursor[1] );

  GFL_CLACT_UNIT_Delete( work->cellUnit );
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n������
//--------------------------------------------------------------------------
static void PLIST_InitMessage( PLIST_WORK *work )
{
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  work->sysFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_small_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //���b�Z�[�W
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_pokelist_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , PLIST_BG_PLT_FONT*16*2, 16*2, work->heapId );
  
  //�L���[������Ȃ������̂Œǉ�(�f�t�H���g1024
  work->printQue = PRINTSYS_QUE_CreateEx( 2048 , work->heapId );
  GFL_FONTSYS_SetDefaultColor();
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�n�J��
//--------------------------------------------------------------------------
static void PLIST_TermMessage( PLIST_WORK *work )
{
  PRINTSYS_QUE_Delete( work->printQue );
  GFL_MSG_Delete( work->msgHandle );
  GFL_FONT_Delete( work->sysFontHandle );
  GFL_FONT_Delete( work->fontHandle );
}

#pragma mark [>resource
//--------------------------------------------------------------------------
//  ���\�[�X�n�ǂݍ���
//--------------------------------------------------------------------------
static void PLIST_LoadResource( PLIST_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKELIST , work->heapId );

  ////BG���\�[�X

  //����ʋ��ʃp���b�g
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_p_list_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_hp_bar_NCLR , 
                    PALTYPE_MAIN_BG , PLIST_BG_PLT_HP_BAR*32 , 32 , work->heapId );

  //����ʔw�i
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_p_list_NCGR ,
                    PLIST_BG_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_p_list_NSCR , 
                    PLIST_BG_MAIN_BG ,  0 , 0, FALSE , work->heapId );

  //����ʃv���[�g
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_plate_NCGR ,
                    PLIST_BG_PLATE , 0 , 0, FALSE , work->heapId );
  work->plateScrRes = GFL_ARCHDL_UTIL_LoadScreen( arcHandle , NARC_pokelist_gra_list_plate_NSCR ,
                    FALSE , &work->plateScrData , work->heapId );

  //����ʃo�[  
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_bar_NCGR ,
                    PLIST_BG_PARAM , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_list_bar_NSCR , 
                    PLIST_BG_PARAM ,  0 , 0, FALSE , work->heapId );

  //���ʔw�i
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_list_sub_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_sub_NCGR ,
                    PLIST_BG_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_list_sub_NSCR , 
                    PLIST_BG_SUB_BG ,  0 , 0, FALSE , work->heapId );

  ////OBJ���\�[�X
  //�p���b�g
  work->cellRes[PCR_PLT_OBJ_COMMON] = GFL_CLGRP_PLTT_Register( arcHandle , 
        NARC_pokelist_gra_list_obj_NCLR , CLSYS_DRAW_MAIN , 
        PLIST_OBJPLT_COMMON*32 , work->heapId  );
  work->cellRes[PCR_PLT_ITEM_ICON] = GFL_CLGRP_PLTT_Register( arcHandle , 
        NARC_pokelist_gra_item_icon_NCLR , CLSYS_DRAW_MAIN , 
        PLIST_OBJPLT_ITEM_ICON*32 , work->heapId  );
  work->cellRes[PCR_PLT_CONDITION] = GFL_CLGRP_PLTT_Register( arcHandle , 
        NARC_pokelist_gra_p_st_ijou_NCLR , CLSYS_DRAW_MAIN , 
        PLIST_OBJPLT_CONDITION*32 , work->heapId  );
  work->cellRes[PCR_PLT_HP_BASE] = GFL_CLGRP_PLTT_Register( arcHandle , 
        NARC_pokelist_gra_hp_dodai_NCLR , CLSYS_DRAW_MAIN , 
        PLIST_OBJPLT_HP_BASE*32 , work->heapId  );
        
  //�L�����N�^
  work->cellRes[PCR_NCG_CURSOR] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_list_cursor_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[PCR_NCG_BALL] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_list_ball_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[PCR_NCG_ITEM_ICON] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_item_icon_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[PCR_NCG_CONDITION] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_p_st_ijou_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[PCR_NCG_HP_BASE] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_hp_dodai_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  
  //�Z���E�A�j��
  work->cellRes[PCR_ANM_CURSOR] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_list_cursor_NCER , NARC_pokelist_gra_list_cursor_NANR, work->heapId  );
  work->cellRes[PCR_ANM_BALL] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_list_ball_NCER , NARC_pokelist_gra_list_ball_NANR, work->heapId  );
  work->cellRes[PCR_ANM_ITEM_ICON] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_item_icon_NCER , NARC_pokelist_gra_item_icon_NANR, work->heapId  );
  work->cellRes[PCR_ANM_CONDITION] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_p_st_ijou_NCER , NARC_pokelist_gra_p_st_ijou_NANR, work->heapId  );
  work->cellRes[PCR_ANM_HP_BASE] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_hp_dodai_NCER , NARC_pokelist_gra_hp_dodai_NANR, work->heapId  );

  GFL_ARC_CloseDataHandle(arcHandle);
  
  //�|�P�A�C�R���p���\�[�X
  //�L�����N�^�͊e�v���[�g��
  {
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , work->heapId );
    work->cellRes[PCR_PLT_POKEICON] = GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke , 
          POKEICON_GetPalArcIndex() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_POKEICON*32 , work->heapId  );
    work->cellRes[PCR_ANM_POKEICON] = GFL_CLGRP_CELLANIM_Register( arcHandlePoke , 
          POKEICON_GetCellArcIndex() , POKEICON_GetAnmArcIndex(), work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
  //���ʑf��
  {
    ARCHANDLE *arcHandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );

    work->cellRes[PCR_PLT_BAR_ICON] = GFL_CLGRP_PLTT_Register( arcHandle , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_BAR_ICON*32 , work->heapId  );
    work->cellRes[PCR_NCG_BAR_ICON] = GFL_CLGRP_CGR_Register( arcHandle , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[PCR_ANM_BAR_ICON] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), 
          work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandleCommon);
  }
  
  //�A�j���p�ɁA�v���[�g�̑I��F�̃p���b�g��ޔ�
  GFL_STD_MemCopy16( (void*)(0x05000000+PPC_NORMAL_SELECT*32) , work->platePalAnm , 16*2 );
}


static void PLIST_ReleaseResource( PLIST_WORK *work )
{
  u8 i;
  //BG
  GFL_HEAP_FreeMemory( work->plateScrRes );
  
  //OBJ
  for( i=PCR_PLT_START ; i<=PCR_PLT_END ; i++ )
  {
    GFL_CLGRP_PLTT_Release( work->cellRes[i] );
  }
  for( i=PCR_NCG_START ; i<=PCR_NCG_END ; i++ )
  {
    GFL_CLGRP_CGR_Release( work->cellRes[i] );
  }
  for( i=PCR_ANM_START ; i<=PCR_ANM_END ; i++ )
  {
    GFL_CLGRP_CELLANIM_Release( work->cellRes[i] );
  }
}

#pragma mark [>accorde mode

//--------------------------------------------------------------------------
//  ���[�h�ʂ̏�����(�J�n��
//--------------------------------------------------------------------------
static void PLIST_InitMode( PLIST_WORK *work )
{
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
  case PL_MODE_BATTLE:
  case PL_MODE_SHINKA:
  case PL_MODE_ITEMSET:
  case PL_MODE_MAILSET:
  case PL_MODE_WAZASET:
    //�I����ʂ�
    PLIST_InitMode_Select( work );
    work->nextMainSeq = PSMS_SELECT_POKE;
    break;

  case PL_MODE_ITEMUSE:
    if( PLIST_ITEM_IsDeathRecoverAllItem( work , work->plData->item ) == TRUE )
    {
      PLIST_ITEM_UseAllDeathRecoverItem( work );
      GF_ASSERT_MSG( NULL , "PLIST mode �܂�����ĂȂ��I[%d]\n" , work->plData->mode );

      PLIST_InitMode_Select( work );
      work->nextMainSeq = PSMS_SELECT_POKE;
      work->mainSeq = PSMS_FADEIN;
    }
    else
    {
      //�I����ʂ�
      PLIST_InitMode_Select( work );
      work->nextMainSeq = PSMS_SELECT_POKE;
    }
    break;

  case PL_MODE_WAZASET_RET:
    work->pokeCursor = work->plData->ret_sel;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
    if( work->plData->waza_pos < 4 )
    {
      //�Z�����肵��
      const u32 wazaNo = PP_Get( work->selectPokePara , ID_PARA_waza1+work->plData->waza_pos , NULL );
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , wazaNo );
      PLIST_MessageWaitInit( work , mes_pokelist_04_10 , TRUE , PSTATUS_MSGCB_ForgetSkill_SkillForget );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    else
    {
      //�I���L�����Z�����o����̂�������߂܂����H
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_07 , FALSE , PSTATUS_MSGCB_ForgetSkill_SkillCancel );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    work->nextMainSeq = PSMS_MSG_WAIT;
    work->mainSeq = PSMS_FADEIN;
    work->canExit = FALSE;
    break;
    
  case PL_MODE_ITEMSET_RET:
    work->pokeCursor = work->plData->ret_sel;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
    if( work->plData->item == 0 )
    {
      //�L�����Z�����ꂽ
      //�I����ʂ�
      //���łɃ��[�h���t�B�[���h�ɖ߂��Ă��܂�
      work->plData->mode = PL_MODE_FIELD;
      PLIST_InitMode_Select( work );
      work->nextMainSeq = PSMS_SELECT_POKE;
    }
    else
    {
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , work->plData->item );
        PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
        PP_Put( work->selectPokePara , ID_PARA_item , work->plData->item );
        PSTATUS_SubItem( work , work->plData->item );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
        //���łɃ��[�h���t�B�[���h�ɖ߂��Ă��܂�
        work->plData->mode = PL_MODE_FIELD;
      }
      else
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_28 , FALSE , PSTATUS_MSGCB_ItemSet_CheckChangeItem );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
      }
    }
    work->nextMainSeq = PSMS_MSG_WAIT;
    work->mainSeq = PSMS_FADEIN;
    work->canExit = TRUE;
    break;

  default:
    GF_ASSERT_MSG( NULL , "PLIST mode �܂�����ĂȂ��I[%d]\n" , work->plData->mode );
    break;
  }}

//--------------------------------------------------------------------------
//  ���[�h�ʂ̏�����(�|�P�����I��
//--------------------------------------------------------------------------
static void PLIST_InitMode_Select( PLIST_WORK *work )
{
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_01 );
    work->canExit = TRUE;
    break;
    
  case PL_MODE_BATTLE:
//    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
//    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_06 );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
    work->canExit = FALSE;
    break;
    
  case PL_MODE_ITEMUSE:
  case PL_MODE_SHINKA:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_04 );
    work->canExit = FALSE;
    break;
  
  case PL_MODE_ITEMSET:
  case PL_MODE_MAILSET:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_03 );
    work->canExit = FALSE;
    break;
  
  case PL_MODE_WAZASET:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_05 );
    work->canExit = FALSE;
    break;

  default:
    GF_ASSERT_MSG( NULL , "PLIST mode �܂�����ĂȂ��I[%d]\n" , work->plData->mode );
    break;
  }
  PLIST_SelectPokeInit( work );
}
//--------------------------------------------------------------------------
//  ���[�h�ʂ̊J��(�|�P�����I��
//--------------------------------------------------------------------------
static void PLIST_TermMode_Select_Decide( PLIST_WORK *work )
{
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
  case PL_MODE_BATTLE:
    PLIST_SelectMenuInit( work );
    //���ňꏏ�Ƀ��j���[���J��
    PLIST_InitMode_Menu( work );
    
    break;
    
  case PL_MODE_ITEMUSE:
    {
      const PLIST_ITEM_USE_CHECK ret = PLIST_ITEM_CanUseRecoverItem( work , work->plData->item , work->selectPokePara );
      switch( ret )
      {
      case PIUC_OK:  //�g����
        break;
        
      case PIUC_NG:  //�g���Ȃ�
        PLIST_ITEM_MSG_CanNotUseItem( work );
        break;
        
      case PIUC_SELECT_SKILL:  //�X�L���I����
        break;
        
      }
    }
    break;
    
  case PL_MODE_SHINKA:
    {
      work->plData->ret_mode = PL_RET_BAG;
      
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 0 , work->plData->item );
      PLIST_MessageWaitInit( work , mes_pokelist_deb_01 , TRUE , PSTATUS_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    break;

  case PL_MODE_ITEMSET:
  case PL_MODE_MAILSET:
    {
      //FIXME ���[������
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        work->plData->ret_mode = PL_RET_BAG;
        
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , work->plData->item );
        PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PSTATUS_MSGCB_ExitCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
        PP_Put( work->selectPokePara , ID_PARA_item , work->plData->item );
        PSTATUS_SubItem( work , work->plData->item );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      }
      else
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_28 , FALSE , PSTATUS_MSGCB_ItemSet_CheckChangeItem );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
      }
    }
    break;

  case PL_MODE_WAZASET:
    switch( PLIST_UTIL_CheckLearnSkill( work , work->selectPokePara ) )
    {
    case LSCL_OK:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PSTATUS_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      
      PSTATUS_LearnSkillEmpty( work , work->selectPokePara );
      if( work->plData->item != 0 )
      {
        PSTATUS_SubItem( work , work->plData->item );
      }
      break;
    
    case LSCL_OK_FULL:

      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_06 , FALSE , PSTATUS_MSGCB_ForgetSkill_ForgetCheck );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      break;

    case LSCL_NG:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_12 , TRUE , PSTATUS_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      
      break;

    case LSCL_LEARN:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_13 , TRUE , PSTATUS_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      break;

    }

    break;
  
  default:
    PLIST_SelectMenuInit( work );
    //���ňꏏ�Ƀ��j���[���J��
    PLIST_InitMode_Menu( work );
    GF_ASSERT_MSG( NULL , "PLIST mode �܂�����ĂȂ��I[%d]\n" , work->plData->mode );
    break;
  }
}

//--------------------------------------------------------------------------
//  ���[�h�ʂ̏�����(���j���[
//--------------------------------------------------------------------------
static void PLIST_InitMode_Menu( PLIST_WORK *work )
{
  PLIST_MENU_ITEM_TYPE itemArr[8];
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
    itemArr[0] = PMIT_STATSU;
    itemArr[1] = PMIT_HIDEN;
    itemArr[2] = PMIT_CHANGE;
    itemArr[3] = PMIT_ITEM;
    itemArr[4] = PMIT_CLOSE;
    itemArr[5] = PMIT_END_LIST;
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );

    break;
  case PL_MODE_BATTLE:
    itemArr[0] = PMIT_SET_JOIN;
    itemArr[1] = PMIT_STATSU;
    itemArr[2] = PMIT_CLOSE;
    itemArr[3] = PMIT_END_LIST;
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );

    break;
    
  default:
    GF_ASSERT_MSG( NULL , "PLIST mode �܂�����ĂȂ��I[%d]\n" , work->plData->mode );
    itemArr[0] = PMIT_STATSU;
    itemArr[1] = PMIT_CLOSE;
    itemArr[2] = PMIT_END_LIST;
    break;
  }
  PLIST_MENU_OpenMenu( work , work->menuWork , itemArr );

}

#pragma mark [>PokeSelect

//--------------------------------------------------------------------------
//  �|�P�����I�����
//--------------------------------------------------------------------------
static void PLIST_SelectPoke( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_INIT:
    work->subSeq = PSSS_MAIN;
    work->selectState = PSSEL_NONE;
    break;
    
  case PSSS_MAIN:
    PLIST_SelectPokeMain( work );
    break;
    
  case PSSS_TERM:
    if( work->mainSeq == PSMS_CHANGE_POKE )
    {
      PLIST_SelectPokeTerm_Change( work );
    }
    else
    {
      PLIST_SelectPokeTerm( work );
    }
    break;
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʏ�����
//--------------------------------------------------------------------------
static void PLIST_SelectPokeInit( PLIST_WORK *work )
{
  work->pokeCursor = work->plData->ret_sel;
  work->subSeq  = PSSS_INIT;
  work->menuRet = PMIT_NONE;
  
//  if( work->ktst == GFL_APP_KTST_KEY || 
//      GFL_CLACT_WK_GetDrawEnable( work->clwkCursor[0] ) == TRUE )
  if( work->ktst == GFL_APP_KTST_KEY )
  {
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      PLIST_SelectPokeSetCursor( work , work->pokeCursor );

      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
    }
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      //�ʏ펞��TP�̓v���[�g��A�N�e�B�u�B�������̓A�N�e�B�u
      if( work->mainSeq == PSMS_CHANGE_POKE )
      {
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
      }
      else
      {
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      }
    }
  }
  
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , work->canExit );
  
  //�o�g������
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );

    work->btlMenuWin[0] = PLIST_MENU_CreateMenuWin_BattleMenu( work , work->menuWork ,
                              mes_pokelist_01_01 , PLIST_BATTLE_BUTTON_DECIDE_X , PLIST_BATTLE_BUTTON_DECIDE_Y , FALSE );
    work->btlMenuWin[1] = PLIST_MENU_CreateMenuWin_BattleMenu( work , work->menuWork ,
                              mes_pokelist_01_02 , PLIST_BATTLE_BUTTON_CANCEL_X , PLIST_BATTLE_BUTTON_CANCEL_Y , TRUE );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , TRUE );
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʊJ��
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  //�o�g������
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    u8 i;
    GFL_BMPWIN_Delete( work->btlMenuWin[0] );
    GFL_BMPWIN_Delete( work->btlMenuWin[1] );
    GFL_BG_FillScreen( PLIST_BG_MENU , 0 , 0 , 21 , 32 , 3 , GFL_BG_SCRWRT_PALNL );
    
    //���ł�in_num�փZ�b�g
    for(i=0;i<6;i++ )
    {
      work->plData->in_num[i] = 0;
    }
    for(i=0;i<PLIST_LIST_MAX;i++ )
    {
      const PLIST_PLATE_BATTLE_ORDER order = PLIST_PLATE_GetBattleOrder( work->plateWork[i] );
      if( order <= PPBO_JOIN_6 )
      {
        work->plData->in_num[order] = i+1;
      }
    }
  }

  switch( work->selectState )
  {
  case PSSEL_SELECT:
    //�J�[�\���͔�\��
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );

    /*
    //�J�[�\����ύX
    if( work->pokeCursor == PL_SEL_POS_POKE1 )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_SELECT_A );
    }
    else
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_SELECT_B );
    }
    GFL_CLACT_WK_SetBgPri( work->clwkCursor[0] , 2 );
    */
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor );
    work->plData->ret_sel = work->pokeCursor;
    PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    
    PLIST_TermMode_Select_Decide( work );
    
    break;

  case PSSEL_DECIDE:
    if(  PLIST_UTIL_IsBattleMenu(work) == TRUE )
    {
      PLIST_SelectPokeTerm_Battle( work );
    }
    else
    {
      work->mainSeq = PSMS_FADEOUT;
      work->plData->ret_sel = PL_SEL_POS_ENTER;
      work->plData->ret_mode = PL_RET_NORMAL;

      PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    }
    break;
    
  case PSSEL_RETURN:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = PL_SEL_POS_EXIT;
    work->plData->ret_mode = PL_RET_NORMAL;
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    break;

  case PSSEL_EXIT:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = PL_SEL_POS_EXIT2;
    work->plData->ret_mode = PL_RET_NORMAL;
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    break;
  }
}

//--------------------------------------------------------------------------
//  ����ւ��p�|�P�����I����ʊJ��
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm_Change( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  //�����v���[�g��I�񂾂̂ŃL�����Z������
  if( work->selectState == PSSEL_SELECT &&
      work->pokeCursor == work->changeTarget )
  {
    work->selectState = PSSEL_RETURN;
  }

  switch( work->selectState )
  {
  case PSSEL_SELECT:
    PLIST_ChangeAnimeInit( work );
    PMSND_PlaySystemSE( PLIST_SND_CHANGE );
    break;
    
  case PSSEL_RETURN:
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    PLIST_PLATE_SetActivePlate( work , work->plateWork[work->changeTarget] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
    work->changeTarget = PL_SEL_POS_MAX;
    work->plData->ret_sel = work->pokeCursor;

    PLIST_InitMode_Select( work );
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    break;

  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʊJ�� �o�g�����莞
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm_Battle( PLIST_WORK *work )
{
  if( work->plData->in_min > work->btlJoinNum )
  {
    PLIST_MessageWaitInit( work , mes_pokelist_04_60_1 + (work->plData->in_min-1) , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
  }
  else
  if( work->plData->in_max < work->btlJoinNum )
  {
    PLIST_MessageWaitInit( work , mes_pokelist_04_62_1 + (work->plData->in_max-1) , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
  }
  else
  {
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = PL_SEL_POS_ENTER;
    work->plData->ret_mode = PL_RET_NORMAL;

    PMSND_PlaySystemSE( PLIST_SND_DECIDE );
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʍX�V
//--------------------------------------------------------------------------
static void PLIST_SelectPokeMain( PLIST_WORK *work )
{
  PLIST_SelectPokeUpdateKey( work );
  //�L�[�ɂ�錈�肪����������TP������
  if( work->selectState == PSSEL_NONE )
  {
    PLIST_SelectPokeUpdateTP( work );
  }

  if( work->selectState != PSSEL_NONE )
  {
    work->subSeq = PSSS_TERM;
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����� �L�[����
//--------------------------------------------------------------------------
static void PLIST_SelectPokeUpdateKey( PLIST_WORK *work )
{
  const int trg = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();
  if( work->ktst == GFL_APP_KTST_TOUCH && 
      GFL_CLACT_WK_GetDrawEnable( work->clwkCursor[0] ) == FALSE )
  {
    //��\���̂Ƃ�����0�ɕ\������悤��
    if( trg != 0 )
    {
      //�J�[�\����0�̈ʒu�ɕ\��
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      work->pokeCursor = PL_SEL_POS_POKE1;
      PLIST_SelectPokeSetCursor( work , work->pokeCursor );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );

      //PLIST_PLATE_SetActivePlatePos( work , work->pokeCursor );
      
      work->ktst = GFL_APP_KTST_KEY;
    }
  }
  else
  {
    s8 moveVal = 0;
    if( trg != 0 )
    {
      work->ktst = GFL_APP_KTST_KEY;
    }
    //�L�[����
    if( repeat & PAD_KEY_UP )
    {
      moveVal = -2;
    }
    else
    if( repeat & PAD_KEY_DOWN )
    {
      moveVal = 2;
    }
    else
    if( repeat & PAD_KEY_LEFT )
    {
      moveVal = -1;
    }
    else
    if( repeat & PAD_KEY_RIGHT )
    {
      moveVal = 1;
    }
    else if( trg & PAD_BUTTON_A )
    {
      work->selectState = PSSEL_SELECT;
    }
    else if( trg & PAD_BUTTON_B )
    {
      work->selectState = PSSEL_RETURN;
    }
    else if( trg & PAD_BUTTON_X )
    {
      if( work->canExit == TRUE )
      {
        work->selectState = PSSEL_EXIT;
      }
    }
    else 
    if( PLIST_UTIL_IsBattleMenu(work) == TRUE &&
        trg & PAD_BUTTON_START )
    {
      work->selectState = PSSEL_DECIDE;
    }

    
    //�\���L�[�ړ�����
    if( moveVal != 0 )
    {
      const PL_SELECT_POS befPos = work->pokeCursor;
      BOOL isFinish = FALSE;
      PMSND_PlaySystemSE( PLIST_SND_CURSOR );
      //�v���[�g������ʒu�܂Ń��[�v
      while( isFinish == FALSE )
      {
        if( work->pokeCursor + moveVal > PL_SEL_POS_POKE6 )
        {
          work->pokeCursor = work->pokeCursor+moveVal-(PL_SEL_POS_POKE6+1);
        }
        else
        if( work->pokeCursor + moveVal < PL_SEL_POS_POKE1 )
        {
          work->pokeCursor = work->pokeCursor+(PL_SEL_POS_POKE6+1)+moveVal;
        }
        else
        {
          work->pokeCursor += moveVal;
        }

        if( PLIST_PLATE_CanSelect( work , work->plateWork[work->pokeCursor] ) == TRUE )
        {
          isFinish = TRUE;
        }
      }
      //�\������X�V
      if( befPos != work->pokeCursor )
      {
        PLIST_SelectPokeSetCursor( work , work->pokeCursor );
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
        PLIST_PLATE_SetActivePlate( work , work->plateWork[befPos] , FALSE );
        work->platePalAnmCnt = PLIST_PLATE_ACTIVE_ANM_CNT;

        //PLIST_PLATE_SetActivePlatePos( work , work->pokeCursor );
      }
    }
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����� TP����
//--------------------------------------------------------------------------
static void PLIST_SelectPokeUpdateTP( PLIST_WORK *work )
{
  //�v���[�g�Ɠ�����
  int ret;
  
  {
    //�����蔻��쐬
    u8 i;
    u8 cnt = 0;
    u8 plateIdx[PLIST_LIST_MAX];
    GFL_UI_TP_HITTBL hitTbl[PLIST_LIST_MAX+1];
    
    for( i=0;i<PLIST_LIST_MAX;i++ )
    {
      if( PLIST_PLATE_CanSelect( work , work->plateWork[i] ) == TRUE )
      {
        plateIdx[cnt] = i;
        PLIST_PLATE_GetPlateRect( work , work->plateWork[i] , &hitTbl[cnt] );
        cnt++;
      }
    }
    hitTbl[cnt].circle.code = GFL_UI_TP_HIT_END;

    ret = GFL_UI_TP_HitTrg( hitTbl );

    if( ret != GFL_UI_TP_HIT_NONE )
    {
      const PL_SELECT_POS befPos = work->pokeCursor;
      work->selectState = PSSEL_SELECT;
      work->pokeCursor = plateIdx[ret];

      //PLIST_SelectPokeSetCursor( work , work->pokeCursor );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[befPos] , FALSE );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
      work->platePalAnmCnt = PLIST_PLATE_ACTIVE_ANM_CNT;

      work->ktst = GFL_APP_KTST_TOUCH;
    }
  }
  
  //���^�[���L�����Z���Ƃ��Ɠ�����
  if( ret == GFL_UI_TP_HIT_NONE )
  {
    //FIXME �{���̓��[�h���򂪂���
    GFL_UI_TP_HITTBL hitTbl[PSSEL_BUTTON_NUM+1] =
    {
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { GFL_UI_TP_HIT_END,0,0,0 },
    };
    
    //�o�g���p
    if( PLIST_UTIL_IsBattleMenu( work ) == TRUE )
    {
      hitTbl[PSSEL_RETURN].rect.top    = PLIST_BATTLE_BUTTON_CANCEL_Y*8;    
      hitTbl[PSSEL_RETURN].rect.bottom = (PLIST_BATTLE_BUTTON_CANCEL_Y+3)*8;
      hitTbl[PSSEL_RETURN].rect.left   = PLIST_BATTLE_BUTTON_CANCEL_X*8;
      hitTbl[PSSEL_RETURN].rect.right  = 255;//(PLIST_BATTLE_BUTTON_CANCEL_X+13)*8;

      hitTbl[PSSEL_DECIDE].rect.top    = PLIST_BATTLE_BUTTON_DECIDE_Y*8;
      hitTbl[PSSEL_DECIDE].rect.bottom = (PLIST_BATTLE_BUTTON_DECIDE_Y+3)*8;
      hitTbl[PSSEL_DECIDE].rect.left   = PLIST_BATTLE_BUTTON_DECIDE_X*8;     
      hitTbl[PSSEL_DECIDE].rect.right  = (PLIST_BATTLE_BUTTON_DECIDE_X+13)*8;
    }
    else
    {
      hitTbl[PSSEL_RETURN].rect.top    = PLIST_BARICON_Y - 12;
      hitTbl[PSSEL_RETURN].rect.bottom = PLIST_BARICON_Y + 12;
      hitTbl[PSSEL_RETURN].rect.left   = PLIST_BARICON_RETURN_X_BAR - 12;
      hitTbl[PSSEL_RETURN].rect.right  = PLIST_BARICON_RETURN_X_BAR + 12;
      if( work->canExit == TRUE )
      {
        hitTbl[PSSEL_EXIT  ].rect.top    = PLIST_BARICON_Y - 12;
        hitTbl[PSSEL_EXIT  ].rect.bottom = PLIST_BARICON_Y + 12;
        hitTbl[PSSEL_EXIT  ].rect.left   = PLIST_BARICON_EXIT_X - 12;
        hitTbl[PSSEL_EXIT  ].rect.right  = PLIST_BARICON_EXIT_X + 12;
      }
    }
    
    ret = GFL_UI_TP_HitTrg( hitTbl );

    if( ret != GFL_UI_TP_HIT_NONE )
    {
      work->selectState = ret;
      work->ktst = GFL_APP_KTST_TOUCH;
    }
  }
  
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʁ@�J�[�\���ݒ�
//--------------------------------------------------------------------------
static void PLIST_SelectPokeSetCursor( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  GFL_CLACTPOS curPos;
  PLIST_PLATE_GetPlatePosition(work , work->plateWork[pos] , &curPos );
  GFL_CLACT_WK_SetPos( work->clwkCursor[0] , &curPos , CLSYS_DRAW_MAIN );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , TRUE );
  GFL_CLACT_WK_SetBgPri( work->clwkCursor[0] , 3 );

  if( pos == PL_SEL_POS_POKE1 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_NORMAL_A );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_NORMAL_B );
  }
}

//--------------------------------------------------------------------------
//  �|�P�����I����ʁ@����ւ��悤�J�[�\���ݒ�
//--------------------------------------------------------------------------
static void PLIST_SelectPokeSetCursor_Change( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  GFL_CLACTPOS curPos;
  PLIST_PLATE_GetPlatePosition(work , work->plateWork[pos] , &curPos );
  GFL_CLACT_WK_SetPos( work->clwkCursor[1] , &curPos , CLSYS_DRAW_MAIN );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , TRUE );
  GFL_CLACT_WK_SetBgPri( work->clwkCursor[1] , 2 );

  if( pos == PL_SEL_POS_POKE1 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[1] , PCA_CHANGE_A );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[1] , PCA_CHANGE_B );
  }
}

static void PLIST_PLATE_SetActivePlatePos( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  //���ݖ��g�p�B�v���[�g�I�����ړ�
  const s8 moveVal[7][6][2] =
  {
    {
      { 0,0 },{ 1,0 },
      { 0,1 },{ 1,1 },
      { 0,1 },{ 1,1 },
    },
    {
      { -1,0 },{ 0,0 },
      { -1,1 },{ 0,1 },
      { -1,1 },{ 0,1 },
    },
    {
      { 0,-1 },{ 1,-1 },
      { 0,0 } ,{ 1,0 },
      { 0,1 } ,{ 1,1 },
    },
    {
      { -1,-1 },{ 0,-1 },
      { -1,0 } ,{ 0,0 },
      { -1,1 } ,{ 0,1 },
    },
    {
      { 0,-1 },{ 1,-1 },
      { 0,-1 },{ 1,-1 },
      { 0,0 } ,{ 1,0 },
    },
    {
      { -1,-1 },{ 0,-1 },
      { -1,-1 },{ 0,-1 },
      { -1,0 } ,{ 0,0 },
    },
    {
      { 0,0 },{ 0,0 },
      { 0,0 },{ 0,0 },
      { 0,0 },{ 0,0 },
    },
  };
  u8 i;
#if DEBUG_ONLY_FOR_ariizumi_nobuhiko | DEBUG_ONLY_FOR_ibe_mana
  GFL_BG_FillScreen( PLIST_BG_PARAM , 0 ,16,0,48,21,GFL_BG_SCRWRT_PALNL);
  GFL_BG_FillScreen( PLIST_BG_PLATE , 0 ,16,0,48,21,GFL_BG_SCRWRT_PALNL);
  for( i=0;i<6;i++ )
  {
    if( i%2 == work->pokeCursor%2 )
    {
      PLIST_PLATE_MovePlateXY( work , work->plateWork[i] , /*moveVal[pos][i][0]*/0 , moveVal[pos][i][1] );
    }
    else
    {
      PLIST_PLATE_MovePlateXY( work , work->plateWork[i] , 0 , 0 );
    }
  }
#endif
}

#pragma mark [>MenuSelect
//--------------------------------------------------------------------------
//  ���j���[�I�����
//--------------------------------------------------------------------------
static void PLIST_SelectMenu( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_INIT:
    PLIST_SelectMenuInit( work );
    work->subSeq = PSSS_MAIN;
    break;
    
  case PSSS_MAIN:
    PLIST_MENU_UpdateMenu( work , work->menuWork );
    {
      const PLIST_MENU_ITEM_TYPE ret = PLIST_MENU_IsFinish( work , work->menuWork );
      if( ret != PMIT_NONE )
      {
        work->menuRet = ret;
        work->subSeq = PSSS_TERM;
      }
    }
    break;
    
  case PSSS_TERM:
    PLIST_SelectMenuTerm( work );
    
    PLIST_SelectMenuExit( work );
    break;
  }
  
}

//--------------------------------------------------------------
//	���j���[������
//--------------------------------------------------------------
static void PLIST_SelectMenuInit( PLIST_WORK *work )
{
  work->mainSeq = PSMS_MENU;
  work->subSeq  = PSSS_INIT;

  //BG�E�v���[�g�E�p�����[�^�������ɂ�������
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , 8 );
  //�߂�A�C�R�������J���[���ʂ�������Ȃ��悤��Window���g��
  //�߂�A�C�R����BG�ɏ�����̂őΉ��s�v�B�����ǒʐM�A�C�R���̂��ߎc��
  //FIXME �ʐM�A�C�R���Ή�
  G2_SetWnd0Position( PLIST_BARICON_RETURN_X_MENU , PLIST_BARICON_Y ,
                      PLIST_BARICON_RETURN_X_MENU + 24, PLIST_BARICON_Y + 24 );
  G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , FALSE );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , TRUE );
  work->isActiveWindowMask = TRUE;

  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  }
  GFL_BG_LoadScreenV_Req(PLIST_BG_MENU);
  
  GFL_UI_SetTouchOrKey( work->ktst );
}

//--------------------------------------------------------------
//	���j���[�J��
//--------------------------------------------------------------
static void PLIST_SelectMenuTerm( PLIST_WORK *work )
{
  PLIST_MENU_CloseMenu( work , work->menuWork );
  PLIST_MSG_CloseWindow( work , work->msgWork );
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , TRUE );
  }
  
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                    GX_BLEND_PLANEMASK_BG3 ,
                    16 , 10 );
  work->isActiveWindowMask = FALSE;
  GFL_BG_ClearScreenCodeVReq(PLIST_BG_MENU,0);
  work->ktst = GFL_UI_CheckTouchOrKey();

  
}

//--------------------------------------------------------------
//	���j���[�߂蕪��
//--------------------------------------------------------------
static void PLIST_SelectMenuExit( PLIST_WORK *work )
{
  switch( work->menuRet )
  {
  case PMIT_CLOSE:
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;

  case PMIT_CHANGE:
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    PLIST_SelectPokeSetCursor_Change( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->changeTarget = work->pokeCursor;
    work->mainSeq = PSMS_CHANGE_POKE;
    work->canExit = FALSE;
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_02 );
    //�X�̓��[�h�ʏ����ł͂Ȃ��A�����̃Z���N�g������
    PLIST_SelectPokeInit( work );
    
    break;

  case PMIT_ITEM:
    {
      PLIST_MENU_ITEM_TYPE itemArr[4] = {PMIT_GIVE,PMIT_TAKE,PMIT_CLOSE,PMIT_END_LIST};
      PLIST_SelectMenuInit( work );
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_02 );
      PLIST_MENU_OpenMenu( work , work->menuWork , itemArr );
    }
    break;

  case PMIT_STATSU:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_STATUS;
    break;
    
  case PMIT_RET_JOIN:
    PLIST_PLATE_SetBattleOrder( work , work->plateWork[work->pokeCursor] , work->btlJoinNum );
    work->btlJoinNum++;
    
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;
    
  case PMIT_JOIN_CANCEL:
    {
      u8 i;
      const PLIST_PLATE_BATTLE_ORDER order = PLIST_PLATE_GetBattleOrder( work->plateWork[work->pokeCursor] );
      for( i=0;i<PLIST_LIST_MAX;i++ )
      {
        const PLIST_PLATE_BATTLE_ORDER checkOrder = PLIST_PLATE_GetBattleOrder( work->plateWork[i] );
        if( checkOrder <= PPBO_JOIN_6 && checkOrder > order )
        {
          PLIST_PLATE_SetBattleOrder( work , work->plateWork[i] , checkOrder-1 );
        }
      }
      PLIST_PLATE_SetBattleOrder( work , work->plateWork[work->pokeCursor] , PPBO_JOIN_OK );
      work->btlJoinNum--;
    }

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;
    
  case PMIT_TAKE:
    {
      //FIXME ���[������
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MessageWaitInit( work , mes_pokelist_04_29 , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
      }
      else
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_30 , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
        PP_Put( work->selectPokePara , ID_PARA_item , 0 );
        PSTATUS_AddItem( work , itemNo );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      }
    }
    break;

  case PMIT_GIVE:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_ITEMSET;
    break;

  case PMIT_WAZA_1:
  case PMIT_WAZA_2:
  case PMIT_WAZA_3:
  case PMIT_WAZA_4:
    OS_TPrintf("�܂�����ĂȂ��I\n");
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;

  default:
    GF_ASSERT_MSG(0,"�܂�����ĂȂ��I\n");

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;
  }
}

//���b�Z�[�W�҂�
#pragma mark [>MessageWait
static void PLIST_MessageWait( PLIST_WORK *work )
{
  if( PLIST_MSG_IsFinishMessage( work , work->msgWork ) == TRUE )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ||
        GFL_UI_TP_GetTrg() == TRUE ||
        work->isMsgWaitKey == FALSE )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
      {
        work->ktst = GFL_APP_KTST_KEY;
      }
      if( GFL_UI_TP_GetTrg() == TRUE )
      {
        work->ktst = GFL_APP_KTST_TOUCH;
      }
      
      work->msgCallBack(work);
    }
  }
}

void PLIST_MessageWaitInit( PLIST_WORK *work , u32 msgId , const BOOL isWaitKey , PSTATUS_CallbackFunc msgCallBack )
{
  work->mainSeq = PSMS_MSG_WAIT;
  work->isMsgWaitKey = isWaitKey;
  work->msgCallBack = msgCallBack;
  PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MESSAGE );
  PLIST_MSG_DrawMessageStream( work , work->msgWork , msgId );
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , FALSE );
  
}

//�͂��E����������
#pragma mark [>YesNoWait
static void PLIST_YesNoWait( PLIST_WORK *work )
{
  PLIST_MENU_ITEM_TYPE ret;
  PLIST_MENU_UpdateMenu( work , work->menuWork );
  
  ret = PLIST_MENU_IsFinish( work , work->menuWork );
  if( ret != PMIT_NONE )
  {
    work->yesNoCallBack(work,ret);
    PLIST_MENU_CloseMenu( work , work->menuWork );
  }

}

static void PLIST_YesNoWaitInit( PLIST_WORK *work , PSTATUS_CallbackFuncYesNo yesNoCallBack )
{
  work->mainSeq = PSMS_YESNO_WAIT;
  work->yesNoCallBack = yesNoCallBack;

  PLIST_MENU_OpenMenu_YesNo( work , work->menuWork );
  
}

//����ւ��A�j��
#pragma mark [>ChangeAnm
//--------------------------------------------------------------
//	����ւ��A�j��
//--------------------------------------------------------------
static void PLIST_ChangeAnime( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_MOVE_OUT:
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->changeTarget] , work->anmCnt );
    work->anmCnt += PLIST_CHANGE_ANM_VALUE;
    PLIST_PLATE_MovePlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_MovePlate( work , work->plateWork[work->changeTarget] , work->anmCnt );

    if( work->anmCnt >= PLIST_CHANGE_ANM_COUNT )
    {
      PokeParty_ExchangePosition( work->plData->pp , work->pokeCursor , work->changeTarget , work->heapId);
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor);
        PLIST_PLATE_ResetParam(work , work->plateWork[work->pokeCursor] , pp , work->anmCnt );
      }
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, work->changeTarget);
        PLIST_PLATE_ResetParam(work , work->plateWork[work->changeTarget] , pp , work->anmCnt );
      }
      //����ւ�
      work->subSeq = PSSS_MOVE_IN;
    }
    break;

  case PSSS_MOVE_IN:
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->changeTarget] , work->anmCnt );
    work->anmCnt -= PLIST_CHANGE_ANM_VALUE;
    PLIST_PLATE_MovePlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_MovePlate( work , work->plateWork[work->changeTarget] , work->anmCnt );

    if( work->anmCnt <= 0 )
    {
      //�I��
      PLIST_ChangeAnimeTerm( work );
    }
    
    break;
  }
}

//--------------------------------------------------------------
//	����ւ��A�j��������
//--------------------------------------------------------------
static void PLIST_ChangeAnimeInit( PLIST_WORK *work )
{
  work->mainSeq = PSMS_CHANGE_ANM;
  work->subSeq = PSSS_MOVE_OUT;
  work->anmCnt = 0;

  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
}

//--------------------------------------------------------------
//	����ւ��A�j���J��
//--------------------------------------------------------------
static void PLIST_ChangeAnimeTerm( PLIST_WORK *work )
{
  work->selectPokePara = NULL;
  work->mainSeq = PSMS_SELECT_POKE;

  PLIST_SelectPokeSetCursor( work , work->pokeCursor );
  PLIST_PLATE_SetActivePlate( work , work->plateWork[work->changeTarget] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
  work->changeTarget = PL_SEL_POS_MAX;
  work->plData->ret_sel = work->pokeCursor;

  PLIST_InitMode_Select( work );
  
}

//--------------------------------------------------------------
//	����ւ��A�j���v���[�g�X�V
//--------------------------------------------------------------
static void PLIST_ChangeAnimeUpdatePlate( PLIST_WORK *work )
{
}

#pragma mark [>changeProc
//--------------------------------------------------------------
//	Proc�ύX������
//--------------------------------------------------------------
static void PLIST_ChangeProcInit( PLIST_WORK *work , GFL_PROC_DATA *procData , FSOverlayID overlayId , void *parentWork )
{
  work->procData = procData;
  work->procOverlayId = overlayId;
  work->procParentWork = parentWork;
  work->changeProcSeq = PSCS_INIT;

  work->mainSeq = PSMS_CHANGEPROC;
  work->subSeq = PSSS_FADEOUT;
}

//--------------------------------------------------------------
//	Proc�ύX���C��
//--------------------------------------------------------------
static void PLIST_ChangeProcUpdate( PLIST_WORK *work )
{
  switch( work->subSeq )
  {   
  case PSSS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->subSeq = PSSS_INIT;
  
    break;
    
  case PSSS_INIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->reqChangeProc = TRUE;
    }
    break;
/*    
  case PSSS_MAIN:
    if( work->procParentWork != NULL )
    {
      GFL_HEAP_FreeMemory( work->procParentWork );
    }
    PLIST_InitPokeList( work );
    //���mainSeq���؂�ւ��̂ł����ŏI���B
    break;
*/    
  }
}

#pragma mark [>util
//--------------------------------------------------------------
//	�w��Z�ӏ��Ƀt�B�[���h�Z�����邩�H
//--------------------------------------------------------------
u32 PLIST_UTIL_CheckFieldWaza( const POKEMON_PARAM *pp , const u8 wazaPos )
{
  const u32 wazaID = PP_Get( pp , ID_PARA_waza1+wazaPos , NULL );
  
  switch( wazaID )
  {
  case WAZANO_IAIGIRI: //����������(15)
    return PL_RET_IAIGIRI;
    break;
  case WAZANO_SORAWOTOBU: //������Ƃ�(19)
    return PL_RET_SORAWOTOBU;
    break;
  case WAZANO_NAMINORI: //�Ȃ݂̂�(57)
    return PL_RET_NAMINORI;
    break;
  case WAZANO_KAIRIKI: //�����肫(70)
    return PL_RET_KAIRIKI;
    break;
  case WAZANO_KIRIBARAI: //����΂炢(432)
    return PL_RET_KIRIBARAI;
    break;
  case WAZANO_IWAKUDAKI: //���킭����(249)
    return PL_RET_IWAKUDAKI;
    break;
  case WAZANO_TAKINOBORI: //�����̂ڂ�(127)
    return PL_RET_TAKINOBORI;
    break;
  case WAZANO_ROKKUKURAIMU: //���b�N�N���C��(431)
    return PL_RET_ROCKCLIMB;
    break;

  case WAZANO_HURASSYU: //�t���b�V��(148)
    return PL_RET_FLASH;
    break;
  case WAZANO_TEREPOOTO: //�e���|�[�g(100)
    return PL_RET_TELEPORT;
    break;
  case WAZANO_ANAWOHORU: //���Ȃ��ق�(91)
    return PL_RET_ANAWOHORU;
    break;
  case WAZANO_AMAIKAORI: //���܂�������(230)
    return PL_RET_AMAIKAORI;
    break;
  case WAZANO_OSYABERI: //������ׂ�(448)
    return PL_RET_OSYABERI;
    break;
  case WAZANO_MIRUKUNOMI: //�~���N�̂�(208)
    return PL_RET_MILKNOMI;
    break;
  case WAZANO_TAMAGOUMI: //�^�}�S����(135)
    return PL_RET_TAMAGOUMI;
    break;

  default:
    return 0;
    break;
  }
}

//--------------------------------------------------------------
//	�o�g���̎Q���I�����H
//--------------------------------------------------------------
const BOOL PLIST_UTIL_IsBattleMenu( const PLIST_WORK *work )
{
  if( work->plData->mode == PL_MODE_BATTLE ||
      work->plData->mode == PL_MODE_BATTLE_STAGE  ||
      work->plData->mode == PL_MODE_BATTLE_CASTLE ||
      work->plData->mode == PL_MODE_BATTLE_ROULETTE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//	���n����Ă���Z���o�����邩�H
//--------------------------------------------------------------
const PLIST_SKILL_CAN_LEARN PLIST_UTIL_CheckLearnSkill( PLIST_WORK *work , const POKEMON_PARAM *pp )
{
  u8 i;
  BOOL isEmpty = FALSE;
  //�����Z������H
  for( i=0;i<4;i++ )
  {
    const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+i , NULL );
    if( wazaNo == work->plData->waza )
    {
      return LSCL_LEARN;
    }
    if( wazaNo == 0 )
    {
      isEmpty = TRUE;
    }
  }

  //FIXME �������Z�`�F�b�N
  if( isEmpty == TRUE )
  {
    return LSCL_OK;
  }
  return LSCL_OK_FULL;
}

//--------------------------------------------------------------
//	�����̕`��
//--------------------------------------------------------------
void PLIST_UTIL_DrawStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , srcStr , work->fontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}
void PLIST_UTIL_DrawStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , srcStr , work->sysFontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}

//--------------------------------------------------------------
//	�����̕`��(�l�p
//--------------------------------------------------------------
void PLIST_UTIL_DrawValueStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , dstStr , work->fontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
void PLIST_UTIL_DrawValueStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , dstStr , work->sysFontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}

#pragma mark [>message callback
static void PSTATUS_MSGCB_ReturnSelectCommon( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );
  work->mainSeq = PSMS_SELECT_POKE;
  PLIST_InitMode_Select( work );
}

void PSTATUS_MSGCB_ExitCommon( PLIST_WORK *work )
{
  work->mainSeq = PSMS_FADEOUT;
}

//�Z�������ς��ŁA�Y��邩�ǂ����H�̑I����
static void PSTATUS_MSGCB_ForgetSkill_ForgetCheck( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PSTATUS_MSGCB_ForgetSkill_ForgetCheckCB );
}

static void PSTATUS_MSGCB_ForgetSkill_ForgetCheckCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //�͂����ǂ̂킴���킷��܂����H
    work->plData->ret_mode = PL_RET_WAZASET;
    PLIST_MessageWaitInit( work , mes_pokelist_04_09 , TRUE , PSTATUS_MSGCB_ExitCommon );
  }
  else
  {
    //���������o����̂�������߂܂����H
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    PLIST_MessageWaitInit( work , mes_pokelist_04_07 , FALSE , PSTATUS_MSGCB_ForgetSkill_SkillCancel );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
}

//�o����̂�������߂܂����H
static void PSTATUS_MSGCB_ForgetSkill_SkillCancel( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PSTATUS_MSGCB_ForgetSkill_SkillCancelCB );
}

static void PSTATUS_MSGCB_ForgetSkill_SkillCancelCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //�͂����o�����ɏI�����
    work->plData->ret_mode = PL_RET_BAG;
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    PLIST_MessageWaitInit( work , mes_pokelist_04_08 , TRUE , PSTATUS_MSGCB_ExitCommon );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
  {
    //������ ���߂���
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    PLIST_MessageWaitInit( work , mes_pokelist_04_06 , FALSE , PSTATUS_MSGCB_ForgetSkill_ForgetCheck );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
}

//12�̃|�J��
static void PSTATUS_MSGCB_ForgetSkill_SkillForget( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  work->plData->ret_mode = PL_RET_BAG;
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
  PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PSTATUS_MSGCB_ExitCommon );
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
  
  PSTATUS_LearnSkillFull( work , work->selectPokePara , work->plData->waza_pos );
  if( work->plData->item != 0 )
  {
    PSTATUS_SubItem( work , work->plData->item );
  }
}

//�A�C�e���Z�b�g ���łɎ����Ă�
static void PSTATUS_MSGCB_ItemSet_CheckChangeItem( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PSTATUS_MSGCB_ItemSet_CheckChangeItemCB );
}
static void PSTATUS_MSGCB_ItemSet_CheckChangeItemCB( PLIST_WORK *work , const int retVal )
{

  if( retVal == PMIT_YES )
  {
    //�͂��A�C�e������ւ�
    const u32 haveItemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );

    PLIST_MSG_CloseWindow( work , work->msgWork );

    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , haveItemNo );
    PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 2 , work->plData->item );
    if( work->plData->mode == PL_MODE_ITEMSET_RET )
    {
      //���X�g����J�n�Ȃ̂Ŗ߂�
      //���łɃ��[�h���t�B�[���h�ɖ߂��Ă��܂�
      work->plData->mode = PL_MODE_FIELD;
      PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PSTATUS_MSGCB_ReturnSelectCommon );
    }
    else
    {
      //�A�C�e�����痈���̂ŏI��
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PSTATUS_MSGCB_ExitCommon );
    }
    PLIST_MSG_DeleteWordSet( work , work->msgWork );

    PP_Put( work->selectPokePara , ID_PARA_item , work->plData->item );
    PSTATUS_SubItem( work , work->plData->item );
    PSTATUS_AddItem( work , work->plData->item );
  }
  else
  {
    //������
    if( work->plData->mode == PL_MODE_ITEMSET_RET )
    {
      //���X�g����J�n�Ȃ̂Ŗ߂�
      //���łɃ��[�h���t�B�[���h�ɖ߂��Ă��܂�
      work->plData->mode = PL_MODE_FIELD;
      PLIST_MSG_CloseWindow( work , work->msgWork );
      work->mainSeq = PSMS_SELECT_POKE;
      PLIST_InitMode_Select( work );
    }
    else
    {
      //�A�C�e�����痈���̂ŏI��
      work->mainSeq = PSMS_FADEOUT;
    }
  }
  
}


#pragma mark [>outer value
//�O�̐��l��������
//�V�����Z���o����(�󂫂���
static void PSTATUS_LearnSkillEmpty( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  PP_SetWaza( pp , work->plData->waza );
}

//�V�����Z���o����(�󂫖���
static void PSTATUS_LearnSkillFull( PLIST_WORK *work  , POKEMON_PARAM *pp , u8 pos )
{
  PP_SetWazaPos( pp , work->plData->waza , pos );
}

//�A�C�e���������
static void PSTATUS_SubItem( PLIST_WORK *work , u16 itemNo )
{
  MYITEM_SubItem( work->plData->myitem , itemNo , 1 , work->heapId );
}
//�A�C�e���𑝂₷
static void PSTATUS_AddItem( PLIST_WORK *work , u16 itemNo )
{
  MYITEM_AddItem( work->plData->myitem , itemNo , 1 , work->heapId );
}

#pragma mark [>debug

struct _PLIST_DEBUG_WORK
{
  u8 alpha1;
  u8 alpha2;
  u8 blend1;
};

#define PLIST_DEBUG_GROUP_NUMBER (50)
static void PLIST_DEB_Update_Alpha1( void* userWork , DEBUGWIN_ITEM* item );
static void PLIST_DEB_Draw_Alpha1( void* userWork , DEBUGWIN_ITEM* item );
static void PLIST_DEB_Update_Alpha2( void* userWork , DEBUGWIN_ITEM* item );
static void PLIST_DEB_Draw_Alpha2( void* userWork , DEBUGWIN_ITEM* item );
static void PLIST_DEB_Update_Blend1( void* userWork , DEBUGWIN_ITEM* item );
static void PLIST_DEB_Draw_Blend1( void* userWork , DEBUGWIN_ITEM* item );


static void PLIST_InitDebug( PLIST_WORK *work )
{
  work->debWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_DEBUG_WORK) );
  work->debWork->alpha1 = 12;
  work->debWork->alpha2 = 16;
  work->debWork->blend1 = 0;
  DEBUGWIN_InitProc( PLIST_BG_MENU , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 31,31,31 );
  
  DEBUGWIN_AddGroupToTop( PLIST_DEBUG_GROUP_NUMBER , "PokeList" , work->heapId );
  
  DEBUGWIN_AddItemToGroupEx( PLIST_DEB_Update_Alpha1   ,PLIST_DEB_Draw_Alpha1   , (void*)work , PLIST_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PLIST_DEB_Update_Alpha2   ,PLIST_DEB_Draw_Alpha2   , (void*)work , PLIST_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PLIST_DEB_Update_Blend1   ,PLIST_DEB_Draw_Blend1   , (void*)work , PLIST_DEBUG_GROUP_NUMBER , work->heapId );
  
}

static void PLIST_TermDebug( PLIST_WORK *work )
{
  DEBUGWIN_RemoveGroup( PLIST_DEBUG_GROUP_NUMBER );
  DEBUGWIN_ExitProc();
  GFL_HEAP_FreeMemory( work->debWork );
}

//�A���t�@��1�ʌ��x
static void PLIST_DEB_Update_Alpha1( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT && 
      work->debWork->alpha1 > 0 )
  {
    work->debWork->alpha1--;
    G2_ChangeBlendAlpha( work->debWork->alpha1 , work->debWork->alpha2 );
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT && 
      work->debWork->alpha1 < 16 )
  {
    work->debWork->alpha1++;
    G2_ChangeBlendAlpha( work->debWork->alpha1 , work->debWork->alpha2 );
    DEBUGWIN_RefreshScreen();
  }
}

static void PLIST_DEB_Draw_Alpha1( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�A���t�@�P[%2d]",work->debWork->alpha1 );
}

//�A���t�@��2�ʌ��x
static void PLIST_DEB_Update_Alpha2( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT && 
      work->debWork->alpha2 > 0 )
  {
    work->debWork->alpha2--;
    G2_ChangeBlendAlpha( work->debWork->alpha1 , work->debWork->alpha2 );
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT && 
      work->debWork->alpha2 < 16 )
  {
    work->debWork->alpha2++;
    G2_ChangeBlendAlpha( work->debWork->alpha1 , work->debWork->alpha2 );
    DEBUGWIN_RefreshScreen();
  }
}

static void PLIST_DEB_Draw_Alpha2( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "�A���t�@�Q[%2d]",work->debWork->alpha2 );
}

//�u�����h1��
static void PLIST_DEB_Update_Blend1( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    if( work->debWork->blend1 == 0 )
    {
      work->debWork->blend1 = 1;
      G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , 
                        GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ ,
                        work->debWork->alpha1 , work->debWork->alpha2 );
    }
    else
    {
      work->debWork->blend1 = 0;
      G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                        GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ ,
                        work->debWork->alpha1 , work->debWork->alpha2 );
    }
    DEBUGWIN_RefreshScreen();
  }
}

static void PLIST_DEB_Draw_Blend1( void* userWork , DEBUGWIN_ITEM* item )
{
  PLIST_WORK *work = (PLIST_WORK*)userWork;
  if( work->debWork->blend1 == 0 )
  {
    DEBUGWIN_ITEM_SetNameV( item , "�u�����h[BG2+OBJ]");
  }
  else
  {
    DEBUGWIN_ITEM_SetNameV( item , "�u�����h[BG2]");
  }
}
