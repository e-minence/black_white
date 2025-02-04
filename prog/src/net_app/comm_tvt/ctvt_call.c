//======================================================================
/**
 * @file	ctvt_call.c
 * @brief	通信TVTシステム：呼び出し時下画面
 * @author	ariizumi
 * @data	09/12/21
 *
 * モジュール名：CTVT_CALL
 */
//======================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/time_icon.h"
#include "system/camera_system.h"
#include "net/network_define.h"
#include "net/net_whpipe.h"
#include "net/ctvt_beacon_local.h"
#include "app/app_menu_common.h"
#include "print/wordset.h"
#include "field/game_beacon_search.h"
#include "field/zonedata.h"
#include "savedata/wifilist.h"
#include "gamesystem/game_data.h"

#include "msg/msg_comm_tvt.h"

#include "comm_tvt.naix"

#include "ctvt_call.h"
#include "ctvt_comm.h"
#include "ctvt_camera.h"
#include "ctvt_snd_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//表示バーの個数
#define CTVT_CALL_BAR_NUM (15) //画面分
//データ個数
#define CTVT_CALL_SEARCH_NUM (15) //画面分
#define CTVT_CALL_INVALID_NO (0xFF)


#define CTVT_CALL_BAR_X (128)
#define CTVT_CALL_BAR_Y (16)
#define CTVT_CALL_BAR_HEIGHT (32)

#define CTVT_CALL_SCROLL_LEN (CTVT_CALL_BAR_HEIGHT*CTVT_CALL_SEARCH_NUM-20*8)

#define CTVT_CALL_CHECK_X (18)
#define CTVT_CALL_CAMERA_X (200)

#define CTVT_CALL_BEACONSIZE (sizeof(GBS_BEACON)<sizeof(CTVT_COMM_BEACON)?sizeof(CTVT_COMM_BEACON):sizeof(GBS_BEACON))


#define CTVT_STRBMP_X (6)
#define CTVT_STRBMP_Y (0)
#define CTVT_STRBMP_WIDTH (22)
#define CTVT_STRBMP_HEIGHT (32)

#define CTVT_CALL_RETURN_X (224)

#define CTVT_CALL_SCROLL_X (256-8)
#define CTVT_CALL_SCROLL_Y (24)
#define CTVT_CALL_SCROLLBAR_LEN (120)

#define CTVT_CALL_SCROLL_HIT_WIDTH  (12)
#define CTVT_CALL_SCROLL_HIT_HEIGHT (12)

#define CTVT_CALL_MSG_SE_CNT (120)

//接続時タイムアウト
#define CTVT_CALL_JOIN_TIMEOUT (CTVT_CALL_TIME)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

typedef enum
{
  CCS_FADEIN,
  CCS_FADEIN_WAIT,
  CCS_FADEOUT,
  CCS_FADEOUT_BOTH,
  CCS_FADEOUT_WAIT,
  //ここ以前はバーの表示をしない判定で使っているので順番変えるな
  
  CCS_MAIN,
  CCS_WAIT_ANIME,
  CCS_WAIT_CONNECT_JOIN,
  CCS_WAIT_CONNECT_JOIN_DIRECT,
  CCS_WAIT_CONNECT_CALL,
  
  CCS_FAILUE_CONNECT,

  CCS_DISP_WARN_INIT,
  CCS_DISP_WARN,

}CTVT_CALL_STATE;

typedef enum
{
  CCBS_NONE,
  CCBS_CALL_CHILD,
  CCBS_JOIN_PARENT,
  CCBS_JOIN_PARENT_DIRECT,
  CCBS_DISP_MSG,
  
  CCBS_CANCEL,
}CTVT_CALL_BAR_STATE;
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _CTVT_CALL_MEMBER_WORK CTVT_CALL_MEMBER_WORK;
typedef struct _CTVT_CALL_BAR_WORK CTVT_CALL_BAR_WORK;

//メンバー情報
struct _CTVT_CALL_MEMBER_WORK
{
  BOOL isEnable;
  BOOL isLost;
  BOOL isCheck;
  BOOL isCamera;
  BOOL isBlinkReq;
  void *beacon;
  GameServiceID serviceType;
  u8 macAddress[6];
  u8 connectNum;
  
  u8 barWorkNo;
};

//バーワーク
struct _CTVT_CALL_BAR_WORK
{
  BOOL isUpdate;
  BOOL isUpdateStr;
  
  GFL_CLWK *clwkBar;
  GFL_CLWK *clwkCheck;
  GFL_CLWK *clwkCamera;

  GFL_BMPWIN *strWin;
  
  u8 memberWorkNo;
};


//ワーク
struct _CTVT_CALL_WORK
{
  u8 checkIdx[3];
  u8 checkIdxParent;
  u16 scrollOfs;
  u16 scrollBarPos;
  CTVT_CALL_STATE state;
  BOOL isUpdateBarPos;
  BOOL isHoldScroll;
  u8   barNum;
  u8   befbarNum; //誰も居ない時Msg更新用
  
  CTVT_CALL_BAR_STATE barState;
  APP_TASKMENU_WIN_WORK *barMenuWork;

  BOOL  isUpdateMsgWin;
  BOOL  isUpdateCallMsgWin;
  BOOL  isPlayCallAnswer;
  GFL_BMPWIN *msgWin;
  GFL_BMPWIN *callMsgWin;
  u16 connectTimeOutCnt;
  u16 callSeCnt;

  BOOL          reqDispTimeIcon;
  GFL_TCBLSYS   *tcblSys;

  CTVT_CALL_BAR_WORK barWork[CTVT_CALL_BAR_NUM];
  CTVT_CALL_MEMBER_WORK memberData[CTVT_CALL_SEARCH_NUM];

  GFL_CLWK *clwkReturn;
  GFL_CLWK *clwkScrollBar;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void CTVT_CALL_UpdateTP( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );

static void CTVT_CALL_UpdateBeacon( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );
static void CTVT_CALL_UpdateBar( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );
static void CTVT_CALL_UpdateBarFunc( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , CTVT_CALL_BAR_WORK *barWork , const u8 idx );
static void CTVT_CALL_UpdateBarPosFunc( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , CTVT_CALL_BAR_WORK *barWork , const u8 idx );
static void CTVT_CALL_UpdateBarMenu( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );

static void CTVT_CALL_UpdateNoMemberMsg( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork );

static const BOOL CTVT_CALL_CheckRegistFriendData( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const STRCODE *name , const u32 id , const u32 sex );
static void CTVT_CALL_DispMessage( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const u16 msgId );
static void CTVT_CALL_DispCallMessage( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const u16 msgId );


//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
CTVT_CALL_WORK* CTVT_CALL_InitSystem( COMM_TVT_WORK *work , const HEAPID heapId )
{
  u8 i;
  CTVT_CALL_WORK* callWork = GFL_HEAP_AllocClearMemory( heapId , sizeof(CTVT_CALL_WORK) );

  return callWork;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
void CTVT_CALL_TermSystem( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  GFL_HEAP_FreeMemory( callWork );
}
//--------------------------------------------------------------
//	モード切替時初期化
//--------------------------------------------------------------
void CTVT_CALL_InitMode( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  ARCHANDLE* arcHandle = COMM_TVT_GetArcHandle( work );
  const COMM_TVT_INIT_WORK *initWork = COMM_TVT_GetInitWork( work );

  //MISCをスクロールするのでキャラ表示に使うのでMSGに展開  

  if( initWork->mode != CTM_CHILD &&
      initWork->mode != CTM_WIFI &&
      COMM_TVT_IsReqWarn( work ) == FALSE )
  {
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCGR ,
                      CTVT_FRAME_SUB_MSG , 0 , CTVT_BMPWIN_CGX*0x20, FALSE , heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_comm_tvt_tv_t_yobidashi_bg_NSCR , 
                      CTVT_FRAME_SUB_MSG ,  0 , 0, FALSE , heapId );
    GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MSG );
  }
  {
    u8 i;
    GFL_CLWK_DATA cellInitData;
    cellInitData.bgpri = 3;
    
    for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
    {
      cellInitData.pos_x = CTVT_CALL_BAR_X;
      cellInitData.pos_y = CTVT_CALL_BAR_Y + CTVT_CALL_BAR_HEIGHT*i;
      cellInitData.anmseq = CTOAS_YOBIDASHI_BAR;
      cellInitData.softpri = 32;
      callWork->barWork[i].clwkBar = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
                &cellInitData ,CLSYS_DRAW_SUB , heapId );
      GFL_CLACT_WK_SetDrawEnable( callWork->barWork[i].clwkBar , FALSE );
      GFL_CLACT_WK_SetAutoAnmFlag( callWork->barWork[i].clwkBar , TRUE );

      cellInitData.pos_x = CTVT_CALL_CHECK_X;
      cellInitData.anmseq = CTOAS_CHECK;
      cellInitData.softpri = 16;
      callWork->barWork[i].clwkCheck = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
                &cellInitData ,CLSYS_DRAW_SUB , heapId );
      GFL_CLACT_WK_SetDrawEnable( callWork->barWork[i].clwkCheck , FALSE );

      cellInitData.pos_x = CTVT_CALL_CAMERA_X;
      cellInitData.anmseq = CTOAS_CAMERA_ON;
      cellInitData.softpri = 16;
      callWork->barWork[i].clwkCamera = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
                COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
                &cellInitData ,CLSYS_DRAW_SUB , heapId );
      GFL_CLACT_WK_SetDrawEnable( callWork->barWork[i].clwkCamera , FALSE );
    }

    //戻るボタン
    cellInitData.pos_x = CTVT_CALL_RETURN_X;
    cellInitData.pos_y = 192-24;
    if( COMM_TVT_IsReqWarn( work ) == FALSE )
    {
      cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    }
    else
    {
      cellInitData.anmseq = APP_COMMON_BARICON_RETURN_OFF;
    }
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    callWork->clwkReturn = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_BAR_BUTTON_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( callWork->clwkReturn , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( callWork->clwkReturn , TRUE );
    
    cellInitData.pos_x = CTVT_CALL_SCROLL_X;
    cellInitData.pos_y = CTVT_CALL_SCROLL_Y;
    cellInitData.anmseq = CTOAS_SCROLL_BAR;
    callWork->clwkScrollBar = GFL_CLACT_WK_Create( COMM_TVT_GetCellUnit(work) ,
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_NCG ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_PLT ),
              COMM_TVT_GetObjResIdx( work, CTOR_COMMON_S_ANM ),
              &cellInitData ,CLSYS_DRAW_SUB , heapId );
    GFL_CLACT_WK_SetDrawEnable( callWork->clwkScrollBar , FALSE );
  }
  {
    u8 i;
    for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
    {
      callWork->memberData[i].isEnable = FALSE;
      callWork->memberData[i].isLost = FALSE;
      callWork->memberData[i].isCheck = FALSE;
      callWork->memberData[i].isBlinkReq = FALSE;
      callWork->memberData[i].barWorkNo = CTVT_CALL_INVALID_NO;
      callWork->memberData[i].beacon = GFL_HEAP_AllocClearMemory( heapId , CTVT_CALL_BEACONSIZE );
      callWork->memberData[i].connectNum = 0;
    }
    for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
    {
      callWork->barWork[i].isUpdate = FALSE;
      callWork->barWork[i].isUpdateStr = FALSE;
      callWork->barWork[i].memberWorkNo = CTVT_CALL_INVALID_NO;
      callWork->barWork[i].strWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MISC , 
                                          3 , i*(CTVT_CALL_BAR_HEIGHT/8)+1 , 25 , 2 ,
                                          CTVT_PAL_BG_SUB_FONT ,
                                          GFL_BMP_CHRAREA_GET_B );
    }
  }
  //MakeScreenは文字表示の時
  callWork->msgWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                        1 , 21 , 21 , 2 ,
                                        CTVT_PAL_BG_SUB_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  callWork->callMsgWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                        2 , 9 , 28 , 2 ,
                                        CTVT_PAL_BG_SUB_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
  callWork->reqDispTimeIcon = FALSE;
  callWork->callSeCnt = 0;
  {
    u8 i;
    for( i=0;i<3;i++ )
    {
      callWork->checkIdx[i] = CTVT_CALL_INVALID_NO;
    }
    callWork->checkIdxParent = CTVT_CALL_INVALID_NO;
  }
  callWork->scrollOfs = 0;
  callWork->scrollBarPos = 0;
  callWork->barNum = 0;
  callWork->befbarNum = 0xFF;  //強制更新
  callWork->isUpdateBarPos = FALSE;
  callWork->isHoldScroll = FALSE;
  callWork->isUpdateMsgWin = FALSE;
  callWork->isUpdateCallMsgWin = FALSE;
  callWork->isPlayCallAnswer = FALSE;
  callWork->barState = 0xFF;  //強制更新
  callWork->barMenuWork = NULL;
  callWork->state = CCS_FADEIN;
  callWork->connectTimeOutCnt = 0;

  GFL_BG_SetScrollReq( CTVT_FRAME_SUB_MISC , GFL_BG_SCROLL_Y_SET , 0 );
  
  {
    if( initWork->mode == CTM_CHILD ||
        initWork->mode == CTM_WIFI )
    {
      if( COMM_TVT_IsReqWarn( work ) == FALSE )
      {
        //ペアコンある時は出さない
        //CGEARから呼び出しできた。
        GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
        CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_CALL_11 );
        callWork->reqDispTimeIcon = TRUE;
        callWork->barState = CCBS_JOIN_PARENT_DIRECT;
      }
    }
    else
    {
      //メッセージのアップデートで出す
      //CTVT_CALL_DispMessage( work , callWork , COMM_TVT_CALL_04 );
    }
  }
}

//--------------------------------------------------------------
//	モード切替時開放
//--------------------------------------------------------------
void CTVT_CALL_TermMode( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  u8 i;
  if( callWork->barMenuWork != NULL )
  {
    APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
    callWork->barMenuWork = NULL;
  }
  
  GFL_BMPWIN_ClearTransWindow( callWork->msgWin );
  GFL_BMPWIN_Delete( callWork->msgWin );
  GFL_BMPWIN_ClearTransWindow( callWork->callMsgWin );
  GFL_BMPWIN_Delete( callWork->callMsgWin );
  
  GFL_CLACT_WK_Remove( callWork->clwkScrollBar );
  GFL_CLACT_WK_Remove( callWork->clwkReturn );
  for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
  {
    GFL_BMPWIN_Delete( callWork->barWork[i].strWin );
    GFL_CLACT_WK_Remove( callWork->barWork[i].clwkCamera );
    GFL_CLACT_WK_Remove( callWork->barWork[i].clwkCheck );
    GFL_CLACT_WK_Remove( callWork->barWork[i].clwkBar );
  }
  for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
  {
    GFL_HEAP_FreeMemory( callWork->memberData[i].beacon );
  }

  GFL_BG_ClearScreen( CTVT_FRAME_SUB_MISC );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MISC );
  GFL_BG_SetScrollReq( CTVT_FRAME_SUB_MISC , GFL_BG_SCROLL_Y_SET , 0 );

  GFL_BG_ClearScreen( CTVT_FRAME_SUB_MSG );
  GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MSG );
  GFL_BG_SetScrollReq( CTVT_FRAME_SUB_MSG , GFL_BG_SCROLL_Y_SET , 0 );
  
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
const COMM_TVT_MODE CTVT_CALL_Main( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  const COMM_TVT_INIT_WORK *initWork = COMM_TVT_GetInitWork( work );
  
  switch( callWork->state )
  {
  case CCS_FADEIN:
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
    callWork->state = CCS_FADEIN_WAIT;
    break;
  case CCS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      const COMM_TVT_INIT_WORK *initWork = COMM_TVT_GetInitWork( work );
      if( COMM_TVT_IsReqWarn( work ) == TRUE )
      {
        //ペアコン警告
        callWork->state = CCS_DISP_WARN_INIT;
      }
      else
      if( initWork->mode == CTM_CHILD ||
          initWork->mode == CTM_WIFI )
      {
        //CGEARから呼び出しできた。
        callWork->state = CCS_WAIT_CONNECT_JOIN_DIRECT;
      }
      else
      {
        callWork->state = CCS_MAIN;
      }
    }
    break;

  case CCS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
    COMM_TVT_SetUpperFade( work , FALSE );
    callWork->state = CCS_FADEOUT_WAIT;
    break;

  case CCS_FADEOUT_BOTH:
    {
      CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
      if( CTVT_CAMERA_IsStopCapture( work , camWork ) == TRUE )
      {
        WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , heapId );
        COMM_TVT_SetUpperFade( work , FALSE );
        callWork->state = CCS_FADEOUT_WAIT;
      }
    }
    break;

  case CCS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( callWork->barState == CCBS_NONE ||
          callWork->barState == CCBS_CANCEL )
      {
        return CTM_END;
      }
      else
      {
        return CTM_TALK;
      }
    }
    break;
    
  case CCS_MAIN:
    CTVT_CALL_UpdateTP( work , callWork );
    CTVT_CALL_UpdateNoMemberMsg( work , callWork );
    break;
    
  case CCS_WAIT_ANIME:
    if( callWork->barState == CCBS_NONE ||
        callWork->barState == CCBS_CANCEL )
    {
      if( GFL_CLACT_WK_CheckAnmActive( callWork->clwkReturn ) == FALSE )
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        CTVT_CAMERA_StopCapture( work , camWork );
        callWork->barState = CCBS_NONE;
        callWork->state = CCS_FADEOUT_BOTH;
      }
    }
    else
    {
      if( APP_TASKMENU_WIN_IsFinish( callWork->barMenuWork ) == TRUE )
      {
        if( callWork->barState == CCBS_JOIN_PARENT )
        {
          CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
          CTVT_COMM_SetMode( work , commWork , CCIM_CHILD );
          CTVT_COMM_SetMacAddress( work , commWork , callWork->memberData[ callWork->checkIdxParent ].macAddress );
          CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_CALL_11 );
          callWork->reqDispTimeIcon = TRUE;
          callWork->state = CCS_WAIT_CONNECT_JOIN;
          callWork->connectTimeOutCnt = 0;
          callWork->callSeCnt = 0;

          APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
          callWork->barMenuWork = NULL;
        }
        else
        {
          CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
          callWork->state = CCS_WAIT_CONNECT_CALL;
          CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_CALL_07 );
          callWork->callSeCnt = 0;
          callWork->reqDispTimeIcon = TRUE;
          CTVT_COMM_SetMode( work , commWork , CCIM_PARENT );

          APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
          callWork->barMenuWork = NULL;
          //ビーコンのセット
          {
            u8 i;
            CTVT_COMM_BEACON *beacon = CTVT_COMM_GetCtvtBeaconData( work , commWork );
            for( i=0;i<3;i++ )
            {
              u8 j;
              if( callWork->checkIdx[i] == CTVT_CALL_INVALID_NO )
              {
                for( j=0;j<6;j++ )
                {
                  beacon->callTarget[i][j] = 0xFF;
                }
              }
              else
              {
                for( j=0;j<6;j++ )
                {
                  beacon->callTarget[i][j] = callWork->memberData[ callWork->checkIdx[i] ].macAddress[j];
                }
              }
            }
            beacon->isCalling = 1;
            CTVT_COMM_ResetBeaconTime( work , commWork );
          }
        }
        GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
      }
    }
    break;
  
  case CCS_WAIT_CONNECT_JOIN:
  case CCS_WAIT_CONNECT_JOIN_DIRECT:
  case CCS_WAIT_CONNECT_CALL:
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      if( COMM_TVT_GetConnectNum( work ) >= 2 )
      {
        if( callWork->isPlayCallAnswer == FALSE )
        {
          CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
          PMSND_StopSE();
          PMSND_PlaySystemSE( CTVT_SND_CALL_ANSWER );
          CTVT_CAMERA_PlayStartSe(work,camWork);
        }
        callWork->state = CCS_FADEOUT;
      }
      if( callWork->state == CCS_WAIT_CONNECT_CALL )
      {
        callWork->callSeCnt++;
        if( callWork->callSeCnt >= CTVT_CALL_MSG_SE_CNT )
        {
          callWork->callSeCnt = 0;
          PMSND_PlaySE_byPlayerID( CTVT_SND_TEL_CALL , SEPLAYER_SE2 );
        }
      }
    }
    //時間切れ。戻る
    if( callWork->state == CCS_WAIT_CONNECT_CALL )
    {
      CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
      if( CTVT_COMM_IsEndBeaconTime( work , commWork ) == TRUE )
      {
        CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
        CTVT_COMM_SetMode( work , commWork , CCIM_SCAN );
        CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_SYS_08 );
        callWork->state = CCS_FAILUE_CONNECT;
      }
    }
    else
    if( callWork->state == CCS_WAIT_CONNECT_JOIN ||
        callWork->state == CCS_WAIT_CONNECT_JOIN_DIRECT )
    {
      callWork->connectTimeOutCnt++;
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
      {
        callWork->connectTimeOutCnt = CTVT_CALL_JOIN_TIMEOUT+1;
      }
#endif
      if( callWork->connectTimeOutCnt >= CTVT_CALL_JOIN_TIMEOUT )
      {
        CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
        CTVT_COMM_SetMode( work , commWork , CCIM_SCAN );
        CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_SYS_09 );
        callWork->state = CCS_FAILUE_CONNECT;
      }
    }
    break;
    
  case CCS_FAILUE_CONNECT:
    if( GFL_UI_TP_GetTrg() == TRUE )
    {
      const COMM_TVT_INIT_WORK *initWork = COMM_TVT_GetInitWork( work );
      if( initWork->mode == CTM_CHILD )
      {
        CTVT_CAMERA_WORK *camWork = COMM_TVT_GetCameraWork( work );
        //CGEARから呼び出しできた。
        CTVT_CAMERA_StopCapture( work , camWork );
        callWork->barState = CCBS_NONE;
        callWork->state = CCS_FADEOUT_BOTH;
      }
      else
      {
        u8 i;
        for( i=0;i<3;i++ )
        {
          callWork->checkIdx[i] = CTVT_CALL_INVALID_NO;
        }
        callWork->checkIdxParent = CTVT_CALL_INVALID_NO;

        callWork->state = CCS_MAIN;
        BmpWinFrame_Clear( callWork->callMsgWin , WINDOW_TRANS_ON_V );
        GFL_BMPWIN_ClearTransWindow( callWork->callMsgWin );
        COMM_TVT_DeleteTimerIcon( work );

        GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN );
        //メッセージのアップデートで出す
        //CTVT_CALL_DispMessage( work , callWork , COMM_TVT_CALL_04 );
        callWork->barState = CCBS_DISP_MSG;
      }
    }
    break;
  case CCS_DISP_WARN_INIT:
    {
      GFL_BMPWIN_Delete(callWork->msgWin);
      callWork->msgWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                            2 , 5 , 28 , 14 ,
                                            CTVT_PAL_BG_SUB_FONT ,
                                            GFL_BMP_CHRAREA_GET_B );
      CTVT_CALL_DispMessage( work , callWork , COMM_TVT_SYS_10 );
    }
    callWork->state = CCS_DISP_WARN;

    break;
  case CCS_DISP_WARN:
    if( COMM_TVT_GetConnectNum( work ) >= 2 )
    {
      if( callWork->isPlayCallAnswer == FALSE )
      {
        PMSND_StopSE();
        callWork->isPlayCallAnswer = TRUE;
        PMSND_PlaySystemSE( CTVT_SND_CALL_ANSWER );
      }
    }
    if( GFL_UI_TP_GetTrg() == TRUE ||
        COMM_TVT_GetFinishReq( work ) == TRUE )
    {
      PMSND_PlaySystemSE( CTVT_SND_DECIDE );
      BmpWinFrame_Clear( callWork->msgWin , WINDOW_TRANS_ON_V );
      GFL_BMPWIN_ClearTransWindow( callWork->msgWin );
      GFL_BMPWIN_Delete(callWork->msgWin);
      callWork->msgWin = GFL_BMPWIN_Create( CTVT_FRAME_SUB_MSG , 
                                            1 , 21 , 21 , 2 ,
                                            CTVT_PAL_BG_SUB_FONT ,
                                            GFL_BMP_CHRAREA_GET_B );
      COMM_TVT_ResetIsReqWarn( work );
      if( initWork->mode == CTM_CHILD ||
          initWork->mode == CTM_WIFI )
      {
        //CGEARから呼び出しできた。
        GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
        if( COMM_TVT_GetConnectNum( work ) < 2 )
        {
          CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_CALL_11 );
        }
        callWork->reqDispTimeIcon = TRUE;
        callWork->barState = CCBS_JOIN_PARENT_DIRECT;
        callWork->state = CCS_WAIT_CONNECT_JOIN_DIRECT;
      }
      else
      {
        //メッセージのアップデートで出す
        callWork->state = CCS_MAIN;
        callWork->barState = CCBS_DISP_MSG;
        GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN );
        {
          const HEAPID heapId = COMM_TVT_GetHeapId( work );
          ARCHANDLE* arcHandle = COMM_TVT_GetArcHandle( work );
          GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_comm_tvt_tv_t_tuuwa_bg_NCGR ,
                            CTVT_FRAME_SUB_MSG , 0 , CTVT_BMPWIN_CGX*0x20, FALSE , heapId );
          GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_comm_tvt_tv_t_yobidashi_bg_NSCR , 
                            CTVT_FRAME_SUB_MSG ,  0 , 0, FALSE , heapId );
          GFL_BG_LoadScreenReq( CTVT_FRAME_SUB_MSG );
        }
        //CTVT_CALL_DispMessage( work , callWork , COMM_TVT_CALL_04 );
      }
    }
    break;

  
  }
  
  if( initWork->mode != CTM_WIFI &&
      COMM_TVT_IsReqWarn( work ) == FALSE )
  {
    CTVT_CALL_UpdateBeacon( work , callWork );
    {
      u8 i;
      for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
      {
        if( callWork->barWork[i].isUpdateStr == TRUE )
        {
          PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
          if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( callWork->barWork[i].strWin )) == FALSE )
          {
            GFL_BMPWIN_MakeTransWindow_VBlank( callWork->barWork[i].strWin );
            callWork->barWork[i].isUpdateStr = FALSE;
          }
        }
      }
    }
    if( callWork->isUpdateBarPos == TRUE )
    {
      u8 i;
      u8 checkNum = 0;
      for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
      {
        CTVT_CALL_UpdateBarPosFunc( work , callWork , &callWork->barWork[i] , i );
      }
      GFL_BG_SetScrollReq( CTVT_FRAME_SUB_MISC , GFL_BG_SCROLL_Y_SET , callWork->scrollOfs );

      //チェックボックスの更新
      for( i=0;i<3;i++ )
      {
        if( callWork->checkIdx[i] != CTVT_CALL_INVALID_NO )
        {
          checkNum++;
        }
      }
      for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
      {
        if( callWork->memberData[i].isEnable == TRUE )
        {
          CTVT_CALL_BAR_WORK *barWork = &callWork->barWork[ callWork->memberData[i].barWorkNo ];
          if( callWork->memberData[i].isCheck == TRUE &&
              callWork->memberData[i].connectNum < CTVT_MEMBER_NUM && 
              ( callWork->checkIdxParent == CTVT_CALL_INVALID_NO ||
                callWork->checkIdxParent == i ) )
          {
            GFL_CLACT_WK_SetAnmSeq( barWork->clwkCheck , CTOAS_CHECK_SELECT );
          }
          else
          {
            if( checkNum == 3 ||
                callWork->checkIdxParent != CTVT_CALL_INVALID_NO ||
                callWork->memberData[i].connectNum == CTVT_MEMBER_NUM )
            {
              GFL_CLACT_WK_SetAnmSeq( barWork->clwkCheck , CTOAS_CHECK_NONE );
            }
            else
            {
              GFL_CLACT_WK_SetAnmSeq( barWork->clwkCheck , CTOAS_CHECK );
            }
          }
          if( callWork->memberData[i].isBlinkReq == TRUE )
          {
            callWork->memberData[i].isBlinkReq = FALSE;
            GFL_CLACT_WK_SetAnmSeq( barWork->clwkBar , CTOAS_SCROLL_BAR_BLINK );
          }
        }
      }

      callWork->isUpdateBarPos = FALSE;
    }
  }

  if( callWork->isUpdateMsgWin == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( callWork->msgWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( callWork->msgWin );
      GFL_BMPWIN_MakeScreen( callWork->msgWin );
      GFL_BG_LoadScreenReq(CTVT_FRAME_SUB_MSG);
      callWork->isUpdateMsgWin = FALSE;
    }
  }
  if( callWork->isUpdateCallMsgWin == TRUE )
  {
    PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
    if( PRINTSYS_QUE_IsExistTarget( printQue , GFL_BMPWIN_GetBmp( callWork->callMsgWin )) == FALSE )
    {
      GFL_BMPWIN_TransVramCharacter( callWork->callMsgWin );
      GFL_BMPWIN_MakeScreen( callWork->callMsgWin );
      GFL_BG_LoadScreenReq(CTVT_FRAME_SUB_MSG);
      callWork->isUpdateCallMsgWin = FALSE;
      if( callWork->reqDispTimeIcon == TRUE )
      {
        callWork->reqDispTimeIcon = FALSE;
        COMM_TVT_CreateTimerIcon( work , callWork->callMsgWin );
      }
    }
  }

  if( callWork->barMenuWork != NULL )
  {
    APP_TASKMENU_WIN_Update( callWork->barMenuWork );
  }

  return CTM_CALL;
}

//--------------------------------------------------------------
//	TP更新
//--------------------------------------------------------------
static void CTVT_CALL_UpdateTP( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  u32 tpx,tpy;
  u32 holdTpx,holdTpy;
  const BOOL isTrg = GFL_UI_TP_GetPointTrg( &tpx,&tpy );
  const BOOL isHold = GFL_UI_TP_GetPointCont( &holdTpx,&holdTpy );
  CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
  BOOL isCanCheck = TRUE;
  
  if( isTrg == TRUE )
  {
    if( tpx > 8 && tpx < 256-(8*4) && isCanCheck == TRUE )
    {
      const s16 idx = (tpy+callWork->scrollOfs)/CTVT_CALL_BAR_HEIGHT;
      if( idx >= 0 && idx < CTVT_CALL_BAR_NUM )
      {
        const u8 memIdx = callWork->barWork[idx].memberWorkNo;
        CTVT_CALL_MEMBER_WORK *memberWork = &callWork->memberData[memIdx];
        BOOL isUpdate = FALSE;
        
        if( memberWork->isEnable == TRUE &&
            memberWork->connectNum < CTVT_MEMBER_NUM )
        {
          if( memberWork->serviceType == WB_NET_COMM_TVT )
          {
            if( memberWork->isCheck == FALSE )
            {
              callWork->checkIdxParent = memIdx;
              memberWork->isCheck = TRUE;
              memberWork->isBlinkReq = TRUE;
              callWork->isUpdateBarPos = TRUE;
              isUpdate = TRUE;
              PMSND_PlaySystemSE( CTVT_SND_CALLBAR_TOUCH );
            }
            else
            {
              callWork->checkIdxParent = CTVT_CALL_INVALID_NO;
              memberWork->isCheck = FALSE;
              memberWork->isBlinkReq = TRUE;
              callWork->isUpdateBarPos = TRUE;
              isUpdate = TRUE;
              PMSND_PlaySystemSE( CTVT_SND_CALLBAR_CANCEL );
            }
          }
          else
          {
            if( callWork->checkIdxParent == CTVT_CALL_INVALID_NO )
            {
              if( memberWork->isCheck == FALSE )
              {
                u8 i;
                for( i=0;i<3;i++ )
                {
                  if( callWork->checkIdx[i] == CTVT_CALL_INVALID_NO )
                  {
                    callWork->checkIdx[i] = memIdx;
                    break;
                  }
                }
                if( i<3 )
                {
                  memberWork->isCheck = TRUE;
                  memberWork->isBlinkReq = TRUE;
                  callWork->isUpdateBarPos = TRUE;
                  isUpdate = TRUE;
                  PMSND_PlaySystemSE( CTVT_SND_CALLBAR_TOUCH );
                }
              }
              else
              {
                u8 i;
                for( i=0;i<3;i++ )
                {
                  if( callWork->checkIdx[i] == memIdx )
                  {
                    callWork->checkIdx[i] = CTVT_CALL_INVALID_NO;
                    break;
                  }
                }
                memberWork->isCheck = FALSE;
                memberWork->isBlinkReq = TRUE;
                callWork->isUpdateBarPos = TRUE;
                isUpdate = TRUE;
                PMSND_PlaySystemSE( CTVT_SND_CALLBAR_CANCEL );
              }
            }
          }
        }
        
        //文字表示の更新
        if( isUpdate == TRUE )
        {
          CTVT_CALL_UpdateBarMenu( work , callWork );
        }
        
      }
    }
    else
    if( tpx >= CTVT_CALL_SCROLL_X - CTVT_CALL_SCROLL_HIT_WIDTH &&
        tpx <= CTVT_CALL_SCROLL_X + CTVT_CALL_SCROLL_HIT_WIDTH &&
        tpy >= CTVT_CALL_SCROLL_Y - CTVT_CALL_SCROLL_HIT_HEIGHT + callWork->scrollBarPos &&
        tpy <= CTVT_CALL_SCROLL_Y + CTVT_CALL_SCROLL_HIT_HEIGHT + callWork->scrollBarPos )
    {
      if( callWork->barNum > 5 )
      {
        callWork->isHoldScroll = TRUE;
        GFL_CLACT_WK_SetAnmSeq( callWork->clwkScrollBar , CTOAS_SCROLL_BAR_ACTIVE );
        PMSND_PlaySystemSE( CTVT_SND_TOUCH );
      }
    }
  }
  if( isHold == TRUE &&
      callWork->isHoldScroll == TRUE &&
      callWork->barNum > 5 )
  {
    if( holdTpx >= CTVT_CALL_SCROLL_X - CTVT_CALL_SCROLL_HIT_WIDTH &&
        holdTpx <= CTVT_CALL_SCROLL_X + CTVT_CALL_SCROLL_HIT_WIDTH &&
        holdTpy >= CTVT_CALL_SCROLL_Y - CTVT_CALL_SCROLL_HIT_HEIGHT &&
        holdTpy <= CTVT_CALL_SCROLL_Y + CTVT_CALL_SCROLLBAR_LEN + CTVT_CALL_SCROLL_HIT_HEIGHT )
    {
      const u32 maxLen = (callWork->barNum-5) * CTVT_CALL_BAR_HEIGHT;
      s32 scrollOfs = holdTpy - CTVT_CALL_SCROLL_Y;
      GFL_CLACTPOS cellPos;
      if( scrollOfs < 0 )
      {
        scrollOfs = 0;
      }
      else
      if( scrollOfs > CTVT_CALL_SCROLLBAR_LEN )
      {
        scrollOfs = CTVT_CALL_SCROLLBAR_LEN;
      }
      
      callWork->scrollBarPos = scrollOfs;
      cellPos.x = CTVT_CALL_SCROLL_X;
      cellPos.y = CTVT_CALL_SCROLL_Y + callWork->scrollBarPos;
      GFL_CLACT_WK_SetPos( callWork->clwkScrollBar , &cellPos , CLSYS_DRAW_SUB );
      
      callWork->scrollOfs = callWork->scrollBarPos * maxLen / CTVT_CALL_SCROLLBAR_LEN;
      callWork->isUpdateBarPos = TRUE;
    }
  }
  else
  {
    callWork->isHoldScroll = FALSE;
    GFL_CLACT_WK_SetAnmSeq( callWork->clwkScrollBar , CTOAS_SCROLL_BAR );
  }
  
  if( callWork->barMenuWork != NULL )
  {
    //次の行動へ
    if( APP_TASKMENU_WIN_IsTrg( callWork->barMenuWork ) == TRUE )
    {
      APP_TASKMENU_WIN_SetDecide( callWork->barMenuWork , TRUE );
      callWork->state = CCS_WAIT_ANIME;
      PMSND_PlaySystemSE( CTVT_SND_DECIDE );
      if( callWork->barState == CCBS_CALL_CHILD )
      {
        PMSND_PlaySE_byPlayerID( CTVT_SND_TEL_CALL , SEPLAYER_SE2 );
      }
      
    }
  }
  
  {
    //戻るボタン
    static const GFL_UI_TP_HITTBL hitTbl[2] = 
    {
      {
        168 , 192 ,
        CTVT_CALL_RETURN_X , CTVT_CALL_RETURN_X+24 ,
      },
      {GFL_UI_TP_HIT_END,0,0,0}
    };
    const int ret = GFL_UI_TP_HitTrg( hitTbl );
    if( ret == 0 || GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      callWork->state = CCS_WAIT_ANIME;
      callWork->barState = CCBS_CANCEL;
      GFL_CLACT_WK_SetAnmSeq( callWork->clwkReturn , APP_COMMON_BARICON_RETURN_ON );
      PMSND_PlaySystemSE( CTVT_SND_CANCEL );
    }
  }
}

//--------------------------------------------------------------
//	ビーコン更新
//--------------------------------------------------------------
static void CTVT_CALL_UpdateBeacon( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  int i;  //負になるかも
  BOOL isUpdate = FALSE;
  BOOL isUpdateBar = FALSE;
  CTVT_COMM_WORK *commWork = COMM_TVT_GetCommWork( work );
  if( CTVT_COMM_IsInitNet( work , commWork ) == FALSE )
  {
    return;
  }
  
  for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
  {
    if( callWork->memberData[i].isEnable == TRUE )
    {
      callWork->memberData[i].isLost = TRUE;
    }
  }
  
  if( callWork->state != CCS_WAIT_CONNECT_JOIN &&
      callWork->state != CCS_WAIT_CONNECT_CALL &&
      callWork->state != CCS_FAILUE_CONNECT &&
      callWork->state >= CCS_MAIN )
  {
    for( i=0;i<CTVT_COMM_BEACON_NUM;i++ )
    {
      BOOL isEntry = FALSE;
      BOOL isRefresh = FALSE;
      void *beaconData = GFL_NET_GetBeaconData(i);
      if( beaconData != NULL )
      {
        u8 registNo = 0xFF;
        u8 mem;
        u8 *macAddress = GFL_NET_GetBeaconMacAddress( i );
        const GameServiceID serviceType = GFL_NET_WLGetUserGameServiceId( i );
        if( serviceType == WB_NET_FIELDMOVE_SERVICEID )
        {
          //フィールドビーコンをはじく
          GBS_BEACON *becData = beaconData;
          if( becData->beacon_type != GBS_BEACONN_TYPE_INFO )
          {
            continue;
          }
        }
        {
          BOOL isFriend = FALSE;
          //友達かチェック
          if( serviceType == WB_NET_COMM_TVT )
          {
            u8 macAddress[6];
            CTVT_COMM_BEACON *becData = beaconData;
            const STRCODE *name = MyStatus_GetMyName( &becData->myStatus );
            const u32 sex = MyStatus_GetMySex( &becData->myStatus );
            const u32 id  = MyStatus_GetID( &becData->myStatus );
            OS_GetMacAddress( macAddress );

            if( becData->isCalling == 0 ||
                CTVT_COMM_CheckCallingAddress( becData , macAddress ) == TRUE )
            {
              isFriend = CTVT_CALL_CheckRegistFriendData( work , callWork , name , id , sex );
            }
          }
          else
          {
            GBS_BEACON *becData = beaconData;
            
            if( ZONEDATA_IsPalace(becData->info.zone_id) == FALSE &&
                ZONEDATA_IsBingo(becData->info.zone_id) == FALSE )
            {
              const STRCODE *name = becData->info.name;
              const u32 sex = becData->info.sex;
              const u32 id  = becData->trainer_id;
              isFriend = CTVT_CALL_CheckRegistFriendData( work , callWork , name , id , sex );
            }
          }
          if( isFriend == FALSE )
          {
            continue;
          }
        }

        for( mem=0;mem<CTVT_CALL_SEARCH_NUM;mem++ )
        {
          if( callWork->memberData[mem].isEnable == TRUE )
          {
            //すでに登録済みか？
            u8 k;
            BOOL isSame = TRUE;
            for( k=0;k<6;k++ )
            {
              if( macAddress[k] != callWork->memberData[mem].macAddress[k] )
              {
                isSame = FALSE;
                break;
              }
            }
            if( isSame == TRUE )
            {
              //登録済だった
              callWork->memberData[mem].isLost = FALSE;
              isEntry = TRUE;
              //情報が変わっていないか？
              if( serviceType != callWork->memberData[mem].serviceType )
              {
                u8 j;
                isEntry = FALSE;
                isRefresh = TRUE;
                registNo = mem;

                for( j=0;j<3;j++ )
                {
                  if( callWork->checkIdx[j] == mem )
                  {
                    callWork->checkIdx[j] = CTVT_CALL_INVALID_NO;
                    isUpdateBar = TRUE;
                  }
                }
                if( callWork->checkIdxParent == mem )
                {
                  callWork->checkIdxParent = CTVT_CALL_INVALID_NO;
                  isUpdateBar = TRUE;
                }
              }
              if( serviceType == WB_NET_COMM_TVT )
              {
                CTVT_COMM_BEACON *newBecData = beaconData;
                CTVT_COMM_BEACON *becData = callWork->memberData[mem].beacon;
                if( becData->connectNum != newBecData->connectNum )
                {
                  isEntry = FALSE;
                  isRefresh = TRUE;
                  registNo = mem;
                }
              }
              break;
            }
          }
          else
          {
            //空き番号を確保しておく
            if( registNo == 0xFF )
            {
              registNo = mem;
            }
          }
        }
        
        if( isEntry == FALSE )
        {
          //未登録だった
          if( registNo != 0xFF )
          {
            //空きがあったので登録
            //もしくは情報更新
            u8 k;

            callWork->memberData[registNo].isEnable = TRUE;
            callWork->memberData[registNo].isLost = FALSE;
            callWork->memberData[registNo].isCheck = FALSE;
            for( k=0;k<6;k++ )
            {
              callWork->memberData[registNo].macAddress[k] = macAddress[k];
            }
            if( serviceType == WB_NET_COMM_TVT )
            {
              callWork->memberData[registNo].serviceType = WB_NET_COMM_TVT;
              GFL_STD_MemCopy( beaconData , callWork->memberData[registNo].beacon , sizeof(CTVT_COMM_BEACON) );
            }
            else
            {
              callWork->memberData[registNo].serviceType = WB_NET_FIELDMOVE_SERVICEID;
              GFL_STD_MemCopy( beaconData , callWork->memberData[registNo].beacon , sizeof(GBS_BEACON) );
            }
            
            if( isRefresh == FALSE )
            {
              for( k=0;k<CTVT_CALL_BAR_NUM;k++ )
              {
                if( callWork->barWork[k].memberWorkNo == CTVT_CALL_INVALID_NO )
                {
                  callWork->barWork[k].memberWorkNo = registNo;
                  callWork->memberData[registNo].barWorkNo = k;
                  callWork->barWork[k].isUpdate = TRUE;
                  break;
                }
              }
            }
            else
            {
              const u8 workNo = callWork->memberData[registNo].barWorkNo;
              callWork->barWork[ workNo ].isUpdate = TRUE;
            }
            isUpdate = TRUE;
          }
        }
      }
    }
  }
  
  //消える処理
  for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
  {
    if( callWork->memberData[i].isEnable == TRUE &&
        callWork->memberData[i].isLost   == TRUE )
    {
      u8 j;
      for( j=callWork->memberData[i].barWorkNo ; j<CTVT_CALL_BAR_NUM-1 ; j++ )
      {
        if( callWork->barWork[j+1].memberWorkNo != CTVT_CALL_INVALID_NO )
        {
          callWork->barWork[j].memberWorkNo = callWork->barWork[j+1].memberWorkNo;
          callWork->memberData[callWork->barWork[j].memberWorkNo].barWorkNo = j;
          callWork->barWork[j].isUpdate = TRUE;
        }
        else
        {
          //最後までデータが入ったときのために外でjのインデックスで処理する
          break;
        }
      }
      callWork->barWork[j].memberWorkNo = CTVT_CALL_INVALID_NO;
      callWork->barWork[j].isUpdate = TRUE;
      
      callWork->memberData[i].barWorkNo = CTVT_CALL_INVALID_NO;
      callWork->memberData[i].isEnable = FALSE;
      for( j=0;j<3;j++ )
      {
        if( callWork->checkIdx[j] == i )
        {
          callWork->checkIdx[j] = CTVT_CALL_INVALID_NO;
          isUpdateBar = TRUE;
        }
      }
      if( callWork->checkIdxParent == i )
      {
        callWork->checkIdxParent = CTVT_CALL_INVALID_NO;
        isUpdateBar = TRUE;
      }

      i--;
      isUpdate = TRUE;
    }
  }
  
  if( isUpdate == TRUE )
  {
    /*
    u8 i;
    OS_TFPrintf(3,"--------------------------------\n");
    for( i=0;i<CTVT_CALL_SEARCH_NUM;i++ )
    {
      OS_TFPrintf(3,"[%d][%3d][%2x:%2x:%2x:%2x:%2x:%2x]\n",
        callWork->memberData[i].isEnable ,
        callWork->memberData[i].barWorkNo ,
        callWork->memberData[i].macAddress[0] ,
        callWork->memberData[i].macAddress[1] ,
        callWork->memberData[i].macAddress[2] ,
        callWork->memberData[i].macAddress[3] ,
        callWork->memberData[i].macAddress[4] ,
        callWork->memberData[i].macAddress[5] 
        );
    }
    OS_TFPrintf(3,"--------------------------------\n");
    for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
    {
      OS_TFPrintf(3,"[%d]",callWork->barWork[i].memberWorkNo);
    }
    OS_TFPrintf(3,"\n");
    //*/
    CTVT_TPrintf("UpdateBar\n");
    CTVT_CALL_UpdateBar( work , callWork );
    callWork->isUpdateBarPos = TRUE;
    isUpdateBar = TRUE;
  }
  //バーの数が必要なのでここでやる。
  if( isUpdateBar == TRUE &&
      callWork->state == CCS_MAIN )
  {
    CTVT_CALL_UpdateBarMenu( work , callWork );
  }
}
//--------------------------------------------------------------
//	バー更新
//--------------------------------------------------------------
static void CTVT_CALL_UpdateBar( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  u8 i;
  u8 num = 0;
  for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
  {
    CTVT_CALL_UpdateBarFunc( work , callWork , &callWork->barWork[i] , i );
  }
  
  for( i=0;i<CTVT_CALL_BAR_NUM;i++ )
  {
    if( callWork->barWork[i].memberWorkNo != CTVT_CALL_INVALID_NO )
    {
      num++;
    }
  }
  if( callWork->barNum != num )
  {
    if( num > 5 )
    {
      GFL_CLACTPOS cellPos;
      const u32 maxLen = (num-5) * CTVT_CALL_BAR_HEIGHT;
      GFL_CLACT_WK_SetDrawEnable( callWork->clwkScrollBar , TRUE );
      if( callWork->scrollOfs > maxLen )
      {
        callWork->scrollOfs = maxLen;
      }
      callWork->scrollBarPos = callWork->scrollOfs * CTVT_CALL_SCROLLBAR_LEN / maxLen;
      //バー押さえられている時は無視
      if( callWork->isHoldScroll == FALSE )
      {
        cellPos.x = CTVT_CALL_SCROLL_X;
        cellPos.y = CTVT_CALL_SCROLL_Y + callWork->scrollBarPos;
        GFL_CLACT_WK_SetPos( callWork->clwkScrollBar , &cellPos , CLSYS_DRAW_SUB );
      }
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( callWork->clwkScrollBar , FALSE );
      callWork->scrollOfs = 0;
      callWork->isUpdateBarPos = TRUE;
    }
    callWork->barNum = num;
  }
}

static void CTVT_CALL_UpdateBarFunc( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , CTVT_CALL_BAR_WORK *barWork , const u8 idx )
{
  if( barWork->isUpdate == TRUE )
  {
    if( barWork->memberWorkNo!= CTVT_CALL_INVALID_NO )
    {
      const HEAPID heapId = COMM_TVT_GetHeapId( work );
      GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
      GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
      PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
      CTVT_CALL_MEMBER_WORK *memberWork = &callWork->memberData[barWork->memberWorkNo];
      WORDSET *wordSet = WORDSET_Create( heapId );
      STRBUF *str;
      
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( barWork->strWin ) , 0x0 );
      
      //名前
      str = GFL_STR_CreateBuffer( 32 , heapId );
      if( memberWork->serviceType == WB_NET_COMM_TVT )
      {
        CTVT_COMM_BEACON *becData = memberWork->beacon;
        GFL_STR_SetStringCodeOrderLength( str , CTVT_BCON_GetName(becData) , CTVT_COMM_NAME_LEN );
      }
      else
      {
        GBS_BEACON *becData = memberWork->beacon;
        GFL_STR_SetStringCodeOrderLength( str , becData->info.name , CTVT_COMM_NAME_LEN );
      }
      PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( barWork->strWin ) , 
              6 , 0 , str , fontHandle ,CTVT_FONT_COLOR_WHITE );
      GFL_STR_DeleteBuffer( str );

      //ID
      str = GFL_MSG_CreateString( msgHandle , COMM_TVT_CALL_01 );
      {
        STRBUF *tempStr = GFL_STR_CreateBuffer( 32 , heapId );
        if( memberWork->serviceType == WB_NET_COMM_TVT )
        {
          CTVT_COMM_BEACON *becData = memberWork->beacon;
          WORDSET_RegisterNumber( wordSet , 0 , CTVT_BCON_GetIDLow(becData) , 5 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
        }
        else
        {
          GBS_BEACON *becData = memberWork->beacon;
          u16 id = (becData->info.trainer_id & 0xFFFF);
          WORDSET_RegisterNumber( wordSet , 0 , id , 5 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
        }
        WORDSET_ExpandStr( wordSet , tempStr , str );
        GFL_STR_DeleteBuffer( str );
        str = tempStr;
      }
      PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( barWork->strWin ) , 
              80 , 0 , str , fontHandle ,CTVT_FONT_COLOR_WHITE );
      GFL_STR_DeleteBuffer( str );

      //人数
      if( memberWork->serviceType == WB_NET_COMM_TVT )
      {
        CTVT_COMM_BEACON *becData = memberWork->beacon;
        memberWork->connectNum = becData->connectNum;
        str = GFL_MSG_CreateString( msgHandle , COMM_TVT_CALL_03 );
        {
          STRBUF *tempStr = GFL_STR_CreateBuffer( 32 , heapId );
          WORDSET_RegisterNumber( wordSet , 0 , becData->connectNum , 1 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( wordSet , tempStr , str );
          GFL_STR_DeleteBuffer( str );
          str = tempStr;
        }
        PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( barWork->strWin ) , 
                144 , 0 , str , fontHandle ,CTVT_FONT_COLOR_WHITE );
        GFL_STR_DeleteBuffer( str );
        
      }
      else
      {
        memberWork->connectNum = 0;
        //str = GFL_MSG_CreateString( msgHandle , COMM_TVT_CALL_02 );
      }

      //カメラ
      if( memberWork->serviceType == WB_NET_COMM_TVT )
      {
        CTVT_COMM_BEACON *becData = memberWork->beacon;
        if( becData->canUseCamera == 1 )
        {
          memberWork->isCamera = TRUE;
        }
        else
        {
          memberWork->isCamera = FALSE;
        }
      }
      else
      {
        GBS_BEACON *becData = memberWork->beacon;
        if( becData->isTwl == TRUE &&
            becData->restrictPhoto == FALSE )
        {
          memberWork->isCamera = TRUE;
        }
        else
        {
          memberWork->isCamera = FALSE;
        }
      }
      
      
      WORDSET_Delete( wordSet );
      barWork->isUpdateStr = TRUE;
    }
    else
    {
      GFL_BMPWIN_ClearTransWindow_VBlank( barWork->strWin );
    }
    barWork->isUpdate = FALSE;
  }
}

static void CTVT_CALL_UpdateBarPosFunc( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , CTVT_CALL_BAR_WORK *barWork , const u8 idx )
{
  if( barWork->memberWorkNo == CTVT_CALL_INVALID_NO )
  {
    GFL_CLACT_WK_SetDrawEnable( barWork->clwkBar , FALSE );
    GFL_CLACT_WK_SetDrawEnable( barWork->clwkCheck , FALSE );
    GFL_CLACT_WK_SetDrawEnable( barWork->clwkCamera , FALSE );
  }
  else
  {
    s16 posY = idx*CTVT_CALL_BAR_HEIGHT - callWork->scrollOfs;
    if( posY >= -CTVT_CALL_BAR_HEIGHT &&
        posY <= 192-CTVT_CALL_BAR_HEIGHT )
    {
      GFL_CLACTPOS pos;
      pos.x = CTVT_CALL_BAR_X;
      pos.y = CTVT_CALL_BAR_Y + posY;
      GFL_CLACT_WK_SetPos( barWork->clwkBar , &pos , CLSYS_DRAW_SUB );
      GFL_CLACT_WK_SetDrawEnable( barWork->clwkBar , TRUE );
      pos.x = CTVT_CALL_CHECK_X;
      GFL_CLACT_WK_SetPos( barWork->clwkCheck , &pos , CLSYS_DRAW_SUB );
      GFL_CLACT_WK_SetDrawEnable( barWork->clwkCheck , TRUE );
      pos.x = CTVT_CALL_CAMERA_X;
      GFL_CLACT_WK_SetPos( barWork->clwkCamera , &pos , CLSYS_DRAW_SUB );

      if( callWork->memberData[ barWork->memberWorkNo ].isCamera == TRUE )
      {
        GFL_CLACT_WK_SetAnmSeq( callWork->barWork[idx].clwkCamera , CTOAS_CAMERA_ON );
        GFL_CLACT_WK_SetDrawEnable( barWork->clwkCamera , TRUE );
      }
      else
      {
        GFL_CLACT_WK_SetDrawEnable( barWork->clwkCamera , FALSE );
      }
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( barWork->clwkBar , FALSE );
      GFL_CLACT_WK_SetDrawEnable( barWork->clwkCheck , FALSE );
      GFL_CLACT_WK_SetDrawEnable( barWork->clwkCamera , FALSE );
    }
    
  }
}
//--------------------------------------------------------------
//	バーメニュー更新
//--------------------------------------------------------------
static void CTVT_CALL_UpdateBarMenu( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{

  u8 i;
  u8 checkNum = 0;
  const HEAPID heapId = COMM_TVT_GetHeapId( work );
  GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
  APP_TASKMENU_RES *taskRes = COMM_TVT_GetTaskMenuRes( work );
  for( i=0;i<3;i++ )
  {
    if( callWork->checkIdx[i] != CTVT_CALL_INVALID_NO )
    {
      checkNum++;
    }
  }
  
  if( callWork->checkIdxParent != CTVT_CALL_INVALID_NO )
  {
    if( callWork->barState != CCBS_JOIN_PARENT )
    {
      APP_TASKMENU_ITEMWORK initWork = {0};

      GFL_BMPWIN_ClearTransWindow_VBlank( callWork->msgWin );
      BmpWinFrame_Clear( callWork->msgWin , WINDOW_TRANS_ON_V );

      if( callWork->barMenuWork != NULL )
      {
        APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
        callWork->barMenuWork = NULL;
      }
      
      initWork.str = GFL_MSG_CreateString( msgHandle , COMM_TVT_CALL_06 );
      initWork.msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
      initWork.type = APP_TASKMENU_WIN_TYPE_NORMAL;
      
      callWork->barMenuWork = APP_TASKMENU_WIN_Create( taskRes , &initWork , 0 , 21 , 21 , heapId );
      
      GFL_STR_DeleteBuffer( initWork.str );

      callWork->barState = CCBS_JOIN_PARENT;
    }
  }
  else
  if( checkNum > 0 )
  {
    if( callWork->barState != CCBS_CALL_CHILD )
    {
      APP_TASKMENU_ITEMWORK initWork = {0};

      GFL_BMPWIN_ClearTransWindow_VBlank( callWork->msgWin );
      BmpWinFrame_Clear( callWork->msgWin , WINDOW_TRANS_ON_V );

      if( callWork->barMenuWork != NULL )
      {
        APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
        callWork->barMenuWork = NULL;
      }
      
      initWork.str = GFL_MSG_CreateString( msgHandle , COMM_TVT_CALL_05 );
      initWork.msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
      initWork.type = APP_TASKMENU_WIN_TYPE_NORMAL;
      
      callWork->barMenuWork = APP_TASKMENU_WIN_Create( taskRes , &initWork , 0 , 21 , 21 , heapId );
      
      GFL_STR_DeleteBuffer( initWork.str );

      callWork->barState = CCBS_CALL_CHILD;
    }
  }
  else
  {
    if( callWork->barState != CCBS_NONE )
    {
      if( callWork->barMenuWork != NULL )
      {
        APP_TASKMENU_WIN_Delete( callWork->barMenuWork );
        callWork->barMenuWork = NULL;
      }
      
      if( callWork->barNum > 0 )
      {
        CTVT_CALL_DispMessage( work , callWork , COMM_TVT_CALL_04 );
      }
      callWork->barState = CCBS_NONE;
    }
  }  
}

//誰も居ない時Msg更新
static void CTVT_CALL_UpdateNoMemberMsg( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork )
{
  if( callWork->barNum != callWork->befbarNum )
  {
    if( callWork->barNum == 0 )
    {
      CTVT_CALL_DispCallMessage( work , callWork , COMM_TVT_CALL_15 );
      callWork->reqDispTimeIcon = TRUE;

      GFL_BMPWIN_ClearTransWindow_VBlank( callWork->msgWin );
      BmpWinFrame_Clear( callWork->msgWin , WINDOW_TRANS_ON_V );
    }
    else
    if( callWork->befbarNum == 0 &&
        callWork->barNum > 0 )
    {
      CTVT_CALL_DispMessage( work , callWork , COMM_TVT_CALL_04 );

      COMM_TVT_DeleteTimerIcon( work );
      BmpWinFrame_Clear( callWork->callMsgWin , WINDOW_TRANS_ON_V );
      GFL_BMPWIN_ClearTransWindow( callWork->callMsgWin );
    }
    
    callWork->befbarNum = callWork->barNum;
  }
}

static const BOOL CTVT_CALL_CheckRegistFriendData( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const STRCODE *name , const u32 id , const u32 sex )
{
  u8 i;
  const COMM_TVT_INIT_WORK *initWork = COMM_TVT_GetInitWork( work );
  WIFI_LIST *wifiList = GAMEDATA_GetWiFiList( initWork->gameData );
  
#if PM_DEBUG
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    return TRUE;
  }
#endif
  
  return WifiList_CheckFriendData( wifiList , name , id , sex, NULL );
}


static void CTVT_CALL_DispMessage( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const u16 msgId )
{
  GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
  GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
  PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
  STRBUF *str;
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( callWork->msgWin) , 0xF );
  str = GFL_MSG_CreateString( msgHandle , msgId );
  PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( callWork->msgWin ) , 
          0 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
  GFL_STR_DeleteBuffer( str );

  BmpWinFrame_Write( callWork->msgWin , WINDOW_TRANS_OFF , 
                      CTVT_BMPWIN_CGX , CTVT_PAL_BG_SUB_WINFRAME );
  callWork->isUpdateMsgWin = TRUE;

}


static void CTVT_CALL_DispCallMessage( COMM_TVT_WORK *work , CTVT_CALL_WORK *callWork , const u16 msgId )
{
  GFL_FONT *fontHandle = COMM_TVT_GetFontHandle( work );
  GFL_MSGDATA *msgHandle = COMM_TVT_GetMegHandle( work );
  PRINT_QUE *printQue = COMM_TVT_GetPrintQue( work );
  STRBUF *str;
  
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( callWork->callMsgWin) , 0xF );
  str = GFL_MSG_CreateString( msgHandle , msgId );
  PRINTSYS_PrintQueColor( printQue , GFL_BMPWIN_GetBmp( callWork->callMsgWin ) , 
          0 , 0 , str , fontHandle ,CTVT_FONT_COLOR_BLACK );
  GFL_STR_DeleteBuffer( str );

  BmpWinFrame_Write( callWork->callMsgWin , WINDOW_TRANS_OFF , 
                      CTVT_BMPWIN_CGX , CTVT_PAL_BG_SUB_WINFRAME );
  callWork->isUpdateCallMsgWin = TRUE;

  callWork->reqDispTimeIcon = FALSE;
  COMM_TVT_DeleteTimerIcon( work );

}

