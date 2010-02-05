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

//�Ȃ��v�Z
typedef enum{
  CALC_NATSUKI_LEVELUP = 0,       //���x���A�b�v
  CALC_NATSUKI_USE_ITEM,          //�A�C�e���g�p
  CALC_NATSUKI_BOSS_BATTLE,       //�{�X�퓬
  CALC_NATSUKI_TSUREARUKI,        //�A�����
  CALC_NATSUKI_HINSHI,            //�m��
  CALC_NATSUKI_LEVEL30_HINSHI,    //���x����30�ȏ�̕m��
  CALC_NATSUKI_MUSICAL,           //�~���[�W�J��
}CALC_NATSUKI;


//�Ȃ��x�v�Z
void  NATSUKI_Calc( POKEMON_PARAM* pp, CALC_NATSUKI calcID, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcUseItem( POKEMON_PARAM* pp, u16 item_no, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcTsurearuki( POKEMON_PARAM* pp, u16 placeID, HEAPID heapID );
void  NATSUKI_CalcBossBattle( POKEPARTY* ppt, u16 placeID, HEAPID heapID );

