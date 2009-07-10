//=============================================================================================
/**
 * @file  btlv_scd.c
 * @brief ポケモンWB バトル 描画 下画面制御モジュール
 * @author  taya
 *
 * @date  2008.11.18  作成
 */
//=============================================================================================
#include <ui.h>
#include "print/printsys.h"

#include "arc_def.h"
#include "font/font.naix"
#include "sound/pm_sndsys.h"
#include "poke_tool/monsno_def.h"

#include "msg/msg_btl_ui.h"

#include "battle/btl_common.h"
#include "battle/btl_util.h"
#include "battle/btl_string.h"
#include "battle/btl_action.h"

#include "btlv_core.h"
#include "btlv_effect.h"
#include "btlv_mcss.h"
#include "btlv_scd.h"

#include "btlv_input.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  TEST_SELWIN_COL1 = 0x04,
  TEST_SELWIN_COL2 = 0x06,
  TEST_SELWIN_COL3 = 0x07,
  TEST_SELWIN_COL4 = 0x0A,

  POKEWIN_WIDTH  = 15*8,
  POKEWIN_HEIGHT = 4*8,

  POKEWIN_1_X = 0,
  POKEWIN_1_Y = 8,
  POKEWIN_2_X = 256-POKEWIN_WIDTH,
  POKEWIN_2_Y = POKEWIN_1_Y,

  POKEWIN_3_X = 0,
  POKEWIN_3_Y = POKEWIN_1_Y+POKEWIN_HEIGHT+8,
  POKEWIN_4_X = 256-POKEWIN_WIDTH,
  POKEWIN_4_Y = POKEWIN_3_Y,

  POKEWIN_5_X = 0,
  POKEWIN_5_Y = POKEWIN_3_Y+POKEWIN_HEIGHT+8,
  POKEWIN_6_X = 256-POKEWIN_WIDTH,
  POKEWIN_6_Y = POKEWIN_5_Y,

  POKEWIN_BACK_X = POKEWIN_6_X + 64,
  POKEWIN_BACK_Y = POKEWIN_6_Y + POKEWIN_HEIGHT + 24,

  SUBPROC_STACK_DEPTH = 4,
};

//--------------------------------------------------------------
/**
 *  ワザ対象の選択／確認画面用パラメータ
 */
//--------------------------------------------------------------
enum {
  STW_CONFIRM_POKE,
  STW_CONFIRM_FIELD,
};
typedef u8 StwConfirmType;

typedef struct {

  u8 selectablePokeCount;       ///< 選択できるポケモンの候補数（0なら選択不可->確認モード）
  u8 confirmCount;              ///< 確認モード対象の数
  StwConfirmType  confirmType;  ///< 確認モード対象タイプ（0:ポケモン, 1:場）

  u8 pos[ BTL_POS_MAX ];    ///< 選択・確認するポケモンの位置

}SEL_TARGET_WORK;

//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
struct _BTLV_SCD {

  GFL_BMPWIN*     win;
  GFL_BMP_DATA*   bmp;
  GFL_FONT*       font;
  STRBUF*         strbuf;

  PRINT_QUE*      printQue;
  PRINT_UTIL      printUtil;

  BTL_PROC      subProc;
  BTL_PROC      subProcStack[ SUBPROC_STACK_DEPTH ];
  u32           subProcStackPtr;
  int           sub_seq;

  const BTL_POKEPARAM*  bpp;
  BTL_ACTION_PARAM*     destActionParam;

  const BTL_POKESELECT_PARAM*   pokesel_param;
  BTL_POKESELECT_RESULT*        pokesel_result;

  SEL_TARGET_WORK   selTargetWork;
  u8                selTargetDone;

  BtlAction  selActionResult;
  const BTLV_CORE* vcore;
  const BTL_MAIN_MODULE* mainModule;
  const BTL_POKE_CONTAINER* pokeCon;
  const BTL_PARTY*          playerParty;
  HEAPID  heapID;

  BTLV_INPUT_WORK     *biw;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void spstack_init( BTLV_SCD* wk );
static void spstack_push( BTLV_SCD* wk, BPFunc initFunc, BPFunc loopFunc );
static BOOL spstack_call( BTLV_SCD* wk );
static void printBtn( BTLV_SCD* wk, u16 posx, u16 posy, u16 sizx, u16 sizy, u16 col, u16 strID );
static void printBtnWaza( BTLV_SCD* wk, u16 btnIdx, u16 col, const STRBUF* str );
static BOOL selectAction_init( int* seq, void* wk_adrs );
static BOOL selectActionRoot_loop( int* seq, void* wk_adrs );
static BOOL selectWaza_init( int* seq, void* wk_adrs );
static BOOL selectWaza_loop( int* seq, void* wk_adrs );
static BtlvScd_SelAction_Result  check_unselectable_waza( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, u8 waza_idx );
static void stw_init( SEL_TARGET_WORK* stw );
static void stw_convert_pos_to_index( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, u8 num );
static void stw_setSelectablePoke( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos );
static void stw_setConfirmPoke( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos );
static void stw_setConfirmField( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos );
static BOOL stw_is_enable_hitpos( SEL_TARGET_WORK* stw, int hitPos, const BTL_MAIN_MODULE* mainModule, u8* target_idx );
static inline u8 stwdraw_vpos_to_tblidx( u8 vpos );
static void stwdraw_button( const u8* pos, u8 count, u8 format, BTLV_SCD* wk );
static void stw_draw( const SEL_TARGET_WORK* stw, BTLV_SCD* work );
static BOOL stw_draw_wait( BTLV_SCD* wk );
static BOOL selectTarget_init( int* seq, void* wk_adrs );
static BOOL selectTarget_loop( int* seq, void* wk_adrs );
static void seltgt_init_setup_work( SEL_TARGET_WORK* stw, BTLV_SCD* wk );
static BOOL selectPokemon_init( int* seq, void* wk_adrs );
static BOOL selectPokemon_loop( int* seq, void* wk_adrs );
static void printCommWait( BTLV_SCD* wk );
static  inline  void  SePlayDecide( void );
static  inline  void  SePlayCancel( void );

static  inline  void  SePlayDecide( void );
static  inline  void  SePlayCancel( void );


BTLV_SCD*  BTLV_SCD_Create( const BTLV_CORE* vcore, const BTL_MAIN_MODULE* mainModule,
        const BTL_POKE_CONTAINER* pokeCon, GFL_TCBLSYS* tcbl, GFL_FONT* font, u8 playerClientID, HEAPID heapID )
{
  BTLV_SCD* wk = GFL_HEAP_AllocMemory( heapID, sizeof(BTLV_SCD) );

  wk->vcore = vcore;
  wk->mainModule = mainModule;
  wk->pokeCon = pokeCon;
  wk->heapID = heapID;
  wk->font = font;
  wk->playerParty = BTL_POKECON_GetPartyDataConst( pokeCon, playerClientID );
  wk->strbuf = GFL_STR_CreateBuffer( 128, heapID );
  wk->pokesel_param = NULL;

  wk->printQue = PRINTSYS_QUE_Create( wk->heapID );

  spstack_init( wk );

  return wk;
}

void BTLV_SCD_Setup( BTLV_SCD* wk )
{
  wk->biw = BTLV_INPUT_Init( BTLV_INPUT_TYPE_SINGLE, wk->font, wk->heapID );

  ///<obj
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

  //GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 0, 0, wk->heapID );
//  PaletteWorkSet_Arc( BTLV_EFFECT_GetPfd(), ARCID_FONT, NARC_font_default_nclr, wk->heapID, FADE_SUB_BG, 0x10, 0xe0 );

  wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME2_S, 0, 2, 32, 22, 0x0e, GFL_BMP_CHRAREA_GET_F );
  wk->bmp = GFL_BMPWIN_GetBmp( wk->win );
  GFL_BMP_Clear( wk->bmp, 0x00 );
  PRINT_UTIL_Setup( &wk->printUtil, wk->win );
  GFL_BMPWIN_MakeScreen( wk->win );
  GFL_BMPWIN_TransVramCharacter( wk->win );
  GFL_BG_LoadScreenReq( GFL_BG_FRAME2_S );

}

void BTLV_SCD_Delete( BTLV_SCD* wk )
{
  GFL_BMPWIN_Delete( wk->win );

  PRINTSYS_QUE_Delete( wk->printQue );
  GFL_STR_DeleteBuffer( wk->strbuf );
  BTLV_INPUT_Exit( wk->biw );
  GFL_HEAP_FreeMemory( wk );
}

//-------------------------------------------------
// サブプロセススタック管理

static void spstack_init( BTLV_SCD* wk )
{
  int i;
  for(i=0; i<SUBPROC_STACK_DEPTH; i++)
  {
    BTL_UTIL_ClearProc( &wk->subProcStack[i] );
  }
  wk->subProcStackPtr = 0;
}

static void spstack_push( BTLV_SCD* wk, BPFunc initFunc, BPFunc loopFunc )
{
  GF_ASSERT(wk->subProcStackPtr < SUBPROC_STACK_DEPTH);

  {
    BTL_PROC* proc = &wk->subProcStack[wk->subProcStackPtr++];
    BTL_UTIL_SetupProc( proc, wk, initFunc, loopFunc );
  }
}
static BOOL spstack_call( BTLV_SCD* wk )
{
  if( wk->subProcStackPtr > 0 )
  {
    BTL_PROC* proc = &wk->subProcStack[ wk->subProcStackPtr - 1 ];

    if( BTL_UTIL_CallProc(proc) )
    {
      BTL_UTIL_ClearProc( proc );
      wk->subProcStackPtr--;
    }
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
//=============================================================================================
//=============================================================================================

//=============================================================================================
/**
 *
 *
 * @param   wk
 */
//=============================================================================================
void BTLV_SCD_CleanupUI( BTLV_SCD* wk )
{
//  Sub_TouchEndDelete( wk->bip, TRUE, TRUE );
  BTLV_INPUT_CreateScreen( wk->biw, BTLV_INPUT_SCRTYPE_STANDBY, NULL );
}

//=============================================================================================
//  アクション選択処理
//=============================================================================================
void BTLV_SCD_StartActionSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  wk->bpp = bpp;
  wk->destActionParam = dest;
  wk->selActionResult = BTL_ACTION_NULL;

  spstack_push( wk, selectAction_init, selectActionRoot_loop );
}
void BTLV_SCD_RestartActionSelect( BTLV_SCD* wk )
{
  wk->selActionResult = BTL_ACTION_NULL;
}
BtlAction BTLV_SCD_WaitActionSelect( BTLV_SCD* wk )
{
  if( spstack_call( wk ) ){
    return wk->selActionResult;
  }else{
    return BTL_ACTION_NULL;
  }
}

void BTLV_SCD_StartWazaSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  wk->bpp = bpp;
  wk->destActionParam = dest;
  wk->selActionResult = BTL_ACTION_NULL;

  spstack_push( wk, selectWaza_init, selectWaza_loop );
}
BOOL BTLV_SCD_WaitWazaSelect( BTLV_SCD* wk )
{
  return spstack_call( wk );
}

void BTLV_SCD_StartTargetSelect( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, BTL_ACTION_PARAM* dest )
{
  wk->bpp = bpp;
  wk->destActionParam = dest;

  spstack_push( wk, selectTarget_init, selectTarget_loop );
}
BOOL BTLV_SCD_WaitTargetSelect( BTLV_SCD* wk )
{
  return spstack_call( wk );
}


///コマンド選択タッチパネル領域設定
static const GFL_UI_TP_HITTBL BattleMenuTouchData[] = {
  //UP DOWN LEFT RIGHT
  {3*8, 0x12*8, 0*8, 255},      //たたかう
  {0x12*8, 0x18*8, 0*8, 0xa*8},   //バッグ
  {0x12*8, 0x18*8, 0x16*8, 255},  //ポケモン
  {0x13*8, 0x18*8, 0xb*8, 0x15*8},  //にげる
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};
///技選択タッチパネル領域設定
static const GFL_UI_TP_HITTBL SkillMenuTouchData[] = {
  //UP DOWN LEFT RIGHT
  {0x13*8, 0x18*8, 1*8, 0x1f*8},    //キャンセル
  {3*8, 10*8, 0*8, 0x10*8},   //技1
  {3*8, 10*8, 0x10*8, 255}, //技2
  {0xb*8, 0x12*8, 0*8, 0x10*8}, //技3
  {0xb*8, 0x12*8, 0x10*8, 255}, //技4
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};

static const struct {
    u8 x;
    u8 y;
    u8 w;
    u8 h;
}BtlPos[] = {
  {   0,  0, 128, 96 },
  { 128,  0, 128, 96 },
  {   0, 96, 128, 96 },
  { 128, 96, 128, 96 },
};


static void printBtn( BTLV_SCD* wk, u16 posx, u16 posy, u16 sizx, u16 sizy, u16 col, u16 strID )
{
  u32 strWidth, drawX, drawY;

  GFL_BMP_Fill( wk->bmp, posx, posy, sizx, sizy, col );
  BTL_STR_GetUIString( wk->strbuf, strID );
  strWidth = PRINTSYS_GetStrWidth( wk->strbuf, wk->font, 0 );
  drawX = posx + (sizx - strWidth) / 2;
  drawY = posy + (sizy - 16) / 2;
  PRINT_UTIL_Print( &wk->printUtil, wk->printQue, drawX, drawY, wk->strbuf, wk->font );
}

static void printBtnWaza( BTLV_SCD* wk, u16 btnIdx, u16 col, const STRBUF* str )
{
  u32 strWidth, drawX, drawY;
  u8 posx, posy, sizx, sizy;

  posx = BtlPos[btnIdx].x;
  posy = BtlPos[btnIdx].y;
  sizx = BtlPos[btnIdx].w;
  sizy = BtlPos[btnIdx].h;

  GFL_BMP_Fill( wk->bmp, posx, posy, sizx, sizy, col );

  // soga
  if( str == NULL ) return;

  strWidth = PRINTSYS_GetStrWidth( wk->strbuf, wk->font, 0 );
  drawX = posx + (sizx - strWidth) / 2;
  drawY = posy + (sizy - 32) / 2;
  PRINT_UTIL_Print( &wk->printUtil, wk->printQue, drawX, drawY, wk->strbuf, wk->font );
}


static BOOL selectAction_init( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;
  BTLV_INPUT_DIR_PARAM  bidp[ TEMOTI_POKEMAX ];
  const BTL_PARTY* party;
  const BTL_POKEPARAM* bpp;
  const POKEMON_PARAM* pp;
  u16 members, hp, i;

  MI_CpuClear16( &bidp, sizeof( BTLV_INPUT_DIR_PARAM ) * TEMOTI_POKEMAX );

  party = wk->playerParty;
  members = BTL_PARTY_GetMemberCount( party );

  for( i = 0 ; i < members ; i++ )
  {
    bpp = BTL_PARTY_GetMemberDataConst( party, i );
    pp  = BTL_POKEPARAM_GetSrcData( bpp );
    hp = PP_Get( pp, ID_PARA_hp, NULL );

    if( hp )
    {
      if( PP_Get( pp, ID_PARA_condition, NULL ) )
      {
        bidp[ i ].status = BTLV_INPUT_STATUS_NG;
      }
      else
      {
        bidp[ i ].status = BTLV_INPUT_STATUS_ALIVE;
      }
    }
    else
    {
      bidp[ i ].status = BTLV_INPUT_STATUS_DEAD;
    }
  }

  BTLV_INPUT_CreateScreen( wk->biw, BTLV_INPUT_SCRTYPE_COMMAND, bidp );

  return TRUE;
}

static BOOL selectActionRoot_loop( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;
  int hit;

  //カメラワークエフェクト
  if( !BTLV_EFFECT_CheckExecute() ){
    BTLV_EFFECT_Add( BTLEFF_CAMERA_WORK );
  }

//  hit = GFL_UI_TP_HitTrg( BattleMenuTouchData );
  hit = BTLV_INPUT_CheckInput( wk->biw, BattleMenuTouchData );
  if( hit != GFL_UI_TP_HIT_NONE )
  {
    static const u8 action[] = {
      BTL_ACTION_FIGHT,   BTL_ACTION_ITEM,
      BTL_ACTION_CHANGE,  BTL_ACTION_ESCAPE,
    };

    wk->selActionResult = action[ hit ];

    switch( wk->selActionResult ){
    // @@@ 現状アイテム選択は実装されていないので
    case BTL_ACTION_ITEM:
      return FALSE;

    case BTL_ACTION_ESCAPE:
      BTL_ACTION_SetEscapeParam( wk->destActionParam );
      break;
    }

    SePlayDecide();
    return TRUE;
  }

  return FALSE;
}


static BOOL selectWaza_init( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;
  BTLV_INPUT_WAZA_PARAM biwp;
  u16 wazaCnt, wazaID, i;
  u8 PP, PPMax;

  wazaCnt = BTL_POKEPARAM_GetWazaCount( wk->bpp );
  for(i=0; i<wazaCnt; i++)
  {
    wazaID = BTL_POKEPARAM_GetWazaParticular( wk->bpp, i, &PP, &PPMax );
    biwp.wazano[ i ] = wazaID;
    biwp.pp[ i ]     = PP;
    biwp.ppmax[ i ]  = PPMax;
  }
  for( ; i<PTL_WAZA_MAX; i++){
    biwp.wazano[ i ]  = 0;
    biwp.pp[ i ]      = 0;
    biwp.ppmax[ i ]   = 0;
  }

  BTLV_INPUT_CreateScreen( wk->biw, BTLV_INPUT_SCRTYPE_WAZA, &biwp );

  return TRUE;
}

static BOOL selectWaza_loop( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;
  int hit;

  //カメラワークエフェクト
  if( !BTLV_EFFECT_CheckExecute() ){
    BTLV_EFFECT_Add( BTLEFF_CAMERA_WORK );
  }

//  hit = GFL_UI_TP_HitTrg( SkillMenuTouchData );
  hit = BTLV_INPUT_CheckInput( wk->biw, SkillMenuTouchData );
  if( hit != GFL_UI_TP_HIT_NONE )
  {
    //キャンセルが押された
    if( hit == 0 )
    {
      SePlayCancel();
      BTL_ACTION_SetNULL( wk->destActionParam );
      return TRUE;
    }
    else if( hit <= BTL_POKEPARAM_GetWazaCount( wk->bpp ) )
    {
      WazaID waza;

      //キャンセルが先頭にはいっているので、デクリメントして正規の選択位置にする
      waza = BTL_POKEPARAM_GetWazaNumber( wk->bpp, --hit );
      BTL_ACTION_SetFightParam( wk->destActionParam, waza, 0 );

      SePlayDecide();
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * こだわり状態など、選択できないワザを選んだら対応する警告IDを返す
 *
 * @param   wk
 * @param   bpp
 * @param   waza_idx
 *
 * @retval  BtlvScd_SelAction_Result
 */
//----------------------------------------------------------------------------------
static BtlvScd_SelAction_Result  check_unselectable_waza( BTLV_SCD* wk, const BTL_POKEPARAM* bpp, u8 waza_idx )
{
  if( BTL_POKEPARAM_GetContFlag(bpp, BPP_CONTFLG_KODAWARI_LOCK) )
  {
    WazaID  select_waza = BTL_POKEPARAM_GetWazaNumber( bpp, waza_idx );
    if( select_waza != BTL_POKEPARAM_GetPrevWazaNumber(bpp) ){
      return BTLV_SCD_SelAction_Warn_Kodawari;
    }
  }

  if( BTL_POKEPARAM_CheckSick(bpp, WAZASICK_ENCORE) )
  {
    WazaID  select_waza = BTL_POKEPARAM_GetWazaNumber( bpp, waza_idx );
    if( select_waza != BTL_POKEPARAM_GetPrevWazaNumber(bpp) ){
      return BTLV_SCD_SelAction_Warn_WazaLock;
    }
  }

  return BTLV_SCD_SelAction_Still;
}

//--------------------------------------------------------------------------------------
// ターゲット選択画面描画処理
//--------------------------------------------------------------------------------------
enum {
// 2vs2
//    D B
//
// A C
  STW_BTN_WIDTH = 14,
  STW_BTN_HEIGHT = 10,

  STW_BTNPOS_D_X = 1,
  STW_BTNPOS_D_Y = 1,
  STW_BTNPOS_B_X = STW_BTNPOS_D_X + STW_BTN_WIDTH + 1,
  STW_BTNPOS_B_Y = STW_BTNPOS_D_Y,
  STW_BTNPOS_A_X = STW_BTNPOS_D_X,
  STW_BTNPOS_A_Y = STW_BTNPOS_D_Y + STW_BTN_HEIGHT + 1,
  STW_BTNPOS_C_X = STW_BTNPOS_B_X,
  STW_BTNPOS_C_Y = STW_BTNPOS_A_Y,

};
static const GFL_UI_TP_HITTBL PokeSeleMenuTouchData[] = {
  //UP DOWN LEFT RIGHT
  {0xb*8, 0x12*8, 0*8, 0xf*8},      //ターゲットA
  {1*8, 0xa*8, 0x11*8, 255},    //ターゲットB
  {0xb*8, 0x12*8, 0x11*8, 255},   //ターゲットC
  {1*8, 0xa*8, 0*8, 0xf*8},     //ターゲットD
  {0x13*8, 0x18*8, 1*8, 0x1f*8},    //キャンセル
  { GFL_UI_TP_HIT_END, 0, 0, 0 }
};
// ↑描画位置から上記テーブルインデックスを引くためのテーブル
static const u8 STW_HitTblIndex[] = {
  BTLV_MCSS_POS_A,
  BTLV_MCSS_POS_B,
  BTLV_MCSS_POS_C,
  BTLV_MCSS_POS_D,
};

/*
typedef struct {

  u8 selectablePokeCount;   ///< 選択できるポケモンの候補数（0なら選択不可->確認モード）
  u8 confirmCount;          ///< 確認モード対象の数
  u8 confirmType;           ///< 確認モード対象タイプ（0:ポケモン, 1:場）

  u8 pos[ BTL_POS_MAX ];    ///< 選択・確認するポケモンの位置

}SEL_TARGET_WORK;
*/
// 構造体初期化
static void stw_init( SEL_TARGET_WORK* stw )
{
  GFL_STD_MemClear( stw, sizeof( *stw ) );
}
// pos(戦闘位置）配列を、ポケモン選択インターフェイス用のインデックスに変換
static void stw_convert_pos_to_index( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, u8 num )
{
  u8 i;

  for(i=0; i<num; ++i)
  {
    stw->pos[i] = BTL_MAIN_BtlPosToViewPos( mainModule, stw->pos[i] );
  }
}
// 選択可能ポケモンをセット
static void stw_setSelectablePoke( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos )
{
  u8 num = BTL_MAIN_ExpandBtlPos( mainModule, exPos, stw->pos );
  stw->selectablePokeCount = num;
}
// 確認のみ（ポケモン）をセット
static void stw_setConfirmPoke( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos )
{
  u8 num = BTL_MAIN_ExpandBtlPos( mainModule, exPos, stw->pos );
  stw->confirmCount = num;
  stw->confirmType = STW_CONFIRM_POKE;
}
// 確認のみ（場）をセット
static void stw_setConfirmField( SEL_TARGET_WORK* stw, const BTL_MAIN_MODULE* mainModule, BtlExPos exPos )
{
  u8 num = BTL_MAIN_ExpandBtlPos( mainModule, exPos, stw->pos );
  stw->confirmCount = num;
  stw->confirmType = STW_CONFIRM_FIELD;
}
//
static BOOL stw_is_enable_hitpos( SEL_TARGET_WORK* stw, int hitPos, const BTL_MAIN_MODULE* mainModule, u8* target_idx )
{
  GF_ASSERT_MSG(hitPos<NELEMS(STW_HitTblIndex), "hitPos=%d", hitPos);

  if( stw->selectablePokeCount )
  {
    u8 hitVpos = STW_HitTblIndex[ hitPos ];
    u8 vpos, i;

    for(i=0; i<stw->selectablePokeCount; ++i)
    {
      vpos = BTL_MAIN_BtlPosToViewPos( mainModule, stw->pos[i] );
      if( vpos == hitVpos )
      {
        *target_idx = stw->pos[i];
        return TRUE;
      }
    }
    return FALSE;
  }
  else
  {
    // 確認のみモードの場合、どこをタッチしてもOKにしておく
    *target_idx = 0;
    return TRUE;
  }
}
//
static inline u8 stwdraw_vpos_to_tblidx( u8 vpos )
{
  u8 i;
  for(i=0; i<NELEMS(STW_HitTblIndex); ++i)
  {
    if( STW_HitTblIndex[i] == vpos )
    {
      return i;
    }
  }
  GF_ASSERT(0);
  return 0;
}

static void stwdraw_button( const u8* pos, u8 count, u8 format, BTLV_SCD* wk )
{
#if 0
  const BTL_POKEPARAM* bpp;
  const POKEMON_PARAM* pp;
  BINPUT_SCENE_POKE bsp;
  u8 vpos;
  WazaID waza = wk->destActionParam->fight.waza;

  MI_CpuClear16( &bsp, sizeof( BINPUT_SCENE_POKE ) );

  bsp.pokesele_type = WAZADATA_GetTarget( waza );
  bsp.client_type = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, BTL_POKEPARAM_GetID(wk->bpp) );

  while( count-- )
  {
    bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, *pos );
    pp  = BTL_POKEPARAM_GetSrcData( bpp );
    vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, *pos );

    bsp.dspp[ *pos ].hp = PP_Get( pp, ID_PARA_hp, NULL );
    if( bsp.dspp[ *pos ].hp )
    {
      int mons_no = PP_Get( pp, ID_PARA_monsno, NULL );
      bsp.dspp[ *pos ].pp = pp;
      bsp.dspp[ *pos ].hpmax = PP_Get( pp, ID_PARA_hpmax, NULL );
      bsp.dspp[ *pos ].exist = 1;
      if( ( mons_no == MONSNO_NIDORAN_M ) || ( mons_no == MONSNO_NIDORAN_F ) )
      {
        bsp.dspp[ *pos ].sex = PTL_SEX_UNKNOWN;
      }
      else
      {
        bsp.dspp[ *pos ].sex = PP_Get( pp, ID_PARA_sex, NULL );
      }

      if( PP_Get( pp, ID_PARA_condition, NULL ) )
      {
        bsp.dspp[ *pos ].status = COMSEL_STATUS_NG;
      }
      else
      {
        bsp.dspp[ *pos ].status = COMSEL_STATUS_ALIVE;
      }
    }
    else
    {
      bsp.dspp[ *pos ].exist = 0;
      bsp.dspp[ *pos ].status = COMSEL_STATUS_DEAD;
    }
    pos++;
  }

  {
    ARCHANDLE* hdl_bg;
    ARCHANDLE* hdl_obj;

    hdl_bg  = GFL_ARC_OpenDataHandle( ARCID_BATT_BG,  wk->heapID );
    hdl_obj = GFL_ARC_OpenDataHandle( ARCID_BATT_OBJ, wk->heapID );
    BINPUT_CreateBG( hdl_bg, hdl_obj, wk->bip, BINPUT_TYPE_POKE, FALSE, &bsp );

    GFL_ARC_CloseDataHandle( hdl_bg );
    GFL_ARC_CloseDataHandle( hdl_obj );
  }
  const BTL_POKEPARAM*  bpp;
  const POKEMON_PARAM*  pp;
  BTLV_INPUT_SCENE_POKE bisp;
  u8 vpos;
  WazaID waza = wk->destActionParam->fight.waza;

  MI_CpuClear16( &bisp, sizeof( BTLV_INPUT_SCENE_POKE ) );

  bisp.pokesele_type = WAZADATA_GetTarget( waza );
  bisp.client_type = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, BTL_POKEPARAM_GetID(wk->bpp) );

  while( count-- )
  {
    bpp = BTL_POKECON_GetFrontPokeDataConst( wk->pokeCon, *pos );
    pp  = BTL_POKEPARAM_GetSrcData( bpp );
    vpos = BTL_MAIN_BtlPosToViewPos( wk->mainModule, *pos );

    bisp.bidp[ *pos ].hp = PP_Get( pp, ID_PARA_hp, NULL );
    if( bsp.dspp[ *pos ].hp )
    {
      int mons_no = PP_Get( pp, ID_PARA_monsno, NULL );
      bsp.dspp[ *pos ].pp = pp;
      bsp.dspp[ *pos ].hpmax = PP_Get( pp, ID_PARA_hpmax, NULL );
      bsp.dspp[ *pos ].exist = 1;
      if( ( mons_no == MONSNO_NIDORAN_M ) || ( mons_no == MONSNO_NIDORAN_F ) )
      {
        bsp.dspp[ *pos ].sex = PTL_SEX_UNKNOWN;
      }
      else
      {
        bsp.dspp[ *pos ].sex = PP_Get( pp, ID_PARA_sex, NULL );
      }

      if( PP_Get( pp, ID_PARA_condition, NULL ) )
      {
        bsp.dspp[ *pos ].status = COMSEL_STATUS_NG;
      }
      else
      {
        bsp.dspp[ *pos ].status = COMSEL_STATUS_ALIVE;
      }
    }
    else
    {
      bsp.dspp[ *pos ].exist = 0;
      bsp.dspp[ *pos ].status = COMSEL_STATUS_DEAD;
    }
    pos++;
  }

  {
    ARCHANDLE* hdl_bg;
    ARCHANDLE* hdl_obj;

    hdl_bg  = GFL_ARC_OpenDataHandle( ARCID_BATT_BG,  wk->heapID );
    hdl_obj = GFL_ARC_OpenDataHandle( ARCID_BATT_OBJ, wk->heapID );
    BINPUT_CreateBG( hdl_bg, hdl_obj, wk->bip, BINPUT_TYPE_POKE, FALSE, &bsp );

    GFL_ARC_CloseDataHandle( hdl_bg );
    GFL_ARC_CloseDataHandle( hdl_obj );
  }
#endif
}
static void stw_draw( const SEL_TARGET_WORK* stw, BTLV_SCD* work )
{
  // ポケモン選択モード
  if( stw->selectablePokeCount )
  {
    stwdraw_button( stw->pos, stw->selectablePokeCount, 0, work );
  }
  // 確認のみ
  else
  {
    GF_ASSERT(stw->confirmCount);
    if( stw->confirmType == STW_CONFIRM_POKE )
    {
      stwdraw_button( stw->pos, stw->confirmCount, 1, work );
    }
    else
    {
      stwdraw_button( stw->pos, stw->confirmCount, 2, work );
    }
  }
}
static BOOL stw_draw_wait( BTLV_SCD* wk )
{
  PRINTSYS_QUE_Main( wk->printQue );
  return PRINT_UTIL_Trans( &wk->printUtil, wk->printQue );
}


//=============================================================================================
//  ワザターゲット選択（確認）処理
//=============================================================================================
static BOOL selectTarget_init( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;

//  Sub_TouchEndDelete( wk->bip, TRUE, TRUE );

  seltgt_init_setup_work( &wk->selTargetWork, wk );
  stw_draw( &wk->selTargetWork, wk );
  return TRUE;
}
static BOOL selectTarget_loop( int* seq, void* wk_adrs )
{
  BTLV_SCD* wk = wk_adrs;

  switch( *seq ){
  case 0:
    {
      int hit = GFL_UI_TP_HitTrg( PokeSeleMenuTouchData );
      if( hit != GFL_UI_TP_HIT_NONE )
      {
//        Sub_TouchEndDelete( wk->bip, TRUE, TRUE );
        if( hit < BTL_CLIENT_MAX )
        {
          u8 target_idx;
          SePlayDecide();
          if( stw_is_enable_hitpos( &wk->selTargetWork, hit, wk->mainModule, &target_idx ) )
          {
            BTL_Printf("ターゲット決定 ... hitBtn=%d, hitPos=%d, target_idx=%d\n", hit, wk->selTargetWork.pos[hit], target_idx);
            BTL_ACTION_SetFightParam( wk->destActionParam, wk->destActionParam->fight.waza, target_idx );
            wk->selTargetDone = TRUE;
            return TRUE;
          }
        }
        else
        {
          wk->selTargetDone = FALSE;
          return TRUE;
        }
      }
    }
    break;
  }
  return FALSE;
}

static void seltgt_init_setup_work( SEL_TARGET_WORK* stw, BTLV_SCD* wk )
{
  WazaID waza = wk->destActionParam->fight.waza;
  WazaTarget target = WAZADATA_GetTarget( waza );
  BtlPokePos  basePos = BTL_MAIN_PokeIDtoPokePosClient( wk->mainModule, BTL_POKEPARAM_GetID(wk->bpp) );

  stw_init( stw );

  switch( target ){
  case WAZA_TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    stw_setSelectablePoke( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_WITHOUT_ME, basePos) );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
    stw_setSelectablePoke( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_ENEMY_ALL, basePos) );
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< 敵側２体
  case WAZA_TARGET_ENEMY_RANDOM:        ///< 敵ランダム
    stw_setConfirmPoke( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_ENEMY_ALL, basePos) );
    break;
  case WAZA_TARGET_OTHER_ALL:     ///< 自分以外全部
    stw_setConfirmPoke( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_WITHOUT_ME, basePos) );
    break;
  case WAZA_TARGET_USER:      ///< 自分１体のみ
    stw_setConfirmPoke( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_DEFAULT, basePos) );
    break;

  case WAZA_TARGET_SIDE_FRIEND:     ///< 自分側陣地
    stw_setConfirmField( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_MYSIDE_ALL, basePos) );
    break;

  case WAZA_TARGET_SIDE_ENEMY:    ///< 敵側陣地
    stw_setConfirmField( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_ENEMY_ALL, basePos) );
    break;

  case WAZA_TARGET_FIELD:         ///< 場に効く（天候系など）
    stw_setConfirmField( stw, wk->mainModule, EXPOS_MAKE(BTL_EXPOS_ALL, basePos) );
    break;

  case WAZA_TARGET_UNKNOWN:     ///< 特殊系（ゆびをふる等）
    // @@@ 未実装
    break;

  default:
    GF_ASSERT_MSG(0, "illegal target(%d)", target);
    break;
  }
}


//=============================================================================================
//  ポケモン選択処理
//=============================================================================================


void BTLV_SCD_PokeSelect_Start( BTLV_SCD* wk, const BTL_POKESELECT_PARAM* param, BTL_POKESELECT_RESULT* result )
{
  GF_ASSERT( wk->pokesel_param == NULL );

  BTLV_INPUT_CreateScreen( wk->biw, BTLV_INPUT_SCRTYPE_STANDBY, NULL );

  wk->pokesel_param = param;
  wk->pokesel_result = result;
  BTL_Printf("ポケモン選択開始 : %d 体えらぶ\n", param->numSelect );
  spstack_push( wk, selectPokemon_init, selectPokemon_loop );
}
BOOL BTLV_SCD_PokeSelect_Wait( BTLV_SCD* wk )
{
  return spstack_call( wk );
}

// ポケモン選択画面：構築
static BOOL selectPokemon_init( int* seq, void* wk_adrs )
{
  static const struct {
    u8 x;
    u8 y;
  }winpos[] = {
    { POKEWIN_1_X, POKEWIN_1_Y },
    { POKEWIN_2_X, POKEWIN_2_Y },
    { POKEWIN_3_X, POKEWIN_3_Y },
    { POKEWIN_4_X, POKEWIN_4_Y },
    { POKEWIN_5_X, POKEWIN_5_Y },
    { POKEWIN_6_X, POKEWIN_6_Y },

    { POKEWIN_BACK_X, POKEWIN_BACK_Y },
  };

  BTLV_SCD* wk = wk_adrs;

  switch(*seq) {
  case 0:
    {
      const BTL_PARTY* party;
      const BTL_POKEPARAM* bpp;
      u16 members, hp, i;
      u8 col;

      GFL_BMP_Clear( wk->bmp, 0xff );

      party = wk->pokesel_param->party;
      members = BTL_PARTY_GetMemberCount( party );

      for(i=0; i<members; i++)
      {
        bpp = BTL_PARTY_GetMemberDataConst( party, i );
        hp = BTL_POKEPARAM_GetValue( bpp, BPP_HP );
        col = (hp)? 6 : 4;
        BTL_STR_MakeStatusWinStr( wk->strbuf, bpp, hp );
        GFL_BMP_Fill( wk->bmp, winpos[i].x, winpos[i].y, POKEWIN_WIDTH, POKEWIN_HEIGHT, col );
        PRINT_UTIL_Print( &wk->printUtil, wk->printQue, winpos[i].x+2, winpos[i].y+2, wk->strbuf, wk->font );
      }

      // [もどる]ボタン描画
      i = NELEMS(winpos) - 1;
      BTL_STR_GetUIString( wk->strbuf, BTLMSG_UI_SEL_BACK );
      GFL_BMP_Fill( wk->bmp, winpos[i].x, winpos[i].y, POKEWIN_WIDTH, POKEWIN_HEIGHT, 0x0a );
      PRINT_UTIL_Print( &wk->printUtil, wk->printQue, winpos[i].x+2, winpos[i].y+2, wk->strbuf, wk->font );

      GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
      (*seq)++;
    }
    break;

  case 1:
    PRINTSYS_QUE_Main( wk->printQue );
    if( PRINT_UTIL_Trans(&wk->printUtil, wk->printQue) )
    {
      GFL_BMPWIN_MakeScreen( wk->win );
      GFL_BMPWIN_TransVramCharacter( wk->win );
      GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
      return TRUE;
    }
    break;
  }

  return FALSE;
}

// ポケモン選択画面：動作
static BOOL selectPokemon_loop( int* seq, void* wk_adrs )
{
  enum {
    SEQ_INIT = 0,
    SEQ_WAIT_SELECT,
    SEQ_WARNING_START,
    SEQ_WARNING_WAIT_PRINT,
    SEQ_WARNING_WAIT_AGREE,
  };

  enum {
    POKEWIN_1_BTM = POKEWIN_1_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_1_RGT = POKEWIN_1_X+POKEWIN_WIDTH - 1,
    POKEWIN_2_BTM = POKEWIN_2_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_2_RGT = POKEWIN_2_X+POKEWIN_WIDTH - 1,
    POKEWIN_3_BTM = POKEWIN_3_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_3_RGT = POKEWIN_3_X+POKEWIN_WIDTH - 1,
    POKEWIN_4_BTM = POKEWIN_4_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_4_RGT = POKEWIN_4_X+POKEWIN_WIDTH - 1,
    POKEWIN_5_BTM = POKEWIN_5_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_5_RGT = POKEWIN_5_X+POKEWIN_WIDTH - 1,
    POKEWIN_6_BTM = POKEWIN_6_Y+POKEWIN_HEIGHT - 1,
    POKEWIN_6_RGT = POKEWIN_6_X+POKEWIN_WIDTH - 1,

    POKEWIN_BACK_BTM = POKEWIN_BACK_Y + POKEWIN_HEIGHT-1,
    POKEWIN_BACK_RGT = POKEWIN_BACK_X + POKEWIN_WIDTH-1,

    WARNWIN_WIDTH = 256-(8*8),
    WARNWIN_HEIGHT = 32,
    WARNWIN_X = 8,
    WARNWIN_Y = 192-8-16-WARNWIN_HEIGHT,
  };

  static const GFL_UI_TP_HITTBL hitTbl[] = {
    { POKEWIN_1_Y, POKEWIN_1_BTM, POKEWIN_1_X, POKEWIN_1_RGT },
    { POKEWIN_2_Y, POKEWIN_2_BTM, POKEWIN_2_X, POKEWIN_2_RGT },
    { POKEWIN_3_Y, POKEWIN_3_BTM, POKEWIN_3_X, POKEWIN_3_RGT },
    { POKEWIN_4_Y, POKEWIN_4_BTM, POKEWIN_4_X, POKEWIN_4_RGT },
    { POKEWIN_5_Y, POKEWIN_5_BTM, POKEWIN_5_X, POKEWIN_5_RGT },
    { POKEWIN_6_Y, POKEWIN_6_BTM, POKEWIN_6_X, POKEWIN_6_RGT },
    { POKEWIN_BACK_Y, POKEWIN_BACK_BTM, POKEWIN_BACK_X, POKEWIN_BACK_RGT },
    { GFL_UI_TP_HIT_END, 0, 0, 0 },
  };

  BTLV_SCD* wk = wk_adrs;
  const BTL_POKESELECT_PARAM* param = wk->pokesel_param;
  BTL_POKESELECT_RESULT *res = wk->pokesel_result;

  switch( *seq ){
  case SEQ_INIT:
    BTL_POKESELECT_RESULT_Init( res, param );
    (*seq) = SEQ_WAIT_SELECT;

    /* fallthru */

  // ポケモン名ウィンドウのタッチ選択待ち
  case SEQ_WAIT_SELECT:
    {
      int hitpos = GFL_UI_TP_HitTrg( hitTbl );
      if( hitpos != GFL_UI_TP_HIT_NONE )
      {
        const BTL_PARTY* party = param->party;

        if( hitpos == (NELEMS(hitTbl)-2) ){
          SePlayCancel();
          BTL_POKESELECT_RESULT_Init( res, param );
          wk->pokesel_param = NULL;
          wk->pokesel_result = NULL;
          GFL_BMP_Clear( wk->bmp, 0x00 );
          GFL_BMPWIN_TransVramCharacter( wk->win );
          GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
          return TRUE;
        }
        if( hitpos < BTL_PARTY_GetMemberCount(party) )
        {
          const BTL_POKEPARAM* bpp;
          BtlPokeselReason  result;

          bpp = BTL_PARTY_GetMemberDataConst( party, hitpos );
          result = BTL_POKESELECT_CheckProhibit( param, res, hitpos );

          switch( result ){
          default:
            GF_ASSERT(0);
          case BTL_POKESEL_CANT_FIGHTING:
            BTL_STR_MakeWarnStr( wk->strbuf, bpp, BTLSTR_UI_WARN_FightingPoke );
            break;

          case BTL_POKESEL_CANT_DEAD:
            BTL_STR_MakeWarnStr( wk->strbuf, bpp, BTLSTR_UI_WARN_NotAlivePoke );
            break;

          case BTL_POKESEL_CANT_SELECTED:
            BTL_STR_MakeWarnStr( wk->strbuf, bpp, BTLSTR_UI_WARN_SelectedPoke );
            break;

          case BTL_POKESEL_CANT_NONE:
            // --- ここまで来たら正しいポケを選んでる ---
            BTL_POKESELECT_RESULT_Push( res, hitpos );
            if( BTL_POKESELECT_IsDone(res) )
            {
              SePlayDecide();
              // 次回に正しく初期化されずに呼び出されたら止まるようにNULLクリアしておく
              wk->pokesel_param = NULL;
              wk->pokesel_result = NULL;
              GFL_BMP_Clear( wk->bmp, 0x00 );
              GFL_BMPWIN_TransVramCharacter( wk->win );
              GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
              return TRUE;
            }
            else
            {
              return FALSE;
            }
          }
          // --- ここにくるのは警告メッセージ表示シーケンス ---
          (*seq) = SEQ_WARNING_START;
        }
      }
    }
    break;

  // 警告表示中（タッチ待ち）
  case SEQ_WARNING_START:
    PRINT_UTIL_Print( &wk->printUtil, wk->printQue, WARNWIN_X, WARNWIN_Y, wk->strbuf, wk->font );
    (*seq) = SEQ_WARNING_WAIT_PRINT;
    /* fallthru */
  case SEQ_WARNING_WAIT_PRINT:
    PRINTSYS_QUE_Main( wk->printQue );
    if( PRINT_UTIL_Trans(&wk->printUtil, wk->printQue) )
    {
      (*seq) = SEQ_WARNING_WAIT_AGREE;
    }
    break;
  case SEQ_WARNING_WAIT_AGREE:
    if( GFL_UI_TP_GetTrg()
    ||  GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A)
    ){
        GFL_BMP_Fill( wk->bmp, WARNWIN_X, WARNWIN_Y, WARNWIN_WIDTH, WARNWIN_HEIGHT, 0x0f );
        GFL_BMPWIN_TransVramCharacter( wk->win );
        (*seq) = SEQ_WAIT_SELECT;
    }
    break;
  }

  return FALSE;
}

//=============================================================================================
//  「通信待機中」表示オン・オフ
//=============================================================================================

void BTLV_SCD_StartCommWaitInfo( BTLV_SCD* wk )
{
  printCommWait( wk );
}
BOOL BTLV_SCD_WaitCommWaitInfo( BTLV_SCD* wk )
{
  PRINTSYS_QUE_Main( wk->printQue );
  if( PRINT_UTIL_Trans(&wk->printUtil, wk->printQue) )
  {
    return TRUE;
  }
  return FALSE;
}
void BTLV_SCD_ClearCommWaitInfo( BTLV_SCD* wk )
{
  GFL_BMP_Clear( wk->bmp, 0x0f );
  GFL_BMPWIN_TransVramCharacter( wk->win );
}

static void printCommWait( BTLV_SCD* wk )
{
  u32 strWidth, drawX, drawY;

  GFL_BMP_Clear( wk->bmp, 0x0f );

  BTL_STR_GetUIString( wk->strbuf, BTLSTR_UI_COMM_WAIT );
  strWidth = PRINTSYS_GetStrWidth( wk->strbuf, wk->font, 0 );
  drawX = (256 - strWidth) / 2;
  drawY = (192 - 16) / 2;
  PRINT_UTIL_Print( &wk->printUtil, wk->printQue, drawX, drawY, wk->strbuf, wk->font );
}

//=============================================================================================
//  決定音再生
//=============================================================================================
static  inline  void  SePlayDecide( void )
{
  PMSND_PlaySE( SEQ_SE_DECIDE2 );
}

//=============================================================================================
//  キャンセル音再生
//=============================================================================================
static  inline  void  SePlayCancel( void )
{
  PMSND_PlaySE( SEQ_SE_CANCEL2 );
}

