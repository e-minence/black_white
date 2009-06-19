//=============================================================================================
/**
 * @file  hand_side.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g�t�@�N�^�[ [�T�C�h�G�t�F�N�g]
 * @author  taya
 *
 * @date  2009.06.18  �쐬
 */
//=============================================================================================
#pragma once

#include "..\btl_pokeparam.h"

/**
 *  �T�C�h�G�t�F�N�g���
 */
typedef enum {

  BTL_SIDEEFF_NULL = 0,
  BTL_SIDEEFF_REFRECTOR,      ///< �����U���𔼌�
  BTL_SIDEEFF_HIKARINOKABE,   ///< ����U���𔼌�
  BTL_SIDEEFF_SINPINOMAMORI,  ///< �|�P�n��Ԉُ�ɂȂ�Ȃ�
  BTL_SIDEEFF_SIROIKIRI,      ///< �����N�_�E�����ʂ��󂯂Ȃ�
  BTL_SIDEEFF_MAKIBISI,       ///< ����ւ��ďo�Ă����|�P�����Ƀ_���[�W
  BTL_SIDEEFF_DOKUBISI,       ///< ����ւ��ďo�Ă����|�P�����ɓ�
  BTL_SIDEEFF_MIZUASOBI,      ///< �����U�З͌�
  BTL_SIDEEFF_DOROASOBI,      ///< �d�C���U�З͌�


  BTL_SIDEEFF_MAX,

}BtlSideEffect;



extern void BTL_HANDLER_SIDE_InitSystem( void );
extern BTL_EVENT_FACTOR*  BTL_HANDLER_SIDE_Add( const BTL_POKEPARAM* pp, BtlSideEffect sideEffect, BPP_SICK_CONT contParam );


