//======================================================================
/**
 * @file	p_sta_skill.c
 * @brief	ポケモンステータス 技ページ
 * @author	ariizumi
 * @data	09/07/10
 *
 * モジュール名：PSTATUS_SKILL
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "print/global_msg.h"
#include "print/str_tool.h"
#include "app/app_menu_common.h"
#include "waza_tool/wazadata.h"

#include "arc_def.h"
#include "message.naix"
#include "p_status_gra.naix"

#include "p_sta_sys.h"
#include "p_sta_skill.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//TP判定
//この値以上動いたら入れ替え判定とみなす
#define PSTATUS_SKILL_TP_SLIDE_CHECK_Y (8)

//文字位置
#define PSTATUS_SKILL_STR_HP_X (24+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_HP_Y ( 0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_HPNOW_X   ( 88+PSTATUS_STR_OFS_X)  //右寄せ
#define PSTATUS_SKILL_STR_HPNOW_Y   (  0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_HPSLASH_X ( 88+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_HPSLASH_Y (  0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_HPMAX_X   (120+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_HPMAX_Y   ( 0+PSTATUS_STR_OFS_Y)
  //攻撃・防御・特攻t・特防・素早さ
#define PSTATUS_SKILL_STR_PARAM_X ( 0+PSTATUS_STR_OFS_X)  
#define PSTATUS_SKILL_STR_PARAM_Y ( 0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_PARAM_NUM_X (104+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_PARAM_NUM_Y (  0+PSTATUS_STR_OFS_Y)
//特性
#define PSTATUS_SKILL_STR_TOKUSEI_X ( 4+PSTATUS_STR_OFS_X)  
#define PSTATUS_SKILL_STR_TOKUSEI_Y ( 0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_TOKUSEI_NAME_X (64+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_TOKUSEI_NAME_Y ( 0+PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_STR_TOKUSEI_INFO_X ( 4+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_TOKUSEI_INFO_Y (16+PSTATUS_STR_OFS_Y)
//技詳細
#define PSTATUS_SKILL_STR_WAZA_TYPE_X ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_TYPE_Y ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_POWER_X ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_POWER_Y ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_POWER_NUM_X (80+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_POWER_NUM_Y ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_HIT_X ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_HIT_Y ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_HIT_NUM_X (80+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_HIT_NUM_Y ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_INFO_X ( 0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_STR_WAZA_INFO_Y ( 0+PSTATUS_STR_OFS_X)

//セル位置
#define PSTATUS_SKILL_CELL_HPBAR_X ( 159 );
#define PSTATUS_SKILL_CELL_HPBAR_Y (  52 );
#define PSTATUS_SKILL_CELL_WAZATYPE_X ( 172 );
#define PSTATUS_SKILL_CELL_WAZATYPE_Y (  56 );
#define PSTATUS_SKILL_CELL_TYPEICON_1_X ( 168 );
#define PSTATUS_SKILL_CELL_TYPEICON_1_Y ( 120 );
#define PSTATUS_SKILL_CELL_TYPEICON_2_X ( 200 );
#define PSTATUS_SKILL_CELL_TYPEICON_2_Y ( 120 );


//HPバー系
#define PSTATUS_SKILL_HPBAR_LEN (48)
#define PSTATUS_SKILL_HPBAR_TOP  (2)
#define PSTATUS_SKILL_HPBAR_LEFT (0)
//色
#define PSTATUS_SKILL_HPBAR_COL_GREEN_IN  (0x5)
#define PSTATUS_SKILL_HPBAR_COL_GREEN_OUT (0x6)
#define PSTATUS_SKILL_HPBAR_COL_YELLOW_IN  (0x7)
#define PSTATUS_SKILL_HPBAR_COL_YELLOW_OUT (0x8)
#define PSTATUS_SKILL_HPBAR_COL_RED_IN  (0x9)
#define PSTATUS_SKILL_HPBAR_COL_RED_OUT (0xa)

//SkillPlate
#define PSTATUS_SKILL_PLATE_NUM (5)
#define PSTATUS_SKILL_PLATE_WIDTH (17)
#define PSTATUS_SKILL_PLATE_HEIGHT (4)
//基本位置からのbmpWin位置
#define PSTATUS_SKILL_PLATE_WIN_TOP (0)
#define PSTATUS_SKILL_PLATE_WIN_LEFT (5)
#define PSTATUS_SKILL_PLATE_WIN_HEIGHT (4)
#define PSTATUS_SKILL_PLATE_WIN_WIDTH (11)
//タイプアイコン
#define PSTATUS_SKILL_PLATE_TYPE_X (24)
#define PSTATUS_SKILL_PLATE_TYPE_Y (8)

//文字位置(プレート
#define PSTATUS_SKILL_PLATE_WAZANAME_X (0+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_WAZANAME_Y (0 +PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_PLATE_PPSTR_X (12+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_PPSTR_Y (16 +PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_PLATE_NOWPP_X (56+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_NOWPP_Y (16 +PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_PLATE_SLASH_X (56+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_SLASH_Y (16 +PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_PLATE_MAXPP_X (60+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_MAXPP_Y (16 +PSTATUS_STR_OFS_Y)
#define PSTATUS_SKILL_PLATE_NONE_X (48+PSTATUS_STR_OFS_X)
#define PSTATUS_SKILL_PLATE_NONE_Y (16 +PSTATUS_STR_OFS_Y)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
enum PSTATUS_SKILL_UP_BMPWIN_TYPE
{
  PSBT_HP,
  PSBT_HP_GAUGE,
  PSBT_ATK,
  PSBT_DEF,
  PSBT_SATK,
  PSBT_SDEF,
  PSBT_SPD,
  PSBT_TOKUSEI,
  
  PSBT_TYPE,
  PSBT_POW,
  PSBT_HIT,
  PSBT_INFO,
  
  PSBT_MAX,
  
  //ステータスの開始と終わり
  PSBT_STATUS_START = PSBT_HP,
  PSBT_STATUS_END = PSBT_TOKUSEI,
  //技詳細の開始と終わり
  PSBT_SKILL_START = PSBT_TYPE,
  PSBT_SKILL_END = PSBT_INFO,
};

enum PSTATUS_SKILL_CELL_CURSOR
{
  PSCC_NORMAL,
  PSCC_CHANGE,
  PSCC_ARROW_BOTH,  //入れ替えを促すカーソル
  PSCC_ARROW_DOWN,
  PSCC_ARROW_UP,
};

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

typedef struct
{
  u8   idx;
  BOOL isUpdateStr;
  
  GFL_CLWK   *clwkPlate;
  GFL_CLWK   *clwkType;
  GFL_BMPWIN *bmpWin;
  
}PSTATUS_SKILL_PLATE;


struct _PSTATUS_SKILL_WORK
{
  BOOL isDisp;
  BOOL isUpdateStrStatus;
  BOOL isUpdateStrSkill;
  BOOL isWaitSwapSkill;
  BOOL isChangeMode;
  BOOL isHoldTp;
  
  u8   cursorPos;
  u8   changeTarget;
  u32  holdTpx;
  u32  holdTpy;

  NNSG2dScreenData *scrDataDown;
  void *scrResDown;
  NNSG2dScreenData *scrDataUpStatus;
  void *scrResUpStatus;
  NNSG2dScreenData *scrDataUpSkill;
  void *scrResUpSkill;
  
  GFL_BMPWIN *upBmpWin[PSBT_MAX];
  
  GFL_CLWK *clwkHpBar;
  GFL_CLWK *clwkWazaKind;
  GFL_CLWK *clwkCur;
  GFL_CLWK *clwkTargetCur;
  GFL_CLWK *clwkArrow;
  
  PSTATUS_SKILL_PLATE plateWork[PSTATUS_SKILL_PLATE_NUM];
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PSTATUS_SKILL_DispStatusPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DispSkillInfoPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DispStatusPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DispSkillInfoPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DrawStrStatus( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DrawHPBar( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_DrawStrSkill( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );

static void PSTATUS_SKILL_UpdateUI( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static const BOOL PSTATUS_SKILL_UpdateKey( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_UpdateTP( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork );
static void PSTATUS_SKILL_UpdateCursorPos( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , const u8 newPos );
static void PSTATUS_SKILL_SwapSkill( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , const u8 pos1 , const u8 pos2 );

static void PSTATUS_SKILL_InitPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork , const u8 idx );
static void PSTATUS_SKILL_TermPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_UpdatePlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_DispPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_DispPlate_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_ClearPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_ClearPlate_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork );
static void PSTATUS_SKILL_GetTPRect( PSTATUS_SKILL_PLATE *plateWork , GFL_UI_TP_HITTBL *hitTbl );
static void PSTATUS_SKILL_GetCursorPos( PSTATUS_SKILL_PLATE *plateWork , GFL_CLACTPOS *pos );
static void PSTATUS_SKILL_ChangeColor( PSTATUS_SKILL_PLATE *plateWork , const u8 colType );

//上画面bmpwin座標
static const u8 winPos[PSBT_MAX][4] =
{
  {  8,  4, 16,  2},  //HP
  { 17,  6,  6,  1},  //HPゲージ
  {  8,  7, 14,  2},  //攻撃
  {  8,  9, 14,  2},  //防御
  {  8, 11, 14,  2},  //特攻
  {  8, 13, 14,  2},  //特防
  {  8, 15, 14,  2},  //素早さ
  {  6, 18, 20,  6},  //特性説明
  
  { 10,  6, 14,  2},  //分類
  { 10,  8, 14,  2},  //威力
  { 10, 10, 14,  2},  //命中
  {  1, 13, 30,  6},  //技説明
};

//下画面技プレート座標
static const u8 platePos[PSTATUS_SKILL_PLATE_NUM][2] =
{
  { 1 , 2 },
  { 1 , 6 },
  { 1 ,10 },
  { 1 ,14 },
  { 1 ,21 },
};

//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
PSTATUS_SKILL_WORK* PSTATUS_SKILL_Init( PSTATUS_WORK *work )
{
  u8 i;
  PSTATUS_SKILL_WORK* skillWork;
  
  skillWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_SKILL_WORK) );
  skillWork->isDisp = FALSE;
  skillWork->isUpdateStrStatus = FALSE;
  skillWork->isUpdateStrSkill = FALSE;
  skillWork->isWaitSwapSkill = FALSE;
  skillWork->isChangeMode = FALSE;
  skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
  
  for( i=0;i<PSTATUS_SKILL_PLATE_NUM;i++ )
  {
    skillWork->plateWork[i].idx = 0xFF;
    skillWork->plateWork[i].isUpdateStr = FALSE;
  }
  return skillWork;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
void PSTATUS_SKILL_Term( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  GFL_HEAP_FreeMemory( skillWork );
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
void PSTATUS_SKILL_Main( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  if( skillWork->isUpdateStrStatus == FALSE &&
      skillWork->isUpdateStrSkill == FALSE &&
      skillWork->isWaitSwapSkill == FALSE )
  {
    PSTATUS_SKILL_UpdateUI( work , skillWork );
  }

  //ステータス部分の文字更新
  if( skillWork->isUpdateStrStatus == TRUE )
  {
    u8 i;
    BOOL isFinish = TRUE;
    for( i=PSBT_STATUS_START;i<=PSBT_STATUS_END;i++ )
    {
      if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( skillWork->upBmpWin[i] )) == TRUE )
      {
        isFinish = FALSE;
        break;
      }
    }
    if( isFinish == TRUE )
    {
      for( i=PSBT_STATUS_START;i<=PSBT_STATUS_END;i++ )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( skillWork->upBmpWin[i] );
      }
      PSTATUS_SKILL_DispStatusPage_Trans( work , skillWork );
      skillWork->isUpdateStrStatus = FALSE;
    }
  }
  //技説明部分の文字更新
  if( skillWork->isUpdateStrSkill == TRUE )
  {
    u8 i;
    BOOL isFinish = TRUE;
    for( i=PSBT_SKILL_START;i<=PSBT_SKILL_END;i++ )
    {
      if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( skillWork->upBmpWin[i] )) == TRUE )
      {
        isFinish = FALSE;
        break;
      }
    }
    if( isFinish == TRUE )
    {
      for( i=PSBT_SKILL_START;i<=PSBT_SKILL_END;i++ )
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( skillWork->upBmpWin[i] );
      }
      PSTATUS_SKILL_DispSkillInfoPage_Trans( work , skillWork );
      skillWork->isUpdateStrSkill = FALSE;
    }
  }
  //入れ替えの更新
  if( skillWork->isWaitSwapSkill == TRUE )
  {
    if( PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE )
    {
      skillWork->isChangeMode = FALSE;

      PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 0 );
      skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
      PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );

      GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );
      skillWork->isWaitSwapSkill = FALSE;
    }
  }

  //技プレートの更新
  for( i=0;i<PSTATUS_SKILL_PLATE_NUM;i++ )
  {
    PSTATUS_SKILL_UpdatePlate( work , skillWork , &skillWork->plateWork[i] );
  }
}

#pragma mark [>Resource
//--------------------------------------------------------------
//	リソース読み込み
//--------------------------------------------------------------
void PSTATUS_SKILL_LoadResource( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , ARCHANDLE *archandle )
{
  u8 i;
  //書き換え用スクリーン読み込み
  skillWork->scrResDown = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_info_d_NSCR ,
                    FALSE , &skillWork->scrDataDown , work->heapId );
  skillWork->scrResUpStatus = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_info_u_NSCR ,
                    FALSE , &skillWork->scrDataUpStatus , work->heapId );
  skillWork->scrResUpSkill = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_info_u_NSCR ,
                    FALSE , &skillWork->scrDataUpSkill , work->heapId );

}

//--------------------------------------------------------------
//	リソース開放
//--------------------------------------------------------------
void PSTATUS_SKILL_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  //ここで描画物の開放もしておく
  if( skillWork->isDisp == TRUE )
  {
    PSTATUS_SKILL_ClearPage( work , skillWork );
  }

  GFL_HEAP_FreeMemory( skillWork->scrResDown );
  GFL_HEAP_FreeMemory( skillWork->scrResUpStatus );
  GFL_HEAP_FreeMemory( skillWork->scrResUpSkill );

}

#pragma mark [>Cell
void PSTATUS_SKILL_InitCell( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PSTATUS_SKILL_CELL_HPBAR_X;
    cellInitData.pos_y = PSTATUS_SKILL_CELL_HPBAR_Y;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    cellInitData.anmseq = 0;
    
    skillWork->clwkHpBar = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_HPBASE],
              work->cellRes[SCR_PLT_HPBASE],
              work->cellRes[SCR_ANM_HPBASE],
              &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkHpBar , FALSE );
    
    cellInitData.pos_x = PSTATUS_SKILL_CELL_WAZATYPE_X;
    cellInitData.pos_y = PSTATUS_SKILL_CELL_WAZATYPE_Y;
    skillWork->clwkWazaKind = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_SKILL_TYPE_HENKA],
              work->cellRes[SCR_PLT_SUB_POKE_TYPE],
              work->cellRes[SCR_ANM_POKE_TYPE],
              &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkWazaKind , FALSE );
    
    skillWork->clwkCur = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_SKILL_CUR],
              work->cellRes[SCR_PLT_CURSOR_COMMON],
              work->cellRes[SCR_ANM_SKILL_CUR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkCur , FALSE );

    cellInitData.anmseq = 1;
    cellInitData.softpri = 8;
    skillWork->clwkTargetCur = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_SKILL_CUR],
              work->cellRes[SCR_PLT_CURSOR_COMMON],
              work->cellRes[SCR_ANM_SKILL_CUR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );

    cellInitData.anmseq = PSCC_ARROW_BOTH;
    cellInitData.softpri = 6;
    skillWork->clwkArrow = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_SKILL_CUR],
              work->cellRes[SCR_PLT_CURSOR_COMMON],
              work->cellRes[SCR_ANM_SKILL_CUR],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkArrow , FALSE );
    GFL_CLACT_WK_SetAutoAnmFlag( skillWork->clwkArrow , TRUE );
  } 

  //基本はcellの初期かなのでここで
  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_InitPlate( work , skillWork , &skillWork->plateWork[i] , i );
  }

}

void PSTATUS_SKILL_TermCell( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_TermPlate( work , skillWork , &skillWork->plateWork[i] );
  }
  GFL_CLACT_WK_Remove( skillWork->clwkHpBar );
  GFL_CLACT_WK_Remove( skillWork->clwkWazaKind );
  GFL_CLACT_WK_Remove( skillWork->clwkCur );
  GFL_CLACT_WK_Remove( skillWork->clwkTargetCur );
  GFL_CLACT_WK_Remove( skillWork->clwkArrow );
}


#pragma mark [>Disp
//--------------------------------------------------------------
//	ページの表示
//--------------------------------------------------------------
void PSTATUS_SKILL_DispPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
   
  for( i=0;i<PSBT_MAX;i++ )
  {
    skillWork->upBmpWin[i] = GFL_BMPWIN_Create( PSTATUS_BG_SUB_STR ,
                winPos[i][0] , winPos[i][1] , winPos[i][2] , winPos[i][3] ,
                PSTATUS_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  }
  //HPバーだけパレット違うの対応
  GFL_BMPWIN_SetPalette( skillWork->upBmpWin[PSBT_HP_GAUGE] , PSTATUS_BG_SUB_PLT_HPBAR );

  PSTATUS_SKILL_DispStatusPage( work , skillWork );

  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_DispPlate( work , skillWork , &skillWork->plateWork[i] );
  }

  skillWork->isDisp = TRUE;
}
//--------------------------------------------------------------
//	ページの表示(転送タイミング
//--------------------------------------------------------------
void PSTATUS_SKILL_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;

   //Window下地の張替え
  GFL_BG_WriteScreenExpand( PSTATUS_BG_PLATE , 
                    0 , 0 , PSTATUS_MAIN_PAGE_WIDTH , 24 ,
                    skillWork->scrDataDown->rawData ,
                    0 , 0 , 32 , 32 );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_PLATE );

  PSTATUS_SKILL_DispStatusPage_Trans( work , skillWork );

  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_DispPlate_Trans( work , skillWork , &skillWork->plateWork[i] );
  }

}

//--------------------------------------------------------------
//	ページのクリア
//--------------------------------------------------------------
void PSTATUS_SKILL_ClearPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_ClearPlate( work , skillWork , &skillWork->plateWork[i] );
  }
  for( i=0;i<PSBT_MAX;i++ )
  {
    GFL_BMPWIN_Delete( skillWork->upBmpWin[i] );
  }
  
  skillWork->isDisp = FALSE;
}

//--------------------------------------------------------------
//	ページのクリア(転送タイミング
//--------------------------------------------------------------
void PSTATUS_SKILL_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  
  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_ClearPlate_Trans( work , skillWork , &skillWork->plateWork[i] );
  }
  GFL_BG_FillScreen( PSTATUS_BG_PARAM , 0 , 0 , 0 , 
                     PSTATUS_MAIN_PAGE_WIDTH , 21 ,
                     GFL_BG_SCRWRT_PALNL );
  GFL_BG_LoadScreenReq( PSTATUS_BG_PARAM );
  
  GFL_BG_ClearScreenCodeVReq( PSTATUS_BG_SUB_STR , 0 );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_STR );

  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkHpBar , FALSE );
  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkWazaKind , FALSE );
  
  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );

}

//--------------------------------------------------------------
//	ページの表示(ステータス
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispStatusPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  PSTATUS_SKILL_DrawStrStatus( work , skillWork );
  PSTATUS_SKILL_DrawHPBar(  work , skillWork );
}
//--------------------------------------------------------------
//	ページの表示(ステータス
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispStatusPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;
  GFL_BG_ClearScreenCodeVReq( PSTATUS_BG_SUB_STR , 0 );
  for( i=PSBT_STATUS_START;i<=PSBT_STATUS_END;i++ )
  {
    GFL_BMPWIN_MakeTransWindow_VBlank( skillWork->upBmpWin[i] );
  }
  GFL_BG_LoadScreen( PSTATUS_BG_SUB_PLATE, 
                     skillWork->scrDataUpStatus->rawData, 
                     skillWork->scrDataUpStatus->szByte, 
                     0 );
  GFL_BG_LoadScreenBuffer( PSTATUS_BG_SUB_PLATE, 
                     skillWork->scrDataUpStatus->rawData, 
                     skillWork->scrDataUpStatus->szByte );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_PLATE );

  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkHpBar , TRUE );
  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkWazaKind , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );

}

//--------------------------------------------------------------
//	ページの表示(技詳細
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispSkillInfoPage( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  PSTATUS_SKILL_DrawStrSkill( work , skillWork );
}

//--------------------------------------------------------------
//	ページの表示(技詳細
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispSkillInfoPage_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u8 i;

  GFL_BG_ClearScreenCodeVReq( PSTATUS_BG_SUB_STR , 0 );
  for( i=PSBT_SKILL_START;i<=PSBT_SKILL_END;i++ )
  {
    GFL_BMPWIN_MakeTransWindow_VBlank( skillWork->upBmpWin[i] );
  }
  GFL_BG_LoadScreen( PSTATUS_BG_SUB_PLATE, 
                     skillWork->scrDataUpSkill->rawData, 
                     skillWork->scrDataUpSkill->szByte, 
                     0 );
  GFL_BG_LoadScreenBuffer( PSTATUS_BG_SUB_PLATE, 
                     skillWork->scrDataUpSkill->rawData, 
                     skillWork->scrDataUpSkill->szByte );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_PLATE );

  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkHpBar , FALSE );
  GFL_CLACT_WK_SetDrawEnable( skillWork->clwkWazaKind , TRUE );

  for( i=0;i<4;i++ )
  {
    PSTATUS_SKILL_DispPlate_Trans( work , skillWork , &skillWork->plateWork[i] );
  }

  //タイプアイコン
  {
    const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
    const u32 type1 = PPP_Get( ppp, ID_PARA_type1, NULL );
    const u32 type2 = PPP_Get( ppp, ID_PARA_type2, NULL );
    GFL_CLACTPOS cellPos;
    {
      NNSG2dImageProxy imageProxy;
      GFL_CLGRP_CGR_GetProxy( work->cellResTypeNcg[type1] , &imageProxy );
      GFL_CLACT_WK_SetImgProxy( work->clwkTypeIcon[0] , &imageProxy );
      GFL_CLACT_WK_SetPlttOffs( work->clwkTypeIcon[0] , 
                                APP_COMMON_GetPokeTypePltOffset(type1) , 
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      cellPos.x = PSTATUS_SKILL_CELL_TYPEICON_1_X;
      cellPos.y = PSTATUS_SKILL_CELL_TYPEICON_1_Y;
      GFL_CLACT_WK_SetPos( work->clwkTypeIcon[0] , &cellPos , CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , TRUE );
      GFL_CLACT_WK_SetBgPri( work->clwkTypeIcon[0] , 0 );
    }
    
    if( type1 != type2 )
    {
      NNSG2dImageProxy imageProxy;
      GFL_CLGRP_CGR_GetProxy( work->cellResTypeNcg[type2] , &imageProxy );
      GFL_CLACT_WK_SetImgProxy( work->clwkTypeIcon[1] , &imageProxy );
      GFL_CLACT_WK_SetPlttOffs( work->clwkTypeIcon[1] , 
                                APP_COMMON_GetPokeTypePltOffset(type2) , 
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      cellPos.x = PSTATUS_SKILL_CELL_TYPEICON_2_X;
      cellPos.y = PSTATUS_SKILL_CELL_TYPEICON_2_Y;
      GFL_CLACT_WK_SetPos( work->clwkTypeIcon[1] , &cellPos , CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , TRUE );
      GFL_CLACT_WK_SetBgPri( work->clwkTypeIcon[1] , 0 );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );
    }
  }
}


//--------------------------------------------------------------
//	文字の描画(ステータス
//--------------------------------------------------------------
static void PSTATUS_SKILL_DrawStrStatus( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  const POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );

  //HP
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_HP] , mes_status_04_02 ,
                            PSTATUS_SKILL_STR_HP_X , PSTATUS_SKILL_STR_HP_Y , PSTATUS_STR_COL_TITLE );
  //現在HP
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_hp , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_HP] , wordSet , mes_status_04_11 , 
                                   PSTATUS_SKILL_STR_HPNOW_X , PSTATUS_SKILL_STR_HPNOW_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }
  //HPのスラッシュ
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_HP] , mes_status_04_09 ,
                            PSTATUS_SKILL_STR_HPSLASH_X , PSTATUS_SKILL_STR_HPSLASH_Y , PSTATUS_STR_COL_VALUE );
  //最大HP
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_hpmax , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_HP] , wordSet , mes_status_04_10 , 
                                   PSTATUS_SKILL_STR_HPMAX_X , PSTATUS_SKILL_STR_HPMAX_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //攻撃
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_ATK] , mes_status_04_03 ,
                            PSTATUS_SKILL_STR_PARAM_X , PSTATUS_SKILL_STR_PARAM_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_pow , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_ATK] , wordSet , mes_status_04_12 , 
                                   PSTATUS_SKILL_STR_PARAM_NUM_X , PSTATUS_SKILL_STR_PARAM_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //防御
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_DEF] , mes_status_04_04 ,
                            PSTATUS_SKILL_STR_PARAM_X , PSTATUS_SKILL_STR_PARAM_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_def , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_DEF] , wordSet , mes_status_04_13 , 
                                   PSTATUS_SKILL_STR_PARAM_NUM_X , PSTATUS_SKILL_STR_PARAM_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //特攻
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_SATK] , mes_status_04_05 ,
                            PSTATUS_SKILL_STR_PARAM_X , PSTATUS_SKILL_STR_PARAM_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_spepow , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_SATK] , wordSet , mes_status_04_14 , 
                                   PSTATUS_SKILL_STR_PARAM_NUM_X , PSTATUS_SKILL_STR_PARAM_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //特防
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_SDEF] , mes_status_04_06 ,
                            PSTATUS_SKILL_STR_PARAM_X , PSTATUS_SKILL_STR_PARAM_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_spedef , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_SDEF] , wordSet , mes_status_04_15 , 
                                   PSTATUS_SKILL_STR_PARAM_NUM_X , PSTATUS_SKILL_STR_PARAM_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //素早さ
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_SPD] , mes_status_04_07 ,
                            PSTATUS_SKILL_STR_PARAM_X , PSTATUS_SKILL_STR_PARAM_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = PP_Get( pp , ID_PARA_agi , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    PSTATUS_UTIL_DrawValueStrFuncRight( work , skillWork->upBmpWin[PSBT_SPD] , wordSet , mes_status_04_16 , 
                                   PSTATUS_SKILL_STR_PARAM_NUM_X , PSTATUS_SKILL_STR_PARAM_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }

  //特性
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_TOKUSEI] , mes_status_04_08 ,
                            PSTATUS_SKILL_STR_TOKUSEI_X , PSTATUS_SKILL_STR_TOKUSEI_Y , PSTATUS_STR_COL_TITLE );
  {
    u32 no = PP_Get( pp , ID_PARA_speabino , NULL );
    {
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_tokuseiinfo_dat , work->heapId );
      STRBUF *srcStr;

      srcStr = GFL_MSG_CreateString( msgHandle , no ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( skillWork->upBmpWin[PSBT_TOKUSEI] ) , 
              PSTATUS_SKILL_STR_TOKUSEI_INFO_X , PSTATUS_SKILL_STR_TOKUSEI_INFO_Y , srcStr , 
              work->fontHandle , PSTATUS_STR_COL_VALUE );

      GFL_STR_DeleteBuffer( srcStr );
      GFL_MSG_Delete( msgHandle );
    }
    {
      GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_tokusei_dat , work->heapId );
      STRBUF *srcStr;

      srcStr = GFL_MSG_CreateString( msgHandle , no ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( skillWork->upBmpWin[PSBT_TOKUSEI] ) , 
              PSTATUS_SKILL_STR_TOKUSEI_NAME_X , PSTATUS_SKILL_STR_TOKUSEI_NAME_Y , srcStr , 
              work->fontHandle , PSTATUS_STR_COL_VALUE );

      GFL_STR_DeleteBuffer( srcStr );
      GFL_MSG_Delete( msgHandle );
    }
  }
  
  skillWork->isUpdateStrStatus = TRUE;
}

//--------------------------------------------------------------
//	HPバー表示
//--------------------------------------------------------------
static void PSTATUS_SKILL_DrawHPBar( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  const POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
  const u8 hpMax = PP_Get( pp , ID_PARA_hpmax , NULL );
  const u8 hp = PP_Get( pp , ID_PARA_hp , NULL );

  const u8 rate = 100*hp/hpMax;
  u8 len = PSTATUS_SKILL_HPBAR_LEN*hp/hpMax;
  u8 inCol,outCol;
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( skillWork->upBmpWin[PSBT_HP_GAUGE] );

  if( hp != 0 && len == 0 )
  {
    len = 1;
  }
  if( hp != hpMax && len == PSTATUS_SKILL_HPBAR_LEN )
  {
    len = PSTATUS_SKILL_HPBAR_LEN-1;
  }
  
  //色決定
  if( rate <= 25 )
  {
    inCol  = PSTATUS_SKILL_HPBAR_COL_RED_IN;
    outCol = PSTATUS_SKILL_HPBAR_COL_RED_OUT;
  }
  else
  if( rate <= 50 )
  {
    inCol  = PSTATUS_SKILL_HPBAR_COL_YELLOW_IN;
    outCol = PSTATUS_SKILL_HPBAR_COL_YELLOW_OUT;
  }
  else
  {
    inCol  = PSTATUS_SKILL_HPBAR_COL_GREEN_IN;
    outCol = PSTATUS_SKILL_HPBAR_COL_GREEN_OUT;
  }
  
  GFL_BMP_Fill( bmp , PSTATUS_SKILL_HPBAR_LEFT , PSTATUS_SKILL_HPBAR_TOP ,
                len , 1 , outCol );
  GFL_BMP_Fill( bmp , PSTATUS_SKILL_HPBAR_LEFT , PSTATUS_SKILL_HPBAR_TOP+1 ,
                len , 2 , inCol );
  GFL_BMP_Fill( bmp , PSTATUS_SKILL_HPBAR_LEFT , PSTATUS_SKILL_HPBAR_TOP+3 ,
                len , 1 , outCol );

}

//--------------------------------------------------------------
//	文字の描画(技
//--------------------------------------------------------------
static void PSTATUS_SKILL_DrawStrSkill( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  u32 wazaNo;
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  u8 i;
  
  //ここはClear→Dispの流れを通らず描画されることがあるので
  for( i=PSBT_SKILL_START;i<=PSBT_SKILL_END;i++ )
  {
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(skillWork->upBmpWin[i]) , 0 );
  }

  if( skillWork->cursorPos < PSTATUS_SKILL_PLATE_NUM-1 )
  {
    //手持ち技
    wazaNo = PPP_Get( ppp , ID_PARA_waza1+skillWork->cursorPos , NULL );
  }
  else
  {
    //取得技
    wazaNo = work->psData->waza;
  }
  
  //技分類
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_TYPE] , mes_status_06_22 ,
                            PSTATUS_SKILL_STR_WAZA_TYPE_X , PSTATUS_SKILL_STR_WAZA_TYPE_Y , PSTATUS_STR_COL_TITLE );
  {
    NNSG2dImageProxy imageProxy;
    const WazaDamageType type = WAZADATA_GetDamageType( wazaNo );
    GFL_CLGRP_CGR_GetProxy( work->cellRes[SCR_NCG_SKILL_TYPE_HENKA+type] , &imageProxy );
    GFL_CLACT_WK_SetImgProxy( skillWork->clwkWazaKind , &imageProxy );
    GFL_CLACT_WK_SetPlttOffs( skillWork->clwkWazaKind , 
                              APP_COMMON_GetWazaKindPltOffset(type) , 
                              CLWK_PLTTOFFS_MODE_PLTT_TOP );
  }
  
  //技威力
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_POW] , mes_status_06_20 ,
                            PSTATUS_SKILL_STR_WAZA_POWER_X , PSTATUS_SKILL_STR_WAZA_POWER_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = WAZADATA_GetPower( wazaNo );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_LEFT , STR_NUM_DISP_SPACE );
    PSTATUS_UTIL_DrawValueStrFunc( work , skillWork->upBmpWin[PSBT_POW] , wordSet , mes_status_06_23 , 
                                   PSTATUS_SKILL_STR_WAZA_POWER_NUM_X , PSTATUS_SKILL_STR_WAZA_POWER_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }
  
  //技命中
  PSTATUS_UTIL_DrawStrFunc( work , skillWork->upBmpWin[PSBT_HIT] , mes_status_06_21 ,
                            PSTATUS_SKILL_STR_WAZA_HIT_X , PSTATUS_SKILL_STR_WAZA_HIT_Y , PSTATUS_STR_COL_TITLE );
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 no = WAZADATA_GetHitRatio( wazaNo );
    WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_LEFT , STR_NUM_DISP_SPACE );
    PSTATUS_UTIL_DrawValueStrFunc( work , skillWork->upBmpWin[PSBT_HIT] , wordSet , mes_status_06_24 , 
                                   PSTATUS_SKILL_STR_WAZA_HIT_NUM_X , PSTATUS_SKILL_STR_WAZA_HIT_NUM_Y , PSTATUS_STR_COL_VALUE );
    WORDSET_Delete( wordSet );
  }
  //技説明
  {
    GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_wazainfo_dat , work->heapId );
    STRBUF *srcStr;

    srcStr = GFL_MSG_CreateString( msgHandle , wazaNo ); 
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( skillWork->upBmpWin[PSBT_INFO] ) , 
            PSTATUS_SKILL_STR_WAZA_INFO_X , PSTATUS_SKILL_STR_WAZA_INFO_Y , srcStr , 
            work->fontHandle , PSTATUS_STR_COL_VALUE );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_MSG_Delete( msgHandle );
  }

  skillWork->isUpdateStrSkill = TRUE;
}

#pragma mark [>UI
//--------------------------------------------------------------
//	操作系更新
//--------------------------------------------------------------
static void PSTATUS_SKILL_UpdateUI( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  if( PSTATUS_SKILL_UpdateKey( work , skillWork ) == FALSE )
  {
    PSTATUS_SKILL_UpdateTP( work , skillWork );
  }

  if( GFL_UI_TP_GetCont() == FALSE )
  {
    skillWork->isHoldTp = FALSE;
  }
}

//--------------------------------------------------------------
//	キー操作更新
//--------------------------------------------------------------
static const BOOL PSTATUS_SKILL_UpdateKey( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  if( skillWork->isHoldTp == TRUE )
  {
    //タッチ移動中にキーで操作しないで・・・
    return FALSE;
  }
  
  if( work->isActiveBarButton == TRUE )
  {
    if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
    {
      skillWork->cursorPos = 0;
      work->ktst = GFL_APP_END_KEY;
      PSTATUS_SetActiveBarButton( work , FALSE );
      PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
      PSTATUS_SKILL_DispSkillInfoPage( work , skillWork );
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B &&
      skillWork->isChangeMode == FALSE )
  {
    //戻る
    PSTATUS_SetActiveBarButton( work , TRUE );
    PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->cursorPos] , 0 );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkCur , FALSE );
    PSTATUS_SKILL_DispStatusPage( work , skillWork );
    work->ktst = GFL_APP_END_KEY;
    return TRUE;
  }
  else 
  if( work->ktst == GFL_APP_END_TOUCH )
  {
    if( GFL_UI_KEY_GetTrg() )
    {
        work->ktst = GFL_APP_END_KEY;
        skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
        PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
        return TRUE;
    }
  }
  else 
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  {
    if( skillWork->isChangeMode == FALSE )
    {
      //入れ替えモードへ
      GFL_CLACTPOS cellPos;
      skillWork->isChangeMode = TRUE;
      skillWork->changeTarget = skillWork->cursorPos;
      PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->cursorPos] , 2 );

      PSTATUS_SKILL_GetCursorPos( &skillWork->plateWork[skillWork->changeTarget] , &cellPos );
      GFL_CLACT_WK_SetPos( skillWork->clwkTargetCur , &cellPos , CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , TRUE );
    }
    else
    {
      if( skillWork->changeTarget != skillWork->cursorPos )
      {
        //入れ替え確定
        PSTATUS_SKILL_SwapSkill( work , skillWork , skillWork->changeTarget , skillWork->cursorPos );
      }
      else
      {
        skillWork->isChangeMode = FALSE;
        PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
        GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );
      }
      //PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 0 );
      //skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
    }
    return TRUE;
  }
  else 
  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B &&
      skillWork->isChangeMode == TRUE )
  {
    skillWork->isChangeMode = FALSE;
    skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
    PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 0 );
    PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );
    return TRUE;
  }
  else 
  if( GFL_UI_KEY_GetTrg() == PAD_KEY_UP ||
      GFL_UI_KEY_GetTrg() == PAD_KEY_DOWN )
  {
    const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
    GFL_CLACTPOS cellPos;
    int newCursorPos = skillWork->cursorPos;
    int moveVal;
    if( GFL_UI_KEY_GetTrg() == PAD_KEY_UP )
    {
      moveVal = -1;
    }
    else
    {
      moveVal = 1;
    }
    
    do
    {
      if( newCursorPos + moveVal < 0 )
      {
        newCursorPos = 3;
      }
      else if( newCursorPos + moveVal > 3 )
      {
        newCursorPos = 0;
      }
      else
      {
        newCursorPos += moveVal;
      }
    }while( PPP_Get( ppp , ID_PARA_waza1+newCursorPos , NULL ) == 0 );
    
    PSTATUS_SKILL_UpdateCursorPos( work , skillWork , newCursorPos );
    return TRUE;
  }


  return FALSE;
}

//--------------------------------------------------------------
//	TP操作更新
//--------------------------------------------------------------
static void PSTATUS_SKILL_UpdateTP( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork )
{
  if( work->barButtonHit == SBT_RETURN )
  {
    //戻るが押された
    PSTATUS_SetActiveBarButton( work , TRUE );
    PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->cursorPos] , 0 );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkCur , FALSE );
    PSTATUS_SKILL_DispStatusPage( work , skillWork );
    work->ktst = GFL_APP_END_TOUCH;
  }
  else
  {
    //プレートとのタッチ判定
    u8 i;
    //当たり判定の作成
    GFL_UI_TP_HITTBL hitTbl[PSTATUS_SKILL_PLATE_NUM+1];
    for( i=0;i<PSTATUS_SKILL_PLATE_NUM;i++ )
    {
      PSTATUS_SKILL_GetTPRect( &skillWork->plateWork[i] , &hitTbl[i] );
    }
    hitTbl[PSTATUS_SKILL_PLATE_NUM].circle.code = GFL_UI_TP_HIT_END;
    
    if( GFL_UI_TP_GetTrg() == TRUE )
    {
      //タッチの瞬間
      const int ret = GFL_UI_TP_HitTrg( hitTbl );
      if( ret != GFL_UI_TP_HIT_NONE )
      {
        const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
        if( PPP_Get( ppp , ID_PARA_waza1+ret , NULL ) != 0 )
        {
          if( work->isActiveBarButton == TRUE )
          {
            PSTATUS_SetActiveBarButton( work , FALSE );
            PSTATUS_SKILL_DispSkillInfoPage( work , skillWork );
          }
          
          skillWork->isHoldTp = TRUE;
          //キーで入れ替え中に来たらchangeTargetを変えない！
          if( skillWork->isChangeMode == TRUE )
          {
            GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );
          }
          else
          {
            skillWork->changeTarget = ret;
            {
              GFL_CLACTPOS cellPos;
              PSTATUS_SKILL_GetCursorPos( &skillWork->plateWork[ret] , &cellPos );
              GFL_CLACT_WK_SetPos( skillWork->clwkArrow , &cellPos , CLSYS_DEFREND_MAIN );
              GFL_CLACT_WK_SetDrawEnable( skillWork->clwkArrow , TRUE );
              if( ret == 0 )
              {
                GFL_CLACT_WK_SetAnmSeq( skillWork->clwkArrow , PSCC_ARROW_DOWN );
              }
              else
              if( ret == 3 )
              {
                GFL_CLACT_WK_SetAnmSeq( skillWork->clwkArrow , PSCC_ARROW_UP );
              }
              else
              {
                GFL_CLACT_WK_SetAnmSeq( skillWork->clwkArrow , PSCC_ARROW_BOTH );
              }
            }
          }
          work->ktst = GFL_APP_END_TOUCH;
          PSTATUS_SKILL_UpdateCursorPos( work , skillWork , ret );
          skillWork->holdTpx = work->tpx;
          skillWork->holdTpy = work->tpy;
        }
      }
    }
    else
    if( GFL_UI_TP_GetCont() == TRUE )
    {
      if( skillWork->isHoldTp == TRUE )
      {
        const int ret = GFL_UI_TP_HitCont( hitTbl );
        //ドラッグ判定
        if( skillWork->isChangeMode == FALSE )
        {
          if( ret == GFL_UI_TP_HIT_NONE )
          {
            //範囲外キャンセル
            skillWork->isHoldTp = FALSE;
            GFL_CLACT_WK_SetDrawEnable( skillWork->clwkArrow , FALSE );
          }
          else
          if( MATH_ABS( (int)skillWork->holdTpy - (int)work->tpy ) > PSTATUS_SKILL_TP_SLIDE_CHECK_Y )
          {
            PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 2 );
            skillWork->isChangeMode = TRUE;
            GFL_CLACT_WK_SetDrawEnable( skillWork->clwkArrow , FALSE );
          }
        }
        //elseは入れない
        if( skillWork->isChangeMode == TRUE )
        {
          if( ret != GFL_UI_TP_HIT_NONE )
          {
            const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
            if( PPP_Get( ppp , ID_PARA_waza1+ret , NULL ) != 0 )
            {
              PSTATUS_SKILL_UpdateCursorPos( work , skillWork , ret );
            }
          }
          else
          {
            //範囲外キャンセル
            PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 0 );
            skillWork->isChangeMode = FALSE;
            skillWork->isHoldTp = FALSE;
            PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
            skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
          }
        }
      }
    }
    else
    {
      if( skillWork->isHoldTp == TRUE )
      {
        GFL_CLACT_WK_SetDrawEnable( skillWork->clwkArrow , FALSE );
        //離されたとき
        if( skillWork->isChangeMode == TRUE )
        {
          /*
          if( skillWork->changeTarget != skillWork->cursorPos )
          {
            //入れ替え確定
            PSTATUS_SKILL_SwapSkill( work , skillWork , skillWork->changeTarget , skillWork->cursorPos );
          }
          skillWork->isChangeMode = FALSE;
          PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->changeTarget] , 0 );
          //PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->cursorPos] , 0 );
          PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
          //カーソル位置が変わらないので手動で技表示更新
          PSTATUS_SKILL_DispSkillInfoPage( work , skillWork );
          skillWork->changeTarget = PSTATUS_SKILL_PLATE_NUM;
          */
          if( skillWork->changeTarget != skillWork->cursorPos )
          {
            //入れ替え確定
            PSTATUS_SKILL_SwapSkill( work , skillWork , skillWork->changeTarget , skillWork->cursorPos );
          }
          else
          {
            skillWork->isChangeMode = FALSE;
            PSTATUS_SKILL_UpdateCursorPos( work , skillWork , skillWork->cursorPos );
            GFL_CLACT_WK_SetDrawEnable( skillWork->clwkTargetCur , FALSE );
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------
//	カーソルの位置が変わったとき呼ぶ
//--------------------------------------------------------------
static void PSTATUS_SKILL_UpdateCursorPos( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , const u8 newPos )
{
  if(work->ktst == GFL_APP_END_KEY)
  {
    GFL_CLACTPOS cellPos;
    PSTATUS_SKILL_GetCursorPos( &skillWork->plateWork[newPos] , &cellPos );
    GFL_CLACT_WK_SetPos( skillWork->clwkCur , &cellPos , CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkCur , TRUE );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( skillWork->clwkCur , FALSE );
  }
  if( skillWork->cursorPos != skillWork->changeTarget ||
      skillWork->isChangeMode == FALSE )
  {
    PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[skillWork->cursorPos] , 0 );
  }
  if( newPos != skillWork->changeTarget ||
      skillWork->isChangeMode == FALSE )
  {
    PSTATUS_SKILL_ChangeColor( &skillWork->plateWork[newPos] , 1 );
  }
  if( skillWork->cursorPos != newPos )
  {
    skillWork->cursorPos = newPos;
    PSTATUS_SKILL_DispSkillInfoPage( work , skillWork );
  }
}

//--------------------------------------------------------------
//	スキル入れ替え処理
//--------------------------------------------------------------
static void PSTATUS_SKILL_SwapSkill( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , const u8 pos1 , const u8 pos2 )
{
  //パラメータの処理
  {
    POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
    u32 wazano1,wazano2;
    u32 nowPp1,nowPp2;
    u32 ppCount1,ppCount2;

    PSTATUS_UTIL_SetCurrentPPPFast( work , TRUE );
    
    wazano1  = PP_Get( pp , ID_PARA_waza1    +pos1 , NULL );
    nowPp1   = PP_Get( pp , ID_PARA_pp1      +pos1 , NULL );
    ppCount1 = PP_Get( pp , ID_PARA_pp_count1+pos1 , NULL );
    
    wazano2  = PP_Get( pp , ID_PARA_waza1    +pos2 , NULL );
    nowPp2   = PP_Get( pp , ID_PARA_pp1      +pos2 , NULL );
    ppCount2 = PP_Get( pp , ID_PARA_pp_count1+pos2 , NULL );
    
    PP_SetWazaPos( pp , wazano2 , pos1 );
    PP_SetWazaPos( pp , wazano1 , pos2 );

    PP_Put( pp , ID_PARA_pp1      +pos1 , nowPp2 );
    PP_Put( pp , ID_PARA_pp_count1+pos1 , ppCount2 );

    PP_Put( pp , ID_PARA_pp1      +pos2 , nowPp1 );
    PP_Put( pp , ID_PARA_pp_count1+pos2 , ppCount1 );

    PSTATUS_UTIL_SetCurrentPPPFast( work , FALSE );
  }
  
  //表示的な処理
  {
    PSTATUS_SKILL_ClearPlate( work , skillWork , &skillWork->plateWork[pos1] );
    PSTATUS_SKILL_ClearPlate( work , skillWork , &skillWork->plateWork[pos2] );
    PSTATUS_SKILL_DispPlate( work , skillWork , &skillWork->plateWork[pos1] );
    PSTATUS_SKILL_DispPlate( work , skillWork , &skillWork->plateWork[pos2] );

    //カーソル位置が変わらないので手動で技表示更新
    PSTATUS_SKILL_DispSkillInfoPage( work , skillWork );

    skillWork->isWaitSwapSkill = TRUE;
  }
}

#pragma mark [>SkillPlate
//--------------------------------------------------------------
//	スキルプレートの初期化
//--------------------------------------------------------------
static void PSTATUS_SKILL_InitPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork , const u8 idx )
{
  plateWork->idx = idx;
  plateWork->isUpdateStr = FALSE;

  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = platePos[idx][0] * 8;
    cellInitData.pos_y = platePos[idx][1] * 8;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 2;
    cellInitData.anmseq = plateWork->idx;
    
    plateWork->clwkPlate = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_SKILL],
              work->cellRes[SCR_PLT_SKILL],
              work->cellRes[SCR_ANM_SKILL],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetDrawEnable( plateWork->clwkPlate , FALSE );
      //タイプ
    cellInitData.pos_x = platePos[plateWork->idx][0]*8 +PSTATUS_SKILL_PLATE_TYPE_X;
    cellInitData.pos_y = platePos[plateWork->idx][1]*8 +PSTATUS_SKILL_PLATE_TYPE_Y;
    cellInitData.softpri = 12;
    cellInitData.bgpri = 1;
    cellInitData.anmseq = 0;
    plateWork->clwkType = GFL_CLACT_WK_Create( work->cellUnit ,
          work->cellResTypeNcg[0],
          work->cellRes[SCR_PLT_POKE_TYPE],
          work->cellRes[SCR_ANM_POKE_TYPE],
          &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( plateWork->clwkType , FALSE );
  } 
}

//--------------------------------------------------------------
//	スキルプレートの開放
//--------------------------------------------------------------
static void PSTATUS_SKILL_TermPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  GFL_CLACT_WK_Remove( plateWork->clwkPlate );
  GFL_CLACT_WK_Remove( plateWork->clwkType );
  plateWork->idx = 0xFF;
}

//--------------------------------------------------------------
//	スキルプレートの更新
//--------------------------------------------------------------
static void PSTATUS_SKILL_UpdatePlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  /*
  if( plateWork->isUpdateStr == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin )) == FALSE )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( plateWork->bmpWin );
      plateWork->isUpdateStr = FALSE;
    }
  }
  */
}

//--------------------------------------------------------------
//	スキルプレートの表示
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  const POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
  
  const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+plateWork->idx , NULL );
  plateWork->bmpWin = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
                platePos[plateWork->idx][0]+PSTATUS_SKILL_PLATE_WIN_LEFT , platePos[plateWork->idx][1]+PSTATUS_SKILL_PLATE_WIN_TOP ,
                PSTATUS_SKILL_PLATE_WIN_WIDTH , PSTATUS_SKILL_PLATE_WIN_HEIGHT ,
                PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  //文字の表示
  {
    GFL_MSGDATA *msgHandelSkill = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_wazaname_dat , work->heapId );
    {
      //技名
      STRBUF *srcStr = GFL_MSG_CreateString( msgHandelSkill , wazaNo );
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
              PSTATUS_SKILL_PLATE_WAZANAME_X , PSTATUS_SKILL_PLATE_WAZANAME_Y , srcStr , 
              work->fontHandle , PSTATUS_STR_COL_BLACK );

      GFL_STR_DeleteBuffer( srcStr );
      GFL_MSG_Delete( msgHandelSkill );
    }
    if( wazaNo != 0 )
    {
      //現在PP
      PSTATUS_UTIL_DrawStrFunc( work , plateWork->bmpWin , mes_status_06_08 ,
                                PSTATUS_SKILL_PLATE_PPSTR_X , PSTATUS_SKILL_PLATE_PPSTR_Y , PSTATUS_STR_COL_BLACK );
      {
        WORDSET *wordSet = WORDSET_Create( work->heapId );
        const u32 nowpp = PP_Get( pp , ID_PARA_pp1+plateWork->idx , NULL );
        WORDSET_RegisterNumber( wordSet , 0 , nowpp , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
        PSTATUS_UTIL_DrawValueStrFuncRight( work , plateWork->bmpWin , wordSet , mes_status_06_09 , 
                                       PSTATUS_SKILL_PLATE_NOWPP_X , PSTATUS_SKILL_PLATE_NOWPP_Y , PSTATUS_STR_COL_BLACK );
        WORDSET_Delete( wordSet );
      }
      PSTATUS_UTIL_DrawStrFunc( work , plateWork->bmpWin , mes_status_04_09 ,
                                PSTATUS_SKILL_PLATE_SLASH_X , PSTATUS_SKILL_PLATE_SLASH_Y , PSTATUS_STR_COL_BLACK );
      //最大PP
      {
        WORDSET *wordSet = WORDSET_Create( work->heapId );
        const u32 maxpp = PP_Get( pp , ID_PARA_pp_max1+plateWork->idx , NULL );
        WORDSET_RegisterNumber( wordSet , 0 , maxpp , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
        PSTATUS_UTIL_DrawValueStrFunc( work , plateWork->bmpWin , wordSet , mes_status_06_14 , 
                                       PSTATUS_SKILL_PLATE_MAXPP_X , PSTATUS_SKILL_PLATE_MAXPP_Y , PSTATUS_STR_COL_BLACK );
        WORDSET_Delete( wordSet );
      }
    }
    else
    {
      //技無しの線
      PSTATUS_UTIL_DrawStrFunc( work , plateWork->bmpWin , mes_status_06_27 ,
                                PSTATUS_SKILL_PLATE_NONE_X , PSTATUS_SKILL_PLATE_NONE_Y , PSTATUS_STR_COL_BLACK );
    }
  }

  plateWork->isUpdateStr = TRUE;
}

//--------------------------------------------------------------
//	スキルプレートの表示
//--------------------------------------------------------------
static void PSTATUS_SKILL_DispPlate_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  const POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
  
  const u32 wazaNo = PP_Get( pp , ID_PARA_waza1+plateWork->idx , NULL );
  if( plateWork->isUpdateStr == TRUE )
  {
    if( wazaNo != 0 )
    {
      NNSG2dImageProxy imageProxy;
      const PokeType type = WAZADATA_GetType( wazaNo );
      GFL_CLGRP_CGR_GetProxy( work->cellResTypeNcg[type] , &imageProxy );
      GFL_CLACT_WK_SetImgProxy( plateWork->clwkType , &imageProxy );
      GFL_CLACT_WK_SetPlttOffs( plateWork->clwkType , 
                                APP_COMMON_GetPokeTypePltOffset(type) , 
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      
      GFL_CLACT_WK_SetDrawEnable( plateWork->clwkType , TRUE );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( plateWork->clwkType , FALSE );
    }

    GFL_BMPWIN_MakeTransWindow_VBlank( plateWork->bmpWin );
    GFL_CLACT_WK_SetDrawEnable( plateWork->clwkPlate , TRUE );

    plateWork->isUpdateStr = FALSE;
  }
}

//--------------------------------------------------------------
//	スキルプレートの消去
//--------------------------------------------------------------
static void PSTATUS_SKILL_ClearPlate( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  GFL_BMPWIN_Delete( plateWork->bmpWin );
}
//--------------------------------------------------------------
//	スキルプレートの消去
//--------------------------------------------------------------
static void PSTATUS_SKILL_ClearPlate_Trans( PSTATUS_WORK *work , PSTATUS_SKILL_WORK *skillWork , PSTATUS_SKILL_PLATE *plateWork )
{
  GFL_CLACT_WK_SetDrawEnable( plateWork->clwkPlate , FALSE );
  GFL_CLACT_WK_SetDrawEnable( plateWork->clwkType , FALSE );
}

//--------------------------------------------------------------
//	スキルプレートの座標取得(TP判定用
//--------------------------------------------------------------
static void PSTATUS_SKILL_GetTPRect( PSTATUS_SKILL_PLATE *plateWork , GFL_UI_TP_HITTBL *hitTbl )
{
  if( plateWork->idx < PSTATUS_SKILL_PLATE_NUM )
  {
    hitTbl->rect.top = platePos[plateWork->idx][1]*8;
    hitTbl->rect.bottom = hitTbl->rect.top + PSTATUS_SKILL_PLATE_HEIGHT*8;
    hitTbl->rect.left = platePos[plateWork->idx][0]*8;
    hitTbl->rect.right = hitTbl->rect.left + PSTATUS_SKILL_PLATE_WIDTH*8;
  }
  else
  {
    hitTbl->rect.top = 0;
    hitTbl->rect.bottom = 0;
    hitTbl->rect.left = 0;
    hitTbl->rect.right = 0;
  }
}

//--------------------------------------------------------------
//	スキルプレートの座標取得(カーソル位置用
//--------------------------------------------------------------
static void PSTATUS_SKILL_GetCursorPos( PSTATUS_SKILL_PLATE *plateWork , GFL_CLACTPOS *pos )
{
  pos->x = platePos[plateWork->idx][0]*8;
  pos->y = platePos[plateWork->idx][1]*8;
}

//--------------------------------------------------------------
//	スキルプレートの色変更
//--------------------------------------------------------------
static void PSTATUS_SKILL_ChangeColor( PSTATUS_SKILL_PLATE *plateWork , const u8 colType )
{
  GFL_CLACT_WK_SetAnmSeq( plateWork->clwkPlate , plateWork->idx + colType*PSTATUS_SKILL_PLATE_NUM );
}
