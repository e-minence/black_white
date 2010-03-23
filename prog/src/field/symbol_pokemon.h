//==============================================================================
/**
 * @file    symbol_pokemon.h
 * @brief   �V���{���|�P�����z�u����
 * @author  tamada
 * @date    2010.03.21
 *
 * �~�j���m���X�p��ύX���č쐬�J�n
 */
//==============================================================================
#pragma once

#include <gflib.h>
#include "savedata/symbol_save.h"
#include "fieldmap.h"

#include "poke_tool/poke_tool.h"

//==============================================================================
//==============================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
extern void SYMBOLPOKE_Add(FIELDMAP_WORK *fieldmap, const SYMBOL_POKEMON * sympoke, int poke_num );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern POKEMON_PARAM * SYMBOLPOKE_PP_CreateByObjID( HEAPID heapID, GAMESYS_WORK * gsys, u16 obj_id );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern u16 SYMBOLPOKE_GetMonsno( FIELDMAP_WORK * fieldmap, u16 obj_id );
extern u16 SYMBOLPOKE_GetFormno( FIELDMAP_WORK * fieldmap, u16 obj_id );
extern u16 SYMBOLPOKE_GetSex( FIELDMAP_WORK * fieldmap, u16 obj_id );
extern u16 SYMBOLPOKE_GetWaza( FIELDMAP_WORK * fieldmap, u16 obj_id );

