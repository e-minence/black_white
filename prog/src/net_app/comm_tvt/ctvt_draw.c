//======================================================================
/**
 * @file	ctvt_talk.c
 * @brief	通信TVTシステム：落書き
 * @author	ariizumi
 * @data	09/12/19
 *
 * モジュール名：CTVT_DRAW
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/camera_system.h"
#include "system/bmp_winframe.h"
#include "net/network_define.h"
#include "app/app_menu_common.h"

#include "msg/msg_comm_tvt.h"

#include "comm_tvt.naix"

#include "ctvt_draw.h"
#include "ctvt_comm.h"
#include "ctvt_camera.h"
#include "ctvt_snd_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define CTVT_DRAW_BAR_ICON_Y (192-12)
#define CTVT_DRAW_PEN_X (16)
#define CTVT_DRAW_SPOITO_X (56)
#define CTVT_DRAW_KESHIGOMU_X (96)
#define CTVT_DRAW_STAMP_X (136)
#define CTVT_DRAW_PAUSE_X (200)
#define CTVT_DRAW_RETURN_X (224)

#define CTVT_DRAW_EXP_ICON_X      (56)
#define CTVT_DRAW_EXP_PEN_Y       ( 72)
#define CTVT_DRAW_EXP_SPOITO_Y    ( 92)
#define CTVT_DRAW_EXP_KESHIGOMU_Y (112)
#define CTVT_DRAW_EXP_STAMP_Y     (132)
#define CTVT_DRAW_EXP_RETURN_Y    (152-12)

#define CTVT_DRAW_PEN_SIZE_X (CTVT_DRAW_PEN_X)
#define CTVT_DRAW_PEN_SIZE_Y (192-32)
#define CTVT_DRAW_STAMP_TYPE_X (CTVT_DRAW_STAMP_X)
#define CTVT_DRAW_STAMP_TYPE_Y (192-32)

#define CTVT_DRAW_PALANM_SPD (0x400)

//文字位置
#define CTVT_DRAW_INFO_DRAW_X1 (8)
#define CTVT_DRAW_INFO_DRAW_X2 (80)

#define CTVT_DRAW_INFO_EDIT_X (64)

#define CTVT_DRAW_USE_SAMPLING (0)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//編集ボタン種類
typedef enum
{
  CDED_PEN,
  CDED_SPOITO,
  CDED_KESHIGOMU,
  CDED_STAMP,
  CDED_PAUSE,
  CDED_RETURN,

  CDED_MAX,
}CTVT_DRAW_EDTI_BUTTON_TYPE;
//説明ボタン種類
typedef enum
{
  CDEX_PEN,
  CDEX_SPOITO,
  CDEX_KESHIGOMU,
  CDEX_STAMP,
  CDEX_RETURN,

  CDEX_MAX,
}CTVT_DRAW_EXPLAIN_BUTTON_TYPE;

//ステート
typedef enum
{
  CDS_FADEIN,
  CDS_FADEIN_WAIT,

  CDS_FADEOUT, //両方フェード
  CDS_FADEOUT_WAIT,

  CDS_DRAW,
  CDS_EDIT,
  
  //親のリクエストによる終了
  CDS_END_PARENT_REQ_INIT,
  CDS_END_PARENT_REQ,

  //WIFIのリクエストによる終了
  CDS_END_WIFI_REQ_SEND,
  CDS_END_WIFI_REQ_INIT_DISP,
  CDS_END_WIFI_REQ_DISP,
  CDS_END_WIFI_REQ_INIT,
  CDS_END_WIFI_REQ,

  //一人になったので終了
  CDS_END_MEMBER_NONE_INIT,
  CDS_END_MEMBER_NONE,  
  
}CTVT_DRAW_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//ワーク
struct _CTVT_DRAW_WORK
{
  CTVT_DRAW_STATE state;
  
  u8 barScroll;     //目標座標
  u8 dispBarScroll; //実座標
  
  BOOL isTouch; //落書ききのTrgをはじくフラグ
  BOOL isHold;
  BOOL isDispPenSize;
  BOOL isDispStampType;
  BOOL canDraw;
  BOOL isFinish;
  BOOL reqStopCamera;
  BOOL isBefStamp;
  CTVT_DRAW_EDTI_BUTTON_TYPE editMode;

  u32 befPenX;
  u32 befPenY;
  
  u16 penCol;
  u8  penSize;
  u8  stampType;

#if CTVT_DRAW_USE_SAMPLING
  u32 samplingRes;
  TP_ONE_DATA	tpData;
#endif //CTVT_DRAW_USE_SAMPLING

  
  BOOL isUpdateInfo;    //上画面用
  BOOL isUpdateInfoEdit;//上画面用の更新がEditモードか？
  BOOL isUpdateTitle;    //上画面用
  BOOL isUpdateMsgWin;  //下画面用
  GFL_BMPWIN *infoWin;  //上画面用
  GFL_BMPWIN *titleWin; //上画面用
  GFL_BMPWIN *msgWin;   //下画面用
  u16        wifiExitCnt;  //Wifiの終了リクエストの自動切断

  //セル関係
  GFL_CLWK    *clwkEditButton[CDED_MAX];
  GFL_CLWK    *clwkExplainButton[CDEX_MAX];
  GFL_CLWK    *clwkPenSize;
  GFL_CLWK    *clwkStampType;
  GFL_CLWK    *clwkBlinkButton;
  GFL_CLWK    *clwkSelectButton;
  
  u16 palColBuf;
  u16 palCalAnm;
  
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void CTVT_DRAW_UpdateDraw( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );
static void CTVT_DRAW_UpdateEdit( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );

static void CTVT_DRAW_UpdateBar( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );
static void CTVT_DRAW_UpdateDrawing( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );

static void CTVT_DRAW_UpdateEditButton( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );

static void CTVT_DRAW_DrawInfoMsg( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork , const BOOL isEditMode );

static const BOOL CTVT_DRAW_CheckFinishReq( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );
static void CTVT_DRAW_DispMessage( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork , const u16 msgId );

static void CTVT_DRAW_EreseSubWindow( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork );

//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
CTVT_DRAW_WORK* CTVT_DRAW_InitSystem( COMM_TVT_WORK *work , const HEAPID heapId )
{
  u8 i;
  CTVT_DRAW_WORK* drawWork = GFL_HEAP_AllocClearMemory( heapId , sizeof(CTVT_DRAW_WORK) );

  drawWork->penSize = DSPS_CIRCLE_1;
  drawWork->stampType = DSPS_STAMP_HEART;
  drawWork->editMode = CDED_PEN;
  drawWork->isBefStamp = FALSE;

  //バージョンで白黒を変える
  if( GET_VERSION() == VERSION_BLACK )
  {
    drawWork->penCol = 0x0000+0x8000;  //最上位ビットがα
  }
  else
  {
    drawWork->penCol = 0x7FFF+0x8000;  //最上位ビットがα
  }
  return drawWork;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
void CTVT_DRAW_TermSystem( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  GFL_HEAP_FreeMemory( drawWork );
}
//--------------------------------------------------------------
//	モード切替時初期化
//--------------------------------------------------------------
void CTVT_DRAW_InitMode( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  ARCHANDLE* arcHandle = COMM_TVT_GetArcHandle( work );

  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_comm_tvt_tv_t_paint_bg_NSCR , 
                    CTVT_FRAME_SUB_BG ,  0 , 0, FALSE , heapId );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_BG );
  GFL_BG_SetVisible( CTVT_FRAME_SUB_BAR , FALSE );
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

  {
    //MSGと共用なのでここでスクリーンを読む
    ARCHANDLE *commonArcHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , heapId );
    GFL_BG_ClearScreen( CTVT_FRAME_MAIN_MSG );
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( commonArcHandle , APP_COMMON_GetBarScrnArcIdx() , 
                      CTVT_FRAME_MAIN_MSG , 32*3 , 0 , 0x2*32*24, FALSE , heapId );
    GFL_BG_ChangeScreenPalette( CTVT_FRAME_MAIN_MSG , 0 , 21 , 32 , 3 , CTVT_PAL_BG_MAIN_BAR );
    GFL_BG_LoadScreenReq( CTVT_FRAME_MAIN_MSG );
    
    GFL_BG_SetScroll( CTVT_FRAME_MAIN_MSG , GFL_BG_SCROLL_Y_SET , 0 );

    GFL_ARC_CloseDataHandle( commonArcHandle );
  }
  {
    u8 i;
    //編集ボタン
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = CTVT_DRAW_PEN_X;
    cellInitData.pos_y = CTVT_DRAW_BAR_ICON_Y+24;
    cellInitData.anmseq = CTOAM_PEN;
    cellInitData.softpri = 8;
    cellInitData.bgpri = 0;
    
    drawWork->clwkEditButton[CDED_PEN] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    
    cellInitData.pos_x = CTVT_DRAW_SPOITO_X;
    cellInitData.anmseq = CTOAM_SUPOITO;
    drawWork->clwkEditButton[CDED_SPOITO] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    
    cellInitData.pos_x = CTVT_DRAW_KESHIGOMU_X;
    cellInitData.anmseq = CTOAM_KESHIGOMU;
    drawWork->clwkEditButton[CDED_KESHIGOMU] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    
    cellInitData.pos_x = CTVT_DRAW_STAMP_X;
    cellInitData.anmseq = CTOAM_STAMP;
    drawWork->clwkEditButton[CDED_STAMP] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    
    cellInitData.pos_x = CTVT_DRAW_PAUSE_X;
    if( COMM_TVT_GetPause( work ) == TRUE )
    {
      cellInitData.anmseq = CTOAM_PAUSE;
    }
    else
    {
      cellInitData.anmseq = CTOAM_PLAY;
    }
    drawWork->clwkEditButton[CDED_PAUSE] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );

    cellInitData.pos_x = CTVT_DRAW_RETURN_X;
    cellInitData.pos_y = CTVT_DRAW_BAR_ICON_Y-12+24;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    drawWork->clwkEditButton[CDED_RETURN] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkEditButton[CDED_RETURN] , TRUE );
    for( i=0;i<CDED_MAX;i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkEditButton[i] , TRUE );
    }
    
    //説明ボタン
    cellInitData.pos_x = CTVT_DRAW_EXP_ICON_X;
    cellInitData.pos_y = CTVT_DRAW_EXP_PEN_Y;
    cellInitData.anmseq = CTOAS_PEN;
    drawWork->clwkExplainButton[CDEX_PEN] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    
    cellInitData.pos_y = CTVT_DRAW_EXP_SPOITO_Y;
    cellInitData.anmseq = CTOAS_SUPOITO;
    drawWork->clwkExplainButton[CDEX_SPOITO] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    
    cellInitData.pos_y = CTVT_DRAW_EXP_KESHIGOMU_Y;
    cellInitData.anmseq = CTOAS_KESHIGOMU;
    drawWork->clwkExplainButton[CDEX_KESHIGOMU] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    
    cellInitData.pos_y = CTVT_DRAW_EXP_STAMP_Y;
    cellInitData.anmseq = CTOAS_STAMP;
    drawWork->clwkExplainButton[CDED_STAMP] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    
    cellInitData.pos_x = CTVT_DRAW_EXP_ICON_X-12;
    cellInitData.pos_y = CTVT_DRAW_EXP_RETURN_Y;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    drawWork->clwkExplainButton[CDEX_RETURN] = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    for( i=0;i<CDEX_MAX;i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkExplainButton[i] , FALSE );
    }
    
    //ペンサイズ
    cellInitData.pos_x = CTVT_DRAW_PEN_SIZE_X;
    cellInitData.pos_y = CTVT_DRAW_PEN_SIZE_Y;
    cellInitData.anmseq = CTOAM_PEN_SELECT_ANIME;
    drawWork->clwkPenSize = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkPenSize , FALSE );

    //スタンプ種類
    cellInitData.pos_x = CTVT_DRAW_STAMP_TYPE_X;
    cellInitData.pos_y = CTVT_DRAW_STAMP_TYPE_Y;
    cellInitData.anmseq = CTOAM_STAMP_SELECT_ANIME;
    drawWork->clwkStampType = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkStampType , FALSE );
    
    //点滅ボタン
    cellInitData.softpri = 0;
    drawWork->clwkBlinkButton = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkBlinkButton , FALSE );
    
    //選択中
    cellInitData.anmseq = CTOAM_BUTTON_SELECT;
    cellInitData.softpri = 2;
    drawWork->clwkSelectButton = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_M_ANM ),
              &cellInitData ,CLSYS_DRAW_MAIN , heapId );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );
    
  }
  CTVT_DRAW_UpdateEditButton( work , drawWork );
  
  drawWork->infoWin = NULL;

  //MakeScreenは文字表示の時
  //Createは文字描画の時
  drawWork->msgWin = NULL;
  
  {
    GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
    GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    STRBUF *str;
    drawWork->titleWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                          3 , 1 , 28 , 2 ,
                                          CTVT_PAL_BG_SUB_FONT ,
                                          GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( drawWork->titleWin) , 0x0 );
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_012 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->titleWin ) , 
            0 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    drawWork->isUpdateTitle = TRUE;
  }
  
  
  drawWork->state = CDS_FADEIN;
  drawWork->barScroll = 24;
  drawWork->dispBarScroll = 24;
  drawWork->isHold = FALSE;
  drawWork->isTouch = FALSE;
  drawWork->isDispPenSize = FALSE;
  drawWork->isDispStampType = FALSE;
  drawWork->isUpdateInfo = FALSE;
  drawWork->isUpdateMsgWin = FALSE;
  drawWork->canDraw = FALSE;
  drawWork->isFinish = FALSE;
  drawWork->reqStopCamera = FALSE;
  drawWork->palCalAnm = 0;
  {
    //ペンの色転送
    u16* spoitColAdr = (u16*)(HW_OBJ_PLTT + 0*32 + 2*0xd);
    *spoitColAdr = (drawWork->penCol&0x7FFF);
  }

  CTVT_DRAW_DrawInfoMsg( work , drawWork , FALSE );
  
  GFL_NET_ReloadIconTopOrBottom(TRUE , heapId );

  //上画面の会話アイコンを消す
  COMM_TVT_EraseTalkIcon( work );
  
#if CTVT_DRAW_USE_SAMPLING
  GFL_UI_TP_AutoStartNoBuff();
#endif //CTVT_DRAW_USE_SAMPLING
  
}

//--------------------------------------------------------------
//	モード切替時開放
//--------------------------------------------------------------
void CTVT_DRAW_TermMode( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  u8 i;
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  
#if CTVT_DRAW_USE_SAMPLING
  //サンプリング終了
  GFL_UI_TP_AutoStop();
#endif //CTVT_DRAW_USE_SAMPLING

  //スポイトだったら戻す
  if( drawWork->editMode == CDED_SPOITO )
  {
    if( drawWork->isBefStamp == FALSE )
    {
      drawWork->editMode = CDED_PEN;
    }
    else
    {
      drawWork->editMode = CDED_STAMP;
    }
  }


  GFL_NET_ReloadIconTopOrBottom(FALSE , heapId );

  BmpWinFrame_Clear( drawWork->infoWin , WINDOW_TRANS_ON );
  GFL_BMPWIN_ClearTransWindow( drawWork->infoWin );
  GFL_BMPWIN_Delete( drawWork->infoWin );
  GFL_BMPWIN_ClearTransWindow( drawWork->titleWin );
  GFL_BMPWIN_Delete( drawWork->titleWin );
  if( drawWork->msgWin != NULL )
  {
    GFL_BMPWIN_ClearTransWindow( drawWork->msgWin );
    GFL_BMPWIN_Delete( drawWork->msgWin );
    drawWork->msgWin = NULL;
  }
  
  for( i=0;i<CDEX_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( drawWork->clwkExplainButton[i] );
  }
  for( i=0;i<CDED_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( drawWork->clwkEditButton[i] );
  }
  
  GFL_CLACT_WK_Remove( drawWork->clwkSelectButton );
  GFL_CLACT_WK_Remove( drawWork->clwkBlinkButton );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkStampType , FALSE );
  GFL_CLACT_WK_Remove( drawWork->clwkStampType );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkPenSize , FALSE );
  GFL_CLACT_WK_Remove( drawWork->clwkPenSize );

  GFL_BG_ClearScreen( CTVT_FRAME_MAIN_MSG );
  GFL_BG_LoadScreenReq( CTVT_FRAME_MAIN_MSG );
  GFL_BG_SetScroll( CTVT_FRAME_MAIN_MSG , GFL_BG_SCROLL_Y_SET , 0 );

  GFL_BG_ClearScreen( CTVT_FRAME_SUB_MISC );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MISC );
  GFL_BG_SetVisible( CTVT_FRAME_SUB_BAR , TRUE );
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  
  COMM_TVT_RedrawName( work );
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
const COMM_TVT_MODE CTVT_DRAW_Main( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  
  int i;
#if CTVT_DRAW_USE_SAMPLING
  drawWork->samplingRes = GFL_UI_TP_AutoSamplingMain( &drawWork->tpData, TP_BUFFERING_JUST, 1 );
#endif //CTVT_DRAW_USE_SAMPLING
  drawWork->isTouch = FALSE;
  
  switch( drawWork->state )
  {
  case CDS_FADEIN:
    if( COMM_TVT_GetUpperFade( work ) == TRUE )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
              WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    }
    else
    {
      WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
              WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    }
    drawWork->state = CDS_FADEIN_WAIT;
    break;
  case CDS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      drawWork->canDraw = TRUE;
      drawWork->state = CDS_DRAW;
    }
    break;

  case CDS_FADEOUT:
    {
      CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
      if( drawWork->reqStopCamera == FALSE ||
          CTVT_CAMERA_IsStopCapture( work , camWork ) == TRUE )
      {
        WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
        drawWork->state = CDS_FADEOUT_WAIT;
        drawWork->canDraw = FALSE;
      }
    }
    break;

  case CDS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( drawWork->isFinish == FALSE )
      {
        return CTM_TALK;
      }
      else
      {
        return CTM_END;
      }
    }
    break;
    
  case CDS_DRAW:
    if( CTVT_DRAW_CheckFinishReq( work , drawWork ) == FALSE )
    {
      CTVT_DRAW_UpdateDraw( work , drawWork );
    }
    break;

  case CDS_EDIT:
    if( CTVT_DRAW_CheckFinishReq( work , drawWork ) == FALSE )
    {
      CTVT_DRAW_UpdateEdit( work , drawWork );
    }
    break;
    
  //親のリクエストによる終了
  case CDS_END_PARENT_REQ_INIT:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      CTVT_COMM_ExitComm( work , commWork );
      
      CTVT_DRAW_DispMessage( work , drawWork , COMM_TVT_SYS_05 );
      COMM_TVT_SetSusspend( work , TRUE );
      drawWork->state = CDS_END_PARENT_REQ;
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_PlayStopSe(work,camWork);
        drawWork->reqStopCamera = TRUE;
      }
    }
    break;
  case CDS_END_PARENT_REQ:
    if( GFL_UI_TP_GetTrg() == TRUE ||
        GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) )
    {
      drawWork->isFinish = TRUE;
      drawWork->state = CDS_FADEOUT;
      COMM_TVT_SetSusspend( work , TRUE );
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_StopCapture( work , camWork );
        drawWork->reqStopCamera = TRUE;
      }
    }
    break;

  //WIFIのリクエストによる終了
  case CDS_END_WIFI_REQ_SEND:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      const BOOL ret = CTVT_COMM_SendFlg( work , commWork , CCFT_FINISH_PARENT , 0 );
      if( ret == TRUE )
      {
        drawWork->state = CDS_END_WIFI_REQ_INIT_DISP;
      }
    }
    break;
  case CDS_END_WIFI_REQ_INIT_DISP:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      CTVT_COMM_ExitComm( work , commWork );
      
      CTVT_DRAW_DispMessage( work , drawWork , COMM_TVT_SYS_06 );
      COMM_TVT_SetSusspend( work , TRUE );
      drawWork->state = CDS_END_WIFI_REQ_DISP;
      drawWork->wifiExitCnt = 0;
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_PlayStopSe(work,camWork);
        drawWork->reqStopCamera = TRUE;
      }
    }
    break;
  case CDS_END_WIFI_REQ_DISP:
    drawWork->wifiExitCnt++;
    if( GFL_UI_TP_GetTrg() == TRUE ||
        GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ||
        drawWork->wifiExitCnt >= CTVT_WIFI_WIFITCNT )
    {
      drawWork->state = CDS_END_WIFI_REQ_INIT;
    }
    break;
  case CDS_END_WIFI_REQ_INIT:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      CTVT_COMM_SendTimingCommnad( work , commWork , CTVT_COMM_TIMING_END );
      drawWork->state = CDS_END_WIFI_REQ;
    }
    break;
  case CDS_END_WIFI_REQ:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      if( CTVT_COMM_CheckTimingCommnad( work , commWork , CTVT_COMM_TIMING_END ) == TRUE )
      {
        drawWork->isFinish = TRUE;
        drawWork->state = CDS_FADEOUT;
        COMM_TVT_SetSusspend( work , TRUE );
        {
          CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
          CTVT_CAMERA_StopCapture( work , camWork );
          drawWork->reqStopCamera = TRUE;
        }
      }
    }
    break;
  //一人になったので終了
  case CDS_END_MEMBER_NONE_INIT:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      CTVT_COMM_ExitComm( work , commWork );
      
      CTVT_DRAW_DispMessage( work , drawWork , COMM_TVT_SYS_07 );
      COMM_TVT_SetSusspend( work , TRUE );
      drawWork->state = CDS_END_MEMBER_NONE;
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_PlayStopSe(work,camWork);
        drawWork->reqStopCamera = TRUE;
      }
    }
    break;
  case CDS_END_MEMBER_NONE:
    if( GFL_UI_TP_GetTrg() == TRUE ||
        GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) )
    {
      drawWork->isFinish = TRUE;
      drawWork->state = CDS_FADEOUT;
      COMM_TVT_SetSusspend( work , TRUE );
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_StopCapture( work , camWork );
        drawWork->reqStopCamera = TRUE;
      }
    }
    break;
  }
  
  //バースクロール更新
  CTVT_DRAW_UpdateBar( work,drawWork );
  
  //落書き更新
  if( drawWork->canDraw == TRUE )
  {
    CTVT_DRAW_UpdateDrawing( work,drawWork );
  }
  
  if( drawWork->isUpdateInfo == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin )) == FALSE )
    {
      BmpWinFrame_Write( drawWork->infoWin , WINDOW_TRANS_OFF , 
                          CTVT_BMPWIN_CGX , CTVT_PAL_BG_SUB_WINFRAME );
      GFL_BMPWIN_TransVramCharacter( drawWork->infoWin );
      GFL_BMPWIN_MakeScreen( drawWork->infoWin );
      GFL_BG_LoadScreenV_Req(CTVT_FRAME_SUB_MSG);
      drawWork->isUpdateInfo = FALSE;
      
      if( drawWork->isUpdateInfoEdit == TRUE )
      {
        u8 i;
        for( i=0;i<CDEX_MAX;i++ )
        {
          GFL_CLACT_WK_SetDrawEnable( drawWork->clwkExplainButton[i] , TRUE );
        }
      }
      else
      {
        u8 i;
        for( i=0;i<CDEX_MAX;i++ )
        {
          GFL_CLACT_WK_SetDrawEnable( drawWork->clwkExplainButton[i] , FALSE );
        }
      }
    }
  }
  
  if( drawWork->isUpdateTitle == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( drawWork->titleWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( drawWork->titleWin );
      GFL_BMPWIN_MakeScreen( drawWork->titleWin );
      GFL_BG_LoadScreenReq(CTVT_FRAME_SUB_MSG);
      drawWork->isUpdateTitle = FALSE;
    }
  }
  
  if( drawWork->isUpdateMsgWin == TRUE &&
      drawWork->msgWin != NULL )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( drawWork->msgWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( drawWork->msgWin );
      GFL_BMPWIN_MakeScreen( drawWork->msgWin );
      GFL_BG_LoadScreenReq(CTVT_FRAME_MAIN_MSG);
      drawWork->isUpdateMsgWin = FALSE;
    }
  }
  
  //カメラエフェクト
#if (defined(SDK_TWL))
  if( drawWork->state <= CDS_EDIT &&
      COMM_TVT_GetPause(work) == FALSE )
  {
    CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
    {
      CTVT_CAMERA_SetCameraEffect( work , camWork , CAMERA_EFFECT_NONE );
    }
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
    {
      CTVT_CAMERA_SetCameraEffect( work , camWork , CAMERA_EFFECT_SEPIA01 );
    }
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
    {
      CTVT_CAMERA_SetCameraEffect( work , camWork , CAMERA_EFFECT_MONO );
    }
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
    {
      CTVT_CAMERA_SetCameraEffect( work , camWork , CAMERA_EFFECT_NEGATIVE );
    }
  }
#endif
  //拡縮操作
  if( COMM_TVT_GetSelfIdx(work) == 0 &&
      GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
  {
    CTVT_CAMERA_WORK *camWork =  COMM_TVT_GetCameraWork( work );
    if( CTVT_CAMERA_GetWaitAllRefreshFlg( work , camWork ) == FALSE )
    {
      COMM_TVT_FlipDoubleMode( work );
      PMSND_PlaySystemSE( CTVT_SND_TOUCH );
    }
  }

  //パレットアニメ
  
  if( drawWork->palCalAnm + CTVT_DRAW_PALANM_SPD >= 0x10000 )
  {
    drawWork->palCalAnm = drawWork->palCalAnm+CTVT_DRAW_PALANM_SPD-0x10000;
  }
  else
  {
    drawWork->palCalAnm += CTVT_DRAW_PALANM_SPD;
  }
  {
    const u16 colS = 0x739c;
    const u16 colE = 0x4e73;
    const u8 s_r = ( colS & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 s_g = ( colS & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 s_b = ( colS & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    const u8 e_r = ( colE & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    const u8 e_g = ( colE & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    const u8 e_b = ( colE & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    //1〜0に変換
    const fx16 cos = (FX_CosIdx(drawWork->palCalAnm)+FX16_ONE)/2;
    const u8 r = s_r + (((e_r-s_r)*cos)>>FX16_SHIFT);
    const u8 g = s_g + (((e_g-s_g)*cos)>>FX16_SHIFT);
    const u8 b = s_b + (((e_b-s_b)*cos)>>FX16_SHIFT);

    drawWork->palColBuf = GX_RGB(r, g, b);
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_OBJ_PLTT_MAIN ,
                                        6*32+2*15 ,
                                        &drawWork->palColBuf , 2 );

  }

  return CTM_DRAW;
}

//--------------------------------------------------------------
//	お絵書き更新
//  キー操作・モード変更周り
//--------------------------------------------------------------
static void CTVT_DRAW_UpdateDraw( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  if( GFL_UI_KEY_GetTrg() & CTVT_BUTTON_DRAW_EDIT &&
      drawWork->isUpdateInfo == FALSE )
  {
    if( GFL_CLACT_WK_CheckAnmActive( drawWork->clwkEditButton[CDED_RETURN] ) == FALSE ||
        GFL_CLACT_WK_GetAnmSeq( drawWork->clwkEditButton[CDED_RETURN] ) == APP_COMMON_BARICON_RETURN )
    {
      drawWork->state = CDS_EDIT;
      drawWork->barScroll = 0;
      CTVT_DRAW_DrawInfoMsg( work , drawWork , TRUE );
      GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_RETURN] , APP_COMMON_BARICON_RETURN );
      PMSND_PlaySystemSE( CTVT_SND_TOUCH );
    }
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    drawWork->state = CDS_FADEOUT;
    PMSND_PlaySystemSE( CTVT_SND_CANCEL );
  }
  if( GFL_UI_KEY_GetTrg() & CTVT_BUTTON_PAUSE )
  {
    COMM_TVT_FlipPause( work );
    CTVT_DRAW_UpdateEditButton( work , drawWork );
    PMSND_PlaySystemSE( CTVT_SND_PAUSE );
  }
}

//--------------------------------------------------------------
//	編集更新
//--------------------------------------------------------------
static void CTVT_DRAW_UpdateEdit( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  static const GFL_UI_TP_HITTBL hitTbl[CDED_MAX+1] = 
  {
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_PEN_X-12,      CTVT_DRAW_PEN_X+12,
    },
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_SPOITO_X-12,   CTVT_DRAW_SPOITO_X+12,
    },
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_KESHIGOMU_X-12,CTVT_DRAW_KESHIGOMU_X+12,
    },
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_STAMP_X-12,CTVT_DRAW_STAMP_X+12,
    },
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_PAUSE_X-12,    CTVT_DRAW_PAUSE_X+12,
    },
    {
      CTVT_DRAW_BAR_ICON_Y-12, CTVT_DRAW_BAR_ICON_Y+12,
      CTVT_DRAW_RETURN_X,   CTVT_DRAW_RETURN_X+24,
    },
    {GFL_UI_TP_HIT_END,0,0,0}
  };
  
  const int ret = GFL_UI_TP_HitTrg( hitTbl );
  if( ret == CDED_PEN )
  {
    if( GFL_CLACT_WK_GetDrawEnable(drawWork->clwkBlinkButton) == FALSE )
    {
      drawWork->editMode = CDED_PEN;
      drawWork->isBefStamp = FALSE;
      CTVT_DRAW_UpdateEditButton( work , drawWork );
      drawWork->isTouch = TRUE;
      drawWork->isDispPenSize = TRUE;
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkPenSize , TRUE );
      GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkPenSize , TRUE );
      PMSND_PlaySystemSE( CTVT_SND_TOUCH );
      CTVT_TPrintf("PenMode\n");
    }
  }
  if( ret == CDED_SPOITO )
  {
    drawWork->editMode = CDED_SPOITO;
    CTVT_DRAW_UpdateEditButton( work , drawWork );
    drawWork->isTouch = TRUE;
    PMSND_PlaySystemSE( CTVT_SND_TOUCH );
    CTVT_TPrintf("SpoitoMode\n");
  }
  if( ret == CDED_KESHIGOMU )
  {
    drawWork->editMode = CDED_KESHIGOMU;
    CTVT_DRAW_UpdateEditButton( work , drawWork );
    drawWork->isTouch = TRUE;
    PMSND_PlaySystemSE( CTVT_SND_TOUCH );
    CTVT_TPrintf("KeshigomuMode\n");
  }
  if( ret == CDED_STAMP )
  {
    if( GFL_CLACT_WK_GetDrawEnable(drawWork->clwkBlinkButton) == FALSE )
    {
      drawWork->editMode = CDED_STAMP;
      drawWork->isBefStamp = TRUE;
      CTVT_DRAW_UpdateEditButton( work , drawWork );
      drawWork->isTouch = TRUE;
      drawWork->isDispStampType = TRUE;
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkStampType , TRUE );
      GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkStampType , TRUE );
      PMSND_PlaySystemSE( CTVT_SND_TOUCH );
      CTVT_TPrintf("StampMode\n");
    }
  }
  if( ret == CDED_PAUSE )
  {
    COMM_TVT_FlipPause( work );
    CTVT_DRAW_UpdateEditButton( work , drawWork );
    drawWork->isTouch = TRUE;
    PMSND_PlaySystemSE( CTVT_SND_PAUSE );
    CTVT_TPrintf("Pause\n");
  }
  
  //ペンサイズ
  if( drawWork->isDispPenSize == TRUE )
  {
    static const GFL_UI_TP_HITTBL penHitTbl[CDED_MAX+1] = 
    {
      {
        CTVT_DRAW_PEN_SIZE_Y-8, CTVT_DRAW_PEN_SIZE_Y+8,
        CTVT_DRAW_PEN_SIZE_X-8,     CTVT_DRAW_PEN_SIZE_X+8,
      },
      {
        CTVT_DRAW_PEN_SIZE_Y-8, CTVT_DRAW_PEN_SIZE_Y+8,
        CTVT_DRAW_PEN_SIZE_X-8+16,     CTVT_DRAW_PEN_SIZE_X+8+16,
      },
      {
        CTVT_DRAW_PEN_SIZE_Y-8, CTVT_DRAW_PEN_SIZE_Y+8,
        CTVT_DRAW_PEN_SIZE_X-8+32,     CTVT_DRAW_PEN_SIZE_X+8+32,
      },
      {GFL_UI_TP_HIT_END,0,0,0}
    };
    static const penSizeArr[3] = {DSPS_CIRCLE_8,DSPS_CIRCLE_4,DSPS_CIRCLE_1};
    const int retPen = GFL_UI_TP_HitTrg( penHitTbl );
    if( retPen != GFL_UI_TP_HIT_NONE )
    {
      drawWork->penSize = penSizeArr[retPen];
      drawWork->isTouch = TRUE;
      
      //点滅ボタンの表示
      {
        GFL_CLACTPOS pos = {CTVT_DRAW_PEN_SIZE_X,CTVT_DRAW_PEN_SIZE_Y};
        GFL_CLACT_WK_SetDrawEnable( drawWork->clwkBlinkButton , TRUE );
        GFL_CLACT_WK_ResetAnm( drawWork->clwkBlinkButton );
        GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkBlinkButton , TRUE );
        GFL_CLACT_WK_SetAnmSeq( drawWork->clwkBlinkButton , retPen + CTOAM_PEN_DECIDE1);
        GFL_CLACT_WK_SetPos( drawWork->clwkBlinkButton , &pos , CLSYS_DRAW_MAIN );
      }
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );
      
      PMSND_PlaySystemSE( CTVT_SND_TOUCH );
    }
  }
  
  //スタンプタイプ
  if( drawWork->isDispStampType == TRUE )
  {
    static const GFL_UI_TP_HITTBL penHitTbl[CDED_MAX+1] = 
    {
      {
        CTVT_DRAW_STAMP_TYPE_Y-8, CTVT_DRAW_STAMP_TYPE_Y+8,
        CTVT_DRAW_STAMP_TYPE_X-8-32,     CTVT_DRAW_STAMP_TYPE_X+8-32,
      },
      {
        CTVT_DRAW_STAMP_TYPE_Y-8, CTVT_DRAW_STAMP_TYPE_Y+8,
        CTVT_DRAW_STAMP_TYPE_X-8-16,     CTVT_DRAW_STAMP_TYPE_X+8-16,
      },
      {
        CTVT_DRAW_STAMP_TYPE_Y-8, CTVT_DRAW_STAMP_TYPE_Y+8,
        CTVT_DRAW_STAMP_TYPE_X-8,     CTVT_DRAW_STAMP_TYPE_X+8,
      },
      {
        CTVT_DRAW_STAMP_TYPE_Y-8, CTVT_DRAW_STAMP_TYPE_Y+8,
        CTVT_DRAW_STAMP_TYPE_X-8+16,     CTVT_DRAW_STAMP_TYPE_X+8+16,
      },
      {
        CTVT_DRAW_STAMP_TYPE_Y-8, CTVT_DRAW_STAMP_TYPE_Y+8,
        CTVT_DRAW_STAMP_TYPE_X-8+32,     CTVT_DRAW_STAMP_TYPE_X+8+32,
      },
      {GFL_UI_TP_HIT_END,0,0,0}
    };
    static const stampTypeArr[5] = {DSPS_STAMP_HEART,
                                    DSPS_STAMP_BALL,
                                    DSPS_STAMP_PIKATYU,
                                    DSPS_STAMP_STAR,
                                    DSPS_STAMP_DROP};
    const int retPen = GFL_UI_TP_HitTrg( penHitTbl );
    if( retPen != GFL_UI_TP_HIT_NONE )
    {
      drawWork->stampType = stampTypeArr[retPen];
      drawWork->isTouch = TRUE;

      PMSND_PlaySystemSE( CTVT_SND_TOUCH );

      //点滅ボタンの表示
      {
        GFL_CLACTPOS pos = {CTVT_DRAW_STAMP_TYPE_X,CTVT_DRAW_STAMP_TYPE_Y};
        GFL_CLACT_WK_SetDrawEnable( drawWork->clwkBlinkButton , TRUE );
        GFL_CLACT_WK_ResetAnm( drawWork->clwkBlinkButton );
        GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkBlinkButton , TRUE );
        GFL_CLACT_WK_SetAnmSeq( drawWork->clwkBlinkButton , retPen + CTOAM_STAMP_DECIDE1);
        GFL_CLACT_WK_SetPos( drawWork->clwkBlinkButton , &pos , CLSYS_DRAW_MAIN );
      }
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );
    }
  }
  
  if( (GFL_UI_KEY_GetTrg() & CTVT_BUTTON_DRAW_EDIT ||
       GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ||
       ret == CDED_RETURN ) &&
      drawWork->isUpdateInfo == FALSE )
  {
    drawWork->state = CDS_DRAW;
    drawWork->barScroll = 24;
    drawWork->isTouch = TRUE;
    CTVT_DRAW_DrawInfoMsg( work , drawWork , FALSE );
    CTVT_DRAW_EreseSubWindow( work , drawWork );
    PMSND_PlaySystemSE( CTVT_SND_CANCEL );

    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_RETURN] , APP_COMMON_BARICON_RETURN_ON );
  }
  if( GFL_UI_KEY_GetTrg() & CTVT_BUTTON_PAUSE )
  {
    COMM_TVT_FlipPause( work );
    CTVT_DRAW_UpdateEditButton( work , drawWork );
    PMSND_PlaySystemSE( CTVT_SND_PAUSE );
  }
  
  if( GFL_CLACT_WK_GetDrawEnable(drawWork->clwkBlinkButton) == TRUE &&
      GFL_CLACT_WK_CheckAnmActive(drawWork->clwkBlinkButton) == FALSE )
  {
    CTVT_DRAW_EreseSubWindow( work , drawWork );
  }
  
  //選択中表示
  if( drawWork->isDispPenSize == TRUE )
  {
    if( GFL_CLACT_WK_CheckAnmActive(drawWork->clwkPenSize) == FALSE &&
        GFL_CLACT_WK_GetDrawEnable(drawWork->clwkSelectButton) == FALSE )
    {
      GFL_CLACTPOS pos = {CTVT_DRAW_PEN_SIZE_X,CTVT_DRAW_PEN_SIZE_Y};
      switch( drawWork->penSize )
      {
      case DSPS_CIRCLE_8:
        break;
      case DSPS_CIRCLE_4:
        pos.x += 16;
        break;
      case DSPS_CIRCLE_1:
        pos.x += 32;
        break;
      }
      GFL_CLACT_WK_SetPos( drawWork->clwkSelectButton , &pos , CLSYS_DRAW_MAIN );
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , TRUE );
    }
  }
  //選択中表示
  if( drawWork->isDispStampType == TRUE )
  {
    if( GFL_CLACT_WK_CheckAnmActive(drawWork->clwkStampType) == FALSE &&
        GFL_CLACT_WK_GetDrawEnable(drawWork->clwkSelectButton) == FALSE )
    {
      GFL_CLACTPOS pos = {CTVT_DRAW_STAMP_TYPE_X,CTVT_DRAW_STAMP_TYPE_Y};
      switch( drawWork->stampType )
      {
      case DSPS_STAMP_HEART:
        pos.x -= 32;
        break;
      case DSPS_STAMP_BALL:
        pos.x -= 16;
        break;
      case DSPS_STAMP_PIKATYU:
        break;
      case DSPS_STAMP_STAR:
        pos.x += 16;
        break;
      case DSPS_STAMP_DROP:
        pos.x += 32;
        break;
      }
      GFL_CLACT_WK_SetPos( drawWork->clwkSelectButton , &pos , CLSYS_DRAW_MAIN );
      GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , TRUE );
    }
  }
}

//--------------------------------------------------------------
//	落書き更新
//--------------------------------------------------------------
static void CTVT_DRAW_UpdateDrawing( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  u32 tpx,tpy;
  GFL_UI_TP_GetPointTrg( &tpx,&tpy );
  if( COMM_TVT_GetSusspendDraw( work ) == TRUE )
  {
    return;
  }
  
  if( GFL_UI_TP_GetCont() == FALSE )
  {
    drawWork->isHold = FALSE;
  }

  if( GFL_UI_TP_GetTrg() == TRUE &&
      drawWork->isTouch == FALSE &&
      drawWork->state == CDS_EDIT &&
      tpy >= 192-24 )
  {
    if( drawWork->editMode == CDED_SPOITO )
    {
      if( drawWork->isBefStamp == FALSE )
      {
        drawWork->editMode = CDED_PEN;
      }
      else
      {
        drawWork->editMode = CDED_STAMP;
      }
      CTVT_DRAW_UpdateEditButton( work , drawWork );
    }
    CTVT_DRAW_EreseSubWindow( work , drawWork );
    return;
  }
  if( (drawWork->state == CDS_EDIT || drawWork->state == CDS_DRAW) &&
      GFL_UI_TP_GetTrg() == TRUE &&
      drawWork->isTouch == FALSE &&
      drawWork->editMode == CDED_STAMP )
  {
    BOOL isFull;
    const u8 connectNum = COMM_TVT_GetConnectNum( work );
    CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
    DRAW_SYS_WORK *drawSys = COMM_TVT_GetDrawSys( work );
    DRAW_SYS_PEN_INFO *info = CTVT_COMM_GetDrawBuf(work,commWork,&isFull);
    if( isFull == FALSE )
    {
      info->startX = tpx;
      info->startY = tpy;
      info->endX = tpx;
      info->endY = tpy;
      info->penType = drawWork->stampType;
      info->col  = drawWork->penCol;
      
      if( connectNum == 1 )
      {
        DRAW_SYS_SetPenInfo( drawSys , info );
      }
      else
      {
        CTVT_COMM_AddDrawBuf( work , commWork );
      }
    }
  }
  
  if( GFL_UI_TP_GetTrg() == TRUE &&
      drawWork->isTouch == FALSE )
  {
    if( drawWork->editMode == CDED_SPOITO )
    {
      u16* spoitColAdr = (u16*)(HW_OBJ_PLTT + 0*32 + 2*0xd);
      //BG2と3のアドレス
      u16* adr2 = (u16*)((u32)G2_GetBG2ScrPtr() + tpx*2 + tpy*256*2);
      u16* adr3 = (u16*)((u32)G2_GetBG3ScrPtr() + tpx*2 + tpy*256*2);

      if( COMM_TVT_IsDoubleMode(work) == TRUE )
      {
        //拡大時はアドレスがずれる
        adr3 = (u16*)((u32)G2_GetBG3ScrPtr() + (tpx/2)*2 + (tpy/2)*256*2);
      }
      else
      {
        adr3 = (u16*)((u32)G2_GetBG3ScrPtr() + tpx*2 + tpy*256*2);
      }
      if( *adr2 & 0x8000 )
      {
        drawWork->penCol = *adr2;
      }
      else
      if( *adr3 & 0x8000 )
      {
        drawWork->penCol = *adr3;
      }
      else
      {
        //強制黒
        drawWork->penCol = 0x8000;
        
      }
      *spoitColAdr = (drawWork->penCol&0x7FFF);
      
      if( drawWork->isBefStamp == FALSE )
      {
        drawWork->editMode = CDED_PEN;
      }
      else
      {
        drawWork->editMode = CDED_STAMP;
      }
      CTVT_DRAW_UpdateEditButton( work , drawWork );
      CTVT_TPrintf("Spoit!![%x]\n",drawWork->penCol-0x8000);
      PMSND_PlaySystemSE( CTVT_SND_SUPOITO );
    }
    else
    {
      //drawWork->befPenX = tpx;
      //drawWork->befPenY = tpy;
      drawWork->isHold = TRUE;
      drawWork->befPenX = 0xFFFF;
      drawWork->befPenY = 0xFFFF;
      CTVT_DRAW_EreseSubWindow( work , drawWork );
    }
  }
  
  //落書き部分
  if( drawWork->editMode == CDED_PEN ||
      drawWork->editMode == CDED_KESHIGOMU )
  {
#if CTVT_DRAW_USE_SAMPLING
    if( drawWork->samplingRes == TP_OK )
    {
      u8 i;
      if( drawWork->tpData.Size > 0 )
      {
        for(i=0 ; i<drawWork->tpData.Size ; i++ )
        {
          if( drawWork->tpData.TPDataTbl[i].validity ==  TP_VALIDITY_VALID &&
              drawWork->tpData.TPDataTbl[i].touch == TRUE )
          {
            if( drawWork->isHold == TRUE &&
                drawWork->befPenX < 0xFFFF &&
                drawWork->befPenY < 0xFFFF )
            {
              BOOL isFull;
              const u8 connectNum = COMM_TVT_GetConnectNum( work );
              CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
              DRAW_SYS_WORK *drawSys = COMM_TVT_GetDrawSys( work );
              DRAW_SYS_PEN_INFO *info = CTVT_COMM_GetDrawBuf(work,commWork,&isFull);
              if( isFull == FALSE )
              {
                info->startX = drawWork->befPenX;
                info->startY = drawWork->befPenY;
                if( drawWork->befPenX != drawWork->tpData.TPDataTbl[i].x ||
                    drawWork->befPenY != drawWork->tpData.TPDataTbl[i].y )
                {
                  info->endX = drawWork->tpData.TPDataTbl[i].x;
                  info->endY = drawWork->tpData.TPDataTbl[i].y;
                  info->penType = drawWork->penSize;
                  if( drawWork->editMode == CDED_KESHIGOMU )
                  {
                    info->col = 0;
                    info->penType = DSPS_CIRCLE_8;
                  }
                  else
                  {
                    info->col  = drawWork->penCol;
                  }
                  
                  if( connectNum == 1 )
                  {
                    DRAW_SYS_SetPenInfo( drawSys , info );
                  }
                  else
                  {
                    CTVT_COMM_AddDrawBuf( work , commWork );
                  }
                }
              }
              else //isFull == TRUE
              {
                drawWork->isHold = FALSE+
              }
            }
            drawWork->befPenX = drawWork->tpData.TPDataTbl[i].x;
            drawWork->befPenY = drawWork->tpData.TPDataTbl[i].y;
          }
          else
          {
          }
        }
      }
    }
#else
  if( drawWork->isHold == TRUE )
  {
    u32 tpx,tpy;
    GFL_UI_TP_GetPointCont( &tpx,&tpy );
    if( drawWork->isHold == TRUE )
    {
      if( drawWork->befPenX != 0xFFFF &&
          drawWork->befPenY != 0xFFFF )
      {
        BOOL isFull;
        const u8 connectNum = COMM_TVT_GetConnectNum( work );
        CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
        DRAW_SYS_WORK *drawSys = COMM_TVT_GetDrawSys( work );
        DRAW_SYS_PEN_INFO *info = CTVT_COMM_GetDrawBuf(work,commWork,&isFull);
        if( isFull == FALSE )
        {
          info->startX = drawWork->befPenX;
          info->startY = drawWork->befPenY;
          info->endX = tpx;
          info->endY = tpy;
          info->penType = drawWork->penSize;
          if( drawWork->editMode == CDED_KESHIGOMU )
          {
            info->col = 0;
            info->penType = DSPS_CIRCLE_8;
          }
          else
          {
            info->col  = drawWork->penCol;
          }
          
          if( connectNum == 1 )
          {
            DRAW_SYS_SetPenInfo( drawSys , info );
          }
          else
          {
            CTVT_COMM_AddDrawBuf( work , commWork );
          }
        }
        else//isFull == TRUE
        {
          drawWork->isHold = FALSE;
        }
      }
      drawWork->befPenX = tpx;
      drawWork->befPenY = tpy;
    }
  }
#endif
  }

}

//--------------------------------------------------------------
//	バー位置更新
//--------------------------------------------------------------
static void CTVT_DRAW_UpdateBar( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  BOOL isUpdate = FALSE;
  if( GFL_CLACT_WK_CheckAnmActive( drawWork->clwkEditButton[CDED_RETURN] ) == FALSE ||
      GFL_CLACT_WK_GetAnmSeq( drawWork->clwkEditButton[CDED_RETURN] ) == APP_COMMON_BARICON_RETURN )
  {
    if( drawWork->barScroll < drawWork->dispBarScroll )
    {
      drawWork->dispBarScroll -= 24;
      isUpdate = TRUE;
    }
    else
    if( drawWork->barScroll > drawWork->dispBarScroll )
    {
      drawWork->dispBarScroll += 24;
      isUpdate = TRUE;
    }
  }
  
  if( isUpdate == TRUE )
  {
    u8 i;
    GFL_BG_SetScrollReq( CTVT_FRAME_MAIN_MSG , GFL_BG_SCROLL_Y_SET , -drawWork->dispBarScroll+24 );
    for( i=0;i<CDED_MAX;i++ )
    {
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos( drawWork->clwkEditButton[i] , &pos , CLSYS_DRAW_MAIN );
      pos.y = CTVT_DRAW_BAR_ICON_Y+drawWork->dispBarScroll;
      if( i == CDED_RETURN )
      {
        pos.y -= 12;
      }
      GFL_CLACT_WK_SetPos( drawWork->clwkEditButton[i] , &pos , CLSYS_DRAW_MAIN );
    }
  }
}


//--------------------------------------------------------------
//	ボタングラフィック更新
//--------------------------------------------------------------
static void CTVT_DRAW_UpdateEditButton( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  {
    static const u8 anmIdx[4][2] = 
    {
      {CTOAM_PEN,CTOAM_PEN_ACTIVE},
      {CTOAM_SUPOITO,CTOAM_SUPOITO_ACTIVE},
      {CTOAM_KESHIGOMU,CTOAM_KESHIGOMU_ACTIVE},
      {CTOAM_STAMP,CTOAM_STAMP_ACTIVE},
    };
    
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_PEN] , 
                            anmIdx[CDED_PEN][(drawWork->editMode==CDED_PEN?1:0)] );
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_SPOITO] , 
                            anmIdx[CDED_SPOITO][(drawWork->editMode==CDED_SPOITO?1:0)] );
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_KESHIGOMU] , 
                            anmIdx[CDED_KESHIGOMU][(drawWork->editMode==CDED_KESHIGOMU?1:0)] );
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_STAMP] , 
                            anmIdx[CDED_STAMP][(drawWork->editMode==CDED_STAMP?1:0)] );
  }
  
  if( COMM_TVT_GetPause( work ) == TRUE )
  {
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_PAUSE] , CTOAM_PAUSE );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( drawWork->clwkEditButton[CDED_PAUSE] , CTOAM_PLAY );
  }
  
  if( drawWork->editMode != CDED_PEN &&
      drawWork->isDispPenSize == TRUE ) 
  {
    drawWork->isDispPenSize = FALSE;
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkPenSize , FALSE );
    GFL_CLACT_WK_ResetAnm( drawWork->clwkPenSize );
    GFL_CLACT_WK_SetAnmFrame( drawWork->clwkPenSize , 0 );
    GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkPenSize , FALSE );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );
  }
  
  if( drawWork->editMode != CDED_STAMP &&
      drawWork->isDispStampType == TRUE ) 
  {
    drawWork->isDispStampType = FALSE;
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkStampType , FALSE );
    GFL_CLACT_WK_ResetAnm( drawWork->clwkStampType );
    GFL_CLACT_WK_SetAnmFrame( drawWork->clwkStampType , 0 );
    GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkStampType , FALSE );
    GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );
  }

}

//--------------------------------------------------------------
//	説明メッセージ更新
//--------------------------------------------------------------
static void CTVT_DRAW_DrawInfoMsg( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork , const BOOL isEditMode )
{
  GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
  GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
  PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
  GFL_BMPWIN *tempWin = drawWork->infoWin;
  if( tempWin != NULL )
  {
    PRINTSYS_QUE_Clear( printQue );
    BmpWinFrame_Clear( drawWork->infoWin , WINDOW_TRANS_OFF );
    GFL_BMPWIN_ClearScreen( drawWork->infoWin );
  }

  drawWork->isUpdateInfoEdit = isEditMode;
  if( isEditMode == TRUE )
  {
    u8 i;
    STRBUF *str;

    drawWork->infoWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                      2 ,  8 , 28 , 13 ,
                                      CTVT_PAL_BG_SUB_FONT ,
                                      GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 0xF );
    GFL_BMPWIN_TransVramCharacter( drawWork->infoWin );
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_007 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_EDIT_X , 4 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_008 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_EDIT_X , 24 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_009 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_EDIT_X , 44 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_011 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_EDIT_X , 64 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_010 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_EDIT_X , 84 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
  }
  else
  {
    STRBUF *str;
    
    drawWork->infoWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                      2 , 16 , 28 ,  6 ,
                                      CTVT_PAL_BG_SUB_FONT ,
                                      GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 0xF );
    GFL_BMPWIN_TransVramCharacter( drawWork->infoWin );

    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_001 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X1 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
    
    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_002 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X1 , 16 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );

    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_003 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X1 , 32 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );

    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_004 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X2 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );

    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_005 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X2 , 16 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );

    str = GFL_MSG_CreateString( msgHandle , COMM_TVT_DRAW_006 );
    PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->infoWin ) , 
            CTVT_DRAW_INFO_DRAW_X2 , 32 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
    GFL_STR_DeleteBuffer( str );
  }
  drawWork->isUpdateInfo = TRUE;


  if( tempWin != NULL )
  {
    //キャラ領域の被りを防ぐためここで開放
    GFL_BMPWIN_Delete(tempWin);
  }
}


//--------------------------------------------------------------
//	強制終了チェック
//--------------------------------------------------------------
static const BOOL CTVT_DRAW_CheckFinishReq( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  if( COMM_TVT_GetFinishReq( work ) == TRUE )
  {
    CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
    if( CTVT_COMM_GetMode( work , commWork ) == CCIM_CONNECTED )
    {
      //Wifiのリクエストによる終了
      drawWork->state = CDS_END_WIFI_REQ_INIT_DISP;
    }
    else
    {
      //親のリクエストによる終了
      drawWork->state = CDS_END_PARENT_REQ_INIT;
    }
    return TRUE;
  }
  if( COMM_TVT_GetConnectNum( work ) <= 1 )
  {
    //誰も居なくなった。
    drawWork->state = CDS_END_MEMBER_NONE_INIT;
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//	Msg表示
//--------------------------------------------------------------
static void CTVT_DRAW_DispMessage( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork , const u16 msgId )
{
  GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
  GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
  PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
  STRBUF *str;
  u8 posy;
  if( drawWork->msgWin != NULL )
  {
    GFL_BMPWIN_Delete( drawWork->msgWin );
  }
  if( drawWork->barScroll == 0 )
  {
    posy = 4;
  }
  else
  {
    posy = 1;
  }
  drawWork->msgWin = GFL_BMPWIN_Create( CTVT_FRAME_MAIN_MSG , 
                                        1 , posy , 30 , 4 ,
                                        CTVT_PAL_BG_SUB_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  
  PRINTSYS_QUE_Clear( printQue );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( drawWork->msgWin) , 0xF );
  str = GFL_MSG_CreateString( msgHandle , msgId );
  PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( drawWork->msgWin ) , 
          0 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
  GFL_STR_DeleteBuffer( str );

  GFL_BMPWIN_TransVramCharacter( drawWork->msgWin );
  GFL_BMPWIN_MakeScreen( drawWork->msgWin );
  BmpWinFrame_Write( drawWork->msgWin , WINDOW_TRANS_ON_V , 
                      CTVT_BMPWIN_CGX_MAIN , CTVT_PAL_BG_MAIN_WINFRAME );
  drawWork->isUpdateMsgWin = TRUE;

}

//--------------------------------------------------------------
//  ペンサイズ、スタンプタイプ非表示
//--------------------------------------------------------------
static void CTVT_DRAW_EreseSubWindow( COMM_TVT_WORK *work , CTVT_DRAW_WORK *drawWork )
{
  //UpdateEditButtonにも単独で消しているところがあるので注意
  drawWork->isDispPenSize = FALSE;
  drawWork->isDispStampType = FALSE;
  GFL_CLACT_WK_ResetAnm( drawWork->clwkPenSize );
  GFL_CLACT_WK_ResetAnm( drawWork->clwkStampType );
  GFL_CLACT_WK_ResetAnm( drawWork->clwkBlinkButton );
  GFL_CLACT_WK_SetAnmFrame( drawWork->clwkPenSize , 0 );
  GFL_CLACT_WK_SetAnmFrame( drawWork->clwkStampType , 0 );
  GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkPenSize , FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( drawWork->clwkStampType , FALSE );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkPenSize , FALSE );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkStampType , FALSE );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkBlinkButton , FALSE );
  GFL_CLACT_WK_SetDrawEnable( drawWork->clwkSelectButton , FALSE );

}
