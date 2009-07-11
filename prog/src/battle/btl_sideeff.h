//=============================================================================================
/**
 * @file  btl_sideeff.h
 * @brief ポケモンWB バトル -システム-  サイドエフェクト種類等
 * @author  taya
 *
 * @date  2008.07.02  作成
 */
//=============================================================================================
#pragma once

/**
 *  サイドエフェクト種類
 */
typedef enum {

  BTL_SIDEEFF_START=0,

  BTL_SIDEEFF_REFRECTOR=BTL_SIDEEFF_START,      ///< 物理攻撃を半減
  BTL_SIDEEFF_HIKARINOKABE,   ///< 特殊攻撃を半減
  BTL_SIDEEFF_SINPINOMAMORI,  ///< ポケ系状態異常にならない
  BTL_SIDEEFF_SIROIKIRI,      ///< ランクダウン効果を受けない
  BTL_SIDEEFF_OIKAZE,         ///< すばやさ２倍
  BTL_SIDEEFF_OMAJINAI,       ///< 攻撃が急所に当たらない
  BTL_SIDEEFF_MAKIBISI,       ///< 入れ替えて出てきたポケモンにダメージ（３段階）
  BTL_SIDEEFF_DOKUBISI,       ///< 入れ替えて出てきたポケモンに毒（２段階）
  BTL_SIDEEFF_STEALTHROCK,    ///< 入れ替えて出てきたポケモンにダメージ（相性計算あり）

  BTL_SIDEEFF_MAX,
  BTL_SIDEEFF_NULL = BTL_SIDEEFF_MAX,

  BTL_SIDEEFF_BITFLG_BYTES = 1 + (BTL_SIDEEFF_MAX/8) + ((BTL_SIDEEFF_MAX%8)!=0),

}BtlSideEffect;


typedef enum {

  BTL_POSEFF_NEGAIGOTO,
  BTL_POSEFF_MIKADUKINOMAI,
  BTL_POSEFF_SIZENNOMEGUMI,

  BTL_POSEFF_MAX,
  BTL_POSEFF_NULL = BTL_POSEFF_MAX,

}BtlPosEffect;

