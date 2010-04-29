//============================================================================
/**
 *
 *  @file   pdc.h
 *  @brief  �|�P�����h���[���L���b�`
 *  @author soga
 *  @data   2010.03.12
 *
 */
//============================================================================
#pragma once

#include  "battle/battle.h"
#include  "poke_tool/poke_tool.h"
#include  "gamesystem/game_data.h"

FS_EXTERN_OVERLAY(pdc);

typedef enum
{
  PDC_RESULT_NONE = 0,
  PDC_RESULT_CAPTURE,
  PDC_RESULT_ESCAPE,
}PDC_RESULT;

typedef struct  _PDC_SETUP_PARAM  PDC_SETUP_PARAM;

extern  PDC_SETUP_PARAM*      PDC_MakeSetUpParam( GAMEDATA* gameData, POKEMON_PARAM* pp, BTL_FIELD_SITUATION* bfs,
                                                  MYSTATUS* my_status, MYITEM* my_item, CONFIG* config, ZUKAN_SAVEDATA* zs,
                                                  HEAPID heapID );
extern  GAMEDATA*             PDC_GetGameData( PDC_SETUP_PARAM* psp );
extern  POKEMON_PARAM*        PDC_GetPP( PDC_SETUP_PARAM* psp );
extern  BTL_FIELD_SITUATION*  PDC_GetBFS( PDC_SETUP_PARAM* psp );
extern  MYSTATUS*             PDC_GetMyStatus( PDC_SETUP_PARAM* psp );
extern  MYITEM*               PDC_GetMyItem( PDC_SETUP_PARAM* psp );
extern  CONFIG*               PDC_GetConfig( PDC_SETUP_PARAM* psp );
extern  ZUKAN_SAVEDATA*       PDC_GetZukanWork( PDC_SETUP_PARAM* psp );
extern  PDC_RESULT            PDC_GetResult( PDC_SETUP_PARAM* psp );

extern  const GFL_PROC_DATA   PDC_ProcData;
