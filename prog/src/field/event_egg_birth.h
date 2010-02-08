///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �^�}�S�z���C�x���g
 * @file   event_egg_birth.h
 * @author obata
 * @date   2010.02.05
 */
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "field/fieldmap_proc.h"
#include "poke_tool/poke_tool.h"

//-------------------------------------------------------------------------------------
/**
 * @brief �^�}�S�z���C�x���g ����
 *
 * @param gameSystem
 * @param fieldmap
 * @param egg        �z��������^�}�S
 */
//-------------------------------------------------------------------------------------
extern GMEVENT* EVENT_EggBirth( 
    GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap, POKEMON_PARAM* egg ); 
