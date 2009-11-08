//=============================================================================
/**
 * @file	  pokemontrade.h
 * @bfief	  �|�P��������
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/07/09
 */
//=============================================================================


#pragma once

/// �|�P���������̃^�C�v
typedef enum{
  POKEMONTRADE_IRC,   ///< �ԊO��
  POKEMONTRADE_UNION, ///< ���j�I��
  POKEMONTRADE_WIFICLUB, ///< WIFI�N���u
  POKEMONTRADE_GTSNEGO, ///< GTS�l�S�V�G�[�V����
  POKEMONTRADE_GTS, ///< GTS
  POKEMONTRADE_TYPE_MAX,
} POKEMONTRADE_TYPE;



typedef struct _POKEMON_TRADE_WORK POKEMON_TRADE_WORK;
typedef struct _POKEMONTRADE_DEMO_WORK POKEMONTRADE_DEMO_WORK;

extern const GFL_PROC_DATA PokemonTradeProcData;
extern const GFL_PROC_DATA PokemonTradeIrcProcData;


