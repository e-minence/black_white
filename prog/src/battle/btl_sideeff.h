//=============================================================================================
/**
 * @file  btl_sideeff.h
 * @brief �|�P����WB �o�g�� -�V�X�e��-  �T�C�h�G�t�F�N�g��ޓ�
 * @author  taya
 *
 * @date  2008.07.02  �쐬
 */
//=============================================================================================
#pragma once

/**
 *  �T�C�h�G�t�F�N�g���
 */
typedef enum {

  BTL_SIDEEFF_START=0,

  BTL_SIDEEFF_REFRECTOR=BTL_SIDEEFF_START,      ///< �����U���𔼌�
  BTL_SIDEEFF_HIKARINOKABE,   ///< ����U���𔼌�
  BTL_SIDEEFF_SINPINOMAMORI,  ///< �|�P�n��Ԉُ�ɂȂ�Ȃ�
  BTL_SIDEEFF_SIROIKIRI,      ///< �����N�_�E�����ʂ��󂯂Ȃ�
  BTL_SIDEEFF_OIKAZE,         ///< ���΂₳�Q�{
  BTL_SIDEEFF_OMAJINAI,       ///< �U�����}���ɓ�����Ȃ�
  BTL_SIDEEFF_MAKIBISI,       ///< ����ւ��ďo�Ă����|�P�����Ƀ_���[�W�i�R�i�K�j
  BTL_SIDEEFF_DOKUBISI,       ///< ����ւ��ďo�Ă����|�P�����ɓŁi�Q�i�K�j
  BTL_SIDEEFF_STEALTHROCK,    ///< ����ւ��ďo�Ă����|�P�����Ƀ_���[�W�i�����v�Z����j
  BTL_SIDEEFF_WIDEGUARD,      ///< �q�b�g�͈́u�G�S�́v�u�����ȊO�v�̍U�����U��h��
  BTL_SIDEEFF_FASTGUARD,      ///< �搧�v���C�I���e�B1�ȏ�̃|�P�����Ώۃ��U��h��
  BTL_SIDEEFF_RAINBOW,        ///< ���̃��U���ʁE���i�ǉ����ʂ̔��������{�ɂȂ�j
  BTL_SIDEEFF_BURNING,        ///< ���̃��U���ʁE�΂̊C�i���^�C�v�ȊO�ɖ��^�[��1/8�_���[�W�j
  BTL_SIDEEFF_MOOR,           ///< ���̃��U���ʁE�����i�f���� 1/4�j

  BTL_SIDEEFF_MAX,
  BTL_SIDEEFF_NULL = BTL_SIDEEFF_MAX,

  BTL_SIDEEFF_BITFLG_BYTES = 1 + (BTL_SIDEEFF_MAX/8) + ((BTL_SIDEEFF_MAX%8)!=0),

}BtlSideEffect;



