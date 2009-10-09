//======================================================================
/**
 * @file  startmenu.c
 * @brief �ŏ�����E����������s���g�b�v���j���[
 * @author  ariizumi
 * @data  09/01/07
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "arc_def.h"
#include "font/font.naix"

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "message.naix"
#include "startmenu.naix"
#include "msg/msg_startmenu.h"
#include "sound/pm_sndsys.h"

#include "net/dwc_raputil.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "app/name_input.h"
#include "title/startmenu.h"
#include "title/title.h"
#include "title/game_start.h"
#include "battle_championship/battle_championship.h"
#include "net_app/irc_battle.h"
#include "test/ariizumi/ari_debug.h"
//======================================================================
//  define
//======================================================================
#define BG_PLANE_MENU (GFL_BG_FRAME1_M)
#define BG_PLANE_BACK_GROUND (GFL_BG_FRAME3_M)

//BG�L�����A�h���X
#define START_MENU_FRAMECHR1  (1)
#define START_MENU_FRAMECHR2  (START_MENU_FRAMECHR1 + TALK_WIN_CGX_SIZ)

//�p���b�g
#define START_MENU_PLT_FONT   (0)
#define START_MENU_PLT_SEL    (3)
#define START_MENU_PLT_NOSEL  (4)

//�t�H���g�I�t�Z�b�g
#define START_MENU_FONT_TOP  (2)
#define START_MENU_FONT_LEFT (2)

//�J�[�\���̈ʒu
#define START_MENU_CUR_POS_X (232)
#define START_MENU_CUR_UP_POS_Y (24)
#define START_MENU_CUR_DOWN_POS_Y (192-24)

//���j���[���ڂ̍��E�T�C�Y(�L�����P��)
#define START_MENU_ITEM_LEFT (3)
#define START_MENU_ITEM_WIDTH (22)
//======================================================================
//  enum
//======================================================================
typedef enum 
{
  SMS_FADE_IN,
  SMS_SELECT,
  SMS_FADE_OUT,

  SMS_MAX,
}START_MENU_STATE;

//�X�^�[�g���j���[�̍���
enum START_MENU_ITEM
{
  SMI_CONTINUE,   //��������
  SMI_NEWGAME,    //�ŏ�����
  SMI_MYSTERY_GIFT, //�s�v�c�ȑ��蕨
  SMI_WIFI_SETTING, //Wifi�ݒ�
  SMI_GBS_CONNECT,  //GlobalBattleStation�ڑ�
  
  SMI_MAX,
  
  SMI_RETURN_TITLE = 0xFF,  //B�Ŗ߂�
};

//���A�j���̎��
enum
{
  CUR_ANM_UP_ON,
  CUR_ANM_DOWN_ON,
  CUR_ANM_UP_OFF,
  CUR_ANM_DOWN_OFF,
};
//======================================================================
//  typedef struct
//======================================================================

//���ڏ��
typedef struct
{
  BOOL enable;
  u8  top;
  GFL_BMPWIN *win;
}START_MENU_ITEM_WORK;

//���[�N
typedef struct
{
  HEAPID heapId;
  START_MENU_STATE state;
    
  u8    selectItem;  //�I�𒆍���
  u8    selectButtom;  //�I�𒆈ʒu(���[
  int   dispPos;   //�\�����ʒu(dot�P��
  int   targetPos;   //�\���ڕW�ʒu(dot�P��
  u8    length;    //���j���[�̒���(�L�����P��
  
  MYSTATUS  *mystatus;
  
  GFL_FONT *fontHandle;
  GFL_MSGDATA *msgMng; //���j���[�쐬�̂Ƃ���ł����L��
  GFL_TCB   *vblankFuncTcb;

  GFL_CLUNIT  *cellUnit;
  GFL_CLWK  *cellCursor[2];
  u32     pltIdx;
  u32     ncgIdx;
  u32     anmIdx;

  START_MENU_ITEM_WORK itemWork[SMI_MAX];
}START_MENU_WORK;

//���ڐݒ���
//���ڂ��J�����H
typedef BOOL (*START_MENU_ITEM_CheckFunc)(START_MENU_WORK *work);
//���莞����
typedef BOOL (*START_MENU_ITEM_SelectFunc)(START_MENU_WORK *work);
//���ړ��`��
typedef void (*START_MENU_ITEM_DrawFunc)(START_MENU_WORK *work,GFL_BMPWIN *win, const u8 idx );
typedef struct
{
  u8  height;  //����(�L�����P��
  //�e�퓮��
  START_MENU_ITEM_CheckFunc checkFunc;
  START_MENU_ITEM_SelectFunc  selectFunc;
  START_MENU_ITEM_DrawFunc  drawFunc;
}START_MENU_ITEM_SETTING;


//======================================================================
//  proto
//======================================================================

static void START_MENU_VBlankFunc(GFL_TCB *tcb,void *work);

static void START_MENU_InitGraphic( START_MENU_WORK *work );
static void START_MENU_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );

static void START_MENU_CreateMenuItem( START_MENU_WORK *work );

//���j���[���ڗp�@�\�֐�
static BOOL START_MENU_ITEM_TempCheck( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_TempCheckFalse( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CheckContinue( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CommonSelect( START_MENU_WORK *work );
static void START_MENU_ITEM_CommonDraw( START_MENU_WORK *work , GFL_BMPWIN *win, const u8 idx );
static void START_MENU_ITEM_ContinueDraw( START_MENU_WORK *work , GFL_BMPWIN *win, const u8 idx );

static void START_MENU_ITEM_MsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId );
static void START_MENU_ITEM_WordMsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId , WORDSET *word);

static void START_MENU_UpdateControl( START_MENU_WORK *work );
static void START_MENU_UpdatePad( START_MENU_WORK *work );
static void START_MENU_UpdateTp( START_MENU_WORK *work );

static BOOL START_MENU_MoveSelectItem( START_MENU_WORK *work , const BOOL isDown );
static void START_MENU_CheckButtonAnime( START_MENU_WORK *work );
static void START_MENU_ChangeActiveItem( START_MENU_WORK *work , const u8 newItem , const u8 oldItem );

//Proc�f�[�^
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA StartMenuProcData = {
  START_MENU_ProcInit,
  START_MENU_ProcMain,
  START_MENU_ProcEnd,
};

//���j���[���ڐݒ�f�[�^�z��
static const START_MENU_ITEM_SETTING ItemSettingData[SMI_MAX] =
{
  { 10 , START_MENU_ITEM_CheckContinue , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_ContinueDraw },
  {  4 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
  {  4 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
  {  4 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
  {  4 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_CommonSelect , START_MENU_ITEM_CommonDraw },
};

FS_EXTERN_OVERLAY(battle_championship);


//--------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  u8 i;
  
  START_MENU_WORK *work;
    //�q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_STARTMENU, 0x80000 );
  work = GFL_PROC_AllocWork( proc, sizeof(START_MENU_WORK), HEAPID_STARTMENU );
  GFL_STD_MemClear(work, sizeof(START_MENU_WORK));

  work->heapId = HEAPID_STARTMENU;
  for( i=0;i<SMI_MAX;i++ )
  {
    work->itemWork[i].enable = FALSE;
  }
  work->mystatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
  work->state = SMS_FADE_IN;
  START_MENU_InitGraphic( work );

  //�t�H���g�p�p���b�g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , START_MENU_PLT_FONT * 32, 16*2, work->heapId );
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );

  //WinFrame�p�O���t�B�b�N�ݒ�
  BmpWinFrame_GraphicSet( BG_PLANE_MENU , START_MENU_FRAMECHR1 , START_MENU_PLT_SEL   , 0, work->heapId);
  BmpWinFrame_GraphicSet( BG_PLANE_MENU , START_MENU_FRAMECHR2 , START_MENU_PLT_NOSEL , 1, work->heapId);
  //�w�i�F
  *((u16 *)HW_BG_PLTT) = 0x7d8c;//RGB(12, 12, 31);
  
  START_MENU_CreateMenuItem( work );
  
  work->vblankFuncTcb = GFUser_VIntr_CreateTCB( START_MENU_VBlankFunc , (void*)work , 0 );

  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 16 , 0 , ARI_FADE_SPD );
  
  return GFL_PROC_RES_FINISH;
}

FS_EXTERN_OVERLAY(mystery);
extern const GFL_PROC_DATA MysteryGiftProcData;
//--------------------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK *work = mywk;

  //�I���ɂ�镪�򏈗�
  {
    switch( work->selectItem )
    {
    case SMI_CONTINUE:    //��������
      //Proc�̕ύX�𒆂ł���Ă�
      GameStart_Continue();
      break;
    case SMI_NEWGAME:   //�ŏ�����
      //Proc�̕ύX�𒆂ł���Ă�
      GameStart_Beginning();
      //���O���͂�
      //GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &NameInputProcData,(void*)NAMEIN_MYNAME);
      break;
      
    case SMI_MYSTERY_GIFT:  //�s�v�c�ȑ��蕨
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mystery), &MysteryGiftProcData, NULL);
      break;

    case SMI_WIFI_SETTING: //WIFI�ݒ�
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifi_util), &WifiUtilProcData, NULL);
        break;
            
    case SMI_GBS_CONNECT: //�o�g�����j���[
      // @todo irc_battle�ł͂Ȃ����j���[�ւȂ�
      //GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(battle_championship), &BATTLE_CHAMPIONSHIP_ProcData, NULL);
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(battle_championship), &IRC_BATTLE_ProcData, NULL);
      break;
                
    case SMI_RETURN_TITLE:
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
      break;
    default:
      //Proc�̕ύX�𒆂ł���Ă�
      GameStart_Beginning();
      break;
    }
  }

  //�J������
  {
    u8 i;
    GFL_TCB_DeleteTask( work->vblankFuncTcb );

    GFL_CLACT_WK_Remove( work->cellCursor[0] );
    GFL_CLACT_WK_Remove( work->cellCursor[1] );
    
    GFL_CLGRP_PLTT_Release( work->pltIdx );
    GFL_CLGRP_CGR_Release( work->ncgIdx );
    GFL_CLGRP_CELLANIM_Release( work->anmIdx );
    
    GFL_CLACT_UNIT_Delete( work->cellUnit );
    GFL_CLACT_SYS_Delete();

    for( i=0;i<SMI_MAX;i++ )
    {
      if( work->itemWork[i].enable == TRUE )
      {
        GFL_BMPWIN_Delete( work->itemWork[i].win );
      }
    }

    GFL_FONT_Delete( work->fontHandle );
    GFL_BMPWIN_Exit();

    GFL_BG_FreeBGControl(BG_PLANE_MENU);
    GFL_BG_FreeBGControl(BG_PLANE_BACK_GROUND);
    GFL_BG_Exit();
    
    GFL_PROC_FreeWork( proc );

    GFL_HEAP_DeleteHeap( HEAPID_STARTMENU );
  }

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK *work = mywk;
  
  switch( work->state )
  {
  case SMS_FADE_IN:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      work->state = SMS_SELECT;
    }
    break;
    
  case SMS_SELECT:
    START_MENU_UpdateControl( work );
    break;
    
  case SMS_FADE_OUT:
    if( GFL_FADE_CheckFade() == FALSE )
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  
  //�X�N���[������(������ɃI�[�o�[�����ȂǓ���ĂȂ��̂�8�̖񐔂�
  if( work->targetPos > work->dispPos )
  {
    work->dispPos += 4;
    GFL_BG_SetScroll( BG_PLANE_MENU , GFL_BG_SCROLL_Y_SET , work->dispPos );
  }
  else
  if( work->targetPos < work->dispPos )
  {
    work->dispPos -= 4;
    GFL_BG_SetScroll( BG_PLANE_MENU , GFL_BG_SCROLL_Y_SET , work->dispPos );
  }

  //OBJ�̍X�V
  GFL_CLACT_SYS_Main();

  return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief VBLank Function
 * @param NONE
 * @return  NONE
 */
//------------------------------------------------------------------
static void START_MENU_VBlankFunc(GFL_TCB *tcb,void *work)
{
  GFL_CLACT_SYS_VBlankFunc();
}

//--------------------------------------------------------------------------
//  �O���t�B�b�N���菉����
//--------------------------------------------------------------------------
static void START_MENU_InitGraphic( START_MENU_WORK *work )
{
  static const GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_128_D,      // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_32K,   // ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
  };

  static const GFL_BG_SYS_HEADER sysHeader = {
    GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
  };
  
  static const GFL_BG_BGCNT_HEADER bgCont_Menu = {
  0, 0, 0x1000, 0,
  GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };

  static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };
  
  ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_STARTMENU , work->heapId );

  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

  GFL_DISP_SetBank( &vramBank );
  GFL_BG_Init( work->heapId );
  GFL_BG_SetBGMode( &sysHeader ); 

  START_MENU_InitBgFunc( &bgCont_Menu , BG_PLANE_MENU );
  START_MENU_InitBgFunc( &bgCont_BackGround , BG_PLANE_BACK_GROUND );
  
  GFL_BMPWIN_Init( work->heapId );
  
  //OBJ�n
  {
    GFL_CLWK_DATA cellInitData;

    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.oamst_main = 0x10;  //�f�o�b�O���[�^�̕�
    cellSysInitData.oamnum_main = 128-0x10;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    work->cellUnit  = GFL_CLACT_UNIT_Create( 2 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );

    //�e��f�ނ̓ǂݍ���
    work->pltIdx = GFL_CLGRP_PLTT_Register( p_handle , NARC_startmenu_title_cursor_NCLR , CLSYS_DRAW_MAIN , 0 , work->heapId  );
    work->ncgIdx = GFL_CLGRP_CGR_Register( p_handle , NARC_startmenu_title_cursor_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->anmIdx = GFL_CLGRP_CELLANIM_Register( p_handle , NARC_startmenu_title_cursor_NCER , NARC_startmenu_title_cursor_NANR, work->heapId  );

    //�Z���̐���

    cellInitData.pos_x = 232;
    cellInitData.pos_y =  24;
    cellInitData.anmseq = CUR_ANM_UP_ON;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    //�����
    work->cellCursor[0] = GFL_CLACT_WK_Create( work->cellUnit ,work->ncgIdx,work->pltIdx,work->anmIdx,
                  &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor[0], FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor[0], TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor[0], TRUE );
    
    //�����
    cellInitData.pos_y =  192-24;
    cellInitData.anmseq = CUR_ANM_UP_OFF;
    work->cellCursor[1] = GFL_CLACT_WK_Create( work->cellUnit ,work->ncgIdx,work->pltIdx,work->anmIdx,
                  &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor[1], FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor[1], TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor[1], TRUE );
        
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }

  GFL_ARC_CloseDataHandle( p_handle );
}

//--------------------------------------------------------------------------
//  Bg������ �@�\��
//--------------------------------------------------------------------------
static void START_MENU_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  ���j���[�̍쐬
//--------------------------------------------------------------------------
static void START_MENU_CreateMenuItem( START_MENU_WORK *work )
{
  u8  i;
  u8  bgTopPos = 1; //�A�C�e���̕\���ʒu(�L�����P��

  work->msgMng = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_startmenu_dat, work->heapId);
  work->selectItem = 0xFF; //�ŏ��ɃA�N�e�B�u�Ȃ͈̂�ԏ�ɂ��邽�߂̔���p
  for( i=0;i<SMI_MAX;i++ )
  {
    if( ItemSettingData[i].checkFunc(work) == TRUE )
    {
      work->itemWork[i].enable = TRUE;
      work->itemWork[i].win = GFL_BMPWIN_Create( 
            BG_PLANE_MENU , 
            START_MENU_ITEM_LEFT , bgTopPos ,
            START_MENU_ITEM_WIDTH , ItemSettingData[i].height ,
            START_MENU_PLT_FONT , 
            GFL_BMP_CHRAREA_GET_B );
      GFL_BMPWIN_MakeScreen( work->itemWork[i].win );
      GFL_BMPWIN_TransVramCharacter( work->itemWork[i].win );
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->itemWork[i].win), 0xf );

      ItemSettingData[i].drawFunc(work,work->itemWork[i].win,i);
      
      GFL_BMPWIN_TransVramCharacter( work->itemWork[i].win );
      BmpWinFrame_Write(work->itemWork[i].win, WINDOW_TRANS_ON, 
              START_MENU_FRAMECHR1, START_MENU_PLT_NOSEL);
      
      bgTopPos += ItemSettingData[i].height + 2;
      if( work->selectItem == 0xFF )
      {
        //��ԏ�̍���
        work->selectItem = i;
      }
    }
  }
  GFL_MSG_Delete( work->msgMng );
  work->msgMng = NULL;
  
  GF_ASSERT( work->selectItem != 0xFF );
  work->selectButtom = ItemSettingData[work->selectItem].height + 2;
  work->length = bgTopPos-1; //���̃��j���[�̘g�̕��������Ă���̂�1����
  START_MENU_ChangeActiveItem( work , work->selectItem , 0xFF );
  
  //�ŏ��̉��J�[�\���̃A�j���`�F�b�N
  if( work->length > 24 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[0], CUR_ANM_UP_OFF );
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[1], CUR_ANM_DOWN_ON );
  }
  else
  {
    //�X�N���[���ł��Ȃ��Ȃ����
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor[0], FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor[1], FALSE );
  }

}

//--------------------------------------------------------------------------
//  �I�����j���[�̐؂�ւ�
//--------------------------------------------------------------------------
static void START_MENU_ChangeActiveItem( START_MENU_WORK *work , const u8 newItem , const u8 oldItem )
{
  if( newItem != 0xFF )
  {
    BmpWinFrame_Write(work->itemWork[newItem].win, WINDOW_TRANS_ON, 
            START_MENU_FRAMECHR2, START_MENU_PLT_SEL);
  }
  if( oldItem != 0xFF )
  {
    BmpWinFrame_Write(work->itemWork[oldItem].win, WINDOW_TRANS_ON, 
            START_MENU_FRAMECHR1, START_MENU_PLT_NOSEL);
  }
}

#pragma mark MenuSelectFunc

//--------------------------------------------------------------------------
//  ���j���[��L���ɂ��邩�H
//  @return TRUE=�L��
//--------------------------------------------------------------------------
static BOOL START_MENU_ITEM_TempCheck( START_MENU_WORK *work )
{
  return TRUE;
}
static BOOL START_MENU_ITEM_TempCheckFalse( START_MENU_WORK *work )
{
  return FALSE;
}
static BOOL START_MENU_ITEM_CheckContinue( START_MENU_WORK *work )
{
  return SaveData_GetExistFlag( SaveControl_GetPointer() );
}

//--------------------------------------------------------------------------
//  ���莞�̓���
//  @return TRUE=�X�^�[�g���j���̏I��������
//--------------------------------------------------------------------------
static BOOL START_MENU_ITEM_CommonSelect( START_MENU_WORK *work )
{
  //���̂Ƃ���I�����ꂽ
  return TRUE;
}
//���j���[���`�拤��(idx��msgId��ǂݕ�����
static void START_MENU_ITEM_CommonDraw( START_MENU_WORK *work , GFL_BMPWIN *win , const u8 idx )
{
  static const msgIdArr[SMI_MAX] =
  {
    START_MENU_STR_ITEM_01_01,  //���ۂ͂��Ȃ�(�_�~�[
    START_MENU_STR_ITEM_02,
    START_MENU_STR_ITEM_03,
    START_MENU_STR_ITEM_04,
    START_MENU_STR_ITEM_05,
  };
  
  START_MENU_ITEM_MsgDrawFunc( work , win , 0,0, msgIdArr[idx] );
}

//�������烁�j���[���`��
static void START_MENU_ITEM_ContinueDraw( START_MENU_WORK *work , GFL_BMPWIN *win , const u8 idx )
{
  u8 lCol,sCol,bCol;
  WORDSET *word;
  STRBUF *nameStr;
  const STRCODE *myname = MyStatus_GetMyName( work->mystatus );

  word = WORDSET_Create( work->heapId );
  //Font�F��߂����ߎ���Ă���
  GFL_FONTSYS_GetColor( &lCol,&sCol,&bCol );

  START_MENU_ITEM_MsgDrawFunc( work , win ,  0, 0, START_MENU_STR_ITEM_01_01 );

  GFL_FONTSYS_SetColor( 5,sCol,bCol );
  START_MENU_ITEM_MsgDrawFunc( work , win , 32,16, START_MENU_STR_ITEM_01_02 );
  START_MENU_ITEM_MsgDrawFunc( work , win , 32,32, START_MENU_STR_ITEM_01_03 );
  START_MENU_ITEM_MsgDrawFunc( work , win , 32,48, START_MENU_STR_ITEM_01_04 );
  START_MENU_ITEM_MsgDrawFunc( work , win , 32,64, START_MENU_STR_ITEM_01_05 );

  //���O  
  nameStr =   GFL_STR_CreateBuffer( 16, work->heapId );
  if( MyStatus_CheckNameClear( work->mystatus ) == FALSE )
  {
    GFL_STR_SetStringCode( nameStr , myname );
  }
  else
  {
    //TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
    u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
    GFL_STR_SetStringCode( nameStr , tempName );
  }
  WORDSET_RegisterWord( word, 0, nameStr, 0, TRUE , 0 );
  START_MENU_ITEM_WordMsgDrawFunc( work , win ,160,16, START_MENU_STR_ITEM_01_06 , word);

  //�v���C����
  WORDSET_RegisterNumber( word, 0, 9, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  WORDSET_RegisterNumber( word, 1, 2, 2, STR_NUM_DISP_ZERO , STR_NUM_CODE_HANKAKU );
  START_MENU_ITEM_WordMsgDrawFunc( work , win ,160,32, START_MENU_STR_ITEM_01_07 , word);
  //�|�P�����}��
  WORDSET_RegisterNumber( word, 0, 55, 3, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  START_MENU_ITEM_WordMsgDrawFunc( work , win ,160,48, START_MENU_STR_ITEM_01_08 , word);
  //�o�b�W
  WORDSET_RegisterNumber( word, 0, MyStatus_GetBadgeCount(work->mystatus), 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
  START_MENU_ITEM_WordMsgDrawFunc( work , win ,160,64, START_MENU_STR_ITEM_01_09 , word);

  //Font�F��߂�
  GFL_FONTSYS_SetColor( lCol,sCol,bCol );
  
  GFL_STR_DeleteBuffer( nameStr );
  WORDSET_Delete( word );
}

static void START_MENU_ITEM_MsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId )
{
  STRBUF  *str = GFL_STR_CreateBuffer( 96 , work->heapId );
  GFL_MSG_GetString( work->msgMng , msgId , str );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(win),
          posX+START_MENU_FONT_LEFT,
          posY+START_MENU_FONT_TOP,
          str,
          work->fontHandle);
  
  GFL_STR_DeleteBuffer( str );
}

static void START_MENU_ITEM_WordMsgDrawFunc( START_MENU_WORK *work , GFL_BMPWIN *win , u8 posX , u8 posY , u16 msgId , WORDSET *word)
{
  u8 strLen;
  STRBUF  *str = GFL_STR_CreateBuffer( 96 , work->heapId );
  STRBUF  *baseStr = GFL_STR_CreateBuffer( 96 , work->heapId );
  GFL_MSG_GetString( work->msgMng , msgId , baseStr );
  WORDSET_ExpandStr( word , str , baseStr );

  strLen = PRINTSYS_GetStrWidth( str , work->fontHandle , 0 );
  
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(win),
          posX+START_MENU_FONT_LEFT - strLen,
          posY+START_MENU_FONT_TOP,
          str,
          work->fontHandle);
  
  GFL_STR_DeleteBuffer( baseStr );
  GFL_STR_DeleteBuffer( str );
  
}

#pragma mark ControlFunc

//--------------------------------------------------------------------------
//  ����n�X�V
//--------------------------------------------------------------------------
static void START_MENU_UpdateControl( START_MENU_WORK *work )
{
  START_MENU_UpdatePad(work);
  START_MENU_UpdateTp(work);
}

//--------------------------------------------------------------------------
//  �L�[����
//--------------------------------------------------------------------------
static void START_MENU_UpdatePad( START_MENU_WORK *work )
{
  const int keyTrg = GFL_UI_KEY_GetTrg();
  if( keyTrg & PAD_KEY_UP )
  {
    START_MENU_MoveSelectItem( work , FALSE );
  }
  else
  if( keyTrg & PAD_KEY_DOWN )
  {
    START_MENU_MoveSelectItem( work , TRUE );
  }
  else
  if( keyTrg & PAD_BUTTON_A )
  {
    const BOOL ret = ItemSettingData[work->selectItem].selectFunc( work );
    if( ret == TRUE )
    {
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
      work->state = SMS_FADE_OUT;
      PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
    }
  }
  else
  if( keyTrg & PAD_BUTTON_B )
  {
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
    work->selectItem = SMI_RETURN_TITLE;
    work->state = SMS_FADE_OUT;
    PMSND_PlaySystemSE( SEQ_SE_CANCEL1 );
  }
}

//--------------------------------------------------------------------------
//  TP����
//--------------------------------------------------------------------------
static void START_MENU_UpdateTp( START_MENU_WORK *work )
{
  //�㉺�{�^��
  if( GFL_UI_TP_GetTrg() == TRUE )
  {
    GFL_UI_TP_HITTBL hitTbl[3] =
    {
      { 
        START_MENU_CUR_UP_POS_Y - 16 , START_MENU_CUR_UP_POS_Y+16,
        START_MENU_CUR_POS_X - 16 , START_MENU_CUR_POS_X + 16 
      },
      { 
        START_MENU_CUR_DOWN_POS_Y-16 , START_MENU_CUR_DOWN_POS_Y+16,
        START_MENU_CUR_POS_X - 16 , START_MENU_CUR_POS_X + 16 
      },
      { 
        GFL_UI_TP_HIT_END , 0,0,0 
      },
    };
    
    const int ret = GFL_UI_TP_HitTrg( hitTbl );
    if( ret == 0 && 
        GFL_CLACT_WK_GetAnmSeq( work->cellCursor[0] ) == CUR_ANM_UP_ON )
    {
      work->targetPos -= 192;
      if( work->targetPos < 0 )
      {
        work->targetPos = 0;
      }
      START_MENU_CheckButtonAnime( work );
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
    else
    if( ret == 1 && 
        GFL_CLACT_WK_GetAnmSeq( work->cellCursor[1] ) == CUR_ANM_DOWN_ON )
    {
      work->targetPos += 192;
      if( work->targetPos > work->length*8 -192 )
      {
        work->targetPos = work->length*8 -192;
      }
      START_MENU_CheckButtonAnime( work );
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }

    if( ret == GFL_UI_TP_HIT_NONE )
    {
      //�{�^���Ƃ̔���
      u32 tpx,tpy;
      GFL_UI_TP_GetPointTrg( &tpx,&tpy );
      if( tpx >= START_MENU_ITEM_LEFT*8 &&
          tpx < (START_MENU_ITEM_LEFT+START_MENU_ITEM_WIDTH)*8 )
        { 
        u8 i =0;
        u8 touchLine = tpy/8;
        int nowLine = -(work->targetPos/8);
        for( i = 0; i < SMI_MAX ; i++ )
        {
          if( work->itemWork[i].enable == TRUE )
          {
            //�g�̏�̕���+1
            if( nowLine+1 <= touchLine &&
                nowLine+ItemSettingData[i].height+1 > touchLine )
            {
              const BOOL ret = ItemSettingData[i].selectFunc( work );
              if( ret == TRUE )
              {
                GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
                work->state = SMS_FADE_OUT;
                work->selectItem = i;
                PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
              }
              break;
            }
            nowLine += ItemSettingData[i].height + 2;//�g��+2
          }
        }
      }
    }
  }
}


//--------------------------------------------------------------------------
//  ���ڈړ�
//  @return �ړ��ł������H
//  @value  BOOL isDown�@FALSE:���ړ� TRUE:���ړ�
//--------------------------------------------------------------------------
static BOOL START_MENU_MoveSelectItem( START_MENU_WORK *work , const BOOL isDown )
{
  const int moveValue = (isDown==TRUE ? 1 : -1 );
  const int endValue = (isDown==TRUE ? SMI_MAX : -1 );
  int i;
  BOOL isShow;
  //�J�n�ʒu�͂P���������Ă���
  for( i = work->selectItem + moveValue; i != endValue ; i += moveValue )
  {
    if( work->itemWork[i].enable == TRUE )
    {
      break;
    }
  }
  if( i == endValue )
  {
    //�����Ȃ�����
    return FALSE;
  }
  
  START_MENU_ChangeActiveItem( work , i , work->selectItem );
  if( isDown == TRUE )
  {
    work->selectButtom += ItemSettingData[i].height + 2;  //�g��+2
  }
  else
  {
    work->selectButtom -= ItemSettingData[work->selectItem].height + 2;  //�g��+2
  }
  
  work->selectItem = i;
  
  //�\���ʒu�v�Z
  if( work->selectButtom*8 > work->targetPos + 192 )
  {
    //���̃`�F�b�N
    work->targetPos = work->selectButtom*8 -192;
  }
  else  //���j���[�̓��̈ʒu�Ȃ̂Ōv�Z�������E�E�E
  if( ( work->selectButtom - ItemSettingData[work->selectItem].height - 2 )*8 < work->targetPos )
  {
    //��̃`�F�b�N
    work->targetPos = ( work->selectButtom - ItemSettingData[work->selectItem].height - 2 )*8;
  }
  PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
  START_MENU_CheckButtonAnime( work );
  return TRUE;
}

static void START_MENU_CheckButtonAnime( START_MENU_WORK *work )
{

  //�J�[�\���\���̃`�F�b�N
  //��
  if( work->targetPos > 0 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[0], CUR_ANM_UP_ON );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[0], CUR_ANM_UP_OFF );
  }

  //��
  if( work->targetPos + 192 < work->length*8 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[1], CUR_ANM_DOWN_ON );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor[1], CUR_ANM_DOWN_OFF );
  }
  
}

