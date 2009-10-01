//=============================================================================================
/**
 * @file  btl_pospoke_state.h
 * @brief �|�P����WB �o�g���V�X�e��  ��ɂ���|�P�����̐����m�F�p�\���̂Ə������[�`��
 * @author  taya
 *
 * @date  2009.07.27  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_common.h"
#include "btl_main.h"

/**
 * ��ԃR�[�h
 */
typedef enum {

  POSPOKE_EMPTY = 0,    ///< �Ǘ��O�i�V���O����łQ�̖ڂ̈ʒu�Ȃǂ͊Ǘ��O�ɂȂ�j
  POSPOKE_EXIST,        ///< �����Ă���|�P���������݂���
  POSPOKE_DEAD,         ///<

}BtlPosPokeState;


typedef struct {
  u8  fEnable;
  u8  clientID;
  u8  existPokeID;
}BTL_POSPOKE_STATE;

/**
 * ��ԕێ��\����
 */
typedef struct {
//  BtlPosPokeState  state[ BTL_POS_MAX ];
  BTL_POSPOKE_STATE  state[ BTL_POS_MAX ];
}BTL_POSPOKE_WORK;

/**
 * ��Ԕ�r���ʍ\����
 */
typedef struct {
  u32          count;
  BtlPokePos   pos[ BTL_POS_MAX ];
}BTL_POSPOKE_COMPAIR_RESULT;


extern void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule );

extern void BTL_POSPOKE_PokeIn( BTL_POSPOKE_WORK* wk, BtlPokePos pos,  u8 pokeID );
extern void BTL_POSPOKE_PokeOut( BTL_POSPOKE_WORK* wk, u8 pokeID );
extern void BTL_POSPOKE_Swap( BTL_POSPOKE_WORK* wk, BtlPokePos pos1, BtlPokePos pos2 );
extern u8 BTL_POSPOKE_GetClientEmptyPos( const BTL_POSPOKE_WORK* wk, u8 clientID, u8* pos );
extern BOOL BTL_POSPOKE_IsExistPokemon( const BTL_POSPOKE_WORK* wk, u8 pokeID );


