//======================================================================
/**
 * @file  mb_parent_sys.c
 * @brief マルチブート・親機メイン
 * @author  ariizumi
 * @data  09/11/13
 *
 * モジュール名：MB_PARENT
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/machine_use.h"
#include "system/gfl_use.h"
#include "system/net_err.h"
#include "system/ds_system.h"
#include "gamesystem/msgspeed.h"
#include "net/wih.h"
#include "net/network_define.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "poke_tool/poke_memo.h"
#include "system/wipe.h"
#include "savedata/misc.h"
#include "app/app_menu_common.h"
#include "field/evt_lock.h" //ロックカプセルのイベントロックのため
#include "sound/pm_sndsys.h"

#include "arc_def.h"
#include "mb_parent.naix"
#include "mystery.naix"
#include "net_err.naix"
#include "msg/msg_net_err.h"

#include "../../../../resource/fldmapdata/script/palpark_scr_local.h"
#include "../../../resource/fldmapdata/flagwork/work_define.h"

#include "multiboot/mb_parent_sys.h"
#include "multiboot/mb_comm_sys.h"
#include "multiboot/comm/mbp.h"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_local_def.h"
#include "test/ariizumi/ari_debug.h"
#include "debug/debug_str_conv.h"
//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define MB_PARENT_FRAME_SSMSG (GFL_BG_FRAME0_M)
#define MB_PARENT_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_PARENT_FRAME_BG  (GFL_BG_FRAME2_M)
#define MB_PARENT_FRAME_BG2  (GFL_BG_FRAME3_M)

#define MB_PARENT_FRAME_SUB_SSMSG  (GFL_BG_FRAME0_S)
#define MB_PARENT_FRAME_SUB_MSG  (GFL_BG_FRAME1_S)
#define MB_PARENT_FRAME_SUB_BAR  (GFL_BG_FRAME2_S)
#define MB_PARENT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

#define MB_PARENT_PLT_SUB_OBJ_APP  (0)

#define MB_PARENT_PLT_SUB_BG  (0)
#define MB_PARENT_PLT_SUB_ERR (7)
#define MB_PARENT_PLT_SUB_BAR (8)
#define MB_PARENT_PLT_SUB_SS  (9)
#define MB_PARENT_PLT_SUB_SS_MSG  (0x0d)

#define MB_PARENT_PLT_MAIN_BG  (0)

#define MB_PARENT_FIRST_TIMEOUT (60*30) //通常5秒以内で接続するのができなかった。


#define MP_PARENT_DEB (defined(PM_DEBUG))

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MPS_FADEIN,
  MPS_WAIT_FADEIN,
  MPS_FADEOUT,
  MPS_WAIT_FADEOUT,

  MPS_START_COMM_INIT_MSG,
  MPS_START_COMM_INIT_YESNO,
  MPS_START_COMM_MAIN,
  
  MPS_SEND_IMAGE_INIT,
  MPS_SEND_IMAGE_MAIN,
  MPS_SEND_IMAGE_TERM,

  MPS_SEND_INIT_NET,
  MPS_SEND_INIT_DATA,

  //ポケシフター専用
  MPS_SEND_GAMEDATA_INIT,
  MPS_SEND_GAMEDATA_SEND,

  MPS_WAIT_SELBOX,
  MPS_WAIT_FINISH_SELBOX,
  MPS_WAIT_POST_GAMEDATA,
  MPS_WAIT_FINISH_CAPTURE,
  MPS_WAIT_SEND_POKE,
  MPS_WAIT_CRC_CHECK,
  //ポケシフター専用ここまで

  MPS_SAVE_INIT,
  MPS_SAVE_MAIN,
  MPS_SAVE_TERM,

  //終了時ポケシフター専用
  MPS_SEND_LEAST_BOX,
  MPS_WAIT_NEXT_GAME_CONFIRM,
  //終了時ポケシフター専用ここまで

  //終了時映画配信専用
  MPS_MOVIE_WAIT_SAVE_MSG,
  MPS_MOVIE_WAIT_LAST_MSG,
  MPS_EXIT_COMM_MOVIE,
  //終了時映画配信専用ここまで

  MPS_EXIT_COMM,
  MPS_WAIT_EXIT_COMM,

  MPS_FAIL_FIRST_CONNECT,
  MPS_WAIT_FAIL_FIRST_CONNECT,
  
  //映画ポケ転送更新
  MPS_UPDATE_MOVIE_MODE,
  
  //無線OFF時エラー(実質てんそうマシンのみ
  MPS_COMM_ERROR_WAIT,
}MB_PARENT_STATE;

typedef enum
{
  MPSS_SEND_IMAGE_INIT_COMM,
  MPSS_SEND_IMAGE_INIT_COMM_WAIT,
  MPSS_SEND_IMAGE_WAIT_SEARCH_CH,
  MPSS_SEND_IMAGE_MBSYS_MAIN,
  
  MPSS_SEND_IMAGE_WAIT_BOOT_INIT,
  MPSS_SEND_IMAGE_WAIT_BOOT,

  MPSS_SEND_IMAGE_NET_EXIT,
  MPSS_SEND_IMAGE_NET_EXIT_WAIT,

  MPSS_SEND_CANCEL_BOOT,
//-------------------------------
  MPSS_SAVE_WAIT_SAVE_INIT,
  MPSS_SAVE_SYNC_SAVE_INIT,
  MPSS_SAVE_INIT,
  MPSS_SAVE_WAIT_FIRST,
  MPSS_SAVE_WAIT_FIRST_SYNC,
  MPSS_SAVE_FIRST_SAVE_LAST,
  MPSS_SAVE_WAIT_SECOND,
  MPSS_SAVE_WAIT_SECOND_SYNC,
  MPSS_SAVE_SECOND_SAVE_LAST,
  MPSS_SAVE_WAIT_LAST_SAVE,
  MPSS_SAVE_WAIT_FINISH_SAVE_SYNC,
  
}MB_PARENT_SUB_STATE;

//映画転送時のチェック
typedef enum
{
  MPMS_WAIT_COUNT_POKE, //ポケモンチェック中
  
  MPMS_CONFIRM_POKE_WAIT_MSG,
  MPMS_CONFIRM_POKE_WAIT_YESNO,

  MPMS_CONFIRM_HIDEN_WARN_INIT,
  MPMS_CONFIRM_HIDEN_WARN_WAIT,

  MPMS_CONFIRM_CHECK_ITEM_INIT,
  MPMS_CONFIRM_CHECK_ITEM_WAIT,
  MPMS_CONFIRM_CHECK_ITEM_YESNO,

  MPMS_CONFIRM_CHECK_FULL_WAIT,
  MPMS_CONFIRM_CHECK_FULL_YESNO,

  MPMS_CONFIRM_CANCEL_INIT,
  MPMS_CONFIRM_CANCEL_WAIT,
  MPMS_CONFIRM_CANCEL_YESNO,

  MPMS_CONFIRM_POKE_SEND_YESNO,

  MPMS_POST_POKE_WAIT,
  MPMS_POST_POKE_RET_POST,
  MPMS_POST_POKE_FINISH,
  
  MPMS_BOX_NOT_ENOUGH_INIT,
  MPMS_BOX_NOT_ENOUGH_WAIT,

  MPMS_WAIT_CHECK_LOCK_CAPSULE,
  
  MPMS_CONFIRM_LOCK_CAPSULE_WAIT,
  MPMS_CONFIRM_LOCK_CAPSULE_YESNO,
  MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT,
  MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO,
  MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT,

  MPMS_CHECK_SAVE,
}
MB_PARENT_MOVIE_STATE;

typedef enum
{
  MPCS_NONE,
  MPCS_INIT,
  MPCS_WAIT_MSG,
  MPCS_WAIT_CONFIRM,
  MPCS_END,
}MB_PARENT_CONFIRM_STATE;

typedef enum
{
  MPCR_PLT_APP,
  MPCR_NCG_APP,
  MPCR_ANM_APP,

  MPCR_MAX,
  
}MB_PARENT_CELL_RES;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  MB_PARENT_INIT_WORK *initWork;
  MB_COMM_WORK *commWork;
  BOOL         isNetErr;
  BOOL         isNetErrMb;
  BOOL         isNetErrSave;
  BOOL         isInitMbComm;
  
  MB_PARENT_STATE state;
  u8              subState;
  u8              movieState;
  u8              confirmState;
  u8              saveWaitCnt;
  u8              mode;         //ポケシフターか映画配信か？
  u16             timeOutCnt;   //初期のROM接続時にタイムアウトをチェックする
  BOOL            isSendGameData;
  BOOL            isSendRom;
  MB_MSG_YESNO_RET yesNoRet;
  u16             localHighScore; //今回のハイスコア
  u16             playNum;
  u16             totalGet;
  
  //映画用
  BOOL isBoxNotEnough;
  BOOL isPostMoviePoke;
  BOOL isPostMovieCapsule;
  
  MISC  *miscSave;
  
  //SendImage
  u16    *romTitleStr;  //DLROMタイトル
  u16    *romInfoStr;   //DLROM説明
  
  MB_COMM_INIT_DATA initData;
  void* gameData;
  u32   gameDataSize;
  //メッセージ用
  MB_MSG_WORK *msgWork;

  GFL_CLUNIT  *cellUnit;
  u32         cellResIdx[MPCR_MAX];
  GFL_CLWK    *clwkReturn;
  
  MBGameRegistry gameRegistry;  //MB配信用のデータ
  
  
  
}MB_PARENT_WORK;

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

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
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};


static void MB_PARENT_Init( MB_PARENT_WORK *work );
static void MB_PARENT_Term( MB_PARENT_WORK *work );
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work );
static void MB_PARENT_VBlankFunc(GFL_TCB *tcb, void *wk );
static void MB_PARENT_VSync( void *pWork );
static void MB_PARENT_VSyncMovie( void *pWork );

static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work );
static void MB_PARENT_CreateErrorDisp( MB_PARENT_WORK *work );

static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work );
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work );
static BOOL MP_PARENT_WhCallBack( BOOL bResult );

static void MB_PARENT_SaveInit( MB_PARENT_WORK *work );
static void MB_PARENT_SaveInitPoke( MB_PARENT_WORK *work );
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work );
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work );

static void MB_PARENT_UpdateMovieMode( MB_PARENT_WORK *work );

static void MB_PARENT_SetFinishState( MB_PARENT_WORK *work , const u8 state );

static BOOL  MB_PARENT_PullOutCallBack( void );

#if MP_PARENT_DEB
static void MB_PARENT_InitDebug( MB_PARENT_WORK *work );
static void MB_PARENT_TermDebug( MB_PARENT_WORK *work );
#endif

BOOL WhCallBackFlg = FALSE;
static u16 MB_PARENT_bgScrollCnt = 0;
static BOOL errIsShifter;

FS_EXTERN_OVERLAY(dev_wireless);

//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
static void MB_PARENT_Init( MB_PARENT_WORK *work )
{
  work->state = MPS_FADEIN;
  work->movieState = MPMS_WAIT_COUNT_POKE;
  work->mode = work->initWork->mode;
  
  MB_PARENT_InitGraphic( work );
  MB_PARENT_LoadResource( work );

  if( work->mode == MPM_POKE_SHIFTER )
  {
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_SUB_MSG , MB_PARENT_FRAME_SUB_MSG , FILE_MSGID_MB , FALSE , FALSE );
  }
  else
  {
    work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_MSG , MB_PARENT_FRAME_MSG , FILE_MSGID_MB , FALSE , TRUE );
  }
  
  work->commWork = MB_COMM_CreateSystem( work->heapId );
  work->isSendGameData = FALSE;
  work->gameData = NULL;
  
  MB_PARENT_CreateErrorDisp( work );
  
  if( work->mode == MPM_POKE_SHIFTER )
  {
    SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( work->initWork->gameData );
    work->miscSave = SaveData_GetMisc( svWork );
    //キャンセル(一番低いステート)にしておく
    MISC_SetPalparkFinishState( work->miscSave , PALPARK_FINISH_CANCEL );
  }
  else
  {
    work->miscSave = NULL;
    MB_PARENT_bgScrollCnt = 0;
  }
  
  work->vBlankTcb = GFUser_VIntr_CreateTCB( MB_PARENT_VBlankFunc , work , 8 );
  if( work->mode == MPM_POKE_SHIFTER )
  {
    GFUser_SetVIntrFunc( MB_PARENT_VSync , NULL );
  }
  else
  {
    GFUser_SetVIntrFunc( MB_PARENT_VSyncMovie , NULL );
    PMSND_PlayBGM( SEQ_BGM_WIFI_PRESENT );
  }

  
  work->isInitMbComm = FALSE;
  work->isPostMoviePoke = FALSE;
  work->isPostMovieCapsule = FALSE;
  work->localHighScore = 0;
  work->playNum = 0;
  work->totalGet = 0;

  GFL_UI_SleepDisable( GFL_UI_SLEEP_MB );
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static void MB_PARENT_Term( MB_PARENT_WORK *work )
{
  
  GFL_UI_SleepEnable( GFL_UI_SLEEP_MB );
  GFL_TCB_DeleteTask( work->vBlankTcb );
  GFUser_ResetVIntrFunc();
  GFL_NET_WirelessIconEasyEnd();
  if( work->mode == MPM_MOVIE_TRANS )
  {
    PMSND_StopBGM();
  }
  else
  {
    EVENTWORK *evWork = GAMEDATA_GetEventWork( work->initWork->gameData );
    u16 *localWk0 = EVENTWORK_GetEventWorkAdrs( evWork, LOCALWORK0 );
    u16 *localWk1 = EVENTWORK_GetEventWorkAdrs( evWork, LOCALWORK1 );
    *localWk0 = work->localHighScore;
    *localWk1 = work->playNum;
    ARI_TPrintf("PlayData[score:%3d][num:%2d][get:%2d]\n",work->localHighScore,work->playNum,work->totalGet);
  }

  if( work->gameData != NULL )
  {
    GFL_HEAP_FreeMemory( work->gameData );
  }
  MB_COMM_DeleteSystem( work->commWork );

  MB_MSG_MessageTerm( work->msgWork );
  MB_PARENT_TermGraphic( work );
}

//--------------------------------------------------------------
//  更新
//--------------------------------------------------------------
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work )
{
  MB_COMM_UpdateSystem( work->commWork );

  if( work->isNetErr == TRUE &&
      work->state != MPS_FADEOUT &&
      work->state != MPS_WAIT_FADEOUT )
  {
    work->isNetErrSave = GAMEDATA_GetIsSave( work->initWork->gameData );
    if( work->isNetErrSave == TRUE )
    {
      GAMEDATA_SaveAsyncCancel( work->initWork->gameData );
    }
    work->state = MPS_FADEOUT;
    MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
  }
  
#if PM_DEBUG
  {
    static int besState = 0xFFFF;
    if( besState != work->state )
    {
      OS_TFPrintf(3,"state[%d]->[%d]\n",besState,work->state);
      besState = work->state;
    }
  }
#endif

  switch( work->state )
  {
  case MPS_FADEIN:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );

    work->state = MPS_WAIT_FADEIN;
    break;
    
  case MPS_WAIT_FADEIN:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      if( DS_SYSTEM_IsAvailableWireless() == TRUE )
      {
        if( work->mode == MPM_POKE_SHIFTER )
        {
          work->state = MPS_SEND_IMAGE_INIT;
        }
        else
        {
          //転送マシンは通信開始を聞く
          work->state = MPS_START_COMM_INIT_MSG;
        }
      }
      else
      {
        //MSGが違うので特殊関数
        //MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageDips_CommDisableError( work->msgWork , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        work->state = MPS_COMM_ERROR_WAIT;
      }
    }
    break;
    
  case MPS_FADEOUT:
    if( work->initWork->mode == MPM_POKE_SHIFTER )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    }
    else
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                  WIPE_FADE_WHITE , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      PMSND_FadeOutBGM(PMSND_FADE_FAST);
    }

    work->state = MPS_WAIT_FADEOUT;
    break;
  case MPS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
    
  //-----------------------------------------------
  //通信開始確認
  case MPS_START_COMM_INIT_MSG:
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_23 , MSGSPEED_GetWait() );
    work->state = MPS_START_COMM_INIT_YESNO;
    break;
  case MPS_START_COMM_INIT_YESNO:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_UP );
      work->state = MPS_START_COMM_MAIN;
    }
    break;
  case MPS_START_COMM_MAIN:
    {
      MB_MSG_YESNO_RET ret;
      ret = MB_MSG_UpdateYesNoUpper( work->msgWork );

      if( ret == MMYR_RET1 )
      {
        work->state = MPS_SEND_IMAGE_INIT;
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_ClearYesNoUpper( work->msgWork );
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->state = MPS_FADEOUT;
      }
    }
    
    break;

  //-----------------------------------------------
  //ROMの送信部分
  case MPS_SEND_IMAGE_INIT:
    MP_PARENT_SendImage_Init( work );
    work->state = MPS_SEND_IMAGE_MAIN;
    break;
    
  case MPS_SEND_IMAGE_MAIN:
    {
      const BOOL ret = MP_PARENT_SendImage_Main( work );
      if( ret == TRUE )
      {
        work->state = MPS_SEND_IMAGE_TERM;
      }
    }
    break;
    
  case MPS_SEND_IMAGE_TERM:
    MP_PARENT_SendImage_Term( work );
    if( work->isSendRom == TRUE )
    {
      //起動成功
      work->state = MPS_SEND_INIT_NET;
      work->timeOutCnt = 0;
      MB_COMM_InitComm( work->commWork );

      if( work->mode == MPM_POKE_SHIFTER )
      {
        GFL_NET_ReloadIconTopOrBottom(FALSE , work->heapId );
      }
      else
      {
        GFL_NET_ReloadIconTopOrBottom(TRUE , work->heapId );
      }
    }
    else
    {
      //起動失敗
      work->state = MPS_FADEOUT;
    }
    break;
    
  //起動後
  case MPS_SEND_INIT_NET:
    if( MB_COMM_IsInitComm( work->commWork ) == TRUE )
    {
      work->state = MPS_SEND_INIT_DATA;
      MB_COMM_InitParent( work->commWork );
    }
    break;
    
  case MPS_SEND_INIT_DATA:
    work->timeOutCnt++;
    if( work->timeOutCnt >= MB_PARENT_FIRST_TIMEOUT )
    {
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      work->state = MPS_FAIL_FIRST_CONNECT;
    }
    else
    if( MB_COMM_IsSendEnable( work->commWork ) == TRUE )
    {
      work->initData.msgSpeed = MSGSPEED_GetWait();
      work->initData.strMode = GFL_MSGSYS_GetLangID();
      if( work->mode == MPM_POKE_SHIFTER )
      {
        work->initData.highScore = MISC_GetPalparkHighscore(work->miscSave);
      }
      if( MB_COMM_Send_InitData( work->commWork , &work->initData ) == TRUE )
      {
        if( work->mode == MPM_POKE_SHIFTER )
        {
          work->state = MPS_SEND_GAMEDATA_INIT;
        }
        else
        {
          work->state = MPS_UPDATE_MOVIE_MODE;
        }
      }
    }
    break;
    
    //ポケシフター用処理
  case MPS_SEND_GAMEDATA_INIT:
    {
      FSFile file;
      BOOL result;
      const char* arcPath = GFUser_GetFileNameByArcID( ARCID_MB_CAPTER );
      //読み出す
      FS_InitFile( &file );
      result = FS_OpenFile(&file,arcPath);
      GF_ASSERT_HEAVY( result );
      FS_SeekFileToBegin( &file );
      work->gameDataSize = FS_GetLength( &file );
      work->gameData = GFL_HEAP_AllocClearMemory( work->heapId , work->gameDataSize );
      FS_ReadFile( &file,work->gameData,work->gameDataSize );
      result = FS_CloseFile( &file );
      GF_ASSERT_HEAVY( result );
      MB_TPrintf( "[%d]\n",work->gameDataSize );
    }
    work->state = MPS_SEND_GAMEDATA_SEND;
    break;

  case MPS_SEND_GAMEDATA_SEND:
    work->state = MPS_WAIT_SELBOX;
    break;
    
  case MPS_WAIT_SELBOX:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
    {
      //読み込みエラーが発生した
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SELECT_BOX )
    {
      if( work->isSendGameData == FALSE )
      {
        work->isSendGameData = TRUE;
        MB_COMM_InitSendGameData( work->commWork , work->gameData , work->gameDataSize );
      }
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_03 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_FINISH_SELBOX;
    }
    break;
    
  case MPS_WAIT_FINISH_SELBOX:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_WAIT_GAME_DATA )
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_04 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_POST_GAMEDATA;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_CANCEL_BOX )
    {
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_CANCEL );

      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
  
  //break;  //MCCS_WAIT_GAME_DATAを経由しないこともあるのでスルー！！
  case MPS_WAIT_POST_GAMEDATA:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_CAP_GAME )
    {
      //ゲーム終了後でもいいが、ココが確実なので。
      MB_COMM_ClearSendPokeData(work->commWork);
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_05 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_FINISH_CAPTURE;
    }
    break;

  case MPS_WAIT_FINISH_CAPTURE:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SEND_POKE )
    {
      RECORD *record = GAMEDATA_GetRecordPtr(work->initWork->gameData);
      RECORD_Inc(record,RECID_POKESHIFTER_COUNT);
      work->playNum++;
      
      //要望でメッセージ切り替えをなくす
      //MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_06 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_SEND_POKE;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_NEXT_GAME )
    {
      RECORD *record = GAMEDATA_GetRecordPtr(work->initWork->gameData);
      RECORD_Inc(record,RECID_POKESHIFTER_COUNT);
      work->playNum++;

      //ここに来たということは捕まえていない！
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_NO_GET );
      work->state = MPS_SEND_LEAST_BOX;
    }
    break;

  case MPS_WAIT_SEND_POKE:
    if( MB_COMM_IsPostPoke( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_POST_POKE , 0 ) == TRUE )
      {
        work->state = MPS_WAIT_CRC_CHECK;
      }
    }
    break;
  case MPS_WAIT_CRC_CHECK:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE ||
        work->mode == MPM_MOVIE_TRANS ) //映画の時はポケ送ってないかも。
    {
      if( MB_COMM_GetChildState(work->commWork) == MCCS_CRC_OK )
      {
        work->state = MPS_SAVE_INIT;
      }
      else
      if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
      {
        //CRCチェックエラーが発生した
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
          MB_COMM_ReqDisconnect( work->commWork );
          work->state = MPS_EXIT_COMM;
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_18 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
          work->state = MPS_MOVIE_WAIT_LAST_MSG;
        }
      }
    }
    break;
    
  case MPS_SAVE_INIT:
    MB_PARENT_SaveInit( work );

    work->state = MPS_SAVE_MAIN;
    work->subState = MPSS_SAVE_WAIT_SAVE_INIT;
    break;

  case MPS_SAVE_MAIN:
    MB_PARENT_SaveMain( work );
    break;

  case MPS_SAVE_TERM:
    MB_PARENT_SaveTerm( work );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_08 , MSGSPEED_GetWait() );
      work->state = MPS_SEND_LEAST_BOX;
    }
    else
    {
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_19 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      //先に通信を切る
      work->state = MPS_EXIT_COMM_MOVIE;
    }
    break;

  //終了時ポケシフター専用
  case MPS_SEND_LEAST_BOX:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
      const u16 leastBoxNum = BOXDAT_GetEmptySpaceTotal( boxMng );
      if( MB_COMM_Send_Flag( work->commWork , MCFT_LEAST_BOX , leastBoxNum ) == TRUE )
      {
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_12 , MSGSPEED_GetWait() );
        MB_Printf("Parent box empty num[%d]\n",leastBoxNum);
        work->state = MPS_WAIT_NEXT_GAME_CONFIRM;
      }
    }
    break;

  case MPS_WAIT_NEXT_GAME_CONFIRM:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SELECT_BOX )
    {
      MB_COMM_ResetFlag( work->commWork);
      work->state = MPS_WAIT_SELBOX;
    }
    else
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME ||
        MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
    {
      if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR )
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_ERROR );
      }
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_09 , MSGSPEED_GetWait() );
      work->state = MPS_EXIT_COMM_MOVIE;
    }
    break;
  //終了時ポケシフター専用ここまで
  
  case MPS_EXIT_COMM_MOVIE:
    if( MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_FINISH_MACHINE , 0 ) == TRUE )
    {
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM;
    }
    break;
  
  case MPS_EXIT_COMM:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_IsDisconnect( work->commWork ) == TRUE )
      {
        MB_COMM_ExitComm( work->commWork );
        work->state = MPS_WAIT_EXIT_COMM;
      }
    }
    break;
  case MPS_WAIT_EXIT_COMM:
    if( MB_COMM_IsFinishComm( work->commWork ) == TRUE )
    {
      if( work->mode == MPM_POKE_SHIFTER )
      {
        work->state = MPS_FADEOUT;
      }
      else
      {
        //映画は先に通信切ってる
        work->state = MPS_MOVIE_WAIT_SAVE_MSG;
      }
    }
    break;

  //終了時映画配信専用
  case MPS_MOVIE_WAIT_SAVE_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_MessageHide( work->msgWork );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_15 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      work->state = MPS_MOVIE_WAIT_LAST_MSG;
    }
    break;
  
  case MPS_MOVIE_WAIT_LAST_MSG:
    if( MB_COMM_IsFinishComm( work->commWork ) == TRUE )
    {
      if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
      {
        work->state = MPS_FADEOUT;
      }
    }
    break;
  //終了時映画配信専用ここまで
    
  //初回接続失敗
  case MPS_FAIL_FIRST_CONNECT:
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_11 , MSGSPEED_GetWait() );
    MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
    work->state = MPS_WAIT_FAIL_FIRST_CONNECT;
    MB_COMM_ExitComm( work->commWork );
    break;
  
  case MPS_WAIT_FAIL_FIRST_CONNECT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->state = MPS_WAIT_EXIT_COMM;
    }
  
  //----------------------------------------------------------------
  //  映画転送
  //----------------------------------------------------------------
  case MPS_UPDATE_MOVIE_MODE:
    MB_PARENT_UpdateMovieMode( work );
    break;

  //無線OFF時エラー(実質てんそうマシンのみ
  case MPS_COMM_ERROR_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->state = MPS_FADEOUT;
    }
    break;

  }
  
  MB_MSG_MessageMain( work->msgWork );
  
  //OBJの更新
  GFL_CLACT_SYS_Main();
  
  
  return FALSE;
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void MB_PARENT_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  //OBJの更新
  GFL_CLACT_SYS_VBlankFunc();
}

static void MB_PARENT_VSync( void *pWork )
{
  static u8 cnt = 0;
  if( cnt > 1 )
  {
    cnt = 0;
    GFL_BG_SetScroll( MB_PARENT_FRAME_BG2 , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_BG2 , GFL_BG_SCROLL_Y_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_SUB_BG , GFL_BG_SCROLL_X_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_SUB_BG , GFL_BG_SCROLL_Y_DEC , 1 );
  }
  else
  {
    cnt++;
  }
}

static void MB_PARENT_VSyncMovie( void *pWork )
{
  //背景アニメ更新
  MB_PARENT_bgScrollCnt++;
  
  if( MB_PARENT_bgScrollCnt >= 5 )
  {
    MB_PARENT_bgScrollCnt = 0;
    GFL_BG_SetScroll( MB_PARENT_FRAME_BG2 , GFL_BG_SCROLL_Y_DEC , 1 );
    GFL_BG_SetScroll( MB_PARENT_FRAME_SUB_BG , GFL_BG_SCROLL_Y_DEC , 1 );
  }
}

//--------------------------------------------------------------
//  グラフィック系初期化
//--------------------------------------------------------------
static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  G2_BlendNone();
  G2S_BlendNone();

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
    // BG0 MAIN (SSメッセージ
    static const GFL_BG_BGCNT_HEADER header_main0 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x00000,0x5800,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 MAIN (メッセージ
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x18000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (背景
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (背景２
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG0 SUB (""メッセージ
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 SUB (メッセージ
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (バー
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (背景
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_PARENT_SetupBgFunc( &header_main0 , MB_PARENT_FRAME_SSMSG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main1 , MB_PARENT_FRAME_MSG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main2 , MB_PARENT_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main3 , MB_PARENT_FRAME_BG2 , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub0  , MB_PARENT_FRAME_SUB_SSMSG, GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub1  , MB_PARENT_FRAME_SUB_MSG, GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub2  , MB_PARENT_FRAME_SUB_BAR, GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub3  , MB_PARENT_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
    if( work->mode == MPM_POKE_SHIFTER )
    {
      GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_BAR , TRUE );
    }
    else
    {
      GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_BAR , FALSE );
    }
  }

  //OBJ系の初期化
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.CGR_RegisterMax = 64;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    work->cellUnit = GFL_CLACT_UNIT_Create( 8 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );

    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
}

static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work )
{
  GFL_CLACT_WK_Remove( work->clwkReturn );
  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SSMSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG2 );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BAR );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_SSMSG );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}


//--------------------------------------------------------------------------
//  Bg初期化 機能部
//--------------------------------------------------------------------------
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}


//--------------------------------------------------------------
//  リソース読み込み
//--------------------------------------------------------------
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work )
{
  if( work->mode == MPM_POKE_SHIFTER )
  {
    //ポケシフター
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_PARENT , work->heapId );

    //下画面
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_sub_NCLR , 
                      PALTYPE_SUB_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_sub_NCGR ,
                      MB_PARENT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_sub_NSCR , 
                      MB_PARENT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
    
    //上画面
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_parent_bg_main_NCLR , 
                      PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_bg_main_NCGR ,
                      MB_PARENT_FRAME_BG2 , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_main_NSCR , 
                      MB_PARENT_FRAME_BG2 ,  0 , 0, FALSE , work->heapId );

    GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_mb_parent_ss_NCLR , 
                      PALTYPE_MAIN_BG , MB_PARENT_PLT_SUB_SS*32 , 
                      MB_PARENT_PLT_SUB_SS*32 , 32*4 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_ss_NCGR ,
                      MB_PARENT_FRAME_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_ss_NSCR , 
                      MB_PARENT_FRAME_BG ,  0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_mb_parent_ss_msg_NCLR , 
                      PALTYPE_MAIN_BG , MB_PARENT_PLT_SUB_SS_MSG*32 , 
                      MB_PARENT_PLT_SUB_SS_MSG*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_ss_msg_NCGR ,
                      MB_PARENT_FRAME_SSMSG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_ss_msg_NSCR , 
                      MB_PARENT_FRAME_SSMSG ,  0 , 0, FALSE , work->heapId );
    GFL_BG_LoadScreenReq( MB_PARENT_FRAME_BG );
    
    GFL_ARC_CloseDataHandle( arcHandle );
  }
  else
  {
    //映画転送
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_MYSTERY , work->heapId );

    //下画面
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mystery_fusigi_bg_00_NCLR , 
                      PALTYPE_SUB_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mystery_fusigi_bg_00_NCGR ,
                      MB_PARENT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mystery_fusigi_bg_00_NSCR , 
                      MB_PARENT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );
    //上画面
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mystery_fusigi_bg_00_NCLR , 
                      PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mystery_fusigi_bg_00_NCGR ,
                      MB_PARENT_FRAME_BG2 , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mystery_fusigi_bg_00_NSCR , 
                      MB_PARENT_FRAME_BG2 ,  0 , 0, FALSE , work->heapId );
    
    GFL_ARC_CloseDataHandle( arcHandle );


    arcHandle = GFL_ARC_OpenDataHandle( ARCID_MB_PARENT , work->heapId );
    GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_mb_parent_ss_NCLR , 
                      PALTYPE_SUB_BG , MB_PARENT_PLT_SUB_SS*32 , 
                      MB_PARENT_PLT_SUB_SS*32 , 32*4 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_ss_NCGR ,
                      MB_PARENT_FRAME_SUB_MSG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_ss_NSCR , 
                      MB_PARENT_FRAME_SUB_MSG ,  0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_mb_parent_ss_msg_NCLR , 
                      PALTYPE_SUB_BG , MB_PARENT_PLT_SUB_SS_MSG*32 , 
                      MB_PARENT_PLT_SUB_SS_MSG*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_parent_ss_msg_NCGR ,
                      MB_PARENT_FRAME_SUB_SSMSG , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_ss_msg_NSCR , 
                      MB_PARENT_FRAME_SUB_SSMSG ,  0 , 0, FALSE , work->heapId );
    GFL_BG_LoadScreenReq( MB_PARENT_FRAME_SUB_MSG );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_MSG , FALSE );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_SSMSG , FALSE );

    GFL_ARC_CloseDataHandle( arcHandle );
  }

  
  //共通素材
  {
    ARCHANDLE *commonArcHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );
    
    //バー
    GFL_ARCHDL_UTIL_TransVramPalette( commonArcHandle , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_SUB_BG , MB_PARENT_PLT_SUB_BAR*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( commonArcHandle , APP_COMMON_GetBarCharArcIdx() ,
                      MB_PARENT_FRAME_SUB_BAR , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( commonArcHandle , APP_COMMON_GetBarScrnArcIdx() , 
                      MB_PARENT_FRAME_SUB_BAR , 0 , 0, FALSE , work->heapId );
    GFL_BG_ChangeScreenPalette( MB_PARENT_FRAME_SUB_BAR , 0 , 21 , 32 , 3 , MB_PARENT_PLT_SUB_BAR );
    GFL_BG_LoadScreenReq( MB_PARENT_FRAME_SUB_BAR );

    //バーアイコン
    work->cellResIdx[MPCR_PLT_APP] = GFL_CLGRP_PLTT_RegisterEx( commonArcHandle , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_SUB , 
          MB_PARENT_PLT_SUB_OBJ_APP*32 , 0 , 
          APP_COMMON_BARICON_PLT_NUM , work->heapId  );
    work->cellResIdx[MPCR_NCG_APP] = GFL_CLGRP_CGR_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellResIdx[MPCR_ANM_APP] = GFL_CLGRP_CELLANIM_Register( commonArcHandle , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), 
          work->heapId  );

    GFL_ARC_CloseDataHandle( commonArcHandle );
  }
  
  {
    //OBJの作成
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 256-24;
    cellInitData.pos_y = 192-24;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    cellInitData.bgpri = 0;
    cellInitData.softpri = 0;

    work->clwkReturn = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellResIdx[MPCR_NCG_APP],
              work->cellResIdx[MPCR_PLT_APP],
              work->cellResIdx[MPCR_ANM_APP],
              &cellInitData ,CLSYS_DRAW_SUB , work->heapId );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkReturn , TRUE );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkReturn , FALSE );
    }
  }
}

//--------------------------------------------------------------
//  カード抜け用エラー画面作成
//--------------------------------------------------------------
static void MB_PARENT_CreateErrorDisp( MB_PARENT_WORK *work )
{
  u8 frame;
  ARCHANDLE *arcHandle;

  //説明画面の逆に出す
  if( work->mode == MPM_POKE_SHIFTER )
  {
    errIsShifter = TRUE;
    frame = MB_PARENT_FRAME_SUB_SSMSG;
  }
  else
  {
    errIsShifter = FALSE;
    frame = MB_PARENT_FRAME_BG;
  }
  GFL_BG_SetVisible( frame , FALSE );

  arcHandle = GFL_ARC_OpenDataHandle( ARCID_NET_ERR , work->heapId );

  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_net_err_net_err_NCLR , 
                    PALTYPE_SUB_BG  , MB_PARENT_PLT_SUB_ERR*32 , 32 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_net_err_net_err_NCLR , 
                    PALTYPE_MAIN_BG , MB_PARENT_PLT_SUB_ERR*32 , 32 , work->heapId );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_net_err_net_err_NCGR ,
                    frame , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_net_err_net_err_NSCR , 
                    frame ,  0 , 0, FALSE , work->heapId );
  GFL_BG_ChangeScreenPalette( frame,0,0,32,24,MB_PARENT_PLT_SUB_ERR );
  GFL_BG_LoadScreenReq( frame );
  
  GFL_ARC_CloseDataHandle( arcHandle );
  
  //メッセージ書き込み
  {
    GFL_BMPWIN *win = GFL_BMPWIN_Create( frame , 
                                        1 , 4 ,32-2 , 8 , 
                                        MB_PARENT_PLT_SUB_ERR ,
                                        GFL_BMP_CHRAREA_GET_B );
    GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
          ARCID_MESSAGE, NARC_message_net_err_dat, work->heapId);
    STRBUF *str = GFL_MSG_CreateString(msgHandle, net_error_0002);
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win) , 7 );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp(win) , 0 , 0,
          str , MB_MSG_GetFont(work->msgWork ) , PRINTSYS_LSB_Make(4, 0xb, 7) );

    GFL_BMPWIN_TransVramCharacter( win );
    GFL_BMPWIN_MakeScreen( win );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(win) );
    
    GFL_STR_DeleteBuffer(str);
    GFL_MSG_Delete( msgHandle );
    GFL_BMPWIN_Delete( win );
  }
  
  //バックドロップ変更
  {
    u16 *mplt = (u16*)HW_BG_PLTT;
    u16 *splt = (u16*)HW_DB_BG_PLTT;
    mplt[0] = 0x7eea;
    splt[0] = 0x7eea;
  }
  
}

#pragma mark [>SendImage func
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work );
static void MB_PARENT_SoftResetCallBack( void *pWork );

//--------------------------------------------------------------
//  ROM送信部分 初期化
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work )
{
  int i;
  STRBUF *titleStr;
  STRBUF *infoStr;
  u16 titleLen,infoLen;
  MYSTATUS *myStatus = GAMEDATA_GetMyStatus(work->initWork->gameData);
  
  work->subState = MPSS_SEND_IMAGE_INIT_COMM;
  work->confirmState = MPCS_NONE;
  work->romTitleStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_NAME_LENGTH*2 );
  work->romInfoStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_INTRO_LENGTH*2 );

  //タイトル
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  MB_MSG_MessageCreateWordset( work->msgWork );
  MB_MSG_MessageWordsetNumberZero( work->msgWork , 0 , MyStatus_GetID_Low(myStatus) , 5 );
  {
    STRBUF *workStr;

    if( work->mode == MPM_POKE_SHIFTER )
    {
      workStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE );
    }
    else
    {
      workStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE_MOVIE );
    }
    titleStr = GFL_STR_CreateBuffer( 256 , work->heapId );
    WORDSET_ExpandStr( MB_MSG_GetWordSet(work->msgWork) , titleStr , workStr );
    GFL_STR_DeleteBuffer( workStr );
  }

  //説明
  if( work->mode == MPM_POKE_SHIFTER )
  {
    if( GET_VERSION() == VERSION_BLACK )
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_B );
    }
    else
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_W );
    }
  }
  else
  {
    if( GET_VERSION() == VERSION_BLACK )
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_MOVIE_B );
    }
    else
    {
      infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO_MOVIE_W );
    }
  }
  titleLen = GFL_STR_GetBufferLength( titleStr );
  infoLen = GFL_STR_GetBufferLength( infoStr );
  
  GFL_STD_MemCopy16( GFL_STR_GetStringCodePointer( titleStr ) ,
                     work->romTitleStr ,
                     titleLen*2 );
  GFL_STD_MemCopy16( GFL_STR_GetStringCodePointer( infoStr ) ,
                     work->romInfoStr ,
                     infoLen*2 );
  GFL_STR_DeleteBuffer( titleStr );
  GFL_STR_DeleteBuffer( infoStr );

  //終端コードを変換
  work->romTitleStr[titleLen] = 0x0000;
  work->romInfoStr[infoLen] = 0x0000;
  
  //改行コード変換
  for( i=0;i<MB_GAME_INTRO_LENGTH;i++ )
  {
    if( work->romInfoStr[i] == 0xFFFE )
    {
      work->romInfoStr[i] = 0x000a;
    }
  }
  //全角数字変換
  for( i=0;i<MB_GAME_NAME_LENGTH;i++ )
  {
    if( work->romTitleStr[i] == L'Ｉ' )
    {
      work->romTitleStr[i] = L'I';
    }
    else
    if( work->romTitleStr[i] == L'Ｄ' )
    {
      work->romTitleStr[i] = L'D';
    }
    else
    if( work->romTitleStr[i] >= L'０' &&
        work->romTitleStr[i] <= L'９')
    {
      work->romTitleStr[i] = work->romTitleStr[i]-L'０'+L'0';
    }
  }
  
  work->isSendRom = FALSE;
  GFL_OVERLAY_Load( FS_OVERLAY_ID(dev_wireless));

  if( work->mode == MPM_POKE_SHIFTER )
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_BG , TRUE );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SSMSG , TRUE );
  }
  else
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_MSG , TRUE );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_SSMSG , TRUE );
  }

  CARD_SetPulledOutCallback( &MB_PARENT_PullOutCallBack );
}

//--------------------------------------------------------------
//  ROM送信部分 開放
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work )
{
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dev_wireless));

#if PM_DEBUG
  MachineSystem_SetPullOutCallBack();
#else
  CARD_SetPulledOutCallback( NULL );
#endif

  GFL_HEAP_FreeMemory( work->romTitleStr );
  GFL_HEAP_FreeMemory( work->romInfoStr );
  if( work->mode == MPM_POKE_SHIFTER )
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_BG , FALSE );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SSMSG , FALSE );
  }
  else
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_MSG , FALSE );
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_SSMSG , FALSE );
  }
}

//--------------------------------------------------------------
//  ROM送信部分 更新
//--------------------------------------------------------------
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work )
{

  if( WH_GetSystemState() == WH_SYSSTATE_ERROR ||
      WH_GetSystemState() == WH_SYSSTATE_FATAL )
  {
    work->isSendRom = FALSE;
    work->isNetErrMb = TRUE;
    if( WH_Finalize() == TRUE )
    {
      MBP_ClearBuffer();
      WH_FreeMemory();
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  
#if PM_DEBUG
  {
    static int besSubState = 0xFFFF;
    if( besSubState != work->subState )
    {
      OS_TFPrintf(3,"state[%d]->[%d]\n",besSubState,work->subState);
      besSubState = work->subState;
    }
  }
#endif


  switch( work->subState )
  {
  case MPSS_SEND_IMAGE_INIT_COMM:
    WhCallBackFlg = FALSE;
    work->isInitMbComm = TRUE;
    WH_Initialize(work->heapId , &MP_PARENT_WhCallBack , FALSE );
    
    GFL_NET_WirelessIconEasyXY( 256-16,0,FALSE,work->heapId );
    //アイコン強制出し
    if( work->mode == MPM_POKE_SHIFTER )
    {
      GFL_NET_WirelessIconEasy_HoldLCD( FALSE,work->heapId );
    }
    else
    {
      GFL_NET_WirelessIconEasy_HoldLCD( TRUE,work->heapId );
    }
    
    work->subState = MPSS_SEND_IMAGE_INIT_COMM_WAIT;

    GFL_UI_SoftResetSetFunc( MB_PARENT_SoftResetCallBack , work );
    break;
  case MPSS_SEND_IMAGE_INIT_COMM_WAIT:
    //if( WhCallBackFlg == TRUE )
    {
      const int state = WH_GetSystemState();
      if( state == WH_SYSSTATE_IDLE )
      {
        if( WH_StartMeasureChannel() == TRUE )
        {
          work->subState = MPSS_SEND_IMAGE_WAIT_SEARCH_CH;
        }
      }
    }
    break;
  case MPSS_SEND_IMAGE_WAIT_SEARCH_CH:
    {
      const int state = WH_GetSystemState();
      if( state == WH_SYSSTATE_MEASURECHANNEL )
      {
        MYSTATUS *myStatus = GAMEDATA_GetMyStatus(work->initWork->gameData);
        MP_PARENT_SendImage_MBPInit( work );

        MB_MSG_MessageHide( work->msgWork );
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE );
        }
        else
        {
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE2 );
        }

        MB_MSG_MessageCreateWordset( work->msgWork );
        MB_MSG_MessageWordsetNumberZero( work->msgWork , 0 , MyStatus_GetID_Low(myStatus) , 5 );

        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 , MSGSPEED_GetWait() );
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_01 , MSGSPEED_GetWait() );
        }
        MB_MSG_MessageDeleteWordset( work->msgWork );

        work->subState = MPSS_SEND_IMAGE_MBSYS_MAIN;
      }
    }
    break;
  case MPSS_SEND_IMAGE_MBSYS_MAIN:
    MP_PARENT_SendImage_MBPMain( work );
    break;

  case MPSS_SEND_IMAGE_WAIT_BOOT_INIT:
    MB_MSG_MessageHide( work->msgWork );
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_02 );
    }
    else
    {
      MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_MOVIE_02 );
    }
    MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
    work->subState = MPSS_SEND_IMAGE_WAIT_BOOT;
    break;
    
  case MPSS_SEND_IMAGE_WAIT_BOOT:
    work->isSendRom = TRUE;
    work->subState = MPSS_SEND_IMAGE_NET_EXIT;
    break;
  case MPSS_SEND_CANCEL_BOOT:
    work->subState = MPSS_SEND_IMAGE_NET_EXIT;
    break;

  case MPSS_SEND_IMAGE_NET_EXIT:
    WhCallBackFlg = FALSE;
    WH_End( &MP_PARENT_WhCallBack );
    work->isInitMbComm = FALSE;
    work->subState = MPSS_SEND_IMAGE_NET_EXIT_WAIT;
    break;
  case MPSS_SEND_IMAGE_NET_EXIT_WAIT:
    if( WhCallBackFlg == TRUE )
    {
      GFL_UI_SoftResetSetFunc( NULL , NULL );
      WH_FreeMemory();
      return TRUE;
    }
    break;
    
  }
  GFL_NET_WirelessIconEasy_SetLevel(3-WM_GetLinkLevel());
  GFL_NET_WirelessIconEasyFunc();
  return FALSE;
}

//--------------------------------------------------------------
//  MBPシステム起動
//--------------------------------------------------------------
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work )
{
#ifdef PM_DEBUG
  const char *srlPath[2] = {"/dl_rom/child.srl","/dl_rom/child2.srl"};
#else
  const char *srlPath[2] = {"/dl_rom/child_r.srl","/dl_rom/child2_r.srl"};
#endif
  const char *charPath[2] = {"/dl_rom/icon_w.char","/dl_rom/icon_b.char"};
  const char *plttPath[2] = {"/dl_rom/icon_w.plt","/dl_rom/icon_b.plt"};
  
  /* このデモがダウンロードさせるプログラム情報 */
  //staticじゃないと動かない！！！
  MBGameRegistry mbGameList = {
    NULL,    // 子機バイナリコード
    NULL ,                  // ゲーム名
    NULL ,                  // ゲーム内容説明
    NULL,                   // アイコンキャラクタデータ
    NULL,                   // アイコンパレットデータ
    MB_DEF_GGID,            // GGID
    2,                      // 最大プレイ人数、親機の数も含めた人数
  };

  const u16 channel = WH_GetMeasureChannel();
  
  GFL_STD_MemCopy( &mbGameList , &work->gameRegistry , sizeof(MBGameRegistry) );
  if( work->mode == MPM_POKE_SHIFTER )
  {
    work->gameRegistry.romFilePathp = srlPath[0];
  }
  else
  {
    work->gameRegistry.romFilePathp = srlPath[1];
  }
  
  if( GET_VERSION() == VERSION_BLACK )
  {
    work->gameRegistry.iconCharPathp = charPath[1];
    work->gameRegistry.iconPalettePathp = plttPath[1];
  }
  else
  {
    work->gameRegistry.iconCharPathp = charPath[0];
    work->gameRegistry.iconPalettePathp = plttPath[0];
  }
  
  work->gameRegistry.gameNamep = work->romTitleStr;
  work->gameRegistry.gameIntroductionp = work->romInfoStr;
  MBP_Init( MB_DEF_GGID , MB_TGID_AUTO );
  MBP_Start( &work->gameRegistry , channel );
  
}

static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work )
{
  const u16 mbpState = MBP_GetState();
  static const GFL_UI_TP_HITTBL hitTbl[2] = 
  {
    { 192-24 , 192 ,
      256-24 , 255 },
    { GFL_UI_TP_HIT_END ,0,0,0 },
  };

#if PM_DEBUG
  {
    static u16 befState = 0xFFFF;
    if( befState != mbpState )
    {
      OS_TFPrintf(3,"MBState[%d]->[%d]\n",befState,mbpState);
      befState = mbpState;
    }
  }
#endif

  //以下サンプル流用
  switch (MBP_GetState())
  {
    //-----------------------------------------
    // アイドル状態
  case MBP_STATE_IDLE:
    //ココには来ない(MP_PARENT_SendImage_MBPInitでStartしてるから)
    break;
    //-----------------------------------------
    // 子機からのエントリー受付中
  case MBP_STATE_ENTRY:
    {
      if( work->confirmState == MPCS_NONE )
      {    
        const int ret = GFL_UI_TP_HitTrg( hitTbl );
        if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B ||
            (ret == 0&&work->mode == MPM_POKE_SHIFTER) )
        {
          // Bボタンでマルチブートキャンセル
          // タッチはポケシフターのみ
          work->confirmState = MPCS_INIT;
          GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_ON );
          GFL_CLACT_WK_SetAutoAnmFlag( work->clwkReturn , TRUE );
          PMSND_PlaySE( SEQ_SE_CANCEL1 );
          break;
        }
        //子機がくれば自動でDLが始まる。キャンセル時ステート移行で再接続が
        //できなくなってしまうのでリブートチェックをここでやる。
        // エントリー中の子機が一台でも存在すれば開始可能とする
        /*
        if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
            MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
            MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
        {
          //子機が来たら始めてしまう
          {
            OS_TPrintf("[%d][%d][%d]\n",MBP_GetChildBmp(MBP_BMPTYPE_ENTRY)
                                       ,MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING)
                                       ,MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE));
            // ダウンロード開始
            //MBP_StartDownloadAll();
            //MBP_StartDownload(1);
          }
        }
        */
        if (MBP_IsBootableAll())
        {
          // ブート開始
          MBP_StartRebootAll();
        }
      }
    }
    break;

    //-----------------------------------------
    // プログラム配信処理
  case MBP_STATE_DATASENDING:
    {
//ここは来ない！
#if 0
      if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
      {
        // Bボタンでマルチブートキャンセル
        work->confirmState = MPCS_INIT;
        GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_ON );
        GFL_CLACT_WK_SetAutoAnmFlag( work->clwkReturn , TRUE );
        break;
      }
      // 全員がダウンロード完了しているならばスタート可能.
      if (MBP_IsBootableAll())
      {
        // ブート開始
        MBP_StartRebootAll();
      }
#endif
    }
    break;

    //-----------------------------------------
    // リブート処理
  case MBP_STATE_REBOOTING:
    {
    }
    break;

    //-----------------------------------------
    // 再接続処理
  case MBP_STATE_COMPLETE:
    {
      // 全員無事に接続完了したらマルチブート処理は終了し
      // 通常の親機として無線を再起動する。
      work->subState = MPSS_SEND_IMAGE_WAIT_BOOT_INIT;
    }
    break;

    //-----------------------------------------
    // エラー発生
  case MBP_STATE_ERROR:
    {
      // 通信をキャンセルする
      MBP_Cancel();
    }
    break;

    //-----------------------------------------
    // 通信キャンセル処理中
  case MBP_STATE_CANCEL:
    // None
    break;

    //-----------------------------------------
    // 通信異常終了
  case MBP_STATE_STOP:
    work->subState = MPSS_SEND_CANCEL_BOOT;
    /*
    switch (WH_GetSystemState())
    {
    case WH_SYSSTATE_IDLE:
      (void)WH_End();
      break;
    case WH_SYSSTATE_BUSY:
      break;
    case WH_SYSSTATE_STOP:
      return FALSE;
    default:
      OS_Panic("illegal state\n");
    }
    */
    break;
  }
  
  //Bキャンセルの確認処理
  switch( work->confirmState )
  {
  case MPCS_INIT:
    MB_MSG_MessageHide( work->msgWork );
    GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN_OFF );
    if( work->mode == MPM_POKE_SHIFTER )
    {
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE_UP );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_10 , MSGSPEED_GetWait() );
    }
    else
    {
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_22 , MSGSPEED_GetWait() );
    }
    work->confirmState = MPCS_WAIT_MSG;
    break;
  case MPCS_WAIT_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( work->mode == MPM_POKE_SHIFTER )
      {
        MB_MSG_DispYesNo( work->msgWork , MMYT_UP );
      }
      else
      {
        MB_MSG_DispYesNoUpper( work->msgWork , MMYT_UP );
      }
      work->confirmState = MPCS_WAIT_CONFIRM;
    }
    break;
  case MPCS_WAIT_CONFIRM:
    {
      MB_MSG_YESNO_RET ret;
      if( work->mode == MPM_POKE_SHIFTER )
      {
        ret = MB_MSG_UpdateYesNo( work->msgWork );
      }
      else
      {
        ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      }

      if( ret == MMYR_RET1 )
      {
        GF_ASSERT_MSG_HEAVY( MBP_GetState() == MBP_STATE_ENTRY , "state is not[MBP_STATE_ENTRY][%d]!!!\n",MBP_GetState() );
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_CANCEL );
        MBP_Cancel();
        work->confirmState = MPCS_END;
      }
      else
      if( ret == MMYR_RET2 )
      {
        MYSTATUS *myStatus = GAMEDATA_GetMyStatus(work->initWork->gameData);
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_ClearYesNo( work->msgWork );
        }
        else
        {
          MB_MSG_ClearYesNoUpper( work->msgWork );
        }
        MB_MSG_MessageHide( work->msgWork );
        
        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE );
        }
        else
        {
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_LARGE2 );
        }
        MB_MSG_MessageCreateWordset( work->msgWork );
        MB_MSG_MessageWordsetNumberZero( work->msgWork , 0 , MyStatus_GetID_Low(myStatus) , 5 );
        GFL_CLACT_WK_SetAnmSeq( work->clwkReturn , APP_COMMON_BARICON_RETURN );

        if( work->mode == MPM_POKE_SHIFTER )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 , MSGSPEED_GetWait() );
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_01 , MSGSPEED_GetWait() );
        }
        MB_MSG_MessageDeleteWordset( work->msgWork );
        work->confirmState = MPCS_NONE;
      }
    }
    break;
  }
}

static BOOL MP_PARENT_WhCallBack( BOOL bResult )
{
  WhCallBackFlg = TRUE;
  return TRUE;
}

static void MB_PARENT_SetFinishState( MB_PARENT_WORK *work , const u8 state )
{
  if( work->mode == MPM_POKE_SHIFTER )
  {
    const u8 nowState = MISC_GetPalparkFinishState( work->miscSave );
    u8 setState = state;
    BOOL isSet = FALSE;


    MB_TPrintf( "SetFinishState[%d->%d]\n",nowState,state );
    //連続プレイ時を考慮して、優先度の高いステートを入れる。
    switch( state )
    {
    case PALPARK_FINISH_NORMAL:    // (0)  //捕獲した
      //ハイスコア・エラー以外
      if( nowState != PALPARK_FINISH_HIGHSOCRE && 
          nowState != PALPARK_FINISH_ERROR &&
          nowState != PALPARK_FINISH_ERROR_GET )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_HIGHSOCRE: // (1)  //捕獲した＋ハイスコア
      //エラー以外
      if( nowState != PALPARK_FINISH_ERROR &&
          nowState != PALPARK_FINISH_ERROR_GET )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_NO_GET:    // (2)  //捕獲できなかった
      //キャンセルかエラーの時
      if( nowState != PALPARK_FINISH_NORMAL &&
          nowState != PALPARK_FINISH_HIGHSOCRE && 
          nowState != PALPARK_FINISH_ERROR &&
          nowState != PALPARK_FINISH_ERROR_GET )
      {
        isSet = TRUE;
      }
      break;

    case PALPARK_FINISH_ERROR:     // (3)  //エラー終了
      //エラーは絶対
      isSet = TRUE;
      if( work->totalGet > 0 )
      {
        setState = PALPARK_FINISH_ERROR_GET;
      }
      break;

    case PALPARK_FINISH_CANCEL:    // (4)  //キャンセル終了
      //一番優先度が低い
      if( nowState != PALPARK_FINISH_NO_GET &&
          nowState != PALPARK_FINISH_NORMAL &&
          nowState != PALPARK_FINISH_HIGHSOCRE && 
          nowState != PALPARK_FINISH_ERROR &&
          nowState != PALPARK_FINISH_ERROR_GET )
      {
        isSet = TRUE;
      }
      break;
    }
    
    if( isSet == TRUE )
    {
      MB_TPrintf( "SetFinishState Set!!![%d->%d]\n",nowState,setState );
      MISC_SetPalparkFinishState( work->miscSave , setState );
    }
  }
}

static void MB_PARENT_SoftResetCallBack( void *pWork )
{
  //ソフトリセット時、自前で終了処理
  MB_PARENT_WORK *work = pWork;
  
  if( MBP_IsInitNet() == TRUE )
  {
    MBP_Cancel();
  }
  while( MBP_GetState() != MBP_STATE_STOP &&
         MBP_GetState() != MBP_STATE_COMPLETE )
  {
    OS_TPrintf("WaitA!\n");
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
  }
  if( work->isInitMbComm == TRUE )
  {
    WhCallBackFlg = FALSE;
    while( WH_End( &MP_PARENT_WhCallBack ) )
    {
      OS_TPrintf("WaitC!\n");
      OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }
    
    while( WhCallBackFlg == FALSE )
    {
      OS_TPrintf("WaitB!\n");
      OS_WaitIrq(TRUE, OS_IE_V_BLANK);
    }
  }
}

#pragma mark [>save func
//--------------------------------------------------------------
//  セーブ準備
//--------------------------------------------------------------
static void MB_PARENT_SaveInit( MB_PARENT_WORK *work )
{
  const u8 pokeNum = MB_COMM_GetPostPokeNum( work->commWork );
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_07 );
  MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
  
  MB_PARENT_SaveInitPoke( work );
  //スコアチェック
  if( work->mode == MPM_POKE_SHIFTER )
  {
    const u16 nowScore = MISC_GetPalparkHighscore(work->miscSave);
    const u16 newScore = MB_COMM_GetScore( work->commWork );
    if( nowScore < newScore )
    {
      MISC_SetPalparkHighscore(work->miscSave,newScore);
      MB_PARENT_SetFinishState( work , PALPARK_FINISH_HIGHSOCRE );
    }
    else
    {
      if( pokeNum == 0 )
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_NO_GET );
      }
      else
      {
        MB_PARENT_SetFinishState( work , PALPARK_FINISH_NORMAL );
      }
    }
    if( work->localHighScore < newScore )
    {
      work->localHighScore = newScore;
    }
    work->totalGet += pokeNum;
  }
  
  MB_TPrintf( "MB_Parent Save Init\n" );
}

//映画のためにポケを分離
static void MB_PARENT_SaveInitPoke( MB_PARENT_WORK *work )
{
  u8 i;
  BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
  ZUKAN_SAVEDATA* zukan_savedata = GAMEDATA_GetZukanSave( work->initWork->gameData );
  const u8 pokeNum = MB_COMM_GetPostPokeNum( work->commWork );

  if( work->mode == MPM_POKE_SHIFTER )
  {
    RECORD *record = GAMEDATA_GetRecordPtr(work->initWork->gameData);
    RECORD_Add(record,RECID_CAPTURE_POKE,pokeNum);
    RECORD_Add(record,RECID_DAYCNT_CAPTURE,pokeNum);
  }
  MB_TPrintf( "Num[%d]\n",pokeNum );
  for( i=0;i<pokeNum;i++ )
  {
    POKEMON_PASO_PARAM *ppp = MB_COMM_GetPostPokeData( work->commWork , i );
    //親機の時間で再設定する
    POKE_MEMO_SetTrainerMemoPokeShifterAfterTrans( ppp );
    {
      const BOOL ret = BOXDAT_PutPokemon( boxMng , ppp );
      POKEMON_PARAM *pp = PP_CreateByPPP( ppp , work->heapId );
      ZUKANSAVE_SetPokeGet( zukan_savedata , pp );
      GFL_HEAP_FreeMemory( pp );
      
      GF_ASSERT_MSG_HEAVY( ret == TRUE , "Multiboot parent Box is full!!\n");
#if DEB_ARI
      {
        char name[32];
        STRBUF *nameStr = GFL_STR_CreateBuffer( 32 , work->heapId );
        u8 nickName = PPP_Get( ppp , ID_PARA_nickname_flag , NULL );
        PPP_Get( ppp , ID_PARA_nickname , nameStr );
        DEB_STR_CONV_StrBufferToSjis( nameStr , name , 32 );
        MB_TPrintf("[%d][%s][%d]\n",i,name,nickName);
        GFL_STR_DeleteBuffer( nameStr );
      }
#endif
    }
  }
}

//--------------------------------------------------------------
//  セーブ開放
//--------------------------------------------------------------
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work )
{
}

//--------------------------------------------------------------
//  セーブ更新
//--------------------------------------------------------------
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SAVE_WAIT_SAVE_INIT:
    if( MB_MSG_CheckPrintQueIsFinish( work->msgWork ) == TRUE &&
        MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      if( MB_COMM_GetIsReadyChildStartSave( work->commWork ) == TRUE )
      {
        if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_START_SAVE , GFUser_GetPublicRand(20)+10  ) == TRUE )
        {
          work->subState = MPSS_SAVE_SYNC_SAVE_INIT;
        }
      }
    }
    break;
  case MPSS_SAVE_SYNC_SAVE_INIT:
    if( MB_COMM_GetIsPermitStartSave( work->commWork ) == TRUE )
    {
      work->subState = MPSS_SAVE_INIT;
    }
    break;
    
  case MPSS_SAVE_INIT:
    GAMEDATA_SaveAsyncStart( work->initWork->gameData );
    work->subState = MPSS_SAVE_WAIT_FIRST;
    MB_TPrintf( "MB_Parent Save Start!\n" );
    break;

  case MPSS_SAVE_WAIT_FIRST:
    {
      const SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( work->initWork->gameData );
      if( ret == SAVE_RESULT_LAST )
      {
        MB_TPrintf( "MB_Parent Finish First!\n" );
  			work->subState = MPSS_SAVE_WAIT_FIRST_SYNC;
      }
    }
    break;

  case MPSS_SAVE_WAIT_FIRST_SYNC:
    if( MB_COMM_GetIsFinishChildFirstSave( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_FIRST_SAVE , GFUser_GetPublicRand(20)+10  ) == TRUE )
      {
  			work->subState = MPSS_SAVE_FIRST_SAVE_LAST;
  			work->saveWaitCnt = 0;
  		}
    }
    break;

  case MPSS_SAVE_FIRST_SAVE_LAST:
    if( MB_COMM_GetIsPermitFirstSave( work->commWork ) == TRUE )
    {
      work->saveWaitCnt++;
      if( work->saveWaitCnt > MB_COMM_GetSaveWaitTime( work->commWork ) == TRUE )
      {
        MB_TPrintf( "MB_Parent FirstLast Save!\n" );
        work->subState = MPSS_SAVE_WAIT_SECOND;
      }
    }
    break;

  case MPSS_SAVE_WAIT_SECOND:
    {
      const SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( work->initWork->gameData );
  		if( ret == SAVE_RESULT_OK )
  		{
        work->subState = MPSS_SAVE_WAIT_SECOND_SYNC;
        MB_TPrintf( "MB_Parent Finish Second!\n" );
        //親機は以降特にセーブはしない
      }
    }
    
    break;

  case MPSS_SAVE_WAIT_SECOND_SYNC:
    if( MB_COMM_GetIsFinishChildSecondSave( work->commWork ) == TRUE )
    {
      //特に待つ必要は無いので、待ち時間は０で送っている
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_SECOND_SAVE , 0  ) == TRUE )
      {
        work->subState = MPSS_SAVE_SECOND_SAVE_LAST;
  			work->saveWaitCnt = 0;
  		}
    }
    break;

  case MPSS_SAVE_SECOND_SAVE_LAST:
    if( MB_COMM_GetIsFinishChildSave( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_PERMIT_FINISH_SAVE , 0  ) == TRUE )
      {
        work->subState = MPSS_SAVE_WAIT_FINISH_SAVE_SYNC;
      }
    }
    break;
    
  case MPSS_SAVE_WAIT_FINISH_SAVE_SYNC:
    if( MB_COMM_GetIsPermitFinishSave( work->commWork ) == TRUE )
    {
      work->state = MPS_SAVE_TERM;
      MB_TPrintf( "MB_Parent Finish All Save Seq!!!\n" );
    }
    break;
  }
}

#pragma mark [>proc movie
static void MB_PARENT_UpdateMovieMode( MB_PARENT_WORK *work )
{
  
#if PM_DEBUG
  {
    static int besState = 0xFFFF;
    if( besState != work->state )
    {
      OS_TFPrintf(3,"movie state[%d]->[%d]\n",besState,work->state);
      besState = work->state;
    }
  }
#endif
  //共通部分から分岐してくる。
  switch( work->movieState )
  {
  //子機のポケ集計待ち
  case MPMS_WAIT_COUNT_POKE:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR ||
        MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR_ANOTHER_COUNTRY )
    {
      //読み込みエラーが発生した
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_21 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM_MOVIE;
    }
    if( MB_COMM_GetChildState(work->commWork) == MCCS_END_GAME_ERROR_BACKUP_LOAD )
    {
      //読み込みエラーが発生した
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_24 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_COMM_ReqDisconnect( work->commWork );
      work->state = MPS_EXIT_COMM_MOVIE;
    }
    if( MB_COMM_IsPostMoviePokeNum( work->commWork ) == TRUE )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );
      const u16 hidenNum = MB_COMM_GetMoviePokeNumHiden( work->commWork );
      if( num > 0 )
      {
        MB_MSG_MessageHide( work->msgWork );

        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageCreateWordset( work->msgWork );
        //ここでの表示は秘伝込み
        MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num+hidenNum , 3 );
        if( GET_VERSION() == VERSION_BLACK )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_03_B , MSGSPEED_GetWait() );
        }
        else
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_03_W , MSGSPEED_GetWait() );
        }
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        MB_MSG_MessageDeleteWordset( work->msgWork );

        work->movieState = MPMS_CONFIRM_POKE_WAIT_MSG;
      }
      else
      {
        work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
      }

      MB_TPrintf( "MB_Parent post movie poke[%d]\n",num );
    }
    break;
  
  //連れてきますか？表示待ち
  case MPMS_CONFIRM_POKE_WAIT_MSG:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_POKE_WAIT_YESNO;
    }
    break;
  
  //連れてきますか？はい・いいえ待ち
  case MPMS_CONFIRM_POKE_WAIT_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      work->yesNoRet = ret;
      if( ret == MMYR_RET1 ) 
      {
        const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );
        BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
        const u16 leastBoxNum = BOXDAT_GetEmptySpaceTotal( boxMng );

        MB_MSG_ClearYesNoUpper( work->msgWork );
        
        if( leastBoxNum < num )
        {
          //ボックスが足りない！
          work->isBoxNotEnough = TRUE;
          work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
        }
        else
        {
          //秘伝チェックへ
          work->isBoxNotEnough = FALSE;
          work->movieState = MPMS_CONFIRM_HIDEN_WARN_INIT;
        }
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
    }
    break;
    
  //秘伝チェックへ
  case MPMS_CONFIRM_HIDEN_WARN_INIT:
    if( MB_COMM_GetMoviePokeNumHiden( work->commWork ) > 0 )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );

      MB_MSG_MessageHide( work->msgWork );

      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      MB_MSG_MessageCreateWordset( work->msgWork );
      MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num , 3 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_16 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_MSG_MessageDeleteWordset( work->msgWork );

      work->movieState = MPMS_CONFIRM_HIDEN_WARN_WAIT;
    }
    else
    {
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
    }
    break;
  
  //秘伝警告表示待ち
  case MPMS_CONFIRM_HIDEN_WARN_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
    }
    break;
  
  //アイテムの個数チェック
  case MPMS_CONFIRM_CHECK_ITEM_INIT:
    if( MB_COMM_GetMoviePokeNumHaveItem( work->commWork ) == TRUE )
    {
      MB_MSG_MessageHide( work->msgWork );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_06 , MSGSPEED_GetWait() );
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_WAIT;
    }
    else
    {
      work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
    }
    break;
  
  //アイテム戻し確認表示待ち
  case MPMS_CONFIRM_CHECK_ITEM_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CHECK_ITEM_YESNO;
    }
    break;
  
  //アイテム戻しはい・いいえ表示待ち
  case MPMS_CONFIRM_CHECK_ITEM_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        if( MB_COMM_GetMoviePokeNumFullItem( work->commWork ) == TRUE )
        {
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_07 , MSGSPEED_GetWait() );
          work->movieState = MPMS_CONFIRM_CHECK_FULL_WAIT;
        }
        else
        {
          work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
        }
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CANCEL_INIT;
      }
    }
    break;

  //アイテム一杯確認表示待ち
  case MPMS_CONFIRM_CHECK_FULL_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CHECK_FULL_YESNO;
    }
    break;
    
  //アイテム一杯。はい・いいえ待ち
  case MPMS_CONFIRM_CHECK_FULL_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CANCEL_INIT;
      }
    }
    break;
    
  //アイテム系キャンセル確認
  case MPMS_CONFIRM_CANCEL_INIT:
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_08 , MSGSPEED_GetWait() );
    work->movieState = MPMS_CONFIRM_CANCEL_WAIT;
    break;

  //アイテム系キャンセル表示待ち
  case MPMS_CONFIRM_CANCEL_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
      work->movieState = MPMS_CONFIRM_CANCEL_YESNO;
    }
    break;

  //アイテム系キャンセル。はい・いいえ待ち
  case MPMS_CONFIRM_CANCEL_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 )
      {
        work->yesNoRet = MMYR_RET2;
        work->movieState = MPMS_CONFIRM_POKE_SEND_YESNO;
      }
      else
      if( ret == MMYR_RET2 )
      {
        work->movieState = MPMS_CONFIRM_CHECK_ITEM_INIT;
      }
    }
    break;
  
  //子機に選択の結果を送る
  case MPMS_CONFIRM_POKE_SEND_YESNO:
    {
      BOOL ret;
      MB_COMM_MOVIE_VALUE sendVal;
      if( work->yesNoRet == MMYR_RET1 )
      {
        if( work->isBoxNotEnough == TRUE )
        {
          sendVal = MCMV_POKETRANS_NG;
        }
        else
        {
          sendVal = MCMV_POKETRANS_YES;
        }
      }
      else
      {
        sendVal = MCMV_POKETRANS_NO;
      }
      ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_POKE_TRANS_CONFIRM , sendVal );
      if( ret == TRUE )
      {
        if( sendVal == MCMV_POKETRANS_YES )
        {
          work->movieState = MPMS_POST_POKE_WAIT;
          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_MOVIE_05 );
          MB_MSG_SetDispTimeIcon( work->msgWork , TRUE );
          
        }
        else
        if( sendVal == MCMV_POKETRANS_NG )
        {
          work->movieState = MPMS_BOX_NOT_ENOUGH_INIT;
        }
        else
        {
          work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
        }
      }
    }
    break;
    
  case MPMS_POST_POKE_WAIT:
    if( MB_COMM_IsPostPoke( work->commWork ) == TRUE )
    {
      MB_PARENT_SaveInitPoke( work );
      work->movieState = MPMS_POST_POKE_RET_POST;
    }
    if( MB_COMM_IsPostMoviePokeFinishSend( work->commWork ) == TRUE )
    {
      const u16 num = MB_COMM_GetMoviePokeNum( work->commWork );

      MB_MSG_MessageHide( work->msgWork );
      MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
      MB_MSG_MessageCreateWordset( work->msgWork );
      MB_MSG_MessageWordsetNumber( work->msgWork , 0 , num , 3 );
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_10 , MSGSPEED_GetWait() );
      MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      MB_MSG_MessageDeleteWordset( work->msgWork );
      work->movieState = MPMS_POST_POKE_FINISH;
      work->isPostMoviePoke = TRUE;
      MB_TPrintf( "MB_Parent Finish poke trans\n" );
    }
    break;
    
  case MPMS_POST_POKE_RET_POST:
    if( MB_COMM_Send_Flag( work->commWork , MCFT_POST_POKE , 0 ) == TRUE )
    {
      MB_COMM_ClearSendPokeData( work->commWork );
      work->movieState = MPMS_POST_POKE_WAIT;
    }
    break;
  
  case MPMS_POST_POKE_FINISH:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
    }
    break;

  case MPMS_BOX_NOT_ENOUGH_INIT:
    MB_MSG_MessageHide( work->msgWork );
    MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_04 , MSGSPEED_GetWait() );
    work->movieState = MPMS_BOX_NOT_ENOUGH_WAIT;
    break;

  case MPMS_BOX_NOT_ENOUGH_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_WAIT_CHECK_LOCK_CAPSULE;
    }
    break;

  case MPMS_WAIT_CHECK_LOCK_CAPSULE:
    if( MB_COMM_IsPostMovieHaveLockCapsule( work->commWork ) == TRUE )
    {
      if( MB_COMM_IsMovieHaveLockCapsule( work->commWork ) == TRUE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_2LINE );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_11 , MSGSPEED_GetWait() );

        work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_WAIT;
      }
      else
      {
        work->movieState = MPMS_CHECK_SAVE;
      }
    }
    break;
  
  case MPMS_CONFIRM_LOCK_CAPSULE_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_YESNO;
      MB_MSG_DispYesNoUpper( work->msgWork , MMYT_MID );
    }
    break;
    
  case MPMS_CONFIRM_LOCK_CAPSULE_YESNO:
    {
      const MB_MSG_YESNO_RET ret = MB_MSG_UpdateYesNoUpper( work->msgWork );
      if( ret == MMYR_RET1 || ret == MMYR_RET2 )
      {
        MYITEM_PTR myItem = GAMEDATA_GetMyItem( work->initWork->gameData );
        MYSTATUS *myStatus = GAMEDATA_GetMyStatus( work->initWork->gameData );
        MISC *miscData = GAMEDATA_GetMiscWork( work->initWork->gameData );

        if( ret == MMYR_RET1 &&
            EVTLOCK_CheckEvtLock( miscData , EVT_LOCK_NO_LOCKCAPSULE , myStatus ) == TRUE )
//           (MYITEM_CheckItem( myItem , ITEM_ROKKUKAPUSERU , 1 , work->heapId ) == TRUE ||
//            MYITEM_CheckItem( myItem , ITEM_WAZAMASIN95 , 1 , work->heapId ) == TRUE) )
        {
          //もう持ってる
          work->yesNoRet = MMYR_RET2; //強制いいえ
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT;

          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_17 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
        }
        else
        {
          work->yesNoRet = ret;
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO;
        }
        
      }
    }
    break;
    
  case MPMS_CONFIRM_LOCK_CAPSULE_POST_YET_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO;
    }
    break;
  
  case MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO:
    {
      const BOOL flg = ( work->yesNoRet == MMYR_RET1 ? TRUE : FALSE );
      const BOOL ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_LOCK_CAPSULE_TRANS_CONFIRM , flg );
      if( ret == TRUE )
      {
        if( flg == TRUE )
        {
          MYITEM_PTR myItem = GAMEDATA_GetMyItem( work->initWork->gameData );
          MYSTATUS *myStatus = GAMEDATA_GetMyStatus( work->initWork->gameData );
          MISC *miscData = GAMEDATA_GetMiscWork( work->initWork->gameData );
          MB_MSG_MessageHide( work->msgWork );
          MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
          MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_13 , MSGSPEED_GetWait() );
          MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
          
          EVTLOCK_SetEvtLock( miscData , EVT_LOCK_NO_LOCKCAPSULE , myStatus );
          MYITEM_AddItem( myItem , ITEM_ROKKUKAPUSERU , 1 , work->heapId );
          work->isPostMovieCapsule = TRUE;
          work->movieState = MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT;
        }
        else
        {
          work->movieState = MPMS_CHECK_SAVE;
        }
      }
    }
    break;

  case MPMS_CONFIRM_LOCK_CAPSULE_SEND_YESNO_WAIT:
    if( MB_MSG_CheckPrintStreamIsFinish( work->msgWork ) == TRUE )
    {
      work->movieState = MPMS_CHECK_SAVE;
    }
    break;
    
  case MPMS_CHECK_SAVE:
    if( work->isPostMoviePoke == TRUE || work->isPostMovieCapsule == TRUE )
    {
      const BOOL ret = MB_COMM_Send_Flag( work->commWork , MCFT_MOVIE_START_SAVE_CHECK , 0 );
      if( ret == TRUE )
      {
        work->state = MPS_WAIT_CRC_CHECK;
      }
    }
    else
    {
      if( MB_COMM_GetMoviePokeNum( work->commWork ) == 0 &&
          MB_COMM_IsMovieHaveLockCapsule( work->commWork ) == FALSE )
      {
        MB_MSG_MessageHide( work->msgWork );
        MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_MOVIE_20 , MSGSPEED_GetWait() );
        MB_MSG_SetDispKeyCursor( work->msgWork , TRUE );
      }
      //先に通信を切る
      work->state = MPS_EXIT_COMM_MOVIE;
    }
    break;
  }
}

#pragma mark [>pull out
static BOOL  MB_PARENT_PullOutCallBack( void )
{
  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );
  
  if( errIsShifter == TRUE )
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_SUB_SSMSG , TRUE );
  }
  else
  {
    GFL_BG_SetVisible( MB_PARENT_FRAME_BG , TRUE );
  }
  return TRUE;
}

#pragma mark [>proc func
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

GFL_PROC_DATA MultiBoot_ProcData =
{
  MB_PARENT_ProcInit,
  MB_PARENT_ProcMain,
  MB_PARENT_ProcTerm
};

//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcInit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_INIT_WORK *initWork;
  MB_PARENT_WORK *work;
  
  if( GFL_NET_IsExit() == FALSE )
  {
    OS_TPrintf("!\n");
    return GFL_PROC_RES_CONTINUE;
  }
  
  MB_TPrintf("LeastAppHeap[%x]\n",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x130000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_PARENT_WORK), HEAPID_MULTIBOOT );
  if( pwk == NULL )
  {
    initWork = GFL_HEAP_AllocMemory( HEAPID_MULTIBOOT , sizeof( MB_PARENT_INIT_WORK ));
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      MB_TPrintf("Boot mode movie!\n");
      initWork->mode = MPM_MOVIE_TRANS;
    }
    else
    {
      MB_TPrintf("Boot mode poke shifter!\n");
      initWork->mode = MPM_POKE_SHIFTER;
      initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
    }
  }
  else
  {
    initWork = pwk;
  }
  
  if( initWork->mode == MPM_MOVIE_TRANS )
  {
    initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
  }
  
  work->heapId = HEAPID_MULTIBOOT;
  work->initWork = initWork;
  work->isNetErr = FALSE;
  work->isNetErrMb = FALSE;
  work->isNetErrSave = FALSE;
  
  MB_PARENT_Init( work );
  
#if MP_PARENT_DEB
  MB_PARENT_InitDebug( work );
#endif

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  //ワーク解放後に使うので保持
  BOOL isMovieTrans = FALSE;
  BOOL isNetErr = work->isNetErr;
  BOOL isSave = work->isNetErrSave;
  //GFBTS1978類似
  BOOL isNetErrMb = work->isNetErrMb;
  
#if MP_PARENT_DEB
  MB_PARENT_TermDebug( work );
#endif

  if( work->initWork->mode == MPM_MOVIE_TRANS )
  {
    isMovieTrans = TRUE;
  }
  
  MB_PARENT_Term( work );

  if( pwk == NULL )
  {
    GAMEDATA_Delete( work->initWork->gameData );
    GFL_HEAP_FreeMemory( work->initWork );
  }
  else
  {
    if( work->initWork->mode == MPM_MOVIE_TRANS )
    {
      GAMEDATA_Delete( work->initWork->gameData );
      GFL_HEAP_FreeMemory( work->initWork );
    }
  }
  
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

  if( isNetErr == TRUE )
  {
    //映画の処理(自前でエラーを出す
    if( isMovieTrans )
    {
      if( isSave == TRUE ||
          NetErr_App_CheckError() == NET_ERR_CHECK_HEAVY )
      {
        //セーブ中だったら電源切断へ
        NetErr_ExitNetSystem();
        NetErr_DispCall( TRUE );
      }
      else
      {
        NetErr_DispCall( FALSE );
      }
    }
    else
    {
      //ポケシフターはセーブ中のみ電源を切る
      if( isSave == TRUE )
      {
        //セーブ中だったら電源切断へ
        NetErr_ExitNetSystem();
        NetErr_DispCall( TRUE );
      }
    }
  }
  //マルチブートでのエラーは強制エラー
  //GFBTS1978類似
  //if( work->isNetErrMb == TRUE )
  if( isNetErrMb == TRUE )
  {
    NetErr_DispCallPushPop();
  }

  if( isMovieTrans == TRUE )
  {
    GFL_UI_SoftReset( 0 );
  }

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  ループ
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcMain( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  const BOOL ret = MB_PARENT_Main( work );
  
  if( ret == TRUE )
  {
    return GFL_PROC_RES_FINISH;
  }
  
  if( NetErr_App_CheckError() != NET_ERR_CHECK_NONE &&
      work->isNetErr == FALSE )
  {
    NetErr_App_ReqErrorDisp();
    work->isNetErr = TRUE;
  }
  
  return GFL_PROC_RES_CONTINUE;
}



#if MP_PARENT_DEB

#include "debug/debugwin_sys.h"

static const BOOL MB_PARENT_Debug_UpdateValue_u16( u16 *val , u16 max );
static void MPD_U_LocalHight( void* userWork , DEBUGWIN_ITEM* item );
static void MPD_D_LocalHight( void* userWork , DEBUGWIN_ITEM* item );

#define MB_PARENT_DEB_GROUP_TOP (50)
static void MB_PARENT_InitDebug( MB_PARENT_WORK *work )
{
  GFL_FONT *fontHandle = MB_MSG_GetFont( work->msgWork );
  
  DEBUGWIN_InitProc( MB_PARENT_FRAME_SSMSG , fontHandle );

  DEBUGWIN_AddGroupToTop( MB_PARENT_DEB_GROUP_TOP , "マルチブート" , work->heapId );
  DEBUGWIN_AddItemToGroupEx( MPD_U_LocalHight ,MPD_D_LocalHight , (void*)work , MB_PARENT_DEB_GROUP_TOP , work->heapId );


  DEBUGWIN_ChangeLetterColor( 0,0,0 );
}

static void MB_PARENT_TermDebug( MB_PARENT_WORK *work )
{
  DEBUGWIN_RemoveGroup( MB_PARENT_DEB_GROUP_TOP );
  DEBUGWIN_ExitProc();
}

static const BOOL MB_PARENT_Debug_UpdateValue_u16( u16 *val , u16 max )
{
  u16 value;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){ value = 10; }
  else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){ value = 100; }
  else{ value = 1; }

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    *val += value;
    if( *val > max )
    {
      *val = 0;
    }
    return TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( *val - value < 0 )
    {
      *val = max;
    }
    else
    {
      *val -= value;
    }
    return TRUE;
  }
  return FALSE;
}

static void MPD_U_LocalHight( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_PARENT_WORK *work = (MB_PARENT_WORK*)userWork;
  const BOOL ret = MB_PARENT_Debug_UpdateValue_u16( &work->localHighScore , 999 );
  if( ret == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void MPD_D_LocalHight( void* userWork , DEBUGWIN_ITEM* item )
{
  MB_PARENT_WORK *work = (MB_PARENT_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "HighScore[%d]",work->localHighScore );
}

#endif
