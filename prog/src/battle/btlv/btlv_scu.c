//=============================================================================================
/**
 * @file  btlv_scu.c
 * @brief ポケモンWB バトル 描画 上画面制御モジュール
 * @author  taya
 *
 * @date  2008.11.18  作成
 */
//=============================================================================================
#include <gflib.h>

#include "sound\pm_sndsys.h"
#include "poke_tool/monsno_def.h"
#include "print/printsys.h"
#include "gamesystem/msgspeed.h"
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "winframe.naix"
#include "battle/battgra_wb.naix"

#include "battle/btl_common.h"
#include "battle/btl_util.h"
#include "battle/btl_string.h"
#include "btlv_common.h"
#include "btlv_core.h"
#include "btlv_effect.h"  //soga
#include "tr_tool/tr_tool.h"

#include "btlv_scu.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  MAIN_STRBUF_LEN = 1024,
  SUB_STRBUF_LEN  = 384,
  SUBPROC_WORK_SIZE = 64,

  PALIDX_MSGWIN        = 0,
  PALIDX_TOKWIN1       = 1,
  PALIDX_SYSWIN        = 2,
  PALIDX_LVUPWIN       = 3,

  COLIDX_MSGWIN_CLEAR  = 0x0c,
  COLIDX_MSGWIN_LETTER = 0x01,
  COLIDX_MSGWIN_SHADOW = 0x09,

  COLIDX_LVUPWIN_CLEAR  = 0x0c,
  COLIDX_LVUPWIN_LETTER = 0x01,
  COLIDX_LVUPWIN_SHADOW = 0x09,

  COLIDX_TOKWIN_CLEAR  = 0x00,  // とくせいウィンドウのフォント色
  COLIDX_TOKWIN_LETTER = 0x01,  // 自分側、敵側どちらのフォントパレットの
  COLIDX_TOKWIN_SHADOW = 0x02,  // 使用場所も同じになっている

  MSGWIN_WIDTH = 30,
  MSGWIN_HEIGHT = 4,
  MSGWIN_ORG_X = 1,
  MSGWIN_ORG_Y = 19,

  LVUPWIN_WIDTH = 10,
  LVUPWIN_HEIGHT = 12,
  LVUPWIN_ORG_X = (32 - LVUPWIN_WIDTH - 1),
  LVUPWIN_ORG_Y = ((MSGWIN_ORG_Y-1)-LVUPWIN_HEIGHT-1),

  MSGWIN_EVA_MAX = 31,
  MSGWIN_EVA_MIN = 0,
  MSGWIN_EVB_MIN = 7,
  MSGWIN_EVB_MAX = 16,
  MSGWIN_VISIBLE_STEP = 6,

  BGFRAME_MAIN_MESSAGE  = GFL_BG_FRAME1_M,
  BGFRAME_TOKWIN_FRIEND = GFL_BG_FRAME2_M,
  BGFRAME_TOKWIN_ENEMY  = GFL_BG_FRAME3_M,
  BGFRAME_LVUPWIN       = GFL_BG_FRAME3_M,  // 使い回している

  TOKWIN_CGRDATA_CHAR_W = 18,   // とくせいウィンドウCGRデータ横キャラ数
  TOKWIN_DRAWAREA_CHAR_W = 17,  // とくせいウィンドウ書き込み可能領域の横キャラ数
  TOKWIN_DRAWAREA_CHAR_H = 4,   // とくせいウィンドウ書き込み可能領域の縦キャラ数
  TOKWIN_CGRDATA_TRANS_CHARS = TOKWIN_CGRDATA_CHAR_W * TOKWIN_DRAWAREA_CHAR_H,
  TOKWIN_CGRDATA_TRANS_SIZE = (TOKWIN_CGRDATA_TRANS_CHARS * 0x20),

  TOKWIN_CGRDATA_TOTAL_SIZE = TOKWIN_CGRDATA_TRANS_SIZE*2,
  TOKWIN_CGRDATA_CHAR_CNT = (TOKWIN_CGRDATA_TOTAL_SIZE / 0x20),

  TOKWIN_CGRDATA_LINE_SIZE = (TOKWIN_CGRDATA_CHAR_W * 0x20),

  TOKWIN_DRAWAREA_WIDTH = TOKWIN_DRAWAREA_CHAR_W * 8,
  TOKWIN_LINE_CROSS_WIDTH = 12, // とくせいウィンドウに書く２行の文字が交差する最低ドット数
  TOKWIN_DRAWAREA_LINE1_OY = 7,
  TOKWIN_DRAWAREA_LINE2_OY = 18,

  TOKWIN_MARGIN_R = 4,
  TOKWIN_MARGIN_L = 8,
  TOKWIN_HIDEPOS_FRIEND = (TOKWIN_CGRDATA_CHAR_W * 8),
  TOKWIN_HIDEPOS_ENEMY = -(TOKWIN_CGRDATA_CHAR_W * 8),
  TOKWIN_SCRN_YPOS = 0,

  TOKWIN_MOVE_FRAMES = 8,

  BTLIN_STD_FADE_WAIT = 2,

  LVUPWIN_STEP_SE = SEQ_SE_MESSAGE,


  BBGP_CLIENT_ID_NONE = BTL_CLIENT_NUM,
};

/**
 *  とくせいウィンドウ用SE指定
 */
enum {
  TOKWIN_SE_IN        = WAVE_SE_WB_SHOOTER,
  TOKWIN_SE_FLASH     = BANK_SE_WB_DECIDE2,
  TOKWIN_SE_CHANGE    = BANK_SE_WB_DECIDE3,
};

typedef enum {

  TASKTYPE_DEFAULT = 0,
  TASKTYPE_MEMBER_IN,
  TASKTYPE_MEMBER_OUT,
  TASKTYPE_WAZA_DAMAGE,
  TASKTYPE_HP_GAUGE,
  TASKTYPE_TOKWIN_SWAP,
  TASKTYPE_MAX,

}TaskType;

typedef enum {

  MSGWIN_STATE_DISP = 0,
  MSGWIN_STATE_TO_HIDE,
  MSGWIN_STATE_HIDE,
  MSGWIN_STATE_TO_DISP,
  MSGWIN_STATE_TO_DISP_READY,

}MsgWinState;

typedef enum {

  TOKWIN_SIDE_FRIEND,
  TOKWIN_SIDE_ENEMY,

}TokwinSide;

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*BtlinEffectSeq)( BTLV_SCU*, int* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/
typedef struct {
  const BTL_POKEPARAM*  bpp;
  BTLV_SCU*         parentWk;
  u8                pokePos;
  u8                vpos;
  u8                dispFlag;
  u8                enableFlag;
}STATUS_WIN;

typedef struct {
  BTLV_SCU*       parentWk;
  GFL_BMP_DATA*   bmp;
  u16             tokusei;
  u8              mySide;
  u8              bgFrame;
  u8              pokeID;
  u8              seq;
  const u8*       cgrSrc;

  fx32            posX;
  fx32            speedX;
  u32             charPos;
  u8              moveTimer;
  u8              writeRaw;
  u8              dispFlag  :  1;
  u8              fFlash    :  1;
  u8              fHideStart : 1;

}TOK_WIN;

typedef struct {
  GFL_BMPWIN*  win;
  fx32  eva;
  fx32  evb;
  fx32  eva_step;
  fx32  evb_step;
  u8    state;
  u8    timer;
}MSGWIN_VISIBLE;

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
struct _BTLV_SCU {

  GFL_BMPWIN*     win;
  GFL_BMP_DATA*   bmp;

  GFL_BMPWIN*     lvupWin;
  GFL_BMP_DATA*   lvupBmp;
  u32             lvupWin_frameCharPos;
  u32             tokwin_CharPos;

  PRINT_QUE*      printQue;
  PRINT_UTIL      printUtil;
  GFL_FONT*       defaultFont;
  GFL_FONT*       smallFont;
  GFL_TCBLSYS*    tcbl;
  PRINT_STREAM*      printStream;
  STRBUF*            strBufMain;
  STRBUF*            strBufSubA;
  STRBUF*            strBufSubB;

  u8              taskCounter[TASKTYPE_MAX];


  STATUS_WIN    statusWin[ BTL_POS_MAX ];
  TOK_WIN       tokWin[ BTL_SIDE_MAX ];
  void*         tokWinCgrHead;
  NNSG2dCharacterData*  tokWinCgr;


  BTL_PROC          proc;
  u8                procWork[ SUBPROC_WORK_SIZE ];
  const BTLV_CORE*  vcore;
  const BTL_MAIN_MODULE*  mainModule;
  const BTL_POKE_CONTAINER* pokeCon;
  HEAPID        heapID;
  u8            printAtOnceFlag;
  u8            printSeq;
  u8            playerClientID;
  u8            printJustDoneFlag;
  PRINTSTREAM_STATE  printState;
  u16           printWait;
  u16           printWaitOrg;
  u16           printWaitEOM;
  u8            msgwinVisibleFlag;
  u8            btlinSeq;
  u8            btlinSkipFlag;
  u16           msgwinVisibleSeq;
  MSGWIN_VISIBLE   msgwinVisibleWork;

  const BTL_POKEPARAM*    lvupBpp;
  BTL_LEVELUP_INFO lvupInfo;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u32 transWinFrameCgx( BTLV_SCU* wk, u32 arcID, u32 datID, u8 bgFrame );
static inline void* Scu_GetProcWork( BTLV_SCU* wk, u32 size );
static BOOL Fade_CheckEnd( BTLV_SCU* wk );
static void btlin_startFade( int wait );
static inline void btlinTool_vpos_exchange( BTLV_SCU* wk, BtlvMcssPos vpos, BtlvMcssPos* vposDst, BtlPokePos* posDst, const BTL_POKEPARAM** bppDst );
static BOOL btlin_skip_core( BTLV_SCU* wk, int* seq, const u8* vposAry, u8 vposCount );
static BOOL btlin_skip_single( BTLV_SCU* wk, int* seq );
static BOOL btlin_skip_double( BTLV_SCU* wk, int* seq );
static BOOL btlin_skip_triple( BTLV_SCU* wk, int* seq );
static BOOL btlin_wild_single( int* seq, void* wk_adrs );
static u16 GetWildSingleEncountStrID( BTLV_SCU* wk );
static BOOL btlin_trainer_single( int* seq, void* wk_adrs );
static BOOL btlin_comm_single( int* seq, void* wk_adrs );
static BOOL btlin_wild_double( int* seq, void* wk_adrs );
static BOOL btlin_trainer_double( int* seq, void* wk_adrs );
static BOOL btlin_trainer_multi( int* seq, void* wk_adrs );
static BOOL btlin_comm_double( int* seq, void* wk_adrs );
static BOOL btlin_comm_double_multi( int* seq, void* wk_adrs );
static BOOL btlin_trainer_triple( int* seq, void* wk_adrs );
static BOOL btlin_comm_triple( int* seq, void* wk_adrs );
static BOOL  btlinEff_OpponentTrainerIn( BTLV_SCU* wk, int* seq );
static BOOL  btlinEff_OpponentTrainerInMulti( BTLV_SCU* wk, int* seq );
static BOOL btlinEffSub_OpponentTrainerIn1( BTLV_SCU* wk, int* seq, u8 clientID );
static BOOL btlinEffSub_OpponentTrainerIn2( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 );
static BOOL btlinEff_OpponentPokeInSingle( BTLV_SCU* wk, int* seq );
static BOOL btlinEff_OpponentPokeInDouble( BTLV_SCU* wk, int* seq );
static BOOL btlinEffSub_OpponentPokeIn_Solo( BTLV_SCU* wk, int* seq, u8 clientID );
static BOOL btlinEffSub_OpponentPokeIn_Tag( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 );
static BOOL btlinEff_OpponentPokeInTriple( BTLV_SCU* wk, int* seq );
static BOOL btlinEff_MyPokeInSingle( BTLV_SCU* wk, int* seq );
static BOOL btlinEff_MyPokeInDouble( BTLV_SCU* wk, int* seq );
static BOOL btlinEffSub_MyPokeIn_Solo( BTLV_SCU* wk, int* seq, u8 clientID );
static BOOL btlinEffSub_MyPokeIn_Tag( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 );
static BOOL btlinEff_MyPokeInTriple( BTLV_SCU* wk, int* seq );
static u16 btlfinEffsub_getOpponentPokeInStrID( BTLV_SCU* wk, u8 pokeCount );
static BOOL subproc_WazaEffect( int* seq, void* wk_adrs );
static void taskPokeOutAct( GFL_TCBL* tcbl, void* wk_adrs );
static void taskPokeInEffect( GFL_TCBL* tcbl, void* wk_adrs );
static void taskFakeDisable( GFL_TCBL* tcbl, void* wk_adrs );
static void taskTransform( GFL_TCBL* tcbl, void* wk_adrs );
static void msgWinVisible_Init( MSGWIN_VISIBLE* wk, GFL_BMPWIN* win );
static void msgWinVisible_Hide( MSGWIN_VISIBLE* wk );
static void msgWinVisible_Disp( MSGWIN_VISIBLE* wk, BOOL printAtOnceFlag );
static BOOL msgWinVisible_Update( MSGWIN_VISIBLE* wk );
static void taskMsgWinHide( GFL_TCBL* tcbl, void* wk_adrs );
static void statwin_setupAll( BTLV_SCU* wk );
static void statwin_cleanupAll( BTLV_SCU* wk );
static void statwin_setup( STATUS_WIN* stwin, BTLV_SCU* wk, BtlPokePos pokePos );
static void statwin_cleanup( STATUS_WIN* stwin );
static void statwin_hide( STATUS_WIN* stwin );
static void statwin_disp_start( STATUS_WIN* stwin );
static TokwinSide PokePosToTokwinSide( const BTL_MAIN_MODULE* mainModule, BtlPokePos pos );
static void taskTokWinSwap( GFL_TCBL* tcbl, void* wk_adrs );
static void tokwin_setupAll( BTLV_SCU* wk, u32 charpos_top );
static void tokwin_cleanupAll( BTLV_SCU* wk );
static void tokwin_disp_first( TOK_WIN* tokwin, BtlPokePos pos, BOOL fFlash );
static BOOL tokwin_disp_progress( TOK_WIN* tokwin );
static void tokwin_update_cgr( TOK_WIN* tokwin );
static void tokwin_hide_first( TOK_WIN* tokwin );
static BOOL tokwin_hide_progress( TOK_WIN* tokwin );
static void tokwin_renew_start( TOK_WIN* tokwin, BtlPokePos pos );
static BOOL tokwin_renew_progress( TOK_WIN* tokwin );
static void bbgp_make( BTLV_SCU* wk, BTLV_BALL_GAUGE_PARAM* bbgp, u8 clientID_1, u8 clientID_2, BTLV_BALL_GAUGE_TYPE type );
static BOOL lvupWinProc_Disp( int* seq, void* wk_adrs );
static BOOL lvupWinProc_Fwd( int* seq, void* wk_adrs );
static BOOL lvupWinProc_Hide( int* seq, void* wk_adrs );



BTLV_SCU*  BTLV_SCU_Create( const BTLV_CORE* vcore,
  const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon,
  GFL_TCBLSYS* tcbl, GFL_FONT* defaultFont, GFL_FONT* smallFont, u8 playerClientID, HEAPID heapID )
{
  BTLV_SCU* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTLV_SCU) );

  wk->vcore = vcore;
  wk->mainModule = mainModule;
  wk->pokeCon = pokeCon;
  wk->heapID = heapID;
  wk->playerClientID = playerClientID;
  wk->defaultFont = defaultFont;
  wk->smallFont = smallFont;
  wk->btlinSkipFlag = FALSE;

  wk->printStream = NULL;
  wk->tcbl = tcbl;
  wk->strBufMain = GFL_STR_CreateBuffer( MAIN_STRBUF_LEN, heapID );
  wk->strBufSubA = GFL_STR_CreateBuffer( SUB_STRBUF_LEN, heapID );
  wk->strBufSubB = GFL_STR_CreateBuffer( SUB_STRBUF_LEN, heapID );
  wk->msgwinVisibleFlag = FALSE;

  wk->tokWinCgrHead = NULL;

  wk->printQue = PRINTSYS_QUE_Create( wk->heapID );
  PRINT_UTIL_Setup( &wk->printUtil, wk->win );

  return wk;
}

void BTLV_SCU_Setup( BTLV_SCU* wk )
{
  // 個別フレーム設定：メインメッセージウィンドウ用
  static const GFL_BG_BGCNT_HEADER bgcntText = {
    0, 0, 0x800, 0,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x10000, 0x8000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };
  // 個別フレーム設定：味方側とくせいウィンドウ用
  static const GFL_BG_BGCNT_HEADER bgcntTok1 = {
    TOKWIN_HIDEPOS_FRIEND, 0, 0x1000, 0,
    GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x08000, 0x8000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };
  // 個別フレーム設定：敵側とくせいウィンドウ用
  static const GFL_BG_BGCNT_HEADER bgcntTok2 = {
    TOKWIN_HIDEPOS_ENEMY, 0, 0x2000, 0,
    GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000, 0x8000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };
  u32 winfrm_charpos, tokwin_charpos;

  GFL_BG_SetBGControl( BGFRAME_MAIN_MESSAGE,  &bgcntText,   GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( BGFRAME_TOKWIN_FRIEND, &bgcntTok1,   GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( BGFRAME_TOKWIN_ENEMY,  &bgcntTok2,   GFL_BG_MODE_TEXT );

  PaletteWorkSetEx_Arc( BTLV_EFFECT_GetPfd(), ARCID_BATTGRA, NARC_battgra_wb_msgwin_frm_NCLR, wk->heapID,
      FADE_MAIN_BG, 0x20, PALIDX_MSGWIN*16, 0 );

  // lvupwin color も含んでいる
  PaletteWorkSetEx_Arc( BTLV_EFFECT_GetPfd(), ARCID_BATTGRA, NARC_battgra_wb_tokusei_w_NCLR, wk->heapID,
        FADE_MAIN_BG, 0x60, PALIDX_TOKWIN1*16, PALIDX_TOKWIN1*16 );

  //BD面カラーを黒にする
  {
    u16 dat = 0;
    PaletteWorkSet( BTLV_EFFECT_GetPfd(), &dat, FADE_MAIN_BG, 0, 2 );
  }

  GFL_BG_FillScreen( BGFRAME_MAIN_MESSAGE, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
  GFL_BG_FillCharacter( BGFRAME_MAIN_MESSAGE, 0x00, 1, 0 );
  winfrm_charpos = transWinFrameCgx( wk, ARCID_BATTGRA, NARC_battgra_wb_msgwin_frm_NCGR, BGFRAME_MAIN_MESSAGE );

//  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_BATTGRA, NARC_battgra_wb_tokusei_w_NCGR, BGFRAME_TOKWIN_FRIEND,
//                              0, 0, FALSE, wk->heapID );



  wk->tokwin_CharPos = transWinFrameCgx( wk, ARCID_BATTGRA, NARC_battgra_wb_tokusei_w_NCGR, BGFRAME_TOKWIN_ENEMY );

  GFL_ARC_UTIL_TransVramScreen( ARCID_BATTGRA, NARC_battgra_wb_tokusei_w01_NSCR, BGFRAME_TOKWIN_FRIEND,
          0, 0, FALSE, wk->heapID );
  GFL_ARC_UTIL_TransVramScreen( ARCID_BATTGRA, NARC_battgra_wb_tokusei_w02_NSCR, BGFRAME_TOKWIN_ENEMY,
          0, 0, FALSE, wk->heapID );

  wk->win = GFL_BMPWIN_Create( BGFRAME_MAIN_MESSAGE, MSGWIN_ORG_X, MSGWIN_ORG_Y, MSGWIN_WIDTH, MSGWIN_HEIGHT,
          PALIDX_MSGWIN, GFL_BMP_CHRAREA_GET_F );
  wk->bmp = GFL_BMPWIN_GetBmp( wk->win );
  GFL_BMPWIN_MakeScreen( wk->win );
  GFL_BMPWIN_MakeFrameScreen( wk->win, winfrm_charpos, PALIDX_MSGWIN );

  GFL_BMP_Clear( wk->bmp, COLIDX_MSGWIN_CLEAR );
  GFL_BMPWIN_TransVramCharacter( wk->win );


  wk->lvupWin_frameCharPos = transWinFrameCgx( wk, ARCID_BATTGRA, NARC_battgra_wb_lvup_w_NCGR, BGFRAME_TOKWIN_ENEMY );

  wk->lvupWin = GFL_BMPWIN_Create( BGFRAME_TOKWIN_ENEMY, LVUPWIN_ORG_X, LVUPWIN_ORG_Y+32, LVUPWIN_WIDTH, LVUPWIN_HEIGHT,
        PALIDX_LVUPWIN, GFL_BMP_CHRAREA_GET_F );
  wk->lvupBmp = GFL_BMPWIN_GetBmp( wk->lvupWin );

  /*
  GFL_BMPWIN_MakeScreen( wk->lvupWin );
  GFL_BMPWIN_MakeFrameScreen( wk->lvupWin, wk->lvupWin_frameCharPos, PALIDX_LVUPWIN );
  GFL_BMP_Clear( wk->lvupBmp, COLIDX_LVUPWIN_CLEAR );
  GFL_BMPWIN_TransVramCharacter( wk->lvupWin );
  */


  msgWinVisible_Init( &wk->msgwinVisibleWork, wk->win );

  statwin_setupAll( wk );
  tokwin_setupAll( wk, wk->tokwin_CharPos );

  GFL_BG_LoadScreenReq( BGFRAME_MAIN_MESSAGE  );
  GFL_BG_LoadScreenReq( BGFRAME_TOKWIN_FRIEND );
  GFL_BG_LoadScreenReq( BGFRAME_TOKWIN_ENEMY  );

//  GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
  GFL_BG_SetVisible( BGFRAME_MAIN_MESSAGE,   VISIBLE_ON );
  GFL_BG_SetVisible( BGFRAME_TOKWIN_FRIEND,  VISIBLE_ON );
  GFL_BG_SetVisible( BGFRAME_TOKWIN_ENEMY,   VISIBLE_ON );
  ///<obj
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}
/**
 *  WindowFrame CGX 転送して転送位置キャラナンバーを返す
 */
static u32 transWinFrameCgx( BTLV_SCU* wk, u32 arcID, u32 datID, u8 bgFrame )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( arcID, GFL_HEAP_LOWID(wk->heapID) );
  GFL_ARCUTIL_TRANSINFO transInfo;
  u32 charPos;

  transInfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( handle, datID, bgFrame,
    0, FALSE, GFL_HEAP_LOWID(wk->heapID) );
  charPos = GFL_ARCUTIL_TRANSINFO_GetPos( transInfo );
  GFL_ARC_CloseDataHandle( handle );

  return charPos;
}

void BTLV_SCU_Delete( BTLV_SCU* wk )
{
  tokwin_cleanupAll( wk );
  statwin_cleanupAll( wk );

  if( wk->lvupWin ){
    GFL_BMPWIN_Delete( wk->lvupWin );
    wk->lvupWin = NULL;
  }
  if( wk->win ){
    GFL_BMPWIN_Delete( wk->win );
    wk->win = NULL;
  }
  GFL_BG_FreeBGControl( BGFRAME_MAIN_MESSAGE );
  GFL_BG_FreeBGControl( BGFRAME_TOKWIN_FRIEND );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );

  if( wk->printQue ){
    PRINTSYS_QUE_Delete( wk->printQue );
    wk->printQue = NULL;
  }

  if( wk->strBufSubB ){
    GFL_STR_DeleteBuffer( wk->strBufSubB );
    wk->strBufSubB = NULL;
  }
  if( wk->strBufSubA ){
    GFL_STR_DeleteBuffer( wk->strBufSubA );
    wk->strBufSubA = NULL;
  }
  GFL_STR_DeleteBuffer( wk->strBufMain );

  GFL_HEAP_FreeMemory( wk );
}


//=============================================================================================
/**
 * ワザエフェクト後、FRAME3用のCGX、スクリーンデータをデフォルトに戻す
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_SCU_RestoreDefaultScreen( const BTLV_SCU* wk )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, GFL_HEAP_LOWID(wk->heapID) );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_battgra_wb_tokusei_w_NCGR, BGFRAME_TOKWIN_ENEMY,
              wk->tokwin_CharPos, 0, FALSE, wk->heapID );

  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_battgra_wb_lvup_w_NCGR, BGFRAME_TOKWIN_ENEMY,
              wk->lvupWin_frameCharPos, 0, FALSE, wk->heapID );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_ClearScreen( BGFRAME_TOKWIN_ENEMY );

  GFL_ARC_UTIL_TransVramScreen( ARCID_BATTGRA, NARC_battgra_wb_tokusei_w02_NSCR, BGFRAME_TOKWIN_ENEMY,
          0, 0, FALSE, wk->heapID );
}

//--------------------------------------------------------------------------
/**
 *
 *
 * @param   wk
 * @param   size
 *
 * @retval  inline void*
 */
//--------------------------------------------------------------------------
static inline void* Scu_GetProcWork( BTLV_SCU* wk, u32 size )
{
  GF_ASSERT(size < sizeof(wk->procWork));
  return wk->procWork;
}



/**
 *  フェード終了チェック
 */
static BOOL Fade_CheckEnd( BTLV_SCU* wk )
{
  return GFL_FADE_CheckFade() == FALSE;
}


//=============================================================================================
/**
 * バトル画面初期セットアップ完了までの演出を開始する
 *
 * @param   wk
 * @param   fChapterSkipMode    チャプタスキップモード（ポケモン配置のみ行う）
 */
//=============================================================================================
void BTLV_SCU_StartBtlIn( BTLV_SCU* wk, BOOL fChapterSkipMode )
{
  BtlCompetitor  competitor = BTL_MAIN_GetCompetitor( wk->mainModule );

  wk->btlinSeq = 0;
  wk->btlinSkipFlag = fChapterSkipMode;

  switch( BTL_MAIN_GetRule(wk->mainModule) ){

  /*----------------------------------------------*/
  /*  シングル                                    */
  /*----------------------------------------------*/
  case BTL_RULE_SINGLE:
    switch( competitor ){
    case BTL_COMPETITOR_WILD:         // 野生
    case BTL_COMPETITOR_DEMO_CAPTURE: // 捕獲デモ
    default:
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_wild_single );
      break;
    case BTL_COMPETITOR_TRAINER:  // ゲーム内トレーナー
    case BTL_COMPETITOR_SUBWAY:   // サブウェイトレーナー
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_trainer_single );
      break;
    case BTL_COMPETITOR_COMM:
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_comm_single );
      break;
    }
    break;

  /*----------------------------------------------*/
  /*  ダブル                                      */
  /*----------------------------------------------*/
  case BTL_RULE_DOUBLE:
    {
      switch( competitor ){
      case BTL_COMPETITOR_WILD:     // 野生
        BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_wild_double );
        break;

      // 通信対戦トレーナー
      case BTL_COMPETITOR_COMM:
        if( !BTL_MAIN_IsMultiMode(wk->mainModule) ){
          BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_comm_double );
        }else{
          BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_comm_double_multi );
        }
        break;

      case BTL_COMPETITOR_TRAINER:  // ゲーム内トレーナー
      case BTL_COMPETITOR_SUBWAY:   // サブウェイトレーナー
        if( !BTL_MAIN_IsMultiMode(wk->mainModule) ){
          BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_trainer_double );
        }else{
          BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_trainer_multi );
        }
        break;
      }
    }
    break;

  /*----------------------------------------------*/
  /*  トリプル                                    */
  /*----------------------------------------------*/
  case BTL_RULE_TRIPLE:
    // ゲーム内トレーナー
    if( competitor != BTL_COMPETITOR_COMM ){
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_trainer_triple );
    // 通信対戦
    }else{
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_comm_triple );
    }
    break;

  /*----------------------------------------------*/
  /*  ローテーション                              */
  /*----------------------------------------------*/
  case BTL_RULE_ROTATION:
    // ゲーム内トレーナー
    if( competitor != BTL_COMPETITOR_COMM ){
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_trainer_triple );
    // 通信対戦
    }else{
      BTL_UTIL_SetupProc( &wk->proc, wk, NULL, btlin_comm_triple );
    }
    break;
  }
}

//=============================================================================================
/**
 * バトル画面初期セットアップ完了までの演出を終了するまで待つ
 *
 * @param   wk
 *
 * @retval  BOOL    終了したらTRUE
 */
//=============================================================================================
BOOL BTLV_SCU_WaitBtlIn( BTLV_SCU* wk )
{
  return BTL_UTIL_CallProc( &wk->proc );
}


/**
 *  現状のマスター輝度値を参照してフェードパラメータ呼び分け
 */
static void btlin_startFade( int wait )
{
  if( GX_GetMasterBrightness() <= 0 ){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, wait );
  }else{
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, wait );
  }
}

//--------------------------------------------------
/**
 *  描画位置-> 位置, ポケモンパラメータ変換
 */
//--------------------------------------------------
static inline void btlinTool_vpos_exchange( BTLV_SCU* wk, BtlvMcssPos vpos, BtlvMcssPos* vposDst, BtlPokePos* posDst, const BTL_POKEPARAM** bppDst )
{
  *vposDst = vpos;
  *posDst = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, vpos );
  *bppDst = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, *posDst );
}

//------------------------------------------------------------------------
/**
 *  戦闘画面セットアップ（デバッグ用簡易版 コア処理）
 */
//------------------------------------------------------------------------
static BOOL btlin_skip_core( BTLV_SCU* wk, int* seq, const u8* vposAry, u8 vposCount )
{
  switch( *seq ){
  case 0:
    {
      BtlvMcssPos  vpos;
      BtlPokePos   pos;
      const BTL_POKEPARAM* bpp;
      u32 i;

      for(i=0; i<vposCount; ++i){
        btlinTool_vpos_exchange( wk, vposAry[i], &vpos, &pos, &bpp );
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(bpp), vpos );
        statwin_disp_start( &wk->statusWin[ pos ] );
      }
      (*seq)++;
    }
    break;
  case 1:
    // チャプタスキップ時はここでフェードインしない
    if( wk->btlinSkipFlag ){
      return TRUE;
    }

    // フェードインするのはデバッグ時のみ
    btlin_startFade(-3);
    (*seq)++;
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//------------------------------------------------------------------------
/**
 *  戦闘画面セットアップ（デバッグ用簡易版／シングル）
 */
//------------------------------------------------------------------------
static BOOL btlin_skip_single( BTLV_SCU* wk, int* seq )
{
  static const u8 vposAry[] = {
    BTLV_MCSS_POS_BB, BTLV_MCSS_POS_AA
  };
  return btlin_skip_core( wk, seq, vposAry, NELEMS(vposAry) );
}

//------------------------------------------------------------------------
/**
 *  戦闘画面セットアップ（デバッグ用簡易版／ダブル）
 */
//------------------------------------------------------------------------
static BOOL btlin_skip_double( BTLV_SCU* wk, int* seq )
{
  static const u8 vposAry[] = {
    BTLV_MCSS_POS_A, BTLV_MCSS_POS_B, BTLV_MCSS_POS_C, BTLV_MCSS_POS_D,
  };
  return btlin_skip_core( wk, seq, vposAry, NELEMS(vposAry) );
}
//------------------------------------------------------------------------
/**
 *  戦闘画面セットアップ（デバッグ用簡易版／トリプル）
 */
//------------------------------------------------------------------------
static BOOL btlin_skip_triple( BTLV_SCU* wk, int* seq )
{
  static const u8 vposAry[] = {
    BTLV_MCSS_POS_A, BTLV_MCSS_POS_B, BTLV_MCSS_POS_C, BTLV_MCSS_POS_D,
    BTLV_MCSS_POS_E, BTLV_MCSS_POS_F,
  };
  return btlin_skip_core( wk, seq, vposAry, NELEMS(vposAry) );
}

//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（野生／シングル）
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL btlin_wild_single( int* seq, void* wk_adrs )
{
  typedef struct {
    const BTL_POKEPARAM* pp;
    BtlPokePos  pokePos;
    u8          viewPos;
    u8  pokeID;
  }ProcWork;

  BTLV_SCU* wk = wk_adrs;
  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_single( wk, seq );
  }
  #endif

  if( wk->btlinSkipFlag ){
    return btlin_skip_single( wk, seq );
  }

  switch( *seq ){
  case 0:
    subwk->viewPos = BTLV_MCSS_POS_BB;
    subwk->pokePos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->viewPos );
    subwk->pp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pokePos );
    subwk->pokeID = BPP_GetID( subwk->pp );
    msgWinVisible_Hide( &wk->msgwinVisibleWork );
    (*seq)++;
    break;
  case 1:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->pp), subwk->viewPos );
      BTLV_EFFECT_AddByPos( subwk->viewPos, BTLEFF_SINGLE_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      (*seq)++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u16 strID = GetWildSingleEncountStrID( wk );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 1, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pokePos ] );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 4:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BtlCompetitor  competitor = BTL_MAIN_GetCompetitor( wk->mainModule );
      if( competitor == BTL_COMPETITOR_DEMO_CAPTURE ){
        BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_ARARAGI );
      }else{
        BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      }
      (*seq)++;
    }
    break;
  case 5:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      subwk->viewPos = BTLV_MCSS_POS_AA;
      subwk->pokePos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->viewPos );
      subwk->pp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pokePos );
      subwk->pokeID = BPP_GetID( subwk->pp );

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->pp), subwk->viewPos );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutSingle, 1, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 6:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 7:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) ){
      (*seq)++;
    }
    break;
  case 8:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pokePos ] );
      return TRUE;
    }
    break;
  }
  return FALSE;
}
/**
 * 野生シングルエンカウント時の文字列IDを取得（セットアップパラメータ参照）
 */
static u16 GetWildSingleEncountStrID( BTLV_SCU* wk )
{
  if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_LEGEND) ){
    return BTL_STRID_STD_Encount_Wild_Legend;
  }
  if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_MOVE_POKE) ){
    return BTL_STRID_STD_Encount_Wild_Move;
  }
  if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_WILD_TALK) ){
    return BTL_STRID_STD_Encount_Wild_Talk;
  }
  if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_SYMBOL) ){
    return BTL_STRID_STD_Encount_Wild_Symbol;
  }
  return BTL_STRID_STD_Encount_Wild1;
}

//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（ゲーム内トレーナー／シングル）
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL btlin_trainer_single( int* seq, void* wk_adrs )
{
  typedef struct {
    const BTL_POKEPARAM* pp;
    BtlPokePos  pokePos;
    u8          viewPos;
    u8  pokeID;
    u8  clientID;
  }ProcWork;

  BTLV_SCU* wk = wk_adrs;
  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_single( wk, seq );
  }
  #endif

  if( wk->btlinSkipFlag ){
    return btlin_skip_single( wk, seq );
  }

  switch( *seq ){
  case 0:
    {
      enum {
        VPOS = BTLV_MCSS_POS_BB,
      };

      u8 btlPos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, VPOS );
      u8 clientID = BTL_MAIN_BtlPosToClientID( wk->mainModule, btlPos );

      u16 trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );
      BTLV_EFFECT_SetTrainer( trType, BTLV_MCSS_POS_TR_BB, 0, 0, 0 );

      subwk->viewPos = VPOS;
      subwk->clientID = clientID;
      subwk->pokePos = btlPos;
      subwk->pp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pokePos );
      subwk->pokeID = BPP_GetID( subwk->pp );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 1:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BTLV_EFFECT_AddByPos( subwk->viewPos, BTLEFF_SINGLE_TRAINER_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      (*seq)++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      bbgp_make( wk, &bbgp, subwk->clientID, BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_ENEMY );
      BTLV_EFFECT_SetBallGauge( &bbgp );
      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_Encount_NPC1, 1, subwk->clientID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_AddByPos( subwk->viewPos, BTLEFF_SINGLE_TRAINER_ENCOUNT_2 );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutSingle_NPC, 2, 1, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 5:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->pp), subwk->viewPos );
      BTLV_EFFECT_AddByPos( subwk->viewPos, BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_ENEMY );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 6:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      bbgp_make( wk, &bbgp, BTL_MAIN_GetPlayerClientID(wk->mainModule), BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_MINE );
      BTLV_EFFECT_SetBallGauge( &bbgp );
      (*seq)++;
    }
    break;
  case 7:
    {
      statwin_disp_start( &wk->statusWin[ subwk->pokePos ] );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      (*seq)++;
    }
    break;
  case 8:
    if( ( !BTLV_EFFECT_CheckExecute() ) && ( !BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE_MINE ) ) )
    {
      subwk->viewPos = BTLV_MCSS_POS_AA;
      subwk->pokePos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->viewPos );
      subwk->pp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pokePos );
      subwk->pokeID = BPP_GetID( subwk->pp );

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->pp), subwk->viewPos );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutSingle, 1, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 9:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 10:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) ){
      (*seq)++;
    }
    break;
  case 11:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pokePos ] );
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（通信対戦トレーナー／シングル）
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL btlin_comm_single( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[][2] = {
    { btlinEff_OpponentTrainerIn,     btlinEff_OpponentTrainerIn    },
    { btlinEff_OpponentPokeInSingle,  btlinEff_MyPokeInSingle       },
    { btlinEff_MyPokeInSingle,        btlinEff_OpponentPokeInSingle },
  };

  BTLV_SCU* wk = wk_adrs;

  if( wk->btlinSkipFlag ){
    return btlin_skip_single( wk, seq );
  }

  if( wk->btlinSeq < NELEMS(funcs) )
  {
    u8 myClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
    if( funcs[wk->btlinSeq][myClientID](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（野生／ダブル）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_wild_double( int* seq, void* wk_adrs )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[2];
    u8  pokeID[2];
    u8  pos[2];
    u8  vpos[2];
    u8  frontMemberCount;
    u8  clientID;
  }ProcWork;

  BTLV_SCU* wk = wk_adrs;
  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_double( wk, seq );
  }
  #endif


  if( wk->btlinSkipFlag ){
    return btlin_skip_double( wk, seq );
  }

  switch( *seq ){
  case 0:
    {
      BtlPokePos myPos = BTL_MAIN_GetClientPokePos( wk->mainModule, wk->playerClientID, 0 );
      subwk->pos[0] = BTL_MAIN_GetOpponentPokePos( wk->mainModule, myPos, 0 );
      subwk->bpp[0] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[0] );
      subwk->pokeID[0] = BPP_GetID( subwk->bpp[0] );
      subwk->pos[1] = BTL_MAIN_GetOpponentPokePos( wk->mainModule, myPos, 1 );
      subwk->bpp[1] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[1] );
      subwk->pokeID[1] = BPP_GetID( subwk->bpp[1] );

      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 1:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      u8 viewPos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, subwk->pos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), viewPos );
      (*seq)++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u8 viewPos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, subwk->pos[1] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), viewPos );

      BTLV_EFFECT_AddByPos( viewPos, BTLEFF_SINGLE_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      (*seq)++;
    }
    break;
  case 3:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_Encount_Wild2, 2, subwk->pokeID[0], subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 4:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 5:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      (*seq)++;
    }
    break;
  case 6:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      u32 i;

      subwk->vpos[0] = BTLV_MCSS_POS_A;
      subwk->vpos[1] = BTLV_MCSS_POS_C;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      for(i=0; i<2; ++i)
      {
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        if( subwk->bpp[i] != NULL ){
          subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
        }
      }

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutDouble, 2, subwk->pokeID[0], subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 7:
    if( BTLV_SCU_WaitMsg(wk) )
    {
//      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
#if 0
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );
#endif:

      (*seq)++;
    }
    break;
  case 8:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      (*seq)++;
    }
    break;
  case 9:
    if( !BTLV_EFFECT_CheckExecute() ){
      return TRUE;
    }
  }
  return FALSE;
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（ゲーム内トレーナー／ダブル）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_trainer_double( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[] = {
    btlinEff_OpponentTrainerIn,
    btlinEff_OpponentPokeInDouble,
    btlinEff_MyPokeInDouble,
  };

  BTLV_SCU* wk = wk_adrs;

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_double( wk, seq );
  }
  #endif

  if( wk->btlinSkipFlag ){
    return btlin_skip_double( wk, seq );
  }


  if( wk->btlinSeq < NELEMS(funcs) )
  {
    if( funcs[wk->btlinSeq](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（ゲーム内トレーナー／ダブルマルチ）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_trainer_multi( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[] = {
    btlinEff_OpponentTrainerInMulti,
    btlinEff_OpponentPokeInDouble,
    btlinEff_MyPokeInDouble,
  };

  BTLV_SCU* wk = wk_adrs;

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_double( wk, seq );
  }
  #endif

  if( wk->btlinSkipFlag ){
    return btlin_skip_double( wk, seq );
  }


  if( wk->btlinSeq < NELEMS(funcs) )
  {
    if( funcs[wk->btlinSeq](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}


//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（通信対戦／ダブル）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_comm_double( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[][2] = {
    { btlinEff_OpponentTrainerIn,     btlinEff_OpponentTrainerIn    },
    { btlinEff_OpponentPokeInDouble,  btlinEff_MyPokeInDouble       },
    { btlinEff_MyPokeInDouble,        btlinEff_OpponentPokeInDouble },
  };

  BTLV_SCU* wk = wk_adrs;

  if( wk->btlinSkipFlag ){
    return btlin_skip_double( wk, seq );
  }

  if( wk->btlinSeq < NELEMS(funcs) )
  {
    u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
    if( funcs[wk->btlinSeq][clientID](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（通信対戦／ダブルマルチ）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_comm_double_multi( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[] = {
    btlinEff_OpponentTrainerInMulti,
    btlinEff_OpponentPokeInDouble,
    btlinEff_MyPokeInDouble,
  };

  BTLV_SCU* wk = wk_adrs;

  if( wk->btlinSkipFlag ){
    return btlin_skip_double( wk, seq );
  }

  if( wk->btlinSeq < NELEMS(funcs) )
  {
    if( funcs[wk->btlinSeq](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（ゲーム内トレーナー／トリプル）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_trainer_triple( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[] = {
    btlinEff_OpponentTrainerIn,
    btlinEff_OpponentPokeInTriple,
    btlinEff_MyPokeInTriple,
  };

  BTLV_SCU* wk = wk_adrs;

  #ifdef PM_DEBUG
  if( BTL_MAIN_GetDebugFlag(wk->mainModule, BTL_DEBUGFLAG_SKIP_BTLIN) ){
    return btlin_skip_triple( wk, seq );
  }
  #endif

  if( wk->btlinSkipFlag ){
    return btlin_skip_triple( wk, seq );
  }


  if( wk->btlinSeq < NELEMS(funcs) )
  {
    if( funcs[wk->btlinSeq](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}
//--------------------------------------------------------------------------
/**
 * 戦闘画面セットアップ完了までの演出（通信対戦／トリプル）
 * @retval  BOOL    TRUEで終了
 */
//--------------------------------------------------------------------------
static BOOL btlin_comm_triple( int* seq, void* wk_adrs )
{
  static const BtlinEffectSeq funcs[][2] = {
    { btlinEff_OpponentTrainerIn,     btlinEff_OpponentTrainerIn    },
    { btlinEff_OpponentPokeInTriple,  btlinEff_MyPokeInTriple       },
    { btlinEff_MyPokeInTriple,        btlinEff_OpponentPokeInTriple },
  };

  BTLV_SCU* wk = wk_adrs;

  if( wk->btlinSkipFlag ){
    return btlin_skip_triple( wk, seq );
  }


  if( wk->btlinSeq < NELEMS(funcs) )
  {
    u8 clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
//    BTL_Printf("clientID=%d\n", clientID);

    if( funcs[wk->btlinSeq][clientID](wk, seq) ){
      wk->btlinSeq++;
      (*seq) = 0;
    }
    return FALSE;
  }
  else{
    return TRUE;
  }
}



//---------------------------------------------------------------------------
/*
 *  開始エフェクト：相手トレーナー登場（１人）
 */
//---------------------------------------------------------------------------
static BOOL  btlinEff_OpponentTrainerIn( BTLV_SCU* wk, int* seq )
{
  u8 clientID = BTL_MAIN_GetEnemyClientID ( wk->mainModule, 0 );
  return btlinEffSub_OpponentTrainerIn1( wk, seq, clientID );
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：相手トレーナー登場（マルチ）
 */
//---------------------------------------------------------------------------
static BOOL  btlinEff_OpponentTrainerInMulti( BTLV_SCU* wk, int* seq )
{
  u8 clientID_1 = BTL_MAIN_GetEnemyClientID ( wk->mainModule, 0 );
  u8 clientID_2 = BTL_MAIN_GetEnemyClientID ( wk->mainModule, 1 );

  if( clientID_1 != clientID_2 )
  {
    return btlinEffSub_OpponentTrainerIn2( wk, seq, clientID_1, clientID_2 );
  }
  else
  {
    return btlinEffSub_OpponentTrainerIn1( wk, seq, clientID_1 );
  }
}
/**
 *  相手側トレーナー登場（１人）
 */
static BOOL btlinEffSub_OpponentTrainerIn1( BTLV_SCU* wk, int* seq, u8 clientID )
{
  typedef struct {
    u8    pos;
    u8    vpos;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:   // トレーナー絵セット
    {
      u16 trType;

      subwk->vpos = BTLV_MCSS_POS_TR_BB;

      trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID );

      BTLV_EFFECT_SetTrainer( trType, subwk->vpos, 0, 0, 0 );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 1:   // カメラ、敵トレーナー等倍表示までズームアウト
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BTLV_EFFECT_AddByPos( subwk->vpos, BTLEFF_SINGLE_TRAINER_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      (*seq)++;
    }
    break;

  case 2:   // 敵ボールゲージ出現＆「○○が勝負をしかけてきた！」表示
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      u16 strID = (BTL_MAIN_GetCompetitor(wk->mainModule) != BTL_COMPETITOR_COMM)?
                      BTL_STRID_STD_Encount_NPC1 : BTL_STRID_STD_Encount_Player1;


      bbgp_make( wk, &bbgp, clientID, BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_ENEMY );
      BTLV_EFFECT_SetBallGauge( &bbgp );

      BTL_STR_MakeStringStd( wk->strBufMain, strID, 1, clientID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3: // カメラ、デフォルト位置までズームアウト
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_AddByPos( subwk->vpos, BTLEFF_SINGLE_TRAINER_ENCOUNT_2 );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  相手側トレーナー登場（２人）
 */
static BOOL btlinEffSub_OpponentTrainerIn2( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 )
{
  typedef struct {
    u8    vpos_1;
    u8    vpos_2;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );


  switch( *seq ){
  case 0:   // トレーナー絵セット
    {
      u16 trType;

      subwk->vpos_1 = BTLV_MCSS_POS_TR_B;
      subwk->vpos_2 = BTLV_MCSS_POS_TR_D;


      trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID_1 );
      BTLV_EFFECT_SetTrainer( trType, subwk->vpos_1, 0, 0, 0 );
      trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID_2 );
      BTLV_EFFECT_SetTrainer( trType, subwk->vpos_2, 0, 0, 0 );

      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 1:   // カメラ、敵トレーナー等倍表示までズームアウト
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      BTLV_EFFECT_AddByPos( subwk->vpos_1, BTLEFF_SINGLE_TRAINER_ENCOUNT_1 );
      btlin_startFade( BTLIN_STD_FADE_WAIT );
      (*seq)++;
    }
    break;

  case 2:   // 敵ボールゲージ出現＆「○○が勝負をしかけてきた！」表示
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      u16 strID = (BTL_MAIN_GetCompetitor(wk->mainModule) != BTL_COMPETITOR_COMM)?
                      BTL_STRID_STD_Encount_NPC2 : BTL_STRID_STD_Encount_Player2;

      OS_TPrintf("Competitor=%d\n", BTL_MAIN_GetCompetitor(wk->mainModule));

      //BTS:5245
      bbgp_make( wk, &bbgp, clientID_1, clientID_2, BTLV_BALL_GAUGE_TYPE_ENEMY );
      BTLV_EFFECT_SetBallGauge( &bbgp );

//      u16 trType = BTL_MAIN_GetClientTrainerType( wk->mainModule, clientID_2 );
//      BTLV_EFFECT_SetTrainer( trType, subwk->vpos_2, 0, 0, 0 );
//      BTLV_EFFECT_AddByPos( subwk->vpos_2, BTLEFF_SINGLE_TRAINER_ENCOUNT_2 );

      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, clientID_1, clientID_2 );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3: // カメラ、デフォルト位置までズームアウト
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_AddByPos( subwk->vpos_1, BTLEFF_SINGLE_TRAINER_ENCOUNT_2 );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：相手ポケモン登場（シングル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_OpponentPokeInSingle( BTLV_SCU* wk, int* seq )
{
  typedef struct {
    const BTL_POKEPARAM* bpp;
    u8    pos;
    u8    vpos;
    u8    pokeID;
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:
    {
      u32 i;
      u16 strID;

      subwk->vpos = BTLV_MCSS_POS_BB;
      {
        u8 myClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
        subwk->clientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, myClientID, 0 );
      }
      subwk->pos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos );
      subwk->bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos );
      subwk->pokeID = BPP_GetID( subwk->bpp );

      strID = btlfinEffsub_getOpponentPokeInStrID( wk, 1 );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, subwk->clientID, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_SCU_WaitMsg(wk) ){
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 2:
    BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp), subwk->vpos );
    BTLV_EFFECT_AddByPos( subwk->vpos, BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );
    BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_ENEMY );
    (*seq)++;
    break;

  case 3:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos ] );
      (*seq)++;
    }
    break;

  case 4:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：相手ポケモン登場（ダブル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_OpponentPokeInDouble( BTLV_SCU* wk, int* seq )
{
  u8 clientID_1 = BTL_MAIN_GetEnemyClientID ( wk->mainModule, 0 );
  u8 clientID_2 = BTL_MAIN_GetEnemyClientID ( wk->mainModule, 1 );
  if( clientID_1 != clientID_2 )
  {
    return btlinEffSub_OpponentPokeIn_Tag( wk, seq, clientID_1, clientID_2 );
  }
  else
  {
    return btlinEffSub_OpponentPokeIn_Solo( wk, seq, clientID_1 );
  }
}
/**
 *  相手ポケモン登場サブ：クライアント１人
 */
static BOOL btlinEffSub_OpponentPokeIn_Solo( BTLV_SCU* wk, int* seq, u8 clientID )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[2];
    u8    pos[2];
    u8    vpos[2];
    u8    pokeID[2];
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:
    {
      u32 i;
      u16 strID;

      subwk->vpos[0] = BTLV_MCSS_POS_B;
      subwk->vpos[1] = BTLV_MCSS_POS_D;
      subwk->clientID = clientID;
      subwk->aliveCnt = 0;

      for(i=0; i<2; ++i){
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
        BTL_Printf("N[%d] vpos=%d -> pos=%d, pokeID=%d\n", i, subwk->vpos[i], subwk->pos[i], subwk->pokeID[i]);
        if( !BPP_IsDead(subwk->bpp[i]) ){
          subwk->aliveCnt++;
        }
      }

      strID = btlfinEffsub_getOpponentPokeInStrID( wk, subwk->aliveCnt );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 3, subwk->clientID, subwk->pokeID[0], subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_SCU_WaitMsg(wk) ){
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 2:
    BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
    if( subwk->aliveCnt == 2 ){
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
    }
    BTLV_EFFECT_AddByPos( subwk->vpos[0], BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );
    BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_ENEMY );
    (*seq)++;
    break;

  case 3:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      if( subwk->aliveCnt == 2 ){
        statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      }
      (*seq)++;
    }
    break;

  case 4:
    return TRUE;
  }
  return FALSE;
}
/**
 *  相手ポケモン登場サブ：クライアント２人
 */
static BOOL btlinEffSub_OpponentPokeIn_Tag( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[2];
    u8    pos[2];
    u8    vpos[2];
    u8    pokeID[2];
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:
    {
      u32 i;
      u16 strID;

      subwk->vpos[0] = BTLV_MCSS_POS_B;
      subwk->pos[0] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[0] );
      subwk->bpp[0] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[0] );
      subwk->pokeID[0] = BPP_GetID( subwk->bpp[0] );

      subwk->vpos[1] = BTLV_MCSS_POS_D;
      subwk->pos[1] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[1] );
      subwk->bpp[1] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[1] );
      subwk->pokeID[1] = BPP_GetID( subwk->bpp[1] );

      strID = btlfinEffsub_getOpponentPokeInStrID( wk, 1 );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, clientID_1, subwk->pokeID[0] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

#if 0  //BTS5243
  case 1:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_AddByPos( subwk->vpos[0], BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 2:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      u16 strID = btlfinEffsub_getOpponentPokeInStrID( wk, 1 );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, clientID_2, subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg( wk )){
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      (*seq)++;
    }
    break;
#else
  case 1:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 2:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork))
    {
      u16 strID = btlfinEffsub_getOpponentPokeInStrID( wk, 1 );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, clientID_2, subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg( wk )){

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_AddByPos( subwk->vpos[0], BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );

      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_ENEMY );

      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      (*seq)++;
    }
    break;
#endif

  case 5:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：相手ポケモン登場（トリプル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_OpponentPokeInTriple( BTLV_SCU* wk, int* seq )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[3];
    u8    pos[3];
    u8    vpos[3];
    u8    pokeID[3];
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:
    {
      u32 i;
      u16 strID;

      subwk->vpos[0] = BTLV_MCSS_POS_B;
      subwk->vpos[1] = BTLV_MCSS_POS_D;
      subwk->vpos[2] = BTLV_MCSS_POS_F;
      subwk->aliveCnt = 0;
      {
        u8 myClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
        subwk->clientID = BTL_MAIN_GetOpponentClientID( wk->mainModule, myClientID, 0 );
      }
      for(i=0; i<3; ++i)
      {
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
        if( !BPP_IsDead(subwk->bpp[i]) ){
          subwk->aliveCnt++;
        }
      }

      strID = btlfinEffsub_getOpponentPokeInStrID( wk, subwk->aliveCnt );
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 4, subwk->clientID, subwk->pokeID[0], subwk->pokeID[1], subwk->pokeID[2] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_SCU_WaitMsg(wk) ){
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 2:
    BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
    if( subwk->aliveCnt > 1){
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
    }
    if( subwk->aliveCnt > 2){
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[2]), subwk->vpos[2] );
    }
    BTLV_EFFECT_AddByPos( subwk->vpos[0], BTLEFF_SINGLE_TRAINER_ENCOUNT_3 );
    BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_ENEMY );
    (*seq)++;
    break;

  case 3:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      if( subwk->aliveCnt > 1 ){
        statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      }
      if( subwk->aliveCnt > 2 ){
        statwin_disp_start( &wk->statusWin[ subwk->pos[2] ] );
      }
      (*seq)++;
    }
    break;

  case 4:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：自分ポケモン登場（シングル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_MyPokeInSingle( BTLV_SCU* wk, int* seq )
{
  typedef struct {
    const BTL_POKEPARAM* bpp;
    u8    pos;
    u8    vpos;
    u8    pokeID;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:// 自分ボールゲージ出現
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      bbgp_make( wk, &bbgp, subwk->clientID, BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_MINE );
      BTLV_EFFECT_SetBallGauge( &bbgp );
      (*seq)++;
    }
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      (*seq)++;
    }
    break;

  case 2:
    if( ( !BTLV_EFFECT_CheckExecute() ) && ( !BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE_MINE ) ) )
    {
      subwk->vpos = BTLV_MCSS_POS_AA;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      subwk->pos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos );
      subwk->bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos );
      subwk->pokeID = BPP_GetID( subwk->bpp );

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp), subwk->vpos );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutSingle, 1, subwk->pokeID );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;

  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos ] );
      (*seq)++;
    }
    break;

  case 5:
    return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：自分ポケモン登場（ダブル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_MyPokeInDouble( BTLV_SCU* wk, int* seq )
{
  u8 clientID_1 = BTL_MAIN_GetPlayerClientID( wk->mainModule );
  u8 clientID_2 = BTL_MAIN_GetPlayerFriendCleintID( wk->mainModule );
  if( clientID_2 == BTL_CLIENTID_NULL )
  {
    return btlinEffSub_MyPokeIn_Solo( wk, seq, clientID_1 );
  }
  else
  {
    /*
    if( clientID_1 > clientID_2 ){
      u8  tmp = clientID_1;
      clientID_2 = clientID_1;
      clientID_1 = tmp;
    }
    */
    return btlinEffSub_MyPokeIn_Tag( wk, seq, clientID_1, clientID_2 );
  }
}
/**
 *  自分側ポケモン登場（ダブル） ソロ
 */
static BOOL btlinEffSub_MyPokeIn_Solo( BTLV_SCU* wk, int* seq, u8 clientID )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[2];
    u8    pos[2];
    u8    vpos[2];
    u8    pokeID[2];
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:// 自分ボールゲージ出現
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      subwk->clientID = clientID;
      bbgp_make( wk, &bbgp, subwk->clientID, BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_MINE );

      BTLV_EFFECT_SetBallGauge( &bbgp );
      (*seq)++;
    }
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      (*seq)++;
    }
    break;

  case 2:
    if( ( !BTLV_EFFECT_CheckExecute() ) && ( !BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE_MINE ) ) )
    {
      u32 i;
      u16 strID;

      subwk->vpos[0] = BTLV_MCSS_POS_A;
      subwk->vpos[1] = BTLV_MCSS_POS_C;
      subwk->aliveCnt = 0;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      for(i=0; i<2; ++i){
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        if( subwk->bpp[i] != NULL ){
          subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
          if( !BPP_IsDead(subwk->bpp[i]) ){
            subwk->aliveCnt++;
          }
        }
      }

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      if( subwk->aliveCnt > 1){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      }
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      strID = ( subwk->aliveCnt == 2 )? BTL_STRID_STD_PutDouble : BTL_STRID_STD_PutSingle;
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, subwk->pokeID[0], subwk->pokeID[1] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
#if 0
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      if( subwk->aliveCnt > 1){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      }
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );
#endif
      (*seq)++;
    }
    break;

  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      if( subwk->aliveCnt > 1){
        statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      }
      (*seq)++;
    }
    break;

  case 5:
    return TRUE;
  }
  return FALSE;
}
/**
 *  自分側ポケモン登場（ダブル） タッグ
 */
static BOOL btlinEffSub_MyPokeIn_Tag( BTLV_SCU* wk, int* seq, u8 clientID_1, u8 clientID_2 )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[2];
    u8    pos[2];
    u8    vpos[2];
    u8    pokeID[2];
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:// 自分ボールゲージ出現
    {
      BTLV_BALL_GAUGE_PARAM bbgp;

      if( clientID_1 < clientID_2 ){  // A C　の順にする。
        bbgp_make( wk, &bbgp, clientID_1, clientID_2, BTLV_BALL_GAUGE_TYPE_MINE );
      }else{
        bbgp_make( wk, &bbgp, clientID_2, clientID_1, BTLV_BALL_GAUGE_TYPE_MINE );
      }

      BTLV_EFFECT_SetBallGauge( &bbgp );
      (*seq)++;
    }
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_TAG );
      (*seq)++;
    }
    break;

  case 2:
    if( ( !BTLV_EFFECT_CheckExecute() ) && ( !BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE_MINE ) ) )
    {
      u32 i;
      u16 strID;
      u8  pokeIdx;

      subwk->vpos[0] = BTLV_MCSS_POS_A;
      subwk->vpos[1] = BTLV_MCSS_POS_C;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      pokeIdx = BTL_MAIN_GetPlayerMultiPos( wk->mainModule ) ^ 1;

      for(i=0; i<2; ++i)
      {
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
      }

#if 0  //BTS5243
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );
#endif
      strID = (BTL_MAIN_IsClientNPC(wk->mainModule, clientID_2))? BTL_STRID_STD_PutSingle_NPC : BTL_STRID_STD_PutSingle_Player;

      BTL_STR_MakeStringStd( wk->strBufMain, strID, 2, clientID_2, subwk->pokeID[pokeIdx] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 4:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) )
    {
      u8 pokeIdx = BTL_MAIN_GetPlayerMultiPos( wk->mainModule );
#if 1 //BTS5243
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );
#endif
      BTL_STR_MakeStringStd( wk->strBufMain, BTL_STRID_STD_PutSingle, 1, subwk->pokeID[pokeIdx] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;
  case 5:
    if( BTLV_SCU_WaitMsg( wk ) ){
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
      (*seq)++;
    }
    break;
  case 6:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      return TRUE;
    }
    break;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
/*
 *  開始エフェクト：自分ポケモン登場（トリプル）
 */
//---------------------------------------------------------------------------
static BOOL btlinEff_MyPokeInTriple( BTLV_SCU* wk, int* seq )
{
  typedef struct {
    const BTL_POKEPARAM* bpp[3];
    u8    pos[3];
    u8    vpos[3];
    u8    pokeID[3];
    u8    aliveCnt;
    u8    clientID;
  }ProcWork;

  ProcWork* subwk = Scu_GetProcWork( wk, sizeof(ProcWork) );

  switch( *seq ){
  case 0:// 自分ボールゲージ出現
    {
      BTLV_BALL_GAUGE_PARAM bbgp;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      BTL_Printf("自ポケ登場トリプル：ClientID=%d\n", subwk->clientID);
      bbgp_make( wk, &bbgp, subwk->clientID, BBGP_CLIENT_ID_NONE, BTLV_BALL_GAUGE_TYPE_MINE );
      BTLV_EFFECT_SetBallGauge( &bbgp );
      (*seq)++;
    }
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_2_SOLO );
      (*seq)++;
    }
    break;

  case 2:
    if( ( !BTLV_EFFECT_CheckExecute() ) && ( !BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE_MINE ) ) )
    {
      u32 i;
      u16 strID;

      subwk->vpos[0] = BTLV_MCSS_POS_A;
      subwk->vpos[1] = BTLV_MCSS_POS_C;
      subwk->vpos[2] = BTLV_MCSS_POS_E;
      subwk->aliveCnt = 0;
      subwk->clientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
      for(i=0; i<3; ++i)
      {
        subwk->pos[i] = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, subwk->vpos[i] );
        subwk->bpp[i] = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, subwk->pos[i] );
        subwk->pokeID[i] = BPP_GetID( subwk->bpp[i] );
        if( !BPP_IsDead(subwk->bpp[i]) ){
          subwk->aliveCnt++;
        }
      }

      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      if( subwk->aliveCnt > 1){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      }
      if( subwk->aliveCnt > 2){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[2]), subwk->vpos[2] );
      }
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );

      if( subwk->aliveCnt == 3 ){
        strID = BTL_STRID_STD_PutTriple;
      }else{
        strID = ( subwk->aliveCnt == 2 )? BTL_STRID_STD_PutDouble : BTL_STRID_STD_PutSingle;
      }
      BTL_STR_MakeStringStd( wk->strBufMain, strID, 3, subwk->pokeID[0], subwk->pokeID[1], subwk->pokeID[2] );
      BTLV_SCU_StartMsg( wk, wk->strBufMain, BTLV_MSGWAIT_STD, NULL );
      (*seq)++;
    }
    break;

  case 3:
    if( BTLV_SCU_WaitMsg(wk) )
    {
      BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE_MINE );
      msgWinVisible_Hide( &wk->msgwinVisibleWork );
#if 0
      BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[0]), subwk->vpos[0] );
      if( subwk->aliveCnt > 1){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[1]), subwk->vpos[1] );
      }
      if( subwk->aliveCnt > 2){
        BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(subwk->bpp[2]), subwk->vpos[2] );
      }
      BTLV_EFFECT_Add( BTLEFF_SINGLE_ENCOUNT_3 );
#endif
      (*seq)++;
    }
    break;

  case 4:
    if( ( msgWinVisible_Update(&wk->msgwinVisibleWork) ) &&
        ( !BTLV_EFFECT_CheckExecute() ) )
    {
      statwin_disp_start( &wk->statusWin[ subwk->pos[0] ] );
      if( subwk->aliveCnt > 1){
        statwin_disp_start( &wk->statusWin[ subwk->pos[1] ] );
      }
      if( subwk->aliveCnt > 1){
        statwin_disp_start( &wk->statusWin[ subwk->pos[2] ] );
      }
      (*seq)++;
    }
    break;

  case 5:
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * 相手ポケモン登場メッセージの文字列IDを取得
 *
 * @param   wk
 * @param   pokeCount
 *
 * @retval  u16
 */
//----------------------------------------------------------------------------------
static u16 btlfinEffsub_getOpponentPokeInStrID( BTLV_SCU* wk, u8 pokeCount )
{
  BtlCompetitor competitor = BTL_MAIN_GetCompetitor( wk->mainModule );
  switch( competitor ){
  case BTL_COMPETITOR_WILD:
  default:
    return ( pokeCount == 1 )? BTL_STRID_STD_Encount_Wild1 : BTL_STRID_STD_Encount_Wild2;

  case BTL_COMPETITOR_TRAINER:
  case BTL_COMPETITOR_SUBWAY:
    if( pokeCount == 1 ){
      return BTL_STRID_STD_PutSingle_NPC;
    }else if( pokeCount == 2 ){
      return BTL_STRID_STD_PutDouble_NPC;
    }
    return BTL_STRID_STD_PutTriple_NPC;

  case BTL_COMPETITOR_COMM:
    if( pokeCount == 1 ){
      return BTL_STRID_STD_PutSingle_Player;
    }else if( pokeCount == 2 ){
      return BTL_STRID_STD_PutDouble_Player;
    }
    return BTL_STRID_STD_PutTriple_Player;
  }
}


//=============================================================================================
/**
 * メッセージ表示（一括）
 *
 * @param   wk
 * @param   str     表示文字列
 *
 */
//=============================================================================================
void BTLV_SCU_PrintMsgAtOnce( BTLV_SCU* wk, const STRBUF* str )
{
  GFL_BMP_Clear( wk->bmp, COLIDX_MSGWIN_CLEAR );
  GFL_FONTSYS_SetColor( COLIDX_MSGWIN_LETTER, COLIDX_MSGWIN_SHADOW, COLIDX_MSGWIN_CLEAR );

#if 1
  PRINTSYS_Print( wk->bmp, 0, 0, str, wk->defaultFont );
  GFL_BMPWIN_TransVramCharacter( wk->win );
#else
  PRINTSYS_PrintQue( wk->printQue, wk->bmp, 0, 0, str, wk->defaultFont );
  while( 1 ){
    if( PRINTSYS_QUE_Main( wk->printQue ) ){ break; }
  }
  GFL_BMPWIN_TransVramCharacter( wk->win );
#endif

  wk->printSeq = 0;
  wk->printWait = 0;
  wk->printWaitOrg = 0;
  wk->printWaitEOM = 0;
  wk->printAtOnceFlag = TRUE;
  wk->printJustDoneFlag = FALSE;
}

//=============================================================================================
/**
 * メッセージ表示開始
 *
 * @param   wk
 * @param   str     表示文字列
 * @param   wait    表示終了後の待ち方( 0:即終了 / 1〜:通常時はキー待ち，通信時は指定フレーム待ち）
 *
 */
//=============================================================================================
void BTLV_SCU_StartMsg( BTLV_SCU* wk, const STRBUF* str, u16 wait, pPrintCallBack callbackFunc )
{
  GF_ASSERT( wk->printStream == NULL );

  GFL_BMP_Clear( wk->bmp, COLIDX_MSGWIN_CLEAR );
  GFL_FONTSYS_SetColor( COLIDX_MSGWIN_LETTER, COLIDX_MSGWIN_SHADOW, COLIDX_MSGWIN_CLEAR );

  wk->printStream = PRINTSYS_PrintStreamCallBack(
        wk->win, 0, 0, str, wk->defaultFont, BTL_MAIN_GetPrintWait(wk->mainModule), wk->tcbl, BTLV_TASKPRI_MAIN_WINDOW,
        wk->heapID, COLIDX_MSGWIN_CLEAR, callbackFunc
  );
  PRINTSYS_PrintStreamStop( wk->printStream );

  wk->printSeq = 0;
  wk->printAtOnceFlag = FALSE;
  if( wait == BTLV_MSGWAIT_QUICK )
  {
    wk->printWait = BTLV_MSGWAIT_STD;
    wk->printWaitEOM = 0;
  }
  else{
    wk->printWait = wait;
    wk->printWaitEOM = wait;
  }

  wk->printWaitOrg = wk->printWait;
  wk->printJustDoneFlag = FALSE;
}
//=============================================================================================
/**
 * メッセージ終端までの表示を終えたタイミングかどうか判定
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_IsJustDoneMsg( BTLV_SCU* wk )
{
  if( wk->printJustDoneFlag ){
    wk->printJustDoneFlag = FALSE;
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * メッセージ表示終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_WaitMsg( BTLV_SCU* wk )
{
  enum {
    SEQ_START_MSGWIN_VISIBLE,
    SEQ_WAIT_MSGWIN_VISIBLE,
    SEQ_WAIT_STREAM_RUNNING,
    SEQ_WAIT_USERCTRL_NOT_COMM,
    SEQ_WAIT_USERCTRL_COMM,
    SEQ_AFTER_USERCTRL,
    SEQ_DONE,
    SEQ_END,
  };

  switch( wk->printSeq ){
  case SEQ_START_MSGWIN_VISIBLE:
    msgWinVisible_Disp( &wk->msgwinVisibleWork, wk->printAtOnceFlag );
    wk->printSeq++;
    /* fallthru */
  case SEQ_WAIT_MSGWIN_VISIBLE:
    if( !msgWinVisible_Update(&wk->msgwinVisibleWork) ){
      break;
    }
    else
    {
      if( wk->printAtOnceFlag )
      {
        wk->printSeq = SEQ_DONE;
        break;
      }
      else
      {
        if( wk->printStream ){
          PRINTSYS_PrintStreamRun( wk->printStream );
        }
        wk->printSeq++;
      }
    }
    /* fallthru */
  case SEQ_WAIT_STREAM_RUNNING:
    if( wk->printStream )
    {
      // 非通信時はキー押しorタッチで「はやい」設定と同速に。
      if( BTL_MAIN_GetCommMode(wk->mainModule) == BTL_COMM_NONE )
      {
        if( (GFL_UI_KEY_GetCont() & (PAD_BUTTON_A|PAD_BUTTON_B))
        ||  (GFL_UI_TP_GetCont())
        ){
          if( !PRINTSYS_IsTempSpeedMode(wk->printStream) ){
            PRINTSYS_PrintStream_StartTempSpeedMode( wk->printStream, MSGSPEED_GetWaitByConfigParam(MSGSPEED_FAST) );
          }
        }
        else{
          if( PRINTSYS_IsTempSpeedMode(wk->printStream) ){
            PRINTSYS_PrintStream_StopTempSpeedMode( wk->printStream );
          }
        }
      }

      wk->printState = PRINTSYS_PrintStreamGetState( wk->printStream );
      if( wk->printState != PRINTSTREAM_STATE_RUNNING )
      {
        if( BTL_MAIN_GetCommMode(wk->mainModule) == BTL_COMM_NONE ){
          wk->printSeq = SEQ_WAIT_USERCTRL_NOT_COMM;
        } else {
          wk->printSeq = SEQ_WAIT_USERCTRL_COMM;
        }
        if( wk->printState == PRINTSTREAM_STATE_DONE )
        {
          wk->printWait = wk->printWaitEOM;
          wk->printJustDoneFlag = TRUE;
        }
      }
    }
    else
    {
      wk->printSeq = SEQ_DONE;
    }
    break;

  case SEQ_WAIT_USERCTRL_NOT_COMM: // 待ち指定あり（非通信時）
    if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
    ||  (GFL_UI_TP_GetTrg())
    ){
      wk->printWait = 0;
    }
    if( wk->printWait ){
      wk->printWait--;
    }else{
      wk->printSeq = SEQ_AFTER_USERCTRL;
    }
    break;

  case SEQ_WAIT_USERCTRL_COMM: // 待ち指定あり（通信時）
    if( wk->printWait )
    {
      wk->printWait--;
    }
    else
    {
      wk->printSeq = SEQ_AFTER_USERCTRL;
    }
    break;

  case SEQ_AFTER_USERCTRL:
    if( wk->printState == PRINTSTREAM_STATE_DONE )
    {
      GFL_FONTSYS_SetDefaultColor();
      PRINTSYS_PrintStreamDelete( wk->printStream );
      wk->printStream = NULL;
      wk->printSeq = SEQ_DONE;
    }
    else
    {
      PRINTSYS_PrintStreamReleasePause( wk->printStream );
      PMSND_PlaySE( SEQ_SE_MESSAGE );
      wk->printSeq = SEQ_WAIT_STREAM_RUNNING;
      wk->printWait = wk->printWaitOrg;
    }
    break;

  case SEQ_DONE:
    if( !PMSND_CheckPlayingSEIdx(SEQ_SE_MESSAGE) ){
      wk->printSeq = SEQ_END;
      return TRUE;
    }
    break;

  case SEQ_END:
  default:
    return TRUE;
  }
  return FALSE;
}




//==============================================================================

//=============================================================================================
/**
 * ワザエフェクト発動開始
 *
 * @param   wk
 * @param   atPos
 * @param   defPos
 * @param   waza
 * @param   turnType
 * @param   continueCount
 */
//=============================================================================================
void BTLV_SCU_StartWazaEffect( BTLV_SCU* wk, BtlvMcssPos atPos, BtlvMcssPos defPos,
  WazaID waza, WazaTarget wazaRange, BtlvWazaEffect_TurnType turnType, u8 continueCount )
{
//  BTLV_WAZAEFFECT_PARAM param;
  BTLV_WAZAEFFECT_PARAM* param = Scu_GetProcWork( wk, sizeof(BTLV_WAZAEFFECT_PARAM) );
  param->waza = waza;
  param->from = atPos;
  param->to = defPos;
  param->turn_count = turnType;
  param->continue_count = continueCount;
  param->waza_range = wazaRange;

  /*  range は client レベルで判定
  if( waza == WAZANO_NOROI )
  {
    TAYA_Printf("のろい打つポケ位置=%d\n", atPos);
    param->waza_range = BTL_CALC_GetNoroiTargetType( BTL_POKECON_GetFrontPokeDataConst(wk->pokeCon, atPos) );
  }
  else
  {
    param->waza_range = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );
  }
  */

  BTL_UTIL_SetupProc( &wk->proc, wk, NULL, subproc_WazaEffect );
}
static BOOL subproc_WazaEffect( int* seq, void* wk_adrs )
{
  BTLV_SCU* wk = wk_adrs;

  switch( (*seq) ){
  case 0:
    msgWinVisible_Hide( &wk->msgwinVisibleWork );
    (*seq)++;
    break;
  case 1:
    if( msgWinVisible_Update(&wk->msgwinVisibleWork) ){
      BTLV_WAZAEFFECT_PARAM* param = Scu_GetProcWork( wk, sizeof(BTLV_WAZAEFFECT_PARAM) );
      BTLV_EFFECT_AddWazaEffect( param );
      (*seq)++;
    }
    break;
  case 2:
    if( !BTLV_EFFECT_CheckExecute() )
    {
//      msgWinVisible_Disp( &wk->msgwinVisibleWork, FALSE );
      PMSND_StopSE();
      (*seq)++;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
/**
 * ワザエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL    TRUEで終了
 */
//=============================================================================================
BOOL BTLV_SCU_WaitWazaEffect( BTLV_SCU* wk )
{
//  return BTLV_EFFECT_CheckExecute() == FALSE;
  return BTL_UTIL_CallProc( &wk->proc );
}

//==============================================================================

//=============================================================================================
/**
 *  ワザによるダメージエフェクト開始
 *
 * @param   wk
 * @param   defPos
 *
 */
//=============================================================================================
void BTLV_SCU_StartWazaDamageAct( BTLV_SCU* wk, BtlPokePos defPos, WazaID wazaID, BOOL fChapterSkipMode )
{
  const BTL_POKEPARAM*  bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, defPos );
  int                   value = BPP_GetValue( bpp, BPP_HP );
  BtlvMcssPos           vpos  = BTL_MAIN_BtlPosToViewPos( wk->mainModule, defPos );

  if( !fChapterSkipMode ){
    BTLV_EFFECT_CalcGaugeHP( vpos, value );
    BTLV_EFFECT_Damage( vpos, wazaID );
  }else{
    BTLV_EFFECT_CalcGaugeHPAtOnce( vpos, value );
  }
}
//=============================================================================================
/**
 * ワザによるダメージエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_WaitWazaDamageAct( BTLV_SCU* wk )
{
  return !( BTLV_EFFECT_CheckExecute() | BTLV_EFFECT_CheckExecuteGauge() );
}

//=============================================================================================
/**
 *  みがわりへのダメージエフェクト開始
 *
 * @param   wk
 * @param   defPos
 *
 */
//=============================================================================================
void BTLV_SCU_StartMigawariDamageAct( BTLV_SCU* wk, BtlPokePos pos, WazaID wazaID )
{
  BtlvMcssPos  vpos  = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

  BTLV_EFFECT_Damage( vpos, wazaID );
}
//=============================================================================================
/**
 *  みがわりへのダメージエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL  終了したらTRUE
 */
//=============================================================================================
BOOL BTLV_SCU_WaitMigawariDamageAct( BTLV_SCU* wk )
{
  return !BTLV_EFFECT_CheckExecute();
}

//=============================================================================================
/**
 * ポケモンひんしアクション開始
 *
 * @param   wk
 * @param   pos        ひんしになったポケモンの位置ID
 * @param   skipFlag   チャプタースキップフラグ
 *
 */
//=============================================================================================
void BTLV_SCU_StartDeadAct( BTLV_SCU* wk, BtlPokePos pos, BOOL skipFlag )
{
  BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

  BTLV_EFFECT_DelGauge( vpos );

  if( !skipFlag ){
    BTLV_EFFECT_Hinshi( vpos );
  }else{
    BTLV_EFFECT_DelPokemon( vpos );
  }
}
//=============================================================================================
/**
 * ポケモンひんしアクション終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_WaitDeadAct( BTLV_SCU* wk )
{
  //瀕死エフェクトが入ったらそれ待ちをいれる
  return !BTLV_EFFECT_CheckExecute();
}

//=============================================================================================
/**
 * ポケモン生き返りアクション開始
 *
 * @param   wk
 * @param   pos   ひんしになったポケモンの位置ID
 *
 */
//=============================================================================================
void BTLV_SCU_StartReliveAct( BTLV_SCU* wk, BtlPokePos pos )
{
  BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pos );
  if( bpp ){
    BTL_N_Printf( DBGSTR_SCU_RelivePokeAct, pos, vpos, BPP_GetID(bpp) );
  }else{
    GF_ASSERT(0);
  }

  BTLV_EFFECT_SetPokemon( BPP_GetViewSrcData(bpp), vpos );
  statwin_disp_start( &wk->statusWin[pos] );

}
//=============================================================================================
/**
 * ポケモン生き返りアクション終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_WaitReliveAct( BTLV_SCU* wk )
{
  return !BTLV_EFFECT_CheckExecute();
}




//--------------------------------------------------------
// ポケモン退場アクション
//--------------------------------------------------------
typedef struct {

  STATUS_WIN*  statWin;
  u16     seq;
  u16     viewpos;
  u16     effectNo;
  u8*     taskCounter;

}POKEOUT_ACT_WORK;


void BTLV_SCU_StartMemberOutAct( BTLV_SCU* wk, BtlvMcssPos vpos, u16 effectNo, BOOL fSkipMode )
{
  if( fSkipMode || (effectNo == 0))
  {
    BTLV_EFFECT_DelGauge( vpos );
    BTLV_EFFECT_DelPokemon( vpos );
  }
  else
  {
    GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskPokeOutAct, sizeof(POKEOUT_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
    POKEOUT_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

    twk->viewpos = vpos;
    twk->statWin = &wk->statusWin[ vpos ];
    twk->taskCounter = &wk->taskCounter[TASKTYPE_MEMBER_OUT];
    twk->effectNo = (effectNo != 0)? effectNo : BTLEFF_POKEMON_MODOSU;
    twk->seq = 0;

    (*(twk->taskCounter))++;
  }
}
BOOL BTLV_SCU_WaitMemberOutAct( BTLV_SCU* wk )
{
  return ( wk->taskCounter[TASKTYPE_MEMBER_OUT] == 0 );
}

static void taskPokeOutAct( GFL_TCBL* tcbl, void* wk_adrs )
{
  POKEOUT_ACT_WORK* twk = wk_adrs;

  switch( twk->seq ){
  case 0:
    TAYA_Printf("MemberOut EffectNo=%d\n", twk->effectNo );
    BTLV_EFFECT_AddByPos( twk->viewpos, twk->effectNo );
    twk->seq++;
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_EFFECT_DelGauge( twk->viewpos );
      BTLV_EFFECT_DelPokemon( twk->viewpos );
      twk->seq++;
    }
    break;
  case 2:
    (*(twk->taskCounter))--;
    GFL_TCBL_Delete( tcbl );
  }
}

//--------------------------------------------------------
// ポケモン入場アクション
//--------------------------------------------------------
typedef struct {

  STATUS_WIN*  statWin;
  u16     seq;
  u16     line;
  u8*     taskCounter;

}POKEIN_ACT_WORK;


//=============================================================================================
/**
 * ポケモン入場アクション開始
 *
 * @param   wk
 * @param   clientID
 *
 */
//=============================================================================================
void BTLV_SCU_StartPokeIn( BTLV_SCU* wk, BtlPokePos pos, u8 clientID, u8 memberIdx, BOOL fEffectSkip )
{
  GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskPokeInEffect, sizeof(POKEIN_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
  POKEIN_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

  twk->statWin = &wk->statusWin[ pos ];
  twk->taskCounter = &wk->taskCounter[TASKTYPE_MEMBER_IN];
  twk->seq = 0;

  (*(twk->taskCounter))++;

  //soga
  {
    const BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeDataConst( wk->pokeCon, clientID, memberIdx );
    {
      u8 vpos =  BTL_MAIN_BtlPosToViewPos(wk->mainModule, pos);
      const POKEMON_PARAM* pp = BPP_GetViewSrcData( bpp );
      BTL_Printf("入るポケモン  memIdx=%d, pokeID=%d, monsno=%d, monsno_src=%d\n",
            memberIdx, BPP_GetID(bpp), BPP_GetMonsNo(bpp), PP_Get(pp, ID_PARA_monsno, NULL) );
      BTLV_EFFECT_SetPokemon( pp, vpos );
      if( !fEffectSkip ){
        BTLV_EFFECT_AddByPos( vpos, BTLEFF_POKEMON_KURIDASU );
      }
    }
  }
}


BOOL BTLV_SCU_WaitPokeIn( BTLV_SCU* wk )
{
  return (wk->taskCounter[TASKTYPE_MEMBER_IN] == 0 );
}

static void taskPokeInEffect( GFL_TCBL* tcbl, void* wk_adrs )
{
  POKEIN_ACT_WORK* twk = wk_adrs;

  switch( twk->seq ){
  case 0:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      statwin_disp_start( twk->statWin );
      twk->seq++;
    }
    break;
  case 1:
    (*(twk->taskCounter))--;
    GFL_TCBL_Delete( tcbl );
  }
}

//--------------------------------------------------------
// HPゲージ増減エフェクト
//--------------------------------------------------------
typedef struct {

  STATUS_WIN*  statWin;
  fx32    hpVal;
  fx32    hpAddVal;
  u16     hpEnd;
  u16     timer;
  u8*     taskCounter;

}HP_GAUGE_EFFECT;


//=============================================================================================
/**
 * HPゲージ増減エフェクト開始
 *
 * @param   wk
 * @param   pos
 * @param   fSkipEffect   演出なし、一瞬でゲージを書き換える場合はTRUE（チャプタスキップ時に使用を想定）
 *
 */
//=============================================================================================
void BTLV_SCU_StartHPGauge( BTLV_SCU* wk, BtlPokePos pos, BOOL fSkipEffect )
{
  const BTL_POKEPARAM*  bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pos );
  int                   value = BPP_GetValue( bpp, BPP_HP );
  BtlvMcssPos           vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );

  if( fSkipEffect )
  {
    BTLV_EFFECT_CalcGaugeHPAtOnce( vpos, value );
  }
  else
  {
    BTLV_EFFECT_CalcGaugeHP( vpos, value );
  }
}
//=============================================================================================
/**
 * HPゲージ増減エフェクト終了待ち
 *
 * @param   wk
 * @param   pos
 *
 */
//=============================================================================================
BOOL BTLV_SCU_WaitHPGauge( BTLV_SCU* wk )
{
  return !(BTLV_EFFECT_CheckExecuteGauge());
}

//=============================================================================================
/**
 * HPゲージのムーブ動作を開始
 *
 * @param   wk
 * @param   pos1
 * @param   pos2
 */
//=============================================================================================
void BTLV_SCU_MoveGauge_Start( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 )
{
  statwin_hide( &wk->statusWin[ pos1 ] );
  statwin_hide( &wk->statusWin[ pos2 ] );
  //ピンチBGMチェックをしないようにする
  BTLV_EFFECT_SetPinchBGMNoCheck( TRUE );
}

//=============================================================================================
/**
 * HPゲージのムーブ動作 終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_MoveGauge_Wait( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 )
{
  statwin_disp_start( &wk->statusWin[ pos1 ] );
  statwin_disp_start( &wk->statusWin[ pos2 ] );
  //ピンチBGMチェックをするようにする
  BTLV_EFFECT_SetPinchBGMNoCheck( FALSE );
  return TRUE;
}

//=============================================================================================
/**
 * HPゲージ更新（対象ポケモンの変更などによる）
 *
 * @param   wk
 * @param   pos
 */
//=============================================================================================
void BTLV_SCU_UpdateGauge_Start( BTLV_SCU* wk, BtlPokePos pos )
{
  statwin_hide( &wk->statusWin[ pos ] );
  //ピンチBGMチェックをしないようにする
  BTLV_EFFECT_SetPinchBGMNoCheck( TRUE );
}
//=============================================================================================
/**
 * HPゲージ更新（対象ポケモンの変更などによる）終了待ち
 *
 * @param   wk
 */
//=============================================================================================
BOOL BTLV_SCU_UpdateGauge_Wait( BTLV_SCU* wk, BtlPokePos pos )
{
  statwin_disp_start( &wk->statusWin[ pos ] );
  //ピンチBGMチェックをするようにする
  BTLV_EFFECT_SetPinchBGMNoCheck( FALSE );
  return TRUE;
}

//=============================================================================================
/**
 * きのみ食べるアクション開始
 *
 * @param   wk
 * @param   pos
 */
//=============================================================================================
void BTLV_SCU_KinomiAct_Start( BTLV_SCU* wk, BtlvMcssPos pos )
{
  BTLV_EFFECT_AddByPos( pos, BTLEFF_SOUBI_ITEM );
}
//=============================================================================================
/**
 * きのみ食べるアクションの終了待ち
 *
 * @param   wk
 * @param   pos
 *
 * @retval  BOOL    終了時TRUE
 */
//=============================================================================================
BOOL BTLV_SCU_KinomiAct_Wait( BTLV_SCU* wk, BtlvMcssPos pos )
{
  return BTLV_EFFECT_CheckExecute() == FALSE;
}

//==============================================================================================
// イリュージョン解除動作
//==============================================================================================

//--------------------------------------------------------
// イリュージョン解除動作タスクワーク
//--------------------------------------------------------
typedef struct {

  BTLV_SCU*    parentWork;
  BtlPokePos   pos;
  BtlvMcssPos  vpos;
  u32          seq;
  BOOL         fSkipMode;
  u8*          pTaskCounter;

}FAKEDISABLE_ACT_WORK;


//=============================================================================================


/**
 *  イリュージョン解除動作：開始
 */
void BTLV_SCU_FakeDisable_Start( BTLV_SCU* wk, BtlPokePos pos, BOOL fSkipMode )
{
  if( pos != BTL_POS_NULL )
  {
    GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskFakeDisable, sizeof(FAKEDISABLE_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
    FAKEDISABLE_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

    twk->parentWork = wk;
    twk->pos = pos;
    twk->fSkipMode = fSkipMode;
    twk->vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
    twk->pTaskCounter = &wk->taskCounter[TASKTYPE_DEFAULT];
    twk->seq = 0;

    (*(twk->pTaskCounter))++;
  }
}

/**
 *  イリュージョン解除動作：終了待ち
 */
BOOL BTLV_SCU_FakeDisable_Wait( BTLV_SCU* wk )
{
  return ( wk->taskCounter[ TASKTYPE_DEFAULT ] == 0 );
}

/**
 *  イリュージョン解除動作：実行タスク
 */
static void taskFakeDisable( GFL_TCBL* tcbl, void* wk_adrs )
{
  FAKEDISABLE_ACT_WORK* wk = wk_adrs;

  switch( wk->seq ){
  case 0:
    {
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->parentWork->pokeCon, wk->pos );
      const POKEMON_PARAM* pp = BPP_GetSrcData( bpp );

      if( !(wk->fSkipMode) ){
        BTLV_EFFECT_Henge( pp, wk->vpos );
      }else{
        BTLV_EFFECT_HengeShortCut( pp, wk->vpos );
      }
      BTLV_EFFECT_DelGauge( wk->vpos );
      wk->seq++;
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecute() ){
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->parentWork->pokeCon, wk->pos );
      BTLV_EFFECT_SetGauge( wk->parentWork->mainModule, bpp, wk->vpos );
      (*(wk->pTaskCounter))--;
      GFL_TCBL_Delete( tcbl );
    }
    break;
  }
}

//==============================================================================================
// へんしん・フォルムチェンジ動作
//==============================================================================================

//--------------------------------------------------------
// へんしん・フォルムチェンジ動作タスクワーク
//--------------------------------------------------------
typedef struct {

  BTLV_SCU*    parentWork;
  BtlvMcssPos  vpos;
  const POKEMON_PARAM* pp;
  u32          seq;
  u8           taskCounterDefault;
  u8           fSkipMode;
  u8*          pTaskCounter;

}TRANSFORM_ACT_WORK;

/**
 *  へんしん 動作開始
 */
void BTLV_SCU_Hensin_Start( BTLV_SCU* wk, BtlvMcssPos vpos, BtlvMcssPos vpos_target, BOOL fSkipMode )
{
  GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskTransform, sizeof(TRANSFORM_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
  TRANSFORM_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

  BtlPokePos targetPos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, vpos_target );
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, targetPos );

  twk->pp = BPP_GetSrcData( bpp );
  twk->parentWork = wk;
  twk->vpos = vpos;
  twk->pTaskCounter = &wk->taskCounter[TASKTYPE_DEFAULT];
  twk->fSkipMode = fSkipMode;
  twk->seq = 0;

  (*(twk->pTaskCounter))++;
}
/**
 *  へんしん動作終了待ち
 */
BOOL BTLV_SCU_Hensin_Wait( BTLV_SCU* wk )
{
  return ( wk->taskCounter[ TASKTYPE_DEFAULT ] == 0 );
}

/**
 *  フォルムチェンジ動作開始
 */
void BTLV_SCU_ChangeForm_Start( BTLV_SCU* wk, BtlvMcssPos vpos, BOOL fSkipMode )
{
  GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskTransform, sizeof(TRANSFORM_ACT_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
  TRANSFORM_ACT_WORK* twk = GFL_TCBL_GetWork( tcbl );

  BtlPokePos targetPos = BTL_MAIN_ViewPosToBtlPos( wk->mainModule, vpos );
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, targetPos );

  twk->pp = BPP_GetSrcData( bpp );
  twk->parentWork = wk;
  twk->vpos = vpos;
  twk->pTaskCounter = &wk->taskCounter[TASKTYPE_DEFAULT];
  twk->fSkipMode = fSkipMode;
  twk->seq = 0;

  (*(twk->pTaskCounter))++;
}
/**
 *  フォルムチェンジ 動作終了待ち
 */
BOOL BTLV_SCU_ChangeForm_Wait( BTLV_SCU* wk )
{
  return ( wk->taskCounter[ TASKTYPE_DEFAULT ] == 0 );
}

/**
 *  へんしん・フォルムチェンジ：実行タスク
 */
static void taskTransform( GFL_TCBL* tcbl, void* wk_adrs )
{
  TRANSFORM_ACT_WORK* wk = wk_adrs;

  switch( wk->seq ){
  case 0:
    {
      if( !(wk->fSkipMode) ){
        BTLV_EFFECT_Henge( wk->pp, wk->vpos );
      }else{
        BTLV_EFFECT_HengeShortCut( wk->pp, wk->vpos );
      }
      wk->seq++;
    }
    break;
  case 1:
    if( !BTLV_EFFECT_CheckExecute() ){
      (*(wk->pTaskCounter))--;
      GFL_TCBL_Delete( tcbl );
    }
    break;
  }
}


//==============================================================================================
// メッセージウィンドウ表示・消去処理
//==============================================================================================
static void msgWinVisible_Init( MSGWIN_VISIBLE* wk, GFL_BMPWIN* win )
{
  wk->win = win;
  wk->state = MSGWIN_STATE_DISP;
  wk->eva = FX32_CONST( MSGWIN_EVA_MAX );
  wk->evb = FX32_CONST( MSGWIN_EVB_MIN );

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                    GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                    GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                    MSGWIN_EVA_MAX, MSGWIN_EVB_MIN );
}
static void msgWinVisible_Hide( MSGWIN_VISIBLE* wk )
{
  if( wk->state == MSGWIN_STATE_DISP
  ||  wk->state == MSGWIN_STATE_TO_DISP
  ){
    wk->eva = FX32_CONST( MSGWIN_EVA_MAX );
    wk->evb = FX32_CONST( MSGWIN_EVB_MIN );
    wk->eva_step = FX32_CONST( MSGWIN_EVA_MIN - MSGWIN_EVA_MAX ) / MSGWIN_VISIBLE_STEP;
    wk->evb_step = FX32_CONST( MSGWIN_EVB_MAX - MSGWIN_EVB_MIN ) / MSGWIN_VISIBLE_STEP;
    wk->timer = MSGWIN_VISIBLE_STEP;
    wk->state = MSGWIN_STATE_TO_HIDE;
  }
}
static void msgWinVisible_Disp( MSGWIN_VISIBLE* wk, BOOL printAtOnceFlag )
{
  if( wk->state == MSGWIN_STATE_HIDE
  ||  wk->state == MSGWIN_STATE_TO_HIDE
  ){
    if( !printAtOnceFlag )
    {
      GFL_BMP_DATA* bmp = GFL_BMPWIN_GetBmp( wk->win );
      GFL_BMP_Clear( bmp, COLIDX_MSGWIN_CLEAR );
      GFL_BMPWIN_TransVramCharacter( wk->win );
    }

    wk->eva = FX32_CONST( MSGWIN_EVA_MIN );
    wk->evb = FX32_CONST( MSGWIN_EVB_MAX );
    wk->eva_step = FX32_CONST( MSGWIN_EVA_MAX - MSGWIN_EVA_MIN ) / MSGWIN_VISIBLE_STEP;
    wk->evb_step = FX32_CONST( MSGWIN_EVB_MIN - MSGWIN_EVB_MAX ) / MSGWIN_VISIBLE_STEP;
    wk->timer = MSGWIN_VISIBLE_STEP;
    wk->state = MSGWIN_STATE_TO_DISP_READY;
  }
}
static BOOL msgWinVisible_Update( MSGWIN_VISIBLE* wk )
{
  switch( wk->state ){
  case MSGWIN_STATE_HIDE:
    return TRUE;
  case MSGWIN_STATE_DISP:
    return TRUE;
  case MSGWIN_STATE_TO_DISP_READY:
    wk->state = MSGWIN_STATE_TO_DISP;
    return FALSE;
  case MSGWIN_STATE_TO_DISP:
    if( wk->timer ){
      wk->eva += wk->eva_step;
      wk->evb += wk->evb_step;
      wk->timer--;
    }else{
      wk->eva = FX32_CONST( MSGWIN_EVA_MAX );
      wk->evb = FX32_CONST( MSGWIN_EVB_MIN );
      wk->state = MSGWIN_STATE_DISP;
    }
    break;
  case MSGWIN_STATE_TO_HIDE:
    if( wk->timer ){
      wk->eva += wk->eva_step;
      wk->evb += wk->evb_step;
      wk->timer--;
    }else{
      wk->eva = FX32_CONST( MSGWIN_EVA_MIN );
      wk->evb = FX32_CONST( MSGWIN_EVB_MAX );
      wk->state = MSGWIN_STATE_HIDE;
    }
    break;
  }

  {
    u8 eva = wk->eva >> FX32_SHIFT;
    u8 evb = wk->evb >> FX32_SHIFT;

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                    GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                    GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                    eva, evb );

  }
  return FALSE;
}
//----------------------------

typedef struct {

  BTLV_SCU*        parentWork;
  u32          seq;
  u8*          pTaskCounter;

}MSGWINHIDE_TASK_WORK;

void BTLV_SCU_MsgWinHide_Start( BTLV_SCU* wk )
{
  GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskMsgWinHide, sizeof(MSGWINHIDE_TASK_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
  MSGWINHIDE_TASK_WORK* twk = GFL_TCBL_GetWork( tcbl );

  twk->parentWork = wk;
  twk->pTaskCounter = &wk->taskCounter[TASKTYPE_DEFAULT];
  twk->seq = 0;

  (*(twk->pTaskCounter))++;
}
BOOL BTLV_SCU_MsgWinHide_Wait( BTLV_SCU* wk )
{
  return ( wk->taskCounter[ TASKTYPE_DEFAULT ] == 0 );
}
/**
 *
 */
static void taskMsgWinHide( GFL_TCBL* tcbl, void* wk_adrs )
{
  MSGWINHIDE_TASK_WORK* wk = wk_adrs;

  switch( wk->seq ){
  case 0:
    msgWinVisible_Hide( &wk->parentWork->msgwinVisibleWork );
    wk->seq++;
    break;
  case 1:
    if( msgWinVisible_Update( &wk->parentWork->msgwinVisibleWork ) ){
      (*(wk->pTaskCounter))--;
      GFL_TCBL_Delete( tcbl );
    }
  }
}

//==============================================================================================
// HP・STATUSゲージ
//==============================================================================================
static void statwin_setupAll( BTLV_SCU* wk )
{
  u32 pos_end;
  u32 i;

  for(i=0; i<=NELEMS(wk->statusWin); ++i)
  {
    wk->statusWin[i].enableFlag = FALSE;
    wk->statusWin[i].bpp = NULL;
  }

  if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_ROTATION ){
    pos_end = BTL_MAIN_GetEnablePosEnd( wk->mainModule );
  }else{
    pos_end = BTL_POS_2ND_2;
  }
  for(i=0; i<=pos_end; ++i){
    statwin_setup( &wk->statusWin[ i ], wk, i );
  }
}

static void statwin_cleanupAll( BTLV_SCU* wk )
{
  int i;
  for(i=0; i<NELEMS(wk->statusWin); i++){
    statwin_cleanup( &wk->statusWin[i] );
  }
}

static void statwin_setup( STATUS_WIN* stwin, BTLV_SCU* wk, BtlPokePos pokePos )
{
  stwin->pokePos = pokePos;
  stwin->vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pokePos );
  stwin->parentWk = wk;
  stwin->dispFlag = FALSE;
  stwin->enableFlag = TRUE;
  stwin->bpp = NULL;
}

static void statwin_cleanup( STATUS_WIN* stwin )
{
  if( stwin->enableFlag )
  {
    if( stwin->dispFlag ){
      BTLV_EFFECT_DelGauge( stwin->vpos );
      stwin->dispFlag = FALSE;
    }
    stwin->bpp = NULL;
    stwin->enableFlag = FALSE;
  }
}

static void statwin_hide( STATUS_WIN* stwin )
{
  if( stwin->dispFlag ){
    BTLV_EFFECT_DelGauge( stwin->vpos );
    stwin->dispFlag = FALSE;
  }
}

static void statwin_disp_start( STATUS_WIN* stwin )
{
  if( stwin->enableFlag )
  {
    stwin->bpp = BTL_POKECON_GetFrontPokeDataConst( stwin->parentWk->pokeCon, stwin->pokePos );
    if( !BPP_IsDead(stwin->bpp) )
    {
      BTL_Printf("ここでポケID=%d, HP=%d pos=%d\n", BPP_GetID(stwin->bpp), BPP_GetValue(stwin->bpp,BPP_HP), stwin->pokePos);
      BTLV_EFFECT_SetGauge( stwin->parentWk->mainModule, stwin->bpp, stwin->vpos );
      stwin->dispFlag = TRUE;
    }
  }
  else
  {
    GF_ASSERT(0);
  }
}


//----------------------------------------------------------------------------------------------
// とくせいウィンドウ
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * 戦闘位置 -> とくせいウィンドウ位置へ変換
 *
 * @param   mainModule
 * @param   pos
 *
 * @retval  TokwinSide
 */
//----------------------------------------------------------------------------------
static TokwinSide PokePosToTokwinSide( const BTL_MAIN_MODULE* mainModule, BtlPokePos pos )
{
  BtlSide btlSide = BTL_MAINUTIL_PosToSide( pos );
  return BTL_MAIN_IsPlayerSide( mainModule, btlSide )? TOKWIN_SIDE_FRIEND : TOKWIN_SIDE_ENEMY;
}

//=============================================================================================
/**
 * とくせいウィンドウ表示開始
 *
 * @param   wk
 * @param   pos
 * @param   fFlash    効果発動を示すフラッシュ動作を入れるか?
 */
//=============================================================================================
void BTLV_SCU_TokWin_DispStart( BTLV_SCU* wk, BtlPokePos pos, BOOL fFlash )
{
  TokwinSide side = PokePosToTokwinSide( wk->mainModule, pos );

  //自分側の特性ウインドウを表示する場合は、ゲームタイマー表示を一旦消す
  if( side == TOKWIN_SIDE_FRIEND )
  {
    BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_GAME_TIME, FALSE, FALSE );
  }

  //ステータス上昇下降エフェクトの連続起動抑制をクリア
  BTLV_EFFECT_ClearOldStatusEffectBuffer();

//  BTLV_SCU_RestoreDefaultScreen( wk );
  tokwin_disp_first( &wk->tokWin[side], pos, fFlash );
}
BOOL BTLV_SCU_TokWin_DispWait( BTLV_SCU* wk, BtlPokePos pos )
{
  TokwinSide side = PokePosToTokwinSide( wk->mainModule, pos );
  return tokwin_disp_progress( &wk->tokWin[side] );
}
//=============================================================================================
/**
 * とくせいウィンドウ表示オフ
 *
 * @param   wk
 * @param   clientID
 *
 */
//=============================================================================================
void BTLV_SCU_TokWin_HideStart( BTLV_SCU* wk, BtlPokePos pos )
{
  BtlSide side = PokePosToTokwinSide( wk->mainModule, pos );
  tokwin_hide_first( &wk->tokWin[side] );
  //消していたゲームタイマー表示を復活
  if( side == TOKWIN_SIDE_FRIEND )
  {
    BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE_GAME_TIME, TRUE, FALSE );
  }
}
BOOL BTLV_SCU_TokWin_HideWait( BTLV_SCU* wk, BtlPokePos pos )
{
  BtlSide side = PokePosToTokwinSide( wk->mainModule, pos );
  return tokwin_hide_progress( &wk->tokWin[side] );
}
//=============================================================================================
/**
 * とくせいウィンドウの内容更新（開始）
 *
 * @param   wk
 * @param   pos
 *
 */
//=============================================================================================
void BTLV_SCU_TokWin_Renew_Start( BTLV_SCU* wk, BtlPokePos pos )
{
  TokwinSide side = PokePosToTokwinSide( wk->mainModule, pos );
  tokwin_renew_start( &wk->tokWin[side], pos );
}
//=============================================================================================
/**
 * とくせいウィンドウの内容更新（終了待ち）
 *
 * @param   wk
 * @param   pos
 *
 * @retval  BOOL    終了したらTRUE
 */
//=============================================================================================
BOOL BTLV_SCU_TokWin_Renew_Wait( BTLV_SCU* wk, BtlPokePos pos )
{
  TokwinSide side = PokePosToTokwinSide( wk->mainModule, pos );
  return tokwin_renew_progress( &wk->tokWin[side] );
}



//--------------------------------------------------------
// とくせいウィンドウスワップ演出
//--------------------------------------------------------
typedef struct {

  BTLV_SCU*   parentWork;
  u16         seq;
  u16         timer;
  u8          fDispEnd;
  u8          fNextStart;
  u8          fNextDispEnd;

  BtlPokePos   pos;
  BtlPokePos   next_pos;

  u8*     pTaskCounter;

}TOKWIN_SWAP_WORK;

static TOKWIN_SWAP_WORK* addSwapTask( BTLV_SCU* wk )
{
  GFL_TCBL* tcbl = GFL_TCBL_Create( wk->tcbl, taskTokWinSwap, sizeof(TOKWIN_SWAP_WORK), BTLV_TASKPRI_DAMAGE_EFFECT );
  TOKWIN_SWAP_WORK* twk = GFL_TCBL_GetWork( tcbl );

  twk->parentWork = wk;
  twk->seq = 0;
  twk->timer = 0;
  twk->pTaskCounter = &wk->taskCounter[TASKTYPE_TOKWIN_SWAP];

  (*(twk->pTaskCounter))++;

  return twk;
}

//=============================================================================================
/**
 * とくせいウィンドウのスワップ演出（開始）
 *
 * @param   wk
 * @param   pos1
 * @param   pos2
 */
//=============================================================================================
void BTLV_SCU_TokWin_Swap_Start( BTLV_SCU* wk, BtlPokePos pos1, BtlPokePos pos2 )
{
  TOKWIN_SWAP_WORK* twk = addSwapTask( wk );

  twk->pos = pos1;
  twk->next_pos = pos2;
}

//=============================================================================================
/**
 * とくせいウィンドウのスワップ演出（終了待ち）
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_SCU_TokWin_Swap_Wait( BTLV_SCU* wk )
{
//  return ( wk->taskCounter[ TASKTYPE_TOKWIN_SWAP ] == 0 );
  return TRUE;
}
//-----------------------------------------------------------
/**
 *  スワップ演出タスク
 */
//-----------------------------------------------------------
static void taskTokWinSwap( GFL_TCBL* tcbl, void* wk_adrs )
{
  // 未実装・未使用
}


static void tokwin_setupAll( BTLV_SCU* wk, u32 charpos_top )
{
  u32 i;

  wk->tokWinCgrHead = GFL_ARC_UTIL_LoadBGCharacter( ARCID_BATTGRA, NARC_battgra_wb_tokusei_w_NCGR,
                        FALSE, &wk->tokWinCgr, wk->heapID );

  for(i=0; i<NELEMS(wk->tokWin); ++i)
  {
    wk->tokWin[i].parentWk = wk;
    wk->tokWin[i].mySide = i;
    wk->tokWin[i].bgFrame = (i == TOKWIN_SIDE_FRIEND)? BGFRAME_TOKWIN_FRIEND : BGFRAME_TOKWIN_ENEMY;
    wk->tokWin[i].cgrSrc = ((const u8*)wk->tokWinCgr->pRawData) + TOKWIN_CGRDATA_TRANS_SIZE * i;
    wk->tokWin[i].bmp = GFL_BMP_Create( TOKWIN_CGRDATA_CHAR_W, TOKWIN_DRAWAREA_CHAR_H, GFL_BMP_16_COLOR, wk->heapID );
    wk->tokWin[i].tokusei = POKETOKUSEI_NULL;
    wk->tokWin[i].pokeID = BTL_POKEID_NULL;
    wk->tokWin[i].charPos = charpos_top + TOKWIN_CGRDATA_TRANS_CHARS * i;
    wk->tokWin[i].dispFlag = FALSE;
  }
}
static void tokwin_cleanupAll( BTLV_SCU* wk )
{
  int i;
  for(i=0; i<NELEMS(wk->tokWin); i++)
  {
    if( wk->tokWin[i].bmp != NULL ){
      GFL_BMP_Delete( wk->tokWin[i].bmp );
      wk->tokWin[i].bmp = NULL;
    }
  }

  if( wk->tokWinCgrHead ){
    GFL_HEAP_FreeMemory( wk->tokWinCgrHead );
    wk->tokWinCgrHead = NULL;
  }
}
// 表示開始
static void tokwin_disp_first( TOK_WIN* tokwin, BtlPokePos pos, BOOL fFlash )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( tokwin->parentWk->pokeCon, pos );
  u16 tokusei = BPP_GetValue( bpp, BPP_TOKUSEI );
  u8  pokeID = BPP_GetID( bpp );

  if( tokwin->dispFlag )
  {
    if( pokeID == tokwin->pokeID ){
      return;
    }
    tokwin_hide_first( tokwin );
  }

//    TAYA_Printf("とくせいウィンドウ: pokeID=%d, tokID=%d\n", pokeID, tokusei);
  if( (tokusei != tokwin->tokusei) || (pokeID != tokwin->pokeID) )
  {
    tokwin->tokusei = tokusei;
    tokwin->pokeID = pokeID;
    tokwin_update_cgr( tokwin );
  }
  tokwin->fFlash = fFlash;
  tokwin->seq = 0;

  //特性の発現を記憶する
   BTLV_EFFECT_SetLookTokusei( pos, tokusei );
}
// 表示更新
static BOOL tokwin_disp_progress( TOK_WIN* tokwin )
{
  switch( tokwin->seq ){
  case 0:
    GFL_BG_LoadCharacter( tokwin->bgFrame, tokwin->cgrSrc,
        TOKWIN_CGRDATA_TRANS_SIZE, tokwin->charPos);
    tokwin->seq++;
    break;
  case 1:
    tokwin->moveTimer = TOKWIN_MOVE_FRAMES;
    if( tokwin->mySide == TOKWIN_SIDE_FRIEND )
    {
      tokwin->posX = FX32_CONST( TOKWIN_HIDEPOS_FRIEND );
      tokwin->speedX = FX32_CONST( -TOKWIN_HIDEPOS_FRIEND ) / TOKWIN_MOVE_FRAMES;
    }
    else
    {
      tokwin->posX = FX32_CONST( TOKWIN_HIDEPOS_ENEMY );
      tokwin->speedX = FX32_CONST( -TOKWIN_HIDEPOS_ENEMY ) / TOKWIN_MOVE_FRAMES;
    }

    {
      int x = tokwin->posX >> FX32_SHIFT;
      GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_X_SET, x );
      GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_Y_SET, 0 );
    }

    PMSND_PlaySE( TOKWIN_SE_IN );
    tokwin->seq++;
    break;
  case 2:
    if( tokwin->moveTimer )
    {
      int x;
      tokwin->posX += tokwin->speedX;
      x = tokwin->posX >> FX32_SHIFT;
      GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_X_SET, x );
      tokwin->moveTimer--;
    }
    else
    {
      GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_X_SET, 0 );
      tokwin->writeRaw = 0;
      tokwin->seq++;
    }
    break;
  case 3:
    if( tokwin->writeRaw < TOKWIN_DRAWAREA_CHAR_W )
    {
      const u8* cp = GFL_BMP_GetCharacterAdrs( tokwin->bmp );
      u32 offs = tokwin->writeRaw * 0x20;
      u32 writeCharOfs = (tokwin->charPos + tokwin->writeRaw);
      u32 i;
      cp += offs;
      for(i=0; i<TOKWIN_DRAWAREA_CHAR_H; ++i)
      {
        GFL_BG_LoadCharacter( tokwin->bgFrame, cp+TOKWIN_CGRDATA_LINE_SIZE*i, 0x20,
              writeCharOfs+TOKWIN_CGRDATA_CHAR_W*i );
      }
      tokwin->writeRaw++;
    }
    else
    {
      tokwin->dispFlag = TRUE;
      if( tokwin->fFlash ){
        tokwin->seq++;
      }else{
        return TRUE;
      }
    }
    break;
  case 4:
    {
      PALETTE_FADE_PTR pfd = BTLV_EFFECT_GetPfd();
      GFL_TCBSYS* tcbSys = BTLV_EFFECT_GetTCBSYS();
      u16 palBit = 1 << (PALIDX_TOKWIN1 + tokwin->mySide );

      PMSND_PlaySE( TOKWIN_SE_FLASH );
      PaletteFadeReq( pfd, PF_BIT_MAIN_BG, palBit, 0, 16, 0, 0x7fff, tcbSys );
      tokwin->seq++;
    }
    break;
  case 5:
    {
      PALETTE_FADE_PTR pfd = BTLV_EFFECT_GetPfd();

      if( PaletteFadeCheck(pfd) == 0){
        tokwin->seq++;
      }
    }
    break;
  case 6:
    if(  !(PMSND_CheckPlayingSEIdx(TOKWIN_SE_FLASH) || PMSND_CheckPlayingSEIdx(TOKWIN_SE_IN)) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

// CGR更新
static void tokwin_update_cgr( TOK_WIN* tokwin )
{
  BTLV_SCU* wk = tokwin->parentWk;

  // 自前CGR領域を元データで初期化する
  {
    u32 bmp_size = GFL_BMP_GetBmpDataSize( tokwin->bmp );
    u8* dst_p = GFL_BMP_GetCharacterAdrs( tokwin->bmp );
    u8* src_p = ((u8*)(tokwin->parentWk->tokWinCgr->pRawData)) + (tokwin->mySide * TOKWIN_CGRDATA_TRANS_SIZE);
    GFL_STD_MemCopy32( src_p, dst_p, bmp_size );
  }

  {
    u32 width[ 2 ];
    u8  lines;

    // フォント色設定
    GFL_FONTSYS_SetColor( COLIDX_TOKWIN_LETTER, COLIDX_TOKWIN_SHADOW, COLIDX_TOKWIN_CLEAR );

    // ２行ある場合、文字列をイイ感じに配置する
    BTL_STR_MakeStringStd( wk->strBufSubA, BTL_STRID_STD_TokWin, 2, tokwin->pokeID, tokwin->tokusei );
    lines = PRINTSYS_GetStrLineWidth( wk->strBufSubA, wk->smallFont, 0, width, NELEMS(width) );
    if( lines > 1 )
    {
      int line1_end  = TOKWIN_MARGIN_L + width[0];
      int line2_head = TOKWIN_DRAWAREA_WIDTH - TOKWIN_MARGIN_R - width[1];
      int diff = line1_end - line2_head;
      if( diff < TOKWIN_LINE_CROSS_WIDTH )
      {
        diff = TOKWIN_LINE_CROSS_WIDTH - diff;
        line1_end += (diff / 2);
        line2_head -= (diff - (diff / 2));
      }

      {
        int line1_head = line1_end - width[0];
        if( line1_head < 0 ){ line1_head = 0; }
        if( line2_head < 0 ){ line2_head = 0; }

        PRINTSYS_CopyLineStr( wk->strBufSubA, wk->strBufSubB, 0 );
        PRINTSYS_Print( tokwin->bmp, line1_head, TOKWIN_DRAWAREA_LINE1_OY, wk->strBufSubB, wk->smallFont );

        PRINTSYS_CopyLineStr( wk->strBufSubA, wk->strBufSubB, 1 );
        PRINTSYS_Print( tokwin->bmp, line2_head, TOKWIN_DRAWAREA_LINE2_OY, wk->strBufSubB, wk->smallFont );
      }
    }
    // １行なら単純なセンタリング表示
    else
    {
      int xpos = (TOKWIN_DRAWAREA_WIDTH - PRINTSYS_GetStrWidth(wk->strBufSubA, wk->smallFont, 0)) / 2;
      if( xpos < 0 ){
        xpos = 0;
      }
      PRINTSYS_Print( tokwin->bmp, xpos, TOKWIN_DRAWAREA_LINE2_OY, wk->strBufSubA, wk->smallFont );
    }
  }
}

static void tokwin_hide_first( TOK_WIN* tokwin )
{
  if( tokwin->mySide == TOKWIN_SIDE_FRIEND ){
    GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_X_SET, TOKWIN_HIDEPOS_FRIEND );
  }else{
    GFL_BG_SetScroll( tokwin->bgFrame, GFL_BG_SCROLL_X_SET, TOKWIN_HIDEPOS_ENEMY );
  }
  tokwin->dispFlag = FALSE;
}
static BOOL tokwin_hide_progress( TOK_WIN* tokwin )
{
  return TRUE;
}

static void tokwin_renew_start( TOK_WIN* tokwin, BtlPokePos pos )
{
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( tokwin->parentWk->pokeCon, pos );
  u16 tokusei = BPP_GetValue( bpp, BPP_TOKUSEI );
  u8  pokeID = BPP_GetID( bpp );

  if( (tokusei != tokwin->tokusei) || (pokeID != tokwin->pokeID) )
  {
    tokwin->tokusei = tokusei;
    tokwin->pokeID = pokeID;
    tokwin_update_cgr( tokwin );
  }
  tokwin->seq = 0;
  //特性の発現を記憶する
   BTLV_EFFECT_SetLookTokusei( pos, tokusei );
}
static BOOL tokwin_renew_progress( TOK_WIN* tokwin )
{
  enum {
    MOSAIC_WAIT_FRAMES = 1,
  };
  switch( tokwin->seq ){
  case 0:
    G2_SetBGMosaicSize( 0, 0 );
    if( tokwin->mySide == TOKWIN_SIDE_FRIEND ){
      G2_BG2Mosaic( TRUE );
    }else{
      G2_BG3Mosaic( TRUE );
    }
    tokwin->moveTimer = 0;
    tokwin->writeRaw = 0;
    PMSND_PlaySE( TOKWIN_SE_CHANGE );
    tokwin->seq++;
    break;
  case 1:
    tokwin->moveTimer++;
    if( tokwin->moveTimer > MOSAIC_WAIT_FRAMES )
    {
      tokwin->moveTimer = 0;
      tokwin->writeRaw++;
      G2_SetBGMosaicSize( tokwin->writeRaw, tokwin->writeRaw );
      if( tokwin->writeRaw >= 15 ){
        tokwin->seq++;
      }
    }
    break;
  case 2:
    {
      const u8* cp = GFL_BMP_GetCharacterAdrs( tokwin->bmp );
      GFL_BG_LoadCharacter( tokwin->bgFrame, cp, TOKWIN_CGRDATA_TRANS_SIZE, tokwin->charPos );
      tokwin->seq++;
    }
    break;
  case 3:
    tokwin->moveTimer++;
    if( tokwin->moveTimer > MOSAIC_WAIT_FRAMES )
    {
      tokwin->moveTimer = 0;
      tokwin->writeRaw--;
      G2_SetBGMosaicSize( tokwin->writeRaw, tokwin->writeRaw );
      if( tokwin->writeRaw == 0 ){
        tokwin->seq++;
      }
    }
    break;
  case 4:
    if( tokwin->mySide == TOKWIN_SIDE_FRIEND ){
      G2_BG2Mosaic( FALSE );
    }else{
      G2_BG3Mosaic( FALSE );
    }
    (tokwin->seq++);
    break;
  case 5:
    if(  !PMSND_CheckPlayingSEIdx(TOKWIN_SE_CHANGE) ){
      tokwin->seq++;
    }
    break;
  default:
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
/**
 * @brief ボールゲージセットアップパラメータ生成
 *
 * @param[in]   wk
 * @param[out]  bbgp  セットアップパラメータ構造体のポインタ
 * @param[in]   pos   セットアップするポケモンの立ち位置
 * @param[in]   type  ボールゲージタイプ
 *
 */
//=============================================================================================
static void bbgp_make( BTLV_SCU* wk, BTLV_BALL_GAUGE_PARAM* bbgp, u8 clientID_1, u8 clientID_2, BTLV_BALL_GAUGE_TYPE type )
{
  int i;
  const BTL_PARTY*      bparty[2]={NULL,};
  const BTL_POKEPARAM*  bpp;
  int count[2]={0};
  s8 party_idx;
  s8 party_ofs;

  //　ゲージパラメータクリア
  GFL_STD_MemClear( bbgp, sizeof(BTLV_BALL_GAUGE_PARAM) );

  // BTS:5245
  // タッグ、マルチ　２人分のボールゲージが表示されるよう修正
  bparty[0] = BTL_POKECON_GetPartyDataConst( wk->pokeCon, clientID_1 );
  count[0]  = BTL_PARTY_GetMemberCount( bparty[0] );
  if( clientID_2 < BTL_CLIENT_NUM ){
    bparty[1] = BTL_POKECON_GetPartyDataConst( wk->pokeCon, clientID_2 );
    count[1]  = BTL_PARTY_GetMemberCount( bparty[1] );

    //タッグ、マルチ　3以下のみ参照
    if( count[0] > (TEMOTI_POKEMAX/2) ){ count[0] = (TEMOTI_POKEMAX/2); }
    if( count[1] > (TEMOTI_POKEMAX/2) ){ count[1] = (TEMOTI_POKEMAX/2); }
  }


  bbgp->type = type;


  for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
  {
    if( i < (count[0] + count[1]) )
    {
      if( i<count[0] ){
        party_idx = 0;
        party_ofs = i;
      }else{
        party_idx = 1;
        party_ofs = i-count[0];
      }

      bpp = BTL_PARTY_GetMemberDataConst( bparty[party_idx], party_ofs );
      if( BPP_IsDead( bpp ) )
      {
        bbgp->status[ (party_idx*3)+party_ofs ] = BTLV_BALL_GAUGE_STATUS_DEAD;
      }
      else if( BPP_GetPokeSick( bpp ) != POKESICK_NULL )
      {
        bbgp->status[ (party_idx*3)+party_ofs ] = BTLV_BALL_GAUGE_STATUS_NG;
      }
      else if( BPP_IsFightEnable( bpp ) == FALSE )
      {
        bbgp->status[ (party_idx*3)+party_ofs ] = BTLV_BALL_GAUGE_STATUS_NONE;
      }
      else
      {
        bbgp->status[ (party_idx*3)+party_ofs ] = BTLV_BALL_GAUGE_STATUS_ALIVE;
      }
    }
  }
}



//=============================================================================================
//  「通信待機中」表示オン・オフ
//=============================================================================================
void BTLV_SCU_StartCommWaitInfo( BTLV_SCU* wk )
{
  BTL_STR_GetUIString( wk->strBufMain, BTLSTR_UI_COMM_WAIT );
  BTLV_SCU_StartMsg( wk, wk->strBufMain, 0, NULL );
}

BOOL BTLV_SCU_WaitCommWaitInfo( BTLV_SCU* wk )
{
  return BTLV_SCU_WaitMsg( wk );
}
void BTLV_SCU_ClearCommWaitInfo( BTLV_SCU* wk )
{
//  GFL_BMP_Clear( wk->bmp, 0x0f );
//  GFL_BMPWIN_TransVramCharacter( wk->win );
}


//=============================================================================================
//  録画再生モードのフェードアウト処理
//=============================================================================================

void BTLV_SCU_RecPlayFadeOut_Start( BTLV_SCU* wk )
{
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, -3 );
}

BOOL BTLV_SCU_RecPlayFadeOut_Wait( BTLV_SCU* wk )
{
//  GFL_FADE_Main
  return Fade_CheckEnd( wk );
}

void BTLV_SCU_RecPlayFadeIn_Start( BTLV_SCU* wk )
{
  GFL_BMP_Clear( wk->bmp, COLIDX_MSGWIN_CLEAR );
  GFL_BMPWIN_TransVramCharacter( wk->win );

  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, -3 );
}

BOOL BTLV_SCU_RecPlayFadeIn_Wait( BTLV_SCU* wk )
{
//  GFL_FADE_Main
  return Fade_CheckEnd( wk );
}

//=============================================================================================
//  レベルアップ情報表示処理
//=============================================================================================
//----------------------------------------------------------------
/**
 *  表示処理 開始
 */
//----------------------------------------------------------------
void BTLV_SCU_LvupWin_StartDisp( BTLV_SCU* wk, const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo )
{
  wk->lvupBpp = bpp;
  wk->lvupInfo = *lvupInfo;

  BTL_UTIL_SetupProc( &wk->proc, wk, NULL, lvupWinProc_Disp );
}
//----------------------------------------------------------------
/**
 *  表示処理 終了待ち
 */
//----------------------------------------------------------------
BOOL BTLV_SCU_LvupWin_WaitDisp( BTLV_SCU* wk )
{
  return BTL_UTIL_CallProc( &wk->proc );
}
//----------------------------------------------------------------
/**
 *  次ページ表示処理 開始
 */
//----------------------------------------------------------------
void BTLV_SCU_LvupWin_StepFwd( BTLV_SCU* wk )
{
  BTL_UTIL_SetupProc( &wk->proc, wk, NULL, lvupWinProc_Fwd );
}
//----------------------------------------------------------------
/**
 *  次ページ表示処理 終了待ち
 */
//----------------------------------------------------------------
BOOL BTLV_SCU_LvupWin_WaitFwd( BTLV_SCU* wk )
{
  return BTL_UTIL_CallProc( &wk->proc );
}
//----------------------------------------------------------------
/**
 *  消去処理 開始
 */
//----------------------------------------------------------------
void BTLV_SCU_LvupWin_StartHide( BTLV_SCU* wk )
{
  BTL_UTIL_SetupProc( &wk->proc, wk, NULL, lvupWinProc_Hide );
}
//----------------------------------------------------------------
/**
 *  消去処理 終了待ち
 */
//----------------------------------------------------------------
BOOL BTLV_SCU_LvupWin_WaitHide( BTLV_SCU* wk )
{
  return BTL_UTIL_CallProc( &wk->proc );
}

/**
 *  １ページ目表示処理（レベルアップによる差分）
 */
static BOOL lvupWinProc_Disp( int* seq, void* wk_adrs )
{
  BTLV_SCU* wk = wk_adrs;

  switch( *seq ){
  case 0:
    GFL_FONTSYS_SetColor( COLIDX_LVUPWIN_LETTER, COLIDX_LVUPWIN_SHADOW, COLIDX_LVUPWIN_CLEAR );

    GFL_BMPWIN_MakeScreen( wk->lvupWin );
    GFL_BMPWIN_MakeFrameScreen( wk->lvupWin, wk->lvupWin_frameCharPos, PALIDX_LVUPWIN );
    GFL_BMP_Clear( wk->lvupBmp, COLIDX_LVUPWIN_CLEAR );
    GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->lvupWin) );
    (*seq)++;
    break;

  case 1:
    BTL_STR_MakeString_LvupInfo_Diff( wk->strBufMain,
      wk->lvupInfo.hp, wk->lvupInfo.atk, wk->lvupInfo.def, wk->lvupInfo.sp_atk,
      wk->lvupInfo.sp_def, wk->lvupInfo.agi );

    PRINTSYS_Print( wk->lvupBmp, 0, 0, wk->strBufMain, wk->defaultFont );
    (*seq)++;
    break;
  case 2:
    GFL_BMPWIN_TransVramCharacter( wk->lvupWin );
    (*seq)++;
    break;
  case 3:
//    GFL_BG_SetPriority(
    GFL_BG_SetScroll( BGFRAME_LVUPWIN, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScroll( BGFRAME_LVUPWIN, GFL_BG_SCROLL_Y_SET, 256 );
    PMSND_PlaySE( LVUPWIN_STEP_SE );
    (*seq)++;
    break;
  case 4:
    return TRUE;
  }

  return FALSE;
}
/**
 *  ２ページ目表示処理（レベルアップによるパラメータ）
 */
static BOOL lvupWinProc_Fwd( int* seq, void* wk_adrs )
{
  BTLV_SCU* wk = wk_adrs;

  switch( *seq ){
  case 0:
    {
      BPP_GetPPStatus( wk->lvupBpp, &wk->lvupInfo );

      BTL_STR_MakeString_LvupInfo_Param( wk->strBufMain,
        wk->lvupInfo.hp, wk->lvupInfo.atk, wk->lvupInfo.def,
        wk->lvupInfo.sp_atk, wk->lvupInfo.sp_def, wk->lvupInfo.agi );
      GFL_BMP_Clear( wk->lvupBmp, COLIDX_LVUPWIN_CLEAR );
      PRINTSYS_Print( wk->lvupBmp, 0, 0, wk->strBufMain, wk->defaultFont );
      (*seq)++;
    }
    break;
  case 1:
    GFL_BMPWIN_TransVramCharacter( wk->lvupWin );
    PMSND_PlaySE( LVUPWIN_STEP_SE );
    (*seq)++;
    break;
  case 2:
    return TRUE;
  }

  return FALSE;
}
/**
 *  消去処理
 */
static BOOL lvupWinProc_Hide( int* seq, void* wk_adrs )
{
  BTLV_SCU* wk = wk_adrs;

  switch( *seq ){
  case 0:
    {
      int win_ox = GFL_BMPWIN_GetPosX( wk->lvupWin ) - 1;
      int win_oy = GFL_BMPWIN_GetPosY( wk->lvupWin ) - 1;
      int win_width  = GFL_BMPWIN_GetSizeX( wk->lvupWin ) + 2;
      int win_height = GFL_BMPWIN_GetSizeY( wk->lvupWin ) + 2;
      u8  win_frame = GFL_BMPWIN_GetFrame( wk->lvupWin );

      GFL_BG_FillScreen( win_frame, 0, win_ox, win_oy, win_width, win_height, 0 );
      GFL_BG_LoadScreenReq( win_frame );
      PMSND_PlaySE( LVUPWIN_STEP_SE );
      (*seq)++;
    }
    break;
  case 1:
    GFL_BG_SetScroll( BGFRAME_LVUPWIN, GFL_BG_SCROLL_X_SET, TOKWIN_HIDEPOS_ENEMY );
    GFL_BG_SetScroll( BGFRAME_LVUPWIN, GFL_BG_SCROLL_Y_SET, TOKWIN_SCRN_YPOS );
    (*seq)++;
    break;
  case 2:
    return TRUE;
  }

  return FALSE;
}




