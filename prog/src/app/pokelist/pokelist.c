//======================================================================
/**
 * @file	pokelist.c
 * @brief	�|�P�������X�g
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPOKE_LIST
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "plist_local_def.h"
#include "plist_sys.h"

//�f�o�b�O�|�P�p�[�e�B�쐬�p
#include  "poke_tool/pokeparty.h"
#include  "poke_tool/poke_tool.h"

//�f�o�b�O���[�^����
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
//	������
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
    //�f�o�O
    if( pwk == NULL )
    {
      u8 i;
      plData = GFL_HEAP_AllocClearMemory( HEAPID_POKELIST , sizeof(PLIST_DATA) );
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
          PP_Put( pPara , ID_PARA_pp1 , 3 );
          PP_Put( pPara , ID_PARA_pp2 , 3 );
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
          PP_Put( pPara , ID_PARA_tamago_flag , 1 );
          break;
        }
  #endif      
        {
          u16 oyaName[5] = {L'�u',L'��',L'�b',L'�N',0xFFFF};
          PP_Put( pPara , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
          PP_Put( pPara , ID_PARA_oyasex , PTL_SEX_MALE );
        }
        PokeParty_Add( plData->pp , pPara );
        GFL_HEAP_FreeMemory( pPara );
      }
      
      plData->mode = PL_MODE_FIELD;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
      {
        u8 i;
        for( i=0;i<6;i++ )
        {
          plData->in_num[i] = 0;
        }
        plData->mode = PL_MODE_BATTLE;
        plData->in_min = 2;
        plData->in_max = 4;
        plData->in_lv = 100;
      }
      else
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
      {
        plData->mode = PL_MODE_ITEMUSE;
        //plData->item = 17; //����
        //plData->item = 26; //E����
        //plData->item = 28; //���C�̂�����
        //plData->item = 18; //�ŏ���
        //plData->item = 47; //�u�����w�L�V��
        //plData->item = 38; //PP�G�C�h
        //plData->item = 41; //PP�}�b�N�X
        //plData->item = 51; //P�A�b�v
        
        //plData->item = 44; //���Ȃ�D
        plData->item = 174; //�}�g�}�̎�
      }
      else
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y )
      {
        plData->mode = PL_MODE_SODATEYA;
        /*
        plData->mode = PL_MODE_WAZASET;
        plData->item = 328; //�Z�}�V���C���p���`
        plData->waza = 0;
        */
      }
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
//	�J��
//--------------------------------------------------------------
static GFL_PROC_RESULT PokeListProc_Term( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  PLIST_WORK *plWork = mywk;

  if( PLIST_TermPokeList( plWork ) == FALSE )
  {
    return GFL_PROC_RES_CONTINUE;
  }

  //�f�o�O
  if( pwk == NULL )
  {
    u8 i;
    for( i=0;i<6;i++ )
    {
      ARI_TPrintf("%d[%d]\n",i,plWork->plData->in_num[i]);
    }
    GFL_HEAP_FreeMemory( plWork->plData->pp );
    GFL_HEAP_FreeMemory( plWork->plData );
  }

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_POKELIST );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
static GFL_PROC_RESULT PokeListProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  PLIST_WORK *plWork = mywk;
  
  if( PLIST_UpdatePokeList( plWork ) == TRUE )
  {
      return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


