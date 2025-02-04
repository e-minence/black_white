//======================================================================
/**
 * @file	mb_child_sys.h
 * @brief	マルチブート・子機メイン
 * @author	ariizumi
 * @data	09/11/16
 *
 * モジュール名：MB_CHILD
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/wipe.h"
#include "system/net_err.h"
#include "poke_tool/pokepara_conv.h"

#include "arc_def.h"
#include "mb_child_gra.naix"
#include "multiboot/wb_sound_palpark.sadl"
#include "sound/pm_sndsys.h"


#include "multiboot/mb_child_sys.h"
#include "multiboot/mb_select_sys.h"
#include "multiboot/mb_capture_sys.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/mb_util.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_data_main.h"
#include "multiboot/mb_local_def.h"

#include "../../test/ariizumi/ari_debug.h"


//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CHILD_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_CHILD_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_CHILD_FRAME_SUB_MSG  (GFL_BG_FRAME1_S)
#define MB_CHILD_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define MB_CHILD_FIRST_TIMEOUT (60*30) //通常5秒以内で接続するのができなかった。

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCS_FADEIN,
  MCS_WAIT_FADEIN,
  MCS_FADEOUT,
  MCS_WAIT_FADEOUT,
  
  MCS_WAIT_COMM_INIT,
  MCS_WAIT_CONNECT,
  MCS_CHECK_ROM,
  MCS_SELECT_ROM, //デバッグ用
  MCS_LOAD_DATA,
  MCS_POKE_CONV,
  MCS_DATA_CONV,

  MCS_LOAD_ERROR,
  MCS_POKE_LACK,  //初期ポケ不足
  
  //選択画面
  MCS_SELECT_FADEOUT,
  MCS_SELECT_FADEOUT_WAIT,
  MCS_SELECT_MAIN,
  MCS_SELECT_TERM,
  MCS_SELECT_FADEIN,
  MCS_SELECT_FADEIN_WAIT,

  //ゲームデータ到着待ち
  MCS_WAIT_GAME_DATA,
  
  //捕獲ゲーム
  MCS_CAPTURE_FADEOUT,
  MCS_CAPTURE_FADEOUT_WAIT,
  MCS_CAPTURE_INIT,
  MCS_CAPTURE_MAIN,
  MCS_CAPTURE_TERM,
  MCS_CAPTURE_FADEIN,
  MCS_CAPTURE_FADEIN_WAIT,

  MCS_CHECK_ROM_CRC_CODE,
  MCS_CHECK_ROM_CRC_LOAD,
  MCS_CHECK_ROM_CRC,

  //転送・保存
  MCS_TRANS_CONFIRM_INIT,
  MCS_TRANS_CONFIRM_WAIT,
  MCS_TRANS_CONFIRM_MAIN,
  MCS_TRANS_CONFIRM_NO_MSG,
  
  MCS_TRANS_POKE_INIT,
  MCS_TRANS_POKE_SEND,
  MCS_TRANS_POKE_SEND_WAIT,

  MCS_SAVE_INIT,
  MCS_SAVE_MAIN,
  MCS_SAVE_TERM,

  MCS_SAVE_FINISH_WAIT,
  MCS_DIPS_NEXT_GAME_CONFIRM,
  MCS_WAIT_NEXT_GAME_CONFIRM,
  MCS_SELECT_NEXT_GAME_CONFIRM,

  MCS_WAIT_NEXT_GAME_ERROR_MSG,

  MCS_WAIT_EXIT_COMM,
  MCS_EXIT_GAME,
  
  MCS_ERROR,
  
}MB_CHILD_STATE;

typedef enum
{
  MCSS_SAVE_WAIT_SAVE_INIT,
  MCSS_SAVE_SYNC_SAVE_INIT,
  MCSS_SAVE_INIT,
  MCSS_SAVE_WAIT_FIRST,       //ここ移行のステートでセーブメインが動く
  MCSS_SAVE_WAIT_FIRST_SYNC,
  MCSS_SAVE_FIRST_SAVE_LAST,
  MCSS_SAVE_WAIT_SECOND,
  MCSS_SAVE_WAIT_SECOND_SYNC,
  MCSS_SAVE_SECOND_SAVE_LAST,
  MCSS_SAVE_WAIT_LAST_SAVE,
  MCSS_SAVE_WAIT_FINISH_SAVE_SYNC,
}MB_CHILD_SUB_STATE;
  
//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  
  MB_COMM_INIT_DATA *initData;
  void  *sndData;
  
  MB_CHILD_STATE  state;
  MB_CHILD_SUB_STATE  subState;
  GFL_PROCSYS   *procSys;
  u16 convCnt;
  u8 captureNum;
  u8 saveWaitCnt;
  u16 timeoutCnt;
  u16 timeoutCnt2;
  
  POKEMON_PASO_PARAM *boxPoke[MB_POKE_BOX_TRAY][MB_POKE_BOX_POKE];
  STRCODE *boxName[MB_POKE_BOX_TRAY];
  
  DLPLAY_CARD_TYPE cardType;

  MB_MSG_WORK *msgWork;
  MB_COMM_WORK *commWork;
  MB_DATA_WORK *dataWork;
  
  MB_SELECT_INIT_WORK selInitWork;
  MB_CAPTURE_INIT_WORK capInitWork;
  
  BOOL isNetErr;
  BOOL isInitCellSys;
  BOOL isSave;
  
}MB_CHILD_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MB_CHILD_Init( MB_CHILD_WORK *work );
static void MB_CHILD_Term( MB_CHILD_WORK *work );
static const BOOL MB_CHILD_Main( MB_CHILD_WORK *work );
static void MB_CHILD_VBlankFunc(GFL_TCB *tcb, void *wk );
static void MB_CHILD_VSync( void *pWork );

static void MB_CHILD_InitGraphic( MB_CHILD_WORK *work );
static void MB_CHILD_TermGraphic( MB_CHILD_WORK *work );
static void MB_CHILD_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_CHILD_LoadResource( MB_CHILD_WORK *work );

static void MB_CHILD_DataConvert( MB_CHILD_WORK *work );
static const u16 MB_CHILD_GetLeastPoke( MB_CHILD_WORK *work );
static const BOOL MB_CHILD_ErrCheck( MB_CHILD_WORK *work , const BOOL noFade );

static void MB_CHILD_SaveInit( MB_CHILD_WORK *work );
static void MB_CHILD_SaveTerm( MB_CHILD_WORK *work );
static void MB_CHILD_SaveMain( MB_CHILD_WORK *work );

static void MB_CHILD_DEBUG_ScreenDraw( const u8 idx );


static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,             // メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,       // メイン2DエンジンのBG拡張パレット
  GX_VRAM_SUB_BG_128_C,         // サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
  GX_VRAM_OBJ_128_B ,           // メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,      // メイン2DエンジンのOBJ拡張パレット
  GX_VRAM_SUB_OBJ_128_D,        // サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
  GX_VRAM_TEX_NONE,             // テクスチャイメージスロット
  GX_VRAM_TEXPLTT_NONE,         // テクスチャパレットスロット
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
static void MB_CHILD_Init( MB_CHILD_WORK *work )
{
  u8 i,j;
  work->state = MCS_FADEIN;
  work->dataWork = NULL;
  work->isInitCellSys = FALSE;

  MB_CHILD_InitGraphic( work );
  MB_CHILD_LoadResource( work );
  work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_SUB_MSG , MB_CHILD_FRAME_SUB_MSG , FILE_MSGID_MB , TRUE , FALSE );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  work->commWork = MB_COMM_CreateSystem( work->heapId );
  
  work->sndData = GFL_ARC_UTIL_Load( ARCID_MB_CHILD ,
                                     NARC_mb_child_gra_wb_sound_palpark_sdat ,
                                     FALSE ,
                                     work->heapId );
                                     
  work->isNetErr = FALSE;
  work->isSave = FALSE;
  PMSND_InitMultiBoot( work->sndData );
  
  work->procSys = GFL_PROC_LOCAL_boot( work->heapId );
  work->captureNum = 0;
  work->timeoutCnt = 0;
  work->timeoutCnt2 = 0;
  work->initData = NULL;
  
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      work->boxPoke[i][j] = GFL_HEAP_AllocClearMemory( work->heapId , POKETOOL_GetPPPWorkSize() );
    }
    work->boxName[i] = GFL_HEAP_AllocClearMemory( work->heapId , 20 );  //8文字+EOM
  }

  work->vBlankTcb = GFUser_VIntr_CreateTCB( MB_CHILD_VBlankFunc , work , 8 );

  GFUser_SetVIntrFunc( MB_CHILD_VSync , NULL );

  PMSND_PlayBGM( SEQ_BGM_PALPARK_BOX );
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static void MB_CHILD_Term( MB_CHILD_WORK *work )
{
  u8 i,j;
  GFUser_ResetVIntrFunc();
  GFL_TCB_DeleteTask( work->vBlankTcb );

  if( work->isSave == TRUE )
  {
    MB_DATA_CancelSave( work->dataWork );
  }

  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    GFL_HEAP_FreeMemory( work->boxName[i] );
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      GFL_HEAP_FreeMemory( work->boxPoke[i][j] );
    }
  }

  GFL_PROC_LOCAL_Exit( work->procSys );
  
  PMSND_Exit();
  GFL_HEAP_FreeMemory( work->sndData );
  MB_COMM_DeleteSystem( work->commWork );

  if( work->dataWork != NULL )
  {
    MB_DATA_TermSystem( work->dataWork );
  }

  if( work->msgWork != NULL )
  {
    MB_MSG_MessageTerm( work->msgWork );
  }
  MB_CHILD_TermGraphic( work );
}

//--------------------------------------------------------------
//  更新
//--------------------------------------------------------------
static const BOOL MB_CHILD_Main( MB_CHILD_WORK *work )
{

  MB_COMM_UpdateSystem( work->commWork );
  
  #if PM_DEBUG
  {
    static u8 befState = 0xFF;
    if( befState != work->state )
    {
      MB_TPrintf("state[%d]->[%d]\n",befState,work->state);
      befState = work->state;
    }
  }
  #endif
  
  switch( work->state )
  {
  case MCS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    //PMSND_PlayBGM( SEQ_BGM_PALPARK_BOX );
    work->state = MCS_WAIT_FADEIN;
    break;
    
  case MCS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      MB_COMM_InitComm( work->commWork );
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_01 );
      MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
      work->state = MCS_WAIT_COMM_INIT;
    }
    break;
  case MCS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_WAIT_FADEOUT;
    break;
  case MCS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  //--------------------------------------------------------
  //起動〜チェック
  case MCS_WAIT_COMM_INIT:
    if( MB_COMM_IsInitComm(work->commWork) == TRUE )
    {
      //親機情報
      if( MB_IsMultiBootChild() == FALSE )
      {
        GF_ASSERT_MSG_HEAVY(0,"This DS is not multiboot child!!\n");
      }
      else
      {
        const MBParentBssDesc *desc = MB_GetMultiBootParentBssDesc();
        MB_COMM_InitChild( work->commWork , (u8*)desc->bssid );
      }
      
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_01 );
      MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
      work->state = MCS_WAIT_CONNECT;
    }
    break;
  
  case MCS_WAIT_CONNECT:
    if( MB_COMM_IsSendEnable( work->commWork ) == FALSE )
    {
      work->timeoutCnt++;
    }
    else
    {
      work->timeoutCnt2++;
    }
    if( work->timeoutCnt >= MB_CHILD_FIRST_TIMEOUT )
    {
      //初期接続タイムアウト
      //MsgSpeedが受信できてない！
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_11 , 1 );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    else
    if( work->timeoutCnt2 >= MB_CHILD_FIRST_TIMEOUT )
    {
      //なぜかカード抜きでタイムアウト・・・
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_11 , 1 );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    else
    if( MB_CHILD_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_COMM_IsPostInitData( work->commWork ) == TRUE )
      {
        work->initData = MB_COMM_GetInitData( work->commWork );
        GFL_MSGSYS_SetLangID( work->initData->strMode );
        work->state = MCS_CHECK_ROM;
        MB_CHILD_DEBUG_ScreenDraw(0);
      }
    }
    break;
  case MCS_CHECK_ROM:
    work->dataWork = MB_DATA_InitSystem( work->heapId );
    work->cardType = MB_DATA_GetCardType( work->dataWork );
    MB_DATA_ResetSaveLoad( work->dataWork );
#if PM_DEBUG
    if( work->cardType == CARD_TYPE_DUMMY )
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_DEB_01 );
      work->state = MCS_SELECT_ROM;
    }
    else
#endif
    if( work->cardType == CARD_TYPE_INVALID )
    {
      //ROM違う！
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_02 , work->initData->msgSpeed );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    else
    {
      work->state = MCS_LOAD_DATA;
      MB_CHILD_DEBUG_ScreenDraw(2);
    }
    break;
  
  //デバッグ用ROM種類選択
  case MCS_SELECT_ROM:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_DP );
      work->state = MCS_LOAD_DATA;
      //work->cardType = CARD_TYPE_DP;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_PT );
      work->state = MCS_LOAD_DATA;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      MB_DATA_SetCardType( work->dataWork , CARD_TYPE_GS );
      work->state = MCS_LOAD_DATA;
    }
    break;
  
  case MCS_LOAD_DATA:
    if( MB_DATA_LoadDataFirst( work->dataWork ) == TRUE )
    {
      MB_CHILD_DEBUG_ScreenDraw(4);
      if( MB_DATA_GetErrorState( work->dataWork ) == DES_NONE )
      {
        work->state = MCS_POKE_CONV;
        work->convCnt = 0;
      }
      else
      {
        work->state = MCS_LOAD_ERROR;
      }
    }
    break;

  case MCS_POKE_CONV:
    {
      //PPP
      const u8 i = work->convCnt/MB_POKE_BOX_POKE;
      const u8 j = work->convCnt%MB_POKE_BOX_POKE;
      void *pppSrc = MB_DATA_GetBoxPPP(work->dataWork,i,j);
      MB_UTIL_ConvertPPP( pppSrc , work->boxPoke[i][j] , work->cardType );

      work->convCnt++;
      if( work->convCnt >= MB_POKE_BOX_TRAY * MB_POKE_BOX_POKE )
      {
        work->state = MCS_DATA_CONV;
      }
      MB_CHILD_DEBUG_ScreenDraw(6);
    }
   
    break;

  case MCS_DATA_CONV:
    MB_CHILD_DataConvert( work );
    if( MB_CHILD_GetLeastPoke( work ) < MB_CAP_POKE_NUM )
    {
      work->state = MCS_POKE_LACK;
    }
    else
    {
      work->state = MCS_SELECT_FADEOUT;
    }
    MB_CHILD_DEBUG_ScreenDraw(8);
    MB_CHILD_ErrCheck( work , FALSE );
    break;
    
  case MCS_LOAD_ERROR:
    {
      u8 errType = MB_DATA_GetErrorState( work->dataWork );
      MB_CHILD_DEBUG_ScreenDraw(32 + errType );
      switch( errType )
      {
      case DES_MISS_LOAD_BACKUP:
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_15 , work->initData->msgSpeed );
        break;
      case DES_ANOTHER_COUNTRY:
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_16 , work->initData->msgSpeed );
        break;
      default:
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_10 , work->initData->msgSpeed );
        break;
      }

      //読み込み失敗
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
      work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    }
    break;
    
  case MCS_POKE_LACK:
    //初期ポケ不足
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_09 , work->initData->msgSpeed );
    MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
    MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
    work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
    break;
  
  //--------------------------------------------------------
  //ボックスセレクト画面
  case MCS_SELECT_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_SELECT_FADEOUT_WAIT;
    MB_COMM_SetChildState( work->commWork , MCCS_SELECT_BOX );
    break;
    
  case MCS_SELECT_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i,j;
      GFUser_ResetVIntrFunc();
      MB_MSG_MessageTerm( work->msgWork );
      MB_CHILD_TermGraphic( work );
      work->msgWork = NULL;
      
      //InitWorkのpppは最初に入れている
      work->selInitWork.parentHeap = work->heapId;
      work->selInitWork.cardType = work->cardType;
      work->selInitWork.dataWork = work->dataWork;
      work->selInitWork.msgSpeed = work->initData->msgSpeed;
      for( i=0;i<MB_POKE_BOX_TRAY;i++ )
      {
        for( j=0;j<MB_POKE_BOX_POKE;j++ )
        {
          work->selInitWork.boxData[i][j] = work->boxPoke[i][j];
        }
        work->selInitWork.boxName[i] = work->boxName[i];
      }
      
      GFL_PROC_LOCAL_CallProc( work->procSys , 
                               NO_OVERLAY_ID ,
                               &MultiBootSelect_ProcData ,
                               &work->selInitWork );
      
      work->state = MCS_SELECT_MAIN;
    }
    break;
    
  case MCS_SELECT_MAIN:
    {
      const GFL_PROC_MAIN_STATUS ret = GFL_PROC_LOCAL_Main( work->procSys );
      if( ret == GFL_PROC_MAIN_NULL )
      {
        work->state = MCS_SELECT_TERM;
      }
    }
    break;
    
  case MCS_SELECT_TERM:
    if( MB_COMM_IsPostGameData( work->commWork ) == TRUE &&
        work->selInitWork.isCancel == FALSE )
    {
      //転送も終わってキャンセルじゃないのでいきなりゲーム
      work->state = MCS_CAPTURE_INIT;
      MB_COMM_SetChildState( work->commWork , MCCS_CAP_GAME );
      if( MB_CHILD_ErrCheck( work , TRUE ) == TRUE )
      {
        //開放が入るから初期化
        MB_CHILD_InitGraphic( work );
        MB_CHILD_LoadResource( work );
      }
    }
    else
    {
      work->state = MCS_SELECT_FADEIN;

      MB_CHILD_InitGraphic( work );
      MB_CHILD_LoadResource( work );
      work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_SUB_MSG , MB_CHILD_FRAME_SUB_MSG , FILE_MSGID_MB , FALSE , FALSE );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      if( work->selInitWork.isCancel == FALSE )
      {
        MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_03 );
        MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
      }
      else
      {
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_07 , work->initData->msgSpeed );
      }
      GFUser_SetVIntrFunc( MB_CHILD_VSync , NULL );
      MB_CHILD_ErrCheck( work , TRUE );
    }
    break;
    
  case MCS_SELECT_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_SELECT_FADEIN_WAIT;
    break;

  case MCS_SELECT_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( work->selInitWork.isCancel == TRUE )
      {
        //キャンセルされた
        MB_COMM_SetChildState( work->commWork , MCCS_CANCEL_BOX );
        MB_COMM_ReqDisconnect( work->commWork );
        work->state = MCS_WAIT_EXIT_COMM;
      }
      else
      {
        if( MB_COMM_IsPostGameData( work->commWork ) == TRUE )
        {
          work->state = MCS_CAPTURE_FADEOUT;
        }
        else
        {
          work->state = MCS_WAIT_GAME_DATA;
          MB_COMM_SetChildState( work->commWork , MCCS_WAIT_GAME_DATA );
        }
      }
    }
    break;
  //--------------------------------------------------------
  //ゲームデータ到着待ち
    
  case MCS_WAIT_GAME_DATA:
    if( MB_COMM_IsPostGameData( work->commWork ) == TRUE )
    {
      work->state = MCS_CAPTURE_FADEOUT;
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;

  //--------------------------------------------------------
  //捕獲ゲーム
  case MCS_CAPTURE_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_CAPTURE_FADEOUT_WAIT;
    MB_COMM_SetChildState( work->commWork , MCCS_CAP_GAME );
    break;
    
  case MCS_CAPTURE_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      GFUser_ResetVIntrFunc();
      MB_MSG_MessageTerm( work->msgWork );
      MB_CHILD_TermGraphic( work );

      work->state = MCS_CAPTURE_INIT;
    }

    break;
  case MCS_CAPTURE_INIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i;
      MB_COMM_INIT_DATA *commInit = MB_COMM_GetInitData( work->commWork );
      PMSND_StopBGM();
      PMSND_Exit();

      work->msgWork = NULL;
      
      work->capInitWork.parentHeap = work->heapId;
      work->capInitWork.cardType = work->cardType;
      work->capInitWork.arcHandle = GFL_ARC_OpenDataHandleByMemory( 
                                              GFL_NET_LDATA_GetPostData( 0 ) ,
                                              GFL_NET_LDATA_GetPostDataSize( 0 ) ,
                                              work->heapId );
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        const u8 tray = work->selInitWork.selectPoke[i][0];
        const u8 idx  = work->selInitWork.selectPoke[i][1];
        work->capInitWork.ppp[i] = work->boxPoke[tray][idx];
      }
      work->capInitWork.highScore = commInit->highScore;

      GFL_PROC_LOCAL_CallProc( work->procSys , 
                               NO_OVERLAY_ID ,
                               &MultiBootCapture_ProcData ,
                               &work->capInitWork );
      
      work->state = MCS_CAPTURE_MAIN;
    }
    break;
    
  case MCS_CAPTURE_MAIN:
    {
      const GFL_PROC_MAIN_STATUS ret = GFL_PROC_LOCAL_Main( work->procSys );
      if( ret == GFL_PROC_MAIN_NULL )
      {
        work->state = MCS_CAPTURE_TERM;
      }
    }
    break;
    
  case MCS_CAPTURE_TERM:
    work->state = MCS_CAPTURE_FADEIN;

    GFL_ARC_CloseDataHandle( work->capInitWork.arcHandle );
    PMSND_InitMultiBoot( work->sndData );
    MB_CHILD_InitGraphic( work );
    MB_CHILD_LoadResource( work );
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_CHILD_FRAME_SUB_MSG , MB_CHILD_FRAME_SUB_MSG , FILE_MSGID_MB , FALSE , FALSE );

    GFUser_SetVIntrFunc( MB_CHILD_VSync , NULL );
    MB_CHILD_ErrCheck( work , TRUE );

    PMSND_PlayBGM( SEQ_BGM_PALPARK_BOX );
    break;
  
  case MCS_CAPTURE_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MCS_CAPTURE_FADEIN_WAIT;
    break;

  case MCS_CAPTURE_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      u8 i;
      work->captureNum = 0;
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        if( work->capInitWork.isCapture[i] == TRUE )
        {
          work->captureNum++;
        }
      }
      if( work->captureNum > 0 )
      {
        work->state = MCS_TRANS_CONFIRM_INIT;
      }
      else
      {
        work->state = MCS_DIPS_NEXT_GAME_CONFIRM;
        MB_COMM_SetChildState( work->commWork , MCCS_NEXT_GAME );
      }
    }
    break;
  //--------------------------------------------------------
  //転送・保存フェイズ
  //確認
  case MCS_TRANS_CONFIRM_INIT:
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE_UP );
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_13 , work->initData->msgSpeed );
    work->state = MCS_TRANS_CONFIRM_WAIT;
    break;
  
  case MCS_TRANS_CONFIRM_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      MB_MSG_DispYesNo( work->msgWork , MMYT_UP );
      work->state = MCS_TRANS_CONFIRM_MAIN;
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;
    
  case MCS_TRANS_CONFIRM_MAIN:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNo( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        MB_COMM_SetChildState( work->commWork , MCCS_SEND_POKE );
        work->state = MCS_TRANS_POKE_INIT;
        MB_MSG_ClearYesNo( work->msgWork );
        MB_MSG_MessageHide( work->msgWork );
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->captureNum = 0;
        work->state = MCS_TRANS_CONFIRM_NO_MSG;
        MB_COMM_SetChildState( work->commWork , MCCS_NEXT_GAME );
        MB_MSG_ClearYesNo( work->msgWork );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_14 , work->initData->msgSpeed );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      }
      MB_CHILD_ErrCheck( work , FALSE );
    }
    break;
  case MCS_TRANS_CONFIRM_NO_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      work->state = MCS_DIPS_NEXT_GAME_CONFIRM;
      MB_MSG_MessageHide( work->msgWork );
    }
    break;
  
  //転送
  case MCS_TRANS_POKE_INIT:
    {
      u8 i;
      MB_COMM_INIT_DATA *commInit = MB_COMM_GetInitData( work->commWork );

      MB_COMM_ClearSendPokeData( work->commWork );
      MB_COMM_SetScore( work->commWork , work->capInitWork.score );
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        if( work->capInitWork.isCapture[i] == TRUE )
        {
          const u8 tray = work->selInitWork.selectPoke[i][0];
          const u8 idx  = work->selInitWork.selectPoke[i][1];
          const u32 itemNo = PPP_Get( work->capInitWork.ppp[i] , ID_PARA_item , NULL );
          
          if( itemNo != 0 )
          {
            MB_DATA_AddItem( work->dataWork , itemNo );
            PPP_Put( work->capInitWork.ppp[i] , ID_PARA_item , 0 );
          }
          
          MB_COMM_AddSendPokeData( work->commWork , work->capInitWork.ppp[i] );
          //元データのpppを消す
          MB_DATA_ClearBoxPPP( work->dataWork , tray , idx );
          //読んでおいた方も消す
          PPP_Clear( work->boxPoke[tray][idx] );
        }
      }
      //ハイスコアの置き換え
      if( commInit->highScore < work->capInitWork.score )
      {
        commInit->highScore = work->capInitWork.score;
      }
    }
    work->state = MCS_TRANS_POKE_SEND;
    break;
    
  case MCS_TRANS_POKE_SEND:
    if( MB_CHILD_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_COMM_Send_PokeData( work->commWork ) == TRUE )
      {
        MB_DATA_ResetSaveLoad( work->dataWork );
        work->state = MCS_CHECK_ROM_CRC_CODE;
      }
    }
    break;
  case MCS_CHECK_ROM_CRC_CODE:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE )
    {
      if( MB_DATA_CheckRomCode( work->dataWork ) == TRUE )
      {
        work->state = MCS_CHECK_ROM_CRC_LOAD;
      }
      else
      {
        //カード不正
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_12 , work->initData->msgSpeed );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
        work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
      }
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;
  case MCS_CHECK_ROM_CRC_LOAD:
    if( MB_DATA_LoadRomCRC( work->dataWork ) == TRUE )
    {
      work->state = MCS_CHECK_ROM_CRC;
    }
    break;

  case MCS_CHECK_ROM_CRC:
    if( MB_CHILD_ErrCheck( work , FALSE ) == FALSE )
    {
      if( MB_DATA_CheckRomCRC( work->dataWork ) == TRUE )
      {
        MB_COMM_SetChildState( work->commWork , MCCS_CRC_OK );
        work->state = MCS_SAVE_INIT;
      }
      else
      {
        //CRCチェック失敗
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_12 , work->initData->msgSpeed );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
        work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
      }
    }
    break;

  case MCS_SAVE_INIT:
    if( MB_CHILD_ErrCheck( work , FALSE ) == FALSE )
    {
      MB_CHILD_SaveInit( work );
      work->state = MCS_SAVE_MAIN;
      work->subState = MCSS_SAVE_WAIT_SAVE_INIT;
    }
    break;

  case MCS_SAVE_MAIN:
    MB_CHILD_SaveMain( work );
    MB_CHILD_ErrCheck( work , FALSE );
    break;

  case MCS_SAVE_TERM:
    MB_CHILD_SaveTerm( work );
    MB_CHILD_ErrCheck( work , FALSE );
    break;

  case MCS_SAVE_FINISH_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      MB_MSG_MessageHide( work->msgWork );
      work->state = MCS_DIPS_NEXT_GAME_CONFIRM;
    }
    break;
  
  case MCS_DIPS_NEXT_GAME_CONFIRM:
    if( MB_COMM_IsPostBoxLeast( work->commWork ) == TRUE )
    {
      if( MB_CHILD_GetLeastPoke( work ) < MB_CAP_POKE_NUM )
      {
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );

        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_09 , work->initData->msgSpeed );
        //エラーではなく通常終了扱い
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME );
        work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
      }
      else
      if( MB_COMM_GetBoxLeast( work->commWork ) < MB_CAP_POKE_NUM )
      {
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );

        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_08 , work->initData->msgSpeed );
        //エラーではなく通常終了扱い
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME );
        work->state = MCS_WAIT_NEXT_GAME_ERROR_MSG;
      }
      else
      {
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE_UP );

        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_06 , work->initData->msgSpeed );
        MB_COMM_SetChildState( work->commWork , MCCS_NEXT_GAME );
        work->state = MCS_WAIT_NEXT_GAME_CONFIRM;
      }
    }
    else
    {
      MB_CHILD_ErrCheck( work , FALSE );
    }
    break;
    
  case MCS_WAIT_NEXT_GAME_CONFIRM:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      MB_MSG_DispYesNo( work->msgWork , MMYT_UP );
      work->state = MCS_SELECT_NEXT_GAME_CONFIRM;
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;
  
  case MCS_SELECT_NEXT_GAME_CONFIRM:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNo( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        work->state = MCS_SELECT_FADEOUT;
        MB_COMM_SetChildState( work->commWork , MCCS_SELECT_BOX );
        MB_MSG_ClearYesNo( work->msgWork );
        MB_COMM_ResetFlag( work->commWork);
      }
      else
      if( ret == MMYR_RET2 )
      {
        MB_COMM_SetChildState( work->commWork , MCCS_END_GAME );
        MB_MSG_ClearYesNo( work->msgWork );
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_07 , work->initData->msgSpeed );
        MB_COMM_ReqDisconnect( work->commWork );
        work->state = MCS_WAIT_EXIT_COMM;
      }
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;
  
  case MCS_WAIT_NEXT_GAME_ERROR_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish(work->msgWork) == TRUE )
    {
      //初期タイムアウトから来るとmsgSpdもらってない。
      const int msgSpd = ( work->initData!= NULL ? work->initData->msgSpeed : 1 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_07 , msgSpd );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MCS_WAIT_EXIT_COMM;
      //ゲーム後のポケ不足でも来る。その際はエラーじゃないので、ここでステートセットはしない！
      //MB_COMM_SetChildState( work->commWork , MCCS_END_GAME_ERROR );
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;
  
  case MCS_WAIT_EXIT_COMM:
    if( MB_COMM_IsDisconnect( work->commWork ) == TRUE )
    {
      work->state = MCS_EXIT_GAME;
      MB_COMM_ExitComm( work->commWork );
    }
    MB_CHILD_ErrCheck( work , FALSE );
    break;

  case MCS_EXIT_GAME:
    break;
  }

  if( work->isInitCellSys == TRUE )
  {
    //OBJの更新
    GFL_CLACT_SYS_Main();
  }
    
  if( work->msgWork != NULL )
  {
    MB_MSG_MessageMain( work->msgWork );
  }
  PMSND_Main();
  
  return FALSE;
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void MB_CHILD_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  MB_CHILD_WORK *work = wk;
  //OBJの更新
  if( work->isInitCellSys == TRUE )
  {
    GFL_CLACT_SYS_VBlankFunc();
  }
}

static void MB_CHILD_VSync( void *pWork )
{
  static u8 cnt = 0;
  if( cnt > 1 )
  {
    cnt = 0;
    GFL_BG_SetScroll( MB_CHILD_FRAME_BG , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_CHILD_FRAME_BG , GFL_BG_SCROLL_Y_DEC , 1 );
    GFL_BG_SetScroll( MB_CHILD_FRAME_SUB_BG , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_CHILD_FRAME_SUB_BG , GFL_BG_SCROLL_Y_DEC , 1 );
  }
  else
  {
    cnt++;
  }

}

//--------------------------------------------------------------
//  グラフィック系初期化
//--------------------------------------------------------------
static void MB_CHILD_InitGraphic( MB_CHILD_WORK *work )
{
  GFL_DISP_SetDispOn();
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

  //BG系の初期化
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram割り当ての設定
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    // BG1 MAIN (メッセージ
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x6000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (キャラ
    //static const GFL_BG_BGCNT_HEADER header_main2 = {
    //  0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
    //  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    //  GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x0C000,
    //  GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    //};
    // BG3 MAIN (背景
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 SUB (メッセージ
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (背景
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_CHILD_SetupBgFunc( &header_main1 , MB_CHILD_FRAME_MSG , GFL_BG_MODE_TEXT );
    //MB_CHILD_SetupBgFunc( &header_main2 , LTVT_FRAME_CHARA , GFL_BG_MODE_TEXT );
    MB_CHILD_SetupBgFunc( &header_main3 , MB_CHILD_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_CHILD_SetupBgFunc( &header_sub1  , MB_CHILD_FRAME_SUB_MSG , GFL_BG_MODE_TEXT );
    MB_CHILD_SetupBgFunc( &header_sub3  , MB_CHILD_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN , &vramBank ,work->heapId );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  GFL_NET_ReloadIconTopOrBottom(FALSE , work->heapId );
  work->isInitCellSys = TRUE;
  
}

static void MB_CHILD_TermGraphic( MB_CHILD_WORK *work )
{
  work->isInitCellSys = FALSE;
  GFL_CLACT_SYS_Delete();
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_SUB_MSG );
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_BG );
  GFL_BG_FreeBGControl( MB_CHILD_FRAME_SUB_BG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg初期化 機能部
//--------------------------------------------------------------------------
static void MB_CHILD_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  リソース読み込み
//--------------------------------------------------------------
static void MB_CHILD_LoadResource( MB_CHILD_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_CHILD , work->heapId );

  //下画面
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_child_gra_bg_sub_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_child_gra_bg_sub_NCGR ,
                    MB_CHILD_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_child_gra_bg_sub_NSCR , 
                    MB_CHILD_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
  
  //上画面
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_child_gra_bg_main_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_child_gra_bg_main_NCGR ,
                    MB_CHILD_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_child_gra_bg_main_NSCR , 
                    MB_CHILD_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  
  GFL_ARC_CloseDataHandle( arcHandle );
}

//--------------------------------------------------------------
//  データの変換
//--------------------------------------------------------------
static void MB_CHILD_DataConvert( MB_CHILD_WORK *work )
{
  u8 i,j;
  /*
  //処理落ち対策で移動
  //PPP
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      void *pppSrc = MB_DATA_GetBoxPPP(work->dataWork,i,j);
      MB_UTIL_ConvertPPP( pppSrc , work->boxPoke[i][j] , work->cardType );
    }
  }
  */
  //ボックス名
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    u16 *strSrc = MB_DATA_GetBoxName( work->dataWork,i );
    
    PPCONV_ConvertStr( strSrc , work->boxName[i] , 10 );
  }
}

//--------------------------------------------------------------
//  ボックスの残個数チェック
//--------------------------------------------------------------
static const u16 MB_CHILD_GetLeastPoke( MB_CHILD_WORK *work )
{
  u8 i,j;
  u16 num = 0;
  for( i=0;i<MB_POKE_BOX_TRAY;i++ )
  {
    for( j=0;j<MB_POKE_BOX_POKE;j++ )
    {
      if( PPP_Get( work->boxPoke[i][j] , ID_PARA_poke_exist , NULL ) == TRUE )
      {
        const u8 ret = MB_UTIL_CheckPlay_PalGate( work->boxPoke[i][j] , work->cardType );
        if( ret == MUCPR_OK )
        {
          num++;
        }
      }
    }
  }
  
  MB_TPrintf("Child box least poke [%d].\n",num);
  return num;
}

//--------------------------------------------------------------
//  エラーのチェック
//--------------------------------------------------------------
static const BOOL MB_CHILD_ErrCheck( MB_CHILD_WORK *work , const BOOL noFade )
{
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isNetErr == FALSE )
  {
    NetErr_App_ReqErrorDisp();
    work->isNetErr = TRUE;
    if( noFade == FALSE )
    {
      work->state = MCS_FADEOUT;
    }
    else
    {
      work->state = MCS_WAIT_FADEOUT;
    }
    return TRUE;
  }
  return FALSE;
}

#pragma mark [>save func
//--------------------------------------------------------------
//  セーブ初期化
//--------------------------------------------------------------
static void MB_CHILD_SaveInit( MB_CHILD_WORK *work )
{
  MB_DATA_ResetSaveLoad( work->dataWork );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_CHILD_04 );
  MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );

}

//--------------------------------------------------------------
//  セーブ開放
//--------------------------------------------------------------
static void MB_CHILD_SaveTerm( MB_CHILD_WORK *work )
{
  MB_MSG_MessageDisp( work->msgWork , MSG_MB_CHILD_05 , work->initData->msgSpeed );
  MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
  work->state = MCS_SAVE_FINISH_WAIT;
  work->isSave = FALSE;
}

//--------------------------------------------------------------
//  セーブ更新
//--------------------------------------------------------------
static void MB_CHILD_SaveMain( MB_CHILD_WORK *work )
{
  work->isSave = TRUE;
  if( work->subState >= MCSS_SAVE_WAIT_FIRST )
  {
    MB_DATA_SaveData( work->dataWork );
  }

  switch( work->subState )
  {
  case MCSS_SAVE_WAIT_SAVE_INIT:
    if( MB_MSG_CheckPrintQueIsFinish( work->msgWork ) == TRUE &&
        MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_READY_START_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_SYNC_SAVE_INIT;
      }
    }
    break;
  case MCSS_SAVE_SYNC_SAVE_INIT:
    if( MB_COMM_GetIsPermitStartSave( work->commWork ) == TRUE )
    {
      work->subState = MCSS_SAVE_INIT;
    }
    break;
    
  case MCSS_SAVE_INIT:
    work->subState = MCSS_SAVE_WAIT_FIRST;
    MB_TPrintf( "MB_Parent Save Start!\n" );
    break;

  case MCSS_SAVE_WAIT_FIRST:
		if( MB_DATA_IsFinishSaveFirst( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_FIRST_SAVE , 0 ) == TRUE )
      {
  			work->subState = MCSS_SAVE_WAIT_FIRST_SYNC;
        MB_TPrintf( "MB_Parent Finish First!\n" );
  		}
		}
    break;

  case MCSS_SAVE_WAIT_FIRST_SYNC:
    work->subState = MCSS_SAVE_FIRST_SAVE_LAST;
		work->saveWaitCnt = 0;
    break;

  case MCSS_SAVE_FIRST_SAVE_LAST:
    if( MB_COMM_GetIsPermitFirstSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_SECOND;
  			MB_DATA_PermitLastSaveFirst( work->dataWork );
        MB_TPrintf( "MB_Parent FirstLast Save!\n" );
      }
    }
    break;

  case MCSS_SAVE_WAIT_SECOND:
		if( MB_DATA_IsFinishSaveSecond( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_SECOND_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_SECOND_SYNC;
        MB_TPrintf( "MB_Parent Finish Second!\n" );
  		}
		}
    break;

  case MCSS_SAVE_WAIT_SECOND_SYNC:
    work->subState = MCSS_SAVE_SECOND_SAVE_LAST;
		work->saveWaitCnt = 0;
    break;

  case MCSS_SAVE_SECOND_SAVE_LAST:
    if( MB_COMM_GetIsPermitSecondSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_LAST_SAVE;
        MB_TPrintf( "MB_Parent SecondLast Save!\n" );
  			MB_DATA_PermitLastSaveSecond( work->dataWork );
      }
    }
    break;

  case MCSS_SAVE_WAIT_LAST_SAVE:
		if( MB_DATA_IsFinishSaveAll( work->dataWork ) == TRUE )
		{
      if( MB_COMM_Send_Flag( work->commWork , MCFT_FINISH_SAVE , 0 ) == TRUE )
      {
        work->subState = MCSS_SAVE_WAIT_FINISH_SAVE_SYNC;
  		}
    }
    break;
  case MCSS_SAVE_WAIT_FINISH_SAVE_SYNC:
    if( MB_COMM_GetIsPermitFinishSave( work->commWork ) == TRUE )
    {
      work->state = MCS_SAVE_TERM;
      MB_TPrintf( "MB_Parent Finish All Save Seq!!!\n" );
    }
    break;
  }
}

#pragma mark [>proc func
static GFL_PROC_RESULT MB_CHILD_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CHILD_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_CHILD_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBootChild_ProcData =
{
  MB_CHILD_ProcInit,
  MB_CHILD_ProcMain,
  MB_CHILD_ProcTerm
};

//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work;
  
  MB_TPrintf("LeastAppHeap[%x]",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0xF8000 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT_DATA, 0x70000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_CHILD_WORK), HEAPID_MULTIBOOT );

  work->heapId = HEAPID_MULTIBOOT;
  
  MB_CHILD_Init( work );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work = mywk;
  
  MB_CHILD_Term( work );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  NetErr_DispCall(FALSE);
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ループ
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_CHILD_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_CHILD_WORK *work = mywk;
  const BOOL ret = MB_CHILD_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

static void MB_CHILD_DEBUG_ScreenDraw( const u8 idx )
{
#if PM_DEBUG&0
  u16 *scrBuf = (u16*)G2_GetBG3ScrPtr();
  scrBuf[idx] = 0;
#endif
}

