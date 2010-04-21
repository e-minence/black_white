//=============================================================================================
/**
 * @file  hand_pos.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�ʒu�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.07.11  �쐬
 */
//=============================================================================================
#pragma once

#include "..\btl_pokeparam.h"
#include "..\btl_server_flow.h"



extern BTL_EVENT_FACTOR*  BTL_HANDLER_POS_Add( BtlPosEffect effect, BtlPokePos pos, u8 pokeID,
  const int* param, u8 param_cnt );


extern BOOL BTL_HANDLER_POS_IsRegistered( BtlPosEffect eff, BtlPokePos pos );
