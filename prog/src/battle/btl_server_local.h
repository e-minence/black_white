//=============================================================================================
/**
 * @file  btl_server_local.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�[�������L�萔�Ȃ�
 * @author  taya
 *
 * @date  2009.02.26  �쐬
 */
//=============================================================================================
#ifndef __BTL_SERVER_LOCAL_H__
#define __BTL_SERVER_LOCAL_H__

typedef enum {

  SV_WAZAFAIL_NULL = 0,
  SV_WAZAFAIL_PP_ZERO,
  SV_WAZAFAIL_NEMURI,
  SV_WAZAFAIL_MAHI,
  SV_WAZAFAIL_KOORI,
  SV_WAZAFAIL_KONRAN,
  SV_WAZAFAIL_SHRINK,
  SV_WAZAFAIL_MEROMERO,
  SV_WAZAFAIL_KANASIBARI,
  SV_WAZAFAIL_TYOUHATSU,
  SV_WAZAFAIL_ICHAMON,
  SV_WAZAFAIL_FUUIN,
  SV_WAZAFAIL_KAIHUKUHUUJI,
  SV_WAZAFAIL_HPFULL,
  SV_WAZAFAIL_FUMIN,
  SV_WAZAFAIL_NAMAKE,
  SV_WAZAFAIL_WAZALOCK,
  SV_WAZAFAIL_TOKUSEI,
  SV_WAZAFAIL_JURYOKU,
  SV_WAZAFAIL_TO_RECOVER,
  SV_WAZAFAIL_SABOTAGE,
  SV_WAZAFAIL_SABOTAGE_GO_SLEEP,

  SV_WAZAFAIL_OTHER,

}SV_WazaFailCause;


typedef enum {

  SV_DMGCAUSE_WAZA,         ///< ���U�ɂ�钼�ڃ_���[�W
  SV_DMGCAUSE_WAZA_REACT,   ///< ���U���g�������Ƃɂ�锽���_���[�W
  SV_DMGCAUSE_WAZA_EFFECT,  ///< ���U�ɂ���Ĕ��������e����ʁi��ǂ肬�A�܂��т����j
  SV_DMGCAUSE_CONFUSE,      ///< �����񎩔��ɂ��_���[�W
  SV_DMGCAUSE_POKESICK,     ///< �|�P�����n��Ԉُ�i�ǂ��E�₯�ǁj�ɂ��_���[�W
  SV_DMGCAUSE_WEATHER,      ///< �V��i���Ȃ��炵�A����ꓙ�j�ɂ��_���[�W
  SV_DMGCAUSE_TOKUSEI,      ///< �e��Ƃ������ɂ���Ĉ����N�������_���[�W�i���߂͂��A�䂤�΂����j
  SV_DMGCAUSE_ITEM,         ///< �A�C�e�����ʂɂ���Ĉ����N�������_���[�W�i�ǂ��ǂ����ܓ��j


}SV_DamageCause;

#endif

