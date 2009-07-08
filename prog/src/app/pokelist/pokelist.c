//======================================================================
/**
 * @file	pokelist.c
 * @brief	ポケモンリスト
 * @author	ariizumi
 * @data	09/06/10
 *
 * モジュール名：POKE_LIST
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "plist_local_def.h"
#include "plist_sys.h"

//デバッグポケパーティ作成用
#include  "poke_tool/pokeparty.h"
#include  "poke_tool/poke_tool.h"

//デバッグメータ消し
#include "test/performance.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static GFL_PROC_RESULT PokeListProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT PokeListProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
static GFL_PROC_RESULT PokeListProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

FS_EXTERN_OVERLAY(pokelist);

GFL_PROC_DATA PokeList_ProcData =
{
  PokeListProc_Init,
  PokeListProc_Main,
  PokeListProc_Term
};

//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
static GFL_PROC_RESULT PokeListProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  PLIST_DATA *plData = pwk;
  PLIST_WORK *plWork = mywk;;

  switch( *seq )
  {
  case 0:
    
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_POKELIST, 0x30000 );
    
    plWork = GFL_PROC_AllocWork( proc, sizeof(PLIST_WORK), HEAPID_POKELIST );
    plWork->heapId = HEAPID_POKELIST;

    DEBUG_PerformanceSetActive( FALSE );
    //デバグ
    if( pwk == NULL )
    {
      u8 i;
      plData = GFL_HEAP_AllocMemory( HEAPID_POKELIST , sizeof(PLIST_DATA) );
      plData->pp = PokeParty_AllocPartyWork(HEAPID_POKELIST);
      plData->ret_sel = PL_SEL_POS_POKE1;
      PokeParty_Init( plData->pp , 6 );
      for( i=0;i<5;i++ )
      {
        POKEMON_PARAM *pPara = PP_Create( i+1 , 10 , PTL_SETUP_POW_AUTO , HEAPID_POKELIST );
  #if DEB_ARI
        switch( i )
        {
        case 1:
          PP_Put( pPara , ID_PARA_hp , 20 );
          break;
        case 2:
          PP_Put( pPara , ID_PARA_hp , 10 );
          PP_SetSick( pPara , POKESICK_DOKU );
          break;
        case 3:
          PP_Put( pPara , ID_PARA_hp , 0 );
          PP_Put( pPara , ID_PARA_item , 1 );
          break;
        case 4:
          PP_Put( pPara , ID_PARA_hp , 1 );
          PP_Put( pPara , ID_PARA_item , 1 );
          break;
        }
  #endif      
        PokeParty_Add( plData->pp , pPara );
        GFL_HEAP_FreeMemory( pPara );
      }
      
      plData->mode = PL_MODE_FIELD;
      
      GFL_UI_SetTouchOrKey( GFL_APP_KTST_TOUCH );
    }
    
    plWork->plData = plData;
    (*seq) = 1;
    
    //break through
  case 1:
    if( PLIST_InitPokeList( plWork ) == TRUE )
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
static GFL_PROC_RESULT PokeListProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  PLIST_WORK *plWork = mywk;

  if( PLIST_TermPokeList( plWork ) == FALSE )
  {
    return GFL_PROC_RES_CONTINUE;
  }

  //デバグ
  if( pwk == NULL )
  {
    GFL_HEAP_FreeMemory( plWork->plData->pp );
    GFL_HEAP_FreeMemory( plWork->plData );
  }

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_POKELIST );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
static GFL_PROC_RESULT PokeListProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  PLIST_WORK *plWork = mywk;
  
  if( plWork->reqChangeProc == FALSE )
  {
    if( PLIST_UpdatePokeList( plWork ) == TRUE )
    {
        return GFL_PROC_RES_FINISH;
    }
  }
  else
  {
    switch( plWork->changeProcSeq )
    {
    case PSCS_INIT:
      if( PLIST_TermPokeList( plWork ) == TRUE )
      {
        GFL_OVERLAY_Unload( FS_OVERLAY_ID(pokelist) );
        GFL_PROC_SysCallProc( plWork->procOverlayId , plWork->procData , plWork->procParentWork );
        plWork->changeProcSeq = PSCS_MAIN;
      } 
      break;

    case PSCS_MAIN:
      if( plWork->procParentWork != NULL )
      {
        GFL_HEAP_FreeMemory( plWork->procParentWork );
      }
      GFL_OVERLAY_Load( FS_OVERLAY_ID(pokelist) );
      plWork->changeProcSeq = PSCS_TERM;
      break;

    case PSCS_TERM:
      if( PLIST_InitPokeList( plWork ) == TRUE )
      {
        plWork->changeProcSeq = TRUE;
      } 
      break;

    }
  }
    
  return GFL_PROC_RES_CONTINUE;
}


