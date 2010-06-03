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
  BTL_DEADREC_TURN_MAX = 4,   ///< �L�^�^�[����
};

/**
 *  �|�P���S���R�[�h�i�^�[���P�ʁj
 */

typedef struct {
  u8 cnt;
  u8 fExpChecked[ BTL_POKEID_MAX ];    ///< �o���l�`�F�b�N�����ς݃t���O�i�|�P����ID���j
  u8 deadPokeID[ BTL_POKEID_MAX ];
}BTL_DEADREC_UNIT;

/**
 *  �|�P���S���R�[�h�i�{�́j
 */
typedef struct {


  BTL_DEADREC_UNIT  record[ BTL_DEADREC_TURN_MAX ];


}BTL_DEADREC;


extern void BTL_DEADREC_Init( BTL_DEADREC* wk );
extern void BTL_DEADREC_StartTurn( BTL_DEADREC* wk );
extern void BTL_DEADREC_Add( BTL_DEADREC* wk, u8 pokeID );

extern u8 BTL_DEADREC_GetCount( const BTL_DEADREC* wk, u8 turn );
extern u8 BTL_DEADREC_GetPokeID( const BTL_DEADREC* wk, u8 turn, u8 idx );
extern BOOL BTL_DEADREC_GetExpCheckedFlag( const BTL_DEADREC* wk, u8 turn, u8 idx );
extern void BTL_DEADREC_SetExpCheckedFlag( BTL_DEADREC* wk, u8 turn, u8 idx );
extern u8 BTL_DEADREC_GetLastDeadPokeID( const BTL_DEADREC* wk );

