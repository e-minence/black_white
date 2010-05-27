//=============================================================================================
/**
 * @file  hand_common.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ ���ʃw�b�_
 * @author  taya
 *
 * @date  2009.12.02  �쐬
 */
//=============================================================================================
#pragma once

#include "../btl_common.h"

typedef struct {
  u8  pokeCnt;
  u8  pokeID[ BTL_POS_MAX ];
}HANDWORK_POKEID;


/**
 *  ���L�c�[���֐��Q
 */
extern BOOL HandCommon_CheckTargetPokeID( u8 pokeID );
extern BOOL HandCommon_IsPokeOrderLast( BTL_SVFLOW_WORK* flowWk, u8 pokeID );
extern BOOL HandCommon_CheckCantStealPoke( BTL_SVFLOW_WORK* flowWk, u8 pokeID );
extern void HandCommon_MagicCoat_Wait( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
extern void HandCommon_MagicCoat_Reaction( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


