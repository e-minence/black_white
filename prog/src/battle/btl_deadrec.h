//=============================================================================================
/**
 * @file  btl_deadrec.h
 * @brief �|�P����WB ���S�|�P�L�^
 * @author  taya
 *
 * @date  2009.11.23  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_common.h"

enum {
  BTL_DEADREC_TURN_MAX = 2,   ///< �L�^�^�[����
};

/**
 *  �|�P���S���R�[�h
 */
typedef struct {
  struct {
    u8 cnt;
    u8 pokeID[ BTL_POKEID_MAX ];
  }record[BTL_DEADREC_TURN_MAX];
}BTL_DEADREC;


extern void BTL_DEADREC_Clear( BTL_DEADREC* wk );
extern void BTL_DEADREC_Start( BTL_DEADREC* wk );

extern void BTL_DEADREC_GetCount( const BTL_DEADREC* wk, u8 turn );
extern void BTL_DEADREC_GetPokeID( const BTL_DEADREC* wk, u8 turn, u8 idx );
