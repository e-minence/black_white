//============================================================================================
/**
 * @file  pdc.c
 * @brief �|�P�����h���[���L���b�`�i�p���X�ł̕ߊl�Q�[���j
 * @author  soga
 * @date  2010.03.12
 */
//============================================================================================
#include <gflib.h>
#include <procsys.h>

#include "system/main.h"

#include "app/pdc.h"
#include "pdc_main.h"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================
#define POKEMON_DREAM_CATCH_HEAP_SIZE ( 0xc0000 )

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct _PDC_SETUP_PARAM
{ 
  GAMEDATA*             gameData;
  POKEMON_PARAM*        pp;
  BTL_FIELD_SITUATION*  bfs;
  MYSTATUS*             my_status;
  MYITEM*               my_item;
  CONFIG*               config;
  ZUKAN_SAVEDATA*       zs;
  PDC_RESULT            result;
};

typedef struct
{ 
  PDC_MAIN_WORK*    pdc_main;
  PDC_SETUP_PARAM*  psp;

  HEAPID            heapID;
}PDC_WORK;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static GFL_PROC_RESULT PDC_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT PDC_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT PDC_ProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

FS_EXTERN_OVERLAY(battle_view);

//--------------------------------------------------------------------------
/**
 * @brief PDC_SETUP_PARAM����
 *
 * @param[in] gameData  GAMEDATA�\����
 * @param[in] pp        POKEMON_PARAM�\����
 * @param[in] bfs       BTL_FIELD_SITUATION�\����
 * @param[in] my_status �v���C���[�f�[�^
 * @param[in] my_item   �����A�C�e���f�[�^
 * @param[in] config    �R���t�B�O
 * @param[in] zs        �}�ӃZ�[�u�f�[�^
 * @param[in] heapID    �q�[�vID
 *
 * @retval  PDC_SETUP_PARAM
 */
//--------------------------------------------------------------------------
PDC_SETUP_PARAM*  PDC_MakeSetUpParam( GAMEDATA* gameData, POKEMON_PARAM* pp, BTL_FIELD_SITUATION* bfs, MYSTATUS* my_status,
                                      MYITEM* my_item, CONFIG* config, ZUKAN_SAVEDATA* zs, HEAPID heapID )
{ 
  PDC_SETUP_PARAM*  psp = GFL_HEAP_AllocMemory( heapID, sizeof( PDC_SETUP_PARAM ) );

  psp->gameData   = gameData;
  psp->pp         = pp;
  psp->bfs        = bfs;
  psp->my_status  = my_status;
  psp->my_item    = my_item;
  psp->config     = config;
  psp->zs         = zs;
  psp->result     = PDC_RESULT_NONE;

  return psp;
}

//--------------------------------------------------------------------------
/**
 * @brief gameData�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  GAMEDATA
 */
//--------------------------------------------------------------------------
GAMEDATA*  PDC_GetGameData( PDC_SETUP_PARAM* psp )
{ 
  return psp->gameData;
}

//--------------------------------------------------------------------------
/**
 * @brief pp�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  POKEMON_PARAM
 */
//--------------------------------------------------------------------------
POKEMON_PARAM*  PDC_GetPP( PDC_SETUP_PARAM* psp )
{ 
  return psp->pp;
}

//--------------------------------------------------------------------------
/**
 * @brief bfs�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  bfs
 */
//--------------------------------------------------------------------------
BTL_FIELD_SITUATION*  PDC_GetBFS( PDC_SETUP_PARAM* psp )
{ 
  return psp->bfs;
}

//--------------------------------------------------------------------------
/**
 * @brief my_status�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  my_status
 */
//--------------------------------------------------------------------------
MYSTATUS*  PDC_GetMyStatus( PDC_SETUP_PARAM* psp )
{ 
  return psp->my_status;
}

//--------------------------------------------------------------------------
/**
 * @brief my_item�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  my_item
 */
//--------------------------------------------------------------------------
MYITEM*  PDC_GetMyItem( PDC_SETUP_PARAM* psp )
{ 
  return psp->my_item;
}

//--------------------------------------------------------------------------
/**
 * @brief config�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  config
 */
//--------------------------------------------------------------------------
CONFIG*  PDC_GetConfig( PDC_SETUP_PARAM* psp )
{ 
  return psp->config;
}

//--------------------------------------------------------------------------
/**
 * @brief zs�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  zs
 */
//--------------------------------------------------------------------------
ZUKAN_SAVEDATA*  PDC_GetZukanWork( PDC_SETUP_PARAM* psp )
{ 
  return psp->zs;
}

//--------------------------------------------------------------------------
/**
 * @brief PDC_RESULT�擾
 *
 * @param[in] psp PDC_SETUP_PARAM
 *
 * @retval  PDC_RESULT
 */
//--------------------------------------------------------------------------
PDC_RESULT  PDC_GetResult( PDC_SETUP_PARAM* psp )
{ 
  return psp->result;
}

//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PDC_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDC_SETUP_PARAM*  psp = ( PDC_SETUP_PARAM* )pwk;
  PDC_WORK* wk;

  GFL_OVERLAY_Load(FS_OVERLAY_ID(battle_view));

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_POKEMON_DREAM_CATCH, POKEMON_DREAM_CATCH_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof( PDC_WORK ), HEAPID_POKEMON_DREAM_CATCH );
  MI_CpuClearFast( wk, sizeof( PDC_WORK ) );
  wk->heapID = HEAPID_POKEMON_DREAM_CATCH;

  wk->pdc_main = PDC_MAIN_Init( psp, wk->heapID );
  wk->psp = psp;

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PDC_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDC_WORK* wk = ( PDC_WORK* )mywk;

  if( ( wk->psp->result = PDC_MAIN_Main( wk->pdc_main ) ) != PDC_RESULT_NONE )
  { 
    return GFL_PROC_RES_FINISH;
  }
  else
  {
    return GFL_PROC_RES_CONTINUE;
  }
}
//--------------------------------------------------------------------------
/**
 * PROC Exit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT PDC_ProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  PDC_WORK* wk = ( PDC_WORK* )mywk;

  PDC_MAIN_Exit( wk->pdc_main );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_POKEMON_DREAM_CATCH );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_view ) );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA   PDC_ProcData = {
  PDC_ProcInit,
  PDC_ProcMain,
  PDC_ProcExit,
};

