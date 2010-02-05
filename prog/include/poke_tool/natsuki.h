//============================================================================================
/**
 * @file    natsuki.h
 * @bfief   �Ȃ��x�v�Z
 * @author  HisashiSogabe
 * @date    10.02.05
 */
//============================================================================================

#pragma once
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

//�Ȃ��x�v�Z
void  NATSUKI_Calc( POKEMON_PARAM* pp, CALC_NATSUKI calcID, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcUseItem( POKEMON_PARAM* pp, u16 item_no, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcTsurearuki( POKEMON_PARAM* pp, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcBossBattle( POKEPARTY* ppt, u16 placeID, HEAPID heapID );

