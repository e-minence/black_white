//=============================================================================================
/**
 * @file  btl_server_flow_sub.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h���������i�T�u���[�`���Q�j
 * @author  taya
 *
 * @date  2010.05.28  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_server_flow_local.h"

typedef enum {

  TRITEM_RESULT_NORMAL = 0, ///< �ʏ퓹����ʔ���
  TRITEM_RESULT_ESCAPE,     ///< �����I�ɓ�����

}TrItemResult;

extern u8 BTL_SVFSUB_RegisterTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* rec );


/**
 *  ���x���A�b�v�v�Z
 */
extern void BTL_SVFSUB_CalcExp( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result );

/**
 *  �g���[�i�[�A�C�e���g�p�������[�g
 */
extern TrItemResult  BTL_SVFSUB_TrainerItemProc( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx );


/**
 *  ���s����
 */
extern BtlResult BTL_SVFSUB_CheckBattleResult( BTL_SVFLOW_WORK* wk );

