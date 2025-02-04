//=============================================================================================
/**
 * @file  btlv_core.c
 * @brief ポケモンWB バトル描画メインモジュール
 * @author  taya
 *
 * @date  2008.10.02  作成
 */
//=============================================================================================
#include <tcbl.h>

#include "print/gf_font.h"
#include "waza_tool/wazano_def.h"   //soga
#include "tr_tool/tr_tool.h"   //soga
#include "arc_def.h"
#include "sound/pm_sndsys.h"
#include "font/font.naix"


#include "../btl_common.h"
#include "../btl_main.h"
#include "../btl_action.h"
#include "../btl_calc.h"
#include "../btl_util.h"
#include "../btl_tables.h"
#include "../app/b_bag.h"
#include "../app/b_plist.h"

#include "btlv_scu.h"
#include "btlv_scd.h"

#include "btlv_core.h"


//バトル描画とバトルバッグとバトルポケモンリストをオーバーレイ化
FS_EXTERN_OVERLAY(battle_view);
FS_EXTERN_OVERLAY(battle_b_app);
FS_EXTERN_OVERLAY(battle_bag);
FS_EXTERN_OVERLAY(battle_plist);
FS_EXTERN_OVERLAY(vram_h);
FS_EXTERN_OVERLAY(tr_ai);

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  STR_BUFFER_SIZE = 384,
  GENERIC_WORK_SIZE = 128,
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef BOOL (*pCmdProc)( BTLV_CORE*, int*, void* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/
struct _BTLV_CORE {

  BTL_MAIN_MODULE*          mainModule;
  const BTL_CLIENT*         myClient;
  const BTL_POKE_CONTAINER* pokeCon;
  u8                        myClientID;

  BtlvCmd   processingCmd;
  pCmdProc  mainProc;
  int       mainSeq;
  BTL_PROC  subProc;
  u8        genericWork[ GENERIC_WORK_SIZE ];

  STRBUF*           strBuf;
  GFL_FONT*         largeFontHandle;
  GFL_FONT*         smallFontHandle;
  BTL_ACTION_PARAM* actionParam;
  const BTL_POKEPARAM*  procPokeParam;
  u32                   procPokeID;
  BtlAction             playerAction;
  BBAG_DATA             bagData;
  BPLIST_DATA           plistData;
  BTL_POKESELECT_RESULT* pokeselResult;
  BTLV_INPUT_BATTLE_RECORDER_PARAM  recPlayerUI;
  BTLV_ROTATION_WAZASEL_PARAM*      rotationSelParam;
  u8                    selectItemSeq;
  u8                    fActionPrevButton;


  GFL_TCBLSYS*  tcbl;
  BTLV_SCU*     scrnU;
  BTLV_SCD*     scrnD;

  HEAPID  heapID;
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void InitSystemCore( HEAPID heapID );
static void QuitSystemCore( void );
static void* getGenericWork( BTLV_CORE* core, u32 size );
static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer );
static BOOL CmdProc_SetupDemo( BTLV_CORE* core, int* seq, void* workBuffer );
static  const BTL_POKEPARAM*  get_btl_pokeparam( BTLV_CORE* core, BtlvMcssPos pos );
static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer );
static BOOL CmdProc_SelectTarget( BTLV_CORE* core, int* seq, void* workBufer );
static void mainproc_setup( BTLV_CORE* core, pCmdProc proc );
static void mainproc_reset( BTLV_CORE* core );
static BOOL mainproc_call( BTLV_CORE* core );
static BOOL CmdProc_SelectWaza( BTLV_CORE* core, int* seq, void* workBufer );
static void ForceQuitSelect_Common( BTLV_CORE* core );
static BOOL subprocDamageEffect( int* seq, void* wk_adrs );
static BOOL subprocMigawariDamageEffect( int* seq, void* wk_adrs );
static void PlayWazaAffSE( BTLV_CORE* wk, BtlTypeAffAbout affAbout );
static BOOL subprocMemberIn( int* seq, void* wk_adrs );
static void StrParamToString( const BTLV_STRPARAM* param, STRBUF* dst );
static void PutMsgToSCU( BTLV_CORE* wk, const STRBUF* buf, u16 wait, pPrintCallBack callBackFunc );
static void PutMsgToSCUatOnce( BTLV_CORE* wk, const STRBUF* buf );
static BOOL subprocMoveMember( int* seq, void* wk_adrs );
static BOOL subprocRotateMember( int* seq, void* wk_adrs );
static void PlaySELocal( BTLV_CORE* wk, u32 SENo );

static  const BTL_POKEPARAM*  get_btl_pokeparam( BTLV_CORE* core, BtlvMcssPos pos );







//=============================================================================================
/**
 * システム初期化
 *
 * @param   heapID
 */
//=============================================================================================
void BTLV_InitSystem( HEAPID heapID )
{
  InitSystemCore( heapID );

  //描画系オーバーレイロード
  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_view ) );

  GX_SetBankForLCDC( GX_VRAM_LCDC_H );
  GFL_OVERLAY_Load( FS_OVERLAY_ID( vram_h ) );

  //Wifi接続時はtr_aiをロードしない
  if( GFL_NET_IsWifiConnect() == FALSE )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID( tr_ai ) );
  }
}
//=============================================================================================
/**
 * システム終了
 */
//=============================================================================================
void BTLV_QuitSystem( void )
{
  QuitSystemCore();

  //描画系オーバーレイアンロード
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_view ) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( vram_h ) );
  //Wifi接続時はtr_aiをアンロードしない
  if( GFL_NET_IsWifiConnect() == FALSE )
  {
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( tr_ai ) );
  }
}
//=============================================================================================
/**
 * システムリセット（チャプタスキップ時に使用）
 */
//=============================================================================================
void BTLV_ResetSystem( HEAPID heapID )
{
  QuitSystemCore();
  InitSystemCore( heapID );
}

static void InitSystemCore( HEAPID heapID )
{
  static const GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_D,           // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
    GX_VRAM_SUB_BG_128_C,       // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // サブ2DエンジンのBG拡張パレット
    GX_VRAM_OBJ_64_E,           // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット
    GX_VRAM_SUB_OBJ_16_I,       // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// サブ2DエンジンのOBJ拡張パレット
    GX_VRAM_TEX_01_AB,          // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_01_FG,      // テクスチャパレットスロット
    GX_OBJVRAMMODE_CHAR_1D_64K, // メインOBJマッピングモード
    GX_OBJVRAMMODE_CHAR_1D_32K, // サブOBJマッピングモード
  };

  // BGsystem初期化
  GFL_BG_Init( heapID );
  GFL_BMPWIN_Init( heapID );
  GFL_FONTSYS_Init();

//  GFL_BG_DebugPrintCtrl( GFL_BG_SUB_DISP, TRUE );

  // VRAMバンク設定
  GFL_DISP_SetBank( &vramBank );

  // 各種効果レジスタ初期化
  G2_BlendNone();
  G2S_BlendNone();

  // 上下画面設定
  GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  //ＢＧモード設定
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }

  //3D関連初期化 soga
  {
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, heapID, NULL );
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
    G3X_AlphaBlend( TRUE );
//    G3X_AlphaTest( TRUE, 31 );    //alpha0でワイヤーフレームにならないようにする
    G3X_EdgeMarking( FALSE );
    G3X_AntiAlias( TRUE );
    G3X_SetFog( FALSE, 0, 0, 0 );
    G2_SetBG0Priority( 1 );
  }
  //ウインドマスク設定（画面両端のエッジマーキングのゴミを消す）soga
  {
#if 0
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 |
                 GX_WND_PLANEMASK_BG1 |
                 GX_WND_PLANEMASK_BG2 |
                 GX_WND_PLANEMASK_BG3 |
                 GX_WND_PLANEMASK_OBJ,
                 TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, TRUE );
    G2_SetWnd0Position( 1, 0, 255, 192 );
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
#endif
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, 3 );
  }

  {
    static  const GFL_CLSYS_INIT clsysinit = {
      0, 0,
      0, 512,
      4, 124,
      4, 124,
      0,
//      48,48,48,48,
      56,48,48,48,
      16, 16,
    };
    GFL_CLACT_SYS_Create( &clsysinit, &vramBank, heapID );
  }
}
static void QuitSystemCore( void )
{
  //エフェクト削除 soga
  BTLV_EFFECT_Exit();

  GFL_CLACT_SYS_Delete();

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  //3D関連削除 soga
  GFL_G3D_Exit();
}



//=============================================================================================
/**
 * 描画メインモジュールの生成
 *
 * @param   mainModule    システムメインモジュールのハンドラ
 * @param   heapID      生成先ヒープID
 *
 * @retval  BTLV_CORE*    生成された描画メインモジュールのハンドラ
 */
//=============================================================================================
BTLV_CORE*  BTLV_Create( BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, const BTL_POKE_CONTAINER* pokeCon, BtlBagMode bagMode, HEAPID heapID )
{
  BTLV_CORE* core = GFL_HEAP_AllocMemory( heapID, sizeof(BTLV_CORE) );

  core->mainModule = mainModule;
  core->myClient = client;
  core->pokeCon = pokeCon;
  core->myClientID = BTL_CLIENT_GetClientID( client );
  core->processingCmd = BTLV_CMD_NULL;
  core->heapID = heapID;
  core->strBuf = GFL_STR_CreateBuffer( STR_BUFFER_SIZE, heapID );
  core->largeFontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
            GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
  core->smallFontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_small_batt_gftr,
            GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

//  BTL_Printf("フォント高さ=%d, %d\n",
//    GFL_FONT_GetLineHeight(core->largeFontHandle), GFL_FONT_GetLineHeight(core->smallFontHandle));

  core->tcbl = GFL_TCBL_Init( heapID, heapID, 64, 128 );

  core->scrnU = BTLV_SCU_Create( core, core->mainModule, pokeCon, core->tcbl,
          core->largeFontHandle, core->smallFontHandle, core->myClientID, heapID );

  core->scrnD = BTLV_SCD_Create( core, core->mainModule, pokeCon, core->tcbl,
          core->largeFontHandle, core->myClient, bagMode, heapID );

  core->mainProc = NULL;
  core->mainSeq = 0;
  core->selectItemSeq = 0;

  TAYA_Printf("描画クライアントID=%dです\n", core->myClientID );

  BTL_STR_InitSystem( mainModule, core->myClientID, pokeCon, heapID );

  return core;
}

//=============================================================================================
/**
 * 描画メインモジュールの破棄
 *
 * @param   core    描画メインモジュール
 *
 */
//=============================================================================================
void BTLV_Delete( BTLV_CORE* core )
{
  BTL_STR_QuitSystem();

  BTLV_SCD_Delete( core->scrnD );
  BTLV_SCU_Delete( core->scrnU );


  GFL_TCBL_Exit( core->tcbl );
  GFL_STR_DeleteBuffer( core->strBuf );
  GFL_FONT_Delete( core->largeFontHandle );
  GFL_FONT_Delete( core->smallFontHandle );
  GFL_HEAP_FreeMemory( core );
}

//=============================================================================================
/**
 * 描画メインループ
 *
 * @param   core
 *
 */
//=============================================================================================
void BTLV_CORE_Main( BTLV_CORE* core )
{
  GFL_TCBL_Main( core->tcbl );
  BTLV_EFFECT_Main();   //soga
}


//=============================================================================================
/**
  * 描画コマンド発行
 *
 * @param   core    描画メインモジュールのハンドラ
 * @param   client    関連クライアントモジュールハンドラ
 * @param   cmd     描画コマンド
 *
 */
//=============================================================================================
void BTLV_StartCommand( BTLV_CORE* core, BtlvCmd cmd )
{
  GF_ASSERT( core->processingCmd == BTLV_CMD_NULL );

  {
    static const struct {
      BtlvCmd   cmd;
      pCmdProc  proc;
    }procTbl[] = {
      { BTLV_CMD_SETUP,           CmdProc_Setup },
      { BTLV_CMD_SETUP_DEMO,      CmdProc_SetupDemo },
    };

    int i;

    for(i=0; i<NELEMS(procTbl); i++)
    {
      if( procTbl[i].cmd == cmd )
      {
        core->processingCmd = cmd;
        core->mainProc = procTbl[i].proc;
        core->mainSeq = 0;
        return;
      }
    }

    GF_ASSERT(0);
  }
}

//=============================================================================================
/**
 * BTLV_StartCommand で受け付けた描画コマンドの終了を待つ
 *
 * @param   core      描画メインモジュールのハンドラ
 *
 * @retval  BOOL    終了していたらTRUE
 */
//=============================================================================================
BOOL BTLV_WaitCommand( BTLV_CORE* core )
{
  if( core->processingCmd != BTLV_CMD_NULL )
  {
    if( core->mainProc(core, &core->mainSeq, core->genericWork) )
    {
      core->processingCmd = BTLV_CMD_NULL;
      core->mainProc = NULL;
      return TRUE;
    }
    return FALSE;
  }
  return TRUE;
}


static void* getGenericWork( BTLV_CORE* core, u32 size )
{
  GF_ASSERT(size<sizeof(core->genericWork));

  return core->genericWork;
}


//----------------------------------------------------------------------------------
/**
 * バトル画面構築用VRAM設定〜双方のポケモン登場まで
 *
 * @param   core
 * @param   seq
 * @param   workBuffer
 *
 * @retval  BOOL        終了時TRUE
 */
//----------------------------------------------------------------------------------
static BOOL CmdProc_Setup( BTLV_CORE* core, int* seq, void* workBuffer )
{
  switch( *seq ){
  case 0:
    {
      BTLV_EFFECT_SETUP_PARAM* besp = BTLV_EFFECT_MakeSetUpParamBtl( core->mainModule, core->scrnU, core->heapID );
      BTLV_EFFECT_Init( besp, core->smallFontHandle, core->heapID );
      GFL_HEAP_FreeMemory( besp );
    }
    BTLV_SCU_Setup( core->scrnU );
    BTLV_SCD_Init( core->scrnD );
    (*seq)++;
    break;

  case 1:
    {
      BOOL fShortMode = BTL_CLIENT_IsChapterSkipMode( core->myClient );
      BTLV_SCU_StartBtlIn( core->scrnU, fShortMode );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_SCU_WaitBtlIn( core->scrnU ) )
    {
      if( BTL_CLIENT_IsRecPlayerMode(core->myClient) )
      {
        //チャプター周りを初期化
        core->recPlayerUI.play_chapter  = 1;
        core->recPlayerUI.view_chapter  = 1;
        core->recPlayerUI.max_chapter   = BTL_CLIENT_GetRecPlayerMaxChapter( core->myClient );
        core->recPlayerUI.stop_flag     = BTLV_INPUT_BR_STOP_NONE;
        //スキップモードのときは下画面再構成を呼ばない
        if( !BTL_CLIENT_IsChapterSkipMode(core->myClient) )
        {
          BTLV_SCD_SetupRecPlayerMode( core->scrnD, &core->recPlayerUI );
        }
#if 0
        core->recPlayerUI.play_chapter = 1;
        core->recPlayerUI.view_chapter = 1;
        core->recPlayerUI.max_chapter = BTL_CLIENT_GetRecPlayerMaxChapter( core->myClient );
        core->recPlayerUI.stop_flag = BTL_CLIENT_IsChapterSkipMode(core->myClient)?
                              BTLV_INPUT_BR_STOP_KEY : BTLV_INPUT_BR_STOP_NONE;

        BTLV_SCD_SetupRecPlayerMode( core->scrnD, &core->recPlayerUI );
#endif
      }
      return TRUE;
    }
    break;
  }

  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * 捕獲デモ
 */
//----------------------------------------------------------------------------------
#include "message.naix"
#include "msg/msg_capturedemo.h"
typedef struct
{
  GFL_MSGDATA*  msg;
  int           wait;
}CAPTURE_DEMO_WORK;

enum
{
  CAPTURE_DEMO_MSG_WAIT = 60,
};

//----------------------------------------------------------------------------------
/**
 * バトル画面構築用VRAM設定〜捕獲デモ〜
 *
 * @param   core
 * @param   seq
 * @param   workBuffer
 *
 * @retval  BOOL        終了時TRUE
 */
//----------------------------------------------------------------------------------
static BOOL CmdProc_SetupDemo( BTLV_CORE* core, int* seq, void* workBuffer )
{
  CAPTURE_DEMO_WORK*  cdw = ( CAPTURE_DEMO_WORK* )workBuffer;
  switch( *seq ){
  case 0:
    {
      BTLV_EFFECT_SETUP_PARAM* besp = BTLV_EFFECT_MakeSetUpParamBtl( core->mainModule, core->scrnU, core->heapID );
      BTLV_EFFECT_Init( besp, core->smallFontHandle, core->heapID );
      GFL_HEAP_FreeMemory( besp );
    }
    BTLV_SCU_Setup( core->scrnU );
    BTLV_SCD_Init( core->scrnD );
    cdw->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_capturedemo_dat, core->heapID );
    cdw->wait = 0;
    (*seq)++;
    break;

  case 1:
    BTLV_SCU_StartBtlIn( core->scrnU, FALSE );
    (*seq)++;
    break;

  case 2:
    if( BTLV_SCU_WaitBtlIn( core->scrnU ) )
    {
      BTLV_STRPARAM sp;
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_AA );
      // 「○○はどうする？」表示
      BTLV_STRPARAM_Setup( &sp, BTL_STRTYPE_STD, BTL_STRID_STD_SelectAction );
      BTLV_STRPARAM_AddArg( &sp, BPP_GetID( pp ) );
      BTLV_STRPARAM_SetWait( &sp, 0 );
      BTLV_PrintMsgAtOnce( core, &sp );
//      BTLV_StartMsg( core, &sp );
      (*seq)++;
    }
    break;
  case 3:
    core->procPokeParam = get_btl_pokeparam( core, BTLV_MCSS_POS_AA );
    core->procPokeID = BPP_GetID( core->procPokeParam );
    core->actionParam = NULL;
    core->playerAction = BTL_ACTION_NULL;
    core->fActionPrevButton = FALSE;
    BTLV_SCD_StartActionSelectDemoCapture( core->scrnD, core->procPokeParam, core->fActionPrevButton, core->actionParam );
    (*seq)++;
    break;
  case 4:
    //下画面演出終了待ち
    if( BTLV_SCD_WaitActionSelect( core->scrnD ) != BTL_ACTION_NULL )
    {
      BTLV_SCD_StartWazaSelectDemoCapture( core->scrnD, core->procPokeParam, core->actionParam );
      (*seq)++;
    }
    break;
  case 5:
    //下画面演出終了待ち
    if( BTLV_SCD_WaitActionSelect( core->scrnD ) != BTL_ACTION_NULL )
    {
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_AA );
      BTLV_StartMsgWaza( core, BPP_GetID( pp ), WAZANO_HATAKU );
      BTLV_SCD_RestartUI( core->scrnD );
      (*seq)++;
    }
    break;
  case 6:
    if( BTLV_WaitMsg(core) )
    {
      if( BTLV_SCD_WaitRestartUI(core->scrnD) )
      {
        WazaID  waza = WAZANO_HATAKU;
        WazaTarget range = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );

        BTLV_SCU_StartWazaEffect( core->scrnU, BTLV_MCSS_POS_AA, BTLV_MCSS_POS_BB,
            waza, range,  BTLV_WAZAEFF_INDEX_DEFAULT, 0 );
        (*seq)++;
      }
    }
    break;
  case 7:
    if( BTLV_SCU_WaitWazaEffect( core->scrnU ) )
    {
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_BB );
      int value = BPP_GetValue( pp, BPP_MAX_HP ) / 6;

      BTLV_EFFECT_CalcGaugeHP( BTLV_MCSS_POS_BB, value );
      BTLV_EFFECT_Damage( BTLV_MCSS_POS_BB, WAZANO_HATAKU );
      PlaySELocal( core, SEQ_SE_KOUKA_M );
      (*seq)++;
    }
    break;
  case 8:
    if( BTLV_SCU_WaitWazaDamageAct( core->scrnU ) )
    {
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_BB );
      BTLV_StartMsgWaza( core, BPP_GetID( pp ), WAZANO_NIRAMITUKERU );
      (*seq)++;
    }
    break;
  case 9:
    if( BTLV_WaitMsg( core ) )
    {
      WazaID  waza = WAZANO_NIRAMITUKERU;
      WazaTarget range = WAZADATA_GetParam( waza, WAZAPARAM_TARGET );

      BTLV_SCU_StartWazaEffect( core->scrnU, BTLV_MCSS_POS_BB, BTLV_MCSS_POS_AA,
                                  waza, range,  BTLV_WAZAEFF_INDEX_DEFAULT, 0 );
      (*seq)++;
    }
    break;
  case 10:
    if( BTLV_SCU_WaitWazaEffect( core->scrnU ) )
    {
      BTLV_StartRankDownEffect( core, BTLV_MCSS_POS_AA );
      (*seq)++;
    }
    break;
  case 11:
    if( BTLV_WaitRankEffect( core, BTLV_MCSS_POS_AA ) )
    {
      BTLV_STRPARAM sp;
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_AA );
      BTLV_STRPARAM_Setup( &sp, BTL_STRTYPE_SET, BTL_STRID_SET_Rankdown_DEF );
      BTLV_STRPARAM_AddArg( &sp, BPP_GetID( pp ) );
      BTLV_STRPARAM_SetWait( &sp, 0 );
      BTLV_StartMsg( core, &sp );
      (*seq)++;
    }
    break;
  case 12:
    if( BTLV_WaitMsg( core ) )
    {
      //たいりょくがへったでしょメッセージ
      GFL_MSG_GetString( cdw->msg, msg_cappture_demo_01, core->strBuf );
      PutMsgToSCU( core, core->strBuf, 0, NULL );
      cdw->wait = CAPTURE_DEMO_MSG_WAIT;
      (*seq)++;
    }
    break;
  case 13:
    if( BTLV_WaitMsg( core ) )
    {
      if( --cdw->wait == 0 )
      {
        BTLV_SCD_StartActionSelectDemoCapture( core->scrnD, core->procPokeParam, core->fActionPrevButton, core->actionParam );
        (*seq)++;
      }
    }
    break;
  case 14:
    //下画面演出終了待ち
    if( BTLV_SCD_WaitActionSelect( core->scrnD ) != BTL_ACTION_NULL )
    {
      BTLV_ITEMSELECT_Start( core, BBAG_MODE_GETDEMO, 0, 0, TRUE, FALSE );
      (*seq)++;
    }
    break;
  case 15:
    if( BTLV_ITEMSELECT_Wait( core ) )
    {
      //ボールなげメッセージ
      GFL_MSG_GetString( cdw->msg, msg_cappture_demo_02, core->strBuf );
      PutMsgToSCU( core, core->strBuf, 0, NULL );
      BTLV_SCD_RestartUI( core->scrnD );
      (*seq)++;
    }
    break;
  case 16:
    if( BTLV_WaitMsg( core ) )
    {
      BTLV_EFFECT_BallThrow( BTLV_MCSS_POS_BB, ITEM_MONSUTAABOORU, 3, TRUE, FALSE );
      (*seq)++;
    }
    break;
  case 17:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_STRPARAM sp;
      const BTL_POKEPARAM*  pp  = get_btl_pokeparam( core, BTLV_MCSS_POS_BB );
      BTLV_STRPARAM_Setup( &sp, BTL_STRTYPE_STD, BTL_STRID_STD_BallThrowS );
      BTLV_STRPARAM_AddArg( &sp, BPP_GetID( pp ) );
      BTLV_StartMsg( core, &sp );
      (*seq)++;
    }
    break;
  case 18:
    if( BTLV_IsJustDoneMsg( core ) )
    {
      PMSND_PlayBGM( SEQ_ME_POKEGET );
    }
    if( BTLV_WaitMsg( core ) )
    {
      (*seq)++;
    }
    break;
  case 19:
    if( !PMSND_CheckPlayBGM() )
    {
      //戦闘エフェクト一時停止を解除
      BTLV_EFFECT_Restart();
      GFL_MSG_Delete( cdw->msg );
      (*seq)++;
    }
    break;
  case 20:
    return TRUE;
    break;
  }

  return FALSE;
}

static  const BTL_POKEPARAM*  get_btl_pokeparam( BTLV_CORE* core, BtlvMcssPos pos )
{
  BtlPokePos  pokePos = BTL_MAIN_ViewPosToBtlPos( core->mainModule, pos );
  return  BTL_POKECON_GetFrontPokeDataConst( core->pokeCon, pokePos );
}


static BOOL CmdProc_SelectAction( BTLV_CORE* core, int* seq, void* workBufer )
{
  switch( *seq ){
  case 0:
    BTLV_SCD_StartActionSelect( core->scrnD, core->procPokeParam,
        core->fActionPrevButton, BTL_CLIENT_GetShooterEnergy(core->myClient), core->actionParam );
    (*seq)++;
  case 1:
    core->playerAction = BTLV_SCD_WaitActionSelect( core->scrnD );
    if( core->playerAction != BTL_ACTION_NULL ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------
/**
 * ワザ対象選択
 *
 * @param   core
 * @param   seq
 * @param   workBufer
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL CmdProc_SelectTarget( BTLV_CORE* core, int* seq, void* workBufer )
{
  switch( *seq ){
  case 0:
    BTLV_SCD_StartTargetSelect( core->scrnD, core->procPokeParam, core->actionParam );
    (*seq)++;
    break;
  case 1:
    if( BTLV_SCD_WaitTargetSelect(core->scrnD) )
    {
      return TRUE;
    }
    break;
  }
  return FALSE;
}

//--------------------------------------------------------------------------
/**
 * メインプロセス設定
 *
 * @param   core
 * @param   proc
 *
 */
//--------------------------------------------------------------------------
static void mainproc_setup( BTLV_CORE* core, pCmdProc proc )
{
  GF_ASSERT(core);
  GF_ASSERT_MSG( core->mainProc == NULL, "mainProc=%p\n", core->mainProc );

  core->mainProc = proc;
  core->mainSeq = 0;
}
//----------------------------------------------------------------------------------
/**
 * メインプロセス初期化
 *
 * @param   core
 */
//----------------------------------------------------------------------------------
static void mainproc_reset( BTLV_CORE* core )
{
  core->mainProc = NULL;
  core->mainSeq = 0;
}
//--------------------------------------------------------------------------
/**
 * メインプロセスコール
 *
 * @param   core
 *
 * @retval  BOOL
 */
//--------------------------------------------------------------------------
static BOOL mainproc_call( BTLV_CORE* core )
{
  if( core->mainProc != NULL )
  {
    if( core->mainProc(core, &core->mainSeq, core->genericWork) )
    {
      core->mainProc = NULL;
      return TRUE;
    }
    return FALSE;
  }
  return TRUE;
}


//=============================================================================================
/**
 * アクション選択開始
 *
 * @param   core
 * @param   bpp           対象ポケモンデータ
 * @param   fPrevButton   TRUEなら「もどる」表示／FALSEなら「にげる」表示
 * @param   dest          [out] 行動結果パラメータ
 */
//=============================================================================================
void BTLV_UI_SelectAction_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BOOL fPrevButton, BTL_ACTION_PARAM* dest )
{
  core->procPokeParam = bpp;
  core->procPokeID = BPP_GetID( bpp );
  core->actionParam = dest;
  core->playerAction = BTL_ACTION_NULL;
  core->fActionPrevButton = fPrevButton;
  mainproc_setup( core, CmdProc_SelectAction );
}
//=============================================================================================
/**
 * アクション選択の強制終了通知（コマンド選択の制限時間）
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_UI_SelectAction_ForceQuit( BTLV_CORE* wk )
{
  ForceQuitSelect_Common( wk );
}
//=============================================================================================
/**
 * アクション選択終了待ち
 *
 * @param   core
 *
 * @retval  BtlAction   選択アクション（選択されていなければ BTL_ACTION_NULL ）
 */
//=============================================================================================
BtlAction BTLV_UI_SelectAction_Wait( BTLV_CORE* core )
{
  if( mainproc_call( core ) ){
    return core->playerAction;
  }
  return BTL_ACTION_NULL;
}
//=============================================================================================
/**
 * ワザ選択開始（通常=ローテーション以外）
 *
 * @param   core
 * @param   bpp
 * @param   dest
 */
//=============================================================================================
void BTLV_UI_SelectWaza_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  core->procPokeParam = bpp;
  core->procPokeID = BPP_GetID( bpp );
  core->actionParam = dest;

  BTLV_SCD_StartWazaSelect( core->scrnD, core->procPokeParam, core->actionParam );

  mainproc_setup( core, CmdProc_SelectWaza );
}
//=============================================================================================
/**
 * ワザ選択開始（ローテーションバトル専用）
 *
 * @param   core
 * @param   selParam
 * @param   dest
 */
//=============================================================================================
void  BTLV_UI_SelectRotationWaza_Start( BTLV_CORE* core, BTLV_ROTATION_WAZASEL_PARAM* selParam, BTL_ACTION_PARAM* dest )
{
  core->procPokeParam = selParam->poke[0].bpp;
  core->procPokeID = BPP_GetID( core->procPokeParam );
  core->rotationSelParam = selParam;
  core->actionParam = dest;

  BTLV_SCD_StartRotationWazaSelect( core->scrnD, core->rotationSelParam, core->actionParam );

  mainproc_setup( core, CmdProc_SelectWaza );
}
//=============================================================================================
/**
 * ワザ選択画面を保持したまま再開（通常／ローテーション共通）
 *
 * @param   core
 */
//=============================================================================================
void BTLV_UI_SelectWaza_Restart( BTLV_CORE* core )
{
  BTLV_SCD_RestartWazaSelect( core->scrnD );
  mainproc_setup( core, CmdProc_SelectWaza );
}
/**
 *  UI ワザ選択呼び出し（通常／ローテーション共通）
 */
static BOOL CmdProc_SelectWaza( BTLV_CORE* core, int* seq, void* workBufer )
{
  if( BTLV_SCD_WaitWazaSelect( core->scrnD ) ){
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
/**
 * ワザ選択の強制終了通知（コマンド選択の制限時間）
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_UI_SelectWaza_ForceQuit( BTLV_CORE* wk )
{
  ForceQuitSelect_Common( wk );
}
//=============================================================================================
/**
 * ワザ選択終了待ち
 *
 * @param   core
 *
 * @retval  BOOL    終了していたらTRUE
 */
//=============================================================================================
BOOL BTLV_UI_SelectWaza_Wait( BTLV_CORE* core )
{
  return mainproc_call( core );
}
//=============================================================================================
/**
 * ワザ対象選択開始
 *
 * @param   core
 * @param   bpp
 * @param   dest
 */
//=============================================================================================
void BTLV_UI_SelectTarget_Start( BTLV_CORE* core, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  core->procPokeParam = bpp;
  core->procPokeID = BPP_GetID( bpp );
  core->actionParam = dest;
  mainproc_setup( core, CmdProc_SelectTarget );
}
//=============================================================================================
/**
 * ワザ対象選択終了待ち
 *
 * @param   core
 *
 * @retval  BOOL    終了していたらTRUE
 */
//=============================================================================================
BtlvResult BTLV_UI_SelectTarget_Wait( BTLV_CORE* core )
{
  if( mainproc_call(core) ){
    if( BTLV_SCD_IsSelTargetDone(core->scrnD) ){
      return BTLV_RESULT_DONE;
    }
    return BTLV_RESULT_CANCEL;
  }
  return BTLV_RESULT_NONE;
}
//=============================================================================================
/**
 * ワザ対象の強制終了通知（コマンド選択の制限時間）
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_UI_SelectTarget_ForceQuit( BTLV_CORE* core )
{
  ForceQuitSelect_Common( core );
}

/**
 *  アクション・ワザ・対象の強制終了（コマンド選択時間制限） 共通
 */
static void ForceQuitSelect_Common( BTLV_CORE* core )
{
  //カメラワークチェックを止めてカメラをデフォルト位置にする
  BTLV_EFFECT_SetCameraWorkStop();
  BTLV_SCD_ForceQuitSelect( core->scrnD );
  mainproc_reset( core );
}


void BTLV_UI_RestartIfNotStandBy( BTLV_CORE* core )
{
  BTLV_SCD_RestartUI_IfNotStandBy( core->scrnD );
}
void BTLV_UI_Restart( BTLV_CORE* core )
{
  BTLV_SCD_RestartUI( core->scrnD );
}
BOOL BTLV_UI_WaitRestart( BTLV_CORE* core )
{
  return BTLV_SCD_WaitRestartUI( core->scrnD );
}

/**
 *  ポケモンリストパラメータ設定共通処理
 */
static void SetupPlistDataCommon( BTLV_CORE* wk, BPLIST_DATA* plist, u8 bplMode, u8 pos_index, u32 chg_waza_param )
{
  u8 clientID = BTL_CLIENT_GetClientID( wk->myClient );

  plist->pp = BTL_MAIN_GetClientSrcParty( wk->mainModule, clientID );

  plist->multiMode = (bplMode != BPL_MODE_ITEMUSE)?
                BTL_MAIN_IsPokeListMultiMode( wk->mainModule ) : FALSE;

  if( plist->multiMode ){
    plist->multi_pp = BTL_MAIN_GetClientMultiSrcParty( wk->mainModule, clientID );
    plist->multiPos = BTL_MAIN_GetClientMultiPos( wk->mainModule, clientID );
  }else{
    plist->multi_pp = NULL;
    plist->multiPos = 0;
  }
  plist->mode = bplMode;

  if( BTL_MAIN_GetRule(wk->mainModule) != BTL_RULE_ROTATION ){
    plist->fight_poke_max = BTL_MAIN_GetClientFrontPosCount( wk->mainModule, clientID );
  }else{
    plist->fight_poke_max = BTL_ROTATION_FRONTPOS_NUM + BTL_ROTATION_BACKPOS_NUM;
  }

  plist->sel_poke = ( (bplMode == BPL_MODE_WAZASET) || (bplMode==BPL_MODE_WAZAINFO) )? pos_index : 0;
  plist->sel_pos_index = pos_index;
  plist->chg_waza = chg_waza_param;
  plist->sel_wp = 0;
  plist->heap = wk->heapID;
  plist->font = wk->largeFontHandle;
  plist->rule = BTL_MAIN_GetRule( wk->mainModule );
  plist->cursor_flg = BTLV_SCD_GetCursorFlagPtr( wk->scrnD );
  plist->tcb_sys = BTLV_EFFECT_GetTCBSYS();
  plist->pfd = BTLV_EFFECT_GetPfd();
  plist->gamedata = BTL_MAIN_GetGameData( wk->mainModule );
  plist->time_out_flg = FALSE;
  plist->seFlag = !(BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_COMM);
  plist->comm_err_flg = FALSE;
  plist->end_flg = FALSE;

}
//=============================================================================================
/**
 * コマンド選択トップから「ポケモン」選択->交換するポケモン選択開始
 *
 * @param   wk
 * @param   param
 * @param   outMemberIndex  リスト画面表示時、何番目を選択した状態で始めるか
 * @param   fCantEsc
 * @param   result
 */
//=============================================================================================
void BTLV_StartPokeSelect( BTLV_CORE* wk, const BTL_POKESELECT_PARAM* param, int outMemberIndex, BOOL fCantEsc, BTL_POKESELECT_RESULT* result )
{
  if( outMemberIndex < 0 ){
    outMemberIndex = 0;
  }
  SetupPlistDataCommon( wk, &wk->plistData, param->bplMode, outMemberIndex, fCantEsc );

  // 既に選択されているポケモンの位置情報を初期化
  {
    u32 i, selectedPokeCnt = BTL_POKESELECT_RESULT_GetCount( result );

    BTL_N_Printf( DBGSTR_VCORE_PokeListStart, wk->plistData.mode, selectedPokeCnt, outMemberIndex);
    for(i=0; i<selectedPokeCnt; ++i)
    {
      if( i >= NELEMS(wk->plistData.change_sel) ){ break; }
      wk->plistData.change_sel[i] = BTL_POKESELECT_RESULT_Get( result, i );
    }
    for( ; i<NELEMS(wk->plistData.change_sel); ++i){
      wk->plistData.change_sel[i] = BPL_CHANGE_SEL_NONE;
    }
  }

  result->fCancel = TRUE;
  wk->pokeselResult = result;
  wk->selectItemSeq = 0;
}
//=============================================================================================
/**
 * ポケモン選択の強制終了通知（制限時間切れによる）
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_ForceQuitPokeSelect( BTLV_CORE* wk )
{
  wk->plistData.time_out_flg = TRUE;
}
//=============================================================================================
/**
 * ポケモン選択画面の終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_WaitPokeSelect( BTLV_CORE* wk )
{
  switch( wk->selectItemSeq ){
  case 0:
/*
  BPL_MODE_NORMAL = 0,  // 通常のポケモン選択
  BPL_MODE_NO_CANCEL,   // キャンセル無効のポケモン選択
  BPL_MODE_CHG_DEAD,    // 瀕死入れ替え選択
  BPL_MODE_ITEMUSE,     // アイテム使用
  BPL_MODE_WAZASET,     // 技忘れ
  BPL_MODE_WAZAINFO,    // 技確認
*/
    if( wk->plistData.mode == BPL_MODE_CHG_DEAD || wk->plistData.mode == BPL_MODE_NO_CANCEL ){
      BTLV_SCD_FadeOut( wk->scrnD, FALSE );
    }else{
      BTLV_SCD_FadeOut( wk->scrnD, TRUE );
    }
    GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_b_app ) );
    wk->selectItemSeq++;
    break;
  case 1:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      BTLV_SCD_Cleanup( wk->scrnD );
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_plist ) );
      BattlePokeList_TaskAdd( &wk->plistData );
      wk->selectItemSeq++;
    }
    break;
  case 2:
    if( wk->plistData.end_flg )
    {
      BTL_N_Printf( DBGSTR_VCORE_SelPokeEnd );

      // ワザせつめい（ショートカット）モードではない & 制限時間による強制終了でないなら、
      // 選択されたポケモンデータを格納
      if( (wk->plistData.mode != BPL_MODE_WAZAINFO)
      &&  (!(wk->plistData.time_out_flg))
      ){
        u32 i;
        for(i=0; i<NELEMS(wk->plistData.sel_pos); ++i)
        {
          if( wk->plistData.sel_pos[i] != BPL_SELPOS_NONE )
          {
            BTL_POKESELECT_RESULT_Push( wk->pokeselResult, i, wk->plistData.sel_pos[i] );
            {
              u8 storeCnt = BTL_POKESELECT_RESULT_GetCount( wk->pokeselResult );
              BTL_N_Printf( DBGSTR_VCORE_SelPokeEnd_Sel, i, wk->plistData.sel_pos[i], storeCnt);
              wk->pokeselResult->fCancel = FALSE;
            }
          }
          else{
            BTL_N_Printf( DBGSTR_VCORE_SelPokeEnd_Unsel, i );
          }
        }
      }

      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_plist ) );
      BTLV_SCD_FadeIn( wk->scrnD );
      BTLV_SCD_Setup( wk->scrnD );
      wk->selectItemSeq++;
    }
    break;
  case 3:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      wk->selectItemSeq++;
    }
    break;
  default:
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_b_app ) );
    wk->selectItemSeq = 0;
    return TRUE;
  }
  return FALSE;
}
//=============================================================================================
/**
 * ワザ説明画面（ショートカット）呼び出し
 *
 * @param   wk
 * @param   pokeIdx
 * @param   wazaIdx
 */
//=============================================================================================
void BTLV_StartWazaInfoView( BTLV_CORE* wk, u8 pokeIdx, u8 wazaIdx )
{
  SetupPlistDataCommon( wk, &wk->plistData, BPL_MODE_WAZAINFO, pokeIdx, 0 );
  wk->plistData.sel_wp = wazaIdx;
  wk->selectItemSeq = 0;
}

//=============================================================================================
/**
 * アイテム選択開始
 *
 * @param   wk
 * @param   bagMode   バッグモード（通常／シューター）
 * @param   energy
 */
//=============================================================================================
void BTLV_ITEMSELECT_Start( BTLV_CORE* wk, u8 bagMode, u8 energy, u8 reserved_energy, u8 fFirstPokemon, u8 fBallTargetHide )
{
  if( wk->selectItemSeq == 0 )
  {
    wk->bagData.myitem = BTL_MAIN_GetItemDataPtr( wk->mainModule );
    wk->bagData.bagcursor = BTL_MAIN_GetBagCursorData( wk->mainModule );
    wk->bagData.mode = bagMode;
    wk->bagData.font = wk->largeFontHandle;
    wk->bagData.heap = wk->heapID;
    wk->bagData.energy = energy;
    wk->bagData.reserved_energy = reserved_energy;
    wk->bagData.ret_item = ITEM_DUMMY_DATA;
    wk->bagData.cursor_flg = BTLV_SCD_GetCursorFlagPtr( wk->scrnD );
    wk->bagData.time_out_flg = FALSE;
    wk->bagData.shooter_item_bit = BTL_MAIN_GetSetupShooterBit( wk->mainModule );
    wk->bagData.seFlag = !(BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_COMM);
    wk->bagData.comm_err_flg = FALSE;
    wk->bagData.wild_flg = (BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD);
    wk->bagData.end_flg = FALSE;

    {
      u8 ballUseMode = BBAG_BALLUSE_TRUE; // ボール投げ禁止モード（デフォルトは禁止しない）

      do{
        // 先頭のポケモン行動時しか投げられない
        if( !fFirstPokemon ){
          ballUseMode = BBAG_BALLUSE_NOT_FIRST;
          break;
        }
        // 手持ち・ボックスが満杯で投げられない
        if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_BOXFULL) ){
          ballUseMode = BBAG_BALLUSE_POKEMAX;
          break;
        }
        // 野生戦の場合…
        if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD )
        {
          // 場に２体以上いるなら投げられない
          if( BTL_POKECON_GetClientAlivePokeCount(wk->pokeCon, BTL_CLIENT_ENEMY1) > 1 ){
            ballUseMode = BBAG_BALLUSE_DOUBLE;
            break;
          }
          // 場のポケモンが（そらをとぶなどで）消えてるなら投げられない
          if( fBallTargetHide ){
            ballUseMode = BBAG_BALLUSE_POKE_NONE;
            break;
          }
        }

        // 上記条件に合致しなければ投げる（ただしトレーナー戦ならボールをはじかれて終わり）

      }while(0);

      wk->bagData.ball_use = ballUseMode;
    }

  // ポケモンリストパラメータ初期化
    SetupPlistDataCommon( wk, &wk->plistData, BPL_MODE_ITEMUSE, 0, 0 );

  //「さしおさえ」で道具を使用できないポケモンを指定
    {
      const BTL_PARTY* party = BTL_CLIENT_GetParty( wk->myClient );
      const BTL_POKEPARAM* bpp;
      u32 i, members = BTL_PARTY_GetMemberCount( party );

      for(i=0; i<members; ++i){
        bpp = BTL_PARTY_GetMemberDataConst( party, i );
        wk->plistData.skill_item_use[ i ] = BPP_CheckSick( bpp, WAZASICK_SASIOSAE );
      }
      for( ; i<NELEMS(wk->plistData.skill_item_use); ++i){
        wk->plistData.skill_item_use[ i ] = 0;  // 0=使用可 / 1=使用不可
      }
    }

    wk->selectItemSeq = 1;
  }
}

//=============================================================================================
/**
 * アイテム選択の強制終了通知（コマンド選択の制限時間切れによる）
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_ITEMSELECT_ForceQuit( BTLV_CORE* wk )
{
  wk->bagData.time_out_flg = TRUE;
  wk->plistData.time_out_flg = TRUE;
}

//=============================================================================================
/**
 * アイテム選択終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL    終了したらTRUE
 */
//=============================================================================================
BOOL BTLV_ITEMSELECT_Wait( BTLV_CORE* wk )
{
  enum {
    SEQ_NULL = 0,
    SEQ_INIT = 1,
    SEQ_START_SCENE_BAG,
    SEQ_WAIT_SCENE_BAG,
    SEQ_START_SCENE_POKELIST,
    SEQ_WAIT_SCENE_POKELIST,

    SEQ_START_RECOVER,
    SEQ_WAIT_RECOVER,
  };

  switch( wk->selectItemSeq ){
  case SEQ_INIT:
    BTLV_SCD_FadeOut( wk->scrnD, TRUE );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_b_app ) );
    wk->selectItemSeq = SEQ_START_SCENE_BAG;
    break;

  case SEQ_START_SCENE_BAG:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      BTLV_SCD_Cleanup( wk->scrnD );
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_bag ) );
      BattleBag_TaskAdd( &wk->bagData );
      wk->selectItemSeq = SEQ_WAIT_SCENE_BAG;
    }
    break;

  case SEQ_WAIT_SCENE_BAG:
    if( wk->bagData.end_flg ){
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_bag ) );

      if( !(wk->bagData.time_out_flg) ){
        wk->selectItemSeq = SEQ_START_SCENE_POKELIST;
      }else{
        wk->selectItemSeq = SEQ_START_RECOVER;
      }
    }
    break;

 case SEQ_START_SCENE_POKELIST:
    if( (wk->bagData.ret_item != ITEM_DUMMY_DATA)
    &&  (wk->bagData.ret_page != BBAG_POKE_BALL)
    &&  (!BTL_TABLES_IsNoTargetItem(wk->bagData.ret_item))
    ){
      wk->plistData.item = wk->bagData.ret_item;
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_plist ) );
      BattlePokeList_TaskAdd( &wk->plistData );
      wk->selectItemSeq = SEQ_WAIT_SCENE_POKELIST;
    }else{
      wk->selectItemSeq = SEQ_START_RECOVER;
    }
    break;

  case SEQ_WAIT_SCENE_POKELIST:
    if( wk->plistData.end_flg ){
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_plist ) );
      wk->selectItemSeq = SEQ_START_RECOVER;
    }
    break;

  case SEQ_START_RECOVER:
    BTLV_SCD_FadeIn( wk->scrnD );
    BTLV_SCD_Setup( wk->scrnD );
    wk->selectItemSeq = SEQ_WAIT_RECOVER;
    break;

  case SEQ_WAIT_RECOVER:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_b_app ) );
      wk->selectItemSeq = SEQ_NULL;
      return TRUE;
    }
  }
  return FALSE;
}
//=============================================================================================
/**
 * アイテム選択結果取得（使用アイテムID）
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u16 BTLV_ITEMSELECT_GetItemID( BTLV_CORE* wk )
{
  if( wk->bagData.end_flg ){
    return wk->bagData.ret_item;
  }
  return ITEM_DUMMY_DATA;
}
//=============================================================================================
/**
 * アイテム選択結果取得（使用アイテムのコスト：シューター時）
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTLV_ITEMSELECT_GetCost( BTLV_CORE* wk )
{
  if( wk->bagData.end_flg ){
    return wk->bagData.ret_cost;
  }
  return 0;
}
//=============================================================================================
/**
 * アイテム選択結果取得（対象インデックス）
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTLV_ITEMSELECT_GetTargetIdx( BTLV_CORE* wk )
{
  BTL_Printf(" Item Target Index = %d\n",wk->plistData.sel_poke );
  return wk->plistData.sel_poke;
}
//=============================================================================================
/**
 * アイテム選択結果取得（PP回復系アイテムなどのワザインデックス）
 *
 * @param   wk
 *
 * @retval  u8
 */
//=============================================================================================
u8 BTLV_ITEMSELECT_GetWazaIdx( BTLV_CORE* wk )
{
  return wk->plistData.sel_wp;
}

//=============================================================================================
/**
 * アイテム使用時、カーソルデータに反映する
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_ITEMSELECT_ReflectUsedItem( BTLV_CORE* wk )
{
  if( (wk->bagData.ret_item != ITEM_DUMMY_DATA)
  &&  (wk->plistData.sel_poke != BPL_SEL_EXIT)
  &&  (wk->bagData.mode != BBAG_MODE_SHOOTER)
  ){
    MYITEM_BattleBagCursorSet( wk->bagData.bagcursor, wk->bagData.item_pos, wk->bagData.item_scr );
    MYITEM_BattleBagLastItemSet( wk->bagData.bagcursor, wk->bagData.ret_item, wk->bagData.ret_page );
  }
}

//--------------------------------------

//=============================================================================================
/**
 * ワザエフェクト開始
 *
 * @param   core
 * @param   dst
 *
 */
//=============================================================================================
void BTLV_ACT_WazaEffect_Start( BTLV_CORE* wk, BtlPokePos atPokePos, BtlPokePos defPokePos,
  WazaID waza, WazaTarget wazaRange, BtlvWazaEffect_TurnType turnType, u8 continueCount )
{
  BtlvMcssPos  at_pos, def_pos;

  at_pos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, atPokePos );

  def_pos = (defPokePos != BTL_POS_NULL)?
      BTL_MAIN_BtlPosToViewPos( wk->mainModule, defPokePos ) : BTLV_MCSS_POS_ERROR;

  BTLV_SCU_StartWazaEffect( wk->scrnU, at_pos, def_pos, waza, wazaRange, turnType, continueCount );
}
//=============================================================================================
/**
 * ワザエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_ACT_WazaEffect_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitWazaEffect( wk->scrnU );
}

//--------------------------------------

typedef struct {
  WazaID           wazaID;
  BtlTypeAffAbout  affAbout;
  u16     timer;
  u8      defPokePos;
}WAZA_DMG_ACT_WORK;

//=============================================================================================
/**
 * 単体ワザダメージエフェクト開始
 *
 * @param   wk
 * @param   wazaID
 * @param   defPokePos
 * @param   damage
 * @param   aff
 */
//=============================================================================================
void BTLV_ACT_DamageEffectSingle_Start( BTLV_CORE* wk, WazaID wazaID, BtlPokePos defPokePos, BtlTypeAffAbout affAbout )
{
  WAZA_DMG_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_DMG_ACT_WORK));

  subwk->affAbout = affAbout;
  subwk->defPokePos = defPokePos;
  subwk->timer = 0;
  subwk->wazaID = wazaID;

  BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocDamageEffect );
}
//=============================================================================================
/**
 * 単体ワザダメージエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_ACT_DamageEffectSingle_Wait( BTLV_CORE* wk )
{
  return BTL_UTIL_CallProc( &wk->subProc );
}
static BOOL subprocDamageEffect( int* seq, void* wk_adrs )
{
  BTLV_CORE* wk = wk_adrs;
  WAZA_DMG_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_DMG_ACT_WORK));

  switch( *seq ){
  case 0:
    {
      BOOL fChapterSkipMode = BTL_CLIENT_IsChapterSkipMode( wk->myClient );

      BTLV_SCU_StartWazaDamageAct( wk->scrnU, subwk->defPokePos, subwk->wazaID, fChapterSkipMode );
      PlayWazaAffSE( wk, subwk->affAbout );

      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_SCU_WaitWazaDamageAct(wk->scrnU) )
    {
      return TRUE;
    }
    break;

  }
  return FALSE;
}

//=============================================================================================
/**
 * みがわりへのダメージエフェクト開始
 *
 * @param   wk
 * @param   wazaID
 * @param   defPokePos
 * @param   damage
 * @param   aff
 */
//=============================================================================================
void BTLV_ACT_MigawariDamageEffect_Start( BTLV_CORE* wk, WazaID wazaID, BtlPokePos migawariPos, BtlTypeAffAbout affAbout )
{
  if( !BTL_CLIENT_IsChapterSkipMode(wk->myClient) )
  {
    WAZA_DMG_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_DMG_ACT_WORK));

    subwk->affAbout = affAbout;
    subwk->defPokePos = migawariPos;
    subwk->timer = 0;
    subwk->wazaID = wazaID;

    BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocMigawariDamageEffect );
  }
}
//=============================================================================================
/**
 * みがわりへのダメージエフェクト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL    終了したらTRUE
 */
//=============================================================================================
BOOL BTLV_ACT_MigawariDamageEffect_Wait( BTLV_CORE* wk )
{
  return BTL_UTIL_CallProc( &wk->subProc );
}
/**
 *  みがわりへのダメージエフェクトタスク
 */
static BOOL subprocMigawariDamageEffect( int* seq, void* wk_adrs )
{
  BTLV_CORE* wk = wk_adrs;
  WAZA_DMG_ACT_WORK* subwk = getGenericWork(wk, sizeof(WAZA_DMG_ACT_WORK));

  switch( *seq ){
  case 0:
    {
      BTLV_SCU_StartMigawariDamageAct( wk->scrnU, subwk->defPokePos, subwk->wazaID );
      PlayWazaAffSE( wk, subwk->affAbout );
      (*seq)++;
    }
    break;

  case 1:
    if( BTLV_SCU_WaitMigawariDamageAct(wk->scrnU) )
    {
      return TRUE;
    }
    break;

  }
  return FALSE;
}

//--------------------------------------
typedef struct {
  u8  pokePos[ BTL_POS_MAX ];
  u8  pokeCnt;
  u16 seq;
  BtlTypeAffAbout  affAbout;
  WazaID           wazaID;
}DMG_PLURAL_ACT_WORK;


void BTLV_ACT_DamageEffectPlural_Start( BTLV_CORE* wk, u32 pokeCnt, BtlTypeAffAbout affAbout, const u8* pokeID, WazaID wazaID )
{
  DMG_PLURAL_ACT_WORK* subwk = getGenericWork(wk, sizeof(DMG_PLURAL_ACT_WORK));

  subwk->affAbout = affAbout;
  subwk->pokeCnt = pokeCnt;
  subwk->wazaID = wazaID;
  subwk->seq = 0;

  BTL_N_Printf( DBGSTR_VCORE_PluralDamageInfo, pokeCnt, wazaID);

  {
    u32 i;
    for(i=0; i<pokeCnt; ++i)
    {
      BTL_N_Printf( DBGSTR_VCORE_PluralDamagePoke, pokeID[i]);
      subwk->pokePos[i] = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, pokeID[i] );
    }
  }
}

BOOL BTLV_ACT_DamageEffectPlural_Wait( BTLV_CORE* wk )
{
  DMG_PLURAL_ACT_WORK* subwk = getGenericWork(wk, sizeof(DMG_PLURAL_ACT_WORK));

  switch( subwk->seq ){
  case 0:
    {
      BOOL fChapterSkipMode = BTL_CLIENT_IsChapterSkipMode( wk->myClient );
      u32 i;

      for(i=0; i<subwk->pokeCnt; ++i){
        BTLV_SCU_StartWazaDamageAct( wk->scrnU, subwk->pokePos[i], subwk->wazaID, fChapterSkipMode );
      }

      PlayWazaAffSE( wk, subwk->affAbout );
      subwk->seq++;
    }
    break;
  case 1:
    if( BTLV_SCU_WaitWazaDamageAct(wk->scrnU)
    &&  BTLV_SCU_WaitMsg(wk->scrnU)
    ){
      return TRUE;
    }
    break;
  }
  return FALSE;

}

/**
 *  ワザヒットSEを相性に応じて鳴らしわけ
 */
static void PlayWazaAffSE( BTLV_CORE* wk, BtlTypeAffAbout affAbout )
{
  u32 seID;

  switch( affAbout ){
  case BTL_TYPEAFF_ABOUT_ADVANTAGE:     seID = SEQ_SE_KOUKA_H; break;
  case BTL_TYPEAFF_ABOUT_DISADVANTAGE:  seID = SEQ_SE_KOUKA_L; break;
  default:
    seID = SEQ_SE_KOUKA_M;
    break;
  }

  PlaySELocal( wk, seID );
}

//=============================================================================================
/**
 * 溜めワザ（そらをとぶ等）によるポケ表示オン・オフ切り替え
 *
 * @param   wk
 * @param   vpos
 * @param   vanishFlag
 */
//=============================================================================================
void BTLV_ACT_TameWazaHide( BTLV_CORE* wk, BtlvMcssPos vpos, BTLV_MCSS_VANISH_FLAG  vanishFlag )
{
  if( BTLV_EFFECT_CheckExist(vpos) )
  {
    if( vanishFlag != BTLV_EFFECT_GetMcssVanishFlag(vpos) )
    {
      if( vanishFlag == BTLV_MCSS_VANISH_ON )
      {
        BTLV_EFFECT_PokemonVanishOn( vpos );
      }
      else
      {
        BTLV_EFFECT_PokemonVanishOff( vpos );
      }
    }
  }
}


//=============================================================================================
/**
 * シンプルHPエフェクト開始
 * ※HPメーターのみ変動。状態異常や天候ダメージに使う。変動後の値は、直接ポケモンデータを参照している。
 *
 * @param   wk
 * @param   pokePos
 *
 */
//=============================================================================================
void BTLV_ACT_SimpleHPEffect_Start( BTLV_CORE* wk, BtlPokePos pokePos )
{
  BOOL fSkipEffect = BTL_CLIENT_IsChapterSkipMode( wk->myClient );

  BTLV_SCU_StartHPGauge( wk->scrnU, pokePos, fSkipEffect );
}
BOOL BTLV_ACT_SimpleHPEffect_Wait( BTLV_CORE* wk )
{
  if( BTLV_SCU_WaitHPGauge(wk->scrnU) )
  {
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
/**
 * 位置指定汎用エフェクト開始
 *
 * @param   wk
 * @param   pos
 */
//=============================================================================================
void BTLV_AddEffectByPos( BTLV_CORE* wk, BtlvMcssPos vpos, u16 effectNo )
{
  if( !BTL_CLIENT_IsChapterSkipMode(wk->myClient) ){
    BTLV_EFFECT_AddByPos( vpos, effectNo );
  }
}
BOOL BTLV_WaitEffectByPos( BTLV_CORE* wk )
{
  if( !BTLV_EFFECT_CheckExecute() ){
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
/**
 * 開始＆終端位置指定汎用エフェクト開始
 *
 * @param   wk
 * @param   vpos_from
 * @param   vpos_to
 */
//=============================================================================================
void BTLV_AddEffectByDir( BTLV_CORE* wk, BtlvMcssPos vpos_from, BtlvMcssPos vpos_to, u16 effectNo )
{
  BTLV_EFFECT_AddByDir( vpos_from, vpos_to, effectNo );
}
BOOL BTLV_WaitEffectByDir( BTLV_CORE* wk )
{
  if( !BTLV_EFFECT_CheckExecute() ){
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
/**
 * ポケモンひんしアクション開始
 *
 * @param   wk
 * @param   pos   ひんしになったポケモンの位置ID
 *
 */
//=============================================================================================
void BTLV_StartDeadAct( BTLV_CORE* wk, BtlPokePos pos )
{
  BTLV_SCU_StartDeadAct( wk->scrnU, pos, BTL_CLIENT_IsChapterSkipMode(wk->myClient) );
}
BOOL BTLV_WaitDeadAct( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitDeadAct( wk->scrnU );
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
void BTLV_StartReliveAct( BTLV_CORE* wk, BtlPokePos pos )
{
  BTLV_SCU_StartReliveAct( wk->scrnU, pos );
}
BOOL BTLV_WaitReliveAct( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitReliveAct( wk->scrnU );
}




//=============================================================================================
/**
 * ポケモン退出アクション開始
 *
 * @param   wk
 * @param   clientID
 * @param   memberIdx
 *
 */
//=============================================================================================
void BTLV_ACT_MemberOut_Start( BTLV_CORE* wk, BtlvMcssPos vpos, u16 effectNo )
{
  BTLV_SCU_StartMemberOutAct( wk->scrnU, vpos, effectNo, BTL_CLIENT_IsChapterSkipMode(wk->myClient) );
}
BOOL BTLV_ACT_MemberOut_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitMemberOutAct( wk->scrnU );
}


//------------------------------------------
typedef struct {
  BtlPokePos  pokePos;
  u8 clientID;
  u8 memberIdx;
  u8 pokeID;
}MEMBER_IN_WORK;

//=============================================================================================
/**
 * ポケモン入れ替えアクション開始
 *
 * @param   wk
 * @param   clientID
 * @param   memberIdx
 *
 */
//=============================================================================================
void BTLV_StartMemberChangeAct( BTLV_CORE* wk, BtlPokePos pos, u8 clientID, u8 memberIdx )
{
  MEMBER_IN_WORK* subwk = getGenericWork( wk, sizeof(MEMBER_IN_WORK) );
  const BTL_POKEPARAM* bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, pos );

  subwk->clientID = clientID;
  subwk->memberIdx = memberIdx;
  subwk->pokePos = pos;
  subwk->pokeID = BPP_GetID( bpp );
  BTL_Printf("メンバー入れ替え : pos=%d, Idx=%d, pokeID=%d\n", pos, memberIdx, subwk->pokeID );

  BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocMemberIn );
}

BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk )
{
  return BTL_UTIL_CallProc( &wk->subProc );
}

static BOOL subprocMemberIn( int* seq, void* wk_adrs )
{
  BTLV_CORE* wk = wk_adrs;
  MEMBER_IN_WORK* subwk = getGenericWork( wk, sizeof(MEMBER_IN_WORK) );

  switch( *seq ){
  case 0:
    {
      BOOL fEffectSkip = BTL_CLIENT_IsChapterSkipMode( wk->myClient );
      BTLV_SCU_StartPokeIn( wk->scrnU, subwk->pokePos, subwk->clientID, subwk->memberIdx, fEffectSkip );
      (*seq)++;
    }
    break;
  case 1:
    if( BTLV_SCU_WaitPokeIn(wk->scrnU) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 *
 *
 * @param   param
 * @param   dst
 */
//----------------------------------------------------------------------------------
static void StrParamToString( const BTLV_STRPARAM* param, STRBUF* dst )
{
  switch( param->strType ){
  case BTL_STRTYPE_STD:
    BTL_STR_MakeStringStdWithArgArray( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_SET:
    BTL_STR_MakeStringSet( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_UI:
    BTL_STR_GetUIString( dst, param->strID );
    break;
  case BTL_STRTYPE_WAZAOBOE:
    BTL_STR_MakeStringWazaOboeWithArgArray( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_YESNO:
    BTL_STR_MakeStringYesNoWithArgArray( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_WAZA:
    BTL_STR_MakeStringWaza( dst, param->args[0], param->args[1] );
    break;
  case BTL_STRTYPE_SUBWAY_LOCALTRAINER:
    BTL_STR_MakeStringSubway( dst, param->args[0], param->args[1] );
    break;
  default:
    GF_ASSERT(0);
    return;
  }
}


//=============================================================================================
/**
 * 内部バッファにメッセージコピー
 *
 * @param   wk
 * @param   src
 */
//=============================================================================================
void BTLV_CopyMsgToBuffer( BTLV_CORE* wk, const STRBUF* src )
{
  GFL_STR_CopyBuffer( wk->strBuf, src );
}
//=============================================================================================
/**
 * 内部バッファにメッセージ展開
 *
 * @param   wk
 * @param   param
 */
//=============================================================================================
void BTLV_SetMsgToBuffer(  BTLV_CORE* wk, const BTLV_STRPARAM* param )
{
  StrParamToString( param, wk->strBuf );
}
//=============================================================================================
/**
 * 内部バッファメッセージを表示開始
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_StartMsgInBuffer( BTLV_CORE* wk )
{
  PutMsgToSCU( wk, wk->strBuf, BTLV_MSGWAIT_STD, NULL );
}

//=============================================================================================
/**
 * メッセージ表示開始
 *
 * @param   wk
 * @param   param
 */
//=============================================================================================
void BTLV_StartMsg( BTLV_CORE* wk, const BTLV_STRPARAM* param )
{
  StrParamToString( param, wk->strBuf );
  PutMsgToSCU( wk, wk->strBuf, param->wait, NULL );
}
//=============================================================================================
/**
 * メッセージ表示開始（コールバックあり）
 *
 * @param   wk
 * @param   param
 * @param   callback
 */
//=============================================================================================
void BTLV_StartMsgCallback( BTLV_CORE* wk, const BTLV_STRPARAM* param, pPrintCallBack callback )
{
  StrParamToString( param, wk->strBuf );
  PutMsgToSCU( wk, wk->strBuf, param->wait, callback );
}
//=============================================================================================
/**
 * メッセージ表示（一括）
 *
 * @param   wk
 * @param   param
 */
//=============================================================================================
void BTLV_PrintMsgAtOnce( BTLV_CORE* wk, const BTLV_STRPARAM* param )
{
  StrParamToString( param, wk->strBuf );
  PutMsgToSCUatOnce( wk, wk->strBuf );
}

//=============================================================================================
/**

 *
 * @param   wk
 * @param   strID
 *
 */
//=============================================================================================
void BTLV_StartMsgStd( BTLV_CORE* wk, u16 strID, const int* args )
{
  BTL_STR_MakeStringStdWithArgArray( wk->strBuf, strID, args );
  PutMsgToSCU( wk, wk->strBuf, BTLV_MSGWAIT_STD, NULL );
}

//=============================================================================================
/**
 * メッセージ表示開始
 *
 * @param   wk
 * @param   strID
 *
 */
//=============================================================================================
void BTLV_StartMsgSet( BTLV_CORE* wk, u16 strID, const int* args )
{
  BTL_STR_MakeStringSet( wk->strBuf, strID, args );
  PutMsgToSCU( wk, wk->strBuf, BTLV_MSGWAIT_STD, NULL );
}

//=============================================================================================
/**
 * メッセージ表示開始（トレーナーメッセージ）
 *
 * @param   wk
 * @param   trainerID
 * @param   param
 *
 * @retval  TRUE:メッセージ有り　FALSE:メッセージなし
 */
//=============================================================================================
BOOL BTLV_StartMsgTrainer( BTLV_CORE* wk, u32 trainerID, int param )
{
  if( TT_TrainerMessageCheck( trainerID, param, wk->heapID ) )
  {
    TT_TrainerMessageGet( trainerID, param, wk->strBuf, wk->heapID );
    PutMsgToSCU( wk, wk->strBuf, BTLV_MSGWAIT_STD, NULL );
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

void BTLV_StartMsgWaza( BTLV_CORE* wk, u8 pokeID, u16 waza )
{
  BTL_STR_MakeStringWaza( wk->strBuf, pokeID, waza );
  PutMsgToSCU( wk, wk->strBuf, BTLV_MSGWAIT_STD, NULL );
}

BOOL BTLV_WaitMsg( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitMsg( wk->scrnU );
}
BOOL BTLV_IsJustDoneMsg( BTLV_CORE* wk )
{
  return BTLV_SCU_IsJustDoneMsg( wk->scrnU );
}


/**
 *  メインメッセージウィンドウへの出力共通
 */
static void PutMsgToSCU( BTLV_CORE* wk, const STRBUF* buf, u16 wait, pPrintCallBack callBackFunc )
{
  if( !BTL_CLIENT_IsChapterSkipMode(wk->myClient) )
  {
    BTLV_SCU_StartMsg( wk->scrnU, buf, wait, callBackFunc );
  }
}
static void PutMsgToSCUatOnce( BTLV_CORE* wk, const STRBUF* buf )
{
  BTLV_SCU_PrintMsgAtOnce( wk->scrnU, buf );
}

//=============================================================================================
/**
 * とくせいウィンドウ表示開始
 *
 * @param   wk
 * @param   pos
 * @param   fFlash  効果発動を示すフラッシュ動作を入れるか？
 */
//=============================================================================================
void BTLV_TokWin_DispStart( BTLV_CORE* wk, BtlPokePos pos, BOOL fFlash )
{
  BTLV_SCU_TokWin_DispStart( wk->scrnU, pos, fFlash );
}
BOOL BTLV_TokWin_DispWait( BTLV_CORE* wk, BtlPokePos pos )
{
  return BTLV_SCU_TokWin_DispWait( wk->scrnU, pos );
}
void BTLV_QuitTokWin( BTLV_CORE* wk, BtlPokePos pos )
{
  BTLV_SCU_TokWin_HideStart( wk->scrnU, pos );
}
BOOL BTLV_QuitTokWinWait( BTLV_CORE* wk, BtlPokePos pos )
{
  return BTLV_SCU_TokWin_HideWait( wk->scrnU, pos );
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
void BTLV_TokWin_Renew_Start( BTLV_CORE* wk, BtlPokePos pos )
{
  BTLV_SCU_TokWin_Renew_Start( wk->scrnU, pos );
}
//=============================================================================================
/**
 * とくせいウィンドウの内容更新（終了待ち）
 *
 * @param   wk
 * @param   pos
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_TokWin_Renew_Wait( BTLV_CORE* wk, BtlPokePos pos )
{
  return BTLV_SCU_TokWin_Renew_Wait( wk->scrnU, pos );
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
void BTLV_TokWin_SwapStart( BTLV_CORE* wk, BtlPokePos pos1, BtlPokePos pos2 )
{
  BTLV_SCU_TokWin_Swap_Start( wk->scrnU, pos1, pos1 );
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
BOOL BTLV_TokWin_SwapWait( BTLV_CORE* wk )
{
  return BTLV_SCU_TokWin_Swap_Wait( wk->scrnU );
}



void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 vpos )
{
  BTL_Printf("ランクダウンエフェクト開始 : %d\n", vpos);
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_STATUS_DOWN );
}
void BTLV_StartRankUpEffect( BTLV_CORE* wk, u8 vpos )
{
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_STATUS_UP );
}
BOOL BTLV_WaitRankEffect( BTLV_CORE* wk, u8 vpos )
{
  if( !BTLV_EFFECT_CheckExecute() ){
    BTL_Printf("エフェクト終了した ... 一応vpos=%d\n", vpos);
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
/**
 * 「つうしんたいきちゅう…」メッセージを表示
 *
 * @param   wk
 *
 */
//=============================================================================================
void BTLV_StartCommWait( BTLV_CORE* wk )
{
  //カメラワークチェック開始
  BTLV_EFFECT_SetCameraWorkExecute( BTLV_EFFECT_CWE_COMM_WAIT );
  BTLV_SCU_StartCommWaitInfo( wk->scrnU );
}
BOOL BTLV_WaitCommWait( BTLV_CORE* wk )
{
  return BTLV_SCU_WaitCommWaitInfo( wk->scrnU );
}
void BTLV_ResetCommWaitInfo( BTLV_CORE* wk )
{
  //カメラワークチェックを止めてカメラをデフォルト位置にする
  BTLV_EFFECT_SetCameraWorkStop();
  BTLV_SCU_ClearCommWaitInfo( wk->scrnU );
}


//=============================================================================================
/**
 * きのみ食べアクション開始
 *
 * @param   wk
 * @param   pos   きのみを食べるポケモンの位置
 */
//=============================================================================================
void BTLV_KinomiAct_Start( BTLV_CORE* wk, BtlPokePos pos )
{
  BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
  BTLV_SCU_KinomiAct_Start( wk->scrnU, vpos );
}
//=============================================================================================
/**
 * きのみ食べアクション終了待ち
 *
 * @param   wk
 * @param   pos   きのみを食べるポケモンの位置
 */
//=============================================================================================
BOOL BTLV_KinomiAct_Wait( BTLV_CORE* wk, BtlPokePos pos )
{
  BtlvMcssPos vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, pos );
  return BTLV_SCU_KinomiAct_Wait( wk->scrnU, vpos );
}
//=============================================================================================
/**
 * イリュージョン解除アクション 開始
 *
 * @param   wk
 * @param   pos
 */
//=============================================================================================
void BTLV_FakeDisable_Start( BTLV_CORE* wk, BtlPokePos pos, BOOL fSkipMode )
{
  BTLV_SCU_FakeDisable_Start( wk->scrnU, pos, fSkipMode );
}
//=============================================================================================
/**
 * イリュージョン解除アクション 終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_FakeDisable_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_FakeDisable_Wait( wk->scrnU );
}

//=============================================================================================
/**
 * フォルムチェンジ 開始
 *
 * @param   wk
 * @param   vpos
 */
//=============================================================================================
void  BTLV_ChangeForm_Start( BTLV_CORE* wk, BtlvMcssPos vpos )
{
  BOOL fSkipMode = BTL_CLIENT_IsChapterSkipMode( wk->myClient );
  BTLV_SCU_ChangeForm_Start( wk->scrnU, vpos, fSkipMode );
}
//=============================================================================================
/**
 * フォルムチェンジ 終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_ChangeForm_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_ChangeForm_Wait( wk->scrnU );
}
//=============================================================================================
/**
 * へんしん開始
 *
 * @param   wk
 * @param   atkVpos   へんしんする側 vpos
 * @param   tgtVops   へんしん対象側 vpos
 */
//=============================================================================================
void  BTLV_Hensin_Start( BTLV_CORE* wk, BtlvMcssPos atkVpos, BtlvMcssPos tgtVpos )
{
  BOOL fSkipMode = BTL_CLIENT_IsChapterSkipMode( wk->myClient );
  BTLV_SCU_Hensin_Start( wk->scrnU, atkVpos, tgtVpos, fSkipMode );
}
//=============================================================================================
/**
 * へんしん終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_Hensin_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_ChangeForm_Wait( wk->scrnU );
}
//=============================================================================================
/**
 * メッセージウィンドウフェードアウト開始
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_MsgWinHide_Start( BTLV_CORE* wk )
{
  BTLV_SCU_MsgWinHide_Start( wk->scrnU );
}
//=============================================================================================
/**
 * メッセージウィンドウフェードアウト終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_MsgWinHide_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_MsgWinHide_Wait( wk->scrnU );
}



/*--------------------------------------------------------------------------------------------------*/
/* トリプルバトル ムーブアクション関連                                                              */
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
  u8 clientID;
  u8 vpos1;
  u8 vpos2;
  u8 posIdx1;
  u8 posIdx2;
  BtlPokePos  pos1;
  BtlPokePos  pos2;
}MOVE_MEMBER_WORK;

//=============================================================================================
/**
 * ムーブアクション開始
 *
 * @param   wk
 * @param   vpos1
 * @param   vpos2
 */
//=============================================================================================
void BTLV_ACT_MoveMember_Start( BTLV_CORE* wk, u8 clientID, u8 vpos1, u8 vpos2, u8 posIdx1, u8 posIdx2 )
{
  MOVE_MEMBER_WORK* subwk = getGenericWork( wk, sizeof(MOVE_MEMBER_WORK) );

  subwk->clientID = clientID;
  subwk->vpos1 = vpos1;
  subwk->vpos2 = vpos2;
  subwk->posIdx1 = posIdx1;
  subwk->posIdx2 = posIdx2;
  subwk->pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx1 );
  subwk->pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, posIdx2 );

  BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocMoveMember );
}
//=============================================================================================
/**
 * ムーブアクション終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_ACT_MoveMember_Wait( BTLV_CORE* wk )
{
  return BTL_UTIL_CallProc( &wk->subProc );
}
static BOOL subprocMoveMember( int* seq, void* wk_adrs )
{
  BTLV_CORE* wk = wk_adrs;
  MOVE_MEMBER_WORK* subwk = getGenericWork( wk, sizeof(MOVE_MEMBER_WORK) );

  switch( *seq ){
  case 0:
    BTLV_EFFECT_SetSideChange( subwk->vpos1, subwk->vpos2 );
    PlaySELocal( wk, SEQ_SE_TRIPLE_S );
    (*seq)++;
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      BTLV_SCU_MoveGauge_Start( wk->scrnU, subwk->pos1, subwk->pos2 );
      (*seq)++;
    }
    break;

  case 2:
    if( BTLV_SCU_MoveGauge_Wait( wk->scrnU, subwk->pos1, subwk->pos2 ) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}


/*--------------------------------------------------------------------------------------------------*/
/* ローテーションバトル関連                                                                         */
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
  u8 clientID;
  u8 dir;
  u8 vpos1;
  u8 vpos2;
  u8 vpos3;
  BtlPokePos  pos1;
  BtlPokePos  pos2;
  BtlPokePos  pos3;
}ROTATE_MEMBER_WORK;


//#define TMP_SELROT_SKIP

//=============================================================================================
/**
 * ユーザ入力：ローテーション選択開始
 *
 * @param   wk
 * @param   prevDir   前ターンに選択されたローテーション方向
 */
//=============================================================================================
void BTLV_UI_SelectRotation_Start( BTLV_CORE* wk, BtlRotateDir prevDir )
{
  BTLV_INPUT_ROTATE_PARAM* rotateParam = getGenericWork( wk, sizeof(BTLV_INPUT_ROTATE_PARAM) );
  const BTL_PARTY* party = BTL_POKECON_GetPartyDataConst( wk->pokeCon, wk->myClientID );
  const BTL_POKEPARAM* bpp;
  u32 i;

#ifdef ROTATION_NEW_SYSTEM

#else
  for(i=0; i<BTL_ROTATE_NUM; ++i){
    bpp = BTL_PARTY_GetMemberDataConst( party, i );
    rotateParam->pp[i] = BPP_GetSrcData( bpp );
  }
  rotateParam->before_select_dir = prevDir;
#endif

#ifndef TMP_SELROT_SKIP
  BTLV_SCD_SelectRotate_Start( wk->scrnD, rotateParam );
#endif
}
//=============================================================================================
/**
 * ユーザ入力：ローテーション選択終了待ち
 *
 * @param   wk
 * @param   result    [out] 今回選択されたローテーション方向
 *
 * @retval  BOOL      終了時TRUE
 */
//=============================================================================================
BOOL BTLV_UI_SelectRotation_Wait( BTLV_CORE* wk, BtlRotateDir* result )
{
  #ifndef TMP_SELROT_SKIP
  if( BTLV_SCD_SelectRotate_Wait(wk->scrnD, result) ){
    return TRUE;
  }
  return FALSE;
  #else
  BTLV_INPUT_ROTATE_PARAM* rotateParam = getGenericWork( wk, sizeof(BTLV_INPUT_ROTATE_PARAM) );
  u8 rnd = GFL_STD_MtRand(( 100 );
  switch( rotateParam->before_select_dir ){
  case BTL_ROTATEDIR_STAY:
    *result = (rnd < 50)? BTL_ROTATEDIR_L : BTL_ROTATEDIR_R;
    break;
  case BTL_ROTATEDIR_L:
    *result = (rnd < 50)? BTL_ROTATEDIR_STAY : BTL_ROTATEDIR_R;
    break;
  case BTL_ROTATEDIR_R:
    *result = (rnd < 50)? BTL_ROTATEDIR_STAY : BTL_ROTATEDIR_L;
    break;
  default:
    *result = BTL_ROTATEDIR_STAY;
    break;
  }
  return TRUE;
  #endif
}
//=============================================================================================
/**
 * ユーザ入力：ローテーション選択の強制終了通知
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_UI_SelectRotation_ForceQuit( BTLV_CORE* wk )
{
#ifndef TMP_SELROT_SKIP
  BTLV_SCD_SelectRotate_ForceQuit( wk->scrnD );
#endif
}

//=============================================================================================
/**
 * ローテーションアクション開始
 *
 * @param   wk
 * @param   clientID    対象クライアントID
 * @param   dir         ローテーション方向
 */
//=============================================================================================
void BTLV_RotationMember_Start( BTLV_CORE* wk, u8 clientID, BtlRotateDir dir )
{
  if( dir != BTL_ROTATEDIR_STAY )
  {
    ROTATE_MEMBER_WORK* subwk = getGenericWork( wk, sizeof(ROTATE_MEMBER_WORK) );

    subwk->clientID = clientID;
    subwk->dir = dir;

    subwk->pos1 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, 0 );
    subwk->pos2 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, 1 );
    subwk->pos3 = BTL_MAIN_GetClientPokePos( wk->mainModule, clientID, 2 );
    subwk->vpos1 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, subwk->pos1 );
    subwk->vpos2 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, subwk->pos2 );
    subwk->vpos3 = BTL_MAIN_BtlPosToViewPos( wk->mainModule, subwk->pos3 );

    BTL_UTIL_SetupProc( &wk->subProc, wk, NULL, subprocRotateMember );
  }

}
//=============================================================================================
/**
 * ローテーションアクション終了待ち
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTLV_RotationMember_Wait( BTLV_CORE* wk )
{
  return BTL_UTIL_CallProc( &wk->subProc );
}
static BOOL subprocRotateMember( int* seq, void* wk_adrs )
{
  BTLV_CORE* wk = wk_adrs;
  ROTATE_MEMBER_WORK* subwk = getGenericWork( wk, sizeof(ROTATE_MEMBER_WORK) );

  switch( *seq ){
  case 0:
    BTLV_EFFECT_SetRotateEffect( subwk->dir, subwk->vpos1 & 1, !BTL_CLIENT_IsChapterSkipMode(wk->myClient) );
    (*seq)++;
    break;

  case 1:
    if( !BTLV_EFFECT_CheckExecute() ){
      BTLV_SCU_UpdateGauge_Start( wk->scrnU, subwk->pos1 );
      BTLV_SCU_UpdateGauge_Start( wk->scrnU, subwk->pos2 );
      BTLV_SCU_UpdateGauge_Start( wk->scrnU, subwk->pos3 );
      (*seq)++;
    }
    break;

  case 2:
    {
      BOOL fEnd = TRUE;
      if( BTLV_SCU_UpdateGauge_Wait(wk->scrnU, subwk->pos1) == FALSE ){
        fEnd = FALSE;
      }
      if( BTLV_SCU_UpdateGauge_Wait(wk->scrnU, subwk->pos2) == FALSE ){
        fEnd = FALSE;
      }
      if( BTLV_SCU_UpdateGauge_Wait(wk->scrnU, subwk->pos3) == FALSE ){
        fEnd = FALSE;
      }
      if( fEnd ){
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}


//=============================================================================================
/**
 * はい／いいえ選択
 *
 * @param   wk
 * @param   yes_msg   はいのボタンに出すメッセージ
 * @param   no_msg    いいえのボタンに出すメッセージ
 * @param   b_cancel  Bでいいえを押したことにするかどうか
 */
//=============================================================================================
void BTLV_YESNO_Start( BTLV_CORE* wk, BTLV_STRPARAM* yes_msg, BTLV_STRPARAM* no_msg, BOOL b_cancel )
{
  BTLV_INPUT_YESNO_PARAM* yesnoParam = getGenericWork( wk, sizeof( BTLV_INPUT_YESNO_PARAM ) );

  yesnoParam->yes_msg = GFL_STR_CreateBuffer( BTL_YESNO_MSG_LENGTH, GFL_HEAP_LOWID(wk->heapID) );
  yesnoParam->no_msg = GFL_STR_CreateBuffer( BTL_YESNO_MSG_LENGTH, GFL_HEAP_LOWID(wk->heapID) );
  yesnoParam->b_cancel_flag = b_cancel;

  StrParamToString( yes_msg, yesnoParam->yes_msg );
  StrParamToString( no_msg,  yesnoParam->no_msg );
//  BTL_STR_MakeStringYesNoWithArgArray( yesnoParam->yes_msg, yes_msg->strID, yes_msg->args );
//  BTL_STR_MakeStringYesNoWithArgArray( yesnoParam->no_msg, no_msg->strID, no_msg->args );

  BTLV_SCD_SelectYesNo_Start( wk->scrnD, yesnoParam );

  GFL_HEAP_FreeMemory( yesnoParam->yes_msg );
  GFL_HEAP_FreeMemory( yesnoParam->no_msg );
}

BOOL BTLV_YESNO_Wait( BTLV_CORE* wk, BtlYesNo* result )
{
  return BTLV_SCD_SelectYesNo_Wait( wk->scrnD, result );
}

//=============================================================================================
/**
 * 技忘れ
 *
 * @param   wk
 * @param   pos   技を覚えるポケモンの位置
 * @param   waza  新しく覚える技ナンバー
 */
//=============================================================================================
void BTLV_WAZAWASURE_Start( BTLV_CORE* wk, u8 pos, WazaID waza )
{
  #if 1
  SetupPlistDataCommon( wk, &wk->plistData, BPL_MODE_WAZASET, pos, waza );
  #else
  wk->plistData.pp = BTL_MAIN_GetPlayerPokeParty( wk->mainModule );
  wk->plistData.multi_pp = BTL_MAIN_GetMultiPlayerPokeParty( wk->mainModule );
  wk->plistData.multiMode = ( wk->plistData.multi_pp != NULL );
  wk->plistData.multiPos = BTL_MAIN_GetPlayerMultiPos( wk->mainModule );
  wk->plistData.font = wk->largeFontHandle;
  wk->plistData.heap = wk->heapID;
  wk->plistData.mode = BPL_MODE_WAZASET;
  wk->plistData.end_flg = FALSE;
  wk->plistData.sel_poke = pos;
  wk->plistData.chg_waza = waza;
  wk->plistData.rule = BTL_MAIN_GetRule( wk->mainModule );
  wk->plistData.cursor_flg = BTLV_SCD_GetCursorFlagPtr( wk->scrnD );
  wk->plistData.tcb_sys = BTLV_EFFECT_GetTCBSYS();
  wk->plistData.pfd = BTLV_EFFECT_GetPfd();
  wk->plistData.gamedata = BTL_MAIN_GetGameData( wk->mainModule );
  #endif
  wk->selectItemSeq = 0;
}

BOOL BTLV_WAZAWASURE_Wait( BTLV_CORE* wk, u8* result )
{
  switch( wk->selectItemSeq ){
  case 0:
    BTLV_SCD_FadeOut( wk->scrnD, FALSE );
    GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_b_app ) );
    wk->selectItemSeq++;
    break;
  case 1:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      BTLV_SCD_Cleanup( wk->scrnD );
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_plist ) );
      BattlePokeList_TaskAdd( &wk->plistData );
      wk->selectItemSeq++;
    }
    break;
  case 2:
    if( wk->plistData.end_flg )
    {
      GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_plist ) );
      BTLV_SCD_FadeIn( wk->scrnD );
      BTLV_SCD_Setup( wk->scrnD );
      wk->selectItemSeq++;
    }
    break;
  case 3:
    if( BTLV_SCD_FadeFwd(wk->scrnD) ){
      wk->selectItemSeq++;
    }
    break;
  default:
    *result = wk->plistData.sel_wp;
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_b_app ) );
    wk->selectItemSeq = 0;
    return TRUE;
  }
  return FALSE;
}

//=============================================================================================
//  レベルアップ情報処理
//=============================================================================================

/**
 *  表示処理 開始
 */
void BTLV_LvupWin_StartDisp( BTLV_CORE* wk, const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo )
{
  BTLV_SCU_LvupWin_StartDisp( wk->scrnU, bpp, lvupInfo );
}
/**
 *  表示処理 終了待ち
 */
BOOL BTLV_LvupWin_WaitDisp( BTLV_CORE* wk )
{
  return BTLV_SCU_LvupWin_WaitDisp( wk->scrnU );
}
/**
 *  次ページ表示処理 開始
 */
void BTLV_LvupWin_StepFwd( BTLV_CORE* wk )
{
  BTLV_SCU_LvupWin_StepFwd( wk->scrnU );
}
/**
 *  次ページ表示処理 終了待ち
 */
BOOL BTLV_LvupWin_WaitFwd( BTLV_CORE* wk )
{
  return BTLV_SCU_LvupWin_WaitFwd( wk->scrnU );
}
/**
 *  消去処理 開始
 */
void BTLV_LvupWin_StartHide( BTLV_CORE* wk )
{
  BTLV_SCU_LvupWin_StartHide( wk->scrnU );
}
/**
 *  消去処理 終了待ち
 */
BOOL BTLV_LvupWin_WaitHide( BTLV_CORE* wk )
{
  return BTLV_SCU_LvupWin_WaitHide( wk->scrnU );
}

//=============================================================================================
//  録画再生時フェードアウト処理
//=============================================================================================

void BTLV_RecPlayFadeOut_Start( BTLV_CORE* wk )
{
  BTLV_SCU_RecPlayFadeOut_Start( wk->scrnU );
}
BOOL BTLV_RecPlayFadeOut_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_RecPlayFadeIn_Wait( wk->scrnU );
}
void BTLV_RecPlayFadeIn_Start( BTLV_CORE* wk )
{
  BTLV_SCU_RecPlayFadeIn_Start( wk->scrnU );
}
BOOL BTLV_RecPlayFadeIn_Wait( BTLV_CORE* wk )
{
  return BTLV_SCU_RecPlayFadeIn_Wait( wk->scrnU );
}


//=============================================================================================
//  時間制限によるアクション選択の強制終了対応
//=============================================================================================
void BTLV_ForceQuitInput_Notify( BTLV_CORE* wk )
{
  BTLV_SCD_ForceQuitInput_Notify( wk->scrnD );
}

BOOL BTLV_ForceQuitInput_Wait( BTLV_CORE* wk )
{
  return BTLV_SCD_ForceQuitInput_Wait( wk->scrnD );
}

//=============================================================================================
//  録画再生モードキー操作
//=============================================================================================

int BTLV_CheckRecPlayerInput( BTLV_CORE* wk )
{
  return BTLV_SCD_CheckRecPlayerInput( wk->scrnD );
}
void BTLV_UpdateRecPlayerInput( BTLV_CORE* wk, u16 currentChapter, u16 ctrlChapter )
{
  wk->recPlayerUI.play_chapter = currentChapter;
  wk->recPlayerUI.view_chapter = ctrlChapter;
  wk->recPlayerUI.stop_flag = BTLV_INPUT_BR_STOP_NONE;

  BTLV_SCD_SetupRecPlayerMode( wk->scrnD, &wk->recPlayerUI );
}
void BTLV_RecPlayer_StartSkip( BTLV_CORE* wk, u16 nextChapter )
{
  wk->recPlayerUI.play_chapter = nextChapter;
  wk->recPlayerUI.view_chapter = nextChapter;
  wk->recPlayerUI.stop_flag = BTLV_INPUT_BR_STOP_SKIP;

  BTLV_SCD_SetupRecPlayerMode( wk->scrnD, &wk->recPlayerUI );
}
void BTLV_RecPlayer_StartQuit( BTLV_CORE* wk, u16 chapter, BTLV_INPUT_BR_STOP_FLAG stop_flag )
{
  wk->recPlayerUI.play_chapter = chapter;
  wk->recPlayerUI.view_chapter = chapter;
  wk->recPlayerUI.stop_flag = stop_flag;

  BTLV_SCD_SetupRecPlayerMode( wk->scrnD, &wk->recPlayerUI );
}

/**
 *  SE再生（チャプタスキップ時には無視する）
 */
static void PlaySELocal( BTLV_CORE* wk, u32 SENo )
{
  if( !BTL_CLIENT_IsChapterSkipMode(wk->myClient) ){
    PMSND_PlaySE( SENo );
  }
}

/*--------------------------------------------------------------------------------------------------*/
/* 文字列パラメータ設定                                                                             */
/*--------------------------------------------------------------------------------------------------*/
void BTLV_STRPARAM_Setup( BTLV_STRPARAM* sp, BtlStrType strType, u16 strID )
{
  int i;
  for(i=0; i<NELEMS(sp->args); ++i){
    sp->args[i] = 0;
  }
  sp->argCnt = 0;
  sp->strID = strID;
  sp->strType = strType;
  sp->wait = BTLV_MSGWAIT_STD;
}
void BTLV_STRPARAM_AddArg( BTLV_STRPARAM* sp, int arg )
{
  if( sp->argCnt < NELEMS(sp->args) ){
    sp->args[ sp->argCnt++ ] = arg;
  }
}
void BTLV_STRPARAM_SetWait( BTLV_STRPARAM* sp, u8 wait )
{
  sp->wait = wait;
}

/*--------------------------------------------------------------------------------------------------*/
/* 下請けから呼び出される関数群                                                                     */
/*--------------------------------------------------------------------------------------------------*/

u8 BTLV_CORE_GetPlayerClientID( const BTLV_CORE* core )
{
  return core->myClientID;
}


#ifdef PM_DEBUG
//=============================================================================================
/**
 * 一時的に依存クライアントを差し替え（デバッグ用AI操作処理のため）
 *
 * @param   wk
 * @param   client
 *
 * @retval  BTL_CLIENT*   元々セットされていたクライアントポインタ
 */
//=============================================================================================
const BTL_CLIENT* BTLV_SetTmpClient( BTLV_CORE* wk, const BTL_CLIENT* client )
{
  const BTL_CLIENT* oldClient = wk->myClient;
  wk->myClient = client;

  return oldClient;
}
#endif
