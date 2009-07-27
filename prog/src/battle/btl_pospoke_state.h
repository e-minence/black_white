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

  POSPOKE_EMPTY = 0, ///< �Ǘ��O�i�V���O����łQ�̖ڂ̈ʒu�Ȃǂ͊Ǘ��O�ɂȂ�j
  POSPOKE_EXIST,           ///< �����Ă���|�P���������݂���
  POSPOKE_DEAD,            ///<

}BtlPosPokeState;


/**
 * ��ԕێ��\����
 */
typedef struct {
  BtlPosPokeState  state[ BTL_POS_MAX ];
}BTL_POSPOKE_WORK;

/**
 * ��Ԕ�r���ʍ\����
 */
typedef struct {
  u32          count;
  BtlPokePos   pos[ BTL_POS_MAX ];
}BTL_POSPOKE_COMPAIR_RESULT;


extern void BTL_POSPOKE_InitWork( BTL_POSPOKE_WORK* wk, const BTL_POKE_CONTAINER* pokeCon, BtlRule rule );
extern void BTL_POSPOKE_Update( BTL_POSPOKE_WORK* wk, const BTL_POKE_CONTAINER* pokeCon );
extern void BTL_POSPOKE_Compair( const BTL_POSPOKE_WORK* wk_before, const BTL_POSPOKE_WORK* wk_after, BTL_POSPOKE_COMPAIR_RESULT* result );
extern void BTL_POSPOKE_CopyWork( const BTL_POSPOKE_WORK* src, BTL_POSPOKE_WORK* dst );


