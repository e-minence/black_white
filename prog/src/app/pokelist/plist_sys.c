//======================================================================
/**
 * @file	plist_sys.c
 * @brief	ポケモンリスト メイン処理
 * @author	ariizumi
 * @data	09/06/10
 *
 * モジュール名：PLIST
 */
//======================================================================
#include <gflib.h>
#include "buflen.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "net/network_define.h"

#include "arc_def.h"
#include "pokelist_gra.naix"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_pokelist.h"

#include "pokeicon/pokeicon.h"
#include "poke_tool/shinka_check.h"
#include "poke_tool/natsuki.h"
#include "savedata/myitem_savedata.h"
#include "savedata/mail.h"
#include "savedata/mail_util.h"
#include "savedata/regulation.h"
#include "waza_tool/wazano_def.h"
#include "item/item.h"
#include "field/zonedata.h"

#include "plist_sys.h"
#include "plist_plate.h"
#include "plist_message.h"
#include "plist_menu.h"
#include "plist_battle.h"
#include "plist_item.h"
#include "plist_demo.h"
#include "plist_comm.h"
#include "plist_snd_def.h"
#include "poke_tool/status_rcv.h"
#include "poke_tool/item_rcv.h"
#include "poke_tool/poke_regulation.h"

#include "app/p_status.h" //Proc切り替え用
#include "app/app_menu_common.h"
#include "app/app_taskmenu.h"
#include "musical/musical_system.h" //ミュージカル参加チェック用

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PLIST_BARICON_Y (168)
#define PLIST_BARICON_EXIT_X   (200)  //抜ける(×マーク
#define PLIST_BARICON_RETURN_X_MENU (224)  //戻る(矢印
#define PLIST_BARICON_RETURN_X_BAR (224)  //戻る(矢印

#define PLIST_CHANGE_ANM_COUNT (16)
#define PLIST_CHANGE_ANM_VALUE (1)    //移動量(キャラ単位

//バトルメニュー用ボタン座標
#define PLIST_BATTLE_BUTTON_DECIDE_X (12)
#define PLIST_BATTLE_BUTTON_DECIDE_Y (21)
#define PLIST_BATTLE_BUTTON_CANCEL_X (22)
#define PLIST_BATTLE_BUTTON_CANCEL_Y (21)

#define PLIST_PLATE_ACTIVE_ANM_CNT (12)
#define PLIST_PLATE_ACTIVE_ANM_CNT_HALF (6)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//サブとなるシーケンス(work->subSeq
typedef enum
{
  PSSS_INIT,
  PSSS_MAIN,
  PSSS_TERM,
  
  //入れ替えアニメ用
  PSSS_MOVE_OUT,
  PSSS_MOVE_IN,

  //Proc切り替え用
  PSSS_FADEOUT,
  PSSS_FADEIN,

  PSSS_MAX,
}PLIST_SYS_SUB_SEQ;

//選択状態
typedef enum
{
  PSSEL_DECIDE, //決定を押した(参加選択とか)
  PSSEL_RETURN, //キャンセル(B)
  PSSEL_EXIT, //戻る(X)

  PSSEL_SELECT, //ポケモンを選んだ
  PSSEL_NONE, //選択中

  PSSEL_BUTTON_NUM = PSSEL_EXIT+1,  //TP判定でタッチテーブルに使う
  PSSEL_MAX,  
}PLIST_SYS_SELECT_STATE;


//カーソルのアニメ
enum PLIST_CURCOR_ANIME
{
  PCA_NORMAL_A, //通常0番用
  PCA_NORMAL_B, //通常
  PCA_CHANGE_A, //入れ替え0番用
  PCA_CHANGE_B, //入れ替え
  PCA_SELECT_A, //選択0番用
  PCA_SELECT_B, //選択
  PCA_MENU,   //メニュー用
};

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto


FS_EXTERN_OVERLAY(poke_status);

static void PLIST_UpdatePlatePalletAnime( PLIST_WORK *work );
static void PLIST_VBlankFunc(GFL_TCB *tcb, void *wk );

//グラフィック系
static void PLIST_InitGraphic( PLIST_WORK *work );
static void PLIST_TermGraphic( PLIST_WORK *work );
static void PLIST_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );

static void PLIST_InitCell( PLIST_WORK *work );
static void PLIST_TermCell( PLIST_WORK *work );

static void PLIST_InitMessage( PLIST_WORK *work );
static void PLIST_TermMessage( PLIST_WORK *work );

//リソース系
static void PLIST_LoadResource( PLIST_WORK *work );
static void PLIST_ReleaseResource( PLIST_WORK *work );

//モード別処理系
static void PLIST_InitMode( PLIST_WORK *work );
static void PLIST_InitMode_Select( PLIST_WORK *work );
static void PLIST_TermMode_Select_Decide( PLIST_WORK *work );
static void PLIST_InitMode_Menu( PLIST_WORK *work );

//ポケモン選択
static void PLIST_SelectPoke( PLIST_WORK *work );
static void PLIST_SelectPokeInit( PLIST_WORK *work );
static void PLIST_SelectPokeTerm( PLIST_WORK *work );
static void PLIST_SelectPokeTerm_Change( PLIST_WORK *work );
static void PLIST_SelectPokeTerm_Use( PLIST_WORK *work );
static void PLIST_SelectPokeTerm_BattleDecide( PLIST_WORK *work );
static void PLIST_SelectPokeMain( PLIST_WORK *work );
static void PLIST_SelectPokeUpdateKey( PLIST_WORK *work );
static void PLIST_SelectPokeUpdateTP( PLIST_WORK *work );
static void PLIST_SelectPokeSetCursor( PLIST_WORK *work , const PL_SELECT_POS pos );
static void PLIST_SelectPokeSetCursor_Change( PLIST_WORK *work , const PL_SELECT_POS pos );
static void PLIST_PLATE_SetActivePlatePos( PLIST_WORK *work , const PL_SELECT_POS pos );
//メニュー選択
static void PLIST_SelectMenu( PLIST_WORK *work );
static void PLIST_SelectMenuInit( PLIST_WORK *work );
static void PLIST_SelectMenuTerm( PLIST_WORK *work );
static void PLIST_SelectMenuExit( PLIST_WORK *work );

//入れ替えアニメ
static void PLIST_ChangeAnime( PLIST_WORK *work );
static void PLIST_ChangeAnimeInit( PLIST_WORK *work );
static void PLIST_ChangeAnimeTerm( PLIST_WORK *work );
static void PLIST_ChangeAnimeUpdatePlate( PLIST_WORK *work );

//メッセージ待ち
static void PLIST_MessageWait( PLIST_WORK *work );

//はい・いいえ
static void PLIST_YesNoWait( PLIST_WORK *work );
static void PLIST_YesNoWaitInit( PLIST_WORK *work , PLIST_CallbackFuncYesNo yesNoCallBack );

//メッセージコールバック
static void PLIST_MSGCB_ForgetSkill_ForgetCheckCB( PLIST_WORK *work , const int retVal );
static void PLIST_MSGCB_ForgetSkill_SkillCancel( PLIST_WORK *work );
static void PLIST_MSGCB_ForgetSkill_SkillCancelCB( PLIST_WORK *work , const int retVal );
static void PLIST_MSGCB_ForgetSkill_SkillForget( PLIST_WORK *work );

static void PLIST_MSGCB_ItemSet_CheckChangeItem( PLIST_WORK *work );
static void PLIST_MSGCB_ItemSet_CheckChangeItemCB( PLIST_WORK *work , const int retVal );

static void PLIST_HPANMCB_SkillRecoverHpFirst( PLIST_WORK *work );
static void PLIST_HPANMCB_SkillRecoverHpSecond( PLIST_WORK *work );

static void PLIST_MSGCB_TakeMail( PLIST_WORK *work );
static void PLIST_MSGCB_TakeMailCB( PLIST_WORK *work , const int retVal );
static void PLIST_MSGCB_TakeMail_Confirm( PLIST_WORK *work );
static void PLIST_MSGCB_TakeMail_ConfirmCB( PLIST_WORK *work , const int retVal );

static void PLIST_MSGCB_FormChange( PLIST_WORK *work );

//外部数値操作
static void PLIST_LearnSkillEmpty( PLIST_WORK *work , POKEMON_PARAM *pp );
static void PLIST_LearnSkillFull( PLIST_WORK *work  , POKEMON_PARAM *pp , u8 pos );
static void PLIST_UseWazaMachine( PLIST_WORK *work  , POKEMON_PARAM *pp );
static void PLIST_SetPokeItem( PLIST_WORK *work , POKEMON_PARAM *pp , u16 itemNo );
static const u16 PLIST_CheckBagItemNum( PLIST_WORK *work , u16 itemNo );
static void PLIST_SubBagItem( PLIST_WORK *work , u16 itemNo );
static void PLIST_AddBagItem( PLIST_WORK *work , u16 itemNo );

//デバッグメニュー
static void PLIST_InitDebug( PLIST_WORK *work );
static void PLIST_TermDebug( PLIST_WORK *work );
//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
const BOOL PLIST_InitPokeList( PLIST_WORK *work )
{
  u8 i;
  u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
  
  work->ktst = GFL_UI_CheckTouchOrKey();
  work->mainSeq = PSMS_FADEIN;
  work->subSeq = PSSS_INIT;
  work->pokeCursor = 0;
  work->selectPokePara = NULL;
  work->menuRet = PMIT_NONE;
  work->changeTarget = PL_SEL_POS_MAX;
  work->useTarget = PL_SEL_POS_MAX;
  work->canExit = FALSE;
  work->isActiveWindowMask = FALSE;
  work->btlJoinNum = 0;
  work->platePalAnmCnt = 0;
  work->platePalWakuAnmCnt = 0;
  work->btlMenuAnmCnt = 0;
  work->btlTermAnmCnt = 0;
  work->hpAnimeCallBack = NULL;
  work->paramWin = NULL;
  work->clwkExitButton = NULL;
  work->btlMenuWin[0] = NULL;
  work->btlMenuWin[1] = NULL; 
  work->isDecideParty = FALSE;
  work->demoType = PDT_NONE;
  work->isCallForceExit = FALSE;
  work->isComm = FALSE;
  work->plData->isNetErr = FALSE;
  work->reqPlaySe = FALSE;
  work->canSelectEgg = TRUE;

  for( i=0;i<PCR_MAX;i++ )
  {
    work->cellRes[i] = PCR_NONE;
  }

  //特殊モード分岐
  if( work->plData->mode == PL_MODE_SET_MUSICAL )
  {
    //work->plData->mode = PL_MODE_SET;
    work->isSetMusicalMode = TRUE;
  }
  else
  {
    work->isSetMusicalMode = FALSE;
  }
  

  if( work->plData->mode == PL_MODE_SET_WAZA )
  {
    work->plData->mode = PL_MODE_SET;
    work->isSetWazaMode = TRUE;
  }
  else
  {
    work->isSetWazaMode = FALSE;
  }

  if( work->plData->mode == PL_MODE_BATTLE_WIFI )
  {
    work->plData->mode = PL_MODE_BATTLE;
    work->isFinishSync = TRUE;
    //PLIST_COMM_InitComm( work );
    
  }
  else
  {
    work->isFinishSync = FALSE;
  }
  
  if( work->plData->mode == PL_MODE_WAZASET )
  {
    if( work->plData->waza == 0 )
    {
      work->plData->waza = ITEM_GetWazaNo(work->plData->item);
    }
  }
  
  PLIST_InitGraphic( work );
  PLIST_LoadResource( work );
  PLIST_InitMessage( work );
  
  //プレートの作成
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    if( i<partyMax )
    {
      POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, i);
      work->plateWork[i] = PLIST_PLATE_CreatePlate( work , i , pp );
    }
    else
    {
      work->plateWork[i] = PLIST_PLATE_CreatePlate_Blank( work , i );
    }
    
    if( PLIST_PLATE_GetBattleOrder( work->plateWork[i] ) <= PPBO_JOIN_6 )
    {
      work->btlJoinNum++;
    }
  }

	work->taskres	= APP_TASKMENU_RES_Create( PLIST_BG_MENU, PLIST_BG_PLT_MENU_ACTIVE,
									work->fontHandle, work->printQue, work->heapId );

  //描画順の関係上ここで
  PLIST_InitCell( work );
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    PLIST_BATTLE_InitBattle( work );
  }
  
//  PLIST_PLATE_ChangeColor( work , work->plateWork[0] , PPC_NORMAL_SELECT );
  work->vBlankTcb = GFUser_VIntr_CreateTCB( PLIST_VBlankFunc , work , 8 );

  //メッセージシステム生成
  work->msgWork = PLIST_MSG_CreateSystem( work );

  //メニューシステム生成
  work->menuWork = PLIST_MENU_CreateSystem( work );

  
  //モード別初期化
  PLIST_InitMode( work );

  //まとめて読み込む
  GFL_BG_LoadScreenV_Req(PLIST_BG_MAIN_BG);
  GFL_BG_LoadScreenV_Req(PLIST_BG_PLATE);
  GFL_BG_LoadScreenV_Req(PLIST_BG_SUB_BG);

  GFL_NET_ReloadIconTopOrBottom(TRUE , work->heapId );

  
#if USE_DEBUGWIN_SYSTEM
  PLIST_InitDebug( work );
#endif
  return TRUE;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
const BOOL PLIST_TermPokeList( PLIST_WORK *work )
{
  u8 i;
#if USE_DEBUGWIN_SYSTEM
  PLIST_TermDebug( work );
#endif

  GFL_TCB_DeleteTask( work->vBlankTcb );
  
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    PLIST_MENU_DeleteMenuWin_BattleMenu( work->btlMenuWin[0] );
    PLIST_MENU_DeleteMenuWin_BattleMenu( work->btlMenuWin[1] );
    work->btlMenuWin[0] = NULL;
    work->btlMenuWin[1] = NULL;
  }
  
  PLIST_MENU_DeleteSystem( work , work->menuWork );
  PLIST_MSG_DeleteSystem( work , work->msgWork );
  
  if( work->paramWin != NULL )
  {
    GFL_BMPWIN_Delete( work->paramWin );
  }

  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    PLIST_PLATE_DeletePlate( work , work->plateWork[i] );
  }

	APP_TASKMENU_RES_Delete( work->taskres );

  PLIST_TermMessage( work );
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    PLIST_BATTLE_TermBattle( work );
  }
  PLIST_TermCell( work );
  PLIST_ReleaseResource( work );
  PLIST_TermGraphic( work );

  //特殊モード戻し
  if( work->isSetWazaMode == TRUE )
  {
    work->plData->mode = PL_MODE_SET_WAZA;
  }
  if( work->isSetMusicalMode == TRUE )
  {
    work->plData->mode = PL_MODE_SET_MUSICAL;
  }
  if( work->isFinishSync == TRUE )
  {
    work->plData->mode = PL_MODE_BATTLE_WIFI;
  }


  GFL_UI_SetTouchOrKey( work->ktst );
  
  return TRUE;
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
const BOOL PLIST_UpdatePokeList( PLIST_WORK *work )
{
  u8 i;

  switch( work->mainSeq )
  {
  case PSMS_FADEIN:
//    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_SHUTTERIN_DOWN , WIPE_TYPE_SHUTTERIN_DOWN , 
//                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    WIPE_SYS_Start( WIPE_PATTERN_FSAM , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = PSMS_FADEIN_WAIT;
    break;
  
  case PSMS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->mainSeq = work->nextMainSeq;
    }
    break;
    
  case PSMS_SELECT_POKE:
  case PSMS_CHANGE_POKE:  //基本は一緒だから同じ処理で
  case PSMS_USE_POKE:
    PLIST_SelectPoke( work );
    break;

  case PSMS_CHANGE_ANM:
    PLIST_ChangeAnime( work );
    break;
    
  case PSMS_MENU:
    PLIST_SelectMenu( work );
    break;
  
  case PSMS_MSG_WAIT:
    PLIST_MessageWait( work );
    break;

  case PSMS_YESNO_WAIT:
    PLIST_YesNoWait( work );
    break;

  case PSMS_INIT_HPANIME:
    PLIST_PALTE_InitHpAnime( work , work->plateWork[work->pokeCursor] );
    if( work->reqPlaySe == TRUE )
    {
      work->reqPlaySe = FALSE;
      PMSND_PlaySystemSE( PLIST_SND_RECOVER );
    }
    work->mainSeq = PSMS_HPANIME;
    //break;    BreakThrough
  case PSMS_HPANIME:
    if( PLIST_PALTE_UpdateHpAnime( work , work->plateWork[work->pokeCursor] ) == TRUE )
    {
      //アニメ終了
      if( work->hpAnimeCallBack != NULL )
      {
        work->hpAnimeCallBack( work );
      }
      else
      {
        //アサートいる？
        work->mainSeq = PSMS_FADEOUT_WAIT;
      }
    }
    break;
    
    //バトル時の決定アニメ
  case PSMS_BATTLE_ANM_WAIT:
    {
      //決定時アニメ
      work->btlTermAnmCnt++;
      if( work->btlTermAnmCnt > APP_TASKMENU_ANM_CNT )
      {
        if( work->isFinishSync == TRUE )
        {
          const BOOL ret = PLIST_COMM_SendFlg( work->plData , PCFT_FINISH_SELECT , 0 );
          if( ret == TRUE )
          {
            //通信同期をとる
            work->mainSeq = PSMS_FINISH_SYNC_INIT;
            //メニューを消す
            PLIST_MENU_DeleteMenuWin_BattleMenu( work->btlMenuWin[0] );
            PLIST_MENU_DeleteMenuWin_BattleMenu( work->btlMenuWin[1] );
            work->btlMenuWin[0] = NULL;
            work->btlMenuWin[1] = NULL;
            PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
            PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_13_07 );
            PLIST_MSG_DispTimerIcon( work , work->msgWork );
          }
        }
        else
        {
          work->mainSeq = PSMS_FADEOUT;
        }
      }
    }
    break;
    
  case PSMS_DISP_PARAM:
    PLIST_UpdateDispParam( work );
    break;

  case PSMS_FORM_CHANGE_INIT:
    PLIST_DEMO_DemoInit( work );
    break;

  case PSMS_FORM_CHANGE_MAIN:
    PLIST_DEMO_DemoMain( work );
    break;

  case PSMS_FORM_CHANGE_TERM:
    PLIST_DEMO_DemoTerm( work );
    break;

  case PSMS_FINISH_SYNC_INIT:  //Wifiバトル終了通信同期
    {
      PLIST_COMM_SendTimming( work->plData , PLIST_COMM_TIMMIN_EXIT_LIST );
      work->mainSeq = PSMS_FINISH_SYNC_WAIT;
    }
    break;

  case PSMS_FINISH_SYNC_WAIT:
    {
      if( PLIST_COMM_CheckTimming( work->plData , PLIST_COMM_TIMMIN_EXIT_LIST ) == TRUE )
      {
        work->mainSeq = PSMS_FADEOUT;
      }
    }
    
    break;

  case PSMS_FADEOUT_FORCE:
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
    work->mainSeq = PSMS_FADEOUT_WAIT;
    break;

  case PSMS_FADEOUT:
    if( work->clwkExitButton == NULL ||
        GFL_CLACT_WK_CheckAnmActive( work->clwkExitButton ) == FALSE )
    {
  //    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_SPLITOUT_VSIDE , WIPE_TYPE_SPLITOUT_VSIDE , 
  //                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      work->mainSeq = PSMS_FADEOUT_WAIT;
    }
    break;
  
  case PSMS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return TRUE;
    }
    break;
  }
  
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    PLIST_PLATE_UpdatePlate( work , work->plateWork[i] );
  }
  PLIST_MSG_UpdateSystem( work , work->msgWork );
  
  PLIST_UpdatePlatePalletAnime( work );

  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    PLIST_BATTLE_UpdateBattle( work );
    
    PLIST_MENU_UpdateMenuWin_BattleMenu( work->btlMenuWin[0] );
    PLIST_MENU_UpdateMenuWin_BattleMenu( work->btlMenuWin[1] );
  }

  //メッセージ
  PRINTSYS_QUE_Main( work->printQue );

  //OBJの更新
  GFL_CLACT_SYS_Main();
  
  //通信チェックは外で
  //PLIST_COMM_UpdateComm( work );

#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
    {
      u8 i;
      const u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
      for( i=0;i<partyMax;i++ )
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, i);
        PP_Put( pp , ID_PARA_hp , 1 );
        PP_Put( pp , ID_PARA_pp1 , 1 );
        PP_SetSick( pp , POKESICK_DOKU );
      }
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      u8 i;
      const u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
      for( i=0;i<partyMax;i++ )
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, i);
        PP_Put( pp , ID_PARA_hp , 0 );
      }
    }
  }
#endif
  
  return FALSE;
}

//--------------------------------------------------------------
//	選択中のパレットのフラッシュアニメ更新
//--------------------------------------------------------------
static void PLIST_UpdatePlatePalletAnime( PLIST_WORK *work )
{
  //プレートアニメ
  //選択フラッシュ
  GFL_STD_MemCopy16( work->platePalAnm , work->platePalTrans , 16*2*PLIST_MENU_ANIME_NUM );

  if( work->platePalAnmCnt > 0 )
  {
    u8 anmRate;
    u8 i,j;
    work->platePalAnmCnt--; //最終的に0にするので先にデクリメント
    if( work->platePalAnmCnt > PLIST_PLATE_ACTIVE_ANM_CNT_HALF )
    {
      anmRate = PLIST_PLATE_ACTIVE_ANM_CNT_HALF-(work->platePalAnmCnt-PLIST_PLATE_ACTIVE_ANM_CNT_HALF);
    }
    else
    {
      anmRate = work->platePalAnmCnt;
    }
    for( j=0;j<PLIST_MENU_ANIME_NUM;j++ )
    {
      for( i=0;i<16;i++ )
      {
        if( i<2 || i>4 )
        {
          u8 r = (work->platePalAnm[j][i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
          u8 g = (work->platePalAnm[j][i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
          u8 b = (work->platePalAnm[j][i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
    //      r = (r<anmRate*2?0:r-(anmRate*2));
    //      g = (g<anmRate*2?0:g-(anmRate*2));
    //      b = (b<anmRate*2?0:b-(anmRate*2));
          r = (r+anmRate*2 > 31?31:r+(anmRate*2));
          g = (g+anmRate*2 > 31?31:g+(anmRate*2));
          b = (b+anmRate*2 > 31?31:b+(anmRate*2));
          work->platePalTrans[j][i] = GX_RGB(r,g,b);
        }
      }
    }
  }
  //枠明滅
  {
    const u16 startCol[3] = {0x7ff4,0x7fff,0x7be9};
    const u16 endCol[3]   = {0x4a0a,0x5ef7,0x4140};

    const u16 anmSpd = 0x10000/64;
    if( work->isActiveWindowMask == FALSE )
    {
      if( work->platePalWakuAnmCnt + anmSpd >= 0x10000 )
      {
        work->platePalWakuAnmCnt = work->platePalWakuAnmCnt + anmSpd - 0x10000;
      }
      else
      {
        work->platePalWakuAnmCnt = work->platePalWakuAnmCnt + anmSpd;
      }
    }
    else
    {
      work->platePalWakuAnmCnt = 0xc000;
    }
    {
      u8 i,j;
      const fx32 sin = (FX_SinIdx( work->platePalWakuAnmCnt )+FX32_ONE)/2;
      for( j=0;j<PLIST_MENU_ANIME_NUM;j++ )
      {
        for( i=0;i<3;i++ )
        {
          const u8 sr = (startCol[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
          const u8 sg = (startCol[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
          const u8 sb = (startCol[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
          const u8 er = (endCol[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
          const u8 eg = (endCol[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
          const u8 eb = (endCol[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
          const s8 or = ((er-sr)*sin)>>FX32_SHIFT;
          const s8 og = ((eg-sg)*sin)>>FX32_SHIFT;
          const s8 ob = ((eb-sb)*sin)>>FX32_SHIFT;
          //0〜31の丸め処理
          const u8 r = (sr+or>31 ? 31 : (sr+or<0 ? 0 : sr+or) );
          const u8 g = (sg+og>31 ? 31 : (sg+og<0 ? 0 : sg+og) );
          const u8 b = (sb+ob>31 ? 31 : (sb+ob<0 ? 0 : sb+ob) );

          work->platePalTrans[j][i+2] = GX_RGB(r,g,b);
        }
      }
    }
  }
  
  
  NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                      PPC_NORMAL_SELECT * 32 ,
                                      work->platePalTrans , 2*16*PLIST_MENU_ANIME_NUM );
}

//--------------------------------------------------------------
//	VBlankTcb
//--------------------------------------------------------------
static void PLIST_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  PLIST_WORK *work = (PLIST_WORK*)wk;

  if( work->isActiveWindowMask == TRUE )
  {
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
  }
  else
  {
    GX_SetVisibleWnd( GX_WNDMASK_NONE );
  }

  //OBJの更新
  GFL_CLACT_SYS_VBlankFunc();
//  NNS_GfdDoVramTransfer();
}


#pragma mark [>graphic
//--------------------------------------------------------------
//グラフィック系初期化
//--------------------------------------------------------------
static void PLIST_InitGraphic( PLIST_WORK *work )
{
  GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_A,       // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
    GX_VRAM_SUB_BG_128_C,     // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
    GX_VRAM_OBJ_128_B,       // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット
    GX_VRAM_SUB_OBJ_NONE,     // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
    GX_VRAM_TEX_0_D,        // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_0_F,     // テクスチャパレットスロット
    GX_OBJVRAMMODE_CHAR_1D_128K,
    GX_OBJVRAMMODE_CHAR_1D_128K
  };

  //バトル選択で3D(持ち物フォルムチェンジ)は使わず、上画面にポケアイコン出す
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    vramBank.sub_obj = GX_VRAM_SUB_OBJ_128_D;
    vramBank.teximage = GX_VRAM_TEX_NONE;
  }
  else
  {
    vramBank.sub_obj = GX_VRAM_SUB_OBJ_16_I;
  }

  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
  GFL_DISP_SetBank( &vramBank );

  //BG系の初期化
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram割り当ての設定
  {
    //メイン画面は2D/3Dの切り替えで再設定あるので注意
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    // BG1 MAIN (パラメータ
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x1000, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (プレート
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (背景
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000,0x05000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG0 SUB (文字
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x18000,0x08000,
      GX_BG_EXTPLTT_23, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 SUB (バー
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (プレート
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
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    PLIST_SetupBgFunc( &header_main3 , PLIST_BG_MAIN_BG , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_main2 , PLIST_BG_PLATE , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_main1 , PLIST_BG_PARAM , GFL_BG_MODE_TEXT );

    PLIST_SetupBgFunc( &header_sub3 , PLIST_BG_SUB_BG , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_sub2 , PLIST_BG_SUB_BATTLE_WIN , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_sub1 , PLIST_BG_SUB_BATTLE_BAR , GFL_BG_MODE_TEXT );
    PLIST_SetupBgFunc( &header_sub0 , PLIST_BG_SUB_BATTLE_STR , GFL_BG_MODE_TEXT );
    
    //最初は2Dで初期化
    PLIST_InitBG0_2DMenu( work );
    
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                      GX_BLEND_PLANEMASK_BG3 ,
                      16 , 10 );
    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , 
                      GX_BLEND_PLANEMASK_BG3 ,
                      16 , 10 );

    //土台とパラメータは取替えアニメのためスクロールさせる
    GFL_BG_SetScroll( PLIST_BG_PLATE , GFL_BG_SCROLL_X_SET , PLIST_BG_SCROLL_X_CHAR*8 );
    GFL_BG_SetScroll( PLIST_BG_PARAM , GFL_BG_SCROLL_X_SET , PLIST_BG_SCROLL_X_CHAR*8 );
  }
  
  //OBJ系の初期化
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
  //Vram転送アニメ
  //machineuse で初期化してた
//  NNS_GfdInitVramTransferManager( work->transTask , PLIST_VTRANS_TASK_NUM );
  NNS_GfdClearVramTransferManagerTask();
}

//--------------------------------------------------------------
//BG0用初期化(BG0面の2D/3D
//--------------------------------------------------------------
void PLIST_InitBG0_2DMenu( PLIST_WORK *work )
{
  // BG0 MAIN (メニュー
  static const GFL_BG_BGCNT_HEADER header_main0 = {
    0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
    GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
    GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x18000,0x8000,
    GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
  };
  GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS , GX_BGMODE_0 , GX_BG0_AS_2D );
  PLIST_SetupBgFunc( &header_main0 , PLIST_BG_MENU , GFL_BG_MODE_TEXT );
}


void PLIST_InitBG0_3DParticle( PLIST_WORK *work )
{
  static const VecFx32 cam_pos = {0,0,FX32_CONST(128.0f)};
  static const VecFx32 cam_target = {0,0,0};
  static const VecFx32 cam_up = {0,FX32_ONE,0};
  //エッジマーキングカラー
  static  const GXRgb stage_edge_color_table[8]=
    { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };


  GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS , GX_BGMODE_0 , GX_BG0_AS_3D );
  GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
          0, work->heapId, NULL );
  //↓を使うと表示もされてしまうのでSDK直叩き
  //GFL_BG_SetBGControl3D( 0 ); //NNS_g3dInitの中で表示優先順位変わる・・・
  G2_SetBG0Priority( 0 );

  G3X_EdgeMarking( TRUE );
  G3X_AntiAlias( TRUE );
  G3X_AlphaBlend( TRUE );
  
  work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       0,
                       FX32_ONE*192.0f/PLIST_DEMO_SCALE,
                       0,
                       FX32_ONE*256.0f/PLIST_DEMO_SCALE,
                       FX32_ONE,
                       FX32_ONE*128,
                       0,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
      GFL_G3D_CAMERA_Switching( work->camera );

  //エフェクトのため変更
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 ,
                    GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ ,
                    16 , 10 );


	GFL_PTC_Init( work->heapId );

}



//--------------------------------------------------------------
//グラフィック系開放
//--------------------------------------------------------------
static void PLIST_TermGraphic( PLIST_WORK *work )
{
  NNS_GfdClearVramTransferManagerTask();
  GFL_CLACT_SYS_Delete();

  PLIST_TermBG0_2DMenu( work );
  GFL_BG_FreeBGControl( PLIST_BG_MAIN_BG );
  GFL_BG_FreeBGControl( PLIST_BG_PLATE );
  GFL_BG_FreeBGControl( PLIST_BG_PARAM );
  GFL_BG_FreeBGControl( PLIST_BG_SUB_BG );
  GFL_BG_FreeBGControl( PLIST_BG_SUB_BATTLE_WIN );
  GFL_BG_FreeBGControl( PLIST_BG_SUB_BATTLE_BAR );
  GFL_BG_FreeBGControl( PLIST_BG_SUB_BATTLE_STR );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

}
void PLIST_TermBG0_2DMenu( PLIST_WORK *work )
{
  GFL_BG_FreeBGControl( PLIST_BG_MENU );
}
void PLIST_TermBG0_3DParticle( PLIST_WORK *work )
{
	GFL_PTC_Exit();
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_BG_FreeBGControl( PLIST_BG_3D );

  //エフェクトで設定したのを戻す
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                    GX_BLEND_PLANEMASK_BG3 ,
                    16 , 10 );
}

//--------------------------------------------------------------------------
//  Bg初期化 機能部
//--------------------------------------------------------------------------
static void PLIST_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------------------
//  セル作成
//--------------------------------------------------------------------------
static void PLIST_InitCell( PLIST_WORK *work )
{
  //個数は適当
  work->cellUnit  = GFL_CLACT_UNIT_Create( 10 , PLIST_CELLUNIT_PRI_MAIN, work->heapId );
  GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
  //セルの生成
  //カーソル
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 3;
    work->clwkCursor[0] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_CURSOR],
              work->cellRes[PCR_PLT_OBJ_COMMON],
              work->cellRes[PCR_ANM_CURSOR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    work->clwkCursor[1] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_CURSOR],
              work->cellRes[PCR_PLT_OBJ_COMMON],
              work->cellRes[PCR_ANM_CURSOR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkCursor[0] , TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkCursor[1] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
  }
  //バーアイコン
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_BARICON_RETURN_X_BAR;
    cellInitData.pos_y = PLIST_BARICON_Y;
    cellInitData.anmseq = APP_COMMON_BARICON_RETURN;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    
    work->clwkBarIcon[PBT_RETURN] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_BAR_ICON],
              work->cellRes[PCR_PLT_BAR_ICON],
              work->cellRes[PCR_ANM_BAR_ICON],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkBarIcon[PBT_RETURN] , TRUE );
    
    cellInitData.pos_x = PLIST_BARICON_EXIT_X-4;
    cellInitData.anmseq = APP_COMMON_BARICON_EXIT;
    work->clwkBarIcon[PBT_EXIT] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[PCR_NCG_BAR_ICON],
              work->cellRes[PCR_PLT_BAR_ICON],
              work->cellRes[PCR_ANM_BAR_ICON],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkBarIcon[PBT_EXIT] , TRUE );
  }
}

//--------------------------------------------------------------------------
//  セル削除
//--------------------------------------------------------------------------
static void PLIST_TermCell( PLIST_WORK *work )
{
  GFL_CLACT_WK_Remove( work->clwkBarIcon[PBT_RETURN] );
  GFL_CLACT_WK_Remove( work->clwkBarIcon[PBT_EXIT] );
  GFL_CLACT_WK_Remove( work->clwkCursor[0] );
  GFL_CLACT_WK_Remove( work->clwkCursor[1] );

  GFL_CLACT_UNIT_Delete( work->cellUnit );
}

//--------------------------------------------------------------------------
//  メッセージ系初期化
//--------------------------------------------------------------------------
static void PLIST_InitMessage( PLIST_WORK *work )
{
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  work->sysFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_num_gftr  , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //メッセージ
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_pokelist_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , PLIST_BG_PLT_FONT*16*2, 16*2, work->heapId );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , PLIST_BG_SUB_PLT_FONT*16*2, 16*2, work->heapId );
  
  //キューが足りなかったので追加(デフォルト1024
  work->printQue = PRINTSYS_QUE_CreateEx( 6144 , work->heapId );
  GFL_FONTSYS_SetDefaultColor();
}

//--------------------------------------------------------------------------
//  メッセージ系開放
//--------------------------------------------------------------------------
static void PLIST_TermMessage( PLIST_WORK *work )
{
  PRINTSYS_QUE_Clear( work->printQue );
  PRINTSYS_QUE_Delete( work->printQue );
  GFL_MSG_Delete( work->msgHandle );
  GFL_FONT_Delete( work->sysFontHandle );
  GFL_FONT_Delete( work->fontHandle );
}

#pragma mark [>resource
//--------------------------------------------------------------------------
//  リソース系読み込み
//--------------------------------------------------------------------------
static void PLIST_LoadResource( PLIST_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKELIST , work->heapId );

  ////BGリソース

  //下画面共通パレット
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_p_list_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_hp_bar_NCLR , 
                    PALTYPE_MAIN_BG , PLIST_BG_PLT_HP_BAR*32 , 32 , work->heapId );

  //下画面背景
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_p_list_NCGR ,
                    PLIST_BG_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_p_list_NSCR , 
                    PLIST_BG_MAIN_BG ,  0 , 0, FALSE , work->heapId );

  //下画面プレート
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_plate_NCGR ,
                    PLIST_BG_PLATE , 0 , 0, FALSE , work->heapId );
  work->plateScrRes = GFL_ARCHDL_UTIL_LoadScreen( arcHandle , NARC_pokelist_gra_list_plate_NSCR ,
                    FALSE , &work->plateScrData , work->heapId );


  //上画面背景
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_list_sub_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_sub_NCGR ,
                    PLIST_BG_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_list_sub_NSCR , 
                    PLIST_BG_SUB_BG ,  0 , 0, FALSE , work->heapId );

  ////OBJリソース
  //パレット
  work->cellRes[PCR_PLT_OBJ_COMMON] = GFL_CLGRP_PLTT_RegisterEx( arcHandle , 
        NARC_pokelist_gra_list_obj_NCLR , CLSYS_DRAW_MAIN , 
        PLIST_OBJPLT_COMMON*32 , 0 , 3 , work->heapId  );
        
  //キャラクタ
  work->cellRes[PCR_NCG_CURSOR] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_list_cursor_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[PCR_NCG_BALL] = GFL_CLGRP_CGR_Register( arcHandle , 
        NARC_pokelist_gra_list_ball_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  
  //セル・アニメ
  work->cellRes[PCR_ANM_CURSOR] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_list_cursor_NCER , NARC_pokelist_gra_list_cursor_NANR, work->heapId  );
  work->cellRes[PCR_ANM_BALL] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
        NARC_pokelist_gra_list_ball_NCER , NARC_pokelist_gra_list_ball_NANR, work->heapId  );

  GFL_ARC_CloseDataHandle(arcHandle);
  
  //ポケアイコン用リソース
  //キャラクタは各プレートで
  {
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , work->heapId );
    work->cellRes[PCR_PLT_POKEICON] = GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke , 
          POKEICON_GetPalArcIndex() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_POKEICON*32 , work->heapId  );
    work->cellRes[PCR_ANM_POKEICON] = GFL_CLGRP_CELLANIM_Register( arcHandlePoke , 
          POKEICON_GetCellArcIndex() , POKEICON_GetAnmArcIndex(), work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
  //共通素材
  {
    ARCHANDLE *arcHandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );
    
    //下画面バー
    //スクリーンはstate素材から(512*256のため
    GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_MAIN_BG , PLIST_BG_PLT_BAR*32 , 32 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , APP_COMMON_GetBarCharArcIdx() ,
                  PLIST_BG_PARAM , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , APP_COMMON_GetBarScrn_512x256ArcIdx() , 
                      PLIST_BG_PARAM ,  0 , 0, FALSE , work->heapId );
    GFL_BG_ChangeScreenPalette( PLIST_BG_PARAM , 0,0,64,32,PLIST_BG_PLT_BAR );

    //HPバー土台
    work->cellRes[PCR_PLT_HP_BASE] = GFL_CLGRP_PLTT_RegisterEx( arcHandleCommon , 
          APP_COMMON_GetHPBarBasePltArcIdx() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_HP_BASE*32 , 0 , 1 , work->heapId  );
    work->cellRes[PCR_NCG_HP_BASE] = GFL_CLGRP_CGR_Register( arcHandleCommon , 
          APP_COMMON_GetHPBarBaseCharArcIdx(APP_COMMON_MAPPING_128K) , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[PCR_ANM_HP_BASE] = GFL_CLGRP_CELLANIM_Register( arcHandleCommon , 
          APP_COMMON_GetHPBarBaseCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetHPBarBaseAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    //バー
    work->cellRes[PCR_PLT_BAR_ICON] = GFL_CLGRP_PLTT_RegisterEx( arcHandleCommon , 
          APP_COMMON_GetBarIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_BAR_ICON*32 , 0 , APP_COMMON_BARICON_PLT_NUM , work->heapId  );
    work->cellRes[PCR_NCG_BAR_ICON] = GFL_CLGRP_CGR_Register( arcHandleCommon , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[PCR_ANM_BAR_ICON] = GFL_CLGRP_CELLANIM_Register( arcHandleCommon , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), 
          work->heapId  );
    //アイコン用アイテム
    work->cellRes[PCR_PLT_ITEM_ICON] = GFL_CLGRP_PLTT_RegisterEx( arcHandleCommon , 
          APP_COMMON_GetPokeItemIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_ITEM_ICON*32 , 0 , APP_COMMON_BALL_PLT_NUM , work->heapId  );
    work->cellRes[PCR_NCG_ITEM_ICON] = GFL_CLGRP_CGR_Register( arcHandleCommon , 
          APP_COMMON_GetPokeItemIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[PCR_ANM_ITEM_ICON] = GFL_CLGRP_CELLANIM_Register( arcHandleCommon , 
          APP_COMMON_GetPokeItemIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetPokeItemIconAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    //状態異常アイコン
    work->cellRes[PCR_PLT_CONDITION] = GFL_CLGRP_PLTT_RegisterEx( arcHandleCommon , 
          APP_COMMON_GetStatusIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          PLIST_OBJPLT_CONDITION*32 , 0 , 1 , work->heapId  );
    work->cellRes[PCR_NCG_CONDITION] = GFL_CLGRP_CGR_Register( arcHandleCommon , 
          APP_COMMON_GetStatusIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[PCR_ANM_CONDITION] = GFL_CLGRP_CELLANIM_Register( arcHandleCommon , 
          APP_COMMON_GetStatusIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetStatusIconAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandleCommon);
  }
  
  //アニメ用に、プレートの選択色のパレットを退避
  GFL_STD_MemCopy16( (void*)(HW_BG_PLTT+PPC_NORMAL_SELECT*32) , work->platePalAnm , 16*2*PLIST_MENU_ANIME_NUM );
}


static void PLIST_ReleaseResource( PLIST_WORK *work )
{
  u8 i;
  //BG
  GFL_HEAP_FreeMemory( work->plateScrRes );
  
  //OBJ
  for( i=PCR_PLT_START ; i<PCR_NCG_START ; i++ )
  {
    if( work->cellRes[i] != PCR_NONE )
    {
      GFL_CLGRP_PLTT_Release( work->cellRes[i] );
    }
  }
  for( i=PCR_NCG_START ; i<PCR_ANM_START ; i++ )
  {
    if( work->cellRes[i] != PCR_NONE )
    {
      GFL_CLGRP_CGR_Release( work->cellRes[i] );
    }
  }
  for( i=PCR_ANM_START ; i<PCR_MAX ; i++ )
  {
    if( work->cellRes[i] != PCR_NONE )
    {
      GFL_CLGRP_CELLANIM_Release( work->cellRes[i] );
    }
  }
}

#pragma mark [>accorde mode

//--------------------------------------------------------------------------
//  モード別の初期化(開始時
//--------------------------------------------------------------------------
static void PLIST_InitMode( PLIST_WORK *work )
{
  switch( work->plData->mode )
  {
  case PL_MODE_BATTLE:
    //決定のみ作成
    work->btlMenuWin[0] = PLIST_MENU_CreateMenuWin_BattleMenu( work , work->menuWork ,
                              mes_pokelist_01_01 , PLIST_BATTLE_BUTTON_DECIDE_X+10 , PLIST_BATTLE_BUTTON_DECIDE_Y , FALSE );
  
  
    //選択画面へ
    PLIST_InitMode_Select( work );
    work->nextMainSeq = PSMS_SELECT_POKE;
    break;
  
  case PL_MODE_BATTLE_SUBWAY:
  case PL_MODE_BATTLE_EXAMINATION:
  
    work->btlMenuWin[0] = PLIST_MENU_CreateMenuWin_BattleMenu( work , work->menuWork ,
                              mes_pokelist_01_01 , PLIST_BATTLE_BUTTON_DECIDE_X , PLIST_BATTLE_BUTTON_DECIDE_Y , FALSE );
    work->btlMenuWin[1] = PLIST_MENU_CreateMenuWin_BattleMenu( work , work->menuWork ,
                              mes_pokelist_01_02 , PLIST_BATTLE_BUTTON_CANCEL_X , PLIST_BATTLE_BUTTON_CANCEL_Y , TRUE );
    //選択画面へ
    PLIST_InitMode_Select( work );
    work->nextMainSeq = PSMS_SELECT_POKE;
  
    break;
  
  case PL_MODE_FIELD:
  case PL_MODE_SHINKA:
  case PL_MODE_ITEMSET:
  case PL_MODE_MAILBOX:
  case PL_MODE_WAZASET:
  case PL_MODE_SODATEYA:
  case PL_MODE_GURU2:
  case PL_MODE_SET_MUSICAL:
    //選択画面へ
    PLIST_InitMode_Select( work );
    work->nextMainSeq = PSMS_SELECT_POKE;
    break;

  case PL_MODE_SET:
    work->canExit = FALSE;
    //選択画面へ
    PLIST_InitMode_Select( work );
    work->nextMainSeq = PSMS_SELECT_POKE;
    break;

  case PL_MODE_ITEMUSE:
    if( PLIST_ITEM_IsDeathRecoverAllItem( work , work->plData->item ) == TRUE )
    {
      const int target = PLIST_ITEM_CanUseDeathRecoverAllItem( work );

      if( target != -1 )
      {
        work->pokeCursor = target;
        work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, target);

        PLIST_ITEM_MSG_UseItemFunc( work , 0 );
        STATUS_RCV_Recover( work->selectPokePara , work->plData->item , 0 , work->plData->zone_id , work->heapId );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
        work->reqPlaySe = TRUE; //↓はフェード後に鳴らす
        //PMSND_PlaySystemSE( PLIST_SND_RECOVER );
        
        work->nextMainSeq = work->mainSeq;
        work->mainSeq = PSMS_FADEIN;

        PLIST_SubBagItem( work , work->plData->item );
      }
      else
      {
        //全体回復アイテムは戻る！
        PLIST_ITEM_MSG_CanNotUseItem( work );
        work->nextMainSeq = work->mainSeq;
        work->mainSeq = PSMS_FADEIN;
      }
    }
    else
    {
      if( PLIST_CheckBagItemNum( work , work->plData->item ) > 0 )
      {
        //選択画面へ
        PLIST_InitMode_Select( work );
        work->nextMainSeq = PSMS_SELECT_POKE;
      }
      else
      {
        work->plData->ret_mode = PL_RET_BAG;
        PLIST_MSG_CloseWindow( work , work->msgWork );
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 0 , work->plData->item );
        PLIST_MessageWaitInit( work , mes_pokelist_13_08 , TRUE , PLIST_MSGCB_ExitCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        work->nextMainSeq = work->mainSeq;
        work->mainSeq = PSMS_FADEIN;
      }
    }
    break;

  case PL_MODE_WAZASET_RET:
  case PL_MODE_LVUPWAZASET_RET:
    work->pokeCursor = work->plData->ret_sel;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
    if( work->plData->waza_pos < 4 )
    {
      //技が決定した
      const u32 wazaNo = PP_Get( work->selectPokePara , ID_PARA_waza1+work->plData->waza_pos , NULL );
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , wazaNo );
      PLIST_MessageWaitInit( work , mes_pokelist_04_10 , TRUE , PLIST_MSGCB_ForgetSkill_SkillForget );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    else
    {
      //選択キャンセル→覚えるのをあきらめますか？
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_07 , FALSE , PLIST_MSGCB_ForgetSkill_SkillCancel );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    work->nextMainSeq = PSMS_MSG_WAIT;
    work->mainSeq = PSMS_FADEIN;
    work->canExit = FALSE;

    PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
    break;
    
  case PL_MODE_ITEMSET_RET:
    work->pokeCursor = work->plData->ret_sel;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
    if( work->plData->item == 0 )
    {
      //キャンセルされた
      //選択画面へ
      //ついでにモードをフィールドに戻してしまう
      work->plData->mode = PL_MODE_FIELD;
      PLIST_InitMode_Select( work );
      work->nextMainSeq = PSMS_SELECT_POKE;
    }
    else
    {
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        //フォルムチェンジ前にReDrawParamが必要
        PLIST_SetPokeItem( work , work->selectPokePara , work->plData->item );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );

        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , work->plData->item );
        
        PLIST_ITEM_CangeAruseusuForm( work , work->selectPokePara , work->plData->item );
        PLIST_ITEM_CangeInsekutaForm( work , work->selectPokePara , work->plData->item );
        if( PLIST_DEMO_CheckGirathnaToOrigin( work , work->selectPokePara ) == TRUE )
        {
          //ギラティナ・フォルムチェンジ
          PLIST_DEMO_ChangeGirathinaToOrigin( work , work->selectPokePara);
          PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PLIST_MSGCB_FormChange );
          work->demoType = PDT_GIRATHINA_TO_ORIGIN;
        }
        else
        {
          PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        }
        PLIST_MSG_DeleteWordSet( work , work->msgWork );

        //ついでにモードをフィールドに戻してしまう
        work->plData->mode = PL_MODE_FIELD;
      }
      else
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_28 , FALSE , PLIST_MSGCB_ItemSet_CheckChangeItem );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
      }
    }
    work->nextMainSeq = PSMS_MSG_WAIT;
    work->mainSeq = PSMS_FADEIN;
    work->canExit = TRUE;

    PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
    break;
  
  //メール画面からの戻り
  case PL_MODE_MAILSET:
  case PL_MODE_MAILSET_BAG:
    work->pokeCursor = work->plData->ret_sel;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
    {
      const u32 haveItemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      u16 msgId;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      if( haveItemNo == 0 )
      {
        msgId = mes_pokelist_04_59;
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , work->plData->item );
      }
      else
      {
        msgId = mes_pokelist_04_32;
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , haveItemNo );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 2 , work->plData->item );
      }

      //フォルムチェンジ前にReDrawParamが必要
      PLIST_SetPokeItem( work , work->selectPokePara , work->plData->item );
      PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );

      PLIST_ITEM_CangeAruseusuForm( work , work->selectPokePara , work->plData->item );
      PLIST_ITEM_CangeInsekutaForm( work , work->selectPokePara , work->plData->item );
      if( PLIST_DEMO_CheckGirathnaToAnother( work , work->selectPokePara ) == TRUE )
      {
        //ギラティナ・フォルムチェンジ
        PLIST_DEMO_ChangeGirathinaToAnother( work , work->selectPokePara);
        PLIST_MessageWaitInit( work , msgId , TRUE , PLIST_MSGCB_FormChange );
        work->demoType = PDT_GIRATHINA_TO_ANOTHER;
      }
      else
      if( work->plData->mode == PL_MODE_MAILSET )
      {
        PLIST_MessageWaitInit( work , msgId , TRUE , PLIST_MSGCB_ExitCommon );
      }
      else
      {
        PLIST_MessageWaitInit( work , msgId , TRUE , PLIST_MSGCB_ReturnSelectCommon );
      }
      PLIST_MSG_DeleteWordSet( work , work->msgWork );

      if( work->plData->mode == PL_MODE_MAILSET )
      {
        //バッグからリストを呼んだのでバッグに戻る
        work->plData->ret_mode = PL_RET_BAG;
      }
      else
      {
        //リストからバッグを呼んでいったので、選択に戻る
        work->plData->mode = PL_MODE_FIELD;
      }
    }
    work->nextMainSeq = PSMS_MSG_WAIT;
    work->mainSeq = PSMS_FADEIN;
    work->canExit = TRUE;
    break;

  default:
    GF_ASSERT_MSG( NULL , "PLIST mode まだ作ってない！[%d]\n" , work->plData->mode );
    break;
  }}

//--------------------------------------------------------------------------
//  モード別の初期化(ポケモン選択
//--------------------------------------------------------------------------
static void PLIST_InitMode_Select( PLIST_WORK *work )
{
  GFL_BG_LoadScreenReq( PLIST_BG_MENU );  //ちらつきぼうし！
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_01 );
    work->canExit = TRUE;
    break;
    
  case PL_MODE_BATTLE:
    PLIST_BATTLE_OpenWaitingMessage( work );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
    work->canExit = FALSE;
    break;

  case PL_MODE_BATTLE_SUBWAY:
  case PL_MODE_BATTLE_EXAMINATION:
    //何もでない
    work->canExit = FALSE;
    break;
    
  case PL_MODE_ITEMUSE:
  case PL_MODE_SHINKA:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_04 );
    work->canExit = FALSE;
    work->canSelectEgg = FALSE;
    break;
  
  case PL_MODE_ITEMSET:
  case PL_MODE_MAILBOX:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_03 );
    work->canExit = FALSE;
    work->canSelectEgg = FALSE;
    break;
  
  case PL_MODE_WAZASET:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_05 );
    work->canExit = FALSE;
    work->canSelectEgg = FALSE;
    break;
  
  case PL_MODE_SODATEYA:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_07 );
    work->canExit = FALSE;
    break;
  
  case PL_MODE_SET:
  case PL_MODE_SET_MUSICAL:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_01 );
    work->canExit = FALSE;
    break;
  
  case PL_MODE_GURU2:
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_10_02_01 );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
    work->canExit = FALSE;
    break;

  default:
    GF_ASSERT_MSG( NULL , "PLIST mode まだ作ってない！[%d]\n" , work->plData->mode );
    break;
  }
  PLIST_SelectPokeInit( work );
}
//--------------------------------------------------------------------------
//  モード別の開放(ポケモン選択
//--------------------------------------------------------------------------
static void PLIST_TermMode_Select_Decide( PLIST_WORK *work )
{
  if( work->plData->isNetErr == TRUE )
  {
    work->mainSeq = PSMS_FADEOUT;
    return;
  }
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
  case PL_MODE_BATTLE:
  case PL_MODE_BATTLE_SUBWAY:
  case PL_MODE_BATTLE_EXAMINATION:
  case PL_MODE_SODATEYA:
  case PL_MODE_GURU2:
  case PL_MODE_SET_MUSICAL:
    PLIST_SelectMenuInit( work );
    //中で一緒にメニューを開く
    GFL_BG_LoadScreenReq(PLIST_BG_MENU);
    PLIST_InitMode_Menu( work );
    
    break;
    
  case PL_MODE_ITEMUSE:
    {
      const BOOL isSelSkill = PLIST_ITEM_IsNeedSelectSkill( work , work->plData->item );
      if( isSelSkill == TRUE )
      {
        //スキル選択へ
        PLIST_SelectMenuInit( work );
        //中で一緒にメニューを開く
        PLIST_InitMode_Menu( work );
      }
      else
      {
        const BOOL canUse = STATUS_RCV_RecoverCheck( work->selectPokePara , work->plData->item , 0 , work->heapId );
        OS_TFPrintf(2,"[%3d][%3d][%3d][%3d][%3d][%3d]:[%d]\n"
              ,PP_Get(work->selectPokePara , ID_PARA_hp_exp , NULL )
              ,PP_Get(work->selectPokePara , ID_PARA_pow_exp, NULL )
              ,PP_Get(work->selectPokePara , ID_PARA_def_exp, NULL )
              ,PP_Get(work->selectPokePara , ID_PARA_agi_exp, NULL )
              ,PP_Get(work->selectPokePara , ID_PARA_spepow_exp , NULL )
              ,PP_Get(work->selectPokePara , ID_PARA_spedef_exp , NULL )
              ,canUse );

        if( canUse == TRUE )
        {
          const PLIST_ITEM_USE_TYPE useType = PLIST_ITEM_MSG_UseItemFunc( work  , 0 );
          
          //実際に適用
          STATUS_RCV_Recover( work->selectPokePara , work->plData->item , 0 , work->plData->zone_id , work->heapId );
          OS_TFPrintf(2,"[%3d][%3d][%3d][%3d][%3d][%3d]:[%d]\n"
                ,PP_Get(work->selectPokePara , ID_PARA_hp_exp , NULL )
                ,PP_Get(work->selectPokePara , ID_PARA_pow_exp, NULL )
                ,PP_Get(work->selectPokePara , ID_PARA_def_exp, NULL )
                ,PP_Get(work->selectPokePara , ID_PARA_agi_exp, NULL )
                ,PP_Get(work->selectPokePara , ID_PARA_spepow_exp , NULL )
                ,PP_Get(work->selectPokePara , ID_PARA_spedef_exp , NULL )
                ,canUse );
          if( useType == ITEM_TYPE_LV_UP )
          {
            //レベルだけ表示HP変わるのでResetParamを使う
            PLIST_PLATE_ResetParam( work , work->plateWork[work->pokeCursor] , work->selectPokePara , 0 );
            //PMSND_PlaySystemSE( PLIST_SND_LVUP );
          }
          else
          {
            if( useType == ITEM_TYPE_HP_DOWN ||
                useType == ITEM_TYPE_HP_UP )
            {
              //HPの努力値も変わる・・・
              PLIST_PLATE_ResetParam( work , work->plateWork[work->pokeCursor] , work->selectPokePara , 0 );
            }
            else
            {
              PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
            }
            PMSND_PlaySystemSE( PLIST_SND_RECOVER );
          }
          PLIST_SubBagItem( work , work->plData->item );
        }
        else
        {
          if( work->plData->item == ITEM_GURASIDEANOHANA &&
              PLIST_DEMO_CheckSheimiToSky( work , work->selectPokePara ) == TRUE )
          {
            PLIST_DEMO_ChangeSheimiToSky( work , work->selectPokePara );
            work->plData->ret_mode = PL_RET_BAG;
            work->mainSeq = PSMS_FORM_CHANGE_INIT;
            work->demoType = PDT_SHEIMI_TO_SKY;
          }
          else
          {
            PLIST_ITEM_MSG_CanNotUseItemContinue( work );
          }
        }
      }
    }
    break;
    
  case PL_MODE_SHINKA:
    {
      const u16 newMonsNo = PLIST_UTIL_CheckItemEvolution( work , work->selectPokePara , work->plData->item );
      if( newMonsNo != 0 )
      {
        PLIST_SubBagItem( work , work->plData->item );
        work->mainSeq = PSMS_FADEOUT;
        work->plData->ret_sel = work->pokeCursor;
        work->plData->ret_mode = PL_RET_ITEMSHINKA;
      }
      else
      {
        PLIST_MessageWaitInit( work , mes_pokelist_04_45 , TRUE , PLIST_MSGCB_ExitCommon );
      }
    }
    break;

  case PL_MODE_ITEMSET:
    {
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        if( ITEM_CheckMail( work->plData->item ) == TRUE )
        {
          work->mainSeq = PSMS_FADEOUT;
          work->plData->ret_sel = work->pokeCursor;
          work->plData->ret_mode = PL_RET_MAILSET;
        }
        else
        {
          work->plData->ret_mode = PL_RET_BAG;
          
          //フォルムチェンジ前にReDrawParamが必要
          PLIST_SetPokeItem( work , work->selectPokePara , work->plData->item );
          PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
          
          PLIST_MSG_CreateWordSet( work , work->msgWork );
          PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
          PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , work->plData->item );

          PLIST_ITEM_CangeAruseusuForm( work , work->selectPokePara , work->plData->item );
          PLIST_ITEM_CangeInsekutaForm( work , work->selectPokePara , work->plData->item );

          if( PLIST_DEMO_CheckGirathnaToOrigin( work , work->selectPokePara ) == TRUE )
          {
            //ギラティナ・フォルムチェンジ
            PLIST_DEMO_ChangeGirathinaToOrigin( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ORIGIN;
          }
          else
          {
            PLIST_MessageWaitInit( work , mes_pokelist_04_59 , TRUE , PLIST_MSGCB_ExitCommon );
          }
          PLIST_MSG_DeleteWordSet( work , work->msgWork );
        }
      }
      else
      if( ITEM_CheckMail( itemNo ) == TRUE )
      {
        //メールをはずさないと道具がもてない
        work->plData->ret_mode = PL_RET_BAG;
        PLIST_MessageWaitInit( work , mes_pokelist_04_27 , TRUE , PLIST_MSGCB_ExitCommon );
      }
      else
      {
        //持ち変える？
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
        PLIST_MessageWaitInit( work , mes_pokelist_04_28 , FALSE , PLIST_MSGCB_ItemSet_CheckChangeItem );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
        
      }
    }
    break;

  case PL_MODE_MAILBOX:
    {
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      if( itemNo == 0 )
      {
        //実データのセット
        MAIL_DATA* mailData = MAIL_AllocMailData( work->plData->mailblock ,
                                                  MAILBLOCK_PASOCOM ,
                                                  work->plData->mail_pos,
                                                  work->heapId );

        PP_Put( work->selectPokePara , ID_PARA_mail_data , (u32)mailData );
        GFL_HEAP_FreeMemory( mailData );

        //アイテム番号のセット
        PP_Put( work->selectPokePara , ID_PARA_item , work->plData->item );
        
        //メール削除処理
        MAIL_DelMailData( work->plData->mailblock , MAILBLOCK_PASOCOM , work->plData->mail_pos );
        
        PLIST_MessageWaitInit( work , mes_pokelist_04_64 , TRUE , PLIST_MSGCB_ExitCommon );
      }
      else
      {
        //道具を持ってるのでメールをもてない！
        PLIST_MessageWaitInit( work , mes_pokelist_04_65 , TRUE , PLIST_MSGCB_ExitCommon );
      }
    }
    break;

  case PL_MODE_WAZASET:
    switch( PLIST_UTIL_CheckLearnSkill( work , work->selectPokePara , work->plData->ret_sel ) )
    {
    case LSCL_OK:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PLIST_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      
      PLIST_LearnSkillEmpty( work , work->selectPokePara );
      PLIST_UseWazaMachine( work , work->selectPokePara );
      /*
      if( work->plData->item != 0 )
      {
        PLIST_SubBagItem( work , work->plData->item );
      }
      */
      break;
    
    case LSCL_OK_FULL:
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_06 , FALSE , PLIST_MSGCB_ForgetSkill_ForgetCheck );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      break;

    case LSCL_NG:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_12 , TRUE , PLIST_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      
      break;

    case LSCL_LEARN:
      work->plData->ret_mode = PL_RET_BAG;
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
      PLIST_MessageWaitInit( work , mes_pokelist_04_13 , TRUE , PLIST_MSGCB_ExitCommon );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      break;

    }
    break;
  case PL_MODE_SET:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_NORMAL;
    break;
  
  default:
    PLIST_SelectMenuInit( work );
    //中で一緒にメニューを開く
    PLIST_InitMode_Menu( work );
    GF_ASSERT_MSG( NULL , "PLIST mode まだ作ってない！[%d]\n" , work->plData->mode );
    break;
  }
}

//--------------------------------------------------------------------------
//  モード別の初期化(メニュー
//--------------------------------------------------------------------------
static void PLIST_InitMode_Menu( PLIST_WORK *work )
{
  PLIST_MENU_ITEM_TYPE itemArr[8];
  GFL_BG_LoadScreenReq( PLIST_BG_MENU );  //ちらつきぼうし！
  switch( work->plData->mode )
  {
  case PL_MODE_FIELD:
    if( PLIST_PLATE_IsEgg( work , work->plateWork[work->pokeCursor] ) == TRUE )
    {
      itemArr[0] = PMIT_STATSU;
      itemArr[1] = PMIT_CHANGE;
      itemArr[2] = PMIT_CLOSE;
      itemArr[3] = PMIT_END_LIST;
    }
    else
    {
      itemArr[0] = PMIT_STATSU;
      itemArr[1] = PMIT_HIDEN;
      itemArr[2] = PMIT_CHANGE;
      itemArr[3] = PMIT_ITEM;
      itemArr[4] = PMIT_CLOSE;
      itemArr[5] = PMIT_END_LIST;
      
      //メール持ってるかチェック
      {
        const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
        if( ITEM_CheckMail( itemNo ) == TRUE )
        {
          itemArr[3] = PMIT_MAIL;
        }
      }
    }
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
    break;
  case PL_MODE_BATTLE:
  case PL_MODE_BATTLE_SUBWAY:
  case PL_MODE_BATTLE_EXAMINATION:
    if( PLIST_PLATE_IsEgg( work , work->plateWork[work->pokeCursor] ) == TRUE )
    {
      itemArr[0] = PMIT_STATSU;
      itemArr[1] = PMIT_CLOSE;
      itemArr[2] = PMIT_END_LIST;
    }
    else
    {
      itemArr[0] = PMIT_SET_JOIN;
      itemArr[1] = PMIT_STATSU;
      itemArr[2] = PMIT_CLOSE;
      itemArr[3] = PMIT_END_LIST;
    }
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );

    break;

  case PL_MODE_ITEMUSE:
    //アイテム使用で来るのはPP回復の技選択時
    itemArr[0] = PMIT_WAZA;
    itemArr[1] = PMIT_CLOSE;
    itemArr[2] = PMIT_END_LIST;
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    {
      u32 msgId = PLIST_ITEM_GetWazaListMessage( work , work->plData->item );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , msgId );
    }
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
    break;
    
  case PL_MODE_SODATEYA:
    if( PLIST_PLATE_IsEgg( work , work->plateWork[work->pokeCursor] ) == TRUE )
    {
      itemArr[0] = PMIT_STATSU;
      itemArr[1] = PMIT_CLOSE;
      itemArr[2] = PMIT_END_LIST;
    }
    else
    {
      itemArr[0] = PMIT_LEAVE;
      itemArr[1] = PMIT_STATSU;
      itemArr[2] = PMIT_CLOSE;
      itemArr[3] = PMIT_END_LIST;
    }
    
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
    
    PLIST_MSG_DeleteWordSet( work , work->msgWork );

    break;
    
  case PL_MODE_GURU2:
    if( PLIST_PLATE_IsEgg( work , work->plateWork[work->pokeCursor] ) == TRUE )
    {
      itemArr[0] = PMIT_ENTER;
      itemArr[1] = PMIT_STATSU;
      itemArr[2] = PMIT_CLOSE;
      itemArr[3] = PMIT_END_LIST;
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_10_02 );
    }
    else
    {
      itemArr[0] = PMIT_STATSU;
      itemArr[1] = PMIT_CLOSE;
      itemArr[2] = PMIT_END_LIST;

      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }
    break;
    
  case PL_MODE_SET_MUSICAL:
    {
      const BOOL ret = MUSICAL_SYSTEM_CheckEntryMusical( work->selectPokePara );
      if( ret == TRUE )
      {
        itemArr[0] = PMIT_ENTER;
        itemArr[1] = PMIT_STATSU;
        itemArr[2] = PMIT_CLOSE;
        itemArr[3] = PMIT_END_LIST;
      }
      else
      {
        itemArr[0] = PMIT_STATSU;
        itemArr[1] = PMIT_CLOSE;
        itemArr[2] = PMIT_END_LIST;
      }
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_01 );
      PLIST_MSG_DeleteWordSet( work , work->msgWork );
    }

    break;
  default:
    GF_ASSERT_MSG( NULL , "PLIST mode まだ作ってない！[%d]\n" , work->plData->mode );
    itemArr[0] = PMIT_STATSU;
    itemArr[1] = PMIT_CLOSE;
    itemArr[2] = PMIT_END_LIST;
    break;
  }
  PLIST_MENU_OpenMenu( work , work->menuWork , itemArr );

}

#pragma mark [>PokeSelect

//--------------------------------------------------------------------------
//  ポケモン選択画面
//--------------------------------------------------------------------------
static void PLIST_SelectPoke( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_INIT:
    work->subSeq = PSSS_MAIN;
    work->selectState = PSSEL_NONE;
    break;
    
  case PSSS_MAIN:
    PLIST_SelectPokeMain( work );
    break;
    
  case PSSS_TERM:
    if( work->mainSeq == PSMS_CHANGE_POKE )
    {
      PLIST_SelectPokeTerm_Change( work );
    }
    else if( work->mainSeq == PSMS_USE_POKE )
    {
      PLIST_SelectPokeTerm_Use( work );
    }
    else
    {
      PLIST_SelectPokeTerm( work );
    }
    break;
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面初期化
//--------------------------------------------------------------------------
static void PLIST_SelectPokeInit( PLIST_WORK *work )
{
  work->pokeCursor = work->plData->ret_sel;
  work->subSeq  = PSSS_INIT;
  work->menuRet = PMIT_NONE;

  //バトル処理
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[0] , FALSE );
    PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[1] , FALSE );
  }
  
//  if( work->ktst == GFL_APP_KTST_KEY || 
//      GFL_CLACT_WK_GetDrawEnable( work->clwkCursor[0] ) == TRUE )
  if( work->ktst == GFL_APP_KTST_KEY )
  {
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      PLIST_SelectPokeSetCursor( work , work->pokeCursor );

      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
    }
    else
    {
      //バトル時決定・戻る
      const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
      PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , TRUE );
    }
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      //通常時はTPはプレート非アクティブ。交換時はアクティブ
      if( work->mainSeq == PSMS_CHANGE_POKE )
      {
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
      }
      else
      {
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      }
    }
    else
    {
      //バトル時決定・戻る
      const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
      PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , FALSE );
    }
  }
  
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , work->canExit );
  
  //バトル処理
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE ||
      PLIST_UTIL_CanReturn(work) == FALSE )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , TRUE );
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面開放
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  //バトル処理
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE )
  {
    u8 i;
    GFL_BG_FillScreen( PLIST_BG_MENU , 0 , 0 , 21 , 32 , 3 , GFL_BG_SCRWRT_PALNL );
    
    //ついでにin_numへセット
    for(i=0;i<6;i++ )
    {
      work->plData->in_num[i] = 0;
    }
    for(i=0;i<PLIST_LIST_MAX;i++ )
    {
      const PLIST_PLATE_BATTLE_ORDER order = PLIST_PLATE_GetBattleOrder( work->plateWork[i] );
      if( order <= PPBO_JOIN_6 )
      {
        work->plData->in_num[order] = i+1;
      }
    }
  }

  switch( work->selectState )
  {
  case PSSEL_SELECT:
    //カーソルは非表示
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor );
    work->plData->ret_sel = work->pokeCursor;
    PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    
    PLIST_TermMode_Select_Decide( work );
    
    break;

  case PSSEL_DECIDE:
    if(  PLIST_UTIL_IsBattleMenu(work) == TRUE )
    {
      PLIST_SelectPokeTerm_BattleDecide( work );
    }
    else
    {
      work->mainSeq = PSMS_FADEOUT;
      work->plData->ret_sel = PL_SEL_POS_ENTER;
      work->plData->ret_mode = PL_RET_NORMAL;

      PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    }
    break;
    
  case PSSEL_RETURN:
    //アクティブだったところを戻す
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      //プレート
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    }

    if(  PLIST_UTIL_IsBattleMenu(work) == TRUE )
    {
      //決定再表示
      PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[0] , FALSE );
      
      //戻るを点灯
      PLIST_MENU_SetDecideMenuWin_BattleMenu( work->btlMenuWin[1] , TRUE );
      work->mainSeq = PSMS_BATTLE_ANM_WAIT;
      work->pokeCursor = PL_SEL_POS_EXIT;
    }
    else
    {
      work->mainSeq = PSMS_FADEOUT;

      work->clwkExitButton = work->clwkBarIcon[PBT_RETURN];
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[PBT_RETURN] , APP_COMMON_BARICON_RETURN_ON );
    }
    work->plData->ret_sel = PL_SEL_POS_EXIT;
    work->plData->ret_mode = PL_RET_NORMAL;
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    break;

  case PSSEL_EXIT:
    //アクティブだったところを戻す
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      //プレート
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    }

    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = PL_SEL_POS_EXIT2;
    work->plData->ret_mode = PL_RET_NORMAL;
    PMSND_PlaySystemSE( PLIST_SND_EXIT );

    work->clwkExitButton = work->clwkBarIcon[PBT_EXIT];
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[PBT_EXIT] , APP_COMMON_BARICON_EXIT_ON );
    break;
  }
}

//--------------------------------------------------------------------------
//  入れ替え用ポケモン選択画面開放
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm_Change( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  //同じプレートを選んだのでキャンセル扱い
  if( work->selectState == PSSEL_SELECT &&
      work->pokeCursor == work->changeTarget )
  {
    work->selectState = PSSEL_RETURN;
  }

  switch( work->selectState )
  {
  case PSSEL_SELECT:
    PLIST_ChangeAnimeInit( work );
    break;
    
  case PSSEL_RETURN:
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    PLIST_PLATE_SetActivePlate( work , work->plateWork[work->changeTarget] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
    work->changeTarget = PL_SEL_POS_MAX;
    work->plData->ret_sel = work->pokeCursor;

    PLIST_InitMode_Select( work );
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    break;

  }
}

//--------------------------------------------------------------------------
//  技使用対象決定ポケモン選択画面開放
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm_Use( PLIST_WORK *work )
{
  //今のところタマゴ産み・ミルク飲み処理のみ
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( work->selectState == PSSEL_SELECT )
  {
    u32 hpNowTrg,hpMaxTrg;
    work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor );
    hpNowTrg = PP_Get( work->selectPokePara , ID_PARA_hp , NULL );
    hpMaxTrg = PP_Get( work->selectPokePara , ID_PARA_hpmax , NULL );
    if( work->pokeCursor == work->useTarget || 
        hpNowTrg == 0 ||
        hpMaxTrg == hpNowTrg )
    {
      //自分と瀕死の相手には仕えない
      PLIST_MessageWaitInit( work , mes_pokelist_04_68 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
      PLIST_SelectPokeSetCursor( work , work->pokeCursor );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->useTarget] , FALSE );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
      work->plData->ret_sel = work->pokeCursor;
      
    }
    else
    {
      PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      
      work->selectPokePara = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor );
      work->mainSeq = PSMS_INIT_HPANIME;
      work->befHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->pokeCursor] );
      work->hpAnimeCallBack = PLIST_HPANMCB_SkillRecoverHpFirst;

      PLIST_SelectPokeSetCursor( work , work->pokeCursor );
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->useTarget] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );

      {
        //HP操作
        POKEMON_PARAM *usePP = PokeParty_GetMemberPointer(work->plData->pp, work->useTarget );
        const u32 hpMaxUse = PP_Get( usePP , ID_PARA_hpmax , NULL );
        const u32 hpNowUse = PP_Get( usePP , ID_PARA_hp , NULL );
        const u32 recvHp = hpMaxUse / 5;
        
        if( hpNowTrg + recvHp > hpMaxTrg )
        {
          PP_Put( usePP , ID_PARA_hp , hpNowUse - (hpMaxTrg-hpNowTrg) );
        }
        else
        {
          PP_Put( usePP , ID_PARA_hp , hpNowUse - (recvHp) );
        }
      }
      
      //アニメ対象のため一回pokeCursolを入れ替えておく
      //ついでにwork->plData->ret_selにカーソルをセットしておく
      work->plData->ret_sel = work->pokeCursor;
      work->pokeCursor = work->useTarget;
      
      PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      PMSND_PlaySystemSE( PLIST_SND_RECOVER );
    }
  }
  else
  {
    //キャンセル
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    PLIST_PLATE_SetActivePlate( work , work->plateWork[work->useTarget] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
    work->useTarget = PL_SEL_POS_MAX;
    work->plData->ret_sel = work->pokeCursor;

    PLIST_InitMode_Select( work );
    PMSND_PlaySystemSE( PLIST_SND_CANCEL );
  }

}

//--------------------------------------------------------------------------
//  ポケモン選択画面開放 バトル決定時
//--------------------------------------------------------------------------
static void PLIST_SelectPokeTerm_BattleDecide( PLIST_WORK *work )
{
  REGULATION *reg = (REGULATION*)work->plData->reg;
  int regRet;
  if( work->pokeCursor != PL_SEL_POS_ENTER )
  {
    //アクティブだったところを戻す
    if( work->pokeCursor <= PL_SEL_POS_POKE6 )
    {
      //プレート
      PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
    }

    work->pokeCursor = PL_SEL_POS_ENTER;
  }

  work->plData->ret_sel = PL_SEL_POS_ENTER;
  
  {
    u8 i;
    u8 arr[6]={0,0,0,0,0,0};

    for(i=0;i<PLIST_LIST_MAX;i++ )
    {
      const PLIST_PLATE_BATTLE_ORDER order = PLIST_PLATE_GetBattleOrder( work->plateWork[i] );
      if( order <= PPBO_JOIN_6 )
      {
        arr[order] = i+1;
      }
    }

    regRet = PokeRegulationMatchFullPokeParty( reg , work->plData->pp , arr );
  }
  
  if( reg->NUM_LO > work->btlJoinNum )
  {
    PLIST_MessageWaitInit( work , mes_pokelist_04_60_1 + (reg->NUM_LO-1) , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    PMSND_PlaySystemSE( PLIST_SND_ERROR );
  }
  else
  if( reg->NUM_HI < work->btlJoinNum )
  {
    PLIST_MessageWaitInit( work , mes_pokelist_04_62_1 + (reg->NUM_HI-1) , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    PMSND_PlaySystemSE( PLIST_SND_ERROR );
  }
  else
  if( regRet == POKE_REG_TOTAL_LV_FAILED )
  {
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_Value( work , work->msgWork , 0 , reg->LEVEL_TOTAL , 3 );
    PLIST_MessageWaitInit( work , mes_pokelist_07_03 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    PMSND_PlaySystemSE( PLIST_SND_ERROR );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
  if( regRet == POKE_REG_NO_MASTPOKE )
  {
    PLIST_MessageWaitInit( work , mes_pokelist_07_05 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    PMSND_PlaySystemSE( PLIST_SND_ERROR );
  }
  else
  {
    //戻るボタン再表示
    PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[1] , FALSE );
    
    //決定を点灯
    PLIST_MENU_SetDecideMenuWin_BattleMenu( work->btlMenuWin[0] , TRUE );
    
    work->mainSeq = PSMS_BATTLE_ANM_WAIT;
    work->plData->ret_sel = PL_SEL_POS_ENTER;
    work->plData->ret_mode = PL_RET_NORMAL;
    work->isDecideParty = TRUE;

    PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面更新
//--------------------------------------------------------------------------
static void PLIST_SelectPokeMain( PLIST_WORK *work )
{
  PLIST_SelectPokeUpdateKey( work );
  //キーによる決定が無かったらTPも見る
  if( work->selectState == PSSEL_NONE )
  {
    PLIST_SelectPokeUpdateTP( work );
  }

  if( work->selectState != PSSEL_NONE )
  {
    work->subSeq = PSSS_TERM;
  }
  if( work->plData->isNetErr == TRUE )
  {
    work->subSeq = PSSS_TERM;
    work->selectState = PSSEL_RETURN;
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面 キー操作
//--------------------------------------------------------------------------
static void PLIST_SelectPokeUpdateKey( PLIST_WORK *work )
{
  const int trg = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();
  if( work->ktst == GFL_APP_KTST_TOUCH && 
      GFL_CLACT_WK_GetDrawEnable( work->clwkCursor[0] ) == FALSE )
  {
    //非表示のときだけカーソル位置に表示するように
    //キャンセル操作時は除く
    if( trg != 0 &&
        (trg & (PAD_BUTTON_B|PAD_BUTTON_X)) == 0 )
    {
      if( work->pokeCursor <= PL_SEL_POS_POKE6 )
      {
        //プレート
        PLIST_SelectPokeSetCursor( work , work->pokeCursor );
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
      }
      else
      {
        //バトル時決定・戻る
        const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
        PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , TRUE );
        
        GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
      }
      PMSND_PlaySystemSE( PLIST_SND_CURSOR );
      work->ktst = GFL_APP_KTST_KEY;
    }
  }
  else
  {
    s8 moveVal = 0;
    if( trg != 0 )
    {
      work->ktst = GFL_APP_KTST_KEY;
    }
    //キー分岐
    if( repeat & PAD_KEY_UP )
    {
      moveVal = -2;
    }
    else
    if( repeat & PAD_KEY_DOWN )
    {
      moveVal = 2;
    }
    else
    if( repeat & PAD_KEY_LEFT )
    {
      moveVal = -1;
    }
    else
    if( repeat & PAD_KEY_RIGHT )
    {
      moveVal = 1;
    }
    else if( trg & PAD_BUTTON_A )
    {
      if( work->pokeCursor == PL_SEL_POS_ENTER )
      {
        work->selectState = PSSEL_DECIDE;
      }
      else
      if( work->pokeCursor == PL_SEL_POS_EXIT )
      {
        work->selectState = PSSEL_RETURN;
      }
      else
      {
        if( work->canSelectEgg == FALSE &&
            PLIST_PLATE_IsEgg( work , work->plateWork[work->pokeCursor] ) == TRUE )
        {
          PMSND_PlaySystemSE( PLIST_SND_ERROR );
        }
        else
        {
          work->selectState = PSSEL_SELECT;
        }
      }
    }
    
    //十字キー移動処理
    if( moveVal != 0 )
    {
      const PL_SELECT_POS befPos = work->pokeCursor;
      BOOL isFinish = FALSE;
      PL_SELECT_POS maxValue;
      
      //バトルのときだけ決定・戻るにカーソルが行く
      if( PLIST_UTIL_IsBattleMenu( work ) == TRUE )
      {
        if( PLIST_UTIL_IsBattleMenu_CanReturn( work ) == TRUE )
        {
          maxValue = PL_SEL_POS_EXIT;
        }
        else
        {
          maxValue = PL_SEL_POS_ENTER;
        }
      }
      else
      {
        maxValue = PL_SEL_POS_POKE6;
      }
      if( (work->pokeCursor == PL_SEL_POS_EXIT || 
          work->pokeCursor == PL_SEL_POS_ENTER ) && 
          (repeat & (PAD_KEY_UP|PAD_KEY_DOWN)) )
      {
        //決定・戻るにある時の上下移動は例外処理
        const u8 partyMax = PokeParty_GetPokeCount( work->plData->pp );
        if( repeat & PAD_KEY_UP )
        {
          if( work->pokeCursor == PL_SEL_POS_EXIT ||
              PLIST_UTIL_IsBattleMenu_CanReturn( work ) == FALSE )
          {
            //右列
            static const u8 arr[6] = {0,1,1,3,3,5};
            work->pokeCursor = arr[partyMax-1];
          }
          else
          {
            //左列
            static const u8 arr[6] = {0,0,2,2,4,4};
            work->pokeCursor = arr[partyMax-1];
          }
        }
        else
        {
          if( work->pokeCursor == PL_SEL_POS_EXIT ||
              PLIST_UTIL_IsBattleMenu_CanReturn( work ) == FALSE )
          {
            //右列
            if( partyMax > 1 )
            {
              work->pokeCursor = 1;
            }
            else
            {
              work->pokeCursor = 0;
            }
          }
          else
          {
            //左列
            work->pokeCursor = 0;
          }
        }
      }
      else
      {
        //プレートがある位置までループ
        while( isFinish == FALSE )
        {
          if( work->pokeCursor + moveVal > maxValue )
          {
            //EXITが無い時動きが変になるので調整
            if( maxValue == PL_SEL_POS_ENTER &&
                moveVal > 1 &&
                befPos != PL_SEL_POS_ENTER )
            {
              work->pokeCursor = maxValue;
            }
            else
            {
              work->pokeCursor = work->pokeCursor+moveVal-(maxValue+1);
            }
          }
          else
          if( work->pokeCursor + moveVal < PL_SEL_POS_POKE1 )
          {
            //EXITが無い時動きが変になるので調整
            if( maxValue == PL_SEL_POS_ENTER &&
                moveVal < 1 )
            {
              work->pokeCursor = maxValue;
            }
            else
            {
              work->pokeCursor = work->pokeCursor+(maxValue+1)+moveVal;
            }
          }
          else
          {
            work->pokeCursor += moveVal;
          }

          if( work->pokeCursor > PL_SEL_POS_POKE6 ||
              PLIST_PLATE_CanSelect( work , work->plateWork[work->pokeCursor] ) == TRUE )
          {
            isFinish = TRUE;
          }
        }
      }
      //表示周り更新
      if( befPos != work->pokeCursor )
      {
        if( work->pokeCursor <= PL_SEL_POS_POKE6 )
        {
          //プレート
          PLIST_SelectPokeSetCursor( work , work->pokeCursor );
          PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
        }
        else
        {
          //バトル時決定・戻る
          const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
          PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , TRUE );
          
          GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
        }
        
        if( befPos <= PL_SEL_POS_POKE6 )
        {
          //プレート
          PLIST_PLATE_SetActivePlate( work , work->plateWork[befPos] , FALSE );
        }
        else
        {
          //バトル時決定・戻る
          const u8 idx = befPos - PL_SEL_POS_ENTER;
          PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , FALSE );
        }
        work->platePalAnmCnt = PLIST_PLATE_ACTIVE_ANM_CNT;

        //PLIST_PLATE_SetActivePlatePos( work , work->pokeCursor );
      }

      PMSND_PlaySystemSE( PLIST_SND_CURSOR );
    }
  }
  //BとXはktsts関係なく動く
  if( trg & PAD_BUTTON_B )
  {
    if( (PLIST_UTIL_IsBattleMenu( work ) == FALSE || 
         PLIST_UTIL_IsBattleMenu_CanReturn( work ) == TRUE) &&
        PLIST_UTIL_CanReturn( work ) == TRUE )
    {
      work->selectState = PSSEL_RETURN;
      work->clwkExitButton = work->clwkBarIcon[PBT_RETURN];
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[PBT_RETURN] , APP_COMMON_BARICON_RETURN_ON );
      work->ktst = GFL_APP_KTST_KEY;
    }
  }
  else if( trg & PAD_BUTTON_X )
  {
    if( work->canExit == TRUE )
    {
      work->selectState = PSSEL_EXIT;
      work->clwkExitButton = work->clwkBarIcon[PBT_EXIT];
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[PBT_EXIT] , APP_COMMON_BARICON_EXIT_ON );
      work->ktst = GFL_APP_KTST_KEY;
    }
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面 TP操作
//--------------------------------------------------------------------------
static void PLIST_SelectPokeUpdateTP( PLIST_WORK *work )
{
  //プレートと当たり
  int ret;
  
  {
    //当たり判定作成
    u8 i;
    u8 cnt = 0;
    u8 plateIdx[PLIST_LIST_MAX];
    GFL_UI_TP_HITTBL hitTbl[PLIST_LIST_MAX+1];
    
    for( i=0;i<PLIST_LIST_MAX;i++ )
    {
      if( PLIST_PLATE_CanSelect( work , work->plateWork[i] ) == TRUE )
      {
        plateIdx[cnt] = i;
        PLIST_PLATE_GetPlateRect( work , work->plateWork[i] , &hitTbl[cnt] );
        cnt++;
      }
    }
    hitTbl[cnt].circle.code = GFL_UI_TP_HIT_END;

    ret = GFL_UI_TP_HitTrg( hitTbl );

    if( ret != GFL_UI_TP_HIT_NONE )
    {
      if( work->canSelectEgg == FALSE && ret <= PL_SEL_POS_POKE6 &&
          PLIST_PLATE_IsEgg( work , work->plateWork[ret] ) == TRUE )
      {
        PMSND_PlaySystemSE( PLIST_SND_ERROR );
      }
      else
      {
        const PL_SELECT_POS befPos = work->pokeCursor;
        work->selectState = PSSEL_SELECT;
        work->pokeCursor = plateIdx[ret];

        //PLIST_SelectPokeSetCursor( work , work->pokeCursor );
        if( befPos <= PL_SEL_POS_POKE6 )
        {
          PLIST_PLATE_SetActivePlate( work , work->plateWork[befPos] , FALSE );
        }
        else
        {
          //バトル時決定・戻る
          const u8 idx = befPos - PL_SEL_POS_ENTER;
          PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , FALSE );
        }

        if( work->pokeCursor <= PL_SEL_POS_POKE6 )
        {
          PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , TRUE );
        }
        else
        {
          //バトル時決定・戻る
          const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
          PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , TRUE );
          
          GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
        }
        work->platePalAnmCnt = PLIST_PLATE_ACTIVE_ANM_CNT;
      }

      work->ktst = GFL_APP_KTST_TOUCH;
    }
  }
  
  //リターンキャンセルとかと当たり
  if( ret == GFL_UI_TP_HIT_NONE )
  {
    GFL_UI_TP_HITTBL hitTbl[PSSEL_BUTTON_NUM+1] =
    {
      { 0,0,0,0 },
      { 0,0,0,0 },
      { 0,0,0,0 },
      { GFL_UI_TP_HIT_END,0,0,0 },
    };
    
    //バトル用
    if( PLIST_UTIL_IsBattleMenu( work ) == TRUE )
    {
      if( PLIST_UTIL_IsBattleMenu_CanReturn( work ) == TRUE )
      {
        hitTbl[PSSEL_RETURN].rect.top    = PLIST_BATTLE_BUTTON_CANCEL_Y*8;    
        hitTbl[PSSEL_RETURN].rect.bottom = (PLIST_BATTLE_BUTTON_CANCEL_Y+3)*8;
        hitTbl[PSSEL_RETURN].rect.left   = PLIST_BATTLE_BUTTON_CANCEL_X*8;
        hitTbl[PSSEL_RETURN].rect.right  = 255;//(PLIST_BATTLE_BUTTON_CANCEL_X+10)*8;

        hitTbl[PSSEL_DECIDE].rect.top    = PLIST_BATTLE_BUTTON_DECIDE_Y*8;
        hitTbl[PSSEL_DECIDE].rect.bottom = (PLIST_BATTLE_BUTTON_DECIDE_Y+3)*8;
        hitTbl[PSSEL_DECIDE].rect.left   = PLIST_BATTLE_BUTTON_DECIDE_X*8;     
        hitTbl[PSSEL_DECIDE].rect.right  = (PLIST_BATTLE_BUTTON_DECIDE_X+10)*8;
      }
      else
      {
        hitTbl[PSSEL_DECIDE].rect.top    = PLIST_BATTLE_BUTTON_DECIDE_Y*8;
        hitTbl[PSSEL_DECIDE].rect.bottom = (PLIST_BATTLE_BUTTON_DECIDE_Y+3)*8;
        hitTbl[PSSEL_DECIDE].rect.left   = (PLIST_BATTLE_BUTTON_DECIDE_X+10)*8;
        hitTbl[PSSEL_DECIDE].rect.right  = 255;//(PLIST_BATTLE_BUTTON_DECIDE_X+10+10)*8;
      }
    }
    else
    {
      if( PLIST_UTIL_CanReturn( work ) == TRUE )
      {
        hitTbl[PSSEL_RETURN].rect.top    = PLIST_BARICON_Y;
        hitTbl[PSSEL_RETURN].rect.bottom = PLIST_BARICON_Y + 24;
        hitTbl[PSSEL_RETURN].rect.left   = PLIST_BARICON_RETURN_X_BAR;
        hitTbl[PSSEL_RETURN].rect.right  = PLIST_BARICON_RETURN_X_BAR + 24;
      }
      if( work->canExit == TRUE )
      {
        hitTbl[PSSEL_EXIT  ].rect.top    = PLIST_BARICON_Y;
        hitTbl[PSSEL_EXIT  ].rect.bottom = PLIST_BARICON_Y + 24;
        hitTbl[PSSEL_EXIT  ].rect.left   = PLIST_BARICON_EXIT_X;
        hitTbl[PSSEL_EXIT  ].rect.right  = PLIST_BARICON_EXIT_X + 24;
      }
    }
    
    ret = GFL_UI_TP_HitTrg( hitTbl );

    if( ret != GFL_UI_TP_HIT_NONE )
    {
      work->selectState = ret;
      work->ktst = GFL_APP_KTST_TOUCH;
      
    }
  }
  
}

//--------------------------------------------------------------------------
//  ポケモン選択画面　カーソル設定
//--------------------------------------------------------------------------
static void PLIST_SelectPokeSetCursor( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  GFL_CLACTPOS curPos;
  PLIST_PLATE_GetPlatePosition(work , work->plateWork[pos] , &curPos );
  GFL_CLACT_WK_SetPos( work->clwkCursor[0] , &curPos , CLSYS_DRAW_MAIN );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , TRUE );
  GFL_CLACT_WK_SetBgPri( work->clwkCursor[0] , 3 );

  if( pos == PL_SEL_POS_POKE1 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_NORMAL_A );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[0] , PCA_NORMAL_B );
  }
}

//--------------------------------------------------------------------------
//  ポケモン選択画面　入れ替えようカーソル設定
//--------------------------------------------------------------------------
static void PLIST_SelectPokeSetCursor_Change( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  GFL_CLACTPOS curPos;
  PLIST_PLATE_GetPlatePosition(work , work->plateWork[pos] , &curPos );
  GFL_CLACT_WK_SetPos( work->clwkCursor[1] , &curPos , CLSYS_DRAW_MAIN );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , TRUE );
  GFL_CLACT_WK_SetBgPri( work->clwkCursor[1] , 2 );

  if( pos == PL_SEL_POS_POKE1 )
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[1] , PCA_CHANGE_A );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->clwkCursor[1] , PCA_CHANGE_B );
  }
}

static void PLIST_PLATE_SetActivePlatePos( PLIST_WORK *work , const PL_SELECT_POS pos )
{
  //現在未使用。プレート選択時移動
  const s8 moveVal[7][6][2] =
  {
    {
      { 0,0 },{ 1,0 },
      { 0,1 },{ 1,1 },
      { 0,1 },{ 1,1 },
    },
    {
      { -1,0 },{ 0,0 },
      { -1,1 },{ 0,1 },
      { -1,1 },{ 0,1 },
    },
    {
      { 0,-1 },{ 1,-1 },
      { 0,0 } ,{ 1,0 },
      { 0,1 } ,{ 1,1 },
    },
    {
      { -1,-1 },{ 0,-1 },
      { -1,0 } ,{ 0,0 },
      { -1,1 } ,{ 0,1 },
    },
    {
      { 0,-1 },{ 1,-1 },
      { 0,-1 },{ 1,-1 },
      { 0,0 } ,{ 1,0 },
    },
    {
      { -1,-1 },{ 0,-1 },
      { -1,-1 },{ 0,-1 },
      { -1,0 } ,{ 0,0 },
    },
    {
      { 0,0 },{ 0,0 },
      { 0,0 },{ 0,0 },
      { 0,0 },{ 0,0 },
    },
  };
  u8 i;
#if DEBUG_ONLY_FOR_ariizumi_nobuhiko | DEBUG_ONLY_FOR_ibe_mana
  GFL_BG_FillScreen( PLIST_BG_PARAM , 0 ,16,0,48,21,GFL_BG_SCRWRT_PALNL);
  GFL_BG_FillScreen( PLIST_BG_PLATE , 0 ,16,0,48,21,GFL_BG_SCRWRT_PALNL);
  for( i=0;i<6;i++ )
  {
    if( i%2 == work->pokeCursor%2 )
    {
      PLIST_PLATE_MovePlateXY( work , work->plateWork[i] , /*moveVal[pos][i][0]*/0 , moveVal[pos][i][1] );
    }
    else
    {
      PLIST_PLATE_MovePlateXY( work , work->plateWork[i] , 0 , 0 );
    }
  }
#endif
}

#pragma mark [>MenuSelect
//--------------------------------------------------------------------------
//  メニュー選択画面
//--------------------------------------------------------------------------
static void PLIST_SelectMenu( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_INIT:
    PLIST_SelectMenuInit( work );
    work->subSeq = PSSS_MAIN;
    break;
    
  case PSSS_MAIN:
    PLIST_MENU_UpdateMenu( work , work->menuWork );
    {
      const PLIST_MENU_ITEM_TYPE ret = PLIST_MENU_IsFinish( work , work->menuWork );
      if( ret != PMIT_NONE )
      {
        work->menuRet = ret;
        work->subSeq = PSSS_TERM;
      }
      if( work->plData->isNetErr == TRUE )
      {
        work->subSeq = PSSS_TERM;
      }
    }
    break;
    
  case PSSS_TERM:
    PLIST_SelectMenuTerm( work );
    
    PLIST_SelectMenuExit( work );
    break;
  }
  
}

//--------------------------------------------------------------
//	メニュー初期化
//--------------------------------------------------------------
static void PLIST_SelectMenuInit( PLIST_WORK *work )
{
  work->mainSeq = PSMS_MENU;
  work->subSeq  = PSSS_INIT;

  //BG・プレート・パラメータを見えにくくする
  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , 8 );
  //戻るアイコンだけカラー効果がかからないようにWindowを使う
  //戻るアイコンはBGに乗ったので対応不要。
  G2_SetWnd0Position( PLIST_BARICON_RETURN_X_MENU , PLIST_BARICON_Y ,
                      PLIST_BARICON_RETURN_X_MENU + 24, PLIST_BARICON_Y + 24 );
  G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , FALSE );
  G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , TRUE );
  work->isActiveWindowMask = TRUE;

  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  }
  GFL_BG_LoadScreenV_Req(PLIST_BG_MENU);
  
  GFL_UI_SetTouchOrKey( work->ktst );
}

//--------------------------------------------------------------
//	メニュー開放
//--------------------------------------------------------------
static void PLIST_SelectMenuTerm( PLIST_WORK *work )
{
  PLIST_MENU_CloseMenu( work , work->menuWork );
  PLIST_MSG_CloseWindow( work , work->msgWork );
  //バトル処理
  if( PLIST_UTIL_IsBattleMenu(work) == TRUE ||
      PLIST_UTIL_CanReturn(work) == FALSE )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , TRUE );
  }
  
  if( work->menuRet != PMIT_ITEM &&
      work->menuRet != PMIT_MAIL )
  {
    //再度メニュー開くのでここでは消さない
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_OBJ , 
                      GX_BLEND_PLANEMASK_BG3 ,
                      16 , 10 );
    work->isActiveWindowMask = FALSE;
  }
  GFL_BG_ClearScreenCodeVReq(PLIST_BG_MENU,0);
  work->ktst = GFL_UI_CheckTouchOrKey();

  
}

//--------------------------------------------------------------
//	メニュー戻り分岐
//--------------------------------------------------------------
static void PLIST_SelectMenuExit( PLIST_WORK *work )
{
  if( work->plData->isNetErr == TRUE )
  {
    work->mainSeq = PSMS_FADEOUT;
    return;
  }
  switch( work->menuRet )
  {
  case PMIT_CLOSE:
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;

  case PMIT_CHANGE:
    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    PLIST_SelectPokeSetCursor_Change( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->changeTarget = work->pokeCursor;
    work->mainSeq = PSMS_CHANGE_POKE;
    work->canExit = FALSE;
    
    PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
    PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_02 );
    //ここはモード別処理ではなく、ただのセレクト初期化
    PLIST_SelectPokeInit( work );
    
    break;

  case PMIT_ITEM:
    {
      PLIST_MENU_ITEM_TYPE itemArr[4] = {PMIT_GIVE,PMIT_TAKE,PMIT_CLOSE,PMIT_END_LIST};
      PLIST_SelectMenuInit( work );
      //先にメニューを開くと、bmpの確保順がずれてゴミが見える・・・
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_02 );
      PLIST_MENU_OpenMenu( work , work->menuWork , itemArr );
      GFL_BG_LoadScreenReq( PLIST_BG_MENU );  //一瞬見えるカーソルプレート対策
    }
    break;
    
  case PMIT_MAIL:
    {
      PLIST_MENU_ITEM_TYPE itemArr[4] = {PMIT_MAIL_READ,PMIT_MAIL_TAKE,PMIT_CLOSE,PMIT_END_LIST};
      PLIST_SelectMenuInit( work );
      //先にメニューを開くと、bmpの確保順がずれてゴミが見える・・・
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MENU );
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_03_02 );
      PLIST_MENU_OpenMenu( work , work->menuWork , itemArr );
      GFL_BG_LoadScreenReq( PLIST_BG_MENU );  //一瞬見えるカーソルプレート対策
    }
    break;

  case PMIT_STATSU:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_STATUS;
    break;
    
  case PMIT_RET_JOIN:
    {
      BOOL isFullMember = FALSE;
      REGULATION *reg = (REGULATION*)work->plData->reg;
      const PLIST_PLATE_CAN_BATTLE ret = PLIST_PLATE_CanJoinBattle( work , work->plateWork[work->pokeCursor] );
      if( ret == PPCB_OK )
      {
        PLIST_PLATE_SetBattleOrder( work , work->plateWork[work->pokeCursor] , work->btlJoinNum );
        work->btlJoinNum++;
        PLIST_InitMode_Select( work );
        if( work->btlJoinNum == reg->NUM_HI )
        {
          isFullMember = TRUE;
        }
        work->mainSeq = PSMS_SELECT_POKE;
      }
      else
      if( ret == PPCB_NG_SAME_MONSNO )
      {
        PLIST_MessageWaitInit( work , mes_pokelist_07_01 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        PMSND_PlaySystemSE( PLIST_SND_ERROR );
      }
      else
      if( ret == PPCB_NG_SAME_ITEM )
      {
        PLIST_MessageWaitInit( work , mes_pokelist_07_02 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        PMSND_PlaySystemSE( PLIST_SND_ERROR );
      }
      else
      if( ret == PPCB_NG_OVER_NUM )
      {
        PLIST_MessageWaitInit( work , mes_pokelist_04_62_1 + (reg->NUM_HI-1) , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        PMSND_PlaySystemSE( PLIST_SND_ERROR );
      }
      
      work->selectPokePara = NULL;
      if( isFullMember == TRUE )
      {
        PLIST_PLATE_SetActivePlate( work , work->plateWork[work->pokeCursor] , FALSE );
        work->pokeCursor = PL_SEL_POS_ENTER;
        if( work->ktst == GFL_APP_KTST_KEY )
        {

          //バトル時決定・戻る
          const u8 idx = work->pokeCursor - PL_SEL_POS_ENTER;
          PLIST_MENU_SetActiveMenuWin_BattleMenu( work->btlMenuWin[idx] , TRUE );
          
          GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
        }
      }
      else
      {
        if( work->ktst == GFL_APP_KTST_KEY )
        {
          PLIST_SelectPokeSetCursor( work , work->pokeCursor );
        }
      }
    }
    break;
    
  case PMIT_JOIN_CANCEL:
    {
      u8 i;
      const PLIST_PLATE_BATTLE_ORDER order = PLIST_PLATE_GetBattleOrder( work->plateWork[work->pokeCursor] );
      for( i=0;i<PLIST_LIST_MAX;i++ )
      {
        const PLIST_PLATE_BATTLE_ORDER checkOrder = PLIST_PLATE_GetBattleOrder( work->plateWork[i] );
        if( checkOrder <= PPBO_JOIN_6 && checkOrder > order )
        {
          PLIST_PLATE_SetBattleOrder( work , work->plateWork[i] , checkOrder-1 );
        }
      }
      PLIST_PLATE_SetBattleOrder( work , work->plateWork[work->pokeCursor] , PPBO_JOIN_OK );
      work->btlJoinNum--;
    }

    if( work->ktst == GFL_APP_KTST_KEY )
    {
      PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    }
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;
    
  case PMIT_TAKE:
    {
      const u32 itemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
      //預かるので持たせるアイテム０
      PLIST_ITEM_CangeAruseusuForm( work , work->selectPokePara , 0 );
      PLIST_ITEM_CangeInsekutaForm( work , work->selectPokePara , 0 );
      
      if( itemNo == 0 )
      {
        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
        PLIST_MessageWaitInit( work , mes_pokelist_04_29 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
      }
      else
      {
        if( PLIST_CheckBagItemNum( work , itemNo ) == ITEM_MAX_NORMAL )
        {
          PLIST_MessageWaitInit( work , mes_pokelist_04_31 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        }
        else
        {
          //フォルムチェンジ前にReDrawParamが必要
          PLIST_SetPokeItem( work , work->selectPokePara , 0 );
          PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );

          PLIST_MSG_CreateWordSet( work , work->msgWork );
          PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
          PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , itemNo );
          
          if( PLIST_DEMO_CheckGirathnaToAnother( work , work->selectPokePara ) == TRUE )
          {
            //ギラティナ・フォルムチェンジ
            PLIST_DEMO_ChangeGirathinaToAnother( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_30 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ANOTHER;
          }
          else
          {
            PLIST_MessageWaitInit( work , mes_pokelist_04_30 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          }
          PLIST_MSG_DeleteWordSet( work , work->msgWork );
        }
      }
    }
    break;

  case PMIT_ENTER:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_NORMAL;
    break;

  case PMIT_GIVE:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_ITEMSET;
    break;

  case PMIT_MAIL_READ:    //メールを読む
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_MAILREAD;
    break;
    
  case PMIT_MAIL_TAKE:    //メールを取る
    PLIST_MessageWaitInit( work , mes_pokelist_04_01 , FALSE , PLIST_MSGCB_TakeMail );
    break;

  case PMIT_LEAVE:
    work->mainSeq = PSMS_FADEOUT;
    work->plData->ret_sel = work->pokeCursor;
    work->plData->ret_mode = PL_RET_NORMAL;
    break;

  case PMIT_WAZA_1:
  case PMIT_WAZA_2:
  case PMIT_WAZA_3:
  case PMIT_WAZA_4:
    if( work->plData->mode == PL_MODE_ITEMUSE )
    {
      //PP回復アイテムの選択
      const BOOL canUse = STATUS_RCV_RecoverCheck( work->selectPokePara , work->plData->item , work->menuRet-PMIT_WAZA_1 , work->heapId );
      if( canUse == TRUE )
      {
        const u32 wazaNo = PP_Get(work->selectPokePara , ID_PARA_waza1+(work->menuRet-PMIT_WAZA_1) , NULL);
        GFL_BG_LoadScreenReq( PLIST_BG_MENU );  //ちらつきぼうし！
        //中に技IDを渡せないので外でWORDSET作成
        PLIST_ITEM_MSG_UseItemFunc( work , wazaNo );
        
        //実際に消費と適用
        STATUS_RCV_Recover( work->selectPokePara , work->plData->item , work->menuRet-PMIT_WAZA_1 , work->plData->zone_id , work->heapId );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
        PLIST_SubBagItem( work , work->plData->item );
        PMSND_PlaySystemSE( PLIST_SND_RECOVER );
      }
      else
      {
        PLIST_ITEM_MSG_CanNotUseItemContinue( work );
      }
    }
    else
    {
      //フィールド秘伝技
      const u32 selSkill = PLIST_UTIL_CheckFieldWaza( work->selectPokePara , work->menuRet-PMIT_WAZA_1 );
      if( selSkill == PL_RET_MILKNOMI || selSkill == PL_RET_TAMAGOUMI )
      {
        if( ZONEDATA_CheckFieldSkillUse( work->plData->zone_id ) == FALSE )
        {
          PLIST_MessageWaitInit( work , mes_pokelist_04_44 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
        }
        else
        {
          //タマゴうみとミルクのみは回復処理
          const u32 hpMax = PP_Get( work->selectPokePara , ID_PARA_hpmax , NULL );
          const u32 hpNow = PP_Get( work->selectPokePara , ID_PARA_hp , NULL );
          if( hpNow > hpMax/5 )
          {
            PLIST_SelectPokeSetCursor( work , work->pokeCursor );
            PLIST_SelectPokeSetCursor_Change( work , work->pokeCursor );

            work->selectPokePara = NULL;
            work->useTarget = work->pokeCursor;
            work->mainSeq = PSMS_USE_POKE;
            work->canExit = FALSE;
            
            PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR );
            PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_02_08 );
            //ここはモード別処理ではなく、ただのセレクト初期化
            PLIST_SelectPokeInit( work );
          }
          else
          {
            PLIST_MessageWaitInit( work , mes_pokelist_04_71 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          }
        }
      }
      else
      {
        const FLDSKILL_RET ret = FLDSKILL_CheckUseSkill( selSkill-PL_RET_IAIGIRI , &work->plData->scwk );
        switch( ret )
        {
        case FLDSKILL_RET_USE_OK:  // 使用可能
          work->mainSeq = PSMS_FADEOUT;
          work->plData->ret_sel = work->pokeCursor;
          work->plData->ret_mode = selSkill;
          break;
        case FLDSKILL_RET_USE_NG:    // 使用不可（ここでは使えません）
          PLIST_MessageWaitInit( work , mes_pokelist_04_44 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          break;
        case FLDSKILL_RET_NO_BADGE:    // 使用不可・バッジなし
          PLIST_MessageWaitInit( work , mes_pokelist_04_26 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          break;
        case FLDSKILL_RET_COMPANION:    // 使用不可・連れ歩き
          PLIST_MessageWaitInit( work , mes_pokelist_09_01 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          break;
        case FLDSKILL_RET_PLAYER_SWIM:    // 使用不可・なみのり中
          PLIST_MessageWaitInit( work , mes_pokelist_04_42 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          break;
        }
      }
    }
    break;

  default:
    GF_ASSERT_MSG(0,"まだ作ってない！\n");

    PLIST_SelectPokeSetCursor( work , work->pokeCursor );
    work->selectPokePara = NULL;
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
    break;
  }
}

//メッセージ待ち
#pragma mark [>MessageWait
static void PLIST_MessageWait( PLIST_WORK *work )
{
  if( PLIST_MSG_IsFinishMessage( work , work->msgWork ) == TRUE )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      work->ktst = GFL_APP_KTST_KEY;
    }
    if( GFL_UI_TP_GetTrg() == TRUE )
    {
      work->ktst = GFL_APP_KTST_TOUCH;
    }
    
    work->msgCallBack(work);
  }
}

void PLIST_MessageWaitInit( PLIST_WORK *work , u32 msgId , const BOOL isWaitKey , PLIST_CallbackFunc msgCallBack )
{
  work->mainSeq = PSMS_MSG_WAIT;
  work->isMsgWaitKey = isWaitKey;
  work->msgCallBack = msgCallBack;
  PLIST_MSG_OpenWindow( work , work->msgWork , PMT_MESSAGE );
  PLIST_MSG_DrawMessageStream( work , work->msgWork , msgId , isWaitKey );
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_RETURN] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[PBT_EXIT] , FALSE );
  
}

//はい・いいえ処理
#pragma mark [>YesNoWait
static void PLIST_YesNoWait( PLIST_WORK *work )
{
  PLIST_MENU_ITEM_TYPE ret;
  PLIST_MENU_UpdateMenu( work , work->menuWork );
  
  ret = PLIST_MENU_IsFinish( work , work->menuWork );
  if( ret != PMIT_NONE )
  {
    work->yesNoCallBack(work,ret);
    PLIST_MENU_CloseMenu( work , work->menuWork );
  }

}

static void PLIST_YesNoWaitInit( PLIST_WORK *work , PLIST_CallbackFuncYesNo yesNoCallBack )
{
  work->mainSeq = PSMS_YESNO_WAIT;
  work->yesNoCallBack = yesNoCallBack;

  PLIST_MENU_OpenMenu_YesNo( work , work->menuWork );
  
}

//入れ替えアニメ
#pragma mark [>ChangeAnm
//--------------------------------------------------------------
//	入れ替えアニメ
//--------------------------------------------------------------
static void PLIST_ChangeAnime( PLIST_WORK *work )
{
  switch( work->subSeq )
  {
  case PSSS_MOVE_OUT:
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->changeTarget] , work->anmCnt );
    work->anmCnt += PLIST_CHANGE_ANM_VALUE;
    PLIST_PLATE_MovePlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_MovePlate( work , work->plateWork[work->changeTarget] , work->anmCnt );

    if( work->anmCnt >= PLIST_CHANGE_ANM_COUNT )
    {
      PokeParty_ExchangePosition( work->plData->pp , work->pokeCursor , work->changeTarget , work->heapId);
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, work->pokeCursor);
        PLIST_PLATE_ResetParam(work , work->plateWork[work->pokeCursor] , pp , work->anmCnt );
      }
      {
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer(work->plData->pp, work->changeTarget);
        PLIST_PLATE_ResetParam(work , work->plateWork[work->changeTarget] , pp , work->anmCnt );
      }
      //入れ替え
      work->subSeq = PSSS_MOVE_IN;
      PMSND_PlaySystemSE( PLIST_SND_CHANGE_IN );
    }
    break;

  case PSSS_MOVE_IN:
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_ClearPlate( work , work->plateWork[work->changeTarget] , work->anmCnt );
    work->anmCnt -= PLIST_CHANGE_ANM_VALUE;
    PLIST_PLATE_MovePlate( work , work->plateWork[work->pokeCursor  ] , work->anmCnt );
    PLIST_PLATE_MovePlate( work , work->plateWork[work->changeTarget] , work->anmCnt );

    if( work->anmCnt <= 0 )
    {
      //終了
      PLIST_ChangeAnimeTerm( work );
    }
    
    break;
  }
}

//--------------------------------------------------------------
//	入れ替えアニメ初期化
//--------------------------------------------------------------
static void PLIST_ChangeAnimeInit( PLIST_WORK *work )
{
  work->mainSeq = PSMS_CHANGE_ANM;
  work->subSeq = PSSS_MOVE_OUT;
  work->anmCnt = 0;

  PMSND_PlaySystemSE( PLIST_SND_CHANGE_OUT );

  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[0] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
}

//--------------------------------------------------------------
//	入れ替えアニメ開放
//--------------------------------------------------------------
static void PLIST_ChangeAnimeTerm( PLIST_WORK *work )
{
  work->selectPokePara = NULL;
  work->mainSeq = PSMS_SELECT_POKE;

  PLIST_SelectPokeSetCursor( work , work->pokeCursor );
  PLIST_PLATE_SetActivePlate( work , work->plateWork[work->changeTarget] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkCursor[1] , FALSE );
  work->changeTarget = PL_SEL_POS_MAX;
  work->plData->ret_sel = work->pokeCursor;

  PLIST_InitMode_Select( work );
  
}

//--------------------------------------------------------------
//	入れ替えアニメプレート更新
//--------------------------------------------------------------
static void PLIST_ChangeAnimeUpdatePlate( PLIST_WORK *work )
{
}

#pragma mark [>util
//--------------------------------------------------------------
//	指定技箇所にフィールド技があるか？
//--------------------------------------------------------------
u32 PLIST_UTIL_CheckFieldWaza( const POKEMON_PARAM *pp , const u8 wazaPos )
{
  const u32 wazaID = PP_Get( pp , ID_PARA_waza1+wazaPos , NULL );
  
  switch( wazaID )
  {
  case WAZANO_IAIGIRI: //いあいぎり(15)
    return PL_RET_IAIGIRI;
    break;
  case WAZANO_SORAWOTOBU: //そらをとぶ(19)
    return PL_RET_SORAWOTOBU;
    break;
  case WAZANO_NAMINORI: //なみのり(57)
    return PL_RET_NAMINORI;
    break;
  case WAZANO_KAIRIKI: //かいりき(70)
    return PL_RET_KAIRIKI;
    break;
  case WAZANO_TAKINOBORI: //たきのぼり(127)
    return PL_RET_TAKINOBORI;
    break;
  case WAZANO_HURASSYU: //フラッシュ(148)
    return PL_RET_FLASH;
    break;
  case WAZANO_TEREPOOTO: //テレポート(100)
    return PL_RET_TELEPORT;
    break;
  case WAZANO_ANAWOHORU: //あなをほる(91)
    return PL_RET_ANAWOHORU;
    break;
  case WAZANO_AMAIKAORI: //あまいかおり(230)
    return PL_RET_AMAIKAORI;
    break;
  case WAZANO_OSYABERI: //おしゃべり(448)
    return PL_RET_OSYABERI;
    break;
  case WAZANO_DAIBINGU: //ダイビング(291)
    return PL_RET_DIVING;
    break;
  case WAZANO_MIRUKUNOMI: //ミルクのみ(208)
    return PL_RET_MILKNOMI;
    break;
  case WAZANO_TAMAGOUMI: //タマゴうみ(135)
    return PL_RET_TAMAGOUMI;
    break;

  default:
    return 0;
    break;
  }
}

//--------------------------------------------------------------
//	バトルの参加選択か？
//--------------------------------------------------------------
const BOOL PLIST_UTIL_IsBattleMenu( const PLIST_WORK *work )
{
  if( work->plData->mode == PL_MODE_BATTLE ||
      work->plData->mode == PL_MODE_BATTLE_SUBWAY ||
      work->plData->mode == PL_MODE_BATTLE_EXAMINATION )
  {
    return TRUE;
  }
  return FALSE;
}
//--------------------------------------------------------------
//	バトルの参加選択で戻ることができるか？
//--------------------------------------------------------------
const BOOL PLIST_UTIL_IsBattleMenu_CanReturn( const PLIST_WORK *work )
{
  if( work->plData->mode == PL_MODE_BATTLE )
  {
    return FALSE;
  }
  return TRUE;
}

const BOOL PLIST_UTIL_CanReturn( const PLIST_WORK *work )
{
  //ぐるぐるはダメ
  if( work->plData->mode == PL_MODE_GURU2 )
  {
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
//	今渡されている技が覚えられるか？
//--------------------------------------------------------------
const PLIST_SKILL_CAN_LEARN PLIST_UTIL_CheckLearnSkill( PLIST_WORK *work , const POKEMON_PARAM *pp , const u8 idx )
{
  u8 i;
  BOOL isEmpty = FALSE;
  if( work->isSetWazaMode == TRUE )
  {
    u16 wazaArr[3];
    
    if( work->plData->waza == PL_SP_WAZANO_STRONGEST )
    {
      //最強技
      wazaArr[0] = WAZANO_HAADOPURANTO;
      wazaArr[1] = WAZANO_BURASUTOBAAN;
      wazaArr[2] = WAZANO_HAIDOROKANON;
    }
    else
    if( work->plData->waza == PL_SP_WAZANO_COALESCENCE )
    {
      //合体技
      wazaArr[0] = WAZANO_KUSANOTIKAI;
      wazaArr[1] = WAZANO_HONOONOTIKAI;
      wazaArr[2] = WAZANO_MIZUNOTIKAI;
    }
    else
    {
      wazaArr[0] = work->plData->waza;
      wazaArr[1] = work->plData->waza;
      wazaArr[2] = work->plData->waza;
    }
    //同じ技がある？
    for( i=0;i<4;i++ )
    {
      const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+i , NULL );
      if( wazaNo == wazaArr[0] ||
          wazaNo == wazaArr[1] ||
          wazaNo == wazaArr[2] )
      {
        return LSCL_LEARN;
      }
      if( wazaNo == 0 )
      {
        isEmpty = TRUE;
      }
    }

    //イベントなど特殊な技覚え(外からBitでもらう)
    if( work->plData->wazaLearnBit & 1<<idx )
    {
      if( isEmpty == TRUE )
      {
        return LSCL_OK;
      }
      return LSCL_OK_FULL;
    }
    else
    {
      return LSCL_NG;
    }
  }
  else
  {
    //同じ技がある？
    for( i=0;i<4;i++ )
    {
      const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+i , NULL );
      if( wazaNo == work->plData->waza )
      {
        return LSCL_LEARN;
      }
      if( wazaNo == 0 )
      {
        isEmpty = TRUE;
      }
    }

    if( work->plData->item != 0 )
    {
      const int machineNo = ITEM_GetWazaMashineNo( work->plData->item );
      if( machineNo != 0xFF &&
          PP_CheckWazaMachine( pp , machineNo ) == TRUE )
      {
        if( isEmpty == TRUE )
        {
          return LSCL_OK;
        }
        return LSCL_OK_FULL;
      }
      else
      {
        return LSCL_NG;
      }
    }
    else
    {
      if( isEmpty == TRUE )
      {
        return LSCL_OK;
      }
      return LSCL_OK_FULL;
    }
  }
}

//--------------------------------------------------------------
//	指定アイテムで進化できるか(モンスターナンバー返し
//--------------------------------------------------------------
const u16 PLIST_UTIL_CheckItemEvolution( PLIST_WORK *work , POKEMON_PARAM *pp , const u16 itemNo )
{
  return SHINKA_Check( work->plData->pp , pp , SHINKA_TYPE_ITEM_CHECK , itemNo , work->plData->season , NULL , work->heapId );
}

//--------------------------------------------------------------
//	文字の描画
//--------------------------------------------------------------
void PLIST_UTIL_DrawStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , srcStr , work->fontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}
void PLIST_UTIL_DrawStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , srcStr , work->sysFontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}
void PLIST_UTIL_DrawStrFuncDirect( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , srcStr , work->sysFontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}
//--------------------------------------------------------------
//	文字の描画(値用
//--------------------------------------------------------------
void PLIST_UTIL_DrawValueStrFunc( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , dstStr , work->fontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
void PLIST_UTIL_DrawValueStrFuncSys( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX , posY , dstStr , work->sysFontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
void PLIST_UTIL_DrawValueStrFuncDirect( PLIST_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );

  PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( bmpWin ) ,
          posX , posY , dstStr , work->sysFontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
#pragma mark [>message callback
void PLIST_MSGCB_ReturnSelectCommon( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );
  work->mainSeq = PSMS_SELECT_POKE;
  PLIST_InitMode_Select( work );
}

void PLIST_MSGCB_ExitCommon( PLIST_WORK *work )
{
  work->mainSeq = PSMS_FADEOUT;
}
//アイテムを連続使用するかのチェック
void PLIST_MSGCB_CheckItemUseContinue( PLIST_WORK *work )
{
  if( PLIST_CheckBagItemNum( work , work->plData->item ) > 0 )
  {
    PLIST_MSG_CloseWindow( work , work->msgWork );
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
  }
  else
  {
    PLIST_MSG_CloseWindow( work , work->msgWork );
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 0 , work->plData->item );
    PLIST_MessageWaitInit( work , mes_pokelist_13_08 , TRUE , PLIST_MSGCB_ExitCommon );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
}
//技がいっぱいで、忘れるかどうか？の選択肢
void PLIST_MSGCB_ForgetSkill_ForgetCheck( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PLIST_MSGCB_ForgetSkill_ForgetCheckCB );
}

static void PLIST_MSGCB_ForgetSkill_ForgetCheckCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //はい→どのわざをわすれますか？
    if( work->plData->mode == PL_MODE_ITEMUSE ||
        work->plData->mode == PL_MODE_LVUPWAZASET_RET )
    {
      //不思議なアメだった
      work->plData->ret_mode = PL_RET_LVUP_WAZASET;
    }
    else
    {
      //技マシン
      work->plData->ret_mode = PL_RET_WAZASET;
    }
    PLIST_MessageWaitInit( work , mes_pokelist_04_09 , TRUE , PLIST_MSGCB_ExitCommon );
  }
  else
  {
    //いいえ→覚えるのをあきらめますか？
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    PLIST_MessageWaitInit( work , mes_pokelist_04_07 , FALSE , PLIST_MSGCB_ForgetSkill_SkillCancel );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
}

//覚えるのをあきらめますか？
static void PLIST_MSGCB_ForgetSkill_SkillCancel( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PLIST_MSGCB_ForgetSkill_SkillCancelCB );
}

static void PLIST_MSGCB_ForgetSkill_SkillCancelCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //はい→覚えずに終わった
    work->plData->ret_mode = PL_RET_BAG;
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    if( work->plData->mode == PL_MODE_ITEMUSE ||
        work->plData->mode == PL_MODE_LVUPWAZASET_RET )
    {
      //不思議なアメだった
      PLIST_MessageWaitInit( work , mes_pokelist_04_08 , TRUE , PLIST_MSGCB_LvUp_CheckLearnWaza );
    }
    else
    {
      //技マシン
      PLIST_MessageWaitInit( work , mes_pokelist_04_08 , TRUE , PLIST_MSGCB_ExitCommon );
    }
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
  {
    //いいえ 初めから
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );
    PLIST_MessageWaitInit( work , mes_pokelist_04_06 , FALSE , PLIST_MSGCB_ForgetSkill_ForgetCheck );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
}

//12のポカン
static void PLIST_MSGCB_ForgetSkill_SkillForget( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );
/*
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
  {
    const u32 wazaNo = PP_Get( work->selectPokePara , ID_PARA_waza1+work->plData->waza_pos , NULL );
    PLIST_MSG_CreateWordSet( work , work->msgWork );
    PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
    PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , wazaNo );
    PLIST_MessageWaitInit( work , mes_pokelist_04_10 , TRUE , PLIST_MSGCB_ForgetSkill_SkillForget );
    PLIST_MSG_DeleteWordSet( work , work->msgWork );
  }
  else
*/
  {
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  PLIST_MSG_AddWordSet_SkillName( work , work->msgWork , 1 , work->plData->waza );

  if( work->plData->mode == PL_MODE_ITEMUSE ||
      work->plData->mode == PL_MODE_LVUPWAZASET_RET )
  {
    //不思議なアメだった
    PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PLIST_MSGCB_LvUp_CheckLearnWaza );
  }
  else
  {
    //技マシン
    PLIST_MessageWaitInit( work , mes_pokelist_04_11 , TRUE , PLIST_MSGCB_ExitCommon );
    PLIST_UseWazaMachine( work , work->selectPokePara );
    work->plData->ret_mode = PL_RET_BAG;
  }

  PLIST_MSG_DeleteWordSet( work , work->msgWork );
  PLIST_LearnSkillFull( work , work->selectPokePara , work->plData->waza_pos );
  }
}

//アイテムセット すでに持ってる
static void PLIST_MSGCB_ItemSet_CheckChangeItem( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PLIST_MSGCB_ItemSet_CheckChangeItemCB );
}
static void PLIST_MSGCB_ItemSet_CheckChangeItemCB( PLIST_WORK *work , const int retVal )
{

  if( retVal == PMIT_YES )
  {
    //はいアイテム入れ替え
    const u32 haveItemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
    if( PLIST_CheckBagItemNum( work , haveItemNo ) == ITEM_MAX_NORMAL )
    {
      //いいえ
      if( work->plData->mode == PL_MODE_ITEMSET_RET )
      {
        //リストから開始なので戻る
        //ついでにモードをフィールドに戻してしまう
        work->plData->mode = PL_MODE_FIELD;
        PLIST_MSG_CloseWindow( work , work->msgWork );
        PLIST_MessageWaitInit( work , mes_pokelist_04_31 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
      }
      else
      {
        //アイテムから来たので終了
        work->mainSeq = PSMS_FADEOUT;
        PLIST_MSG_CloseWindow( work , work->msgWork );
        PLIST_MessageWaitInit( work , mes_pokelist_04_31 , TRUE , PLIST_MSGCB_ExitCommon );
      }
    }
    else
    {
      if( ITEM_CheckMail( work->plData->item ) == TRUE )
      {
        //メール処理
        work->plData->ret_sel = work->pokeCursor;
        work->mainSeq = PSMS_FADEOUT;
        work->plData->ret_mode = PL_RET_MAILSET;
      }
      else
      {
        BOOL isGirathinaToOrigin;
        BOOL isGirathinaToAnother;
        PLIST_MSG_CloseWindow( work , work->msgWork );

        //フォルムチェンジ前にReDrawParamが必要
        PLIST_SetPokeItem( work , work->selectPokePara , work->plData->item );
        PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );

        PLIST_ITEM_CangeAruseusuForm( work , work->selectPokePara , work->plData->item );
        PLIST_ITEM_CangeInsekutaForm( work , work->selectPokePara , work->plData->item );

        isGirathinaToOrigin = PLIST_DEMO_CheckGirathnaToOrigin( work , work->selectPokePara );
        isGirathinaToAnother = PLIST_DEMO_CheckGirathnaToAnother( work , work->selectPokePara );

        PLIST_MSG_CreateWordSet( work , work->msgWork );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 1 , haveItemNo );
        PLIST_MSG_AddWordSet_ItemName( work , work->msgWork , 2 , work->plData->item );
        if( work->plData->mode == PL_MODE_ITEMSET_RET )
        {
          //リストから開始なので戻る
          //ついでにモードをフィールドに戻してしまう
          work->plData->mode = PL_MODE_FIELD;
          if( isGirathinaToOrigin == TRUE )
          {
            //ギラティナ・フォルムチェンジ(アナザー→オリジン
            PLIST_DEMO_ChangeGirathinaToOrigin( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ORIGIN;
          }
          else
          if( isGirathinaToAnother == TRUE )
          {
            //ギラティナ・フォルムチェンジ(オリジン→アナザー
            PLIST_DEMO_ChangeGirathinaToAnother( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ANOTHER;
          }
          else
          {
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
          }
        }
        else
        {
          //アイテムから来たので終了
          work->plData->ret_mode = PL_RET_BAG;
          if( isGirathinaToOrigin == TRUE )
          {
            //ギラティナ・フォルムチェンジ(アナザー→オリジン
            PLIST_DEMO_ChangeGirathinaToOrigin( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ORIGIN;
          }
          else
          if( isGirathinaToAnother == TRUE )
          {
            //ギラティナ・フォルムチェンジ(オリジン→アナザー
            PLIST_DEMO_ChangeGirathinaToAnother( work , work->selectPokePara);
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_FormChange );
            work->demoType = PDT_GIRATHINA_TO_ANOTHER;
          }
          else
          {
            PLIST_MessageWaitInit( work , mes_pokelist_04_32 , TRUE , PLIST_MSGCB_ExitCommon );
          }
        }
        PLIST_MSG_DeleteWordSet( work , work->msgWork );
      }
    }
  }
  else
  {
    //いいえ
    if( work->plData->mode == PL_MODE_ITEMSET_RET )
    {
      //リストから開始なので戻る
      //ついでにモードをフィールドに戻してしまう
      work->plData->mode = PL_MODE_FIELD;
      PLIST_MSG_CloseWindow( work , work->msgWork );
      work->mainSeq = PSMS_SELECT_POKE;
      PLIST_InitMode_Select( work );
    }
    else
    {
      //アイテムから来たので終了
      work->mainSeq = PSMS_FADEOUT;
    }
  }
}

//ミルク飲み・タマゴ産み第１段階
static void PLIST_HPANMCB_SkillRecoverHpFirst( PLIST_WORK *work )
{
  POKEMON_PARAM *usePP = PokeParty_GetMemberPointer(work->plData->pp, work->useTarget );
  const u32 hpNowUse = PP_Get( usePP , ID_PARA_hp , NULL );
  const u32 hpMaxUse = PP_Get( usePP , ID_PARA_hpmax , NULL );

  POKEMON_PARAM *trgPP = PokeParty_GetMemberPointer(work->plData->pp, work->plData->ret_sel );
  const u32 hpNowTrg = PP_Get( work->selectPokePara , ID_PARA_hp , NULL );
  const u32 hpMaxTrg = PP_Get( work->selectPokePara , ID_PARA_hpmax , NULL );

  const u32 recvHp = hpMaxUse/5;

  //入れ替えたのを戻す
  work->pokeCursor = work->plData->ret_sel;
  
  if( hpNowTrg + recvHp > hpMaxTrg )
  {
    PP_Put( trgPP , ID_PARA_hp , hpMaxTrg );
  }
  else
  {
    PP_Put( trgPP , ID_PARA_hp , hpNowTrg + recvHp );
  }

  work->hpAnimeCallBack = PLIST_HPANMCB_SkillRecoverHpSecond;
  work->mainSeq = PSMS_INIT_HPANIME;
  work->reqPlaySe = TRUE;

}
//ミルク飲み・タマゴ産み第２段階
static void PLIST_HPANMCB_SkillRecoverHpSecond( PLIST_WORK *work )
{
  u16 nowHp = PLIST_PLATE_GetDispHp( work , work->plateWork[work->plData->ret_sel] );

  //入れ替えたのを戻す
  work->pokeCursor = work->plData->ret_sel;
  
  PLIST_MSG_CreateWordSet( work , work->msgWork );
  PLIST_MSG_AddWordSet_PokeName( work , work->msgWork , 0 , work->selectPokePara );
  if( work->befHp != 0 )
  {
    PLIST_MSG_AddWordSet_Value( work , work->msgWork , 1 , nowHp-work->befHp , 3 );
    PLIST_MessageWaitInit( work , mes_pokelist_04_14 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
  }
  PLIST_MSG_DeleteWordSet( work , work->msgWork );
}


//メールを預かりますか？
static void PLIST_MSGCB_TakeMail( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PLIST_MSGCB_TakeMailCB );
}

static void PLIST_MSGCB_TakeMailCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //はい→預ける
    const int emptyBoxId = MAIL_SearchNullID( work->plData->mailblock , MAILBLOCK_PASOCOM );
    if( emptyBoxId >= 0 )
    {
      MAIL_DATA *mailData  = MailData_CreateWork(work->heapId);
      PP_Get( work->selectPokePara , ID_PARA_mail_data , mailData );
      MAIL_AddMailFormWork( work->plData->mailblock , MAILBLOCK_PASOCOM , emptyBoxId , mailData );
      GFL_HEAP_FreeMemory( mailData );
      PP_Put( work->selectPokePara , ID_PARA_item , 0 );
      PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      
      PLIST_MessageWaitInit( work , mes_pokelist_04_02 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    }
    else
    {
      //BOX一杯！
      PLIST_MessageWaitInit( work , mes_pokelist_04_04 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    }
  }
  else
  {
    //いいえ→消えるけどOK？
    PLIST_MessageWaitInit( work , mes_pokelist_04_03 , FALSE , PLIST_MSGCB_TakeMail_Confirm );
  }
}

//メール取る：消えるけどOK?
static void PLIST_MSGCB_TakeMail_Confirm( PLIST_WORK *work )
{
  PLIST_YesNoWaitInit( work , PLIST_MSGCB_TakeMail_ConfirmCB );
}

static void PLIST_MSGCB_TakeMail_ConfirmCB( PLIST_WORK *work , const int retVal )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );

  if( retVal == PMIT_YES )
  {
    //はい→消す
    const u32 haveItemNo = PP_Get( work->selectPokePara , ID_PARA_item , NULL );
    if( PLIST_CheckBagItemNum( work , haveItemNo ) == ITEM_MAX_NORMAL )
    {
      PLIST_MessageWaitInit( work , mes_pokelist_04_31 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    }
    else
    {
      PLIST_AddBagItem( work , haveItemNo );
      PP_Put( work->selectPokePara , ID_PARA_item , 0 );
      PLIST_PLATE_ReDrawParam( work , work->plateWork[work->pokeCursor] );
      PLIST_MessageWaitInit( work , mes_pokelist_04_05 , TRUE , PLIST_MSGCB_ReturnSelectCommon );
    }
  }
  else
  {
    //いいえ→戻る
    PLIST_MSG_CloseWindow( work , work->msgWork );
    work->mainSeq = PSMS_SELECT_POKE;
    PLIST_InitMode_Select( work );
  }
}

static void PLIST_MSGCB_FormChange( PLIST_WORK *work )
{
  PLIST_MSG_CloseWindow( work , work->msgWork );
  work->mainSeq = PSMS_FORM_CHANGE_INIT;
}
#pragma mark [>force exit
void PLIST_ForceExit_Timeup( PLIST_WORK *work )
{
  if( work->isCallForceExit == FALSE &&
      work->mainSeq != PSMS_FADEOUT &&
      work->mainSeq != PSMS_FADEOUT_WAIT &&
      work->mainSeq != PSMS_FADEIN &&
      work->mainSeq != PSMS_FADEIN_WAIT )
  {
    if( work->mainSeq == PSMS_MENU )
    {
      PLIST_SelectMenuTerm( work );
    }
    
    work->isCallForceExit = TRUE;
    if( work->isComm == TRUE )
    {
      work->mainSeq = PSMS_FINISH_SYNC_INIT;
    }
    else
    {
      work->mainSeq = PSMS_FADEOUT_FORCE;
    }
    work->plData->ret_sel = PL_SEL_POS_ENTER;
    work->plData->ret_mode = PL_RET_NORMAL;
    

    //強制選択
    if( work->isDecideParty == FALSE )
    {
      u8 i;
      for( i=0;i<PLIST_LIST_MAX;i++ )
      {
        work->plData->in_num[i] = 0;
      }
      {
        REGULATION *reg = (REGULATION*)work->plData->reg;
        const BOOL ret = PokeRegulationCheckPokeParty_Func( reg , work->plData->pp , work->plData->in_num );
        GF_ASSERT_MSG( ret ,"PLIST can't select battle order!!\n" );
        if( ret == FALSE )
        {
          u8 i;
          //止まらないために・・・
          for( i=0;i<PLIST_LIST_MAX;i++ )
          {
            work->plData->in_num[i] = 0;
          }
          for( i=0;i<reg->NUM_LO;i++ )
          {
            work->plData->in_num[i] = i+1;
          }
        }
      }
    }
  }
}

#pragma mark [>outer value
//外の数値をいじる
//新しく技を覚える(空きあり
static void PLIST_LearnSkillEmpty( PLIST_WORK *work , POKEMON_PARAM *pp )
{
  if( work->plData->mode == PL_MODE_ITEMUSE ||
      work->plData->mode == PL_MODE_LVUPWAZASET_RET )
  {
    //不思議なアメ
    PP_SetWaza( pp , work->plData->waza );
  }
  else
  {
    //技マシンならば技ポイント引継　
    PP_SetWazaPPCont( pp , work->plData->waza );
  }

}

//新しく技を覚える(空き無し
static void PLIST_LearnSkillFull( PLIST_WORK *work  , POKEMON_PARAM *pp , u8 pos )
{
  if( work->plData->mode == PL_MODE_ITEMUSE ||
      work->plData->mode == PL_MODE_LVUPWAZASET_RET )
  {
    //不思議なアメ
    PP_SetWazaPos( pp , work->plData->waza , pos );
  }
  else
  {
    //わざマシンならば技ポイント引継
    PP_SetWazaPosPPCont( pp , work->plData->waza , pos );
  }
}

//技マシンを使った処理(技覚える部分以外
static void PLIST_UseWazaMachine( PLIST_WORK *work  , POKEMON_PARAM *pp )
{
  NATSUKI_Calc( pp , CALC_NATSUKI_WAZA_MACHINE , work->plData->zone_id , work->heapId );
}

//アイテムを持たせる処理
//持っている場合はアイテムをリストに戻します。
static void PLIST_SetPokeItem( PLIST_WORK *work , POKEMON_PARAM *pp , u16 itemNo )
{
  const u32 haveNo = PP_Get( pp , ID_PARA_item , NULL );
  if( haveNo != 0 )
  {
    PLIST_AddBagItem( work , haveNo );
  }
  PP_Put( pp , ID_PARA_item , itemNo );
  if( itemNo != 0 )
  {
    PLIST_SubBagItem( work , itemNo );
  }
}
//アイテム個数のチェック
static const u16 PLIST_CheckBagItemNum( PLIST_WORK *work , u16 itemNo )
{
  return MYITEM_GetItemNum( work->plData->myitem , itemNo , work->heapId );
}
//アイテムを消費する
static void PLIST_SubBagItem( PLIST_WORK *work , u16 itemNo )
{
  MYITEM_SubItem( work->plData->myitem , itemNo , 1 , work->heapId );
}
//アイテムを増やす
static void PLIST_AddBagItem( PLIST_WORK *work , u16 itemNo )
{
  MYITEM_AddItem( work->plData->myitem , itemNo , 1 , work->heapId );
}

#pragma mark [>debug
#if USE_DEBUGWIN_SYSTEM

#include "plist_debug_menu.c"

#endif