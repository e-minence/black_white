//======================================================================
/**
 * @file	p_sta_sub.c
 * @brief	ポケモンステータス 右側情報画面
 * @author	ariizumi
 * @data	09/06/10
 *
 * モジュール名：PSTATUS_SUB
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "print/printsys.h"
#include "print/wordset.h"

#include "arc_def.h"

#include "p_sta_sys.h"
#include "p_sta_sub.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PSTATUS_MCSS_POS_X (FX32_CONST(( 200+16 )/16.0f))
#define PSTATUS_MCSS_POS_Y (FX32_CONST((192.0f-( 120 ))/16.0f))

//座標系
//上用Win
#define PSTATUS_SUB_UPWIN_LEFT (19)
#define PSTATUS_SUB_UPWIN_TOP  ( 0)
#define PSTATUS_SUB_UPWIN_WIDTH  (13)
#define PSTATUS_SUB_UPWIN_HEIGHT ( 4)

//下用Win
#define PSTATUS_SUB_DOWNWIN_LEFT (19)
#define PSTATUS_SUB_DOWNWIN_TOP  (17)
#define PSTATUS_SUB_DOWNWIN_WIDTH  (13)
#define PSTATUS_SUB_DOWNWIN_HEIGHT ( 4)

//文字列共通オフセット
#define PSTATUS_STR_OFS_X (2)
#define PSTATUS_STR_OFS_Y (2)

//上Win用文字座標
#define PSTATUS_SUB_STR_NAME_X (24 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_NAME_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_SEX_X (88 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_SEX_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_LV_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_LV_Y (16 + PSTATUS_STR_OFS_Y)

//下Win用文字座標
#define PSTATUS_SUB_STR_ITEM_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_ITEM_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_ITEMNAME_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_ITEMNAME_Y (16 + PSTATUS_STR_OFS_Y)

//セル系座標
#define PSTATUS_SUB_CELL_BALL_X ( 168 )
#define PSTATUS_SUB_CELL_BALL_Y (   8 )

//ポケのタッチ範囲
#define PSTATUS_SUB_TOUCH_TOP ( 32)
#define PSTATUS_SUB_TOUCH_BOTTOM ( 120)
#define PSTATUS_SUB_TOUCH_LEFT ( 152)
#define PSTATUS_SUB_TOUCH_RIGHT ( 255)


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

enum
{
  SSMT_BOX_DISC,
  SSMT_BOX_TRIANGLE,
  SSMT_BOX_SQUARE,
  SSMT_BOX_HEART,
  SSMT_BOX_STAR,
  SSMT_BOX_DIAMOND,
  SSMT_MARK_RARE,
  SSMT_MARK_POKERUS,
  
  SSMT_MAX,
}PSTAUTS_SUB_MARK_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PSTATUS_SUB_WORK
{
  MCSS_WORK     *pokeMcss;
  MCSS_WORK     *pokeMcssBack;
  
  BOOL        isUpdateStr;
  BOOL        isDispFront;
  
  GFL_BMPWIN  *bmpWinUpper;
  GFL_BMPWIN  *bmpWinDown;
  
  GFL_CLWK    *clwkBall;
  GFL_CLWK    *clwkMark[SSMT_MAX];

};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PSTATUS_SUB_DrawStr( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp );

static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp );
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );

//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
PSTATUS_SUB_WORK* PSTATUS_SUB_Init( PSTATUS_WORK *work )
{
  PSTATUS_SUB_WORK* subWork;
  
  subWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_SUB_WORK) );
  subWork->pokeMcss = NULL;
  subWork->pokeMcssBack = NULL;
  subWork->isUpdateStr = FALSE;

  subWork->isDispFront = TRUE;

  return subWork;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
void PSTATUS_SUB_Term( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //PSTATUS_SUB_PokeDeleteMcss( work,subWork );
  GFL_HEAP_FreeMemory( subWork );
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
void PSTATUS_SUB_Main( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  /*
  if( subWork->isUpdateStr == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper )) == FALSE && 
        PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinDown )) == FALSE )
    {
      subWork->isUpdateStr = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinUpper );
      GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinDown );
    }
  }
  */
  {
    //当たり判定作成
    GFL_UI_TP_HITTBL hitTbl[2] =
    {
      { PSTATUS_SUB_TOUCH_TOP , PSTATUS_SUB_TOUCH_BOTTOM , PSTATUS_SUB_TOUCH_LEFT , PSTATUS_SUB_TOUCH_RIGHT },
      { GFL_UI_TP_HIT_END ,0,0,0 },
    };
    
    const int ret = GFL_UI_TP_HitTrg( hitTbl );
    
    if( ret == 0 )
    {
      subWork->isDispFront = !subWork->isDispFront;

      if( subWork->isDispFront == TRUE )
      {
        MCSS_ResetVanishFlag( subWork->pokeMcss );
        MCSS_SetVanishFlag( subWork->pokeMcssBack );
      }
      else
      {
        MCSS_SetVanishFlag( subWork->pokeMcss );
        MCSS_ResetVanishFlag( subWork->pokeMcssBack );
      }
    }
  }
}

//--------------------------------------------------------------
//	描画
//--------------------------------------------------------------
void PSTATUS_SUB_Draw( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //未使用
}

#pragma mark [>Resource
//--------------------------------------------------------------
//	リソース読み込み
//--------------------------------------------------------------
void PSTATUS_SUB_LoadResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , ARCHANDLE *archandle )
{
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_bg_d_NSCR , 
                    PSTATUS_BG_PLATE ,  0 , 0, FALSE , work->heapId );
  
}

//--------------------------------------------------------------
//	リソース開放
//--------------------------------------------------------------
void PSTATUS_SUB_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //ここで描画物の開放もしておく
  PSTATUS_SUB_ClearPage( work , subWork );

}

#pragma mark [>Cell
//--------------------------------------------------------------
//	セル初期化
//--------------------------------------------------------------
void PSTATUS_SUB_InitCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //選択カーソル
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PSTATUS_SUB_CELL_BALL_X;
    cellInitData.pos_y = PSTATUS_SUB_CELL_BALL_Y;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    cellInitData.anmseq = 0;
    
    subWork->clwkBall = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_BALL],
              work->cellRes[SCR_PLT_BALL],
              work->cellRes[SCR_ANM_BALL],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , FALSE );
  }
  {
    static const u8 markPosX[SSMT_MAX] = 
    { 208 , 216 , 224 , 232 , 240 , 248 , 184 , 176 };
    static const u8 markPosY = 128;
    static const u8 markAnmSeq[SSMT_MAX] = 
    { 0 , 2 , 4 , 6 , 8 , 10 , 12 , 13 };
    u8 i;
    //マーク
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_y = markPosY;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 0;
    
    for( i=0;i<SSMT_MAX;i++ )
    {
      cellInitData.pos_x = markPosX[i];
      cellInitData.anmseq = markAnmSeq[i];
      subWork->clwkMark[i] = GFL_CLACT_WK_Create( work->cellUnit ,
                work->cellRes[SCR_NCG_MARK],
                work->cellRes[SCR_PLT_MARK],
                work->cellRes[SCR_ANM_MARK],
                &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
      GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[i] , TRUE );
    }
    
  }
}

//--------------------------------------------------------------
//	セル開放
//--------------------------------------------------------------
void PSTATUS_SUB_TermCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  u8 i;
  for( i=0;i<SSMT_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( subWork->clwkMark[i] );
  }
  GFL_CLACT_WK_Remove( subWork->clwkBall );
}

#pragma mark [>Disp
//--------------------------------------------------------------
//	ページの表示
//--------------------------------------------------------------
void PSTATUS_SUB_DispPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
//  PSTATUS_SUB_PokeCreateMcss( work , subWork , ppp );

  //BmpWin
  subWork->bmpWinUpper = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
              PSTATUS_SUB_UPWIN_LEFT , PSTATUS_SUB_UPWIN_TOP ,
              PSTATUS_SUB_UPWIN_WIDTH , PSTATUS_SUB_UPWIN_HEIGHT ,
              PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  subWork->bmpWinDown  = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
              PSTATUS_SUB_DOWNWIN_LEFT , PSTATUS_SUB_DOWNWIN_TOP ,
              PSTATUS_SUB_DOWNWIN_WIDTH , PSTATUS_SUB_DOWNWIN_HEIGHT ,
              PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  PSTATUS_SUB_DrawStr( work , subWork , ppp );
}
//--------------------------------------------------------------
//	ページの表示
//--------------------------------------------------------------
void PSTATUS_SUB_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  PSTATUS_SUB_PokeCreateMcss( work , subWork , ppp );
  GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinUpper );
  GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinDown );
  
  //ボールの更新
  {
    u32 ballIdx = PPP_Get( ppp , ID_PARA_get_ball , NULL );
    if( ballIdx != 0 )
    {
      ballIdx--;
    }
    {
      NNSG2dPaletteData *pltData;
      void *pltRes = GFL_ARC_UTIL_LoadPalette( ARCID_P_STATUS , 
                          NARC_p_status_gra_ball00_NCLR + ballIdx ,
                          &pltData , work->heapId );
      GFL_CLGRP_PLTT_Replace( work->cellRes[SCR_PLT_BALL] , pltData , 1 );
      GFL_HEAP_FreeMemory( pltRes );
    }
    {
      NNSG2dCharacterData *ncgData;
      void *ncgRes = GFL_ARC_UTIL_LoadBGCharacter( ARCID_P_STATUS , 
                          NARC_p_status_gra_ball00_NCGR + ballIdx ,
                          FALSE , &ncgData , work->heapId );
      GFL_CLGRP_CGR_Replace( work->cellRes[SCR_NCG_BALL] , ncgData );
      GFL_HEAP_FreeMemory( ncgRes );
    }
    
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , TRUE );
  }
  //マークの更新
  {
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[SSMT_MARK_RARE] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[SSMT_MARK_POKERUS] , FALSE );
    
  }
  subWork->isDispFront = TRUE;

}

//--------------------------------------------------------------
//	ページのクリア
//--------------------------------------------------------------
void PSTATUS_SUB_ClearPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{

  //文字
  GFL_BMPWIN_Delete( subWork->bmpWinDown );
  GFL_BMPWIN_Delete( subWork->bmpWinUpper );

  //MCSS
//  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
}
//--------------------------------------------------------------
//	ページのクリア
//--------------------------------------------------------------
void PSTATUS_SUB_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
  GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , FALSE );
}

//--------------------------------------------------------------
//	文字の描画
//--------------------------------------------------------------
void PSTATUS_SUB_DrawStr( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp )
{
  //名前
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 32, work->heapId );
    STRBUF *nameStr = GFL_STR_CreateBuffer( 32, work->heapId );
    PPP_Get( ppp, ID_PARA_nickname, nameStr );

    WORDSET_RegisterWord( wordSet , 0 , nameStr , 0,TRUE,PM_LANG );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_01 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );

    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
            PSTATUS_SUB_STR_NAME_X , PSTATUS_SUB_STR_NAME_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK);

    GFL_STR_DeleteBuffer( nameStr );
    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  
  //性別
  {
    STRBUF *srcStr;
    u32 sex = PPP_Get( ppp , ID_PARA_sex , NULL );
    if( sex == PTL_SEX_MALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_02 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
              PSTATUS_SUB_STR_SEX_X , PSTATUS_SUB_STR_SEX_Y ,
              srcStr , work->fontHandle , PSTATUS_STR_COL_BLUE );
      GFL_STR_DeleteBuffer( srcStr );
    }
    else if( sex == PTL_SEX_FEMALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_03 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
              PSTATUS_SUB_STR_SEX_X , PSTATUS_SUB_STR_SEX_Y ,
              srcStr , work->fontHandle , PSTATUS_STR_COL_RED );
      GFL_STR_DeleteBuffer( srcStr );
    }
  }

  //レベル
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 lv = PPP_CalcLevel( ppp );
    WORDSET_RegisterNumber( wordSet , 0 , lv , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_04 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
            PSTATUS_SUB_STR_LV_X , PSTATUS_SUB_STR_LV_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }

  //"もちもの"
  {
    STRBUF *srcStr;
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_05 ); 
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinDown ) , 
            PSTATUS_SUB_STR_ITEM_X , PSTATUS_SUB_STR_ITEM_Y ,
            srcStr , work->fontHandle , PSTATUS_STR_COL_BLACK );
    GFL_STR_DeleteBuffer( srcStr );
  }

  //所持アイテム名
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 itemId = PPP_Get( ppp , ID_PARA_item , NULL );
    WORDSET_RegisterItemName( wordSet , 0 , itemId );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_06 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinDown ) , 
            PSTATUS_SUB_STR_ITEMNAME_X , PSTATUS_SUB_STR_ITEMNAME_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }


  subWork->isUpdateStr = TRUE;
  
}

#pragma mark [>Poke
//--------------------------------------------------------------
//	ポケモンMCSS作成
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  VecFx32 shadowScale = {PSTATUS_SUB_SHADOW_SCALE_X , PSTATUS_SUB_SHADOW_SCALE_Y , PSTATUS_SUB_SHADOW_SCALE_Z};
  VecFx32 shadowOffset= {PSTATUS_SUB_SHADOW_OFFSET_X , PSTATUS_SUB_SHADOW_OFFSET_Y , PSTATUS_SUB_SHADOW_OFFSET_Z};
  
  work->shadowRotate = 302*65536/360;
  
  GF_ASSERT( subWork->pokeMcss == NULL );
  {
    MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_FRONT );
    subWork->pokeMcss = MCSS_Add( work->mcssSys , PSTATUS_MCSS_POS_X , PSTATUS_MCSS_POS_Y ,0 , &addWork );
    MCSS_SetScale( subWork->pokeMcss , &scale );
    MCSS_SetShadowAlpha( subWork->pokeMcss , 2 );
  #if USE_DEBUGWIN_SYSTEM
    MCSS_SetShadowScale( subWork->pokeMcss , &work->shadowScale );
    MCSS_SetShadowRotate( subWork->pokeMcss , work->shadowRotate );
    MCSS_SetShadowOffset( subWork->pokeMcss , &work->shadowOfs );
  #else
    MCSS_SetShadowRotate( subWork->pokeMcss , PSTATUS_SUB_SHADOW_ROTATE );
    MCSS_SetShadowOffset( subWork->pokeMcss , &shadowOffset );
    MCSS_SetShadowScale( subWork->pokeMcss , &shadowScale );
  #endif
  }
  {
    MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_BACK );
    subWork->pokeMcssBack = MCSS_Add( work->mcssSys , PSTATUS_MCSS_POS_X , PSTATUS_MCSS_POS_Y ,0 , &addWork );
    MCSS_SetScale( subWork->pokeMcssBack , &scale );
    MCSS_SetShadowAlpha( subWork->pokeMcssBack , 2 );
  #if USE_DEBUGWIN_SYSTEM
    MCSS_SetShadowScale( subWork->pokeMcssBack , &work->shadowScale );
    MCSS_SetShadowRotate( subWork->pokeMcssBack , work->shadowRotate );
    MCSS_SetShadowOffset( subWork->pokeMcssBack , &work->shadowOfs );
  #else
    MCSS_SetShadowRotate( subWork->pokeMcss , PSTATUS_SUB_SHADOW_ROTATE );
    MCSS_SetShadowOffset( subWork->pokeMcss , &shadowOffset );
    MCSS_SetShadowScale( subWork->pokeMcss , &shadowScale );
  #endif
  }
  MCSS_ResetVanishFlag( subWork->pokeMcss );
  MCSS_SetVanishFlag( subWork->pokeMcssBack );
}

//--------------------------------------------------------------
//	ポケモンMCSS削除
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  GF_ASSERT( subWork->pokeMcss != NULL );
  
  MCSS_SetVanishFlag( subWork->pokeMcss );
  MCSS_SetVanishFlag( subWork->pokeMcssBack );
  MCSS_Del(work->mcssSys,subWork->pokeMcss);
  MCSS_Del(work->mcssSys,subWork->pokeMcssBack);
  subWork->pokeMcss = NULL;
  subWork->pokeMcssBack = NULL;
}

//デバグ用
void PSTATUS_SUB_SetShadowScale( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  MCSS_SetShadowScale( subWork->pokeMcss , &work->shadowScale );
  MCSS_SetShadowRotate( subWork->pokeMcss , work->shadowRotate );
  MCSS_SetShadowOffset( subWork->pokeMcss , &work->shadowOfs );
  MCSS_SetShadowScale( subWork->pokeMcssBack , &work->shadowScale );
  MCSS_SetShadowRotate( subWork->pokeMcssBack , work->shadowRotate );
  MCSS_SetShadowOffset( subWork->pokeMcssBack , &work->shadowOfs );
}
