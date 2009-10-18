//======================================================================
/**
 * @file	field_menu.c
 * @brief	�t�B�[���h���j���[
 * @author	ariizumi
 * @date	09/05/12
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "field_menu.naix"
#include "msg/msg_fldmapmenu.h"

#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "field/field_msgbg.h"
#include "field/fieldmap.h"
#include "field_subscreen.h"
#include "field_menu.h"
#include "field/zonedata.h" //ZONEDATA_IsUnionRoom


//======================================================================
//	define
//======================================================================
#pragma mark [> define

//InfoBar��S0
#define FIELD_MENU_BG_BACK (GFL_BG_FRAME1_S)
#define FIELD_MENU_BG_NAME (GFL_BG_FRAME2_S)

//�X�N���[�����x
#define FIELD_MENU_SCROLL_SPD (64) 

//�����_�[�̃T�[�t�F�C�X�ԍ�
#define FIELD_MENU_RENDER_SURFACE (0)

//�߂鍞�݂̃A�C�e����
#define FIELD_MENU_ITEM_NUM (7) 

//�Z���̉�荞�ݖh�~�`�F�b�N�Ɏg���B�c�̍ő�T�C�Y��
#define FIELD_MENU_ICON_SIZE_Y (32) 
#define FIELD_MENU_CURSOR_SIZE_Y (44)
//�A�C�R���m�莞�̃E�F�C�g
#define FIELD_MENU_ICON_DECIDE_ANIME_CNT (4) 

//�J�[�\���s���ʒu
#define FIELD_MENU_INVALU_CURSOR (0xFF)

//�p���b�g
#define FIELD_MENU_PLT_FONT (0x0d)
//InfoBar��0x0f

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
enum
{
  FMO_COM_PLT,
  FMO_COM_CHR,
  FMO_COM_ANM,
  
  FIELD_MENU_OBJ_RES_NUM,
}FIELD_MENU_OBJ_RES;

//�J�[�\���A�j��
enum
{
  FCA_NORMAL_BIG,
  FCA_NORMAL_SMALL,
  
}FIELD_MENU_CURSOR_ANIME;

//�A�C�R���A�j��
enum
{
  FIA_NORMAL,
  FIA_ACTIVE,
  FIA_DECIDE,
  
}FIELD_MENU_ICON_ANIME;

typedef enum
{
  FMS_WAIT_MOVEIN,
  FMS_WAIT_INIT,
  FMS_LOOP,
  FMS_EXIT_INIT,  
  FMS_WAIT_ICON_ANIME ,
  FMS_EXIT_ENDMENU,     //�I�����̏���
  FMS_EXIT_DECIDEITEM,  //���ڌ��莞�̏���
  FMS_WAIT_MOVEOUT, 
  FMS_FINISH,
  
  FIELD_MENU_STATE_MAX,
}FIELD_MENU_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
//�A�C�R���쐬�p���[�N
typedef struct
{
  FIELD_MENU_ITEM_TYPE type;
  u8  cursorPosX;
  u8  cursorPosY;
  u8  strBmpPosX; //���ꂾ���L�����P��
  u8  strBmpPosY;
  u8  iconPosX;
  u8  iconPosY;
  STRBUF *str;
  
  ARCHANDLE *arcHandle; //NULL�Ȃ�A�C�R���Ȃ�
  u32 ncgArcIdx;
  u32 nceArcIdx;
  u32 anmArcIdx;
  
}FIELD_MENU_ICON_INIT;

typedef struct
{
  BOOL isInit;
  GFL_BMPWIN *strBmp;
  FIELD_MENU_ITEM_TYPE type;
  u32   objResChr;
  u32   objResAnm;
  u8    posX;
  u8    posY;
  GFL_CLWK  *cellIcon;
}FIELD_MENU_ICON;

struct _FIELD_MENU_WORK
{
  HEAPID heapId;
  FIELDMAP_WORK *fieldWork;
  FIELD_SUBSCREEN_WORK *subScrWork;
  FIELD_MENU_STATE state;
  u16 zoneId;

  u8  cursorPosX; //0��1
  u8  cursorPosY; //0�`3(3�͖߂�)
  u8  waitCnt;
  BOOL isUpdateCursor;
  BOOL isCancel;
  BOOL isDispCursor;
  FIELD_MENU_ICON icon[FIELD_MENU_ITEM_NUM];
  FIELD_MENU_ICON *activeIcon; //�Q�ƒ��̃A�C�R��
  
  s16 scrollOffset;
  BOOL isUpdateScroll;
  
  u32 objRes[FIELD_MENU_OBJ_RES_NUM];
  GFL_CLUNIT *cellUnit;
  GFL_CLSYS_REND *cellRender;
  GFL_CLWK  *cellCursor;
  PRINT_QUE *printQue;
  
  GFL_TCB *vBlankTcb;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void FIELD_MENU_InitGraphic(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle );
static void FIELD_MENU_InitIcon( FIELD_MENU_WORK* work , ARCHANDLE *arcHandle );

static void FIELD_MENU_InitScrollIn( FIELD_MENU_WORK* work );
static void FIELD_MENU_VBlankTCB( GFL_TCB *tcb , void *userWork );

static void  FIELD_MENU_UpdateKey( FIELD_MENU_WORK* work );
static void  FIELD_MENU_UpdateTP( FIELD_MENU_WORK* work );
static void  FIELD_MENU_UpdateCursor( FIELD_MENU_WORK* work );

static void FIELD_MENU_Icon_CreateIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon , const FIELD_MENU_ICON_INIT *initWork );
static void FIELD_MENU_Icon_DeleteIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon ); 
static void FIELD_MENU_Icon_TransBmp( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon );

static const u16 FIELD_MENU_ITEM_MSG_ARR[FMIT_MAX] =
{
  FLDMAPMENU_STR_ITEM07,
  FLDMAPMENU_STR_ITEM02,
  FLDMAPMENU_STR_ITEM01,
  FLDMAPMENU_STR_ITEM03,
  FLDMAPMENU_STR_ITEM04,
  FLDMAPMENU_STR_ITEM05,
  FLDMAPMENU_STR_ITEM06,
};
static const u32 FIELD_MENU_ITEM_ICON_RES_ARR[FMIT_MAX][3] =
{
  //�_�~�[
  { NARC_field_menu_menu_obj_common_NCGR, 
    NARC_field_menu_menu_obj_common_NCER,
    NARC_field_menu_menu_obj_common_NANR},

  { NARC_field_menu_menu_icon_poke_NCGR,
    NARC_field_menu_menu_icon_poke_NCER,
    NARC_field_menu_menu_icon_poke_NANR},
  { NARC_field_menu_menu_icon_zukan_NCGR,
    NARC_field_menu_menu_icon_zukan_NCER,
    NARC_field_menu_menu_icon_zukan_NANR},
  { NARC_field_menu_menu_icon_itembag_NCGR,
    NARC_field_menu_menu_icon_itembag_NCER,
    NARC_field_menu_menu_icon_itembag_NANR},
  { NARC_field_menu_menu_icon_tcard_NCGR,
    NARC_field_menu_menu_icon_tcard_NCER,
    NARC_field_menu_menu_icon_tcard_NANR},
  { NARC_field_menu_menu_icon_report_NCGR,
    NARC_field_menu_menu_icon_report_NCER,
    NARC_field_menu_menu_icon_report_NANR},
  { NARC_field_menu_menu_icon_confing_NCGR,
    NARC_field_menu_menu_icon_confing_NCER,
    NARC_field_menu_menu_icon_confing_NANR},
};

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
FIELD_MENU_WORK* FIELD_MENU_InitMenu( const HEAPID heapId , FIELD_SUBSCREEN_WORK* subScreenWork , FIELDMAP_WORK *fieldWork , const BOOL isScrollIn )
{
  FIELD_MENU_WORK *work = GFL_HEAP_AllocMemory( heapId , sizeof( FIELD_MENU_WORK ) );
  ARCHANDLE *arcHandle;
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
  work->heapId = heapId;
  work->fieldWork = fieldWork;
  work->subScrWork = subScreenWork;
  work->state = FMS_WAIT_INIT;
  work->zoneId = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gameSys) );
  
  work->cursorPosX = 0;
  work->cursorPosY = 0;
  work->isUpdateCursor = FALSE;
  work->isDispCursor = FALSE;
  work->activeIcon = NULL;
  work->vBlankTcb = GFUser_VIntr_CreateTCB( FIELD_MENU_VBlankTCB , (void*)work , 0 );
  
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
  work->isUpdateScroll = FALSE;
  work->scrollOffset = 0;
  
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_FIELD_MENU , work->heapId );
  FIELD_MENU_InitGraphic( work , arcHandle);
  FIELD_MENU_InitIcon( work , arcHandle);
  GFL_ARC_CloseDataHandle(arcHandle);

  G2_SetBlendBrightnessExt( GX_BLEND_PLANEMASK_BG0 , GX_BLEND_PLANEMASK_NONE , 
                            0 , 0 , -6 );
  
  if( isScrollIn == TRUE )
  {
    work->cursorPosX = 0;
    work->cursorPosY = 0;
    work->isUpdateCursor = TRUE;
    FIELD_MENU_UpdateCursor( work );
    FIELD_MENU_InitScrollIn( work );
  }
  else
  {
    FIELD_MENU_SetMenuItemNo( work , GAMEDATA_GetSubScreenType( gameData ) );
  }
  
  return work;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
void FIELD_MENU_ExitMenu( FIELD_MENU_WORK* work )
{
  u8 i;
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    FIELD_MENU_Icon_DeleteIcon( work , &work->icon[i] );
  }
  G2_SetBlendBrightnessExt( GX_BLEND_PLANEMASK_BG0 , GX_BLEND_PLANEMASK_NONE , 
                            0 , 0 , 0 );
  FLDMSGBG_SetBlendAlpha();
  GFL_TCB_DeleteTask( work->vBlankTcb );
  
  GFL_CLACT_WK_Remove( work->cellCursor );
  GFL_CLACT_USERREND_Delete( work->cellRender );
  GFL_CLACT_UNIT_Delete(work->cellUnit);
  GFL_CLGRP_CELLANIM_Release(work->objRes[FMO_COM_ANM]);
  GFL_CLGRP_CGR_Release(work->objRes[FMO_COM_CHR]);
  GFL_CLGRP_PLTT_Release(work->objRes[FMO_COM_PLT]);
  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , 0 );
  GFL_BG_FreeBGControl(FIELD_MENU_BG_BACK);
  GFL_BG_FreeBGControl(FIELD_MENU_BG_NAME);
  PRINTSYS_QUE_Delete( work->printQue ); 
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
void FIELD_MENU_UpdateMenu( FIELD_MENU_WORK* work )
{
  switch( work->state )
  {
  case FMS_WAIT_INIT:
    if( PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE )
    {
      u8 i;
      for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
      {
        FIELD_MENU_Icon_TransBmp( work , &work->icon[i] );
      }
      if( work->scrollOffset != 0 )
      {
        work->state = FMS_WAIT_MOVEIN;
      }
      else
      {
        work->state = FMS_LOOP;
      }
      PMSND_PlaySystemSE( SEQ_SE_OPEN1 );
    }
    break;
    
  case FMS_WAIT_MOVEIN:
    if( work->scrollOffset < FIELD_MENU_SCROLL_SPD )
    {
      work->scrollOffset = 0;
      work->state = FMS_LOOP;
    }
    else
    {
      work->scrollOffset -= FIELD_MENU_SCROLL_SPD;
    }
    work->isUpdateScroll = TRUE;
    break;

  case FMS_LOOP:
    FIELD_MENU_UpdateKey( work );
    FIELD_MENU_UpdateTP( work );
    FIELD_MENU_UpdateCursor( work );
    break;
   
  case FMS_EXIT_INIT:
    if( work->isCancel == TRUE ||
        work->cursorPosY == 3 )
    {
      //work->state = FMS_EXIT_ENDMENU;
      work->state = FMS_WAIT_MOVEOUT;
      PMSND_PlaySystemSE( SEQ_SE_CLOSE1 );
    }
    else
    {
      if( work->activeIcon->cellIcon != NULL )
      {
        work->waitCnt = 0;
        GFL_CLACT_WK_SetAnmSeq( work->activeIcon->cellIcon , FIA_DECIDE );
        work->state = FMS_WAIT_ICON_ANIME;
        PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
      }
      else
      {
        work->state = FMS_EXIT_DECIDEITEM;
      }
    }
    break;
    
  case FMS_WAIT_ICON_ANIME:
    work->waitCnt++;
    if( work->waitCnt > FIELD_MENU_ICON_DECIDE_ANIME_CNT )
    {
      work->state = FMS_EXIT_DECIDEITEM;
    }
    break;
  case FMS_EXIT_ENDMENU:
    FIELD_SUBSCREEN_SetAction( work->subScrWork , FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT );
    work->state = FMS_FINISH;
    break;
    
  case FMS_EXIT_DECIDEITEM:
    FIELD_SUBSCREEN_SetAction( work->subScrWork , FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE );
    work->state = FMS_FINISH;
    break;
  case FMS_WAIT_MOVEOUT:
    if( work->scrollOffset + FIELD_MENU_SCROLL_SPD > 192 )
    {
      work->scrollOffset = 192;
      work->state = FMS_EXIT_ENDMENU;
    }
    else
    {
      work->scrollOffset += FIELD_MENU_SCROLL_SPD;
    }
    work->isUpdateScroll = TRUE;
  }
  PRINTSYS_QUE_Main( work->printQue );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
void FIELD_MENU_DrawMenu( FIELD_MENU_WORK* work )
{
  if( work->isUpdateScroll == TRUE )
  {
    u8 i;
    GFL_CLACTPOS sufacePos;

    GFL_BG_SetScrollReq( FIELD_MENU_BG_BACK , GFL_BG_SCROLL_Y_SET , -work->scrollOffset );
    GFL_BG_SetScrollReq( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -work->scrollOffset -4 );
    
    sufacePos.x = 0;
    sufacePos.y = -work->scrollOffset;
    GFL_CLACT_USERREND_SetSurfacePos( work->cellRender , FIELD_MENU_RENDER_SURFACE , &sufacePos );
    //�A�C�R���̏���
    for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
    {
      if( work->icon[i].cellIcon != NULL )
      {
        GFL_CLACTPOS cellPos;
        GFL_CLACT_WK_GetPos( work->icon[i].cellIcon , &cellPos , FIELD_MENU_RENDER_SURFACE );
        if( cellPos.y < 192 + FIELD_MENU_ICON_SIZE_Y/2 )
        {
          GFL_CLACT_WK_SetDrawEnable( work->icon[i].cellIcon, TRUE );
        }
        else
        {
          GFL_CLACT_WK_SetDrawEnable( work->icon[i].cellIcon, FALSE );
        }
      }
    }
    //�J�[�\���̏���
    {
      GFL_CLACTPOS cellPos;
      GFL_CLACT_WK_GetPos( work->cellCursor , &cellPos , FIELD_MENU_RENDER_SURFACE );
      if( work->isDispCursor &&
          cellPos.y < 192 + FIELD_MENU_CURSOR_SIZE_Y/2 )
      {
        GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
      }
      else
      {
        GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );
      }
    }
    
    work->isUpdateScroll = FALSE;
  }
}

//--------------------------------------------------------------
//  �O���t�B�b�N�n������
//--------------------------------------------------------------
static void FIELD_MENU_InitGraphic(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle )
{
  static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
  0, 0, 0x1000, 0,
  GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x5800,
  GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
  };
  static const GFL_BG_BGCNT_HEADER bgCont_BackStr = {
  0, 0, 0x1000, 0,
  GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x5800,
  GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
  };

  GXS_SetMasterBrightness(-16);

  GFL_BG_SetBGControl( FIELD_MENU_BG_BACK, &bgCont_BackGround, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_MENU_BG_BACK, VISIBLE_ON );
  GFL_BG_ClearScreen( FIELD_MENU_BG_BACK );
  
  GFL_BG_SetBGControl( FIELD_MENU_BG_NAME, &bgCont_BackStr, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_MENU_BG_NAME, VISIBLE_ON );
  GFL_BG_ClearScreen( FIELD_MENU_BG_NAME );
  //�t�H���g�����炵���ʒu�ɏo�������̂�
  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -4 );
  
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_field_menu_menu_bg_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_field_menu_menu_bg_NCGR ,
                    FIELD_MENU_BG_BACK , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_field_menu_menu_bg_NSCR , 
                    FIELD_MENU_BG_BACK ,  0 , 0, FALSE , work->heapId );

  work->objRes[FMO_COM_PLT] = GFL_CLGRP_PLTT_Register( arcHandle , 
                                  NARC_field_menu_menu_obj_common_NCLR ,
                                  CLSYS_DRAW_SUB , 0 , work->heapId );
  
  work->objRes[FMO_COM_CHR] = GFL_CLGRP_CGR_Register( arcHandle , 
                                  NARC_field_menu_menu_obj_common_NCGR ,
                                  FALSE , CLSYS_DRAW_SUB , work->heapId );
  
  work->objRes[FMO_COM_ANM] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
                                  NARC_field_menu_menu_obj_common_NCER ,
                                  NARC_field_menu_menu_obj_common_NANR ,
                                  work->heapId );
  
  GFL_BG_LoadScreenReq( FIELD_MENU_BG_BACK );
  
  //�t�H���g�p�p���b�g
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , FIELD_MENU_PLT_FONT * 32, 16*2, work->heapId );
  
  //�Z���n�V�X�e���̍쐬
  work->cellUnit = GFL_CLACT_UNIT_Create( 7 , 0 , work->heapId );
  
  //�����_���[�쐬
  {
    const GFL_REND_SURFACE_INIT renderInitData = { 0,0,256,191,CLSYS_DRAW_SUB};
    
    work->cellRender = GFL_CLACT_USERREND_Create( &renderInitData , 1 , work->heapId );
    GFL_CLACT_UNIT_SetUserRend( work->cellUnit , work->cellRender );
  }
  
  {
    GFL_CLWK_DATA cellInitData;
    //�Z���̐���
    cellInitData.pos_x =  64;
    cellInitData.pos_y =  44;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    //�����
    work->cellCursor = GFL_CLACT_WK_Create( work->cellUnit ,
                          work->objRes[FMO_COM_CHR],
                          work->objRes[FMO_COM_PLT],
                          work->objRes[FMO_COM_ANM],
                          &cellInitData , 
                          FIELD_MENU_RENDER_SURFACE , 
                          work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor, FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor, TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );
    
  }
}

static void FIELD_MENU_InitIcon(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle )
{
  //TODO �ꏊ�ɂ����Ȃǂ��ϓ����邱�Ƃ�z��
  u8 i;
  u8 menuType;
  //BmpWin�̈ʒu(���ɔ��L�������炵�Ďg��
  static const u8 bmpState[7][2] =
  {
    {  6, 4 },
    { 22, 4 },
    {  6,10 },
    { 22,10 },
    {  6,16 },
    { 22,16 },
    { 13,21 },
  };

  static const FIELD_MENU_ITEM_TYPE typeArr[2][7] =
  {
    { //�ʏ�
      FMIT_POKEMON,
      FMIT_ZUKAN,
      FMIT_ITEMMENU,
      FMIT_TRAINERCARD,
      FMIT_REPORT,
      FMIT_CONFING,
      FMIT_EXIT,
    },
    { //���j�I��
      FMIT_POKEMON,
      FMIT_ZUKAN,
      FMIT_ITEMMENU,
      FMIT_TRAINERCARD,
      FMIT_REPORT,
      FMIT_CONFING,
      FMIT_EXIT,
    }
  };

  GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , 
                                           NARC_message_fldmapmenu_dat , work->heapId );
  //���j���[�̎�ނ̃`�F�b�N
  {
    if (ZONEDATA_IsUnionRoom(work->zoneId) || 
        ZONEDATA_IsColosseum(work->zoneId) )
    {
      menuType = 1;
    }
    else
    {
      menuType = 0;
    }
  }
  
  //������̍쐬
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    work->icon[i].isInit = FALSE;
  }
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    const FIELD_MENU_ITEM_TYPE itemType = typeArr[menuType][i];
    FIELD_MENU_ICON_INIT initWork;
    //�\����
    if( itemType == FMIT_TRAINERCARD )
    {
      //�g���[�i�[�J�[�h
      MYSTATUS  *mystatus = SaveData_GetMyStatus( SaveControl_GetPointer() );
      initWork.str = GFL_STR_CreateBuffer( 16 , work->heapId );
      if( MyStatus_CheckNameClear( mystatus ) == FALSE )
      {
        const STRCODE *myname = MyStatus_GetMyName( mystatus );
        GFL_STR_SetStringCode( initWork.str , myname );
      }
      else
      {
        //TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
        u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
        GFL_STR_SetStringCode( initWork.str , tempName );
      }
    }
    else
    {
      initWork.str = GFL_MSG_CreateString( msgHandle , FIELD_MENU_ITEM_MSG_ARR[itemType] );
    }
    
    initWork.type = itemType;
    if( i != 6 )
    {
      //�ʏ�
      initWork.cursorPosX = 64 + (i%2)*128;
      initWork.cursorPosY = 44 + (i/2)*48;
      initWork.iconPosX = initWork.cursorPosX - 32;
      initWork.iconPosY = initWork.cursorPosY;
      initWork.arcHandle = arcHandle;
      initWork.ncgArcIdx = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][0];
      initWork.nceArcIdx = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][1];
      initWork.anmArcIdx = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][2];
    }
    else
    {
      //����
      initWork.cursorPosX = 128;
      initWork.cursorPosY = 180;
      initWork.iconPosX = 0;
      initWork.iconPosY = 0;
      initWork.arcHandle = NULL;
      initWork.ncgArcIdx = 0;
      initWork.nceArcIdx = 0;
      initWork.anmArcIdx = 0;
    }
    initWork.strBmpPosX = bmpState[i][0];
    initWork.strBmpPosY = bmpState[i][1];
    
    FIELD_MENU_Icon_CreateIcon( work , &work->icon[i] , &initWork );
    GFL_STR_DeleteBuffer( initWork.str );
  }
  
  GFL_MSG_Delete( msgHandle );

  GFL_BG_LoadScreenReq( FIELD_MENU_BG_NAME );

}

static void FIELD_MENU_VBlankTCB( GFL_TCB *tcb , void *userWork )
{
  FIELD_MENU_WORK *work = userWork;
}

static void FIELD_MENU_InitScrollIn( FIELD_MENU_WORK* work )
{
  GFL_CLACTPOS sufacePos;
  work->scrollOffset = 192;
  GFL_BG_SetScroll( FIELD_MENU_BG_BACK , GFL_BG_SCROLL_Y_SET , -192 );
  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -192 -4);
  
  sufacePos.x = 0;
  sufacePos.y = 192;
  
  GFL_CLACT_USERREND_SetSurfacePos( work->cellRender , FIELD_MENU_RENDER_SURFACE , &sufacePos );
}


//--------------------------------------------------------------
//  ���荀�ڂ�n��
//--------------------------------------------------------------
const FIELD_MENU_ITEM_TYPE FIELD_MENU_GetMenuItemNo( FIELD_MENU_WORK* work )
{
  /*
  if( work->cursorPosY == 3 )
  {
    return FMIT_EXIT;
  }
  else
  {
    const u8 idx = work->cursorPosX + work->cursorPosY*2;
    return work->icon[idx].type;
  }
  */
  if( work->activeIcon == NULL )
  {
    GF_ASSERT_MSG(0,"FieldMenu ActiveItem is NULL!!\n");
    return FMIT_EXIT;
  }
  else
  {
    return work->activeIcon->type;
  }
}

//--------------------------------------------------------------
//  ���ڏ����ʒu��Ⴄ
//--------------------------------------------------------------
void FIELD_MENU_SetMenuItemNo( FIELD_MENU_WORK* work , const FIELD_MENU_ITEM_TYPE itemType )
{
  u8 i;
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    if( work->icon[i].isInit == TRUE )
    {
      if( work->icon[i].type == itemType )
      {
        work->cursorPosX = i%2;
        work->cursorPosY = i/2;
        work->isUpdateCursor = TRUE;
        FIELD_MENU_UpdateCursor( work );
        
        //FIXME ���̓��j���[�ŉ��̂��̂�����A�ċN����������Ȃ��̂�
        //�X�e�[�^�X�������ɖ߂������őΉ�
        if( work->state == FMS_FINISH )
        {
          work->state = FMS_WAIT_INIT;
        }
      }
    }
  }
}

#pragma mark [> UI Func
static void  FIELD_MENU_UpdateKey( FIELD_MENU_WORK* work )
{
  const int trg = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();

  if( work->isDispCursor == FALSE &&
      trg != 0 )
  {
    if( trg & PAD_BUTTON_B || 
        trg & PAD_BUTTON_X )
    {
      work->isCancel = TRUE;
      work->state = FMS_EXIT_INIT;
    }
    else
    {
      work->isDispCursor = TRUE;
      work->isUpdateCursor = TRUE;
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
    }

    return;
  }
  
  if( repeat & PAD_KEY_UP )
  {
    if( work->cursorPosY > 0 )
    {
      work->cursorPosY--;
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_DOWN )
  {
    if( work->cursorPosY < 3 )
    {
      work->cursorPosY++;
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_LEFT )
  {
    if( work->cursorPosX > 0 && 
        work->cursorPosY < 3)
    {
      work->cursorPosX--;
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_RIGHT )
  {
    if( work->cursorPosX < 1 && 
        work->cursorPosY < 3 )
    {
      work->cursorPosX++;
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( trg & PAD_BUTTON_B || 
      trg & PAD_BUTTON_X )
  {
    work->isCancel = TRUE;
    work->state = FMS_EXIT_INIT;
  }
  else
  if( trg & PAD_BUTTON_A )
  {
    work->isCancel = FALSE;
    work->state = FMS_EXIT_INIT;
  }
}

static void  FIELD_MENU_UpdateTP( FIELD_MENU_WORK* work )
{
  const GFL_UI_TP_HITTBL hitTbl[FIELD_MENU_ITEM_NUM+1] =
  {
    { 24, 64 ,  8,120},
    { 24, 64 ,136,248},
    { 72,112 ,  8,120},
    { 72,112 ,136,248},
    {120,160 ,  8,120},
    {120,160 ,136,248},
    {168,192 ,  0,255},
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  
  const int ret = GFL_UI_TP_HitTrg( hitTbl );
  
  if( ret != GFL_UI_TP_HIT_NONE )
  {
    work->cursorPosX = ret%2;
    work->cursorPosY = ret/2;
    work->isUpdateCursor = TRUE;
    work->isDispCursor = FALSE;
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );
    work->isCancel = FALSE;
    work->state = FMS_EXIT_INIT;
  }
}

static void  FIELD_MENU_UpdateCursor( FIELD_MENU_WORK* work )
{
  if( work->isUpdateCursor == TRUE )
  {
    FIELD_MENU_ICON *prevIcon = work->activeIcon;
    GFL_CLACTPOS pos;
    if( work->cursorPosY < 3 )
    {
      //�߂�ȊO
      work->activeIcon = &work->icon[work->cursorPosX + work->cursorPosY*2];

      GFL_CLACT_WK_SetAnmSeq( work->cellCursor , FCA_NORMAL_BIG );
    }
    else
    {
      //�߂�
      work->activeIcon = &work->icon[6];
      GFL_CLACT_WK_SetAnmSeq( work->cellCursor , FCA_NORMAL_SMALL );
    }
    
    pos.x = work->activeIcon->posX;
    pos.y = work->activeIcon->posY;
    GFL_CLACT_WK_SetPos( work->cellCursor , &pos , FIELD_MENU_RENDER_SURFACE );
    if( work->activeIcon->cellIcon != NULL &&
        work->isDispCursor == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->activeIcon->cellIcon , FIA_ACTIVE );
    }
    //�O��̃A�C�R���̃A�j����߂�
    if( prevIcon != NULL &&
        prevIcon->cellIcon != NULL )
    {
      GFL_CLACT_WK_SetAnmSeq( prevIcon->cellIcon , FIA_NORMAL );
    }
    
    work->isUpdateCursor = FALSE;
  }
}

#pragma mark [> IconFunc

static void FIELD_MENU_Icon_CreateIcon( FIELD_MENU_WORK* work , 
                                   FIELD_MENU_ICON *icon , 
                                   const FIELD_MENU_ICON_INIT *initWork )
{
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
  GFL_FONT *fontHandle = FLDMSGBG_GetFontHandle( msgBG );

  icon->isInit = TRUE;
  icon->type = initWork->type;
  icon->posX = initWork->cursorPosX;
  icon->posY = initWork->cursorPosY;
  icon->strBmp = GFL_BMPWIN_Create( FIELD_MENU_BG_NAME , 
                        initWork->strBmpPosX , initWork->strBmpPosY , 8 , 2, 
                        FIELD_MENU_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( icon->strBmp );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(icon->strBmp), 0x00 );
  
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( icon->strBmp ) , 2 , 2 ,
                          initWork->str , fontHandle , PRINTSYS_LSB_Make( 1,2,0 ) );
  if( initWork->arcHandle != NULL )
  {
    GFL_CLWK_AFFINEDATA cellInitData;
    icon->objResChr = GFL_CLGRP_CGR_Register( initWork->arcHandle , initWork->ncgArcIdx ,
                           FALSE , CLSYS_DRAW_SUB , work->heapId );
    
    icon->objResAnm = GFL_CLGRP_CELLANIM_Register( initWork->arcHandle , 
                           initWork->nceArcIdx , initWork->anmArcIdx ,work->heapId );
    //�Z���̐���  //�{�p���g���Ă���̂�Affine��
    cellInitData.clwkdata.pos_x = initWork->iconPosX;
    cellInitData.clwkdata.pos_y = initWork->iconPosY;
    cellInitData.clwkdata.anmseq = 0;
    cellInitData.clwkdata.softpri = 0;
    cellInitData.clwkdata.bgpri = 0;
    cellInitData.affinepos_x = 0;
    cellInitData.affinepos_y = 0;
    cellInitData.scale_x = FX32_ONE;
    cellInitData.scale_y = FX32_ONE;
    cellInitData.rotation = 0;
    cellInitData.affine_type = CLSYS_AFFINETYPE_DOUBLE;
    icon->cellIcon = GFL_CLACT_WK_CreateAffine( work->cellUnit ,
                          icon->objResChr,
                          work->objRes[FMO_COM_PLT],
                          icon->objResAnm,
                          &cellInitData , 
                          FIELD_MENU_RENDER_SURFACE , 
                          work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( icon->cellIcon, FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( icon->cellIcon, TRUE );
    GFL_CLACT_WK_SetAnmSeq( icon->cellIcon , 0 );
//    GFL_CLACT_WK_SetAffineParam( icon->cellIcon , CLSYS_AFFINETYPE_NORMAL );

    GFL_CLACT_WK_SetDrawEnable( icon->cellIcon, TRUE );
    
  }
  else
  {
    icon->cellIcon = NULL;
  }
}

static void FIELD_MENU_Icon_DeleteIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon )
{
  if( icon->isInit == TRUE )
  {
    GFL_BMPWIN_Delete( icon->strBmp );
    if( icon->cellIcon != NULL )
    {
      GFL_CLACT_WK_Remove( icon->cellIcon );
      GFL_CLGRP_CELLANIM_Release(icon->objResAnm);
      GFL_CLGRP_CGR_Release(icon->objResChr);
    }
  }
}

static void FIELD_MENU_Icon_TransBmp( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon )
{
  if( icon->isInit == TRUE )
  {
    GFL_BMPWIN_TransVramCharacter( icon->strBmp );
  }
}

