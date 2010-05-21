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

typedef struct {
  u8  fEnable;
  u8  clientID;
  u8  existPokeID;
}BTL_POSPOKE_STATE;

/**
 * ��ԕێ��\����
 */
typedef struct {
  BTL_POSPOKE_STATE  state[ BTL_POS_MAX ];        ///< �ʒu���Ƃ̏��
  BtlPokePos         lastPos[ BTL_POKEID_MAX ];   ///< �|�P�������Ƃ̍ŏI�ʒu�i���񂾂��ւ����肵�����ɂ������Ȃ��j
}BTL_POSPOKE_WORK;

/**
 * ��Ԕ�r���ʍ\����
 */
typedef struct {
  u32          count;
  BtlPokePos   pos[ BTL_POS_MAX ];
}BTL_POSPOKE_COMPAIR_RESULT;


extern void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_MAIN_MODULE* mainModule, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule );

extern void BTL_POSPOKE_PokeIn( BTL_POSPOKE_WORK* wk, BtlPokePos pos,  u8 pokeID, BTL_POKE_CONTAINER* pokeCon );
extern void BTL_POSPOKE_PokeOut( BTL_POSPOKE_WORK* wk, u8 pokeID );
extern void BTL_POSPOKE_Swap( BTL_POSPOKE_WORK* wk, BtlPokePos pos1, BtlPokePos pos2 );
extern u8 BTL_POSPOKE_GetClientEmptyPos( const BTL_POSPOKE_WORK* wk, u8 clientID, u8* pos );
extern BOOL BTL_POSPOKE_IsExist( const BTL_POSPOKE_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_POSPOKE_GetPokeExistPos( const BTL_POSPOKE_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_POSPOKE_GetPokeLastPos( const BTL_POSPOKE_WORK* wk, u8 pokeID );
extern u8 BTL_POSPOKE_GetExistPokeID( const BTL_POSPOKE_WORK* wk, BtlPokePos pos );
extern void BTL_POSPOKE_Rotate( BTL_POSPOKE_WORK* wk, BtlRotateDir dir, u8 clientID, const BTL_POKEPARAM* inPoke, BTL_POKE_CONTAINER* pokeCon );

