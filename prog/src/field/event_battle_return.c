//============================================================================================
/**
 * @file    event_battle_return.c
 * @brief   �o�g������t�B�[���h�ւ̕��A�Ǘ�
 * @author  taya
 * @date    2009.10.23
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <procsys.h>

// global includes --------------------
#include "system\main.h"
#include "savedata\box_savedata.h"
#include "app\name_input.h"
#include "poke_tool\pokerus.h"

// local includes --------------------
#include "event_battle_return.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  STRBUF_SIZE = 32,   ///< �v���C���[�����ꎞ�I�Ɋi�[���邽�߂̃o�b�t�@�T�C�Y
};

/*--------------------------------------------------------------------------*/
/* Main Work                                                                */
/*--------------------------------------------------------------------------*/
typedef struct {

  NAMEIN_PARAM*     nameinParam;
  STRBUF*           strbuf;
  POKEMON_PARAM*    pp;

  HEAPID  heapID;

}BTLRET_WORK;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT BtlRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BtlRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT BtlRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );


/*--------------------------------------------------------------------------*/
/* Proc Table                                                               */
/*--------------------------------------------------------------------------*/
const GFL_PROC_DATA   BtlRet_ProcData = {
  BtlRet_ProcInit,
  BtlRet_ProcMain,
  BtlRet_ProcQuit,
};


//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK* wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTLRET_SYS, 0x1000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(BTLRET_WORK), HEAPID_BTLRET_SYS );

  wk->strbuf = GFL_STR_CreateBuffer( STRBUF_SIZE, HEAPID_BTLRET_SYS );
  wk->nameinParam = NULL;
  wk->pp = NULL;
  wk->heapID = HEAPID_BTLRET_SYS;

  return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK* wk = mywk;

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_BTLRET_SYS );
  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT BtlRet_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  BTLRET_WORK*  wk = mywk;
  BTLRET_PARAM* param = pwk;

  switch( *seq ){
  case 0:
    {
      POKEPARTY* party    = GAMEDATA_GetMyPokemon( param->gameData );
      MYSTATUS*  myStatus = GAMEDATA_GetMyStatus( param->gameData );

      PokeParty_Copy( param->btlResult->party[ BTL_CLIENT_PLAYER ], party );

      //�|�P���X�����`�F�b�N
      //�ʐM�ΐ�ł͊����`�F�b�N�����Ȃ�
      if( param->btlResult->competitor != BTL_COMPETITOR_COMM )
      {
        //�����`�F�b�N
        POKERUS_CheckCatchPokerus( party );
        //�`���`�F�b�N
        POKERUS_CheckContagion( party );
      }

      // @todo �}�Ӄt���O���Z�b�g����

      // �ߊl����
      if( param->btlResult->result == BTL_RESULT_CAPTURE )
      {
        wk->pp = PokeParty_GetMemberPointer(
                                param->btlResult->party[ BTL_CLIENT_ENEMY1 ], param->btlResult->capturedPokeIdx );

        // �e���Z�b�g���Ă邯�ǁA�{���̓G���J�E���g�O�Ƀt�B�[���h���Őݒ肷�ׂ��H
        MyStatus_CopyNameString( myStatus, wk->strbuf );
        PP_Put( wk->pp, ID_PARA_oyaname, (u32)(wk->strbuf) );

        // @todo ���͕K�����O���͂����Ă��邪�A������m�F��ʂ����B
        GFL_OVERLAY_Load( FS_OVERLAY_ID(namein) );
        wk->nameinParam = NAMEIN_AllocParamPokemonByPP( wk->heapID, wk->pp, NAMEIN_POKEMON_LENGTH, NULL );

        GFL_PROC_SysCallProc( NO_OVERLAY_ID, &NameInputProcData, wk->nameinParam );
        (*seq)++;
      }else{
        (*seq) = 3;
      }
    }
    break;

  case 1:
    if( !NAMEIN_IsCancel(wk->nameinParam) ){
      NAMEIN_CopyStr( wk->nameinParam, wk->strbuf );
      PP_Put( wk->pp, ID_PARA_nickname, (u32)(wk->strbuf) );
    }
    NAMEIN_FreeParam( wk->nameinParam );
    GFL_OVERLAY_Unload( FS_OVERLAY_ID(namein) );
    wk->nameinParam = NULL;
    (*seq)++;
    break;

  case 2:
    {
      POKEPARTY* party    = GAMEDATA_GetMyPokemon( param->gameData );

      if( PokeParty_GetPokeCount(party) < PokeParty_GetPokeCountMax(party) ){
        PokeParty_Add( party, wk->pp );
      }else{
        BOX_MANAGER* boxman = GAMEDATA_GetBoxManager( param->gameData );
        BOXDAT_PutPokemon( boxman, PP_GetPPPPointer(wk->pp) );
      }
    }
    (*seq)++;
    break;

  default:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}

