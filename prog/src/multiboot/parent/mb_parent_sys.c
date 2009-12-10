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
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "net/wih.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/wipe.h"

#include "arc_def.h"
#include "mb_parent.naix"

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
#define MB_PARENT_FRAME_MSG (GFL_BG_FRAME1_M)
#define MB_PARENT_FRAME_BG  (GFL_BG_FRAME3_M)

#define MB_PARENT_FRAME_SUB_BG  (GFL_BG_FRAME3_S)

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
  
  MPS_SEND_IMAGE_INIT,
  MPS_SEND_IMAGE_MAIN,
  MPS_SEND_IMAGE_TERM,

  MPS_SEND_INIT_DATA,

  MPS_SEND_GAMEDATA_INIT,
  MPS_SEND_GAMEDATA_SEND,

  MPS_WAIT_SELBOX,
  MPS_WAIT_FINISH_SELBOX,
  MPS_WAIT_POST_GAMEDATA,
  MPS_WAIT_FINISH_CAPTURE,
  MPS_WAIT_SEND_POKE,

  MPS_SAVE_INIT,
  MPS_SAVE_MAIN,
  MPS_SAVE_TERM,

}MB_PARENT_STATE;

typedef enum
{
  MPSS_SEND_IMAGE_INIT_COMM,
  MPSS_SEND_IMAGE_INIT_COMM_WAIT,
  MPSS_SEND_IMAGE_WAIT_SEARCH_CH,
  MPSS_SEND_IMAGE_MBSYS_MAIN,
  
  MPSS_SEND_IMAGE_WAIT_BOOT_INIT,
  MPSS_SEND_IMAGE_WAIT_BOOT,
  MPSS_SEND_IMAGE_FINISH_BOOT,
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


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  MB_PARENT_INIT_WORK *initWork;
  MB_COMM_WORK *commWork;
  
  MB_PARENT_STATE state;
  u8              subState;
  u8              saveWaitCnt;
  
  //SendImage
  u16    *romTitleStr;  //DLROMタイトル
  u16    *romInfoStr;   //DLROM説明
  
  MB_COMM_INIT_DATA initData;
  void* gameData;
  u32   gameDataSize;
  //メッセージ用
  MB_MSG_WORK *msgWork;
  
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
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


static void MB_PARENT_Init( MB_PARENT_WORK *work );
static void MB_PARENT_Term( MB_PARENT_WORK *work );
static const BOOL MB_PARENT_Main( MB_PARENT_WORK *work );

static void MB_PARENT_InitGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work );
static void MB_PARENT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );
static void MB_PARENT_LoadResource( MB_PARENT_WORK *work );

static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work );
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work );

static void MB_PARENT_SaveInit( MB_PARENT_WORK *work );
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work );
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work );


//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
static void MB_PARENT_Init( MB_PARENT_WORK *work )
{
  work->state = MPS_FADEIN;
  
  MB_PARENT_InitGraphic( work );
  MB_PARENT_LoadResource( work );
  work->msgWork = MB_MSG_MessageInit( work->heapId , MB_PARENT_FRAME_MSG , FILE_MSGID_MB );
  MB_MSG_MessageCreateWindow( work->msgWork , MMWT_NORMAL );
  
  work->commWork = MB_COMM_CreateSystem( work->heapId );
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static void MB_PARENT_Term( MB_PARENT_WORK *work )
{
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
      work->state = MPS_SEND_IMAGE_INIT;
    }
    break;
    
  case MPS_FADEOUT:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->state = MPS_WAIT_FADEOUT;
    break;
  case MPS_WAIT_FADEOUT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
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
    if( work->subState == MPSS_SEND_IMAGE_FINISH_BOOT )
    {
      //起動成功
      work->state = MPS_SEND_INIT_DATA;
    }
    else
    {
      //起動失敗
      work->state = MPS_FADEOUT;
    }
    break;
    
  //起動後
  case MPS_SEND_INIT_DATA:
    work->initData.msgSpeed = MSGSPEED_GetWait();
    if( MB_COMM_Send_InitData( work->commWork , &work->initData ) == TRUE )
    {
      work->state = MPS_SEND_GAMEDATA_INIT;
    }
    break;
    
  case MPS_SEND_GAMEDATA_INIT:
    {
      FSFile file;
      BOOL result;
      const char* arcPath = GFUser_GetFileNameByArcID( ARCID_MB_CAPTER );
      //読み出す
      FS_InitFile( &file );
      result = FS_OpenFile(&file,arcPath);
      GF_ASSERT( result );
      FS_SeekFileToBegin( &file );
      work->gameDataSize = FS_GetLength( &file );
      work->gameData = GFL_HEAP_AllocClearMemory( work->heapId , work->gameDataSize );
      FS_ReadFile( &file,work->gameData,work->gameDataSize );
      result = FS_CloseFile( &file );
      GF_ASSERT( result );
      MB_TPrintf( "[%d]\n",work->gameDataSize );
    }
    work->state = MPS_SEND_GAMEDATA_SEND;
    break;

  case MPS_SEND_GAMEDATA_SEND:
    MB_COMM_InitSendGameData( work->commWork , work->gameData , work->gameDataSize );
    work->state = MPS_WAIT_SELBOX;
    break;
    
  case MPS_WAIT_SELBOX:
    if( MB_COMM_GetChildState(work->commWork) == MCCS_SELECT_BOX )
    {
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
      MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_06 , MSGSPEED_GetWait() );
      work->state = MPS_WAIT_SEND_POKE;
    }
    //TODO 一気に次のゲームへ飛ぶ場合がある
    break;

  case MPS_WAIT_SEND_POKE:
    if( MB_COMM_IsPostPoke( work->commWork ) == TRUE )
    {
      if( MB_COMM_Send_Flag( work->commWork , MCFT_POST_POKE , 0 ) == TRUE )
      {
        work->state = MPS_SAVE_INIT;
      }
    }
    break;
    
  case MPS_SAVE_INIT:
    if( MB_COMM_IsPost_PostPoke( work->commWork ) == TRUE )
    {
      MB_PARENT_SaveInit( work );

      work->state = MPS_SAVE_MAIN;
      work->subState = MPSS_SAVE_WAIT_SAVE_INIT;
    }
    break;

  case MPS_SAVE_MAIN:
    MB_PARENT_SaveMain( work );
    break;

  case MPS_SAVE_TERM:
    MB_PARENT_SaveTerm( work );
    break;

  }

  MB_MSG_MessageMain( work->msgWork );
  
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START &&
      GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
  {
    return TRUE;
  }
  return FALSE;
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

    // BG3 SUB (背景
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    MB_PARENT_SetupBgFunc( &header_main1 , MB_PARENT_FRAME_MSG , GFL_BG_MODE_TEXT );
    //MB_PARENT_SetupBgFunc( &header_main2 , LTVT_FRAME_CHARA , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_main3 , MB_PARENT_FRAME_BG , GFL_BG_MODE_TEXT );
    MB_PARENT_SetupBgFunc( &header_sub3  , MB_PARENT_FRAME_SUB_BG , GFL_BG_MODE_TEXT );
    
  }
  
}

static void MB_PARENT_TermGraphic( MB_PARENT_WORK *work )
{
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_MSG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_BG );
  GFL_BG_FreeBGControl( MB_PARENT_FRAME_SUB_BG );
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
                    MB_PARENT_FRAME_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_parent_bg_main_NSCR , 
                    MB_PARENT_FRAME_BG ,  0 , 0, FALSE , work->heapId );
  
  GFL_ARC_CloseDataHandle( arcHandle );
}


#pragma mark [>SendImage func
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work );
static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work );

//--------------------------------------------------------------
//  ROM送信部分 初期化
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Init( MB_PARENT_WORK *work )
{
  int i;
  STRBUF *titleStr;
  STRBUF *infoStr;
  u16 titleLen,infoLen;
  
  work->subState = MPSS_SEND_IMAGE_INIT_COMM;
  work->romTitleStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_NAME_LENGTH*2 );
  work->romInfoStr = GFL_HEAP_AllocClearMemory( work->heapId , MB_GAME_INTRO_LENGTH*2 );
  MB_MSG_MessageCreateWordset( work->msgWork );
  MB_MSG_MessageWordsetName( work->msgWork , 0 , GAMEDATA_GetMyStatus(work->initWork->gameData) );
  {
    STRBUF *workStr = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_TITLE );
    titleStr = GFL_STR_CreateBuffer( 128 , work->heapId );
    WORDSET_ExpandStr( MB_MSG_GetWordSet(work->msgWork) , titleStr , workStr );
    GFL_STR_DeleteBuffer( workStr );
  }
  MB_MSG_MessageDeleteWordset( work->msgWork );

  infoStr  = GFL_MSG_CreateString( MB_MSG_GetMsgHandle(work->msgWork) , MSG_MB_PAERNT_ROM_INFO );
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
}

//--------------------------------------------------------------
//  ROM送信部分 開放
//--------------------------------------------------------------
static void MP_PARENT_SendImage_Term( MB_PARENT_WORK *work )
{
  GFL_HEAP_FreeMemory( work->romTitleStr );
  GFL_HEAP_FreeMemory( work->romInfoStr );
}

//--------------------------------------------------------------
//  ROM送信部分 更新
//--------------------------------------------------------------
static const BOOL MP_PARENT_SendImage_Main( MB_PARENT_WORK *work )
{
  switch( work->subState )
  {
  case MPSS_SEND_IMAGE_INIT_COMM:
    MB_COMM_InitComm( work->commWork );
    work->subState = MPSS_SEND_IMAGE_INIT_COMM_WAIT;
    break;
  case MPSS_SEND_IMAGE_INIT_COMM_WAIT:
    if( MB_COMM_IsInitComm( work->commWork ) == TRUE )
    {
      if( WH_StartMeasureChannel() == TRUE )
      {
        work->subState = MPSS_SEND_IMAGE_WAIT_SEARCH_CH;
      }
    }
    break;
  case MPSS_SEND_IMAGE_WAIT_SEARCH_CH:
    if( WH_GetSystemState() == WH_SYSSTATE_MEASURECHANNEL )
    {
#if DEB_ARI
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
      {
        const u16 channel = WH_GetMeasureChannel();
        work->subState = MPSS_SEND_IMAGE_WAIT_BOOT_INIT;
        MB_TPrintf("Quick boot!!\n");
      }
      else
#endif
      {
        MP_PARENT_SendImage_MBPInit( work );

        MB_MSG_MessageCreateWordset( work->msgWork );
        MB_MSG_MessageWordsetName( work->msgWork , 0 , GAMEDATA_GetMyStatus(work->initWork->gameData) );
        MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_01 , MSGSPEED_GetWait() );
        MB_MSG_MessageDeleteWordset( work->msgWork );

        work->subState = MPSS_SEND_IMAGE_MBSYS_MAIN;
      }
    }
    break;
  case MPSS_SEND_IMAGE_MBSYS_MAIN:
    MP_PARENT_SendImage_MBPMain( work );
    break;

  case MPSS_SEND_IMAGE_WAIT_BOOT_INIT:
    MB_COMM_InitParent( work->commWork );
    MB_MSG_MessageDisp( work->msgWork , MSG_MB_PAERNT_02 , MSGSPEED_GetWait() );
    work->subState = MPSS_SEND_IMAGE_WAIT_BOOT;
    break;
    
  case MPSS_SEND_IMAGE_WAIT_BOOT:
    if( MB_COMM_IsSendEnable( work->commWork ) == TRUE )
    {
      work->subState = MPSS_SEND_IMAGE_FINISH_BOOT;
      return TRUE;
    }
    break;
  }
  
  return FALSE;
}

//--------------------------------------------------------------
//  MBPシステム起動
//--------------------------------------------------------------
static void MP_PARENT_SendImage_MBPInit( MB_PARENT_WORK *work )
{
  /* このデモがダウンロードさせるプログラム情報 */
  //staticじゃないと動かない！！！
  static MBGameRegistry mbGameList = {
    "/dl_rom/child.srl",    // 子機バイナリコード
    NULL ,                  // ゲーム名
    NULL ,                  // ゲーム内容説明
    "/dl_rom/icon.char",    // アイコンキャラクタデータ
    "/dl_rom/icon.plt",     // アイコンパレットデータ
    MB_DEF_GGID,            // GGID
    2,                      // 最大プレイ人数、親機の数も含めた人数
  };

  const u16 channel = WH_GetMeasureChannel();
  mbGameList.gameNamep = work->romTitleStr;
  mbGameList.gameIntroductionp = work->romInfoStr;
  MBP_Init( MB_DEF_GGID , MB_TGID_AUTO );
  MBP_Start( &mbGameList , channel );
}

static void MP_PARENT_SendImage_MBPMain( MB_PARENT_WORK *work )
{
  const u16 mbpState = MBP_GetState();

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
      if ( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
      {
        // Bボタンでマルチブートキャンセル
        MBP_Cancel();
        break;
      }
      // エントリー中の子機が一台でも存在すれば開始可能とする
      if (MBP_GetChildBmp(MBP_BMPTYPE_ENTRY) ||
          MBP_GetChildBmp(MBP_BMPTYPE_DOWNLOADING) ||
          MBP_GetChildBmp(MBP_BMPTYPE_BOOTABLE))
      {
        //子機が来たらとりあえず始めてしまう
        {
          // ダウンロード開始
          MBP_StartDownloadAll();
        }
      }
    }
    break;

    //-----------------------------------------
    // プログラム配信処理
  case MBP_STATE_DATASENDING:
    {
      // 全員がダウンロード完了しているならばスタート可能.
      if (MBP_IsBootableAll())
      {
        // ブート開始
        MBP_StartRebootAll();
      }
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
  
}

#pragma mark [>save func
//--------------------------------------------------------------
//  セーブ準備
//--------------------------------------------------------------
static void MB_PARENT_SaveInit( MB_PARENT_WORK *work )
{
  u8 i;
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork(work->initWork->gameData);
  BOX_MANAGER *boxMng = GAMEDATA_GetBoxManager(work->initWork->gameData);
  const u8 pokeNum = MB_COMM_GetPostPokeNum( work->commWork );
  MB_MSG_MessageDispNoWait( work->msgWork , MSG_MB_PAERNT_07 );
  for( i=0;i<pokeNum;i++ )
  {
    const POKEMON_PASO_PARAM *ppp = MB_COMM_GetPostPokeData( work->commWork , i );
    const BOOL ret = BOXDAT_PutPokemon( boxMng , ppp );
    GF_ASSERT_MSG( ret == TRUE , "Multiboot parent Box is full!!\n");
#if DEB_ARI
    {
      char name[32];
      STRBUF *nameStr = GFL_STR_CreateBuffer( 32 , work->heapId );
      PPP_Get( ppp , ID_PARA_nickname , nameStr );
      DEB_STR_CONV_StrBufferToSjis( nameStr , name , 32 );
      MB_TPrintf("[%d][%s]\n",i,name);
      GFL_STR_DeleteBuffer( nameStr );
    }
#endif
  }
  
  MB_TPrintf( "MB_Parent Save Init\n" );
}

//--------------------------------------------------------------
//  セーブ開放
//--------------------------------------------------------------
static void MB_PARENT_SaveTerm( MB_PARENT_WORK *work )
{
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork(work->initWork->gameData);
}

//--------------------------------------------------------------
//  セーブ更新
//--------------------------------------------------------------
static void MB_PARENT_SaveMain( MB_PARENT_WORK *work )
{
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork(work->initWork->gameData);
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
    SaveControl_SaveAsyncInit( svWork );
    work->subState = MPSS_SAVE_WAIT_FIRST;
    MB_TPrintf( "MB_Parent Save Start!\n" );
    break;

  case MPSS_SAVE_WAIT_FIRST:
    {
      const SAVE_RESULT ret = SaveControl_SaveAsyncMain( svWork );
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
  		const SAVE_RESULT ret = SaveControl_SaveAsyncMain( svWork );
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
  
  MB_TPrintf("LeastAppHeap[%x]\n",GFI_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MULTIBOOT, 0x150000 );
  work = GFL_PROC_AllocWork( proc, sizeof(MB_PARENT_WORK), HEAPID_MULTIBOOT );
  if( pwk == NULL )
  {
    initWork = GFL_HEAP_AllocMemory( HEAPID_MULTIBOOT , sizeof( MB_PARENT_INIT_WORK ));
    initWork->gameData = GAMEDATA_Create( HEAPID_MULTIBOOT );
  }
  else
  {
    initWork = pwk;
  }
  work->heapId = HEAPID_MULTIBOOT;
  work->initWork = initWork;
  
  MB_PARENT_Init( work );
  
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
static GFL_PROC_RESULT MB_PARENT_ProcTerm( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  MB_PARENT_WORK *work = mywk;
  
  MB_PARENT_Term( work );

  if( pwk == NULL )
  {
    GAMEDATA_Delete( work->initWork->gameData );
    GFL_HEAP_FreeMemory( work->initWork );
  }
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_MULTIBOOT );

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
  return GFL_PROC_RES_CONTINUE;
}

