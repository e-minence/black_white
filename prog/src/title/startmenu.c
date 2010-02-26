#if 0
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
#include "net_app/mystery.h"
#include "msg/msg_wifi_system.h"

//======================================================================
//  define
//======================================================================
#define BG_PLANE_MENU (GFL_BG_FRAME1_M)
#define BG_PLANE_INFO (GFL_BG_FRAME2_M)
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

// �J�[�\���p���b�g�A�j���̏����J���[
#define START_MENU_ANIME_E_R ( 5)
#define START_MENU_ANIME_E_G (15)
#define START_MENU_ANIME_E_B (21)

// info���b�Z�[�W
#define _MESSAGE_BUF_NUM (200)
#define _BUTTON_MSG_PAL   (12)  // �E�C���h�E
#define _BUTTON_WIN_PAL   (13)  // �E�C���h�E

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
  SMI_PDW_ACCOUNT,  //PDW�A�J�E���g�ݒ�
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
  MISC*  misc;
  
  GFL_FONT *fontHandle;
  GFL_MSGDATA *msgMng; //���j���[�쐬�̂Ƃ���ł����L��
  GFL_MSGDATA *pMsgWiFiData;  //
  GFL_TCB   *vblankFuncTcb;

  GFL_BMPWIN* infoDispWin;
  STRBUF* pStrBuf;
  u32 bgchar;
  
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK  *cellCursor[2];
  u32     pltIdx;
  u32     ncgIdx;
  u32     anmIdx;

  START_MENU_ITEM_WORK itemWork[SMI_MAX];

  u16 anmCnt;
  u16 transBuf;
  
  u16 anime_r;
  u16 anime_g;
  u16 anime_b;
  
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

static void START_MENU_UpdatePalletAnime( START_MENU_WORK *work );

//���j���[���ڗp�@�\�֐�
static BOOL START_MENU_ITEM_TempCheck( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_TempCheckFalse( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CheckContinue( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_CommonSelect( START_MENU_WORK *work );
static BOOL START_MENU_ITEM_WifiUtil( START_MENU_WORK *work );
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
  {  4 , START_MENU_ITEM_TempCheck , START_MENU_ITEM_WifiUtil , START_MENU_ITEM_CommonDraw },
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
  work->misc = SaveData_GetMisc( SaveControl_GetPointer() );
  work->state = SMS_FADE_IN;
  work->anmCnt = 0;

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

  work->anime_r = START_MENU_ANIME_E_R;
  work->anime_g = START_MENU_ANIME_E_G;
  work->anime_b = START_MENU_ANIME_E_B;

  work->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, HEAPID_STARTMENU );

  
  work->pMsgWiFiData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, work->heapId );


  
  return GFL_PROC_RES_FINISH;
}

FS_EXTERN_OVERLAY(pdw_acc);
   
extern const GFL_PROC_DATA PDW_ACC_MainProcData;

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
      break;
      
    case SMI_MYSTERY_GIFT:  //�s�v�c�ȑ��蕨
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mystery), &MysteryGiftProcData, NULL);
      break;

    case SMI_WIFI_SETTING: //WIFI�ݒ�
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifi_util), &WifiUtilProcData, NULL);
      break;

    case SMI_PDW_ACCOUNT: //�|�P�����h���[�����[���h�ݒ�
      GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &PDW_ACC_MainProcData, NULL);
        break;
      
    case SMI_GBS_CONNECT: //�o�g�����j���[
      GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(battle_championship), &BATTLE_CHAMPIONSHIP_ProcData, NULL);
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
    // infomsg
    GFL_STR_DeleteBuffer(work->pStrBuf);
    GFL_MSG_Delete(work->pMsgWiFiData);
    if(work->infoDispWin){
      GFL_BMPWIN_Delete(work->infoDispWin);
      work->infoDispWin=NULL;
    }

    GFL_FONT_Delete( work->fontHandle );
    GFL_BMPWIN_Exit();

    GFL_BG_FreeBGControl(BG_PLANE_MENU);
    GFL_BG_FreeBGControl(BG_PLANE_INFO);
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

  START_MENU_UpdatePalletAnime( work );

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
  GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
  };

  static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
  };

  static const GFL_BG_BGCNT_HEADER bgCont_Info = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };
  
  ARCHANDLE *p_handle = GFL_ARC_OpenDataHandle( ARCID_STARTMENU , work->heapId );

  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  GFL_DISP_GX_SetVisibleControlDirect(0);   //�SBG&OBJ�̕\��OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

  GFL_DISP_SetBank( &vramBank );
  GFL_BG_Init( work->heapId );
  GFL_BG_SetBGMode( &sysHeader ); 

  START_MENU_InitBgFunc( &bgCont_Menu , BG_PLANE_MENU );
  START_MENU_InitBgFunc( &bgCont_BackGround , BG_PLANE_BACK_GROUND );
  START_MENU_InitBgFunc( &bgCont_Info , BG_PLANE_INFO );
  
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

    //infomsg
    GFL_BG_FillCharacter( BG_PLANE_INFO, 0x00, 1, 0 );
    GFL_BG_FillScreen( BG_PLANE_INFO,	0x0000, 0, 0, 32, 24, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( BG_PLANE_INFO );

    work->bgchar = BmpWinFrame_GraphicSetAreaMan(BG_PLANE_INFO,
                                                  _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, work->heapId);

    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                  0x20*_BUTTON_MSG_PAL, 0x20, work->heapId);

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

// �J�[�\������Ԕ����Ȃ鎞
#define CURSOR_MAX_WHITE_POINT  (0xc000)

//--------------------------------------------------------------
//  �p���b�g�A�j���[�V�����̍X�V
//--------------------------------------------------------------
//�v���[�g�̃A�j���Bsin�g���̂�0�`0xFFFF�̃��[�v
#define START_MENU_ANIME_VALUE (0x400)
#define START_MENU_ANIME_S_R (25)
#define START_MENU_ANIME_S_G (30)
#define START_MENU_ANIME_S_B (29)
//�v���[�g�̃A�j������F
#define START_MENU_ANIME_COL (0x6)

static void START_MENU_UpdatePalletAnime( START_MENU_WORK *work )
{
  //�v���[�g�A�j��
  if( work->anmCnt + START_MENU_ANIME_VALUE >= 0x10000 )
  {
    work->anmCnt = work->anmCnt+START_MENU_ANIME_VALUE-0x10000;
  }
  else
  {
    work->anmCnt += START_MENU_ANIME_VALUE;
  }

  // �J�[�\����Ԕ����Ȃ鎞�ɃJ���[��ύX����
  if(work->anmCnt==CURSOR_MAX_WHITE_POINT){
    work->anime_r = GFL_STD_MtRand(14)+10;
    work->anime_g = GFL_STD_MtRand(24);
    work->anime_b = 24-work->anime_g;
  }

  {
    //1�`0�ɕϊ�
    const fx16 sin = (FX_SinIdx(work->anmCnt)+FX16_ONE)/2;
    const u8 r = START_MENU_ANIME_S_R + (((work->anime_r-START_MENU_ANIME_S_R)*sin)>>FX16_SHIFT);
    const u8 g = START_MENU_ANIME_S_G + (((work->anime_g-START_MENU_ANIME_S_G)*sin)>>FX16_SHIFT);
    const u8 b = START_MENU_ANIME_S_B + (((work->anime_b-START_MENU_ANIME_S_B)*sin)>>FX16_SHIFT);
    
    work->transBuf = GX_RGB(r, g, b);
    
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        START_MENU_PLT_SEL * 32 + START_MENU_ANIME_COL*2 ,
                                        &work->transBuf , 2 );
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

//------------------------------------------------------------------------------
/**
 * @brief   �����E�C���h�E�\��
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _infoMessageDisp(START_MENU_WORK* pWork)
{
  GFL_BMPWIN* pwin;

  
  if(pWork->infoDispWin==NULL){


    pWork->infoDispWin = GFL_BMPWIN_Create(
      BG_PLANE_INFO,
      1 , 3, 30 ,4 ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = pWork->infoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin), 15);
  GFL_FONTSYS_SetColor(1, 2, 15);

  PRINTSYS_Print(GFL_BMPWIN_GetBmp(pwin) ,0,0, pWork->pStrBuf, pWork->fontHandle);

  BmpWinFrame_Write( pwin, WINDOW_TRANS_ON_V,
                     GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

  GFL_BMPWIN_TransVramCharacter(pwin);
  GFL_BMPWIN_MakeScreen(pwin);
  GFL_BG_LoadScreenV_Req(BG_PLANE_INFO);
}
//--------------------------------------------------------------------------
//  ���莞�̓���  WIFI�ݒ�
//  @return TRUE=�X�^�[�g���j���̏I��������
//--------------------------------------------------------------------------
static BOOL START_MENU_ITEM_WifiUtil( START_MENU_WORK *work )
{
  if( !OS_IsRunOnTwl() ){//DSI�͌ĂԂ��Ƃ��o���Ȃ�
    return TRUE;
  }
  else{
    GFL_MSG_GetString( work->pMsgWiFiData, dwc_message_0017, work->pStrBuf );
    _infoMessageDisp(work);
    GFL_BG_SetVisible( BG_PLANE_INFO, VISIBLE_ON );
    return FALSE;
  }
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
    START_MENU_STR_ITEM_06,
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
  WORDSET_RegisterNumber( word, 0, MISC_GetBadgeCount(work->misc), 1, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
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
//  START_MENU_UpdateTp(work);
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
    if(work->infoDispWin!=NULL){
      GFL_BG_SetVisible( BG_PLANE_INFO, VISIBLE_OFF );
      GFL_BMPWIN_Delete(work->infoDispWin);
      work->infoDispWin=NULL;
    }
    else{
      const BOOL ret = ItemSettingData[work->selectItem].selectFunc( work );
      if( ret == TRUE )
      {
        GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN , 0 , 16 , ARI_FADE_SPD );
        work->state = SMS_FADE_OUT;
        PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
      }
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



















#else

//============================================================================================
/**
 * @file		startmenu.c
 * @brief		�ŏ�����E����������s���g�b�v���j���[
 * @author	ariizumi
 * @author	Hiroyuki Nakamura
 * @data		09/01/07
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "message.naix"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"
#include "system/blink_palanm.h"
#include "system/bgwinfrm.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "system/ds_system.h"
#include "savedata/mystatus.h"
#include "savedata/playtime.h"
#include "savedata/zukan_savedata.h"
#include "savedata/situation.h"
#include "savedata/misc.h"
#include "sound/pm_sndsys.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "msg/msg_startmenu.h"
#include "field/zonedata.h"
#include "net/dwc_raputil.h"
#include "net_app/mystery.h"
#include "battle_championship/battle_championship.h"
#include "app/mictest.h"
#include "gamesystem/msgspeed.h"

#include "title/title.h"
#include "title/game_start.h"
#include "title/startmenu.h"
#include "title/startmenu_def.h"

#include "font/font.naix"
#include "startmenu.naix"
#include "wifileadingchar.naix"


//============================================================================================
//	�萔��`
//============================================================================================

//���[�N
typedef struct {

/*
  HEAPID heapId;
  START_MENU_STATE state;
    
  u8    selectItem;  //�I�𒆍���
  u8    selectButtom;  //�I�𒆈ʒu(���[
  int   dispPos;   //�\�����ʒu(dot�P��
  int   targetPos;   //�\���ڕW�ʒu(dot�P��
  u8    length;    //���j���[�̒���(�L�����P��
*/  

	SAVE_CONTROL_WORK * savedata;
  MYSTATUS * mystatus;

	GFL_TCB * vtask;					// TCB ( VBLANK )

/*
  MISC*  misc;
  
  GFL_FONT *fontHandle;
  GFL_MSGDATA *msgMng; //���j���[�쐬�̂Ƃ���ł����L��
  GFL_MSGDATA *pMsgWiFiData;  //
  GFL_TCB   *vblankFuncTcb;

  GFL_BMPWIN* infoDispWin;
  STRBUF* pStrBuf;
  u32 bgchar;
  
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK  *cellCursor[2];
  u32     pltIdx;
  u32     ncgIdx;
  u32     anmIdx;

  START_MENU_ITEM_WORK itemWork[SMI_MAX];

  u16 anmCnt;
  u16 transBuf;
  
  u16 anime_r;
  u16 anime_g;
  u16 anime_b;
*/
	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[OBJ_ID_MAX];
	u32	chrRes[CHRRES_MAX];
	u32	palRes[PALRES_MAX];
	u32	celRes[CELRES_MAX];

	GFL_FONT * font;					// �ʏ�t�H���g
	GFL_MSGDATA * mman;				// ���b�Z�[�W�f�[�^�}�l�[�W��
	WORDSET * wset;						// �P��Z�b�g
	STRBUF * exp;							// ���b�Z�[�W�W�J�̈�
	PRINT_QUE * que;					// �v�����g�L���[
	PRINT_STREAM * stream;		// �v�����g�X�g���[��
	GFL_TCBLSYS * tcbl;

	BMPMENU_WORK * mwk;

	PRINT_UTIL	util[BMPWIN_MAX];
	PRINT_UTIL	utilWin;

	u16 * listFrame[LIST_ITEM_MAX];

	BGWINFRM_WORK * wfrm;

	BLINKPALANM_WORK * blink;		// �J�[�\���A�j�����[�N

	u8	list[LIST_ITEM_MAX];
	u8	listPos;
	u8	listResult;

	u8	continueSeq;
	u8	continueRet;

	s8	cursorPutPos;
	int	bgScroll;
	int	bgScrollCount;
	int	bgScrollSpeed;

	int	mainSeq;
	int	nextSeq;
	int	fadeSeq;
  
}START_MENU_WORK;

// ���X�g���ڃf�[�^
typedef struct {
	u16	scrnIdx;		// �X�N���[���C���f�b�N�X
	u16	sy;					// �x�T�C�Y
}LIST_ITEM_DATA;

enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_FADE,
	MAINSEQ_BUTTON_ANM,

	MAINSEQ_INIT_WAIT,
	MAINSEQ_MAIN,
	MAINSEQ_SCROLL,

	MAINSEQ_CONTINUE,
	MAINSEQ_NEWGAME,

	MAINSEQ_END_SET,

	MAINSEQ_END,
};

typedef int (*pSTARTMENU_FUNC)(START_MENU_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static int MainSeq_Init( START_MENU_WORK * wk );
static int MainSeq_Release( START_MENU_WORK * wk );
static int MainSeq_Fade( START_MENU_WORK * wk );
static int MainSeq_ButtonAnm( START_MENU_WORK * wk );
static int MainSeq_InitWait( START_MENU_WORK * wk );
static int MainSeq_Main( START_MENU_WORK * wk );
static int MainSeq_Scroll( START_MENU_WORK * wk );
static int MainSeq_Continue( START_MENU_WORK * wk );
static int MainSeq_NewGame( START_MENU_WORK * wk );
static int MainSeq_EndSet( START_MENU_WORK * wk );

static void InitVBlank( START_MENU_WORK * wk );
static void ExitVBlank( START_MENU_WORK * wk );
static void InitVram(void);
static void InitBg(void);
static void ExitBg(void);
static void LoadBgGraphic(void);
static void InitMsg( START_MENU_WORK * wk );
static void ExitMsg( START_MENU_WORK * wk );
static void InitBmp( START_MENU_WORK * wk );
static void ExitBmp( START_MENU_WORK * wk );
static void InitObj( START_MENU_WORK * wk );
static void ExitObj( START_MENU_WORK * wk );
static void InitBgWinFrame( START_MENU_WORK * wk );
static void ExitBgWinFrame( START_MENU_WORK * wk );
static void InitBlinkAnm( START_MENU_WORK * wk );
static void ExitBlinkAnm( START_MENU_WORK * wk );


static void InitList( START_MENU_WORK * wk );
static void LoadListFrame( START_MENU_WORK * wk );
static void UnloadListFrame( START_MENU_WORK * wk );
static void InitListPut( START_MENU_WORK * wk );
static void PutListItem( START_MENU_WORK * wk, u8 item, s8 py );
static void ChangeListItemPalette( START_MENU_WORK * wk, u8 item, s8 py, u8 pal );
static s8 GetListPutY( START_MENU_WORK * wk, s8 py );

static void ScrollObj( START_MENU_WORK * wk, int vec );

static void SetBlendAlpha(void);

static int SetFadeIn( START_MENU_WORK * wk, int next );
static int SetFadeOut( START_MENU_WORK * wk, int next );

static BOOL CursorMove( START_MENU_WORK * wk, s8 vec );

static void VanishMenuObj( START_MENU_WORK * wk, BOOL flg );

static void PutNewGameWarrning( START_MENU_WORK * wk );
static void ClearNewGameWarrning( START_MENU_WORK * wk );

static void PutMessage( START_MENU_WORK * wk, int strIdx );
static void ClearMessage( START_MENU_WORK * wk );
static BOOL MainMessage( START_MENU_WORK * wk );

static void SetYesNoMenu( START_MENU_WORK * wk );

static void PutContinueInfo( START_MENU_WORK * wk );
static void ClearContinueInfo( START_MENU_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

FS_EXTERN_OVERLAY(battle_championship);
FS_EXTERN_OVERLAY(pdw_acc);
extern const GFL_PROC_DATA PDW_ACC_MainProcData;


const GFL_PROC_DATA StartMenuProcData = {
  START_MENU_ProcInit,
  START_MENU_ProcMain,
  START_MENU_ProcEnd,
};

// VRAM����U��
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
	GX_VRAM_BGEXTPLTT_NONE,				// ���C��2D�G���W����BG�g���p���b�g

	GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g

	GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

	GX_VRAM_SUB_OBJ_128_D,					// �T�u2D�G���W����OBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		// �T�u2D�G���W����OBJ�g���p���b�g

	GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
	GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g

	GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
};

// ���C���V�[�P���X
static const pSTARTMENU_FUNC MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_Fade,
	MainSeq_ButtonAnm,

	MainSeq_InitWait,
	MainSeq_Main,
	MainSeq_Scroll,

	MainSeq_Continue,
	MainSeq_NewGame,

	MainSeq_EndSet
};

// ���X�g���ڃf�[�^
static const LIST_ITEM_DATA ListItemData[] =
{
	{ NARC_startmenu_list_frame1_lz_NSCR, LIST_FRAME_CONTINUE_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_NEW_GAME_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_HUSHIGI_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_BATTLE_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_GAME_SYNC_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_WIFI_SET_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_MIC_TEST_SY },
	{ NARC_startmenu_list_frame2_lz_NSCR, LIST_FRAME_MACHINE_SY },
};

// BMPWIN�f�[�^
static const u8 BmpWinData[][6] =
{
	{	//�u��������n�߂�v
		BMPWIN_LIST_FRM, BMPWIN_TITLE_PX, BMPWIN_TITLE_PY,
		BMPWIN_TITLE_SX, BMPWIN_TITLE_SY, BMPWIN_LIST_PAL
	},
	{	// �v���C���[��
		BMPWIN_LIST_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_LIST_PAL
	},
	{	// �Z�[�u�ꏊ
		BMPWIN_LIST_FRM, BMPWIN_PLACE_PX, BMPWIN_PLACE_PY,
		BMPWIN_PLACE_SX, BMPWIN_PLACE_SY, BMPWIN_LIST_PAL
	},
	{	// �v���C����
		BMPWIN_LIST_FRM, BMPWIN_TIME_PX, BMPWIN_TIME_PY,
		BMPWIN_TIME_SX, BMPWIN_TIME_SY, BMPWIN_LIST_PAL
	},
	{	// �}��
		BMPWIN_LIST_FRM, BMPWIN_ZUKAN_PX, BMPWIN_ZUKAN_PY,
		BMPWIN_ZUKAN_SX, BMPWIN_ZUKAN_SY, BMPWIN_LIST_PAL
	},
	{	// �o�b�W
		BMPWIN_LIST_FRM, BMPWIN_BADGE_PX, BMPWIN_BADGE_PY,
		BMPWIN_BADGE_SX, BMPWIN_BADGE_SY, BMPWIN_LIST_PAL
	},
	{	//�u�ŏ�����n�߂�v
		BMPWIN_LIST_FRM, BMPWIN_START_PX, BMPWIN_START_PY,
		BMPWIN_START_SX, BMPWIN_START_SY, BMPWIN_LIST_PAL
	},
	{	//�u�s�v�c�ȑ��蕨�v
		BMPWIN_LIST_FRM, BMPWIN_HUSHIGI_PX, BMPWIN_HUSHIGI_PY,
		BMPWIN_HUSHIGI_SX, BMPWIN_HUSHIGI_SY, BMPWIN_LIST_PAL
	},
	{	//�u�o�g�����v
		BMPWIN_LIST_FRM, BMPWIN_BATTLE_PX, BMPWIN_BATTLE_PY,
		BMPWIN_BATTLE_SX, BMPWIN_BATTLE_SY, BMPWIN_LIST_PAL
	},
	{	//�u�Q�[���V���N�ݒ�v
		BMPWIN_LIST_FRM, BMPWIN_GAMESYNC_PX, BMPWIN_GAMESYNC_PY,
		BMPWIN_GAMESYNC_SX, BMPWIN_GAMESYNC_SY, BMPWIN_LIST_PAL
	},
	{	//�uWi-Fi�ݒ�v
		BMPWIN_LIST_FRM, BMPWIN_WIFI_PX, BMPWIN_WIFI_PY,
		BMPWIN_WIFI_SX, BMPWIN_WIFI_SY, BMPWIN_LIST_PAL
	},
	{	//�u�}�C�N�e�X�g�v
		BMPWIN_LIST_FRM, BMPWIN_MIC_PX, BMPWIN_MIC_PY,
		BMPWIN_MIC_SX, BMPWIN_MIC_SY, BMPWIN_LIST_PAL
	},
	{	//�u�]���}�V�����g���v
		BMPWIN_LIST_FRM, BMPWIN_MACHINE_PX, BMPWIN_MACHINE_PY,
		BMPWIN_MACHINE_SX, BMPWIN_MACHINE_SY, BMPWIN_LIST_PAL
	},

	{	// ���b�Z�[�W
		BMPWIN_MSG_FRM, BMPWIN_MSG_PX, BMPWIN_MSG_PY,
		BMPWIN_MSG_SX, BMPWIN_MSG_SY, BMPWIN_MSG_PAL
	},
/*
	{	// �͂��E������
		BMPWIN_YESNO_FRM, BMPWIN_YESNO_PX, BMPWIN_YESNO_PY,
		BMPWIN_YESNO_SX, BMPWIN_YESNO_SY, BMPWIN_YESNO_PAL
	},
*/
};

// OBJ
static const GFL_CLWK_DATA PlayerObjData = { PLAYER_OBJ_PX, PLAYER_OBJ_PY, 1, 0, 1 };
static const GFL_CLWK_DATA NewObjData = { NEW_OBJ_PX, 0, 0, 0, 1 };





static GFL_PROC_RESULT START_MENU_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_STARTMENU, 0x80000 );
  wk = GFL_PROC_AllocWork( proc, sizeof(START_MENU_WORK), HEAPID_STARTMENU );
  GFL_STD_MemClear( wk, sizeof(START_MENU_WORK) );

	wk->savedata = SaveControl_GetPointer();
  wk->mystatus = SaveData_GetMyStatus( wk->savedata );

/*
  u8 i;
  
    //�q�[�v�쐬

  work->heapId = HEAPID_STARTMENU;
  for( i=0;i<SMI_MAX;i++ )
  {
    work->itemWork[i].enable = FALSE;
  }
  work->misc = SaveData_GetMisc( SaveControl_GetPointer() );
  work->state = SMS_FADE_IN;
  work->anmCnt = 0;

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

  work->anime_r = START_MENU_ANIME_E_R;
  work->anime_g = START_MENU_ANIME_E_G;
  work->anime_b = START_MENU_ANIME_E_B;

  work->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, HEAPID_STARTMENU );

  
  work->pMsgWiFiData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, work->heapId );
*/

  
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT START_MENU_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk = mywk;

	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
	  return GFL_PROC_RES_FINISH;
	}

  GFL_TCBL_Main( wk->tcbl );
	GFL_CLACT_SYS_Main();

  return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT START_MENU_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  START_MENU_WORK * wk;
	u8	result;
	
	wk = mywk;
	result = wk->listResult;

	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_STARTMENU );

	switch( result ){
	case LIST_ITEM_CONTINUE:			// ��������
		GameStart_Continue();
/*
		if( wk->continueRet == 0 ){
			GameStart_ContinueNet();
		}else{
			GameStart_ContinueNetOff();
		}
*/
		break;

	case LIST_ITEM_NEW_GAME:			// �ŏ�����
		GameStart_Beginning();
		break;

	case LIST_ITEM_HUSHIGI:				// �s�v�c�ȑ��蕨
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mystery), &MysteryGiftProcData, NULL);
		break;

	case LIST_ITEM_BATTLE:				// �o�g�����
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(battle_championship), &BATTLE_CHAMPIONSHIP_ProcData, NULL);
		break;

	case LIST_ITEM_GAME_SYNC:			// �Q�[���V���N�ݒ�
		GFL_PROC_SysSetNextProc(NO_OVERLAY_ID, &PDW_ACC_MainProcData, NULL);
		break;

	case LIST_ITEM_WIFI_SET:			// Wi-Fi�ݒ�
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(wifi_util), &WifiUtilProcData, NULL);
		break;

	case LIST_ITEM_MIC_TEST:			// �}�C�N�e�X�g
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL);
		break;

	case LIST_ITEM_MACHINE:				// �]���}�V�����g��
		break;

	default:		// �L�����Z��
		GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
		break;
	}

  return GFL_PROC_RES_FINISH;
}





static int MainSeq_Init( START_MENU_WORK * wk )
{
	// �Z�[�u�f�[�^���Ȃ��ꍇ�́u�ŏ�����v�̏����ɔ�΂�
	if( SaveData_GetExistFlag( wk->savedata ) == FALSE ){
		wk->listResult = LIST_ITEM_NEW_GAME;
		return MAINSEQ_END;
	}

	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN);

	InitVram();
	InitBg();
	LoadBgGraphic();
	InitMsg( wk );

	InitList( wk );
	LoadListFrame( wk );

	InitBmp( wk );
	InitObj( wk );

	InitBgWinFrame( wk );

	InitListPut( wk );

	InitBlinkAnm( wk );

	SetBlendAlpha();

	InitVBlank( wk );

	return MAINSEQ_INIT_WAIT;
}

static int MainSeq_Release( START_MENU_WORK * wk )
{
	ExitVBlank( wk );

	ExitBlinkAnm( wk );

	ExitBgWinFrame( wk );

	ExitObj( wk );
	ExitBmp( wk );

	UnloadListFrame( wk );

	ExitMsg( wk );
	ExitBg();

	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	return MAINSEQ_END;
}

static int MainSeq_Fade( START_MENU_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->fadeSeq;
	}
	return MAINSEQ_FADE;
}

static int MainSeq_ButtonAnm( START_MENU_WORK * wk )
{
	return MAINSEQ_BUTTON_ANM;
}

static int MainSeq_InitWait( START_MENU_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<BMPWIN_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->util[i], wk->que );
	}

	if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
		return SetFadeIn( wk, MAINSEQ_MAIN );
	}
	return MAINSEQ_INIT_WAIT;
}

static int MainSeq_Main( START_MENU_WORK * wk )
{
	BLINKPALANM_Main( wk->blink );

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		if( wk->list[wk->listPos] == LIST_ITEM_NEW_GAME ){
			PutNewGameWarrning( wk );
			return MAINSEQ_NEWGAME;
		}else if( wk->list[wk->listPos] == LIST_ITEM_CONTINUE ){
			wk->listResult = wk->list[wk->listPos];
			return MAINSEQ_CONTINUE;
		}
		wk->listResult = wk->list[wk->listPos];
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		wk->listResult = LIST_ITEM_MAX;
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
		if( CursorMove( wk, -1 ) == TRUE ){
			return MAINSEQ_SCROLL;
		}
	}

	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
		if( CursorMove( wk, 1 ) == TRUE ){
			return MAINSEQ_SCROLL;
		}
	}

	return MAINSEQ_MAIN;
}

static int MainSeq_Scroll( START_MENU_WORK * wk )
{
	if( wk->bgScrollCount == 3 ){
		wk->bgScrollCount = 0;
		wk->bgScrollSpeed = 0;
		ChangeListItemPalette( wk, wk->list[wk->listPos], GetListPutY(wk,wk->cursorPutPos), CURSOR_PALETTE );
		return MAINSEQ_MAIN;
	}

	wk->bgScroll += wk->bgScrollSpeed;
	if( wk->bgScroll < 0 ){
		wk->bgScroll += 512;
	}else if( wk->bgScroll >= 512 ){
		wk->bgScroll -= 512;
	}

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );

	ScrollObj( wk, -wk->bgScrollSpeed );

	wk->bgScrollCount++;

	return MAINSEQ_SCROLL;
}

static int MainSeq_Continue( START_MENU_WORK * wk )
{
	switch( wk->continueSeq ){
	case 0:
		// ���b�Z�[�W�\��
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );
		VanishMenuObj( wk, FALSE );
		wk->continueSeq++;
		break;

	case 1:
		PutMessage( wk, START_MENU_STR_CONTINUE_01 );
		PutContinueInfo( wk );
		wk->continueSeq++;
		break;

	case 2:
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
			SetYesNoMenu( wk );
			wk->continueSeq++;
		}
		break;

	case 3:
		// �͂��E�������҂�
		switch( BmpMenu_YesNoSelectMain( wk->mwk ) ){
		case 0:
			// �{�̐ݒ�̖����ݒ�
			if( DS_SYSTEM_IsAvailableWireless() == FALSE ){
				PutMessage( wk, START_MENU_STR_ATTENTION_01 );
				wk->continueSeq = 5;
				break;
			}
/*	�Ƃ肠�����R�����g�A�E�g
			// �y�A�����^���R���g���[��
			if( DS_SYSTEM_IsRestrictUGC() == TRUE ){
				PutMessage( wk, START_MENU_STR_ATTENTION_02 );
				wk->continueSeq = 5;
				break;
			}
*/
			wk->continueRet = 0;
			wk->continueSeq = 4;
			break;

		case BMPMENU_CANCEL:
			wk->continueRet = 1;
			wk->continueSeq = 4;
			break;
		}
		break;

	case 4:		// �I��
		ClearContinueInfo( wk );
		ClearMessage( wk );
		return SetFadeOut( wk, MAINSEQ_RELEASE );

	case 5:		// �����ݒ�G���[
		// ���b�Z�[�W�҂�
		if( MainMessage( wk ) == FALSE ){
	    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
				wk->continueRet = 1;
				wk->continueSeq = 4;
				break;
			}
		}
		break;
	}

	PRINTSYS_QUE_Main( wk->que );
	PRINT_UTIL_Trans( &wk->utilWin, wk->que );

	return MAINSEQ_CONTINUE;
}

static int MainSeq_NewGame( START_MENU_WORK * wk )
{
	if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
		// �`�{�^��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
			ClearNewGameWarrning( wk );
			wk->listResult = LIST_ITEM_NEW_GAME;
			return SetFadeOut( wk, MAINSEQ_RELEASE );
		}
		// �a�{�^��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
			ClearNewGameWarrning( wk );
			return MAINSEQ_MAIN;
		}
	}

	PRINTSYS_QUE_Main( wk->que );
	PRINT_UTIL_Trans( &wk->utilWin, wk->que );

	return MAINSEQ_NEWGAME;
}

static int MainSeq_EndSet( START_MENU_WORK * wk )
{
	return MAINSEQ_END_SET;
}



static void VBlankTask( GFL_TCB * tcb, void * work )
{
//	BOX2_SYS_WORK * syswk = work;

	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

//	PaletteFadeTrans( syswk->app->pfd );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static void InitVBlank( START_MENU_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

static void ExitVBlank( START_MENU_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

static void InitVram(void)
{
	GFL_DISP_ClearVRAM( NULL );
	GFL_DISP_SetBank( &VramTbl );
}

static void InitBg(void)
{
	GFL_BG_Init( HEAPID_STARTMENU );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF���b�Z�[�W�E�B���h�E
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���X�g����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF���X�g�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xb000, GX_BG_CHARBASE_0x08000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// ���C����ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// �T�u��ʁF����
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF�t���[��
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// �T�u��ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_ON );
}

static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}

static void LoadBgGraphic(void)
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_startmenu_bgu_NCLR, PALTYPE_MAIN_BG, 0, 0x20*4, HEAPID_STARTMENU );
//	GFL_ARCHDL_UTIL_TransVramBgCharacter(
//		ah, NARC_startmenu_frame_bgu_lz_NCGR, GFL_BG_FRAME1_M, 0, 0, TRUE, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_startmenu_bgu_lz_NCGR, GFL_BG_FRAME2_M, 0, 0, TRUE, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_startmenu_base_bgu_lz_NSCR,
		GFL_BG_FRAME3_M, 0, 0, TRUE, HEAPID_STARTMENU );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_startmenu_bgd_NCLR, PALTYPE_SUB_BG, 0, 0x20*4, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_startmenu_bgd_lz_NCGR, GFL_BG_FRAME1_S, 0, 0, TRUE, HEAPID_STARTMENU );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_startmenu_base_bgd_lz_NSCR,
		GFL_BG_FRAME2_S, 0, 0, TRUE, HEAPID_STARTMENU );

	GFL_ARC_CloseDataHandle( ah );

	// �V�X�e���E�B���h�E
	BmpWinFrame_GraphicSet(
		GFL_BG_FRAME0_M, MESSAGE_WIN_CHAR_NUM,
		MESSAGE_WIN_PLTT_NUM, MENU_TYPE_SYSTEM, HEAPID_STARTMENU );

	// �t�H���g�p���b�g
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
		FONT_PALETTE_M*0x20, 0x20, HEAPID_STARTMENU );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
		FONT_PALETTE_S*0x20, 0x20, HEAPID_STARTMENU );
}

static void InitBlinkAnm( START_MENU_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	wk->blink = BLINKPALANM_Create(
								CURSOR_PALETTE*16, 16, GFL_BG_FRAME2_M, HEAPID_STARTMENU );

	BLINKPALANM_SetPalBufferArcHDL(
		wk->blink, ah, NARC_startmenu_bgu_NCLR, CURSOR_PALETTE*16, CURSOR_PALETTE2*16 );

	GFL_ARC_CloseDataHandle( ah );
}

static void ExitBlinkAnm( START_MENU_WORK * wk )
{
	BLINKPALANM_Exit( wk->blink );
}

static void InitMsg( START_MENU_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_startmenu_dat, HEAPID_STARTMENU );
	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STARTMENU );
	wk->wset = WORDSET_Create( HEAPID_STARTMENU );
	wk->que  = PRINTSYS_QUE_Create( HEAPID_STARTMENU );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_STARTMENU );

	wk->tcbl = GFL_TCBL_Init( HEAPID_STARTMENU, HEAPID_STARTMENU, 1, 4 );
}

static void ExitMsg( START_MENU_WORK * wk )
{
  GFL_TCBL_Exit( wk->tcbl );

	GFL_STR_DeleteBuffer( wk->exp );
	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

static void InitBmp( START_MENU_WORK * wk )
{
	const u8 * dat;
	STRBUF * str;
	u32	i;

	GFL_BMPWIN_Init( HEAPID_STARTMENU );

	for( i=0; i<BMPWIN_MAX; i++ ){
		dat = BmpWinData[i];
		wk->util[i].win = GFL_BMPWIN_Create(
												dat[0], dat[1], dat[2], dat[3], dat[4], dat[5], GFL_BMP_CHRAREA_GET_B );
	}

	// ���ڕ�����`��
	// ��������n�߂�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_01 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_TITLE], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// ��l����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_06 );
	WORDSET_RegisterPlayerName( wk->wset, 0, wk->mystatus );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	if( MyStatus_GetMySex( wk->mystatus ) == PM_MALE ){
		PRINT_UTIL_PrintColor( &wk->util[BMPWIN_NAME], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15BLN );
	}else{
		PRINT_UTIL_PrintColor( &wk->util[BMPWIN_NAME], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15RN );
	}
	GFL_STR_DeleteBuffer( str );
	// �ꏊ
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_10 );
	{
		PLAYERWORK_SAVE	plwk;
		u16	num;

		SaveData_SituationLoad_PlayerWorkSave( wk->savedata, &plwk );
		ZONEDATA_Open( HEAPID_STARTMENU );
		num = ZONEDATA_GetPlaceNameID( plwk.zoneID );
		ZONEDATA_Close();
		WORDSET_RegisterPlaceName( wk->wset, 0, num );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_PLACE], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �v���C����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_03 );
	{
		PLAYTIME * ptime = SaveData_GetPlayTime( wk->savedata );
		WORDSET_RegisterNumber( wk->wset, 0, PLAYTIME_GetHour(ptime), 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_RegisterNumber( wk->wset, 1, PLAYTIME_GetMinute(ptime), 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_TIME], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �}��
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_04 );
	{
		u16	num = ZUKANSAVE_GetZukanPokeSeeCount( ZUKAN_SAVEDATA_GetZukanSave(wk->savedata), HEAPID_STARTMENU );
		WORDSET_RegisterNumber( wk->wset, 0, num, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_ZUKAN], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �o�b�W
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_01_05 );
	{
		int	num = MISC_GetBadgeCount( SaveData_GetMisc(wk->savedata) );
		WORDSET_RegisterNumber( wk->wset, 0, num, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	}
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_BADGE], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );

	// �ŏ�����n�߂�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_02 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_START], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �s�v�c�ȑ��蕨
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_03 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_HUSHIGI], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �o�g�����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_05 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_BATTLE], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �Q�[���V���N�ݒ�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_06 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_GAMESYNC], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// Wi-Fi�ݒ�
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_04 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_WIFI], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �}�C�N�e�X�g
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_08 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_MIC], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �]���}�V�����g��
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ITEM_07 );
	PRINT_UTIL_PrintColor( &wk->util[BMPWIN_MACHINE], wk->que, 0, 4, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
}

static void ExitBmp( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<BMPWIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->util[i].win );
	}

	GFL_BMPWIN_Exit();
}

static void InitObj( START_MENU_WORK * wk )
{
	{	// �V�X�e��������
		const GFL_CLSYS_INIT init = {
			0, 0,									// ���C���@�T�[�t�F�[�X�̍�����W
			0, 512,								// �T�u�@�T�[�t�F�[�X�̍�����W
			4,										// ���C�����OAM�Ǘ��J�n�ʒu	4�̔{��
			124,									// ���C�����OAM�Ǘ���				4�̔{��
			4,										// �T�u���OAM�Ǘ��J�n�ʒu		4�̔{��
			124,									// �T�u���OAM�Ǘ���					4�̔{��
			0,										// �Z��Vram�]���Ǘ���
	
			CHRRES_MAX,						// �o�^�ł���L�����f�[�^��
			PALRES_MAX,						// �o�^�ł���p���b�g�f�[�^��
			CELRES_MAX,						// �o�^�ł���Z���A�j���p�^�[����
			0,										// �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j

		  16,										// ���C�� CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		  16										// �T�u CGR�@VRAM�Ǘ��̈�@�J�n�I�t�Z�b�g�i�L�����N�^�P�ʁj
		};
		GFL_CLACT_SYS_Create( &init, &VramTbl, HEAPID_STARTMENU );
	}

	{	// ���\�[�X�ǂݍ���
		ARCHANDLE * ah;
		
		// �v���C���[
		ah = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, HEAPID_STARTMENU_L );

		if( MyStatus_GetMySex( wk->mystatus ) == PM_MALE ){
			wk->chrRes[CHRRES_PLAYER] = GFL_CLGRP_CGR_Register(
																		ah, NARC_wifileadingchar_hero_NCGR,
																		FALSE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
		  wk->palRes[PALRES_PLAYER] = GFL_CLGRP_PLTT_Register(
																		ah, NARC_wifileadingchar_hero_NCLR,
																		CLSYS_DRAW_MAIN, PALNUM_PLAYER*0x20, HEAPID_STARTMENU );
	    wk->celRes[CELRES_PLAYER] = GFL_CLGRP_CELLANIM_Register(
																		ah,
																		NARC_wifileadingchar_hero_NCER,
																		NARC_wifileadingchar_hero_NANR,
																		HEAPID_STARTMENU );
		}else{
			wk->chrRes[CHRRES_PLAYER] = GFL_CLGRP_CGR_Register(
																		ah, NARC_wifileadingchar_heroine_NCGR,
																		FALSE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
		  wk->palRes[PALRES_PLAYER] = GFL_CLGRP_PLTT_Register(
																		ah, NARC_wifileadingchar_heroine_NCLR,
																		CLSYS_DRAW_MAIN, PALNUM_PLAYER*0x20, HEAPID_STARTMENU );
	    wk->celRes[CELRES_PLAYER] = GFL_CLGRP_CELLANIM_Register(
																		ah,
																		NARC_wifileadingchar_heroine_NCER,
																		NARC_wifileadingchar_heroine_NANR,
																		HEAPID_STARTMENU );
		}

		GFL_ARC_CloseDataHandle( ah );

		// ���̑�
		ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );
		wk->chrRes[CHRRES_OBJ_U] = GFL_CLGRP_CGR_Register(
																ah, NARC_startmenu_obj_u_lz_NCGR,
																TRUE, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
	  wk->palRes[PALRES_OBJ_U] = GFL_CLGRP_PLTT_Register(
																ah, NARC_startmenu_obj_u_NCLR,
																CLSYS_DRAW_MAIN, PALNUM_OBJ_U*0x20, HEAPID_STARTMENU );
    wk->celRes[CELRES_OBJ_U] = GFL_CLGRP_CELLANIM_Register(
																ah,
																NARC_startmenu_obj_u_NCER,
																NARC_startmenu_obj_u_NANR,
																HEAPID_STARTMENU );
		GFL_ARC_CloseDataHandle( ah );
	}

	{	// OBJ�ǉ�
		u32	i;

		wk->clunit = GFL_CLACT_UNIT_Create( OBJ_ID_MAX, 0, HEAPID_STARTMENU );

		wk->clwk[OBJ_ID_PLAYER] = GFL_CLACT_WK_Create(
																wk->clunit,
																wk->chrRes[CHRRES_PLAYER],
																wk->palRes[PALRES_PLAYER],
																wk->celRes[CELRES_PLAYER],
																&PlayerObjData, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );

		for( i=OBJ_ID_NEW_HUSHIGI; i<=OBJ_ID_NEW_MACHINE; i++ ){
			wk->clwk[i] = GFL_CLACT_WK_Create(
											wk->clunit,
											wk->chrRes[CHRRES_OBJ_U],
											wk->palRes[PALRES_OBJ_U],
											wk->celRes[CELRES_OBJ_U],
											&NewObjData, CLSYS_DRAW_MAIN, HEAPID_STARTMENU );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], FALSE );
		}
	}

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

static void ExitObj( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<OBJ_ID_MAX; i++ ){
		GFL_CLACT_WK_Remove( wk->clwk[i] );
	}
	GFL_CLACT_UNIT_Delete( wk->clunit );

	for( i=0; i<CHRRES_MAX; i++ ){
		GFL_CLGRP_CGR_Release( wk->chrRes[i] );
	}
	for( i=0; i<PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( wk->palRes[i] );
	}
	for( i=0; i<CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( wk->celRes[i] );
	}

	GFL_CLACT_SYS_Delete();
}

static void InitBgWinFrame( START_MENU_WORK * wk )
{
	u32	i;

	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_NONE, LIST_ITEM_MAX, HEAPID_STARTMENU );

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		BGWINFRM_Add( wk->wfrm, i, BMPWIN_LIST_FRM, LIST_FRAME_SX, ListItemData[i].sy );
	}

	// ��������
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_TITLE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_NAME].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_PLACE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_TIME].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_ZUKAN].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_CONTINUE, wk->util[BMPWIN_BADGE].win );

	// ����ȊO�̍���
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_NEW_GAME, wk->util[BMPWIN_START].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_HUSHIGI, wk->util[BMPWIN_HUSHIGI].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_BATTLE, wk->util[BMPWIN_BATTLE].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_GAME_SYNC, wk->util[BMPWIN_GAMESYNC].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_WIFI_SET, wk->util[BMPWIN_WIFI].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_MIC_TEST, wk->util[BMPWIN_MIC].win );
	BGWINFRM_BmpWinOn( wk->wfrm, LIST_ITEM_MACHINE, wk->util[BMPWIN_MACHINE].win );
}

static void ExitBgWinFrame( START_MENU_WORK * wk )
{
	BGWINFRM_Exit( wk->wfrm );
}


static void InitList( START_MENU_WORK * wk )
{
	u32	i;
	
	i = 0;
	// ��������
#if 0
	if( SaveData_GetExistFlag( wk->savedata ) == TRUE ){
		wk->list[i] = LIST_ITEM_CONTINUE;
		i++;
	}
#else
	wk->list[i] = LIST_ITEM_CONTINUE;
	i++;
#endif
	// �ŏ�����
	wk->list[i] = LIST_ITEM_NEW_GAME;
	i++;
	// �s�v�c�ȑ��蕨
	if( 1 ){
		wk->list[i] = LIST_ITEM_HUSHIGI;
		i++;
	}
	// �o�g�����
	if( 1 ){
		wk->list[i] = LIST_ITEM_BATTLE;
		i++;
	}
	// �Q�[���V���N�ݒ�
	if( 1 ){
		wk->list[i] = LIST_ITEM_GAME_SYNC;
		i++;
	}
	// Wi-Fi�ݒ�
	wk->list[i] = LIST_ITEM_WIFI_SET;
	i++;
	// �}�C�N�e�X�g
	wk->list[i] = LIST_ITEM_MIC_TEST;
	i++;
	// �]���}�V�����g��
	if( 1 ){
		wk->list[i] = LIST_ITEM_MACHINE;
		i++;
	}

	// �_�~�[�����Ă���
	for( i=i; i<LIST_ITEM_MAX; i++ ){
		wk->list[i] = LIST_ITEM_MAX;
	}
}

static void LoadListFrame( START_MENU_WORK * wk )
{
	ARCHANDLE * ah;
	u32	i;
	
	ah = GFL_ARC_OpenDataHandle( ARCID_STARTMENU, HEAPID_STARTMENU_L );

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		const LIST_ITEM_DATA * item;
		NNSG2dScreenData * scrn;
		void * buf;
		u32	siz;

		item = &ListItemData[i];
		siz  = LIST_FRAME_SX * item->sy * 2;

		wk->listFrame[i] = GFL_HEAP_AllocMemory( HEAPID_STARTMENU, siz );
		buf = GFL_ARCHDL_UTIL_LoadScreen( ah, item->scrnIdx, TRUE, &scrn, HEAPID_STARTMENU );
		GFL_STD_MemCopy16( (void *)scrn->rawData, wk->listFrame[i], siz );
		GFL_HEAP_FreeMemory( buf );
	}

	GFL_ARC_CloseDataHandle( ah );
}

static void UnloadListFrame( START_MENU_WORK * wk )
{
	u32	i;

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		GFL_HEAP_FreeMemory( wk->listFrame[i] );
	}
}

static void PutNewObj( START_MENU_WORK * wk, u16 idx, s16 py )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
	pos.y = py;
	GFL_CLACT_WK_SetPos( wk->clwk[idx], &pos, CLSYS_DRAW_MAIN );
}

static void InitListPut( START_MENU_WORK * wk )
{
	u32	i;
	s8	py;

	wk->cursorPutPos = LIST_ITEM_PY;
	py = LIST_ITEM_PY;

	for( i=0; i<LIST_ITEM_MAX; i++ ){
		if( wk->list[i] == LIST_ITEM_MAX ){
			break;
		}
		PutListItem( wk, wk->list[i], py );

		if( wk->list[i] == LIST_ITEM_HUSHIGI ){
			PutNewObj( wk, OBJ_ID_NEW_HUSHIGI, py*8+ListItemData[wk->list[i]].sy*8/2 );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_HUSHIGI], TRUE );
		}else if( wk->list[i] == LIST_ITEM_BATTLE ){
			PutNewObj( wk, OBJ_ID_NEW_BATTLE, py*8+ListItemData[wk->list[i]].sy*8/2 );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_BATTLE], TRUE );
		}else if( wk->list[i] == LIST_ITEM_GAME_SYNC ){
			PutNewObj( wk, OBJ_ID_NEW_GAMESYNC, py*8+ListItemData[wk->list[i]].sy*8/2 );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_GAMESYNC], TRUE );
		}else if( wk->list[i] == LIST_ITEM_MACHINE ){
			PutNewObj( wk, OBJ_ID_NEW_MACHINE, py*8+ListItemData[wk->list[i]].sy*8/2 );
			GFL_CLACT_WK_SetDrawEnable( wk->clwk[OBJ_ID_NEW_MACHINE], TRUE );
		}

		py += ListItemData[wk->list[i]].sy;
	}
}

static void PutListItem( START_MENU_WORK * wk, u8 item, s8 py )
{
	u32	i;

	if( item == LIST_ITEM_MAX ){
		for( i=0; i<3; i++ ){
			if( py >= LIST_SCRN_SY ){
				py -= LIST_SCRN_SY;
			}else if( py < 0 ){
				py += LIST_SCRN_SY;
			}
			GFL_BG_FillScreen(
				GFL_BG_FRAME2_M, 0,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1, 0 );
			GFL_BG_FillScreen(
				GFL_BG_FRAME1_M, 0,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1, 0 );
			py++;
		}
	}else{
		for( i=0; i<ListItemData[item].sy; i++ ){
			if( py >= LIST_SCRN_SY ){
				py -= LIST_SCRN_SY;
			}else if( py < 0 ){
				py += LIST_SCRN_SY;
			}
			GFL_BG_WriteScreenExpand(
				GFL_BG_FRAME2_M,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1,
				wk->listFrame[item],
				0, i,
				LIST_FRAME_SX,
				ListItemData[item].sy );
			GFL_BG_WriteScreenExpand(
				GFL_BG_FRAME1_M,
				LIST_ITEM_PX, py,
				LIST_FRAME_SX, 1,
				BGWINFRM_FrameBufGet(wk->wfrm,item),
				0, i,
				LIST_FRAME_SX,
				ListItemData[item].sy );
			py++;
		}
	}

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
}

static void ChangeListItemPalette( START_MENU_WORK * wk, u8 item, s8 py, u8 pal )
{
	u32	i;

	for( i=0; i<ListItemData[item].sy; i++ ){
		if( py >= LIST_SCRN_SY ){
			py -= LIST_SCRN_SY;
		}else if( py < 0 ){
			py += LIST_SCRN_SY;
		}
		GFL_BG_ChangeScreenPalette(
			GFL_BG_FRAME2_M,
			LIST_ITEM_PX, py,
			LIST_FRAME_SX, 1,
			pal );
		py++;
	}

	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
}


static s8 GetListPutY( START_MENU_WORK * wk, s8 py )
{
	return ( py + ( wk->bgScroll / 8 ) );
}

static void ScrollObj( START_MENU_WORK * wk, int vec )
{
	GFL_CLACTPOS	pos;
	u32	i;

	for( i=0; i<OBJ_ID_MAX; i++ ){
		GFL_CLACT_WK_GetPos( wk->clwk[i], &pos, CLSYS_DRAW_MAIN );
		pos.y += vec;
		GFL_CLACT_WK_SetPos( wk->clwk[i], &pos, CLSYS_DRAW_MAIN );
	}
}


static void SetBlendAlpha(void)
{
	G2_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG2,
		GX_BLEND_PLANEMASK_BG3,
		10, 6 );
}

static int SetFadeIn( START_MENU_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STARTMENU );
	wk->fadeSeq = next;
	return MAINSEQ_FADE;
}

static int SetFadeOut( START_MENU_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STARTMENU );
	wk->fadeSeq = next;
	return MAINSEQ_FADE;
}



static BOOL CursorMove( START_MENU_WORK * wk, s8 vec )
{
	u8	now_item;
	u8	now_pos;

	now_pos  = wk->listPos;
	now_item = wk->list[ now_pos ];

	wk->bgScrollCount = 0;

	if( vec < 0 ){
		if( wk->listPos == 0 ){
			return FALSE;
		}

		{
			u8	mv_item;
			s8	mv_pos;
			s8	cur_py;

			mv_pos  = now_pos + vec;
			mv_item = wk->list[ mv_pos ];

			cur_py = wk->cursorPutPos - ListItemData[mv_pos].sy;

			ChangeListItemPalette( wk, now_item, GetListPutY(wk,wk->cursorPutPos), LIST_PALETTE );

			if( cur_py < LIST_ITEM_PY ){
				if( mv_pos == 0 ){
					wk->bgScrollSpeed = ( ListItemData[ mv_item ].sy - (wk->cursorPutPos-LIST_ITEM_PY) ) / 3 * -8;
//					wk->bgScrollCount = ( ListItemData[ mv_item ].sy - (wk->cursorPutPos-LIST_ITEM_PY) ) * -8;
//					wk->bgScrollSpeed = wk->bgScrollCount / 3;
//					OS_Printf( " mv -0 = %d\n", ( ListItemData[ mv_item ].sy - (wk->cursorPutPos-LIST_ITEM_PY) ) );
					wk->cursorPutPos = LIST_ITEM_PY;

				}else{
					wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * -8;
//					wk->bgScrollCount = ListItemData[ mv_item ].sy * -8;
//					wk->bgScrollSpeed = wk->bgScrollCount / 3;

				}
			}else{
				wk->cursorPutPos = cur_py;
			}
		}

	}else{
		if( ( now_pos + vec ) >= LIST_ITEM_MAX ){
			return FALSE;
		}
		if( wk->list[now_pos+vec] == LIST_ITEM_MAX ){
			return FALSE;
		}

		{
			u8	mv_item;
			u8	mv_pos;
			u8	cur_py;

			mv_pos  = now_pos + vec;
			mv_item = wk->list[ mv_pos ];

			cur_py = wk->cursorPutPos + ListItemData[now_pos].sy;

			ChangeListItemPalette( wk, now_item, GetListPutY(wk,wk->cursorPutPos), LIST_PALETTE );

			if( cur_py == 24 ){
				wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * 8;
//				wk->bgScrollCount = ListItemData[ mv_item ].sy * 8;
//				wk->bgScrollSpeed = wk->bgScrollCount / 3;
			}else if( ( cur_py + ListItemData[mv_item].sy ) > 24 ){
				wk->bgScrollSpeed = ListItemData[ mv_item ].sy / 3 * 8;
//				wk->bgScrollCount = ListItemData[ mv_item ].sy * 8;
//				wk->bgScrollSpeed = wk->bgScrollCount / 3;
			}else{
				wk->cursorPutPos = cur_py;
			}
		}
	}

	wk->listPos += vec;

	return TRUE;
}

static void VanishMenuObj( START_MENU_WORK * wk, BOOL flg )
{
	u32	i;

	for( i=OBJ_ID_PLAYER; i<=OBJ_ID_NEW_MACHINE; i++ ){
		GFL_CLACT_WK_SetDrawEnable( wk->clwk[i], flg );
	}
}

static void PutWarrningWindow( u8 px, u8 py, u8 sx, u8 sy, u8 frm )
{
	// ����
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM, px, py, 1, 1, WARRNING_WIN_PLTT_NUM );
	// �E��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+2, px+sx-1, py, 1, 1, WARRNING_WIN_PLTT_NUM );
	// ����
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+6, px, py+sy-1, 1, 1, WARRNING_WIN_PLTT_NUM );
	// �E��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+8, px+sx-1, py+sy-1, 1, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+1, px+1, py, sx-2, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+7, px+1, py+sy-1, sx-2, 1, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+3, px, py+1, 1, sy-2, WARRNING_WIN_PLTT_NUM );
	// �E
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+5, px+sx-1, py+1, 1, sy-2, WARRNING_WIN_PLTT_NUM );
	// ��
	GFL_BG_FillScreen( frm, WARRNING_WIN_CHAR_NUM+4, px+1, py+1, sx-2, sy-2, WARRNING_WIN_PLTT_NUM );

	GFL_BG_LoadScreenV_Req( frm );
}

static void ClearWarrningWindow( u8 px, u8 py, u8 sx, u8 sy, u8 frm )
{
	GFL_BG_FillScreen( frm, 0, px, py, sx, sy, 0 );
	GFL_BG_LoadScreenV_Req( frm );
}


static void PutNewGameWarrning( START_MENU_WORK * wk )
{
	STRBUF * str;

	wk->utilWin.win = GFL_BMPWIN_Create(
											BMPWIN_NEWGAME_WIN_FRM,
											BMPWIN_NEWGAME_WIN_PX, BMPWIN_NEWGAME_WIN_PY,
											BMPWIN_NEWGAME_WIN_SX, BMPWIN_NEWGAME_WIN_SY,
											BMPWIN_NEWGAME_WIN_PAL, GFL_BMP_CHRAREA_GET_B );

	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_06 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_ATTENTION_PY, str, wk->font, FCOL_MP15RN );
	GFL_STR_DeleteBuffer( str );
	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_07 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_MESSAGE_PY, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �`
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_08 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_A_BUTTON_PY, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );
	// �a
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_09 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, NEW_GAME_WARRNING_B_BUTTON_PY, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeScreen( wk->utilWin.win );
	GFL_BG_LoadScreenV_Req( BMPWIN_NEWGAME_WIN_FRM );

	// �E�B���h�E�`��
	PutWarrningWindow(
		NEW_GAME_WARRNING_WIN_PX, NEW_GAME_WARRNING_WIN_PY,
		NEW_GAME_WARRNING_WIN_SX, NEW_GAME_WARRNING_WIN_SY, GFL_BG_FRAME2_M );

	// ���X�g��ޔ�
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_X_SET, 256 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_X_SET, 256 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, 0 );

	VanishMenuObj( wk, FALSE );
}

static void ClearNewGameWarrning( START_MENU_WORK * wk )
{
	ClearWarrningWindow(
		NEW_GAME_WARRNING_WIN_PX, NEW_GAME_WARRNING_WIN_PY,
		NEW_GAME_WARRNING_WIN_SX, NEW_GAME_WARRNING_WIN_SY, GFL_BG_FRAME2_M );

	GFL_BG_ClearScreenCodeVReq( BMPWIN_NEWGAME_WIN_FRM, 0 );

	GFL_BMPWIN_Delete( wk->utilWin.win );

	// ���X�g���A
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_X_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_X_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->bgScroll );

	VanishMenuObj( wk, TRUE );
}

static void PutMessage( START_MENU_WORK * wk, int strIdx )
{
  GFL_MSG_GetString( wk->mman, strIdx, wk->exp );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[BMPWIN_MSG].win), 15 );
	BmpWinFrame_Write(
		wk->util[BMPWIN_MSG].win, WINDOW_TRANS_OFF, MESSAGE_WIN_CHAR_NUM, MESSAGE_WIN_PLTT_NUM );

	wk->stream = PRINTSYS_PrintStream(
								wk->util[BMPWIN_MSG].win,
								0, 0, wk->exp,
								wk->font,
								MSGSPEED_GetWait(),
								wk->tcbl,
								10,		// tcbl pri
								HEAPID_STARTMENU,
								15 );	// clear color
	GFL_BMPWIN_MakeTransWindow_VBlank( wk->util[BMPWIN_MSG].win );
}

static void ClearMessage( START_MENU_WORK * wk )
{
	BmpWinFrame_Clear( wk->util[BMPWIN_MSG].win, WINDOW_TRANS_ON_V );
}

static BOOL MainMessage( START_MENU_WORK * wk )
{
  switch( PRINTSYS_PrintStreamGetState(wk->stream) ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      PRINTSYS_PrintStreamShortWait( wk->stream, 0 );
    }
    break;

  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ){
      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      PRINTSYS_PrintStreamReleasePause( wk->stream );
    }
    break;

  case PRINTSTREAM_STATE_DONE: //�I��
    PRINTSYS_PrintStreamDelete( wk->stream );
		return FALSE;
	}

	return TRUE;
}

static void SetYesNoMenu( START_MENU_WORK * wk )
{
	BMPWIN_YESNO_DAT	dat;

	dat.frmnum = BMPWIN_YESNO_FRM;
	dat.pos_x  = BMPWIN_YESNO_PX;
	dat.pos_y  = BMPWIN_YESNO_PY;
	dat.palnum = BMPWIN_YESNO_PAL;
	dat.chrnum = 0;

	wk->mwk = BmpMenu_YesNoSelectInit(
							&dat, MESSAGE_WIN_CHAR_NUM, MESSAGE_WIN_PLTT_NUM, 0, HEAPID_STARTMENU );
}

static void PutContinueInfo( START_MENU_WORK * wk )
{
	STRBUF * str;

	wk->utilWin.win = GFL_BMPWIN_Create(
											BMPWIN_CONTINUE_WIN_FRM,
											BMPWIN_CONTINUE_WIN_PX, BMPWIN_CONTINUE_WIN_PY,
											BMPWIN_CONTINUE_WIN_SX, BMPWIN_CONTINUE_WIN_SY,
											BMPWIN_CONTINUE_WIN_PAL, GFL_BMP_CHRAREA_GET_B );

	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_04 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, CONTINUE_INFO_ATTENTION_PY, str, wk->font, FCOL_MP15RN );
	GFL_STR_DeleteBuffer( str );
	// ����
	str = GFL_MSG_CreateString( wk->mman, START_MENU_STR_ATTENTION_05 );
	PRINT_UTIL_PrintColor( &wk->utilWin, wk->que, 0, CONTINUE_INFO_MESSAGE_PY, str, wk->font, FCOL_MP15WN );
	GFL_STR_DeleteBuffer( str );

	GFL_BMPWIN_MakeScreen( wk->utilWin.win );
	GFL_BG_LoadScreenV_Req( BMPWIN_CONTINUE_WIN_FRM );

	// �E�B���h�E�`��
	PutWarrningWindow(
		CONTINUE_INFO_WIN_PX, CONTINUE_INFO_WIN_PY,
		CONTINUE_INFO_WIN_SX, CONTINUE_INFO_WIN_SY, GFL_BG_FRAME1_S );
}

static void ClearContinueInfo( START_MENU_WORK * wk )
{
/*
	ClearWarrningWindow(
		CONTINUE_INFO_WIN_PX, CONTINUE_INFO_WIN_PY,
		CONTINUE_INFO_WIN_SX, CONTINUE_INFO_WIN_SY, GFL_BG_FRAME1_S );
	GFL_BG_ClearScreenCodeVReq( BMPWIN_CONTINUE_WIN_FRM, 0 );
*/

	GFL_BMPWIN_Delete( wk->utilWin.win );
}





#endif
