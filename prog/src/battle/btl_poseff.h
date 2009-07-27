//=============================================================================================
/**
 * @file  btl_poseff.h
 * @brief �|�P����WB �o�g�� -�V�X�e�� - �ʒu�G�t�F�N�g��ޓ� -
 * @author  taya
 *
 * @date  2008.07.21  �쐬
 */
//=============================================================================================
#pragma once

enum {
  BTL_POSEFF_PARAM_MAX = 4,
};

/**
 *  �ʒu�G�t�F�N�g���
 */
typedef enum {

  BTL_POSEFF_NEGAIGOTO,
  BTL_POSEFF_MIKADUKINOMAI,
  BTL_POSEFF_IYASINONEGAI,
  BTL_POSEFF_DELAY_ATTACK,
  BTL_POSEFF_BATONTOUCH,

  BTL_POSEFF_MAX,
  BTL_POSEFF_NULL = BTL_POSEFF_MAX,

}BtlPosEffect;

